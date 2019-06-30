//*******************************************************************
// Copyright 2011-2020 Sungkil Lee
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
#ifndef __GX_OPENGL_H__
#define __GX_OPENGL_H__

#include "gxmath.h"
#include "gxstring.h"
#include "gxfilesystem.h"
#include <malloc.h>

#if !defined(__gxcorearb_h_) && defined(__has_include)
	#if __has_include( "gxcorearb.h" )
		#include "gxcorearb.h"
	#elif __has_include( <gxut/gxcorearb.h> )
		#include <gxut/gxcorearb.h>
	#endif
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
		{GL_PARAMETER_BUFFER_ARB,GL_PARAMETER_BUFFER_BINDING_ARB},
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
inline size_t	gxGetTextureChannelBPP( GLint internal_format ){ size_t s=0; switch(gxGetTextureType(internal_format)){ case GL_HALF_FLOAT: case GL_SHORT: case GL_UNSIGNED_SHORT: s=sizeof(short); break; case GL_BYTE: case GL_UNSIGNED_BYTE: s=sizeof(char); break; case GL_FLOAT: case GL_INT: case GL_UNSIGNED_INT: s=sizeof(int); break; }; return s*8; }
inline size_t	gxGetTextureBPP( GLint internal_format ){ return gxGetTextureChannelBPP(internal_format)*gxGetTextureChannels(internal_format); }
inline GLenum	gxGetImageTextureInternalFormat( int depth, int channels ){ if(depth==8) return channels==1?GL_R8:channels==2?GL_RG8:channels==3?GL_RGB8:channels==4?GL_RGBA8:0; else if(depth==16)	return channels==1?GL_R16F:channels==2?GL_RG16F:channels==3?GL_RGB16F:channels==4?GL_RGBA16F:0; else if(depth==32)	return channels==1?GL_R32F:channels==2?GL_RG32F:channels==3?GL_RGB32F:channels==4?GL_RGBA32F:0; return 0; }
inline bool		gxIsSamplerType( GLenum uniformType ){ GLenum t=uniformType; if(t>=GL_SAMPLER_1D && t<=GL_SAMPLER_2D_SHADOW) return true; if(t>=GL_SAMPLER_1D_ARRAY && t<=GL_SAMPLER_CUBE_SHADOW) return true; if(t>=GL_INT_SAMPLER_1D && t<=GL_UNSIGNED_INT_SAMPLER_2D_ARRAY) return true; if(t>=GL_SAMPLER_2D_RECT && t<=GL_SAMPLER_2D_RECT_SHADOW ) return true; if(t>=GL_SAMPLER_BUFFER && t<=GL_UNSIGNED_INT_SAMPLER_BUFFER ) return true; if(t>=GL_SAMPLER_CUBE_MAP_ARRAY && t<=GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY ) return true; if(t>=GL_SAMPLER_2D_MULTISAMPLE && t<=GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY ) return true; /* TODO: if(t>=GL_SAMPLER_RENDERBUFFER_NV && t<=GL_UNSIGNED_INT_SAMPLER_RENDERBUFFER_NV ) return true;*/ return false; }
inline path		gxGetProgramBinaryPath( const char* name ){ path p=path::temp()+L"glProgramBinary\\"+atow(name)+L".bin"; if(!p.dir().exists()) p.dir().mkdir(); return p; }
inline int		gxGetMipLevels( int width, int height=1, int depth=1 ){ int l=0,s=uint(max(max(width,height),depth)); while(s){s=s>>1;l++;} return l; }
inline uint		gxGetMipLevels( uint width, uint height=1, uint depth=1 ){ uint l=0,s=uint(max(max(width,height),depth)); while(s){s=s>>1;l++;} return l; }
inline GLuint	gxCreateQuery( GLenum target ){ GLuint idx; if(glCreateQueries) glCreateQueries(target,1,&idx); else glGenQueries(1,&idx); return idx; }
inline GLuint	gxCreateTexture( GLenum target ){ GLuint idx; if(glCreateTextures) glCreateTextures( target, 1, &idx ); else{ GLuint b0=gxGetBinding(target); glGenTextures(1,&idx); glBindTexture(target,idx); glBindTexture(target,b0); } return idx; }
inline GLuint	gxCreateRenderBuffer(){ GLuint idx; if(glCreateRenderbuffers) glCreateRenderbuffers(1,&idx); else { GLuint b0=gxGetBinding(GL_RENDERBUFFER); glGenRenderbuffers(1,&idx); glBindRenderbuffer(GL_RENDERBUFFER,idx); glBindRenderbuffer(GL_RENDERBUFFER,b0); } return idx; }
inline GLuint	gxCreateVertexArray(){ GLuint idx; if(glCreateVertexArrays) glCreateVertexArrays(1, &idx); else glGenVertexArrays(1,&idx); return idx; }
inline const char* gxGetErrorString( GLenum e ){ if(e==GL_NO_ERROR) return ""; if(e==GL_INVALID_ENUM) return "GL_INVALID_ENUM"; if(e==GL_INVALID_VALUE) return "GL_INVALID_VALUE"; if(e==GL_INVALID_OPERATION) return "GL_INVALID_OPERATION"; if(e==GL_INVALID_FRAMEBUFFER_OPERATION) return "GL_INVALID_FRAMEBUFFER_OPERATION"; if(e==GL_OUT_OF_MEMORY) return "GL_OUT_OF_MEMORY"; if(e==GL_STACK_UNDERFLOW) return "GL_STACK_UNDERFLOW"; if(e==GL_STACK_OVERFLOW) return "GL_STACK_OVERFLOW"; return "UNKNOWN"; }

//***********************************************
// forward declarations
namespace gl
{
	struct Texture; struct Buffer; struct Program; struct VertexArray;
	unsigned int crc32c( const void* ptr, size_t size, unsigned int crc0=0 );
}

gl::Texture*		gxCreateTexture1D(const char*,GLint,GLsizei,GLsizei,GLint,GLvoid*,bool);
gl::Texture*		gxCreateTexture2D(const char*,GLint,GLsizei,GLsizei,GLsizei,GLint,GLvoid*,bool,bool,GLsizei);
gl::Texture*		gxCreateTexture3D(const char*,GLint,GLsizei,GLsizei,GLsizei,GLint,GLvoid*);
gl::Texture*		gxCreateTextureCube(const char*,GLint,GLsizei,GLsizei,GLsizei,GLint,GLvoid*[6],bool);
gl::Texture*		gxCreateTextureBuffer(const char*,gl::Buffer*,GLint);
gl::Texture*		gxCreateTextureRectangle(const char*,GLsizei,GLsizei,GLint,GLvoid*);
gl::Texture*		gxCreateTextureView(gl::Texture*,GLuint,GLuint,GLuint,GLuint,bool,GLenum);
gl::Buffer*			gxCreateBuffer(const char*,GLenum,GLsizeiptr,GLenum,const void*,GLbitfield);
gl::Program*		gxCreateProgram(const char*,const char*,const std::map<GLuint,std::string>&,const char*,std::vector<const char*>*);
gl::Program*		gxCreateProgram( const char*,const char*,const char*,const char*,const char*);
gl::VertexArray*	gxCreateQuadVertexArray();
gl::VertexArray*	gxCreatePointVertexArray( GLsizei width, GLsizei height );

//***********************************************
namespace gl {
//***********************************************

	//***********************************************
	struct GLObject
	{
		const GLuint	ID;
		const char		name[64];
		const GLenum	target;
		const GLenum	target_binding;

		GLObject( GLuint id, const char* _name, GLenum _target ):ID(id),name(""),target(_target),target_binding(gxGetTargetBinding(_target)){ size_t l=strlen(_name),c=sizeof(name); strncpy((char*)name,_name,l<c?l:c-1);((char*)name)[l]=0; }
		virtual ~GLObject(){}
		virtual const char* get_name() const { return name; }
		__forceinline GLuint binding( GLenum target1=0 ){ return target1==0?gxGetIntegerv(target_binding):gxGetIntegerv(gxGetTargetBinding(target1)); }
	};

	//***********************************************
	struct Query : public GLObject
	{
		GLuint64 result;
		Query( GLuint ID, const char* name, GLenum target ):GLObject(ID,name,target){}
		virtual ~Query() override { GLuint id=ID; glDeleteQueries(1,&id); }
		bool	is_available(){ GLint available; glGetQueryObjectiv(ID,GL_QUERY_RESULT_AVAILABLE, &available); return available!=0; }
		void	finish(){ glGetQueryObjectui64v(ID,GL_QUERY_RESULT,&result); } // use GL_QUERY_RESULT_NO_WAIT for async query download
		void	begin(){ glBeginQuery(target,ID); }
		void	end(){ glEndQuery(target); }
	};

	struct TimeElapsed : public Query
	{
		TimeElapsed():Query(gxCreateQuery(GL_TIME_ELAPSED),"GL_TIME_ELAPSED",GL_TIME_ELAPSED){}
		inline double	delta(){ return double(result)/1000000.0; }
	};

	//***********************************************
#ifdef __GX_TIMER__
	struct timer_t : public GLObject
	{
		const GLuint	ID1=0;
		union { double2 result; struct { double x, y; }; };	// Async OpenGL Timestamp: (begin,end)
		gx::timer_t&	qpc;	// synchronous QueryPerformance CPU counter: reference to external or internal counter
		gx::timer_t		qpc0;	// synchronous QueryPerformance CPU counter: internal counter instance
		bool			complete=false;

		static double&	offset(){ static double f=0; return f; }
		static void		sync_with_qpc(){ static int64_t e=epoch(); glFinish(); double i=0,j=0;uint k;for(k=0;k<16;k++){j+=(gxGetInteger64v(GL_TIMESTAMP)-e)/1000000.0;i+=gx::timer_t::now();} offset()=(i-j)/double(k); glFinish(); }
		static int64_t	epoch(){ static int64_t e=0; if(e) return e; auto* ef=(int64_t(*)()) GetProcAddress(GetModuleHandleW(nullptr),"rex_timer_epoch_GL"); if(ef) return e=ef(); else if(glGetInteger64v){ printf( "gl::Timer::epoch(): unable to get rex_timer_epoch_GL()\n" ); glGetInteger64v(GL_TIMESTAMP,&e); return e; } printf( "gl::epoch(): unable to get the epoch of OpenGL\n" ); return e=-1; }

		// constructor: initialized with the internal qpc or an external qpc
		timer_t():timer_t(qpc0){}
		timer_t(gx::timer_t& t):GLObject(gxCreateQuery(GL_TIMESTAMP),"",GL_TIMESTAMP),ID1(gxCreateQuery(target)),qpc(t){ result=qpc.result=dvec2(0.0); }
		virtual ~timer_t() override { GLuint idx[2]={ID,ID1}; glDeleteQueries(2,idx); }

		// gl::Timer specific implementations
		inline bool		is_available(){ GLint available; glGetQueryObjectiv(ID1,GL_QUERY_RESULT_AVAILABLE, &available); return complete=(available!=GL_FALSE); }
		inline void		finish(){ if(complete) return; static int64_t e=epoch(); static const GLenum q=GL_QUERY_RESULT; GLuint64 v; glGetQueryObjectui64v(ID,q,&v); x = double(v-e)/1000000.0+offset(); glGetQueryObjectui64v(ID1,q,&v); y=double(v-e)/1000000.0+offset(); complete=true; }
		inline double	latency(){ if(!complete) finish(); return x-qpc.x; }	// difference between CPU time and the time for flushing preceding commands = result.x - qpc.x

		// consistent interface with gx::timer
		inline void		begin(){ qpc.begin(); glQueryCounter(ID,target); complete=false; }
		inline void		end(){ glQueryCounter(ID1,target); qpc.end(); complete=false; }
		inline double	delta(){ if(!complete) finish(); return y-x; }
		inline double	now(){ return (gxGetInteger64v(GL_TIMESTAMP)-epoch())/1000000.0+offset(); }
		inline void		clear(){ qpc.begin();qpc.end();result=qpc.result; complete=true; }
	};
#endif // __GX_TIMER__

	//***********************************************
	struct Buffer : public GLObject
	{
		Buffer( GLuint ID, const char* name, GLenum target ):GLObject(ID,name,target){} // bind() and bind_back() should be called to actually create this buffer
		virtual ~Buffer() override { GLuint id=ID; if(id) glDeleteBuffers( 1, &id ); }
		GLuint bind( bool b_bind=true ){ GLuint b0=binding(); if(!b_bind||b0!=ID) glBindBuffer( target, b_bind?ID:0 ); if(target==GL_TRANSFORM_FEEDBACK) glBindTransformFeedback(target, b_bind?ID:0 ); return b0; }
		GLuint bind_as( GLenum target1, bool b_bind=true ){ GLuint b0=binding(target1); if(!b_bind||b0!=ID) glBindBuffer( target1, b_bind?ID:0 ); return b0; }
		GLuint bind_base( GLuint index, bool b_bind=true ){ GLuint b0=binding(); if(base_bindable(target)&&(!b_bind||b0!=ID)) glBindBufferBase( target, index, b_bind?ID:0 ); return b0; }
		GLuint bind_base_as( GLenum target1, GLuint index, bool b_bind=true ){ GLuint b0=binding(target1); if(base_bindable(target1)&&(!b_bind||b0!=ID)) glBindBufferBase( target1, index, b_bind?ID:0 ); return b0; }
		GLuint bind_range( GLuint index, GLintptr offset, GLsizeiptr size, bool b_bind=true ){ GLuint b0=binding(); if(base_bindable(target)&&(!b_bind||b0!=ID)) glBindBufferRange( target, index, b_bind?ID:0, offset, size ); return b0; }
		__forceinline bool base_bindable( GLenum target1 ){ return target1==GL_SHADER_STORAGE_BUFFER||target1==GL_UNIFORM_BUFFER||target1==GL_TRANSFORM_FEEDBACK_BUFFER||target1==GL_ATOMIC_COUNTER_BUFFER; }

