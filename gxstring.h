#pragma once
#ifndef __GX_STRING_H__
#define __GX_STRING_H__
//###################################################################
// COMMON HEADERS for GXUT
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
// C standard
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
// STL
#include <algorithm>
#include <array>
#include <map>
#include <set>
#include <string>
#include <vector>
// C++11
#if (_MSC_VER>=1600/*VS2010*/) || (__cplusplus>199711L)
	#include <chrono>
	#include <functional>
	#include <thread>
	#include <typeindex>
	#include <type_traits>
	#include <unordered_map>
	#include <unordered_set>
	using namespace std::placeholders;
#endif
// windows
#if !defined(__GNUC__)&&(defined(_WIN32)||defined(_WIN64))
	#include <windows.h>
	#include <wchar.h>
	#include <stdint.h>
#elif !defined(__forceinline) // GCC and clang
	#define __forceinline inline __attribute__((__always_inline__))
	#include <inttypes.h> // defines int64_t, uint64_t
#endif
#if defined(__clang__) // clang-specific preprocessor
	#pragma clang diagnostic ignored "-Wmissing-braces"				// ignore excessive warning for initialzer
	#pragma clang diagnostic ignored "-Wdelete-non-virtual-dtor"	// ignore non-virtual destructor
#endif
// common macros
#ifndef __REDIR_WIDE_STDIO
	#define __REDIR_WIDE_STDIO
	inline int __wprintf_r( const wchar_t* fmt,... ){va_list a; va_start(a,fmt);int len=_vscwprintf(fmt,a);static int l=0;static wchar_t* w=nullptr;if(l<len)w=(wchar_t*)realloc(w,((l=len)+1)*sizeof(wchar_t));vswprintf_s(w,len+1,fmt,a); va_end(a);int mblen=WideCharToMultiByte(CP_ACP,0,w,-1,0,0,0,0);static int m=0;static char* b=nullptr;if(m<mblen)b=(char*)realloc(b,((m=mblen)+1)*sizeof(char));WideCharToMultiByte(CP_ACP,0,w,-1,b,mblen,nullptr,nullptr);return fputs(b,stdout);}
	#define wprintf(...)	__wprintf_r(__VA_ARGS__)
#endif
#ifndef SAFE_RELEASE
	#define SAFE_RELEASE(a) {if(a){a->Release();a=nullptr;}}
#endif
#ifndef SAFE_DELETE
	#define SAFE_DELETE(p) {if(p){delete p;p=nullptr;}}
#endif
#ifndef SAFE_FREE
	#define SAFE_FREE(p) {if(p){free(p);p=nullptr;}}
