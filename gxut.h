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
#include <inttypes.h>	// defines int64_t, uint64_t
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
#include <map>
#include <set>
#include <string>
#include <typeinfo>
#include <typeindex>
#include <vector>
using namespace std::string_literals; // enable s-suffix for std::string literals
// common unique types in std namespace; these types should hardly have the same names in user definitions
using std::string;
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

// platform-specific header files
#ifdef __msvc__
	#include <windows.h>
	#include <direct.h>	// directory control
	#include <intrin.h>	// cpu info
	#include <io.h>		// low-level io functions
	typedef struct _stat64 stat_t;
	template <typename T> struct dll_function_t { HMODULE hdll=nullptr;T ptr=nullptr; dll_function_t(const char* dll,const char* func){if((hdll=LoadLibraryA(dll)))ptr=T(GetProcAddress(hdll,func));} ~dll_function_t(){if(hdll){FreeLibrary(hdll);hdll=nullptr;}} operator T(){return ptr;} }; // dll function wrapper: load from dll and operates as a function without auto dll release
	template <class T> T*& safe_release( T*& p ){if(p) p->Release(); return p=nullptr; }
	inline HANDLE& safe_close_handle( HANDLE& h ){ if(h!=INVALID_HANDLE_VALUE) CloseHandle(h); return h=INVALID_HANDLE_VALUE; }
#elif defined(__gcc__)
	#include <unistd.h>
	#include <linux/limits.h>
	#include <cpuid.h>
	typedef struct stat64 stat_t;
#elif defined(__clang__)
#endif

// platform-independent posix headers
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/utime.h>
#include <time.h>

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
template <class T=char> __forceinline T* __strbuf( size_t len ){ static T* C[1<<14]={}; static unsigned int cid=0; cid=cid%(sizeof(C)/sizeof(C[0])); C[cid]=(T*)(C[cid]?realloc(C[cid],sizeof(T)*(len+2)):malloc(sizeof(T)*(len+2))); if(C[cid]){ C[cid][0]=C[cid][len]=C[cid][len+1]=0; } return C[cid++]; }
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
#define wcstok_s	wcstok
#define strtok_s	strtok
#define _stricmp	strcasecmp
#define _wcsicmp	wcscasecmp
inline char* _strlwr( char* _String ){ for( char* p=_String; *p; p++ ){ *p=tolower(*p); } return _String; }
inline char* _strupr( char* _String ){ for( char* p=_String; *p; p++ ){ *p=toupper(*p); } return _String; }
inline wchar_t* _wcslwr( wchar_t* _String ){ for( wchar_t* p=_String; *p; p++ ){ *p=tolower(*p); } return _String; }
inline wchar_t* _wcsupr( wchar_t* _String ){ for( wchar_t* p=_String; *p; p++ ){ *p=toupper(*p); } return _String; }
inline int _wtoi( const wchar_t* _String ){ return int(wcstol(_String,0,10)); }
#endif

// vcpp extensions
inline wchar_t* _strlwr( wchar_t* _Str ){ return _wcslwr(_Str); }
inline wchar_t* _strupr( wchar_t* _Str ){ return _wcsupr(_Str); }
inline int _stricmp( const wchar_t* _Str1, const wchar_t* _Str2 ){ return _wcsicmp(_Str1,_Str2); }
inline wchar_t* strtok_s( wchar_t* _Str, const wchar_t* _Delim, wchar_t** context ){ return wcstok_s(_Str,_Delim,context); } // VS2015 fixes to ISO C Standard

