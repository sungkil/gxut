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
#include <numbers>	// C++20

// using declarations
using std::floating_point;

// user-defined template type traits
template <template<typename...>class, template<typename...>class> constexpr bool same_template = false;
template <template<typename...>class T> constexpr bool same_template<T,T> = true;

using std::numbers::pi_v;
using std::numbers::inv_pi_v;
using std::numbers::inv_sqrtpi_v;
using std::numbers::sqrt2_v;
template <class T> constexpr T pi2_v = T(6.283185307179586);
#ifdef PI
#undef PI
#endif
constexpr float PI = pi_v<float>;
constexpr float pi = pi_v<float>;
constexpr float inv_pi = inv_pi_v<float>;
constexpr float inv_sqrtpi = inv_sqrtpi_v<float>;
constexpr float sqrt2 = sqrt2_v<float>;
constexpr float pi2 = pi2_v<float>;

// half-precision float and conversion
#ifndef __GXMATH_NO_HALF__
#define __GXMATH_HALF__
struct half { unsigned short mantissa:10,exponent:5,sign:1; __forceinline operator float() const; };	// IEEE 754-2008 half-precision (16-bit) floating-point storage. // https://github.com/HeliumProject/Helium/blob/master/Math/Float16.cpp
__forceinline float	htof( half value ){ struct _float32_t {uint mantissa:23,exponent:8,sign:1;}; _float32_t result={};result.sign = value.sign;uint exponent=value.exponent,mantissa=value.mantissa; if(exponent==31){result.exponent=255;result.mantissa=0;} else if(exponent==0&&mantissa==0){result.exponent=0;result.mantissa=0;} else if(exponent==0){uint mantissa_shift=10-static_cast<uint>(log2(float(mantissa)));result.exponent=127-(15-1)-mantissa_shift;result.mantissa=mantissa<<(mantissa_shift+23-10);} else{result.exponent=127-15+exponent;result.mantissa=static_cast<uint>(value.mantissa)<<(23-10);} return reinterpret_cast<float&>(result); }
__forceinline half::operator float() const {return htof(*this);}
__forceinline float* htof( const half* ph, float* pf, size_t nElements, size_t float_stride=1 ){ if(pf==nullptr||ph==nullptr||nElements==0) return pf; float* pf0=pf; for( size_t k=0; k < nElements; k++, ph++, pf+=float_stride ) *pf=htof(*ph); return pf0; }
__forceinline half ftoh( float f ){ struct _float32_t {uint mantissa:23,exponent:8,sign:1;}; _float32_t value=reinterpret_cast<_float32_t&>(f);half result={};result.sign=value.sign; uint exponent=value.exponent; if(exponent==255){result.exponent=31;result.mantissa=0;} else if(exponent<127-15+1){uint mantissa=(1<<23)|value.mantissa; uint mantissa_shift=(23-10)+(127-15+1)-exponent;result.exponent=0;result.mantissa=static_cast<ushort>(mantissa>>mantissa_shift);} else if(exponent>127+(31-15-1)){result.exponent=31-1;result.mantissa=1023;} else {result.exponent=static_cast<ushort>(127-15+exponent);result.mantissa=static_cast<ushort>(value.mantissa>>(23-10));} return result; }
__forceinline half* ftoh( const float* pf, half* ph, size_t nElements, size_t half_stride=1 ){ if(pf==nullptr||ph==nullptr||nElements==0) return ph; half* ph0=ph; for( size_t k=0; k < nElements; k++, pf++, ph+=half_stride ) *ph=ftoh(*pf); return ph0; }
using half2	= tarray2<half>; using half3 = tarray3<half>; using half4 = tarray4<half>;
#endif

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
	template <class X, class Y> __forceinline tvec2( X a, Y b ){x=T(a);y=T(b);}
	template <class X> __forceinline tvec2( tvec2<X> v ):tvec2(v.x,v.y){}

	// assignment operators
	__forceinline tvec2& operator=( A&& v ){ memcpy(this,&v,sizeof(v)); return *this; }
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
	__forceinline tvec2 operator-() const requires std::is_signed_v<T> { return tvec2(-x,-y); }

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
	__forceinline T length2() const requires floating_point<T> { return T(x*x+y*y); }
	__forceinline T norm2() const requires floating_point<T> { return T(x*x+y*y); }
	__forceinline T length() const requires floating_point<T> { return T(sqrt(x*x+y*y)); }
	__forceinline T norm() const requires floating_point<T> { return T(sqrt(x*x+y*y)); }
	__forceinline T dot( const tvec2& v ) const requires floating_point<T> { return x*v.x+y*v.y; }
	__forceinline tvec2 normalize() const requires floating_point<T> { return operator/(length()); }
};

template <class T> struct tvec3
{
	using A = tarray3<T>;
	using V2 = tvec2<T>;
	__default_array_impl(3,T,tvec3);
	union{struct{T x,y,z;};struct{T r,g,b;};union{V2 xy,rg;};struct{T _x;union{V2 yz,gb;};};A xyz,rgb;};

	// constructors
	__forceinline tvec3( A v ){x=v.x;y=v.y;z=v.z;}
	__forceinline tvec3( T a ){x=y=z=a;}
	__forceinline tvec3( T a, T b, T c ){x=a;y=b;z=c;}
	__forceinline tvec3( V2 v, T c ){x=v.x;y=v.y;z=c;}
	__forceinline tvec3( T a, V2 v ){x=a;y=v.x;z=v.y;}

	// extended constructors with explicit casting
	template <class X, class Y, class Z> __forceinline tvec3( X a, Y b, Z c ){x=T(a);y=T(b);z=T(c);}
	template <class X> __forceinline tvec3( tvec3<X> v ):tvec3(v.x,v.y,v.z){}
	template <class X, class Z> __forceinline tvec3( tvec2<X> v, Z c ):tvec3(v.x,v.y,c){}
	template <class X, class Y> __forceinline tvec3( X a, tvec2<Y> v ):tvec3(a,v.x,v.y){}

	// assignment operators
	__forceinline tvec3& operator=( A&& v ){ memcpy(this,&v,sizeof(v)); return *this; }
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
	__forceinline tvec3 operator-() const requires std::is_signed_v<T> { return tvec3(-x,-y,-z); }

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
	__forceinline T length2() const requires floating_point<T> { return T(x*x+y*y+z*z); }
	__forceinline T norm2() const requires floating_point<T> { return T(x*x+y*y+z*z); }
	__forceinline T length() const requires floating_point<T> { return T(sqrt(x*x+y*y+z*z)); }
	__forceinline T norm() const requires floating_point<T> { return T(sqrt(x*x+y*y+z*z)); }
	__forceinline T dot( const tvec3& v ) const requires floating_point<T> { return x*v.x+y*v.y+z*v.z; }
	__forceinline tvec3 normalize() const requires floating_point<T> { return operator/(length()); }

	// tvec3 only: cross product
	__forceinline tvec3 cross( const tvec3& v ) const requires floating_point<T> { return tvec3( y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x ); }
	friend __forceinline tvec3<T> cross( const tvec3<T>& v0, const tvec3<T>& v1){ return v0.cross(v1); }
};

