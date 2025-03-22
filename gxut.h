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
#ifndef __GXUT_H__
#define __GXUT_H__

// pre-validation for essential macros
#ifndef __has_include
	#error __has_include is undefined. Use a modern C++ compiler (at the least c++17)
#endif

// platform macros
#if defined(_WIN32)&&defined(_MSC_VER)&&!defined(__clang__) // Visual Studio without clang
	#define __msvc__
#elif defined(__GNUC__)
	#define __gcc__
#endif
#ifdef __VECTOR_TYPES_H__ // type definitions in CUDA/vector_types.h
	#define __cuda__
#endif

// common macros
#ifdef __msvc__
	#ifndef _CRT_SECURE_NO_WARNINGS
		#define _CRT_SECURE_NO_WARNINGS 1
	#endif
	#ifndef _CRT_NONSTDC_NO_WARNINGS
		#define _CRT_NONSTDC_NO_WARNINGS 1
	#endif
	#ifndef _CRT_DECLARE_NONSTDC_NAMES
		#define _CRT_DECLARE_NONSTDC_NAMES 1
	#endif
	#ifndef _HAS_EXCEPTIONS
		#ifdef __cpp_exceptions
			#define _HAS_EXCEPTIONS 1
		#else
			#define _HAS_EXCEPTIONS 0
		#endif
	#endif
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
	#ifndef NOMINMAX
		#define NOMINMAX // suppress definition of min/max in <windows.h>
	#endif
	#ifndef __attribute__
		#define __attribute__(a)
	#endif
	#ifndef __noinline
		#define __noinline __declspec(noinline) inline
	#endif
	#if __has_include(<SDKDDKVer.h>)
		#include <SDKDDKVer.h>
	#endif
	#pragma optimize( "gs", on )
	#pragma check_stack( off )
	#pragma strict_gs_check( off )
	#pragma float_control(except,off)
	#pragma warning( disable: 4201 )	// nameless struct/union
	#pragma warning( disable: 4100 )	// unreferenced formal parameter
	#pragma warning( disable: 4244 )	// int to wchar_t, possible loss of data
	#pragma warning( disable: 4324 )	// alignment padding
	#pragma warning( disable: 4458 )	// hiding class member
	#pragma warning( disable: 4996 )	// POSIX name is deprecated; this is not entirely true, since posix systems use such names
	#pragma warning( disable: 6011 )	// dereferencing NULL pointer
	#pragma warning( disable: 6031 )	// return value ignored
	#pragma warning( disable: 6308 )	// realloc might return null pointer
	#pragma warning( disable: 6326 )	// potential comparison of a constant with another constant
	#pragma warning( disable: 6387 )	// could be '0':  this does not adhere to the specification for the function
	#pragma warning( disable: 26451 )	// arithmetic overflow: operator* on a 4 byte value
	#pragma warning( disable: 26495 )	// always initialize a member variable: not applicable to an anonymous struct in a union
	#pragma warning( disable: 26813 )	// use 'bitwise and' to check if a flag is set.
	#pragma warning( disable: 26819 )	// unannotated fallthrough between switch labels (es.78)
#elif defined(__gcc__)
	#ifndef __noinline
		#define __noinline __attribute__((noinline)) inline
	#endif
	#ifndef __forceinline
		#define __forceinline inline __attribute__((__always_inline__))
	#endif
	#pragma GCC diagnostic ignored "-Wconversion"
	#pragma GCC diagnostic ignored "-Wmisleading-indentation" // clause does not guard
	#pragma GCC diagnostic ignored "-Wmultichar"
	#pragma GCC diagnostic ignored "-Wunknown-pragmas"
	#pragma GCC diagnostic ignored "-Wunused-variable"
#elif defined(__clang__) // Clang
	#ifndef __noinline
		#define __noinline __attribute__((noinline)) inline
	#endif
	#ifndef __forceinline
		#define __forceinline inline __attribute__((__always_inline__))
	#endif
	#pragma clang diagnostic ignored "-Wmissing-braces"					// ignore excessive warning for initialzer
	#pragma clang diagnostic ignored "-Wdelete-non-virtual-dtor"		// ignore non-virtual destructor
	#pragma clang diagnostic ignored "-Wunused-variable"				// supress warning for unused b0
	#pragma clang diagnostic ignored "-Wunused-command-line-argument"	// e.g., /Gm-, /QPar, /FpC; clang bugs show still warnings
	#pragma clang diagnostic ignored "-Wclang-cl-pch"					// clang bugs show still warnings
#endif // __msvc__

// C standard
#include <inttypes.h> // int64_t, uint64_t, ...
#include <math.h>
#include <stdarg.h>
#if defined(GX_PRINTF_REDIR)||defined(_REXDLL) // printf redirection with custom printf
	#if (!defined(printf)&&!defined(__GX_PRINTF_REDIR__))&&(defined(_INC_STDIO)||defined(_INC_WCHAR)||defined(_CSTDIO_)||defined(_CWCHAR_))
		#error do not include <stdio.h> before gxut headers, when defining GX_PRINTF_REDIR
	#endif
	#ifndef __GX_PRINTF_REDIR__
		#define __GX_PRINTF_REDIR__
	#endif
	#ifndef printf
		#define	printf	__printf	// rename default printf
		#define	wprintf	__wprintf	// rename default printf	
	#endif
#endif
#include <stdio.h>
#include <wchar.h>
#include <wctype.h>
#include <cstdio>
#include <cwchar>
#include <stdlib.h>
#include <string.h>

// compile-time test of printf-style format string
#ifdef __msvc__
	#define __printf_format_string__	_Printf_format_string_
#elif defined(__gcc__)
	#define __printf_format_string__
#endif
// drop-in replacement of printf, where console applications fallbacks to stdout
#if defined(GX_PRINTF_REDIR)||defined(_REXDLL)
	#undef	printf
	#undef	wprintf // disable wprintf entirely
	int printf( __printf_format_string__ const char* fmt, ... );
#endif

// STL
#include <algorithm>
#include <array>
#include <deque>
#include <map>
#include <set>
#include <string>
#include <typeinfo>
#include <typeindex>
#include <vector>
using namespace std::string_literals; // enable s suffix for std::string literals
using namespace std::literals::string_literals;

// common unique types in std namespace; these types should hardly have the same names in user definitions
using std::array;
using std::string;
using std::string_view;
using std::vector;

// C++11/14/17/20: 201402L, 201703L, 202002L, ...
#if (__cplusplus>199711L)||(defined(_MSVC_LANG)&&_MSVC_LANG>199711L) // MSVC define not __cplusplus but _MSVC_LANG
	#include <chrono>	// microtimer		
	#include <functional>
	#include <filesystem> // std::filesystem
	namespace fs = std::filesystem;
	#include <random>
	#include <type_traits>
	#include <thread> // usleep
	#include <unordered_map>
	#include <unordered_set>
	#if (__cplusplus>=201703L)||(defined(_MSVC_LANG)&&_MSVC_LANG>=201703L)||(defined(_HAS_CXX17)&&_HAS_CXX17)
		#include <string_view>
	#endif
	#if (__cplusplus>=202002L)||(defined(_MSVC_LANG)&&_MSVC_LANG>=202002L)||(defined(_HAS_CXX20)&&_HAS_CXX20)
		#include <span>
	#endif
#endif

// platform-independent posix headers
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

