//*************************************
// Copyright 2011-2040 Sungkil Lee
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
//*************************************

#pragma once
#ifndef __GX_OPENGL_H__
#define __GX_OPENGL_H__

#include "gxmath.h"
#include "gxstring.h"
#include <malloc.h> // _get_heap_handle()
#include <chrono> // for timer

#if defined(GX_OPENGL_GLEW)
	#ifdef GX_OPENGL_GLAD
		#error do not define GX_OPENGL_GLAD and GX_OPENGL_GLEW simultaneously
	#endif
	#ifndef GLEW_STATIC
		#define GLEW_STATIC
	#endif
	#if __has_include(<GL/glew.h>)
		#include <GL/glew.h>
	#elif __has_include("glew.h")
		#include "glew.h"
	#endif
	#pragma comment( lib, "glew32s" )	// explicit linking to static glew32 x64
	#pragma comment( lib, "OpenGL32" )	// explicit linking to system OpenGL
	#define gxHasExtension(name)	GLEW_ARB_##name
#elif defined(GX_OPENGL_GLAD)
	#if __has_include(<GL/glad/gl.h>)
		#include <GL/glad/gl.h>
	#elif __has_include(<GL/glad.h>)
		#include <GL/glad.h>
	#elif __has_include("glad.h")
		#include "glad.h"
	#endif
	#define gxHasExtension(name)	GLAD_GL_ARB_##name
#else
	#if __has_include(<gxut/gxcorearb.h>)
		#include <gxut/gxcorearb.h>
	#elif __has_include(<GL/gxcorearb.h>)
		#include <GL/gxcorearb.h>
	#elif __has_include("gxcorearb.h")
		#include "gxcorearb.h"
	#endif
	#define gxHasExtension(name)	GX_ARB_##name
#endif
	
#ifdef GX_OPENGL_GLFX
	#if __has_include(<GL/glfx.h>)
		#include <GL/glfx.h>
	#elif __has_include(<glfx/glfx.h>)
		#include <glfx/glfx.h>
	#elif __has_include("glfx.h")
		#include "glfx.h"
	#endif
#endif

#if defined(GX_CUDA_GL_INTEROP) && __has_include(<cuda_gl_interop.h>)
	#pragma warning( disable: 4819 )	// disable waring on the foreign codepage
	#include <cuda_gl_interop.h>		// this should be located after glew/glad/gxcorearb.h
	#pragma warning( default: 4819 )
#endif

// GCC compatibility
#ifdef __gcc__
	uint64_t _get_heap_handle(){ return 0; }
#endif

//***********************************************
// binding query targets
inline GLenum gxGetTargetBinding( GLenum target )
{
	const static std::unordered_map<GLenum,GLenum> t =
	{
		{GL_ARRAY_BUFFER,GL_ARRAY_BUFFER_BINDING},
		{GL_ELEMENT_ARRAY_BUFFER,GL_ELEMENT_ARRAY_BUFFER_BINDING},
		{GL_DRAW_INDIRECT_BUFFER,GL_DRAW_INDIRECT_BUFFER_BINDING},
		{GL_SHADER_STORAGE_BUFFER,GL_SHADER_STORAGE_BUFFER_BINDING},
		{GL_UNIFORM_BUFFER,GL_UNIFORM_BUFFER_BINDING},
		{GL_PIXEL_PACK_BUFFER,GL_PIXEL_PACK_BUFFER_BINDING},
		{GL_PIXEL_UNPACK_BUFFER,GL_PIXEL_UNPACK_BUFFER_BINDING},
		{GL_ATOMIC_COUNTER_BUFFER,GL_ATOMIC_COUNTER_BUFFER_BINDING},
		{GL_DISPATCH_INDIRECT_BUFFER,GL_DISPATCH_INDIRECT_BUFFER_BINDING},
		{GL_QUERY_BUFFER,GL_QUERY_BUFFER_BINDING},
		{GL_TRANSFORM_FEEDBACK_BUFFER,GL_TRANSFORM_FEEDBACK_BUFFER_BINDING},
		{GL_COPY_READ_BUFFER,GL_COPY_READ_BUFFER_BINDING},
		{GL_COPY_WRITE_BUFFER,GL_COPY_WRITE_BUFFER_BINDING},
		{GL_PARAMETER_BUFFER,GL_PARAMETER_BUFFER_BINDING},
		{GL_TEXTURE_BUFFER,GL_TEXTURE_BUFFER_BINDING},
		{GL_TEXTURE_1D,GL_TEXTURE_BINDING_1D},
		{GL_TEXTURE_2D,GL_TEXTURE_BINDING_2D},
		{GL_TEXTURE_2D_MULTISAMPLE,GL_TEXTURE_BINDING_2D_MULTISAMPLE},
		{GL_TEXTURE_3D,GL_TEXTURE_BINDING_3D},
		{GL_TEXTURE_CUBE_MAP,GL_TEXTURE_BINDING_CUBE_MAP},
		{GL_TEXTURE_CUBE_MAP_ARRAY,GL_TEXTURE_BINDING_CUBE_MAP_ARRAY},
		{GL_TEXTURE_1D_ARRAY,GL_TEXTURE_BINDING_1D_ARRAY},
		{GL_TEXTURE_2D_ARRAY,GL_TEXTURE_BINDING_2D_ARRAY},
		{GL_TEXTURE_2D_MULTISAMPLE_ARRAY,GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY},
		{GL_TEXTURE_RECTANGLE,GL_TEXTURE_BINDING_RECTANGLE},
		{GL_TRANSFORM_FEEDBACK,GL_TRANSFORM_FEEDBACK_BINDING},
		{GL_PROGRAM,GL_CURRENT_PROGRAM},
		{GL_VERTEX_ARRAY,GL_VERTEX_ARRAY_BINDING},
		{GL_FRAMEBUFFER,GL_FRAMEBUFFER_BINDING},
		{GL_RENDERBUFFER,GL_RENDERBUFFER_BINDING},
		// ignore query buffer targets
		{GL_ANY_SAMPLES_PASSED,0},
		{GL_ANY_SAMPLES_PASSED_CONSERVATIVE,0},
		{GL_PRIMITIVES_GENERATED,0},
		{GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN,0},
		{GL_TIMESTAMP,0},
		{GL_TIME_ELAPSED,0},
	};
	auto it=t.find(target); if(it!=t.end()) return it->second;
	if(target) printf( "%s(): unable to find target binding for 0x%04X\n",__func__,target);
	return 0;
}

//***********************************************
// query utitlities
inline GLuint	gxGetBinding( GLenum target ){ GLint iv; glGetIntegerv(gxGetTargetBinding(target),&iv); return iv; }
inline GLint	gxGetProgramiv( GLuint program, GLenum pname ){ GLint iv; glGetProgramiv( program, pname, &iv); return iv; }
inline ivec3	gxGetProgramiv3( GLuint program, GLenum pname ){ ivec3 iv; glGetProgramiv( program, pname, &iv.x); return iv; }
inline GLint	gxGetActiveUniformBlockiv( GLuint program, GLuint uniformBlockIndex, GLenum pname ){ GLint i; glGetActiveUniformBlockiv( program, uniformBlockIndex, pname, &i ); return i; }
inline GLint	gxGetShaderiv( GLuint shader, GLenum pname ){ GLint iv; glGetShaderiv( shader, pname, &iv); return iv; }
inline GLint	gxGetIntegerv( GLenum pname ){ GLint iv; glGetIntegerv( pname, &iv ); return iv; }
inline bool		gxGetBooleanv( GLenum pname ){ GLboolean bv; glGetBooleanv( pname, &bv ); return bv!=0; }
inline GLint64	gxGetInteger64v( GLenum pname ){ GLint64 iv; glGetInteger64v( pname, &iv ); return iv; }
inline GLfloat	gxGetFloatv( GLenum pname ){ GLfloat fv; glGetFloatv( pname, &fv ); return fv; }
inline GLint	gxGetInternalFormativ( GLenum target, GLint internal_format, GLenum pname ){ GLint iv; glGetInternalformativ( target, internal_format, pname, 1, &iv ); return iv; }
inline bool		gxIsSizedInternalFormat( GLint internal_format ){ switch(internal_format){ case GL_RED: case GL_RG: case GL_RGB: case GL_RGBA: return false; default: return true; } }
inline GLenum	gxGetTextureFormat( GLint internal_format ){ switch( internal_format ){ case GL_R8: case GL_R16: case GL_R16F: case GL_R32F: return GL_RED; case GL_R8I: case GL_R16I: case GL_R32I: case GL_R8UI: case GL_R16UI: case GL_R32UI: return GL_RED_INTEGER; /**/ case GL_RG: case GL_RG8: case GL_RG16: case GL_RG16F: case GL_RG32F: return GL_RG; case GL_RG8I: case GL_RG16I: case GL_RG32I: case GL_RG8UI: case GL_RG16UI: case GL_RG32UI: return GL_RG_INTEGER; /**/ case GL_RGB: case GL_RGB8: case GL_RGB16: case GL_RGB16F: case GL_RGB32F: return GL_RGB; case GL_RGB8I: case GL_RGB16I: case GL_RGB32I: case GL_RGB8UI: case GL_RGB16UI: case GL_RGB32UI: return GL_RGB_INTEGER; /**/ case GL_RGBA: case GL_RGBA8: case GL_RGBA16: case GL_RGBA16F: case GL_RGBA32F: return GL_RGBA; case GL_RGBA8I: case GL_RGBA16I: case GL_RGBA32I: case GL_RGBA8UI: case GL_RGBA16UI: case GL_RGBA32UI: return GL_RGBA_INTEGER; } return 0; }
inline GLenum	gxGetTextureType( GLint internal_format ){ switch( internal_format ){ case GL_R8UI: case GL_RG8UI: case GL_RGB8UI: case GL_RGBA8UI: case GL_R8: case GL_RG8: case GL_RGB8: case GL_RGBA8: return GL_UNSIGNED_BYTE; /**/ case GL_R8I: case GL_RG8I: case GL_RGB8I: case GL_RGBA8I: return GL_BYTE; /**/ case GL_R16: case GL_RG16: case GL_RGB16: case GL_RGBA16: return GL_HALF_FLOAT; /**/ case GL_R16UI: case GL_RG16UI: case GL_RGB16UI: case GL_RGBA16UI: return GL_UNSIGNED_SHORT; /**/ case GL_R16I: case GL_RG16I: case GL_RGB16I: case GL_RGBA16I: return GL_SHORT; /**/ case GL_R16F: case GL_RG16F: case GL_RGB16F: case GL_RGBA16F: return GL_HALF_FLOAT; /**/ case GL_R32F: case GL_RG32F: case GL_RGB32F: case GL_RGBA32F: return GL_FLOAT; /**/ case GL_R32UI: case GL_RG32UI: case GL_RGB32UI: case GL_RGBA32UI: return GL_UNSIGNED_INT; /**/ case GL_R32I: case GL_RG32I: case GL_RGB32I: case GL_RGBA32I: return GL_INT;	} return 0; }
inline GLenum	gxGetInternalFormat( GLint type, int channels ){ static const uint a=1u<<16u,b=2u<<16u,c=3u<<16u,d=4u<<16u; static std::map<uint,GLenum> ifm = { {a+GL_BYTE,GL_R8}, {b+GL_BYTE,GL_RG8}, {c+GL_BYTE,GL_RGB8}, {d+GL_BYTE,GL_RGBA8}, {a+GL_UNSIGNED_BYTE,GL_R8}, {b+GL_UNSIGNED_BYTE,GL_RG8}, {c+GL_UNSIGNED_BYTE,GL_RGB8}, {d+GL_UNSIGNED_BYTE,GL_RGBA8}, {a+GL_SHORT,GL_R16I}, {b+GL_SHORT,GL_RG16I}, {c+GL_SHORT,GL_RGB16I}, {d+GL_SHORT,GL_RGBA16I}, {a+GL_UNSIGNED_SHORT,GL_R16UI}, {b+GL_UNSIGNED_SHORT,GL_RG16UI}, {c+GL_UNSIGNED_SHORT,GL_RGB16UI}, {d+GL_UNSIGNED_SHORT,GL_RGBA16UI}, {a+GL_INT,GL_R32I}, {b+GL_INT,GL_RG32I}, {c+GL_INT,GL_RGB32I}, {d+GL_INT,GL_RGBA32I}, {a+GL_UNSIGNED_INT,GL_R32UI}, {b+GL_UNSIGNED_INT,GL_RG32UI}, {c+GL_UNSIGNED_INT,GL_RGB32UI}, {d+GL_UNSIGNED_INT,GL_RGBA32UI}, {a+GL_HALF_FLOAT,GL_R16F}, {b+GL_HALF_FLOAT,GL_RG16F}, {c+GL_HALF_FLOAT,GL_RGB16F}, {d+GL_HALF_FLOAT,GL_RGBA16F}, {a+GL_FLOAT,GL_R32F}, {b+GL_FLOAT,GL_RG32F}, {c+GL_FLOAT,GL_RGB32F}, {d+GL_FLOAT,GL_RGBA32F}, }; return ifm[(channels<<16)+type]; }
inline GLint	gxGetTextureChannels( GLint internal_format ){ switch(gxGetTextureFormat(internal_format)){ case GL_RGBA: case GL_BGRA: case GL_RGBA_INTEGER: case GL_BGRA_INTEGER: return 4; case GL_RGB: case GL_BGR: case GL_RGB_INTEGER: case GL_BGR_INTEGER: return 3; case GL_RG: case GL_RG_INTEGER: return 2; case GL_RED: case GL_GREEN: case GL_BLUE: case GL_RED_INTEGER: case GL_GREEN_INTEGER: case GL_BLUE_INTEGER: return 1; default: return 0; } }
inline GLint	gxGetTextureBPC( GLint internal_format ){ GLint s=0; switch(gxGetTextureType(internal_format)){ case GL_HALF_FLOAT: case GL_SHORT: case GL_UNSIGNED_SHORT: s=GLint(sizeof(short)); break; case GL_BYTE: case GL_UNSIGNED_BYTE: s=GLint(sizeof(char)); break; case GL_FLOAT: case GL_INT: case GL_UNSIGNED_INT: s=GLint(sizeof(int)); break; }; return s*8; }
inline GLint	gxGetTextureBPP( GLint internal_format ){ return gxGetTextureBPC(internal_format)*gxGetTextureChannels(internal_format); }
inline GLenum	gxGetImageTextureInternalFormat( int depth, int channels ){ if(depth==8) return channels==1?GL_R8:channels==2?GL_RG8:channels==3?GL_RGB8:channels==4?GL_RGBA8:0; else if(depth==16)	return channels==1?GL_R16F:channels==2?GL_RG16F:channels==3?GL_RGB16F:channels==4?GL_RGBA16F:0; else if(depth==32)	return channels==1?GL_R32F:channels==2?GL_RG32F:channels==3?GL_RGB32F:channels==4?GL_RGBA32F:0; return 0; }
inline bool		gxIsSamplerType( GLenum uniformType ){ GLenum t=uniformType; if(t>=GL_SAMPLER_1D && t<=GL_SAMPLER_2D_SHADOW) return true; if(t>=GL_SAMPLER_1D_ARRAY && t<=GL_SAMPLER_CUBE_SHADOW) return true; if(t>=GL_INT_SAMPLER_1D && t<=GL_UNSIGNED_INT_SAMPLER_2D_ARRAY) return true; if(t>=GL_SAMPLER_2D_RECT && t<=GL_SAMPLER_2D_RECT_SHADOW ) return true; if(t>=GL_SAMPLER_BUFFER && t<=GL_UNSIGNED_INT_SAMPLER_BUFFER ) return true; if(t>=GL_SAMPLER_CUBE_MAP_ARRAY && t<=GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY ) return true; if(t>=GL_SAMPLER_2D_MULTISAMPLE && t<=GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY ) return true; /* TODO: if(t>=GL_SAMPLER_RENDERBUFFER_NV && t<=GL_UNSIGNED_INT_SAMPLER_RENDERBUFFER_NV ) return true;*/ return false; }
inline bool		gxIsImageType( GLenum uniformType ){ GLenum t=uniformType; if(t>=GL_IMAGE_1D && t<=GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY) return true; return false; }
inline int		gxGetMipLevels( int width, int height=1, int depth=1 ){ int l=0,s=uint(max(max(width,height),depth)); while(s){s=s>>1;l++;} return l; }
inline uint		gxGetMipLevels( uint width, uint height=1, uint depth=1 ){ uint l=0,s=uint(max(max(width,height),depth)); while(s){s=s>>1;l++;} return l; }
inline GLuint	gxCreateQuery( GLenum target ){ GLuint idx; if(glCreateQueries) glCreateQueries(target,1,&idx); else glGenQueries(1,&idx); return idx; }
inline GLuint	gxCreateTexture( GLenum target ){ GLuint idx; if(glCreateTextures) glCreateTextures( target, 1, &idx ); else{ GLuint b0=gxGetBinding(target); glGenTextures(1,&idx); glBindTexture(target,idx); glBindTexture(target,b0); } return idx; }
inline GLuint	gxCreateRenderBuffer(){ GLuint idx; if(glCreateRenderbuffers) glCreateRenderbuffers(1,&idx); else { GLuint b0=gxGetBinding(GL_RENDERBUFFER); glGenRenderbuffers(1,&idx); glBindRenderbuffer(GL_RENDERBUFFER,idx); glBindRenderbuffer(GL_RENDERBUFFER,b0); } return idx; }
inline GLuint	gxCreateVertexArray(){ GLuint idx; if(glCreateVertexArrays) glCreateVertexArrays(1, &idx); else glGenVertexArrays(1,&idx); return idx; }
inline const char* gxGetErrorString( GLenum e ){ if(e==GL_NO_ERROR) return ""; if(e==GL_INVALID_ENUM) return "GL_INVALID_ENUM"; if(e==GL_INVALID_VALUE) return "GL_INVALID_VALUE"; if(e==GL_INVALID_OPERATION) return "GL_INVALID_OPERATION"; if(e==GL_INVALID_FRAMEBUFFER_OPERATION) return "GL_INVALID_FRAMEBUFFER_OPERATION"; if(e==GL_OUT_OF_MEMORY) return "GL_OUT_OF_MEMORY"; if(e==GL_STACK_UNDERFLOW) return "GL_STACK_UNDERFLOW"; if(e==GL_STACK_OVERFLOW) return "GL_STACK_OVERFLOW"; return "UNKNOWN"; }
inline bool		gxHasMultidraw(){ return gxHasExtension(bindless_texture)&&gxHasExtension(shader_draw_parameters); }

//***********************************************
// type-to-internalformat mapper
template <class T>		GLenum gxTypeToInternalFormat();
template <> constexpr	GLenum gxTypeToInternalFormat<bool>(){	return GL_R8UI; }
template <> constexpr	GLenum gxTypeToInternalFormat<char>(){	return GL_R8I; }
template <> constexpr	GLenum gxTypeToInternalFormat<uchar>(){	return GL_R8UI; }
template <> constexpr	GLenum gxTypeToInternalFormat<int>(){	return GL_R32I; }
template <> constexpr	GLenum gxTypeToInternalFormat<uint>(){	return GL_R32UI; }
#ifdef __GXMATH_HALF__
template <> constexpr	GLenum gxTypeToInternalFormat<half>(){	return GL_R16F; }
#endif
template <> constexpr	GLenum gxTypeToInternalFormat<float>(){	return GL_R32F; }

//***********************************************
// forward declarations
namespace gl { struct Texture; struct Buffer; struct Program; struct VertexArray; }
namespace fx { struct metadata_t; }

gl::Texture*		gxCreateTexture1D(const char*,GLint,GLsizei,GLsizei,GLint,GLvoid*,bool);
gl::Texture*		gxCreateTexture2D(const char*,GLint,GLsizei,GLsizei,GLsizei,GLint,GLvoid*,bool,bool,GLsizei);
gl::Texture*		gxCreateTexture3D(const char*,GLint,GLsizei,GLsizei,GLsizei,GLint,GLvoid*);
gl::Texture*		gxCreateTextureCube(const char*,GLint,GLsizei,GLsizei,GLsizei,GLint,GLvoid*[6],bool);
gl::Texture*		gxCreateTextureBuffer(const char*,gl::Buffer*,GLint);
gl::Texture*		gxCreateTextureRectangle(const char*,GLsizei,GLsizei,GLint,GLvoid*);
gl::Texture*		gxCreateTextureView(gl::Texture*,GLuint,GLuint,GLuint,GLuint,GLenum,bool);
gl::Texture*		gxCreateTexture2DFromMemory(const char*,GLsizei,GLsizei,GLuint,GLint);
gl::Buffer*			gxCreateBuffer(const char*,GLenum,GLsizeiptr,GLenum,const void*,GLbitfield,bool);
gl::VertexArray*	gxCreateQuadVertexArray();
gl::VertexArray*	gxCreatePointVertexArray( GLsizei width, GLsizei height );

//***********************************************
namespace gl {
//***********************************************
	
namespace context
{
	inline bool is_core_profile(){ static GLint mask=0; if(mask==0) glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &mask); return mask==GL_CONTEXT_CORE_PROFILE_BIT; }
	inline bool is_compatibility_profile(){ static GLint mask=0; if(mask==0) glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &mask); return mask==GL_CONTEXT_COMPATIBILITY_PROFILE_BIT; } 
}

