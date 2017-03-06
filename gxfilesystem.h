//*******************************************************************
// Copyright 2017 Sungkil Lee
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
#ifndef __GX_FILESYSTEM_H__
#define __GX_FILESYSTEM_H__
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

#include <deque>		// for canonicalization
#include <direct.h>		// directory control
#include <io.h>			// low-level io functions
#include "gxstring.h"	// make sure to include gxstring for nocase extension

//*******************************************************************
// Utility for filetime comparison
inline FILETIME DiscardFileTimeMilliseconds( FILETIME f ){ SYSTEMTIME s; FileTimeToSystemTime(&f,&s); s.wMilliseconds=0; SystemTimeToFileTime(&s,&f); return f; }
inline uint64_t FileTimeOffset( uint days, uint hours=0, uint mins=0, uint secs=0, uint mss=0 ){ return 10000ull*(mss+1000ull*secs+60*1000ull*mins+60*60*1000ull*hours+24*60*60*1000ull*days); } // FILETIME in 100 ns scale
inline uint64_t FileTimeToUINT64( const FILETIME& f, uint offset_days=0 ){ return (uint64_t(f.dwHighDateTime)<<32|uint64_t(f.dwLowDateTime))-FileTimeOffset(offset_days); }
inline FILETIME	UINT64ToFileTime( uint64_t u ){ FILETIME f; f.dwHighDateTime=DWORD(u>>32); f.dwLowDateTime=u&0xffffffff; return f; }
inline uint64_t SystemTimeToUINT64( const SYSTEMTIME& s, uint offset_days=0 ){ FILETIME f; SystemTimeToFileTime( &s, &f ); return FileTimeToUINT64(f,offset_days); }
inline bool operator==( const FILETIME& f1, const FILETIME& f2 ){	return CompareFileTime(&f1,&f2)==0; }
inline bool operator!=( const FILETIME& f1, const FILETIME& f2 ){	return CompareFileTime(&f1,&f2)!=0; }
inline bool operator>(  const FILETIME& f1, const FILETIME& f2 ){	return CompareFileTime(&f1,&f2)>0; }
inline bool operator<(  const FILETIME& f1, const FILETIME& f2 ){	return CompareFileTime(&f1,&f2)<0; }
inline bool operator>=( const FILETIME& f1, const FILETIME& f2 ){	return CompareFileTime(&f1,&f2)>=0; }
inline bool operator<=( const FILETIME& f1, const FILETIME& f2 ){	return CompareFileTime(&f1,&f2)<=0; }

/******************************************************************** 
1. path is only handled with wchar_t (no const char*)
2. input type: const wchar_t*
3. return type: const wchar_t*
********************************************************************/

//*******************************************************************
class path
{
protected:
	wchar_t* data;	// for casting without (const wchar_t*)

public:
	static const int max_buffers = 1024;
	static const int capacity = 1024;		// MAX_PATH == 260
	typedef struct _stat stat_t;			// use "struct _stat" instead of "_stat" for C-compatibility
	
	// get shared buffer for return values
	static const wchar_t* __wcsistr( const wchar_t* _Str1, const wchar_t* _Str2 ){ size_t l1=wcslen(_Str1); wchar_t* s1=(wchar_t*)memcpy(__wcsbuf(),_Str1,sizeof(wchar_t)*l1); s1[l1]=L'\0'; for(wchar_t* s=s1;*s;s++) *s=towlower(*s); size_t l2=wcslen(_Str2); wchar_t* s2=(wchar_t*)memcpy(__wcsbuf(),_Str2,sizeof(wchar_t)*l2); s2[l2]=L'\0'; for(wchar_t* s=s2;*s;s++) *s=towlower(*s); const wchar_t* r=wcsstr(s1,s2); return r?_Str1+(r-s1):nullptr; }
	static wchar_t*	__wcsbuf(){ static wchar_t buff[max_buffers][capacity]; static int i=0; return buff[i=(i+1)%std::extent<decltype(buff)>::value];}
	static char*	__strbuf(){ return (char*)__wcsbuf(); }
	struct split_info { wchar_t *drive, *dir, *fname, *ext; }; split_info split( wchar_t* drive=nullptr, wchar_t* dir=nullptr, wchar_t* fname=nullptr, wchar_t* ext=nullptr ) const { split_info si={drive,dir,fname,ext};_wsplitpath_s(data,si.drive,si.drive?_MAX_DRIVE:0,si.dir,si.dir?_MAX_DIR:0,si.fname,si.fname?_MAX_FNAME:0,si.ext,si.ext?_MAX_EXT:0);if(si.drive) si.drive[0]=toupper(si.drive[0]); return si;}
	static inline wchar_t* __mb2wc( const char* _Src, wchar_t* _Dst ){ MultiByteToWideChar(0 /*CP_ACP*/,0,_Src,-1,_Dst,min(capacity-1,MultiByteToWideChar(0 /*CP_ACP*/,0,_Src,-1,0,0))); return _Dst; }
	static inline bool __wcsiext( const wchar_t* ext_filter, const wchar_t* filename ){ static wchar_t ext[path::capacity]; _wsplitpath_s(filename,nullptr,0,nullptr,0,nullptr,0,ext,_MAX_EXT); if(ext[0]==0) return false; size_t ext_len=wcslen(ext); ext[0]=L';'; ext[ext_len]=L';'; ext[ext_len+1]=0; /* ext include dot first */ return path::__wcsistr(ext_filter,ext)!=nullptr; }

