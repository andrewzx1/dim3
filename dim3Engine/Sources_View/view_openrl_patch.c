/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View OpenRL Patches

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

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "interface.h"

#ifdef D3_OPENRL
	#include "ray_interface.h"
#endif

extern map_type				map;
extern server_type			server;
extern camera_type			camera;
extern view_type			view;
extern iface_type			iface;
extern setup_type			setup;
extern network_setup_type	net_setup;
extern file_path_setup_type	file_path_setup;

#ifdef D3_OPENRL

	#define view_rl_buffer_wid		320
	#define view_rl_buffer_high		200

	int								view_rl_scene_id,
									view_rl_purple_material_id,
									view_rl_font_material_id,
									view_rl_lx,view_rl_rx,
									view_rl_ty,view_rl_by,
									view_rl_last_msec,view_rl_msec_display,
									view_rl_msec,view_rl_msec_count,
									view_rl_overlay[4];
	GLuint							view_rl_gl_id;

	texture_font_size_type			view_rl_font;

	extern void view_openrl_model_update(void);

#endif

/* =======================================================

      Blank Patches
      
======================================================= */

#ifndef D3_OPENRL

bool view_openrl_initialize(char *err_str) { return(TRUE); }
void view_openrl_shutdown(void) {}
void view_openrl_map_setup(void) {}
void view_openrl_render(void) {}
void view_openrl_model_setup(void) {}
void view_openrl_map_close_cleanup(void) {}

#else

/* =======================================================

      View OpenRL Init/Shutdown
      
======================================================= */

bool view_openrl_initialize(char *err_str)
{
	int					n,sz,wid,high;
	float				f;
	unsigned char		*data,*dptr;
	rlColor				col;
	rl2DPoint			p_pnt,s_pnt;

		// initialize OpenRL

	if (rlInitialize(0)!=RL_ERROR_OK) {
		strcpy(err_str,"Unable to initialize OpenRL");
		return(FALSE);
	}

		// make the scene

	s_pnt.x=view_rl_buffer_wid;
	s_pnt.y=view_rl_buffer_high;

	view_rl_scene_id=rlSceneAdd(&s_pnt,RL_SCENE_TARGET_MEMORY,RL_SCENE_FORMAT_32_RGBA,NULL,0);
	if (view_rl_scene_id<0) {
		strcpy(err_str,"Unable to create OpenRL scene");
		rlShutdown();
		return(FALSE);
	}

		// test material

	col.r=1.0f;
	col.g=0.0f;
	col.b=1.0f;
	col.a=1.0f;
	view_rl_purple_material_id=rlMaterialAdd(1,1,0);
	rlMaterialAttachBufferColor(view_rl_purple_material_id,RL_MATERIAL_TARGET_COLOR,&col);

		// text material

	data=bitmap_text_size_data(&view_rl_font,"Arial",48,1024,512);
	view_rl_font_material_id=rlMaterialAdd(1024,512,0);
	rlMaterialAttachBufferData(view_rl_font_material_id,RL_MATERIAL_TARGET_COLOR,RL_MATERIAL_FORMAT_32_RGBA,data);
	free(data);

	rlMaterialBuildMipMaps(view_rl_font_material_id);

		// overlays for fps

	p_pnt.x=5;
	p_pnt.y=view_rl_buffer_high-25;
	s_pnt.x=15;
	s_pnt.y=20;

	col.r=1.0f;
	col.g=1.0f;
	col.b=0.0f;
	col.a=1.0f;

	for (n=0;n!=4;n++) {
		view_rl_overlay[n]=rlSceneOverlayAdd(view_rl_scene_id,view_rl_font_material_id,0);
		rlSceneOverlaySetPosition(view_rl_scene_id,view_rl_overlay[n],&p_pnt);
		rlSceneOverlaySetSize(view_rl_scene_id,view_rl_overlay[n],&s_pnt);
		rlSceneOverlayColor(view_rl_scene_id,view_rl_overlay[n],&col);
		p_pnt.x+=s_pnt.x;
	}

		// we need a texture to transfer
		// the scene to opengl raster

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1,&view_rl_gl_id);
	glBindTexture(GL_TEXTURE_2D,view_rl_gl_id);

	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	sz=(view_rl_buffer_wid*4)*view_rl_buffer_high;
	data=malloc(sz);
	if (data==NULL) {
		strcpy(err_str,"Out of memory");
		rlShutdown();
		return(FALSE);
	}
	bzero(data,sz);

	dptr=data;
	for (n=0;n!=(sz/4);n++) {
		*dptr++=0xFF;
		*dptr++=0x0;
		*dptr++=0x0;
		*dptr++=0xFF;
	}

	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,view_rl_buffer_wid,view_rl_buffer_high,0,GL_RGBA,GL_UNSIGNED_BYTE,data);

	free(data);

	glBindTexture(GL_TEXTURE_2D,0);

		// get drawing size

