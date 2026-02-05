//*********************************************************
// Copyright 2011-2040 Sungkil Lee
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

#if __has_include("gxmath.h")
	#include "gxmath.h"
#endif
#if __has_include("gxstring.h")
	#include "gxstring.h"
#endif
#if __has_include("gxfilesystem.h")
	#include "gxfilesystem.h"
#endif

// timestamp to indicate struct changes in other files
static const char* __GX_MESH_H_TIMESTAMP__ = strdup(__TIMESTAMP__);

//*************************************
// OpenGL forward declarations
#ifndef GL_TRIANGLES
	#define GL_TRIANGLES 0x0004
#endif
namespace gl { struct Buffer; struct VertexArray; struct Texture; } 
// D3DX forward decl.
struct ID3D10Buffer; struct ID3D10ShaderResourceView; struct ID3D11Buffer; struct ID3D11ShaderResourceView;
// mesh forward decl.
struct mesh; struct object;
// acceleration structure types defined in <gxrt.h>
struct acc_t; struct bvh_t; struct kdtree_t;

//*************************************
// 16-byte aligned light for direct lights
#ifndef __cplusplus // std140 definition for shaders
struct light_t { vec4 pos, color, normal; }		// normal: use only normal.xyz
#else
struct light_t
{
	vec4	pos;			// directional light (position.a==0) ignores normal
	vec4	color;			// shared for diffuse/specular; color.a = specular scale
	vec3	normal;			// direction (the negated vector of position for directional light)
	uint	geometry:24;	// [00:23] ID of the geometry that is bound to this light
	uint	bounce:7;		// [24:30] zero means emissive light; non-zero means VPLs
	uint	mouse:1;		// [31:31] dynamic binding to the mouse?

	light_t(){ normal=vec3(0); geometry=0xffffffff; bounce=0; mouse=0;  }
	vec3	dir() const { return normalize(pos.xyz); }				// directions and angles against view vector
	float	phi() const { vec3 d=dir(); return atan2(d.y,d.x); }	// angle on the xy plane orthogonal to the optical axis
	float	theta() const { return acos(dir().z); }					// angle between outgoing light direction and the optical axis
};
static_assert(sizeof(light_t)%16==0, "size of struct light_t should be aligned at 16-byte boundaries");
inline string STR_GLSL_STRUCT_LIGHT = "struct light_t { vec4 pos, color, normal; }; // normal: use only normal.xyz\n"; // std140 definition for shaders
#endif

// overloading light transformations to camera space
inline light_t operator*( const mat4& view_matrix, const light_t& light ){ light_t l=light; l.pos.xyz=l.pos.a==0?(mat3(view_matrix)*l.pos.xyz):(view_matrix*l.pos).xyz; l.normal=mat3(view_matrix)*light.normal; return l; }
inline vector<light_t> operator*( const mat4& view_matrix, const vector<light_t>& lights ){ vector<light_t> v; if(lights.empty()) return v; v.reserve(lights.size()); for( const auto& l : lights ) v.emplace_back(view_matrix*l); return v; }
inline vector<light_t> operator*( const mat4& view_matrix, const vector<light_t>* lights ){ vector<light_t> v; if(!lights||lights->empty()) return v; v.reserve(lights->size()); for(auto& l:*lights) v.emplace_back(view_matrix*l); return v; }

//*************************************
// ray for ray tracing
struct ray
{
	union { struct { vec3 pos, dir; vec4 tex; }; struct { vec3 o, d; float t, tfar, time; int depth; }; }; // lens system rays (with texcoord) or pbrt-like rays (tnear/tfar = parameters of the nearest/farthest intersections)
	ray(): t(0), tfar(FLT_MAX), time(0), depth(0){}
	ray(const vec3& _pos, const vec3& _dir, float _tnear=0.0f, float _tfar=FLT_MAX): ray(){ pos=_pos; dir=_dir; t=_tnear; tfar=_tfar; }
	ray(const vec3& _pos, const vec3& _dir, const vec4& _tex): ray(){ pos=_pos; dir=_dir; tex=_tex; }
};