// simple GL/WGL extension loader
struct module_t
{
	HMODULE handle=nullptr;
	PROC(*__wglGetProcAddress)(LPCSTR)=nullptr;
	~module_t(){ if(!handle) return; FreeLibrary(handle); handle=nullptr; }
	module_t(){ handle=LoadLibraryA("opengl32.dll"); if(!handle){ printf("gl::module: unable to load opengl32.dll\n"); return; } if(handle) __wglGetProcAddress=(decltype(__wglGetProcAddress))GetProcAddress(handle,"wglGetProcAddress"); if(!__wglGetProcAddress) printf("gl::module_t(): wglGetProcAddress==nullptr\n"); }
	static void* get_proc_address( const char* name ){ if(!name||!*name) return nullptr; static module_t m; void* f=nullptr; if(m.__wglGetProcAddress&&(f=m.__wglGetProcAddress(name))!=nullptr) return f; f=(void*)GetProcAddress(m.handle,name); if(!f) printf("gl::get_proc_address(%s)==nullptr\n",name); return f; }
};

template <class T=void*> __forceinline T get_proc_address( const char* name )
{
	return T(module_t::get_proc_address(name));
}

//***********************************************
struct Object
{
	const GLuint	ID;
	const GLenum	target;
	
	virtual ~Object(){}
	Object( GLuint id, const char* name, GLenum _target ):ID(id),target(_target),_target_binding(gxGetTargetBinding(_target)){ size_t l=strlen(name),c=sizeof(_name)-1,n=l<c?l:c;strncpy((char*)_name,name,n);((char*)_name)[n]=0; }
	const char* name() const { return _name; }
	__forceinline GLuint binding( GLenum _target=0 ) const { return _target==0?gxGetIntegerv(_target_binding):gxGetIntegerv(gxGetTargetBinding(_target)); }
	
protected:
	const char		_name[256]={};
	const GLenum	_target_binding=0;
};

//***********************************************
struct Query : public Object
{
	GLuint64 result=0;
	Query( GLuint ID, const char* name, GLenum target ):Object(ID,name,target){}
	~Query() override { GLuint id=ID; glDeleteQueries(1,&id); }

	bool	is_available(){ GLint available; glGetQueryObjectiv(ID,GL_QUERY_RESULT_AVAILABLE, &available); return available!=0; }
	void	finish(){ glGetQueryObjectui64v(ID,GL_QUERY_RESULT,&result); } // use GL_QUERY_RESULT_NO_WAIT for async query download
	void	begin(){ glBeginQuery(target,ID); }
	void	end(){ glEndQuery(target); }
};

struct TimeElapsed : public Query
{
	TimeElapsed():Query(gxCreateQuery(GL_TIME_ELAPSED),"GL_TIME_ELAPSED",GL_TIME_ELAPSED){}
	double	delta(){ return double(result)/1000000.0; }
};

//***********************************************
struct timer_t : public Object
{
	union { double2 result; struct { double x, y; }; };	// Async OpenGL Timestamp: (begin,end)

	timer_t():Object(gxCreateQuery(GL_TIMESTAMP),"",GL_TIMESTAMP),_ID1(gxCreateQuery(target)){ result={}; }
	~timer_t() override { GLuint idx[2]={ID,_ID1}; glDeleteQueries(2,idx); }

	bool	is_available(){ GLint available; glGetQueryObjectiv(_ID1,GL_QUERY_RESULT_AVAILABLE, &available); return _complete=(available!=GL_FALSE); }
	void	finish(){ if(_complete) return; static const GLenum q=GL_QUERY_RESULT; GLuint64 v; glGetQueryObjectui64v(ID,q,&v); x = double(v)/1000000.0; glGetQueryObjectui64v(_ID1,q,&v); y=double(v)/1000000.0; _complete=true; }

	void	begin(){ glQueryCounter(ID,target); _complete=false; }
	void	end(){ glQueryCounter(_ID1,target); _complete=false; }
	double	delta(){ if(!_complete) finish(); return y-x; }
	double	now(){ return (gxGetInteger64v(GL_TIMESTAMP))/1000000.0; }
	void	clear(){ _complete=true; }
	
protected:
	const GLuint	_ID1=0;
	bool			_complete=false;
};

//***********************************************
struct Buffer : public Object
{
	Buffer( GLuint ID, const char* name, GLenum target, size_t size ):Object(ID,name,target),_size(size){} // bind() and bind_back() should be called to actually create this buffer
	~Buffer() override { GLuint id=ID; if(id) glDeleteBuffers( 1, &id ); }
	GLuint bind( bool b_bind=true ){ GLuint b0=binding(); if(!b_bind||b0!=ID) glBindBuffer( target, b_bind?ID:0 ); if(target==GL_TRANSFORM_FEEDBACK) glBindTransformFeedback(target, b_bind?ID:0 ); return b0; }
	GLuint bind_as( GLenum target1, bool b_bind=true ){ GLuint b0=binding(target1); if(!b_bind||b0!=ID) glBindBuffer( target1, b_bind?ID:0 ); return b0; }
	GLuint bind_base( GLuint index, bool b_bind=true ){ GLuint b0=binding(); if(base_bindable(target)&&(!b_bind||b0!=ID)) glBindBufferBase( target, index, b_bind?ID:0 ); return b0; }
	GLuint bind_base_as( GLenum target1, GLuint index, bool b_bind=true ){ GLuint b0=binding(target1); if(base_bindable(target1)&&(!b_bind||b0!=ID)) glBindBufferBase( target1, index, b_bind?ID:0 ); return b0; }
	GLuint bind_range( GLuint index, GLintptr offset, GLsizeiptr size, bool b_bind=true ){ GLuint b0=binding(); if(base_bindable(target)&&(!b_bind||b0!=ID)) glBindBufferRange( target, index, b_bind?ID:0, offset, size ); return b0; }
	__forceinline bool base_bindable( GLenum target1 ){ return target1==GL_SHADER_STORAGE_BUFFER||target1==GL_UNIFORM_BUFFER||target1==GL_TRANSFORM_FEEDBACK_BUFFER||target1==GL_ATOMIC_COUNTER_BUFFER; }

	void set_sub_data( const GLvoid* data, GLsizeiptr size, GLintptr offset=0 ){ if(glNamedBufferSubData) glNamedBufferSubData(ID,offset,size,data); else { GLuint b0=bind(); glBufferSubData(target,offset,size,data); glBindBuffer(target,b0); } }
	template <class T> void set_sub_data( const vector<T>& data, GLintptr offset=0 ){ set_sub_data(data.data(),GLsizeiptr(sizeof(T)*data.size()),offset); }
	template <class T, size_t N> void set_sub_data( const std::array<T,N>& data, GLintptr offset=0 ){ set_sub_data(data.data(),GLsizeiptr(sizeof(T)*N),offset); }
	template <class T> void set_data( const T* data, GLsizeiptr count ){ set_sub_data((const GLvoid*)data,sizeof(T)*count,0); }
	template <class T> void set_data( T data ){ set_sub_data(&data,GLsizeiptr(sizeof(T)),0); }
	template <class T> void set_data( const vector<T>& data ){ set_sub_data(data.data(),GLsizeiptr(sizeof(T)*data.size()),0); }
	template <class T, size_t N> void set_data( const std::array<T,N>& data ){ set_sub_data(data.data(),GLsizeiptr(sizeof(T)*N),0); }
	void copy_data( Buffer* write_buffer, GLsizeiptr size=0 ){ copy_sub_data( write_buffer, size?size:this->size() ); }
	void copy_sub_data( Buffer* write_buffer, GLsizeiptr size, GLintptr read_offset=0, GLintptr write_offset=0 ){ if(glCopyNamedBufferSubData) glCopyNamedBufferSubData(ID,write_buffer->ID,read_offset,write_offset,size); else printf("Buffer::copySubData(): glCopyNamedBufferSubData==nullptr (only supports named mode)\n" ); }
	template <class T> void clear( const T value ){ constexpr GLenum iformat=gxTypeToInternalFormat<T>(); static GLenum format=gxGetTextureFormat(iformat), type=gxGetTextureType(iformat); if(glClearNamedBufferData) glClearNamedBufferData(ID,iformat,format,type,&value); else if(glClearBufferData){ GLuint b0=bind(); glClearBufferData(target,iformat,format,type,&value); bind(b0); } }
	void get_sub_data( GLvoid* data, GLsizeiptr size, GLintptr offset=0 ){ if(glGetNamedBufferSubData) glGetNamedBufferSubData(ID,offset,size?size:this->size(),data); else if(glGetBufferSubData){ GLuint b0=bind(); glGetBufferSubData(target,offset,size?size:this->size(),data); glBindBuffer(target,b0); } }
	template <class T> T get_data(){ T v=0; get_sub_data(&v,sizeof(T),0); return v; }
	template <class T> vector<T> get_data( GLsizeiptr count ){ vector<T> v; v.resize(count); get_sub_data(v.data(),sizeof(T)*count,0); return v; }
	void* map( GLenum access=GL_READ_ONLY, GLenum target=0 ){ if(target==0) return glMapNamedBuffer(ID,access); bind(); return glMapBuffer(target,access); }
	void* map_range( GLintptr offset, GLsizeiptr length, GLenum access=GL_MAP_READ_BIT, GLenum target=0 ){ if(is_map_persistent()) access|=GL_MAP_PERSISTENT_BIT|GL_MAP_COHERENT_BIT; if(target==0) return glMapNamedBufferRange(ID,offset,length,access); bind(); return glMapBufferRange(target,offset,length,access); }
	void unmap( GLenum target=0 ){ if(target==0&&glUnmapNamedBuffer) glUnmapNamedBuffer(ID); else glUnmapBuffer(target); }

	// queries
	GLint parameteriv( GLenum value ){ GLint i; if(glGetNamedBufferParameteriv) glGetNamedBufferParameteriv(ID,value,&i); else { GLuint b0=bind(); glGetBufferParameteriv(target,value,&i); glBindBuffer(target,b0); } return i; }
	GLint64 parameteri64v( GLenum value ){ GLint64 i; if(glGetNamedBufferParameteri64v) glGetNamedBufferParameteri64v(ID,value,&i); else { GLuint b0=bind(); glGetBufferParameteri64v(target,value,&i); glBindBuffer(target,b0); } return i; }
	GLint access(){ return parameteriv(GL_BUFFER_ACCESS); }
	GLint access_flags(){ return parameteriv(GL_BUFFER_ACCESS_FLAGS); }
	bool is_immutable(){ return parameteriv(GL_BUFFER_IMMUTABLE_STORAGE)!=GL_FALSE; }
	bool is_mapped(){ return parameteriv(GL_BUFFER_MAPPED)!=GL_FALSE; }
	bool is_map_persistent(){ return is_immutable() && (storage_flags()&GL_MAP_PERSISTENT_BIT); }
	GLsizeiptr map_length(){ return GLsizeiptr(parameteri64v(GL_BUFFER_MAP_LENGTH)); }
	GLintptr map_offset(){ return GLintptr(parameteri64v(GL_BUFFER_MAP_OFFSET)); }
	size_t size(){ return _size?_size:size_t(parameteri64v(GL_BUFFER_SIZE)); }
	GLbitfield storage_flags(){ return parameteriv(GL_BUFFER_STORAGE_FLAGS); }
	GLint usage(){ return parameteriv(GL_BUFFER_USAGE); }

protected:
		
	size_t _size=0;
};

//***********************************************
struct TransformFeedback : public Buffer
{
	TransformFeedback( GLuint ID, const char* name, GLenum target=GL_TRANSFORM_FEEDBACK ):Buffer(ID,name,target,0){};

	void begin( GLenum primitive_mode ){ bind(); glEnable( GL_RASTERIZER_DISCARD ); glBeginTransformFeedback( primitive_mode ); }
	void end( GLuint b0=0 ){ glEndTransformFeedback(); glDisable( GL_RASTERIZER_DISCARD ); bind(false); }
	void draw( GLenum mode ){ GLuint b0=bind(); glDrawTransformFeedback( mode, ID ); if(b0!=ID) glBindTransformFeedback( target, b0 ); }
};

//***********************************************
// DrawIndirectCommands
struct DrawArraysIndirectCommand { uint count, instance_count=1, first_vertex=0, base_instance=0; };
struct DrawElementsIndirectCommand { uint count, instance_count=1, first_index=0, base_vertex=0, base_instance=0; uint pad0, pad1, pad2; }; // see https://www.opengl.org/wiki/GLAPI/glDrawElementsInstancedBaseVertexBaseInstance

//***********************************************
struct Texture : public Object
{
	Texture( GLuint ID, const char* name, GLenum target, GLenum InternalFormat, GLenum Format, GLenum Type, uint64_t heap=_get_heap_handle() ):Object(ID,name,target),_internal_format(InternalFormat),_type(Type),_format(Format),_channels(gxGetTextureChannels(InternalFormat)),_bpp(gxGetTextureBPP(InternalFormat)),_crtheap(heap){}
	~Texture() override { delete_views(_next); make_resident(false); glDeleteTextures(1,(GLuint*)&ID); invalidated()=true; }

	// delete texture views and query to check whether dirty/deleted textures exist
	static void delete_views( Texture*& v ){ if(!v) return; delete_views(v->_next); v->make_resident(false); glDeleteTextures(1,(GLuint*)&v->ID);
#ifdef __msvc__
	HeapFree((void*)v->_crtheap,0,(Texture*)v);
#endif
	v=nullptr; invalidated()=true; } // use HeapFree() for views
	static bool& invalidated(){ static bool b=true; return b; }

	GLuint bind( bool b_bind=true ){ GLuint b0=gxGetIntegerv(_target_binding); if(!b_bind||ID!=b0) glBindTexture( target, b_bind?ID:0 ); return b0; }
	void bind_image_texture( GLuint unit, GLenum access=GL_READ_ONLY /* or GL_WRITE_ONLY or GL_READ_WRITE */ , GLint level=0, GLenum format=0, bool bLayered=false, GLint layer=0 ){ glBindImageTexture( unit, ID, level, bLayered?GL_TRUE:GL_FALSE, layer, access, format?format:internal_format() ); }

	// query and bind back
	inline GLint get_texture_parameteriv( GLenum pname ) const { GLint iv; if(glGetTextureParameteriv) glGetTextureParameteriv(ID,pname,&iv); else { GLuint b0=gxGetIntegerv(_target_binding); glBindTexture(target,ID); glGetTexParameteriv(target,pname,&iv ); glBindTexture(target,b0); } return iv; }
	inline GLint get_texture_level_parameteriv( GLenum pname, GLint level ) const { GLint iv; if(glGetTextureLevelParameteriv) glGetTextureLevelParameteriv(ID,level,pname,&iv); else { GLuint b0=gxGetIntegerv(_target_binding); glBindTexture(target,ID); glGetTexLevelParameteriv(target,level,pname,&iv); glBindTexture(target,b0); } return iv; }
	inline void texture_parameteri( GLenum pname, GLint param ) const { if(glTextureParameteri) glTextureParameteri(ID,pname,param); else { GLuint b0=gxGetIntegerv(_target_binding); glBindTexture(target,ID); glTexParameteri(target,pname,param); glBindTexture(target,b0); } }
	inline void texture_parameterf( GLenum pname, GLfloat param ) const { if(glTextureParameterf) glTextureParameterf(ID,pname,param); else { GLuint b0=gxGetIntegerv(_target_binding); glBindTexture(target,ID); glTexParameterf(target,pname,param); glBindTexture(target,b0); } }
	inline void texture_parameterfv( GLenum pname, const GLfloat* params ) const { if(glTextureParameterfv) glTextureParameterfv(ID, pname, params); else { GLuint b0=gxGetIntegerv(_target_binding); glBindTexture(target,ID); glTexParameterfv(target,pname,params); glBindTexture(target,b0); } }

	// texture dimension queries: pre-recorded when creating this
	GLint mip_levels() const {				return _levels; } // on-demand query: is_immutable()?get_texture_parameteriv(GL_TEXTURE_VIEW_NUM_LEVELS):get_texture_parameteriv(GL_TEXTURE_MAX_LEVEL)-get_texture_parameteriv(GL_TEXTURE_BASE_LEVEL)+1; }
	GLint width( GLint level=0 ) const {	return max(1,_width>>level); } // on-demand query: get_texture_level_parameteriv( GL_TEXTURE_WIDTH, level )
	GLint height( GLint level=0 ) const {	return (target==GL_TEXTURE_1D||target==GL_TEXTURE_1D_ARRAY||target==GL_TEXTURE_BUFFER)?1:max(1,_height>>level); } // on-demand query: get_texture_level_parameteriv( GL_TEXTURE_HEIGHT, level );
	GLint depth( GLint level=0 ) const {	return (target==GL_TEXTURE_1D||target==GL_TEXTURE_1D_ARRAY||target==GL_TEXTURE_BUFFER||target==GL_TEXTURE_2D||target==GL_TEXTURE_2D_MULTISAMPLE||target==GL_TEXTURE_RECTANGLE)?1:(target==GL_TEXTURE_2D_ARRAY||target==GL_TEXTURE_2D_MULTISAMPLE_ARRAY||target==GL_TEXTURE_CUBE_MAP||target==GL_TEXTURE_CUBE_MAP_ARRAY)?_depth:max(1,_depth>>level); } // on-demand query: get_texture_level_parameteriv( GL_TEXTURE_DEPTH, level );
	GLint layers( GLint level=0 ) const {	return (target==GL_TEXTURE_1D_ARRAY)?height(level):(target==GL_TEXTURE_2D_ARRAY||target==GL_TEXTURE_2D_MULTISAMPLE_ARRAY||target==GL_TEXTURE_3D)?depth(level):target==GL_TEXTURE_CUBE_MAP||target==GL_TEXTURE_CUBE_MAP_ARRAY?depth(level):1; }

	// texture size helpers for the first mip level
	ivec2	size( GLint level=0 ) const {	return ivec2{width(level),height(level)}; }
	vec2	sizef( GLint level=0 ) const {	return vec2{float(width(level)),float(height(level))}; }

	// other texture queries
	ivec2 mip_range() const {	ivec2 range=ivec2{get_texture_parameteriv(GL_TEXTURE_BASE_LEVEL),get_texture_parameteriv(GL_TEXTURE_MAX_LEVEL)}; return ivec2{range.x,range.y-range.x+1}; }
	ivec2 filter() const {		return ivec2{get_texture_parameteriv(GL_TEXTURE_MIN_FILTER),get_texture_parameteriv(GL_TEXTURE_MAG_FILTER)}; }
	ivec3 wrap() const {		ivec3 w{get_texture_parameteriv(GL_TEXTURE_WRAP_S),get_texture_parameteriv(GL_TEXTURE_WRAP_T),0}; if(target==GL_TEXTURE_3D||target==GL_TEXTURE_CUBE_MAP||target==GL_TEXTURE_CUBE_MAP_ARRAY) w.z=get_texture_parameteriv(GL_TEXTURE_WRAP_R); return w; }
	GLint base_level() const {	return get_texture_parameteriv(GL_TEXTURE_BASE_LEVEL); }
	GLint max_level() const {	return get_texture_parameteriv(GL_TEXTURE_MAX_LEVEL); }
	GLint min_filter() const {	return get_texture_parameteriv(GL_TEXTURE_MIN_FILTER); }
	GLint mag_filter() const {	return get_texture_parameteriv(GL_TEXTURE_MAG_FILTER); }
	GLint wrap_s() const {		return get_texture_parameteriv(GL_TEXTURE_WRAP_S); }
	GLint wrap_t() const {		return get_texture_parameteriv(GL_TEXTURE_WRAP_T); }
	GLint wrap_r() const {		return get_texture_parameteriv(GL_TEXTURE_WRAP_R); }
	GLint min_LOD() const {		return get_texture_parameteriv(GL_TEXTURE_MIN_LOD); }
	GLint max_LOD() const {		return get_texture_parameteriv(GL_TEXTURE_MAX_LOD); }
	bool is_immutable() const {	return get_texture_parameteriv(GL_TEXTURE_IMMUTABLE_FORMAT)!=GL_FALSE; }

	// bindless texture extension
	GLuint64 handle() const {	return glGetTextureHandleARB?glGetTextureHandleARB(ID):0; }
	void make_resident( bool b ) const { static bool support_residence = glGetTextureHandleARB&&glIsTextureHandleResidentARB&&glMakeTextureHandleResidentARB&&glMakeTextureHandleNonResidentARB; if(!support_residence) return; GLuint64 h=handle(); if(!h) return; bool r=glIsTextureHandleResidentARB(h); if(b&&!r) glMakeTextureHandleResidentARB(h); else if(!b&&r) glMakeTextureHandleNonResidentARB(h); }

	// type/format/channels/size
	inline GLint  internal_format() const { return _internal_format; }
	inline GLenum format() const { return _format; }
	inline GLenum type() const { return _type; }
	inline GLint  channels() const { return _channels; }
	inline GLint  bpc() const { return _bpp/_channels; } // bits per channel
	inline GLint  bpp() const { return _bpp; } // bits per pixels
	inline GLint  Bpp() const { return GLint(_bpp>>3); } // bytes per pixels
	inline GLsizei multisamples() const { return target==GL_TEXTURE_2D_MULTISAMPLE||target==GL_TEXTURE_2D_MULTISAMPLE_ARRAY?_multisamples:1; }
	size_t width_step( GLint level=0 ) const { int s=((width(level)*Bpp()+3)>>2)<<2; return size_t(s); } // 4-byte-aligned size in a single row
	size_t mem_size( GLint level=0 ) const { return width_step(level)*size_t(height(level)); }