// platform-specific header files and configurations
#ifdef __msvc__
	#include <windows.h>
	#include <direct.h>	// directory control
	#include <intrin.h>	// cpu info
	#include <io.h>		// low-level io functions
	#include <sys/utime.h>
	#define PATH_MAX _MAX_PATH // _MAX_PATH = 260 in Windows
	constexpr char preferred_separator = '\\';
	typedef struct _stat64 stat_t;
	template <class T> T*& safe_release( T*& p ){if(p) p->Release(); return p=nullptr; }
	inline HANDLE& safe_close_handle( HANDLE& h ){ if(h!=INVALID_HANDLE_VALUE) CloseHandle(h); return h=INVALID_HANDLE_VALUE; }
	template <typename T> struct dll_function_t { HMODULE hdll=nullptr;T* ptr=nullptr; dll_function_t(const char* dll,const char* func){if((hdll=LoadLibraryA(dll)))ptr=(T*)(GetProcAddress(hdll,func));} ~dll_function_t(){if(hdll){FreeLibrary(hdll);hdll=nullptr;}} operator T* (){return ptr;} }; // dll function wrapper: load from dll and operates as a function with auto dll release
#elif defined(__gcc__)
	#include <unistd.h>
	#include <linux/limits.h>
	#include <cpuid.h>
	#include <libgen.h> // basename, dirname
	#include <utime.h>
	#include <sys/wait.h>
	#include <dlfcn.h>
	#define _MAX_PATH PATH_MAX // PATH_MAX = 4096 in linux
	constexpr char preferred_separator = '/';
	typedef struct stat64 stat_t;
	template <typename T> struct dll_function_t { void* hdll=nullptr;T* ptr=nullptr; dll_function_t(const char* dll,const char* func){if((hdll=dlopen(dll,RTLD_LAZY)))ptr=(T*)(dlsym(hdll,func));} ~dll_function_t(){if(hdll){dlclose(hdll);hdll=nullptr;}} operator T* (){return ptr;} }; // dll function wrapper: load from dll and operates as a function with auto dll release	
#elif defined(__clang__)
#endif

// definitinos for posix compatibility
#if defined __msvc__
	#define ftello		_ftelli64
	#define fseeko		_fseeki64	
	#define S_IFIFO		_S_IFIFO
	#define stat64		_stat64
	#define popen		_popen
	#define pclose		_pclose
	#define strtok_r	strtok_s
#elif defined __gcc__
	#define _ftelli64	ftello
	#define _fseeki64	fseeko
	#define _S_IFIFO	S_IFIFO
	#define _stat64		stat64
	#define _popen		popen
	#define _pclose		pclose
	#define strtok_s	strtok_r
#endif

// deallocation functions
template <class T> T*& safe_free( T*& p ){if(p) free((void*)p); return p=nullptr; }
template <class T> T*& safe_delete( T*& p ){if(p) delete p; return p=nullptr; }

// compiler's utility
namespace compiler
{
	inline int monthtoi( const char* month ){ if(!month||!month[0]||!month[1]||!month[2]) return 0; char a=tolower(month[0]), b=tolower(month[1]), c=tolower(month[2]); if(a=='j'){ if(b=='a') return 1; if(c=='n') return 6; return 7; } if(a=='f') return 2; if(a=='m'){ if(c=='r') return 3; return 5; } if(a=='a'){ if(b=='p') return 4; return 8; } if(a=='s') return 9; if(a=='o') return 10; if(a=='n') return 11; return 12; }
	inline int year(){ static int y=0; if(y) return y; char buff[64]={}; int r=sscanf(__DATE__,"%*s %*s %s", buff); return y=atoi(buff); }
	inline int month(){ static int m=0; if(m) return m; char buff[64]={}; int r=sscanf(__DATE__,"%s", buff); return m=monthtoi(buff); }
	inline int day(){ static int d=0; if(d) return d; char buff[64]={}; int r=sscanf(__DATE__,"%*s %s %*s", buff); return d=atoi(buff); }
}

// shared circular string buffers
template <class T=char> __forceinline T* __strbuf( size_t len ){ static T* C[1<<14]={}; static unsigned int cid=0; cid=cid%(sizeof(C)/sizeof(C[0])); C[cid]=(T*)(C[cid]?realloc(C[cid],sizeof(T)*(len+2)):malloc(sizeof(T)*(len+2))); if(C[cid]){ C[cid][0]=C[cid][len]=C[cid][len+1]=0; } return C[cid++]; } // add one more char for convenience
template <class T=char> __forceinline T* __strdup( const T* s, size_t len ){ T* d=__strbuf<T>(size_t(len)); if(!len){ d[0]=d[1]=0; return d; } memcpy(d, s, sizeof(T)*len); d[len]=d[len+1]=0; return d; }
template <class T=char> __forceinline T* __strdup( const T* s ){ size_t l=0; const T* t=s; while(*t){t++;l++;} return __strdup<T>(s,l); }

// pointer type with size: waiting for a C++ standard for proposal P0901R3 (std::sized_ptr_t)
template <class T=void> struct sized_ptr_t { T* ptr; size_t size; operator T* (){ return ptr; }	operator const T* () const { return ptr; } T* operator->(){ return ptr; } const T* operator->() const { return ptr; } }; 

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
#ifndef __cuda__ // type definitions in CUDA/vector_types.h
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

//*************************************
namespace cpu {
//*************************************
#ifdef __msvc__
	inline uint	 __get_function_count( bool extended=false ){ uint4 n={}; uint ext=extended?0x80000000:0; __cpuid((int*)&n.x,ext); return n.x-ext; }
	inline uint4 __get_cpu_id( unsigned int level, bool extended=false ){ uint ext=extended?0x80000000:0; uint4 i; __cpuidex((int*)&i.x,ext+level,0); return i; }
	inline uint64_t memory(){ uint64_t m; return GetPhysicallyInstalledSystemMemory(&m)?m:0; }
#elif defined(__gcc__) // GCC
	inline uint  __get_function_count( bool extended=false ){ uint ext=extended?0x80000000:0; unsigned int n=__get_cpuid_max(ext,0); return n-ext; }
	inline uint4 __get_cpu_id( unsigned int level, bool extended=false ){ uint ext=extended?0x80000000:0; uint4 i; __get_cpuid(ext+level,&i.x,&i.y,&i.z,&i.w); return i; }
#endif

__noinline const uint4 info( unsigned int level, bool extended=false )
{
	static vector<uint4> i[2]; // basic, extended
	auto& v = i[extended?1:0]; if(!v.empty()) return v[level];
	uint kn = __get_function_count(extended); if(kn<4){ return uint4{}; } v.resize(kn);
	for( unsigned int k=0; k<kn; k++ ) v[k]=__get_cpu_id(k,extended);
	return v[level];
}
__noinline const char* vendor(){ static uint4 s={}; if(!s.x){ auto v=info(0); s.x=v.y; s.y=v.z; s.z=v.w; } return (const char*)&s.x; }
__noinline const char* brand(){ static uint4 s[4]={}; if(s[0].x) return (const char*)&s[0]; s[0]=info(2,true); s[1]=info(3,true); s[2]=info(4,true); return (const char*)&s[0]; }
__noinline bool has_sse42(){ return ((info(1).z>>20)&1)==1; }

//*************************************
} // namespace cpu
//*************************************

