/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Property Palette Editor Preferences

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
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "ui_common.h"
#include "interface.h"

#define kPrefPropertyEngineName				0
#define kPrefPropertyMipMapMode				1
#define kPrefPropertyFreeLook				2
#define kPrefPropertyAutoTexture			3
#define kPrefPropertyBigTexture				4
#define kPrefPropertyShowTangentBinormal	5
#define kPrefPropertyDuplicateOffset		6
#define kPrefPropertySnapSize				7
#define kPrefPropertyClipDistance			8
#define kPrefPropertyFlipHorzMovement		9
#define kPrefPropertyFlipVertMovement		10
#define kPrefPropertyFlipHorzTurn			11
#define kPrefPropertyFlipVertTurn			12
#define kPrefPropertyFlipForwardMovement	13
#define kPrefPropertyBackgroundColor		14
#define kPrefPropertyLineColor				15
#define kPrefPropertyMeshSelColor			16
#define kPrefPropertyPolySelColor			17
#define kPrefPropertyPolyCullColor			18
#define kPrefPropertyImportNormalFactor		19

extern map_type					map;
extern editor_state_type		state;
extern editor_setup_type		setup;

extern list_palette_type		property_palette;

char							pref_mipmap_type_str[][32]=mipmap_mode_setup_list_def;

/* =======================================================

      Property Palette Fill Editor Preferences
      
======================================================= */

void property_palette_fill_editor_preference(void)
{
	list_palette_set_title(&property_palette,"Editor Preferences");

	list_palette_add_header(&property_palette,0,"Editor Engine Launch");
	list_palette_add_string(&property_palette,kPrefPropertyEngineName,"Engine Name",setup.engine_name,FALSE);

	list_palette_add_header(&property_palette,0,"Editor Settings");
	list_palette_add_checkbox(&property_palette,kPrefPropertyFreeLook,"Free Look",&setup.free_look,FALSE);
	list_palette_add_checkbox(&property_palette,kPrefPropertyAutoTexture,"Auto Texture",&setup.auto_texture,FALSE);
	list_palette_add_checkbox(&property_palette,kPrefPropertyBigTexture,"Large Texture Palette",&setup.big_texture,FALSE);
	list_palette_add_checkbox(&property_palette,kPrefPropertyShowTangentBinormal,"Show Tangent-Binormal",&setup.show_tangent_binormal,FALSE);

	list_palette_add_header(&property_palette,0,"Editor Options");
	list_palette_add_picker_list_int(&property_palette,kPrefPropertyMipMapMode,"Mipmap Mode",(char*)pref_mipmap_type_str,-1,name_str_len,0,FALSE,&setup.mipmap_mode,FALSE);
	list_palette_add_int(&property_palette,kPrefPropertyDuplicateOffset,"Duplicate Offset",&setup.duplicate_offset,FALSE);
	list_palette_add_int(&property_palette,kPrefPropertySnapSize,"Snap Size",&setup.snap_size,FALSE);
	list_palette_add_int(&property_palette,kPrefPropertyClipDistance,"Clip Distance",&setup.clip_distance,FALSE);

	list_palette_add_header(&property_palette,0,"Editor Movement");
	list_palette_add_checkbox(&property_palette,kPrefPropertyFlipHorzMovement,"Flip Horizontal Movement",&setup.flip_horz_movement,FALSE);
	list_palette_add_checkbox(&property_palette,kPrefPropertyFlipVertMovement,"Flip Vertical Movement",&setup.flip_vert_movement,FALSE);
	list_palette_add_checkbox(&property_palette,kPrefPropertyFlipHorzTurn,"Flip Horizontal Turn",&setup.flip_horz_turn,FALSE);
	list_palette_add_checkbox(&property_palette,kPrefPropertyFlipVertTurn,"Flip Vertical Turn",&setup.flip_vert_turn,FALSE);
	list_palette_add_checkbox(&property_palette,kPrefPropertyFlipForwardMovement,"Flip Forward Movement",&setup.flip_forward_movement,FALSE);

	list_palette_add_header(&property_palette,0,"Editor Colors");
	list_palette_add_pick_color(&property_palette,kPrefPropertyBackgroundColor,"Background",&setup.col.background,FALSE);
	list_palette_add_pick_color(&property_palette,kPrefPropertyLineColor,"Mesh Line",&setup.col.mesh_line,FALSE);
	list_palette_add_pick_color(&property_palette,kPrefPropertyMeshSelColor,"Mesh Select",&setup.col.mesh_sel,FALSE);
	list_palette_add_pick_color(&property_palette,kPrefPropertyPolySelColor,"Polygon Select",&setup.col.poly_sel,FALSE);
	list_palette_add_pick_color(&property_palette,kPrefPropertyPolyCullColor,"Culled Polygons",&setup.col.poly_cull,FALSE);
	
	list_palette_add_header(&property_palette,0,"Editor OBJ Import");
	list_palette_add_vector(&property_palette,kPrefPropertyImportNormalFactor,"Normal Factor",&setup.import_normal_factor,FALSE);
}

/* =======================================================

      Property Palette Click Editor Preference
      
======================================================= */

void property_palette_click_editor_preference(int id,bool double_click)
{
	if (double_click) {

		switch (id) {

			case kPrefPropertyEngineName:
				dialog_property_string_run(list_string_value_string,(void*)setup.engine_name,name_str_len,0,0);
				break;

		}
	}

		// write prefs

	setup_xml_write();
}

