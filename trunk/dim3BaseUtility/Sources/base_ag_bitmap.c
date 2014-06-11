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

/* =======================================================

      Pixel Utilities
      
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

bool bitmap_ag_texture_brick(texture_frame_type *frame,int pixel_sz)
{
	bitmap_ag_type	ag_bitmap;

	ag_bitmap.pixel_sz=pixel_sz;
	ag_bitmap.frame=frame;
	bitmap_ag_random_color(&ag_bitmap.back_col,127,0,0,127,127,64);

	if (!bitmap_ag_texture_create(&ag_bitmap)) return(FALSE);
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
	bitmap_ag_random_color(&ag_bitmap.back_col,0,0,255,64,127,0);

	if (!bitmap_ag_texture_create(&ag_bitmap)) return(FALSE);
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
	bitmap_ag_random_color(&ag_bitmap.back_col,127,0,0,64,0,0);

	if (!bitmap_ag_texture_create(&ag_bitmap)) return(FALSE);
	return(bitmap_ag_texture_finish(&ag_bitmap));
}

/* =======================================================

      Cement Type Textures
      
======================================================= */

bool bitmap_ag_texture_cement(texture_frame_type *frame,int pixel_sz)
{
	bitmap_ag_type	ag_bitmap;

	ag_bitmap.pixel_sz=pixel_sz;
	ag_bitmap.frame=frame;
	bitmap_ag_random_color_lock(&ag_bitmap.back_col,127,64);

	if (!bitmap_ag_texture_create(&ag_bitmap)) return(FALSE);
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
