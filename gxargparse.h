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

#if __has_include(<gxut/gxut.h>)
	#include <gxut/gxut.h>
#endif

//***********************************************
namespace gx { namespace argparse {
//***********************************************

struct argument_t
{
	argument_t& add_help( __printf_format_string__ const char* fmt, ... ){ va_list a; va_start(a,fmt); vector<char> buff(vsnprintf(0,0,fmt,a)+1); vsnprintf(&buff[0],buff.size(),fmt,a); va_end(a); shelp=trim(&buff[0],"\n"); return *this; }
	argument_t& set_default( const char* v ){ parsed.value=atow(v); return *this; }
	argument_t& set_optional(){ optional=true; return *this; } 
	
protected:
	string		name;
	string		shelp;
	bool		optional=false;
	struct parsed_t { wstring value; void clear(){ value.clear(); } } parsed;
	
	friend struct parser_t;
	bool value_exists() const { return !parsed.value.empty(); }
	void clear(){ parsed.clear(); } // clear values
};

struct option_t
{
	bool empty() const { return names.empty(); }
	void clear(){ parsed.clear(); } // clear values

	option_t& add_name( const char* name ){ if(name&&name[0]) names.insert(name); return *this; }
	option_t& add_subarg( vector<string> constraints={} ){ subarg_count++; this->constraints.clear(); for( auto& c : constraints ) this->constraints.emplace_back(atow(c.c_str())); return *this; }
	option_t& add_help( __printf_format_string__ const char* fmt, ... ){ va_list a; va_start(a,fmt); vector<char> buff(vsnprintf(0,0,fmt,a)+1); vsnprintf(&buff[0],buff.size(),fmt,a); shelp=trim(&buff[0],"\n"); va_end(a); return *this; }
	option_t& add_break( int count=1 ){ break_count+=count; return* this; }
	option_t& set_default( const char* arg ){ parsed.value=atow(arg); return *this; }
	option_t& set_hidden(){ hidden=true; return *this; } 

protected:
	friend struct parser_t;
	bool exists() const { if(!parsed.instance) return false; return !subarg_count||!parsed.value.empty(); }
	option_t& add_name(){ return *this; } // terminator for template argument recursion
	template<typename... Args> option_t& add_name( const char* name, Args... args ){ if(name&&name[0]) names.insert(name); return add_name(args...); }
	const char* name() const { for( auto& n : names ) if(n.size()==1) return n.c_str(); for( auto& n : names ) if(n.size()>1) return n.c_str(); return ""; }
	const char* short_name() const { for( auto& n : names ) if(n.size()==1) return n.c_str(); return ""; }
	vector<const char*> long_names() const { vector<const char*> v; for( auto& n : names ) if(n.size()>1) v.push_back(n.c_str()); return v; }
	bool is_value_acceptable( const char* v ){ if(constraints.empty()) return true; for( const auto& c:constraints ) if(stricmp(c.c_str(),atow(v))==0) return true; return false; }
	wstring contraints_to_str(){ if(constraints.empty()) return L""; auto s=constraints.front(); for(size_t k=1,kn=constraints.size();k<kn;k++) s+=L","s+constraints[k]; return s; }
	bool push_back( const char* v ){ if(!is_value_acceptable(v)){ printf( "option '%s' not accepts '%s'; use one in {%s}\n", name(), v, wtoa(contraints_to_str().c_str()) ); return false; } if(parsed.instance==0||parsed.value.empty()) parsed.value=atow(v); else parsed.others.push_back(atow(v)); return true; }

	struct lless { bool operator()(const string& a,const string& b)const{return a.size()!=b.size()?a.size()<b.size():stricmp(a.c_str(),b.c_str())<0;}};
	std::set<string,lless>	names;				// multiple names allowed for a single option
	string					shelp;				// help string
	bool					hidden=false;		// hide in usage()
	int						break_count=0;		// post-line breaks
	int						subarg_count=0;		// required subarg counter
	vector<wstring>			constraints;		// value constraints: allow values only in this set

	struct parsed_t
	{
		wstring			value;		// found first values
		vector<wstring>	others;		// additional excessive multiple options
		int				instance=0;	// occurence of option and multiple instances of an option (for subarguments)
		void clear(){ value.clear(); others.clear(); instance=0; }
	} parsed;
};

struct parser_t
{
	using callback_t = bool(*)(parser_t&);

	// ctor/dtor
	parser_t(){ strcpy(attrib.name,exe::name()); }
	virtual ~parser_t(){ for(auto& c:commands)safe_delete(c); for(auto& a:arguments)safe_delete(a); for(auto& o:options)safe_delete(o); }

