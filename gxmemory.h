//*******************************************************************
// Copyright 2017 Sungkil Lee
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
#ifndef __GX_MEMORY_H__
#define __GX_MEMORY_H__

#if defined(__has_include)&&__has_include( "gxfilesystem.h" )
	#include "gxfilesystem.h"
#endif
#if defined(__has_include)&&__has_include(<intrin.h>)&&__has_include(<nmmintrin.h>)
	#include <intrin.h>
	#include <nmmintrin.h>
#endif

struct mem_t
{
	void*		ptr = nullptr;
	size_t		size = 0;

	__forceinline mem_t(){ ptr=nullptr; size=0; }
	__forceinline mem_t( size_t _size, const void* _ptr=nullptr ){ ptr=(void*)_ptr; size=_size; }
	__forceinline void* alloc( size_t _size, ptrdiff_t elems=1 ){ return ptr=malloc(size=_size*elems); }
	__forceinline void free(){ if(ptr) ::free(ptr); }
	__forceinline void* cpy( void* src, size_t _size ){ return memcpy(ptr,src,_size); }
	__forceinline void* dup( void* src, size_t _size ){ return ptr=memcpy(malloc(size=_size),src,_size); }
	__forceinline int cmp( void* src, size_t _size ){ return memcmp(ptr,src,_size); }
	__forceinline int strcmp( const char* src ){ return ::memcmp((char*)ptr,src,::strlen(src)); }
	__forceinline int wcscmp( const wchar_t* src ){ return ::memcmp((wchar_t*)ptr,src,::wcslen(src)*sizeof(wchar_t)); }
	__forceinline bool operator==( const mem_t& m ) const { if(size!=m.size) return false; return memcmp(ptr,m.ptr,size)==0; }
	__forceinline operator bool() const { return ptr!=nullptr&&size>0; }
	template <class T> __forceinline operator T* (){ return (T*) ptr; }
};

template <class T> class mmap // memory-mapped file (similarly to virtual memory)
{
	HANDLE	hFile = INVALID_HANDLE_VALUE;		// handle to the file (INVALID_HANDLE_VALUE means pagefile)
	HANDLE	hFileMap = INVALID_HANDLE_VALUE;	// handle to the file mapping
	size_t	_chunk=(1<<16); public: size_t size=0;

	static const wchar_t* _uname(){ static wchar_t fileName[256]; SYSTEMTIME s; GetSystemTime( &s ); wsprintfW( fileName, L"%p%02d%02d%02d%02d%04d%05d", GetCurrentThreadId(), s.wDay, s.wHour, s.wMinute, s.wSecond, s.wMilliseconds, rand() ); return fileName; } // make unique file name
	mmap( size_t n, size_t chunk=(1<<16) ):size(n),_chunk(chunk){ size_t s=size*sizeof(T); hFileMap=CreateFileMappingW( INVALID_HANDLE_VALUE /* use pagefile */, nullptr, PAGE_READWRITE, DWORD(s>>32), DWORD(s&0xffffffff), _uname() ); }
	mmap( const wchar_t* file_path, size_t n=0, size_t chunk=(1<<16) ):size(n),_chunk(chunk) /* if n>0 or n != existing file size, a new file with n is created */ { struct _stat st={0}; bool file_exists=_waccess(file_path,0)==0; if(file_exists) _wstat(file_path,&st); size_t file_size=st.st_size; bool b_open = file_size>0&&(n==0||n==file_size); if(b_open) size=file_size/sizeof(T); else if(file_exists) _wunlink(file_path); hFile = CreateFileW( file_path, GENERIC_READ|GENERIC_WRITE, 0, nullptr, b_open?OPEN_EXISTING:CREATE_ALWAYS, FILE_FLAG_RANDOM_ACCESS, nullptr ); if(hFile==INVALID_HANDLE_VALUE){ size=0; _chunk=0; return; } size_t memsize=sizeof(T)*size; hFileMap=CreateFileMappingW( hFile, nullptr, PAGE_READWRITE, DWORD(uint64_t(memsize)>>32), DWORD(memsize&0xffffffff), nullptr ); if(hFileMap==INVALID_HANDLE_VALUE){ size=0; _chunk=0; CloseHandle(hFile); return; } }
	~mmap(){ if(hFile!=INVALID_HANDLE_VALUE) CloseHandle(hFile); hFile=INVALID_HANDLE_VALUE; if(hFileMap!=INVALID_HANDLE_VALUE) CloseHandle(hFileMap); hFileMap=INVALID_HANDLE_VALUE; }
	T* map( size_t offset=0, size_t n=0 ){ size_t s=offset*sizeof(T); return n==0&&size==0?nullptr:(T*)MapViewOfFile( hFileMap, FILE_MAP_READ|FILE_MAP_WRITE, DWORD(uint64_t(s)>>32), DWORD(s&0xffffffff), (n?n:size)*sizeof(T) ); }
	void unmap( T* p ){ if(p){FlushViewOfFile(p,0);UnmapViewOfFile(p);} }

	// chunk implementation
	inline size_t num_chunks(){ return size==0?0:(size-1)/_chunk+1; }
	inline size_t chunk_size( size_t index ){ return min(_chunk,size-_chunk*index); }
	T* map_chunk( size_t index ){ return map(index*_chunk,chunk_size(index)); }
};

