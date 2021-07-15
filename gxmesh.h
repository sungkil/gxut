//*********************************************************
// Copyright 2011-2020 Sungkil Lee
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//*********************************************************

#pragma once
#ifndef __GX_MESH_H__
#define __GX_MESH_H__

// timestamp to indicate struct changes in other files
static const char* __GX_MESH_H_TIMESTAMP__ = _strdup(__TIMESTAMP__);

#if defined(__has_include)
	#if __has_include("gxmath.h")
		#include "gxmath.h" // the only necessary header for gxmesh
	#endif
	#if __has_include("gxsampler.h")
		#include "gxsampler.h"
	#endif
#endif

//*************************************
// OpenGL forward declarations
namespace gl
{
	struct Buffer;
	struct VertexArray;
	struct Texture;
} 
// D3DX forward decl.
struct ID3D10Buffer;
struct ID3D11Buffer;
struct ID3D10ShaderResourceView;
struct ID3D11ShaderResourceView;
// mesh forward decl.
struct geometry;
struct object;
struct mesh;
// camera
struct camera_t;

//*************************************
// 16-byte aligned light for direct lights
#ifndef __cplusplus // std140 definition for shaders
struct light_t
{
	vec4	pos, color, normal;		// use normal.xyz
};

struct vpl_t
{
	vec4	pos, color, normal;		// use normal.xyz
	mat4	view_matrix, projection_matrix;
	float	fovy, aspect, dnear, dfar;
};
#else
struct light_t
{
	vec4	pos;				// directional light (position.a==0) ignores normal
	vec4	color;				// shared for diffuse/specular; color.a = specular scale
	vec3	normal;				// direction (the negated vector of position for directional light)
	uint	bounce:6;			// zero means direct light
	uint	mouse:1;			// dynamic binding to the mouse?
	uint	bind:1;				// dynamic binding to an object
	uint	object_index:24;	// ID of object bound to this light

	// transformation to camera (eye-coordinate) space
	vec4	ecpos( const mat4& view_matrix ) const { return pos.a==0 ? vec4(mat3(view_matrix)*pos.xyz, 0.0f) : view_matrix*pos; }
	vec3	ecnorm( const mat4& view_matrix ) const { return mat3(view_matrix)*normal; }
	vec3	dir() const { return normalize(pos.xyz); }				// directions and angles against view vector
	float	phi() const { vec3 d=dir(); return atan2(d.y,d.x); }	// angle on the xy plane orthogonal to the optical axis
	float	theta() const { return acos(dir().z); }					// angle between outgoing light direction and the optical axis
};

// area light source
struct area_light_t : public light_t
{
	float	radius;		// world-space radius as a sphere
};

// light as camera: use for VPLs and shadows (actually, including real lights as well)
struct vpl_t : public light_t
{
	mat4	view_matrix;		// view matrix to render shadow map
	mat4	projection_matrix;	// projection matrix to render shadow map
	float	fovy;				// vertical field of view
	float	aspect;				// aspect ratio
	float	dnear;				// near clip for projection
	float	dfar;				// far clip for projection
};

static_assert(sizeof(light_t)%16==0, "size of struct light_t should be aligned at 16-byte boundaries");
static_assert(sizeof(vpl_t)%16==0,	 "size of struct vpl_t should be aligned at 16-byte boundaries");
#endif

//*************************************
// light list type
struct lights_t : public std::vector<light_t>
{
	uint crc=0;	// crc32c to detect the change of samples
	void clear() noexcept { __super::clear(); crc=0; }
};

//*************************************
// ray for ray tracing
template <class T> struct tray // defined as a template to avoid "a constructor in aggregate struct
{
	union { struct { tvec3<T> pos, dir; tvec4<T> tex; }; struct { tvec3<T> o, d; T t, tfar, time; int depth; }; }; // lens system rays (with texcoord) or pbrt-like rays (tnear/tfar = parameters of the nearest/farthest intersections)
	tray(): t(0), tfar(T(FLT_MAX)), time(0), depth(0){}
	tray(const vec3& _pos, const vec3& _dir, float _tnear=0.0f, float _tfar=FLT_MAX): tray(){ pos=_pos; dir=_dir; t=_tnear; tfar=_tfar; }
	tray(const vec3& _pos, const vec3& _dir, const vec4& _tex): tray(){ pos=_pos; dir=_dir; tex=_tex; }
};
using ray = tray<float>;

//*************************************
// intersection
struct isect
{
	vec3	pos;						// position at intersection
	vec3	norm;						// face normal at intersection
	float	t=FLT_MAX;					// nearest intersection: t<0 indicates inverted intersection on spherical surfaces
	union{float tfar=FLT_MAX,theta;};	// farthest intersection (gxut) or incident angle (oxut)
	uint	g=0xffffffff;				// index of the geometry at the intersection
	vec3	vnorm;						// vertex normal at intersection
	vec2	tc;							// texture coordinate
};

//*************************************
// bounding box: 16-bytes aligned for std140 layout
#ifndef __cplusplus
struct bbox { vec4 m; vec4 M; }; // bounding box in std140 layout
#else
struct bbox : public bbox_t
{
	bbox(){ clear(); }
	bbox( const vec3& v0 ){ m=M=v0; }
	bbox( bbox&& b ){ m=b.m; M=b.M; }
	bbox( const bbox& b ){ m=b.m; M=b.M; }
	bbox( const vec3& v0, const vec3& v1 ){ m=vec3(min(v0.x, v1.x), min(v0.y, v1.y), min(v0.z, v1.z)); M=vec3(max(v0.x, v1.x), max(v0.y, v1.y), max(v0.z, v1.z)); }
	bbox( const vec3& v0, const vec3& v1, const vec3& v2 ){ m=vec3(min(min(v0.x, v1.x), v2.x), min(min(v0.y, v1.y), v2.y), min(min(v0.z, v1.z), v2.z)); M=vec3(max(max(v0.x, v1.x), v2.x), max(max(v0.y, v1.y), v2.y), max(max(v0.z, v1.z), v2.z)); }
	bbox( const bbox& b0, const bbox& b1 ){ m=vec3(min(b0.m.x, b1.m.x), min(b0.m.y, b1.m.y), min(b0.m.z, b1.m.z)); M=vec3(max(b0.M.x, b1.M.x), max(b0.M.y, b1.M.y), max(b0.M.z, b1.M.z)); }
	inline void clear(){ M=-(m=FLT_MAX); }

