/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Auto Generate Textures

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

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3baseutility.h"
#endif

#define bitmap_ag_max_brick_count					20
#define bitmap_ag_start_brick_row_size				40
#define bitmap_ag_extra_brick_row_size				80
#define bitmap_ag_start_brick_margin				2
#define bitmap_ag_extra_brick_margin				4
#define bitmap_ag_start_brick_lip					4
#define bitmap_ag_extra_brick_lip					6
#define bitmap_ag_brick_large_color_factor			0.15f
#define bitmap_ag_brick_small_color_factor			0.05f

#define bitmap_ag_start_block_row_count				4
#define bitmap_ag_extra_block_row_count				2
#define bitmap_ag_block_color_factor				0.05f

#define bitmap_ag_tile_start_split_count			2
#define bitmap_ag_tile_extra_split_normal_count		3
#define bitmap_ag_tile_extra_split_complex_count	1
#define bitmap_ag_tile_complex_percent				80
#define bitmap_ag_tile_design_percent				90

#define bitmap_ag_tile_style_border					0
#define bitmap_ag_tile_style_checker				1
#define bitmap_ag_tile_style_stripe					2

#define bitmap_ag_metal_start_split_count			1
#define bitmap_ag_metal_extra_split_normal_count	3
#define bitmap_ag_metal_darken_factor				0.75f
#define bitmap_ag_metal_lighten_factor				1.25f

#define bitmap_ag_cement_mark_count					50
#define bitmap_ag_cement_start_mark_size			20
#define bitmap_ag_cement_extra_mark_size			50

/* =======================================================

      Brick Type Textures
      
======================================================= */

void bitmap_ag_texture_brick_single(bitmap_ag_type *ag_bitmap,int px,int py,int wid,int high,int margin,d3col *base_col,float color_factor)
{
	int				n,rx,ry,r_wid,rough_count,lip_sz,
					tot_wid;
	d3col			brick_col;
	
		// the brick

	bitmap_ag_random_color_change(base_col,&brick_col,color_factor);
	lip_sz=bitmap_ag_start_brick_lip+bitmap_ag_random_int(bitmap_ag_extra_brick_lip);
	bitmap_ag_texture_draw_rectangle(ag_bitmap,px,py,(wid-margin),(high-margin),lip_sz,FALSE,&brick_col);

		// rough effect

	rough_count=4+bitmap_ag_random_int(6);
	tot_wid=(wid-margin)-(lip_sz<<1);

	for (n=0;n!=rough_count;n++) {
		rx=(px+lip_sz)+bitmap_ag_random_int(tot_wid>>1);
		r_wid=bitmap_ag_random_int(tot_wid>>1);
		ry=(py+lip_sz)+bitmap_ag_random_int((high-margin)-((lip_sz<<1)+5));
		bitmap_ag_texture_add_ridge_horizontal(ag_bitmap,rx,ry,r_wid,2,0.9f,0.8f);
	}

		// brick noise

	bitmap_ag_texture_add_noise(ag_bitmap,px,py,(wid-margin),(high-margin),0.8f,0.5f);
}

bool bitmap_ag_texture_brick(texture_frame_type *frame,char *base_path,int pixel_sz)
{
	int				px,py,wid,high,
					row,col,row_count,col_count,
					margin;
	d3col			base_col;
	bitmap_ag_type	ag_bitmap;

	ag_bitmap.pixel_sz=pixel_sz;
	ag_bitmap.no_bump_spec=FALSE;
	ag_bitmap.frame=frame;
	bitmap_ag_random_color_lock(&ag_bitmap.back_col,127,64);		// grout

	if (!bitmap_ag_texture_create(&ag_bitmap,FALSE)) return(FALSE);

		// grout noise

	bitmap_ag_texture_add_noise(&ag_bitmap,0,0,pixel_sz,pixel_sz,0.8f,0.5f);

		// margin

	margin=bitmap_ag_start_brick_margin+bitmap_ag_random_int(bitmap_ag_extra_brick_margin);

		// brick base color

	switch (bitmap_ag_random_int(3)) {
		case 0:
			bitmap_ag_random_color(&base_col,150,40,40,127,0,0);
			break;
		case 1:
			bitmap_ag_random_color(&base_col,127,50,0,90,40,0);
			break;
		default:
			bitmap_ag_random_color(&base_col,50,50,80,40,40,60);
			break;
	}
	
		// brick sizes

	high=bitmap_ag_start_brick_row_size+bitmap_ag_random_int(bitmap_ag_extra_brick_row_size);
	row_count=pixel_sz/high;
	if (row_count<0) row_count=0;
	high=pixel_sz/row_count;
		
	wid=high<<1;
	col_count=pixel_sz/wid;
	if (col_count<0) col_count=1;
	wid=pixel_sz/col_count;

		// draw the bricks

	py=margin;

	for (row=0;row!=row_count;row++) {

		px=margin;
		if ((row%2)!=0) px+=(wid>>1);

		for (col=0;col!=col_count;col++) {
			bitmap_ag_texture_brick_single(&ag_bitmap,px,py,wid,high,margin,&base_col,bitmap_ag_brick_large_color_factor);
			px+=wid;
		}

		py+=high;
	}

		// save the texture

	bitmap_ag_texture_make_spec(&ag_bitmap,0.3f);
	return(bitmap_ag_texture_finish(&ag_bitmap,base_path));
}

