#pragma once
#ifndef __GX_INET_H__
#define __GX_INET_H__

#if __has_include("gxfilesystem.h")
	#include "gxfilesystem.h"
#elif __has_include("../gxfilesystem.h")
	#include "../gxfilesystem.h"	
#elif __has_include(<gxut/gxfilesystem.h>)
	#include <gxut/gxfilesystem.h>
#endif

#include <winhttp.h>
#include <ws2tcpip.h>
#include <wininet.h>
#include <windns.h>
#pragma comment( lib, "winhttp.lib" )
#pragma comment( lib, "ws2_32" )
#pragma comment( lib, "wininet" )
#pragma comment( lib, "dnsapi" )

struct inet
{
	static bool is_offline(){ static int status=-1; if(status<0){ DWORD s; status=InternetGetConnectedState(&s,0)&&s!=INTERNET_CONNECTION_OFFLINE?1:0; } return status<1; }
	static inline const path_t CACHE_DIR = apptemp()+"update\\";
	static const char* get_registered_ip_address();
};

// raw download primitive: return true only when a new file is downloaded or up-to-date cache exists
__noinline bool wget( string remote_url, path local_path )
{
	struct handle { handle(HINTERNET h):ptr(h){} ~handle(){ if(!ptr) return; WinHttpCloseHandle(ptr); ptr=nullptr; } operator HINTERNET(){ return ptr; } HINTERNET ptr=nullptr; };
	if(inet::is_offline()){ fprintf(stdout,"%s(): offline\n",__func__); return false; }
	url u=remote_url; if(remote_url.empty()) return false;
	bool https=u.protocol=="https";
	int port=u.port?u.port:https?443:u.protocol=="http"?80:0; if(!port){ fprintf(stdout,"%s(): no valid port\n",__func__); return false; }

	handle s=WinHttpOpen(L"gxinet/1.0",WINHTTP_ACCESS_TYPE_NO_PROXY,WINHTTP_NO_PROXY_NAME,WINHTTP_NO_PROXY_BYPASS,0); if(!s){ fprintf(stdout,"%s(%s): unable to open http\n",__func__,remote_url.c_str()); return false; }
	handle c=WinHttpConnect(s,atow(u.host.c_str()),port,0); if(!c){ printf("%s(%s): unable to connect to %s\n",__func__,remote_url.c_str(),u.host.c_str()); return false; }
	handle r=WinHttpOpenRequest(c,L"GET",atow(u.path.c_str()),nullptr,0,0,WINHTTP_FLAG_REFRESH|(https?WINHTTP_FLAG_SECURE:0)); if(!r){ printf("%s(%s): unable to open request\n", __func__, remote_url.c_str() ); return false; }
	if(!WinHttpSendRequest(r,0,0,0,0,0,0)){ printf("%s(%s): unable to send request\n",__func__,remote_url.c_str()); return false; }
	if(!WinHttpReceiveResponse(r,0)){ printf("%s(%s): unable to receive response\n",__func__,remote_url.c_str()); return false; }

	SYSTEMTIME st={}; DWORD z=sizeof(SYSTEMTIME);
	BOOL q=WinHttpQueryHeaders(r,WINHTTP_QUERY_LAST_MODIFIED|WINHTTP_QUERY_FLAG_SYSTEMTIME,0,&st,&z,0); if(!q){ printf("%s(%s): unable to query headers\n",__func__,remote_url.c_str()); return false; }
	if(st.wYear==0){ printf("%s(%s): last modified time not exists\n",__func__,remote_url.c_str()); return false; }
	time_t t=SystemTimeToTime(st); if(local_path.exists()&&local_path.mtime()>=(t-2)) return true;
	
	if(!local_path.dir().exists()) local_path.dir().mkdir();
	FILE* fp=fopen(local_path.c_str(),"wb"); if(!fp){ printf("%s(%s): unable to open %s\n",__func__,remote_url.c_str(), local_path.to_slash().c_str()); return false; }
	const size_t capacity=1<<16; static void* buffer = malloc(capacity);
	DWORD bytes_read=0;
	while(WinHttpQueryDataAvailable(r,&z)&&z>0)
	{
		if(!WinHttpReadData(r,buffer,z<capacity?z:capacity,&bytes_read)){ printf("%s(%s): unable to read data\n",__func__,remote_url.c_str()); break; }
		if(bytes_read==0) break; fwrite(buffer,1,bytes_read,fp);
	}
	fclose(fp);
	if(!local_path.exists()) return false; // write failed
	local_path.utime(t);
	return true;
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