//*************************************
// intersection
struct isect
{
	vec3	pos={};						// position at intersection
	vec3	norm={};					// face normal at intersection
	float	t=FLT_MAX;					// nearest intersection: t<0 indicates inverted intersection on spherical surfaces
	union{float tfar=FLT_MAX,theta;};	// farthest intersection (gxut) or incident angle (oxut)
	uint	g=0xffffffff;				// index of the geometry at the intersection
	vec3	vnorm={};					// vertex normal at intersection
	vec2	tc={};						// texture coordinate
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
	bbox( bbox&& b ) noexcept { m=b.m; M=b.M; }
	bbox( const bbox& b ){ m=b.m; M=b.M; }
	bbox( const vec3& v0, const vec3& v1 ){ m=vec3(min(v0.x, v1.x), min(v0.y, v1.y), min(v0.z, v1.z)); M=vec3(max(v0.x, v1.x), max(v0.y, v1.y), max(v0.z, v1.z)); }
	bbox( const vec3& v0, const vec3& v1, const vec3& v2 ){ m=vec3(min(min(v0.x, v1.x), v2.x), min(min(v0.y, v1.y), v2.y), min(min(v0.z, v1.z), v2.z)); M=vec3(max(max(v0.x, v1.x), v2.x), max(max(v0.y, v1.y), v2.y), max(max(v0.z, v1.z), v2.z)); }
	bbox( const bbox& b0, const bbox& b1 ){ m=vec3(min(b0.m.x, b1.m.x), min(b0.m.y, b1.m.y), min(b0.m.z, b1.m.z)); M=vec3(max(b0.M.x, b1.M.x), max(b0.M.y, b1.M.y), max(b0.M.z, b1.M.z)); }
	void clear(){ M=-(m=FLT_MAX); }

	// assignment
	bbox& operator=( bbox&& b ) noexcept { m=b.m; M=b.M; return *this; }
	bbox& operator=( const bbox& b ){ m=b.m; M=b.M; return *this; }
	bbox& operator=( bbox_t&& b ){ m=b.m; M=b.M; return *this; }
	bbox& operator=( const bbox_t& b ){ m=b.m; M=b.M; return *this; }

	// array access
	inline const vec3& operator[]( ptrdiff_t i ) const { return (&m)[i]; }
	inline vec3& operator[]( ptrdiff_t i ){ return (&m)[i]; }

	// type cast
	operator bool() const { return (m.x+0.00001f)<M.x&&(m.y+0.00001f)<M.y&&(m.z+0.00001f)<M.z; }

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
	inline array<uvec3,8> corner_indices() const { array<uvec3,8> v={}; for(uint k=0; k<8; k++) v[k]=corner_index(k); return v; }
	inline vec3 corner( uint index ) const { uvec3 i=corner_index(index); return lerp(float(i.x), float(i.y), float(i.z)); }
	inline array<vec3,8> corners() const { array<vec3,8> v={}; for(uint k=0; k<8; k++) v[k]=corner(k); return v; }	// returns 000 100 110 010 001 101 111 011

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
	bool intersect( ray r );
	bool intersect( ray r, isect& h );
};

inline bbox operator*(const mat4& m, const bbox& b){ return bbox(b).expand(m); }
inline string STR_GLSL_STRUCT_BBOX = "struct bbox { vec4 m; vec4 M; }; // bounding box in std140 layout\n";
#endif

//*************************************
// sphere
struct sphere
{
	vec3	pos;
	float	radius;
	bool	intersect( ray r, isect& h );
};

//*************************************
// view frustum for culling
struct frustum_t : public array<vec4, 6> // left, right, top, bottom, near, far
{
	__forceinline frustum_t() = default;
	__forceinline frustum_t( frustum_t&& ) = default;
	__forceinline frustum_t( const frustum_t& ) = default;
	__forceinline frustum_t( const mat4& view_projection_matrix ){ update(view_projection_matrix); }
	__forceinline frustum_t& operator=( frustum_t&& ) = default;
	__forceinline frustum_t& operator=( const frustum_t& ) = default;
	__forceinline frustum_t& update( const mat4& view_projection_matrix ){ mat4 r=view_projection_matrix.transpose(); auto* p=data();for(int k=0;k<6;k++){ p[k]=r[3]+r[k>>1]*float(1-(k&1)*2);p[k]/=p[k].xyz.length();} return *this; }
	__forceinline bool cull( const bbox_t& b) const { vec4 pv={}; pv.w=1.0f; for(int k=0;k<6;k++){ const vec4& plane=operator[](k); for(int j=0;j<3;j++)pv[j]=plane[j]>0?b.M[j]:b.m[j]; if(pv.dot(plane)<0)return true;} return false; }
};

