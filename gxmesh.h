//*******************************************************************
// Copyright 2017 Sungkil Lee
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
//*******************************************************************

#pragma once
#ifndef __GX_MESH__
#define __GX_MESH__

#include "gxmath.h" // the only necessary header for gxmesh

//***********************************************
// forward declarations
namespace gl { struct Buffer; struct VertexArray; struct Texture;}											// OpenGL forward decl.
struct ID3D10Buffer; struct ID3D11Buffer; struct ID3D10ShaderResourceView; struct ID3D11ShaderResourceView; // D3DX forward decl.
struct geometry; struct object; struct mesh;																// mesh forward decl.

//***********************************************
// sampler interface (for lens/rays): vec4(x,y,z,weight) in a unit surface
template <size_t max_samples>
struct tsampler_t
{
	enum model_t {	SIMPLE, STRATIFIED, POISSON, HAMMERSLEY, HALTON };
	enum surface_t{	SQUARE, CIRCLE, HEMISPHERE, SPHERE, CYLINDER };
	using array_t = const std::array<vec4,max_samples>;
	
	model_t			model;
	surface_t		surface;
	uint			seed;	// random seed
	uint			crc;	// crc32c to detect the change of samples
	
	bool			empty() const { return n==0; }
	uint			size() const { return n; }
	const vec4*		begin() const { return &data[0]; }
	const vec4*		end() const { return begin()+n; }
	uint			capacity() const { return uint(data.max_size()); }
	const vec4&		operator[]( size_t i ) const { return data[i]; }
	const vec4&		at( size_t i ) const { return data[i]; }
	void			resize( uint new_size, bool b_resample=true ){ const_cast<uint&>(n)=min(new_size,max_samples); if(b_resample) resample(); }
	virtual uint	resample()=0; // return the number of generated samples; implemented in Sampler plugin

protected:

	array_t			data;
	const uint		n=1;
};

using sampler_t = tsampler_t<1<<16>; // up to 64K samples

//***********************************************
// 16-byte aligned light for direct and virtual point lights
#ifndef __cplusplus // definition for shaders
struct light_t { vec4 position, color, normal; }; // normal should use xyz
#else
struct light_t
{
	vec4 position;	// directional light (position.a==0) ignores normal
	vec4 color;		// shared for diffuse/specular
	vec3 normal;	// direction (the negated vector of position for directional light)
	uint depth:8;	// vpl bounces (depth>0: VPLs; otherwise, a real light)
	int	 bind:24;	// ID of object bound to this light (negative means static lights)

	// transformation to camera (eye-coordinate) space
	vec4 ecpos( mat4& view_matrix ){ return position.a==0?vec4(mat3(view_matrix)*position.xyz,0.0f):view_matrix*position; }
	vec3 ecnorm( mat4& view_matrix ){ return mat3(view_matrix)*normal; }
};
static_assert(sizeof(light_t)%16==0,"size of struct light_t should be aligned at 16-byte boundary" );
#endif

//***********************************************
// ray
template <class T=float> struct tray_t // template used to avoid non-trivial constructors in unions
{
	union{struct{tvec3<T> pos,dir;tvec4<T> tex;};struct{tvec3<T> o,d;float t,tfar,time;int depth;};}; // lens system rays (with texcoord) or pbrt-like rays (tnear/tfar = parameters of the nearest/farthest intersections)
	tray_t():t(0.0f),tfar(FLT_MAX),time(0.0f),depth(0){}
	tray_t( const tvec3<T>& _pos, const tvec3<T>& _dir, float _tnear=0.0f, float _tfar=FLT_MAX ):tray_t(){ pos=_pos; dir=_dir; t=_tnear; tfar=_tfar; }
};
using ray = tray_t<float>;

//***********************************************
// intersection
struct isect
{
	vec3	pos;			// position at intersection
	vec3	norm;			// normal at intersection
	vec2	bc;				// barycentric coordinates at t
	float	t=FLT_MAX;		// (nearest,farthest) intersections
	float	tfar=FLT_MAX;	// (nearest,farthest) intersections
	uint	g=-1;			// index of an intersected geometry
	bool	hit=0, _[3];	// is intersected? and padding
};

//***********************************************
// bounding box: 16-bytes aligned for std140 layout
#ifndef __cplusplus
struct bbox { vec4 m; vec4 M; }; // bounding box in std140 layout
#else
struct bbox
{
	alignas(16) vec3 m;
	alignas(16) vec3 M;

