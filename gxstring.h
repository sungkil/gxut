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

namespace logical
{
	template <> struct less<std::wstring>{ bool operator()(const std::wstring& a,const std::wstring& b) const { return strcmplogical(a.c_str(),b.c_str())<0;} };
	template <> struct less<string>{ bool operator()(const string& a,const string& b) const { return strcmplogical(a.c_str(),b.c_str())<0;} };
	template <class T> using			set = std::set<T,less<T>>;
	template <class T, class V> using	map = std::map<T,V,less<T>>;
}

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

// conversion to string types
inline const char* btoa( bool b ){ return b?"true":"false"; }
inline const char* itoa( int i ){ return format("%d",i); }
inline const char* utoa( uint u ){ return format("%u",u); }
inline const char* ftoa( float f ){ if(fabs(f)<0.00000001f) return "0"; return format("%g",f); }
inline const char* dtoa( double d ){ if(fabs(d)<0.00000001) return "0"; return format("%g",d); }
inline const char* illtoa( int64_t i ){ return format("%lld",i); }
inline const char* ulltoa( uint64_t u ){ return format("%llu",u); }
inline const char* itoa( const int2& v ){ return format("%d %d",v.x,v.y); }
inline const char* itoa( const int3& v ){ return format("%d %d %d",v.x,v.y,v.z); }
inline const char* itoa( const int4& v ){ return format("%d %d %d %d",v.x,v.y,v.z,v.w); }
inline const char* itoa( const short2& v ){ int2 i={v.x,v.y};return itoa(i); }
inline const char* itoa( const short3& v ){ int3 i={v.x,v.y,v.z};return itoa(i); }
inline const char* itoa( const short4& v ){ int4 i={v.x,v.y,v.z,v.w};return itoa(i); }
inline const char* itoa( const char2& v ){ int2 i={v.x,v.y};return itoa(i); }
inline const char* itoa( const char3& v ){ int3 i={v.x,v.y,v.z};return itoa(i); }
inline const char* itoa( const char4& v ){ int4 i={v.x,v.y,v.z,v.w};return itoa(i); }
inline const char* utoa( const uint2& v ){ return format("%u %u",v.x,v.y); }
inline const char* utoa( const uint3& v ){ return format("%u %u %u",v.x,v.y,v.z); }
inline const char* utoa( const uint4& v ){ return format("%u %u %u %u",v.x,v.y,v.z,v.w); }
inline const char* utoa( const ushort2& v ){ uint2 u={v.x,v.y};return utoa(u); }
inline const char* utoa( const ushort3& v ){ uint3 u={v.x,v.y,v.z};return utoa(u); }
inline const char* utoa( const ushort4& v ){ uint4 u={v.x,v.y,v.z,v.w};return utoa(u); }
inline const char* utoa( const uchar2& v ){ uint2 u={v.x,v.y};return utoa(u); }
inline const char* utoa( const uchar3& v ){ uint3 u={v.x,v.y,v.z};return utoa(u); }
inline const char* utoa( const uchar4& v ){ uint4 u={v.x,v.y,v.z,v.w};return utoa(u); }
inline const char* ftoa( const float2& v ){ return format("%g %g",fabs(v.x)<0.00000001f?0:v.x,fabs(v.y)<0.00000001f?0:v.y); }
inline const char* ftoa( const float3& v ){ return format("%g %g %g",fabs(v.x)<0.00000001f?0:v.x,fabs(v.y)<0.00000001f?0:v.y,fabs(v.z)<0.00000001f?0:v.z); }
inline const char* ftoa( const float4& v ){ return format("%g %g %g %g",fabs(v.x)<0.00000001f?0:v.x,fabs(v.y)<0.00000001f?0:v.y,fabs(v.z)<0.00000001f?0:v.z,fabs(v.w)<0.00000001f?0:v.w); }
inline const char* dtoa( const double2& v ){ return format("%g %g",v.x,v.y); }
inline const char* dtoa( const double3& v ){ return format("%g %g %g",v.x,v.y,v.z); }
inline const char* dtoa( const double4& v ){ return format("%g %g %g %g",v.x,v.y,v.z,v.w); }

