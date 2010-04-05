/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Polygon Palette

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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#include "interface.h"
#include "common_view.h"
#include "dialog.h"

#define kMeshPolySettingOffX					FOUR_CHAR_CODE('offx')
#define kMeshPolySettingOffY					FOUR_CHAR_CODE('offy')
#define kMeshPolySettingSizeX					FOUR_CHAR_CODE('sizx')
#define kMeshPolySettingSizeY					FOUR_CHAR_CODE('sizy')
#define kMeshPolySettingShiftX					FOUR_CHAR_CODE('sftx')
#define kMeshPolySettingShiftY					FOUR_CHAR_CODE('sfty')
#define kMeshPolySettingCamera					FOUR_CHAR_CODE('cnde')
#define kMeshPolySettingClimbable				FOUR_CHAR_CODE('fclb')
#define kMeshPolySettingNeverCull				FOUR_CHAR_CODE('ncul')

extern map_type				map;
extern editor_state_type	state;

WindowRef					palette_poly_wind;

/* =======================================================

      Palette Poly Data
      
======================================================= */

void palette_polygon_load(void)
{
	int						type,main_idx,poly_idx;
	float					x_txtoff,y_txtoff,x_txtfact,y_txtfact;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;
	map_liquid_type			*liq;
	
		// get polygon
		
	select_get(0,&type,&main_idx,&poly_idx);
	
	if (type==mesh_piece) {
		mesh=&map.mesh.meshes[main_idx];
		poly=&mesh->polys[poly_idx];

		map_mesh_get_poly_uv_as_box(&map,main_idx,poly_idx,(state.uv_layer==uv_layer_light_map),&x_txtoff,&y_txtoff,&x_txtfact,&y_txtfact);
		
		dialog_set_float(palette_poly_wind,kMeshPolySettingOffX,0,x_txtoff);
		dialog_set_float(palette_poly_wind,kMeshPolySettingOffY,0,y_txtoff);
		dialog_set_float(palette_poly_wind,kMeshPolySettingSizeX,0,x_txtfact);
		dialog_set_float(palette_poly_wind,kMeshPolySettingSizeY,0,y_txtfact);
		
		dialog_set_float(palette_poly_wind,kMeshPolySettingShiftX,0,poly->x_shift);
		dialog_set_float(palette_poly_wind,kMeshPolySettingShiftY,0,poly->y_shift);
		
		dialog_special_combo_fill_node(palette_poly_wind,kMeshPolySettingCamera,0,poly->camera);
		
		dialog_enable(palette_poly_wind,kMeshPolySettingClimbable,0,TRUE);
		dialog_set_boolean(palette_poly_wind,kMeshPolySettingClimbable,0,poly->climbable);
		
		dialog_set_boolean(palette_poly_wind,kMeshPolySettingNeverCull,0,poly->never_cull);
	}
	
		// liquid data
		
	else {
		liq=&map.liquid.liquids[main_idx];
		
		if (state.uv_layer==uv_layer_normal) {
			dialog_set_float(palette_poly_wind,kMeshPolySettingOffX,0,liq->main_uv.x_offset);
			dialog_set_float(palette_poly_wind,kMeshPolySettingOffY,0,liq->main_uv.y_offset);
			dialog_set_float(palette_poly_wind,kMeshPolySettingSizeX,0,liq->main_uv.x_size);
			dialog_set_float(palette_poly_wind,kMeshPolySettingSizeY,0,liq->main_uv.y_size);
		}
		else {
			dialog_set_float(palette_poly_wind,kMeshPolySettingOffX,0,liq->lmap_uv.x_offset);
			dialog_set_float(palette_poly_wind,kMeshPolySettingOffY,0,liq->lmap_uv.y_offset);
			dialog_set_float(palette_poly_wind,kMeshPolySettingSizeX,0,liq->lmap_uv.x_size);
			dialog_set_float(palette_poly_wind,kMeshPolySettingSizeY,0,liq->lmap_uv.y_size);
		}
		
		dialog_set_float(palette_poly_wind,kMeshPolySettingShiftX,0,liq->x_shift);
		dialog_set_float(palette_poly_wind,kMeshPolySettingShiftY,0,liq->y_shift);
		
		dialog_special_combo_fill_node(palette_poly_wind,kMeshPolySettingCamera,0,liq->camera);
		
		dialog_enable(palette_poly_wind,kMeshPolySettingClimbable,0,FALSE);
		dialog_set_boolean(palette_poly_wind,kMeshPolySettingClimbable,0,FALSE);
		
		dialog_set_boolean(palette_poly_wind,kMeshPolySettingNeverCull,0,liq->never_cull);
	}
	
	DrawControls(palette_poly_wind);
}