		void set_sub_data( GLsizeiptr size, GLvoid* data, GLintptr offset=0 ){ if(glNamedBufferSubData) glNamedBufferSubData(ID,offset,size,data); else { GLuint b0=bind(); glBufferSubData(target,offset,size,data); glBindBuffer(target,b0); } }
		void get_data( GLvoid* data, GLsizeiptr size=0 ){ get_sub_data( data, size, 0 ); }
		void get_sub_data( GLvoid* data, GLsizeiptr size, GLintptr offset=0 ){ if(glGetNamedBufferSubData) glGetNamedBufferSubData(ID,offset,size?size:this->size(),data); else if(glGetBufferSubData){ GLuint b0=bind(); glGetBufferSubData(target,offset,size?size:this->size(),data); glBindBuffer(target,b0); } }
		void copy_data( Buffer* write_buffer, GLsizei size=0 ){ copy_sub_data( write_buffer, size?size:this->size() ); }
		void copy_sub_data( Buffer* write_buffer, GLsizei size, GLintptr read_offset=0, GLintptr write_offset=0 ){ if(glCopyNamedBufferSubData) glCopyNamedBufferSubData(ID,write_buffer->ID,read_offset,write_offset,size); else printf("Buffer::copySubData(): glCopyNamedBufferSubData==nullptr (only supports named mode)\n" ); }
		void clear_data( GLenum internalformat, GLenum format, GLenum type, const void* data, GLintptr offset=0, GLsizeiptr size=0 ){ if(!glClearBufferData||!glClearBufferSubData) return; if(!size&&!offset){ GLuint b0=bind(true); glClearBufferData(target,internalformat,format,type,data); bind(b0); return; } GLuint b0=bind(true); glClearBufferSubData(target,internalformat,offset,size?size:parameteriv(GL_BUFFER_SIZE),format,type,data); bind(b0); }
		void* map( GLenum access=GL_READ_ONLY, GLenum target=0 ){ if(target==0) return glMapNamedBuffer(ID,access); bind(); return glMapBuffer(target,access); }
		void* map_range( GLintptr offset, GLsizeiptr length, GLenum access=GL_MAP_READ_BIT, GLenum target=0 ){ if(target==0) return glMapNamedBufferRange(ID,offset,length,access); bind(); return glMapBufferRange(target,offset,length,access); }
		void unmap( GLenum target=0 ){ if(target==0&&glUnmapNamedBuffer) glUnmapNamedBuffer(ID); else glUnmapBuffer(target); }

		// queries
		GLint parameteriv( GLenum value ){ GLint i; if(glGetNamedBufferParameteriv) glGetNamedBufferParameteriv(ID,value,&i); else { GLuint b0=bind(); glGetBufferParameteriv(target,value,&i); glBindBuffer(target,b0); } return i; }
		GLint access(){ return parameteriv(GL_BUFFER_ACCESS); }
		bool is_immutable(){ return parameteriv(GL_BUFFER_IMMUTABLE_STORAGE)!=GL_FALSE; }
		bool is_mapped(){ return parameteriv(GL_BUFFER_MAPPED)!=GL_FALSE; }
		GLint size(){ return parameteriv(GL_BUFFER_SIZE); }
		GLint usage(){ return parameteriv(GL_BUFFER_USAGE); }
	};

	//***********************************************
	struct TransformFeedback : public Buffer
	{
		TransformFeedback( GLuint ID, const char* name, GLenum target=GL_TRANSFORM_FEEDBACK ):Buffer(ID,name,target){};

		void begin( GLenum primitive_mode ){ bind(); glEnable( GL_RASTERIZER_DISCARD ); glBeginTransformFeedback( primitive_mode ); }
		void end( GLuint b0=0 ){ glEndTransformFeedback(); glDisable( GL_RASTERIZER_DISCARD ); bind(false); }
		void draw( GLenum mode ){ GLuint b0=bind(); glDrawTransformFeedback( mode, ID ); if(b0!=ID) glBindTransformFeedback( target, b0 ); }
	};

	//***********************************************
	// DrawIndirectCommands
	struct DrawArraysIndirectCommand { uint count, instance_count=1, first_vertex=0, base_instance=0; };
	struct DrawElementsIndirectCommand { uint count, instance_count=1, first_index=0, base_vertex=0, base_instance=0; uint pad0, pad1, pad2; }; // see https://www.opengl.org/wiki/GLAPI/glDrawElementsInstancedBaseVertexBaseInstance

	//***********************************************
	struct Texture : public GLObject
	{
		Texture( GLuint ID, const char* name, GLenum target, GLenum InternalFormat, GLenum Format, GLenum Type, uint64_t _crtheap=_get_heap_handle() ):GLObject(ID,name,target),_internal_format(InternalFormat),_type(Type),_format(Format),_channels(gxGetTextureChannels(InternalFormat)),_bpp(gxGetTextureBPP(InternalFormat)),_multisamples(1),key(0),next(nullptr),crtheap(_crtheap){}
		virtual ~Texture() override { if(next){ next->~Texture(); HeapFree((void*)next->crtheap,0,next); } glDeleteTextures(1,(GLuint*)&ID); b_texture_deleted()=true; } // use HeapFree() for views

		GLuint bind( bool b_bind=true ){ GLuint b0=gxGetIntegerv(target_binding); if(!b_bind||ID!=b0) glBindTexture( target, b_bind?ID:0 ); return b0; }
		void bind_image_texture( GLuint unit, GLenum access=GL_READ_ONLY /* or GL_WRITE_ONLY or GL_READ_WRITE */ , GLint level=0, GLenum format=0, bool bLayered=false, GLint layer=0 ){ glBindImageTexture( unit, ID, level, bLayered?GL_TRUE:GL_FALSE, layer, access, format?format:internal_format() ); }

		// query and bind back
		inline GLint get_texture_parameteriv( GLenum pname ) const { GLint iv; if(glGetTextureParameteriv) glGetTextureParameteriv(ID,pname,&iv); else { GLuint b0=gxGetIntegerv(target_binding); glBindTexture(target,ID); glGetTexParameteriv(target,pname,&iv ); glBindTexture(target,b0); } return iv; }
		inline GLint get_texture_level_parameteriv( GLenum pname, GLint level ) const { GLint iv; if(glGetTextureLevelParameteriv) glGetTextureLevelParameteriv(ID,level,pname,&iv); else { GLuint b0=gxGetIntegerv(target_binding); glBindTexture(target,ID); glGetTexLevelParameteriv(target,level,pname,&iv); glBindTexture(target,b0); } return iv; }
		inline void texture_parameteri( GLenum pname, GLint param ) const { if(glTextureParameteri) glTextureParameteri(ID,pname,param); else { GLuint b0=gxGetIntegerv(target_binding); glBindTexture(target,ID); glTexParameteri(target,pname,param); glBindTexture(target,b0); } }
		inline void texture_parameterf( GLenum pname, GLfloat param ) const { if(glTextureParameterf) glTextureParameterf(ID,pname,param); else { GLuint b0=gxGetIntegerv(target_binding); glBindTexture(target,ID); glTexParameterf(target,pname,param); glBindTexture(target,b0); } }
		inline void texture_parameterfv( GLenum pname, const GLfloat* params ) const { if(glTextureParameterfv) glTextureParameterfv(ID, pname, params); else { GLuint b0=gxGetIntegerv(target_binding); glBindTexture(target,ID); glTexParameterfv(target,pname,params); glBindTexture(target,b0); } }

		// texture dimension queries: pre-recorded when creating this
		GLint mip_levels() const {				return _levels; } // on-demand query: is_immutable()?get_texture_parameteriv(GL_TEXTURE_VIEW_NUM_LEVELS):get_texture_parameteriv(GL_TEXTURE_MAX_LEVEL)-get_texture_parameteriv(GL_TEXTURE_BASE_LEVEL)+1; }
		GLint width( GLint level=0 ) const {	return max(1,_width>>level); } // on-demand query: get_texture_level_parameteriv( GL_TEXTURE_WIDTH, level )
		GLint height( GLint level=0 ) const {	return (target==GL_TEXTURE_1D||target==GL_TEXTURE_1D_ARRAY||target==GL_TEXTURE_BUFFER)?1:max(1,_height>>level); } // on-demand query: get_texture_level_parameteriv( GL_TEXTURE_HEIGHT, level );
		GLint depth( GLint level=0 ) const {	return (target==GL_TEXTURE_1D||target==GL_TEXTURE_1D_ARRAY||target==GL_TEXTURE_BUFFER||target==GL_TEXTURE_2D||target==GL_TEXTURE_2D_MULTISAMPLE||target==GL_TEXTURE_RECTANGLE)?1:(target==GL_TEXTURE_2D_ARRAY||target==GL_TEXTURE_2D_MULTISAMPLE_ARRAY||target==GL_TEXTURE_CUBE_MAP||target==GL_TEXTURE_CUBE_MAP_ARRAY)?_depth:max(1,_depth>>level); } // on-demand query: get_texture_level_parameteriv( GL_TEXTURE_DEPTH, level );
		GLint layers( GLint level=0 ) const {	return (target==GL_TEXTURE_1D_ARRAY)?height(level):(target==GL_TEXTURE_2D_ARRAY||target==GL_TEXTURE_2D_MULTISAMPLE_ARRAY||target==GL_TEXTURE_3D)?depth(level):target==GL_TEXTURE_CUBE_MAP||target==GL_TEXTURE_CUBE_MAP_ARRAY?depth(level):1; }

		// other texture queries
		ivec2 mip_range() const {	ivec2 range=ivec2(get_texture_parameteriv(GL_TEXTURE_BASE_LEVEL),get_texture_parameteriv(GL_TEXTURE_MAX_LEVEL)); return ivec2(range.x,range.y-range.x+1); }
		ivec2 filter() const {		return ivec2(get_texture_parameteriv(GL_TEXTURE_MIN_FILTER),get_texture_parameteriv(GL_TEXTURE_MAG_FILTER)); }
		ivec3 wrap() const {		ivec3 w(get_texture_parameteriv(GL_TEXTURE_WRAP_S),get_texture_parameteriv(GL_TEXTURE_WRAP_T),0); if(target==GL_TEXTURE_3D||target==GL_TEXTURE_CUBE_MAP||target==GL_TEXTURE_CUBE_MAP_ARRAY) w.z=get_texture_parameteriv(GL_TEXTURE_WRAP_R); return w; }
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
		inline size_t bpp() const { return _bpp; }
		inline GLsizei multisamples() const { return target==GL_TEXTURE_2D_MULTISAMPLE||target==GL_TEXTURE_2D_MULTISAMPLE_ARRAY?_multisamples:1; }
		size_t width_step( GLint level=0 ) const { return (((width(level)*bpp()+3)>>2)<<2); } // 4byte-aligned size in a single row
		size_t size( GLint level=0 ) const { return width_step(level)*height(level); }

		// set attributes
		void set_filter( GLint min_filter, GLint mag_filter=0 ){ static const GLenum n=GL_NEAREST,l=GL_LINEAR,nn=GL_NEAREST_MIPMAP_NEAREST,nl=GL_NEAREST_MIPMAP_LINEAR,ln=GL_LINEAR_MIPMAP_NEAREST,ll=GL_LINEAR_MIPMAP_LINEAR; GLint i=min_filter,g=mag_filter?mag_filter:min_filter; texture_parameteri( GL_TEXTURE_MIN_FILTER, _levels==1?(i==nn||i==nl?n:i==ln||i==ll?l:i):i==n?nn:i==l?ll:i ); texture_parameteri( GL_TEXTURE_MAG_FILTER, g==nn||g==nl?n:g==ln||g==ll?l:g ); }
		void set_wrap( GLint wrap ){ texture_parameteri( GL_TEXTURE_WRAP_S, wrap ); if(target==GL_TEXTURE_2D||target==GL_TEXTURE_2D_ARRAY||target==GL_TEXTURE_2D_MULTISAMPLE||target==GL_TEXTURE_2D_MULTISAMPLE_ARRAY||target==GL_TEXTURE_RECTANGLE) texture_parameteri( GL_TEXTURE_WRAP_T, wrap ); if(target==GL_TEXTURE_3D||target==GL_TEXTURE_CUBE_MAP||target==GL_TEXTURE_CUBE_MAP_ARRAY) texture_parameteri( GL_TEXTURE_WRAP_R, wrap ); }
		void set_mipmap_range( GLint base_level, GLint level_count ){ texture_parameteri(GL_TEXTURE_BASE_LEVEL,base_level); texture_parameteri(GL_TEXTURE_MAX_LEVEL,base_level+level_count-1); }
		void set_min_LOD( GLfloat min_LOD ){ texture_parameterf(GL_TEXTURE_MIN_LOD,min_LOD); }
		void set_max_LOD( GLfloat max_LOD ){ texture_parameterf(GL_TEXTURE_MAX_LOD,max_LOD); }
		void set_LOD( GLfloat min_LOD, GLfloat max_LOD ){ texture_parameterf(GL_TEXTURE_MIN_LOD,min_LOD); texture_parameterf(GL_TEXTURE_MAX_LOD,max_LOD); }
		void set_border_color( const vec4& color ){ texture_parameterfv(GL_TEXTURE_BORDER_COLOR, color); }

