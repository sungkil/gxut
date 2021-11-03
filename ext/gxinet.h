#pragma once
#ifndef __GX_INET_H__
#define __GX_INET_H__

#ifdef __has_include
	#if __has_include("gxfilesystem.h")
		#include "gxfilesystem.h"
		#include "gxstring.h"
	#elif __has_include("../gxfilesystem.h")
		#include "../gxfilesystem.h"
		#include "../gxstring.h"
	#elif __has_include(<gxut/gxfilesystem.h>)
		#include <gxut/gxfilesystem.h>
		#include <gxut/gxstring.h>
	#endif
#endif

#include <wininet.h>
#include <future> // std::async
#pragma comment( lib, "wininet.lib" )

//***********************************************
namespace gx { namespace inet {
//***********************************************

inline bool is_online(){ DWORD s; return InternetGetConnectedState(&s,0)&&s!=INTERNET_CONNECTION_OFFLINE; }
inline bool is_offline(){ return !is_online(); }

struct file_t
{
	HINTERNET		hfile=nullptr;
	std::wstring	url;		// source url
	path			tmp, dst;	// temporary and final target path; sequetially download and copy
	size_t			file_size=0;
	FILETIME		mfiletime={};
	SYSTEMTIME		msystemtime={};
	struct { bool	downloaded=false; } b;

	virtual ~file_t(){ release(); }
	void release(){ if(!hfile) return; InternetCloseHandle(hfile); hfile=nullptr; }
	bool open( HINTERNET session, const wchar_t* url );
	bool get_file_size( HINTERNET session );
};

struct session_t
{
	std::vector<std::future<bool>> threads;
	HINTERNET handle = nullptr;

	virtual ~session_t(){ release(); }
	session_t(){ if(!is_online()) return; handle=InternetOpenW( path::module_path().name().c_str(),INTERNET_OPEN_TYPE_PRECONFIG,0,0,0); }
	void release(){ if(!handle) return; InternetCloseHandle(handle); handle=nullptr; }
	operator bool() const { return handle!=nullptr; }

	bool download( std::wstring url, path dst, bool b_wait=true ){ return download(std::vector<std::wstring>{url},dst,b_wait); }
	bool download( std::vector<std::wstring> urls, path dst, bool b_wait=true );
};

__noinline bool file_t::open( HINTERNET session, const wchar_t* url )
{
	if(!session){ fprintf( stdout, "%s(%s): session==nullptr", __func__, wtoa(url) ); return false; }

	static const DWORD flags = INTERNET_FLAG_TRANSFER_BINARY|INTERNET_FLAG_RELOAD|INTERNET_FLAG_NO_CACHE_WRITE|INTERNET_FLAG_EXISTING_CONNECT; // INTERNET_FLAG_PRAGMA_NOCACHE
	release(); hfile=InternetOpenUrlW( session, url, 0, 0, flags, 0); if(!hfile){ release(); return false; }
	//	DWORD status, dw_size=sizeof(status); if(!HttpQueryInfoW(hfile,HTTP_QUERY_STATUS_CODE|HTTP_QUERY_FLAG_NUMBER,&status,&dw_size,NULL)||status!=HTTP_STATUS_OK){ fprintf( stdout, "%s(%s): https status != HTTP_STATUS_OK\n", __func__, wtoa(url) ); release(); return false; }
	DWORD dw_size=sizeof(msystemtime); if(!HttpQueryInfoW(hfile,HTTP_QUERY_LAST_MODIFIED|HTTP_QUERY_FLAG_SYSTEMTIME,&msystemtime,&dw_size,NULL)){ release(); return false; }

	// preprocessing: invalidate second, because HTTP_QUERY_LAST_MODIFIED returns varying second
	msystemtime.wMilliseconds=0; msystemtime.wSecond=0; // discard seconds and ms
	SystemTimeToFileTime(&msystemtime,&mfiletime);
	this->url = url;

	return this->hfile!=nullptr;
}

__noinline bool file_t::get_file_size( HINTERNET session )
{
	if(!hfile) return false;
	DWORD content_length, dw_size=sizeof(content_length); if(!HttpQueryInfoW(hfile,HTTP_QUERY_CONTENT_LENGTH|HTTP_QUERY_FLAG_NUMBER,&content_length,&dw_size,NULL)){ release(); return false; }
	file_size = content_length;
	return true;
}

__noinline bool download_thread_func( session_t* session, std::vector<std::wstring> urls, path dst )
{
	if(!session||!*session) return false;

	// fetch timestamp of the existing file
	FILETIME f0={};
	bool dst_exists = dst.exists();
	if(dst_exists)
	{
		SYSTEMTIME s0=dst.msystemtime(); s0.wMilliseconds=0; s0.wSecond=5; // set small offset forward for local old file
		SystemTimeToFileTime(&s0,&f0);
		//fprintf( stdout, "old: %04d-%02d-%02d %02d:%02d:%02d %04d\n", s0.wYear, s0.wMonth, s0.wDay, s0.wHour, s0.wMinute, s0.wSecond, s0.wMilliseconds );
		//fprintf( stdout, "new: %04d-%02d-%02d %02d:%02d:%02d %04d\t", s1.wYear, s1.wMonth, s1.wDay, s1.wHour, s1.wMinute, s1.wSecond, s1.wMilliseconds );
	}
	
	for( auto url : urls )
	{
		file_t f; if(!f.open(session->handle,url.c_str())) continue; // url not exists
		if(dst_exists&&f.mfiletime<f0){ f.release(); break; } // urls exists, but older
		
		if(!f.get_file_size(session->handle)){ f.release(); break; } // now try to get the file size
		std::vector<char> buffer(f.file_size);
		
		dst.dir().mkdir();
		FILE* fp = _wfopen( dst.c_str(), L"wb" ); if(!fp){ fprintf(stdout,"%s(): unable to open %s\n",__func__,dst.wtoa()); return false; }
		DWORD dwSize, dwRead; do
		{
			InternetQueryDataAvailable(f.hfile, &dwSize, 0, 0);
			if(dwSize>buffer.size()) dwSize = DWORD(buffer.size());
			InternetReadFile(f.hfile, buffer.data(), dwSize, &dwRead);
			if(dwRead) fwrite( buffer.data(), 1, dwRead, fp );
		} while(dwRead);
		fclose(fp);

		// modify the time stamp
		dst.set_filetime(now());
		dst.set_filetime(nullptr,nullptr,&f.mfiletime);
		fprintf( stdout, "%s\n", dst.name().wtoa() );
		return true;
	}

	return false;
}

__noinline bool session_t::download( std::vector<std::wstring> urls, path dst, bool b_wait )
{
	if(is_offline()||!handle||urls.empty()||dst.empty()) return false;
	if(!b_wait){ if(threads.empty()) threads.reserve(4096); threads.emplace_back(std::move(std::async(download_thread_func,this,urls,dst))); return false; }
	auto t=std::async(download_thread_func,this,urls,dst);
	while(std::future_status::ready!=t.wait_for(std::chrono::milliseconds(1)));
	return t.get();
}

//***********************************************
}} // namespace gx::inet
//***********************************************

#endif // __GX_INET_H__
