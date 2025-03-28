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
#ifndef __GX_TIMER_H__
#define __GX_TIMER_H__

#if !defined(__GXUT_H__) && __has_include(<gxut/gxut.h>)
	#include <gxut/gxut.h>
#endif

#include <chrono>
using namespace std::chrono_literals;

//*************************************
namespace gx {
//*************************************

struct timer_t
{
	double x, y;
	
	__forceinline timer_t(){ x=now(); }
	__forceinline void clear(){ x=y=now(); }
	__forceinline double begin(){ return x=now(); }
	__forceinline double end(){ return (y=now())-x; }
	__forceinline double delta() const { return y-x; }
	
	__forceinline static double now(){ return std::chrono::duration_cast<std::chrono::duration<double,std::milli>>(std::chrono::high_resolution_clock::now()-__epoch()).count(); }
	__forceinline static timer_t& singleton(){ static timer_t* i=new timer_t(); return *i; }

protected:
	__forceinline static std::chrono::high_resolution_clock::time_point __epoch(){ static auto e = std::chrono::high_resolution_clock::now(); return e; } // local epoch for all instances
};

inline void usleep( long long us )
{
#if defined(__msvc__)&&__has_include(<winsock2.h>) // use higher-precision select() when available
	struct ws2select_t
	{
		HMODULE hdll=nullptr;
		int(*pf_select)(int,void*,void*,void*,void*)=nullptr;
		ws2select_t(){ hdll=LoadLibraryA("WS2_32.dll"); if(!hdll) return; pf_select=(decltype(pf_select))GetProcAddress(hdll,"select"); if(!pf_select) printf("ws2usleep_t::select()==nullptr\n"); }
		~ws2select_t(){ if(hdll) FreeLibrary(hdll); }
		bool select( int usec ){ if(!pf_select)return false;struct{long tv_sec,tv_usec;}t={usec/1000000,usec%1000000};pf_select(0,0,0,0,&t);return true; }
	};
	static ws2select_t ws2; if(ws2.select(us)) return;
#elif defined __gcc__
	std::this_thread::sleep_for(std::chrono::microseconds(us)); // actually, this doesn't work well due to too much context switch overhead
#endif
}

inline void sleep( long long ms ){ usleep(ms*1000); }

inline void limit_fps( double fps, double overhead_in_ms=0 ) // overhead: estimated delta to compensate slight differences
{
	static timer_t m; static auto t = m.now();
	auto t1 = t + 1000.0/fps-overhead_in_ms;
	for( t=m.now(); t<t1; t=m.now()) usleep(10);
}

#ifdef __msvc__
struct qpc_timer_t
{
	union { double2 result; struct { double x, y; }; };
	inline qpc_timer_t(){ begin(); }
	inline void clear(){ x=y=now(); }
	inline void begin(){ x=now(); }
	inline double end(){ return (y=now())-x; }
	inline double delta(){ return y-x; }
	static double now(){ double c=freq_scale(); int64_t e=epoch(); LARGE_INTEGER li; QueryPerformanceCounter(&li); return double(li.QuadPart-e)*c; }
	static qpc_timer_t* singleton(){ static qpc_timer_t i; return &i; }
	static double freq_scale(){ static double c=0; if(c==0){ LARGE_INTEGER li; QueryPerformanceFrequency(&li); c=1000.0/double(li.QuadPart); } return c; }
	static int64_t epoch(){ static int64_t e=0; if(e==0){ auto* ef=(int64_t(*)()) GetProcAddress(GetModuleHandleW(nullptr),"get_timer_epoch"); e=ef?ef():0; if(e==0){ LARGE_INTEGER li; QueryPerformanceCounter(&li); e=li.QuadPart;} } return e; }
};
#endif

//*************************************
} // namespace gx
//*************************************

#endif // __GX_TIMER_H__

