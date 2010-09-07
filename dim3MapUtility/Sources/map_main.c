/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Map Main Routines

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

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3maputility.h"
#endif

maputility_settings_type		maputility_settings;

/* =======================================================

      Map Setup
      
======================================================= */

void map_setup(file_path_setup_type *file_path_setup,int anisotropic_mode,int mipmap_mode,int texture_quality_mode,bool in_engine)
{
	memmove(&maputility_settings.file_path_setup,file_path_setup,sizeof(file_path_setup_type));
	maputility_settings.anisotropic_mode=anisotropic_mode;
	maputility_settings.mipmap_mode=mipmap_mode;
	maputility_settings.texture_quality_mode=texture_quality_mode;
	maputility_settings.in_engine=in_engine;
}

/* =======================================================

      New Maps
      
======================================================= */

bool map_new(map_type *map,char *name)
{
		// info
		
	strcpy(map->info.name,name);
	map->info.title[0]=0x0;
	map->info.author[0]=0x0;
	
	file_paths_data_default(&maputility_settings.file_path_setup,map->info.load_path,"Maps",map->info.name,"xml");
	
		// settings
		
	map->settings.gravity=1;
	map->settings.gravity_max_power=32;
	map->settings.gravity_max_speed=400;
	map->settings.resistance=1;
	map->settings.never_cull=FALSE;
	map->settings.no_shaders=FALSE;
	
	map->settings.network_game_list[0]=0x0;

	map->settings.light_map.quality=70;
	map->settings.light_map.quality=512;
	map->settings.light_map.pixel_border_count=6;
	map->settings.light_map.blur_count=3;
	
	map->settings.editor.txt_scale_x=0.04f;
	map->settings.editor.txt_scale_y=0.04f;
	map->settings.editor.view_near_dist=6*map_enlarge;
	map->settings.editor.view_far_dist=2000*map_enlarge;
	map->settings.editor.link_always_start=FALSE;

		// camera

	map->camera.mode=cv_fpp;
	
	map->camera.pnt.x=0;
	map->camera.pnt.y=0;
	map->camera.pnt.z=0;

	map->camera.ang.x=0.0f;
	map->camera.ang.y=0.0f;
	map->camera.ang.z=0.0f;
    
    map->camera.chase.distance=map_enlarge*50;
	map->camera.chase.track_speed=1.0f;
    
	map->camera.chase.slop.x=0.0f;
	map->camera.chase.slop.y=0.0f;
	map->camera.chase.slop.z=0.0f;
    
    map->camera.c_static.follow=TRUE;
	map->camera.c_static.attach_node[0]=0x0;

	map->camera.plane.fov=60.0f;
	map->camera.plane.aspect_ratio=1.0f;
	map->camera.plane.near_z=3*map_enlarge;
	map->camera.plane.far_z=2000*map_enlarge;
	map->camera.plane.near_z_offset=-(3*map_enlarge);
	
		// media
		
	map->media.type=mi_none;
	map->media.name[0]=0x0;
	map->media.title_sound_name[0]=0x0;
	
		// music
		
	map->music.fade_msec=0;
	map->music.name[0]=0x0;
	
		// ambients
		
	map->ambient.light_color.r=map->ambient.light_color.g=map->ambient.light_color.b=0.0f;
	map->ambient.light_map_boost=0.0f;
	map->ambient.sound_name[0]=0x0;
	map->ambient.sound_pitch=1.0f;
	
		// rain
		
	map->rain.on=FALSE;
	map->rain.density=200;
	map->rain.radius=40000;
	map->rain.height=15000;
	map->rain.speed=35;
	map->rain.line_length=1500;
	map->rain.line_width=1;
	map->rain.slant_add=20;
	map->rain.slant_time_msec=8000;
	map->rain.slant_change_msec=1000;
	map->rain.alpha=0.3f;
	map->rain.start_color.r=map->rain.start_color.g=map->rain.start_color.b=1.0f;
	map->rain.end_color.r=map->rain.end_color.g=0.2f;
	map->rain.end_color.b=1.0f;
    
		// background
		
	map->background.on=FALSE;
	map->background.front.fill=-1;
	map->background.front.x_scroll_fact=0.0f;
	map->background.front.y_scroll_fact=0.0f;
	map->background.middle.fill=-1;
	map->background.middle.x_scroll_fact=0.0f;
	map->background.middle.y_scroll_fact=0.0f;
	map->background.back.fill=-1;
	map->background.back.x_scroll_fact=0.0f;
	map->background.back.y_scroll_fact=0.0f;
	
		// sky
		
	map->sky.on=FALSE;
	map->sky.type=st_cube;
    map->sky.fill=-1;
    map->sky.bottom_fill=-1;
    map->sky.north_fill=-1;
    map->sky.south_fill=-1;
    map->sky.east_fill=-1;
    map->sky.west_fill=-1;
    map->sky.radius=300*map_enlarge;
	map->sky.dome_y=0;
	map->sky.dome_mirror=FALSE;
	map->sky.txt_fact=1;
	map->sky.txt_x_shift=0;
	map->sky.txt_y_shift=0;
	
		// fog
		
	map->fog.on=FALSE;
	map->fog.count=30;
	map->fog.outer_radius=1000*map_enlarge;
	map->fog.inner_radius=500*map_enlarge;
	map->fog.high=150*map_enlarge;
	map->fog.drop=50*map_enlarge;
	map->fog.texture_idx=0;
	map->fog.speed=0.001f;
	map->fog.txt_x_fact=8.0f;
	map->fog.txt_y_fact=1.0f;
	map->fog.col.r=0.5;
	map->fog.col.g=0.5;
	map->fog.col.b=0.5;
	map->fog.alpha=0.05f;
	map->fog.use_solid_color=TRUE;

		// editor setup

	map->editor_views.count=0;
		
		// pieces
	
	map->nspot=0;
	map->nnode=0;
	map->nscenery=0;
	map->nmovement=0;
	map->nlight=0;
	map->nsound=0;
	map->nparticle=0;
	map->ngroup=0;
	map->narea=0;

		// meshes and liquids

	map->mesh.nmesh=0;
	map->mesh.meshes=NULL;

	map->liquid.nliquid=0;
	map->liquid.liquids=NULL;
	
		// memory
		
	map->textures=(texture_type*)malloc(max_map_texture*sizeof(texture_type));
	if (map->textures==NULL) return(FALSE);
	
	map->spots=(spot_type*)malloc(max_spot*sizeof(spot_type));
	if (map->spots==NULL) return(FALSE);
	
	map->nodes=(node_type*)malloc(max_node*sizeof(node_type));
	if (map->nodes==NULL) return(FALSE);
	
	map->sceneries=(map_scenery_type*)malloc(max_map_scenery*sizeof(map_scenery_type));
	if (map->sceneries==NULL) return(FALSE);
	
	map->movements=(movement_type*)malloc(max_movement*sizeof(movement_type));
	if (map->movements==NULL) return(FALSE);
	
	map->lights=(map_light_type*)malloc(max_map_light*sizeof(map_light_type));
	if (map->lights==NULL) return(FALSE);
	
	map->sounds=(map_sound_type*)malloc(max_map_sound*sizeof(map_sound_type));
	if (map->sounds==NULL) return(FALSE);
	
	map->particles=(map_particle_type*)malloc(max_map_particle*sizeof(map_particle_type));
	if (map->particles==NULL) return(FALSE);
	
	map->groups=(group_type*)malloc(max_group*sizeof(group_type));
	if (map->groups==NULL) return(FALSE);

		// zero memory
		
	bzero(map->textures,(max_map_texture*sizeof(texture_type)));
	bzero(map->spots,(max_spot*sizeof(spot_type)));
	bzero(map->nodes,(max_node*sizeof(node_type)));
	bzero(map->sceneries,(max_map_scenery*sizeof(map_scenery_type)));
	bzero(map->movements,(max_movement*sizeof(movement_type)));
	bzero(map->lights,(max_map_light*sizeof(map_light_type)));
	bzero(map->sounds,(max_map_sound*sizeof(map_sound_type)));
	bzero(map->particles,(max_map_particle*sizeof(map_particle_type)));
	bzero(map->groups,(max_group*sizeof(group_type)));
	
		// bitmaps
		
	map_textures_new(map);
		
	return(TRUE);
}

