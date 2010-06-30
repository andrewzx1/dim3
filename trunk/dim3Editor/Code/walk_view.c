/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Main Walk View Routines

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

extern int						top_view_x,top_view_z,txt_palette_high;
extern d3rect					main_wind_box;

extern file_path_setup_type		file_path_setup;
extern map_type					map;
extern setup_type				setup;
extern editor_state_type		state;

bitmap_type						spot_bitmap,scenery_bitmap,node_bitmap,node_defined_bitmap,
								light_bitmap,sound_bitmap,particle_bitmap;
								
int								view_select_idx;

/* =======================================================

      Initialize and Shutdown Walk View
	        
======================================================= */

bool walk_view_initialize(void)
{
	char			sub_path[1024],path[1024];

		// on OS X, icons are stored as resources
		// in the bundle.  Everywhere else it's a folder

#ifdef D3_OS_MAC
	strcpy(sub_path,"Contents/Resources/Icons");
#else
	strcpy(sub_path,"dim3 Editor Icons");
#endif
	
		// interface textures
		
	file_paths_app(&file_path_setup,path,sub_path,"spot","png");
	bitmap_open(&spot_bitmap,path,anisotropic_mode_none,mipmap_mode_none,texture_quality_mode_high,FALSE,FALSE,FALSE,FALSE,FALSE);

	file_paths_app(&file_path_setup,path,sub_path,"scenery","png");
	bitmap_open(&scenery_bitmap,path,anisotropic_mode_none,mipmap_mode_none,texture_quality_mode_high,FALSE,FALSE,FALSE,FALSE,FALSE);
	
	file_paths_app(&file_path_setup,path,sub_path,"node","png");
	bitmap_open(&node_bitmap,path,anisotropic_mode_none,mipmap_mode_none,texture_quality_mode_high,FALSE,FALSE,FALSE,FALSE,FALSE);
	
	file_paths_app(&file_path_setup,path,sub_path,"node_defined","png");
	bitmap_open(&node_defined_bitmap,path,anisotropic_mode_none,mipmap_mode_none,texture_quality_mode_high,FALSE,FALSE,FALSE,FALSE,FALSE);
	
	file_paths_app(&file_path_setup,path,sub_path,"light","png");
	bitmap_open(&light_bitmap,path,anisotropic_mode_none,mipmap_mode_none,texture_quality_mode_high,FALSE,FALSE,FALSE,FALSE,FALSE);

	file_paths_app(&file_path_setup,path,sub_path,"sound","png");
	bitmap_open(&sound_bitmap,path,anisotropic_mode_none,mipmap_mode_none,texture_quality_mode_high,FALSE,FALSE,FALSE,FALSE,FALSE);

	file_paths_app(&file_path_setup,path,sub_path,"particle","png");
	bitmap_open(&particle_bitmap,path,anisotropic_mode_none,mipmap_mode_none,texture_quality_mode_high,FALSE,FALSE,FALSE,FALSE,FALSE);

	return(TRUE);
}

void walk_view_shutdown(void)
{
		// interface textures
		
    bitmap_close(&spot_bitmap);
    bitmap_close(&scenery_bitmap);
    bitmap_close(&node_bitmap);
    bitmap_close(&node_defined_bitmap);
    bitmap_close(&light_bitmap);
    bitmap_close(&sound_bitmap);
    bitmap_close(&particle_bitmap);
}

/* =======================================================

      Default View Setup
      
======================================================= */

void walk_view_setup_default_views(void)
{
	editor_view_type		*view;
	
	map.editor_views.count=1;
	view_select_idx=0;
	
	view=&map.editor_views.views[0];
	
	view->box.lft=0.0f;
	view->box.rgt=1.0f;
	view->box.top=0.0f;
	view->box.bot=1.0f;

	view->pnt.x=map_max_size/2;
	view->pnt.y=map_max_size/2;
	view->pnt.z=map_max_size/2;

	view->ang.x=0.0f;
	view->ang.y=0.0f;
	view->ang.z=0.0f;

	view->uv_layer=0;
	view->magnify_factor=magnify_factor_default;

	view->cull=FALSE;
	view->ortho=FALSE;
}

/* =======================================================

      Get Panel Box
      
======================================================= */

