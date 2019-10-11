//*******************************************************************
// Copyright 2011-2020 Sungkil Lee
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
	#if __has_include("gxstring.h")
		#include "gxstring.h"
	#elif __has_include(<gxstring.h>)
		#include <gxstring.h>
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
	argument_t& help( const char* fmt, ... ){ va_list a; va_start(a,fmt); std::vector<char> buff(_vscprintf(fmt,a)+1); vsprintf_s(&buff[0],buff.size(),fmt,a); va_end(a); shelp=trim(&buff[0],"\n"); return *this; }
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
	struct size_less{ bool operator()(const std::string& a,const std::string& b)const{ if(a.length()!=b.length()) return a.length()<b.length(); else return _stricmp(a.c_str(),b.c_str())<0;}};

	option_t& subarg( const char* subarg_name ){ use_subarg=true; this->subarg_name=subarg_name; return *this; }
	option_t& help( const char* fmt, ... ){ va_list a; va_start(a,fmt); std::vector<char> buff(_vscprintf(fmt,a)+1); vsprintf_s(&buff[0],buff.size(),fmt,a); shelp=trim(&buff[0],"\n"); va_end(a); return *this; }
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

	std::set<std::string,size_less>	names;				// multiple names allowed for a single option
	std::wstring					value;				// found values
	std::vector<std::wstring>		others;				// additional excessive multiple options
	std::string						shelp;				// help string
	std::string						subarg_name;		// name of sub-argument
	bool							use_subarg=false;	// use a sub-argument
	int								instance=0;			// allow multiple instances of an option
};

struct parser_t
{
	std::vector<argument_t>	arguments;
	std::vector<option_t>	options;
	bool					b_help_exists = false;

	// attributes
	inline const char* name() const { static const std::string n=path::module_path().name(false).wtoa(); return n.c_str(); }
	inline const wchar_t* wname() const { static const path n=path::module_path().name(false); return n.c_str(); }
	inline void header( const char* fmt, ... ){ va_list a; va_start(a,fmt); std::vector<char> buff(_vscprintf(fmt,a)+1); vsprintf_s(&buff[0],buff.size(),fmt,a); sheader=trim(&buff[0]); va_end(a); }
	inline void footer( const char* fmt, ... ){ va_list a; va_start(a,fmt); std::vector<char> buff(_vscprintf(fmt,a)+1); vsprintf_s(&buff[0],buff.size(),fmt,a); sfooter=trim(&buff[0]); va_end(a); }
	inline void copyright( const char* author, int since_year ){ this->author=author; this->since_year=since_year; }
	inline bool exists( const std::string& name ) const;
	inline bool help_exists() const { return b_help_exists; }

	// short template functions
	argument_t& add_argument( const char* name ){ arguments.push_back(argument_t()); auto& a=arguments.back(); a.name=name; return a; }
	template<typename... Args> option_t& add_option( Args... args ){ options.push_back(option_t()); auto& o=options.back(); return o.add_name(args...);  }

	// long functions
	bool parse( int argc, wchar_t* argv[] ){ return parse(argc,(const wchar_t**)argv); }
	bool parse( int argc, const wchar_t** argv );
	bool usage( const char* alt_name=nullptr );

	// get<> specializations, and other get functions
	template <class T=std::string>	inline T get( const std::string& name ) const;
	template<> inline std::wstring	get<std::wstring>( const std::string& name ) const;
	template<> inline std::string	get<std::string>( const std::string& name ) const { return wtoa(get<std::wstring>(name).c_str()); }
	template<> inline path			get<path>( const std::string& name ) const { return get<std::wstring>(name).c_str(); }
	template<> inline int			get<int>( const std::string& name ) const { return _wtoi(get<std::wstring>(name).c_str()); }
	template<> inline uint			get<uint>( const std::string& name ) const { return uint(_wtoi(get<std::wstring>(name).c_str())); }
	std::vector<std::wstring>		others( const std::string& name="" ) const;

