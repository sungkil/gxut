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
	argument_t& set_default( const char* v ){ parsed.value=atow(v); return *this; }
	argument_t& set_default( const wchar_t* v ){ parsed.value=v; return *this; }
	argument_t& set_optional(){ optional=true; return *this; } 
	
protected:
	std::string		name;
	std::string		shelp;
	bool			optional=false;
	struct parsed_t { std::wstring value; void clear(){ value.clear(); } } parsed;
	
	friend struct parser_t;
	bool value_exists() const { return !parsed.value.empty(); }
	void clear(){ parsed.clear(); } // clear values
};

struct option_t
{
	bool empty() const { return names.empty(); }
	void clear(){ parsed.clear(); } // clear values

	option_t& add_subarg( std::vector<std::string> constraints={} ){ subarg_count++; if(!constraints.empty()) this->constraints=constraints; return *this; }
	option_t& add_help( const char* fmt, ... ){ va_list a; va_start(a,fmt); std::vector<char> buff(_vscprintf(fmt,a)+1); vsprintf_s(&buff[0],buff.size(),fmt,a); shelp=trim(&buff[0],"\n"); va_end(a); return *this; }
	option_t& add_break( int count=1 ){ break_count+=count; return* this; }
	option_t& set_default( const char* arg ){ parsed.value=atow(arg); return *this; }
	option_t& set_default( const wchar_t* arg ){ parsed.value=arg; return *this; }
	option_t& set_hidden(){ hidden=true; return *this; } 

protected:
	friend struct parser_t;
	bool exists() const { if(!parsed.instance) return false; return !subarg_count||!parsed.value.empty(); }
	template<typename... Args> option_t& add_name( const char* name, Args... args ){ if(name&&name[0]) names.insert(name); return add_name(args...); }
	option_t& add_name( const char* name ){ if(name&&name[0]) names.insert(name); return *this; }
	const char* name() const { for( auto& n : names ) if(n.size()==1) return n.c_str(); for( auto& n : names ) if(n.size()>1) return n.c_str(); return ""; }
	const char* short_name() const { for( auto& n : names ) if(n.size()==1) return n.c_str(); return ""; }
	std::vector<const char*> long_names() const { std::vector<const char*> v; for( auto& n : names ) if(n.size()>1) v.push_back(n.c_str()); return v; }
	bool is_value_acceptable( const wchar_t* v ){ if(constraints.empty()) return true; std::string s=wtoa(v); for( const auto& c:constraints ) if(_stricmp(c.c_str(),s.c_str())==0) return true; return false; }
	std::string contraints_to_str(){ if(constraints.empty()) return ""; std::string s=constraints.front(); for(size_t k=1,kn=constraints.size();k<kn;k++) s+=std::string(",")+constraints[k]; return s; }
	bool push_back( const wchar_t* v ){ if(!is_value_acceptable(v)){ printf( "option '%s' not accepts '%s'; use one in {%s}\n", name(), wtoa(v), contraints_to_str().c_str() ); return false; } if(parsed.instance==0||parsed.value.empty()) parsed.value=v; else parsed.others.push_back(v); return true; }

	struct lless { bool operator()(const std::string& a,const std::string& b)const{return a.size()!=b.size()?a.size()<b.size():_stricmp(a.c_str(),b.c_str())<0;}};
	std::set<std::string,lless>	names;				// multiple names allowed for a single option
	std::string					shelp;				// help string
	bool						hidden=false;		// hide in usage()
	int							break_count=0;		// post-line breaks
	int							subarg_count=0;		// required subarg counter
	std::vector<std::string>	constraints;		// value constraints: allow values only in this set

	struct parsed_t
	{
		std::wstring				value;				// found first values
		std::vector<std::wstring>	others;				// additional excessive multiple options
		int							instance=0;			// occurence of option and multiple instances of an option (for subarguments)
		void clear(){ value.clear(); others.clear(); instance=0; }
	} parsed;
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
	inline void add_break( int count=1 ){ if(options.empty()) return; options.back()->add_break(count); }

	// query
	inline bool option_exists() const;
	inline bool exists( const std::string& name ) const;
	inline bool command_exists( const std::string& name ) const;
	inline bool callback_exists(){ parser_t* p=this;while(p&&!p->pf_callback) p=p->parent; return p&&p->pf_callback?true:false; }
	inline bool help_exists() const { return parsed.b.help_exists; }

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
	void clear(); // clear parsing results
	bool parse( int argc, wchar_t* argv[], bool b_validate=true );
	bool validate();
	bool usage( const char* alt_name=nullptr );
	static const wchar_t* rebuild_arguments( int argc, wchar_t* argv[] );

