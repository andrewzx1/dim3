/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Model Window Drawing

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

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#include "model.h"

d3rect						model_box,drag_sel_box;

extern int					cur_mesh,cur_bone,shift_x,shift_y,magnify_z,
							tool_pixel_sz,txt_palette_pixel_sz;
extern float				ang_y,ang_x;
extern bool					fileopen,model_bone_drag_on,drag_sel_on;
extern AGLContext			ctx;
extern WindowRef			wind;

extern model_draw_setup		draw_setup;
extern animator_state_type	state;

/* =======================================================

      Model Window Setup
      
======================================================= */

void model_wind_setup(void)
{
	int				x_sz;
	d3rect			wbox;
	
	os_get_window_box(&wbox);
	
	x_sz=(wbox.rx-wbox.lx)-total_list_width;
	if (x_sz<model_view_min_size) x_sz=model_view_min_size;
	
	model_box.lx=0;
	model_box.rx=model_box.lx+x_sz;
	model_box.ty=tool_pixel_sz;
	model_box.by=(wbox.by-wbox.ty)-txt_palette_pixel_sz;
}

/* =======================================================

      Setup GL Draw
      
======================================================= */

void draw_model_gl_setup(model_type *model,int z_offset)
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

	yoff=model->view_box.size.y/2;

	sz=500+((4000-magnify_z)*5);
	
	glTranslatef(-((GLfloat)shift_x),-((GLfloat)(shift_y-yoff)),(GLfloat)sz);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glRotatef(-ang_x,1.0f,0.0f,0.0f);
	glRotatef(angle_add(ang_y,180.0f),0.0f,1.0f,0.0f);

		// drawing setup
		
	glColor4f(1.0f,1.0f,1.0f,0.0f);
	
	glDisable(GL_SMOOTH);
	glDisable(GL_DITHER);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_ALPHA_TEST);
}

/* =======================================================

      Draw Model Bone and Vertex Setup
      
======================================================= */

void draw_model_setup_bones_vertexes(model_type *model,int mesh_idx,model_draw_setup *draw_setup)
{
		// create the drawing bones
	
	model_create_draw_bones(model,draw_setup);
	
		// calculate vertexes for first mesh if "show first mesh" is on
		
	if ((state.first_mesh) && (mesh_idx!=0)) {
		model_create_draw_vertexes(model,0,draw_setup);
		model_create_draw_normals(model,0,draw_setup);
	}

		// calculate vertexes for drawing mesh
		
	model_create_draw_vertexes(model,mesh_idx,draw_setup);
	model_create_draw_normals(model,mesh_idx,draw_setup);
}

/* =======================================================

      Draw Model Window
      
======================================================= */

void draw_model_wind(model_type *model,int mesh_idx,model_draw_setup *draw_setup)
{
	if (!fileopen) return;

		// setup transformation to fit model in middle of screen
		
	draw_model_gl_setup(model,0);
	
		// draw memory
		
	model_draw_setup_initialize(model,draw_setup,TRUE);
	
		// draw the center
		
	draw_model_axis(model);

		// create the drawing bones, vertex arrays and normal arrays
		
	draw_model_setup_bones_vertexes(model,mesh_idx,draw_setup);
	
		// draw the mesh(es) in the current view
	
	if (state.texture) {
		if ((state.first_mesh) && (mesh_idx!=0)) draw_model(model,0,draw_setup);
		draw_model(model,mesh_idx,draw_setup);
		draw_model_selected_vertexes(model,mesh_idx,draw_setup);
	}
	
	if (state.mesh) {
		draw_model_gl_setup(model,10);
		if ((state.first_mesh) && (mesh_idx!=0)) draw_model_mesh(model,0,draw_setup);
		draw_model_mesh(model,mesh_idx,draw_setup);
		draw_model_gl_setup(model,0);
	}
	
	if (state.bone) draw_model_bones(model,draw_setup,cur_bone);
	
	if ((state.texture) || (state.mesh)) {
		draw_model_selected_vertexes(model,mesh_idx,draw_setup);
	}
	
		// boxes and normals
		
	if (state.hit_box) draw_model_box_hit_boxes(model,draw_setup);
	if (state.normal) draw_model_normals(model,mesh_idx,draw_setup);
	if (state.view_box) draw_model_box_view(model,draw_setup);
	
		// free memory
		
	model_draw_setup_shutdown(model,draw_setup);
	
		// 2D drawing
		
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho((GLdouble)model_box.lx,(GLdouble)model_box.rx,(GLdouble)model_box.by,(GLdouble)model_box.ty,-1.0,1.0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
		// draw the drag selection
		
	if (drag_sel_on) {
		glColor4f(0.8,0.8,0.8,0.4);
		
		glBegin(GL_QUADS);
		glVertex2i((drag_sel_box.lx+model_box.lx),(drag_sel_box.ty+model_box.ty));
		glVertex2i((drag_sel_box.rx+model_box.lx),(drag_sel_box.ty+model_box.ty));
		glVertex2i((drag_sel_box.rx+model_box.lx),(drag_sel_box.by+model_box.ty));
		glVertex2i((drag_sel_box.lx+model_box.lx),(drag_sel_box.by+model_box.ty));

		glEnd();
	}
	
		// border
		
	glColor4f(0.0f,0.0f,0.0f,1.0f);

	glBegin(GL_LINES);
	glVertex2i(model_box.rx,model_box.ty);
	glVertex2i(model_box.rx,model_box.by);
	glEnd();
}

void draw_model_setup_pose(model_type *model,model_draw_setup *draw_setup,int wpose)
{
	model_draw_setup_clear(model,draw_setup);

    draw_setup->poses[0].idx_1=wpose;
	draw_setup->poses[0].idx_2=-1;
    draw_setup->poses[0].factor=0;
	draw_setup->poses[0].acceleration=0;

	draw_setup->ang.x=0;
	draw_setup->ang.y=0;
	draw_setup->ang.z=0;
	
    draw_setup->sway.x=draw_setup->sway.z=draw_setup->sway.y=0;
    draw_setup->move.x=draw_setup->move.z=draw_setup->move.y=0;
}

void draw_model_wind_pose(model_type *model,int wmesh,int wpose)
{
	draw_model_setup_pose(model,&draw_setup,wpose);
	draw_model_wind(model,wmesh,&draw_setup);
}
