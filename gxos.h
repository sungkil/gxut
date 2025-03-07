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

#include "gxtype.h"
#if __has_include("gxstring.h")
	#include "gxstring.h"
#endif

#include <time.h>
#include <functional>

#if defined(_MSC_VER) && !defined(__clang__) // Visual Studio without clang
	#include <psapi.h>		// EnumProcesses
	#include <tlhelp32.h>	// process info helper
	#include <intrin.h>		// processor info
	#if __has_include("gxwin32.h") // Visual Studio without clang
		#include "gxwin32.h"
	#endif
#elif defined(__GNUC__) // GCC
	#include <cpuid.h>
#endif

//*************************************
namespace os {
//*************************************

//*************************************
namespace cpu {
//*************************************
#if defined(_MSC_VER) && !defined(__clang__) // Visual Studio without clang
	inline uint	 __get_function_count( bool extended=false ){ uint4 n={}; uint ext=extended?0x80000000:0; __cpuid((int*)&n.x,ext); return n.x-ext; }
	inline uint4 __get_cpu_id( unsigned int level, bool extended=false ){ uint ext=extended?0x80000000:0; uint4 i; __cpuidex((int*)&i.x,ext+level,0); return i; }
	inline uint64_t memory(){ uint64_t m; return GetPhysicallyInstalledSystemMemory(&m)?m:0; }
#elif defined(__GNUC__) // GCC
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
} // namespace os::cpu
//*************************************

// executable path
#ifdef __GX_FILESYSTEM_H__
#else
#endif

//*************************************
namespace env {
//*************************************

inline const char* var( const char* key )
{
	char* v=getenv(key); if(!v||!*v) return "";
	static std::vector<char> buff(4096); size_t l=strlen(v); if(l>4095) buff.resize(l+1);
	return strcpy(buff.data(),v);
}

#ifdef __GX_FILESYSTEM_H__
inline std::vector<path> paths()
{
	std::vector<path> v; v.reserve(64);
	char* buff = (char*) var("PATH"); if(!buff||!*buff) return v;
	for(char *ctx=nullptr,*token=strtok_s(buff,";",&ctx);token;token=strtok_s(nullptr,";",&ctx))
	{
		if(!*token) continue;
		path t=path(token).canonical().add_backslash();
		if(t.is_absolute()&&t.exists()) v.emplace_back(t);
	}
	return v;
}

inline path where( const char* file_name )
{
	if(!file_name||!*file_name) return "";
	path f(file_name); if(f.is_absolute()&&f.exists()) return f;
	std::vector<path> v={f}; if(f.ext().empty()){for(auto e:{".com",".exe",".bat",".cmd"}) v.emplace_back(f+e);} // add the executable extensions
	for(auto& p:v) if(p.exists()) return p.absolute(cwd());
	for(const auto& e:paths() ){ for(auto& p:v) if((e+p).exists()) return (e+p).canonical(); }
	return path();
}

inline void add_paths( const std::vector<path>& dirs )
{
	if(dirs.empty()) return;
	nocase::set<path> m; for( auto& p : paths() ) m.insert(p.canonical());
	std::string v; for( auto d : dirs )
	{
		d = (d.is_relative()?d.absolute():d).canonical();
		if(m.find(d)==m.end()){ v += d.c_str(); v += ';'; }
	}
	SetEnvironmentVariableA( "PATH", (v+var("PATH")).c_str() );
}

inline void add_path( path d )
{
	add_paths( std::vector<path>{d} );
}

inline bool set_dll_directory( path dir )
{
	return SetDllDirectoryW(atow(dir.c_str()))?true:false;
}
#endif // __GX_FILESYSTEM_H__

//*************************************
} // end namespace env
//*************************************

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

#endif // __GX_OS_H__