	// get<> specializations, and other get functions
	template <class T=std::string>	inline T get( const std::string& name ) const;
	template<> inline std::wstring	get<std::wstring>( const std::string& name ) const;
	template<> inline std::string	get<std::string>( const std::string& name ) const { return wtoa(get<std::wstring>(name).c_str()); }
	template<> inline path			get<path>( const std::string& name ) const { return get<std::wstring>(name).c_str(); }
	template<> inline int			get<int>( const std::string& name ) const { return _wtoi(get<std::wstring>(name).c_str()); }
	template<> inline uint			get<uint>( const std::string& name ) const { return uint(_wtoi(get<std::wstring>(name).c_str())); }
	template<> inline float			get<float>( const std::string& name ) const { return float(_wtof(get<std::wstring>(name).c_str())); }
	template<> inline double		get<double>( const std::string& name ) const { return _wtof(get<std::wstring>(name).c_str()); }
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
	} attrib;

	struct cmdline_t
	{
		int argc=0;
		wchar_t** argv=nullptr;
		void clear(){ argc=0; argv=nullptr; }
	} cmdline;

	struct parsed_t
	{
		bool		instance=false;
		int			argument_count=0;	// parsed argument count
		struct { bool help_exists = false; } b;
		void clear(){ b.help_exists=false; argument_count=0; instance=false; }
	} parsed;

	parser_t*					parent = nullptr;
	callback_t					pf_callback = nullptr;
	std::vector<parser_t*>		commands;
	std::vector<argument_t*>	arguments;
	std::vector<option_t*>		options;

	option_t* find_option( const char* name ) const	{ if(!name||!name[0]) return nullptr; for( auto* o : options ) for( auto& n : o->names ) if(strcmp(name,n.c_str())==0) return o; return nullptr; }
	void select_parser_impl( parser_t*& p, int argc, wchar_t** argv );

	const int arg_begin() const { return attrib.depth+1; }
	std::vector<argument_t*> get_required_arguments(){ std::vector<argument_t*> v; for(auto* a:arguments ) if(!a->optional) v.emplace_back(a); return v; }
};

inline bool parser_t::option_exists() const
{
	for( auto* o : options ) if(!o->empty()&&o->parsed.instance>0) return true;
	return false;
}

inline bool parser_t::exists( const std::string& name ) const
{
	if(name=="h"||name=="help") return parsed.b.help_exists;
	auto* o = find_option(name.c_str()); if(o) return o->parsed.instance>0;
	for( auto* a : arguments ){ if(strcmp(a->name.c_str(),name.c_str())==0) return a->value_exists(); }
	return false;
}

inline bool parser_t::command_exists( const std::string& name ) const
{
	for(auto* c:commands)
	{
		if(strcmp(c->attrib.name.c_str(),name.c_str())!=0) continue;
		if(c->parsed.instance) return true;
	}
	return false;
}

template<> inline std::wstring parser_t::get<std::wstring>( const std::string& name ) const
{
	for( auto* a : arguments ){ if(strcmp(a->name.c_str(),name.c_str())==0) return a->parsed.value; }
	auto* o = find_option(name.c_str()); return o&&o->parsed.instance>0?o->parsed.value:L"";
}

inline std::vector<std::wstring> parser_t::others( const std::string& name ) const
{
	std::vector<std::wstring> v;
	if(name.empty()){ for(auto& a:arguments){ if(a->name.empty()) v.push_back(a->parsed.value); } }	// unnamed arguments
	else{ auto* o=find_option(name.c_str()); if(o&&o->parsed.instance>1) v=o->parsed.others; }				// named options
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
	p->parsed.instance=true; int next=p->attrib.depth+1; if(argc<=next) return;
	const wchar_t* a=trim(argv[next]); if(!a||!*a||_wcsicmp(a,L"help")==0) return;
	std::string arg=wtoa(a); if(arg[0]==L'-') return; // option should not precede the command
	for(auto* c:p->commands)
	{
		if(c->attrib.name!=arg) continue;
		select_parser_impl(p=c,argc,argv); // further traversal to find child with callbacks
	}
}

inline void parser_t::clear()
{
	// parser attributes and clear options
	cmdline.clear();
	parsed.clear();

	for( auto& o : options ) o->clear();
	for( auto it=arguments.rbegin(); it!=arguments.rend();  )
	{
		if(!(*it)->name.empty()){ (*it)->clear(); it++; }
		else { safe_delete(*it); it=decltype(it)(arguments.erase( std::next(it).base())); }
	}
}

