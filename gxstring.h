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

#include "gxtype.h"

// size of shared circular buffer
#define SHARED_CIRCULAR_BUFFER_SIZE (1<<14)

// Byte order masks for UTF encoding
static const unsigned char BOM_UTF8[3]={0xEF,0xBB,0xBF};
static const unsigned char BOM_UTF16[2]={0xFF,0xFE};			// little endian
static const unsigned char BOM_UTF32[4]={0xFF,0xFE,0x00,0x00};	// little endian

//***********************************************
// 0. shared circular buffers
template <class T> __forceinline T* __tstrbuf( size_t len ){ static T* C[SHARED_CIRCULAR_BUFFER_SIZE]={}; static uint cid=0; cid=(++cid)%(sizeof(C)/sizeof(T*)); C[cid] = (T*)(C[cid]?realloc(C[cid],sizeof(T)*(len+1)):malloc(sizeof(T)*(len+1))); if(C[cid]) C[cid][len]=0; return C[cid]; }
template <class T> __forceinline T* __tstrdup( const T* s,size_t len=-1 ){ if(len==-1){const T* t=s; while(*t)t++;len=t-s;} T* d=__tstrbuf<T>(len); return len?(T*)memcpy(d,s,sizeof(T)*len):d; }
__forceinline char* _strbuf( size_t len ){ return __tstrbuf<char>(len); }
__forceinline wchar_t* _wcsbuf( size_t len ){ return __tstrbuf<wchar_t>(len); }

//***********************************************
// 1. overloaded string functions for wchar_t
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
inline wchar_t* strtok_s( wchar_t* _Str, const wchar_t* _Delim, wchar_t** context ){ return wcstok_s(_Str,_Delim,context); } // VS2015 fixes to ISO C Standard
inline size_t strspn( const wchar_t* _Str, const wchar_t* _Control ){ return wcsspn(_Str,_Control); }
inline size_t strcspn( const wchar_t* _Str, const wchar_t* _Control ){ return wcscspn(_Str,_Control); }
inline size_t strlen( const wchar_t* _Str ){ return wcslen(_Str); }
inline const wchar_t * strpbrk( const wchar_t* _Str, const wchar_t* _Control ){ return wcspbrk(_Str,_Control); }
inline wchar_t * strpbrk( wchar_t* _Str, const wchar_t* _Control ){ return wcspbrk(_Str,_Control); }
// 1.1 VC extensions
inline wchar_t* _strlwr( wchar_t* _Str ){ return _wcslwr(_Str); }
inline wchar_t* _strupr( wchar_t* _Str ){ return _wcsupr(_Str); }
inline errno_t _strlwr_s( wchar_t* _Str, size_t _Size ){ return _wcslwr_s(_Str,_Size); }
inline errno_t _strupr_s( wchar_t* _Str, size_t _Size ){ return _wcsupr_s(_Str,_Size); }
inline int _stricmp( const wchar_t* _Str1, const wchar_t* _Str2 ){ return _wcsicmp(_Str1,_Str2); }
// 1.2 slee extensions
template <class T> size_t _strrspn( const T* _Str, const T* _Control ){ size_t L=strlen(_Str),C=strlen(_Control),k=0,j=0;for(k=0;k<L;k++){for(j=0;j<C;j++)if(_Str[L-1-k]==_Control[j])break;if(j==C)break;}return k; }
inline const char*    _stristr( const char* _Str1, size_t l1, const char* _Str2, size_t l2 ){ char* s1=__tstrdup(_Str1,l1); _strlwr_s(s1,l1+1); char* s2=__tstrdup(_Str2,l2); _strlwr_s(s2,l2+1); const char* r=strstr(s1,s2); return r?(_Str1+(r-s1)):nullptr; }
inline const wchar_t* _wcsistr( const wchar_t* _Str1, size_t l1, const wchar_t* _Str2, size_t l2 ){ wchar_t* s1=__tstrdup(_Str1,l1); _wcslwr_s(s1,l1+1); wchar_t* s2=__tstrdup(_Str2,l2); _wcslwr_s(s2,l2+1); const wchar_t* r=wcsstr(s1,s2); return r?(_Str1+(r-s1)):nullptr; }
inline const char*    _stristr( const char* _Str1, const char* _Str2 ){ return _stristr(_Str1, strlen(_Str1), _Str2, strlen(_Str2)); }
inline const wchar_t* _wcsistr( const wchar_t* _Str1, const wchar_t* _Str2 ){ return _wcsistr(_Str1, wcslen(_Str1), _Str2, wcslen(_Str2)); }
inline const wchar_t* _stristr( const wchar_t* _Str1, const wchar_t* _Str2 ){ return _wcsistr(_Str1, wcslen(_Str1), _Str2, wcslen(_Str2)); }

//***********************************************
// 2. conversion between const wchar_t* and const char*
#ifndef CP_ACP
	#define CP_ACP		0           // default to ANSI code page
	#define CP_UTF8		65001       // UTF-8 translation
