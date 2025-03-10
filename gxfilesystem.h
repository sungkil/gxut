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

// include gxut.h without other headers
#ifndef __GXUT_H__
#pragma push_macro("__GXUT_EXCLUDE_HEADERS__")
#define __GXUT_EXCLUDE_HEADERS__
#include "gxut.h"
#pragma pop_macro("__GXUT_EXCLUDE_HEADERS__")
#endif

#include <time.h>
#include <deque>
#if __has_include(<shellapi.h>)
	#include <shellapi.h>
#endif

//***********************************************
// Win32-like filetime utilities
#ifdef _MINWINDEF_
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
#endif

struct path
{
	using value_type		= char; // follow posix-system definitions
	using string_type		= std::basic_string<value_type>;
	using iterator			= value_type*;
	using const_iterator	= const value_type*;
	using attrib_t			= WIN32_FILE_ATTRIBUTE_DATA; // auxiliary cache information from scan()
	static constexpr int capacity = PATH_MAX;

	// platform-specific definitions
	static constexpr value_type __slash = '/';
	static constexpr value_type __backslash = '\\';
#if defined(_WIN32)||defined(_WIN64)
	static constexpr value_type	preferred_separator = __backslash;
#else
	static constexpr value_type	preferred_separator = __slash;
#endif

protected:
	value_type* _data;
public:

	// destructor/constuctors
	__forceinline value_type* alloc(){ static constexpr size_t s=capacity*sizeof(value_type)+sizeof(attrib_t); _data=(value_type*)malloc(s); if(_data) _data[0]=0; return _data; }
	path() noexcept { alloc(); clear_cache(); }
	path( const path& p ) noexcept { strcpy(alloc(),p._data); cache()=p.cache(); }
	path( path&& p ) noexcept { _data=p._data; p._data=nullptr; } // cache moves as well
	path( const char* s ) noexcept : path() { if(s) strcpy(_data,s); }
	explicit path( const string_type& s ) noexcept : path() { strcpy(_data,s.c_str()); }
	~path() noexcept { if(_data){ free(_data); _data=nullptr; } }

	// operator overloading: assignment
	path& operator=( const path& p ) noexcept { strcpy(_data,p._data); cache()=p.cache(); return *this; }
	path& operator=( path&& p ) noexcept { if(_data) free(_data); _data=p._data; p._data=nullptr; return *this; }
	path& operator=( const char* s ) noexcept { if(s) strcpy(_data,s); return *this; }
	path& operator=( const string_type& s ) noexcept { strcpy(_data,s.c_str()); return *this; }

	// operator overloading: concatenations
	path& operator+=( const path& p ){ strcat(_data,p._data+((p._data[0]==value_type('.')&&p._data[1]==value_type(preferred_separator)&&p._data[2])?2:0)); return *this; }
	path& operator+=( const char* s ){ if(s[0]=='.'&&s[1]==__backslash&&s[2]) s+=2; size_t l=strlen(_data); strcpy(_data+l,s); return *this; }
	path& operator+=( const string_type& s ){ return operator+=(s.c_str()); }
	path& operator+=( char c ){ size_t l=strlen(_data); _data[l]=value_type(c); _data[l+1]=0; return *this; }

	path& operator/=( const path& p ){ return *this=add_preferred()+p; }
	path& operator/=( const char* s ){ return *this=add_preferred()+s; }
	path& operator/=( const string_type& s ){ return *this=add_preferred()+s.c_str(); }
	path& operator/=( char c ){ return *this=add_preferred()+c; }

	path operator+( const path& p ) const { return path(*this).operator+=(p); }
	path operator+( const char* s ) const { return path(*this).operator+=(s); }
	path operator+( const string_type& s ) const { return path(*this).operator+=(s.c_str()); }
	path operator+( char c ){ return path(*this).operator+=(c); }

	path operator/( const path& p ) const { return path(*this).operator/=(p); }
	path operator/( const char* s ) const { return path(*this).operator/=(s); }
	path operator/( const string_type& s ) const { return path(*this).operator/=(s.c_str()); }
	path operator/( char c ) const { return path(*this).operator/=(c); }

	// operator overloading: comparisons
	bool operator==( const path& p ) const { return _stricmp(_data,p._data)==0; }
	bool operator==( const char* s ) const { return _stricmp(_data,s)==0; }
	bool operator!=( const path& p ) const { return _stricmp(_data,p._data)!=0; }
	bool operator!=( const char* s ) const { return _stricmp(_data,s)!=0; }
	bool operator<( const path& p )	 const { return ::_strcmplogical(_data,p._data)<0; }
	bool operator>( const path& p )	 const { return ::_strcmplogical(_data,p._data)>0; }
	bool operator<=( const path& p ) const { return ::_strcmplogical(_data,p._data)<=0; }
	bool operator>=( const path& p ) const { return ::_strcmplogical(_data,p._data)>=0; }

	// operator overloading: array operator
	inline value_type& operator[]( ptrdiff_t i ){ return _data[i]; }
	inline const value_type& operator[]( ptrdiff_t i ) const { return _data[i]; }

	// operator overloading: casting and conversion
	value_type* data(){ return _data; }
	const value_type* c_str() const { return _data; }

	// iterators
	iterator begin() const { return _data; }
	iterator end() const { return _data+strlen(_data); }
	value_type& front() const { return *_data; }
	value_type& back() const { return *_data==0?*_data:_data[strlen(_data)-1]; }

	// size
	void clear(){ _data[0]=0; clear_cache(); }
	bool empty() const { return _data[0]==0; }
	size_t size() const { return strlen(_data); }

	// crc32c/md5 checksums of the file content
	inline uint crc() const;	// implemented in gxmemory.h
	inline uint4 md5() const;	// implemented in gxmemory.h

