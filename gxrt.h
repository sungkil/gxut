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
#ifndef __GX_RT_H__
#define __GX_RT_H__

#if __has_include("gxmesh.h")
	#include "gxmesh.h"
#endif
#if __has_include("gxsampler.h")
	#include "gxsampler.h"
#endif

// timestamp to indicate struct changes in other files
static const char* __GX_RT_H_TIMESTAMP__ = _strdup(__TIMESTAMP__);

//*************************************
// 16-byte aligned light for direct lights
#ifndef __cplusplus // std140 definition for shaders
struct area_light_t { vec4 pos, color, normal; vec3 u, v; }; // uv: scaled basis vectors to sample points with [-0.5,0.5]
struct vpl_t
{
	vec4	pos, color, normal;		// normal: use only normal.xyz
	mat4	view_matrix, projection_matrix;
	float	fovy, aspect, dnear, dfar;
};
#else
struct area_light_t : public light_t
{
	alignas(16) vec3 u, v; // uv: scaled basis vectors to sample points with [-0.5,0.5]
	vec3	get_sample( vec2 subpixel ) const { vec2 s=subpixel-0.5f; return pos.xyz + u*s.x + v*s.y + normal*0.001f; } // subpixel in [0,1], add slight margin to avoid self-intersection
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

static_assert(sizeof(area_light_t)%16==0, "size of struct area_light_t should be aligned at 16-byte boundaries");
static_assert(sizeof(vpl_t)%16==0,	 "size of struct vpl_t should be aligned at 16-byte boundaries");
#endif

// overloading light transformations to camera space
inline area_light_t operator*( const mat4& view_matrix, const area_light_t& light ){ area_light_t l=light; l.pos.xyz=l.pos.a==0?(mat3(view_matrix)*l.pos.xyz):(view_matrix*l.pos).xyz; mat3 rotation_matrix=mat3(view_matrix); l.normal=rotation_matrix*light.normal; l.u=rotation_matrix*light.u; l.v=rotation_matrix*light.v; return l; }
inline vector<area_light_t> operator*( const mat4& view_matrix, const vector<area_light_t>& lights ){ vector<area_light_t> v; if(lights.empty()) return v; v.reserve(lights.size()); for( const auto& l : lights ) v.emplace_back(view_matrix*l); return v; }
inline vector<area_light_t> operator*( const mat4& view_matrix, const vector<area_light_t>* lights ){ vector<area_light_t> v; if(!lights||lights->empty()) return v; v.reserve(lights->size()); for(auto& l:*lights) v.emplace_back(view_matrix*l); return v; }

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
	vector<node> nodes;

	~bvh_t(){}
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
		uint second_or_index:30, axis:2;
		__forceinline bool is_leaf() const { return axis==3; }
	};
	vector<node>	nodes;
	vector<ivec2>	prims; // ordered primitives (face index, geometry index)
	virtual bool intersect( ray r, isect& h ) const;
};

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
		for(uint k=0; k<8; k++){ auto& v = proxy->vertices[size_t(8llu*g.ID+k)]; v.pos = (m*vec4(v.norm,1.0f)).xyz; }
	}
}

//*************************************
// late implementations

