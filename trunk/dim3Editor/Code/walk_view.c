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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "interface.h"
#include "view.h"

extern int						top_view_x,top_view_z,
								tool_pixel_sz,txt_palette_pixel_sz;
extern d3rect					main_wind_box;

extern file_path_setup_type		file_path_setup;
extern map_type					map;
extern editor_setup_type		setup;
extern editor_state_type		state;

double							view_mod_matrix[16],view_proj_matrix[16];
GLint							view_vport[4];

bitmap_type						spot_bitmap,scenery_bitmap,node_bitmap,node_defined_bitmap,
								light_bitmap,sound_bitmap,particle_bitmap;

/* =======================================================

      Initialize and Shutdown Walk View
	        
======================================================= */

bool view_initialize(void)
{
	char			sub_path[1024],path[1024];

		// interface textures
		
	os_get_support_file_path(sub_path,"Editor");
	strcat(sub_path,"/Items");
		
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
	
		// some defaults
		
	state.view_select_idx=0;
	state.select_box_on=FALSE;

	return(TRUE);
}

void view_shutdown(void)
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

void view_setup_default_views(void)
{
	editor_view_type		*view;
	
	map.editor_views.count=1;
	
	state.view_select_idx=0;
	state.select_box_on=FALSE;
	
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

	view->cull=FALSE;
	view->ortho=FALSE;
	view->clip=FALSE;
}

/* =======================================================

      Get Panel Box
      
======================================================= */