//*************************************
// Thin-lens camera definition
#ifndef __cplusplus
struct camera_t // std140 layout for OpenGL uniform buffer objects
{
	mat4	view_matrix, projection_matrix;
	float	fovy, aspect, dnear, dfar;
	vec4	eye, dir, up; // 16-bytes aligned for std140 layout
	float	focal, fy, E, df;
};
#else
struct camera_t // 16-bytes aligned for std140 layout
{
	mat4 view_matrix, projection_matrix;
	float fovy, aspect, dnear, dfar; // fov in radians; height for orthographic projection
	vec3 eye;	float height=0;
	vec3 dir;	float fovy_degrees=30.0f;
	vec3 up;	float fn=0;
	alignas(16) float focal=0;	// focal length (in mm); alignas should apply to a SINGLE float
	float fy=0, E=0, df=0;		// focal length (in pixels), lens radius, focusing depth (in object distance)

	camera_t() = default;
	camera_t(camera_t&& c) = default;
	camera_t(const camera_t& c) = default;
	camera_t& operator=(camera_t&& c) = default;
	camera_t& operator=(const camera_t& c) = default;

	mat4 view_projection_matrix() const { return projection_matrix*view_matrix; }
	mat4 inverse_view_matrix() const { return mat4::look_to_inverse(eye,dir,up); } // works without eye, center, up
	mat4 perspective_dx() const { mat4 m=projection_matrix; m._22=dfar/(dnear-dfar); m._23*=0.5f; return m; } // you may use mat4::perspectiveDX() to set canonical depth range in [0,1] instead of [-1,1]
	vec2 plane_size( float ecd=1.0f) const { return vec2(2.0f/projection_matrix._00, 2.0f/projection_matrix._11)*ecd; } // plane size (width, height) at eye-coordinate distance 1
	void update_depth_clips( const bbox& bound, const float min_near_scale=0.0005f ){ bbox b=view_matrix*bound; vec2 z(max(0.001f,-b.M.z),max(0.001f,-b.m.z)); float r=bound.radius(); dnear=max(max(r*min_near_scale,0.05f),z.x*0.99f); dfar=max(max(dnear+1.0f,dnear*1.01f),z.y*1.01f); }
	void extend_frustum( double scale ){ projection_matrix=mat4::perspective(fovy=float(atan2(tan(fovy*0.5)*scale,1.0)*2.0),aspect,dnear,dfar); }

	// lens attributes
	float	coc_norm_scale() const { float E=focal/(fn==0?4.0f:fn)*0.5f; return E/df/tan(fovy*0.5f); } // normalized coc scale in the screen space; E: lens_radius; when fn==0, use default for 4
	float	coc_scale( int height ) const { return coc_norm_scale()*float(height)*0.5f; } // screen-space coc scale; so called "K" so far
};
static_assert(sizeof(camera_t)%16==0, "sizeof(camera_t) should be aligned at 16-byte boundary");
static_assert(sizeof(camera_t)==208, "sizeof(camera_t)!=208"); // manual size check
inline string STR_GLSL_STRUCT_CAMERA = "struct camera_t { mat4 view_matrix, projection_matrix; float fovy, aspect, dnear, dfar; vec4 eye, dir, up; float focal, fy, E, df; };\n";
#endif

struct campreset; // forward decls.
struct camera : public camera_t
{
	camera*		last=nullptr;		// current to last
	camera*		next=nullptr;		// last to current
	int			frame = RAND_MAX;	// frame used for this camera; used in a motion tracer
	frustum_t	frustum;			// view frustum for culling
	char		name[PATH_MAX]={};	// loaded preset name (e.g., image name) if any
	campreset*	preset=nullptr;		// a preset bound to this camera
};

