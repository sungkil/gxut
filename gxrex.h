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

#if !defined(__GXUT_H__)
	#if __has_include("gxut.h")
		#include "gxut.h"
	#elif __has_include(<gxut/gxut.h>)
		#include <gxut/gxut.h>
	#endif
#endif

//*************************************
namespace rex {
//*************************************

// general query to factory
inline const char* env( const char* query ){ static auto f=get_proc_address<const char*(*)(const char*)>("rex_get_env"); return f(query); } // rex environment variables (e.g., paths/directories)
inline bool is_logging(){ static bool* b=get_proc_address<bool*>("rex_is_logging"); return b?*b:false; } // project is using logging on the console?
inline bool is_animation(){ static bool* b=get_proc_address<bool*>("rex_is_animation"); return b?*b:false; }
inline path get_data_directory(){ return env("REX_DATA_DIR"); }
inline path get_project_data_directory( bool b_mkdir=true ){ path d=env("REX_PROJECT_DATA_DIR"); if(b_mkdir) d.mkdir(); return d; }
inline path get_script_directory(){ return env("REX_SCRIPT_DIR"); }
inline path get_capture_directory(){ return env("REX_CAPTURE_DIR"); }

// image functions that can be linked from the factory
inline image* load_image( const char* file_path, bool vflip=true, bool force_rgb=true, bool use_cache=false ){ static auto rex_load_image = get_proc_address<decltype(&load_image)>("rex_load_image"); return rex_load_image( file_path, vflip, force_rgb, use_cache ); }
inline image* load_image_header( const char* file_path ){ static auto rex_load_image_header = get_proc_address<decltype(&load_image_header)>("rex_load_image_header"); return rex_load_image_header( file_path ); }
inline image* load_image_from_memory( void* ptr, size_t size, bool vflip=true, bool force_rgb=true ){ static auto rex_load_image_from_memory = get_proc_address<decltype(&load_image_from_memory)>("rex_load_image_from_memory"); return rex_load_image_from_memory( ptr, size, vflip, force_rgb ); }
inline void   save_image( const char* file_path, image* pimage, bool vflip=true, bool force_rgb=false, int quality=95 ){ static auto rex_save_image = get_proc_address<decltype(&save_image)>("rex_save_image"); if(pimage) rex_save_image( file_path, pimage, vflip, force_rgb, quality ); }
inline void   resize_image( image* src, image* dst ){ static auto rex_resize_image = get_proc_address<decltype(&resize_image)>("rex_resize_image"); if(src&&dst) rex_resize_image(src,dst); }

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
	static progressive_t* ptr=get_proc_address<progressive_t*>("rex_get_progressive"); return *ptr;
}

#define progress (rex::get_progressive())

inline path capture_path( const char* extension, const char* postfix )
{
	static auto rex_capture_path=get_proc_address<const char*(*)(const char*,const char*)>("rex_capture_path"); if(!rex_capture_path) return "";
	return rex_capture_path(extension,postfix);
}

// open directory in explorer with reusing the pre-open window
inline void open_folder( path folder_path )
{
	if(!folder_path.exists()) return;
	static auto rex_open_folder = get_proc_address<void(*)(const char*)>("rex_open_folder"); if(rex_open_folder) rex_open_folder( folder_path.c_str() );
}

// capture info
struct capture_t
{
	// const input attributes
	const char* default_path=nullptr; // image dir or video path
	int frame=0, width=0, height=0, channels=0;
	struct { bool video=false, bgr=false; } b={};

	// mutable output attributes
	mutable const char*	user_image_path=nullptr;	// overriden image path

	capture_t(){ memset(this,0,sizeof(*this)); }
	capture_t( const capture_t& c ){ memcpy(this,&c,sizeof(*this)); }
	capture_t& operator=( const capture_t& c ){ memcpy(this,&c,sizeof(*this)); return *this; }
	capture_t& clear(){ memset(this,0,sizeof(*this)); return *this; }
};

