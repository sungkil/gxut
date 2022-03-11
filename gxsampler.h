//*********************************************************
// Copyright 2011-2022 Sungkil Lee
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

#ifdef __has_include
	#if __has_include("gxmath.h")
		#include "gxmath.h"
	#endif
	#if __has_include("gxfilesystem.h")
		#include "gxfilesystem.h"
	#endif
#endif

#include <deque>

//*************************************
// sampler interface (for lens/rays): vec4(x,y,z,weight) in a unit surface
struct isampler_t
{
	enum model_t { HAMMERSLEY, HALTON, POISSON };
	enum surface_t { SQUARE, CIRCLE, HEMISPHERE, COSHEMI, SPHERE, CYLINDER }; // COSHEMI: cosine-weighted hemisphere
	
	model_t			model=POISSON;
	surface_t		surface=CIRCLE;
	uint			seed=0;		// random seed
	const uint		n=1;
};

template <size_t _capacity=4096> // up to 4K samples by default
struct tsampler_t : public isampler_t
{
	uint			crc;		// crc32c to detect the change of samples
	uint			index=0;	// index for sequential sampling

	tsampler_t()	= default;
	tsampler_t( size_t size, bool b_resample=true ){ resize(size,b_resample); }

	constexpr uint	capacity() { return _capacity; }
	bool			empty() const { return n==0; }
	bool			dirty() const { return crc!=tcrc32<>(&model,sizeof(isampler_t)); }
	uint			size() const { return n; }
	const vec4*		begin() const { return &_data[0]; }
	const vec4*		end() const { return begin() + n; }
	const vec4&		operator[]( ptrdiff_t i ) const { return _data[i]; }
	const vec4&		at( ptrdiff_t i ) const { return _data[i]; }
	void			resize( size_t size, bool b_resample=true ){ const_cast<uint&>(n)=uint(size<_capacity?size:_capacity); if(b_resample) resample(); }
	void			rewind(){ index=0; }
	const vec4&		next(){ index=(++index)%n; return _data[index]; } // only for fixed sequence; needs to be improved for sequential sampling
	uint			resample(); // return the number of generated samples
	
protected:
	virtual void	generate( vec4* v, uint n );
	void			normalize();
	void			reshape( surface_t dst );

	std::vector<vec4> _data = std::move(std::vector<vec4>(_capacity));
};

using sampler_t = tsampler_t<>;

// make the center position of samples to the origin
template <size_t _capacity>
inline void tsampler_t<_capacity>::normalize()
{
	if(empty()||surface==HEMISPHERE||surface==SPHERE||surface==CYLINDER) return;
	dvec3 m=dvec3(0,0,0); vec4* v=(vec4*)&_data[0]; for(size_t k=0;k<n;k++) m+=dvec3(v[k].x,v[k].y,v[k].z); m/=double(n);
	vec3 f=vec3(float(-m.x),float(-m.y),float(-m.z));
	for(size_t k=0,kn=size();k<kn;k++)v[k].xyz+=f;
} 

template <size_t _capacity>
inline void tsampler_t<_capacity>::reshape( surface_t dst )
{
	vec4* d=(vec4*)(&_data);

	if(empty()||dst==surface_t::SQUARE) return;
	else if(dst==surface_t::CYLINDER&&model!=sampler_t::HALTON){printf("[Sampler] cylindrical sampling supports only Halton sequences\n" );return;}
	else if(dst==surface_t::CIRCLE||dst==surface_t::CYLINDER||dst==surface_t::COSHEMI)	// cylinder only for Halton sampling
	{
		bool coshemi=dst==surface_t::COSHEMI; // apply Nusselt analog: 2D uniform is top view of cosine-weighted hemisphere
		float r, t, qpi=PI<float>*0.25f;
		for(uint k=0;k<n;k++,d++)
		{
			auto& v=d->xyz; if(v.x==0||v.y==0) continue; // no transform requird on the axis samples
			else if(v.x*v.x>v.y*v.y){ r=fabs(v.x); t=(v.x<v.y?4.0f:0.0f)+(v.y/v.x); } else { r=fabs(v.y); t=(v.x<v.y?2.0f:6.0f)-(v.x/v.y); }
			v.xy = vec2(cos(qpi*t),sin(qpi*t))*r;
			if(coshemi) v.z=sqrt( 1-v.xy.dot(v.xy));
		}
	}
	else if(dst==surface_t::HEMISPHERE)
	{
		for(uint k=0;k<n;k++,d++)
		{
			vec3& v = d->xyz;
			float phi=PI<float>*v.x, y=(1.0f-v.y)*0.5f, theta=acos(y); // [-1,1] to [-PI,PI], [-1,1] to [1,0], [1,0] to [0,PI/2]
			v = vec3(vec2(cos(phi),sin(phi))*sin(theta),cos(theta));
		}
	}
	else if(dst==surface_t::SPHERE)
	{
		for(uint k=0;k<n;k++,d++)
		{
			vec3& v = d->xyz;
			float phi=PI<float>*v.x, s=sqrtf(1-v.y*v.y); // [-1,1] to [-PI,PI], [-1,1] to [0,1]
			v = vec3(vec2(cos(phi),sin(phi))*s,v.y);
		}
	}
}

