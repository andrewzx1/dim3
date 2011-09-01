/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Property Palette Map

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
#include "ui_common.h"
#include "interface.h"

#define kMapPropertyInfoTitle				0
#define kMapPropertyInfoAuthor				1

#define kMapPropertyGravity					2
#define kMapPropertyGravityMaxPower			3
#define kMapPropertyGravityMaxSpeed			4
#define kMapPropertyResistance				5
#define kMapPropertySlopeMinAngle			6
#define kMapPropertySlopeMaxAngle			7
#define kMapPropertySlopeMaxSpeed			8
#define kMapPropertySlopeMinGravity			9

#define kMapPropertyNetworkGameList			20

#define kMapPropertyNormalCull				30
#define kMapPropertyCullAngle				31
#define kMapPropertyDisableShaders			32
#define kMapPropertyRayTraceObscure			33
#define kMapPropertyModelObscureDistance	34
#define kMapPropertyShadowObscureDistance	35
#define kMapPropertyEffectObscureDistance	36

#define kMapPropertyEditorTextureFactor		40
#define kMapPropertyEditorViewNearZ			41
#define kMapPropertyEditorViewFarZ			42
#define kMapPropertyEditorLinkStartAlways	43

#define kMapPropertyParamsStart				100
#define kMapPropertyParamsEnd				109

extern map_type					map;
extern editor_state_type		state;
extern editor_setup_type		setup;

extern list_palette_type		property_palette;

/* =======================================================

      Property Palette Fill Map
      
======================================================= */

void property_palette_fill_map(void)
{
	int				n;
	char			name[256],str[256];
	d3fpnt			uv;

	list_palette_set_title(&property_palette,"Map Settings");

		// info

	list_palette_add_header(&property_palette,0,"Map Info");
	list_palette_add_string(&property_palette,-1,"Name",map.info.name,TRUE);
	list_palette_add_string(&property_palette,kMapPropertyInfoTitle,"Title",map.info.title,FALSE);
	list_palette_add_string(&property_palette,kMapPropertyInfoAuthor,"Author",map.info.author,FALSE);

		// settings

	list_palette_add_header(&property_palette,0,"Map Physics");
	list_palette_add_string_float(&property_palette,kMapPropertyGravity,"Gravity",map.physics.gravity,FALSE);
	list_palette_add_string_float(&property_palette,kMapPropertyGravityMaxPower,"Gravity Max Power",map.physics.gravity_max_power,FALSE);
	list_palette_add_string_float(&property_palette,kMapPropertyGravityMaxSpeed,"Gravity Max Speed",map.physics.gravity_max_speed,FALSE);
	list_palette_add_string_float(&property_palette,kMapPropertyResistance,"Resistance",map.physics.resistance,FALSE);
	list_palette_add_string_float(&property_palette,kMapPropertySlopeMinAngle,"Slope Min Angle",map.physics.slope_min_ang,FALSE);
	list_palette_add_string_float(&property_palette,kMapPropertySlopeMaxAngle,"Slope Max Angle",map.physics.slope_max_ang,FALSE);
	list_palette_add_string_float(&property_palette,kMapPropertySlopeMaxSpeed,"Slope Max Speed",map.physics.slope_max_speed,FALSE);
	list_palette_add_string_float(&property_palette,kMapPropertySlopeMinGravity,"Slope Min Gravity",map.physics.slope_min_gravity,FALSE);

		// network

	list_palette_add_header(&property_palette,0,"Map Networking");
	list_palette_add_string(&property_palette,kMapPropertyNetworkGameList,"Net Game List","...",FALSE);

		// optimizations

	list_palette_add_header(&property_palette,0,"Map Optimizations");
	list_palette_add_checkbox(&property_palette,kMapPropertyNormalCull,"Cull By Normals",(!map.optimize.never_cull),FALSE);
	list_palette_add_string_float(&property_palette,kMapPropertyCullAngle,"Cull Angle",map.optimize.cull_angle,FALSE);
	list_palette_add_checkbox(&property_palette,kMapPropertyDisableShaders,"Disable Shaders",map.optimize.no_shaders,FALSE);
	list_palette_add_checkbox(&property_palette,kMapPropertyRayTraceObscure,"Ray Trace Obscure",map.optimize.ray_trace_obscure,FALSE);
	list_palette_add_string_int(&property_palette,kMapPropertyModelObscureDistance,"Model Obscure Distance",map.optimize.obscure_dist.model,FALSE);
	list_palette_add_string_int(&property_palette,kMapPropertyShadowObscureDistance,"Shadow Obscure Distance",map.optimize.obscure_dist.shadow,FALSE);
	list_palette_add_string_int(&property_palette,kMapPropertyEffectObscureDistance,"Effect Obscure Distance",map.optimize.obscure_dist.effect,FALSE);

		// parameters

	list_palette_add_header(&property_palette,0,"Map Parameters");
	for (n=0;n!=10;n++) {
		sprintf(name,"Param %d",n);
		property_get_parameter(n,map.settings.params,str);
		list_palette_add_string(&property_palette,(kMapPropertyParamsStart+n),name,str,FALSE);
	}

		// editor setup

	list_palette_add_header(&property_palette,0,"Map Editor Setup");
	uv.x=map.editor_setup.txt_scale_x;
	uv.y=map.editor_setup.txt_scale_y;
	list_palette_add_uv(&property_palette,kMapPropertyEditorTextureFactor,"Default Texture Scale",&uv,FALSE);
	list_palette_add_string_int(&property_palette,kMapPropertyEditorViewNearZ,"Near Z",map.editor_setup.view_near_dist,FALSE);
	list_palette_add_string_int(&property_palette,kMapPropertyEditorViewFarZ,"Far Z",map.editor_setup.view_far_dist,FALSE);
	list_palette_add_checkbox(&property_palette,kMapPropertyEditorLinkStartAlways,"Always Start at Player Spot",map.editor_setup.link_always_start,FALSE);

	list_palette_add_header(&property_palette,0,"Map Counts");
	list_palette_add_string_int(&property_palette,-1,"Meshes",map.mesh.nmesh,TRUE);
	list_palette_add_string_int(&property_palette,-1,"Liquids",map.liquid.nliquid,TRUE);
	list_palette_add_string_int(&property_palette,-1,"Spot",map.nspot,TRUE);
	list_palette_add_string_int(&property_palette,-1,"Light",map.nlight,TRUE);
	list_palette_add_string_int(&property_palette,-1,"Sound",map.nsound,TRUE);
	list_palette_add_string_int(&property_palette,-1,"Particle",map.nparticle,TRUE);
	list_palette_add_string_int(&property_palette,-1,"Scenery",map.nscenery,TRUE);
	list_palette_add_string_int(&property_palette,-1,"Node",map.nnode,TRUE);
}

