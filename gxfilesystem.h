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
#ifndef __GX_FILESYSTEM_H__
#define __GX_FILESYSTEM_H__
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

#include <direct.h>		// directory control
#include <io.h>			// low-level io functions
#include <time.h>
#include <deque>
#ifndef __GNUC__		// MinGW has a problem with threads
	#include <thread>	// usleep
	#include <chrono>	// microtimer
#endif

//***********************************************
// Win32-like filetime utilities
inline uint64_t FileTimeOffset( uint days, uint hours=0, uint mins=0, uint secs=0, uint mss=0 ){ return 10000ull*(mss+1000ull*secs+60*1000ull*mins+60*60*1000ull*hours+24*60*60*1000ull*days); } // FILETIME in 100 ns scale
inline FILETIME DiscardFileTimeMilliseconds( FILETIME f ){uint64_t u=((uint64_t(f.dwHighDateTime)<<32|uint64_t(f.dwLowDateTime))/10000000)*10000000;return FILETIME{DWORD(u&0xffffffff),DWORD(u>>32)};} // 1ms = 10000 in FILETIME
inline SYSTEMTIME FileTimeToSystemTime( const FILETIME& f ){ SYSTEMTIME s; FileTimeToSystemTime(&f,&s); return s; }
inline FILETIME SystemTimeToFileTime( const SYSTEMTIME& s ){ FILETIME f; SystemTimeToFileTime(&s,&f); return f; }
inline uint64_t FileTimeToUint64( const FILETIME& f, uint offset_days=0 ){ return (uint64_t(f.dwHighDateTime)<<32|uint64_t(f.dwLowDateTime))-FileTimeOffset(offset_days); }
inline FILETIME	Uint64ToFileTime( uint64_t u ){ FILETIME f; f.dwHighDateTime=DWORD(u>>32); f.dwLowDateTime=u&0xffffffff; return f; }
inline uint64_t SystemTimeToUint64( const SYSTEMTIME& s, uint offset_days=0 ){ FILETIME f; SystemTimeToFileTime( &s, &f ); return FileTimeToUint64(f,offset_days); }
inline FILETIME Now(){ FILETIME f; GetSystemTimeAsFileTime(&f); return f; } // current time

inline bool operator==( const FILETIME& f1, const FILETIME& f2 ){	return CompareFileTime(&f1,&f2)==0; }
inline bool operator!=( const FILETIME& f1, const FILETIME& f2 ){	return CompareFileTime(&f1,&f2)!=0; }
inline bool operator>(  const FILETIME& f1, const FILETIME& f2 ){	return CompareFileTime(&f1,&f2)>0; }
inline bool operator<(  const FILETIME& f1, const FILETIME& f2 ){	return CompareFileTime(&f1,&f2)<0; }
inline bool operator>=( const FILETIME& f1, const FILETIME& f2 ){	return CompareFileTime(&f1,&f2)>=0; }
inline bool operator<=( const FILETIME& f1, const FILETIME& f2 ){	return CompareFileTime(&f1,&f2)<=0; }

struct path
{
	typedef WIN32_FILE_ATTRIBUTE_DATA attrib_t; // auxiliary cache information from scan()
	typedef struct _stat stat_t; // use "struct _stat" instead of "_stat" for C-compatibility

	static const int max_buffers = 4096;
	static const int capacity = 1024;		// MAX_PATH == 260
	
	// get shared buffer for return values
	static inline wchar_t*			__wcsbuf(){ static wchar_t buff[max_buffers][capacity]; static int i=0; return buff[(i++)%std::extent<decltype(buff)>::value]; }
	static inline char*				__strbuf(){ return (char*)__wcsbuf(); }
	static inline wchar_t*			__mb2wc( const char* _Src, wchar_t* _Dst ){ int l=MultiByteToWideChar(0,0,_Src,-1,0,0);if(l>capacity-1)l=capacity-1; MultiByteToWideChar(0,0,_Src,-1,_Dst,l); return _Dst; }
	static inline char*				__wc2mb( const wchar_t* _Src, char* _Dst ){ int l=WideCharToMultiByte(0,0,_Src,-1,0,0,0,0);if(l>capacity-1)l=capacity-1; WideCharToMultiByte(0,0,_Src,-1,_Dst,l,0,0); return _Dst; }
	static inline const wchar_t*	__tolower( const wchar_t* _Str, size_t l ){ wchar_t* s=(wchar_t*)memcpy(__wcsbuf(),_Str,sizeof(wchar_t)*l); s[l]=L'\0'; for(wchar_t* p=s;*p;p++) *p=towlower(*p); return s; }
	static inline const wchar_t*	__wcsistr( const wchar_t* _Str1, size_t l1, const wchar_t* _Str2, size_t l2 ){ const wchar_t *s1=__tolower(_Str1,l1), *s2=__tolower(_Str2,l2); const wchar_t* r=wcsstr(s1,s2); return r?_Str1+(r-s1):nullptr; }
	static inline const wchar_t*	__wcsistr( const wchar_t* _Str1, const wchar_t* _Str2 ){ return __wcsistr(_Str1,wcslen(_Str1),_Str2,wcslen(_Str2)); }
	static inline bool				__wcsiext( const wchar_t* fname, std::vector<std::wstring>* exts, size_t fl ){ for(size_t k=0,kn=exts->size();k<kn;k++){size_t el=exts->operator[](k).size();if(el<fl&&_wcsicmp(exts->operator[](k).c_str(),fname+fl-el)==0) return true; } return false; }
	static inline bool				__wcsiext( const wchar_t* fname, std::vector<std::wstring>* exts ){ return __wcsiext(fname,exts,wcslen(fname)); }

