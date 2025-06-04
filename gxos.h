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

// include gxut.h without other headers
#ifndef __GXUT_H__
#pragma push_macro("__GXUT_EXCLUDE_HEADERS__")
#define __GXUT_EXCLUDE_HEADERS__
#include "gxut.h"
#pragma pop_macro("__GXUT_EXCLUDE_HEADERS__")
#endif

#if defined __gcc__ || !defined __msvc__
	#error <gxut/gxos.h> runs only in Windows without support for linux/gcc
#endif

#if !defined(__GX_STRING_H__) && __has_include("gxstring.h")
	#include "gxstring.h"
#endif

#if __has_include("gxfilesystem.h")
	#include "gxfilesystem.h"
#endif

#include <psapi.h>		// EnumProcesses
#include <tlhelp32.h>	// process info helper

//*************************************
namespace os {
//*************************************

inline DWORD current_process()
{
	static DWORD curr_pid = GetCurrentProcessId();
	return curr_pid;
}

inline const vector<DWORD>& enum_process_indices()
{
	static vector<DWORD> pids(4096);
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

__noinline vector<DWORD> find_process( const char* name_or_path )
{
	vector<DWORD> v;
	static wchar_t buff[4096];
	static DWORD curr_pid = GetCurrentProcessId();
	for( auto pid : enum_process_indices() )
	{
		HMODULE hMod; DWORD cbNeeded;
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,pid); if(!hProcess) continue;
		if(!EnumProcessModules(hProcess,&hMod,sizeof(hMod),&cbNeeded)) continue;
		GetModuleBaseNameW(hProcess,hMod,buff,sizeof(buff)/sizeof(buff[0]) );
		if(wcsicmp(buff,atow(name_or_path))==0&&pid!=curr_pid) v.push_back(pid);
		GetModuleFileNameExW(hProcess,hMod,buff,sizeof(buff)/sizeof(buff[0]) );
		if(wcsicmp(buff,atow(name_or_path))==0&&pid!=curr_pid) v.push_back(pid);
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
	BOOL hRes=Module32FirstW( hSnapShot,&entry ); for(; hRes; hRes=Module32NextW(hSnapShot,&entry) ) if(stricmp(file_path,wtoa(entry.szExePath))==0) break;
	CloseHandle(hSnapShot);
	return hRes==TRUE;
#else
	auto v = std::move(find_process(file_path));
	return !v.empty();
#endif
}

__noinline vector<std::pair<uint,string>> list_process( const char* app_name=nullptr )
{
	string cmd= "tasklist /FO:csv";
	if(app_name&&*app_name) cmd += format(" /FI \"ImageName eq %s\"",app_name );
	string r = read_process(cmd.c_str());
	vector<std::pair<uint,string>> v; v.reserve(32);
	char *ctx=0,*token=strtok_s(__strdup(r.c_str()),"\r\n",&ctx); // skip the first line
	for(token=strtok_s(0,"\r\n",&ctx);token;token=strtok_s(0,"\r\n",&ctx))
	{
		char *c=0,*t=strtok_s(__strdup(token),",",&c);
		string name = t?string(t).substr(1,int(strlen(t))-2):""; if(name.empty()) continue;
		t=strtok_s(0,",",&c); uint pid = t?uint(atoi(string(t).substr(1,strlen(t)-2).c_str())):-1; if(pid==0||pid==-1) continue;
		v.emplace_back( pid, name );
	}
	return v;
}

//*************************************
} // end namespace os
//*************************************

//*************************************
namespace env {
//*************************************
inline bool set_dll_directory( const char* dir ){ return SetDllDirectoryW(atow(dir))?true:false; }
inline const char* windir(){ return append_slash(env::get("WINDIR")); }
//*************************************
} // end namespace env
//*************************************

// forward decl.
namespace os { int message_box( const char* msg, const char* title, HWND hwnd ); }
#define __gxos_va__() va_list a;va_start(a,fmt);size_t l=size_t(vsnprintf(0,0,fmt,a));vector<char> v(l+1,0); char* b=v.data();vsnprintf(b,l+1,fmt,a);va_end(a)
__noinline bool confirm( HWND hwnd, __printf_format_string__ const char* fmt, ... ){ __gxos_va__(); return IDOK==os::message_box(b,"Warning",hwnd); }
__noinline bool confirm( __printf_format_string__ const char* fmt, ... ){ __gxos_va__(); return IDOK==os::message_box(b,"Warning",nullptr); }
__noinline bool mbox( __printf_format_string__ const char* fmt, ... ){ __gxos_va__(); return IDOK==os::message_box(b,"Message",nullptr); }

//*************************************
namespace os { 
//*************************************

inline HWND& __message_box_owner_hwnd(){ static HWND h=nullptr; return h; }
inline void set_message_box_owner( HWND hwnd ){ __message_box_owner_hwnd()=hwnd; }
inline int message_box( const char* msg, const char* title, HWND hwnd=nullptr ){ return MessageBoxA(hwnd?hwnd:__message_box_owner_hwnd(),msg,title,MB_OKCANCEL|MB_ICONWARNING|MB_SYSTEMMODAL); }

// win32 utilities
__noinline const char* get_last_error( DWORD error=0 ){ static wchar_t buff[4096]={};if(!error)error=GetLastError();wchar_t *s=nullptr;FormatMessageW( FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_IGNORE_INSERTS,nullptr,error,MAKELANGID(LANG_ENGLISH,SUBLANG_DEFAULT),(LPWSTR)&s,0,nullptr);swprintf(buff,4096,L"%s (code=%x)",s,uint(error));LocalFree(s);return wtoa(buff); }
__noinline void flush_message( int sleepTime=1 ){MSG m;for(int k=0;k<100&&PeekMessageW(&m,nullptr,0,0,PM_REMOVE);k++)SendMessageW(m.hwnd,m.message,m.wParam,m.lParam);if(sleepTime>=0) Sleep(sleepTime);}

inline const char* module_path( HMODULE h_module ){ static char buff[_MAX_PATH]={}; wchar_t w[_MAX_PATH]; GetModuleFileNameW(h_module,w,_MAX_PATH); w[0]=::toupper(w[0]); return strcpy(buff,wtoa(w)); }
inline const char* module_name( HMODULE h_module ){ const char* f=module_path(h_module); for( int kn=int(strlen(f)), k=kn-1; k>=0; k-- ) if(f[k]==L'\\') return f+k+1; return f; }
inline void exit( __printf_format_string__ const char* fmt, ... ){ va_list a; va_start(a,fmt); vector<char> buff(size_t(vsnprintf(0,0,fmt,a))+1); vsnprintf(&buff[0],buff.size(),fmt,a); va_end(a); fprintf( stdout, "[%s] %s", exe::name(), &buff[0]); ::exit(EXIT_FAILURE); }
inline bool shutdown()
{
	HANDLE hToken; if(!OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,&hToken)) return false;
	TOKEN_PRIVILEGES tkp={}; LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
	tkp.PrivilegeCount=1; tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0); if(GetLastError()!=ERROR_SUCCESS) return false;
	#pragma warning( suppress: 28159 ) // Consider using 'a design alternative' instead of 'InitiateSystemShutdownExW'. Reason: Rearchitect to avoid Reboot
	return InitiateSystemShutdownExW(nullptr,nullptr,0,TRUE,FALSE,SHTDN_REASON_FLAG_PLANNED|SHTDN_REASON_MAJOR_APPLICATION|SHTDN_REASON_MINOR_OTHER);
}

