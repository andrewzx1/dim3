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

#define kLiquidPropertyNeverObscure			0
#define kLiquidPropertyNoDraw				1

#define kLiquidPropertyColor				2
#define kLiquidPropertyTintAlpha			3
#define kLiquidPropertyDepth				4
#define kLiquidPropertySpeedAlter			6
#define kLiquidPropertySoundName			5

#define kLiquidPropertyWaveSize				7
#define kLiquidPropertyTideSize				8
#define kLiquidPropertyTideRate				9
#define kLiquidPropertyTideDirection		10
#define kLiquidPropertyWaveFlat				11

#define kLiquidPropertyGroup				12

#define kLiquidPropertyHarm					13
#define kLiquidPropertyDrownTick			14
#define kLiquidPropertyDrownHarm			15

extern map_type					map;
extern editor_state_type		state;
extern editor_setup_type		setup;

extern list_palette_type		property_palette;

/* =======================================================

      Property Palette Fill Liquid
      
======================================================= */

/*
	
	dialog_set_value(palette_liquid_wind,kLiquidWaveSize,0,liq->tide.division);
	dialog_set_value(palette_liquid_wind,kLiquidTideSize,0,liq->tide.high);
	dialog_set_value(palette_liquid_wind,kLiquidTideRate,0,liq->tide.rate);
	dialog_set_combo(palette_liquid_wind,kLiquidTideDirection,0,liq->tide.direction);
	dialog_set_boolean(palette_liquid_wind,kLiquidWaveFlat,0,liq->tide.flat);
	
	dialog_set_int(palette_liquid_wind,kLiquidHarm,0,liq->harm.in_harm);
	dialog_set_int(palette_liquid_wind,kLiquidDrownTick,0,liq->harm.drown_tick);
	dialog_set_int(palette_liquid_wind,kLiquidDrownHarm,0,liq->harm.drown_harm);
	
	
	dialog_fill_group_combo(palette_liquid_wind,kLiquidGroup,0,liq->group_idx);
	
	
*/

void property_palette_fill_liquid(map_liquid_type *liq)
{
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
	
	
	list_palette_add_header(&property_palette,0,"Liquid Harm");
	
	

	list_palette_add_header(&property_palette,0,"Liquid Group");
	if (liq->group_idx==-1) {
		list_palette_add_string(&property_palette,kLiquidPropertyGroup,"Group","");
	}
	else {
		list_palette_add_string(&property_palette,kLiquidPropertyGroup,"Group",map.group.groups[liq->group_idx].name);
	}


/*
	list_palette_add_checkbox(&property_palette,kMeshPropertyOn,"On",mesh->flag.on);
	list_palette_add_checkbox(&property_palette,kMeshPropertyPassThrough,"Pass Through",mesh->flag.pass_through);
	list_palette_add_checkbox(&property_palette,kMeshPropertyMovable,"Movable",mesh->flag.moveable);
	list_palette_add_checkbox(&property_palette,kMeshPropertyHilite,"High Lighted",mesh->flag.hilite);
	list_palette_add_checkbox(&property_palette,kMeshPropertyLockUV,"Lock UVs",mesh->flag.lock_uv);
	list_palette_add_checkbox(&property_palette,kMeshPropertyLockMove,"Lock Position",mesh->flag.lock_move);
	list_palette_add_checkbox(&property_palette,kMeshPropertyShadow,"Shadow",mesh->flag.shadow);
	list_palette_add_checkbox(&property_palette,kMeshPropertyNeverObscure,"Never Obscure",mesh->flag.never_obscure);
	list_palette_add_checkbox(&property_palette,kMeshPropertyRotIndependent,"Rotate Independently",mesh->flag.rot_independent);
	list_palette_add_checkbox(&property_palette,kMeshPropertyNoLightMap,"No Light Map",mesh->flag.no_light_map);
	list_palette_add_checkbox(&property_palette,kMeshPropertySkipLightMapTrace,"Skip Light Map Trace",mesh->flag.skip_light_map_trace);

	list_palette_add_header(&property_palette,0,"Mesh Modes");
	list_palette_add_string(&property_palette,kMeshPropertyHideMode,"Hide",mesh_property_hide_list[mesh->hide_mode]);
	list_palette_add_string(&property_palette,kMeshPropertyNormalMode,"Normals",mesh_property_normal_list[mesh->normal_mode]);

	list_palette_add_header(&property_palette,0,"Mesh Group");
	if (mesh->group_idx==-1) {
		list_palette_add_string(&property_palette,kMeshPropertyGroup,"Group","");
	}
	else {
		list_palette_add_string(&property_palette,kMeshPropertyGroup,"Group",map.group.groups[mesh->group_idx].name);
	}

	list_palette_add_header(&property_palette,0,"Mesh Rotational Center");
	list_palette_add_string_int(&property_palette,kMeshPropertyRotX,"X",mesh->rot_off.x);
	list_palette_add_string_int(&property_palette,kMeshPropertyRotY,"Y",mesh->rot_off.y);
	list_palette_add_string_int(&property_palette,kMeshPropertyRotZ,"Z",mesh->rot_off.z);

	list_palette_add_header(&property_palette,0,"Mesh Messages");
	list_palette_add_checkbox(&property_palette,kMeshPropertyMessageEnter,"Entry On",mesh->msg.entry_on);
	list_palette_add_string_int(&property_palette,kMeshPropertyMessageEnterId,"Entry ID",mesh->msg.entry_id);
	list_palette_add_checkbox(&property_palette,kMeshPropertyMessageExit,"Exit On",mesh->msg.exit_on);
	list_palette_add_string_int(&property_palette,kMeshPropertyMessageExitId,"Exit ID",mesh->msg.exit_id);
	list_palette_add_checkbox(&property_palette,kMeshPropertyMessageMapChange,"Map Change On",mesh->msg.map_change_on);
	list_palette_add_string(&property_palette,kMeshPropertyMessageMapChangeName,"Map Name",mesh->msg.map_name);
	list_palette_add_string(&property_palette,kMeshPropertyMessageMapChangeSpotName,"Map Spot Name",mesh->msg.map_spot_name);
	list_palette_add_checkbox(&property_palette,kMeshPropertyMessageBase,"Base On",mesh->msg.base_on);
	list_palette_add_string(&property_palette,kMeshPropertyMessageBaseTeam,"Base Team",mesh_property_team_list[mesh->msg.base_team]);
	*/
}

