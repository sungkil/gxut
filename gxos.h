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
#include <time.h>
#include <functional>

#if defined(_MSC_VER) && !defined(__clang__) // Visual Studio without clang
	#include <psapi.h>		// EnumProcesses
	#include <tlhelp32.h>	// process info helper
	#include <intrin.h>		// processor info
#elif defined(__GNUC__) // GCC
	#if __has_include(<unistd.h>)
		#include <unistd.h>
	#endif
	#include <cpuid.h>
#endif

#if __has_include("gxstring.h") && !defined(__GX_STRING_H__)
	#include "gxstring.h"
#endif

// compiler utility
namespace gx { namespace compiler
{
	inline int monthtoi( const char* month ){ if(!month||!month[0]||!month[1]||!month[2]) return 0; char a=tolower(month[0]), b=tolower(month[1]), c=tolower(month[2]); if(a=='j'){ if(b=='a') return 1; if(c=='n') return 6; return 7; } if(a=='f') return 2; if(a=='m'){ if(c=='r') return 3; return 5; } if(a=='a'){ if(b=='p') return 4; return 8; } if(a=='s') return 9; if(a=='o') return 10; if(a=='n') return 11; return 12; }
	inline int year(){ static int y=0; if(y) return y; char buff[64]={}; int r=sscanf(__DATE__,"%*s %*s %s", buff); return y=atoi(buff); }
	inline int month(){ static int m=0; if(m) return m; char buff[64]={}; int r=sscanf(__DATE__,"%s", buff); return m=monthtoi(buff); }
	inline int day(){ static int d=0; if(d) return d; char buff[64]={}; int r=sscanf(__DATE__,"%*s %s %*s", buff); return d=atoi(buff); }
}}

//***********************************************
namespace os { namespace cpu {
//***********************************************
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

//***********************************************
}} // namespace os::cpu
//***********************************************

// win32 messagebox wrapper utilities
#ifdef _MSC_VER
//*************************************
namespace os {
//*************************************
inline HWND& __message_box_owner_hwnd(){ static HWND h=nullptr; return h; }
inline void set_message_box_owner( HWND hwnd ){ __message_box_owner_hwnd()=hwnd; }
inline int message_box( const char* msg, const char* title, HWND hwnd=nullptr ){ return MessageBoxA(hwnd?hwnd:__message_box_owner_hwnd(),msg,title,MB_OKCANCEL|MB_ICONWARNING|MB_SYSTEMMODAL); }
//*************************************
} // end namespace os
//*************************************

#define __gxos_va__() va_list a;va_start(a,fmt);size_t l=size_t(vsnprintf(0,0,fmt,a));std::vector<char> v(l+1,0); char* b=v.data();vsnprintf(b,l+1,fmt,a);va_end(a)
__noinline bool confirm( HWND hwnd, __printf_format_string__ const char* fmt, ... ){ __gxos_va__(); return IDOK==os::message_box(b,"Warning",hwnd); }
__noinline bool confirm( __printf_format_string__ const char* fmt, ... ){ __gxos_va__(); return IDOK==os::message_box(b,"Warning"); }
__noinline bool mbox( __printf_format_string__ const char* fmt, ... ){ __gxos_va__(); return IDOK==os::message_box(b,"Message"); }
__noinline int ebox( __printf_format_string__ const char* fmt, ... ){ __gxos_va__(); os::message_box(b,"Error"); return -1; }
#endif

