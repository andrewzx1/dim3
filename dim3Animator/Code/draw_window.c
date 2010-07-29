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

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#include "model.h"

extern int					cur_mesh,cur_bone,shift_x,shift_y,magnify_z,
							gl_view_x_sz,gl_view_y_sz,gl_view_texture_palette_size;
extern float				ang_y,ang_x;
extern bool					fileopen,play_animate,model_bone_drag_on,drag_sel_on;
extern Rect					drag_sel_box;
extern AGLContext			ctx;
extern WindowRef			model_wind;

extern display_type			display;

extern model_draw_setup		draw_setup;

/* =======================================================

      Setup GL Draw
      
======================================================= */

void draw_model_gl_setup(model_type *model,int z_offset)
{
	int				yoff,sz;
	float			ratio;
	Rect			wbox;
	GLint			rect[4];
	
		// model viewport
		
	aglSetCurrentContext(ctx);
		
	GetWindowPortBounds(model_wind,&wbox);
	
	rect[0]=0;
	rect[1]=info_palette_height+gl_view_texture_palette_size;
	rect[2]=gl_view_x_sz;
	rect[3]=gl_view_y_sz;

	aglSetInteger(ctx,AGL_BUFFER_RECT,rect);
	aglEnable(ctx,AGL_BUFFER_RECT);
	
	glViewport(0,0,gl_view_x_sz,gl_view_y_sz);
	
	glScissor(0,0,gl_view_x_sz,gl_view_y_sz);
	glEnable(GL_SCISSOR_TEST);
	
		// projection
		
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	ratio=(float)gl_view_x_sz/(float)gl_view_y_sz;
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
		
	if ((display.first_mesh) && (mesh_idx!=0)) {
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
		// setup transformation to fit model in middle of screen
		
	draw_model_gl_setup(model,0);

		// clear the buffer
		
	glClearColor(0.9,0.9,0.9,0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	
	if (!fileopen) {
		aglSwapBuffers(ctx);
		return;
	}
	
		// draw memory
		
	model_draw_setup_initialize(model,draw_setup,TRUE);
	
		// draw the center
		
	draw_model_axis(model);

		// create the drawing bones, vertex arrays and normal arrays
		
	draw_model_setup_bones_vertexes(model,mesh_idx,draw_setup);
	
		// draw the mesh(es) in the current view
	
	if (display.texture) {
		if ((display.first_mesh) && (mesh_idx!=0)) draw_model(model,0,draw_setup);
		draw_model(model,mesh_idx,draw_setup);
		draw_model_selected_vertexes(model,mesh_idx,draw_setup);
	}
	
	if (display.mesh) {
		draw_model_gl_setup(model,10);
		if ((display.first_mesh) && (mesh_idx!=0)) draw_model_mesh(model,0,draw_setup);
		draw_model_mesh(model,mesh_idx,draw_setup);
		draw_model_gl_setup(model,0);
	}
	
	if (display.bone) draw_model_bones(model,draw_setup,cur_bone);
	
	if ((display.texture) || (display.mesh)) {
		draw_model_selected_vertexes(model,mesh_idx,draw_setup);
	}
	
		// boxes and normals
		
	if (display.hit_box) draw_model_box_hit_boxes(model,draw_setup);
	if (display.normal) draw_model_normals(model,mesh_idx,draw_setup);
	if (display.view_box) draw_model_box_view(model,draw_setup);
	
		// draw the drag selection
		
	if (drag_sel_on) {
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0,gl_view_x_sz,0,gl_view_y_sz,-1,1);
	
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	
		glColor4f(0.8,0.8,0.8,0.4);
		glBegin(GL_POLYGON);
		glVertex3i(drag_sel_box.left,(gl_view_y_sz-drag_sel_box.top),-0.1);
		glVertex3i(drag_sel_box.right,(gl_view_y_sz-drag_sel_box.top),-0.1);
		glVertex3i(drag_sel_box.right,(gl_view_y_sz-drag_sel_box.bottom),-0.1);
		glVertex3i(drag_sel_box.left,(gl_view_y_sz-drag_sel_box.bottom),-0.1);
		glEnd();
	}
   
	aglSwapBuffers(ctx);
	
		// free memory
		
	model_draw_setup_shutdown(model,draw_setup);
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