	// split path
	struct split_info { wchar_t *drive, *dir, *fname, *ext; };
	split_info split( wchar_t* drive=nullptr, wchar_t* dir=nullptr, wchar_t* fname=nullptr, wchar_t* ext=nullptr ) const { split_info si={drive,dir,fname,ext};_wsplitpath_s(data,si.drive,si.drive?_MAX_DRIVE:0,si.dir,si.dir?_MAX_DIR:0,si.fname,si.fname?_MAX_FNAME:0,si.ext,si.ext?_MAX_EXT:0);if(si.drive&&si.drive[0]) si.drive[0]=::toupper(si.drive[0]); return si;}

	// destructor/constuctors
	~path(){free(data);}
	path():data((wchar_t*)malloc(sizeof(wchar_t)*capacity+sizeof(attrib_t))){ data[0]=0; clear_cache(); }
	path( const path& p ):path(){ wcscpy(data,p); cache()=p.cache(); } // do not canonicalize for copy constructor
	path( path&& p ):path(){ std::swap(data,p.data); std::swap(cache(),p.cache()); }
	path( const wchar_t* s ):path(){ wcscpy(data,s); canonicalize(); }
	path( const char* s ):path(){ __mb2wc(s,data); canonicalize(); }
	explicit path( const std::wstring& s ):path(s.c_str()){}
	explicit path( const std::string& s ):path(s.c_str()){}

	// operator overloading: assignment
	path& operator=( const path& p ){ wcscpy(data,p.data); cache()=p.cache(); return *this; }
	path& operator=( path&& p ){ std::swap(data,p.data); std::swap(cache(),p.cache()); return *this; }
	path& operator=( const wchar_t* s ){ wcscpy(data,s); canonicalize(); return *this; }
	path& operator=( const char* s ){ __mb2wc(s,data); canonicalize(); return *this; }
	path& operator=( const std::wstring& s ){ return operator=(s.c_str()); }
	path& operator=( const std::string& s ){ return operator=(s.c_str()); }

	// operator overloading: concatenations
	path& operator+=( const path& p ){ wcscat(data,p.data+((p.data[0]==L'.'&&p.data[1]==L'\\'&&p.data[2])?2:0)); canonicalize(); return *this; }
	path& operator+=( const wchar_t* s ){ wcscat(data,s+((s[0]==L'.'&&s[1]==L'\\'&&s[2])?2:0)); canonicalize(); return *this; }
	path& operator+=( const char* s ){ return operator+=(path(s)); }
	path& operator+=( const std::wstring& s ){ return operator+=(s.c_str()); }
	path& operator+=( const std::string& s ){ return operator+=(s.c_str()); }

	path& operator/=( const path& p ){ return *this=add_backslash()+p; }
	path& operator/=( const wchar_t* s ){ return *this=add_backslash()+s; }
	path& operator/=( const char* s ){ return *this=add_backslash()+s; }
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
	
	// conversion to wstring and multibyte string
	std::wstring str() const { return std::wstring(data); }
	const char* wtoa() const { return __wc2mb(data,__strbuf()); }

	// operator overloading: array operator
	inline wchar_t& operator[]( ptrdiff_t i ){ return data[i]; }
	inline const wchar_t& operator[]( ptrdiff_t i ) const { return data[i]; }

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

	// size/length
	inline bool empty()			const { return data[0]==0; }
	inline size_t size()		const { return data[0]==0?0:wcslen(data); }
	inline size_t length()		const { return data[0]==0?0:wcslen(data); }
	inline uint64_t file_size() const { if(!cache_exists()) update_cache(); auto& c=cache(); return uint64_t(c.nFileSizeHigh)<<32ull|uint64_t(c.nFileSizeLow); }

	// md5/crc checksums: implemented in gxmemory.h
	inline std::string md5() const;
	inline uint crc32() const;
	inline uint crc32c() const;