	// assignment
	bbox& operator=( bbox&& b ) noexcept { m=b.m; M=b.M; return *this; }
	bbox& operator=( const bbox& b ){ m=b.m; M=b.M; return *this; }
	bbox& operator=( bbox_t&& b ){ m=b.m; M=b.M; return *this; }
	bbox& operator=( const bbox_t& b ){ m=b.m; M=b.M; return *this; }

	// array access
	inline const vec3& operator[]( ptrdiff_t i ) const { return (&m)[i]; }
	inline vec3& operator[]( ptrdiff_t i ){ return (&m)[i]; }

	// attribute query
	inline vec3 center() const { return (M+m)*0.5f; }
	inline vec3 size() const { return (M-m); }
	inline vec3 extent() const { return (M-m); }
	inline float radius() const { return size().length()*0.5f; }
	inline float surface_area() const { vec3 e=size(); return (e.x*e.z+e.y*e.x+e.z*e.y)*2.0f; }
	inline float volume() const { vec3 e=size(); return e.x*e.y*e.z; }
	inline int max_axis() const { vec3 e=size(); int a=0;if(e.y>e.x)a=1;if(e.z>e[a])a=2; return a; }
	inline int min_axis() const { vec3 e=size(); int a=0;if(e.y<e.x)a=1;if(e.z<e[a])a=2; return a; }
	inline float max_extent() const { vec3 e=size(); int a=0;if(e.y>e.x)a=1;if(e.z>e[a])a=2; return e[a]; }
	inline float min_extent() const { vec3 e=size(); int a=0;if(e.y<e.x)a=1;if(e.z<e[a])a=2; return e[a]; }

	// query on boxes or points
	inline bool overlap( const bbox& b ) const { return (M.x>=b.m.x)&&(m.x<=b.M.x)&&(M.y>=b.m.y)&&(m.y<=b.M.y)&&(M.z>=b.m.z)&&(m.z<=b.M.z); }
	inline bool include( const vec3& v ) const { return v.x>=m.x&&v.x<=M.x&&v.y>=m.y&&v.y<=M.y&&v.z>=m.z&&v.z<=M.z; }
	inline vec3 lerp( float tx, float ty, float tz ) const { return vec3(::lerp(m.x,M.x,tx), ::lerp(m.y,M.y,ty), ::lerp(m.z,M.z,tz)); }
	inline vec3 lerp( const vec3& t ) const { return lerp(t.x,t.y,t.z); }
	inline vec3 offset( const vec3& v ) const { return (v-m)/size(); }
	inline uvec3 corner_index( uint index ) const { uint j=index%4; return uvec3((j>>1)^(j&1),j>>1,index>>2); }
	inline std::array<uvec3,8> corner_indices() const { std::array<uvec3,8> v; for(uint k=0; k<8; k++) v[k]=corner_index(k); return v; }
	inline vec3 corner( uint index ) const { uvec3 i=corner_index(index); return lerp(float(i.x), float(i.y), float(i.z)); }
	inline std::array<vec3,8> corners() const { std::array<vec3,8> v; for(uint k=0; k<8; k++) v[k]=corner(k); return v; }	// returns 000 100 110 010 001 101 111 011

	// comparison
	bool operator==( const bbox& b ) const { return m==b.m&&M==b.M; }
	bool operator!=( const bbox& b ) const { return m!=b.m||M!=b.M; }

	// expansion (union)
	inline const bbox& expand( const bbox& b ){ m=vec3(min(m.x,b.m.x),min(m.y,b.m.y),min(m.z,b.m.z)); M=vec3(max(M.x,b.M.x),max(M.y,b.M.y),max(M.z,b.M.z)); return *this; }
	inline const bbox& expand( const vec3& v ){ m=vec3(min(m.x,v.x),min(m.y,v.y),min(m.z,v.z)); M=vec3(max(M.x,v.x),max(M.y,v.y),max(M.z,v.z)); return *this; }
	inline const bbox& expand( const vec3& v0, const vec3& v1, const vec3& v2 ){ m=vec3(min(min(m.x,v0.x),min(v1.x,v2.x)),min(min(m.y,v0.y),min(v1.y,v2.y)),min(min(m.z,v0.z),min(v1.z,v2.z))); M=vec3(max(max(M.x,v0.x),max(v1.x,v2.x)),max(max(M.y,v0.y),max(v1.y,v2.y)),max(max(M.z,v0.z),max(v1.z,v2.z))); return *this; }
	inline const bbox& expand( const mat4& m ){ auto c=corners(); clear(); for(uint k=0; k<c.size(); k++) expand(m*c[k]); return *this; }

	// transformation
	inline const bbox& scale( float s ){ vec3 c=center(); m=c+(m-c)*s; M=c+(M-c)*s; return *this; }
	inline const bbox& scale( const vec3& v ){ vec3 c=center(); m=c+(m-c)*v; M=c+(M-c)*v; return *this; }
	inline const bbox& scale( float sx, float sy, float sz ){ vec3 c=center(); m=c+(m-c)*vec3(sx,sy,sz); M=c+(M-c)*vec3(sx,sy,sz); return *this; }

	// matrics
	inline mat4 model_matrix() const { return mat4::translate(center())*mat4::scale(size()*0.5f); }

	// intersection
	inline bool intersect( ray r );
	inline bool intersect( ray r, isect& h );
};

inline bbox operator*(const mat4& m, const bbox& b){ return bbox(b).expand(m); }
#endif

//*************************************
// sphere
struct sphere
{
	vec3	pos;
	float	radius;
	inline bool intersect( ray r, isect& h );
};

//*************************************
// view frustum for culling
struct frustum_t : public std::array<vec4, 6> // left, right, top, bottom, near, far
{
	__forceinline frustum_t() = default;
	__forceinline frustum_t( frustum_t&& ) = default;
	__forceinline frustum_t( const frustum_t& ) = default;
	__forceinline frustum_t( const mat4& view_matrix, const mat4& projection_matrix ){ update(projection_matrix*view_matrix); }
	__forceinline frustum_t( camera_t& c );
	__forceinline frustum_t( vpl_t& c ){ update(c); }
	__forceinline frustum_t& operator=( frustum_t&& ) = default;
	__forceinline frustum_t& operator=( const frustum_t& ) = default;

	__forceinline frustum_t& update( const mat4& view_projection_matrix ){ auto* planes = data(); for(int k=0;k<6;k++){planes[k]=view_projection_matrix[k>>1]*float(1-(k&1)*2)+view_projection_matrix[3];planes[k]/=planes[k].xyz.length();} return *this; }
	__forceinline frustum_t& update( camera_t& c );
	__forceinline frustum_t& update( vpl_t& c ){ return update(c.projection_matrix*c.view_matrix); }
	__forceinline bool cull( const bbox_t&  b) const { vec4 pv; pv.w=1.0f; for(int k=0;k<6;k++){ const vec4& plane=operator[](k); for(int j=0;j<3;j++)pv[j]=plane[j]>0?b.M[j]:b.m[j]; if(pv.dot(plane)<0)return true;} return false; }
};

