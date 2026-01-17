//*******************************************************************
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
//*******************************************************************

#pragma once
#ifndef __GX_STRING_H__
#define __GX_STRING_H__

// include gxut.h without other headers
#ifndef __GXUT_H__
#pragma push_macro("__GXUT_EXCLUDE_HEADERS__")
#define __GXUT_EXCLUDE_HEADERS__
#include "gxut.h"
#pragma pop_macro("__GXUT_EXCLUDE_HEADERS__")
#endif

// Byte Order Masks (BOMs) for unicode encoding
static const unsigned char BOM_UTF8[3]	= {0xEF,0xBB,0xBF};
static const unsigned char BOM_UTF16[2]	= {0xFF,0xFE}; // little endian
static const unsigned char BOM_UTF32[4]	= {0xFF,0xFE,0x00,0x00}; // little endian

namespace nocase
{
	template <> struct less<string>{ bool operator()(const string& a,const string& b)const{return stricmp(a.c_str(),b.c_str())<0;}};
	template <> struct less<std::wstring>{ bool operator()(const std::wstring& a,const std::wstring& b)const{return wcsicmp(a.c_str(),b.c_str())<0;}};
	template <> struct equal_to<string>{ bool operator()(const string& a,const string& b)const{return stricmp(a.c_str(),b.c_str())==0;}};
	template <> struct equal_to<std::wstring>{ bool operator()(const std::wstring& a,const std::wstring& b)const{return wcsicmp(a.c_str(),b.c_str())==0;}};
	template <> struct hash<string> { size_t operator()( const string& p ) const { return std::hash<string>()(strlwr(__strdup(p.c_str()))); }};
	template <> struct hash<std::wstring> { size_t operator()( const std::wstring& p ) const { return std::hash<std::wstring>()(strlwr(__strdup(p.c_str()))); }};

	template <class T> using			set = std::set<T,less<T>>;
	template <class T, class V> using	map = std::map<T,V,less<T>>;
	template <class T> using			unordered_set = std::unordered_set<T,hash<T>,equal_to<T>>;
	template <class T, class V> using	unordered_map = std::unordered_map<T,V,hash<T>,equal_to<T>>;
}

#ifdef __msvc__
namespace logical
{
	template <> struct less<std::wstring>{ bool operator()(const std::wstring& a,const std::wstring& b) const { return strcmplogical(a.c_str(),b.c_str())<0;} };
	template <> struct less<string>{ bool operator()(const string& a,const string& b) const { return strcmplogical(a.c_str(),b.c_str())<0;} };
	template <class T> using			set = std::set<T,less<T>>;
	template <class T, class V> using	map = std::map<T,V,less<T>>;
}
#endif

// conversion for POD and POD-like types
inline const char* btoa( bool b ){ return b?"true":"false"; }
inline const char* itoa( int i ){ return format("%d",i); }
inline const char* itoa( long i ){ return format("%d",i); }
inline const char* itoa( short i ){ return format("%d",i); }
inline const char* itoa( int8_t i ){ return format("%d",i); }
inline const char* itoa( int64_t i ){ return format("%lld",i); }
inline const char* utoa( uint u ){ return format("%u",u); }
inline const char* utoa( ulong u ){ return format("%u",u); }
inline const char* utoa( ushort u ){ return format("%u",u); }
inline const char* utoa( uchar u ){ return format("%u",u); }
inline const char* utoa( uint64_t u ){ return format("%llu",u); }
inline const char* ftoa( float f ){ if(fabs(f)<0.00000001f) return "0"; return format("%g",f); }
inline const char* ftoa( double d ){ if(fabs(d)<0.00000001) return "0"; return format("%g",d); }
inline bool		   atob( const char* a ){ return a&&*a&&(stricmp(a,"true")==0||atoi(a)!=0); }
inline uint		   atou( const char* a ){ char* e=nullptr;uint v=(uint)strtoul(a,&e,10); return v; }
inline int64_t	   atoill( const char* a ){ char* e=nullptr;int64_t v=strtoll(a,&e,10); return v; }
inline uint64_t	   atoull( const char* a ){ char* e=nullptr;uint64_t v=strtoull(a,&e,10); return v; }
  
// comma separation for size_t
inline const char* tocomma( uint64_t u ){ const char* a=format("%llu",u); if(u<1000) return a; size_t l=strlen(a); vector<char> v; v.resize(l+1); memcpy(&v[0],a,l+1); for( uint k=((l%3)?(l%3):3); k<l; k+=4,l++ ) v.emplace(v.begin()+k,','); return format("%s",&v[0]); }