	// in-place transforms
	void make_preferred()	const {	value_type* t=_data; for(size_t k=0,l=strlen(t);k<l;k++,t++) if(*t==__slash) *t=__backslash; }
protected:
	void canonicalize(); // remove redundancies in directories (e.g., "../some/..", "./" )
public:

	// system-related: slash, backslash, unix, quote
	path to_preferred()		const { return preferred_separator==__backslash?to_backslash():to_slash(); }
	path to_backslash()		const {	path p(*this); value_type* t=p._data; for(size_t k=0,l=p.size();k<l;k++,t++) if(*t==__slash) *t=__backslash; return p; }
	path to_slash()			const {	path p(*this); value_type* t=p._data; for(size_t k=0,l=p.size();k<l;k++,t++) if(*t==__backslash) *t=__slash; return p; }
	path to_dot()			const {	path p(*this); value_type* t=p._data; for(size_t k=0,l=p.size();k<l;k++,t++) if(*t==__backslash||*t==__slash) *t='.'; return p; }
	path add_preferred()	const { return preferred_separator==__backslash?add_backslash():add_slash(); }
	path add_backslash()	const { path p(*this); size_t l=p.size(); if(l&&p._data[l-1]==__slash) p._data[l-1]=__backslash; else if(l&&p._data[l-1]!=__backslash){p._data[l]=__backslash;p._data[l+1]='\0';} return p; }
	path add_slash()		const { path p(*this); size_t l=p.size(); if(l&&p._data[l-1]==__backslash) p._data[l-1]=__slash; else if(l&&p._data[l-1]!=__slash){p._data[l]=__slash;p._data[l+1]='\0';} return p; }
	path remove_backslash()	const { path p(*this); size_t l=p.size(); if(l&&(p._data[l-1]==__backslash||p._data[l-1]==__slash)) p._data[l-1]='\0'; return p; }
	path remove_slash()		const { path p(*this); size_t l=p.size(); if(l&&(p._data[l-1]==__backslash||p._data[l-1]==__slash)) p._data[l-1]='\0'; return p; }
	path unix()				const {	path p(*this); p.canonicalize(); p=p.to_slash(); if(p.size()<2||p.is_relative()||p.is_unc()||p.is_rsync()||p.is_remote()) return p; if(p._data[1]==':'){ p._data[1]=value_type(tolower(p._data[0])); p._data[0]=__slash; } return p; }
	path cygwin()			const { path p(*this); p.canonicalize(); p=p.to_slash(); if(p.size()<2||p.is_relative()||p.is_unc()||p.is_rsync()||p.is_remote()) return p; path p2; snprintf( p2._data, capacity, "/cygdrive/%c%s", tolower(p[0]), p._data+2 ); return p2; }

	// split path
	struct split_t { value_type *drive, *dir, *fname, *ext; };
	__forceinline split_t split( bool drive=false, bool dir=false, bool fname=false, bool ext=false ) const
	{
		auto* r=drive?__strbuf<wchar_t>(_MAX_DRIVE):nullptr;
		auto* d=dir?__strbuf<wchar_t>(_MAX_DIR):nullptr;
		auto* n=fname?__strbuf<wchar_t>(_MAX_FNAME):nullptr;
		auto* x=ext?__strbuf<wchar_t>(_MAX_EXT):nullptr;
		_wsplitpath_s(atow(_data),r,r?_MAX_DRIVE:0,d,d?_MAX_DIR:0,n,n?_MAX_FNAME:0,x,x?_MAX_EXT:0);
		if(r&&*r) *r=value_type(toupper(*r));
		return split_t{(value_type*)wtoa(r),(value_type*)wtoa(d),(value_type*)wtoa(n),(value_type*)wtoa(x)};
	}

	// helpers for C-string functions
	bool find( const char* s, bool case_insensitive=true ){ return (case_insensitive?_stristr(_data,s):strstr(_data,s))!=nullptr; }

	// attribute by stats
	inline stat_t	stat() const { stat_t s={}; if(exists()) _stat64(_data,&s); return s; }
	inline DWORD&	attributes() const { update_cache(); return cache().dwFileAttributes; }
	inline void		update_cache() const { if(!*_data) return; auto& c=cache(); if(!GetFileAttributesExW(atow(_data),GetFileExInfoStandard,&c)||c.dwFileAttributes==INVALID_FILE_ATTRIBUTES){ memset(&c,0,sizeof(attrib_t)); c.dwFileAttributes=INVALID_FILE_ATTRIBUTES; return; } c.ftLastWriteTime=DiscardFileTimeMilliseconds(c.ftLastWriteTime); }
	inline void		clear_cache() const { attrib_t* a=(attrib_t*)(_data+capacity); memset(a,0,sizeof(attrib_t)); a->dwFileAttributes=INVALID_FILE_ATTRIBUTES; }
	inline uint64_t file_size() const { update_cache(); auto& c=cache(); if(c.dwFileAttributes==INVALID_FILE_ATTRIBUTES) return 0; return uint64_t(c.nFileSizeHigh)<<32ull|uint64_t(c.nFileSizeLow); }

	// set attributes
	void set_hidden( bool h ) const {	if(!*_data) return; auto& a=attributes(); if(a!=INVALID_FILE_ATTRIBUTES) SetFileAttributesW(atow(c_str()),a=h?(a|FILE_ATTRIBUTE_HIDDEN):(a^FILE_ATTRIBUTE_HIDDEN)); }
	void set_readonly( bool r ) const {	if(!*_data) return; auto& a=attributes(); if(a!=INVALID_FILE_ATTRIBUTES) SetFileAttributesW(atow(c_str()),a=r?(a|FILE_ATTRIBUTE_READONLY):(a^FILE_ATTRIBUTE_READONLY)); }
	void set_system( bool s ) const {	if(!*_data) return; auto& a=attributes(); if(a!=INVALID_FILE_ATTRIBUTES) SetFileAttributesW(atow(c_str()),a=s?(a|FILE_ATTRIBUTE_SYSTEM):(a^FILE_ATTRIBUTE_SYSTEM)); }