	// auxiliary cache information from scan()
	struct cache_t { FILETIME mtime; uint64_t size; DWORD attributes; };

	// destructor/constuctors
	~path(){free(data);}
	path():data((wchar_t*)malloc(sizeof(wchar_t)*capacity+sizeof(cache_t))){data[0]=L'\0';}
	path( const path& p ):path(){ wcscpy(data,p); memcpy(&cache(),&p.cache(),sizeof(cache_t)); } // do not canonicalize for copy constructor
	path( path&& p ):path(){ std::swap(data,p.data); }
	path( const wchar_t* s ):path(){ wcscpy(data,s); _canonicalize(); }
	path( const char* s ):path(){ __mb2wc(s,data); _canonicalize(); }
	explicit path( const std::wstring& s ):path(s.c_str()){}
	explicit path( const std::string& s ):path(s.c_str()){}

	// operator overloading: assignment
	path& operator=( const path& p ){ wcscpy(data,p.data); memcpy(&cache(),&p.cache(),sizeof(cache_t)); return *this; }
	path& operator=( path&& p ){ std::swap(data,p.data); return *this; }
	path& operator=( const wchar_t* s ){ wcscpy(data,s); _canonicalize(); return *this; }
	path& operator=( const char* s ){ __mb2wc(s,data); _canonicalize(); return *this; }
	path& operator=( const std::wstring& s ){ return operator=(s.c_str()); }
	path& operator=( const std::string& s ){ return operator=(s.c_str()); }

	// operator overloading: concatenations
	path& operator+=( const path& p ){ wcscat(data,p.data+((p.data[0]==L'.'&&p.data[1]==L'\\'&&p.data[2])?2:0)); _canonicalize(); return *this; }
	path& operator+=( const wchar_t* s ){ wcscat(data,s+((s[0]==L'.'&&s[1]==L'\\'&&s[2])?2:0)); _canonicalize(); return *this; }
	path& operator+=( const char* s ){ return operator+=(path(s)); }
	path& operator+=( const std::wstring& s ){ return operator+=(s.c_str()); }
	path& operator+=( const std::string& s ){ return operator+=(s.c_str()); }

	path& operator/=( const path& p ){ return *this=addBackslash()+p; }
	path& operator/=( const wchar_t* s ){ return *this=addBackslash()+s; }
	path& operator/=( const char* s ){ return *this=addBackslash()+s; }
	path& operator/=( const std::wstring& s ){ return operator/=(s.c_str()); }
	path& operator/=( const std::string& s ){ return operator/=(s.c_str()); }

	path operator+( const path& p ) const { return clone().operator+=(p); }
	path operator+( const wchar_t* s ) const { return clone().operator+=(s); }
	path operator+( const char* s ) const { return clone().operator+=(s); }
	path operator+( const std::wstring& s ) const { return clone().operator+=(s.c_str()); }
	path operator+( const std::string& s ) const { return clone().operator+=(s.c_str()); }

	path operator/( const path& p ) const { return clone().operator/=(p); }
	path operator/( const wchar_t* s ) const { return clone().operator/=(s); }
	path operator/( const char* s ) const { return clone().operator/=(s); }
	path operator/( const std::wstring& s ) const { return clone().operator/=(s.c_str()); }
	path operator/( const std::string& s ) const { return clone().operator/=(s.c_str()); }

	// operator overloading: comparisons
	bool operator==( const path& p )	const { return _wcsicmp(data,p.data)==0; }
	bool operator==( const wchar_t* s )	const { return _wcsicmp(data,s)==0; }
	bool operator!=( const path& p )	const { return _wcsicmp(data,p.data)!=0; }
	bool operator!=( const wchar_t* s )	const { return _wcsicmp(data,s)!=0; }
	bool operator<( const path& p )		const { return _wcsicmp(data,p.data)<0; }
	bool operator>( const path& p )		const { return _wcsicmp(data,p.data)>0; }
	bool operator<=( const path& p )	const { return _wcsicmp(data,p.data)<=0; }
	bool operator>=( const path& p )	const { return _wcsicmp(data,p.data)>=0; }