/* =======================================================

      Property Palette Click Liquid
      
======================================================= */

void property_palette_click_liquid(map_liquid_type *liq,int id)
{
	/*
	switch (id) {

		case kMeshPropertyOn:
			mesh->flag.on=!mesh->flag.on;
			break;

		case kMeshPropertyPassThrough:
			mesh->flag.pass_through=!mesh->flag.pass_through;
			break;

		case kMeshPropertyMovable:
			mesh->flag.moveable=!mesh->flag.moveable;
			break;

		case kMeshPropertyHilite:
			mesh->flag.hilite=!mesh->flag.hilite;
			break;

		case kMeshPropertyLockUV:
			mesh->flag.lock_uv=!mesh->flag.lock_uv;
			break;

		case kMeshPropertyLockMove:
			mesh->flag.lock_move=!mesh->flag.lock_move;
			break;

		case kMeshPropertyShadow:
			mesh->flag.shadow=!mesh->flag.shadow;
			break;

		case kMeshPropertyNeverObscure:
			mesh->flag.never_obscure=!mesh->flag.never_obscure;
			break;

		case kMeshPropertyRotIndependent:
			mesh->flag.rot_independent=!mesh->flag.rot_independent;
			break;

		case kMeshPropertyNoLightMap:
			mesh->flag.no_light_map=!mesh->flag.no_light_map;
			break;

		case kMeshPropertySkipLightMapTrace:
			mesh->flag.skip_light_map_trace=!mesh->flag.skip_light_map_trace;
			break;

		case kMeshPropertyHideMode:
		case kMeshPropertyNormalMode:
		case kMeshPropertyGroup:
		case kMeshPropertyRotX:
		case kMeshPropertyRotY:
		case kMeshPropertyRotZ:
			break;

		case kMeshPropertyMessageEnter:
			mesh->msg.entry_on=!mesh->msg.entry_on;
			break;

		case kMeshPropertyMessageEnterId:
			break;

		case kMeshPropertyMessageExit:
			mesh->msg.exit_on=!mesh->msg.exit_on;
			break;

		case kMeshPropertyMessageExitId:
			break;

		case kMeshPropertyMessageMapChange:
			mesh->msg.map_change_on=!mesh->msg.map_change_on;
			break;

		case kMeshPropertyMessageMapChangeName:
		case kMeshPropertyMessageMapChangeSpotName:
			break;

		case kMeshPropertyMessageBase:
			mesh->msg.base_on=!mesh->msg.base_on;
			break;

		case kMeshPropertyMessageBaseTeam:
			break;

	}
*/
	main_wind_draw();
}

