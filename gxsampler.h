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
#ifndef __GX_SAMPLER_H__
#define __GX_SAMPLER_H__

#if __has_include("gxmath.h")
	#include "gxmath.h"
#endif
#if __has_include("gxfilesystem.h")
	#include "gxfilesystem.h"
#endif

#include <deque>

//*************************************
// sampler interface (for lens/rays): vec4(x,y,z,weight) in a unit surface
struct isampler_t
{
	enum model_t { SIMPLE, POISSON, HALTON, HAMMERSLEY };
	enum surface_t { SQUARE, CIRCLE, HEMISPHERE, COSHEMI, SPHERE, CYLINDER }; // COSHEMI: cosine-weighted hemisphere
	static vector<string> model_names(){ return {"simple","Poisson","Halton","Hammersley"}; }
	static vector<string> surface_names(){ return {"square","circle","hemisphere","coshemi","sphere","cylinder"}; }
	
	model_t		model=POISSON;
	surface_t	surface=CIRCLE;
	uint		seed=5489u; // std::mt19937::default_seed
	const uint	n=1;

	bool		empty() const { return n==0; }
	size_t		size() const { return size_t(n); }
};

struct sampler_t : public isampler_t
{
	typedef vec4		value_type;
	typedef const vec4*	iterator;	// actually, const_iterator
	typedef const vec4&	reference;	// actually, const_reference

	uint crc;		// crc32c to detect the change of samples
	uint index=0;	// index for sequential sampling

	sampler_t(){ _data.reserve(4096); } // reserve up to 4K samples by default
	sampler_t( size_t size, bool b_resample=true ){ _data.reserve(4096); _data.resize(const_cast<uint&>(n)=uint(size)); if(b_resample) resample(); }

	bool		dirty() const { return crc!=crc32(0,&model,sizeof(isampler_t)); }
	iterator	data() const { return _data.data(); }
	iterator	begin() const { return _data.data(); }
	iterator	end() const { return _data.data()+n; }
	reference	front() const { return _data.front(); }
	reference	back() const { return _data.back(); }
	reference	operator[]( ptrdiff_t i ) const { return _data[i%n]; }
	reference	at( ptrdiff_t i ) const { return _data[i%n]; }
	void		rewind(){ index=0; }
	reference	operator++(){ return _data[(++index)%n]; }						// prefix increment
	reference	operator++(int){ uint i=index; index=(++index)%n; return _data[i]; }	// postfix increment

	void		resize( size_t size ){ _data.resize(const_cast<uint&>(n)=uint(size)); resample(); }
	uint		resample(); // return the number of generated samples
	
protected:
	void		_reshape( vec4* v, surface_t dst );
	void		_make_centered();

	vector<value_type> _data;
};

//*************************************
// per-method implementations
__forceinline vec2 hammersley( uint k, float n_inverse )
{
	return vec2(k*n_inverse,float(bitswap(k)>>8)/float(1<<24)); // with radical inverse
}

__noinline vec2 halton( uint index, uint p2=3 )
{
	const float ip1=0.5f, ip2=1/float(p2);
	uint kk=index;	float x=0;for(float p=ip1;kk;p*=ip1,kk>>=1) if(kk&1) x+=p;
	uint a;kk=index;float y=0;for(float p=ip2;kk;p*=ip2,kk/=p2) if(a=kk%p2) y+=float(a)*p;
	return vec2(x,y);
}

__noinline vec3 halton3( uint index, uint p2=7, uint p3=11 )
{
	static const float ip3=1/float(p3);
	uint a,kk=index;float z=0;for(float p=ip3;kk;p*=ip3,kk/=p3) if(a=kk%p3) z+=float(a)*p;
	return vec3(halton(index,p2),z);
}

struct bridson_t
{
	struct cell {	vec2 pos; bool occupied; bool empty(){ return !occupied; } void clear(){ occupied=false; } };
	struct sample { vec2 pos; ivec2	tc; int index( int grid_size ){ return grid_size*tc.y+tc.x; } };

	const bool		circular;
	float			r;
	int				grid_size;
	vector<cell>	grid;
	vector<vec2>	samples;
	std::deque<sample>	active_queue;
	
