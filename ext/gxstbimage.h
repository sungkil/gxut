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
	unsigned int	depth;		// should be one of 8=IPL_DEPTH_8U, and 32=IPL_DEPTH_32F
	unsigned int	channels;	// should be one of 1, 2, 3, and 4
	unsigned int	space=0;	// color space: accepts only RGB, YUY2, YV12
	unsigned int	crc=0;		// image data crc
	int				index;		// signed image index
	const unsigned	align=4;	// byte alignment for rows; can be overriden for YUV (e.g., 64)

	inline unsigned int stride( int channel=0 ) const { bool i420=space==I420||space==YV12||space==IYUV; unsigned int bpp=(space==YUY2)?2:(i420||space==NV12)?1:channels; uint r=(depth>>3)*bpp*width; if(align<2) return r; return (((i420&&channel)?(r>>1):r)+align-1)&(~(align-1)); }
	inline unsigned int size() const { bool i420=space==I420||space==YV12||space==IYUV; return height*(i420?(stride()+stride(1)):space==NV12?(stride()+stride(1)/2):stride()); }
	template <class T> inline T* ptr( int y=0, int x=0, bool vflip=false ){ return ((T*)(data+stride()*(vflip?height-1-y:y)))+x; } // works only for RGB
	template <class T> inline T* plane( int channel=0 ){ unsigned char* p=data; int c=channel; if(c){ p+=stride()*height; if((space==I420||space==YV12||space==IYUV)&&c>1) p+=stride(1)*height/2; } return (T*)p; }

	// fourcc; YUY2==YUYV, I420==YU12==IYUV (YUV420P), YV12 (YVU420P), NV12 (YUV420SP)
	enum color_space { RGB=0, YUY2='2yuy', YUYV='vyuy', I420='024i', YU12=I420, IYUV='vuyi', YV12='21vy', NV12='21vn' };
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