	// operator overloading: casting
	operator wchar_t*(){ return data; }
	operator const wchar_t*() const { return data; }
	const wchar_t* c_str() const { return data; }

	// operator overloading: array operator
	inline wchar_t& operator[]( size_t i ){ return data[i]; }
	inline wchar_t& operator[]( int i ){ return data[i]; }
	inline const wchar_t& operator[]( size_t i ) const { return data[i]; }
	inline const wchar_t& operator[]( int i ) const { return data[i]; }

	// iterators
	using iterator = wchar_t*;
	using const_iterator = const wchar_t*;
	iterator begin() const { return data; }
	iterator end() const { return data+length(); }
	wchar_t& front() const { return data[0]; }
	wchar_t& back() const { return data[0]==0?data[0]:data[length()-1]; }
	
	// clear and trivial clone
	void clear(){ data[0]=0; }
	path clone() const { return path(*this); }

	// attribute by stats
	inline DWORD& attributes() const { return cache().attributes=GetFileAttributesW(data); }
	inline stat_t stat() const { stat_t s={0}; if(exists()) _wstat(data,&s); return s; }
	inline cache_t& cache() const { return *((cache_t*)(data+capacity)); }
	inline cache_t& update_cache(){ cache().attributes=attributes(); cache().mtime=mfiletime(); cache().size=file_size(); return cache(); }

	// size/length
	inline bool empty()			const { return data[0]==0; }
	inline size_t size()		const { return data[0]==0?0:wcslen(data); }
	inline size_t length()		const { return data[0]==0?0:wcslen(data); }
	inline size_t file_size()	const { if(!exists()) return 0; return size_t(cache().size=stat().st_size); }

	// md5/crc checksums: implemented in gxmemory.h
	inline std::string path::md5() const;
	inline uint path::crc32() const;
	inline uint path::crc32c() const;

	// system-related: slash, backslash, unix, quote
	path slashToBackslash()	const {	path p(*this); std::replace(p.begin(),p.end(),L'/',L'\\'); return p; }
	path backslashToSlash()	const {	path p(*this); std::replace(p.begin(),p.end(),L'\\',L'/'); return p; }
	path addBackslash()		const { path p(*this); size_t len=wcslen(p.data); if(len&&p.data[len-1]!='\\'){p.data[len]='\\';p.data[len+1]=L'\0';} return p; }
	path removeBackslash()	const { path p(*this); size_t len=wcslen(p.data); if(len&&p.data[len-1]=='\\'){p.data[len-1]=L'\0';} return p; }
	path removeFirstDot()	const { return (wcslen(data)>2&&data[0]==L'.'&&data[1]==L'\\') ? path(data+2) : *this; }
	path auto_quote()		const { if(data[0]==0||wcschr(data,L' ')==nullptr||(data[0]==L'\"'&&data[wcslen(data)-1]==L'\"')) return *this; path p; swprintf_s(p,capacity*sizeof(wchar_t),L"\"%s\"",data); return p; }
	path unix()				const {	path p(*this); p._canonicalize(); p=p.backslashToSlash(); size_t len=p.length(); if(p.is_relative()||p.is_unc()||len<2) return p; p.data[1]=tolower(p.data[0]); p.data[0]=L'/'; return p; }
	path cygwin()			const { path p(*this); p._canonicalize(); p=p.backslashToSlash(); size_t len=p.length(); if(p.is_relative()||p.is_unc()||len<2) return p; path p2; swprintf_s( p2, capacity*sizeof(wchar_t), L"/cygdrive/%c%s", tolower(p[0]), p+2 ); return p2; }