		// clear colors
		void clear( const vec4& color, GLint level=0 ){ GLenum t=type(),f=format(); if(t==GL_HALF_FLOAT){ half4 h; ftoh(color,h,channels()); glClearTexImage(ID,level,f,t,h);} else if(t==GL_FLOAT) glClearTexImage(ID,level,format(),t,&color); else printf( "%s->clear(): texture is not one of float/half types\n", name ); }
		void cleari( const ivec4& color, GLint level=0 ){ GLenum t=type(),f=format(); if(t==GL_INT) glClearTexImage(ID,level,f,t,color); else if(t==GL_SHORT){ short4 i={short(color.x),short(color.y),short(color.z),short(color.w)}; glClearTexImage(ID,level,f,t,&i); } else if(t==GL_BYTE){ char4 i={char(color.x),char(color.y),char(color.z),char(color.w)}; glClearTexImage(ID,level,f,t,&i); } else printf( "%s->cleari(): texture is not one of byte/short/int types\n", name ); }
		void clearui( const uvec4& color, GLint level=0 ){ GLenum t=type(),f=format(); if(t==GL_UNSIGNED_INT) glClearTexImage(ID,level,f,t,color); else if(t==GL_UNSIGNED_SHORT){ ushort4 i={ushort(color.x),ushort(color.y),ushort(color.z),ushort(color.w)}; glClearTexImage(ID,level,f,t,&i); } else if(t==GL_UNSIGNED_BYTE){ uchar4 i={uchar(color.x),uchar(color.y),uchar(color.z),uchar(color.w)}; glClearTexImage(ID,level,f,t,&i); } else printf( "%s->clearui(): texture is not one of unsigned byte/short/int types\n", name ); }

		// mipmap
		void generate_mipmap(){ if(get_texture_parameteriv(GL_TEXTURE_IMMUTABLE_FORMAT)&&get_texture_parameteriv(GL_TEXTURE_MAX_LEVEL)<1){ printf( "Texture::generate_mipmap(%s): no mipmap generated for 1 level.\n", name ); return; } int w=get_texture_level_parameteriv(GL_TEXTURE_WIDTH,0), h=get_texture_level_parameteriv(GL_TEXTURE_HEIGHT,0), d=get_texture_level_parameteriv(GL_TEXTURE_DEPTH,0); texture_parameteri(GL_TEXTURE_BASE_LEVEL,0); texture_parameteri(GL_TEXTURE_MAX_LEVEL,0+gxGetMipLevels(w,h,d)-1); if(glGenerateTextureMipmap) glGenerateTextureMipmap(ID); else { GLuint b0=gxGetIntegerv(target_binding); glBindTexture(target,ID); glGenerateMipmap(target); glBindTexture(target,b0); }if(get_texture_parameteriv(GL_TEXTURE_MIN_FILTER)==GL_LINEAR) texture_parameteri( GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR ); else if(get_texture_parameteriv(GL_TEXTURE_MIN_FILTER)==GL_NEAREST) texture_parameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST ); }

		// get_image/set_image
		void read_pixels( GLvoid* pixels, GLint level=0 ){ if(target==GL_TEXTURE_BUFFER){ printf("[%s] read_pixels() not supports GL_TEXTURE_BUFFER\n", name ); return; } else if(glGetTextureImage){ glGetTextureImage( ID, level, format(), type(), GLsizei(size()), pixels ); return; } GLuint b0=bind(); glGetTexImage( target, level, format(), type(), pixels ); glBindTexture( target, b0 ); }
		void get_image( GLvoid* pixels, GLint level=0 ){ read_pixels(pixels,level); }
		void sub_image2D( GLvoid* pixels, GLint level=0, GLsizei width=0, GLsizei height=0, GLint xoffset=0, GLint yoffset=0 ){ if(glTextureSubImage2D) glTextureSubImage2D(ID,0,xoffset,yoffset,width?width:this->width(),height?height:this->height(),format(),type(),pixels); else { GLuint b0=bind(); glTexSubImage2D(target,0,xoffset,yoffset,width?width:this->width(),height?height:this->height(),format(),type(),pixels); glBindTexture(target,b0); } }
		void set_image( GLvoid* pixels, GLint level=0, GLsizei width=0, GLsizei height=0, GLint xoffset=0, GLint yoffset=0 ){ sub_image2D( pixels, level, width, height, xoffset, yoffset ); }

		// instance-related
		inline Texture* clone( const char* name );
		inline bool copy( Texture* dst, GLint level=0 );

		// static query to check whether there are dirty/deleted textures
		static bool& b_texture_deleted(){ static bool bDeleted=true; return bDeleted; }

		// view-related function: wrapper to createTextureView
		inline static uint crc( GLuint min_level, GLuint levels, GLuint min_layer, GLuint layers, bool force_array, GLenum target ){ struct info { GLuint min_level, levels, min_layer, layers; bool force_array; GLenum target; }; info i={min_level,levels,min_layer,layers,force_array,target}; return gl::crc32c(&i,sizeof(i)); }
		inline Texture* view( GLuint min_level, GLuint levels, GLuint min_layer=0, GLuint layers=1, GLint internal_format=0, GLenum target=0 ){ return gxCreateTextureView(this,min_level,levels,min_layer,layers,false,target); } // view support (> OpenGL 4.3)
		inline Texture* slice( GLuint layer, GLuint level=0 ){ return view(level,1,layer,1); }
		inline Texture* last_mip( GLuint layer=0 ){ return view(_levels-1,1,layer,1); }
		inline Texture* array_view(){ return (layers()>1)?this:gxCreateTextureView(this,0,mip_levels(),0,layers(),true,0); }

		// dimensions
		GLint		_width;
		GLint		_height=1;
		GLint		_depth=1;
		GLint		_levels=1;

		// internal format, type, format
		GLenum		_internal_format;
		GLenum		_type;
		GLenum		_format;
		GLint		_channels;
		size_t		_bpp;
		GLsizei		_multisamples;