// crc32 with 4-batch parallel construction (from zlib implementation)
template <unsigned int poly=0x82f63b78UL> // defaulted to crc32c; zlib/deflate use 0xedb88320UL
__noinline unsigned int crc32( unsigned int crc0, const void* ptr, size_t size )
{
	if(!ptr||!size){ return crc0; }
	static unsigned* t[4]={}; if(!t[0]){ for(int k=0;k<4;k++) t[k]=(unsigned*) malloc(sizeof(unsigned)*256); for(int k=0;k<256;k++){ unsigned v=k; for( unsigned j=0;j<8;j++) v=v&1?poly^(v>>1):v>>1; t[0][k]=v; } for(int k=0;k<256;k++){ unsigned v=t[0][k]; for(int j=1;j<4;j++) t[j][k]=v=t[0][v&0xff]^(v>>8); } }
	unsigned c = ~crc0; const unsigned char* b=(const unsigned char*)ptr;
	for(;size&&(ptrdiff_t(b)&7);size--,b++) c=t[0][(c^(*b))&0xff]^(c>>8); // move forward to the 8-byte aligned boundary
	for(;size>=4;size-=4,b+=4){c^=*(unsigned*)b;c=t[3][(c>>0)&0xff]^t[2][(c>>8)&0xff]^t[1][(c>>16)&0xff]^t[0][(c>>24)&0xff]; }
	for(;size;size--,b++)c=t[0][(c^(*b))&0xff]^(c>>8);
	return ~c;
}

#ifdef __msvc__
inline bool is_utf8( const char * s ) // https://stackoverflow.com/questions/28270310/how-to-easily-detect-utf8-encoding-in-the-string
{
	if(!s||!*s) return true; const unsigned char* b=(const unsigned char*)s;
	int n; while(*b!=0x00)
	{
		if((*b&0x80)==0x00) n=1;		// U+0000 to U+007F
		else if((*b&0xE0)==0xC0) n=2;	// U+0080 to U+07FF
		else if((*b&0xF0)==0xE0) n=3;	// U+0800 to U+FFFF
		else if((*b&0xF8)==0xF0) n=4;	// U+10000 to U+10FFFF
		else return false;
		b++; for(int k=1;k<n;k++){ if((*b&0xC0)!=0x80) return false; b++; }
	}
	return true;
}

inline const wchar_t* atow( const char* a ){ if(!a) return nullptr; if(!*a) return L""; uint cp=is_utf8(a)?CP_UTF8:0; int l=MultiByteToWideChar(cp,0,a,-1,0,0); wchar_t* w=__strbuf<wchar_t>(l); MultiByteToWideChar(cp,0,a,-1,w,l); return w; }
inline const char* wtoa( const wchar_t* w ){ if(!w) return nullptr; if(!*w) return ""; int l=WideCharToMultiByte(0,0,w,-1,0,0,0,0); char* a=__strbuf(l); WideCharToMultiByte(0,0,w,-1,a,l,0,0); return a; }
inline const char* atoa( const char* src, uint src_cp, uint dst_cp ){ if(!src) return nullptr; if(!*src) return ""; if(src_cp==dst_cp) return __strdup(src); int l=MultiByteToWideChar(src_cp,0,src,-1,0,0); wchar_t* w=__strbuf<wchar_t>(l); MultiByteToWideChar(src_cp,0,src,-1,w,l); l=WideCharToMultiByte(dst_cp,0,w,-1,0,0,0,0); char* a=__strbuf(l); WideCharToMultiByte(dst_cp,0,w,-1,a,l,0,0); return a; }
inline bool ismbs( const char* s ){ if(!s||!*s) return false; for(int k=0,kn=int(strlen(s));k<kn;k++,s++) if(*s<0)return true; return false; }
#else
inline const wchar_t* atow( const char* a ){ if(!a) return nullptr; if(!*a) return L""; const char* p=a; size_t l=mbsrtowcs(0,&p,0,0); wchar_t* b=__strbuf<wchar_t>(l); mbstate_t s={}; mbsrtowcs(b,&p,l+1,&s); return b; }
inline const char* wtoa( const wchar_t* w ){ if(!w) return nullptr; if(!*w) return ""; const wchar_t* p=w; size_t l=wcsrtombs(0,&p,0,0); char* b=__strbuf(l); mbstate_t s={}; wcsrtombs(b,&p,l+1,&s); return b; }
#endif

// auto conversion between const wchar_t* and const char*
template<class T, class U> __forceinline T*	__strdup( const U* s );
template<> __forceinline char*		__strdup<char,char>( const char* s ){ return __strdup(s); }
template<> __forceinline char*		__strdup<char,wchar_t>( const wchar_t* s ){ return (char*)wtoa(s); }
template<> __forceinline wchar_t*	__strdup<wchar_t,char>( const char* s ){ return (wchar_t*)atow(s); }
template<> __forceinline wchar_t*	__strdup<wchar_t,wchar_t>( const wchar_t* s ){ return __strdup(s); }

// define overloaded functions for wchar_t
inline size_t strlen( const wchar_t* _Str ){ return wcslen(_Str); }
inline wchar_t* strcpy( wchar_t* _Dest, const wchar_t* _Src ){ return wcscpy(_Dest,_Src); }
inline wchar_t* strncpy( wchar_t* _Dest, const wchar_t* _Src, size_t _Count ){ return wcsncpy(_Dest,_Src,_Count); }
inline wchar_t* strcat( wchar_t* _Dest, const wchar_t* _Src ){ return wcscat(_Dest,_Src);  }
inline wchar_t* strncat( wchar_t* _Dest, const wchar_t* _Src, size_t _Count ){ return wcsncat(_Dest,_Src,_Count); }
inline int strcmp( const wchar_t* _Str1, const wchar_t* _Str2 ){ return wcscmp(_Str1,_Str2); }
inline int strncmp( const wchar_t* _Str1, const wchar_t* _Str2, size_t _MaxCount ){ return wcsncmp(_Str1,_Str2,_MaxCount); }
inline const wchar_t* strchr( const wchar_t* _Str, int _Val ){ return wcschr(_Str,wchar_t(_Val)); }
inline wchar_t* strchr( wchar_t* _Str, int _Val ){ return wcschr(_Str,wchar_t(_Val)); }
inline const wchar_t* strstr( const wchar_t* _Str1, const wchar_t* _Str2 ){ return wcsstr(_Str1,_Str2); }
inline wchar_t* strstr( wchar_t* _Str1, const wchar_t* _Str2 ){ return wcsstr(_Str1,_Str2); }
inline size_t strspn( const wchar_t* _Str, const wchar_t* _Control ){ return wcsspn(_Str,_Control); }
inline size_t strcspn( const wchar_t* _Str, const wchar_t* _Control ){ return wcscspn(_Str,_Control); }
inline const wchar_t * strpbrk( const wchar_t* _Str, const wchar_t* _Control ){ return wcspbrk(_Str,_Control); }
inline wchar_t * strpbrk( wchar_t* _Str, const wchar_t* _Control ){ return wcspbrk(_Str,_Control); }

// GCC extensions
#ifdef __gcc__
#define stricmp	strcasecmp
#define wcsicmp	wcscasecmp
inline char* strlwr( char* _String ){ for( char* p=_String; *p; p++ ){ *p=tolower(*p); } return _String; }
inline char* strupr( char* _String ){ for( char* p=_String; *p; p++ ){ *p=toupper(*p); } return _String; }
inline wchar_t* wcslwr( wchar_t* _String ){ for( wchar_t* p=_String; *p; p++ ){ *p=tolower(*p); } return _String; }
inline wchar_t* wcsupr( wchar_t* _String ){ for( wchar_t* p=_String; *p; p++ ){ *p=toupper(*p); } return _String; }
inline int wtoi( const wchar_t* _String ){ return int(wcstol(_String,0,10)); }
inline int strnicmp( const char* s1, const char* s2, size_t n ){ char *l1=strlwr(__strdup(s1)), *l2=strlwr(__strdup(s2)); return strncmp(l1,l2,n); }
#endif