	// attributes
	inline const char* name() const { return attrib.name; }
	inline parser_t& add_header( __printf_format_string__ const char* fmt, ... ){ va_list a; va_start(a,fmt); int l=vsnprintf(0,0,fmt,a); vector<char> buff(l+1); vsnprintf(&buff[0],l+1llu,fmt,a); attrib.header=trim(&buff[0]); va_end(a); return *this; }
	inline parser_t& add_footer( __printf_format_string__ const char* fmt, ... ){ va_list a; va_start(a,fmt); int l=vsnprintf(0,0,fmt,a); vector<char> buff(l+1); vsnprintf(&buff[0],l+1llu,fmt,a); attrib.footer=trim(&buff[0]); va_end(a); return *this; }
	inline parser_t& add_copyright( const char* author, int since_year ){ attrib.copyright = format( "copyright (c) %d-%d by %s\n", since_year, compiler::year()+1, author ); return *this; }
	inline parser_t& add_help( __printf_format_string__ const char* fmt, ... ){ va_list a; va_start(a,fmt); vector<char> buff(vsnprintf(0,0,fmt,a)+1); vsnprintf(&buff[0],buff.size(),fmt,a); attrib.help=trim(&buff[0],"\n"); va_end(a); return *this; }
	inline parser_t& add_break( int count=1 ){ if(options.empty()) return *this; options.back()->add_break(count); return *this; }

	// query
	inline bool option_exists() const;
	inline bool command_exists( const string& name ) const;
	inline bool callback_exists(){ parser_t* p=this;while(p&&!p->pf_callback) p=p->parent; return p&&p->pf_callback?true:false; }
	inline bool help_exists() const { return parsed.b.help_exists; }
		
	// multi-name query for exists
	inline bool exists() const { return false; }  // terminator for template argument-pack expansion
	template<typename... Args> bool exists( const char* name, Args... args ) const;

	// subcommands
	inline parser_t& select_parser( int argc, wchar_t* argv[] ){ parser_t* p=this; select_parser_impl(p,argc,argv); return *p; }
	inline parser_t& add_command( const char* name ){ commands.emplace_back(new parser_t()); auto* c=commands.back(); c->parent=this; strcpy(c->attrib.name,name); c->attrib.depth=attrib.depth+1; return *c; }
	inline parser_t& set_callback( callback_t cb ){ pf_callback=cb; return *this; }
	inline bool run(){ parser_t* p=this;while(p&&!p->pf_callback) p=p->parent; if(!p||!p->pf_callback){fprintf(stdout,"unable to find callback for %s\n",name());return false;} return p->pf_callback(*p); }
	inline const char* get_command_names() const { string c=this->name(); for( parser_t* p=this->parent; p; p=p->parent ) c = string(p->attrib.name)+" "+c; return __strdup(c.c_str()); }

	// short template functions
	argument_t& add_argument( const char* name ){ arguments.push_back(new argument_t()); auto* a=arguments.back(); a->name=name; return *a; }
	template<typename... Args> option_t& add_option( Args... args ){ options.push_back(new option_t()); auto* o=options.back(); return o->add_name(args...);  }

	// long functions
	void clear(); // clear parsing results
	bool parse( int argc, wchar_t* argv[], bool b_validate=true );
	bool validate();
	bool usage( const char* alt_name=nullptr );
	static const char* rebuild_arguments( int argc, wchar_t* argv[] );
	static const char* rebuild_arguments( int argc, char* argv[] );

	// get<> specializations, and other get functions
	template <class T=string>	inline T get( const string& name ) const;
	template<> inline string	get<string>( const string& name ) const { return wtoa(get<wstring>(name).c_str()); }
	template<> inline wstring	get<wstring>( const string& name ) const;
	template<> inline path		get<path>( const string& name ) const { return get<string>(name).c_str(); }
	template<> inline int		get<int>( const string& name ) const { return atoi(get<string>(name).c_str()); }
	template<> inline uint		get<uint>( const string& name ) const { return uint(atoi(get<string>(name).c_str())); }
	template<> inline float		get<float>( const string& name ) const { return float(atof(get<string>(name).c_str())); }
	template<> inline double	get<double>( const string& name ) const { return atof(get<string>(name).c_str()); }
	
	// get multiple values
	template <class T=string>	vector<T> others( const string& name="" ) const;
	template <>					vector<string> others( const string& name ) const;
	template <>					vector<wstring> others( const string& name ) const;
	template <class T=string>	vector<T> get_values( const string& name ) const;
	template <>					vector<string>	get_values( const string& name ) const;
	template <>					vector<wstring> get_values( const string& name ) const;