inline bool is_dos_cmd( const char* cmd )
{
	static std::set<string> dos_cmd_map = 
	{
		"append", "attrib",
		"batch",
		"call", "cd", "chdir", "cls", "cmd", "comp", "copy",
		"date", "de", "delete", "deltree", "dir",
		"echo", "edit", "erase", "exit", "expand",
		"fc", "find", "findstr", "for", "ftp", "ftype",
		"getmac",
		"hostname",
		"if", "ipconfig",
		"md", "mkdir", "mklink", "move",
		"net",
		"pause", "popd", "pushd",
		"rd", "reg", "rename", "replace", "rmdir", "robocopy", "runas",
		"set", "setloca", "setver", "shutdown", "sort", "start", "subst",
		"taskkil", "tasklist", "time", "timeout", "title", "tree", "type",
		"undelete",
		"ver",
		"xcopy",
		"w32tm",
	};
	return cmd&&*cmd&&dos_cmd_map.find(cmd)!=dos_cmd_map.end();
}

//*************************************
// system variables/paths
inline const char* computer_name( bool b_lowercase=true )
{
	static char cname[1024]={};
	wchar_t w[1024]={0}; unsigned long l=sizeof(w)/sizeof(w[0]); GetComputerNameW(w,&l);
	if(b_lowercase) for(size_t k=0,kn=wcslen(w);k<kn;k++) w[k]=tolower(w[k]);
	return strcpy(cname,wtoa(w));
}