#endif
// user types
#ifndef __TARRAY__
#define __TARRAY__
template <class T> struct _tarray2 { union{ struct { T r, g; }; struct { T x, y; }; }; __forceinline T& operator[]( int i ){ return (&x)[i]; } __forceinline const T& operator[]( int i ) const { return (&x)[i]; } };
template <class T> struct _tarray3 { union{ struct { T r, g, b; }; struct { T x, y, z; }; union{ _tarray2<T> xy; _tarray2<T> rg; struct { T _x; union{ _tarray2<T> yz; _tarray2<T> gb; }; }; }; }; __forceinline T& operator[]( int i ){ return (&x)[i]; } __forceinline const T& operator[]( int i ) const { return (&x)[i]; } };
template <class T> struct _tarray4 { union{ struct { T r, g, b, a; }; struct { T x, y, z, w; }; struct { union{ _tarray2<T> xy; _tarray2<T> rg; }; union{ _tarray2<T> zw; _tarray2<T> ba; }; }; union{ _tarray3<T> xyz; _tarray3<T> rgb; }; struct { T _x; union{ _tarray3<T> yzw; _tarray3<T> gba; _tarray2<T> yz; _tarray2<T> gb; }; }; }; __forceinline T& operator[]( int i ){ return (&x)[i]; } __forceinline const T& operator[]( int i ) const { return (&x)[i]; } };
template <class T> struct _tarray9 { union{ T a[9]; struct {T _11,_12,_13,_21,_22,_23,_31,_32,_33;}; }; __forceinline T& operator[]( int i ){ return a[i]; } __forceinline const T& operator[]( int i ) const { return a[i]; } };
template <class T> struct _tarray16{ union{ T a[16]; struct {T _11,_12,_13,_14,_21,_22,_23,_24,_31,_32,_33,_34,_41,_42,_43,_44;}; }; __forceinline T& operator[]( int i ){ return a[i]; } __forceinline const T& operator[]( int i ) const { return a[i]; } };
#endif
using uint		=  unsigned int;		using uchar		= unsigned char;		using ushort	= unsigned short;
using float2	=  _tarray2<float>;		using float3	= _tarray3<float>;		using float4	= _tarray4<float>;
using double2	=  _tarray2<double>;	using double3	= _tarray3<double>;		using double4	= _tarray4<double>;
using int2		=  _tarray2<int>;		using int3		= _tarray3<int>;		using int4		= _tarray4<int>;
using uint2		=  _tarray2<uint>;		using uint3		= _tarray3<uint>;		using uint4		= _tarray4<uint>;
using short2	=  _tarray2<short>;		using short3	= _tarray3<short>;		using short4	= _tarray4<short>;
using ushort2	=  _tarray2<ushort>;	using ushort3	= _tarray3<ushort>;		using ushort4	= _tarray4<ushort>;
using char2		=  _tarray2<char>;		using char3		= _tarray3<char>;		using char4		= _tarray4<char>;
using uchar2	=  _tarray2<uchar>;		using uchar3	= _tarray3<uchar>;		using uchar4	= _tarray4<uchar>;
using bool2		=  _tarray2<bool>;		using bool3		= _tarray3<bool>;		using bool4		= _tarray4<bool>;
using float9	=  _tarray9<float>;		using float16	= _tarray16<float>;
using double9	=  _tarray9<double>;	using double16	= _tarray16<double>;
// end COMMON HEADERS for GXUT
//###################################################################

#ifndef SHARED_CIRCULAR_BUFFER_SIZE
#define SHARED_CIRCULAR_BUFFER_SIZE 8192
#endif

// Byte order masks for UTF encoding
static const unsigned char BOM_UTF8[3]={0xEF,0xBB,0xBF};
static const unsigned char BOM_UTF16[2]={0xFF,0xFE};			// little endian
static const unsigned char BOM_UTF32[4]={0xFF,0xFE,0x00,0x00};	// little endian

//*******************************************************************
// 0. overloaded string functions for wchar_t
inline wchar_t* strcpy( wchar_t* _Dest, const wchar_t* _Source ){ return wcscpy(_Dest,_Source); }
inline wchar_t* strncpy( wchar_t* _Dest, const wchar_t* _Source, size_t _Count ){ return wcsncpy(_Dest,_Source,_Count); }
inline wchar_t* strcat( wchar_t* _Dest, const wchar_t* _Source ){ return wcscat(_Dest,_Source); }
inline wchar_t* strncat( wchar_t* _Dest, const wchar_t* _Source, size_t _Count ){ return wcsncat(_Dest,_Source,_Count); }
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
inline wchar_t* _strlwr( wchar_t* _Str ){ return _wcslwr(_Str); } 
inline wchar_t* _strupr( wchar_t* _Str ){ return _wcsupr(_Str); }
inline int _stricmp( const wchar_t* _Str1, const wchar_t* _Str2 ){ return _wcsicmp(_Str1,_Str2); }
// 0.2 slee extensions
template <class T> inline size_t _strrspn( const T* _Str, const T* _Control ){size_t L=strlen(_Str),C=strlen(_Control),k=0,j=0;for(k=0;k<L;k++){for(j=0;j<C;j++)if(_Str[L-1-k]==_Control[j])break;if(j==C)break;}return k;}
inline const char* _stristr( const char* _Str1, const char* _Str2 ){ char* s1=_strlwr(_strdup(_Str1)); char* s2=_strlwr(_strdup(_Str2)); const char* r=strstr(s1,s2); if(r)r=_Str1+(r-s1); free(s1); free(s2); return r; }
inline const wchar_t* _stristr( const wchar_t* _Str1, const wchar_t* _Str2 ){ wchar_t* s1=_wcslwr(_wcsdup(_Str1)); wchar_t* s2=_wcslwr(_wcsdup(_Str2)); const wchar_t* r=wcsstr(s1,s2); if(r)r=_Str1+(r-s1); free(s1); free(s2); return r; }
inline const wchar_t* _wcsistr( const wchar_t* _Str1, const wchar_t* _Str2 ){ wchar_t* s1=_wcslwr(_wcsdup(_Str1)); wchar_t* s2=_wcslwr(_wcsdup(_Str2)); const wchar_t* r=wcsstr(s1,s2); if(r)r=_Str1+(r-s1); free(s1); free(s2); return r; }