/* =======================================================

      Interlaced Brick Type Textures
      
======================================================= */

bool bitmap_ag_texture_brick_interlaced(texture_frame_type *frame,char *base_path,int pixel_sz)
{
	int				margin;
	d3col			base_col;
	bitmap_ag_type	ag_bitmap;

	ag_bitmap.pixel_sz=pixel_sz;
	ag_bitmap.no_bump_spec=FALSE;
	ag_bitmap.frame=frame;
	bitmap_ag_random_color_lock(&ag_bitmap.back_col,127,64);		// grout

	if (!bitmap_ag_texture_create(&ag_bitmap,FALSE)) return(FALSE);

		// grout noise

	bitmap_ag_texture_add_noise(&ag_bitmap,0,0,pixel_sz,pixel_sz,0.8f,0.5f);

		// margin

	margin=bitmap_ag_start_brick_margin+bitmap_ag_random_int(bitmap_ag_extra_brick_margin);

		// brick base color

	switch (bitmap_ag_random_int(3)) {
		case 0:
			bitmap_ag_random_color(&base_col,150,40,40,127,0,0);
			break;
		case 1:
			bitmap_ag_random_color(&base_col,127,50,0,90,40,0);
			break;
		default:
			bitmap_ag_random_color(&base_col,50,50,80,40,40,60);
			break;
	}
	
		// draw the bricks
		
	bitmap_ag_texture_brick_single(&ag_bitmap,0,0,(pixel_sz>>1),(pixel_sz>>2),margin,&base_col,bitmap_ag_brick_small_color_factor);
	bitmap_ag_texture_brick_single(&ag_bitmap,(pixel_sz>>1),0,(pixel_sz>>1),(pixel_sz>>2),margin,&base_col,bitmap_ag_brick_small_color_factor);
	bitmap_ag_texture_brick_single(&ag_bitmap,0,(pixel_sz>>2),(pixel_sz>>1),(pixel_sz>>2),margin,&base_col,bitmap_ag_brick_small_color_factor);
	bitmap_ag_texture_brick_single(&ag_bitmap,(pixel_sz>>1),(pixel_sz>>2),(pixel_sz>>1),(pixel_sz>>2),margin,&base_col,bitmap_ag_brick_small_color_factor);
	bitmap_ag_texture_brick_single(&ag_bitmap,0,(pixel_sz>>1),(pixel_sz>>2),(pixel_sz>>1),margin,&base_col,bitmap_ag_brick_small_color_factor);
	bitmap_ag_texture_brick_single(&ag_bitmap,(pixel_sz>>2),(pixel_sz>>1),(pixel_sz>>2),(pixel_sz>>1),margin,&base_col,bitmap_ag_brick_small_color_factor);
	bitmap_ag_texture_brick_single(&ag_bitmap,((pixel_sz>>2)*2),(pixel_sz>>1),(pixel_sz>>2),(pixel_sz>>1),margin,&base_col,bitmap_ag_brick_small_color_factor);
	bitmap_ag_texture_brick_single(&ag_bitmap,((pixel_sz>>2)*3),(pixel_sz>>1),(pixel_sz>>2),(pixel_sz>>1),margin,&base_col,bitmap_ag_brick_small_color_factor);

		// save the texture

	bitmap_ag_texture_make_spec(&ag_bitmap,0.3f);
	return(bitmap_ag_texture_finish(&ag_bitmap,base_path));
}

