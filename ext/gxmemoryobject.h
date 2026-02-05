#pragma once
#ifndef __GX_MEMORYOBJECT_H__
#define __GX_MEMORYOBJECT_H__

#if __has_include(<gxut/gxopengl.h>)
	#include <gxut/gxopengl.h>
#elif __has_include("gxopengl.h")
	#include "gxopengl.h"
#else
	#error unable to include gxopengl.h
#endif

// constants
#ifndef GL_HANDLE_TYPE_OPAQUE_WIN32_EXT
	#define GL_HANDLE_TYPE_OPAQUE_WIN32_EXT 0x9587
	#define GL_HANDLE_TYPE_OPAQUE_WIN32_KMT_EXT 0x9588
	#define GL_HANDLE_TYPE_D3D12_TILEPOOL_EXT 0x9589
	#define GL_HANDLE_TYPE_D3D12_RESOURCE_EXT 0x958A
	#define GL_HANDLE_TYPE_D3D11_IMAGE_EXT 0x958B
	#define GL_HANDLE_TYPE_D3D11_IMAGE_KMT_EXT 0x958C
	#define GL_HANDLE_TYPE_D3D12_FENCE_EXT 0x9594
#endif

// function prototypes
typedef void (*PFNGLCREATEMEMORYOBJECTSEXTPROC)(GLsizei n, GLuint *memoryObjects);
typedef void (*PFNGLDELETEMEMORYOBJECTSEXTPROC)(GLsizei n, const GLuint *memoryObjects);
typedef void (*PFNGLIMPORTMEMORYWIN32HANDLEEXTPROC)(GLuint memory, GLuint64 size, GLenum handleType, void *handle);
typedef GLboolean (*PFNGLACQUIREKEYEDMUTEXWIN32EXTPROC)(GLuint  memory, GLuint64  key, GLuint  timeout);
typedef GLboolean (*PFNGLRELEASEKEYEDMUTEXWIN32EXTPROC)(GLuint  memory, GLuint64  key);

// function pointers
inline PFNGLCREATEMEMORYOBJECTSEXTPROC glCreateMemoryObjectsEXT=nullptr;
inline PFNGLDELETEMEMORYOBJECTSEXTPROC glDeleteMemoryObjectsEXT=nullptr;
inline PFNGLIMPORTMEMORYWIN32HANDLEEXTPROC glImportMemoryWin32HandleEXT=nullptr;
inline PFNGLACQUIREKEYEDMUTEXWIN32EXTPROC glAcquireKeyedMutexWin32EXT=nullptr;
inline PFNGLRELEASEKEYEDMUTEXWIN32EXTPROC glReleaseKeyedMutexWin32EXT=nullptr;

//***********************************************
namespace gl {
//***********************************************

struct MemoryObject : public Object
{
	MemoryObject( GLuint ID, const char* name ):Object(ID,name,0){}
	~MemoryObject(){ if(ID) glDeleteMemoryObjectsEXT(1,&ID); }
	bool acquire_keyed_mutex( GLuint64 key, GLuint timeout ){ return glAcquireKeyedMutexWin32EXT(ID,key,timeout)!=0; }
	bool release_keyed_mutex( GLuint64 key ){ return glReleaseKeyedMutexWin32EXT(ID,key)!=0; }
};

//***********************************************
} // end namespace gl
//***********************************************