#if !defined(_unzip_H)&&!defined(_zip_H)
typedef struct
{ int index;                 // index of this file within the zip
  TCHAR name[MAX_PATH];      // filename within the zip
  DWORD attr;                // attributes, as in GetFileAttributes.
  FILETIME atime,ctime,mtime;// access, create, modify filetimes in UTC
  long comp_size;            // sizes of item, compressed and uncompressed. These
  long unc_size;             // may be -1 if not yet known (e.g. being streamed in)
} ZIPENTRY;
#endif

struct izip_t	// common interface to zip, 7zip, ...
{
	struct entry : public ZIPENTRY { void* ptr=nullptr; size_t size=0; void* aux=nullptr; /* internal use */ bool is_dir() const { return (attr&FILE_ATTRIBUTE_DIRECTORY)!=0; }}; std::vector<entry> entries;
	path file_path;

	virtual ~izip_t(){}
	virtual void release() = 0;
	virtual bool load() = 0;
	virtual bool extract_to_files( path dir, const wchar_t* name=nullptr ) = 0;	// if name==nullptr, extract all files. otherwise, extract a single file with the name
	virtual bool extract_to_memory( const wchar_t* name=nullptr ) = 0;			// if name==nullptr, extract all files. otherwise, extract a single file with the name
#ifdef UNICODE
	virtual entry* find( const TCHAR* name ){ for(auto& e:entries){ if(_wcsicmp(e.name,name)==0) return &e; } return nullptr; }
#else
	virtual entry* find( const TCHAR* name ){ for(auto& e:entries){ if(_stricmp(e.name,name)==0) return &e; } return nullptr; }
#endif
};

#ifdef _unzip_H
struct zip_t : public izip_t
{
	HZIP hzip=nullptr;

	virtual ~zip_t(){ release(); }
	zip_t( const path& file_path ){ hzip=OpenZip( file_path.absolute(), nullptr ); }
	zip_t( void* ptr, size_t size ){ hzip=OpenZip( ptr, uint(size), nullptr ); }
	
	virtual void release(){ for( auto& e : entries ){ if(e.ptr){ free(e.ptr); e.ptr=nullptr; }} entries.clear(); if(hzip){ CloseZipU(hzip); hzip=nullptr; } }
	virtual bool load(){ if(!hzip) return false; entry e; GetZipItem(hzip,-1,&e ); for(int k=0, kn=e.index;k<kn;k++){ GetZipItem( hzip, k, (ZIPENTRY*) &e); e.ptr=nullptr; e.size=0; if(!e.is_dir()) entries.emplace_back(e); } return true; }
	virtual bool extract_to_files( path dir, const wchar_t* name=nullptr ){ bool b=false; if(!hzip) return b; for(size_t k=0;k<entries.size();k++){ auto& e=entries[k]; if(e.is_dir()||(name&&_wcsicmp(name,e.name)!=0)) continue; path p=dir+e.name; if(!p.dir().exists()) p.dir().mkdir(); UnzipItem( hzip, e.index, p ); b=true; } return b; }
	virtual bool extract_to_memory( const wchar_t* name=nullptr )
	{
		if(!hzip) return false;
		for(size_t k=0;k<entries.size();k++)
		{
			auto& e=entries[k]; if(e.is_dir()||e.ptr||(name&&_wcsicmp(name,e.name)!=0)) continue;
			UnzipItem( hzip, e.index, e.ptr=malloc(e.unc_size), uint(e.size=uint(e.unc_size)) );
		}
		return true;
	}

	static bool cmp_signature( void* ptr ){ static uchar s[4]={0x50,0x4b,0x03,0x04}; return memcmp(ptr,s,4)==0; }
};
#endif

