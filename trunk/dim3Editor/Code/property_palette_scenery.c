/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Property Palette Scenery

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

#define kSceneryPropertyModelName				0
#define kSceneryPropertyAnimationName			1
#define kSceneryPropertyResize					2
#define kSceneryPropertyContactObject			3
#define kSceneryPropertyContactProjectile		4
#define kSceneryPropertyContactHitBox			5
#define kSceneryPropertyFaceFront				6
#define kSceneryPropertyShadow					7
#define kSceneryPropertyFrame					8

extern map_type					map;
extern editor_state_type		state;
extern editor_setup_type		setup;

extern list_palette_type		property_palette;

/* =======================================================

      Property Palette Fill Scenery
      
======================================================= */

void property_palette_fill_scenery(int scenery_idx)
{
	map_scenery_type			*scenery;

	scenery=&map.sceneries[scenery_idx];

	/*
	dialog_set_text(palette_scenery_wind,kSceneryModelName,0,scenery->model_name);
	dialog_set_text(palette_scenery_wind,kSceneryAnimationName,0,scenery->animation_name);
	dialog_set_float(palette_scenery_wind,kSceneryResize,0,scenery->resize);
	dialog_set_boolean(palette_scenery_wind,kSceneryContactObject,0,scenery->contact_object_on);
	dialog_set_boolean(palette_scenery_wind,kSceneryContactProjectile,0,scenery->contact_projectile_on);
	dialog_set_boolean(palette_scenery_wind,kSceneryContactHitBox,0,scenery->contact_hit_box);
	dialog_set_boolean(palette_scenery_wind,kSceneryFaceFront,0,scenery->face_forward);
	dialog_set_boolean(palette_scenery_wind,kSceneryShadow,0,scenery->shadow);
	
	for (n=0;n!=max_map_scenery_model_texture_frame;n++) {
		dialog_set_int(palette_scenery_wind,kSceneryFrame,n,(int)scenery->texture_frame[n]);
	}
*/
	/*
	list_palette_add_header(&property_palette,0,"Liquid Settings");
	list_palette_add_checkbox(&property_palette,kLiquidPropertyWaveFlat,"Draw as Flat Surface",liq->tide.flat);
	list_palette_add_checkbox(&property_palette,kLiquidPropertyNeverObscure,"Never Obscure",liq->never_obscure);
	list_palette_add_checkbox(&property_palette,kLiquidPropertyNoDraw,"No Draw (Volume Only)",liq->no_draw);

	list_palette_add_header(&property_palette,0,"Liquid Under");
	list_palette_add_pick_color(&property_palette,kLiquidPropertyColor,"Color",&liq->col);
	list_palette_add_string_float(&property_palette,kLiquidPropertyTintAlpha,"Tint Alpha",liq->tint_alpha);
	list_palette_add_string_int(&property_palette,kLiquidPropertyDepth,"Depth",liq->depth);
	list_palette_add_string_float(&property_palette,kLiquidPropertySpeedAlter,"Speed Alter",liq->speed_alter);
	list_palette_add_string(&property_palette,kLiquidPropertySoundName,"Sound",liq->ambient.sound_name);

	list_palette_add_header(&property_palette,0,"Liquid Waves");
	list_palette_add_string_int(&property_palette,kLiquidPropertyWaveSize,"Wave Size",liq->tide.division);
	list_palette_add_string_int(&property_palette,kLiquidPropertyTideSize,"Tide Size",liq->tide.high);
	list_palette_add_string_int(&property_palette,kLiquidPropertyTideRate,"Tide Rate",liq->tide.rate);
	list_palette_add_string(&property_palette,kLiquidPropertyTideDirection,"Tide Direction",liquid_property_tide_direction_list[liq->tide.direction]);
	
	list_palette_add_header(&property_palette,0,"Liquid Harm");
	list_palette_add_string_int(&property_palette,kLiquidPropertyHarm,"In Damage",liq->harm.in_harm);
	list_palette_add_string_int(&property_palette,kLiquidPropertyDrownTick,"Drowning Tick",liq->harm.drown_tick);
	list_palette_add_string_int(&property_palette,kLiquidPropertyDrownHarm,"Drowning Damage",liq->harm.drown_harm);

	list_palette_add_header(&property_palette,0,"Liquid Group");
	if (liq->group_idx==-1) {
		list_palette_add_string(&property_palette,kLiquidPropertyGroup,"Group","");
	}
	else {
		list_palette_add_string(&property_palette,kLiquidPropertyGroup,"Group",map.group.groups[liq->group_idx].name);
	}
	*/
}

/* =======================================================

      Property Palette Click Scenery
      
======================================================= */

void property_palette_click_scenery(int scenery_idx,int id)
{
	map_scenery_type			*scenery;

	scenery=&map.sceneries[scenery_idx];

	/*
	switch (id) {

		case kLiquidPropertyWaveFlat:
			liq->tide.flat=!liq->tide.flat;
			break;

		case kLiquidPropertyNeverObscure:
			liq->never_obscure=!liq->never_obscure;
			break;

		case kLiquidPropertyNoDraw:
			liq->no_draw=!liq->no_draw;
			break;

		case kLiquidPropertyColor:
		case kLiquidPropertyTintAlpha:
		case kLiquidPropertyDepth:
		case kLiquidPropertySpeedAlter:
		case kLiquidPropertySoundName:
		case kLiquidPropertyWaveSize:
		case kLiquidPropertyTideSize:
		case kLiquidPropertyTideRate:
		case kLiquidPropertyTideDirection:
		case kLiquidPropertyHarm:
		case kLiquidPropertyDrownTick:
		case kLiquidPropertyDrownHarm:
		case kLiquidPropertyGroup:
			break;

	}
*/
	main_wind_draw();
}

