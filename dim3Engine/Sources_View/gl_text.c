/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Text Routines

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit �Created with dim3 Technology� is given on a single
application screen and in a single piece of the documentation
3. It is not resold, in it's current form or modified, as an
engine-only product

This code is presented as is. The author of dim3 takes no
responsibilities for any version of this code.

Any non-engine product (games, etc) created with this code is free
from any and all payment and/or royalties to the author of dim3,
and can be sold or given away.

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "video.h"

extern hud_type				hud;
extern setup_type			setup;

int							font_size;
float						font_char_size[90];
bitmap_type					font_bitmap;

/* =======================================================

      Setup Text Drawing
      
======================================================= */

#ifdef D3_OS_MAC


// supergumba -- try kCGImageAlphaNoneSkipLast, maybe that's why no drawing
// try release, then use data
// to test font, try:
// CTFontRef != null  CTFontCreateWithName(CFSTR("blech"),size,NULL);


void gl_text_initialize(void)
{
	int					n,x,y,data_sz,row_add;
	char				ch,font_name[256];
	unsigned char		*bm_data,*txt_data,*sptr,*dptr;
	CFStringRef			cf_font_name;
	CGPoint				txt_pt;
	CGContextRef		bitmap_ctx;
	CGColorSpaceRef		color_space;
	CGAffineTransform	trans_flip;
	
	CGRect	rect;
	
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
	
	CGContextClear(bitmap_ctx, CGRectMake( 0, 0, font_bitmap_pixel_sz, font_bitmap_pixel_sz ) );
 
	
	rect=CGRectMake(0,0,font_bitmap_pixel_sz,font_bitmap_pixel_sz);
//	CGContextClipToRect(bitmap_ctx,rect);

	// supergumba -- also try

//	CGContextScaleCTM(bitmap_ctx,1.0f,-1.0f);

	CGContextSetAlpha(bitmap_ctx,1.0f);		// supergumba -- do we need all of these???
	CGContextSetBlendMode(bitmap_ctx,kCGBlendModeNormal);

		// check which font to use

	strcpy(font_name,hud.font.name);
// supergumba
/*
	cf_font_name=CFStringCreateWithCString(kCFAllocatorDefault,hud.font.name,kCFStringEncodingMacRoman);
	if (CTFontCreateWithName(cf_font_name,font_bitmap_point,NULL)==NULL) strcpy(font_name,hud.font.alt_name);
	CFRelease(cf_font_name);
*/
		// create font

	CGContextSelectFont(bitmap_ctx,font_name,font_bitmap_point,kCGEncodingMacRoman);
	
	CGContextSetTextDrawingMode(bitmap_ctx,kCGTextFillStroke);
	CGContextSetRGBFillColor(bitmap_ctx,1.0f,1.0f,1.0f,1.0f);
	CGContextSetRGBStrokeColor(bitmap_ctx,1.0f,1.0f,1.0f,1.0f);

	trans_flip=CGAffineTransformMake(1.0f,0.0f,0.0f,-1.0f,0.0f,0.0f);
	CGContextSetTextMatrix(bitmap_ctx,trans_flip);
		
		// draw the characters

	for (n=0;n!=90;n++) {
	
			// draw the character

		ch=(n+'!');

		x=(n%font_bitmap_char_per_line)*font_bitmap_char_wid;
		y=((n/font_bitmap_char_per_line)*font_bitmap_char_high)+font_bitmap_char_baseline;

		CGContextShowTextAtPoint(bitmap_ctx,x,y,&ch,1);

			// get the spacing information
			
		txt_pt=CGContextGetTextPosition(bitmap_ctx);

		font_char_size[n]=(txt_pt.x-(float)x)/(float)font_bitmap_char_wid;
	}
	
	// supergumba -- test!
	
//	rect=CGRectMake(0,0,512,250);
//	CGContextSetRGBFillColor(bitmap_ctx,1.0f,0.0f,1.0f,1.0f);
//	CGContextFillRect(bitmap_ctx,rect);
	
//	CGContextSetRGBFillColor( bitmap_ctx, 1, 1, 0, 1 );
//	CGContextFillRect( bitmap_ctx, CGRectMake( 0, 0, font_bitmap_pixel_sz, font_bitmap_pixel_sz ) );

	
//	CGContextFlush(bitmap_ctx);

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

	for (y=0;y!=font_bitmap_pixel_sz;y++) {
	
		sptr=bm_data+(((font_bitmap_pixel_sz-1)-y)*row_add);

		for (x=0;x!=font_bitmap_pixel_sz;x++) {

			*dptr++=0xFF;
			*dptr++=0xFF;
			*dptr++=0xFF;
			*dptr++=*sptr;		// use the anti-aliased font as the alpha mask (using the red component)
			
			sptr+=4;

		}
	}
	
	bitmap_data(&font_bitmap,txt_data,font_bitmap_pixel_sz,font_bitmap_pixel_sz,TRUE,anisotropic_mode_none,mipmap_mode_none,FALSE);

	free(txt_data);
	
		// dispose the bitmap context
		
	CGContextRelease(bitmap_ctx);
	free(bm_data);
}



