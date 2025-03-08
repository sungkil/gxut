#ifndef __GX_STBIMAGE__
#define __GX_STBIMAGE__

#if __has_include("gxlib.h")
	#include "gxlib.h"
#elif __has_include("../gxlib.h")
	#include "../gxlib.h"
#elif __has_include(<gxut/gxlib.h>)
	#include <gxut/gxlib.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#if __has_include("stb_image.h")
	#include "stb_image.h"
#elif __has_include(<stb_image.h>)
	#include <stb_image.h>
#elif __has_include(<stb/stb_image.h>)
	#include <stb/stb_image.h>
#endif

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

inline image* create_image_header( int width, int height, int depth, int channels, unsigned align=4, void* data=nullptr )
{
	image* i = new image();
	i->width = width;
	i->height = height;
	i->depth = depth;
	i->channels = channels;
	const_cast<unsigned&>(i->align) = align;
	i->data = (decltype(i->data)) data;
	return i;
}

inline image* create_image( int width, int height, int depth, int channels, unsigned align=4 )
{
	image* i = create_image_header( width, height, depth, channels, align, nullptr );
	i->data = (unsigned char*) malloc(i->size());
	return i;
}

inline image* load_image( const char* file_path, bool vflip=true, bool force_rgb=true )
{
	if(access(file_path,0)!=0){ printf("%s(): %s not exists\n", __func__, file_path); return nullptr; }

	int width, height, channels; // comp returns the original channels
	stbi_uc* data = stbi_load( file_path, &width, &height, &channels, force_rgb?3:0); if(!data) return nullptr;
	
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