void walk_view_get_pixel_box(editor_view_type *view,d3rect *box)
{
	float			wid,high;
	d3rect			wbox;
	
		// get viewport
		
	os_get_window_box(&wbox);
	
	wbox.ty+=toolbar_high;
	wbox.by-=(txt_palette_high+info_high);
	
	wid=(float)(wbox.rx-wbox.lx);
	high=(float)(wbox.by-wbox.ty);
	
		// translate to pixels
	
	box->lx=wbox.lx+(int)(wid*view->box.lft);
	box->rx=wbox.lx+(int)(wid*view->box.rgt);
	box->ty=wbox.ty+(int)(high*view->box.top);
	box->by=wbox.ty+(int)(high*view->box.bot);
}

/* =======================================================

      Split and Remove Views
      
======================================================= */

void walk_view_split_horizontal(void)
{
	float					mid;
	editor_view_type		*old_view,*view;

	if (map.editor_views.count>=max_editor_view) return;
	
	old_view=&map.editor_views.views[view_select_idx];
	
	view=&map.editor_views.views[map.editor_views.count];
	
		// point and angle are the same
		
	memmove(&view->pnt,&old_view->pnt,sizeof(d3pnt));
	memmove(&view->ang,&old_view->ang,sizeof(d3ang));
	
		// split horizontal
		
	memmove(&view->box,&old_view->box,sizeof(editor_view_box_type));
		
	mid=old_view->box.lft+((old_view->box.rgt-old_view->box.lft)*0.5f);
	
	old_view->box.rgt=mid;
	view->box.lft=mid;

		// select new view
	
	view_select_idx=map.editor_views.count;
	map.editor_views.count++;
}

void walk_view_split_vertical(void)
{
	float					mid;
	editor_view_type		*old_view,*view;

	if (map.editor_views.count>=max_editor_view) return;
	
	old_view=&map.editor_views.views[view_select_idx];
	
	view=&map.editor_views.views[map.editor_views.count];
	
		// point and angle are the same
		
	memmove(&view->pnt,&old_view->pnt,sizeof(d3pnt));
	memmove(&view->ang,&old_view->ang,sizeof(d3ang));
	
		// split horizontal
		
	memmove(&view->box,&old_view->box,sizeof(editor_view_box_type));
		
	mid=old_view->box.top+((old_view->box.bot-old_view->box.top)*0.5f);
	
	old_view->box.bot=mid;
	view->box.top=mid;

		// select new view
	
	view_select_idx=map.editor_views.count;
	map.editor_views.count++;
}

void walk_view_remove(void)
{
	int						n,move_count;
	editor_view_type		*view,*del_view;

	if (map.editor_views.count==1) return;
	
	del_view=&map.editor_views.views[view_select_idx];

		// move all views that
		// border this view

	for (n=0;n!=map.editor_views.count;n++) {
		if (n==view_select_idx) continue;

		view=&map.editor_views.views[n];

		if ((view->box.top>=del_view->box.top) && (view->box.bot<=del_view->box.bot)) {
			if (view->box.lft==del_view->box.rgt) {
				view->box.lft=del_view->box.lft;
				continue;
			}
			if (view->box.rgt==del_view->box.lft) {
				view->box.rgt=del_view->box.rgt;
				continue;
			}
		}
		if ((view->box.lft>=del_view->box.lft) && (view->box.rgt<=del_view->box.rgt)) {
			if (view->box.top==del_view->box.bot) {
				view->box.top=del_view->box.top;
				continue;
			}
			if (view->box.bot==del_view->box.top) {
				view->box.bot=del_view->box.bot;
				continue;
			}
		}
	}

		// remove old view

	move_count=(map.editor_views.count-view_select_idx)-1;
	if (move_count>=1) memmove(&map.editor_views.views[view_select_idx],&map.editor_views.views[view_select_idx+1],(sizeof(editor_view_type)*move_count));

	map.editor_views.count--;
}

/* =======================================================

      Viewport and Projection Setup
      
======================================================= */

