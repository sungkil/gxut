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

#include "../gxlib.h"
#if defined(_WIN32)||defined(_WIN64) // Windows
	#include <guiddef.h>
#endif

//***********************************************
// GUID conversion
struct guid_t : public GUID
{
	guid_t() noexcept { memset(this,0,sizeof(GUID)); }
	guid_t( const GUID& other ) noexcept { operator=(other); }
	guid_t( GUID&& other ) noexcept { operator=(other); }
	guid_t( const char* other ) noexcept { operator=(other); }
	explicit guid_t( const std::string& other ) noexcept { operator=(other.c_str()); }

	guid_t& operator=( const GUID& other ) noexcept { memcpy(this,&other,sizeof(GUID)); return *this; }
	guid_t& operator=( GUID&& other ) noexcept { auto t=*this; memcpy(this,&other,sizeof(GUID)); memcpy(&other,&t,sizeof(GUID)); return *this; }
	guid_t& operator=( const char* other ) noexcept { auto* d=Data4; sscanf_s( other, "{%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX}", &Data1,&Data2,&Data3,d,d+1,d+2,d+3,d+4,d+5,d+6,d+7 ); return *this; }
	guid_t& operator=( const std::string& other ) noexcept { return operator=(other.c_str()); }
	bool operator==( const GUID& other ) const noexcept { return memcmp(this,&other,sizeof(GUID))==0; }
	bool operator!=( const GUID& other ) const noexcept { return memcmp(this,&other,sizeof(GUID))!=0; }

	operator GUID (){ return *this; }
	operator const GUID () const { return *this; }
	operator const std::string () const { return c_str(); }
	operator bool() const { static decltype(Data4) z={}; return Data1||Data2||Data3||memcmp(Data4,&z,sizeof(z))!=0; }
	const char* c_str() const { char* b=__strbuf(64); auto* d=Data4; snprintf( b, 64, "{%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX}", Data1,Data2,Data3,d[0],d[1],d[2],d[3],d[4],d[5],d[6],d[7] ); return b; }
	void clear(){ memset(this,0,sizeof(GUID)); }
};

#endif // __GX_GUID_H__
