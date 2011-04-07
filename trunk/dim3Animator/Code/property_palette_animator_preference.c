/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Property Palette Animator Preferences

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
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
	#include "dim3animator.h"
#endif

#include "glue.h"
#include "ui_common.h"
#include "interface.h"

#define kPrefPropertyShowTangentBinormal	0

#define kPrefPropertyBackgroundColor		1
#define kPrefPropertyMeshLineColor			2
#define kPrefPropertyMeshSelectColor		3

extern model_type				model;
extern animator_state_type		state;
extern file_path_setup_type		file_path_setup;
extern animator_setup_type		setup;

extern list_palette_type		property_palette;

/* =======================================================

      Property Palette Fill Animator Preferences
      
======================================================= */

void property_palette_fill_animator_preference(void)
{
	list_palette_add_header(&property_palette,0,"Animator Settings");
	list_palette_add_checkbox(&property_palette,kPrefPropertyShowTangentBinormal,"Show Tangent-Binormal",setup.show_tangent_binormal,FALSE);

	list_palette_add_header(&property_palette,0,"Animator Colors");
	list_palette_add_pick_color(&property_palette,kPrefPropertyBackgroundColor,"Background",&setup.col.background,FALSE);
	list_palette_add_pick_color(&property_palette,kPrefPropertyMeshLineColor,"Mesh Line",&setup.col.mesh_line,FALSE);
	list_palette_add_pick_color(&property_palette,kPrefPropertyMeshSelectColor,"Mesh Select",&setup.col.mesh_sel,FALSE);
}

/* =======================================================

      Property Palette Click Animator Preference
      
======================================================= */

void property_palette_click_animator_preference(int id)
{
	switch (id) {

		case kPrefPropertyShowTangentBinormal:
			setup.show_tangent_binormal=!setup.show_tangent_binormal;
			break;

		case kPrefPropertyBackgroundColor:
			os_pick_color(&setup.col.background);
			break;

		case kPrefPropertyMeshLineColor:
			os_pick_color(&setup.col.mesh_line);
			break;

		case kPrefPropertyMeshSelectColor:
			os_pick_color(&setup.col.mesh_sel);
			break;

	}

		// write prefs

	setup_xml_write();

		// need to reset palettes
		// as prefs can change the sizes

	texture_palette_setup();
	item_palette_setup();
	property_palette_setup();
	alt_property_palette_setup();

	main_wind_draw();
}

