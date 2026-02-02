#pragma once
#ifndef __GXZIP_H__
#define __GXZIP_H__

#if __has_include("gxut.h")
	#include "gxut.h"
	#include "gxmemory.h"
#elif __has_include("../gxut.h")
	#include "../gxut.h"
	#include "../gxmemory.h"
#elif __has_include(<gxut/gxut.h>)
	#include <gxut/gxut.h>
	#include <gxut/gxmemory.h>
#endif

// automatically link static lib
#pragma comment( lib, "gxzip" )

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

	~zip_t(){ release(); }
	zip_t( const path& _file_path ){ hzip=OpenZip( atow((file_path=_file_path.absolute()).c_str()), nullptr); if(!hzip) file_path.clear(); else _zipmtime=TimeToFileTime(file_path.mtime()); }
	zip_t( void* ptr, size_t size ){ hzip=OpenZip( ptr, uint(size), nullptr ); }

	virtual void release() override { for( auto& e : entries ){ if(e.ptr){ free(e.ptr); e.ptr=nullptr; }} entries.clear(); if(hzip){ CloseZipU(hzip); hzip=nullptr; } }
	virtual bool load() override { if(!hzip) return false; zipentry_t e;GetZipItem(hzip,-1,&e); for(int k=0,kn=e.index;k<kn;k++){ GetZipItem( hzip, k, (ZIPENTRY*) &e); e.size=size_t(e.unc_size); e.ptr=nullptr; e.mtime=_fix_dostime(e.mtime); if(!e.is_dir()) entries.emplace_back(e); } return true; }
	virtual bool extract_to_files( path dir, const char* name=nullptr ) override { bool b=false; if(!hzip) return b; for(auto& e:entries){ if(e.is_dir()||(name&&stricmp(name,wtoa(e.name))!=0)) continue; path p=dir+wtoa(e.name); if(!p.dir().exists()) p.dir().mkdir(); bool x=p.exists(),r=x&&p.is_readonly(),h=x&&p.is_hidden(); if(r) p.set_readonly(false); if(h) p.set_hidden(false); UnzipItem( hzip, e.index, atow(p.c_str())); _fix_dostime(e.mtime, p); if(r) p.set_readonly(true); if(h) p.set_hidden(true); b=true; } return b; }
	virtual zipentry_t* extract_to_memory( const char* name=nullptr ) override { if(!hzip||entries.empty()) return nullptr; for(size_t k=0;k<entries.size();k++){ auto& e=entries[k]; if(e.is_dir()||e.ptr||(name&&stricmp(name,wtoa(e.name))!=0)) continue; UnzipItem( hzip, e.index, e.ptr=malloc(e.unc_size), uint(e.unc_size) ); } return name?find(name):&entries.front(); }

private:
	FILETIME	_zipmtime={};	// to fix dostime error
	FILETIME	_fix_dostime( FILETIME t, path f="" );
};

inline FILETIME zip_t::_fix_dostime( FILETIME t, path f )
{
	time_t t0 = (_zipmtime.dwLowDateTime||_zipmtime.dwHighDateTime)?FileTimeToTime(_zipmtime):0;
	if(!t0||abs(FileTimeToTime(t)-t0)>2) return t;
	if(!f.empty()&&f.exists()) f.utime(t0); return _zipmtime;
}

#ifdef __7ZIP_H__
struct szip_t : public izip_t
{
	~szip_t(){ release(); }
	szip_t( const path& _file_path ){file_path=_file_path;file_stream=new CFileInStream();if(InFile_OpenW(&file_stream->file,atow(file_path.absolute().c_str()))!=0){printf("unable to InFile_OpenW(%s)\n",file_path.c_str());return;} FileInStream_CreateVTable(file_stream);LookToRead2_CreateVTable(look_stream=new CLookToRead2(),0);look_stream->buf=(Byte*)alloc_impl.Alloc(&alloc_impl,1ull<<18);look_stream->bufSize=1ull<<18;look_stream->pos=look_stream->size=0;look_stream->realStream=&file_stream->vt;crc_generate_table();}
	szip_t( void* ptr, size_t size ){ if(!ptr||!size) return; crc_generate_table(); MemInStream_Init(mem_stream = new CMemInStream(),ptr,size); }