// user types to string
inline const char* itoa( const int2& v ){ return format("%d %d",v.x,v.y); }
inline const char* itoa( const int3& v ){ return format("%d %d %d",v.x,v.y,v.z); }
inline const char* itoa( const int4& v ){ return format("%d %d %d %d",v.x,v.y,v.z,v.w); }
inline const char* utoa( const uint2& v ){ return format("%u %u",v.x,v.y); }
inline const char* utoa( const uint3& v ){ return format("%u %u %u",v.x,v.y,v.z); }
inline const char* utoa( const uint4& v ){ return format("%u %u %u %u",v.x,v.y,v.z,v.w); }
inline const char* ftoa( const float2& v ){ return format("%g %g",fabs(v.x)<0.00000001f?0:v.x,fabs(v.y)<0.00000001f?0:v.y); }
inline const char* ftoa( const float3& v ){ return format("%g %g %g",fabs(v.x)<0.00000001f?0:v.x,fabs(v.y)<0.00000001f?0:v.y,fabs(v.z)<0.00000001f?0:v.z); }
inline const char* ftoa( const float4& v ){ return format("%g %g %g %g",fabs(v.x)<0.00000001f?0:v.x,fabs(v.y)<0.00000001f?0:v.y,fabs(v.z)<0.00000001f?0:v.z,fabs(v.w)<0.00000001f?0:v.w); }
inline const char* ftoa( const double2& v ){ return format("%g %g",v.x,v.y); }
inline const char* ftoa( const double3& v ){ return format("%g %g %g",v.x,v.y,v.z); }
inline const char* ftoa( const double4& v ){ return format("%g %g %g %g",v.x,v.y,v.z,v.w); }

// templated string to user types
template <class T> T		atoi( const char* a );
template <class T> T		atou( const char* a );
template <class T> T		atof( const char* a );
template<> inline int		atoi<int>( const char* a ){ return atoi(a); }
template<> inline long		atoi<long>( const char* a ){ return long(atoi(a)); }
template<> inline short		atoi<short>( const char* a ){ return short(atoi(a)); }
template<> inline int8_t	atoi<int8_t>( const char* a ){ return int8_t(atoi(a)); }
template<> inline uint		atou<uint>( const char* a ){ return atou(a); }
template<> inline ulong		atou<ulong>( const char* a ){ return atou(a); }
template<> inline ushort	atou<ushort>( const char* a ){ return ushort(atou(a)); }
template<> inline uint8_t	atou<uint8_t>( const char* a ){ return uint8_t(atou(a)); }
template<> inline float		atof<float>( const char* a ){ return float(atof(a)); }
template<> inline double	atof<double>( const char* a ){ return double(atof(a)); }
template<> inline int2		atoi<int2>( const char* a ){ int2 v={};char* e=nullptr;for(int k=0;k<2;k++,a=e) (&v.x)[k]=int(strtol(a,&e,10)); return v; }
template<> inline int3		atoi<int3>( const char* a ){ int3 v={};char* e=nullptr;for(int k=0;k<3;k++,a=e) (&v.x)[k]=int(strtol(a,&e,10)); return v; }
template<> inline int4		atoi<int4>( const char* a ){ int4 v={};char* e=nullptr;for(int k=0;k<4;k++,a=e) (&v.x)[k]=int(strtol(a,&e,10)); return v; }
template<> inline uint2		atou<uint2>( const char* a ){ uint2 v={};char* e=nullptr;for(int k=0;k<2;k++,a=e) (&v.x)[k]=strtoul(a,&e,10); return v; }
template<> inline uint3		atou<uint3>( const char* a ){ uint3 v={};char* e=nullptr;for(int k=0;k<3;k++,a=e) (&v.x)[k]=strtoul(a,&e,10); return v; }
template<> inline uint4		atou<uint4>( const char* a ){ uint4 v={};char* e=nullptr;for(int k=0;k<4;k++,a=e) (&v.x)[k]=strtoul(a,&e,10); return v; }
template<> inline float2	atof<float2>( const char* a ){ float2 v={};char* e=nullptr;for(int k=0;k<2;k++,a=e) (&v.x)[k]=strtof(a,&e); return v; }
template<> inline float3	atof<float3>( const char* a ){ float3 v={};char* e=nullptr;for(int k=0;k<3;k++,a=e) (&v.x)[k]=strtof(a,&e); return v; }
template<> inline float4	atof<float4>( const char* a ){ float4 v={};char* e=nullptr;for(int k=0;k<4;k++,a=e) (&v.x)[k]=strtof(a,&e); return v; }
template<> inline double2	atof<double2>( const char* a ){ double2 v={};char* e=nullptr;for(int k=0;k<2;k++,a=e) (&v.x)[k]=strtod(a,&e); return v; }
template<> inline double3	atof<double3>( const char* a ){ double3 v={};char* e=nullptr;for(int k=0;k<3;k++,a=e) (&v.x)[k]=strtod(a,&e); return v; }
template<> inline double4	atof<double4>( const char* a ){ double4 v={};char* e=nullptr;for(int k=0;k<4;k++,a=e) (&v.x)[k]=strtod(a,&e); return v; }

