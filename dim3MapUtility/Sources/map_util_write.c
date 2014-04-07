/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Map File Writting

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

(c) 2000-2010 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3maputility.h"
#endif

extern char					media_type_str[][32],
							camera_mode_str[][32],
							camera_input_str[][32],
							sky_type_str[][32],
							gl_fog_type_str[][32],
							mesh_hide_mode_str[][32],
							mesh_normal_mode_str[][32],
							light_type_str[][32],
							light_direction_str[][32],
							spot_type_str[][32],
							skill_type_str[][32],
							spawn_type_str[][32],
							cinema_action_type_str[][32],
							cinema_actor_type_str[][32],
							map_bump_mode_str[][32];

/* =======================================================

      Map Fix
      
======================================================= */

void write_map_fix_problems(map_type *map)
{
	int					n,k;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	map_liquid_type		*liq;
	
		// delete any meshes with no vertexes or no polys
		// or polygon meshes over or under min/max point size
		
	n=0;
	
	while (n<map->mesh.nmesh) {
		mesh=&map->mesh.meshes[n];
		
			// if no vertexes or polys, delete entire mesh
			
		if ((mesh->nvertex<=0) || (mesh->npoly<=0)) {
			map_mesh_delete(map,n);
			continue;
		}
		
			// delete any polygons that are bad
			
		k=0;
		
		while (k<mesh->npoly) {
			poly=&mesh->polys[k];
			
			if ((poly->ptsz<3) || (poly->ptsz>8)) {
				if (map_mesh_delete_poly(map,n,k)) continue;
			}
			
			k++;
		}
		
		n++;
	}
	
		// remove any unused vertexes

	mesh=map->mesh.meshes;
	
	for (n=0;n!=map->mesh.nmesh;n++) {
		map_mesh_delete_unused_vertexes(map,n);
		mesh++;
	}

		// fix any liquids that got their coordinates
		// switched or odd tide settings
		
	liq=map->liquid.liquids;
	
	for (n=0;n!=map->liquid.nliquid;n++) {
	
			// switched coordinates
			
		if (liq->lft>liq->rgt) {
			k=liq->rgt;
			liq->rgt=liq->lft;
			liq->lft=k;
		}
		
		if (liq->top>liq->bot) {
			k=liq->bot;
			liq->bot=liq->top;
			liq->top=k;
		}
		
		liq++;
	}
}

/* =======================================================

      Write Settings
      
======================================================= */