	bridson_t( bool circ ):circular(circ){};
	void reset( const int count, float radius ){ r=radius; grid_size=int(ceil(1.0f/cell_size())); grid.resize(size_t(grid_size)*grid_size); for(auto& c:grid) c.clear(); active_queue.clear(); samples.clear(); samples.reserve(size_t(count)*3); } /* 3 times more samples */
	float cell_size(){ return r/1.4142135623730950488f; }
	bool in_bound( vec2 v ){ return (!circular&&v.x>=0&&v.x<1.0f&&v.y>=0&&v.y<1.0f)||(circular&&((v.x-0.5f)*(v.x-0.5f)+(v.y-0.5f)*(v.y-0.5f))<0.25f); }
	bool in_grid( ivec2 v ){ return v.x>=0&&v.x<grid_size&&v.y>=0&&v.y<grid_size; }
	ivec2 to_grid( vec2& v ){ return ivec2(int((v.x/cell_size())), int((v.y/cell_size()))); }
	sample gen_intial_sample(){ sample s={}; s.pos=vec2(0.5f,0.5f); s.tc = to_grid(s.pos); return s; }
	sample gen_annulus( vec2 p ){ sample s={}; float t = prand()*pi2; s.pos = p + vec2(cosf(t),sinf(t))*(prand()+1.0f)*r; s.tc = to_grid(s.pos); return s; } 
	static std::array<ivec2,20> gen_neighbors(){ std::array<ivec2,20> n={}; for(int k=0,y=-2;y<3;y++)for(int x=-2;x<3;x++){ if((x==0&&y==0)||(abs(x)==2&&abs(y)==2)) continue; n[k++]=ivec2(x,y); } return n; }

	int generate( const int count, float radius );
};

struct _poisson_disk_cache_t
{
	static ::path_t path( uint count, bool circular, uint seed );
	static bool load( vector<vec2>& v, uint count, bool circular, uint seed );
	static void save( const vector<vec2>& v, bool circular, uint seed );
};

__noinline ::path_t _poisson_disk_cache_t::path( uint count, bool circular, uint seed )
{
	static auto cache_dir = apptemp()+"sampler\\poisson\\"s;
	static uint crc0 = crc32(0,__TIMESTAMP__,strlen(__TIMESTAMP__)*sizeof(char));
	uint crc = crc32(crc0, &count, sizeof(count) );
	crc = crc32(crc, &circular, sizeof(circular) );
	crc = crc32(crc, &seed, sizeof(seed) );
	char b[64]; snprintf(b,64,"%08x",crc);
	return (cache_dir+b).c_str();
}

__noinline bool _poisson_disk_cache_t::load( vector<vec2>& v, uint count, bool circular, uint seed )
{
	auto cache_path = path(count,circular,seed); if(!cache_path.exists()) return false;
	if(cache_path.file_size()!=sizeof(vec4)*count) return false;
	FILE* fp = fopen(cache_path.c_str(),"rb"); if(!fp) return false;
	size_t read_count = fread( v.data(), sizeof(vec2), count, fp);
	fclose(fp);
	return read_count==count;
}

__noinline void _poisson_disk_cache_t::save( const vector<vec2>& v, bool circular, uint seed )
{
	if(v.empty()) return;
	auto cache_path = path(uint(v.size()), circular, seed);
	if(!cache_path.dir().exists()) cache_path.dir().mkdir();
	FILE* fp = fopen(cache_path.c_str(),"wb"); if(!fp){ printf("unable to open %s\n", cache_path.c_str()); return; }
	fwrite( v.data(), sizeof(vec2), v.size(), fp);
	fclose(fp);
}