void walk_view_set_viewport_box(d3rect *box,bool erase,bool use_background)
{
	int				bot_y;
	d3rect			wbox;
	
		// set viewport
		
	os_get_window_box(&wbox);
	bot_y=wbox.by-info_high;

	glEnable(GL_SCISSOR_TEST);
	glScissor(box->lx,(bot_y-box->by),(box->rx-box->lx),(box->by-box->ty));

	glViewport(box->lx,(bot_y-box->by),(box->rx-box->lx),(box->by-box->ty));
	
		// default setup
		
	glDisable(GL_DEPTH_TEST);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho((GLdouble)box->lx,(GLdouble)box->rx,(GLdouble)box->by,(GLdouble)box->ty,-1.0,1.0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
		
		// erase viewport
		
	if (!erase) return;
		
	if (use_background) {
		glColor4f(setup.col.background.r,setup.col.background.g,setup.col.background.b,1.0f);
	}
	else {
		glColor4f(1.0f,1.0f,1.0f,1.0f);
	}
	
	glBegin(GL_QUADS);
	glVertex2i(box->lx,box->ty);
	glVertex2i(box->rx,box->ty);
	glVertex2i(box->rx,box->by);
	glVertex2i(box->lx,box->by);
	glEnd();
}

void walk_view_set_viewport(editor_view_type *view,bool erase,bool use_background)
{
	d3rect			box;
	
	walk_view_get_pixel_box(view,&box);
	walk_view_set_viewport_box(&box,erase,use_background);
}

void walk_view_set_2D_projection(editor_view_type *view)
{
	d3rect			box;
	
	walk_view_get_pixel_box(view,&box);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho((GLdouble)box.lx,(GLdouble)box.rx,(GLdouble)box.by,(GLdouble)box.ty,-1.0,1.0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void walk_view_set_3D_projection(editor_view_type *view,int near_z,int far_z,int near_z_offset)
{
	int				x_sz,y_sz;
	float			ratio;
	d3rect			box;
	
	walk_view_get_pixel_box(view,&box);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	if (!view->ortho) {
		ratio=(float)(box.rx-box.lx)/(float)(box.by-box.ty);
		gluPerspective(45.0f,ratio,(GLdouble)near_z,(GLdouble)far_z);
	}
	else {
		x_sz=(box.rx-box.lx)*(map_enlarge>>2);
		y_sz=(box.by-box.ty)*(map_enlarge>>2);
		glOrtho((GLdouble)-x_sz,(GLdouble)x_sz,(GLdouble)-y_sz,(GLdouble)y_sz,(GLdouble)near_z,(GLdouble)far_z);
	}
	
	glScalef(-1.0f,-1.0f,-1.0f);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glRotatef(-view->ang.x,1.0f,0.0f,0.0f);
	glRotatef(-angle_add(view->ang.y,180.0f),0.0f,1.0f,0.0f);
	
	glTranslatef((GLfloat)-view->pnt.x,(GLfloat)-view->pnt.y,(GLfloat)((-view->pnt.z)+near_z_offset));
}

/* =======================================================

      Find Views
      
======================================================= */

editor_view_type* walk_view_get_current_view(void)
{
	return(&map.editor_views.views[view_select_idx]);
}

bool walk_view_point_in_view(editor_view_type *view,d3pnt *pnt)
{
	d3rect			box;
	
	walk_view_get_pixel_box(view,&box);
	
	if (pnt->x<box.lx) return(FALSE);
	if (pnt->x>box.rx) return(FALSE);
	if (pnt->y<box.ty) return(FALSE);
	return(pnt->y<=box.by);
}

/* =======================================================

      Cursors
      
======================================================= */

void walk_view_cursor(d3pnt *pnt)
{
 	int					n;
	bool				in_view;
	
		// only change cursor if in view
		
	in_view=FALSE;
	
	for (n=0;n!=map.editor_views.count;n++) {
		if (walk_view_point_in_view(&map.editor_views.views[n],pnt)) {
			in_view=TRUE;
			break;
		}
	}
	
	if (!in_view) return;

		// setup cursor
		
	if (os_key_space_down()) {
        os_set_hand_cursor();
        return;
    }
	
    if ((os_key_option_down()) && ((!os_key_control_down()) && (!os_key_shift_down()))) {
        os_set_drag_cursor();
        return;
    }
	
    if (os_key_command_down()) {
        os_set_drag_cursor();
        return;
    }
    
    os_set_arrow_cursor();
}

/* =======================================================

      Walk View Keys
      
======================================================= */

void walk_view_key(char ch)
{
		// tab switches view
		
	if (ch==0x9) {
		view_select_idx++;
		if (view_select_idx>=map.editor_views.count) view_select_idx=0;
		walk_view_draw();
		return;
	}
	
		// esc key deselects
		
	if (ch==0x1B) {
		select_clear();
		walk_view_draw();
		return;
	}
	
		// piece movement

	piece_key(ch);
}

/* =======================================================

      View Position and Angles
      
======================================================= */

void walk_view_get_position(d3pnt *pnt)
{
	memmove(pnt,&map.editor_views.views[view_select_idx].pnt,sizeof(d3pnt));
}

void walk_view_set_position(d3pnt *pnt)
{
	memmove(&map.editor_views.views[view_select_idx].pnt,pnt,sizeof(d3pnt));
}

void walk_view_set_position_y_shift(d3pnt *pnt,int y_shift)
{
	memmove(&map.editor_views.views[view_select_idx].pnt,pnt,sizeof(d3pnt));
	map.editor_views.views[view_select_idx].pnt.y+=y_shift;
}

void walk_view_move_position(d3pnt *pnt)
{
	map.editor_views.views[view_select_idx].pnt.x+=pnt->x;
	map.editor_views.views[view_select_idx].pnt.y+=pnt->y;
	map.editor_views.views[view_select_idx].pnt.z+=pnt->z;
}

void walk_view_get_angle(d3ang *ang)
{
	memmove(ang,&map.editor_views.views[view_select_idx].ang,sizeof(d3ang));
}

void walk_view_set_angle(d3ang *ang)
{
	memmove(&map.editor_views.views[view_select_idx].ang,ang,sizeof(d3ang));
}

void walk_view_turn_angle(d3ang *ang)
{
	d3ang			*vang;
	
	vang=&map.editor_views.views[view_select_idx].ang;
	
	vang->x=angle_add(vang->x,ang->x);
	vang->y=angle_add(vang->y,ang->y);
	vang->z=angle_add(vang->z,ang->z);
	
	if ((vang->x>90.0f) && (vang->x<180.0f)) vang->x=90.0f;
	if ((vang->x<270.0f) && (vang->x>180.0f)) vang->x=270.0f;
}

/* =======================================================

      View Perspective, Cull, UV Layer, Magnify
      
======================================================= */

void walk_view_perspective_ortho(bool on)
{
	map.editor_views.views[view_select_idx].ortho=on;
}

void walk_view_cull(bool on)
{
	map.editor_views.views[view_select_idx].cull=on;
}

int walk_view_get_uv_layer(void)
{
	return(map.editor_views.views[view_select_idx].uv_layer);
}

void walk_view_set_uv_layer(int uv_layer)
{
	map.editor_views.views[view_select_idx].uv_layer=uv_layer;
}

int walk_view_get_magnify_factor(void)
{
	return(map.editor_views.views[view_select_idx].magnify_factor);
}

void walk_view_set_magnify_factor(int magnify_factor)
{
	map.editor_views.views[view_select_idx].magnify_factor=magnify_factor;
}

/* =======================================================

      View Facing
      
======================================================= */

void walk_view_face_front(void)
{
	d3ang			ang;
	
	ang.x=0.0f;
	ang.y=0.0f;
	ang.z=0.0f;
	
	walk_view_set_angle(&ang);
}

void walk_view_face_left(void)
{
	d3ang			ang;
	
	ang.x=0.0f;
	ang.y=90.0f;
	ang.z=0.0f;
	
	walk_view_set_angle(&ang);
}

void walk_view_face_right(void)
{
	d3ang			ang;
	
	ang.x=0.0f;
	ang.y=270.0f;
	ang.z=0.0f;
	
	walk_view_set_angle(&ang);
}

void walk_view_face_back(void)
{
	d3ang			ang;
	
	ang.x=0.0f;
	ang.y=180.0f;
	ang.z=0.0f;
	
	walk_view_set_angle(&ang);
}

void walk_view_face_top(void)
{
	d3ang			ang;
	
	ang.x=270.0f;
	ang.y=0.0f;
	ang.z=0.0f;
	
	walk_view_set_angle(&ang);
}

void walk_view_face_bottom(void)
{
	d3ang			ang;
	
	ang.x=90.0f;
	ang.y=0.0f;
	ang.z=0.0f;
	
	walk_view_set_angle(&ang);
}

/* =======================================================

      Clicking
      
======================================================= */

void walk_view_select_view(d3pnt *pnt)
{
	int					n,old_idx;
	
		// find selection
		
	old_idx=view_select_idx;
	
	for (n=0;n!=map.editor_views.count;n++) {
		if (walk_view_point_in_view(&map.editor_views.views[n],pnt)) {
			view_select_idx=n;
			break;
		}
	}
	
		// if select changed, we need to update
		// interface
		
	if (view_select_idx!=old_idx) {
		main_wind_tool_reset();
		menu_update_view();
	}
}

bool walk_view_click(d3pnt *pnt,bool dblclick)
{
	editor_view_type	*view;
	
		// select clicking view
		
	walk_view_select_view(pnt);
	
		// handle click

	view=&map.editor_views.views[view_select_idx];
	if (!walk_view_point_in_view(view,pnt)) return(FALSE);
		
		// scrolling and movement clicks
		
	if (os_key_space_down()) {
		walk_view_mouse_scroll_movement(pnt);
		return(TRUE);
	}

	if (os_key_option_down()) {
		walk_view_mouse_forward_movement(pnt);
		return(TRUE);
	}

	if (os_key_command_down()) {
		walk_view_mouse_turn(pnt);
		return(TRUE);
	}

		// click the view pieces

	walk_view_click_piece(view,pnt,dblclick);
	return(TRUE);
}

/* =======================================================

      Walk View Draw
      
======================================================= */

void walk_view_draw(void)
{
	int					n;
	d3rect				box;
	editor_view_type	*view;
	
		// draw the views
		
	for (n=0;n!=map.editor_views.count;n++) {
		walk_view_draw_view(&map.editor_views.views[n]);
	}
	
	walk_view_set_viewport_box(&main_wind_box,FALSE,FALSE);

		// view box outlines
		
	for (n=0;n!=map.editor_views.count;n++) {
		view=&map.editor_views.views[n];
		walk_view_get_pixel_box(view,&box);

		glLineWidth(1.0f);
		glColor4f(0.0f,0.0f,0.0f,1.0f);

		glBegin(GL_LINE_LOOP);
		glVertex2i((box.lx+1),(box.ty+1));
		glVertex2i((box.rx+1),(box.ty+1));
		glVertex2i((box.rx+1),(box.by+1));
		glVertex2i((box.lx+1),(box.by+1));
		glEnd();
	}
	
		// draw the selection
		
	if ((view_select_idx>=0) && (view_select_idx<map.editor_views.count)) {
	
		view=&map.editor_views.views[view_select_idx];
		walk_view_get_pixel_box(view,&box);
		
		glEnable(GL_BLEND);
		
		glColor4f(0.8f,0.8f,0.8f,0.5f);

		glBegin(GL_QUADS);
		
			// top
			
		glVertex2i(box.lx,box.ty);
		glVertex2i(box.rx,box.ty);
		glVertex2i(box.rx,(box.ty+view_selection_size));
		glVertex2i(box.lx,(box.ty+view_selection_size));
		
			// bottom
			
		glVertex2i(box.lx,(box.by-view_selection_size));
		glVertex2i(box.rx,(box.by-view_selection_size));
		glVertex2i(box.rx,box.by);
		glVertex2i(box.lx,box.by);
		
			// left
			
		glVertex2i(box.lx,(box.ty+view_selection_size));
		glVertex2i((box.lx+view_selection_size),(box.ty+view_selection_size));
		glVertex2i((box.lx+view_selection_size),(box.by-view_selection_size));
		glVertex2i(box.lx,(box.by-view_selection_size));

			// right
			
		glVertex2i((box.rx-view_selection_size),(box.ty+view_selection_size));
		glVertex2i(box.rx,(box.ty+view_selection_size));
		glVertex2i(box.rx,(box.by-view_selection_size));
		glVertex2i((box.rx-view_selection_size),(box.by-view_selection_size));

		glEnd();
		
		glDisable(GL_BLEND);
	}
}