void write_map_settings_xml(map_type *map)
{
	int					n;
	char				name[32];

    xml_add_tagstart("Creator");
    xml_add_attribute_text("name","dim3 Map Editor");
    xml_add_attribute_int("version",map_current_version);
    xml_add_tagend(TRUE);
    
    xml_add_tagstart("Info");
    xml_add_attribute_text("name",map->info.title);
    xml_add_attribute_text("author",map->info.author);
	xml_add_tagend(TRUE);
    
    xml_add_tagstart("Settings");
    xml_add_attribute_float("gravity",map->physics.gravity);
	xml_add_attribute_float("gravity_max_power",map->physics.gravity_max_power);
	xml_add_attribute_float("gravity_max_speed",map->physics.gravity_max_speed);
    xml_add_attribute_float("resistance",map->physics.resistance);
    xml_add_attribute_float("slope_min_ang",map->physics.slope_min_ang);
    xml_add_attribute_float("slope_max_ang",map->physics.slope_max_ang);
    xml_add_attribute_float("slope_max_speed",map->physics.slope_max_speed);
    xml_add_attribute_float("slope_min_gravity",map->physics.slope_min_gravity);
	xml_add_tagend(TRUE);

    xml_add_tagstart("Optimize");
    xml_add_attribute_boolean("never_cull",map->optimize.never_cull);
    xml_add_attribute_float("cull_angle",map->optimize.cull_angle);
    xml_add_attribute_boolean("ray_trace_obscure",map->optimize.ray_trace_obscure);
    xml_add_attribute_int("model_obscure_distance",map->optimize.obscure_dist.model);
    xml_add_attribute_int("shadow_obscure_distance",map->optimize.obscure_dist.shadow);
    xml_add_attribute_int("effect_obscure_distance",map->optimize.obscure_dist.effect);
    xml_add_attribute_int("shadow_poly_min_area",map->optimize.shadow_poly_min_area);
    xml_add_attribute_boolean("shadow_floor_only",map->optimize.shadow_floor_only);
	xml_add_tagend(TRUE);
	
    xml_add_tagstart("Singleplayer");
	xml_add_attribute_boolean("map_picker",map->singleplayer.map_picker);
	xml_add_tagend(TRUE);

    xml_add_tagstart("Network");
	xml_add_attribute_text("game_list",map->multiplayer.game_list);
	xml_add_attribute_text("params",map->settings.params);
	xml_add_tagend(TRUE);

    xml_add_tagstart("LightMap");
    xml_add_attribute_int("quality",map->light_map.quality);
    xml_add_attribute_int("size",map->light_map.size);
	xml_add_attribute_boolean("use_normals",map->light_map.use_normals);
	xml_add_attribute_boolean("skip_glows",map->light_map.skip_glows);
	xml_add_attribute_float("diffuse_boost",map->light_map.diffuse_boost);
	xml_add_tagend(TRUE);

    xml_add_tagstart("Editor");
	xml_add_attribute_int("view_near_dist",map->editor_setup.view_near_dist);
	xml_add_attribute_int("view_far_dist",map->editor_setup.view_far_dist);
	xml_add_attribute_boolean("link_always_start",map->editor_setup.link_always_start);
	xml_add_tagend(TRUE);
	
    xml_add_tagstart("Media");
	xml_add_attribute_list("type",(char*)media_type_str,map->media.type);
	xml_add_attribute_int("event_id",map->media.event_id);
	xml_add_attribute_text("name",map->media.name);
	xml_add_attribute_text("title_sound_name",map->media.title_sound_name);
	xml_add_tagend(TRUE);
	
    xml_add_tagstart("Music");
	xml_add_attribute_int("fade_msec",map->music.fade_msec);
	xml_add_attribute_text("name",map->music.name);
	for (n=0;n!=max_music_preload;n++) {
		sprintf(name,"preload_%d",n);
		xml_add_attribute_text(name,map->music.preload_name[n]);
	}
	xml_add_tagend(TRUE);
	
    xml_add_tagstart("Ambient_Light");
	xml_add_attribute_color("rgb",&map->ambient.light_color);
    xml_add_tagend(TRUE);
	
    xml_add_tagstart("Ambient_Sound");
	xml_add_attribute_text("name",map->ambient.sound_name);
	xml_add_attribute_float("pitch",map->ambient.sound_pitch);
    xml_add_tagend(TRUE);
	
    xml_add_tagstart("Background");
	xml_add_attribute_boolean("on",map->background.on);
    xml_add_attribute_int("front_fill",map->background.front.fill);
    xml_add_attribute_2_coord_float("front_stamp",map->background.front.size.x,map->background.front.size.y);
    xml_add_attribute_2_coord_float("front_scroll",map->background.front.scroll_factor.x,map->background.front.scroll_factor.y);
	xml_add_attribute_2_coord_float("front_clip",map->background.front.clip.x,map->background.front.clip.y);
 	
	xml_add_attribute_int("middle_fill",map->background.middle.fill);
    xml_add_attribute_2_coord_float("middle_stamp",map->background.middle.size.x,map->background.middle.size.y);
    xml_add_attribute_2_coord_float("middle_scroll",map->background.middle.scroll_factor.x,map->background.middle.scroll_factor.y);
	xml_add_attribute_2_coord_float("middle_clip",map->background.middle.clip.x,map->background.middle.clip.y);
	
    xml_add_attribute_int("back_fill",map->background.back.fill);
    xml_add_attribute_2_coord_float("back_stamp",map->background.back.size.x,map->background.back.size.y);
    xml_add_attribute_2_coord_float("back_scroll",map->background.back.scroll_factor.x,map->background.back.scroll_factor.y);
	xml_add_attribute_2_coord_float("back_clip",map->background.back.clip.x,map->background.back.clip.y);

    xml_add_tagend(TRUE);
    
    xml_add_tagstart("Sky");
    xml_add_tagend(FALSE);
    xml_add_tagstart("Settings");
	xml_add_attribute_boolean("on",map->sky.on);
	xml_add_attribute_list("type",(char*)sky_type_str,map->sky.type);
    xml_add_attribute_int("radius",map->sky.radius);
    xml_add_attribute_int("dome_y",map->sky.dome_y);
    xml_add_attribute_boolean("dome_mirror",map->sky.dome_mirror);
    xml_add_tagend(TRUE);
    xml_add_tagstart("Fill");
    xml_add_attribute_int("index",map->sky.fill);
    xml_add_attribute_int("bottom_index",map->sky.bottom_fill);
    xml_add_attribute_int("north_index",map->sky.north_fill);
    xml_add_attribute_int("south_index",map->sky.south_fill);
    xml_add_attribute_int("east_index",map->sky.east_fill);
    xml_add_attribute_int("west_index",map->sky.west_fill);
    xml_add_attribute_float("factor",map->sky.txt_fact);
    xml_add_tagend(TRUE);
    xml_add_tagclose("Sky");

    xml_add_tagstart("Fog");
	xml_add_attribute_boolean("on",map->fog.on);
	xml_add_attribute_int("count",map->fog.count);
	xml_add_attribute_int("inner_radius",map->fog.inner_radius);
	xml_add_attribute_int("outer_radius",map->fog.outer_radius);
	xml_add_attribute_int("high",map->fog.high);
	xml_add_attribute_int("drop",map->fog.drop);
	xml_add_attribute_int("texture_index",map->fog.texture_idx);
    xml_add_attribute_float("speed",map->fog.speed);
    xml_add_attribute_float("txt_x_fact",map->fog.txt_fact.x);
    xml_add_attribute_float("txt_y_fact",map->fog.txt_fact.y);
	xml_add_attribute_color("rgb",&map->fog.col);
    xml_add_attribute_float("alpha",map->fog.alpha);
	xml_add_attribute_boolean("use_solid_color",map->fog.use_solid_color);
    xml_add_tagend(TRUE);
	
    xml_add_tagstart("Rain");
	xml_add_attribute_boolean("on",map->rain.on);
	xml_add_attribute_int("density",map->rain.density);
	xml_add_attribute_int("radius",map->rain.radius);
	xml_add_attribute_int("height",map->rain.height);
	xml_add_attribute_int("speed",map->rain.speed);
	xml_add_attribute_int("line_width",map->rain.line_width);
	xml_add_attribute_int("line_length",map->rain.line_length);
	xml_add_attribute_int("slant_add",map->rain.slant_add);
	xml_add_attribute_int("slant_time_msec",map->rain.slant_time_msec);
	xml_add_attribute_int("slant_change_msec",map->rain.slant_change_msec);
    xml_add_attribute_float("alpha",map->rain.alpha);
	xml_add_attribute_color("start_rgb",&map->rain.start_color);
	xml_add_attribute_color("end_rgb",&map->rain.end_color);
    xml_add_tagend(TRUE);
}

