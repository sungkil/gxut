//*******************************************************************
// Copyright 2011-2018 Sungkil Lee
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
// C standard
#include <inttypes.h>	// defines int64_t, uint64_t
#include <math.h>
#include <stdio.h>
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
#if defined(_WIN32)||defined(_WIN64) // Windows
	#ifndef NOMINMAX
		#define NOMINMAX // suppress definition of min/max
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
	#pragma optimize( "gsy", on )
	#pragma check_stack( off )
	#pragma strict_gs_check( off )
	#pragma float_control(except,off)
	#ifndef __noinline
		#define __noinline __declspec(noinline)
	#endif
#else // GCC or Clang
	#ifndef __noinline
		#define __noinline //__attribute__((noinline))
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
// printf replacements: define implementation somewhere to use this
extern int (*gprintf)( const char*, ... );
extern int (*gwprintf)( const wchar_t*, ... );
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
using uint		= unsigned int;		using uchar		= unsigned char;	using ushort	= unsigned short;
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

#include <stdarg.h>
#include <wchar.h>

// size of shared circular buffer
#define SHARED_CIRCULAR_BUFFER_SIZE (1<<14)

// Byte order masks for UTF encoding
static const unsigned char BOM_UTF8[3]={0xEF,0xBB,0xBF};
static const unsigned char BOM_UTF16[2]={0xFF,0xFE};			// little endian
static const unsigned char BOM_UTF32[4]={0xFF,0xFE,0x00,0x00};	// little endian

//***********************************************
// 0. overloaded string functions for wchar_t
inline wchar_t* strcpy( wchar_t* _Dest, const wchar_t* _Src ){ return wcscpy(_Dest,_Src); }
inline wchar_t* strncpy( wchar_t* _Dest, const wchar_t* _Src, size_t _Count ){ return wcsncpy(_Dest,_Src,_Count); }
inline wchar_t* strcat( wchar_t* _Dest, const wchar_t* _Src ){ return wcscat(_Dest,_Src);  }
inline wchar_t* strncat( wchar_t* _Dest, const wchar_t* _Src, size_t _Count ){ return wcsncat(_Dest,_Src,_Count); }
inline int strcmp( const wchar_t* _Str1, const wchar_t* _Str2 ){ return wcscmp(_Str1,_Str2); }
inline int strncmp( const wchar_t* _Str1, const wchar_t* _Str2, size_t _MaxCount ){ return wcsncmp(_Str1,_Str2,_MaxCount); }
inline const wchar_t* strchr( const wchar_t* _Str, int _Val ){ return wcschr(_Str,_Val); }
inline wchar_t* strchr( wchar_t* _Str, int _Val ){ return wcschr(_Str,_Val); }
inline const wchar_t* strstr( const wchar_t* _Str1, const wchar_t* _Str2 ){ return wcsstr(_Str1,_Str2); }
inline wchar_t* strstr( wchar_t* _Str1, const wchar_t* _Str2 ){ return wcsstr(_Str1,_Str2); }
inline wchar_t* strtok_s( wchar_t* _Str, const wchar_t* _Delim, wchar_t** context ){ return wcstok_s(_Str,_Delim,context); } // VS2015 fixes to ISO C Standard
inline size_t strspn( const wchar_t* _Str, const wchar_t* _Control ){ return wcsspn(_Str,_Control); }
inline size_t strcspn( const wchar_t* _Str, const wchar_t* _Control ){ return wcscspn(_Str,_Control); }
inline size_t strlen( const wchar_t* _Str ){ return wcslen(_Str); }
inline const wchar_t * strpbrk( const wchar_t* _Str, const wchar_t* _Control ){ return wcspbrk(_Str,_Control); }
inline wchar_t * strpbrk( wchar_t* _Str, const wchar_t* _Control ){ return wcspbrk(_Str,_Control); }
// 0.1 VC extensions
inline wchar_t* _strlwr( wchar_t* _Str ){ return _wcslwr(_Str); return _Str; } 
inline wchar_t* _strupr( wchar_t* _Str ){ return _wcsupr(_Str); return _Str; }
inline int _stricmp( const wchar_t* _Str1, const wchar_t* _Str2 ){ return _wcsicmp(_Str1,_Str2); }
// 0.2 slee extensions
template <class T> inline size_t _strrspn( const T* _Str, const T* _Control ){size_t L=strlen(_Str),C=strlen(_Control),k=0,j=0;for(k=0;k<L;k++){for(j=0;j<C;j++)if(_Str[L-1-k]==_Control[j])break;if(j==C)break;}return k;}
inline const char* _stristr( const char* _Str1, const char* _Str2 ){ char* s1=_strdup(_Str1);_strlwr(s1); char* s2=_strdup(_Str2);_strlwr(s2); const char* r=strstr(s1,s2); if(r)r=_Str1+(r-s1); free(s1); free(s2); return r; }
inline const wchar_t* _wcsistr( const wchar_t* _Str1, const wchar_t* _Str2 ){ wchar_t* s1=_wcsdup(_Str1); _wcslwr(s1); wchar_t* s2=_wcsdup(_Str2); _wcslwr(s2); const wchar_t* r=wcsstr(s1,s2); if(r)r=_Str1+(r-s1); free(s1); free(s2); return r; }
inline const wchar_t* _stristr( const wchar_t* _Str1, const wchar_t* _Str2 ){ return _wcsistr(_Str1,_Str2); }