template <class T> struct tvec4
{
	using A = tarray4<T>;
	using V2 = tvec2<T>;
	using V3 = tvec3<T>;
	__default_array_impl(4,T,tvec4);
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
	template <class X, class Y, class Z, class W> __forceinline tvec4( X a, Y b, Z c, W d ){x=T(a);y=T(b);z=T(c);w=T(d);}
	template <class X> __forceinline tvec4( tvec4<X> v ):tvec4(v.x,v.y,v.z,v.w){}
	template <class X, class Y, class W> __forceinline tvec4( X a, tvec2<Y> v, W d ):tvec4(a,v.x,v.y,d){}
	template <class X, class Y> __forceinline tvec4( X a, tvec3<Y> v ):tvec4(a,v.x,v.y,v.z){}
	template <class X, class Y, class Z> __forceinline tvec4( X a, Y b, tvec2<Z> v ):tvec4(a,b,v.x,v.y){}
	template <class X, class Z, class W> __forceinline tvec4( tvec2<X> v, Z c, W d ):tvec4(v.x,v.y,c,d){}
	template <class X, class Z> __forceinline tvec4( tvec2<X> v, tvec2<Z> u ):tvec4(v.x,v.y,u.x,u.y){}
	template <class X, class W> __forceinline tvec4( tvec3<X> v, W d ):tvec4(v.x,v.y,v.z,d){}

	// assignment operators
	__forceinline tvec4& operator=( A&& v ){ memcpy(this,&v,sizeof(v)); return *this; }
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
	__forceinline tvec4 operator-() const requires std::is_signed_v<T> { return tvec4(-x,-y,-z,-w); }

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
	__forceinline T length2() const requires floating_point<T> { return T(x*x+y*y+z*z+w*w); }
	__forceinline T norm2() const requires floating_point<T> { return T(x*x+y*y+z*z+w*w); }
	__forceinline T length() const requires floating_point<T> { return T(sqrt(x*x+y*y+z*z+w*w)); }
	__forceinline T norm() const requires floating_point<T> { return T(sqrt(x*x+y*y+z*z+w*w)); }
	__forceinline T dot( const tvec4& v ) const requires floating_point<T> { return x*v.x+y*v.y+z*v.z+w*v.w; }
	__forceinline tvec4 normalize() const requires floating_point<T> { return operator/(length()); }
};

// member function specialization
template <floating_point T> struct precision { static const T value(){ return std::numeric_limits<T>::epsilon()*30; } };	// need to be 30x for robust practical test
template<> __forceinline bool tvec2<float>::operator==(const tvec2& v) const { static const float p=precision<float>::value(); return std::abs(x-v.x)<=p&&std::abs(y-v.y)<=p; }
template<> __forceinline bool tvec3<float>::operator==(const tvec3& v) const { static const float p=precision<float>::value(); return std::abs(x-v.x)<=p&&std::abs(y-v.y)<=p&&std::abs(z-v.z)<=p; }
template<> __forceinline bool tvec4<float>::operator==(const tvec4& v) const { static const float p=precision<float>::value(); return std::abs(x-v.x)<=p&&std::abs(y-v.y)<=p&&std::abs(z-v.z)<=p&&std::abs(w-v.w)<=p; }
template<> __forceinline bool tvec2<double>::operator==(const tvec2& v) const { static const double p=precision<double>::value(); return std::abs(x-v.x)<=p&&std::abs(y-v.y)<=p; }
template<> __forceinline bool tvec3<double>::operator==(const tvec3& v) const { static const double p=precision<double>::value(); return std::abs(x-v.x)<=p&&std::abs(y-v.y)<=p&&std::abs(z-v.z)<=p; }
template<> __forceinline bool tvec4<double>::operator==(const tvec4& v) const { static const double p=precision<double>::value(); return std::abs(x-v.x)<=p&&std::abs(y-v.y)<=p&&std::abs(z-v.z)<=p&&std::abs(w-v.w)<=p; }

// global operators for vector
#define __v_t_x__ template <template<typename...> class V, floating_point T, class X> requires tvec<V>&&std::convertible_to<X,T> __forceinline
__v_t_x__ V<T> operator+( X f, const V<T>& v ){ return v+f; }
__v_t_x__ V<T> operator-( X f, const V<T>& v ){ return -v+f; }
__v_t_x__ V<T> operator*( X f, const V<T>& v ){ return v*f; }
__v_t_x__ V<T> operator/( X f, const V<T>& v ){ return V<T>(f)/v; }
#undef __v_t_x__

// alias type definitions
using bvec2 = tvec2<bool>;			using bvec3 = tvec3<bool>;			using bvec4 = tvec4<bool>;
using vec2	= tvec2<float>;			using vec3	= tvec3<float>;			using vec4	= tvec4<float>;
using dvec2 = tvec2<double>;		using dvec3 = tvec3<double>;		using dvec4 = tvec4<double>;
using ivec2 = tvec2<int>;			using ivec3 = tvec3<int>;			using ivec4 = tvec4<int>;
using uvec2 = tvec2<uint>;			using uvec3 = tvec3<uint>;			using uvec4 = tvec4<uint>;
using llvec2 = tvec2<int64_t>;		using llvec3 = tvec3<int64_t>;		using llvec4 = tvec4<int64_t>;
using ullvec2 = tvec2<uint64_t>;	using ullvec3 = tvec3<uint64_t>;	using ullvec4 = tvec4<uint64_t>;

// half-precision float and conversion
#ifdef __GXMATH_HALF__
__forceinline vec2	htof( const half2& h ){ return vec2(htof(h.x),htof(h.y)); }
__forceinline vec3	htof( const half3& h ){ return vec3(htof(h.x),htof(h.y),htof(h.z)); }
__forceinline vec4	htof( const half4& h ){ return vec4(htof(h.x),htof(h.y),htof(h.z),htof(h.w)); }
__forceinline half2	ftoh( const vec2& v ){ return half2{ftoh(v.x),ftoh(v.y)}; }
__forceinline half3	ftoh( const vec3& v ){ return half3{ftoh(v.x),ftoh(v.y),ftoh(v.z)}; }
__forceinline half4 ftoh( const vec4& v ){ return half4{ftoh(v.x),ftoh(v.y),ftoh(v.z),ftoh(v.w)}; }
#endif

// concepts for vectors
template <template<typename...>class T> concept tvec = same_template<T,tvec2>||same_template<T,tvec3>||same_template<T,tvec4>;

// common matrix interface
template <template<typename...>class I, template<typename...>class W, class T, int _D>
struct imat
{
	using M = I<T>;
	using V = W<T>;
	static constexpr int D = _D;
	static constexpr int N = _D*_D;