//*************************************
// Thin-lens camera definition
#ifndef __cplusplus
struct camera_t // std140 layout for OpenGL uniform buffer objects
{
	mat4	view_matrix, projection_matrix;
	float	fovy, aspect, dnear, dfar;
	vec4	eye, center, up;			// 16-bytes aligned for std140 layout
	float	F, E, df, fn;
};
#else
struct camera_t
{
	mat4 view_matrix, projection_matrix;
	union { struct { union {float fovy, height;}; float aspect,dnear,dfar; }; vec4 pp; }; // fov in radians; height for orthographic projection; pp=perspective parameters
	alignas(16) vec3 eye, center, up;	// lookAt params (16-bytes aligned for std140 layout)
	float F, E, df, fn;					// focal distance, lens radius, focusing depth (in object distance), f-number

	camera_t() = default;
	camera_t(camera_t&& c) = default;
	camera_t(const camera_t& c) = default;
	camera_t& operator=(camera_t&& c) = default;
	camera_t& operator=(const camera_t& c) = default;
};
static_assert(sizeof(camera_t)%16==0, "size of struct camera_t should be aligned at 16-byte boundary");
#endif

struct stereo_t
{
	enum model_t {NONE=0,LEFT=1,RIGHT=2,BOTH=3,ALTER=4}; // bitwise-and with left/right indicates left/right is drawn; ALTER renders left/right for even/odd frames
	uint		model = 0;				// stereo-rendering model
	float		ipd = 64.0f;			// inter-pupil distance for stereoscopy: 6.4 cm = men's average
	camera_t	left, right;			// left, right cameras
};

struct camera : public camera_t
{
	vec3		dir;					// dir = center - eye (view direction vector)
	int			frame = RAND_MAX;		// frame used for this camera; used in a motion tracer
	frustum_t	frustum;				// view frustum for culling
	camera_t	prev;					// placeholder for the camera at the previous frame
	stereo_t	stereo;					// stereo rendering attributes

	mat4	inverse_view_matrix() const { return mat4::look_at_inverse(eye,center,up); } // works without eye, center, up
	float	coc_norm_scale() const { return (F/fn*0.5f)*0.5f/df/tan(fovy*0.5f); } // normalized coc scale in the screen space; E (lens_radius)=F/fn*0.5f
	float	coc_scale( int height ) const { return coc_norm_scale()*float(height); } // screen-space coc scale; so called "K" so far
	mat4	perspective_dx() const { mat4 m = projection_matrix; m._33 = dfar/(dnear-dfar); m._34*=0.5f; return m; } // you may use mat4::perspectiveDX() to set canonical depth range in [0,1] instead of [-1,1]
	vec2	plane_size(float ecd = 1.0f) const { return vec2(2.0f/projection_matrix._11, 2.0f/projection_matrix._22)*ecd; } // plane size (width, height) at eye-coordinate distance 1
	void	update_depth_clips( const bbox& bound ){ bbox b=view_matrix*bound; vec2 z(max(0.001f,-b.M.z),max(0.001f,-b.m.z)); dnear=max(max(bound.radius()*0.00001f, 50.0f),z.x*0.99f); dfar=max(max(dnear+1.0f,dnear*1.01f),z.y*1.01f); }
	void	update_view_frustum(){ frustum.update(projection_matrix*view_matrix); }
	bool	cull( const bbox_t& b ) const { return frustum.cull(b); }
	void	update_stereo(){ if(!stereo.model) return; float s=0.5f*stereo.ipd, o=s*dnear/df, t=dnear*tanf(0.5f*fovy*(fovy<PI<float>?1.0f:PI<float>/180.0f)), R=t*aspect; vec3 stereo_dir = normalize(cross(dir,up))*s; auto& l=stereo.left=*this; l.eye-=stereo_dir; l.center-=stereo_dir; l.view_matrix=mat4::look_at(l.eye, l.center, l.up); l.projection_matrix=mat4::perspective_off_center(-R+o, R+o, t, -t, dnear, dfar); auto& r=stereo.right=*this; r.eye+=stereo_dir; r.center+=stereo_dir; r.view_matrix=mat4::look_at(r.eye,r.center,r.up); r.projection_matrix=mat4::perspective_off_center(-R-o, R-o, t, -t, dnear, dfar);}
};

__forceinline frustum_t::frustum_t( camera_t& c ){ update(c); }
__forceinline frustum_t& frustum_t::update( camera_t& c ){ return update(c.projection_matrix*c.view_matrix); }

//*************************************
// material definition (std140 layout, aligned at 16-byte/vec4 boundaries)
#ifndef __cplusplus
struct material { vec4 color; float metal, rough, emissive, beta, specular, n; uvec2 TEX, NRM, PBR; };
#else
struct material
{
	vec4		color;			// albedo or Blinn-Phong diffuse; color.a=opacity=1-transmittance
	float		metal=0.0f;		// metallic: mapped to specular intensity
	float		rough=0.2f;		// roughness: mapped to specular power; zero means mirror-reflective (used for ray tracing)
	float		emissive=0.0f;	// 1 only for light sources; if an object is named "light*", its material is forced to be emissive
	float		beta=48.0f;		// specular power of Phong model
	float		specular=1.0f;	// specular intensity
	float		n=1.0f;			// refractive index
	uint64_t	TEX=0;			// GPU handle to (albedo,alpha) texture; RGBA format indicates the presence of alpha/opacity channel
	uint64_t	NRM=0;			// GPU handle to normal map
	uint64_t	PBR=0;			// GPU handle to PBR texture: (ambient occlusion,roughness,metallic), where RM follows glTF spec
};
static_assert(sizeof(material)%16==0, "struct material must be aligned at a 16-byte boundary");
#endif

inline float beta_to_roughness(float b){ return float(sqrtf(2.0f/(b+2.0f))); }  // Beckmann roughness in [0,1] (0:mirror, 1: Lambertian)
inline float roughness_to_beta(float r){ return 2.0f/(r*r)-2.0f; }

union material_textures_t
{
	struct { gl::Texture				*albedo, *normal, *pbr; };
	struct { ID3D10ShaderResourceView	*albedo, *normal, *pbr; } d3d10;
	struct { ID3D11ShaderResourceView	*albedo, *normal, *pbr; } d3d11;
};