#endif
template <class T,class I> const T* _strcvt( const I* s ){size_t len=strlen(s);T* buff=__tstrbuf<T>(len);for(uint k=0;k<len;k++)buff[k]=T(s[k]);return buff;}
inline const wchar_t* atow( const char* a, int cp=0 /* code page: CP_ACP=0 */ ){int wlen=MultiByteToWideChar(cp,0,a,-1,0,0);wchar_t* wbuff=_wcsbuf(wlen);MultiByteToWideChar(cp,0,a,-1,wbuff,wlen);return wbuff;}
inline const char* wtoa( const wchar_t* w, int cp=0 /* code page: CP_ACP=0 */ ){int mblen=WideCharToMultiByte(cp,0,w,-1,0,0,0,0);char* buff=_strbuf(mblen);WideCharToMultiByte(cp,0,w,-1,buff,mblen,0,0);return buff;}
inline const char* atoa( const char* src, int src_cp, int dst_cp ){ return wtoa(atow(src,src_cp),dst_cp); }
inline bool ismbs( const char* s ){ if(!s||!*s)return false;for(int k=0,kn=int(strlen(s));k<kn;k++,s++)if(*s<0)return true;return false; }

//***********************************************
// 3. format and printf replacement
inline const char* vformat( const char* fmt, va_list a ){ size_t len=size_t(_vscprintf(fmt,a)); char* buffer=_strbuf(len); vsprintf_s(buffer,len+1,fmt,a); return buffer; }
inline const wchar_t* vformat( const wchar_t* fmt, va_list a ){ size_t len=size_t(_vscwprintf(fmt,a)); wchar_t* bufferW=_wcsbuf(len); vswprintf_s(bufferW,len+1,fmt,a); return bufferW; }
inline const char* format( const char* fmt,... ){ va_list a; va_start(a,fmt); size_t len=size_t(_vscprintf(fmt,a)); char* buffer=_strbuf(len); vsprintf_s(buffer,len+1,fmt,a); va_end(a); return buffer; }
inline const wchar_t* format( const wchar_t* fmt,... ){ va_list a; va_start(a,fmt); size_t len=size_t(_vscwprintf(fmt,a)); wchar_t* bufferW=_wcsbuf(len); vswprintf_s(bufferW,len+1,fmt,a); va_end(a); return bufferW; }

//***********************************************
// 4. natural-order and case-insensitive comparison for std::sort, std::map/set, std::unordered_map/set
#ifdef _INC_SHLWAPI
	inline int _strcmplogical( const wchar_t* _Str1, const wchar_t* _Str2 ){ return StrCmpLogicalW(_Str1,_Str2); }
#else
	inline int _strcmplogical( const wchar_t* _Str1, const wchar_t* _Str2 ){ static dll_function_t<int(*)(const wchar_t*,const wchar_t*)> f(L"shlwapi.dll","StrCmpLogicalW"); return f?f(_Str1,_Str2):_wcsicmp(_Str1,_Str2); } // load StrCmpLogicalW(): when unavailable, fallback to wcsicmp
#endif
inline int _strcmplogical( const char* _Str1, const char* _Str2, int cp=0 /* code page: CP_ACP=0 */){ return _strcmplogical(atow(_Str1,cp),atow(_Str2,cp)); }

namespace logical
{
	template <> struct less<std::wstring>{ bool operator()(const std::wstring& a,const std::wstring& b) const { return _strcmplogical(a.c_str(),b.c_str())<0;} };
	template <> struct less<std::string>{ bool operator()(const std::string& a,const std::string& b) const { return _strcmplogical(a.c_str(),b.c_str())<0;} };
	template <class T> using			set = std::set<T,less<T>>;
	template <class T, class V> using	map = std::map<T,V,less<T>>;
}

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
// 5. case conversion
template <class T> const T* tolower( const T* src ){ return _strlwr(__tstrdup(src)); }
template <class T> const T* toupper( const T* src ){ return _strupr(__tstrdup(src)); }
inline const char* tovarname( const char* src, bool to_upper=false ){ if(!src||!*src) return ""; char *s=(char*)src,*dst=_strbuf(strlen(src)+2),*d=dst; if(!isalpha(*s)&&(*s)!='_') *(d++)='_'; for(;*s;s++,d++) *d=isalnum(*s)?(to_upper?toupper(*s):(*s)):'_'; *d='\0'; return dst; }
inline const char* tovarname( const wchar_t* src, bool to_upper=false ){ return tovarname(wtoa(src),to_upper); }

