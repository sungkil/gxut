//*******************************************************************
// Copyright 2011-2020 Sungkil Lee
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

#include <direct.h>		// directory control
#include <io.h>			// low-level io functions
#include <time.h>
#include <deque>

//***********************************************
// Win32-like filetime utilities
inline int64_t FileTimeOffset( int days, int hours=0, int mins=0, int secs=0, int mss=0 ){ return 10000ll*(mss+1000ll*secs+60*1000ll*mins+60*60*1000ll*hours+24*60*60*1000ll*days); } // FILETIME in 100 ns scale
inline FILETIME DiscardFileTimeMilliseconds( FILETIME f ){uint64_t u=((uint64_t(f.dwHighDateTime)<<32|uint64_t(f.dwLowDateTime))/10000000)*10000000;return FILETIME{DWORD(u&0xffffffff),DWORD(u>>32)};} // 1ms = 10000 in FILETIME
inline SYSTEMTIME FileTimeToSystemTime( const FILETIME& f ){ SYSTEMTIME s; FileTimeToSystemTime(&f,&s); return s; }
inline FILETIME SystemTimeToFileTime( const SYSTEMTIME& s ){ FILETIME f; SystemTimeToFileTime(&s,&f); return f; }
inline uint64_t FileTimeToUint64( const FILETIME& f, uint offset_days=0 ){ return (uint64_t(f.dwHighDateTime)<<32|uint64_t(f.dwLowDateTime))-FileTimeOffset(offset_days); }
inline FILETIME	Uint64ToFileTime( uint64_t u ){ FILETIME f; f.dwHighDateTime=DWORD(u>>32); f.dwLowDateTime=u&0xffffffff; return f; }
inline uint64_t SystemTimeToUint64( const SYSTEMTIME& s, uint offset_days=0 ){ FILETIME f; SystemTimeToFileTime( &s, &f ); return FileTimeToUint64(f,offset_days); }
inline FILETIME now(){ FILETIME f; GetSystemTimeAsFileTime(&f); return f; } // current time

inline bool operator==( const FILETIME& f1, const FILETIME& f2 ){ return CompareFileTime(&f1,&f2)==0; }
inline bool operator!=( const FILETIME& f1, const FILETIME& f2 ){ return CompareFileTime(&f1,&f2)!=0; }
inline bool operator>(  const FILETIME& f1, const FILETIME& f2 ){ return CompareFileTime(&f1,&f2)>0; }
inline bool operator<(  const FILETIME& f1, const FILETIME& f2 ){ return CompareFileTime(&f1,&f2)<0; }
inline bool operator>=( const FILETIME& f1, const FILETIME& f2 ){ return CompareFileTime(&f1,&f2)>=0; }
inline bool operator<=( const FILETIME& f1, const FILETIME& f2 ){ return CompareFileTime(&f1,&f2)<=0; }

//***********************************************
// common constants
static const int GX_MAX_PATH = 1024;	// MAX_PATH == 260

struct path
{
	typedef WIN32_FILE_ATTRIBUTE_DATA attrib_t; // auxiliary cache information from scan()
	typedef struct _stat stat_t; // use "struct _stat" instead of "_stat" for C-compatibility

	static constexpr int max_buffers	= 4096;
	static constexpr int capacity		= GX_MAX_PATH; // MAX_PATH == 260

	// utility string functions
	static __forceinline wchar_t*		__wcsbuf(){ static wchar_t buff[max_buffers][capacity]; static int i=0; return buff[(i++)%std::extent<decltype(buff)>::value]; }
	static __forceinline char*			__strbuf(){ return (char*)__wcsbuf(); }
	static __forceinline wchar_t*		__mb2wc( const char* _Src, wchar_t* _Dst ){ int l=MultiByteToWideChar(0,0,_Src,-1,0,0);if(l>capacity-1)l=capacity-1; MultiByteToWideChar(0,0,_Src,-1,_Dst,l); return _Dst; }
	static __forceinline char*			__wc2mb( const wchar_t* _Src, char* _Dst ){ int l=WideCharToMultiByte(0,0,_Src,-1,0,0,0,0);if(l>capacity-1)l=capacity-1; WideCharToMultiByte(0,0,_Src,-1,_Dst,l,0,0); return _Dst; }
	static __forceinline const wchar_t*	__tolower( const wchar_t* _Str, size_t l ){ wchar_t* s=(wchar_t*)memcpy(__wcsbuf(),_Str,sizeof(wchar_t)*l); s[l]=L'\0'; for(wchar_t* p=s;*p;p++) *p=towlower(*p); return s; }
	static __forceinline const wchar_t*	__wcsistr( const wchar_t* _Str1, size_t l1, const wchar_t* _Str2, size_t l2 ){ const wchar_t *s1=__tolower(_Str1,l1), *s2=__tolower(_Str2,l2); const wchar_t* r=wcsstr(s1,s2); return r?_Str1+(r-s1):nullptr; }
	static __forceinline const wchar_t*	__wcsistr( const wchar_t* _Str1, const wchar_t* _Str2 ){ return __wcsistr(_Str1,wcslen(_Str1),_Str2,wcslen(_Str2)); }
	static __forceinline bool			__wcsiext( const wchar_t* _Str, size_t l, const wchar_t* ext, size_t el ){ return el<l&&_wcsicmp(ext,_Str+l-el)==0; }

	// disk volume
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

	// destructor/constuctors
	__forceinline wchar_t* alloc(){ static constexpr size_t s=sizeof(wchar_t)*capacity+sizeof(attrib_t); data=(wchar_t*)malloc(s); data[0]=0; return data; }
	~path() noexcept { if(data) free(data); }
	path() noexcept { alloc(); clear_cache(); }
	path( const path& p ) noexcept { wcscpy(alloc(),p); cache()=p.cache(); } // do not canonicalize for copy constructor
	path( path&& p ) noexcept { data=p.data; p.data=nullptr; } // cache moves as well
	path( const wchar_t* s ) noexcept : path() { wcscpy(data,s); canonicalize(); }
	path( const char* s ) noexcept : path() { __mb2wc(s,data); canonicalize(); }
	explicit path( const std::wstring& s ) noexcept : path() { wcscpy(data,s.c_str()); canonicalize(); }
	explicit path( const std::string& s ) noexcept : path() { __mb2wc(s.c_str(),data); canonicalize(); }