	// error handling, debugging
	bool exit( const char* fmt, ... ){ va_list a; va_start(a,fmt); const char* w=vformat(fmt,a); va_end(a); fprintf( stdout, "[%s] %s\nUse -h option to see usage.\n", name(), trim(w,"\n") ); return false; }
	bool exit( const wchar_t* fmt, ... ){ va_list a; va_start(a,fmt); const wchar_t* w=vformat(fmt,a); va_end(a); fprintf( stdout, "[%s] %s\nUse -h option to see usage.\n", name(), trim(wtoa(w),"\n") ); return false; }
	void dump();

protected:
	option_t* find_option( const char* name ) const	{ if(!name||!name[0]) return nullptr; for( auto& o : options ) for( auto& n : o.names ) if(_stricmp(name,n.c_str())==0) return const_cast<option_t*>(&o); return nullptr; }

	std::string sheader, author, sfooter;
	int			since_year=0;
};

inline bool parser_t::exists( const std::string& name ) const
{
	if(name=="h"||name=="help") return b_help_exists;
	auto* o = find_option(name.c_str()); if(o) return o->instance>0;
	for( auto& a : arguments ){ if(_stricmp(a.name.c_str(),name.c_str())==0) return a.value_exists(); }
	return false;
}

template<> inline std::wstring parser_t::get<std::wstring>( const std::string& name ) const
{
	for( auto& a : arguments ){ if(_stricmp(a.name.c_str(),name.c_str())==0) return a.value; }
	auto* o = find_option(name.c_str()); return o&&o->instance>0?o->value:L"";
}

inline std::vector<std::wstring> parser_t::others( const std::string& name ) const
{
	std::vector<std::wstring> v;
	if(name.empty()) for(auto& a:arguments) if(a.name.empty()) v.push_back(a.value);	// unnamed arguments
	auto* o = find_option(name.c_str()); if(o&&o->instance>1) v=o->others;				// optioan others
	return v;
}

