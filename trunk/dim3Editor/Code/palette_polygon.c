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
#define kMeshPolySettingAlpha					FOUR_CHAR_CODE('alph')
#define kMeshPolySettingDark					FOUR_CHAR_CODE('dark')
#define kMeshPolySettingShiftX					FOUR_CHAR_CODE('sftx')
#define kMeshPolySettingShiftY					FOUR_CHAR_CODE('sfty')

extern map_type				map;

WindowRef					palette_poly_wind;

/* =======================================================

      Palette Poly Event Handlers
      
======================================================= */


/* =======================================================

      Run Poly Setting
      
======================================================= */
/*
bool dialog_poly_setting_run(void)
{
	int						type,main_idx,poly_idx;
	float					x_txtoff,y_txtoff,x_txtfact,y_txtfact;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;
	map_liquid_type			*liq;
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_poly_setting_wind,"PolySetting");
	
		// use first selected mesh as basis
		
	select_get(0,&type,&main_idx,&poly_idx);
	
		// polygon data
	
	if (type==mesh_piece) {
		mesh=&map.mesh.meshes[main_idx];
		poly=&mesh->polys[poly_idx];

		map_mesh_get_poly_uv_as_box(&map,main_idx,poly_idx,&x_txtoff,&y_txtoff,&x_txtfact,&y_txtfact);
		
		dialog_set_float(dialog_poly_setting_wind,kMeshPolySettingOffX,0,x_txtoff);
		dialog_set_float(dialog_poly_setting_wind,kMeshPolySettingOffY,0,y_txtoff);
		dialog_set_float(dialog_poly_setting_wind,kMeshPolySettingSizeX,0,x_txtfact);
		dialog_set_float(dialog_poly_setting_wind,kMeshPolySettingSizeY,0,y_txtfact);
		
		dialog_set_float(dialog_poly_setting_wind,kMeshPolySettingAlpha,0,poly->alpha);
		dialog_set_float(dialog_poly_setting_wind,kMeshPolySettingDark,0,poly->dark_factor);
		dialog_set_float(dialog_poly_setting_wind,kMeshPolySettingShiftX,0,poly->x_shift);
		dialog_set_float(dialog_poly_setting_wind,kMeshPolySettingShiftY,0,poly->y_shift);
	}
	
		// liquid data
		
	else {
		liq=&map.liquid.liquids[main_idx];
		
		dialog_enable(dialog_poly_setting_wind,kMeshPolySettingDark,0,FALSE);
		
		dialog_set_float(dialog_poly_setting_wind,kMeshPolySettingOffX,0,liq->x_txtoff);
		dialog_set_float(dialog_poly_setting_wind,kMeshPolySettingOffY,0,liq->y_txtoff);
		dialog_set_float(dialog_poly_setting_wind,kMeshPolySettingSizeX,0,liq->x_txtfact);
		dialog_set_float(dialog_poly_setting_wind,kMeshPolySettingSizeY,0,liq->y_txtfact);

		dialog_set_float(dialog_poly_setting_wind,kMeshPolySettingAlpha,0,liq->alpha);
		dialog_set_float(dialog_poly_setting_wind,kMeshPolySettingShiftX,0,liq->x_shift);
		dialog_set_float(dialog_poly_setting_wind,kMeshPolySettingShiftY,0,liq->y_shift);
	}
	
		// show window
	
	ShowWindow(dialog_poly_setting_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(poly_setting_event_proc);
	InstallWindowEventHandler(dialog_poly_setting_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_poly_setting_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_poly_setting_wind);
	
		// dialog to data
		
	if (!dialog_poly_setting_cancel) {
	
			// polygon data
			
		if (type==mesh_piece) {
			x_txtoff=dialog_get_float(dialog_poly_setting_wind,kMeshPolySettingOffX,0);
			y_txtoff=dialog_get_float(dialog_poly_setting_wind,kMeshPolySettingOffY,0);
			x_txtfact=dialog_get_float(dialog_poly_setting_wind,kMeshPolySettingSizeX,0);
			y_txtfact=dialog_get_float(dialog_poly_setting_wind,kMeshPolySettingSizeY,0);

			map_mesh_set_poly_uv_as_box(&map,main_idx,poly_idx,x_txtoff,y_txtoff,x_txtfact,y_txtfact);

			poly->dark_factor=dialog_get_float(dialog_poly_setting_wind,kMeshPolySettingDark,0);
			poly->alpha=dialog_get_float(dialog_poly_setting_wind,kMeshPolySettingAlpha,0);
			poly->x_shift=dialog_get_float(dialog_poly_setting_wind,kMeshPolySettingShiftX,0);
			poly->y_shift=dialog_get_float(dialog_poly_setting_wind,kMeshPolySettingShiftY,0);
		}
		
			// liquid data
			
		else {
			liq->x_txtoff=dialog_get_float(dialog_poly_setting_wind,kMeshPolySettingOffX,0);
			liq->y_txtoff=dialog_get_float(dialog_poly_setting_wind,kMeshPolySettingOffY,0);
			liq->x_txtfact=dialog_get_float(dialog_poly_setting_wind,kMeshPolySettingSizeX,0);
			liq->y_txtfact=dialog_get_float(dialog_poly_setting_wind,kMeshPolySettingSizeY,0);
			
			liq->alpha=dialog_get_float(dialog_poly_setting_wind,kMeshPolySettingAlpha,0);
			liq->x_shift=dialog_get_float(dialog_poly_setting_wind,kMeshPolySettingShiftX,0);
			liq->y_shift=dialog_get_float(dialog_poly_setting_wind,kMeshPolySettingShiftY,0);
		}
	}
	
		// close window
		
	DisposeWindow(dialog_poly_setting_wind);
	
	return(!dialog_poly_setting_cancel);
}
*/


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

		map_mesh_get_poly_uv_as_box(&map,main_idx,poly_idx,&x_txtoff,&y_txtoff,&x_txtfact,&y_txtfact);
		
		dialog_set_float(palette_poly_wind,kMeshPolySettingOffX,0,x_txtoff);
		dialog_set_float(palette_poly_wind,kMeshPolySettingOffY,0,y_txtoff);
		dialog_set_float(palette_poly_wind,kMeshPolySettingSizeX,0,x_txtfact);
		dialog_set_float(palette_poly_wind,kMeshPolySettingSizeY,0,y_txtfact);
		
		dialog_set_float(palette_poly_wind,kMeshPolySettingAlpha,0,poly->alpha);
		dialog_set_float(palette_poly_wind,kMeshPolySettingDark,0,poly->dark_factor);
		dialog_set_float(palette_poly_wind,kMeshPolySettingShiftX,0,poly->x_shift);
		dialog_set_float(palette_poly_wind,kMeshPolySettingShiftY,0,poly->y_shift);
	}
	
		// liquid data
		
	else {
		liq=&map.liquid.liquids[main_idx];
		
		dialog_enable(palette_poly_wind,kMeshPolySettingDark,0,FALSE);
		
		dialog_set_float(palette_poly_wind,kMeshPolySettingOffX,0,liq->x_txtoff);
		dialog_set_float(palette_poly_wind,kMeshPolySettingOffY,0,liq->y_txtoff);
		dialog_set_float(palette_poly_wind,kMeshPolySettingSizeX,0,liq->x_txtfact);
		dialog_set_float(palette_poly_wind,kMeshPolySettingSizeY,0,liq->y_txtfact);

		dialog_set_float(palette_poly_wind,kMeshPolySettingAlpha,0,liq->alpha);
		dialog_set_float(palette_poly_wind,kMeshPolySettingShiftX,0,liq->x_shift);
		dialog_set_float(palette_poly_wind,kMeshPolySettingShiftY,0,liq->y_shift);
	}
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
		x_txtoff=dialog_get_float(palette_poly_wind,kMeshPolySettingOffX,0);
		y_txtoff=dialog_get_float(palette_poly_wind,kMeshPolySettingOffY,0);
		x_txtfact=dialog_get_float(palette_poly_wind,kMeshPolySettingSizeX,0);
		y_txtfact=dialog_get_float(palette_poly_wind,kMeshPolySettingSizeY,0);

		if ((x_txtfact>0.0f) && (y_txtfact>0.0f)) map_mesh_set_poly_uv_as_box(&map,main_idx,poly_idx,x_txtoff,y_txtoff,x_txtfact,y_txtfact);

		poly->dark_factor=dialog_get_float(palette_poly_wind,kMeshPolySettingDark,0);
		poly->alpha=dialog_get_float(palette_poly_wind,kMeshPolySettingAlpha,0);
		poly->x_shift=dialog_get_float(palette_poly_wind,kMeshPolySettingShiftX,0);
		poly->y_shift=dialog_get_float(palette_poly_wind,kMeshPolySettingShiftY,0);
	}
	
		// liquid data
		
	else {
		liq->x_txtoff=dialog_get_float(palette_poly_wind,kMeshPolySettingOffX,0);
		liq->y_txtoff=dialog_get_float(palette_poly_wind,kMeshPolySettingOffY,0);
		liq->x_txtfact=dialog_get_float(palette_poly_wind,kMeshPolySettingSizeX,0);
		liq->y_txtfact=dialog_get_float(palette_poly_wind,kMeshPolySettingSizeY,0);
		
		liq->alpha=dialog_get_float(palette_poly_wind,kMeshPolySettingAlpha,0);
		liq->x_shift=dialog_get_float(palette_poly_wind,kMeshPolySettingShiftX,0);
		liq->y_shift=dialog_get_float(palette_poly_wind,kMeshPolySettingShiftY,0);
	}
	
	main_wind_draw();
}

static pascal OSStatus palette_poly_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	if ((GetEventClass(event)==kEventClassControl) || (GetEventClass(event)==kEventClassKeyboard)) palette_polygon_save();
	return(eventNotHandledErr);
}

void palette_polygon_open(int x,int y)
{
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassControl,kEventControlHit},
										  {kEventClassKeyboard,kEventRawKeyUp}};

		// open the window
		
	dialog_open(&palette_poly_wind,"PolyPalette");
	MoveWindow(palette_poly_wind,x,y,FALSE);

		// show palette
		
	ShowWindow(palette_poly_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(palette_poly_event_proc);
	InstallWindowEventHandler(palette_poly_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
}

void palette_polygon_close(void)
{
	DisposeWindow(palette_poly_wind);
}



