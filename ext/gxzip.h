#pragma once
#ifndef __GXZIP_H__
#define __GXZIP_H__

#if __has_include("gxtype.h")
	#include "gxtype.h"
	#include "gxmemory.h"
#elif __has_include("../gxtype.h")
	#include "../gxtype.h"
	#include "../gxmemory.h"
#elif __has_include(<gxut/gxtype.h>)
	#include <gxut/gxtype.h>
	#include <gxut/gxmemory.h>
#endif

// automatically link static lib
#pragma comment( lib, "gxzip.lib" )

// to be compatible with zip_utils
#ifndef _unzip_H
	#define _unzip_H
#endif
#ifndef _zip_H
	#define _zip_H
#endif

// include essential headers
#if __has_include(<7zip/7zTypes.h>)
	#define __7ZIP_H__
	#include <7zip/7zTypes.h>
	#include <7zip/7z.h>
	#include <7zip/7zAlloc.h>
	#include <7zip/7zCrc.h>
	#include <7zip/7zFile.h>
	#include <7zip/7zVersion.h>
	#include <7zip/7zMemInStream-slee.h> // https://github.com/poiru/7z-memstream
#elif __has_include(<7zip/include/7zTypes.h>)
	#define __7ZIP_H__
	#include <7zip/include/7zTypes.h>
	#include <7zip/include/7z.h>
	#include <7zip/include/7zAlloc.h>
	#include <7zip/include/7zCrc.h>
	#include <7zip/include/7zFile.h>
	#include <7zip/include/7zVersion.h>
	#include <7zip/include/7zMemInStream-slee.h> // https://github.com/poiru/7z-memstream
#endif

//*************************************
// common definitions for /zip_utils/zip.h and unzip.h
DECLARE_HANDLE(HZIP);
typedef DWORD ZRESULT;
#define ZR_OK         0x00000000     // nb. the pseudo-code zr-recent is never returned,
#define ZR_RECENT     0x00000001     // but can be passed to FormatZipMessage.
#define ZR_GENMASK    0x0000FF00
#define ZR_NODUPH     0x00000100     // couldn't duplicate the handle
#define ZR_NOFILE     0x00000200     // couldn't create/open the file
#define ZR_NOALLOC    0x00000300     // failed to allocate some resource
#define ZR_WRITE      0x00000400     // a general error writing to the file
#define ZR_NOTFOUND   0x00000500     // couldn't find that file in the zip
#define ZR_MORE       0x00000600     // there's still more data to be unzipped
#define ZR_CORRUPT    0x00000700     // the zipfile is corrupt or not a zipfile
#define ZR_READ       0x00000800     // a general error reading the file
#define ZR_PASSWORD   0x00001000     // we didn't get the right password to unzip the file
#define ZR_CALLERMASK 0x00FF0000
#define ZR_ARGS       0x00010000     // general mistake with the arguments
#define ZR_NOTMMAP    0x00020000     // tried to ZipGetMemory, but that only works on mmap zipfiles, which yours wasn't
#define ZR_MEMSIZE    0x00030000     // the memory size is too small
#define ZR_FAILED     0x00040000     // the thing was already failed when you called this function
#define ZR_ENDED      0x00050000     // the zip creation has already been closed
#define ZR_MISSIZE    0x00060000     // the indicated input file size turned out mistaken
#define ZR_PARTIALUNZ 0x00070000     // the file had already been partially unzipped
#define ZR_ZMODE      0x00080000     // tried to mix creating/opening a zip 
#define ZR_BUGMASK    0xFF000000
#define ZR_NOTINITED  0x01000000     // initialisation didn't work
#define ZR_SEEK       0x02000000     // trying to seek in an unseekable file
#define ZR_NOCHANGE   0x04000000     // changed its mind on storage, but not allowed
#define ZR_FLATE      0x05000000     // an internal error in the de/inflation code