// vcpp extensions
inline wchar_t* strlwr( wchar_t* _Str ){ return wcslwr(_Str); }
inline wchar_t* strupr( wchar_t* _Str ){ return wcsupr(_Str); }
inline int stricmp( const wchar_t* _Str1, const wchar_t* _Str2 ){ return wcsicmp(_Str1,_Str2); }
#ifdef __msvc__
inline wchar_t* strtok_s( wchar_t* _Str, const wchar_t* _Delim, wchar_t** context ){ return wcstok_s(_Str,_Delim,context); }
#endif

// slee extensions
template <class T> size_t strrspn( const T* _Str, const T* _Control ){ size_t L=strlen(_Str),C=strlen(_Control),k=0,j=0;for(k=0;k<L;k++){for(j=0;j<C;j++)if(_Str[L-1-k]==_Control[j])break;if(j==C)break;}return k; }
inline const char*    stristr( const char* _Str1, size_t l1, const char* _Str2, size_t l2 ){ char* s1=__strdup(_Str1,l1); strlwr(s1); char* s2=__strdup(_Str2,l2); strlwr(s2); const char* r=strstr(s1,s2); return r?(_Str1+(r-s1)):nullptr; }
inline const wchar_t* wcsistr( const wchar_t* _Str1, size_t l1, const wchar_t* _Str2, size_t l2 ){ wchar_t* s1=__strdup(_Str1,l1); wcslwr(s1); wchar_t* s2=__strdup(_Str2,l2); wcslwr(s2); const wchar_t* r=wcsstr(s1,s2); return r?(_Str1+(r-s1)):nullptr; }
inline const char*    stristr( const char* _Str1, const char* _Str2 ){ return stristr(_Str1, strlen(_Str1), _Str2, strlen(_Str2)); }
inline const wchar_t* wcsistr( const wchar_t* _Str1, const wchar_t* _Str2 ){ return wcsistr(_Str1, wcslen(_Str1), _Str2, wcslen(_Str2)); }
inline const wchar_t* stristr( const wchar_t* _Str1, const wchar_t* _Str2 ){ return wcsistr(_Str1, wcslen(_Str1), _Str2, wcslen(_Str2)); }
// natural-order and case-insensitive comparison for std::sort, std::map/set, std::unordered_map/set
#if defined __msvc__
	#ifdef _INC_SHLWAPI
		inline int strcmplogical( const wchar_t* _Str1, const wchar_t* _Str2 ){ return (!_Str1||!_Str2)?0:StrCmpLogicalW(_Str1,_Str2); }
	#else
		inline int strcmplogical( const wchar_t* _Str1, const wchar_t* _Str2 ){ static dll_function_t<int(const wchar_t*,const wchar_t*)> f("shlwapi.dll","StrCmpLogicalW"); return !f?wcsicmp(_Str1,_Str2):(!_Str1||!_Str2)?0:f(_Str1,_Str2); } // load StrCmpLogicalW(): when unavailable, fallback to wcsicmp
	#endif
	inline int strcmplogical( const char* _Str1, const char* _Str2 ){ return strcmplogical(atow(_Str1),atow(_Str2)); }
#endif

// format and printf replacement
inline const char*		vformat( __printf_format_string__ const char* fmt, va_list a ){ size_t len=size_t(vsnprintf(0,0,fmt,a)); char* buffer=__strbuf(len); vsnprintf(buffer,len+1,fmt,a); return buffer; }
inline const wchar_t*	vformat( __printf_format_string__ const wchar_t* const fmt, va_list a ){ size_t len=size_t(vswprintf(0,0,fmt,a)); wchar_t* bufferW=__strbuf<wchar_t>(len); vswprintf(bufferW,len+1,fmt,a); return bufferW; }
inline const char*		__attribute__((format(printf,1,2))) format( __printf_format_string__ const char* fmt, ... ){ va_list a; va_start(a,fmt); size_t len=size_t(vsnprintf(0,0,fmt,a)); char* buffer=__strbuf(len); vsnprintf(buffer,len+1,fmt,a); va_end(a); return buffer; }
inline const wchar_t*	format( __printf_format_string__ const wchar_t* fmt, ... ){ va_list a; va_start(a,fmt); size_t len=size_t(vswprintf(0,0,fmt,a)); wchar_t* bufferW=__strbuf<wchar_t>(len); vswprintf(bufferW,len+1,fmt,a); va_end(a); return bufferW; }

// case conversion
template <class T> const T* tolower( const T* src ){ return strlwr(__strdup(src)); }
template <class T> const T* toupper( const T* src ){ return strupr(__strdup(src)); }
inline const char* tovarname( const char* src, bool to_upper=false ){ if(!src||!*src) return ""; char *s=(char*)src,*dst=__strbuf(strlen(src)+2),*d=dst; if(!isalpha(*s)&&(*s)!='_') *(d++)='_'; for(;*s;s++,d++) *d=isalnum(*s)?(to_upper?char(toupper(*s)):(*s)):'_'; *d='\0'; return dst; }
inline const char* tovarname( const wchar_t* src, bool to_upper=false ){ return tovarname(wtoa(src),to_upper); }

// string function for path
#define __add_separator(src,sep) size_t l=src.size(); if(l==0)return __strdup(src.data()); char* s=strcpy(__strbuf(l+1),src.data()); if(src[l-1]!='/'&&src[l-1]!='\\'){s[l]=sep;s[l+1]=0;} return s
#define __to_separator(src,sep) char* s=(char*)__strdup(src.data()); if(!s||!*s) return s; for(auto* p=s;*p;p++) if(*p=='/'||*p=='\\') *p=sep; return s
inline const char* append_slash( string_view src ){ __add_separator(src,preferred_separator); }
inline const char* to_backslash( string_view src ){ __to_separator(src,'\\'); }
inline const char* to_slash( string_view src ){ __to_separator(src,'/'); }
inline const char* to_preferred( string_view src ){ __to_separator(src,preferred_separator); }
#undef __add_separator
#undef __to_separator

// global path definitions/functions
__noinline const char* cwd(){ static char c[_MAX_PATH]={}; getcwd(c, _MAX_PATH); size_t l=strlen(c); if(*c&&c[l-1]!=preferred_separator){ c[l]=preferred_separator; c[l+1]=0; } return c; } // current working directory

// time helper functions
__forceinline time_t now(){ return time(0); }
__forceinline time_t time_offset( int days, int hours=0, int mins=0, int secs=0 ){ return 1ll*secs + 60ll*mins + 3600ll*hours + 86400ll*days; } // time_t in second scale: 1 is one second
__forceinline bool	 time_greater( time_t t0, time_t t1, time_t offset=30 ){ return t0>t1+offset; } // server-local difference can be up to several seconds; do not make time_less(), which causes confusion in use cases
__forceinline const char* asctime( time_t t ){ return __strdup(::asctime(gmtime(&t))); }	

// file pointer helpers
inline bool		is_fifo( FILE* fp ){ if(!fp) return false; struct stat s; return fstat(fileno(fp),&s)==0?(s.st_mode&S_IFIFO?true:false):false; } // posix-like or std::filesystem-like utilities
inline size_t	file_size( FILE* fp ){ if(!fp) return 0; auto pos=_ftelli64(fp); _fseeki64(fp,0,SEEK_END); size_t s=_ftelli64(fp); _fseeki64(fp,pos,SEEK_SET); return s; }