// forward declarations
uint hammersley_square(vec4*,uint);
uint halton_cube(vec4*,uint);
uint simple_square(vec4*,uint);
uint poisson_disk( vec4* v, uint _count, bool circular, uint seed );

template <size_t _capacity>
__noinline void tsampler_t<_capacity>::generate( vec4* v, uint n )
{
	if(model==HAMMERSLEY)	hammersley_square(v,n);
	else if(model==HALTON)	halton_cube(v,n);
	else if(model==POISSON)	poisson_disk(v,n,surface==CIRCLE,seed);
	else					simple_square(v,n);
}

template <size_t _capacity>
__noinline uint tsampler_t<_capacity>::resample()
{
	if(surface==surface_t::CYLINDER&&model!=sampler_t::HALTON){ printf("[Sampler] cylinder sampling is supported only in Halton sampling\n" ); return 0; }
	
	auto* v = (vec4*)&_data[0];
	sprand(seed);
	generate(v,n);
	if(surface!=SQUARE&&(model!=POISSON||surface!=CIRCLE)) reshape(surface); // reshape to circle, hemisphere, sphere, ...
	if(surface==SQUARE||surface==CIRCLE) for(uint k=0;k<n;k++) v[k].z=0.0f; // set zero at non-used z-component
	float nrf=1.0f/float(n); for(uint k=0;k<n;k++) v[k].w = nrf; // weight
	if(surface==SQUARE||surface==CIRCLE) normalize();
	crc = tcrc32<>(&model,sizeof(isampler_t));	// to detect format change (equivalent to the detection of data change)
	rewind(); // rewind the index
	return n;
}

//*************************************
// late implementations
__noinline uint simple_square( vec4* v, uint n ){ for(uint k=0;k<n;k++,v++) v->xyz=vec3{prand2()*2.0f-1.0f,0.0f}; return n; }
__noinline uint hammersley_square( vec4* v, uint n ){ const float nf=1.0f/float(n); for(uint k=0;k<n;k++,v++) v->xy=vec2(k*nf,float(bitswap(k)>>8)/float(1<<24))*2.0f-1.0f; return n; } // with radical inverse
__noinline uint halton_cube( vec4* v, uint n )
{
	static const int	p1=2, p2=7, p3=11;
	static const float	ip1=1/float(p1), ip2=1/float(p2), ip3=1/float(p3);
	for(uint k=0,kk,a;k<n;k++,v++)
	{
		float x=0;kk=k;for(float p=ip1;kk;p*=ip1,kk>>=1) if(kk&1) x+=p;
		float y=0;kk=k;for(float p=ip2;kk;p*=ip2,kk/=p2) if(a=kk%p2) y+=float(a)*p;
		float z=0;kk=k;for(float p=ip3;kk;p*=ip3,kk/=p3) if(a=kk%p3) z+=float(a)*p;
		v->xyz = vec3(x,y,z)*2.0f-1.0f;
	}
	return n;
}

struct bridson_t
{
	struct cell {	vec2 pos; bool occupied; bool empty(){ return !occupied; } void clear(){ occupied=false; } };
	struct sample { vec2 pos; ivec2	tc; int index( int grid_size ){ return grid_size*tc.y+tc.x; } };