	// set attributes
	void set_filter( GLint min_filter, GLint mag_filter=0, bool b_sync_view=true ){ static const GLenum n=GL_NEAREST,l=GL_LINEAR,nn=GL_NEAREST_MIPMAP_NEAREST,nl=GL_NEAREST_MIPMAP_LINEAR,ln=GL_LINEAR_MIPMAP_NEAREST,ll=GL_LINEAR_MIPMAP_LINEAR; GLint i=min_filter,g=mag_filter?mag_filter:min_filter; texture_parameteri( GL_TEXTURE_MIN_FILTER, _levels==1?(i==nn||i==nl?n:i==ln||i==ll?l:i):i==n?nn:i==l?ll:i ); texture_parameteri( GL_TEXTURE_MAG_FILTER, g==nn||g==nl?n:g==ln||g==ll?l:g ); if(b_sync_view&&_next) _next->set_filter( min_filter, mag_filter ); }
	void set_wrap( GLint wrap, bool b_sync_view=true ){ texture_parameteri( GL_TEXTURE_WRAP_S, wrap ); if(target==GL_TEXTURE_2D||target==GL_TEXTURE_2D_ARRAY||target==GL_TEXTURE_2D_MULTISAMPLE||target==GL_TEXTURE_2D_MULTISAMPLE_ARRAY||target==GL_TEXTURE_RECTANGLE) texture_parameteri( GL_TEXTURE_WRAP_T, wrap ); if(target==GL_TEXTURE_3D||target==GL_TEXTURE_CUBE_MAP||target==GL_TEXTURE_CUBE_MAP_ARRAY) texture_parameteri( GL_TEXTURE_WRAP_R, wrap ); if(b_sync_view&&_next) _next->set_wrap( wrap, b_sync_view ); }
	void set_mipmap_range( GLint base_level, GLint level_count ){ texture_parameteri(GL_TEXTURE_BASE_LEVEL,base_level); texture_parameteri(GL_TEXTURE_MAX_LEVEL,base_level+level_count-1); }
	void set_min_LOD( GLfloat min_LOD ){ texture_parameterf(GL_TEXTURE_MIN_LOD,min_LOD); }
	void set_max_LOD( GLfloat max_LOD ){ texture_parameterf(GL_TEXTURE_MAX_LOD,max_LOD); }
	void set_LOD( GLfloat min_LOD, GLfloat max_LOD ){ texture_parameterf(GL_TEXTURE_MIN_LOD,min_LOD); texture_parameterf(GL_TEXTURE_MAX_LOD,max_LOD); }
	void set_border_color( const vec4& color ){ texture_parameterfv(GL_TEXTURE_BORDER_COLOR, &color.x ); }

	// clear colors
	void clear( const vec4& color, GLint level=0 ){ GLenum t=type(),f=format(); if(t==GL_HALF_FLOAT){ half4 h={}; ftoh(color,h,channels()); glClearTexImage(ID,level,f,t,h); } else if(t==GL_FLOAT) glClearTexImage(ID,level,format(),t,&color); else printf( "%s->clear(): texture is not one of float/half types\n", _name ); }
	void cleari( const ivec4& color, GLint level=0 ){ GLenum t=type(),f=format(); if(t==GL_INT) glClearTexImage(ID,level,f,t,color); else if(t==GL_SHORT){ short4 i={short(color.x),short(color.y),short(color.z),short(color.w)}; glClearTexImage(ID,level,f,t,&i); } else if(t==GL_BYTE){ char4 i={char(color.x),char(color.y),char(color.z),char(color.w)}; glClearTexImage(ID,level,f,t,&i); } else printf( "%s->cleari(): texture is not one of byte/short/int types\n", _name ); }
	void clearui( const uvec4& color, GLint level=0 ){ GLenum t=type(),f=format(); if(t==GL_UNSIGNED_INT) glClearTexImage(ID,level,f,t,color); else if(t==GL_UNSIGNED_SHORT){ ushort4 i={ushort(color.x),ushort(color.y),ushort(color.z),ushort(color.w)}; glClearTexImage(ID,level,f,t,&i); } else if(t==GL_UNSIGNED_BYTE){ uchar4 i={uchar(color.x),uchar(color.y),uchar(color.z),uchar(color.w)}; glClearTexImage(ID,level,f,t,&i); } else printf( "%s->clearui(): texture is not one of unsigned byte/short/int types\n", _name ); }

	// mipmap
	void generate_mipmap(){ if(get_texture_parameteriv(GL_TEXTURE_IMMUTABLE_FORMAT)&&get_texture_parameteriv(GL_TEXTURE_MAX_LEVEL)<1){ printf( "Texture::generate_mipmap(%s): no mipmap generated for 1 level.\n", _name ); return; } int w=get_texture_level_parameteriv(GL_TEXTURE_WIDTH,0), h=get_texture_level_parameteriv(GL_TEXTURE_HEIGHT,0), d=get_texture_level_parameteriv(GL_TEXTURE_DEPTH,0); texture_parameteri(GL_TEXTURE_BASE_LEVEL,0); texture_parameteri(GL_TEXTURE_MAX_LEVEL,0+gxGetMipLevels(w,h,d)-1); if(glGenerateTextureMipmap) glGenerateTextureMipmap(ID); else { GLuint b0=gxGetIntegerv(_target_binding); glBindTexture(target,ID); glGenerateMipmap(target); glBindTexture(target,b0); }if(get_texture_parameteriv(GL_TEXTURE_MIN_FILTER)==GL_LINEAR) texture_parameteri( GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR ); else if(get_texture_parameteriv(GL_TEXTURE_MIN_FILTER)==GL_NEAREST) texture_parameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST ); }

	// get_image/set_image
	void get_image( GLvoid* pixels, GLint level=0 ){ if(target==GL_TEXTURE_BUFFER){ printf("[%s] read_pixels() not supports GL_TEXTURE_BUFFER\n", _name ); return; } else if(glGetTextureImage){ glGetTextureImage( ID, level, format(), type(), GLsizei(mem_size()), pixels ); return; } GLuint b0=bind(); glGetTexImage( target, level, format(), type(), pixels ); glBindTexture( target, b0 ); }
	void set_image( GLvoid* pixels, GLint level=0, GLsizei width=0, GLsizei height=0, GLsizei depth=0, GLint x=0, GLint y=0, GLint z=0 );

	// instance-related
	inline Texture* clone( const char* name );
	inline bool copy( Texture* dst, GLint level=0 );

	// view-related function: wrapper to createTextureView
	inline static uint crc( GLuint min_level, GLuint levels, GLuint min_layer, GLuint layers, GLenum target, bool force_array ){ struct info { GLuint min_level, levels, min_layer, layers; GLenum target; bool force_array; }; info i={min_level,levels,min_layer,layers,target,force_array}; return crc32(0,&i,sizeof(i)); }
	inline Texture* view( GLuint min_level, GLuint levels, GLuint min_layer=0, GLuint layers=1, GLenum target=0 ){ return gxCreateTextureView(this,min_level,levels,min_layer,layers,target,false); } // view support (> OpenGL 4.3)
	inline Texture* slice( GLuint layer, GLuint level=0 ){ return view(level,1,layer,1); }
	inline Texture* last_mip( GLuint layer=0 ){ return view(_levels-1,1,layer,1); }
	inline Texture* array_view(){ return (layers()>1)?this:gxCreateTextureView(this,0,mip_levels(),0,layers(),0,true); }

	// friend functions to access data members
	friend Texture* ::gxCreateTexture1D(const char*,GLint,GLsizei,GLsizei,GLint,GLvoid*,bool);
	friend Texture* ::gxCreateTexture2D(const char*,GLint,GLsizei,GLsizei,GLsizei,GLint,GLvoid*,bool,bool,GLsizei);
	friend Texture* ::gxCreateTexture3D(const char*,GLint,GLsizei,GLsizei,GLsizei,GLint,GLvoid*);
	friend Texture* ::gxCreateTextureCube(const char*,GLint,GLsizei,GLsizei,GLsizei,GLint,GLvoid* data[6],bool);
	friend Texture* ::gxCreateTextureBuffer(const char*,gl::Buffer*,GLint);
	friend Texture* ::gxCreateTextureRectangle(const char*,GLsizei,GLsizei,GLint,GLvoid*);
	friend Texture* ::gxCreateTextureView(Texture* src,GLuint,GLuint,GLuint,GLuint,GLenum,bool);
	friend Texture* ::gxCreateTexture2DFromMemory( const char* name, GLsizei width, GLsizei height, GLuint memory_object, GLint internal_format );

	friend struct fx::metadata_t;
	fx::metadata_t* metadata = nullptr;

protected: // protected data members

	// dimensions
	GLint		_width=1;
	GLint		_height=1;
	GLint		_depth=1;
	GLint		_levels=1;

	// internal format, type, format
	GLenum		_internal_format;
	GLenum		_type;
	GLenum		_format;
	GLint		_channels;
	GLint		_bpp;
	GLsizei		_multisamples=1;

	// view-related
	uint			_key=0;			// key of the current view
	Texture*		_next=nullptr;	// next view node: a node of linked list, starting from the parent node
	const uint64_t	_crtheap=0;		// heap handle to the parent, require to allocate view across DLL boundaries
};

inline void Texture::set_image( GLvoid* pixels, GLint level, GLsizei width, GLsizei height, GLsizei depth, GLint x, GLint y, GLint z )
{
	GLenum g=target, f=format(),t=type(); GLsizei w=width?width:this->width(),h=height?height:this->height(),d=depth?depth:this->depth();
	bool dsa=glTextureSubImage1D&&glTextureSubImage2D&&glTextureSubImage3D; GLuint b0=dsa?0:bind();
	if(g==GL_TEXTURE_1D){ if(dsa) glTextureSubImage1D(ID,0,x,w,f,t,pixels); else glTexSubImage1D(g,0,x,w,f,t,pixels); }
	else if(g==GL_TEXTURE_2D||g==GL_TEXTURE_1D_ARRAY||(g>=GL_TEXTURE_CUBE_MAP_POSITIVE_X&&g<=GL_TEXTURE_CUBE_MAP_NEGATIVE_Z)){ if(dsa) glTextureSubImage2D(ID,0,x,y,w,h,f,t,pixels); else glTexSubImage2D(g,0,x,y,w,h,f,t,pixels); }
	else if(g==GL_TEXTURE_3D||g==GL_TEXTURE_2D_ARRAY){ if(dsa) glTextureSubImage3D(ID,0,x,y,z,w,h,d,f,t,pixels); else glTexSubImage3D(g,0,x,y,z,w,h,d,f,t,pixels); }
	if(b0) glBindTexture(g,b0);
}

inline Texture* Texture::clone( const char* name )
{
	if(target==GL_TEXTURE_BUFFER) printf( "[%s] clone() does not support for GL_TEXTURE_BUFFER\n", name );
	GLint wrap			= get_texture_parameteriv( GL_TEXTURE_WRAP_S );
	GLint min_filter	= get_texture_parameteriv( GL_TEXTURE_MIN_FILTER );
	GLint mag_filter	= get_texture_parameteriv( GL_TEXTURE_MAG_FILTER );

	bool b_multisample = target==GL_TEXTURE_2D_MULTISAMPLE||target==GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
	GLint m=mip_levels(), w=width(), h=height(), d=depth(), l=layers(), f=internal_format();

	Texture* t =
		(target==GL_TEXTURE_1D||target==GL_TEXTURE_1D_ARRAY) ? gxCreateTexture1D( name, m, w, l, f, nullptr, false ):
		(target==GL_TEXTURE_2D||target==GL_TEXTURE_2D_ARRAY||target==GL_TEXTURE_2D_MULTISAMPLE||target==GL_TEXTURE_2D_MULTISAMPLE_ARRAY) ? gxCreateTexture2D( name, m, w, h, l, f, nullptr, false, b_multisample, multisamples() ):
		(target==GL_TEXTURE_3D) ? gxCreateTexture3D( name, m, w, h, d, f, nullptr ):
		(target==GL_TEXTURE_CUBE_MAP||target==GL_TEXTURE_CUBE_MAP_ARRAY) ? gxCreateTextureCube( name, m, w, h, l, f, nullptr, false ):
		(target==GL_TEXTURE_RECTANGLE) ? gxCreateTextureRectangle( name, w, h, f, nullptr ):nullptr;

	if(t==nullptr){ printf( "Texture[\"%s\"]::clone() == nullptr\n", name ); return nullptr; }

	t->texture_parameteri( GL_TEXTURE_WRAP_S, wrap );
	if(t->height()>1)	t->texture_parameteri( GL_TEXTURE_WRAP_T, wrap );
	if(t->depth()>1)	t->texture_parameteri( GL_TEXTURE_WRAP_R, wrap );
	t->texture_parameteri( GL_TEXTURE_MIN_FILTER, min_filter );
	t->texture_parameteri( GL_TEXTURE_MAG_FILTER, mag_filter );

	this->copy(t);
	if(t->mip_levels()>1) t->generate_mipmap();

	return t;
}

inline bool Texture::copy( Texture* dst, GLint level )
{
	if(dst==nullptr) return false; GLint w0=width(level),h0=height(level),d0=depth(level),w1=dst->width(level),h1=dst->height(level),d1=dst->depth(level);
	if(w0!=w1||h0!=h1||d0!=d1){ printf("%s::copy(): Dimension (%dx%dx%d) is different from %s (%dx%dx%d)\n", _name, w0, h0, d0, dst->_name, w1, h1, d1 ); return false; }
	if(mem_size()!=dst->mem_size()){ printf("%s::copy(): %s.mem_size(=%d) != %s.mem_size(=%d)\n", _name, _name, int(mem_size()), dst->_name, int(dst->mem_size()) ); return false; }
	glCopyImageSubData( ID, target, level, 0, 0, 0, dst->ID, dst->target, level, 0, 0, 0, w0, h0, d0 ); return true;
}

//***********************************************
// this vertex array generates multiple instances of VAO to bind multiple programs
struct VertexArray : public Object
{
	VertexArray( GLuint ID, const char* name ):Object(ID,name,GL_VERTEX_ARRAY),vertex_buffer(nullptr),index_buffer(nullptr),vertex_count(0),index_count(0){};
	~VertexArray() override { if(vertex_buffer){ delete vertex_buffer; vertex_buffer=nullptr; } if(index_buffer){ delete index_buffer; index_buffer=nullptr; } GLuint id=ID; if(id) glDeleteVertexArrays( 1, &id ); }
	GLuint bind( bool b_bind=true ){ GLuint b0=binding(); if(!b_bind||b0!=ID) glBindVertexArray( b_bind?ID:0 ); return b0; }

	inline void draw_arrays( GLint first, GLsizei count=0, GLenum mode=GL_TRIANGLES, bool b_bind=true ){ if(b_bind) bind(); glDrawArrays( mode, first, count?count:GLsizei(vertex_count) ); }
	inline void draw_arrays_instanced( GLint first, GLsizei instance_count, GLsizei count=0, GLenum mode=GL_TRIANGLES, bool b_bind=true ){ if(b_bind) bind(); glDrawArraysInstanced( mode, first, count?count:GLsizei(vertex_count), instance_count ); }
	inline void draw_elements( GLuint first, GLsizei count=0, GLenum mode=GL_TRIANGLES, bool b_bind=true ){ if(index_buffer==nullptr) return; if(b_bind) bind(); glDrawElements( mode, count?count:GLsizei(index_count), GL_UNSIGNED_INT, uint_offset(first) ); }
	inline void draw_elements_instanced( GLuint first, GLsizei instance_count, GLsizei count=0, GLenum mode=GL_TRIANGLES, bool b_bind=true ){ if(index_buffer==nullptr) return; if(b_bind) bind(); glDrawElementsInstanced( mode, count?count:GLsizei(index_count), GL_UNSIGNED_INT, uint_offset(first), instance_count ); }
	inline void draw_elements_indirect( GLvoid* indirect=nullptr, GLenum mode=GL_TRIANGLES, bool b_bind=true ){ if(index_buffer==nullptr) return; if(b_bind) bind(); glDrawElementsIndirect( mode, GL_UNSIGNED_INT, indirect ); }
	inline void draw_range_elements( GLuint first, GLuint end, GLsizei count=0, GLenum mode=GL_TRIANGLES, bool b_bind=true ){ if(index_buffer==nullptr) return; if(b_bind) bind(); glDrawRangeElements( mode, first, end, count?count:GLsizei(index_count), GL_UNSIGNED_INT, uint_offset(first)); }
	inline void multi_draw_elements( GLuint* pfirst, const GLsizei* pcount, GLsizei draw_count, GLenum mode=GL_TRIANGLES, bool b_bind=true ){ if(index_buffer==nullptr) return; if(b_bind) bind(); glMultiDrawElements( mode, pcount, GL_UNSIGNED_INT, uint_offset(pfirst,draw_count), draw_count ); }
	inline void multi_draw_elements_indirect( GLsizei draw_count, GLsizei stride=sizeof(DrawElementsIndirectCommand), GLvoid* indirect=nullptr, GLenum mode=GL_TRIANGLES, bool b_bind=true ){ if(index_buffer==nullptr) return; if(b_bind) bind(); glMultiDrawElementsIndirect( mode, GL_UNSIGNED_INT, indirect, draw_count, stride ); }
	inline void multi_draw_elements_indirect_count( GLsizei max_draw_count, GLsizei stride=sizeof(DrawElementsIndirectCommand), const void* indirect=0 /* GL_DRAW_INDIRECT_BUFFER */, GLintptr draw_count=0 /* GL_PARAMETER_BUFFER_ARB */, GLenum mode=GL_TRIANGLES, bool b_bind=true ){ if(index_buffer==nullptr) return; if(b_bind) bind(); glMultiDrawElementsIndirectCount(mode,GL_UNSIGNED_INT,indirect,draw_count,max_draw_count,stride); }

	inline GLvoid* uint_offset( GLuint first ){ return (GLvoid*)(first*sizeof(GLuint)); }
	inline const GLvoid* const* uint_offset( GLuint* pfirst, GLsizei draw_count ){ static vector<GLvoid*> offsets; if(offsets.size()<uint(draw_count)) offsets.resize(draw_count); for( int k=0; k<draw_count; k++ ) offsets[k] = (GLvoid*)(pfirst[k]*sizeof(GLuint)); return &offsets[0]; }

	Buffer*		vertex_buffer = nullptr;
	Buffer*		index_buffer = nullptr;
	size_t		vertex_count = 0;
	size_t		index_count = 0;
};

//***********************************************
struct Framebuffer : public Object
{
	struct depth_key_t { union { struct {uint width:16, height:16, multisamples:8, layers:16, renderbuffer:1, dummy:7;}; uint64_t value; }; depth_key_t():value(0){} };
	static const GLint MAX_COLOR_ATTACHMENTS=8;
	static Framebuffer*& instance();