/* =======================================================

      Write Groups
      
======================================================= */

void write_map_groups_xml(map_type *map)
{
	int						n;
	group_type				*group;

	xml_add_tagstart("Groups");
    xml_add_tagend(FALSE);
    
    group=map->group.groups;
    
    for (n=0;n!=map->group.ngroup;n++) {
		xml_add_tagstart("Group");
		xml_add_attribute_text("name",group->name);
		xml_add_tagend(TRUE);
        
        group++;
    }
    
    xml_add_tagclose("Groups");
}

/* =======================================================

      Write Textures
      
======================================================= */

void write_map_textures_xml(map_type *map)
{
	int						n,frame_count;
    texture_type			*texture;

	xml_add_tagstart("Fills");
    xml_add_tagend(FALSE);
    
    texture=map->textures;
    
    for (n=0;n!=max_map_texture;n++) {
    
		frame_count=map_count_texture_frames(map,n);
		if (frame_count!=0) {
			xml_add_tagstart("Fill");
            xml_add_attribute_int("id",n);
			bitmap_texture_write_xml(texture,frame_count,TRUE);
            xml_add_tagclose("Fill");
         }
        
        texture++;
    }
    
    xml_add_tagclose("Fills");
}

/* =======================================================

      Write Movements
      
======================================================= */

void write_map_movements_xml(map_type *map)
{
	int						n,k;
	movement_type			*movement;
	movement_move_type		*move;

	xml_add_tagstart("Movements");
    xml_add_tagend(FALSE);
    
    movement=map->movement.movements;
    
    for (n=0;n!=map->movement.nmovement;n++) {
    
		xml_add_tagstart("Movement");
		xml_add_attribute_text("name",movement->name);
		xml_add_attribute_int("group",movement->group_idx);
		xml_add_attribute_int("group_reverse",movement->reverse_group_idx);
		xml_add_attribute_boolean("auto_start",movement->auto_start);
		xml_add_attribute_boolean("auto_open",movement->auto_open);
		xml_add_attribute_boolean("auto_open_stand",movement->auto_open_stand);
		xml_add_attribute_boolean("auto_open_never_close",movement->auto_open_never_close);
		xml_add_attribute_int("auto_open_distance",movement->auto_open_distance);
		xml_add_attribute_boolean("loop",movement->loop);
		xml_add_tagend(FALSE);
		
		xml_add_tagstart("Moves");
		xml_add_tagend(FALSE);

		move=movement->moves;
    
		for (k=0;k!=movement->nmove;k++) {
			xml_add_tagstart("Move");
			xml_add_attribute_3_coord_int("c3",move->mov.x,move->mov.y,move->mov.z);
			xml_add_attribute_3_coord_float("r3",move->rot.x,move->rot.y,move->rot.z);
			xml_add_attribute_int("msec",move->msec);
			xml_add_attribute_int("user_id",move->user_id);
			xml_add_attribute_text("sound_name",move->sound_name);
			xml_add_attribute_float("sound_pitch",move->sound_pitch);
			xml_add_tagend(TRUE);
			
			move++;
		}

		xml_add_tagclose("Moves");
		xml_add_tagclose("Movement");
        
        movement++;
    }
    
    xml_add_tagclose("Movements");
}

/* =======================================================

      Write Cinemas
      
======================================================= */

