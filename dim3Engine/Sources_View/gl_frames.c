/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: OpenGL Frame Routines

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
	#include "dim3engine.h"
#endif

#include "interface.h"

extern app_type				app;
extern map_type				map;
extern view_type			view;
extern server_type			server;
extern iface_type			iface;
extern setup_type			setup;
extern render_info_type		render_info;
extern camera_type			camera;

extern SDL_Window			*sdl_wind;

GLint						vport[4];
float						gl_proj_matrix[16],gl_model_view_matrix[16];

/* =======================================================

      View Port
      
======================================================= */

void gl_set_viewport(int x,int y,int wid,int high,bool force_shader_reset)
{
	glViewport(x,y,wid,high);

		// force shaders to reset matrixes
		
	if (force_shader_reset) gl_shader_force_matrix_resets();
}

/* =======================================================

      3D Projections
      
======================================================= */

void gl_3D_view(void)
{
	float			ratio;
	matrix_type		mat;
	
	
		// setup projections

	glMatrixMode(GL_PROJECTION);		// supergumba -- delete
	glLoadIdentity();

#ifndef D3_ROTATE_VIEW
	ratio=(((float)view.screen.x_sz)/((float)view.screen.y_sz))*map.camera.plane.aspect_ratio;
#else
	ratio=(((float)view.screen.y_sz)/((float)view.screen.x_sz))*map.camera.plane.aspect_ratio;
#endif

	glu_patch_gluPerspective(view.render->camera.fov,ratio,(float)map.camera.plane.near_z,(float)map.camera.plane.far_z);
	
		// projection flips
		
	if (view.render->camera.flip) {
		glScalef(1.0f,1.0f,-1.0f);
	}
	else {
		glScalef(-1.0f,-1.0f,-1.0f);
	}
	
	glTranslatef(0.0f,0.0f,(float)(map.camera.plane.near_z_offset+view.render->camera.z_adjust));

		// default rotations

	glMatrixMode(GL_MODELVIEW);		// supergumba -- delete
	glLoadIdentity();
	
#ifdef D3_ROTATE_VIEW
	glTranslatef((float)view.screen.y_sz,0.0f,0.0f);
	glRotatef(-90.0f,0.0f,0.0f,1.0f);
#endif
	
	glu_patch_gluLookAt((float)view.render->camera.pnt.x,(float)view.render->camera.pnt.y,(float)(view.render->camera.pnt.z+map.camera.plane.near_z),(float)view.render->camera.pnt.x,(float)view.render->camera.pnt.y,(float)view.render->camera.pnt.z,0.0f,1.0f,0.0f);











		// create the projection matrix
		// as a column ordered opengl matrix
		
#ifndef D3_ROTATE_VIEW
	ratio=(((float)view.screen.x_sz)/((float)view.screen.y_sz))*map.camera.plane.aspect_ratio;
#else
	ratio=(((float)view.screen.y_sz)/((float)view.screen.x_sz))*map.camera.plane.aspect_ratio;
#endif
	
	matrix_perspective(&mat,view.render->camera.fov,ratio,(float)map.camera.plane.near_z,(float)map.camera.plane.far_z);

	if (view.render->camera.flip) {
		matrix_scale(&mat,1.0f,1.0f,-1.0f);
	}
	else {
		matrix_scale(&mat,-1.0f,-1.0f,-1.0f);
	}
	
	matrix_translate(&mat,0.0f,0.0f,(float)(map.camera.plane.near_z_offset+view.render->camera.z_adjust));
	
	matrix_to_opengl_uniform_4x4(&mat,gl_proj_matrix);

		// create the model view matrix
		// as a column ordered opengl matrix

	matrix_identity(&mat);

#ifdef D3_ROTATE_VIEW
	matrix_translate(&mat,(float)view.screen.y_sz,0.0f,0.0f);
	matrix_rotate(&mat,-90.0f,0.0f,0.0f,1.0f);
#endif
	
	matrix_lookat(&mat,(float)view.render->camera.pnt.x,(float)view.render->camera.pnt.y,(float)(view.render->camera.pnt.z+map.camera.plane.near_z),(float)view.render->camera.pnt.x,(float)view.render->camera.pnt.y,(float)view.render->camera.pnt.z,0.0f,1.0f,0.0f);

	matrix_to_opengl_uniform_4x4(&mat,gl_model_view_matrix);
	
		// force shaders to reset matrixes
		
	gl_shader_force_matrix_resets();
}

