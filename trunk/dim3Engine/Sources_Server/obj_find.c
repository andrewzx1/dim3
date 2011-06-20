/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Object Finding

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

#include "interface.h"
#include "objects.h"

extern map_type				map;
extern server_type			server;
extern network_setup_type	net_setup;
extern js_type				js;

/* =======================================================

      Find Objects
      
======================================================= */

inline obj_type* object_get_attach(JSObjectRef j_obj)
{
	int					script_idx;
	script_type			*script;

	script_idx=(int)JSObjectGetPrivate(j_obj);
	script=js.script_list.scripts[script_idx];
	
	return(server.obj_list.objs[script->attach.obj_idx]);
}

obj_type* object_find_remote_net_uid(int net_uid)
{
	int				n;
	obj_type		*obj;
	
	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;

		if (obj->remote.net_uid==net_uid) return(obj);
	}
	
	return(NULL);
}

/* =======================================================

      Find Objects by Name and Tag
      
======================================================= */

obj_type* object_find_name(char *name)
{
	int			n;
	obj_type	*obj;
	
	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;

		if (strcasecmp(obj->name,name)==0) return(obj);
	}
	
	return(NULL);
}

/* =======================================================

      Find Objects by Position
      
======================================================= */

obj_type* object_find_nearest(d3pnt *pt,char *name,int type,int team_idx,float ang,float ang_sweep,int min_dist,int max_dist,bool player,int skip_obj_idx)
{
	int				n,i,d,dist;
	float			fang;
	obj_type		*obj;

	i=-1;
    dist=max_dist;
	
	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;
		
		if (obj->hidden) continue;
		if (!obj->find_on) continue;
		
			// player and remotes
			
		if (player) {
			if ((obj->type!=object_type_player) && (obj->type!=object_type_bot_multiplayer) && (obj->type!=object_type_remote)) continue;
		}

		if (obj->idx==skip_obj_idx) continue;
		
			// check name
			
		if (name!=NULL) {
			if (strstr(obj->name,name)==NULL) continue;
		}
		
			// check type
			
		if (type!=-1) {
			if (obj->type!=type) continue;
		}
		
			// check team
			
		if (team_idx!=-1) {
			if (obj->team_idx!=team_idx) continue;
		}
		
			// check distance
			
		d=distance_get(pt->x,pt->y,pt->z,obj->pnt.x,obj->pnt.y,obj->pnt.z);
		if (d<min_dist) continue;
		if (d>dist) continue;
		
			// check angle
			
		if (ang!=-1) {
			fang=angle_find(pt->x,pt->z,obj->pnt.x,obj->pnt.z);
			if (angle_dif(fang,ang,NULL)>ang_sweep) continue;
		}
		
		i=n;
		dist=d;
	}
	
	if (i==-1) return(NULL);
	
	return(server.obj_list.objs[i]);
}

/* =======================================================

      Teams
      
======================================================= */

int object_count_team(int team_idx,int ignore_obj_idx)
{
	int				n,count;
	obj_type		*obj;

	count=0;

	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;
		
		if ((obj->idx!=ignore_obj_idx) && (obj->team_idx==team_idx)) count++;
	}

	return(count);
}

void object_set_even_team(obj_type *obj)
{
	if (object_count_team(net_team_red,obj->idx)<object_count_team(net_team_blue,obj->idx)) {
		obj->team_idx=net_team_red;
	}
	else {
		obj->team_idx=net_team_blue;
	}
}

/* =======================================================

      Find Click Object
      
======================================================= */

int object_find_idx_click_object(obj_type *obj)
{
	int				n,idx,d,dist;
	float			fang;
	obj_type		*click_obj;
	
	idx=-1;
	dist=0;

	for (n=0;n!=max_obj_list;n++) {
		click_obj=server.obj_list.objs[n];
		if (click_obj==NULL) continue;
		
		if (click_obj->hidden) continue;
		if (!click_obj->click.on) continue;
		
			// check distance
			
		d=distance_get(obj->pnt.x,obj->pnt.y,obj->pnt.z,click_obj->pnt.x,click_obj->pnt.y,click_obj->pnt.z);
		if (d>click_obj->click.distance) continue;
		if ((idx!=-1) && (d>dist)) continue;
		
			// check angle
			
		fang=angle_find(obj->pnt.x,obj->pnt.z,click_obj->pnt.x,click_obj->pnt.z);
		if (angle_dif(fang,obj->ang.y,NULL)>ci_object_click_angle) continue;
		
		idx=click_obj->idx;
		dist=d;
	}
	
	return(idx);
}

/* =======================================================

      Find Objects by Standing Under
      
======================================================= */

int object_find_idx_by_stood_on_object_idx(int stand_obj_idx)
{
	int				n;
	obj_type		*obj;
	
	for (n=0;n!=max_obj_list;n++) {
		if (stand_obj_idx==n) continue;
		
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;

		if ((obj->touch.stand) && (obj->touch.obj_idx==stand_obj_idx)) return(obj->idx);
	}
	
	return(-1);
}

/* =======================================================

      Sight Path Tests
      
======================================================= */

