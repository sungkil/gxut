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
#ifndef __GX_MATH_H__
#define __GX_MATH_H__

// include gxut.h without other headers
#ifndef __GXUT_H__
#pragma push_macro("__GXUT_EXCLUDE_HEADERS__")
#define __GXUT_EXCLUDE_HEADERS__
#include "gxut.h"
#pragma pop_macro("__GXUT_EXCLUDE_HEADERS__")
#endif

#include <limits.h>
#include <float.h>
#include <numeric>	// for std::iota
#include <random>	// for std::shuffle

#ifdef PI
#undef PI
#endif

template <class T,class N,class X> T clamp( T v, N vmin, X vmax ){ return v<T(vmin)?T(vmin):v>T(vmax)?T(vmax):v; }
constexpr float PI = 3.141592653589793f;
template <class T=float> constexpr T T_PI = T(3.141592653589793);

//*************************************
// template type_traits helpers
template <class T> using floating_point_t = typename std::enable_if_t<std::is_floating_point<T>::value,T>;
template <class T> using signed_t = typename std::enable_if_t<std::is_signed<T>::value,T>;
#define float_memfun(U)	 template <typename U=floating_point_t<T>>

//*************************************
// legacy: template <class T,template <class> class A=tarray2> struct tvec2 (template argument for template)
template <class T> struct tvec2
{
	using A = tarray2<T>;
	__default_array_impl(2,T,tvec2);
	union{struct{T x,y;};struct{T r,g;};A xy,rg;};

	// basic constructors
	__forceinline tvec2( A v ){x=v.x;y=v.y;}
	__forceinline tvec2( T a ){x=y=a;}
	__forceinline tvec2( T a, T b ){x=a;y=b;}

	// extended constructors with explicit casting
	template <class X> __forceinline tvec2( tvec2<X> v ){x=T(v.x);y=T(v.y);}
	template <class X,class Y> __forceinline tvec2( X a, Y b ){x=T(a);y=T(b);}

	// assignment operators
	__forceinline tvec2& operator=( A&& v ){ memmove(this,&v,sizeof(v)); return *this; }
	__forceinline tvec2& operator=( const A& v ){ memcpy(this,&v,sizeof(v)); return *this; }

	// casting operators
	__forceinline operator A&(){ return reinterpret_cast<A&>(*this); }
	__forceinline operator const A&() const { return reinterpret_cast<const A&>(*this); }

	// comparison operators
	__forceinline bool operator!=(const tvec2& v) const { return !operator==(v); }
	__forceinline bool operator==(const tvec2& v) const { return x==v.x&&y==v.y; }

	// unary operators
	__forceinline tvec2& operator+(){ return *this; }
	__forceinline const tvec2& operator+() const { return *this; }
	__forceinline tvec2 operator-() const { return tvec2(-x,-y); }

	// binary operators
	__forceinline tvec2 operator+( T a ) const { return tvec2(x+a, y+a); }
    __forceinline tvec2 operator-( T a ) const { return tvec2(x-a, y-a); }
    __forceinline tvec2 operator*( T a ) const { return tvec2(x*a, y*a); }
    __forceinline tvec2 operator/( T a ) const { return tvec2(x/a, y/a); }
	__forceinline tvec2 operator+(const tvec2& v) const { return tvec2(x+v.x, y+v.y); }
    __forceinline tvec2 operator-(const tvec2& v) const { return tvec2(x-v.x, y-v.y); }
    __forceinline tvec2 operator*(const tvec2& v) const { return tvec2(x*v.x, y*v.y); }
    __forceinline tvec2 operator/(const tvec2& v) const { return tvec2(x/v.x, y/v.y); }

	// compound assignment operators
	__forceinline tvec2& operator+=( T a ) { x+=a; y+=a; return *this; }
	__forceinline tvec2& operator-=( T a ) { x-=a; y-=a; return *this; }
	__forceinline tvec2& operator*=( T a ) { x*=a; y*=a; return *this; }
	__forceinline tvec2& operator/=( T a ) { x/=a; y/=a; return *this; }
	__forceinline tvec2& operator+=(const tvec2& v) { x+=v.x; y+=v.y; return *this; }
	__forceinline tvec2& operator-=(const tvec2& v) { x-=v.x; y-=v.y; return *this; }
	__forceinline tvec2& operator*=(const tvec2& v) { x*=v.x; y*=v.y; return *this; }
	__forceinline tvec2& operator/=(const tvec2& v) { x/=v.x; y/=v.y; return *this; }

	// norm/length/dot: floating-point only functions
	float_memfun(T) __forceinline T length2() const { return T(x*x+y*y); }
	float_memfun(T) __forceinline T norm2() const { return T(x*x+y*y); }
	float_memfun(T) __forceinline T length() const { return T(sqrt(x*x+y*y)); }
	float_memfun(T) __forceinline T norm() const { return T(sqrt(x*x+y*y)); }
	float_memfun(T) __forceinline T dot( const tvec2& v ) const { return x*v.x+y*v.y; }
	float_memfun(T) __forceinline tvec2<T> normalize() const { return operator/(length()); }
};

template <class T> struct tvec3
{
	using A = tarray3<T>;
	__default_array_impl(3,T,tvec3);
	
	using V2 = tvec2<T>;

	union{struct{T x,y,z;};struct{T r,g,b;};union{V2 xy,rg;};struct{T _x;union{V2 yz,gb;};};A xyz,rgb;};

	// constructors
	__forceinline tvec3( A v ){x=v.x;y=v.y;z=v.z;}
	__forceinline tvec3( T a ){x=y=z=a;}
	__forceinline tvec3( T a, T b, T c ){x=a;y=b;z=c;}
	__forceinline tvec3( V2 v, T c ){x=v.x;y=v.y;z=c;}
	__forceinline tvec3( T a, V2 v ){x=a;y=v.x;z=v.y;}

	// extended constructors with explicit casting
	template <class X> __forceinline tvec3( tvec3<X> v ){x=T(v.x);y=T(v.y);z=T(v.z);}
	template <class X,class Y,class Z> __forceinline tvec3( X a, Y b, Z c ){x=T(a);y=T(b);z=T(c);}
	template <class X,class Z> __forceinline tvec3( tvec2<X> v, Z c ){x=T(v.x);y=T(v.y);z=T(c);}
	template <class X,class Y> __forceinline tvec3( X a, tvec2<Y> v ){x=a;y=T(v.x);z=T(v.y);}

	// assignment operators
	__forceinline tvec3& operator=( A&& v ){ memmove(this,&v,sizeof(v)); return *this; }
	__forceinline tvec3& operator=( const A& v ){ memcpy(this,&v,sizeof(v)); return *this; }

	// casting operators
	__forceinline operator A&(){ return reinterpret_cast<A&>(*this); }
	__forceinline operator const A&() const { return reinterpret_cast<const A&>(*this); }

	// comparison operators
	__forceinline bool operator==(const tvec3& v) const { return x==v.x&&y==v.y&&z==v.z; }
	__forceinline bool operator!=(const tvec3& v) const { return !operator==(v); }

	// unary operators
	__forceinline tvec3& operator+(){ return *this; }
	__forceinline const tvec3& operator+() const { return *this; }
	__forceinline tvec3 operator-() const { return tvec3(-x,-y,-z); }

	// binary operators
	__forceinline tvec3 operator+( T a ) const { return tvec3(x+a, y+a, z+a); }
    __forceinline tvec3 operator-( T a ) const { return tvec3(x-a, y-a, z-a); }
	__forceinline tvec3 operator*( T a ) const { return tvec3(x*a, y*a, z*a); }
    __forceinline tvec3 operator/( T a ) const { return tvec3(x/a, y/a, z/a); }
    __forceinline tvec3 operator+( const tvec3& v ) const { return tvec3(x+v.x, y+v.y, z+v.z); }
    __forceinline tvec3 operator-( const tvec3& v ) const { return tvec3(x-v.x, y-v.y, z-v.z); }
    __forceinline tvec3 operator*( const tvec3& v ) const { return tvec3(x*v.x, y*v.y, z*v.z); }
    __forceinline tvec3 operator/( const tvec3& v ) const { return tvec3(x/v.x, y/v.y, z/v.z); }

	// compound assignment operators
	__forceinline tvec3& operator+=( T a ){ x+=a; y+=a; z+=a; return *this; }
	__forceinline tvec3& operator-=( T a ){ x-=a; y-=a; z-=a; return *this; }
	__forceinline tvec3& operator*=( T a ){ x*=a; y*=a; z*=a; return *this; }
	__forceinline tvec3& operator/=( T a ){ x/=a; y/=a; z/=a; return *this; }
	__forceinline tvec3& operator+=( const tvec3& v ){ x+=v.x; y+=v.y; z+=v.z; return *this; }
	__forceinline tvec3& operator-=( const tvec3& v ){ x-=v.x; y-=v.y; z-=v.z; return *this; }
	__forceinline tvec3& operator*=( const tvec3& v ){ x*=v.x; y*=v.y; z*=v.z; return *this; }
	__forceinline tvec3& operator/=( const tvec3& v ){ x/=v.x; y/=v.y; z/=v.z; return *this; }

	// norm/length/dot: floating-point only functions
	float_memfun(T) __forceinline T length2() const { return T(x*x+y*y+z*z); }
	float_memfun(T) __forceinline T norm2() const { return T(x*x+y*y+z*z); }
	float_memfun(T) __forceinline T length() const { return T(sqrt(x*x+y*y+z*z)); }
	float_memfun(T) __forceinline T norm() const { return T(sqrt(x*x+y*y+z*z)); }
	float_memfun(T) __forceinline T dot( const tvec3& v ) const { return x*v.x+y*v.y+z*v.z; }
	float_memfun(T) __forceinline tvec3<T> normalize() const { return operator/(length()); }

	// tvec3 only: cross product (floating-point only)
	float_memfun(T) __forceinline tvec3<T> cross( const tvec3& v ) const { return tvec3( y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x ); }
};