// templated generic conversion from user types
template <class T> const char* ntoa( T v );
template<> inline const char* ntoa<bool>( bool v ){ return btoa(v); }
template<> inline const char* ntoa<int>( int v ){ return itoa(v); }
template<> inline const char* ntoa<uint>( uint v ){ return utoa(v); }
template<> inline const char* ntoa<float>( float v ){ return ftoa(v); }
template<> inline const char* ntoa<double>( double v ){ return ftoa(v); }
template<> inline const char* ntoa<int64_t>( int64_t v ){ return itoa(v); }
template<> inline const char* ntoa<uint64_t>( uint64_t v ){ return utoa(v); }
template<> inline const char* ntoa<int2>( int2 v ){ return itoa(v); }
template<> inline const char* ntoa<int3>( int3 v ){ return itoa(v); }
template<> inline const char* ntoa<int4>( int4 v ){ return itoa(v); }
template<> inline const char* ntoa<uint2>( uint2 v ){ return utoa(v); }
template<> inline const char* ntoa<uint3>( uint3 v ){ return utoa(v); }
template<> inline const char* ntoa<uint4>( uint4 v ){ return utoa(v); }
template<> inline const char* ntoa<float2>( float2 v ){ return ftoa(v); }
template<> inline const char* ntoa<float3>( float3 v ){ return ftoa(v); }
template<> inline const char* ntoa<float4>( float4 v ){ return ftoa(v); }
template<> inline const char* ntoa<double2>( double2 v ){ return ftoa(v); }
template<> inline const char* ntoa<double3>( double3 v ){ return ftoa(v); }
template<> inline const char* ntoa<double4>( double4 v ){ return ftoa(v); }

// templated generic conversion from string
template <class T> T		aton( const char* a );
template<> inline bool		aton<bool>( const char* a ){	return atob(a); }
template<> inline int		aton<int>( const char* a ){		return atoi(a); }
template<> inline uint		aton<uint>( const char* a ){	return atou(a); }
template<> inline float		aton<float>( const char* a ){	return float(atof(a)); }
template<> inline double	aton<double>( const char* a ){	return double(atof(a)); }
template<> inline int64_t	aton<int64_t>( const char* a ){	return atoill(a); }
template<> inline uint64_t	aton<uint64_t>( const char* a ){return atoull(a); }
template<> inline int2		aton<int2>( const char* a ){	return atoi<int2>(a); }
template<> inline int3		aton<int3>( const char* a ){	return atoi<int3>(a); }
template<> inline int4		aton<int4>( const char* a ){	return atoi<int4>(a); }
template<> inline uint2		aton<uint2>( const char* a ){	return atou<uint2>(a); }
template<> inline uint3		aton<uint3>( const char* a ){	return atou<uint3>(a); }
template<> inline uint4		aton<uint4>( const char* a ){	return atou<uint4>(a); }
template<> inline float2	aton<float2>( const char* a ){	return atof<float2>(a); }
template<> inline float3	aton<float3>( const char* a ){	return atof<float3>(a); }
template<> inline float4	aton<float4>( const char* a ){	return atof<float4>(a); }
template<> inline double2	aton<double2>( const char* a ){	return atof<double2>(a); }
template<> inline double3	aton<double3>( const char* a ){	return atof<double3>(a); }
template<> inline double4	aton<double4>( const char* a ){	return atof<double4>(a); }