//*************************************
// below taken from /zip_utils/unzip.h
#ifndef __ZIPENTRY__
#define __ZIPENTRY__
typedef struct
{ int index;                 // index of this file within the zip
  TCHAR name[MAX_PATH];      // filename within the zip
  DWORD attr;                // attributes, as in GetFileAttributes.
  FILETIME atime,ctime,mtime;// access, create, modify filetimes
  long comp_size;            // sizes of item, compressed and uncompressed. These
  long unc_size;             // may be -1 if not yet known (e.g. being streamed in)
} ZIPENTRY;
#endif
HZIP OpenZip(const TCHAR *fn, const char *password);
HZIP OpenZip(void *z,unsigned int len, const char *password);
HZIP OpenZipHandle(HANDLE h, const char *password);
ZRESULT GetZipItem(HZIP hz, int index, ZIPENTRY *ze);
ZRESULT FindZipItem(HZIP hz, const TCHAR *name, bool ic, int *index, ZIPENTRY *ze);
ZRESULT UnzipItem(HZIP hz, int index, const TCHAR *fn);
ZRESULT UnzipItem(HZIP hz, int index, void *z,unsigned int len);
ZRESULT UnzipItemHandle(HZIP hz, int index, HANDLE h);
ZRESULT SetUnzipBaseDir(HZIP hz, const TCHAR *dir);
ZRESULT CloseZipU(HZIP hz);
bool IsZipHandleU(HZIP hz);

//*************************************
// below taken from /zip_utils/zip.h
HZIP CreateZip(const TCHAR *fn, const char *password);
HZIP CreateZip(void *buf,unsigned int len, const char *password);
HZIP CreateZipHandle(HANDLE h, const char *password);
ZRESULT ZipAdd(HZIP hz,const TCHAR *dstzn, const TCHAR *fn);
ZRESULT ZipAdd(HZIP hz,const TCHAR *dstzn, void *src,unsigned int len);
ZRESULT ZipAddHandle(HZIP hz,const TCHAR *dstzn, HANDLE h);
ZRESULT ZipAddHandle(HZIP hz,const TCHAR *dstzn, HANDLE h, unsigned int len);
ZRESULT ZipAddFolder(HZIP hz,const TCHAR *dstzn);
ZRESULT ZipGetMemory(HZIP hz, void **buf, unsigned long *len);
ZRESULT CloseZipZ(HZIP hz);
bool IsZipHandleZ(HZIP hz);

//*************************************
// standardized interface to zip
struct zip_t : public izip_t
{
	HZIP hzip=nullptr;

	static bool cmp_signature( void* ptr ){ static uchar s[4]={0x50,0x4b,0x03,0x04}; return memcmp(ptr,s,4)==0; }

	virtual ~zip_t(){ release(); }
	zip_t( const path& _file_path ){ hzip=OpenZip( file_path=_file_path.absolute(), nullptr ); if(!hzip) file_path.clear(); else _zipmtime64=FileTimeToUint64(_zipmtime=file_path.mfiletime()); }
	zip_t( void* ptr, size_t size ){ hzip=OpenZip( ptr, uint(size), nullptr ); }

	virtual void release(){ for( auto& e : entries ){ if(e.ptr){ free(e.ptr); e.ptr=nullptr; }} entries.clear(); if(hzip){ CloseZipU(hzip); hzip=nullptr; } }
	virtual bool load(){ if(!hzip) return false; entry e;GetZipItem(hzip,-1,&e); for(int k=0,kn=e.index;k<kn;k++){ GetZipItem( hzip, k, (ZIPENTRY*) &e); e.ptr=nullptr; e.size=0; e.mtime=_fix_dostime(e.mtime); if(!e.is_dir()) entries.emplace_back(e); } return true; }
	virtual bool extract_to_files( path dir, const wchar_t* name=nullptr ){ bool b=false; if(!hzip) return b; for(size_t k=0;k<entries.size();k++){ auto& e=entries[k]; if(e.is_dir()||(name&&_wcsicmp(name,e.name)!=0)) continue; path p=dir+e.name; if(!p.dir().exists()) p.dir().mkdir(); UnzipItem( hzip, e.index, p ); _fix_dostime( e.mtime, p ); b=true; } return b; }
	virtual bool extract_to_memory( const wchar_t* name=nullptr ){ if(!hzip) return false; for(size_t k=0;k<entries.size();k++){ auto& e=entries[k]; if(e.is_dir()||e.ptr||(name&&_wcsicmp(name,e.name)!=0)) continue; UnzipItem( hzip, e.index, e.ptr=malloc(e.unc_size), uint(e.size=uint(e.unc_size)) ); } return true; }

protected:

	FILETIME	_zipmtime={};	// to fix dostime error
	uint64_t	_zipmtime64=0;	// 
	bool		_in_dostime_error( FILETIME t ){ return (_zipmtime64&&abs(int(FileTimeToUint64(t)-_zipmtime64))<=FileTimeOffset(0,0,0,2)); }
	FILETIME	_fix_dostime( FILETIME t, path f=L"" ){ if(!_in_dostime_error(t)) return t; if(!f.empty()&&f.exists()) f.set_filetime(nullptr,nullptr,&_zipmtime); return _zipmtime; }
};