	using value_type=T; using iterator=T*; using const_iterator=const iterator; using reference=T&; using const_reference=const T&; using size_type=size_t;
	static constexpr size_t size(){ return N; }
	constexpr iterator begin() const { return iterator(this); } constexpr iterator end() const { return iterator(this)+N; }
	__forceinline operator T*(){ return (T*)this; } __forceinline operator const T*() const { return (T*)this; }
	__forceinline T* data(){ return (T*)this; } __forceinline const T* data() const { return (T*)this; }

	// vector access
	__forceinline V& operator[]( ptrdiff_t col ){ return ((V*)this)[col]; }
	__forceinline V operator[]( ptrdiff_t col ) const { return ((V*)this)[col]; }
	__forceinline V rvec( ptrdiff_t row ) const { V v; for(int k=0;k<D;k++) v[k]=iterator(this)[k*D+row]; return v; }

	// assignment operators
	__forceinline M& operator=( const M& m ){ memcpy(this,&m,sizeof(M)); return reinterpret_cast<M&>(*this); }

	// comparison operators
	__forceinline bool operator==( const M& m ) const { iterator f(this), g(&m); for(int k=0;k<N;k++) if(std::abs(f[k]-g[k])>precision<T>::value()) return false; return true; }
	__forceinline bool operator!=( const M& m ) const { return !(operator==(m)); }

	// unary operators
	__forceinline M& operator+(){ return reinterpret_cast<M&>(*this); }
	__forceinline const M& operator+() const { return reinterpret_cast<M&>(*this); }
	__forceinline M operator-() const { M m; iterator f=(T*)this, g=(T*)&m; for( int k=0;k<N;k++) g[k]=-f[k]; return m; }

	// binary operators
	__forceinline M operator+( const M& m ) const { M t=*((M*)this); t+=m; return reinterpret_cast<M&>(t); }
	__forceinline M operator-( const M& m ) const { M t=*((M*)this); t-=m; return reinterpret_cast<M&>(t); }
	__forceinline M operator*( const M& m ) const { M t=*((M*)this); t*=m; return reinterpret_cast<M&>(t); }
	__forceinline M operator/( T f ) const { M t=*this; t/=f; return reinterpret_cast<M&>(t); }

	// compound assignment operators
	__forceinline M& operator+=( const M& m ){ for(int k=0;k<D;k++) (*this)[k]+=m[k]; return reinterpret_cast<M&>(*this); }
	__forceinline M& operator-=( const M& m ){ for(int k=0;k<D;k++) (*this)[k]-=m[k]; return reinterpret_cast<M&>(*this); }
	__forceinline M& operator*=( const M& m ){ M t=*((M*)this); for(int k=0;k<D;k++) ((V*)this)[k]=t*((V*)&m)[k]; return reinterpret_cast<M&>(*this); }
	__forceinline M& operator/=( T f ){ for(int k=0;k<N;k++) iterator(this)[k]/=f; return reinterpret_cast<M&>(*this); }

	// vector arithmetic
	__forceinline V operator*( const V& v ) const { V r=((V*)this)[0]*v.x+((V*)this)[1]*v.y; for(int k=2;k<D;k++) r+=((V*)this)[k]*v[k]; return r; }
	friend __forceinline V operator*( const V& v, const M& m ){ V r; for(int k=0;k<D;k++) r[k]=v.dot(m.cvec[k]); return r; } // left mult: vec*mat
	friend __forceinline V mul( const V& v, const M& m ){ V r; for(int k=0;k<D;k++) r[k]=v.dot(m.cvec[k]); return r; } // left mult: vec*mat

	// global matrix functions
	friend __forceinline M transpose( const M& m ){ return m.transpose(); }
	friend __forceinline V mul( const M& m, const V& v ){ return m*v; }

	// fundamental matrix functions
	__forceinline static M identity(){ return M(); }
	__forceinline V diag() const { V v; for(int k=0;k<D;k++) v[k]=iterator(this)[k*D+k]; return v; }
	__forceinline T trace() const { V d=diag(); T f=0; for(int k=0;k<D;k++) f+=d[k]; return f; }
};

// common matrix implemenetations
#define __default_matrix_impl(M,W,dim) \
	static constexpr int D=dim; static constexpr int N=dim*dim;\
	using B=imat<M,W,T,dim>; using V=W<T>; using value_type=T; using iterator=T*; using const_iterator=const iterator; using reference=T&; using const_reference=const T&; using size_type=size_t;\
	using B::operator=; using B::operator==; using B::operator!=; using B::operator+; using B::operator-; using B::operator*; using B::operator/; using B::operator+=; using B::operator-=; using B::operator*=; using B::operator/=;\
	using B::size; using B::begin; using B::end; using B::data; using B::rvec; using B::identity; using B::diag; using B::trace;\
	__forceinline M( const M& m )=default;

// matrix like GLSL: column-major memory layout, row-major element indexing/operations
template <floating_point T> struct tmat2 : public imat<tmat2,tvec2,T,2>
{
	__default_matrix_impl(tmat2,tvec2,2);
	union { struct {T _00,_10,_01,_11;}; V cvec[2]; };

	// constructors
	__forceinline tmat2(){ _10=_01=0;_00=_11=T(1); }
	template <class X> requires std::convertible_to<X,T> __forceinline tmat2( X f ){ _10=_01=0;_00=_11=T(f); }
	template <class X, class Y, class Z, class W>
	__forceinline tmat2( X f00, Y f10, Z f01, W f11 ){ _00=T(f00);_10=T(f10);_01=T(f01);_11=T(f11); }
	__forceinline tmat2( V c0, V c1 ){ cvec[0]=c0; cvec[1]=c1; }

	// transpose
	__forceinline tmat2 transpose() const { return {_00,_01,_10,_11}; }

	// determinant/inverse
	__forceinline double det() const { return double(_00)*double(_11)-double(_01)*double(_10); }
	__forceinline tmat2 inverse() const { double d=1/det(); return {_11*d,-_10*d,-_01*d,_00*d}; }

	// row-major transformations in 2D Cartesian coordinate system
	__forceinline static tmat2 scale( const V& v ){ return {v.x,0,0,v.y}; }
	__forceinline static tmat2 scale( T x, T y ){ return {x,0,0,y}; }
	__forceinline static tmat2 rotate( T theta ){ T c=T(cos(theta)), s=T(sin(theta)); return {c,s,-s,c}; }
};