struct material_impl : public material
{
	const uint	ID;
	char		name[_MAX_PATH]={};
	material_textures_t	texture={};
	std::map<std::string,path> path;	// <key,path>: albedo, alpha, normal, ambient, rough, metal, emissive, ...

	material_impl( uint id ):ID(id){}
};

//*************************************
// cull data definition
struct cull_t
{
	enum model { NONE=0, USER=1<<0, REF=1<<1, VFC=1<<2, CHCPP=1<<3, WOC=1<<4, ROC=1<<5, HROC=1<<6, DROC=1<<7 };
	static const char* name( model m ){ return m==NONE?"NONE":m==USER?"USER":m==VFC?"VFC":m==CHCPP?"CHCPP":m==WOC?"WOC":m==ROC?"ROC":m==HROC?"HROC":m==DROC?"DROC":"UNKNOWN"; }
	uchar data = 0; // bits of the corresponding culling types are set (bitwise OR-ed)

	inline operator uchar() const { return data; }
	inline bool operator()(uchar m) const { return (data&m)!=0; }
	inline cull_t& reset(uchar m=0xff){ data = (m==0xff)?0:(data&~m); return *this; }
	inline cull_t& set(uchar m){ data|=m; return *this; }
};

//*************************************
// acceleration structures
struct acc_t
{
	enum model_t { NONE, BVH, KDTREE } model = NONE;
	enum method_t { SAH, MIDDLE, EQUAL_COUNTS };
	mesh* p_mesh = nullptr;	// should be set to source mesh
	mesh* proxy = nullptr;	// mesh of transform-baked bounding boxes; release only where this is created

	virtual ~acc_t(){}			// should be virtual to enforce to call inherited destructors
	virtual void release()=0;
	virtual bool intersect( ray r, isect& h ) const=0;
};

//*************************************
// Bounding volume hierarchy
struct bvh_t : public acc_t // two-level hierarchy: mesh or geometry
{
	struct node
	{
		vec3 m=FLT_MAX; uint second_or_index;		// second: offset to right child of interior node; index: primitive index in index buffer
		vec3 M=-FLT_MAX; uint parent:30, axis:2;
		const bbox& box() const { return reinterpret_cast<const bbox&>(*this); }
		__forceinline bool is_leaf() const { return axis==3; }
	};
	std::vector<node> nodes;
	virtual void release()=0;
	virtual bool intersect( ray r, isect& h ) const;
	virtual bool has_prims() const;

	uint geometry_node_count() const;
	mesh* create_proxy( bool double_sided=false, bool quads=false ); // create meshes of bounding boxes of all the nodes; may include primitives or not
	void update_proxy();
};

//*************************************
// KDtree
struct kdtree_t : public acc_t // single-level hierarchy
{
	struct node
	{
		union { float pos; uint count; };
		uint second_or_index;
		uint geometry;
		uint parent:30, axis:2;
		__forceinline bool is_leaf() const { return axis==3; }
	};
	std::vector<node>	nodes;
	std::vector<ivec2>	prims; // ordered primitives (face index, geometry index)
	virtual bool intersect( ray r, isect& h ) const;
};

//*************************************
// geometry: 144-bytes aligned; differentiated by material and grouping
#ifndef __cplusplus
struct geometry // std430 layout for OpenGL shader storage buffers
{
	uint	count, instance_count, first_index, base_vertex, base_instance; // DrawElementsIndirectCommand
	uint	material_index, acc_index, acc_prim_count, acc_prim_index, pad[3]; // geometry-bvh node index, primitive-bvh count/first_index
	bbox	box;
	mat4	mtx;
};
#else
struct geometry
{
	uint	count=0;			// start of DrawElementsIndirectCommand: number of indices
	uint	instance_count=1;	// default: 1
	uint	first_index=0;
	uint	base_vertex=0;		// default: 0
	uint	ID=-1;				// shared with base_instance (typically 0)
	uint	material_index=-1;
	uint	acc_index=0;		// geometry-bvh node index
	uint	acc_prim_count=0;	// primitive-bvh: node count
	uint	acc_prim_index=0;	// primitive-bvh: first node index
	ushort	instance=0;			// host-side instance ID (up to 2^16: 65536)
	cull_t	cull;				// 8-bit cull mask
	bool	bg=false;			// is this a background object?
	mesh*	root=nullptr;		// pointer to mesh; should be aligned at 8-byte boundary
	union{bbox box;struct{uint _[7],object_index;};}; // transform-unbaked bounding box, object index in box.M.a (dummy padding)
	mat4	mtx;				// transformation matrix (the same for all object geometries)

	geometry() = delete; // no default ctor to enforce to assign root
	geometry( const geometry& other ){ memcpy(this,&other,sizeof(geometry)); }
	geometry(mesh* p_mesh): root(p_mesh){}
	geometry(mesh* p_mesh, uint id, uint obj_index, uint start_index, uint index_count, const bbox* box, uint mat_index): count(index_count), first_index(start_index), ID(id), material_index(mat_index), object_index(obj_index), root(p_mesh){ if(box) this->box=*box; }
	geometry& operator=( const geometry& other ){ memcpy(this,&other,sizeof(geometry)); return *this; }

	inline object*		parent() const;
	inline const char*	name() const;
	inline void*		offset() const { return (void*)(sizeof(geometry)*ID); } // for rendering commands
	inline uint			face_count() const { return count/3; }
	inline float		surface_area() const;
	inline bool			intersect( ray r, isect& h ) const; // linear intersection
	inline bool			acc_empty() const { return acc_prim_count==0; }
};

static_assert(sizeof(geometry)%16==0,	"sizeof(geometry) should be multiple of 16-byte");
static_assert(sizeof(geometry)==144,	"sizeof(geometry) should be 144, when aligned correctly");
#endif

//*************************************
// a set of geometries for batch control (not related to the rendering)
struct object
{
	uint				ID=-1;
	uint				instance=0;
	char				name[_MAX_PATH]={};
	mesh*				root=nullptr;
	bbox				box;
	std::vector<uint>	children;				// indices to child geometries
	struct {bool dynamic=false,bg=false;} attrib;	// dynamic: potential matrix changes; background: backdrops such as floor/ground; set in other plugins (e.g., AnimateMesh)

	object() = delete;  // no default ctor to enforce to assign parent
	object( mesh* p_mesh ):root(p_mesh){}
	object( mesh* p_mesh, uint id, const char* name ):ID(id), root(p_mesh){ strcpy(this->name, name); }

	// query and attributes
	inline bool empty() const { return children.empty(); }
	inline uint size() const { return uint(children.size()); }
	inline uint face_count() const { uint n=0; for( auto& g : *this ) n+=g.face_count(); return n; }
	inline bbox update_bound(){ box.clear(); for( auto& g : *this ) box.expand(g.mtx*g.box); return box; }