// early path functions
#ifdef __msvc__
// https://man7.org/linux/man-pages/man3/basename.3.html
inline const char* basename( const char* path )
{
	if(!path) return "";  if(!*path) return "";
	int l=int(strlen(path)); char* s=__strdup(path,l); if(l==1&&(*s=='/'||*s=='\\')) return s;
	if((s[l-1]=='/'||s[l-1]=='\\')&&s[l-2]!=':') s[--l]=0;
	for(int k=l-1,kn=(isalpha(s[0])&&s[1]==':')?2:0;k>=kn;k--){ if(s[k]=='/'||s[k]=='\\') return s+k+1; }
	return s;
}
inline const char* dirname( const char* path )
{
	if(!path) return ""; if(!*path) return ".";
	int l=int(strlen(path)); char* s=__strdup(path,l); if(l==1&&(*s=='/'||*s=='\\')) return s;
	if((s[l-1]=='/'||s[l-1]=='\\')&&s[l-2]!=':') s[--l]=0;
	for(int k=l-1,kn=(isalpha(s[0])&&s[1]==':')?2:0;k>=kn;k--){ if(s[k]=='/'||s[k]=='\\'){ s[k>kn?k:k+1]=0; return s; } }
	return ".";
}
#elif defined __gcc__
inline int mkdir( const char* path ){ return mkdir(path,0776); }
#endif

// path decomposition
struct __pathinfo { char *dir=nullptr, *stem=nullptr, *x=nullptr; };
__noinline __pathinfo __split_path( const char* path, bool b_dir, bool b_stem, bool b_extension )
{
#ifdef __msvc__
	size_t dl = b_dir?PATH_MAX:0;		wchar_t* d=dl?__strbuf<wchar_t>(dl):0;
	size_t il = b_dir?_MAX_DIR:0;		wchar_t* i=il?__strbuf<wchar_t>(il):0;
	size_t sl = b_stem?_MAX_FNAME:0;	wchar_t* s=sl?__strbuf<wchar_t>(sl):0;
	size_t xl = b_extension?_MAX_EXT:0;	wchar_t* x=xl?__strbuf<wchar_t>(xl):0;
	_wsplitpath_s(atow(path),d,dl,i,il,s,sl,x,xl);
	if(d&&*d) *d=toupper(*d); d=d&&i?strcat(d,i):d?d:i;
	return __pathinfo{d?(char*)wtoa(d):0,s?(char*)wtoa(s):0,x?(char*)wtoa(x):0};
#elif defined __gcc__
	size_t l=strlen(path);
	if(path[l-1]=='/'||path[l-1]=='\\') return __pathinfo{b_dir?__strdup(path):(char*)"",0,0}; // trailing slash means just a directory
	const char* d = b_dir?__strdup(dirname((char*)__strdup(path))):nullptr; string b = basename((char*)__strdup(path));
	const char* s = b_stem?__strdup(fs::path(b.c_str()).stem().c_str()):nullptr;
	const char* x = b_extension?__strdup(fs::path(b.c_str()).extension().c_str()):nullptr; if(*x=='.') x++;
	return __pathinfo{(char*)d,(char*)s,(char*)x};
#endif
}

//*************************************
namespace exe {
//*************************************
#ifdef __msvc__
inline const char* path(){	static char e[PATH_MAX+1]={}; if(*e) return e; GetModuleFileNameA(nullptr,e,sizeof(e)-1); return e; }
#elif defined(__gcc__)
inline const char* path(){	static char e[PATH_MAX+1]={}; if(*e) return e; if(readlink("/proc/self/exe",e,sizeof(e)-1)>0) return ""; return e; }
#endif
inline const char* dir(){	static char d[PATH_MAX]={}; if(*d) return d; return strcpy(d,__split_path(path(),true,false,false).dir); }
inline const char* name(){	static char n[PATH_MAX]={}; if(*n) return n; return strcpy(n,__split_path(path(),false,true,false).stem); }
//*************************************
} // end namespace exe
//*************************************

struct path_t
{
	static constexpr int capacity = PATH_MAX;
	using value_type		= char; // posix convention for std::filesystem::path
	using string_type		= std::basic_string<value_type>;
	using string_view_type	= std::basic_string_view<value_type>;
	using iterator			= value_type*;
	using const_iterator	= const value_type*;
	using attrib_t			= stat_t; // platform-indepdent alternative to WIN32_FILE_ATTRIBUTE_DATA in Windows

	// destructor/constuctors
	~path_t() noexcept { if(!_data) return; free(_data); _data=nullptr; }
	path_t() noexcept { __alloc(); memset(_data+capacity,0,sizeof(attrib_t)); }
	path_t( const path_t& p ) noexcept { strcpy(__alloc(),p._data); memcpy(_data+capacity,p._data+capacity,sizeof(attrib_t)); }
	path_t( path_t&& p ) noexcept { _data=p._data; p._data=nullptr; } // cache moves as well
	path_t( const value_type* s ) noexcept : path_t() { if(s) strcpy(_data,s); }
	path_t( const string_type& s ) noexcept : path_t() { strcpy(_data,s.c_str()); }
	path_t( string_view_type s ) noexcept : path_t() { strcpy(_data,s.data()); }

	// operator overloading: casting and conversion
	value_type* data(){ return _data; }
	const value_type* c_str() const { return _data; }
	operator string_type () const { return string_type{_data,size()}; }
	operator string_view_type () const { return string_view_type{_data,size()}; }

	// operator overloading: assignment
	path_t& operator=( path_t&& p ) noexcept { if(_data) free(_data); _data=p._data; p._data=nullptr; return *this; }
	path_t& operator=( const path_t& p ) noexcept { strcpy(_data,p._data); memcpy(_data+capacity,p._data+capacity,sizeof(attrib_t)); return *this; }
	path_t& operator=( const value_type* s ) noexcept { if(s) strcpy(_data,s); return *this; }
	path_t& operator=( const string_type& s ) noexcept { strcpy(_data,s.c_str()); return *this; }
	path_t& operator=( string_view_type s ) noexcept{ strcpy(_data,s.data()); return *this; }
	path_t& operator=( value_type c ){ _data[0]=c; _data[1]=0; return *this; }

	// operator overloading: concatenations
	path_t operator+( const path_t& p ) const { return path_t(this)+=p; }
	path_t operator+( const value_type* s ) const { return path_t(this)+=s; }
	path_t operator+( const string_type& s ) const { return path_t(this)+=s; }
	path_t operator+( string_view_type s ) const { return path_t(this)+=s; }
	path_t operator+( value_type c ) const { return path_t(this)+=c; }
	path_t operator/( const path_t& p ) const { return append_slash()+p; }
	path_t operator/( const value_type* s ) const { return append_slash()+s; }
	path_t operator/( const string_type& s ) const { return append_slash()+s; }
	path_t operator/( string_view_type s ) const { return append_slash()+s; }
	path_t operator/( value_type c ) const { return append_slash()+c; }
	path_t& operator+=( const path_t& p ){ return operator+=(p._data); }
	path_t& operator+=( const value_type* s ){ strcpy(end(),s+((s[0]=='.'&&s[2]&&__is_separator(s[1]))?2:0)); return *this; }
	path_t& operator+=( const string_type& s ){ return operator+=(s.c_str()); }
	path_t& operator+=( string_view_type s ){ return operator+=(s.data()); }
	path_t& operator+=( value_type c ){ size_t l=strlen(_data); _data[l]=c; _data[l+1]=0; return *this; }
	path_t& operator/=( const path_t& p ){ return *this=operator/(p); }
	path_t& operator/=( const value_type* s ){ return *this=operator/(s); }
	path_t& operator/=( const string_type& s ){ return *this=operator/(s); }
	path_t& operator/=( string_view_type s ){ return *this=operator/(s); }
	path_t& operator/=( value_type c ){ return *this=operator/(c); }
	
	// operator overloading: array access
	inline value_type& operator[]( ptrdiff_t i ){ return _data[i]; }
	inline const value_type& operator[]( ptrdiff_t i ) const { return _data[i]; }