void palette_polygon_save(void)
{
	int						type,main_idx,poly_idx;
	float					x_txtoff,y_txtoff,x_txtfact,y_txtfact;
	map_mesh_poly_type		*poly;
	map_liquid_type			*liq;
	
		// get polygon
		
	select_get(0,&type,&main_idx,&poly_idx);
	
		// save data
		
	if (type==mesh_piece) {
		poly=&map.mesh.meshes[main_idx].polys[poly_idx];

		x_txtoff=dialog_get_float(palette_poly_wind,kMeshPolySettingOffX,0);
		y_txtoff=dialog_get_float(palette_poly_wind,kMeshPolySettingOffY,0);
		x_txtfact=dialog_get_float(palette_poly_wind,kMeshPolySettingSizeX,0);
		y_txtfact=dialog_get_float(palette_poly_wind,kMeshPolySettingSizeY,0);

		if ((x_txtfact>0.0f) && (y_txtfact>0.0f)) map_mesh_set_poly_uv_as_box(&map,main_idx,poly_idx,(state.uv_layer==uv_layer_light_map),x_txtoff,y_txtoff,x_txtfact,y_txtfact);

		poly->x_shift=dialog_get_float(palette_poly_wind,kMeshPolySettingShiftX,0);
		poly->y_shift=dialog_get_float(palette_poly_wind,kMeshPolySettingShiftY,0);
		
		dialog_special_combo_get_node(palette_poly_wind,kMeshPolySettingCamera,0,poly->camera,name_str_len);
		poly->climbable=dialog_get_boolean(palette_poly_wind,kMeshPolySettingClimbable,0);
		poly->never_cull=dialog_get_boolean(palette_poly_wind,kMeshPolySettingNeverCull,0);
	}
	
		// liquid data
		
	else {
		liq=&map.liquid.liquids[main_idx];

		if (state.uv_layer==uv_layer_normal) {
			liq->main_uv.x_offset=dialog_get_float(palette_poly_wind,kMeshPolySettingOffX,0);
			liq->main_uv.y_offset=dialog_get_float(palette_poly_wind,kMeshPolySettingOffY,0);
			liq->main_uv.x_size=dialog_get_float(palette_poly_wind,kMeshPolySettingSizeX,0);
			liq->main_uv.y_size=dialog_get_float(palette_poly_wind,kMeshPolySettingSizeY,0);
		}
		else {
			liq->lmap_uv.x_offset=dialog_get_float(palette_poly_wind,kMeshPolySettingOffX,0);
			liq->lmap_uv.y_offset=dialog_get_float(palette_poly_wind,kMeshPolySettingOffY,0);
			liq->lmap_uv.x_size=dialog_get_float(palette_poly_wind,kMeshPolySettingSizeX,0);
			liq->lmap_uv.y_size=dialog_get_float(palette_poly_wind,kMeshPolySettingSizeY,0);
		}
		
		liq->x_shift=dialog_get_float(palette_poly_wind,kMeshPolySettingShiftX,0);
		liq->y_shift=dialog_get_float(palette_poly_wind,kMeshPolySettingShiftY,0);
		
		dialog_special_combo_get_node(palette_poly_wind,kMeshPolySettingCamera,0,liq->camera,name_str_len);
		
		liq->never_cull=dialog_get_boolean(palette_poly_wind,kMeshPolySettingNeverCull,0);
	}
	
	main_wind_draw();
}

static pascal OSStatus palette_poly_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	palette_polygon_save();
	return(eventNotHandledErr);
}

/* =======================================================

      Palette Polygon Open/Close
      
======================================================= */

void palette_polygon_open(int x,int y)
{
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassControl,kEventControlHit},
										  {kEventClassKeyboard,kEventRawKeyUp}};

		// open the window
		
	dialog_open(&palette_poly_wind,"PolyPalette");
	MoveWindow(palette_poly_wind,x,y,FALSE);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(palette_poly_event_proc);
	InstallWindowEventHandler(palette_poly_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// numeric only controls

	dialog_set_numeric_only(palette_poly_wind,kMeshPolySettingOffX,0);
	dialog_set_numeric_only(palette_poly_wind,kMeshPolySettingOffY,0);
	dialog_set_numeric_only(palette_poly_wind,kMeshPolySettingSizeX,0);
	dialog_set_numeric_only(palette_poly_wind,kMeshPolySettingSizeY,0);
	dialog_set_numeric_only(palette_poly_wind,kMeshPolySettingShiftX,0);
	dialog_set_numeric_only(palette_poly_wind,kMeshPolySettingShiftY,0);

		// show palette
		
	ShowWindow(palette_poly_wind);
}

void palette_polygon_close(int *x,int *y)
{
	Rect			box;
	
	GetWindowBounds(palette_poly_wind,kWindowGlobalPortRgn,&box);
	*x=box.left;
	*y=box.top;

	DisposeWindow(palette_poly_wind);
}