//*************************************
namespace os {
//*************************************
#ifdef _MSC_VER
// win32 utilities
__noinline const char* get_last_error(){ static char buff[4096]={};DWORD e=GetLastError();char *s=nullptr;FormatMessageA( FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_IGNORE_INSERTS,nullptr,e,MAKELANGID(LANG_ENGLISH,SUBLANG_DEFAULT),(LPSTR)&s,0,nullptr);snprintf(buff,4096,"%s (code=%x)",s,uint(e));LocalFree(s);return buff; }
__noinline void flush_message( int sleepTime=1 ){MSG m;for(int k=0;k<100&&PeekMessageA(&m,nullptr,0,0,PM_REMOVE);k++)SendMessageA(m.hwnd,m.message,m.wParam,m.lParam);if(sleepTime>=0) Sleep(sleepTime);}

inline const char* module_path( HMODULE h_module=nullptr ){ static char buff[_MAX_PATH]={}; wchar_t w[_MAX_PATH]; GetModuleFileNameW(h_module,w,_MAX_PATH); w[0]=::toupper(w[0]); return strcpy(buff,wtoa(w)); }
inline const char* module_name( HMODULE h_module=nullptr ){ const char* f=module_path(h_module); for( int kn=int(strlen(f)), k=kn-1; k>=0; k-- ) if(f[k]==L'\\') return f+k+1; return f; }
inline void exit( __printf_format_string__ const char* fmt, ... ){ va_list a; va_start(a,fmt); std::vector<char> buff(size_t(vsnprintf(0,0,fmt,a))+1); vsnprintf(&buff[0],buff.size(),fmt,a); va_end(a); fprintf( stdout, "[%s] %s", module_name(), &buff[0]); ::exit(EXIT_FAILURE); }
inline bool shutdown()
{
	HANDLE hToken; if(!OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,&hToken)) return false;
	TOKEN_PRIVILEGES tkp={}; LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
	tkp.PrivilegeCount=1; tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0); if(GetLastError()!=ERROR_SUCCESS) return false;
#pragma warning( disable: 28159 ) // Consider using 'a design alternative' instead of 'InitiateSystemShutdownExW'. Reason: Rearchitect to avoid Reboot
	return InitiateSystemShutdownExW(nullptr,nullptr,0,TRUE,FALSE,SHTDN_REASON_FLAG_PLANNED|SHTDN_REASON_MAJOR_APPLICATION|SHTDN_REASON_MINOR_OTHER);
#pragma warning( default: 28159 )
}