// matrix like GLSL: column-major memory layout, row-major element indexing/operations
template <floating_point T> struct tmat3 : public imat<tmat3,tvec3,T,3>
{
	__default_matrix_impl(tmat3,tvec3,3);
	using V2=tvec2<T>; using M2=tmat2<T>;
	union { struct {T _00,_10,_20,_01,_11,_21,_02,_12,_22;}; V cvec[3]; };

	// constructors
	__forceinline tmat3(){ _10=_20=_01=_21=_02=_12=0;_00=_11=_22=T(1); }
	template <class X> requires std::convertible_to<X,T> __forceinline tmat3( X f ){ _10=_20=_01=_21=_02=_12=0;_00=_11=_22=T(f); }
	template <class X0, class X1, class X2, class Y0, class Y1, class Y2, class Z0, class Z1, class Z2>
	__forceinline tmat3( X0 f00, X1 f10, X2 f20, Y0 f01, Y1 f11, Y2 f21, Z0 f02, Z1 f12, Z2 f22 ){ _00=T(f00);_10=T(f10);_20=T(f20);_01=T(f01);_11=T(f11);_21=T(f21);_02=T(f02);_12=T(f12);_22=T(f22); }
	__forceinline tmat3( V c0, V c1, V c2 ){ cvec[0]=c0; cvec[1]=c1; cvec[2]=c2; } // colmun-major initialization

	// casting operators
	__forceinline operator tmat2<T>() const { return { _00,_10,_01,_11 }; }

	// transpose
	__forceinline tmat3 transpose() const { return {_00,_01,_02,_10,_11,_12,_20,_21,_22 }; }
	
	// determinant/inverse
	__forceinline double det() const { return _00*double(_11*_22-_12*_21) + _01*double(_12*_20-_10*_22) + _02*double(_10*_21-_11*_20); }
	__forceinline tmat3 inverse() const { double d=1/det(); return { (_11*_22-_21*_12)*d,(_12*_20-_10*_22)*d,(_10*_21-_20*_11)*d,(_02*_21-_01*_22)*d,(_00*_22-_02*_20)*d,(_20*_01-_00*_21)*d,(_01*_12-_02*_11)*d,(_10*_02-_00*_12)*d,(_00*_11-_10*_01)*d }; }

	// row-major 2D transformations in 3D homogeneous coordinate system
	template <class X, class Y > __forceinline static tmat3 translate( X x, Y y ){ tmat3 m; m._02=T(x); m._12=T(y); return m; }
	__forceinline static tmat3 translate( const V2& v ){ return translate(v.x,v.y); }
	template <class X, class Y > __forceinline static tmat3 scale( X x, Y y ){ tmat3 m; m._00=T(x); m._11=T(y); return m; }
	__forceinline static tmat3 scale( const V2& v ){ return scale(v.x,v.y); }
	template <class X> __forceinline static tmat3 rotate( X theta ){ tmat3 m; m._00=m._11=T(cos(double(theta)));m._01=-T(sin(double(theta))); m._10=-m._01; return m; }

	// row-major 3D transformations in 3D Cartesian coordinate system
	__forceinline static tmat3 scale( const V& v ){ return scale(v.x,v.y,v.z); }
	template <class X, class Y, class Z> __forceinline static tmat3 scale( X x, Y y, Z z ){ tmat3 m; m._00=T(x); m._11=T(y); m._22=T(z); return m; }
	__forceinline static tmat3 rotate( const V& from, const V& to );
	__forceinline static tmat3 rotate( const V& axis, T angle );
};

template <floating_point T>
__noinline tmat3<T> tmat3<T>::rotate( const V& from, const V& to )
{
	vec3 f=from.normalize(), t=to.normalize();
	T d=T(f.dot(t)); if(d>T(0.999999)) return tmat3<T>(); else if(d<T(-0.999999)) return scale(-1,-1,-1);
	V n=f.cross(t); T l=T(n.length());
	return rotate(n/l, asin(l));
}

template <floating_point T>
__noinline tmat3<T> tmat3<T>::rotate( const V& axis, T angle )
{
	V a = axis.normalize();
	double c=cos(double(angle)), s=sin(double(angle)), x=double(a.x), y=double(a.y), z=double(a.z);
	tmat3<T> m;
	m._00=T(x*x*(1-c)+c);	m._01=T(x*y*(1-c)-z*s);	m._02=T(x*z*(1-c)+y*s);
	m._10=T(x*y*(1-c)+z*s);	m._11=T(y*y*(1-c)+c);	m._12=T(y*z*(1-c)-x*s);
	m._20=T(x*z*(1-c)-y*s);	m._21=T(y*z*(1-c)+x*s);	m._22=T(z*z*(1-c)+c);
	return m;
}