template <class T> struct tvec4
{
	using A = tarray4<T>;
	__default_array_impl(4,T,tvec4);

	using V2 = tvec2<T>;
	using V3 = tvec3<T>;

	union{struct{T x,y,z,w;};struct{T r,g,b,a;};struct{union{V2 xy,rg;};union{V2 zw,ba;};};union{V3 xyz,rgb;};struct{T _x;union{V3 yzw,gba;V2 yz,gb;};};A xyzw,rgba;};

	// basic constructors
	__forceinline tvec4( A v ){x=v.x;y=v.y;z=v.z;w=v.w;}
	__forceinline tvec4( T a ){x=y=z=w=a;}
	__forceinline tvec4( T a, T b, T c, T d ){x=a;y=b;z=c;w=d;}
	__forceinline tvec4( T a, V2 v, T d ){x=a;y=v.x;z=v.y;w=d;}
	__forceinline tvec4( T a, V3 v ){x=a;y=v.x;z=v.y;w=v.z;}
	__forceinline tvec4( T a, T b, V2 v ){x=a;y=b;z=v.x;w=v.y;}
	__forceinline tvec4( V2 v, T c, T d ){x=v.x;y=v.y;z=c;w=d;}
	__forceinline tvec4( V2 v, V2 u ){x=v.x;y=v.y;z=u.x;w=u.y;}
	__forceinline tvec4( V3 v, T d ){x=v.x;y=v.y;z=v.z;w=d;}

	// extended constructors with explicit casting
	template <class X> __forceinline tvec4( tvec4<X> v ){x=T(v.x);y=T(v.y);z=T(v.z);w=T(v.w);}
	template <class X,class Y,class Z,class W> __forceinline tvec4( X a, Y b, Z c, W d ){x=T(a);y=T(b);z=T(c);w=T(d);}
	template <class X,class Y,class W> __forceinline tvec4( X a, tvec2<Y> v, W d ){x=T(a);y=T(v.x);z=T(v.y);w=T(d);}
	template <class X,class Y> __forceinline tvec4( X a, tvec3<Y> v ){x=T(a);y=T(v.x);z=T(v.y);w=T(v.z);}
	template <class X,class Y,class Z> __forceinline tvec4( X a, Y b, tvec2<Z> v ){x=T(a);y=T(b);z=T(v.x);w=T(v.y);}
	template <class X,class Z,class W> __forceinline tvec4( tvec2<X> v, Z c, W d ){x=T(v.x);y=T(v.y);z=T(c);w=T(d);}
	template <class X,class Z> __forceinline tvec4( tvec2<X> v, tvec2<Z> u ){x=T(v.x);y=T(v.y);z=T(u.x);w=T(u.y);}
	template <class X,class W> __forceinline tvec4( tvec3<X> v, W d ){x=T(v.x);y=T(v.y);z=T(v.z);w=T(d);}

	// assignment operators
	__forceinline tvec4& operator=( A&& v ){ memmove(this,&v,sizeof(v)); return *this; }
	__forceinline tvec4& operator=( const A& v ){ memcpy(this,&v,sizeof(v)); return *this; }

	// casting operators
	__forceinline operator A&(){ return reinterpret_cast<A&>(*this); }
	__forceinline operator const A&() const { return reinterpret_cast<const A&>(*this); }

	// comparison operators
	__forceinline bool operator==(const tvec4& v) const { return x==v.x&&y==v.y&&z==v.z&&w==v.w; }
	__forceinline bool operator!=(const tvec4& v) const { return !operator==(v); }

	// unary operators
	__forceinline tvec4& operator+(){ return *this; }
	__forceinline const tvec4& operator+() const { return *this; }
	__forceinline tvec4 operator-() const { return tvec4(-x,-y,-z,-w); }

    // binary operators
    __forceinline tvec4 operator+( T a ) const { return tvec4(x+a,y+a,z+a,w+a); }
    __forceinline tvec4 operator-( T a ) const { return tvec4(x-a,y-a,z-a,w-a); }
	__forceinline tvec4 operator*( T a ) const { return tvec4(x*a,y*a,z*a,w*a); }
    __forceinline tvec4 operator/( T a ) const { return tvec4(x/a,y/a,z/a,w/a); }
    __forceinline tvec4 operator+( const tvec4& v) const { return tvec4(x+v.x,y+v.y,z+v.z,w+v.w); }
    __forceinline tvec4 operator-( const tvec4& v) const { return tvec4(x-v.x,y-v.y,z-v.z,w-v.w); }
    __forceinline tvec4 operator*( const tvec4& v) const { return tvec4(x*v.x,y*v.y,z*v.z,w*v.w); }
    __forceinline tvec4 operator/( const tvec4& v) const { return tvec4(x/v.x,y/v.y,z/v.z,w/v.w); }

    // compound assignment operators
    __forceinline tvec4& operator+=( T a ){ x+=a; y+=a; z+=a; w+=a; return *this; }
    __forceinline tvec4& operator-=( T a ){ x-=a; y-=a; z-=a; w-=a; return *this; }
	__forceinline tvec4& operator*=( T a ){ x*=a; y*=a; z*=a; w*=a; return *this; }
    __forceinline tvec4& operator/=( T a ){ x/=a; y/=a; z/=a; w/=a; return *this; }
    __forceinline tvec4& operator+=( const tvec4& v){ x+=v.x; y+=v.y; z+=v.z; w+=v.w; return *this; }
    __forceinline tvec4& operator-=( const tvec4& v){ x-=v.x; y-=v.y; z-=v.z; w-=v.w; return *this; }
    __forceinline tvec4& operator*=( const tvec4& v){ x*=v.x; y*=v.y; z*=v.z; w*=v.w; return *this; }
    __forceinline tvec4& operator/=( const tvec4& v){ x/=v.x; y/=v.y; z/=v.z; w/=v.w; return *this; }

	// norm/length/dot: floating-point only functions
	float_memfun(T) __forceinline T length2() const { return T(x*x+y*y+z*z+w*w); }
	float_memfun(T) __forceinline T norm2() const { return T(x*x+y*y+z*z+w*w); }
	float_memfun(T) __forceinline T length() const { return T(sqrt(x*x+y*y+z*z+w*w)); }
	float_memfun(T) __forceinline T norm() const { return T(sqrt(x*x+y*y+z*z+w*w)); }
	float_memfun(T) __forceinline T dot( const tvec4& v ) const { return x*v.x+y*v.y+z*v.z+w*v.w; }
	float_memfun(T) __forceinline tvec4<T> normalize() const { return operator/(length()); }
};

// member function specialization
template <class T> struct precision { static const T value(){ return std::numeric_limits<T>::epsilon()*20; } };	// need to be 20x for robust practical test
template<> __forceinline bool tvec2<float>::operator==(const tvec2& v) const { static const float p=precision<float>::value(); return std::abs(x-v.x)<=p&&std::abs(y-v.y)<=p; }
template<> __forceinline bool tvec3<float>::operator==(const tvec3& v) const { static const float p=precision<float>::value(); return std::abs(x-v.x)<=p&&std::abs(y-v.y)<=p&&std::abs(z-v.z)<=p; }
template<> __forceinline bool tvec4<float>::operator==(const tvec4& v) const { static const float p=precision<float>::value(); return std::abs(x-v.x)<=p&&std::abs(y-v.y)<=p&&std::abs(z-v.z)<=p&&std::abs(w-v.w)<=p; }
template<> __forceinline bool tvec2<double>::operator==(const tvec2& v) const { static const double p=precision<double>::value(); return std::abs(x-v.x)<=p&&std::abs(y-v.y)<=p; }
template<> __forceinline bool tvec3<double>::operator==(const tvec3& v) const { static const double p=precision<double>::value(); return std::abs(x-v.x)<=p&&std::abs(y-v.y)<=p&&std::abs(z-v.z)<=p; }
template<> __forceinline bool tvec4<double>::operator==(const tvec4& v) const { static const double p=precision<double>::value(); return std::abs(x-v.x)<=p&&std::abs(y-v.y)<=p&&std::abs(z-v.z)<=p&&std::abs(w-v.w)<=p; }

//*************************************
// type definitions and size check
using bvec2 = tvec2<bool>;			using bvec3 = tvec3<bool>;			using bvec4 = tvec4<bool>;
using vec2	= tvec2<float>;			using vec3	= tvec3<float>;			using vec4	= tvec4<float>;
using dvec2 = tvec2<double>;		using dvec3 = tvec3<double>;		using dvec4 = tvec4<double>;
using ivec2 = tvec2<int>;			using ivec3 = tvec3<int>;			using ivec4 = tvec4<int>;
using uvec2 = tvec2<uint>;			using uvec3 = tvec3<uint>;			using uvec4 = tvec4<uint>;
using llvec2 = tvec2<int64_t>;		using llvec3 = tvec3<int64_t>;		using llvec4 = tvec4<int64_t>;
using ullvec2 = tvec2<uint64_t>;	using ullvec3 = tvec3<uint64_t>;	using ullvec4 = tvec4<uint64_t>;

static_assert(sizeof(vec2)==(sizeof(float)*2),"sizeof(vec2)!=sizeof(float)*2" );
static_assert(sizeof(vec3)==(sizeof(float)*3),"sizeof(vec3)!=sizeof(float)*3" );
static_assert(sizeof(vec4)==(sizeof(float)*4),"sizeof(vec4)!=sizeof(float)*4" );

// fundamental types for computer graphics
struct vertex { vec3 pos; vec3 norm; vec2 tex; }; // default vertex layout
struct bbox_t { vec3 m=3.402823466e+38F; uint __0; vec3 M=-3.402823466e+38F; uint __1; }; // bounding box in std140 layout; FLT_MAX = 3.402823466e+38F; __0, __1: padding

//*************************************
// std::hash support here