inline bool parser_t::parse( int argc, wchar_t* argv[], bool b_validate )
{
	// clear values
	clear();

	cmdline.argc = argc;
	cmdline.argv = argv;

	// configure attributes
	auto required = get_required_arguments();

	// start parsing
	option_t* option_needs_subarg=nullptr;
	for( int k=arg_begin(); k < argc; k++ )
	{
		const wchar_t* a = argv[k];
		
		if(!a[0]) continue;
		else if(option_needs_subarg) // a[0]!=L'-'
		{
			if(a[0]==L'-') return exit( "sub-argument for option '%s' is missing", option_needs_subarg->name() );

			if(!option_needs_subarg->push_back(a)) return false;
			option_needs_subarg = nullptr; // clear subgarg requirement
		}
		else if(_wcsicmp(a,L"help")==0||_wcsicmp(a,L"-h")==0||_wcsicmp(a,L"--help")==0) // test whether help exists
		{
			parsed.b.help_exists=true;
		}
		else if(a[0]!=L'-'||wcscmp(a,L"-")==0) // accept stdin as argument
		{
			if(parsed.argument_count>=int(arguments.size())) arguments.push_back(new argument_t()); // increase array to accept excessive arguments
			arguments[parsed.argument_count++]->parsed.value = a;
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
					if(n=='h'){ parsed.b.help_exists=true; continue; }
					char sn[2]={n,0}; option_t* p=find_option(sn);
					if(!p) return exit( "unrecognized option: -%s in {%s}", sn, name.c_str() );
					if(p->subarg_count) return exit( "-%s: %s is not a simple flag that can be used in combination.", name.c_str(), sn );
					p->parsed.instance=1;
				}
				continue;
			}

			// find option by name
			option_t* p=find_option(name.c_str()); if(!p) return exit( "unrecognized option: %s%s", name.size()>1?"--":"-", name.c_str() );
			if( p->subarg_count )	p->parsed.instance++;
			else {					p->parsed.instance=1; continue; }

			// find inline sub-arguments
			if(!subarg_exists) option_needs_subarg = p;
			else if(!value1.empty())
			{
				if(!p->push_back(value1.c_str())) return false;
				option_needs_subarg = nullptr;
			}
		}
	}

	// validate
	return !b_validate || validate();
}

inline bool parser_t::validate()
{
	auto required = get_required_arguments();
	int	required_count = int(required.size());

	if(cmdline.argc<=arg_begin())
	{
		if(required_count>0) return usage();
		else if(!option_exists()&&!commands.empty()) return usage();
	}

	// if help exists, show usage
	if( parsed.b.help_exists ){ usage(); return false; }

	if(parsed.argument_count<required_count)
		return exit( "argument <%s>: required\n", required[parsed.argument_count]->name.c_str() );

	// check the value provided for subargument
	for( auto* o : options )
	{
		if(o->empty()||!o->subarg_count) continue;
		if(o->parsed.instance==0&&!o->parsed.value.empty()) o->parsed.instance=1; // apply default argument
		if(o->parsed.instance>0&&o->parsed.value.empty()) return exit( "option [%s]: subarg not exists.", o->name() );
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
		if(o->hidden) continue;

		auto* short_name = o->short_name();
		std::string front = short_name&&short_name[0]?format("-%s ",short_name):"   ";
		auto lv=o->long_names();
		for( size_t j=0, jn=lv.size(); j<jn; j++ ) front+=format("%s%s",j==0?"--":"|",lv[j]);
		if(o->subarg_count) front += std::string("=")+(o->constraints.empty()?"*":std::string("{")+o->contraints_to_str()+"}");
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
	fprintf( stdout, " ...\n" );

	std::string sfmt=format(" %%-%ds %%s\n",int(cap+4));
	const char* fmt=sfmt.c_str();

	if(!commands.empty())
	{
		fprintf( stdout, "\ncommands:\n");
		for( auto* c : commands ) print_option( fmt, c->name(), c->attrib.help.c_str() );
	}

	if(!req_args.empty()||!opt_args.empty())
	{
		fprintf( stdout, "\narguments:\n");
		for( auto& a : req_args ) print_option( fmt, format(" %s",a.first.c_str()), a.second.c_str() );
		for( auto& a : opt_args ) print_option( fmt, format("[%s]",a.first.c_str()), a.second.c_str() );
	}

	if(!opts.empty())
	{
		fprintf( stdout, "\noptions:\n");
		for( auto& o : opts ) print_option( fmt, o.first.c_str(), o.second.c_str() );
	}

	if(!attrib.footer.empty())
	{
		fprintf( stdout, "\n%s\n", attrib.footer.c_str() );
	}

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
	for(auto* a:arguments) if(!a->name.empty()) args.emplace_back( a->name.c_str(), wtoa(a->parsed.value.c_str()) );
	std::string ots; for(auto& a:others()) ots+=format( "%s ",wtoa(a.c_str())); args.emplace_back( "others", ots );
	for(auto* o:options)
	{
		std::string n=o->name();
		std::string v;
		if(o->parsed.instance==0) v="0";
		else
		{
			v=format( "%d", o->parsed.instance );
			if(o->subarg_count)
			{
				v+= format( ": '%s'", wtoa(o->parsed.value.c_str()) );
				if(o->parsed.instance>1){ for( size_t j=0; j<o->parsed.others.size();j++){auto& t=o->parsed.others[j]; v+= format(" '%s'",wtoa(t.c_str()));} }
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
