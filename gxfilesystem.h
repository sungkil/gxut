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

#if defined __msvc__ && __has_include(<shellapi.h>)
	#include <shellapi.h>
#endif

// early definitions
inline int chdir( const path_t& dir ){ return ::chdir(dir.c_str()); }

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

// extension/overriding of minimal path type path_t
struct path : public path_t
{
	// reuse constuctors/operators
	using path_t::path_t;
	using path_t::operator=;
	using path_t::operator+;
	using path_t::operator/;
	using path_t::operator+=;
	using path_t::operator/=;
	using path_t::operator==;
	using path_t::operator!=;
	
	// additional constructors
	path( const path_t& p ) noexcept : path_t(p){}
	path( path_t&& p ) noexcept : path_t(p){}
	path( const path& p ) noexcept : path_t(reinterpret_cast<const path_t&>(p)){}
	path( path&& p ) noexcept { _data=p._data; p._data=nullptr; } // cache moves as well
	path( const wchar_t* s ) noexcept : path_t() { if(s) strcpy(_data,wtoa(s)); }
	path( const std::wstring& s ) noexcept : path_t() { if(!s.empty()) strcpy(_data,wtoa(s.c_str())); }
	path( std::wstring_view s ) noexcept : path_t() { if(!s.empty()) strcpy(_data,wtoa(s.data())); }

	// operator overloading: casting and conversion
	operator path_t& (){ return *this; }
	operator const path_t& () const { return *this; }

	// operator overloading: assignment
	path& operator=( path&& p ) noexcept { return reinterpret_cast<path&>(operator=(reinterpret_cast<path_t&&>(p))); }
	path& operator=( const path& p ) noexcept { return reinterpret_cast<path&>(operator=(reinterpret_cast<const path_t&&>(p))); }

	// operator overloading: concatenations
	path operator+( const path& p ) const { return path(this)+=p; }
	path operator/( const path& p ) const { return append_slash()+p; }
	path& operator+=( const path& p ){ return reinterpret_cast<path&>(operator+=(reinterpret_cast<const path_t&>(p))); }
	path& operator/=( const path& p ){ return reinterpret_cast<path&>(operator/=(reinterpret_cast<const path_t&>(p))); }
	bool operator==( const path& p ) const { return stricmp(_data,p._data)==0; }
	bool operator!=( const path& p ) const { return stricmp(_data,p._data)!=0; }

	bool operator<( const path& p ) const { return strcmplogical(_data,p._data)<0; }
	bool operator<( const value_type* s ) const { return strcmplogical(_data,s)<0; }
	bool operator<( const string_type& s ) const { return strcmplogical(_data,s.c_str())<0; }
	bool operator<( string_view_type s ) const { return strcmplogical(_data,s.data())<0; }
	bool operator>( const path& p ) const { return strcmplogical(_data,p._data)>0; }
	bool operator>( const value_type* s ) const { return strcmplogical(_data,s)>0; }
	bool operator>( const string_type& s ) const { return strcmplogical(_data,s.c_str())>0; }
	bool operator>( string_view_type s ) const { return strcmplogical(_data,s.data())>0; }
	bool operator<=( const path& p ) const { return strcmplogical(_data,p._data)<=0; }
	bool operator<=( const value_type* s ) const { return strcmplogical(_data,s)<=0; }
	bool operator<=( const string_type& s ) const { return strcmplogical(_data,s.c_str())<=0; }
	bool operator<=( string_view_type s ) const { return strcmplogical(_data,s.data())<=0; }
	bool operator>=( const path& p ) const { return strcmplogical(_data,p._data)>=0; }
	bool operator>=( const value_type* s ) const { return strcmplogical(_data,s)>=0; }
	bool operator>=( const string_type& s ) const { return strcmplogical(_data,s.c_str())>=0; }
	bool operator>=( string_view_type s ) const { return strcmplogical(_data,s.data())>=0; }

	// separator opertions
	path to_slash()			const { return __super::to_slash(); }
	path to_backslash()		const { return __super::to_backslash(); }
	path to_preferred()		const { return __super::to_preferred(); }
	path append_slash()		const { return __super::append_slash(); }
	path prepend_dot()		const { return __super::prepend_dot(); }
	path trim_slash()		const { return __super::trim_slash(); }
	path trim_dot()			const { return __super::trim_dot(); }

