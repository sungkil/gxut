#pragma once
#ifndef __GX_INIPARSER_H__
#define __GX_INIPARSER_H__

#if __has_include("gxut.h")
	#include "gxut.h"
#elif __has_include("../gxut.h")
	#include "../gxut.h"
#elif __has_include(<gxut/gxut.h>)
	#include <gxut/gxut.h>
#endif

//*************************************
namespace ini {
//*************************************

// case-insensitive keys ("SECTIONNAME:KEYNAME") and values (char)
// important features: can handle multiple values for a single key

struct entry_t
{
	size_t index; string section, key; string value;
	entry_t( size_t idx, const char* _sec, const char* _key ):index(idx),section(_sec),key(_key){};
	static bool compare_by_index( entry_t* e0, entry_t* e1 ){ return e0->index < e1->index; }
};

struct auto_lock_t
{
	CRITICAL_SECTION& cs_r;
	auto_lock_t( CRITICAL_SECTION& cs ):cs_r(cs){ EnterCriticalSection(&cs_r); }
	~auto_lock_t(){ LeaveCriticalSection(&cs_r); }
};

struct file_time_t { int s,m,h,d,M,y; };	// sec, min, hour, day, month, year: aligned by the struct tm
using dictionary_t = logical::map<string,entry_t*>;

struct parser_t
{
protected:

	path_t				file_path;	// current file path
	bool				b_batch = false;
	file_time_t			mtime = {};
	CRITICAL_SECTION	cs;			// for load/save
	dictionary_t		dic;		// case-insensitive dictionary map
	char				buffer[4096+1]={};
	static const size_t	buffer_capacity = std::extent<decltype(buffer)>::value-1;

	entry_t* get_or_create_entry( const char* seckey ){ if(seckey==nullptr||!*seckey) return nullptr; auto it=dic.find(seckey); if(it!=dic.end())return it->second; auto ss=split_section_key(seckey); return dic[seckey]=new entry_t(dic.size(),ss.first,ss.second); }
	std::pair<const char*,const char*> split_section_key( const char* seckey ){ char *sk=__strdup(seckey),*colon=(char*)strchr(&sk[0],':');if(colon==nullptr)return std::make_pair("",sk);else{colon[0]=0;return std::make_pair(sk,colon+1);} }
	void read_line( wchar_t* line, wchar_t* sec );

public:
	virtual ~parser_t(){ for(auto& it:dic)if(it.second!=nullptr){delete it.second;} dic.clear(); DeleteCriticalSection(&cs); }
	parser_t(){ InitializeCriticalSectionAndSpinCount(&cs,4000); }
	parser_t( path_t file_path ):parser_t(){ set_path(file_path); }

	// query and retrieval
	void set_path( const path_t& file_path ){ this->file_path = file_path.absolute(); }
	const path_t& get_path() const { return file_path; }
	bool key_exists( const char* key ) const { if(key==nullptr||key[0]=='\0') return false; return dic.find(key)!=dic.end(); }
	bool key_exists( const char* sec, const char* key ) const { if(!key||!*key) return false; if(!sec||!*sec) return key_exists(key); char sk[4096]; snprintf(sk,4096,"%s:%s",sec,key); return dic.find(sk)!=dic.end(); }
	bool section_exists( const char* sec ) const { if(sec==nullptr||sec[0]=='\0') return false; for(auto& it:dic) if(stricmp(it.second->section.c_str(),sec)==0) return true; return false; }
	std::set<string> section_set() const { std::set<string> ss;for(auto& it:dic)ss.emplace(it.second->section);return ss;}
	vector<string> sections(){ vector<string> sl; std::set<string> ss; for(auto& it:entries()){ if(ss.find(it->section.c_str())!=ss.end()) continue; sl.emplace_back(it->section); ss.emplace(it->section); } return sl; }
	vector<entry_t*> entries( const char* sec=nullptr ){ vector<entry_t*> el; for(auto& it:dic) if(sec==nullptr||stricmp(it.second->section.c_str(),sec)==0) el.emplace_back(it.second); std::sort(el.begin(),el.end(),entry_t::compare_by_index); return el; } // return all entries if no section is given

	// clear
	void clear( const char* seckey ){ auto it=dic.find(seckey);if(it==dic.end()) return; delete it->second; dic.erase(it); save(); }
	void clear( const char* sec, const char* key ){ char sk[4096]; snprintf(sk,4096,"%s:%s",sec,key); clear(sk); }
	void clear_section( const string& sec ){ bool b_save=false; for(auto it=dic.begin();it!=dic.end();){if(stricmp(it->second->section.c_str(),sec.c_str())==0){delete it->second;it=dic.erase(it);b_save=true; }else it++;} if(b_save) save(); }

