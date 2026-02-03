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

#if __has_include("gxmath.h")
	#include "gxmath.h"
#endif
#if __has_include("gxfilesystem.h")
	#include "gxfilesystem.h"
#endif
#if __has_include(<nmmintrin.h>)
	#include <nmmintrin.h>
#endif

struct mem_t : sized_ptr_t<void>
{
	__forceinline mem_t(){ ptr=nullptr; size=0; }
	__forceinline mem_t( size_t Size, const void* Ptr=nullptr ){ ptr=(void*)Ptr; size=Size; }
	virtual ~mem_t(){}
	__forceinline void* alloc( size_t _size, ptrdiff_t elems=1 ){ return ptr=malloc(size=_size*elems); }
	__forceinline void free(){ if(ptr) ::free(ptr); size=0; }
	__forceinline void* cpy( void* src, size_t _size ){ return memcpy(ptr,src,_size); }
	__forceinline void* dup( void* src, size_t _size ){ ptr=malloc(size=_size); if(ptr) memcpy(ptr,src,_size); return ptr; }
	__forceinline int cmp( void* src, size_t _size ){ return memcmp(ptr,src,_size); }
	__forceinline int strcmp( const char* src ){ return ::memcmp((char*)ptr,src,::strlen(src)); }
	__forceinline int wcscmp( const wchar_t* src ){ return ::memcmp((wchar_t*)ptr,src,::wcslen(src)*sizeof(wchar_t)); }
	__forceinline bool operator==( const mem_t& m ) const { if(size!=m.size) return false; return memcmp(ptr,m.ptr,size)==0; }
	__forceinline operator bool() const { return ptr!=nullptr&&size>0; }
	template <class T> __forceinline operator T* (){ return (T*) ptr; }
};

