/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View Setup

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
	#include "dim3engine.h"
#endif

#include "objects.h"
#include "remotes.h"
#include "weapons.h"
#include "projectiles.h"
#include "models.h"
#include "lights.h"
#include "effects.h"
#include "cameras.h"
#include "consoles.h"
#include "interfaces.h"
#include "video.h"
#include "timing.h"

extern bool					shadow_on;

extern map_type				map;
extern camera_type			camera;
extern view_type			view;
extern server_type			server;
extern setup_type			setup;

extern bool fog_solid_on(void);
extern bool mesh_inview(map_mesh_type *mesh);
extern bool mesh_shadow_inview(map_mesh_type *mesh);
extern bool model_inview(model_draw *draw);
extern bool model_shadow_inview(model_draw *draw);
extern double distance_to_view_center(int x,int y,int z);
extern bool boundbox_inview(int x,int z,int ex,int ez,int ty,int by);
extern bool effect_inview(effect_type *effect,int count,d3pnt *center_pnt);

/* =======================================================

      Map Visibility Obscuring
      
======================================================= */

inline void view_visibility_check_clear(void)
{
	bzero(view.obscure.grid,obscure_grid_byte_size);
}

inline bool view_visibility_check_get(int x,int z)
{
	int				idx,offset;
	unsigned char	*byte;

	idx=(x>>3)+(z*obscure_grid_byte_row_size);
	offset=x&0x7;

	byte=view.obscure.grid+idx;
	return(((*byte)&(0x1<<offset))==0x0);
}

inline void view_visibility_check_set(int x,int z)
{
	int				idx,offset;
	unsigned char	*byte;

	idx=(x>>3)+(z*obscure_grid_byte_row_size);
	offset=x&0x7;

	byte=view.obscure.grid+idx;
	*byte=(*byte)|(0x1<<offset);
}

void view_visibility_check_calculate(void)
{
	int				n,x,z,lft,rgt,top,bot;
	d3pnt			map_min,map_max;
	map_mesh_type	*mesh;

		// build a grid of spots
		// that represent blank areas (areas with no
		// meshes) in the map.  These areas will obscure
		// meshes behind them

		// get total map size

	mesh=map.mesh.meshes;

	memmove(&map_min,&mesh->box.min,sizeof(d3pnt));
	memmove(&map_max,&mesh->box.max,sizeof(d3pnt));
	mesh++;

	for (n=1;n<map.mesh.nmesh;n++) {
		if (mesh->box.min.x<map_min.x) map_min.x=mesh->box.min.x;
		if (mesh->box.max.x>map_max.x) map_max.x=mesh->box.max.x;
		if (mesh->box.min.z<map_min.z) map_min.z=mesh->box.min.z;
		if (mesh->box.max.z>map_max.z) map_max.z=mesh->box.max.z;
		mesh++;
	}

		// get grid size

	view.obscure.x_size=(map_max.x-map_min.x)/obscure_grid_division;
	view.obscure.z_size=(map_max.z-map_min.z)/obscure_grid_division;

	view.obscure.x_start=map_min.x;
	view.obscure.z_start=map_min.z;

		// find obscuring points

	view_visibility_check_clear();

	for (z=0;z!=obscure_grid_division;z++) {

		top=view.obscure.z_start+(z*view.obscure.z_size);
		bot=top+view.obscure.z_size;

		for (x=0;x!=obscure_grid_division;x++) {
			
			lft=view.obscure.x_start+(x*view.obscure.x_size);
			rgt=lft+view.obscure.x_size;

			mesh=map.mesh.meshes;

			for (n=0;n!=map.mesh.nmesh;n++) {
				if ((lft>mesh->box.max.x) || (top>mesh->box.max.z) || (rgt<mesh->box.min.x) || (bot<mesh->box.min.z)) {
					mesh++;
					continue;
				}
				view_visibility_check_set(x,z);
				break;
			}
		}
	}
}

