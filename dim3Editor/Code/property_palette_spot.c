/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Property Palette Spot

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

#define kSpotPropertyName						0
#define kSpotPropertyType						1
#define kSpotPropertyScript						2
#define kSpotPropertyScriptEdit					3
#define kSpotPropertySkill						4
#define kSpotPropertySpawn						5
#define kSpotPropertyDisplayModel				6

#define kSpotPropertyParamsStart				100
#define kSpotPropertyParamsEnd					109

extern map_type					map;
extern editor_state_type		state;
extern editor_setup_type		setup;

extern list_palette_type		property_palette;

char							spot_property_type_list[][name_str_len]={"Object","Bot","Player","Spawn",""},
								spot_property_skill_list[][name_str_len]={"Easy","Medium","Hard",""},
								spot_property_spawn_list[][name_str_len]={"Always","Single Player Only","Multiplayer Only",""};

/* =======================================================

      Property Palette Fill Spot
      
======================================================= */

void property_palette_fill_spot(int spot_idx)
{
	int				n;
	char			name[256],str[256];
	spot_type		*spot;

	spot=&map.spots[spot_idx];

	list_palette_add_header(&property_palette,0,"Spot Settings");
	list_palette_add_string(&property_palette,kSpotPropertyName,"Name",spot->name,FALSE);
	list_palette_add_string(&property_palette,kSpotPropertyType,"Type",spot_property_type_list[spot->type],FALSE);
	list_palette_add_string_edit(&property_palette,kSpotPropertyScript,kSpotPropertyScriptEdit,"Script",spot->script,FALSE);
	list_palette_add_string(&property_palette,kSpotPropertySkill,"Skill",spot_property_skill_list[spot->skill],FALSE);
	list_palette_add_string(&property_palette,kSpotPropertySpawn,"Spawn",spot_property_spawn_list[spot->spawn],FALSE);
	list_palette_add_string(&property_palette,kSpotPropertyDisplayModel,"Model",spot->display_model,FALSE);

	list_palette_add_header(&property_palette,0,"Spot Parameters");
	for (n=0;n!=10;n++) {
		sprintf(name,"Param %d",n);
		property_get_parameter(n,spot->params,str);
		list_palette_add_string(&property_palette,(kSpotPropertyParamsStart+n),name,str,FALSE);
	}
	
	list_palette_add_header(&property_palette,0,"Spot Info");
	list_palette_add_point(&property_palette,-1,"Position",&spot->pnt,TRUE);
	list_palette_add_angle(&property_palette,-1,"Angle",&spot->ang,TRUE);
}

/* =======================================================

      Property Palette Click Spot
      
======================================================= */

void property_palette_click_spot(int spot_idx,int id)
{
	int				param_idx;
	char			file_name[256],str[256];
	spot_type		*spot;

	spot=&map.spots[spot_idx];

		// parameters

	if ((id>=kSpotPropertyParamsStart) && (id<=kSpotPropertyParamsEnd)) {
		param_idx=(id-kSpotPropertyParamsStart);
		
		property_get_parameter(param_idx,spot->params,str);
		dialog_property_string_run(list_string_value_string,(void*)str,256,0,0);
		property_set_parameter(param_idx,spot->params,str);

		main_wind_draw();
		return;
	}

		// regular properties

	switch (id) {

		case kSpotPropertyName:
			dialog_property_string_run(list_string_value_string,(void*)spot->name,name_str_len,0,0);
			break;

		case kSpotPropertyType:
			property_pick_list((char*)spot_property_type_list,&spot->type);
			break;

		case kSpotPropertyScript:
			strcpy(file_name,spot->script);
			if (dialog_file_open_run("Pick a Script","Scripts/Objects","js",NULL,file_name)) {
				strncpy(spot->script,file_name,file_str_len);
				spot->script[file_str_len-1]=0x0;
			}
			break;

		case kSpotPropertyScriptEdit:
			launch_spot_script_editor(spot);
			break;

		case kSpotPropertySkill:
			property_pick_list((char*)spot_property_skill_list,&spot->skill);
			break;

		case kSpotPropertySpawn:
			property_pick_list((char*)spot_property_spawn_list,&spot->spawn);
			break;

		case kSpotPropertyDisplayModel:
			strcpy(file_name,spot->display_model);
			if (dialog_file_open_run("Pick a Model","Models",NULL,"Mesh.xml",file_name)) {
				strncpy(spot->display_model,file_name,name_str_len);
				spot->script[name_str_len-1]=0x0;
				view_models_reset();		// need to reset view models
			}
			break;

	}

	main_wind_draw();
}

