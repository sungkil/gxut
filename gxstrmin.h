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
#ifndef __GX_STRMIN_H__
#define __GX_STRMIN_H__

#include "gxtype.h"

// shared circular buffers
template <class T=char> __forceinline T* __strbuf( size_t len ){ static T* C[1<<14]={}; static uint cid=0; cid=cid%(sizeof(C)/sizeof(C[0])); C[cid]=(T*)(C[cid]?realloc(C[cid],sizeof(T)*(len+2)):malloc(sizeof(T)*(len+2))); if(C[cid]){ C[cid][len]=0; C[cid][len+1]=0; } return C[cid++]; }
template <class T=char> __forceinline T* __strdup( const T* s, size_t len ){ T* d=__strbuf<T>(size_t(len)); if(!len){ d[0]=d[1]=0; return d; } memcpy(d, s, sizeof(T)*len); d[len]=d[len+1]=0; return d; }
template <class T=char> __forceinline T* __strdup( const T* s ){ size_t l=0; const T* t=s; while(*t){t++;l++;} return __strdup<T>(s,l); }

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
#ifdef __GNUC__
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

// VC extensions
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

// conversion between const wchar_t* and const char*
template <class T,class I> const T* _strcvt( const I* s ){size_t len=strlen(s);T* buff=__strbuf<T>(len);for(uint k=0;k<len;k++)buff[k]=T(s[k]);return buff;}

#ifdef _MSC_VER
// https://stackoverflow.com/questions/28270310/how-to-easily-detect-utf8-encoding-in-the-string
inline bool is_utf8( const char * s )
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

inline bool is_valid_utf8( const char* s )
{
	if(!s||!*s) return true; const unsigned char* b=(const unsigned char*)s;
	int n; unsigned int c; while(*b!=0x00)
	{
		if((*b&0x80)==0x00){ n=1; c=(*b&0x7F); }		// U+0000 to U+007F
		else if((*b&0xE0)==0xC0){ n=2; c=(*b&0x1F); }	// U+0080 to U+07FF
		else if((*b&0xF0)==0xE0){ n=3; c=(*b&0x0F); }	// U+0800 to U+FFFF
		else if((*b&0xF8)==0xF0){ n=4; c=(*b&0x07); }	// U+10000 to U+10FFFF
		else return false;
		b++; for(int k=1;k<n;k++){ if((*b&0xC0)!=0x80) return false; c=(c<<6)|(*b&0x3F); b++; }
		if((c>0x10FFFF)||((c>=0xD800)&&(c<=0xDFFF))||((c<=0x007F)&&(n!=1))||((c>=0x0080)&&(c<=0x07FF)&&(n!=2))||((c>=0x0800)&&(c<=0xFFFF)&&(n!=3))||((c>=0x10000)&&(c<=0x1FFFFF)&&(n!=4))) return false;
	}
	return true;
}

inline const wchar_t* atow( const char* a ){ if(!a) return nullptr; if(!*a) return L""; uint cp=is_utf8(a)?CP_UTF8:0; int l=MultiByteToWideChar(cp, 0, a, -1, 0, 0); wchar_t* w=__strbuf<wchar_t>(l); MultiByteToWideChar(cp, 0, a, -1, w, l); return w; }
inline const char* wtoa( const wchar_t* w ){ if(!w) return nullptr; if(!*w) return ""; int l=WideCharToMultiByte(CP_GXUT, 0, w, -1, 0, 0, 0, 0); char* a=__strbuf(l); WideCharToMultiByte(CP_GXUT, 0, w, -1, a, l, 0, 0); return a; }
inline const char* atoa( const char* src, uint src_cp, uint dst_cp=CP_GXUT ){ if(!src) return nullptr; if(!*src) return ""; if(src_cp==dst_cp) return __strdup(src); int l=MultiByteToWideChar(src_cp, 0, src, -1, 0, 0); wchar_t* w=__strbuf<wchar_t>(l); MultiByteToWideChar(src_cp, 0, src, -1, w, l); l=WideCharToMultiByte(dst_cp, 0, w, -1, 0, 0, 0, 0); char* a=__strbuf(l); WideCharToMultiByte(dst_cp, 0, w, -1, a, l, 0, 0); return a; }
#else
inline const wchar_t* atow( const char* a ){ if(!a) return nullptr; if(!*a) return L""; const char* p=a; size_t l=mbsrtowcs(0,&p,0,0); wchar_t* b=__strbuf<wchar_t>(l); mbstate_t s={}; mbsrtowcs(b,&p,l+1,&s); return b; }
inline const char* wtoa( const wchar_t* w ){ if(!w) return nullptr; if(!*w) return ""; const wchar_t* p=w; size_t l=wcsrtombs(0,&p,0,0); char* b=__strbuf(l); mbstate_t s={}; wcsrtombs(b,&p,l+1,&s); return b; }
#endif
inline bool ismbs( const char* s ){ if(!s||!*s) return false; for(int k=0,kn=int(strlen(s));k<kn;k++,s++) if(*s<0)return true; return false; }

