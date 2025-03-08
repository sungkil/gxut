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
#ifndef __GX_OS_H__
#define __GX_OS_H__

#include "gxlib.h"
#if __has_include("gxstring.h")
	#include "gxstring.h"
#endif

#include <time.h>
#include <functional>

#ifdef __msvc__
	#include <psapi.h>		// EnumProcesses
	#include <tlhelp32.h>	// process info helper
	#include <intrin.h>		// processor info
#elif defined(__gcc__) // GCC
	#include <cpuid.h>
#endif

//*************************************
namespace cpu {
//*************************************
#ifdef __msvc__
	inline uint	 __get_function_count( bool extended=false ){ uint4 n={}; uint ext=extended?0x80000000:0; __cpuid((int*)&n.x,ext); return n.x-ext; }
	inline uint4 __get_cpu_id( unsigned int level, bool extended=false ){ uint ext=extended?0x80000000:0; uint4 i; __cpuidex((int*)&i.x,ext+level,0); return i; }
	inline uint64_t memory(){ uint64_t m; return GetPhysicallyInstalledSystemMemory(&m)?m:0; }
#elif defined(__gcc__) // GCC
	inline uint  __get_function_count( bool extended=false ){ uint ext=extended?0x80000000:0; unsigned int n=__get_cpuid_max(ext,0); return n-ext; }
	inline uint4 __get_cpu_id( unsigned int level, bool extended=false ){ uint ext=extended?0x80000000:0; uint4 i; __get_cpuid(ext+level,&i.x,&i.y,&i.z,&i.w); return i; }
#endif

__noinline const uint4 info( unsigned int level, bool extended=false )
{
	static std::vector<uint4> i[2]; // basic, extended
	auto& v = i[extended?1:0]; if(!v.empty()) return v[level];
	uint kn = __get_function_count(extended); if(kn<4){ return uint4{}; } v.resize(kn);
	for( unsigned int k=0; k<kn; k++ ) v[k]=__get_cpu_id(k,extended);
	return v[level];
}
__noinline const char* vendor(){ static uint4 s={}; if(!s.x){ auto v=info(0); s.x=v.y; s.y=v.z; s.z=v.w; } return (const char*)&s.x; }
__noinline const char* brand(){ static uint4 s[4]={}; if(s[0].x) return (const char*)&s[0]; s[0]=info(2,true); s[1]=info(3,true); s[2]=info(4,true); return (const char*)&s[0]; }
__noinline bool has_sse42(){ return ((info(1).z>>20)&1)==1; }

//*************************************
} // namespace cpu
//*************************************

// path/filesystem


//*************************************
namespace env {
//*************************************

inline const char* get( const char* key )
{
	if(!key||!*key) return "";
	char* v=getenv(key); return v&&*v?__strdup(v):"";
}

inline bool put( const char* key, const char* value )
{
	return 0==_putenv((std::string(key)+"="+value).c_str()); // SetEnvironmentVariableA(key,value);
}

inline std::vector<std::string> paths()
{
	std::vector<std::string> v; v.reserve(64);
	char* buff=__strdup(get("PATH")); if(!buff||!*buff) return v;
	for(char *ctx=nullptr,*token=strtok_s(buff,";",&ctx);token;token=strtok_s(nullptr,";",&ctx))
	{
		size_t l=strlen(token); if(!l||access(token,0)!=0) continue;
		v.emplace_back(token); if(v.back().back()!='\\') v.back()+='\\';
	}
	return v;
}

inline const char* where( const char* file_name )
{
	if(!file_name||!*file_name) return "";
	if(is_absolute_path(file_name)&&path_exists(file_name)) return file_name;
	std::vector<std::string> v={file_name}; std::string x=extension(file_name);
#ifdef __msvc__
	if(x.empty()){for(auto e:{".com",".exe",".bat",".cmd"}) v.emplace_back(std::string(file_name)+e);} // add the executable extensions
#endif
	for(auto& f:v) if(path_exists(f.c_str())) return absolute_path(f.c_str(),cwd());
	for(const auto& e:paths() ){ for(auto& p:v) if(path_exists((e+p).c_str())) return __strdup((e+p).c_str()); }
	return "";
}

inline void add_paths( const std::vector<std::string>& dirs )
{
	if(dirs.empty()) return;
	nocase::set<std::string> m; for( auto& p : paths() ) m.insert(p);
	std::string v; for( auto d : dirs )
	{
		d = is_relative_path(d.c_str())?absolute_path(d.c_str()):d;
		if(m.find(d)==m.end()){ v += d.c_str(); v += ';'; }
	}
	put("PATH",(v+get("PATH")).c_str());
}

inline void add_path( const std::string& d ){ add_paths( {d} ); }

//*************************************
} // end namespace env
//*************************************