	// get attributes
	bool exists() const {		if(!*_data) return false; auto& a=attributes(); return a!=INVALID_FILE_ATTRIBUTES; }
	bool is_dir() const {		if(!*_data) return false; auto& a=attributes(); return a!=INVALID_FILE_ATTRIBUTES&&(a&FILE_ATTRIBUTE_DIRECTORY)!=0; }
	bool is_drive() const {		if(!*_data) return false; size_t l=size();		return is_dir()&&l<4&&l>1&&_data[1]==':'; }
	bool is_hidden() const {	if(!*_data) return false; auto& a=attributes(); return a!=INVALID_FILE_ATTRIBUTES&&(a&FILE_ATTRIBUTE_HIDDEN)!=0; }
	bool is_readonly() const {	if(!*_data) return false; auto& a=attributes(); return a!=INVALID_FILE_ATTRIBUTES&&(a&FILE_ATTRIBUTE_READONLY)!=0; }
	bool is_system() const {	if(!*_data) return false; auto& a=attributes(); return a!=INVALID_FILE_ATTRIBUTES&&(a&FILE_ATTRIBUTE_SYSTEM)!=0; }
	bool is_junction() const {	if(!*_data||is_drive()) return false;auto& a=attributes(); if(a==INVALID_FILE_ATTRIBUTES||(a&FILE_ATTRIBUTE_DIRECTORY)==0) return false; return ((a&FILE_ATTRIBUTE_REPARSE_POINT)!=0); }
	
	// non-local attributes
	bool is_unc() const {		return (_data[0]==__backslash&&_data[1]==__backslash)||(_data[0]==__slash&&_data[1]==__slash); }
	bool is_pipe() const {		if(!*_data) return false; return strcmp(_data,"-")==0||_strnicmp(_data,"pipe:",5)==0; }
	bool is_rsync() const {		auto* p=strstr(_data,":\\"); if(!p) p=strstr(_data,":/"); return p!=nullptr&&p>_data+1; }
	bool is_http_url() const {	if(!*_data) return false; return _strnicmp(_data,"http://",7)==0||_strnicmp(_data,"https://",8)==0; }
	bool is_ssh() const {		if(!*_data) return false; return strstr(_data+2,":\\")!=nullptr||strstr(_data+2,":/")!=nullptr; }
	bool is_synology() const {	if(!is_ssh()) return false; return strstr(_data,":\\volume")!=nullptr||strstr(_data,":/volume")!=nullptr; }
	bool is_remote() const {	return is_http_url()||is_ssh(); }

	// relative/absolute path
	inline bool is_subdir( const path& ancestor ) const { return _strnicmp(_data,ancestor._data,ancestor.size())==0; } // do not check existence
	inline bool is_absolute() const { if(!*_data) return false; return _data[1]==':'||is_unc()||is_rsync()||is_remote(); }
	inline bool is_relative() const { return !is_pipe()&&!is_absolute(); }
	inline path absolute( path base="" ) const { return !*_data||is_absolute()?*this:(_fullpath(__strbuf(capacity),(base.empty()||is_absolute())?_data:strcat(strcpy(__strbuf(capacity), path(base).add_backslash()._data), _data), capacity)); }	// do not directly return for non-canonicalized path
	inline path relative( path from="", bool first_dot=false ) const;
	inline path canonical() const { if(is_pipe()) return *this; if(is_rsync()||is_remote()) return to_slash(); path p(*this); p.canonicalize(); return p; } // not necessarily absolute: return relative path as well

	// path info/operations
	path drive() const { if(!*_data) return path();if(is_unc()) return unc_root(); return split(true).drive; }
	path dir() const { path p; value_type* d=__strbuf(capacity);if(is_unc()){path r=unc_root();size_t rl=r.size();if(size()<=rl+1){if(r._data[rl-1]!=__backslash){r._data[rl]=__backslash;r._data[rl+1]=0;}return r;}} auto s=split(true,true); if(!*s.dir&&!*s.drive) return ".\\"; strcpy(p._data,s.drive); if(*s.dir) strcat(p._data,s.dir); return p; }
	path unc_root() const { if(!is_unc()) return path(); path r=*this;size_t l=strlen(_data);for(size_t k=0;k<l;k++){if(r[k]==__slash)r[k]=__backslash;} auto* b=strpbrk(r._data+2,"\\/"); if(b)((value_type*)b)[0]=0; return r; } // similar to drive (but to the root unc path without backslash)
	path dir_name() const { return strpbrk(_data,"\\/")?dir().remove_backslash().filename():""; }
	path filename( bool with_ext=true ) const { auto s=split(false,false,true,with_ext); if(!with_ext||!s.ext||!*s.ext) return s.fname; return strcat(strcpy(__strbuf(capacity),s.fname),s.ext); }
	path ext() const { auto s=split(false,false,false,true); return *s.ext?s.ext+1:""; }
	path parent() const { return dir().remove_backslash().dir(); }
	vector<path> ancestors( path root="" ) const { if(empty()) return vector<path>(); if(root._data[0]==0) root=is_unc()?unc_root():exe::dir(); path d=dir(); int l=int(d.size()),rl=int(root.size()); bool r=_strnicmp(d._data,root._data,rl)==0; vector<path> a;a.reserve(4); for(int k=l-1,e=r?rl-1:0;k>=e;k--){ if(d._data[k]!=__backslash&&d._data[k]!=__slash) continue; d._data[k+1]=0; a.emplace_back(d); } return a; }
	path junction() const { path t; if(!*_data||!exists()) return t; bool b_dir=is_dir(),j=false; for(auto& d:b_dir?ancestors():dir().ancestors()){ if(d.is_drive()) break; if((d.attributes()&FILE_ATTRIBUTE_REPARSE_POINT)!=0){j=true;break;} } if(!j) return t; HANDLE h=CreateFileW(atow(c_str()), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, 0); if(h==INVALID_HANDLE_VALUE) return t; auto* w=__strbuf<wchar_t>(capacity); GetFinalPathNameByHandleW(h, w, t.capacity, FILE_NAME_NORMALIZED); CloseHandle(h); strcpy(t._data,wtoa(w)); if(strncmp(t._data, "\\\\?\\", 4)==0) t=path(t._data+4); return b_dir?t.add_backslash():t; }
	
