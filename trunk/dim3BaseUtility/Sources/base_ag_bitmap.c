/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Auto Generate Textures

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit ÒCreated with dim3 TechnologyÓ is given on a single
application screen and in a single piece of the documentation
3. It is not resold, in it's current form or modified, as an
engine-only product

This code is presented as is. The author of dim3 takes no
responsibilities for any version of this code.

Any non-engine product (games, etc) created with this code is free
from any and all payment and/or royalties to the author of dim3,
and can be sold or given away.

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3baseutility.h"
#endif

#define bitmap_ag_max_brick_count			20
#define bitmap_ag_start_brick_col_size		90
#define bitmap_ag_extra_brick_col_size		90
#define bitmap_ag_start_brick_row_size		40
#define bitmap_ag_extra_brick_row_size		50
#define bitmap_ag_start_brick_margin		2
#define bitmap_ag_extra_brick_margin		5
#define bitmap_ag_start_brick_lip			4
#define bitmap_ag_extra_brick_lip			4
#define bitmap_ag_brick_color_factor		0.2f
#define bitmap_ag_brick_darken_factor		0.7f

typedef struct	{
					int						pixel_sz;
					unsigned char			*png_data,*bump_data,*spec_data;
					d3col					back_col;
					texture_frame_type		*frame;
				} bitmap_ag_type;

/* =======================================================

      Brick Type Utility
      
======================================================= */

int bitmap_ag_random_int(int max)
{
	float			f;

	f=(float)random()/(float)RAND_MAX;
	return((int)(f*(float)max));
}

bool bitmap_ag_random_bool(void)
{
	return(bitmap_ag_random_int(100)>=50);
}

void bitmap_ag_random_color(d3col *col,int min_r,int min_g,int min_b,int max_r,int max_g,int max_b)
{
	col->r=((float)(min_r+(bitmap_ag_random_int(max_r-min_r))))/255.0f;
	col->g=((float)(min_g+(bitmap_ag_random_int(max_g-min_g))))/255.0f;
	col->b=((float)(min_b+(bitmap_ag_random_int(max_b-min_b))))/255.0f;
}

void bitmap_ag_random_color_lock(d3col *col,int min,int max)
{
	float		f;

	f=((float)(min+(bitmap_ag_random_int(max-min))))/255.0f;
	col->r=col->g=col->b=f;
}

void bitmap_ag_random_color_change(d3col *base_col,d3col *col,float fct)
{
	float		f;

	f=(((float)(100-bitmap_ag_random_int(200)))*0.01f)*fct;

	col->r=base_col->r+f;
	if (col->r<0.0f) col->r=0.0f;
	if (col->r>1.0f) col->r=1.0f;

	col->g=base_col->g+f;
	if (col->g<0.0f) col->g=0.0f;
	if (col->g>1.0f) col->g=1.0f;

	col->b=base_col->b+f;
	if (col->b<0.0f) col->b=0.0f;
	if (col->b>1.0f) col->b=1.0f;
}

/* =======================================================

      Pixel Utilities
      
======================================================= */

void bitmap_ag_texture_read_pixel(bitmap_ag_type *ag_bitmap,unsigned char *data,int x,int y,d3col *col)
{
	unsigned char		*ptr;

	if ((x<0) || (x>=ag_bitmap->pixel_sz)) return;
	if ((y<0) || (y>=ag_bitmap->pixel_sz)) return;

	ptr=data+((y*(ag_bitmap->pixel_sz*3))+(x*3));
	col->r=((float)*ptr++)/255.0f;
	col->g=((float)*ptr++)/255.0f;
	col->b=((float)*ptr)/255.0f;
}

void bitmap_ag_texture_write_pixel(bitmap_ag_type *ag_bitmap,unsigned char *data,int x,int y,d3col *col)
{
	unsigned char		*ptr;

	if ((x<0) || (x>=ag_bitmap->pixel_sz)) return;
	if ((y<0) || (y>=ag_bitmap->pixel_sz)) return;

	ptr=data+((y*(ag_bitmap->pixel_sz*3))+(x*3));
	*ptr++=(int)(((float)0xFF)*col->r);
	*ptr++=(int)(((float)0xFF)*col->g);
	*ptr=(int)(((float)0xFF)*col->b);
}