#if defined(_M_X64)||defined(__LP64__)
template <class T> struct bitwise_hash {size_t operator()(const T v)const{ size_t h=14695981039346656037ULL;const uchar* p=(const uchar*)&v;for(size_t k=0;k<sizeof(T);k++){h^=size_t(p[k]);h*=1099511628211ULL;}return h;}}; // FNV-1a hash function (from VC2015/2017)
#elif defined(_M_IX86)
template <class T> struct bitwise_hash {size_t operator()(const T v)const{ size_t h=2166136261U;const uchar* p=(const uchar*)&v;for(size_t k=0;k<sizeof(T);k++){h^=size_t(p[k]);h*=16777619U;}return h;}}; // FNV-1a hash function (from VC2015/2017)
#endif

namespace std
{
	template<> struct hash<int2> :	public bitwise_hash<int2> {};
	template<> struct hash<int3> :	public bitwise_hash<int3> {};
	template<> struct hash<int4> :	public bitwise_hash<int4> {};
	template<> struct hash<uint2> :	public bitwise_hash<uint2> {};
	template<> struct hash<uint3> :	public bitwise_hash<uint3> {};
	template<> struct hash<uint4> :	public bitwise_hash<uint4> {};
	template<> struct hash<ivec2> :	public bitwise_hash<int2> {};
	template<> struct hash<ivec3> :	public bitwise_hash<int3> {};
	template<> struct hash<ivec4> :	public bitwise_hash<int4> {};
	template<> struct hash<uvec2> :	public bitwise_hash<uint2> {};
	template<> struct hash<uvec3> :	public bitwise_hash<uint3> {};
	template<> struct hash<uvec4> :	public bitwise_hash<uint4> {};
}

//*************************************
// half-precision float and conversion
#ifndef __GXMATH_NO_HALF__
#define __GXMATH_HALF__
struct half { unsigned short mantissa:10,exponent:5,sign:1; __forceinline operator float() const; };	// IEEE 754-2008 half-precision (16-bit) floating-point storage. // https://github.com/HeliumProject/Helium/blob/master/Math/Float16.cpp
__forceinline float	htof( half value ){ struct _float32_t {uint mantissa:23,exponent:8,sign:1;}; _float32_t result={};result.sign = value.sign;uint exponent=value.exponent,mantissa=value.mantissa; if(exponent==31){result.exponent=255;result.mantissa=0;} else if(exponent==0&&mantissa==0){result.exponent=0;result.mantissa=0;} else if(exponent==0){uint mantissa_shift=10-static_cast<uint>(log2(float(mantissa)));result.exponent=127-(15-1)-mantissa_shift;result.mantissa=mantissa<<(mantissa_shift+23-10);} else{result.exponent=127-15+exponent;result.mantissa=static_cast<uint>(value.mantissa)<<(23-10);} return reinterpret_cast<float&>(result); }
__forceinline half::operator float() const {return htof(*this);}
__forceinline float*htof( const half* ph, float* pf, size_t nElements, size_t float_stride=1 ){ if(pf==nullptr||ph==nullptr||nElements==0) return pf; float* pf0=pf; for( size_t k=0; k < nElements; k++, ph++, pf+=float_stride ) *pf=htof(*ph); return pf0; }
__forceinline half ftoh( float f ){ struct _float32_t {uint mantissa:23,exponent:8,sign:1;}; _float32_t value=reinterpret_cast<_float32_t&>(f);half result={};result.sign=value.sign; uint exponent=value.exponent; if(exponent==255){result.exponent=31;result.mantissa=0;} else if(exponent<127-15+1){uint mantissa=(1<<23)|value.mantissa; uint mantissa_shift=(23-10)+(127-15+1)-exponent;result.exponent=0;result.mantissa=static_cast<ushort>(mantissa>>mantissa_shift);} else if(exponent>127+(31-15-1)){result.exponent=31-1;result.mantissa=1023;} else {result.exponent=static_cast<ushort>(127-15+exponent);result.mantissa=static_cast<ushort>(value.mantissa>>(23-10));} return result; }
using half2	= tarray2<half>;	using half3	= tarray3<half>;	using half4 = tarray4<half>;

// half vector conversion
__forceinline vec2	htof( const half2& h ){ return vec2(htof(h.x),htof(h.y)); }
__forceinline vec3	htof( const half3& h ){ return vec3(htof(h.x),htof(h.y),htof(h.z)); }
__forceinline vec4	htof( const half4& h ){ return vec4(htof(h.x),htof(h.y),htof(h.z),htof(h.w)); }
__forceinline half2	ftoh( const vec2& v ){ return half2{ftoh(v.x),ftoh(v.y)}; }
__forceinline half3	ftoh( const vec3& v ){ return half3{ftoh(v.x),ftoh(v.y),ftoh(v.z)}; }
__forceinline half4 ftoh( const vec4& v ){ return half4{ftoh(v.x),ftoh(v.y),ftoh(v.z),ftoh(v.w)}; }
__forceinline half* ftoh( const float* pf, half* ph, size_t nElements, size_t half_stride=1 ){ if(pf==nullptr||ph==nullptr||nElements==0) return ph; half* ph0=ph; for( size_t k=0; k < nElements; k++, pf++, ph+=half_stride ) *ph=ftoh(*pf); return ph0; }
#endif

//*************************************
// common matrix implemenetations
#define __default_matrix_impl(M,dim)	\
	static const int D = dim;\
	using V = tvec##dim<T>;\
	__default_types(dim*dim);\
	__default_index(V);\
	__forceinline M( M&& m )=default;\
	__forceinline M( const M& m )=default;\
	__forceinline M( T diag ){for(int k=0;k<D;k++)for(int j=0;j<D;j++){v[k][j]=(k==j)?diag:0;} }\
	__forceinline M& operator=( M&& m )=default;\
	__forceinline M& operator=( const M& m )=default;\
	__forceinline bool operator==( const M& m ) const { for(int k=0;k<N;k++) if(std::abs((&_11)[k]-(&m._11)[k])>precision<T>::value()) return false; return true; }\
	__forceinline bool operator!=( const M& m ) const { return !operator==(m); }\
	__forceinline M& operator+(){ return *this; }\
	__forceinline const M& operator+() const { return *this; }\
	__forceinline M operator-() const { M m; for(int k=0;k<N;k++) (&m._11)[k]=-(&_11)[k]; return m; }\
	__forceinline M& operator+=( const M& m ){ for(int k=0;k<N;k++) (&_11)[k]+=(&m._11)[k]; return *this; }\
	__forceinline M& operator-=( const M& m ){ for(int k=0;k<N;k++) (&_11)[k]-=(&m._11)[k]; return *this; }\
	__forceinline M& operator*=( const M& m ){ M t=m.transpose(); for(int k=0;k<D;k++) v[k]=t*v[k]; return *this; }\
	__forceinline M& operator*=( T f ){ for(int k=0;k<N;k++) (&_11)[k]*=f; return *this; }\
	__forceinline M& operator/=( T f ){ for(int k=0;k<N;k++) (&_11)[k]/=f; return *this; }\
	__forceinline M operator+( const M& m ) const { return M(*this)+=m; }\
	__forceinline M operator-( const M& m ) const { return M(*this)-=m; }\
	__forceinline M operator*( const M& m ) const { return M(*this)*=m; }\
	__forceinline V operator*( const V& w ) const { V f; for(int k=0;k<D;k++) f[k]=v[k].dot(w); return f; }\
	__forceinline M operator*( T f ) const { return M(*this)*=f; }\
	__forceinline M operator/( T f ) const { return M(*this)/=f; }\
	__forceinline static M identity(){ return M(); }\
	__forceinline M& set_identity(){ return *this=M(); }\
	__forceinline V diag() const { V f; for(int k=0;k<D;k++) f[k]=v[k][k]; return f; }\
	__forceinline V cvec( int col ) const { V f; for(int k=0;k<D;k++) f[k]=v[k][col]; return f; }\
	__forceinline T trace() const { T f=0; for(int k=0;k<D;k++) f+=v[k][k]; return f; }

//*************************************
template <class T> struct tmat2
{
	__default_matrix_impl(tmat2,2);

	union { V v[2]; struct { T _11,_12,_21,_22;}; };

	// constructors
	__forceinline tmat2(){ _12=_21=0;_11=_22=T(1.0); }
	__forceinline tmat2( T f11, T f12, T f21, T f22 ){ _11=f11;_12=f12;_21=f21;_22=f22; }

	// identity and transpose
	__forceinline tmat2 transpose() const { return tmat2{_11,_21,_12,_22}; }

	// determinant/trace/inverse
	__forceinline T det() const { return _11*_22 - _12*_21; }
	__forceinline tmat2 inverse() const { T s=T(1.0)/det(); return tmat2(_22*s,-_12*s,-_21*s,_11*s); }

	// static row-major transformations: 2D transformation in 2D Cargesian coordinate system
	__forceinline static tmat2 scale( const V& v ){ return tmat2().set_scale(v); }
	__forceinline static tmat2 scale( T x, T y ){ return tmat2().set_scale(x,y); }
	__forceinline static tmat2 rotate( T theta ){ return tmat2().set_rotate(theta); }

	// row-major transformations: 2D transformation in 2D Cargesian coordinate system
	__forceinline tmat2& set_scale( V v ){ return set_scale(v.x,v.y); }
	__forceinline tmat2& set_scale( T x, T y ){ _11=x; _22=y; _12=_21=0; return *this; }
	__forceinline tmat2& set_rotate( T theta ){ _11=_22=T(cos(theta));_21=T(sin(theta));_12=-_21; return *this; }
};

