#pragma once
#ifndef __GX_INIPARSER_H__
#define __GX_INIPARSER_H__

#if defined(__has_include)&&__has_include("gxstring.h")&&__has_include("gxfilesystem.h")
	#include "gxstring.h"
	#include "gxfilesystem.h"
#endif

//*************************************
namespace ini {
//*************************************

// case-insensitive keys ("SECTIONNAME:KEYNAME") and values (wchar_t)
// important features: can handle multiple values for a single key

struct entry_t
{
	size_t index; std::string section, key; std::wstring value;
	entry_t( size_t idx, const char* Sec, const char* Key ):index(idx),section(Sec),key(Key){};
	static bool compare_by_index( entry_t* e0, entry_t* e1 ){ return e0->index < e1->index; }
};

struct auto_lock_t
{
	CRITICAL_SECTION& cs_r;
	auto_lock_t( CRITICAL_SECTION& cs ):cs_r(cs){ EnterCriticalSection(&cs_r); }
	~auto_lock_t(){ LeaveCriticalSection(&cs_r); }
};

struct file_time_t { int s,m,h,d,M,y; };	// sec, min, hour, day, month, year: aligned by the struct tm
using dictionary_t = nocase::map<std::string,entry_t*>;

struct parser_t
{
protected:

	path				file_path;	// current file path
	bool				b_batch = false;
	file_time_t			mtime = {};
	CRITICAL_SECTION	cs;			// for load/save
	dictionary_t		dic;		// case-insensitive dictionary map
	static const size_t	buffer_capacity=4096;
	wchar_t				buffer[buffer_capacity+1]={};

	entry_t* get_or_create_entry( const char* seckey ){ if(seckey==nullptr||seckey[0]==L'\0') return nullptr; auto it=dic.find(seckey); if(it!=dic.end())return it->second; auto ss=split_section_key(seckey); return dic[seckey]=new entry_t(dic.size(),ss.first,ss.second); }
	std::pair<const char*,const char*> split_section_key( const char* seckey ){ char *sk=__tstrdup(seckey),*colon=(char*)strchr(&sk[0],':');if(colon==nullptr)return std::make_pair("",sk);else{colon[0]=0;return std::make_pair(sk,colon+1);} }

public:
	~parser_t(){ DeleteCriticalSection(&cs); }
	parser_t(){ InitializeCriticalSectionAndSpinCount(&cs,2000); }
	parser_t( path file_path ):parser_t(){ set_path(file_path); }

	// query and retrieval
	void set_path( const path& file_path ){ this->file_path = file_path; }
	const path& get_path() const { return file_path; }
	bool key_exists( const char* key ) const { if(key==nullptr||key[0]=='\0') return false; return dic.find(key)!=dic.end(); }
	bool key_exists( const char* sec, const char* key ) const { if(!key||!*key) return false; if(!sec||!*sec) return key_exists(key); char sk[4096]; sprintf_s(sk,4096,"%s:%s",sec,key); return dic.find(sk)!=dic.end(); }
	bool section_exists( const char* sec ) const { if(sec==nullptr||sec[0]=='\0') return false; for(auto& it:dic) if(_stricmp(it.second->section.c_str(),sec)==0) return true; return false; }
	std::set<std::string> section_set() const { std::set<std::string> ss;for(auto& it:dic)ss.emplace(it.second->section);return ss;}
	std::vector<std::string> sections(){ std::vector<std::string> sl; std::set<std::string> ss; for(auto& it:entries()){ if(ss.find(it->section.c_str())!=ss.end()) continue; sl.emplace_back(it->section); ss.emplace(it->section); } return sl; }
	std::vector<entry_t*> entries( const char* sec=nullptr ){ std::vector<entry_t*> el; for(auto& it:dic) if(sec==nullptr||_stricmp(it.second->section.c_str(),sec)==0) el.emplace_back(it.second); std::sort(el.begin(),el.end(),entry_t::compare_by_index); return el; } // return all entries if no section is given