#if defined(__GX_MATH_H__)&&!defined(__cuda__) // type definitions in CUDA/vector_types.h
template<> inline ivec2	atoi<ivec2>( const char* a ){ ivec2 v={};char* e=nullptr;for(int k=0;k<2;k++,a=e) (&v.x)[k]=(int)strtol(a,&e,10); return v; }
template<> inline ivec3	atoi<ivec3>( const char* a ){ ivec3 v={};char* e=nullptr;for(int k=0;k<3;k++,a=e) (&v.x)[k]=(int)strtol(a,&e,10); return v; }
template<> inline ivec4	atoi<ivec4>( const char* a ){ ivec4 v={};char* e=nullptr;for(int k=0;k<4;k++,a=e) (&v.x)[k]=(int)strtol(a,&e,10); return v; }
template<> inline uvec2	atou<uvec2>( const char* a ){ uvec2 v={};char* e=nullptr;for(int k=0;k<2;k++,a=e) (&v.x)[k]=strtoul(a,&e,10); return v; }
template<> inline uvec3	atou<uvec3>( const char* a ){ uvec3 v={};char* e=nullptr;for(int k=0;k<3;k++,a=e) (&v.x)[k]=strtoul(a,&e,10); return v; }
template<> inline uvec4	atou<uvec4>( const char* a ){ uvec4 v={};char* e=nullptr;for(int k=0;k<4;k++,a=e) (&v.x)[k]=strtoul(a,&e,10); return v; }
template<> inline vec2	atof<vec2>( const char* a ){ vec2 v={};char* e=nullptr;for(int k=0;k<2;k++,a=e) (&v.x)[k]=strtof(a,&e); return v; }
template<> inline vec3	atof<vec3>( const char* a ){ vec3 v={};char* e=nullptr;for(int k=0;k<3;k++,a=e) (&v.x)[k]=strtof(a,&e); return v; }
template<> inline vec4	atof<vec4>( const char* a ){ vec4 v={};char* e=nullptr;for(int k=0;k<4;k++,a=e) (&v.x)[k]=strtof(a,&e); return v; }
template<> inline dvec2	atof<dvec2>( const char* a ){ dvec2 v={};char* e=nullptr;for(int k=0;k<2;k++,a=e) (&v.x)[k]=strtod(a,&e); return v; }
template<> inline dvec3	atof<dvec3>( const char* a ){ dvec3 v={};char* e=nullptr;for(int k=0;k<3;k++,a=e) (&v.x)[k]=strtod(a,&e); return v; }
template<> inline dvec4	atof<dvec4>( const char* a ){ dvec4 v={};char* e=nullptr;for(int k=0;k<4;k++,a=e) (&v.x)[k]=strtod(a,&e); return v; }
template<> inline mat2	atof<mat2>( const char* a ){ mat2 m;char* e=0; for(int k=0;k<4;k++,a=e) (&m._00)[k]=strtof(a,&e); return m; }
template<> inline mat3	atof<mat3>( const char* a ){ mat3 m;char* e=0; for(int k=0;k<9;k++,a=e) (&m._00)[k]=strtof(a,&e); return m; }
template<> inline mat4	atof<mat4>( const char* a ){ mat4 m;char* e=0; for(int k=0;k<16;k++,a=e)(&m._00)[k]=strtof(a,&e); return m; }
template<> inline dmat2	atof<dmat2>( const char* a ){ dmat2 m;char* e=0;for(int k=0;k<4;k++,a=e) (&m._00)[k]=strtod(a,&e); return m; }
template<> inline dmat3	atof<dmat3>( const char* a ){ dmat3 m;char* e=0;for(int k=0;k<9;k++,a=e) (&m._00)[k]=strtod(a,&e); return m; }
template<> inline dmat4	atof<dmat4>( const char* a ){ dmat4 m;char* e=0;for(int k=0;k<16;k++,a=e)(&m._00)[k]=strtod(a,&e); return m; }

template<> inline const char* ntoa<ivec2>( ivec2 v ){ return itoa(v); }
template<> inline const char* ntoa<ivec3>( ivec3 v ){ return itoa(v); }
template<> inline const char* ntoa<ivec4>( ivec4 v ){ return itoa(v); }
template<> inline const char* ntoa<uvec2>( uvec2 v ){ return utoa(v); }
template<> inline const char* ntoa<uvec3>( uvec3 v ){ return utoa(v); }
template<> inline const char* ntoa<uvec4>( uvec4 v ){ return utoa(v); }
template<> inline const char* ntoa<vec2>( vec2 v ){ return ftoa(v); }
template<> inline const char* ntoa<vec3>( vec3 v ){ return ftoa(v); }
template<> inline const char* ntoa<vec4>( vec4 v ){ return ftoa(v); }
template<> inline const char* ntoa<dvec2>( dvec2 v ){ return ftoa(v); }
template<> inline const char* ntoa<dvec3>( dvec3 v ){ return ftoa(v); }
template<> inline const char* ntoa<dvec4>( dvec4 v ){ return ftoa(v); }

