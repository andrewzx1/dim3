/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Piece Creation

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
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "interface.h"
#include "ui_common.h"

extern file_path_setup_type		file_path_setup;
extern map_type					map;
extern editor_state_type		state;

/* =======================================================

      Check if Piece Creation OK
      
======================================================= */

bool piece_create_texture_ok(void)
{
	if (map.textures[0].frames[0].bitmap.gl_id!=-1) return(TRUE);

	os_dialog_alert("You need at least one texture","You need a default texture in the first spot to add pieces with.  Please add a texture (by double clicking the first open texture) before adding pieces to the map.");
	return(FALSE);
}

/* =======================================================

      Find Create Spot
      
======================================================= */

void piece_create_get_spot(d3pnt *pnt)
{
	d3pnt			min,max;
	
		// center in view if no selection
		
	if (select_count()==0) {
		view_get_position(pnt);
	}
	
		// find place in selection
		
	else {
		select_get_extent(&min,&max);
		pnt->x=(min.x+max.x)>>1;
		pnt->z=(min.z+max.z)>>1;
		pnt->y=((min.y+max.y)>>1)-1;
	}
}

/* =======================================================

      Create Spot
      
======================================================= */

void piece_create_spot(void)
{
	int				index;
	char			name[256];
	spot_type		*spot;
	
	if (map.nspot==max_spot) {
		os_dialog_alert("Can Not Create Spot","You've reached the maximum number of spots for this map.");
	}
			
		// create spot
		
	index=map.nspot;
	
	spot=&map.spots[index];
 	map.nspot++;
	
	piece_create_get_spot(&spot->pnt);
    
	spot->ang.y=0;
	spot->name[0]=0x0;
	spot->type=spot_type_object;
	spot->script[0]=0x0;
	spot->display_model[0]=0x0;
	spot->params[0]=0x0;
    
		// select spot
		
    state.show_object=TRUE;
	
	select_clear();
	select_add(spot_piece,index,-1);

	item_palette_reset();
	property_palette_reset();
	menu_update_view();
	
	main_wind_draw();

		// pick script and auto-create name

	name[0]=0x0;
	if (!dialog_file_open_run("Pick a Script","Scripts/Objects","js",NULL,name)) return;

	strncpy(spot->script,name,file_str_len);
	spot->script[file_str_len-1]=0x0;
	
	strcpy(spot->name,spot->script);
}

/* =======================================================

      Create Scenery
      
======================================================= */

void piece_create_scenery(void)
{
	int					n,index;
	char				file_name[name_str_len];
	map_scenery_type	*scenery;
	
	if (map.nscenery==max_map_scenery) {
		os_dialog_alert("Can Not Create Scenery","You've reached the maximum number of sceneries for this map.");
	}
	
		// scenery must pick a model
		
	if (!dialog_file_open_run("Pick a Model","Models",NULL,"Mesh.xml",file_name)) return;
	if (file_name[0]==0x0) return;

		// create scenery
		
	index=map.nscenery;
	
	scenery=&map.sceneries[index];
 	map.nscenery++;
	
	piece_create_get_spot(&scenery->pnt);
    
	scenery->ang.x=0.0f;
	scenery->ang.y=0.0f;
	scenery->ang.z=0.0f;
	
	scenery->resize=1.0f;
	
	strcpy(scenery->model_name,file_name);
	scenery->animation_name[0]=0x0;
	scenery->contact_object_on=TRUE;
	scenery->contact_projectile_on=TRUE;
	scenery->contact_hit_box=TRUE;
	scenery->face_forward=FALSE;
	scenery->shadow=FALSE;
	
	for (n=0;n!=max_map_scenery_model_texture_frame;n++) {
		scenery->texture_frame[n]=0;
	}
    
		// select spot
		
    state.show_object=TRUE;
	
	select_clear();
	select_add(scenery_piece,index,-1);
	
	view_models_reset();

	item_palette_reset();
	property_palette_reset();
	menu_update_view();
	
	main_wind_draw();
}

/* =======================================================

      Create Light
      
======================================================= */