void bitmap_ag_texture_add_noise(bitmap_ag_type *ag_bitmap,unsigned char *data,int x,int y,int wid,int high,float percentage)
{
	int			px,py,per_ok;
	float		fct;
	d3col		col;

		// get precent where it's OK to
		// add noise

	per_ok=(int)(100.0f*percentage);

	for (py=y;py!=(y+high);py++) {
		for (px=x;px!=(x+wid);px++) {

			if (bitmap_ag_random_int(100)>per_ok) continue;

			bitmap_ag_texture_read_pixel(ag_bitmap,data,px,py,&col);

			fct=0.8f+(((float)bitmap_ag_random_int(100))*0.004f);

			col.r*=fct;
			if (col.r>1.0f) col.r=1.0f;
			col.g*=fct;
			if (col.g>1.0f) col.g=1.0f;
			col.b*=fct;
			if (col.b>1.0f) col.b=1.0f;

			bitmap_ag_texture_write_pixel(ag_bitmap,data,px,py,&col);
		}
	}
}

void bitmap_ag_texture_damage_mark(bitmap_ag_type *ag_bitmap,unsigned char *data,int x,int y,float fct)
{
	int			n,px,py,sz;
	d3col		col;

	sz=15;

	while (sz>4) {

		for (n=0;n!=100;n++) {
			px=x+(sz-bitmap_ag_random_int(sz*2));
			py=y+(sz-bitmap_ag_random_int(sz*2));
		
			bitmap_ag_texture_read_pixel(ag_bitmap,data,px,py,&col);

			col.r*=fct;
			if (col.r>1.0f) col.r=1.0f;
			col.g*=fct;
			if (col.g>1.0f) col.g=1.0f;
			col.b*=fct;
			if (col.b>1.0f) col.b=1.0f;

			bitmap_ag_texture_write_pixel(ag_bitmap,data,px,py,&col);
		}

		sz--;
	}

}

/* =======================================================

      Bitmap Utilities
      
======================================================= */

bool bitmap_ag_texture_create(bitmap_ag_type *ag_bitmap)
{
	int				n,pixel_count,byte_len,kr,kg,kb;
	unsigned char	*p_ptr,*b_ptr,*s_ptr;
	
	ag_bitmap->frame->bitmap.wid=ag_bitmap->frame->bitmap.high=ag_bitmap->pixel_sz;
	ag_bitmap->frame->bitmap.opaque=TRUE;

	ag_bitmap->frame->bumpmap.wid=ag_bitmap->frame->bumpmap.high=ag_bitmap->pixel_sz;
	ag_bitmap->frame->bumpmap.opaque=TRUE;

	ag_bitmap->frame->specularmap.wid=ag_bitmap->frame->specularmap.high=ag_bitmap->pixel_sz;
	ag_bitmap->frame->specularmap.opaque=TRUE;

	ag_bitmap->frame->glowmap.wid=ag_bitmap->frame->glowmap.high=ag_bitmap->pixel_sz;
	ag_bitmap->frame->glowmap.opaque=TRUE;
	
		// get the data

	pixel_count=ag_bitmap->frame->bitmap.wid*ag_bitmap->frame->bitmap.high;
	byte_len=pixel_count*3;

	ag_bitmap->png_data=malloc(byte_len);
	if (ag_bitmap->png_data==NULL) return(FALSE);

	ag_bitmap->bump_data=malloc(byte_len);
	if (ag_bitmap->bump_data==NULL) {
		free(ag_bitmap->png_data);
		return(FALSE);
	}

	ag_bitmap->spec_data=malloc(byte_len);
	if (ag_bitmap->spec_data==NULL) {
		free(ag_bitmap->bump_data);
		free(ag_bitmap->png_data);
		return(FALSE);
	}

		// fill the png

	kr=(int)(((float)0xFF)*ag_bitmap->back_col.r);
	kg=(int)(((float)0xFF)*ag_bitmap->back_col.g);
	kb=(int)(((float)0xFF)*ag_bitmap->back_col.b);
	
	p_ptr=ag_bitmap->png_data;
	b_ptr=ag_bitmap->bump_data;
	s_ptr=ag_bitmap->spec_data;

	for (n=0;n!=pixel_count;n++) {
		*p_ptr++=kr;
		*p_ptr++=kg;
		*p_ptr++=kb;
		*b_ptr++=0x0;
		*b_ptr++=0x0;
		*b_ptr++=0xFF;
		*s_ptr++=0x0;
		*s_ptr++=0x0;
		*s_ptr++=0x0;
	}

	return(TRUE);
}

bool bitmap_ag_texture_finish(bitmap_ag_type *ag_bitmap)
{
		// open the texture in opengl
		// supergumba -- this will eventually save to disk

	bitmap_texture_open(&ag_bitmap->frame->bitmap,ag_bitmap->png_data,FALSE,FALSE,FALSE,FALSE);
	bitmap_texture_open(&ag_bitmap->frame->bumpmap,ag_bitmap->bump_data,FALSE,FALSE,FALSE,FALSE);
	bitmap_texture_open(&ag_bitmap->frame->specularmap,ag_bitmap->spec_data,FALSE,FALSE,FALSE,FALSE);
//	bitmap_texture_open(&ag_bitmap->frame->glowmap,ag_bitmap->glow_data,FALSE,FALSE,FALSE,FALSE);

		// free data

	free(ag_bitmap->png_data);
	free(ag_bitmap->bump_data);
	free(ag_bitmap->spec_data);

	return(TRUE);
}