	// additional separator opertions
	path unix()		const {	path p(*this); p.__canonicalize(); p=p.to_slash(); if(p.size()<2||p.is_relative()||p.is_unc()||p.is_remote()) return p; if(p._data[1]==':'){ p._data[1]=value_type(tolower(p._data[0])); p._data[0]='/'; } return p; }
	path cygwin()	const { path p(*this); p.__canonicalize(); p=p.to_slash(); if(p.size()<2||p.is_relative()||p.is_unc()||p.is_remote()) return p; path p2; snprintf( p2._data, capacity, "/cygdrive/%c%s", tolower(p[0]), p._data+2 ); return p2; }

	// helpers for C-string functions
	bool find( const char* s, bool case_insensitive=true ){ return (case_insensitive?stristr(_data,s):strstr(_data,s))!=nullptr; }

	// windows-only attributes
	uint wattrib() const {		if(!*_data) return INVALID_FILE_ATTRIBUTES; WIN32_FILE_ATTRIBUTE_DATA a; if(!GetFileAttributesExA(_data,GetFileExInfoStandard,&a)||a.dwFileAttributes==INVALID_FILE_ATTRIBUTES) return INVALID_FILE_ATTRIBUTES; return a.dwFileAttributes; }
	bool is_hidden() const {	if(!*_data) return false; auto a=wattrib(); return a!=INVALID_FILE_ATTRIBUTES&&(a&FILE_ATTRIBUTE_HIDDEN)!=0; }
	bool is_system() const {	if(!*_data) return false; auto a=wattrib(); return a!=INVALID_FILE_ATTRIBUTES&&(a&FILE_ATTRIBUTE_SYSTEM)!=0; }
	bool is_junction() const;
	void set_hidden( bool h ) const {	if(!*_data) return; auto a=wattrib(); if(a!=INVALID_FILE_ATTRIBUTES) SetFileAttributesA(_data,a=h?(a|FILE_ATTRIBUTE_HIDDEN):(a^FILE_ATTRIBUTE_HIDDEN)); }
	void set_readonly( bool r ) const {	if(!*_data) return; auto a=wattrib(); if(a!=INVALID_FILE_ATTRIBUTES) SetFileAttributesA(_data,a=r?(a|FILE_ATTRIBUTE_READONLY):(a^FILE_ATTRIBUTE_READONLY)); }
	void set_system( bool s ) const {	if(!*_data) return; auto a=wattrib(); if(a!=INVALID_FILE_ATTRIBUTES) SetFileAttributesA(_data,a=s?(a|FILE_ATTRIBUTE_SYSTEM):(a^FILE_ATTRIBUTE_SYSTEM)); }	
	path junction() const;

	// path structure query
	bool is_drive()		const {	if(!*_data) return false; size_t l=size(); return l<4&&l>1&&_data[1]==':'; } // backslash-less drive fails with st_mode
	bool is_synology() const {	return is_ssh()&&strstr(_data,":\\volume")!=nullptr||strstr(_data,":/volume")!=nullptr; }
	bool is_subdir( const path& ancestor ) const { return strnicmp(_data,ancestor._data,ancestor.size())==0; } // do not check existence
	path canonical() const { if(is_pipe()) return *this; if(is_remote()) return to_slash(); path p(*this); p.__canonicalize(); return p; } // not necessarily absolute: return relative path as well

