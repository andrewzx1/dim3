/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Text to Textures

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
application screen and in a single piece of the documentation
3. It is not resold, in it's current form or modified, as an
engine-only product

This code is presented as is. The author of dim3 takes no
responsibilities for any version of this code.

Any non-engine product (games, etc) created with this code is free
from any and all payment and/or royalties to the author of dim3,
and can be sold or given away.

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3baseutility.h"
#endif

#ifdef D3_OS_LINUX
 #include <ft2build.h>
 #include FT_FREETYPE_H
 #include <fontconfig/fontconfig.h>
#endif

#define font_bitmap_pixel_sz					512

/* =======================================================

      Create Font Texture OS X
      
======================================================= */

#ifdef D3_OS_MAC

void bitmap_text_size_internal(texture_font_size_type *d3_size_font,char *name,int size)
{
	int					n,x,y,data_sz,row_add;
	char				ch;
	unsigned char		*bm_data,*txt_data,*sptr,*dptr;
	CGPoint				txt_pt;
	CGContextRef		bitmap_ctx;
	CGColorSpaceRef		color_space;
	CGAffineTransform	trans_flip;
	
		// data for bitmap

	row_add=font_bitmap_pixel_sz*4;
	data_sz=row_add*font_bitmap_pixel_sz;
	
	bm_data=malloc(data_sz);
	if (bm_data==NULL) return;
	
	bzero(bm_data,data_sz);
	
		// create bitmap context
		
	color_space=CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
	bitmap_ctx=CGBitmapContextCreate(bm_data,font_bitmap_pixel_sz,font_bitmap_pixel_sz,8,row_add,color_space,kCGImageAlphaPremultipliedLast);
	CGColorSpaceRelease(color_space);
	
	if (bitmap_ctx==NULL) {
		free(bm_data);
		return;
	}
	
	CGContextTranslateCTM(bitmap_ctx,0,font_bitmap_pixel_sz);
    CGContextScaleCTM(bitmap_ctx,1.0f,-1.0f);
	
	CGContextClear(bitmap_ctx,CGRectMake(0,0,font_bitmap_pixel_sz,font_bitmap_pixel_sz));
 
	CGContextSetAlpha(bitmap_ctx,1.0f);
	CGContextSetBlendMode(bitmap_ctx,kCGBlendModeNormal);

		// setup the correct font

	CGContextSelectFont(bitmap_ctx,name,size,kCGEncodingMacRoman);
	
		// setup text drawing
		
	CGContextSetTextDrawingMode(bitmap_ctx,kCGTextFill);
	CGContextSetRGBFillColor(bitmap_ctx,1.0f,1.0f,1.0f,1.0f);
	CGContextSetRGBStrokeColor(bitmap_ctx,1.0f,1.0f,1.0f,1.0f);

	trans_flip=CGAffineTransformMake(1.0f,0.0f,0.0f,-1.0f,0.0f,0.0f);
	CGContextSetTextMatrix(bitmap_ctx,trans_flip);
		
		// draw the characters

	for (n=0;n!=90;n++) {
	
			// draw the character

		ch=(n+'!');

		x=(n%d3_size_font->char_per_line)*d3_size_font->char_wid;
		y=((n/d3_size_font->char_per_line)*d3_size_font->char_high)+d3_size_font->char_baseline;

		CGContextShowTextAtPoint(bitmap_ctx,x,y,&ch,1);

			// get the spacing information
			
		txt_pt=CGContextGetTextPosition(bitmap_ctx);

		d3_size_font->char_size[n]=(txt_pt.x-(float)x)/(float)d3_size_font->char_wid;
	}

		// texture data
		
	txt_data=malloc(data_sz);
	if (txt_data==NULL) {
		CGContextRelease(bitmap_ctx);
		free(bm_data);
		return;
	}
	
		// create the texture
		// note the coordinate systems are flipped

	dptr=txt_data;
	sptr=bm_data;

	for (y=0;y!=font_bitmap_pixel_sz;y++) {

		for (x=0;x!=font_bitmap_pixel_sz;x++) {

			*dptr++=0xFF;
			*dptr++=0xFF;
			*dptr++=0xFF;
			*dptr++=*sptr;		// use the anti-aliased font as the alpha mask (using the red component)
			
			sptr+=4;

		}
	}
	
	bitmap_data(&d3_size_font->bitmap,txt_data,font_bitmap_pixel_sz,font_bitmap_pixel_sz,TRUE,anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE);

	free(txt_data);
	
		// dispose the bitmap context
		
	CGContextRelease(bitmap_ctx);
	free(bm_data);
}

