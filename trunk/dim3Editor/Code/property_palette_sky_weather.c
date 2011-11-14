/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Property Palette Map Sky Weather

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

#define kMapPropertySkyOn					1
#define kMapPropertySkyType					2
#define kMapPropertySkyRadius				3
#define kMapPropertyDomeY					4
#define kMapPropertyDomeMirror				5
#define kMapPropertyTextureRepeat			6
#define kMapPropertyTextureShift			7
#define kMapPropertyTextureFill				8
#define kMapPropertyTextureBottomFill		9
#define kMapPropertyTextureNorthFill		10
#define kMapPropertyTextureSouthFill		11
#define kMapPropertyTextureEastFill			12
#define kMapPropertyTextureWestFill			13

#define kMapPropertyBackgroundOn			20
#define kMapPropertyBackgroundFrontFill		21
#define kMapPropertyBackgroundFrontStamp	22
#define kMapPropertyBackgroundFrontShift	23
#define kMapPropertyBackgroundMiddleFill	24
#define kMapPropertyBackgroundMiddleStamp	25
#define kMapPropertyBackgroundMiddleShift	26
#define kMapPropertyBackgroundBackFill		27
#define kMapPropertyBackgroundBackStamp		28
#define kMapPropertyBackgroundBackShift		29

#define kMapPropertyFogOn					30
#define kMapPropertyFogInnerRadius			31
#define kMapPropertyFogOuterRadius			32
#define kMapPropertyFogUseSolidColor		33
#define kMapPropertyFogColor				34
#define kMapPropertyFogTextureIndex			35
#define kMapPropertyFogCount				36
#define kMapPropertyFogHigh					37
#define kMapPropertyFogDrop					38
#define kMapPropertyFogAlpha				39
#define kMapPropertyFogTextureSpeed			40
#define kMapPropertyFogTextureFact			41

#define kMapPropertyRainOn					50
#define kMapPropertyRainDensity				51
#define kMapPropertyRainRadius				52
#define kMapPropertyRainHeight				53
#define kMapPropertyRainSpeed				54
#define kMapPropertyRainWidth				55
#define kMapPropertyRainLength				56
#define kMapPropertyRainAlpha				57
#define kMapPropertyRainStartColor			58
#define kMapPropertyRainEndColor			59
#define kMapPropertyRainSlantAdd			60
#define kMapPropertyRainSlantMaxTime		61
#define kMapPropertyRainSlantChangeTime		62

extern map_type					map;
extern editor_state_type		state;
extern editor_setup_type		setup;

extern list_palette_type		property_palette;

char							map_property_sky_type_list[][name_str_len]={"Panoramic","Dome Hemisphere","Cube",""};

/* =======================================================

      Property Palette Fill Map Sky Weather
      
======================================================= */