void piece_create_light_setting(map_light_setting_type *lit_set,bool on)
{
	lit_set->type=lt_normal;
	lit_set->direction=ld_all;
	lit_set->intensity=10000;
	lit_set->exponent=1.0f;
	lit_set->on=on;
	lit_set->light_map=TRUE;
	lit_set->halo_name[0]=0x0;
	lit_set->col.r=lit_set->col.g=lit_set->col.b=1;
}

void piece_create_light(void)
{
	int					index;
	map_light_type		*lit;
	
	if (map.nlight==max_map_light) {
		os_dialog_alert("Can Not Create Light","You've reached the maximum number of lights for this map.");
        return;
    }
	
		// create light
    
	index=map.nlight;
	
	lit=&map.lights[index];
	map.nlight++;
	
	piece_create_get_spot(&lit->pnt);
	
	lit->name[0]=0x0;
	piece_create_light_setting(&lit->setting,TRUE);
	
		// select light
		
	state.show_lightsoundparticle=TRUE;
	
	select_clear();
	select_add(light_piece,index,-1);

	item_palette_reset();
	property_palette_reset();
	menu_update_view();
	
	main_wind_draw();

		// pick color

	os_pick_color(&lit->setting.col);
}

/* =======================================================

      Create Sound
      
======================================================= */

void piece_create_sound(void)
{
	int					index;
	char				name[name_str_len];
	map_sound_type		*snd;
	
	if (map.nsound==max_map_sound) {
		os_dialog_alert("Can Not Create Sound","You've reached the maximum number of sounds for this map.");
        return;
    }
	
		// must pick a sound
		
	property_palette_pick_sound(name,FALSE);
	if (name[0]==0x0) return;
	
		// create sound
	
	index=map.nsound;
	
	snd=&map.sounds[index];
	map.nsound++;
	
	piece_create_get_spot(&snd->pnt);
    
	snd->on=TRUE;
	strcpy(snd->name,name);
	snd->pitch=1.0f;
	
		// select sound
		
    state.show_lightsoundparticle=TRUE;
	
	select_clear();
	select_add(sound_piece,index,-1);

	item_palette_reset();
	property_palette_reset();
	menu_update_view();
	
	main_wind_draw();
}

/* =======================================================

      Create Particle
      
======================================================= */

void piece_create_particle(void)
{
	int					index;
	char				name[name_str_len];
	map_particle_type	*prt;
	
	if (map.nparticle==max_map_particle) {
		os_dialog_alert("Can Not Create Particle","You've reached the maximum number of particles for this map.");
        return;
    }
	
		// must pick a particle
		
	property_palette_pick_particle(name);
	if (name[0]==0x0) return;
	
		// create particle
	
	index=map.nparticle;
	
	prt=&map.particles[index];
	map.nparticle++;
	
	piece_create_get_spot(&prt->pnt);
    
	prt->on=TRUE;
	strcpy(prt->name,name);
	prt->spawn_tick=5000;
	prt->slop_tick=0;
	prt->single_spawn=FALSE;
	
	piece_create_light_setting(&prt->light_setting,FALSE);
	
		// select particle
		
    state.show_lightsoundparticle=TRUE;
	
	select_clear();
	select_add(particle_piece,index,-1);

	item_palette_reset();
	property_palette_reset();
	menu_update_view();
	
	main_wind_draw();
}

/* =======================================================

      Create Node
      
======================================================= */

void piece_create_node(void)
{
	int				n,index;
	node_type		*node;
			
	if (map.nnode==max_node) {
		os_dialog_alert("Can Not Create Node","You've reached the maximum number of nodes for a map.");
	}
	
		// create node
	
	index=map.nnode;
	
	node=&map.nodes[index];
	map.nnode++;
	
	piece_create_get_spot(&node->pnt);
	
	node->ang.x=0.0f;
	node->ang.y=0.0f;
	node->ang.z=0.0f;
	node->name[0]=0x0;
	node->event_id=0;
	
	node->follow_camera=FALSE;
	node->alpha=1.0f;

	for (n=0;n!=max_node_link;n++) {
		node->link[n]=-1;
	}

	node->watch.on=FALSE;
	node->watch.dist=0;

		// select node
		
	state.show_node=TRUE;
	
	select_clear();
	select_add(node_piece,index,-1);

	item_palette_reset();
	property_palette_reset();
	menu_update_view();
	
	main_wind_draw();
}