#if defined(__7Z_H) && defined(__7Z_MEMINSTREAM_H)
struct szip_t : public izip_t
{
	CSzArEx			db;
	CFileInStream*	file_stream = nullptr;
	CMemInStream*	mem_stream = nullptr;
	CLookToRead2*	look_stream = nullptr;
	ISzAlloc		alloc_impl = {SzAlloc,SzFree};
	ISzAlloc		alloc_temp = {SzAllocTemp,SzFreeTemp};

	virtual ~szip_t(){ release(); }
	szip_t( const path& _file_path ){file_path=_file_path;file_stream=new CFileInStream();if(InFile_OpenW(&file_stream->file,file_path.absolute())!=0){printf("unable to InFile_OpenW(%s)\n",wtoa(file_path.c_str()));return;} FileInStream_CreateVTable(file_stream);LookToRead2_CreateVTable(look_stream=new CLookToRead2(),0);look_stream->buf=(Byte*)alloc_impl.Alloc(&alloc_impl,1<<18);look_stream->bufSize=1<<18;look_stream->pos=look_stream->size=0;look_stream->realStream=&file_stream->vt;crc_generate_table();}
	szip_t( void* ptr, size_t size ){ if(!ptr||!size) return; crc_generate_table(); MemInStream_Init(mem_stream = new CMemInStream(),ptr,size); }

	virtual void release(){for(auto& e:entries){if(e.ptr){free(e.ptr);e.ptr=nullptr;}}entries.clear();SzArEx_Free(&db,&alloc_impl);if(look_stream){delete look_stream;look_stream=nullptr;} if(file_stream){File_Close(&file_stream->file);delete file_stream;file_stream=nullptr;} if(mem_stream){delete mem_stream;mem_stream=nullptr;} }
	virtual bool load(){if(!look_in_stream()) return false; SzArEx_Init(&db);if(SzArEx_Open(&db,look_in_stream(),&alloc_impl,&alloc_temp)!=SZ_OK){printf("unable to SzArEx_Open(%s)\n",wtoa(file_path.c_str()));release();return false;}for( uint k=0, kn=db.NumFiles; k<kn; k++ ){if(SzArEx_IsDir(&db,k)) continue;entry e;memset(&e,0,sizeof(e));e.index=k;e.attr=SzBitWithVals_Check(&db.Attribs,k)?db.Attribs.Vals[k]:0;if(SzBitWithVals_Check(&db.MTime,k)) memcpy(&e.mtime,db.MTime.Vals+k,sizeof(FILETIME));SzArEx_GetFileNameUtf16(&db,k,(ushort*)e.name);entries.emplace_back(e);}return true;}
	virtual bool extract_to_files( path dir, const wchar_t* name=nullptr ){uchar* ob=nullptr;uint bl=-1;for(size_t k=0,kn=entries.size(),of=0,obs=0,os=0;k<kn;k++){auto& e=entries[k];if(e.is_dir()||(name&&_wcsicmp(name,e.name)!=0)) continue; path p=dir+e.name; if(!p.dir().exists()) p.dir().mkdir(); if(SZ_OK!=SzArEx_Extract(&db,look_in_stream(),e.index,&bl,&ob,&obs,&of,&os,&alloc_impl,&alloc_temp)){wprintf(L"unable to SzArEx_Extract(%s)\n",e.name);return false;} FILE* fp=_wfopen(p,L"wb"); if(!fp){wprintf( L"unable to fopen(%s)\n",p.c_str()); return false; } fwrite(ob+of,os,1,fp );fclose(fp); if(e.attr) SetFileAttributesW(p,e.attr); p.set_filetime(nullptr,nullptr,&e.mtime); } if(ob) alloc_impl.Free(&alloc_impl,ob); return true; }
	virtual bool extract_to_memory( const wchar_t* name=nullptr ){if(!look_in_stream())return false;uchar* ob=nullptr;uint bl=-1;for(size_t k=0,kn=entries.size(),of=0,obs=0,os=0;k<kn;k++){auto& e=entries[k];if(e.is_dir()||e.ptr||(name&&_wcsicmp(name,e.name)!=0)) continue;if(SZ_OK!=SzArEx_Extract(&db,look_in_stream(),e.index,&bl,&ob,&obs,&of,&os,&alloc_impl,&alloc_temp)){wprintf(L"unable to SzArEx_Extract(%s)\n",e.name);return false;} memcpy(e.ptr=malloc(os),ob+of,e.size=os); } if(ob) alloc_impl.Free(&alloc_impl,ob); return true; }