bool view_visibility_check_obscure_line(d3pnt *pt1,d3pnt *pt2)
{
	int			n,x,z,x2,z2,kx,kz,count;
	float		fx,fz,xadd,zadd;

		// get points in obscure space

	x=(pt1->x-view.obscure.x_start)/view.obscure.x_size;
	z=(pt1->z-view.obscure.z_start)/view.obscure.z_size;

	x2=(pt2->x-view.obscure.x_start)/view.obscure.x_size;
	z2=(pt2->z-view.obscure.z_start)/view.obscure.z_size;

		// automatically OK points next to
		// on another

	kx=abs(x-x2);
	kz=abs(z-z2);

	if ((kx<=1) && (kz<=1)) return(TRUE);

		// trace the line looking for obscure
		// points (0x0) in grid

	if (kx>kz) {
		xadd=1.0f;
		zadd=((float)kz)/((float)kx);
		count=kx;
	}
	else {
		xadd=((float)kx)/((float)kz);
		zadd=1.0f;
		count=kz;
	}

	if (x2<x) xadd=-xadd;
	if (z2<z) zadd=-zadd;

	fx=(float)x;
	fz=(float)z;

	for (n=0;n!=count;n++) {
		if (view_visibility_check_get((int)fx,(int)fz)) return(FALSE);

		fx+=xadd;
		fz+=zadd;
	}

		// no hit, view lines don't pass through
		// obscured point

	return(TRUE);
}

bool view_visibility_check_box(d3pnt *mid,d3pnt *min,d3pnt *max)
{
	int				x,z,xsz,zsz;
	d3pnt			pt;

		// find the grid size for box

	xsz=(max->x-min->x)/view.obscure.x_size;
	if (xsz<1) xsz=1;
	if (xsz>obscure_grid_max_box_side_length) xsz=obscure_grid_max_box_side_length;

	zsz=(max->z-min->z)/view.obscure.z_size;
	if (zsz<1) zsz=1;
	if (zsz>obscure_grid_max_box_side_length) zsz=obscure_grid_max_box_side_length;

		// see if we can trace to the grid points
		// of a box (in 2D) without hitting any
		// obscure zones (0x0 in grid.)  If we can
		// get to the eyepoint without being blocked,
		// then use the mesh

	for (z=0;z<=zsz;z++) {
		for (x=0;x<=xsz;x++) {
			pt.x=min->x+(((max->x-min->x)*x)/xsz);
			pt.z=min->z+(((max->z-min->z)*z)/zsz);
			if (view_visibility_check_obscure_line(&view.render->camera.pnt,&pt)) return(TRUE);
		}
	}

	return(FALSE);
}

inline bool view_visibility_check_mesh(int eye_mesh_idx,map_mesh_type *mesh)
{
	return(view_visibility_check_box(&mesh->box.mid,&mesh->box.min,&mesh->box.max));
}

bool view_visibility_check_liquid(int eye_mesh_idx,map_liquid_type *liq)
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

	return(view_visibility_check_box(&mid,&min,&max));
}

/* =======================================================

      Draw Lists
      
======================================================= */

void view_start_draw_list(void)
{
	view.render->draw_list.count=0;
}

void view_add_draw_list(int item_type,int item_idx,double item_dist,int item_flag)
{
	int				t,idx,sz;

		// room for any more items?

	if (view.render->draw_list.count==max_view_render_item) return;
	
		// find place in sorted list

	idx=-1;

	for (t=0;t!=view.render->draw_list.count;t++) {
		if (view.render->draw_list.items[t].dist>item_dist) {
			idx=t;
			break;
		}
	}

		// insert at end of list
		
	if (idx==-1) {
		view.render->draw_list.items[view.render->draw_list.count].type=item_type;
		view.render->draw_list.items[view.render->draw_list.count].dist=item_dist;
		view.render->draw_list.items[view.render->draw_list.count].flag=item_flag;
		view.render->draw_list.items[view.render->draw_list.count].idx=(short)item_idx;
	}
	
		// insert in list
	
	else {
		sz=sizeof(view_render_draw_list_item_type)*(view.render->draw_list.count-idx);
		memmove(&view.render->draw_list.items[idx+1],&view.render->draw_list.items[idx],sz);
		
		view.render->draw_list.items[idx].type=item_type;
		view.render->draw_list.items[idx].dist=item_dist;
		view.render->draw_list.items[idx].flag=item_flag;
		view.render->draw_list.items[idx].idx=(short)item_idx;
	}

		// add up list

	view.render->draw_list.count++;
}

