/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View OpenRL Effects

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
#include "objects.h"

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

	extern int						view_rl_scene_id,
									view_rl_purple_material_id;

	extern int view_openrl_create_material(char *sub_path,texture_type *texture,texture_frame_type *frame);

#endif

/* =======================================================

      Blank Patches
      
======================================================= */

#ifndef D3_OPENRL

	void view_openrl_effect_mesh_setup(effect_type *effect) {}
	void view_openrl_effect_mesh_close(effect_type *effect) {}

#else

/* =======================================================

      OpenRL Effect Meshes
      
======================================================= */

void view_openrl_effect_mesh_setup(effect_type *effect)
{
	effect->openrl_mesh_id=rlSceneMeshAdd(view_rl_scene_id,(RL_MESH_FLAG_NON_LIGHT_BLOCKING|RL_MESH_FLAG_HIGHLIGHT));
}

void view_openrl_effect_mesh_close(effect_type *effect)
{
	if (effect->openrl_mesh_id!=-1) rlSceneMeshDelete(view_rl_scene_id,effect->openrl_mesh_id);
}

/* =======================================================

      Update OpenRL Effect Meshes
      
======================================================= */

void view_openrl_effect_mesh_particle_quad(float *vp,float *uv,d3pnt *pnt,d3ang *rot_ang,float pixel_size,matrix_type *pixel_x_mat,matrix_type *pixel_y_mat,float gravity,float f_count,int particle_count,iface_particle_piece_type *pps,float gx,float gy,float g_size)
{
	int					n,k;
	float				fx,fy,fz,px[4],py[4],pz[4];
	matrix_type			rot_mat;
	
		// if move angle, setup matrixes
		
	if (rot_ang!=NULL) matrix_rotate_xzy(&rot_mat,rot_ang->x,rot_ang->y,rot_ang->z);
	
		// get particle box
		
	px[0]=px[3]=-pixel_size;
	px[1]=px[2]=pixel_size;
	py[0]=py[1]=-pixel_size;
	py[2]=py[3]=pixel_size;
	pz[0]=pz[1]=pz[2]=pz[3]=0.0f;
	
	for (k=0;k!=4;k++) {
		matrix_vertex_multiply_ignore_transform(pixel_x_mat,&px[k],&py[k],&pz[k]);
		matrix_vertex_multiply_ignore_transform(pixel_y_mat,&px[k],&py[k],&pz[k]);
	}
	
		// fill particle arrays
	
	for (n=0;n!=particle_count;n++) {
	
			// particle quad
			
		if (rot_ang!=NULL) {			// rotated particles
			fx=pps->vct.x;
			fy=pps->vct.y;
			fz=pps->vct.z;
			
			matrix_vertex_multiply_ignore_transform(&rot_mat,&fx,&fy,&fz);
		
			fx=(float)(pnt->x+pps->pt.x)+(fx*f_count);
			fy=(float)(pnt->y+pps->pt.y)+((fy*f_count)+gravity);
			fz=(float)(pnt->z+pps->pt.z)+(fz*f_count);
		}
		else {							// normal particles
			fx=(float)(pnt->x+pps->pt.x)+((pps->vct.x)*f_count);
			fy=(float)(pnt->y+pps->pt.y)+(((pps->vct.y)*f_count)+gravity);
			fz=(float)(pnt->z+pps->pt.z)+((pps->vct.z)*f_count);
		}
		
		pps++;

			// the quad

		*vp++=(px[0]+fx);
		*vp++=(py[0]+fy);
		*vp++=(pz[0]+fz);
		
		*uv++=gx;
		*uv++=gy;

		*vp++=(px[1]+fx);
		*vp++=(py[1]+fy);
		*vp++=(pz[1]+fz);
		
		*uv++=gx+g_size;
		*uv++=gy;

		*vp++=(px[2]+fx);
		*vp++=(py[2]+fy);
		*vp++=(pz[2]+fz);
		
		*uv++=gx+g_size;
		*uv++=gy+g_size;

		*vp++=(px[3]+fx);
		*vp++=(py[3]+fy);
		*vp++=(pz[3]+fz);
		
		*uv++=gx;
		*uv++=gy+g_size;

			// change particle image
		
		if (g_size==1) continue;
		
		if (gx<1) {
			gx+=g_size;
			continue;
		}

		gx=0;
		gy+=g_size;
		if (gy>=1) gy=0;
	}
}