	// operator overloading: assignment
	path& operator=( const path& p ) noexcept { wcscpy(data,p.data); cache()=p.cache(); return *this; }
	path& operator=( path&& p ) noexcept { if(data) free(data); data=p.data; p.data=nullptr; return *this; }
	path& operator=( const wchar_t* s ) noexcept { wcscpy(data,s); canonicalize(); return *this; }
	path& operator=( const char* s ) noexcept { __mb2wc(s,data); canonicalize(); return *this; }
	path& operator=( const std::wstring& s ) noexcept { wcscpy(data,s.c_str()); canonicalize(); return *this; }
	path& operator=( const std::string& s ) noexcept { __mb2wc(s.c_str(),data); canonicalize(); return *this; }

	// concatenations
	path& operator+=( const path& p ){ wcscat(data,p.data+((p.data[0]==L'.'&&p.data[1]==L'\\'&&p.data[2])?2:0)); canonicalize(); return *this; }
	path& operator+=( const wchar_t* s ){ if(s[0]==L'.'&&s[1]==L'\\'&&s[2]) s+=2; size_t l=wcslen(data); wcscpy(data+l,s); canonicalize(); return *this; }
	path& operator+=( const char* s ){ size_t l=wcslen(data); __mb2wc(s,data+l); canonicalize(); return *this; }
	path& operator+=( wchar_t c ){ size_t l=wcslen(data); data[l]=c; data[l+1]=0; return *this; }
	path& operator+=( char c ){ size_t l=wcslen(data); data[l]=wchar_t(c); data[l+1]=0; return *this; }
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
	path operator+( wchar_t c ){ return clone().operator+=(c); }
	path operator+( char c ){ return clone().operator+=(c); }
	path operator+( const std::wstring& s ) const { return clone().operator+=(s.c_str()); }
	path operator+( const std::string& s ) const { return clone().operator+=(s.c_str()); }

	path cat( const path& p ) const { return clone().operator+=(p); }
	path cat( const wchar_t* s ) const { return clone().operator+=(s); }
	path cat( const char* s ) const { return clone().operator+=(s); }
	path cat( wchar_t c ){ return clone().operator+=(c); }
	path cat( char c ){ return clone().operator+=(c); }
	path cat( const std::wstring& s ) const { return clone().operator+=(s.c_str()); }
	path cat( const std::string& s ) const { return clone().operator+=(s.c_str()); }

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

	// operator overloading: array operator
	inline wchar_t& operator[]( ptrdiff_t i ){ return data[i]; }
	inline const wchar_t& operator[]( ptrdiff_t i ) const { return data[i]; }

	// operator overloading: casting and conversion
	operator wchar_t*(){ return data; }
	operator const wchar_t*() const { return data; }
	const wchar_t* c_str() const { return data; }
	const char* wtoa() const { return __wc2mb(data,__strbuf()); }
	std::wstring str() const { return std::wstring(data); }

	// iterators
	using iterator = wchar_t*;
	using const_iterator = const wchar_t*;
	iterator begin() const { return data; }
	iterator end() const { return data+wcslen(data); }
	wchar_t& front() const { return data[0]; }
	wchar_t& back() const { return data[0]==0?data[0]:data[wcslen(data)-1]; }

	// clear and trivial clone
	void clear(){ data[0]=0; clear_cache(); }
	path clone() const { return path(*this); }

	// size/length
	inline bool empty()			const { return data[0]==0; }
	inline size_t size()		const { return wcslen(data); }
	inline size_t length()		const { return wcslen(data); }
	inline uint64_t file_size() const { if(!cache_exists()) update_cache(); auto& c=cache(); return uint64_t(c.nFileSizeHigh)<<32ull|uint64_t(c.nFileSizeLow); }

	// crc32c/md5 checksums
	inline uint crc32c() const;	// implemented in gxmath.h
	inline uint4 md5() const;	// implemented in gxmemory.h

	// system-related: slash, backslash, unix, quote
	path to_backslash()		const {	path p(*this); std::replace(p.begin(),p.end(),L'/',L'\\'); return p; }
	path to_slash()			const {	path p(*this); std::replace(p.begin(),p.end(),L'\\',L'/'); return p; }
	path to_dot()			const {	path p(*this); std::replace(p.begin(),p.end(),L'\\',L'/'); std::replace(p.begin(),p.end(),L'/',L'.'); return p; }
	path add_backslash()	const { path p(*this); size_t l=wcslen(p.data); if(l&&p.data[l-1]!='\\'){p.data[l]='\\';p.data[l+1]=L'\0';} return p; }
	path add_slash()		const { path p(*this); size_t l=wcslen(p.data); if(l&&p.data[l-1]=='\\') p.data[l-1]='/'; else if(l&&p.data[l-1]!='/'){p.data[l]='/';p.data[l+1]=L'\0';} return p; }
	path remove_backslash()	const { path p(*this); size_t l=wcslen(p.data); if(l&&p.data[l-1]=='\\'){p.data[l-1]=L'\0';} return p; }
	path remove_slash()		const { path p(*this); size_t l=wcslen(p.data); if(l&&p.data[l-1]=='/'){p.data[l-1]=L'\0';} return p; }
	path auto_quote()		const { if(!data[0]||!wcschr(data,L' ')||(data[0]==L'\"'&&data[wcslen(data)-1]==L'\"')) return *this; path p; p[0]=L'\"'; size_t l=wcslen(data); memcpy(p.data+1,data,l*sizeof(wchar_t)); p[l+1]=L'\"'; p[l+2]=0; return p; }
	path unix()				const {	path p(*this); p.canonicalize(); p=p.to_slash(); size_t len=p.length(); if(len<2||p.is_relative()||p.is_unc()||p.is_rsync()) return p; p.data[1]=wchar_t(::tolower(p.data[0])); p.data[0]=L'/'; return p; }
	path cygwin()			const { path p(*this); p.canonicalize(); p=p.to_slash(); size_t len=p.length(); if(len<2||p.is_relative()||p.is_unc()||p.is_rsync()) return p; path p2; swprintf_s( p2, capacity, L"/cygdrive/%c%s", ::tolower(p[0]), p.data+2 ); return p2; }

