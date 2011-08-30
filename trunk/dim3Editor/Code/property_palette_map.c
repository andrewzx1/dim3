/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Property Palette Map

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

#define kMapPropertyNormalCull				21
#define kMapPropertyCullAngle				22
#define kMapPropertyDisableShaders			23
#define kMapPropertyModelObscureDistance	24
#define kMapPropertyShadowObscureDistance	25
#define kMapPropertyEffectObscureDistance	26

#define kMapPropertyAmbientColor			30
#define kMapPropertyAmbientLightMapBoost	31
#define kMapPropertyAmbientSound			32
#define kMapPropertyAmbientSoundPitch		33

#define kMapPropertyLightMapQuality			34
#define kMapPropertyLightMapSize			35
#define kMapPropertyLightMapBorderCount		36
#define kMapPropertyLightMapBlurCount		37
#define kMapPropertyLightMapUseNormals		38
#define kMapPropertyLightMapDiffuseBoost	39

#define kMapPropertyMediaType				50
#define kMapPropertyMediaEventId			51
#define kMapPropertyMediaName				52
#define kMapPropertyMediaTitleSound			53

#define kMapPropertyMusicName				60
#define kMapPropertyMusicFadeTime			61
#define kMapPropertyMusicPreloadName		62

#define kMapPropertyEditorTextureFactor		70
#define kMapPropertyEditorViewNearZ			71
#define kMapPropertyEditorViewFarZ			72
#define kMapPropertyEditorLinkStartAlways	73

#define kMapPropertyParamsStart				100
#define kMapPropertyParamsEnd				109

extern map_type					map;
extern editor_state_type		state;
extern editor_setup_type		setup;

extern list_palette_type		property_palette;

char							map_property_light_map_size_list[][name_str_len]={"256","512","1024",""},
								map_property_media_type_list[][name_str_len]={"None","Chooser","Title","Cinema",""},
								map_property_light_map_quality_list[][name_str_len]={"Very Low","Low","Medium","High","Very High",""};

/* =======================================================

      Property Palette Fill Map
      
======================================================= */

