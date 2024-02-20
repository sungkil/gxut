#pragma once

using std::string;
using std::wstring;
using std::vector;

//*************************************
namespace ctx {
//*************************************

struct registry_t
{
	wstring			name;
	wstring			cmd;
	wstring			icon;
	
	static path exe_path(){ return path::module_path().replace_extension(L"exe"); }

	template <HKEY root=HKEY_CLASSES_ROOT> bool add( wstring subkey, wstring name, wstring value );
	template <HKEY root=HKEY_CLASSES_ROOT> bool add_shell( wstring subkey ){ return add_shell<root>(vector<wstring>{subkey}); }
	template <HKEY root=HKEY_CLASSES_ROOT> bool add_shell( vector<wstring> subkeys );
	template <HKEY root=HKEY_CLASSES_ROOT> bool add_shell_office( const wchar_t* prefix, int end, int begin=1 );
	bool install( bool b_dry=false );

protected:
	wstring			buffer = L"Windows Registry Editor Version 5.00\n\n";
};

template <HKEY root>
inline bool registry_t::add( wstring subkey, wstring name, wstring value )
{
	os::reg::key_t<root> i(str_replace(subkey.c_str(),L"/",L"\\")); if(!i.open()) return false;
	buffer += format( L"%s\n", i.c_str() );
	buffer += format( L"\"%s\"=\"%s\"\n", name.c_str(), str_escape(value.c_str()) );
	buffer += format( L"\n");
	return true;
}

template <HKEY root>
inline bool registry_t::add_shell( vector<wstring> subkeys )
{
	for( auto s : subkeys )
	{
		os::reg::key_t<root> r( s.c_str()); if(!r.open()) continue;

		// register icon first
		os::reg::key_t<root> i( L"%s/shell/%s", s.c_str(), name.c_str() );
		buffer += format( L"%s\n", i.c_str() );
		buffer += format( L"\"Icon\"=\"%s\"\n", str_escape(icon.c_str()) );
		buffer += format( L"\n");

		// register cmd
		os::reg::key_t<root> c( L"%s/shell/%s/command", s.c_str(), name.c_str() );
		buffer += format( L"%s\n", c.c_str() );
		buffer += format( L"@=\"%s\"\n", str_escape(cmd.c_str()) );
		buffer += format( L"\n");
	}

	return true;
}

template <HKEY root>
inline bool registry_t::add_shell_office( const wchar_t* prefix, int end, int begin )
{
	wstring cls = root==HKEY_CLASSES_ROOT?L"":root==HKEY_LOCAL_MACHINE?L"SOFTWARE\\Classes\\":L"";
	vector<wstring> subkeys = { format(L"%s%s",cls.c_str(),prefix ) };
	for( int k=begin, kn=end+1; k<kn; k++ ) subkeys.emplace_back( format(L"%s%s.%d",cls.c_str(),prefix,k) );

	return add_shell<root>( subkeys );
}

inline bool registry_t::install( bool b_dry )
{
	path reg_path = path::temp( true ) + exe_path().name() + L".reg";
	FILE* fp = _wfopen( reg_path.c_str(), L"w,ccs=utf-16le"); if(!fp){ printf("unable to open %s\n", reg_path.to_slash().wtoa()); return false; }
	fputws( buffer.c_str(), fp );
	fclose(fp);

	if(b_dry)	printf( "%s\n", trim(wtoa(buffer.c_str())) ) ;
	else {		printf( "%s\n", reg_path.to_slash().wtoa() ); reg_path.open(); }

	return true;
}

//*************************************
} // end namespace reg
//*************************************
