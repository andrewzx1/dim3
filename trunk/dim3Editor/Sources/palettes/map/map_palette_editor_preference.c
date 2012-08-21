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

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
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
extern app_pref_type			pref;

extern list_palette_type		map_palette;

/* =======================================================

      Property Palette Fill Editor Preferences
      
======================================================= */

void map_palette_fill_editor_preference(void)
{
	list_palette_set_title(&map_palette,"Editor Preferences",NULL,NULL,NULL,NULL,NULL);

	list_palette_add_header(&map_palette,0,"Editor Engine Launch");
	list_palette_add_string(&map_palette,kPrefPropertyEngineName,"Engine Name",pref.map.engine_name,name_str_len,FALSE);

	list_palette_add_header(&map_palette,0,"Editor Settings");
	list_palette_add_checkbox(&map_palette,kPrefPropertyFreeLook,"Free Look",&pref.map.free_look,FALSE);
	list_palette_add_checkbox(&map_palette,kPrefPropertyAutoTexture,"Auto Texture",&pref.map.auto_texture,FALSE);
	list_palette_add_checkbox(&map_palette,kPrefPropertyBigTexture,"Large Texture Palette",&pref.map.big_texture,FALSE);
	list_palette_add_checkbox(&map_palette,kPrefPropertyShowTangentBinormal,"Show Tangent-Binormal",&pref.map.show_tangent_binormal,FALSE);

	list_palette_add_header(&map_palette,0,"Editor Options");
	list_palette_add_int(&map_palette,kPrefPropertyDuplicateOffset,"Duplicate Offset",&pref.map.duplicate_offset,FALSE);
	list_palette_add_int(&map_palette,kPrefPropertySnapSize,"Snap Size",&pref.map.snap_size,FALSE);
	list_palette_add_int(&map_palette,kPrefPropertyClipDistance,"Clip Distance",&pref.map.clip_distance,FALSE);

	list_palette_add_header(&map_palette,0,"Editor Movement");
	list_palette_add_checkbox(&map_palette,kPrefPropertyFlipHorzMovement,"Flip Horizontal Movement",&pref.map.flip_horz_movement,FALSE);
	list_palette_add_checkbox(&map_palette,kPrefPropertyFlipVertMovement,"Flip Vertical Movement",&pref.map.flip_vert_movement,FALSE);
	list_palette_add_checkbox(&map_palette,kPrefPropertyFlipHorzTurn,"Flip Horizontal Turn",&pref.map.flip_horz_turn,FALSE);
	list_palette_add_checkbox(&map_palette,kPrefPropertyFlipVertTurn,"Flip Vertical Turn",&pref.map.flip_vert_turn,FALSE);
	list_palette_add_checkbox(&map_palette,kPrefPropertyFlipForwardMovement,"Flip Forward Movement",&pref.map.flip_forward_movement,FALSE);

	list_palette_add_header(&map_palette,0,"Editor Colors");
	list_palette_add_pick_color(&map_palette,kPrefPropertyBackgroundColor,"Background",&pref.map.col.background,FALSE);
	list_palette_add_pick_color(&map_palette,kPrefPropertyLineColor,"Mesh Line",&pref.map.col.mesh_line,FALSE);
	list_palette_add_pick_color(&map_palette,kPrefPropertyMeshSelColor,"Mesh Select",&pref.map.col.mesh_sel,FALSE);
	list_palette_add_pick_color(&map_palette,kPrefPropertyPolySelColor,"Polygon Select",&pref.map.col.poly_sel,FALSE);
	list_palette_add_pick_color(&map_palette,kPrefPropertyPolyCullColor,"Culled Polygons",&pref.map.col.poly_cull,FALSE);
	
	list_palette_add_header(&map_palette,0,"Editor OBJ Import");
	list_palette_add_vector(&map_palette,kPrefPropertyImportNormalFactor,"Normal Factor",&pref.map.import_normal_factor,FALSE);
}

/* =======================================================

      Property Palette Click Editor Preference
      
======================================================= */

void map_palette_click_editor_preference(int id,bool double_click)
{
		// write prefs

	setup_xml_write();
}