	static bool cmp_signature( void* ptr ){ static uchar s[6]={'7','z',0xBC,0xAF,0x27,0x1C}; return memcmp(ptr,s,6)==0; }
	static void crc_generate_table(){ static bool b=false; if(b) return; CrcGenerateTable(); b=true; }
	ILookInStream* look_in_stream(){ return mem_stream?&mem_stream->s:(file_stream&&look_stream)?&look_stream->vt:nullptr; }
};
#endif

#ifdef MAKEINTRESOURCEW
struct resource_t : public mem_t
{
	HMODULE			hModule = nullptr;
	HRSRC			hres = nullptr;
	HGLOBAL			hmem = nullptr;
	const LPCWSTR	type = nullptr;
	izip_t*			zip = nullptr;
		
	__forceinline resource_t( LPCWSTR lpType, HMODULE _hModule=nullptr ):hModule(_hModule),hres(nullptr),hmem(nullptr),type(lpType),zip(nullptr){}
	__forceinline ~resource_t(){ release(); }
	__forceinline void release(){ if(hmem){ /*UnlockResource(hmem);*/ FreeResource(hmem); hmem=nullptr; } if(zip){ zip->release(); delete zip; zip=nullptr; } }
	__forceinline bool find( LPCWSTR lpName ){ return (hres=FindResourceW( hModule, lpName, type ))!=nullptr; }
	__forceinline bool find( int res_id ){ return find(MAKEINTRESOURCEW(res_id)); }
	
	__forceinline bool load(){ if(!hres||!(hmem=LoadResource(hModule,hres))) return false; size=SizeofResource(hModule,hres); ptr=LockResource(hmem); return size!=0; }
	__forceinline bool load( LPCWSTR lpName ){ return find(lpName)&&load(); }
	__forceinline bool load( int res_id ){ return find(res_id)&&load(); }

	// loading for specific types
	__forceinline std::wstring load_wstring(){ if(type!=MAKEINTRESOURCEW(6/*string 6*/)||!load()) return L""; std::wstring w; w.resize(size/sizeof(wchar_t)); memcpy((void*)w.c_str(),ptr,size); return w; }
	__forceinline izip_t* load_zip()
	{
		if(!load()||!ptr||size<6) return nullptr;
		zip=nullptr;
#if defined(_unzip_H)
		if(!zip&&zip_t::cmp_signature(ptr))		zip=new zip_t(ptr,size);
#endif
#if defined(__7Z_H) && defined(__7Z_MEMINSTREAM_H)
		if(!zip&&szip_t::cmp_signature(ptr))	zip=new szip_t(ptr,size);
#endif
		if(zip&&!zip->load()){delete zip;zip=nullptr;} return zip;
	}
};
#endif // MAKEINTRESOURCEW

//***********************************************
// zlib in-memory compression/decompression

#ifdef ZLIB_H
namespace zlib
{
	inline size_t capacity( size_t size ){ return size+(((size+16383)>>16)*5)+6; }
	__noinline inline int compress( void* src, size_t src_size, void* dst, size_t dst_capacity )
	{
		int ret			= -1; // size of compression (failure: -1)
		z_stream zInfo	= {0};
		zInfo.total_in	= zInfo.avail_in  = (uint)src_size;
		zInfo.total_out	= zInfo.avail_out = (uint)dst_capacity;
		zInfo.next_in	= (BYTE*)src;
		zInfo.next_out	= (BYTE*)dst;

		if(Z_OK!=deflateInit(&zInfo,Z_DEFAULT_COMPRESSION)||Z_STREAM_END!=deflate(&zInfo,Z_FINISH)){ deflateEnd(&zInfo); return ret; }
		else{ ret=zInfo.total_out; deflateEnd(&zInfo); return ret; }
	}

	__noinline inline int decompress( void* src, size_t src_size, void* dst, size_t dst_capacity )
	{
		int ret			= -1; // size of compression (failure: -1)
		z_stream zInfo	= {0};
		zInfo.total_in	= zInfo.avail_in  = (uint)src_size;
		zInfo.total_out	= zInfo.avail_out = (uint)dst_capacity;
		zInfo.next_in	= (BYTE*)src;
		zInfo.next_out	= (BYTE*)dst;

		if(Z_OK!=inflateInit(&zInfo)||Z_STREAM_END!=inflate(&zInfo,Z_FINISH)){ inflateEnd(&zInfo); return ret; }
		else{ ret=zInfo.total_out; inflateEnd(&zInfo); return ret; }
	}

	__noinline inline bool unittest( void* src, size_t src_size )
	{
		size_t c_capacity = capacity(src_size);
		uchar* c = (uchar*) malloc(c_capacity);
		uchar* d = (uchar*) malloc(src_size);
		int r = compress( src, src_size, c, c_capacity );
		decompress( c, r, d, src_size );
		bool b = memcmp( src, d, src_size )==0;
		free(c); free(d);
		return b;
	}
} // namespace zlib
#endif