template<> inline ivec2	aton<ivec2>( const char* a ){	return atoi<ivec2>(a); }
template<> inline ivec3	aton<ivec3>( const char* a ){	return atoi<ivec3>(a); }
template<> inline ivec4	aton<ivec4>( const char* a ){	return atoi<ivec4>(a); }
template<> inline uvec2	aton<uvec2>( const char* a ){	return atou<uvec2>(a); }
template<> inline uvec3	aton<uvec3>( const char* a ){	return atou<uvec3>(a); }
template<> inline uvec4	aton<uvec4>( const char* a ){	return atou<uvec4>(a); }
template<> inline vec2	aton<vec2>( const char* a ){	return atof<vec2>(a); }
template<> inline vec3	aton<vec3>( const char* a ){	return atof<vec3>(a); }
template<> inline vec4	aton<vec4>( const char* a ){	return atof<vec4>(a); }
template<> inline dvec2	aton<dvec2>( const char* a ){	return atof<dvec2>(a); }
template<> inline dvec3	aton<dvec3>( const char* a ){	return atof<dvec3>(a); }
template<> inline dvec4	aton<dvec4>( const char* a ){	return atof<dvec4>(a); }
template<> inline mat3	aton<mat3>( const char* a ){	return atof<mat3>(a); }
template<> inline mat4	aton<mat4>( const char* a ){	return atof<mat4>(a); }
template<> inline dmat3	aton<dmat3>( const char* a ){	return atof<dmat3>(a); }
template<> inline dmat4	aton<dmat4>( const char* a ){	return atof<dmat4>(a); }
#endif

// typed string as a value-string holder
template <class T> struct typed_string
{
	typed_string()=default;
	typed_string( typed_string&& other ) noexcept { data=std::move(other.data); }
	typed_string( const typed_string& other ){ data=other.data; }
	typed_string( T&& f ){ data=ntoa(f); }
	typed_string( const T& f ){ data=ntoa(f); }
	typed_string& operator=( typed_string&& f ){ data=std::move(f.data); return *this; }
	typed_string& operator=( const typed_string& f ){ data=f.data; return *this; }
	typed_string& operator=( T&& f ){ data=ntoa(f); return *this; }
	typed_string& operator=( const T& f ){ data=ntoa(f); return *this; }
	operator const T() const { return aton<T>(data.c_str()); }
	operator T() const && { return aton<T>(data.c_str()); }
	const char* c_str() const { return data.c_str(); }
	bool empty() const { return data.empty(); }
	void clear(){ data.clear(); }
protected:
	string data;
};

// bitwise conversion
template <class T> const char* unpack_bits( const T& v )
{
	char* buff=__strbuf(sizeof(T)*8); buff[sizeof(T)*8]=0;
	for(int k=0,B=int(sizeof(T))-1;B>=0;B--){ for(int b=7;b>=0;b--) buff[k++]=(((const char*)&v)[B]&(1<<b))?'1':'0'; }
	return buff;
}

// fast manual conversion from string to int/float (3x--4x faster than CRT atoi/atof)
namespace fast
{
	__forceinline int atoi( const char* str )
	{
		while(*str==' '||*str=='\t'||*str=='\n'||*str=='\r')str++; // skip leading white spaces
		bool neg=false; if(*str=='-'){neg=true;str++;} else if(*str=='+')str++; // sign
		int i=0;for(;;str++){uint d=static_cast<uint>(*str)-'0';if(d>9)break;i=i*10+d;} // integers
		return neg?-i:i;
	}
	__forceinline int wtoi( const wchar_t* str ){ return atoi(wtoa(str)); }
	__forceinline int atoi( const wchar_t* str ){ return atoi(wtoa(str)); }

	__forceinline double atof( const char* str )
	{
		while(*str==' '||*str=='\t'||*str=='\n'||*str=='\r')str++; // skip leading white spaces
		bool neg=false; if(*str=='-'){neg=true;str++;} else if(*str=='+')str++; // sign
		int i=0;for(;;str++){uint d=static_cast<uint>(*str)-'0';if(d>9)break;i=i*10+d;} double v=double(i); // integers
		if(*str=='.'){double f=0.1;str++;for(;;str++,f*=0.1){uint d=static_cast<uint>(*str)-'0';if(d>9)break;v+=d*f;}} // fractions
		if(*str!='e'&&*str!='E') return neg?-v:v; // early return for non-exponent float
		bool eng=false; str++; if(*str=='-'){eng=true;str++;} else if(*str=='+') str++; // exponent sign
		int e=0;for(;;str++){uint d=static_cast<uint>(*str)-'0';if(d>9)break;e=e*10+d;} if(e>308)e=308; // exponents
		double scale=1;while(e>=8){scale*=1E8;e-=8;} while(e>0){scale*=10.0;e--;} v=eng?v/scale:v*scale; // apply exponents
		return neg?-v:v;
	}
	__forceinline double wtof( const wchar_t* str ){ return atof(wtoa(str)); }
	__forceinline double atof( const wchar_t* str ){ return atof(wtoa(str)); }
}