void property_palette_fill_sky_weather(void)
{
	d3fpnt			uv;

	list_palette_set_title(&property_palette,"Map Sky & Weather");

		// sky

	list_palette_add_header(&property_palette,0,"Map Sky");
	list_palette_add_checkbox(&property_palette,kMapPropertySkyOn,"On",&map.sky.on,FALSE);
	list_palette_add_string(&property_palette,kMapPropertySkyType,"Type",map_property_sky_type_list[map.sky.type],FALSE);
	list_palette_add_int(&property_palette,kMapPropertySkyRadius,"Radius",&map.sky.radius,FALSE);
	list_palette_add_int(&property_palette,kMapPropertyDomeY,"Dome Height",&map.sky.dome_y,FALSE);
	list_palette_add_checkbox(&property_palette,kMapPropertyDomeMirror,"Mirror Dome",&map.sky.dome_mirror,FALSE);
	list_palette_add_float(&property_palette,kMapPropertyTextureRepeat,"Texture Repeat",&map.sky.txt_fact,FALSE);
	uv.x=map.sky.txt_x_shift;
	uv.y=map.sky.txt_y_shift;
	list_palette_add_uv(&property_palette,kMapPropertyTextureShift,"Texture Shift",&uv,FALSE);
	list_palette_add_texture(&property_palette,map.textures,kMapPropertyTextureFill,"Fill/Cube Top Fill",map.sky.fill,FALSE);
	list_palette_add_texture(&property_palette,map.textures,kMapPropertyTextureBottomFill,"Cube Bottom Fill",map.sky.bottom_fill,FALSE);
	list_palette_add_texture(&property_palette,map.textures,kMapPropertyTextureNorthFill,"Cube North Fill",map.sky.north_fill,FALSE);
	list_palette_add_texture(&property_palette,map.textures,kMapPropertyTextureSouthFill,"Cube South Fill",map.sky.south_fill,FALSE);
	list_palette_add_texture(&property_palette,map.textures,kMapPropertyTextureEastFill,"Cube East Fill",map.sky.east_fill,FALSE);
	list_palette_add_texture(&property_palette,map.textures,kMapPropertyTextureWestFill,"Cube West Fill",map.sky.west_fill,FALSE);

		// background

	list_palette_add_header(&property_palette,0,"Map Background");
	list_palette_add_checkbox(&property_palette,kMapPropertyBackgroundOn,"On",&map.background.on,FALSE);
	uv.x=map.background.front.x_fact;
	uv.y=map.background.front.y_fact;
	list_palette_add_uv(&property_palette,kMapPropertyBackgroundFrontStamp,"Front Stamp",&uv,FALSE);
	uv.x=map.background.front.x_scroll_fact;
	uv.y=map.background.front.y_scroll_fact;
	list_palette_add_uv(&property_palette,kMapPropertyBackgroundFrontShift,"Front Scroll",&uv,FALSE);
	list_palette_add_texture(&property_palette,map.textures,kMapPropertyBackgroundFrontFill,"Front Fill",map.background.front.fill,FALSE);
	uv.x=map.background.middle.x_fact;
	uv.y=map.background.middle.y_fact;
	list_palette_add_uv(&property_palette,kMapPropertyBackgroundMiddleStamp,"Middle Stamp",&uv,FALSE);
	uv.x=map.background.middle.x_scroll_fact;
	uv.y=map.background.middle.y_scroll_fact;
	list_palette_add_uv(&property_palette,kMapPropertyBackgroundMiddleShift,"Middle Scroll",&uv,FALSE);
	list_palette_add_texture(&property_palette,map.textures,kMapPropertyBackgroundMiddleFill,"Middle Fill",map.background.middle.fill,FALSE);
	uv.x=map.background.back.x_fact;
	uv.y=map.background.back.y_fact;
	list_palette_add_uv(&property_palette,kMapPropertyBackgroundBackStamp,"Back Stamp",&uv,FALSE);
	uv.x=map.background.back.x_scroll_fact;
	uv.y=map.background.back.y_scroll_fact;
	list_palette_add_uv(&property_palette,kMapPropertyBackgroundBackShift,"Back Scroll",&uv,FALSE);
	list_palette_add_texture(&property_palette,map.textures,kMapPropertyBackgroundBackFill,"Back Fill",map.background.back.fill,FALSE);

		// fog general

	list_palette_add_header(&property_palette,0,"Map Fog General");
	list_palette_add_checkbox(&property_palette,kMapPropertyFogOn,"On",&map.fog.on,FALSE);
	list_palette_add_int(&property_palette,kMapPropertyFogInnerRadius,"Inner Radius",&map.fog.inner_radius,FALSE);
	list_palette_add_int(&property_palette,kMapPropertyFogOuterRadius,"Outer Radius",&map.fog.outer_radius,FALSE);

		// fog obscure

	list_palette_add_header(&property_palette,0,"Map Fog Obscure");
	list_palette_add_checkbox(&property_palette,kMapPropertyFogUseSolidColor,"Use Obscure (GL) Type",&map.fog.use_solid_color,FALSE);
	list_palette_add_pick_color(&property_palette,kMapPropertyFogColor,"Color",&map.fog.col,FALSE);

		// fog textured

	list_palette_add_header(&property_palette,0,"Map Fog Textured");
	list_palette_add_texture(&property_palette,map.textures,kMapPropertyFogTextureIndex,"Fill",map.fog.texture_idx,FALSE);
	list_palette_add_int(&property_palette,kMapPropertyFogCount,"Layer Count",&map.fog.count,FALSE);
	list_palette_add_int(&property_palette,kMapPropertyFogHigh,"Height",&map.fog.high,FALSE);
	list_palette_add_int(&property_palette,kMapPropertyFogDrop,"Y Drop",&map.fog.drop,FALSE);
	list_palette_add_float(&property_palette,kMapPropertyFogAlpha,"Alpha",&map.fog.alpha,FALSE);
	list_palette_add_float(&property_palette,kMapPropertyFogTextureSpeed,"Flow Speed",&map.fog.speed,FALSE);
	uv.x=map.fog.txt_x_fact;
	uv.y=map.fog.txt_y_fact;
	list_palette_add_uv(&property_palette,kMapPropertyFogTextureFact,"Texture Stamp",&uv,FALSE);

		// rain

	list_palette_add_header(&property_palette,0,"Map Rain General");
	list_palette_add_checkbox(&property_palette,kMapPropertyRainOn,"On",&map.rain.on,FALSE);
	list_palette_add_int(&property_palette,kMapPropertyRainDensity,"Density",&map.rain.density,FALSE);
	list_palette_add_int(&property_palette,kMapPropertyRainRadius,"Radius",&map.rain.radius,FALSE);
	list_palette_add_int(&property_palette,kMapPropertyRainHeight,"Height",&map.rain.height,FALSE);
	list_palette_add_int(&property_palette,kMapPropertyRainSpeed,"Speed",&map.rain.speed,FALSE);

	list_palette_add_header(&property_palette,0,"Map Rain Look");
	list_palette_add_int(&property_palette,kMapPropertyRainWidth,"Line Width",&map.rain.line_width,FALSE);
	list_palette_add_int(&property_palette,kMapPropertyRainLength,"Line Length",&map.rain.line_length,FALSE);
	list_palette_add_float(&property_palette,kMapPropertyRainAlpha,"Line Alpha",&map.rain.alpha,FALSE);
	list_palette_add_pick_color(&property_palette,kMapPropertyRainStartColor,"Line Start Color",&map.rain.start_color,FALSE);
	list_palette_add_pick_color(&property_palette,kMapPropertyRainEndColor,"Line End Color",&map.rain.end_color,FALSE);
	list_palette_add_int(&property_palette,kMapPropertyRainSlantAdd,"Slant Add",&map.rain.slant_add,FALSE);
	list_palette_add_int(&property_palette,kMapPropertyRainSlantMaxTime,"Slant Max Time",&map.rain.slant_time_msec,FALSE);
	list_palette_add_int(&property_palette,kMapPropertyRainSlantChangeTime,"Slant Change Time",&map.rain.slant_change_msec,FALSE);
}

