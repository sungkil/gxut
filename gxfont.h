#pragma once
#ifndef __GXFONT_H__
#define __GXFONT_H__

#if defined(__has_include)&&__has_include(<gxut/gxrex.h>)
#include <gxut/gxrex.h> // rex image functions
#endif

//*************************************
namespace gx {
//*************************************

struct font_engine
{
	static const int N0=32;			// beginning of table in ASCII code
	static const int NX=96;			// number of characters in a row
	const int TX=0, TY=0;			// size of a single unicode char
	const bool vflip_table=false;	// true/false for OpenGL/Direct3D
	struct char_t { uint index:30, wide:2; };

	HDC						hDC=nullptr;
	HFONT					hFont=nullptr;
	HBITMAP					bitmap=nullptr;
	BITMAPINFO*				bitmap_info=nullptr;
	image*					buffer=nullptr;	// RGB buffer for color and shadows
	image*					table=nullptr;	// font raster images
	uchar4					text_color={255,255,0,255};
	uchar4					shadow_color={64,64,64,255};
	std::map<ushort,char_t>	char_map;		// LUT table: ANSI and then UTF8 >> (index,wide)
	int						dpi=96;			// 96 for normal case

	font_engine( const wchar_t* face, int point_size, uchar3 color={255,255,0}, uchar3 shadow={64,64,64}, bool vflip=false );
	font_engine( image* table_path, int ty, int row=0, bool vflip=false );
	~font_engine();

	image*	resize_table( int count );
	void	save_table( const wchar_t* file_path ){ if(table) rex::save_image( file_path, table ); }
	bool	update( const wchar_t* str );	// return true when the table is dirty
	void	begin_raster( HBITMAP* phBitmap0, HFONT* phFont0 );
	void	end_raster( HBITMAP hBitmap0, HFONT hFont0 );
	bool	raster( wchar_t c );			// return wide or single
	ivec4	char_pos( wchar_t c );			// (first_x,first_y,second_x,second_y): second is only for wide characters
	int		line_height(){ const int DY=((TY*96/dpi)>8?-4:2)*dpi/96; return TY+DY; }
	bool	is_wide( wchar_t c ){ auto it=char_map.find(c); return it==char_map.end()?false:(it->second.wide>0); }
	std::vector<ivec4>& get_locations( const wchar_t* text );
};

inline BITMAPINFO* create_font_bitmap_info( int width, int height, int channels )
{
	BITMAPINFO* bmp_info = (BITMAPINFO*)malloc(sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)*256);
	BITMAPINFOHEADER* bmih = &(bmp_info->bmiHeader);
	memset(bmih,0,sizeof(*bmih));
	bmih->biSize = sizeof(BITMAPINFOHEADER);
	bmih->biPlanes = 1;
	bmih->biCompression = BI_RGB;
	bmih->biBitCount = 8*channels;
	bmih->biWidth = width;
	bmih->biHeight = height; // bottom up
	RGBQUAD* palette = bmp_info->bmiColors;
 	for(int i=0;i<256;i++){ palette[i].rgbBlue=palette[i].rgbGreen=palette[i].rgbRed=BYTE(i); palette[i].rgbReserved=0; }
	return bmp_info;
}

inline font_engine::~font_engine()
{
	if(bitmap_info)	free(bitmap_info);
	if(bitmap)		DeleteObject(bitmap);
	if(hFont)		DeleteObject(hFont);
	if(hDC)			DeleteDC(hDC);
	if(buffer)		rex::release_image(&buffer);
	if(table)		rex::release_image(&table);
}

inline font_engine::font_engine( const wchar_t* face, int point_size, uchar3 color, uchar3 shadow, bool vflip ):vflip_table(vflip)
{
	if(!(hDC=CreateCompatibleDC(nullptr))){ printf( "font_engine: hDC==nullptr\n" ); return; }
	
	// monochrome memory DC 
	dpi = GetDeviceCaps(hDC,LOGPIXELSY);
	POINT pt={0,dpi*point_size/72};DPtoLP(hDC,&pt,1);

	const_cast<int&>(TY)=pt.y;
	const_cast<int&>(TX)=TY/2;

	if(hDC) bitmap = CreateCompatibleBitmap(hDC,TY,TY); // compatible (monochrome) bitmap
	LOGFONT lf={}; lf.lfHeight=-TY; lf.lfCharSet=DEFAULT_CHARSET;wcscpy(lf.lfFaceName,face);hFont=CreateFontIndirectW(&lf);
	buffer		= rex::create_image(TY,TY,8,4);
	bitmap_info	= create_font_bitmap_info(TY,TY,1);

	text_color = uchar4{color.x,color.y,color.z,255};
	shadow_color = uchar4{shadow.x,shadow.y,shadow.z,255};

	// create an initial LUT table image for ANSI chars. In the beginning, 2 rows are created
	wchar_t ascii[129-N0]={}; for(wchar_t k=0,kn=_countof(ascii)-1;k<kn; k++) ascii[k]=k+N0; update( ascii );
}

inline font_engine::font_engine( image* table0, int ty, int row, bool vflip ):vflip_table(vflip)
{
	if(!(hDC=CreateCompatibleDC(nullptr))){ MessageBoxW( 0, L"FontEngine: dc==nullptr", L"Error", MB_OK ); return; }
	dpi = GetDeviceCaps(hDC,LOGPIXELSY); DeleteDC(hDC); hDC=nullptr; // remove DC after getting dpi

	const_cast<int&>(TY)=ty*dpi/96;
	const_cast<int&>(TX)=TY/2;

	int count = table0->width/ty; for(int k=0;k<count;k++) char_map[N0+k].index=k;	// update char map

	resize_table( count );
	int w=table->width, h=table->height;
	bool b_wide = (w/ty)>64;
	for( int y=0; y<h; y++ )
	{
		uchar4* d = table->ptr<uchar4>(y,0,vflip_table);
		for( int x=0; x<w; x++, d++ )
		{
			uchar3 s = *table0->ptr<uchar3>((row*h+y)*ty/TY,x*ty/TY);
			uchar r=s.x, g=s.y, b=s.z;
			*d = (r==255&&g==255&&b==255)?uchar4{0,0,0,0}:uchar4{r,g,b,uchar(r||g||b?255:0)};
			char_map[N0+x/ty].wide=b_wide;
		}
	}
}