	// path info/operations
	void setWorkingDirectory() const { if(is_dir()) _wchdir(data); }
	path drive() const { return split(__wcsbuf()).drive; }
	path dir() const { path p; if(wcschr(data,L'\\')==nullptr) return L".\\"; split_info si=split(__wcsbuf(),__wcsbuf()); wcscpy(p.data,wcscat(si.drive,si.dir)); size_t len=wcslen(p.data); if(len>0&&p.data[len-1]!='\\'){p.data[len]='\\';p.data[len+1]=L'\0';} return p; }
	path dirfilename() const { split_info si=split(__wcsbuf(),__wcsbuf(),__wcsbuf()); return wcscat(wcscat(si.drive,si.dir),si.fname); }
	path name( bool bExt=true ) const { split_info si=split(nullptr,nullptr,__wcsbuf(),__wcsbuf()); return bExt?wcscat(si.fname,si.ext):si.fname; }
	path ext() const { split_info si=split(nullptr,nullptr,nullptr,__wcsbuf()); return (si.ext[0]==0)?path():path(si.ext+1); }
	path parent() const { return dir().removeBackslash().dir(); }
	std::vector<path> explode() const {std::vector<path> L;wchar_t* ctx;for(wchar_t* t=wcstok_s(wcscpy(__wcsbuf(),data),L"/\\",&ctx);t;t=wcstok_s(0,L"/\\",&ctx))L.push_back(path(t));return L;}
	std::vector<path> relative_ancestors() const { std::vector<path> a, e=dir().relative().explode(); if(e.empty()) return a; path t=e.front().absolute().addBackslash(); a.push_back(t); for(size_t k=1,kn=e.size();k<kn;k++)a.push_back(t+=e[k].addBackslash()); return a; }

	// is file or directory
	bool exists() const { if(data[0]==0) return false; return attributes()!=INVALID_FILE_ATTRIBUTES; } // return _waccess(data,0)==0;
	bool is_dir() const { if(data[0]==0) return false; DWORD a=attributes(); return a!=INVALID_FILE_ATTRIBUTES&&(a&FILE_ATTRIBUTE_DIRECTORY)!=0; }
	
	// get/set attributes
	bool isHidden() const { return data[0]!=0&&_waccess(data,0)==0&&(attributes()&FILE_ATTRIBUTE_HIDDEN); }
	bool isReadOnly() const { return data[0]!=0&&_waccess(data,0)==0&&(attributes()&FILE_ATTRIBUTE_READONLY); }
	void setHidden( bool bHidden ) const { if(!exists()) return; if(bHidden) SetFileAttributesW(data,attributes()|=FILE_ATTRIBUTE_HIDDEN); else SetFileAttributesW(data,attributes()^=FILE_ATTRIBUTE_HIDDEN);}
	void setReadOnly( bool bReadOnly ) const { if(!exists()) return; if(bReadOnly) SetFileAttributesW(data,attributes()|=FILE_ATTRIBUTE_READONLY); else SetFileAttributesW(data,attributes()^=FILE_ATTRIBUTE_READONLY);}
	
	// make/copy/delete file/dir operations
	bool mkdir() const 	// make all super directories
	{
		if(exists()) return false; std::vector<path> d; path p=slashToBackslash().removeBackslash();wchar_t* ctx;for( wchar_t* t=wcstok_s(p,L"\\",&ctx); t; t=wcstok_s(nullptr,L"\\", &ctx) ){ d.push_back((d.empty()?path(L""):d.back())+t+L"\\"); }
		for(size_t k=0;k<d.size();k++) if(!d[k].exists()&&_wmkdir(d[k].data)!=0) return false; return true;
	}
	bool copy_file( path dst, bool overwrite=true ) const { if(!exists()||is_dir()) return false; if(dst.exists()&&dst.is_dir()) dst=dst.addBackslash()+name(); if(dst.exists()&&overwrite){ if(dst.isHidden()) dst.setHidden(false); if(dst.isReadOnly()) dst.setReadOnly(false); } return CopyFileW( data, dst, overwrite?FALSE:TRUE )?true:false; }
	bool move_file( path dst, bool overwrite=true ) const { if(!copy_file(dst,overwrite)||!exists()) return false; return deleteFile(); }
#ifndef _INC_SHELLAPI 
	bool rmdir() const { if(!is_dir()) return false; if(isHidden()) setHidden(false); if(isReadOnly()) setReadOnly(false); _wrmdir(data); return true; }
	bool rmfile() const { return deleteFile(); }
	bool deleteFile() const { if(!exists()||is_dir()) return false; if(isHidden()) setHidden(false); if(isReadOnly()) setReadOnly(false); return DeleteFileW(data)==TRUE; }
#else
	bool rmdir() const { if(!is_dir()) return false; if(isHidden()) setHidden(false); if(isReadOnly()) setReadOnly(false); SHFILEOPSTRUCTW fop={0};fop.wFunc=FO_DELETE;fop.fFlags=FOF_ALLOWUNDO|FOF_SILENT|FOF_NOCONFIRMATION;fop.pFrom=data;data[wcslen(data)+1]=L'\0';return SHFileOperationW(&fop)==0;}
	bool rmfile() const { return deleteFile(); }
	bool deleteFile( bool bUndo=false ) const { if(!exists()||is_dir()) return false; if(isHidden()) setHidden(false); if(isReadOnly()) setReadOnly(false); SHFILEOPSTRUCTW fop={0};fop.wFunc=FO_DELETE;fop.fFlags=(bUndo?FOF_ALLOWUNDO:0)|FOF_SILENT|FOF_NOCONFIRMATION|FOF_FILESONLY;fop.pFrom=data;data[wcslen(data)+1]=L'\0';return SHFileOperationW(&fop)==0;}
	void open( const wchar_t* args=nullptr, bool bShowWindow=true ) const {path cmd;swprintf(cmd,capacity,L"\"%s\"",data);ShellExecuteW(GetDesktopWindow(),L"Open",cmd,args,nullptr,bShowWindow?SW_SHOW:SW_HIDE);}
#endif

