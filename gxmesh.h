#pragma once
#include "gxmath.h"

//*******************************************************************
// General sampler interface (e.g., lens sampler, ray sampler, etc.): vec4(x,y,z,weight) in unit circle/sphere
struct ISampler
{
	std::array<vec4,1024>	data;
	uint					crc;	// crc32c coding, which can easily detect the change of samples using data callback
	uint					n=1;	// number of samples
	
	inline bool			empty() const { return data.empty(); }
	inline uint			size() const { return n; }
	inline uint			max_size() const { return uint(data.max_size()); }
	inline vec4&		operator[]( int i ){ return data[i]; }
	inline const vec4&	operator[]( int i ) const { return data[i]; }
	inline vec4&		operator[]( uint i ){ return data[i]; }
	inline const vec4&	operator[]( uint i ) const { return data[i]; }
#ifdef _M_X64
	inline vec4&		operator[]( size_t i ){ return data[i]; }
	inline const vec4&	operator[]( size_t i ) const { return data[i]; }
#endif
	inline vec4*		ptr( size_t i=0u ){ return empty()?nullptr:&data[i]; }
	inline const vec4*	ptr( size_t i=0u ) const { return empty()?nullptr:&data[i]; }
	inline operator const vec4* () const { return empty()?nullptr:&data[0]; }
	inline operator vec4* () { return empty()?nullptr:&data[0]; }
	inline operator const float* () const { return empty()?nullptr:(float*)&data[0]; }
	inline operator float* () { return empty()?nullptr:(float*)&data[0]; }

	inline uint		max_samples() const { return uint(data.max_size()); }
	inline float	sample_dist() const { float s=0.0f; const vec4* p=ptr(); for(uint k=0;k<n;k++){float m=FLT_MAX;for(uint j=0;j<n;j++) if(k!=j) m=min(m,(p[k].xyz-p[j].xyz).length2() );s+=sqrtf(m);} return s/float(max(1u,n)); } // mean distance of pairwise samples
	inline void		normalize(){ if(empty())return;dvec3 m=dvec3(0,0,0);vec4* p=ptr();for(uint k=0;k<n;k++)m+=dvec3(p[k].x,p[k].y,p[k].z);m/=double(n);vec3 f=vec3(float(-m.x),float(-m.y),float(-m.z));for(uint k=0;k<n;k++)p[k].xyz+=f; } // make the center position of samples to the origin
};

//*******************************************************************
struct Light
{
	vec4	position;
	vec4	ambient;
	vec4	diffuse;
	vec4	specular;
	float	powerScale;		// power of material shininess
};

//*******************************************************************
// ray
struct ray
{
	union
	{
		struct { vec3 pos, dir; vec4 tex; };						// lens system rays (with texcoord)
		struct { vec3 o, d; float tnear, tfar, time; int depth; };	// pbrt-like rays (tnear/tfar = parameters of the nearest/farthest intersections)
	};
	
	ray():tnear(0.0f),tfar(FLT_MAX),time(0.0f),depth(0){}
	ray( const vec3& _pos, const vec3& _dir, float _tnear=0.0f, float _tfar=FLT_MAX ):ray(){ pos=_pos; dir=_dir; tnear=_tnear; tfar=_tfar; }
};

//*******************************************************************
// forward declaration
struct geometry;
struct object;
struct mesh;

//*******************************************************************
// intersection
struct isect
{
	geometry* g=nullptr;
	vec3 pos;				// position at intersection
	vec3 norm;				// normal at intersection
	vec2 bc;				// barycentric coordinates at t
	float t=FLT_MAX;		// nearest intersection
	float tfar=FLT_MAX;		// fartheset intersection
	bool hit=false;			// is intersected?
};

//*******************************************************************
// bounding box
struct bbox
{
	vec3 m, M;