//***********************************************
// CRC32 implementation with 4-batch parallel construction (taken from zlib)
template <unsigned int poly>
__noinline inline unsigned int tcrc32( const unsigned char* buff, size_t size, unsigned int crc0 )
{
	if(buff==nullptr||size==0) return crc0;
	static unsigned* t[4] = {nullptr};
	if(!t[0])
	{
		for(int k=0;k<4;k++) t[k]=(unsigned*) malloc(sizeof(unsigned)*256);
		for(int k=0;k<256;k++){ unsigned c=k; for( unsigned j=0;j<8;j++) c=c&1?poly^(c>>1):c>>1; t[0][k]=c; }
		for(int k=0;k<256;k++){ unsigned c=t[0][k]; for(int j=1;j<4;j++) t[j][k]=c=t[0][c&0xff]^(c>>8); }
	}

	unsigned c = ~crc0;
	for(;size&&(((ptrdiff_t)buff)&7);size--,buff++) c=t[0][(c^(*buff))&0xff]^(c>>8);	 // move forward to the 8-byte aligned boundary
	for(;size>=4;size-=4,buff+=4){c^=*(unsigned*)buff;c=t[3][(c>>0)&0xff]^t[2][(c>>8)&0xff]^t[1][(c>>16)&0xff]^t[0][(c>>24)&0xff]; }
	for(;size;size--,buff++) c=t[0][(c^(*buff))&0xff]^(c>>8);
	return ~c;
}

// normal crc32
inline unsigned int crc32( void* buff, size_t size, unsigned int crc0=0 ){ return tcrc32<0xedb88320UL>((const unsigned char*)buff,size,crc0); }

// CRC32C SSE4.2 implementation up to 8-batch parallel construction (https://github.com/Voxer/sse4_crc32)
#if defined(__SSE4_2__)||!defined(__clang__)
__noinline inline unsigned int crc32c_hw( const void* buff, size_t size, unsigned int crc0 )
{
	if(buff==nullptr||size==0) return crc0;
	const unsigned char* b = (const unsigned char*) buff;
#if defined(_M_X64)
	uint64_t c = ~uint64_t(crc0);
	for(;size && ((ptrdiff_t)b&7);size--,b++) c=_mm_crc32_u8(uint32_t(c),*b); // move forward to the 8-byte aligned boundary
	for(;size>=sizeof(uint64_t);size-=sizeof(uint64_t),b+=sizeof(uint64_t)) c=_mm_crc32_u64(c,*(uint64_t*)b);
#elif defined(_M_IX86)
	uint32_t c = ~crc0;
	for(;size && ((ptrdiff_t)b&7);size--,b++) c=_mm_crc32_u8(uint32_t(c),*b); // move forward to the 8-byte aligned boundary
#endif
	for(;size>=sizeof(uint32_t);size-=sizeof(uint32_t),b+=sizeof(uint32_t)) c=_mm_crc32_u32((uint32_t)c,*(uint32_t*)b);
	for(;size>=sizeof(uint16_t);size-=sizeof(uint16_t),b+=sizeof(uint16_t)) c=_mm_crc32_u16((uint32_t)c,*(uint16_t*)b);
	for(;size>=sizeof(uint8_t);size-=sizeof(uint8_t),b+=sizeof(uint8_t)) c=_mm_crc32_u8((uint32_t)c,*(uint8_t*)b);

	return uint32_t(~c);
}
#ifdef _MSC_VER
inline bool has_sse42(){ static bool b=false,h=true; if(b) return h; b=true; int regs[4]={0}; __cpuid(regs,1); return h=((regs[2]>>20)&1)==1; }
#else
inline bool has_sse42(){ return true; }
#endif
inline unsigned int crc32c( const void* buff, size_t size, unsigned int crc0=0 ){ return has_sse42()?crc32c_hw(buff,size,crc0):tcrc32<0x82f63b78UL>((const unsigned char*)buff,size,crc0); }
#else
inline unsigned int crc32c( const void* buff, size_t size, unsigned int crc0=0 ){ return tcrc32<0x82f63b78UL>((const unsigned char*)buff,size,crc0); }
#endif

// crc32c wrappers
inline unsigned int crc32c( const char* s ){ return crc32c((void*)s,strlen(s)); }
inline unsigned int crc32c( const wchar_t* s ){ return crc32c((void*)s,wcslen(s)*sizeof(wchar_t)); }