/* =======================================================

      Brick Type Textures
      
======================================================= */

int bitmap_ag_texture_brick_create_sizes(bitmap_ag_type *ag_bitmap,int start_size,int extra_size,int *sizes)
{
	int			n,tot_sz;

	n=0;
	tot_sz=0;

	while (TRUE) {
		sizes[n]=start_size+bitmap_ag_random_int(extra_size);
		if ((tot_sz+sizes[n])>=ag_bitmap->pixel_sz) {
			sizes[n]=ag_bitmap->pixel_sz-tot_sz;
			return(n+1);
		}

		tot_sz+=sizes[n];

		n++;
		if (n==bitmap_ag_max_brick_count) return(n);
	}
}

void bitmap_ag_texture_brick_draw_brick(bitmap_ag_type *ag_bitmap,int px,int py,int wid,int high,int lip_sz,d3col *col)
{
	int			n,x,y,lx,rx,ty,by;
	float		f;
	d3col		dark_col,col2;

		// get darker color for outlines

	dark_col.r=col->r*bitmap_ag_brick_darken_factor;
	dark_col.g=col->g*bitmap_ag_brick_darken_factor;
	dark_col.b=col->b*bitmap_ag_brick_darken_factor;

		// draw the brick

	lx=px;
	rx=px+wid;
	ty=py;
	by=py+high;

	for (n=0;n!=lip_sz;n++) {

		if ((rx<=lx) || (by<=ty)) break;

			// get lip color

		f=(float)(n+1)/(float)lip_sz;
		col2.r=dark_col.r+((col->r-dark_col.r)*f);
		col2.g=dark_col.g+((col->g-dark_col.g)*f);
		col2.b=dark_col.b+((col->b-dark_col.b)*f);

			// draw brick

		for (y=ty;y!=by;y++) {
			for (x=lx;x!=rx;x++) {
				bitmap_ag_texture_write_pixel(ag_bitmap,ag_bitmap->png_data,x,y,&col2);
			}
		}

		lx++;
		rx-=2;
		ty++;
		by-=2;
	}
}

bool bitmap_ag_texture_brick(texture_frame_type *frame,int pixel_sz)
{
	int				px,py,row,col,margin,lip_sz,
					row_count,row_sizes[bitmap_ag_max_brick_count],
					col_count,col_sizes[bitmap_ag_max_brick_count];
	d3col			base_col,brick_col;
	bitmap_ag_type	ag_bitmap;

	ag_bitmap.pixel_sz=pixel_sz;
	ag_bitmap.frame=frame;
	bitmap_ag_random_color_lock(&ag_bitmap.back_col,127,64);		// grout

	if (!bitmap_ag_texture_create(&ag_bitmap)) return(FALSE);

		// grout noise

	bitmap_ag_texture_add_noise(&ag_bitmap,ag_bitmap.png_data,0,0,pixel_sz,pixel_sz,0.5f);

		// get number of bricks

	row_count=bitmap_ag_texture_brick_create_sizes(&ag_bitmap,bitmap_ag_start_brick_row_size,bitmap_ag_extra_brick_row_size,row_sizes);
	col_count=bitmap_ag_texture_brick_create_sizes(&ag_bitmap,bitmap_ag_start_brick_col_size,bitmap_ag_extra_brick_col_size,col_sizes);
	margin=bitmap_ag_start_brick_margin+bitmap_ag_random_int(bitmap_ag_extra_brick_margin);

		// brick base color

	switch (bitmap_ag_random_int(3)) {
		case 0:
			bitmap_ag_random_color(&base_col,150,40,40,127,0,0);
			break;
		case 1:
			bitmap_ag_random_color(&base_col,127,50,0,70,50,0);
			break;
		case 2:
			bitmap_ag_random_color(&base_col,127,127,127,200,200,200);
			break;
	}

		// draw the bricks

	py=margin;

	for (row=0;row!=row_count;row++) {

		px=margin;

		for (col=0;col!=col_count;col++) {
			bitmap_ag_random_color_change(&base_col,&brick_col,bitmap_ag_brick_color_factor);
			lip_sz=bitmap_ag_start_brick_lip+bitmap_ag_random_int(bitmap_ag_extra_brick_lip);
			bitmap_ag_texture_brick_draw_brick(&ag_bitmap,px,py,(col_sizes[col]-margin),(row_sizes[row]-margin),lip_sz,&brick_col);
			bitmap_ag_texture_add_noise(&ag_bitmap,ag_bitmap.png_data,px,py,(col_sizes[col]-margin),(row_sizes[row]-margin),0.3f);
			px+=col_sizes[col];
		}

		py+=row_sizes[row];
	}

		// save the texture

	return(bitmap_ag_texture_finish(&ag_bitmap));
}