	bbox(){ M=-(m=FLT_MAX*0.01f); }
	bbox( const vec3& v0 ):m(v0),M(v0){}
	bbox( const vec3& v0, const vec3& v1 ){ m=vec3(min(v0.x,v1.x),min(v0.y,v1.y),min(v0.z,v1.z)); M=vec3(max(v0.x,v1.x),max(v0.y,v1.y),max(v0.z,v1.z)); }
	bbox( const vec3& v0, const vec3& v1, const vec3& v2 ){ m=vec3(min(min(v0.x,v1.x),v2.x),min(min(v0.y,v1.y),v2.y),min(min(v0.z,v1.z),v2.z)); M=vec3(max(max(v0.x,v1.x),v2.x),max(max(v0.y,v1.y),v2.y),max(max(v0.z,v1.z),v2.z)); }
	bbox( const bbox& b0, const bbox& b1 ){ m=vec3(min(b0.m.x,b1.m.x),min(b0.m.y,b1.m.y),min(b0.m.z,b1.m.z)); M=vec3(max(b0.M.x,b1.M.x),max(b0.M.y,b1.M.y),max(b0.M.z,b1.M.z)); }
	inline void clear(){ M=-(m=FLT_MAX*0.0001f); }

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
	inline const bbox& expand( const mat4& mtx ){ auto c=corners(); clear(); for( uint k=0;k<c.size();k++) expand(mtx*c[k]); return *this; }
	
	// transformation
	inline const bbox& scale( float s ){ vec3 c=center(); m=c+(m-c)*s; M=c+(M-c)*s; return *this; }
	inline const bbox& scale( const vec3& v ){ vec3 c=center(); m=c+(m-c)*v; M=c+(M-c)*v; return *this; }
	inline const bbox& scale( float sx, float sy, float sz ){ vec3 c=center(); m=c+(m-c)*vec3(sx,sy,sz); M=c+(M-c)*vec3(sx,sy,sz); return *this; }

	// intersection
	inline bool intersect( const ray& r, isect* pi=nullptr );
};

inline bbox operator*( const mat4& m, const bbox& b ){ bbox b1=b; return b1.expand(m); }

//*******************************************************************
// Thin-lens camera definition
struct camera
{
	mat4	viewMatrix, projectionMatrix;		// matrices
	vec3	eye, center, up, dir;				// look at params; dir = center - eye (view direction vector)
	float	fovy, aspectRatio, dNear, dFar;		// projection params
	vec4	frustum_planes[6];					// for view frustum culling: left, right, top, bottom, near, far
	float	F, fn, df;							// focal distance, f-number, focusing depth (in object distance)
	
	inline mat4		perspectiveDX() const { mat4 m=projectionMatrix; m._33=dFar/(dNear-dFar); m._34*=0.5f; return m; } // you may use mat4::perspectiveDX() to set canonical depth range in [0,1] instead of [-1,1]
	inline vec2		frustum_size() const {float fh=tan(deg2rad(fovy)*0.5f)*2.0f; return vec2(fh*aspectRatio,fh); } // view frustum (width,height) at depth=-1 (i.e., in NDC cube)
	inline float	lens_radius() const { return F/fn*0.5f; }
	inline void		update_frustum_planes(){ const mat4 m=projectionMatrix*viewMatrix; for( int k=0;k<6;k++){ vec4& p=frustum_planes[k]; p=m.rvec4(k/2)*float(k%2==0?1:-1)+m.rvec4(3); p/=p.xyz.length(); }} // left, right, bottom, top, near, far
	inline void		update_depth_clips( const bbox* bound ){ if(!bound) return; bbox b=viewMatrix*(*bound); vec2 z(max(0.001f,-b.M.z),max(0.001f,-b.m.z)); dNear=max(max(bound->radius()*0.00001f,50.0f),z.x*0.99f); dFar=max(max(dNear+1.0f,dNear*1.01f),z.y*1.01f); }
};

//*******************************************************************
// vertex definition
#if (_MSC_VER>=1900/*VS2015*/) && (__cplusplus>199711L)
struct alignas(32) vertex { vec3 pos; vec3 norm; vec2 tex; };
#else
struct vertex { vec3 pos; vec3 norm; vec2 tex; };
#endif

