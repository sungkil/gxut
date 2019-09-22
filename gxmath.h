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
#ifndef __GX_MATH__
#define __GX_MATH__
//###################################################################
// COMMON HEADERS for GXUT
#ifndef __GXUT_COMMON__
#define __GXUT_COMMON__
// common macros
#ifndef _CRT_SECURE_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS
#endif
#ifndef _HAS_EXCEPTIONS
	#define _HAS_EXCEPTIONS 0
#endif
// SDKDDKVer
#if defined(__has_include) && __has_include(<SDKDDKVer.h>)
	#include <SDKDDKVer.h>
#endif
// prerequiste macros
#ifndef NOMINMAX
	#define NOMINMAX // suppress definition of min/max in <windows.h>
#endif
// C standard
#include <inttypes.h>	// defines int64_t, uint64_t
#include <math.h>
#include <stdarg.h>
#if !defined(GX_USE_STD_PRINTF) && (defined(_WIN32)||defined(_WIN64)) // Windows
	#define	printf	std_printf	// disable default printf
	#define	wprintf std_wprintf	// disable default wprintf
		#include <stdio.h>
		#include <wchar.h>
		#include <cstdio>
		#include <cwchar>
	#undef	printf
	#undef	wprintf
	// drop-in replacement of printf, where non-rex applications fallbacks to stdout
	int __cdecl printf( const char* fmt, ... );
	int __cdecl wprintf( const wchar_t* fmt, ... );
	#ifndef REX_FACTORY_IMPL
		#include <windows.h>
		inline int __cdecl printf( const char* fmt, ... ){ static int(*f)(const char*,va_list)=(int(*)(const char*,va_list)) GetProcAddress(GetModuleHandleW(nullptr),"mvprintf"); va_list a; va_start(a,fmt); int r=f?f(fmt,a):vprintf(fmt,a); va_end(a); return r; }
		inline int __cdecl wprintf( const wchar_t* fmt, ... ){ static int(*f)(const wchar_t*,va_list)=(int(*)(const wchar_t*,va_list)) GetProcAddress(GetModuleHandleW(nullptr),"mvwprintf"); va_list a; va_start(a,fmt); int r=f?f(fmt,a):vwprintf(fmt,a); va_end(a); return r; }
	#endif
#else
	#include <stdio.h>
	#include <wchar.h>
#endif
#include <stdlib.h>
#include <string.h>
// STL
#include <algorithm>
#include <array>
#include <map>
#include <set>
#include <string>
#include <vector>
// C++11
#if (__cplusplus>199711L) || (_MSC_VER>=1600/*VS2010*/)
	#include <type_traits>
	#include <unordered_map>
	#include <unordered_set>
#endif
// Windows
#if defined(_WIN32)||defined(_WIN64) // Windows
	#include <windows.h>
#endif
// platform-specific
#if defined _M_IX86
	#define GX_PLATFORM "x86"
#elif defined _M_X64
	#define GX_PLATFORM "x64"
#endif
#if defined(_MSC_VER) && !defined(__clang__) // Visual Studio with cl
	#pragma optimize( "gs", on )
	#pragma check_stack( off )
	#pragma strict_gs_check( off )
	#pragma float_control(except,off)
	#ifndef __noinline
		#define __noinline __declspec(noinline)
	#endif
	#pragma warning( disable: 4201 ) // nameless struct/union
	#pragma warning( disable: 4100 ) // unreferenced formal parameter
	#pragma warning( disable: 4244 ) // int to wchar_t, possible loss of data
	#pragma warning( disable: 4324 ) // alignment padding
	#pragma warning( disable: 4458 ) // hiding class member
#else // GCC or Clang
	#ifndef __noinline
		#define __noinline __attribute__((noinline))
	#endif
	#ifdef __GNUC__
		#ifndef __forceinline
			#define __forceinline inline __attribute__((__always_inline__))
		#endif
	#elif defined(__clang__)
		#pragma clang diagnostic ignored "-Wmissing-braces"					// ignore excessive warning for initialzer
		#pragma clang diagnostic ignored "-Wdelete-non-virtual-dtor"		// ignore non-virtual destructor
		#pragma clang diagnostic ignored "-Wunused-variable"				// supress warning for unused b0
		#pragma clang diagnostic ignored "-Wunused-command-line-argument"	// e.g., /Gm-, /QPar, /FpC; clang bugs show still warnings
		#pragma clang diagnostic ignored "-Wclang-cl-pch"					// clang bugs show still warnings
	#endif
#endif
// utility functions
template <class T> std::nullptr_t safe_free( T*& p ){if(p){free(p);p=nullptr;} return nullptr; }
template <class T> std::nullptr_t safe_delete( T*& p ){if(p){delete p;p=nullptr;} return nullptr; }
template <class T> std::nullptr_t safe_release( T*& p ){if(p){p->Release();p=nullptr;} return nullptr; }
// nocase base template
namespace nocase { template <class T> struct less {}; template <class T> struct equal_to {}; template <class T> struct hash {}; };
// user types
#define default_tarray(D)	static const int N=D; using value_type=T; using iterator=T*; using const_iterator=const iterator; using reference=T&; using const_reference=const T&; using size_type=size_t; __forceinline T& operator[]( ptrdiff_t i ){ return ((T*)this)[i]; } __forceinline const T& operator[]( ptrdiff_t i ) const { return ((T*)this)[i]; } __forceinline operator T*(){ return (T*)this; } __forceinline operator const T*() const { return (T*)this; } constexpr iterator begin() const { return iterator(this); } constexpr iterator end() const { return iterator(this)+N; } constexpr size_t size() const { return N; }
#define default_cmps(A)		__forceinline bool operator==( const A& rhs) const { return memcmp(this,&rhs,sizeof(*this))==0; } __forceinline bool operator!=( const A& rhs) const { return memcmp(this,&rhs,sizeof(*this))!=0; }
#define default_ctors(c)	__forceinline c()=default;__forceinline c(c&&)=default;__forceinline c(const c&)=default;__forceinline c(std::initializer_list<T> l){T* p=&x;for(auto i:l)(*p++)=i;}
#define default_assns(c)	__forceinline c& operator=(c&&)=default;__forceinline c& operator=(const c&)=default; __forceinline c& operator=(T a){ for(auto& it:*this) it=a; return *this; }
template <class T> struct tarray2  { default_tarray(2); union{struct{T x,y;};struct{T r,g;};}; default_ctors(tarray2); default_assns(tarray2); default_cmps(tarray2); };
template <class T> struct tarray3  { default_tarray(3); using V2=tarray2<T>; union{struct{T x,y,z;};struct{T r,g,b;};union{V2 xy,rg;};struct{T _x;union{V2 yz,gb;};};}; default_ctors(tarray3); default_assns(tarray3); default_cmps(tarray3); };
template <class T> struct tarray4  { default_tarray(4);	using V2=tarray2<T>; using V3=tarray3<T>; union{struct{T x,y,z,w;};struct{T r,g,b,a;};struct{union{V2 xy,rg;};union{V2 zw,ba;};};union{V3 xyz,rgb;};struct{T _x;union{V3 yzw,gba;V2 yz,gb;};};}; default_ctors(tarray4); default_assns(tarray4); default_cmps(tarray4); };
template <class T> struct tarray9  { default_tarray(9);	union{T a[9];struct{T _11,_12,_13,_21,_22,_23,_31,_32,_33;};}; default_cmps(tarray9); };
template <class T> struct tarray16 { default_tarray(16); union{T a[16];struct{T _11,_12,_13,_14,_21,_22,_23,_24,_31,_32,_33,_34,_41,_42,_43,_44;}; }; default_cmps(tarray16); };
using uint		= unsigned int;		using uchar		= unsigned char;	using ushort	= unsigned short;	using ulong = unsigned long;
using float2	= tarray2<float>;	using float3	= tarray3<float>;	using float4	= tarray4<float>;
using double2	= tarray2<double>;	using double3	= tarray3<double>;	using double4	= tarray4<double>;
using int2		= tarray2<int>;		using int3		= tarray3<int>;		using int4		= tarray4<int>;
using uint2		= tarray2<uint>;	using uint3		= tarray3<uint>;	using uint4		= tarray4<uint>;
using short2	= tarray2<short>;	using short3	= tarray3<short>;	using short4	= tarray4<short>;
using ushort2	= tarray2<ushort>;	using ushort3	= tarray3<ushort>;	using ushort4	= tarray4<ushort>;
using char2		= tarray2<char>;	using char3		= tarray3<char>;	using char4		= tarray4<char>;
using uchar2	= tarray2<uchar>;	using uchar3	= tarray3<uchar>;	using uchar4	= tarray4<uchar>;
using bool2		= tarray2<bool>;	using bool3		= tarray3<bool>;	using bool4		= tarray4<bool>;
using float9	= tarray9<float>;	using float16	= tarray16<float>;
using double9	= tarray9<double>;	using double16	= tarray16<double>;
#endif // __GXUT_COMMON__
//###################################################################