void write_map_cinemas_xml(map_type *map)
{
	int						n,k;
	map_cinema_type			*cinema;
	map_cinema_action_type	*action;

	xml_add_tagstart("Cinemas");
    xml_add_tagend(FALSE);
    
    cinema=map->cinema.cinemas;
    
    for (n=0;n!=map->cinema.ncinema;n++) {
    
		xml_add_tagstart("Cinema");
		xml_add_attribute_text("name",cinema->name);
		xml_add_attribute_int("length",cinema->len_msec);
		xml_add_attribute_boolean("freeze_input",cinema->freeze_input);
		xml_add_attribute_boolean("show_hud",cinema->show_hud);
		xml_add_attribute_boolean("no_cancel",cinema->no_cancel);
		xml_add_tagend(FALSE);
		
		xml_add_tagstart("Actions");
		xml_add_tagend(FALSE);

		action=cinema->actions;
    
		for (k=0;k!=cinema->naction;k++) {

			xml_add_tagstart("Action");

			xml_add_attribute_list("action",(char*)cinema_action_type_str,action->action);

			xml_add_attribute_list("actor_type",(char*)cinema_actor_type_str,action->actor_type);
			xml_add_attribute_text("actor_name",action->actor_name);

			if (action->start_msec!=0) xml_add_attribute_int("start",action->start_msec);
			if (action->end_msec!=0) xml_add_attribute_int("end",action->end_msec);

			xml_add_attribute_boolean("move_reverse",action->move_reverse);

			if (action->animation_name[0]!=0x0) xml_add_attribute_text("animation",action->animation_name);
			if (action->next_animation_name[0]!=0x0) xml_add_attribute_text("next_animation",action->next_animation_name);
			if (action->node_name[0]!=0x0) xml_add_attribute_text("node",action->node_name);
			
			if (action->text_str[0]!=0x0) xml_add_attribute_text("text",action->text_str);

			xml_add_tagend(TRUE);
			
			action++;
		}

		xml_add_tagclose("Actions");
		xml_add_tagclose("Cinema");
        
        cinema++;
    }
    
    xml_add_tagclose("Cinemas");
}

/* =======================================================

      Write Camera
      
======================================================= */

void write_map_camera_xml(map_type *map)
{
    xml_add_tagstart("Camera");
	xml_add_attribute_list("mode",(char*)camera_mode_str,map->camera.camera_mode);
	xml_add_attribute_list("input",(char*)camera_input_str,map->camera.input_mode);
	xml_add_attribute_3_coord_float("ang",map->camera.ang_offset.x,map->camera.ang_offset.y,map->camera.ang_offset.z);
	xml_add_attribute_3_coord_int("pnt",map->camera.pnt_offset.x,map->camera.pnt_offset.y,map->camera.pnt_offset.z);
	xml_add_tagend(FALSE);

	xml_add_tagstart("Plane");
	xml_add_attribute_float("fov",map->camera.plane.fov);
	xml_add_attribute_float("aspectRatio",map->camera.plane.aspect_ratio);
	xml_add_attribute_int("near",map->camera.plane.near_z);
	xml_add_attribute_int("far",map->camera.plane.far_z);
	xml_add_attribute_int("nearOffset",map->camera.plane.near_z_offset);
	xml_add_tagend(TRUE);

	xml_add_tagstart("Chase");
	xml_add_attribute_int("distance",map->camera.chase.distance);
	xml_add_attribute_float("track_speed",map->camera.chase.track_speed);
	xml_add_attribute_3_coord_float("track_ang",map->camera.chase.track_ang.x,map->camera.chase.track_ang.y,map->camera.chase.track_ang.z);
	xml_add_attribute_3_coord_float("slop",map->camera.chase.slop.x,map->camera.chase.slop.y,map->camera.chase.slop.z);
	xml_add_tagend(TRUE);

	xml_add_tagstart("Static");
	xml_add_attribute_boolean("follow",map->camera.c_static.follow);
	xml_add_attribute_text("attach_node",map->camera.c_static.attach_node);
	xml_add_tagend(TRUE);

    xml_add_tagclose("Camera");
}

/* =======================================================

      Write Editor Views
      
======================================================= */

void write_map_editor_views_xml(map_type *map)
{
	int						n;
	editor_view_type		*view;

	xml_add_tagstart("Editor_Views");
    xml_add_tagend(FALSE);
  
    view=map->editor_views.views;
    
    for (n=0;n!=map->editor_views.count;n++) {
    
		xml_add_tagstart("Editor_View");
		xml_add_attribute_2_coord_float("box_top_left",view->box.top,view->box.lft);
		xml_add_attribute_2_coord_float("box_bottom_right",view->box.bot,view->box.rgt);
		xml_add_attribute_3_coord_int("pnt",view->pnt.x,view->pnt.y,view->pnt.z);
		xml_add_attribute_3_coord_float("ang",view->ang.x,view->ang.y,view->ang.z);
		xml_add_attribute_int("uv_layer",view->uv_layer);
  		xml_add_attribute_boolean("ortho",view->ortho);
    	xml_add_attribute_boolean("cull",view->cull);
    	xml_add_attribute_boolean("clip",view->clip);
    	xml_add_attribute_boolean("no_rot",view->no_rot);
		xml_add_tagend(TRUE);
        
        view++;
    }
    
    xml_add_tagclose("Editor_Views");
}

