/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Alt Property Palette HUD Text

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
	#include "dim3setup.h"
#endif

#include "glue.h"
#include "ui_common.h"
#include "interface.h"

#define kModelSettingsName						0
#define kModelSettingsAnimate					1

#define kModelPositionX							2
#define kModelPositionY							3

#define kModelOptionResize						4
#define kModelOptionRot							5

extern iface_type				iface;
extern setup_state_type			state;
extern list_palette_type		alt_property_palette;

/* =======================================================

      Property Palette Fill HUD Text
      
======================================================= */

void alt_property_palette_fill_hud_text(int hud_text_idx)
{
	/*
	iface_intro_model_type				*model;

	model=&iface.intro.model_list.models[intro_model_idx];

		// settings

	list_palette_add_header(&alt_property_palette,0,"Settings");
	list_palette_add_string(&alt_property_palette,kModelSettingsName,"Model",model->model_name,FALSE);
	list_palette_add_string(&alt_property_palette,kModelSettingsAnimate,"Animation",model->animate_name,FALSE);

		// position

	list_palette_add_header(&alt_property_palette,0,"Position");
	list_palette_add_string_int(&alt_property_palette,kModelPositionX,"X",model->x,FALSE);
	list_palette_add_string_int(&alt_property_palette,kModelPositionY,"Y",model->y,FALSE);

		// options

	list_palette_add_header(&alt_property_palette,0,"Options");
	list_palette_add_string_float(&alt_property_palette,kModelOptionResize,"Resize",model->resize,FALSE);
	list_palette_add_angle(&alt_property_palette,kModelOptionRot,"Rot",&model->rot,FALSE);
	*/
}

/* =======================================================

      Property Palette Click HUD Text
      
======================================================= */

void alt_property_palette_click_hud_text(int hud_text_idx,int id)
{
	/*
	char								model_name[name_str_len];
	iface_intro_model_type				*model;

	model=&iface.intro.model_list.models[intro_model_idx];

	switch (id) {

			// settings

		case kModelSettingsName:
			strcpy(model_name,model->model_name);
			if (dialog_file_open_run("Pick a Model","Models",NULL,"Mesh.xml",model_name)) strcpy(model->model_name,model_name);
			break;

		case kModelSettingsAnimate:
			dialog_property_string_run(list_string_value_string,(void*)model->animate_name,name_str_len,0,0);
			break;

			// position

		case kModelPositionX:
			dialog_property_string_run(list_string_value_positive_int,(void*)&model->x,0,0,0);
			break;

		case kModelPositionY:
			dialog_property_string_run(list_string_value_positive_int,(void*)&model->y,0,0,0);
			break;

			// options

		case kModelOptionResize:
			dialog_property_string_run(list_string_value_positive_float,(void*)&model->resize,0,0,0);
			break;

		case kModelOptionRot:
			dialog_property_chord_run(list_chord_value_angle,(void*)&model->rot);
			break;

	}
*/
		// redraw

	main_wind_draw();
}