	// split path
	struct split_t { wchar_t *drive, *dir, *fname, *ext; };
	__forceinline split_t split( wchar_t* drive=nullptr, wchar_t* dir=nullptr, wchar_t* fname=nullptr, wchar_t* ext=nullptr ) const { _wsplitpath_s(data,drive,drive?_MAX_DRIVE:0,dir,dir?_MAX_DIR:0,fname,fname?_MAX_FNAME:0,ext,ext?_MAX_EXT:0); if(drive&&drive[0]) drive[0]=wchar_t(::toupper(drive[0])); return split_t{drive,dir,fname,ext}; }

	// path info/operations
	volume_t volume() const { return (is_unc()||is_rsync()||!drive().exists())?volume_t():volume_t(drive().c_str()); }
	path drive() const { path p; _wsplitpath_s(data,p.data,_MAX_DRIVE,0,0,0,0,0,0); if(*p.data) *p.data=wchar_t(::toupper(*p.data)); return p; }
	path dir() const { path p; wchar_t* d=__wcsbuf(); _wsplitpath_s(data,p.data,_MAX_DRIVE,d,_MAX_DIR,0,0,0,0); size_t pl=wcslen(p.data), dl=wcslen(d); if(0==(pl+dl)) return L".\\"; if(dl){ memcpy(p.data+pl,d,dl*sizeof(wchar_t)); p.data[pl+dl]=0; } return p; }
	path name( bool with_ext=true ) const { path p; wchar_t* ext=with_ext?__wcsbuf():nullptr; _wsplitpath_s(data,0,0,0,0,p.data,_MAX_FNAME,ext,ext?_MAX_EXT:0); if(!ext) return p; size_t pl=wcslen(p.data), el=wcslen(ext); if(el){ memcpy(p.data+pl,ext,el*sizeof(wchar_t)); p.data[pl+el]=0; } return p; }
	path dir_name() const { if(!wcschr(data,L'\\')) return L""; return dir().remove_backslash().name(); }
	path ext() const { wchar_t e[_MAX_EXT+1]; _wsplitpath_s(data,0,0,0,0,0,0,e,_MAX_EXT); path p; if(*e!=0) wcscpy(p.data,e+1); return p; }
	path parent() const { return dir().remove_backslash().dir(); }
	path remove_first_dot()	const { return (wcslen(data)>2&&data[0]==L'.'&&data[1]==L'\\') ? path(data+2) : *this; }
	path remove_ext() const { split_t si=split(__wcsbuf(),__wcsbuf(),__wcsbuf()); return wcscat(wcscat(si.drive,si.dir),si.fname); }
	std::vector<path> explode( const wchar_t* delim=L"\\") const { std::vector<path> L; if(!delim||!*delim) return L; path s=delim[0]==L'/'&&delim[1]==0?to_slash():*this; L.reserve(16); wchar_t* ctx; for(wchar_t* t=wcstok_s(s.data,delim,&ctx);t;t=wcstok_s(0,delim,&ctx)) L.emplace_back(t); return L; }
	std::vector<path> ancestors( path root=L"" ) const { if(root.data[0]==0) root=module_dir(); int rl=int(root.size()); path d=dir();int l=int(d.size());bool r=_wcsnicmp(d.data,root.data,rl)==0;std::vector<path> a;a.reserve(4);for(int k=l-1,e=r?rl-1:0;k>=e;k--){if(d.data[k]==L'\\'||d.data[k]==L'/'){d.data[k+1]=0;a.emplace_back(d);}}return a;}

	// content manipulations
	path replace_ext( const wchar_t* ext ) const { if(!ext||!ext[0])return *this;split_t si=split(__wcsbuf(),__wcsbuf(),__wcsbuf(),__wcsbuf());path p;swprintf_s(p.data,capacity,L"%s%s%s%s%s",si.drive,si.dir,si.fname,ext[0]==L'.'?L"":L".",ext );return p; }
	path replace_ext( const char* ext ) const { if(!ext||!ext[0])return *this; return replace_ext(__mb2wc(ext,__wcsbuf())); }

	// multi-byte path info
	const char* aname( bool with_ext=true ) const { return name(with_ext).wtoa(); }
	const wchar_t* wname( bool with_ext=true ) const { return name(with_ext).c_str(); }

	// attribute by stats
	inline stat_t	stat() const { stat_t s={}; if(exists()) _wstat(data,&s); return s; }
	inline DWORD&	attributes() const { if(!cache_exists()) update_cache(); return cache().dwFileAttributes; }
	inline void		update_cache() const { auto& c=cache(); if(!GetFileAttributesExW(data,GetFileExInfoStandard,&c)||c.dwFileAttributes==INVALID_FILE_ATTRIBUTES){ memset(&c,0,sizeof(attrib_t)); c.dwFileAttributes=INVALID_FILE_ATTRIBUTES; return; } c.ftLastWriteTime = DiscardFileTimeMilliseconds(c.ftLastWriteTime); }
	inline void		clear_cache() const { attrib_t* a=(attrib_t*)(data+capacity); memset(a,0,sizeof(attrib_t)); a->dwFileAttributes=INVALID_FILE_ATTRIBUTES; }