	// relative/absolute path
	inline bool is_absolute() const { return (data[0]!=0&&data[1]==L':')||memcmp(data,L"\\\\",sizeof(wchar_t)*2)==0||memcmp(data,L"//",sizeof(wchar_t)*2)==0; }
	inline bool is_relative() const { return !is_absolute(); }
	inline bool is_unc() const { return (data[0]==L'\\'&&data[1]==L'\\')||(data[0]==L'/'&&data[1]==L'/'); }
	inline bool is_subdir( const path& parent ) const { path p=parent.canonical(); return _wcsnicmp(canonical(),p,p.size())==0; } // do not check existence
	inline path absolute( const wchar_t* base_for_relative=L"" ) const { return _wfullpath(__wcsbuf(),(!base_for_relative[0]||is_absolute())?data:wcscat(wcscpy(__wcsbuf(),base_for_relative),data),capacity); }	// do not directly return for non-canonicalized path
	inline path relative( const wchar_t* from=L"" ) const;
	inline path canonical() const { path p=*this; p._canonicalize(); return p; }

	// create process
	inline void createProcess( const wchar_t* arguments=nullptr, bool bShowWindow=true, bool bWaitFinish=false ) const;

	// time stamp
	static const char* timestamp( const struct tm* t ){char* buff=__strbuf();sprintf(buff,"%04d%02d%02d%02d%02d%02d",t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);return buff;}
	static const char* timestamp( const SYSTEMTIME& t ){char* buff=__strbuf();sprintf(buff,"%04d%02d%02d%02d%02d%02d",t.wYear,t.wMonth,t.wDay,t.wHour,t.wMinute,t.wSecond);return buff;}
	const char* ctimestamp() const { stat_t s=stat(); return exists()?timestamp(_gmtime64(&s.st_ctime)):""; }
	const char* atimestamp() const { stat_t s=stat(); return exists()?timestamp(_gmtime64(&s.st_atime)):""; }
	const char* mtimestamp() const { stat_t s=stat(); return exists()?timestamp(_gmtime64(&s.st_mtime)):""; }
	
	// current time
	static SYSTEMTIME now(){ SYSTEMTIME s; GetSystemTime(&s); return s; }
	static const char* nowstamp(){ return timestamp(now()); }
	
	struct FILETIMES { FILETIME c, a, m; };
	FILETIMES filetimes() const { FILETIMES f;HANDLE h=CreateFileW(data,FILE_READ_ATTRIBUTES,0,0,OPEN_EXISTING,0,0);if(h){GetFileTime(h,&f.c,&f.a,&f.m);CloseHandle(h);cache().mtime=f.m=DiscardFileTimeMilliseconds(f.m);} return f;}
	FILETIME cfiletime() const { FILETIME t; HANDLE h=CreateFileW(data,FILE_READ_ATTRIBUTES,0,0,OPEN_EXISTING,0,0);if(h){GetFileTime(h,&t,0,0);CloseHandle(h);} return t; }
	FILETIME afiletime() const { FILETIME t; HANDLE h=CreateFileW(data,FILE_READ_ATTRIBUTES,0,0,OPEN_EXISTING,0,0);if(h){GetFileTime(h,0,&t,0);CloseHandle(h);} return t; }
	FILETIME mfiletime() const { FILETIME t; HANDLE h=CreateFileW(data,FILE_READ_ATTRIBUTES,0,0,OPEN_EXISTING,0,0);if(h){GetFileTime(h,0,0,&t);CloseHandle(h);cache().mtime=t=DiscardFileTimeMilliseconds(t);} return t; }
	SYSTEMTIME cfilesystemtime() const { FILETIME f=cfiletime(); SYSTEMTIME s; FileTimeToSystemTime(&f,&s); return s; }
	SYSTEMTIME afilesystemtime() const { FILETIME f=afiletime(); SYSTEMTIME s; FileTimeToSystemTime(&f,&s); return s; }
	SYSTEMTIME mfilesystemtime() const { FILETIME f=mfiletime(); SYSTEMTIME s; FileTimeToSystemTime(&f,&s); return s; }
	void setFileTime( FILETIME* c, FILETIME* a, FILETIME* m ) const {HANDLE h=CreateFileW( data, FILE_WRITE_ATTRIBUTES, 0, nullptr, OPEN_EXISTING, 0, nullptr );if(h){SetFileTime(h, c, a, &(cache().mtime=*m) );CloseHandle(h);}}
	void setFileTime( SYSTEMTIME* c, SYSTEMTIME* a, SYSTEMTIME* m ) const { FILETIMES f; if(c) SystemTimeToFileTime(c,&f.c); if(a) SystemTimeToFileTime(a,&f.a); if(m) SystemTimeToFileTime(m,&f.m); HANDLE h=CreateFileW( data, FILE_WRITE_ATTRIBUTES, 0, nullptr, OPEN_EXISTING, 0, nullptr ); if(h){SetFileTime(h,c?&f.c:nullptr,a?&f.a:nullptr,m?&(cache().mtime=f.m):nullptr);CloseHandle(h);}}
	void setFileTimeToNow() const { SYSTEMTIME s=now(); setFileTime(&s,&s,&s); }

