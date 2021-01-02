//*******************************************************************
// Copyright 2011-2019 Sungkil Lee
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
#if defined(__has_include)&&__has_include( "gxmath.h" )
	#include "gxmath.h"
#endif
#if defined(__has_include)&&__has_include(<intrin.h>)&&__has_include(<nmmintrin.h>)
	#include <intrin.h>
	#include <nmmintrin.h>
#endif

using std::min;
using std::max;

struct mem_t : sized_ptr_t<void>
{
	__forceinline mem_t(){ ptr=nullptr; size=0; }
	__forceinline mem_t( size_t Size, const void* Ptr=nullptr ){ ptr=(void*)Ptr; size=Size; }
	virtual ~mem_t(){}
	__forceinline void* alloc( size_t _size, ptrdiff_t elems=1 ){ return ptr=malloc(size=_size*elems); }
	__forceinline void free(){ if(ptr) ::free(ptr); size=0; }
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
	mmap( const wchar_t* file_path, size_t n=0, size_t chunk=(1<<16) ):size(n),_chunk(chunk) /* if n>0 or n != existing file size, a new file with n is created */ { struct _stat st={}; bool file_exists=_waccess(file_path,0)==0; if(file_exists) _wstat(file_path,&st); size_t file_size=st.st_size; bool b_open = file_size>0&&(n==0||n==file_size); if(b_open) size=file_size/sizeof(T); else if(file_exists) _wunlink(file_path); hFile = CreateFileW( file_path, GENERIC_READ|GENERIC_WRITE, 0, nullptr, b_open?OPEN_EXISTING:CREATE_ALWAYS, FILE_FLAG_RANDOM_ACCESS, nullptr ); if(hFile==INVALID_HANDLE_VALUE){ size=0; _chunk=0; return; } size_t memsize=sizeof(T)*size; hFileMap=CreateFileMappingW( hFile, nullptr, PAGE_READWRITE, DWORD(uint64_t(memsize)>>32), DWORD(memsize&0xffffffff), nullptr ); if(hFileMap==INVALID_HANDLE_VALUE){ size=0; _chunk=0; CloseHandle(hFile); return; } }
	virtual ~mmap(){ if(hFile!=INVALID_HANDLE_VALUE) CloseHandle(hFile); hFile=INVALID_HANDLE_VALUE; if(hFileMap!=INVALID_HANDLE_VALUE) CloseHandle(hFileMap); hFileMap=INVALID_HANDLE_VALUE; }
	T* map( size_t offset=0, size_t n=0 ){ size_t s=offset*sizeof(T); return n==0&&size==0?nullptr:(T*)MapViewOfFile( hFileMap, FILE_MAP_READ|FILE_MAP_WRITE, DWORD(uint64_t(s)>>32), DWORD(s&0xffffffff), (n?n:size)*sizeof(T) ); }
	void unmap( T* p ){ if(p){FlushViewOfFile(p,0);UnmapViewOfFile(p);} }

	// chunk implementation
	inline size_t num_chunks(){ return size==0?0:(size-1)/_chunk+1; }
	inline size_t chunk_size( size_t index ){ return min(_chunk,size-_chunk*index); }
	T* map_chunk( size_t index ){ return map(index*_chunk,chunk_size(index)); }
};

#ifndef __ZIPENTRY__
#define __ZIPENTRY__
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
	virtual entry* find( const wchar_t* name ){ for(auto& e:entries){ if(_wcsicmp(e.name,name)==0) return &e; } return nullptr; }
};

#ifdef MAKEINTRESOURCEW
struct resource_t : public mem_t
{
	HMODULE			hModule = nullptr;
	HRSRC			hres = nullptr;
	HGLOBAL			hmem = nullptr;
	const LPCWSTR	type = nullptr;
	izip_t*			zip = nullptr;

	resource_t( LPCWSTR lpType, HMODULE _hModule=nullptr ):hModule(_hModule),hres(nullptr),hmem(nullptr),type(lpType),zip(nullptr){}
	~resource_t(){ release(); }
	void release(){ if(hmem){ /*UnlockResource(hmem);*/ FreeResource(hmem); hmem=nullptr; } if(zip){ zip->release(); delete zip; zip=nullptr; } }
	bool find( LPCWSTR lpName ){ return (hres=FindResourceW( hModule, lpName, type ))!=nullptr; }
	bool find( int res_id ){ return find(MAKEINTRESOURCEW(res_id)); }