/* supergumba
void gl_text_initialize(void)
{
	int					n,x,y,row_add,font;
	unsigned char		ch,p_str[name_str_len+1];
	unsigned char		*data,*sptr,*dptr;
	Rect				box;
	PixMapHandle		texturemap;
	GWorldPtr			gworld;
	CGrafPtr			org_gworld;
	GDHandle			org_gdhand;
	
		// data for bitmap

	data=malloc((font_bitmap_pixel_sz<<2)*font_bitmap_pixel_sz);
	if (data==NULL) return;

		// create bitmap
		
	SetRect(&box,0,0,font_bitmap_pixel_sz,font_bitmap_pixel_sz);
	NewGWorld(&gworld,32,&box,NULL,NULL,0);
	
	GetGWorld(&org_gworld,&org_gdhand);
	SetGWorld(gworld,NULL);
	
		// draw the characters

	strcpy((char*)&p_str[1],hud.font.name);
	p_str[0]=(unsigned char)strlen(hud.font.name);
	font=FMGetFontFamilyFromName(p_str);

	if (font==kInvalidFontFamily) {
		strcpy((char*)&p_str[1],hud.font.alt_name);
		p_str[0]=(unsigned char)strlen(hud.font.alt_name);
		font=FMGetFontFamilyFromName(p_str);
	}

	TextFont(font);
	TextSize(font_bitmap_point);
	TextFace(0);
	
	SwapQDTextFlags(kQDUseCGTextRendering|kQDUseCGTextMetrics);
		
	for (n=0;n!=90;n++) {

			// draw the character

		ch=(unsigned char)(n+'!');

		x=(n%font_bitmap_char_per_line)*font_bitmap_char_wid;
		y=((n/font_bitmap_char_per_line)*font_bitmap_char_high)+font_bitmap_char_baseline;

		MoveTo(x,y);
		DrawChar(ch);

			// get the spacing information

		font_char_size[n]=(float)CharWidth(ch)/(float)font_bitmap_char_wid;
	}

		// get the bitmap

	texturemap=GetGWorldPixMap(gworld);

	LockPixels(texturemap);
	sptr=(unsigned char*)GetPixBaseAddr(texturemap);
	row_add=GetPixRowBytes(texturemap)-(font_bitmap_pixel_sz<<2);
	
	dptr=data;

	for (y=0;y!=font_bitmap_pixel_sz;y++) {

		for (x=0;x!=font_bitmap_pixel_sz;x++) {

			*dptr++=0xFF;
			*dptr++=0xFF;
			*dptr++=0xFF;

			*dptr++=*sptr++;		// use the anti-aliased font as the alpha mask (using the red component)
			sptr+=3;

		}
		
		sptr+=row_add;
	}
	
	UnlockPixels(texturemap);
	
	bitmap_data(&font_bitmap,data,font_bitmap_pixel_sz,font_bitmap_pixel_sz,TRUE,anisotropic_mode_none,mipmap_mode_none,FALSE);

	free(data);

		// dispose the gworld
		
	SetGWorld(org_gworld,org_gdhand);
	DisposeGWorld(gworld);
}
*/