	// system-related: slash, backslash, unix, quote
	path to_backslash()		const {	path p(*this); std::replace(p.begin(),p.end(),L'/',L'\\'); return p; }
	path to_slash()			const {	path p(*this); std::replace(p.begin(),p.end(),L'\\',L'/'); return p; }
	path add_backslash()	const { path p(*this); size_t len=wcslen(p.data); if(len&&p.data[len-1]!='\\'){p.data[len]='\\';p.data[len+1]=L'\0';} return p; }
	path remove_backslash()	const { path p(*this); size_t len=wcslen(p.data); if(len&&p.data[len-1]=='\\'){p.data[len-1]=L'\0';} return p; }
	path auto_quote()		const { if(data[0]==0||wcschr(data,L' ')==nullptr||(data[0]==L'\"'&&data[wcslen(data)-1]==L'\"')) return *this; path p; swprintf_s(p,capacity,L"\"%s\"",data); return p; }
	path unix()				const {	path p(*this); p.canonicalize(); p=p.to_slash(); size_t len=p.length(); if(len<2||p.is_relative()||p.is_unc()||p.is_rsync()) return p; p.data[1]=::tolower(p.data[0]); p.data[0]=L'/'; return p; }
	path cygwin()			const { path p(*this); p.canonicalize(); p=p.to_slash(); size_t len=p.length(); if(len<2||p.is_relative()||p.is_unc()||p.is_rsync()) return p; path p2; swprintf_s( p2, capacity, L"/cygdrive/%c%s", ::tolower(p[0]), p+2 ); return p2; }
	void chdir()			const { if(is_dir()) _wchdir(data); }

	// path info/operations
	path drive() const { path d; split(d.data); return d; }
	path dir() const { path p; if(wcschr(data,L'\\')==nullptr) return L".\\"; split_info si=split(__wcsbuf(),__wcsbuf()); wcscpy(p.data,wcscat(si.drive,si.dir)); size_t len=wcslen(p.data); if(len>0&&p.data[len-1]!='\\'){p.data[len]='\\';p.data[len+1]=L'\0';} return p; }
	path name( bool with_ext=true ) const { split_info si=split(nullptr,nullptr,__wcsbuf(),__wcsbuf()); return with_ext?wcscat(si.fname,si.ext):si.fname; }
	path ext() const { path e; split_info si=split(nullptr,nullptr,nullptr,e.data); return e.empty()?e:path(e.data+1); }
	path parent() const { return dir().remove_backslash().dir(); }
	path remove_first_dot()	const { return (wcslen(data)>2&&data[0]==L'.'&&data[1]==L'\\') ? path(data+2) : *this; }
	path remove_ext() const { split_info si=split(__wcsbuf(),__wcsbuf(),__wcsbuf()); return wcscat(wcscat(si.drive,si.dir),si.fname); }
	std::vector<path> explode() const { path slashed=to_slash(); std::vector<path> L;L.reserve(8); wchar_t* ctx; for(wchar_t* t=wcstok_s(slashed.data,L"/",&ctx);t;t=wcstok_s(0,L"/",&ctx)) L.emplace_back(t); return L; }
	std::vector<path> relative_ancestors() const { std::vector<path> a, e=dir().relative().explode(); if(e.empty()) return a; a.reserve(8); path t=e.front().absolute()+L"\\"; a.emplace_back(t); for(size_t k=1,kn=e.size();k<kn;k++)a.emplace_back(t+=e[k]+L"\\"); return a; }

	// attribute by stats
	inline stat_t	stat() const { stat_t s={0}; if(exists()) _wstat(data,&s); return s; }
	inline DWORD&	attributes() const { if(!cache_exists()) update_cache(); return cache().dwFileAttributes; }
	inline void		update_cache() const { auto& c=cache(); if(!GetFileAttributesExW(data,GetFileExInfoStandard,&c)||c.dwFileAttributes==INVALID_FILE_ATTRIBUTES){ memset(&c,0,sizeof(attrib_t)); c.dwFileAttributes=INVALID_FILE_ATTRIBUTES; return; } c.ftLastWriteTime = DiscardFileTimeMilliseconds(c.ftLastWriteTime); }
	inline void		clear_cache() const { attrib_t* a=(attrib_t*)(data+capacity); memset(a,0,sizeof(attrib_t)); a->dwFileAttributes=INVALID_FILE_ATTRIBUTES; }
	
	// get/set attributes
	bool exists() const {				if(!data[0]) return false; auto& a=attributes(); return a!=INVALID_FILE_ATTRIBUTES; }
	bool is_dir() const {				if(!data[0]) return false; auto& a=attributes(); return a!=INVALID_FILE_ATTRIBUTES&&(a&FILE_ATTRIBUTE_DIRECTORY)!=0; }
	bool is_hidden() const {			if(!data[0]) return false; auto& a=attributes(); return a!=INVALID_FILE_ATTRIBUTES&&(a&FILE_ATTRIBUTE_HIDDEN)!=0; }
	bool is_readonly() const {			if(!data[0]) return false; auto& a=attributes(); return a!=INVALID_FILE_ATTRIBUTES&&(a&FILE_ATTRIBUTE_READONLY)!=0; }
	void set_hidden( bool h ) const {	if(!exists()) return; auto& a=attributes(); SetFileAttributesW(data,a=h?(a|FILE_ATTRIBUTE_HIDDEN):(a^FILE_ATTRIBUTE_HIDDEN)); }
	void set_readonly( bool r ) const {	if(!exists()) return; auto& a=attributes(); SetFileAttributesW(data,a=r?(a|FILE_ATTRIBUTE_READONLY):(a^FILE_ATTRIBUTE_READONLY)); }
	
