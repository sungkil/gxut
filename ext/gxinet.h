#pragma once
#ifndef __GX_INET_H__
#define __GX_INET_H__

#if __has_include("gxut.h")
	#include "gxut.h"
#elif __has_include("../gxut.h")
	#include "../gxut.h"
#elif __has_include(<gxut/gxut.h>)
	#include <gxut/gxut.h>
#endif

#include <winsock2.h>
#include <windns.h>
#include <ws2tcpip.h>
#include <wininet.h>
#include <future> // std::async
#pragma comment( lib, "ws2_32" )
#pragma comment( lib, "wininet" )
#pragma comment( lib, "Dnsapi" )

//***********************************************
namespace gx { namespace inet {
//***********************************************

inline bool is_online(){ static int status=-1; if(status<0){ DWORD s; status=InternetGetConnectedState(&s,0)&&s!=INTERNET_CONNECTION_OFFLINE?1:0; } return status>0; }
inline bool is_offline(){ return !is_online(); }

struct file_t
{
	HINTERNET	hfile=nullptr;
	string		url;		// source url
	path_t		tmp, dst;	// temporary and final target path; sequetially download and copy
	size_t		file_size=0;
	time_t		mtime=0;

	virtual ~file_t(){ release(); }
	void release(){ if(!hfile) return; InternetCloseHandle(hfile); hfile=nullptr; }
	bool open( HINTERNET session, const char* url );
	bool get_file_size( HINTERNET session );
};

struct session_t
{
	HINTERNET handle = nullptr;

	virtual ~session_t(){ release(); }
	session_t(){ if(!is_online()) return; handle=InternetOpenW( atow(path_t(exe::path()).filename().c_str()), INTERNET_OPEN_TYPE_PRECONFIG,0,0,0); }
	void release(){ if(!handle) return; InternetCloseHandle(handle); handle=nullptr; }
	operator bool() const { return handle!=nullptr; }