	// create helpers
	inline geometry* create_geometry( size_t first_index, size_t index_count=0, const bbox* box=nullptr, size_t mat_index=-1 );
	inline geometry* create_geometry( const geometry& other );

	// iterator types
	struct iterator
	{
		using value_type = geometry;
		using reference = value_type&;
		using pointer = value_type*;
		using iterator_category = std::forward_iterator_tag;

		iterator( mesh* p_mesh, uint object_index, size_t idx=0 ):root(p_mesh),parent(object_index),index(uint(idx)){}
		iterator operator++(){ ++index; return *this; }
		iterator operator++(int){ auto self=*this; index++; return self; }
		reference operator*();
		pointer operator->();
		const reference operator*() const { return operator*(); }
		const pointer operator->() const { return operator->(); }
		bool operator==( const iterator& rhs ) const { return memcmp(this,&rhs,sizeof(*this))==0; }
		bool operator!=( const iterator& rhs ) const { return memcmp(this,&rhs,sizeof(*this))!=0; }
	
	protected:
		uint	index, parent;
		mesh*	root;
	};

	// iterators
	iterator begin(){return iterator(root,ID);}			const iterator begin() const { return begin(); }
	iterator end(){return iterator(root,ID,size());}	const iterator end() const { return end(); }
	geometry& front();									const geometry& front() const { return front(); }
	geometry& back();									const geometry& back() const { return back(); }
	geometry& operator[]( size_t i );					const geometry& operator[] ( size_t i ) const { return operator[](i); }
};

//*************************************
// a set of vertices, indices, objects, geometries, materials
struct mesh
{
	std::vector<vertex>			vertices;
	std::vector<uint>			indices;
	std::vector<object>			objects;
	std::vector<geometry>		geometries;
	std::vector<material_impl>	materials;

	// volitile spaces for GPU buffers
	union buffers_t {
		struct { gl::VertexArray* vertex; gl::Buffer *geometry, *count; }; // vertex array, geometry/command buffer, count buffer for OpenGL
		struct { ID3D10Buffer *vertex, *index; } d3d10; // vertex/index buffers for D3D
		struct { ID3D11Buffer *vertex, *index; } d3d11; // vertex/index buffers for D3D
	} buffer = {};

	// acceleration and dynamics
	bbox		box;
	union { acc_t* acc=nullptr; bvh_t* bvh; kdtree_t* kdtree; }; // BVH or KD-tree

	// instancing
	uint		instance_count=1;	// instances are physically added into objects

	// proxy mesh
	mesh*		proxy=nullptr;		// created and released in GLMesh 

	// auxiliary information
	wchar_t		file_path[_MAX_PATH]={};	// mesh file path
	wchar_t		mtl_path[_MAX_PATH]={};	// material file path (e.g., *.mtl)

	// constructor
	mesh(){ vertices.reserve(1<<20); indices.reserve(1<<20); objects.reserve(1<<16); geometries.reserve(1<<16); materials.reserve(1<<16); }
	mesh(mesh&& other){ operator=(std::move(other)); } // move constructor
	mesh& operator=(mesh&& other); // move assignment operator
	virtual ~mesh(){ release(); }

	// release/memory
	void release(){ vertices.clear(); indices.clear(); geometries.clear(); objects.clear(); materials.clear(); shrink_to_fit(); }
	mesh* shrink_to_fit(){ vertices.shrink_to_fit(); indices.shrink_to_fit(); geometries.shrink_to_fit(); objects.shrink_to_fit(); materials.shrink_to_fit(); return this; }

	// face/object/geometry/proxy/material helpers
	uint face_count() const { uint f=0; for(const auto& g:geometries) f+=g.count; return f/3; }
	uint vertex_count() const { return uint(vertices.size())*instance_count; }
	object* create_object( const char* name ){ objects.emplace_back(object(this, uint(objects.size()), name)); return &objects.back(); }
	object*	find_object( const char* name ){ for(uint k=0; k<objects.size(); k++)if(_stricmp(objects[k].name,name)==0) return &objects[k]; return nullptr; }
	std::vector<object*> find_objects( const char* name ){ std::vector<object*> v; for(uint k=0; k<objects.size(); k++)if(_stricmp(objects[k].name,name)==0) v.push_back(&objects[k]); return v; }
	mesh* create_proxy( bool double_sided=false, bool use_quads=false );	// proxy mesh helpers: e.g., bounding box
	void update_proxy();													// update existing proxy with newer matrices
	std::vector<material> pack_materials() const { std::vector<material> p; auto& m=materials; p.resize(m.size()); for(size_t k=0,kn=p.size();k<kn;k++) p[k]=m[k]; return p; }
	void dump_binary( const wchar_t* dir=L""); // dump the vertex/index buffers as binary files

	// bound, dynamic, intersection
	inline bool is_dynamic() const { for(size_t k=0, kn=objects.size()/instance_count; k<kn; k++) if(objects[k].attrib.dynamic) return true; return false; }
	void update_bound( bool b_recalc_tris=false );
	int find_up_vector() const;
	bool intersect( ray r, isect& h, bool use_acc=true ) const;

	// opengl draw functions (implemented in gxopengl.h)
	static const int GL_TRIANGLES=0x0004;
	typedef void GLvoid;
	typedef unsigned int GLenum;
	typedef int GLint;
	typedef int GLsizei;
	typedef unsigned int GLuint;
	typedef signed long long int GLintptr;
	void draw_arrays( GLint first, GLsizei count=0, GLenum mode=GL_TRIANGLES, bool b_bind=true );
	void draw_arrays_instanced( GLint first, GLsizei instance_count, GLsizei count=0, GLenum mode=GL_TRIANGLES, bool b_bind=true );
	void draw_elements( GLuint first, GLsizei count=0, GLenum mode=GL_TRIANGLES, bool b_bind=true );
	void draw_elements_instanced( GLuint first, GLsizei instance_count, GLsizei count=0, GLenum mode=GL_TRIANGLES, bool b_bind=true );
	void draw_elements_indirect( GLvoid* indirect=nullptr, GLenum mode=GL_TRIANGLES, bool b_bind=true );
	void draw_range_elements( GLuint first, GLuint end, GLsizei count=0, GLenum mode=GL_TRIANGLES, bool b_bind=true );
	void multi_draw_elements( GLuint* pfirst, const GLsizei* pcount, GLsizei draw_count, GLenum mode=GL_TRIANGLES, bool b_bind=true );
	void multi_draw_elements_indirect( GLsizei draw_count, GLsizei stride=sizeof(geometry), GLvoid* indirect=nullptr, GLenum mode=GL_TRIANGLES, bool b_bind=true );
	void multi_draw_elements_indirect_count( GLsizei max_draw_count, GLsizei stride=sizeof(geometry), const void* indirect=0 /* GL_DRAW_INDIRECT_BUFFER */, GLintptr draw_count=0 /* GL_PARAMETER_BUFFER_ARB */, GLenum mode=GL_TRIANGLES, bool b_bind=true );
};

