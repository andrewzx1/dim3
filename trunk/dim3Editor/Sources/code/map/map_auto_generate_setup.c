/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Auto Generation Setup

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

extern map_type					map;
extern file_path_setup_type		file_path_setup;

extern ag_state_type			ag_state;

/* =======================================================

      Auto Generate Memory
      
======================================================= */

bool ag_initialize(char *err_str)
{
	ag_state.rooms=NULL;

	ag_state.rooms=(ag_room_type*)malloc(ag_max_room*sizeof(ag_room_type));
	if (ag_state.rooms==NULL) {
		strcpy(err_str,"Out of Memory");
		return(FALSE);
	}

	return(TRUE);
}

void ag_release(void)
{
	if (ag_state.rooms!=NULL) free(ag_state.rooms);
	ag_state.rooms=NULL;
}

/* =======================================================

      Check Required Textures
      
======================================================= */

texture_type* ag_create_texture(int idx,char *name)
{
	int					n;
	texture_type		*texture;

		// close old texture

	map_textures_close_texture(&map,idx);

		// start new texture

	texture=&map.textures[idx];
	
	sprintf(texture->material_name,name);
	texture->shader_name[0]=0x0;
	texture->additive=FALSE;
	texture->compress=FALSE;

	for (n=0;n!=max_texture_frame;n++) {
		texture->frames[n].name[0]=0x0;
	}
	
	sprintf(texture->frames[0].name,"%s/%s",map.info.name,texture->material_name);

	return(texture);
}

void ag_create_texture_set(void)
{
	char				base_path[1024],dir_path[1024];
	texture_type		*texture;
	
		// create directory for new textures
		
	file_paths_data_default(&file_path_setup,base_path,"Bitmaps/Textures",NULL,NULL);

	sprintf(dir_path,"%s/%s",base_path,map.info.name);
	os_create_directory(dir_path);

		// note: this randomness requires the
		// random seed to already be set

	texture=ag_create_texture(ag_texture_wall,"Wall");
	bitmap_ag_texture_brick(&texture->frames[0],base_path,512);
	map_textures_read_texture(&map,ag_texture_wall);
	
	texture=ag_create_texture(ag_texture_floor,"Floor");
	bitmap_ag_texture_tile(&texture->frames[0],base_path,512);
	map_textures_read_texture(&map,ag_texture_floor);

	texture=ag_create_texture(ag_texture_ceiling,"Ceiling");
	bitmap_ag_texture_tile(&texture->frames[0],base_path,512);
	map_textures_read_texture(&map,ag_texture_ceiling);

	texture=ag_create_texture(ag_texture_connect,"Connect");
	bitmap_ag_texture_brick(&texture->frames[0],base_path,512);
	map_textures_read_texture(&map,ag_texture_connect);

	texture=ag_create_texture(ag_texture_alt_wall,"Alt Wall");
	bitmap_ag_texture_brick(&texture->frames[0],base_path,512);
	map_textures_read_texture(&map,ag_texture_alt_wall);

	texture=ag_create_texture(ag_texture_second_floor,"Second Story");
	bitmap_ag_texture_brick(&texture->frames[0],base_path,512);
	map_textures_read_texture(&map,ag_texture_second_floor);

	texture=ag_create_texture(ag_texture_lift,"Lift");
	bitmap_ag_texture_metal(&texture->frames[0],base_path,512);
	map_textures_read_texture(&map,ag_texture_lift);

	texture=ag_create_texture(ag_texture_door,"Door");
	bitmap_ag_texture_metal(&texture->frames[0],base_path,512);
	map_textures_read_texture(&map,ag_texture_door);

	texture=ag_create_texture(ag_texture_stair,"Stairs");
	bitmap_ag_texture_cement(&texture->frames[0],base_path,512,TRUE);
	map_textures_read_texture(&map,ag_texture_stair);

	texture=ag_create_texture(ag_texture_decoration_pillar,"Pillar");
	bitmap_ag_texture_cement(&texture->frames[0],base_path,512,FALSE);
	map_textures_read_texture(&map,ag_texture_decoration_pillar);

	texture=ag_create_texture(ag_texture_decoration_box,"Box");
	bitmap_ag_texture_wood(&texture->frames[0],base_path,512);
	map_textures_read_texture(&map,ag_texture_decoration_box);

	texture=ag_create_texture(ag_texture_equipment,"Equipment");
	bitmap_ag_texture_machine(&texture->frames[0],base_path,512);
	map_textures_read_texture(&map,ag_texture_equipment);

	texture=ag_create_texture(ag_texture_window,"Window");
	bitmap_ag_texture_window(&texture->frames[0],base_path,512);
	map_textures_read_texture(&map,ag_texture_window);
}