/* =======================================================

      Write Vertexes, Meshes, Liquids
      
======================================================= */

void write_single_vertex(int x,int y,int z)
{
    xml_add_tagstart("v");
	xml_add_attribute_3_coord_int("c3",x,y,z);
    xml_add_tagend(TRUE);
}

void write_single_mesh(map_mesh_type *mesh)
{
	int					n,nvertex,npoly;
	d3pnt				*pt;
	map_mesh_poly_type	*poly;

		// mesh settings

    xml_add_tagstart("Mesh");

  	if (mesh->group_idx!=-1) xml_add_attribute_int("group",mesh->group_idx);

	xml_add_attribute_boolean("off",!mesh->flag.on);
	xml_add_attribute_boolean("pass",mesh->flag.pass_through);
	xml_add_attribute_boolean("moveable",mesh->flag.moveable);
	xml_add_attribute_boolean("hilite",mesh->flag.hilite);
	xml_add_attribute_boolean("simple_collision",mesh->flag.simple_collision);
	xml_add_attribute_boolean("lock_uv",mesh->flag.lock_uv);
	xml_add_attribute_boolean("lock_move",mesh->flag.lock_move);
	xml_add_attribute_boolean("cascade_size",mesh->flag.cascade_size);
	xml_add_attribute_boolean("never_obscure",mesh->flag.never_obscure);
	xml_add_attribute_boolean("never_cull",mesh->flag.never_cull);
	xml_add_attribute_boolean("rot_independent",mesh->flag.rot_independent);
	xml_add_attribute_boolean("no_light_map",mesh->flag.no_light_map);
	xml_add_attribute_boolean("skip_light_map_trace",mesh->flag.skip_light_map_trace);
	xml_add_attribute_boolean("no_halo_obscure",mesh->flag.no_halo_obscure);

	if (mesh->hide_mode!=mesh_hide_mode_never) xml_add_attribute_list("hide",(char*)mesh_hide_mode_str,mesh->hide_mode);
	
  	if (mesh->harm!=0) xml_add_attribute_int("harm",mesh->harm);
	
	if ((mesh->rot_off.x!=0.0f) || (mesh->rot_off.y!=0.0f) || (mesh->rot_off.z!=0.0f)) xml_add_attribute_3_coord_int("rot_off",mesh->rot_off.x,mesh->rot_off.y,mesh->rot_off.z);

	xml_add_tagend(FALSE);

		// messages

    if ((mesh->msg.entry_on) || (mesh->msg.exit_on) || (mesh->msg.base_on) || (mesh->msg.map_change_on)) {
        xml_add_tagstart("Messages");
        xml_add_tagend(FALSE);
        
        xml_add_tagstart("Entry");
        xml_add_attribute_boolean("on",mesh->msg.entry_on);
        xml_add_attribute_int("id",mesh->msg.entry_id);
        xml_add_tagend(TRUE);
        
        xml_add_tagstart("Exit");
        xml_add_attribute_boolean("on",mesh->msg.exit_on);
        xml_add_attribute_int("id",mesh->msg.exit_id);
        xml_add_tagend(TRUE);
		
        xml_add_tagstart("Base");
        xml_add_attribute_boolean("on",mesh->msg.base_on);
        xml_add_attribute_int("team",mesh->msg.base_team);
        xml_add_tagend(TRUE);
		
        xml_add_tagstart("Map");
        xml_add_attribute_boolean("on",mesh->msg.map_change_on);
		xml_add_attribute_text("name",mesh->msg.map_name);
		xml_add_attribute_text("spot_name",mesh->msg.map_spot_name);
		xml_add_tagend(TRUE);

        xml_add_tagclose("Messages");
    }
	
		// import
		
	xml_add_tagstart("Import");
	xml_add_attribute_text("obj",mesh->import.obj_name);
	xml_add_attribute_text("group",mesh->import.group_name);
	xml_add_tagend(TRUE);

		// vertexes

    xml_add_tagstart("Vertexes");
	xml_add_tagend(FALSE);

	nvertex=mesh->nvertex;
	pt=mesh->vertexes;

	for (n=0;n!=nvertex;n++) {
		write_single_vertex(pt->x,pt->y,pt->z);
		pt++;
	}
	
	xml_add_tagclose("Vertexes");

		// polys

    xml_add_tagstart("Polys");
	xml_add_tagend(FALSE);

	npoly=mesh->npoly;
	poly=mesh->polys;

	for (n=0;n!=npoly;n++) {

			// single polygon

		xml_add_tagstart("p");
		
		xml_add_attribute_int_array("v",poly->v,poly->ptsz,FALSE);
		
		xml_add_attribute_int("txt",poly->txt_idx);
		if (poly->lmap_txt_idx!=-1) xml_add_attribute_int("lmp",poly->lmap_txt_idx);

		xml_add_attribute_3_coord_float("t3",poly->tangent_space.tangent.x,poly->tangent_space.tangent.y,poly->tangent_space.tangent.z);
		xml_add_attribute_3_coord_float("n3",poly->tangent_space.normal.x,poly->tangent_space.normal.y,poly->tangent_space.normal.z);

		xml_add_attribute_uv_x_array("x",poly->main_uv.uvs,poly->ptsz);
		xml_add_attribute_uv_y_array("y",poly->main_uv.uvs,poly->ptsz);
		if ((poly->shift.x!=0.0f) || (poly->shift.y!=0.0f)) xml_add_attribute_2_coord_float("shift",poly->shift.x,poly->shift.y);
		if (poly->lmap_txt_idx!=-1) {
			xml_add_attribute_uv_x_array("x_1",poly->lmap_uv.uvs,poly->ptsz);
			xml_add_attribute_uv_y_array("y_1",poly->lmap_uv.uvs,poly->ptsz);
		}
		
		xml_add_attribute_boolean("climbable",poly->flag.climbable);
		xml_add_attribute_boolean("never_cull",poly->flag.never_cull);
		xml_add_attribute_boolean("obscuring",poly->flag.obscuring);
		
		if (poly->camera[0]!=0x0) xml_add_attribute_text("camera",poly->camera);

		xml_add_tagend(TRUE);

		poly++;
	}
	
	xml_add_tagclose("Polys");

	xml_add_tagclose("Mesh");
}