// slee extensions
template <class T> size_t _strrspn( const T* _Str, const T* _Control ){ size_t L=strlen(_Str),C=strlen(_Control),k=0,j=0;for(k=0;k<L;k++){for(j=0;j<C;j++)if(_Str[L-1-k]==_Control[j])break;if(j==C)break;}return k; }
inline const char*    _stristr( const char* _Str1, size_t l1, const char* _Str2, size_t l2 ){ char* s1=__strdup(_Str1,l1); _strlwr(s1); char* s2=__strdup(_Str2,l2); _strlwr(s2); const char* r=strstr(s1,s2); return r?(_Str1+(r-s1)):nullptr; }
inline const wchar_t* _wcsistr( const wchar_t* _Str1, size_t l1, const wchar_t* _Str2, size_t l2 ){ wchar_t* s1=__strdup(_Str1,l1); _wcslwr(s1); wchar_t* s2=__strdup(_Str2,l2); _wcslwr(s2); const wchar_t* r=wcsstr(s1,s2); return r?(_Str1+(r-s1)):nullptr; }
inline const char*    _stristr( const char* _Str1, const char* _Str2 ){ return _stristr(_Str1, strlen(_Str1), _Str2, strlen(_Str2)); }
inline const wchar_t* _wcsistr( const wchar_t* _Str1, const wchar_t* _Str2 ){ return _wcsistr(_Str1, wcslen(_Str1), _Str2, wcslen(_Str2)); }
inline const wchar_t* _stristr( const wchar_t* _Str1, const wchar_t* _Str2 ){ return _wcsistr(_Str1, wcslen(_Str1), _Str2, wcslen(_Str2)); }

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
#else
inline const wchar_t* atow( const char* a ){ if(!a) return nullptr; if(!*a) return L""; const char* p=a; size_t l=mbsrtowcs(0,&p,0,0); wchar_t* b=__strbuf<wchar_t>(l); mbstate_t s={}; mbsrtowcs(b,&p,l+1,&s); return b; }
inline const char* wtoa( const wchar_t* w ){ if(!w) return nullptr; if(!*w) return ""; const wchar_t* p=w; size_t l=wcsrtombs(0,&p,0,0); char* b=__strbuf(l); mbstate_t s={}; wcsrtombs(b,&p,l+1,&s); return b; }
#endif
inline bool ismbs( const char* s ){ if(!s||!*s) return false; for(int k=0,kn=int(strlen(s));k<kn;k++,s++) if(*s<0)return true; return false; }

// auto conversion between const wchar_t* and const char*
template<class T, class U> __forceinline T*	__strdup( const U* s );
template<> inline __forceinline char*		__strdup<char,char>( const char* s ){ return __strdup(s); }
template<> inline __forceinline char*		__strdup<char,wchar_t>( const wchar_t* s ){ return (char*)wtoa(s); }
template<> inline __forceinline wchar_t*	__strdup<wchar_t,char>( const char* s ){ return (wchar_t*)atow(s); }
template<> inline __forceinline wchar_t*	__strdup<wchar_t,wchar_t>( const wchar_t* s ){ return __strdup(s); }

// format and printf replacement
inline const char*		vformat( __printf_format_string__ const char* fmt, va_list a ){ size_t len=size_t(vsnprintf(0,0,fmt,a)); char* buffer=__strbuf(len); vsnprintf(buffer,len+1,fmt,a); return buffer; }
inline const wchar_t*	vformat( __printf_format_string__ const wchar_t* const fmt, va_list a ){ size_t len=size_t(vswprintf(0,0,fmt,a)); wchar_t* bufferW=__strbuf<wchar_t>(len); vswprintf(bufferW,len+1,fmt,a); return bufferW; }
inline const char*		__attribute__((format(printf,1,2))) format( __printf_format_string__ const char* fmt, ... ){ va_list a; va_start(a,fmt); size_t len=size_t(vsnprintf(0,0,fmt,a)); char* buffer=__strbuf(len); vsnprintf(buffer,len+1,fmt,a); va_end(a); return buffer; }
inline const wchar_t*	format( __printf_format_string__ const wchar_t* fmt, ... ){ va_list a; va_start(a,fmt); size_t len=size_t(vswprintf(0,0,fmt,a)); wchar_t* bufferW=__strbuf<wchar_t>(len); vswprintf(bufferW,len+1,fmt,a); va_end(a); return bufferW; }