struct campreset
{
	uint					id=-1, crc=0;
	string					name;
	typed_string<float>		fovy, aspect, dnear, dfar;
	typed_string<vec3>		eye, at, up, dir;
	typed_string<float>		width, height, fn;
	typed_string<float>		focal, fx, fy, E, df;

	__noinline void clear(){ crc=0; id=-1; name.clear(); fovy.clear(); aspect.clear(); dnear.clear(); dfar.clear(); eye.clear(); at.clear(); up.clear(); dir.clear(); width.clear(); height.clear(); fn.clear(); focal.clear(); fx.clear(); fy.clear(); E.clear(); df.clear(); }
	bool empty() const { return crc==0; }
};

struct stereo_t
{
	enum model_t {NONE=0,LEFT=1,RIGHT=2,BOTH=3}; // bitwise-and with left/right indicates left/right is drawn; ALTER renders left/right for even/odd frames
	uint		model = 0;			// stereo-rendering model
	float		ipd = 64.0f;		// inter-pupil distance for stereoscopy: 6.4 cm = men's average
	camera_t	left, right;		// left, right cameras
	void		update( const camera& c ){ if(!model) return; float s=0.5f*ipd, o=s*c.dnear/c.df, t=c.dnear*tanf(0.5f*c.fovy*(c.fovy<pi?1.0f:pi/180.0f)), R=t*c.aspect; vec3 stereo_dir = normalize(cross(c.dir,c.up))*s; auto& l=left; memcpy(&l,&c,sizeof(l)); l.eye-=stereo_dir; l.dir-=stereo_dir; l.view_matrix=mat4::look_to(l.eye,l.dir,l.up); l.projection_matrix=mat4::perspective_off_center(-R+o,R+o,t,-t,c.dnear,c.dfar); auto& r=right; memcpy(&r,&c,sizeof(r)); r.eye+=stereo_dir; r.dir+=stereo_dir; r.view_matrix=mat4::look_to(r.eye,r.dir,r.up); r.projection_matrix=mat4::perspective_off_center(-R-o,R-o,t,-t,c.dnear,c.dfar);}
};

// utilities for camera
inline float focal_to_fov( float focal, float image_size ){ return focal==0?0:atan2(image_size*0.5f,focal)*2.0f; }
inline float fov_to_focal( float fov, float image_size ){ return fov==0?0:image_size*0.5f/tan(fov*0.5f); }

//*************************************
// material definition (std140 layout, aligned at 16-byte/vec4 boundaries)
#ifndef __cplusplus
struct material { vec4 color; uint bsdf; float metal, rough, beta, specular, n; uvec2 TEX, NRM, PBR; };
#else
// gloss (specular, rough conductor), dielectric (glass)
enum bsdf_t { BSDF_EMISSIVE=0, BSDF_DIFFUSE=1, BSDF_GLOSS=2, BSDF_MIRROR=4, BSDF_DIELECTRIC=8, BSDF_FRESNEL=16 };
struct material
{
	vec4		color={};			// albedo or Blinn-Phong diffuse; color.a=opacity=1-transmittance
	uint		bsdf=BSDF_DIFFUSE;	// bitwise combination: diffuse by default
	float		metal=0.0f;			// metallic: mapped to specular intensity
	float		rough=0.2f;			// roughness: mapped to specular power; zero means mirror-reflective (used for ray tracing)
	float		beta=48.0f;			// specular power of Phong model
	float		specular=1.0f;		// specular intensity
	float		n=1.0f;				// refractive index
	uint64_t	TEX=0;				// GPU handle to (albedo,alpha) texture; RGBA format indicates the presence of alpha/opacity channel
	uint64_t	NRM=0;				// GPU handle to normal map
	uint64_t	PBR=0;				// GPU handle to PBR texture: (ambient occlusion,roughness,metallic), where RM follows glTF spec
};
static_assert(sizeof(material)%16==0, "struct material must be aligned at a 16-byte boundary");
inline string STR_GLSL_STRUCT_MATERIAL = "struct material { vec4 color; uint bsdf; float metal, rough, beta, specular, n; uvec2 TEX, NRM, PBR; };\n";
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
	std::map<string,path_t> path;	// <key,path>: albedo, alpha, normal, ambient, rough, metal, emissive, ...

	material_impl( uint id ):ID(id){}
	material_impl& operator=(const material_impl& other){ memcpy(this,&other,sizeof(material)); const_cast<uint&>(ID)=other.ID; strcpy(name,other.name); texture=other.texture; path=other.path; return *this; }
};