	// load/save
	void begin_update(){ b_batch=true; }
	void end_update(){ b_batch=false; }
	bool load();
	bool load( const wchar_t* source );
	bool load( const char* source ){ return source?load(atow(source)):false; }
	bool save(){ return save_as(file_path); }
	bool save_as( const path_t& file_path );

	// get
	__forceinline const char* get( const char* key ){ if(!key||!*key) return ""; auto it=dic.find(key); return it==dic.end()?"":it->second->value.c_str(); }
	__forceinline const char* get( const char* sec, const char* key ){ if(!sec||!*sec||!key||!*key) return ""; char sk[257]; snprintf(sk,256,"%s:%s",sec,key); return get(sk); }
	template<class T> T get( const char* sec, const char* key ){ if(!sec||!*sec||!key||!*key) return get<T>(""); if(!sec||!*sec) return get<T>(key); if(!key||!*key) return get<T>(sec); char sk[257]; snprintf(sk,256,"%s:%s",sec,key); return get<T>(sk); }
	template<class T> T get( const char* key );

	// set
	template<class T> void set( const char* key, T value );
	template<class T> void set( const char* sec, const char* key, T value ){ if(!sec||!*sec||!key||!*key) return; if(!sec||!*sec) return set<T>(key,value); if(!key||!*key) return set<T>(sec,value); char sk[257]; snprintf(sk,256,"%s:%s",sec,key); set<T>(sk,value); }
	template<class T> void set_or_clear_default( const char* key, T value, T default_value ){ if(value==default_value) return clear(key); set<T>(key,value); }
	template<class T> void set_or_clear_default( const char* sec, const char* key, T value, T default_value ){ if(value==default_value) return clear(sec,key); set<T>(sec,key,value); }
};

__noinline void parser_t::read_line( wchar_t* line, wchar_t* sec )
{
	char seckey[512];
	wchar_t* b=itrim(line); if(!*b||*b==L'#'||*b==L';') return; // skip comment only in the first character
	while(b) // split by the right bracket or semicolon (for single-line multistatements)
	{
		wchar_t* e=b; while(*e&&*e!=L']'&&*e!=L';')e++;bool eol=(*e==0);*e=0;
		auto* t=itrim(b); b=eol?0:itrim(e+1);

		int l=int(wcslen(t)); if(l<2) continue;
		if(t[0]==L'['){ wcscpy(sec,itrim(t+1)); continue; } // assign section
		wchar_t* v=0; for(int k=0;k<l;k++)if(t[k]==L'='){t[k]=0;v=t+k+1;break;} if(!t||!v) continue; t=itrim(t); if(!*t) continue;
		snprintf(seckey,std::extent<decltype(seckey)>::value,"%s:%s",wtoa(sec),wtoa(t));get_or_create_entry(seckey)->value=wtoa(itrim(v));
	}
}

__noinline bool parser_t::load( const wchar_t* source )
{
	if(!source) return false;
	auto_lock_t lock(cs);
	for(auto& it:dic){if(it.second!=nullptr){delete it.second;}} dic.clear(); // clear dictionary and buffer
	wchar_t sec[512]; for( auto& buff: explode( source, L"\r\n" ) ) read_line( (wchar_t*)buff.c_str(), sec ); // read lines
	return true;
}

__noinline bool parser_t::load()
{
	if(file_path.empty()) return false;
	struct _stat s; if(_stat(file_path.c_str(),&s)!=0) return false;

	// bypass non-modified file
	file_time_t mt={}; memcpy(&mt,_gmtime64(&s.st_mtime),sizeof(file_time_t));mt.y+=1900;mt.M+=1;
	if(memcmp(&mtime,&mt,sizeof(file_time_t))==0) return true; mtime = mt;

	// open now
	FILE* fp=fopen(file_path.c_str(),"r,ccs=UTF-8");if(!fp){ printf("Unable to open %s",file_path.c_str()); return false; }
	auto_lock_t lock(cs);

	// clear dictionary and buffer
	for(auto& it:dic){if(it.second!=nullptr){delete it.second;}} dic.clear();
	memset(buffer,0,sizeof(buffer));

	// read lines
	wchar_t buffer[4096]={}, sec[512]={};
	while(fgetws(buffer,buffer_capacity,fp)) read_line(buffer,sec);
	fclose(fp);
	return true;
}

__noinline bool parser_t::save_as( const path_t& file_path )
{
	if(b_batch) return false;
	if(file_path.empty()){ printf( "%s(): file_path is empty\n", __func__ ); return false; }

	auto_lock_t lock(cs);

	string sec; bool bLine0=true;
	std::wstring buff;
	for( auto it=dic.begin(); it!=dic.end(); )
	{
		entry_t* e=it->second;
		e->section = trim(e->section.c_str());
		e->value = trim(e->value.c_str());
		if(e->section.empty()||e->value.empty()){ it=dic.erase(it); continue; }

		if(sec!=e->section){buff+=format(L"%s[%s]\n",bLine0?L"":L"\n",atow(e->section.c_str()));sec=e->section;}
		buff+=format(L"%s = %s\n",atow(e->key.c_str()),atow(e->value.c_str()));bLine0=false;
		it++;
	}
	
	bool file_exists = access(file_path.c_str(),0)==0;
	bool b_hidden_file=false;if(file_exists&&(GetFileAttributesW(::atow(file_path.c_str()))&FILE_ATTRIBUTE_HIDDEN)){b_hidden_file=true;SetFileAttributesW(atow(file_path.c_str()),GetFileAttributesW(atow(file_path.c_str()))&(~FILE_ATTRIBUTE_HIDDEN) );} // save and remove hidden attribute
	FILE* fp=nullptr; for(uint k=0;fp==nullptr&&k<20;k++){ fp=fopen(file_path.c_str(), "w,ccs=UTF-8"); Sleep(5); } // wait 100ms for busy writing
	if(fp==nullptr){ printf( "%s(): Unable to open %s to write", __func__, file_path.c_str() ); return false; }
	_fseeki64( fp, 0, SEEK_SET ); // remove BOM
	fputws(buff.c_str(),fp);
	fclose(fp);
	if(b_hidden_file) SetFileAttributesW(atow(file_path.c_str()),GetFileAttributesW(atow(file_path.c_str()))|FILE_ATTRIBUTE_HIDDEN ); // restore hidden attribute
	
	return true;
}

// template specializations for get()
template<> __noinline string parser_t::get<string>(const char* key){		auto* v=get(key); return *v==0?"":string(v); }
#ifdef __GX_FILESYSTEM_H__
template<> __noinline path parser_t::get<path>( const char* key ){			auto* v=get(key); return *v==0?path():path(v); }
#endif
template<> __noinline bool parser_t::get<bool>( const char* key ){			auto* v=get(key); return *v==0?false:atob(v); }
template<> __noinline int parser_t::get<int>( const char* key ){			auto* v=get(key); return *v==0?0:atoi(v); }
template<> __noinline uint parser_t::get<uint>( const char* key ){			return uint(get<int>(key)); }
template<> __noinline int64_t parser_t::get<int64_t>( const char* key ){	auto* v=get(key); return *v==0?0:_atoi64(v); }
template<> __noinline uint64_t parser_t::get<uint64_t>( const char* key ){	return uint64_t(get<int64_t>(key)); }
template<> __noinline float parser_t::get<float>( const char* key ){		auto* v=get(key); return *v==0?0:float(atof(v)); }
template<> __noinline int2 parser_t::get<int2>( const char* key ){			auto* v=get(key); return *v==0?int2{}:atoi<int2>(v); }
template<> __noinline int3 parser_t::get<int3>( const char* key ){			auto* v=get(key); return *v==0?int3{}:atoi<int3>(v); }
template<> __noinline int4 parser_t::get<int4>( const char* key ){			auto* v=get(key); return *v==0?int4{}:atoi<int4>(v); }
template<> __noinline ivec2 parser_t::get<ivec2>( const char* key ){		auto* v=get(key); return *v==0?ivec2{}:atoi<ivec2>(v); }
template<> __noinline ivec3 parser_t::get<ivec3>( const char* key ){		auto* v=get(key); return *v==0?ivec3{}:atoi<ivec3>(v); }
template<> __noinline ivec4 parser_t::get<ivec4>( const char* key ){		auto* v=get(key); return *v==0?ivec4{}:atoi<ivec4>(v); }
template<> __noinline float2 parser_t::get<float2>( const char* key ){		auto* v=get(key); return *v==0?float2{}:atof<float2>(v); }
template<> __noinline float3 parser_t::get<float3>( const char* key ){		auto* v=get(key); return *v==0?float3{}:atof<float3>(v); }
template<> __noinline float4 parser_t::get<float4>( const char* key ){		auto* v=get(key); return *v==0?float4{}:atof<float4>(v); }
template<> __noinline vec2 parser_t::get<vec2>( const char* key ){			auto* v=get(key); return *v==0?vec2{}:atof<vec2>(v); }
template<> __noinline vec3 parser_t::get<vec3>( const char* key ){			auto* v=get(key); return *v==0?vec3{}:atof<vec3>(v); }
template<> __noinline vec4 parser_t::get<vec4>( const char* key ){			auto* v=get(key); return *v==0?vec4{}:atof<vec4>(v); }

// template specializations for set()
template<> __noinline void parser_t::set<const char*>( const char* key, const char* value ){ if(!key||!*key) return; if(!value||!*value) return clear(key); bool b=key_exists(key); entry_t* e=get_or_create_entry(key); if(b&&e->value==value) return; e->value=value; save(); }
template<> __noinline void parser_t::set<char*>( const char* key, char* value ){		set<const char*>(key,value); }
template<> __noinline void parser_t::set<string>( const char* key, string value ){		set<const char*>(key,value.c_str()); }
#ifdef __GX_FILESYSTEM_H__
template<> __noinline void parser_t::set<path>( const char* key, path value ){			set<const char*>(key,value.c_str()); }
#endif
template<> __noinline void parser_t::set<bool>( const char* key, bool value ){			set(key,value?"true":"false"); }
template<> __noinline void parser_t::set<int>( const char* key, int value ){			snprintf(buffer,buffer_capacity,"%d",value); set(key,buffer); }
template<> __noinline void parser_t::set<uint>( const char* key, uint value ){			snprintf(buffer,buffer_capacity,"%u",value); set(key,buffer); }
template<> __noinline void parser_t::set<int64_t>( const char* key, int64_t value ){	snprintf(buffer,buffer_capacity,"%lld",value); set(key,buffer); }
template<> __noinline void parser_t::set<uint64_t>( const char* key, uint64_t value ){	snprintf(buffer,buffer_capacity,"%llu",value); set(key,buffer); }
template<> __noinline void parser_t::set<float>( const char* key, float value ){		snprintf(buffer,buffer_capacity,"%g",value); set(key,buffer); }
template<> __noinline void parser_t::set<double>( const char* key, double value ){		snprintf(buffer,buffer_capacity,"%g",value); set(key,buffer); }
template<> __noinline void parser_t::set<int2>( const char* key, int2 value ){			snprintf(buffer,buffer_capacity,"%d %d",value.x,value.y); set(key,buffer); }
template<> __noinline void parser_t::set<int3>( const char* key, int3 value ){			snprintf(buffer,buffer_capacity,"%d %d %d",value.x,value.y,value.z); set(key,buffer); }
template<> __noinline void parser_t::set<int4>( const char* key, int4 value ){			snprintf(buffer,buffer_capacity,"%d %d %d %d",value.x,value.y,value.z,value.w); set(key,buffer); }
template<> __noinline void parser_t::set<ivec2>( const char* key, ivec2 value ){		set<int2>(key,reinterpret_cast<int2&>(value)); }
template<> __noinline void parser_t::set<ivec3>( const char* key, ivec3 value ){		set<int3>(key,reinterpret_cast<int3&>(value)); }
template<> __noinline void parser_t::set<ivec4>( const char* key, ivec4 value ){		set<int4>(key,reinterpret_cast<int4&>(value)); }
template<> __noinline void parser_t::set<float2>( const char* key, float2 value ){		snprintf(buffer,buffer_capacity,"%g %g",value.x,value.y); set(key,buffer); }
template<> __noinline void parser_t::set<float3>( const char* key, float3 value ){		snprintf(buffer,buffer_capacity,"%g %g %g",value.x,value.y,value.z); set(key,buffer); }
template<> __noinline void parser_t::set<float4>( const char* key, float4 value ){		snprintf(buffer,buffer_capacity,"%g %g %g %g",value.x,value.y,value.z,value.w); set(key,buffer); }
template<> __noinline void parser_t::set<vec2>( const char* key, vec2 value ){			set<float2>(key,reinterpret_cast<float2&>(value)); }
template<> __noinline void parser_t::set<vec3>( const char* key, vec3 value ){			set<float3>(key,reinterpret_cast<float3&>(value)); }
template<> __noinline void parser_t::set<vec4>( const char* key, vec4 value ){			set<float4>(key,reinterpret_cast<float4&>(value)); }

// template specializations for set_or_clear_default()
template<> __noinline void parser_t::set_or_clear_default<const char*>( const char* key, const char* value, const char* default_value ){ if(stricmp(value,default_value)==0) return clear(key); set<const char*>(key,value); }
template<> __noinline void parser_t::set_or_clear_default<char*>( const char* key, char* value, char* default_value ){ if(stricmp(value,default_value)==0) return clear(key); set<const char*>(key,value); }
template<> __noinline void parser_t::set_or_clear_default<const char*>( const char* sec, const char* key, const char* value, const char* default_value ){ if(stricmp(value,default_value)==0) return clear(sec,key); set<const char*>(sec,key,value); }
template<> __noinline void parser_t::set_or_clear_default<char*>( const char* sec, const char* key, char* value, char* default_value ){ if(stricmp(value,default_value)==0) return clear(sec,key); set<const char*>(sec,key,value); }

//*************************************
} // namespace ini
//*************************************

#endif // __GX_INIPARSER_H__