// hexadecimanal conversion
inline const char* tohex( void* ptr, size_t size ){ unsigned char* u=(unsigned char*)ptr; char *buff=__strbuf(size*2), *b=buff; for(size_t k=0;k<size;k++,u++,b+=2) sprintf(b,"%02x",*u); buff[size*2]=0; return buff; }
inline const char* tohex( int i ){ return tohex(&i,sizeof(i)); }
inline const char* tohex( int2 i ){ return tohex(&i,sizeof(i)); }
inline const char* tohex( int3 i ){ return tohex(&i,sizeof(i)); }
inline const char* tohex( int4 i ){ return tohex(&i,sizeof(i)); }
inline const char* tohex( uint u ){ return tohex(&u,sizeof(u)); }
inline const char* tohex( uint2 u ){ return tohex(&u,sizeof(u)); }
inline const char* tohex( uint3 u ){ return tohex(&u,sizeof(u)); }
inline const char* tohex( uint4 u ){ return tohex(&u,sizeof(u)); }

// default whitespace delimiters
template <class T> const T*		 __whitespaces();
template<> inline const char*	 __whitespaces<char>(){ return " \n\t\r"; }
template<> inline const wchar_t* __whitespaces<wchar_t>(){ return L" \n\t\r"; }

template <class T> const T* ltrim( const T* src, const T* delims=__whitespaces<T>() )
{
	if(!src||!src[0]) return (const T*)L"";
	return __strdup(src+strspn(src,delims));
}

template <class T> const T* ltrim( const T* src, T delim )
{
	if(!src||!src[0]) return (const T*)L"";
	while(*src&&*src==delim){ src++; } return __strdup(src);
}

template <class T> const T* rtrim( const T* src, const T* delims=__whitespaces<T>() )
{
	if(!src||!src[0]) return (const T*)L"";
	return __strdup(src,strlen(src)-strrspn(src,delims));
}

template <class T> const T* rtrim( const T* src, T delim )
{
	if(!src||!src[0]) return (const T*)L"";
	int l=int(strlen(src)); const T* d=src+l-1; for(int k=l-1;*d==delim&&k>=0;k--,d--);
	return __strdup(src,d-src+1);
}

template <class T> const T* trim( const T* src, const T* delims=__whitespaces<T>() )
{
	if(!src||!src[0]) return (const T*)L"";
	const T* r=rtrim(src,delims); return r+(*r?strspn(src,delims):0);
}

template <class T> const T* trim( const T* src, T delim )
{
	if(!src||!src[0]) return (const T*)L"";
	while(*src&&*src==delim){ src++; } if(!src[0]) return (const T*)L"";
	int l=int(strlen(src)); const T* d=src+l-1; for(int k=l-1;*d==delim&&k>=0;k--,d--);
	return __strdup(src,d-src+1);
}

// in-place trim
template <class T> T* itrim( T* src, const T* delims=__whitespaces<T>() )
{
	if(!src||!src[0]) return src; // return as-is
	src += strspn(src,delims);
	src[strlen(src)-strrspn(src,delims)]=0;
	return src;
}

template <class T> const T* trim_comment( const T* src, const char* marker="#" )
{
	if(!src||!src[0]) return (const T*)L"";
	size_t slen=strlen(src),clen=strlen(marker),sc=slen-clen+1;
	T* buff=__strdup(src,slen); const char* cs=marker;
	bool cpp=false;for(int k=0;k<clen-1;k++){if(cs[k]!='/'||cs[k+1]!='/')continue;cpp=true;break;} // detect double slashes
	for(size_t k=0;k<slen;k++)
	{
		T s=buff[k];
		int j;for(j=0;j<clen;j++)
		{
			if(s!=cs[j]) continue;
			if(cs[j]!='/'||!cpp) break;
			if(j<clen-1&&k<slen-1&&buff[k+1]=='/') break;
		}
		if(j<clen){buff[slen=k]=0;break;}
	}
	buff[slen-strrspn(buff,__whitespaces<T>())]=0; // rtrim
	return buff;
}

template <class V, class T=V::value_type::value_type>
__noinline const T* join( const V& v, const T* delims=__strdup<T,char>(" ") )
{
	if(v.empty()) return (const T*)L"";
	std::basic_string<T> s; for( const auto& k : v ){ if(k.empty()) continue; if(!s.empty()) s+=std::basic_string<T>(delims); s+=k; }
	return __strdup(s.c_str());
}