//***********************************************
// 1. shared circular buffers
template <class T> __forceinline T* __tstrbuf( size_t len ){ static T* C[SHARED_CIRCULAR_BUFFER_SIZE]={0}; static uint cid=0; cid=(++cid)%(sizeof(C)/sizeof(T*));C[cid]=(T*)realloc(C[cid],sizeof(T)*(len+1)); C[cid][len]=0; return C[cid]; }
template <class T> __forceinline T* __tstrdup( const T* s,size_t slen=-1 ){ if(slen==-1)slen=strlen(s); return (T*)memcpy(__tstrbuf<T>(slen),s,sizeof(T)*slen); }
__forceinline char* _strbuf( size_t len ){ return __tstrbuf<char>(len); }
__forceinline wchar_t* _wcsbuf( size_t len ){ return __tstrbuf<wchar_t>(len); }

//***********************************************
// 2. format
inline const char* vformat( va_list a, const char* fmt ){ size_t len=size_t(_vscprintf(fmt,a)); char* buffer=_strbuf(len); vsprintf_s(buffer,len+1,fmt,a); return buffer; }
inline const wchar_t* vformat( va_list a, const wchar_t* fmt ){ size_t len=size_t(_vscwprintf(fmt,a)); wchar_t* bufferW=_wcsbuf(len); vswprintf_s(bufferW,len+1,fmt,a); return bufferW; }
inline const char* format( const char* fmt,... ){ va_list a; va_start(a,fmt); size_t len=size_t(_vscprintf(fmt,a)); char* buffer=_strbuf(len); vsprintf_s(buffer,len+1,fmt,a); va_end(a); return buffer; }
inline const wchar_t* format( const wchar_t* fmt,... ){ va_list a; va_start(a,fmt); size_t len=size_t(_vscwprintf(fmt,a)); wchar_t* bufferW=_wcsbuf(len); vswprintf_s(bufferW,len+1,fmt,a); va_end(a); return bufferW; }

//***********************************************
// 3. case conversion
template <class T> inline const T* tolower( const T* src ){ return _strlwr(__tstrdup(src)); }
template <class T> inline const T* toupper( const T* src ){ return _strupr(__tstrdup(src)); }
inline const char* tovarname( const char* src ){ if(!src||!*src) return ""; char *s=(char*)src,*dst=_strbuf(strlen(src)+2), *d=dst; if(!isalpha(*s)&&(*s)!='_') *(d++)='_'; for(;*s;s++,d++) *d=isalnum(*s)?(*s):'_'; *d='\0'; return dst; }
inline const wchar_t* tovarname( const wchar_t* src ){ if(!src||!*src) return L""; wchar_t *s=(wchar_t*)src,*dst=_wcsbuf(wcslen(src)+2), *d=dst; if(!isalpha(*s)&&(*s)!=L'_') *(d++)=L'_'; for(;*s;s++,d++) *d=isalnum(*s)?(*s):L'_'; *d=L'\0'; return dst; }

//***********************************************
// 4. case-insensitive comparison for std::map/set, std::unordered_map/set
namespace nocase
{
	template <> struct less<std::string>{ bool operator()(const std::string& a,const std::string& b)const{return _stricmp(a.c_str(),b.c_str())<0;}};
	template <> struct less<std::wstring>{ bool operator()(const std::wstring& a,const std::wstring& b)const{return _wcsicmp(a.c_str(),b.c_str())<0;}};
	template <> struct equal_to<std::string>{ bool operator()(const std::string& a,const std::string& b)const{return _stricmp(a.c_str(),b.c_str())==0;}};
	template <> struct equal_to<std::wstring>{ bool operator()(const std::wstring& a,const std::wstring& b)const{return _wcsicmp(a.c_str(),b.c_str())==0;}};
	template <> struct hash<std::string> { size_t operator()( const std::string& p ) const { return std::hash<std::string>()(_strlwr(__tstrdup(p.c_str()))); }};
	template <> struct hash<std::wstring> { size_t operator()( const std::wstring& p ) const { return std::hash<std::wstring>()(_strlwr(__tstrdup(p.c_str()))); }};

	template <class T> using			set = std::set<T,less<T>>;
	template <class T, class V> using	map = std::map<T,V,less<T>>;
	template <class T> using			unordered_set = std::unordered_set<T,hash<T>,equal_to<T>>;
	template <class T, class V> using	unordered_map = std::unordered_map<T,V,hash<T>,equal_to<T>>;
}