	// error handling, debugging
	bool exit( __printf_format_string__ const char* fmt, ... ){ va_list a; va_start(a,fmt); const char* w=vformat(fmt,a); va_end(a); char msg[2048]; snprintf( msg, 2048, "[%s] %s\nUse -h option to see usage.\n", name(), trim(w,"\n") ); fprintf( stdout, msg ); return false; }
	void dump();
	
protected:

	struct attribute_t
	{
		char	name[256];
		string	copyright;
		string	header, footer;
		string	help;		// shorter help for commands
		int		depth=0;	// depth to commands; non-zero means sub-commands
	} attrib;

	struct cmdline_t
	{
		int argc=0;
		wchar_t** argv=nullptr;
		void clear(){ argc=0; argv=nullptr; }
	} cmdline;

	struct parsed_t
	{
		int argument_count=0;
		struct { bool help_exists = false; } b;
		void clear(){ b.help_exists=false; argument_count=0; }
	} parsed;

	parser_t*			parent = nullptr;
	callback_t			pf_callback = nullptr;
	vector<parser_t*>	commands;
	vector<argument_t*>	arguments;
	vector<option_t*>	options;
	bool				b_command_found=false; // this parser exists in arguments

	option_t* find_option( const char* name ) const	{ if(!name||!name[0]) return nullptr; for(auto* o:options) for( auto& n:o->names ) if(n==name) return o; return nullptr; }
	argument_t* find_argument( const char* name ) const { if(!name||!name[0]) return nullptr; for(auto* a:arguments){ if(a->name==name) return a; } return nullptr; }
	void select_parser_impl( parser_t*& p, int argc, wchar_t** argv );
	const int arg_begin() const { return attrib.depth+1; }
	vector<argument_t*> get_required_arguments(){ vector<argument_t*> v; for(auto* a:arguments ) if(!a->optional) v.emplace_back(a); return v; }
};

template <typename... Args> bool parser_t::exists( const char* name, Args... args ) const
{
	if(!name) return exists(args...);
	if(*name=='h'){ if(!name[1]||strcmp(name+1,"elp")==0)	return parsed.b.help_exists; }
	if(auto* o=find_option(name); o&&o->parsed.instance>0)	return true;
	if(auto* a=find_argument(name); a&&a->value_exists())	return true;
	return exists(args...);
}

inline bool parser_t::option_exists() const
{
	for( auto* o : options ) if(!o->empty()&&o->parsed.instance>0) return true;
	return false;
}

template<> inline wstring parser_t::get<wstring>( const string& name ) const
{
	auto* a=find_argument(name.c_str()); if(a) return a->parsed.value;
	auto* o=find_option(name.c_str()); return o&&o->parsed.instance>0?o->parsed.value:L"";
}

template<> inline vector<wstring> parser_t::others<wstring>( const string& name ) const
{
	vector<wstring> v;
	if(name.empty()){ for(auto& a:arguments){ if(a->name.empty()) v.push_back(a->parsed.value); } }	// unnamed arguments
	else{ auto* o=find_option(name.c_str()); if(o&&o->parsed.instance>1) v=o->parsed.others; }		// named options
	return v;
}

template<> inline vector<string> parser_t::others<string>( const string& name ) const
{
	vector<string> v;
	if(name.empty()){ for(auto& a:arguments){ if(a->name.empty()) v.push_back(wtoa(a->parsed.value.c_str())); } } // unnamed arguments
	else{ auto* o=find_option(name.c_str()); if(o&&o->parsed.instance>1){ v.clear(); for(auto& t:o->parsed.others) v.emplace_back(wtoa(t.c_str())); }} // named options
	return v;
}

template<> inline vector<wstring> parser_t::get_values( const string& name ) const
{
	vector<wstring> v = { get<wstring>(name) };
	auto o=others<wstring>(name); v.insert(v.end(),o.begin(),o.end());
	return v;
}

template<> inline vector<string> parser_t::get_values( const string& name ) const
{
	vector<string> v = { get<string>(name) };
	auto o=others<string>(name); v.insert(v.end(),o.begin(),o.end());
	return v;
}

inline bool parser_t::command_exists( const string& name ) const
{
	for(auto* c:commands)
	{
		if(strcmp(c->attrib.name,name.c_str())!=0) continue;
		if(c->b_command_found) return true;
	}
	return false;
}

inline void parser_t::select_parser_impl( parser_t*& p, int argc, wchar_t** argv )
{
	p->b_command_found = true;
	int next=p->attrib.depth+1; if(argc<=next) return;
	
	const wchar_t* a=trim(argv[next]); if(!a||!*a||wcsicmp(a,L"help")==0) return;
	string arg=wtoa(a); if(arg[0]==L'-') return; // option should not precede the command
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

			if(!option_needs_subarg->push_back(wtoa(a))) return false;
			option_needs_subarg = nullptr; // clear subgarg requirement
		}
		else if(wcsicmp(a,L"help")==0||wcsicmp(a,L"-h")==0||wcsicmp(a,L"--help")==0) // test whether help exists
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
			string name = wtoa(b_short?a+1:a+2); // strip hyphens
			if(name.empty()||!isalpha(name[0])) continue;

