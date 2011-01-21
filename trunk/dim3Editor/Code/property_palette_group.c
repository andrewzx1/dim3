/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Property Palette Group

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
#include "ui_common.h"
#include "interface.h"
#include "dialog.h"

#define kGroupPropertyName					0

extern map_type					map;
extern editor_state_type		state;
extern editor_setup_type		setup;

extern list_palette_type		property_palette;

/* =======================================================

      Property Palette Fill Group
      
======================================================= */

void property_palette_fill_group(int group_idx)
{
	int					n,mesh_cnt,liq_cnt;
	map_mesh_type		*mesh;
	map_liquid_type		*liq;
	group_type			*group;

	group=&map.group.groups[group_idx];

		// get some counts

	mesh_cnt=0;
	liq_cnt=0;
	
	mesh=map.mesh.meshes;
	
	for (n=0;n!=map.mesh.nmesh;n++) {
		if (mesh->group_idx==group_idx) mesh_cnt++;
		mesh++;
	}
	
	liq=map.liquid.liquids;
	
	for (n=0;n!=map.liquid.nliquid;n++) {
		if (liq->group_idx==group_idx) liq_cnt++;
		liq++;
	}

		// fill items

	list_palette_add_header(&property_palette,0,"Group Options");
	list_palette_add_string(&property_palette,kGroupPropertyName,"Name",group->name,FALSE);
	
	list_palette_add_header(&property_palette,0,"Group Info");
	list_palette_add_string_int(&property_palette,-1,"Mesh Count",mesh_cnt,TRUE);
	list_palette_add_string_int(&property_palette,-1,"Liquid Count",liq_cnt,TRUE);
}

/* =======================================================

      Property Palette Click Group
      
======================================================= */

void property_palette_click_group(int group_idx,int id)
{
	group_type			*group;

	group=&map.group.groups[group_idx];

	switch (id) {

		case kGroupPropertyName:
			dialog_property_string_run(list_string_value_string,(void*)group->name,name_str_len,0,0);
			break;

	}

	main_wind_draw();
}

