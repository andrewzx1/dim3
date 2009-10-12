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

int							light_cur_mesh_reduce_idx,
							light_spot_reduce_count,light_spot_reduce_idx[max_light_spot];

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

extern bool light_inview(d3pnt *pnt,int intensity);

/* =======================================================

      Get Light Intensity For Type
      
======================================================= */

double gl_light_get_intensity(int tick,int light_type,int intensity)
{
	double			d_tick,d_intensity;
	
	d_intensity=(double)intensity;
	
	switch (light_type) {
	
		case lt_normal:
			return(d_intensity);
	
		case lt_blink:
			if ((tick&0x100)==0) return(d_intensity);
			return(0);
			
		case lt_glow:
			d_tick=(double)(tick&0x7FF);
			if (d_tick>1024) d_tick=2048-d_tick;
			return(d_intensity*(0.75+(d_tick/4096)));
			
		case lt_pulse:
			d_tick=(double)(tick&0x3FF);
			if (d_tick>512) d_tick=1024-d_tick;
			return(d_intensity*(0.5+(d_tick/1024)));
			
		case lt_flicker:
			d_intensity*=light_flicker_value[(tick>>7)&0x3F];
			break;
			
		case lt_failing:
			d_intensity*=light_fail_value[(tick>>7)&0x3F];
			break;
	}
	
	return(d_intensity);
}

/* =======================================================

      Compile Light List
      
======================================================= */

void gl_lights_compile_add(int tick,d3pnt *pnt,int light_type,int filter,int intensity,float exponent,int direction,d3col *col)
{
	view_light_spot_type			*lspot;
	
		// already too many lights?

	if (view.render->light.count==max_light_spot) return;
	if (intensity<=0) return;

		// is light in view?

	if (!light_inview(pnt,intensity)) return;

		// create light

	lspot=&view.render->light.spots[view.render->light.count];
	
		// create intensity for light type
		
	lspot->intensity=gl_light_get_intensity(tick,light_type,intensity);
	if (lspot->intensity<=0) return;

	lspot->i_intensity=(int)lspot->intensity;			// need alternate versions of data to speed up later calculations
	
	lspot->filter=filter;
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
			
			gl_lights_compile_add(tick,&pnt,light->type,light->filter,light->intensity,light->exponent,light->direction,&light->col);
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
	
	gl_lights_compile_add(tick,&effect->pnt,lt_normal,lf_none,intensity,1.0f,ld_all,&flash->col);
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
		if (maplight->on) gl_lights_compile_add(tick,&maplight->pnt,maplight->type,maplight->filter,maplight->intensity,maplight->exponent,maplight->direction,&maplight->col);
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

		// this flag allows us to reduce
		// the amount of lights as we work through
		// the polygons of a mesh for shader lights

	light_cur_mesh_reduce_idx=-1;
}

/* =======================================================

      Turn Indexed list of Lights into Array
      
======================================================= */

void gl_lights_fill_light_list(int set_light_count,view_glsl_light_list_type *light_list)
{
	int						n,idx;
	view_light_spot_type	*lspot;
	
	idx=0;
	
	for (n=0;n!=set_light_count;n++) {
		
			// null lights

			// default shaders get all lights set so we
			// have to null out unused lights
			
		if (n>=light_list->nlight) {
			light_list->pos[idx]=0.0f;
			light_list->pos[idx+1]=0.0f;
			light_list->pos[idx+2]=0.0f;
			
			light_list->col[idx]=0.0f;
			light_list->col[idx+1]=0.0f;
			light_list->col[idx+2]=0.0f;
			
			light_list->intensity[n]=0.0f;		// effectively turns light off
			light_list->exponent[n]=1.0f;

			light_list->direction[idx]=0.0f;
			light_list->direction[idx+1]=0.0f;
			light_list->direction[idx+2]=0.0f;
		}
		
			// regular lights
			
		else {
			lspot=&view.render->light.spots[light_list->light_idx[n]];
			
			light_list->pos[idx]=lspot->f_x;
			light_list->pos[idx+1]=lspot->f_y;
			light_list->pos[idx+2]=lspot->f_z;
			
			light_list->col[idx]=lspot->col.r;
			light_list->col[idx+1]=lspot->col.g;
			light_list->col[idx+2]=lspot->col.b;
			
			light_list->intensity[n]=(float)lspot->intensity;
			light_list->exponent[n]=lspot->exponent;

			light_list->direction[idx]=light_shader_direction[lspot->direction][0];
			light_list->direction[idx+1]=light_shader_direction[lspot->direction][1];
			light_list->direction[idx+2]=light_shader_direction[lspot->direction][2];
		}
		
		idx+=3;
	}
}

