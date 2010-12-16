/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Property Palette Liquid

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
#include "interface.h"
#include "view.h"
#include "dialog.h"

#define kLiquidPropertyWaveFlat				0
#define kLiquidPropertyNeverObscure			1
#define kLiquidPropertyNeverCull			2
#define kLiquidPropertyNoDraw				3

#define kLiquidPropertyColor				4
#define kLiquidPropertyTintAlpha			5
#define kLiquidPropertyDepth				6
#define kLiquidPropertySpeedAlter			7
#define kLiquidPropertySoundName			8

#define kLiquidPropertyWaveSize				9
#define kLiquidPropertyTideSize				10
#define kLiquidPropertyTideRate				11
#define kLiquidPropertyTideDirection		12

#define kLiquidPropertyHarm					13
#define kLiquidPropertyDrownTick			14
#define kLiquidPropertyDrownHarm			15

#define kLiquidPropertyGroup				16

#define kLiquidPropertyOffX					20
#define kLiquidPropertyOffY					21
#define kLiquidPropertySizeX				22
#define kLiquidPropertySizeY				23
#define kLiquidPropertyShiftX				24
#define kLiquidPropertyShiftY				25
#define kLiquidPropertyCamera				26

extern map_type					map;
extern editor_state_type		state;
extern editor_setup_type		setup;

extern list_palette_type		property_palette;

char							liquid_property_tide_direction_list[][name_str_len]={"Horizontal","Vertical",""};

/* =======================================================

      Property Palette Fill Liquid
      
======================================================= */

void property_palette_fill_liquid(int liq_idx)
{
	d3pnt					pnt,size;
	map_liquid_type			*liq;
	editor_view_type		*view;

		// liquid settings
		
	liq=&map.liquid.liquids[liq_idx];

	list_palette_add_header(&property_palette,0,"Liquid Settings");
	list_palette_add_checkbox(&property_palette,kLiquidPropertyWaveFlat,"Draw as Flat Surface",liq->tide.flat,FALSE);
	list_palette_add_checkbox(&property_palette,kLiquidPropertyNeverObscure,"Never Obscure",liq->never_obscure,FALSE);
	list_palette_add_checkbox(&property_palette,kLiquidPropertyNeverCull,"Never Cull",liq->never_cull,FALSE);
	list_palette_add_checkbox(&property_palette,kLiquidPropertyNoDraw,"No Draw (Volume Only)",liq->no_draw,FALSE);

	list_palette_add_header(&property_palette,0,"Liquid Under");
	list_palette_add_pick_color(&property_palette,kLiquidPropertyColor,"Color",&liq->col,FALSE);
	list_palette_add_string_float(&property_palette,kLiquidPropertyTintAlpha,"Tint Alpha",liq->tint_alpha,FALSE);
	list_palette_add_string_int(&property_palette,kLiquidPropertyDepth,"Depth",liq->depth,FALSE);
	list_palette_add_string_float(&property_palette,kLiquidPropertySpeedAlter,"Speed Alter",liq->speed_alter,FALSE);
	list_palette_add_string(&property_palette,kLiquidPropertySoundName,"Sound",liq->ambient.sound_name,FALSE);

	list_palette_add_header(&property_palette,0,"Liquid Waves");
	list_palette_add_string_int(&property_palette,kLiquidPropertyWaveSize,"Wave Size",liq->tide.division,FALSE);
	list_palette_add_string_int(&property_palette,kLiquidPropertyTideSize,"Tide Size",liq->tide.high,FALSE);
	list_palette_add_string_int(&property_palette,kLiquidPropertyTideRate,"Tide Rate",liq->tide.rate,FALSE);
	list_palette_add_string(&property_palette,kLiquidPropertyTideDirection,"Tide Direction",liquid_property_tide_direction_list[liq->tide.direction],FALSE);
	
	list_palette_add_header(&property_palette,0,"Liquid Harm");
	list_palette_add_string_int(&property_palette,kLiquidPropertyHarm,"In Damage",liq->harm.in_harm,FALSE);
	list_palette_add_string_int(&property_palette,kLiquidPropertyDrownTick,"Drowning Tick",liq->harm.drown_tick,FALSE);
	list_palette_add_string_int(&property_palette,kLiquidPropertyDrownHarm,"Drowning Damage",liq->harm.drown_harm,FALSE);

	list_palette_add_header(&property_palette,0,"Liquid Group");
	if (liq->group_idx==-1) {
		list_palette_add_string(&property_palette,kLiquidPropertyGroup,"Group","",FALSE);
	}
	else {
		list_palette_add_string(&property_palette,kLiquidPropertyGroup,"Group",map.group.groups[liq->group_idx].name,FALSE);
	}

		// polygon like settings
		
	view=view_get_current_view();
	
	list_palette_add_header(&property_palette,0,"Liquid UVs");
	if (view->uv_layer==uv_layer_normal) {
		list_palette_add_string_float(&property_palette,kLiquidPropertyOffX,"X Offset",liq->main_uv.x_offset,FALSE);
		list_palette_add_string_float(&property_palette,kLiquidPropertyOffY,"Y Offset",liq->main_uv.y_offset,FALSE);
		list_palette_add_string_float(&property_palette,kLiquidPropertySizeX,"X Size",liq->main_uv.x_size,FALSE);
		list_palette_add_string_float(&property_palette,kLiquidPropertySizeY,"Y Size",liq->main_uv.y_size,FALSE);
	}
	else {
		list_palette_add_string_float(&property_palette,kLiquidPropertyOffX,"X Offset",liq->lmap_uv.x_offset,FALSE);
		list_palette_add_string_float(&property_palette,kLiquidPropertyOffY,"Y Offset",liq->lmap_uv.y_offset,FALSE);
		list_palette_add_string_float(&property_palette,kLiquidPropertySizeX,"X Size",liq->lmap_uv.x_size,FALSE);
		list_palette_add_string_float(&property_palette,kLiquidPropertySizeY,"Y Size",liq->lmap_uv.y_size,FALSE);
	}
	list_palette_add_string_float(&property_palette,kLiquidPropertyShiftX,"X Shift",liq->x_shift,FALSE);
	list_palette_add_string_float(&property_palette,kLiquidPropertyShiftY,"Y Shift",liq->y_shift,FALSE);

	list_palette_add_header(&property_palette,0,"Liquid Camera");
	list_palette_add_string(&property_palette,kLiquidPropertyCamera,"Node",liq->camera,FALSE);
	
		// info
		
	pnt.x=liq->lft;
	pnt.y=liq->y;
	pnt.z=liq->top;
	
	size.x=liq->rgt-liq->lft;
	size.y=liq->depth;
	size.z=liq->bot-liq->top;
		
	list_palette_add_header(&property_palette,0,"Liquid Info");
	list_palette_add_point(&property_palette,-1,"Position",&pnt,TRUE);
	list_palette_add_point(&property_palette,-1,"Size",&size,TRUE);
}