bool view_mesh_in_draw_list(int mesh_idx)
{
	int			n;

	for (n=0;n!=view.render->draw_list.count;n++) {
		if (view.render->draw_list.items[n].type==view_render_type_mesh) {
			if (view.render->draw_list.items[n].idx==mesh_idx) return(TRUE);
		}
	}

	return(FALSE);
}

bool view_obj_in_draw_list(int obj_idx)
{
	int			n;

	for (n=0;n!=view.render->draw_list.count;n++) {
		if (view.render->draw_list.items[n].type==view_render_type_object) {
			if (view.render->draw_list.items[n].idx==obj_idx) return(TRUE);
		}
	}

	return(FALSE);
}

bool view_proj_in_draw_list(int proj_idx)
{
	int			n;

	for (n=0;n!=view.render->draw_list.count;n++) {
		if (view.render->draw_list.items[n].type==view_render_type_projectile) {
			if (view.render->draw_list.items[n].idx==proj_idx) return(TRUE);
		}
	}

	return(FALSE);
}

/* =======================================================

      Add Map Items to Draw Lists
      
======================================================= */

void view_add_mesh_draw_list(int start_mesh_idx)
{
	int					n;
	double				d,obscure_dist;
	map_mesh_type		*start_mesh,*mesh;
	
	start_mesh=&map.mesh.meshes[start_mesh_idx];

		// obscure distance -- normally is the opengl projection
		// distance but can be the fog distance if fog is on

	if (!fog_solid_on()) {
		obscure_dist=(double)(camera.setup.plane.far_z-camera.setup.plane.near_z);
	}
	else {
		obscure_dist=(double)((map.fog.outer_radius>>1)*3);
	}
	
		// check all visibile meshes from the start mesh
	
	for (n=0;n!=map.mesh.nmesh;n++) {

		mesh=&map.mesh.meshes[n];
		if (!mesh->flag.on) continue;
			
		if (!mesh->flag.never_obscure) {
			
				// auto-eliminate meshes drawn outside the obscure distance
				
			d=map_mesh_calculate_distance(mesh,&view.render->camera.pnt);
			if (d>obscure_dist) continue;
				
				// check if bound box is within view
				
			if (!mesh_inview(mesh)) {
				if (!mesh->flag.shadow) continue;
				if (!mesh_shadow_inview(mesh)) continue;
			}

				// check if obscured by other meshes
				// this only works in fpp

			if ((n!=start_mesh_idx) && (camera.setup.mode==cv_fpp)) {
				if (!view_visibility_check_mesh(start_mesh_idx,mesh)) continue;
			}
		}
		else {
			d=map_mesh_calculate_distance(mesh,&view.render->camera.pnt);
		}

			// sort meshes into drawing list

		view_add_draw_list(view_render_type_mesh,n,d,0x0);
	}
}