	// shortcuts to C-string
	const char* name( bool with_ext=true ) const { return __strdup(filename(with_ext).c_str()); }
	const char* slash() const { return __strdup(to_slash().c_str()); }
	const char* auto_quote() const { if(!*_data||(_data[0]=='\"'&&_data[strlen(_data)-1]=='\"')) return c_str(); auto* t=__strbuf(capacity); size_t l=strlen(_data); memcpy(t,_data,l*sizeof(value_type)); if(t[l]==' '||t[l]=='\t'||t[l]=='\n') t[l]=0; if(t[0]==' '||t[0]=='\t'||t[0]=='\n') t++; if(t[0]!='-'&&!strpbrk(t," \t\n|&<>")) return c_str(); path p; p[0]='\"'; memcpy(p._data+1, _data, l*sizeof(value_type)); p[l+1]='\"'; p[l+2]=0; return __strdup(p.c_str()); }

	// content manipulations
	path remove_first_dot()	const { return (strlen(_data)>2&&_data[0]=='.'&&_data[1]==__backslash) ? path(_data+2) : *this; }
	path remove_extension() const { split_t si=split(true,true,true); auto* b=__strbuf(capacity); snprintf(b,capacity,"%s%s%s",si.drive, si.dir, si.fname); return b; }
	path replace_extension( path ext ) const { if(ext.empty()) return *this; split_t si=split(true,true,true); path p; snprintf(p._data, capacity, "%s%s%s%s%s", si.drive, si.dir, si.fname, ext[0]=='.'?"":".", ext._data); return p; }
	vector<path> explode( char delim=preferred_separator ) const { vector<path> L; path s=preferred_separator==__slash?to_slash():*this; L.reserve(16); value_type* ctx=nullptr; const value_type d[2]={delim,0}; for(value_type* t=strtok_s(s._data,d,&ctx); t; t=strtok_s(0,d,&ctx)) L.emplace_back(t); return L; }

	// directory attributes
	bool has_file( const path& file_name ) const { return is_dir()&&operator/(file_name).exists(); }

	// chdir/make/copy/delete file/dir operations
	bool mkdir() const; // make all super directories
	bool copy_file( path dst, bool overwrite=true ) const { if(!exists()||is_dir()||dst.empty()) return false; if(dst.is_dir()||dst.back()==__backslash) dst=dst.add_backslash()+name(); dst.dir().mkdir(); if(dst.exists()&&overwrite){ if(dst.is_hidden()) dst.set_hidden(false); if(dst.is_readonly()) dst.set_readonly(false); } return bool(CopyFileW( atow(c_str()), atow(dst.c_str()), overwrite?FALSE:TRUE)); }
	bool move_file( path dst ) const { return !*_data||!dst._data||is_dir()?false:(drive()==dst.drive()&&!dst.exists()) ? MoveFileW(atow(_data),atow(dst._data))!=0 : !copy_file(dst,true) ? false: rmfile(); }
#ifndef _INC_SHELLAPI
	bool delete_file() const { if(!exists()||is_dir()) return false; if(is_hidden()) set_hidden(false); if(is_readonly()) set_readonly(false); return DeleteFileW(_data)==TRUE; }
	bool rmfile() const { return delete_file(); }
	bool delete_dir() const { if(!is_dir()) return false; if(is_hidden()) set_hidden(false); if(is_readonly()) set_readonly(false); rmdir(_data); return true; }
	bool rmdir() const { return delete_dir(); }
#else
	bool delete_file( bool b_undo=false ) const { if(!exists()||is_dir()) return false; if(is_hidden()) set_hidden(false); if(is_readonly()) set_readonly(false); SHFILEOPSTRUCTW fop={};fop.wFunc=FO_DELETE;fop.fFlags=FOF_FILESONLY|FOF_SILENT|FOF_NOCONFIRMATION|(b_undo?FOF_ALLOWUNDO:0); auto* w=atow(c_str()); auto* b=wcscpy(__strbuf<wchar_t>(wcslen(w)+1),w); fop.pFrom=b; return SHFileOperationW(&fop)==0;}
	bool rmfile( bool b_undo=false ) const { return delete_file(b_undo); }
	bool delete_dir( bool b_undo=true ) const { if(!exists()||!is_dir()) return false; if(is_hidden()) set_hidden(false); if(is_readonly()) set_readonly(false); SHFILEOPSTRUCTW fop={};fop.wFunc=FO_DELETE;fop.fFlags=FOF_SILENT|FOF_NOCONFIRMATION|(b_undo?FOF_ALLOWUNDO:0); auto* w=atow(c_str()); auto* b=wcscpy(__strbuf<wchar_t>(wcslen(w)+1),w); fop.pFrom=b; return SHFileOperationW(&fop)==0;}
	bool rmdir( bool b_undo=true ) const { return delete_dir(b_undo); }
	bool copy_dir( path dst, bool overwrite=true ) const { if(!is_dir()) return false;value_type* from=__strbuf(capacity);snprintf(from,capacity,"%s\\*\0",_data);dst[dst.size()+1]='\0'; SHFILEOPSTRUCTW fop={};fop.wFunc=FO_COPY;fop.fFlags=FOF_ALLOWUNDO|FOF_SILENT|FOF_NOCONFIRMATION; fop.pFrom=atow(path(from).c_str());fop.pTo=atow(dst.c_str()); return SHFileOperationW(&fop)==0; }
	bool move_dir( path dst ) const { return !exists()||!is_dir()?false:(drive()==dst.drive()&&!dst.exists()) ? MoveFileW(atow(_data),atow(dst._data))!=0 : !copy_dir(dst,true) ? false: rmdir(); }
	void open( const char* args=nullptr, bool b_show_window=true ) const { if(!*_data) return; ShellExecuteW(GetDesktopWindow(),L"Open",atow(auto_quote()), args?atow(args):nullptr, nullptr, b_show_window?SW_SHOW:SW_HIDE); }
	void open_dir() const { dir().open(nullptr,true); }
#endif