/* =======================================================

      Property Palette Click Liquid
      
======================================================= */

void property_palette_click_liquid(int liq_idx,int id)
{
	map_liquid_type			*liq;
	editor_view_type		*view;

	liq=&map.liquid.liquids[liq_idx];
	view=view_get_current_view();

	switch (id) {

		case kLiquidPropertyWaveFlat:
			liq->tide.flat=!liq->tide.flat;
			break;

		case kLiquidPropertyNeverObscure:
			liq->never_obscure=!liq->never_obscure;
			break;

		case kLiquidPropertyNeverCull:
			liq->never_cull=!liq->never_cull;
			break;

		case kLiquidPropertyNoDraw:
			liq->no_draw=!liq->no_draw;
			break;

		case kLiquidPropertyColor:
			os_pick_color(&liq->col);
			break;
			
		case kLiquidPropertyTintAlpha:
			dialog_property_string_run(list_string_value_0_to_1_float,(void*)&liq->tint_alpha,0);
			break;
			
		case kLiquidPropertyDepth:
			dialog_property_string_run(list_string_value_positive_int,(void*)&liq->depth,0);
			break;

		case kLiquidPropertySpeedAlter:
			dialog_property_string_run(list_string_value_positive_float,(void*)&liq->speed_alter,0);
			break;

		case kLiquidPropertySoundName:
			property_palette_pick_sound(liq->ambient.sound_name,TRUE);
			break;

		case kLiquidPropertyWaveSize:
			dialog_property_string_run(list_string_value_positive_int,(void*)&liq->tide.division,0);
			break;
			
		case kLiquidPropertyTideSize:
			dialog_property_string_run(list_string_value_positive_int,(void*)&liq->tide.high,0);
			break;
			
		case kLiquidPropertyTideRate:
			dialog_property_string_run(list_string_value_positive_int,(void*)&liq->tide.rate,0);
			break;

		case kLiquidPropertyTideDirection:
			property_palette_pick_list((char*)liquid_property_tide_direction_list,&liq->tide.direction);
			break;

		case kLiquidPropertyHarm:
			dialog_property_string_run(list_string_value_int,(void*)&liq->harm.in_harm,0);
			break;
			
		case kLiquidPropertyDrownTick:
			dialog_property_string_run(list_string_value_positive_int,(void*)&liq->harm.drown_tick,0);
			break;
			
		case kLiquidPropertyDrownHarm:
			dialog_property_string_run(list_string_value_int,(void*)&liq->harm.drown_harm,0);
			break;

		case kLiquidPropertyGroup:
			property_palette_pick_group(&liq->group_idx);
			break;
			
		case kLiquidPropertyOffX:
			if (view->uv_layer==uv_layer_normal) {
				dialog_property_string_run(list_string_value_0_to_1_float,(void*)&liq->main_uv.x_offset,0);
			}
			else {
				dialog_property_string_run(list_string_value_0_to_1_float,(void*)&liq->lmap_uv.x_offset,0);
			}
			break;
			
		case kLiquidPropertyOffY:
			if (view->uv_layer==uv_layer_normal) {
				dialog_property_string_run(list_string_value_0_to_1_float,(void*)&liq->main_uv.y_offset,0);
			}
			else {
				dialog_property_string_run(list_string_value_0_to_1_float,(void*)&liq->lmap_uv.y_offset,0);
			}
			break;

		case kLiquidPropertySizeX:
			if (view->uv_layer==uv_layer_normal) {
				dialog_property_string_run(list_string_value_positive_float,(void*)&liq->main_uv.x_size,0);
			}
			else {
				dialog_property_string_run(list_string_value_positive_float,(void*)&liq->lmap_uv.x_size,0);
			}
			break;
			
		case kLiquidPropertySizeY:
			if (view->uv_layer==uv_layer_normal) {
				dialog_property_string_run(list_string_value_positive_float,(void*)&liq->main_uv.y_size,0);
			}
			else {
				dialog_property_string_run(list_string_value_positive_float,(void*)&liq->lmap_uv.y_size,0);
			}
			break;

		case kLiquidPropertyShiftX:
			dialog_property_string_run(list_string_value_float,(void*)&liq->x_shift,0);
			break;
			
		case kLiquidPropertyShiftY:
			dialog_property_string_run(list_string_value_float,(void*)&liq->y_shift,0);
			break;

		case kLiquidPropertyCamera:
			property_palette_pick_node(liq->camera);
			break;

	}

	main_wind_draw();
}