	const bool			circular;
	float				r;
	int					grid_size;
	std::vector<cell>	grid;
	std::vector<vec2>	samples;
	std::deque<sample>	active_queue;
	
	bridson_t( bool circ ):circular(circ){};
	void reset( const int count, float radius ){ r=radius; grid_size=int(ceil(1.0f/cell_size())); grid.resize(grid_size*grid_size); for(auto& c:grid) c.clear(); active_queue.clear(); samples.clear(); samples.reserve(count*3); } /* 3 times more samples */
	float cell_size(){ return r/1.4142135623730950488f; }
	bool in_bound( vec2 v ){ return (!circular&&v.x>=0&&v.x<1.0f&&v.y>=0&&v.y<1.0f)||(circular&&((v.x-0.5f)*(v.x-0.5f)+(v.y-0.5f)*(v.y-0.5f))<0.25f); }
	bool in_grid( ivec2 v ){ return v.x>=0&&v.x<grid_size&&v.y>=0&&v.y<grid_size; }
	ivec2 to_grid( vec2& v ){ return ivec2(int((v.x/cell_size())), int((v.y/cell_size()))); }
	sample gen_intial_sample(){ sample s; s.pos=vec2(0.5f,0.5f); s.tc = to_grid(s.pos); return s; }
	sample gen_annulus( vec2 p ){ sample s; float t = prand()*PI<float>*2.0f; s.pos = p + vec2(cosf(t),sinf(t))*(prand()+1.0f)*r; s.tc = to_grid(s.pos); return s; } 
	static std::array<ivec2,20> gen_neighbors(){ std::array<ivec2,20> n; for(int k=0,y=-2;y<3;y++)for(int x=-2;x<3;x++){ if((x==0&&y==0)||(abs(x)==2&&abs(y)==2)) continue; n[k++]=ivec2(x,y); } return n; }

	int generate( const int count, float radius );
};

__noinline path poisson_disk_cache_path( uint count, bool circular, uint seed )
{
	static path cache_dir = path::temp(false)+L"global\\sampler\\poisson\\";
	static uint hash0 = tcrc32<>(__TIMESTAMP__,sizeof(char)*strlen(__TIMESTAMP__));
	uint hash = hash0;
	hash = tcrc32<>(&count, sizeof(count), hash );
	hash = tcrc32<>(&circular, sizeof(circular), hash );
	hash = tcrc32<>(&seed, sizeof(seed), hash );
	wchar_t b[64]; swprintf(b,64,L"%08x",hash);
	return cache_dir+b;
}

__noinline bool poisson_disk_cache( vec4* v, uint count, bool circular, uint seed )
{
	path cache_path = poisson_disk_cache_path(count,circular,seed); if(!cache_path.exists()) return false;
	if(cache_path.file_size()!=sizeof(vec4)*count) return false;
	FILE* fp = _wfopen( cache_path.c_str(), L"rb" ); if(!fp) return false;
	size_t read_count = fread( v, sizeof(vec4), count, fp );
	fclose(fp);
	return read_count==count;
}

__noinline void poisson_disk_save( vec4* v, uint count, bool circular, uint seed )
{
	path cache_path = poisson_disk_cache_path(count,circular,seed);
	if(!cache_path.dir().exists()) cache_path.dir().mkdir();
	FILE* fp = _wfopen( cache_path.c_str(), L"wb" ); if(!fp){ printf("unable to open %s\n",cache_path.wtoa()); return; }
	fwrite( v, sizeof(vec4), count, fp );
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
				cell& sn = grid[n.y*grid_size+n.x]; if(sn.empty()) continue;
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

__noinline uint poisson_disk( vec4* v, uint _count, bool circular, uint seed )
{
	if(poisson_disk_cache(v,_count,circular,seed)) return _count;

	int			count = _count;
	bridson_t	b(circular);
	float r0	= sqrtf((circular?(2.0f/5.0f):(8.0f/(5.0f*PI<float>))))/sqrtf(float(count));
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
	for(k=0,kn=min(count,int(b.samples.size()));k<kn;k++) v[k].xy=b.samples[k]*2.0f-1.0f;

	// save cache
	if(count>=int(_count)) poisson_disk_save( v, _count, circular, seed );

	return count;
}

#endif // __GX_SAMPLER_H__