	// time stamp
	static const char* timestamp( const struct tm* t ){char* buff=__strbuf(capacity);sprintf(buff,"%04d%02d%02d%02d%02d%02d",t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);return buff;}
	static const char* timestamp( FILETIME f ){ SYSTEMTIME s;FileTimeToSystemTime(&f,&s);char* buff=__strbuf(capacity);sprintf(buff,"%04d%02d%02d%02d%02d%02d",s.wYear,s.wMonth,s.wDay,s.wHour,s.wMinute,s.wSecond);return buff; }
	const char* ctimestamp() const { if(!exists()) return ""; stat_t s=stat(); struct tm t; _gmtime64_s(&t,&s.st_ctime); return timestamp(&t); }
	const char* atimestamp() const { if(!exists()) return ""; stat_t s=stat(); struct tm t; _gmtime64_s(&t,&s.st_atime); return timestamp(&t); }
	const char* mtimestamp() const { if(!exists()) return ""; stat_t s=stat(); struct tm t; _gmtime64_s(&t,&s.st_mtime); return timestamp(&t); }

	FILETIME cfiletime() const { update_cache(); return cache().ftCreationTime; }
	FILETIME afiletime() const { update_cache(); return cache().ftLastAccessTime; }
	FILETIME mfiletime() const { update_cache(); return cache().ftLastWriteTime; }
	SYSTEMTIME csystemtime() const { return FileTimeToSystemTime(cfiletime()); }
	SYSTEMTIME asystemtime() const { return FileTimeToSystemTime(afiletime()); }
	SYSTEMTIME msystemtime() const { return FileTimeToSystemTime(mfiletime()); }
	void set_filetime( const FILETIME* ctime, const FILETIME* atime, const FILETIME* mtime ) const { if(!exists()) return; HANDLE h=CreateFileW(atow(c_str()),FILE_WRITE_ATTRIBUTES,0,nullptr,OPEN_EXISTING,0,nullptr); if(!h)return; auto& c=cache(); if(ctime) c.ftCreationTime=*ctime; if(atime) c.ftLastAccessTime=*atime; if(mtime) c.ftLastWriteTime=*mtime; SetFileTime(h, ctime, atime, mtime ); CloseHandle(h); }
	void set_filetime( FILETIME f ) const { set_filetime(&f,&f,&f); }
	void set_filetime( const path& other ) const { if(!other.exists()) return; other.update_cache(); auto& c=other.cache(); set_filetime(&c.ftCreationTime,&c.ftLastAccessTime,&c.ftLastWriteTime); }

	// file content access: void (rb/wb), char (r/w), wchar_t (r/w,ccs=UTF-8)
	FILE* fopen( const char* mode, bool utf8=false ) const;
	template <class T=void> sized_ptr_t<T> read_file() const;
	string read_file() const;
	bool write_file( const void* ptr, size_t size ) const;
	bool write_file( sized_ptr_t<void> p ) const { return p.ptr&&p.size?write_file(p.ptr,p.size):false; }
	bool write_file( const char* s ) const;
	bool write_file( const wchar_t* s ) const;
	bool is_binary_file() const	{ FILE* f=fopen("rb"); if(!f) return false; char b[4096]; while(1){ size_t n=fread(b, 1, sizeof(b), f); if(!n) break; if(memchr(b, 0, n)){ fclose(f); return true; } } fclose(f); return false; }

	// temporary directories
	static path temp( bool local=true, path local_dir="" ); // local_dir is used only when local is true

	// utilities
	static path serial( path dir, const char* prefix, const char* postfix, int numzero=4 );

	// scan(): ext_filter (specific extensions delimited by semicolons), str_filter (path should contain this string)
	template <bool recursive=true> vector<path> scan( const char* ext_filter=nullptr, const char* pattern=nullptr, bool b_subdirs=false ) const;
	template <bool recursive=true> vector<path> subdirs( const char* pattern=nullptr ) const;

protected:
	__forceinline attrib_t& cache() const { return *((attrib_t*)(_data+capacity)); }
#if defined _M_IX86
	static size_t file_size( FILE* fp ){ if(!fp) return 0; auto p=ftell(fp); fseek(fp,0,SEEK_END); size_t s=ftell(fp); fseek(fp,p,SEEK_SET); return s; }
#elif defined(_M_X64)||defined(__LP64__)
	static size_t file_size( FILE* fp ){ if(!fp) return 0; auto p=_ftelli64(fp); _fseeki64(fp,0,SEEK_END); size_t s=_ftelli64(fp); _fseeki64(fp,p,SEEK_SET); return s; }
#endif
	
	struct scan_t { vector<path> result; struct { bool recursive, subdirs, dir, glob; } b; struct { sized_ptr_t<wchar_t>* v; size_t l; } ext; struct { const wchar_t* pattern; size_t l; } glob; };
	void scan_recursive( path dir, scan_t& si ) const;
	void subdirs_recursive( path dir, scan_t& si ) const;
};

