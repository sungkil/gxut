#pragma once
#ifndef __GX_LUA_H__
#define __GX_LUA_H__

#ifdef __has_include
	#if __has_include("sol/sol.hpp")
		#include "sol/sol.hpp"
	#elif __has_include("../lua/sol/sol.hpp")
		#include "../lua/sol/sol.hpp"
	#elif __has_include("../../lua/sol/sol.hpp")
		#include "../../lua/sol/sol.hpp"
	#elif __has_include(<lua/sol/sol.hpp>)
		#include <lua/sol/sol.hpp>
	#elif __has_include(<lua/sol.hpp>)
		#include <lua/sol.hpp>
	#elif __has_include(<sol/sol.hpp>)
		#include <sol/sol.hpp>
	#elif // no sol is defined: fallback to regular lua
		#pragma comment( lib, "lua.lib" )
		#include "Lua/lua.h"
		#include "Lua/lauxlib.h"
		#include "Lua/lualib.h"
	#endif
#endif

//*************************************
namespace lua {
//*************************************

// type definitions
using cfunction = lua_CFunction;

// legacy lua stack helper
struct stack_t
{
	lua_State* L;

	// stack properties
	int size(){ return lua_gettop(L); }

	// element properties
	size_t raw_len( int i ) const { return lua_rawlen(L,i); }
	
	// push
	template <class T=void> void push( T v ) const {}
	template<> void push<bool>( bool b ) const {				lua_pushboolean(L,b?1:0); }
	template<> void push<int>( int i ) const {					lua_pushinteger(L,i); }
	template<> void push<float>( float f ) const {				lua_pushnumber(L,(double)f); }
	template<> void push<double>( double f ) const {			lua_pushnumber(L,f); }
	template<> void push<void*>( void* ptr ) const {			lua_pushlightuserdata(L,ptr); }
	template<> void push<std::string>( std::string s ) const {	lua_pushstring(L,s.c_str()); }
	template<> void push<lua_CFunction>( lua_CFunction f ) const {	lua_pushcclosure(L,f,0); }
	
	void push_nil() const { lua_pushnil(L); }
	void push_string( const std::string& s ) const { lua_pushstring(L,s.c_str()); }
	void push_string( const char* s ) const { lua_pushstring(L,s); }
	void push_pointer( void* ptr ) const { lua_pushlightuserdata(L,ptr); }
	void push_function( lua_CFunction f, int num_upvalues=0 ) const { lua_pushcclosure(L,f,num_upvalues); }
	void push_value( int i ){ lua_pushvalue(L,i); }

	// top/pop
	template <class T> T top(){ return value<T>(-1); }
	void pop( int n=1 ){ if(lua_gettop(L)>0) lua_settop(L,-1-n); }

	// accessors
	template <class T> T		value( int i )				const;
	template <> const char*		value<const char*>( int i )	const { return lua_tolstring(L,i,nullptr); }
	template <> std::string		value<std::string>( int i )	const { return lua_tolstring(L,i,nullptr); }
	template <> bool			value<bool>( int i )		const { return lua_toboolean(L,i)!=0; }
	template <> int				value<int>( int i )			const { return (int) lua_tointegerx(L,i,nullptr); }
	template <> uint			value<uint>( int i )		const { return (uint) lua_tointegerx(L,i,nullptr); }
	template <> float			value<float>( int i )		const { return (float) lua_tonumberx(L,i,nullptr); }
	template <> double			value<double>( int i )		const { return (double) lua_tonumberx(L,i,nullptr); }
	
	// special accessors
	void*		pointer( int i )	const { return (void*) lua_topointer(L,i); }
	void*		userdata( int i )	const { return lua_touserdata(L,i); }
	cfunction	function( int i )	const { return lua_tocfunction(L,i); }
};

// legacy state helper
struct lua_state_t
{
	union { lua_State* L; stack_t stack; };
	
	// basic functions
	lua_state_t(){ L=nullptr; }
	~lua_state_t(){ release(); }
	void reset(){ if(L) release(); L=luaL_newstate(); luaL_openlibs(L); register_default_lib(L); }	// create and open standard libraries
	void release(){ if(L) lua_close(L); L=nullptr; }
	operator lua_State*(){ return L; }

