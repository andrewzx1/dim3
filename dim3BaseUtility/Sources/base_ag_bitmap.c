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
#define bitmap_ag_brick_color_factor		0.05f
#define bitmap_ag_brick_darken_factor		0.7f

#define bitmap_ag_tile_start_split_count	2
#define bitmap_ag_tile_extra_split_count	2
#define bitmap_ag_tile_style_border			0
#define bitmap_ag_tile_style_checker		1
#define bitmap_ag_tile_style_stripe			2

#define bitmap_ag_cement_mark_count			50
#define bitmap_ag_cement_start_mark_size	20
#define bitmap_ag_cement_extra_mark_size	50

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

void bitmap_ag_texture_read_normal(bitmap_ag_type *ag_bitmap,unsigned char *data,int x,int y,d3vct *normal)
{
	unsigned char		*ptr;

	if ((x<0) || (x>=ag_bitmap->pixel_sz)) return;
	if ((y<0) || (y>=ag_bitmap->pixel_sz)) return;

	ptr=data+((y*(ag_bitmap->pixel_sz*3))+(x*3));
	normal->x=(((float)*ptr++)/128.0f)-1.0f;
	normal->y=(((float)*ptr++)/128.0f)-1.0f;
	normal->z=(((float)*ptr)/128.0f)-1.0f;
}

void bitmap_ag_texture_write_normal(bitmap_ag_type *ag_bitmap,unsigned char *data,int x,int y,d3vct *normal)
{
	unsigned char		*ptr;

	if ((x<0) || (x>=ag_bitmap->pixel_sz)) return;
	if ((y<0) || (y>=ag_bitmap->pixel_sz)) return;

	vector_normalize(normal);		// normalize before writing

	ptr=data+((y*(ag_bitmap->pixel_sz*3))+(x*3));
	*ptr++=(int)((normal->x+1.0f)*128.0f);
	*ptr++=(int)((normal->y+1.0f)*128.0f);
	*ptr=(int)((normal->z+1.0f)*128.0f);
}

void bitmap_ag_texture_add_noise(bitmap_ag_type *ag_bitmap,int x,int y,int wid,int high,float base_darken,float percentage)
{
	int			px,py,per_ok;
	float		fct;
	d3col		col;

		// get precent where it's OK to
		// add noise

	per_ok=(int)(100.0f*percentage);

	for (py=y;py<(y+high);py++) {
		for (px=x;px<(x+wid);px++) {

			if (bitmap_ag_random_int(100)>per_ok) continue;

				// the bitmap noise

			bitmap_ag_texture_read_pixel(ag_bitmap,ag_bitmap->png_data,px,py,&col);

			fct=base_darken+(((float)bitmap_ag_random_int(100))*0.004f);

			col.r*=fct;
			if (col.r>1.0f) col.r=1.0f;
			col.g*=fct;
			if (col.g>1.0f) col.g=1.0f;
			col.b*=fct;
			if (col.b>1.0f) col.b=1.0f;

			bitmap_ag_texture_write_pixel(ag_bitmap,ag_bitmap->png_data,px,py,&col);
		}
	}
}

