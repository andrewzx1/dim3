/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Shadow Volumes

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit �Created with dim3 Technology� is given on a single
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
	#include "dim3engine.h"
#endif

#include "models.h"
#include "physics.h"
#include "video.h"

extern int					shadow_pbuffer_pixel_size;

extern map_type				map;
extern setup_type			setup;

int							shadow_texture_count;

extern bool complex_boundbox_inview(int *cbx,int *cby,int *cbz);
extern void model_render_shadow(model_draw *draw,float draw_z,int shadow_idx);
extern light_spot_type* map_find_closest_light(portal_type *portal,double x,double y,double z,int *p_dist);

/* =======================================================

      Ground Shadow Volume
      
======================================================= */

bool shadow_get_volume_ground(model_draw *draw)
{
	int							x,z,y,xadd,zadd,
								*px,py[4],*pz,
								wid,high,dist;
	float						fx,fy,fz,model_ang,volume_ang;
	matrix_type					mat;
	light_spot_type				*lspot;
	model_type					*mdl;
	model_draw_shadow			*shadow;
	
	mdl=model_find_uid(draw->uid);
	if (mdl==NULL) return(FALSE);
	
	shadow=&draw->shadow;
	
		// get position
		
	x=draw->pos.x;
	y=draw->pos.y;
	z=draw->pos.z;
	
	fx=(float)mdl->shadow_box.offset.x;
	fy=(float)mdl->shadow_box.offset.y;
	fz=(float)mdl->shadow_box.offset.z;
	
	matrix_rotate_y(&mat,draw->setup.ang.y);
	matrix_vertex_multiply(&mat,&fx,&fy,&fz);

	x+=(int)fx;
	y+=(int)fy;
	z+=(int)fz;
	
		// normal shadow mode
		
	if (shadow->mode==sm_normal) {

			// find closest light
		
		lspot=map_find_closest_light(&map.portals[draw->pos.rn],(double)x,(double)y,(double)z,&dist);
		if (lspot==NULL) return(FALSE);

			// find angle and size

		volume_ang=angle_find(lspot->pnt.x,lspot->pnt.z,x,z);
		shadow->alpha=0.55f-((0.5f*(float)dist)/(float)lspot->intensity);
		high=((mdl->shadow_box.size.y<<2)*dist)/(int)lspot->intensity;
	}
	
		// static shadow mode
		
	else {
		volume_ang=shadow->static_face_angle;
		shadow->alpha=shadow->static_alpha;
		high=shadow->static_size;
	}
	
		// shadow fudging
		
	if (mdl->shadow_fudge!=0) {
		angle_get_movement(volume_ang,mdl->shadow_fudge,&xadd,&zadd);
		x+=xadd;
		z+=zadd;
	}
	
		// starting position and size

	model_ang=angle_add(volume_ang,-draw->setup.ang.y);
		
	shadow->pos.rn=draw->pos.rn;
	shadow->pos.x=x;
	shadow->pos.z=z;
	shadow->pos.y=draw->pos.y;
	shadow->ang.y=model_ang;
	shadow->ang.x=0;
	
	shadow->high=high;
	
		// find polygon for volume
	
	px=shadow->px;
	pz=shadow->pz;
	
	wid=mdl->shadow_box.size.x;
	
	px[0]=px[3]=x-wid;
	px[1]=px[2]=x+wid;
	pz[0]=pz[1]=z-high;
	pz[2]=pz[3]=z;
	py[0]=py[1]=py[2]=py[3]=0;

	rotate_polygon(4,px,py,pz,x,0,z,0,volume_ang,0);
	
	return(TRUE);
}

/* =======================================================

      Air Shadow Volume
      
======================================================= */

bool shadow_get_volume_air(model_draw *draw)
{
	int						x,z,y,wid,high,fy,*px,py[4],*pz;
	float					alpha;
	model_type				*mdl;
	model_draw_shadow		*shadow;
	
	mdl=model_find_uid(draw->uid);
	if (mdl==NULL) return(FALSE);
	
	shadow=&draw->shadow;

		// find shadow floor

	x=draw->pos.x;
	z=draw->pos.z;
	y=draw->pos.y;
		
    fy=find_poly_nearest_stand(x,(y-draw->size.y),z,(map_enlarge*100),TRUE);
    if (fy==-1) return(FALSE);

		// setup volume

	wid=mdl->shadow_box.size.x;
	high=mdl->shadow_box.size.z;

	shadow->pos.rn=draw->pos.rn;
	shadow->pos.x=x;
	shadow->pos.z=z;
	shadow->pos.y=fy;
	shadow->ang.y=0;
	shadow->ang.x=90;
	
	shadow->high=high;
	
    alpha=0.5f-((float)(fy-y)/(float)(map_enlarge*50));
	if (alpha<0.1f) alpha=0.1f;
	
	shadow->alpha=alpha;
	
		// find polygon for volume
	
	px=shadow->px;
	pz=shadow->pz;
	
	wid=wid>>1;
	high=high>>1;
	
	px[0]=px[3]=x-wid;
	px[1]=px[2]=x+wid;
	pz[0]=pz[1]=z-high;
	pz[2]=pz[3]=z+high;
	py[0]=py[1]=py[2]=py[3]=0;

	rotate_polygon(4,px,py,pz,x,0,z,0,draw->setup.ang.y,0);
	
	return(TRUE);
}