//*******************************************************************
// material definition
struct material
{
	const uint		ID;
	char			name[_MAX_PATH];
	bool			bShade;
	vec4			ambient, diffuse, specular;
	float			power, alpha, refractiveIndex, bump_scale;
	wchar_t			diffuseMapPath[_MAX_PATH],bumpMapPath[_MAX_PATH],normalMapPath[_MAX_PATH],cubeMapPath[_MAX_PATH],alphaMapPath[_MAX_PATH];
	void			*diffuseTexture,*normalTexture,*cubeTexture,*alphaTexture;	// placeholders for ID3D1*ShaderResourceView or gl::Texture
	int				diffuse_slice, normal_slice, alpha_slice;					// placeholders for slide indices of textures when they are merged into a single array texture
	material( uint id ):ID(id),bShade(true),bump_scale(1.0f),diffuseTexture(nullptr),normalTexture(nullptr),cubeTexture(nullptr),alphaTexture(nullptr),diffuse_slice(-1),normal_slice(-1),alpha_slice(-1){ memset(name,0,sizeof(name)); memset(diffuseMapPath,0,sizeof(diffuseMapPath)); memset(bumpMapPath,0,sizeof(bumpMapPath)); memset(normalMapPath,0,sizeof(normalMapPath)); memset(cubeMapPath,0,sizeof(cubeMapPath)); memset(alphaMapPath,0,sizeof(alphaMapPath));};
};

//*******************************************************************
// cull data definition
struct Cull // 16-bytes reserved
{
	enum TYPE { USER, REF, VFC, CHCPP, WOC, NOHC, CULL_TYPE_RESERVED6, CULL_TYPE_RESERVED7, NUM_CULL_TYPE };

	uint	data:7;			bool	occluder:1;		// current frame: bits of the corresponding culling types are set, occluder=1/occludee=0
	uint	duration:14;	// how many frames the previous state lasted? For instance, if the last frame was just changed, duration=1. potential: is this one of the potential occluders?
	bool	potential:1;	// the frame number of the last access to this
	bool	disable:1;		// set this bit to force culling anyway
	int		access;			// frame index accessed the last time
	int		reserved[2];	// 8-bytes reserved for further revision

	inline Cull(){ clear(true); }
	inline Cull( TYPE type ):Cull(){ set_bit(type,true); }
	inline void	clear( bool bClearAttrib=false ){ if(bClearAttrib) memset(this,0,sizeof(Cull)); else data=0; }
	inline void	set_bit( TYPE type, bool b=true ){ uchar bit=(1<<type); data=b?data|bit:data&(~bit); }
	inline bool	get_bit( TYPE type ){ uchar bit=(1<<type); return (data&bit)!=0; }
	inline bool operator()() const { return data!=0&&!disable; }
	inline operator bool() const { return data!=0&&!disable; }
	inline Cull& operator=( TYPE type ){ set_bit(type,true); return *this; }
};

//*******************************************************************
// Bounding volume hierarchy
struct BVH	// two-level hierarchy: mesh or geometry
{
	mesh*		pMesh = nullptr;	// should be set to mesh
	uint		geom = -1;			// should be set for geometry BVH, -1 for mesh BVH
	struct node { bbox box; uint parent; uint nprims; union { uint idx:30, second:30; }; uint axis:2;  };	// idx: primitive index in index buffer; second: offset to right child of interiod node; axis==3 (leaf node)
	std::vector<node> nodes;
	inline bool intersect( const ray& r, std::vector<uint>* hit_prim_list=nullptr );			// return primitive ids
	inline bool intersect( const ray& r, isect* pi=nullptr, bool use_backface=false );
	virtual void release()=0;
};

//*******************************************************************
// KDtree
struct KDtree // for two-level hierarchy: mesh or geometry
{
	mesh*		pMesh = nullptr;	// should be set to mesh
	uint		geom = -1;			// should be set for geometry BVH, -1 for mesh BVH
	struct node { float pos; uint parent; uint second:29, has_left:1, axis:2; __forceinline bool is_leaf(){return axis==3;} };
	std::vector<node> nodes;
	inline bool intersect( const ray& r, std::vector<uint>* hit_prim_list=nullptr ){ return false; }	// TODL: not implemented yet
	inline bool intersect( const ray& r, isect* pi=nullptr ){ return false; }	// TODO: not implemented yet
	virtual void release()=0;
};