//*************************************
// cull data definition
struct cull_t
{
	enum model_t { NONE=0, USER=1<<0, REF=1<<1, VFC=1<<2, CHCPP=1<<3, WOC=1<<4, ROC=1<<5, HROC=1<<6, DROC=1<<7 };
	static const char* name( model_t m ){ return !m?"NONE":m&USER?"USER":m&VFC?"VFC":m&CHCPP?"CHCPP":m&WOC?"WOC":m&ROC?"ROC":m&HROC?"HROC":m&DROC?"DROC":"UNKNOWN"; }
	uchar data = 0; // bits of the corresponding culling types are set (bitwise OR-ed)

	inline operator uchar() const { return data; }
	inline bool operator()(uchar m) const { return (data&m)!=0; }
	inline cull_t& reset(uchar m=0xff){ data = (m==0xff)?0:(data&~m); return *this; }
	inline cull_t& set(uchar m){ data|=m; return *this; }
};

//*************************************
// geometry: 144-bytes aligned; differentiated by material and grouping
#ifndef __cplusplus
struct geometry // std430 layout for OpenGL shader storage buffers
{
	uint	count, instance_count, first_index, base_vertex, base_instance;		// DrawElementsIndirectCommand
	uint	material_index, acc_index, acc_prim_count, acc_prim_index, pad[3];	// geometry-bvh node index, primitive-bvh count/first_index
	bbox	box; // object_index = floatBitsToUint(box.M.a)
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

	object*		parent() const;
	const char*	name() const;
	void*		offset() const { return (void*)(sizeof(geometry)*ID); } // for rendering commands
	uint		face_count() const { return count/3; }
	float		surface_area() const;
	bool		intersect( ray r, isect& h ) const; // linear intersection
	bool		acc_empty() const { return acc_prim_count==0; }
};
static_assert(sizeof(geometry)%16==0,	"sizeof(geometry) should be multiple of 16-byte");
static_assert(sizeof(geometry)==144,	"sizeof(geometry) should be 144, when aligned correctly");
inline string STR_GLSL_STRUCT_GEOMETRY = "struct geometry { uint count, instance_count, first_index, base_vertex, base_instance; uint material_index, acc_index, acc_prim_count, acc_prim_index, pad[3]; bbox box; mat4 mtx; };\n";
#endif

//*************************************
// unified glsl includes
inline string STR_GLSL_STRUCT_GBUFFER = "struct gbuffer_t { vec4 albedo; vec3 normal, epos; int item; };\n";
inline string STR_GLSL_STRUCT_MESH = STR_GLSL_STRUCT_CAMERA+STR_GLSL_STRUCT_LIGHT+STR_GLSL_STRUCT_BBOX+STR_GLSL_STRUCT_MATERIAL+STR_GLSL_STRUCT_GEOMETRY+STR_GLSL_STRUCT_GBUFFER;

//*************************************
// a set of geometries for batch control (not related to the rendering)
struct object
{
	uint			ID=-1;
	uint			instance=0;
	char			name[_MAX_PATH]={};
	mesh*			root=nullptr;
	bbox			box;
	vector<uint>	geometries; // child geometry indices
	struct {bool dynamic=false,bg=false;} attrib; // dynamic: potential matrix changes; background: backdrops such as floor/ground; set in other plugins (e.g., AnimateMesh)

	object() = delete;  // no default ctor to enforce to assign parent
	object( mesh* p_mesh ):root(p_mesh){}
	object( mesh* p_mesh, uint id, const char* name ):ID(id), root(p_mesh){ strcpy(this->name, name); }

	// query and attributes
	inline bool empty() const { return geometries.empty(); }
	inline uint size() const { return uint(geometries.size()); }
	inline uint face_count() const { uint n=0; for( auto& g : *this ) n+=g.face_count(); return n; }
	inline bbox update_bound(){ box.clear(); for( auto& g : *this ) box.expand(g.mtx*g.box); return box; }

