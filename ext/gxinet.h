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

#include <ws2tcpip.h>
#include <windns.h>
#include <wininet.h>
#include <urlmon.h>
#pragma comment( lib, "wininet" )
#pragma comment( lib, "urlmon.lib" )

struct inet
{
	static bool is_offline(){ static int status=-1; if(status<0){ DWORD s; status=InternetGetConnectedState(&s,0)&&s!=INTERNET_CONNECTION_OFFLINE?1:0; } return status<1; }
	static inline const path_t CACHE_DIR = apptemp()+"update\\";
	static const char* get_registered_ip_address();
};

// raw download primitive
__noinline bool wget( string url, path local_path, bool use_index=true )
{
	if(inet::is_offline()||url.empty()) return false;
	local_path.dir().mkdir();
	time_t t=0;

	// download index and update mtime by the index if available
	string index_dir = path(url).dir();
	string index_url = index_dir+"index.txt";
	if(use_index&&index_url!=url)
	{
		static std::map<string,nocase::map<string,time_t>> index_maps;
		auto j=index_maps.find(index_dir); if(j==index_maps.end())
		{
			index_maps[index_dir]={}; j=index_maps.find(index_dir);
			path index_path = inet::CACHE_DIR+"index.txt";
			DeleteUrlCacheEntryA(index_url.c_str());
			if(S_OK==URLDownloadToFileA(0,index_url.c_str(),index_path.c_str(),0,0)&&index_path.exists())
			{
				auto findex = index_path.read_file();
				for(auto line:explode(findex.c_str(),"\r\n"))
				{
					if(line.empty()) continue; auto v=explode( line.c_str() ); if(v.empty()||v.front().empty()) continue;
					time_t u=0; if(v.size()>=2){ time_t u1=0; sscanf( v[1].c_str(), "%llx", &u1 ); if(u1) u=u1; }
					j->second.emplace(v.front(),u);
				}
			}
		}
		auto k=j->second.find(path(url).name());
		if(k!=j->second.end()) t=k->second;
	}
	
	if(t&&local_path.mtime()>=t) return true;
	DeleteUrlCacheEntryA(url.c_str());
	if(S_OK!=URLDownloadToFileA(0,url.c_str(), local_path.c_str(), 0, 0)) return false;
	if(t&&local_path.exists()) local_path.utime(t);
	return local_path.exists();
}

// registered ip can be retrieved using cmdline:
// >> nslookup myip.opendns.com resolver1.opendns.com
__noinline const char* inet::get_registered_ip_address()
{
	static char* buff=nullptr; if(buff) return buff; buff=(char*)malloc(1024*sizeof(char));
	WSADATA wsadata; if(WSAStartup(MAKEWORD(2,2), &wsadata)!=0) return nullptr;
	DWORD dns[]={1,1111}; if(!inet_pton(AF_INET,"208.67.222.222",(struct sockaddr_in*)&dns[1])) return nullptr;
	PDNS_RECORD rec; if(DnsQuery_W(L"myip.opendns.com",DNS_TYPE_A,DNS_QUERY_BYPASS_CACHE,dns,&rec,nullptr)!=0||!rec) return nullptr;
	if(rec&&buff&&!inet_ntop(AF_INET,(struct sockaddr_in*)&rec->Data.A.IpAddress,buff,1024)) return nullptr;
	DnsFree(rec, DnsFreeRecordList);
	return buff;
}

#endif // __GX_INET_H__