/* =======================================================

      Light Direction Elimination
      
======================================================= */

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

      Create Light Lists For Mesh
      
======================================================= */

bool gl_lights_collide_with_box(view_light_spot_type *lspot,d3pnt *min,d3pnt *max)
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

/* =======================================================

      Light Spot Reductions
      
======================================================= */

void gl_lights_spot_reduce_none(void)
{
	int				n;

	light_spot_reduce_count=view.render->light.count;
	
	for (n=0;n!=light_spot_reduce_count;n++) {
		light_spot_reduce_idx[n]=n;
	}
}

void gl_lights_spot_reduce_box(d3pnt *min,d3pnt *max,bool is_mesh)
{
	int				n;

	light_spot_reduce_count=0;

	for (n=0;n!=view.render->light.count;n++) {
	
			// check filtering
			
		switch (view.render->light.spots[n].filter) {
		
			case lf_mesh_only:
				if (!is_mesh) continue;
				break;
				
			case lf_not_mesh:
				if (is_mesh) continue;
				break;
		}
		
			// check box collision
			
		if (gl_lights_collide_with_box(&view.render->light.spots[n],min,max)) {
			light_spot_reduce_idx[light_spot_reduce_count]=n;
			light_spot_reduce_count++;
		}
	}
}

/* =======================================================

      Per Vertex non GLSL Lighting
      
======================================================= */

