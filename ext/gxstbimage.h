#ifndef __GX_STBIMAGE__
#define __GX_STBIMAGE__

#if __has_include("gxut.h")
	#include "gxut.h"
#elif __has_include("../gxut.h")
	#include "../gxut.h"
#elif __has_include(<gxut/gxut.h>)
	#include <gxut/gxut.h>
#endif

#include <stdio.h>
#include <stdlib.h>

// auto include stb_image.h
#if __has_include("stb_image.h")
	#include "stb_image.h"
#elif __has_include("../stb/stb_image.h")
	#include "../stb/stb_image.h"
#elif __has_include("../../stb/stb_image.h")
	#include "../../stb/stb_image.h"
#elif __has_include(<stb/stb_image.h>)
	#include <stb/stb_image.h>
#endif

// auto include stb_image_writer.h
#if __has_include("stb_image_write.h")
	#include "stb_image_write.h"
#elif __has_include("../stb/stb_image_write.h")
	#include "../stb/stb_image_write.h"
#elif __has_include("../../stb/stb_image_write.h")
	#include "../../stb/stb_image_write.h"
#elif __has_include(<stb/stb_image_write.h>)
	#include <stb/stb_image_write.h>
#endif

//*************************************
namespace stb {
//*************************************

__noinline image* load_image( path file_path, bool vflip=true, bool force_rgb=true )
{
	if(!file_path.exists()){ printf( "%s(): %s not exists\n", __func__, file_path.c_str() ); return nullptr; }

	int w, h, c; // comp returns the original channels
	stbi_set_flip_vertically_on_load(vflip?1:0);
	stbi_uc* data = stbi_load( file_path.c_str(), &w, &h, &c, force_rgb?3:0); if(!data) return nullptr;
	
	image* i = new image_header(w,h,8,c);
	if(i->stride()==(w*c)) // already 4-byte alinged
	{
		i->data = data; 
	}
	else // apply 4-byte alignment
	{
		i->data = (uchar*)malloc(i->size());
		for( int y=0, c=i->channels; y<h; y++ ) memcpy( i->ptr<uchar>(y), data+y*w*c, w*c );
		stbi_image_free(data);
	}
	
	return i;
}

__noinline bool save_image( image* i, path file_path, bool vflip=true, bool rgb_to_bgr=false, int jpeg_quality=95 )
{
	if(!i||!i->data) return false;
	
	// save only jpeg or png
	struct { bool jpg=false, png=false; } b;
	if(const auto e=file_path.extension(); e=="jpg"||e=="jpeg") b.jpg=true; else if(e=="png") b.png=true;
	else { printf("%s(): unsupported extension (%s)\n", __func__, e.c_str() ); return false; }
	
	int w=i->width, h=i->height, c=i->channels, sr=int(i->stride()), dr=w*c;
	struct { uchar* data; int stride; } output = {i->data,sr};
	
	if((b.jpg&&sr>dr)||rgb_to_bgr)
	{
		static uchar* t=nullptr; static size_t tz=0; if(tz<sr*h) t=realloc<uchar>(t,tz=sr*h);
		for( int y=0; y<h; y++ )
		{
			uchar *s=i->ptr<uchar>(y), *d=t+dr*y;
			if(!rgb_to_bgr) memcpy(d,s,dr);
			else for( int x=0; x<w; x++, s+=c, d+=c ){ d[0]=s[2]; if(c==1) continue; d[1]=s[1]; d[2]=s[0]; if(c==3) continue; d[3]=s[3]; }
		}
		output = { t, dr };
	}

	if(!file_path.dir().exists()) file_path.dir().mkdir();
	stbi_flip_vertically_on_write(vflip?1:0);
	if(b.jpg){		if(!stbi_write_jpg( file_path.c_str(), w, h, c, output.data, jpeg_quality )) return false; }
	else if(b.png){	if(!stbi_write_png( file_path.c_str(), w, h, c, output.data, output.stride )) return false; }
	if(!file_path.exists()){ printf( "%s(): failed to write %s\n", __func__, file_path.relative().to_slash().c_str() ); return false; }
	return true;
}

//*************************************
} // namespace gx

#endif // __GX_STBIMAGE__