//*******************************************************************
// geometry group: differentiated by material and grouping
struct geometry
{
	uint		ID;
	object*		parent;
	material*	mat;
	uint		indexStart;
	uint		indexCount;		// index buffer indicator
	bbox		box;			// bounding box
	uint		level;			// LOD level represented by this geometry
	uint		instance;		// instance ID
	Cull		cull;			// culling info
	BVH*		bvh;			// geometry-level bounding volume hierarchy
	KDtree*		kd;				// geometry-level KD-tree
	
	geometry():ID(-1),parent(nullptr),mat(nullptr),indexStart(0),indexCount(0),level(0),instance(0),bvh(nullptr),kd(nullptr){ cull.clear(); }
	geometry( object* _parent,material* _mat,uint _indexStart,uint _indexCount,bbox& _box,uint _level=0):ID(-1),parent(_parent),mat(_mat),indexStart(_indexStart),indexCount(_indexCount),box(_box),level(_level),instance(0),bvh(nullptr),kd(nullptr){ cull.clear(); }
	~geometry(){ if(bvh) bvh->release(); if(kd) kd->release(); }
	inline mat4& mtx();
	inline const char* name();
	inline uint num_faces(){ return uint(indexCount/3); }
	inline bool intersect( const ray& r, isect* pi=nullptr, bool use_backface=false ); // linear intersection
	inline float surface_area();
};

//*******************************************************************
// object: set of geometries
struct object
{
	uint	ID;
	mesh*	parent;
	char	name[_MAX_PATH];
	mat4	mtx;
	bbox	box;
	float	LOD;			// current LOD
	uint	instance;		// instance ID
	
	// temporary data
	void*	data;	// reserved for user data
	
	object():ID(-1),parent(nullptr),LOD(0.0f),instance(0),data(nullptr){mtx.setIdentity();memset(name,0,sizeof(name));}
	inline void setLOD( float level );
	inline uint num_geometries();
	inline std::vector<geometry*> find_geometries( int level=0 );
	inline uint num_faces( int level=0 );
};

//*******************************************************************
// mesh: set of vertices, indices, objects, materials
struct mesh
{
	// core data
	std::vector<vertex>		vertices;
	std::vector<uint>		indices;
	std::vector<geometry>	geometries;		// if LODs are found, multiple copies at different levels (level>0) are consecutively stored here
	std::vector<object*>	objects;
	std::vector<material*>	materials;

	// space for GPU buffers
	union { void* vertex_buffer; void* vertex_array; };	// vertex buffer for D3DX and vertex array for OpenGL
	void*	index_buffer;	// index buffer for D3DX and nullptr for OpenGL
	void*	command_buffer;	// indirect command buffer, filled with DrawElementsIndirectCommand
	void*	count_buffer;	// indirect count buffer, holding GL_PARAMETER_BUFFER_ARB 

	// spatial acceleration structures
	BVH*		bvh;			// bounding volume hierarchy
	KDtree*		kd;				// KD-tree

	// LOD and instancing
	uint		num_LOD;			// number of detail levels
	uint		num_instance;		// number of instances. The instances are physically added into objects

	// auxiliary information
	wchar_t*	file_path	= (wchar_t*)malloc(_MAX_PATH*sizeof(wchar_t));	// mesh file path
	wchar_t*	mtl_path	= (wchar_t*)malloc(_MAX_PATH*sizeof(wchar_t));	// material file path (e.g., *.mtl)
	bbox		box;