//***********************************************
// 6. conversion to string types
inline const char* btoa( bool b ){ return b?"true":"false"; }
inline const char* itoa( int i ){static const char* fmt="%d";size_t size=size_t(_scprintf(fmt,i));char* buff=_strbuf(size); sprintf_s(buff,size+1,fmt,i);return buff;}
inline const char* utoa( uint u ){static const char* fmt="%u";size_t size=size_t(_scprintf(fmt,u));char* buff=_strbuf(size); sprintf_s(buff,size+1,fmt,u);return buff;}
inline const char* ftoa( float f ){if(fabs(f)<0.00000001f)return "0";static const char* fmt="%g";size_t size=size_t(_scprintf(fmt,f));char* buff=_strbuf(size); sprintf_s(buff,size+1,fmt,f); return buff; }
inline const char* dtoa( double d ){if(fabs(d)<0.00000001)return "0";static const char* fmt="%g";size_t size=size_t(_scprintf(fmt,d));char* buff=_strbuf(size); sprintf_s(buff,size+1,fmt,d); return buff; }
inline const char* illtoa( int64_t i ){ static const char* fmt="%lld";size_t size=size_t(_scprintf(fmt,i));char* buff=_strbuf(size); sprintf_s(buff,size+1,fmt,i);return buff; }
inline const char* ulltoa( uint64_t u ){ static const char* fmt="%llu";size_t size=size_t(_scprintf(fmt,u));char* buff=_strbuf(size); sprintf_s(buff,size+1,fmt,u);return buff; }
inline const char* itoa( const int2& v ){static const char* fmt="%d %d";size_t size=size_t(_scprintf(fmt,v.x,v.y));char* buff=_strbuf(size); sprintf_s(buff,size+1,fmt,v.x,v.y);return buff;}
inline const char* itoa( const int3& v ){static const char* fmt="%d %d %d";size_t size=size_t(_scprintf(fmt,v.x,v.y,v.z));char* buff=_strbuf(size); sprintf_s(buff,size+1,fmt,v.x,v.y,v.z);return buff;}
inline const char* itoa( const int4& v ){static const char* fmt="%d %d %d %d";size_t size=size_t(_scprintf(fmt,v.x,v.y,v.z,v.w));char* buff=_strbuf(size); sprintf_s(buff,size+1,fmt,v.x,v.y,v.z,v.w);return buff;}
inline const char* itoa( const short2& v ){ int2 i={v.x,v.y};return itoa(i); }
inline const char* itoa( const short3& v ){ int3 i={v.x,v.y,v.z};return itoa(i); }
inline const char* itoa( const short4& v ){ int4 i={v.x,v.y,v.z,v.w};return itoa(i); }
inline const char* itoa( const char2& v ){ int2 i={v.x,v.y};return itoa(i); }
inline const char* itoa( const char3& v ){ int3 i={v.x,v.y,v.z};return itoa(i); }
inline const char* itoa( const char4& v ){ int4 i={v.x,v.y,v.z,v.w};return itoa(i); }
inline const char* utoa( const uint2& v ){static const char* fmt="%u %u";size_t size=size_t(_scprintf(fmt,v.x,v.y));char* buff=_strbuf(size); sprintf_s(buff,size+1,fmt,v.x,v.y);return buff;}
inline const char* utoa( const uint3& v ){static const char* fmt="%u %u %u";size_t size=size_t(_scprintf(fmt,v.x,v.y,v.z));char* buff=_strbuf(size); sprintf_s(buff,size+1,fmt,v.x,v.y,v.z);return buff;}
inline const char* utoa( const uint4& v ){static const char* fmt="%u %u %u %u";size_t size=size_t(_scprintf(fmt,v.x,v.y,v.z,v.w));char* buff=_strbuf(size); sprintf_s(buff,size+1,fmt,v.x,v.y,v.z,v.w);return buff;}
inline const char* utoa( const ushort2& v ){ uint2 u={v.x,v.y};return utoa(u); }
inline const char* utoa( const ushort3& v ){ uint3 u={v.x,v.y,v.z};return utoa(u); }
inline const char* utoa( const ushort4& v ){ uint4 u={v.x,v.y,v.z,v.w};return utoa(u); }
inline const char* utoa( const uchar2& v ){ uint2 u={v.x,v.y};return utoa(u); }
inline const char* utoa( const uchar3& v ){ uint3 u={v.x,v.y,v.z};return utoa(u); }
inline const char* utoa( const uchar4& v ){ uint4 u={v.x,v.y,v.z,v.w};return utoa(u); }
inline const char* ftoa( const float2& v ){static const char* fmt="%g %g";size_t size=size_t(_scprintf(fmt,v.x,v.y));char* buff=_strbuf(size); sprintf_s(buff,size+1,fmt,v.x,v.y);return buff;}
inline const char* ftoa( const float3& v ){static const char* fmt="%g %g %g";size_t size=size_t(_scprintf(fmt,v.x,v.y,v.z));char* buff=_strbuf(size); sprintf_s(buff,size+1,fmt,v.x,v.y,v.z);return buff;}
inline const char* ftoa( const float4& v ){static const char* fmt="%g %g %g %g";size_t size=size_t(_scprintf(fmt,v.x,v.y,v.z,v.w));char* buff=_strbuf(size); sprintf_s(buff,size+1,fmt,v.x,v.y,v.z,v.w);return buff;}
inline const char* dtoa( const double2& v ){static const char* fmt="%g %g";size_t size=size_t(_scprintf(fmt,v.x,v.y));char* buff=_strbuf(size); sprintf_s(buff,size+1,fmt,v.x,v.y);return buff;}
inline const char* dtoa( const double3& v ){static const char* fmt="%g %g %g";size_t size=size_t(_scprintf(fmt,v.x,v.y,v.z));char* buff=_strbuf(size); sprintf_s(buff,size+1,fmt,v.x,v.y,v.z);return buff;}
inline const char* dtoa( const double4& v ){static const char* fmt="%g %g %g %g";size_t size=size_t(_scprintf(fmt,v.x,v.y,v.z,v.w));char* buff=_strbuf(size); sprintf_s(buff,size+1,fmt,v.x,v.y,v.z,v.w);return buff;}

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
#if defined(__GX_MATH__)&&!defined(__VECTOR_TYPES_H__) // type definitions in CUDA/vector_types.h
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
//***********************************************
// 7. conversion to wstring types
inline const wchar_t* btow( bool b ){			return b?L"true":L"false"; }
inline const wchar_t* itow( int i ){			return atow(itoa(i)); }
inline const wchar_t* utow( uint u ){			return atow(utoa(u)); }
inline const wchar_t* ftow( float f ){			return atow(ftoa(f)); }
inline const wchar_t* dtow( double d ){			return atow(dtoa(d)); }
inline const wchar_t* illtow( int64_t i ){		return atow(illtoa(i)); }
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
inline const wchar_t* dtow( const double2& v ){	return atow(dtoa(v)); }
inline const wchar_t* dtow( const double3& v ){	return atow(dtoa(v)); }
inline const wchar_t* dtow( const double4& v ){	return atow(dtoa(v)); }