	bbox(){ clear(); }
	bbox( const vec3& v0 ){ m=M=v0; }
	bbox( bbox&& b ) = default;
	bbox( const bbox& b ){ m=b.m; M=b.M; }
	bbox( const vec3& v0, const vec3& v1 ){ m=vec3(min(v0.x,v1.x),min(v0.y,v1.y),min(v0.z,v1.z)); M=vec3(max(v0.x,v1.x),max(v0.y,v1.y),max(v0.z,v1.z)); }
	bbox( const vec3& v0, const vec3& v1, const vec3& v2 ){ m=vec3(min(min(v0.x,v1.x),v2.x),min(min(v0.y,v1.y),v2.y),min(min(v0.z,v1.z),v2.z)); M=vec3(max(max(v0.x,v1.x),v2.x),max(max(v0.y,v1.y),v2.y),max(max(v0.z,v1.z),v2.z)); }
	bbox( const bbox& b0, const bbox& b1 ){ m=vec3(min(b0.m.x,b1.m.x),min(b0.m.y,b1.m.y),min(b0.m.z,b1.m.z)); M=vec3(max(b0.M.x,b1.M.x),max(b0.M.y,b1.M.y),max(b0.M.z,b1.M.z)); }
	inline void clear(){ M=-(m=FLT_MAX*0.0001f); }

	// assignment
	bbox& operator=( bbox&& b ) = default;
	bbox& operator=( const bbox& b ) = default;

	// array access
	inline const vec3& operator[]( int i) const { return (&m)[i]; }
	inline vec3& operator[]( int i ){ return (&m)[i]; }

	// query
	inline vec3 center() const { return (M+m)*0.5f; }
	inline vec3 size() const { return (M-m); }
	inline vec3 extent() const { return (M-m); }
	inline float radius() const { return size().length()*0.5f; }
	inline float surface_area() const { vec3 e=size(); return (e.x*e.z+e.y*e.x+e.z*e.y)*2.0f; }
	inline float volume() const { vec3 e=size(); return e.x*e.y*e.z; }
	inline bool overlap( const bbox& b ) const { return (M.x>=b.m.x)&&(m.x<=b.M.x)&&(M.y>=b.m.y)&&(m.y<=b.M.y)&&(M.z>=b.m.z)&&(m.z<=b.M.z); }
	inline int max_extent() const { vec3 e=size(); return e.x>e.y&&e.x>e.z?0:e.y>e.z?1:2; }

	// query on points
	inline bool include( const vec3& v ) const { return v.x>=m.x&&v.x<=M.x&&v.y>=m.y&&v.y<=M.y&&v.z>=m.z&&v.z<=M.z; }
	inline vec3 lerp( float tx, float ty, float tz ) const { return vec3(::lerp(m.x,M.x,tx),::lerp(m.y,M.y,ty),::lerp(m.z,M.z,tz)); }
	inline vec3 lerp( const vec3& t ) const { return vec3(::lerp(m.x,M.x,t.x),::lerp(m.y,M.y,t.y),::lerp(m.z,M.z,t.z)); }
	inline vec3 offset( const vec3& v ) const { return (v-m)/size(); }
	inline uvec3 corner_index( uint index ) const { uint j=index%4; return uvec3( (j>>1)^(j&1), j/2, index/4 ); }
	inline std::vector<uvec3> corner_indices() const { std::vector<uvec3> v(8); for(uint k=0;k<8;k++) v[k]=corner_index(k); return v; }
	inline vec3 corner( uint index ) const { uvec3 i=corner_index(index); return lerp( float(i.x), float(i.y), float(i.z) ); }
	inline std::vector<vec3> corners() const { std::vector<vec3> v(8); for(uint k=0;k<8;k++) v[k]=corner(k); return v; }	// returns 000 100 110 010 001 101 111 011

	// comparison
	bool operator==( const bbox& b ) const { return m==b.m && M==b.M; }
	bool operator!=( const bbox& b ) const { return m!=b.m || M!=b.M; }

	// expansion (union)
	inline const bbox& expand( const bbox& b ){ m=vec3(min(m.x,b.m.x),min(m.y,b.m.y),min(m.z,b.m.z)); M=vec3(max(M.x,b.M.x),max(M.y,b.M.y),max(M.z,b.M.z)); return *this; }
	inline const bbox& expand( const vec3& v ){ m=vec3(min(m.x,v.x),min(m.y,v.y),min(m.z,v.z)); M=vec3(max(M.x,v.x),max(M.y,v.y),max(M.z,v.z)); return *this; }
	inline const bbox& expand( const vec3& v0, const vec3& v1, const vec3& v2 ){ m=vec3(min(min(m.x,v0.x),min(v1.x,v2.x)),min(min(m.y,v0.y),min(v1.y,v2.y)),min(min(m.z,v0.z),min(v1.z,v2.z))); M=vec3(max(max(M.x,v0.x),max(v1.x,v2.x)),max(max(M.y,v0.y),max(v1.y,v2.y)),max(max(M.z,v0.z),max(v1.z,v2.z))); return *this; }
	inline const bbox& expand( const mat4& m ){ auto c=corners(); clear(); for( uint k=0;k<c.size();k++) expand(m*c[k]); return *this; }
	