// case conversion
template <class T> const T* tolower( const T* src ){ return _strlwr(__strdup(src)); }
template <class T> const T* toupper( const T* src ){ return _strupr(__strdup(src)); }
inline const char* tovarname( const char* src, bool to_upper=false ){ if(!src||!*src) return ""; char *s=(char*)src,*dst=__strbuf(strlen(src)+2),*d=dst; if(!isalpha(*s)&&(*s)!='_') *(d++)='_'; for(;*s;s++,d++) *d=isalnum(*s)?(to_upper?char(toupper(*s)):(*s)):'_'; *d='\0'; return dst; }
inline const char* tovarname( const wchar_t* src, bool to_upper=false ){ return tovarname(wtoa(src),to_upper); }

// string function for path
inline const char* add_backslash( const char* src ){ if(!src||!*src) return src; size_t l=strlen(src); if(src[l-1]=='\\'||src[l-1]=='/') return __strdup(src); auto* s=strcpy(__strbuf(l+1),src); s[l]='\\';s[l+1]=0; return s; }
inline const char* add_slash( const char* src ){ if(!src||!*src) return src; size_t l=strlen(src); if(src[l-1]=='\\'||src[l-1]=='/') return __strdup(src); auto* s=strcpy(__strbuf(l+1),src); s[l]='/';s[l+1]=0; return s; }
inline const char* to_backslash( const char* src ){ if(!src||!*src) return src; auto* s=__strdup(src); for( auto* p=s; *p; p++ ) if(*p=='/') *p='\\'; return s; }
inline const char* to_slash( const char* src ){ if(!src||!*src) return src; auto* s=__strdup(src); for( auto* p=s; *p; p++ ) if(*p=='\\') *p='/'; return s; }
#ifdef __msvc__
inline const char* to_preferred( const char* src ){ return to_backslash(src); }
#elif __gcc__
inline const char* to_preferred( const char* src ){ return to_slash(src); }
#endif

// natural-order and case-insensitive comparison for std::sort, std::map/set, std::unordered_map/set
#ifdef _INC_SHLWAPI
	inline int _strcmplogical( const wchar_t* _Str1, const wchar_t* _Str2 ){ return (!_Str1||!_Str2)?0:StrCmpLogicalW(_Str1,_Str2); }
#elif defined(__msvc__)
	inline int _strcmplogical( const wchar_t* _Str1, const wchar_t* _Str2 ){ static dll_function_t<int(*)(const wchar_t*,const wchar_t*)> f("shlwapi.dll","StrCmpLogicalW"); return !f?_wcsicmp(_Str1,_Str2):(!_Str1||!_Str2)?0:f(_Str1,_Str2); } // load StrCmpLogicalW(): when unavailable, fallback to wcsicmp
#else
	inline int _strcmplogical( const wchar_t* s1, const wchar_t* s2 )
	{
		if(!s1||!s2) return 0; while(*s1)
		{
			if(!*s2) return 1;
			else if(iswdigit(*s1)){ if(!iswdigit(*s2)) return -1; int i1=_wtoi(s1), i2=_wtoi(s2); if(i1<i2) return -1; else if(i1>i2)	return 1; while(iswdigit(*s1)){ s1++; } while(iswdigit(*s2)){ s2++; } }
			else if(!iswdigit(*s2)){ int d=wcsncasecmp(s1,s2,1); if(d>0) return 1; else if(d<0) return -1; s1++; s2++; /*int d=CompareStringW(GetThreadLocale(),NORM_IGNORECASE,s1,1,s2,1)-CSTR_EQUAL*/ }
			else return 1;
		}
		return (*s2)?-1:0;
	}
#endif
inline int _strcmplogical( const char* _Str1, const char* _Str2 ){ return _strcmplogical(atow(_Str1),atow(_Str2)); }