//***********************************************
// 5. conversion between const wchar_t* and const char*
template <class T,class I> const T* _strcvt( const I* s ){size_t len=strlen(s);T* buff=__tstrbuf<T>(len);for(uint k=0;k<len;k++)buff[k]=T(s[k]);return buff;}
inline const wchar_t* atow( const char* a ){int wlen=MultiByteToWideChar(0,0,a,-1,0,0);wchar_t* wbuff=_wcsbuf(wlen);MultiByteToWideChar(0,0,a,-1,wbuff,wlen);return wbuff;}
inline const char* wtoa( const wchar_t* w ){int mblen=WideCharToMultiByte(0,0,w,-1,0,0,0,0);char* buff=_strbuf(mblen);WideCharToMultiByte(0,0,w,-1,buff,mblen,0,0);return buff;}

//***********************************************
// 6. conversion to string types
inline const char* btoa( bool b ){ return b?"1":"0"; }
inline const char* itoa( int i ){static const char* fmt="%d";size_t size=size_t(_scprintf(fmt,i));char* buff=_strbuf(size); sprintf_s(buff,size+1,fmt,i);return buff;}
inline const char* utoa( uint u ){static const char* fmt="%u";size_t size=size_t(_scprintf(fmt,u));char* buff=_strbuf(size); sprintf_s(buff,size+1,fmt,u);return buff;}
inline const char* ftoa( float f ){if(fabs(f)<0.00000001f)return "0";static const char* fmt="%g";size_t size=size_t(_scprintf(fmt,f));char* buff=_strbuf(size); sprintf_s(buff,size+1,fmt,f); return buff; }
inline const char* dtoa( double d ){if(fabs(d)<0.00000001)return "0";static const char* fmt="%g";size_t size=size_t(_scprintf(fmt,d));char* buff=_strbuf(size); sprintf_s(buff,size+1,fmt,d); return buff; }
inline const char* illtoa( int64_t i ){ static const char* fmt="%lld";size_t size=size_t(_scprintf(fmt,i));char* buff=_strbuf(size); sprintf_s(buff,size+1,fmt,i);return buff; }
inline const char* ulltoa( uint64_t u ){ static const char* fmt="%llu";size_t size=size_t(_scprintf(fmt,u));char* buff=_strbuf(size); sprintf_s(buff,size+1,fmt,u);return buff; }
inline const char* itoa( const int2& v ){static const char* fmt="%d %d";size_t size=size_t(_scprintf(fmt,v[0],v[1]));char* buff=_strbuf(size); sprintf_s(buff,size+1,fmt,v[0],v[1]);return buff;}
inline const char* itoa( const int3& v ){static const char* fmt="%d %d %d";size_t size=size_t(_scprintf(fmt,v[0],v[1],v[2]));char* buff=_strbuf(size); sprintf_s(buff,size+1,fmt,v[0],v[1],v[2]);return buff;}
inline const char* itoa( const int4& v ){static const char* fmt="%d %d %d %d";size_t size=size_t(_scprintf(fmt,v[0],v[1],v[2],v[3]));char* buff=_strbuf(size); sprintf_s(buff,size+1,fmt,v[0],v[1],v[2],v[3]);return buff;}
inline const char* itoa( const short2& v ){ int2 i={v[0],v[1]};return itoa(i); }
inline const char* itoa( const short3& v ){ int3 i={v[0],v[1],v[2]};return itoa(i); }
inline const char* itoa( const short4& v ){ int4 i={v[0],v[1],v[2],v[3]};return itoa(i); }
inline const char* itoa( const char2& v ){ int2 i={v[0],v[1]};return itoa(i); }
inline const char* itoa( const char3& v ){ int3 i={v[0],v[1],v[2]};return itoa(i); }
inline const char* itoa( const char4& v ){ int4 i={v[0],v[1],v[2],v[3]};return itoa(i); }
inline const char* utoa( const uint2& v ){static const char* fmt="%u %u";size_t size=size_t(_scprintf(fmt,v[0],v[1]));char* buff=_strbuf(size); sprintf_s(buff,size+1,fmt,v[0],v[1]);return buff;}
inline const char* utoa( const uint3& v ){static const char* fmt="%u %u %u";size_t size=size_t(_scprintf(fmt,v[0],v[1],v[2]));char* buff=_strbuf(size); sprintf_s(buff,size+1,fmt,v[0],v[1],v[2]);return buff;}
inline const char* utoa( const uint4& v ){static const char* fmt="%u %u %u %u";size_t size=size_t(_scprintf(fmt,v[0],v[1],v[2],v[3]));char* buff=_strbuf(size); sprintf_s(buff,size+1,fmt,v[0],v[1],v[2],v[3]);return buff;}
inline const char* utoa( const ushort2& v ){ uint2 u={v[0],v[1]};return utoa(u); }
inline const char* utoa( const ushort3& v ){ uint3 u={v[0],v[1],v[2]};return utoa(u); }
inline const char* utoa( const ushort4& v ){ uint4 u={v[0],v[1],v[2],v[3]};return utoa(u); }
inline const char* utoa( const uchar2& v ){ uint2 u={v[0],v[1]};return utoa(u); }
inline const char* utoa( const uchar3& v ){ uint3 u={v[0],v[1],v[2]};return utoa(u); }
inline const char* utoa( const uchar4& v ){ uint4 u={v[0],v[1],v[2],v[3]};return utoa(u); }
inline const char* ftoa( const float2& v ){static const char* fmt="%g %g";size_t size=size_t(_scprintf(fmt,v[0],v[1]));char* buff=_strbuf(size); sprintf_s(buff,size+1,fmt,v[0],v[1]);return buff;}
inline const char* ftoa( const float3& v ){static const char* fmt="%g %g %g";size_t size=size_t(_scprintf(fmt,v[0],v[1],v[2]));char* buff=_strbuf(size); sprintf_s(buff,size+1,fmt,v[0],v[1],v[2]);return buff;}
inline const char* ftoa( const float4& v ){static const char* fmt="%g %g %g %g";size_t size=size_t(_scprintf(fmt,v[0],v[1],v[2],v[3]));char* buff=_strbuf(size); sprintf_s(buff,size+1,fmt,v[0],v[1],v[2],v[3]);return buff;}
inline const char* ftoa( const float9& m ){static const char* fmt="%g %g %g %g %g %g %g %g %g";size_t size=size_t(_scprintf(fmt,m[0],m[1],m[2],m[3],m[4],m[5],m[6],m[7],m[8]));char* buff=_strbuf(size); sprintf_s(buff,size+1,fmt,m[0],m[1],m[2],m[3],m[4],m[5],m[6],m[7],m[8]);return buff;}
inline const char* ftoa( const float16& m ){static const char* fmt="%g %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g";size_t size=size_t(_scprintf(fmt,m[0],m[1],m[2],m[3],m[4],m[5],m[6],m[7],m[8],m[9],m[10],m[11],m[12],m[13],m[14],m[15]));char* buff=_strbuf(size); sprintf_s(buff,size+1,fmt,m[0],m[1],m[2],m[3],m[4],m[5],m[6],m[7],m[8],m[9],m[10],m[11],m[12],m[13],m[14],m[15]);return buff;}
inline const char* dtoa( const double2& v ){static const char* fmt="%g %g";size_t size=size_t(_scprintf(fmt,v[0],v[1]));char* buff=_strbuf(size); sprintf_s(buff,size+1,fmt,v[0],v[1]);return buff;}
inline const char* dtoa( const double3& v ){static const char* fmt="%g %g %g";size_t size=size_t(_scprintf(fmt,v[0],v[1],v[2]));char* buff=_strbuf(size); sprintf_s(buff,size+1,fmt,v[0],v[1],v[2]);return buff;}
inline const char* dtoa( const double4& v ){static const char* fmt="%g %g %g %g";size_t size=size_t(_scprintf(fmt,v[0],v[1],v[2],v[3]));char* buff=_strbuf(size); sprintf_s(buff,size+1,fmt,v[0],v[1],v[2],v[3]);return buff;}
inline const char* dtoa( const double9& v ){static const char* fmt="%g %g %g %g %g %g %g %g %g";size_t size=size_t(_scprintf(fmt,v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],v[8]));char* buff=_strbuf(size); sprintf_s(buff,size+1,fmt,v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],v[8]);return buff;}
inline const char* dtoa( const double16& v ){static const char* fmt="%g %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g";size_t size=size_t(_scprintf(fmt,v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],v[8],v[9],v[10],v[11],v[12],v[13],v[14],v[15]));char* buff=_strbuf(size); sprintf_s(buff,size+1,fmt,v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],v[8],v[9],v[10],v[11],v[12],v[13],v[14],v[15]);return buff;}

