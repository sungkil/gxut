#ifndef __GX_STBIMAGE__
#define __GX_STBIMAGE__

#include <stdio.h>
#include <stdlib.h>

#if defined(__has_include)
	#if __has_include("stb_image.h")
		#include "stb_image.h"
	#elif __has_include(<stb_image.h>)
		#include <stb_image.h>
	#elif __has_include(<stb/stb_image.h>)
		#include <stb/stb_image.h>
	#endif
#endif

#ifndef __GX_IMAGE_DECL__
#define __GX_IMAGE_DECL__
struct image
{
	unsigned char*	data;
	unsigned int	width;
	unsigned int	height;
	unsigned int	depth;					// should be one of 8=IPL_DEPTH_8U, and 32=IPL_DEPTH_32F
	unsigned int	channels;				// should be one of 1, 2, 3, and 4
	unsigned int	space;					// color space: accepts only RGB, YUY2, YV12
	union { unsigned int crc; int index; };	// user can fill crc or other attributes; not used by default

	inline unsigned int stride() const { unsigned int bpp=space==YUY2?2:space==YV12?1:channels; return ((depth>>3)*bpp*width+3)&(~3); }	// 4-byte aligned stride for the first plane; YUY2/YV12 uses 2/1 bytes for the first plane
	inline unsigned int size() const { return space==YV12?stride()*3/2:stride()*height; }
	template <class T> inline T* ptr( int y=0, int x=0, bool vflip=false ){ return ((T*)(data+stride()*(vflip?height-1-y:y)))+x; }
	template <class T> inline const T* ptr ( int y=0, int x=0, bool vflip=false ) const { return ((T*)(data+stride()*(vflip?height-1-y:y)))+x; }
	template <class T> inline T* plane( int z=0 ){ if(space!=YV12) return data; else return data+(z==1?stride()*height:z==2?stride()*height*5/4:0); }
	template <class T> inline const T* plane( int z=0 ) const { if(space!=YV12) return data; else return data+(z==1?stride()*height:z==2?stride()*height*5/4:0); }

	enum color_space { RGB=0, YUY2=0x32595559, YUYV=0x56595559, YV12=0x32315659, I420=0x30323449, IYUV=0x56555949, NV12=0x3231564E }; // fourcc; duplicats: YUY2==YUYV, I420==IYUV
};
#endif // __GX_IMAGE_DECL__

//*************************************
namespace gx {
//*************************************

inline void release_image( image** pp_image )
{
	if(!pp_image||!*pp_image) return;
	if((*pp_image)->data) free((*pp_image)->data);
	delete (*pp_image);
	*pp_image = nullptr;
}

inline image* create_image_header( int width, int height, int depth, int channels, void* data=nullptr )
{
	image* i = new image();
	i->width = width;
	i->height = height;
	i->depth = depth;
	i->channels = channels;
	i->data = (decltype(i->data)) data;
	return i;
}

inline image* create_image( int width, int height, int depth, int channels )
{
	image* i = create_image_header( width, height, depth, channels );
	i->data = (unsigned char*) malloc(i->size());
	return i;
}

inline image* load_image( const wchar_t* filepath, bool vflip=true, bool force_rgb=true )
{
	path file_path = path(filepath).absolute(); 
	if(!file_path.exists()){ printf( "%s(): %s not exists\n", __func__, wtoa(filepath) ); return nullptr; }

	int width, height, channels; // comp returns the original channels
	stbi_uc* data = stbi_load( wtoa(file_path), &width, &height, &channels, force_rgb?3:0 ); if(!data) return nullptr;
	
	// apply 4-byte alignment
	image* i = gx::create_image(width,height,8,force_rgb?3:channels);
	for( int y=0, yn=i->height, c=i->channels; y<yn; y++ )
		memcpy( i->ptr<uchar>(vflip?yn-1-y:y), data+y*width*c, width*c );

	// release temporary memory
	stbi_image_free(data);
	
	return i;
}

//*************************************
} // namespace gx

#endif // __GX_STBIMAGE__