	// scan/findfile: ext_filter (specific extensions delimited by semicolons), str_filter (path should contain this string)
	struct scan_info { std::vector<path>& result; bool recursive; const wchar_t* ext; const wchar_t* str; };
	inline std::vector<path> scan( bool bRecursive=true, const wchar_t* ext_filter=nullptr, const wchar_t* str_filter=nullptr ) const;

private:
	inline void _canonicalize(); // remove redundant dir indicates such as "..", "."
	//inline void _scan( const path& dir, const scan_info& si ) const;
	inline void _scan( path&& dir, const scan_info& si ) const;
};

//*******************************************************************
// definitions of inline-long member functions

inline void path::_scan( path&& dir, const path::scan_info& si ) const
{
	std::vector<path> child_dirs; child_dirs.reserve(64);
	WIN32_FIND_DATAW fd; HANDLE h; wchar_t* f=fd.cFileName; path t; bool recursive=si.recursive;
	for( h=FindFirstFileExW(dir+L"*.*",FindExInfoBasic/*minimal(faster)*/,&fd,FindExSearchNameMatch,0,FIND_FIRST_EX_LARGE_FETCH); h!=INVALID_HANDLE_VALUE&&FindNextFileW(h,&fd); )
	{
		if(f[0]==L'.'&&(!f[1]||(f[1]==L'.'&&!f[2]))) continue;
		if(fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY){ if(recursive){wcscat(wcscpy(t,dir),f);size_t l=wcslen(t);t[l]=L'\\';t[l+1]=0;child_dirs.push_back(t);} continue; }
		if(!((si.ext==nullptr||__wcsiext(si.ext,f))&&(si.str==nullptr||__wcsistr(f,si.str)!=0))) continue;
		wcscat(wcscpy(t,dir),f); si.result.push_back(std::move(t)); path& r=si.result.back(); 
		cache_t& d=r.cache(); d.size=fd.nFileSizeLow;d.size+=(uint64_t(fd.nFileSizeHigh)<<32ull);d.attributes=fd.dwFileAttributes;
		SYSTEMTIME s;FileTimeToSystemTime(&fd.ftLastWriteTime,&s);s.wMilliseconds=0;SystemTimeToFileTime(&s,&d.mtime);
	}
	FindClose(h);
	for(size_t k=0,kn=child_dirs.size();k<kn;k++) _scan( std::move(child_dirs[k]), si );
}

inline std::vector<path> path::scan( bool bRecursive, const wchar_t* ext_filter, const wchar_t* str_filter ) const
{
	std::vector<path> result; if(!is_dir()) return result; result.reserve(8192);
	std::wstring ext_filter1=ext_filter?std::wstring(L";")+std::wstring(ext_filter)+L";":L"";
	scan_info si = { result, bRecursive, ext_filter?ext_filter1.c_str():nullptr, str_filter };
	path src=(is_relative()?absolute(L".\\"):*this).addBackslash();
	_scan( std::move(src), si ); return result;
}