// matrix like GLSL: column-major memory layout, row-major element indexing/operations
template <floating_point T> struct tmat4 : public imat<tmat4,tvec4,T,4>
{
	__default_matrix_impl(tmat4,tvec4,4);
	using V2=tvec2<T>; using M2=tmat2<T>; using V3=tvec3<T>; using M3=tmat3<T>;
	union { struct {T _00,_10,_20,_30,_01,_11,_21,_31,_02,_12,_22,_32,_03,_13,_23,_33;}; V cvec[4]; };

	// constructors
	__forceinline tmat4(){ _10=_20=_30=_01=_21=_31=_02=_12=_32=_03=_13=_23=0;_00=_11=_22=_33=T(1); }
	template <class X> requires std::convertible_to<X,T> __forceinline tmat4( X f ){ _10=_20=_30=_01=_21=_31=_02=_12=_32=_03=_13=_23=0;_00=_11=_22=_33=T(f); }
	__forceinline tmat4( T f00, T f10, T f20, T f30, T f01, T f11, T f21, T f31, T f02, T f12, T f22, T f32, T f03, T f13, T f23, T f33 ){ _00=f00;_10=f10;_20=f20;_30=f30;_01=f01;_11=f11;_21=f21;_31=f31;_02=f02;_12=f12;_22=f22;_32=f32;_03=f03;_13=f13;_23=f23;_33=f33; }
	__forceinline tmat4( const M3& m ){ _00=m._00;_01=m._01;_02=m._02;_10=m._10;_11=m._11;_12=m._12;_20=m._20;_21=m._21;_22=m._22; _03=_13=_23=_30=_31=_32=0;_33=T(1); }
	__forceinline tmat4( V c0, V c1, V c2, V c3 ){ cvec[0]=c0;cvec[1]=c1;cvec[2]=c2;cvec[3]=c3; }
	__forceinline tmat4( V3 c0, V3 c1, V3 c2, V3 c3 ){ cvec[0]=V(c0,0);cvec[1]=V(c1,0);cvec[2]=V(c2,0);cvec[3]=V(c3,1); }
	__forceinline tmat4( V3 c0, V3 c1, V3 c2 ){ cvec[0]=V(c0,0);cvec[1]=V(c1,0);cvec[2]=V(c2,0);cvec[3]=V(0,0,0,1); }

	// assignment operator
	__forceinline tmat4& operator=( const M3& m ){ cvec[0]=vec4(m.cvec[0],0); cvec[1]=vec4(m.cvec[1],0); cvec[2]=vec4(m.cvec[2],0); cvec[3]=vec4(0,0,0,1); return *this; }

	// casting operators
	__forceinline operator M2() const { return {_00,_10,_01,_11}; }
	__forceinline operator M3() const { return {_00,_10,_20,_01,_11,_21,_02,_12,_22}; }

	// multiplication operators
	__forceinline V3 operator*( const V3& v ) const	{ return cvec[0].xyz*v.x+cvec[1].xyz*v.y+cvec[2].xyz*v.z+cvec[3].xyz; }
	friend __forceinline vec3 mul( const tmat4& m, const V3& v ){ return m*v; }

	// transpose
	__forceinline tmat4 transpose() const { return {_00,_01,_02,_03,_10,_11,_12,_13,_20,_21,_22,_23,_30,_31,_32,_33}; }

	// determinant/inverse
	__forceinline double det() const { return dvec4(_03,_13,_23,_33).dot(_xdet()); }
	tmat4 inverse() const;

	// static row-major transformations
	__forceinline static tmat4 translate( const V3& v ){ return translate(v.x,v.y,v.z); }
	__forceinline static tmat4 translate( T x, T y, T z ){ tmat4 m; m._03=x; m._13=y; m._23=z; return m; }
	__forceinline static tmat4 scale( const V3& v ){ return scale(v.x,v.y,v.z); }
	__forceinline static tmat4 scale( T x, T y, T z ){ tmat4 m; m._00=x; m._11=y; m._22=z; return m; }
	__forceinline static tmat4 shear( const V2& yz, const V2& zx, const V2& xy ){ tmat4 m; m._01=yz.x; m._02=yz.y; m._10=zx.y; m._12=zx.x; m._20=xy.x; m._21=xy.y; return m; }
	__forceinline static tmat4 rotate( const V3& from, const V3& to ){ return M3::rotate(from,to); }
	__forceinline static tmat4 rotate( const V3& axis, T angle ){ return M3::rotate(axis,angle); }
	
	__forceinline static tmat4 look_at( const V3& eye, const V3& center, const V3& up ){ return look_to(eye,center-eye,up); }
	__forceinline static tmat4 look_at_inverse( const V3& eye, const V3& center, const V3& up ){ look_to_inverse(eye,center-eye,up); }
	__forceinline static tmat4 look_to( const V3& eye, const V3& dir, const V3& up ){ V3 n=-dir.normalize(), u=(up.cross(n)).normalize(), v=n.cross(u); tmat4 m; m._00=u.x;m._01=u.y;m._02=u.z; m._03=-u.dot(eye); m._10=v.x;m._11=v.y;m._12=v.z; m._13=-v.dot(eye); m._20=n.x;m._21=n.y;m._22=n.z; m._23=-n.dot(eye); return m; }
	__forceinline static tmat4 look_to_inverse( const V3& eye, const V3& dir, const V3& up ){ V3 n=-dir.normalize(), u=(up.cross(n)).normalize(), v=n.cross(u); return tmat4(u,v,n,eye); }
	__forceinline static tmat4 perspective( T fovy, T aspect, T dn, T df ){ if(fovy>pi_v<T>) fovy*=pi_v<T>/T(180.0); /* autofix for fov in degrees */ tmat4 m; m._11=T(1.0/tanf(fovy*0.5)); m._00=m._11/aspect; m._22=(dn+df)/(dn-df); m._23=2.0f*dn*df/(dn-df); m._32=T(-1.0); m._33=0; return m; }
	__forceinline static tmat4 perspective_off_center( T left, T right, T top, T bottom, T dn, T df ){ tmat4 m; m._00=T(2.0)*dn/(right-left); m._11=T(2.0)*dn/(top-bottom); m._02=(right+left)/(right-left); m._12=(top+bottom)/(top-bottom); m._22=(dn+df)/(dn-df); m._23=T(2.0)*dn*df/(dn-df); m._32=T(-1.0); m._33=0; return m; }
	__forceinline static tmat4 ortho( T width, T height, T dn, T df ){ tmat4 m; m._00=T(2.0)/width; m._11=T(2.0)/height; m._22=2.0f/(dn-df); m._23=(dn+df)/(dn-df); return m; }
	__forceinline static tmat4 ortho_off_center( T left, T right, T top, T bottom, T dn, T df ){ tmat4 m=ortho( right-left, top-bottom, dn, df ); m._03=(left+right)/(left-right); m._13=(bottom+top)/(bottom-top); return m; }

	// viewport, lookat inferred from view matrix
	__forceinline static tmat4 viewport( int width, int height ){ tmat4 m; m._00=width*T(0.5); m._11=-height*T(0.5); m._03=width*T(0.5); m._13=height*T(0.5); return m; }
	__forceinline V3 look_at_eye() const { const V3 u=rvec(0).xyz,v=rvec(1).xyz,n=rvec(2).xyz,uv=u.cross(v),vn=v.cross(n),nu=n.cross(u); return (vn*_03+nu*_13+uv*_23)/(-u.dot(vn)); }
	__forceinline tmat4 look_at_inverse() const { tmat4 m; m._00=_00; m._01=_10; m._02=_20; m._10=_01; m._11=_11; m._12=_21; m._20=_02; m._21=_12; m._22=_22; m.cvec[3].xyz=look_at_eye(); return m; }

	// Canonical view volume in DirectX: [-1,1]^2*[0,1]: diffes only in _22 and _23
	__forceinline static tmat4 perspective_dx( T fovy, T aspect, T dn, T df ){ tmat4 m=perspective( fovy, aspect, dn, df ); m._22=df/(dn-df); m._23*=T(0.5); return m; } // equivalent to D3DXMatrixPerspectiveFovRH()
	__forceinline static tmat4 perspective_off_center_dx( T left, T right, T top, T bottom, T dn, T df ){ tmat4 m=perspective_off_center( left, right, top, bottom, dn, df ); m._22=df/(dn-df); m._23*=T(0.5); return m; }
	__forceinline static tmat4 ortho_dx( T width, T height, T dn, T df ){ tmat4 m=ortho( width, height, dn, df ); m._22*=T(0.5); m._23=dn/(dn-df); return m; }
	__forceinline static tmat4 ortho_off_center_dx( T left, T right, T top, T bottom, T dn, T df ){ tmat4 m=ortho_off_center( left, right, top, bottom, dn, df ); m._22*=T(0.5); m._23=dn/(dn-df); return m; }

protected:
	dvec4 _xdet() const // support function for det() and inverse()
	{
		return {double(_30*_21-_20*_31)*_12+double(_10*_31-_30*_11)*_22+double(_20*_11-_10*_21)*_32,
				double(_20*_31-_30*_21)*_02+double(_30*_01-_00*_31)*_22+double(_00*_21-_20*_01)*_32,
				double(_30*_11-_10*_31)*_02+double(_00*_31-_30*_01)*_12+double(_10*_01-_00*_11)*_32,
				double(_10*_21-_20*_11)*_02+double(_20*_01-_00*_21)*_12+double(_00*_11-_10*_01)*_22};
	}
};

