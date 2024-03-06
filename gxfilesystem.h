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
#ifndef __GX_FILESYSTEM_H__
#define __GX_FILESYSTEM_H__

#include "gxtype.h"
#if defined(__has_include)&&__has_include("gxstring.h")
	#include "gxstring.h"
#endif

#include <direct.h>		// directory control
#include <io.h>			// low-level io functions
#include <time.h>
#include <deque>
#if __has_include(<shellapi.h>)
	#include <shellapi.h>
#endif

//***********************************************
// Win32-like filetime utilities
inline int64_t FileTimeOffset( int days, int hours=0, int mins=0, int secs=0, int mss=0 ){ return 10000ll*(mss+1000ll*secs+60ll*1000*mins+60ll*60*1000*hours+24ll*60*60*1000*days); } // FILETIME in 100 ns scale
inline FILETIME DiscardFileTimeMilliseconds( FILETIME f ){ uint64_t u=((uint64_t(f.dwHighDateTime)<<32|uint64_t(f.dwLowDateTime))/10000000)*10000000;return FILETIME{DWORD(u&0xffffffff),DWORD(u>>32)}; } // 1ms = 10000 in FILETIME
inline SYSTEMTIME FileTimeToSystemTime( FILETIME f ){ SYSTEMTIME s; FileTimeToSystemTime(&f,&s); return s; }
inline FILETIME SystemTimeToFileTime( const SYSTEMTIME& s ){ FILETIME f; SystemTimeToFileTime(&s,&f); return f; }
inline uint64_t FileTimeToUint64( FILETIME f ){ return (uint64_t(f.dwHighDateTime)<<32|uint64_t(f.dwLowDateTime)); }
inline FILETIME	Uint64ToFileTime( uint64_t u ){ FILETIME f={}; f.dwHighDateTime=DWORD(u>>32); f.dwLowDateTime=u&0xffffffff; return f; }
inline uint64_t SystemTimeToUint64( const SYSTEMTIME& s ){ FILETIME f; SystemTimeToFileTime( &s, &f ); return FileTimeToUint64(f); }
inline FILETIME now(){ FILETIME f; GetSystemTimeAsFileTime(&f); return f; } // current time
inline bool operator==( FILETIME f1, FILETIME f2 ){ return CompareFileTime(&f1,&f2)==0; }
inline bool operator!=( FILETIME f1, FILETIME f2 ){ return CompareFileTime(&f1,&f2)!=0; }
inline bool operator>=( FILETIME f1, FILETIME f2 ){ return CompareFileTime(&f1,&f2)>=0; }
inline bool operator<=( FILETIME f1, FILETIME f2 ){ return CompareFileTime(&f1,&f2)<=0; }
inline bool operator>(  FILETIME f1, FILETIME f2 ){ return CompareFileTime(&f1,&f2)>0; }
inline bool operator<(  FILETIME f1, FILETIME f2 ){ return CompareFileTime(&f1,&f2)<0; }
static const uint64_t DefaultFileTimeOffset = FileTimeOffset(0,0,0,30); // server-local difference can be up to several seconds
inline bool FileTimeGreater( FILETIME f1, FILETIME f2, int64_t offset=DefaultFileTimeOffset ){ return FileTimeToUint64(f1)>FileTimeToUint64(f2)+offset; } // do not make FileTimeLess(), which causes confusion in use cases

//***********************************************
// posix-like or std::filesystem-like utilities
inline bool is_fifo( FILE* fp ){ if(!fp) return false; struct stat s; return fstat(_fileno(fp),&s)==0?(s.st_mode&_S_IFIFO?true:false):false; }

//***********************************************
// common constants
static const int GX_MAX_PATH = 1024;	// MAX_PATH == 260

// disk volume type
struct volume_t
{
	static const int	capacity = GX_MAX_PATH; // MAX_PATH == 260
	wchar_t				root[4]={}; // trailing backslash required
	wchar_t				name[capacity+1]={};
	unsigned long		serial_number=0;
	unsigned long		maximum_component_length=0;
	uint64_t			_disk_size=0;
	struct { unsigned long flags=0; wchar_t name[capacity+1]={}; } filesystem;

	// constructor
	volume_t() = default;
	volume_t( const volume_t& other ) = default;
	volume_t& operator=( const volume_t& other ) = default;
	volume_t( const wchar_t* drive )
	{
		if(!drive||!drive[0]||!isalpha(drive[0])) return;
		root[0]=drive[0]; root[1]=L':'; root[2]=L'\\'; root[3]=0;
		if(!GetVolumeInformationW(root,name,capacity,&serial_number,&maximum_component_length,&filesystem.flags,filesystem.name,capacity)){ root[0]=0; return; }
		ULARGE_INTEGER a,t,f; GetDiskFreeSpaceExW( root, &a, &t, &f);
		_disk_size = uint64_t(t.QuadPart);
	}

	// query
	bool exists() const { return root[0]!=0&&serial_number!=0&&filesystem.name[0]!=0; }
	uint64_t size() const { return exists()?_disk_size:0; }
	uint64_t free_space() const { ULARGE_INTEGER a,t,f; GetDiskFreeSpaceExW( root, &a, &t, &f); return uint64_t(a.QuadPart); }
	bool has_free_space( uint64_t inverse_thresh=10 ) const { return exists()&&free_space()>(size()/inverse_thresh); }
	bool is_exfat() const { return _wcsicmp(filesystem.name,L"exFAT")==0; }
	bool is_ntfs() const { return _wcsicmp(filesystem.name,L"NTFS")==0; }
	bool is_fat32() const { return _wcsicmp(filesystem.name,L"FAT32")==0; }
};

struct path
{
	using value_type  = wchar_t;
	using string_type = std::wstring;
	using attrib_t = WIN32_FILE_ATTRIBUTE_DATA;	// auxiliary cache information from scan()
	typedef struct _stat64 stat_t;				// use "struct _stat" instead of "_stat" for C-compatibility

	static constexpr wchar_t preferred_separator = L'\\';
	static constexpr int max_buffers	= 4096;
	static constexpr int capacity		= GX_MAX_PATH; // MAX_PATH == 260

	// utility string functions
	static __forceinline wchar_t*		__wcsbuf(){ return ::_wcsbuf(capacity); }
	static __forceinline char*			__strbuf(){ return (char*)::_wcsbuf(capacity); }
	static __forceinline wchar_t*		__atow( const char* _Src, wchar_t* _Dst, uint cp=0 ){ int l=MultiByteToWideChar(cp,0,_Src,-1,0,0);if(l>capacity-1)l=capacity-1; MultiByteToWideChar(cp,0,_Src,-1,_Dst,l); return _Dst; }