//***********************************************
// MD5 implementation
class md5
{
	uint32_t a=0x67452301, b=0xefcdab89, c=0x98badcfe, d=0x10325476, lo=0, hi=0; unsigned char buffer[64]; uint4 _digest;
	inline const void* body( const unsigned char* data, size_t size );
	inline void update( const void* data, size_t size );
	inline void finalize( unsigned char *result );
public:
	md5( const void* ptr, size_t size ){ memset(buffer,0,sizeof(buffer)); memset(&_digest,0,sizeof(_digest)); if(!ptr||!size) return; update(ptr,size); finalize((unsigned char*)&_digest); for(int k=0;k<16;k++)sprintf((char*)buffer+k*2,"%02x",((unsigned char*)&_digest)[k]); buffer[32]=0; }
	md5( const char* str ):md5(str,strlen(str)){}
	const char* c_str() const { return (const char*)buffer; }
	const uint4& digest() const { return _digest; }
};

__noinline inline const void* md5::body( const unsigned char* data, size_t size )
{
#define MD5_F(x,y,z)				((z)^((x)&((y)^(z))))
#define MD5_G(x,y,z)				((y)^((z)&((x)^(y))))
#define MD5_H(x,y,z)				(((x)^(y))^(z))
#define MD5_H2(x,y,z)				((x)^((y)^(z)))
#define MD5_I(x,y,z)				((y)^((x)|~(z)))
#define MD5_STEP(f,a,b,c,d,x,t,s) 	(a)+=f((b),(c),(d))+(*(uint32_t *)&data[(x)*4])+(t);(a)=(((a)<<(s))|(((a)&0xffffffff)>>(32-(s))));(a)+=(b)
	for( uint32_t a0=a,b0=b,c0=c,d0=d; size; data+= 64,size-=64,a0=a+=a0,b0=b+=b0,c0=c+=c0,d0=d+=d0)
	{
		MD5_STEP(MD5_F, a, b, c, d, 0,	0xd76aa478, 7);	MD5_STEP(MD5_F, d, a, b, c, 1,	0xe8c7b756, 12);	MD5_STEP(MD5_F, c, d, a, b, 2, 0x242070db, 17);	MD5_STEP(MD5_F, b, c, d, a, 3, 0xc1bdceee, 22);
		MD5_STEP(MD5_F, a, b, c, d, 4,	0xf57c0faf, 7);	MD5_STEP(MD5_F, d, a, b, c, 5,	0x4787c62a, 12);	MD5_STEP(MD5_F, c, d, a, b, 6, 0xa8304613, 17);	MD5_STEP(MD5_F, b, c, d, a, 7, 0xfd469501, 22);
		MD5_STEP(MD5_F, a, b, c, d, 8,	0x698098d8, 7);	MD5_STEP(MD5_F, d, a, b, c, 9,	0x8b44f7af, 12);	MD5_STEP(MD5_F, c, d, a, b, 10,0xffff5bb1, 17);	MD5_STEP(MD5_F, b, c, d, a, 11,0x895cd7be, 22);
		MD5_STEP(MD5_F, a, b, c, d, 12,	0x6b901122, 7);	MD5_STEP(MD5_F, d, a, b, c, 13,	0xfd987193, 12);	MD5_STEP(MD5_F, c, d, a, b, 14,0xa679438e, 17);	MD5_STEP(MD5_F, b, c, d, a, 15,0x49b40821, 22);
		MD5_STEP(MD5_G, a, b, c, d, 1,	0xf61e2562, 5);	MD5_STEP(MD5_G, d, a, b, c, 6,	0xc040b340, 9);		MD5_STEP(MD5_G, c, d, a, b, 11,0x265e5a51, 14);	MD5_STEP(MD5_G, b, c, d, a, 0, 0xe9b6c7aa, 20);
		MD5_STEP(MD5_G, a, b, c, d, 5,	0xd62f105d, 5);	MD5_STEP(MD5_G, d, a, b, c, 10,	0x02441453, 9);		MD5_STEP(MD5_G, c, d, a, b, 15,0xd8a1e681, 14);	MD5_STEP(MD5_G, b, c, d, a, 4, 0xe7d3fbc8, 20);
		MD5_STEP(MD5_G, a, b, c, d, 9,	0x21e1cde6, 5);	MD5_STEP(MD5_G, d, a, b, c, 14,	0xc33707d6, 9);		MD5_STEP(MD5_G, c, d, a, b, 3, 0xf4d50d87, 14);	MD5_STEP(MD5_G, b, c, d, a, 8, 0x455a14ed, 20);
		MD5_STEP(MD5_G, a, b, c, d, 13,	0xa9e3e905, 5);	MD5_STEP(MD5_G, d, a, b, c, 2,	0xfcefa3f8, 9);		MD5_STEP(MD5_G, c, d, a, b, 7, 0x676f02d9, 14);	MD5_STEP(MD5_G, b, c, d, a, 12,0x8d2a4c8a, 20);
		MD5_STEP(MD5_H, a, b, c, d, 5,	0xfffa3942, 4);	MD5_STEP(MD5_H2,d, a, b, c, 8,	0x8771f681, 11);	MD5_STEP(MD5_H, c, d, a, b, 11,0x6d9d6122, 16);	MD5_STEP(MD5_H2,b, c, d, a, 14,0xfde5380c, 23);
		MD5_STEP(MD5_H, a, b, c, d, 1,	0xa4beea44, 4);	MD5_STEP(MD5_H2,d, a, b, c, 4,	0x4bdecfa9, 11);	MD5_STEP(MD5_H, c, d, a, b, 7, 0xf6bb4b60, 16);	MD5_STEP(MD5_H2,b, c, d, a, 10,0xbebfbc70, 23);
		MD5_STEP(MD5_H, a, b, c, d, 13,	0x289b7ec6, 4);	MD5_STEP(MD5_H2,d, a, b, c, 0,	0xeaa127fa, 11);	MD5_STEP(MD5_H, c, d, a, b, 3, 0xd4ef3085, 16);	MD5_STEP(MD5_H2,b, c, d, a, 6, 0x04881d05, 23);
		MD5_STEP(MD5_H, a, b, c, d, 9,	0xd9d4d039, 4);	MD5_STEP(MD5_H2,d, a, b, c, 12,	0xe6db99e5, 11);	MD5_STEP(MD5_H, c, d, a, b, 15,0x1fa27cf8, 16);	MD5_STEP(MD5_H2,b, c, d, a, 2, 0xc4ac5665, 23);
		MD5_STEP(MD5_I, a, b, c, d, 0,	0xf4292244, 6);	MD5_STEP(MD5_I, d, a, b, c, 7,	0x432aff97, 10);	MD5_STEP(MD5_I, c, d, a, b, 14,0xab9423a7, 15);	MD5_STEP(MD5_I, b, c, d, a, 5, 0xfc93a039, 21);
		MD5_STEP(MD5_I, a, b, c, d, 12,	0x655b59c3, 6);	MD5_STEP(MD5_I, d, a, b, c, 3,	0x8f0ccc92, 10);	MD5_STEP(MD5_I, c, d, a, b, 10,0xffeff47d, 15);	MD5_STEP(MD5_I, b, c, d, a, 1, 0x85845dd1, 21);
		MD5_STEP(MD5_I, a, b, c, d, 8,	0x6fa87e4f, 6);	MD5_STEP(MD5_I, d, a, b, c, 15,	0xfe2ce6e0, 10);	MD5_STEP(MD5_I, c, d, a, b, 6, 0xa3014314, 15);	MD5_STEP(MD5_I, b, c, d, a, 13,0x4e0811a1, 21);
		MD5_STEP(MD5_I, a, b, c, d, 4,	0xf7537e82, 6);	MD5_STEP(MD5_I, d, a, b, c, 11,	0xbd3af235, 10);	MD5_STEP(MD5_I, c, d, a, b, 2, 0x2ad7d2bb, 15);	MD5_STEP(MD5_I, b, c, d, a, 9, 0xeb86d391, 21);
	}
	return data;
}
 