		// view-related protected members
		uint		key;		// key of the current view
		Texture*	next;		// next view node: a node of linked list, starting from the parent node
		uint64_t	crtheap;	// heap handle to the parent, require to allocate view across DLL boundaries
	};

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
			(target==GL_TEXTURE_CUBE_MAP||GL_TEXTURE_CUBE_MAP_ARRAY) ? gxCreateTextureCube( name, m, w, h, l, f, nullptr, false ):
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
		if(w0!=w1||h0!=h1||d0!=d1){ printf("%s::copy(): Dimension (%dx%dx%d) is different from %s (%dx%dx%d)\n", name, w0, h0, d0, dst->name, w1, h1, d1 ); return false; }
		if(size()!=dst->size()){ printf("%s::copy(): %s.size(=%d) != %s.size(=%d)\n", name, name, int(size()), dst->name, int(dst->size()) ); return false; }
		glCopyImageSubData( ID, target, level, 0, 0, 0, dst->ID, dst->target, level, 0, 0, 0, w0, h0, d0 ); return true;
	}

	//***********************************************
	// this vertex array generates multiple instances of VAO to bind multiple programs
	struct VertexArray : public GLObject
	{
		VertexArray( GLuint ID, const char* name ):GLObject(ID,name,GL_VERTEX_ARRAY),vertex_buffer(nullptr),index_buffer(nullptr),vertex_count(0),index_count(0){};
		virtual ~VertexArray() override { if(vertex_buffer){ delete vertex_buffer; vertex_buffer=nullptr; } if(index_buffer){ delete index_buffer; index_buffer=nullptr; } GLuint id=ID; if(id) glDeleteVertexArrays( 1, &id ); }
		GLuint bind( bool b_bind=true ){ GLuint b0=binding(); if(!b_bind||b0!=ID) glBindVertexArray( b_bind?ID:0 ); return b0; }

		inline void draw_arrays( GLint first, GLsizei count=0, GLenum mode=GL_TRIANGLES ){ bind(); glDrawArrays( mode, first, count?count:GLsizei(vertex_count) ); }
		inline void draw_arrays_instanced( GLint first, GLsizei instance_count, GLsizei count=0, GLenum mode=GL_TRIANGLES ){ bind(); glDrawArraysInstanced( mode, first, count?count:GLsizei(vertex_count), instance_count ); }
		inline void draw_elements( GLuint first, GLsizei count=0, GLenum mode=GL_TRIANGLES ){ if(index_buffer==nullptr) return; bind(); glDrawElements( mode, count?count:GLsizei(index_count), GL_UNSIGNED_INT, uint_offset(first) ); }
		inline void draw_elements_instanced( GLuint first, GLsizei instance_count, GLsizei count=0, GLenum mode=GL_TRIANGLES ){ if(index_buffer==nullptr) return; bind(); glDrawElementsInstanced( mode, count?count:GLsizei(index_count), GL_UNSIGNED_INT, uint_offset(first), instance_count ); }
		inline void draw_elements_indirect( GLvoid* indirect=nullptr, GLenum mode=GL_TRIANGLES ){ if(index_buffer==nullptr) return; bind(); glDrawElementsIndirect( mode, GL_UNSIGNED_INT, indirect ); }
		inline void draw_range_elements( GLuint first, GLuint end, GLsizei count=0, GLenum mode=GL_TRIANGLES ){ if(index_buffer==nullptr) return; bind(); glDrawRangeElements( mode, first, end, count?count:GLsizei(index_count), GL_UNSIGNED_INT, uint_offset(first)); }
		inline void multi_draw_elements( GLuint* pfirst, const GLsizei* pcount, GLsizei draw_count, GLenum mode=GL_TRIANGLES ){ if(index_buffer==nullptr) return; bind(); glMultiDrawElements( mode, pcount, GL_UNSIGNED_INT, uint_offset(pfirst,draw_count), draw_count ); }
		inline void multi_draw_elements_indirect( GLsizei draw_count, GLsizei stride=sizeof(DrawElementsIndirectCommand), GLvoid* indirect=nullptr, GLenum mode=GL_TRIANGLES ){ if(index_buffer==nullptr) return; bind(); glMultiDrawElementsIndirect( mode, GL_UNSIGNED_INT, indirect, draw_count, stride ); }
		inline void multi_draw_elements_indirect_count( GLsizei max_draw_count, GLsizei stride=sizeof(DrawElementsIndirectCommand), const void* indirect=0 /* GL_DRAW_INDIRECT_BUFFER */, GLintptr draw_count=0 /* GL_PARAMETER_BUFFER_ARB */, GLenum mode=GL_TRIANGLES ){ if(index_buffer==nullptr) return; bind(); glMultiDrawElementsIndirectCount(mode,GL_UNSIGNED_INT,indirect,draw_count,max_draw_count,stride); }

		inline GLvoid* uint_offset( GLuint first ){ return (GLvoid*)(first*sizeof(GLuint)); }
		inline const GLvoid* const* uint_offset( GLuint* pfirst, GLsizei draw_count ){ static std::vector<GLvoid*> offsets; if(offsets.size()<uint(draw_count)) offsets.resize(draw_count); for( int k=0; k<draw_count; k++ ) offsets[k] = (GLvoid*)(pfirst[k]*sizeof(GLuint)); return &offsets[0]; }

		Buffer*		vertex_buffer;
		Buffer*		index_buffer;
		size_t		vertex_count;
		size_t		index_count;
	};

	//***********************************************
	struct Program : public GLObject
	{
		struct Uniform
		{
			GLint ID; GLchar name[256]; GLint array_size; GLenum type; GLint textureID; Texture* texture;
			template <class T> inline void set( GLuint program_ID, T* v, GLsizei count )
			{
				switch(type)
				{
				case GL_FLOAT:				glProgramUniform1fv( program_ID, ID, count, (const GLfloat*) v );	break;
				case GL_FLOAT_VEC2:			glProgramUniform2fv( program_ID, ID, count, (const GLfloat*) v );	break;
				case GL_FLOAT_VEC3:			glProgramUniform3fv( program_ID, ID, count, (const GLfloat*) v );	break;
				case GL_FLOAT_VEC4:			glProgramUniform4fv( program_ID, ID, count, (const GLfloat*) v );	break;
				case GL_INT:				glProgramUniform1iv( program_ID, ID, count, (const GLint*) v );		break;
				case GL_INT_VEC2:			glProgramUniform2iv( program_ID, ID, count, (const GLint*) v );		break;
				case GL_INT_VEC3:			glProgramUniform3iv( program_ID, ID, count, (const GLint*) v );		break;
				case GL_INT_VEC4:			glProgramUniform4iv( program_ID, ID, count, (const GLint*) v );		break;
				case GL_UNSIGNED_INT:		glProgramUniform1uiv( program_ID, ID, count, (const GLuint*) v );	break;
				case GL_UNSIGNED_INT_VEC2:	glProgramUniform2uiv( program_ID, ID, count, (const GLuint*)v );	break;
				case GL_UNSIGNED_INT_VEC3:	glProgramUniform3uiv( program_ID, ID, count, (const GLuint*)v );	break;
				case GL_UNSIGNED_INT_VEC4:	glProgramUniform4uiv( program_ID, ID, count, (const GLuint*)v );	break;
				case GL_BOOL:				glProgramUniform1iv( program_ID, ID, count, (const GLint*) v );		break;
				case GL_BOOL_VEC2:			glProgramUniform2iv( program_ID, ID, count, (const GLint*) v );		break;
				case GL_BOOL_VEC3:			glProgramUniform3iv( program_ID, ID, count, (const GLint*) v );		break;
				case GL_BOOL_VEC4:			glProgramUniform4iv( program_ID, ID, count, (const GLint*) v );		break;
				case GL_FLOAT_MAT2:			glProgramUniformMatrix2fv( program_ID, ID, count, GL_TRUE, (const GLfloat*)v );		break;
				case GL_FLOAT_MAT3:			glProgramUniformMatrix3fv( program_ID, ID, count, GL_TRUE, (const GLfloat*)v );		break;
				case GL_FLOAT_MAT4:			glProgramUniformMatrix4fv( program_ID, ID, count, GL_TRUE, (const GLfloat*)v );		break;
				}
			}
		};

		Program( GLuint ID, const char* name ) : GLObject(ID,name,GL_PROGRAM){ get_instances().emplace(this); }
		virtual ~Program() override { if(!ID) return; glDeleteProgram(ID); uniform_cache.clear(); invalid_uniform_cache.clear(); get_instances().erase(this); }
		static void unbind(){ glUseProgram(0); }
		GLuint bind( bool b_bind=true );

		// getUniform: in-program uniform variables
		Uniform* get_uniform( const char* name )
		{
			{auto it=uniform_cache.find(name);if(it!=uniform_cache.end()) return &it->second;}
#ifdef WARN_INVALID_UNIFORM
			{auto it=invalid_uniform_cache.find(name);if(it!=invalid_uniform_cache.end()) return nullptr;}
			printf( "[Warning] %s.get_uniform(%s) not exists\n",this->name, name );
			invalid_uniform_cache.emplace(name);
#endif
			return nullptr;
		}

		// setUniform
		template <class T>
		void set_uniform( const char* name, T* v, GLsizei count=1 ){			Uniform* u=get_uniform(name); if(!u) return; u->set(ID,v,count); }
		void set_uniform( const char* name, const float& v, GLsizei count=1 ){	Uniform* u=get_uniform(name); if(!u) return; u->set(ID,(float*)&v,count); }
		void set_uniform( const char* name, const vec2& v, GLsizei count=1 ){	Uniform* u=get_uniform(name); if(!u) return; u->set(ID,(float*)&v,count); }
		void set_uniform( const char* name, const vec3& v, GLsizei count=1 ){	Uniform* u=get_uniform(name); if(!u) return; u->set(ID,(float*)&v,count); }
		void set_uniform( const char* name, const vec4& v, GLsizei count=1 ){	Uniform* u=get_uniform(name); if(!u) return; u->set(ID,(float*)&v,count); }
		void set_uniform( const char* name, const int& v, GLsizei count=1 ){	Uniform* u=get_uniform(name); if(!u) return; u->set(ID,(int*)&v,count); }
		void set_uniform( const char* name, const ivec2& v, GLsizei count=1 ){	Uniform* u=get_uniform(name); if(!u) return; u->set(ID,(int*)&v,count); }
		void set_uniform( const char* name, const ivec3& v, GLsizei count=1 ){	Uniform* u=get_uniform(name); if(!u) return; u->set(ID,(int*)&v,count); }
		void set_uniform( const char* name, const ivec4& v, GLsizei count=1 ){	Uniform* u=get_uniform(name); if(!u) return; u->set(ID,(int*)&v,count); }
		void set_uniform( const char* name, const uint& v, GLsizei count=1 ){	Uniform* u=get_uniform(name); if(!u) return; u->set(ID,(uint*)&v,count); }
#ifdef _M_X64
		void set_uniform( const char* name, const size_t& v, GLsizei count=1 ){	Uniform* u=get_uniform(name); if(!u) return; if(count==1) u->set(ID,(uint*)&v,count); else {std::vector<uint> l(count);for(int j=0;j<count;j++)l[j]=uint((&v)[j]);u->set(ID,(uint*)&l[0],count);} }
#endif
		void set_uniform( const char* name, const uvec2& v, GLsizei count=1 ){	Uniform* u=get_uniform(name); if(!u) return; u->set(ID,(uint*)&v,count); }
		void set_uniform( const char* name, const uvec3& v, GLsizei count=1 ){	Uniform* u=get_uniform(name); if(!u) return; u->set(ID,(uint*)&v,count); }
		void set_uniform( const char* name, const uvec4& v, GLsizei count=1 ){	Uniform* u=get_uniform(name); if(!u) return; u->set(ID,(uint*)&v,count); }
		void set_uniform( const char* name, const bool& b, GLsizei count=1 ){	Uniform* u=get_uniform(name); if(!u) return; int v=b?1:0; u->set(ID,(int*)&v,count); }
		void set_uniform( const char* name, const mat4& v, GLsizei count=1 ){	Uniform* u=get_uniform(name); if(!u) return; u->set(ID,(float*)&v,count); }
		void set_uniform( const char* name, const mat3& v, GLsizei count=1 ){	Uniform* u=get_uniform(name); if(!u) return; u->set(ID,(float*)&v,count); }
		void set_uniform( const char* name, const mat2& v, GLsizei count=1 ){	Uniform* u=get_uniform(name); if(!u) return; u->set(ID,(float*)&v,count); }
		void set_uniform( const char* name, Texture* t ){ if(!t) return;		Uniform* u=get_uniform(name); if(!u) return; if(u->ID<0||u->textureID<0) return; u->texture=t; glUseProgram(ID); glProgramUniform1i( ID, u->ID, u->textureID ); if(glBindTextureUnit) glBindTextureUnit( u->textureID, u->texture->ID ); else {glActiveTexture(GL_TEXTURE0+u->textureID); u->texture->bind();} }

		// update uniform/uniformBlock
		inline void update_uniform_cache();
		inline void update_uniform_block();

		// uniform block, used with uniform buffer: uniform buffer is assigned from effect to share it with other programs
		struct UniformBlock
		{
			GLuint ID; GLchar name[256]; GLint size=0; Program* program=nullptr; gl::Buffer* buffer=nullptr;
			GLuint get_binding() const { return gxGetActiveUniformBlockiv(program->ID,ID, GL_UNIFORM_BLOCK_BINDING ); }
			void set_binding( GLuint binding_point ){ glUniformBlockBinding(program->ID,ID,binding_point); if(buffer&&buffer->target==GL_UNIFORM_BUFFER) buffer->bind_base(binding_point); }
		};

		// uniform/program block query
		UniformBlock* get_uniform_block( const char* name ){ auto it=uniform_block_map.find(name); return it==uniform_block_map.end() ? nullptr : &it->second; }
		GLuint get_uniform_block_binding( const char* name ){ auto* ub=get_uniform_block(name); return ub?ub->get_binding():-1; }
		GLuint get_shader_storage_block_binding( const char* name ){ const GLenum prop = GL_BUFFER_BINDING; GLint binding;glGetProgramResourceiv(ID,GL_SHADER_STORAGE_BLOCK,glGetProgramResourceIndex(ID,GL_SHADER_STORAGE_BLOCK,name),1,&prop,1,nullptr,&binding); return binding; }

		std::map<std::string,Uniform> uniform_cache;
		std::set<std::string> invalid_uniform_cache;
		std::map<std::string,UniformBlock> uniform_block_map;
		static std::set<Program*>& get_instances(){ static std::set<Program*> i; return i; }
	};

	// template specialization on bool array
	template<> inline void Program::Uniform::set<bool>( GLuint program_ID, bool* v, GLsizei count ){ std::vector<int> i(count);for(int k=0;k<count;k++)i[k]=int(v[k]); glProgramUniform1iv( program_ID, ID, count, &i[0] ); }

	// bind
	inline GLuint Program::bind( bool b_bind )
	{
		GLuint b0=binding(); if(!b_bind||b0!=ID) glUseProgram(b_bind?ID:0); if(!b_bind) return b0;
		if(uniform_cache.empty()) update_uniform_cache();
		else if( Texture::b_texture_deleted() ){std::set<Program*>& s=get_instances();for(auto it:s)it->update_uniform_cache();Texture::b_texture_deleted()=false;}
		for(auto it:uniform_cache)
		{
			Uniform& u=it.second;if(!u.texture||u.textureID==-1) continue;
			if(glBindTextureUnit) glBindTextureUnit( u.textureID, u.texture->ID );
			else {glActiveTexture(GL_TEXTURE0+u.textureID);glBindTexture(u.texture->target,u.texture->ID);}
		}
		return b0;
	}

	// late implementations of Program
	inline void Program::update_uniform_cache()
	{
		GLint program0 = glProgramUniform1i?gxGetIntegerv(GL_CURRENT_PROGRAM):-1; if(program0>=0) glUseProgram(ID);
		for(int k=0,texID=0,n=gxGetProgramiv( ID, GL_ACTIVE_UNIFORMS );k<n;k++)
		{
			Uniform u; GLsizei l; glGetActiveUniform(ID,k,std::extent<decltype(u.name)>::value-1,&l,&u.array_size,&u.type,u.name);
			if(strstr(u.name,"gl_")) continue; u.ID = glGetUniformLocation(ID,u.name); if(u.ID<0) continue;
			u.textureID=-1; u.texture=nullptr; bool bTexture=gxIsSamplerType(u.type);
			if(bTexture){ u.textureID=texID++; if(glProgramUniform1i)glProgramUniform1i(ID,u.ID,u.textureID);else glUniform1i(u.ID,u.textureID); } // setting sampler locations avoids validation error in Intel Compiler
			uniform_cache[u.name]=u; if(u.array_size==1) continue;
			GLchar name[256]; strcpy(name,u.name); GLint loc=0; GLchar* bracket=strchr(name,'['); if(bracket) bracket[0]='\0';
			for( GLint loc=bracket?1:0;loc<u.array_size;loc++){ Uniform u1=u;sprintf(u1.name,"%s[%d]",name,loc);u1.ID=glGetUniformLocation(ID,u1.name);if(u1.ID==-1)continue;u1.array_size=u.array_size-loc;if(bTexture)u1.textureID=texID++;uniform_cache[u1.name]=u1; }
		}
		if(program0>=0) glUseProgram(program0); // restore the original program
	}

	inline void Program::update_uniform_block()
	{
		uniform_block_map.clear();
		for(int k=0,kn=gxGetProgramiv(ID,GL_ACTIVE_UNIFORM_BLOCKS);k<kn;k++)
		{
			UniformBlock ub; ub.ID = k; ub.program = this; ub.size = gxGetActiveUniformBlockiv(ID,k,GL_UNIFORM_BLOCK_DATA_SIZE);
			GLsizei l=gxGetActiveUniformBlockiv(ID,k,GL_UNIFORM_BLOCK_NAME_LENGTH); /* length includes NULL */ if(l>std::extent<decltype(UniformBlock::name)>::value) printf("[%s] uniform block name is too long\n",name);
			glGetActiveUniformBlockName(ID,k,l,&l,ub.name);
			uniform_block_map[ ub.name ] = ub;
		}
	}

	//***********************************************
	// a set of programs
	struct Effect : public GLObject
	{
		Effect( GLuint ID, const char* name ) : GLObject(ID,name,0), active_program(nullptr), quad(nullptr){ if(!(quad=gxCreateQuadVertexArray())) printf("[%s] unable to create quad buffer\n",name); }
		virtual ~Effect() override { active_program=nullptr; if(quad){ delete quad; quad=nullptr; } if(!pts.empty()){ for(auto it:pts) safe_delete(it.second); pts.clear(); } for(auto& it:uniform_buffer_map){if(it.second){ delete it.second; it.second=nullptr; }} uniform_buffer_map.clear(); for(auto* p:programs) delete p; programs.clear(); }
		static void unbind(){ glUseProgram(0); }

		Program* bind( const char* programName ){ active_program=get_program(programName); if(active_program) active_program->bind(); else{ active_program=nullptr; glUseProgram(0); } return active_program; }
		Program* bind( uint index ){ active_program=get_program(index); if(active_program) active_program->bind(); else { active_program=nullptr; glUseProgram(0); } return active_program; }

		bool empty() const { return programs.empty(); }
		size_t size() const { return programs.size(); }
		Program* get_program( const char* programName ) const { for(uint k=0;k<programs.size();k++)if(_stricmp(programs[k]->name,programName)==0) return programs[k]; printf("Unable to find program \"%s\" in effect \"%s\"\n", programName, name ); return nullptr; }
		Program* get_program( uint index ) const { if(index<programs.size()) return programs[index]; else { printf("[%s] Out-of-bound program index\n", name ); return nullptr; } }
		bool create_program( const char* prefix, const char* name, const std::map<GLuint,std::string>& shaderSourceMap, const char* pMacro=nullptr, std::vector<const char*>* tfVaryings=nullptr ){ Program* program=gxCreateProgram(prefix,name,shaderSourceMap,pMacro,tfVaryings); if(!program) return false; programs.emplace_back(program); auto& m=program->uniform_block_map;for(auto& it:m){gl::Program::UniformBlock& ub=it.second;ub.buffer=get_or_create_uniform_buffer(ub.name,ub.size);} return true; }
		bool create_program( const char* prefix, const char* name, const char* vertShaderSource, const char* fragShaderSource, const char* pMacro=nullptr ){ std::map<GLuint,std::string> ssm={std::make_pair(GL_VERTEX_SHADER,vertShaderSource),std::make_pair(GL_FRAGMENT_SHADER,fragShaderSource)}; return create_program(prefix,name,ssm,pMacro,nullptr); }
		bool attach( const char* name, const char* effect_source, const char* p_macro=nullptr );

		Program::Uniform* get_uniform( const char* name ){ return active_program?active_program->get_uniform(name):nullptr; }
		Program::Uniform* get_uniform( const std::string& name ){ return active_program?active_program->get_uniform(name.c_str()):nullptr; }
		template <class T>	void set_uniform( const char* name, const T& v, GLsizei count=1 ){ if(active_program) active_program->set_uniform(name,v,count); }
		template <class T>	void set_uniform( const char* name, T* v, GLsizei count=1 ){ if(active_program) active_program->set_uniform(name,v,count); }
		void set_uniform( const char* name, Texture* t ){ if(active_program) active_program->set_uniform(name,t); }
		template <class T>	void set_uniform( const std::string& name, const T& v, GLsizei count=1 ){ if(active_program) active_program->set_uniform(name.c_str(),v,count); }
		template <class T>	void set_uniform( const std::string& name, T* v, GLsizei count=1 ){ if(active_program) active_program->set_uniform(name.c_str(),v,count); }
		void set_uniform( const std::string& name, Texture* t ){ if(active_program) active_program->set_uniform(name.c_str(),t); }

		// uniform buffer/block
		gl::Buffer* get_or_create_uniform_buffer( const char* name, size_t size ){ gl::Buffer* b=get_uniform_buffer(name); if(b&&b->size()!=size){ static std::set<std::string> warns; auto it=warns.find(name); if(it==warns.end()){ warns.insert(name); printf("uniform_buffer(%s).size(=%d)!=%d\n",name,int(b->size()),int(size));} } if(b) return b; b=gxCreateBuffer(name,GL_UNIFORM_BUFFER,size,GL_STATIC_DRAW,nullptr,GL_DYNAMIC_STORAGE_BIT|GL_MAP_WRITE_BIT); if(b){ bind_uniform_buffer(name,b); return uniform_buffer_map[name]=b; } printf("[%s] unable to create uniform buffer [%s]\n", this->name, name); return nullptr; }
		gl::Buffer* get_uniform_buffer( const char* name ){ auto it=uniform_buffer_map.find(name); return it==uniform_buffer_map.end()?nullptr:it->second; }
		GLuint get_uniform_block_binding( const char* name ){ GLint binding=active_program?active_program->get_uniform_block_binding(name):-1; if(binding!=-1) return binding; for( auto* program : programs ){ GLint binding=program->get_uniform_block_binding(name); if(binding!=-1) return binding; } return -1; }
		void bind_uniform_buffer( const char* name, gl::Buffer* ub=nullptr /* if nullptr, use default buffer */ ){ gl::Buffer* b=ub?ub:get_uniform_buffer(name); GLuint binding=get_uniform_block_binding(name); if(b&&binding!=-1) b->bind_base(binding); else if(!b) printf( "[%s] bind_uniform_buffer(): unable to find uniform buffer %s\n", this->name, name ); else printf( "[%s] bind_uniform_buffer(): unable to find uniform buffer binding %s\n", this->name, name ); }

		// blocks: uniform and shader_storage
		GLuint get_shader_storage_block_binding( const char* name ){ GLint binding=active_program?active_program->get_shader_storage_block_binding(name):-1; if(binding!=-1) return binding; for( auto* program : programs ){ GLint binding=program->get_shader_storage_block_binding(name); if(binding!=-1) return binding; } printf( "[%s] bind_uniform_buffer(): unable to find shader storage block binding %s\n", this->name, name ); return -1; }
		void bind_shader_storage_buffer( const char* name, Buffer* buffer ){ GLint binding=get_shader_storage_block_binding(name); if(binding<0) return; if(buffer->target==GL_SHADER_STORAGE_BUFFER) buffer->bind_base(binding); else buffer->bind_base_as(GL_SHADER_STORAGE_BUFFER,binding); }

		// draw or compute
		inline void draw_quads(){ if(!quad) return; if(quad->index_buffer) quad->draw_elements(0,4,GL_TRIANGLE_STRIP); else quad->draw_arrays(0,4,GL_TRIANGLE_STRIP); }
		inline void draw_points( GLsizei width, GLsizei height, bool no_attrib=false ){ if(no_attrib) return draw_points_no_attrib( width*height ); uint64_t key=uint64_t(width)|(uint64_t(height)<<32); auto it=pts.find(key); VertexArray* va=it!=pts.end()?it->second:pts[key]=gxCreatePointVertexArray(width,height); if(va) va->draw_arrays( 0, width*height, GL_POINTS ); }
		inline void draw_points_no_attrib( GLsizei count ){ glBindVertexArray(0); glDrawArrays( GL_POINTS, 0, count ); } // attribute-less rendering without binding any vertex array: simply using gl_VertexID in vertex shaders
		inline void dispatch_compute( GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z=1 ){ glDispatchCompute( num_groups_x?num_groups_x:1, num_groups_y?num_groups_y:1, num_groups_z?num_groups_z:1 ); }
		inline void dispatch_compute( GLuint num_threads_x, double local_size_x, GLuint num_threads_y, double local_size_y, GLuint num_threads_z=1, double local_size_z=1 )	{ GLuint num_groups_x = max(GLuint(ceil(num_threads_x/float(local_size_x))),1u), num_groups_y = max(GLuint(ceil(num_threads_y/float(local_size_y))),1u), num_groups_z = max(GLuint(ceil(num_threads_z/float(local_size_z))),1u); dispatch_compute(num_groups_x, num_groups_y, num_groups_z ); }
		inline void dispatch_compute_indirect( GLintptr indirect ){ glDispatchComputeIndirect( indirect ); }

		// internal members
		Program*						active_program;
		std::vector<Program*>			programs;
		std::map<std::string,Buffer*>	uniform_buffer_map;	// Do not define uniform buffers in each program, since they are shared across programs.
		std::map<uint64_t,VertexArray*>	pts;				// point vertex array
		VertexArray*					quad;
	};

	//***********************************************
	struct Framebuffer : public GLObject
	{
		struct depth_key_t { union { struct {uint width:16, height:16, multisamples:8, layers:16, renderbuffer:1, dummy:7;}; uint64_t value; }; depth_key_t():value(0){} };

		Framebuffer( GLuint ID, const char* name ) : GLObject(ID,name,GL_FRAMEBUFFER),b_color_mask(true),b_depth_mask(true){ memset(active_targets,0,sizeof(active_targets)); glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ); }
		virtual ~Framebuffer() override { glBindFramebuffer(GL_FRAMEBUFFER,0); glBindRenderbuffer(GL_RENDERBUFFER,0);memset(active_targets,0,sizeof(active_targets)); GLuint id=ID; if(ID) glDeleteFramebuffers(1,&id); for(auto& it:depth_buffers){const depth_key_t& key=reinterpret_cast<const depth_key_t&>(it.first); if(key.renderbuffer) glDeleteRenderbuffers(1,&it.second); else glDeleteTextures(1,&it.second); } depth_buffers.clear(); }
		GLuint bind( bool b_bind=true ){ GLuint b0=binding(); if(!b_bind||ID!=b0) glBindFramebuffer( GL_FRAMEBUFFER, b_bind?ID:0 ); return b0; }

		void bind( Texture* t0, Texture* t1=nullptr, Texture* t2=nullptr, Texture* t3=nullptr, Texture* t4=nullptr, Texture* t5=nullptr, Texture* t6=nullptr, Texture* t7=nullptr ){ if(ID) bind(t0,0,0,t1,0,0,t2,0,0,t3,0,0,t4,0,0,t5,0,0,t6,0,0,t7,0,0); }
		void bind( Texture* t0, GLint layer0, GLint mipLevel0, Texture* t1=nullptr, GLint layer1=0, GLint mipLevel1=0, Texture* t2=nullptr, GLint layer2=0, GLint mipLevel2=0, Texture* t3=nullptr, GLint layer3=0, GLint mipLevel3=0, Texture* t4=nullptr, GLint layer4=0, GLint mipLevel4=0, Texture* t5=nullptr, GLint layer5=0, GLint mipLevel5=0, Texture* t6=nullptr, GLint layer6=0, GLint mipLevel6=0, Texture* t7=nullptr, GLint layer7=0, GLint mipLevel7=0 );
		void bind_no_attachments( GLint width, GLint height, GLint layers=1, GLint samples=1 ); // ARB_framebuffer_no_attachments
		void bind_layers( Texture* t0, GLint mipLevel0, Texture* t1=nullptr, GLint mipLevel1=0, Texture* t2=nullptr, GLint mipLevel2=0, Texture* t3=nullptr, GLint mipLevel3=0, Texture* t4=nullptr, GLint mipLevel4=0, Texture* t5=nullptr, GLint mipLevel5=0, Texture* t6=nullptr, GLint mipLevel6=0, Texture* t7=nullptr, GLint mipLevel7=0 ); // t needs to be multi-layers
		void bind_layers( Texture* t0, Texture* t1=nullptr, Texture* t2=nullptr, Texture* t3=nullptr, Texture* t4=nullptr, Texture* t5=nullptr, Texture* t6=nullptr, Texture* t7=nullptr ){ bind_layers(t0,0,t1,0,t2,0,t3,0,t4,0,t5,0,t6,0,t7,0); } // t needs to be multi-layers
		void bind_depth_buffer( GLint width, GLint height, GLint layers, bool multisample, GLsizei multisamples );
		void unbind_depth_buffer();
		void check_status(){ GLenum s=glCheckNamedFramebufferStatus?glCheckNamedFramebufferStatus(ID,GL_FRAMEBUFFER):glCheckFramebufferStatus(GL_FRAMEBUFFER); if(s==GL_FRAMEBUFFER_COMPLETE) return; if(s==GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) printf( "[%s] GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT\n", name ); else if(s==GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT) printf( "[%s] GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT\n", name ); else if(s==GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER) printf( "[%s] GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER\n", name ); else if(s==GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER) printf( "[%s] GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER\n", name ); else if(s==GL_FRAMEBUFFER_UNSUPPORTED) printf( "[%s] GL_FRAMEBUFFER_UNSUPPORTED\n", name ); }
		void read_pixels( GLenum attachment, GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid* img ){ if(ID) glNamedFramebufferReadBuffer(ID,GL_COLOR_ATTACHMENT0+attachment); else glReadBuffer(GL_BACK); glReadPixels( x, y, width, height, format, type, img ); }
		void set_viewport( GLint x, GLint y, GLsizei width, GLsizei height ){ glViewport( x, y, width, height ); }
		void set_viewport( ivec4 origin_size ){ glViewport( origin_size.x, origin_size.y, origin_size.z, origin_size.w ); }
		void set_viewport( ivec2 origin, ivec2 size ){ glViewport( origin.x, origin.y, size.x, size.y ); }
		void set_viewport( ivec2 size ){ glViewport( 0, 0, size.x, size.y ); }
		ivec4 get_viewport(){ ivec4 i; glGetIntegerv( GL_VIEWPORT, (int*)i); return i; }
		void set_color_mask( bool mask, bool b_force_mask=false ){ if(!b_force_mask&&mask==b_color_mask) return; GLboolean b=(b_color_mask=mask)?GL_TRUE:GL_FALSE; glColorMask(b,b,b,b); }
		void set_depth_mask( bool mask, bool b_force_mask=false ){ if(!b_force_mask&&mask==b_depth_mask) return; GLboolean b=(b_depth_mask=mask)?GL_TRUE:GL_FALSE; glDepthMask(b); }
		void set_color_depth_mask( bool mask, bool b_force_mask=false ){ set_color_mask(mask,b_force_mask); set_depth_mask(mask,b_force_mask); }
		void set_read_buffer( GLenum mode ){ if(glNamedFramebufferReadBuffer) glNamedFramebufferReadBuffer( ID, mode ); else glReadBuffer( mode ); }
		void set_draw_buffer( GLenum mode ){ if(glNamedFramebufferDrawBuffer) glNamedFramebufferDrawBuffer( ID, mode ); else glDrawBuffer( mode ); }

		static const GLint MAX_COLOR_ATTACHMENTS=8;
		static void unbind(){ glBindFramebuffer( GL_FRAMEBUFFER, 0 ); }
		static void set_state( bool b_depth_test, bool b_cull_face, bool b_blend=false, bool b_wireframe=false ){ b_depth_test?glEnable(GL_DEPTH_TEST):glDisable(GL_DEPTH_TEST); b_cull_face?glEnable(GL_CULL_FACE):glDisable(GL_CULL_FACE); b_blend?glEnable(GL_BLEND):glDisable(GL_BLEND); glPolygonMode(GL_FRONT_AND_BACK,b_wireframe?GL_LINE:GL_FILL); }
		static void set_blend_func( GLenum sfactor, GLenum dfactor ){ glBlendFunc( sfactor, dfactor ); }
		static void set_blend_func_separate( GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha ){ glBlendFuncSeparate( sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha ); }
		static void set_multisample( bool b_multisample ){ b_multisample ? glEnable(GL_MULTISAMPLE):glDisable(GL_MULTISAMPLE); }
		static const GLenum* draw_buffers( uint index=0 ){ static GLenum d[MAX_COLOR_ATTACHMENTS]={GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2,GL_COLOR_ATTACHMENT3,GL_COLOR_ATTACHMENT4,GL_COLOR_ATTACHMENT5,GL_COLOR_ATTACHMENT6,GL_COLOR_ATTACHMENT7}; return &d[index]; }
		void clear( const vec4& color=vec4(.0f,.0f,.0f,.0f) ){ for( uint k=0;k<MAX_COLOR_ATTACHMENTS;k++) if(active_targets[k]) clear_color_buffer( k, color, false ); clear_depth_buffer(); }
		void clear_depth_buffer( float depth=1.0f ){ if(glClearNamedFramebufferfv) glClearNamedFramebufferfv( ID, GL_DEPTH, 0, &depth ); else glClearBufferfv( GL_DEPTH, 0, &depth ); }
		void clear_color_buffer( GLint draw_buffer, const vec4& color, bool b_clear_depth=false ){ if(draw_buffer>=GL_DRAW_BUFFER0) draw_buffer-=GL_DRAW_BUFFER0; if(glClearNamedFramebufferfv) glClearNamedFramebufferfv( ID, GL_COLOR, draw_buffer, color ); else glClearBufferfv( GL_COLOR, draw_buffer, color ); if(b_clear_depth) clear_depth_buffer(); }
		void clear_color_bufferi( GLint draw_buffer, const ivec4& color, bool b_clear_depth=false ){ if(draw_buffer>=GL_DRAW_BUFFER0) draw_buffer-=GL_DRAW_BUFFER0; if(glClearNamedFramebufferiv) glClearNamedFramebufferiv( ID, GL_COLOR, draw_buffer, color ); else glClearBufferiv( GL_COLOR, draw_buffer, color ); if(b_clear_depth) clear_depth_buffer(); }
		void clear_color_bufferui( GLint draw_buffer, const uvec4& color, bool b_clear_depth=false ){ if(draw_buffer>=GL_DRAW_BUFFER0) draw_buffer-=GL_DRAW_BUFFER0; if(glClearNamedFramebufferuiv) glClearNamedFramebufferuiv( ID, GL_COLOR, draw_buffer, color ); else glClearBufferuiv( GL_COLOR, draw_buffer, color ); if(b_clear_depth) clear_depth_buffer(); }

		GLenum	active_targets[MAX_COLOR_ATTACHMENTS];
		std::map<uint64_t,GLuint> depth_buffers;
		bool b_color_mask, b_depth_mask;
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
			GLenum target = t ? (active_targets[k]=t->target) : active_targets[k];
			if(target==GL_TEXTURE_3D||target==GL_TEXTURE_1D_ARRAY||target==GL_TEXTURE_2D_ARRAY||target==GL_TEXTURE_2D_MULTISAMPLE_ARRAY||target==GL_TEXTURE_CUBE_MAP||target==GL_TEXTURE_CUBE_MAP_ARRAY) glFramebufferTextureLayer( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+k, t?t->ID:0, t?M[k]:0, t?L[k]:0 );
			else if(target==GL_TEXTURE_1D||target==GL_TEXTURE_2D||target==GL_TEXTURE_2D_MULTISAMPLE||target==GL_TEXTURE_BUFFER||target==GL_TEXTURE_RECTANGLE) glFramebufferTexture( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+k, t?t->ID:0, t?M[k]:0 );
			if(t) draw_buffer_count++; else active_targets[k]=0;
		}

		// detach all depth buffers for no-attachments
		if(draw_buffer_count==0){ unbind_depth_buffer(); return; } // detach all color/depth buffers for no-attachments

		// if there are attachments, set viewport and bind depth buffer
		GLint width=t0->width(mipLevel0), height=t0->height(mipLevel0);
		glViewport( 0, 0, width, height );
		
		// bind depth buffer, when depth test is enabled
		bool b_multisample = active_targets[0]==GL_TEXTURE_2D_MULTISAMPLE||active_targets[0]==GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
		bind_depth_buffer( width, height, 1, b_multisample, b_multisample&&T[0]?T[0]->multisamples():1 );
		
		// set the draw buffers, even without attachments; then check the status
		if(glNamedFramebufferDrawBuffers)	glNamedFramebufferDrawBuffers( ID, draw_buffer_count, draw_buffers() );
		else								glDrawBuffers( draw_buffer_count, draw_buffers() );
		check_status();
	}

	inline void Framebuffer::bind_layers( Texture* t0, GLint mipLevel0, Texture* t1, GLint mipLevel1, Texture* t2, GLint mipLevel2, Texture* t3, GLint mipLevel3, Texture* t4, GLint mipLevel4, Texture* t5, GLint mipLevel5, Texture* t6, GLint mipLevel6, Texture* t7, GLint mipLevel7 ) // for gl_Layer redirection in geometry shader
	{
		if(ID==0||t0==nullptr){ bind(false); return; }

		if(t0->target!=GL_TEXTURE_1D_ARRAY&&t0->target!=GL_TEXTURE_2D_ARRAY&&t0->target!=GL_TEXTURE_2D_MULTISAMPLE_ARRAY&&t0->target!=GL_TEXTURE_3D&&t0->target!=GL_TEXTURE_CUBE_MAP&&t0->target!=GL_TEXTURE_CUBE_MAP_ARRAY){ printf("[%s] Framebuffer::bind_layers() supports only array textures\n", name ); return; }

		// regardless of direct_state_access, it is still necessary, because draw functions are not aware where they are drawn.
		glBindFramebuffer( GL_FRAMEBUFFER, ID );

		gl::Texture*	T[MAX_COLOR_ATTACHMENTS] = { t0, t1, t2, t3, t4, t5, t6, t7 };
		GLint			M[MAX_COLOR_ATTACHMENTS] = { mipLevel0, mipLevel1, mipLevel2, mipLevel3, mipLevel4, mipLevel5, mipLevel6, mipLevel7 };

		uint num_draw_buffers=0;
		for( int k=0; k < MAX_COLOR_ATTACHMENTS; k++ )
		{
			gl::Texture* t = T[k];
			glFramebufferTexture( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+k, t?t->ID:0, t?M[k]:0 );
			if(t){ num_draw_buffers++; active_targets[k] = t->target; }
			else active_targets[k] = 0;
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
		auto it=depth_buffers.find(key.value); if(it!=depth_buffers.end()) idx=it->second;
		else if(layers==1)
		{
			glBindRenderbuffer( GL_RENDERBUFFER, depth_buffers[key.value] = idx = gxCreateRenderBuffer() );
			if(multisample)	glNamedRenderbufferStorageMultisample?glNamedRenderbufferStorageMultisample( idx, multisamples, GL_DEPTH_COMPONENT32F, width, height ):glRenderbufferStorageMultisample( GL_RENDERBUFFER, multisamples, GL_DEPTH_COMPONENT32F, width, height );
			else			glNamedRenderbufferStorage?glNamedRenderbufferStorage( idx, GL_DEPTH_COMPONENT32F, width, height ):glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, width, height );
		}
		else
		{
			GLenum target=multisample?GL_TEXTURE_2D_MULTISAMPLE_ARRAY:GL_TEXTURE_2D_ARRAY; glBindTexture( target, depth_buffers[key.value] = idx = gxCreateTexture(target) );
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

//***********************************************
} // end namespace gl
//***********************************************

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
} // end namespace gl
//***********************************************