	// destructor/constuctors
	__forceinline wchar_t* alloc(){ static constexpr size_t s=sizeof(wchar_t)*capacity+sizeof(attrib_t); _data=(wchar_t*)malloc(s); if(_data)_data[0]=0; return _data; }
	path() noexcept { alloc(); clear_cache(); }
	path( const path& p ) noexcept { wcscpy(alloc(),p); cache()=p.cache(); } // do not canonicalize for copy constructor
	path( path&& p ) noexcept { _data=p._data; p._data=nullptr; } // cache moves as well
	path( const wchar_t* s ) noexcept : path() { wcscpy(_data,s); }
	path( const wchar_t* s, size_t length ) noexcept : path() { memcpy(_data,s,length*sizeof(wchar_t)); _data[length]=0; }
	path( const char* s ) noexcept : path() { __atow(s,_data); }
	explicit path( const string_type& s ) noexcept : path() { wcscpy(_data,s.c_str()); }
	explicit path( const std::string& s ) noexcept : path() { __atow(s.c_str(),_data); }
	~path() noexcept { if(_data) free(_data); }

	// operator overloading: assignment
	path& operator=( const path& p ) noexcept { wcscpy(_data,p._data); cache()=p.cache(); return *this; }
	path& operator=( path&& p ) noexcept { if(_data) free(_data); _data=p._data; p._data=nullptr; return *this; }
	path& operator=( const wchar_t* s ) noexcept { wcscpy(_data,s); return *this; }
	path& operator=( const char* s ) noexcept { __atow(s,_data); return *this; }
	path& operator=( const string_type& s ) noexcept { wcscpy(_data,s.c_str()); return *this; }
	path& operator=( const std::string& s ) noexcept { __atow(s.c_str(),_data); return *this; }

	// operator overloading: concatenations
	path& operator+=( const path& p ){ wcscat(_data,p._data+((p._data[0]==L'.'&&p._data[1]==L'\\'&&p._data[2])?2:0)); return *this; }
	path& operator+=( const wchar_t* s ){ if(s[0]==L'.'&&s[1]==L'\\'&&s[2]) s+=2; size_t l=wcslen(_data); wcscpy(_data+l,s); return *this; }
	path& operator+=( const char* s ){ size_t l=wcslen(_data); __atow(s,_data+l); return *this; }
	path& operator+=( wchar_t c ){ size_t l=wcslen(_data); _data[l]=c; _data[l+1]=0; return *this; }
	path& operator+=( char c ){ size_t l=wcslen(_data); _data[l]=wchar_t(c); _data[l+1]=0; return *this; }
	path& operator+=( const string_type& s ){ return operator+=(s.c_str()); }
	path& operator+=( const std::string& s ){ return operator+=(s.c_str()); }

	path& operator/=( const path& p ){ return *this=add_backslash()+p; }
	path& operator/=( const wchar_t* s ){ return *this=add_backslash()+s; }
	path& operator/=( const char* s ){ return *this=add_backslash()+s; }
	path& operator/=( const string_type& s ){ return operator/=(s.c_str()); }
	path& operator/=( const std::string& s ){ return operator/=(s.c_str()); }

	path operator+( const path& p ) const { return clone().operator+=(p); }
	path operator+( const wchar_t* s ) const { return clone().operator+=(s); }
	path operator+( const char* s ) const { return clone().operator+=(s); }
	path operator+( wchar_t c ){ return clone().operator+=(c); }
	path operator+( char c ){ return clone().operator+=(c); }
	path operator+( const string_type& s ) const { return clone().operator+=(s.c_str()); }
	path operator+( const std::string& s ) const { return clone().operator+=(s.c_str()); }

	path operator/( const path& p ) const { return clone().operator/=(p); }
	path operator/( const wchar_t* s ) const { return clone().operator/=(s); }
	path operator/( const char* s ) const { return clone().operator/=(s); }
	path operator/( const string_type& s ) const { return clone().operator/=(s.c_str()); }
	path operator/( const std::string& s ) const { return clone().operator/=(s.c_str()); }

	// operator overloading: comparisons
	bool operator==( const path& p )	const { return _wcsicmp(_data,p._data)==0; }
	bool operator==( const wchar_t* s )	const { return _wcsicmp(_data,s)==0; }
	bool operator!=( const path& p )	const { return _wcsicmp(_data,p._data)!=0; }
	bool operator!=( const wchar_t* s )	const { return _wcsicmp(_data,s)!=0; }

#ifdef _INC_SHLWAPI
	bool operator<( const path& p )		const { return StrCmpLogicalW(_data,p._data)<0; }
	bool operator>( const path& p )		const { return StrCmpLogicalW(_data,p._data)>0; }
	bool operator<=( const path& p )	const { return StrCmpLogicalW(_data,p._data)<=0; }
	bool operator>=( const path& p )	const { return StrCmpLogicalW(_data,p._data)>=0; }
#else
	bool operator<( const path& p )		const { return ::_strcmplogical(_data,p._data)<0; }
	bool operator>( const path& p )		const { return ::_strcmplogical(_data,p._data)>0; }
	bool operator<=( const path& p )	const { return ::_strcmplogical(_data,p._data)<=0; }
	bool operator>=( const path& p )	const { return ::_strcmplogical(_data,p._data)>=0; }
#endif

	// operator overloading: array operator
	inline wchar_t& operator[]( ptrdiff_t i ){ return _data[i]; }
	inline const wchar_t& operator[]( ptrdiff_t i ) const { return _data[i]; }

	// operator overloading: casting and conversion
	wchar_t* data(){ return _data; }
	operator wchar_t*(){ return _data; }
	operator const wchar_t*() const { return _data; }
	const wchar_t* c_str() const { return _data; }
	const char* wtoa( uint cp=0 ) const { return ::wtoa(_data,cp); }
	std::wstring str() const { return std::wstring(_data); }

	// iterators
	using iterator = wchar_t*;
	using const_iterator = const wchar_t*;
	iterator begin() const { return _data; }
	iterator end() const { return _data+wcslen(_data); }
	wchar_t& front() const { return _data[0]; }
	wchar_t& back() const { return _data[0]==0?_data[0]:_data[wcslen(_data)-1]; }

	// clear and trivial clone
	void clear(){ _data[0]=0; clear_cache(); }
	path clone() const { return path(*this); }

	// size/length
	inline bool empty()			const { return _data[0]==0; }
	inline size_t size()		const { return wcslen(_data); }
	inline size_t length()		const { return wcslen(_data); }
	inline uint64_t file_size() const { if(!cache_exists()) update_cache(); auto& c=cache(); if(c.dwFileAttributes==INVALID_FILE_ATTRIBUTES) return 0; return uint64_t(c.nFileSizeHigh)<<32ull|uint64_t(c.nFileSizeLow); }

	// crc32c/md5 checksums of the file content
	inline uint crc32c() const;	// implemented in gxmemory.h
	inline uint4 md5() const;	// implemented in gxmemory.h

	// in-place transforms
	void make_preferred()	const {	wchar_t* t=_data; for(size_t k=0,l=wcslen(t);k<l;k++,t++) if(*t==L'/') *t=L'\\'; }