void view_add_liquid_draw_list(int start_mesh_idx)
{
	int					n,mx,mz;
	double				d,obscure_dist;
	map_liquid_type		*liq;

		// obscure distance -- normally is the opengl projection
		// distance but can be the fog distance if fog is on

	if (!fog_solid_on()) {
		obscure_dist=(double)(camera.setup.plane.far_z-camera.setup.plane.near_z);
	}
	else {
		obscure_dist=(double)((map.fog.outer_radius>>1)*3);
	}

		// find all drawable liquids

	for (n=0;n!=map.liquid.nliquid;n++) {

		liq=&map.liquid.liquids[n];
		
			// no draw liquids are volume only, so skip
			// any drawing
			
		if (liq->no_draw) continue;
		
		if (!liq->never_obscure) {
		
				// skip liquids with away facing normals
				// do dot product between normal and vector
				// from liq mid-eye point
				// liquid normal is always facing up (0,-1,0)
				// so this calculation is realitively easy
				
			if ((!liq->never_cull) && (!map.settings.never_cull)) {
				mx=(liq->lft+liq->rgt)>>1;
				mz=(liq->top+liq->bot)>>1;
				
				if ((-1.0f*(float)(liq->y-view.render->camera.pnt.y))>0.0f) continue;
			}

				// auto-eliminate liquids drawn outside the obscure distance
					
			d=map_liquid_calculate_distance(liq,&view.render->camera.pnt);
			if (d>obscure_dist) continue;
					
				// check if liquid within bound box

			if (!boundbox_inview(liq->lft,liq->top,liq->rgt,liq->bot,liq->y,liq->y)) continue;
			
				// check if obscured by other meshes

			if (camera.setup.mode==cv_fpp) {
				if (!view_visibility_check_liquid(start_mesh_idx,liq)) continue;
			}
		}
		else {
			d=map_liquid_calculate_distance(liq,&view.render->camera.pnt);
		}
		
			// sort liquids into drawing list

		view_add_draw_list(view_render_type_liquid,n,d,0x0);
	}
}

void view_add_mesh_liquid_draw_list(void)
{
	int				start_mesh_idx;

		// get mesh camera is in

	start_mesh_idx=map_mesh_find_always(&map,&view.render->camera.pnt);

		// add meshes

	view_add_mesh_draw_list(start_mesh_idx);

		// add liquids

	view_add_liquid_draw_list(start_mesh_idx);
}

/* =======================================================

      Model Setup and Draw List
      
======================================================= */

bool view_setup_model_in_view(model_draw *draw,int mesh_idx)
{
	double					obscure_dist;
	
	if ((draw->model_idx==-1) || (!draw->on)) return(FALSE);

		// is model in a mesh that's in the mesh draw list?

	if (mesh_idx!=-1) {
		if (!view_mesh_in_draw_list(mesh_idx)) return(FALSE);
	}
	
		// is model within obscure distance

	if (!fog_solid_on()) {
		obscure_dist=(double)(camera.setup.plane.far_z-camera.setup.plane.near_z);
	}
	else {
		obscure_dist=(double)((map.fog.outer_radius>>1)*3);
	}
		
	draw->draw_dist=distance_to_view_center(draw->pnt.x,draw->pnt.y,draw->pnt.z);
	if (draw->draw_dist>=obscure_dist) return(FALSE);
	
		// is model in view?
	
	return(model_inview(draw));
}

bool view_setup_shadow_in_view(model_draw *draw,int mesh_idx)
{
	double					obscure_dist;

	if ((draw->model_idx==-1) || (!draw->on)) return(FALSE);

		// is model in a mesh that's in the mesh draw list?

	if (mesh_idx!=-1) {
		if (!view_mesh_in_draw_list(mesh_idx)) return(FALSE);
	}
	
		// is model within obscure distance

	if (!fog_solid_on()) {
		obscure_dist=(double)(camera.setup.plane.far_z-camera.setup.plane.near_z);
	}
	else {
		obscure_dist=(double)((map.fog.outer_radius>>1)*3);
	}
		
	draw->draw_dist=distance_to_view_center(draw->pnt.x,draw->pnt.y,draw->pnt.z);
	if (draw->draw_dist>=obscure_dist) return(FALSE);
	
		// is model in view?

	return(model_shadow_inview(draw));
}