//***********************************************
// 6.2 Special-purpose functions

// conversion int to string with commas
__noinline inline const char* itoasep( int n )
{
	if(n<1000&&n>-1000) return itoa(n);
	const char* s=itoa(n>0?n:-n); size_t len=strlen(s);
	std::vector<char> v; v.resize(len+1); memcpy(&v[0],s,len+1);
	for( uint idx=(len%3?len%3:3); idx<len; idx+=4,len++ ) v.emplace(v.begin()+idx,',');
	return format("%s%s",n>0?"":"-",&v[0]);
}

__noinline inline const char* illtoasep( int64_t n )
{
	if(n<1000&&n>-1000) return illtoa(n);
	const char* s=illtoa(n>0?n:-n); size_t len=strlen(s);
	std::vector<char> v; v.resize(len+1); memcpy(&v[0],s,len+1);
	for( uint idx=(len%3?len%3:3); idx<len; idx+=4,len++ ) v.emplace(v.begin()+idx,',');
	return format("%s%s",n>0?"":"-",&v[0]);
}

//***********************************************
// 7. conversion to wstring types
inline const wchar_t* btow( bool b ){			return atow(btoa(b)); }
inline const wchar_t* itow( int i ){			return atow(itoa(i)); }
inline const wchar_t* utow( uint u ){			return atow(utoa(u)); }
inline const wchar_t* ftow( float f ){			return atow(ftoa(f)); }
inline const wchar_t* dtow( double d ){			return atow(dtoa(d)); }
inline const wchar_t* ulltow( uint64_t u ){		return atow(ulltoa(u)); }
inline const wchar_t* itow( const int2& v ){	return atow(itoa(v)); }
inline const wchar_t* itow( const int3& v ){	return atow(itoa(v)); }
inline const wchar_t* itow( const int4& v ){	return atow(itoa(v)); }
inline const wchar_t* itow( const short2& v ){	return atow(itoa(v)); }
inline const wchar_t* itow( const short3& v ){	return atow(itoa(v)); }
inline const wchar_t* itow( const short4& v ){	return atow(itoa(v)); }
inline const wchar_t* itow( const char2& v ){	return atow(itoa(v)); }
inline const wchar_t* itow( const char3& v ){	return atow(itoa(v)); }
inline const wchar_t* itow( const char4& v ){	return atow(itoa(v)); }
inline const wchar_t* utow( const uint2& u ){	return atow(utoa(u)); }
inline const wchar_t* utow( const uint3& u ){	return atow(utoa(u)); }
inline const wchar_t* utow( const uint4& u ){	return atow(utoa(u)); }
inline const wchar_t* utow( const ushort2& v ){	return atow(utoa(v)); }
inline const wchar_t* utow( const ushort3& v ){	return atow(utoa(v)); }
inline const wchar_t* utow( const ushort4& v ){	return atow(utoa(v)); }
inline const wchar_t* utow( const uchar2& v ){	return atow(utoa(v)); }
inline const wchar_t* utow( const uchar3& v ){	return atow(utoa(v)); }
inline const wchar_t* utow( const uchar4& v ){	return atow(utoa(v)); }
inline const wchar_t* ftow( const float2& v ){	return atow(ftoa(v)); }
inline const wchar_t* ftow( const float3& v ){	return atow(ftoa(v)); }
inline const wchar_t* ftow( const float4& v ){	return atow(ftoa(v)); }
inline const wchar_t* ftow( const float9& m ){	return atow(ftoa(m)); }
inline const wchar_t* ftow( const float16& m ){	return atow(ftoa(m)); }
inline const wchar_t* dtow( const double2& v ){	return atow(dtoa(v)); }
inline const wchar_t* dtow( const double3& v ){	return atow(dtoa(v)); }
inline const wchar_t* dtow( const double4& v ){	return atow(dtoa(v)); }
inline const wchar_t* dtow( const double9& m ){	return atow(dtoa(m)); }
inline const wchar_t* dtow( const double16& m ){return atow(dtoa(m)); }