	// get attributes
	bool exists() const {				if(!data[0]) return false; auto& a=attributes(); return a!=INVALID_FILE_ATTRIBUTES; }
	bool is_dir() const {				if(!data[0]) return false; auto& a=attributes(); return a!=INVALID_FILE_ATTRIBUTES&&(a&FILE_ATTRIBUTE_DIRECTORY)!=0; }
	bool is_root_dir() const {			if(!data[0]) return false; size_t l=length(); return is_dir()&&l<4&&l>1&&data[1]==L':'; }
	bool is_hidden() const {			if(!data[0]) return false; auto& a=attributes(); return a!=INVALID_FILE_ATTRIBUTES&&(a&FILE_ATTRIBUTE_HIDDEN)!=0; }
	bool is_readonly() const {			if(!data[0]) return false; auto& a=attributes(); return a!=INVALID_FILE_ATTRIBUTES&&(a&FILE_ATTRIBUTE_READONLY)!=0; }
	bool is_system() const {			if(!data[0]) return false; auto& a=attributes(); return a!=INVALID_FILE_ATTRIBUTES&&(a&FILE_ATTRIBUTE_SYSTEM)!=0; }
	bool is_junction() const {			if(!data[0]) return false; auto& a=attributes(); return a!=INVALID_FILE_ATTRIBUTES&&(a&FILE_ATTRIBUTE_REPARSE_POINT)!=0; }
	bool is_ssh() const {				if(!data[0]) return false; return wcsstr(data+2,L":\\")!=nullptr||wcsstr(data+2,L":/")!=nullptr; }
	bool is_synology() const {			if(!data[0]) return false; return __wcsistr(data,L":\\volume")!=nullptr||__wcsistr(data,L":/volume")!=nullptr; }

	// set attributes
	void set_hidden( bool h ) const {	if(!exists()) return; auto& a=attributes(); SetFileAttributesW(data,a=h?(a|FILE_ATTRIBUTE_HIDDEN):(a^FILE_ATTRIBUTE_HIDDEN)); }
	void set_readonly( bool r ) const {	if(!exists()) return; auto& a=attributes(); SetFileAttributesW(data,a=r?(a|FILE_ATTRIBUTE_READONLY):(a^FILE_ATTRIBUTE_READONLY)); }
	void set_system( bool s ) const {	if(!exists()) return; auto& a=attributes(); SetFileAttributesW(data,a=s?(a|FILE_ATTRIBUTE_SYSTEM):(a^FILE_ATTRIBUTE_SYSTEM)); }

	// directory attributes
	bool has_file( const path& file_name ) const { return is_dir()&&cat(file_name).exists(); }

	// chdir/make/copy/delete file/dir operations
	void chdir() const { if(is_dir()) _wchdir(data); }
	bool mkdir() const { if(exists()) return false; path p=to_backslash().remove_backslash(), d; wchar_t* ctx;for( wchar_t* t=wcstok_s(p,L"\\",&ctx); t; t=wcstok_s(nullptr,L"\\", &ctx) ){ d+=t;d+=L'\\'; if(!d.exists()&&_wmkdir(d.data)!=0) return false; } return true; } // make all super directories
	bool copy_file( path dst, bool overwrite=true ) const { if(!exists()||is_dir()||dst.empty()) return false; if(dst.is_dir()||dst.back()==L'\\') dst=dst.add_backslash()+name(); dst.dir().mkdir(); if(dst.exists()&&overwrite){ if(dst.is_hidden()) dst.set_hidden(false); if(dst.is_readonly()) dst.set_readonly(false); } return bool(CopyFileW( data, dst, overwrite?FALSE:TRUE )); }
	bool move_file( path dst ) const { return is_dir()?false:(drive()==dst.drive()&&!dst.exists()) ? MoveFileW(data,dst.c_str())!=0 : !copy_file(dst,true) ? false: rmfile(); }
#ifndef _INC_SHELLAPI
	bool rmdir() const { if(!is_dir()) return false; if(is_hidden()) set_hidden(false); if(is_readonly()) set_readonly(false); _wrmdir(data); return true; }
	bool rmfile() const { return delete_file(); }
	bool delete_file() const { if(!exists()||is_dir()) return false; if(is_hidden()) set_hidden(false); if(is_readonly()) set_readonly(false); return DeleteFileW(data)==TRUE; }
#else
	bool rmdir() const { if(!is_dir()) return false; if(is_hidden()) set_hidden(false); if(is_readonly()) set_readonly(false); SHFILEOPSTRUCTW fop={};fop.wFunc=FO_DELETE;fop.fFlags=FOF_ALLOWUNDO|FOF_SILENT|FOF_NOCONFIRMATION;fop.pFrom=data;data[wcslen(data)+1]=L'\0';return SHFileOperationW(&fop)==0;}
	bool rmfile( bool b_undo=false ) const { return delete_file(b_undo); }
	bool delete_file( bool b_undo=false ) const { if(!exists()||is_dir()) return false; if(is_hidden()) set_hidden(false); if(is_readonly()) set_readonly(false); SHFILEOPSTRUCTW fop={};fop.wFunc=FO_DELETE;fop.fFlags=(b_undo?FOF_ALLOWUNDO:0)|FOF_SILENT|FOF_NOCONFIRMATION|FOF_FILESONLY;fop.pFrom=data;data[wcslen(data)+1]=L'\0';return SHFileOperationW(&fop)==0;}
	bool copy_dir( path dst, bool overwrite=true ) const { if(!is_dir()) return false;wchar_t* from=__wcsbuf();swprintf_s(from,capacity,L"%s\\*\0",data);dst[dst.size()+1]=L'\0'; SHFILEOPSTRUCTW fop={};fop.wFunc=FO_COPY;fop.fFlags=FOF_ALLOWUNDO|FOF_SILENT|FOF_NOCONFIRMATION;fop.pFrom=from;fop.pTo=dst;return SHFileOperationW(&fop)==0; }
	bool move_dir( path dst ) const { return !is_dir()?false:(drive()==dst.drive()&&!dst.exists()) ? MoveFileW(data,dst.c_str())!=0 : !copy_dir(dst,true) ? false: rmdir(); }
	void open( const wchar_t* args=nullptr, bool bShowWindow=true ) const {path cmd;swprintf(cmd,capacity,L"\"%s\"",data);ShellExecuteW(GetDesktopWindow(),L"Open",cmd,args,nullptr,bShowWindow?SW_SHOW:SW_HIDE);}
#endif