inline path path::relative( const wchar_t* from ) const
{
	if(is_relative()) return *this;

	path md; GetModuleFileNameW(0,md,capacity);
	path fromDir = (from&&from[0]) ? path(from).dir().absolute() : path(md).dir().absolute();
	if(fromDir.drive()[0]!=this->drive()[0]) return *this;

	// 1. keep filename, make the list of reference and current path
	std::vector<path> srcList=fromDir.explode(), dstList=dir().explode();

	// 2. compare and count the different directory levels
	path result;
	size_t s=0,d=0,ss=srcList.size(),ds=dstList.size();
	for( ; s<ss&&d<ds; s++, d++ ) if(_wcsicmp(srcList[s],dstList[d])!=0) break;
	for( ; s<ss; s++ ) result += L"..\\";
	for( ; d<ds; d++ ){ result += path(dstList[d]).addBackslash(); }

	// 3. if empty dir, then attach ./
	if(result.empty()||result[0]!=L'.') result=path(".\\")+result;
	result._canonicalize();
	return this->is_dir()?result:result+name();
}

inline void path::createProcess( const wchar_t* arguments, bool bShowWindow, bool bWaitFinish ) const
{
	wchar_t cmd[4096]; swprintf_s( cmd, 4095, L"\"%s\" %s", data, arguments?arguments:L""  );
	PROCESS_INFORMATION pi={0}; STARTUPINFOW si={0}; si.cb=sizeof(si); si.dwFlags=STARTF_USESHOWWINDOW; si.wShowWindow=bShowWindow?SW_SHOW:SW_HIDE;
	CreateProcessW( nullptr, (LPWSTR)cmd, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi );
	if(bWaitFinish) WaitForSingleObject(pi.hProcess,INFINITE);
	CloseHandle( pi.hThread );    // must release handle
	CloseHandle( pi.hProcess );   // must release handle
}

inline void path::_canonicalize()
{
	size_t len = length(); if(len==0) return;
	for(uint k=0;k<len;k++) if(data[k]==L'/') data[k]=L'\\'; // slash to backslash
	if(data[len-1]==L'.'&&((len>2&&data[len-2]==L'\\')||(len>3&&data[len-3]==L'\\'&&data[len-2]==L'.'))){ data[len++]=L'\\'; data[len]=L'\0'; } // add trailing slash to "\\." or "\\.."
	if(len==2&&data[1]==L':'){ data[len++]=L'\\'; data[len]=L'\0'; } // root correction
	if(!wcsstr(data,L"\\.\\")&&!wcsstr(data,L"\\..\\")) return; // trivial return
	wchar_t* ds; while( (ds=wcsstr(data+1,L"\\\\")) ) memmove(ds+1,ds+2,((len--)-(ds-data)-1)*sizeof(wchar_t));	// correct multiple backslashes, except the beginning of unc path
	if(is_absolute()){ _wfullpath(data,(const wchar_t*)memcpy(__wcsbuf(),data,sizeof(wchar_t)*(len+1)),capacity); return; }
	
	// misc. flags to check
	bool bTrailingBackslash = (data[len-1]==L'\\');
	bool bSingleDotBegin = data[0]==L'.'&&data[1]==L'\\';

	// perform canonicalization
	std::deque<wchar_t*> L;wchar_t* ctx;
	for(wchar_t* t=wcstok_s(wcscpy(__wcsbuf(),data),L"\\",&ctx);t;t=wcstok_s(nullptr,L"\\",&ctx))
	{
		if(t[0]==L'.'&&t[1]==L'.'&&!L.empty()&&memcmp(L.back(),L"..",sizeof(wchar_t)*2)!=0) L.pop_back();
		else if(t[0]!=L'.'||t[1]!=0) L.push_back(wcscpy(__wcsbuf(),t));
	}

	// reconstruct the path
	wchar_t* d=data;
	if(!bSingleDotBegin&&L.empty()){ d[0]=L'\0'; return; }
	if(L.empty()||(bSingleDotBegin&&(!L.empty()&&L.front()[0]!=L'.'))){ (d++)[0]=L'.'; (d++)[0]=L'\\'; d[0]=L'\0'; } // make it begin with single-dot again
	for(size_t k=0,kn=L.size();k<kn;k++){ wchar_t* t=L[k]; size_t l=wcslen(t); memcpy(d,t,l*sizeof(wchar_t)); d+=l; (d++)[0]=L'\\'; d[0]=L'\0'; }
	if(!bTrailingBackslash&&d>data)(d-1)[0]=L'\0';
}

//*******************************************************************
// module/working dir retrieval
inline path getModulePath( HMODULE hModule=nullptr ){ static path p0; if(p0[0]&&hModule==nullptr) return p0; path p; GetModuleFileNameW(hModule,p,path::capacity); p[0]=toupper(p[0]); p=p.canonical(); return hModule==nullptr?p0=p:p; }
inline path getWorkingDir(){ path p; _wgetcwd(p,path::capacity); return p.absolute().addBackslash(); }
inline path getWorkingDirectory(){ path p; _wgetcwd(p,path::capacity); return p.absolute().addBackslash(); }
inline void setModuleDirAsWorkingDir(){ _wchdir(getModulePath().dir()); }