//***********************************************
// 8. fast manual conversion from string to int/float (3x--4x faster than CRT atoi/atof)

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

//***********************************************
// 9. conversion from string to user types

inline bool atob( const char* a ){		return _stricmp(a,"true")==0||fast::atoi(a)!=0; }
inline uint atou( const char* a ){		char* e=nullptr;uint v=(uint)strtoul(a,&e,10); return v; }
inline uint atou( const wchar_t* w ){	wchar_t* e=nullptr;uint v=(uint)wcstoul(w,&e,10); return v; }
inline uint64_t atoull( const char* a ){char* e=nullptr;uint64_t v=strtoull(a,&e,10); return v; }
inline int2 atoi2( const char* a ){		int2 v;char* e=nullptr;for(int k=0;k<2;k++,a=e) v[k]=(int)strtol(a,&e,10); return v; }
inline int3 atoi3( const char* a ){		int3 v;char* e=nullptr;for(int k=0;k<3;k++,a=e) v[k]=(int)strtol(a,&e,10); return v; }
inline int4 atoi4( const char* a ){		int4 v;char* e=nullptr;for(int k=0;k<4;k++,a=e) v[k]=(int)strtol(a,&e,10); return v; }
inline uint2 atou2( const char* a ){	uint2 v;char* e=nullptr;for(int k=0;k<2;k++,a=e) v[k]=strtoul(a,&e,10); return v; }
inline uint3 atou3( const char* a ){	uint3 v;char* e=nullptr;for(int k=0;k<3;k++,a=e) v[k]=strtoul(a,&e,10); return v; }
inline uint4 atou4( const char* a ){	uint4 v;char* e=nullptr;for(int k=0;k<4;k++,a=e) v[k]=strtoul(a,&e,10); return v; }
inline float2 atof2( const char* a ){	float2 v;char* e=nullptr;for(int k=0;k<2;k++,a=e) v[k]=strtof(a,&e); return v; }
inline float3 atof3( const char* a ){	float3 v;char* e=nullptr;for(int k=0;k<3;k++,a=e) v[k]=strtof(a,&e); return v; }
inline float4 atof4( const char* a ){	float4 v;char* e=nullptr;for(int k=0;k<4;k++,a=e) v[k]=strtof(a,&e); return v; }
inline float9 atof9( const char* a ){	float9 v;char* e=nullptr;for(int k=0;k<9;k++,a=e) v[k]=strtof(a,&e); return v; }
inline float16 atof16( const char* a ){	float16 v;char* e=nullptr;for(int k=0;k<16;k++,a=e) v[k]=strtof(a,&e); return v; }
inline double2 atod2( const char* a ){	double2 v;char* e=nullptr;for(int k=0;k<2;k++,a=e) v[k]=strtod(a,&e); return v; }
inline double3 atod3( const char* a ){	double3 v;char* e=nullptr;for(int k=0;k<3;k++,a=e) v[k]=strtod(a,&e); return v; }
inline double4 atod4( const char* a ){	double4 v;char* e=nullptr;for(int k=0;k<4;k++,a=e) v[k]=strtod(a,&e); return v; }
inline double9 atod9( const char* a ){	double9 v;char* e=nullptr;for(int k=0;k<9;k++,a=e) v[k]=strtod(a,&e); return v; }
inline double16 atod16( const char* a ){double16 v;char* e=nullptr;for(int k=0;k<16;k++,a=e) v[k]=strtod(a,&e); return v; }