	// system-related: slash, backslash, unix, quote
	path to_preferred()		const {	path p(*this); wchar_t* t=p._data; for(size_t k=0,l=wcslen(t);k<l;k++,t++) if(*t==L'/') *t=L'\\'; return p; }
	path to_backslash()		const {	path p(*this); wchar_t* t=p._data; for(size_t k=0,l=wcslen(t);k<l;k++,t++) if(*t==L'/') *t=L'\\'; return p; }
	path to_slash()			const {	path p(*this); wchar_t* t=p._data; for(size_t k=0,l=wcslen(t);k<l;k++,t++) if(*t==L'\\') *t=L'/'; return p; }
	path to_dot()			const {	path p(*this); wchar_t* t=p._data; for(size_t k=0,l=wcslen(t);k<l;k++,t++) if(*t==L'\\'||*t==L'/') *t=L'.'; return p; }
	path add_backslash()	const { path p(*this); size_t l=wcslen(p._data); if(l&&p._data[l-1]=='/') p._data[l-1]='\\'; else if(l&&p._data[l-1]!='\\'){p._data[l]='\\';p._data[l+1]=L'\0';} return p; }
	path add_slash()		const { path p(*this); size_t l=wcslen(p._data); if(l&&p._data[l-1]=='\\') p._data[l-1]='/'; else if(l&&p._data[l-1]!='/'){p._data[l]='/';p._data[l+1]=L'\0';} return p; }
	path remove_backslash()	const { path p(*this); size_t l=wcslen(p._data); if(l&&(p._data[l-1]=='\\'||p._data[l-1]=='/')) p._data[l-1]=L'\0'; return p; }
	path remove_slash()		const { path p(*this); size_t l=wcslen(p._data); if(l&&(p._data[l-1]=='\\'||p._data[l-1]=='/')) p._data[l-1]=L'\0'; return p; }
	path auto_quote()		const { if(!*_data||(_data[0]==L'\"'&&_data[wcslen(_data)-1]==L'\"')) return *this; auto* t=__wcsbuf(); size_t l=wcslen(_data); memcpy(t,_data,l*sizeof(wchar_t)); if(t[l]==L' '||t[l]==L'\t'||t[l]==L'\n') t[l]=0; if(t[0]==L' '||t[0]==L'\t'||t[0]==L'\n') t++; if(t[0]!=L'-'&&!wcschr(t,L' ')&&!wcschr(t,L'\t')&&!wcschr(t,L'\n')&&!wcschr(t,L'|')&&!wcschr(t,L'&')&&!wcschr(t,L'<')&&!wcschr(t,L'>')) return *this; path p; p[0]=L'\"'; memcpy(p._data+1,_data,l*sizeof(wchar_t)); p[l+1]=L'\"'; p[l+2]=0; return p; }
	path unix()				const {	path p(*this); p.canonicalize(); p=p.to_slash(); size_t len=p.length(); if(len<2||p.is_relative()||p.is_unc()||p.is_rsync()) return p; if(p._data[1]==L':'){ p._data[1]=wchar_t(::tolower(p._data[0])); p._data[0]=L'/'; } return p; }
	path cygwin()			const { path p(*this); p.canonicalize(); p=p.to_slash(); size_t len=p.length(); if(len<2||p.is_relative()||p.is_unc()||p.is_rsync()) return p; path p2; swprintf_s( p2, capacity, L"/cygdrive/%c%s", ::tolower(p[0]), p._data+2 ); return p2; }

	// split path
	struct split_t { wchar_t *drive, *dir, *fname, *ext; };
	__forceinline split_t split( wchar_t* drive=nullptr, wchar_t* dir=nullptr, wchar_t* fname=nullptr, wchar_t* ext=nullptr ) const { _wsplitpath_s(_data,drive,drive?_MAX_DRIVE:0,dir,dir?_MAX_DIR:0,fname,fname?_MAX_FNAME:0,ext,ext?_MAX_EXT:0); if(drive&&drive[0]) drive[0]=wchar_t(::toupper(drive[0])); return split_t{drive,dir,fname,ext}; }

	// multi-byte path info
	const char* aname( bool with_ext=true ) const { return name(with_ext).wtoa(); }
	const wchar_t* wname( bool with_ext=true ) const { return name(with_ext).c_str(); }

	// attribute by stats
	inline stat_t	stat() const { stat_t s={}; if(exists()) _wstat64(_data,&s); return s; }
	inline DWORD&	attributes() const { if(!cache_exists()) update_cache(); return cache().dwFileAttributes; }
	inline void		update_cache() const { auto& c=cache(); if(!GetFileAttributesExW(_data,GetFileExInfoStandard,&c)||c.dwFileAttributes==INVALID_FILE_ATTRIBUTES){ memset(&c,0,sizeof(attrib_t)); c.dwFileAttributes=INVALID_FILE_ATTRIBUTES; return; } c.ftLastWriteTime=DiscardFileTimeMilliseconds(c.ftLastWriteTime); }
	inline void		clear_cache() const { attrib_t* a=(attrib_t*)(_data+capacity); memset(a,0,sizeof(attrib_t)); a->dwFileAttributes=INVALID_FILE_ATTRIBUTES; }

	// set attributes
	void set_hidden( bool h ) const {	if(!*_data) return; auto& a=attributes(); if(a!=INVALID_FILE_ATTRIBUTES) SetFileAttributesW(_data,a=h?(a|FILE_ATTRIBUTE_HIDDEN):(a^FILE_ATTRIBUTE_HIDDEN)); }
	void set_readonly( bool r ) const {	if(!*_data) return; auto& a=attributes(); if(a!=INVALID_FILE_ATTRIBUTES) SetFileAttributesW(_data,a=r?(a|FILE_ATTRIBUTE_READONLY):(a^FILE_ATTRIBUTE_READONLY)); }
	void set_system( bool s ) const {	if(!*_data) return; auto& a=attributes(); if(a!=INVALID_FILE_ATTRIBUTES) SetFileAttributesW(_data,a=s?(a|FILE_ATTRIBUTE_SYSTEM):(a^FILE_ATTRIBUTE_SYSTEM)); }