// pattern matching: simple ?/* is supported; posix-style **/* (subdirectory matching) is not implemented yet
template <class T=wchar_t>
__noinline bool iglob( const T* str, size_t slen, const T* pattern, size_t plen ) // case-insensitive
{
	static const T q=T('?'), a=T('*');
	int n=int(slen?slen:strlen(str)), m=int(plen?plen:strlen(pattern)); if(m==0) return n==0;
	int i,j,t,p; for(i=0,j=0,t=-1,p=-1;i<n;)
	{
		if(tolower(str[i])==tolower(pattern[j])||(j<m&&pattern[j]==q)){i++;j++;}
		else if(j<m&&pattern[j]==a){t=i;p=j;j++;}
		else if(p!=-1){j=p+1;i=t+1;t++;}
		else return false;
	}
	while(j<m&&pattern[j]==a)j++;
	return j==m;
}

// forward decls.
namespace exe { const char* dir(); }

//*************************************
namespace gx {
//*************************************

// path queries
inline bool file_exists( const string& path ){ return !path.empty()&&access(path.c_str(), 0)==0; }
inline bool is_pipe_path( const string& path ){ if(path.empty()) return false; return path=="-"||_strnicmp(path.c_str(), "pipe:", 5)==0; }
inline bool is_fifo( FILE* fp ){ if(!fp) return false; struct stat s; return fstat(_fileno(fp),&s)==0?(s.st_mode&_S_IFIFO?true:false):false; } // posix-like or std::filesystem-like utilities
inline bool is_unc_path( const string& path ){ if(path.empty()) return false; return (path[0]=='\\'&&path[1]=='\\')||(path[0]=='/'&&path[1]=='/'); }
inline bool is_rsync_path( const string& path ){ if(path.empty()) return false; auto* p=strstr(path.c_str(), ":\\"); if(!p) p=strstr(path.c_str(), ":/"); return p&&p>path.c_str()+1; }
inline bool is_http_url( const string& path ){ if(path.empty()) return false; return _strnicmp(path.c_str(),"http://",7)==0||_strnicmp(path.c_str(),"https://",8)==0; }
inline bool is_ssh_path( const string& path ){ if(path.empty()) return false; return strstr(path.c_str()+2,":\\")!=nullptr||strstr(path.c_str()+2,":/")!=nullptr; }
inline bool is_remote_path( const string& path ){ if(path.empty()) return false; return is_http_url(path)||is_ssh_path(path); }
inline bool is_absolute_path( const string& path ){ if(path.empty()) return false; return path[0]=='/'||path[1]==':'||is_unc_path(path)||is_rsync_path(path)||is_remote_path(path); }
inline bool is_relative_path( const string& path ){ if(path.empty()) return false; return !is_pipe_path(path)&&!is_absolute_path(path); }
#define __pathbuf()		(__strbuf<char>(PATH_MAX))
#ifdef __msvc__
	#define PATH_MAX _MAX_PATH // _MAX_PATH = 260 in Windows
	inline string dir( const string& path ){ if(path.empty()) return ""; char *d=__pathbuf(), *a=__pathbuf(); _splitpath_s(path.c_str(),d,PATH_MAX,a,PATH_MAX,0,0,0,0); strcat(d, a); return d; }
	inline string filename( const string& path ){ if(path.empty()) return ""; char *b=__pathbuf(), *a=__pathbuf(); _splitpath_s(path.c_str(),0,0,0,0,b,PATH_MAX,a,PATH_MAX); strcat(b, a); return b; }
	inline string stem( const string& path ){ if(path.empty()) return ""; char* b=__pathbuf(); _splitpath_s(path.c_str(),0,0,0,0,b,PATH_MAX,0,0); return b; } // filename without extension
	inline string extension( const string& path ){ if(path.empty()) return ""; char* x=__pathbuf(); _splitpath_s(path.c_str(),0,0,0,0,0,0,x,PATH_MAX); return (x&&*x=='.')?x+1:x; }
	inline bool is_dir( const string& path ){ if(path.empty()||access(path.c_str(),0)!=0) return false; stat_t s; if(!_stat64(path.c_str(), &s)) return false; return s.st_mode&S_IFDIR?true:false; }
	inline string absolute_path( const string& path, const string& base="" ){ if(path.empty()||is_absolute_path(path)) return path; auto* b=_fullpath(__pathbuf(),base.empty()?path.c_str():strcat(strcpy(__pathbuf(),add_backslash(base.c_str())),path.c_str()),PATH_MAX); return b?b:""; }
#elif defined(__gcc__)
	#define _MAX_PATH PATH_MAX // PATH_MAX = 4096 in linux
	inline string dir( const string& path ){ if(path.empty()) return ""; return fs::path(path).remove_filename().c_str(); }
	inline string filename( const string& path ){ if(path.empty()) return ""; return fs::path(path).filename().c_str(); }
	inline string stem( const string& path ){ if(path.empty()) return ""; return fs::path(path).stem().c_str(); } // filename without extension
	inline string extension( const string& path ){ if(path.empty()) return ""; string x=fs::path(path).extension().c_str(); return !x.empty()&&x.front()=='.')?x.substr(1):x; }
	inline bool is_dir( const string& path ){ if(path.empty()||access(path.c_str(),0)!=0) return false;  stat_t s; if(!stat64(path.c_str(), &s)) return false; return s.st_mode&S_IFDIR?true:false; }