inline gl::Query* gxCreateQuery( const char* name, GLenum target=GL_TIME_ELAPSED )
{
	GLuint ID; if(glCreateQueries) glCreateQueries( target, 1, &ID ); else glGenQueries(1,&ID); if(ID==0){ printf( "Unable to create query[%s]", name ); return nullptr; }
	return new gl::Query(ID,name,target);
}

inline gl::Buffer* gxCreateBuffer( const char* name, GLenum target, GLsizeiptr size, GLenum usage, const void* data=nullptr, GLbitfield storage_flags=GL_DYNAMIC_STORAGE_BIT|GL_MAP_READ_BIT|GL_MAP_WRITE_BIT )
{
	GLuint ID; if(glCreateBuffers) glCreateBuffers(1,&ID); else glGenBuffers(1,&ID); if(ID==0){ printf( "Unable to create buffer[%s]", name ); return nullptr; }	// glCreateBuffers() also initializes objects, while glGenBuffers() do not initialze until bind() is called
	gl::Buffer* buffer = new gl::Buffer(ID,name,target);

	if(glNamedBufferStorage) glNamedBufferStorage(ID,size,data,storage_flags);
	else if(glBufferStorage){ GLuint b0=buffer->bind(); glBufferStorage(target,size,data,storage_flags); glBindBuffer(target,b0); }
	else if(glNamedBufferData) glNamedBufferData(ID,size,data,usage);
	else if(glBufferData){ GLuint b0=buffer->bind(); glBufferData(target,size,data,usage); glBindBuffer(target,b0); }

	return buffer;
}