//	f=((float)view.screen.x_sz)/((float)view_rl_buffer_wid);
	f=1.0f;

	wid=(int)(((float)view_rl_buffer_wid)*f);
	view_rl_lx=(view.screen.x_sz-wid)>>1;
	view_rl_rx=view_rl_lx+wid;

	high=(int)(((float)view_rl_buffer_high)*f);
	view_rl_ty=(view.screen.y_sz-high)>>1;
	view_rl_by=view_rl_ty+high;

	view_rl_msec_count=0;
	view_rl_msec=0;
	view_rl_msec_display=0;
	view_rl_last_msec=game_time_get_raw();

	return(TRUE);
}

void view_openrl_shutdown(void)
{
	glDeleteTextures(1,&view_rl_gl_id);

	rlSceneDelete(view_rl_scene_id);
	rlShutdown();
}

/* =======================================================

      OpenRL Clean Up
      
======================================================= */

void view_openrl_map_close_cleanup(void)
{
	rlSceneMeshDeleteAll(view_rl_scene_id);
	rlMaterialDeleteAll();
}

/* =======================================================

      OpenRL Overlay Setup
      
======================================================= */

void view_openrl_update_overlays(void)
{
	int			n,xoff,yoff;
	rlUV		uv,uv_size;
	char		ch,str[32];

		// update timing

	view_rl_msec_count++;
	view_rl_msec+=(game_time_get_raw()-view_rl_last_msec);

	if (view_rl_msec_count==10) {
		view_rl_msec_display=view_rl_msec/view_rl_msec_count;
		view_rl_msec=0;
		view_rl_msec_count=0;
	}

		// update overlay

	sprintf(str,"%4d",view_rl_msec_display);

	for (n=0;n!=4;n++) {
		ch=str[n]-'!';

		yoff=ch/view_rl_font.char_per_line;
		xoff=ch-(yoff*view_rl_font.char_per_line);

		uv.x=((float)xoff)*view_rl_font.gl_xoff;
		uv.y=((float)yoff)*view_rl_font.gl_yoff;
		rlSceneOverlaySetUV(view_rl_scene_id,view_rl_overlay[n],&uv);

		uv_size.x=view_rl_font.gl_xadd;
		uv_size.y=view_rl_font.gl_yadd;
		rlSceneOverlaySetUVStamp(view_rl_scene_id,view_rl_overlay[n],&uv_size);
	}
}

/* =======================================================

      OpenRL Rendering
      
======================================================= */

void view_openrl_transfer_to_opengl(void)
{
	int				err;
	unsigned char	*data;

		// draws on 2D screen

	gl_2D_view_screen();

		// get the scene buffer
		// and push it to a texture
		// scene memory buffers was set
		// to RL_SCENE_FORMAT_32_RGBA

	err=rlSceneGetBuffer(view_rl_scene_id,(void**)&data);
	if (err!=RL_ERROR_OK) return;

	gl_texture_bind(0,view_rl_gl_id);
	glTexSubImage2D(GL_TEXTURE_2D,0,0,0,view_rl_buffer_wid,view_rl_buffer_high,GL_RGBA,GL_UNSIGNED_BYTE,data);

		// build the vertex and uv list

	view_primitive_2D_texture_quad(view_rl_gl_id,NULL,1.0f,view_rl_lx,view_rl_rx,view_rl_ty,view_rl_by,0.0f,1.0f,0.0f,1.0f,TRUE);
}

void view_openrl_render(void)
{
	float			ang_y;
	rlPoint			pnt;
	rlVector		scale;
	rlMatrix		mat,x_mat,scale_mat;
	rlColor			col;

		// build the eye point

	pnt.x=(float)view.render->camera.pnt.x;
	pnt.y=(float)view.render->camera.pnt.y;
	pnt.z=(float)view.render->camera.pnt.z;

		// build the rotation matrix
		// dim3 always had a backwards look, so
		// we need to fix that with the matrix
		// normally it wouldn't be this complex

	ang_y=angle_add(view.render->camera.ang.y,180.0f);
	rlMatrixRotateY(&mat,ang_y);

	rlMatrixRotateX(&x_mat,-view.render->camera.ang.x);
	rlMatrixMultiply(&mat,&x_mat);

	scale.x=-1.0f;
	scale.y=1.0f;
	scale.z=1.0f;
	rlMatrixScale(&scale_mat,&scale);
	rlMatrixMultiply(&mat,&scale_mat);

		// set the eye position

	rlSceneEyePositionSet(view_rl_scene_id,&pnt,&mat,200.0f,300000.0f);

		// update the scene
		
	view_openrl_model_update();
	view_openrl_update_overlays();

		// render

	col.r=0.0f;
	col.g=0.0f;
	col.b=0.0f;
	col.a=1.0f;
	rlSceneClearBuffer(view_rl_scene_id,&col);

	if (rlSceneRender(view_rl_scene_id)!=RL_ERROR_OK) return;

		// wait for render to end

	while (rlSceneRenderState(view_rl_scene_id)==RL_SCENE_STATE_RENDERING) {
		usleep(10);
	}
	
		// transfer to OpenGL

	view_openrl_transfer_to_opengl();

		// update timer

	view_rl_last_msec=game_time_get_raw();
}

#endif