#endif
inline string path_key( const string& path )
{
	if(path.empty()) return "";
	size_t l=path.size(); char* d=__strbuf(l);
	size_t n=0; for(size_t k=0;k<l; k++){ char c=path[k]; if(c!=':'&&c!=' ') d[n++]=(c=='\\'||c=='/')?'.':(::tolower(c)); }
	if(d[n-1]=='.') n--; d[n]=0; return d;
}

 // root unc path without slash/backslash
__noinline string unc_root( const string& path )
{
	if(!is_unc_path(path)) return "";
	string r = to_preferred(path.c_str());
	const char* p = strpbrk(r.c_str()+2,"\\/");
	return p ? r.substr(0,p-r.c_str()) : r;
}

__noinline vector<string> ancestors( const string& path, string root="" )
{
	if(path.empty()) return vector<string>();
	if(root.empty()) root = is_unc_path(path)?unc_root(path):string(exe::dir());
	string d = dir(path); int l=int(d.size()),rl=int(root.size());
	bool r=_strnicmp(d.c_str(),root.c_str(),rl)==0;
	vector<string> a; a.reserve(4); for(int k=l-1,e=r?rl-1:0;k>=e;k--){ if(d[k]=='\\'||d[k]=='/') a.emplace_back(d.substr(k)); }
	return a;
}

__noinline bool mkdir( const string& dir_path ) // make all super directories recursively
{
	if(dir_path.empty()||file_exists(dir_path)) return false;
	auto v = ancestors(dir(to_backslash(dir_path.c_str()))); if(v.empty()) return false;
	auto bl=v.back().size();
	if(is_unc_path(dir_path)){ auto r=unc_root(dir(dir_path));size_t rl=r.size();if(bl<=rl+1){v.pop_back();bl=v.back().size();}if(bl<=rl+1)v.pop_back(); }
	else if(bl<=3){ if(v.back()[1]==':')v.pop_back();else if(bl<=1)v.pop_back(); }
	for( auto it=v.rbegin(); it!=v.rend(); it++ ){ if(!file_exists(*it)&&::mkdir(it->c_str())!=0) return false; }
	return true;
}

// time-related functions
__noinline time_t mtime( const string& path )
{
	stat_t t; if(_stat64( path.c_str(), &t )!=0) return 0;
	return t.st_mtime;
}

__noinline bool utime( const string& path, time_t mtime )
{
	stat_t t; if(_stat64( path.c_str(), &t )!=0) return 0;
	utimbuf u = { t.st_atime, mtime };
	return ::utime( path.c_str(), &u )==0;
}

//*************************************
} // end namespace gx
//*************************************

// global path functions
#ifdef __msvc__
	inline const char* cwd(){ static char c[PATH_MAX]={}; _getcwd(c, PATH_MAX); size_t l=strlen(c); if(*c&&c[l-1]!='\\'){ c[l]='\\'; c[l+1]=0; } return c; } // current working directory