	// clear
	void clear(){ for(auto& it:dic)if(it.second!=nullptr){delete it.second;} dic.clear(); save(); }
	void clear_entry( const char* seckey ){ auto it=dic.find(seckey);if(it==dic.end())return; delete it->second; dic.erase(it); save(); }
	void clear_entry( const char* sec, const char* key ){ char sk[4096]; sprintf_s(sk,4096,"%s:%s",sec,key); clear_entry(sk); save(); }
	void clear_section( const std::string& sec ){ bool b_save=false; for(auto it=dic.begin();it!=dic.end();){if(_stricmp(it->second->section.c_str(),sec.c_str())==0){delete it->second;it=dic.erase(it);b_save=true; }else it++;} save(); }

	// load/save
	void begin_update(){ b_batch=true; }
	void end_update(){ b_batch=false; }
	bool load(){ return load(file_path); }
	bool load( const path& file_path );
	bool save(){ return save(file_path); }
	bool save( const path& file_path );

	// get
	__forceinline const wchar_t* operator()( const char* key ){	return get(key); }
	__forceinline const wchar_t* operator[]( const char* key ){	return get(key); }
	__forceinline const wchar_t* get( const char* key ){ auto it=dic.find(key); return it==dic.end()?L"":it->second->value.c_str();}
	__forceinline const wchar_t* get( const char* sec, const char* key ){ char sk[257]; sprintf_s(sk,256,"%s:%s",sec,key); return get(sk); }
	template<class T> T get( const char* key );
	template<class T> T get( const char* sec, const char* key ){ char sk[257]; sprintf_s(sk,256,"%s:%s",sec,key); return get<T>(sk); }

