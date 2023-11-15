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
#ifndef __GX_ARGPARSE_H__
#define __GX_ARGPARSE_H__
#ifdef __has_include
	#if __has_include("gxmath.h")
		#include "gxmath.h"
	#elif __has_include(<gxmath.h>)
		#include <gxmath.h>
	#endif
	#if __has_include("gxstring.h")
		#include "gxstring.h"
	#elif __has_include(<gxstring.h>)
		#include <gxstring.h>
	#endif
	#if __has_include("gxos.h")
		#include "gxos.h"
	#elif __has_include(<gxos.h>)
		#include <gxos.h>
	#endif
	#if __has_include("gxfilesystem.h")
		#include "gxfilesystem.h"
	#elif __has_include(<gxfilesystem.h>)
		#include <gxfilesystem.h>
	#endif
#endif

//***********************************************
namespace gx { namespace argparse {
//***********************************************

struct argument_t
{
	argument_t& add_help( const char* fmt, ... ){ va_list a; va_start(a,fmt); std::vector<char> buff(_vscprintf(fmt,a)+1); vsprintf_s(&buff[0],buff.size(),fmt,a); va_end(a); shelp=trim(&buff[0],"\n"); return *this; }
	argument_t& set_default( const char* v ){ value=atow(v); return *this; }
	argument_t& set_default( const wchar_t* v ){ value=v; return *this; }
	argument_t& set_optional(){ optional=true; return *this; } 

protected:
	std::string		name;
	std::wstring	value;
	std::string		shelp;
	bool			optional=false;

	friend struct parser_t;
	bool value_exists() const { return !value.empty(); }
};

struct option_t
{
	bool empty() const { return names.empty(); }
	option_t& subarg( const char* subarg_name ){ use_subarg=true; this->subarg_name=subarg_name; return *this; }
	option_t& add_help( const char* fmt, ... ){ va_list a; va_start(a,fmt); std::vector<char> buff(_vscprintf(fmt,a)+1); vsprintf_s(&buff[0],buff.size(),fmt,a); shelp=trim(&buff[0],"\n"); va_end(a); return *this; }
	option_t& add_break( int count=1 ){ break_count+=count; return* this; }
	option_t& set_default( const char* arg ){ value=atow(arg); return *this; }
	option_t& set_default( const wchar_t* arg ){ value=arg; return *this; }

protected:
	friend struct parser_t;
	bool exists() const { if(!instance) return false; return !use_subarg||!value.empty(); }
	template<typename... Args> option_t& add_name( const char* name, Args... args ){ if(name&&name[0]) names.insert(name); return add_name(args...); }
	option_t& add_name( const char* name ){ if(name&&name[0]) names.insert(name); return *this; }
	const char* name() const { for( auto& n : names ) if(n.size()==1) return n.c_str(); for( auto& n : names ) if(n.size()>1) return n.c_str(); return ""; }
	const char* short_name() const { for( auto& n : names ) if(n.size()==1) return n.c_str(); return ""; }
	std::vector<const char*> long_names() const { std::vector<const char*> v; for( auto& n : names ) if(n.size()>1) v.push_back(n.c_str()); return v; }

	struct lless { bool operator()(const std::string& a,const std::string& b)const{return a.size()!=b.size()?a.size()<b.size():_stricmp(a.c_str(),b.c_str())<0;}};
	std::set<std::string,lless>	names;				// multiple names allowed for a single option
	std::wstring				value;				// found first values
	std::vector<std::wstring>	others;				// additional excessive multiple options
	std::string					shelp;				// help string
	std::string					subarg_name;		// name of sub-argument
	bool						use_subarg=false;	// use a sub-argument
	int							instance=0;			// allow multiple instances of an option
	int							break_count=0;		// post-line breaks
};

struct parser_t
{
	using callback_t = bool(*)(parser_t&);

	// ctor/dtor
	virtual ~parser_t(){ for(auto& c:commands)safe_delete(c); for(auto& a:arguments)safe_delete(a); for(auto& o:options)safe_delete(o); }