#include <limits.h>
#include <float.h>

#undef PI
using std::min;
using std::max;

template <class T=float> constexpr T PI = T(3.141592653589793);
template <class T,class N,class X> T clamp( T v, N vmin, X vmax ){ return v<T(vmin)?T(vmin):v>T(vmax)?T(vmax):v; }

//*************************************
// template type_traits helpers
template <class T> using enable_signed_t = typename std::enable_if_t<std::is_signed<T>::value,T>;
template <class T> using enable_float_t	 = typename std::enable_if_t<std::is_floating_point<T>::value,T>;
#define signed_memfun(U) template <class X=T, typename U=enable_signed_t<X>>
#define float_memfun(U)	 template <class X=T, typename U=enable_float_t<X>>

//*************************************
template <class T,template <class> class A=tarray2> struct tvec2
{
	default_tarray(2);

	union{struct{T x,y;};struct{T r,g;};A<T> xy,rg;};

	// constructor inheritance
	default_ctors(tvec2);
	__forceinline tvec2( const A<T>& v ){x=v.x;y=v.y;}
	__forceinline tvec2( T a ){x=y=a;}
	__forceinline tvec2( T a, T b ){x=a;y=b;}

	// assignment operators
	default_assns(tvec2);
	__forceinline tvec2& operator=( A<T>&& v ){ memmove(this,&v,sizeof(v)); return *this; }
	__forceinline tvec2& operator=( const A<T>& v ){ memcpy(this,&v,sizeof(v)); return *this; }

	// casting operators
	__forceinline operator A<T>&(){ return reinterpret_cast<A<T>&>(*this); }
	__forceinline operator const A<T>&() const { return reinterpret_cast<const A<T>&>(*this); }

	// comparison operators
	__forceinline bool operator!=(const tvec2& v) const { return !operator==(v); }
	__forceinline bool operator==(const tvec2& v) const { return x==v.x&&y==v.y; }

	// unary operators
	__forceinline tvec2& operator+(){ return *this; }
	__forceinline const tvec2& operator+() const { return *this; }
	signed_memfun(U) __forceinline tvec2<U> operator-() const { return tvec2(-x,-y); }

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
	float_memfun(U) __forceinline U length2() const { return T(x*x+y*y); }
	float_memfun(U) __forceinline U norm2() const { return T(x*x+y*y); }
	float_memfun(U) __forceinline U length() const { return T(sqrt(x*x+y*y)); }
	float_memfun(U) __forceinline U norm() const { return T(sqrt(x*x+y*y)); }
	float_memfun(U) __forceinline U dot( const tvec2& v ) const { return x*v.x+y*v.y; }
	float_memfun(U) __forceinline tvec2<U> normalize() const { return operator/(length()); }
};

template <class T,template <class> class A=tarray3> struct tvec3
{
	default_tarray(3);
	using V2 = tvec2<T>;

	union{struct{T x,y,z;};struct{T r,g,b;};union{V2 xy,rg;};struct{T _x;union{V2 yz,gb;};};A<T> xyz,rgb;};

	// constructor inheritance
	default_ctors(tvec3);
	__forceinline tvec3( const A<T>& v ){x=v.x;y=v.y;z=v.z;}
	__forceinline tvec3( T a ){x=y=z=a;}
	__forceinline tvec3( T a, T b, T c ){x=a;y=b;z=c;}
	__forceinline tvec3( const V2& v, T c ){x=v.x;y=v.y;z=c;}
	__forceinline tvec3( T a, const V2& v ){x=a;y=v.x;z=v.y;}

	// assignment operators
	default_assns(tvec3);
	__forceinline tvec3& operator=( A<T>&& v ){ memmove(this,&v,sizeof(v)); return *this; }
	__forceinline tvec3& operator=( const A<T>& v ){ memcpy(this,&v,sizeof(v)); return *this; }

	// casting operators
	__forceinline operator A<T>&(){ return reinterpret_cast<A<T>&>(*this); }
	__forceinline operator const A<T>&() const { return reinterpret_cast<const A<T>&>(*this); }

	// comparison operators
	__forceinline bool operator==(const tvec3& v) const { return x==v.x&&y==v.y&&z==v.z; }
	__forceinline bool operator!=(const tvec3& v) const { return !operator==(v); }

	// unary operators
	__forceinline tvec3& operator+(){ return *this; }
	__forceinline const tvec3& operator+() const { return *this; }
	signed_memfun(U) __forceinline tvec3<U> operator-() const { return tvec3(-x,-y,-z); }

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
	float_memfun(U) __forceinline U length2() const { return T(x*x+y*y+z*z); }
	float_memfun(U) __forceinline U norm2() const { return T(x*x+y*y+z*z); }
	float_memfun(U) __forceinline U length() const { return T(sqrt(x*x+y*y+z*z)); }
	float_memfun(U) __forceinline U norm() const { return T(sqrt(x*x+y*y+z*z)); }
	float_memfun(U) __forceinline U dot( const tvec3& v ) const { return x*v.x+y*v.y+z*v.z; }
	float_memfun(U) __forceinline tvec3<U> normalize() const { return operator/(length()); }

	// tvec3 only: cross product (floating-point only)
	float_memfun(U) __forceinline tvec3<U> cross( const tvec3& v ) const { return tvec3( y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x ); }
};