	// transformation
	inline const bbox& scale( float s ){ vec3 c=center(); m=c+(m-c)*s; M=c+(M-c)*s; return *this; }
	inline const bbox& scale( const vec3& v ){ vec3 c=center(); m=c+(m-c)*v; M=c+(M-c)*v; return *this; }
	inline const bbox& scale( float sx, float sy, float sz ){ vec3 c=center(); m=c+(m-c)*vec3(sx,sy,sz); M=c+(M-c)*vec3(sx,sy,sz); return *this; }

	// intersection
	inline bool intersect( const ray& r, isect* pi=nullptr );
};

inline bbox operator*( const mat4& m, const bbox& b ){ bbox b1=b; return b1.expand(m); }
#endif

//***********************************************
// Thin-lens camera definition
#ifndef __cplusplus
struct camera_t // std140 layout for OpenGL uniform buffer objects
{
	mat4	view_matrix, projection_matrix;
	float	fovy, aspect, dnear, dfar;
	vec3	eye, center, up;
};

#else
struct camera_t
{
	mat4		view_matrix, projection_matrix;
	union {		float fovy, height; }; float aspect, dnear, dfar; // fov in radians; height for orthographic projection
	alignas(16)	vec3 eye, center, up;	// lookAt params (16-bytes aligned for std140 layout)

	camera_t() = default;
	camera_t( camera_t&& c ) = default;
	camera_t( const camera_t& c ) = default;
	camera_t& operator=( camera_t&& c ) = default;
	camera_t& operator=( const camera_t& c ) = default;
};
static_assert(sizeof(camera_t)%16==0,"size of struct camera_t should be aligned at 16-byte boundary" );
#endif

struct camera : public camera_t
{
	vec3		dir;					// dir = center - eye (view direction vector)
	float		F, fn, df;				// focal distance, f-number, focusing depth (in object distance)
	int			frame = RAND_MAX;		// frame used for this camera; used in a motion tracer
	vec4		frustum[6];				// view frustum planes: left, right, top, bottom, near, far
	camera_t	prev;					// placeholder for the camera at the previous frame
	
	mat4		inverse_view_matrix() const { return mat4::lookAtInverse(eye,center,up); } // works without eye, center, up
	mat4		perspective_dx() const { mat4 m=projection_matrix; m._33=dfar/(dnear-dfar); m._34*=0.5f; return m; } // you may use mat4::perspectiveDX() to set canonical depth range in [0,1] instead of [-1,1]
	vec2		plane_size( float ecd=1.0f ) const { return vec2(2.0f/projection_matrix._11,2.0f/projection_matrix._22)*ecd; } // plane size (width, height) at eye-coordinate distance 1
	void		update_view_frusta(){ const mat4 m=projection_matrix*view_matrix;for(int k=0;k<6;k++){vec4& p=frustum[k];p=m.rvec4(k>>1)*float(1-(k&1)*2)+m.rvec4(3);p/=p.xyz.length();}} // left, right, bottom, top, near, far
	void		update_depth_clips( const bbox* bound ){ if(!bound) return; bbox b=view_matrix*(*bound); vec2 z(max(0.001f,-b.M.z),max(0.001f,-b.m.z)); dnear=max(max(bound->radius()*0.00001f,50.0f),z.x*0.99f); dfar=max(max(dnear+1.0f,dnear*1.01f),z.y*1.01f); }
	bool		vfc( bbox& b ) const { vec4 pv(0,0,0,1.0f); for(int c=0;c<6;c++){ for(int j=0;j<3;j++) pv[j]=frustum[c][j]>0?b.M[j]:b.m[j]; if(pv.dot(frustum[c])<0) return true; } return false; }
	float		lens_radius() const { return F/fn*0.5f; }
};

//***********************************************
// material definition (std140 layout, aligned at 16-byte/vec4 boundaries)
#ifndef __cplusplus
struct material { vec4 color; float specular, beta, emissive, n; uvec2 TEX, NRM; };
#else
struct material
{
	vec4		color;				// Blinn-Phong diffuse/specular color; color.a = opacity = 1-transmittance
	float		specular=0.0f;		// specular intensity
	float		beta=48.0f;			// specular power/shininess
	float		emissive=0.0f;		// non-zero only for light sources (use color*emissive for its true color)
	float		n=1.0f;				// refractive index
	uint64_t	TEX=0;				// GPU handle to a diffuse texture; TEX.a = alpha values
	uint64_t	NRM=0;				// GPU handle to a normal map
};
static_assert(sizeof(material)%16==0,"size of struct material should be aligned at 16-byte boundary" );
inline float beta_to_roughness( float b ){ return float(sqrtf(2.0f/(b+2.0f))); }  // Beckmann roughness in [0,1] (0:mirror, 1: Lambertian)
inline float roughness_to_beta( float r ){ return 2.0f/(r*r)-2.0f; }
#endif