__noinline inline void md5::update( const void* data, size_t size )
{
	uint32_t lo0=lo;
	if((lo = (lo0+size)&0x1fffffff) < lo0) hi++; hi += uint32_t(size) >> 29;
 	uint32_t used = lo0&0x3f;
 	if(used)
	{
		uint32_t available = 64-used;
		if(size<available){ memcpy(&buffer[used], data, size); return; }
		memcpy(&buffer[used], data, available);
		data = (const unsigned char *)data + available;
		size -= available;
		body(buffer,64);
	}
	if (size >= 64){ data = body((const unsigned char*)data,size&~(unsigned long)0x3f); size &= 0x3f; }
	memcpy(buffer, data, size);
}
 
__noinline inline void md5::finalize( unsigned char* result )
{
	uint32_t used = lo & 0x3f;;
	buffer[used++] = 0x80;
	uint32_t available = 64 - used;
 
	if(available < 8){ memset(&buffer[used], 0, available); body(buffer,64); used = 0; available = 64; }
	memset(&buffer[used], 0, available - 8);
 
	buffer[56] = (lo <<= 3); buffer[57] = lo>>8; buffer[58] = lo>>16; buffer[59] = lo>>24; buffer[60] = hi; buffer[61] = hi>>8; buffer[62] = hi>>16; buffer[63] = hi>>24; body(buffer,64);
 	result[0] = a; result[1] = a >> 8; result[2] = a >> 16; result[3] = a >> 24; result[4] = b; result[5] = b >> 8; result[6] = b >> 16; result[7] = b >> 24; result[8] = c;
	result[9] = c >> 8; result[10] = c >> 16; result[11] = c >> 24; result[12] = d; result[13] = d >> 8; result[14] = d >> 16; result[15] = d >> 24;
}