// format and printf replacement
inline const char*		vformat( __printf_format_string__ const char* fmt, va_list a ){ size_t len=size_t(vsnprintf(0,0,fmt,a)); char* buffer=__strbuf(len); vsnprintf(buffer,len+1,fmt,a); return buffer; }
inline const wchar_t*	vformat( __printf_format_string__ const wchar_t* const fmt, va_list a ){ size_t len=size_t(vswprintf(0,0,fmt,a)); wchar_t* bufferW=__strbuf<wchar_t>(len); vswprintf(bufferW,len+1,fmt,a); return bufferW; }
inline const char*		__attribute__((format(printf,1,2))) format( __printf_format_string__ const char* fmt, ... ){ va_list a; va_start(a,fmt); size_t len=size_t(vsnprintf(0,0,fmt,a)); char* buffer=__strbuf(len); vsnprintf(buffer,len+1,fmt,a); va_end(a); return buffer; }
inline const wchar_t*	format( __printf_format_string__ const wchar_t* fmt, ... ){ va_list a; va_start(a,fmt); size_t len=size_t(vswprintf(0,0,fmt,a)); wchar_t* bufferW=__strbuf<wchar_t>(len); vswprintf(bufferW,len+1,fmt,a); va_end(a); return bufferW; }

// natural-order and case-insensitive comparison for std::sort, std::map/set, std::unordered_map/set
#ifdef _INC_SHLWAPI
	inline int _strcmplogical( const wchar_t* _Str1, const wchar_t* _Str2 ){ return (!_Str1||!_Str2)?0:StrCmpLogicalW(_Str1,_Str2); }
#elif defined(_MSC_VER)
	inline int _strcmplogical( const wchar_t* _Str1, const wchar_t* _Str2 ){ static dll_function_t<int(*)(const wchar_t*,const wchar_t*)> f("shlwapi.dll","StrCmpLogicalW"); return !f?_wcsicmp(_Str1,_Str2):(!_Str1||!_Str2)?0:f(_Str1,_Str2); } // load StrCmpLogicalW(): when unavailable, fallback to wcsicmp
#else
	inline int _strcmplogical( const wchar_t* s1, const wchar_t* s2 )
	{
		if(!s1||!s2) return 0;
		while(*s1)
		{
			if(!*s2) return 1;
			else if(iswdigit(*s1))
			{
				if(!iswdigit(*s2)) return -1;
				int i1=_wtoi(s1), i2=_wtoi(s2); if(i1<i2) return -1; else if(i1>i2)	return 1;
				while(iswdigit(*s1)){ s1++; } while(iswdigit(*s2)){ s2++; }
			}
			else if(!iswdigit(*s2))
			{
				int d=wcsncasecmp(s1,s2,1); if(d>0) return 1; else if(d<0) return -1; /*int d=CompareStringW(GetThreadLocale(),NORM_IGNORECASE,s1,1,s2,1)-CSTR_EQUAL*/
				s1++; s2++;
			}
			else return 1;
		}
		if(*s2) return -1;
		return 0;
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

//***********************************************
#endif // __GX_STRMIN_H__