	// relative/absolute path
	inline bool is_absolute() const { return (data[0]!=0&&data[1]==L':')||memcmp(data,L"\\\\",sizeof(wchar_t)*2)==0||memcmp(data,L"//",sizeof(wchar_t)*2)==0||wcsstr(data,L":\\")!=nullptr||wcsstr(data,L":/")!=nullptr; }
	inline bool is_relative() const { return !is_absolute(); }
	inline bool is_unc() const { return (data[0]==L'\\'&&data[1]==L'\\')||(data[0]==L'/'&&data[1]==L'/'); }
	inline bool is_rsync() const { auto* p=wcsstr(data,L":\\"); if(!p) p=wcsstr(data,L":/"); return p!=nullptr&&p>data+1; }
	inline bool is_subdir( const path& ancestor ) const { return _wcsnicmp(data,ancestor.data,ancestor.size())==0; } // do not check existence
	inline path absolute( const wchar_t* base=L"" ) const { if(!data[0]) return *this; return _wfullpath(__wcsbuf(),(!*base||is_absolute())?data:wcscat(wcscpy(__wcsbuf(),path(base).add_backslash()),data),capacity); }	// do not directly return for non-canonicalized path
	inline path relative( bool first_dot, const wchar_t* from ) const;
	inline path relative( bool first_dot ) const { return relative(first_dot,L""); }
	inline path relative( const wchar_t* from=L"" ) const { return relative(false,from); }
	inline path canonical() const { path p(*this); p.canonicalize(); return p; } // not necessarily absolute: return relative path as well

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
	SYSTEMTIME csystemtime() const { return FileTimeToSystemTime(cfiletime()); }
	SYSTEMTIME asystemtime() const { return FileTimeToSystemTime(afiletime()); }
	SYSTEMTIME msystemtime() const { return FileTimeToSystemTime(mfiletime()); }
	void set_filetime( const FILETIME* ctime, const FILETIME* atime, const FILETIME* mtime ) const { HANDLE h=CreateFileW(data,FILE_WRITE_ATTRIBUTES,0,nullptr,OPEN_EXISTING,0,nullptr); if(!h)return; auto& c=cache(); if(ctime) c.ftCreationTime=*ctime; if(atime) c.ftLastAccessTime=*atime; if(mtime) c.ftLastWriteTime=*mtime; SetFileTime(h, ctime, atime, mtime ); CloseHandle(h); }
	void set_filetime( const FILETIME& f ) const { set_filetime(&f,&f,&f); }
	void set_filetime( const path& other ) const { if(!other.exists()) return; other.update_cache(); auto& c=other.cache(); set_filetime(&c.ftCreationTime,&c.ftLastAccessTime,&c.ftLastWriteTime); }

	// module/working directories
	static inline path module_path( HMODULE h_module=nullptr ){ static path m; if(!m.empty()&&!h_module) return m; path p;GetModuleFileNameW(h_module,p,path::capacity);p[0]=::toupper(p[0]); p=p.canonical(); return h_module?p:(m=p); } // 'module' conflicts with C++ modules
	static inline path module_dir( HMODULE h_module=nullptr ){ static path d=module_path().dir(); return h_module?module_path(h_module).dir():d; }
	static inline path cwd(){ path p; _wgetcwd(p.data,path::capacity); return p.absolute().add_backslash(); }	// current working dir
	static inline void chdir( path dir ){ if(dir.is_dir()) _wchdir(dir.data); }

	// file content access: void (rb/wb), char (r/w), wchar_t (r/w,ccs=UTF-8)
	template <class T=void> sized_ptr_t<T> read_file() const;
	bool write_file( const void* ptr, size_t size ) const;
	bool write_file( void* ptr, size_t size ) const { return ptr&&size?write_file(ptr,size):false; }
	bool write_file( sized_ptr_t<void> p ) const { return p.ptr&&p.size?write_file(p.ptr,p.size):false; }
	bool write_file( const char* s ) const;
	bool write_file( const wchar_t* s ) const;

	// system/global path attributes
	struct system { static inline path temp(); static inline path system_dir(){static path s;if(!s.empty())return s;GetSystemDirectoryW(s,path::capacity);return s=s.add_backslash();} };
	struct global { static inline path temp( const char* subdir=nullptr ); };

	// utilities
	static path temp( const wchar_t* subkey=nullptr, const char* subdir=nullptr );
	static path serial( path dir, const wchar_t* prefix, const wchar_t* postfix, int numzero=4 );
	inline path key() const { if(!*data)return path();path d;size_t n=0;for(size_t k=0,kn=length();k<kn;k++){wchar_t c=data[k];if(c!=L':'&&c!=L' ') d[n++]=(c==L'\\'||c==L'/')?L'.':(::tolower(c));}if(d[n-1]==L'.')n--;d[n]=0;return d; }
	inline path tolower() const { path d;size_t l=length();for(size_t k=0;k<l;k++)d[k]=::tolower(data[k]);d[l]=L'\0'; return d; }
	inline path toupper() const { path d;size_t l=length();for(size_t k=0;k<l;k++)d[k]=::toupper(data[k]);d[l]=L'\0'; return d; }

	// scan/findfile: ext_filter (specific extensions delimited by semicolons), str_filter (path should contain this string)
	static bool glob( const wchar_t* str, size_t slen, const wchar_t* pattern, size_t plen );
	static bool iglob( const wchar_t* str, size_t slen, const wchar_t* pattern, size_t plen ); // case-insensitive
	inline bool glob( const wchar_t* pattern, size_t plen=0 ) const { return glob( data, size(), pattern, plen ); }
	inline bool iglob( const wchar_t* pattern, size_t plen=0 ) const { return iglob( data, size(), pattern, plen ); } // case-insensitive