#ifdef __7ZIP_H__
struct szip_t : public izip_t
{
	CSzArEx			db;
	CFileInStream*	file_stream = nullptr;
	CMemInStream*	mem_stream = nullptr;
	CLookToRead2*	look_stream = nullptr;
	ISzAlloc		alloc_impl = {SzAlloc,SzFree};
	ISzAlloc		alloc_temp = {SzAllocTemp,SzFreeTemp};

	~szip_t(){ release(); }
	szip_t( const path& _file_path ){file_path=_file_path;file_stream=new CFileInStream();if(InFile_OpenW(&file_stream->file,file_path.absolute())!=0){printf("unable to InFile_OpenW(%s)\n",wtoa(file_path.c_str()));return;} FileInStream_CreateVTable(file_stream);LookToRead2_CreateVTable(look_stream=new CLookToRead2(),0);look_stream->buf=(Byte*)alloc_impl.Alloc(&alloc_impl,1ull<<18);look_stream->bufSize=1ull<<18;look_stream->pos=look_stream->size=0;look_stream->realStream=&file_stream->vt;crc_generate_table();}
	szip_t( void* ptr, size_t size ){ if(!ptr||!size) return; crc_generate_table(); MemInStream_Init(mem_stream = new CMemInStream(),ptr,size); }

	virtual void release(){for(auto& e:entries){if(e.ptr){free(e.ptr);e.ptr=nullptr;}}entries.clear();SzArEx_Free(&db,&alloc_impl);if(look_stream){delete look_stream;look_stream=nullptr;} if(file_stream){File_Close(&file_stream->file);delete file_stream;file_stream=nullptr;} if(mem_stream){delete mem_stream;mem_stream=nullptr;} }
	virtual bool load(){if(!look_in_stream()) return false; SzArEx_Init(&db);if(SzArEx_Open(&db,look_in_stream(),&alloc_impl,&alloc_temp)!=SZ_OK){printf("unable to SzArEx_Open(%s)\n",wtoa(file_path.c_str()));release();return false;}for( uint k=0, kn=db.NumFiles; k<kn; k++ ){if(SzArEx_IsDir(&db,k)) continue;entry e;memset(&e,0,sizeof(e));e.index=k;e.attr=SzBitWithVals_Check(&db.Attribs,k)?db.Attribs.Vals[k]:0;if(SzBitWithVals_Check(&db.MTime,k)) memcpy(&e.mtime,db.MTime.Vals+k,sizeof(FILETIME));SzArEx_GetFileNameUtf16(&db,k,(ushort*)e.name);entries.emplace_back(e);}return true;}
	virtual bool extract_to_files( path dir, const wchar_t* name=nullptr ){uchar* ob=nullptr;uint bl=-1;for(size_t k=0,kn=entries.size(),of=0,obs=0,os=0;k<kn;k++){auto& e=entries[k];if(e.is_dir()||(name&&_wcsicmp(name,e.name)!=0)) continue; path p=dir+e.name; if(!p.dir().exists()) p.dir().mkdir(); if(SZ_OK!=SzArEx_Extract(&db,look_in_stream(),e.index,&bl,&ob,&obs,&of,&os,&alloc_impl,&alloc_temp)){wprintf(L"unable to SzArEx_Extract(%s)\n",e.name);return false;} FILE* fp=_wfopen(p,L"wb"); if(!fp){wprintf( L"unable to fopen(%s)\n",p.c_str()); return false; } fwrite(ob+of,os,1,fp );fclose(fp); if(e.attr) SetFileAttributesW(p,e.attr); p.set_filetime(nullptr,nullptr,&e.mtime); } if(ob) alloc_impl.Free(&alloc_impl,ob); return true; }
	virtual bool extract_to_memory( const wchar_t* name=nullptr ){if(!look_in_stream())return false;uchar* ob=nullptr;uint bl=-1;for(size_t k=0,kn=entries.size(),of=0,obs=0,os=0;k<kn;k++){auto& e=entries[k];if(e.is_dir()||e.ptr||(name&&_wcsicmp(name,e.name)!=0)) continue;if(SZ_OK!=SzArEx_Extract(&db,look_in_stream(),e.index,&bl,&ob,&obs,&of,&os,&alloc_impl,&alloc_temp)){wprintf(L"unable to SzArEx_Extract(%s)\n",e.name);return false;} e.ptr=malloc(os); if(e.ptr) memcpy(e.ptr,ob+of,e.size=os); } if(ob) alloc_impl.Free(&alloc_impl,ob); return true; }

