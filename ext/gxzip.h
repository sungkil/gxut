#pragma once
#ifndef __GXZIP_H__
#define __GXZIP_H__

#if __has_include("gxmemory.h")
	#include "gxmemory.h"
#elif __has_include("../gxmemory.h")
	#include "../gxmemory.h"
#elif __has_include(<gxut/gxmemory.h>)
	#include <gxut/gxmemory.h>
#endif

// automatically link static lib
#pragma comment( lib, "gxzip" )

// unified loaders for zip and 7zip
izip_t* load_zip( const path& file_path );
izip_t* load_zip( void* ptr, size_t size );
bool save_zip( const path& zip_path, const path& file_path );

//*************************************
namespace zlib {
//*************************************
inline size_t capacity( size_t size ){ return size+(((size+16383)>>16)*5)+6; }
vector<uchar> compress( void* ptr, size_t size, bool b_gzip );
vector<uchar> decompress( void* ptr, size_t size );
//*************************************
} // namespace zlib
//*************************************

//***********************************************
namespace gx {
//***********************************************

template <bool zip=false>
struct binary_cache
{
	virtual ~binary_cache(){ close(); }
	virtual path file_path()=0;
	virtual uint crc()=0; // implement and augment crc for signature detection
	virtual bool open( bool read=true );
	void close();
	
	operator FILE* () const { return fp; }
	path zip_path(){ return _file_path.empty()?"":_file_path+".zip"; }
	int writef( __printf_format_string__ const char* fmt, ... ){ if(!fp) return EOF; va_list a; va_start(a,fmt); int r=_vfprintf_l(fp,fmt,NULL,a); va_end(a); return r; }
	int getsf( const char* fmt, ... ){ if(!fp) return EOF; static char* buff=(char*)malloc((1<<14)+1); fgets(buff,(1<<14),fp); va_list a; va_start(a,fmt); int r=vsscanf(buff,fmt,a); va_end(a); return r; }
	bool write( void* ptr, size_t size ){ if(!fp) return false; return size==fwrite(ptr,1,size,fp); }
	bool read( void* ptr, size_t size ){ if(!fp) return false; return size==fread(ptr,1,size,fp); }

private:
	FILE* fp = nullptr;
	struct { bool read=false; } b;
	const path _file_path; // cache of file_path(); declare as a member, since virtual function in dtor is already destroyed
	bool compress();
	bool decompress();
};

template<> __noinline void binary_cache<false>::close()
{
	if(!fp) return; fclose(fp); fp=nullptr;
}
template<> __noinline void binary_cache<true>::close()
{
	// caution: dtor here uses only base-class functions, because a derive object has been already destroyed
	if(!fp) return; fclose(fp); fp=nullptr;
	if(!b.read) compress(); else if(zip_path().exists()&&_file_path.exists()) unlink(_file_path.c_str());
}

template <bool zip>
__noinline bool binary_cache<zip>::open( bool read )
{
	b.read = read;
	uint sig = crc32c(crc(),string(__TIMESTAMP__));
	const_cast<path&>(_file_path) = file_path();
	path zpath = zip_path();
	if(b.read)
	{
		if(zip&&zpath.exists()&&!decompress()) return false;
		if(!_file_path.exists()) return false;
		fp = fopen(_file_path.c_str(),"rb");
		uint s=0; fread(&s,sizeof(s),1,fp);
		if(!fp||sig!=s){ if(fp){ fclose(fp); fp=nullptr; } if(zpath.exists()) unlink(_file_path.c_str()); return false; }
	}
	else
	{
		if(!_file_path.dir().exists()) _file_path.dir().mkdir();
		fp = fopen(_file_path.c_str(),"wb"); if(!fp) return false;
		fwrite(&sig,sizeof(sig),1,fp);
	}
	return true;
}

template<> inline bool binary_cache<false>::compress(){ return false; }
template<> inline bool binary_cache<false>::decompress(){ return false; }
template<> bool binary_cache<true>::compress();
template<> bool binary_cache<true>::decompress();

//***********************************************
} // namespace gx
//***********************************************

#endif // __GXZIP_H__