	// create helpers
	geometry* create_geometry( size_t first_index, size_t index_count=0, const bbox* box=nullptr, size_t mat_index=-1 );
	geometry* create_geometry( const geometry& other );

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
	uint					crc=0;		// crc hash to trigger on_dirty_mesh() callbacks
	vector<vertex>			vertices;
	vector<uint>			indices;
	vector<object>			objects;
	vector<geometry>		geometries;
	vector<material_impl>	materials;
	vector<light_t>			lights;		// built-in object lights

	// volitile spaces for GPU buffers
	union buffers_t {
		struct { gl::VertexArray* vertex; gl::Buffer *geometry, *count; }; // vertex array, geometry/command buffer, count buffer for OpenGL
		struct { ID3D10Buffer *vertex, *index; } d3d10; // vertex/index buffers for D3D
		struct { ID3D11Buffer *vertex, *index; } d3d11; // vertex/index buffers for D3D
	} buffer = {};

	// acceleration and dynamics
	bbox	box;
	union { acc_t* acc=nullptr; bvh_t* bvh; kdtree_t* kdtree; }; // BVH or KD-tree

	// instancing
	uint	instance_count=1;	// instances are physically added into objects

	// proxy mesh
	mesh*	proxy=nullptr;		// created and released in GL_Mesh 

	// auxiliary information
	char	file_path[PATH_MAX]={};	// mesh file path (e.g., *.obj, *.obj.7z)
	char	mtl_path[PATH_MAX]={};		// material file path (e.g., *.mtl)

	// constructor
	mesh(){ vertices.reserve(size_t(1<<20)); indices.reserve(size_t(1<<20)); objects.reserve(size_t(1<<16)); geometries.reserve(size_t(1<<16)); }
	virtual ~mesh(){ release(); }

	// release/memory
	void release(){ vertices.clear(); indices.clear(); geometries.clear(); objects.clear(); materials.clear(); shrink_to_fit(); }
	mesh* shrink_to_fit(){ vertices.shrink_to_fit(); indices.shrink_to_fit(); geometries.shrink_to_fit(); objects.shrink_to_fit(); if(materials.capacity()>materials.size()){ decltype(materials) t=materials; materials.swap(t); } return this; }

	// face/object/geometry/proxy/material/light helpers
	uint face_count() const { uint f=0; for(const auto& g:geometries) f+=g.count; return f/3; }
	uint vertex_count() const { return uint(vertices.size())*instance_count; }
	object* create_object( const char* name ){ objects.emplace_back(object(this, uint(objects.size()), name)); return &objects.back(); }
	object*	find_object( const char* name ){ for(uint k=0; k<objects.size(); k++)if(stricmp(objects[k].name,name)==0) return &objects[k]; return nullptr; }
	vector<object*> find_objects( const char* name ){ vector<object*> v; for(uint k=0; k<objects.size(); k++)if(stricmp(objects[k].name,name)==0) v.push_back(&objects[k]); return v; }
	mesh* create_proxy( bool double_sided=false, bool use_quads=false );	// proxy mesh helpers: e.g., bounding box
	void update_proxy();													// update existing proxy with newer matrices
	vector<material> pack_materials() const { vector<material> p; auto& m=materials; p.resize(m.size()); for(size_t k=0,kn=p.size();k<kn;k++) p[k]=m[k]; return p; }
	void dump_binary( const char* dir=""); // dump the vertex/index buffers as binary files

	// bound, dynamic, intersection
	inline bool is_dynamic() const { for(size_t k=0, kn=objects.size()/instance_count; k<kn; k++) if(objects[k].attrib.dynamic) return true; return false; }
	void update_bound( bool b_recalc_tris=false );
	bool intersect( ray r, isect& h, bool use_acc=true ) const;