//*******************************************************************
// 0.3 case-insensitive comparison for std::map/set, std::unordered_map/set
namespace nocase
{
	template <class T> struct equal_to:public std::binary_function<T,T,bool> { bool operator()(const T& a,const T& b)const;};
	template <class T> struct not_equal_to:public std::binary_function<T,T,bool> { bool operator()(const T& a,const T& b)const;};
	template <class T> struct greater:public std::binary_function<T,T,bool> { bool operator()(const T& a,const T& b)const;};
	template <class T> struct less:public std::binary_function<T,T,bool> { bool operator()(const T& a,const T& b)const;};
	template <class T> struct greater_equal:public std::binary_function<T,T,bool> { bool operator()(const T& a,const T& b)const;};
	template <class T> struct less_equal:public std::binary_function<T,T,bool> { bool operator()(const T& a,const T& b)const;};

	template <> struct equal_to<std::string>:public std::binary_function<std::string,std::string,bool> { bool operator()(const std::string& a,const std::string& b)const{return _stricmp(a.c_str(),b.c_str())==0;}};
	template <> struct not_equal_to<std::string>:public std::binary_function<std::string,std::string,bool> { bool operator()(const std::string& a,const std::string& b)const{return _stricmp(a.c_str(),b.c_str())!=0;}};
	template <> struct greater<std::string>:public std::binary_function<std::string,std::string,bool> { bool operator()(const std::string& a,const std::string& b)const{return _stricmp(a.c_str(),b.c_str())>0;}};
	template <> struct less<std::string>:public std::binary_function<std::string,std::string,bool> { bool operator()(const std::string& a,const std::string& b)const{return _stricmp(a.c_str(),b.c_str())<0;}};
	template <> struct greater_equal<std::string>:public std::binary_function<std::string,std::string,bool> { bool operator()(const std::string& a,const std::string& b)const{return _stricmp(a.c_str(),b.c_str())>=0;}};
	template <> struct less_equal<std::string>:public std::binary_function<std::string,std::string,bool> { bool operator()(const std::string& a,const std::string& b)const{return _stricmp(a.c_str(),b.c_str())<=0;}};

	template <> struct equal_to<std::wstring>:public std::binary_function<std::wstring,std::wstring,bool> { bool operator()(const std::wstring& a,const std::wstring& b)const{return _wcsicmp(a.c_str(),b.c_str())==0;}};
	template <> struct not_equal_to<std::wstring>:public std::binary_function<std::wstring,std::wstring,bool> { bool operator()(const std::wstring& a,const std::wstring& b)const{return _wcsicmp(a.c_str(),b.c_str())!=0;}};
	template <> struct greater<std::wstring>:public std::binary_function<std::wstring,std::wstring,bool> { bool operator()(const std::wstring& a,const std::wstring& b)const{return _wcsicmp(a.c_str(),b.c_str())>0;}};
	template <> struct less<std::wstring>:public std::binary_function<std::wstring,std::wstring,bool> { bool operator()(const std::wstring& a,const std::wstring& b)const{return _wcsicmp(a.c_str(),b.c_str())<0;}};
	template <> struct greater_equal<std::wstring>:public std::binary_function<std::wstring,std::wstring,bool> { bool operator()(const std::wstring& a,const std::wstring& b)const{return _wcsicmp(a.c_str(),b.c_str())>=0;}};
	template <> struct less_equal<std::wstring>:public std::binary_function<std::wstring,std::wstring,bool> { bool operator()(const std::wstring& a,const std::wstring& b)const{return _wcsicmp(a.c_str(),b.c_str())<=0;}};

	// template aliases
	template <class T> using			set = std::set<T,nocase::less<T>>;
	template <class T, class V> using	map = std::map<T,V,nocase::less<T>>;
	template <class T> using			unordered_set = std::unordered_set<T,std::hash<T>,nocase::equal_to<T>>;
	template <class T, class V> using	unordered_map = std::unordered_map<T,V,std::hash<T>,nocase::equal_to<T>>;
}

