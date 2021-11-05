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

	virtual ~file_t(){ release(); }
	void release(){ if(!hfile) return; InternetCloseHandle(hfile); hfile=nullptr; }
	bool open( HINTERNET session, const wchar_t* url );
	bool get_file_size( HINTERNET session );
};

struct session_t
{
	HINTERNET handle = nullptr;

	virtual ~session_t(){ release(); }
	session_t(){ if(!is_online()) return; handle=InternetOpenW( path::module_path().name().c_str(),INTERNET_OPEN_TYPE_PRECONFIG,0,0,0); }
	void release(){ if(!handle) return; InternetCloseHandle(handle); handle=nullptr; }
	operator bool() const { return handle!=nullptr; }

	inline bool download( std::wstring url, path dst ){ return download(std::vector<std::wstring>{url},dst); }
	bool download( std::vector<std::wstring> urls, path dst );

protected:
	bool download_thread_func( std::vector<std::wstring> urls, path dst );
};

__noinline bool file_t::open( HINTERNET session, const wchar_t* url )
{
	if(!session){ fprintf( stdout, "%s(%s): session==nullptr", __func__, wtoa(url) ); return false; }

	static const DWORD flags = INTERNET_FLAG_TRANSFER_BINARY|INTERNET_FLAG_RELOAD|INTERNET_FLAG_NO_CACHE_WRITE|INTERNET_FLAG_EXISTING_CONNECT; // INTERNET_FLAG_PRAGMA_NOCACHE
	release(); hfile=InternetOpenUrlW( session, url, 0, 0, flags, 0); if(!hfile){ release(); return false; }
	//	DWORD status, dw_size=sizeof(status); if(!HttpQueryInfoW(hfile,HTTP_QUERY_STATUS_CODE|HTTP_QUERY_FLAG_NUMBER,&status,&dw_size,NULL)||status!=HTTP_STATUS_OK){ fprintf( stdout, "%s(%s): https status != HTTP_STATUS_OK\n", __func__, wtoa(url) ); release(); return false; }
	SYSTEMTIME msystemtime={}; DWORD dw_size=sizeof(msystemtime); if(!HttpQueryInfoW(hfile,HTTP_QUERY_LAST_MODIFIED|HTTP_QUERY_FLAG_SYSTEMTIME,&msystemtime,&dw_size,NULL)){ release(); return false; }

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

__noinline bool session_t::download_thread_func( std::vector<std::wstring> urls, path dst )
{
	if(!handle) return false;

	// fetch timestamp of the existing file
	bool b_dst_exists = dst.exists();
	FILETIME f0={}; if(b_dst_exists) f0=dst.mfiletime();
	
	for( auto url : urls )
	{
		file_t f; if(!f.open(handle,url.c_str())) continue; // url not exists
		if(b_dst_exists&&!FileTimeGreater(f.mfiletime,f0)) return false; // older url file exists
		if(!f.get_file_size(handle)){ fprintf(stdout,"error: unable to get file size %s\n", dst.name().wtoa() );return false;} // now try to get the file size
		std::vector<char> buffer(f.file_size);
		
		if(!dst.dir().exists()) dst.dir().mkdir();
		FILE* fp = _wfopen( dst.c_str(), L"wb" ); if(!fp){ fprintf(stdout,"error: unable to open %s\n", dst.name().wtoa());return false;}
		DWORD dw_size, dw_read; do
		{
			InternetQueryDataAvailable(f.hfile, &dw_size, 0, 0);
			if(dw_size>buffer.size()) dw_size = DWORD(buffer.size());
			InternetReadFile(f.hfile, buffer.data(), dw_size, &dw_read);
			if(dw_read) fwrite( buffer.data(), 1, dw_read, fp );
		} while(dw_read);
		fclose(fp);

		// modify the time stamp
		dst.set_filetime(now());
		dst.set_filetime(nullptr,nullptr,&f.mfiletime);
		fprintf( stdout, "%s\n", dst.name().wtoa() );
		return true;
	}

	return false;
}

__noinline bool session_t::download( std::vector<std::wstring> urls, path dst )
{
	if(is_offline()||!handle||urls.empty()||dst.empty()) return false;
	auto t=std::async(std::launch::async,&session_t::download_thread_func,this,urls,dst);
	while(std::future_status::ready!=t.wait_for(std::chrono::milliseconds(1)));
	return t.get();
}

//***********************************************
}} // namespace gx::inet
//***********************************************

#endif // __GX_INET_H__
