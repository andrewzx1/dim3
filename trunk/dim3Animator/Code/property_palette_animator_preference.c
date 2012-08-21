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

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
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
extern file_path_setup_type		file_path_setup;
extern app_pref_type			pref;

extern list_palette_type		model_palette;

/* =======================================================

      Property Palette Fill Animator Preferences
      
======================================================= */

void model_palette_fill_animator_preference(void)
{
	list_palette_set_title(&model_palette,"Animator Preferences",NULL,NULL,NULL,NULL,NULL);

	list_palette_add_header(&model_palette,0,"Animator Settings");
	list_palette_add_checkbox(&model_palette,kPrefPropertyShowTangentBinormal,"Show Tangent-Binormal",&pref.model.show_tangent_binormal,FALSE);

	list_palette_add_header(&model_palette,0,"Animator Colors");
	list_palette_add_pick_color(&model_palette,kPrefPropertyBackgroundColor,"Background",&pref.model.col.background,FALSE);
	list_palette_add_pick_color(&model_palette,kPrefPropertyMeshLineColor,"Mesh Line",&pref.model.col.mesh_line,FALSE);
	list_palette_add_pick_color(&model_palette,kPrefPropertyMeshSelectColor,"Mesh Select",&pref.model.col.mesh_sel,FALSE);
}

/* =======================================================

      Property Palette Click Animator Preference
      
======================================================= */

void model_palette_click_animator_preference(int id,bool double_click)
{
	setup_xml_write();
}