	// attributes
	inline const char* name() const { return attrib.name.c_str(); }
	inline const wchar_t* wname() const { return atow(attrib.name.c_str()); }
	inline void add_header( const char* fmt, ... ){ va_list a; va_start(a,fmt); int l=_vscprintf(fmt,a); std::vector<char> buff(l+1); vsprintf_s(&buff[0],l+1llu,fmt,a); attrib.header=trim(&buff[0]); va_end(a); }
	inline void add_footer( const char* fmt, ... ){ va_list a; va_start(a,fmt); int l=_vscprintf(fmt,a); std::vector<char> buff(l+1); vsprintf_s(&buff[0],l+1llu,fmt,a); attrib.footer=trim(&buff[0]); va_end(a); }
	inline void add_copyright( const char* author, int since_year ){ attrib.copyright = format( "copyright (c) %d-%d by %s\n", since_year, gx::compiler::year()+1, author ); }
	inline parser_t& add_help( const char* fmt, ... ){ va_list a; va_start(a,fmt); std::vector<char> buff(_vscprintf(fmt,a)+1); vsprintf_s(&buff[0],buff.size(),fmt,a); attrib.help=trim(&buff[0],"\n"); va_end(a); return *this; }
	
	// query
	inline bool option_exists() const;
	inline bool exists( const std::string& name ) const;
	inline bool command_exists( const std::string& name ) const;
	inline bool callback_exists(){ parser_t* p=this;while(p&&!p->pf_callback) p=p->parent; return p&&p->pf_callback?true:false; }
	inline bool help_exists() const { return attrib.b.help_exists; }

	// subcommands
	inline parser_t& select_parser( int argc, wchar_t* argv[] ){ parser_t* p=this; select_parser_impl(p,argc,argv); return *p; }
	inline parser_t& add_command( const char* name ){ commands.emplace_back(new parser_t()); auto* c=commands.back(); c->parent=this; c->attrib.name=name; c->attrib.depth=attrib.depth+1; return *c; }
	inline parser_t& set_callback( callback_t cb ){ pf_callback=cb; return *this; }
	inline bool run(){ parser_t* p=this;while(p&&!p->pf_callback) p=p->parent; if(!p||!p->pf_callback){fprintf(stdout,"unable to find callback for %s\n",name());return false;} return p->pf_callback(*p); }
	inline const char* get_command_names() const { std::string c=this->name(); for( parser_t* p=this->parent; p; p=p->parent ) c = p->attrib.name+" "+c; return __tstrdup(c.c_str()); }

	// short template functions
	argument_t& add_argument( const char* name ){ arguments.push_back(new argument_t()); auto* a=arguments.back(); a->name=name; return *a; }
	template<typename... Args> option_t& add_option( Args... args ){ options.push_back(new option_t()); auto* o=options.back(); return o->add_name(args...);  }

	// long functions
	bool parse( int argc, wchar_t* argv[] ){ return parse_impl( argc, (const wchar_t**)argv ); }
	bool usage( const char* alt_name=nullptr );
	static const wchar_t* rebuild_arguments( int argc, wchar_t* argv[] );

	// get<> specializations, and other get functions
	template <class T=std::string>	inline T get( const std::string& name ) const;
	template<> inline std::wstring	get<std::wstring>( const std::string& name ) const;
	template<> inline std::string	get<std::string>( const std::string& name ) const { return wtoa(get<std::wstring>(name).c_str()); }
	template<> inline path			get<path>( const std::string& name ) const { return get<std::wstring>(name).c_str(); }
	template<> inline int			get<int>( const std::string& name ) const { return _wtoi(get<std::wstring>(name).c_str()); }
	template<> inline uint			get<uint>( const std::string& name ) const { return uint(_wtoi(get<std::wstring>(name).c_str())); }
	std::vector<std::wstring>		others( const std::string& name="" ) const;

	// get multiple values
	inline std::vector<std::wstring> get_values( const std::string& name ) const;

	// error handling, debugging
	bool exit( const char* fmt, ... ){ va_list a; va_start(a,fmt); const char* w=vformat(fmt,a); va_end(a); char msg[2048]; sprintf_s( msg, 2048, "[%s] %s\nUse -h option to see usage.\n", name(), trim(w,"\n") ); fprintf( stdout, msg ); return false; }
	bool exit( const wchar_t* fmt, ... ){ va_list a; va_start(a,fmt); const wchar_t* w=vformat(fmt,a); va_end(a); wchar_t msg[2048]; swprintf_s( msg, 2048, L"[%s] %s\nUse -h option to see usage.\n", wname(), trim(w,L"\n") ); fwprintf( stdout, msg ); return false; }
	void dump();
	
protected:

	struct attribute_t
	{
		std::string	name = path::module_path().name(false).wtoa();
		std::string copyright;
		std::string	header, footer;
		std::string help;		// shorter help for commands
		int			depth=0;	// depth to commands; non-zero means sub-commands
		struct { bool help_exists = false; } b;
		bool		instance=false;
	} attrib;