/* =======================================================

      Property Palette Click Map
      
======================================================= */

void property_palette_click_map(int id,bool double_click)
{
	int				param_idx;
	char			str[256];
	d3fpnt			uv;

	if (!double_click) return;

		// parameters

	if ((id>=kMapPropertyParamsStart) && (id<=kMapPropertyParamsEnd)) {
		param_idx=(id-kMapPropertyParamsStart);
		
		property_get_parameter(param_idx,map.settings.params,str);
		dialog_property_string_run(list_string_value_string,(void*)str,256,0,0);
		property_set_parameter(param_idx,map.settings.params,str);

		main_wind_draw();
		return;
	}
	
		// regular properties

	switch (id) {

			// info

		case kMapPropertyInfoTitle:
			dialog_property_string_run(list_string_value_string,(void*)map.info.title,name_str_len,0,0);
			break;

		case kMapPropertyInfoAuthor:
			dialog_property_string_run(list_string_value_string,(void*)map.info.author,name_str_len,0,0);
			break;

			// physics

		case kMapPropertyGravity:
			dialog_property_string_run(list_string_value_positive_float,(void*)&map.physics.gravity,0,0,0);
			break;

		case kMapPropertyGravityMaxPower:
			dialog_property_string_run(list_string_value_positive_float,(void*)&map.physics.gravity_max_power,0,0,0);
			break;

		case kMapPropertyGravityMaxSpeed:
			dialog_property_string_run(list_string_value_positive_float,(void*)&map.physics.gravity_max_speed,0,0,0);
			break;

		case kMapPropertyResistance:
			dialog_property_string_run(list_string_value_positive_float,(void*)&map.physics.resistance,0,0,0);
			break;

		case kMapPropertySlopeMinAngle:
			dialog_property_string_run(list_string_value_positive_float,(void*)&map.physics.slope_min_ang,0,0,0);
			break;

		case kMapPropertySlopeMaxAngle:
			dialog_property_string_run(list_string_value_positive_float,(void*)&map.physics.slope_max_ang,0,0,0);
			break;

		case kMapPropertySlopeMaxSpeed:
			dialog_property_string_run(list_string_value_positive_float,(void*)&map.physics.slope_max_speed,0,0,0);
			break;

		case kMapPropertySlopeMinGravity:
			dialog_property_string_run(list_string_value_positive_float,(void*)&map.physics.slope_min_gravity,0,0,0);
			break;

			// networking

		case kMapPropertyNetworkGameList:
			dialog_property_string_run(list_string_value_string,(void*)map.settings.network_game_list,256,0,0);
			break;
			
			// optimizations
			
		case kMapPropertyNormalCull:
			map.optimize.never_cull=!map.optimize.never_cull;
			break;
			
		case kMapPropertyCullAngle:
			dialog_property_string_run(list_string_value_float,(void*)&map.optimize.cull_angle,0,0,0);
			break;

		case kMapPropertyDisableShaders:
			map.optimize.no_shaders=!map.optimize.no_shaders;
			break;

		case kMapPropertyRayTraceObscure:
			map.optimize.ray_trace_obscure=!map.optimize.ray_trace_obscure;
			break;

		case kMapPropertyModelObscureDistance:
			dialog_property_string_run(list_string_value_positive_int,(void*)&map.optimize.obscure_dist.model,0,0,0);
			break;
			
		case kMapPropertyShadowObscureDistance:
			dialog_property_string_run(list_string_value_positive_int,(void*)&map.optimize.obscure_dist.shadow,0,0,0);
			break;

		case kMapPropertyEffectObscureDistance:
			dialog_property_string_run(list_string_value_positive_int,(void*)&map.optimize.obscure_dist.effect,0,0,0);
			break;

			// editor setup

		case kMapPropertyEditorTextureFactor:
			uv.x=map.editor_setup.txt_scale_x;
			uv.y=map.editor_setup.txt_scale_y;
			dialog_property_chord_run(list_chord_value_uv,(void*)&uv);
			map.editor_setup.txt_scale_x=uv.x;
			map.editor_setup.txt_scale_y=uv.y;
			break;

		case kMapPropertyEditorViewNearZ:
			dialog_property_string_run(list_string_value_positive_int,(void*)&map.editor_setup.view_near_dist,0,0,0);
			break;

		case kMapPropertyEditorViewFarZ:
			dialog_property_string_run(list_string_value_positive_int,(void*)&map.editor_setup.view_far_dist,0,0,0);
			break;

		case kMapPropertyEditorLinkStartAlways:
			map.editor_setup.link_always_start=!map.editor_setup.link_always_start;
			break;

	}

	main_wind_draw();
}