void gl_3D_rotate(d3pnt *pnt,d3ang *ang)
{
	float		fx,fz,fy,ang_x;
	matrix_type	mat,look_mat;

	glMatrixMode(GL_MODELVIEW);		// supergumba -- delete
	glLoadIdentity();
	
#ifdef D3_ROTATE_VIEW
	glTranslatef((float)view.screen.y_sz,0.0f,0.0f);
	glRotatef(-90.0f,0.0f,0.0f,1.0f);
#endif

		// create the model view matrix
		// as a column ordered opengl matrix

	matrix_identity(&mat);

#ifdef D3_ROTATE_VIEW
	matrix_translate(&mat,(float)view.screen.y_sz,0.0f,0.0f);
	matrix_rotate(&mat,-90.0f,0.0f,0.0f,1.0f);
#endif

	
		// need to cap look up/down at 90
		
	ang_x=ang->x;
	if (ang_x>=90.0f) ang_x=89.9f;
	if (ang_x<=-90.0f) ang_x=-89.9f;

		// create the look at vector
		
	matrix_rotate_zyx(&look_mat,ang_x,ang->y,ang->z);
	fx=fy=0.0f;
	fz=-400.0f;			// the default near z
	matrix_vertex_multiply(&look_mat,&fx,&fy,&fz);

		// create

	if (pnt==NULL) {		// supergumba -- delete
		glu_patch_gluLookAt(fx,fy,fz,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f);
	}
	else {
		glu_patch_gluLookAt((((float)pnt->x)+fx),(((float)pnt->y)+fy),(((float)pnt->z)+fz),(float)pnt->x,(float)pnt->y,(float)pnt->z,0.0f,1.0f,0.0f);
	}




	if (pnt==NULL) {
		matrix_lookat(&mat,fx,fy,fz,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f);
	}
	else {
		matrix_lookat(&mat,(((float)pnt->x)+fx),(((float)pnt->y)+fy),(((float)pnt->z)+fz),(float)pnt->x,(float)pnt->y,(float)pnt->z,0.0f,1.0f,0.0f);
	}

	matrix_to_opengl_uniform_4x4(&mat,gl_model_view_matrix);
	
		// force shaders to reset matrixes
		
	gl_shader_force_matrix_resets();
}

/* =======================================================

      2D Projections
      
======================================================= */

void gl_2D_view_screen(void)
{
	matrix_type			mat;
	
	glMatrixMode(GL_PROJECTION);		// supergumba -- delete
	glLoadIdentity();
	
#ifndef D3_ROTATE_VIEW
	glOrtho(0.0f,(GLfloat)view.screen.x_sz,(GLfloat)view.screen.y_sz,0.0f,-1.0f,1.0f);
#else
	glOrtho(0.0f,(GLfloat)view.screen.y_sz,(GLfloat)view.screen.x_sz,0.0f,-1.0f,1.0f);
#endif


		// create the projection matrix
		// as a column ordered opengl matrix
				
#ifndef D3_ROTATE_VIEW
	matrix_ortho(&mat,0.0f,(float)view.screen.x_sz,(float)view.screen.y_sz,0.0f,-1.0f,1.0f);
#else
	matrix_ortho(&mat,0.0f,(float)view.screen.y_sz,(float)view.screen.x_sz,0.0f,-1.0f,1.0f);
#endif

	matrix_to_opengl_uniform_4x4(&mat,gl_proj_matrix);




	glMatrixMode(GL_MODELVIEW);		// supergumba -- delete
	glLoadIdentity();
	
#ifdef D3_ROTATE_VIEW
	glTranslatef((float)view.screen.y_sz,0.0f,0.0f);
	glRotatef(90.0f,0.0f,0.0f,1.0f);
#endif



		// create the model view matrix
		// as a column ordered opengl matrix

	matrix_identity(&mat);

#ifdef D3_ROTATE_VIEW
	matrix_translate(&mat,(float)view.screen.y_sz,0.0f,0.0f);
	matrix_rotate(&mat,90.0f,0.0f,0.0f,1.0f);
#endif

	matrix_to_opengl_uniform_4x4(&mat,gl_model_view_matrix);

		// force shaders to reset matrixes
		
	gl_shader_force_matrix_resets();
}