template <class T,bool readonly=false> struct mmap // memory-mapped file (similar to virtual memory)
{
	T*		ptr=nullptr; // last mapped ptr
	size_t	size=0;
	
	mmap( size_t n ):size(n){ size_t s=size*sizeof(T); hFileMap=CreateFileMappingW( INVALID_HANDLE_VALUE /* use pagefile */, nullptr, PAGE_READWRITE, DWORD(s>>32), DWORD(s&0xffffffff), _uname() ); }
	mmap( const char* file_path){ struct _stat st={}; if(access(file_path,0)!=0) return; _stat(file_path,&st); size_t file_size=st.st_size; size=file_size/sizeof(T); if(file_size==0) return; hFile=CreateFileW(atow(file_path),GENERIC_READ|(readonly?0:GENERIC_WRITE), FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, nullptr); if(hFile==INVALID_HANDLE_VALUE){size=0;return;} size_t memsize=sizeof(T)*size; hFileMap=CreateFileMappingW(hFile,nullptr,readonly?PAGE_READONLY:PAGE_READWRITE,DWORD(uint64_t(memsize)>>32),DWORD(memsize&0xffffffff),nullptr); if(hFileMap==INVALID_HANDLE_VALUE){size=0;CloseHandle(hFile);hFile=nullptr;return;} }
	virtual ~mmap(){ if(!empty()&&ptr){ FlushViewOfFile(ptr,0); UnmapViewOfFile(ptr); ptr=nullptr; } if(hFileMap!=INVALID_HANDLE_VALUE) CloseHandle(hFileMap); hFileMap=INVALID_HANDLE_VALUE; if(hFile!=INVALID_HANDLE_VALUE) CloseHandle(hFile); hFile=INVALID_HANDLE_VALUE; }
	bool empty() const { return hFile==INVALID_HANDLE_VALUE||hFileMap==INVALID_HANDLE_VALUE||size==0; }
	T* map( size_t offset=0, size_t n=0 ){ if(empty()) return nullptr; size_t s=offset*sizeof(T); return ptr=(T*)MapViewOfFile(hFileMap,FILE_MAP_READ|(readonly?0:FILE_MAP_WRITE),DWORD(uint64_t(s)>>32),DWORD(s&0xffffffff),(n?n:size)*sizeof(T)); }
	void unmap( T* p=nullptr ){ T* t=p?p:ptr; if(empty()||!t) return; FlushViewOfFile(t,0); UnmapViewOfFile(t); }

protected:
	HANDLE hFile = INVALID_HANDLE_VALUE;		// handle to the file (INVALID_HANDLE_VALUE means pagefile)
	HANDLE hFileMap = INVALID_HANDLE_VALUE;		// handle to the file mapping
	static const char* _uname(){ static char n[256]; SYSTEMTIME s; GetSystemTime(&s); snprintf( n, _countof(n), "%p%02d%02d%02d%02d%04d%05d", GetCurrentThreadId(), s.wDay, s.wHour, s.wMinute, s.wSecond, s.wMilliseconds, rand() ); return n; } // make unique file name
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

struct zipentry_t : public ZIPENTRY
{
	size_t	size=0;
	void*	ptr=nullptr;
	bool is_dir() const { return (attr&FILE_ATTRIBUTE_DIRECTORY)!=0; }
};

struct izip_t	// common interface to zip, 7zip, ...
{
	path_t file_path;
	vector<zipentry_t> entries;

	virtual ~izip_t(){}
	virtual void release()=0;
	virtual bool load()=0;
	virtual bool extract_to_files( path dir, const char* name=nullptr )=0;	// if name==nullptr, extract all files. otherwise, extract a single file with the name
	virtual zipentry_t* extract_to_memory( const char* name=nullptr )=0;		// if name==nullptr, extract all files and return the first pointer. otherwise, extract a single file with the name and returns the pointer
	virtual zipentry_t* find( const char* name ){ wstring w=atow(name); for(auto& e:entries){ if(wcsicmp(e.name,w.c_str())==0) return &e; } return nullptr; }
};

// signature detection
inline bool is_7zip_signature( void* ptr, size_t size=0 )
{
	static constexpr unsigned char szip[]={'7','z',0xBC,0xAF,0x27,0x1C};
	if(size&&size<sizeof(szip)) return false;
	return memcmp(ptr,szip,sizeof(szip))==0;
}

inline bool is_zip_signature( void* ptr, size_t size=0 )
{
	static constexpr unsigned char pk[]		= {0x50,0x4b,0x03,0x04};
	static constexpr unsigned char pklite[]	= {0x50,0x4B,0x4C,0x49,0x54,0x45};
	static constexpr unsigned char pksfx[]	= {0x50,0x4B,0x53,0x70,0x58};
	static constexpr unsigned char winzip[]	= {0x57,0x69,0x6E,0x5A,0x69,0x70};
	if(size&&size<6) return false;
	if(memcmp(ptr,pk,sizeof(pk))==0)			return true;
	if(memcmp(ptr,pklite,sizeof(pklite))==0)	return true;
	if(memcmp(ptr,pksfx,sizeof(pksfx))==0)		return true;
	if(memcmp(ptr,winzip,sizeof(winzip))==0)	return true;
	return false;
}

inline bool is_7zip_file( const path_t& file_path )
{
	FILE* fp = fopen(file_path.c_str(),"rb"); if(!fp) return false;
	_fseeki64(fp,0,SEEK_END); size_t size=size_t(_ftelli64(fp)); _fseeki64(fp,0,SEEK_SET);
	unsigned char t[6]={}; if(size>=sizeof(t)) fread(t,1,sizeof(t),fp); fclose(fp);
	return is_7zip_signature(t,size);
}

inline bool is_zip_file( const path_t& file_path )
{
	FILE* fp = fopen(file_path.c_str(),"rb"); if(!fp) return false;
	_fseeki64(fp,0,SEEK_END); size_t size=size_t(_ftelli64(fp)); _fseeki64(fp,0,SEEK_SET);
	unsigned char t[6]={}; if(size>=sizeof(t)) fread(t,1,sizeof(t),fp); fclose(fp);
	return is_zip_signature(t,size);
}

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
	bool load(){ if(!hres){ printf("resource_t::%s(): hres==nullptr\n",__func__); return false; } if(!(hmem=LoadResource(hModule,hres))) return false; size=SizeofResource(hModule,hres); ptr=LockResource(hmem); return size!=0; }
	bool load( LPCWSTR lpName ){ return find(lpName)&&load(); }
	bool load( int res_id ){ return find(res_id)&&load(); }

	// loading for specific types
	string load_string(){ if(type!=MAKEINTRESOURCEW(6/*string 6*/)||!load()) return ""; wstring w; w.resize(size/sizeof(decltype(w)::value_type)); memcpy((void*)w.c_str(),ptr,size); return wtoa(w.c_str()); }
	izip_t* load_zip( LPCWSTR lpName ){ if(!find(lpName)) return nullptr; return load_zip(); }
	izip_t* load_zip( int res_id ){ if(!find(res_id)) return nullptr; return load_zip(); }
	izip_t* load_zip();
};
#endif // MAKEINTRESOURCEW