// windows user/system temp directory
inline path usertemp()
{
	static path t; if(!t.empty()) return t;
	wchar_t b[path::capacity]; GetTempPathW(path::capacity,b); t=wtoa(b); t=t.absolute().append_slash();
	path tmp=t.drive().append_slash()+"temp\\"; if(t==tmp.junction()) t=tmp; t[0]=::toupper(t[0]);
	if(volume_t(t).has_free_space()) return t;
	printf("temp(): not enough space in %s\n", t.c_str() );
	t=path(exe::dir())+"temp\\"; if(!t.exists()) t.mkdir(); printf("temp(): %s is used instead", t.c_str() );
	if(!volume_t(t).has_free_space()) printf(", but still not enough space." );
	printf("\n");
	return t;
}

inline const char* userprofile()
{
	static string e; if(!e.empty()) return e.c_str();
	e=env::get("USERPROFILE");
#ifdef _SHLOBJ_H_
	wchar_t buff[_MAX_PATH]; if(e.empty()) SHGetFolderPathW(NULL, CSIDL_PROFILE, NULL, 0, buff ); e=wtoa(buff);
#endif
	if(!e.empty()&&e.back()!='\\') e+='\\';
	return e.c_str();
}

inline const char* home()
{
	return userprofile();
}

inline const char* user()
{
	static string u=env::get("USERNAME");
	return u.c_str();
}

inline const char* local_appdata()
{
	static string e; if(!e.empty()) return e.c_str();
	e=env::get("LOCALAPPDATA");
#ifdef _SHLOBJ_H_
	wchar_t buff[_MAX_PATH]; if(e.empty()) SHGetFolderPathW(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, buff ); e=wtoa(buff);
#endif
	if(!e.empty()&&e.back()!='\\') e+='\\';
	return e.c_str();
}

inline const char* appdata()
{
	static string e; if(!e.empty()) return e.c_str();
	e=env::get("APPDATA");
#ifdef _SHLOBJ_H_
	wchar_t buff[_MAX_PATH]; if(e.empty()) SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, buff ); e=wtoa(buff);
#endif
	if(!e.empty()&&e.back()!='\\') e+='\\';
	return e.c_str();
}

inline const char* system_dir()
{
	static char e[_MAX_PATH]={}; if(*e) return e;
	wchar_t w[_MAX_PATH]; GetSystemDirectoryW(w,_MAX_PATH);
	size_t l=wcslen(w); if(l&&w[l-1]!='\\'){ w[l-1]='\\'; w[l]=0; }
	return strcpy(e,wtoa(w));
}

class mutex_t
{
	int		wait_counter = 256;
	HANDLE	h_mutex=nullptr;
public:
	mutex_t( const char* name, int wait_time=2560 );
	~mutex_t(){ close(); }
	operator bool() const { return h_mutex!=nullptr; }
	HANDLE& close(){ if(h_mutex) CloseHandle(h_mutex); return h_mutex=0; }
};

