//*******************************************************************
// Copyright 2011-2022 Sungkil Lee
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

#if defined(__has_include) && __has_include(<tlhelp32.h>)
	#include <tlhelp32.h> // process info helper
#endif

#ifndef __GNUC__		// MinGW has a problem with threads
	#include <thread>	// usleep
	#include <chrono>	// microtimer
#endif

//*************************************
namespace os {
//*************************************

inline const wchar_t* get_last_error(){ static wchar_t buff[4096]={};DWORD e=GetLastError();wchar_t *s;FormatMessageW( FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_IGNORE_INSERTS,nullptr,e,MAKELANGID(LANG_ENGLISH,SUBLANG_DEFAULT),(LPWSTR)&s,0,nullptr);wsprintf(buff,L"%s (code=%x)",s,uint(e));LocalFree(s);return buff; }
inline void exit( const char* fmt, ... ){ va_list a; va_start(a,fmt); std::vector<char> buff(_vscprintf(fmt,a)+1); vsprintf_s(&buff[0],buff.size(),fmt,a); va_end(a); fprintf( stdout, "[%s] %s", path::module_path().name(false).wtoa(), &buff[0] ); ::exit(EXIT_FAILURE); }
inline void exit( const wchar_t* fmt, ... ){ va_list a; va_start(a,fmt); std::vector<wchar_t> buff(_vscwprintf(fmt,a)+1); vswprintf_s(&buff[0],buff.size(),fmt,a); va_end(a); fwprintf( stdout, L"[%s] %s", path::module_path().name(false).c_str(), &buff[0] ); ::exit(EXIT_FAILURE); }
#if !defined(__GNUC__)&&((__cplusplus>199711L)||(_MSC_VER>=1600/*VS2010*/))
inline void usleep( int us ){ std::this_thread::sleep_for(std::chrono::microseconds(us)); }
#endif
inline bool shutdown()
{
	HANDLE hToken; if(!OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,&hToken)) return false;
	TOKEN_PRIVILEGES tkp; LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
	tkp.PrivilegeCount=1; tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0); if(GetLastError()!=ERROR_SUCCESS) return false;
	return ExitWindowsEx(EWX_POWEROFF|EWX_SHUTDOWN|EWX_FORCE,SHTDN_REASON_FLAG_PLANNED|SHTDN_REASON_MAJOR_APPLICATION|SHTDN_REASON_MINOR_OTHER);
}

inline bool is_dos_cmd( const wchar_t* cmd )
{
	static std::set<std::wstring> dos_cmd_map = 
	{
		L"append", L"attrib",
		L"batch",
		L"call", L"cd", L"chdir", L"cls", L"cmd", L"comp", L"copy",
		L"date", L"del", L"delete", L"deltree", L"dir",
		L"echo", L"edit", L"erase", L"exit", L"expand",
		L"fc", L"find", L"findstr", L"for", L"ftp", L"ftype",
		L"getmac",
		L"hostname",
		L"if", L"ipconfig",
		L"md", L"mkdir", L"mklink", L"move",
		L"net",
		L"pause", L"popd", L"pushd",
		L"rd", L"reg", L"rename", L"replace", L"rmdir", L"robocopy", L"runas",
		L"set", L"setlocal", L"setver", L"shutdown", L"sort", L"start", L"subst",
		L"taskkill", L"tasklist", L"time", L"timeout", L"title", L"tree", L"type",
		L"undelete",
		L"ver",
		L"xcopy",
		L"w32tm",
	};
	return cmd&&*cmd&&dos_cmd_map.find(cmd)!=dos_cmd_map.end();
}

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

inline path search_executable( path file_name )
{
	if(file_name.empty()||file_name.exists()) return file_name;
	if(file_name.is_absolute()) return L"";
	// add the executable extensions
	std::vector<path> v; if(!file_name.ext().empty()) v={file_name};
	else for(auto x:{L".com",L".exe",L".bat",L".cmd"}) v.emplace_back( file_name+x );
	for(auto& p:v)
	{
		if(p.exists()) return p;
		for( const auto& e : paths() ) if((e+p).exists()) return e+p;
	}
	return L"";
}

//***********************************************
} // namespace env
//***********************************************

//***********************************************
// system variables/paths
inline path computer_name( bool b_lowercase=true )
{
	wchar_t cname[1024]={0}; DWORD cl=sizeof(cname)/sizeof(cname[0]); GetComputerNameW( cname, &cl );
	if(b_lowercase) for(size_t k=0,kn=wcslen(cname);k<kn;k++) cname[k]=tolower(cname[k]);
	return path(cname);
}

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

inline path userprofile()
{
	path e=env::var(L"USERPROFILE");
#ifdef _SHLOBJ_H_
	if(e.empty()||!e.exists()) SHGetFolderPath(NULL, CSIDL_PROFILE, NULL, 0, (wchar_t*)e );
#endif
	return e.empty()||!e.exists()?path():e.add_backslash();
}

inline path home()
{
	return userprofile();
}

inline const wchar_t* user()
{
	static std::wstring u=env::var(L"USERNAME");
	return u.c_str();
}

inline path local_appdata()
{
	path e=env::var(L"LOCALAPPDATA");
#ifdef _SHLOBJ_H_
	if(e.empty()||!e.exists()) SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, (wchar_t*)e );
#endif
	return e.empty()||!e.exists()?path():e.add_backslash();
}

inline path appdata()
{
	path e=env::var(L"APPDATA");
#ifdef _SHLOBJ_H_
	if(e.empty()||!e.exists()) SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, (wchar_t*)e );
#endif
	return e.empty()||!e.exists()?path():e.add_backslash();
}

