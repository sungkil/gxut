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
#ifndef __GX_APP_H__
#define __GX_APP_H__

#if !defined(__GXUT_H__) && __has_include(<gxut/gxut.h>)
	#include <gxut/gxut.h>
#endif

#if !defined(__GX_OS_H__) && __has_include(<gxut/gxos.h>)
	#include <gxut/gxos.h>
#endif

#if !defined(__GX_INET_H__)
	#if __has_include("gxinet.h")
		#include "gxinet.h"
	#elif __has_include(<gxut/gxinet.h>)
		#include <gxut/gxut.h>
	#elif __has_include(<gxut/ext/gxinet.h>)
		#include <gxut/ext/gxinet.h>
	#endif
#endif

#if !defined(__GXZIP_H__)
	#if __has_include("gxzip.h")
		#include "gxzip.h"
	#elif __has_include(<gxut/gxzip.h>)
		#include <gxut/gxzip.h>
	#elif __has_include(<gxut/ext/gxzip.h>)
		#include <gxut/ext/gxzip.h>
	#endif
#endif

//*************************************
namespace gx::app {
//*************************************

struct updater
{
	static inline const string	server_dir = "https://cg.skku.edu/apps/";
	static inline const path	cache_dir = path(os::local_appdata())+"gxapp\\update\\";

	const path	name;
	const path	dst;
	const struct { path src, dst, index; } cache;

	updater( string appname, path dst_path );
	bool fetch(); // return a newer server file exists
	bool open();
};

__noinline updater::updater( string appname, path dst_path ):
	name(appname.empty()?exe::name(true):appname), dst(dst_path)
{
	if(dst.empty()) const_cast<path&>(dst) = exe::path();
	const_cast<path&>(cache.dst) = cache_dir+dst.name();
	const_cast<path&>(cache.index) = cache_dir+"index.txt";
}

__noinline bool updater::fetch()
{
	if(inet::is_offline()) return false;

	// fetch index first
	wget(server_dir+"index.txt",cache.index); // try to download index; still may return false for cache
	if(!cache.index.exists()) return false; // no index found

	// read indices
	std::map<string,time_t> indices;
	for(auto&& l:explode(cache.index.read().c_str(),"\r\n"))
	{
		auto v=explode(l.c_str()," \t\n"); if(v.empty()||v.front().size()<2||v.front()[0]=='.') continue;
		string name=v.front(); time_t t=0; if(v.size()>1) sscanf(v[1].c_str(),"%llx",&t);
		indices.emplace(name,t);
	}

	// find an entry from the index
	path server_name;
	for( auto& t : {name,name+".7z",name+".zip",name.replace_extension("7z"),name.replace_extension("zip")} )
	{
		auto j=indices.find(t); if(j==indices.end()) continue;
		server_name=t.c_str(); break;
	}
	if(server_name.empty()) server_name=name+".7z"; // defaulted to 7z

	// now fetch the entry
	const_cast<path&>(cache.src) = cache_dir+server_name;
	bool b_zip = server_name.extension()=="zip"||server_name.extension()=="7z";
	time_t t0 = cache.src.mtime();
	bool b_wget = wget(server_dir+server_name.c_str(),cache.src); if(!cache.src.exists()) return false; // try to download; still may return false for cache
	//if(b_wget&&cache.src.mtime()>t0&&!b_zip) printf( "downloaded %s\n", cache.src.as() );

	if(b_zip) // try to extract the app from zip/7z
	{
		izip_t* z = load_zip(cache.src); if(!z){ printf( "[update] unable to load %s\n",cache.src.to_slash().c_str()); return false; }
		auto* e = z->find(cache.dst.name()); if(!e){ safe_delete(z); return false; }
		if(!cache.dst.exists()||FileTimeToTime(e->mtime)>cache.dst.mtime())
		{
			if(!z->extract_to_files(cache.dst.dir(),cache.dst.name())){ printf( "[update] unable to extract %s\n", cache.dst.name() ); safe_delete(z); return false; }
			//printf( "downloaded %s\n", cache.dst.as() );
		}
		safe_delete(z);
	}
	return cache.dst.exists()&&cache.dst.mtime()>dst.mtime();
}

__noinline bool updater::open()
{
	if(!cache.dst.exists()||cache.dst.mtime()<=dst.mtime()) return false;
	if(!mbox.confirm("Found a new update. Would you like to update the app?")) return false;
	return os::create_process( nullptr, format("%s --update %s",auto_quote(cache.dst.c_str()),auto_quote(dst.canonical().c_str())), false );
}

__noinline bool update( string name="", path dst_path="" )
{
	if(__argc>2&&wcscmp(__wargv[1],L"--update")==0)
	{
		path src1=path(exe::path());
		path dst1=__wargv[2]; if(!dst1.exists()) return false;
		// wait for dst1 closes
		int k=0; for(;k<1024;k++){ if(os::find_process(dst1.c_str()).empty())break; Sleep(10); }
		if(k==1024) return ebox("Unable to update, since %s is still alive.\n", dst1.rs());
		if(src1.copy_file(dst1)){ mbox("%s is updated well\n", dst1.rs()); return true; }
		return ebox("failed to update from %s to %s\n",src1.rs(),dst1.rs());
	}

	updater u(name,dst_path);
	if(!u.fetch()) return false;
	if(!u.open()) return false;
	return true;
}

//*************************************
} // namespace gx::app
//*************************************

#endif // __GX_APP_H__