//***********************************************
// augmentation of filesystem
#ifdef __GX_FILESYSTEM_H__

__noinline inline std::string path::md5() const
{
	mem_t m={0}; if(!exists()) return ""; FILE* fp=_wfopen(data,L"rb"); if(!fp) return "";
	fseek(fp,0,SEEK_END); m.size=ftell(fp); fseek(fp,0,SEEK_SET); fread(m.ptr=malloc(m.size),m.size,1,fp); fclose(fp);
	std::string s = ::md5(m.ptr,m.size).c_str();
	if(m.ptr) free(m.ptr);
	return s;
}

__noinline inline uint path::crc32() const
{
	mem_t m={0}; if(!exists()) return 0; FILE* fp=_wfopen(data,L"rb"); if(!fp) return 0;
	fseek(fp,0,SEEK_END); m.size=ftell(fp); fseek(fp,0,SEEK_SET); fread(m.ptr=malloc(m.size),m.size,1,fp); fclose(fp);
	uint c = ::crc32(m.ptr,m.size);
	if(m.ptr) free(m.ptr);
	return c;
}

__noinline inline uint path::crc32c() const
{
	mem_t m={0}; if(!exists()) return 0; FILE* fp=_wfopen(data,L"rb"); if(!fp) return 0;
	fseek(fp,0,SEEK_END); m.size=ftell(fp); fseek(fp,0,SEEK_SET); fread(m.ptr=malloc(m.size),m.size,1,fp); fclose(fp);
	uint c = ::crc32c(m.ptr,m.size);
	if(m.ptr) free(m.ptr);
	return c;
}

#endif // __GX_FILESYSTEM_H__

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

	void writef( const char* fmt, ... ){ if(!fp) return; va_list a; va_start(a,fmt); vfprintf_s(fp,fmt,a); va_end(a); }
	void readf( const char* fmt, ... ){ if(!fp) return; va_list a; va_start(a,fmt); vfscanf_s(fp,fmt,a); va_end(a); }
	void write( void* ptr, size_t size ){ if(fp) fwrite( ptr, size, 1, fp ); }
	void read( void* ptr, size_t size ){ if(fp) fread(ptr,size,1,fp); }
	void close(){ if(fp){ fclose(fp); fp=nullptr; } if(b_read&&zip_path().exists()) cache_path().rmfile(); else if(!b_read) compress(); }
	__noinline bool open( bool read=true )
	{
		b_read = read;
		uint64_t sig = uint64_t(std::hash<std::string>{}(std::string(__TIMESTAMP__)+signature()));
		path cpath = cache_path(), zpath = zip_path();
		if(b_read)
		{
			if(zpath.exists()&&!decompress()) return false;
			fp = _wfopen( cpath, L"rb" ); if(!fp){ if(zpath.exists()) cpath.rmfile(); return false; }
			uint64_t s; fscanf( fp, "%*s = %llu\n", &s ); if(sig!=s){ fclose(fp); return false; }
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

#if defined(_zip_H) && defined(_unzip_H)
__noinline inline bool binary_cache::compress( bool rm_src )
{
	if(!cache_path().exists()) return false;
	HZIP hZip = CreateZip( zip_path(), nullptr );
	if(ZR_OK==ZipAdd( hZip, cache_path().name(), cache_path() )){ CloseZip(hZip); if(rm_src) cache_path().rmfile(); return true; }
	else { wprintf( L"Unable to compress %s\n", cache_path().name() ); CloseZip( hZip ); return false; }
}

__noinline inline bool binary_cache::decompress()
{
	if(!zip_path().exists()) return false;
	zip_t zip_file(zip_path()); return zip_file.load()&&!zip_file.entries.empty()&&zip_file.extract_to_files(zip_path().dir());
}
#endif

//***********************************************
} // namespace gx
//***********************************************

//***********************************************
#endif // __GX_MEMORY__