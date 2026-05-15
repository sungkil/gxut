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
#include <netlistmgr.h>
#pragma comment( lib, "winhttp" )
#pragma comment( lib, "ws2_32" )
#pragma comment( lib, "wininet" )
#pragma comment( lib, "dnsapi" )

//*************************************
namespace inet {
//*************************************

static inline const path_t CACHE_DIR = apptemp()+"update\\";

struct ipv4_t : public uchar4
{
	using uchar4::uchar4;
	using uchar4::operator=;
	ipv4_t(){ x=y=z=w=0; }
	operator bool(){ return x||y||z||w; }
	const char* c_str(){ return operator bool()?format("%u.%u.%u.%u",x,y,z,w):""; }
};

// registered ip can be retrieved using cmdline:
// >> nslookup myip.opendns.com resolver1.opendns.com
__noinline ipv4_t get_registered_ip_address()
{
	static bool b=false; static ipv4_t ip; if(b) return ip; b=true;
	vector<char> v(1024,0); char* buff=v.data();
	WSADATA wsa; if(WSAStartup(MAKEWORD(2,2), &wsa)!=0) return {};
	DWORD dns[]={1,1111}; if(!inet_pton(AF_INET,"208.67.222.222",(struct sockaddr_in*)&dns[1])) return {};
	PDNS_RECORD rec; if(DnsQuery_W(L"myip.opendns.com",DNS_TYPE_A,DNS_QUERY_BYPASS_CACHE,dns,&rec,nullptr)!=0||!rec) return {};
	if(rec&&buff&&!inet_ntop(AF_INET,(struct sockaddr_in*)&rec->Data.A.IpAddress,buff,1024)) return {};
	DnsFree(rec, DnsFreeRecordList);
	uint4 u;sscanf(buff,"%u.%u.%u.%u",&u.x,&u.y,&u.z,&u.w);
	ip.x=u.x;ip.y=u.y;ip.z=u.z;ip.w=u.w;
	return ip;
}

__noinline bool is_offline()
{
	static bool i=false, b=true; if(i) return b; i=true; // defaulted to offline
	HRESULT hr0=CoInitializeEx(nullptr,COINIT_APARTMENTTHREADED|COINIT_DISABLE_OLE1DDE); if(FAILED(hr0)) return b;
	INetworkListManager* nlm=nullptr; if(FAILED(CoCreateInstance(CLSID_NetworkListManager,0,CLSCTX_ALL,IID_PPV_ARGS(&nlm)))) return b;
	VARIANT_BOOL c=VARIANT_FALSE; auto hr1=nlm->get_IsConnectedToInternet(&c);safe_release(nlm);
	if(hr0!=RPC_E_CHANGED_MODE) CoUninitialize();
	return b=(FAILED(hr1)||c!=VARIANT_TRUE);
}

// non-blocking fast ping (for within 10 ms)
__noinline bool ping( const char* ip, unsigned short port, int timeout_ms=100 )
{
	SOCKET s=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP); if(s==INVALID_SOCKET) return 0;
	u_long m=1; ioctlsocket( s, FIONBIO, &m );
	sockaddr_in a; a.sin_family=AF_INET; a.sin_port=htons(port); a.sin_addr.s_addr=inet_addr(ip);
	connect(s,(sockaddr*)&a,sizeof(a));
	fd_set wset; FD_ZERO(&wset); FD_SET(s,&wset);
	timeval tv={}; tv.tv_usec=timeout_ms*1000;
	int r=select(0,NULL,&wset,NULL,&tv ); if(r<=0){ closesocket(s); return false; }
	int e=0, l=sizeof(e); getsockopt(s,SOL_SOCKET,SO_ERROR,(char*)&e,&l); closesocket(s);
	return e==0;
}

//*************************************
} // end namespace inet
//*************************************

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

#endif // __GX_INET_H__
