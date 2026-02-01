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
#ifndef __GX_GUID_H__
#define __GX_GUID_H__

#include "../gxut.h"
#if defined(_WIN32)||defined(_WIN64) // Windows
	#include <guiddef.h>
#endif

//***********************************************
// GUID conversion
struct guid_t : public GUID
{
	guid_t() noexcept { clear(); }
	guid_t( const GUID& other ) noexcept { operator=(other); }
	guid_t( GUID&& other ) noexcept { operator=(other); }
	guid_t( const char* other ) noexcept { operator=(other); }

	bool operator==( const GUID& other ) const noexcept { return memcmp(this,&other,sizeof(GUID))==0; }
	bool operator!=( const GUID& other ) const noexcept { return memcmp(this,&other,sizeof(GUID))!=0; }

	guid_t& operator=( const GUID& other ) noexcept { memcpy(this,&other,sizeof(GUID)); return *this; }
	guid_t& operator=( GUID&& other ) noexcept { auto t=*this; memcpy(this,&other,sizeof(GUID)); memcpy(&other,&t,sizeof(GUID)); return *this; }
	guid_t& operator=( const char* other ) noexcept { auto* h=__to_hex(other); if(!h||!*h||!is_guid(other,h)){printf("guid_t(%s): not a guid\n",other); return clear(); } auto* d=Data4; int read_fields=sscanf(h,"%08lX%04hX%04hX%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX",&Data1,&Data2,&Data3,d,d+1,d+2,d+3,d+4,d+5,d+6,d+7); if(read_fields!=11) return clear(); return *this; }

	operator GUID (){ return *this; }
	operator const GUID () const { return *this; }
	operator bool() const { static constexpr decltype(Data4) z={}; return Data1||Data2||Data3||memcmp(Data4, &z, sizeof(z))!=0; }
	const char* c_str( bool lowercase=false, bool braces=true ) const { char* b=__strbuf(39); auto* d=Data4; snprintf(b,39,lowercase?"{%08lx-%04hx-%04hx-%02hhx%02hhx-%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx}":"{%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX}",Data1,Data2,Data3,d[0],d[1],d[2],d[3],d[4],d[5],d[6],d[7]); if(!braces) (++b)[36]=0; return b; }
	guid_t& clear(){ memset(this,0,sizeof(GUID)); return *this; }

	static bool is_guid( const char* s, const char* hex=nullptr ){ return s&&strlen(hex?hex:__to_hex(s))==32; }
protected:
	static const char* __to_hex( const char* s ){ if(!s) return nullptr; auto* h = __strbuf(32); size_t j=0; for(size_t k=0,kn=strlen(s);k<kn;k++,s++) if(*s&&isalnum(*s)) h[j++]=*s; return j==32?h:""; }
};

#endif // __GX_GUID_H__
