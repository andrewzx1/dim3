/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: OpenGL Lights

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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "weapons.h"
#include "models.h"
#include "lights.h"
#include "cameras.h"
#include "consoles.h"
#include "video.h"

extern map_type				map;
extern view_type			view;
extern server_type			server;
extern setup_type			setup;

float						light_shader_direction[7][3]={{0.0f,0.0f,0.0f},{1.0f,0.0f,0.0f},{-1.0f,0.0f,0.0f},{0.0f,1.0f,0.0f},{0.0f,-1.0f,0.0f},{0.0f,0.0f,1.0f},{0.0f,0.0f,-1.0f}};

double						light_flicker_value[64]={
														1.00,1.00,1.00,1.00,1.00,1.00,1.00,1.00,
														0.92,0.85,0.70,0.85,0.92,1.00,0.92,0.85,
														0.92,1.00,1.00,1.00,1.00,0.92,0.85,0.70,
														0.50,0.70,0.85,0.70,0.85,0.92,1.00,1.00,
														1.00,1.00,1.00,1.00,0.92,0.85,0.70,0.85,
														0.70,0.85,0.92,1.00,1.00,0.92,0.85,0.70,
														0.50,0.70,0.85,0.92,1.00,1.00,0.92,1.00,
														0.92,0.85,0.70,0.85,0.92,1.00,1.00,1.00
													},
							light_fail_value[64]=	{
														0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
														0.00,0.15,0.45,0.15,0.00,0.00,0.00,0.15,
														0.00,0.00,0.00,0.00,0.00,0.00,0.15,0.45,
														0.85,0.45,0.15,0.45,0.15,0.00,0.00,0.00,
														0.00,0.00,0.00,0.00,0.00,0.15,0.45,0.15,
														0.45,0.15,0.00,0.00,0.00,0.00,0.15,0.45,
														0.85,0.45,0.15,0.00,0.00,0.00,0.00,0.00,
														0.00,0.15,0.45,0.15,0.00,0.00,0.00,0.00
													};

/* =======================================================

      Get Light Intensity For Type
      
======================================================= */

int gl_light_get_intensity(int tick,int light_type,int intensity)
{
	switch (light_type) {
	
		case lt_normal:
			return(intensity);
	
		case lt_blink:
			if ((tick&0x100)==0) return(intensity);
			return(0);
			
		case lt_glow:
			tick=tick&0x7FF;
			if (tick>1024) tick=2048-tick;
			return((int)((float)intensity*(0.75f+(((float)tick)*0.0003f))));
			
		case lt_pulse:
			tick=tick&0x3FF;
			if (tick>512) tick=1024-tick;
			return((int)((float)intensity*(0.5f+(((float)tick)*0.001f))));
			
		case lt_flicker:
			return((int)((float)intensity*light_flicker_value[(tick>>7)&0x3F]));
			break;
			
		case lt_failing:
			return((int)((float)intensity*light_fail_value[(tick>>7)&0x3F]));
			break;
	}
	
	return(intensity);
}

/* =======================================================

      Light Collisions and Directions
      
======================================================= */

inline bool gl_lights_collide_with_box(view_light_spot_type *lspot,d3pnt *min,d3pnt *max)
{
	int				i_add;
	
	i_add=lspot->i_intensity;
		
	if ((lspot->pnt.x+i_add)<min->x) return(FALSE);
	if ((lspot->pnt.x-i_add)>max->x) return(FALSE);
	if ((lspot->pnt.z+i_add)<min->z) return(FALSE);
	if ((lspot->pnt.z-i_add)>max->z) return(FALSE);
	if ((lspot->pnt.y+i_add)<min->y) return(FALSE);
	if ((lspot->pnt.y-i_add)>max->y) return(FALSE);

		// in direction
		
	if (lspot->direction==ld_all) return(TRUE);

	switch (lspot->direction) {

		case ld_neg_x:
			if (min->x>lspot->pnt.x) return(FALSE);
			break;

		case ld_pos_x:
			if (max->x<lspot->pnt.x) return(FALSE);
			break;

		case ld_neg_y:
			if (min->y>lspot->pnt.y) return(FALSE);
			break;

		case ld_pos_y:
			if (max->y<lspot->pnt.y) return(FALSE);
			break;

		case ld_neg_z:
			if (min->z>lspot->pnt.z) return(FALSE);
			break;

		case ld_pos_z:
			if (max->z<lspot->pnt.z) return(FALSE);
			break;

	}
	
	return(TRUE);
}