void view_setup_objects(int tick)
{
	int					n,flag;
	bool				is_camera;
	obj_type			*obj;
	weapon_type			*weap;
	
	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;
		
		if (obj->hidden) continue;
		
		is_camera=((camera.setup.mode==cv_fpp) && (obj->idx==camera.obj_idx));
		
			// setup model positions
			
		object_rigid_body_reset_angle(obj);
		object_fly_reset_angle(obj);
		model_draw_setup_object(obj);
		
		model_calc_animation(&obj->draw,tick);
		model_calc_draw_bones(&obj->draw);

			// detect if model or shadow is in view
			
		flag=0x0;
		
		if ((is_camera) && (!view.render->force_camera_obj)) {
			flag|=view_list_item_flag_shadow_in_view;
		}
		else {
			if (view_setup_model_in_view(&obj->draw,obj->mesh.cur_mesh_idx)) flag|=view_list_item_flag_model_in_view;

			if (obj->draw.shadow.on) {
				if ((flag&view_list_item_flag_model_in_view)!=0x0) {		// model in view means shadow is automatically in view
					flag|=view_list_item_flag_shadow_in_view;
				}
				else {
					if (view_setup_shadow_in_view(&obj->draw,obj->mesh.cur_mesh_idx)) flag|=view_list_item_flag_shadow_in_view;
				}
			}
		}
		
			// not in view, skip out
	
		if (flag==0x0) continue;
		
			// add to draw list

		obj->draw.built_vertex_list=FALSE;
		view_add_draw_list(view_render_type_object,n,obj->draw.draw_dist,flag);
	
			// setup model in view
		
		render_model_setup(&obj->draw,tick);
		
			// setup held weapon model
			// if player

		if ((obj->idx==server.player_obj_idx) && (obj->held_weapon.current_idx!=-1)) {
			weap=weapon_find_current(obj);
			if (weap!=NULL) {
				model_draw_setup_weapon(obj,weap,FALSE,FALSE);
				view_setup_model_in_view(&weap->draw,obj->mesh.cur_mesh_idx);
			}
		}
	}
}

void view_setup_projectiles(int tick)
{
	int					n,mesh_idx,flag;
	proj_type			*proj;
	
	for (n=0;n!=max_proj_list;n++) {
		proj=server.proj_list.projs[n];
		if (!proj->on) continue;
		
			// setup model positions
			
		projectile_reset_angle_for_flight(proj);
		model_draw_setup_projectile(proj);
		
		model_calc_animation(&proj->draw,tick);
		model_calc_draw_bones(&proj->draw);
		
			// find model and shadows in view
			
		flag=0x0;
		
		mesh_idx=map_mesh_find(&map,&proj->draw.pnt);
		if (view_setup_model_in_view(&proj->draw,mesh_idx)) flag|=view_list_item_flag_model_in_view;

		if (proj->draw.shadow.on) {
			if ((flag&view_list_item_flag_model_in_view)!=0x0) {		// model in view means shadow is automatically in view
				flag|=view_list_item_flag_shadow_in_view;
			}
			else {
				if (model_shadow_inview(&proj->draw)) flag|=view_list_item_flag_shadow_in_view;
			}
		}
	
		if (flag==0x0) continue;

			// add to draw list
			// projs share vertex list so we can't do the "built_vertex_list" optimization

		view_add_draw_list(view_render_type_projectile,n,proj->draw.draw_dist,flag);
		
			// setup model in view
			
		render_model_setup(&proj->draw,tick);
	}
}

/* =======================================================

      Effects in View
      
======================================================= */

void view_add_effect_draw_list(int tick)
{
	int					n;
	double				d,obscure_dist;
	d3pnt				center_pnt;
	effect_type			*effect;

		// obscure distance -- normally is the opengl projection
		// distance but can be the fog distance if fog is on

	if (!fog_solid_on()) {
		obscure_dist=(double)(camera.setup.plane.far_z-camera.setup.plane.near_z);
	}
	else {
		obscure_dist=(double)((map.fog.outer_radius>>1)*3);
	}

		// find all effects

	for (n=0;n!=max_effect_list;n++) {
		effect=server.effect_list.effects[n];
		if (!effect->on) continue;
		
			// effect inside a mesh that's hidden?

		if (effect->mesh_idx!=-1) {
			if (!view_mesh_in_draw_list(effect->mesh_idx)) continue;
		}
				
			// check if effect within bound box

		if (!effect_inview(effect,(tick-effect->start_tick),&center_pnt)) continue;

			// auto-eliminate effects drawn outside the obscure distance
				
		d=distance_to_view_center(center_pnt.x,center_pnt.y,center_pnt.z);
		if (d>obscure_dist) continue;

			// sort effects into drawing list
			
		view_add_draw_list(view_render_type_effect,n,d,0x0);
	}
}

