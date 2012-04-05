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

extern int					app_state;

extern map_type				map;
extern view_type			view;
extern server_type			server;
extern iface_type			iface;
extern setup_type			setup;
extern render_info_type		render_info;
extern camera_type			camera;

#ifdef D3_SDL_1_3
	extern SDL_Window		*sdl_wind;
#endif

GLint						vport[4];
float						mod_matrix[16],proj_matrix[16];

/* =======================================================

      3D Projections
      
======================================================= */

void gl_3D_view(void)
{
	float			ratio;

		// setup projections

	glMatrixMode(GL_PROJECTION);
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

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
#ifdef D3_ROTATE_VIEW
	glTranslatef((float)view.screen.y_sz,0.0f,0.0f);
	glRotatef(-90.0f,0.0f,0.0f,1.0f);
#endif
	
	glu_patch_gluLookAt((float)view.render->camera.pnt.x,(float)view.render->camera.pnt.y,(float)(view.render->camera.pnt.z+map.camera.plane.near_z),(float)view.render->camera.pnt.x,(float)view.render->camera.pnt.y,(float)view.render->camera.pnt.z,0.0f,1.0f,0.0f);
}

void gl_3D_rotate(d3pnt *pnt,d3ang *ang)
{
	float		fx,fz,fy,ang_x;
	matrix_type	mat;

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
#ifdef D3_ROTATE_VIEW
	glTranslatef((float)view.screen.y_sz,0.0f,0.0f);
	glRotatef(-90.0f,0.0f,0.0f,1.0f);
#endif
	
		// need to cap look up/down at 90
		
	ang_x=ang->x;
	if (ang_x>=90.0f) ang_x=89.9f;
	if (ang_x<=-90.0f) ang_x=-89.9f;

		// create the look at vector
		
	matrix_rotate_zyx(&mat,ang_x,ang->y,ang->z);
	fx=fy=0.0f;
	fz=-400.0f;			// the default near z
	matrix_vertex_multiply(&mat,&fx,&fy,&fz);

	if (pnt==NULL) {
		glu_patch_gluLookAt(fx,fy,fz,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f);
	}
	else {
		glu_patch_gluLookAt((((float)pnt->x)+fx),(((float)pnt->y)+fy),(((float)pnt->z)+fz),(float)pnt->x,(float)pnt->y,(float)pnt->z,0.0f,1.0f,0.0f);
	}
}

/* =======================================================

      2D Projections
      
======================================================= */

void gl_2D_view_screen(void)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
#ifndef D3_ROTATE_VIEW
	glOrtho(0.0f,(GLfloat)view.screen.x_sz,(GLfloat)view.screen.y_sz,0.0f,-1.0f,1.0f);
#else
	glOrtho(0.0f,(GLfloat)view.screen.y_sz,(GLfloat)view.screen.x_sz,0.0f,-1.0f,1.0f);
#endif

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
#ifdef D3_ROTATE_VIEW
	glTranslatef((float)view.screen.y_sz,0.0f,0.0f);
	glRotatef(90.0f,0.0f,0.0f,1.0f);
#endif
}

void gl_2D_view_interface(void)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
#ifndef D3_ROTATE_VIEW
	glOrtho(0.0f,(GLfloat)iface.scale_x,(GLfloat)iface.scale_y,0.0f,-1.0f,1.0f);
#else
	glOrtho(0.0f,(GLfloat)iface.scale_y,(GLfloat)iface.scale_x,0.0f,-1.0f,1.0f);
#endif

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

#ifdef D3_ROTATE_VIEW
	glTranslatef((float)iface.scale_y,0.0f,0.0f);
	glRotatef(90.0f,0.0f,0.0f,1.0f);
#endif
}

/* =======================================================

      Pseudo 3D Model Projections (for models in interface)
      
======================================================= */

void gl_3D_view_interface_model()
{
	float				x,y;

	glMatrixMode(GL_PROJECTION);
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

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
#ifdef D3_ROTATE_VIEW
	glRotatef(90.0f,0.0f,0.0f,1.0f);
#endif
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
		// is this app deactivated?

	if (app_state==as_inactive) {
		gl_2D_view_screen();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

		glDisable(GL_ALPHA_TEST);
		glDisable(GL_DEPTH_TEST);

		glColor4f(0.0f,0.0f,0.0f,0.5f);
		view_primitive_2D_tint_screen();
	}

		// swap buffer

#ifdef D3_SDL_1_3
	SDL_GL_SwapWindow(sdl_wind);
#else
	SDL_GL_SwapBuffers();
#endif
}

/* =======================================================

      Interface to Screen Coordinates
      
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
	glGetFloatv(GL_MODELVIEW_MATRIX,mod_matrix);
	glGetFloatv(GL_PROJECTION_MATRIX,proj_matrix);
	glGetIntegerv(GL_VIEWPORT,vport);
}

bool gl_project_in_view_z(int x,int y,int z)
{
	return(((((float)x)*mod_matrix[2])+(((float)y)*mod_matrix[6])+(((float)z)*mod_matrix[10])+mod_matrix[14])>0.0);
}

void gl_project_point(int *x,int *y,int *z)
{
	float		dx,dy,dz;

#ifndef D3_ROTATE_VIEW
	glu_patch_gluProject((float)*x,(float)*y,(float)*z,mod_matrix,proj_matrix,vport,&dx,&dy,&dz);
	
	*x=(int)dx;
	*y=(int)dy;
	*z=(int)dz;
#else
	glu_patch_gluProject((float)*x,(float)*y,(float)*z,mod_matrix,proj_matrix,vport,&dy,&dx,&dz);
	
	*x=(int)(vport[3]-dx);
	*y=(int)dy;
	*z=(int)dz;
#endif
}

float gl_project_get_depth(int x,int y,int z)
{
	float		dx,dy,dz;

	glu_patch_gluProject((float)x,(float)y,(float)z,mod_matrix,proj_matrix,vport,&dx,&dy,&dz);	
	return((float)dz);
}

void gl_project_to_eye_coordinates(float *x,float *y,float *z)
{
	float		fx,fy,fz;
	
	fx=*x;
	fy=*y;
	fz=*z;
	
	*x=(float)((fx*mod_matrix[0])+(fy*mod_matrix[4])+(fz*mod_matrix[8])+mod_matrix[12]);
	*y=(float)((fx*mod_matrix[1])+(fy*mod_matrix[5])+(fz*mod_matrix[9])+mod_matrix[13]);
	*z=(float)((fx*mod_matrix[2])+(fy*mod_matrix[6])+(fz*mod_matrix[10])+mod_matrix[14]);
}