inline mutex_t::mutex_t( const char* name, int wait_time ):wait_counter(wait_time/10)
{
	close()=CreateMutexW(0,FALSE,atow(name)); DWORD e=GetLastError();
	for(uint k=0,kn=wait_counter;k<kn&&e==ERROR_ALREADY_EXISTS;k++){ close()=CreateMutexW(0,FALSE,atow(name));e=GetLastError(); Sleep(10); }
	if(e==ERROR_ALREADY_EXISTS) close();
}

inline vector<HWND> enum_windows( const char* filter=nullptr )
{
	struct params { vector<HWND> v; const char* filter=nullptr; };
	auto __enum_windows_proc = []( HWND hwnd , LPARAM pParams )->BOOL
	{
		params* p = ((params*)pParams);
		if(p->filter){ wchar_t buff[4096]; GetWindowTextW(hwnd,buff,4096); if(!strstr(wtoa(buff),p->filter)) return TRUE; }
		p->v.emplace_back(hwnd); return TRUE;
	};
	params p; p.filter=filter;
	EnumWindows(__enum_windows_proc,(LPARAM)(&p));
	return p.v;
}

inline HWND find_window( const char* filter )
{
	HWND h = GetTopWindow(GetDesktopWindow());
	for(; h!=nullptr; h=GetWindow(h, GW_HWNDNEXT))
	{
		wchar_t buff[4096]; GetWindowTextW(h,buff,4096);
		if(strstr(wtoa(buff),filter)) return h;
	}
	return nullptr;
}

//*************************************
// create process
__noinline bool redirect_process( const char* app, const char* args=nullptr,
	uint* count=nullptr, DWORD priority=NORMAL_PRIORITY_CLASS )
{
	#pragma warning( disable: 6248 ) // Setting a SECURITY_DESCRIPTOR's DACL to NULL will result in an unprotected object.
	SECURITY_DESCRIPTOR sd; InitializeSecurityDescriptor(&sd,SECURITY_DESCRIPTOR_REVISION); SetSecurityDescriptorDacl(&sd,true,NULL,false);
	SECURITY_ATTRIBUTES sa={sizeof(SECURITY_ATTRIBUTES),&sd,TRUE/*inheritance*/};
	#pragma warning( default: 6248 )

	constexpr int REDIR_BUFFER_SIZE = 1<<24; // 16 MB should be enough for buffering; otherwise, stdout will not be written no more, which printf hangs
	HANDLE stdout_read=INVALID_HANDLE_VALUE, stdout_write=INVALID_HANDLE_VALUE;	// parent process's stdout read handle, child process' stdout write handle
	if(!CreatePipe( &stdout_read, &stdout_write, &sa, REDIR_BUFFER_SIZE )){ printf( "[redir] CreatePipe(stdout) failed: %s\n", os::get_last_error() ); return false; }
	if(stdout_read==INVALID_HANDLE_VALUE||stdout_write==INVALID_HANDLE_VALUE){ printf( "[redir] CreatePipe(stdout) failed: %s\n", os::get_last_error() ); return false; }
	if(!SetHandleInformation(stdout_read,HANDLE_FLAG_INHERIT,0)){ printf( "[redir] SetHandleInformation() failed: %s\n", os::get_last_error() ); return false; }

	// additional configuration for non-buffered IO
	setvbuf( stdout, nullptr, _IONBF, 0 );	// absolutely needed
	setvbuf( stderr, nullptr, _IONBF, 0 );	// absolutely needed
	std::ios::sync_with_stdio();

	// override startup attributes
	STARTUPINFOW si={sizeof(si)};
	si.dwFlags		= STARTF_USESHOWWINDOW;
	si.wShowWindow	= SW_HIDE;
	si.hStdError	= stdout_write;
	si.hStdOutput	= stdout_write;
	si.hStdInput	= INVALID_HANDLE_VALUE;
	si.dwFlags		= si.dwFlags|STARTF_USESTDHANDLES;

	// create process and wait if necessary
	PROCESS_INFORMATION pi={}; if(!CreateProcessW(0,build_cmdline(app,args),0,0,TRUE,priority,0,0,&si,&pi )||!pi.hProcess||!pi.hThread){ printf("%s\n",get_last_error()); return false; }

	static char* buff=nullptr; static size_t blen=0;
	DWORD n_avail=0, n_read=0, read_count=0;
	while( PeekNamedPipe( stdout_read, nullptr, 0, nullptr, &n_avail, nullptr ))
	{
		DWORD exit; GetExitCodeProcess(pi.hProcess,&exit); if(exit!=STILL_ACTIVE) break;
		if(n_avail==0){ Sleep(1); continue; }
		if(blen<n_avail){ char* buff1=(char*)realloc(buff,((blen=n_avail)+1)*sizeof(char)); if(buff1) buff=buff1; }
		if(!buff){ printf("%s(): null buff\n",__func__); break; }
		if(!ReadFile(stdout_read, buff, n_avail, &n_read, nullptr)) return false; if(n_read==0) continue;
		buff[n_read]=0; printf(read_count==0?ltrim(buff):buff);
		read_count += n_read;
	}
	
	safe_close_handle( pi.hThread );
	safe_close_handle( pi.hProcess );
	safe_close_handle( stdout_read );
	safe_close_handle( stdout_write );

	if(count) *count = read_count;
	return true;
}