	// make/copy/delete file/dir operations
	bool mkdir() const { if(exists()) return false; path p=to_backslash().remove_backslash(), d; wchar_t* ctx;for( wchar_t* t=wcstok_s(p,L"\\",&ctx); t; t=wcstok_s(nullptr,L"\\", &ctx) ){ d+=t;d+=L"\\"; if(!d.exists()&&_wmkdir(d.data)!=0) return false; } return true; } // make all super directories
	bool copy_file( path dst, bool overwrite=true ) const { if(!exists()||is_dir()) return false; if(dst.exists()&&dst.is_dir()) dst=dst.add_backslash()+name(); if(dst.exists()&&overwrite){ if(dst.is_hidden()) dst.set_hidden(false); if(dst.is_readonly()) dst.set_readonly(false); } return CopyFileW( data, dst, overwrite?FALSE:TRUE )?true:false; }
	bool move_file( path dst, bool overwrite=true ) const { if(!copy_file(dst,overwrite)||!exists()) return false; return delete_file(); }
#ifndef _INC_SHELLAPI 
	bool rmdir() const { if(!is_dir()) return false; if(is_hidden()) set_hidden(false); if(is_readonly()) set_readonly(false); _wrmdir(data); return true; }
	bool rmfile() const { return delete_file(); }
	bool delete_file() const { if(!exists()||is_dir()) return false; if(is_hidden()) set_hidden(false); if(is_readonly()) set_readonly(false); return DeleteFileW(data)==TRUE; }
#else
	bool rmdir() const { if(!is_dir()) return false; if(is_hidden()) set_hidden(false); if(is_readonly()) set_readonly(false); SHFILEOPSTRUCTW fop={0};fop.wFunc=FO_DELETE;fop.fFlags=FOF_ALLOWUNDO|FOF_SILENT|FOF_NOCONFIRMATION;fop.pFrom=data;data[wcslen(data)+1]=L'\0';return SHFileOperationW(&fop)==0;}
	bool rmfile( bool b_undo=false ) const { return delete_file(b_undo); }
	bool delete_file( bool b_undo=false ) const { if(!exists()||is_dir()) return false; if(is_hidden()) set_hidden(false); if(is_readonly()) set_readonly(false); SHFILEOPSTRUCTW fop={0};fop.wFunc=FO_DELETE;fop.fFlags=(b_undo?FOF_ALLOWUNDO:0)|FOF_SILENT|FOF_NOCONFIRMATION|FOF_FILESONLY;fop.pFrom=data;data[wcslen(data)+1]=L'\0';return SHFileOperationW(&fop)==0;}
	void open( const wchar_t* args=nullptr, bool bShowWindow=true ) const {path cmd;swprintf(cmd,capacity,L"\"%s\"",data);ShellExecuteW(GetDesktopWindow(),L"Open",cmd,args,nullptr,bShowWindow?SW_SHOW:SW_HIDE);}
#endif

	// relative/absolute path
	inline bool is_absolute() const { return (data[0]!=0&&data[1]==L':')||memcmp(data,L"\\\\",sizeof(wchar_t)*2)==0||memcmp(data,L"//",sizeof(wchar_t)*2)==0||wcsstr(data,L":\\")!=nullptr||wcsstr(data,L":/")!=nullptr; }
	inline bool is_relative() const { return !is_absolute(); }
	inline bool is_unc() const { return (data[0]==L'\\'&&data[1]==L'\\')||(data[0]==L'/'&&data[1]==L'/'); }
	inline bool is_rsync() const { auto* p=wcsstr(data,L":\\"); if(!p) p=wcsstr(data,L":/"); return p!=nullptr&&p>data+1; }
	inline bool is_subdir( const path& parent ) const { path p=parent.canonical(); return _wcsnicmp(canonical(),p,p.size())==0; } // do not check existence
	inline path absolute( const wchar_t* base=L"" ) const { return _wfullpath(__wcsbuf(),(!*base||is_absolute())?data:wcscat(wcscpy(__wcsbuf(),path(base).add_backslash()),data),capacity); }	// do not directly return for non-canonicalized path
	inline path relative( const wchar_t* from=L"" ) const;
	inline path canonical() const { path p=*this; p.canonicalize(); return p; }

	// create process
	void create_process( const wchar_t* arguments=nullptr, bool bShowWindow=true, bool bWaitFinish=false ) const;