template <class T,template <class> class A=tarray4> struct tvec4
{
	default_tarray(4);

	using V2 = tvec2<T>;
	using V3 = tvec3<T>;

	union{struct{T x,y,z,w;};struct{T r,g,b,a;};struct{union{V2 xy,rg;};union{V2 zw,ba;};};union{V3 xyz,rgb;};struct{T _x;union{V3 yzw,gba;V2 yz,gb;};};A<T> xyzw,rgba;};

	// constructor inheritance
	default_ctors(tvec4);
	__forceinline tvec4( const A<T>& v ){x=v.x;y=v.y;z=v.z;w=v.w;}
	__forceinline tvec4( T a ){x=y=z=w=a;}
	__forceinline tvec4( T a, T b, T c, T d ){x=a;y=b;z=c;w=d;}
	__forceinline tvec4( T a, const V2& v, T d ){x=a;y=v.x;z=v.y;w=d;}
	__forceinline tvec4( T a, const V3& v ){x=a;y=v.x;z=v.y;w=v.z;}
	__forceinline tvec4( T a, T b, const V2& v ){x=a;y=b;z=v.x;w=v.y;}
	__forceinline tvec4( const V2& v, T c, T d ){x=v.x;y=v.y;z=c;w=d;}
	__forceinline tvec4( const V2& v1, const V2& v2 ){x=v1.x;y=v1.y;z=v2.x;w=v2.y;}
	__forceinline tvec4( const V3& v, T d ){x=v.x;y=v.y;z=v.z;w=d;}

	// assignment operators
	default_assns(tvec4);
	__forceinline tvec4& operator=( A<T>&& v ){ memmove(this,&v,sizeof(v)); return *this; }
	__forceinline tvec4& operator=( const A<T>& v ){ memcpy(this,&v,sizeof(v)); return *this; }

	// casting operators
	__forceinline operator A<T>&(){ return reinterpret_cast<A<T>&>(*this); }
	__forceinline operator const A<T>&() const { return reinterpret_cast<const A<T>&>(*this); }

	// comparison operators
	__forceinline bool operator==(const tvec4& v) const { return x==v.x&&y==v.y&&z==v.z&&w==v.w; }
	__forceinline bool operator!=(const tvec4& v) const { return !operator==(v); }

	// unary operators
	__forceinline tvec4& operator+(){ return *this; }
	__forceinline const tvec4& operator+() const { return *this; }
	signed_memfun(U) __forceinline tvec4<U> operator-() const { return tvec4(-x,-y,-z,-w); }

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
	float_memfun(U) __forceinline U length2() const { return T(x*x+y*y+z*z+w*w); }
	float_memfun(U) __forceinline U norm2() const { return T(x*x+y*y+z*z+w*w); }
	float_memfun(U) __forceinline U length() const { return T(sqrt(x*x+y*y+z*z+w*w)); }
	float_memfun(U) __forceinline U norm() const { return T(sqrt(x*x+y*y+z*z+w*w)); }
	float_memfun(U) __forceinline U dot( const tvec4& v ) const { return x*v.x+y*v.y+z*v.z+w*v.w; }
	float_memfun(U) __forceinline tvec4<U> normalize() const { return operator/(length()); }
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
using vec2	= tvec2<float>;		using vec3	= tvec3<float>;		using vec4	= tvec4<float>;
using dvec2 = tvec2<double>;	using dvec3 = tvec3<double>;	using dvec4 = tvec4<double>;
using ivec2 = tvec2<int>;		using ivec3 = tvec3<int>;		using ivec4 = tvec4<int>;
using uvec2 = tvec2<uint>;		using uvec3 = tvec3<uint>;		using uvec4 = tvec4<uint>;
using bvec2 = tvec2<bool>;		using bvec3 = tvec3<bool>;		using bvec4 = tvec4<bool>;

static_assert(sizeof(vec2)==(sizeof(float)*2),"sizeof(vec2)!=sizeof(float)*2" );
static_assert(sizeof(vec3)==(sizeof(float)*3),"sizeof(vec3)!=sizeof(float)*3" );
static_assert(sizeof(vec4)==(sizeof(float)*4),"sizeof(vec4)!=sizeof(float)*4" );

// basic math types for computer graphics
struct vertex { vec3 pos; vec3 norm; vec2 tex; };	// default vertex layout
struct bbox_t { alignas(16) vec3 m=-3.402823466e+38F; alignas(16) vec3 M=3.402823466e+38F; }; // bounding box in std140 layout; FLT_MAX = 3.402823466e+38F

//*************************************
// std::hash support here

#ifdef _M_X64
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
struct half { unsigned short mantissa:10,exponent:5,sign:1; __forceinline operator float() const; };	// IEEE 754-2008 half-precision (16-bit) floating-point storage. // https://github.com/HeliumProject/Helium/blob/master/Math/Float16.cpp
__forceinline float	htof( half value ){ struct _float32_t {uint mantissa:23,exponent:8,sign:1;}; _float32_t result;result.sign = value.sign;uint exponent=value.exponent,mantissa=value.mantissa; if(exponent==31){result.exponent=255;result.mantissa=0;} else if(exponent==0&&mantissa==0){result.exponent=0;result.mantissa=0;} else if(exponent==0){uint mantissa_shift=10-static_cast<uint>(log2(float(mantissa)));result.exponent=127-(15-1)-mantissa_shift;result.mantissa=mantissa<<(mantissa_shift+23-10);} else{result.exponent=127-15+exponent;result.mantissa=static_cast<uint>(value.mantissa)<<(23-10);} return reinterpret_cast<float&>(result); }
__forceinline half::operator float() const {return htof(*this);}
__forceinline float*htof( const half* ph, float* pf, size_t nElements, size_t float_stride=1 ){ if(pf==nullptr||ph==nullptr||nElements==0) return pf; float* pf0=pf; for( size_t k=0; k < nElements; k++, ph++, pf+=float_stride ) *pf=htof(*ph); return pf0; }
__forceinline half ftoh( float f ){ struct _float32_t {uint mantissa:23,exponent:8,sign:1;}; _float32_t value=reinterpret_cast<_float32_t&>(f);half result;result.sign=value.sign;uint exponent=value.exponent; if(exponent==255){result.exponent=31;result.mantissa=0;} else if(exponent<127-15+1){uint mantissa=(1<<23)|value.mantissa;size_t mantissa_shift=(23-10)+(127-15+1)-exponent;result.exponent=0;result.mantissa=static_cast<ushort>(mantissa>>mantissa_shift);} else if(exponent>127+(31-15-1)){result.exponent=31-1;result.mantissa=1023;} else {result.exponent=static_cast<ushort>(127-15+exponent);result.mantissa=static_cast<ushort>(value.mantissa>>(23-10));} return result; }
using half2	= tarray2<half>;	using half3	= tarray3<half>;	using half4 = tarray4<half>;

// half vector conversion
__forceinline vec2	htof( const half2& h ){ return vec2(htof(h.x),htof(h.y)); }
__forceinline vec3	htof( const half3& h ){ return vec3(htof(h.x),htof(h.y),htof(h.z)); }
__forceinline vec4	htof( const half4& h ){ return vec4(htof(h.x),htof(h.y),htof(h.z),htof(h.w)); }
__forceinline half2	ftoh( const vec2& v ){ return half2{ftoh(v.x),ftoh(v.y)}; }
__forceinline half3	ftoh( const vec3& v ){ return half3{ftoh(v.x),ftoh(v.y),ftoh(v.z)}; }
__forceinline half4 ftoh( const vec4& v ){ return half4{ftoh(v.x),ftoh(v.y),ftoh(v.z),ftoh(v.w)}; }
__forceinline half* ftoh( const float* pf, half* ph, size_t nElements, size_t half_stride=1 ){ if(pf==nullptr||ph==nullptr||nElements==0) return ph; half* ph0=ph; for( size_t k=0; k < nElements; k++, pf++, ph+=half_stride ) *ph=ftoh(*pf); return ph0; }

//*************************************
struct mat2
{
	using T=float;
	default_tarray(4);

	union { float a[4]; struct {float _11,_12,_21,_22;}; };

	// constructors
	__forceinline mat2(){ set_identity(); }
	__forceinline mat2( mat2&& v ) = default;
	__forceinline mat2( const mat2& v ) = default;
	__forceinline mat2( float f11, float f12, float f21, float f22 ){_11=f11;_12=f12;_21=f21;_22=f22;}
	__forceinline mat2( float4&& f ){ memmove(a,&f,sizeof(f)); }
	__forceinline mat2( const float4& f ){ memcpy(a,&f,sizeof(f)); }

	// assignment
	__forceinline mat2& operator=( mat2&& v ){ memmove(this,&v,sizeof(v)); return *this; }
	__forceinline mat2& operator=( const mat2& v ){ memcpy(this,&v,sizeof(v)); return *this; }

	// casting operators
	__forceinline operator float4& (){ return reinterpret_cast<float4&>(*this); }
	__forceinline operator const float4& () const { return reinterpret_cast<const float4&>(*this); }

	// comparison operators
	__forceinline bool operator==( const mat2& m ) const { for( int k=0; k<std::extent<decltype(a)>::value; k++ ) if(std::abs(a[k]-m[k])>precision<float>::value()) return false; return true; }
	__forceinline bool operator!=( const mat2& m ) const { return !operator==(m); }

	// unary operators
	__forceinline mat2& operator+(){ return *this; }
	__forceinline const mat2& operator+() const { return *this; }
	__forceinline mat2 operator-() const { return mat2(-_11,-_12,-_21,-_22); }

	// column/row/diagonal vectors
	__forceinline vec2 cvec2( int col ) const { return vec2(a[col],a[2+col]); }
	__forceinline vec2 diag() const { return vec2(_11,_22); }
	__forceinline const vec2& rvec2( int row ) const { return reinterpret_cast<const vec2&>(a[row*2]); }
	__forceinline vec2& rvec2( int row ){ return reinterpret_cast<vec2&>(a[row*2]); }

	// addition/subtraction operators
	__forceinline mat2& operator+=( const mat2& m ){ for( int k=0; k < std::extent<decltype(a)>::value; k++ ) a[k]+=m[k]; return *this; }
	__forceinline mat2& operator-=( const mat2& m ){ for( int k=0; k < std::extent<decltype(a)>::value; k++ ) a[k]-=m[k]; return *this; }
	__forceinline mat2 operator+( const mat2& m ) const { return mat2(*this).operator+=(m); }
	__forceinline mat2 operator-( const mat2& m ) const { return mat2(*this).operator-=(m); }

	// multiplication operators
	__forceinline mat2& operator*=( float f ){ for( int k=0; k < std::extent<decltype(a)>::value; k++ ) a[k]*=f; return *this; }
	__forceinline mat2& operator*=( const mat2& m ){ mat2 t=m.transpose(); for(uint k=0;k<2;k++) rvec2(k)=t.operator*(rvec2(k)); return *this; } // a bit tricky implementation
	__forceinline mat2 operator*( float f ) const { return mat2(*this).operator*=(f); }
	__forceinline mat2 operator*( const mat2& m ) const { return mat2(*this).operator*=(m); }
	__forceinline vec2 operator*( const vec2& v ) const { return vec2(rvec2(0).dot(v), rvec2(1).dot(v)); }

	// identity and transpose
	__forceinline static mat2 identity(){ return mat2(); }
	__forceinline mat2& set_identity(){ _12=_21=0.0f;_11=_22=1.0f; return *this; }
	__forceinline mat2 transpose() const { return mat2(_11,_21,_12,_22); }

	// determinant/trace/inverse
	__forceinline float det() const { return _11*_22 - _12*_21; }
	__forceinline float trace() const { return _11+_22; }
	__forceinline mat2 inverse() const { float s=1.0f/det(); return mat2( +_22*s, -_12*s, -_21*s, +_11*s ); }

	// static row-major transformations: 2D transformation in 2D Cargesian coordinate system
	__forceinline static mat2 scale( const vec2& v ){ return mat2().set_scale(v); }
	__forceinline static mat2 scale( float x, float y ){ return mat2().set_scale(x,y); }
	__forceinline static mat2 rotate( float theta ){ return mat2().set_rotate(theta); }

	// row-major transformations: 2D transformation in 2D Cargesian coordinate system
	__forceinline mat2& set_scale( const vec2& v ){ _11=v.x; _12=0.0f; _21=0.0f; _22=v.y; return *this; }
	__forceinline mat2& set_scale( float x, float y ){ _11=x; _12=0.0f; _21=0.0f; _22=y; return *this; }
	__forceinline mat2& set_rotate( float theta ){ _11=_22=cosf(theta);_21=sinf(theta);_12=-_21; return *this; }
};

//*************************************
struct mat3
{
	using T=float;
	default_tarray(9);

	union{float a[9];struct{float _11,_12,_13,_21,_22,_23,_31,_32,_33;};};

	// constructors
	__forceinline mat3(){ set_identity(); }
	__forceinline mat3( mat3&& )=default;
	__forceinline mat3( const mat3& )=default;
	__forceinline mat3( float f11, float f12, float f13, float f21, float f22, float f23, float f31, float f32, float f33 ){_11=f11;_12=f12;_13=f13;_21=f21;_22=f22;_23=f23;_31=f31;_32=f32;_33=f33;}
	__forceinline mat3( float9&& f ){ memmove(a,&f,sizeof(f)); }
	__forceinline mat3( const float9& f ){ memcpy(a,&f,sizeof(f)); }

	// assignment operators
	__forceinline mat3& operator=( mat3&& v ){ memmove(this,&v,sizeof(v)); return *this; }
	__forceinline mat3& operator=( const mat3& v ){ memcpy(this,&v,sizeof(v)); return *this; }
	__forceinline mat3& operator=( float9&& v ){ memmove(this,&v,sizeof(v)); return *this; }
	__forceinline mat3& operator=( const float9& v ){ memcpy(this,&v,sizeof(v)); return *this; }

	// casting operators
	__forceinline operator float9& (){ return reinterpret_cast<float9&>(*this); }
	__forceinline operator const float9& () const { return reinterpret_cast<const float9&>(*this); }

	// comparison operators
	__forceinline bool operator==( const mat3& m ) const { for( int k=0; k<std::extent<decltype(a)>::value; k++ ) if(std::abs(a[k]-m[k])>precision<float>::value()) return false; return true; }
	__forceinline bool operator!=( const mat3& m ) const { return !operator==(m); }

	// unary operators
	__forceinline mat3& operator+(){ return *this; }
	__forceinline const mat3& operator+() const { return *this; }
	__forceinline mat3 operator-() const { return mat3(-_11,-_12,-_13,-_21,-_22,-_23,-_31,-_32,-_33); }

	// column/row/diagonal vectors
	__forceinline vec3 cvec3( int col ) const { return vec3(a[col],a[3+col],a[6+col]); }
	__forceinline vec3 diag() const { return vec3(_11,_22,_33); }
	__forceinline const vec3& rvec3( int row ) const { return reinterpret_cast<const vec3&>(a[row*3]); }
	__forceinline vec3& rvec3( int row ){ return reinterpret_cast<vec3&>(a[row*3]); }

	// addition/subtraction operators
	__forceinline mat3& operator+=( const mat3& m ){ for( int k=0; k < std::extent<decltype(a)>::value; k++ ) a[k]+=m[k]; return *this; }
	__forceinline mat3& operator-=( const mat3& m ){ for( int k=0; k < std::extent<decltype(a)>::value; k++ ) a[k]-=m[k]; return *this; }
	__forceinline mat3 operator+( const mat3& m ) const { return mat3(*this).operator+=(m); }
	__forceinline mat3 operator-( const mat3& m ) const { return mat3(*this).operator-=(m); }

	// multiplication operators
	__forceinline mat3& operator*=( float f ){ for( int k=0; k < std::extent<decltype(a)>::value; k++ ) a[k]*=f; return *this; }
	__forceinline mat3& operator*=( const mat3& m ){ mat3 t=m.transpose(); for(uint k=0;k<3;k++) rvec3(k)=t.operator*(rvec3(k)); return *this; } // a bit tricky implementation
	__forceinline mat3 operator*( float f ) const { return mat3(*this).operator*=(f); }
	__forceinline mat3 operator*( const mat3& m ) const { return mat3(*this).operator*=(m); }
	__forceinline vec3 operator*( const vec3& v ) const { return vec3(rvec3(0).dot(v), rvec3(1).dot(v), rvec3(2).dot(v)); }

	// identity and transpose
	__forceinline static mat3 identity(){ return mat3(); }
	__forceinline mat3& set_identity(){ _12=_13=_21=_23=_31=_32=0.0f;_11=_22=_33=1.0f; return *this; }
	__forceinline mat3 transpose() const { return mat3(_11,_21,_31,_12,_22,_32,_13,_23,_33); }

	// determinant/trace/inverse
	__forceinline float det() const { return _11*(_22*_33-_23*_32) + _12*(_23*_31-_21*_33) + _13*(_21*_32-_22*_31); }
	__forceinline float trace() const { return _11+_22+_33; }
	__forceinline mat3 inverse() const { float s=1.0f/det(); return mat3( (_22*_33-_32*_23)*s, (_13*_32-_12*_33)*s, (_12*_23-_13*_22)*s, (_23*_31-_21*_33)*s, (_11*_33-_13*_31)*s, (_21*_13-_11*_23)*s, (_21*_32-_31*_22)*s, (_31*_12-_11*_32)*s, (_11*_22-_21*_12)*s ); }

	// static row-major transformations: 2D transformation in 3D homogeneous coordinate system
	__forceinline static mat3 translate( const vec2& v ){ return mat3().set_translate(v); }
	__forceinline static mat3 translate( float x, float y ){ return mat3().set_translate(x,y); }
	__forceinline static mat3 scale( const vec2& v ){ return mat3().set_scale(v); }
	__forceinline static mat3 scale( float x, float y ){ return mat3().set_scale(x,y); }
	__forceinline static mat3 rotate( float theta ){ return mat3().set_rotate(theta); }

	// row-major transformations: 2D transformation in 3D homogeneous coordinate system
	__forceinline mat3& set_translate( const vec2& v ){ set_identity(); _13=v.x; _23=v.y; return *this; }
	__forceinline mat3& set_translate( float x,float y ){ set_identity(); _13=x; _23=y; return *this; }
	__forceinline mat3& set_scale( const vec2& v ){ set_identity(); _11=v.x; _22=v.y; return *this; }
	__forceinline mat3& set_scale( float x, float y ){ set_identity(); _11=x; _22=y; return *this; }
	__forceinline mat3& set_rotate( float theta ){ set_identity(); _11=_22=cosf(theta);_21=sinf(theta);_12=-_21; return *this; }
};

//*************************************
// mat4 uses only row-major and right-hand (RH) notations even for D3D
struct mat4
{
	using T=float;
	default_tarray(16);

	union{float a[16];struct{float _11,_12,_13,_14,_21,_22,_23,_24,_31,_32,_33,_34,_41,_42,_43,_44;}; };

	// constructors
	__forceinline mat4(){ set_identity(); }
	__forceinline mat4( mat4&& )=default;
	__forceinline mat4( const mat4& )=default;
	__forceinline mat4( float f11, float f12, float f13, float f14, float f21, float f22, float f23, float f24, float f31, float f32, float f33, float f34, float f41, float f42, float f43, float f44 ){_11=f11;_12=f12;_13=f13;_14=f14;_21=f21;_22=f22;_23=f23;_24=f24;_31=f31;_32=f32;_33=f33;_34=f34;_41=f41;_42=f42;_43=f43;_44=f44;}
	__forceinline mat4( float16&& f ){ memmove(a,&f,sizeof(float)*16); }
	__forceinline mat4( const float16& f ){ memcpy(a,&f,sizeof(float)*16); }

	// assignment operators
	__forceinline mat4& operator=( mat4&& v ){ memmove(this,&v,sizeof(v)); return *this; }
	__forceinline mat4& operator=( const mat4& v ){ memcpy(this,&v,sizeof(v)); return *this; }
	__forceinline mat4& operator=( float16&& v ){ memmove(this,&v,sizeof(v)); return *this; }
	__forceinline mat4& operator=( const float16& v ){ memcpy(this,&v,sizeof(v)); return *this; }

	// casting operators
	__forceinline operator float16&(){ return reinterpret_cast<float16&>(*this); }
	__forceinline operator const float16& () const { return reinterpret_cast<const float16&>(*this); }
	__forceinline operator mat3() const { return mat3(_11,_12,_13,_21,_22,_23,_31,_32,_33); }
	__forceinline operator mat2() const { return mat2(_11,_12,_21,_22); }

	// comparison operators
	__forceinline bool operator==( const mat4& m ) const { for( int k=0; k<std::extent<decltype(a)>::value; k++ ) if(std::abs(a[k]-m[k])>precision<float>::value()) return false; return true; }
	__forceinline bool operator!=( const mat4& m ) const { return !operator==(m); }

	// unary operators
	__forceinline mat4& operator+(){ return *this; }
	__forceinline const mat4& operator+() const { return *this; }
	__forceinline mat4 operator-() const { return mat4(-_11,-_12,-_13,-_14,-_21,-_22,-_23,-_24,-_31,-_32,-_33,-_34,-_41,-_42,-_43,-_44); }

	// column/row/diagonal vectors
	__forceinline vec4 cvec4( int col ) const { return vec4(a[col],a[4+col],a[8+col],a[12+col]); }
	__forceinline vec3 cvec3( int col ) const { return vec3(a[col],a[4+col],a[8+col]); }
	__forceinline vec4 diag() const { return vec4(_11,_22,_33,_44); }
	__forceinline const vec4& rvec4( int row ) const { return reinterpret_cast<const vec4&>(a[row*4]); }
	__forceinline vec4& rvec4( int row ){ return reinterpret_cast<vec4&>(a[row*4]); }
	__forceinline const vec3& rvec3( int row ) const { return reinterpret_cast<const vec3&>(a[row*4]); }
	__forceinline vec3& rvec3( int row ){ return reinterpret_cast<vec3&>(a[row*4]); }

	// addition/subtraction operators
	__forceinline mat4& operator+=( const mat4& m ){ for( int k=0; k < std::extent<decltype(a)>::value; k++ ) a[k]+=m[k]; return *this; }
	__forceinline mat4& operator-=( const mat4& m ){ for( int k=0; k < std::extent<decltype(a)>::value; k++ ) a[k]-=m[k]; return *this; }
	__forceinline mat4 operator+( const mat4& m ) const { return mat4(*this).operator+=(m); }
	__forceinline mat4 operator-( const mat4& m ) const { return mat4(*this).operator-=(m); }

	// multiplication operators
	__forceinline mat4& operator*=( float f ){ for( int k=0; k < std::extent<decltype(a)>::value; k++ ) a[k]*=f; return *this; }
	__forceinline mat4& operator*=( const mat4& m ){ mat4 t=m.transpose(); for(uint k=0;k<4;k++) rvec4(k)=t.operator*(rvec4(k)); return *this; } // a bit tricky implementation
	__forceinline mat4 operator*( float f ) const { return mat4(*this).operator*=(f); }
	__forceinline mat4 operator*( const mat4& m ) const { return mat4(*this).operator*=(m); }
	__forceinline vec4 operator*( const vec4& v ) const { return vec4(rvec4(0).dot(v),rvec4(1).dot(v),rvec4(2).dot(v),rvec4(3).dot(v)); }
	__forceinline vec3 operator*( const vec3& v ) const { vec4 v4(v,1); return vec3(rvec4(0).dot(v4),rvec4(1).dot(v4),rvec4(2).dot(v4)); }

	// identity and transpose
	__forceinline static mat4 identity(){ return mat4(); }
	__forceinline mat4& set_identity(){ _12=_13=_14=_21=_23=_24=_31=_32=_34=_41=_42=_43=0.0f;_11=_22=_33=_44=1.0f; return *this; }
	__forceinline mat4 transpose() const { return mat4(_11,_21,_31,_41,_12,_22,_32,_42,_13,_23,_33,_43,_14,_24,_34,_44); }

	// determinant/trace/inverse
	vec4 _xdet() const;	// support function for det() and inverse()
	__forceinline float det() const { return cvec4(3).dot(_xdet()); }
	__forceinline float trace() const { return _11+_22+_33+_44; }
	mat4 inverse() const;

	// static row-major transformations
	__forceinline static mat4 translate( const vec3& v ){ return mat4().set_translate(v); }
	__forceinline static mat4 translate( float x, float y, float z ){ return mat4().set_translate(x,y,z); }
	__forceinline static mat4 scale( const vec3& v ){ return mat4().set_scale(v); }
	__forceinline static mat4 scale( float x, float y, float z ){ return mat4().set_scale(x,y,z); }
	__forceinline static mat4 shear( const vec2& yz, const vec2& zx, const vec2& xy ){ return mat4().set_shear(yz,zx,xy); }
	__forceinline static mat4 rotate_vec_to_vec( const vec3& v0, vec3 v1 ){ return mat4().set_rotate_vec_to_vec(v0,v1); }
	__forceinline static mat4 rotate( const vec3& axis, float angle ){ return mat4().set_rotate(axis,angle); }

	__forceinline static mat4 viewport( int width, int height ){ return mat4().set_viewport(width,height); }
	__forceinline static mat4 look_at( const vec3& eye, const vec3& center, const vec3& up ){ return mat4().set_look_at(eye,center,up); }
	__forceinline static mat4 look_at_inverse( const vec3& eye, const vec3& center, const vec3& up ){ return mat4().set_look_at_inverse(eye,center,up); }

	__forceinline static mat4 perspective( float fovy, float aspect, float dn, float df ){ return mat4().set_perspective(fovy,aspect,dn,df); }
	__forceinline static mat4 perspective_off_center( float left, float right, float top, float bottom, float dn, float df ){ return mat4().set_perspective_off_center(left,right,top,bottom,dn,df); }
	__forceinline static mat4 ortho( float width, float height, float dn, float df ){ return mat4().set_ortho(width,height,dn,df); }
	__forceinline static mat4 ortho_off_center( float left, float right, float top, float bottom, float dn, float df ){ return mat4().set_ortho_off_center(left,right,top,bottom,dn,df); }
	__forceinline static mat4 perspective_dx( float fovy, float aspect, float dn, float df ){ return mat4().set_perspective_dx(fovy,aspect,dn,df); }
	__forceinline static mat4 perspective_off_center_dx( float left, float right, float top, float bottom, float dn, float df ){ return mat4().set_perspective_off_center_dx(left,right,top,bottom,dn,df); }
	__forceinline static mat4 ortho_dx( float width, float height, float dn, float df ){ return mat4().set_ortho_dx(width,height,dn,df); }
	__forceinline static mat4 ortho_off_center_dx( float left, float right, float top, float bottom, float dn, float df ){ return mat4().set_ortho_off_center_dx(left,right,top,bottom,dn,df); }

	// row-major transformations
	__forceinline mat4& set_translate( const vec3& v ){ set_identity(); _14=v.x; _24=v.y; _34=v.z; return *this; }
	__forceinline mat4& set_translate( float x,float y,float z ){ set_identity(); _14=x; _24=y; _34=z; return *this; }
	__forceinline mat4& set_scale( const vec3& v ){ set_identity(); _11=v.x; _22=v.y; _33=v.z; return *this; }
	__forceinline mat4& set_scale( float x,float y,float z ){ set_identity(); _11=x; _22=y; _33=z; return *this; }
	__forceinline mat4& set_shear( const vec2& yz, const vec2& zx, const vec2& xy ){ set_identity(); _12=yz.x; _13=yz.y; _21=zx.y; _23=zx.x; _31=xy.x; _32=xy.y; return *this; }
	__forceinline mat4& set_rotate_vec_to_vec( const vec3& from, const vec3& to )
	{
		float fdt=from.dot(to); if(abs(fdt)>0.999999f) return fdt>0?set_identity():set_scale(-1.0f,-1.0f,-1.0f); // degenerate case:s exactly the same vectors or flipped
		const vec3 n=from.cross(to);
		return set_rotate( n.normalize(), asin(min(n.length(),1.0f)) );
	}
	__noinline mat4& set_rotate( const vec3& axis, float angle )
	{
		float c=cos(angle), s=sin(angle), x=axis.x, y=axis.y, z=axis.z;
		a[0] = x*x*(1-c)+c;		a[1] = x*y*(1-c)-z*s;		a[2] = x*z*(1-c)+y*s;	a[3] = 0.0f;
		a[4] = x*y*(1-c)+z*s;	a[5] = y*y*(1-c)+c;			a[6] = y*z*(1-c)-x*s;	a[7] = 0.0f;
		a[8] = x*z*(1-c)-y*s;	a[9] = y*z*(1-c)+x*s;		a[10] = z*z*(1-c)+c;	a[11] = 0.0f;
		a[12] = 0;				a[13] = 0;					a[14] = 0;				a[15] = 1.0f;
		return *this;
	}

	// viewport, lookat, projection
	__forceinline mat4& set_viewport( int width, int height ){ set_identity(); _11=width*0.5f; _22=-height*0.5f; _14=width*0.5f; _24=height*0.5f; return *this; }
	__forceinline mat4& set_look_at( const vec3& eye, const vec3& center, const vec3& up ){ set_identity(); rvec3(2) = (eye-center).normalize(); rvec3(0) = (up.cross(rvec3(2))).normalize(); rvec3(1) = rvec3(2).cross(rvec3(0)); return *this = (*this)*(mat4::translate(-eye)); }
	__forceinline mat4& set_look_at_inverse( const vec3& eye, const vec3& center, const vec3& up ){ set_identity(); rvec3(2) = (eye-center).normalize(); rvec3(0) = (up.cross(rvec3(2))).normalize(); rvec3(1) = rvec3(2).cross(rvec3(0)); return *this = mat4::translate(eye)*transpose(); }
	__forceinline vec3  look_at_eye() const { const vec3 &u=rvec3(0),&v=rvec3(1),&n=rvec3(2),uv=u.cross(v),vn=v.cross(n),nu=n.cross(u); return (vn*_14+nu*_24+uv*_34)/(-u.dot(vn)); }
	__forceinline mat4  look_at_inverse() const { vec3 eye=look_at_eye(); return mat4(_11,_21,_31,eye.x,_12,_22,_32,eye.y,_13,_23,_33,eye.z,0,0,0,1); }

	// Canonical view volume in OpenGL: [-1,1]^3
	__forceinline mat4& set_perspective( float fovy, float aspect, float dn, float df ){ if(fovy>PI<float>) fovy*=PI<float>/180.0f; /* autofix for fov in degrees */ set_identity(); _22=1.0f/tanf(fovy*0.5f); _11=_22/aspect; _33=(dn+df)/(dn-df); _34=2.0f*dn*df/(dn-df); _43=-1.0f;  _44=0.0f; return *this; }
	__forceinline mat4& set_perspective_off_center( float left, float right, float top, float bottom, float dn, float df ){ set_identity(); _11=2.0f*dn/(right-left); _22=2.0f*dn/(top-bottom); _13=(right+left)/(right-left); _23=(top+bottom)/(top-bottom); _33=(dn+df)/(dn-df); _34=2.0f*dn*df/(dn-df); _43=-1.0f; _44=0.0f; return *this; }
	__forceinline mat4& set_ortho( float width, float height, float dn, float df ){ set_identity(); _11=2.0f/width; _22=2.0f/height;  _33=2.0f/(dn-df); _34=(dn+df)/(dn-df); return *this; }
	__forceinline mat4& set_ortho_off_center( float left, float right, float top, float bottom, float dn, float df ){ set_ortho( right-left, top-bottom, dn, df ); _14=(left+right)/(left-right); _24=(bottom+top)/(bottom-top); return *this; }

	// Canonical view volume in DirectX: [-1,1]^2*[0,1]: diffes only in _33 and _34
	__forceinline mat4& set_perspective_dx( float fovy, float aspect, float dn, float df ){ set_perspective( fovy, aspect, dn, df ); _33=df/(dn-df); _34*=0.5f; return *this; } // equivalent to D3DXMatrixPerspectiveFovRH()
	__forceinline mat4& set_perspective_off_center_dx( float left, float right, float top, float bottom, float dn, float df ){ set_perspective_off_center( left, right, top, bottom, dn, df ); _33=df/(dn-df); _34*=0.5f; return *this; }
	__forceinline mat4& set_ortho_dx( float width, float height, float dn, float df ){ set_ortho( width, height, dn, df ); _33*=0.5f; _34=dn/(dn-df); return *this; }
	__forceinline mat4& set_ortho_off_center_dx( float left, float right, float top, float bottom, float dn, float df ){ set_ortho_off_center( left, right, top, bottom, dn, df ); _33*=0.5f; _34=dn/(dn-df); return *this; }
};

__noinline inline vec4 mat4::_xdet() const
{
	return vec4((_41*_32-_31*_42)*_23+(_21*_42-_41*_22)*_33+(_31*_22-_21*_32)*_43,
				(_31*_42-_41*_32)*_13+(_41*_12-_11*_42)*_33+(_11*_32-_31*_12)*_43,
				(_41*_22-_21*_42)*_13+(_11*_42-_41*_12)*_23+(_21*_12-_11*_22)*_43,
				(_21*_32-_31*_22)*_13+(_31*_12-_11*_32)*_23+(_11*_22-_21*_12)*_33);
}

// http://www.cg.info.hiroshima-cu.ac.jp/~miyazaki/knowledge/teche23.html
__noinline inline mat4 mat4::inverse() const
{
	vec4 xd=_xdet();
	return mat4((_32*_43-_42*_33)*_24 + (_42*_23-_22*_43)*_34 + (_22*_33-_32*_23)*_44,
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
				xd.x, xd.y, xd.z, xd.w )*(1.0f/cvec4(3).dot(xd));
}

//*************************************
// matrix size check
static_assert(sizeof(mat2)%sizeof(float)*4==0,"sizeof(mat2)!=sizeof(float)*4" );
static_assert(sizeof(mat3)%sizeof(float)*9==0,"sizeof(mat3)!=sizeof(float)*9" );
static_assert(sizeof(mat4)%sizeof(float)*16==0,"sizeof(mat4)!=sizeof(float)*16" );

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
template <class T> __forceinline tvec2<enable_signed_t<T>> operator-( T f, const tvec2<T>& v ){ return -v+f; }
template <class T> __forceinline tvec2<T> operator*( T f, const tvec2<T>& v ){ return v*f; }
template <class T> __forceinline tvec2<T> operator/( T f, const tvec2<T>& v ){ return tvec2<T>(f/v.x,f/v.y); }
template <class T> __forceinline tvec3<T> operator+( T f, const tvec3<T>& v ){ return v+f; }
template <class T> __forceinline tvec3<enable_signed_t<T>> operator-( T f, const tvec3<T>& v ){ return -v+f; }
template <class T> __forceinline tvec3<T> operator*( T f, const tvec3<T>& v ){ return v*f; }
template <class T> __forceinline tvec3<T> operator/( T f, const tvec3<T>& v ){ return tvec3<T>(f/v.x,f/v.y,f/v.z); }
template <class T> __forceinline tvec4<T> operator+( T f, const tvec4<T>& v ){ return v+f; }
template <class T> __forceinline tvec4<enable_signed_t<T>> operator-( T f, const tvec4<T>& v ){ return -v+f; }
template <class T> __forceinline tvec4<T> operator*( T f, const tvec4<T>& v ){ return v*f; }
template <class T> __forceinline tvec4<T> operator/( T f, const tvec4<T>& v ){ return tvec4<T>(f/v.x,f/v.y,f/v.z,f/v.w); }

//*************************************
// global operators for vector length/normalize/dot/cross
template <class T> __forceinline enable_float_t<T> length( const tvec2<T>& v ){ return v.length(); }
template <class T> __forceinline enable_float_t<T> length( const tvec3<T>& v ){ return v.length(); }
template <class T> __forceinline enable_float_t<T> length( const tvec4<T>& v ){ return v.length(); }
template <class T> __forceinline enable_float_t<T> length2( const tvec2<T>& v ){ return v.length2(); }
template <class T> __forceinline enable_float_t<T> length2( const tvec3<T>& v ){ return v.length2(); }
template <class T> __forceinline enable_float_t<T> length2( const tvec4<T>& v ){ return v.length2(); }
template <class T> __forceinline tvec2<enable_float_t<T>> normalize( const tvec2<T>& v ){ return v.normalize(); }
template <class T> __forceinline tvec3<enable_float_t<T>> normalize( const tvec3<T>& v ){ return v.normalize(); }
template <class T> __forceinline tvec4<enable_float_t<T>> normalize( const tvec4<T>& v ){ return v.normalize(); }
template <class T> __forceinline enable_float_t<T> dot( const tvec2<T>& v1, const tvec2<T>& v2){ return v1.dot(v2); }
template <class T> __forceinline enable_float_t<T> dot( const tvec3<T>& v1, const tvec3<T>& v2){ return v1.dot(v2); }
template <class T> __forceinline enable_float_t<T> dot( const tvec4<T>& v1, const tvec4<T>& v2){ return v1.dot(v2); }
template <class T> __forceinline tvec3<enable_float_t<T>> cross( const tvec3<T>& v1, const tvec3<T>& v2){ return v1.cross(v2); }

//*************************************
// general math utility functions
template <class T> __forceinline enable_float_t<T> radians( T f ){ return f*PI<T>/T(180.0); }
template <class T> __forceinline enable_float_t<T> degrees( T f ){ return f*T(180.0)/PI<T>; }
template <class T> __forceinline tvec2<enable_float_t<T>> minmax( const tvec2<T>& a, const tvec2<T>& b ){ return tvec2<T>(a.x<b.x?a.x:b.x,a.y>b.y?a.y:b.y); }
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
// {GLSL|HLSL}-like shader intrinsic functions
__forceinline vec2 abs( const vec2& v ){ return vec2(fabs(v.x),fabs(v.y)); }
__forceinline vec3 abs( const vec3& v ){ return vec3(fabs(v.x),fabs(v.y),fabs(v.z)); }
__forceinline vec4 abs( const vec4& v ){ return vec4(fabs(v.x),fabs(v.y),fabs(v.z),fabs(v.w)); }
__forceinline float distance( const vec2& a, const vec2& b ){ return (a-b).length(); }
__forceinline float distance( const vec3& a, const vec3& b ){ return (a-b).length(); }
__forceinline float distance( const vec4& a, const vec4& b ){ return (a-b).length(); }
__forceinline vec2 floor( const vec2& v ){ return vec2(floor(v.x),floor(v.y)); }
__forceinline vec3 floor( const vec3& v ){ return vec3(floor(v.x),floor(v.y),floor(v.z)); }
__forceinline vec4 floor( const vec4& v ){ return vec4(floor(v.x),floor(v.y),floor(v.z),floor(v.w)); }
__forceinline float fract( float f ){ return float(f-floor(f)); }
__forceinline vec2 fract( const vec2& v ){ return vec2(fract(v.x),fract(v.y)); }
__forceinline vec3 fract( const vec3& v ){ return vec3(fract(v.x),fract(v.y),fract(v.z)); }
__forceinline vec4 fract( const vec4& v ){ return vec4(fract(v.x),fract(v.y),fract(v.z),fract(v.w)); }
__forceinline vec2 fma( vec2 a, vec2 b, vec2 c ){ return vec2(fma(a.x,b.x,c.x),fma(a.y,b.y,c.y)); }
__forceinline vec3 fma( vec3 a, vec3 b, vec3 c ){ return vec3(fma(a.x,b.x,c.x),fma(a.y,b.y,c.y),fma(a.z,b.z,c.z)); }
__forceinline vec4 fma( vec4 a, vec4 b, vec4 c ){ return vec4(fma(a.x,b.x,c.x),fma(a.y,b.y,c.y),fma(a.z,b.z,c.z),fma(a.w,b.w,c.w)); }
__forceinline vec2 fabs( const vec2& v ){ return vec2(fabs(v.x),fabs(v.y)); }
__forceinline vec3 fabs( const vec3& v ){ return vec3(fabs(v.x),fabs(v.y),fabs(v.z)); }
__forceinline vec4 fabs( const vec4& v ){ return vec4(fabs(v.x),fabs(v.y),fabs(v.z),fabs(v.w)); }
__forceinline float lerp( float v1, float v2, float t ){ return v1*(1.0f-t)+v2*t; }
__forceinline vec2 lerp( const vec2& y1, const vec2& y2, float t ){ return y1*(-t+1.0f)+y2*t; }
__forceinline vec3 lerp( const vec3& y1, const vec3& y2, float t ){ return y1*(-t+1.0f)+y2*t; }
__forceinline vec4 lerp( const vec4& y1, const vec4& y2, float t ){ return y1*(-t+1.0f)+y2*t; }
__forceinline vec2 lerp( const vec2& y1, const vec2& y2, const vec2& t ){ return y1*(-t+1.0f)+y2*t; }
__forceinline vec3 lerp( const vec3& y1, const vec3& y2, const vec3& t ){ return y1*(-t+1.0f)+y2*t; }
__forceinline vec4 lerp( const vec4& y1, const vec4& y2, const vec4& t ){ return y1*(-t+1.0f)+y2*t; }
__forceinline double lerp( double v1, double v2, double t ){ return v1*(1.0-t)+v2*t; }
__forceinline double lerp( double v1, double v2, float t ){ return v1*(1.0f-t)+v2*t; }
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
__forceinline vec2 mix( const vec2& y1, const vec2& y2, const vec2& t ){ return y1*(-t+1.0f)+y2*t; }
__forceinline vec3 mix( const vec3& y1, const vec3& y2, const vec3& t ){ return y1*(-t+1.0f)+y2*t; }
__forceinline vec4 mix( const vec4& y1, const vec4& y2, const vec4& t ){ return y1*(-t+1.0f)+y2*t; }
__forceinline double mix( double v1, double v2, double t ){ return v1*(1.0-t)+v2*t; }
__forceinline double mix( double v1, double v2, float t ){ return v1*(1.0f-t)+v2*t; }
__forceinline dvec2 mix( const dvec2& y1, const dvec2& y2, double t ){ return y1*(-t+1.0)+y2*t; }
__forceinline dvec3 mix( const dvec3& y1, const dvec3& y2, double t ){ return y1*(-t+1.0)+y2*t; }
__forceinline dvec4 mix( const dvec4& y1, const dvec4& y2, double t ){ return y1*(-t+1.0)+y2*t; }
__forceinline dvec2 mix( const dvec2& y1, const dvec2& y2, float t ){ return y1*(-t+1.0)+y2*t; }
__forceinline dvec3 mix( const dvec3& y1, const dvec3& y2, float t ){ return y1*(-t+1.0)+y2*t; }
__forceinline dvec4 mix( const dvec4& y1, const dvec4& y2, float t ){ return y1*(-t+1.0)+y2*t; }
__forceinline mat4 mix( const mat4& v1, const mat4& v2, float t ){ return v1*(1.0f-t)+v2*t; }
__forceinline vec3 reflect( const vec3& I, const vec3& N ){ return I-N*dot(I,N)*2.0f; }	// I: incident vector, N: normal
__forceinline vec3 refract( const vec3& I, const vec3& N, float eta /* = n0/n1 */ ){ float d = I.dot(N); float k = 1.0f-eta*eta*(1.0f-d*d); return k<0.0f?0.0f:(I*eta-N*(eta*d+sqrtf(k))); } // I: incident vector, N: normal
__forceinline float saturate( float f ){ return clamp(f,0.0f,1.0f); }
__forceinline vec2 saturate( const vec2& v ){ return vec2(saturate(v.x),saturate(v.y)); }
__forceinline vec3 saturate( const vec3& v ){ return vec3(saturate(v.x),saturate(v.y),saturate(v.z)); }
__forceinline vec4 saturate(const  vec4& v ){ return vec4(saturate(v.x),saturate(v.y),saturate(v.z),saturate(v.w)); }
__forceinline float sign( float f ){ return f>0.0f?1.0f:f<0.0f?-1.0f:0; }
__forceinline vec2 sign( const vec2& v ){ return vec2(sign(v.x),sign(v.y)); }
__forceinline vec3 sign( const vec3& v ){ return vec3(sign(v.x),sign(v.y),sign(v.z)); }
__forceinline vec4 sign( const vec4& v ){ return vec4(sign(v.x),sign(v.y),sign(v.z),sign(v.w)); }
__forceinline float smoothstep( float t ){ t=clamp(t,0.0f,1.0f); return t*t*(3-2*t); }							// C1-continuity
__forceinline vec2 smoothstep( const vec2& t ){ return vec2(smoothstep(t.x),smoothstep(t.y)); }
__forceinline vec3 smoothstep( const vec3& t ){ return vec3(smoothstep(t.x),smoothstep(t.y),smoothstep(t.z)); }
__forceinline vec4 smoothstep( const vec4& t ){ return vec4(smoothstep(t.x),smoothstep(t.y),smoothstep(t.z),smoothstep(t.w)); }
__forceinline float smootherstep( float t ){ t=clamp(t,0.0f,1.0f); return t*t*t*(6.0f*t*t-15.0f*t+10.0f); }		// C2-continuity (by Ken Perlin)
__forceinline vec2 smootherstep( const vec2& t ){ return vec2(smootherstep(t.x),smootherstep(t.y)); }
__forceinline vec3 smootherstep( const vec3& t ){ return vec3(smootherstep(t.x),smootherstep(t.y),smootherstep(t.z)); }
__forceinline vec4 smootherstep( const vec4& t ){ return vec4(smootherstep(t.x),smootherstep(t.y),smootherstep(t.z),smootherstep(t.w)); }
// packing/unpacking/casting: https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_shading_language_packing.txt
__forceinline uint packUnorm2x16( vec2 v ){ ushort2 u; for(int k=0;k<2;k++) u[k]=ushort(round(clamp(v[k],0.0f,1.0f)*65535.0f)); return reinterpret_cast<uint&>(u); }
__forceinline uint packSnorm2x16( vec2 v ){ short2 s; for(int k=0;k<2;k++) s[k]=short(round(clamp(v[k],-1.0f,1.0f)*32767.0f)); return reinterpret_cast<uint&>(s); }
__forceinline uint packUnorm4x8( vec4 v ){ uchar4 u; for(int k=0;k<4;k++) u[k]=uchar(round(clamp(v[k],0.0f,1.0f)*255.0f)); return reinterpret_cast<uint&>(u); }
__forceinline uint packSnorm4x8( vec4 v ){ char4 s; for(int k=0;k<4;k++) s[k]=char(round(clamp(v[k],-1.0f,1.0f)*127.0f)); return reinterpret_cast<uint&>(s); }
__forceinline uint packHalf2x16( vec2 v ){ half2 h=ftoh(v); return reinterpret_cast<uint&>(h); }
__forceinline vec2 unpackUnorm2x16( uint u ){ vec2 v; for(int k=0;k<2;k++) v[k]=reinterpret_cast<ushort2&>(u)[k]/65535.0f; return v; }
__forceinline vec2 unpackSnorm2x16( uint u ){ vec2 v; for(int k=0;k<2;k++) v[k]=clamp(reinterpret_cast<short2&>(u)[k]/32767.0f,-1.0f,1.0f); return v; }
__forceinline vec4 unpackUnorm4x8( uint u ){ vec4 v; for(int k=0;k<4;k++) v[k]=reinterpret_cast<uchar4&>(u)[k]/255.0f; return v; }
__forceinline vec4 unpackSnorm4x8( uint u ){ vec4 v; for(int k=0;k<4;k++) v[k]=clamp(reinterpret_cast<char4&>(u)[k]/127.0f,-1.0f,1.0f); return v; }
__forceinline vec2 unpackHalf2x16( uint u ){ return htof(reinterpret_cast<half2&>(u)); }
__forceinline uint packUint4x8( uint4 v ){ return (v[0]&0xff)+((v[1]&0xff)<<8)+((v[2]&0xff)<<16)+((v[3]&0xff)<<24); }
__forceinline uvec4 unpackUint4x8( uint u ){ return uvec4(u&0xff,(u>>8)&0xff,(u>>16)&0xff,(u>>24)&0xff); }
__forceinline uint floatBitsToUint( float f ){ return reinterpret_cast<uint&>(f); }
__forceinline int floatBitsToInt( float f ){ return reinterpret_cast<int&>(f); }
__forceinline float intBitsToFloat( int i ){ return reinterpret_cast<float&>(i); }
__forceinline float uintBitsToFloat( uint u ){ return reinterpret_cast<float&>(u); }
// casting for normalized vec3 in [0,1]
__forceinline vec2 normVec3BitsToVec2( vec3 v )
{
	static const uint cap=21, hcap=10, cmask=0x1fffff, hmask=0x3ff;	// channel capacity, half capacity, channel capacity mask (=(1<<cap)-1), half capacity mask (=(1<<hcap)-1)
	uvec3 u = uvec3(uint(v.x*cmask),uint(v.y*cmask),uint(v.z*cmask));
	return vec2( uintBitsToFloat(u.x|((u.z&~hmask)<<(cap-hcap))), uintBitsToFloat((u.y<<1)|(u.z&hmask)<<(cap+1)|1) ); // bits = ( [z.11,x.21], [z.10,y.21,validity-bit] ) 
}
__forceinline vec3 vec2BitsToNormVec3( vec2 v )
{
	static const uint cap=21, hcap=10, cmask=0x1fffff, hmask=0x3ff;	// channel capacity, half capacity, channel capacity mask (=(1<<cap)-1), half capacity mask (=(1<<hcap)-1)
	uvec2 u = uvec2( floatBitsToUint(v.x), floatBitsToUint(v.y) );
	return vec3(float(u.x&cmask),float((u.y>>1)&cmask),float(((u.x&~cmask)>>(cap-hcap))|(u.y>>(cap+1))))/float(cmask);
}

//*************************************
// spline interpolations
#pragma warning( disable: 4244 )
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
#pragma warning( default: 4244 )

//*************************************
// pseudo random number generator
__forceinline uint& pseed(){ static uint seed=0; return seed; }
__forceinline void sprand( uint seed ){ pseed()=seed; }
__forceinline uint urand(){ pseed() = pseed()*214013L+2531011L; return ((pseed()>>16)&0x7fff); }
__forceinline float prand(){ return urand()/float(RAND_MAX); }
__forceinline vec2 prand2(){ return vec2(prand(),prand()); }
__forceinline vec3 prand3(){ return vec3(prand(),prand(),prand()); }
__forceinline vec4 prand4(){ return vec4(prand(),prand(),prand(),prand()); }

//*************************************
// CRC32 with 4-batch parallel construction (from zlib)
template <unsigned int poly=0x82f63b78UL> // defaulted to crc32c
__noinline inline unsigned int tcrc32( const void* buff, size_t size, unsigned int crc0=0 )
{
	if(!buff||!size) return crc0; unsigned c = ~crc0;
	static unsigned* t[4] = {nullptr}; if(!t[0]){ for(int k=0;k<4;k++) t[k]=(unsigned*) malloc(sizeof(unsigned)*256); for(int k=0;k<256;k++){ unsigned c=k; for( unsigned j=0;j<8;j++) c=c&1?poly^(c>>1):c>>1; t[0][k]=c; } for(int k=0;k<256;k++){ unsigned c=t[0][k]; for(int j=1;j<4;j++) t[j][k]=c=t[0][c&0xff]^(c>>8); } }
	const unsigned char* b=(const unsigned char*)buff;
	for(;size&&(ptrdiff_t(b)&7);size--,b++) c=t[0][(c^(*b))&0xff]^(c>>8); // move forward to the 8-byte aligned boundary
	for(;size>=4;size-=4,b+=4){c^=*(unsigned*)b;c=t[3][(c>>0)&0xff]^t[2][(c>>8)&0xff]^t[1][(c>>16)&0xff]^t[0][(c>>24)&0xff]; }
	for(;size;size--,b++)c=t[0][(c^(*b))&0xff]^(c>>8);
	return ~c;
}

//*************************************
#endif // __GX_MATH__