#endif

#ifdef D3_OS_WINDOWS

void gl_text_initialize(void)
{
	int				n,x,y;
	unsigned char	ch;
	unsigned char	*data,*ptr;
	HDC				screen_dc,dc;
	HBITMAP			bmp,*old_bmp;
	HFONT			font;
	COLORREF		col;
	SIZE			chsz;

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

		// draw the characters

	font=CreateFont(-font_bitmap_point,0,0,0,FW_MEDIUM,0,0,0,0,OUT_OUTLINE_PRECIS,0,ANTIALIASED_QUALITY,0,hud.font.name);
	if (font==NULL) {
		font=CreateFont(-font_bitmap_point,0,0,0,FW_MEDIUM,0,0,0,0,OUT_OUTLINE_PRECIS,0,ANTIALIASED_QUALITY,0,hud.font.alt_name);
	}

	SelectObject(dc,font);
	SetTextColor(dc,RGB(255,255,255));

	for (n=0;n!=90;n++) {

			// draw the character

		ch=(unsigned char)(n+'!');

		x=(n%font_bitmap_char_per_line)*font_bitmap_char_wid;
		y=((n/font_bitmap_char_per_line)*font_bitmap_char_high)+font_bitmap_char_baseline;

		TextOut(dc,x,y,(char*)&ch,1);

			// get the spacing information

		GetTextExtentPoint32(dc,(char*)&ch,1,&chsz);
		font_char_size[n]=(float)chsz.cx/(float)font_bitmap_char_wid;
	}

	DeleteObject(font);

		// get the bitmap

	ptr=data;

	for (y=0;y!=font_bitmap_pixel_sz;y++) {

		for (x=0;x!=font_bitmap_pixel_sz;x++) {

			col=GetPixel(dc,x,y);

			*ptr++=0xFF;
			*ptr++=0xFF;
			*ptr++=0xFF;

			*ptr++=GetRValue(col);		// use the anti-aliased font as the alpha mask

		}
	}

	bitmap_data(&font_bitmap,data,font_bitmap_pixel_sz,font_bitmap_pixel_sz,TRUE,anisotropic_mode_none,mipmap_mode_none,FALSE);

	free(data);

		// delete the bitmap

	SelectObject(dc,old_bmp);
	DeleteObject(bmp);
	DeleteDC(dc);
}

#endif

void gl_text_shutdown(void)
{
	bitmap_close(&font_bitmap);
}

/* =======================================================

      Character/String Sizes
      
======================================================= */

inline int gl_text_get_char_height(int text_size)
{
	return((int)(((float)text_size)*text_height_factor));
}

int gl_text_get_string_width(int text_size,char *str)
{
	int			i,ch,len;
	float		fx,f_wid;
	char		*c;
	
	f_wid=(float)text_size;
	
	c=str;
	len=strlen(str);
	
	fx=0.0f;
	
	for (i=0;i<len;i++) {
		ch=(int)*c++;

		if ((ch>='!') && (ch<='z')) {
			ch-='!';
			fx+=(f_wid*font_char_size[ch]);
		}
		else {
			fx+=(f_wid/3.0f);
		}
	}
	
	return((int)fx);
}

/* =======================================================

      Start and End Text Drawing
      
======================================================= */

void gl_text_start(int text_size)
{
	GLfloat				fct[4];

		// remember font size
		
	font_size=text_size;
	
		// setup texture
		
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D,font_bitmap.gl_id);
	
		// texture combines
		
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);
	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_MODULATE);

	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB,GL_SRC_COLOR);
	
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB,GL_PRIMARY_COLOR);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_RGB,GL_SRC_COLOR);
	
		// alpha
		
	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_ALPHA,GL_SRC_ALPHA);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_ALPHA,GL_CONSTANT);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_ALPHA,GL_SRC_ALPHA);

	fct[0]=fct[1]=fct[2]=fct[3]=1.0f;	
	glTexEnvfv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_COLOR,fct);
	
		// no wrapping
		
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
	
		// transparency blending
		
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
}