//*************************************
template <class T> struct tmat3
{
	__default_matrix_impl(tmat3,3);

	union{ V v[3]; struct{T _11,_12,_13,_21,_22,_23,_31,_32,_33;}; };

	// constructors
	__forceinline tmat3(){ _12=_13=_21=_23=_31=_32=0;_11=_22=_33=T(1.0); }
	__forceinline tmat3( T f11, T f12, T f13, T f21, T f22, T f23, T f31, T f32, T f33 ){_11=f11;_12=f12;_13=f13;_21=f21;_22=f22;_23=f23;_31=f31;_32=f32;_33=f33;}

	// casting operators
	__forceinline operator tmat2<T>() const { return tmat2<T>(_11,_12,_21,_22); }

	// identity and transpose
	__forceinline tmat3 transpose() const { return tmat3(_11,_21,_31,_12,_22,_32,_13,_23,_33); }

	// determinant/trace/inverse
	__forceinline T det() const { return _11*(_22*_33-_23*_32) + _12*(_23*_31-_21*_33) + _13*(_21*_32-_22*_31); }
	__forceinline tmat3 inverse() const { T s=T(1.0)/det(); return tmat3( (_22*_33-_32*_23)*s, (_13*_32-_12*_33)*s, (_12*_23-_13*_22)*s, (_23*_31-_21*_33)*s, (_11*_33-_13*_31)*s, (_21*_13-_11*_23)*s, (_21*_32-_31*_22)*s, (_31*_12-_11*_32)*s, (_11*_22-_21*_12)*s ); }

	// static row-major transformations: 2D transformation in 3D homogeneous coordinate system
	__forceinline static tmat3 translate( const V& v ){ return tmat3().set_translate(v); }
	__forceinline static tmat3 translate( T x, T y ){ return tmat3().set_translate(x,y); }
	__forceinline static tmat3 scale( const V& v ){ return tmat3().set_scale(v); }
	__forceinline static tmat3 scale( T x, T y ){ return tmat3().set_scale(x,y); }
	__forceinline static tmat3 rotate( T theta ){ return tmat3().set_rotate(theta); }

	// row-major transformations: 2D transformation in 3D homogeneous coordinate system
	__forceinline tmat3& set_translate( const V& v ){ set_identity(); _13=v.x; _23=v.y; return *this; }
	__forceinline tmat3& set_translate( T x,T y ){ set_identity(); _13=x; _23=y; return *this; }
	__forceinline tmat3& set_scale( const V& v ){ set_identity(); _11=v.x; _22=v.y; return *this; }
	__forceinline tmat3& set_scale( T x, T y ){ set_identity(); _11=x; _22=y; return *this; }
	__forceinline tmat3& set_rotate( T theta ){ set_identity(); _11=_22=T(cos(theta));_21=T(sin(theta));_12=-_21; return *this; }
};

//*************************************
// mat4 uses only row-major and right-hand (RH) notations even for D3D
template <class T> struct tmat4
{
	__default_matrix_impl(tmat4,4);
	using V2 = tvec2<T>;
	using V3 = tvec3<T>;

	union{ V v[4]; struct{T _11,_12,_13,_14,_21,_22,_23,_24,_31,_32,_33,_34,_41,_42,_43,_44;}; };

	// constructors
	__forceinline tmat4(){ _12=_13=_14=_21=_23=_24=_31=_32=_34=_41=_42=_43=0;_11=_22=_33=_44=T(1.0); }
	__forceinline tmat4( T f11, T f12, T f13, T f14, T f21, T f22, T f23, T f24, T f31, T f32, T f33, T f34, T f41, T f42, T f43, T f44 ){_11=f11;_12=f12;_13=f13;_14=f14;_21=f21;_22=f22;_23=f23;_24=f24;_31=f31;_32=f32;_33=f33;_34=f34;_41=f41;_42=f42;_43=f43;_44=f44;}

	// casting operators
	__forceinline operator tmat3<T>() const { return tmat3<T>(_11,_12,_13,_21,_22,_23,_31,_32,_33); }
	__forceinline operator tmat2<T>() const { return tmat2<T>(_11,_12,_21,_22); }

	// multiplication operators
	__forceinline tvec3<T> operator*( const tvec3<T>& f ) const { tvec4<T> g(f,T(1)); return tvec3<T>(v[0].dot(g),v[1].dot(g),v[2].dot(g)); }

	// identity and transpose
	__forceinline tmat4 transpose() const { return tmat4(_11,_21,_31,_41,_12,_22,_32,_42,_13,_23,_33,_43,_14,_24,_34,_44); }

	// determinant/trace/inverse
	V _xdet() const;	// support function for det() and inverse()
	__forceinline T det() const { return cvec(3).dot(_xdet()); }
	tmat4 inverse() const;

	// static row-major transformations
	__forceinline static tmat4 translate( const V3& v ){ return tmat4().set_translate(v); }
	__forceinline static tmat4 translate( T x, T y, T z ){ return tmat4().set_translate(x,y,z); }
	__forceinline static tmat4 scale( const V3& v ){ return tmat4().set_scale(v); }
	__forceinline static tmat4 scale( T x, T y, T z ){ return tmat4().set_scale(x,y,z); }
	__forceinline static tmat4 shear( const V2& yz, const V2& zx, const V2& xy ){ return tmat4().set_shear(yz,zx,xy); }
	__forceinline static tmat4 rotate( const V3& from, const V3& to ){ return tmat4().set_rotate(from,to); }
	__forceinline static tmat4 rotate( const V3& axis, T angle ){ return tmat4().set_rotate(axis,angle); }

	__forceinline static tmat4 viewport( int width, int height ){ return tmat4().set_viewport(width,height); }
	__forceinline static tmat4 look_at( const V3& eye, const V3& center, const V3& up ){ return tmat4().set_look_at(eye,center,up); }
	__forceinline static tmat4 look_at_inverse( const V3& eye, const V3& center, const V3& up ){ return tmat4().set_look_at_inverse(eye,center,up); }

	__forceinline static tmat4 perspective( T fovy, T aspect, T dn, T df ){ return tmat4().set_perspective(fovy,aspect,dn,df); }
	__forceinline static tmat4 perspective_off_center( T left, T right, T top, T bottom, T dn, T df ){ return tmat4().set_perspective_off_center(left,right,top,bottom,dn,df); }
	__forceinline static tmat4 ortho( T width, T height, T dn, T df ){ return tmat4().set_ortho(width,height,dn,df); }
	__forceinline static tmat4 ortho_off_center( T left, T right, T top, T bottom, T dn, T df ){ return tmat4().set_ortho_off_center(left,right,top,bottom,dn,df); }
	__forceinline static tmat4 perspective_dx( T fovy, T aspect, T dn, T df ){ return tmat4().set_perspective_dx(fovy,aspect,dn,df); }
	__forceinline static tmat4 perspective_off_center_dx( T left, T right, T top, T bottom, T dn, T df ){ return tmat4().set_perspective_off_center_dx(left,right,top,bottom,dn,df); }
	__forceinline static tmat4 ortho_dx( T width, T height, T dn, T df ){ return tmat4().set_ortho_dx(width,height,dn,df); }
	__forceinline static tmat4 ortho_off_center_dx( T left, T right, T top, T bottom, T dn, T df ){ return tmat4().set_ortho_off_center_dx(left,right,top,bottom,dn,df); }

	// row-major transformations
	__forceinline tmat4& set_translate( const V3& v ){ set_identity(); _14=v.x; _24=v.y; _34=v.z; return *this; }
	__forceinline tmat4& set_translate( T x,T y,T z ){ set_identity(); _14=x; _24=y; _34=z; return *this; }
	__forceinline tmat4& set_scale( const V3& v ){ set_identity(); _11=v.x; _22=v.y; _33=v.z; return *this; }
	__forceinline tmat4& set_scale( T x,T y,T z ){ set_identity(); _11=x; _22=y; _33=z; return *this; }
	__forceinline tmat4& set_shear( const V2& yz, const V2& zx, const V2& xy ){ set_identity(); _12=yz.x; _13=yz.y; _21=zx.y; _23=zx.x; _31=xy.x; _32=xy.y; return *this; }
	tmat4& set_rotate( V3 from, V3 to );
	tmat4& set_rotate( const V3& axis, T angle );

	// viewport, lookat, projection
	__forceinline tmat4& set_viewport( int width, int height ){ set_identity(); _11=width*T(0.5); _22=-height*T(0.5); _14=width*T(0.5); _24=height*T(0.5); return *this; }
	__forceinline tmat4& set_look_at( const V3& eye, const V3& center, const V3& up ){ V3 n=(eye-center).normalize(), u=(up.cross(n)).normalize(), v=n.cross(u); return *this = tmat4{u.x,u.y,u.z,-u.dot(eye),v.x,v.y,v.z,-v.dot(eye),n.x,n.y,n.z,-n.dot(eye),0,0,0,T(1.0)}; }
	__forceinline tmat4& set_look_at_inverse( const V3& eye, const V3& center, const V3& up ){ V3 n=(eye-center).normalize(), u=(up.cross(n)).normalize(), v=n.cross(u); return *this = tmat4{u.x,v.x,n.x,eye.x,u.y,v.y,n.y,eye.y,u.z,v.z,n.z,eye.z,0,0,0,T(1.0)}; }
	__forceinline V3	 look_at_eye() const { const V3 &u=v[0].xyz,&V=v[1].xyz,&n=v[2].xyz,uv=u.cross(V),vn=V.cross(n),nu=n.cross(u); return (vn*_14+nu*_24+uv*_34)/(-u.dot(vn)); }
	__forceinline tmat4  look_at_inverse() const { V3 eye=look_at_eye(); return tmat4{_11,_21,_31,eye.x,_12,_22,_32,eye.y,_13,_23,_33,eye.z,0,0,0,T(1)}; }

	// Canonical view volume in OpenGL: [-1,1]^3
	__forceinline tmat4& set_perspective( T fovy, T aspect, T dn, T df ){ if(fovy>T_PI<T>) fovy*=T_PI<T>/T(180.0); /* autofix for fov in degrees */ set_identity(); _22=T(1.0/tanf(fovy*0.5)); _11=_22/aspect; _33=(dn+df)/(dn-df); _34=2.0f*dn*df/(dn-df); _43=T(-1.0); _44=0; return *this; }
	__forceinline tmat4& set_perspective_off_center( T left, T right, T top, T bottom, T dn, T df ){ set_identity(); _11=T(2.0)*dn/(right-left); _22=T(2.0)*dn/(top-bottom); _13=(right+left)/(right-left); _23=(top+bottom)/(top-bottom); _33=(dn+df)/(dn-df); _34=T(2.0)*dn*df/(dn-df); _43=T(-1.0); _44=0; return *this; }
	__forceinline tmat4& set_ortho( T width, T height, T dn, T df ){ set_identity(); _11=T(2.0)/width; _22=T(2.0)/height;  _33=2.0f/(dn-df); _34=(dn+df)/(dn-df); return *this; }
	__forceinline tmat4& set_ortho_off_center( T left, T right, T top, T bottom, T dn, T df ){ set_ortho( right-left, top-bottom, dn, df ); _14=(left+right)/(left-right); _24=(bottom+top)/(bottom-top); return *this; }

	// Canonical view volume in DirectX: [-1,1]^2*[0,1]: diffes only in _33 and _34
	__forceinline tmat4& set_perspective_dx( T fovy, T aspect, T dn, T df ){ set_perspective( fovy, aspect, dn, df ); _33=df/(dn-df); _34*=T(0.5); return *this; } // equivalent to D3DXMatrixPerspectiveFovRH()
	__forceinline tmat4& set_perspective_off_center_dx( T left, T right, T top, T bottom, T dn, T df ){ set_perspective_off_center( left, right, top, bottom, dn, df ); _33=df/(dn-df); _34*=T(0.5); return *this; }
	__forceinline tmat4& set_ortho_dx( T width, T height, T dn, T df ){ set_ortho( width, height, dn, df ); _33*=T(0.5); _34=dn/(dn-df); return *this; }
	__forceinline tmat4& set_ortho_off_center_dx( T left, T right, T top, T bottom, T dn, T df ){ set_ortho_off_center( left, right, top, bottom, dn, df ); _33*=T(0.5); _34=dn/(dn-df); return *this; }
};