//*************************************
namespace os {
//*************************************

// process-related
__noinline std::string __build_process_cmd( const char* const app, const char* args )
{
	// buffers
	std::vector<char> vcmd(4096,0);	auto* cmd=vcmd.data();
	std::vector<char> vbuf(4096,0);	auto* buf=vbuf.data();
	
	// prioritize com against exe for no-extension apps
	if(!app&&args&&*args!='\"')
	{
		strcpy(buf,args); char *ctx=nullptr, *token=strtok_s(buf," \t\n",&ctx);
		const char* t=to_backslash(token);
		std::string e=env::where(t); const char* x=extension(t);
		if(!path_exists(t)&&(!x||!*x)&&!e.empty()&&strcmp(x,"com")==0) args=strcpy(buf,strcat(strcat(strcpy(cmd,token),".com "),buf+strlen(token)+1)); // use cmd as temp
	}

	// build cmdline, which should also embed app path
	*cmd=0; bool p=app&&*app,g=args&&*args;
	if(p) strcpy(cmd,auto_quote(app)); if(p&&g) strcat(cmd," "); if(g) strcat(cmd,args);

	return cmd;
}

__noinline bool create_process( const char* app, const char* args=nullptr,
	bool wait=true, bool windowed=false, DWORD priority=NORMAL_PRIORITY_CLASS )
{
	STARTUPINFOW si={sizeof(si)}; si.dwFlags=STARTF_USESHOWWINDOW; si.wShowWindow=windowed?SW_SHOW:SW_HIDE;
	PROCESS_INFORMATION pi={};
	if(!CreateProcessW( app?atow(app):nullptr, (LPWSTR)atow(__build_process_cmd(app,args).c_str()), nullptr, nullptr, FALSE, priority, nullptr, nullptr, &si, &pi)||!pi.hProcess) return false;
	if(wait){ WaitForSingleObject(pi.hProcess,INFINITE); safe_close_handle( pi.hThread ); safe_close_handle( pi.hProcess ); }
	return true;
}

inline DWORD current_process()
{
	static DWORD curr_pid = GetCurrentProcessId();
	return curr_pid;
}

inline const std::vector<DWORD>& enum_process_indices()
{
	static std::vector<DWORD> pids(4096);
	DWORD cb_needed, npids;

	if(!EnumProcesses( &pids[0], DWORD(pids.size())*sizeof(DWORD), &cb_needed)){ pids.clear(); return pids; }
	for( npids=cb_needed/sizeof(DWORD); npids>=pids.size(); npids=cb_needed/sizeof(DWORD) )
	{
		pids.resize(2llu*npids);
		if(!EnumProcesses( &pids[0], DWORD(pids.size())*sizeof(DWORD), &cb_needed)){ pids.clear(); return pids; }
	}
	pids.resize(npids);
	return pids;
}

inline const char* get_process_path( DWORD pid )
{
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,pid); if(!hProcess) return "";
	HMODULE hMod; DWORD cbNeeded; if(!EnumProcessModules(hProcess,&hMod,sizeof(hMod),&cbNeeded)){ CloseHandle(hProcess); return ""; }
	std::array<wchar_t,4096> buff={}; GetModuleFileNameExW(hProcess,hMod,buff.data(),DWORD(buff.size())); CloseHandle(hProcess);
	return buff[0]?wtoa(buff.data()):"";
}

inline const char* get_process_name( DWORD pid )
{
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,pid); if(!hProcess) return "";
	HMODULE hMod; DWORD cbNeeded; if(!EnumProcessModules(hProcess,&hMod,sizeof(hMod),&cbNeeded)){ CloseHandle(hProcess); return ""; }
	std::array<wchar_t,4096> buff={}; GetModuleBaseNameW(hProcess,hMod,buff.data(),DWORD(buff.size())); CloseHandle(hProcess);
	return buff[0]?wtoa(buff.data()):"";
}