//***********************************************
// crc32c wrappers
#ifdef __msvc__
// CRC32C SSE4.2 implementation up to 8-batch parallel construction (https://github.com/Voxer/sse4_crc32)
__noinline unsigned int crc32c_hw( unsigned int crc0, const void* ptr, size_t size )
{
	if(!ptr||!size) return crc0; const unsigned char* b = (const unsigned char*) ptr;
#if defined(_M_X64)||defined(__LP64__)
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
inline bool __has_sse42(){ static bool b=false; if(!b){int r[4]={};__cpuid(r,1);b=(((r[2]>>20)&1)==1);} return b; }
inline unsigned int crc32c( unsigned int crc0, const void* ptr, size_t size ){ static unsigned int(*pcrc32c)(unsigned int,const void*,size_t)=__has_sse42()?crc32c_hw:crc32<0x82f63b78UL>; return ptr&&size?pcrc32c(crc0,ptr,size):crc0; }
#else
inline unsigned int crc32c( unsigned int crc0, const void* ptr, size_t size ){ return ptr&&size?crc32<0x82f63b78UL>(crc0,ptr,size):crc0; }
#endif
inline unsigned int crc32c( unsigned int crc0, const sized_ptr_t<void>& p ){ return p.ptr&&p.size?crc32c(crc0,(const void*)p.ptr,p.size):crc0; }
inline unsigned int crc32c( unsigned int crc0, const char* s ){ return s&&*s?crc32c(crc0,(const void*)s,strlen(s)):crc0; }
inline unsigned int crc32c( unsigned int crc0, const wchar_t* s ){ return s&&*s?crc32c(crc0,(const void*)s,wcslen(s)):crc0; }
inline unsigned int crc32c( unsigned int crc0, const string& s ){ return s.empty()?crc0:crc32c(crc0,(const void*)s.c_str(),s.size()); }
inline unsigned int crc32c( unsigned int crc0, const wstring& s ){ return s.empty()?crc0:crc32c(crc0,(const void*)s.c_str(), s.size()*sizeof(wchar_t)); }
template <class T> inline unsigned int crc32c( unsigned int crc0, const vector<T>& v ){ return v.empty()?crc0:crc32c(crc0,v.data(),v.size()*sizeof(T)); }
template <class T> inline unsigned int crc32c( unsigned int crc0, const vector<T>* v ){ return !v||v->empty()?crc0:crc32c(crc0,v->data(),v->size()*sizeof(T)); }
template <class T, size_t N> inline unsigned int crc32c( unsigned int crc0, const array<T,N>& v ){ return v.empty()?crc0:crc32c(crc0,v.data(),v.size()*sizeof(T)); }
template <class T, size_t N> inline unsigned int crc32c( unsigned int crc0, const array<T,N>* v ){ return !v||v->empty()?crc0:crc32c(crc0,v.data(),v.size()*sizeof(T)); }

inline unsigned int crc32c( const void* ptr, size_t size ){ return crc32c(0,ptr,size); }
inline unsigned int crc32c( const sized_ptr_t<void>& p ){ return crc32c(0,p); }
inline unsigned int crc32c(	const char* s ){ return crc32c(0,s); }
inline unsigned int crc32c(	const wchar_t* s ){ return crc32c(0,s); }
inline unsigned int crc32c(	const string& s ){ return crc32c(0,s); }
inline unsigned int crc32c(	const wstring& s ){ return crc32c(0,s); }

template <class T> inline unsigned int crc32c( const vector<T>& v ){ return crc32c<T>(0,v); }
template <class T> inline unsigned int crc32c( const vector<T>* v ){ return crc32c<T>(0,v); }
template <class T, size_t N> inline unsigned int crc32c( const array<T,N>& v ){ return crc32c<T,N>(v); }
template <class T, size_t N> inline unsigned int crc32c( const array<T,N>* v ){ return crc32c<T,N>(v); }

//***********************************************
// MD5 implementation
struct md5
{
	uint4 digest={0xd98c1dd4,0x4b2008f,0x980980e9,0x7e42f8ec}; // null digest
	explicit md5(const char* str):md5(str,strlen(str)){}
	explicit md5(string str):md5(str.c_str(),str.size()){}
	md5(const void* ptr, size_t size){ if(ptr&&size) update(ptr,size); }
	md5(sized_ptr_t<void> p):md5(p.ptr,p.size){}
	const char* c_str() const { static char buff[64]; unsigned char* u=(unsigned char*)&digest; char* c=buff; for(int k=0;k<16;k++,u++,c+=2)sprintf(c,"%02x",*u); buff[32]=0; return buff; }
	operator uint4() const { return digest; }

private:
	uint32_t a=0x67452301,b=0xefcdab89,c=0x98badcfe,d=0x10325476;
	inline const void* body( const unsigned char* data, size_t size );
	void update( const void* data, size_t size );
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
	memset(&buffer[used],0,size_t(available-8));
	buffer[56]=(lo<<=3);buffer[57]=lo>>8;buffer[58]=lo>>16;buffer[59]=lo>>24;buffer[60]=hi;buffer[61]=hi>>8;buffer[62]=hi>>16;buffer[63]=hi>>24;
	body(buffer,64);
	digest.x=a; digest.y=b; digest.z=c; digest.w=d; // copy back to digest
}

//***********************************************
// augmentation of filesystem
#ifdef __GX_FILESYSTEM_H__
__noinline uint path::crc() const
{
	FILE* fp=fopen("rb"); if(!fp) return 0;
	size_t bs=min(::file_size(fp),size_t(1<<16)); if(bs==0){ fclose(fp); return 0; }
	char* buff=(char*)malloc(bs); uint c=0; size_t r=0; if(buff){ while(r=fread(buff,1,bs,fp)) c=::crc32c(c,buff,r); }
	fclose(fp); if(buff) free(buff);
	return c;
}
#endif // __GX_FILESYSTEM_H__

//*************************************
namespace base64 {
//*************************************

__noinline const char* __encoding_table()
{
	static char t[64]={}; if(*t) return t;
	char *v=t;
	for(int k=0;k<26;k++)*v++='A'+k;
	for(int k=0;k<26;k++)*v++='a'+k;
	for(int k=0;k<10;k++)*v++='0'+k;
	t[62]='+';t[63]='/';
	return t;
}

__noinline const char* __decoding_table()
{
	static char t[256]={}; if(*t) return t;
	for(int k=0;k<256;k++)t[k]=char(-1);
	int v=0;
	for(char k='A';k<='Z';k++)t[k]=v++;
	for(char k='a';k<='z';k++)t[k]=v++;
	for(char k='0';k<='9';k++)t[k]=v++;
	t['+']=62;t['/']=63;
	return t;
}

__noinline string encode( const void* ptr, size_t size )
{
	struct char4 { char x, y, z, w; }; // local temporary type
	static const char* table = __encoding_table();
	static const auto encode3 = []( const char* t, uchar a, uchar b, uchar c )->char4
	{
		// fill four zero-padded 6 bits in dst[0..3]
		return char4{ t[(a&0xFC)>>2], t[((a&0x3)<<4)|((b&0xF0)>>4)], t[((b&0xF)<<2)|((c&0xC0)>>6)], t[c&0x3F] };
	};

	if(size==0) return "";
	uchar *src=(uchar*)ptr;
	size_t mod = size%3;
	vector<char4> v((size/3+mod)+1);
	char4* dst=v.data();

	// out-of-bound values are zero-filled
	for( int k=0,kn=int(size)-3; k<kn; k+=3, src+=3, dst++ ) *dst = encode3( table, src[0], src[1], src[2] );
	*dst = encode3( table, src[0], mod==1?0:src[1], mod?0:src[2] );
	if(mod)dst->w='=';if(mod==1)dst->z='='; // padding
	*++dst={0};

	return string((char*)&v.front());
}

template <class T=void>
__noinline sized_ptr_t<T> decode( const string& encoded )
{
	sized_ptr_t<T> m = {};
	if(encoded.empty()) return m;
	if(encoded.size()%4!=0){ printf( "%s(): encoded.size()%%4!=0\n", __func__ ); return m; }

	const char* table = __decoding_table();
	size_t el = encoded.size();
	m.size = el/4*3; if(encoded[el-1]=='=') m.size--; if(encoded[el-2]=='=') m.size--;
	m.ptr = (T*) malloc(m.size+1); ((char*)m.ptr)[m.size]=0; // allocate 1 more byte for encoded string 
	uchar3*	dst = (uchar3*) m.ptr;
	uchar* s = (uchar*) encoded.c_str();
	for( size_t k=0, kn=encoded.size(); k<kn; k+=4, s+=4, dst++ )
	{
		char a=table[s[0]]; if(a<0) continue;
		dst->x = (a<<2)|((table[s[1]]&0x30)>>4);
		if(s[2]!='=') dst->y = (((table[s[1]]&0xf)<<4)|((table[s[2]]&0x3c)>>2));
		if(s[3]!='=') dst->z = (((table[s[2]]&0x03)<<6)|table[s[3]]);
	}
	return m;
}

//*************************************
} // namespace base64
//*************************************

//***********************************************
#endif // __GX_MEMORY__