__noinline gl::MemoryObject* gxCreateMemoryObject( const char* name, void* handle, size_t size, GLenum handle_type=GL_HANDLE_TYPE_D3D11_IMAGE_EXT )
{
	if(!name||!*name){ printf("%s(): name==nullptr\n", __func__); return nullptr; }
	if(!handle){ printf("%s(): handle==nullptr\n", __func__); return nullptr; }
	if(!size){ printf("%s(): size==0\n", __func__); return nullptr; }
	
	// load extensions
	if(!glCreateMemoryObjectsEXT&&!(glCreateMemoryObjectsEXT=gl::get_proc_address<decltype(glCreateMemoryObjectsEXT)>("glCreateMemoryObjectsEXT"))) return nullptr;
	if(!glDeleteMemoryObjectsEXT&&!(glDeleteMemoryObjectsEXT=gl::get_proc_address<decltype(glDeleteMemoryObjectsEXT)>("glDeleteMemoryObjectsEXT"))) return nullptr;
	if(!glImportMemoryWin32HandleEXT&&!(glImportMemoryWin32HandleEXT=gl::get_proc_address<decltype(glImportMemoryWin32HandleEXT)>("glImportMemoryWin32HandleEXT"))) return nullptr;
	if(!glAcquireKeyedMutexWin32EXT&&!(glAcquireKeyedMutexWin32EXT=gl::get_proc_address<decltype(glAcquireKeyedMutexWin32EXT)>("glAcquireKeyedMutexWin32EXT"))) return nullptr;
	if(!glReleaseKeyedMutexWin32EXT&&!(glReleaseKeyedMutexWin32EXT=gl::get_proc_address<decltype(glReleaseKeyedMutexWin32EXT)>("glReleaseKeyedMutexWin32EXT"))) return nullptr;

	// create memory object
	GLuint ID; glCreateMemoryObjectsEXT(1, &ID); if(!ID){ printf("%s(): memory_object==0\n", __func__); return nullptr; }

	// import handle
	GLenum e0=glGetError(); // previous error from others
	// [CAUTION] memory size should be larger than actual size (e.g., 2 times or 1M bytes)
	glImportMemoryWin32HandleEXT( ID, GLuint64(size+(1<<20)), handle_type, handle );
	GLenum e1=glGetError(); // current error
	if(e1!=GL_NO_ERROR&&e1!=e0){ printf("%s(%s): error %s\n", __func__, "name", gxGetErrorString(e1)); return nullptr; }

	// create an instance
	return new gl::MemoryObject(ID,name);
}

__noinline gl::Texture* gxCreateTexture2DFromMemory( const char* name, GLsizei width, GLsizei height, GLuint memory_object, GLint internal_format=GL_RGBA8 )
{
	typedef void(*PFNGLTEXTURESTORAGEMEM2DEXTPROC)(GLuint texture,GLsizei levels,GLenum internalFormat,GLsizei width,GLsizei height,GLuint memory,GLuint64 offset);
	static auto glTextureStorageMem2DEXT=gl::get_proc_address<PFNGLTEXTURESTORAGEMEM2DEXTPROC>("glTextureStorageMem2DEXT"); if(!glTextureStorageMem2DEXT){ printf("%s(): glTextureStorageMem2DEXT==nullptr\n",__func__); return nullptr; }

	if(!gxIsSizedInternalFormat(internal_format)){ printf( "%s(): internal_format must use a sized format instead of GL_RED, GL_RG, GL_RGB, GL_RGBA.\n", __func__ ); return nullptr; }

	GLenum target = GL_TEXTURE_2D;
	GLuint ID = gxCreateTexture(target); if(ID==0) return nullptr;
	GLenum format = gxGetTextureFormat( internal_format );
	GLenum type = gxGetTextureType( internal_format );

	gl::Texture* texture = new gl::Texture(ID,name,target,internal_format,format,type); if(width==0||height==0) return texture;
	texture->_key = gl::Texture::crc(0,1,0,1,target,false);
	glBindTexture( target, ID );

	// allocate storage: at this point, lock is not needed
	GLenum e0 = glGetError(); // previous error from others
	glTextureStorageMem2DEXT( ID, 1, GL_RGBA8, width, height, memory_object, 0 );
	GLenum e1 = glGetError(); // texture error
	if(e1!=GL_NO_ERROR&&e1!=e0){ printf( "%s(%s): error %s\n", __func__, name, 	gxGetErrorString(e1) ); delete texture; return nullptr; }

	// set dimensions
	texture->_width		= width;
	texture->_height	= height;
	texture->_layers	= 1;
	texture->_levels	= 1;

	// attributes
	glTexParameteri( target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri( target, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( target, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

	glBindTexture( target, 0 );
	return texture;
}

#endif // __GX_MEMORYOBJECT_H__