// templated generic conversion
template <class T> const wchar_t* ntow( T v );
template<> inline const wchar_t* ntow<bool>( bool v ){ return btow(v); }
template<> inline const wchar_t* ntow<int>( int v ){ return itow(v); }
template<> inline const wchar_t* ntow<uint>( uint v ){ return utow(v); }
template<> inline const wchar_t* ntow<float>( float v ){ return ftow(v); }
template<> inline const wchar_t* ntow<double>( double v ){ return dtow(v); }
template<> inline const wchar_t* ntow<int64_t>( int64_t v ){ return illtow(v); }
template<> inline const wchar_t* ntow<uint64_t>( uint64_t v ){ return ulltow(v); }
template<> inline const wchar_t* ntow<int2>( int2 v ){ return itow(v); }
template<> inline const wchar_t* ntow<int3>( int3 v ){ return itow(v); }
template<> inline const wchar_t* ntow<int4>( int4 v ){ return itow(v); }
template<> inline const wchar_t* ntow<short2>( short2 v ){ return itow(v); }
template<> inline const wchar_t* ntow<short3>( short3 v ){ return itow(v); }
template<> inline const wchar_t* ntow<short4>( short4 v ){ return itow(v); }
template<> inline const wchar_t* ntow<char2>( char2 v ){ return itow(v); }
template<> inline const wchar_t* ntow<char3>( char3 v ){ return itow(v); }
template<> inline const wchar_t* ntow<char4>( char4 v ){ return itow(v); }
template<> inline const wchar_t* ntow<uint2>( uint2 v ){ return utow(v); }
template<> inline const wchar_t* ntow<uint3>( uint3 v ){ return utow(v); }
template<> inline const wchar_t* ntow<uint4>( uint4 v ){ return utow(v); }
template<> inline const wchar_t* ntow<ushort2>( ushort2 v ){ return utow(v); }
template<> inline const wchar_t* ntow<ushort3>( ushort3 v ){ return utow(v); }
template<> inline const wchar_t* ntow<ushort4>( ushort4 v ){ return utow(v); }
template<> inline const wchar_t* ntow<uchar2>( uchar2 v ){ return utow(v); }
template<> inline const wchar_t* ntow<uchar3>( uchar3 v ){ return utow(v); }
template<> inline const wchar_t* ntow<uchar4>( uchar4 v ){ return utow(v); }
template<> inline const wchar_t* ntow<float2>( float2 v ){ return ftow(v); }
template<> inline const wchar_t* ntow<float3>( float3 v ){ return ftow(v); }
template<> inline const wchar_t* ntow<float4>( float4 v ){ return ftow(v); }
template<> inline const wchar_t* ntow<double2>( double2 v ){ return dtow(v); }
template<> inline const wchar_t* ntow<double3>( double3 v ){ return dtow(v); }
template<> inline const wchar_t* ntow<double4>( double4 v ){ return dtow(v); }
#if defined(__GX_MATH__)&&!defined(__VECTOR_TYPES_H__) // type definitions in CUDA/vector_types.h
template<> inline const wchar_t* ntow<ivec2>( ivec2 v ){ return itow(v); }
template<> inline const wchar_t* ntow<ivec3>( ivec3 v ){ return itow(v); }
template<> inline const wchar_t* ntow<ivec4>( ivec4 v ){ return itow(v); }
template<> inline const wchar_t* ntow<uvec2>( uvec2 v ){ return utow(v); }
template<> inline const wchar_t* ntow<uvec3>( uvec3 v ){ return utow(v); }
template<> inline const wchar_t* ntow<uvec4>( uvec4 v ){ return utow(v); }
template<> inline const wchar_t* ntow<vec2>( vec2 v ){ return ftow(v); }
template<> inline const wchar_t* ntow<vec3>( vec3 v ){ return ftow(v); }
template<> inline const wchar_t* ntow<vec4>( vec4 v ){ return ftow(v); }
template<> inline const wchar_t* ntow<dvec2>( dvec2 v ){ return dtow(v); }
template<> inline const wchar_t* ntow<dvec3>( dvec3 v ){ return dtow(v); }
template<> inline const wchar_t* ntow<dvec4>( dvec4 v ){ return dtow(v); }
#endif