	// operator overloading: comparisons
	__forceinline bool operator==( const path_t& p ) const { return stricmp(_data,p._data)==0; }
	__forceinline bool operator==( const value_type* s ) const { return stricmp(_data,s)==0; }
	__forceinline bool operator==( const string_type& p ) const { return stricmp(_data,p.c_str())==0; }
	__forceinline bool operator==( string_view_type p ) const { return stricmp(_data,p.data())==0; }
	__forceinline bool operator!=( const path_t& p ) const { return stricmp(_data,p._data)!=0; }
	__forceinline bool operator!=( const value_type* s ) const { return stricmp(_data,s)!=0; }
	__forceinline bool operator!=( const string_type& p ) const { return stricmp(_data,p.c_str())!=0; }
	__forceinline bool operator!=( string_view_type p ) const { return stricmp(_data,p.data())!=0; }

	// iterators
	iterator begin(){ return _data; }
	const iterator begin() const { return _data; }
	iterator end(){ return _data+size(); }
	const iterator end() const { return _data+size(); }
	value_type& front(){ return *_data; }
	const value_type& front() const { return *_data; }
	value_type& back(){ size_t l=size(); return l?_data[l-1]:_data[0]; }
	const value_type& back() const { size_t l=size(); return l?_data[l-1]:_data[0]; }

	// data queries
	__forceinline void clear() const { _data[0]=0; memset(_data+capacity,0,sizeof(attrib_t)); }
	__forceinline bool empty() const { return _data[0]==0; }
	__forceinline size_t size() const { return strlen(_data); }

	// separator opertions
	path_t to_slash()		const { return __to_separator(*this,'/'); }
	path_t to_backslash()	const { return __to_separator(*this,'\\'); }
	path_t to_preferred()	const { return __to_separator(*this,preferred_separator); }
	path_t append_slash()	const { return __append_separator(*this,preferred_separator); }
	path_t prepend_dot()	const { return *_data=='.'?*this:path_t(".")+preferred_separator+_data; }
	path_t trim_slash()		const { path_t p(*this); if(!*_data) return p; size_t l=p.size(); if(__is_separator(p._data[l-1])) p._data[l-1]=0; return p; }
	path_t trim_dot()		const { return (strlen(_data)>2&&_data[0]=='.'&&__is_separator(_data[1])) ? _data+2 : _data; }

	// query on non-local urls
	bool is_pipe() const {		return strcmp(_data,"-")==0||strncmp(_data,"pipe:",5)==0; }
	bool is_fifo() const {		if(empty()) return false; auto& a=__attrib(); return (a.st_mode&_S_IFIFO)!=0; } // as posix/std::filesystem does
	bool is_unc() const {		return __is_separator(_data[0])&&__is_separator(_data[1]); }
	bool is_ssh() const {		if(!_data[0]||!_data[1]) return false; return strstr(_data+2, ":/")!=nullptr||strstr(_data+2, ":\\")!=nullptr; }
	bool is_http() const {		return strncmp(_data,"http://",7)==0||strncmp(_data,"https://",8)==0; }
	bool is_remote() const {	return is_ssh()||is_http(); }

	// path structure query
	bool exists()		const {	return *_data&&access(_data,0)==0; }
	bool is_dir()		const {	if(!*_data) return false; auto m=__attrib().st_mode; return (m&S_IFDIR)!=0; }
	bool is_readonly()	const {	if(!*_data) return false; auto m=__attrib().st_mode; return (m&S_IFMT)&&(m&S_IREAD)&&!(m&S_IWRITE); }
#ifdef __msvc__
	bool is_absolute()	const {	if(empty()) return false; return _data[1]==':'||is_unc()||is_remote(); }
#elif defined __gcc__
	bool is_absolute()	const {	if(empty()) return false; return _data[0]=='/'||is_unc()||is_remote(); }
#endif
	bool is_relative()	const {	if(empty()) return false; return !is_pipe()&&!is_absolute(); }

	// decompositions
	path_t unc_root()	const { if(!is_unc()) return ""; path_t r=to_preferred(); auto* b=(value_type*)strpbrk(r._data+2, "/\\"); if(b) b[0]=0; return r; } // similar to drive (but to the root unc path without backslash)
	vector<path_t> ancestors( path_t root="" ) const;
	path_t relative( path_t from="" ) const;
	path_t absolute( path_t base="" ) const;
#ifdef __msvc__
	path_t dir()		const { if(empty()) return ""; if(is_unc()){ path_t r=unc_root(); size_t rl=r.size(); if(size()<=rl+1){ if(!__is_separator(r._data[rl-1])){ r._data[rl]=preferred_separator; r._data[rl+1]=0; } return r; } } const char* d=__split_path(_data,true,false,false).dir; return (d&&*d)?d:string(".")+preferred_separator; }
	path_t dirname()	const { return strpbrk(_data,"/\\")?dir().trim_slash().filename():""; }
	path_t filename()	const { auto s=__split_path(_data,false,true,true); if(!s.x||!*s.x) return s.stem; return strcat(strcpy(__strbuf(capacity),s.stem),s.x); }
	path_t stem()		const { return __split_path(_data,false,true,false).stem; } // filename without extension
	path_t extension()	const { auto s=__split_path(_data,false,false,true); return *s.x=='.'?s.x+1:""; } // alias to extension
	path_t parent()		const { return dir().trim_slash().dir(); }
	path_t remove_extension() const { auto s=__split_path(_data,true,true,false); return s.dir?strcat(strcpy(__strbuf(capacity),s.dir),s.stem):s.stem; }
	path_t replace_extension( path_t x ) const { if(x.empty()) return *this; return remove_extension()+(x[0]=='.'?x._data:"."s+x._data); }
#elif defined(__gcc__)
	path_t dir()		const { if(empty()) return ""; return fs::path(_data).remove_filename().c_str(); }
	path_t filename()	const { if(empty()) return ""; return fs::path(_data).filename().c_str(); }
	path_t stem()		const { if(empty()) return ""; return fs::path(_data).stem().c_str(); } // filename without extension
	path_t extension()	const { if(empty()) return ""; path_t x=fs::path(_data).extension().c_str(); return !x.empty()&&x.front()=='.'?x._data+1:x; }
#endif

	// file/directory operations
	int		chdir(){ return ::chdir(_data); }
	bool	mkdir()		const; // make all super directories recursively

	// custom attributes and time functions
	uint64_t	file_size() const { return uint64_t(__attrib().st_size); }
	time_t		mtime() const { auto& a=__attrib(); return (a.st_mode&S_IFMT)?a.st_mtime:0; }
	bool		utime( time_t mtime ) const { auto& a=__attrib(); if(!(a.st_mode&S_IFMT)) return false; utimbuf u={a.st_atime,mtime}; return ::utime(_data,&u )==0; } // set file modification time, while keeping access time
	string		key() const;

protected:

	value_type* _data;

	path_t( const path_t* p ) noexcept : path_t(p?*p:""){}
	value_type*		__alloc(){ static constexpr size_t s=capacity*sizeof(value_type)+sizeof(attrib_t); _data=(value_type*)malloc(s); if(_data) _data[0]=0; return _data; }
	const attrib_t&	__attrib() const { auto* a=(attrib_t*)(_data+capacity); if(!*_data||0!=stat64(_data,a)) memset(a,0,sizeof(attrib_t)); return *a; } // stat64()==ENOENT: not found; stat64()==EINVAL: invalid parameter
	static __forceinline bool __is_separator( value_type c ){ return c=='/'||c=='\\'; }
	static __forceinline path_t __to_separator( const path_t& p, value_type sep=preferred_separator ){ value_type* t=p._data; if(!*t) return p; size_t l=p.size(); for(size_t k=0; k<l; k++, t++) if(__is_separator(*t)) *t=sep; return p; }
	static __forceinline path_t __append_separator( const path_t& p, value_type sep=preferred_separator ){ value_type* t=p._data; if(!*t) return p; size_t l=p.size(); if(!__is_separator(t[l-1])){t[l]=sep;t[l+1]=0;} return p; }