inline bool atob( const std::string& s ){		return atob(s.c_str()); }
inline int atoi( const std::string& s ){		return int(fast::atoi(s.c_str())); }
inline float atof( const std::string& s ){		return float(fast::atof(s.c_str())); }
inline uint atou( const std::string& s ){		return atou(s.c_str()); }
inline uint64_t atoull( const std::string& s ){	return atoull(s.c_str()); }
inline int2 atoi2( const std::string& s ){		return atoi2(s.c_str()); }
inline int3 atoi3( const std::string& s ){		return atoi3(s.c_str()); }
inline int4 atoi4( const std::string& s ){		return atoi4(s.c_str()); }
inline uint2 atou2( const std::string& s ){		return atou2(s.c_str()); }
inline uint3 atou3( const std::string& s ){		return atou3(s.c_str()); }
inline uint4 atou4( const std::string& s ){		return atou4(s.c_str()); }
inline float2 atof2( const std::string& s ){	return atof2(s.c_str()); }
inline float3 atof3( const std::string& s ){	return atof3(s.c_str()); }
inline float4 atof4( const std::string& s ){	return atof4(s.c_str()); }
inline float9 atof9( const std::string& s ){	return atof9(s.c_str()); }
inline float16 atof16( const std::string& s ){	return atof16(s.c_str()); }
inline double2 atod2( const std::string& s ){	return atod2(s.c_str()); }
inline double3 atod3( const std::string& s ){	return atod3(s.c_str()); }
inline double4 atod4( const std::string& s ){	return atod4(s.c_str()); }
inline double9 atod9( const std::string& s ){	return atod9(s.c_str()); }
inline double16 atod16( const std::string& s ){	return atod16(s.c_str()); }

//***********************************************
// 10. conversion from wstring to user types
inline int atoi( const wchar_t* w ){		return fast::atoi(wtoa(w)); }
inline double atof( const wchar_t* w ){		return fast::atof(wtoa(w)); }
inline int wtoi( const std::wstring& w ){	return fast::atoi(wtoa(w.c_str())); }
inline double wtof( const std::wstring& w ){return fast::atof(wtoa(w.c_str())); }
inline int atoi( const std::wstring& w ){	return fast::atoi(wtoa(w.c_str())); }
inline double atof( const std::wstring& w ){return fast::atof(wtoa(w.c_str())); }
inline bool wtob( const wchar_t* w ){		return w&&w[0]&&(_wcsicmp(w,L"true")==0||wtoi(w)!=0); }
inline uint wtou( const wchar_t* w ){		return atou(wtoa(w)); }
inline uint64_t wtoull( const wchar_t* w ){	return atoull(wtoa(w)); }