	Framebuffer( GLuint ID, const char* name ) : Object(ID,name,GL_FRAMEBUFFER){ memset(_active_targets,0,sizeof(_active_targets)); glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ); }
	~Framebuffer() override { glBindFramebuffer(GL_FRAMEBUFFER,0); glBindRenderbuffer(GL_RENDERBUFFER,0);memset(_active_targets,0,sizeof(_active_targets)); GLuint id=ID; if(ID) glDeleteFramebuffers(1,&id); for(auto& it:_depth_buffers){const depth_key_t& key=reinterpret_cast<const depth_key_t&>(it.first); if(key.renderbuffer) glDeleteRenderbuffers(1,&it.second); else glDeleteTextures(1,&it.second); } _depth_buffers.clear(); }
	GLuint bind( bool b_bind=true ){ GLuint b0=binding(); if(!b_bind||ID!=b0) glBindFramebuffer( GL_FRAMEBUFFER, b_bind?ID:0 ); return b0; }

	void bind( Texture* t0, Texture* t1=nullptr, Texture* t2=nullptr, Texture* t3=nullptr, Texture* t4=nullptr, Texture* t5=nullptr, Texture* t6=nullptr, Texture* t7=nullptr ){ if(ID) bind(t0,0,0,t1,0,0,t2,0,0,t3,0,0,t4,0,0,t5,0,0,t6,0,0,t7,0,0); }
	void bind( Texture* t0, GLint layer0, GLint mipLevel0, Texture* t1=nullptr, GLint layer1=0, GLint mipLevel1=0, Texture* t2=nullptr, GLint layer2=0, GLint mipLevel2=0, Texture* t3=nullptr, GLint layer3=0, GLint mipLevel3=0, Texture* t4=nullptr, GLint layer4=0, GLint mipLevel4=0, Texture* t5=nullptr, GLint layer5=0, GLint mipLevel5=0, Texture* t6=nullptr, GLint layer6=0, GLint mipLevel6=0, Texture* t7=nullptr, GLint layer7=0, GLint mipLevel7=0 );
	void bind_no_attachments( GLint width, GLint height, GLint layers=1, GLint samples=1 ); // ARB_framebuffer_no_attachments
	void bind_layers( Texture* t0, GLint mipLevel0, Texture* t1=nullptr, GLint mipLevel1=0, Texture* t2=nullptr, GLint mipLevel2=0, Texture* t3=nullptr, GLint mipLevel3=0, Texture* t4=nullptr, GLint mipLevel4=0, Texture* t5=nullptr, GLint mipLevel5=0, Texture* t6=nullptr, GLint mipLevel6=0, Texture* t7=nullptr, GLint mipLevel7=0 ); // t needs to be multi-layers
	void bind_layers( Texture* t0, Texture* t1=nullptr, Texture* t2=nullptr, Texture* t3=nullptr, Texture* t4=nullptr, Texture* t5=nullptr, Texture* t6=nullptr, Texture* t7=nullptr ){ bind_layers(t0,0,t1,0,t2,0,t3,0,t4,0,t5,0,t6,0,t7,0); } // t needs to be multi-layers
	void bind_depth_buffer( GLint width, GLint height, GLint layers=1, bool multisample=false, GLsizei multisamples=1 );
	static void unbind(){ glBindFramebuffer( GL_FRAMEBUFFER, 0 ); }
	void unbind_depth_buffer();
	void check_status(){ GLenum s=glCheckNamedFramebufferStatus?glCheckNamedFramebufferStatus(ID,GL_FRAMEBUFFER):glCheckFramebufferStatus(GL_FRAMEBUFFER); if(s==GL_FRAMEBUFFER_COMPLETE) return; if(s==GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) printf( "[%s] GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT\n", _name ); else if(s==GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT) printf( "[%s] GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT\n", _name ); else if(s==GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER) printf( "[%s] GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER\n", _name ); else if(s==GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER) printf( "[%s] GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER\n", _name ); else if(s==GL_FRAMEBUFFER_UNSUPPORTED) printf( "[%s] GL_FRAMEBUFFER_UNSUPPORTED\n", _name ); }
	void read_pixels( GLenum attachment, GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid* img ){ if(ID) glNamedFramebufferReadBuffer(ID,GL_COLOR_ATTACHMENT0+attachment); else glReadBuffer(GL_BACK); glReadPixels( x, y, width, height, format, type, img ); }
	void set_viewport( GLint x, GLint y, GLsizei width, GLsizei height ){ glViewport( x, y, width, height ); }
	void set_viewport( ivec4 origin_size ){ glViewport( origin_size.x, origin_size.y, origin_size.z, origin_size.w ); }
	void set_viewport( ivec2 origin, ivec2 size ){ glViewport( origin.x, origin.y, size.x, size.y ); }
	void set_viewport( ivec2 size ){ glViewport( 0, 0, size.x, size.y ); }
	ivec4 get_viewport(){ ivec4 i={}; glGetIntegerv( GL_VIEWPORT, (int*)&i.x); return i; }
	void set_color_mask( bool mask, bool b_force_mask=false ){ if(!b_force_mask&&mask==_b_color_mask) return; GLboolean b=(_b_color_mask=mask)?GL_TRUE:GL_FALSE; glColorMask(b,b,b,b); }
	void set_depth_mask( bool mask, bool b_force_mask=false ){ if(!b_force_mask&&mask==_b_depth_mask) return; GLboolean b=(_b_depth_mask=mask)?GL_TRUE:GL_FALSE; glDepthMask(b); }
	void set_color_depth_mask( bool mask, bool b_force_mask=false ){ set_color_mask(mask,b_force_mask); set_depth_mask(mask,b_force_mask); }
	void set_read_buffer( GLenum mode ){ if(glNamedFramebufferReadBuffer) glNamedFramebufferReadBuffer( ID, mode ); else glReadBuffer( mode ); }
	void set_draw_buffer( GLenum mode ){ if(glNamedFramebufferDrawBuffer) glNamedFramebufferDrawBuffer( ID, mode ); else glDrawBuffer( mode ); }
		
	static void set_state( bool b_depth_test, bool b_cull_face, bool b_blend=false, bool b_wireframe=false ){ b_depth_test?glEnable(GL_DEPTH_TEST):glDisable(GL_DEPTH_TEST); b_cull_face?glEnable(GL_CULL_FACE):glDisable(GL_CULL_FACE); b_blend?glEnable(GL_BLEND):glDisable(GL_BLEND); glPolygonMode(GL_FRONT_AND_BACK,b_wireframe?GL_LINE:GL_FILL); }
	static void get_state( bool* b_depth_test, bool* b_cull_face, bool* b_blend, bool* b_wireframe=nullptr ){ if(b_depth_test) *b_depth_test=gxGetBooleanv(GL_DEPTH_TEST); if(b_cull_face) *b_cull_face=gxGetBooleanv(GL_CULL_FACE); if(b_blend) *b_blend=gxGetBooleanv(GL_BLEND); if(b_wireframe){ GLint pm[2]; glGetIntegerv(GL_POLYGON_MODE,pm); *b_wireframe=pm[0]==GL_LINE; } }
	void push_state(){ GLint pm[2]; glGetIntegerv(GL_POLYGON_MODE,pm); _state_stack={gxGetBooleanv(GL_DEPTH_TEST),gxGetBooleanv(GL_CULL_FACE),gxGetBooleanv(GL_BLEND),pm[0]==GL_LINE,pm[1]==GL_LINE}; }
	void pop_state(){ set_state(_state_stack.depth_test,_state_stack.cull_face,_state_stack.blend,_state_stack.wireframe[0]); if(_state_stack.wireframe[0]!=_state_stack.wireframe[1])glPolygonMode(GL_BACK,_state_stack.wireframe[1]?GL_LINE:GL_FILL); }

	static void set_blend_func( GLenum sfactor, GLenum dfactor ){ glBlendFunc( sfactor, dfactor ); }
	static void set_blend_func_separate( GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha ){ glBlendFuncSeparate( sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha ); }
	static void set_multisample( bool b_multisample ){ b_multisample ? glEnable(GL_MULTISAMPLE):glDisable(GL_MULTISAMPLE); }
	static const GLenum* draw_buffers( uint index=0 ){ static GLenum d[MAX_COLOR_ATTACHMENTS]={GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2,GL_COLOR_ATTACHMENT3,GL_COLOR_ATTACHMENT4,GL_COLOR_ATTACHMENT5,GL_COLOR_ATTACHMENT6,GL_COLOR_ATTACHMENT7}; return &d[index]; }
	void clear( const vec4& color=0 ){ for( uint k=0;k<MAX_COLOR_ATTACHMENTS;k++) if(_active_targets[k]) clear_color_buffer( k, color, false ); clear_depth_buffer(); }
	void clear_depth_buffer( float depth=1.0f ){ if(glClearNamedFramebufferfv) glClearNamedFramebufferfv( ID, GL_DEPTH, 0, &depth ); else glClearBufferfv( GL_DEPTH, 0, &depth ); }
	void clear_color_buffer( GLint draw_buffer, const vec4& color, bool b_clear_depth=false ){ if(draw_buffer>=GL_DRAW_BUFFER0) draw_buffer-=GL_DRAW_BUFFER0; if(glClearNamedFramebufferfv) glClearNamedFramebufferfv( ID, GL_COLOR, draw_buffer, (GLfloat*)&color ); else glClearBufferfv( GL_COLOR, draw_buffer, color ); if(b_clear_depth) clear_depth_buffer(); }
	void clear_color_bufferi( GLint draw_buffer, const ivec4& color, bool b_clear_depth=false ){ if(draw_buffer>=GL_DRAW_BUFFER0) draw_buffer-=GL_DRAW_BUFFER0; if(glClearNamedFramebufferiv) glClearNamedFramebufferiv( ID, GL_COLOR, draw_buffer, color ); else glClearBufferiv( GL_COLOR, draw_buffer, color ); if(b_clear_depth) clear_depth_buffer(); }
	void clear_color_bufferui( GLint draw_buffer, const uvec4& color, bool b_clear_depth=false ){ if(draw_buffer>=GL_DRAW_BUFFER0) draw_buffer-=GL_DRAW_BUFFER0; if(glClearNamedFramebufferuiv) glClearNamedFramebufferuiv( ID, GL_COLOR, draw_buffer, color ); else glClearBufferuiv( GL_COLOR, draw_buffer, color ); if(b_clear_depth) clear_depth_buffer(); }
	
protected:
	struct state_t { bool depth_test=true, cull_face=true, blend=false, wireframe[2]={false,false}; } _state_stack;
	bool _b_color_mask=true, _b_depth_mask=true;
	GLenum _active_targets[MAX_COLOR_ATTACHMENTS];
	std::map<uint64_t,GLuint> _depth_buffers;
};

//***********************************************
inline void Framebuffer::bind( Texture* t0, GLint layer0, GLint mipLevel0, Texture* t1, GLint layer1, GLint mipLevel1, Texture* t2, GLint layer2, GLint mipLevel2, Texture* t3, GLint layer3, GLint mipLevel3, Texture* t4, GLint layer4, GLint mipLevel4, Texture* t5, GLint layer5, GLint mipLevel5, Texture* t6, GLint layer6, GLint mipLevel6, Texture* t7, GLint layer7, GLint mipLevel7 )
{
	if(ID==0){ bind(false); return; }

	// still necessary, regardless of direct_state_access, because draw functions are not aware where they are drawn.
	glBindFramebuffer( GL_FRAMEBUFFER, ID );

	gl::Texture*	T[MAX_COLOR_ATTACHMENTS]	= {t0,t1,t2,t3,t4,t5,t6,t7};
	GLint			L[MAX_COLOR_ATTACHMENTS]	= {layer0,layer1,layer2,layer3,layer4,layer5,layer6,layer7};
	GLint			M[MAX_COLOR_ATTACHMENTS]	= {mipLevel0,mipLevel1,mipLevel2,mipLevel3,mipLevel4,mipLevel5,mipLevel6,mipLevel7};

	uint draw_buffer_count=0;
	for( int k=0; k < MAX_COLOR_ATTACHMENTS; k++ )
	{
		gl::Texture* t = T[k];
		GLenum target = t ? (_active_targets[k]=t->target) : _active_targets[k];
		if(target==GL_TEXTURE_3D||target==GL_TEXTURE_1D_ARRAY||target==GL_TEXTURE_2D_ARRAY||target==GL_TEXTURE_2D_MULTISAMPLE_ARRAY||target==GL_TEXTURE_CUBE_MAP||target==GL_TEXTURE_CUBE_MAP_ARRAY) glFramebufferTextureLayer( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+k, t?t->ID:0, t?M[k]:0, t?L[k]:0 );
		else if(target==GL_TEXTURE_1D||target==GL_TEXTURE_2D||target==GL_TEXTURE_2D_MULTISAMPLE||target==GL_TEXTURE_BUFFER||target==GL_TEXTURE_RECTANGLE) glFramebufferTexture( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+k, t?t->ID:0, t?M[k]:0 );
		if(t) draw_buffer_count++; else _active_targets[k]=0;
	}

	// detach all depth buffers for no-attachments
	if(draw_buffer_count==0){ unbind_depth_buffer(); return; } // detach all color/depth buffers for no-attachments

	// if there are attachments, set viewport and bind depth buffer
	GLint width=t0->width(mipLevel0), height=t0->height(mipLevel0);
	glViewport( 0, 0, width, height );
		
	// bind depth buffer, when depth test is enabled
	bool b_multisample = _active_targets[0]==GL_TEXTURE_2D_MULTISAMPLE||_active_targets[0]==GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
	bind_depth_buffer( width, height, 1, b_multisample, b_multisample&&T[0]?T[0]->multisamples():1 );
		
	// set the draw buffers, even without attachments; then check the status
	if(glNamedFramebufferDrawBuffers)	glNamedFramebufferDrawBuffers( ID, draw_buffer_count, draw_buffers() );
	else								glDrawBuffers( draw_buffer_count, draw_buffers() );
	check_status();
}

inline void Framebuffer::bind_layers( Texture* t0, GLint mipLevel0, Texture* t1, GLint mipLevel1, Texture* t2, GLint mipLevel2, Texture* t3, GLint mipLevel3, Texture* t4, GLint mipLevel4, Texture* t5, GLint mipLevel5, Texture* t6, GLint mipLevel6, Texture* t7, GLint mipLevel7 ) // for gl_Layer redirection in geometry shader
{
	if(ID==0||t0==nullptr){ bind(false); return; }

	if(t0->target!=GL_TEXTURE_1D_ARRAY&&t0->target!=GL_TEXTURE_2D_ARRAY&&t0->target!=GL_TEXTURE_2D_MULTISAMPLE_ARRAY&&t0->target!=GL_TEXTURE_3D&&t0->target!=GL_TEXTURE_CUBE_MAP&&t0->target!=GL_TEXTURE_CUBE_MAP_ARRAY){ printf("[%s] Framebuffer::bind_layers() supports only array textures\n", _name ); return; }

	// regardless of direct_state_access, it is still necessary, because draw functions are not aware where they are drawn.
	glBindFramebuffer( GL_FRAMEBUFFER, ID );

	gl::Texture*	T[MAX_COLOR_ATTACHMENTS] = { t0, t1, t2, t3, t4, t5, t6, t7 };
	GLint			M[MAX_COLOR_ATTACHMENTS] = { mipLevel0, mipLevel1, mipLevel2, mipLevel3, mipLevel4, mipLevel5, mipLevel6, mipLevel7 };

	uint num_draw_buffers=0;
	for( int k=0; k < MAX_COLOR_ATTACHMENTS; k++ )
	{
		gl::Texture* t = T[k];
		glFramebufferTexture( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+k, t?t->ID:0, t?M[k]:0 );
		if(t){ num_draw_buffers++; _active_targets[k] = t->target; }
		else _active_targets[k] = 0;
	}

	// if there are attachments, set viewport and bind depth buffer
	if(num_draw_buffers>0)
	{
		GLint width=t0->width(mipLevel0), height=t0->height(mipLevel0);
		glViewport( 0, 0, width, height );
		bool b_multisample = t0->target==GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
		bind_depth_buffer( width, height, t0->layers(), b_multisample, b_multisample&&t0?t0->multisamples():1 );
	}

	// set the draw buffers, even without attachments; then check the status
	if(glNamedFramebufferDrawBuffers)	glNamedFramebufferDrawBuffers( ID, num_draw_buffers, draw_buffers() );
	else								glDrawBuffers( num_draw_buffers, draw_buffers() );
	check_status();
}

inline void Framebuffer::bind_depth_buffer( GLint width, GLint height, GLint layers, bool multisample, GLsizei multisamples )
{
	if(this->ID==0) return;

	depth_key_t key; key.width=width; key.height=height; key.multisamples=multisample?multisamples:1; key.renderbuffer=layers==1?1:0; key.layers=layers; key.dummy=0;

	GLuint idx;
	auto it=_depth_buffers.find(key.value); if(it!=_depth_buffers.end()) idx=it->second;
	else if(layers==1)
	{
		glBindRenderbuffer( GL_RENDERBUFFER, _depth_buffers[key.value] = idx = gxCreateRenderBuffer() );
		if(multisample)	glNamedRenderbufferStorageMultisample?glNamedRenderbufferStorageMultisample( idx, multisamples, GL_DEPTH_COMPONENT32F, width, height ):glRenderbufferStorageMultisample( GL_RENDERBUFFER, multisamples, GL_DEPTH_COMPONENT32F, width, height );
		else			glNamedRenderbufferStorage?glNamedRenderbufferStorage( idx, GL_DEPTH_COMPONENT32F, width, height ):glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, width, height );
	}
	else
	{
		GLenum target=multisample?GL_TEXTURE_2D_MULTISAMPLE_ARRAY:GL_TEXTURE_2D_ARRAY; glBindTexture( target, _depth_buffers[key.value] = idx = gxCreateTexture(target) );
		if(multisample)	glTexStorage3DMultisample( target, multisamples, GL_DEPTH_COMPONENT32, width, height, layers, GL_TRUE );
		else			glTexStorage3D( target, 1, GL_DEPTH_COMPONENT32, width, height, layers );
	}

	if(layers==1)
	{
		if(glNamedFramebufferRenderbuffer) glNamedFramebufferRenderbuffer( ID, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, idx );
		else { glBindFramebuffer(GL_FRAMEBUFFER,ID); glBindRenderbuffer(GL_RENDERBUFFER,idx); glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, idx ); }
	}
	else
	{
		if(glNamedFramebufferTexture) glNamedFramebufferTexture( ID, GL_DEPTH_ATTACHMENT, idx, 0 );
		else { glBindFramebuffer(GL_FRAMEBUFFER,ID); glBindRenderbuffer(GL_RENDERBUFFER,idx); glFramebufferTexture( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, idx, 0 ); }
	}
}

inline void Framebuffer::unbind_depth_buffer()
{
	glBindRenderbuffer( GL_RENDERBUFFER, 0 );
	glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0 );
	glFramebufferTexture( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 0, 0 );
}

inline void Framebuffer::bind_no_attachments( GLint width, GLint height, GLint layers, GLint samples )
{
	// bind FBO without attachments: nothing will be written to the color/depth buffers
	if(glNamedFramebufferParameteri)
	{
		glNamedFramebufferParameteri( ID, GL_FRAMEBUFFER_DEFAULT_WIDTH, width );
		glNamedFramebufferParameteri( ID, GL_FRAMEBUFFER_DEFAULT_HEIGHT, height );
		glNamedFramebufferParameteri( ID, GL_FRAMEBUFFER_DEFAULT_LAYERS, layers );
		glNamedFramebufferParameteri( ID, GL_FRAMEBUFFER_DEFAULT_SAMPLES, samples );
		glNamedFramebufferParameteri( ID, GL_FRAMEBUFFER_DEFAULT_FIXED_SAMPLE_LOCATIONS, GL_TRUE );
	}
	else
	{
		glBindFramebuffer(GL_FRAMEBUFFER,ID);
		glFramebufferParameteri( GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_WIDTH, width );
		glFramebufferParameteri( GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_HEIGHT, height );
		glFramebufferParameteri( GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_LAYERS, layers );
		glFramebufferParameteri( GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_SAMPLES, samples );
		glFramebufferParameteri( GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_FIXED_SAMPLE_LOCATIONS, GL_TRUE );
	}

	bind(nullptr,0,0); // detach all
	glViewport( 0, 0, width, height ); // set viewport
}

struct Uniform
{
	GLint		ID=-1;
	GLchar		name[256]={};
	GLint		array_size=1;
	GLenum		type=0;
	GLint		block_index=-1;		// uniform block index to which this belongs
	GLint		block_offset=-1;	// only for uniform-block variables
	GLchar		block_name[256]={};
	GLint		textureID=-1;
	Texture*	texture=nullptr;
	GLint		binding=-1;	// only for image texture unit

	const char* type_name()
	{
		switch(type)
		{
			case GL_FLOAT: return "float"; case GL_FLOAT_VEC2: return "vec2"; case GL_FLOAT_VEC3: return "vec3"; case GL_FLOAT_VEC4: return "vec4";
			case GL_INT: return "int"; case GL_INT_VEC2: return "ivec2"; case GL_INT_VEC3: return "ivec3"; case GL_INT_VEC4: return "ivec4";
			case GL_UNSIGNED_INT: return "uint"; case GL_UNSIGNED_INT_VEC2: return "uvec2"; case GL_UNSIGNED_INT_VEC3: return "uvec3"; case GL_UNSIGNED_INT_VEC4: return "uvec4";
			case GL_BOOL: return "bool"; case GL_BOOL_VEC2: return "bvec2"; case GL_BOOL_VEC3: return "bvec3"; case GL_BOOL_VEC4: return "bvec4";
			case GL_FLOAT_MAT2: return "mat2"; case GL_FLOAT_MAT3: return "mat3"; case GL_FLOAT_MAT4: return "mat4";
		}
		return "unknown";
	}

	const char* get_value( GLuint prog )
	{
		if(ID==-1) return "";
		static float4 f; static int4 i; static uint4 u; static float m[16];
			
		switch(type)
		{
		case GL_FLOAT:				glGetUniformfv(prog,ID,&f.x); return format("%g",f.x);
		case GL_FLOAT_VEC2:			glGetUniformfv(prog,ID,&f.x); return format("%g %g",f.x,f.y);
		case GL_FLOAT_VEC3:			glGetUniformfv(prog,ID,&f.x); return format("%g %g %g",f.x,f.y,f.z);
		case GL_FLOAT_VEC4:			glGetUniformfv(prog,ID,&f.x); return format("%g %g %g %g",f.x,f.y,f.z,f.w);
		case GL_INT:				glGetUniformiv(prog,ID,&i.x); return format("%d",i.x);
		case GL_INT_VEC2:			glGetUniformiv(prog,ID,&i.x); return format("%d %d",i.x,i.y);
		case GL_INT_VEC3:			glGetUniformiv(prog,ID,&i.x); return format("%d %d %d",i.x,i.y,i.z);
		case GL_INT_VEC4:			glGetUniformiv(prog,ID,&i.x); return format("%d %d %d %d",i.x,i.y,i.z,i.w);
		case GL_UNSIGNED_INT:		glGetUniformuiv(prog,ID,&u.x); return format("%u",u.x);
		case GL_UNSIGNED_INT_VEC2:	glGetUniformuiv(prog,ID,&u.x); return format("%u %u",u.x,u.y);
		case GL_UNSIGNED_INT_VEC3:	glGetUniformuiv(prog,ID,&u.x); return format("%u %u %u",u.x,u.y,u.z);
		case GL_UNSIGNED_INT_VEC4:	glGetUniformuiv(prog,ID,&u.x); return format("%u %u %u %u",u.x,u.y,u.z,u.w);
		case GL_BOOL:				glGetUniformiv(prog,ID,&i.x); return format("%d",i.x);
		case GL_BOOL_VEC2:			glGetUniformiv(prog,ID,&i.x); return format("%d %d",i.x,i.y);
		case GL_BOOL_VEC3:			glGetUniformiv(prog,ID,&i.x); return format("%d %d %d",i.x,i.y,i.z);
		case GL_BOOL_VEC4:			glGetUniformiv(prog,ID,&i.x); return format("%d %d %d %d",i.x,i.y,i.z,i.w);
		case GL_FLOAT_MAT2:			glGetUniformfv(prog,ID,m); return format("%g %g %g %g",m[0],m[2],m[1],m[3]);
		case GL_FLOAT_MAT3:			glGetUniformfv(prog,ID,m); return format("%g %g %g %g %g %g %g %g %g",m[0],m[3],m[6],m[1],m[4],m[7],m[2],m[5],m[8]);
		case GL_FLOAT_MAT4:			glGetUniformfv(prog,ID,m); return format("%g %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g", m[0],m[4],m[8],m[12],m[1],m[5],m[9],m[13],m[2],m[6],m[10],m[14],m[3],m[7],m[11],m[15]);
		}
		return "unknown value";
	}