inline bool is_dos_cmd( const char* cmd )
{
	static std::set<std::string> dos_cmd_map = 
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
#endif

//***********************************************
namespace env {
//***********************************************
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

inline path where( const char* file_name )
{
	if(!file_name||!*file_name) return "";
	path f(file_name); if(f.is_absolute()&&f.exists()) return f;
	std::vector<path> v={f}; if(f.ext().empty()){for(auto e:{".com",".exe",".bat",".cmd"}) v.emplace_back(f+e);} // add the executable extensions
	for(auto& p:v) if(p.exists()) return p.absolute(path::cwd());
	for(const auto& e:paths() ){ for(auto& p:v) if((e+p).exists()) return (e+p).canonical(); }
	return path();
}

// particular variables
inline path windir(){ return path(os::env::var("WINDIR")).add_backslash(); }
#endif
//***********************************************
} // namespace env
//***********************************************

//***********************************************
// system variables/paths
inline const char* computer_name( bool b_lowercase=true )
{
	static char cname[1024]={};
	wchar_t w[1024]={0}; unsigned long l=sizeof(w)/sizeof(w[0]); GetComputerNameW(w,&l);
	if(b_lowercase) for(size_t k=0,kn=wcslen(w);k<kn;k++) w[k]=tolower(w[k]);
	return strcpy(cname,wtoa(w));
}

#ifdef __GX_FILESYSTEM_H__
inline path temp()
{
	static path t; if(!t.empty()) return t;
	wchar_t b[path::capacity]; GetTempPathW(path::capacity,b); t=wtoa(b); t=t.absolute().add_backslash();
	path tmp=t.drive()+"\\temp\\"; if(t==tmp.junction()) t=tmp; t[0]=::toupper(t[0]);
	if(t.volume().has_free_space()) return t;
	printf("temp(): not enough space in %s\n", t.c_str() );
	t=path::module_dir()+"temp\\"; if(!t.exists()) t.mkdir(); printf("temp(): %s is used instead", t.c_str() );
	if(!t.volume().has_free_space()) printf(", but still not enough space." );
	printf("\n");
	return t;
}
#endif

inline const char* userprofile()
{
	static std::string e; if(!e.empty()) return e.c_str();
	e=env::var("USERPROFILE");
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
	static std::string u=env::var("USERNAME");
	return u.c_str();
}

inline const char* local_appdata()
{
	static std::string e; if(!e.empty()) return e.c_str();
	e=env::var("LOCALAPPDATA");
#ifdef _SHLOBJ_H_
	wchar_t buff[_MAX_PATH]; if(e.empty()) SHGetFolderPathW(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, buff ); e=wtoa(buff);
#endif
	if(!e.empty()&&e.back()!='\\') e+='\\';
	return e.c_str();
}

inline const char* appdata()
{
	static std::string e; if(!e.empty()) return e.c_str();
	e=env::var("APPDATA");
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

inline std::vector<HWND> enum_windows( const char* filter=nullptr )
{
	struct params { std::vector<HWND> v; const char* filter=nullptr; };
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
#ifdef __GX_FILESYSTEM_H__
__noinline std::string __build_process_cmd( const char* const app, const char* args )
{
	// buffers
	std::vector<char> vcmd(4096,0);	auto* cmd=vcmd.data();
	std::vector<char> vbuf(4096,0);	auto* buf=vbuf.data();
	
	// prioritize com against exe for no-extension apps
	if(!app&&args&&*args!='\"')
	{
		strcpy(buf,args); char *ctx=nullptr, *token=strtok_s(buf," \t\n",&ctx);
		path t=path(token).to_backslash(), e=env::where(t.c_str());
		if(!t.exists()&&t.ext().empty()&&!e.empty()&&e.ext()=="com") args=strcpy(buf,strcat(strcat(strcpy(cmd,token),".com "),buf+strlen(token)+1)); // use cmd as temp
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
	if(!CreateProcessW( app?atow(app):nullptr, (LPWSTR)atow(__build_process_cmd(app,args).c_str()), nullptr, nullptr, FALSE, priority, nullptr, nullptr, &si, &pi)||!pi.hProcess){ ebox(get_last_error()); return false; }
	if(wait){ WaitForSingleObject(pi.hProcess,INFINITE); safe_close_handle( pi.hThread ); safe_close_handle( pi.hProcess ); }
	return true;
}

__noinline bool redirect_process( const char* app, const char* args=nullptr,
	uint* count=nullptr, DWORD priority=NORMAL_PRIORITY_CLASS )
{
	// startup attributes
	STARTUPINFOW si={sizeof(si)};
	si.dwFlags=STARTF_USESHOWWINDOW;
	si.wShowWindow=SW_HIDE;

	// prepare redirection
	HANDLE stdout_read=INVALID_HANDLE_VALUE;	// parent process's stdout read handle
	HANDLE stdout_write=INVALID_HANDLE_VALUE;	// child process' stdout write handle
	static const int REDIR_BUFFER_SIZE = 1<<24;	// 16 MB should be enough for buffering; otherwise, stdout will not be written no more, which printf hangs

	SECURITY_DESCRIPTOR sd; InitializeSecurityDescriptor(&sd,SECURITY_DESCRIPTOR_REVISION);
	//@@SetSecurityDescriptorDacl(&sd,true,NULL,false);
	SECURITY_ATTRIBUTES sa={sizeof(SECURITY_ATTRIBUTES),&sd,TRUE/*inheritance*/};
	if(!CreatePipe( &stdout_read, &stdout_write, &sa, REDIR_BUFFER_SIZE )){ printf( "[redir] CreatePipe(stdout) failed: %s\n", os::get_last_error() ); return false; }
	if(stdout_read==INVALID_HANDLE_VALUE||stdout_write==INVALID_HANDLE_VALUE){ printf( "[redir] CreatePipe(stdout) failed: %s\n", os::get_last_error() ); return false; }
	if(!SetHandleInformation(stdout_read,HANDLE_FLAG_INHERIT,0)){ printf( "[redir] SetHandleInformation() failed: %s\n", os::get_last_error() ); return false; }

	// additional configuration for non-buffered IO
	setvbuf( stdout, nullptr, _IONBF, 0 );	// absolutely needed
	setvbuf( stderr, nullptr, _IONBF, 0 );	// absolutely needed
	std::ios::sync_with_stdio();

	// override startup attributes
	si.hStdError	= stdout_write;
	si.hStdOutput	= stdout_write;
	si.hStdInput	= INVALID_HANDLE_VALUE;
	si.dwFlags		= si.dwFlags|STARTF_USESTDHANDLES;

	// create process and wait if necessary
	PROCESS_INFORMATION pi={};
	if(!CreateProcessW( app?atow(app):nullptr, (LPWSTR)atow(__build_process_cmd(app,args).c_str()), nullptr, nullptr, TRUE, priority, nullptr, nullptr, &si, &pi )||!pi.hProcess){ ebox(get_last_error()); return false; }

	static char* buff=nullptr; static size_t blen=0;
	DWORD n_avail=0, n_read=0, read_count=0;
	while( PeekNamedPipe( stdout_read, nullptr, 0, nullptr, &n_avail, nullptr ))
	{
		DWORD exit; GetExitCodeProcess(pi.hProcess,&exit); if(exit!=STILL_ACTIVE) break;
		if(n_avail==0){ Sleep(1); continue; }
		if(blen<n_avail){ char* buff1=(char*)realloc(buff,((blen=n_avail)+1)*sizeof(char)); if(buff1) buff=buff1; }
		if(buff&&!ReadFile(stdout_read, buff, n_avail, &n_read, nullptr)) return false; if(n_read==0) continue;
		read_count += n_read;
		if(buff){ buff[n_read]=0; printf(buff); }
	}
	if(count) *count = read_count;

	safe_close_handle( pi.hThread );
	safe_close_handle( pi.hProcess );
	safe_close_handle( stdout_read );
	safe_close_handle( stdout_write );

	return true;
}
#endif

__noinline std::string read_process( std::string cmd, uint cp=0, const char* trims=" \t\r\n") // provide CP_UTF8 for some apps
{
	FILE* pp=_popen(cmd.c_str(),"r"); if(!pp) return "";
	std::string b; char buff[256]={}; while( fgets(buff,sizeof(buff)/sizeof(buff[0]),pp) ) b+= buff;
	bool b_eof= feof(pp); if(!b_eof) printf("%s(%s): broken pipe\n", __func__, cmd.c_str() );
	_pclose(pp);
	if(cp) b=atoa(b.c_str(),cp);
	return trim(b.c_str(),trims);
}

#ifdef _INC_SHELLAPI
__noinline bool runas( const char* app, const char* args=nullptr, bool wait=true, bool windowed=false )
{
	if(!app){ printf("%s(): app must be provided\n",__func__); return false; }
	path app_path = path(app); if(app_path.ext()!="exe"){ printf("%s(): *.exe is only supported\n",__func__); return false; }
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

inline std::vector<DWORD> find_process( const char* name_or_path )
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
#endif // _PSAPI_H_
//*************************************

#ifdef _WINREG_
//*************************************
namespace reg { // read-only registry reader
//*************************************

template <HKEY root=HKEY_CLASSES_ROOT> // HKEY_CLASSES_ROOT, HKEY_LOCAL_MACHINE, ...
struct key_t
{
	key_t( __printf_format_string__ const char* fmt, ... ){ if(!fmt) return; va_list a; va_start(a,fmt); size_t len=size_t(vsnprintf(0,0,fmt,a)); std::vector<char> b; b.resize(len+1,0); vsnprintf(b.data(),len+1,fmt,a); va_end(a); if(b.back()==L'\\'||b.back()==L'/') b.back()=0; this->key=atow(b.data()); }
	~key_t(){ if(hkey) RegCloseKey(hkey); }

	operator bool() const { return hkey!=nullptr; }
	bool open(){ if(hkey) return true; if(key.empty()) return false; LONG r=RegOpenKeyExW(root, key.c_str(),0,KEY_READ,&hkey); if(r!=ERROR_SUCCESS) hkey=nullptr; return hkey!=nullptr; }
	const wchar_t* c_str() const { return format( L"[%s\\%s]", _root_str(), key.c_str() ); }
	template <class T=std::string> T get( const char* name=nullptr );

	// template specialization
	template <> inline std::string get<std::string>( const char* name )
	{
		std::vector<BYTE> v; DWORD t; if(!_query( name, &v, &t )) return "";
		if(t==REG_SZ)		return std::string((char*)v.data());
		if(t==REG_DWORD){ char b[256]; snprintf(b,256,"%u",*((DWORD*)v.data())); return b; }
		return "";
	}
#ifdef __GX_FILESYSTEM_H__
	template <> inline path get<path>( const char* name ){ return path(get<std::string>(name).c_str()); }
#endif
	template <> inline DWORD get<DWORD>( const char* name )
	{
		std::vector<BYTE> v; DWORD t; if(!_query( name, &v, &t )) return 0;
		if(t==REG_DWORD)	return *((DWORD*)v.data());
		if(t==REG_SZ)		return DWORD(atoi((char*)v.data()));
		return 0;
	}

protected:
	bool _query( const char* name, std::vector<BYTE>* v, DWORD* t )
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
} // namespace reg
#endif // _WINREG_
//*************************************

//*************************************
} // namespace os
//*************************************

#endif // __GX_OS_H__