//*******************************************************************
// temp dir/names
inline path getTempDir( const wchar_t* baseName=L"" ){path d;GetTempPathW(path::capacity,d);d[0]=toupper(d[0]);d=d.absolute();path bn=path(baseName);path m=bn.empty()?getModulePath().dir().removeBackslash():bn;size_t ds=d.size(),ms=m.size();size_t c=0;for(size_t k=0;k<ms;k++){ if(m[k]==L':') continue; d[ds+(c++)]=(m[k]==L'\\'||m[k]==L'/'?L'.':m[k]); } d[ds+c]=L'\0'; d=d.addBackslash(); if(!d.exists())d.mkdir();return d;}
inline path getSerialFilePath( path dir, const wchar_t* prefix, const wchar_t* postfix, int numzero=4 )
{
	dir=dir.addBackslash(); if(!dir.exists()) dir.mkdir();
	int nMaxFiles=1; for(int k=0;k<numzero;k++) nMaxFiles*=10;
	path fmt,p; swprintf((wchar_t*)fmt.c_str(),path::capacity,L"%s%s%%0%dd%s%s",dir.c_str(),prefix,numzero,postfix&&postfix[0]?L".":L"",postfix);
	for(int k=0;k<nMaxFiles;k++){swprintf(p,path::capacity,fmt,k);if(!p.exists()) return p;}
	swprintf(p,path::capacity,fmt,nMaxFiles-1);
	return p;
}

//*******************************************************************
// nocase/std map/unordered_map extension for path
namespace nocase
{
	template <> struct equal_to<path>:public std::binary_function<path,path,bool> { bool operator()(const path& a,const path& b)const{return _wcsicmp(a.c_str(),b.c_str())==0;}};
	template <> struct not_equal_to<path>:public std::binary_function<path,path,bool> { bool operator()(const path& a,const path& b)const{return _wcsicmp(a.c_str(),b.c_str())!=0;}};
	template <> struct greater<path>:public std::binary_function<path,path,bool> { bool operator()(const path& a,const path& b)const{return _wcsicmp(a.c_str(),b.c_str())>0;}};
	template <> struct less<path>:public std::binary_function<path,path,bool> { bool operator()(const path& a,const path& b)const{return _wcsicmp(a.c_str(),b.c_str())<0;}};
	template <> struct greater_equal<path>:public std::binary_function<path,path,bool> { bool operator()(const path& a,const path& b)const{return _wcsicmp(a.c_str(),b.c_str())>=0;}};
	template <> struct less_equal<path>:public std::binary_function<path,path,bool> { bool operator()(const path& a,const path& b)const{return _wcsicmp(a.c_str(),b.c_str())<=0;}};
}

namespace std
{
	template <> struct hash<path> { size_t operator()( const path& p ) const { return hash<wstring>()(p.c_str()); } };
}

//*******************************************************************
// operating-system utilities
inline BOOL CALLBACK EnumWindowsProc( HWND hwnd , LPARAM pProcessList ){ ((std::vector<HWND>*)pProcessList)->push_back(hwnd); return TRUE; }
inline std::vector<HWND> scanWindows( const wchar_t* filter=nullptr ){ std::vector<HWND> processList; EnumWindows( EnumWindowsProc, (LPARAM)(&processList) ); return processList; }
inline void usleep( int us ){ std::this_thread::sleep_for(std::chrono::microseconds(us)); }

//*******************************************************************
// timer
struct gxTimer
{
	union { double2 result; struct { double x, y; }; };
	inline gxTimer(){begin();}
	inline void clear(){result.x=result.y=now();}
	inline void begin(){result.x=now();}
	inline double end(){result.y=now(); return delta();}
	inline double delta(){return result.y-result.x;}
	static gxTimer* singleton(){ static gxTimer i; return &i; }
	static double now(){ static double c=0; if(c==0){int64_t f;QueryPerformanceFrequency((LARGE_INTEGER*)&f);c=1000.0/double(f);} static int64_t e=0; if(e==0){auto* ef=(int64_t(*)()) GetProcAddress(GetModuleHandleW(nullptr),"rex_timer_epoch");if(ef)e=ef();else QueryPerformanceCounter((LARGE_INTEGER*)&e);} int64_t i; QueryPerformanceCounter((LARGE_INTEGER*)&i); return double(i-e)*c; } // if rex found, use its epoch; otherwise, use a local epoch
};

//*******************************************************************
#endif __GX_FILESYSTEM__