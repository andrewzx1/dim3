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
						cinema_action_type_str[][32]={"none","place","move","stop","show","hide","fade in","fade out",""},
						cinema_actor_type_str[][32]={"camera","player","object","movement","particle","hud_text","hud_bitmap",""};

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
		map->optimize.never_cull=xml_get_attribute_boolean(tag,"never_cull");
		map->optimize.cull_angle=xml_get_attribute_float_default(tag,"cull_angle",0.0f);
		map->optimize.no_shaders=xml_get_attribute_boolean(tag,"no_shaders");
		
        xml_get_attribute_text(tag,"network_game_list",map->settings.network_game_list,256);
		xml_get_attribute_text(tag,"params",map->settings.params,param_str_len);

        map->light_map.quality=xml_get_attribute_int_default(tag,"light_map_quality",2);
        map->light_map.size=xml_get_attribute_int_default(tag,"light_map_size",1024);
        map->light_map.pixel_border_count=xml_get_attribute_int_default(tag,"light_map_pixel_border_count",6);
        map->light_map.blur_count=xml_get_attribute_int_default(tag,"light_map_blur_count",3);
		map->light_map.use_normals=xml_get_attribute_boolean(tag,"light_map_use_normals");
        map->light_map.diffuse_boost=xml_get_attribute_float_default(tag,"light_map_diffuse_boost",0.0f);

		if (map->light_map.quality>4) map->light_map.quality=2;		// reset from older map formats
	}
	
    tag=xml_findfirstchild("Editor",map_head);
    if (tag!=-1) {
		map->editor_setup.txt_scale_x=xml_get_attribute_float_default(tag,"txt_scale_x",0.04f);
		map->editor_setup.txt_scale_y=xml_get_attribute_float_default(tag,"txt_scale_y",0.04f);
		map->editor_setup.view_near_dist=xml_get_attribute_int_default(tag,"view_near_dist",400);
		map->editor_setup.view_far_dist=xml_get_attribute_int_default(tag,"view_far_dist",300000);
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
		xml_get_attribute_2_coord_float_default(main_background_tag,"front_stamp",&map->background.front.x_fact,&map->background.front.y_fact,1.0f,1.0f);
		xml_get_attribute_2_coord_float(main_background_tag,"front_scroll",&map->background.front.x_scroll_fact,&map->background.front.y_scroll_fact);
		map->background.middle.fill=xml_get_attribute_int(main_background_tag,"middle_fill");
		xml_get_attribute_2_coord_float_default(main_background_tag,"middle_stamp",&map->background.middle.x_fact,&map->background.middle.y_fact,1.0f,1.0f);
		xml_get_attribute_2_coord_float(main_background_tag,"middle_scroll",&map->background.middle.x_scroll_fact,&map->background.middle.y_scroll_fact);
		map->background.back.fill=xml_get_attribute_int(main_background_tag,"back_fill");
		xml_get_attribute_2_coord_float_default(main_background_tag,"back_stamp",&map->background.back.x_fact,&map->background.back.y_fact,1.0f,1.0f);
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
		cinema->no_cancel=xml_get_attribute_boolean(cinema_tag,"no_cancel");
		
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

      Read Meshes and Liquids
      
======================================================= */

bool read_single_mesh_v3(map_type *map,int mesh_idx,int mesh_tag)
{
	int					n,nvertex,npoly,old_mesh_lmap_txt_idx,
						msg_tag,main_vertex_tag,vertex_tag,main_poly_tag,poly_tag,tag;
	float				import_factor;
	bool				mesh_climbable;
	d3pnt				*pt;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

	mesh=&map->mesh.meshes[mesh_idx];
	
		// older versions had light maps on a per mesh level
		// check to see if that exists and if it does, override the
		// polygons
		
	old_mesh_lmap_txt_idx=xml_get_attribute_int_default(mesh_tag,"lmap_txt_idx",-1);

		// mesh settings

	mesh->group_idx=xml_get_attribute_int_default(mesh_tag,"group",-1);

	mesh->flag.on=!xml_get_attribute_boolean(mesh_tag,"off");
	mesh->flag.pass_through=xml_get_attribute_boolean(mesh_tag,"pass");
	mesh->flag.moveable=xml_get_attribute_boolean(mesh_tag,"moveable");
	mesh->flag.hilite=xml_get_attribute_boolean(mesh_tag,"hilite");
	mesh->flag.simple_collision=xml_get_attribute_boolean(mesh_tag,"simple_collision");
	mesh->flag.lock_uv=xml_get_attribute_boolean(mesh_tag,"lock_uv");
	mesh->flag.lock_move=xml_get_attribute_boolean(mesh_tag,"lock_move");
	mesh->flag.never_obscure=xml_get_attribute_boolean(mesh_tag,"never_obscure");
	mesh->flag.rot_independent=xml_get_attribute_boolean(mesh_tag,"rot_independent");
	mesh->flag.shadow=xml_get_attribute_boolean(mesh_tag,"shadow");
	mesh->flag.no_light_map=xml_get_attribute_boolean(mesh_tag,"no_light_map");
	mesh->flag.skip_light_map_trace=xml_get_attribute_boolean(mesh_tag,"skip_light_map_trace");
	
	mesh->hide_mode=xml_get_attribute_list(mesh_tag,"hide",(char*)mesh_hide_mode_str);
	mesh->normal_mode=xml_get_attribute_list(mesh_tag,"normal",(char*)mesh_normal_mode_str);
	
	mesh->harm=xml_get_attribute_int_default(mesh_tag,"harm",0);

	xml_get_attribute_3_coord_int(mesh_tag,"rot_off",&mesh->rot_off.x,&mesh->rot_off.y,&mesh->rot_off.z);
	
		// old version of import factor
		
	import_factor=xml_get_attribute_float_default(mesh_tag,"import_factor",0.0f);
	
		// old version of climbable at mesh level
		// push it to polys if it's still there
		
	mesh_climbable=xml_get_attribute_boolean(mesh_tag,"climbable");

		// messages

    mesh->msg.entry_on=mesh->msg.exit_on=mesh->msg.base_on=mesh->msg.map_change_on=FALSE;
    mesh->msg.entry_id=mesh->msg.exit_id=0;
	mesh->msg.map_name[0]=0x0;
	strcpy(mesh->msg.map_spot_name,"Start");
    
    msg_tag=xml_findfirstchild("Messages",mesh_tag);
    if (msg_tag!=-1) {
        tag=xml_findfirstchild("Entry",msg_tag);
        if (tag!=-1) {
            mesh->msg.entry_on=xml_get_attribute_boolean(tag,"on");
            mesh->msg.entry_id=xml_get_attribute_int(tag,"id");
        }
        tag=xml_findfirstchild("Exit",msg_tag);
        if (tag!=-1) {
            mesh->msg.exit_on=xml_get_attribute_boolean(tag,"on");
            mesh->msg.exit_id=xml_get_attribute_int(tag,"id");
        }
        tag=xml_findfirstchild("Base",msg_tag);
        if (tag!=-1) {
            mesh->msg.base_on=xml_get_attribute_boolean(tag,"on");
            mesh->msg.base_team=xml_get_attribute_int(tag,"team");
        }
        tag=xml_findfirstchild("Map",msg_tag);
        if (tag!=-1) {
            mesh->msg.map_change_on=xml_get_attribute_boolean(tag,"on");
			xml_get_attribute_text(tag,"name",mesh->msg.map_name,name_str_len);
			xml_get_attribute_text(tag,"spot_name",mesh->msg.map_spot_name,name_str_len);
        }
	}
	
		// import
		
	tag=xml_findfirstchild("Import",mesh_tag);
	if (tag!=-1) {
		xml_get_attribute_text(tag,"obj",mesh->import.obj_name,name_str_len);
		xml_get_attribute_text(tag,"group",mesh->import.group_name,name_str_len);
	}
	else {
		mesh->import.obj_name[0]=0x0;
		mesh->import.group_name[0]=0x0;
	}

		// vertexes

    main_vertex_tag=xml_findfirstchild("Vertexes",mesh_tag);
    if (main_vertex_tag!=-1) {
    
        nvertex=xml_countchildren(main_vertex_tag);
		vertex_tag=xml_findfirstchild("v",main_vertex_tag);

		if (!map_mesh_set_vertex_count(map,mesh_idx,nvertex)) return(FALSE);

		pt=mesh->vertexes;

		for (n=0;n!=nvertex;n++) {
			xml_get_attribute_3_coord_int(vertex_tag,"c3",&pt->x,&pt->y,&pt->z);
			pt++;
			vertex_tag=xml_findnextchild(vertex_tag);
		}
	}

		// polys

    main_poly_tag=xml_findfirstchild("Polys",mesh_tag);
    if (main_poly_tag!=-1) {
    
        npoly=xml_countchildren(main_poly_tag);
		poly_tag=xml_findfirstchild("p",main_poly_tag);

		if (!map_mesh_set_poly_count(map,mesh_idx,npoly)) return(FALSE);

		poly=mesh->polys;

		for (n=0;n!=npoly;n++) {
			poly->ptsz=xml_get_attribute_int_array(poly_tag,"v",poly->v,8);
			
			poly->txt_idx=xml_get_attribute_int(poly_tag,"txt");
			poly->lmap_txt_idx=xml_get_attribute_int_default(poly_tag,"lmap_txt_idx",old_mesh_lmap_txt_idx);

			xml_get_attribute_3_coord_float(poly_tag,"t3",&poly->tangent_space.tangent.x,&poly->tangent_space.tangent.y,&poly->tangent_space.tangent.z);
			xml_get_attribute_3_coord_float(poly_tag,"n3",&poly->tangent_space.normal.x,&poly->tangent_space.normal.y,&poly->tangent_space.normal.z);

			xml_get_attribute_float_array(poly_tag,"x",poly->main_uv.x,8);
			xml_get_attribute_float_array(poly_tag,"y",poly->main_uv.y,8);
			xml_get_attribute_2_coord_float(poly_tag,"shift",&poly->x_shift,&poly->y_shift);

			if (poly->lmap_txt_idx!=-1) {
				xml_get_attribute_float_array(poly_tag,"x_1",poly->lmap_uv.x,8);
				xml_get_attribute_float_array(poly_tag,"y_1",poly->lmap_uv.y,8);
			}

			poly->climbable=xml_get_attribute_boolean(poly_tag,"climbable") || (mesh_climbable);
			poly->never_cull=xml_get_attribute_boolean(poly_tag,"never_cull");

			xml_get_attribute_text_default_blank(poly_tag,"camera",poly->camera,name_str_len);

			poly++;
			poly_tag=xml_findnextchild(poly_tag);
		}
	}

	return(TRUE);
}

void read_single_liquid_v3(map_type *map,int liquid_idx,int liquid_tag)
{
	int					tag;
	map_liquid_type		*liq;

	liq=&map->liquid.liquids[liquid_idx];

		// liquid settings

	liq->group_idx=xml_get_attribute_int_default(liquid_tag,"group",-1);
	liq->never_obscure=xml_get_attribute_boolean(liquid_tag,"never_obscure");
	liq->never_cull=xml_get_attribute_boolean(liquid_tag,"never_cull");
	liq->no_draw=xml_get_attribute_boolean(liquid_tag,"no_draw");

		// polygon

    tag=xml_findfirstchild("Poly",liquid_tag);
    if (tag!=-1) {
		liq->txt_idx=xml_get_attribute_int(tag,"txt");
		liq->lmap_txt_idx=xml_get_attribute_int_default(tag,"lmap_txt_idx",-1);
		xml_get_attribute_3_coord_int(tag,"v1",&liq->lft,&liq->y,&liq->top);
		xml_get_attribute_3_coord_int(tag,"v2",&liq->rgt,&liq->y,&liq->bot);
		liq->depth=xml_get_attribute_int(tag,"depth");

		xml_get_attribute_2_coord_float(tag,"uv_off",&liq->main_uv.x_offset,&liq->main_uv.y_offset);
		xml_get_attribute_2_coord_float(tag,"uv_size",&liq->main_uv.x_size,&liq->main_uv.y_size);
		if (liq->lmap_txt_idx!=-1) {
			xml_get_attribute_2_coord_float(tag,"uv_1_off",&liq->lmap_uv.x_offset,&liq->lmap_uv.y_offset);
			xml_get_attribute_2_coord_float(tag,"uv_1_size",&liq->lmap_uv.x_size,&liq->lmap_uv.y_size);
		}

		xml_get_attribute_color(tag,"rgb",&liq->col);
		liq->tint_alpha=xml_get_attribute_float(tag,"tint_alpha");
		xml_get_attribute_2_coord_float(tag,"shift",&liq->x_shift,&liq->y_shift);
		
		xml_get_attribute_text_default_blank(tag,"camera",liq->camera,name_str_len);
		xml_get_attribute_text_default_blank(tag,"ambient_sound_name",liq->ambient.sound_name,name_str_len);
	}

		// physics

    tag=xml_findfirstchild("Physic",liquid_tag);
    if (tag!=-1) {
		liq->speed_alter=xml_get_attribute_float(tag,"speed_alter");
	}

		// harm

    tag=xml_findfirstchild("Harm",liquid_tag);
    if (tag!=-1) {
		liq->harm.in_harm=xml_get_attribute_int(tag,"harm");
		liq->harm.drown_harm=xml_get_attribute_int(tag,"drown_harm");
		liq->harm.drown_tick=xml_get_attribute_int(tag,"drown_tick");
	}

		// tides

    tag=xml_findfirstchild("Tide",liquid_tag);
    if (tag!=-1) {
		liq->tide.division=xml_get_attribute_int_default(tag,"division",liquid_min_division);
		liq->tide.rate=xml_get_attribute_int(tag,"rate");
		liq->tide.high=xml_get_attribute_int(tag,"high");
		liq->tide.direction=xml_get_attribute_list(tag,"tide_direction",(char*)liquid_tide_direction_str);
        liq->tide.flat=xml_get_attribute_boolean(tag,"flat");
	}
}

/* =======================================================

      Read Map XML
      
======================================================= */

bool decode_map_xml(map_type *map,int map_head)
{
	int						k,j,idx,nmesh,mesh_idx,nliquid,liquid_idx,
							main_mesh_tag,mesh_tag,main_liquid_tag,liquid_tag,
							main_light_tag,light_tag,main_sound_tag,sound_tag,
							main_particle_tag,particle_tag,main_node_tag,node_tag,
							main_obj_tag,obj_tag,tag,cnt;
	char					str[32];
    map_light_type			*light;
    map_sound_type			*sound;
	map_particle_type		*particle;
    node_type				*node;
    spot_type				*spot;
	map_scenery_type		*scenery;

        // meshes

	map->mesh.nmesh=0;
	map->mesh.meshes=NULL;

	main_mesh_tag=xml_findfirstchild("Meshes",map_head);
	if (main_mesh_tag!=-1) {

		nmesh=xml_countchildren(main_mesh_tag);
		if (nmesh!=0) {
			
			mesh_tag=xml_findfirstchild("Mesh",main_mesh_tag);

			for (k=0;k!=nmesh;k++) {

					// create new mesh

				mesh_idx=map_mesh_add(map);
				if (mesh_idx==-1) return(FALSE);

					// read mesh

				if (!read_single_mesh_v3(map,mesh_idx,mesh_tag)) return(FALSE);
				
				mesh_tag=xml_findnextchild(mesh_tag);
			}
		}
	}

		// liquids

	map->liquid.nliquid=0;
	map->liquid.liquids=NULL;

	main_liquid_tag=xml_findfirstchild("Liquids",map_head);
	if (main_liquid_tag!=-1) {

		nliquid=xml_countchildren(main_liquid_tag);
		if (nliquid!=0) {
			
			liquid_tag=xml_findfirstchild("Liquid",main_liquid_tag);

			for (k=0;k!=nliquid;k++) {

					// create new liquid

				liquid_idx=map_liquid_add(map);
				if (liquid_idx==-1) return(FALSE);

					// read liquid

				read_single_liquid_v3(map,liquid_idx,liquid_tag);
				
				liquid_tag=xml_findnextchild(liquid_tag);
			}
		}
	}

		// scenery

	main_obj_tag=xml_findfirstchild("Sceneries",map_head);
	if (main_obj_tag!=-1) {
	
		cnt=xml_countchildren(main_obj_tag);
		obj_tag=xml_findfirstchild("Scenery",main_obj_tag);
		
		for (k=0;k!=cnt;k++) {
			if (map->nscenery>=max_map_scenery) break;
			
			scenery=&map->sceneries[map->nscenery];
			map->nscenery++;
			
			xml_get_attribute_3_coord_int(obj_tag,"c3",&scenery->pnt.x,&scenery->pnt.y,&scenery->pnt.z);
			
			xml_get_attribute_text(obj_tag,"model_name",scenery->model_name,name_str_len);
			xml_get_attribute_text(obj_tag,"animation_name",scenery->animation_name,name_str_len);
			xml_get_attribute_3_coord_float(obj_tag,"angle",&scenery->ang.x,&scenery->ang.y,&scenery->ang.z);
			scenery->contact_object_on=xml_get_attribute_boolean(obj_tag,"contact");
			scenery->contact_projectile_on=xml_get_attribute_boolean(obj_tag,"contact_projectile");
			scenery->contact_hit_box=xml_get_attribute_boolean(obj_tag,"contact_hit_box");
			scenery->face_forward=xml_get_attribute_boolean(obj_tag,"face_forward");
			scenery->shadow=xml_get_attribute_boolean(obj_tag,"shadow");
			
			xml_get_attribute_short_array(obj_tag,"frames",scenery->texture_frame,max_map_scenery_model_texture_frame);
			
			scenery->resize=xml_get_attribute_float_default(obj_tag,"resize",1.0f);
			
			obj_tag=xml_findnextchild(obj_tag);
		}
	}

		// lights
	
	main_light_tag=xml_findfirstchild("Lights",map_head);
	if (main_light_tag!=-1) {
	
		cnt=xml_countchildren(main_light_tag);
		light_tag=xml_findfirstchild("Light",main_light_tag);
		
		for (k=0;k!=cnt;k++) {
			if (map->nlight>=max_map_light) break;
			
			light=&map->lights[map->nlight];
			map->nlight++;

			xml_get_attribute_text(light_tag,"name",light->name,name_str_len);

			light->type=xml_get_attribute_list(light_tag,"type",(char*)light_type_str);
			light->light_map=xml_get_attribute_boolean(light_tag,"light_map");

			if (xml_get_attribute_text(light_tag,"filter",str,32)) {
				if (strcasecmp(str,"not_mesh")==0) light->light_map=TRUE;		// supergumba -- temporary as we change over attributes here
			}

			light->direction=xml_get_attribute_list(light_tag,"direction",(char*)light_direction_str);
			xml_get_attribute_3_coord_int(light_tag,"c3",&light->pnt.x,&light->pnt.y,&light->pnt.z);
			light->intensity=xml_get_attribute_int(light_tag,"intensity");
			light->exponent=xml_get_attribute_float_default(light_tag,"exponent",1.0f);
			xml_get_attribute_color(light_tag,"rgb",&light->col);

			light->on=!xml_get_attribute_boolean(light_tag,"off");
			
			if (light->intensity<0) light->intensity=1;
		
			light_tag=xml_findnextchild(light_tag);
		}
	}
	
		// sounds
		
	main_sound_tag=xml_findfirstchild("Sounds",map_head);
	if (main_sound_tag!=-1) {
	
		cnt=xml_countchildren(main_sound_tag);
		sound_tag=xml_findfirstchild("Sound",main_sound_tag);
		
		for (k=0;k!=cnt;k++) {
			if (map->nsound>=max_map_sound) break;
			
			sound=&map->sounds[map->nsound];
			map->nsound++;
			
			xml_get_attribute_text(sound_tag,"name",sound->name,file_str_len);
			xml_get_attribute_3_coord_int(sound_tag,"c3",&sound->pnt.x,&sound->pnt.y,&sound->pnt.z);
			sound->pitch=xml_get_attribute_float(sound_tag,"pitch");
			sound->on=!xml_get_attribute_boolean(sound_tag,"off");
			
			sound_tag=xml_findnextchild(sound_tag);
		}
	}
	
		// particles
		
	main_particle_tag=xml_findfirstchild("Particles",map_head);
	if (main_particle_tag!=-1) {
	
		cnt=xml_countchildren(main_particle_tag);
		particle_tag=xml_findfirstchild("Particle",main_particle_tag);
		
		for (k=0;k!=cnt;k++) {
			if (map->nparticle>=max_map_particle) break;
			
			particle=&map->particles[map->nparticle];
			map->nparticle++;
			
			xml_get_attribute_text(particle_tag,"name",particle->name,file_str_len);
			xml_get_attribute_3_coord_int(particle_tag,"c3",&particle->pnt.x,&particle->pnt.y,&particle->pnt.z);
			particle->spawn_tick=xml_get_attribute_int(particle_tag,"spawn_tick");
			particle->slop_tick=xml_get_attribute_int(particle_tag,"slop_tick");
			particle->on=!xml_get_attribute_boolean(particle_tag,"off");
			
			particle_tag=xml_findnextchild(particle_tag);
		}
	}

		// nodes

	main_node_tag=xml_findfirstchild("Nodes",map_head);
	if (main_node_tag!=-1) {
	
		cnt=xml_countchildren(main_node_tag);
		node_tag=xml_findfirstchild("Node",main_node_tag);
		
		for (k=0;k!=cnt;k++) {
		
			idx=xml_get_attribute_int(node_tag,"id");
			node=&map->nodes[idx];
			if (idx>=map->nnode) map->nnode=idx+1;

			node->idx=idx;
			
			for (j=0;j!=max_node_link;j++) {
				node->link[j]=-1;
			}
			for (j=0;j!=max_node;j++) {
				node->path_hint[j]=-1;
			}
			
			xml_get_attribute_3_coord_int(node_tag,"c3",&node->pnt.x,&node->pnt.y,&node->pnt.z);

			xml_get_attribute_3_coord_float(node_tag,"angle",&node->ang.x,&node->ang.y,&node->ang.z);
			node->follow_camera=xml_get_attribute_boolean(node_tag,"follow_camera");
			node->alpha=xml_get_attribute_float_default(node_tag,"alpha",1.0f);

			xml_get_attribute_text(node_tag,"name",node->name,name_str_len);
			node->event_id=xml_get_attribute_int_default(node_tag,"event_id",0);
			
			tag=xml_findfirstchild("Link",node_tag);
			xml_get_attribute_short_array(tag,"node",node->link,max_node_link);
			
			tag=xml_findfirstchild("Hint",node_tag);
			xml_get_attribute_short_array(tag,"node",node->path_hint,max_node);

			node_tag=xml_findnextchild(node_tag);
		}
	}

		// spots

	main_obj_tag=xml_findfirstchild("Spots",map_head);
	if (main_obj_tag!=-1) {
	
		cnt=xml_countchildren(main_obj_tag);
		obj_tag=xml_findfirstchild("Spot",main_obj_tag);
		
		for (k=0;k!=cnt;k++) {
		
			idx=xml_get_attribute_int(obj_tag,"id");
			spot=&map->spots[idx];
			if (idx>=map->nspot) map->nspot=idx+1;
			
			xml_get_attribute_3_coord_int(obj_tag,"c3",&spot->pnt.x,&spot->pnt.y,&spot->pnt.z);
			
			xml_get_attribute_text(obj_tag,"name",spot->name,name_str_len);
			spot->type=xml_get_attribute_list(obj_tag,"type",(char*)spot_type_str);
			xml_get_attribute_text(obj_tag,"script",spot->script,name_str_len);
			xml_get_attribute_text(obj_tag,"display_model",spot->display_model,name_str_len);
			xml_get_attribute_text(obj_tag,"params",spot->params,param_str_len);
			spot->ang.y=xml_get_attribute_float(obj_tag,"angle");
			
			spot->skill=xml_get_attribute_list(obj_tag,"skill",(char*)skill_type_str);
			spot->spawn=xml_get_attribute_list(obj_tag,"spawn",(char*)spawn_type_str);
			
			obj_tag=xml_findnextchild(obj_tag);
		}
	}

	return(TRUE);
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
		// only allow loading of the current version
	
	tag=xml_findfirstchild("Creator",map_head);
	version=xml_get_attribute_int_default(tag,"version",1);
	if (version!=map_current_version) return(FALSE);

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

	map_ok=decode_map_xml(map,map_head);
	
	xml_close_file(); 
	  
    return(map_ok);
}