	template <class T> void set( GLuint prog, const T* ptr, GLsizei count );
	bool is_matrix(){ return (type>=GL_FLOAT_MAT2&&type<=GL_FLOAT_MAT4)||(type>=GL_FLOAT_MAT2x3&&type<=GL_FLOAT_MAT4x3); }

protected:
	void __set_log( const char* func, void* ptr, GLsizei count ){ printf( "Uniform[%s].%s(%p,%d): unsupported type\n", name, func, ptr, count ); }
};

template<> inline void Uniform::set<int>( GLuint prog, const int* ptr, GLsizei count )
{
	if(ID==-1) return;
	switch(type)
	{
	case GL_INT:				return glProgramUniform1iv( prog, ID, count, (const GLint*)ptr );
	case GL_UNSIGNED_INT:		return glProgramUniform1uiv( prog, ID, count, (const GLuint*)ptr );
	case GL_BOOL:				return glProgramUniform1iv( prog, ID, count, (const GLint*)ptr );
	}
	__set_log(__func__,(void*)ptr,count);
}

template<> inline void Uniform::set<ivec2>( GLuint prog, const ivec2* ptr, GLsizei count )
{
	if(ID==-1) return;
	switch(type)
	{
	case GL_INT_VEC2:			return glProgramUniform2iv( prog, ID, count, (const GLint*)ptr );
	case GL_UNSIGNED_INT_VEC2:	return glProgramUniform2uiv( prog, ID, count, (const GLuint*)ptr );
	case GL_BOOL_VEC2:			return glProgramUniform2iv( prog, ID, count, (const GLint*)ptr );
	}
	__set_log(__func__,(void*)ptr,count);
}

template<> inline void Uniform::set<ivec3>( GLuint prog, const ivec3* ptr, GLsizei count )
{
	if(ID==-1) return;
	switch(type)
	{
	case GL_INT_VEC3:			return glProgramUniform3iv( prog, ID, count, (const GLint*)ptr );
	case GL_UNSIGNED_INT_VEC3:	return glProgramUniform3uiv( prog, ID, count, (const GLuint*)ptr );
	case GL_BOOL_VEC3:			return glProgramUniform3iv( prog, ID, count, (const GLint*)ptr );
	}
	__set_log(__func__,(void*)ptr,count);
}

template<> inline void Uniform::set<ivec4>( GLuint prog, const ivec4* ptr, GLsizei count )
{
	if(ID==-1) return;
	switch(type)
	{
	case GL_INT_VEC4:			return glProgramUniform4iv( prog, ID, count, (const GLint*)ptr );
	case GL_UNSIGNED_INT_VEC4:	return glProgramUniform4uiv( prog, ID, count, (const GLuint*)ptr );
	case GL_BOOL_VEC4:			return glProgramUniform4iv( prog, ID, count, (const GLint*)ptr );
	}
	__set_log(__func__,(void*)ptr,count);
}

template<> inline void Uniform::set<size_t>( GLuint prog, const size_t* ptr, GLsizei count ){ set<int>(prog,(int*)ptr,count); }
template<> inline void Uniform::set<uint>( GLuint prog, const uint* ptr, GLsizei count ){ set<int>(prog,(int*)ptr,count); }
template<> inline void Uniform::set<uvec2>( GLuint prog, const uvec2* ptr, GLsizei count ){ set<ivec2>(prog,(ivec2*)ptr,count); }
template<> inline void Uniform::set<uvec3>( GLuint prog, const uvec3* ptr, GLsizei count ){ set<ivec3>(prog,(ivec3*)ptr,count); }
template<> inline void Uniform::set<uvec4>( GLuint prog, const uvec4* ptr, GLsizei count ){ set<ivec4>(prog,(ivec4*)ptr,count); }
template<> inline void Uniform::set<float>( GLuint prog, const float* ptr, GLsizei count ){ if(ID==-1) return; if(type==GL_FLOAT) return glProgramUniform1fv( prog, ID, count, (const GLfloat*)ptr ); __set_log(__func__,(void*)ptr,count); }
template<> inline void Uniform::set<vec2>( GLuint prog, const vec2* ptr, GLsizei count ){ if(ID==-1) return; if(type==GL_FLOAT_VEC2) return glProgramUniform2fv( prog, ID, count, (const GLfloat*)ptr ); __set_log(__func__,(void*)ptr,count); }
template<> inline void Uniform::set<vec3>( GLuint prog, const vec3* ptr, GLsizei count ){ if(ID==-1) return; if(type==GL_FLOAT_VEC3) return glProgramUniform3fv( prog, ID, count, (const GLfloat*)ptr ); __set_log(__func__,(void*)ptr,count); }
template<> inline void Uniform::set<vec4>( GLuint prog, const vec4* ptr, GLsizei count ){ if(ID==-1) return; if(type==GL_FLOAT_VEC4) return glProgramUniform4fv( prog, ID, count, (const GLfloat*)ptr ); __set_log(__func__,(void*)ptr,count); }
template<> inline void Uniform::set<mat2>( GLuint prog, const mat2* ptr, GLsizei count ){ if(ID==-1) return; if(type==GL_FLOAT_MAT2) return glProgramUniformMatrix2fv( prog, ID, count, GL_TRUE, (const GLfloat*)ptr ); __set_log(__func__,(void*)ptr,count); }
template<> inline void Uniform::set<mat3>( GLuint prog, const mat3* ptr, GLsizei count ){ if(ID==-1) return; if(type==GL_FLOAT_MAT3) return glProgramUniformMatrix3fv( prog, ID, count, GL_TRUE, (const GLfloat*)ptr ); __set_log(__func__,(void*)ptr,count); }
template<> inline void Uniform::set<mat4>( GLuint prog, const mat4* ptr, GLsizei count ){ if(ID==-1) return; if(type==GL_FLOAT_MAT4) return glProgramUniformMatrix4fv( prog, ID, count, GL_TRUE, (const GLfloat*)ptr ); __set_log(__func__,(void*)ptr,count); }
template<> inline void Uniform::set<int2>( GLuint prog, const int2* ptr, GLsizei count ){ set<ivec2>(prog,(ivec2*)ptr,count); }
template<> inline void Uniform::set<int3>( GLuint prog, const int3* ptr, GLsizei count ){ set<ivec3>(prog,(ivec3*)ptr,count); }
template<> inline void Uniform::set<int4>( GLuint prog, const int4* ptr, GLsizei count ){ set<ivec4>(prog,(ivec4*)ptr,count); }
template<> inline void Uniform::set<uint2>( GLuint prog, const uint2* ptr, GLsizei count ){ set<uvec2>(prog,(uvec2*)ptr,count); }
template<> inline void Uniform::set<uint3>( GLuint prog, const uint3* ptr, GLsizei count ){ set<uvec3>(prog,(uvec3*)ptr,count); }
template<> inline void Uniform::set<uint4>( GLuint prog, const uint4* ptr, GLsizei count ){ set<uvec4>(prog,(uvec4*)ptr,count); }
template<> inline void Uniform::set<float2>( GLuint prog, const float2* ptr, GLsizei count ){ set<vec2>(prog,(vec2*)ptr,count); }
template<> inline void Uniform::set<float3>( GLuint prog, const float3* ptr, GLsizei count ){ set<vec3>(prog,(vec3*)ptr,count); }
template<> inline void Uniform::set<float4>( GLuint prog, const float4* ptr, GLsizei count ){ set<vec4>(prog,(vec4*)ptr,count); }

//***********************************************
// shader/program/effect source structures
struct shader_macro_t : public vector<string>
{
	void append( __printf_format_string__ const char* m, ... ){ char b[4096];va_list a;va_start(a,m);size_t len=size_t(vsnprintf(0,0,m,a));vsnprintf(b,len+1,m,a);va_end(a); for(auto& s:*this) if(strcmp(s.c_str(),b)==0) return; emplace_back(b); }
	string merge() const { string m; for(auto& s:*this){ m+=s; if(s.back()!='\n') m+='\n'; } if(!m.empty()) m+='\n'; return m; }
};
struct named_string_t { string name, value; };
struct shader_source_t : public vector<named_string_t> // a list of source strings
{
	using vector<named_string_t>::vector;		// inherit ctors
	using vector<named_string_t>::operator=;	// inherit operator=
	string flatten() const
	{
		static const bool TRIM_LINE_DIRECTIVE=true;
		static const bool TRIM_TRIPLE_NEWLINES=true;

		string f;
		for( size_t k=0; k < size(); k++ )
		{
			for( auto& l : explode_conservative(at(k).value.c_str(),'\n') )
			{
				if(TRIM_LINE_DIRECTIVE&&strncmp(l.c_str(), "#line", strlen("#line"))==0) continue;
				if(strncmp(l.c_str(), " in ", strlen(" in "))==0||strncmp(l.c_str(), " out ", strlen(" out "))==0) l=l.substr(1);
				f += l; f+="\n";
			}
		}

		if(TRIM_TRIPLE_NEWLINES){ for(int k=0;k<16&&strstr(f.c_str(),"\n\n\n");k++) f=str_replace(f.c_str(),"\n\n\n","\n\n"); }
		f = str_replace(f.c_str(),";\nvoid main()\n\n{", ";\n\nvoid main()\n{" );
			
		return f;
	}
		
	void flatten( const char* file_path ) const
	{
		path_t d=path_t(file_path).dir(); if(!d.exists()) d.mkdir();
		FILE* fp=fopen(file_path,"wb"); if(!fp) return;
		string f=flatten(); fwrite(f.c_str(),1,f.size(),fp); fclose(fp);
		fclose(fp);
	}
};

struct program_source_t : public std::map<GLuint,shader_source_t> // <shader_type,shader_source_t>
{
	using std::map<GLuint,shader_source_t>::map;		// inherit ctors
	using std::map<GLuint,shader_source_t>::operator=;	// inherit operator=

	shader_source_t get_shader_source( GLuint shader_type ) const { auto it=find(shader_type); return it==end()?shader_source_t():it->second; }
	void export_shader_sources( const char* dir, const char* name ){ for( auto it : *this ) export_shader_source( it.first, dir, name ); }
	void export_shader_source( GLuint shader_type, const char* dir, const char* name, bool b_print_log=true )
	{
		const char* ext = get_shader_extension_name( shader_type ); if(!ext||!*ext){ printf( "unable to find shader_type %d\n", int(shader_type) ); return; }
		string file_path = string(append_slash(dir))+name+"."+ext;
		if(b_print_log) printf( "%s.%s\n", name, ext );
		get_shader_source(shader_type).flatten( file_path.c_str() );
	}
	const char* get_shader_extension_name( GLuint shader_type ) const
	{
		// extension names: https://www.khronos.org/opengles/sdk/tools/Reference-Compiler/
		static const std::map<GLuint,string> m = { {GL_VERTEX_SHADER, "vert"}, {GL_FRAGMENT_SHADER, "frag"}, {GL_GEOMETRY_SHADER, "geom"}, {GL_TESS_EVALUATION_SHADER, "tesc"}, {GL_TESS_CONTROL_SHADER, "tese"}, {GL_COMPUTE_SHADER, "comp"}, };
		auto it=m.find(shader_type); return it==m.end() ? "":it->second.c_str();
	}
};

//***********************************************
struct Program : public Object
{
	Program( GLuint ID, const char* name ) : Object(ID,name,GL_PROGRAM){ get_instances().emplace(this); }
	~Program() override { if(!ID) return; glDeleteProgram(ID); _uniform_cache.clear(); _invalid_uniform_cache.clear(); get_instances().erase(this); }
	static void unbind(){ glUseProgram(0); }
	GLuint bind( bool b_bind=true );

	// in-program uniform variables
	Uniform* get_uniform( const char* name )
	{
		{auto it=_uniform_cache.find(name);if(it!=_uniform_cache.end()) return &it->second;}
#ifdef WARN_INVALID_UNIFORM
		{auto it=invalid_uniform_cache.find(name);if(it!=invalid_uniform_cache.end()) return nullptr;}
		printf( "[Warning] %s.get_uniform(%s) not exists\n",this->_name, name );
		invalid_uniform_cache.emplace(name);
#endif
		return nullptr;
	}

	// special set_uniform functions: vector, texture, and bool
	template <class T> void set_uniform( const char* name, T* v, GLsizei count=1 ){ Uniform* u=get_uniform(name); if(u) u->set(ID,v,count); }
	template <class T> void set_uniform( const char* name, const T& v ){ Uniform* u=get_uniform(name); if(u) u->set(ID,&v,1); }
	template <class T> void set_uniform( const char* name, const vector<T>& v ){ set_uniform( name, v.data(), GLsizei(v.size()) ); }
	void set_uniform( const char* name, Texture* t ){ if(!t) return; Uniform* u=get_uniform(name); if(!u) return; if(u->ID<0||u->textureID<0) return; if(binding()!=ID) glUseProgram(ID); u->texture=t; glProgramUniform1i(ID,u->ID,u->textureID); if(glBindTextureUnit) glBindTextureUnit(u->textureID,u->texture->ID); else { glActiveTexture(GL_TEXTURE0+u->textureID);u->texture->bind();} }
	void set_uniform( const char* name, bool b ){ Uniform* u=get_uniform(name); if(!u) return; int v=b?1:0; u->set(ID,(int*)&v,1); }

	// bind image texture
	void bind_image_texture( const char* name, Texture* t, GLenum access=GL_READ_WRITE /* or GL_WRITE_ONLY or GL_READ_ONLY */, GLint level=0, GLenum format=0, bool bLayered=false, GLint layer=0 )
	{
		Uniform* u=get_uniform(name); if(!u||u->ID<0||u->binding<0){ printf( "%s(): %s not found\n", __func__, name ); return; }
		u->texture=t; glBindImageTexture( u->binding, t->ID, level, bLayered?GL_TRUE:GL_FALSE, layer, access, format?format:t->internal_format() );
	}

	// uniform cache/block/dump
	void update_uniform_cache();
	const char* dump_uniforms( bool b_matrix=false, bool b_array=false );

	// uniform block, used with uniform buffer: uniform buffer is assigned from effect to share it with other programs
	struct UniformBlock
	{
		GLuint ID=-1; GLchar name[256]={}; GLint size=0; Program* program=nullptr; gl::Buffer* buffer=nullptr;
		GLuint get_binding() const { return gxGetActiveUniformBlockiv(program->ID,ID, GL_UNIFORM_BLOCK_BINDING ); }
		void set_binding( GLuint binding_point ){ glUniformBlockBinding(program->ID,ID,binding_point); if(buffer&&buffer->target==GL_UNIFORM_BUFFER) buffer->bind_base(binding_point); }
	};

	// query for uniform, program, and compute
	GLint get_active_uniform_count(){ GLint count=0; if(glGetProgramInterfaceiv) glGetProgramInterfaceiv( ID, GL_UNIFORM, GL_ACTIVE_RESOURCES, &count ); else if(glGetProgramiv) glGetProgramiv( ID, GL_ACTIVE_UNIFORMS, &count ); return count; }
	vector<Uniform> get_active_uniforms( bool b_bind=true );
	UniformBlock* get_uniform_block( const char* name ){ auto it=_uniform_block_map.find(name); return it==_uniform_block_map.end() ? nullptr : &it->second; }
	GLuint get_uniform_block_binding( const char* name ){ auto* ub=get_uniform_block(name); return ub?ub->get_binding():-1; }
	GLuint get_shader_storage_block_binding( const char* name ){ auto it=_shader_storage_block_binding_map.find(name); if(it!=_shader_storage_block_binding_map.end()) return it->second; const GLenum prop=GL_BUFFER_BINDING; GLint binding;glGetProgramResourceiv(ID,GL_SHADER_STORAGE_BLOCK,glGetProgramResourceIndex(ID,GL_SHADER_STORAGE_BLOCK,name),1,&prop,1,nullptr,&binding); return _shader_storage_block_binding_map[name]=binding; }
	GLuint get_atomic_counter_buffer_binding( const char* name ){ auto it=_atomic_counter_buffer_binding_map.find(name); return it==_atomic_counter_buffer_binding_map.end()?-1:it->second; }
	ivec3 get_compute_work_group_size(){ return _compute_work_group_size.x?_compute_work_group_size:(_compute_work_group_size=gxGetProgramiv3(ID,GL_COMPUTE_WORK_GROUP_SIZE)); }
	bool assert_compute_work_group_size( int work_group_size_x, int work_group_size_y, int work_group_size_z=1 ){ auto d=get_compute_work_group_size(); if(work_group_size_x==d.x&&work_group_size_y==d.y&&work_group_size_z==d.z) return true; printf( "program[%s]: work_group_size (%d,%d,%d) != (%d,%d,%d)\n", name(), d.x, d.y, d.z, work_group_size_x, work_group_size_y, work_group_size_z ); return false; }

	// instances
	static std::set<Program*>& get_instances(){ static std::set<Program*> i; return i; }

	// friend classes/functions
	friend struct Effect;

	// public member variables
	program_source_t					source;

protected:
	std::map<string,Uniform>		_uniform_cache;
	std::set<string>				_invalid_uniform_cache;
	std::map<string,GLint>			_shader_storage_block_binding_map;
	std::map<string,GLint>			_atomic_counter_buffer_binding_map;
	std::map<string,UniformBlock>	_uniform_block_map;
	ivec3							_compute_work_group_size={}; // cache for get_compute_work_group_size()
};

// bind
inline GLuint Program::bind( bool b_bind )
{
	GLuint b0=binding(); if(!b_bind||b0!=ID) glUseProgram(b_bind?ID:0); if(!b_bind) return b0;
	if(_uniform_cache.empty()) update_uniform_cache(); else if(Texture::invalidated()){std::set<Program*>& s=get_instances();for(auto it:s)it->update_uniform_cache();Texture::invalidated()=false;}
	for(auto& it:_uniform_cache)
	{
		auto& n=it.first; auto& u=it.second;
		if(u.ID<0||!u.texture||u.textureID<0) continue;
		if(glBindTextureUnit) glBindTextureUnit(u.textureID,u.texture->ID); else { glActiveTexture(GL_TEXTURE0+u.textureID);glBindTexture(u.texture->target,u.texture->ID); }
	}
	return b0;
}

// late implementations of Program
inline vector<gl::Uniform> gl::Program::get_active_uniforms( bool b_bind )
{
	GLint program0=-1; if(b_bind&&glProgramUniform1i) glGetIntegerv(GL_CURRENT_PROGRAM,&program0); if(program0>=0) glUseProgram(ID);
	vector<Uniform> v;
	for(int k=0,kn=get_active_uniform_count();k<kn;k++)
	{
		static const GLenum pnames[]={GL_BLOCK_INDEX,GL_LOCATION,GL_NAME_LENGTH,GL_TYPE,GL_OFFSET,GL_ARRAY_SIZE,GL_ARRAY_STRIDE,GL_MATRIX_STRIDE,GL_IS_ROW_MAJOR,GL_ATOMIC_COUNTER_BUFFER_INDEX};
		static std::array<GLint,std::extent<decltype(pnames)>::value> values;
		glGetProgramResourceiv(ID,GL_UNIFORM,k,GLsizei(values.size()),pnames,GLsizei(values.size()),nullptr,&values[0]);
		static std::map<GLenum,GLint> prop; for(int j=0,jn=int(values.size());j<jn;j++) prop[pnames[j]]=values[j];
		Uniform u; u.block_index=prop[GL_BLOCK_INDEX]; u.ID=prop[GL_LOCATION]; u.type=prop[GL_TYPE]; u.block_offset=prop[GL_OFFSET]; u.array_size=prop[GL_ARRAY_SIZE]; u.binding=-1;
		glGetProgramResourceName(ID,GL_UNIFORM,k,prop[GL_NAME_LENGTH],nullptr,u.name);
		if(strstr(u.name,"gl_")) continue; // query name now, and skip for built-in gl variables
		if(u.ID<0&&u.block_index<0) continue; // invalid variables
		//if(u.block_index>=0) continue; // keep uniform-block variables as well
		if(prop[GL_ATOMIC_COUNTER_BUFFER_INDEX]!=-1){ int b; glGetActiveAtomicCounterBufferiv(ID,prop[GL_ATOMIC_COUNTER_BUFFER_INDEX],GL_ATOMIC_COUNTER_BUFFER_BINDING,&b); _atomic_counter_buffer_binding_map[u.name]=b; }
		v.emplace_back(u);
	}
	if(program0>=0) glUseProgram(program0); // restore the original program
	return v;
}