template <class T>
__noinline tmat4<T>& tmat4<T>::set_rotate( V3 from, V3 to )
{
	from=from.normalize(); to=to.normalize();
	T d=T(from.dot(to)); if(d>T(0.999999)) return set_identity(); else if(d<T(-0.999999)) return set_scale(-1,-1,-1);
	V3 n=from.cross(to); T l=T(n.length());
	return set_rotate(n/l, asin(l));
}

template <class T>
__noinline tmat4<T>& tmat4<T>::set_rotate( const tvec3<T>& axis, T angle )
{
	T c=T(cos(angle)), s=T(sin(angle)), x=axis.x, y=axis.y, z=axis.z;
	_11 = x*x*(1-c)+c;		_12 = x*y*(1-c)-z*s;	_13 = x*z*(1-c)+y*s;	_14 = 0;
	_21 = x*y*(1-c)+z*s;	_22 = y*y*(1-c)+c;		_23 = y*z*(1-c)-x*s;	_24 = 0;
	_31 = x*z*(1-c)-y*s;	_32 = y*z*(1-c)+x*s;	_33 = z*z*(1-c)+c;		_34 = 0;
	_41 = 0;				_42 = 0;				_43 = 0;				_44 = T(1.0);
	return *this;
}

template <class T>
__noinline tvec4<T> tmat4<T>::_xdet() const
{
	return V((_41*_32-_31*_42)*_23+(_21*_42-_41*_22)*_33+(_31*_22-_21*_32)*_43,
			 (_31*_42-_41*_32)*_13+(_41*_12-_11*_42)*_33+(_11*_32-_31*_12)*_43,
			 (_41*_22-_21*_42)*_13+(_11*_42-_41*_12)*_23+(_21*_12-_11*_22)*_43,
			 (_21*_32-_31*_22)*_13+(_31*_12-_11*_32)*_23+(_11*_22-_21*_12)*_33);
}

template <class T>
__noinline tmat4<T> tmat4<T>::inverse() const
{
	auto xd=_xdet();
	// http://www.cg.info.hiroshima-cu.ac.jp/~miyazaki/knowledge/teche23.html
	return tmat4((_32*_43-_42*_33)*_24 + (_42*_23-_22*_43)*_34 + (_22*_33-_32*_23)*_44,
				 (_42*_33-_32*_43)*_14 + (_12*_43-_42*_13)*_34 + (_32*_13-_12*_33)*_44,
				 (_22*_43-_42*_23)*_14 + (_42*_13-_12*_43)*_24 + (_12*_23-_22*_13)*_44,
				 (_32*_23-_22*_33)*_14 + (_12*_33-_32*_13)*_24 + (_22*_13-_12*_23)*_34,
				 (_41*_33-_31*_43)*_24 + (_21*_43-_41*_23)*_34 + (_31*_23-_21*_33)*_44,
				 (_31*_43-_41*_33)*_14 + (_41*_13-_11*_43)*_34 + (_11*_33-_31*_13)*_44,
				 (_41*_23-_21*_43)*_14 + (_11*_43-_41*_13)*_24 + (_21*_13-_11*_23)*_44,
				 (_21*_33-_31*_23)*_14 + (_31*_13-_11*_33)*_24 + (_11*_23-_21*_13)*_34,
				 (_31*_42-_41*_32)*_24 + (_41*_22-_21*_42)*_34 + (_21*_32-_31*_22)*_44,
				 (_41*_32-_31*_42)*_14 + (_11*_42-_41*_12)*_34 + (_31*_12-_11*_32)*_44,
				 (_21*_42-_41*_22)*_14 + (_41*_12-_11*_42)*_24 + (_11*_22-_21*_12)*_44,
				 (_31*_22-_21*_32)*_14 + (_11*_32-_31*_12)*_24 + (_21*_12-_11*_22)*_34,
				 xd.x, xd.y, xd.z, xd.w )*(T(1.0)/cvec(3).dot(xd));
}

//*************************************
// type definitions and size check
using mat2	= tmat2<float>;		using mat3 = tmat3<float>;		using mat4 = tmat4<float>;
using dmat2	= tmat2<double>;	using dmat3 = tmat3<double>;	using dmat4 = tmat4<double>;

//*************************************
// matrix size check
static_assert(sizeof(mat2)%sizeof(float)*4==0,"sizeof(mat2)!=sizeof(float)*4" );
static_assert(sizeof(mat3)%sizeof(float)*9==0,"sizeof(mat3)!=sizeof(float)*9" );
static_assert(sizeof(mat4)%sizeof(float)*16==0,"sizeof(mat4)!=sizeof(float)*16" );
static_assert(sizeof(dmat2)%sizeof(double)*4==0,"sizeof(dmat2)!=sizeof(double)*4" );
static_assert(sizeof(dmat3)%sizeof(double)*9==0,"sizeof(dmat3)!=sizeof(double)*9" );
static_assert(sizeof(dmat4)%sizeof(double)*16==0,"sizeof(dmat4)!=sizeof(double)*16" );

//*************************************
// string-matrix conversion functions
inline const char* ftoa( const mat2& m ){ const auto* f=&m._11;static const char* fmt="%g %g %g %g";size_t size=size_t(snprintf(0,0,fmt,f[0],f[1],f[2],f[3]));char* buff=__strbuf(size); snprintf(buff,size+1,fmt,f[0],f[1],f[2],f[3]);return buff;}
inline const char* ftoa( const mat3& m ){ const auto* f=&m._11;static const char* fmt="%g %g %g %g %g %g %g %g %g";size_t size=size_t(snprintf(0,0,fmt,f[0],f[1],f[2],f[3],f[4],f[5],f[6],f[7],f[8]));char* buff=__strbuf(size); snprintf(buff,size+1,fmt,f[0],f[1],f[2],f[3],f[4],f[5],f[6],f[7],f[8]);return buff;}
inline const char* ftoa( const mat4& m ){ const auto* f=&m._11;static const char* fmt="%g %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g";size_t size=size_t(snprintf(0,0,fmt,f[0],f[1],f[2],f[3],f[4],f[5],f[6],f[7],f[8],f[9],f[10],f[11],f[12],f[13],f[14],f[15]));char* buff=__strbuf(size); snprintf(buff,size+1,fmt,f[0],f[1],f[2],f[3],f[4],f[5],f[6],f[7],f[8],f[9],f[10],f[11],f[12],f[13],f[14],f[15]);return buff;}
inline const char* ftoa( const dmat2&m ){ const auto* f=&m._11;static const char* fmt="%g %g %g %g";size_t size=size_t(snprintf(0,0,fmt,f[0],f[1],f[2],f[3]));char* buff=__strbuf(size); snprintf(buff,size+1,fmt,f[0],f[1],f[2],f[3]);return buff;}
inline const char* ftoa( const dmat3&m ){ const auto* f=&m._11;static const char* fmt="%g %g %g %g %g %g %g %g %g";size_t size=size_t(snprintf(0,0,fmt,f[0],f[1],f[2],f[3],f[4],f[5],f[6],f[7],f[8]));char* buff=__strbuf(size); snprintf(buff,size+1,fmt,f[0],f[1],f[2],f[3],f[4],f[5],f[6],f[7],f[8]);return buff;}
inline const char* ftoa( const dmat4&m ){ const auto* f=&m._11;static const char* fmt="%g %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g";size_t size=size_t(snprintf(0,0,fmt,f[0],f[1],f[2],f[3],f[4],f[5],f[6],f[7],f[8],f[9],f[10],f[11],f[12],f[13],f[14],f[15]));char* buff=__strbuf(size); snprintf(buff,size+1,fmt,f[0],f[1],f[2],f[3],f[4],f[5],f[6],f[7],f[8],f[9],f[10],f[11],f[12],f[13],f[14],f[15]);return buff;}
inline mat3 atof9( const char* a ){		mat3 m;char* e=0; for(int k=0;k<9;k++,a=e) (&m._11)[k]=strtof(a,&e); return m; }
inline mat4 atof16( const char* a ){	mat4 m;char* e=0; for(int k=0;k<16;k++,a=e)(&m._11)[k]=strtof(a,&e); return m; }
inline dmat3 atod9( const char* a ){	dmat3 m;char* e=0;for(int k=0;k<9;k++,a=e) (&m._11)[k]=strtod(a,&e); return m; }
inline dmat4 atod16( const char* a ){	dmat4 m;char* e=0;for(int k=0;k<16;k++,a=e)(&m._11)[k]=strtod(a,&e); return m; }