// globally shared data; registerd or allocated in factory
struct shared_t
{
	struct var // shared variable type
	{
		std::type_index type=typeid(void*); void* ptr=nullptr; size_t size=0; string name; bool persistent; var()=default;
		template <class T> var(T& v,const char* _name,bool _persistent):type(typeid(T)),ptr((void*)&v),size(sizeof(T)),name(_name&&*_name?_name:""),persistent(_persistent){}
		var( std::type_index t, void* p, size_t z, const char* n, bool _persistent ):type(t),ptr(p),size(z),name(n&&*n?n:""),persistent(_persistent){}
	};

	static shared_t& instance();
	~shared_t(){ data.clear(); }
	void clear(){ for(auto it=data.begin();it!=data.end();){ if(it->second.persistent) it++; it=data.erase(it); } }
	template <class T> T&	set( const char* name, T& v, bool persistent=true ){ data.emplace(std::pair{name,var(v,name,persistent)}); return v; }
	template <class T> T*	get( const char* name ){ return (T*)get_impl(name,typeid(T),sizeof(T),false).ptr; }
	template <class T> void set_function( const char* name, T func, bool persistent=true ){ data.emplace(std::pair{name,var{typeid(T),func,sizeof(void*),name,persistent}}); }
	template <class T> T	get_function( const char* name ){ return (T)get_impl(name,typeid(T),sizeof(void*),false).ptr; }

protected:
	var& get_impl( const char* name, std::type_index type, size_t size, bool b_alloc ){ static var nil; auto it=data.find(name); if(it==data.end()){ if(!b_alloc) printf("shared_t::get(%s): unable to find\n",name); return nil; } auto& t=it->second; if(t.type==type) return t; printf("shared_t::get(%s): type is different from found\nquery: %s\nfound: %s\n", name, type.name(), t.type.name()); return nil; }
	nocase::map<string,var>	data;
};

// simpler wrappers
template <class T> T* shared( const char* name ){ return shared_t::instance().get<T>(name); }
template <class T> T shared_function( const char* name ){ return shared_t::instance().get_function<T>(name); }

// window functions
__noinline HWND get_window(){ HWND* h=rex::shared<HWND>("Window"); return h?*h:nullptr; }

//*************************************
} // end namespace rex
//*************************************

#ifdef REX_FACTORY_IMPL
// this is created only in application, and shared with any plugins
extern "C" __declspec(dllexport) inline rex::shared_t* __rex_shared=nullptr;
__noinline rex::shared_t& rex::shared_t::instance(){ if(!__rex_shared) __rex_shared=new rex::shared_t(); return *__rex_shared; }
#else
__noinline rex::shared_t& rex::shared_t::instance(){ static shared_t** i=(shared_t**)GetProcAddress(nullptr,"__rex_shared"); if(i&&*i) return **i; if(!i) printf( "shared_t::instance() not found\n"); else if(!*i) printf( "shared_t::instance()==nullptr\n"); static shared_t nil; return nil; }
#endif

//*************************************
namespace animation {
//*************************************
// context for user-defined animation; not related to the project-builtin animation

struct context_t
{
	virtual void play()=0; // run current animation seleted in Animation plugin
	virtual void stop()=0; // stop current animation seleted in Animation plugin
	virtual bool load_xml( const char* file_path )=0; // load from camera.xml or *.rex
	
	virtual bool empty() const=0;
	virtual size_t size() const=0;
	virtual void clear()=0;
	virtual void resize( size_t size )=0;
	virtual void push_back( const char* keyframe_name )=0;
	virtual void add_param( const char* param_name, const char* param_value, const char* plug_name=nullptr, float duration=1.0f )=0;
	virtual void add_param( uint keyframe_index, const char* param_name, const char* param_value, const char* plug_name=nullptr, float duration=1.0f )=0;
};

#ifndef REX_FACTORY_IMPL
	inline context_t* get_context(){ static context_t* c=nullptr; if(c) return c; auto f=get_proc_address<context_t*(*)()>("rex_get_animation_context"); if(!f) printf("failed to get_proc_address(rex_get_animation_context)\n"); c=f(); return c; }
#else
	context_t* get_context();
#endif

//*************************************
} // end namespace animation
//*************************************


#endif // __GX_REX_H__