struct material_impl : public material
{
	const uint	ID;
	uint64_t	CUB=0;							// GPU handle to a cube/reflection map
	char		name[_MAX_PATH]={0};
	float		bump_scale=1.0f;
	union
	{
		struct { gl::Texture				*diffuse, *normal, *cube, *alpha; };
		struct { ID3D10ShaderResourceView	*diffuse, *normal, *cube, *alpha; } d3d10;
		struct { ID3D11ShaderResourceView	*diffuse, *normal, *cube, *alpha; } d3d11;
	} texture = {0};
	struct
	{
		wchar_t diffuse[_MAX_PATH];
		wchar_t bump[_MAX_PATH];
		wchar_t normal[_MAX_PATH];
		wchar_t cube[_MAX_PATH];
		wchar_t alpha[_MAX_PATH];
	} path = {0};

	material_impl( uint id ):ID(id){color=vec4(0.7f,0.7f,0.7f,1.0f);}
	float roughness(){ return float(sqrtf(2.0f/(beta+2.0f))); }  // Beckmann roughness in [0,1] (0:mirror, 1: Lambertian)
};

//***********************************************
// acceleration structures
struct acc_t
{
	enum { NONE, BVH, KDTREE } model = NONE;
	mesh*		p_mesh = nullptr;	// should be set to mesh
	uint		geom = -1;			// should be set for geometry BVH, -1 for mesh BVH
	virtual bool intersect( const ray& r, std::vector<uint>* hit_prim_list=nullptr )=0; // return primitive ids
	virtual bool intersect( const ray& r, isect* pi )=0;
	virtual void release()=0;
};

//***********************************************
// Bounding volume hierarchy
struct bvh_t : public acc_t // two-level hierarchy: mesh or geometry
{
	struct node { bbox box; uint parent; uint nprims; union { uint idx:30, second:30; }; uint axis:2;  };	// idx: primitive index in index buffer; second: offset to right child of interiod node; axis==3 (leaf node)
	std::vector<node> nodes;
};

//***********************************************
// KDtree
struct kdtree_t : public acc_t // two-level hierarchy: mesh or geometry
{
	struct node { float pos; uint parent; uint second:29, has_left:1, axis:2; __forceinline bool is_leaf(){return axis==3;} };
	std::vector<node> nodes;
};

//***********************************************
// cull data definition
struct cull_t
{
	enum model { NONE=0, USER=1<<0, REF=1<<1, VFC=1<<2, CHCPP=1<<3, WOC=1<<4, RESERVED5=1<<5, RESERVED6=1<<6, RESERVED7=1<<7 };
	uchar data = 0 ; // bits of the corresponding culling types are set (bitwise OR-ed)

	inline operator bool() const { return data!=0; }
	inline bool operator()( uint m ) const { return (data&m)!=0; }
	inline cull_t& reset( uchar m=0xff ){ data=(m==0xff)?0:(data&~m); return *this; }
	inline cull_t& set( uchar m ){ data|=m; return *this;}
};

//***********************************************
// geometry: 144-bytes aligned; differentiated by material and grouping
#ifndef __cplusplus
struct geometry // std430 layout for OpenGL shader storage buffers
{
	uint	count, instance_count, first_index, base_vertex, base_instance; // DrawElementsIndirectCommand
	uint	material_index, object_index, pad[5]; // material index and other C++ members (hidden padding in shaders)
	bbox	box;
	mat4	mtx;
};
#else
struct geometry 
{
	uint	count=0;			// start of DrawElementsIndirectCommand: number of indices
	uint	instance_count=1;	// default: 1
	uint	first_index=0;		// first index
	uint	base_vertex=0;		// default: 0
	uint	ID=-1;				// ID shared with base_instance (typically 0)
	uint	material_index=-1;
	uint	object_index=-1;	// object index to avoid pointer changes with vector reallocation
	ushort	instance=0;			// host-side instance ID (up to 2^16: 65536)
	cull_t	cull;				// 8-bit cull mask
	bool	bg=false;			// is it background object?
	mesh*	root=nullptr;		// pointer to the mesh
	acc_t*	acc=nullptr;		// BVH or KD-tree build on geometry level
	bbox	box;				// transform-unbaked bounding box
	mat4	shader_matrix;		// do not use in C++; reserved for shader usage; use mesh::update_matrix() to fetch from object matrix
	
	geometry() = delete; // no default ctor to enforce to assign root
	geometry( mesh* p_mesh ):root(p_mesh){}
	geometry( mesh* p_mesh, uint id, uint obj_index, uint start_index, uint index_count, bbox* box, uint mat_index ):root(p_mesh),ID(id),object_index(obj_index),first_index(start_index),count(index_count),material_index(mat_index){ if(box) this->box=*box; }
	~geometry(){ if(acc) acc->release(); }

	inline object* parent() const;
	inline const char* name() const;
	inline mat4 matrix() const;
	inline void* offset() const { return (void*)(ID*sizeof(geometry)); } // for rendering commands