#endif

/* =======================================================

      Create Font Texture Linux
      
======================================================= */

#ifdef D3_OS_LINUX

// code from cyst, additional edit by brian

void bitmap_text_size_internal(texture_font_size_type *d3_size_font,char *name,int size)
{
	int				n, x, y, font_x, font_y, txt_x, txt_y, error;
	unsigned char 	*data, *ptr;
	FcPattern 		*pat, *match;
	FcResult 		fc_result;
	char 			*font_filename;
	FT_Library 		font_library;
	FT_Face 		face;
	FT_Bitmap		bitmap;
	unsigned char	ch;
	
	data = malloc((font_bitmap_pixel_sz<<2)*font_bitmap_pixel_sz);
	if (data==NULL) return;

	// find the font
	pat = FcPatternCreate();
	FcPatternAddString(pat, FC_FAMILY, name);
	FcConfigSubstitute(NULL, pat, FcMatchPattern);
	FcDefaultSubstitute(pat);
	match = FcFontMatch(NULL, pat, &fc_result);
	FcPatternGetString(match, FC_FILE, 0, (FcChar8 **)&font_filename);

	FT_Init_FreeType(&font_library);
	error = FT_New_Face(font_library,font_filename,0,&face);
	if (error) {
		return;
	}

	error = FT_Set_Char_Size(face,0,(size<<6),0,0);
	
	FcPatternDestroy(match);
	FcPatternDestroy(pat);
	
	for (n=0;n!=90;n++) {
		ch=(unsigned char)(n+'!');
		x=(n%d3_size_font->char_per_line)*d3_size_font->char_wid;
		y=((n/d3_size_font->char_per_line)*d3_size_font->char_high)+d3_size_font->char_baseline;

		// get the bitmap
		FT_Select_Charmap(face, ft_encoding_unicode);
		FT_UInt glyph_index = FT_Get_Char_Index(face,ch);

		error = FT_Load_Glyph(face,glyph_index,FT_LOAD_NO_HINTING);
		if (error){
			continue;
		}
		
		error = FT_Render_Glyph(face->glyph,FT_RENDER_MODE_LIGHT);
		if (error){
			continue;
		}
		bitmap = face->glyph->bitmap;
		
		// draw the bitmap
		for(font_y=0;font_y<bitmap.rows;font_y++){
			for(font_x=0;font_x<bitmap.width;font_x++){
				txt_x = x + font_x + face->glyph->bitmap_left;
				txt_y = y + font_y - face->glyph->bitmap_top-1;
				
				//bail if we are outside of the texture bounds
				if (
					(txt_x<0) || (txt_x>font_bitmap_pixel_sz)
					|| (txt_y<0) || (txt_y>font_bitmap_pixel_sz)
					) continue;
				
				//set the pointer to the (x,y) position in the texture
				ptr = data + ((txt_y * font_bitmap_pixel_sz + txt_x) << 2);
				*ptr++=0xFF;
				*ptr++=0xFF;
				*ptr++=0xFF;
				*ptr++=(bitmap.buffer[font_y * bitmap.pitch + font_x]);
				d3_size_font->char_size[n]=(float)(1+(face->glyph->advance.x>>6))/(float)d3_size_font->char_wid;
			}
		}
	}
	bitmap_data(&d3_size_font->bitmap,data,font_bitmap_pixel_sz,font_bitmap_pixel_sz,TRUE,anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE);
	free(data);
}

#endif

/* =======================================================

      Create Font Texture Windows
      
======================================================= */

#ifdef D3_OS_WINDOWS