	// decomposition
	path dir()		const { return __super::dir(); }
	path unc_root()	const { return __super::unc_root(); }
	path dirname()	const { return __super::dirname(); }
	path filename()	const { return __super::filename(); }
	path stem()		const { return __super::stem(); }
	path extension()const { return __super::extension(); }
	path parent()	const { return __super::parent(); }
	path remove_extension() const { return __super::remove_extension(); }
	path replace_extension( path x ) const { return __super::replace_extension(x); }
	path absolute( path base="" ) const { return __super::absolute(base); }
	path relative( path from="" ) const { return __super::relative(from); }
	// decomposition: alias
	path x()		const { return __super::extension(); }
	path nox()		const { return __super::remove_extension(); }
	// decompostion: extensions
	vector<path> ancestors(path root="") const { auto a=__super::ancestors(root); vector<path> v;for(const auto& p:a) v.emplace_back(p); return v; }
	vector<path> explode() const { vector<path> v; v.reserve(16); for( char *ctx=nullptr, *t=strtok_s(__strdup(_data),"\\/", &ctx); t; t=strtok_s(0,"\\/", &ctx)) v.emplace_back(t); return v; }
	path drive() const	{ if(empty()) return ""; if(is_unc()) return unc_root(); char r[_MAX_DRIVE]={}; _splitpath_s(_data,r,_MAX_DRIVE,0,0,0,0,0,0); if(r&&*r) *r=toupper(*r); return r; }
	// decompostion: extensions for remote
	int port() const { if(!is_remote()) return 0; char* s=_data; const char* d=strstr(s,"://"); const char* c=strstr(d?d+3:s+2,":"); if(!c||!++c) return 0; const char* h=strstr(c,"/"); return atoi(h?__strdup(c,h-c):c); }
	path remove_port() const { int port=this->port(); if(!port) return *this; path t=*this; char* s=t._data; char p[17]; snprintf(p,16,":%d",port); const char* d=strstr(s+2,"://"); const char* h=strstr(d?d+3:s+2,p); if(!h) return t; char* b=__strdup(s); strcpy(b+(h-s)+(d?0:1),h+strlen(p)); return b; }
	path add_port( int port ) const { if(!is_remote()) return *this; path t=remove_port(); char* s=t._data; char p[17]; snprintf(p,16,":%d/",port); const char* d=strstr(s+2,"://"); const char* h=strstr(d?d+3:s+2,d?"/":":/"); if(!h) return t; char* b=__strdup(s); strcpy(strcpy(b+(h-s),p)+strlen(p),h+(d?1:2)); return b; }

	// simpler alias to decompositions
	const char* name( bool with_ext=true ) const { return __strdup((with_ext?filename():stem()).c_str()); }
	const wchar_t* wname( bool with_ext=true ) const { return atow((with_ext?filename():stem()).c_str()); }
	const char* slash() const { return __strdup(to_slash().c_str()); }
	const char* auto_quote() const { if(!*_data||(_data[0]=='\"'&&_data[strlen(_data)-1]=='\"')) return c_str(); auto* t=__strbuf(capacity); size_t l=strlen(_data); memcpy(t,_data,l*sizeof(value_type)); if(t[l]==' '||t[l]=='\t'||t[l]=='\n') t[l]=0; if(t[0]==' '||t[0]=='\t'||t[0]=='\n') t++; if(t[0]!='-'&&!strpbrk(t," \t\n|&<>")) return c_str(); path p; p[0]='\"'; memcpy(p._data+1, _data, l*sizeof(value_type)); p[l+1]='\"'; p[l+2]=0; return __strdup(p.c_str()); }