	parser_t*					parent = nullptr;
	callback_t					pf_callback = nullptr;
	std::vector<parser_t*>		commands;
	std::vector<argument_t*>	arguments;
	std::vector<option_t*>		options;

	option_t* find_option( const char* name ) const	{ if(!name||!name[0]) return nullptr; for( auto* o : options ) for( auto& n : o->names ) if(strcmp(name,n.c_str())==0) return o; return nullptr; }
	void select_parser_impl( parser_t*& p, int argc, wchar_t** argv );
	bool parse_impl( int argc, const wchar_t** argv );
};

inline bool parser_t::option_exists() const
{
	for( auto* o : options ) if(!o->empty()&&o->instance>0) return true;
	return false;
}

inline bool parser_t::exists( const std::string& name ) const
{
	if(name=="h"||name=="help") return attrib.b.help_exists;
	auto* o = find_option(name.c_str()); if(o) return o->instance>0;
	for( auto* a : arguments ){ if(strcmp(a->name.c_str(),name.c_str())==0) return a->value_exists(); }
	return false;
}

inline bool parser_t::command_exists( const std::string& name ) const
{
	for(auto* c:commands)
	{
		if(strcmp(c->attrib.name.c_str(),name.c_str())!=0) continue;
		if(c->attrib.instance) return true;
	}
	return false;
}

template<> inline std::wstring parser_t::get<std::wstring>( const std::string& name ) const
{
	for( auto* a : arguments ){ if(strcmp(a->name.c_str(),name.c_str())==0) return a->value; }
	auto* o = find_option(name.c_str()); return o&&o->instance>0?o->value:L"";
}

inline std::vector<std::wstring> parser_t::others( const std::string& name ) const
{
	std::vector<std::wstring> v;
	if(name.empty()){ for(auto& a:arguments){ if(a->name.empty()) v.push_back(a->value); } }	// unnamed arguments
	else{ auto* o=find_option(name.c_str()); if(o&&o->instance>1) v=o->others; }				// named options
	return v;
}

inline std::vector<std::wstring> parser_t::get_values( const std::string& name ) const
{
	std::vector<std::wstring> v;	v.emplace_back( get<std::wstring>(name) );
	auto o=others(name);			v.insert(v.end(),o.begin(),o.end());
	return v;
}

inline void parser_t::select_parser_impl( parser_t*& p, int argc, wchar_t** argv )
{
	p->attrib.instance=true; int next=p->attrib.depth+1; if(argc<=next) return;
	const wchar_t* a=trim(argv[next]); if(!a||!*a||_wcsicmp(a,L"help")==0) return;
	std::string arg=wtoa(a); if(arg[0]==L'-') return; // option should not precede the command
	for(auto* c:p->commands)
	{
		if(c->attrib.name!=arg) continue;
		select_parser_impl(p=c,argc,argv); // further traversal to find child with callbacks
	}
}

inline bool parser_t::parse_impl( int argc, const wchar_t** argv )
{
	// configure attributes
	std::vector<argument_t*> required; for( auto* a : arguments ) if(!a->optional) required.push_back(a);
	int	nr=int(required.size());

	// test prerequisite
	const int arg_begin = attrib.depth+1;
	if(argc<=arg_begin)
	{
		if(nr>0) return usage();
		else if(!option_exists()&&!commands.empty()) return usage();
	}

	// start parsing
	option_t* option_needs_subarg=nullptr;
	int r=0; for( int k=arg_begin; k < argc; k++ )
	{
		const wchar_t* a = argv[k];
		
		if(!a[0]) continue;
		else if(option_needs_subarg) // a[0]!=L'-'
		{
			if(a[0]==L'-') return exit( "sub-argument for option '%s' is missing", option_needs_subarg->name() );

			auto* p = option_needs_subarg;
			if(p->instance==0||p->value.empty())	p->value = a;
			else									p->others.push_back(a);
			option_needs_subarg = nullptr; // clear subgarg requirement
		}
		else if(_wcsicmp(a,L"help")==0||_wcsicmp(a,L"-h")==0||_wcsicmp(a,L"--help")==0) // test whether help exists
		{
			attrib.b.help_exists=true;
		}
		else if(a[0]!=L'-')
		{
			if(r>=int(arguments.size())) arguments.push_back(new argument_t()); // increase array to accept excessive arguments
			arguments[r++]->value = a;
		}
		else if(a[1]) // in case of (not too short) option
		{
			bool b_short = a[1]!=L'-';
			std::string name = wtoa(b_short?a+1:a+2); // strip hyphens
			if(name.empty()||!isalpha(name[0])) continue;

			// split by equal
			std::wstring value1;
			const char* eq=strchr(name.c_str(),'=');
			bool eq_exists = name.length()>=2&&eq;
			if(name.length()>=2&&eq)
			{
				value1 = atow(trim(eq+1,"'")); // ignored in combination mode; allows additional '='
				name = name.substr(0,size_t(eq-name.c_str()));
			}
			bool subarg_exists = eq_exists && !value1.empty();

			// combination of short arguments
			if(b_short&&name.length()>1)
			{
				for( auto& n : name )
				{
					char sn[2]={n,0};
					option_t* p=find_option(sn);
					if(!p) return exit( "unrecognized option: -%s in {%s}", sn, name.c_str() );
					if(p->use_subarg) return exit( "-%s: %s is not a simple flag that can be used in combination.", name.c_str(), sn );
					p->instance=1;
				}
				continue;
			}

			// find option by name
			option_t* p=find_option(name.c_str()); if(!p) return exit( "unrecognized option: %s", name.c_str() );
			if( p->use_subarg )	p->instance++;
			else {				p->instance=1; continue; }

			// find inline sub-arguments
			if(!subarg_exists) option_needs_subarg = p;
			else if(!value1.empty())
			{
				if(p->instance==0||p->value.empty())	p->value = value1;
				else									p->others.push_back(value1);
				option_needs_subarg = nullptr;
			}
		}
	}

	// if help exists, show usage
	if( attrib.b.help_exists ){ usage(); return false; }

	// check the value provided for subargument
	for( auto* o : options )
	{
		if(o->empty()||!o->use_subarg) continue;
		if(o->instance==0&&!o->value.empty()) o->instance=1; // apply default argument
		if(o->instance>0&&o->value.empty()) return exit( "option [%s]: subarg not exists.", o->name() );
	}

	// check required arguments
	if(r<nr)
	{
		auto* ar = r<int(required.size())?required[r]:nullptr;
		if(ar) return exit( "argument <%s>: required\n", ar->name.c_str() );
	}

	return true;
}

inline bool parser_t::usage( const char* alt_name )
{
	std::vector<std::pair<std::string,std::string>> req_args, opt_args, opts;

	// print options for multi-line help
	static auto print_option = []( const char* fmt, const char* o, const char* h )
	{
		auto v = explode(trim(h),"\r\n");
		if(v.empty()) fprintf( stdout, fmt, o, h );
		else for(size_t j=0;j<v.size();j++) fprintf( stdout, fmt, j?"":o, v[j].c_str() );
	};

	// process arguments to requried/optional
	for( auto* a : arguments )
	{
		if(a->name.empty()) continue;
		(a->optional?opt_args:req_args).emplace_back(a->name,a->shelp);
	}

	for( auto* o : options )
	{
		if(o->empty()){ opts.emplace_back( "", "" ); continue; } // simple line break

		auto* short_name = o->short_name();
		std::string front = short_name&&short_name[0]?format("-%s ",short_name):"   ";
		auto lv=o->long_names();
		for( size_t j=0, jn=lv.size(); j<jn; j++ ) front+=format("%s%s",j==0?"--":"|",lv[j]);
		if(o->use_subarg) front += format( "=%s", o->subarg_name.c_str() );
		opts.emplace_back( front, o->shelp );

		// add line breaks
		for( int k=0;k<o->break_count;k++ ) opts.emplace_back( "", "" );
	}

	// find the longest front length
	size_t cap=0;
	for( auto& a : req_args )	cap=cap>a.first.length()?cap:a.first.length();
	for( auto& a : opt_args )	cap=cap>a.first.length()?cap:a.first.length();
	for( auto& o : opts )		cap=cap>o.first.length()?cap:o.first.length();
	for( auto* c : commands)	cap=cap>c->attrib.name.length()?cap:c->attrib.name.length();
	if(cap==0&&commands.empty()) return exit("no argument/options found\n");

	// now, prints the results
	std::string module_name = path::module_path().name(false).wtoa();
	fprintf( stdout, "\n%s version %04d-%02d-%02d\n", module_name.c_str(), gx::compiler::year(), gx::compiler::month(), gx::compiler::day() );
	if(!attrib.copyright.empty()) fprintf( stdout, "%s\n", trim(attrib.copyright.c_str()) );
	if(!attrib.header.empty()) fprintf( stdout, "%s\n\n", attrib.header.c_str() );

	fprintf( stdout, "usage: %s", get_command_names() );
	fprintf( stdout, " [help]" );
	if(!commands.empty())
	{
		if(options.empty())	fprintf( stdout, " <command>" );
		else				fprintf( stdout, " [<command>]" );
	}
	if(!options.empty()) fprintf( stdout, " [options...]" );
	for( auto* a : arguments )
	{
		if(a->name.empty()) continue;
		if(a->optional)	fprintf( stdout, " [%s]", a->name.c_str() );
		else			fprintf( stdout, " %s", a->name.c_str() );
	}
	fprintf( stdout, " ...\n\n" );

	std::string sfmt=format(" %%-%ds %%s\n",int(cap+4));
	const char* fmt=sfmt.c_str();

	if(!commands.empty())
	{
		fprintf( stdout, "commands:\n");
		for( auto* c : commands ) print_option( fmt, c->name(), c->attrib.help.c_str() );
		fprintf( stdout, "\n");
	}

	if(!req_args.empty()||!opt_args.empty())
	{
		fprintf( stdout, "arguments:\n");
		for( auto& a : req_args ) print_option( fmt, format(" %s",a.first.c_str()), a.second.c_str() );
		for( auto& a : opt_args ) print_option( fmt, format("[%s]",a.first.c_str()), a.second.c_str() );
		fprintf( stdout, "\n" );
	}

	if(!opts.empty())
	{
		fprintf( stdout, "options:\n");
		for( auto& o : opts ) print_option( fmt, o.first.c_str(), o.second.c_str() );
		fprintf( stdout, "\n" );
	}

	if(!attrib.footer.empty()) fprintf( stdout, "%s\n\n", attrib.footer.c_str() );

	if(!os::console::has_parent()) _wsystem(L"pause");

	return false;
}

inline const wchar_t* parser_t::rebuild_arguments( int argc, wchar_t* argv[] )
{
	if(argc<=1) return L"";
	static std::wstring a; a.clear();
	for( int k=1; k < argc; k++ )
	{
		bool ws=wcschr(argv[k],L' ')||wcschr(argv[k],L'\t');
		if(k>1) a+=L" "; if(ws) a+=L"\""; a += argv[k]; if(ws) a+=L"\"";
	}
	return a.c_str();
}

inline void parser_t::dump()
{
	// retrive things to print
	std::vector<std::pair<std::string,std::string>> args, opts;
	for(auto* a:arguments) if(!a->name.empty()) args.emplace_back( a->name.c_str(), wtoa(a->value.c_str()) );
	std::string ots; for(auto& a:others()) ots+=format( "%s ",wtoa(a.c_str())); args.emplace_back( "others", ots );
	for(auto* o:options)
	{
		std::string n=o->name();
		std::string v;
		if(o->instance==0) v="0";
		else
		{
			v=format( "%d", o->instance );
			if(o->use_subarg)
			{
				v+= format( ": '%s'", wtoa(o->value.c_str()) );
				if(o->instance>1){ for( size_t j=0; j<o->others.size();j++){auto& t=o->others[j]; v+= format(" '%s'",wtoa(t.c_str()));} }
			}
		}
		opts.emplace_back(n,v);
	}

	// find the max length
	size_t cap=0;
	for( auto& a:args ) cap=cap>a.first.size()?cap:a.first.size();
	for( auto& o:opts ) cap=cap>o.first.size()?cap:o.first.size();
	if(cap==0) return void(exit( "cap==0\n"));
	std::string sfmt = format(" %%-%ds   = %%s\n",cap+4); const char* fmt=sfmt.c_str();

	// now, print
	fprintf( stdout, "******************************\n");
	fprintf( stdout, "[arguments]\n");
	for(auto& a:args) fprintf( stdout, fmt, a.first.c_str(), a.second.c_str() );
	fprintf( stdout, "\n[options]\n");
	for(auto& o:opts) fprintf( stdout, fmt, o.first.c_str(), o.second.c_str() );
	fprintf( stdout, "******************************\n\n");
}

//***********************************************
} // namespace argparse

// alias
using argparser = argparse::parser_t;

} // namespace gx
//***********************************************
#endif // __GX_ARGPARSE_H__