	inline uint face_count() const { return uint(count/3); }
	inline float surface_area() const ;
	inline bool intersect( const ray& r, isect* pi=nullptr ) const; // linear intersection
};
static_assert( sizeof(geometry)%16==0, "sizeof(geometry) should be 16-byte aligned" );
#endif

//***********************************************
// a set of geometries for batch control (not related to the rendering)
struct object
{
	mat4	matrix;				// group transformation
	bbox	box;				// transformation-baked bounding box (expanded by geometry box and transformation) 
	uint	ID=-1;
	float	level=0;			// real-number LOD
	uint	instance=0;
	struct { bool dynamic=false, bg=false; } attrib; // dynamic: potential matrix changes; background: backdrops such as floor/ground; set in other plugins (e.g., AnimateMesh)
	char	name[_MAX_PATH]={0};
	mesh*	root=nullptr;
	
	object() = delete;  // no default ctor to enforce to assign parent
	object( mesh* p_mesh ):root(p_mesh){}
	object( mesh* p_mesh, uint id, const char* name, bbox* box=nullptr ):ID(id),root(p_mesh){ strcpy(this->name,name); if(box) this->box=*box; }
	
	// face/geometry query
	inline bool empty() const;
	inline uint face_count() const;
	inline uint geometry_count() const;
	inline geometry* find_geometry( uint index ) const;
	inline std::vector<geometry*> find_geometries() const;

	// create helpers
	inline geometry* create_geometry( size_t first_index, size_t index_count=0, bbox* box=nullptr, size_t mat_index=-1 );
	inline geometry* create_geometry( const geometry& other );
};

//***********************************************
// a set of vertices, indices, objects, geometries, materials
struct mesh
{
	std::vector<vertex>			vertices;
	std::vector<uint>			indices;
	std::vector<object>			objects;
	std::vector<geometry>		geometries;		// if LODs are found, multiple copies at different levels (level>0) are consecutively stored here
	std::vector<material_impl>	materials;

	// volitile spaces for GPU buffers
	union {
		struct { gl::VertexArray* vertex; gl::Buffer *geometry, *count; }; // vertex array, geometry/command buffer, count buffer for OpenGL
		struct { ID3D10Buffer *vertex, *index; } d3d10; // vertex/index buffers for D3D
		struct { ID3D11Buffer *vertex, *index; } d3d11; // vertex/index buffers for D3D
	} buffer={0};

	// acceleration and dynamics
	bbox		box;
	acc_t*		acc=nullptr;		// BVH or KD-tree

	// LOD and instancing
	uint		levels=1;			// LOD levels
	uint		instance_count=1;	// number of instances. The instances are physically added into objects

	// auxiliary information
	wchar_t		file_path[_MAX_PATH]={0};	// mesh file path
	wchar_t		mtl_path[_MAX_PATH]={0};	// material file path (e.g., *.mtl)

	// constructor
	mesh(){ vertices.reserve(1<<20); indices.reserve(1<<20); objects.reserve(1<<16); geometries.reserve(1<<16); materials.reserve(1<<16); }
	inline mesh( mesh&& other ){ operator=(std::move(other)); } // move constructor
	inline mesh& operator=( mesh&& other ); // move assignment operator

	// release/memory
	void release(){ if(!vertices.empty()){ vertices.clear(); vertices.shrink_to_fit(); } if(!indices.empty()){ indices.clear(); indices.shrink_to_fit(); } if(!geometries.empty()){ geometries.clear(); geometries.shrink_to_fit(); } objects.clear(); objects.shrink_to_fit(); materials.clear(); materials.shrink_to_fit(); if(acc){ acc->release(); acc=nullptr; } }
	mesh* shrink_to_fit(){ vertices.shrink_to_fit(); indices.shrink_to_fit(); objects.shrink_to_fit(); geometries.shrink_to_fit(); materials.shrink_to_fit(); return this; }

	// face/object/proxy/material helpers
	uint face_count( int level=0 ) const { uint kn=uint(geometries.size())/levels; auto* g=&geometries[kn*level];uint f=0;for(uint k=0;k<kn;k++,g++)f+=g->count;return f/3;}
	object* create_object( const char* name, bbox* _box=nullptr ){ objects.emplace_back(object(this,uint(objects.size()),name,_box)); return &objects.back(); }
	object* create_object( const object& o ){ objects.emplace_back(o); auto& o1=objects.back(); o1.root=this; o1.ID=uint(objects.size())-1; return &o1; }
	object*	find_object( const char* name ){ for(uint k=0;k<objects.size();k++)if(_stricmp(objects[k].name,name)==0) return &objects[k];return nullptr;}
	inline mesh* create_proxy( bool use_quads=false, bool double_sided=false ); // proxy mesh helpers: e.g., bounding box
	std::vector<material> pack_materials() const { std::vector<material> p; auto& m=materials; p.resize(m.size()); for(size_t k=0, kn=p.size(); k<kn; k++) p[k]=m[k]; return p; }