	// shell operations
	bool copy_file( path dst, bool overwrite=true ) const { if(!exists()||is_dir()||dst.empty()) return false; if(dst.is_dir()||__is_separator(dst.back())) dst=dst.append_slash()+name(); dst.dir().mkdir(); if(dst.exists()&&overwrite){ if(dst.is_hidden()) dst.set_hidden(false); if(dst.is_readonly()) dst.set_readonly(false); } return bool(CopyFileW( atow(c_str()), atow(dst.c_str()), overwrite?FALSE:TRUE)); }
	bool move_file( path dst ) const { return !*_data||!dst._data||is_dir()?false:(drive()==dst.drive()&&!dst.exists()) ? MoveFileW(atow(_data),atow(dst._data))!=0 : !copy_file(dst,true) ? false: rmfile(); }
#ifndef _INC_SHELLAPI
	bool delete_file() const { if(!exists()||is_dir()) return false; if(is_hidden()) set_hidden(false); if(is_readonly()) set_readonly(false); return DeleteFileW(_data)==TRUE; }
	bool rmfile() const { return delete_file(); }
	bool delete_dir() const { if(!is_dir()) return false; if(is_hidden()) set_hidden(false); if(is_readonly()) set_readonly(false); rmdir(_data); return true; }
	bool rmdir() const { return delete_dir(); }
#else
	bool delete_file( bool b_undo=false ) const { if(!exists()||is_dir()) return false; if(is_hidden()) set_hidden(false); if(is_readonly()) set_readonly(false); SHFILEOPSTRUCTW fop={};fop.wFunc=FO_DELETE;fop.fFlags=FOF_FILESONLY|FOF_SILENT|FOF_NOCONFIRMATION|(b_undo?FOF_ALLOWUNDO:0); fop.pFrom=atow(to_preferred().c_str()); return SHFileOperationW(&fop)==0;}
	bool rmfile( bool b_undo=false ) const { return delete_file(b_undo); }
	bool delete_dir( bool b_undo=true ) const { if(!exists()||!is_dir()) return false; if(is_hidden()) set_hidden(false); if(is_readonly()) set_readonly(false); SHFILEOPSTRUCTW fop={};fop.wFunc=FO_DELETE;fop.fFlags=FOF_SILENT|FOF_NOCONFIRMATION|(b_undo?FOF_ALLOWUNDO:0); fop.pFrom=atow(to_preferred().c_str()); return SHFileOperationW(&fop)==0;}
	bool rmdir( bool b_undo=true ) const { return delete_dir(b_undo); }
	bool copy_dir( path dst, bool overwrite=true ) const { if(!is_dir()) return false;value_type* from=__strbuf(capacity);snprintf(from,capacity,"%s\\*\0",_data);dst[dst.size()+1]=0; SHFILEOPSTRUCTW fop={};fop.wFunc=FO_COPY;fop.fFlags=FOF_ALLOWUNDO|FOF_SILENT|(overwrite?FOF_NOCONFIRMATION:0); fop.pFrom=atow(path(from).to_preferred().c_str()); fop.pTo=atow(dst.to_preferred().c_str()); return SHFileOperationW(&fop)==0; }
	bool move_dir( path dst ) const { return !exists()||!is_dir()?false:(drive()==dst.drive()&&!dst.exists()) ? MoveFileW(atow(_data),atow(dst._data))!=0 : !copy_dir(dst,true) ? false: rmdir(); }
	bool open( const char* args=nullptr, bool b_show_window=true ) const { if(!*_data) return false; auto r=ShellExecuteW(GetDesktopWindow(),L"Open",atow(auto_quote()), args?atow(args):nullptr, nullptr, b_show_window?SW_SHOWNORMAL:SW_HIDE); return ((INT_PTR)r)>32; }
#endif

	// additional time functions
	FILETIME mfiletime() const;
	const char* mtimestamp() const { if(!exists()) return ""; struct tm t; _gmtime64_s(&t,&(__attrib().st_mtime)); char* buff=__strbuf(256);sprintf(buff,"%04d%02d%02d%02d%02d%02d",t.tm_year+1900,t.tm_mon+1,t.tm_mday,t.tm_hour,t.tm_min,t.tm_sec);return buff; }

	// file content access: void (rb/wb), char (r/w), wchar_t (r/w,ccs=UTF-8)
	FILE* fopen( const char* mode, bool utf8=false ) const;
	template <class T=void> sized_ptr_t<T> read_file() const;
	string read_file() const;
	bool write_file( const void* ptr, size_t size ) const;
	bool write_file( sized_ptr_t<void> p ) const { return p.ptr&&p.size?write_file(p.ptr,p.size):false; }
	bool write_file( const char* s ) const;
	bool write_file( const wchar_t* s ) const;
	bool is_binary_file() const	{ FILE* f=fopen("rb"); if(!f) return false; char b[4096]; while(1){ size_t n=fread(b, 1, sizeof(b), f); if(!n) break; if(memchr(b, 0, n)){ fclose(f); return true; } } fclose(f); return false; }

	// scan(): ext_filter (specific extensions delimited by semicolons), str_filter (path should contain this string)
	template <bool recursive=true> vector<path> scan( const char* pattern=nullptr, const char* ext_filter=nullptr ) const;
	template <bool recursive=true> vector<path> subdirs( const char* pattern=nullptr ) const;
	