/* =======================================================

      Open Maps
      
======================================================= */

bool map_open(map_type *map,char *name)
{
	if (!map_new(map,name)) return(FALSE);
	
	strcpy(map->info.name,name);
	file_paths_data(&maputility_settings.file_path_setup,map->info.load_path,"Maps",map->info.name,"xml");
	
	if (!read_map_xml(map)) return(FALSE);

	if (!map_textures_read(map)) return(FALSE);

	map_find_random_spot_clear(map,NULL,-1);
	
	return(TRUE);
}

/* =======================================================

      Reload Maps
      
======================================================= */

bool map_reload(map_type *map)
{
	if (!read_map_xml(map)) return(FALSE);

	return(TRUE);
}

/* =======================================================

      Save Maps
      
======================================================= */

bool map_save(map_type *map)
{
    return(write_map_xml(map));
}

/* =======================================================

      Close Maps
      
======================================================= */

void map_close(map_type *map)
{
		// bitmaps
		
	map_textures_close(map);

		// meshes and liquids

	if (map->mesh.meshes!=NULL) {
		free(map->mesh.meshes);
		map->mesh.nmesh=0;
		map->mesh.meshes=NULL;
	}
	if (map->liquid.liquids!=NULL) {
		free(map->liquid.liquids);
		map->liquid.nliquid=0;
		map->liquid.liquids=NULL;
	}
	
		// memory
		
	free(map->textures);
	free(map->spots);
	free(map->nodes);
	free(map->sceneries);
	free(map->movements);
	free(map->lights);
	free(map->sounds);
	free(map->particles);
	free(map->groups);
}

/* =======================================================

      Refresh Textures
      
======================================================= */

void map_refresh_textures(map_type *map)
{
	map_textures_close(map);
	map_textures_read(map);
}

