#pragma once
#ifndef __GX_LUA_H__
#define __GX_LUA_H__

// sol configuration
#define SOL_SAFE_USERTYPE	1
#define SOL_SAFE_REFERENCES	1
#define SOL_SAFE_FUNCTION	1

#if !defined(__GXUT_H__) && __has_include(<gxut/gxut.h>)
	#include <gxut/gxut.h>
#endif
#if !defined(__REX_H__) && __has_include(<rex.h>)
	#include <rex.h>
#endif
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
	#pragma comment( lib, "lua" )
	#include "Lua/lua.h"
	#include "Lua/lauxlib.h"
	#include "Lua/lualib.h"
#endif

// automatic linking
#ifndef NO_LINK_LUA_LIB
	#pragma comment( lib, "lua547s" )
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
	template<> void push<bool>( bool b ) const {		lua_pushboolean(L,b?1:0); }
	template<> void push<int>( int i ) const {			lua_pushinteger(L,i); }
	template<> void push<float>( float f ) const {		lua_pushnumber(L,(double)f); }
	template<> void push<double>( double f ) const {	lua_pushnumber(L,f); }
	template<> void push<void*>( void* ptr ) const {	lua_pushlightuserdata(L,ptr); }
	template<> void push<string>( string s ) const {	lua_pushstring(L,s.c_str()); }
	template<> void push<lua_CFunction>( lua_CFunction f ) const {	lua_pushcclosure(L,f,0); }
	
	void push_nil() const { lua_pushnil(L); }
	void push_string( const string& s ) const { lua_pushstring(L,s.c_str()); }
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
	template <> string			value<string>( int i )		const { return lua_tolstring(L,i,nullptr); }
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
	inline static int print( lua_State* L ){ string s; for(int i=1, nargs=lua_gettop(L);i<=nargs;i++){ if(lua_isstring(L, i)) s+=lua_tolstring(L,i,0); else if(lua_isnumber(L,i)) s+=ftoa(lua_tonumberx(L,i,0)); else if(lua_isinteger(L,i)) s+=itoa((int)lua_tointegerx(L,i,0)); else if(lua_type(L,i)==LUA_TBOOLEAN)	s+=btoa(lua_toboolean(L,i)); else if(lua_isuserdata(L,i)) s+=format("0x%p",lua_topointer(L,i)); } printf("%s",s.c_str()); return 0; }
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
	const char* error(){ static string msg; msg=lua_tolstring(L,-1,nullptr); lua_settop(L,-2); return msg.c_str(); }	
	
	// globals
	bool get_global( const char* name ){ lua_getglobal(L,name); bool b=lua_toboolean(L,-1)!=0; if(!b) lua_settop(L,-2); return b; }
	void set_global( const char* name ){ lua_setglobal(L,name); }

	// table manipulation
	void create_table( const char* name ){ lua_createtable(L,0,0); lua_setglobal( L, name ); }
	void set_table( int idx ){ lua_settable( L, idx ); }
	bool next( int idx ){ return lua_next(L,idx)!=0; }
};

// prefix for enabling logging
inline string prefix;

template <class V>
void log_dirty_value( const string& key, const V& dst )
{
	if(prefix.empty()) return;
	printf( "[%s] ", prefix.c_str() );
	if(!key.empty()) printf( "%s = ", key.c_str() );
	printf( "%s\n", ntoa<V>(dst) );
}

template <class T, class V>
bool get_dirty_value( const T& proxy, string key, V& dst )
{
	if(!proxy.valid()) return false;
	V v; if(!strchr(key.c_str(),'.')){ auto t=proxy[key]; if(!t.valid()||(v=V(t))==dst) return false; }
	else { auto k=explode(key.c_str(),"."); if(k.empty()) return false; auto t=proxy[k.back()]; if(!t.valid()||(v=V(t))==dst) return false; }
	dst=v; log_dirty_value(key,dst);
	return true;
}

struct state : public sol::state
{
	state( string _prefix="", bool b_open_default_libs=true );
	void open_default_libraries(){ open_libraries(sol::lib::base,sol::lib::math); }
	void open_gxmath();
	void open_printf();

	bool script( const char* src, bool b_safe=true ); // override default script
	template <class V> bool get_dirty_value( string key, V& dst ){ auto t=this->operator[](key); V v; if(!t.valid()||(v=t)==dst) return false; dst=v; log_dirty_value(key,dst); return true; }
	void gc(){ lua_gc( this->lua_state(), LUA_GCCOLLECT, 0 ); }
protected:
	std::set<string> _default_objects;
};

// lua internal exception handler
__noinline int lua_exception_handler( lua_State* L, sol::optional<const std::exception&> maybe_exception, sol::string_view desc )
{
	string m=maybe_exception?maybe_exception->what():string(desc.data(),static_cast<std::streamsize>(desc.size()));
	return sol::stack::push(L,m);
}