//*******************************************************************
// 1. shared circular buffers
template <class T> inline T* __tstrbuf( size_t len ){static T* C[SHARED_CIRCULAR_BUFFER_SIZE]={0};static uint cid=0;T*& p=C[cid=((cid+1)%SHARED_CIRCULAR_BUFFER_SIZE)];p=(T*)realloc(p,sizeof(T)*(len+1));p[len]=0;return p;}
template <class T> inline T* __tstrdup( const T* s,size_t slen=-1 ){ if(slen==-1)slen=strlen(s); return (T*)memcpy(__tstrbuf<T>(slen),s,sizeof(T)*slen); }
inline char* _strbuf( size_t len ){ return __tstrbuf<char>(len); }
inline wchar_t* _wcsbuf( size_t len ){ return __tstrbuf<wchar_t>(len); }

//*******************************************************************
// 2. format
inline const char* vformat( va_list a, const char* fmt ){ size_t len=size_t(_vscprintf(fmt,a)); char* buffer=_strbuf(len); vsprintf_s(buffer,len+1,fmt,a); return buffer; }
inline const wchar_t* vformat( va_list a, const wchar_t* fmt ){ size_t len=size_t(_vscwprintf(fmt,a)); wchar_t* bufferW=_wcsbuf(len); vswprintf_s(bufferW,len+1,fmt,a); return bufferW; }
inline const char* format( const char* fmt,... ){ va_list a; va_start(a,fmt); size_t len=size_t(_vscprintf(fmt,a)); char* buffer=_strbuf(len); vsprintf_s(buffer,len+1,fmt,a); va_end(a); return buffer; }
inline const wchar_t* format( const wchar_t* fmt,... ){ va_list a; va_start(a,fmt); size_t len=size_t(_vscwprintf(fmt,a)); wchar_t* bufferW=_wcsbuf(len); vswprintf_s(bufferW,len+1,fmt,a); va_end(a); return bufferW; }

//*******************************************************************
// 3. case conversion
template <class T> inline const T* tolower( const T* src ){ return _strlwr(__tstrdup(src)); }
template <class T> inline const T* toupper( const T* src ){ return _strupr(__tstrdup(src)); }
inline const char* tovarname( const char* src ){ if(!src||!*src) return ""; char *s=(char*)src,*dst=__tstrbuf<char>(strlen(src)+2), *d=dst; if(!isalpha(*s)&&(*s)!='_') *(d++)='_'; for(;*s;s++,d++) *d=isalnum(*s)?(*s):'_'; *d='\0'; return dst; }

//*******************************************************************
// 4. conversion between const wchar_t* and const char*
template <class T,class I> const T* _strcvt( const I* s ){size_t len=strlen(s);T* buff=__tstrbuf<T>(len);for(uint k=0;k<len;k++)buff[k]=T(s[k]);return buff;}
inline const wchar_t* atow( const char* a ){int wlen=MultiByteToWideChar(CP_ACP,0,a,-1,0,0);wchar_t* wbuff=_wcsbuf(wlen);MultiByteToWideChar(CP_ACP,0,a,-1,wbuff,wlen);return wbuff;}
inline const char* wtoa( const wchar_t* w ){int mblen=WideCharToMultiByte(CP_ACP,0,w,-1,0,0,0,0);char* buff=_strbuf(mblen);WideCharToMultiByte(CP_ACP,0,w,-1,buff,mblen,nullptr,nullptr);return buff;}