//***********************************************
// 7.1 bitwise conversion
template <class T> const char* unpack_bits( const T& v ){ size_t n=sizeof(T)*8; char* buff=_strbuf(n); buff[n]=0; for(size_t k=0,s=0;k<n;k++,s=k%8) buff[k]=(((const char*)&v)[k>>3]&(1<<s))?'1':'0'; return buff; }

//***********************************************
// 7.2 Special-purpose functions

// conversion int to string with commas
__noinline const char* itoasep( int n )
{
	if(n<1000&&n>-1000) return itoa(n);
	const char* s=itoa(n>0?n:-n); size_t len=strlen(s);
	std::vector<char> v; v.resize(len+1); memcpy(&v[0],s,len+1);
	for( uint idx=(len%3?len%3:3); idx<len; idx+=4,len++ ) v.emplace(v.begin()+idx,',');
	return format("%s%s",n>0?"":"-",&v[0]);
}

__noinline const char* illtoasep( int64_t n )
{
	if(n<1000&&n>-1000) return illtoa(n);
	const char* s=illtoa(n>0?n:-n); size_t len=strlen(s);
	std::vector<char> v; v.resize(len+1); memcpy(&v[0],s,len+1);
	for( uint idx=(len%3?len%3:3); idx<len; idx+=4,len++ ) v.emplace(v.begin()+idx,',');
	return format("%s%s",n>0?"":"-",&v[0]);
}

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

inline bool atob( const char* a ){		return a&&*a&&(_stricmp(a,"true")==0||fast::atoi(a)!=0); }
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

//***********************************************
// 10. conversion from wstring to user types
inline bool wtob( const wchar_t* w ){		return w&&*w&&(_wcsicmp(w,L"true")==0||_wtoi(w)!=0); }
inline uint wtou( const wchar_t* w ){		return atou(wtoa(w)); }
inline int64_t wtoill( const wchar_t* w ){	return atoill(wtoa(w)); }
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
inline double2 wtod2( const wchar_t* w ){	return atod2(wtoa(w)); }
inline double3 wtod3( const wchar_t* w ){	return atod3(wtoa(w)); }
inline double4 wtod4( const wchar_t* w ){	return atod4(wtoa(w)); }

inline int atoi( const wchar_t* w ){		return fast::atoi(wtoa(w)); }
inline double atof( const wchar_t* w ){		return fast::atof(wtoa(w)); }

// hexadecimanal conversion
inline const char* tohex( void* ptr, size_t size ){ unsigned char* u=(unsigned char*)ptr; char *buff=_strbuf(size*2), *b=buff; for(size_t k=0;k<size;k++,u++,b+=2) sprintf(b,"%02x",*u); buff[size*2]=0; return buff; }
inline const char* tohex( int i ){ return tohex(&i,sizeof(i)); }
inline const char* tohex( int2 i ){ return tohex(&i,sizeof(i)); }
inline const char* tohex( int3 i ){ return tohex(&i,sizeof(i)); }
inline const char* tohex( int4 i ){ return tohex(&i,sizeof(i)); }
inline const char* tohex( uint u ){ return tohex(&u,sizeof(u)); }
inline const char* tohex( uint2 u ){ return tohex(&u,sizeof(u)); }
inline const char* tohex( uint3 u ){ return tohex(&u,sizeof(u)); }
inline const char* tohex( uint4 u ){ return tohex(&u,sizeof(u)); }

//***********************************************
// 11. trim: delimiters of strings or characters

template <class T>
const T* ltrim( const T* src, const T* delims=_strcvt<T>(" \t\r\n") )
{
	if(!src||!src[0]) return reinterpret_cast<const T*>(L"");
	return __tstrdup(src,strlen(src))+strspn(src,delims);
}

template <class T>
const T* ltrim( const T* src, T delim )
{
	if(!src||!src[0]) return reinterpret_cast<const T*>(L"");
	while(*src&&*src==delim) src++; return __tstrdup(src);
}

