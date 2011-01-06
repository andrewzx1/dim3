/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Model Window Drawing

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
	#include "dim3animator.h"
#endif

#include "glue.h"
#include "interface.h"
#include "ui_common.h"

extern int					tool_palette_pixel_sz,txt_palette_pixel_sz;
extern bool					list_palette_open;

extern model_type			model;
extern model_draw_setup		draw_setup;
extern animator_state_type	state;

double						tran_mod_matrix[16],tran_proj_matrix[16],tran_vport[4];
d3rect						model_box,tran_wbox;

/* =======================================================

      Model Window Setup
      
======================================================= */

void model_wind_setup(void)
{
	d3rect			wbox;
	
	os_get_window_box(&wbox);
	
	model_box.lx=0;
	if (list_palette_open) {
		model_box.rx=wbox.rx-list_palette_tree_sz;
	}
	else {
		model_box.rx=wbox.rx-list_palette_border_sz;
	}
	model_box.ty=tool_palette_pixel_sz;
	model_box.by=(wbox.by-wbox.ty)-txt_palette_pixel_sz;
}

/* =======================================================

      Setup GL Draw
      
======================================================= */

void draw_model_gl_setup(int z_offset)
{
	int				yoff,sz;
	float			ratio;
	d3rect			wbox;
	
		// viewport setup
		
	os_get_window_box(&wbox);

	glEnable(GL_SCISSOR_TEST);
	glScissor(model_box.lx,(wbox.by-model_box.by),(model_box.rx-model_box.lx),(model_box.by-model_box.ty));

	glViewport(model_box.lx,(wbox.by-model_box.by),(model_box.rx-model_box.lx),(model_box.by-model_box.ty));

		// projection
		
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	ratio=(float)(model_box.rx-model_box.lx)/(float)(model_box.by-model_box.ty);
	gluPerspective(45.0,ratio,(float)(100+z_offset),(float)(25000-z_offset));
	glScalef(-1.0f,-1.0f,-1.0f);

	yoff=model.view_box.size.y/2;

	sz=500+((4000-state.magnify_z)*5);
	
	glTranslatef(-((GLfloat)state.shift.x),-((GLfloat)(state.shift.y-yoff)),(GLfloat)sz);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glRotatef(-state.ang.x,1.0f,0.0f,0.0f);
	glRotatef(angle_add(state.ang.y,180.0f),0.0f,1.0f,0.0f);

		// drawing setup
		
	glColor4f(1.0f,1.0f,1.0f,0.0f);
	
	glDisable(GL_SMOOTH);
	glDisable(GL_DITHER);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_ALPHA_TEST);
}