//*******************************************************************
// 5. conversion to string types
inline const char* btoa( bool b ){ return b?"1":"0"; }
inline const char* itoa( int i ){static const char* fmt="%d";size_t size=size_t(_scprintf(fmt,i));char* buff=_strbuf(size); sprintf_s(buff,size+1,fmt,i);return buff;}
inline const char* utoa( uint u ){static const char* fmt="%u";size_t size=size_t(_scprintf(fmt,u));char* buff=_strbuf(size); sprintf_s(buff,size+1,fmt,u);return buff;}
inline const char* ftoa( float f ){if(fabs(f)<0.00001f)return "0";static const char* fmt="%g";size_t size=size_t(_scprintf(fmt,f));char* buff=_strbuf(size); sprintf_s(buff,size+1,fmt,f); return buff; }
inline const char* dtoa( double d ){if(fabs(d)<0.0000001)return "0";static const char* fmt="%g";size_t size=size_t(_scprintf(fmt,d));char* buff=_strbuf(size); sprintf_s(buff,size+1,fmt,d); return buff; }
inline const char* ulltoa( uint64_t u ){ static const char* fmt="%Iu";size_t size=size_t(_scprintf(fmt,u));char* buff=_strbuf(size); sprintf_s(buff,size+1,fmt,u);return buff; }
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

//*******************************************************************
// 5.2 Special-purpose functions

// conversion int to string with commas
inline const char* itoasep( int n )
{
	if(n<1000&&n>-1000) return itoa(n);
	const char* s=itoa(n>0?n:-n); size_t len=strlen(s);
	std::vector<char> v; v.resize(len+1); memcpy(&v[0],s,len+1);
	for( uint idx=(len%3?len%3:3); idx<len; idx+=4,len++ ) v.insert(v.begin()+idx,',');
	return format("%s%s",n>0?"":"-",&v[0]);
}

//*******************************************************************
// 6. conversion to wstring types
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

//*******************************************************************
// 7. conversion from string to user types
inline bool atob( const char* a ){ return _stricmp(a,"true")==0||atoi(a)!=0; }
inline uint atou( const char* a ){uint v;sscanf(a,"%u",&v);return v; }
inline uint atou( const wchar_t* a ){uint v;swscanf(a,L"%u",&v);return v; }
inline uint64_t atoull( const char* a ){uint64_t v;sscanf(a,"%llu",&v); return v; }
inline int2 atoi2( const char* a ){int2 v;sscanf(a,"%d %d",&v[0],&v[1]);return v; }
inline int3 atoi3( const char* a ){int3 v;sscanf(a,"%d %d %d",&v[0],&v[1],&v[2]);return v; }
inline int4 atoi4( const char* a ){int4 v;sscanf(a,"%d %d %d %d",&v[0],&v[1],&v[2],&v[3]);return v; }
inline uint2 atou2( const char* a ){uint2 v;sscanf(a,"%u %u",&v[0],&v[1]);return v; }
inline uint3 atou3( const char* a ){uint3 v;sscanf(a,"%u %u %u",&v[0],&v[1],&v[2]);return v; }
inline uint4 atou4( const char* a ){uint4 v;sscanf(a,"%u %u %u %u",&v[0],&v[1],&v[2],&v[3]);return v; }
inline float2 atof2( const char* a ){float2 v; sscanf(a,"%f %f",&v[0],&v[1]);return v;}
inline float3 atof3( const char* a ){float3 v; sscanf(a,"%f %f %f",&v[0],&v[1],&v[2]);return v;}
inline float4 atof4( const char* a ){float4 v; sscanf(a,"%f %f %f %f",&v[0],&v[1],&v[2],&v[3]);return v;}
inline float9 atof9( const char* a ){float9 v; sscanf(a,"%f %f %f %f %f %f %f %f %f",&v[0],&v[1],&v[2],&v[3],&v[4],&v[5],&v[6],&v[7],&v[8]);return v; }
inline float16 atof16( const char* a ){float16 v; sscanf(a,"%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",&v[0],&v[1],&v[2],&v[3],&v[4],&v[5],&v[6],&v[7],&v[8],&v[9],&v[10],&v[11],&v[12],&v[13],&v[14],&v[15]);return v; }
inline double2 atod2( const char* a ){double2 v; sscanf(a,"%lf %lf",&v[0],&v[1]);return v;}
inline double3 atod3( const char* a ){double3 v; sscanf(a,"%lf %lf %lf",&v[0],&v[1],&v[2]);return v;}
inline double4 atod4( const char* a ){double4 v; sscanf(a,"%lf %lf %lf %lf",&v[0],&v[1],&v[2],&v[3]);return v;}
inline double9 atod9( const char* a ){double9 v; sscanf(a,"%lf %lf %lf %lf %lf %lf %lf %lf %lf",&v[0],&v[1],&v[2],&v[3],&v[4],&v[5],&v[6],&v[7],&v[8]);return v; }
inline double16 atod16( const char* a ){double16 v; sscanf(a,"%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",&v[0],&v[1],&v[2],&v[3],&v[4],&v[5],&v[6],&v[7],&v[8],&v[9],&v[10],&v[11],&v[12],&v[13],&v[14],&v[15]);return v; }