//*************************************
// Volume data format
struct volume
{
	wchar_t	file_path[_MAX_PATH]={};
	uint	width, height, depth;
	vec3	voxel_size;		// size of a unit voxel
	uint	format;			// GL_R32F or GL_RGBA32F
	bbox	box;			// bounding box
	float*	data;			// voxel data array: cast by format
	vec4	table[256];		// transfer function: usually 256 size
};

//*************************************
// late implementations for geometry

inline object* geometry::parent() const { return root && object_index < root->objects.size() ? &root->objects[object_index] : nullptr; }
inline const char* geometry::name() const { auto* p=parent(); return p ? p->name : ""; }
inline float geometry::surface_area() const { if(count==0) return 0.0f; if(root->vertices.empty()||root->indices.empty()) return 0.0f; vertex* v=&root->vertices[0]; uint* i=&root->indices[0]; float sa=0.0f; for(uint k=first_index, kn=k+count; k<kn; k+=3) sa+=(v[i[k+1]].pos-v[i[k+0]].pos).cross(v[i[k+2]].pos-v[i[k+0]].pos).length()*0.5f; return sa; }

//*************************************
// late implementations for object

inline geometry* object::create_geometry( size_t first_index, size_t index_count, const bbox* box, size_t mat_index )
{
	auto& v = root->geometries; uint gid=uint(v.size());
	v.emplace_back(geometry(root, gid, this->ID, uint(first_index), uint(index_count), box, uint(mat_index)));
	children.emplace_back(gid);
	return &v.back();
}

inline geometry* object::create_geometry( const geometry& other )
{
	auto& v = root->geometries; uint gid=uint(v.size());
	v.emplace_back(other);
	auto* g=&v.back(); g->ID=gid; children.emplace_back(gid); return g;
}

inline object::iterator::reference object::iterator::operator*(){ auto g=root->objects[parent].children[index]; return root->geometries[g]; }
inline object::iterator::pointer object::iterator::operator->(){ auto g=root->objects[parent].children[index]; return &root->geometries[g]; }
inline geometry& object::front(){ return root->geometries[children.front()]; }
inline geometry& object::back(){ return root->geometries[children.back()]; }
inline geometry& object::operator[]( size_t i ){ return root->geometries[i]; }

//*************************************
// late implementations for mesh

__noinline mesh& mesh::operator=( mesh&& other ) // move assignment operator
{
	size_t offset = 0;
	vertices = std::move(other.vertices);		offset += sizeof(decltype(vertices));
	indices = std::move(other.indices);			offset += sizeof(decltype(indices));
	objects = std::move(other.objects);			offset += sizeof(decltype(objects));
	geometries = std::move(other.geometries);	offset += sizeof(decltype(geometries));
	materials = std::move(other.materials);		offset += sizeof(decltype(materials));
	memcpy(((char*)this)+offset, ((char*)&other)+offset, sizeof(mesh)-offset);
	if(acc) acc->p_mesh = this;
	for(auto& o:objects) o.root=this;
	for(auto& g:geometries) g.root=this;

	return *this;
}

__noinline void mesh::update_bound( bool b_recalc_tris )
{
	// update geometry first
	size_t kn=geometries.size(), gn=kn/instance_count;
	if(b_recalc_tris)
	{
		for( size_t k=0; k<gn; k++ )
		{
			auto& g = geometries[k]; g.box.clear();
			vertex* V=&vertices[0]; uint* I=&indices[g.first_index];
			for( uint j=0, jn=g.count; j<jn; j+=3, I+=3)
				g.box.expand( V[I[0]].pos, V[I[1]].pos, V[I[2]].pos );
		}
	}

	for( size_t k=gn; k<kn; k++ ) geometries[k].box=geometries[k%gn].box;

	// update objects and mesh
	box.clear();
	for( auto& obj : objects )
		box.expand(obj.update_bound());
}

__noinline int mesh::find_up_vector() const
{
	if(box.max_extent()>(box.min_extent()*4.0f)) return box.min_axis();

	vec3 d=0;
	for(auto f:{"floor","ground","ceil","terrain","plane"})
		for(auto& o:objects)
		{
			if(!_stristr(o.name,f)||o.box.max_extent()<(o.box.min_extent()*4.0f)) continue;
			d[o.box.min_axis()] += 1.0; break;
		}
	
	int a=2;
	if(d[0]>0&&d[0]>d[2]) a=0;
	if(d[1]>0&&d[1]>d[a]) a=1;
	return a;
}

__noinline std::vector<uint> get_box_indices( bool double_sided, bool quads )
{
	std::vector<uint> v; // index definitions (CCW: counterclockwise by default)
	if(quads)	v = { 0, 3, 2, 1, /*bottom*/ 4, 5, 6, 7, /*top*/ 0, 1, 5, 4, /*left*/ 2, 3, 7, 6, /*right*/ 1, 2, 6, 5, /*front*/ 3, 0, 4, 7 /*back*/ };
	else		v = { 0, 2, 1, 0, 3, 2, /*bottom*/ 4, 5, 6, 4, 6, 7, /*top*/ 0, 1, 5, 0, 5, 4, /*left*/ 2, 3, 6, 3, 7, 6, /*right*/ 1, 2, 6, 1, 6, 5, /*front*/ 0, 4, 3, 3, 4, 7 /*back*/ };
	if(double_sided){ for(size_t k=0, f=quads?4:3, kn=v.size()/f; k<kn; k++) for(size_t j=0; j<f; j++) v.emplace_back(v[(k+1)*f-j-1]); } // insert indices (for CW)
	return v;
}

__noinline std::vector<uint> get_box_line_indices()
{
	return std::vector<uint>{
		0, 3, 3, 2, 2, 1, 1, 0, /*bottom*/ 4, 5, 5, 6, 6, 7, 7, 4, /*top*/   0, 1, 1, 5, 5, 4, 4, 0, /*left*/
		2, 3, 3, 7, 7, 6, 6, 2, /*right*/  1, 2, 2, 6, 6, 5, 5, 1, /*front*/ 3, 0, 0, 4, 4, 7, 7, 3 /*back*/ };
}