	// friend class/function definitions
#if __has_include("gxfilesystem.h") && !defined(__gcc__) && !defined __NO_PATHIMPL__
	friend struct path;
#endif
};

__noinline vector<path_t> path_t::ancestors( path_t root ) const
{
	if(empty()) return {};
	if(root.empty()) root=is_unc()?unc_root():exe::dir();
	path_t d=dir(); int l=int(d.size()),rl=int(root.size());
	bool r=strnicmp(d._data,root._data,rl)==0;
	vector<path_t> a; a.reserve(4); for(int k=l-1,e=r?rl-1:0;k>=e;k--){ if(!__is_separator(d[k])) continue; d._data[k+1]=0; a.emplace_back(d); }
	return a;
}

__noinline bool path_t::mkdir() const
{
	if(empty()) return false; if(exists()) return true;
	auto v = to_preferred().dir().ancestors(); if(v.empty()) return false;
	auto bl = v.back().size();
	if(is_unc()){ auto r=unc_root(); size_t rl=r.size();if(bl<=rl+1){v.pop_back();bl=v.back().size();}if(bl<=rl+1)v.pop_back(); }
	else if(bl<=3){ if(v.back()[1]==':')v.pop_back();else if(bl<=1)v.pop_back(); }
	for( auto it=v.rbegin(); it!=v.rend(); it++ ){ if(!it->exists()&&::mkdir(it->_data)!=0) return false; }
	return true;
}

__noinline string path_t::key() const
{
	if(empty()) return "";
	size_t l=size(); char* d=__strbuf(l);
	size_t n=0; for(size_t k=0;k<l; k++){ char c=_data[k]; if(c!=':'&&c!=' ') d[n++]=__is_separator(c)?'.':(::tolower(c)); }
	d[(d[n-1]=='.')?(n-1):n]=0; return d;
}

__noinline path_t path_t::absolute( path_t base ) const
{
	if(empty()) return "";
#ifdef __msvc__
	// do not directly return for non-canonicalized path
	return _fullpath(__strbuf(capacity),(base.empty()||is_absolute())?_data:(base/_data)._data,capacity);
#elif defined __gcc__
	path_t p; if(!base.empty()&&is_absolute()) p=base.append_slash();
	path_t r; return realpath((p/_data).c_str(),r.data())?r:"";
#endif
}

__noinline path_t path_t::relative( path_t from ) const
{
	if(empty()||is_pipe()||is_ssh()||is_remote()||is_relative()) return *this;
	path_t df = from.empty()?cwd():from.dir().absolute(); if(tolower(df[0])!=tolower(_data[0])) return *this; // different drive
	path_t dt = dir().absolute();

	// explode directories
	vector<path_t> vf; vf.reserve(8); for( char *ctx=nullptr, *k=strtok_s(df._data,"/\\", &ctx); k; k=strtok_s(0,"/\\", &ctx)) vf.emplace_back(k);
	vector<path_t> vt; vt.reserve(8); for( char *ctx=nullptr, *k=strtok_s(dt._data,"/\\", &ctx); k; k=strtok_s(0,"/\\", &ctx)) vt.emplace_back(k);

	// traverse across different directory levels
	path_t r; size_t f=0,t=0,zf=vf.size(),zt=vt.size();	for(; f<zf&&t<zt; f++, t++ ){ if(vf[f]!=vt[t]) break; }
	static const path_t dd="..";						for(; f<zf; f++ ) r += dd + preferred_separator;
														for(; t<zt; t++ ) r += vt[t] + preferred_separator;
	return __is_separator(back())?r:r+filename();
}

__noinline path_t apptemp()
{
	static path_t d; if(!d.empty()) return d;
#ifdef __msvc__
	d=getenv("LOCALAPPDATA"); if(d.empty()) GetTempPathA(PATH_MAX,d.data()); if(d.empty()) return "";
#elif defined __gcc__
	d=getenv("TMPDIR"); if(d.empty()) d=getenv("TEMP"); if(d.empty()) d=getenv("TMP"); if(d.empty()) return "";
#endif
	d=d.append_slash()+exe::name()+preferred_separator; if(!d.exists()) d.mkdir();
	return d;
}

__noinline path_t localtemp( path_t keydir=exe::dir() )
{
	static auto root=apptemp();
	path_t t = root + "local\\"+keydir.key()+"\\"; if(!t.exists()) t.mkdir();
	return t;
}

__noinline path_t serial_path( path_t dir, string prefix, string postfix, int numzero=4 )
{
	dir = dir.append_slash(); if(!dir.exists()) dir.mkdir();
	int nMaxFiles=1; for(int k=0;k<numzero;k++) nMaxFiles*=10;
	char fmt[PATH_MAX+1]={}; snprintf(fmt,PATH_MAX,"%s%s%%0%dd%s%s",dir.c_str(),prefix.c_str(),numzero,postfix.empty()?"":".",postfix.c_str());
	path_t f; for(int k=0;k<nMaxFiles;k++){snprintf(f.data(),PATH_MAX,fmt,k); if(!f.exists()) break; }
	return f;
}

//*************************************
namespace env {
//*************************************

__noinline const char* get( const char* key )
{
	if(!key||!*key) return "";
	char* v=getenv(key); return v&&*v?__strdup(v):"";
}

__noinline bool put( const char* key, const char* value )
{
	if(!key||!*key||!value||!*value) return false;
	return 0==putenv((char*)((string(key)+"="+value).c_str()));
}

__noinline vector<path_t> paths()
{
	vector<path_t> v; v.reserve(64);
	char* buff=__strdup(get("PATH")); if(!buff||!*buff) return v;
	for(char *ctx=nullptr,*token=strtok_s(buff,";",&ctx);token;token=strtok_s(nullptr,";",&ctx))
	{
		size_t l=strlen(token); if(!l||access(token,0)!=0) continue;
		v.emplace_back(path_t(token).append_slash());
	}
	return v;
}

__noinline path_t where( path_t file_name )
{
	if(file_name.empty()) return "";
	if(file_name.is_absolute()&&file_name.exists()) return file_name;
	vector<path_t> v={file_name}; path_t x = file_name.extension();
#ifdef __msvc__
	if(x.empty()){for(auto e:{".com",".exe",".bat",".cmd"}) v.emplace_back(file_name+e);} // add the executable extensions
#endif
	for(auto& f:v) if(f.exists()) return f.absolute(cwd());
	for(const auto& e:paths() ){ for(auto& p:v) if((e+p).exists()) return e+p; }
	return "";
}

__noinline void add_paths( const vector<path_t>& dirs )
{
	if(dirs.empty()) return;
	std::set<string> m; for( auto& p : paths() ) m.insert(tolower(p.c_str()));
	string v; for( auto d : dirs )
	{
		d = d.absolute();
		if(m.find(tolower(d.c_str()))==m.end()){ v += d.c_str(); v += ';'; }
	}
	put("PATH",(v+get("PATH")).c_str());
}

__noinline void add_path( path_t d ){ add_paths({d}); }

//*************************************
} // end namespace env
//*************************************

