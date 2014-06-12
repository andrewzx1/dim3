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

bool ag_check_required_textures(char *err_str)
{
	texture_type		*texture;

		// note: this randomness requires the
		// random seed to already be set

	texture=&map.textures[ag_texture_wall];
	bitmap_ag_texture_brick(&texture->frames[0],512);

	texture=&map.textures[ag_texture_floor];
	bitmap_ag_texture_tile(&texture->frames[0],512);

	texture=&map.textures[ag_texture_ceiling];
	bitmap_ag_texture_tile(&texture->frames[0],512);

	texture=&map.textures[ag_texture_connect];
	bitmap_ag_texture_brick(&texture->frames[0],512);

	texture=&map.textures[ag_texture_alt_wall];
	bitmap_ag_texture_brick(&texture->frames[0],512);

	texture=&map.textures[ag_texture_second_floor];
	bitmap_ag_texture_brick(&texture->frames[0],512);

	texture=&map.textures[ag_texture_lift];
	bitmap_ag_texture_metal(&texture->frames[0],512);

	texture=&map.textures[ag_texture_door];
	bitmap_ag_texture_metal(&texture->frames[0],512);

	texture=&map.textures[ag_texture_stair];
	bitmap_ag_texture_cement(&texture->frames[0],512);

	texture=&map.textures[ag_texture_decoration_pillar];
	bitmap_ag_texture_cement(&texture->frames[0],512);

	texture=&map.textures[ag_texture_decoration_box];
	bitmap_ag_texture_wood(&texture->frames[0],512);

	texture=&map.textures[ag_texture_equipment];
	bitmap_ag_texture_machine(&texture->frames[0],512);

	texture=&map.textures[ag_texture_window];
	bitmap_ag_texture_window(&texture->frames[0],512);

	return(TRUE);
}


