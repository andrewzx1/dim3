/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Intro Model

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

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
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
extern list_palette_type		property_palette;

/* =======================================================

      Property Palette Fill Intro Model
      
======================================================= */

void property_palette_fill_intro_model(int intro_model_idx)
{
	iface_intro_model_type				*model;

	model=&iface.intro.model_list.models[intro_model_idx];

	list_palette_set_title(&property_palette,"Intro",NULL,"Model",model->model_name,NULL,NULL);

		// settings

	list_palette_add_header(&property_palette,0,"Settings");
	list_palette_add_picker_file(&property_palette,kModelSettingsName,list_button_none,0,"Model","Models","","Mesh.xml;Model.xml",model->model_name,FALSE);
	list_palette_add_string(&property_palette,kModelSettingsAnimate,"Animation",model->animate_name,FALSE);

		// position

	list_palette_add_header(&property_palette,0,"Position");
	list_palette_add_int(&property_palette,kModelPositionX,"X",&model->x,FALSE);
	list_palette_add_int(&property_palette,kModelPositionY,"Y",&model->y,FALSE);

		// options

	list_palette_add_header(&property_palette,0,"Options");
	list_palette_add_float(&property_palette,kModelOptionResize,"Resize",&model->resize,FALSE);
	list_palette_add_angle(&property_palette,kModelOptionRot,"Rotate",&model->rot,FALSE);
}

/* =======================================================

      Property Palette Click Intro Model
      
======================================================= */

void property_palette_click_intro_model(int intro_model_idx,bool double_click)
{
	iface_intro_model_type				*model;

	if (!double_click) return;

	model=&iface.intro.model_list.models[intro_model_idx];

	switch (property_palette.item_pane.click.id) {

			// settings

		case kModelSettingsAnimate:
			dialog_property_string_run(list_string_value_string,(void*)model->animate_name,name_str_len,0,0);
			break;

	}
}