inline int2 wtoi2( const wchar_t* w ){		return atoi2(wtoa(w)); }
inline int3 wtoi3( const wchar_t* w ){		return atoi3(wtoa(w)); }
inline int4 wtoi4( const wchar_t* w ){		return atoi4(wtoa(w)); }
inline uint2 wtou2( const wchar_t* w ){		return atou2(wtoa(w)); }
inline uint3 wtou3( const wchar_t* w ){		return atou3(wtoa(w)); }
inline uint4 wtou4( const wchar_t* w ){		return atou4(wtoa(w)); }
inline float2 wtof2( const wchar_t* w ){	return atof2(wtoa(w)); }
inline float3 wtof3( const wchar_t* w ){	return atof3(wtoa(w)); }
inline float4 wtof4( const wchar_t* w ){	return atof4(wtoa(w)); }
inline float9 wtof9( const wchar_t* w ){	return atof9(wtoa(w)); }
inline float16 wtof16( const wchar_t* w ){	return atof16(wtoa(w)); }
inline double2 wtod2( const wchar_t* w ){	return atod2(wtoa(w)); }
inline double3 wtod3( const wchar_t* w ){	return atod3(wtoa(w)); }
inline double4 wtod4( const wchar_t* w ){	return atod4(wtoa(w)); }
inline double9 wtod9( const wchar_t* w ){	return atod9(wtoa(w)); }
inline double16 wtod16( const wchar_t* w ){	return atod16(wtoa(w)); }

inline bool wtob( const std::wstring& w ){		return wtob(w.c_str()); }
inline uint wtou( const std::wstring& w ){		return wtou(w.c_str()); }
inline uint64_t wtoull( const std::wstring& w ){return wtoull(w.c_str()); }
inline int2 wtoi2( const std::wstring& w ){		return wtoi2(w.c_str()); }
inline int3 wtoi3( const std::wstring& w ){		return wtoi3(w.c_str()); }
inline int4 wtoi4( const std::wstring& w ){		return wtoi4(w.c_str()); }
inline uint2 wtou2( const std::wstring& w ){	return wtou2(w.c_str()); }
inline uint3 wtou3( const std::wstring& w ){	return wtou3(w.c_str()); }
inline uint4 wtou4( const std::wstring& w ){	return wtou4(w.c_str()); }
inline float2 wtof2( const std::wstring& w ){	return wtof2(w.c_str()); }
inline float3 wtof3( const std::wstring& w ){	return wtof3(w.c_str()); }
inline float4 wtof4( const std::wstring& w ){	return wtof4(w.c_str()); }
inline float9 wtof9( const std::wstring& w ){	return wtof9(w.c_str()); }
inline float16 wtof16( const std::wstring& w ){	return wtof16(w.c_str()); }
inline double2 wtod2( const std::wstring& w ){	return wtod2(w.c_str()); }
inline double3 wtod3( const std::wstring& w ){	return wtod3(w.c_str()); }
inline double4 wtod4( const std::wstring& w ){	return wtod4(w.c_str()); }
inline double9 wtod9( const std::wstring& w ){	return wtod9(w.c_str()); }
inline double16 wtod16( const std::wstring& w ){return wtod16(w.c_str()); }

//***********************************************
// 11. trim

template <class T>
inline const T* ltrim( const T* src, const T* symbols=_strcvt<T>(" \t\n") )
{
	return !src||!src[0]?reinterpret_cast<const T*>(L""):__tstrdup(src,strlen(src))+strspn(src,symbols);
}

template <class T>
inline const T* rtrim( const T* src, const T* symbols=_strcvt<T>(" \t\n") )
{
	return !src||!src[0]?reinterpret_cast<const T*>(L""):__tstrdup(src,strlen(src)-_strrspn(src,symbols));
}

template <class T>
inline const T* trim( const T* src, const T* symbols=_strcvt<T>(" \t\n") )
{
	if(!src||!src[0]) return _strcvt<T>(""); const T* r=rtrim(src,symbols); return r+(r&&r[0]?strspn(src,symbols):0);
}

template <class T>
inline const T* trim_comment( const T* src, const char* comment_symbols="#" )
{
	if(src==nullptr||src[0]==0) return src;
	size_t slen=strlen(src),clen=strlen(comment_symbols),sc=slen-clen+1;
	T* buff=__tstrdup(src,slen); const char* cs=comment_symbols;
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
	buff[slen-_strrspn(buff,_strcvt<T>(" \t\n"))]=0; // rtrim
	return buff;
}

//***********************************************
// 12. explode/join

template <class T>
__noinline inline const T* join( std::vector<std::basic_string<T,std::char_traits<T>,std::allocator<T>>> v, const T* delim=_strcvt<T>(" ") )
{
	std::basic_string<T,std::char_traits<T>,std::allocator<T>> s;
	for( size_t k=0, kn=v.size(); k<kn; k++ ){ if(k>0) s+=decltype(s)(delim); s+=v[k]; }
	return __tstrdup(s.c_str());
}

template <class T>
__noinline inline std::vector<std::basic_string<T,std::char_traits<T>,std::allocator<T> > >
explode( const T* src, const T* seps=_strcvt<T>(" \t\n") )
{
	std::vector<std::basic_string<T,std::char_traits<T>,std::allocator<T> > > vs; vs.reserve(32);
	T *ctx, *token = (T*) strtok_s(__tstrdup(src),seps,&ctx);
	while(token!=nullptr){ vs.emplace_back(token); token=strtok_s(nullptr,seps,&ctx); }
	return vs;
}