	// get attributes
	bool exists() const {		if(!*_data) return false; auto& a=attributes(); return a!=INVALID_FILE_ATTRIBUTES; }
	bool is_dir() const {		if(!*_data) return false; auto& a=attributes(); return a!=INVALID_FILE_ATTRIBUTES&&(a&FILE_ATTRIBUTE_DIRECTORY)!=0; }
	bool is_drive() const {		if(!*_data) return false; size_t l=length(); return is_dir()&&l<4&&l>1&&_data[1]==L':'; }
	bool is_hidden() const {	if(!*_data) return false; auto& a=attributes(); return a!=INVALID_FILE_ATTRIBUTES&&(a&FILE_ATTRIBUTE_HIDDEN)!=0; }
	bool is_readonly() const {	if(!*_data) return false; auto& a=attributes(); return a!=INVALID_FILE_ATTRIBUTES&&(a&FILE_ATTRIBUTE_READONLY)!=0; }
	bool is_system() const {	if(!*_data) return false; auto& a=attributes(); return a!=INVALID_FILE_ATTRIBUTES&&(a&FILE_ATTRIBUTE_SYSTEM)!=0; }
	bool is_ssh() const {		if(!*_data) return false; return wcsstr(_data+2,L":\\")!=nullptr||wcsstr(_data+2,L":/")!=nullptr; }
	bool is_synology() const {	if(!*_data) return false; return _wcsistr(_data,L":\\volume")!=nullptr||_wcsistr(_data,L":/volume")!=nullptr; }
	bool is_pipe() const {		if(!*_data) return false; return wcscmp(_data,L"-")==0||_wcsnicmp(_data,L"pipe:",5)==0; }
	bool is_http_url() const {	if(!*_data) return false; return _wcsnicmp(_data,L"http://",7)==0||_wcsnicmp(_data,L"https://",8)==0; }
	bool is_junction() const {	if(!*_data||is_drive()) return false;auto& a=attributes(); if(a==INVALID_FILE_ATTRIBUTES||(a&FILE_ATTRIBUTE_DIRECTORY)==0) return false; return ((a&FILE_ATTRIBUTE_REPARSE_POINT)!=0); }

	// path info/operations
	volume_t volume() const { return (is_unc()||is_rsync()||!drive().exists())?volume_t():volume_t(drive().c_str()); }
	path drive() const { if(!*_data) return path();if(is_unc()) return unc_root();path d;_wsplitpath_s(_data,d._data,_MAX_DRIVE,0,0,0,0,0,0);return d; }
	path dir() const { path p; wchar_t* d=__wcsbuf();if(is_unc()){path r=unc_root();size_t rl=r.length();if(length()<=rl+1){if(r._data[rl-1]!=L'\\'){r._data[rl]='\\';r._data[rl+1]=0;}return r;}} _wsplitpath_s(_data,p._data,_MAX_DRIVE,d,_MAX_DIR,0,0,0,0); size_t pl=wcslen(p._data), dl=wcslen(d); if(0==(pl+dl)) return L".\\"; if(dl){ memcpy(p._data+pl,d,dl*sizeof(wchar_t)); p._data[pl+dl]=0; } return p; }
	path unc_root() const { if(!is_unc()) return path(); path r=*this;size_t l=wcslen(_data);for(size_t k=0;k<l;k++)if(r[k]==L'/')r[k]=L'\\'; auto* b=wcschr(r._data+2,L'\\');if(b)b[0]=0; return r; } // similar to drive (but to the root unc path without backslash)
	path name( bool with_ext=true ) const { path p; wchar_t* ext=with_ext?__wcsbuf():nullptr; _wsplitpath_s(_data,0,0,0,0,p._data,_MAX_FNAME,ext,ext?_MAX_EXT:0); if(!ext) return p; size_t pl=wcslen(p._data), el=wcslen(ext); if(el){ memcpy(p._data+pl,ext,el*sizeof(wchar_t)); p._data[pl+el]=0; } return p; }
	path dir_name() const { if(wcschr(_data,L'\\')) return dir().remove_backslash().name(); else if(wcschr(_data,L'/')) return dir().remove_slash().name(); else return L""; }
	path ext() const { wchar_t e[_MAX_EXT+1]; _wsplitpath_s(_data,0,0,0,0,0,0,e,_MAX_EXT); path p; if(*e!=0) wcscpy(p._data,e+1); return p; }
	path parent() const { return dir().remove_backslash().dir(); }
	std::vector<path> ancestors( path root=L"" ) const { if(empty()) return std::vector<path>(); if(root._data[0]==0) root=is_unc()?unc_root():module_dir(); path d=dir(); int l=int(d.size()),rl=int(root.size()); bool r=_wcsnicmp(d._data,root._data,rl)==0; std::vector<path> a;a.reserve(4); for(int k=l-1,e=r?rl-1:0;k>=e;k--){ if(d._data[k]!=L'\\'&&d._data[k]!=L'/') continue; d._data[k+1]=0; a.emplace_back(d); } return a; }
	path junction() const { path t; if(!*_data||!exists()) return t; bool b_dir=is_dir(),j=false; for(auto& d:b_dir?ancestors():dir().ancestors()){ if(d.is_drive()) break; if((d.attributes()&FILE_ATTRIBUTE_REPARSE_POINT)!=0){j=true;break;} } if(!j) return t; HANDLE h=CreateFileW(_data,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_FLAG_BACKUP_SEMANTICS,0); if(h==INVALID_HANDLE_VALUE) return t; GetFinalPathNameByHandleW(h,t._data,t.capacity,FILE_NAME_NORMALIZED); CloseHandle(h); if(wcsncmp(t._data,L"\\\\?\\",4)==0) t=path(t._data+4); return b_dir?t.add_backslash():t; }

	// content manipulations
	path remove_first_dot()	const { return (wcslen(_data)>2&&_data[0]==L'.'&&_data[1]==L'\\') ? path(_data+2) : *this; }
	path remove_extension() const { split_t si=split(__wcsbuf(),__wcsbuf(),__wcsbuf()); return wcscat(wcscat(si.drive,si.dir),si.fname); }
	path replace_extension( const wchar_t* ext ) const { if(!ext||!*ext) return *this; split_t si=split(__wcsbuf(),__wcsbuf(),__wcsbuf(),__wcsbuf());path p;swprintf_s(p._data,capacity,L"%s%s%s%s%s",si.drive,si.dir,si.fname,ext[0]==L'.'?L"":L".",ext );return p; }
	path replace_extension( const char* ext ) const { return (!ext||!*ext)?*this:replace_extension(::atow(ext)); }
	std::vector<path> explode( const wchar_t* delim=L"\\") const { std::vector<path> L; if(!delim||!*delim) return L; path s=delim[0]==L'/'&&delim[1]==0?to_slash():*this; L.reserve(16); wchar_t* ctx=nullptr; for(wchar_t* t=wcstok_s(s._data,delim,&ctx);t;t=wcstok_s(0,delim,&ctx)) L.emplace_back(t); return L; }

	// directory attributes
	bool has_file( const path& file_name ) const { return is_dir()&&operator+(file_name).exists(); }