void property_palette_fill_map(void)
{
	int				n,size;
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
	list_palette_add_string_int(&property_palette,kMapPropertyModelObscureDistance,"Model Obscure Distance",map.optimize.obscure_dist.model,FALSE);
	list_palette_add_string_int(&property_palette,kMapPropertyShadowObscureDistance,"Shadow Obscure Distance",map.optimize.obscure_dist.shadow,FALSE);
	list_palette_add_string_int(&property_palette,kMapPropertyEffectObscureDistance,"Effect Obscure Distance",map.optimize.obscure_dist.effect,FALSE);

		// ambient

	list_palette_add_header(&property_palette,0,"Map Ambient");
	list_palette_add_pick_color(&property_palette,kMapPropertyAmbientColor,"Color",&map.ambient.light_color,FALSE);
	list_palette_add_string_float(&property_palette,kMapPropertyAmbientLightMapBoost,"Light Map Boost",map.ambient.light_map_boost,FALSE);
	list_palette_add_string(&property_palette,kMapPropertyAmbientSound,"Sound",map.ambient.sound_name,FALSE);
	list_palette_add_string_float(&property_palette,kMapPropertyAmbientSoundPitch,"Sound Pitch",map.ambient.sound_pitch,FALSE);

		// light map

	size=((int)log2(map.light_map.size))-8;
	if ((size<0) || (size>2)) size=0;

	list_palette_add_header(&property_palette,0,"Map Light Map");
	list_palette_add_string(&property_palette,kMapPropertyLightMapQuality,"Quality",map_property_light_map_quality_list[map.light_map.quality],FALSE);
	list_palette_add_string(&property_palette,kMapPropertyLightMapSize,"Texture Size",map_property_light_map_size_list[size],FALSE);
	list_palette_add_string_int(&property_palette,kMapPropertyLightMapBorderCount,"Pixel Border Count",map.light_map.pixel_border_count,FALSE);
	list_palette_add_string_int(&property_palette,kMapPropertyLightMapBlurCount,"Blur Count",map.light_map.blur_count,FALSE);
	list_palette_add_checkbox(&property_palette,kMapPropertyLightMapUseNormals,"Use Normals (Hard Edges)",map.light_map.use_normals,FALSE);
	list_palette_add_string_float(&property_palette,kMapPropertyLightMapDiffuseBoost,"Diffuse Boost",map.light_map.diffuse_boost,FALSE);

		// media

	list_palette_add_header(&property_palette,0,"Map Media");
	list_palette_add_string(&property_palette,kMapPropertyMediaType,"Startup Type",map_property_media_type_list[map.media.type],FALSE);
	list_palette_add_string_int(&property_palette,kMapPropertyMediaEventId,"Event Id",map.media.event_id,FALSE);
	list_palette_add_string(&property_palette,kMapPropertyMediaName,"Startup Name",map.media.name,FALSE);
	list_palette_add_string(&property_palette,kMapPropertyMediaTitleSound,"Startup Title Sound",map.media.title_sound_name,FALSE);

		// music

	list_palette_add_header(&property_palette,0,"Map Music");
	list_palette_add_string(&property_palette,kMapPropertyMusicName,"Name",map.music.name,FALSE);
	list_palette_add_string_int(&property_palette,kMapPropertyMusicFadeTime,"Fade In Time",map.music.fade_msec,FALSE);
	for (n=0;n!=max_music_preload;n++) {
		sprintf(name,"Preload Name %d",n);
		list_palette_add_string(&property_palette,(kMapPropertyMusicPreloadName+n),name,map.music.preload_name[n],FALSE);
	}

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
	int				param_idx,music_idx,size;
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

		// preload music

	if ((id>=kMapPropertyMusicPreloadName) && (id<(kMapPropertyMusicPreloadName+max_music_preload))) {
		music_idx=(id-kMapPropertyMusicPreloadName);
		dialog_property_string_run(list_string_value_string,(void*)map.music.preload_name[music_idx],name_str_len,0,0);
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

		case kMapPropertyModelObscureDistance:
			dialog_property_string_run(list_string_value_positive_int,(void*)&map.optimize.obscure_dist.model,0,0,0);
			break;
			
		case kMapPropertyShadowObscureDistance:
			dialog_property_string_run(list_string_value_positive_int,(void*)&map.optimize.obscure_dist.shadow,0,0,0);
			break;

		case kMapPropertyEffectObscureDistance:
			dialog_property_string_run(list_string_value_positive_int,(void*)&map.optimize.obscure_dist.effect,0,0,0);
			break;

			// ambients

		case kMapPropertyAmbientColor:
			os_pick_color(&map.ambient.light_color);
			break;

		case kMapPropertyAmbientLightMapBoost:
			dialog_property_string_run(list_string_value_positive_float,(void*)&map.ambient.light_map_boost,0,0,0);
			break;

		case kMapPropertyAmbientSound:
			property_palette_pick_sound(map.ambient.sound_name,TRUE);
			break;

		case kMapPropertyAmbientSoundPitch:
			dialog_property_string_run(list_string_value_0_to_1_float,(void*)&map.ambient.sound_pitch,0,0,0);
			break;

			// light maps
	
		case kMapPropertyLightMapQuality:
			property_pick_list("Pick Light Map Quality",(char*)map_property_light_map_quality_list,&map.light_map.quality);
			break;
			
		case kMapPropertyLightMapSize:
			size=((int)log2(map.light_map.size))-8;
			if ((size<0) || (size>2)) size=0;
			property_pick_list("Pick a Light Map Size",(char*)map_property_light_map_size_list,&size);
			map.light_map.size=(int)pow(2,(size+8));
			break;
			
		case kMapPropertyLightMapBorderCount:
			dialog_property_string_run(list_string_value_range_int,(void*)&map.light_map.pixel_border_count,0,0,10);
			break;
			
		case kMapPropertyLightMapBlurCount:
			dialog_property_string_run(list_string_value_range_int,(void*)&map.light_map.blur_count,0,0,6);
			break;

		case kMapPropertyLightMapUseNormals:
			map.light_map.use_normals=!map.light_map.use_normals;
			break;

		case kMapPropertyLightMapDiffuseBoost:
			dialog_property_string_run(list_string_value_positive_float,(void*)&map.light_map.diffuse_boost,0,0,0);
			break;

			// map media

		case kMapPropertyMediaType:
			property_pick_list("Pick a Media Type",(char*)map_property_media_type_list,&map.media.type);
			break;
		
		case kMapPropertyMediaEventId:
			dialog_property_string_run(list_string_value_int,(void*)&map.media.event_id,0,0,0);
			break;

		case kMapPropertyMediaName:
			dialog_property_string_run(list_string_value_string,(void*)map.media.name,name_str_len,0,0);
			break;

		case kMapPropertyMediaTitleSound:
			property_palette_pick_sound(map.media.title_sound_name,TRUE);
			break;

			// map music

		case kMapPropertyMusicName:
			dialog_property_string_run(list_string_value_string,(void*)map.music.name,name_str_len,0,0);
			break;

		case kMapPropertyMusicFadeTime:
			dialog_property_string_run(list_string_value_positive_int,(void*)&map.music.fade_msec,0,0,0);
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