void draw_model_gl_setup_2D(void)
{
	d3rect				wbox;

	os_get_window_box(&wbox);

	glEnable(GL_SCISSOR_TEST);
	glScissor(wbox.lx,0,(wbox.rx-wbox.lx),(wbox.by-wbox.ty));

	glViewport(wbox.lx,0,(wbox.rx-wbox.lx),(wbox.by-wbox.ty));
		
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho((GLdouble)wbox.lx,(GLdouble)wbox.rx,(GLdouble)wbox.by,(GLdouble)wbox.ty,-1.0,1.0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

/* =======================================================

      3D to 2D Transform
      
======================================================= */

void draw_model_2D_transform_setup(void)
{
	os_get_window_box(&tran_wbox);
	
	glGetDoublev(GL_MODELVIEW_MATRIX,tran_mod_matrix);
	glGetDoublev(GL_PROJECTION_MATRIX,tran_proj_matrix);
	glGetIntegerv(GL_VIEWPORT,(GLint*)tran_vport);
}

void draw_model_2D_transform(d3fpnt *pnt,d3pnt *tran_pnt)
{
	double			dx,dy,dz;
	
	gluProject(pnt->x,pnt->y,pnt->z,tran_mod_matrix,tran_proj_matrix,(GLint*)tran_vport,&dx,&dy,&dz);
	tran_pnt->x=(int)dx;
	tran_pnt->y=(tran_wbox.by-((int)dy))-model_box.ty;
}

/* =======================================================

      Draw Model Bone and Vertex Setup
      
======================================================= */

void draw_model_setup_bones_vertexes(int mesh_idx)
{
		// create the drawing bones
	
	model_create_draw_bones(&model,&draw_setup);
	
		// calculate vertexes for first mesh if "show first mesh" is on
		
	if ((state.first_mesh) && (mesh_idx!=0)) {
		model_create_draw_vertexes(&model,0,&draw_setup);
		model_create_draw_normals(&model,0,&draw_setup);
	}

		// calculate vertexes for drawing mesh
		
	model_create_draw_vertexes(&model,mesh_idx,&draw_setup);
	model_create_draw_normals(&model,mesh_idx,&draw_setup);
}

/* =======================================================

      Draw Model Window
      
======================================================= */

void draw_model_wind(int mesh_idx)
{
	if (!state.model_open) return;

		// setup transformation to fit model in middle of screen
		
	draw_model_gl_setup(0);
	
		// draw memory
		
	model_draw_setup_initialize(&model,&draw_setup,TRUE);
	
		// draw the center
		
	draw_model_axis();

		// create the drawing bones, vertex arrays and normal arrays
		
	draw_model_setup_bones_vertexes(mesh_idx);
	
		// draw the mesh(es) in the current view
	
	if (state.texture) {
		if ((state.first_mesh) && (mesh_idx!=0)) draw_model(0);
		draw_model(mesh_idx);
	}
	
	if (state.mesh) {
		draw_model_gl_setup(1);
		if ((state.first_mesh) && (mesh_idx!=0)) draw_model_mesh(0);
		draw_model_mesh(mesh_idx);
		draw_model_gl_setup(0);
	}
	
	if (state.bone) {
		draw_model_bones(state.cur_bone_idx);
		draw_model_bone_names(state.cur_bone_idx);
		draw_model_gl_setup(0);			// names goes into 2D
	}
	
	if ((state.texture) || (state.mesh)) {
		draw_model_gl_setup(2);
		if (state.select_mode==select_mode_vertex) {
			draw_model_selected_vertexes(mesh_idx);
		}
		else {
			draw_model_selected_trig(mesh_idx);
		}
		draw_model_gl_setup(0);
	}
	
		// boxes
		
	if (state.hit_box) draw_model_box_hit_boxes();
	if (state.view_box) draw_model_box_view();
	
		// normals
		
	if (state.normal) {
		draw_model_gl_setup(3);
		if (state.select_mode==select_mode_vertex) {
			draw_model_normals_vertexes(mesh_idx);
		}
		else {
			draw_model_normals_trig(mesh_idx);
		}
		draw_model_gl_setup(0);
	}
	
		// free memory
		
	model_draw_setup_shutdown(&model,&draw_setup);
	
		// 2D drawing
		
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho((GLdouble)model_box.lx,(GLdouble)model_box.rx,(GLdouble)model_box.by,(GLdouble)model_box.ty,-1.0,1.0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
		// draw the drag selection
		
	if (state.drag_sel_on) {
		glColor4f(0.8f,0.8f,0.8f,0.4f);
		
		glBegin(GL_QUADS);
		glVertex2i((state.drag_sel_box.lx+model_box.lx),(state.drag_sel_box.ty+model_box.ty));
		glVertex2i((state.drag_sel_box.rx+model_box.lx),(state.drag_sel_box.ty+model_box.ty));
		glVertex2i((state.drag_sel_box.rx+model_box.lx),(state.drag_sel_box.by+model_box.ty));
		glVertex2i((state.drag_sel_box.lx+model_box.lx),(state.drag_sel_box.by+model_box.ty));

		glEnd();
	}
	
		// border
		
	glColor4f(0.0f,0.0f,0.0f,1.0f);

	glBegin(GL_LINES);
	glVertex2i(model_box.rx,model_box.ty);
	glVertex2i(model_box.rx,model_box.by);
	glEnd();
}

void draw_model_setup_pose(int pose_idx)
{
	model_draw_setup_clear(&model,&draw_setup);

    draw_setup.poses[0].idx_1=pose_idx;
	draw_setup.poses[0].idx_2=-1;
    draw_setup.poses[0].factor=0;
	draw_setup.poses[0].acceleration=0;

	draw_setup.ang.x=0;
	draw_setup.ang.y=0;
	draw_setup.ang.z=0;
	
    draw_setup.sway.x=draw_setup.sway.z=draw_setup.sway.y=0;
    draw_setup.move.x=draw_setup.move.z=draw_setup.move.y=0;
}

void draw_model_wind_pose(int mesh_idx,int pose_idx)
{
	draw_model_setup_pose(pose_idx);
	draw_model_wind(mesh_idx);
}
