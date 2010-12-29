/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Texture Edit View

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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "interface.h"
#include "dialog.h"

#define view_texture_item_high					140
#define view_texture_scroll_wheel_move			25

#define view_texture_frame_click_return_idx		100
#define view_texture_frame_click_delete_idx		101

int								view_texture_scroll_pos,
								view_texture_frame_click_idx;

extern int						tool_palette_pixel_sz,txt_palette_pixel_sz;
extern list_palette_type		item_palette,property_palette;

extern map_type					map;
extern file_path_setup_type		file_path_setup;
extern editor_setup_type		setup;
extern editor_state_type		state;

/* =======================================================

      Switching View
      
======================================================= */

void view_texture_switch(int texture_idx)
{
		// clicking again leaves

	if ((state.view_texture_idx!=-1) && (state.view_texture_idx==texture_idx)) {
		state.view_texture_idx=-1;
	}
	else {
		state.view_texture_idx=texture_idx;
	}
	
	view_texture_scroll_pos=0;
	view_texture_frame_click_idx=-1;

	main_wind_draw();
}

/* =======================================================

      View Texture Edit Draw
      
======================================================= */

void view_texture_get_box(d3rect *box)
{
	os_get_window_box(box);
	
	box->ty+=tool_palette_pixel_sz;
	box->by-=txt_palette_pixel_sz;
	box->rx-=item_palette.pixel_sz;
}

void view_texture_draw_bitmap(d3rect *box,char *name,unsigned long gl_id)
{
		// the bitmap

	if (gl_id!=-1) {

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,gl_id);

		glColor4f(1.0f,1.0f,1.0f,1.0f);

  		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);

		glBegin(GL_QUADS);
		glTexCoord2f(0.0f,0.0f);
		glVertex2i(box->lx,box->ty);
		glTexCoord2f(1.0f,0.0f);
		glVertex2i(box->rx,box->ty);
		glTexCoord2f(1.0f,1.0f);
		glVertex2i(box->rx,box->by);
		glTexCoord2f(0.0f,1.0f);
		glVertex2i(box->lx,box->by);
		glEnd();

		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

		glDisable(GL_TEXTURE_2D);
	}

		// the box

	glColor4f(0.0f,0.0f,0.0f,1.0f);

	glBegin(GL_LINE_LOOP);
	glVertex2i(box->lx,box->ty);
	glVertex2i(box->rx,box->ty);
	glVertex2i(box->rx,box->by);
	glVertex2i(box->lx,box->by);
	glEnd();

		// the name
		
	text_draw_center(((box->lx+box->rx)>>1),(box->by+15),12.0f,NULL,name);
}

void view_texture_draw_button(d3rect *box,char *title,float font_size,bool has_trig,int sel_idx)
{
	float			comp;
	
	comp=0.9f;
	if (view_texture_frame_click_idx==sel_idx) comp=0.8f;

	glBegin(GL_QUADS);
	glColor4f(comp,comp,comp,1.0f);
	glVertex2i(box->lx,box->ty);
	glVertex2i(box->rx,box->ty);
	glColor4f((comp-0.2f),(comp-0.2f),(comp-0.2f),1.0f);
	glVertex2i(box->rx,box->by);
	glVertex2i(box->lx,box->by);
	glEnd();

	if (has_trig) {
		glBegin(GL_TRIANGLES);
		glColor4f(0.2f,0.2f,1.0f,1.0f);
		glVertex2i((box->rx-20),(box->ty+5));
		glColor4f(0.0f,0.0f,0.8f,1.0f);
		glVertex2i((box->rx-5),(box->by-5));
		glVertex2i((box->rx-35),(box->by-5));
		glEnd();
	}
	
	glColor4f(0.0f,0.0f,0.0f,1.0f);

	if (has_trig) {
		glBegin(GL_LINE_LOOP);
		glVertex2i((box->rx-20),(box->ty+5));
		glVertex2i((box->rx-5),(box->by-5));
		glVertex2i((box->rx-35),(box->by-5));
		glEnd();
	}
	
	glBegin(GL_LINE_LOOP);
	glVertex2i(box->lx,box->ty);
	glVertex2i(box->rx,box->ty);
	glVertex2i(box->rx,box->by);
	glVertex2i(box->lx,box->by);
	glEnd();

	text_draw((box->lx+5),(box->by-5),font_size,NULL,title);
}