template <class T>
const T* rtrim( const T* src, const T* delims=_strcvt<T>(" \t\r\n") )
{
	if(!src||!src[0]) return reinterpret_cast<const T*>(L"");
	return __tstrdup(src,strlen(src)-_strrspn(src,delims));
}

template <class T>
const T* rtrim( const T* src, T delim )
{
	if(!src||!src[0]) return reinterpret_cast<const T*>(L"");
	int l=int(strlen(src)); const T* d=src+l-1; for(int k=l-1;*d==delim&&k>=0;k--,d--);
	return __tstrdup(src,d-src+1);
}

template <class T>
const T* trim( const T* src, const T* delims=_strcvt<T>(" \t\r\n") )
{
	if(!src||!src[0]) return reinterpret_cast<const T*>(L"");
	const T* r=rtrim(src,delims); return r+(*r?strspn(src,delims):0);
}

template <class T>
const T* trim( const T* src, T delim )
{
	if(!src||!src[0]) return reinterpret_cast<const T*>(L"");
	while(*src&&*src==delim) src++; if(!src[0]) return reinterpret_cast<const T*>(__tstrdup(L""));
	int l=int(strlen(src)); const T* d=src+l-1; for(int k=l-1;*d==delim&&k>=0;k--,d--);
	return __tstrdup(src,d-src+1);
}

// in-place trim
template <class T>
T* itrim( T* src, const T* delims=_strcvt<T>(" \t\r\n") )
{
	if(!src||!src[0]) return src; // return as-is
	src += strspn(src,delims);
	src[strlen(src)-_strrspn(src,delims)]=0;
	return src;
}

template <class T>
const T* trim_comment( const T* src, const char* marker="#" )
{
	if(!src||!src[0]) return reinterpret_cast<const T*>(L"");
	size_t slen=strlen(src),clen=strlen(marker),sc=slen-clen+1;
	T* buff=__tstrdup(src,slen); const char* cs=marker;
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
	buff[slen-_strrspn(buff,_strcvt<T>(" \t\r\n"))]=0; // rtrim
	return buff;
}

//***********************************************
// 12. explode/join

template <class T>
__noinline const T* join( std::vector<std::basic_string<T,std::char_traits<T>,std::allocator<T>>> v, const T* delim=_strcvt<T>(" ") )
{
	std::basic_string<T,std::char_traits<T>,std::allocator<T>> s;
	for( const auto& k : v ){ if(k.empty()) continue; if(!s.empty()) s+=decltype(s)(delim); s+=k; }
	return __tstrdup(s.c_str());
}

template <class T>
__noinline std::vector<std::basic_string<T,std::char_traits<T>,std::allocator<T> > >
explode( const T* src, const T* seps=_strcvt<T>(" \t\r\n") )
{
	std::vector<std::basic_string<T,std::char_traits<T>,std::allocator<T> > > vs; vs.reserve(32);
	T *ctx=nullptr, *token = strtok_s(__tstrdup(src),seps,&ctx);
	while(token!=nullptr){ vs.emplace_back(token); token=strtok_s(nullptr,seps,&ctx); }
	return vs;
}

template <class T>
__noinline std::set<std::basic_string<T,std::char_traits<T>,std::allocator<T> > >
explode_set( const T* src, const T* seps=_strcvt<T>(" \t\r\n") )
{
	std::set<std::basic_string<T,std::char_traits<T>,std::allocator<T> > > vs;
	T *ctx=nullptr, *token = strtok_s(__tstrdup(src), seps, &ctx);
	while(token!=nullptr){ vs.emplace(token); token=strtok_s(nullptr,seps,&ctx); }
	return vs;
}

template <class T>
__noinline std::vector<int> explodei( const T* src, const T* seps=_strcvt<T>(" \t\r\n") )
{
	std::vector<int> vs; vs.reserve(32);
	T *ctx=nullptr, *token = strtok_s(__tstrdup(src), seps, &ctx);
	while(token!=nullptr){ vs.emplace_back(atoi(token)); token=strtok_s(nullptr,seps,&ctx); }
	return vs;
}

template <class T>
__noinline std::vector<unsigned int> explodeu( const T* src, const T* seps=_strcvt<T>(" \t\r\n") )
{
	std::vector<unsigned int> vs; vs.reserve(32);
	T *ctx=nullptr, *token = strtok_s(__tstrdup(src), seps, &ctx);
	while(token!=nullptr){ vs.emplace_back(atou(token)); token=strtok_s(nullptr,seps,&ctx); }
	return vs;
}

template <class T>
__noinline std::vector<float> explodef( const T* src, const T* seps=_strcvt<T>(" \t\r\n") )
{
	std::vector<float>  vs; vs.reserve(32);
	T *ctx=nullptr, *token = strtok_s(__tstrdup(src), seps, &ctx);
	while(token!=nullptr){ vs.emplace_back(float(fast::atof(token))); token=strtok_s(nullptr,seps,&ctx); }
	return vs;
}

