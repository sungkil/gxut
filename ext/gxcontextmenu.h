#pragma once

#if __has_include(<gxut/gxos.h>)
	#include <gxut/gxos.h>
#else __has_include("../gxos.h")
	#include "../gxos.h"
#endif

#include <future>
#include <shellapi.h>

//*************************************
namespace ctx {
//*************************************

struct registry_t
{
	string	subkey;

	// common shell entries
	struct
	{
		string	name, custom_name, icon, cmd;
		struct { bool before=false, after=false; } separator;
	} shell;

	struct entry_t { string name, value; entry_t()=default; entry_t( string n, string v):name(n),value(v){} };
	struct datum_t { string section, subkey, value; vector<entry_t> entries; datum_t()=default; datum_t( string sk ):subkey(sk){} };
	vector<datum_t*> data;

	static path_t exe_path(){ return path_t(exe::path()).replace_extension("exe"); }

	template <HKEY root=HKEY_CLASSES_ROOT> bool add( string subkey, string name, string value );
	bool	add_shell( string subkey, bool b_log=true ){ return add_shell_impl<HKEY_CLASSES_ROOT>(subkey,b_log)&&add_shell_impl<HKEY_LOCAL_MACHINE>(subkey,b_log); }
	bool	add_office( string prefix, int end, int begin=1 );
	bool	install( bool b_dry=false );
	string	dump();

protected:
	template <HKEY root> datum_t* get_or_create_datum( string subkey );
	template <HKEY root> bool subkey_exists( string subkey ) const { return os::reg::key_t<root>(subkey.c_str()).open(); }
	template <HKEY root> bool add_shell_impl( string subkey, bool b_log );
};

template <HKEY root>
registry_t::datum_t* registry_t::get_or_create_datum( string subkey )
{
	subkey=str_replace(subkey.c_str(),"/","\\");
	for( auto* d : data ) if(stricmp(d->subkey.c_str(),subkey.c_str())==0) return d;
	auto* d = new datum_t(subkey); d->section = wtoa(os::reg::key_t<root>(subkey.c_str()).c_str());
	data.emplace_back(d);
	return d;
}

template <HKEY root>
inline bool registry_t::add( string subkey, string name, string value )
{
	subkey=str_replace(subkey.c_str(),"/","\\");
	if(!subkey_exists<root>(subkey)){ printf( "%s(%s,%s,%s): %s not exists\n",__func__,subkey.c_str(),name.c_str(),value.c_str(),subkey.c_str() ); return false; }
	auto* d = get_or_create_datum<root>(subkey); if(!d) return false;
	d->entries.emplace_back(name,value);
	return true;
}

template <HKEY root>
bool registry_t::add_shell_impl( string subkey, bool b_log )
{
	if(shell.name.empty()){ printf("%s(): empty name\n",__func__); return false; }
	subkey=format("%s\\shell",str_replace(subkey.c_str(),"/","\\"));
	if(root==HKEY_LOCAL_MACHINE) subkey="SOFTWARE\\Classes\\"s+subkey;
	if(!subkey_exists<root>(subkey)){ if(b_log) printf( "%s(%s): %s not exists\n",__func__, subkey.c_str(), subkey.c_str() ); return false; }
	
	subkey+="\\"s+shell.name;
	auto* d=get_or_create_datum<root>(subkey); if(!d) return false;
	if(!shell.custom_name.empty())	d->value = shell.custom_name;
	if(!shell.icon.empty())			d->entries.emplace_back("Icon",shell.icon);
	if(shell.separator.before)		d->entries.emplace_back("SeparatorBefore","");
	if(shell.separator.after)		d->entries.emplace_back("SeparatorAfter","");

	// add command
	if(!shell.cmd.empty())
	{
		auto* c=get_or_create_datum<root>(subkey+"\\command"); if(!c) return false;
		c->value = shell.cmd;
	}

	return true;
}

inline bool registry_t::add_office( string prefix, int end, int begin )
{
	bool b_dirty=false;
	for( int k=begin, kn=end+1; k<kn; k++ )
	{
		string j = prefix+format(".%d",k);
		if(add_shell_impl<HKEY_CLASSES_ROOT>(j,false)) continue;
		if(add_shell_impl<HKEY_LOCAL_MACHINE>(j,false)) continue;
		b_dirty = true;
	}
	return b_dirty;
}

inline string registry_t::dump()
{
	if(data.empty()) return "";

	string buffer = "Windows Registry Editor Version 5.00\n\n";
	for( auto* d : data )
	{
		buffer += d->section+"\n";
		if(!d->value.empty()) buffer += format( "@=\"%s\"\n", str_escape(d->value.c_str()));
		for( auto& e : d->entries )
			buffer += format( "\"%s\"=\"%s\"\n", e.name.c_str(), str_escape(e.value.c_str()));
		buffer += format( "\n");
	}
	return buffer;
}

inline bool registry_t::install( bool b_dry )
{
	path reg_path = localtemp()+exe_path().filename()+".reg";
	if(!b_dry)
	{
		FILE* fp = fopen(reg_path.c_str(),"w,ccs=utf-16le"); if(!fp){ printf("unable to open %s\n", reg_path.to_slash().c_str()); return false; }
		fputws( atow(dump().c_str()), fp );
		fputws( L"\n", fp);
		fclose(fp);
	}

	printf("____________________\n\n");
	printf(trim(dump().c_str()));
	printf("\n____________________\n\n");
	
	if(b_dry) return true;

	// find reg
	path reg_exe_path = env::where("reg");
	if(!reg_exe_path.exists())
	{
		printf( "%s\n", reg_path.to_slash().c_str() );
		reg_path.open(); // simply open the registry file in shell
	}
	else
	{
		auto reg_thread = []( path exe_path, path file_path )
		{
			return os::runas( exe_path.c_str(), format("import %s", auto_quote(file_path.c_str())), true, true );
		};

		printf( "%s import %s", reg_exe_path.stem().c_str(), reg_path.to_slash().c_str() );
		auto t = std::async(reg_thread,reg_exe_path,reg_path);
		
		// find and focus to a UAC window from consent.exe
		HWND h_uac=nullptr;
		for( int k=0, kn=300; k<kn; k++ ) // wait for 3 sec.
		{
			h_uac=os::find_window("Credential Dialog Xaml Host"); if(h_uac&&IsWindowVisible(h_uac)) break;
			h_uac=os::find_window("User Account Control"); if(h_uac&&IsWindowVisible(h_uac)) break;
			Sleep(10);
		}
		if(h_uac&&IsWindowVisible(h_uac)) SwitchToThisWindow(h_uac,true); // move focus to consent.exe

		// now complete the thread
		bool r = t.get(); if(!r){ printf(" failed\n"); return false; }
		printf( "\n" );
	}

	return true;
}

//*************************************
} // end namespace reg
//*************************************