void view_get_pixel_box(editor_view_type *view,d3rect *box)
{
	float			wid,high;
	d3rect			wbox;
	
		// get viewport
		
	os_get_window_box(&wbox);
	
	wbox.ty+=tool_pixel_sz;
	wbox.by-=txt_palette_pixel_sz;
	
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

void view_split_horizontal(void)
{
	float					mid;
	editor_view_type		*old_view,*view;

	if (map.editor_views.count>=max_editor_view) return;
	
	old_view=&map.editor_views.views[state.view_select_idx];
	
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
	
	state.view_select_idx=map.editor_views.count;
	map.editor_views.count++;
}

void view_split_vertical(void)
{
	float					mid;
	editor_view_type		*old_view,*view;

	if (map.editor_views.count>=max_editor_view) return;
	
	old_view=&map.editor_views.views[state.view_select_idx];
	
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
	
	state.view_select_idx=map.editor_views.count;
	map.editor_views.count++;
}

void view_split_remove(void)
{
	int						n,move_count;
	bool					moved;
	editor_view_box_type	box;
	editor_view_type		*view,*del_view;

	if (map.editor_views.count==1) return;
	
	del_view=&map.editor_views.views[state.view_select_idx];

		// get the view box, and slowly
		// remove parts of it to move the other
		// views around

	memmove(&box,&del_view->box,sizeof(editor_view_box_type));

	while (TRUE) {

		moved=FALSE;

		for (n=0;n!=map.editor_views.count;n++) {
			if (n==state.view_select_idx) continue;

			view=&map.editor_views.views[n];

			if ((view->box.top==box.top) && (view->box.bot<=box.bot)) {
				if (view->box.lft==box.rgt) {
					view->box.lft=box.lft;
					box.top=view->box.bot;
					moved=TRUE;
					break;
				}
				if (view->box.rgt==box.lft) {
					view->box.rgt=box.rgt;
					box.top=view->box.bot;
					moved=TRUE;
					break;
				}
			}

			if ((view->box.top>=box.top) && (view->box.bot==box.bot)) {
				if (view->box.lft==box.rgt) {
					view->box.lft=box.lft;
					box.bot=view->box.top;
					moved=TRUE;
					break;
				}
				if (view->box.rgt==box.lft) {
					view->box.rgt=box.rgt;
					box.bot=view->box.top;
					moved=TRUE;
					break;
				}
			}

			if ((view->box.lft==box.lft) && (view->box.rgt<=box.rgt)) {
				if (view->box.top==box.bot) {
					view->box.top=box.top;
					box.lft=view->box.rgt;
					moved=TRUE;
					break;
				}
				if (view->box.bot==box.top) {
					view->box.bot=box.bot;
					box.lft=view->box.rgt;
					moved=TRUE;
					break;
				}
			}

			if ((view->box.lft>=box.lft) && (view->box.rgt==box.rgt)) {
				if (view->box.top==box.bot) {
					view->box.top=box.top;
					box.rgt=view->box.lft;
					moved=TRUE;
					break;
				}
				if (view->box.bot==box.top) {
					view->box.bot=box.bot;
					box.rgt=view->box.lft;
					moved=TRUE;
					break;
				}
			}
		}

			// break if no movement

		if (!moved) break;

			// is box finished?

		if ((box.top==box.bot) || (box.lft==box.rgt)) break;
	}

		// remove old view

	move_count=(map.editor_views.count-state.view_select_idx)-1;
	if (move_count>=1) memmove(&map.editor_views.views[state.view_select_idx],&map.editor_views.views[state.view_select_idx+1],(sizeof(editor_view_type)*move_count));

	map.editor_views.count--;

		// new selection

	state.view_select_idx=0;
}

/* =======================================================

      Center Walk Views
      
======================================================= */

void view_center_all(bool reset_ang)
{
	int					n,old_sel_idx;
	d3pnt				pnt;
	d3ang				ang;
	
		// find center
		
	pnt.x=-1;
	
		// look for player spot first
		
	for (n=0;n!=map.nspot;n++) {
		if (map.spots[n].type==spot_type_player) {
			memmove(&pnt,&map.spots[n].pnt,sizeof(d3pnt));
			pnt.y-=(map_enlarge*20);
			break;
		}
	}

		// otherwise do first mesh with vertexes
		
	if (pnt.x==-1) {
	
		for (n=0;n!=map.mesh.nmesh;n++) {
			if (map.mesh.meshes[n].nvertex!=0) {
				map_mesh_calculate_center(&map,n,&pnt);
				break;
			}
		}
	
	}
	
		// just center in total map size
		
	if (pnt.x==-1) {
		pnt.x=map_max_size>>1;
		pnt.y=map_max_size>>1;
		pnt.z=map_max_size>>1;
	}
	
		// reset all the views
	
	old_sel_idx=state.view_select_idx;

	for (n=0;n!=map.editor_views.count;n++) {
	
		state.view_select_idx=n;
	
			// view angles
			
		if (reset_ang) {
			ang.x=0.0f;
			ang.y=0.0f;
			ang.z=0.0f;
			
			view_set_angle(&ang);
		}
		
			// view position
			
		view_set_position(&pnt);
	}
	
	state.view_select_idx=old_sel_idx;
}

/* =======================================================

      Projection Utilities
      
======================================================= */

void view_setup_project_point(void)
{
	glGetDoublev(GL_MODELVIEW_MATRIX,view_mod_matrix);
	glGetDoublev(GL_PROJECTION_MATRIX,view_proj_matrix);
	glGetIntegerv(GL_VIEWPORT,view_vport);
}

void view_project_point(editor_view_type *view,d3pnt *pnt)
{
	double			dx,dy,dz;
	d3rect			wbox;

	os_get_window_box(&wbox);

	gluProject(pnt->x,pnt->y,pnt->z,view_mod_matrix,view_proj_matrix,view_vport,&dx,&dy,&dz);

	pnt->x=((int)dx)-wbox.lx;
	pnt->y=wbox.by-(((int)dy)-wbox.ty);
}

bool view_project_point_in_z(d3pnt *pnt)
{
	return(((((double)pnt->x)*view_mod_matrix[2])+(((double)pnt->y)*view_mod_matrix[6])+(((double)pnt->z)*view_mod_matrix[10])+view_mod_matrix[14])>0.0);
}

/* =======================================================

      View Look At Point
      
======================================================= */

void view_get_lookat_point(editor_view_type *view,float dist,d3vct *look_vct)
{
	float				fx,fy,fz,ang_x;
	matrix_type			mat;
	
		// stop gimble effect on extreme X angle
		
	ang_x=view->ang.x;
	if ((ang_x<271.0f) && (ang_x>180.0f)) ang_x=271.0f;
	if ((ang_x>89.0f) && (ang_x<180.0f)) ang_x=89.0f;
	
		// create the lookat
		
	matrix_rotate_zyx(&mat,ang_x,view->ang.y,0.0f);

	fx=fy=0.0f;
	fz=-dist;
	matrix_vertex_multiply(&mat,&fx,&fy,&fz);

	look_vct->x=fx;
	look_vct->y=fy;
	look_vct->z=fz;
}

/* =======================================================

      Viewport and Projection Setup
      
======================================================= */

void view_set_viewport_box(d3rect *box,bool erase,bool use_background)
{
	d3rect			wbox;
	
		// set viewport
		
	os_get_window_box(&wbox);

	glEnable(GL_SCISSOR_TEST);
	glScissor(box->lx,(wbox.by-box->by),(box->rx-box->lx),(box->by-box->ty));

	glViewport(box->lx,(wbox.by-box->by),(box->rx-box->lx),(box->by-box->ty));
	
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

void view_set_viewport(editor_view_type *view,bool erase,bool use_background)
{
	d3rect			box;
	
	view_get_pixel_box(view,&box);
	view_set_viewport_box(&box,erase,use_background);
}

void view_set_2D_projection(editor_view_type *view)
{
	d3rect			box;
	
	view_get_pixel_box(view,&box);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho((GLdouble)box.lx,(GLdouble)box.rx,(GLdouble)box.by,(GLdouble)box.ty,-1.0,1.0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void view_set_3D_projection(editor_view_type *view,int near_z,int far_z,int near_z_offset)
{
	int				x_sz,y_sz;
	float			ratio;
	d3vct			look_vct;
	d3rect			box;
	
	view_get_pixel_box(view,&box);
	
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

		// create the look at vector

	view_get_lookat_point(view,(float)near_z_offset,&look_vct);

	gluLookAt((((float)view->pnt.x)+look_vct.x),(((float)view->pnt.y)+look_vct.y),(((float)view->pnt.z)+look_vct.z),(float)view->pnt.x,(float)view->pnt.y,(float)view->pnt.z,0.0f,1.0f,0.0f);
	
		// save projection
		
	view_setup_project_point();
}

/* =======================================================

      Find Views
      
======================================================= */

editor_view_type* view_get_current_view(void)
{
	return(&map.editor_views.views[state.view_select_idx]);
}

bool view_point_in_view(editor_view_type *view,d3pnt *pnt)
{
	d3rect			box;
	
	view_get_pixel_box(view,&box);
	
	if (pnt->x<box.lx) return(FALSE);
	if (pnt->x>box.rx) return(FALSE);
	if (pnt->y<box.ty) return(FALSE);
	return(pnt->y<=box.by);
}

/* =======================================================

      Cursors
      
======================================================= */

void view_cursor(d3pnt *pnt)
{
 	int					n;
	bool				in_view;
	
		// only change cursor if in view
		
	in_view=FALSE;
	
	for (n=0;n!=map.editor_views.count;n++) {
		if (view_point_in_view(&map.editor_views.views[n],pnt)) {
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
	
    if ((os_key_option_down()) && (!os_key_control_down())) {
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

void view_key(char ch)
{
		// tab switches view
		
	if (ch==0x9) {
		state.view_select_idx++;
		if (state.view_select_idx>=map.editor_views.count) state.view_select_idx=0;
		main_wind_draw();
		return;
	}
	
		// esc key deselects
		
	if (ch==0x1B) {
		select_clear();
		main_wind_draw();
		return;
	}
	
		// piece movement

	piece_key(ch);
}

/* =======================================================

      View Position and Angles
      
======================================================= */

void view_get_position(d3pnt *pnt)
{
	memmove(pnt,&map.editor_views.views[state.view_select_idx].pnt,sizeof(d3pnt));
}

void view_set_position(d3pnt *pnt)
{
	memmove(&map.editor_views.views[state.view_select_idx].pnt,pnt,sizeof(d3pnt));
}

void view_set_position_y_shift(d3pnt *pnt,int y_shift)
{
	memmove(&map.editor_views.views[state.view_select_idx].pnt,pnt,sizeof(d3pnt));
	map.editor_views.views[state.view_select_idx].pnt.y+=y_shift;
}

void view_move_position(d3pnt *pnt)
{
	map.editor_views.views[state.view_select_idx].pnt.x+=pnt->x;
	map.editor_views.views[state.view_select_idx].pnt.y+=pnt->y;
	map.editor_views.views[state.view_select_idx].pnt.z+=pnt->z;
}

void view_get_angle(d3ang *ang)
{
	memmove(ang,&map.editor_views.views[state.view_select_idx].ang,sizeof(d3ang));
}

void view_set_angle(d3ang *ang)
{
	d3ang			*vang;
	
	vang=&map.editor_views.views[state.view_select_idx].ang;

	memmove(vang,ang,sizeof(d3ang));
	
	if ((vang->x>90.0f) && (vang->x<180.0f)) vang->x=90.0f;
	if ((vang->x<270.0f) && (vang->x>180.0f)) vang->x=270.0f;
}

void view_turn_angle(d3ang *ang)
{
	d3ang			*vang;
	
	vang=&map.editor_views.views[state.view_select_idx].ang;
	
	vang->x=angle_add(vang->x,ang->x);
	vang->y=angle_add(vang->y,ang->y);
	vang->z=angle_add(vang->z,ang->z);
	
	if ((vang->x>90.0f) && (vang->x<180.0f)) vang->x=90.0f;
	if ((vang->x<270.0f) && (vang->x>180.0f)) vang->x=270.0f;
}

/* =======================================================

      View Perspective, Cull, UV Layer
      
======================================================= */

void view_perspective_ortho(bool on)
{
	map.editor_views.views[state.view_select_idx].ortho=on;
}

void view_cull(bool on)
{
	map.editor_views.views[state.view_select_idx].cull=on;
}

void view_clip(bool on)
{
	map.editor_views.views[state.view_select_idx].clip=on;
}

void view_flip_clip(void)
{
	map.editor_views.views[state.view_select_idx].clip=!map.editor_views.views[state.view_select_idx].clip;
}

int view_get_uv_layer(void)
{
	return(map.editor_views.views[state.view_select_idx].uv_layer);
}

void view_set_uv_layer(int uv_layer)
{
	map.editor_views.views[state.view_select_idx].uv_layer=uv_layer;
}

/* =======================================================

      View Goto
      
======================================================= */

void view_goto_select(void)
{
	float			fx,fy,fz;
	d3ang			ang;
	d3pnt			pnt;
	matrix_type		mat;
	
	if (select_count()==0) return;
	
	view_get_angle(&ang);
	
	select_get_center(&pnt);
	
	matrix_rotate_zyx(&mat,ang.x,ang.y,0.0f);

	fx=fy=0.0f;
	fz=(map_enlarge*100);
	matrix_vertex_multiply(&mat,&fx,&fy,&fz);

	pnt.x+=(int)fx;
	pnt.y+=(int)fy;
	pnt.z+=(int)fz;
	
	view_set_position(&pnt);
}

void view_goto_map_center(void)
{
	int				n,k,count;
	d3pnt			center,*pt;
	map_mesh_type	*mesh;
	
	count=0;
	center.x=center.y=center.z=0;
	
	mesh=map.mesh.meshes;
	
	for (n=0;n!=map.mesh.nmesh;n++) {
	
		pt=mesh->vertexes;
		for (k=0;k!=mesh->nvertex;k++) {
			center.x+=pt->x;
			center.y+=pt->y;
			center.z+=pt->z;
			pt++;
			count++;
		}
		
		mesh++;
	}
	
	if (count==0) return;
	
	center.x/=count;
	center.y/=count;
	center.z/=count;
	
	view_set_position(&center);
}

void view_goto_map_center_all(void)
{
	int			n,old_idx;

	old_idx=state.view_select_idx;

	for (n=0;n!=map.editor_views.count;n++) {
		state.view_select_idx=n;
		view_goto_map_center();
	}

	state.view_select_idx=old_idx;
}

/* =======================================================

      View Facing
      
======================================================= */

void view_face_front(void)
{
	d3ang			ang;
	
	ang.x=0.0f;
	ang.y=0.0f;
	ang.z=0.0f;
	
	view_set_angle(&ang);
}

void view_face_left(void)
{
	d3ang			ang;
	
	ang.x=0.0f;
	ang.y=90.0f;
	ang.z=0.0f;
	
	view_set_angle(&ang);
}

void view_face_right(void)
{
	d3ang			ang;
	
	ang.x=0.0f;
	ang.y=270.0f;
	ang.z=0.0f;
	
	view_set_angle(&ang);
}

void view_face_back(void)
{
	d3ang			ang;
	
	ang.x=0.0f;
	ang.y=180.0f;
	ang.z=0.0f;
	
	view_set_angle(&ang);
}

void view_face_top(void)
{
	d3ang			ang;
	
	ang.x=270.0f;
	ang.y=0.0f;
	ang.z=0.0f;
	
	view_set_angle(&ang);
}

void view_face_bottom(void)
{
	d3ang			ang;
	
	ang.x=90.0f;
	ang.y=0.0f;
	ang.z=0.0f;
	
	view_set_angle(&ang);
}

/* =======================================================

      Clicking
      
======================================================= */

void view_select_view(d3pnt *pnt)
{
	int					n,old_idx;
	
		// find selection
		
	old_idx=state.view_select_idx;
	
	for (n=0;n!=map.editor_views.count;n++) {
		if (view_point_in_view(&map.editor_views.views[n],pnt)) {
			state.view_select_idx=n;
			break;
		}
	}
	
		// if select changed, we need to update
		// interface
		
	if (state.view_select_idx!=old_idx) {
		menu_update_view();
	}
}

bool view_click(d3pnt *pnt,bool dblclick)
{
	editor_view_type	*view;
	
		// select clicking view
		
	view_select_view(pnt);
	
		// handle click

	view=view_get_current_view();
	if (!view_point_in_view(view,pnt)) return(FALSE);
		
		// scrolling and movement clicks
		
	if (os_key_space_down()) {
		view_mouse_scroll_movement(view,pnt);
		return(TRUE);
	}

	if (os_key_option_down()) {
		view_mouse_forward_movement(view,pnt);
		return(TRUE);
	}

	if (os_key_command_down()) {
		view_mouse_turn(view,pnt);
		return(TRUE);
	}

		// click the view pieces

	view_click_piece(view,pnt,dblclick);
	return(TRUE);
}

/* =======================================================

      Walk View Draw
      
======================================================= */

void view_draw(void)
{
	int					n;
	d3rect				box;
	editor_view_type	*view;
	
		// draw the views
		
	for (n=0;n!=map.editor_views.count;n++) {
		view_draw_view(&map.editor_views.views[n]);
	}
	
	view_set_viewport_box(&main_wind_box,FALSE,FALSE);

		// view box outlines
		
	for (n=0;n!=map.editor_views.count;n++) {
		view=&map.editor_views.views[n];
		view_get_pixel_box(view,&box);

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
		
	if ((state.view_select_idx>=0) && (state.view_select_idx<map.editor_views.count)) {
	
		view=&map.editor_views.views[state.view_select_idx];
		view_get_pixel_box(view,&box);
		
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