	bool load(){ if(!hres||!(hmem=LoadResource(hModule,hres))) return false; size=SizeofResource(hModule,hres); ptr=LockResource(hmem); return size!=0; }
	bool load( LPCWSTR lpName ){ return find(lpName)&&load(); }
	bool load( int res_id ){ return find(res_id)&&load(); }

	// loading for specific types
	std::wstring load_wstring(){ if(type!=MAKEINTRESOURCEW(6/*string 6*/)||!load()) return L""; std::wstring w; w.resize(size/sizeof(wchar_t)); memcpy((void*)w.c_str(),ptr,size); return w; }
	izip_t* load_zip();
};
#endif // MAKEINTRESOURCEW

//***********************************************
// CRC32C SSE4.2 implementation up to 8-batch parallel construction (https://github.com/Voxer/sse4_crc32)
#if !defined(_MSC_VER)||defined(__clang__)
inline unsigned int crc32c( const void* buff, size_t size, unsigned int crc0=0 ){ return tcrc32<0x82f63b78UL>(buff,size,crc0); }
#else
__noinline unsigned int crc32c_hw( const void* buff, size_t size, unsigned int crc0 )
{
	if(!buff||!size) return crc0; const unsigned char* b = (const unsigned char*) buff;
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
inline bool has_sse42(){ static bool b=false,h=true; if(b) return h; b=true; int regs[4]={}; __cpuid(regs,1); return h=((regs[2]>>20)&1)==1; }
inline unsigned int crc32c( const void* buff, size_t size, unsigned int crc0=0 ){ static unsigned int(*pcrc32c)(const void*,size_t,unsigned int)=has_sse42()?crc32c_hw:tcrc32<0x82f63b78UL>; return pcrc32c(buff,size,crc0); }
#endif

// regular crc32 wrappers
inline unsigned int crc32( const void* buff, size_t size, unsigned int crc0=0 ){ return tcrc32<0xedb88320UL>(buff,size,crc0); }
inline unsigned int crc32( sized_ptr_t<void> p, unsigned int crc0=0 ){ return crc32((const void*)p.ptr,p.size); }
// crc32c wrappers
inline unsigned int crc32c( sized_ptr_t<void> p, unsigned int crc0=0 ){ return crc32c((const void*)p.ptr,p.size,crc0); }
inline unsigned int crc32c( const char* s ){ return crc32c(s,strlen(s)); }
inline unsigned int crc32c( const wchar_t* s ){ return crc32c(s,wcslen(s)*sizeof(wchar_t)); }

//***********************************************
// MD5 implementation
struct md5
{
	uint4 digest={0xd98c1dd4,0x4b2008f,0x980980e9,0x7e42f8ec}; // null digest
	explicit md5(const char* str):md5(str,strlen(str)){}
	explicit md5(std::string str):md5(str.c_str(),str.size()){}
	explicit md5(std::wstring wstr):md5(wstr.c_str(),wstr.size()*sizeof(wchar_t)){}
	md5(const void* ptr, size_t size){ if(ptr&&size) update(ptr,size); }
	md5(sized_ptr_t<void> p):md5(p.ptr,p.size){}
	const char* c_str() const { static char buff[64]; unsigned char* u=(unsigned char*)&digest; char* c=buff; for(int k=0;k<16;k++,u++,c+=2)sprintf(c,"%02x",*u); buff[32]=0; return buff; }
	operator uint4() const { return digest; }

private:
	uint32_t a=0x67452301,b=0xefcdab89,c=0x98badcfe,d=0x10325476;
	inline const void* body( const unsigned char* data, size_t size );
	inline void update( const void* data, size_t size );
};

__noinline const void* md5::body( const unsigned char* data, size_t size )
{
	#define MD5_F(x,y,z)				((z)^((x)&((y)^(z))))
	#define MD5_G(x,y,z)				((y)^((z)&((x)^(y))))
	#define MD5_H(x,y,z)				(((x)^(y))^(z))
	#define MD5_H2(x,y,z)				((x)^((y)^(z)))
	#define MD5_I(x,y,z)				((y)^((x)|~(z)))
	#define MD5_STEP(f,a,b,c,d,x,t,s) 	(a)+=f((b),(c),(d))+(*(uint32_t*)&data[(x)*4])+(t);(a)=(((a)<<(s))|(((a)&0xffffffff)>>(32-(s))));(a)+=(b)
	for( uint32_t a0=a,b0=b,c0=c,d0=d; size; data+= 64,size-=64,a0=a+=a0,b0=b+=b0,c0=c+=c0,d0=d+=d0)
	{
		MD5_STEP(MD5_F,a,b,c,d,0, 0xd76aa478,7);MD5_STEP(MD5_F,d,a,b,c,1,  0xe8c7b756,12);MD5_STEP(MD5_F,c,d,a,b,2, 0x242070db,17);MD5_STEP(MD5_F,b,c,d,a,3,  0xc1bdceee,22);
		MD5_STEP(MD5_F,a,b,c,d,4, 0xf57c0faf,7);MD5_STEP(MD5_F,d,a,b,c,5,  0x4787c62a,12);MD5_STEP(MD5_F,c,d,a,b,6, 0xa8304613,17);MD5_STEP(MD5_F,b,c,d,a,7,  0xfd469501,22);
		MD5_STEP(MD5_F,a,b,c,d,8, 0x698098d8,7);MD5_STEP(MD5_F,d,a,b,c,9,  0x8b44f7af,12);MD5_STEP(MD5_F,c,d,a,b,10,0xffff5bb1,17);MD5_STEP(MD5_F,b,c,d,a,11, 0x895cd7be,22);
		MD5_STEP(MD5_F,a,b,c,d,12,0x6b901122,7);MD5_STEP(MD5_F,d,a,b,c,13, 0xfd987193,12);MD5_STEP(MD5_F,c,d,a,b,14,0xa679438e,17);MD5_STEP(MD5_F,b,c,d,a,15, 0x49b40821,22);
		MD5_STEP(MD5_G,a,b,c,d,1, 0xf61e2562,5);MD5_STEP(MD5_G,d,a,b,c,6,  0xc040b340,9); MD5_STEP(MD5_G,c,d,a,b,11,0x265e5a51,14);MD5_STEP(MD5_G,b,c,d,a,0,  0xe9b6c7aa,20);
		MD5_STEP(MD5_G,a,b,c,d,5, 0xd62f105d,5);MD5_STEP(MD5_G,d,a,b,c,10, 0x02441453,9); MD5_STEP(MD5_G,c,d,a,b,15,0xd8a1e681,14);MD5_STEP(MD5_G,b,c,d,a,4,  0xe7d3fbc8,20);
		MD5_STEP(MD5_G,a,b,c,d,9, 0x21e1cde6,5);MD5_STEP(MD5_G,d,a,b,c,14, 0xc33707d6,9); MD5_STEP(MD5_G,c,d,a,b,3, 0xf4d50d87,14);MD5_STEP(MD5_G,b,c,d,a,8,  0x455a14ed,20);
		MD5_STEP(MD5_G,a,b,c,d,13,0xa9e3e905,5);MD5_STEP(MD5_G,d,a,b,c,2,  0xfcefa3f8,9); MD5_STEP(MD5_G,c,d,a,b,7, 0x676f02d9,14);MD5_STEP(MD5_G,b,c,d,a,12, 0x8d2a4c8a,20);
		MD5_STEP(MD5_H,a,b,c,d,5, 0xfffa3942,4);MD5_STEP(MD5_H2,d,a,b,c,8, 0x8771f681,11);MD5_STEP(MD5_H,c,d,a,b,11,0x6d9d6122,16);MD5_STEP(MD5_H2,b,c,d,a,14,0xfde5380c,23);
		MD5_STEP(MD5_H,a,b,c,d,1, 0xa4beea44,4);MD5_STEP(MD5_H2,d,a,b,c,4, 0x4bdecfa9,11);MD5_STEP(MD5_H,c,d,a,b,7, 0xf6bb4b60,16);MD5_STEP(MD5_H2,b,c,d,a,10,0xbebfbc70,23);
		MD5_STEP(MD5_H,a,b,c,d,13,0x289b7ec6,4);MD5_STEP(MD5_H2,d,a,b,c,0, 0xeaa127fa,11);MD5_STEP(MD5_H,c,d,a,b,3, 0xd4ef3085,16);MD5_STEP(MD5_H2,b,c,d,a,6, 0x04881d05,23);
		MD5_STEP(MD5_H,a,b,c,d,9, 0xd9d4d039,4);MD5_STEP(MD5_H2,d,a,b,c,12,0xe6db99e5,11);MD5_STEP(MD5_H,c,d,a,b,15,0x1fa27cf8,16);MD5_STEP(MD5_H2,b,c,d,a,2, 0xc4ac5665,23);
		MD5_STEP(MD5_I,a,b,c,d,0, 0xf4292244,6);MD5_STEP(MD5_I,d,a,b,c,7,  0x432aff97,10);MD5_STEP(MD5_I,c,d,a,b,14,0xab9423a7,15);MD5_STEP(MD5_I,b,c,d,a,5,  0xfc93a039,21);
		MD5_STEP(MD5_I,a,b,c,d,12,0x655b59c3,6);MD5_STEP(MD5_I,d,a,b,c,3,  0x8f0ccc92,10);MD5_STEP(MD5_I,c,d,a,b,10,0xffeff47d,15);MD5_STEP(MD5_I,b,c,d,a,1,  0x85845dd1,21);
		MD5_STEP(MD5_I,a,b,c,d,8, 0x6fa87e4f,6);MD5_STEP(MD5_I,d,a,b,c,15, 0xfe2ce6e0,10);MD5_STEP(MD5_I,c,d,a,b,6, 0xa3014314,15);MD5_STEP(MD5_I,b,c,d,a,13, 0x4e0811a1,21);
		MD5_STEP(MD5_I,a,b,c,d,4, 0xf7537e82,6);MD5_STEP(MD5_I,d,a,b,c,11, 0xbd3af235,10);MD5_STEP(MD5_I,c,d,a,b,2, 0x2ad7d2bb,15);MD5_STEP(MD5_I,b,c,d,a,9,  0xeb86d391,21);
	}
	return data;
}

__noinline void md5::update( const void* data, size_t size )
{
	static unsigned char buffer[64];
	uint32_t lo=0, hi=0, lo0=lo, available, used;
	if((lo=(lo0+size)&0x1fffffff)<lo0)hi++;hi+=uint32_t(size)>>29;
 	used=lo0&0x3f;
 	if(used)
	{
		available = 64-used;
		if(size<available){memcpy(&buffer[used],data,size);return;}
		memcpy(&buffer[used],data,available);
		data=(const unsigned char*)data+available;
		size-=available;
		body(buffer,64);
	}
	if(size>=64){data=body((const unsigned char*)data,size&~(unsigned long)0x3f);size&=0x3f;}
	memcpy(buffer,data,size);
		
	// finalize
	used=lo&0x3f; buffer[used++]=0x80; available=64-used;
	if(available<8){memset(&buffer[used],0,available);body(buffer,64);used=0;available=64;}
	memset(&buffer[used],0,available-8);
	buffer[56]=(lo<<=3);buffer[57]=lo>>8;buffer[58]=lo>>16;buffer[59]=lo>>24;buffer[60]=hi;buffer[61]=hi>>8;buffer[62]=hi>>16;buffer[63]=hi>>24;
	body(buffer,64);
	digest.x=a; digest.y=b; digest.z=c; digest.w=d; // copy back to digest
}

//***********************************************
// augmentation of filesystem
#ifdef __GX_FILESYSTEM_H__
__noinline uint4 path::md5() const
{
	auto p = read_file<void>();
	if(!p.ptr) return ::md5(nullptr,0);
	::md5 m(p); safe_free(p.ptr);
	return m.digest;
}
__noinline uint path::crc32c() const
{
	auto p=read_file<void>();
	if(!p.ptr) return 0; uint c=::crc32c(p);
	safe_free(p.ptr);
	return c;
}
#endif // __GX_FILESYSTEM_H__

//***********************************************
#endif // __GX_MEMORY__
