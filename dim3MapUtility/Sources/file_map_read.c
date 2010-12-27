/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Map File Reading

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

(c) 2000-2010 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3maputility.h"
#endif

extern maputility_settings_type		maputility_settings;

char					media_type_str[][32]={"none","chooser","title","movie",""},
						camera_mode_str[][32]={"fpp","chase","static","chase_static",""},
						sky_type_str[][32]={"dome_panoramic","dome_hemisphere","cube",""},
						gl_fog_type_str[][32]={"linear","exp","exp2",""},
						mesh_hide_mode_str[][32]={"never","single_player","multiplayer",""},
						mesh_normal_mode_str[][32]={"auto","in","out","in_out","edge","lock",""},
						liquid_tide_direction_str[][32]={"horizontal","vertical",""},
                        light_type_str[][32]={"normal","blink","glow","pulse","flicker","failing",""},
                        light_direction_str[][32]={"all","neg_x","pos_x","neg_y","pos_y","neg_z","pos_z",""},
						spot_type_str[][32]={"Object","Bot","Player","Spawn",""},
						skill_type_str[][32]={"easy","medium","hard",""},
						spawn_type_str[][32]={"always","single_player","multiplayer",""},
						cinema_action_type_str[][32]={"none","place","move","stop","show","hide",""},
						cinema_actor_type_str[][32]={"camera","player","object","movement","particle","hud_text","hud_bitmap",""};

extern bool decode_map_v1_xml(map_type *map,int map_head);
extern bool decode_map_v2_xml(map_type *map,int map_head);
extern bool decode_map_v3_xml(map_type *map,int map_head);

/* =======================================================

      Read Map Settings
      
======================================================= */