	// chdir/make/copy/delete file/dir operations
	path chdir() const { path old=cwd(); int r=is_dir()?_wchdir(_data):0; return old; } // return old working directory
	bool mkdir() const; // make all super directories
	bool copy_file( path dst, bool overwrite=true ) const { if(!exists()||is_dir()||dst.empty()) return false; if(dst.is_dir()||dst.back()==L'\\') dst=dst.add_backslash()+name(); dst.dir().mkdir(); if(dst.exists()&&overwrite){ if(dst.is_hidden()) dst.set_hidden(false); if(dst.is_readonly()) dst.set_readonly(false); } return bool(CopyFileW( _data, dst, overwrite?FALSE:TRUE )); }
	bool move_file( path dst ) const { return is_dir()?false:(drive()==dst.drive()&&!dst.exists()) ? MoveFileW(_data,dst.c_str())!=0 : !copy_file(dst,true) ? false: rmfile(); }
#ifndef _INC_SHELLAPI
	bool delete_file() const { if(!exists()||is_dir()) return false; if(is_hidden()) set_hidden(false); if(is_readonly()) set_readonly(false); return DeleteFileW(_data)==TRUE; }
	bool rmfile() const { return delete_file(); }
	bool delete_dir() const { if(!is_dir()) return false; if(is_hidden()) set_hidden(false); if(is_readonly()) set_readonly(false); _wrmdir(_data); return true; }
	bool rmdir() const { return delete_dir(); }
#else
	bool delete_file( bool b_undo=false ) const { if(!exists()||is_dir()) return false; if(is_hidden()) set_hidden(false); if(is_readonly()) set_readonly(false); SHFILEOPSTRUCTW fop={};fop.wFunc=FO_DELETE;fop.fFlags=FOF_FILESONLY|FOF_SILENT|FOF_NOCONFIRMATION|(b_undo?FOF_ALLOWUNDO:0);fop.pFrom=_data;_data[wcslen(_data)+1]=L'\0';return SHFileOperationW(&fop)==0;}
	bool rmfile( bool b_undo=false ) const { return delete_file(b_undo); }
	bool delete_dir( bool b_undo=true ) const { if(!is_dir()) return false; if(is_hidden()) set_hidden(false); if(is_readonly()) set_readonly(false); SHFILEOPSTRUCTW fop={};fop.wFunc=FO_DELETE;fop.fFlags=FOF_SILENT|FOF_NOCONFIRMATION|(b_undo?FOF_ALLOWUNDO:0); fop.pFrom=_data;_data[wcslen(_data)+1]=L'\0';return SHFileOperationW(&fop)==0;}
	bool rmdir( bool b_undo=true ) const { return delete_dir(b_undo); }
	bool copy_dir( path dst, bool overwrite=true ) const { if(!is_dir()) return false;wchar_t* from=__wcsbuf();swprintf_s(from,capacity,L"%s\\*\0",_data);dst[dst.size()+1]=L'\0'; SHFILEOPSTRUCTW fop={};fop.wFunc=FO_COPY;fop.fFlags=FOF_ALLOWUNDO|FOF_SILENT|FOF_NOCONFIRMATION;fop.pFrom=from;fop.pTo=dst;return SHFileOperationW(&fop)==0; }
	bool move_dir( path dst ) const { return !is_dir()?false:(drive()==dst.drive()&&!dst.exists()) ? MoveFileW(_data,dst.c_str())!=0 : !copy_dir(dst,true) ? false: rmdir(); }
	void open( const wchar_t* args=nullptr, bool b_show_window=true ) const { path cmd;swprintf(cmd,capacity,L"\"%s\"",_data);ShellExecuteW(GetDesktopWindow(),L"Open",cmd,args,nullptr,b_show_window?SW_SHOW:SW_HIDE); }
	void open_dir() const { dir().open(nullptr,true); }
#endif

	// relative/absolute path
	inline bool is_absolute() const { return (_data[0]!=0&&_data[1]==L':')||memcmp(_data,L"\\\\",sizeof(wchar_t)*2)==0||memcmp(_data,L"//",sizeof(wchar_t)*2)==0||wcsstr(_data,L":\\")!=nullptr||wcsstr(_data,L":/")!=nullptr; }
	inline bool is_relative() const { return !is_absolute(); }
	inline bool is_unc() const { return (_data[0]==L'\\'&&_data[1]==L'\\')||(_data[0]==L'/'&&_data[1]==L'/'); }
	inline bool is_rsync() const { auto* p=wcsstr(_data,L":\\"); if(!p) p=wcsstr(_data,L":/"); return p!=nullptr&&p>_data+1; }
	inline bool is_subdir( const path& ancestor ) const { return _wcsnicmp(_data,ancestor._data,ancestor.size())==0; } // do not check existence
	inline path absolute( const wchar_t* base=L"" ) const { if(!*_data||is_pipe()||is_http_url()) return *this; return _wfullpath(__wcsbuf(),(!*base||is_absolute())?_data:wcscat(wcscpy(__wcsbuf(),path(base).add_backslash()),_data),capacity); }	// do not directly return for non-canonicalized path
	inline path relative( const wchar_t* from=L"", bool first_dot=false ) const;
	inline path canonical() const { path p(*this); if(p.is_pipe()||p.is_http_url()) return p; p.canonicalize(); return p; } // not necessarily absolute: return relative path as well

	// time stamp
	static const char* timestamp( const struct tm* t ){char* buff=__strbuf();sprintf(buff,"%04d%02d%02d%02d%02d%02d",t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);return buff;}
	static const char* timestamp( FILETIME f ){ SYSTEMTIME s;FileTimeToSystemTime(&f,&s);char* buff=__strbuf();sprintf(buff,"%04d%02d%02d%02d%02d%02d",s.wYear,s.wMonth,s.wDay,s.wHour,s.wMinute,s.wSecond);return buff; }
	const char* ctimestamp() const { if(!exists()) return ""; stat_t s=stat(); struct tm t; _gmtime64_s(&t,&s.st_ctime); return timestamp(&t); }
	const char* atimestamp() const { if(!exists()) return ""; stat_t s=stat(); struct tm t; _gmtime64_s(&t,&s.st_atime); return timestamp(&t); }
	const char* mtimestamp() const { if(!exists()) return ""; stat_t s=stat(); struct tm t; _gmtime64_s(&t,&s.st_mtime); return timestamp(&t); }

	FILETIME cfiletime() const { if(!cache_exists()) update_cache(); return cache().ftCreationTime; }
	FILETIME afiletime() const { if(!cache_exists()) update_cache(); return cache().ftLastAccessTime; }
	FILETIME mfiletime() const { if(!cache_exists()) update_cache(); return cache().ftLastWriteTime; }
	SYSTEMTIME csystemtime() const { return FileTimeToSystemTime(cfiletime()); }
	SYSTEMTIME asystemtime() const { return FileTimeToSystemTime(afiletime()); }
	SYSTEMTIME msystemtime() const { return FileTimeToSystemTime(mfiletime()); }
	void set_filetime( const FILETIME* ctime, const FILETIME* atime, const FILETIME* mtime ) const { HANDLE h=CreateFileW(_data,FILE_WRITE_ATTRIBUTES,0,nullptr,OPEN_EXISTING,0,nullptr); if(!h)return; auto& c=cache(); if(ctime) c.ftCreationTime=*ctime; if(atime) c.ftLastAccessTime=*atime; if(mtime) c.ftLastWriteTime=*mtime; SetFileTime(h, ctime, atime, mtime ); CloseHandle(h); }
	void set_filetime( FILETIME f ) const { set_filetime(&f,&f,&f); }
	void set_filetime( const path& other ) const { if(!other.exists()) return; other.update_cache(); auto& c=other.cache(); set_filetime(&c.ftCreationTime,&c.ftLastAccessTime,&c.ftLastWriteTime); }