void bitmap_ag_texture_damage_mark(bitmap_ag_type *ag_bitmap,int x,int y,int wid,int high,float fct)
{
	int			n,k,px,py,mx,my;
	d3col		col;
	d3vct		normal;

	for (k=0;k!=20;k++) {

		for (n=0;n!=100;n++) {
			mx=wid-bitmap_ag_random_int(wid<<1);
			px=x+mx;

			my=high-bitmap_ag_random_int(high<<1);
			py=y+my;

				// make sure they wrap

			if (px<0) px=ag_bitmap->pixel_sz+px;
			if (px>=ag_bitmap->pixel_sz) px-=ag_bitmap->pixel_sz;

			if (py<0) py=ag_bitmap->pixel_sz+py;
			if (py>=ag_bitmap->pixel_sz) py-=ag_bitmap->pixel_sz;
		
				// add the mark pixels as random particles

			bitmap_ag_texture_read_pixel(ag_bitmap,ag_bitmap->png_data,px,py,&col);

			col.r*=fct;
			if (col.r>1.0f) col.r=1.0f;
			col.g*=fct;
			if (col.g>1.0f) col.g=1.0f;
			col.b*=fct;
			if (col.b>1.0f) col.b=1.0f;

			bitmap_ag_texture_write_pixel(ag_bitmap,ag_bitmap->png_data,px,py,&col);

				// get a normal for the pixel change

			bitmap_ag_texture_read_normal(ag_bitmap,ag_bitmap->bump_data,px,py,&normal);
			normal.x+=((float)mx)/((float)wid);
			normal.y+=((float)my)/((float)high);
			normal.z=0.5f;		// always force Z normal back to top
			bitmap_ag_texture_write_normal(ag_bitmap,ag_bitmap->bump_data,px,py,&normal);
		}
		
		wid-=2;
		high-=2;
		
		if ((wid<5) || (high<5)) break;
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

void bitmap_ag_texture_make_spec(bitmap_ag_type *ag_bitmap,float spec_fct)
{
	int			px,py;
	float		f;
	d3col		col;

	for (py=0;py!=ag_bitmap->pixel_sz;py++) {
		for (px=0;px!=ag_bitmap->pixel_sz;px++) {

			bitmap_ag_texture_read_pixel(ag_bitmap,ag_bitmap->png_data,px,py,&col);

				// get max color

			f=col.r;
			if (col.g>f) f=col.g;
			if (col.b>f) f=col.b;

				// add in the contrast

			f=((f-0.5f)*spec_fct)+f;
			if (f<0.0f) f=0.0f;
			if (f>1.0f) f=1.0f;
			
			col.r=col.g=col.b=f;

			bitmap_ag_texture_write_pixel(ag_bitmap,ag_bitmap->spec_data,px,py,&col);
		}
	}
}

bool bitmap_ag_texture_finish(bitmap_ag_type *ag_bitmap,char *base_path)
{
	bool		ok;
	char		path[1024];

		// save file to disk
		
	sprintf(path,"%s/%s.png",base_path,ag_bitmap->frame->name);
	ok=bitmap_write_png_data(ag_bitmap->png_data,ag_bitmap->pixel_sz,ag_bitmap->pixel_sz,FALSE,path);

	if (ok) {
		sprintf(path,"%s/%s_n.png",base_path,ag_bitmap->frame->name);
		ok=bitmap_write_png_data(ag_bitmap->bump_data,ag_bitmap->pixel_sz,ag_bitmap->pixel_sz,FALSE,path);
	}

	if (ok) {
		sprintf(path,"%s/%s_s.png",base_path,ag_bitmap->frame->name);
		ok=bitmap_write_png_data(ag_bitmap->spec_data,ag_bitmap->pixel_sz,ag_bitmap->pixel_sz,FALSE,path);
	}

		// free data

	free(ag_bitmap->png_data);
	free(ag_bitmap->bump_data);
	free(ag_bitmap->spec_data);

	return(ok);
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
	d3vct		normal;

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

				if (x==lx) {
					normal.x=-0.89f;
					normal.y=0;
					normal.z=0.44f;
					bitmap_ag_texture_write_normal(ag_bitmap,ag_bitmap->bump_data,x,y,&normal);
					continue;
				}

				if (x==(rx-1)) {
					normal.x=0.89f;
					normal.y=0;
					normal.z=0.44f;
					bitmap_ag_texture_write_normal(ag_bitmap,ag_bitmap->bump_data,x,y,&normal);
					continue;
				}

				if (y==ty) {
					normal.x=0;
					normal.y=-0.89f;
					normal.z=0.44f;
					bitmap_ag_texture_write_normal(ag_bitmap,ag_bitmap->bump_data,x,y,&normal);
					continue;
				}

				if (y==(by-1)) {
					normal.x=0;
					normal.y=0.89f;
					normal.z=0.44f;
					bitmap_ag_texture_write_normal(ag_bitmap,ag_bitmap->bump_data,x,y,&normal);
					continue;
				}

			}
		}

		lx++;
		rx--;
		ty++;
		by--;
	}
}

bool bitmap_ag_texture_brick(texture_frame_type *frame,char *base_path,int pixel_sz)
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

	bitmap_ag_texture_add_noise(&ag_bitmap,0,0,pixel_sz,pixel_sz,0.8f,0.5f);

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
			bitmap_ag_random_color_lock(&base_col,200,127);
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
			bitmap_ag_texture_add_noise(&ag_bitmap,px,py,(col_sizes[col]-margin),(row_sizes[row]-margin),0.8f,0.3f);
			px+=col_sizes[col];
		}

		py+=row_sizes[row];
	}

		// save the texture

	bitmap_ag_texture_make_spec(&ag_bitmap,0.6f);
	return(bitmap_ag_texture_finish(&ag_bitmap,base_path));
}

/* =======================================================

      Tile Type Textures
      
======================================================= */