//***********************************************
// definitions of long inline member functions

__noinline bool path::mkdir() const // make all super directories
{
	if(exists()) return false;
	auto v=to_backslash().dir().ancestors(); if(v.empty()) return false; auto bl=v.back().size();
	if(is_unc()){ auto r=dir().unc_root();size_t rl=r.size();if(bl<=rl+1){v.pop_back();bl=v.back().size();}if(bl<=rl+1)v.pop_back(); }
	else if(bl<=3){ if(v.back()[1]==':')v.pop_back();else if(bl<=1)v.pop_back(); }
	for( auto it=v.rbegin(); it!=v.rend(); it++ ){ if(!it->exists()&&::mkdir(it->_data)!=0) return false; }
	return true;
}

__noinline FILE* path::fopen( const char* mode, bool utf8 ) const
{
	char m[32]={0}; snprintf(m,32,"%s%s",mode,utf8?",ccs=UTF-8":"");
	FILE* fp = ::fopen(_data,m); if(!fp) return nullptr;
	else if(strcmp(mode,"w")==0&&utf8&&ext()!="sln") _fseeki64(fp,0,SEEK_SET); // remove byte order mask (BOM); sln use BOM, but vcxproj do not use BOM
	return fp;
}

template<> __noinline sized_ptr_t<void> path::read_file<void>() const
{
	sized_ptr_t<void> p={nullptr,0};
	FILE* fp=fopen("rb",false); if(!fp) return {nullptr,0};
	p.size = file_size(fp); if(!p.size){ fclose(fp); return {nullptr, 0}; }
	p.ptr=malloc(p.size+1); if(p.ptr) fread(p.ptr,1,p.size,fp); ((char*)p.ptr)[p.size]=0;
	fclose(fp);
	return p;
}

template<> __noinline sized_ptr_t<wchar_t> path::read_file<wchar_t>() const
{
	sized_ptr_t<wchar_t> p={nullptr,0};
	FILE* fp=fopen("r",true); if(!fp) return {nullptr,0};
	size_t size0 = file_size(fp); if(!size0){ fclose(fp); p.ptr=(wchar_t*)L""; return p; }

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
	FILE* fp=fopen("r"); if(!fp) return {nullptr,0};
	size_t size0 = file_size(fp); if(!size0){ fclose(fp); p.ptr=(char*)""; return p; }

	string buffer; buffer.reserve(size0*2);
	char buff[4096]; while(fgets(buff,4096,fp)) buffer+=buff; fclose(fp);
	p.size = buffer.size();
	p.ptr = (char*) malloc((p.size+2)*sizeof(char));
	if(p.ptr) memcpy(p.ptr,buffer.c_str(),p.size*sizeof(char));
	p.ptr[p.size]=p.ptr[p.size+1]=0; // two more bytes for null-ended wchar_t string
	return p;
}

__noinline string path::read_file() const
{
	sized_ptr_t<char> p=read_file<char>(); if(!p||!p.size) return "";
	string s=p.ptr; free(p.ptr); return s;
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
	FILE* fp=fopen("wb"); if(!fp) return false;
	size_t size_written = ptr&&size?fwrite(ptr,1,size,fp):0; fclose(fp);
	return size_written==size;
}

__noinline bool path::write_file( const char* s ) const
{
	FILE* fp=fopen("w"); if(!fp) return false;
	int ret = s&&*s?fputs(s,fp):0; fclose(fp);
	return ret>=0;
}

__noinline bool path::write_file( const wchar_t* s ) const
{
	FILE* fp=fopen("w",true); if(!fp) return false;
	int ret = s&&*s?fputws(s,fp):0; fclose(fp);
	return ret>=0;
}

template <bool recursive> __noinline
vector<path> path::scan( const char* ext_filter, const char* pattern, bool b_subdirs ) const
{
	path src=empty()?path(".\\"):(is_relative()?absolute(".\\"):*this).add_backslash(); if(!src.is_dir()) return vector<path>{};
	vector<std::wstring> exts; if(ext_filter&&ext_filter[0]){ value_type ef[4096]={}, *ctx=nullptr; strcpy(ef,ext_filter); for(value_type* e=strtok_s(ef,";",&ctx);e;e=strtok_s(nullptr,";",&ctx)) if(e[0]) exts.push_back(L"."s+atow(e)); }
	vector<sized_ptr_t<wchar_t>> eptr; for( auto& e:exts ) eptr.emplace_back(sized_ptr_t<wchar_t>{(wchar_t*)e.c_str(),e.size()});
	scan_t si;
	si.b.recursive=recursive; si.b.subdirs=b_subdirs; si.b.dir=recursive||b_subdirs; si.b.glob=pattern&&strpbrk(pattern,"*?");
	si.ext.v=eptr.size()>0?&eptr[0]:nullptr; si.ext.l=eptr.size();
	std::wstring wpattern=pattern?atow(pattern):L""; si.glob.pattern=pattern?wpattern.c_str():nullptr; si.glob.l=pattern?wpattern.size():0;
	si.result.reserve(1ull<<16);scan_recursive(src,si);si.result.shrink_to_fit();
	return si.result;
}