void decode_map_settings_xml(map_type *map,int map_head)
{
	int						main_ambient_light_tag,main_ambient_sound_tag,main_rain_tag,
							main_background_tag,main_sky_tag,main_fog_tag,
							tag;
	
        // map info
    
    tag=xml_findfirstchild("Info",map_head);
    if (tag!=-1) {
        xml_get_attribute_text(tag,"name",map->info.title,name_str_len);
        xml_get_attribute_text(tag,"author",map->info.author,name_str_len);
    }
    
    tag=xml_findfirstchild("Settings",map_head);
    if (tag!=-1) {
		map->settings.gravity=xml_get_attribute_float_default(tag,"gravity",1);
        map->settings.gravity_max_power=xml_get_attribute_float_default(tag,"gravity_max_power",32);
		map->settings.gravity_max_speed=xml_get_attribute_float_default(tag,"gravity_max_speed",400);
		map->settings.resistance=xml_get_attribute_float_default(tag,"resistance",1);
		map->settings.never_cull=xml_get_attribute_boolean(tag,"never_cull");
		map->settings.no_shaders=xml_get_attribute_boolean(tag,"no_shaders");
		
        xml_get_attribute_text(tag,"network_game_list",map->settings.network_game_list,256);
		xml_get_attribute_text(tag,"params",map->settings.params,param_str_len);

        map->light_map.quality=xml_get_attribute_int_default(tag,"light_map_quality",50);
        map->light_map.size=xml_get_attribute_int_default(tag,"light_map_size",1024);
        map->light_map.pixel_border_count=xml_get_attribute_int_default(tag,"light_map_pixel_border_count",6);
        map->light_map.blur_count=xml_get_attribute_int_default(tag,"light_map_blur_count",3);
	}
	
    tag=xml_findfirstchild("Editor",map_head);
    if (tag!=-1) {
		map->editor_setup.txt_scale_x=xml_get_attribute_float_default(tag,"txt_scale_x",0.04f);
		map->editor_setup.txt_scale_y=xml_get_attribute_float_default(tag,"txt_scale_y",0.04f);
		map->editor_setup.view_near_dist=xml_get_attribute_int_default(tag,"view_near_dist",(6*map_enlarge));
		map->editor_setup.view_far_dist=xml_get_attribute_int_default(tag,"view_far_dist",(2000*map_enlarge));
		map->editor_setup.link_always_start=xml_get_attribute_boolean(tag,"link_always_start");
	}
	
    tag=xml_findfirstchild("Media",map_head);
    if (tag!=-1) {
		map->media.type=xml_get_attribute_list(tag,"type",(char*)media_type_str);
		xml_get_attribute_text(tag,"name",map->media.name,name_str_len);
		xml_get_attribute_text(tag,"title_sound_name",map->media.title_sound_name,name_str_len);
	}
	
    tag=xml_findfirstchild("Music",map_head);
    if (tag!=-1) {
		map->music.fade_msec=xml_get_attribute_int(tag,"fade_msec");
		xml_get_attribute_text(tag,"name",map->music.name,name_str_len);
	}
	
	main_ambient_light_tag=xml_findfirstchild("Ambient_Light",map_head);
    if (main_ambient_light_tag!=-1) {
		xml_get_attribute_color(main_ambient_light_tag,"rgb",&map->ambient.light_color);
		map->ambient.light_map_boost=xml_get_attribute_float_default(main_ambient_light_tag,"light_map_boost",0.0f);
	}
	
	main_ambient_sound_tag=xml_findfirstchild("Ambient_Sound",map_head);
    if (main_ambient_sound_tag!=-1) {
		xml_get_attribute_text(main_ambient_sound_tag,"name",map->ambient.sound_name,name_str_len);
		map->ambient.sound_pitch=xml_get_attribute_float_default(main_ambient_sound_tag,"pitch",1.0f);
	}
	
	main_background_tag=xml_findfirstchild("Background",map_head);
    if (main_background_tag!=-1) {
		map->background.on=xml_get_attribute_boolean(main_background_tag,"on");
		map->background.front.fill=xml_get_attribute_int(main_background_tag,"front_fill");
		xml_get_attribute_2_coord_float(main_background_tag,"front_scroll",&map->background.front.x_scroll_fact,&map->background.front.y_scroll_fact);
		map->background.middle.fill=xml_get_attribute_int(main_background_tag,"middle_fill");
		xml_get_attribute_2_coord_float(main_background_tag,"middle_scroll",&map->background.middle.x_scroll_fact,&map->background.middle.y_scroll_fact);
		map->background.back.fill=xml_get_attribute_int(main_background_tag,"back_fill");
		xml_get_attribute_2_coord_float(main_background_tag,"back_scroll",&map->background.back.x_scroll_fact,&map->background.back.y_scroll_fact);
    }
  
	main_sky_tag=xml_findfirstchild("Sky",map_head);
    if (main_sky_tag!=-1) {
		tag=xml_findfirstchild("Settings",main_sky_tag);
		if (tag!=-1) {
			map->sky.on=xml_get_attribute_boolean(tag,"on");
			map->sky.type=xml_get_attribute_list(tag,"type",(char*)sky_type_str);
			map->sky.radius=xml_get_attribute_int(tag,"radius");
			map->sky.dome_y=xml_get_attribute_int(tag,"dome_y");
			map->sky.dome_mirror=xml_get_attribute_boolean(tag,"dome_mirror");
		}
		tag=xml_findfirstchild("Fill",main_sky_tag);
		if (tag!=-1) {
			map->sky.fill=xml_get_attribute_int_default(tag,"index",-1);
			map->sky.bottom_fill=xml_get_attribute_int_default(tag,"bottom_index",-1);
			map->sky.north_fill=xml_get_attribute_int_default(tag,"north_index",-1);
			map->sky.south_fill=xml_get_attribute_int_default(tag,"south_index",-1);
			map->sky.east_fill=xml_get_attribute_int_default(tag,"east_index",-1);
			map->sky.west_fill=xml_get_attribute_int_default(tag,"west_index",-1);
			map->sky.txt_fact=xml_get_attribute_float_default(tag,"factor",1);
			xml_get_attribute_2_coord_float(tag,"shift",&map->sky.txt_x_shift,&map->sky.txt_y_shift);
		}
    }

	main_fog_tag=xml_findfirstchild("Fog",map_head);
    if (main_fog_tag!=-1) {
		map->fog.on=xml_get_attribute_boolean(main_fog_tag,"on");
		map->fog.count=xml_get_attribute_int(main_fog_tag,"count");
		map->fog.inner_radius=xml_get_attribute_int(main_fog_tag,"inner_radius");
		map->fog.outer_radius=xml_get_attribute_int(main_fog_tag,"outer_radius");
		map->fog.high=xml_get_attribute_int(main_fog_tag,"high");
		map->fog.drop=xml_get_attribute_int(main_fog_tag,"drop");
		map->fog.texture_idx=xml_get_attribute_int_default(main_fog_tag,"texture_index",-1);
		map->fog.speed=xml_get_attribute_float(main_fog_tag,"speed");
		map->fog.txt_x_fact=xml_get_attribute_float(main_fog_tag,"txt_x_fact");
		map->fog.txt_y_fact=xml_get_attribute_float(main_fog_tag,"txt_y_fact");
		xml_get_attribute_color(main_fog_tag,"rgb",&map->fog.col);
		map->fog.alpha=xml_get_attribute_float(main_fog_tag,"alpha");
		map->fog.use_solid_color=xml_get_attribute_boolean(main_fog_tag,"use_solid_color");
    }
	
	main_rain_tag=xml_findfirstchild("Rain",map_head);
    if (main_rain_tag!=-1) {
		map->rain.on=xml_get_attribute_boolean(main_rain_tag,"on");
		map->rain.density=xml_get_attribute_int(main_rain_tag,"density");
		map->rain.radius=xml_get_attribute_int(main_rain_tag,"radius");
		map->rain.height=xml_get_attribute_int(main_rain_tag,"height");
		map->rain.speed=xml_get_attribute_int(main_rain_tag,"speed");
		map->rain.line_width=xml_get_attribute_int(main_rain_tag,"line_width");
		map->rain.line_length=xml_get_attribute_int(main_rain_tag,"line_length");
		map->rain.slant_add=xml_get_attribute_int(main_rain_tag,"slant_add");
		map->rain.slant_time_msec=xml_get_attribute_int(main_rain_tag,"slant_time_msec");
		map->rain.slant_change_msec=xml_get_attribute_int(main_rain_tag,"slant_change_msec");
		map->rain.alpha=xml_get_attribute_float(main_rain_tag,"alpha");
		xml_get_attribute_color(main_rain_tag,"start_rgb",&map->rain.start_color);
		xml_get_attribute_color(main_rain_tag,"end_rgb",&map->rain.end_color);
	}
}

