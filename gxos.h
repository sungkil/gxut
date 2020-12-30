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
#ifndef __GX_OS_H__
#define __GX_OS_H__

#include "gxtype.h"
#include <time.h>

#if defined(__has_include) && __has_include("gxfilesystem.h") && !defined(__GX_FILESYSTEM_H__)
	#include "gxfilesystem.h"
#endif

#if defined(_WIN32)||defined(_WIN64) // Windows
	#include <shlobj.h>		// for SHGetKnownFolderPath
#endif

#if defined(__has_include) && __has_include(<psapi.h>)
	#include <psapi.h>	// EnumProcesses
#endif
#ifndef __GNUC__		// MinGW has a problem with threads
	#include <thread>	// usleep
	#include <chrono>	// microtimer
#endif

//*************************************
namespace os {
//*************************************

//***********************************************
namespace env {
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
} // namespace env
//***********************************************

//***********************************************
// system paths
inline path temp()
{
	static path t; if(!t.empty()) return t;
	GetTempPathW(path::capacity,t); t=t.absolute().add_backslash();
	path tmp=t.drive()+L"\\temp\\"; if(t==tmp.junction()) t=tmp; t[0]=::toupper(t[0]);
	if(t.volume().has_free_space()) return t;
	wprintf(L"temp(): not enough space in %s\n", t.c_str() );
	t=path::module_dir()+L"temp\\"; if(!t.exists()) t.mkdir(); wprintf(L"temp(): %s is used instead", t.c_str() );
	if(!t.volume().has_free_space()) wprintf(L", but still not enough space." );
	wprintf(L"\n");
	return t;
}

#ifdef _SHLOBJ_H_
inline path local_appdata()
{
	path e=env::var(L"LOCALAPPDATA"); if(!e.empty()&&e.exists()) return e.add_backslash();
	SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, (wchar_t*)e );
	return e.add_backslash();
}
#endif

inline path system_dir()
{
	static path s;if(!s.empty())return s;
	GetSystemDirectoryW(s,path::capacity);
	return s=s.add_backslash();
}

//***********************************************

inline const char* get_last_error(){ static char buff[4096]={};DWORD e=GetLastError();char *s;FormatMessageA( FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_IGNORE_INSERTS,nullptr,e,MAKELANGID(LANG_ENGLISH,SUBLANG_DEFAULT),(LPSTR)&s,0,nullptr);sprintf(buff,"%s (code=%x)",s,uint(e));LocalFree(s);return buff; }
inline void exit( const char* fmt, ... ){ va_list a; va_start(a,fmt); std::vector<char> buff(_vscprintf(fmt,a)+1); vsprintf_s(&buff[0],buff.size(),fmt,a); va_end(a); fprintf( stdout, "[%s] %s", path::module_path().name(false).wtoa(), &buff[0] ); ::exit(EXIT_FAILURE); }
inline void exit( const wchar_t* fmt, ... ){ va_list a; va_start(a,fmt); std::vector<wchar_t> buff(_vscwprintf(fmt,a)+1); vswprintf_s(&buff[0],buff.size(),fmt,a); va_end(a); fwprintf( stdout, L"[%s] %s", path::module_path().name(false).c_str(), &buff[0] ); ::exit(EXIT_FAILURE); }
#if !defined(__GNUC__)&&((__cplusplus>199711L)||(_MSC_VER>=1600/*VS2010*/))
inline void usleep( int us ){ std::this_thread::sleep_for(std::chrono::microseconds(us)); }
#endif