inline bool gl_lights_direction_ok(double x,double y,double z,view_light_spot_type *lspot)
{
	switch (lspot->direction) {

		case ld_all:
			return(TRUE);

		case ld_neg_x:
			return(x<=lspot->d_x);

		case ld_pos_x:
			return(x>=lspot->d_x);

		case ld_neg_y:
			return(y<=lspot->d_y);

		case ld_pos_y:
			return(y>=lspot->d_y);

		case ld_neg_z:
			return(z<=lspot->d_z);

		case ld_pos_z:
			return(z>=lspot->d_z);

	}

	return(TRUE);
}

/* =======================================================

      Setup Meshes, Liquids, and Models for
	  Lighting Calculations (get a list of hit lights
	  to make later calculations quicker)
      
======================================================= */

int gl_lights_setup_box(d3pnt *mid,d3pnt *min,d3pnt *max,int max_list_idx,int *list_idx)
{
	int							n,k,low_idx,count;
	double						d,dx,dy,dz,low_dist,
								list_dist[max_map_light_cache_index];
	view_light_spot_type		*lspot;

		// find all lights which contact this mesh

	count=0;

	for (n=0;n!=view.render->light.count;n++) {
		lspot=&view.render->light.spots[n];

			// box hit?
			
		if (!gl_lights_collide_with_box(lspot,min,max)) continue;

			// get distance
			// skip square root as we are comparing
			// them against other distances

		dx=mid->x-lspot->pnt.x;
		dy=mid->y-lspot->pnt.y;
		dz=mid->z-lspot->pnt.z;

		d=((dx*dx)+(dy*dy)+(dz*dz));

			// put in list

		if (count<max_list_idx) {
			list_idx[count]=n;
			list_dist[count]=d;
			count++;
		}
		else {
			low_idx=0;
			low_dist=list_dist[0];

			for (k=1;k<count;k++) {
				if (list_dist[k]<low_dist) {
					low_idx=k;
					low_dist=list_dist[k];
				}
			}

			if (d>low_dist) {
				list_idx[low_idx]=n;
				list_dist[low_idx]=d;
			}
		}
	}

	return(count);
}

void gl_lights_setup_liquid(map_liquid_type *liq)
{
	d3pnt			mid,min,max;

	mid.x=(liq->lft+liq->rgt)>>1;
	mid.y=liq->y;
	mid.z=(liq->top+liq->bot)>>1;
	
	min.x=liq->lft;
	min.y=liq->y;
	min.z=liq->top;
	
	max.x=liq->rgt;
	max.y=liq->y;
	max.z=liq->bot;

	liq->light_cache.count=gl_lights_setup_box(&mid,&min,&max,max_shader_light,liq->light_cache.indexes);
}

void gl_lights_setup_model(model_draw *draw)
{
	d3pnt				mid,min,max;

	model_get_view_min_max(draw,&mid,&min,&max);
	draw->light_cache.count=gl_lights_setup_box(&mid,&min,&max,max_shader_light,draw->light_cache.indexes);
}

void gl_lights_setup_cache(void)
{
	int					n;
	obj_type			*obj;
	proj_type			*proj;
	weapon_type			*weap;
	map_mesh_type		*mesh;
	
		// cache all the lights for the meshes, liquids,
		// and models in this view

		// meshes remember all lights hitting them
		// so polygons have a better list.  Everything else
		// just gets max shader lights
		
	for (n=0;n!=view.render->draw_list.count;n++) {

		switch (view.render->draw_list.items[n].type) {
			
			case view_render_type_mesh:
				mesh=&map.mesh.meshes[view.render->draw_list.items[n].idx];
				mesh->light_cache.count=gl_lights_setup_box(&mesh->box.mid,&mesh->box.min,&mesh->box.max,max_map_light_cache_index,mesh->light_cache.indexes);
				break;

			case view_render_type_liquid:
				gl_lights_setup_liquid(&map.liquid.liquids[view.render->draw_list.items[n].idx]);
				break;

			case view_render_type_object:
				obj=&server.objs[view.render->draw_list.items[n].idx];
				if ((view.render->draw_list.items[n].flag&view_list_item_flag_model_in_view)!=0x0) gl_lights_setup_model(&obj->draw);
				
				if ((obj->uid==server.player_obj_uid) && (obj->held_weapon.current_uid!=-1)) {
					weap=weapon_find_uid(obj->held_weapon.current_uid);
					if (weap!=NULL) gl_lights_setup_model(&weap->draw);
				}
				break;

			case view_render_type_projectile:
				proj=&server.projs[view.render->draw_list.items[n].idx];
				if ((view.render->draw_list.items[n].flag&view_list_item_flag_model_in_view)!=0x0) gl_lights_setup_model(&proj->draw);
				break;
				
		}
	}
}