	mesh():vertex_buffer(nullptr),index_buffer(nullptr),command_buffer(nullptr),count_buffer(nullptr),bvh(nullptr),kd(nullptr),num_LOD(1),num_instance(1){file_path[0]=mtl_path[0]=L'\0';};
	void release(){ if(file_path) free(file_path); file_path=nullptr; if(mtl_path) free(mtl_path); mtl_path=nullptr; if(!vertices.empty()){ vertices.clear(); vertices.shrink_to_fit(); } if(!indices.empty()){ indices.clear(); indices.shrink_to_fit(); } if(!geometries.empty()){ geometries.clear(); geometries.shrink_to_fit(); } for(uint i=0;i<objects.size();i++) delete objects[i]; objects.clear(); objects.shrink_to_fit(); for(uint i=0;i<materials.size();i++) delete materials[i]; materials.clear(); materials.shrink_to_fit(); if(bvh){ bvh->release(); bvh=nullptr; } if(kd){ kd->release(); kd=nullptr; } }

	// query
	uint num_base_geometries(){ return uint(geometries.size())/num_LOD; }
	uint num_faces( int level=0 ){ uint ng=num_base_geometries(); geometry* pg=&geometries[0]+ng*level; uint n=0;for(uint k=0;k<ng;k++) n+=pg[k].num_faces(); return n; }
	object*	find_object( const char* name ){ for(uint k=0;k<objects.size();k++)if(_stricmp(objects[k]->name,name)==0) return objects[k];return nullptr;}

	// bound
	inline void update_bound( bool bRecalcTris=false );

	// linear intersections
	inline bool intersect( const ray& r, std::vector<uint>* hit_prim_list=nullptr );
	inline bool intersect( const ray& r, isect* pi=nullptr, bool use_backface=false );
};

inline void mesh::update_bound( bool bRecalcTris )
{
	for(uint k=0,kn=uint(objects.size());k<kn;k++) objects[k]->box.clear();
	geometry* g = &geometries[0]; for(uint k=0,kn=uint(geometries.size()),gn=kn/num_instance;k<kn;k++,g++)
	{
		if(g->instance>0) g->box = geometries[k%gn].box;
		else if(bRecalcTris){ g->box.clear(); vertex* V=&vertices[0]; uint* I=&indices[0]; for(uint j=g->indexStart,jn=j+g->indexCount;g->instance==0&&j<jn;j+=3) g->box.expand(V[I[j+0]].pos,V[I[j+1]].pos,V[I[j+2]].pos); }
		g->parent->box.expand( g->parent->mtx*g->box );
	}
	box.clear(); for( uint k=0, kn=uint(objects.size()); k<kn; k++ ) box.expand( objects[k]->box );
}

//*******************************************************************
// Volume data format
struct volume
{
	wchar_t	filePath[260];
	uint	width, height, depth;
	vec3	voxelSize;		// size of a unit voxel
	uint	format;			// GL_R32F or GL_RGBA32F
	bbox	box;			// bounding box
	float*	data;			// voxel data array: cast by format
	vec4	table[256];		// transfer function: usually 256 size
};

//*******************************************************************
// late implementations for objects/geometry
inline mat4& geometry::mtx(){ return parent->mtx; }
inline const char* geometry::name(){ return parent->name; }
inline float geometry::surface_area(){ if(indexCount==0||parent==nullptr||parent->parent==nullptr) return 0.0f; mesh* pMesh = parent->parent; if(pMesh->vertices.empty()||pMesh->indices.empty()) return 0.0f; vertex* v = &pMesh->vertices[0]; uint* i = &pMesh->indices[0]; float sa = 0.0f; for( uint k=indexStart, kn=k+indexCount; k<kn; k+=3 ) sa += (v[i[k+1]].pos-v[i[k+0]].pos).cross(v[i[k+2]].pos-v[i[k+0]].pos).length()*0.5f; return sa; }
inline void object::setLOD( float level ){ LOD=clamp(level,0.0f,float(parent->num_LOD-1)); }
inline uint object::num_geometries(){ uint n=0; geometry* pg=&parent->geometries[0]; for( uint k=0, kn=uint(parent->geometries.size()); k<kn; k++ ) if(pg[k].level==0&&pg[k].parent==this) n++; return n; }
inline std::vector<geometry*> object::find_geometries( int level ){ geometry* pg=&parent->geometries[0]; std::vector<geometry*> gl; for( uint k=0,kn=uint(parent->geometries.size()); k<kn; k++ ) if(pg[k].level==level&&pg[k].parent==this) gl.push_back(&pg[k]); return gl; }
inline uint object::num_faces( int level ){ geometry* pg=&parent->geometries[0]; uint f=0; for( uint k=0,kn=uint(parent->geometries.size()); k<kn; k++ ) if(pg[k].level==level&&pg[k].parent==this) f+=pg[k].num_faces(); return f; }