// timer
#ifndef __GX_OS_TIMER__
#define __GX_OS_TIMER__
struct timer_t
{
	union { double2 result; struct { double x, y; }; };
	inline timer_t(){ begin(); }
	inline void clear(){ x=y=now(); }
	inline void begin(){ x=now(); }
	inline double end(){ return (y=now())-x; }
	inline double delta(){ return y-x; }
	static double now(){ double c=freq_scale(); int64_t e=epoch(); LARGE_INTEGER li; QueryPerformanceCounter(&li); return double(li.QuadPart-e)*c; } // if rex found, use its epoch; otherwise, use a local epoch
	static timer_t* singleton(){ static timer_t i; return &i; }
	static double freq_scale(){ static double c=0; if(c==0){ LARGE_INTEGER li; QueryPerformanceFrequency(&li); c=1000.0/double(li.QuadPart); } return c; }
	static int64_t epoch(){ static int64_t e=0; if(e==0){ auto* ef=(int64_t(*)()) GetProcAddress(GetModuleHandleW(nullptr),"rex_timer_epoch"); e=ef?ef():0; if(e==0){ LARGE_INTEGER li; QueryPerformanceCounter(&li); e=li.QuadPart;} } return e; }
};
#endif // __GX_OS_TIMER__

class mutex_t
{
	HANDLE h_mutex=nullptr;
public:
	mutex_t( const wchar_t* name );
	~mutex_t(){ close(); }
	operator bool() const { return h_mutex!=nullptr; }
	HANDLE& close(){ if(h_mutex) CloseHandle(h_mutex); return h_mutex=0; }
};

inline mutex_t::mutex_t( const wchar_t* name )
{
	close()=CreateMutexW(0,FALSE,name); DWORD e=GetLastError();
	for(uint k=0,kn=256;k<kn&&e==ERROR_ALREADY_EXISTS;k++){ close()=CreateMutexW(0,FALSE,name);e=GetLastError(); Sleep(10); }
	if(e==ERROR_ALREADY_EXISTS) close();
}

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

inline std::vector<HWND> enum_windows( const wchar_t* filter=nullptr )
{
	auto __enum_windows_proc = []( HWND hwnd , LPARAM pProcessList ) -> BOOL { ((std::vector<HWND>*)pProcessList)->emplace_back(hwnd); return TRUE; };
	std::vector<HWND> v; EnumWindows(__enum_windows_proc,(LPARAM)(&v));
	return v;
}

//*************************************
#ifdef _PSAPI_H_
//*************************************

inline DWORD current_process(){ static DWORD curr_pid = GetCurrentProcessId(); return curr_pid; }

inline const std::vector<DWORD>& enum_process()
{
	static std::vector<DWORD> pids(4096);
	DWORD cb_needed, npids;

	if(!EnumProcesses( &pids[0], DWORD(pids.size())*sizeof(DWORD), &cb_needed)){ pids.clear(); return pids; }
	for( npids=cb_needed/sizeof(DWORD); npids>=pids.size(); npids=cb_needed/sizeof(DWORD) )
	{
		pids.resize(npids*2);
		if(!EnumProcesses( &pids[0], DWORD(pids.size())*sizeof(DWORD), &cb_needed)){ pids.clear(); return pids; }
	}
	pids.resize(npids);
	return pids;
}

inline std::vector<DWORD> find_process( const wchar_t* process_name )
{
	std::vector<DWORD> v;
	static wchar_t buff[4096];
	static DWORD curr_pid = GetCurrentProcessId();
	for( auto pid : enum_process() )
	{
		HMODULE hMod; DWORD cbNeeded;
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,pid); if(!hProcess) continue;
		if(!EnumProcessModules(hProcess,&hMod,sizeof(hMod),&cbNeeded)) continue;
		GetModuleBaseNameW(hProcess,hMod,buff,sizeof(buff)/sizeof(wchar_t) );
		CloseHandle(hProcess);
		if(_wcsicmp(buff,process_name)==0&&pid!=curr_pid) v.push_back(pid);
	}
	return v;
}

namespace console 
{
inline const wchar_t* title(){ static wchar_t buff[MAX_PATH+1]; GetConsoleTitleW(buff,MAX_PATH); return buff; }
inline DWORD process(){ DWORD console; GetWindowThreadProcessId(GetConsoleWindow(),& console); return console; }
inline bool has_parent(){ return GetCurrentProcessId()!=process(); }
}

//*************************************
#endif // _PSAPI_H_
//*************************************

//*************************************
} // namespace os

//*************************************
#endif // __GX_OS_H__