template <template<typename...> class V=vector, class T=char>
__noinline V<std::basic_string<T>> explode( const T* src, const T* delims=__whitespaces<T>() )
{
	using R = std::basic_string<T>; V<R> v; if(!src||!*src) return v;
	if constexpr (requires{v.emplace_back(R());}){ v.reserve(32); for( T *ctx=0, *t=strtok_s(__strdup(src),delims,&ctx); t; t=strtok_s(nullptr,delims,&ctx) ) v.emplace_back(t); return v; }
	else if constexpr (requires{v.emplace(R());}){ for( T *ctx=0, *t=strtok_s(__strdup(src),delims,&ctx); t; t=strtok_s(nullptr,delims,&ctx) ) v.emplace(t); return v; }
	else { static_assert(std::false_type::value, "explode(): unsupported container type V"); }
}

template <class R, class T>
__noinline auto explode( const T* src, const T* delims=__whitespaces<T>() )
{
	auto v = explode<vector,T>(src,delims);
	// if constexpr (std::is_same_v<std::remove_cv_t<std::remove_reference_t<R>>,int>)
	if constexpr (std::is_integral_v<R>){ vector<R> x; for(auto& t:v) x.emplace_back(R(fast::atoi(t.c_str()))); return x; }
	else if constexpr (std::is_floating_point_v<R>){ vector<R> x; for(auto& t:v) x.emplace_back(R(fast::atof(t.c_str()))); return x; }
	else { static_assert(std::false_type::value, "explode_t(): unsupported return type R"); }
}

template <class T>
__noinline vector<std::basic_string<T>>
explode_conservative( const T* src, T delim )
{
	vector<std::basic_string<T>> v; if(!src||!*src) return v; v.reserve(32);
	for(const T *s=src,*e=s;*s&&*e;s=e+1){for(e=s;*e!=delim&&*e;e++){}; v.emplace_back(e>s?__strdup<T>(s,size_t(e-s)):(const T*)L""); }
	return v;
}

template <class T> __noinline const T* substr( const T* _Src, int _Pos, int _Count=0 )
{
	static T *nullstr=(T*)L""; if(!_Src||!*_Src) return nullstr;
	size_t l = strlen(_Src); if(_Pos<0) _Pos+=int(l); if(_Pos>=int(l)) return nullstr; if(_Count<=0) _Count+=int(l);
	return __strdup(_Src+_Pos,size_t(_Pos)+_Count<=l?size_t(_Count):size_t(l-_Pos));
}

template <class T> __noinline const T* str_replace( const T* _Src, const T* _Find, const T* _Replace )
{
	if(!_Find||!*_Find) return __strdup(_Src);	// no change
	int sl=int(strlen(_Src)), fl=int(strlen(_Find)); if(sl<fl) return __strdup(_Src); int rl=int(strlen(_Replace));
	T *s=(T*)_Src, *p=nullptr;
	vector<T> buff; buff.reserve(sl*2llu); while( nullptr!=(p=(T*)strstr(s,_Find)) ){ buff.insert(buff.end(),s,p); if(rl>0) buff.insert(buff.end(),_Replace,_Replace+rl); s=p+fl; }
	buff.insert(buff.end(),s,(T*)(_Src+sl));buff.emplace_back(0);
	return __strdup(&buff[0],buff.size());
}

template <class T> __noinline const T* str_ireplace( const T* _Src, const T* _Find, const T* _Replace )
{
	if(!_Find||!*_Find) return __strdup(_Src);	// no change
	int sl=int(strlen(_Src)), fl=int(strlen(_Find)); if(sl<fl) return __strdup(_Src); int rl=int(strlen(_Replace));
	T *s=(T*)_Src, *p=nullptr;
	vector<T> buff; buff.reserve(sl*2llu); while( nullptr!=(p=(T*)stristr(s,_Find)) ){ buff.insert(buff.end(),s,p); if(rl>0) buff.insert(buff.end(),_Replace,_Replace+rl); s=p+fl; }
	buff.insert(buff.end(),s,(T*)(_Src+sl));buff.emplace_back(0);
	return __strdup(&buff[0],buff.size());
}

template <class T> __noinline const T* str_replace( const T* _Src, T _Find, T _Replace )
{
	T *s=__strdup(_Src), *p=s;
	for(int k=0,l=int(strlen(s));k<l;k++,p++) if(*p==_Find) *p=_Replace;
	return s;
}