	std::vector<path> scan( char recursive=true, const wchar_t* ext_filter=nullptr, const wchar_t* pattern=nullptr ) const; // char blocks implicit conversion of wchar_t*
	std::vector<path> subdirs( char recursive=true, const wchar_t* pattern=nullptr ) const;

protected:

	__forceinline attrib_t& cache() const { return *((attrib_t*)(data+capacity)); }
	__forceinline bool		cache_exists() const { attrib_t* c=(attrib_t*)(data+capacity); return c->ftLastWriteTime.dwHighDateTime>0&&c->dwFileAttributes==INVALID_FILE_ATTRIBUTES; }
	void canonicalize(); // remove redundancies in directories (e.g., "../some/..", "./" )

	struct scan_t { std::vector<path> result; bool recursive; sized_ptr_t<wchar_t>* exts; size_t exts_l; const wchar_t* pattern; size_t plen; bool b_glob; };
	void scan_recursive( path dir, scan_t& si ) const;
	void subdirs_recursive( path dir, scan_t& si ) const;

	wchar_t* data;
};

//***********************************************
// definitions of long inline member functions

template<> __noinline sized_ptr_t<void> path::read_file<void>() const
{
	sized_ptr_t<void> p={nullptr,0};
	FILE* fp=_wfopen(data,L"rb"); if(!fp) return {nullptr,0};
	fseek(fp,0,SEEK_END); p.size=ftell(fp); fseek(fp,0,SEEK_SET);
	if(p.size){ fread(p.ptr=malloc(p.size+1),1,p.size,fp); ((char*)p.ptr)[p.size]=0; }
	fclose(fp);
	return p;
}

template<> __noinline sized_ptr_t<const void> path::read_file<const void>() const
{
	auto p=read_file<void>(); return {(const void*)p.ptr,p.size};
}

template<> __noinline sized_ptr_t<wchar_t> path::read_file<wchar_t>() const
{
	sized_ptr_t<wchar_t> p={nullptr,0};
	FILE* fp=_wfopen(data,L"r,ccs=UTF-8"); if(!fp) return {nullptr,0};
	fseek(fp,0,SEEK_END); size_t size0=ftell(fp); fseek(fp,0,SEEK_SET);
	if(!size0){ fclose(fp); return {nullptr,0}; }

	std::wstring buffer; buffer.reserve(size0*2);
	wchar_t buff[4096]; while(fgetws(buff,4096,fp)) buffer+=buff; fclose(fp);
	p.size = buffer.size();
	p.ptr = (wchar_t*)memcpy(malloc((p.size+1)*sizeof(wchar_t)),buffer.c_str(),p.size*sizeof(wchar_t));
	p.ptr[p.size]=0;
	return p;
}

template<> __noinline sized_ptr_t<const wchar_t> path::read_file<const wchar_t>() const
{
	auto p=read_file<wchar_t>(); return {(const wchar_t*)p.ptr,p.size};
}

template<> __noinline sized_ptr_t<char> path::read_file<char>() const
{
	sized_ptr_t<char> p={nullptr,0};
	FILE* fp=_wfopen(data,L"r"); if(!fp) return {nullptr,0};
	fseek(fp,0,SEEK_END); size_t size0=ftell(fp); fseek(fp,0,SEEK_SET);
	if(!size0){ fclose(fp); return {nullptr,0}; }

	std::string buffer; buffer.reserve(size0*2);
	char buff[4096]; while(fgets(buff,4096,fp)) buffer+=buff; fclose(fp);
	p.size = buffer.size();
	p.ptr = (char*)memcpy(malloc((p.size+1)*sizeof(char)),buffer.c_str(),p.size*sizeof(char));
	p.ptr[p.size]=0;
	return p;
}

template<> __noinline sized_ptr_t<const char> path::read_file<const char>() const
{
	auto p=read_file<char>(); return {(const char*)p.ptr,p.size};
}

__noinline bool path::write_file( const void* ptr, size_t size ) const
{
	FILE* fp=_wfopen(data,L"wb"); if(!fp) return false;
	size_t size_written = ptr&&size?fwrite(ptr,1,size,fp):0; fclose(fp);
	return size_written==size;
}

__noinline bool path::write_file( const char* s ) const
{
	FILE* fp=_wfopen(data,L"w"); if(!fp) return false;
	int ret = s?fputs(s,fp):0; fclose(fp);
	return ret>=0;
}

__noinline bool path::write_file( const wchar_t* s ) const
{
	FILE* fp=_wfopen(data,L"w,ccs=UTF-8"); if(!fp) return false;
	if(ext()!=L".sln") fseek(fp,0,SEEK_SET); // rewind to remove BOM for non-solution files; caution: vcxproj do not use BOM
	int ret = s?fputws(s,fp):0; fclose(fp);
	return ret>=0;
}