__noinline int bridson_t::generate( const int count, float radius )
{
	// 0. reset all
	reset(count,radius);
	static const std::array<ivec2,20> neighbors = gen_neighbors();
		
	// 1. generate first sample
	const float r2 = r*r;
	sample s0 = std::move(gen_intial_sample());
	active_queue.emplace_back(s0);
	samples.emplace_back(s0.pos);
	grid[s0.index(grid_size)] = {s0.pos,true};

	// 3. process
	int i=0,in=count*2-1; for(i=0; !active_queue.empty()&&i<in; i++ )
	{
		int j = urand()%active_queue.size();
		sample p = active_queue[j];
			
		static const int kn=30;
		int k; for(k=0;k<kn;k++)
		{
			// generate a new sample q around p with [r,2r]
			sample q = std::move(gen_annulus(p.pos));
			if(!in_bound(q.pos)) continue;				// out-of-bound sample
			if(!grid[q.index(grid_size)].empty()) continue;	// already occupied

			// test against neighbor cells
			bool bfar = true;
			for(size_t x=0, xn=neighbors.size(); x<xn; x++)
			{
				ivec2 n=q.tc+neighbors[x]; if(!in_grid(n)) continue;
				cell& sn = grid[size_t(n.y)*grid_size+n.x]; if(sn.empty()) continue;
				if((q.pos-sn.pos).length2()>r2) continue;
				bfar = false; break;
			}

			if(!bfar) continue;

			active_queue.emplace_back(q);
			samples.emplace_back(q.pos);
			grid[q.index(grid_size)] = {q.pos,true};
		}

		if(k==kn) active_queue.erase(active_queue.begin()+j);
	}
		
	return int(samples.size());
}

__noinline vector<vec2> poisson_disk( uint _count, bool circular, uint seed )
{
	vector<vec2> v;
	if(_poisson_disk_cache_t::load(v,_count,circular,seed)) return v;

	int			count = _count;
	bridson_t	b(circular);
	float r0	= sqrtf((circular?(2.0f/5.0f):(8.0f/(5.0f*pi))))/sqrtf(float(count));
	vec3 r		= vec3(2.0f,0.1f,0.0f)*r0;
	ivec3 n		= ivec3( b.generate(count,r.x), b.generate(count,r.y), 0 );

	// valid bound and re-construnction
	while(n.y<count) n.y=b.generate(count,r.y*=0.5f);
	while(n.x>count) n.x=b.generate(count,r.x*=2.0f);

	// bisection for faster convergence
	uint k, kn=max(64,int(count*0.2f)); for(k=0; k<kn; k++)
	{
		bool conv = fabs(r.y-r.x)<0.00001f;
		float f = float(count-n.x)/(n.y-n.x);			// center fraction
		float jitter = conv?(prand()*2.0f-1.0f):0.0f;	// jittering for annealing
		r.z = max(0.000001f,lerp(r.x,r.y,f+jitter));
		n.z = b.generate(count,r.z);

		if(n.z==count) break;
		if(conv) // do not update r anymore
		{
			if(n.z>count&&n.z<count+max(2,count/512)) break; // discard 1--2 more samples (for faster convergence)
		}
		else if(n.z<count){ n.x=n.z; r.x=r.z; }
		else if(n.z>count){ n.y=n.z; r.y=r.z; }
	}

	if(k==kn||n.y<count) printf( "[Sampler] (%d): undersampling (last=%d) in [%d,%d])\n", count, n.z, n.x, n.y );

	// now copy the result
	v.reserve(count);
	for(k=0,kn=min(count,int(b.samples.size()));k<kn;k++) v.emplace_back(b.samples[k]);

	// save cache
	if(count>=int(_count))
	{
		v.resize(_count);
		_poisson_disk_cache_t::save( v, circular, seed );
	}

	return v;
}

__noinline uint sampler_t::resample()
{
	if(surface==CYLINDER&&model!=HALTON){ printf("[Sampler] cylindrical sampling supports only Halton sequences\n" ); return 0; }
	bool b_circular = surface==CIRCLE||surface==COSHEMI||surface==CYLINDER;

	auto* v = _data.data();
	for(uint k=0;k<n;k++) v[k].z=0.0f; // reset z=zero

	// generate samples
	sprand(seed);
	float nrf =1.0f/float(n);
	if(model==POISSON)
	{
		auto pd = std::move(poisson_disk(n,b_circular,seed));
		for(uint k=0;k<n;k++) v[k].xy = pd[k];
	}
	else if(model==HALTON)
	{
		if(surface==CYLINDER)	for(uint k=0;k<n;k++) v[k].xyz=halton3(k);
		else					for(uint k=0;k<n;k++) v[k].xy=halton(k);
	}
	else if(model==HAMMERSLEY)
	{
		for(uint k=0;k<n;k++) v[k].xy=hammersley(k,nrf);
	}
	else
	{
		for(uint k=0;k<n;k++) v[k].xy=prand2();
	}

	_reshape(_data.data(),surface); // reshape to circle, hemisphere, sphere, ...
	for(uint k=0;k<n;k++) v[k].w=nrf; // weight
	if(surface==CIRCLE) _make_centered();
	crc = crc32(0,&model,sizeof(isampler_t)); // to detect format change (equivalent to the detection of data change)
	rewind(); // rewind the index
	return n;
}