	// time stamp
	static const char* timestamp( const struct tm* t ){char* buff=__strbuf();sprintf(buff,"%04d%02d%02d%02d%02d%02d",t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);return buff;}
	static const char* timestamp( const FILETIME& f ){ SYSTEMTIME s;FileTimeToSystemTime(&f,&s);char* buff=__strbuf();sprintf(buff,"%04d%02d%02d%02d%02d%02d",s.wYear,s.wMonth,s.wDay,s.wHour,s.wMinute,s.wSecond);return buff; }
	const char* ctimestamp() const { if(!exists()) return ""; stat_t s=stat(); struct tm t; _gmtime64_s(&t,&s.st_ctime); return timestamp(&t); }
	const char* atimestamp() const { if(!exists()) return ""; stat_t s=stat(); struct tm t; _gmtime64_s(&t,&s.st_atime); return timestamp(&t); }
	const char* mtimestamp() const { if(!exists()) return ""; stat_t s=stat(); struct tm t; _gmtime64_s(&t,&s.st_mtime); return timestamp(&t); }
	
	FILETIME cfiletime() const { if(!cache_exists()) update_cache(); return cache().ftCreationTime; }
	FILETIME afiletime() const { if(!cache_exists()) update_cache(); return cache().ftLastAccessTime; }
	FILETIME mfiletime() const { if(!cache_exists()) update_cache(); return cache().ftLastWriteTime; }
	SYSTEMTIME cfilesystemtime() const { return FileTimeToSystemTime(cfiletime()); }
	SYSTEMTIME afilesystemtime() const { return FileTimeToSystemTime(afiletime()); }
	SYSTEMTIME mfilesystemtime() const { return FileTimeToSystemTime(mfiletime()); }
	void set_filetime( const FILETIME* ctime, const FILETIME* atime, const FILETIME* mtime ) const { HANDLE h=CreateFileW(data,FILE_WRITE_ATTRIBUTES,0,nullptr,OPEN_EXISTING,0,nullptr); if(!h)return; auto& c=cache(); if(ctime) c.ftCreationTime=*ctime; if(atime) c.ftLastAccessTime=*atime; if(mtime) c.ftLastWriteTime=*mtime; SetFileTime(h, ctime, atime, mtime ); CloseHandle(h); }
	void set_filetime( const FILETIME& f ) const { set_filetime(&f,&f,&f); }

	// module/working directories
	static inline path module( HMODULE h_module=nullptr ){ static path m; if(!m.empty()&&!h_module) return m; path p;GetModuleFileNameW(h_module,p,path::capacity);p[0]=::toupper(p[0]); p=p.canonical(); return h_module?p:(m=p); }
	static inline path module_dir( HMODULE h_module=nullptr ){ static path d=module().dir(); return h_module?module(h_module).dir():d; }
	static inline path cwd(){ path p; _wgetcwd(p,path::capacity); return p.absolute().add_backslash(); }	// current working dir
	static inline void chdir( path dir ){ if(dir.is_dir()) _wchdir(dir); }

	// system/global path attributes
	struct system { static inline path temp(); static inline path system_dir(){static path s;if(!s.empty())return s;GetSystemDirectoryW(s,path::capacity);return s=s.add_backslash();} };
	struct global { static inline path temp(); };

	// utilities
	static path temp( const wchar_t* subkey=L"" );
	static path serial( path dir, const wchar_t* prefix, const wchar_t* postfix, int numzero=4 );
	inline path key() const { if(!*data)return path();path d;size_t n=0;for(size_t k=0,kn=length();k<kn;k++){wchar_t c=data[k];if(c!=L':'&&c!=L' ') d[n++]=(c==L'\\'||c==L'/')?L'.':(::tolower(c));}if(d[n-1]==L'.')n--;d[n]=0;return d; }
	inline path tolower() const { path d;size_t l=length();for(size_t k=0;k<l;k++)d[k]=::tolower(data[k]);d[l]=L'\0'; return d; }
	inline path toupper() const { path d;size_t l=length();for(size_t k=0;k<l;k++)d[k]=::toupper(data[k]);d[l]=L'\0'; return d; }

	// scan/findfile: ext_filter (specific extensions delimited by semicolons), str_filter (path should contain this string)
	std::vector<path> scan( bool recursive=true, const wchar_t* ext_filter=nullptr, const wchar_t* str_filter=nullptr ) const;
	std::vector<path> subdirs( bool recursive=true, const wchar_t* str_filter=nullptr ) const;

protected:

	__forceinline attrib_t& cache() const { return *((attrib_t*)(data+capacity)); }
	__forceinline bool		cache_exists() const { attrib_t* c=(attrib_t*)(data+capacity); return c->ftLastWriteTime.dwHighDateTime>0&&c->dwFileAttributes==INVALID_FILE_ATTRIBUTES; }
	void canonicalize(); // remove redundant dir indicates such as "..", "."

