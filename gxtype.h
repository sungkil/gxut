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
#ifndef __GX_TYPE_H__
#define __GX_TYPE_H__

// common macros
#ifndef _CRT_SECURE_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS
#endif
#ifndef _HAS_EXCEPTIONS
	#ifdef __cpp_exceptions
		#define _HAS_EXCEPTIONS 1
	#else
		#define _HAS_EXCEPTIONS 0
	#endif
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
#if defined(GX_PRINTF_REDIR)||defined(_REXDLL) // printf redirection with custom printf/wprintf (e.g., rex GUI printf)
	#if (!defined(printf)&&!defined(__GX_PRINTF_REDIR__))&&(defined(_INC_STDIO)||defined(_INC_WCHAR)||defined(_CSTDIO_)||defined(_CWCHAR_))
		#error do not include <stdio.h> before gxut headers, when defining GX_PRINTF_REDIR
	#endif
	#ifndef printf
		#define	printf	__printf	// rename default printf
		#define	wprintf	__wprintf	// rename default wprintf
	#endif
	#include <stdio.h>
	#include <wchar.h>
	#include <cstdio>
	#include <cwchar>
	#undef	printf
	#undef	wprintf
	#ifndef __GX_PRINTF_REDIR__
		#define __GX_PRINTF_REDIR__
	#endif
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
using namespace std::string_literals; // enable s-suffix for std::string literals
#include <vector>
// C++11 (c++14/17/20: 201402L, 201703L, 202002L, ...)
#if (__cplusplus>199711L)||(defined(_MSVC_LANG)&&_MSVC_LANG>199711L) // MSVC define not  __cplusplus but _MSVC_LANG
	#include <random>
	#include <type_traits>
	#include <unordered_map>
	#include <unordered_set>
	#if (__cplusplus>=201703L)||(defined(_MSVC_LANG)&&_MSVC_LANG>=201703L)
		#include <string_view>
	#endif
	#if (__cplusplus>=202002L)||(defined(_MSVC_LANG)&&_MSVC_LANG>=202002L)
		#include <span>
	#endif
#endif

// Windows
#if defined(_WIN32)||defined(_WIN64) // Windows
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
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
	#pragma warning( disable: 4201 )	// nameless struct/union
	#pragma warning( disable: 4100 )	// unreferenced formal parameter
	#pragma warning( disable: 4244 )	// int to wchar_t, possible loss of data
	#pragma warning( disable: 4324 )	// alignment padding
	#pragma warning( disable: 4458 )	// hiding class member
	#pragma warning( disable: 6031 )	// return value ignored
	#pragma warning( disable: 26451 )	// arithmetic overflow: operator* on a 4 byte value
	#pragma warning( disable: 26495 )	// always initialize a member variable: not applicable to an anonymous struct in a union
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
template <class T=void> struct sized_ptr_t { T* ptr; size_t size; operator T* (){ return ptr; }	operator const T* () const { return ptr; } T* operator->(){ return ptr; }	const T* operator->() const { return ptr; } }; 
// dll function wrapper: load from dll and operates as a function without auto dll release
template <typename T> struct dll_function_t { HMODULE hdll=nullptr;T ptr=nullptr; dll_function_t(const wchar_t* dll,const char* func){if(hdll=LoadLibraryW(dll))ptr=T(GetProcAddress(hdll,func));} ~dll_function_t(){if(hdll){FreeLibrary(hdll);hdll=nullptr;}} operator T(){return ptr;} };
// utility functions
template <class T> T*& safe_free( T*& p ){if(p) free((void*)p); return p=nullptr; }
template <class T> T*& safe_delete( T*& p ){if(p) delete p; return p=nullptr; }
template <class T> T*& safe_release( T*& p ){if(p) p->Release(); return p=nullptr; }
#ifdef INVALID_HANDLE_VALUE
inline HANDLE& safe_close_handle( HANDLE& h ){ if(h!=INVALID_HANDLE_VALUE) CloseHandle(h); return h=INVALID_HANDLE_VALUE; }
#endif

