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

d3pnt							view_pnt;
d3ang							view_ang;
bitmap_type						spot_bitmap,scenery_bitmap,node_bitmap,node_defined_bitmap,
								light_bitmap,sound_bitmap,particle_bitmap;
								
int								view_count,view_select_idx;
editor_3D_view_setup			views[max_editor_views];

/* =======================================================

      Initialize and Shutdown Walk View
	        
======================================================= */

bool walk_view_initialize(void)
{
	char			path[1024];
	
		// interface textures
		
	file_paths_app(&file_path_setup,path,"Contents/Resources/Icons","spot","png");
	bitmap_open(&spot_bitmap,path,anisotropic_mode_none,mipmap_mode_none,texture_quality_mode_high,FALSE,FALSE,FALSE,FALSE,FALSE);

	file_paths_app(&file_path_setup,path,"Contents/Resources/Icons","scenery","png");
	bitmap_open(&scenery_bitmap,path,anisotropic_mode_none,mipmap_mode_none,texture_quality_mode_high,FALSE,FALSE,FALSE,FALSE,FALSE);
	
	file_paths_app(&file_path_setup,path,"Contents/Resources/Icons","node","png");
	bitmap_open(&node_bitmap,path,anisotropic_mode_none,mipmap_mode_none,texture_quality_mode_high,FALSE,FALSE,FALSE,FALSE,FALSE);
	
	file_paths_app(&file_path_setup,path,"Contents/Resources/Icons","node_defined","png");
	bitmap_open(&node_defined_bitmap,path,anisotropic_mode_none,mipmap_mode_none,texture_quality_mode_high,FALSE,FALSE,FALSE,FALSE,FALSE);
	
	file_paths_app(&file_path_setup,path,"Contents/Resources/Icons","light","png");
	bitmap_open(&light_bitmap,path,anisotropic_mode_none,mipmap_mode_none,texture_quality_mode_high,FALSE,FALSE,FALSE,FALSE,FALSE);

	file_paths_app(&file_path_setup,path,"Contents/Resources/Icons","sound","png");
	bitmap_open(&sound_bitmap,path,anisotropic_mode_none,mipmap_mode_none,texture_quality_mode_high,FALSE,FALSE,FALSE,FALSE,FALSE);

	file_paths_app(&file_path_setup,path,"Contents/Resources/Icons","particle","png");
	bitmap_open(&particle_bitmap,path,anisotropic_mode_none,mipmap_mode_none,texture_quality_mode_high,FALSE,FALSE,FALSE,FALSE,FALSE);

		// supergumba -- testing view setup
		
	view_count=2;
	view_select_idx=0;
	
	main_wind_setup_panel_forward_full(&views[0]);
	views[0].box.rx=(views[0].box.lx+views[0].box.rx)>>1;
	
	main_wind_setup_panel_top_full(&views[1]);
	views[1].box.lx=(views[1].box.lx+views[1].box.rx)>>1;

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

      Viewport and Projection Setup
      
======================================================= */

void main_wind_set_viewport(d3rect *view_box,bool erase,bool use_background)
{
	int				bot_y;
	d3rect			wbox;
	
		// set viewport
		
	os_get_window_box(&wbox);
	bot_y=wbox.by-info_high;

	glEnable(GL_SCISSOR_TEST);
	glScissor(view_box->lx,(bot_y-view_box->by),(view_box->rx-view_box->lx),(view_box->by-view_box->ty));

	glViewport(view_box->lx,(bot_y-view_box->by),(view_box->rx-view_box->lx),(view_box->by-view_box->ty));
	
		// default setup
		
	glDisable(GL_DEPTH_TEST);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho((GLdouble)view_box->lx,(GLdouble)view_box->rx,(GLdouble)view_box->by,(GLdouble)view_box->ty,-1.0,1.0);
	
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
	glVertex2i(view_box->lx,view_box->ty);
	glVertex2i(view_box->rx,view_box->ty);
	glVertex2i(view_box->rx,view_box->by);
	glVertex2i(view_box->lx,view_box->by);
	glEnd();
}

void main_wind_set_2D_projection(editor_3D_view_setup *view_setup)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho((GLdouble)view_setup->box.lx,(GLdouble)view_setup->box.rx,(GLdouble)view_setup->box.by,(GLdouble)view_setup->box.ty,-1.0,1.0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void main_wind_set_3D_projection(editor_3D_view_setup *view_setup,int near_z,int far_z,int near_z_offset)
{
	int				x_sz,y_sz;
	float			ratio;
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	if (state.perspective==ps_perspective) {
		ratio=(float)(view_setup->box.rx-view_setup->box.lx)/(float)(view_setup->box.by-view_setup->box.ty);
		gluPerspective(view_setup->fov,ratio,(GLdouble)near_z,(GLdouble)far_z);
	}
	else {
		x_sz=(view_setup->box.rx-view_setup->box.lx)*(map_enlarge>>2);
		y_sz=(view_setup->box.by-view_setup->box.ty)*(map_enlarge>>2);
		glOrtho((GLdouble)-x_sz,(GLdouble)x_sz,(GLdouble)-y_sz,(GLdouble)y_sz,(GLdouble)near_z,(GLdouble)far_z);
	}
	
	glScalef(-1.0f,-1.0f,-1.0f);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glRotatef(-view_setup->ang.x,1.0f,0.0f,0.0f);
	glRotatef(-angle_add(view_setup->ang.y,180.0f),0.0f,1.0f,0.0f);
	
	glTranslatef(-view_setup->pnt.x,-view_setup->pnt.y,((-view_setup->pnt.z)+near_z_offset));
}

/* =======================================================

      Cursors
      
======================================================= */

void walk_view_cursor(bool rot_ok)
{
    if (os_key_space_down()) {
        os_set_hand_cursor();
        return;
    }
    if ((os_key_option_down()) && ((!os_key_control_down()) && (!os_key_shift_down()))) {
        os_set_drag_cursor();
        return;
    }
    if ((os_key_command_down()) && (rot_ok)) {
        os_set_drag_cursor();
        return;
    }
    
    os_set_arrow_cursor();
}

/* =======================================================

      Walk View Keys
      
======================================================= */

void walk_view_key(editor_3D_view_setup *view_setup,int view_move_dir,char ch)
{
	piece_key(view_setup,view_move_dir,ch);
}


/* =======================================================

      Clicking
      
======================================================= */

/*
void walk_view_click(editor_3D_view_setup *view_setup,d3pnt *pt,int view_move_dir,bool rot_ok,bool dblclick)
{
       // scrolling and movement keys
        
    if (os_key_space_down()) {
        walk_view_mouse_xy_movement(view_setup,pt,view_move_dir);
        return;
    }

    if (os_key_option_down()) {
        walk_view_mouse_yz_movement(view_setup,pt,view_move_dir);
        return;
    }

    if ((os_key_command_down()) && (rot_ok)) {
        walk_view_mouse_turn(pt);
        return;
    }

        // click the map pieces
    
    walk_view_click_piece(view_setup,pt,view_move_dir,dblclick);
}
*/

bool walk_view_click(d3pnt *pt,bool dblclick)
{
	int						n;
	editor_3D_view_setup	*view;

	for (n=0;n!=view_count;n++) {
		view=&views[n];
		
		view->pnt.x=view_pnt.x;
		view->pnt.y=view_pnt.y;
		view->pnt.z=view_pnt.z;

		if (!main_wind_click_check_box(pt,&view->box)) continue;
		
		   // scrolling and movement clicks
			
		if (os_key_space_down()) {
			walk_view_mouse_xy_movement(view,pt,vm_dir_forward);
			return(TRUE);
		}

		if (os_key_option_down()) {
			walk_view_mouse_yz_movement(view,pt,vm_dir_forward);
			return(TRUE);
		}

		if (os_key_command_down()) {
			walk_view_mouse_turn(pt);
			return(TRUE);
		}

			// click the view pieces
    
		walk_view_click_piece(view,pt,vm_dir_forward,dblclick);
		return(TRUE);
	}
	
	return(FALSE);
}

/* =======================================================

      Walk View Draw
      
======================================================= */

/* supergumba -- clean a bit of this up, no swap or stuff involved
typedef struct		{
						int						clip_y;
						d3rect					box;
						d3pnt					pnt;
						d3ang					ang;
						float					fov;
						bool					mesh_only,draw_light_circle,
												clip_on;
					} editor_3D_view_setup;
*/

void walk_view_draw(void)
{
	int						n;
	editor_3D_view_setup	*view;
	
	for (n=0;n!=view_count;n++) {
		view=&views[n];
	
			// draw the view
			
		view->pnt.x=view_pnt.x;
		view->pnt.y=view_pnt.y;
		view->pnt.z=view_pnt.z;
		walk_view_draw_view(view,FALSE);
		
			// view box
			
		main_wind_set_viewport(&main_wind_box,FALSE,FALSE);
		
		glColor4f(0.0f,0.0f,0.0f,1.0f);

		glBegin(GL_LINE_LOOP);
		glVertex2i((view->box.lx-1),(view->box.ty-1));
		glVertex2i((view->box.rx+1),(view->box.ty-1));
		glVertex2i((view->box.rx+1),(view->box.by+1));
		glVertex2i((view->box.lx-1),(view->box.by+1));
		glEnd();
		
			// selection
			
		if (n!=view_select_idx) continue;
		
		glEnable(GL_BLEND);
		
		glColor4f(0.8f,0.8f,0.8f,0.5f);

		glBegin(GL_QUADS);
		
			// top
			
		glVertex2i(view->box.lx,view->box.ty);
		glVertex2i(view->box.rx,view->box.ty);
		glVertex2i(view->box.rx,(view->box.ty+view_selection_size));
		glVertex2i(view->box.lx,(view->box.ty+view_selection_size));
		
			// bottom
			
		glVertex2i(view->box.lx,(view->box.by-view_selection_size));
		glVertex2i(view->box.rx,(view->box.by-view_selection_size));
		glVertex2i(view->box.rx,view->box.by);
		glVertex2i(view->box.lx,view->box.by);
		
			// left
			
		glVertex2i(view->box.lx,(view->box.ty+view_selection_size));
		glVertex2i((view->box.lx+view_selection_size),(view->box.ty+view_selection_size));
		glVertex2i((view->box.lx+view_selection_size),(view->box.by-view_selection_size));
		glVertex2i(view->box.lx,(view->box.by-view_selection_size));

			// right
			
		glVertex2i((view->box.rx-view_selection_size),(view->box.ty+view_selection_size));
		glVertex2i(view->box.rx,(view->box.ty+view_selection_size));
		glVertex2i(view->box.rx,(view->box.by-view_selection_size));
		glVertex2i((view->box.rx-view_selection_size),(view->box.by-view_selection_size));

		glEnd();
		
		glDisable(GL_BLEND);
	}
}



