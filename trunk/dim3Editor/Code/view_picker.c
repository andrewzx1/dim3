/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: View Picking

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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3editor.h"
#endif

#include "interface.h"
#include "common_view.h"
#include "walk_view.h"

extern int					txt_pixel_sz;
extern d3rect				main_wind_box;

extern map_type				map;
extern setup_type			setup;
extern editor_state_type	state;

int							pick_count,pick_col[3];
view_picker_type			*picks;

/* =======================================================

      View Picking Start/Stop
      
======================================================= */

bool view_pick_list_start(editor_view_type *view,bool in_2D,int count)
{
		// start picker list

	picks=(view_picker_type*)malloc(sizeof(view_picker_type)*count);
	if (picks==NULL) return(FALSE);

		// no picks set yet

	pick_count=0;

		// start the pick color

	pick_col[0]=0x1;
	pick_col[1]=0x0;
	pick_col[2]=0x0;

		// start the back buffer drawing
		// disable smoothing so colors stay true

	walk_view_set_viewport(view,FALSE,FALSE);
	
	if (!in_2D) {
		walk_view_set_3D_projection(view,map.settings.editor.view_near_dist,map.settings.editor.view_far_dist,walk_view_near_offset);
	}
	else {
		walk_view_set_2D_projection(view);
	}
	
	glClearColor(0.0f,0.0f,1.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glDisable(GL_SMOOTH);
	glDisable(GL_BLEND);

	glDisable(GL_TEXTURE_2D);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glDisable(GL_ALPHA_TEST);

	return(TRUE);
}

void view_pick_list_end(editor_view_type *view,d3pnt *pnt,int *type,int *main_idx,int *sub_idx)
{
	int					n;
	unsigned char		pixel[3];
	d3pnt				click_pnt;
	d3rect				box;
	view_picker_type	*pick;

		// flush the drawing

	glFinish();

		// need to make click point global
		// and swap Y for OpenGL read

	walk_view_get_pixel_box(view,&box);

	click_pnt.x=pnt->x+box.lx;
	click_pnt.y=pnt->y+box.ty;

	os_get_window_box(&box);

	click_pnt.y=box.by-click_pnt.y;

		// find the color at the clicked point

	glReadPixels(click_pnt.x,click_pnt.y,1,1,GL_RGB,GL_UNSIGNED_BYTE,(void*)pixel);

		// find which pick it represents
		
	*type=-1;

	pick=picks;

	for (n=0;n!=pick_count;n++) {

		if ((pick->col[0]==pixel[0]) && (pick->col[1]==pixel[1]) && (pick->col[2]==pixel[2])) {
			*type=pick->type;
			*main_idx=pick->main_idx;
			*sub_idx=pick->sub_idx;
			break;
		}

		pick++;
	}

	free(picks);

		// restore smoothing

	glEnable(GL_SMOOTH);
}

int view_pick_list_end_multiple(editor_view_type *view,d3pnt *start_pnt,d3pnt *end_pnt,int *type,int *main_idx,int *sub_idx,int max_item)
{
	int					n,k,x,y,lx,rx,ty,by,
						wid,high,pixel_count,
						item_count,p_type,p_main_idx,p_sub_idx;
	unsigned char		*pixel,*pixels;
	bool				hit;
	d3rect				box;
	view_picker_type	*pick;

		// flush the drawing

	glFinish();

		// get the starting position and size

	walk_view_get_pixel_box(view,&box);

	lx=start_pnt->x+box.lx;
	rx=end_pnt->x+box.lx;

	ty=start_pnt->y+box.ty;
	by=end_pnt->y+box.ty;

	if (lx<box.lx) lx=box.lx;
	if (rx>=box.rx) rx=box.rx-1;
	if (ty<box.ty) ty=box.ty;
	if (by>=box.by) by=box.by-1;

	if (lx<rx) {
		x=lx;
		wid=rx-lx;
	}
	else {
		x=rx;
		wid=lx-rx;
	}

	if (ty<by) {
		y=by;
		high=by-ty;
	}
	else {
		y=ty;
		high=ty-by;
	}

	if ((wid<=0) || (high<=0)) {
		free(picks);
		glEnable(GL_SMOOTH);
		return(0);
	}

		// swap Y for OpenGL read

	os_get_window_box(&box);

	y=box.by-y;

		// get pixel data

	pixel_count=wid*high;

	pixels=(unsigned char*)malloc(pixel_count*3);
	if (pixels==NULL) {
		free(picks);
		glEnable(GL_SMOOTH);
		return(0);
	}

	glPixelStoref(GL_PACK_ALIGNMENT,1);
	glReadPixels(x,y,wid,high,GL_RGB,GL_UNSIGNED_BYTE,(void*)pixels);

		// run through the pixels

	item_count=0;
	pixel=pixels;

	for (k=0;k!=pixel_count;k++) {

			// find color in pick list

		p_type=-1;

		pick=picks;

		for (n=0;n!=pick_count;n++) {

			if ((pick->col[0]==*pixel) && (pick->col[1]==*(pixel+1)) && (pick->col[2]==*(pixel+2))) {
				p_type=pick->type;
				p_main_idx=pick->main_idx;
				p_sub_idx=pick->sub_idx;
				break;
			}

			pick++;
		}

			// next color

		pixel+=3;

			// is it in list already?

		if (p_type==-1) continue;

		hit=FALSE;

		for (n=0;n!=item_count;n++) {
			if ((type[n]==p_type) && (main_idx[n]==p_main_idx) && (sub_idx[n]==p_sub_idx)) {
				hit=TRUE;
				break;
			}
		}

		if (hit) continue;

			// add to list

		if (item_count==max_item) break;

		type[item_count]=p_type;
		main_idx[item_count]=p_main_idx;
		sub_idx[item_count]=p_sub_idx;

		item_count++;
	}

		// free memory

	free(pixels);
	free(picks);

		// restore smoothing

	glEnable(GL_SMOOTH);

		// return item count

	return(item_count);
}

/* =======================================================

      View Picking Adding
      
======================================================= */

void view_pick_list_add(int type,int main_idx,int sub_idx)
{
	view_picker_type	*pick;

		// set color

	glColor3ub((unsigned char)pick_col[0],(unsigned char)pick_col[1],(unsigned char)pick_col[2]);

		// remember color in pick list

	pick=&picks[pick_count];

	pick->type=type;
	pick->main_idx=(short)main_idx;
	pick->sub_idx=(short)sub_idx;

	pick->col[0]=(unsigned char)pick_col[0];
	pick->col[1]=(unsigned char)pick_col[1];
	pick->col[2]=(unsigned char)pick_col[2];

		// next color

	pick_col[0]++;
	if (pick_col[0]>0xFF) {
		pick_col[0]=0x0;
		pick_col[1]++;
		if (pick_col[1]>0xFF) {
			pick_col[1]=0x0;
			pick_col[2]++;
		}
	}

		// next picker

	pick_count++;
}

void view_pick_list_add_cube(d3pnt *v_pnts,int type,int main_idx,int sub_idx)
{
	int				n,idx,quad_list[24]={0,1,2,3,4,5,6,7,3,2,5,6,0,1,4,7,0,7,6,3,1,2,5,4};

		// set the color

	view_pick_list_add(type,main_idx,sub_idx);

		// draw the cube

	glBegin(GL_QUADS);
	
	for (n=0;n!=24;n++) {
		idx=quad_list[n];
		glVertex3i(v_pnts[idx].x,v_pnts[idx].y,v_pnts[idx].z);
	}

	glEnd();
}

void view_pick_list_add_handle(int x,int y,int z,int type,int main_idx,int sub_idx)
{
		// set the color

	view_pick_list_add(type,main_idx,sub_idx);

		// draw the point

	glPointSize(view_handle_size);
	
	glBegin(GL_POINTS);
	glVertex3i(x,y,z);
	glEnd();
	
	glPointSize(1.0f);
}

void view_pick_list_add_2D_handle(int x,int y,int type,int main_idx,int sub_idx)
{
		// set the color

	view_pick_list_add(type,main_idx,sub_idx);

		// draw the point

	glPointSize(view_handle_size);
	
	glBegin(GL_POINTS);
	glVertex2i(x,y);
	glEnd();
	
	glPointSize(1.0f);
}