inline gl::VertexArray* gxCreateVertexArray( const char* name, vertex* p_vertices, size_t vertex_count, uint* p_indices=nullptr, size_t index_count=0, GLenum usage=GL_STATIC_DRAW )
{
	if(vertex_count==0){ printf( "%s(%s): vertex_count==0\n", __func__, name ); return nullptr; }

	GLuint ID=gxCreateVertexArray(); if(ID==0) return nullptr;
	gl::VertexArray* va = new gl::VertexArray( ID, name );

	va->vertex_buffer = gxCreateBuffer( "vertexBuffer", GL_ARRAY_BUFFER, sizeof(vertex)*vertex_count, usage, p_vertices, 0 ); if(va->vertex_buffer==nullptr){ printf( "%s(): unable to create vertex_buffer\n", __func__ ); delete va; return nullptr; }
	if(p_indices&&index_count){ va->index_buffer = gxCreateBuffer( "indexBuffer", GL_ELEMENT_ARRAY_BUFFER, sizeof(uint)*index_count, usage, p_indices ); if(va->index_buffer==nullptr){ printf( "%s(): unable to create index_buffer\n", __func__ ); delete va; return nullptr; } }

	// use fixed binding (without direct state access)
	va->bind();
	static const GLuint offset[] = { offsetof(vertex,pos), offsetof(vertex,norm), offsetof(vertex,tex) };
	static const GLint  size[]	 = { sizeof(vertex::pos)/sizeof(GLfloat), sizeof(vertex::norm)/sizeof(GLfloat), sizeof(vertex::tex)/sizeof(GLfloat) };
	for( GLuint k=0; k<3; k++ ){ glEnableVertexAttribArray( k ); glVertexAttribBinding( k, 0 ); glVertexAttribFormat( k, size[k], GL_FLOAT, GL_FALSE, offset[k] ); }
	glBindVertexBuffer( 0, va->vertex_buffer->ID, 0, sizeof(vertex) );
	if(va->index_buffer) glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, va->index_buffer->ID );
	va->bind(false);

	va->vertex_count = vertex_count;
	va->index_count = va->index_buffer ? index_count : 0;

	return va;
}

inline gl::VertexArray* gxCreateQuadVertexArray()
{
	static vertex vertices[] = { {vec3(-1,-1,0),vec3(0,0,1),vec2(0,0)}, {vec3(1,-1,0),vec3(0,0,1),vec2(1,0)}, {vec3(-1,1,0),vec3(0,0,1),vec2(0,1)}, {vec3(1,1,0),vec3(0,0,1),vec2(1,1)} }; // strip ordering [0, 1, 3, 2]
	gl::VertexArray* va = gxCreateVertexArray( "QUAD", vertices, std::extent<decltype(vertices)>::value ); if(!va) printf( "%s(): Unable to create QUAD\n", __func__ );
	return va;
}

inline gl::VertexArray* gxCreatePointVertexArray( GLsizei width, GLsizei height )
{
	std::vector<vertex> pts(width*height); for(int y=0,k=0;y<height;y++)for(int x=0;x<width;x++,k++) pts[k]={vec3(float(x),float(y),0.0f),vec3(0.0f,0.0f,1.0f),vec2(x/float(width-1),y/float(height-1))};
	gl::VertexArray* va = gxCreateVertexArray( "PTS", &pts[0], pts.size() ); if(!va) printf( "%s(): Unable to create PTS(%dx%d)\n", __func__, width, height );
	return va;
}

inline unsigned int gl::crc32c( const void* ptr, size_t size, unsigned int crc0 )
{
	const unsigned char* buff= (unsigned char*) ptr;
	static unsigned* t[4] = {nullptr}; if(!t[0]){ for(int k=0;k<4;k++) t[k]=(unsigned*) malloc(sizeof(unsigned)*256); for(int k=0;k<256;k++){ unsigned c=k; for( unsigned j=0;j<8;j++) c=c&1?0x82f63b78UL^(c>>1):c>>1; t[0][k]=c; } for(int k=0;k<256;k++){ unsigned c=t[0][k]; for(int j=1;j<4;j++) t[j][k]=c=t[0][c&0xff]^(c>>8); } }
	if(buff==nullptr||size==0) return crc0; unsigned c = ~crc0;
	for(;size&&(((ptrdiff_t)buff)&7);size--,buff++) c=t[0][(c^(*buff))&0xff]^(c>>8); // move forward to the 8-byte aligned boundary
	for(;size>=4;size-=4,buff+=4){c^=*(unsigned*)buff;c=t[3][(c>>0)&0xff]^t[2][(c>>8)&0xff]^t[1][(c>>16)&0xff]^t[0][(c>>24)&0xff]; }
	for(;size;size--,buff++) c=t[0][(c^(*buff))&0xff]^(c>>8);
	return ~c;
}

inline void gxSaveProgramBinary( const char* name, GLuint ID, uint crc )
{
	std::vector<char> program_binary(gxGetProgramiv( ID, GL_PROGRAM_BINARY_LENGTH ),0);
	GLenum binary_format=0; glGetProgramBinary( ID, GLsizei(program_binary.size()), nullptr, &binary_format, &program_binary[0] );
	FILE* fp = _wfopen( gxGetProgramBinaryPath(name), L"wb");
	fwrite( &crc, sizeof(crc), 1, fp );
	fwrite( &binary_format, sizeof(GLenum), 1, fp );
	fwrite( &program_binary[0], sizeof(char), program_binary.size(), fp );
	fclose(fp);
}

inline GLuint gxLoadProgramBinary( const char* name, uint crc )
{
	path program_binary_path = gxGetProgramBinaryPath(name); if(!program_binary_path.exists()) return 0;
	size_t crc_size=sizeof(crc), offset=crc_size+sizeof(GLenum);
	size_t program_binary_size = program_binary_path.file_size();
	if(program_binary_size<offset) return 0; // no md5 hash and binary format exists
	std::vector<char> buff(program_binary_size);
	FILE* fp = _wfopen( program_binary_path, L"rb"); if(fp==nullptr) return 0; fread( &buff[0], sizeof(char), program_binary_size, fp ); fclose(fp);
	if( memcmp( &buff[0], &crc, crc_size ) !=0 ) return 0;
	GLenum binary_format=0; memcpy( &binary_format, (&buff[0])+crc_size, sizeof(GLenum) );
	GLuint ID = glCreateProgram(); glProgramBinary( ID, binary_format, (&buff[0])+offset, GLsizei(program_binary_size-offset) );
	return ID;
}