//*************************************
// mesh utilities
__noinline mesh* create_box_mesh( bbox box=bbox{vec3(-1.0f),vec3(1.0f)}, const char* name="box", bool double_sided=false, bool quads=false )
{
	mesh* m = new mesh();
	
	// create box vertices
	for( auto& c : box.corners() ) m->vertices.emplace_back(vertex{c,vec3(0.0f),vec2(0.0f)});

	// create box triangle/quad elements
	auto indices = std::move(get_box_indices( double_sided, quads ));
	m->indices.insert(m->indices.end(),indices.begin(),indices.end());
	m->create_object(name)->create_geometry(0,uint(m->indices.size()),(bbox*)&box,size_t(-1));

	// create box line elements
	auto line_indices = std::move(get_box_line_indices());
	m->indices.insert(m->indices.end(),line_indices.begin(),line_indices.end());
	m->create_object(format("%s.lines",name))->create_geometry(uint(indices.size()),uint(line_indices.size()),(bbox*)&box, size_t(-1));

	return m;
}

__noinline mesh* mesh::create_proxy( bool double_sided, bool quads )
{
	proxy = new mesh();

	// direct copy from parent mesh
	proxy->materials.clear();	// proxy not use materials
	proxy->instance_count = instance_count;
	proxy->box = box;
	proxy->acc = acc;	// use the same acceleration structures
	wcscpy(proxy->file_path, file_path);

	// copy objects and correct mesh pointers to proxy
	for(auto& o:proxy->objects=objects) o.root = proxy;

	// vertex/index definitions of a default box
	const auto corners = bbox(-1.0f,1.0f).corners();
	const auto tri_indices = get_box_indices(double_sided,quads);
		
	// create vertices/indices for triangles
	auto& i = proxy->indices;
	auto& v = proxy->vertices;
	uint vof=0;
	for(auto& g : geometries)
	{
		auto& b = g.box;
		proxy->objects[g.object_index].create_geometry(i.size(),tri_indices.size(),&b)->mtx=g.mtx;
		for(auto j:tri_indices) i.emplace_back(j+vof);
		mat4 m=mat4::translate(b.center())*mat4::scale(b.size()*0.5f);
		for(auto& c:corners) v.emplace_back(vertex{(m*c).xyz,c,vec2(0)}); // world pos, local pos
		vof+=uint(corners.size()); // increment by the corner count
	}

	return proxy;
}

__noinline void mesh::update_proxy()
{
	if(!proxy) return;
	if(proxy->instance_count!=instance_count){ printf("%s(): proxy->instance_count!=instance_count\n",__func__); return; }

	proxy->box = box;
	proxy->acc = acc;	// use the same acceleration structures

	for(auto& g : geometries)
	{
		proxy->geometries[g.ID].mtx = g.mtx;
		mat4 m = mat4::translate(g.box.center())*mat4::scale(g.box.size()*0.5f);
		for( uint k=0; k<8; k++ ){ auto& v = proxy->vertices[g.ID*8+k]; v.pos = (m*vec4(v.norm,1.0f)).xyz; }
	}
}

//*************************************
// late implementations for BVH
inline uint bvh_t::geometry_node_count() const
{
	if(!p_mesh) return 0; auto& g=p_mesh->geometries; if(g.empty()) return 0;
	return has_prims()? g[0].acc_prim_index : uint(nodes.size());
}

__noinline mesh* bvh_t::create_proxy( bool double_sided, bool quads )
{
	proxy = new mesh();

	// direct copy from source mesh
	proxy->materials.clear();	// proxy not use materials
	proxy->instance_count = 1;	// proxy instantiates all simultaneously
	proxy->box = p_mesh->box;
	proxy->acc = this;			// set this bvh

	// create objects for triangles and lines
	auto* tris = proxy->create_object("tris");
	auto* lines = proxy->create_object("lines");

	// vertex/index definitions of a default box
	const auto corners = bbox(-1.0f, 1.0f).corners();
	const auto tri_indices = get_box_indices(double_sided,quads);
		
	// create vertices/indices for triangles
	auto& i = proxy->indices;
	auto& v = proxy->vertices;
	uint vof=0;
	for(auto& n : nodes)
	{
		auto& b = n.box();
		tris->create_geometry(i.size(),tri_indices.size(),&b);
		for(auto j:tri_indices) i.emplace_back(j+vof);
		mat4 m=mat4::translate(b.center())*mat4::scale(b.size()*0.5f);
		for(auto& c:corners) v.emplace_back(vertex{(m*c).xyz,c,vec2(0)}); // world pos, local pos
		vof+=uint(corners.size()); // increment by the corner count
	}

	return proxy;
}

__noinline void bvh_t::update_proxy()
{
	if(!proxy) return;

	proxy->box = p_mesh->box;
	proxy->acc = this; // set this bvh

	for( auto& g : proxy->geometries )
	{
		auto& n = nodes[g.ID];
		mat4 m = mat4::translate(n.box().center())*mat4::scale(n.box().size()*0.5f);
		for(uint k=0; k<8; k++){ auto& v = proxy->vertices[g.ID*8+k]; v.pos = (m*vec4(v.norm,1.0f)).xyz; }
	}
}

//*************************************
// intersection implementations

// primary ray
__noinline ray gen_primary_ray( camera* cam, float x, float y )	// (x,y) in [0,1]
{
	const vec3& eye=cam->eye.xyz, center=cam->center.xyz, up=cam->up.xyz;
	float fh=tan(cam->fovy*0.5f)*2.0f, fw=fh*cam->aspect;		// frustum height/width in NDC
	vec3 dst=normalize(vec3(fw*(x-0.5f),fh*(y-0.5f),-1.0f));	// target pixel position on the image plane: make sure to have negative depth
	mat4 I = mat4::look_at_inverse(eye,center,up);				// inverse view matrix
	ray r; r.t=0.0f; r.tfar=FLT_MAX; r.o=eye; r.d=mat3(I)*dst;  return r;
}