void gl_lights_calc_vertex(double x,double y,double z,float *cf)
{
	int						n;
	double					dx,dz,dy,r,g,b,d,mult;
	view_light_spot_type	*lspot;

		// combine all light spots attenuated for distance
		
	r=g=b=0.0;
	
	for (n=0;n!=light_spot_reduce_count;n++) {

		lspot=&view.render->light.spots[light_spot_reduce_idx[n]];

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

void gl_lights_get_ambient(d3col *col)
{
	col->r=map.ambient.light_color.r+setup.gamma;
	col->g=map.ambient.light_color.g+setup.gamma;
	col->b=map.ambient.light_color.b+setup.gamma;
}

void gl_lights_calc_vertex_setup_none(void)
{
	gl_lights_spot_reduce_none();
}

bool gl_lights_calc_vertex_setup_mesh(map_mesh_type *mesh)
{
	gl_lights_spot_reduce_box(&mesh->box.min,&mesh->box.max,TRUE);
	return(light_spot_reduce_count!=0);
}

bool gl_lights_calc_vertex_setup_liquid(map_liquid_type *liq)
{
	d3pnt			min,max;
	
	min.x=liq->lft;
	min.y=liq->y;
	min.z=liq->top;
	
	max.x=liq->rgt;
	max.y=liq->y;
	max.z=liq->bot;

	gl_lights_spot_reduce_box(&min,&max,TRUE);
	
	return(light_spot_reduce_count!=0);
}

bool gl_lights_calc_vertex_setup_model(model_draw *draw)
{
	d3pnt			pnt,min,max;

	model_get_view_min_max(draw,&pnt,&min,&max);
	gl_lights_spot_reduce_box(&min,&max,FALSE);
	
	return(light_spot_reduce_count!=0);
}

/* =======================================================

      GLSL Lights
      
======================================================= */

void gl_lights_build_from_box(d3pnt *mid,d3pnt *min,d3pnt *max,view_glsl_light_list_type *light_list)
{
	int						n,k,
							idx,cnt,sort_list[max_light_spot];
	double					d,dx,dy,dz,sort_dist[max_light_spot];
	view_light_spot_type	*lspot;
	
		// start with no lights
		
	light_list->nlight=0;
	
		// special check for single light
		
	if (light_spot_reduce_count==1) {
		lspot=&view.render->light.spots[light_spot_reduce_idx[0]];
		if (!gl_lights_collide_with_box(lspot,min,max)) return;
		
		light_list->nlight=1;
		light_list->light_idx[0]=light_spot_reduce_idx[0];

		return;
	}
	
		// multiple lights on poly
		// sort the light spots, only using
		// top max_shader_light

	cnt=0;

	for (n=0;n!=light_spot_reduce_count;n++) {

		lspot=&view.render->light.spots[light_spot_reduce_idx[n]];
		
			// does light hit this box?
			
		if (!gl_lights_collide_with_box(lspot,min,max)) continue;

			// get distance
			// don't need square root as we are only comparing them
			// against one another

		dx=(double)(lspot->pnt.x-mid->x);
		dy=(double)(lspot->pnt.y-mid->y);
		dz=(double)(lspot->pnt.z-mid->z);

		d=(dx*dx)+(dy*dy)+(dz*dz);
		
			// special check for first light
			
		if (cnt==0) {
			sort_dist[cnt]=d;
			sort_list[cnt]=light_spot_reduce_idx[n];
			cnt++;
			continue;
		}

			// find position in list (top is closest)

		idx=-1;
	
		for (k=0;k!=cnt;k++) {
			if (sort_dist[k]>d) {
				idx=k;
				break;
			}
		}

			// since we can only use max_shader_light lights,
			// ignore all lights that sort beyond max_shader_light

		if ((idx>=max_shader_light) || ((idx==-1) && (cnt>=max_shader_light))) continue;
	
			// insert at end of list
			
		if (idx==-1) {
			sort_dist[cnt]=d;
			sort_list[cnt]=light_spot_reduce_idx[n];
			cnt++;
			continue;
		}
		
			// insert in list
			
		memmove(&sort_dist[idx+1],&sort_dist[idx],(sizeof(double)*(cnt-idx)));
		memmove(&sort_list[idx+1],&sort_list[idx],(sizeof(int)*(cnt-idx)));
		
		sort_dist[idx]=d;
		sort_list[idx]=light_spot_reduce_idx[n];
		
		cnt++;
		if (cnt>=max_shader_light) cnt=max_shader_light;
	}
	
		// create the light list
		
	light_list->nlight=cnt;

	for (n=0;n!=cnt;n++) {
		light_list->light_idx[n]=sort_list[n];
	}
}

void gl_lights_build_from_poly(int mesh_idx,map_mesh_poly_type *poly,view_glsl_light_list_type *light_list)
{
	map_mesh_type		*mesh;

	if (light_cur_mesh_reduce_idx!=mesh_idx) {
		light_cur_mesh_reduce_idx=mesh_idx;

		mesh=&map.mesh.meshes[mesh_idx];
		gl_lights_spot_reduce_box(&mesh->box.min,&mesh->box.max,TRUE);
	}

	gl_lights_build_from_box(&poly->box.mid,&poly->box.min,&poly->box.max,light_list);
}

void gl_lights_build_from_liquid(map_liquid_type *liq,view_glsl_light_list_type *light_list)
{
	d3pnt			mid,min,max;

	light_cur_mesh_reduce_idx=-1;
	
	mid.x=(liq->lft+liq->rgt)>>1;
	mid.y=liq->y;
	mid.z=(liq->top+liq->bot)>>1;
	
	min.x=liq->lft;
	min.y=liq->y;
	min.z=liq->top;
	
	max.x=liq->rgt;
	max.y=liq->y;
	max.z=liq->bot;
	
	gl_lights_spot_reduce_box(&min,&max,TRUE);
	gl_lights_build_from_box(&mid,&min,&max,light_list);
}

void gl_lights_build_from_model(model_draw *draw,view_glsl_light_list_type *light_list)
{
	d3pnt			mid,min,max;

	light_cur_mesh_reduce_idx=-1;
	
	model_get_view_min_max(draw,&mid,&min,&max);
	gl_lights_spot_reduce_box(&min,&max,FALSE);
	gl_lights_build_from_box(&mid,&min,&max,light_list);
}