	virtual void release() override {for(auto& e:entries){if(e.ptr){free(e.ptr);e.ptr=nullptr;}}entries.clear();SzArEx_Free(&db,&alloc_impl);if(look_stream){delete look_stream;look_stream=nullptr;} if(file_stream){File_Close(&file_stream->file);delete file_stream;file_stream=nullptr;} if(mem_stream){delete mem_stream;mem_stream=nullptr;} }
	virtual bool load() override {if(!look_in_stream()) return false; SzArEx_Init(&db);if(SzArEx_Open(&db,look_in_stream(),&alloc_impl,&alloc_temp)!=SZ_OK){printf("unable to SzArEx_Open(%s)\n",file_path.c_str());release();return false;}for( uint k=0, kn=db.NumFiles; k<kn; k++ ){if(SzArEx_IsDir(&db,k)) continue;zipentry_t e;memset(&e,0,sizeof(e));e.index=k;SzArEx_GetFileNameUtf16(&db,k,(ushort*)e.name);e.attr=SzBitWithVals_Check(&db.Attribs,k)?db.Attribs.Vals[k]:0;if(SzBitWithVals_Check(&db.MTime,k)) memcpy(&e.mtime,db.MTime.Vals+k,sizeof(FILETIME));e.size=SzArEx_GetFileSize(&db,k);e.unc_size=long(e.size);entries.emplace_back(e);}return true;}
	virtual bool extract_to_files( path dir, const char* name=nullptr ) override {uchar* ob=nullptr;uint bl=-1;for(size_t k=0,kn=entries.size(),of=0,obs=0,os=0;k<kn;k++){auto& e=entries[k];if(e.is_dir()||(name&&stricmp(name,wtoa(e.name))!=0)) continue; path p=dir+wtoa(e.name); if(!p.dir().exists()) p.dir().mkdir(); if(SZ_OK!=SzArEx_Extract(&db,look_in_stream(),e.index,&bl,&ob,&obs,&of,&os,&alloc_impl,&alloc_temp)){printf("unable to SzArEx_Extract(%s)\n",wtoa(e.name));return false;} bool x=p.exists(),r=x&&p.is_readonly(),h=x&&p.is_hidden(); if(r) p.set_readonly(false); if(h) p.set_hidden(false); FILE* fp=fopen(p.c_str(),"wb"); if(!fp){ printf("unable to fopen(%s)\n", p.c_str()); return false; } fwrite(ob+of, os, 1, fp); fclose(fp); if(e.attr) SetFileAttributesW(atow(p.c_str()), e.attr); p.utime(FileTimeToTime(e.mtime)); if(r) p.set_readonly(true); if(h) p.set_hidden(true); } if(ob) alloc_impl.Free(&alloc_impl, ob); return true; }
	virtual zipentry_t* extract_to_memory( const char* name=nullptr ) override { if(!look_in_stream()||entries.empty())return nullptr; uchar* ob=nullptr;uint bl=-1;for(size_t k=0,kn=entries.size(),of=0,obs=0,os=0;k<kn;k++){auto& e=entries[k];if(e.is_dir()||e.ptr||(name&&stricmp(name,wtoa(e.name))!=0)) continue;if(SZ_OK!=SzArEx_Extract(&db,look_in_stream(),e.index,&bl,&ob,&obs,&of,&os,&alloc_impl,&alloc_temp)){printf("unable to SzArEx_Extract(%s)\n",wtoa(e.name));return nullptr;} e.ptr=malloc(os); if(e.ptr) memcpy(e.ptr,ob+of,e.size=os); if(ob) alloc_impl.Free(&alloc_impl,ob); } return name?find(name):&entries.front(); }

	static void crc_generate_table(){ static bool b=false; if(b) return; CrcGenerateTable(); b=true; }
	ILookInStream* look_in_stream(){ return mem_stream?&mem_stream->s:(file_stream&&look_stream)?&look_stream->vt:nullptr; }

private:
	CSzArEx			db;
	CFileInStream*	file_stream = nullptr;
	CMemInStream*	mem_stream = nullptr;
	CLookToRead2*	look_stream = nullptr;
	ISzAlloc		alloc_impl = {SzAlloc,SzFree};
	ISzAlloc		alloc_temp = {SzAllocTemp,SzFreeTemp};	
};
#endif

// unified loaders for zip and 7zip
inline izip_t* load_zip( const path& file_path )
{
	izip_t* z=nullptr; path x=file_path.extension();
	if(x=="zip") z=new zip_t(file_path);
#ifdef __7ZIP_H__
	else if(x=="7z") z=new szip_t(file_path);
#else
	else if(x=="7z") printf("%s(%s): 7zip implementation not found\n",__func__,file_path.c_str());
#endif
	if(!z) return nullptr; if(!z->load()||z->entries.empty()) safe_delete(z); return z;
}

inline izip_t* load_zip( void* ptr, size_t size )
{
	izip_t* z=nullptr;
	if(is_zip_signature(ptr,size)) z=new zip_t(ptr,size); // is_zip_signature in gxmemory.h
#ifdef __7ZIP_H__
	else if(is_7zip_signature(ptr,size)) z=new szip_t(ptr,size); // is_7zip_signature in gxmemory.h
#else
	else if(is_7zip_signature(ptr,size)) printf("%s(): 7zip implementation not found\n",__func__);
#endif
	if(!z) return nullptr; if(!z->load()||z->entries.empty()) safe_delete(z); return z;
}