// triangle intersection: isect=(pos,t), bc=(s,t)
__noinline bool intersect( ray r, vec3 v0, vec3 v1, vec3 v2, isect& h )
{
	// http://geomalgorithms.com/a06-_intersect-2.html#intersect3D_RayTriangle
	vec3 u=v1-v0, v=v2-v0, n=u.cross(v); if(n.length2()<0.000001f) return false;	// degenerate case: non-triangle
	float b=dot(n,r.dir); if(b>-0.000001f) return false;							// skip backfaces or rays lying on the plane
	float t=dot(n,v0-r.pos)/b; if(t<r.t||t>r.tfar) return false;					// out of range of [tnear,tfar]
	vec3 ipos=r.pos+r.dir*t, w=ipos-v0;

	// test on barycentric coordinate
	float uu=dot(u,u),uv=dot(u,v),vv=dot(v,v),wu=dot(w,u),wv=dot(w,v),D=uv*uv-uu*vv;
	float bs=(uv*wv-vv*wu)/D; if(bs<0||bs>1.0f)		return false;
	float bt=(uv*wu-uu*wv)/D; if(bt<0||bs+bt>1.0f)	return false;

	h.pos = ipos;
	h.norm = normalize(n);
	h.tc = vec2(bs, bt); // temporaily store texcoord
	h.t = t;
	h.tfar = t;
	return true;
}

// box intersection
__noinline bool bbox::intersect( ray r )
{
	float t0=r.t, t1=r.tfar;
	for( int k=0; k<3; k++ )
	{
		float i = 1.0f/r.d[k];
		if(i<0){t0=max(t0,(M[k]-r.o[k])*i);t1=min(t1,(m[k]-r.o[k])*i*1.00000024f); } // epsilon by 4 ulps
		else{	t0=max(t0,(m[k]-r.o[k])*i);t1=min(t1,(M[k]-r.o[k])*i*1.00000024f); } // epsilon by 4 ulps
		if(t0>t1) return false;
	}
	return t1>0;
}

__noinline bool bbox::intersect( ray r, isect& h )
{
	float t0=r.t, t1=r.tfar;
	for( int k=0; k<3; k++ )
	{
		float i = 1.0f/r.d[k];
		if(i<0){t0=max(t0,(M[k]-r.o[k])*i);t1=min(t1,(m[k]-r.o[k])*i*1.00000024f); } // epsilon by 4 ulps
		else{	t0=max(t0,(m[k]-r.o[k])*i);t1=min(t1,(M[k]-r.o[k])*i*1.00000024f); } // epsilon by 4 ulps
		if(t0>t1) return false;
	}

	h.t=t0; h.tfar=t1;
	return t1>0;
}

__noinline bool intersect( bbox_t b, ray r ){ return reinterpret_cast<bbox&>(b).intersect(r); }
__noinline bool intersect( bbox_t b, ray r, isect& h ){ return reinterpret_cast<bbox&>(b).intersect(r,h); }

// sphere intersection
__noinline bool sphere::intersect( ray r, isect& h )
{
	vec3 pc = r.pos - pos;
	float B = dot(pc, r.dir);
	float C = dot(pc, pc) - radius * radius;
	float B2C = B*B-C; if(B2C < 0.0f) return false; // no surface hit

	h.t = sqrt(B2C)*sign(radius*r.dir.z) - B;
	h.pos = r.dir*h.t + r.pos;	// update intersection point
	h.norm = normalize(h.pos - pos); h.norm *= -sign(dot(h.norm, r.dir));
	h.theta = acos(dot(-r.dir, h.norm)); if(h.theta < 0) h.theta = 0.0f;	// incident angle: preclude division by zero

	return true;
}

__noinline bool geometry::intersect( ray r, isect& h ) const
{
	const vertex* V = &root->vertices[0];
	const uint* I = &root->indices[first_index];
	h.g = 0xffffffff;

	for( uint k=0, kn=count/3; k<kn; k++, I+=3 )
	{
		const vertex &v0=V[I[0]], &v1=V[I[1]], &v2=V[I[2]];
		isect i; if(!::intersect(r,mtx*v0.pos,mtx*v1.pos,mtx*v2.pos,i)||i.t>h.t) continue;
		h=i; h.g=ID;
		reinterpret_cast<uvec3&>(h.vnorm) = uvec3{I[0],I[1],I[2]};
	}

	if(h.g!=0xffffffff)
	{
		uvec3 i = reinterpret_cast<uvec3&>(h.vnorm);
		const vertex& v0=V[i.x], &v1=V[i.y], &v2=V[i.z];
		vec3 bc=vec3(1-h.tc.x-h.tc.y,h.tc);
		h.vnorm=v0.norm*bc.x+v1.norm*bc.y+v2.norm*bc.z;
		h.tc=v0.tex*bc.x+v1.tex*bc.y+v2.tex*bc.z;
	}

	return h.g!=0xffffffff;
}

__noinline bool mesh::intersect( ray r, isect& h, bool use_acc ) const
{
	if(acc&&use_acc) return acc->intersect(r,h);

	h.g=0xffffffff;
	h.t=FLT_MAX;
	for( auto& g : geometries )
	{
		if(!(g.mtx*g.box).intersect(r)) continue;
		isect i; if(!g.intersect(r,i)||i.t>h.t) continue;
		h=i; h.g=g.ID;
	}
	return h.g!=0xffffffff;
}

//*************************************
// utility
#ifdef __GX_FILESYSTEM_H__
inline void mesh::dump_binary( const wchar_t* _dir )
{
	path dir = _dir;
	if(vertices.empty() || indices.empty()) return;
	dir = dir.add_backslash(); if(!dir.exists()) dir.mkdir();
	path vertex_bin_path = dir + path(file_path).name(false).name(false) + L".vertex.bin";
	path index_bin_path = dir + path(file_path).name(false).name(false) + L".index.bin";
	FILE* fp = _wfopen(vertex_bin_path, L"wb"); fwrite(&vertices[0], sizeof(vertex), vertices.size(), fp); fclose(fp);
	fp = _wfopen(index_bin_path, L"wb");  fwrite(&indices[0], sizeof(uint), indices.size(), fp);	   fclose(fp);
}
#endif

//*************************************
// line clipping by Liang Barsky algorithm
__noinline bool clip_line( vec2 p, vec2 q, vec2 lb, vec2 rt, vec2* p1=nullptr, vec2* q1=nullptr )
{
	vec2 d = q - p, t = vec2(0, 1);
	vec2 vs[4] = { {-d.x,p.x-lb.x}, {d.x,rt.x-p.x}, {-d.y,p.y-lb.y}, {d.y,rt.y-p.y} };
	for(int k = 0; k < 4; k++)
	{
		vec2 v = vs[k];
		float f = v.y / v.x;
		if(v.x<0){ if(f>t.y) return true; if(f>t.x) t.x=f; }
		else if(v.x>0){ if(f<t.x) return true; if(f<t.y) t.y=f; }
		else if(v.x==0 && v.y<0) return true;
	}

	if(p1&&t.x > 0.0f) *p1 = lerp(p, q, t.x);		// new p
	if(q1&&t.y < 1.0f) *q1 = lerp(p, q, t.y);		// new q
	return false;
}

#endif // __GX_MESH_H__