/* =======================================================

      Read Groups
      
======================================================= */

void decode_map_groups_xml(map_type *map,int map_head)
{
	int				n,ngroup,idx,
					main_group_tag,group_tag;

    main_group_tag=xml_findfirstchild("Groups",map_head);
    if (main_group_tag==-1) return;
    
	map->group.ngroup=0;

    ngroup=xml_countchildren(main_group_tag);
	group_tag=xml_findfirstchild("Group",main_group_tag);

    for (n=0;n!=ngroup;n++) {
		idx=map_group_add(map);
		if (idx==-1) break;

 		xml_get_attribute_text(group_tag,"name",map->group.groups[idx].name,name_str_len);
		group_tag=xml_findnextchild(group_tag);
	}
}

/* =======================================================

      Read Movements
      
======================================================= */

void decode_map_movements_xml(map_type *map,int map_head)
{
	int						n,k,nmovement,nmove,movement_idx,move_idx,
							main_movement_tag,movement_tag,main_movement_move_tag,movement_move_tag;
	movement_type			*movement;
	movement_move_type		*move;
    
    main_movement_tag=xml_findfirstchild("Movements",map_head);
    if (main_movement_tag==-1) return;

	map->movement.nmovement=0;
    
    nmovement=xml_countchildren(main_movement_tag);
	movement_tag=xml_findfirstchild("Movement",main_movement_tag);

    for (n=0;n!=nmovement;n++) {
		movement_idx=map_movement_add(map);
		if (movement_idx==-1) break;

		movement=&map->movement.movements[movement_idx];
	
 		xml_get_attribute_text(movement_tag,"name",movement->name,name_str_len);
		movement->group_idx=xml_get_attribute_int(movement_tag,"group");
		movement->reverse_group_idx=xml_get_attribute_int_default(movement_tag,"group_reverse",-1);
 		movement->auto_start=xml_get_attribute_boolean(movement_tag,"auto_start");
 		movement->auto_open=xml_get_attribute_boolean(movement_tag,"auto_open");
 		movement->auto_open_stand=xml_get_attribute_boolean(movement_tag,"auto_open_stand");
 		movement->auto_open_never_close=xml_get_attribute_boolean(movement_tag,"auto_open_never_close");
 		movement->auto_open_distance=xml_get_attribute_int(movement_tag,"auto_open_distance");
		movement->loop=xml_get_attribute_boolean(movement_tag,"loop");
		
		main_movement_move_tag=xml_findfirstchild("Moves",movement_tag);
		if (main_movement_move_tag!=-1) {
			
			movement->nmove=0;

			nmove=xml_countchildren(main_movement_move_tag);
			movement_move_tag=xml_findfirstchild("Move",main_movement_move_tag);
		
			for (k=0;k!=nmove;k++) {
				move_idx=map_movement_move_add(map,movement_idx);
				if (move_idx==-1) break;

				move=&movement->moves[move_idx];

				xml_get_attribute_3_coord_int(movement_move_tag,"c3",&move->mov.x,&move->mov.y,&move->mov.z);
				xml_get_attribute_3_coord_float(movement_move_tag,"r3",&move->rot.x,&move->rot.y,&move->rot.z);
				move->msec=(short)xml_get_attribute_int(movement_move_tag,"msec");
				move->user_id=(short)xml_get_attribute_int(movement_move_tag,"user_id");
				xml_get_attribute_text(movement_move_tag,"sound_name",move->sound_name,name_str_len);
				move->sound_pitch=xml_get_attribute_float(movement_move_tag,"sound_pitch");
				
				movement_move_tag=xml_findnextchild(movement_move_tag);
			}
		}
		
		movement_tag=xml_findnextchild(movement_tag);
	}
}