	// crc32c/md5 checksums of the file content implement in gxmemory.h
	inline uint crc() const;

protected:

	struct __scan_t { vector<path> result; struct { bool recursive, glob; } b; struct { sized_ptr_t<wchar_t>* v; size_t l; } ext; struct { const wchar_t* pattern; size_t l; } glob; };
	void __scan_recursive( const wchar_t* dir, __scan_t& si ) const;
	void __subdirs_recursive( const wchar_t* dir, __scan_t& si ) const;
	void __canonicalize() const; // remove redundancies in directories (e.g., "../some/..", "./" )
	path __normalize_final() const; // normalize path by GetFinalPathNameByHandleW(FILE_NAME_NORMALIZED)
};

__noinline path path::__normalize_final() const
{
	if(!*_data) return ""; auto a=wattrib(); if(a==INVALID_FILE_ATTRIBUTES) return "";
	HANDLE h=CreateFileW(atow(_data),GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_FLAG_BACKUP_SEMANTICS,0); if(h==INVALID_HANDLE_VALUE) return "";
	auto* w=__strbuf<wchar_t>(capacity); GetFinalPathNameByHandleW(h,w,capacity,FILE_NAME_NORMALIZED); CloseHandle(h);
	path t=wcsncmp(w,L"\\\\?\\",4)==0?w+4:w;
	return (a&FILE_ATTRIBUTE_DIRECTORY)!=0?t.append_slash():t;
}

__noinline bool path::is_junction() const
{
	// After finding reparse_point, junction path should be examined. This is because
	// a cloud service (e.g., Dropbox) often reports FILE_ATTRIBUTE_REPARSE_POINT for a regular directory but its junction path is actually identical
	if(!*_data||is_drive()) return false;
	auto a=wattrib(); if(a==INVALID_FILE_ATTRIBUTES||(a&FILE_ATTRIBUTE_REPARSE_POINT)==0) return false;
	return (a&FILE_ATTRIBUTE_DIRECTORY)==0?false:canonical().append_slash()!=__normalize_final();
} 

__noinline path path::junction() const
{
	if(!*_data||is_drive()) return "";
	auto a=wattrib(); if(a==INVALID_FILE_ATTRIBUTES) return "";
	if((a&FILE_ATTRIBUTE_DIRECTORY)==0) a=dir().wattrib();
	if((a&FILE_ATTRIBUTE_REPARSE_POINT)!=0) return __normalize_final();
	for( const auto& k : ancestors() )
	{
		if(k.is_drive()) break; auto b=k.wattrib(); if(b==INVALID_FILE_ATTRIBUTES) break;
		if((b&FILE_ATTRIBUTE_REPARSE_POINT)!=0&&(b&FILE_ATTRIBUTE_DIRECTORY)!=0) return __normalize_final();
	}
	return "";
}

__noinline FILE* path::fopen( const char* mode, bool utf8 ) const
{
	char m[32]={0}; snprintf(m,32,"%s%s",mode,utf8?",ccs=UTF-8":"");
	FILE* fp = ::fopen(_data,m); if(!fp) return nullptr;
	else if(strcmp(mode,"w")==0&&utf8&&extension()!="sln") _fseeki64(fp,0,SEEK_SET); // remove byte order mask (BOM); sln use BOM, but vcxproj do not use BOM
	return fp;
}

template<> __noinline sized_ptr_t<void> path::read_file<void>() const
{
	sized_ptr_t<void> p={nullptr,0};
	FILE* fp=fopen("rb",false); if(!fp) return {nullptr,0};
	p.size = ::file_size(fp); if(!p.size){ fclose(fp); return {nullptr, 0}; }
	p.ptr=malloc(p.size+1); if(p.ptr) fread(p.ptr,1,p.size,fp); ((char*)p.ptr)[p.size]=0;
	fclose(fp);
	return p;
}