// primary ray
__noinline ray gen_primary_ray( camera* cam, float x, float y )	// (x,y) in [0,1]
{
	const vec3& eye=cam->eye.xyz, center=cam->center.xyz, up=cam->up.xyz;
	float fh=tan(cam->fovy*0.5f)*2.0f, fw=fh*cam->aspect;		// frustum height/width in NDC
	vec3 dst=normalize(vec3(fw*(x-0.5f),fh*(y-0.5f),-1.0f));	// target pixel position at d=-1: make sure to have negative depth
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

// ray-ray intersection: find the closest points on two 3D rays, which may skew in space
__noinline vec2 intersect_rays( vec3 ro, vec3 rd, vec3 po, vec3 pd )
{
	vec3 a=rd, b=pd, c=(po-ro);
	vec3 ab=cross(a,b), ca=cross(c,a), cb=cross(c,b);
	float l=dot(ab,ab); if(l==0) return vec2(0,0); // parallel lines
	//if(dot(c,ab)!=0) return 0.0f; // non-coplanar rays, which mostly happens due to limited floating-point precision
	return vec2(dot(cb,ab),dot(ca,ab))/l; // t of the closest point on (r,p)
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

// TBN (tangent-binormal-normal) matrix
template <bool bump_flavor=false> mat3 tbn_matrix( vec3 normal );
template <> __forceinline mat3 tbn_matrix<false>( vec3 normal ) // used in ray tracer
{
	vec3 t = normalize(cross(abs(normal.x)>0.0f?vec3(0,1,0):vec3(1,0,0),normal)); // tangent: typical choice in path tracer
	vec3 b = normalize(cross(normal,t)); // binormal
	return mat3{t.x,b.x,normal.x,t.y,b.y,normal.y,t.z,b.z,normal.z};
}
template <> __forceinline mat3 tbn_matrix<true>( vec3 normal ) // used in normal/bump mapping
{
	// http://www.ozone3d.net/tutorials/mesh_deformer_p2.php#tangent_space
	vec3 c = cross(normal,vec3(0,0,1)), d = cross(normal,vec3(0,1,0));
	vec3 t = normalize(length(c)>length(d)?c:d); // tangent
	vec3 b = normalize(cross(normal,t)); // binormal
	return mat3{t.x,b.x,normal.x,t.y,b.y,normal.y,t.z,b.z,normal.z};
}

// find an up vector from mesh
__noinline int find_up_vector( mesh* p_mesh )
{
	if(!p_mesh) return 2;
	if(p_mesh->box.max_extent()>(p_mesh->box.min_extent()*4.0f)) return p_mesh->box.min_axis();

	vec3 d=0;
	for(auto f:{"floor","ground","ceil","terrain","plane"})
	for(auto& o:p_mesh->objects)
	{
		char buff[4096]; strcpy(buff,o.name);
		if(!strstr(_strlwr(buff),f)||o.box.max_extent()<(o.box.min_extent()*4.0f)) continue;
		d[o.box.min_axis()] += 1.0; break;
	}
	
	int a=2;
	if(d[0]>0&&d[0]>d[2]) a=0;
	if(d[1]>0&&d[1]>d[a]) a=1;
	return a;
}

// find area lights from mesh
__noinline vector<area_light_t> find_area_lights( mesh* p_mesh )
{
	vector<area_light_t> lights; if(!p_mesh) return lights;
	// aliases
	auto& vertices = p_mesh->vertices;
	auto& indices = p_mesh->indices;
	for( const auto& g : p_mesh->geometries )
	{
		const auto& m = p_mesh->materials[g.material_index]; if(m.bsdf!=BSDF_EMISSIVE) continue;
		if(g.count!=6){ printf("%s(%s): no support other than two-triangle quads\n", __func__, g.name() ); continue; }
		
		// first find the indices of shared edge and isolated vertices
		std::map<uint,uint> c; // index counter
		for( uint k=g.first_index, kn=k+g.count; k<kn; k++ ){ auto i=indices[k]; auto it=c.find(i); c[i]=1+(it==c.end()?0:it->second); }
		ivec2 shared={-1,-1}, isolated={-1,-1}; uint s=0, i=0; for(auto [k,n]:c){ if(s<2&&n>1) shared[s++]=k; if(i<2&&n==1) isolated[i++]=k; }
		if(shared.x<0||shared.y<0){ printf("%s(%s): no shared edge found\n", __func__, g.name() ); continue; }
		if(isolated.x<0||isolated.y<0){ printf("%s(%s): no isolated vertices found\n", __func__, g.name() ); continue; }

		// take the min index as the origin
		vec3 o = vertices[shared.x].pos;
		vec3 u = vertices[isolated.x].pos-o;
		vec3 v = vertices[isolated.y].pos-o;
		vec3 n = normalize(cross(u,v));

		// find the first normal
		vec3 a0 = vertices[indices[g.first_index+1]].pos-vertices[indices[g.first_index]].pos;
		vec3 b0 = vertices[indices[g.first_index+2]].pos-vertices[indices[g.first_index]].pos;
		vec3 n0 = cross(a0,b0);

		// take the correct isolated indices by comparing normals
		area_light_t l;
		l.color = m.color;
		l.bounce = 0;
		l.mouse = 0;
		l.geometry = g.ID;

		// fill the half uv and normal
		if(dot(n,n0)>0){ l.u=u; l.v=v; l.normal=n; } else { l.u=v; l.v=u; l.normal=-n; }
		l.pos = vec4( o + (l.u+l.v)*0.5f, 1.0f ); // take the center as pos
		lights.emplace_back(l);
	}

	return lights;
}

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

#endif // __GX_RT_H__