inline void gxInfoLog( const char* name, const char* msg, std::map<size_t,std::string>* lines=nullptr )
{
	char L[53]={0}; for(int k=0;k<50;k++)L[k]='_'; L[50]=L[51]='\n';
	printf("\n"); printf("%s",L);
	std::vector<std::string> vm=explode(msg,"\n");
	for( size_t k=0,kn=vm.size();k<kn;k++ )
	{
		const char* v = vm[k].c_str(); printf("%s: %s\n", name, v ); if(lines==nullptr) continue;
		char s[16384]={0};const char *l=strchr(v,'('), *r=strchr(v,')'); if(!l||!r||l>=r) continue;
		memcpy(s,l+1,r-l-1); s[r-l-1]=0; int idx=atoi(s);
		auto it=lines->find(size_t(idx-1));	if(it!=lines->end()) printf( "%d> %s\n", idx-1, trim(str_replace(it->second.c_str(),"%", "%%")) );	// % causes crash in gprintf
		it=lines->find(size_t(idx+0));		if(it!=lines->end()) printf( "%d> %s\n", idx+0, trim(str_replace(it->second.c_str(),"%", "%%")) );
		it=lines->find(size_t(idx+1));		if(it!=lines->end()) printf( "%d> %s\n", idx+1, trim(str_replace(it->second.c_str(),"%", "%%")) );
	}
	printf("%s",L);
}

// explode shader source by exploiting #line directives
inline std::vector<std::pair<size_t,std::string>> gxExplodeShaderSource( const char* source )
{
	std::vector<std::pair<size_t,std::string>> v;
	std::vector<const char*> vs = explode_conservative(source,'\n');
	for( int k=0,kn=int(vs.size()),idx=1; k<kn;k++,idx++ )
	{
		const char* s = trim(vs[k]);
		bool b_line = strncmp(s,"#line",5)==0;
		if(b_line) sscanf(str_replace(s,"\t"," "),"#line %d",&idx);
		v.emplace_back( std::make_pair(idx,vs[k]) );
		if(b_line) idx--;
	}
	return v;
}

inline std::map<size_t,std::string> gxExplodeShaderSourceMap( const char* source )
{
	auto v = gxExplodeShaderSource(source);
	std::map<size_t,std::string> m; for( size_t k=0,kn=v.size();k<kn;k++) if(!strstr(v[k].second.c_str(),"#line")) m[v[k].first]=v[k].second;
	return m;
}

inline GLuint gxCompileShaderSource( GLenum shader_type, const char* name, const char* source )
{
	GLuint ID = glCreateShader( shader_type ); if(ID==0){ printf("compileShaderSource(): unable to glCreateShader(%d)\n", shader_type); return 0; }
	glShaderSource( ID, 1, &source, nullptr );
	glCompileShader( ID );

	static const int MAX_LOG_LENGTH=8192; static char msg[MAX_LOG_LENGTH] = {0}; GLint L; bool bLogExists;
	glGetShaderInfoLog(ID,MAX_LOG_LENGTH,&L,msg); bLogExists=L>1&&L<=MAX_LOG_LENGTH;
	if(bLogExists){ auto v=gxExplodeShaderSourceMap(source); gxInfoLog(name,msg,&v); }
	if(gxGetShaderiv(ID,GL_COMPILE_STATUS)!=GL_TRUE){ glDeleteShader(ID); return 0; }

	return ID;
}

inline bool gxCheckProgramLink( const char* name, GLuint ID, bool bLog=true )
{
	if(ID==0) return false;
	static const int MAX_LOG_LENGTH=4096; static char msg[MAX_LOG_LENGTH] = {0}; GLint L; bool bLogExists;
	glGetProgramInfoLog(ID,MAX_LOG_LENGTH,&L,msg); bLogExists=L>1&&L<=MAX_LOG_LENGTH; if(bLogExists&&bLog) gxInfoLog(name,msg); if(gxGetProgramiv(ID,GL_LINK_STATUS)!=GL_TRUE){ glDeleteProgram(ID); return false; }
	return true;
}

inline bool gxValidateProgram( const char* name, GLuint ID, bool bLog=true ) // check if the program can run in the current state
{
	if(ID==0) return false;
	static const int MAX_LOG_LENGTH=4096; static char msg[MAX_LOG_LENGTH] = {0}; GLint L; bool bLogExists;
	glValidateProgram(ID); glGetProgramInfoLog(ID,MAX_LOG_LENGTH,&L,msg); bLogExists=L>1&&L<=MAX_LOG_LENGTH; if(bLogExists&&bLog) gxInfoLog(name,msg); if(gxGetProgramiv(ID,GL_VALIDATE_STATUS)!=GL_TRUE){ glDeleteProgram(ID); return false; }
	return true;
}

// the last element of attribNames should be nullptr
inline gl::Program* gxCreateProgram( const char* prefix, const char* name, const std::map<GLuint,std::string>& shader_source_map, const char* p_macro, std::vector<const char*>* tf_varyings )
{
	std::string sname = format("%s%s%s",prefix?prefix:"",prefix?".":"",name); const char* pname=sname.c_str();

	// 1. combine macro with shader sources, and add layout qualifier
	std::string macro = p_macro&&p_macro[0]?p_macro:""; if(!macro.empty()&&macro.back()!='\n') macro+='\n';
	for( auto& it : shader_source_map )
	{
		auto v = gxExplodeShaderSource(it.second.c_str());
		std::vector<std::string> directive_list;	for(auto& j:v){ const char* s=str_replace(trim(j.second.c_str()),"\t"," "); if(strstr(s,"#version")||strstr(s,"#extension")){ directive_list.emplace_back(j.second); j.second.clear();} }
		std::vector<std::string> macro_list;		for(auto& j:explode_conservative(macro.c_str(),'\n')) macro_list.emplace_back(j);
		std::vector<std::string> layout_list;		if(it.first==GL_FRAGMENT_SHADER) layout_list.emplace_back("layout(pixel_center_integer) in vec4 gl_FragCoord;");

		// merge all together
		std::string& s = const_cast<std::string&>(it.second); s.clear();
		for( auto& j: directive_list ) s+=j+'\n';
		for( auto& j: layout_list ) s+=j+'\n';
		for( auto& j: macro_list ) s+=j+'\n';
		for( auto& j: v ) s+=j.second+'\n';
	}

	// 2. trivial return for the default program
	auto vit=shader_source_map.find(GL_VERTEX_SHADER);	bool vertex_shader_exists = vit!=shader_source_map.end()&&!vit->second.empty();
	auto cit=shader_source_map.find(GL_COMPUTE_SHADER);	bool compute_shader_exists = cit!=shader_source_map.end()&&!cit->second.empty();
	if(!vertex_shader_exists&&!compute_shader_exists) return new gl::Program(0,name);

	// 3. create md5 hash of shader souces
	std::string crcsrc=pname; for( auto& it : shader_source_map ) crcsrc += it.second;
	for( size_t k=0; tf_varyings&& k < tf_varyings->size(); k++ ) crcsrc += tf_varyings->at(k);
	uint crc = gl::crc32c(crcsrc.c_str(),crcsrc.length());

	// 4. try to load binary cache
	GLuint binary_program_ID = gxLoadProgramBinary( pname, crc );
	if(binary_program_ID&&gxCheckProgramLink(name, binary_program_ID,false)){ gl::Program* program = new gl::Program(binary_program_ID, name); program->update_uniform_cache(); program->update_uniform_block(); return program; }

	__int64 freq;	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	__int64 tbegin;	QueryPerformanceCounter((LARGE_INTEGER*)&tbegin);

	printf( "Compiling %s ... ", pname );

	// 5. create program first
	GLuint program_ID = glCreateProgram();
	gl::Program* program = new gl::Program( program_ID, name );

	// 6. compile and attach shaders
	std::vector<GLuint> attached_shaders;
	for( auto& it : shader_source_map )
	{
		GLuint shader_type = it.first;
		GLuint shader_ID = gxCompileShaderSource( shader_type, format("%s",pname), it.second.c_str() );

		if( !it.second.empty() && shader_ID==0 ) return nullptr;
		if( shader_ID ){ glAttachShader( program->ID, shader_ID ); attached_shaders.emplace_back( shader_ID ); }
		if( shader_type == GL_GEOMETRY_SHADER )
		{
			int maxGeometryOutputVertices;
			glGetIntegerv( GL_MAX_GEOMETRY_OUTPUT_VERTICES, &maxGeometryOutputVertices );
			glProgramParameteri( program->ID, GL_GEOMETRY_VERTICES_OUT, maxGeometryOutputVertices);
			glProgramParameteri( program->ID, GL_GEOMETRY_INPUT_TYPE, GL_TRIANGLES );
			glProgramParameteri( program->ID, GL_GEOMETRY_OUTPUT_TYPE, GL_TRIANGLES );
		}
	}

	// 7. specify the output varyings of transform feedback, if given
	if(tf_varyings&&!tf_varyings->empty()) glTransformFeedbackVaryings( program->ID, (GLsizei)tf_varyings->size(), &(*tf_varyings)[0], GL_INTERLEAVED_ATTRIBS );

	// 8. first linking of program
	glLinkProgram( program->ID );
	if(!gxCheckProgramLink( pname, program->ID ))
	{
		if(program){ delete program; program=nullptr; }
		return nullptr;
	}

	// 9. query the active attributes
	char aname[256] = {0};
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
			if(loc!=0&&(_stristr(aname,"pos")||_stristr(aname,"position")))						glBindAttribLocation( program->ID, 0, aname );
			if(loc!=1&&(_stristr(aname,"norm")||_stristr(aname,"nrm")))							glBindAttribLocation( program->ID, 1, aname );
			if(loc!=2&&(_stristr(aname,"tex")||_stristr(aname,"coord")||_stristr(aname,"tc")))	glBindAttribLocation( program->ID, 2, aname );
		}

		// link agin to reflect the changes in binding locations
		glLinkProgram( program->ID ); if(!gxCheckProgramLink( pname, program->ID )){ if(program){ delete program; program=nullptr; } return nullptr; }
	}

	// 10. update uniforms before validating the program
	program->update_uniform_cache(); // to avoid validation errors in Intel compilers
	program->update_uniform_block();

	// 11. validation: test if the program can run in the current state
	if(!gxValidateProgram( pname, program->ID )){ if(program){ delete program; program=nullptr; } return nullptr; }

	// 12. save cache
	glProgramParameteri( program->ID, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, GL_TRUE );
	gxSaveProgramBinary( pname, program->ID, crc );

	// 13. detach/delete shaders
	GLsizei shader_count; GLuint shaders[256];
	glGetAttachedShaders( program->ID,256,&shader_count,shaders);
	for(GLsizei k=0;k<shader_count;k++){ glDetachShader(program->ID,shaders[k]); glDeleteShader(shaders[k]); }

	// 14. logging
	__int64 tend; QueryPerformanceCounter((LARGE_INTEGER*)&tend);
	printf( "completed in %.1f ms\n", (float)(double(tend-tbegin)/double(freq)*1000.0) );

	return program;
}

inline gl::Program* gxCreateProgram( const char* prefix, const char* name, const char* vert_shader_source, const char* frag_shader_source, const char* p_macro )
{
	std::map<GLuint,std::string> shader_source_map;
	shader_source_map[ GL_VERTEX_SHADER ] = vert_shader_source?vert_shader_source:"";
	shader_source_map[ GL_FRAGMENT_SHADER ] = frag_shader_source?frag_shader_source:"";
	return gxCreateProgram( prefix, name, shader_source_map, p_macro, nullptr );
}

inline gl::Effect* gxCreateEffect( const char* name )
{
	return new gl::Effect(0, name);
}

//***********************************************
#ifdef GLFX_PARSER_IMPL
#pragma comment( lib, "glfx.lib" )
struct glfxParserImpl : public glfx::IParser
{
	struct program_t{std::string name;struct shader_t{uint type;std::string source;};std::vector<shader_t> shaders;};
	std::string				log;
	std::vector<program_t>	programs;

	virtual const char* parse_log(){ return log.c_str(); }
	virtual int program_count(){ return int(programs.size()); }
	virtual const char* program_name( int program_id ){ return program_id>=program_count()?"":programs[program_id].name.c_str(); }
	virtual int shader_count( int program_id ){ return program_id>=program_count()?0:int(programs[program_id].shaders.size()); }
	virtual unsigned shader_type( int program_id, int shader_id ){ return program_id>=program_count()?0:shader_id>=shader_count(program_id)?0:programs[program_id].shaders[shader_id].type; }
	virtual const char* shader_source( int program_id, int shader_id ){ return program_id>=program_count()?0:shader_id>=shader_count(program_id)?0:programs[program_id].shaders[shader_id].source.c_str(); }
	virtual bool parse( const char* str )
	{
		std::string src;for(auto* v:explode_conservative(str,'\n')){if(*v)src+=trim_comment(v,"//");src+='\n';} // bug fix for glfxfindblock with blockmarks inside the comments
		int id=glfxGenEffect();if(!glfxParseEffectFromMemory(id,src.c_str())){log=glfxGetEffectLog(id);return false;}
		for(int k=0,kn=glfxGetProgramCount(id);k<kn;k++)
		{
			programs.emplace_back(program_t());program_t& p=programs.back();p.name=glfxGetProgramName(id,k);
			std::map<unsigned int,std::string> glfxGetProgramSource(int,const char*);
			for(auto& it:glfxGetProgramSource(id,p.name.c_str()))p.shaders.emplace_back(program_t::shader_t{it.first,it.second});
		}
		glfxDeleteEffect(id);
		return true;
	}
};