/* =======================================================

      Compile Light List
      
======================================================= */

void gl_lights_compile_add(int tick,d3pnt *pnt,int light_type,bool light_map,int intensity,float exponent,int direction,d3col *col)
{
	view_light_spot_type			*lspot;
	
		// already too many lights?

	if (view.render->light.count==max_light_spot) return;

		// calculate intensity

	intensity=gl_light_get_intensity(tick,light_type,intensity);
	if (intensity<=0) return;
	
		// create light

	lspot=&view.render->light.spots[view.render->light.count];
	
		// create intensity for light type
		
	lspot->intensity=intensity;

	lspot->i_intensity=(int)lspot->intensity;			// need alternate versions of data to speed up later calculations
	lspot->f_intensity=(float)lspot->intensity;
	
	lspot->light_map=light_map;
	lspot->exponent=exponent;
	lspot->direction=direction;
	
		// create light position and color
		
	memmove(&lspot->pnt,pnt,sizeof(d3pnt));
	memmove(&lspot->col,col,sizeof(d3col));

		// non shader calculation speed ups

	lspot->d_intensity=lspot->intensity*lspot->intensity;
	lspot->d_inv_intensity=1.0/lspot->d_intensity;

	lspot->f_x=(float)lspot->pnt.x;
	lspot->f_y=(float)lspot->pnt.y;
	lspot->f_z=(float)lspot->pnt.z;
		
	lspot->d_x=(double)lspot->f_x;
	lspot->d_y=(double)lspot->f_y;
	lspot->d_z=(double)lspot->f_z;
	
	lspot->d_col_r=(double)lspot->col.r;
	lspot->d_col_g=(double)lspot->col.g;
	lspot->d_col_b=(double)lspot->col.b;

		// eye space for shaders

	lspot->pnt_eye_space.x=lspot->f_x;
	lspot->pnt_eye_space.y=lspot->f_y;
	lspot->pnt_eye_space.z=lspot->f_z;
		
	gl_project_to_eye_coordinates(&lspot->pnt_eye_space.x,&lspot->pnt_eye_space.y,&lspot->pnt_eye_space.z);

	view.render->light.count++;
}

void gl_lights_compile_model_add(int tick,model_draw *draw)
{
	int					n;
	d3pnt				pnt;
	model_type			*mdl;
	model_draw_light	*light;

		// any model?
		
	mdl=NULL;
	if ((draw->uid!=-1) && (draw->on)) mdl=model_find_uid(draw->uid);
	
		// add lights
		
	light=draw->lights;
	
	for (n=0;n!=max_model_light;n++) {

		if (light->on) {
			memmove(&pnt,&draw->pnt,sizeof(d3pnt));
			
			if (mdl!=NULL) {
				model_get_light_position(mdl,&draw->setup,n,&pnt.x,&pnt.y,&pnt.z);
				if (draw->no_rot.on) gl_project_fix_rotation(&pnt.x,&pnt.y,&pnt.z);
			}
			
			gl_lights_compile_add(tick,&pnt,light->type,FALSE,light->intensity,light->exponent,light->direction,&light->col);
		}

		light++;
	}
}

void gl_lights_compile_effect_add(int tick,effect_type *effect)
{
	int					intensity,count,mid_tick,fade_tick;
	flash_effect_data	*flash;
	
	if (effect->effecttype!=ef_flash) return;

		// calculate flash
		
	flash=&effect->data.flash;

	intensity=flash->intensity;
	mid_tick=flash->mid_tick;
	
	count=tick-effect->start_tick;

	if (count<mid_tick) {										// start of flash
		if (mid_tick>0) intensity=(intensity*count)/mid_tick;
	}
	else {
		if (count>mid_tick) {									// fade of flash
			fade_tick=(effect->life_tick-flash->mid_tick);
			count=fade_tick-(count-mid_tick);
	
			if (fade_tick>0) intensity=(intensity*count)/fade_tick;
		}
	}
	
	gl_lights_compile_add(tick,&effect->pnt,lt_normal,FALSE,intensity,flash->exponent,ld_all,&flash->col);
}