	// update for matrix/bound
	inline bool is_dynamic() const { for( size_t k=0, kn=objects.size()/instance_count; k<kn; k++ ) if(objects[k].attrib.dynamic) return true; return false; }
	inline void update_matrix( bool transpose=false ){ if(!transpose) for(auto& g:geometries) g.shader_matrix=objects[g.object_index].matrix; else for(auto& g:geometries) g.shader_matrix=objects[g.object_index].matrix.transpose(); }
	inline void update_bound( bool bRecalcTris=false );

	// intersection
	inline bool intersect( const ray& r, std::vector<uint>* hit_prim_list=nullptr ) const;
	inline bool intersect( const ray& r, isect* pi=nullptr ) const;
};

//***********************************************
// Volume data format
struct volume
{
	wchar_t	file_path[_MAX_PATH]={0};
	uint	width, height, depth;
	vec3	voxel_size;		// size of a unit voxel
	uint	format;			// GL_R32F or GL_RGBA32F
	bbox	box;			// bounding box
	float*	data;			// voxel data array: cast by format
	vec4	table[256];		// transfer function: usually 256 size
};

//***********************************************
// late implementations for geometry

inline object* geometry::parent() const { return root&&object_index<root->objects.size()?&root->objects[object_index]:nullptr; }
inline mat4 geometry::matrix() const { return parent()?parent()->matrix:mat4::identity(); }
inline const char* geometry::name() const { return parent()?parent()->name:""; }
inline float geometry::surface_area() const { if(count==0) return 0.0f; if(root->vertices.empty()||root->indices.empty()) return 0.0f; vertex* v=&root->vertices[0]; uint* i=&root->indices[0]; float sa = 0.0f; for( uint k=first_index, kn=k+count; k<kn; k+=3 ) sa += (v[i[k+1]].pos-v[i[k+0]].pos).cross(v[i[k+2]].pos-v[i[k+0]].pos).length()*0.5f; return sa; }

//***********************************************
// late implementations for object

inline bool object::empty() const { auto* g=&root->geometries[0]; for( uint k=0,kn=uint(root->geometries.size()); k<kn; k++, g++ ) if(g->parent()==this) return false; return true; }
inline uint object::geometry_count() const { uint n=0; for( auto& g : root->geometries ) if(g.parent()==this) n++; return n; }
inline uint object::face_count() const { auto* g=&root->geometries[0]; uint f=0; for( uint k=0,kn=uint(root->geometries.size()); k<kn; k++ ) if(g[k].parent()==this) f+=g[k].face_count(); return f; }
inline geometry* object::find_geometry( uint index ) const { auto* g=&root->geometries[0]; uint n=0; for( uint k=0,kn=uint(root->geometries.size()); k<kn; k++, g++ ) if(g->parent()==this&&(n++)==index) return g; return nullptr; }
inline std::vector<geometry*> object::find_geometries() const { auto* g=&root->geometries[0]; std::vector<geometry*> gl; for( uint k=0,kn=uint(root->geometries.size()); k<kn; k++, g++ ) if(g->parent()==this) gl.emplace_back(g); return std::move(gl); }
inline geometry* object::create_geometry( size_t first_index, size_t index_count, bbox* box, size_t mat_index ){ auto& g=root->geometries; g.emplace_back(geometry(root,uint(g.size()),this->ID,uint(first_index),uint(index_count),box,uint(mat_index))); return &g.back(); }
inline geometry* object::create_geometry( const geometry& other ){ auto& g=root->geometries; g.emplace_back(other); auto* p=&g.back(); p->ID=uint(g.size())-1; return p; }

//***********************************************
// late implementations for mesh

__noinline inline mesh& mesh::operator=( mesh&& other ) // move assignment operator
{
	vertices = std::move(other.vertices);
	indices = std::move(other.indices);
	geometries = std::move(other.geometries);
	objects = std::move(other.objects);
	materials = std::move(other.materials);
	size_t offset = sizeof(decltype(vertices))+sizeof(decltype(indices))+sizeof(decltype(geometries))+sizeof(decltype(objects))+sizeof(decltype(materials));
	memcpy( ((char*)this)+offset, ((char*)&other)+offset, sizeof(mesh)-offset );
	if(acc) acc->p_mesh=this; for(auto& o:objects ) o.root=this; for(auto& g:geometries ){ g.root=this; if(g.acc) g.acc->p_mesh=this; }
	return *this;
}

__noinline inline void mesh::update_bound( bool bRecalcTris )
{
	box.clear(); for(auto& obj:objects) obj.box.clear();
	for( size_t k=0, kn=geometries.size(), gn=kn/instance_count; k<kn; k++ )
	{
		auto& g=geometries[k], g0=geometries[k%gn];
		if(g.instance>0) g.box=g0.box; else if(bRecalcTris){ g.box.clear(); vertex* V=&vertices[0]; uint* I=&indices[0]; for(uint j=g.first_index,jn=j+g.count;j<jn;j+=3) g.box.expand(V[I[j+0]].pos,V[I[j+1]].pos,V[I[j+2]].pos); }
		g.parent()->box.expand( g.matrix()*g.box );
	}
	for( auto& obj : objects ) box.expand( obj.box );
}

