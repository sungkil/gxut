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
#ifndef __GX_SAMPLER_H__
#define __GX_SAMPLER_H__

#if defined(__has_include) && __has_include("gxmath.h")
	#include "gxmath.h" // the only necessary header for gxmesh
#endif

//*************************************
// sampler interface (for lens/rays): vec4(x,y,z,weight) in a unit surface
struct isampler_t
{
	enum model_t { HAMMERSLEY, HALTON, POISSON, PMJ };
	enum surface_t { SQUARE, CIRCLE, HEMISPHERE, COSHEMI, SPHERE, CYLINDER }; // COSHEMI: cosine-weighted hemisphere
	
	model_t			model=POISSON;
	surface_t		surface=CIRCLE;
	uint			seed=0;		// random seed
	const uint		n=1;
};

template <size_t max_samples=65536> // up to 64K samples
struct tsampler_t : public isampler_t
{
	uint			crc;		// crc32c to detect the change of samples
	uint			index=0;	// index for sequential sampling

	constexpr uint	capacity() { return max_samples; }
	bool			empty() const { return n==0; }
	bool			dirty() const { return crc!=tcrc32<>(&model,sizeof(isampler_t)); }
	uint			size() const { return n; }
	const vec4*		begin() const { return &_data[0]; }
	const vec4*		end() const { return begin() + n; }
	const vec4&		operator[]( ptrdiff_t i ) const { return _data[i]; }
	const vec4&		at( ptrdiff_t i ) const { return _data[i]; }
	void			resize( uint new_size, bool b_resample=true ){ const_cast<uint&>(n)=new_size<uint(max_samples)?new_size:uint(max_samples);if(b_resample)resample();}
	void			rewind(){ index=0; }
	const vec4&		next(){ index=(++index)%n; return _data[index]; } // only for fixed sequence; needs to be improved for sequential sampling
	uint			resample(); // return the number of generated samples
	
protected:
	virtual void	generate( vec4* v, uint n );
	void			normalize();
	void			reshape( surface_t dst );

	const std::array<vec4, max_samples> _data;
};

using sampler_t = tsampler_t<>;

// make the center position of samples to the origin
template <size_t max_samples>
inline void tsampler_t<max_samples>::normalize()
{
	if(empty()||surface==HEMISPHERE||surface==SPHERE||surface==CYLINDER) return;
	dvec3 m=dvec3(0,0,0); vec4* v=(vec4*)&_data[0]; for(size_t k=0;k<n;k++) m+=dvec3(v[k].x,v[k].y,v[k].z); m/=double(n);
	vec3 f=vec3(float(-m.x),float(-m.y),float(-m.z));
	for(size_t k=0,kn=size();k<kn;k++)v[k].xyz+=f;
} 

template <size_t max_samples>
inline void tsampler_t<max_samples>::reshape( surface_t dst )
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

template <size_t max_samples>
__noinline inline void tsampler_t<max_samples>::generate( vec4* v, uint n )
{
	if(model==sampler_t::HAMMERSLEY){	uint hammersley_square(vec4*,uint); hammersley_square(v,n); }
	else if(model==sampler_t::HALTON){	uint halton_cube(vec4*,uint);		halton_cube(v,n); }
	else {								uint simple_square(vec4*,uint);		simple_square(v,n); }
}

template <size_t max_samples>
__noinline inline uint tsampler_t<max_samples>::resample()
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
// simple implementations
__noinline inline uint simple_square( vec4* v, uint n ){ for(uint k=0;k<n;k++,v++) v->xyz=vec3{prand2()*2.0f-1.0f,0.0f}; return n; }
__noinline inline uint hammersley_square( vec4* v, uint n ){ const float nf=1.0f/float(n); for(uint k=0;k<n;k++,v++) v->xy=vec2(k*nf,float(bitswap(k)>>8)/float(1<<24))*2.0f-1.0f; return n; } // with radical inverse
__noinline inline uint halton_cube( vec4* v, uint n )
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

#endif // __GX_SAMPLER_H__