// templated generic conversion
template <class T> const char* ntoa( T v );
template<> inline const char* ntoa<bool>( bool v ){ return btoa(v); }
template<> inline const char* ntoa<int>( int v ){ return itoa(v); }
template<> inline const char* ntoa<uint>( uint v ){ return utoa(v); }
template<> inline const char* ntoa<float>( float v ){ return ftoa(v); }
template<> inline const char* ntoa<double>( double v ){ return dtoa(v); }
template<> inline const char* ntoa<int64_t>( int64_t v ){ return illtoa(v); }
template<> inline const char* ntoa<uint64_t>( uint64_t v ){ return ulltoa(v); }
template<> inline const char* ntoa<int2>( int2 v ){ return itoa(v); }
template<> inline const char* ntoa<int3>( int3 v ){ return itoa(v); }
template<> inline const char* ntoa<int4>( int4 v ){ return itoa(v); }
template<> inline const char* ntoa<short2>( short2 v ){ return itoa(v); }
template<> inline const char* ntoa<short3>( short3 v ){ return itoa(v); }
template<> inline const char* ntoa<short4>( short4 v ){ return itoa(v); }
template<> inline const char* ntoa<char2>( char2 v ){ return itoa(v); }
template<> inline const char* ntoa<char3>( char3 v ){ return itoa(v); }
template<> inline const char* ntoa<char4>( char4 v ){ return itoa(v); }
template<> inline const char* ntoa<uint2>( uint2 v ){ return utoa(v); }
template<> inline const char* ntoa<uint3>( uint3 v ){ return utoa(v); }
template<> inline const char* ntoa<uint4>( uint4 v ){ return utoa(v); }
template<> inline const char* ntoa<ushort2>( ushort2 v ){ return utoa(v); }
template<> inline const char* ntoa<ushort3>( ushort3 v ){ return utoa(v); }
template<> inline const char* ntoa<ushort4>( ushort4 v ){ return utoa(v); }
template<> inline const char* ntoa<uchar2>( uchar2 v ){ return utoa(v); }
template<> inline const char* ntoa<uchar3>( uchar3 v ){ return utoa(v); }
template<> inline const char* ntoa<uchar4>( uchar4 v ){ return utoa(v); }
template<> inline const char* ntoa<float2>( float2 v ){ return ftoa(v); }
template<> inline const char* ntoa<float3>( float3 v ){ return ftoa(v); }
template<> inline const char* ntoa<float4>( float4 v ){ return ftoa(v); }
template<> inline const char* ntoa<double2>( double2 v ){ return dtoa(v); }
template<> inline const char* ntoa<double3>( double3 v ){ return dtoa(v); }
template<> inline const char* ntoa<double4>( double4 v ){ return dtoa(v); }
#if defined(__GX_MATH_H__)&&!defined(__cuda__) // type definitions in CUDA/vector_types.h
template<> inline const char* ntoa<ivec2>( ivec2 v ){ return itoa(v); }
template<> inline const char* ntoa<ivec3>( ivec3 v ){ return itoa(v); }
template<> inline const char* ntoa<ivec4>( ivec4 v ){ return itoa(v); }
template<> inline const char* ntoa<uvec2>( uvec2 v ){ return utoa(v); }
template<> inline const char* ntoa<uvec3>( uvec3 v ){ return utoa(v); }
template<> inline const char* ntoa<uvec4>( uvec4 v ){ return utoa(v); }
template<> inline const char* ntoa<vec2>( vec2 v ){ return ftoa(v); }
template<> inline const char* ntoa<vec3>( vec3 v ){ return ftoa(v); }
template<> inline const char* ntoa<vec4>( vec4 v ){ return ftoa(v); }
template<> inline const char* ntoa<dvec2>( dvec2 v ){ return dtoa(v); }
template<> inline const char* ntoa<dvec3>( dvec3 v ){ return dtoa(v); }
template<> inline const char* ntoa<dvec4>( dvec4 v ){ return dtoa(v); }
#endif