template<> __noinline sized_ptr_t<wchar_t> path::read_file<wchar_t>() const
{
	sized_ptr_t<wchar_t> p={nullptr,0};
	FILE* fp=fopen("r",true); if(!fp) return {nullptr,0};
	size_t size0 = ::file_size(fp); if(!size0){ fclose(fp); p.ptr=(wchar_t*)L""; return p; }

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
	size_t size0 = ::file_size(fp); if(!size0){ fclose(fp); p.ptr=(char*)""; return p; }

	string buffer; buffer.reserve(size0*2);
	char buff[4096]; while(fgets(buff,4096,fp)) buffer+=buff; fclose(fp);
	p.size = buffer.size();
	p.ptr = (char*) malloc((p.size+2)*sizeof(char));
	if(p.ptr) memcpy(p.ptr,buffer.c_str(),p.size*sizeof(char));
	p.ptr[p.size]=p.ptr[p.size+1]=0; // two more bytes for null-ended wchar_t string
	return p;
}

__noinline string path::read_file() const {			sized_ptr_t<char> p=read_file<char>(); if(!p||!p.size) return ""; string s=p.ptr; free(p.ptr); return s; }
template<> __noinline sized_ptr_t<const void>		path::read_file<const void>() const { auto p=read_file<void>(); return {(const void*)p.ptr,p.size}; }
template<> __noinline sized_ptr_t<const char>		path::read_file<const char>() const { auto p=read_file<char>(); return {(const char*)p.ptr,p.size}; }
template<> __noinline sized_ptr_t<const wchar_t>	path::read_file<const wchar_t>() const { auto p=read_file<wchar_t>(); return {(const wchar_t*)p.ptr,p.size}; }

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
vector<path> path::scan( const char* pattern, const char* ext_filter ) const
{
	path src=empty()?path(".")+preferred_separator:(is_relative()?absolute():canonical()); if(src.exists()&&src.is_dir()) src=src.append_slash();
	path srcp=src.filename(); if(!srcp.empty()){ if(!pattern) pattern=srcp.c_str(); src=src.dir(); }
	vector<std::wstring> exts; if(ext_filter&&*ext_filter){ value_type ef[4096]={}, *ctx=nullptr; strcpy(ef,ext_filter); for(value_type* e=strtok_s(ef,";",&ctx);e;e=strtok_s(nullptr,";",&ctx)) if(e[0]) exts.push_back(L"."s+atow(e)); }
	vector<sized_ptr_t<wchar_t>> eptr; for( auto& e:exts ) eptr.emplace_back(sized_ptr_t<wchar_t>{(wchar_t*)e.c_str(),e.size()});
	__scan_t si; si.b.recursive=recursive; si.b.glob=pattern&&strpbrk(pattern,"*?"); si.ext.v=eptr.size()>0?eptr.data():nullptr; si.ext.l=eptr.size();
	std::wstring wpattern=pattern?atow(pattern):L""; si.glob.pattern=pattern?wpattern.c_str():nullptr; si.glob.l=pattern?wpattern.size():0;
	si.result.reserve(1ull<<12);__scan_recursive(atow(src.c_str()),si);
	return si.result;
}

template <bool recursive> __noinline
vector<path> path::subdirs( const char* pattern ) const
{
	path src=empty()?path(".")+preferred_separator:(is_relative()?path(".").append_slash().absolute():*this).append_slash(); if(!src.is_dir()) return vector<path>{};
	__scan_t si; si.b.recursive=recursive; si.b.glob=pattern&&strpbrk(pattern,"*?"); si.ext.v=nullptr; si.ext.l=0;
	std::wstring wpattern=pattern?atow(pattern):L""; si.glob.pattern=pattern?wpattern.c_str():nullptr; si.glob.l=pattern?wpattern.size():0;
	si.result.reserve(1ull<<12);__subdirs_recursive(atow(src.c_str()),si);
	return si.result;
}