void view_texture_draw(void)
{
	int					n,ty,by,wid,high,frame_count;
	char				str[256];
	char				type_str[3][32]={"Opaque","Cut-Out","Transparent"};
	d3rect				wbox,box,tbox;
	texture_type		*texture;

	texture=&map.textures[state.view_texture_idx];

		// setup drawing

	os_get_window_box(&wbox);
	view_texture_get_box(&box);

	wid=box.rx-box.lx;
	high=box.by-box.ty;

	glEnable(GL_SCISSOR_TEST);
	glScissor(box.lx,(wbox.by-box.by),(box.rx-box.lx),(box.by-box.ty));

	glViewport(box.lx,(wbox.by-box.by),(box.rx-box.lx),(box.by-box.ty));
		
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho((GLdouble)0,(GLdouble)wid,(GLdouble)high,(GLdouble)0,-1.0,1.0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

		// draw setup

	glDisable(GL_DEPTH_TEST);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
	
		// get frame count
		
	frame_count=map_count_texture_frames(&map,state.view_texture_idx);

		// the textures

	for (n=0;n!=max_texture_frame;n++) {
		ty=(view_texture_item_high*n)-view_texture_scroll_pos;
		by=ty+view_texture_item_high;
		
			// the background
			
		if (n>frame_count) {
			glColor4f(0.5f,0.5f,0.5f,1.0f);
		}
		else {
			if (view_texture_frame_click_idx==n) {
				glColor4f(0.85f,0.85f,0.85f,1.0f);
			}
			else {
				glColor4f(0.95f,0.95f,0.95f,1.0f);
			}
		}
		
		glBegin(GL_QUADS);
		glVertex2i(0,ty);
		glVertex2i(wid,ty);
		glVertex2i(wid,by);
		glVertex2i(0,by);
		glEnd();

			// the separator

		glColor4f(0.0f,0.0f,0.0f,1.0f);

		glBegin(GL_LINES);
		glVertex2i(0,ty);
		glVertex2i(wid,ty);
		glEnd();

			// the frame

		sprintf(str,"Frame %d",n);
		text_draw(5,(ty+20),15.0f,NULL,str);

			// the textures

		tbox.lx=25;
		tbox.rx=tbox.lx+100;
		tbox.ty=ty+22;
		tbox.by=tbox.ty+100;
		view_texture_draw_bitmap(&tbox,"Bitmap",texture->frames[n].bitmap.gl_id);

		tbox.lx+=110;
		tbox.rx+=110;
		view_texture_draw_bitmap(&tbox,"Bump",texture->frames[n].bumpmap.gl_id);

		tbox.lx+=110;
		tbox.rx+=110;
		view_texture_draw_bitmap(&tbox,"Specular",texture->frames[n].specularmap.gl_id);

		tbox.lx+=110;
		tbox.rx+=110;
		view_texture_draw_bitmap(&tbox,"Glow",texture->frames[n].glowmap.gl_id);

			// the info

		str[0]=0x0;
	
		if (texture->frames[n].name[0]!=0x0) {
			sprintf(str,"Name: %s",texture->frames[n].name);
			text_draw(470,(ty+35),15.0f,NULL,str);

			sprintf(str,"Size: %dx%d",texture->frames[n].bitmap.wid,texture->frames[n].bitmap.high);
			text_draw(470,(ty+55),15.0f,NULL,str);

			sprintf(str,"Alpha: %s",type_str[texture->frames[n].bitmap.alpha_mode]);
			text_draw(470,(ty+75),15.0f,NULL,str);

			sprintf(str,"Wait: %d",texture->animate.wait[n]);
			text_draw(470,(ty+95),15.0f,NULL,str);
		}
		else {
			text_draw(470,(ty+35),15.0f,NULL,"Name: (empty)");
		}
		
			// delete button
			
		if ((n>0) && (n==(frame_count-1))) {
			tbox.lx=470;
			tbox.rx=570;
			tbox.ty=ty+100;
			tbox.by=tbox.ty+25;
	
			view_texture_draw_button(&tbox,"Delete Frame",15.0f,FALSE,view_texture_frame_click_delete_idx);
		}
	}

		// the return button

	tbox.lx=wid-120;
	tbox.rx=tbox.lx+120;
	tbox.ty=high-40;
	tbox.by=tbox.ty+40;
	
	view_texture_draw_button(&tbox,"Return",25.0f,TRUE,view_texture_frame_click_return_idx);
}

/* =======================================================

      Cursors and Scrolling
      
======================================================= */

bool view_texture_cursor(void)
{
	if (os_key_space_down()) {
		os_set_hand_cursor();
        return(TRUE);
    }

	return(FALSE);
}

void view_texture_scroll(int y_move)
{
	int			high;
	d3rect		box;

	view_texture_scroll_pos-=y_move;

	if (view_texture_scroll_pos<0) view_texture_scroll_pos=0;
	
	view_texture_get_box(&box);
	high=(view_texture_item_high*max_texture_frame)-(box.by-box.ty);
	if (view_texture_scroll_pos>high) view_texture_scroll_pos=high;

	main_wind_draw();
}

void view_texture_scroll_wheel(int delta)
{
	view_texture_scroll(delta*view_texture_scroll_wheel_move);
}

/* =======================================================

      Load New Textures
      
======================================================= */

bool view_texture_click_bitmap_open(char *bitmap_name)
{
    char				err_str[256],path[1024];
	
		// get bitmap
				
	if (!dialog_file_open_run("Open a Bitmap","Bitmaps/Textures","png",NULL,bitmap_name)) return(FALSE);
	
		// check bitmap
		
	file_paths_data(&file_path_setup,path,"Bitmaps/Textures",bitmap_name,"png");

	if (!bitmap_check(path,err_str)) {
		os_dialog_alert("Texture Error",err_str);
		return(FALSE);
	}
	
	return(TRUE);
}

/* =======================================================

      View Texture Scrolling Click
      
======================================================= */

void view_texture_click_scroll(void)
{
	int				org_y;
	d3pnt			pt;
	d3rect			box;

	org_y=-1;
	view_texture_get_box(&box);

	main_wind_draw();

	while (!os_track_mouse_location(&pt,&box)) {

		if (org_y==-1) org_y=pt.y;

		if (pt.y!=org_y) {
			view_texture_scroll(pt.y-org_y);
			main_wind_draw();
			org_y=pt.y;
		}
		
		usleep(10000);
	}
}

/* =======================================================

      View Texture Edit Click
      
======================================================= */

bool view_texture_click(d3pnt *pnt,bool double_click)
{
	int				wid,high,frame_idx,frame_count;
    char			bitmap_name[file_str_len];
	d3pnt			pt;
	d3rect			box,tbox;
	texture_type	*texture;

		// check for scrolling

	if (os_key_space_down()) {
		view_texture_click_scroll();
		return(TRUE);
	}
	
		// regular clicks

	texture=&map.textures[state.view_texture_idx];
	frame_count=map_count_texture_frames(&map,state.view_texture_idx);

	view_texture_get_box(&box);

	wid=box.rx-box.lx;
	high=box.by-box.ty;

		// move within view
		
	pnt->x-=box.lx;
	pnt->y-=box.ty;

		// clicking in return?
		
	frame_idx=-1;

	tbox.lx=wid-120;
	tbox.rx=tbox.lx+120;
	tbox.ty=high-40;
	tbox.by=tbox.ty+40;

	if ((pnt->x>=tbox.lx) && (pnt->x<=tbox.rx) && (pnt->y>=tbox.ty) && (pnt->y<=tbox.by)) {
		frame_idx=view_texture_frame_click_return_idx;
	}
	
		// clicking in delete
		
	if ((frame_idx==-1) && (frame_count>1)) {
		tbox.lx=470;
		tbox.rx=570;
		tbox.ty=(view_texture_item_high*(frame_count-1))+100;
		tbox.by=tbox.ty+25;

		if ((pnt->x>=tbox.lx) && (pnt->x<=tbox.rx) && (pnt->y>=tbox.ty) && (pnt->y<=tbox.by)) {
			frame_idx=view_texture_frame_click_delete_idx;
		}
	}
	
		// clicking in item?
		
	if (frame_idx==-1) {
		frame_idx=pnt->y/view_texture_item_high;
		
		tbox.lx=0;
		tbox.rx=wid;
		tbox.ty=(frame_idx*view_texture_item_high);
		tbox.by=tbox.ty+view_texture_item_high;
		
		if ((frame_idx<0) || (frame_idx>=max_texture_frame)) return(FALSE);
	}
	
	view_texture_frame_click_idx=frame_idx;
		
		// run the click
		
	main_wind_draw();

	while (!os_track_mouse_location(&pt,&box)) {
		if ((pt.x<tbox.lx) || (pt.x>=tbox.rx) || (pt.y<tbox.ty) || (pt.y>=tbox.by)) {
			view_texture_frame_click_idx=-1;
		}
		else {
			view_texture_frame_click_idx=frame_idx;
		}

		main_wind_draw();
		
		usleep(10000);
	}

	if (view_texture_frame_click_idx==-1) return(FALSE);

	view_texture_frame_click_idx=-1;
	main_wind_draw();
		
		// return was clicked
		
	if (frame_idx==view_texture_frame_click_return_idx) {
		view_texture_switch(-1);
		return(TRUE);
	}
	
		// delete was clicked
		
	if (frame_idx==view_texture_frame_click_delete_idx) {
		os_set_wait_cursor();
		map_delete_texture_frame(&map,state.view_texture_idx);
		os_set_arrow_cursor();
		
		main_wind_draw();
		return(TRUE);
	}
	
		// can we edit this item?
		
	if (frame_idx>frame_count) {
		os_dialog_alert("Texture Frames","This texture frame can not be edited.\nYou can only add adjancent texture frames.");
		return(FALSE);
	}
	
		// item was clicked
		
	if (view_texture_click_bitmap_open(bitmap_name)) {
		strcpy(texture->frames[frame_idx].name,bitmap_name);
		os_set_wait_cursor();
		map_refresh_textures(&map);
		os_set_arrow_cursor();
	}

	main_wind_draw();

	return(TRUE);
}