// nocase/logical base template
namespace nocase { template <class T> struct less {}; template <class T> struct equal_to {}; template <class T> struct hash {}; };
namespace logical { template <class T> struct less {}; };
// user types
#define __default_types(n)	static const int N=n; using value_type=T; using iterator=T*; using const_iterator=const iterator; using reference=T&; using const_reference=const T&; using size_type=size_t; __forceinline operator T*(){ return (T*)this; } __forceinline operator const T*() const { return (T*)this; } constexpr iterator begin() const { return iterator(this); } constexpr iterator end() const { return iterator(this)+N; } static constexpr size_t size(){ return N; }
#define __default_index(T)	__forceinline T& operator[]( ptrdiff_t i ){ return ((T*)this)[i]; } __forceinline const T& operator[]( ptrdiff_t i ) const { return ((T*)this)[i]; }
#define __default_ctors(c)	__forceinline c()=default;__forceinline c(c&&)=default;__forceinline c(const c&)=default;__forceinline c(std::initializer_list<T> l){T* p=&x;for(auto i:l)(*p++)=i;}
#define __default_assns(c)	__forceinline c& operator=(c&&)=default;__forceinline c& operator=(const c&)=default; __forceinline c& operator=(T a){ for(auto& it:*this) it=a; return *this; }
#define __default_cmps(A)	__forceinline bool operator==( const A& other ) const { return memcmp(this,&other,sizeof(*this))==0; } __forceinline bool operator!=( const A& other ) const { return memcmp(this,&other,sizeof(*this))!=0; }
#define __default_array_impl(D,T,V) __default_types(D); __default_index(T); __default_ctors(V); __default_assns(V);
template <class T> struct tarray2  { __default_array_impl(2,T,tarray2); __default_cmps(tarray2); union{struct{T x,y;};struct{T r,g;};}; };
template <class T> struct tarray3  { __default_array_impl(3,T,tarray3); __default_cmps(tarray3); using V2=tarray2<T>; union{struct{T x,y,z;};struct{T r,g,b;};union{V2 xy,rg;};struct{T _x;union{V2 yz,gb;};};}; };
template <class T> struct tarray4  { __default_array_impl(4,T,tarray4); __default_cmps(tarray4); using V2=tarray2<T>; using V3=tarray3<T>; union{struct{T x,y,z,w;};struct{T r,g,b,a;};struct{union{V2 xy,rg;};union{V2 zw,ba;};};union{V3 xyz,rgb;};struct{T _x;union{V3 yzw,gba;V2 yz,gb;};};}; };
using uint		= unsigned int;			using uchar		= unsigned char;		using ushort	= unsigned short;	using ulong = unsigned long;
using bool2		= tarray2<bool>;		using bool3		= tarray3<bool>;		using bool4		= tarray4<bool>;
#ifndef __VECTOR_TYPES_H__ // type definitions in CUDA/vector_types.h
using float2	= tarray2<float>;		using float3	= tarray3<float>;		using float4	= tarray4<float>;
using double2	= tarray2<double>;		using double3	= tarray3<double>;		using double4	= tarray4<double>;
using char2		= tarray2<char>;		using char3		= tarray3<char>;		using char4		= tarray4<char>;
using uchar2	= tarray2<uchar>;		using uchar3	= tarray3<uchar>;		using uchar4	= tarray4<uchar>;
using short2	= tarray2<short>;		using short3	= tarray3<short>;		using short4	= tarray4<short>;
using ushort2	= tarray2<ushort>;		using ushort3	= tarray3<ushort>;		using ushort4	= tarray4<ushort>;
using int2		= tarray2<int>;			using int3		= tarray3<int>;			using int4		= tarray4<int>;
using uint2		= tarray2<uint>;		using uint3		= tarray3<uint>;		using uint4		= tarray4<uint>;
using long2		= tarray2<long>;		using long3		= tarray3<long>;		using long4		= tarray4<long>;
using ulong2	= tarray2<ulong>;		using ulong3	= tarray3<ulong>;		using ulong4	= tarray4<ulong>;
using longlong2	= tarray2<int64_t>;		using longlong3	= tarray3<int64_t>;		using longlong4	= tarray4<int64_t>;
using ulonglong2= tarray2<uint64_t>;	using ulonglong3= tarray3<uint64_t>;	using ulonglong4= tarray4<uint64_t>;
#endif

// Common unique types in std namespace
// These types should hardly have the same names in user definitions
using std::string;
using std::wstring;
using std::vector;

#endif // __GX_TYPE_H__