/* =======================================================

      Read Textures
      
======================================================= */

void decode_map_textures_xml(map_type *map,int map_head)
{
	int						i,main_fill_tag,fill_tag,
							id,cnt;
 
    main_fill_tag=xml_findfirstchild("Fills",map_head);
    if (main_fill_tag==-1) return;
    
    cnt=xml_countchildren(main_fill_tag);
	fill_tag=xml_findfirstchild("Fill",main_fill_tag);

    for (i=0;i!=cnt;i++) {
		id=xml_get_attribute_int(fill_tag,"id");
		bitmap_texture_read_xml(&map->textures[id],fill_tag,TRUE);
		fill_tag=xml_findnextchild(fill_tag);
    }
}

/* =======================================================

      Read Cinemas
      
======================================================= */

void decode_map_cinemas_xml(map_type *map,int map_head)
{
	int						n,k,cinema_idx,action_idx,ncinema,naction,
							cinemas_tag,cinema_tag,actions_tag,action_tag;
	map_cinema_type			*cinema;
	map_cinema_action_type	*action;
    
    cinemas_tag=xml_findfirstchild("Cinemas",map_head);
    if (cinemas_tag==-1) return;
    
    map->cinema.ncinema=0;
	
	ncinema=xml_countchildren(cinemas_tag);
	cinema_tag=xml_findfirstchild("Cinema",cinemas_tag);

    for (n=0;n!=ncinema;n++) {
		cinema_idx=map_cinema_add(map);
		if (cinema_idx==-1) break;

		cinema=&map->cinema.cinemas[cinema_idx];

		xml_get_attribute_text(cinema_tag,"name",cinema->name,name_str_len);
		cinema->len_msec=xml_get_attribute_int(cinema_tag,"length");
		cinema->freeze_input=xml_get_attribute_boolean(cinema_tag,"freeze_input");
		cinema->show_hud=xml_get_attribute_boolean(cinema_tag,"show_hud");
		
		cinema->naction=0;
		
		actions_tag=xml_findfirstchild("Actions",cinema_tag);
		if (actions_tag==-1) continue;

		naction=xml_countchildren(actions_tag);
		action_tag=xml_findfirstchild("Action",actions_tag);
		
		for (k=0;k!=naction;k++) {
			action_idx=map_cinema_add_action(map,cinema_idx);
			if (action_idx==-1) break;

			action=&cinema->actions[action_idx];

			action->action=xml_get_attribute_list(action_tag,"action",(char*)cinema_action_type_str);

			action->actor_type=xml_get_attribute_list(action_tag,"actor_type",(char*)cinema_actor_type_str);
			xml_get_attribute_text(action_tag,"actor_name",action->actor_name,name_str_len);

			action->start_msec=xml_get_attribute_int_default(action_tag,"start",0);
			action->end_msec=xml_get_attribute_int_default(action_tag,"end",0);

			action->move_reverse=xml_get_attribute_boolean(action_tag,"move_reverse");

			xml_get_attribute_text(action_tag,"animation",action->animation_name,name_str_len);
			xml_get_attribute_text(action_tag,"next_animation",action->next_animation_name,name_str_len);
			xml_get_attribute_text(action_tag,"node",action->node_name,name_str_len);
			
			xml_get_attribute_text(action_tag,"text",action->text_str,256);
			
			action_tag=xml_findnextchild(action_tag);
		}
		
		cinema_tag=xml_findnextchild(cinema_tag);
	}
}

