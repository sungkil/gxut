//*********************************************************
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
//*********************************************************

#pragma once
#ifndef __GX_REX_H__
#define __GX_REX_H__

#if !defined(__GXUT_H__) && __has_include(<gxut/gxut.h>)
	#include <gxut/gxut.h>
#endif

//*************************************
namespace rex {
//*************************************

// image functions that can be linked from the factory
inline image* load_image( const char* file_path, bool vflip=true, bool force_rgb=true, bool use_cache=false ){ static decltype(&load_image) rex_load_image = (decltype(&load_image)) GetProcAddress(GetModuleHandleW(nullptr),"rex_load_image"); return rex_load_image( file_path, vflip, force_rgb, use_cache ); }
inline image* load_image_header( const char* file_path ){ static decltype(&load_image_header) rex_load_image_header = (decltype(&load_image_header)) GetProcAddress(GetModuleHandleW(nullptr),"rex_load_image_header"); return rex_load_image_header( file_path ); }
inline image* load_image_from_memory( void* ptr, size_t size, bool vflip=true, bool force_rgb=true ){ static decltype(&load_image_from_memory) rex_load_image_from_memory = (decltype(&load_image_from_memory)) GetProcAddress(GetModuleHandleW(nullptr),"rex_load_image_from_memory"); return rex_load_image_from_memory( ptr, size, vflip, force_rgb ); }
inline void   save_image( const char* file_path, image* pimage, bool vflip=true, bool force_rgb=false, int quality=95 ){ static decltype(&save_image) rex_save_image = (decltype(&save_image)) GetProcAddress(GetModuleHandleW(nullptr),"rex_save_image"); if(pimage) rex_save_image( file_path, pimage, vflip, force_rgb, quality ); }
inline void   resize_image( image* src, image* dst ){ static decltype(&resize_image) rex_resize_image = (decltype(&resize_image)) GetProcAddress(GetModuleHandleW(nullptr),"rex_resize_image"); if(src&&dst) rex_resize_image(src,dst); }

// progressive rendering plugin helper
struct progressive_t
{
	const uint begin=0;						// first iterator index for the current chunk
	const uint end=1;						// last iterator index for the current chunk
	virtual void enable( bool b )=0;		// enable or disable progressive rendereing
	virtual bool exists() const=0;			// progressive plugin exists
	virtual uint size() const=0;			// total number of samples
	virtual bool is_used() const=0;			// progressive plugin is enabled
	virtual bool is_complete() const=0;		// indicate if progressive is complete
	virtual operator bool() const { return exists()&&is_used(); }

	virtual void reset()=0;					// reset progressive rendering
	virtual void set_trigger( void* ptr, size_t size, bool enable=true )=0;	// register a user datum that triggers when it is dirty
	template <class T> void set_trigger( T& v, bool enable=true ){ set_trigger(&v,sizeof(T),enable); }
	template <class T> void set_trigger( const T& v, bool enable=true ){ set_trigger((void*)&v, sizeof(T), enable); }
};

inline progressive_t& get_progressive()
{
	static progressive_t* ptr=nullptr; if(ptr) return *ptr;
	auto pf=(progressive_t*(*)()) GetProcAddress(GetModuleHandleW(nullptr),"rex_get_progressive"); if(!pf) printf( "unable to get rex_get_progressive()\n" );
	if(pf) ptr=pf();
	return *ptr;
}

//*************************************
} // end namespace rex
//*************************************

#define progress (rex::get_progressive())

#endif // __GX_REX_H__