void gl_lights_compile(int tick)
{
	int					n;
	map_light_type		*maplight;
	obj_type			*obj;
	weapon_type			*weap;
	proj_type			*proj;
	effect_type			*effect;
	
	view.render->light.count=0;

		// map lights
		
	maplight=map.lights;
		
	for (n=0;n!=map.nlight;n++) {
		if (maplight->on) gl_lights_compile_add(tick,&maplight->pnt,maplight->type,maplight->light_map,maplight->intensity,maplight->exponent,maplight->direction,&maplight->col);
		maplight++;
	}	

		// lights from objects and their weapons
	
	obj=server.objs;
	
	for (n=0;n!=server.count.obj;n++) {
		gl_lights_compile_model_add(tick,&obj->draw);
		if (obj->held_weapon.current_uid!=-1) {
			weap=weapon_find_uid(obj->held_weapon.current_uid);
			if (weap!=NULL) gl_lights_compile_model_add(tick,&weap->draw);
		}
		obj++;
	}
	
		// lights from projectiles

	proj=server.projs;
	
	for (n=0;n!=server.count.proj;n++) {
		gl_lights_compile_model_add(tick,&proj->draw);
		proj++;
	}
	
		// lights from effects
		
	effect=server.effects;
		
	for (n=0;n!=server.count.effect;n++) {
		gl_lights_compile_effect_add(tick,effect);		
		effect++;
	}
	
		// setup per render item caches

	gl_lights_setup_cache();
}

/* =======================================================

      Find Closest Light
      
======================================================= */

view_light_spot_type* gl_light_find_closest_light(double x,double y,double z)
{
	int						n,k;
	double					dx,dz,dy,d,dist;
	view_light_spot_type	*lspot;

		// no lights in scene

	if (view.render->light.count==0) return(NULL);

		// find closest light
	
	k=-1;
	dist=-1;
	
	for (n=0;n!=view.render->light.count;n++) {

		lspot=&view.render->light.spots[n];
		
			// get distance to light spot
			
		dx=lspot->d_x-x;
		dz=lspot->d_z-z;
		dy=lspot->d_y-y;

		d=(dx*dx)+(dz*dz)+(dy*dy);
		
			// reject lights outside globe
			// and in wrong direction

		if (d<=lspot->d_intensity) {

			if (gl_lights_direction_ok(x,y,z,lspot)) {
		
					// compare distances
			
				if ((d<dist) || (dist==-1)) {
					dist=d;
					k=n;
				}
			}
		}
		
		lspot++;
	}
	
	if (k==-1) return(NULL);

	return(&view.render->light.spots[k]);
}

/* =======================================================

      Calculate Ambient Only Lighting
      
======================================================= */

void gl_lights_calc_ambient_color(d3col *col)
{
	col->r=map.ambient.light_color.r+setup.gamma;
	col->g=map.ambient.light_color.g+setup.gamma;
	col->b=map.ambient.light_color.b+setup.gamma;
}

/* =======================================================

      Calculate Lighting at a Point
      
======================================================= */

void gl_lights_calc_color(double x,double y,double z,float *cf)
{
	int						n;
	double					dx,dz,dy,r,g,b,d,mult;
	view_light_spot_type	*lspot;

		// combine all light spots attenuated for distance
		
	r=g=b=0.0;

	lspot=view.render->light.spots;
	
	for (n=0;n!=view.render->light.count;n++) {

		dx=lspot->d_x-x;
		dy=lspot->d_y-y;
		dz=lspot->d_z-z;
		
		d=(dx*dx)+(dz*dz)+(dy*dy);

		if (d<=lspot->d_intensity) {
			if (gl_lights_direction_ok(x,y,z,lspot)) {

				mult=(lspot->d_intensity-d)*lspot->d_inv_intensity;
				
				mult+=pow(mult,lspot->exponent);

				r+=(lspot->d_col_r*mult);
				g+=(lspot->d_col_g*mult);
				b+=(lspot->d_col_b*mult);
			}
		}

		lspot++;
	}

		// set light value

	*cf++=(map.ambient.light_color.r+setup.gamma)+(float)r;
	*cf++=(map.ambient.light_color.g+setup.gamma)+(float)g;
	*cf=(map.ambient.light_color.b+setup.gamma)+(float)b;
}