/* =======================================================

      Create Liquid
	        
======================================================= */

void piece_create_liquid(void)
{
	int				index,sz;
	d3pnt			pnt;
	map_liquid_type	*liq;

	if (!piece_create_texture_ok()) return;

	piece_create_get_spot(&pnt);
	
		// create the liquid
		
	index=map_liquid_add(&map);
	if (index==-1) {
		os_dialog_alert("Can Not Create Liquid","Not enough memory.");
		return;
	}
	
	liq=&map.liquid.liquids[index];
	
	sz=720;
	liq->lft=pnt.x-sz;
	liq->rgt=pnt.x+sz;
	liq->top=pnt.z-sz;
	liq->bot=pnt.z+sz;
	
	liq->y=pnt.y;
	liq->depth=5000;
	
	liq->txt_idx=0;
	liq->lmap_txt_idx=-1;
	liq->group_idx=-1;

	liq->flag.lock_uv=FALSE;
	liq->flag.never_obscure=FALSE;
	liq->flag.never_cull=FALSE;
	liq->flag.no_draw=FALSE;
	
	liq->tint_alpha=0.5f;
	liq->speed_alter=1.0f;
	
	liq->col.r=liq->col.g=liq->col.b=1.0f;
	
	liq->main_uv.x_offset=liq->main_uv.y_offset=0.0f;
	liq->main_uv.x_size=liq->main_uv.y_size=1.0f;
    liq->x_shift=liq->y_shift=0.0f;
	
	liq->tide.rate=1000;
	liq->tide.high=100;
	liq->tide.uv_shift=0.01f;
	
	liq->harm.in_harm=0;
	liq->harm.drown_tick=10000;
	liq->harm.drown_harm=10;
	
	liq->reflect.texture_size=512;
	liq->reflect.x_refract_factor=20000;
	liq->reflect.z_refract_factor=20000;
	liq->reflect.no_hit_col.r=liq->reflect.no_hit_col.g=liq->reflect.no_hit_col.b=0.5f;
	liq->reflect.alpha=0.7f;

	liq->overlay.on=TRUE;
	liq->overlay.txt_idx=-1;
	liq->overlay.stamp_size=10000;

		// select the liquid
		
	state.show_liquid=TRUE;
	select_clear();
	select_add(liquid_piece,index,-1);

	main_wind_draw();
	
	menu_update_view();
}

/* =======================================================

      Create Cinema, Groups, and Movements
	        
======================================================= */

void piece_create_cinema(void)
{
	int				cinema_idx;
	
	cinema_idx=map_cinema_add(&map);
	if (cinema_idx==-1) {
		os_dialog_alert("Create Cinema","Out of Memory -- could not create a new cinema");
		return;
	}

	item_palette_select(cinema_piece,cinema_idx);

	dialog_property_string_run(list_string_value_string,(void*)map.cinema.cinemas[cinema_idx].name,name_str_len,0,0);
	
	main_wind_draw();
}

void piece_create_group(void)
{
	int				group_idx;
	
	group_idx=map_group_add(&map);
	if (group_idx==-1) {
		os_dialog_alert("Create Group","Out of Memory -- could not create a new group");
		return;
	}

	item_palette_select(group_piece,group_idx);

	dialog_property_string_run(list_string_value_string,(void*)map.group.groups[group_idx].name,name_str_len,0,0);
	
	main_wind_draw();
}

void piece_create_movement(void)
{
	int				movement_idx;
	
	movement_idx=map_movement_add(&map);
	if (movement_idx==-1) {
		os_dialog_alert("Create Movement","Out of Memory -- could not create a new movement");
		return;
	}

	item_palette_select(movement_piece,movement_idx);

	dialog_property_string_run(list_string_value_string,(void*)map.movement.movements[movement_idx].name,name_str_len,0,0);
	
	main_wind_draw();
}