	struct scan_t { std::vector<path> result; bool recursive; bool has_ext; std::vector<std::wstring> exts; const wchar_t* str; size_t slen; };
	void scan_recursive( path& dir, scan_t& si ) const;
	void subdirs_recursive( path& dir, scan_t& si ) const;

	wchar_t* data;	// for casting without (const wchar_t*)
};

//***********************************************
// definitions of long inline member functions
__noinline inline std::vector<path> path::scan( bool recursive, const wchar_t* ext_filter, const wchar_t* str_filter ) const
{
	std::vector<std::wstring> exts; if(ext_filter&&ext_filter[0]){ wchar_t ef[4096]={0}, *ctx=nullptr; wcscpy(ef,ext_filter); for(wchar_t* e=wcstok_s(ef,L";",&ctx);e;e=wcstok_s(nullptr,L";",&ctx)) if(e[0]) exts.push_back(std::wstring(L".")+e); }
	scan_t si={{},recursive,!exts.empty(),exts,str_filter,str_filter?wcslen(str_filter):0};
	if(!is_dir()) return si.result; path src=(is_relative()?absolute(L".\\"):*this).add_backslash();
	si.result.reserve(1<<16);scan_recursive(src,si);si.result.shrink_to_fit();return si.result;
}

__noinline inline std::vector<path> path::subdirs( bool recursive, const wchar_t* str_filter ) const
{
	scan_t si={{},recursive,false,{},str_filter,str_filter?wcslen(str_filter):0};
	if(!is_dir()) return si.result; path src=(is_relative()?absolute(L".\\"):*this).add_backslash();
	si.result.reserve(1<<12);subdirs_recursive(src,si);si.result.shrink_to_fit();return si.result;
}

__noinline inline void path::scan_recursive( path& dir, path::scan_t& si ) const
{
	WIN32_FIND_DATAW fd; HANDLE h=FindFirstFileExW(dir+L"*.*",FindExInfoBasic/*minimal(faster)*/,&fd,FindExSearchNameMatch,0,FIND_FIRST_EX_LARGE_FETCH); if(h==INVALID_HANDLE_VALUE) return;
	path t;
	size_t dl=wcslen(dir); memcpy(t.data,dir.data,dl*sizeof(wchar_t)); wchar_t *f=fd.cFileName, *p=t.data+dl; 
	std::vector<path> sdir; if(si.recursive) sdir.reserve(16);

	while(FindNextFileW(h,&fd))
	{
		size_t fl=wcslen(f);
		if((fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)==0)
		{
			if((si.has_ext&&!__wcsiext(f,&si.exts,fl))||(si.slen>0&&!__wcsistr(f,fl,si.str,si.slen))) continue;
			memcpy(p,f,sizeof(wchar_t)*fl); p[fl]=0; t.clear_cache(); si.result.emplace_back(t);
		}
		else if(si.recursive&&(fd.dwFileAttributes&FILE_ATTRIBUTE_HIDDEN)==0)
		{
			if(f[0]==L'.'){ if(!f[1]||memcmp(f+1,L".",4)==0||memcmp(f+1,L"git",8)==0) continue; } // skip .git
			memcpy(p,f,sizeof(wchar_t)*fl); p[fl]=L'\\';p[fl+1]=0; sdir.emplace_back(t);
		}
	}
	FindClose(h);
	for(auto& c:sdir) scan_recursive(c,si);
}

__noinline inline void path::subdirs_recursive( path& dir, path::scan_t& si ) const
{
	WIN32_FIND_DATAW fd; HANDLE h=FindFirstFileExW(dir+L"*.*",FindExInfoBasic/*minimal(faster)*/,&fd,FindExSearchNameMatch,0,FIND_FIRST_EX_LARGE_FETCH); if(h==INVALID_HANDLE_VALUE) return;
	path t; size_t dl=wcslen(dir); memcpy(t.data,dir.data,dl*sizeof(wchar_t)); wchar_t *f=fd.cFileName, *p=t.data+dl; 
	std::vector<path> sdir; if(si.recursive) sdir.reserve(16);

	while(FindNextFileW(h,&fd))
	{
		if((fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)==0) continue;
		if((fd.dwFileAttributes&FILE_ATTRIBUTE_HIDDEN)!=0) continue;
		if(f[0]==L'.'){ if(!f[1]||memcmp(f+1,L".",4)==0||memcmp(f+1,L"git",8)==0) continue; } // skip .git
		size_t fl=wcslen(f); memcpy(p,f,sizeof(wchar_t)*fl); p[fl]=L'\\';p[fl+1]=0;
		if(si.recursive) sdir.emplace_back(t);
		if(si.slen==0) si.result.emplace_back(t); else if(__wcsistr(f,fl,si.str,si.slen)) si.result.emplace_back(t);
	}
	FindClose(h);
	for(auto& c:sdir) subdirs_recursive(c,si);
}