__noinline bool path::glob( const wchar_t* str, size_t slen, const wchar_t* pattern, size_t plen )
{
	static const wchar_t q=L'?', a=L'*';
	int n=int(slen?slen:wcslen(str)), m=int(plen?plen:wcslen(pattern)); if(m==0) return n==0;
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

__noinline bool path::iglob( const wchar_t* str, size_t slen, const wchar_t* pattern, size_t plen )
{
	static const wchar_t q=L'?', a=L'*';
	int n=int(slen?slen:wcslen(str)), m=int(plen?plen:wcslen(pattern)); if(m==0) return n==0;
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

__noinline std::vector<path> path::scan( char recursive, const wchar_t* ext_filter, const wchar_t* pattern ) const
{
	std::vector<std::wstring> exts; if(ext_filter&&ext_filter[0]){ wchar_t ef[4096]={}, *ctx=nullptr; wcscpy(ef,ext_filter); for(wchar_t* e=wcstok_s(ef,L";",&ctx);e;e=wcstok_s(nullptr,L";",&ctx)) if(e[0]) exts.push_back(std::wstring(L".")+e); }
	std::vector<sized_ptr_t<wchar_t>> eptr; for( auto& e:exts ) eptr.emplace_back(sized_ptr_t<wchar_t>{(wchar_t*)e.c_str(),e.size()});
	scan_t si={{},recursive!=0,eptr.size()>0?&eptr[0]:nullptr,eptr.size(),pattern,pattern?wcslen(pattern):0,pattern&&(wcschr(pattern,L'*')||wcschr(pattern,L'?')) };
	if(!is_dir()) return si.result; path src=(is_relative()?absolute(L".\\"):*this).add_backslash();
	si.result.reserve(1<<16);scan_recursive(src,si);si.result.shrink_to_fit();return si.result;
}

__noinline void path::scan_recursive( path dir, path::scan_t& si ) const
{
	WIN32_FIND_DATAW fd; HANDLE h=FindFirstFileExW(dir+L"*.*",FindExInfoBasic/*minimal(faster)*/,&fd,FindExSearchNameMatch,0,FIND_FIRST_EX_LARGE_FETCH); if(h==INVALID_HANDLE_VALUE) return;
	size_t dl=wcslen(dir.data); wchar_t *f=fd.cFileName, *p=dir.data+dl;
	std::vector<path> sdir; if(si.recursive) sdir.reserve(16);

	while(FindNextFileW(h,&fd))
	{
		size_t fl=wcslen(f);
		if((fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)==0)
		{
			if(si.exts){size_t e=0;for(;e<si.exts_l;e++)if(__wcsiext(f,fl,si.exts[e].ptr,si.exts[e].size))break;if(e==si.exts_l)continue;}
			if(si.plen){if(si.b_glob?!glob(f,fl,si.pattern,si.plen):__wcsistr(f,fl,si.pattern,si.plen)==nullptr)continue;}
			memcpy(p,f,sizeof(wchar_t)*fl); p[fl]=0; si.result.emplace_back(dir);
		}
		else if(si.recursive&&(fd.dwFileAttributes&FILE_ATTRIBUTE_HIDDEN)==0)
		{
			if(f[0]==L'.'){ if(f[1]==0||memcmp(f+1,L".",4)==0||memcmp(f+1,L"git",8)==0) continue; } // skip .git
			memcpy(p,f,sizeof(wchar_t)*fl); p[fl]=L'\\';p[fl+1]=0; sdir.emplace_back(dir);
		}
	}
	FindClose(h);
	for(auto& c:sdir) scan_recursive(c,si);
}

__noinline std::vector<path> path::subdirs( char recursive, const wchar_t* pattern ) const
{
	scan_t si={{},recursive!=0,0,0,pattern,pattern?wcslen(pattern):0,pattern&&(wcschr(pattern,L'*')||wcschr(pattern,L'?'))};
	if(!is_dir()) return si.result; path src=(is_relative()?absolute(L".\\"):*this).add_backslash();
	si.result.reserve(1<<12);subdirs_recursive(src,si);si.result.shrink_to_fit();return si.result;
}

__noinline void path::subdirs_recursive( path dir, path::scan_t& si ) const
{
	WIN32_FIND_DATAW fd; HANDLE h=FindFirstFileExW(dir+L"*.*",FindExInfoBasic/*minimal(faster)*/,&fd,FindExSearchNameMatch,0,FIND_FIRST_EX_LARGE_FETCH); if(h==INVALID_HANDLE_VALUE) return;
	size_t dl=wcslen(dir.data); wchar_t *f=fd.cFileName, *p=dir.data+dl;
	std::vector<path> sdir; if(si.recursive) sdir.reserve(16);

	while(FindNextFileW(h,&fd))
	{
		if((fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)==0) continue;
		if(f[0]==L'.'){ if(!f[1]||memcmp(f+1,L".",4)==0||memcmp(f+1,L"git",8)==0) continue; } // skip .git
		size_t fl=wcslen(f); memcpy(p,f,sizeof(wchar_t)*fl); p[fl]=L'\\';p[fl+1]=0;
		if(si.recursive) sdir.emplace_back(dir);
		if(si.pattern==0) si.result.emplace_back(dir);
		else if(!si.b_glob&&__wcsistr(f,fl,si.pattern,si.plen)) si.result.emplace_back(dir);
		else if(si.b_glob&&glob(f,fl,si.pattern,si.plen)) si.result.emplace_back(dir);
	}
	FindClose(h);
	for(auto& c:sdir) subdirs_recursive(c,si);
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

//***********************************************
// temp directories

inline path path::system::temp()
{
	static path t; if(!t.empty()) return t;
	GetTempPathW(path::capacity,t); t=t.absolute().add_backslash(); t[0]=::toupper(t[0]);
	if(t.volume().has_free_space()) return t;
	wprintf(L"temp(): not enough space in %s\n", t.data );
	t=path::module_dir()+L"temp\\"; if(!t.exists()) t.mkdir(); wprintf(L"temp(): %s is used instead", t.data );
	if(!t.volume().has_free_space()) wprintf(L", but still not enough space." );
	wprintf(L"\n");
	return t;
}

inline path path::global::temp( const char* subdir )
{
#ifdef REX_FACTORY_IMPL
	if(!subdir) subdir = ".rex\\";
#else
	if(!subdir) subdir = ".gxut\\";
#endif
	path t = path::system::temp();
	if(subdir&&subdir[0]) t+=path(subdir).add_backslash();
	return t;
}

//***********************************************
__noinline path path::temp( const wchar_t* subkey, const char* subdir )
{
	static path g=global::temp(subdir),mod=path(L"local\\")+module_dir().key().add_backslash();
	path key = (subkey&&subkey[0])?path(subkey).key().add_backslash():mod;
	path t = key.empty()?g:g+key; if(!t.exists()) t.mkdir();
	return t;
}

__noinline path path::relative( bool first_dot, const wchar_t* from ) const
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
	for(; d<ds; d++ ) result += dst_list[d]+L'\\';
	result.canonicalize();

	// 3. if empty dir, then attach ./
	if(first_dot&&(result[0]==0||result[0]!=L'.')) result=path(".\\")+result;

	return this->is_dir()?result:result+name();
}

__noinline void path::create_process( const wchar_t* arguments, bool bShowWindow, bool bWaitFinish ) const
{
	wchar_t cmd[4096]; swprintf_s( cmd, 4096, L"\"%s\" %s", data, arguments?arguments:L""  );
	PROCESS_INFORMATION pi={}; STARTUPINFOW si={}; si.cb=sizeof(si); si.dwFlags=STARTF_USESHOWWINDOW; si.wShowWindow=bShowWindow?SW_SHOW:SW_HIDE;
	CreateProcessW( nullptr, (LPWSTR)cmd, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi );
	if(bWaitFinish) WaitForSingleObject(pi.hProcess,INFINITE);
	CloseHandle( pi.hThread );    // must release handle
	CloseHandle( pi.hProcess );   // must release handle
}

__noinline void path::canonicalize()
{
	size_t len=data[0]?wcslen(data):0; if(len==0) return;
	for(uint k=0;k<len;k++) if(data[k]==L'/') data[k]=L'\\'; // slash to backslash
	if(data[len-1]==L'.'&&((len>2&&data[len-2]==L'\\')||(len>3&&data[len-3]==L'\\'&&data[len-2]==L'.'))){ data[len++]=L'\\'; data[len]=L'\0'; } // add trailing slash to "\\." or "\\.."
	if(len==2&&data[1]==L':'){ data[len++]=L'\\'; data[len]=L'\0'; } // root correction
	if(!wcsstr(data,L"\\.\\")&&!wcsstr(data,L"\\..\\")) return; // trivial return
	wchar_t* ds; while((ds=wcsstr(data+1,L"\\\\"))) memmove(ds+1,ds+2,((len--)-(ds-data)-1)*sizeof(wchar_t)); // correct multiple backslashes, except the beginning of unc path
	if(is_absolute()){ _wfullpath(data,(const wchar_t*)memcpy(__wcsbuf(),data,sizeof(wchar_t)*(len+1)),capacity); return; }

	// flags to check
	bool b_trailing_backslash = (data[len-1]==L'\\');
	bool b_single_dot_begin = data[0]==L'.'&&data[1]==L'\\';

	// perform canonicalization
	std::deque<wchar_t*> L;wchar_t* ctx;
	for(wchar_t* t=wcstok_s(wcscpy(__wcsbuf(),data),L"\\",&ctx);t;t=wcstok_s(nullptr,L"\\",&ctx))
	{
		if(t[0]==L'.'&&t[1]==L'.'&&!L.empty()&&memcmp(L.back(),L"..",sizeof(wchar_t)*2)!=0) L.pop_back();
		else if(t[0]!=L'.'||t[1]!=0) L.emplace_back(wcscpy(__wcsbuf(),t));
	}

	// reconstruction
	wchar_t* d=data;
	if(!b_single_dot_begin&&L.empty()){ d[0]=L'\0';return; }
	if(L.empty()||(b_single_dot_begin&&(!L.empty()&&L.front()[0]!=L'.'))){ (d++)[0]=L'.';(d++)[0]=L'\\';d[0]=L'\0'; } // make it begin with single-dot again
	for(size_t k=0,kn=L.size();k<kn;k++){ wchar_t* t=L[k];size_t l=wcslen(t);memcpy(d,t,l*sizeof(wchar_t));d+=l;(d++)[0]=L'\\';d[0]=L'\0'; }
	if(!b_trailing_backslash&&d>data)(d-1)[0]=L'\0';
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
// compiler utility
namespace gx { namespace compiler
{
	inline int mtoi( const char* month ){ if(!month||!month[0]||!month[1]||!month[2]) return 0; char a=tolower(month[0]), b=tolower(month[1]), c=tolower(month[2]); if(a=='j'){ if(b=='a') return 1; if(c=='n') return 6; return 7; } if(a=='f') return 2; if(a=='m'){ if(c=='r') return 3; return 5; } if(a=='a'){ if(b=='p') return 4; return 8; } if(a=='s') return 9; if(a=='o') return 10; if(a=='n') return 11; return 12; }
	inline int year(){ static int y=0; if(y) return y; char buff[64]; sscanf(__DATE__,"%*s %*s %s", buff); return y=atoi(buff); }
	inline int month(){ static int m=0; if(m) return m; char buff[64]={}; sscanf(__DATE__,"%s", buff); return m=mtoi(buff); }
	inline int day(){ static int d=0; if(d) return d; char buff[64]; sscanf(__DATE__,"%*s %s %*s", buff); return d=atoi(buff); }
}}

//***********************************************
// os implementations, having dependency to path
//***********************************************
namespace os { namespace env {
//***********************************************

inline const wchar_t* var( const wchar_t* key )
{
	static std::vector<wchar_t> buff(4096);
	size_t size_required = GetEnvironmentVariableW( key, &buff[0], DWORD(buff.size()) );
	if(size_required>buff.size()){ buff.resize(size_required); GetEnvironmentVariableW( key, &buff[0], DWORD(buff.size()) ); }
	return &buff[0];
}

inline const std::vector<path>& paths()
{
	static std::vector<path> v; v.reserve(64); if(!v.empty()) return v;
	wchar_t* buff = (wchar_t*) var( L"PATH" ); if(!buff||!*buff) return v;
	for(wchar_t *ctx,*token=wcstok_s(buff,L";",&ctx);token;token=wcstok_s(nullptr,L";",&ctx))
	{
		if(!*token) continue;
		path t=path(token).canonical().add_backslash();
		if(t.is_absolute()&&t.exists()) v.emplace_back(t);
	}
	return v;
}

inline path search( path file_name )
{
	for( const auto& e : paths() ) if((e+file_name).exists()) return e+file_name;
	return path();
}
//***********************************************
}} // namespace os::env
//***********************************************

//***********************************************
#endif // __GX_FILESYSTEM__