// this simple radial mapping is poor for Halton and Hammersley
inline vec2 square_to_circle( vec2 v )
{
	float t=pi2*v.y;
	return vec2(cos(t),sin(t))*sqrt(v.x);
}

// shirley mapping
// https://www.pbr-book.org/3ed-2018/Monte_Carlo_Integration/2D_Sampling_with_Multidimensional_Transformations
inline vec2 square_to_circle_concentric( vec2 v )
{
	v = v*2.0f-1.0f; if(v.x==0&&v.y==0) return vec2(0);
	constexpr float piover4 = pi*0.25f;
	float r, t; if(abs(v.x)>abs(v.y)){ r=v.x; t=piover4*v.y/v.x; } else { r=v.y; t=piover4*(2.0f-v.x/v.y); }
	return vec2(cos(t),sin(t))*r;
}

inline void sampler_t::_reshape( vec4* v, surface_t dst )
{
	if(empty()||dst==SQUARE) return; // square in [0,1]
	if(dst==CYLINDER&&model!=HALTON) return void(printf("[Sampler] cylindrical sampling supports only Halton sequences\n" ));  // cylinder only for Halton sampling
	
	if(dst==CIRCLE||dst==COSHEMI||dst==CYLINDER)
	{
		// coshemi: apply Nusselt analog (2D uniform is a top view of cosine-weighted hemisphere)
		// cylinder: keep z in [0,1]
		if(model==POISSON){ for(uint k=0; k<n; k++){ auto& s=v[k].xy; s=s*2.0f-1.0f; } }
		else if(model==HAMMERSLEY||model==HALTON){ for(uint k=0;k<n;k++) v[k].xy=square_to_circle_concentric(v[k].xy); }
		else { for(uint k=0;k<n;k++) v[k].xy=square_to_circle(v[k].xy); }
		if(dst==COSHEMI) for(uint k=0;k<n;k++) v[k].z = sqrt(1.0f-v[k].xy.length());
	}
	else if(dst==HEMISPHERE)
	{
		for(uint k=0;k<n;k++)
		{
			vec3& s = v[k].xyz;
			float phi=pi*(s.x*2.0f-1.0f), theta=acos(1.0f-s.y); // [0,1] to [-PI,PI]; [0,1] to [1,0] to [0,PI/2]
			s = vec3(vec2(cos(phi),sin(phi))*sin(theta),cos(theta));
		}
	}
	else if(dst==surface_t::SPHERE)
	{
		for(uint k=0;k<n;k++)
		{
			vec3& s = v[k].xyz;
			float phi=pi*(s.x*2.0f-1.0f), r=sqrtf(4.0f*(1-s.y)*s.y); // [0,1] to [-PI,PI], [0,1] to [0,1]
			s = vec3(vec2(cos(phi),sin(phi))*r,s.y);
		}
	}
}

// make the center position of samples to the origin
inline void sampler_t::_make_centered()
{
	if(empty()||surface==SQUARE||surface==HEMISPHERE||surface==SPHERE||surface==CYLINDER) return;
	dvec3 m=dvec3(0,0,0); vec4* v=_data.data(); for(size_t k=0;k<n;k++) m+=dvec3(v[k].x,v[k].y,v[k].z); m/=double(n);
	vec3 f=vec3(float(-m.x),float(-m.y),float(-m.z));
	for(size_t k=0,kn=size();k<kn;k++)v[k].xyz+=f;
}

#endif // __GX_SAMPLER_H__