/* =======================================================

      Concrete Block Type Textures
      
======================================================= */

bool bitmap_ag_texture_concrete_block(texture_frame_type *frame,char *base_path,int pixel_sz)
{
	int				px,py,high,row,row_count,
					margin,lip_sz;
	d3col			base_col,block_col;
	bitmap_ag_type	ag_bitmap;

	ag_bitmap.pixel_sz=pixel_sz;
	ag_bitmap.no_bump_spec=FALSE;
	ag_bitmap.frame=frame;
	bitmap_ag_random_color_lock(&ag_bitmap.back_col,127,64);		// grout

	if (!bitmap_ag_texture_create(&ag_bitmap,FALSE)) return(FALSE);

		// grout noise

	bitmap_ag_texture_add_noise(&ag_bitmap,0,0,pixel_sz,pixel_sz,0.8f,0.5f);

		// margin

	margin=bitmap_ag_start_brick_margin+bitmap_ag_random_int(bitmap_ag_extra_brick_margin);

		// concrete is gray

	bitmap_ag_random_color_lock(&base_col,127,64);
	
		// brick sizes

	row_count=bitmap_ag_start_block_row_count+bitmap_ag_random_int(bitmap_ag_extra_block_row_count);
	high=pixel_sz/row_count;

		// draw the blocks

	py=margin;

	for (row=0;row!=row_count;row++) {
	
		px=margin;
		if ((row%2)!=0) px+=(pixel_sz>>1);

		bitmap_ag_random_color_change(&base_col,&block_col,bitmap_ag_block_color_factor);
		lip_sz=bitmap_ag_start_brick_lip+bitmap_ag_random_int(bitmap_ag_extra_brick_lip);
		bitmap_ag_texture_draw_rectangle(&ag_bitmap,px,py,(pixel_sz-margin),(high-margin),lip_sz,FALSE,&block_col);
		bitmap_ag_texture_add_noise(&ag_bitmap,px,py,(pixel_sz-margin),(high-margin),0.5f,0.7f);

		py+=high;
	}

		// save the texture

	bitmap_ag_texture_make_spec(&ag_bitmap,0.3f);
	return(bitmap_ag_texture_finish(&ag_bitmap,base_path));
}

/* =======================================================

      Tile Type Textures
      
======================================================= */

void bitmap_ag_texture_tile_inner(bitmap_ag_type *ag_bitmap,int split_count,int sx,int sy,int pixel_sz,d3col *cols,bool complex,bool design_ok)
{
	int				x,y,px,py,wid,high,
					tile_style,sz;
	d3col			*t_col,border_col;

		// tile style

	tile_style=bitmap_ag_random_int(3);
	
		// splits

	sz=pixel_sz/split_count;

	py=sy;

	for (y=0;y!=split_count;y++) {
		high=sz;
		if (y==(split_count-1)) high=pixel_sz-(py-sy);

		px=sx;

		for (x=0;x!=split_count;x++) {
			wid=sz;
			if (x==(split_count-1)) wid=pixel_sz-(px-sx);
			
				// if complex, we could create another tile set
				// inside this tile set
				
			if ((complex) && (bitmap_ag_random_int(100)>bitmap_ag_tile_complex_percent)) {
				bitmap_ag_texture_tile_inner(ag_bitmap,2,px,py,sz,cols,FALSE,FALSE);
			}
			
				// make the tile

			else {
				t_col=&cols[0];

				switch (tile_style) {

					case bitmap_ag_tile_style_border:
						if ((x!=0) && (y!=0)) t_col=&cols[1];
						break;

					case bitmap_ag_tile_style_checker:
						t_col=&cols[(x+y)%2];
						break;

					case bitmap_ag_tile_style_stripe:
						if ((x%2)!=0) t_col=&cols[1];
						break;

				}

				bitmap_ag_texture_draw_rectangle(ag_bitmap,px,py,wid,high,5,FALSE,t_col);

					// possible design

				if ((complex) && (bitmap_ag_random_int(100)>bitmap_ag_tile_design_percent)) {
					border_col.r=t_col->r*0.7f;
					border_col.g=t_col->g*0.7f;
					border_col.b=t_col->b*0.7f;
					bitmap_ag_texture_draw_oval(ag_bitmap,(px+5),(py+5),(wid-10),(high-10),5,&border_col,NULL);
				}
			}
			
			px+=sz;
		}

		py+=sz;
	}
}

