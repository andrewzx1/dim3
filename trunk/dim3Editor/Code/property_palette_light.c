/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Property Palette Light

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

#define kLightPropertyOn					0
#define kLightPropertyLightMap				1
#define kLightPropertyName					2
#define kLightPropertyType					3
#define kLightPropertyDirection				4
#define kLightPropertyIntensity				5
#define kLightPropertyExponent				6
#define kLightPropertyColor					7
#define kLightPropertyHalo					8

extern map_type					map;
extern editor_state_type		state;
extern editor_setup_type		setup;

extern list_palette_type		property_palette;

char							light_property_type_list[][name_str_len]={"Normal","Blink","Glow","Pulse","Flicker","Failing",""},
								light_property_direction_list[][name_str_len]={"All","-X","+X","-Y","+Y","-Z","+Z",""};

/* =======================================================

      Property Palette Fill Light
      
======================================================= */

void property_palette_fill_light(int light_idx)
{
	map_light_type		*light;

	light=&map.lights[light_idx];

	list_palette_set_sub_title(&property_palette,"Light",light->name);

	list_palette_add_header(&property_palette,0,"Light Settings");
	list_palette_add_checkbox(&property_palette,kLightPropertyOn,"On",&light->setting.on,FALSE);
	list_palette_add_checkbox(&property_palette,kLightPropertyLightMap,"Used in Light Map",&light->setting.light_map,FALSE);

	list_palette_add_header(&property_palette,0,"Light Naming");
	list_palette_add_string(&property_palette,kLightPropertyName,"Name",light->name,FALSE);

	list_palette_add_header(&property_palette,0,"Light Display");
	list_palette_add_string(&property_palette,kLightPropertyType,"Type",light_property_type_list[light->setting.type],FALSE);
	list_palette_add_string(&property_palette,kLightPropertyDirection,"Direction",light_property_direction_list[light->setting.direction],FALSE);
	list_palette_add_int(&property_palette,kLightPropertyIntensity,"Intensity",&light->setting.intensity,FALSE);
	list_palette_add_float(&property_palette,kLightPropertyExponent,"Exponent",&light->setting.exponent,FALSE);
	list_palette_add_pick_color(&property_palette,kLightPropertyColor,"Color",&light->setting.col,FALSE);
	list_palette_add_string(&property_palette,kLightPropertyHalo,"Halo",light->setting.halo_name,FALSE);

	list_palette_add_header(&property_palette,0,"Light Info");
	list_palette_add_point(&property_palette,-1,"Position",&light->pnt,TRUE);
}

/* =======================================================

      Property Palette Click Light
      
======================================================= */

void property_palette_click_light(int light_idx,int id,bool double_click)
{
	map_light_type		*light;

	if (!double_click) return;

	light=&map.lights[light_idx];

	switch (id) {

		case kLightPropertyName:
			dialog_property_string_run(list_string_value_string,(void*)light->name,name_str_len,0,0);
			break;

		case kLightPropertyType:
			property_pick_list("Pick a Light Type",(char*)light_property_type_list,&light->setting.type);
			break;

		case kLightPropertyDirection:
			property_pick_list("Pick a Light Direction",(char*)light_property_direction_list,&light->setting.direction);
			break;

		case kLightPropertyHalo:
			property_palette_pick_halo(light->setting.halo_name);
			break;

	}
}