/* =======================================================

      Read Cameras
      
======================================================= */

void decode_map_camera_xml(map_type *map,int map_head)
{
	int				camera_tag,tag;
	
    camera_tag=xml_findfirstchild("Camera",map_head);
    if (camera_tag==-1) return;
	
	map->camera.mode=xml_get_attribute_list(camera_tag,"mode",(char*)camera_mode_str);
	xml_get_attribute_3_coord_float(camera_tag,"ang",&map->camera.ang.x,&map->camera.ang.y,&map->camera.ang.z);
	
    tag=xml_findfirstchild("Plane",camera_tag);
    if (tag!=-1) {
		map->camera.plane.fov=xml_get_attribute_float(tag,"fov");
		map->camera.plane.aspect_ratio=xml_get_attribute_float(tag,"aspectRatio");
		map->camera.plane.near_z=xml_get_attribute_int(tag,"near");
		map->camera.plane.far_z=xml_get_attribute_int(tag,"far");
		map->camera.plane.near_z_offset=xml_get_attribute_int(tag,"nearOffset");
	}
	
    tag=xml_findfirstchild("Chase",camera_tag);
    if (tag!=-1) {
		map->camera.chase.distance=xml_get_attribute_int(tag,"distance");
		map->camera.chase.track_speed=xml_get_attribute_float(tag,"track_speed");
		xml_get_attribute_3_coord_float(tag,"slop",&map->camera.chase.slop.x,&map->camera.chase.slop.y,&map->camera.chase.slop.z);
	}
	
    tag=xml_findfirstchild("Static",camera_tag);
    if (tag!=-1) {
		map->camera.c_static.follow=xml_get_attribute_boolean(tag,"follow");
		xml_get_attribute_text(tag,"attach_node",map->camera.c_static.attach_node,name_str_len);
	}
}

/* =======================================================

      Read Editor Views
      
======================================================= */

void read_map_editor_views_xml(map_type *map,int map_head)
{
	int						n,main_views_tag,view_tag;
	editor_view_type		*view;

	map->editor_views.count=0;

    main_views_tag=xml_findfirstchild("Editor_Views",map_head);
    if (main_views_tag==-1) return;

    map->editor_views.count=xml_countchildren(main_views_tag);
	view_tag=xml_findfirstchild("Editor_View",main_views_tag);

    view=map->editor_views.views;
    
    for (n=0;n!=map->editor_views.count;n++) {
    
		xml_get_attribute_2_coord_float(view_tag,"box_top_left",&view->box.top,&view->box.lft);
		xml_get_attribute_2_coord_float(view_tag,"box_bottom_right",&view->box.bot,&view->box.rgt);
		xml_get_attribute_3_coord_int(view_tag,"pnt",&view->pnt.x,&view->pnt.y,&view->pnt.z);
		xml_get_attribute_3_coord_float(view_tag,"ang",&view->ang.x,&view->ang.y,&view->ang.z);
		view->uv_layer=xml_get_attribute_int(view_tag,"uv_layer");
  		view->ortho=xml_get_attribute_boolean(view_tag,"ortho");
    	view->cull=xml_get_attribute_boolean(view_tag,"cull");
     	view->clip=xml_get_attribute_boolean(view_tag,"clip");
     	view->no_rot=xml_get_attribute_boolean(view_tag,"no_rot");
    
		view_tag=xml_findnextchild(view_tag);
        view++;
    }
}