inline void gl::Program::update_uniform_cache()
{
	GLuint program0=-1; if(glProgramUniform1i) glGetIntegerv(GL_CURRENT_PROGRAM,(GLint*)&program0); if(program0!=ID) glUseProgram(ID);

	// update uniform variables
	int texture_id=0; for( auto& u : get_active_uniforms(false) )
	{
		if(gxIsImageType(u.type)) glGetUniformiv(ID,u.ID,&u.binding);
		bool b_texture = gxIsSamplerType(u.type); if(b_texture){ u.textureID=texture_id++; if(glProgramUniform1i)glProgramUniform1i(ID,u.ID,u.textureID);else glUniform1i(u.ID,u.textureID); } // setting sampler locations avoids validation error in Intel Compiler
		_uniform_cache[u.name]=u; if(u.array_size==1) continue;
		GLchar name[256]; strcpy(name,u.name); GLchar* bracket=strchr(name,'['); if(bracket) bracket[0]='\0';
		for( GLint loc=bracket?1:0;loc<u.array_size;loc++){ Uniform u1=u;sprintf(u1.name,"%s[%d]",name,loc);u1.ID=glGetUniformLocation(ID,u1.name);if(u1.ID==-1)continue;u1.array_size=u.array_size-loc;if(b_texture)u1.textureID=texture_id++;_uniform_cache[u1.name]=u1; }
	}

	// update uniform blocks
	_uniform_block_map.clear();
	for(int k=0,kn=gxGetProgramiv(ID,GL_ACTIVE_UNIFORM_BLOCKS);k<kn;k++)
	{
		UniformBlock ub; ub.ID=k; ub.program=this; ub.size=gxGetActiveUniformBlockiv(ID,k,GL_UNIFORM_BLOCK_DATA_SIZE);
		GLsizei l=gxGetActiveUniformBlockiv(ID,k,GL_UNIFORM_BLOCK_NAME_LENGTH); /* length includes NULL */ if(l>GLsizei(std::extent<decltype(UniformBlock::name)>::value)) printf("[%s] uniform block name is too long\n",_name);
		glGetActiveUniformBlockName(ID,k,l,&l,ub.name);
		_uniform_block_map[ub.name] = ub;
		for(auto& it:_uniform_cache){ auto&n=it.first;auto&u=it.second; if(u.block_index==GLint(ub.ID)) strcpy(u.block_name,ub.name); } // update uniform block in uniform cache
	}

	if(program0!=ID&&program0>=0) glUseProgram(program0); // restore the original program
}

inline const char* gl::Program::dump_uniforms( bool b_matrix, bool b_array )
{
	static string buff; buff.clear();
	int texID=0; for( auto& u : get_active_uniforms(false) )
	{
		if(!b_array&&strchr(u.name,'[')) continue;
		if(!b_matrix&&u.is_matrix()) continue;
		buff += format( "[%04d] %s %s%s %s\n", u.ID, u.type_name(), u.name, u.array_size>1?format("[%d]",u.array_size):"", u.get_value(ID) );
	}
	return buff.c_str();
}

// string with index in a source
struct indexed_string_t : public string
{
	int index=0;
	using string::string;		// inherit ctors
	using string::operator=;	// inherit operator=
};

struct effect_source_t : public vector<named_string_t>
{
	shader_macro_t macro; // embedded macro

	effect_source_t() = default;
	effect_source_t( const vector<value_type>& v ){ reinterpret_cast<vector<value_type>&>(*this)=v; }
	effect_source_t( const std::initializer_list<value_type>& v ){ reinterpret_cast<vector<value_type>&>(*this)=v; }

	// override and extend members
	void clear() noexcept { vector<named_string_t>::clear(); macro.clear(); }
	iterator find( string name ){ for( auto it=begin(); it!=end(); it++ ) if(stricmp(it->name.c_str(),name.c_str())==0) return it; return end(); }
	void append( string name, string source ){ if(!name.empty()){for(auto& s:*this) if(stricmp(s.name.c_str(),name.c_str())==0){ s.value=source; return; }} emplace_back( value_type{name,source} ); }
	bool replace( string _Where, string name, string source ){ auto it=find(_Where); if(it==end()) return false; it->name = name; it->value = source; return true; }
	bool replace( iterator _Where, string name, string source ){ if(_Where==end()) return false; _Where->name = name; _Where->value = source; return true; }
		
	string get_name( int index ) const { if(!macro.empty()){ if(index==0) return "macro.fx"; index--; } return this->at(index).name; }
	vector<string> names() const { vector<string> vs; if(!macro.empty()) vs.emplace_back("macro.fx"); for( auto& s:*this) vs.emplace_back(s.name); return vs; }
	vector<string> sources() const { vector<string> vs; if(!macro.empty()) vs.emplace_back(macro.merge()); for( auto& s:*this) vs.emplace_back(s.value); return vs; }
	string merge() const { string m=macro.merge(); for(auto& s:*this) m+=s.value+'\n'; return m; }
	vector<string> explode_parsed( const char* parsed ) const;

	// recursive append with template parameter pack
	template <typename T, class... Ts> void append_r( T arg, Ts... args ){ append("",arg); append_r(args...); }
	void append_r( string arg ){ if(!arg.empty()) append("",arg); }
	void append_r( const char* arg ){ if(arg&&*arg) append("",arg); }
};

//***********************************************
} // namespace gl
//***********************************************

// explode shader source by exploiting #line directives
inline vector<gl::indexed_string_t> gxExplodeShaderSource( const char* source, int first_index=1 )
{
	vector<gl::indexed_string_t> v;
	auto vs = explode_conservative(source,'\n');

	for( int k=0, kn=int(vs.size()), idx=first_index; k<kn; k++ )
	{
		const char* s = ltrim(vs[k].c_str());
		bool b_line = strncmp(s,"#line",5)==0;
		int r = b_line?sscanf( s+6, "%d", &idx ):0;
		gl::indexed_string_t i; i=vs[k]; i.index=idx;
		v.emplace_back(i);
		if(!b_line) idx++;
	}
	return v;
}

inline std::map<int,string> gxExplodeShaderSourceMap( const char* source, int first_index=1 )
{
	std::map<int,string> m;
	for( const auto& s : gxExplodeShaderSource(source,first_index) )
		if(!strstr(s.c_str(),"#line")) m.emplace(s.index,s);
	return m;
}

inline vector<string> gl::effect_source_t::explode_parsed( const char* parsed ) const
{
	if(!parsed||!*parsed) return vector<string>();
	auto ss=sources(); if(ss.empty()) return vector<string>();
	vector<int> n; int c0=0; for(auto& e:ss){ int c=1; for(const char* p=e.c_str();*p;p++){if(*p=='\n')c++;} n.push_back(c0+=c); }

	vector<string> v; v.resize(ss.size());
	int page_index=0; for( auto& l : gxExplodeShaderSource(parsed) )
	{
		if(l.index>=n[page_index]) page_index++;
		if(page_index>=int(v.size())){ printf("%s(): page_index(%d)>=v.size(%d)\n",__func__,page_index,int(v.size())); break; }
		v[page_index] += l + '\n';
	}

	return v;
}

//***********************************************
namespace gl {
//***********************************************
	
// effect: a list of programs
struct Effect : public Object
{
	Effect( GLuint ID, const char* name ) : Object(ID,name,0){ if(!(quad=gxCreateQuadVertexArray())) printf("[%s] unable to create quad buffer\n",name); }
	~Effect() override { active_program=nullptr; if(quad){ delete quad; quad=nullptr; } if(!pts.empty()){ for(auto& it:pts) safe_delete(it.second); pts.clear(); } for(auto& it:uniform_buffer_map){if(it.second){ delete it.second; it.second=nullptr; }} uniform_buffer_map.clear(); for(auto* p:programs) delete p; programs.clear(); }
	static void unbind(){ glUseProgram(0); }

	Program* bind( __printf_format_string__ const char* program_name, ... ){ char buff[1024]; va_list a;va_start(a,program_name);vsnprintf(buff,1024,program_name,a);va_end(a); active_program=get_program(buff); if(active_program) active_program->bind(); else{ active_program=nullptr; glUseProgram(0); } return active_program; }
	Program* bind( uint index ){ active_program=get_program_by_index(index); if(active_program) active_program->bind(); else { active_program=nullptr; glUseProgram(0); } return active_program; }

	bool empty() const { return programs.empty(); }
	size_t size() const { return programs.size(); }
	Program* get_program( const char* name ) const { for(auto* p:programs)if(stricmp(p->name(),name)==0) return p; printf("Unable to find program \"%s\" in effect \"%s\"\n", name, this->_name ); return nullptr; }
	Program* get_program_by_index( uint index ) const { if(index<programs.size()) return programs[index]; else { printf("[%s] Out-of-bound program index\n", _name ); return nullptr; } }
	Program* get_program_by_id( uint program_ID ) const { for(auto* p:programs)if(p->ID==program_ID) return p; printf("Unable to find program \"%u\" in effect \"%s\"\n", program_ID, this->_name ); return nullptr; }
	Program* create_program( const char* name, const program_source_t& source );
	Program* append_program( Program* program ){ if(!program) return nullptr; programs.emplace_back(program); auto& m=program->_uniform_block_map;for(auto& it:m){gl::Program::UniformBlock& ub=it.second;ub.buffer=get_or_create_uniform_buffer(ub.name,ub.size);} return program; }
	bool append( gl::effect_source_t source );
	template <class... Ts> bool append( Ts... args ){ gl::effect_source_t source; source.append_r(args...); return append(source); }

	Uniform* get_uniform( const char* name ){ if(active_program) return active_program->get_uniform(name); printf("%s.%s(%s): no program is bound.",this->name(),__func__,name); return nullptr; }

	void set_uniform( const char* name, Texture* t ){ auto* p=_get_or_find_program_for_uniform(name,__func__); if(p) p->set_uniform(name,t); }
	void set_uniform( const char* name, bool b ){ auto* p=_get_or_find_program_for_uniform(name,__func__); if(p) p->set_uniform(name,b); }
	template <class T> void set_uniform( const char* name, const vector<T>& v ){ GLsizei count=GLsizei(v.size()); set_uniform(name,v.data(),count); }
	template <class T> void set_uniform( const char* name, const T& v ){ set_uniform<T>(name,(T*)&v, 1); }
	template <class T> void set_uniform( const char* name, T* v, GLsizei count=1 ) // [NOTE] do not make const T*; this causes some template instances go wrong
	{
		auto* p=_get_or_find_program_for_uniform(name,__func__); if(!p) return;
		auto* u=p->get_uniform(name); if(!u) return; if(u->block_index==-1) p->set_uniform(name, v, count);
		if(!u->block_name[0]||u->block_offset==-1) return; auto* b=get_uniform_buffer(u->block_name); if(!b) return; if(v) b->set_sub_data(v,sizeof(T)*count,u->block_offset);
	}

	// image textures
	void bind_image_texture( const char* name, Texture* t, GLenum access=GL_READ_WRITE /* or GL_WRITE_ONLY or GL_READ_ONLY */, GLint level=0, GLenum format=0, bool bLayered=false, GLint layer=0 ){ if(!active_program) return void(printf("%s.%s(%s): no program is bound.\n",this->name(),__func__,name)); active_program->bind_image_texture(name,t,access,level,format,bLayered,layer); }

	// uniform buffer/block
	gl::Buffer* get_or_create_uniform_buffer( const char* name, size_t size ){ gl::Buffer* b=get_uniform_buffer(name); if(b&&b->size()!=size){ static std::set<string> warns; auto it=warns.find(name); if(it==warns.end()){ warns.insert(name); printf("[%s] %s(): uniform_buffer(%s).size(=%d)!=%d\n",this->_name,__func__,name,int(b->size()),int(size));} } if(b) return b; b=gxCreateBuffer(name,GL_UNIFORM_BUFFER,size,GL_STATIC_DRAW,nullptr,GL_MAP_WRITE_BIT|GL_DYNAMIC_STORAGE_BIT,false); if(!b){ printf("[%s] unable to create uniform buffer [%s]\n", this->_name, name); return nullptr; } return uniform_buffer_map[name]=b; }
	gl::Buffer* get_uniform_buffer( const char* name ){ auto it=uniform_buffer_map.find(name); return it==uniform_buffer_map.end()?nullptr:it->second; }
	GLint get_uniform_block_binding( const char* name ){ GLint binding=active_program?active_program->get_uniform_block_binding(name):-1; if(binding!=-1) return binding; for( auto* program : programs ){ GLint b=program->get_uniform_block_binding(name); if(b!=-1) return b; } return -1; }
	gl::Buffer* bind_uniform_buffer( const char* name, gl::Buffer* ub=nullptr /* if nullptr, use default buffer */ ){ gl::Buffer* b=ub?ub:get_uniform_buffer(name); GLuint binding=get_uniform_block_binding(name); if(b&&binding!=GLuint(-1)){ if(b->target==GL_UNIFORM_BUFFER) b->bind_base(binding); else b->bind_base_as(GL_UNIFORM_BUFFER, binding); return b; }
	else{ if(!b) printf( "[%s] %s(): unable to find uniform buffer %s\n", this->_name, __func__, name ); else printf( "[%s] %s(): unable to find uniform buffer binding %s\n", this->_name, __func__, name ); return nullptr; } }

	// blocks: uniform, shader storage, and atomic counter
	GLint get_shader_storage_block_binding( const char* name ){ GLint binding=active_program?active_program->get_shader_storage_block_binding(name):-1; if(binding!=-1) return binding; for( auto* program : programs ){ GLint b=program->get_shader_storage_block_binding(name); if(b!=-1) return b; } printf( "[%s] %s(): unable to find shader storage block binding %s\n", this->_name, __func__, name ); return -1; }
	gl::Buffer* bind_shader_storage_buffer( const char* name, Buffer* buffer ){ GLint binding=get_shader_storage_block_binding(name); if(binding<0) return nullptr; if(buffer->target==GL_SHADER_STORAGE_BUFFER) buffer->bind_base(binding); else buffer->bind_base_as(GL_SHADER_STORAGE_BUFFER,binding); return buffer; }
	GLint get_atomic_counter_buffer_binding( const char* name ){ GLint binding=active_program?active_program->get_atomic_counter_buffer_binding(name):-1; if(binding!=-1) return binding; for( auto* program : programs ){ GLint b=program->get_atomic_counter_buffer_binding(name); if(b!=-1) return b; } printf( "[%s] %s(): unable to find atomic counter buffer binding %s\n", this->_name, __func__, name ); return -1; }
	gl::Buffer* bind_atomic_counter_buffer( const char* name, Buffer* buffer ){ GLint binding=get_atomic_counter_buffer_binding(name); if(binding<0) return nullptr; if(buffer->target==GL_ATOMIC_COUNTER_BUFFER) buffer->bind_base(binding); else buffer->bind_base_as(GL_ATOMIC_COUNTER_BUFFER,binding); return buffer; }

	// draw or compute
	inline void draw_quads(){ if(!quad) return; if(quad->index_buffer) quad->draw_elements(0,4,GL_TRIANGLE_STRIP); else quad->draw_arrays(0,4,GL_TRIANGLE_STRIP); }
	inline void draw_points( GLsizei width, GLsizei height, bool no_attrib=false ){ if(no_attrib) return draw_points_no_attrib( width*height ); uint64_t key=uint64_t(width)|(uint64_t(height)<<32); auto it=pts.find(key); VertexArray* va=it!=pts.end()?it->second:(pts[key]=gxCreatePointVertexArray(width,height)); if(va) va->draw_arrays(0,width*height,GL_POINTS); }
	inline void draw_points_no_attrib( GLsizei count ){ GLuint v=0; if(context::is_core_profile()){ auto it=pts.find(0);VertexArray* va=it!=pts.end()?it->second:(pts[0]=gxCreatePointVertexArray(0,0)); if(va)v=va->ID; } glBindVertexArray(v); glDrawArrays( GL_POINTS, 0, count ); } // core profile should bind non-empty VAO; attribute-less rendering without binding any vertex array: simply using gl_VertexID in vertex shaders
	inline void dispatch_compute_indirect( GLintptr indirect ){ glDispatchComputeIndirect( indirect ); }
	inline void dispatch_compute_groups( GLuint gx, GLuint gy=1, GLuint gz=1 ){ glDispatchCompute( gx,gy,gz ); }
	inline void dispatch_compute_threads( GLuint tx, GLuint ty=1, GLuint tz=1 ){ auto* p=get_program_by_id(gxGetIntegerv(GL_CURRENT_PROGRAM)); if(!p){ printf("%s(): no program is bound\n",__func__); return; } ivec3 s=p->get_compute_work_group_size(); glDispatchCompute(max((tx+s.x-1)/s.x,1u),max((ty+s.y-1)/s.y,1u),max((tz+s.z-1)/s.z,1u)); }

	// shader source files
	void export_shader_sources( const char* dir, const char* fxname="" )
	{
		path_t d=path_t(dir).to_preferred(), dx=d.extension();
		if(!d.empty()&&dx.empty()&&d.back()!=preferred_separator) d=d.dir(); d += "glsl\\";
		string f=!fxname||!*fxname?_name:path_t(fxname).stem();
		for( auto* p : programs ) p->source.export_shader_sources( dir, (f+"."+p->name()).c_str() );
	}

	// internal members
	Program*						active_program=nullptr;
	vector<Program*>				programs;
	std::map<string,Buffer*>		uniform_buffer_map;	// do not define uniform buffers in each program, since they are shared across programs.
	std::map<uint64_t,VertexArray*>	pts;				// point vertex array
	VertexArray*					quad=nullptr;
	
protected:
	Program* _get_or_find_program_for_uniform( const char* name, const char* func=nullptr );
};

inline Program* Effect::_get_or_find_program_for_uniform( const char* name, const char* func )
{
	if(!func) func=__func__;
	if(active_program) return active_program;
	printf("%s.%s(%s): no program is bound; ",this->name(),func,name);
	for(auto* p:programs){ auto* u=p->get_uniform(name); if(!u||u->block_index==-1) continue; printf( "using \"%s\" for the buffer-backed uniform.\n",p->name() ); return p; }
	printf( "search ends up with no associated program.\n");
	return nullptr;
}

//***********************************************
} // end namespace gl
//***********************************************

#ifdef __GX_MESH_H__
inline void mesh::draw_arrays( GLint first, GLsizei count, GLenum mode, bool b_bind ){ buffer.vertex->draw_arrays(first,count,mode,b_bind); }
inline void mesh::draw_arrays_instanced( GLint first, GLsizei instance_count, GLsizei count, GLenum mode, bool b_bind ){ buffer.vertex->draw_arrays_instanced( first, instance_count, count, mode, b_bind ); }
inline void mesh::draw_elements( GLuint first, GLsizei count, GLenum mode, bool b_bind ){ buffer.vertex->draw_elements( first, count, mode, b_bind ); }
inline void mesh::draw_elements_instanced( GLuint first, GLsizei instance_count, GLsizei count, GLenum mode, bool b_bind ){ buffer.vertex->draw_elements_instanced( first, instance_count, count, mode, b_bind ); }
inline void mesh::draw_elements_indirect( GLvoid* indirect, GLenum mode, bool b_bind ){ buffer.vertex->draw_elements_indirect( indirect, mode, b_bind ); }
inline void mesh::draw_range_elements( GLuint first, GLuint end, GLsizei count, GLenum mode, bool b_bind ){ buffer.vertex->draw_range_elements( first, end, count, mode, b_bind ); }
inline void mesh::multi_draw_elements( GLuint* pfirst, const GLsizei* pcount, GLsizei draw_count, GLenum mode, bool b_bind ){ buffer.vertex->multi_draw_elements( pfirst, pcount, draw_count, mode, b_bind ); }
inline void mesh::multi_draw_elements_indirect( GLsizei draw_count, GLsizei stride, GLvoid* indirect, GLenum mode, bool b_bind ){ buffer.vertex->multi_draw_elements_indirect( draw_count, stride, indirect, mode, b_bind ); }
inline void mesh::multi_draw_elements_indirect_count( GLsizei max_draw_count, GLsizei stride, const void* indirect, GLintptr draw_count, GLenum mode, bool b_bind ){ buffer.vertex->multi_draw_elements_indirect_count( max_draw_count, stride, indirect, draw_count, mode, b_bind ); }
#endif

inline gl::Query* gxCreateQuery( const char* name, GLenum target=GL_TIME_ELAPSED )
{
	GLuint ID; if(glCreateQueries) glCreateQueries( target, 1, &ID ); else glGenQueries(1,&ID); if(ID==0){ printf( "%s(): unable to create query %s\n", __func__, name ); return nullptr; }
	return new gl::Query(ID,name,target);
}

inline gl::Buffer* gxCreateBuffer( const char* name, GLenum target, GLsizeiptr size, GLenum usage, const void* data=nullptr, GLbitfield storage_flags=GL_MAP_READ_BIT|GL_MAP_WRITE_BIT|GL_DYNAMIC_STORAGE_BIT, bool persistent=false )
{
	// glCreateBuffers() also initializes objects, while glGenBuffers() do not initialze until bind() is called
	GLuint ID; if(glCreateBuffers) glCreateBuffers(1,&ID); else glGenBuffers(1,&ID); if(ID==0){ printf( "%s(): unable to create buffer %s\n", __func__, name ); return nullptr; }
	
	gl::Buffer* buffer = new gl::Buffer(ID,name,target,size);
	if(persistent) storage_flags |= GL_MAP_PERSISTENT_BIT|GL_MAP_COHERENT_BIT;
	if(glNamedBufferStorage) glNamedBufferStorage(ID,size,data,storage_flags);
	else if(glBufferStorage){ GLuint b0=buffer->bind(); glBufferStorage(target,size,data,storage_flags); glBindBuffer(target,b0); }
	else if(glNamedBufferData) glNamedBufferData(ID,size,data,usage);
	else if(glBufferData){ GLuint b0=buffer->bind(); glBufferData(target,size,data,usage); glBindBuffer(target,b0); }

	return buffer;
}