//*************************************
namespace os { // minimal process definitions
//*************************************

#ifdef __msvc__
__noinline wchar_t* build_cmdline( const char* app, const char* args )
{
	// allocate buffers
	size_t za=app?strlen(app):0, zg=args?strlen(args):0;
	vector<char> va(1024,0), vb(1024,0), vc(1024,0); { size_t z=za; if(zg>z) z=zg; z=z<<1; if(va.size()<z){ va.resize(z,0); vb.resize(z,0); vc.resize(z,0); } }
	char *a=va.data(), *b=vb.data(), *c=vc.data();
	
	// append extension to app
	if(za&&!path_t(app).exists()&&path_t(app).extension().empty()){ path_t e=env::where(app); if(!e.empty()){ app=__strdup((path_t(app).is_absolute()?e:e.filename()).c_str()); za=strlen(app); } }

	// auto_quote app
	constexpr char q='\"'; if(za&&(*app!=q||app[za-1]!=q)&&strpbrk(app," '\t|&<>")){ memcpy(a+1,app,za); a[0]=a[za+1]=q;a[za+2]=0; app=__strdup(a); }

	// append extension to argument-only no-extension app
	if(!za&&zg&&*args!=q)
	{
		path_t t=strtok((char*)to_backslash(args)," \t\r\n");
		if(!t.exists()&&t.extension().empty()){ path_t e=env::where(t); if(!e.empty()&&!e.extension().empty()){ strcpy(b,e.filename().c_str()); size_t zt=t.size(); if(args[zt]) strcat(b,args+zt); args=__strdup(b); } }
	}

	// build cmdline, which should also embed app path
	if(!za) return (wchar_t*)(zg?atow(args):L"");
	strcpy(c,app); return (wchar_t*)(atow(zg?strcat(strcat(c," "),args):c));
}

__noinline bool create_process( const char* app, const char* args=nullptr, bool wait=true, bool windowed=false, DWORD priority=NORMAL_PRIORITY_CLASS )
{
	STARTUPINFOW si={sizeof(si)}; si.dwFlags=STARTF_USESHOWWINDOW; si.wShowWindow=windowed?SW_SHOW:SW_HIDE;
	PROCESS_INFORMATION pi={}; if(!CreateProcessW(0,build_cmdline(app,args),0,0,FALSE,priority,0,0,&si,&pi)||!pi.hProcess||!pi.hThread){ printf( "%s(%s,%s): failed to create process\n", __func__, app?app:"", args?args:"" ); return false; }
	if(wait){ WaitForSingleObject(pi.hProcess,INFINITE); CloseHandle(pi.hThread); CloseHandle(pi.hProcess); }
	return true;
}

__noinline bool kill_process( string_view process_name, bool quiet=true )
{
	string cmd = format("taskkill /f /t /im %s", process_name.data());
	if(quiet) cmd += " 1>nul 2>&1";
	return 0==system(cmd.c_str());
}

#elif defined __gcc__
__noinline bool create_process( const char* app, const char* args=nullptr, bool b_wait=true )
{
	pid_t pid = fork(); if(pid<0){ printf( "%s(): fork failed\n" ); return false; }
	
	if(pid==0){ char* const args2[2] = {(char*)args,nullptr}; execvp( app, args2 ); } // child process
	else if(pid>0){ if(b_wait) wait(nullptr); } // parent process
	return true;
}
#endif

__noinline string read_process( string cmd )
{
	FILE* pp = popen(cmd.c_str(),"rb"); if(!pp) return "";
	vector<char> v; v.reserve(1024); char buff[64]={}; size_t n=0; while( (n=fread(buff,1,sizeof(buff),pp)) ) v.insert(v.end(),buff,buff+n); v.emplace_back(0);
	bool b_eof= feof(pp); pclose(pp); if(!b_eof) printf("%s(%s): broken pipe\n", __func__, cmd.c_str() );
#ifdef __msvc__
	char* s=v.data(); return is_utf8(s)?atoa(s,CP_UTF8,0):s;  // auto convert CP_UTF8 to current code page
#elif defined __gcc__
	char* s=v.data(); return s;
#endif
}

//*************************************
} // end namespace os
//*************************************

// general dynamic linking wrapper with DLL
#ifdef __msvc__
struct dll_t
{
	HMODULE hdll = nullptr;

	~dll_t(){ release(); }
	void release(){ if(hdll){ FreeLibrary(hdll); hdll=nullptr; } }
	const char* file_path(){ static char f[_MAX_PATH]={}; wchar_t w[_MAX_PATH]; if(hdll) GetModuleFileNameW(hdll,w,_MAX_PATH); return strcpy(f,wtoa(w)); }
	bool load( const char* dll_path ){ return nullptr!=(hdll=LoadLibraryW(atow(dll_path))); }
	template <class T> T get_proc_address( const char* name ) const { return hdll==nullptr?nullptr:(T)GetProcAddress(hdll,name); }
	template <class T> T* get_proc_address( const char* name, T*& p ) const { return hdll==nullptr?p=nullptr:p=(T*)GetProcAddress(hdll,name); }
	operator bool() const { return hdll!=nullptr; }
};
#endif

// image type declaration
struct image
{
	unsigned char*	data;
	unsigned int	width;
	unsigned int	height;
	unsigned int	depth;		// should be one of 8=IPL_DEPTH_8U, and 32=IPL_DEPTH_32F
	unsigned int	channels;	// should be one of 1, 2, 3, and 4
	unsigned int	fcc=0;		// color space fourcc: accepts only RGB, YUY2, YV12
	unsigned int	crc=0;		// image data crc
	int				index;		// signed image index
	const unsigned	align=4;	// byte alignment for rows; can be overriden for YUV (e.g., 64)

	inline unsigned int stride( int channel=0 ) const { bool i420=fcc==I420||fcc==YV12||fcc==IYUV; unsigned int bpp=(fcc==YUY2)?2:(i420||fcc==NV12)?1:channels; uint r=(depth>>3)*bpp*width; if(align<2) return r; return (((i420&&channel)?(r>>1):r)+align-1)&(~(align-1)); }
	inline unsigned int size() const { bool i420=fcc==I420||fcc==YV12||fcc==IYUV; return height*(i420?(stride()+stride(1)):fcc==NV12?(stride()+stride(1)/2):stride()); }
	template <class T> T* ptr( int y=0, int x=0, bool vflip=false ){ return ((T*)(data+stride()*(vflip?height-1-y:y)))+x; } // works only for RGB
	template <class T> T* plane( int channel=0 ){ unsigned char* p=data; int c=channel; if(c){ p+=stride()*height; if((fcc==I420||fcc==YV12||fcc==IYUV)&&c>1) p+=stride(1)*height/2; } return (T*)p; }

	// fourcc; YUY2==YUYV, I420==YU12==IYUV (YUV420P), YV12 (YVU420P), NV12 (YUV420SP)
	enum fcc_t { RGB=0, YUY2='2yuy', YUYV='vyuy', I420='024i', YU12=I420, IYUV='vuyi', YV12='21vy', NV12='21vn' };
};

// includes only minimal essential header files
#ifndef __GXUT_EXCLUDE_HEADERS__
	#if !defined(__GX_MATH_H__) && __has_include("gxmath.h")
		#include "gxmath.h"
	#endif
	#if !defined(__GX_STRING_H__) && __has_include("gxstring.h")
		#include "gxstring.h"
	#endif
	#if !defined(__GX_FILESYSTEM_H__) && __has_include("gxfilesystem.h") && !defined(__gcc__) && !defined __NO_PATHIMPL__
		#include "gxfilesystem.h"
	#endif
#endif

//*************************************
// set alias path_t to path when not using gxfilesystem.h
#if !__has_include("gxfilesystem.h") || defined __gcc__ || defined __NO_PATHIMPL__
	using path = path_t;
#endif

//*************************************
#endif // __GXUT_H__