/* =======================================================

      Property Palette Click Map Sky Weather
      
======================================================= */

void property_palette_click_sky_weather(int id,bool double_click)
{
	d3fpnt			uv;

	if (!double_click) return;

	switch (id) {

			// map sky

		case kMapPropertySkyType:
			property_pick_list("Pick a Sky Type",(char*)map_property_sky_type_list,&map.sky.type);
			break;

		case kMapPropertyTextureShift:
			uv.x=map.sky.txt_x_shift;
			uv.y=map.sky.txt_y_shift;
			dialog_property_chord_run(list_chord_value_uv,(void*)&uv);
			map.sky.txt_x_shift=uv.x;
			map.sky.txt_y_shift=uv.y;
			break;

		case kMapPropertyTextureFill:
			property_palette_pick_texture(NULL,&map.sky.fill);
			break;

		case kMapPropertyTextureBottomFill:
			property_palette_pick_texture(NULL,&map.sky.bottom_fill);
			break;

		case kMapPropertyTextureNorthFill:
			property_palette_pick_texture(NULL,&map.sky.north_fill);
			break;

		case kMapPropertyTextureSouthFill:
			property_palette_pick_texture(NULL,&map.sky.south_fill);
			break;

		case kMapPropertyTextureEastFill:
			property_palette_pick_texture(NULL,&map.sky.east_fill);
			break;

		case kMapPropertyTextureWestFill:
			property_palette_pick_texture(NULL,&map.sky.west_fill);
			break;

			// map background

		case kMapPropertyBackgroundFrontStamp:
			uv.x=map.background.front.x_fact;
			uv.y=map.background.front.y_fact;
			dialog_property_chord_run(list_chord_value_uv,(void*)&uv);
			map.background.front.x_fact=uv.x;
			map.background.front.y_fact=uv.y;
			break;

		case kMapPropertyBackgroundFrontShift:
			uv.x=map.background.front.x_scroll_fact;
			uv.y=map.background.front.y_scroll_fact;
			dialog_property_chord_run(list_chord_value_uv,(void*)&uv);
			map.background.front.x_scroll_fact=uv.x;
			map.background.front.y_scroll_fact=uv.y;
			break;

		case kMapPropertyBackgroundFrontFill:
			property_palette_pick_texture(NULL,&map.background.front.fill);
			break;

		case kMapPropertyBackgroundMiddleStamp:
			uv.x=map.background.middle.x_fact;
			uv.y=map.background.middle.y_fact;
			dialog_property_chord_run(list_chord_value_uv,(void*)&uv);
			map.background.middle.x_fact=uv.x;
			map.background.middle.y_fact=uv.y;
			break;

		case kMapPropertyBackgroundMiddleShift:
			uv.x=map.background.middle.x_scroll_fact;
			uv.y=map.background.middle.y_scroll_fact;
			dialog_property_chord_run(list_chord_value_uv,(void*)&uv);
			map.background.middle.x_scroll_fact=uv.x;
			map.background.middle.y_scroll_fact=uv.y;
			break;

		case kMapPropertyBackgroundMiddleFill:
			property_palette_pick_texture(NULL,&map.background.middle.fill);
			break;

		case kMapPropertyBackgroundBackStamp:
			uv.x=map.background.back.x_fact;
			uv.y=map.background.back.y_fact;
			dialog_property_chord_run(list_chord_value_uv,(void*)&uv);
			map.background.back.x_fact=uv.x;
			map.background.back.y_fact=uv.y;
			break;

		case kMapPropertyBackgroundBackShift:
			uv.x=map.background.back.x_scroll_fact;
			uv.y=map.background.back.y_scroll_fact;
			dialog_property_chord_run(list_chord_value_uv,(void*)&uv);
			map.background.back.x_scroll_fact=uv.x;
			map.background.back.y_scroll_fact=uv.y;
			break;

		case kMapPropertyBackgroundBackFill:
			property_palette_pick_texture(NULL,&map.background.back.fill);
			break;

			// fog textured

		case kMapPropertyFogTextureIndex:
			property_palette_pick_texture(NULL,&map.fog.texture_idx);
			break;

		case kMapPropertyFogTextureFact:
			uv.x=map.fog.txt_x_fact;
			uv.y=map.fog.txt_y_fact;
			dialog_property_chord_run(list_chord_value_uv,(void*)&uv);
			map.fog.txt_x_fact=uv.x;
			map.fog.txt_y_fact=uv.y;
			break;


	}
}