//*************************************
// vertor-matrix multiplications
__forceinline vec2 operator*( const vec2& v, const mat2& m ){ return m.transpose()*v; }
__forceinline vec3 operator*( const vec3& v, const mat3& m ){ return m.transpose()*v; }
__forceinline vec3 operator*( const vec3& v, const mat4& m ){ return m.transpose()*v; }
__forceinline vec4 operator*( const vec4& v, const mat4& m ){ return m.transpose()*v; }
__forceinline vec2 mul( const vec2& v, const mat2& m ){ return m.transpose()*v; }
__forceinline vec3 mul( const vec3& v, const mat3& m ){ return m.transpose()*v; }
__forceinline vec3 mul( const vec3& v, const mat4& m ){ return m.transpose()*v; }
__forceinline vec4 mul( const vec4& v, const mat4& m ){ return m.transpose()*v; }
__forceinline vec2 mul( const mat2& m, const vec2& v ){ return m*v; }
__forceinline vec3 mul( const mat3& m, const vec3& v ){ return m*v; }
__forceinline vec3 mul( const mat4& m, const vec3& v ){ return m*v; }
__forceinline vec4 mul( const mat4& m, const vec4& v ){ return m*v; }

//*************************************
// scalar-vector algebra
template <class T> __forceinline tvec2<T> operator+( T f, const tvec2<T>& v ){ return v+f; }
template <class T> __forceinline tvec2<signed_t<T>> operator-( T f, const tvec2<T>& v ){ return -v+f; }
template <class T> __forceinline tvec2<T> operator*( T f, const tvec2<T>& v ){ return v*f; }
template <class T> __forceinline tvec2<T> operator/( T f, const tvec2<T>& v ){ return tvec2<T>(f/v.x,f/v.y); }
template <class T> __forceinline tvec3<T> operator+( T f, const tvec3<T>& v ){ return v+f; }
template <class T> __forceinline tvec3<signed_t<T>> operator-( T f, const tvec3<T>& v ){ return -v+f; }
template <class T> __forceinline tvec3<T> operator*( T f, const tvec3<T>& v ){ return v*f; }
template <class T> __forceinline tvec3<T> operator/( T f, const tvec3<T>& v ){ return tvec3<T>(f/v.x,f/v.y,f/v.z); }
template <class T> __forceinline tvec4<T> operator+( T f, const tvec4<T>& v ){ return v+f; }
template <class T> __forceinline tvec4<signed_t<T>> operator-( T f, const tvec4<T>& v ){ return -v+f; }
template <class T> __forceinline tvec4<T> operator*( T f, const tvec4<T>& v ){ return v*f; }
template <class T> __forceinline tvec4<T> operator/( T f, const tvec4<T>& v ){ return tvec4<T>(f/v.x,f/v.y,f/v.z,f/v.w); }

//*************************************
// global operators for vector length/normalize/dot/cross
template <class T> __forceinline floating_point_t<T> length( const tvec2<T>& v ){ return v.length(); }
template <class T> __forceinline floating_point_t<T> length( const tvec3<T>& v ){ return v.length(); }
template <class T> __forceinline floating_point_t<T> length( const tvec4<T>& v ){ return v.length(); }
template <class T> __forceinline floating_point_t<T> length2( const tvec2<T>& v ){ return v.length2(); }
template <class T> __forceinline floating_point_t<T> length2( const tvec3<T>& v ){ return v.length2(); }
template <class T> __forceinline floating_point_t<T> length2( const tvec4<T>& v ){ return v.length2(); }
template <class T> __forceinline tvec2<floating_point_t<T>> normalize( const tvec2<T>& v ){ return v.normalize(); }
template <class T> __forceinline tvec3<floating_point_t<T>> normalize( const tvec3<T>& v ){ return v.normalize(); }
template <class T> __forceinline tvec4<floating_point_t<T>> normalize( const tvec4<T>& v ){ return v.normalize(); }
template <class T> __forceinline floating_point_t<T> dot( const tvec2<T>& v1, const tvec2<T>& v2){ return v1.dot(v2); }
template <class T> __forceinline floating_point_t<T> dot( const tvec3<T>& v1, const tvec3<T>& v2){ return v1.dot(v2); }
template <class T> __forceinline floating_point_t<T> dot( const tvec4<T>& v1, const tvec4<T>& v2){ return v1.dot(v2); }
template <class T> __forceinline tvec3<floating_point_t<T>> cross( const tvec3<T>& v1, const tvec3<T>& v2){ return v1.cross(v2); }

//*************************************
// general math utility functions
template <class T> __forceinline floating_point_t<T> radians( T f ){ return f*T_PI<T>/T(180.0); }
template <class T> __forceinline floating_point_t<T> degrees( T f ){ return f*T(180.0)/T_PI<T>; }
template <class T> __forceinline tvec2<floating_point_t<T>> minmax( const tvec2<T>& a, const tvec2<T>& b ){ return tvec2<T>(a.x<b.x?a.x:b.x,a.y>b.y?a.y:b.y); }
template <class T> __forceinline floating_point_t<T> round( T f, int digits ){ T m=T(pow(10.0,digits)); return round(f*m)/m; }
__forceinline bool ispot( uint i ){ return (i&(i-1))==0; }		// http://en.wikipedia.org/wiki/Power_of_two
__forceinline uint nextpot( uint n ){ int m=int(n)-1; for( uint k=1; k<uint(sizeof(int))*8; k<<=1 ) m=m|m>>k; return m+1; }	// closest (equal or larger) power-of-two
__forceinline uint nextsqrt( uint n ){ return uint(ceil(sqrt(double(n)))+0.001); } // root of closest (equal or larger) square
__forceinline uint nextsquare( uint n ){ uint r=nextsqrt(n); return r*r; } // closest (equal or larger) square
__forceinline uint miplevels( uint width, uint height=1 ){ uint l=0; uint s=width>height?width:height; while(s){s=s>>1;l++;} return l; }
__forceinline float rsqrt( float x ){ float y=0.5f*x; int i=*(int*)&x; i=0x5F375A86-(i>>1); x=*(float*)&i; x=x*(1.5f-y*x*x); x=x*(1.5f-y*x*x); return x; }						// Quake3's Fast InvSqrt(): 1/sqrt(x): magic number changed from 0x5f3759df to 0x5F375A86 for more accuracy; 2 iteration has quite good accuracy
__forceinline double rsqrt( double x ){ double y=0.5*x; int64_t i=*(int64_t*)&x; i=0x5FE6EB50C7B537A9-(i>>1); x=*(double*)&i; x=x*(1.5-y*x*x); x=x*(1.5-y*x*x); return x; }		// Quake3's Fast InvSqrt(): 1/sqrt(x): 64-bit magic number (0x5FE6EB50C7B537A9) used; 2 iteration has quite good accuracy
__forceinline uint bitswap( uint n ){ n=((n&0x55555555)<<1)|((n&0xaaaaaaaa)>>1); n=((n&0x33333333)<<2)|((n&0xcccccccc)>>2); n=((n&0x0f0f0f0f)<<4)|((n&0xf0f0f0f0)>>4); n=((n&0x00ff00ff)<<8)|((n&0xff00ff00)>>8); return (n<<16)|(n>>16); }
__forceinline float triangle_area( vec2 a, vec2 b, vec2 c ){ return abs(a.x*b.y+b.x*c.y+c.x*a.y-a.x*c.y-c.x*b.y-b.x*a.y)*0.5f; }

//*************************************
// viewport helpers
__forceinline ivec4 effective_viewport( int width, int height, double aspect_to_keep ){ int w=int(height*aspect_to_keep), h=int(width/aspect_to_keep); if((width*h)==(height*w)) return ivec4{0,0,width,height}; return width>w?ivec4{(width-w)/2,0,w,height}:ivec4{0,(height-h)/2,width,h}; }
__forceinline ivec4 effective_viewport( int width, int height, int width_to_keep, int height_to_keep ){ return (width*height_to_keep)==(height*width_to_keep)?ivec4{0,0,width,height}:effective_viewport(width,height,width_to_keep/double(height_to_keep)); }
__forceinline ivec4 effective_viewport( int width, int height, ivec2 aspect_to_keep ){ return effective_viewport(width,height,aspect_to_keep.x,aspect_to_keep.y); }
__forceinline ivec4 effective_viewport( ivec2 size, double aspect_to_keep ){ return effective_viewport(size.x,size.y,aspect_to_keep); }
__forceinline ivec4 effective_viewport( ivec2 size, int width_to_keep, int height_to_keep ){ return effective_viewport(size.x,size.y,width_to_keep,height_to_keep); }
__forceinline ivec4 effective_viewport( ivec2 size, ivec2 aspect_to_keep ){ return effective_viewport(size.x,size.y,aspect_to_keep.x,aspect_to_keep.y); }

//*************************************
// {GLSL|HLSL}-like shader intrinsic functions
__forceinline float fract( float f ){ return float(f-floor(f)); }
__forceinline float saturate( float f ){ return f<0.0f?0.0f:f>1.0f?1.0f:f; }
__forceinline int sign( int f ){ return f>0?1:f<0?-1:0; }
__forceinline float sign( float f ){ return f>0.0f?1.0f:f<0.0f?-1.0f:0; }

#define VEC2F(f) __forceinline vec2 f( const vec2& v ){ return vec2(f(v.x),f(v.y)); }
#define VEC3F(f) __forceinline vec3 f( const vec3& v ){ return vec3(f(v.x),f(v.y),f(v.z)); }
#define VEC4F(f) __forceinline vec4 f( const vec4& v ){ return vec4(f(v.x),f(v.y),f(v.z),f(v.w)); }
#define VECF(f)	VEC2F(f) VEC3F(f) VEC4F(f)
VECF(cos)	VECF(sin)	VECF(tan)	VECF(acos)	VECF(asin)	VECF(atan)
VECF(cosh)	VECF(sinh)	VECF(tanh)	VECF(acosh)	VECF(asinh)	VECF(atanh)
VECF(abs)	VECF(ceil)	VECF(fabs)	VECF(floor)	VECF(fract)	VECF(saturate)	VECF(sign)
#undef VEC2F
#undef VEC3F
#undef VEC4F
#undef VECF