template <floating_point T>
__noinline tmat4<T> tmat4<T>::inverse() const
{
	dvec4 x=_xdet();
	tmat4<double> d; // work in higher precision
	d._00=double(_21*_32-_31*_22)*_13 + double(_31*_12-_11*_32)*_23 + double(_11*_22-_21*_12)*_33;
	d._01=double(_31*_22-_21*_32)*_03 + double(_01*_32-_31*_02)*_23 + double(_21*_02-_01*_22)*_33;
	d._02=double(_11*_32-_31*_12)*_03 + double(_31*_02-_01*_32)*_13 + double(_01*_12-_11*_02)*_33;
	d._03=double(_21*_12-_11*_22)*_03 + double(_01*_22-_21*_02)*_13 + double(_11*_02-_01*_12)*_23;
	d._10=double(_30*_22-_20*_32)*_13 + double(_10*_32-_30*_12)*_23 + double(_20*_12-_10*_22)*_33;
	d._11=double(_20*_32-_30*_22)*_03 + double(_30*_02-_00*_32)*_23 + double(_00*_22-_20*_02)*_33;
	d._12=double(_30*_12-_10*_32)*_03 + double(_00*_32-_30*_02)*_13 + double(_10*_02-_00*_12)*_33;
	d._13=double(_10*_22-_20*_12)*_03 + double(_20*_02-_00*_22)*_13 + double(_00*_12-_10*_02)*_23;
	d._20=double(_20*_31-_30*_21)*_13 + double(_30*_11-_10*_31)*_23 + double(_10*_21-_20*_11)*_33;
	d._21=double(_30*_21-_20*_31)*_03 + double(_00*_31-_30*_01)*_23 + double(_20*_01-_00*_21)*_33;
	d._22=double(_10*_31-_30*_11)*_03 + double(_30*_01-_00*_31)*_13 + double(_00*_11-_10*_01)*_33;
	d._23=double(_20*_11-_10*_21)*_03 + double(_00*_21-_20*_01)*_13 + double(_10*_01-_00*_11)*_23;
	d._30=x.x; d._31=x.y; d._32=x.z; d._33=x.w;
	T s=T(1)/vec4(_03,_13,_23,_33).dot(x);
	tmat4<T> m; for( int k=0;k<N;k++) (&m._00)[k]=(&d._00)[k]*s;
	return m;
}

// type definitions and size check
using mat2	= tmat2<float>;		using mat3 = tmat3<float>;		using mat4 = tmat4<float>;
using dmat2	= tmat2<double>;	using dmat3 = tmat3<double>;	using dmat4 = tmat4<double>;

// concepts for matrices
template <template<typename...>class T> concept tmat = same_template<T,tmat2>||same_template<T,tmat3>||same_template<T,tmat4>;

// matrix types to string
inline const char* ftoa( const mat2& m ){ const auto* f=&m._00;static const char* fmt="%g %g %g %g";size_t size=size_t(snprintf(0,0,fmt,f[0],f[1],f[2],f[3]));char* buff=__strbuf(size); snprintf(buff,size+1,fmt,f[0],f[1],f[2],f[3]);return buff;}
inline const char* ftoa( const mat3& m ){ const auto* f=&m._00;static const char* fmt="%g %g %g %g %g %g %g %g %g";size_t size=size_t(snprintf(0,0,fmt,f[0],f[1],f[2],f[3],f[4],f[5],f[6],f[7],f[8]));char* buff=__strbuf(size); snprintf(buff,size+1,fmt,f[0],f[1],f[2],f[3],f[4],f[5],f[6],f[7],f[8]);return buff;}
inline const char* ftoa( const mat4& m ){ const auto* f=&m._00;static const char* fmt="%g %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g";size_t size=size_t(snprintf(0,0,fmt,f[0],f[1],f[2],f[3],f[4],f[5],f[6],f[7],f[8],f[9],f[10],f[11],f[12],f[13],f[14],f[15]));char* buff=__strbuf(size); snprintf(buff,size+1,fmt,f[0],f[1],f[2],f[3],f[4],f[5],f[6],f[7],f[8],f[9],f[10],f[11],f[12],f[13],f[14],f[15]);return buff;}
inline const char* ftoa( const dmat2&m ){ const auto* f=&m._00;static const char* fmt="%g %g %g %g";size_t size=size_t(snprintf(0,0,fmt,f[0],f[1],f[2],f[3]));char* buff=__strbuf(size); snprintf(buff,size+1,fmt,f[0],f[1],f[2],f[3]);return buff;}
inline const char* ftoa( const dmat3&m ){ const auto* f=&m._00;static const char* fmt="%g %g %g %g %g %g %g %g %g";size_t size=size_t(snprintf(0,0,fmt,f[0],f[1],f[2],f[3],f[4],f[5],f[6],f[7],f[8]));char* buff=__strbuf(size); snprintf(buff,size+1,fmt,f[0],f[1],f[2],f[3],f[4],f[5],f[6],f[7],f[8]);return buff;}
inline const char* ftoa( const dmat4&m ){ const auto* f=&m._00;static const char* fmt="%g %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g";size_t size=size_t(snprintf(0,0,fmt,f[0],f[1],f[2],f[3],f[4],f[5],f[6],f[7],f[8],f[9],f[10],f[11],f[12],f[13],f[14],f[15]));char* buff=__strbuf(size); snprintf(buff,size+1,fmt,f[0],f[1],f[2],f[3],f[4],f[5],f[6],f[7],f[8],f[9],f[10],f[11],f[12],f[13],f[14],f[15]);return buff;}

// global functions for length/normalize/dot/cross
#define __tvec__ template <template<typename...> class V, floating_point T> requires tvec<V> __forceinline
#define __tmat__ template <template<typename...> class V, floating_point T> requires tmat<V> __forceinline
__tvec__ T length( const V<T>& v ){ return v.length(); }
__tvec__ T length2( const V<T>& v ){ return v.length2(); }
__tvec__ V<T> normalize( const V<T>& v ){ return v.normalize(); }
__tvec__ T dot( const V<T>& v0, const V<T>& v1){ return v0.dot(v1); }
__tvec__ T distance( const V<T>& a, const V<T>& b ){ return (a-b).length(); }
template <floating_point T> __forceinline tmat2<T> outerProduct( const tvec2<T>& v0, const tvec2<T>& v1 ){ tmat2<T> m; m._00=v0.x*v1.x; m._01=v0.x*v1.y; m._10=v0.y*v1.x; m._11=v0.y*v1.y; return m; }
template <floating_point T> __forceinline tmat3<T> outerProduct( const tvec3<T>& v0, const tvec3<T>& v1 ){ tmat3<T> m; m._00=v0.x*v1.x; m._01=v0.x*v1.y; m._02=v0.x*v1.z; m._10=v0.y*v1.x; m._11=v0.y*v1.y; m._12=v0.y*v1.z; m._20=v0.z*v1.x; m._21=v0.z*v1.y; m._22=v0.z*v1.z; return m; }
template <floating_point T> __forceinline tmat4<T> outerProduct( const tvec4<T>& v0, const tvec4<T>& v1 ){ tmat4<T> m; m._00=v0.x*v1.x; m._01=v0.x*v1.y; m._02=v0.x*v1.z; m._03=v0.x*v1.w; m._10=v0.y*v1.x; m._11=v0.y*v1.y; m._12=v0.y*v1.z; m._13=v0.y*v1.w; m._20=v0.z*v1.x; m._21=v0.z*v1.y; m._22=v0.z*v1.z; m._23=v0.z*v1.w; m._30=v0.w*v1.x; m._31=v0.w*v1.y; m._32=v0.w*v1.z; m._33=v0.w*v1.w; return m; }