__noinline inline path path::serial( path dir, const wchar_t* prefix, const wchar_t* postfix, int numzero )
{
	dir=dir.add_backslash(); if(!dir.exists()) dir.mkdir();
	int nMaxFiles=1; for(int k=0;k<numzero;k++) nMaxFiles*=10;
	path fmt,p; swprintf((wchar_t*)fmt.c_str(),path::capacity,L"%s%s%%0%dd%s%s",dir.c_str(),prefix,numzero,postfix&&postfix[0]?L".":L"",postfix);
	for(int k=0;k<nMaxFiles;k++){swprintf(p,path::capacity,fmt,k);if(!p.exists()) return p;}
	swprintf(p,path::capacity,fmt,nMaxFiles-1);
	return p;
}

//***********************************************
// temp directories

inline path path::system::temp(){ static path t; if(!t.empty()) return t; GetTempPathW(path::capacity,t);t=t.absolute().add_backslash();t[0]=::toupper(t[0]); return t; }
#ifdef REX_IMPL
inline path path::global::temp(){ static path t = path::system::temp()+L".rex\\"; return t; }
#else
inline path path::global::temp(){ static path t = path::system::temp()+L".gxut\\"; return t; }
#endif

//***********************************************
__noinline inline path path::temp( const wchar_t* subkey )
{
	static path g=global::temp(),mod=path(L"local\\")+module_dir().key().add_backslash();
	path key = (subkey&&subkey[0])?path(subkey).key().add_backslash():mod;
	path t = key.empty()?g:g+key; if(!t.exists()) t.mkdir();
	return t;
}

__noinline inline path path::relative( const wchar_t* from ) const
{
	if(is_relative()) return *this;

	path from_dir = (!from||!from[0]) ? module_dir() : path(from).dir().absolute();
	if(::tolower(from_dir[0])!=::tolower(this->data[0])) return *this; // different drive

	// 1. keep filename, make the list of reference and current path
	std::vector<path> src_list=from_dir.explode(), dst_list=dir().explode();

	// 2. compare and count the different directory levels
	path result;
	size_t s=0,d=0,ss=src_list.size(),ds=dst_list.size();
	for(; s<ss&&d<ds; s++, d++ ) if(_wcsicmp(src_list[s],dst_list[d])!=0) break;
	for(; s<ss; s++ ) result += L"..\\";
	for(; d<ds; d++ ) result += dst_list[d]+L"\\";
	result.canonicalize();

	// 3. if empty dir, then attach ./
	if(result[0]==0||result[0]!=L'.') result=path(".\\")+result;
	return this->is_dir()?result:result+name();
}

__noinline inline void path::create_process( const wchar_t* arguments, bool bShowWindow, bool bWaitFinish ) const
{
	wchar_t cmd[4096]; swprintf_s( cmd, 4096, L"\"%s\" %s", data, arguments?arguments:L""  );
	PROCESS_INFORMATION pi={0}; STARTUPINFOW si={0}; si.cb=sizeof(si); si.dwFlags=STARTF_USESHOWWINDOW; si.wShowWindow=bShowWindow?SW_SHOW:SW_HIDE;
	CreateProcessW( nullptr, (LPWSTR)cmd, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi );
	if(bWaitFinish) WaitForSingleObject(pi.hProcess,INFINITE);
	CloseHandle( pi.hThread );    // must release handle
	CloseHandle( pi.hProcess );   // must release handle
}

__noinline inline void path::canonicalize()
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
		else if(t[0]!=L'.'||t[1]!=0) L.emplace_back(wcscpy(__wcsbuf(),t));
	}

	// reconstruct the path
	wchar_t* d=data;
	if(!bSingleDotBegin&&L.empty()){ d[0]=L'\0'; return; }
	if(L.empty()||(bSingleDotBegin&&(!L.empty()&&L.front()[0]!=L'.'))){ (d++)[0]=L'.'; (d++)[0]=L'\\'; d[0]=L'\0'; } // make it begin with single-dot again
	for(size_t k=0,kn=L.size();k<kn;k++){ wchar_t* t=L[k]; size_t l=wcslen(t); memcpy(d,t,l*sizeof(wchar_t)); d+=l; (d++)[0]=L'\\'; d[0]=L'\0'; }
	if(!bTrailingBackslash&&d>data)(d-1)[0]=L'\0';
}

//***********************************************
// nocase/std map/unordered_map extension for path
namespace std
{
	template <> struct hash<path>{ size_t operator()(const path& p)const{ return std::hash<std::wstring>()(p.tolower().c_str());}};
}

namespace nocase
{
	template <> struct less<path>{ bool operator()(const path& a,const path& b)const{return _wcsicmp(a.c_str(),b.c_str())<0;}};
	template <> struct equal_to<path>{ bool operator()(const path& a,const path& b)const{return _wcsicmp(a.c_str(),b.c_str())==0;}};
	template <> struct hash<path>{ size_t operator()(const path& p)const{ return std::hash<std::wstring>()(p.tolower().c_str());}};
}