template <class T> gl::Buffer* gxCreateBuffer(const char* name, GLenum target, const vector<T>& data, GLenum usage, GLbitfield storage_flags=GL_MAP_READ_BIT|GL_MAP_WRITE_BIT|GL_DYNAMIC_STORAGE_BIT, bool persistent=false){ return gxCreateBuffer(name,target,GLsizeiptr(data.size()*sizeof(T)),usage,(const void*)data.data(),storage_flags,persistent); }
template <class T, size_t N> gl::Buffer* gxCreateBuffer(const char* name, GLenum target, const std::array<T,N>& data, GLenum usage, GLbitfield storage_flags=GL_MAP_READ_BIT|GL_MAP_WRITE_BIT|GL_DYNAMIC_STORAGE_BIT, bool persistent=false){ return gxCreateBuffer(name,target,GLsizeiptr(N*sizeof(T)),usage,(const void*)data.data(),storage_flags,persistent); }

inline gl::VertexArray* gxCreateVertexArray( const char* name, const vertex* p_vertices, size_t vertex_count, const uint* p_indices=nullptr, size_t index_count=0, GLenum usage=GL_STATIC_DRAW )
{
	GLuint ID=gxCreateVertexArray(); if(ID==0) return nullptr; if(ID==0){ printf( "%s(): unable to create vertex array %s\n", __func__, name ); return nullptr; }
	gl::VertexArray* va = new gl::VertexArray( ID, name );
	if(!p_vertices||!vertex_count){ va->bind(); va->bind(false); return va; } // create empty VAO for no-attrib rendering

	gl::Buffer* vbo = gxCreateBuffer( format("%s.VTX",name), GL_ARRAY_BUFFER, sizeof(vertex)*vertex_count, usage, p_vertices, 0 ); if(!vbo){ printf( "%s(): unable to create vertex_buffer %s.VTX\n", __func__, name ); return nullptr; }
	gl::Buffer* ibo = p_indices&&index_count ? gxCreateBuffer( format("%s.IDX",name), GL_ELEMENT_ARRAY_BUFFER, sizeof(uint)*index_count, usage, p_indices ) : nullptr; if(p_indices&&index_count&&!ibo){ printf( "%s(): unable to create index buffer %s.IDX\n", __func__, name ); return nullptr; }
	
	va->vertex_buffer = vbo;
	va->index_buffer = ibo;
	va->vertex_count = vertex_count;
	va->index_count = va->index_buffer ? index_count : 0;

	// use fixed binding (without direct state access)
	va->bind();
	static const GLuint offset[] = { offsetof(vertex,pos), offsetof(vertex,norm), offsetof(vertex,tex) };
	static const GLint  size[] = { sizeof(vertex::pos)/sizeof(GLfloat), sizeof(vertex::norm)/sizeof(GLfloat), sizeof(vertex::tex)/sizeof(GLfloat) };
	for( GLuint k=0; k<3; k++ ){ glEnableVertexAttribArray(k); glVertexAttribBinding(k,0); glVertexAttribFormat(k,size[k],GL_FLOAT,GL_FALSE,offset[k]); }
	glBindVertexBuffer( 0, va->vertex_buffer->ID, 0, sizeof(vertex) );
	if(va->index_buffer) glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, va->index_buffer->ID );
	va->bind(false);

	return va;
}

inline gl::VertexArray* gxCreateVertexArray( const char* name, const vector<vertex>& vertices, GLenum usage=GL_STATIC_DRAW )
{
	return vertices.empty()?nullptr:gxCreateVertexArray(name,&vertices[0],vertices.size(),nullptr,0,usage);
}

inline gl::VertexArray* gxCreateVertexArray( const char* name, const vector<vertex>& vertices, const vector<uint>& indices, GLenum usage=GL_STATIC_DRAW )
{
	return vertices.empty()?nullptr:gxCreateVertexArray(name,vertices.data(),vertices.size(),indices.empty()?nullptr:indices.data(),indices.size(),usage);
}

#ifdef __GX_MESH_H__
inline gl::VertexArray* gxCreateVertexArray( const char* name, mesh* p_mesh, GLenum usage=GL_STATIC_DRAW )
{
	return p_mesh?gxCreateVertexArray(name,&p_mesh->vertices[0],p_mesh->vertices.size(),&p_mesh->indices[0],p_mesh->indices.size(),usage):nullptr;
}
#endif

inline gl::VertexArray* gxCreateQuadVertexArray()
{
	static vertex vertices[] = { {{-1,-1,0},{0,0,1},{0,0}}, {{1,-1,0},{0,0,1},{1,0}}, {{-1,1,0},{0,0,1},{0,1}}, {{1,1,0},{0,0,1},{1,1}} }; // strip ordering [0, 1, 3, 2]
	gl::VertexArray* va = gxCreateVertexArray( "QUAD", vertices, std::extent<decltype(vertices)>::value ); if(!va) printf( "%s(): Unable to create QUAD\n", __func__ );
	return va;
}

inline gl::VertexArray* gxCreatePointVertexArray( GLsizei width, GLsizei height )
{
	gl::VertexArray* va = nullptr;
	if(width==0||height==0) va = gxCreateVertexArray( "PTS", (vertex*)nullptr,0 );
	else
	{
		vector<vertex> pts(size_t(width)*size_t(height));
		for(int y=0,k=0;y<height;y++)
			for(int x=0;x<width;x++,k++)
				pts[k] = {{float(x),float(y),0},{0,0,1.0f},{x/float(width-1),y/float(height-1)}};
		va = gxCreateVertexArray( "PTS", &pts[0], pts.size() );
	}
	return va;
}

inline const char* gxGetProgramBinaryPath( const char* name )
{
	auto d=localtemp()+"glfx\\binary\\"s; if(!d.exists()) d.mkdir();
	return format("%s%s.bin",d.c_str(),name);
}

inline void gxSaveProgramBinary( const char* name, GLuint ID, uint crc )
{
	vector<char> program_binary(gxGetProgramiv( ID, GL_PROGRAM_BINARY_LENGTH ),0);
	GLenum binary_format=0; glGetProgramBinary( ID, GLsizei(program_binary.size()), nullptr, &binary_format, &program_binary[0] );

	string cache_path = gxGetProgramBinaryPath(name); if(cache_path.empty()){ printf("%s(): cache_path is empty\n",__func__); return; }
	FILE* fp = fopen(cache_path.c_str(), "wb"); if(!fp){ printf("unable to open %s\n",cache_path.c_str()); return; }
	fwrite( &crc, sizeof(crc), 1, fp );
	fwrite( &binary_format, sizeof(GLenum), 1, fp );
	fwrite( &program_binary[0], sizeof(char), program_binary.size(), fp );
	fclose(fp);
}

inline GLuint gxLoadProgramBinary( const char* name, uint crc )
{
	path_t program_binary_path = gxGetProgramBinaryPath(name); if(!program_binary_path.exists()) return 0;
	size_t crc_size=sizeof(crc), offset=crc_size+sizeof(GLenum);
	FILE* fp = fopen(program_binary_path.c_str(),"rb"); if(!fp) return 0;
	_fseeki64(fp,0,SEEK_END); size_t program_binary_size=_ftelli64(fp); _fseeki64(fp,0,SEEK_SET);
	if(program_binary_size<offset){ fclose(fp); return 0; } // no md5 hash and binary format exists
	vector<char> buff(program_binary_size); fread(&buff[0], sizeof(char), program_binary_size, fp); fclose(fp);
	if( memcmp(&buff[0],&crc,crc_size)!=0) return 0;
	GLenum binary_format=0; memcpy( &binary_format, (&buff[0])+crc_size, sizeof(GLenum) );
	GLuint ID = glCreateProgram(); glProgramBinary( ID, binary_format, (&buff[0])+offset, GLsizei(program_binary_size-offset) );
	return ID;
}

inline void gxInfoLog( const char* name, const char* msg, const vector<gl::named_string_t>* p_source=nullptr )
{
	char L[53]={}; for(int k=0;k<50;k++)L[k]='_'; L[50]=L[51]='\n'; printf("\n%s",L);

	if(!p_source){ for( auto& v : explode(msg,"\n") ) printf("%s: %s\n", name, v.c_str() ); }
	else
	{
		struct line_t { int page; int offset; string s; }; std::map<size_t,line_t> lines;
		vector<int> offsets; offsets.resize(p_source->size(),0);
		for( int k=0, kn=int(p_source->size()); k<kn; k++ )
		{
			if(k>0&&!lines.empty()) offsets[k] = int(lines.rbegin()->first);
			for(auto& it:gxExplodeShaderSourceMap(p_source->at(k).value.c_str(),offsets[k]+1))
			{
				int offset=offsets[k]; if(k>0&&it.first<=offset){ for(int j=k-1;j>=0&&it.first<=offset;j--) offset=offsets[j]; }
				lines.emplace(it.first,line_t{k,offset,it.second});
			}
		}

		auto extract = [&]( string& m, int idx, bool b_replace=false )->string
		{
			if(idx<0) return "";
			auto it=lines.find(size_t(idx)); if(it==lines.end()) return "";
			auto& l = it->second; if(idx<=l.offset) return "";
			string pname = p_source->at(l.page).name;
			string pl = pname.empty()?format("%d(%d)",l.page,idx-l.offset):format("%s(%d)",pname.c_str(),idx-l.offset);
			if(b_replace) m = str_replace(m.c_str(),format("0(%d)",idx),pl.c_str());
			l.s = trim(str_replace(l.s.c_str(),"%", "%%")); // % causes crash in gprintf
			return l.s.empty() ? "": format( "\n%s> %s", pl.c_str(), l.s.c_str() );
		};

		for( auto& m : explode(msg,"\n") )
		{
			const char *l=strchr(m.c_str(),'('), *r=strchr(m.c_str(),')');
			if(l&&r&&l<r)
			{
				char s[4096]={}; memcpy(s,l+1,r-l-1); s[r-l-1]=0; int idx=atoi(s);
				m += extract(m,idx-1) + extract(m,idx,true);
			}
			printf("%s: %s\n", name, m.c_str() );
		}
	}
	printf("%s",L);
}

inline GLuint gxCompileShader( GLenum shader_type, const char* name, const vector<gl::named_string_t>& source )
{
	vector<const char*> ps; for( auto& s : source ) ps.push_back(s.value.c_str());
	
	GLuint ID = glCreateShader( shader_type ); if(ID==0){ printf("%s(): unable to glCreateShader(%u)\n", __func__, shader_type); return 0; }
	glShaderSource( ID, GLsizei(ps.size()), ps.data(), nullptr );
	glCompileShader( ID );

	static const int MAX_LOG_LENGTH=8192; static char msg[MAX_LOG_LENGTH] = {};
	GLint L; glGetShaderInfoLog(ID,MAX_LOG_LENGTH,&L,msg);
	if(L>1&&L<=MAX_LOG_LENGTH) gxInfoLog(name,msg,&source);
	if(gxGetShaderiv(ID,GL_COMPILE_STATUS)!=GL_TRUE){ glDeleteShader(ID); return 0; }

	return ID;
}

inline bool gxCheckProgramLink( const char* name, GLuint ID, bool bLog=true )
{
	if(ID==0) return false;
	static const int MAX_LOG_LENGTH=4096; static char msg[MAX_LOG_LENGTH] = {}; GLint L; bool bLogExists;
	glGetProgramInfoLog(ID,MAX_LOG_LENGTH,&L,msg); bLogExists=L>1&&L<=MAX_LOG_LENGTH; if(bLogExists&&bLog) gxInfoLog(name,msg); if(gxGetProgramiv(ID,GL_LINK_STATUS)!=GL_TRUE){ glDeleteProgram(ID); return false; }
	return true;
}

inline bool gxValidateProgram( const char* name, GLuint ID, bool bLog=true ) // check if the program can run in the current state
{
	if(ID==0) return false;
	static const int MAX_LOG_LENGTH=4096; static char msg[MAX_LOG_LENGTH] = {}; GLint L; bool bLogExists;
	glValidateProgram(ID); glGetProgramInfoLog(ID,MAX_LOG_LENGTH,&L,msg); bLogExists=L>1&&L<=MAX_LOG_LENGTH; if(bLogExists&&bLog) gxInfoLog(name,msg); if(gxGetProgramiv(ID,GL_VALIDATE_STATUS)!=GL_TRUE){ glDeleteProgram(ID); return false; }
	return true;
}

namespace gl { struct directive_t
{
	string version, extension, pragma, layout;
	string merge() const { return version+extension+pragma+layout; }
	directive_t& operator+=( const directive_t& other )
	{
		if(!other.version.empty())		version+=other.version;
		if(!other.extension.empty())	extension+=other.extension;
		if(!other.pragma.empty())		pragma+=other.pragma;
		if(!other.layout.empty())		layout+=other.layout;
		return *this;
	}
};}

inline gl::directive_t gxPreprocessShaderDirectives( uint shader_type, string& src, bool keep_blank=true )
{
	static const char v[]="#version", e[]="#extension", p[]="#pragma", l[]="layout", h[]="shared";
	constexpr size_t le=sizeof(e)-1, lp=sizeof(p)-1, lv=sizeof(v)-1, ll=sizeof(l)-1, lh=sizeof(l)-1; // exclude trailing zeros

	gl::directive_t d; string src0=src; src.clear();
	for( auto& j : gxExplodeShaderSource(src0.c_str()) )
	{
		string* pd=nullptr;
		const char* s=str_replace(trim(j.c_str()),"\t"," "); if(s[0]=='#')
		{
			if(strncmp(s,v,lv)==0){ pd=&d.version; if(gl::context::is_core_profile()&&!strstr(s,"core")) j+=" core"; } // add "core" to core profile version
			else if(strncmp(s,e,le)==0){ pd=&d.extension; }
			else if(strncmp(s,p,lp)==0){ pd=&d.pragma; }
			else if(strncmp(s,l,ll)==0){ pd=&d.layout; }
		}

		// remove erroneous per-shader directives
		if(shader_type!=GL_COMPUTE_SHADER)
		{
			if(strncmp(s,h,lh)==0) j.clear(); // remove shared in non-compute shaders, while keeping index
		}

		if(pd){	*pd+=j+'\n'; if(keep_blank) src+='\n'; }
		else{ src+=j+'\n'; }
	}
	return d;
}

inline gl::Program* gxCreateProgram( string prefix, string name, const gl::program_source_t& source )
{
	if(!prefix.empty()) prefix+='.';
	string pname_s = prefix+name; const char* pname = pname_s.c_str();

	// 1. extract directives, and add layout qualifier
	for( auto& it : source )
	{
		if(it.second.empty()) continue;

		gl::directive_t directives;
		directives.layout = it.first==GL_FRAGMENT_SHADER?"layout(pixel_center_integer) in vec4 gl_FragCoord;\n":"";
		for( auto& f: it.second )
		{
			auto& s = const_cast<string&>(f.value); // source element
			directives += gxPreprocessShaderDirectives(it.first,s);
		}
		auto& first = const_cast<string&>(it.second.front().value); // first source element
		first = directives.merge()+first; // merge all together
	}

	// 2. trivial return for the default program
	auto vit=source.find(GL_VERTEX_SHADER);		bool vertex_shader_exists = vit!=source.end()&&!vit->second.empty();
	auto cit=source.find(GL_COMPUTE_SHADER);	bool compute_shader_exists = cit!=source.end()&&!cit->second.empty();
	if(!vertex_shader_exists&&!compute_shader_exists) return new gl::Program(0,name.c_str());

	// 3. create md5 hash of shader souces
	string crcsrc=pname; for( auto& it : source ){ for( auto& s : it.second ) crcsrc += s.value; }
	uint crc = crc32(0,crcsrc.c_str(),crcsrc.size()*sizeof(decltype(crcsrc)::value_type));

	// 4. try to load binary cache
	GLuint binary_program_ID = gxLoadProgramBinary( pname, crc );
	if(binary_program_ID&&gxCheckProgramLink(pname, binary_program_ID,false))
	{
		gl::Program* program = new gl::Program(binary_program_ID, name.c_str());
		program->source = source;
		program->update_uniform_cache();
		return program;
	}

	// 5. create program first
	GLuint program_ID = glCreateProgram();
	gl::Program* program = new gl::Program( program_ID, name.c_str() );
	program->source = source;

	// 6. compile and attach shaders
	std::chrono::high_resolution_clock::time_point tbegin=std::chrono::high_resolution_clock::now();
	printf( "compiling %s ... ", pname );

	vector<GLuint> attached_shaders;
	for( auto& it : program->source )
	{
		if(it.second.empty()) continue;
		GLuint shader_ID = gxCompileShader( it.first, pname, it.second ); if(shader_ID==0) return nullptr;
		glAttachShader( program->ID, shader_ID );
		attached_shaders.emplace_back( shader_ID );
	}

	// 7. first linking of program
	glLinkProgram( program->ID );
	if(!gxCheckProgramLink( pname, program->ID ))
	{
		if(program){ delete program; program=nullptr; }
		return nullptr;
	}

	// 8. query the active attributes
	char aname[256] = {};
	GLint active_attribute_count = gxGetProgramiv( program->ID, GL_ACTIVE_ATTRIBUTES );
	if(active_attribute_count!=3)
	{
		for( int k=0; k < active_attribute_count; k++ )
		{
			GLint asize;	// attribute type: GL_FLOAT_VEC3 or GL_FLOAT_VEC2
			GLenum atype;	// attribute size in array

			glGetActiveAttrib( program->ID, k, 256, nullptr, &asize, &atype, aname );
			GLint loc = glGetAttribLocation(program->ID,aname);

			// bind attributes explicitely again
			if(loc!=0&&(stristr(aname,"pos")||stristr(aname,"position")))					glBindAttribLocation( program->ID, 0, aname );
			if(loc!=1&&(stristr(aname,"norm")||stristr(aname,"nrm")))						glBindAttribLocation( program->ID, 1, aname );
			if(loc!=2&&(stristr(aname,"tex")||stristr(aname,"coord")||stristr(aname,"tc")))	glBindAttribLocation( program->ID, 2, aname );
		}

		// link agin to reflect the changes in binding locations
		glLinkProgram( program->ID ); if(!gxCheckProgramLink( pname, program->ID )){ if(program){ delete program; program=nullptr; } return nullptr; }
	}

	// 9. update uniforms before validating the program
	program->update_uniform_cache(); // to avoid validation errors in Intel compilers

	// 10. validation: test if the program can run in the current state
	if(!gxValidateProgram( pname, program->ID )){ if(program){ delete program; program=nullptr; } return nullptr; }

	// 11. save cache
	glProgramParameteri( program->ID, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, GL_TRUE );
	gxSaveProgramBinary( pname, program->ID, crc );

	// 12. detach/delete shaders
	GLsizei shader_count; GLuint shaders[256];
	glGetAttachedShaders( program->ID,256,&shader_count,shaders);
	for(GLsizei k=0;k<shader_count;k++){ glDetachShader(program->ID,shaders[k]); glDeleteShader(shaders[k]); }

	// 13. logging
	std::chrono::high_resolution_clock::time_point tend=std::chrono::high_resolution_clock::now();
	double tdelta = std::chrono::duration_cast<std::chrono::duration<double,std::milli>>(tend-tbegin).count();

	printf( "completed in %.1f ms\n", float(tdelta) );

	return program;
}

inline gl::Program* gl::Effect::create_program( const char* name, const program_source_t& source )
{
	gl::Program* p=gxCreateProgram(this->name(),name,source); return p?append_program(p):nullptr;
}

inline gl::Effect* gxCreateEffect( const char* name )
{
	return new gl::Effect(0, name);
}

//***********************************************
#ifndef GLFXAPI // when <glfx.h> is not included
//***********************************************
namespace glfx {
//***********************************************
struct IParser
{
	virtual bool parse( const char* src ) = 0;
	virtual const char* parse_log() = 0;
	virtual int program_count() = 0;
	virtual const char* program_name( int program_id ) = 0;
	virtual int shader_count( int program_id ) = 0;
	virtual unsigned shader_type( int program_id, int shader_id ) = 0;
	virtual const char* shader_source( int program_id, int shader_id ) = 0;
};
//***********************************************
} // end namespace glfx
//***********************************************

inline glfx::IParser* glfxCreateParser()
{
#ifdef __msvc__
	static glfx::IParser*(*f)()=(glfx::IParser*(*)())GetProcAddress(GetModuleHandleW(nullptr),"__glfxCreateParser");
	if(!f){ printf( "unable to link to %s()\n", __func__ ); return nullptr; } return f();
#else
	printf( "unable to link to %s(). include <glfx.h>\n", __func__ ); return nullptr;
#endif
}
inline void glfxDeleteParser( glfx::IParser** pp_parser )
{
#ifdef __msvc__
	static void(*f)(glfx::IParser**)=(void(*)(glfx::IParser**))GetProcAddress(GetModuleHandleW(nullptr),"__glfxDeleteParser");
	if(!f){ printf( "unable to link to %s()\n", __func__ ); return; } f(pp_parser);
#else
	printf( "unable to link to %s(). include <glfx.h>\n", __func__ );
#endif
}

#endif // GLFXAPI

//***********************************************
inline gl::Effect* __gxCreateEffectImpl( gl::Effect* parent, const char* fxname, gl::effect_source_t source )
{
	glfx::IParser* parser = glfxCreateParser(); if(!parser){ printf( "%s(): unable to create parser\n", __func__ ); return nullptr; }
	if(!parser->parse(source.merge().c_str())){ printf( "%s(): failed to parse %s\n%s\n", __func__, fxname, parser->parse_log() ); return nullptr; }

	gl::Effect* e = parent ? parent : new gl::Effect(0, fxname);
	for( int k=0, kn=parser->program_count(); k<kn; k++ )
	{
		gl::program_source_t ss;
		for( int j=0, jn=parser->shader_count(k); j<jn; j++ )
		{
			vector<gl::named_string_t> ns;
			auto ev = source.explode_parsed(parser->shader_source(k,j));
			for( int i=0, ni=int(ev.size()); i<ni; i++ ) ns.emplace_back( gl::named_string_t{source.get_name(i), ev[i]} );
			ss[parser->shader_type(k,j)] = ns;
		}
		
		gl::Program* program = gxCreateProgram(fxname?fxname:"",parser->program_name(k),ss); if(!program){ glfxDeleteParser(&parser); if(e!=parent) delete e; return nullptr; }
		e->append_program(program);
	}

	glfxDeleteParser(&parser);
	return e;
}