// general math utility functions
template <class X, class T=float> __forceinline T radians( X f ){ return f*pi_v<T>/T(180.0); }
template <class X, class T=float> __forceinline T degrees( X f ){ return f*T(180.0)/pi_v<T>; }
template <class X, class T=float> __forceinline T round( X f, int digits ){ T m=T(pow(10.0,digits)); return round(f*m)/m; }
template <class X, class T=float> __forceinline T triangle_area( tvec2<X> a, tvec2<X> b, tvec2<X> c ){ return T(abs(a.x*b.y+b.x*c.y+c.x*a.y-a.x*c.y-c.x*b.y-b.x*a.y))*T(0.5); }
template <class T> __forceinline tvec2<T> minmax( const tvec2<T>& a, const tvec2<T>& b ){ return tvec2<T>(a.x<b.x?a.x:b.x,a.y>b.y?a.y:b.y); }
template <std::integral T> __forceinline bool ispot( T i ){ return (uint(i)&(uint(i)-1))==0; } // http://en.wikipedia.org/wiki/Power_of_two
template <std::integral T> __forceinline uint nextpot( T n ){ int m=int(n)-1; for( uint k=1; k<uint(sizeof(int))*8; k<<=1 ) m=m|m>>k; return m+1; }	// closest (equal or larger) power-of-two
template <std::integral T> __forceinline uint nextsqrt( T n ){ return uint(ceil(sqrt(double(n)))+0.001); } // root of closest (equal or larger) square
template <std::integral T> __forceinline uint nextsquare( T n ){ uint r=nextsqrt(n); return r*r; } // closest (equal or larger) square
template <std::integral T> __forceinline uint miplevels( T width, T height=1 ){ uint l=0; uint s=width>height?width:height; while(s){s=s>>1;l++;} return l; }
__forceinline uint bitswap( uint n ){ n=((n&0x55555555)<<1)|((n&0xaaaaaaaa)>>1); n=((n&0x33333333)<<2)|((n&0xcccccccc)>>2); n=((n&0x0f0f0f0f)<<4)|((n&0xf0f0f0f0)>>4); n=((n&0x00ff00ff)<<8)|((n&0xff00ff00)>>8); return (n<<16)|(n>>16); }
__forceinline float rsqrt( float x ){ float y=0.5f*x; int i=*(int*)&x; i=0x5F375A86-(i>>1); x=*(float*)&i; x=x*(1.5f-y*x*x); x=x*(1.5f-y*x*x); return x; }						// Quake3's Fast InvSqrt(): 1/sqrt(x): magic number changed from 0x5f3759df to 0x5F375A86 for more accuracy; 2 iteration has quite good accuracy
__forceinline double rsqrt( double x ){ double y=0.5*x; int64_t i=*(int64_t*)&x; i=0x5FE6EB50C7B537A9-(i>>1); x=*(double*)&i; x=x*(1.5-y*x*x); x=x*(1.5-y*x*x); return x; }		// Quake3's Fast InvSqrt(): 1/sqrt(x): 64-bit magic number (0x5FE6EB50C7B537A9) used; 2 iteration has quite good accuracy

// viewport helpers
__forceinline ivec4 effective_viewport( int width, int height, double aspect_to_keep ){ int w=int(height*aspect_to_keep), h=int(width/aspect_to_keep); if((width*h)==(height*w)) return ivec4{0,0,width,height}; return width>w?ivec4{(width-w)/2,0,w,height}:ivec4{0,(height-h)/2,width,h}; }
__forceinline ivec4 effective_viewport( int width, int height, int width_to_keep, int height_to_keep ){ return (width*height_to_keep)==(height*width_to_keep)?ivec4{0,0,width,height}:effective_viewport(width,height,width_to_keep/double(height_to_keep)); }
__forceinline ivec4 effective_viewport( int width, int height, ivec2 aspect_to_keep ){ return effective_viewport(width,height,aspect_to_keep.x,aspect_to_keep.y); }
__forceinline ivec4 effective_viewport( ivec2 size, double aspect_to_keep ){ return effective_viewport(size.x,size.y,aspect_to_keep); }
__forceinline ivec4 effective_viewport( ivec2 size, int width_to_keep, int height_to_keep ){ return effective_viewport(size.x,size.y,width_to_keep,height_to_keep); }
__forceinline ivec4 effective_viewport( ivec2 size, ivec2 aspect_to_keep ){ return effective_viewport(size.x,size.y,aspect_to_keep.x,aspect_to_keep.y); }

// {GLSL|HLSL}-like shader intrinsic functions
__forceinline float fract( float f ){ return float(f-floor(f)); }
__forceinline float saturate( float f ){ return f<0.0f?0.0f:f>1.0f?1.0f:f; }
__forceinline int sign( int f ){ return f>0?1:f<0?-1:0; }
__forceinline float sign( float f ){ return f>0.0f?1.0f:f<0.0f?-1.0f:0; }
__forceinline float sigmoid( float x ){ return 1.0f/(1.0f+exp(-x)); }
template <class T,class N,class X> T clamp( T v, N vmin, X vmax ){ return v<T(vmin)?T(vmin):v>T(vmax)?T(vmax):v; }
__forceinline float smoothstep( float edge0, float edge1, float t ){	t=saturate((t-edge0)/(edge1-edge0)); return t*t*(3-2*t); } // C1-continuity
__forceinline float smoothstep( float t ){ t=saturate(t); return t*t*(3-2*t); } // C1-continuity
__forceinline float smootherstep( float edge0, float edge1, float t ){	t=saturate((t-edge0)/(edge1-edge0)); return t*t*t*(6.0f*t*t-15.0f*t+10.0f); } // C2-continuity (by Ken Perlin)
__forceinline float smootherstep( float t ){ t=saturate(t); return t*t*t*(6.0f*t*t-15.0f*t+10.0f); } // C2-continuity (by Ken Perlin)
template <floating_point T> __forceinline float  lerp( float v1, float v2, T t ){ return v1*(1.0f-float(t))+v2*float(t); }
template <floating_point T> __forceinline float  mix( float v1, float v2, T t ){ return v1*(1.0f-float(t))+v2*float(t); }
template <floating_point T> __forceinline double lerp( double v1, double v2, T t ){ return v1*(1.0-double(t))+v2*double(t); }
template <floating_point T> __forceinline double mix( double v1, double v2, T t ){ return v1*(1.0-double(t))+v2*double(t); }

// define simple vector wrappers
#define __fvec2(f) __forceinline vec2 f( const vec2& v ){ return {f(v.x),f(v.y)}; }
#define __fvec3(f) __forceinline vec3 f( const vec3& v ){ return {f(v.x),f(v.y),f(v.z)}; }
#define __fvec4(f) __forceinline vec4 f( const vec4& v ){ return {f(v.x),f(v.y),f(v.z),f(v.w)}; }
#define __fvec(f)	__fvec2(f) __fvec3(f) __fvec4(f)