	// module/working directories
	static inline path module_path( HMODULE h_module=nullptr ){ static path m; if(!m.empty()&&!h_module) return m; path p;GetModuleFileNameW(h_module,p,path::capacity);p[0]=::toupper(p[0]); p=p.canonical(); return h_module?p:(m=p); } // 'module' conflicts with C++ modules
	static inline path module_name( bool with_ext=true, HMODULE h_module=nullptr ){ return module_path(h_module).name(with_ext); }
	static inline path module_dir( HMODULE h_module=nullptr ){ static path d=module_path().dir(); return h_module?module_path(h_module).dir():d; }
	static inline path current_path(){ path p; auto* r=_wgetcwd(p._data,path::capacity); return p.absolute().add_backslash(); }	// current working directory
	static inline path cwd(){ return current_path(); }	// current working directory
	static inline path chdir( path dir ){ return dir.chdir(); }

	// file content access: void (rb/wb), char (r/w), wchar_t (r/w,ccs=UTF-8)
	FILE* fopen( const wchar_t* mode, bool utf8=false ) const;
	FILE* fopen( const char* mode, bool utf8=false ) const { wchar_t m[64]={}; __atow(mode,m); return fopen(m,utf8); }
	template <class T=void> sized_ptr_t<T> read_file() const;
	std::wstring read_file() const;
	bool write_file( const void* ptr, size_t size ) const;
	bool write_file( sized_ptr_t<void> p ) const { return p.ptr&&p.size?write_file(p.ptr,p.size):false; }
	bool write_file( const char* s ) const;
	bool write_file( const wchar_t* s ) const;

	// temporary directories
	static path temp( bool local=true, path local_dir=L"" ); // local_dir is used only when local is true

	// utilities
	static path serial( path dir, const wchar_t* prefix, const wchar_t* postfix, int numzero=4 );
	inline path key() const { if(!*_data)return path();path d;size_t n=0;for(size_t k=0,kn=length();k<kn;k++){wchar_t c=_data[k];if(c!=L':'&&c!=L' ') d[n++]=(c==L'\\'||c==L'/')?L'.':(::tolower(c));}if(d[n-1]==L'.')n--;d[n]=0;return d; }

	// scan(): ext_filter (specific extensions delimited by semicolons), str_filter (path should contain this string)
	template <bool recursive=true> std::vector<path> scan( const wchar_t* ext_filter=nullptr, const wchar_t* pattern=nullptr, bool b_subdirs=false ) const;
	template <bool recursive=true> std::vector<path> subdirs( const wchar_t* pattern=nullptr ) const;

protected:

	__forceinline attrib_t& cache() const { return *((attrib_t*)(_data+capacity)); }
	__forceinline bool		cache_exists() const { attrib_t* c=(attrib_t*)(_data+capacity); return c->ftLastWriteTime.dwHighDateTime>0&&c->dwFileAttributes==INVALID_FILE_ATTRIBUTES; }
#if defined _M_IX86
	static size_t file_size( FILE* fp ){ if(!fp) return 0; auto p=ftell(fp); fseek(fp,0,SEEK_END); size_t s=ftell(fp); fseek(fp,p,SEEK_SET); return s; }
#elif defined _M_X64
	static size_t file_size( FILE* fp ){ if(!fp) return 0; auto p=_ftelli64(fp); _fseeki64(fp,0,SEEK_END); size_t s=_ftelli64(fp); _fseeki64(fp,p,SEEK_SET); return s; }
#endif
	void canonicalize(); // remove redundancies in directories (e.g., "../some/..", "./" )

	struct scan_t { std::vector<path> result; struct { bool recursive, subdirs, dir, glob; } b; struct { sized_ptr_t<wchar_t>* v; size_t l; } ext; struct { const wchar_t* pattern; size_t l; } glob; };
	void scan_recursive( path dir, scan_t& si ) const;
	void subdirs_recursive( path dir, scan_t& si ) const;

	wchar_t* _data;
};

//***********************************************
// definitions of long inline member functions

__noinline bool path::mkdir() const // make all super directories
{
	if(exists()) return false;
	auto v=to_backslash().dir().ancestors(); if(v.empty()) return false; auto bl=v.back().length();
	if(is_unc()){ auto r=dir().unc_root();size_t rl=r.length();if(bl<=rl+1){v.pop_back();bl=v.back().length();}if(bl<=rl+1)v.pop_back(); }
	else if(bl<=3){ if(v.back()[1]==L':')v.pop_back();else if(bl<=1)v.pop_back(); }
	for( auto it=v.rbegin(); it!=v.rend(); it++ ){ if(!it->exists()&&_wmkdir(it->_data)!=0) return false; }
	return true;
}

__noinline FILE* path::fopen( const wchar_t* mode, bool utf8 ) const
{
	wchar_t m[32]={}; _swprintf(m,L"%s%s",mode,utf8?L",ccs=UTF-8":L"");
	FILE* fp = _wfopen(_data,m); if(!fp) return nullptr;
	else if(wcscmp(mode,L"w")==0&&utf8&&ext()!=L"sln") _fseeki64(fp,0,SEEK_SET); // remove byte order mask (BOM); sln use BOM, but vcxproj do not use BOM
	return fp;
}

template<> __noinline sized_ptr_t<void> path::read_file<void>() const
{
	sized_ptr_t<void> p={nullptr,0};
	FILE* fp=fopen(L"rb",false); if(!fp) return {nullptr,0};
	p.size = file_size(fp); if(!p.size){ fclose(fp); return {nullptr,0}; }
	p.ptr=malloc(p.size+1); if(p.ptr) fread(p.ptr,1,p.size,fp); ((char*)p.ptr)[p.size]=0;
	fclose(fp);
	return p;
}

template<> __noinline sized_ptr_t<wchar_t> path::read_file<wchar_t>() const
{
	sized_ptr_t<wchar_t> p={nullptr,0};
	FILE* fp=fopen(L"r",true); if(!fp) return {nullptr,0};
	size_t size0 = file_size(fp); if(!size0){ fclose(fp); return {nullptr,0}; }

	std::wstring buffer; buffer.reserve(size0*2);
	wchar_t buff[4096]; while(fgetws(buff,4096,fp)) buffer+=buff; fclose(fp);
	p.size = buffer.size();
	p.ptr = (wchar_t*) malloc((p.size+1)*sizeof(wchar_t));
	if(p.ptr) memcpy(p.ptr,buffer.c_str(),p.size*sizeof(wchar_t));
	p.ptr[p.size]=0;
	return p;
}