bool bitmap_ag_texture_tile(texture_frame_type *frame,char *base_path,int pixel_sz,bool complex)
{
	int				split_count;
	d3col			cols[2];
	bitmap_ag_type	ag_bitmap;

	ag_bitmap.pixel_sz=pixel_sz;
	ag_bitmap.no_bump_spec=FALSE;
	ag_bitmap.frame=frame;
	ag_bitmap.back_col.r=ag_bitmap.back_col.g=ag_bitmap.back_col.b=0.0f;

	if (!bitmap_ag_texture_create(&ag_bitmap,FALSE)) return(FALSE);

		// tile colors

	bitmap_ag_random_color(&cols[0],120,120,150,64,64,80);
	
	cols[1].r=cols[0].r*0.8f;
	cols[1].g=cols[0].g*0.8f;
	cols[1].b=cols[0].b*0.8f;
	
		// original splits

	if (!complex) {
		split_count=bitmap_ag_tile_start_split_count+bitmap_ag_random_int(bitmap_ag_tile_extra_split_normal_count);
	}
	else {
		split_count=bitmap_ag_tile_start_split_count+bitmap_ag_random_int(bitmap_ag_tile_extra_split_complex_count);
	}

	bitmap_ag_texture_tile_inner(&ag_bitmap,split_count,0,0,pixel_sz,cols,complex,TRUE);

		// tile noise
		
	bitmap_ag_texture_add_noise(&ag_bitmap,0,0,pixel_sz,pixel_sz,1.2f,0.2f);

	bitmap_ag_texture_make_spec(&ag_bitmap,0.4f);
	return(bitmap_ag_texture_finish(&ag_bitmap,base_path));
}

/* =======================================================

      Metal Type Textures
      
======================================================= */

void bitmap_ag_texture_metal_pattern(bitmap_ag_type *ag_bitmap,int px,int py,int wid,int high,int mark_count)
{
	int				n,x,y,m_wid,m_high;

	for (n=0;n!=mark_count;n++) {
		x=(px+20)+bitmap_ag_random_int(wid-40);
		y=(py+20)+bitmap_ag_random_int(high-40);
		m_wid=10+bitmap_ag_random_int(10);
		m_high=10+bitmap_ag_random_int(10);
		bitmap_ag_texture_add_particle(ag_bitmap,x,y,m_wid,m_high,(((n%2)==0)?bitmap_ag_metal_darken_factor:bitmap_ag_metal_lighten_factor),TRUE);
	}
}

void bitmap_ag_texture_metal_bolts(bitmap_ag_type *ag_bitmap)
{
	int				x,y,px,py,wid,high,split_count,sz;
	d3col			col,border_col;

		// splits for panels

	split_count=bitmap_ag_metal_start_split_count+bitmap_ag_random_int(bitmap_ag_metal_extra_split_normal_count);
	sz=ag_bitmap->pixel_sz/split_count;

		// bolt colors

	col.r=ag_bitmap->back_col.r*0.7f;
	col.g=ag_bitmap->back_col.g*0.7f;
	col.b=ag_bitmap->back_col.b*0.7f;

	border_col.r=border_col.g=border_col.b=0.0f;

		// draw the panels

	py=0;

	for (y=0;y!=split_count;y++) {
		high=sz;
		if (y==(split_count-1)) high=ag_bitmap->pixel_sz-py;

		px=0;

		for (x=0;x!=split_count;x++) {
			wid=sz;
			if (x==(split_count-1)) wid=ag_bitmap->pixel_sz-px;
			
			bitmap_ag_texture_draw_rectangle(ag_bitmap,px,py,wid,high,15,TRUE,&ag_bitmap->back_col);
			bitmap_ag_texture_metal_pattern(ag_bitmap,px,py,wid,high,60);

			bitmap_ag_texture_draw_oval(ag_bitmap,(px+20),(py+20),20,20,3,&border_col,&col);
			bitmap_ag_texture_draw_oval(ag_bitmap,(px+(wid-40)),(py+20),20,20,3,&border_col,&col);
			bitmap_ag_texture_draw_oval(ag_bitmap,(px+(wid-40)),(py+(high-40)),20,20,3,&border_col,&col);
			bitmap_ag_texture_draw_oval(ag_bitmap,(px+20),(py+(high-40)),20,20,3,&border_col,&col);

			px+=sz;
		}

		py+=sz;
	}
}