__fvec(cos) __fvec(sin) __fvec(tan) __fvec(cosh) __fvec(sinh) __fvec(tanh)
__fvec(acos) __fvec(asin) __fvec(atan) __fvec(acosh) __fvec(asinh) __fvec(atanh)
__fvec(abs) __fvec(ceil) __fvec(fabs) __fvec(floor) __fvec(fract) __fvec(saturate) __fvec(sign) __fvec(exp)
__fvec(sigmoid) __fvec(smoothstep) __fvec(smootherstep)

__forceinline vec2 fma( vec2 a, vec2 b, vec2 c ){ return vec2(fma(a.x,b.x,c.x),fma(a.y,b.y,c.y)); }
__forceinline vec3 fma( vec3 a, vec3 b, vec3 c ){ return vec3(fma(a.x,b.x,c.x),fma(a.y,b.y,c.y),fma(a.z,b.z,c.z)); }
__forceinline vec4 fma( vec4 a, vec4 b, vec4 c ){ return vec4(fma(a.x,b.x,c.x),fma(a.y,b.y,c.y),fma(a.z,b.z,c.z),fma(a.w,b.w,c.w)); }
__tvec__ V<T> lerp( const V<T>& y1, const V<T>& y2, const V<T>& t ){ return y1*(-t+1.0f)+y2*t; }
__tvec__ V<T> mix( const V<T>& y1, const V<T>& y2, const V<T>& t ){ return y1*(-t+1.0f)+y2*t; }
template <template<typename...> class V, floating_point T, floating_point U> requires tvec<V>||tmat<V> V<T> lerp( const V<T>& y1, const V<T>& y2, U t ){ V<T> v; for(int k=0;k<V<T>::N;k++) v[k]=y1[k]*(-T(t)+1.0f)+y2[k]*T(t); return v; }
template <template<typename...> class V, floating_point T, floating_point U> requires tvec<V>||tmat<V> V<T> mix( const V<T>& y1, const V<T>& y2, U t ){ V<T> v; for(int k=0;k<V<T>::N;k++) v[k]=y1[k]*(-T(t)+1.0f)+y2[k]*T(t); return v; }
__forceinline vec3 reflect( const vec3& I, const vec3& N ){ return I-N*dot(I,N)*2.0f; }	// I: incident vector, N: normal
__forceinline vec3 refract( const vec3& I, const vec3& N, float eta /* = n0/n1 */ ){ float d = dot(I,N); float k = 1.0f-eta*eta*(1.0f-d*d); return k<0.0f?0.0f:(I*eta-N*(eta*d+sqrtf(k))); } // I: incident vector, N: normal
__tvec__ V<T> smoothstep( T edge0, T edge1, V<T> t ){	t=saturate((t-edge0)/(edge1-edge0)); return t*t*(3.0f-t*2.0f); }
__tvec__ V<T> smootherstep( T edge0, T edge1, V<T> t ){	t=saturate((t-edge0)/(edge1-edge0)); return t*t*t*(6.0f*t*t-15.0f*t+10.0f); }
// packing/unpacking/casting: https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_shading_language_packing.txt
__forceinline uint packUnorm2x16( vec2 v ){ ushort2 u={}; for(int k=0;k<2;k++) (&u.x)[k]=ushort(round(clamp(v[k],0.0f,1.0f)*65535.0f)); return reinterpret_cast<uint&>(u); }
__forceinline uint packSnorm2x16( vec2 v ){ short2 s={}; for(int k=0;k<2;k++) (&s.x)[k]=short(round(clamp(v[k],-1.0f,1.0f)*32767.0f)); return reinterpret_cast<uint&>(s); }
__forceinline uint packUnorm4x8( vec4 v ){ uchar4 u={}; for(int k=0;k<4;k++) (&u.x)[k]=uchar(round(clamp(v[k],0.0f,1.0f)*255.0f)); return reinterpret_cast<uint&>(u); }
__forceinline uint packSnorm4x8( vec4 v ){ int8_t s[4]={}; for(int k=0; k<4; k++) s[k]=char(round(clamp(v[k], -1.0f, 1.0f)*127.0f)); return reinterpret_cast<uint&>(s); }
__forceinline vec2 unpackUnorm2x16( uint u ){ vec2 v={}; for(int k=0;k<2;k++) v[k]=((ushort*)&u)[k]/65535.0f; return v; }
__forceinline vec2 unpackSnorm2x16( uint u ){ vec2 v={}; for(int k=0;k<2;k++) v[k]=clamp(((short*)&u)[k]/32767.0f,-1.0f,1.0f); return v; }
__forceinline vec4 unpackUnorm4x8( uint u ){ vec4 v={}; for(int k=0;k<4;k++) v[k]=((uchar*)&u)[k]/255.0f; return v; }
__forceinline vec4 unpackSnorm4x8( uint u ){ vec4 v={}; for(int k=0;k<4;k++) v[k]=clamp(((char*)&u)[k]/127.0f,-1.0f,1.0f); return v; }
__forceinline uint  packUint4x8( uint4 v ){ return (v.x&0xff)+((v.y&0xff)<<8)+((v.z&0xff)<<16)+((v.w&0xff)<<24); }
__forceinline uvec4 unpackUint4x8( uint u ){ return uvec4(u&0xff,(u>>8)&0xff,(u>>16)&0xff,(u>>24)&0xff); }
#ifdef __GXMATH_HALF__
__forceinline uint packHalf2x16( vec2 v ){ half2 h=ftoh(v); return reinterpret_cast<uint&>(h); }
__forceinline vec2 unpackHalf2x16( uint u ){ return htof(reinterpret_cast<half2&>(u)); }
#endif
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

// NaN/inf detection
template <floating_point T> __forceinline bool isnan( T* f, size_t n ){ for(size_t k=0;k<n;k++) if(isnan(f[k])) return true; return false; }
template <floating_point T> __forceinline bool isinf( T* f, size_t n ){ for(size_t k=0;k<n;k++) if(isinf(f[k])) return true; return false; }
__tvec__ bool isnan( V<T> v ){ return isnan(&v.x,v.size()); }
__tvec__ bool isinf( V<T> v ){ return isinf(&v.x,v.size()); }
__tmat__ bool isnan( V<T> m ){ return isnan((T*)&m,m.size()); }
__tmat__ bool isinf( V<T> m ){ return isinf((T*)&m,m.size()); }

// fundamental types for computer graphics
struct vertex { vec3 pos; vec3 norm; vec2 tex; }; // default vertex layout
struct bbox_t { vec3 m=3.402823466e+38F; uint __0; vec3 M=-3.402823466e+38F; uint __1; }; // bounding box in std140 layout; FLT_MAX = 3.402823466e+38F; __0, __1: padding

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

// undef macros
#undef __tvec__
#undef __tmat__
#undef __fvec2
#undef __fvec3
#undef __fvec4
#undef __fvec

//*************************************
#endif // __GX_MATH_H__
//*************************************