#ifdef _INC_SHELLAPI
__noinline bool runas( const char* app, const char* args=nullptr, bool wait=true, bool windowed=false )
{
	if(!app){ printf("%s(): app must be provided\n",__func__); return false; }
	path_t app_path = path_t(app); if(app_path.extension()!="exe"){ printf("%s(): *.exe is only supported\n",__func__); return false; }
	if(app_path.is_relative()) app_path = app_path.absolute();

	// build execute info
	SHELLEXECUTEINFOW info = {0};
	info.cbSize = sizeof(info);
	info.fMask = SEE_MASK_NOCLOSEPROCESS;
	info.hwnd = 0;
	info.lpVerb = L"runas";
	info.lpFile = atow(app_path.c_str());
	info.lpParameters = atow(args);
	info.lpDirectory = 0;
	info.nShow=windowed?SW_SHOW:SW_HIDE;
	info.hInstApp = 0;

	if(!ShellExecuteExW(&info)) return false;
	if(wait)
	{
		if(info.hProcess) WaitForSingleObject(info.hProcess,INFINITE);
		safe_close_handle(info.hProcess);
	}
	return true;
}
#endif

//*************************************
namespace console {
//*************************************
inline const char* title(){ static wchar_t buff[MAX_PATH+1]; GetConsoleTitleW(buff,MAX_PATH); return wtoa(buff); }
inline DWORD get_console_process(){ DWORD console; GetWindowThreadProcessId(GetConsoleWindow(),&console); return console; }
inline bool has_parent(){ return GetCurrentProcessId()!=get_console_process(); }
inline void setnobuf(){ setbuf(stdout,0);setbuf(stderr,0);setvbuf(stdout,nullptr,_IONBF,0); }
static auto get_default_color(){ static WORD c=0; if(!c){ HANDLE h=GetStdHandle(STD_OUTPUT_HANDLE); CONSOLE_SCREEN_BUFFER_INFO csbi;GetConsoleScreenBufferInfo(h,&csbi);c=csbi.wAttributes; } return c; } // usually 7
inline void set_color( int color=0 ){ HANDLE h=GetStdHandle(STD_OUTPUT_HANDLE); if(!h) return; auto d=get_default_color(); SetConsoleTextAttribute(h,color?color:d); } // console color: 10: lightgreen, 11: sky, 12: red, 14: lightyellow
inline void clear_color(){ HANDLE h=GetStdHandle(STD_OUTPUT_HANDLE); if(!h) return; auto d=get_default_color(); SetConsoleTextAttribute(h,d); }
//*************************************
} // namespace console
//*************************************