__noinline void path::__scan_recursive( const wchar_t* _dir, path::__scan_t& si ) const
{
	size_t dl=wcslen(_dir); wchar_t dir[capacity]={}; wcsncpy(dir,_dir,dl); wcscpy(dir+dl,L"*.*"); // append wildcard for search
	WIN32_FIND_DATAW fd={}; HANDLE h=FindFirstFileExW(dir,FindExInfoBasic/*minimal(faster)*/,&fd,FindExSearchNameMatch,0,FIND_FIRST_EX_LARGE_FETCH); if(h==INVALID_HANDLE_VALUE) return;
	wchar_t *f=fd.cFileName, *p=dir+dl; auto* e=si.ext.v; dir[dl]=0; // revert wildcard
	vector<std::wstring> sdir; if(si.b.recursive) sdir.reserve(32);
	while(FindNextFileW(h,&fd)) // skip directly first '.'
	{
		if(f[0]==L'.'){ if(!f[1]||(f[1]==L'.'&&f[2]==0)||memcmp(f+1,L"git",sizeof(wchar_t)*4)==0) continue; } // skip ., .., .git
		if((fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)==0) // files
		{
			size_t fl=wcslen(f); if(e){size_t j=0;for(;j<si.ext.l;j++){if(e[j].size<fl&&wcsicmp(e[j],f+fl-e[j].size)==0)break;}if(j==si.ext.l)continue;}
			if(si.glob.l){if(si.b.glob?!iglob(f,fl,si.glob.pattern,si.glob.l):wcsistr(f,fl,si.glob.pattern,si.glob.l)==nullptr)continue;}
			wcsncpy(p,f,fl); p[fl]=0; si.result.emplace_back(dir);
		}
		if(!si.b.recursive) continue; // directories
		size_t fl=wcslen(f); memcpy(p,f,sizeof(wchar_t)*fl); p[fl]=preferred_separator; p[fl+1]=0; sdir.emplace_back(dir);
	}
	FindClose(h);
	for(auto& c:sdir) __scan_recursive(c.c_str(), si);
}

__noinline void path::__subdirs_recursive( const wchar_t* _dir, path::__scan_t& si ) const
{
	size_t dl=wcslen(_dir); wchar_t dir[capacity]={}; wcsncpy(dir,_dir,dl); wcscpy(dir+dl,L"*.*"); // append wildcard for search
	WIN32_FIND_DATAW fd={}; HANDLE h=FindFirstFileExW(dir,FindExInfoBasic/*minimal(faster)*/,&fd,FindExSearchNameMatch,0,FIND_FIRST_EX_LARGE_FETCH); if(h==INVALID_HANDLE_VALUE) return;
	wchar_t *f=fd.cFileName, *p=dir+dl; auto* e=si.ext.v; dir[dl]=0; // revert wildcard
	while(FindNextFileW(h,&fd)) // skip directly first '.'
	{
		if((fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)==0) continue; // skip files
		if(f[0]==L'.'){ if(!f[1]||(f[1]==L'.'&&f[2]==0)||memcmp(f+1,L"git",sizeof(wchar_t)*4)==0) continue; } // skip ., .., .git
		size_t fl=wcslen(f); wcsncpy(p,f,fl); p[fl]=preferred_separator; p[fl+1]=0;
		if(si.glob.l==0) si.result.emplace_back(dir);
		else if(!si.b.glob){ if(iglob(f,fl,si.glob.pattern,si.glob.l)) si.result.emplace_back(dir); }
		else if(wcsistr(f, fl, si.glob.pattern, si.glob.l)) si.result.emplace_back(dir);
		if(si.b.recursive) __subdirs_recursive(dir, si);
	}
	FindClose(h);
}