	// library
	inline static int print( lua_State* L ){ std::string s; for(int i=1, nargs=lua_gettop(L);i<=nargs;i++){ if(lua_isstring(L, i)) s+=lua_tolstring(L,i,0); else if(lua_isnumber(L,i)) s+=dtoa(lua_tonumberx(L,i,0)); else if(lua_isinteger(L,i)) s+=itoa((int)lua_tointegerx(L,i,0)); else if(lua_type(L,i)==LUA_TBOOLEAN)	s+=btoa(lua_toboolean(L,i)); else if(lua_isuserdata(L,i)) s+=format("0x%p",lua_topointer(L,i)); } printf("%s",s.c_str()); return 0; }
	inline static void register_default_lib( lua_State* L ){ static const struct luaL_Reg libs[]={{"print",print},{0,0}}; lua_getglobal(L,"_G");luaL_newlib(L,libs);lua_settop(L,-3); }
	void require_c( const char* name, lua_CFunction func ){ luaL_requiref(L,name,func,0); }
	void require_string( const char* name, const char* src ){ luaL_getsubtable(L,LUA_REGISTRYINDEX,"_LOADED"); lua_getfield(L,-1,name ); if(!lua_toboolean(L,-1)){ do_string(src); lua_setfield(L,-3,name); } lua_settop(L,-3); /*restore the stack*/ } // equivalent to : package.loaded[name] = load(src)() in Lua code
	void require_file( const char* name, const char* file_path ){ luaL_getsubtable(L,LUA_REGISTRYINDEX,"_LOADED"); lua_getfield(L,-1,name ); if(!lua_toboolean(L,-1)){ do_file(file_path); lua_setfield(L,-3,name); } lua_settop(L,-3); /*restore the stack*/ } // equivalent to : package.loaded[name] = load(src)() in Lua code

	// script functions
	bool load_string( const char* src ) const { return luaL_loadstring(L,src)==0; }
	bool load_file( const char* file_path ) const { return luaL_loadfilex(L,file_path,0)==0; }
	bool do_string( const char* src ) const { return load_string(src)&&pcall(0); }
	bool do_file( const char* file_path ) const { return load_file(file_path)&&pcall(0); }
	bool pcall( int nargs, int nresults=LUA_MULTRET ) const { return lua_pcallk(L,nargs,nresults,0,0,0)==0; }
	bool pcall( const char* func_name, int nargs, int nresults=LUA_MULTRET )
	{
		if(!get_global(func_name)){ printf( "unable to find function %s\n", func_name ); return false; }
		if(!pcall(nargs,nresults)){ printf( "script fails: %s\n", error() ); return false; }
		return true;
	}
	
	// utilities: garbage collection, error handling
	void gc(){ lua_gc( L, LUA_GCCOLLECT, 0 ); }
	const char* error(){ static std::string msg; msg=lua_tolstring(L,-1,nullptr); lua_settop(L,-2); return msg.c_str(); }	
	
	// globals
	bool get_global( const char* name ){ lua_getglobal(L,name); bool b=lua_toboolean(L,-1)!=0; if(!b) lua_settop(L,-2); return b; }
	void set_global( const char* name ){ lua_setglobal(L,name); }

	// table manipulation
	void create_table( const char* name ){ lua_createtable(L,0,0); lua_setglobal( L, name ); }
	void set_table( int idx ){ lua_settable( L, idx ); }
	bool next( int idx ){ return lua_next(L,idx)!=0; }
};

__noinline int _exception_handler( lua_State* L, sol::optional<const std::exception&> maybe_exception, sol::string_view desc )
{
	std::string m=maybe_exception?maybe_exception->what():std::string(desc.data(),static_cast<std::streamsize>(desc.size()));
	return sol::stack::push(L,m);
}

struct state : public sol::state
{
	state( bool b_open_default_libs=true, bool b_default_exception_handler=true ){ if(b_default_exception_handler) set_exception_handler(&_exception_handler); if(b_open_default_libs) open_default_libraries(); }
	void open_default_libraries(){ open_libraries(sol::lib::base,sol::lib::math); }
	bool load_script( const char* src, const wchar_t* _prefix=nullptr, bool b_safe=true, bool b_log=true );

	// utilities
	void set_log( bool b_log=true ){ b.log=b_log; }
	template <class T> bool get_value( const char* name, T& dst ){ auto t=this->operator[](name); if(!t.valid()) return false; T v=t.get<T>(); if(v==dst) return false; dst=v; if(b.log){ if(!prefix.empty()) printf( "[%s] ", prefix.c_str() ); printf( "%s = %s\n", name, ntoa<T>(dst) ); } return true; }

protected:
	std::string prefix;
	struct { bool log=true; } b;
};

__noinline bool state::load_script( const char* src, const wchar_t* _prefix, bool b_safe, bool b_log )
{
	if(_prefix) prefix=wtoa(_prefix);
	sol::protected_function_result r=b_safe?safe_script( src, &sol::script_pass_on_error ):script(src,&sol::script_pass_on_error);
	if(r.valid()) return true;
	if(b_log){ sol::error e=r; if(!prefix.empty()) printf( "[%s] ", prefix.c_str() ); printf("%s\n",e.what()); }
	return false;
}

//*************************************
} // end namespace lua
//*************************************

//*************************************
#endif __GX_LUA_H__