void gl_2D_view_interface(void)
{
	matrix_type		mat;
	
	glMatrixMode(GL_PROJECTION);			// supergumba -- delete
	glLoadIdentity();
	
#ifndef D3_ROTATE_VIEW
	glOrtho(0.0f,(GLfloat)iface.scale_x,(GLfloat)iface.scale_y,0.0f,-1.0f,1.0f);
#else
	glOrtho(0.0f,(GLfloat)iface.scale_y,(GLfloat)iface.scale_x,0.0f,-1.0f,1.0f);
#endif


		// create the projection matrix
		// as a column ordered opengl matrix
				
#ifndef D3_ROTATE_VIEW
	matrix_ortho(&mat,0.0f,(float)iface.scale_x,(float)iface.scale_y,0.0f,-1.0f,1.0f);
#else
	matrix_ortho(&mat,0.0f,(float)iface.scale_y,(float)iface.scale_x,0.0f,-1.0f,1.0f);
#endif

	matrix_to_opengl_uniform_4x4(&mat,gl_proj_matrix);





	glMatrixMode(GL_MODELVIEW);		// supergumba -- delete
	glLoadIdentity();

#ifdef D3_ROTATE_VIEW
	glTranslatef((float)iface.scale_y,0.0f,0.0f);
	glRotatef(90.0f,0.0f,0.0f,1.0f);
#endif


		// create the model view matrix
		// as a column ordered opengl matrix

	matrix_identity(&mat);

#ifdef D3_ROTATE_VIEW
	matrix_translate(&mat,(float)view.screen.y_sz,0.0f,0.0f);
	matrix_rotate(&mat,90.0f,0.0f,0.0f,1.0f);
#endif

	matrix_to_opengl_uniform_4x4(&mat,gl_model_view_matrix);

		// force shaders to reset matrixes
		
	gl_shader_force_matrix_resets();
}

/* =======================================================

      Pseudo 3D Model Projections (for models in interface)
      
======================================================= */

void gl_3D_view_interface_model()
{
	float				x,y;
	matrix_type			mat;

	glMatrixMode(GL_PROJECTION);		// supergumba -- delete
	glLoadIdentity();

	x=(float)(iface.scale_x>>1);
	y=(float)(iface.scale_y>>1);

#ifndef D3_ROTATE_VIEW
	glFrustum(-x,x,-y,y,1000.0f,21000.0f);
#else
	glFrustum(-y,y,-x,x,1000.0f,21000.0f);
#endif

	glScalef(1.0f,-1.0f,-1.0f);
	glTranslatef(0.0f,0.0f,5000.0f);
	
	
		// create the projection matrix
		// as a column ordered opengl matrix
		
	x=(float)(iface.scale_x>>1);
	y=(float)(iface.scale_y>>1);

#ifndef D3_ROTATE_VIEW
	matrix_frustum(&mat,-x,x,-y,y,1000.0f,21000.0f);
#else
	matrix_frustum(&mat,-y,y,-x,x,1000.0f,21000.0f);
#endif

	matrix_scale(&mat,1.0f,-1.0f,-1.0f);
	matrix_translate(&mat,0.0f,0.0f,5000.0f);
	
	matrix_to_opengl_uniform_4x4(&mat,gl_proj_matrix);





	glMatrixMode(GL_MODELVIEW);		// supergumba -- delete
	glLoadIdentity();
	
#ifdef D3_ROTATE_VIEW
	glRotatef(90.0f,0.0f,0.0f,1.0f);
#endif



		// create the model view matrix
		// as a column ordered opengl matrix

	matrix_identity(&mat);

#ifdef D3_ROTATE_VIEW
	matrix_rotate(&mat,90.0f,0.0f,0.0f,1.0f);
#endif

	matrix_to_opengl_uniform_4x4(&mat,gl_model_view_matrix);

		// force shaders to reset matrixes
		
	gl_shader_force_matrix_resets();
}

/* =======================================================

      Start and End a Drawing Session
      
======================================================= */