__noinline void path::__canonicalize() const
{
	size_t len=*_data?strlen(_data):0; if(!len) return;
	for(uint k=0;k<len;k++) if(__is_separator(_data[k])) _data[k]=preferred_separator;
	if(_data[len-1]=='.'&&((len>2&&__is_separator(_data[len-2]))||(len>3&&__is_separator(_data[len-3])&&_data[len-2]=='.'))){ _data[len++]=preferred_separator; _data[len]=0; } // add trailing slash to "\\." or "\\.."
	if(len==2&&_data[1]==':'){ _data[len++]=preferred_separator; _data[len]=0; } // root correction
	if(!strstr(_data,"\\.\\")&&!strstr(_data,"\\..\\")&&!strstr(_data,"/./")&&!strstr(_data,"/../")) return; // trivial return
	value_type* ds; while((ds=(value_type*)strstr(_data+1,"\\\\"))) memmove(ds+1,ds+2,((len--)-(ds-_data)-1)*sizeof(value_type)); // correct multiple backslashes, except the beginning of unc path
	if(is_absolute()){ auto* r=_fullpath(_data,(const value_type*)memcpy(__strbuf(capacity),_data,sizeof(value_type)*(len+1)),capacity); return; }

	// flags to check
	bool b_trailing_backslash = __is_separator(_data[len-1]);
	bool b_single_dot_begin = _data[0]=='.'&&__is_separator(_data[1]);

	// perform canonicalization
	std::deque<value_type*> L;value_type* ctx=nullptr;
	for(value_type* t=strtok_s(strcpy(__strbuf(capacity),_data),"/\\", &ctx); t; t=strtok_s(nullptr, "/\\", &ctx))
	{
		if(t[0]=='.'&&t[1]=='.'&&!L.empty()&&memcmp(L.back(),"..",sizeof(value_type)*2)!=0) L.pop_back();
		else if(t[0]!='.'||t[1]!=0) L.emplace_back(strcpy(__strbuf(capacity),t));
	}

	// reconstruction
	value_type* d=_data;
	if(!b_single_dot_begin&&L.empty()){ d[0]=0;return; }
	if(L.empty()||(b_single_dot_begin&&(!L.empty()&&L.front()[0]!='.'))){ (d++)[0]='.';(d++)[0]=preferred_separator;d[0]=0; } // make it begin with single-dot again
	for(size_t k=0,kn=L.size();k<kn;k++){ value_type* t=L[k];size_t l=strlen(t);memcpy(d,t,l*sizeof(value_type));d+=l;(d++)[0]=preferred_separator;d[0]=0; }
	if(!b_trailing_backslash&&d>_data)(d-1)[0]=0;
}

// Win32 filetime utilities: FILETIME in 100 ns scale
inline SYSTEMTIME	FileTimeToSystemTime( FILETIME f ){ SYSTEMTIME s; FileTimeToSystemTime(&f,&s); return s; }
inline FILETIME		SystemTimeToFileTime( const SYSTEMTIME& s ){ FILETIME f; SystemTimeToFileTime(&s,&f); return f; }
inline time_t		FileTimeToTime( FILETIME f ){ ULARGE_INTEGER u; u.LowPart=f.dwLowDateTime; u.HighPart=f.dwHighDateTime; return (u.QuadPart-116444736000000000LL)/10000000LL; }
inline time_t		SystemTimeToTime( const SYSTEMTIME& s ){ return FileTimeToTime(SystemTimeToFileTime(s)); }
inline FILETIME		TimeToFileTime( time_t t ){ ULARGE_INTEGER u; u.QuadPart = (t*10000000LL) + 116444736000000000LL; return FILETIME{u.LowPart,u.HighPart}; }
inline SYSTEMTIME	TimeToSystemTime( time_t t ){ return FileTimeToSystemTime(TimeToFileTime(t)); }
inline FILETIME		path::mfiletime() const { return TimeToFileTime(mtime()); }

//***********************************************
// nocase/std map/unordered_map extension for path
namespace std
{
	template <> struct hash<path>{ size_t operator()(const path& p)const{ return std::hash<string>()(strlwr(__strdup(p.c_str()))); }};
}

namespace nocase
{
	template <> struct less<path>{ bool operator()(const path& a,const path& b)const{return a<b;}};
	template <> struct equal_to<path>{ bool operator()(const path& a,const path& b)const{return stricmp(a.c_str(),b.c_str())==0; } };
	template <> struct hash<path>{ size_t operator()(const path& p)const{ return std::hash<string>()(strlwr(__strdup(p.c_str()))); } };
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
		if(file_path.is_unc()||file_path.is_remote()) return;
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
	bool is_exfat() const { return wcsicmp(filesystem.name,L"exFAT")==0; }
	bool is_ntfs() const { return wcsicmp(filesystem.name,L"NTFS")==0; }
	bool is_fat32() const { return wcsicmp(filesystem.name,L"FAT32")==0; }
};
#endif

//***********************************************
#endif // __GX_FILESYSTEM_H__