inline bool atob( const std::string& s ){ return atob(s.c_str()); }
inline int atoi( const std::string& s ){ return int(atoi(s.c_str())); }
inline float atof( const std::string& s ){	return float(atof(s.c_str())); }
inline uint atou( const std::string& s ){ return atou(s.c_str()); }
inline uint64_t atoull( const std::string& s ){ return atoull(s.c_str()); }
inline int2 atoi2( const std::string& s ){ return atoi2(s.c_str()); }
inline int3 atoi3( const std::string& s ){ return atoi3(s.c_str()); }
inline int4 atoi4( const std::string& s ){ return atoi4(s.c_str()); }
inline uint2 atou2( const std::string& s ){ return atou2(s.c_str()); }
inline uint3 atou3( const std::string& s ){ return atou3(s.c_str()); }
inline uint4 atou4( const std::string& s ){ return atou4(s.c_str()); }
inline float2 atof2( const std::string& s ){ return atof2(s.c_str()); }
inline float3 atof3( const std::string& s ){ return atof3(s.c_str()); }
inline float4 atof4( const std::string& s ){ return atof4(s.c_str()); }
inline float9 atof9( const std::string& s ){ return atof9(s.c_str()); }
inline float16 atof16( const std::string& s ){ return atof16(s.c_str()); }
inline double2 atod2( const std::string& s ){ return atod2(s.c_str()); }
inline double3 atod3( const std::string& s ){ return atod3(s.c_str()); }
inline double4 atod4( const std::string& s ){ return atod4(s.c_str()); }
inline double9 atod9( const std::string& s ){ return atod9(s.c_str()); }
inline double16 atod16( const std::string& s ){ return atod16(s.c_str()); }

//*******************************************************************
// 8. conversion from wstring to user types
inline int atoi( const wchar_t* w ){		return int(_wtoi(w)); }
inline float atof( const wchar_t* w ){		return float(_wtof(w)); }
inline int wtoi( const std::wstring& w ){	return int(_wtoi(w.c_str())); }
inline float wtof( const std::wstring& w ){	return float(_wtof(w.c_str())); }
inline int atoi( const std::wstring& w ){	return int(_wtoi(w.c_str())); }
inline float atof( const std::wstring& w ){	return float(_wtof(w.c_str())); }
inline bool wtob( const wchar_t* w ){		return _wcsicmp(w,L"true")==0||_wtoi(w)!=0; }
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

//*******************************************************************
// 9. trim
template <class T>
inline const T* trim( const T* src, const T* junk=_strcvt<T>(" \t\n") )
{
	if(src==nullptr||src[0]==0) return __tstrdup(src,0);
	size_t slen=strlen(src);
	T* s=__tstrdup(src,slen);
	s[slen-_strrspn(src,junk)]=0;		// rtrim
	return s+strspn(src,junk);			// ltrim
}

template <class T>
inline const T* trim_comment( const T* src, const T* commentMark=_strcvt<T>("#") )
{
	if(src==nullptr||src[0]==0) return src;
	size_t slen=strlen(src),clen=strlen(commentMark),sc=slen-clen+1;
	T* s=__tstrdup(src,slen);
	for(size_t k=0;k<sc;k++){size_t c=0;for(;c<clen;c++)if(s[k+c]!=commentMark[c])break;if(c==clen){s[k]=0;break;}}
	return trim(s);
}

//*******************************************************************
// 10. explode
template <class T>
inline std::vector<std::basic_string<T,std::char_traits<T>,std::allocator<T> > >
explode( const T* src, const T* seps=_strcvt<T>(" \t\n") )
{
	std::vector<std::basic_string<T,std::char_traits<T>,std::allocator<T> > > vs;
	T *ctx, *token = (T*) strtok_s(__tstrdup(src),seps,&ctx);
	while(token!=nullptr){ vs.push_back(token); token=strtok_s(nullptr,seps,&ctx); }
	return vs;
}