			// split by equal
			string value1;
			const char* eq=strchr(name.c_str(),'=');
			bool eq_exists = name.length()>=2&&eq;
			if(name.length()>=2&&eq)
			{
				value1 = trim(eq+1,"'"); // ignored in combination mode; allows additional '='
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

	if( parsed.argument_count < required_count )
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
	vector<std::pair<string,string>> req_args, opt_args, opts;

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
		string front = short_name&&short_name[0]?format("-%s ",short_name):"   ";
		auto lv=o->long_names();
		for( size_t j=0, jn=lv.size(); j<jn; j++ ) front+=format("%s%s",j==0?"--":"|",lv[j]);
		if(o->subarg_count) front += string("=")+(o->constraints.empty()?"*":string("{")+wtoa(o->contraints_to_str().c_str())+"}");
		opts.emplace_back( front, o->shelp );

		// add line breaks
		for( int k=0;k<o->break_count;k++ ) opts.emplace_back( "", "" );
	}

	// find the longest front length
	size_t cap=0;
	for( auto& a : req_args )	cap=cap>a.first.length()?cap:a.first.length();
	for( auto& a : opt_args )	cap=cap>a.first.length()?cap:a.first.length();
	for( auto& o : opts )		cap=cap>o.first.length()?cap:o.first.length();
	for( auto* c : commands)	cap=cap>strlen(c->attrib.name)?cap:strlen(c->attrib.name);
	if(cap==0&&commands.empty()) return exit("no argument/options found\n");

	// now, prints the results
	fprintf( stdout, "\n%s version %04d-%02d-%02d\n", attrib.name, compiler::year(), compiler::month(), compiler::day() );
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

	string sfmt=format(" %%-%ds %%s\n",int(cap+4));
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

#ifdef __GX_OS_H__
	if(!os::console::has_parent()) system("pause");
#endif

	return false;
}

inline const char* parser_t::rebuild_arguments( int argc, wchar_t* argv[] )
{
	if(argc<=1) return "";
	static wstring a; a.clear();
	for( int k=1; k < argc; k++ )
	{
		bool ws=wcschr(argv[k],L' ')||wcschr(argv[k],L'\t');
		if(k>1) a+=L" "; if(ws) a+=L"\""; a += argv[k]; if(ws) a+=L"\"";
	}
	return wtoa(a.c_str());
}

inline const char* parser_t::rebuild_arguments( int argc, char* argv[] )
{
	if(argc<=1) return "";
	static string a; a.clear();
	for( int k=1; k < argc; k++ )
	{
		bool ws=strpbrk(argv[k]," \t")!=nullptr;
		if(k>1) a+=" "; if(ws) a+="\""; a += argv[k]; if(ws) a+="\"";
	}
	return a.c_str();
}

inline void parser_t::dump()
{
	// retrive things to print
	vector<std::pair<string,wstring>> args, opts;
	for(auto* a:arguments) if(!a->name.empty()) args.emplace_back( a->name, a->parsed.value );
	wstring ots; for(auto& a:others()) ots+=format( L"%s ",a.c_str()); args.emplace_back( "others", ots );
	for(auto* o:options)
	{
		string n=o->name();
		wstring v;
		if(o->parsed.instance==0) v=L"0";
		else
		{
			v=format( L"%d", o->parsed.instance );
			if(o->subarg_count)
			{
				v+= format( L": '%s'", o->parsed.value.c_str() );
				if(o->parsed.instance>1){ for( size_t j=0; j<o->parsed.others.size();j++){auto& t=o->parsed.others[j]; v+= format(L" '%s'",t.c_str());} }
			}
		}
		opts.emplace_back(n,v);
	}

	// find the max length
	size_t cap=0;
	for( auto& a:args ) cap=cap>a.first.size()?cap:a.first.size();
	for( auto& o:opts ) cap=cap>o.first.size()?cap:o.first.size();
	if(cap==0) return void(exit( "cap==0\n"));
	string sfmt = format(" %%-%zds   = %%s\n",cap+4); const char* fmt=sfmt.c_str();

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