inline glfx::IParser* glfxCreateParser(){ return new glfxParserImpl; }
inline void glfxDeleteParser( glfx::IParser** pp_parser ){ if(!pp_parser||!(*pp_parser)) return; delete *((glfxParserImpl**)pp_parser); *(pp_parser)=nullptr; }

#endif // GLFX_PARSER_IMPL
//***********************************************

inline gl::Effect* gxCreateEffect( const char* name, const char* effect_source, const char* p_macro=nullptr, gl::Effect* p_effect_to_append=nullptr )
{
#ifndef GLFX_PARSER_IMPL
	static glfx::IParser*(*glfxCreateParser)() = (glfx::IParser*(*)()) GetProcAddress(GetModuleHandleW(nullptr),"glfxCreateParser"); if(glfxCreateParser==nullptr){ printf( "%s(): unable to link to glfxCreateParser()\n", __func__ ); return nullptr; }
	static void(*glfxDeleteParser)(glfx::IParser**) = (void(*)(glfx::IParser**)) GetProcAddress(GetModuleHandleW(nullptr),"glfxDeleteParser"); if(glfxDeleteParser==nullptr){ printf( "%s(): unable to link to glfxDeleteParser()\n", __func__ ); return nullptr; }
#endif

	// preprocess source code
	std::string src; if(p_macro&&p_macro[0]) src=p_macro; if(!src.empty()&&src.back()!='\n') src+='\n'; src+=effect_source;

	// load shaders from effect
	glfx::IParser* parser = glfxCreateParser(); if(parser==nullptr){ printf( "%s(): unable to create parser()\n", __func__ ); return nullptr; }
	if(!parser->parse(src.c_str())){ printf( "%s(%s)\n%s\n", __func__, name, parser->parse_log() ); return nullptr; }

	gl::Effect* e = p_effect_to_append ? p_effect_to_append : new gl::Effect(0, name);
	for( int k=0, kn=parser->program_count(); k<kn; k++ )
	{
		std::map<uint,std::string> shader_map; for( int j=0; j<parser->shader_count(k); j++ ) shader_map[parser->shader_type(k,j)]=parser->shader_source(k,j);
		if(!e->create_program( name, parser->program_name(k), shader_map, nullptr )){ printf( "Unable to create %s.%s\n", name, parser->program_name(k) ); glfxDeleteParser(&parser); if(e!=p_effect_to_append) delete e; return nullptr; }
	}
	glfxDeleteParser(&parser);

	return e;
}

inline bool gl::Effect::attach( const char* name, const char* effect_source, const char* p_macro )
{
	return gxCreateEffect(name,effect_source,p_macro,this)==this;
}

inline gl::Framebuffer* gxCreateFramebuffer( const char* name=nullptr )
{
	if(!name){ printf( "%s(): name==nullptr\n", __func__ ); return nullptr; }
	GLuint ID=0; if(glCreateFramebuffers) glCreateFramebuffers( 1, &ID ); else glGenFramebuffers(1,&ID); if(ID==0){ printf( "Unable to create buffer[%s]", name ); return nullptr; }
	return new gl::Framebuffer(ID,name&&name[0]?name:"");	// if name is nullptr, return default FBO
}

inline gl::TransformFeedback* gxCreateTransformFeedback( const char* name )
{
	GLuint ID=0; if(glCreateTransformFeedbacks) glCreateTransformFeedbacks( 1, &ID ); else if(glGenTransformFeedbacks) glGenTransformFeedbacks(1,&ID);
	return (ID!=0||glIsTransformFeedback(ID)) ? new gl::TransformFeedback( ID, name ) : nullptr;
}

inline gl::Texture* gxCreateTexture1D( const char* name, GLint levels, GLsizei width, GLsizei layers=1, GLint internal_format=GL_RGBA16F, GLvoid* data=nullptr, bool force_array=false )
{
	if(layers>gxGetIntegerv( GL_MAX_ARRAY_TEXTURE_LAYERS )){ printf( "%s(): layer (=%d) > GL_MAX_ARRAY_TEXTURE_LAYERS (=%d)\n", __func__, layers, gxGetIntegerv( GL_MAX_ARRAY_TEXTURE_LAYERS ) ); return nullptr; }
	if(!gxIsSizedInternalFormat(internal_format)){ printf( "%s(): internal_format must use a sized format instead of GL_RED, GL_RG, GL_RGB, GL_RGBA.\n", __func__ ); return nullptr; }

	GLenum target = layers>1||force_array?GL_TEXTURE_1D_ARRAY:GL_TEXTURE_1D;
	GLuint ID = gxCreateTexture(target); if(ID==0) return nullptr;
	GLenum format = gxGetTextureFormat( internal_format );
	GLenum type = gxGetTextureType( internal_format );

	gl::Texture* texture = new gl::Texture(ID,name,target,internal_format,format,type); if(width==0) return texture;
	texture->key = gl::Texture::crc(0,levels,0,layers,false,target);
	glBindTexture( target, ID );

	// allocate storage
	if( target==GL_TEXTURE_1D ){	glTexStorage1D( target, levels, internal_format, width );			if(data) glTexSubImage1D( target, 0, 0, width, format, type, data ); }
	else {							glTexStorage2D( target, levels, internal_format, width, layers );	if(data) glTexSubImage2D( target, 0, 0, 0, width, layers, format, type, data ); }

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

inline gl::Texture* gxCreateTexture2D( const char* name, GLint levels, GLsizei width, GLsizei height, GLsizei layers=1, GLint internal_format=GL_RGBA16F, GLvoid* data=nullptr, bool force_array=false, bool multisample=false, GLsizei multisamples=4 )
{
	if(layers>gxGetIntegerv( GL_MAX_ARRAY_TEXTURE_LAYERS )){ printf( "%s: layer (=%d) > GL_MAX_ARRAY_TEXTURE_LAYERS (=%d)\n", __func__, layers, gxGetIntegerv( GL_MAX_ARRAY_TEXTURE_LAYERS ) ); return nullptr; }
	if(!gxIsSizedInternalFormat(internal_format)){ printf( "%s(): internal_format must use a sized format instead of GL_RED, GL_RG, GL_RGB, GL_RGBA.\n", __func__ ); return nullptr; }

	GLenum target = layers>1||force_array?(multisample?GL_TEXTURE_2D_MULTISAMPLE_ARRAY:GL_TEXTURE_2D_ARRAY):(multisample?GL_TEXTURE_2D_MULTISAMPLE:GL_TEXTURE_2D);
	GLuint ID = gxCreateTexture(target); if(ID==0) return nullptr;
	GLenum format = gxGetTextureFormat( internal_format );
	GLenum type = gxGetTextureType( internal_format );

	gl::Texture* texture = new gl::Texture(ID,name,target,internal_format,format,type); if(width==0||height==0) return texture;
	texture->key = gl::Texture::crc(0,levels,0,layers,false,target);
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

inline gl::Texture* gxCreateTexture3D( const char* name, GLint levels, GLsizei width, GLsizei height, GLsizei depth, GLint internal_format=GL_RGBA16F, GLvoid* data=nullptr )
{
	if(!gxIsSizedInternalFormat(internal_format)){ printf( "%s(): internal_format must use a sized format instead of GL_RED, GL_RG, GL_RGB, GL_RGBA.\n", __func__ ); return nullptr; }

	GLenum target = GL_TEXTURE_3D;
	GLuint ID = gxCreateTexture(target); if(ID==0) return nullptr;
	GLenum format = gxGetTextureFormat( internal_format );
	GLenum type = gxGetTextureType( internal_format );

	gl::Texture* texture = new gl::Texture(ID,name,target,internal_format,format,type); if(width==0||height==0||depth==0) return texture;
	texture->key = gl::Texture::crc(0,levels,0,depth,false,target);
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

inline gl::Texture* gxCreateTextureCube( const char* name, GLint levels, GLsizei width, GLsizei height, GLsizei count=1, GLint internal_format=GL_RGBA16F, GLvoid* data[6]=nullptr, bool force_array=false )
{
	if(count*6>gxGetIntegerv( GL_MAX_ARRAY_TEXTURE_LAYERS )){ printf( "%s(): count*6 (=%d) > GL_MAX_ARRAY_TEXTURE_LAYERS (=%d)\n", __func__, count*6, gxGetIntegerv( GL_MAX_ARRAY_TEXTURE_LAYERS ) ); return nullptr; }
	if(!gxIsSizedInternalFormat(internal_format)){ printf( "%s(): internal_format must use a sized format instead of GL_RED, GL_RG, GL_RGB, GL_RGBA.\n", __func__ ); return nullptr; }
	if(width==0||height==0){ printf( "%s(%s): width==0 or height==0", __func__, name ); return nullptr; }

	GLenum target = count>1||force_array?GL_TEXTURE_CUBE_MAP_ARRAY:GL_TEXTURE_CUBE_MAP;
	GLuint ID = gxCreateTexture(target); if(ID==0) return nullptr;
	GLenum format = gxGetTextureFormat( internal_format );
	GLenum type = gxGetTextureType( internal_format );

	gl::Texture* texture = new gl::Texture(ID,name,target,internal_format,format,type);
	texture->key = gl::Texture::crc(0,levels,0,count,false,target);

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

inline gl::Texture* gxCreateTextureBuffer( const char* name, gl::Buffer* buffer, GLint internal_format=GL_RGBA16F )
{
	if(!gxIsSizedInternalFormat(internal_format)){ printf( "%s(): internal_format must use a sized format instead of GL_RED, GL_RG, GL_RGB, GL_RGBA.\n", __func__ ); return nullptr; }

	GLenum target = GL_TEXTURE_BUFFER;
	GLuint ID = gxCreateTexture(target); if(ID==0) return nullptr;
	GLenum format = gxGetTextureFormat( internal_format );
	GLenum type = gxGetTextureType( internal_format );

	// create a texture
	gl::Texture* texture = new gl::Texture(ID,name,target,internal_format,format,type);
	texture->key = gl::Texture::crc(0,1,0,1,false,target);
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

inline gl::Texture* gxCreateTextureRectangle( const char* name, GLsizei width, GLsizei height, GLint internal_format=GL_RGBA16F, GLvoid* data=nullptr )
{
	if(!gxIsSizedInternalFormat(internal_format)){ printf( "%s(): internal_format must use a sized format instead of GL_RED, GL_RG, GL_RGB, GL_RGBA.\n", __func__ ); return nullptr; }

	GLenum target = GL_TEXTURE_RECTANGLE;
	GLuint ID = gxCreateTexture(target); if(ID==0) return nullptr;
	GLenum format = gxGetTextureFormat( internal_format );
	GLenum type = gxGetTextureType( internal_format );

	gl::Texture* texture = new gl::Texture(ID,name,target,internal_format,format,type); if(width==0||height==0) return texture;
	texture->key = gl::Texture::crc(0,1,0,1,false,target);
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

inline gl::Texture* gxCreateTextureView( gl::Texture* src, GLuint min_level, GLuint levels, GLuint min_layer=0, GLuint layers=1, bool force_array=false, GLenum target=0 )
{
	uint key = gl::Texture::crc(min_level,levels,min_layer,layers,force_array,target);
	for(gl::Texture* t=src; t; t=t->next ) if(t->key==key) return t;

	if(src->target==GL_TEXTURE_BUFFER){ printf( "%s->view should not be a texture buffer\n", src->name ); return nullptr; }
	if(levels==0){ printf( "%s->view should have more than one levels\n", src->name ); return nullptr; }
	if(layers==0){ printf( "%s->view should have more than one layers\n", src->name ); return nullptr; }
	if((min_level+levels)>GLuint(src->mip_levels())){ printf( "%s->view should have less than %d levels\n", src->name, src->mip_levels() ); return nullptr; }
	if((min_layer+layers)>GLuint(src->layers())){ printf( "%s->view should have less than %d layers\n", src->name, src->layers() ); return nullptr; }
	if(!src->is_immutable()){ printf("!%s->is_immutable()\n", src->name ); return nullptr; }

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
	const char* name1 = b_slice0?format("%s[%d]",src->name,min_layer):format("%s[%d:%d][%d:%d]",src->name,min_layer,layers-1,min_level,levels-1);

	// get attributes
	GLint internal_format	= src->internal_format();
	GLint wrap				= src->get_texture_parameteriv( GL_TEXTURE_WRAP_S );
	GLint min_filter		= src->get_texture_parameteriv( GL_TEXTURE_MIN_FILTER );
	GLint mag_filter		= src->get_texture_parameteriv( GL_TEXTURE_MAG_FILTER );

	// allocate the new texture using the initial crt heap
	gl::Texture* t1 = (gl::Texture*) HeapAlloc((void*)src->crtheap,0,sizeof(gl::Texture));
	new(t1) gl::Texture(ID1,name1,target,internal_format,src->format(),src->type(),src->crtheap);
	t1->key = key;

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
	t1->_width	= src->_width;
	t1->_height	= src->_height;
	t1->_depth	= layers;
	t1->_levels	= levels;

	// filter should be set after dimensions
	t1->set_filter( min_filter, mag_filter );
	t1->set_wrap( wrap );

	// add to linked list for search
	for( gl::Texture* t=src; t; t=t->next ) if(t->next==nullptr){ t->next=t1; break; }
	return t1;
}

#endif // __GX_OPENGL_H__