	inline bool download( string url, path_t dst ){ return download(vector<string>{url},dst); }
	bool download( vector<string> urls, path_t dst );

protected:
	bool download_thread_func( vector<string> urls, path_t dst );
};

__noinline bool file_t::open( HINTERNET session, const char* url )
{
	if(!session){ fprintf( stdout, "%s(%s): session==nullptr", __func__, url ); return false; }

	wchar_t canonical_url[4096]; DWORD dwSize=4096;
	InternetCanonicalizeUrlW( atow(url), canonical_url, &dwSize, ICU_BROWSER_MODE );

	static const DWORD flags = INTERNET_FLAG_TRANSFER_BINARY|INTERNET_FLAG_RELOAD|INTERNET_FLAG_NO_CACHE_WRITE; // INTERNET_FLAG_PRAGMA_NOCACHE
	release(); hfile=InternetOpenUrlW(session,canonical_url,0,0,flags,0); if(!hfile){ release(); return false; }

	DWORD dw_size=0;
	//DWORD status; dw_size=sizeof(status); if(!HttpQueryInfoW(hfile,HTTP_QUERY_STATUS_CODE|HTTP_QUERY_FLAG_NUMBER,&status,&dw_size,NULL)||status!=HTTP_STATUS_OK){ fprintf( stdout, "%s(%s): https status != HTTP_STATUS_OK\n", __func__, url ); release(); return false; }
	SYSTEMTIME msystime={}; dw_size=sizeof(SYSTEMTIME);
	if(!HttpQueryInfoW(hfile,HTTP_QUERY_LAST_MODIFIED|HTTP_QUERY_FLAG_SYSTEMTIME,&msystime,&dw_size,NULL)){ release(); return false; }

	msystime.wMilliseconds=0; // discard ms; HTTP_QUERY_LAST_MODIFIED returns varying seconds
	mtime = SystemTimeToTime(msystime);
	
	this->url = url;
	return this->hfile!=nullptr;
}

__noinline bool file_t::get_file_size( HINTERNET session )
{
	if(!hfile) return false;
	DWORD content_length=0, dw_size=sizeof(content_length); if(!HttpQueryInfoW(hfile,HTTP_QUERY_CONTENT_LENGTH|HTTP_QUERY_FLAG_NUMBER,&content_length,&dw_size,NULL)){ release(); return false; }
	file_size = content_length;
	return true;
}

__noinline bool session_t::download_thread_func( vector<string> urls, path_t dst )
{
	if(!handle) return false;

	// fetch timestamp of the existing file
	bool b_dst_exists = dst.exists();
	time_t f0=0; if(b_dst_exists) f0=dst.mtime();

	for( const auto& url : urls )
	{
		file_t f; if(!f.open(handle,url.c_str())) continue; // url not exists
		//auto s = TimeToSystemTime(f.mtime); fprintf( stdout, "%s %04d-%02d-%02d-%02d-%02d-%02d-%02d\n", url.c_str(), s.wYear, s.wMonth, s.wDay, s.wHour, s.wMinute, s.wSecond, s.wMilliseconds );

		// server-local time difference can be up to several seconds
		if(b_dst_exists&&!time_greater(f.mtime,f0)) return false; // older url file exists

		if(!f.get_file_size(handle)){ fprintf(stdout,"error: unable to get file size %s\n", dst.filename().c_str() );return false;} // now try to get the file size
		vector<char> buffer(f.file_size);
		
		if(!dst.dir().exists()&&!dst.dir().mkdir()){ fprintf(stdout, "error: unable to create %s\n", dst.dir().c_str() ); return false; }
		FILE* fp = fopen(dst.c_str(),"wb"); if(!fp){ fprintf(stdout, "error: unable to write to %s\n", dst.filename().c_str()); return false; }
		DWORD dw_size, dw_read; do
		{
			InternetQueryDataAvailable(f.hfile, &dw_size, 0, 0);
			if(dw_size>buffer.size()) dw_size = DWORD(buffer.size());
			InternetReadFile(f.hfile, buffer.data(), dw_size, &dw_read);
			if(dw_read) fwrite( buffer.data(), 1, dw_read, fp );
		} while(dw_read);
		fclose(fp);

		// modify the time stamp
		dst.utime(f.mtime);
		// fprintf( stdout, "%s\n", dst.name() );

		return true;
	}

	return false;
}

__noinline bool session_t::download( vector<string> urls, path_t dst )
{
	if(is_offline()||!handle||urls.empty()||dst.empty()) return false;
	auto t=std::async(std::launch::async,&session_t::download_thread_func,this,urls,dst);
	while(std::future_status::ready!=t.wait_for(std::chrono::milliseconds(1)));
	return t.get();
}

// registered ip can be retrieved using cmdline:
// >> nslookup myip.opendns.com resolver1.opendns.com
__noinline const char* get_registered_ip_address()
{
	static char* buff=nullptr; if(buff) return buff; buff=(char*)malloc(1024*sizeof(char));
	WSADATA wsadata; if(WSAStartup(MAKEWORD(2,2), &wsadata)!=0) return nullptr;
	DWORD dns[]={1,1111}; if(!inet_pton(AF_INET,"208.67.222.222",(struct sockaddr_in*)&dns[1])) return nullptr;
	PDNS_RECORD rec; if(DnsQuery_W(L"myip.opendns.com",DNS_TYPE_A,DNS_QUERY_BYPASS_CACHE,dns,&rec,nullptr)!=0||!rec) return nullptr;
	if(rec&&buff&&!inet_ntop(AF_INET,(struct sockaddr_in*)&rec->Data.A.IpAddress,buff,1024)) return nullptr;
	DnsFree(rec, DnsFreeRecordList);
	return buff;
}

//***********************************************
}} // namespace gx::inet
//***********************************************

#endif // __GX_INET_H__