void gl_frame_clear(bool in_view)
{
		// if obscuring fog on, then background = fog color

	if ((!fog_solid_on()) || (!in_view)) {
		glClearColor(0.0f,0.0f,0.0f,0.0f);
	}
	else {
		glClearColor(map.fog.col.r,map.fog.col.g,map.fog.col.b,0.0f);
	}

		// clear the frame

	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
}

void gl_frame_swap(void)
{
#ifdef D3_OS_IPHONE
	const GLenum		discards[]={GL_DEPTH_ATTACHMENT,GL_STENCIL_ATTACHMENT};
#endif
	d3col				col;
	
		// is this app deactivated?

	if (app.state==as_inactive) {
		gl_2D_view_screen();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

		glDisable(GL_DEPTH_TEST);
		
		col.r=col.g=col.b=0.0f;
		view_primitive_2D_tint_screen(&col,0.5f);
	}
	
		// discard the depth and stencil
	
#ifdef D3_OS_IPHONE
	glDiscardFramebufferEXT(GL_FRAMEBUFFER_EXT,2,discards);
#endif

		// swap buffer

	SDL_GL_SwapWindow(sdl_wind);
}

/* =======================================================

      Scissoring
      
======================================================= */

void gl_2D_scissor_start(int lx,int rx,int ty,int by)
{
	lx=(view.screen.x_sz*(lx))/iface.scale_x;
	rx=(view.screen.x_sz*(rx))/iface.scale_x;
	ty=(view.screen.y_sz*(ty))/iface.scale_y;
	by=(view.screen.y_sz*(by))/iface.scale_y;

	glEnable(GL_SCISSOR_TEST);
	
#ifndef D3_ROTATE_VIEW
	glScissor(lx,(view.screen.y_sz-by),(rx-lx),(by-ty));
#else
	glScissor((view.screen.y_sz-by),lx,(by-ty),(rx-lx));
#endif
}

void gl_2D_scissor_end(void)
{
	glDisable(GL_SCISSOR_TEST);
}

/* =======================================================

      Projection Points
      
======================================================= */

void gl_setup_project(void)
{
	// supergumba -- delete all this
	// we will need gs_set_viewport that records all the viewport changes,
	// and one to save/restore it (for back rendering)
	glGetIntegerv(GL_VIEWPORT,vport);
}

bool gl_project_in_view_z(int x,int y,int z)
{
	return(((((float)x)*gl_model_view_matrix[2])+(((float)y)*gl_model_view_matrix[6])+(((float)z)*gl_model_view_matrix[10])+gl_model_view_matrix[14])>0.0);
}

void gl_project_point(int *x,int *y,int *z)
{
	float		dx,dy,dz;

#ifndef D3_ROTATE_VIEW
	glu_patch_gluProject((float)*x,(float)*y,(float)*z,gl_model_view_matrix,gl_proj_matrix,vport,&dx,&dy,&dz);
	
	*x=(int)dx;
	*y=(int)dy;
	*z=(int)dz;
#else
	glu_patch_gluProject((float)*x,(float)*y,(float)*z,gl_model_view_matrix,gl_proj_matrix,vport,&dy,&dx,&dz);
	
	*x=(int)(vport[3]-dx);
	*y=(int)dy;
	*z=(int)dz;
#endif
}

float gl_project_get_depth(int x,int y,int z)
{
	float		dx,dy,dz;

	glu_patch_gluProject((float)x,(float)y,(float)z,gl_model_view_matrix,gl_proj_matrix,vport,&dx,&dy,&dz);	
	return((float)dz);
}

void gl_project_to_eye_coordinates(float *x,float *y,float *z)
{
	float		fx,fy,fz;
	
	fx=*x;
	fy=*y;
	fz=*z;
	
	*x=(float)((fx*gl_model_view_matrix[0])+(fy*gl_model_view_matrix[4])+(fz*gl_model_view_matrix[8])+gl_model_view_matrix[12]);
	*y=(float)((fx*gl_model_view_matrix[1])+(fy*gl_model_view_matrix[5])+(fz*gl_model_view_matrix[9])+gl_model_view_matrix[13]);
	*z=(float)((fx*gl_model_view_matrix[2])+(fy*gl_model_view_matrix[6])+(fz*gl_model_view_matrix[10])+gl_model_view_matrix[14]);
}