__forceinline float distance( const vec2& a, const vec2& b ){ return (a-b).length(); }
__forceinline float distance( const vec3& a, const vec3& b ){ return (a-b).length(); }
__forceinline float distance( const vec4& a, const vec4& b ){ return (a-b).length(); }
__forceinline vec2 fma( vec2 a, vec2 b, vec2 c ){ return vec2(fma(a.x,b.x,c.x),fma(a.y,b.y,c.y)); }
__forceinline vec3 fma( vec3 a, vec3 b, vec3 c ){ return vec3(fma(a.x,b.x,c.x),fma(a.y,b.y,c.y),fma(a.z,b.z,c.z)); }
__forceinline vec4 fma( vec4 a, vec4 b, vec4 c ){ return vec4(fma(a.x,b.x,c.x),fma(a.y,b.y,c.y),fma(a.z,b.z,c.z),fma(a.w,b.w,c.w)); }
__forceinline float lerp( float v1, float v2, float t ){ return v1*(1.0f-t)+v2*t; }
__forceinline vec2 lerp( const vec2& y1, const vec2& y2, float t ){ return y1*(-t+1.0f)+y2*t; }
__forceinline vec3 lerp( const vec3& y1, const vec3& y2, float t ){ return y1*(-t+1.0f)+y2*t; }
__forceinline vec4 lerp( const vec4& y1, const vec4& y2, float t ){ return y1*(-t+1.0f)+y2*t; }
__forceinline vec2 lerp( const vec2& y1, const vec2& y2, const vec2& t ){ return y1*(1.0f-t)+y2*t; }
__forceinline vec3 lerp( const vec3& y1, const vec3& y2, const vec3& t ){ return y1*(1.0f-t)+y2*t; }
__forceinline vec4 lerp( const vec4& y1, const vec4& y2, const vec4& t ){ return y1*(1.0f-t)+y2*t; }
__forceinline double lerp( double v1, double v2, double t ){ return v1*(1.0-t)+v2*t; }
__forceinline double lerp( double v1, double v2, float t ){ return v1*(1.0-double(t))+v2*t; }
__forceinline dvec2 lerp( const dvec2& y1, const dvec2& y2, double t ){ return y1*(-t+1.0)+y2*t; }
__forceinline dvec3 lerp( const dvec3& y1, const dvec3& y2, double t ){ return y1*(-t+1.0)+y2*t; }
__forceinline dvec4 lerp( const dvec4& y1, const dvec4& y2, double t ){ return y1*(-t+1.0)+y2*t; }
__forceinline dvec2 lerp( const dvec2& y1, const dvec2& y2, float t ){ return y1*(-t+1.0)+y2*t; }
__forceinline dvec3 lerp( const dvec3& y1, const dvec3& y2, float t ){ return y1*(-t+1.0)+y2*t; }
__forceinline dvec4 lerp( const dvec4& y1, const dvec4& y2, float t ){ return y1*(-t+1.0)+y2*t; }
__forceinline mat4 lerp( const mat4& v1, const mat4& v2, float t ){ return v1*(1.0f-t)+v2*t; }
__forceinline float mix( float v1, float v2, float t ){ return v1*(1.0f-t)+v2*t; }
__forceinline vec2 mix( const vec2& y1, const vec2& y2, float t ){ return y1*(-t+1.0f)+y2*t; }
__forceinline vec3 mix( const vec3& y1, const vec3& y2, float t ){ return y1*(-t+1.0f)+y2*t; }
__forceinline vec4 mix( const vec4& y1, const vec4& y2, float t ){ return y1*(-t+1.0f)+y2*t; }
__forceinline vec2 mix( const vec2& y1, const vec2& y2, const vec2& t ){ return y1*(1.0f-t)+y2*t; }
__forceinline vec3 mix( const vec3& y1, const vec3& y2, const vec3& t ){ return y1*(1.0f-t)+y2*t; }
__forceinline vec4 mix( const vec4& y1, const vec4& y2, const vec4& t ){ return y1*(1.0f-t)+y2*t; }
__forceinline double mix( double v1, double v2, double t ){ return v1*(1.0-t)+v2*t; }
__forceinline double mix( double v1, double v2, float t ){ return v1*(1.0-double(t))+v2*t; }
__forceinline dvec2 mix( const dvec2& y1, const dvec2& y2, double t ){ return y1*(-t+1.0)+y2*t; }
__forceinline dvec3 mix( const dvec3& y1, const dvec3& y2, double t ){ return y1*(-t+1.0)+y2*t; }
__forceinline dvec4 mix( const dvec4& y1, const dvec4& y2, double t ){ return y1*(-t+1.0)+y2*t; }
__forceinline dvec2 mix( const dvec2& y1, const dvec2& y2, float t ){ return y1*(-t+1.0)+y2*t; }
__forceinline dvec3 mix( const dvec3& y1, const dvec3& y2, float t ){ return y1*(-t+1.0)+y2*t; }
__forceinline dvec4 mix( const dvec4& y1, const dvec4& y2, float t ){ return y1*(-t+1.0)+y2*t; }
__forceinline mat4 mix( const mat4& v1, const mat4& v2, float t ){ return v1*(1.0f-t)+v2*t; }
__forceinline vec3 reflect( const vec3& I, const vec3& N ){ return I-N*dot(I,N)*2.0f; }	// I: incident vector, N: normal
__forceinline vec3 refract( const vec3& I, const vec3& N, float eta /* = n0/n1 */ ){ float d = I.dot(N); float k = 1.0f-eta*eta*(1.0f-d*d); return k<0.0f?0.0f:(I*eta-N*(eta*d+sqrtf(k))); } // I: incident vector, N: normal
__forceinline float smoothstep(float edge0, float edge1, float t ){	t=saturate((t-edge0)/(edge1-edge0)); return t*t*(3-2*t); } // C1-continuity
__forceinline vec2 smoothstep( float edge0, float edge1, vec2 t ){	t=saturate((t-edge0)/(edge1-edge0)); return t*t*(3.0f-t*2.0f); }
__forceinline vec3 smoothstep( float edge0, float edge1, vec3 t ){	t=saturate((t-edge0)/(edge1-edge0)); return t*t*(3.0f-t*2.0f); }
__forceinline vec4 smoothstep( float edge0, float edge1, vec4 t ){	t=saturate((t-edge0)/(edge1-edge0)); return t*t*(3.0f-t*2.0f); }
__forceinline float smoothstep( float t ){ t=saturate(t); return t*t*(3-2*t); } // C1-continuity
__forceinline vec2 smoothstep( const vec2& t ){ return vec2(smoothstep(t.x),smoothstep(t.y)); }
__forceinline vec3 smoothstep( const vec3& t ){ return vec3(smoothstep(t.x),smoothstep(t.y),smoothstep(t.z)); }
__forceinline vec4 smoothstep( const vec4& t ){ return vec4(smoothstep(t.x),smoothstep(t.y),smoothstep(t.z),smoothstep(t.w)); }
__forceinline float smootherstep( float edge0, float edge1, float t ){	t=saturate((t-edge0)/(edge1-edge0)); return t*t*t*(6.0f*t*t-15.0f*t+10.0f); } // C2-continuity (by Ken Perlin)
__forceinline vec2 smootherstep( float edge0, float edge1, vec2 t ){	t=saturate((t-edge0)/(edge1-edge0)); return t*t*t*(6.0f*t*t-15.0f*t+10.0f); }
__forceinline vec3 smootherstep( float edge0, float edge1, vec3 t ){	t=saturate((t-edge0)/(edge1-edge0)); return t*t*t*(6.0f*t*t-15.0f*t+10.0f); }
__forceinline vec4 smootherstep( float edge0, float edge1, vec4 t ){	t=saturate((t-edge0)/(edge1-edge0)); return t*t*t*(6.0f*t*t-15.0f*t+10.0f); }
__forceinline float smootherstep( float t ){ t=saturate(t); return t*t*t*(6.0f*t*t-15.0f*t+10.0f); } // C2-continuity (by Ken Perlin)
__forceinline vec2 smootherstep( const vec2& t ){ return vec2(smootherstep(t.x),smootherstep(t.y)); }
__forceinline vec3 smootherstep( const vec3& t ){ return vec3(smootherstep(t.x),smootherstep(t.y),smootherstep(t.z)); }
__forceinline vec4 smootherstep( const vec4& t ){ return vec4(smootherstep(t.x),smootherstep(t.y),smootherstep(t.z),smootherstep(t.w)); }
// packing/unpacking/casting: https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_shading_language_packing.txt
__forceinline uint packUnorm2x16( vec2 v ){ ushort2 u={}; for(int k=0;k<2;k++) (&u.x)[k]=ushort(round(clamp(v[k],0.0f,1.0f)*65535.0f)); return reinterpret_cast<uint&>(u); }
__forceinline uint packSnorm2x16( vec2 v ){ short2 s={}; for(int k=0;k<2;k++) (&s.x)[k]=short(round(clamp(v[k],-1.0f,1.0f)*32767.0f)); return reinterpret_cast<uint&>(s); }
__forceinline uint packUnorm4x8( vec4 v ){ uchar4 u={}; for(int k=0;k<4;k++) (&u.x)[k]=uchar(round(clamp(v[k],0.0f,1.0f)*255.0f)); return reinterpret_cast<uint&>(u); }
__forceinline uint packSnorm4x8( vec4 v ){ char4 s={}; for(int k=0;k<4;k++) (&s.x)[k]=char(round(clamp(v[k],-1.0f,1.0f)*127.0f)); return reinterpret_cast<uint&>(s); }
#ifndef __GXMATH_NO_HALF__
__forceinline uint packHalf2x16( vec2 v ){ half2 h=ftoh(v); return reinterpret_cast<uint&>(h); }
#endif
__forceinline vec2 unpackUnorm2x16( uint u ){ vec2 v={}; for(int k=0;k<2;k++) v[k]=((ushort*)&u)[k]/65535.0f; return v; }
__forceinline vec2 unpackSnorm2x16( uint u ){ vec2 v={}; for(int k=0;k<2;k++) v[k]=clamp(((short*)&u)[k]/32767.0f,-1.0f,1.0f); return v; }
__forceinline vec4 unpackUnorm4x8( uint u ){ vec4 v={}; for(int k=0;k<4;k++) v[k]=((uchar*)&u)[k]/255.0f; return v; }
__forceinline vec4 unpackSnorm4x8( uint u ){ vec4 v={}; for(int k=0;k<4;k++) v[k]=clamp(((char*)&u)[k]/127.0f,-1.0f,1.0f); return v; }
#ifndef __GXMATH_NO_HALF__
__forceinline vec2 unpackHalf2x16( uint u ){ return htof(reinterpret_cast<half2&>(u)); }
#endif
__forceinline uint  packUint4x8( uint4 v ){ return (v.x&0xff)+((v.y&0xff)<<8)+((v.z&0xff)<<16)+((v.w&0xff)<<24); }
__forceinline uvec4 unpackUint4x8( uint u ){ return uvec4(u&0xff,(u>>8)&0xff,(u>>16)&0xff,(u>>24)&0xff); }
__forceinline uint  floatBitsToUint( float f ){ return reinterpret_cast<uint&>(f); }
__forceinline int   floatBitsToInt( float f ){ return reinterpret_cast<int&>(f); }
__forceinline float intBitsToFloat( int i ){ return reinterpret_cast<float&>(i); }
__forceinline float uintBitsToFloat( uint u ){ return reinterpret_cast<float&>(u); }
__forceinline uint  bitfieldExtract( uint  u, int offset, int bits ){ return (u>>offset)&(~(0xffffffff<<bits)); }
__forceinline uvec2 bitfieldExtract( uvec2 u, int offset, int bits ){ return uvec2{bitfieldExtract(u.x,offset,bits),bitfieldExtract(u.y,offset,bits)}; }
__forceinline uvec3 bitfieldExtract( uvec3 u, int offset, int bits ){ return uvec3{bitfieldExtract(u.x,offset,bits),bitfieldExtract(u.y,offset,bits),bitfieldExtract(u.z,offset,bits)}; }
__forceinline uvec4 bitfieldExtract( uvec4 u, int offset, int bits ){ return uvec4{bitfieldExtract(u.x,offset,bits),bitfieldExtract(u.y,offset,bits),bitfieldExtract(u.z,offset,bits),bitfieldExtract(u.w,offset,bits)}; }