inline path system_dir()
{
	static path s;if(!s.empty())return s;
	GetSystemDirectoryW(s,path::capacity);
	return s=s.add_backslash();
}

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
	struct params { std::vector<HWND> v; const wchar_t* filter; };
	auto __enum_windows_proc = []( HWND hwnd , LPARAM pParams )->BOOL
	{
		params* p = ((params*)pParams);
		if(p->filter){ wchar_t buff[4096]; GetWindowTextW(hwnd,buff,4096); if(!wcsstr(buff,p->filter)) return TRUE; }
		p->v.emplace_back(hwnd); return TRUE;
	};
	params p; p.filter=filter;
	EnumWindows(__enum_windows_proc,(LPARAM)(&p));
	return p.v;
}

inline HWND find_window( const wchar_t* filter )
{
	HWND h = GetTopWindow(GetDesktopWindow());
	for(; h!=nullptr; h=GetWindow(h, GW_HWNDNEXT))
	{
		wchar_t buff[4096]; GetWindowTextW(h,buff,4096);
		if(wcsstr(buff,filter)) return h;
	}
	return nullptr;
}

//*************************************
// process
__noinline bool create_process( const wchar_t* app=nullptr, const wchar_t* arguments=nullptr, bool bShowWindow=true, bool bWaitFinish=false )
{
	wchar_t *exe=(wchar_t*) malloc(sizeof(wchar_t)*4096), *cmd=(wchar_t*) malloc(sizeof(wchar_t)*4096);
	if(!app||!*app) exe[0]=0; else swprintf_s( exe, 4096, L"%s ", path(app).auto_quote().c_str() );
	swprintf_s( cmd, 4096, L"%s%s", exe, arguments?arguments:L"" );
	PROCESS_INFORMATION pi={}; STARTUPINFOW si={}; si.cb=sizeof(si); si.dwFlags=STARTF_USESHOWWINDOW; si.wShowWindow=bShowWindow?SW_SHOW:SW_HIDE;
	BOOL r=CreateProcessW( app, (LPWSTR)cmd, nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS, nullptr, nullptr, &si, &pi );
	if(r&&bWaitFinish&&pi.hProcess) WaitForSingleObject(pi.hProcess,INFINITE);
	if(pi.hThread) CloseHandle( pi.hThread );
	if(pi.hProcess) CloseHandle( pi.hProcess );
	if(exe) free(exe); if(cmd) free(cmd);
	return bool(r);
}

//*************************************
#ifdef _PSAPI_H_
//*************************************

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
		pids.resize(npids*2);
		if(!EnumProcesses( &pids[0], DWORD(pids.size())*sizeof(DWORD), &cb_needed)){ pids.clear(); return pids; }
	}
	pids.resize(npids);
	return pids;
}

inline std::vector<DWORD> find_process( const wchar_t* name_or_path )
{
	std::vector<DWORD> v;
	static wchar_t buff[4096];
	static DWORD curr_pid = GetCurrentProcessId();
	for( auto pid : enum_process_indices() )
	{
		HMODULE hMod; DWORD cbNeeded;
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,pid); if(!hProcess) continue;
		if(!EnumProcessModules(hProcess,&hMod,sizeof(hMod),&cbNeeded)) continue;
		GetModuleBaseNameW(hProcess,hMod,buff,sizeof(buff)/sizeof(wchar_t) );
		if(_wcsicmp(buff,name_or_path)==0&&pid!=curr_pid) v.push_back(pid);
		GetModuleFileNameExW(hProcess,hMod,buff,sizeof(buff)/sizeof(wchar_t) );
		if(_wcsicmp(buff,name_or_path)==0&&pid!=curr_pid) v.push_back(pid);
		CloseHandle(hProcess);
	}
	return v;
}

inline bool process_exists( uint pid )
{
#ifdef _INC_TOOLHELP32
	PROCESSENTRY32W entry; entry.dwSize =sizeof(decltype(entry));
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0); if(hSnapShot==INVALID_HANDLE_VALUE) return false;
	BOOL hRes=Process32First( hSnapShot,&entry ); for(; hRes; hRes=Process32NextW(hSnapShot,&entry) ) if(entry.th32ProcessID==pid) break;
	CloseHandle(hSnapShot);
	return hRes==TRUE;
#else
	for( auto p : enum_process_indices() )
		if(p==pid) return true;
	return false;
#endif
}

inline bool process_exists( path file_path )
{
#ifdef _INC_TOOLHELP32
	MODULEENTRY32W entry; entry.dwSize =sizeof(decltype(entry));
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0); if(hSnapShot==INVALID_HANDLE_VALUE) return false;
	BOOL hRes=Module32FirstW( hSnapShot,&entry ); for(; hRes; hRes=Module32NextW(hSnapShot,&entry) ) if(file_path==path(entry.szExePath)) break;
	CloseHandle(hSnapShot);
	return hRes==TRUE;
#else
	auto v = std::move(find_process( file_path ));
	return !v.empty();
#endif
}

namespace console
{
inline const wchar_t* title(){ static wchar_t buff[MAX_PATH+1]; GetConsoleTitleW(buff,MAX_PATH); return buff; }
inline DWORD process(){ DWORD console; GetWindowThreadProcessId(GetConsoleWindow(),& console); return console; }
inline bool has_parent(){ return GetCurrentProcessId()!=process(); }
inline void setnobuf(){ setbuf(stdout,0);setbuf(stderr,0);setvbuf(stdout,nullptr,_IONBF,0); }
}

//*************************************
#endif // _PSAPI_H_
//*************************************

//*************************************
} // namespace os

//*************************************
#endif // __GX_OS_H__