inline bool parser_t::parse( int argc, const wchar_t** argv )
{
	// configure attributes
	std::vector<argument_t*> required; for( auto& a : arguments ) if(!a.optional) required.push_back(&a);
	int	 nr=int(required.size());

	// test prerequisite
	if(nr>0&&argc<2) return usage();

	// start parsing
	int r=0; for( int k=1; k<argc; k++ )
	{
		const wchar_t* a = argv[k]; if(!a[0]) continue;
		if(a[0]!=L'-'){ if(r>=nr) arguments.push_back(argument_t()); arguments[r++].value = a; continue; } // increase array to accept excessive arguments
		if(_wcsicmp(a,L"-h")==0||_wcsicmp(a,L"--help")==0){ b_help_exists=true; continue; } // test whether help exists
		if(!a[1]) continue;	// skip too short options

		bool b_short = a[1]!=L'-';
		std::string name = wtoa(b_short?a+1:a+2); // strip hyphens
		if(name.empty()||!isalpha(name[0])) continue;

		// split by equal
		std::wstring new_value;
		const char* eq=strchr(name.c_str(),'=');
		if(name.length()>=2&&eq)
		{
			new_value = atow(trim(eq+1,"'")); // ignored in combination mode; allows additional '='
			name = name.substr(0,size_t(eq-name.c_str()));
		}

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
		if(!p->use_subarg){ p->instance=1; continue; }
		else p->instance++;

		// find non-inline sub-arguments
		if(new_value.empty()){ while(k<argc-1){ if(argv[k+1][0]!=L'-') new_value=argv[++k]; break; } }
		if(new_value.empty()) continue; // skip still empty argument

		if(0==p->instance||p->value.empty())	p->value = new_value;
		else									p->others.push_back(new_value);
	}

	// if help show usage
	if( b_help_exists ){ usage(); return false; }

	// check the value provided for subargument
	for( auto& o : options )
	{
		if(!o.use_subarg) continue;
		if(o.instance==0&&!o.value.empty()) o.instance=1; // apply default argument
		if(o.instance>0&&o.value.empty()) return exit( "option [%s]: subarg not exists.", o.name() );
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
		for(size_t j=0;j<v.size();j++) fprintf( stdout, fmt, j?"":o, v[j].c_str() );
	};

	// process arguments to requried/optional
	for( auto& a : arguments )
	{
		if(a.name.empty()) continue;
		(a.optional?opt_args:req_args).emplace_back(a.name,a.shelp);
	}

	opts.emplace_back( "-h --help", "show usage help" );
	for( auto& o : options )
	{
		auto* short_name = o.short_name();
		std::string front = short_name&&short_name[0]?format("-%s ",short_name):"   ";
		auto lv=o.long_names();
		for( size_t j=0, jn=lv.size(); j<jn; j++ ) front+=format("%s%s",j==0?"--":"|",lv[j]);
		if(o.use_subarg) front += format( "=%s", o.subarg_name.c_str() );
		opts.emplace_back( front, o.shelp );
	}

	// find the longest front length
	size_t cap=0;
	for( auto& a : req_args )	cap=cap>a.first.length()?cap:a.first.length();
	for( auto& a : opt_args )	cap=cap>a.first.length()?cap:a.first.length();
	for( auto& o : opts )		cap=cap>o.first.length()?cap:o.first.length();
	if(cap==0) return exit("no argument/options found\n");

	std::string sfmt=format(" %%-%ds %%s\n",int(cap+4));
	const char* fmt=sfmt.c_str();

	// now, prints the results
	fprintf( stdout, "\n%s version %04d-%02d-%02d\n", name(), gx::compiler::year(), gx::compiler::month(), gx::compiler::day() );
	if(!author.empty()&&since_year>0) fprintf( stdout, "copyright (c) %d-%d by %s\n", since_year, gx::compiler::year(), author.c_str());
	if(!sheader.empty()) fprintf( stdout, "\n%s\n\n", sheader.c_str() );

	fprintf( stdout, "usage: %s", name() );
	if(!options.empty()) fprintf( stdout, " [options...]" ); else fprintf( stdout, " [-h|--help]" );
	for( auto& a : arguments ) if(!a.name.empty()) fprintf( stdout, " %s%s%s", a.optional?"[":"",a.name.c_str(),a.optional?"]":"" );
	fprintf( stdout, " ...\n\n" );

	if(!req_args.empty()||!opt_args.empty())
	{
		fprintf( stdout, "arguments:\n");
		for( auto& a : req_args ) print_option( fmt, a.first.c_str(), a.second.c_str() );
		for( auto& a : opt_args ) print_option( fmt, format("[%s]",a.first.c_str()), a.second.c_str() );
		fprintf( stdout, "\n" );
	}

	if(!opts.empty())
	{
		fprintf( stdout, "options:\n");
		for( auto& o : opts ) print_option( fmt, o.first.c_str(), o.second.c_str() );
		fprintf( stdout, "\n" );
	}

	if(!sfooter.empty()) fprintf( stdout, "%s\n\n", sfooter.c_str() );

	return false;
}

inline void parser_t::dump()
{
	// retrive things to print
	std::vector<std::pair<std::string,std::string>> args, opts;
	for(auto& a:arguments) if(!a.name.empty()) args.emplace_back( a.name.c_str(), wtoa(a.value.c_str()) );
	std::string ots; for(auto& a:others()) ots+=format( "%s ",wtoa(a.c_str())); args.emplace_back( "others", ots );
	for(auto& o:options)
	{
		std::string n=o.name();
		std::string v=format( "%d: %s", o.instance, wtoa(o.value.c_str()) );
		if(o.instance>1){ v+= " ("; for( size_t j=0; j<o.others.size();j++){auto& t=o.others[j]; v+= format("%s%s",j>0?" ":"",wtoa(t.c_str()));} v+=")"; }
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