template<> __noinline sized_ptr_t<char> path::read_file<char>() const
{
	sized_ptr_t<char> p={nullptr,0};
	FILE* fp=fopen(L"r"); if(!fp) return {nullptr,0};
	size_t size0 = file_size(fp); if(!size0){ fclose(fp); return {nullptr,0}; }

	std::string buffer; buffer.reserve(size0*2);
	char buff[4096]; while(fgets(buff,4096,fp)) buffer+=buff; fclose(fp);
	p.size = buffer.size();
	p.ptr = (char*) malloc((p.size+2)*sizeof(char));
	if(p.ptr) memcpy(p.ptr,buffer.c_str(),p.size*sizeof(char));
	p.ptr[p.size]=p.ptr[p.size+1]=0; // two more bytes for null-ended wchar_t string
	return p;
}

__noinline std::wstring path::read_file() const
{
	sized_ptr_t<wchar_t> p=read_file<wchar_t>(); if(!p) return L"";
	std::wstring s=p.ptr; free(p.ptr); return s;
}

template<> __noinline sized_ptr_t<const void> path::read_file<const void>() const
{
	auto p=read_file<void>(); return {(const void*)p.ptr,p.size};
}

template<> __noinline sized_ptr_t<const char> path::read_file<const char>() const
{
	auto p=read_file<char>(); return {(const char*)p.ptr,p.size};
}

template<> __noinline sized_ptr_t<const wchar_t> path::read_file<const wchar_t>() const
{
	auto p=read_file<wchar_t>(); return {(const wchar_t*)p.ptr,p.size};
}

__noinline bool path::write_file( const void* ptr, size_t size ) const
{
	FILE* fp=fopen(L"wb"); if(!fp) return false;
	size_t size_written = ptr&&size?fwrite(ptr,1,size,fp):0; fclose(fp);
	return size_written==size;
}

__noinline bool path::write_file( const char* s ) const
{
	FILE* fp=fopen(L"w"); if(!fp) return false;
	int ret = s?fputs(s,fp):0; fclose(fp);
	return ret>=0;
}

__noinline bool path::write_file( const wchar_t* s ) const
{
	FILE* fp=fopen(L"w",true); if(!fp) return false;
	int ret = s?fputws(s,fp):0; fclose(fp);
	return ret>=0;
}

template <bool recursive> __noinline
std::vector<path> path::scan( const wchar_t* ext_filter, const wchar_t* pattern, bool b_subdirs ) const
{
	path src=empty()?path(L".\\"):(is_relative()?absolute(L".\\"):*this).add_backslash(); if(!src.is_dir()) return std::vector<path>{};
	std::vector<std::wstring> exts; if(ext_filter&&ext_filter[0]){ wchar_t ef[4096]={}, *ctx=nullptr; wcscpy(ef,ext_filter); for(wchar_t* e=wcstok_s(ef,L";",&ctx);e;e=wcstok_s(nullptr,L";",&ctx)) if(e[0]) exts.push_back(std::wstring(L".")+e); }
	std::vector<sized_ptr_t<wchar_t>> eptr; for( auto& e:exts ) eptr.emplace_back(sized_ptr_t<wchar_t>{(wchar_t*)e.c_str(),e.size()});
	scan_t si;
	si.b.recursive=recursive; si.b.subdirs=b_subdirs; si.b.dir=recursive||b_subdirs; si.b.glob=pattern&&(wcschr(pattern,L'*')||wcschr(pattern,L'?'));
	si.ext.v=eptr.size()>0?&eptr[0]:nullptr; si.ext.l=eptr.size();
	si.glob.pattern=pattern; si.glob.l=pattern?wcslen(pattern):0;
	si.result.reserve(1ull<<16);scan_recursive(src,si);si.result.shrink_to_fit();
	return si.result;
}

__noinline void path::scan_recursive( path dir, path::scan_t& si ) const
{
	WIN32_FIND_DATAW fd={}; HANDLE h=FindFirstFileExW(dir+L"*.*",FindExInfoBasic/*minimal(faster)*/,&fd,FindExSearchNameMatch,0,FIND_FIRST_EX_LARGE_FETCH); if(h==INVALID_HANDLE_VALUE) return;
	size_t dl=wcslen(dir._data); wchar_t *f=fd.cFileName, *p=dir._data+dl; auto*e=si.ext.v;
	std::vector<path> sdir; if(si.b.recursive) sdir.reserve(16);

	while(FindNextFileW(h,&fd))
	{
		size_t fl=wcslen(f);
		if((fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)==0) // files
		{
			if(e){size_t j=0;for(;j<si.ext.l;j++){if(e[j].size<fl&&_wcsicmp(e[j],f+fl-e[j].size)==0)break;}if(j==si.ext.l)continue;}
			if(si.glob.l){if(si.b.glob?!iglob(f,fl,si.glob.pattern,si.glob.l):_wcsistr(f,fl,si.glob.pattern,si.glob.l)==nullptr)continue;}
			memcpy(p,f,sizeof(wchar_t)*fl); p[fl]=0; si.result.emplace_back(dir);
		}
		else if((fd.dwFileAttributes&FILE_ATTRIBUTE_HIDDEN)==0&&si.b.dir)
		{
			if(f[0]==L'.'){ if(f[1]==0||memcmp(f+1,L".",4)==0||memcmp(f+1,L"git",8)==0) continue; } // skip .git
			memcpy(p,f,sizeof(wchar_t)*fl); p[fl]=L'\\';p[fl+1]=0; sdir.emplace_back(dir);
		}
	}
	FindClose(h);

	for(auto& c:sdir)
	{
		if(si.b.subdirs)	si.result.emplace_back(c);
		if(si.b.recursive)	scan_recursive(c,si);
	}
}

template <bool recursive> __noinline
std::vector<path> path::subdirs( const wchar_t* pattern ) const
{
	path src=empty()?path(L".\\"):(is_relative()?absolute(L".\\"):*this).add_backslash(); if(!src.is_dir()) return std::vector<path>{};
	scan_t si;
	si.b.recursive=recursive; si.b.subdirs=true; si.b.dir=true; si.b.glob=pattern&&(wcschr(pattern,L'*')||wcschr(pattern,L'?'));
	si.ext.v=nullptr; si.ext.l=0;
	si.glob.pattern=pattern; si.glob.l=pattern?wcslen(pattern):0;
	si.result.reserve(1ull<<12);subdirs_recursive(src,si);si.result.shrink_to_fit();
	return si.result;
}