void write_single_liquid(map_liquid_type *liq)
{
		// liquid settings

    xml_add_tagstart("Liquid");
  	if (liq->group_idx!=-1) xml_add_attribute_int("group",liq->group_idx);
	xml_add_attribute_boolean("never_obscure",liq->flag.never_obscure);
	xml_add_attribute_boolean("never_cull",liq->flag.never_cull);
	xml_add_attribute_boolean("no_draw",liq->flag.no_draw);
	xml_add_attribute_boolean("lock_uv",liq->flag.lock_uv);
	xml_add_tagend(FALSE);

		// polygon

	xml_add_tagstart("Poly");
	xml_add_attribute_int("txt",liq->txt_idx);
	if (liq->lmap_txt_idx!=-1) xml_add_attribute_int("lmp",liq->lmap_txt_idx);
	xml_add_attribute_3_coord_int("v1",liq->lft,liq->y,liq->top);
	xml_add_attribute_3_coord_int("v2",liq->rgt,liq->y,liq->bot);
	xml_add_attribute_int("depth",liq->depth);

	xml_add_attribute_2_coord_float("uv_off",liq->main_uv.offset.x,liq->main_uv.offset.y);
	xml_add_attribute_2_coord_float("uv_size",liq->main_uv.size.x,liq->main_uv.size.y);
	if (liq->lmap_txt_idx!=-1) {
		xml_add_attribute_2_coord_float("uv_1_off",liq->lmap_uv.offset.x,liq->lmap_uv.offset.y);
		xml_add_attribute_2_coord_float("uv_1_size",liq->lmap_uv.size.x,liq->lmap_uv.size.y);
	}

	xml_add_attribute_color("rgb",&liq->col);
	xml_add_attribute_float("tint_alpha",liq->tint_alpha);
	if ((liq->shift.x!=0.0f) || (liq->shift.y!=0.0f)) xml_add_attribute_2_coord_float("shift",liq->shift.x,liq->shift.y);

	if (liq->camera[0]!=0x0) xml_add_attribute_text("camera",liq->camera);
	if (liq->ambient.sound_name[0]!=0x0) xml_add_attribute_text("ambient_sound_name",liq->ambient.sound_name);
	xml_add_tagend(TRUE);

		// physics

	xml_add_tagstart("Physic");
	xml_add_attribute_float("speed_alter",liq->speed_alter);
	xml_add_tagend(TRUE);

		// harm

	xml_add_tagstart("Harm");
	xml_add_attribute_int("harm",liq->harm.in_harm);
	xml_add_attribute_int("drown_harm",liq->harm.drown_harm);
	xml_add_attribute_int("drown_tick",liq->harm.drown_tick);
	xml_add_tagend(TRUE);

		// tides

	xml_add_tagstart("Tide");
	xml_add_attribute_int("rate",liq->tide.rate);
	xml_add_attribute_int("high",liq->tide.high);
	xml_add_attribute_float("uv_shift",liq->tide.uv_shift);
	xml_add_tagend(TRUE);

		// waves

	xml_add_tagstart("Wave");
	xml_add_attribute_boolean("on",liq->wave.on);
	xml_add_attribute_boolean("dir_north_south",liq->wave.dir_north_south);
	xml_add_attribute_int("length",liq->wave.length);
	xml_add_attribute_int("high",liq->wave.high);
	xml_add_attribute_int("period_msec",liq->wave.period_msec);
	xml_add_tagend(TRUE);
	
		// reflection

	xml_add_tagstart("Reflect");
	xml_add_attribute_int("texture_size",liq->reflect.texture_size);
	xml_add_attribute_int("x_refract_factor",liq->reflect.x_refract_factor);
	xml_add_attribute_int("z_refract_factor",liq->reflect.z_refract_factor);
	xml_add_attribute_color("no_hit_col",&liq->reflect.no_hit_col);
	xml_add_attribute_float("alpha",liq->reflect.alpha);
	xml_add_tagend(TRUE);

		// overlay

	xml_add_tagstart("Overlay");
	xml_add_attribute_boolean("on",liq->overlay.on);
	xml_add_attribute_int("txt_idx",liq->overlay.txt_idx);
	xml_add_attribute_int("stamp_size",liq->overlay.stamp_size);
	if ((liq->overlay.shift.x!=0.0f) || (liq->overlay.shift.y!=0.0f)) xml_add_attribute_2_coord_float("shift",liq->overlay.shift.x,liq->overlay.shift.y);
	xml_add_tagend(TRUE);

	xml_add_tagclose("Liquid");
}