template <class T>
__noinline std::vector<double> exploded( const T* src, const T* seps=_strcvt<T>(" \t\r\n") )
{
	std::vector<double>  vs; vs.reserve(32);
	T *ctx=nullptr, *token = strtok_s(__tstrdup(src), seps, &ctx);
	while(token!=nullptr){ vs.emplace_back(double(fast::atof(token))); token=strtok_s(nullptr,seps,&ctx); }
	return vs;
}

template <class T>
__noinline std::vector<const T*> explode_conservative( const T* _Src, T _Delim )
{
	std::vector<const T*> vs; vs.reserve(16); if(_Src==nullptr) return vs;
	for(T *s=__tstrdup(_Src),*e=s;*s&&*e;s=e+1){for(e=s;*e!=_Delim&&*e;e++){}; vs.emplace_back(__tstrdup(s,size_t(e-s))); }
	return vs;
}

//***********************************************
// 13. substring, replace, escape

template <class T>
__noinline const T* substr( const T* _Src, int _Pos, int _Count=0 )
{
	static T *nullstr=(T*)L""; if(!_Src) return nullstr;
	size_t l = strlen(_Src); if(_Pos<0) _Pos+=int(l); if(_Pos>=int(l)) return nullstr; if(_Count<=0) _Count+=int(l);
	return __tstrdup(_Src+_Pos,size_t(_Pos)+_Count<=l?size_t(_Count):size_t(l-_Pos));
}

template <class T>
__noinline const T* str_replace( const T* _Src, const T* _Find, const T* _Replace )
{
	if(_Find==nullptr||_Find[0]==0) return __tstrdup(_Src);	// no change
	int sl=int(strlen(_Src)), fl=int(strlen(_Find)); if(sl<fl) return __tstrdup(_Src);
	int rl=int(strlen(_Replace));
	T *s=(T*)_Src, *p=nullptr;
	std::vector<T> buff; buff.reserve(2llu*sl); while( nullptr!=(p=(T*)strstr(s,_Find)) ){ buff.insert(buff.end(),s,p); if(rl>0) buff.insert(buff.end(),_Replace,_Replace+rl); s=p+fl; }
	buff.insert(buff.end(),s,(T*)(_Src+sl));buff.emplace_back(0);
	return __tstrdup(&buff[0],buff.size());
}

template <class T>
__noinline const T* str_ireplace( const T* _Src, const T* _Find, const T* _Replace )
{
	if(_Find==nullptr||_Find[0]==0) return __tstrdup(_Src);	// no change
	int sl=int(strlen(_Src)), fl=int(strlen(_Find)); if(sl<fl) return __tstrdup(_Src); int rl=int(strlen(_Replace));
	T *s=(T*)_Src, *p=nullptr;
	std::vector<T> buff; buff.reserve(sl*2); while( nullptr!=(p=(T*)_stristr(s,_Find)) ){ buff.insert(buff.end(),s,p); buff.insert(buff.end(),_Replace,_Replace+rl); s=p+fl; }
	buff.insert(buff.end(),s,(T*)(_Src+sl));buff.emplace_back(0);
	return __tstrdup(&buff[0],buff.size());
}

template <class T>
__noinline const T* str_replace( const T* _Src, T _Find, T _Replace )
{
	T *s=__tstrdup(_Src), *p=s;
	for(int k=0,l=int(strlen(s));k<l;k++,p++) if(*p==_Find) *p=_Replace;
	return s;
}

template <class T>
__noinline const T* str_escape( const T* _Src )
{
	const T* t = str_replace( _Src, _strcvt<T>("\\"), _strcvt<T>("\\\\") );
	t = str_replace( t, _strcvt<T>("\""), _strcvt<T>("\\\"") );
	t = str_replace( t, _strcvt<T>("\'"), _strcvt<T>("\\\'") );
	t = str_replace( t, _strcvt<T>("\t"), _strcvt<T>("\\\t") );
	return t;
}

//***********************************************
// 14. pattern matching
// - simple ?/* is supported
// - posix-style **/* (subdirectory matching) is not implemented yet

template <class T>
__noinline bool glob( const T* str, size_t slen, const T* pattern, size_t plen )
{
	static const T q=T('?'), a=T('*');
	int n=int(slen?slen:strlen(str)), m=int(plen?plen:strlen(pattern)); if(m==0) return n==0;
	int i,j,t,p; for(i=0,j=0,t=-1,p=-1;i<n;)
	{
		if(str[i]==pattern[j]||(j<m&&pattern[j]==q)){i++;j++;}
		else if(j<m&&pattern[j]==a){t=i;p=j;j++;}
		else if(p!=-1){j=p+1;i=t+1;t++;}
		else return false;
	}
	while(j<m&&pattern[j]==a)j++;
	return j==m;
}