__noinline std::vector<DWORD> find_process( const char* name_or_path )
{
	std::vector<DWORD> v;
	static wchar_t buff[4096];
	static DWORD curr_pid = GetCurrentProcessId();
	for( auto pid : enum_process_indices() )
	{
		HMODULE hMod; DWORD cbNeeded;
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,pid); if(!hProcess) continue;
		if(!EnumProcessModules(hProcess,&hMod,sizeof(hMod),&cbNeeded)) continue;
		GetModuleBaseNameW(hProcess,hMod,buff,sizeof(buff)/sizeof(buff[0]) );
		if(_wcsicmp(buff,atow(name_or_path))==0&&pid!=curr_pid) v.push_back(pid);
		GetModuleFileNameExW(hProcess,hMod,buff,sizeof(buff)/sizeof(buff[0]) );
		if(_wcsicmp(buff,atow(name_or_path))==0&&pid!=curr_pid) v.push_back(pid);
		CloseHandle(hProcess);
	}
	return v;
}

inline bool process_exists( uint pid )
{
#ifdef _INC_TOOLHELP32
	PROCESSENTRY32 entry={}; entry.dwSize =sizeof(decltype(entry));
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0); if(hSnapShot==INVALID_HANDLE_VALUE) return false;
	BOOL hRes=Process32First( hSnapShot,&entry ); for(; hRes; hRes=Process32Next(hSnapShot,&entry) ) if(entry.th32ProcessID==pid) break;
	CloseHandle(hSnapShot);
	return hRes==TRUE;
#else
	for( auto p : enum_process_indices() )
		if(p==pid) return true;
	return false;
#endif
}

inline bool process_exists( const char* file_path )
{
#ifdef _INC_TOOLHELP32
	MODULEENTRY32W entry={}; entry.dwSize =sizeof(decltype(entry));
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0); if(hSnapShot==INVALID_HANDLE_VALUE) return false;
	BOOL hRes=Module32FirstW( hSnapShot,&entry ); for(; hRes; hRes=Module32NextW(hSnapShot,&entry) ) if(_stricmp(file_path,wtoa(entry.szExePath))==0) break;
	CloseHandle(hSnapShot);
	return hRes==TRUE;
#else
	auto v = std::move(find_process(file_path));
	return !v.empty();
#endif
}

__noinline std::string read_process( std::string cmd, const char* trims=" \t\r\n")
{
	FILE* pp = _popen(cmd.c_str(),"rb"); if(!pp) return "";
	std::vector<char> v; v.reserve(1024); char buff[64]={}; size_t n=0; while( n=fread(buff,1,sizeof(buff),pp) ) v.insert(v.end(),buff,buff+n); v.emplace_back(0);
	bool b_eof= feof(pp); _pclose(pp); if(!b_eof) printf("%s(%s): broken pipe\n", __func__, cmd.c_str() );
	char* s=v.data(); return trim(is_utf8(s)?atoa(s,CP_UTF8,0):s, trims);  // auto convert CP_UTF8 to current code page
}

// general dynamic linking wrapper with DLL
struct dll_t
{
	HMODULE hdll = nullptr;

	~dll_t(){ release(); }
	void release(){ if(hdll){ FreeLibrary(hdll); hdll=nullptr; } }
	const char* file_path(){ static char f[_MAX_PATH]={}; wchar_t w[_MAX_PATH]; if(hdll) GetModuleFileNameW(hdll,w,_MAX_PATH); return strcpy(f,wtoa(w)); }
	bool load( const char* dll_path ){ return nullptr!=(hdll=LoadLibraryW(atow(dll_path))); }
	template <class T> T get_proc_address( const char* name ) const { return hdll==nullptr?nullptr:(T)GetProcAddress(hdll,name); }
	template <class T> T* get_proc_address( const char* name, T*& p ) const { return hdll==nullptr?p=nullptr:p=(T*)GetProcAddress(hdll,name); }
	operator bool() const { return hdll!=nullptr; }
};

//*************************************
} // end namespace os
//*************************************

#if defined(__msvc__) && __has_include("gxwin32.h")
	#include "gxwin32.h"
#endif

#endif // __GX_OS_H__