// lazy implementation of resource_t::load_zip
#ifdef MAKEINTRESOURCEW
inline izip_t* resource_t::load_zip()
{
	if(!load()||!ptr||size<6) return nullptr;
	zip=nullptr;
	if(is_zip_signature(ptr,size)) zip=new zip_t(ptr,size); // is_zip_signature in gxmemory.h
#ifdef __7ZIP_H__
	if(!zip&&is_7zip_signature(ptr,size)) zip=new szip_t(ptr,size); // is_7zip_signature in gxmemory.h
#endif
	if(!zip) return nullptr; if(!zip->load()||zip->entries.empty()) safe_delete(zip); return zip;
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

__noinline vector<uchar> compress( void* ptr, size_t size, bool b_gzip )
{
	z_stream s; memset(&s,0,sizeof(decltype(s)));
	int ret = b_gzip?deflateInit2_(&s,-1,8,MAX_WBITS+16,8,0,ZLIB_VERSION,sizeof(s)):deflateInit_(&s,-1,ZLIB_VERSION,sizeof(s));
	if(ret!=0/*Z_OK*/) return vector<uchar>();
	
	vector<uchar> buff(capacity(size));
	static const uint chunk = 0x8000;
	size_t todo = size;
	s.next_in	= (uchar*) ptr;
	s.next_out	= &buff[0];
	for( ret=0; todo && ret==0; )
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

__noinline vector<uchar> decompress( void* ptr, size_t size )
{
	z_stream s; memset(&s,0,sizeof(s));
	vector<uchar> buff; if(size<3) return buff;

	static const uchar siggz[] = {0x1F,0x8B,0x08};
	bool b_gz = memcmp(ptr,siggz,sizeof(siggz))==0;
	s.next_in = (uchar*) ptr;
	if(b_gz) inflateInit2_(&s,MAX_WBITS+16,ZLIB_VERSION,int(sizeof(z_stream))); else inflateInit_(&s,ZLIB_VERSION,int(sizeof(z_stream)));
	
	static const uint chunk = 0x8000;
	static vector<uchar> temp(chunk);
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
	path file_path; // this must be used in dtor, so declare as a member; do not use virtual function

	binary_cache( bool compress ){ b.compress=compress; }
	virtual ~binary_cache();
	operator FILE* () const { return fp; }

	virtual path zip_path(){ return file_path.empty()?"":file_path+".zip"; }
	virtual uint crc()=0; // custom crc for signature detection
	virtual bool open( bool read=true );

	int writef( __printf_format_string__ const char* fmt, ... ){ if(!fp) return EOF; va_list a; va_start(a,fmt); int r=_vfprintf_l(fp,fmt,NULL,a); va_end(a); return r; }
	int getsf( const char* fmt, ... ){ if(!fp) return EOF; static char* buff=(char*)malloc((1<<14)+1); fgets(buff,(1<<14),fp); va_list a; va_start(a,fmt); int r=vsscanf(buff,fmt,a); va_end(a); return r; }
	bool write( void* ptr, size_t size ){ if(!fp) return false; return size==fwrite(ptr,1,size,fp); }
	bool read( void* ptr, size_t size ){ if(!fp) return false; return size==fread(ptr,1,size,fp); }
	bool compress( bool rm_src=true );
	bool decompress();

private:
	FILE* fp = nullptr;
	struct { bool read=false, compress=false; } b;
};

__noinline binary_cache::~binary_cache()
{
	// caution: dtor here uses only base-class functions, because a derive object has been already destroyed
	if(fp){ fclose(fp); fp=nullptr; } if(!b.compress) return;
	if(!b.read) compress(); else if(zip_path().exists()&&file_path.exists()) unlink(file_path.c_str());
}

__noinline bool binary_cache::open( bool read )
{
	b.read = read;
	uint sig = crc32c(crc(),string(__TIMESTAMP__));
	path cpath = file_path;
	path zpath = zip_path();
	if(b.read)
	{
		if(b.compress&&zpath.exists()&&!decompress()) return false;
		if(!cpath.exists()) return false;
		fp = fopen(cpath.c_str(),"rb");
		uint s=0; fread(&s,sizeof(s),1,fp);
		if(!fp||sig!=s){ if(fp){ fclose(fp); fp=nullptr; } if(zpath.exists()) unlink(cpath.c_str()); return false; }
	}
	else
	{
		if(!cpath.dir().exists()) cpath.dir().mkdir();
		fp = fopen(cpath.c_str(),"wb"); if(!fp) return false;
		fwrite(&sig,sizeof(sig),1,fp);
	}
	return true;
}

__noinline bool binary_cache::compress( bool rm_src )
{
	if(!file_path.exists()) return false;
	path zpath=zip_path(); zpath.dir().mkdir();
	HZIP hZip = CreateZip( atow(zpath.c_str()), nullptr);
	if(ZR_OK==ZipAdd( hZip, atow(file_path.filename().c_str()), atow(file_path.c_str()))){ CloseZipZ(hZip); if(rm_src) unlink(file_path.c_str()); return true; }
	else { printf( "Unable to compress %s\n", file_path.filename().c_str() ); CloseZipZ( hZip ); return false; }
}

__noinline bool binary_cache::decompress()
{
	path zpath=zip_path(); if(!zpath.exists()) return false;
	zip_t zip_file(zpath); return zip_file.load()&&!zip_file.entries.empty()&&zip_file.extract_to_files(zpath.dir());
}

//***********************************************
} // namespace gx
//***********************************************

#endif // __GXZIP_H__