void bitmap_text_size_internal(texture_font_size_type *d3_size_font,char *name,int size)
{
	int				n,x,y;
	unsigned char	ch;
	unsigned char	*data,*ptr;
	HDC				screen_dc,dc;
	HBITMAP			bmp,old_bmp;
	HFONT			font,old_font;
	HBRUSH			brsh,old_brsh;
	COLORREF		col;
	RECT			box;
	ABC				ch_abc;

		// data for bitmap

	data=malloc((font_bitmap_pixel_sz<<2)*font_bitmap_pixel_sz);
	if (data==NULL) return;

		// create bitmap

	screen_dc=GetDC(NULL);

	dc=CreateCompatibleDC(screen_dc);
	bmp=CreateCompatibleBitmap(screen_dc,font_bitmap_pixel_sz,font_bitmap_pixel_sz);
	old_bmp=SelectObject(dc,bmp);

	SetMapMode(dc,MM_TEXT);
	SetMapperFlags(dc,1);
	SetTextAlign(dc,TA_BASELINE);
	SetBkMode(dc,TRANSPARENT);
	SetBkColor(dc,RGB(0,0,0));

		// clear bitmap

	brsh=CreateSolidBrush(RGB(255,255,255));
	old_brsh=SelectObject(dc,brsh);

	box.left=box.top=0;
	box.right=font_bitmap_pixel_sz;
	box.bottom=font_bitmap_pixel_sz;

	FillRect(dc,&box,brsh);

	SelectObject(dc,old_brsh);
	DeleteObject(brsh);

		// draw the characters

	font=CreateFont(-size,0,0,0,FW_MEDIUM,0,0,0,0,OUT_OUTLINE_PRECIS,0,ANTIALIASED_QUALITY,0,name);
	if (font!=NULL) old_font=SelectObject(dc,font);
	
	SetTextColor(dc,RGB(0,0,0));

	for (n=0;n!=90;n++) {

			// draw the character

		ch=(unsigned char)(n+'!');

		x=(n%d3_size_font->char_per_line)*d3_size_font->char_wid;
		y=((n/d3_size_font->char_per_line)*d3_size_font->char_high)+d3_size_font->char_baseline;

		TextOut(dc,x,y,(char*)&ch,1);

			// get the spacing information

		GetCharABCWidths(dc,ch,ch,&ch_abc);
		d3_size_font->char_size[n]=((float)(ch_abc.abcA+ch_abc.abcB+ch_abc.abcC))/((float)d3_size_font->char_wid);
	}

	if (font!=NULL) {
		SelectObject(dc,old_font);
		DeleteObject(font);
	}
	
		// get the bitmap

	ptr=data;

	for (y=0;y!=font_bitmap_pixel_sz;y++) {

		for (x=0;x!=font_bitmap_pixel_sz;x++) {

			col=GetPixel(dc,x,y);

			*ptr++=0xFF;
			*ptr++=0xFF;
			*ptr++=0xFF;

			*ptr++=255-GetRValue(col);		// use the anti-aliased font as the alpha mask

		}
	}

	bitmap_data(&d3_size_font->bitmap,data,font_bitmap_pixel_sz,font_bitmap_pixel_sz,TRUE,anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE);

	free(data);

		// delete the bitmap

	SelectObject(dc,old_bmp);
	DeleteObject(bmp);
	DeleteDC(dc);
}

#endif

/* =======================================================

      Create Font Main Line
      
======================================================= */

void bitmap_text_size(texture_font_size_type *d3_size_font,char *name,int size)
{
	float			f_size;

	f_size=(float)size;

	d3_size_font->char_wid=(int)(f_size*1.05f);
	d3_size_font->char_high=(int)(f_size*1.15);
	d3_size_font->char_baseline=(int)(f_size*0.84);
	d3_size_font->char_per_line=font_bitmap_pixel_sz/d3_size_font->char_wid;

	d3_size_font->gl_xoff=((float)d3_size_font->char_wid/(float)font_bitmap_pixel_sz);
	d3_size_font->gl_xadd=(d3_size_font->gl_xoff-0.005f);
	d3_size_font->gl_yoff=((float)d3_size_font->char_high/(float)font_bitmap_pixel_sz);
	d3_size_font->gl_yadd=(((float)d3_size_font->char_high/(float)font_bitmap_pixel_sz)-0.005f);

	bitmap_text_size_internal(d3_size_font,name,size);
}

void bitmap_text_initialize(texture_font_type *d3_font)
{
	bitmap_text_size(&d3_font->size_24,d3_font->name,24);
	bitmap_text_size(&d3_font->size_48,d3_font->name,48);
}

void bitmap_text_shutdown(texture_font_type *d3_font)
{
	bitmap_close(&d3_font->size_24.bitmap);
	bitmap_close(&d3_font->size_48.bitmap);
}
