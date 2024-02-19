#pragma once

//*************************************
namespace ctx {
//*************************************

inline vector<wstring> build_office_subkeys( const wchar_t* prefix, int end, int begin=1 )
{
	vector<wstring> v = {prefix};
	for( int k=begin, kn=end+1; k<kn; k++ ) v.emplace_back( format(L"%s.%d",prefix,k) );
	return v;
}

template <HKEY ROOT=HKEY_CLASSES_ROOT>
inline bool install( std::wstring name, std::wstring cmd, std::wstring icon, const std::vector<std::wstring>& subkeys )
{
	path reg_path = path::module_path().replace_extension(L"exe")+L".reg";
	FILE* fp = _wfopen( reg_path.c_str(), L"w,ccs=utf-16le"); if(!fp){ printf("unable to open %s\n", reg_path.to_slash().wtoa()); return false; }

	fwprintf( fp, L"Windows Registry Editor Version 5.00\n\n" );
	for( auto s : subkeys )
	{
		if(ROOT==HKEY_LOCAL_MACHINE) s = std::wstring(L"SOFTWARE\\Classes\\")+s;

		os::reg::key_t<ROOT> r(s.c_str()); if(!r.open()) continue;

		// register icon first
		os::reg::key_t<ROOT> i( L"%s/shell/%s", s.c_str(), name.c_str() );
		fwprintf( fp, L"%s\n", i.c_str() );
		fwprintf( fp, L"\"Icon\"=\"%s\"\n", str_escape(icon.c_str()) );
		fwprintf( fp, L"\n");

		// register cmd
		os::reg::key_t<ROOT> c( L"%s/shell/%s/command", s.c_str(), name.c_str() );
		fwprintf( fp, L"%s\n", c.c_str() );
		fwprintf( fp, L"@=\"%s\"\n", str_escape(cmd.c_str()) );
		fwprintf( fp, L"\n");
	}
	fclose(fp);
	
	if(!reg_path.exists()) return false;

	//printf( "%s\n", reg_path.to_slash().wtoa() );
	printf( "%s\n", trim(wtoa(reg_path.read_file().c_str())) );
	reg_path.open();
	return true;
}

//*************************************
} // end namespace reg
//*************************************