// casting for normalized vec3 in [0,1]
__forceinline vec2 normVec3BitsToVec2( vec3 v )
{
	static const uint cap=21, hcap=10, cmask=0x1fffff, hmask=0x3ff;	// channel capacity, half capacity, channel capacity mask (=(1<<cap)-1), half capacity mask (=(1<<hcap)-1)
	uvec3 u = uvec3(uint(v.x*cmask),uint(v.y*cmask),uint(v.z*cmask));
	return vec2( uintBitsToFloat(u.x|((u.z&~hmask)<<(cap-hcap))), uintBitsToFloat((u.y<<1)|(u.z&hmask)<<(cap+1)|1) ); // bits = ( [z.11,x.21], [z.10,y.21,validity-bit] ) 
}
__forceinline vec3 vec2BitsToNormVec3( vec2 v )
{
	static const uint cap=21, hcap=10, cmask=0x1fffff;	// channel capacity, half capacity, channel capacity mask (=(1<<cap)-1)
	uvec2 u = uvec2( floatBitsToUint(v.x), floatBitsToUint(v.y) );
	return vec3(float(u.x&cmask),float((u.y>>1)&cmask),float(((u.x&~cmask)>>(cap-hcap))|(u.y>>(cap+1))))/float(cmask);
}

//*************************************
// spline interpolations
template <class T> T hermite( T v0, T v1, T v2, T v3, double t, double tension=0.5, double bias=0.0, double continuity=-0.5 )
{
	// REF: http://en.wikipedia.org/wiki/Kochanek%E2%80%93Bartels_spline
	// parameters[-1,1]: tension (round--tight), bias (postshoot--preshoot), continuity (box--inverted corners)
	double t2=t*t, t3=t*t*t, a[4]={ 2*t3-3*t2+1,t3-2*t2+t,t3-t2,-2*t3+3*t2 };
	T m0 = (v1-v0)*(1+bias)*(1-tension)*(1+continuity)*0.5 + (v2-v1)*(1-bias)*(1-tension)*(1-continuity)*0.5;
	T m1 = (v2-v1)*(1+bias)*(1-tension)*(1-continuity)*0.5 + (v3-v2)*(1-bias)*(1-tension)*(1+continuity)*0.5;
	return v1*a[0]+m0*a[1]+m1*a[2]+v2*a[3];
}

template <class T> T catmull_rom( T v0, T v1, T v2, T v3, double t )
{
	// REF: http://en.wikipedia.org/wiki/B-spline
	double t2=t*t, t3=t*t*t, a[4] = { -t3+2*t2-t, 3*t3-5*t2+2, -3*t3+4*t2+t, t3-t2 };
	return (v0*a[0]+v1*a[1]+v2*a[2]+v3*a[3])*0.5;
}

template <class T> T bezier( T v0, T v1, T v2, T v3, double t )
{
	// REF: http://en.wikipedia.org/wiki/Bezier_curve
	double t2=1.0-t, a[4] = { t2*t2*t2, 3*t2*t2*t, 3*t2*t*t, t*t*t };
	return v0*a[0]+v1*a[1]+v2*a[2]+v3*a[3];
}

//*************************************
namespace gx {
//*************************************
struct random_t
{
	uint			seed=std::mt19937::default_seed;
	std::mt19937	engine; // Mersenne Twister with period of (2^19937-1)
	std::uniform_real_distribution<float> distf=std::uniform_real_distribution<float>(0.0f,1.0f);
	float operator()(){ return distf(engine); }
	static random_t& singleton(){ static random_t* r=new random_t(); return *r; }
};
//*************************************
} // end namespace gx
//*************************************

// pseudo random number generator
__forceinline void	sprand( uint seed ){ static auto& r=gx::random_t::singleton(); r.engine.seed(r.seed=seed); }
__forceinline uint	urand(){ static auto& s=gx::random_t::singleton().seed; s=s*214013L+2531011L; return ((s>>16)&0x7fff); }

// vector/range helper based on Windows legacy rand()
__forceinline float	randf(){ return rand()/float(RAND_MAX); }
__forceinline vec2	randf2(){ return vec2(randf(),randf()); }
__forceinline vec3	randf3(){ return vec3(randf(),randf(),randf()); }
__forceinline vec4	randf4(){ return vec4(randf(),randf(),randf(),randf()); }
__forceinline float	randf( float fmin, float fmax ){ return randf()*(fmax-fmin)+fmin; }
__forceinline vec2	randf2( float fmin, float fmax ){ return randf2()*(fmax-fmin)+fmin; }
__forceinline vec3	randf3( float fmin, float fmax ){ return randf3()*(fmax-fmin)+fmin; }
__forceinline vec4	randf4( float fmin, float fmax ){ return randf4()*(fmax-fmin)+fmin; }

// pseudo random number generator based on mt199737
__forceinline float	prand(){ static auto& r=gx::random_t::singleton(); return r(); }
__forceinline vec2	prand2(){ return vec2(prand(),prand()); }
__forceinline vec3	prand3(){ return vec3(prand(),prand(),prand()); }
__forceinline vec4	prand4(){ return vec4(prand(),prand(),prand(),prand()); }
__forceinline float	prand( float fmin, float fmax ){ return prand()*(fmax-fmin)+fmin; }
__forceinline vec2	prand2( float fmin, float fmax ){ return prand2()*(fmax-fmin)+fmin; }
__forceinline vec3	prand3( float fmin, float fmax ){ return prand3()*(fmax-fmin)+fmin; }
__forceinline vec4	prand4( float fmin, float fmax ){ return prand4()*(fmax-fmin)+fmin; }

//*************************************
// xorshift-based random number generator: https://en.wikipedia.org/wiki/Xorshift
__forceinline uint  xorshift32( uint& x ){ x^=x<<13;x^=x>>17;x^=x<<5;return x; }
__forceinline float xrand(  uint& x ){ return float(x=xorshift32(x))*2.3283064e-10f; }
__forceinline vec2  xrand2( uint& x ){ return vec2(xrand(x),xrand(x)); }
__forceinline vec3  xrand3( uint& x ){ return vec3(xrand(x),xrand(x),xrand(x)); }
__forceinline vec4  xrand4( uint& x ){ return vec4(xrand(x),xrand(x),xrand(x),xrand(x)); }
__forceinline float xrand(  uint& x, float fmin, float fmax ){ return xrand(x)*(fmax-fmin)+fmin; }
__forceinline vec2  xrand2( uint& x, float fmin, float fmax ){ return xrand2(x)*(fmax-fmin)+fmin; }
__forceinline vec3  xrand3( uint& x, float fmin, float fmax ){ return xrand3(x)*(fmax-fmin)+fmin; }
__forceinline vec4  xrand4( uint& x, float fmin, float fmax ){ return xrand4(x)*(fmax-fmin)+fmin; }

//*************************************
// alternative wrappers for deprecated std::random_shuffle
template <class RandomIt>
void random_shuffle( RandomIt first, RandomIt last, uint seed=0 ){ std::mt19937 e=seed?std::mt19937(seed):std::mt19937(std::random_device()()); std::shuffle(first,last,e); }

template <class T=uint>
vector<T> random_shuffle_indices( size_t count, uint seed=0 )
{
	vector<T> v; v.resize(count);
	std::iota( v.begin(), v.end(), 0 );
	random_shuffle( v.begin(), v.end(), seed );
	return v;
}

//*************************************
#endif // __GX_MATH_H__
