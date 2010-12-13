/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Property Palette Node

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
#include "view.h"
#include "dialog.h"

#define kNodePropertyName					0
#define kNodePropertyEventID				1
#define kNodePropertyAlpha					2
#define kNodePropertyFollowCamera			3

extern map_type					map;
extern editor_state_type		state;
extern editor_setup_type		setup;

extern list_palette_type		property_palette;

/* =======================================================

      Property Palette Fill Node
      
======================================================= */

void property_palette_fill_node(int node_idx)
{
	node_type			*node;

	node=&map.nodes[node_idx];

	list_palette_add_header(&property_palette,0,"Node Identity");
	list_palette_add_string(&property_palette,kNodePropertyName,"Name",node->name);
	list_palette_add_string_int(&property_palette,kNodePropertyEventID,"Event ID",node->event_id);
	list_palette_add_string_float(&property_palette,kNodePropertyAlpha,"Alpha",node->alpha);

	list_palette_add_header(&property_palette,0,"Node Settings");
	list_palette_add_checkbox(&property_palette,kNodePropertyFollowCamera,"Follow Camera",node->follow_camera);
	
	list_palette_add_header(&property_palette,0,"Node Info [Not Editable]");
	list_palette_add_point(&property_palette,-1,"Position",&node->pnt);
	list_palette_add_angle(&property_palette,-1,"Angle",&node->ang);
}

/* =======================================================

      Property Palette Click Node
      
======================================================= */

void property_palette_click_node(int node_idx,int id)
{
	node_type			*node;

	node=&map.nodes[node_idx];

	switch (id) {

		case kNodePropertyName:
			dialog_property_string_run(list_string_value_string,(void*)node->name,name_str_len);
			break;

		case kNodePropertyEventID:
			dialog_property_string_run(list_string_value_positive_int,(void*)&node->event_id,0);
			break;

		case kNodePropertyAlpha:
			dialog_property_string_run(list_string_value_0_to_1_float,(void*)&node->alpha,0);
			break;

		case kNodePropertyFollowCamera:
			node->follow_camera=!node->follow_camera;
			break;

	}

	main_wind_draw();
}