#elif defined(__gcc__)
	inline const char* cwd(){ static char c[PATH_MAX]={}; getcwd(c); size_t l=strlen(c); if(*c&&c[l-1]!='/'){ c[l]='/'; c[l+1]=0; } return c; } // current working directory
#endif
inline int chdir( const string& dir ){ return ::chdir(dir.c_str()); }

//*************************************
namespace exe {
//*************************************
#ifdef __msvc__
inline const char* path(){	static string e; if(!e.empty()) return e.c_str(); auto* b=__pathbuf(); GetModuleFileNameA(nullptr,b,PATH_MAX); return (e=b).c_str(); }
#elif defined(__gcc__)
inline const char* path(){	static string e; if(!e.empty()) return e.c_str(); auto* b=__pathbuf(); if(!readlink("/proc/self/exe",e,PATH_MAX)>0) *m=0; return (e=b).c_str(); }
#endif
inline const char* dir(){	static auto d=gx::dir(path()); return d.c_str(); }
inline const char* name(){	static auto n=gx::stem(path()); return n.c_str(); }
//*************************************
} // end namespace exe
//*************************************
#undef __pathbuf

//*************************************
namespace env {
//*************************************

inline const char* get( const char* key )
{
	if(!key||!*key) return "";
	char* v=getenv(key); return v&&*v?__strdup(v):"";
}

inline bool put( const char* key, const char* value )
{
	if(!key||!*key||!value||!*value) return false;
	return 0==putenv((string(key)+"="+value).c_str());
}

inline vector<string> paths()
{
	vector<string> v; v.reserve(64);
	char* buff=__strdup(get("PATH")); if(!buff||!*buff) return v;
	for(char *ctx=nullptr,*token=strtok_s(buff,";",&ctx);token;token=strtok_s(nullptr,";",&ctx))
	{
		size_t l=strlen(token); if(!l||access(token,0)!=0) continue;
		v.emplace_back(token); if(v.back().back()!='\\') v.back()+='\\';
	}
	return v;
}

inline const char* where( const char* file_name )
{
	if(!file_name||!*file_name) return "";
	if(gx::is_absolute_path(file_name)&&gx::file_exists(file_name)) return file_name;
	vector<string> v={file_name}; string x=gx::extension(file_name);
#ifdef __msvc__
	if(x.empty()){for(auto e:{".com",".exe",".bat",".cmd"}) v.emplace_back(string(file_name)+e);} // add the executable extensions
#endif
	for(auto& f:v) if(gx::file_exists(f.c_str())) return __strdup(gx::absolute_path(f.c_str(),cwd()).c_str());
	for(const auto& e:paths() ){ for(auto& p:v) if(gx::file_exists((e+p).c_str())) return __strdup((e+p).c_str()); }
	return "";
}

inline void add_paths( const vector<string>& dirs )
{
	if(dirs.empty()) return;
	std::set<string> m; for( auto& p : paths() ) m.insert(tolower(p.c_str()));
	string v; for( auto d : dirs )
	{
		d = gx::is_relative_path(d.c_str())?gx::absolute_path(d.c_str()):d;
		if(m.find(tolower(d.c_str()))==m.end()){ v += d.c_str(); v += ';'; }
	}
	put("PATH",(v+get("PATH")).c_str());
}

inline void add_path( const string& d ){ add_paths( {d} ); }

//*************************************
} // end namespace env
//*************************************

// general dynamic linking wrapper with DLL
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

// includes only minimal essential header files
#ifndef __GXUT_EXCLUDE_HEADERS__
	#if !defined(__GX_MATH_H__) && __has_include("gxmath.h")
		#include "gxmath.h"
	#endif
	#if !defined(__GX_STRING_H__) && __has_include("gxstring.h")
		#include "gxstring.h"
	#endif
	#if !defined(__GX_FILESYSTEM_H__) && __has_include("gxfilesystem.h") && !defined(__gcc__)
		#include "gxfilesystem.h"
	#endif
#endif

//*************************************
#endif // __GXUT_H__
