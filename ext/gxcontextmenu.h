#pragma once

#if __has_include(<gxut/gxos.h>)
	#include <gxut/gxos.h>
#else __has_include("../gxos.h")
	#include "../gxos.h"
#endif

using std::string;
using std::vector;

//*************************************
namespace ctx {
//*************************************

struct registry_t
{
	string	name;
	string	cmd;
	string	icon;
	
	static path exe_path(){ return path(exe::path()).replace_extension("exe"); }

	template <HKEY root=HKEY_CLASSES_ROOT> bool add( string subkey, string name, string value );
	template <HKEY root=HKEY_CLASSES_ROOT> bool add_shell( string subkey ){ return add_shell<root>(vector<string>{subkey}); }
	template <HKEY root=HKEY_CLASSES_ROOT> bool add_shell( vector<string> subkeys );
	template <HKEY root=HKEY_CLASSES_ROOT> bool add_shell_office( const char* prefix, int end, int begin=1 );
	bool install( bool b_dry=false );

protected:
	string buffer = "Windows Registry Editor Version 5.00\n\n";
};

template <HKEY root>
inline bool registry_t::add( string subkey, string name, string value )
{
	os::reg::key_t<root> i(str_replace(subkey.c_str(),"/","\\")); if(!i.open()) return false;
	buffer += std::string(wtoa(i.c_str()))+"\n";
	buffer += format( "\"%s\"=\"%s\"\n", name.c_str(), str_escape(value.c_str()) );
	buffer += format( "\n");
	return true;
}

template <HKEY root>
inline bool registry_t::add_shell( vector<string> subkeys )
{
	for( auto s : subkeys )
	{
		os::reg::key_t<root> r( s.c_str()); if(!r.open()) continue;

		// register icon first
		os::reg::key_t<root> i( "%s/shell/%s", s.c_str(), name.c_str() );
		buffer += std::string(wtoa(i.c_str()))+"\n";
		buffer += format( "\"Icon\"=\"%s\"\n", str_escape(icon.c_str()) );
		buffer += format( "\n");

		// register cmd
		os::reg::key_t<root> c( "%s/shell/%s/command", s.c_str(), name.c_str() );
		buffer += std::string(wtoa(c.c_str()))+"\n";
		buffer += format( "@=\"%s\"\n", str_escape(cmd.c_str()) );
		buffer += format( "\n");
	}

	return true;
}

template <HKEY root>
inline bool registry_t::add_shell_office( const char* prefix, int end, int begin )
{
	string cls = root==HKEY_CLASSES_ROOT?"":root==HKEY_LOCAL_MACHINE?"SOFTWARE\\Classes\\":"";
	vector<string> subkeys = { format("%s%s",cls.c_str(),prefix ) };
	for( int k=begin, kn=end+1; k<kn; k++ ) subkeys.emplace_back( format("%s%s.%d",cls.c_str(),prefix,k) );
	return add_shell<root>( subkeys );
}

inline bool registry_t::install( bool b_dry )
{
	path reg_path = path::temp( true ) + exe_path().name() + ".reg";
	FILE* fp = reg_path.fopen("w,ccs=utf-16le"); if(!fp){ printf("unable to open %s\n", reg_path.slash()); return false; }
	fputws( atow(buffer.c_str()), fp );
	fclose(fp);

	if(b_dry)	printf( "%s\n", trim(buffer.c_str()) ) ;
	else {		printf( "%s\n", reg_path.slash() ); reg_path.open(); }

	return true;
}

//*************************************
} // end namespace reg
//*************************************