/* =======================================================

      Halos in View
      
======================================================= */

void view_add_model_halo(model_draw *draw,int obj_idx)
{
	int					n,x,z,y;
	model_type			*mdl;
	model_draw_halo		*halo;

		// any model?
		
	if ((draw->model_idx==-1) || (!draw->on)) return;
	
	mdl=server.model_list.models[draw->model_idx];
	
		// add halo
		
	halo=draw->halos;

	for (n=0;n!=max_model_halo;n++) {

		if (halo->on) {
			x=draw->pnt.x;
			y=draw->pnt.y;
			z=draw->pnt.z;
			
			model_get_halo_position(mdl,&draw->setup,n,&x,&y,&z);
			if (draw->no_rot.on) gl_project_fix_rotation(&x,&y,&z);
			
			halo_draw_add(x,y,z,obj_idx,halo);
		}

		halo++;
	}
}

void view_add_halos(void)
{
	int					n;
	obj_type			*obj;
	proj_type			*proj;

		// halos from objects and their weapons
		
	for (n=0;n!=view.render->draw_list.count;n++) {

		switch (view.render->draw_list.items[n].type) {

			case view_render_type_object:
				obj=server.obj_list.objs[view.render->draw_list.items[n].idx];
				view_add_model_halo(&obj->draw,obj->idx);
				break;

			case view_render_type_projectile:
				proj=server.proj_list.projs[view.render->draw_list.items[n].idx];
				view_add_model_halo(&proj->draw,-1);
				break;

		}
	}
}

/* =======================================================

      Scope FOV changes
      
======================================================= */

void view_calculate_scope(obj_type *obj,obj_type *camera_obj)
{
	int				tick;
	float			f_step,f_max_step,f_zoom,
					sway_add,sway_max;
	weapon_type		*weap;
	
		// is object the camera object?

	if (camera_obj->idx!=obj->idx) return;
	
		// scoped weapon on?
	
	weap=weapon_find_current(obj);
	if (weap==NULL) return;
	
	if ((!weap->zoom.on) || (weap->zoom.mode==zoom_mode_off)) return;
	
		// calculate fov
		
	if (weap->zoom.step_count<=1) {
		view.render->camera.fov=weap->zoom.fov_max;
		return;
	}
		
	f_step=(float)weap->zoom.current_step;
	f_max_step=(float)(weap->zoom.step_count-1);
	
	f_zoom=(weap->zoom.fov_max-(((weap->zoom.fov_max-weap->zoom.fov_min)/f_max_step)*f_step))-view.render->camera.fov;
	
		// zoom look down sites

	tick=game_time_get();

	switch (weap->zoom.mode) {

		case zoom_mode_in:
			if (tick<(obj->zoom_draw.start_tick+weap->zoom.tick)) {
				f_zoom*=((float)(tick-obj->zoom_draw.start_tick)/(float)weap->zoom.tick);
			}
			else {
				weap->zoom.mode=zoom_mode_on;
			}
			break;

		case zoom_mode_out:
			if (tick<(obj->zoom_draw.start_tick+weap->zoom.tick)) {
				f_zoom*=1.0f-((float)(tick-obj->zoom_draw.start_tick)/(float)weap->zoom.tick);
			}
			else {
				weapon_zoom_off(obj,weap);
				return;
			}
			break;
	}
	
		// change perspective
	
	view.render->camera.fov+=f_zoom;

		// no sways until zoom modes is on

	if (weap->zoom.mode!=zoom_mode_on) return;
	if (weap->zoom.sway_factor==0.0f) return;

	sway_add=0.025f*weap->zoom.sway_factor;
	sway_max=3.0f*weap->zoom.sway_factor;

		// first time start of sways

	if (obj->zoom_draw.sway_reset) {
		obj->zoom_draw.sway_reset=FALSE;
		obj->zoom_draw.sway_ang.x=obj->zoom_draw.sway_ang.y=0.0f;
		obj->zoom_draw.sway_next.x=random_float(sway_max)-(sway_max/2.0f);
		obj->zoom_draw.sway_next.y=random_float(sway_max)-(sway_max/2.0f);
	}

		// calculate any sways

	if (obj->zoom_draw.sway_next.x<obj->zoom_draw.sway_ang.x) {
		obj->zoom_draw.sway_ang.x-=sway_add;
		if (obj->zoom_draw.sway_next.x>=obj->zoom_draw.sway_ang.x) {
			obj->zoom_draw.sway_next.x=random_float(sway_max)-(sway_max/2.0f);
		}
	}
	else {
		obj->zoom_draw.sway_ang.x+=sway_add;
		if (obj->zoom_draw.sway_next.x<=obj->zoom_draw.sway_ang.x) {
			obj->zoom_draw.sway_next.x=random_float(sway_max)-(sway_max/2.0f);
		}
	}

	if (obj->zoom_draw.sway_next.y<obj->zoom_draw.sway_ang.y) {
		obj->zoom_draw.sway_ang.y-=sway_add;
		if (obj->zoom_draw.sway_next.y>=obj->zoom_draw.sway_ang.y) {
			obj->zoom_draw.sway_next.y=random_float(sway_max)-(sway_max/2.0f);
		}
	}
	else {
		obj->zoom_draw.sway_ang.y+=sway_add;
		if (obj->zoom_draw.sway_next.y<=obj->zoom_draw.sway_ang.y) {
			obj->zoom_draw.sway_next.y=random_float(sway_max)-(sway_max/2.0f);
		}
	}

		// set sways

	view.render->camera.ang.x+=obj->zoom_draw.sway_ang.x;
	view.render->camera.ang.y+=obj->zoom_draw.sway_ang.y;
}