template <class T>
inline std::set<std::basic_string<T,std::char_traits<T>,std::allocator<T> > >
explode_set( const T* src, const T* seps=_strcvt<T>(" \t\n") )
{
	std::set<std::basic_string<T,std::char_traits<T>,std::allocator<T> > > vs;
	T *ctx, *token = (T*)strtok_s(__tstrdup(src), seps, &ctx);
	while(token!=nullptr){ vs.insert(token); token=strtok_s(nullptr,seps,&ctx); }
	return vs;
}

template <class T>
inline std::vector<int> explodei( const T* src, const T* seps=_strcvt<T>(" \t\n") )
{
	std::vector<int> vs;
	T *ctx, *token = (T*)strtok_s(__tstrdup(src), seps, &ctx);
	while(token!=nullptr){ vs.push_back(atoi(token)); token=strtok_s(nullptr,seps,&ctx); }
	return vs;
}

template <class T>
inline std::vector<unsigned int> explodeu( const T* src, const T* seps=_strcvt<T>(" \t\n") )
{
	std::vector<unsigned int> vs;
	T *ctx, *token = (T*)strtok_s(__tstrdup(src), seps, &ctx);
	while(token!=nullptr){ vs.push_back(atou(token)); token=strtok_s(nullptr,seps,&ctx); }
	return vs;
}

template <class T>
inline std::vector<float> explodef( const T* src, const T* seps=_strcvt<T>(" \t\n") )
{
	std::vector<float>  vs;
	T *ctx, *token = (T*)strtok_s(__tstrdup(src), seps, &ctx);
	while(token!=nullptr){ vs.push_back(float(atof(token))); token=strtok_s(nullptr,seps,&ctx); }
	return vs;
}

template <class T>
inline std::vector<double> exploded( const T* src, const T* seps=_strcvt<T>(" \t\n") )
{
	std::vector<double>  vs;
	T *ctx, *token = (T*)strtok_s(__tstrdup(src), seps, &ctx);
	while(token!=nullptr){ vs.push_back(double(atof(token))); token=strtok_s(nullptr,seps,&ctx); }
	return vs;
}

template <class T>
inline std::vector<const T*> explode_conservative( const T* _Src, T _Delim )
{
	std::vector<const T*> vs; if(_Src==nullptr) return vs;
	for( T *s=__tstrdup(_Src), *e=s; *s&&*e; s=e+1){ for(e=s; *e!=_Delim && *e; e++ ); vs.push_back( __tstrdup(s,size_t(e-s)) ); }
	return vs;
}

//*******************************************************************
// 11. replace
template <class T>
inline const T* str_replace( const T* _Src, const T* _Find, const T* _Replace )
{
	if(_Find==nullptr||_Find[0]==0) return __tstrdup(_Src);	// no change
	int slen=int(strlen(_Src)), flen=int(strlen(_Find)), rlen=int(strlen(_Replace));
	if(slen<flen) return __tstrdup(_Src);	// no change
	
	T *s=(T*)_Src, *p=nullptr;
	std::vector<T> buff; while( (p=(T*)strstr(s,_Find)) ){ buff.insert(buff.end(),s,p); buff.insert(buff.end(),_Replace,_Replace+rlen); s=p+flen; }
	buff.insert(buff.end(),s,(T*)(_Src+slen));buff.push_back(0);
	return __tstrdup(&buff[0],buff.size());
}

template <class T>
inline const T* str_ireplace( const T* _Src, const T* _Find, const T* _Replace )
{
	if(_Find==nullptr||_Find[0]==0) return __tstrdup(_Src);	// no change
	int slen=int(strlen(_Src)), flen=int(strlen(_Find)), rlen=int(strlen(_Replace));
	if(slen<flen) return __tstrdup(_Src);	// no change
	
	T *s=(T*)_Src, *p=nullptr;
	std::vector<T> buff; while( p=(T*)_stristr(s,_Find) ){ buff.insert(buff.end(),s,p); buff.insert(buff.end(),_Replace,_Replace+rlen); s=p+flen; }
	buff.insert(buff.end(),s,(T*)(_Src+slen));buff.push_back(0);
	return __tstrdup(&buff[0],buff.size());
}

//*******************************************************************
#endif __GX_STRING_H__