// bitwise conversion
template <class T> const char* unpack_bits( const T& v )
{
	char* buff=__strbuf(sizeof(T)*8); buff[sizeof(T)*8]=0;
	for(int k=0,B=int(sizeof(T))-1;B>=0;B--){ for(int b=7;b>=0;b--) buff[k++]=(((const char*)&v)[B]&(1<<b))?'1':'0'; }
	return buff;
}

// conversion int to string with commas
__noinline const char* itoasep( int n )
{
	if(n<1000&&n>-1000) return itoa(n);
	const char* s=itoa(n>0?n:-n); size_t len=strlen(s);
	vector<char> v; v.resize(len+1); memcpy(&v[0],s,len+1);
	for( uint idx=((len%3)?(len%3):3); idx<len; idx+=4,len++ ) v.emplace(v.begin()+idx,',');
	return format("%s%s",n>0?"":"-",&v[0]);
}

__noinline const char* illtoasep( int64_t n )
{
	if(n<1000&&n>-1000) return illtoa(n);
	const char* s=illtoa(n>0?n:-n); size_t len=strlen(s);
	vector<char> v; v.resize(len+1); memcpy(&v[0],s,len+1);
	for( uint idx=((len%3)?(len%3):3); idx<len; idx+=4,len++ ) v.emplace(v.begin()+idx,',');
	return format("%s%s",n>0?"":"-",&v[0]);
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
}

// conversion from string to user types
inline bool atob( const char* a ){		return a&&*a&&(stricmp(a,"true")==0||fast::atoi(a)!=0); }
inline uint atou( const char* a ){		char* e=nullptr;uint v=(uint)strtoul(a,&e,10); return v; }
inline uint atou( const wchar_t* w ){	wchar_t* e=nullptr;uint v=(uint)wcstoul(w,&e,10); return v; }
inline int64_t atoill( const char* a ){	char* e=nullptr;int64_t v=strtoll(a,&e,10); return v; }
inline uint64_t atoull( const char* a ){char* e=nullptr;uint64_t v=strtoull(a,&e,10); return v; }
inline int2 atoi2( const char* a ){		int2 v={};char* e=nullptr;for(int k=0;k<2;k++,a=e) (&v.x)[k]=(int)strtol(a,&e,10); return v; }
inline int3 atoi3( const char* a ){		int3 v={};char* e=nullptr;for(int k=0;k<3;k++,a=e) (&v.x)[k]=(int)strtol(a,&e,10); return v; }
inline int4 atoi4( const char* a ){		int4 v={};char* e=nullptr;for(int k=0;k<4;k++,a=e) (&v.x)[k]=(int)strtol(a,&e,10); return v; }
inline uint2 atou2( const char* a ){	uint2 v={};char* e=nullptr;for(int k=0;k<2;k++,a=e) (&v.x)[k]=strtoul(a,&e,10); return v; }
inline uint3 atou3( const char* a ){	uint3 v={};char* e=nullptr;for(int k=0;k<3;k++,a=e) (&v.x)[k]=strtoul(a,&e,10); return v; }
inline uint4 atou4( const char* a ){	uint4 v={};char* e=nullptr;for(int k=0;k<4;k++,a=e) (&v.x)[k]=strtoul(a,&e,10); return v; }
inline float2 atof2( const char* a ){	float2 v={};char* e=nullptr;for(int k=0;k<2;k++,a=e) (&v.x)[k]=strtof(a,&e); return v; }
inline float3 atof3( const char* a ){	float3 v={};char* e=nullptr;for(int k=0;k<3;k++,a=e) (&v.x)[k]=strtof(a,&e); return v; }
inline float4 atof4( const char* a ){	float4 v={};char* e=nullptr;for(int k=0;k<4;k++,a=e) (&v.x)[k]=strtof(a,&e); return v; }
inline double2 atod2( const char* a ){	double2 v={};char* e=nullptr;for(int k=0;k<2;k++,a=e) (&v.x)[k]=strtod(a,&e); return v; }
inline double3 atod3( const char* a ){	double3 v={};char* e=nullptr;for(int k=0;k<3;k++,a=e) (&v.x)[k]=strtod(a,&e); return v; }
inline double4 atod4( const char* a ){	double4 v={};char* e=nullptr;for(int k=0;k<4;k++,a=e) (&v.x)[k]=strtod(a,&e); return v; }

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