void view_calculate_recoil(obj_type *obj)
{
	weapon_type		*weap;

	weap=weapon_find_current(obj);
	if (weap==NULL) return;

	weapon_recoil_add(obj,weap,&view.render->camera.ang);
}

/* =======================================================

      Shakes and Sways
      
======================================================= */

void view_calculate_shakes(obj_type *obj)
{
	int					n,k,d,shake_sz,shake_cnt;
	float				shake_freq;
	effect_type			*effect;
	
	shake_cnt=0;
	shake_freq=0.0f;
	shake_sz=0;
	
		// find any shake effects
		
	for (n=0;n!=max_effect_list;n++) {
		effect=server.effect_list.effects[n];
		if (!effect->on) continue;
	
		if (effect->effecttype==ef_shake) {
			
			d=distance_get(obj->pnt.x,obj->pnt.y,obj->pnt.z,effect->pnt.x,effect->pnt.y,effect->pnt.z);
			if (d<effect->data.shake.distance) {
				shake_sz+=effect->size;
				shake_freq+=(0.3f-((0.3f*(float)d)/(float)effect->data.shake.distance))+0.2f;
				shake_cnt++;
			}
		}
	}
	
		// any shake data?
		
	if (shake_cnt==0) return;

	shake_sz=shake_sz/shake_cnt;
	shake_freq=shake_freq/(float)shake_cnt;
	
		// do shake
		
	k=(int)((float)(game_time_get()-effect->start_tick)*shake_freq)%shake_sz;
	if (k>(shake_sz>>1)) k=shake_sz-k;
	
	view.render->camera.ang.x=view.render->camera.ang.x+((float)k/25.0f);
}

void view_calculate_sways(obj_type *obj)
{
	float				f;

	return;

	f=(float)(((game_time_get()>>4)%12)-6);
	view.render->camera.ang.z+=f;

	// supergumba -- work on sways

}

void view_calculate_bump(obj_type *obj)
{
	if (obj->bump.on) view.render->camera.pnt.y+=obj->bump.smooth_offset;
}

/* =======================================================

      View Script 3D to 2D transform
      
======================================================= */

void view_script_transform_3D_to_2D(int x,int y,int z,int *x2,int *y2)
{
	gl_3D_view();
	gl_3D_rotate(&view.render->camera.pnt,&view.render->camera.ang);
	gl_setup_project();
	
	gl_project_point(&x,&y,&z);
	
	*x2=x;
	*y2=y;
}