__noinline void path::scan_recursive( path dir, path::scan_t& si ) const
{
	wchar_t wdir[capacity]={}; wcscpy(wdir,atow(dir._data));
	WIN32_FIND_DATAW fd={}; HANDLE h=FindFirstFileExW(atow((dir+"*.*")._data), FindExInfoBasic/*minimal(faster)*/, &fd, FindExSearchNameMatch, 0, FIND_FIRST_EX_LARGE_FETCH); if(h==INVALID_HANDLE_VALUE) return;
	size_t dl=wcslen(wdir); wchar_t *f=fd.cFileName, *p=wdir+dl; auto*e=si.ext.v;
	vector<path> sdir; if(si.b.recursive) sdir.reserve(16);

	while(FindNextFileW(h,&fd))
	{
		size_t fl=wcslen(f);
		if((fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)==0) // files
		{
			if(e){size_t j=0;for(;j<si.ext.l;j++){if(e[j].size<fl&&_wcsicmp(e[j],f+fl-e[j].size)==0)break;}if(j==si.ext.l)continue;}
			if(si.glob.l){if(si.b.glob?!iglob(f,fl,si.glob.pattern,si.glob.l):_wcsistr(f,fl,si.glob.pattern,si.glob.l)==nullptr)continue;}
			memcpy(p,f,sizeof(wchar_t)*fl); p[fl]=0; si.result.emplace_back(wtoa(wdir));
		}
		else if((fd.dwFileAttributes&FILE_ATTRIBUTE_HIDDEN)==0&&si.b.dir)
		{
			if(f[0]==L'.'){ if(f[1]==0||(f[1]==L'.'&&f[2]==0)||memcmp(f+1, L"git", sizeof(wchar_t)*4)==0) continue; } // skip .git
			memcpy(p,f,sizeof(wchar_t)*fl); p[fl]=__backslash; p[fl+1]=0; sdir.emplace_back(wtoa(wdir));
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
vector<path> path::subdirs( const char* pattern ) const
{
	path src=empty()?path(".\\"):(is_relative()?absolute(".\\"):*this).add_backslash(); if(!src.is_dir()) return vector<path>{};
	scan_t si;
	si.b.recursive=recursive; si.b.subdirs=true; si.b.dir=true; si.b.glob=pattern&&strpbrk(pattern,"*?");
	si.ext.v=nullptr; si.ext.l=0;
	std::wstring wpattern=pattern?atow(pattern):L""; si.glob.pattern=pattern?wpattern.c_str():nullptr; si.glob.l=pattern?wpattern.size():0;
	si.result.reserve(1ull<<12);subdirs_recursive(src,si);si.result.shrink_to_fit();
	return si.result;
}

__noinline void path::subdirs_recursive( path dir, path::scan_t& si ) const
{
	wchar_t wdir[capacity]={}; wcscpy(wdir,atow(dir._data));
	WIN32_FIND_DATAW fd={}; HANDLE h=FindFirstFileExW(atow((dir+"*.*")._data), FindExInfoBasic/*minimal(faster)*/, &fd, FindExSearchNameMatch, 0, FIND_FIRST_EX_LARGE_FETCH); if(h==INVALID_HANDLE_VALUE) return;
	size_t dl=wcslen(wdir); wchar_t *f=fd.cFileName, *p=wdir+dl;
	vector<path> sdir; if(si.b.recursive) sdir.reserve(16);

	while(FindNextFileW(h,&fd))
	{
		if((fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)==0) continue;
		if(f[0]==L'.'){ if(!f[1]||(f[1]==L'.'&&f[2]==0)||memcmp(f+1,L"git",sizeof(wchar_t)*4)==0) continue; } // skip .git
		size_t fl=wcslen(f); memcpy(p,f,sizeof(value_type)*fl); p[fl]=__backslash;p[fl+1]=0;
		if(si.b.recursive) sdir.emplace_back(wtoa(wdir));
		if(si.glob.l==0) si.result.emplace_back(wtoa(wdir));
		else if(!si.b.glob&&_wcsistr(f,fl,si.glob.pattern,si.glob.l)) si.result.emplace_back(wtoa(wdir));
		else if(si.b.glob&&iglob(f,fl,si.glob.pattern,si.glob.l)) si.result.emplace_back(wtoa(wdir));
	}
	FindClose(h);
	if(si.b.recursive){ for(auto& c:sdir) subdirs_recursive(c,si); }
}

__noinline path path::serial( path dir, const char* prefix, const char* postfix, int numzero )
{
	dir=dir.add_backslash(); if(!dir.exists()) dir.mkdir();
	int nMaxFiles=1; for(int k=0;k<numzero;k++) nMaxFiles*=10;
	char fmt[capacity]={}; snprintf(fmt,path::capacity,"%s%s%%0%dd%s%s",dir.c_str(),prefix,numzero,postfix&&postfix[0]?".":"",postfix);
	char buff[capacity]={}; for(int k=0;k<nMaxFiles;k++){snprintf(buff,capacity,fmt,k); if(!path(buff).exists()) break; }
	return buff;
}

__noinline path path::temp( bool local, path local_dir )
{
	// get local_appdata
	char* buff=getenv("LOCALAPPDATA");
	static path r=path(buff?buff:"").add_backslash()+path_key(exe::name())+__backslash;
	path t=r; if(local){ if(local_dir.empty()) local_dir=exe::dir(); t+=path("local\\")+::add_backslash(path_key(local_dir.c_str())); }
	if(!t.exists()) t.mkdir();
	return t;
}

__noinline path path::relative( path from, bool first_dot ) const
{
	if(is_pipe()||is_rsync()||is_remote()) return *this;
	if(is_relative()) return !first_dot?this->remove_first_dot():*this;

	path from_dir = from.empty() ? cwd() : path(from).dir().absolute();
	if(::tolower(from_dir[0])!=::tolower(this->_data[0])) return *this; // different drive

	// 1. keep filename, make the list of reference and current path
	vector<path> src_list=from_dir.explode(), dst_list=dir().explode();

	// 2. compare and count the different directory levels
	path result;
	size_t s=0,d=0,ss=src_list.size(),ds=dst_list.size();
	for(; s<ss&&d<ds; s++, d++ ){ if(_stricmp(src_list[s]._data,dst_list[d]._data)!=0) break; }
	for(; s<ss; s++ ) result += "..\\";
	for(; d<ds; d++ ) result += dst_list[d]+__backslash;
	result.canonicalize();

	// 3. if empty dir, then attach ./
	if(first_dot){ if(result[0]==0||result[0]!='.') result=path(".\\")+result; }

	value_type b=back();
	return b==__backslash||b==__slash?result:result+name();
}

__noinline void path::canonicalize()
{
	size_t len=*_data?strlen(_data):0; if(!len) return;
	for(uint k=0;k<len;k++) if(_data[k]==__slash) _data[k]=__backslash; // slash to backslash
	if(_data[len-1]=='.'&&((len>2&&_data[len-2]==__backslash)||(len>3&&_data[len-3]==__backslash&&_data[len-2]=='.'))){ _data[len++]=__backslash; _data[len]='\0'; } // add trailing slash to "\\." or "\\.."
	if(len==2&&_data[1]==':'){ _data[len++]=__backslash; _data[len]='\0'; } // root correction
	if(!strstr(_data,"\\.\\")&&!strstr(_data,"\\..\\")) return; // trivial return
	value_type* ds; while((ds=(value_type*)strstr(_data+1,"\\\\"))) memmove(ds+1,ds+2,((len--)-(ds-_data)-1)*sizeof(value_type)); // correct multiple backslashes, except the beginning of unc path
	if(is_absolute()){ auto* r=_fullpath(_data,(const value_type*)memcpy(__strbuf(capacity),_data,sizeof(value_type)*(len+1)),capacity); return; }

	// flags to check
	bool b_trailing_backslash = (_data[len-1]==__backslash);
	bool b_single_dot_begin = _data[0]=='.'&&_data[1]==__backslash;

	// perform canonicalization
	std::deque<value_type*> L;value_type* ctx=nullptr; char delim[]={__backslash,0};
	for(value_type* t=strtok_s(strcpy(__strbuf(capacity),_data),delim,&ctx);t;t=strtok_s(nullptr,delim,&ctx))
	{
		if(t[0]=='.'&&t[1]=='.'&&!L.empty()&&memcmp(L.back(),"..",sizeof(value_type)*2)!=0) L.pop_back();
		else if(t[0]!='.'||t[1]!=0) L.emplace_back(strcpy(__strbuf(capacity),t));
	}

	// reconstruction
	value_type* d=_data;
	if(!b_single_dot_begin&&L.empty()){ d[0]='\0';return; }
	if(L.empty()||(b_single_dot_begin&&(!L.empty()&&L.front()[0]!='.'))){ (d++)[0]='.';(d++)[0]=__backslash;d[0]='\0'; } // make it begin with single-dot again
	for(size_t k=0,kn=L.size();k<kn;k++){ value_type* t=L[k];size_t l=strlen(t);memcpy(d,t,l*sizeof(value_type));d+=l;(d++)[0]=__backslash;d[0]='\0'; }
	if(!b_trailing_backslash&&d>_data)(d-1)[0]='\0';
}

//***********************************************
// nocase/std map/unordered_map extension for path
namespace std
{
	template <> struct hash<path>{ size_t operator()(const path& p)const{ return std::hash<string>()(_strlwr(__strdup(p.c_str()))); }};
}

namespace nocase
{
	template <> struct less<path>{ bool operator()(const path& a,const path& b)const{return a<b;}};
	template <> struct equal_to<path>{ bool operator()(const path& a,const path& b)const{return _stricmp(a.c_str(),b.c_str())==0; } };
	template <> struct hash<path>{ size_t operator()(const path& p)const{ return std::hash<string>()(_strlwr(__strdup(p.c_str()))); } };
}

// disk volume type for windows
#ifdef _APISETFILE_
struct volume_t
{
	static const int	capacity = 256;
	wchar_t				root[4]={}; // trailing backslash required
	wchar_t				name[capacity] = {};
	unsigned long		serial_number=0;
	unsigned long		maximum_component_length=0;
	uint64_t			_disk_size=0, _free_space=0;
	struct {			unsigned long flags=0; wchar_t name[capacity+1]={}; } filesystem;

	// constructor
	volume_t() = default;
	volume_t( const volume_t& other ) = default;
	volume_t& operator=( const volume_t& other ) = default;
	volume_t( const path& file_path )
	{
		if(file_path.is_unc()||file_path.is_rsync()||file_path.is_remote()) return;
		path drive = file_path.drive(); if(drive.empty()||!isalpha(drive[0])||!drive.exists()) return;
		root[0]=drive[0]; root[1]=L':'; root[2]=L'\\'; root[3]=0;
		if(!GetVolumeInformationW(root,name,capacity,&serial_number,&maximum_component_length,&filesystem.flags,filesystem.name,capacity)){ root[0]=0; return; }
		ULARGE_INTEGER a,t,f; GetDiskFreeSpaceExW(root, &a, &t, &f);
		_disk_size = uint64_t(t.QuadPart);
		_free_space = uint64_t(a.QuadPart);
	}

	// query
	bool exists() const { return root[0]!=0&&serial_number!=0&&filesystem.name[0]!=0; }
	uint64_t size() const { return exists()?_disk_size:0; }
	uint64_t free_space() const { return exists()?_free_space:0; }
	bool has_free_space( uint64_t inverse_thresh=10 ) const { return exists()&&free_space()>(size()/inverse_thresh); }
	bool is_exfat() const { return _wcsicmp(filesystem.name,L"exFAT")==0; }
	bool is_ntfs() const { return _wcsicmp(filesystem.name,L"NTFS")==0; }
	bool is_fat32() const { return _wcsicmp(filesystem.name,L"FAT32")==0; }
};
#endif

//***********************************************
#endif // __GX_FILESYSTEM_H__