void gl_lights_calc_color_light_cache(int count,int *indexes,bool skip_light_map,double x,double y,double z,float *cf)
{
	int						n;
	double					dx,dz,dy,r,g,b,d,mult;
	view_light_spot_type	*lspot;

		// combine all light spots attenuated for distance
		
	r=g=b=0.0;
	
	for (n=0;n!=count;n++) {
	
		lspot=&view.render->light.spots[indexes[n]];
		if ((skip_light_map) && (lspot->light_map)) continue;

		dx=lspot->d_x-x;
		dy=lspot->d_y-y;
		dz=lspot->d_z-z;
		
		d=(dx*dx)+(dz*dz)+(dy*dy);

		if (d<=lspot->d_intensity) {

			if (gl_lights_direction_ok(x,y,z,lspot)) {

				mult=(lspot->d_intensity-d)*lspot->d_inv_intensity;
				
				mult+=pow(mult,lspot->exponent);

				r+=(lspot->d_col_r*mult);
				g+=(lspot->d_col_g*mult);
				b+=(lspot->d_col_b*mult);
			}
		}
	}

		// set light value

	*cf++=(map.ambient.light_color.r+setup.gamma)+(float)r;
	*cf++=(map.ambient.light_color.g+setup.gamma)+(float)g;
	*cf=(map.ambient.light_color.b+setup.gamma)+(float)b;
}

/* =======================================================

      GLSL Light Lists
      
======================================================= */

void gl_lights_build_poly_light_list(int mesh_idx,map_mesh_poly_type *poly,view_light_list_type *light_list)
{
	int						n,k,count,low_idx;
	double					d,dx,dy,dz,low_dist,
							list_dist[max_shader_light];
	map_mesh_type			*mesh;
	view_light_spot_type	*lspot;

		// meshes already have a reduced light list
		
	mesh=&map.mesh.meshes[mesh_idx];
	
		// if the mesh already has <= max_shader_light
		// lights, just use them

	if (mesh->light_cache.count<max_shader_light) {
		light_list->nlight=mesh->light_cache.count;
		
		for (n=0;n!=light_list->nlight;n++) {
			light_list->light_idx[n]=mesh->light_cache.indexes[n];
		}
		
		return;
	}

		// otherwise we need to calc the
		// closest from the list
		
	count=0;
		
	for (n=0;n!=mesh->light_cache.count;n++) {
		lspot=&view.render->light.spots[mesh->light_cache.indexes[n]];

			// box hit?
			
		if (!gl_lights_collide_with_box(lspot,&poly->box.min,&poly->box.max)) continue;

			// get distance
			// skip square root as we are comparing
			// them against other distances

		dx=poly->box.mid.x-lspot->pnt.x;
		dy=poly->box.mid.y-lspot->pnt.y;
		dz=poly->box.mid.z-lspot->pnt.z;

		d=((dx*dx)+(dy*dy)+(dz*dz));

			// put in list

		if (count<max_shader_light) {
			light_list->light_idx[count]=mesh->light_cache.indexes[n];
			list_dist[count]=d;
			count++;
		}
		else {
			low_idx=0;
			low_dist=list_dist[0];

			for (k=1;k<count;k++) {
				if (list_dist[k]<low_dist) {
					low_idx=k;
					low_dist=list_dist[k];
				}
			}

			if (d>low_dist) {
				light_list->light_idx[low_idx]=mesh->light_cache.indexes[n];
				list_dist[low_idx]=d;
			}
		}
	}
		
	light_list->nlight=count;
}

void gl_lights_build_liquid_light_list(map_liquid_type *liq,view_light_list_type *light_list)
{
	int				n;

	light_list->nlight=liq->light_cache.count;
		
	for (n=0;n!=light_list->nlight;n++) {
		light_list->light_idx[n]=liq->light_cache.indexes[n];
	}
}

void gl_lights_build_model_light_list(model_draw *draw,view_light_list_type *light_list)
{
	int				n;

	light_list->nlight=draw->light_cache.count;
		
	for (n=0;n!=light_list->nlight;n++) {
		light_list->light_idx[n]=draw->light_cache.indexes[n];
	}
}