template <class T>
__noinline inline std::set<std::basic_string<T,std::char_traits<T>,std::allocator<T> > >
explode_set( const T* src, const T* seps=_strcvt<T>(" \t\n") )
{
	std::set<std::basic_string<T,std::char_traits<T>,std::allocator<T> > > vs;
	T *ctx, *token = (T*)strtok_s(__tstrdup(src), seps, &ctx);
	while(token!=nullptr){ vs.emplace(token); token=strtok_s(nullptr,seps,&ctx); }
	return vs;
}

template <class T>
__noinline inline std::vector<int> explodei( const T* src, const T* seps=_strcvt<T>(" \t\n") )
{
	std::vector<int> vs; vs.reserve(32);
	T *ctx, *token = (T*)strtok_s(__tstrdup(src), seps, &ctx);
	while(token!=nullptr){ vs.emplace_back(atoi(token)); token=strtok_s(nullptr,seps,&ctx); }
	return vs;
}

template <class T>
__noinline inline std::vector<unsigned int> explodeu( const T* src, const T* seps=_strcvt<T>(" \t\n") )
{
	std::vector<unsigned int> vs; vs.reserve(32);
	T *ctx, *token = (T*)strtok_s(__tstrdup(src), seps, &ctx);
	while(token!=nullptr){ vs.emplace_back(atou(token)); token=strtok_s(nullptr,seps,&ctx); }
	return vs;
}

template <class T>
__noinline inline std::vector<float> explodef( const T* src, const T* seps=_strcvt<T>(" \t\n") )
{
	std::vector<float>  vs; vs.reserve(32);
	T *ctx, *token = (T*)strtok_s(__tstrdup(src), seps, &ctx);
	while(token!=nullptr){ vs.emplace_back(float(fast::atof(token))); token=strtok_s(nullptr,seps,&ctx); }
	return vs;
}

template <class T>
__noinline inline std::vector<double> exploded( const T* src, const T* seps=_strcvt<T>(" \t\n") )
{
	std::vector<double>  vs; vs.reserve(32);
	T *ctx, *token = (T*)strtok_s(__tstrdup(src), seps, &ctx);
	while(token!=nullptr){ vs.emplace_back(double(fast::atof(token))); token=strtok_s(nullptr,seps,&ctx); }
	return vs;
}

template <class T>
__noinline inline std::vector<const T*> explode_conservative( const T* _Src, T _Delim )
{
	std::vector<const T*> vs; vs.reserve(16); if(_Src==nullptr) return vs;
	for(T *s=__tstrdup(_Src),*e=s;*s&&*e;s=e+1){for(e=s;*e!=_Delim&&*e;e++){}; vs.emplace_back(__tstrdup(s,size_t(e-s))); }
	return vs;
}

//***********************************************
// 13. replace
template <class T>
__noinline inline const T* str_replace( const T* _Src, const T* _Find, const T* _Replace )
{
	if(_Find==nullptr||_Find[0]==0) return __tstrdup(_Src);	// no change
	int sl=int(strlen(_Src)), fl=int(strlen(_Find)); if(sl<fl) return __tstrdup(_Src);
	int rl=int(strlen(_Replace));
	T *s=(T*)_Src, *p=nullptr;
	std::vector<T> buff; buff.reserve(sl*2); while( nullptr!=(p=(T*)strstr(s,_Find)) ){ buff.insert(buff.end(),s,p); if(rl>0) buff.insert(buff.end(),_Replace,_Replace+rl); s=p+fl; }
	buff.insert(buff.end(),s,(T*)(_Src+sl));buff.emplace_back(0);
	return __tstrdup(&buff[0],buff.size());
}

template <class T>
__noinline inline const T* str_ireplace( const T* _Src, const T* _Find, const T* _Replace )
{
	if(_Find==nullptr||_Find[0]==0) return __tstrdup(_Src);	// no change
	int sl=int(strlen(_Src)), fl=int(strlen(_Find)); if(sl<fl) return __tstrdup(_Src); int rl=int(strlen(_Replace));
	T *s=(T*)_Src, *p=nullptr;
	std::vector<T> buff; buff.reserve(sl*2); while( nullptr!=(p=(T*)_stristr(s,_Find)) ){ buff.insert(buff.end(),s,p); buff.insert(buff.end(),_Replace,_Replace+rl); s=p+fl; }
	buff.insert(buff.end(),s,(T*)(_Src+sl));buff.emplace_back(0);
	return __tstrdup(&buff[0],buff.size());
}

template <class T>
__noinline inline const T* str_replace( const T* _Src, T _Find, T _Replace )
{
	T *s=__tstrdup(_Src), *p=s;
	for(int k=0,l=int(strlen(s));k<l;k++,p++) if(*p==_Find) *p=_Replace;
	return s;
}

//***********************************************
#endif // __GX_STRING_H__