/* =======================================================

      Write Map Light Settings
      
======================================================= */

void write_single_light_setting(map_light_setting_type *lit_set)
{
	xml_add_attribute_list("type",(char*)light_type_str,lit_set->type);
	xml_add_attribute_boolean("light_map",lit_set->light_map);
	xml_add_attribute_list("direction",(char*)light_direction_str,lit_set->direction);
	xml_add_attribute_int("intensity",lit_set->intensity);
	xml_add_attribute_float("exponent",lit_set->exponent);
	xml_add_attribute_color("rgb",&lit_set->col);
	xml_add_attribute_text("halo",lit_set->halo_name);
	xml_add_attribute_boolean("off",!lit_set->on);
}

/* =======================================================

      Write Map XML
      
======================================================= */

bool write_map_xml(map_type *map,char *err_str)
{
    int						k,nmesh,nliq;
	bool					ok;
    map_mesh_type			*mesh;
	map_liquid_type			*liq;
    map_light_type			*light;
    map_sound_type			*sound;
	map_particle_type		*particle;
    node_type				*node;
    spot_type				*spot;
	map_scenery_type		*scenery;
	
		// fix any problems with map before saving
		
	write_map_fix_problems(map);
	
		// save map

    xml_new_file();
    
    xml_add_tagstart("Map");
    xml_add_tagend(FALSE);
    
        // map settings

	write_map_settings_xml(map);
	write_map_groups_xml(map);
	write_map_textures_xml(map);
	write_map_movements_xml(map);
	write_map_cinemas_xml(map);
	write_map_camera_xml(map);

		// editor setup

	write_map_editor_views_xml(map);

		// meshes

	xml_add_tagstart("Meshes");
	xml_add_tagend(FALSE);

	nmesh=map->mesh.nmesh;
	mesh=map->mesh.meshes;

	for (k=0;k!=nmesh;k++) {
		write_single_mesh(mesh);
		mesh++;
	}

    xml_add_tagclose("Meshes");

		// liquids

	xml_add_tagstart("Liquids");
	xml_add_tagend(FALSE);

	nliq=map->liquid.nliquid;
	liq=map->liquid.liquids;

	for (k=0;k!=nliq;k++) {
		write_single_liquid(liq);
		liq++;
	}

    xml_add_tagclose("Liquids");

		// scenery

	xml_add_tagstart("Sceneries");
	xml_add_tagend(FALSE);
	
	for (k=0;k!=map->nscenery;k++) {
		scenery=&map->sceneries[k];
		
		xml_add_tagstart("Scenery");
		xml_add_attribute_3_coord_int("c3",scenery->pnt.x,scenery->pnt.y,scenery->pnt.z);
		xml_add_attribute_text("model_name",scenery->model_name);
		xml_add_attribute_text("animation_name",scenery->animation_name);
		xml_add_attribute_3_coord_float("angle",scenery->ang.x,scenery->ang.y,scenery->ang.z);
		xml_add_attribute_boolean("contact",scenery->contact_object_on);
		xml_add_attribute_boolean("contact_projectile",scenery->contact_projectile_on);
		xml_add_attribute_boolean("contact_hit_box",scenery->contact_hit_box);
		xml_add_attribute_boolean("face_forward",scenery->face_forward);
		xml_add_attribute_boolean("shadow",scenery->shadow);
		xml_add_attribute_int_array("frames",scenery->texture_frame,max_map_scenery_model_texture_frame,FALSE);
		xml_add_attribute_float("resize",scenery->resize);
		xml_add_tagend(TRUE);
	}
	
	xml_add_tagclose("Sceneries");
		
		// lights
    
	xml_add_tagstart("Lights");
	xml_add_tagend(FALSE);

	for (k=0;k!=map->nlight;k++) {
		light=&map->lights[k];

		xml_add_tagstart("Light");
		xml_add_attribute_text("name",light->name);
		xml_add_attribute_3_coord_int("c3",light->pnt.x,light->pnt.y,light->pnt.z);
		
		write_single_light_setting(&light->setting);
		xml_add_tagend(TRUE);
	}
	
	xml_add_tagclose("Lights");

		// sounds
		
	xml_add_tagstart("Sounds");
	xml_add_tagend(FALSE);
	
	for (k=0;k!=map->nsound;k++) {
		sound=&map->sounds[k];
	
		xml_add_tagstart("Sound");
		xml_add_attribute_text("name",sound->name);
		xml_add_attribute_3_coord_int("c3",sound->pnt.x,sound->pnt.y,sound->pnt.z);
		xml_add_attribute_float("pitch",sound->pitch);
		xml_add_attribute_boolean("off",!sound->on);
		xml_add_tagend(TRUE);
	}
	
	xml_add_tagclose("Sounds");
	
		// particles
		
	xml_add_tagstart("Particles");
	xml_add_tagend(FALSE);
	
	for (k=0;k!=map->nparticle;k++) {
		particle=&map->particles[k];
	
		xml_add_tagstart("Particle");
		xml_add_attribute_text("name",particle->name);
		xml_add_attribute_3_coord_int("c3",particle->pnt.x,particle->pnt.y,particle->pnt.z);
		xml_add_attribute_int("spawn_tick",particle->spawn_tick);
		xml_add_attribute_int("slop_tick",particle->slop_tick);
		xml_add_attribute_boolean("single_spawn",particle->single_spawn);
		xml_add_attribute_boolean("off",!particle->on);
		xml_add_tagend(FALSE);
		
		xml_add_tagstart("Light");
		write_single_light_setting(&particle->light_setting);
		xml_add_tagend(TRUE);
		
		xml_add_tagclose("Particle");
	}
	
	xml_add_tagclose("Particles");
		
		// nodes
    
	xml_add_tagstart("Nodes");
	xml_add_tagend(FALSE);

	for (k=0;k!=map->nnode;k++) {
		node=&map->nodes[k];
	
		xml_add_tagstart("Node");
		xml_add_attribute_int("id",k);
		xml_add_attribute_3_coord_int("c3",node->pnt.x,node->pnt.y,node->pnt.z);
		xml_add_attribute_3_coord_float("angle",node->ang.x,node->ang.y,node->ang.z);
		xml_add_attribute_boolean("follow_camera",node->follow_camera);
		xml_add_attribute_float("alpha",node->alpha);
		xml_add_attribute_text("name",node->name);
		xml_add_attribute_int("event_id",node->event_id);
		xml_add_tagend(FALSE);
	
		xml_add_tagstart("Link");
		xml_add_attribute_short_array("node",node->link,max_node_link,TRUE);
		xml_add_tagend(TRUE);
	
		xml_add_tagstart("Hint");
		xml_add_attribute_short_array("node",node->path_hint,map->nnode,FALSE);
		xml_add_tagend(TRUE);
	
		xml_add_tagclose("Node");
	}

	xml_add_tagclose("Nodes");
		
		// spots

	xml_add_tagstart("Spots");
	xml_add_tagend(FALSE);
	
	for (k=0;k!=map->nspot;k++) {
		spot=&map->spots[k];
		
		xml_add_tagstart("Spot");
		xml_add_attribute_int("id",k);
		xml_add_attribute_3_coord_int("c3",spot->pnt.x,spot->pnt.y,spot->pnt.z);
		xml_add_attribute_text("name",spot->name);
		xml_add_attribute_list("type",(char*)spot_type_str,spot->type);
		xml_add_attribute_text("script",spot->script);
		xml_add_attribute_text("display_model",spot->display_model);
		xml_add_attribute_text("params",spot->params);
		xml_add_attribute_float("angle",spot->ang.y);
		xml_add_attribute_list("skill",(char*)skill_type_str,spot->skill);
		xml_add_attribute_list("spawn",(char*)spawn_type_str,spot->spawn);
		xml_add_attribute_int("checkpoint_radius",spot->checkpoint.radius);
		xml_add_tagend(TRUE);
	}
	
	xml_add_tagclose("Spots");

		// close map

    xml_add_tagclose("Map");

        // save the map
		
	ok=xml_save_file(map->info.load_path,err_str);
    xml_close_file();
	
	return(ok);
}