__noinline state::state( string _prefix, bool b_open_default_libs )
{
	prefix=_prefix;
	if(b_open_default_libs) open_default_libraries();
	// set_exception_handler(&lua_exception_handler); // handle exceptions in host instead of lua internals
	for(const auto& [key,value]:*this) _default_objects.insert(key.as<string>());
}

__noinline sol::protected_function_result _exception_handler( lua_State* L, sol::protected_function_result result )
{
	printf("[%s] %s\n", prefix.c_str(), result.operator string().c_str());
	return result;
};

__noinline bool state::script( const char* src, bool b_safe )
{
	sol::protected_function_result r=!b_safe?safe_script( src, _exception_handler /*&sol::script_pass_on_error*/ ):
		__super::script( src, _exception_handler /*&sol::script_pass_on_error*/ );
	return r.valid();
}

__noinline bool __lprint_impl( lua_State* L, string& r )
{
	// refer to: luaB_print in lbaselib.c
	int kn=lua_gettop(L);
	lua_getglobal(L, "tostring");
	for( int k=1;k<=kn;k++)
	{
		lua_pushvalue(L,-1);  // function to be called
		lua_pushvalue(L,k);   // value to print
		lua_call(L, 1, 1);
		const char* s = lua_tostring(L,-1); if(!s) return false;
		if(k>1) r+=" "; r+=s;
		lua_pop(L, 1);
	}
	lua_pop(L, 2);
	return true;
}

__noinline int lprint( lua_State* L )
{
	string r; if(!__lprint_impl(L,r)) return luaL_error(L, format("%s(): tostring must return a string\n",__func__) );
	::printf("%s\n",r.c_str());
	return 0;
}

__noinline int ltprint( lua_State* L )
{
	string r; if(!__lprint_impl(L,r)) return luaL_error(L, format("%s(): tostring must return a string\n",__func__) );
	::tprintf("%s\n",r.c_str()); return 0;
}

// gxlua libraries
__noinline int lprintf( lua_State *L )
{
	lua_pushvalue(L,lua_upvalueindex(1)); lua_insert(L, 1);	// string.format>[args...]
	lua_call(L, lua_gettop(L)-1, 1);		// string.format(args)
	::printf( "%s", lua_tostring(L, 1) );	// print output on the stack top
	lua_pop(L,1);							// pop output
	return 0;
}

__noinline int ltprintf( lua_State *L )
{
	lua_pushvalue(L,lua_upvalueindex(1)); lua_insert(L, 1);	// string.format>[args...]
	lua_call(L, lua_gettop(L)-1, 1);		// string.format(args)
	::tprintf( "%s", lua_tostring(L, 1) );	// print output on the stack top
	lua_pop(L,1);							// pop output
	return 0;
}

__noinline void state::open_printf()
{
	set_function( "print", lprint );
	set_function( "tprint", ltprint );

	auto* L = lua_state();
	lua_getglobal(L, "string");			// [string]
	lua_pushstring(L, "format");		// [string, "format"]
	lua_gettable(L, -2);				// [string, string.format]
	lua_pushcclosure(L, lprintf, 1);	// [string, string.format, lprintf]
	lua_setglobal(L, "printf");			// [string]
	lua_pushstring(L, "format");		// [string, "format"]
	lua_gettable(L, -2);				// [string, string.format]
	lua_pushcclosure(L, ltprintf, 1);	// [string, string.format, ltprintf]
	lua_setglobal(L, "tprintf");		// [string]
	lua_pop(L,1);						// []
}

__noinline string __call_tostring( sol::stack_object v, sol::this_state L )
{
	sol::type t = v.get_type();
	if(t==sol::type::boolean)	return v.as<bool>()?"true":"false";
	if(t==sol::type::number)	return ftoa(v.as<double>());
	if(t==sol::type::string)	return v.as<string>();
	if(t==sol::type::userdata)
	{
		lua_getglobal(L, "tostring"); lua_pushvalue(L,-1); lua_pushvalue(L,v.stack_index());  // [tosring, function to be called, value to print]
		lua_call(L, 1, 1); const char* s=lua_tostring(L,-1); string r=s?s:"";
		lua_pop(L, 2); // pop [output,string]
		return r;
	}
	return "";
}

__noinline sol::object concat( sol::stack_object left, sol::stack_object right, sol::this_state L )
{
	string l=__call_tostring(left,L);
	string r=__call_tostring(right,L);
	lua_pop(L, 2); // pop [left, right]
	return sol::object( L, sol::in_place, l+r );
}

//*************************************
} // end namespace lua
//*************************************

// type helper macros
#define SOL_DEMANGLE_ONCE(T) \
	template <> inline string demangle_once<T>() { return #T; }\
	template <> inline string short_demangle_once<T>() { return #T; }

#define SOL_DEMANGLE_ONCE2(T,N) \
	template <> inline string demangle_once<T>() { return #N; }\
	template <> inline string short_demangle_once<T>() { return #N; }

#endif __GX_LUA_H__