//*************************************
namespace reg { // read-only registry reader
//*************************************

template <HKEY root=HKEY_CLASSES_ROOT> // HKEY_CLASSES_ROOT, HKEY_LOCAL_MACHINE, ...
struct key_t
{
	key_t( __printf_format_string__ const char* fmt, ... ){ if(!fmt) return; va_list a; va_start(a,fmt); size_t len=size_t(vsnprintf(0,0,fmt,a)); vector<char> b; b.resize(len+1,0); vsnprintf(b.data(),len+1,fmt,a); va_end(a); if(b.back()=='/'||b.back()=='\\') b.back()=0; this->key=atow(b.data()); }
	~key_t(){ if(hkey) RegCloseKey(hkey); }

	operator bool() const { return hkey!=nullptr; }
	bool open(){ if(hkey) return true; if(key.empty()) return false; LONG r=RegOpenKeyExW(root, key.c_str(),0,KEY_READ,&hkey); if(r!=ERROR_SUCCESS) hkey=nullptr; return hkey!=nullptr; }
	const wchar_t* c_str() const { return format( L"[%s\\%s]", _root_str(), key.c_str() ); }
	template <class T=string> T get( const char* name=nullptr );

	// template specialization
	template <> inline string get<string>( const char* name )
	{
		vector<BYTE> v; DWORD t; if(!_query( name, &v, &t )) return "";
		if(t==REG_SZ)		return string((char*)v.data());
		if(t==REG_DWORD){ char b[256]; snprintf(b,256,"%u",*((DWORD*)v.data())); return b; }
		return "";
	}
#ifdef __GX_FILESYSTEM_H__
	template <> inline path get<path>( const char* name ){ return path(get<string>(name)); }
#endif
	template <> inline DWORD get<DWORD>( const char* name )
	{
		vector<BYTE> v; DWORD t; if(!_query( name, &v, &t )) return 0;
		if(t==REG_DWORD)	return *((DWORD*)v.data());
		if(t==REG_SZ)		return DWORD(atoi((char*)v.data()));
		return 0;
	}

protected:
	bool _query( const char* name, vector<BYTE>* v, DWORD* t )
	{
		if(!hkey&&!open()) return false;
		DWORD n=4096; LONG r=RegQueryValueExW(hkey,atow(name),0,t,nullptr,&n); if(r!=ERROR_SUCCESS) return false;
		v->resize(n+2,0); r=RegQueryValueExW(hkey,atow(name),0,t,v->data(),&n); if(r!=ERROR_SUCCESS) return false;
		return true;
	}

	static const wchar_t* _root_str()
	{
		if(root==HKEY_CLASSES_ROOT) return L"HKEY_CLASSES_ROOT";
		if(root==HKEY_CURRENT_USER) return L"HKEY_CURRENT_USER";
		if(root==HKEY_LOCAL_MACHINE) return L"HKEY_LOCAL_MACHINE";
		if(root==HKEY_USERS) return L"HKEY_USERS";
		if(root==HKEY_PERFORMANCE_DATA) return L"HKEY_PERFORMANCE_DATA";
		if(root==HKEY_PERFORMANCE_TEXT) return L"HKEY_PERFORMANCE_TEXT";
		if(root==HKEY_PERFORMANCE_NLSTEXT) return L"HKEY_PERFORMANCE_NLSTEXT";
#if(WINVER >= 0x0400)
		if(root==HKEY_CURRENT_CONFIG) return L"HKEY_CURRENT_CONFIG";
		if(root==HKEY_DYN_DATA) return L"HKEY_DYN_DATA";
		if(root==HKEY_CURRENT_USER_LOCAL_SETTINGS) return L"HKEY_CURRENT_USER_LOCAL_SETTINGS";
#endif
		return L"";
	}

	HKEY hkey=nullptr;
	std::wstring key;
};

//*************************************
} // end namespace reg
//*************************************

//*************************************
} // namespace os
//*************************************

#endif // __GX_OS_H__