//*******************************************************************
// intersection implementations
inline ray gen_primary_ray( camera* pCam, float x, float y )	// (x,y) in [0,1]
{
	const vec3& eye=pCam->eye, center=pCam->center, up=pCam->up;
	float fh = tan(deg2rad(pCam->fovy)*0.5f)*2.0f, fw=fh*pCam->aspectRatio;	// frustum height/width in NDC; you may use pCam->frustum_size()
	vec3 epos = vec3( fw*(x-0.5f), fh*(y-0.5f), -1.0f );					// pixel position on the image plane: make sure to have negative depth
	mat4 I = mat4::lookAtInv(eye,center,up);								// inverse view matrix
	return ray( eye, (I*epos-eye).normalize(), 0.0f, FLT_MAX );
}

// triangle intersection
inline bool intersect( const ray& r, const vec3& v0, const vec3& v1, const vec3& v2, isect* pi=nullptr, bool use_backface=false )	// iset=(pos,t), bc=(s,t)
{
	// http://geomalgorithms.com/a06-_intersect-2.html#intersect3D_RayTriangle
	if(pi) pi->hit = false;

	vec3 u=v1-v0, v=v2-v0, n=u.cross(v); if(n.length()==0) return false;	// degenerate case: non-triangle
	float b=dot(n,r.dir); if(!use_backface&&b>-0.00001f) return false;		// skip backfaces or rays lying on the plane
	float t=dot(n,v0-r.pos)/b; if(t<r.tnear||t>r.tfar) return false;		// out of range of [tnear,tfar]
	vec3 ipos=r.pos+t*r.dir, w=ipos-v0;
	
	// barycentric coord test
    float uu=dot(u,u), uv=dot(u,v), vv=dot(v,v), wu=dot(w,u), wv=dot(w,v), D=uv*uv-uu*vv;
    float bs = (uv*wv-vv*wu)/D; if( bs < 0.0f || bs > 1.0f )		return false;
    float bt = (uv*wu-uu*wv)/D; if( bt < 0.0f || (bs + bt) > 1.0f )	return false;

	if(pi)
	{
		pi->pos = ipos.xyz;
		pi->norm = n.normalize();
		pi->bc = vec2(bs,bt);
		pi->t = t;
		pi->tfar = t;
		pi->hit = true;
	}

	return true;
}