__noinline inline mesh* mesh::create_proxy( bool use_quads, bool double_sided )
{
	mesh* proxy = new mesh();

	// direct copy
	proxy->objects = objects;
	proxy->materials.clear();	// proxy not use materials
	proxy->box = box;
	proxy->acc = acc;			// use the same acceleration structures
	proxy->instance_count = instance_count;
	wcscpy( proxy->file_path, file_path );

	// correction of mesh pointers
	for( auto& o : proxy->objects ) o.root = proxy;

	// index definitions (CCW: counterclockwise by default)
	std::vector<uint> i0;
	if(use_quads)	i0 = { 0,3,2,1, /*bottom*/ 4,5,6,7, /*top*/ 0,1,5,4, /*left*/ 2,3,7,6, /*right*/ 1,2,6,5, /*front*/ 3,0,4,7 /*back*/ };
	else			i0 = { 0,2,1,0,3,2, /*bottom*/ 4,5,6,4,6,7, /*top*/ 0,1,5,0,5,4, /*left*/ 2,3,6,3,7,6, /*right*/ 1,2,6,1,6,5, /*front*/ 0,4,3,3,4,7 /*back*/ };
	if(double_sided){ for(size_t k=0,f=use_quads?4:3,kn=i0.size()/f;k<kn;k++) for(size_t j=0;j<f;j++) i0.emplace_back(i0[(k+1)*f-j-1]); } // insert indices (for CW)

	// default corner/vertex definition
	bbox cv(-1.0f,1.0f); vec4 corners[8]; for(uint k=0;k<8;k++) corners[k] = vec4(cv.corner(k),1.0f);
	vertex v = { vec3(0.0f), vec3(0.0f), vec2(0.0f) };

	// create vertices/geometries
	auto& i=proxy->indices; for( auto& g : geometries )
	{
		proxy->objects[g.object_index].create_geometry( i.size(), i0.size(), &g.box );
		for(auto& j:i0) i.emplace_back(j+uint(proxy->vertices.size()));
		mat4 m = mat4::translate(g.box.center())*mat4::scale(g.box.size()*0.5f);
		for(uint k=0;k<8;k++){ v.pos=(m*corners[k]).xyz; proxy->vertices.emplace_back(v); }
	}

	return proxy;
}

//***********************************************
// intersection implementations

__noinline inline ray gen_primary_ray( camera* cam, float x, float y )	// (x,y) in [0,1]
{
	const vec3& eye=cam->eye, center=cam->center, up=cam->up;
	float fh = tan(cam->fovy*0.5f)*2.0f, fw=fh*cam->aspect;		// frustum height/width in NDC
	vec3 epos = vec3( fw*(x-0.5f), fh*(y-0.5f), -1.0f );			// pixel position on the image plane: make sure to have negative depth
	mat4 I = mat4::lookAtInverse(eye,center,up);					// inverse view matrix
	ray r; r.o=eye; r.d=(I*epos-eye).normalize(); r.t=0.0f; r.tfar=FLT_MAX; return r;
}

// triangle intersection: isect=(pos,t), bc=(s,t)
__noinline inline bool intersect( const ray& r, const vec3& v0, const vec3& v1, const vec3& v2, isect* pi=nullptr )
{
	// http://geomalgorithms.com/a06-_intersect-2.html#intersect3D_RayTriangle
	if(pi) pi->hit = false;

	vec3 u=v1-v0, v=v2-v0, n=u.cross(v); if(n.length()==0) return false;	// degenerate case: non-triangle
	float b=dot(n,r.dir); if(b>-0.000001f) return false;					// skip backfaces or rays lying on the plane
	float t=dot(n,v0-r.pos)/b; if(t<r.t||t>r.tfar) return false;		// out of range of [tnear,tfar]
	vec3 ipos=r.pos+t*r.dir, w=ipos-v0;
	
	// barycentric coord test
    float uu=dot(u,u), uv=dot(u,v), vv=dot(v,v), wu=dot(w,u), wv=dot(w,v), D=uv*uv-uu*vv;
    float bs = (uv*wv-vv*wu)/D; if( bs < 0.0f || bs > 1.0f )		return false;
    float bt = (uv*wu-uu*wv)/D; if( bt < 0.0f || (bs + bt) > 1.0f )	return false;

	if(pi)
	{
		pi->pos = ipos;
		pi->norm = n.normalize();
		pi->bc = vec2(bs,bt);
		pi->t = t;
		pi->tfar = t;
		pi->hit = true;
	}

	return true;
}