__noinline void path::subdirs_recursive( path dir, path::scan_t& si ) const
{
	WIN32_FIND_DATAW fd={}; HANDLE h=FindFirstFileExW(dir+L"*.*",FindExInfoBasic/*minimal(faster)*/,&fd,FindExSearchNameMatch,0,FIND_FIRST_EX_LARGE_FETCH); if(h==INVALID_HANDLE_VALUE) return;
	size_t dl=wcslen(dir._data); wchar_t *f=fd.cFileName, *p=dir._data+dl;
	std::vector<path> sdir; if(si.b.recursive) sdir.reserve(16);

	while(FindNextFileW(h,&fd))
	{
		if((fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)==0) continue;
		if(f[0]==L'.'){ if(!f[1]||memcmp(f+1,L".",4)==0||memcmp(f+1,L"git",8)==0) continue; } // skip .git
		size_t fl=wcslen(f); memcpy(p,f,sizeof(wchar_t)*fl); p[fl]=L'\\';p[fl+1]=0;
		if(si.b.recursive) sdir.emplace_back(dir);
		if(si.glob.l==0) si.result.emplace_back(dir);
		else if(!si.b.glob&&_wcsistr(f,fl,si.glob.pattern,si.glob.l)) si.result.emplace_back(dir);
		else if(si.b.glob&&iglob(f,fl,si.glob.pattern,si.glob.l)) si.result.emplace_back(dir);
	}
	FindClose(h);
	if(si.b.recursive){ for(auto& c:sdir) subdirs_recursive(c,si); }
}

__noinline path path::serial( path dir, const wchar_t* prefix, const wchar_t* postfix, int numzero )
{
	dir=dir.add_backslash(); if(!dir.exists()) dir.mkdir();
	int nMaxFiles=1; for(int k=0;k<numzero;k++) nMaxFiles*=10;
	path fmt,p; swprintf((wchar_t*)fmt.c_str(),path::capacity,L"%s%s%%0%dd%s%s",dir.c_str(),prefix,numzero,postfix&&postfix[0]?L".":L"",postfix);
	for(int k=0;k<nMaxFiles;k++){swprintf(p,path::capacity,fmt,k);if(!p.exists()) return p;}
	swprintf(p,path::capacity,fmt,nMaxFiles-1);
	return p;
}

// forward declarations (defined in gxos.h)
namespace os { path temp(); path local_appdata(); } 

__noinline path path::temp( bool local, path local_dir )
{
	static path r=os::local_appdata()+module_path().name(false).key()+L"\\";
	path t=r; if(local){ if(local_dir.empty()) local_dir=module_dir(); t+=path(L"local\\")+local_dir.key().add_backslash(); }
	if(!t.exists()) t.mkdir();
	return t;
}

__noinline path path::relative( const wchar_t* from, bool first_dot ) const
{
	if(is_pipe()||is_http_url()) return *this;
	if(is_relative()) return !first_dot?this->remove_first_dot():*this;

	path from_dir = (!from||!from[0]) ? path::cwd() : path(from).dir().absolute();
	if(::tolower(from_dir[0])!=::tolower(this->_data[0])) return *this; // different drive

	// 1. keep filename, make the list of reference and current path
	std::vector<path> src_list=from_dir.explode(), dst_list=dir().explode();

	// 2. compare and count the different directory levels
	path result;
	size_t s=0,d=0,ss=src_list.size(),ds=dst_list.size();
	for(; s<ss&&d<ds; s++, d++ ) if(_wcsicmp(src_list[s],dst_list[d])!=0) break;
	for(; s<ss; s++ ) result += L"..\\";
	for(; d<ds; d++ ) result += dst_list[d]+L'\\';
	result.canonicalize();

	// 3. if empty dir, then attach ./
	if(first_dot){ if(result[0]==0||result[0]!=L'.') result=path(".\\")+result; }

	wchar_t b=back();
	return b==L'\\'||b==L'/'?result:result+name();
}

__noinline void path::canonicalize()
{
	size_t len=_data[0]?wcslen(_data):0; if(len==0) return;
	for(uint k=0;k<len;k++) if(_data[k]==L'/') _data[k]=L'\\'; // slash to backslash
	if(_data[len-1]==L'.'&&((len>2&&_data[len-2]==L'\\')||(len>3&&_data[len-3]==L'\\'&&_data[len-2]==L'.'))){ _data[len++]=L'\\'; _data[len]=L'\0'; } // add trailing slash to "\\." or "\\.."
	if(len==2&&_data[1]==L':'){ _data[len++]=L'\\'; _data[len]=L'\0'; } // root correction
	if(!wcsstr(_data,L"\\.\\")&&!wcsstr(_data,L"\\..\\")) return; // trivial return
	wchar_t* ds; while((ds=wcsstr(_data+1,L"\\\\"))) memmove(ds+1,ds+2,((len--)-(ds-_data)-1)*sizeof(wchar_t)); // correct multiple backslashes, except the beginning of unc path
	if(is_absolute()){ auto* r=_wfullpath(_data,(const wchar_t*)memcpy(__wcsbuf(),_data,sizeof(wchar_t)*(len+1)),capacity); return; }

	// flags to check
	bool b_trailing_backslash = (_data[len-1]==L'\\');
	bool b_single_dot_begin = _data[0]==L'.'&&_data[1]==L'\\';

	// perform canonicalization
	std::deque<wchar_t*> L;wchar_t* ctx=nullptr;
	for(wchar_t* t=wcstok_s(wcscpy(__wcsbuf(),_data),L"\\",&ctx);t;t=wcstok_s(nullptr,L"\\",&ctx))
	{
		if(t[0]==L'.'&&t[1]==L'.'&&!L.empty()&&memcmp(L.back(),L"..",sizeof(wchar_t)*2)!=0) L.pop_back();
		else if(t[0]!=L'.'||t[1]!=0) L.emplace_back(wcscpy(__wcsbuf(),t));
	}

	// reconstruction
	wchar_t* d=_data;
	if(!b_single_dot_begin&&L.empty()){ d[0]=L'\0';return; }
	if(L.empty()||(b_single_dot_begin&&(!L.empty()&&L.front()[0]!=L'.'))){ (d++)[0]=L'.';(d++)[0]=L'\\';d[0]=L'\0'; } // make it begin with single-dot again
	for(size_t k=0,kn=L.size();k<kn;k++){ wchar_t* t=L[k];size_t l=wcslen(t);memcpy(d,t,l*sizeof(wchar_t));d+=l;(d++)[0]=L'\\';d[0]=L'\0'; }
	if(!b_trailing_backslash&&d>_data)(d-1)[0]=L'\0';
}

//***********************************************
// nocase/std map/unordered_map extension for path
namespace std
{
	template <> struct hash<path>{ size_t operator()(const path& p)const{ return std::hash<std::wstring>()(::tolower(p.c_str()));}};
}

namespace nocase
{
	template <> struct less<path>{ bool operator()(const path& a,const path& b)const{return a<b;}};
	template <> struct equal_to<path>{ bool operator()(const path& a,const path& b)const{return _wcsicmp(a.c_str(),b.c_str())==0;}};
	template <> struct hash<path>{ size_t operator()(const path& p)const{ return std::hash<std::wstring>()(::tolower(p.c_str()));}};
}

//***********************************************
#endif // __GX_FILESYSTEM_H__