// box intersection
inline bool bbox::intersect( const ray& r, isect* pi )
{
	if(pi) pi->hit = false;

	float t0=r.tnear, t1=r.tfar;
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

inline bool geometry::intersect( const ray& r, isect* pi, bool use_backface )
{
	if(pi) pi->hit = false;
	mesh* pMesh=parent->parent;
	isect m;

	const mat4& gm=this->mtx();
	const vertex* V = &pMesh->vertices[0];
	const uint* I = &pMesh->indices[indexStart];
	for( uint k=0, kn=indexCount; k<kn; k+=3 )
	{
		const vertex& v0=V[I[k+0]], v1=V[I[k+1]], v2=V[I[k+2]];
		isect i; if(!::intersect(r,gm*v0.pos,gm*v1.pos,gm*v2.pos,&i,use_backface)||i.t>m.t) continue;
		m=i;
	}
	
	if(pi&&m.hit)
	{
		pi->g = this;
		pi->pos = m.pos;
		pi->norm = m.norm;
		pi->bc = m.bc;
		pi->t = m.t;
		pi->tfar = m.tfar;
		pi->hit = true;
	}

	return m.hit;
}

inline bool mesh::intersect( const ray& r, std::vector<uint>* hit_prim_list )
{
	std::vector<uint> m;
	for( uint k=0, kn=uint(geometries.size()); k<kn; k++ )
	{
		if(!(geometries[k].mtx()*geometries[k].box).intersect(r,nullptr)) continue;
		m.push_back(uint(k));
	}
	if(hit_prim_list) *hit_prim_list = m;
	return !m.empty();
}

inline bool mesh::intersect( const ray& r, isect* pi, bool use_backface )
{
	isect m;
	std::vector<uint> hit_prim_list;
	if(!intersect( r, &hit_prim_list )) return false;

	for( uint k=0; k < uint(hit_prim_list.size()); k++ )
	{
		isect i; if(!geometries[hit_prim_list[k]].intersect(r,&i,use_backface)||i.t>m.t) continue;
		m=i;
	}
	if(m.hit&&pi) *pi = m;
		
	return m.hit;
}

inline bool BVH::intersect( const ray& r, std::vector<uint>* hit_prim_list )
{
	// prepare the traversal
	std::vector<uint> m;
	uchar3 neg = { uchar(r.d.x<0.0f?1:0), uchar(r.d.y<0.0f?1:0), uchar(r.d.z<0.0f?1:0) };

	uint stack[512];
	for( int s=stack[0]=0, n=0; s>=0; s-- )	// s: stack pointer, n: node id
	{
		BVH::node& b = nodes[n=stack[s]];
		if(!b.box.intersect(r))	continue;	// no intersection: process next stack element
		else if( b.nprims==0 ){ stack[s+neg[b.axis]]=n+1; stack[s+!neg[b.axis]]=b.second; s+=2; }	// intersection on interior: push the children
		else m.push_back(b.idx);			// add the leaf nodes
	}
	if(hit_prim_list) *hit_prim_list = m;
	return !m.empty();
}

inline bool BVH::intersect( const ray& r, isect* pi, bool use_backface )
{
	if(pi) pi->hit = false;
	
	std::vector<uint> hit_prim_list;
	if(geom!=-1) hit_prim_list.push_back(geom);
	else if(!intersect( r, &hit_prim_list )) return false;

	// traverse the primitive-level BVH on hit geometries
	isect	m;
	uchar3	neg = { uchar(r.d.x<0.0f?1:0), uchar(r.d.y<0.0f?1:0), uchar(r.d.z<0.0f?1:0) };
	const vertex*	V = &pMesh->vertices[0];
	const uint*		I = &pMesh->indices[0];

	uint stack[512];
	for( uint k=0, kn=uint(hit_prim_list.size()); k<kn; k++ )
	{
		geometry* g = &pMesh->geometries[0]+hit_prim_list[k];
		
		// perform normal intersection test
		if(g->bvh==nullptr){ isect i; if(g->intersect(r,&i,use_backface)&&i.t<m.t){ m=i; m.g=g; } continue; } 

		// BVH intersection test
		BVH::node* B = &(g->bvh->nodes[0]);
		const mat4& gm = g->mtx();
		for( int s=stack[0]=0, n=0; s>=0; s-- )	// s: stack pointer, n: node id
		{
			BVH::node& b = B[n=stack[s]];
			if(!b.box.intersect(r))	continue;	// no intersection: process next stack element
			else if( b.nprims==0 ){ stack[s+neg[b.axis]]=n+1; stack[s+!neg[b.axis]]=b.second; s+=2; } // intersection on interior: push the children
			else // intersection on leaf nodes
			{
				const vertex& v0=V[I[b.idx+0]], v1=V[I[b.idx+1]], v2=V[I[b.idx+2]];
				isect i; if(!::intersect(r,gm*v0.pos,gm*v1.pos,gm*v2.pos,&i,use_backface)||i.t>m.t) continue;
				m = i;
				m.g = g;
			}
		}
	}

	if(pi) *pi = m;

	return m.hit;
}