inline gl::Effect* gxCreateEffect( const char* fxname, gl::effect_source_t source )
{
	return __gxCreateEffectImpl(nullptr,fxname,source);
}

template <class... Ts> inline gl::Effect* gxCreateEffect( const char* fxname, Ts... args )
{
	gl::effect_source_t source; source.append_r(args...);
	return gxCreateEffect(fxname,source);
}

inline bool gl::Effect::append( gl::effect_source_t source )
{
	return __gxCreateEffectImpl(this,this->_name,source);
}

inline gl::Framebuffer* gxCreateFramebuffer( const char* name=nullptr )
{
	if(!name){ printf( "%s(): name==nullptr\n", __func__ ); return nullptr; }
	GLuint ID=0; if(glCreateFramebuffers) glCreateFramebuffers( 1, &ID ); else glGenFramebuffers(1,&ID); if(ID==0){ printf( "%s(): unable to create buffer[%s]", __func__, name ); return nullptr; }
	return new gl::Framebuffer(ID,name&&name[0]?name:"");	// if name is nullptr, return default FBO
}

inline gl::Framebuffer*& gl::Framebuffer::instance()
{
	struct auto_fbo_t { gl::Framebuffer* ptr=gxCreateFramebuffer("FBO"); ~auto_fbo_t(){ if(ptr) delete ptr; } };
	static auto_fbo_t f; if(!f.ptr) printf("%s(): fbo==nullptr",__func__); return f.ptr;
}

inline gl::TransformFeedback* gxCreateTransformFeedback( const char* name )
{
	GLuint ID=0; if(glCreateTransformFeedbacks) glCreateTransformFeedbacks( 1, &ID ); else if(glGenTransformFeedbacks) glGenTransformFeedbacks(1,&ID); if(ID==0||!glIsTransformFeedback(ID)){ printf( "%s(): unable to create transform feedback[%s]", __func__, name ); return nullptr; }
	return new gl::TransformFeedback( ID, name );
}

__noinline gl::Texture* gxCreateTexture1D( const char* name, GLint levels, GLsizei width, GLsizei layers=1, GLint internal_format=GL_RGBA16F, GLvoid* data=nullptr, bool force_array=false )
{
	if(layers>gxGetIntegerv( GL_MAX_ARRAY_TEXTURE_LAYERS )){ printf( "%s(): layer (=%d) > GL_MAX_ARRAY_TEXTURE_LAYERS (=%d)\n", __func__, layers, gxGetIntegerv( GL_MAX_ARRAY_TEXTURE_LAYERS ) ); return nullptr; }
	if(!gxIsSizedInternalFormat(internal_format)){ printf( "%s(): internal_format must use a sized format instead of GL_RED, GL_RG, GL_RGB, GL_RGBA.\n", __func__ ); return nullptr; }

	GLenum target = layers>1||force_array?GL_TEXTURE_1D_ARRAY:GL_TEXTURE_1D;
	GLuint ID = gxCreateTexture(target); if(ID==0) return nullptr;
	GLenum format = gxGetTextureFormat( internal_format );
	GLenum type = gxGetTextureType( internal_format );

	gl::Texture* texture = new gl::Texture(ID,name,target,internal_format,format,type); if(width==0) return texture;
	texture->_key = gl::Texture::crc(0,levels,0,layers,target,false);
	glBindTexture( target, ID );

	// allocate storage
	if(target==GL_TEXTURE_1D ){	glTexStorage1D(target,levels,internal_format,width );			if(data) glTexSubImage1D( target, 0, 0, width, format, type, data ); }
	else{						glTexStorage2D(target,levels,internal_format,width,layers );	if(data) glTexSubImage2D( target, 0, 0, 0, width, layers, format, type, data ); }

	// set dimensions
	texture->_width		= width;
	texture->_height	= target==GL_TEXTURE_1D_ARRAY?layers:1;
	texture->_depth		= 1;
	texture->_levels	= levels;

	// generate mipmap
	glTexParameteri( target, GL_TEXTURE_BASE_LEVEL, 0 );
	glTexParameteri( target, GL_TEXTURE_MAX_LEVEL, levels-1 );
	if( levels>1 ) glGenerateMipmap( target );

	// attributes
	glTexParameteri( target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	if(layers>1) glTexParameteri( target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri( target, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( target, GL_TEXTURE_MIN_FILTER, levels>1?GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST );

	return texture;
}

__noinline gl::Texture* gxCreateTexture2D( const char* name, GLint levels, GLsizei width, GLsizei height, GLsizei layers=1, GLint internal_format=GL_RGBA16F, GLvoid* data=nullptr, bool force_array=false, bool multisample=false, GLsizei multisamples=4 )
{
	if(layers>gxGetIntegerv( GL_MAX_ARRAY_TEXTURE_LAYERS )){ printf( "%s(): layer (=%d) > GL_MAX_ARRAY_TEXTURE_LAYERS (=%d)\n", __func__, layers, gxGetIntegerv( GL_MAX_ARRAY_TEXTURE_LAYERS ) ); return nullptr; }
	if(!gxIsSizedInternalFormat(internal_format)){ printf( "%s(): internal_format must use a sized format instead of GL_RED, GL_RG, GL_RGB, GL_RGBA.\n", __func__ ); return nullptr; }

	GLenum target = layers>1||force_array?(multisample?GL_TEXTURE_2D_MULTISAMPLE_ARRAY:GL_TEXTURE_2D_ARRAY):(multisample?GL_TEXTURE_2D_MULTISAMPLE:GL_TEXTURE_2D);
	GLuint ID = gxCreateTexture(target); if(ID==0) return nullptr;
	GLenum format = gxGetTextureFormat( internal_format );
	GLenum type = gxGetTextureType( internal_format );

	gl::Texture* texture = new gl::Texture(ID,name,target,internal_format,format,type); if(width==0||height==0) return texture;
	texture->_key = gl::Texture::crc(0,levels,0,layers,target,false);
	glBindTexture( target, ID );

	// multipsamples
	GLint max_samples = multisample?gxGetInternalFormativ( target, internal_format, GL_SAMPLES ):1;
	texture->_multisamples = multisample?min(multisamples,max_samples):1;
	if(multisample&&multisamples>max_samples) printf("%s(): input multisamples (=%d) is clamped to max_samples (=%d)\n", __func__, multisamples, max_samples );

	// allocate storage
	GLenum e0 = glGetError(); // previous error from others
	if(target==GL_TEXTURE_2D){							glTexStorage2D( target, levels, internal_format, width, height );			if(data) glTexSubImage2D( target, 0, 0, 0, width, height, format, type, data ); }
	else if(target==GL_TEXTURE_2D_ARRAY){				glTexStorage3D( target, levels, internal_format, width, height, layers );	if(data) glTexSubImage3D( target, 0, 0, 0, 0, width, height, layers, format, type, data ); }
	else if(target==GL_TEXTURE_2D_MULTISAMPLE){			glTexStorage2DMultisample(target, multisamples, internal_format, width, height, GL_TRUE ); }
	else if(target==GL_TEXTURE_2D_MULTISAMPLE_ARRAY){	glTexStorage3DMultisample(target, multisamples, internal_format, width, height, layers, GL_TRUE ); }
	GLenum e1 = glGetError(); // texture error 
	if(e1!=GL_NO_ERROR&&e1!=e0){ printf( "%s(%s): error %s\n", __func__, name, 	gxGetErrorString(e1) ); delete texture; return nullptr; }

	// test if the format is immutable
	GLint b_immutable; glGetTexParameteriv( target, GL_TEXTURE_IMMUTABLE_FORMAT, &b_immutable );
	if(!b_immutable) printf( "%s(): %s is not immutable\n", __func__, name );

	// set dimensions
	texture->_width		= width;
	texture->_height	= height;
	texture->_depth		= target==GL_TEXTURE_2D_ARRAY||target==GL_TEXTURE_2D_MULTISAMPLE_ARRAY?layers:1;
	texture->_levels	= target==GL_TEXTURE_2D||target==GL_TEXTURE_2D_ARRAY?levels:1;

	// generate mipmap
	glTexParameteri( target, GL_TEXTURE_BASE_LEVEL, 0 );
	glTexParameteri( target, GL_TEXTURE_MAX_LEVEL, levels-1 );
	if( levels>1 ) glGenerateMipmap( target );

	// attributes
	glTexParameteri( target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	if(layers>1) glTexParameteri( target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
	glTexParameteri( target, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( target, GL_TEXTURE_MIN_FILTER, levels>1?GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST );

	return texture;
}

__noinline gl::Texture* gxCreateTexture3D( const char* name, GLint levels, GLsizei width, GLsizei height, GLsizei depth, GLint internal_format=GL_RGBA16F, GLvoid* data=nullptr )
{
	if(!gxIsSizedInternalFormat(internal_format)){ printf( "%s(): internal_format must use a sized format instead of GL_RED, GL_RG, GL_RGB, GL_RGBA.\n", __func__ ); return nullptr; }

	GLenum target = GL_TEXTURE_3D;
	GLuint ID = gxCreateTexture(target); if(ID==0) return nullptr;
	GLenum format = gxGetTextureFormat( internal_format );
	GLenum type = gxGetTextureType( internal_format );

	gl::Texture* texture = new gl::Texture(ID,name,target,internal_format,format,type); if(width==0||height==0||depth==0) return texture;
	texture->_key = gl::Texture::crc(0,levels,0,depth,target,false);
	glBindTexture( target, ID );

	// allocate storage
	glTexStorage3D( target, levels, internal_format, width, height, depth );
	if(data) glTexSubImage3D( target, 0, 0, 0, 0, width, height, depth, format, type, data );

	// set dimensions
	texture->_width		= width;
	texture->_height	= height;
	texture->_depth		= depth;
	texture->_levels	= levels;

	// generate mipmap
	glTexParameteri( target, GL_TEXTURE_BASE_LEVEL, 0 );
	glTexParameteri( target, GL_TEXTURE_MAX_LEVEL, levels-1 );
	if( levels>1 ) glGenerateMipmap( target );

	// attributes
	glTexParameteri( target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri( target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
	glTexParameteri( target, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( target, GL_TEXTURE_MIN_FILTER, levels>1?GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST );

	return texture;
}

__noinline gl::Texture* gxCreateTextureCube( const char* name, GLint levels, GLsizei width, GLsizei height, GLsizei count=1, GLint internal_format=GL_RGBA16F, GLvoid* data[6]=nullptr, bool force_array=false )
{
	if(count*6>gxGetIntegerv( GL_MAX_ARRAY_TEXTURE_LAYERS )){ printf( "%s(): count*6 (=%d) > GL_MAX_ARRAY_TEXTURE_LAYERS (=%d)\n", __func__, count*6, gxGetIntegerv( GL_MAX_ARRAY_TEXTURE_LAYERS ) ); return nullptr; }
	if(!gxIsSizedInternalFormat(internal_format)){ printf( "%s(): internal_format must use a sized format instead of GL_RED, GL_RG, GL_RGB, GL_RGBA.\n", __func__ ); return nullptr; }
	if(width==0||height==0){ printf( "%s(%s): width==0 or height==0", __func__, name ); return nullptr; }

	GLenum target = count>1||force_array?GL_TEXTURE_CUBE_MAP_ARRAY:GL_TEXTURE_CUBE_MAP;
	GLuint ID = gxCreateTexture(target); if(ID==0) return nullptr;
	GLenum format = gxGetTextureFormat( internal_format );
	GLenum type = gxGetTextureType( internal_format );

	gl::Texture* texture = new gl::Texture(ID,name,target,internal_format,format,type);
	texture->_key = gl::Texture::crc(0,levels,0,count,target,false);

	glBindTexture( target, ID );
	if( target==GL_TEXTURE_CUBE_MAP )
	{
		// mutable version: // int l=0; for(int w=width,h=height;l<levels&&(w>1||h>1);l++,w=max(1,w>>1),h=max(1,h>>1)) for(int k=0;k<6;k++) glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X+k, l, internal_format, w, h, 0, format, type, l==0&&data&&data[k]?data[k]:nullptr ); if(l<levels) levels=l;
		glTexStorage2D( target, levels, internal_format, width, height ); // immutable
		if(data) for(int k=0;k<6;k++) if(data[k]) glTexSubImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X+k, 0, 0, 0, width, height, format, type, data[k] );
	}
	else if( target==GL_TEXTURE_CUBE_MAP_ARRAY )
	{
		// mutable version: // int l=0; for(int w=width,h=height;l<levels&&(w>1||h>1);l++,w=max(1,w>>1),h=max(1,h>>1)) glTexImage3D( target, l, internal_format, w, h, layers*6, 0, format, type, nullptr ); if(l<levels) levels=l;
		glTexStorage3D( target, levels, internal_format, width, height, count*6 );
	}

	// set dimensions
	texture->_width		= width;
	texture->_height	= height;
	texture->_depth		= count*6;
	texture->_levels	= levels;

	// generate mipmap
	glTexParameteri( target, GL_TEXTURE_BASE_LEVEL, 0 );
	glTexParameteri( target, GL_TEXTURE_MAX_LEVEL, levels-1 );
	if( levels>1 ) glGenerateMipmap( target );

	// attributes
	glTexParameteri( target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );	// GL_CLAMP_TO_EDGE is the best option
	glTexParameteri( target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri( target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
	glTexParameteri( target, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( target, GL_TEXTURE_MIN_FILTER, levels>1?GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST );

	// enable seamless cubemap
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	return texture;
}

__noinline gl::Texture* gxCreateTextureBuffer( const char* name, gl::Buffer* buffer, GLint internal_format=GL_RGBA16F )
{
	if(!gxIsSizedInternalFormat(internal_format)){ printf( "%s(): internal_format must use a sized format instead of GL_RED, GL_RG, GL_RGB, GL_RGBA.\n", __func__ ); return nullptr; }

	GLenum target = GL_TEXTURE_BUFFER;
	GLuint ID = gxCreateTexture(target); if(ID==0) return nullptr;
	GLenum format = gxGetTextureFormat( internal_format );
	GLenum type = gxGetTextureType( internal_format );

	// create a texture
	gl::Texture* texture = new gl::Texture(ID,name,target,internal_format,format,type);
	texture->_key = gl::Texture::crc(0,1,0,1,target,false);
	glBindTexture( target, ID );

	// create a buffer object and bind the storage using buffer object
	if(glTextureBuffer) glTextureBuffer( ID, internal_format, buffer->ID ); else glTexBuffer( target, internal_format, buffer->ID );

	// set dimensions
	texture->_width		= texture->get_texture_level_parameteriv( GL_TEXTURE_WIDTH, 0 );
	texture->_height	= 1;
	texture->_depth		= 1;
	texture->_levels	= 1;

	// unbind the texture
	glBindTexture( target, 0 );

	return texture;
}

__noinline gl::Texture* gxCreateTextureRectangle( const char* name, GLsizei width, GLsizei height, GLint internal_format=GL_RGBA16F, GLvoid* data=nullptr )
{
	if(!gxIsSizedInternalFormat(internal_format)){ printf( "%s(): internal_format must use a sized format instead of GL_RED, GL_RG, GL_RGB, GL_RGBA.\n", __func__ ); return nullptr; }

	GLenum target = GL_TEXTURE_RECTANGLE;
	GLuint ID = gxCreateTexture(target); if(ID==0) return nullptr;
	GLenum format = gxGetTextureFormat( internal_format );
	GLenum type = gxGetTextureType( internal_format );

	gl::Texture* texture = new gl::Texture(ID,name,target,internal_format,format,type); if(width==0||height==0) return texture;
	texture->_key = gl::Texture::crc(0,1,0,1,target,false);
	glBindTexture( target, ID );

	// allocate storage
	glTexStorage2D( target, 1, internal_format, width, height );
	if(data) glTexSubImage2D( target, 0, 0, 0, width, height, format, type, data );

	// set dimensions
	texture->_width		= width;
	texture->_height	= height;
	texture->_depth		= 1;
	texture->_levels	= 1;

	// attributes
	glTexParameteri( target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri( target, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( target, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

	return texture;
}

__noinline gl::Texture* gxCreateTextureView( gl::Texture* src, GLuint min_level, GLuint levels, GLuint min_layer=0, GLuint layers=1, GLenum target=0, bool force_array=false )
{
	if(!src) return nullptr;
	uint key = gl::Texture::crc(min_level,levels,min_layer,layers,target,force_array);
	for(gl::Texture* t=src; t; t=t->_next ) if(t->_key==key) return t;

	if(src->target==GL_TEXTURE_BUFFER){ printf( "%s(): texture buffer (%s) cannot have a view\n", __func__, src->_name ); return nullptr; }
	if(levels==0){ printf( "%s(): %s->view should have more than one levels\n", __func__, src->_name ); return nullptr; }
	if(layers==0){ printf( "%s(): %s->view should have more than one layers\n", __func__, src->_name ); return nullptr; }
	if((min_level+levels)>GLuint(src->mip_levels())){ printf( "%s(): %s->view should have less than %d levels\n", __func__, src->_name, src->mip_levels() ); return nullptr; }
	if((min_layer+layers)>GLuint(src->layers())){ printf( "%s(): %s->view should have less than %d layers\n", __func__, src->_name, src->layers() ); return nullptr; }
	if(!src->is_immutable()){ printf("%s(): !%s->is_immutable()\n", __func__, src->_name ); return nullptr; }

	// correct the new target
	if(target==0)
	{
		target=src->target;
		GLenum t=src->target;
		if(force_array)
		{
			if(t==GL_TEXTURE_1D||t==GL_TEXTURE_1D_ARRAY) target=GL_TEXTURE_1D_ARRAY;
			else if(t==GL_TEXTURE_2D||t==GL_TEXTURE_2D_ARRAY) target=GL_TEXTURE_2D_ARRAY;
			else if(t==GL_TEXTURE_2D_MULTISAMPLE||t==GL_TEXTURE_2D_MULTISAMPLE_ARRAY) target=GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
			else if(t==GL_TEXTURE_CUBE_MAP) target=GL_TEXTURE_CUBE_MAP_ARRAY;
		}
		else if(src->target==GL_TEXTURE_1D_ARRAY&&layers==1) target=GL_TEXTURE_1D;
		else if(src->target==GL_TEXTURE_2D_ARRAY&&layers==1) target=GL_TEXTURE_2D;
		else if(src->target==GL_TEXTURE_2D_MULTISAMPLE_ARRAY&&layers==1) target=GL_TEXTURE_2D_MULTISAMPLE;
		else if(src->target==GL_TEXTURE_CUBE_MAP_ARRAY&&layers==1) target=GL_TEXTURE_CUBE_MAP;
	}

	// create a new texture; here, we must use glGenTextures() instead of glCreateTextures(), because texture view requires to have a created but uninitialized texture.
	GLuint ID1; glGenTextures(1,&ID1); if(ID1==0) return nullptr;
	bool b_slice0 = min_level==0&&levels==1&&layers==1;
	const char* name1 = b_slice0?format("%s[%u]",src->_name,min_layer):format("%s[%u:%u][%u:%u]",src->_name,min_layer,layers-1,min_level,levels-1);

	// get attributes
	GLint internal_format	= src->internal_format();
	GLint wrap				= src->get_texture_parameteriv( GL_TEXTURE_WRAP_S );
	GLint min_filter		= src->get_texture_parameteriv( GL_TEXTURE_MIN_FILTER );
	GLint mag_filter		= src->get_texture_parameteriv( GL_TEXTURE_MAG_FILTER );

	// allocate a new texture using the heap of the parent
#ifdef __msvc__
	gl::Texture* t1=new(HeapAlloc((void*)src->_crtheap,0,sizeof(gl::Texture))) gl::Texture(ID1,name1,target,internal_format,src->format(),src->type(),src->_crtheap);
#else
	gl::Texture* t1=new gl::Texture(ID1,name1,target,internal_format,src->format(),src->type(),src->_crtheap);
#endif
	t1->_key = key;

	// correct min_filter, mag_filter
	if(levels==1)
	{
		if(min_filter==GL_LINEAR_MIPMAP_LINEAR||min_filter==GL_LINEAR_MIPMAP_NEAREST)	min_filter = GL_LINEAR;
		if(min_filter==GL_NEAREST_MIPMAP_LINEAR||min_filter==GL_NEAREST_MIPMAP_NEAREST)	min_filter = GL_NEAREST;
	}

	// create view and set attributes
	glTextureView( t1->ID, target, src->ID, internal_format, min_level, levels, min_layer, layers);
	
	// set dimensions
	t1->_multisamples = src->_multisamples;
	t1->_width	= src->width(min_level);
	t1->_height	= src->height(min_level);
	t1->_depth	= layers;
	t1->_levels	= levels;

	// filter should be set after dimensions
	t1->set_filter( min_filter, mag_filter );
	t1->set_wrap( wrap );

	// add to linked list for search
	for( gl::Texture* t=src; t; t=t->_next ) if(t->_next==nullptr){ t->_next=t1; break; }
	return t1;
}

#endif // __GX_OPENGL_H__