void bitmap_ag_texture_metal_lines(bitmap_ag_type *ag_bitmap)
{
	int			n,y,sz,line_count;
	d3col		col;
	
	line_count=5+bitmap_ag_random_int(20);
	sz=ag_bitmap->pixel_sz/line_count;
	
	y=3;
	
	col.r=ag_bitmap->back_col.r*0.7f;
	col.g=ag_bitmap->back_col.g*0.7f;
	col.b=ag_bitmap->back_col.b*0.7f;
	
	for (n=0;n!=line_count;n++) {
		bitmap_ag_texture_draw_line_horizontal(ag_bitmap,0,y,ag_bitmap->pixel_sz,3,FALSE,&col);
		y+=sz;
	}
}

bool bitmap_ag_texture_metal(texture_frame_type *frame,char *base_path,int pixel_sz,bool bolts)
{
	int				n,mark_count,x,y,wid,high;
	bitmap_ag_type	ag_bitmap;

	ag_bitmap.pixel_sz=pixel_sz;
	ag_bitmap.no_bump_spec=FALSE;
	ag_bitmap.frame=frame;
	bitmap_ag_random_color(&ag_bitmap.back_col,0,0,150,64,64,100);

	if (!bitmap_ag_texture_create(&ag_bitmap,FALSE)) return(FALSE);

		// marks

	mark_count=bitmap_ag_cement_start_mark_size+bitmap_ag_random_int(bitmap_ag_cement_extra_mark_size);

	for (n=0;n!=mark_count;n++) {
		x=bitmap_ag_random_int(pixel_sz);
		y=bitmap_ag_random_int(pixel_sz);
		wid=bitmap_ag_cement_start_mark_size+bitmap_ag_random_int(bitmap_ag_cement_extra_mark_size);
		high=bitmap_ag_cement_start_mark_size+bitmap_ag_random_int(bitmap_ag_cement_extra_mark_size);
		bitmap_ag_texture_add_particle(&ag_bitmap,x,y,wid,high,0.9f,TRUE);
	}

		// bolt version

	if (bolts) {
		bitmap_ag_texture_metal_bolts(&ag_bitmap);
	}
	else {
		bitmap_ag_texture_metal_lines(&ag_bitmap);
		bitmap_ag_texture_metal_pattern(&ag_bitmap,0,0,pixel_sz,pixel_sz,200);
	}

		// metals have high spec

	bitmap_ag_texture_make_spec(&ag_bitmap,0.8f);

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
	ag_bitmap.no_bump_spec=FALSE;
	ag_bitmap.frame=frame;
	bitmap_ag_random_color(&ag_bitmap.back_col,127,50,0,100,40,0);

	if (!bitmap_ag_texture_create(&ag_bitmap,FALSE)) return(FALSE);

		// inner boxes

	dark_col.r=ag_bitmap.back_col.r*0.7f;
	dark_col.g=ag_bitmap.back_col.g*0.7f;
	dark_col.b=ag_bitmap.back_col.b*0.7f;

		// outside boards
		
	bitmap_ag_texture_draw_rectangle(&ag_bitmap,0,0,pixel_sz,50,3,FALSE,&ag_bitmap.back_col);
	bitmap_ag_texture_random_color_stripe_vertical(&ag_bitmap,3,3,(pixel_sz-6),44,&ag_bitmap.back_col,0.1f);
	bitmap_ag_texture_add_noise(&ag_bitmap,0,0,pixel_sz,50,0.9f,0.8f);

	bitmap_ag_texture_draw_rectangle(&ag_bitmap,0,(pixel_sz-50),pixel_sz,50,3,FALSE,&ag_bitmap.back_col);
	bitmap_ag_texture_random_color_stripe_vertical(&ag_bitmap,3,(pixel_sz-47),(pixel_sz-6),44,&ag_bitmap.back_col,0.1f);
	bitmap_ag_texture_add_noise(&ag_bitmap,0,(pixel_sz-50),pixel_sz,50,0.9f,0.8f);

	bitmap_ag_texture_draw_rectangle(&ag_bitmap,0,50,50,(pixel_sz-100),3,FALSE,&ag_bitmap.back_col);
	bitmap_ag_texture_random_color_stripe_horizontal(&ag_bitmap,3,53,44,(pixel_sz-106),&ag_bitmap.back_col,0.1f);
	bitmap_ag_texture_add_noise(&ag_bitmap,0,50,50,(pixel_sz-100),0.9f,0.8f);

	bitmap_ag_texture_draw_rectangle(&ag_bitmap,(pixel_sz-50),50,50,(pixel_sz-100),3,FALSE,&ag_bitmap.back_col);
	bitmap_ag_texture_random_color_stripe_horizontal(&ag_bitmap,(pixel_sz-47),53,44,(pixel_sz-106),&ag_bitmap.back_col,0.1f);
	bitmap_ag_texture_add_noise(&ag_bitmap,(pixel_sz-50),50,50,(pixel_sz-100),0.9f,0.8f);

		// inner boards
		
	bitmap_ag_texture_draw_rectangle(&ag_bitmap,50,50,(pixel_sz-100),(pixel_sz-100),3,FALSE,&dark_col);
	bitmap_ag_texture_random_color_stripe_slant(&ag_bitmap,53,53,(pixel_sz-106),(pixel_sz-106),&dark_col,0.05f);
	bitmap_ag_texture_add_noise(&ag_bitmap,50,50,(pixel_sz-100),(pixel_sz-100),0.9f,0.8f);
	
		// cross boards
	
	bitmap_ag_texture_draw_rectangle(&ag_bitmap,50,236,(pixel_sz-100),40,3,FALSE,&ag_bitmap.back_col);
	bitmap_ag_texture_random_color_stripe_vertical(&ag_bitmap,53,239,(pixel_sz-106),34,&ag_bitmap.back_col,0.1f);
	bitmap_ag_texture_add_noise(&ag_bitmap,50,236,(pixel_sz-100),40,0.9f,0.2f);

	bitmap_ag_texture_draw_rectangle(&ag_bitmap,236,50,40,(pixel_sz-100),3,FALSE,&ag_bitmap.back_col);
	bitmap_ag_texture_random_color_stripe_horizontal(&ag_bitmap,239,53,34,(pixel_sz-106),&ag_bitmap.back_col,0.1f);
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
	ag_bitmap.no_bump_spec=FALSE;
	ag_bitmap.frame=frame;

	if (dark) {
		bitmap_ag_random_color_lock(&ag_bitmap.back_col,80,40);
	}
	else {
		bitmap_ag_random_color_lock(&ag_bitmap.back_col,140,100);
	}

	if (!bitmap_ag_texture_create(&ag_bitmap,FALSE)) return(FALSE);

		// cement noise

	bitmap_ag_texture_add_noise(&ag_bitmap,0,0,pixel_sz,pixel_sz,(dark?0.8f:0.5f),0.8f);

		// marks

	mark_count=bitmap_ag_cement_start_mark_size+bitmap_ag_random_int(bitmap_ag_cement_extra_mark_size);

	for (n=0;n!=mark_count;n++) {
		x=bitmap_ag_random_int(pixel_sz);
		y=bitmap_ag_random_int(pixel_sz);
		wid=bitmap_ag_cement_start_mark_size+bitmap_ag_random_int(bitmap_ag_cement_extra_mark_size);
		high=bitmap_ag_cement_start_mark_size+bitmap_ag_random_int(bitmap_ag_cement_extra_mark_size);
		bitmap_ag_texture_add_particle(&ag_bitmap,x,y,wid,high,(dark?0.8f:1.2f),TRUE);
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
	bitmap_ag_texture_draw_rectangle(ag_bitmap,px,py,wid,high,1,FALSE,border_col);
	bitmap_ag_texture_add_noise(ag_bitmap,px,py,wid,high,0.6f,0.5f);

	bitmap_ag_texture_draw_rectangle(ag_bitmap,(px+10),(py+10),(wid-20),(high-20),1,TRUE,pane_col);
	bitmap_ag_texture_gradient_overlay_vertical(ag_bitmap,(px+10),(py+10),(wid-20),(high-20),1.0f,0.7f);
}

bool bitmap_ag_texture_window(texture_frame_type *frame,char *base_path,int pixel_sz)
{
	int				sz,margin;
	d3col			border_col,pane_col;
	bitmap_ag_type	ag_bitmap;

	ag_bitmap.pixel_sz=pixel_sz;
	ag_bitmap.no_bump_spec=FALSE;
	ag_bitmap.frame=frame;
	bitmap_ag_random_color_lock(&ag_bitmap.back_col,200,150);

	if (!bitmap_ag_texture_create(&ag_bitmap,FALSE)) return(FALSE);

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

void bitmap_ag_texture_machine_glow(bitmap_ag_type *ag_bitmap,int px,int py,int wid,int high)
{
	int			x,y;
	d3col		col;

		// glow color

	col.r=col.g=col.b=1.0f;

		// draw the glow

	for (y=py;y<(py+high);y++) {
		for (x=px;x<(px+wid);x++) {
			bitmap_ag_texture_write_pixel(ag_bitmap,ag_bitmap->glow_data,x,y,&col);
		}
	}
}

bool bitmap_ag_texture_machine(texture_frame_type *frame,char *base_path,int pixel_sz)
{
	int				n,y;
	d3col			border_col,glow_col[2];
	bitmap_ag_type	ag_bitmap;

	ag_bitmap.pixel_sz=pixel_sz;
	ag_bitmap.no_bump_spec=FALSE;
	ag_bitmap.frame=frame;
	bitmap_ag_random_color(&ag_bitmap.back_col,0,0,127,64,64,127);

	if (!bitmap_ag_texture_create(&ag_bitmap,TRUE)) return(FALSE);

		// metal look

	bitmap_ag_texture_draw_rectangle(&ag_bitmap,0,0,pixel_sz,pixel_sz,2,TRUE,&ag_bitmap.back_col);
	bitmap_ag_texture_metal_pattern(&ag_bitmap,0,0,pixel_sz,pixel_sz,300);
	
		// supergumba -- temp
		
	border_col.r=border_col.g=border_col.b=0.0f;
	glow_col[0].r=glow_col[0].b=0.0f;
	glow_col[0].g=1.0f;
	glow_col[1].g=glow_col[1].b=0.0f;
	glow_col[1].r=1.0f;
	
		// lights
		
	y=30;
	
	for (n=0;n!=5;n++) {
		bitmap_ag_texture_window_pane(&ag_bitmap,30,y,200,80,&border_col,&glow_col[n%2]);
		bitmap_ag_texture_machine_glow(&ag_bitmap,35,(y+5),190,70);
		y+=90;
	}

	bitmap_ag_texture_make_spec(&ag_bitmap,0.8f);
	return(bitmap_ag_texture_finish(&ag_bitmap,base_path));
}

/* =======================================================

      Skybox Type Textures
      
======================================================= */

void bitmap_ag_texture_skybox_start(bool night)
{
}

void bitmap_ag_texture_skybox_stars(bitmap_ag_type *ag_bitmap,int x,int y,int wid,int high)
{
	int				n,px,py;
	d3col			col;

	for (n=0;n!=100;n++) {
		px=x+bitmap_ag_random_int(wid);
		py=y+bitmap_ag_random_int(high);
		bitmap_ag_random_color_lock(&col,255,127);
		bitmap_ag_texture_write_pixel(ag_bitmap,ag_bitmap->png_data,px,py,&col);
	}
}

bool bitmap_ag_texture_skybox_top(texture_frame_type *frame,char *base_path,int pixel_sz)
{
	bitmap_ag_type	ag_bitmap;

	ag_bitmap.pixel_sz=pixel_sz;
	ag_bitmap.no_bump_spec=TRUE;
	ag_bitmap.frame=frame;
	ag_bitmap.back_col.r=0.1f;
	ag_bitmap.back_col.g=0.0f;
	ag_bitmap.back_col.b=0.5f;

	if (!bitmap_ag_texture_create(&ag_bitmap,TRUE)) return(FALSE);

	bitmap_ag_texture_skybox_stars(&ag_bitmap,0,0,pixel_sz,pixel_sz);

	return(bitmap_ag_texture_finish(&ag_bitmap,base_path));
}

bool bitmap_ag_texture_skybox_bottom(texture_frame_type *frame,char *base_path,int pixel_sz)
{
	bitmap_ag_type	ag_bitmap;

	ag_bitmap.pixel_sz=pixel_sz;
	ag_bitmap.no_bump_spec=TRUE;
	ag_bitmap.frame=frame;
	ag_bitmap.back_col.r=0.0f;
	ag_bitmap.back_col.g=0.0f;
	ag_bitmap.back_col.b=0.0f;

	if (!bitmap_ag_texture_create(&ag_bitmap,TRUE)) return(FALSE);
	return(bitmap_ag_texture_finish(&ag_bitmap,base_path));
}

bool bitmap_ag_texture_skybox_north(texture_frame_type *frame,char *base_path,int pixel_sz)
{
	bitmap_ag_type	ag_bitmap;

	ag_bitmap.pixel_sz=pixel_sz;
	ag_bitmap.no_bump_spec=TRUE;
	ag_bitmap.frame=frame;
	ag_bitmap.back_col.r=0.1f;
	ag_bitmap.back_col.g=0.0f;
	ag_bitmap.back_col.b=0.5f;

	if (!bitmap_ag_texture_create(&ag_bitmap,TRUE)) return(FALSE);

	bitmap_ag_texture_gradient_overlay_vertical(&ag_bitmap,0,0,pixel_sz,pixel_sz,1.0f,0.0f);
	bitmap_ag_texture_skybox_stars(&ag_bitmap,0,0,pixel_sz,(pixel_sz>>1));

	return(bitmap_ag_texture_finish(&ag_bitmap,base_path));
}

bool bitmap_ag_texture_skybox_south(texture_frame_type *frame,char *base_path,int pixel_sz)
{
	bitmap_ag_type	ag_bitmap;

	ag_bitmap.pixel_sz=pixel_sz;
	ag_bitmap.no_bump_spec=TRUE;
	ag_bitmap.frame=frame;
	ag_bitmap.back_col.r=0.1f;
	ag_bitmap.back_col.g=0.0f;
	ag_bitmap.back_col.b=0.5f;

	if (!bitmap_ag_texture_create(&ag_bitmap,TRUE)) return(FALSE);

	bitmap_ag_texture_gradient_overlay_vertical(&ag_bitmap,0,0,pixel_sz,pixel_sz,1.0f,0.0f);
	bitmap_ag_texture_skybox_stars(&ag_bitmap,0,0,pixel_sz,(pixel_sz>>1));

	return(bitmap_ag_texture_finish(&ag_bitmap,base_path));
}

bool bitmap_ag_texture_skybox_east(texture_frame_type *frame,char *base_path,int pixel_sz)
{
	bitmap_ag_type	ag_bitmap;

	ag_bitmap.pixel_sz=pixel_sz;
	ag_bitmap.no_bump_spec=TRUE;
	ag_bitmap.frame=frame;
	ag_bitmap.back_col.r=0.1f;
	ag_bitmap.back_col.g=0.0f;
	ag_bitmap.back_col.b=0.5f;

	if (!bitmap_ag_texture_create(&ag_bitmap,TRUE)) return(FALSE);

	bitmap_ag_texture_gradient_overlay_vertical(&ag_bitmap,0,0,pixel_sz,pixel_sz,1.0f,0.0f);
	bitmap_ag_texture_skybox_stars(&ag_bitmap,0,0,pixel_sz,(pixel_sz>>1));

	return(bitmap_ag_texture_finish(&ag_bitmap,base_path));
}

bool bitmap_ag_texture_skybox_west(texture_frame_type *frame,char *base_path,int pixel_sz)
{
	bitmap_ag_type	ag_bitmap;

	ag_bitmap.pixel_sz=pixel_sz;
	ag_bitmap.no_bump_spec=TRUE;
	ag_bitmap.frame=frame;
	ag_bitmap.back_col.r=0.1f;
	ag_bitmap.back_col.g=0.0f;
	ag_bitmap.back_col.b=0.5f;

	if (!bitmap_ag_texture_create(&ag_bitmap,TRUE)) return(FALSE);

	bitmap_ag_texture_gradient_overlay_vertical(&ag_bitmap,0,0,pixel_sz,pixel_sz,1.0f,0.0f);
	bitmap_ag_texture_skybox_stars(&ag_bitmap,0,0,pixel_sz,(pixel_sz>>1));

	return(bitmap_ag_texture_finish(&ag_bitmap,base_path));
}