	static bool cmp_signature( void* ptr ){ static uchar s[6]={'7','z',0xBC,0xAF,0x27,0x1C}; return memcmp(ptr,s,6)==0; }
	static void crc_generate_table(){ static bool b=false; if(b) return; CrcGenerateTable(); b=true; }
	ILookInStream* look_in_stream(){ return mem_stream?&mem_stream->s:(file_stream&&look_stream)?&look_stream->vt:nullptr; }
};
#endif

//***********************************************
// lazy implementation of resource_t::load_zip
#ifdef MAKEINTRESOURCEW
inline izip_t* resource_t::load_zip()
{
	if(!load()||!ptr||size<6) return nullptr;
	zip=nullptr;
	if(!zip&&zip_t::cmp_signature(ptr))		zip=new zip_t(ptr,size);
#ifdef __7ZIP_H__
	if(!zip&&szip_t::cmp_signature(ptr))	zip=new szip_t(ptr,size);
#endif
	if(zip&&!zip->load()){delete zip;zip=nullptr;} return zip;
}
#endif

//***********************************************
// zlib in-memory compression/decompression
#ifndef ZLIB_H
#define ZLIB_H
#define ZLIB_VERSION "1.2.11"
#define MAX_WBITS 15 /* 32K LZ77 window */
typedef void* (*alloc_func)(void* opaque, uint items, uint size);
typedef void  (*free_func)(void* opaque, void* address);
typedef struct z_stream_s
{
	uchar*	next_in;	// next input byte
	uint	avail_in;	// number of bytes available at next_in
	ulong	total_in;	// total number of input bytes read so far
	uchar*	next_out;	// next output byte will go here
	uint	avail_out;	// remaining free space at next_out
	ulong	total_out;	// total number of bytes output so far
	char*	msg;		// last error message, NULL if no error
	struct internal_state* state;	// not visible by applications
	alloc_func	zalloc;	// used to allocate the internal state
	free_func	zfree;	// used to free the internal state
	void*	opaque;		// private data object passed to zalloc and zfree
	int		data_type;	// best guess about the data type: binary or text for deflate, or the decoding state for inflate
	ulong	adler;		// Adler-32 or CRC-32 value of the uncompressed data
	ulong	reserved;	// reserved for future use
} z_stream;

extern "C"
{
	int deflateInit_( z_stream* strm, int level, const char* version, int stream_size );
	int inflateInit_( z_stream* strm, const char* version, int stream_size );
	int deflateInit2_( z_stream* strm, int level, int method, int windowBits, int memLevel, int strategy, const char *version, int stream_size );
	int inflateInit2_( z_stream* strm, int windowBits, const char* version, int stream_size );

	int inflate( z_stream* strm, int flush );
	int inflateEnd( z_stream* strm );
	int deflate (z_stream* strm, int flush );
	int deflateEnd( z_stream* strm );
}

//*************************************
namespace zlib {
//*************************************
inline size_t capacity( size_t size ){ return size+(((size+16383)>>16)*5)+6; }

__noinline std::vector<uchar> compress( void* ptr, size_t size, bool b_gzip )
{
	z_stream s; memset(&s,0,sizeof(decltype(s)));
	int ret = b_gzip?deflateInit2_(&s,-1,8,MAX_WBITS+16,8,0,ZLIB_VERSION,sizeof(s)):deflateInit_(&s,-1,ZLIB_VERSION,sizeof(s));
	if(ret!=0/*Z_OK*/) return std::vector<uchar>();
	
	std::vector<uchar> buff(capacity(size));
	static const uint chunk = 0x8000;
	size_t todo = size;
	s.next_in	= (uchar*) ptr;
	s.next_out	= &buff[0];
	for( int ret=0; todo && ret==0; )
	{
		ulong total_in0 = s.total_in;
		s.avail_in = uint(todo<chunk?todo:chunk);
		s.avail_out = chunk;
		ret = deflate( &s,(s.avail_in==todo) ? 4 : 2 ); // Z_FINISH : Z_SYNC_FLUSH
		todo -= (size_t(s.total_in)-total_in0);
	}
	buff.resize(s.total_out);
	buff.shrink_to_fit();
	deflateEnd(&s);
	return buff;
}

__noinline std::vector<uchar> decompress( void* ptr, size_t size )
{
	z_stream s; memset(&s,0,sizeof(s));
	std::vector<uchar> buff; if(size<3) return buff;

	static const uchar siggz[] = {0x1F,0x8B,0x08};
	bool b_gz = memcmp(ptr,siggz,sizeof(siggz))==0;
	s.next_in = (uchar*) ptr;
	if(b_gz) inflateInit2_(&s,MAX_WBITS+16,ZLIB_VERSION,int(sizeof(z_stream))); else inflateInit_(&s,ZLIB_VERSION,int(sizeof(z_stream)));
	
	static const uint chunk = 0x8000;
	static std::vector<uchar> temp(chunk);
	size_t todo = size;

	buff.reserve(chunk);
	for( int ret=0; todo && ret==0; )
	{
		unsigned long total_in0 = s.total_in;
		unsigned long total_out0 = s.total_out;
		s.avail_in = uint(todo<chunk?todo:chunk);
		s.avail_out = chunk;
		s.next_out = &temp[0];
		ret = inflate(&s,2); // 2: Z_SYNC_FLUSH
		todo -= size_t(s.total_in)-total_in0;
		buff.insert(buff.end(),temp.begin(),temp.begin()+s.total_out-total_out0);
	}
	inflateEnd(&s);
	buff.shrink_to_fit();
	return buff;
}

//*************************************
} // namespace zlib
//*************************************
#endif // ZLIB_H