// box intersection
__noinline inline bool bbox::intersect( const ray& r, isect* pi )
{
	if(pi) pi->hit = false;

	float t0=r.t, t1=r.tfar;
	for( int k=0; k < 3; k++ )
	{
		float i=1.0f/r.d[k];
		if(i<0){ t0=max(t0,(M[k]-r.o[k])*i); t1=min(t1,(m[k]-r.o[k])*i); }
		else { t0=max(t0,(m[k]-r.o[k])*i); t1=min(t1,(M[k]-r.o[k])*i); }
		if(t0>t1) return false;
	}

	if(pi)
	{
		pi->t = t0;
		pi->tfar = t1;
		pi->hit = true;
	}

	return true;
}

__noinline inline bool geometry::intersect( const ray& r, isect* pi ) const
{
	isect i, t; i.hit=false;
	mat4 m = matrix();
	const vertex* V = &root->vertices[0];
	const uint* I = &root->indices[first_index];
	for( uint k=0, kn=count/3; k<kn; k++, I+=3 )
		if(::intersect(r,m*V[I[0]].pos,m*V[I[1]].pos,m*V[I[2]].pos,&t)&&t.t<i.t) i=t;
	
	if(pi&&i.hit){ *pi=i; pi->g=this->ID; }
	return i.hit;
}

__noinline inline bool mesh::intersect( const ray& r, std::vector<uint>* hit_prim_list ) const
{
	std::vector<uint> m;
	for( uint k=0, kn=uint(geometries.size()); k<kn; k++ )
	{
		if(!(geometries[k].matrix()*geometries[k].box).intersect(r,nullptr)) continue;
		m.emplace_back(uint(k));
	}
	if(hit_prim_list) *hit_prim_list = m;
	return !m.empty();
}

__noinline inline bool mesh::intersect( const ray& r, isect* pi ) const
{
	isect m;
	std::vector<uint> hit_prim_list;
	if(!intersect( r, &hit_prim_list )) return false;

	for( uint k=0; k < uint(hit_prim_list.size()); k++ )
	{
		isect i; if(!geometries[hit_prim_list[k]].intersect(r,&i)||i.t>m.t) continue;
		m=i;
	}
	if(m.hit&&pi) *pi = m;
		
	return m.hit;
}

//***********************************************
// line clipping by Liang Barsky algorithm
__noinline inline bool clip_line( vec2 p, vec2 q, vec2 lb, vec2 rt, vec2* p1=nullptr, vec2* q1=nullptr )
{
	vec2 d=q-p, t=vec2(0,1);
	vec2 vs[4] = { {-d.x,p.x-lb.x}, {d.x,rt.x-p.x}, {-d.y,p.y-lb.y}, {d.y,rt.y-p.y} };
	for( int k=0; k<4; k++ )
	{
		vec2 v = vs[k];
		float f=v.y/v.x;
		if(v.x<0){		if(f>t.y) return true; if(f>t.x) t.x=f; }
		else if(v.x>0){	if(f<t.x) return true; if(f<t.y) t.y=f; }
		else if(v.x==0&&v.y<0) return true;
	}
	
	if(p1&&t.x>0.0f) *p1=lerp(p,q,t.x);		// new p
	if(q1&&t.y<1.0f) *q1=lerp(p,q,t.y);		// new q
	return false;
}

//***********************************************
// mesh utilities
__noinline inline mesh* create_box_mesh( bbox& box, const char* name="box", bool use_quads=false, bool double_sided=false )
{
	mesh* m = new mesh();
	
	// vertex definitions
	for( uint k=0; k < 8; k++ ) m->vertices.emplace_back(vertex{ box.corner(k), vec3(0.0f), vec2(0.0f) });

	// index definitions (CCW: counterclockwise by default)
	if(use_quads)	m->indices = { 0,3,2,1, /*bottom*/ 4,5,6,7, /*top*/ 0,1,5,4, /*left*/ 2,3,7,6, /*right*/ 1,2,6,5, /*front*/ 3,0,4,7 /*back*/ };
	else			m->indices = { 0,2,1,0,3,2, /*bottom*/ 4,5,6,4,6,7, /*top*/ 0,1,5,0,5,4, /*left*/ 2,3,6,3,7,6, /*right*/ 1,2,6,1,6,5, /*front*/ 0,4,3,3,4,7 /*back*/ };
	if(double_sided){ auto& i=m->indices; for(size_t k=0,f=use_quads?4:3,kn=i.size()/f;k<kn;k++) for(size_t j=0;j<f;j++) i.emplace_back(i[(k+1)*f-j-1]); } // insert indices (for CW)

	// create object and geometry
	m->create_object(name,&box)->create_geometry(0,uint(m->indices.size()),&box,-1);

	return m;
}

inline mesh* create_box_mesh( const char* name="box", bool use_quads=false, bool double_sided=false, float half_size=1.0f )
{
	return create_box_mesh( bbox{vec3(-half_size),vec3(half_size)}, name, use_quads, double_sided );
}

#endif // __GX_MESH__