bool object_sight_test_object(obj_type *obj,int test_obj_idx)
{
	int						x,y,side_div,look_div;
	float					side_start,look_start,side_add,look_add;
	d3pnt					spt,hpt;
	d3ang					ang;
	ray_trace_contact_type	contact;
	
	if (test_obj_idx==-1) return(FALSE);

		// setup contact

	contact.obj.on=TRUE;
	contact.proj.on=FALSE;

	contact.obj.ignore_idx=obj->idx;
	contact.proj.ignore_idx=-1;

	contact.origin=poly_ray_trace_origin_object;

		// set divisions

	side_div=obj->sight.side_division;
	if (side_div<=0) side_div=1;

	look_div=obj->sight.look_division;
	if (look_div<=0) look_div=1;

		// get ray angles

	if (side_div==1) {
		side_start=obj->ang.y;
		side_add=0.0f;
	}
	else {
		side_start=obj->ang.y-(obj->sight.side_angle/2.0f);
		side_add=(obj->sight.side_angle/(float)side_div);
	}

	if (look_div==1) {
		look_start=obj->ang.x;
		look_add=0.0f;
	}
	else {
		look_start=obj->ang.x-(obj->sight.look_angle/2.0f);
		look_add=(obj->sight.look_angle/(float)look_div);
	}

		// run the ray traces

	ang.x=look_start;
	ang.z=0.0f;

	for (y=0;y!=look_div;y++) {

		ang.y=side_start;

		for (x=0;x!=side_div;x++) {

				// get starting position

			spt.x=obj->pnt.x;
			spt.y=obj->pnt.y+obj->size.eye_offset;
			spt.z=obj->pnt.z;

			ray_push(&spt,&ang,obj->size.radius);

				// ray trace

			if (ray_trace_map_by_angle(&spt,&ang,obj->sight.distance,&hpt,&contact)) {
				if (contact.obj.idx==test_obj_idx) return(TRUE);
			}

			ang.y=angle_add(ang.y,side_add);		// these angles are constrained 0...360
		}

		ang.x+=look_add;			// these angles are constrained -x...x
	}


	return(FALSE);
}

/* =======================================================

      Nearest Object Searches
      
======================================================= */

int object_find_neareset_unheld_weapon_node(obj_type *obj,int from_node_idx)
{
	int				n,k,idx,d,dist;
	obj_type		*weap_obj;
	weapon_type		*weap;
	spot_type		*spot;

		// check all unheld weapons
		
	idx=-1;
	dist=-1;
	
	for (n=0;n!=max_weap_list;n++) {

			// get the weapon

		weap=obj->weap_list.weaps[n];
		if (weap==NULL) continue;
		if (!weap->hidden) continue;

			// find all spots with the name of the weapon

		for (k=0;k!=map.nspot;k++) {
			spot=&map.spots[k];
			if (strcmp(spot->name,weap->name)!=0) continue;
			if (spot->lookup.nearest_node_idx==-1) continue;
			if (spot->lookup.spawned_obj_idx==-1) continue;
			
				// is weapon hidden?
			
			weap_obj=server.obj_list.objs[spot->lookup.spawned_obj_idx];
			if (weap_obj->hidden) continue;

				// get distance

			d=map_node_to_node_distance(&map,from_node_idx,spot->lookup.nearest_node_idx);
			if (d==-1) continue;

			if ((dist==-1) || (d<dist)) {
				dist=d;
				idx=spot->lookup.nearest_node_idx;
			}
		}
	}
	
	return(idx);
}

int object_find_neareset_held_weapon_ammo_node(obj_type *obj,int from_node_idx,bool empty_only)
{
	int				n,k,idx,d,dist;
	char			name[name_str_len];
	obj_type		*ammo_obj;
	weapon_type		*weap;
	spot_type		*spot;

		// check all unheld weapons
		
	idx=-1;
	dist=-1;
	
	for (n=0;n!=max_weap_list;n++) {

			// get the weapon

		weap=obj->weap_list.weaps[n];
		if (weap==NULL) continue;
		if (!weap->hidden) continue;

			// empty only flag

		if (empty_only) {
			if (weap->ammo.count!=0) continue;
		}

			// find all spots with the name of the weapon+ammo

		strcpy(name,weap->name);
		string_safe_strcat(name," Ammo",name_str_len);

		for (k=0;k!=map.nspot;k++) {
			spot=&map.spots[k];
			if (strcmp(spot->name,name)!=0) continue;
			if (spot->lookup.nearest_node_idx==-1) continue;
			if (spot->lookup.spawned_obj_idx==-1) continue;
			
				// is ammo hidden?
			
			ammo_obj=server.obj_list.objs[spot->lookup.spawned_obj_idx];
			if (ammo_obj->hidden) continue;

				// get distance

			d=map_node_to_node_distance(&map,from_node_idx,spot->lookup.nearest_node_idx==-1);
			if (d==-1) continue;

			if ((dist==-1) || (d<dist)) {
				dist=d;
				idx=spot->lookup.nearest_node_idx;
			}
		}
	}
	
	return(idx);
}

int object_find_neareset_armor_node(obj_type *obj,int from_node_idx)
{
	int				n,idx,d,dist;
	spot_type		*spot;

		// check all unheld weapons
		
	idx=-1;
	dist=-1;

	for (n=0;n!=map.nspot;n++) {
		spot=&map.spots[n];
		if (strcmp(spot->name,"Armor")!=0) continue;
		if (spot->lookup.nearest_node_idx==-1) continue;

			// get distance

		d=map_node_to_node_distance(&map,from_node_idx,spot->lookup.nearest_node_idx==-1);
		if (d==-1) continue;

		if ((dist==-1) || (d<dist)) {
			dist=d;
			idx=spot->lookup.nearest_node_idx;
		}
	}
	
	return(idx);
}