//***********************************************
// operating-system utilities
inline BOOL CALLBACK enum_windows_proc( HWND hwnd , LPARAM pProcessList ){((std::vector<HWND>*)pProcessList)->emplace_back(hwnd);return TRUE;}
inline std::vector<HWND> enum_windows( const wchar_t* filter=nullptr ){std::vector<HWND> v;EnumWindows(enum_windows_proc,(LPARAM)(&v));return v;}
inline void exit( const char* fmt, ... ){ va_list a; va_start(a,fmt); std::vector<char> buff(_vscprintf(fmt,a)+1); vsprintf_s(&buff[0],buff.size(),fmt,a); va_end(a); fprintf( stdout, "[%s] %s", path::module().name(false).wtoa(), &buff[0] ); exit(EXIT_FAILURE); }
inline void exit( const wchar_t* fmt, ... ){ va_list a; va_start(a,fmt); std::vector<wchar_t> buff(_vscwprintf(fmt,a)+1); vswprintf_s(&buff[0],buff.size(),fmt,a); va_end(a); fwprintf( stdout, L"[%s] %s", path::module().name(false).c_str(), &buff[0] ); exit(EXIT_FAILURE); }
inline const char* get_last_error(){ static char buff[4096]={0};DWORD e=GetLastError();char *s;FormatMessageA( FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_IGNORE_INSERTS,nullptr,e,MAKELANGID(LANG_ENGLISH,SUBLANG_DEFAULT),(LPSTR)&s,0,nullptr);sprintf(buff,"%s (code=%x)",s,uint(e));LocalFree(s);return buff; }
#if !defined(__GNUC__)&&((__cplusplus>199711L)||(_MSC_VER>=1600/*VS2010*/))
inline void usleep( int us ){ std::this_thread::sleep_for(std::chrono::microseconds(us)); }
#endif

//***********************************************
// compiler utility
namespace gx { namespace compiler
{
	inline int mtoi( const char* month ){ if(!month||!month[0]||!month[1]||!month[2]) return 0; char a=tolower(month[0]), b=tolower(month[1]), c=tolower(month[2]); if(a=='j'){ if(b=='a') return 1; if(c=='n') return 6; return 7; } if(a=='f') return 2; if(a=='m'){ if(c=='r') return 3; return 5; } if(a=='a'){ if(b=='p') return 4; return 8; } if(a=='s') return 9; if(a=='o') return 10; if(a=='n') return 11; return 12; }
	inline int year(){ static int y=0; if(y) return y; char buff[64]; sscanf(__DATE__,"%*s %*s %s", buff); return y=atoi(buff); }
	inline int month(){ static int m=0; if(m) return m; char buff[64]={0}; sscanf(__DATE__,"%s", buff); return m=mtoi(buff); }
	inline int day(){ static int d=0; if(d) return d; char buff[64]; sscanf(__DATE__,"%*s %s %*s", buff); return d=atoi(buff); }
}}

//***********************************************
// timer
#ifndef __GX_TIMER__
#define __GX_TIMER__
namespace gx { struct timer_t
{
	union { double2 result; struct { double x, y; }; };
	inline timer_t(){ begin(); }
	inline void clear(){ x=y=now(); }
	inline void begin(){ x=now(); }
	inline double end(){ return (y=now())-x; }
	inline double delta(){ return y-x; }
	static double now(){ static double c=0; if(c==0){ int64_t f;QueryPerformanceFrequency((LARGE_INTEGER*)&f);c=1000.0/double(f);} static int64_t e=0; if(e==0){auto* ef=(int64_t(*)()) GetProcAddress(GetModuleHandleW(nullptr),"rex_timer_epoch");if(ef)e=ef();else QueryPerformanceCounter((LARGE_INTEGER*)&e);} int64_t i; QueryPerformanceCounter((LARGE_INTEGER*)&i); return double(i-e)*c; } // if rex found, use its epoch; otherwise, use a local epoch
	static timer_t* singleton(){ static timer_t i; return &i; }
};}
#endif

//***********************************************
// general dynamic linking wrapper with DLL
struct dll_t
{
	HMODULE hdll = nullptr;

	~dll_t(){ release(); }
	void release(){ if(hdll){ FreeLibrary(hdll); hdll=nullptr; }  }
	path file_path(){ path f; if(hdll) GetModuleFileNameW(hdll,f,path::capacity); return f; }
	bool load( const wchar_t* dll_path ){ return nullptr!=(hdll=LoadLibraryW(dll_path)); }
	template <class T> T get_proc_address( const char* name ) const { return hdll==nullptr?nullptr:(T)GetProcAddress(hdll,name); }
	template <class T> T* get_proc_address( const char* name, T*& p ) const { return hdll==nullptr?p=nullptr:p=(T*)GetProcAddress(hdll,name); }
	operator bool() const { return hdll!=nullptr; }
};

//***********************************************
#endif // __GX_FILESYSTEM__