/* =======================================================

      Get Shadow Volume
      
======================================================= */

bool shadow_get_volume(model_draw *draw,bool in_air)
{
		// static shadow changes?
		
	if (draw->shadow.mode==sm_static) {
		in_air=draw->shadow.static_in_air;
	}
	
		// get specific volumes
		
	if ((!in_air) && (!draw->shadow.always_in_air)) {
		return(shadow_get_volume_ground(draw));
	}
	
	return(shadow_get_volume_air(draw));
}

/* =======================================================

      Find in Shadow Volume is in View
      
======================================================= */

bool shadow_inview(model_draw *draw)
{
	int					i,rn,y,px[8],py[8],pz[8];
	bool				path_ok;
	model_draw_shadow	*shadow;
	
	shadow=&draw->shadow;
	
		// look for shadow in path
		
	path_ok=FALSE;
	
	for ((i=0);(i!=4);i++) {
		rn=map_find_portal(&map,shadow->px[i],shadow->pos.y,shadow->pz[i]);
		if (rn==-1) continue;
		if (map.portals[rn].in_path) {
			path_ok=TRUE;
			break;
		}
	}
	
	if (!path_ok) return(FALSE);
		
		// check if shadow is in view
		
	y=shadow->pos.y;
	
	memmove(&px[0],shadow->px,(sizeof(int)*4));
	memmove(&pz[0],shadow->pz,(sizeof(int)*4));
	py[0]=py[1]=py[2]=py[3]=y-shadow->high;
	
	memmove(&px[4],shadow->px,(sizeof(int)*4));
	memmove(&pz[4],shadow->pz,(sizeof(int)*4));
	py[4]=py[5]=py[6]=py[7]=y;
	
	return(complex_boundbox_inview(px,py,pz));
}

/* =======================================================

      Start/Stop Shadow Texture Creation
      
======================================================= */

bool shadow_texture_init(void)
{
		// shadows on?

	if (setup.shadow_mode==shadow_mode_none) return(FALSE);

		// start shadow slicing
		
	shadow_texture_count=0;
	
		// switch to shadow pbuffer
		
	if (!gl_shadow_texture_render_start()) return(FALSE);

		// setup pbuffer for drawing
		
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0f,shadow_pbuffer_gl_transform_size,0.0f,shadow_pbuffer_gl_transform_size);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glViewport(0,0,shadow_pbuffer_pixel_size,shadow_pbuffer_pixel_size);
		
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT);

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);

	return(TRUE);
}

void shadow_texture_finish(void)
{
	if (setup.shadow_mode==shadow_mode_none) return;

		// flush so shadows draw correctly
		
	glFlush();
	
		// return to original gl context

	gl_shadow_texture_render_end();
}

/* =======================================================

      Create Shadow Texture from Model
      
======================================================= */

void shadow_texture_create(model_draw *draw)
{
	int					x_org,y_org,slice_pixel_size;
	model_type			*mdl;
	model_draw_shadow   *shadow;
	model_draw			shd_draw;
	
		// any more shadow slices?
		
	shadow=&draw->shadow;
	shadow->texture_idx=-1;
	
	if (shadow_texture_count==(shadow_pbuffer_slice_count*shadow_pbuffer_slice_count)) return;
	
		// get model
	
	mdl=model_find_uid(draw->uid);
	if (mdl==NULL) return;
	
		// shadow texture index and slices
		// shadows are all drawn on a single pbuffer, split into slices
		
	shadow->texture_idx=shadow_texture_count;

	slice_pixel_size=(shadow_pbuffer_pixel_size/shadow_pbuffer_slice_count);
	
	x_org=slice_pixel_size*(shadow_texture_count%shadow_pbuffer_slice_count);
	y_org=slice_pixel_size*(shadow_texture_count/shadow_pbuffer_slice_count);

	glViewport(x_org,y_org,slice_pixel_size,slice_pixel_size);

	shadow_texture_count++;

	// this is used for testing shadow
/*
	glLineWidth(2.0f);
	
	glColor4f(1,0,0,1);
	glBegin(GL_LINES);
	glVertex2f(0.0f,0.0f);
	glVertex2f(shadow_pbuffer_gl_transform_size,shadow_pbuffer_gl_transform_size);
	glVertex2f(shadow_pbuffer_gl_transform_size,0.0f);
	glVertex2f(0.0f,shadow_pbuffer_gl_transform_size);
	glEnd();
	
	glColor4f(0,1,0,1);
	glBegin(GL_LINE_LOOP);
	glVertex2f(0.0f,0.0f);
	glVertex2f(shadow_pbuffer_gl_transform_size,0.0f);
	glVertex2f(shadow_pbuffer_gl_transform_size,shadow_pbuffer_gl_transform_size);
	glVertex2f(0.0f,shadow_pbuffer_gl_transform_size);
	glEnd();
	
	shadow->alpha=1.0f;
*/
		// draw model

	memmove(&shd_draw,draw,sizeof(model_draw));
	
	model_clear_draw_setup(mdl,&shd_draw.setup);
	
	shd_draw.setup.ang.y=-shadow->ang.y;
	shd_draw.setup.ang.x=shadow->ang.x;
	shd_draw.setup.ang.z=0;

	model_render_shadow(&shd_draw,shadow_pbuffer_gl_transform_size,shadow->texture_idx);
}


