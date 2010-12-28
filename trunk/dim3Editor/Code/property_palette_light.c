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
#include "interface.h"
#include "dialog.h"

#define kLightPropertyOn					0
#define kLightPropertyLightMap				1
#define kLightPropertyName					2
#define kLightPropertyType					3
#define kLightPropertyDirection				4
#define kLightPropertyIntensity				5
#define kLightPropertyExponent				6
#define kLightPropertyColor					7

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

	list_palette_add_header(&property_palette,0,"Light Settings");
	list_palette_add_checkbox(&property_palette,kLightPropertyOn,"On",light->on,FALSE);
	list_palette_add_checkbox(&property_palette,kLightPropertyLightMap,"Used in Light Map",light->light_map,FALSE);

	list_palette_add_header(&property_palette,0,"Light Naming");
	list_palette_add_string(&property_palette,kLightPropertyName,"Name",light->name,FALSE);

	list_palette_add_header(&property_palette,0,"Light Display");
	list_palette_add_string(&property_palette,kLightPropertyType,"Type",light_property_type_list[light->type],FALSE);
	list_palette_add_string(&property_palette,kLightPropertyDirection,"Direction",light_property_direction_list[light->direction],FALSE);
	list_palette_add_string_int(&property_palette,kLightPropertyIntensity,"Intensity",light->intensity,FALSE);
	list_palette_add_string_float(&property_palette,kLightPropertyExponent,"Exponent",light->exponent,FALSE);
	list_palette_add_pick_color(&property_palette,kLightPropertyColor,"Color",&light->col,FALSE);

	list_palette_add_header(&property_palette,0,"Light Info");
	list_palette_add_point(&property_palette,-1,"Position",&light->pnt,TRUE);
}

/* =======================================================

      Property Palette Click Light
      
======================================================= */

void property_palette_click_light(int light_idx,int id)
{
	map_light_type		*light;

	light=&map.lights[light_idx];

	switch (id) {

		case kLightPropertyOn:
			light->on=!light->on;
			break;

		case kLightPropertyLightMap:
			light->light_map=!light->light_map;
			break;

		case kLightPropertyName:
			dialog_property_string_run(list_string_value_string,(void*)light->name,name_str_len,0,0);
			break;

		case kLightPropertyType:
			property_palette_pick_list((char*)light_property_type_list,&light->type);
			break;

		case kLightPropertyDirection:
			property_palette_pick_list((char*)light_property_direction_list,&light->direction);
			break;

		case kLightPropertyIntensity:
			dialog_property_string_run(list_string_value_positive_int,(void*)&light->intensity,0,0,0);
			break;
			
		case kLightPropertyExponent:
			dialog_property_string_run(list_string_value_positive_float,(void*)&light->exponent,0,0,0);
			break;
			
		case kLightPropertyColor:
			os_pick_color(&light->col);
			break;

	}

	main_wind_draw();
}

