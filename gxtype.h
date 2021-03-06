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
#ifndef __GX_TYPE_H__
#define __GX_TYPE_H__

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
#if (defined(_USRDLL)||defined(GX_REDIR_IMPL))&&!defined(GX_NO_REDIR)&&defined(_MSC_VER) // printf redirection in rex with VS
	#ifndef GX_REDIR
		#define GX_REDIR
	#endif
	#if defined(_INC_STDIO) && !defined(GX_REDIR_IMPL)
		#error do not include <stdio.h> or define GX_NO_REDIR before gxut headers
	#endif
	#define	printf	__printf	// rename default printf
	#define	wprintf __wprintf	// rename default wprintf
		#include <stdio.h>
		#include <wchar.h>
		#include <cstdio>
		#include <cwchar>
	#undef	printf
	#undef	wprintf
	// drop-in replacement of printf, where non-rex applications fallbacks to stdout
	int __cdecl printf( const char* fmt, ... );
	int __cdecl wprintf( const wchar_t* fmt, ... );
#else
	#include <stdio.h>
	#include <wchar.h>
	#include <cstdio>
	#include <cwchar>
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
	#include <random>
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
		#define __noinline __declspec(noinline) inline
	#endif
	#pragma warning( disable: 4201 ) // nameless struct/union
	#pragma warning( disable: 4100 ) // unreferenced formal parameter
	#pragma warning( disable: 4244 ) // int to wchar_t, possible loss of data
	#pragma warning( disable: 4324 ) // alignment padding
	#pragma warning( disable: 4458 ) // hiding class member
#else // GCC or Clang
	#ifndef __noinline
		#define __noinline __attribute__((noinline)) inline
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
// pointer type with size: waiting for a C++ standard for proposal P0901R3 (std::sized_ptr_t)
template <class T=void> struct sized_ptr_t
{
	T* ptr; size_t size;
	operator T* (){ return ptr; }	operator const T* () const { return ptr; }
	T* operator->(){ return ptr; }	const T* operator->() const { return ptr; }
}; 
// utility functions
template <class T> T*& safe_free( T*& p ){if(p) free(p); return p=nullptr; }
template <class T> T*& safe_delete( T*& p ){if(p) delete p; return p=nullptr; }
template <class T> T*& safe_release( T*& p ){if(p) p->Release(); return p=nullptr; }
// nocase base template
namespace nocase { template <class T> struct less {}; template <class T> struct equal_to {}; template <class T> struct hash {}; };
// user types
#define __default_types(n)	static const int N=n; using value_type=T; using iterator=T*; using const_iterator=const iterator; using reference=T&; using const_reference=const T&; using size_type=size_t; __forceinline operator T*(){ return (T*)this; } __forceinline operator const T*() const { return (T*)this; } constexpr iterator begin() const { return iterator(this); } constexpr iterator end() const { return iterator(this)+N; } constexpr size_t size() const { return N; }
#define __default_index(T)	__forceinline T& operator[]( ptrdiff_t i ){ return ((T*)this)[i]; } __forceinline const T& operator[]( ptrdiff_t i ) const { return ((T*)this)[i]; }
#define __default_ctors(c)	__forceinline c()=default;__forceinline c(c&&)=default;__forceinline c(const c&)=default;__forceinline c(std::initializer_list<T> l){T* p=&x;for(auto i:l)(*p++)=i;}
#define __default_assns(c)	__forceinline c& operator=(c&&)=default;__forceinline c& operator=(const c&)=default; __forceinline c& operator=(T a){ for(auto& it:*this) it=a; return *this; }
#define __default_cmps(A)	__forceinline bool operator==( const A& other ) const { return memcmp(this,&other,sizeof(*this))==0; } __forceinline bool operator!=( const A& other ) const { return memcmp(this,&other,sizeof(*this))!=0; }
#define __default_array_impl(D,T,V) __default_types(D); __default_index(T); __default_ctors(V); __default_assns(V);
template <class T> struct tarray2  { __default_array_impl(2,T,tarray2); __default_cmps(tarray2); union{struct{T x,y;};struct{T r,g;};}; };
template <class T> struct tarray3  { __default_array_impl(3,T,tarray3); __default_cmps(tarray3); using V2=tarray2<T>; union{struct{T x,y,z;};struct{T r,g,b;};union{V2 xy,rg;};struct{T _x;union{V2 yz,gb;};};}; };
template <class T> struct tarray4  { __default_array_impl(4,T,tarray4); __default_cmps(tarray4); using V2=tarray2<T>; using V3=tarray3<T>; union{struct{T x,y,z,w;};struct{T r,g,b,a;};struct{union{V2 xy,rg;};union{V2 zw,ba;};};union{V3 xyz,rgb;};struct{T _x;union{V3 yzw,gba;V2 yz,gb;};};}; };
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

#endif // __GXUT_TYPE_H__