/* =======================================================

      Tile Type Textures
      
======================================================= */

bool bitmap_ag_texture_tile(texture_frame_type *frame,int pixel_sz)
{
	bitmap_ag_type	ag_bitmap;

	ag_bitmap.pixel_sz=pixel_sz;
	ag_bitmap.frame=frame;
	bitmap_ag_random_color(&ag_bitmap.back_col,200,200,200,150,100,150);

	if (!bitmap_ag_texture_create(&ag_bitmap)) return(FALSE);
	
	bitmap_ag_texture_brick_draw_brick(&ag_bitmap,0,0,pixel_sz,pixel_sz,20,&ag_bitmap.back_col);
	bitmap_ag_texture_add_noise(&ag_bitmap,ag_bitmap.png_data,0,0,pixel_sz,pixel_sz,0.3f);

	return(bitmap_ag_texture_finish(&ag_bitmap));
}

/* =======================================================

      Metal Type Textures
      
======================================================= */

bool bitmap_ag_texture_metal(texture_frame_type *frame,int pixel_sz)
{
	bitmap_ag_type	ag_bitmap;

	ag_bitmap.pixel_sz=pixel_sz;
	ag_bitmap.frame=frame;
	bitmap_ag_random_color(&ag_bitmap.back_col,0,0,200,32,64,127);

	if (!bitmap_ag_texture_create(&ag_bitmap)) return(FALSE);
	
	bitmap_ag_texture_brick_draw_brick(&ag_bitmap,0,0,pixel_sz,pixel_sz,20,&ag_bitmap.back_col);

	return(bitmap_ag_texture_finish(&ag_bitmap));
}

/* =======================================================

      Wood Type Textures
      
======================================================= */

bool bitmap_ag_texture_wood(texture_frame_type *frame,int pixel_sz)
{
	bitmap_ag_type	ag_bitmap;

	ag_bitmap.pixel_sz=pixel_sz;
	ag_bitmap.frame=frame;
	bitmap_ag_random_color(&ag_bitmap.back_col,127,50,0,100,40,0);

	if (!bitmap_ag_texture_create(&ag_bitmap)) return(FALSE);
	return(bitmap_ag_texture_finish(&ag_bitmap));
}

/* =======================================================

      Cement Type Textures
      
======================================================= */

bool bitmap_ag_texture_cement(texture_frame_type *frame,int pixel_sz)
{
	int				n,x,y;
	bitmap_ag_type	ag_bitmap;

	ag_bitmap.pixel_sz=pixel_sz;
	ag_bitmap.frame=frame;
	bitmap_ag_random_color_lock(&ag_bitmap.back_col,127,64);

	if (!bitmap_ag_texture_create(&ag_bitmap)) return(FALSE);

		// cement noise

	bitmap_ag_texture_add_noise(&ag_bitmap,ag_bitmap.png_data,0,0,pixel_sz,pixel_sz,0.8f);

		// pot marks

	for (n=0;n!=30;n++) {
		x=bitmap_ag_random_int(pixel_sz);
		y=bitmap_ag_random_int(pixel_sz);
		bitmap_ag_texture_damage_mark(&ag_bitmap,ag_bitmap.png_data,x,y,1.2f);
	}

		// save textures

	return(bitmap_ag_texture_finish(&ag_bitmap));
}

/* =======================================================

      Window Type Textures
      
======================================================= */

bool bitmap_ag_texture_window(texture_frame_type *frame,int pixel_sz)
{
	bitmap_ag_type	ag_bitmap;

	ag_bitmap.pixel_sz=pixel_sz;
	ag_bitmap.frame=frame;
	bitmap_ag_random_color(&ag_bitmap.back_col,255,255,255,200,200,32);

	if (!bitmap_ag_texture_create(&ag_bitmap)) return(FALSE);
	return(bitmap_ag_texture_finish(&ag_bitmap));
}

/* =======================================================

      Machine Type Textures
      
======================================================= */

bool bitmap_ag_texture_machine(texture_frame_type *frame,int pixel_sz)
{
	bitmap_ag_type	ag_bitmap;

	ag_bitmap.pixel_sz=pixel_sz;
	ag_bitmap.frame=frame;
	bitmap_ag_random_color(&ag_bitmap.back_col,0,0,255,64,127,0);

	if (!bitmap_ag_texture_create(&ag_bitmap)) return(FALSE);
	return(bitmap_ag_texture_finish(&ag_bitmap));
}