/* =======================================================

      Check Map Network Game Types
      
======================================================= */
    
bool map_check_game_type(char *game_type,char *map_name,char *info_name)
{
	int				map_head,tag,idx;
	char			*c,*c2,game_token[256],
					game_list[256],path[1024];

	file_paths_data(&maputility_settings.file_path_setup,path,"Maps",map_name,"xml");

		// open XML file

	if (!xml_open_file(path)) return(FALSE);

    map_head=xml_findrootchild("Map");
    if (map_head==-1) {
		xml_close_file();
		return(FALSE);
    }
	
		// get infor name
		
	info_name[0]=0x0;
	
    tag=xml_findfirstchild("Info",map_head);
    if (tag!=-1) {
        xml_get_attribute_text(tag,"name",info_name,name_str_len);
    }
	
	if (info_name[0]==0x0) strcpy(info_name,map_name);

		// get game list
		
	game_list[0]=0x0;

    tag=xml_findfirstchild("Settings",map_head);
    if (tag!=-1) {
        xml_get_attribute_text(tag,"network_game_list",game_list,256);
	}

	xml_close_file();

	if (game_list[0]==0x0) return(FALSE);

		// search game list

	c=game_list;

	while (TRUE) {

		if (*c==0x0) break;

			// check next token

		strcpy(game_token,c);
		c2=strchr(game_token,',');
		if (c2!=NULL) *c2=0x0;

			// remove white space

		if (game_token[0]!=0x0) {
			while (game_token[0]==' ') {
				strcpy((char*)&game_token[0],(char*)&game_token[1]);
			}
			while (TRUE) {
				idx=strlen(game_token);
				if (idx==0) break;
				if (game_token[idx-1]!=' ') break;
				
				game_token[idx-1]=0x0;
			}
		}

			// compare

		if (strcasecmp(game_type,game_token)==0) return(TRUE);

			// get next token

		c2=strchr(c,',');
		if (c2==NULL) break;

		c=c2+1;
	}

	return(FALSE);
}

/* =======================================================

      Read Map XML
      
======================================================= */

bool read_map_xml(map_type *map)
{
	int			version,map_head,tag;
	bool		map_ok;

	if (!xml_open_file(map->info.load_path)) return(FALSE);
    
    map_head=xml_findrootchild("Map");
    if (map_head==-1) {
		xml_close_file();
		return(FALSE);
    }
	
		// get the version
		// if in engine, only allow loading of the current version
	
	tag=xml_findfirstchild("Creator",map_head);
	version=xml_get_attribute_int_default(tag,"version",1);
	
	if (maputility_settings.in_engine) {
		if (version!=map_current_version) return(FALSE);
	}

		// decode general non-versioned part

	decode_map_settings_xml(map,map_head);
	decode_map_groups_xml(map,map_head);
	decode_map_textures_xml(map,map_head);
	decode_map_movements_xml(map,map_head);
	decode_map_cinemas_xml(map,map_head);
	decode_map_camera_xml(map,map_head);

		// editor setup

	read_map_editor_views_xml(map,map_head);
	
		// decode

	map_ok=FALSE;
		
	switch (version) {
		case 1:
			map_ok=decode_map_v1_xml(map,map_head);
			break;
		case 2:
			map_ok=decode_map_v2_xml(map,map_head);
			break;
		default:
			map_ok=decode_map_v3_xml(map,map_head);
			break;
	}
	
	xml_close_file(); 
	  
    return(map_ok);
}