	// opengl draw functions (implemented in gxopengl.h)
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
// volume data format
struct volume
{
	char	file_path[_MAX_PATH]={};
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
	geometries.emplace_back(gid);
	return &v.back();
}

inline geometry* object::create_geometry( const geometry& other )
{
	auto& v = root->geometries; uint gid=uint(v.size());
	v.emplace_back(other);
	auto* g=&v.back(); g->ID=gid; geometries.emplace_back(gid); return g;
}

inline object::iterator::reference object::iterator::operator*(){ auto g=root->objects[parent].geometries[index]; return root->geometries[g]; }
inline object::iterator::pointer object::iterator::operator->(){ auto g=root->objects[parent].geometries[index]; return &root->geometries[g]; }
inline geometry& object::front(){ return root->geometries[geometries.front()]; }
inline geometry& object::back(){ return root->geometries[geometries.back()]; }
inline geometry& object::operator[]( size_t i ){ return root->geometries[i]; }

// late implementations for mesh
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

// find an up vector from box or mesh
__noinline int find_up_vector( const bbox& box )
{
	if(box.max_extent()>(box.min_extent()*4.0f)) return box.min_axis();
	return 2;
}

__noinline int find_up_vector( const mesh* p_mesh )
{
	if(!p_mesh) return 2;
	if(p_mesh->box.max_extent()>(p_mesh->box.min_extent()*4.0f)) return p_mesh->box.min_axis();

	ivec3 d=0;
	for(auto f:{"floor","ground","ceil","terrain","plane"})
	for(auto& o:p_mesh->objects)
	{
		char buff[4096]; strcpy(buff,o.name);
		if(!strstr(strlwr(buff),f)||o.box.max_extent()<(o.box.min_extent()*4.0f)) continue;
		d[o.box.min_axis()]++; break;
	}
	
	int a=2;
	if(d[0]>0&&d[0]>d[2]) a=0;
	if(d[1]>0&&d[1]>d[a]) a=1;
	return a;
}

__noinline vector<uint> get_box_indices( bool double_sided, bool quads )
{
	vector<uint> v; // index definitions (CCW: counterclockwise by default)
	if(quads)	v = { 0, 3, 2, 1, /*bottom*/ 4, 5, 6, 7, /*top*/ 0, 1, 5, 4, /*left*/ 2, 3, 7, 6, /*right*/ 1, 2, 6, 5, /*front*/ 3, 0, 4, 7 /*back*/ };
	else		v = { 0, 2, 1, 0, 3, 2, /*bottom*/ 4, 5, 6, 4, 6, 7, /*top*/ 0, 1, 5, 0, 5, 4, /*left*/ 2, 3, 6, 3, 7, 6, /*right*/ 1, 2, 6, 1, 6, 5, /*front*/ 0, 4, 3, 3, 4, 7 /*back*/ };
	if(double_sided){ for(size_t k=0, f=quads?4:3, kn=v.size()/f; k<kn; k++) for(size_t j=0; j<f; j++) v.emplace_back(v[(k+1)*f-j-1]); } // insert indices (for CW)
	return v;
}

__noinline vector<uint> get_box_line_indices()
{
	return vector<uint>{
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
	vector<uint> indices = std::move(get_box_indices( double_sided, quads ));
	m->indices.insert(m->indices.end(),indices.begin(),indices.end());
	m->create_object(name)->create_geometry(0,uint(m->indices.size()),(bbox*)&box,size_t(-1));

	// create box line elements
	vector<uint> line_indices = std::move(get_box_line_indices());
	m->indices.insert(m->indices.end(),line_indices.begin(),line_indices.end());
	char buff[4096]; sprintf(buff,"%s.lines",name);
	m->create_object(buff)->create_geometry(uint(indices.size()),uint(line_indices.size()),(bbox*)&box, size_t(-1));

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
	strcpy(proxy->file_path, file_path);

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
		for( uint k=0; k<8; k++ ){ auto& v = proxy->vertices[size_t(8llu*g.ID+k)]; v.pos = (m*vec4(v.norm,1.0f)).xyz; }
	}
}

inline void mesh::dump_binary( const char* _dir )
{
	if(vertices.empty() || indices.empty()) return;
	path_t dir = path_t(_dir).append_slash(); if(!dir.exists()) dir.mkdir();
	path_t vertex_bin_path = dir + path_t(file_path).stem2() + ".vertex.bin";
	path_t index_bin_path = dir + path_t(file_path).stem2() + ".index.bin";
	FILE* fp = fopen(vertex_bin_path.c_str(),"wb"); fwrite(&vertices[0], sizeof(vertex), vertices.size(), fp); fclose(fp);
	fp = fopen(index_bin_path.c_str(),"wb");  fwrite(&indices[0], sizeof(uint), indices.size(), fp); fclose(fp);
}

#if defined(__GX_OPENGL_H__)&&!defined(__GX_MESH_OPENGL_IMPL__)
#define __GX_MESH_OPENGL_IMPL__
inline void mesh::draw_arrays( GLint first, GLsizei count, GLenum mode, bool b_bind ){ buffer.vertex->draw_arrays(first,count,mode,b_bind); }
inline void mesh::draw_arrays_instanced( GLint first, GLsizei instance_count, GLsizei count, GLenum mode, bool b_bind ){ buffer.vertex->draw_arrays_instanced( first, instance_count, count, mode, b_bind ); }
inline void mesh::draw_elements( GLuint first, GLsizei count, GLenum mode, bool b_bind ){ buffer.vertex->draw_elements( first, count, mode, b_bind ); }
inline void mesh::draw_elements_instanced( GLuint first, GLsizei instance_count, GLsizei count, GLenum mode, bool b_bind ){ buffer.vertex->draw_elements_instanced( first, instance_count, count, mode, b_bind ); }
inline void mesh::draw_elements_indirect( GLvoid* indirect, GLenum mode, bool b_bind ){ buffer.vertex->draw_elements_indirect( indirect, mode, b_bind ); }
inline void mesh::draw_range_elements( GLuint first, GLuint end, GLsizei count, GLenum mode, bool b_bind ){ buffer.vertex->draw_range_elements( first, end, count, mode, b_bind ); }
inline void mesh::multi_draw_elements( GLuint* pfirst, const GLsizei* pcount, GLsizei draw_count, GLenum mode, bool b_bind ){ buffer.vertex->multi_draw_elements( pfirst, pcount, draw_count, mode, b_bind ); }
inline void mesh::multi_draw_elements_indirect( GLsizei draw_count, GLsizei stride, GLvoid* indirect, GLenum mode, bool b_bind ){ buffer.vertex->multi_draw_elements_indirect( draw_count, stride, indirect, mode, b_bind ); }
inline void mesh::multi_draw_elements_indirect_count( GLsizei max_draw_count, GLsizei stride, const void* indirect, GLintptr draw_count, GLenum mode, bool b_bind ){ buffer.vertex->multi_draw_elements_indirect_count( max_draw_count, stride, indirect, draw_count, mode, b_bind ); }
inline gl::VertexArray* gxCreateVertexArray( const char* name, mesh* p_mesh, GLenum usage=GL_STATIC_DRAW ){ return p_mesh?gxCreateVertexArray(name,&p_mesh->vertices[0],p_mesh->vertices.size(),&p_mesh->indices[0],p_mesh->indices.size(),usage):nullptr; }
#endif

//*************************************
namespace gs { // extension to Gaussian Splatting
//*************************************
struct vertex
{
	vec3	pos;	float opacity;
	vec3	norm;
	alignas(16) vec3	dc;			// f_dc: (feature) diffuse color; dc.a: padding
	alignas(16) vec4	cov[2];		// cov.a: padding
	alignas(16) float	sh[45];		// f_rest: (feature) spherical harmonics coefficients
private:
	float	_2[3];
};
static_assert(sizeof(gs::vertex)%16==0,"sizeof(gs::vertex)%16!=0");

struct cloud
{
	uint			crc=0;			// crc hash to trigger on_dirty callbacks
	path			file_path;		// file path (e.g., *.ply)
	vector<vertex>	vertices;		// vertices
	vector<uint>	indices;		// sorted vertex indices
	bbox			box;			// scene bound

	// volitile variables
	uint			sh_degrees=4;	// degrees of spherical harmonics
	struct { gl::Buffer *vertex, *index; } buffer = {};

	cloud(){ vertices.reserve(size_t(1<<20)); indices.reserve(size_t(1<<20)); }
	virtual ~cloud(){ release(); }
	void release(){ vertices.clear(); indices.clear(); }
	size_t size() const { return vertices.size(); }
};

//*************************************
} // end namespace gs
//*************************************

#endif // __GX_MESH_H__