void view_openrl_effect_mesh_particle_update(effect_type *effect,int image_offset)
{
	int						n,k,idx,count,particle_count,ntot_count,
							ntrail,pixel_dif;
	short					*polys,*vk;
	float					gravity,gx,gy,g_size,pixel_sz,f,pc[3],trail_step,
							alpha,alpha_dif,color_dif,f_count,f_tick;
	float					*vp,*uv;
	d3pnt					pnt;
	d3ang					*rot_ang,rang;
	d3col					col,ambient_col;
	iface_particle_type		*particle;
	particle_effect_data	*eff_particle;
	matrix_type				pixel_x_mat,pixel_y_mat;
	
	eff_particle=&effect->data.particle;
	particle=&iface.particle_list.particles[eff_particle->particle_idx];

	count=game_time_get()-effect->start_tick;
	f_count=(float)count;

		// position
	
	particle_draw_position(effect,count,&pnt);

	if (particle->ambient_factor!=1.0f) {		// get ambient before position change
		gl_lights_calc_color((float)pnt.x,(float)pnt.y,(float)pnt.z,pc);
		ambient_col.r=pc[0];
		ambient_col.g=pc[1];
		ambient_col.b=pc[2];
	}
	
		// particle move rotation
		// we can have rotations from being attached to a bone of a model
		// or the timed rotations from the particle setup
		// we only want to set these up if the rotation is necessary as it
		// saves time over thousands of particles
		
	rot_ang=NULL;

	if (eff_particle->rot.on) {
		rang.x=eff_particle->rot.ang.x;
		rang.y=eff_particle->rot.ang.y;
		rang.z=eff_particle->rot.ang.z;
		rot_ang=&rang;
	}

	if ((particle->rot.x!=0.0f) || (particle->rot.y!=0.0f) || (particle->rot.z!=0.0f)) {

		if (rot_ang==NULL) {
			rang.x=rang.y=rang.z=0.0f;
			rot_ang=&rang;
		}

		f=f_count*particle->rot.x;
		f+=(f*particle->rot_accel.x);
		rot_ang->x=angle_add(rot_ang->x,f);

		f=f_count*particle->rot.y;
		f+=(f*particle->rot_accel.y);
		rot_ang->y=angle_add(rot_ang->y,f);

		f=f_count*particle->rot.z;
		f+=(f*particle->rot_accel.z);
		rot_ang->z=angle_add(rot_ang->z,f);
	}

		// particle billboard rotation
		
	matrix_rotate_x(&pixel_x_mat,view.render->camera.ang.x);
	matrix_rotate_y(&pixel_y_mat,view.render->camera.ang.y);
	
		// reverse
		
	if (particle->reverse) count=(effect->life_tick-count);

		// setup alpha
		
	f_tick=(float)effect->life_tick;
	
	alpha_dif=particle->end_alpha-particle->start_alpha;
    alpha=particle->start_alpha+((alpha_dif*f_count)/f_tick);
	
		// setup size
		
	pixel_dif=particle->end_pixel_size-particle->start_pixel_size;
    pixel_sz=(float)(particle->start_pixel_size+((pixel_dif*count)/effect->life_tick));
	
		// setup color
		
	color_dif=particle->end_color.r-particle->start_color.r;
    col.r=particle->start_color.r+((color_dif*f_count)/f_tick);
	color_dif=particle->end_color.g-particle->start_color.g;
    col.g=particle->start_color.g+((color_dif*f_count)/f_tick);
	color_dif=particle->end_color.b-particle->start_color.b;
    col.b=particle->start_color.b+((color_dif*f_count)/f_tick);

	col.r*=eff_particle->tint.r;
	col.g*=eff_particle->tint.g;
	col.b*=eff_particle->tint.b;

	if (particle->ambient_factor!=1.0f) {
		col.r=(col.r*particle->ambient_factor)+((1.0f-particle->ambient_factor)*ambient_col.r);
		if (col.r>1.0f) col.r=1.0f;

		col.g=(col.g*particle->ambient_factor)+((1.0f-particle->ambient_factor)*ambient_col.g);
		if (col.g>1.0f) col.g=1.0f;

		col.b=(col.b*particle->ambient_factor)+((1.0f-particle->ambient_factor)*ambient_col.b);
		if (col.b>1.0f) col.b=1.0f;
	}
	
		// setup images
		
	effect_image_animate_get_uv(count,image_offset,&particle->animate,&gx,&gy,&g_size);

		// reduce x/z/y movement and add in offset
		
	count=count/10;

		// construct meshes
		// each particle is a quad

	ntot_count=(particle->count*(particle->trail_count+1))*4;

	rlSceneMeshSetVertex(view_rl_scene_id,effect->openrl_mesh_id,RL_MESH_FORMAT_VERTEX_3_FLOAT,ntot_count,NULL);
	rlSceneMeshMapVertexPointer(view_rl_scene_id,effect->openrl_mesh_id,&vp);
	
	rlSceneMeshSetUV(view_rl_scene_id,effect->openrl_mesh_id,RL_MESH_FORMAT_UV_2_FLOAT,ntot_count,NULL);
	rlSceneMeshMapUVPointer(view_rl_scene_id,effect->openrl_mesh_id,&uv);

		// setup the vertexes

	particle_count=particle->count;
	ntrail=particle->trail_count+1;
	
	trail_step=particle->trail_step;
	f_count=(float)count;
		
	for (n=0;n!=ntrail;n++) {

			// get gravity

		gravity=particle_get_gravity(particle,count);

			// create mesh quads

		view_openrl_effect_mesh_particle_quad(vp,uv,&pnt,rot_ang,pixel_sz,&pixel_x_mat,&pixel_y_mat,gravity,f_count,particle_count,particle->pieces[eff_particle->variation_idx],gx,gy,g_size);

		vp+=((particle->count*3)*4);
		uv+=((particle->count*2)*4);

			// reduce pixel sizes and counts for trails
			
		pixel_sz=pixel_sz*particle->reduce_pixel_fact;
		f_count-=trail_step;
	}

	rlSceneMeshUnMapVertexPointer(view_rl_scene_id,effect->openrl_mesh_id);
	rlSceneMeshUnMapUVPointer(view_rl_scene_id,effect->openrl_mesh_id);

		// build the polygons

	polys=(short*)malloc(sizeof(short)*(ntot_count*10));
	vk=polys;

	idx=0;

	for (n=0;n!=ntot_count;n++) {
		*vk++=4;
		*vk++=view_rl_purple_material_id;
		for (k=0;k!=4;k++) {
			*vk++=idx;
			*vk++=idx;
			idx++;
		}
	}

	rlSceneMeshSetPoly(view_rl_scene_id,effect->openrl_mesh_id,RL_MESH_FORMAT_POLY_SHORT_VERTEX_UV,ntot_count,polys);
	free(polys);
}

void view_openrl_effect_mesh_update(void)
{
	int					n;
	effect_type			*effect;

	for (n=0;n!=max_effect_list;n++) {
		effect=server.effect_list.effects[n];
		if (effect==NULL) continue;

		if (!effect->on) continue;
		if (effect->effecttype!=ef_particle) continue;
		if (effect->openrl_mesh_id==-1) continue;

			
		view_openrl_effect_mesh_particle_update(effect,n);
	}
}

#endif