bool bitmap_ag_texture_tile(texture_frame_type *frame,char *base_path,int pixel_sz)
{
	int				x,y,px,py,wid,high,
					tile_style,split_count,sz;
	d3col			col[2],*t_col;
	bitmap_ag_type	ag_bitmap;

	ag_bitmap.pixel_sz=pixel_sz;
	ag_bitmap.frame=frame;
	ag_bitmap.back_col.r=ag_bitmap.back_col.g=ag_bitmap.back_col.b=0.0f;

	if (!bitmap_ag_texture_create(&ag_bitmap)) return(FALSE);

		// tile colors

	bitmap_ag_random_color(&col[0],200,200,200,64,64,64);
	bitmap_ag_random_color(&col[1],200,200,200,64,64,64);

		// tile style

	tile_style=bitmap_ag_random_int(3);
	
		// splits

	split_count=bitmap_ag_tile_start_split_count+bitmap_ag_random_int(bitmap_ag_tile_extra_split_count);
	sz=pixel_sz/split_count;

	py=0;

	for (y=0;y!=split_count;y++) {
		high=sz;
		if (y==(split_count-1)) high=pixel_sz-py;

		px=0;

		for (x=0;x!=split_count;x++) {
			wid=sz;
			if (x==(split_count-1)) wid=pixel_sz-px;

			t_col=&col[0];

			switch (tile_style) {

				case bitmap_ag_tile_style_border:
					if ((x!=0) && (y!=0)) t_col=&col[1];
					break;

				case bitmap_ag_tile_style_checker:
					t_col=&col[(x+y)%2];
					break;

				case bitmap_ag_tile_style_stripe:
					if ((x%2)!=0) t_col=&col[1];
					break;

			}

			bitmap_ag_texture_brick_draw_brick(&ag_bitmap,px,py,wid,high,5,t_col);
			px+=sz;
		}

		py+=sz;
	}
		
		// tile noise
		
	bitmap_ag_texture_add_noise(&ag_bitmap,0,0,pixel_sz,pixel_sz,1.2f,0.2f);

	bitmap_ag_texture_make_spec(&ag_bitmap,0.7f);
	return(bitmap_ag_texture_finish(&ag_bitmap,base_path));
}

/* =======================================================

      Metal Type Textures
      
======================================================= */

bool bitmap_ag_texture_metal(texture_frame_type *frame,char *base_path,int pixel_sz)
{
	bitmap_ag_type	ag_bitmap;

	ag_bitmap.pixel_sz=pixel_sz;
	ag_bitmap.frame=frame;
	bitmap_ag_random_color(&ag_bitmap.back_col,0,0,200,32,64,127);

	if (!bitmap_ag_texture_create(&ag_bitmap)) return(FALSE);
	
	bitmap_ag_texture_brick_draw_brick(&ag_bitmap,0,0,pixel_sz,pixel_sz,20,&ag_bitmap.back_col);

	bitmap_ag_texture_make_spec(&ag_bitmap,1.0f);
	return(bitmap_ag_texture_finish(&ag_bitmap,base_path));
}

/* =======================================================

      Wood Type Textures
      
======================================================= */

bool bitmap_ag_texture_wood(texture_frame_type *frame,char *base_path,int pixel_sz)
{
	d3col			dark_col;
	bitmap_ag_type	ag_bitmap;

	ag_bitmap.pixel_sz=pixel_sz;
	ag_bitmap.frame=frame;
	bitmap_ag_random_color(&ag_bitmap.back_col,127,50,0,100,40,0);

	if (!bitmap_ag_texture_create(&ag_bitmap)) return(FALSE);

		// inner boxes

	dark_col.r=ag_bitmap.back_col.r*0.7f;
	dark_col.g=ag_bitmap.back_col.g*0.7f;
	dark_col.b=ag_bitmap.back_col.b*0.7f;

	bitmap_ag_texture_brick_draw_brick(&ag_bitmap,0,0,pixel_sz,pixel_sz,3,&ag_bitmap.back_col);
	bitmap_ag_texture_add_noise(&ag_bitmap,0,0,pixel_sz,pixel_sz,0.9f,0.2f);

	bitmap_ag_texture_brick_draw_brick(&ag_bitmap,50,50,(pixel_sz-100),(pixel_sz-100),3,&dark_col);
	bitmap_ag_texture_add_noise(&ag_bitmap,50,50,(pixel_sz-100),(pixel_sz-100),0.9f,0.8f);

	bitmap_ag_texture_brick_draw_brick(&ag_bitmap,50,236,(pixel_sz-100),40,3,&ag_bitmap.back_col);
	bitmap_ag_texture_add_noise(&ag_bitmap,50,236,(pixel_sz-100),40,0.9f,0.2f);

	bitmap_ag_texture_brick_draw_brick(&ag_bitmap,236,50,40,(pixel_sz-100),3,&ag_bitmap.back_col);
	bitmap_ag_texture_add_noise(&ag_bitmap,236,50,40,(pixel_sz-100),0.9f,0.2f);

	bitmap_ag_texture_make_spec(&ag_bitmap,0.3f);
	return(bitmap_ag_texture_finish(&ag_bitmap,base_path));
}