template <class T> __noinline const T* str_escape( const T* _Src )
{
	const T* t = _Src;
	t = str_replace( t, __strdup<T,char>("\\"), __strdup<T,char>("\\\\") );
	t = str_replace( t, __strdup<T,char>("\""), __strdup<T,char>("\\\"") );
	t = str_replace( t, __strdup<T,char>("\'"), __strdup<T,char>("\\\'") );
	t = str_replace( t, __strdup<T,char>("\t"), __strdup<T,char>("\\\t") );
	return t;
}

template <class T> __noinline const T* str_remove_tokens( const T* _Src, const T* _Tokens )
{
	if(!_Src||!_Tokens) return (const T*)L"";
	int l=int(strlen(_Src)), m=int(strlen(_Tokens)); if(!l||!m) return (const T*)L"";
	T *dst=__strbuf(l), *d=dst; const T *s=_Src;
	for(int k=0;k<l;k++,s++){ bool t=false; for(int j=0;j<m;j++)if(*s==_Tokens[j]){t=true;break;} if(t) continue; *(d++)=*s; }
	*d=0; return dst;
}

// NaN detection
__noinline bool strnan( const char* s )
{
	if(!stristr(s,"nan")&&!stristr(s,"inf")) return false;
	if(strchr(s,'(')&&strchr(s,')')){ if(stristr(s,"(ind)")||stristr(s,"(snan)")||stristr(s,"(qnan)")||stristr(s,"(0x")) return true; }
	if(stricmp(s,"nan")==0||stricmp(s,"-nan")==0||stristr(s," nan")||stristr(s," -nan")) return true;
	return stricmp(s,"inf")==0||stricmp(s,"-inf")==0||stristr(s," inf")||stristr(s," -inf");
}

// Unicode file name normalization
inline bool is_normalized_string( const wchar_t* s )
{
	return 0!=IsNormalizedString(NormalizationC,s,int(wcslen(s)) );
}

__noinline const wchar_t* normalize_string( const wchar_t* s )
{
	if(is_normalized_string(s)) return __strdup(s);
	int sl=int(wcslen(s)); int dl=NormalizeString(NormalizationC,s,sl+1,nullptr,0);
	wchar_t* d=__strbuf<wchar_t>(dl+1);
	NormalizeString(NormalizationC,s,sl+1,d,dl+1);
	d[dl]=0; // make sure null-terminated for dst
	return d;
}

// common unnecesary unicode symbols to common ansi
__noinline const wchar_t* unicode_symbols_to_ansi( const wchar_t* str )
{
	wchar_t* b=__strdup(str);
	static std::map<int,wchar_t> lut =
	{
		{0x00B7,L'-'},	{0x2022,L'-'},	{0x22C5,L'-'}, // middle dot, small circle, dot operator
		{0x2010,L'-'},	{0x2011,L'-'},	{0x2012,L'-'}, {0x2013,L'-'}, {0x2212,L'-'}, {0xFE58,L'-'}, {0xFE63,L'-'}, // hyphen, non-breaking hyphen, figure dash, En dash, minus sign, small Em dash, small Hyphen-Minus
		{0x2014,L'-'},	{0x2015,L'-'},	// Em dash, horizontal bar
		{0x2018,L'\''},	{0x2019,L'\''},	{0x2032,L'\''},
		{0x201C,L'\"'},	{0x201D,L'\"'},	{0x2033,L'\"'},
	};

	for( size_t k=0, kn=wcslen(b); k<kn; k++ )
	{ auto it=lut.find(b[k]); if(it!=lut.end()) b[k]=it->second; }

	return b;
}

template <class T> __noinline const T* auto_quote( const T* _Src )
{
	size_t l=strlen(_Src); static const T q=T('\"'); if(!*_Src||(_Src[0]==q&&_Src[l-1]==q)) return __strdup(_Src,l);
	const T* s=trim(_Src); if(!strpbrk(s,__strdup<T,char>(" '\t|&<>"))) return __strdup(_Src,l);
	T* b=__strbuf<T>(l+2); b[0]=q; memcpy(b+1,_Src,l*sizeof(T)); b[l+1]=q; b[l+2]=0; return b;
}

template <class T> __noinline T strlcss( const vector<T>& v ) // longest common substring
{
	if(v.empty()) return T();
	T a=v.front();
	for( int k=1, kn=int(v.size()); k<kn; k++ )
	{
		auto& b=v[k];
		int bl=int(b.size()); if(bl==0) return T();
		int al=int(a.size()), l=al<bl?al:bl;
		int j=0; for( ; j<l; j++ ){ if(a[j]!=b[j]) break; } a[j]=0;
	}
	return a;
}

//***********************************************
#endif // __GX_STRING_H__
