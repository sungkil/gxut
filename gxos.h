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

#if !defined(__GX_STRING_H__) && __has_include("gxstring.h")
	#include "gxstring.h"
#endif

#include <time.h>
#include <functional>

#ifdef __msvc__
	#include <psapi.h>		// EnumProcesses
	#include <tlhelp32.h>	// process info helper
#endif

//*************************************
namespace os {
//*************************************

// process-related
__noinline string __build_process_cmd( const char* const app, const char* args )
{
	// buffers
	vector<char> vcmd(4096,0);	auto* cmd=vcmd.data();
	vector<char> vbuf(4096,0);	auto* buf=vbuf.data();
	
	// prioritize com against exe for no-extension apps
	if(!app&&args&&*args!='\"')
	{
		strcpy(buf,args); char *ctx=nullptr, *token=strtok_s(buf," \t\n",&ctx);
		const char* t=to_backslash(token);
		string e=env::where(t); const char* x=extension(t);
		if(!file_exists(t)&&(!x||!*x)&&!e.empty()&&strcmp(x,"com")==0) args=strcpy(buf,strcat(strcat(strcpy(cmd,token),".com "),buf+strlen(token)+1)); // use cmd as temp
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

__noinline string read_process( string cmd, const char* trims=" \t\r\n" )
{
	FILE* pp = _popen(cmd.c_str(),"rb"); if(!pp) return "";
	vector<char> v; v.reserve(1024); char buff[64]={}; size_t n=0; while( n=fread(buff,1,sizeof(buff),pp) ) v.insert(v.end(),buff,buff+n); v.emplace_back(0);
	bool b_eof= feof(pp); _pclose(pp); if(!b_eof) printf("%s(%s): broken pipe\n", __func__, cmd.c_str() );
	char* s=v.data(); return trim(is_utf8(s)?atoa(s,CP_UTF8,0):s, trims);  // auto convert CP_UTF8 to current code page
}

//*************************************
} // end namespace os
//*************************************

#if defined(__msvc__) && __has_include("gxwin32.h")
	#include "gxwin32.h"
#endif

#endif // __GX_OS_H__