inline image* font_engine::resize_table( int count )
{
	ivec2 size = ivec2(TY,TY)*ivec2(NX,(int(count)+NX-1)/NX);
	if(table&&table->height>=uint(size.y)) return table;
	image* t = rex::create_image( size.x, size.y, 8, 4 ); if(!t) return nullptr;
	memset(t->data,0,t->size()); if(table){memcpy(t->data,table->data,table->size());rex::release_image(&table);}
	return table = t;
}

inline void font_engine::begin_raster( HBITMAP* phBitmap0, HFONT* phFont0 )
{
	HBITMAP hBitmap0 = (HBITMAP)SelectObject(hDC,bitmap); if(phBitmap0) *phBitmap0=hBitmap0;
	HFONT hFont0 = (HFONT)SelectObject(hDC,hFont); if(phFont0) *phFont0=hFont0;
	SetBkColor(hDC,RGB(0,0,0));
	SetTextColor(hDC,RGB(255,255,255));
}

inline void font_engine::end_raster( HBITMAP hBitmap0, HFONT hFont0 )
{
	if(hFont0) SelectObject(hDC,hFont0);
	if(hBitmap0) SelectObject(hDC,hBitmap0);
}

inline bool font_engine::raster( wchar_t c )
{
	const int sof=std::min(2,std::max(1,(dpi+48)/96)); // shadow offsets

	int w=buffer->width, hw=w/2, h=buffer->height;
	static std::vector<image*> bmps;
	if(bmps.empty()||(bmps.front()->width*bmps.front()->height)!=w*h)
	{
		for(auto* b:bmps)			rex::release_image(&b); bmps.clear();
		for(int k=0;k<sof+1;k++)	bmps.emplace_back(rex::create_image(w,h,8,1));
	}

	// clear buffer
	memset(buffer->data,0,buffer->size());
	if(c==L' ') return true; // trivial handling of space

	RECT rc0={0,0,w,h};
	wchar_t buff[] = {c,L' ',L' ',L' ',0};
	DrawTextW( hDC, L"       ", -1, &rc0, DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_NOCLIP );
	for( int k=0, kn=int(bmps.size()), x=-kn/2, y=x-1; k<kn; k++, x++, y++ )
	{
		auto* b = bmps[k];
		RECT rc={x,y,x+w,y+h};
		DrawTextW( hDC, buff, -1, &rc, DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_NOCLIP );
		GetDIBits( hDC, bitmap, 0, h, b->data, bitmap_info, DIB_RGB_COLORS );
	}
	
	bool wide=false;
	for(int y=0, kn=int(bmps.size());y<h;y++)
	{
		uchar4*	d = buffer->ptr<uchar4>(y,0);
		for(int x=0;x<w;x++,d++)
		{
			for( int k=0; k<kn; k++ )
			{
				uchar* s = bmps[k]->ptr<uchar>(y,x); if(*s==0) continue;
				*d=k?shadow_color:text_color;
				if(k==0&&x>=hw) wide=true;
				break;
			}
		}
	}

	return wide;
}

inline bool font_engine::update( const wchar_t* str )
{
	if(!hDC||!buffer) return false; // using existing table

	// find dirty characters and trivial return
	static std::vector<wchar_t> dirty_chars; dirty_chars.clear();
	for(const wchar_t* p=str;*p;p++) if(char_map.find(*p)==char_map.end()) dirty_chars.emplace_back(*p);
	if(dirty_chars.empty()) return false; // nothing to update

	int w=buffer->width, h=buffer->height;
	HBITMAP hBitmap0=nullptr; HFONT hFont0=nullptr;
	begin_raster( &hBitmap0, &hFont0 );
	for( auto c : dirty_chars )
	{
		char_map[c].index = uint(char_map.size());
		char_map[c].wide = raster(c);
		resize_table( int(char_map.size()) );
		for( int y=0,t=char_map[c].index,x1=(t%NX)*w,y1=(t/NX)*h;y<h;y++)
			memcpy( table->ptr<uchar4>(y+y1,x1,vflip_table), buffer->ptr<uchar4>(y,0,false), sizeof(uchar4)*w );
	}
	end_raster( hBitmap0, hFont0 );

	return true;
}

inline ivec4 font_engine::char_pos( wchar_t c )
{
	auto it=char_map.find(c); if(it==char_map.end()) return ivec4(-1,-1,-1,-1);
	int t=it->second.index;
	ivec4 p={}; p.xy=ivec2(t%NX,t/NX)*TY; p.zw=(it->second.wide)?ivec2(p.x+TX,p.y):ivec2(-1,-1);
	return p;
}

inline std::vector<ivec4>& font_engine::get_locations( const wchar_t* text )
{
	static std::vector<ivec4> buffer; buffer.clear();
	for( uint k=0, kn=uint(wcslen(text)); k<kn; k++ )
	{
		ivec4 cpos = char_pos(text[k]);
		buffer.emplace_back(ivec4(cpos.xy,0,0));
		if(cpos.z>-1) buffer.emplace_back(ivec4(cpos.zw,0,0));
	}
	return buffer;
}

//*************************************
} // namespace gx
//*************************************

#endif // __GXFONT_H__