template <class T>
__noinline bool iglob( const T* str, size_t slen, const T* pattern, size_t plen ) // case-insensitive
{
	static const T q=T('?'), a=T('*');
	int n=int(slen?slen:strlen(str)), m=int(plen?plen:strlen(pattern)); if(m==0) return n==0;
	int i,j,t,p; for(i=0,j=0,t=-1,p=-1;i<n;)
	{
		if(::tolower(str[i])==::tolower(pattern[j])||(j<m&&pattern[j]==q)){i++;j++;}
		else if(j<m&&pattern[j]==a){t=i;p=j;j++;}
		else if(p!=-1){j=p+1;i=t+1;t++;}
		else return false;
	}
	while(j<m&&pattern[j]==a)j++;
	return j==m;
}

template <class T>
__noinline bool glob( const T* str, const T* pattern )
{
	return glob<T>(str,str?strlen(str):0,pattern,pattern?strlen(pattern):0);
}

template <class T>
__noinline bool iglob( const T* str, const T* pattern )
{
	return iglob<T>(str,str?strlen(str):0,pattern,pattern?strlen(pattern):0);
}

//***********************************************
// 15. common unnecesary unicode symbols to common ansi

__noinline const wchar_t* unicode_symbols_to_ansi( const wchar_t* str )
{
	wchar_t* b=__tstrdup(str);
	static std::map<wchar_t,wchar_t> lut =
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

//***********************************************
// 16. autoquote

template <class T>
__noinline const T* auto_quote( const T* _Src )
{
	size_t l=strlen(_Src); static const T q=T('\"'); if(!*_Src||(_Src[0]==q&&_Src[l-1]==q)) return __tstrdup(_Src,l);
	const T* s=trim(_Src); if(*s!=T('-')&&!strchr(s,T(' '))&&!strchr(s,T('\t'))&&!strchr(s,T('|'))&&!strchr(s,T('&'))&&!strchr(s,T('<'))&&!strchr(s,T('>'))) return __tstrdup(_Src,l);
	T* b=__tstrbuf<T>(l+2); b[0]=q; memcpy(b+1,_Src,l*sizeof(T)); b[l+1]=q; b[l+2]=0; return b;
}

//***********************************************
// 17. Longest Common SubString (LCSS)

template <class T>
__noinline T strlcss( const std::vector<T>& v ) // longest common substring
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
// 18. GUID conversion

#if defined(GUID_DEFINED)

struct guid_t : public GUID
{
	guid_t() noexcept { memset(this,0,sizeof(GUID)); }
	guid_t( const GUID& other ) noexcept { operator=(other); }
	guid_t( GUID&& other ) noexcept { operator=(other); }
	guid_t( const wchar_t* other ) noexcept { operator=(other); }
	guid_t( const char* other ) noexcept { operator=(other); }
	explicit guid_t( const std::wstring& other ) noexcept { operator=(other.c_str()); }
	explicit guid_t( const std::string& other ) noexcept { operator=(other.c_str()); }

	guid_t& operator=( const GUID& other ) noexcept { memcpy(this,&other,sizeof(GUID)); return *this; }
	guid_t& operator=( GUID&& other ) noexcept { auto t=*this; memcpy(this,&other,sizeof(GUID)); memcpy(&other,&t,sizeof(GUID)); return *this; }
	guid_t& operator=( const wchar_t* other ) noexcept { auto* d=Data4; swscanf_s( other, L"{%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX}", &Data1,&Data2,&Data3,d,d+1,d+2,d+3,d+4,d+5,d+6,d+7 ); return *this; }
	guid_t& operator=( const char* other ) noexcept { auto* d=Data4; sscanf_s( other, "{%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX}", &Data1,&Data2,&Data3,d,d+1,d+2,d+3,d+4,d+5,d+6,d+7 ); return *this; }
	guid_t& operator=( const std::wstring& other ) noexcept { return operator=(other.c_str()); }
	guid_t& operator=( const std::string& other ) noexcept { return operator=(other.c_str()); }
	bool operator==( const GUID& other ) const noexcept { return memcmp(this,&other,sizeof(GUID))==0; }
	bool operator!=( const GUID& other ) const noexcept { return memcmp(this,&other,sizeof(GUID))!=0; }

	operator GUID (){ return *this; }
	operator const GUID () const { return *this; }
	operator const std::string () const { return wtoa(); }
	operator const std::wstring () const { return c_str(); }
	operator bool() const { static decltype(Data4) z={}; return Data1||Data2||Data3||memcmp(Data4,&z,sizeof(z))!=0; }
	const wchar_t* c_str() const { wchar_t* b=_wcsbuf(64); auto* d=Data4; swprintf_s( b, 64, L"{%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX}", Data1,Data2,Data3,d[0],d[1],d[2],d[3],d[4],d[5],d[6],d[7] ); return b; }
	const char* wtoa() const { char* b=_strbuf(64); auto* d=Data4; sprintf_s( b, 64, "{%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX}", Data1,Data2,Data3,d[0],d[1],d[2],d[3],d[4],d[5],d[6],d[7] ); return b; }
	void clear(){ memset(this,0,sizeof(GUID)); }
};

#endif

//***********************************************
#endif // __GX_STRING_H__