//***********************************************
namespace gx {
//***********************************************

struct binary_cache
{
	FILE* fp = nullptr;
	bool b_read;

	~binary_cache(){ if(fp) fclose(fp); fp=nullptr; }

	virtual path cache_path() = 0;
	virtual path zip_path(){ return cache_path()+L".zip"; }
	virtual std::string signature() = 0;

	void writef( const char* fmt, ... ){ if(!fp) return; va_list a; va_start(a,fmt); vfprintf(fp,fmt,a); va_end(a); }
	void readf( const char* fmt, ... ){ if(!fp) return; va_list a; va_start(a,fmt); vfscanf(fp,fmt,a); va_end(a); }
	void write( void* ptr, size_t size ){ if(fp) fwrite( ptr, size, 1, fp ); }
	void read( void* ptr, size_t size ){ if(fp) fread(ptr,size,1,fp); }
	void close()
	{
		if(fp){ fclose(fp); fp=nullptr; }
		if(!b_read) compress();
		else { if(zip_path().exists()) cache_path().rmfile(); }
	}
	__noinline bool open( bool read=true )
	{
		b_read = read;
		uint64_t sig = uint64_t(std::hash<std::string>{}(std::string(__TIMESTAMP__)+signature()));
		path cpath=cache_path(), zpath=zip_path();
		if(b_read)
		{
			if(zpath.exists()&&!decompress()) return false;
			if(!cpath.exists()) return false;
			fp = _wfopen( cpath, L"rb" );
			uint64_t s=0; fscanf( fp, "%*s = %llu\n", &s );
			if(!fp||sig!=s){ if(fp) fclose(fp); if(zpath.exists()) cpath.rmfile(); return false; }
		}
		else
		{
			if(!cpath.dir().exists()) cpath.dir().mkdir();
			fp = _wfopen( cpath, L"wb" ); if(!fp) return false;
			fprintf( fp, "signature = %llu\n", sig );
		}
		return true;
	}

	std::vector<uchar> pack_bits( std::vector<bool>& v ){ std::vector<uchar> b((v.size()+7)>>3,0); for(size_t k=0,kn=v.size();k<kn;k++) if(v[k]) b[k>>3] |= (1<<uchar(k&7)); return b; }
	std::vector<bool> unpack_bits( std::vector<uchar>& b, size_t max_count=0xffffffff ){ std::vector<bool> v(min(b.size()*8,max_count),0); for(size_t k=0,kn=v.size();k<kn;k++) if(b[k>>3]&(1<<uchar(k&7))) v[k]=true; return v; }

	bool compress( bool rm_src=true );
	bool decompress();
};

__noinline bool binary_cache::compress( bool rm_src )
{
	if(!cache_path().exists()) return false;
	HZIP hZip = CreateZip( zip_path(), nullptr );
	if(ZR_OK==ZipAdd( hZip, cache_path().name(), cache_path() )){ CloseZipZ(hZip); if(rm_src) cache_path().rmfile(); return true; }
	else { wprintf( L"Unable to compress %s\n", cache_path().name().c_str() ); CloseZipZ( hZip ); return false; }
}

__noinline bool binary_cache::decompress()
{
	if(!zip_path().exists()) return false;
	zip_t zip_file(zip_path()); return zip_file.load()&&!zip_file.entries.empty()&&zip_file.extract_to_files(zip_path().dir());
}

//***********************************************
} // namespace gx
//***********************************************

#endif // __GXZIP_H__