	// set
	template<class T> void set( const char* key, T value );
	template<class T> void set( const char* sec, const char* key, T value ){ char sk[257]; sprintf_s(sk,256,"%s:%s",sec,key); set<T>(sk,value); }
};

__noinline bool parser_t::load( const path& file_path )
{
	if(file_path.empty()) return false;
	struct _stat s; if(_wstat(file_path,&s)!=0) return false;

	// bypass non-modified file
	file_time_t mt={}; memcpy(&mt,_gmtime64(&s.st_mtime),sizeof(file_time_t));mt.y+=1900;mt.M+=1;
	if(memcmp(&mtime,&mt,sizeof(file_time_t))==0) return true; mtime = mt;

	// open now
	auto_lock_t lock(cs);
	FILE* fp=file_path.fopen(L"r",true);if(fp==nullptr){ printf("Unable to open %s",file_path.wtoa()); return false; }

	// clear dictionary
	for(auto& it:dic){if(it.second!=nullptr){delete it.second;}} dic.clear();

	wchar_t sec[512],seckey[512];
	memset(buffer,0,sizeof(buffer));
	while(fgetws(buffer,buffer_capacity,fp))
	{
		wchar_t* b=itrim(buffer); if(!*b||*b==L'#'||*b==L';') continue; // skip comment only in the first character
		while(b) // split by the right bracket or semicolon (for single-line multistatements)
		{
			wchar_t* e=b; while(*e&&*e!=L']'&&*e!=L';')e++;bool eol=(*e==0);*e=0;
			auto* t=itrim(b); b=eol?0:itrim(e+1);

			int l=int(wcslen(t)); if(l<2) continue;
			if(t[0]==L'['){ wcscpy(sec,itrim(t+1)); continue; } // assign section
			wchar_t* v=0; for(int k=0;k<l;k++)if(t[k]==L'='){t[k]=0;v=t+k+1;break;} t=itrim(t);v=itrim(v); if(!t||!t[0]||!v) continue;
			swprintf_s(seckey,L"%s:%s",sec,t);get_or_create_entry(wtoa(seckey))->value=v;
		}
	}
	
	fclose(fp);
	this->file_path=file_path.absolute();

	return true;
}

__noinline bool parser_t::save( const path& file_path )
{
	if(b_batch) return false;
	if(file_path.empty()){ printf( "%s(): file_path is empty\n", __func__ ); return false; }

	auto_lock_t lock(cs);

	bool b_hidden_file=false;if(_waccess(file_path,0)==0&&(GetFileAttributesW(file_path)&FILE_ATTRIBUTE_HIDDEN)){b_hidden_file=true;SetFileAttributesW( file_path,GetFileAttributesW(file_path)&(~FILE_ATTRIBUTE_HIDDEN) );} // save and remove hidden attribute
	FILE* fp=nullptr; for(uint k=0;fp==nullptr&&k<20;k++){ fp=file_path.fopen(L"w",true); Sleep(5); } // wait 100ms for busy writing
	if(fp==nullptr){ printf( "%s(): Unable to open %s to write", __func__, file_path.wtoa() ); return false; }
	fseek( fp, 0, SEEK_SET ); // remove BOM
	
	std::string sec=""; bool bLine0=true;
	for( auto it=dic.begin(); it!=dic.end(); )
	{
		entry_t* e=it->second;
		e->section = trim(e->section.c_str());
		e->value = trim(e->value.c_str());
		if(e->section.empty()||e->value.empty()){ it=dic.erase(it); continue; }

		if(sec!=e->section){fwprintf(fp,L"%s[%s]\n",bLine0?L"":L"\n",atow(e->section.c_str()));sec=e->section;}
		fwprintf(fp,L"%s = %s\n",atow(e->key.c_str()),e->value.c_str());bLine0=false;

		it++;
	}
	fclose(fp);
	if(b_hidden_file) SetFileAttributesW( file_path,GetFileAttributesW(file_path)|FILE_ATTRIBUTE_HIDDEN ); // restore hidden attribute
	
	return true;
}

// template specializations for get()
template<> __noinline std::string parser_t::get<std::string>(const char* key){	auto* v=get(key); return *v==0?"":std::string(wtoa(v)); }
template<> __noinline path parser_t::get<path>( const char* key ){				auto* v=get(key); return *v==0?path():path(v); }
template<> __noinline bool parser_t::get<bool>( const char* key ){				auto* v=get(key); return *v==0?false:_wcsicmp(v,L"true")==0?true:(_wtoi(v)!=0); }
template<> __noinline int parser_t::get<int>( const char* key ){				auto* v=get(key); return *v==0?0:_wtoi(v); }
template<> __noinline uint parser_t::get<uint>( const char* key ){				return uint(get<int>(key)); }
template<> __noinline int64_t parser_t::get<int64_t>( const char* key ){		auto* v=get(key); return *v==0?0:_wtoi64(v); }
template<> __noinline uint64_t parser_t::get<uint64_t>( const char* key ){		return uint64_t(get<int64_t>(key)); }
template<> __noinline float parser_t::get<float>( const char* key ){			auto* v=get(key); return *v==0?0:float(_wtof(v)); }
template<> __noinline int2 parser_t::get<int2>( const char* key ){				auto* v=get(key); return *v==0?int2{}:wtoi2(v); }
template<> __noinline int3 parser_t::get<int3>( const char* key ){				auto* v=get(key); return *v==0?int3{}:wtoi3(v); }
template<> __noinline int4 parser_t::get<int4>( const char* key ){				auto* v=get(key); return *v==0?int4{}:wtoi4(v); }
template<> __noinline float2 parser_t::get<float2>( const char* key ){			auto* v=get(key); return *v==0?float2{}:wtof2(v); }
template<> __noinline float3 parser_t::get<float3>( const char* key ){			auto* v=get(key); return *v==0?float3{}:wtof3(v); }
template<> __noinline float4 parser_t::get<float4>( const char* key ){			auto* v=get(key); return *v==0?float4{}:wtof4(v); }

// template specializations for set()
template<> __noinline void parser_t::set<const wchar_t*>( const char* key, const wchar_t* value ){ bool b=key_exists(key); entry_t* e=get_or_create_entry(key); if(b&&e->value==value) return; e->value=value; save(); }
template<> __noinline void parser_t::set<const char*>( const char* key, const char* value ){ set<const wchar_t*>(key,atow(value)); }
template<> __noinline void parser_t::set<wchar_t*>( const char* key, wchar_t* value ){	set<const wchar_t*>(key,(const wchar_t*)value); }
template<> __noinline void parser_t::set<char*>( const char* key, char* value ){		set<const wchar_t*>(key,atow(value)); }
template<> __noinline void parser_t::set<path>( const char* key, path value ){			set<const wchar_t*>(key,value.c_str()); }
template<> __noinline void parser_t::set<bool>( const char* key, bool value ){			set(key,value?L"1":L"0"); }
template<> __noinline void parser_t::set<int>( const char* key, int value ){			swprintf_s(buffer,buffer_capacity,L"%d",value); set(key,buffer); }
template<> __noinline void parser_t::set<uint>( const char* key, uint value ){			swprintf_s(buffer,buffer_capacity,L"%u",value); set(key,buffer); }
template<> __noinline void parser_t::set<int64_t>( const char* key, int64_t value ){	swprintf_s(buffer,buffer_capacity,L"%lld",value); set(key,buffer); }
template<> __noinline void parser_t::set<uint64_t>( const char* key, uint64_t value ){	swprintf_s(buffer,buffer_capacity,L"%llu",value); set(key,buffer); }
template<> __noinline void parser_t::set<float>( const char* key, float value ){		swprintf_s(buffer,buffer_capacity,L"%g",value); set(key,buffer); }
template<> __noinline void parser_t::set<double>( const char* key, double value ){		swprintf_s(buffer,buffer_capacity,L"%g",value); set(key,buffer); }
template<> __noinline void parser_t::set<int2>( const char* key, int2 value ){			swprintf_s(buffer,buffer_capacity,L"%d %d",value.x,value.y); set(key,buffer); }
template<> __noinline void parser_t::set<int3>( const char* key, int3 value ){			swprintf_s(buffer,buffer_capacity,L"%d %d %d",value.x,value.y,value.z); set(key,buffer); }
template<> __noinline void parser_t::set<int4>( const char* key, int4 value ){			swprintf_s(buffer,buffer_capacity,L"%d %d %d %d",value.x,value.y,value.z,value.w); set(key,buffer); }
template<> __noinline void parser_t::set<ivec2>( const char* key, ivec2 value ){		set<int2>(key,value); }
template<> __noinline void parser_t::set<ivec3>( const char* key, ivec3 value ){		set<int3>(key,value); }
template<> __noinline void parser_t::set<ivec4>( const char* key, ivec4 value ){		set<int4>(key,value); }
template<> __noinline void parser_t::set<float2>( const char* key, float2 value ){		swprintf_s(buffer,buffer_capacity,L"%g %g",value.x,value.y); set(key,buffer); }
template<> __noinline void parser_t::set<float3>( const char* key, float3 value ){		swprintf_s(buffer,buffer_capacity,L"%g %g %g",value.x,value.y,value.z); set(key,buffer); }
template<> __noinline void parser_t::set<float4>( const char* key, float4 value ){		swprintf_s(buffer,buffer_capacity,L"%g %g %g %g",value.x,value.y,value.z,value.w); set(key,buffer); }
template<> __noinline void parser_t::set<vec2>( const char* key, vec2 value ){			set<float2>(key,value); }
template<> __noinline void parser_t::set<vec3>( const char* key, vec3 value ){			set<float3>(key,value); }
template<> __noinline void parser_t::set<vec4>( const char* key, vec4 value ){			set<float4>(key,value); }

//*************************************
} // namespace ini
//*************************************

#endif // __GX_INIPARSER_H__