void gl_text_end(void)
{
	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	
		// restore wrapping
		
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    
	glDisable(GL_TEXTURE_2D);
}

/* =======================================================

      Draw Line of Text
      
======================================================= */

void gl_text_draw(int x,int y,char *txt,int just,bool vcenter,d3col *col,float alpha)
{
	int			n,txtlen,ch,xoff,yoff,cnt;
	float		f_lft,f_rgt,f_top,f_bot,f_wid,f_high,
				gx_lft,gx_rgt,gy_top,gy_bot;
	float		*vertex_ptr,*uv_ptr;
	char		*c;
	GLfloat		fct[4];

		// get text length

	txtlen=strlen(txt);
	if (txtlen==0) return;

        // font justification
        
	switch (just) {
		case tx_center:
			x-=(gl_text_get_string_width(font_size,txt)>>1);
			break;
		case tx_right:
			x-=gl_text_get_string_width(font_size,txt);
			break;
	}
	
        // font color and alpha
        
	glColor3f(col->r,col->g,col->b);

	fct[0]=fct[1]=fct[2]=1.0f;
	fct[3]=alpha;
	
	glTexEnvfv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_COLOR,fct);
    
		// get width and height
		
	f_wid=(float)font_size;
	f_high=f_wid*text_height_factor;

		// construct VBO

	vertex_ptr=view_bind_map_next_vertex_object(((txtlen*4)*(2+2)));
	if (vertex_ptr==NULL) return;

	uv_ptr=vertex_ptr+((txtlen*4)*2);

		// create the quads

	f_lft=(float)x;
	f_bot=(float)y;
	if (vcenter) f_bot+=((f_high/2)+(f_high/8));		// add in middle + descender
	f_top=f_bot-f_high;
	
	c=txt;
	cnt=0;
	
	for (n=0;n<txtlen;n++) {
	
		ch=(int)*c++;

		if ((ch>='!') && (ch<='z')) {
			
			ch-='!';

				// the vertexes

			f_rgt=f_lft+f_wid;

			*vertex_ptr++=f_lft;
			*vertex_ptr++=f_top;
			*vertex_ptr++=f_rgt;
			*vertex_ptr++=f_top;
			*vertex_ptr++=f_rgt;
			*vertex_ptr++=f_bot;
			*vertex_ptr++=f_lft;
			*vertex_ptr++=f_bot;

			f_lft+=(f_wid*font_char_size[ch]);

				// the UVs

			yoff=ch/font_bitmap_char_per_line;
			xoff=ch-(yoff*font_bitmap_char_per_line);

			gx_lft=((float)xoff)*font_bitmap_gl_xoff;
			gx_rgt=gx_lft+font_bitmap_gl_xadd;
			gy_top=((float)yoff)*font_bitmap_gl_yoff;
			gy_bot=gy_top+font_bitmap_gl_yadd;

			*uv_ptr++=gx_lft;
			*uv_ptr++=gy_top;
			*uv_ptr++=gx_rgt;
			*uv_ptr++=gy_top;
			*uv_ptr++=gx_rgt;
			*uv_ptr++=gy_bot;
			*uv_ptr++=gx_lft;
			*uv_ptr++=gy_bot;

			cnt+=4;
		}
		else {
			f_lft+=(f_wid/3);
		}
	}

	view_unmap_current_vertex_object();

		// draw text

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2,GL_FLOAT,0,(void*)0);
		
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(void*)(((txtlen*4)*2)*sizeof(float)));

	glDrawArrays(GL_QUADS,0,cnt);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	view_unbind_current_vertex_object();
}