template <class T> __noinline const T* join( vector<std::basic_string<T>> v, const T* delims=__strdup<T,char>(" ") )
{
	std::basic_string<T> s; if(v.empty()) return (const T*)L"";
	for( const auto& k : v ){ if(k.empty()) continue; if(!s.empty()) s+=decltype(s)(delims); s+=k; }
	return __strdup(s.c_str());
}

template <class T> __noinline vector<std::basic_string<T>>
explode( const T* src, const T* delims=__whitespaces<T>() )
{
	vector<std::basic_string<T>> v; if(!src||!*src) return v; v.reserve(32);
	for( T *ctx=nullptr, *token = strtok_s(__strdup(src),delims,&ctx); token; token=strtok_s(nullptr,delims,&ctx) ) v.emplace_back(token);
	return v;
}

template <class T> __noinline std::set<std::basic_string<T>>
explode_set( const T* src, const T* delims=__whitespaces<T>() )
{
	std::set<std::basic_string<T>> v; if(!src||!*src) return v;
	for( T *ctx=nullptr, *token = strtok_s(__strdup(src),delims,&ctx); token; token=strtok_s(nullptr,delims,&ctx) ) v.emplace(token);
	return v;
}

template <class T> __noinline vector<int>			explodei( const T* src, const T* delims=__whitespaces<T>() ){ vector<int> v;			if(!src||!*src) return v; auto s=explode(src,delims); if(!s.empty()) v.reserve(s.size()); for( auto& t : s ) v.emplace_back(fast::atoi(t.c_str())); return v; }
template <class T> __noinline vector<unsigned int>	explodeu( const T* src, const T* delims=__whitespaces<T>() ){ vector<unsigned int> v;	if(!src||!*src) return v; auto s=explode(src,delims); if(!s.empty()) v.reserve(s.size()); for( auto& t : s ) v.emplace_back((unsigned int)fast::atoi(t.c_str())); return v; }
template <class T> __noinline vector<float>			explodef( const T* src, const T* delims=__whitespaces<T>() ){ vector<float> v;			if(!src||!*src) return v; auto s=explode(src,delims); if(!s.empty()) v.reserve(s.size()); for( auto& t : s ) v.emplace_back(float(fast::atof(t.c_str()))); return v; }
template <class T> __noinline vector<double>		exploded( const T* src, const T* delims=__whitespaces<T>() ){ vector<double> v;			if(!src||!*src) return v; auto s=explode(src,delims); if(!s.empty()) v.reserve(s.size()); for( auto& t : s ) v.emplace_back(fast::atof(t.c_str())); return v; }

template <class T> __noinline vector<std::basic_string<T>>
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

// common unnecesary unicode symbols to common ansi
__noinline const wchar_t* unicode_symbols_to_ansi( const wchar_t* str )
{
	wchar_t* b=__strdup(str);
	static std::map<int,wchar_t> lut =
	{
		{0x2013,L'-'}, {0x2014,L'-'}, {0x2015,L'-'}, {0x2212,L'-'},
		{0x2018,L'\''},{0x2019,L'\''},{0x2032,L'\''},
		{0x201C,L'\"'},{0x201D,L'\"'},{0x2033,L'\"'},
		{0x2022,L'-'},{0x00B7,L'-'},{0x22C5,L'-'},
	};

	for( size_t k=0, kn=wcslen(b); k<kn; k++ )
	{ auto it=lut.find(b[k]); if(it!=lut.end()) b[k]=it->second; }

	return b;
}

template <class T> __noinline const T* auto_quote( const T* _Src )
{
	size_t l=strlen(_Src); static const T q=T('\"'); if(!*_Src||(_Src[0]==q&&_Src[l-1]==q)) return __strdup(_Src,l);
	const T* s=trim(_Src); if(*s!=T('-')&&!strpbrk(s,__strdup<T,char>(" '\t|&<>"))) return __strdup(_Src,l);
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