/* =======================================================

      Cement Type Textures
      
======================================================= */

bool bitmap_ag_texture_cement(texture_frame_type *frame,char *base_path,int pixel_sz,bool dark)
{
	int				n,x,y,wid,high,mark_count;
	bitmap_ag_type	ag_bitmap;

	ag_bitmap.pixel_sz=pixel_sz;
	ag_bitmap.frame=frame;

	if (dark) {
		bitmap_ag_random_color_lock(&ag_bitmap.back_col,80,40);
	}
	else {
		bitmap_ag_random_color_lock(&ag_bitmap.back_col,140,100);
	}

	if (!bitmap_ag_texture_create(&ag_bitmap)) return(FALSE);

		// cement noise

	bitmap_ag_texture_add_noise(&ag_bitmap,0,0,pixel_sz,pixel_sz,(dark?0.8f:0.5f),0.8f);

		// marks

	mark_count=bitmap_ag_cement_start_mark_size+bitmap_ag_random_int(bitmap_ag_cement_extra_mark_size);

	for (n=0;n!=mark_count;n++) {
		x=bitmap_ag_random_int(pixel_sz);
		y=bitmap_ag_random_int(pixel_sz);
		wid=bitmap_ag_cement_start_mark_size+bitmap_ag_random_int(bitmap_ag_cement_extra_mark_size);
		high=bitmap_ag_cement_start_mark_size+bitmap_ag_random_int(bitmap_ag_cement_extra_mark_size);
		bitmap_ag_texture_damage_mark(&ag_bitmap,x,y,wid,high,(dark?0.8f:1.2f));
	}

		// save textures

	bitmap_ag_texture_make_spec(&ag_bitmap,0.5f);
	return(bitmap_ag_texture_finish(&ag_bitmap,base_path));
}

/* =======================================================

      Window Type Textures
      
======================================================= */

void bitmap_ag_texture_window_pane(bitmap_ag_type *ag_bitmap,int px,int py,int wid,int high,d3col *border_col,d3col *pane_col)
{
	bitmap_ag_texture_brick_draw_brick(ag_bitmap,px,py,wid,high,1,border_col);
	bitmap_ag_texture_add_noise(ag_bitmap,px,py,wid,high,0.6f,0.5f);

	bitmap_ag_texture_brick_draw_brick(ag_bitmap,(px+10),(py+10),(wid-20),(high-20),1,pane_col);
}

bool bitmap_ag_texture_window(texture_frame_type *frame,char *base_path,int pixel_sz)
{
	int				sz,margin;
	d3col			border_col,pane_col;
	bitmap_ag_type	ag_bitmap;

	ag_bitmap.pixel_sz=pixel_sz;
	ag_bitmap.frame=frame;
	bitmap_ag_random_color_lock(&ag_bitmap.back_col,200,150);

	if (!bitmap_ag_texture_create(&ag_bitmap)) return(FALSE);

		// background noise

	bitmap_ag_texture_add_noise(&ag_bitmap,0,0,pixel_sz,pixel_sz,1.0f,0.3f);

		// panes

	sz=pixel_sz/2;
	margin=20;

	bitmap_ag_random_color_lock(&border_col,64,32);
	bitmap_ag_random_color(&pane_col,255,255,255,200,200,32);

	bitmap_ag_texture_window_pane(&ag_bitmap,margin,margin,(sz-margin),(sz-margin),&border_col,&pane_col);
	bitmap_ag_texture_window_pane(&ag_bitmap,margin,sz,(sz-margin),(sz-margin),&border_col,&pane_col);
	bitmap_ag_texture_window_pane(&ag_bitmap,sz,margin,(sz-margin),(sz-margin),&border_col,&pane_col);
	bitmap_ag_texture_window_pane(&ag_bitmap,sz,sz,(sz-margin),(sz-margin),&border_col,&pane_col);

		// save

	bitmap_ag_texture_make_spec(&ag_bitmap,0.9f);
	return(bitmap_ag_texture_finish(&ag_bitmap,base_path));
}

/* =======================================================

      Machine Type Textures
      
======================================================= */

bool bitmap_ag_texture_machine(texture_frame_type *frame,char *base_path,int pixel_sz)
{
	bitmap_ag_type	ag_bitmap;

	ag_bitmap.pixel_sz=pixel_sz;
	ag_bitmap.frame=frame;
	bitmap_ag_random_color(&ag_bitmap.back_col,0,0,255,64,127,127);

	if (!bitmap_ag_texture_create(&ag_bitmap)) return(FALSE);

	bitmap_ag_texture_make_spec(&ag_bitmap,1.0f);
	return(bitmap_ag_texture_finish(&ag_bitmap,base_path));
}
