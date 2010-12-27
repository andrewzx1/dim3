/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Property Palette Map

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit �Created with dim3 Technology� is given on a single
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

#define kMapPropertyInfoTitle				0
#define kMapPropertyInfoAuthor				1

#define kMapPropertyGravity					2
#define kMapPropertyGravityMaxPower			3
#define kMapPropertyGravityMaxSpeed			4
#define kMapPropertyResistance				5
#define kMapPropertyNeverCull				6
#define kMapPropertyDisableShaders			7
#define kMapPropertyNetworkGameList			8

#define kMapPropertyAmbientColor			9
#define kMapPropertyAmbientLightMapBoost	10
#define kMapPropertyAmbientSound			11
#define kMapPropertyAmbientSoundPitch		12

#define kMapPropertyLightMapQuality			13
#define kMapPropertyLightMapSize			14
#define kMapPropertyLightMapBorderCount		15
#define kMapPropertyLightMapBlurCount		16

#define kMapPropertyCameraMode				17
#define kMapPropertyCameraAngle				18

#define kMapPropertyCameraFOV				19
#define kMapPropertyCameraAspectRatio		20
#define kMapPropertyCameraNearZ				21
#define kMapPropertyCameraFarZ				22
#define kMapPropertyCameraNearZOffset		23

#define kMapPropertyCameraChaseDistance		24
#define kMapPropertyCameraChaseTrackSpeed	25
#define kMapPropertyCameraChaseSlop			26

#define kMapPropertyCameraStaticFollow		27
#define kMapPropertyCameraStaticAttachNode	28

#define kMapPropertyMediaType				29
#define kMapPropertyMediaName				30
#define kMapPropertyMediaTitleSound			31

#define kMapPropertyMusicName				32
#define kMapPropertyMusicFadeTime			33

#define kMapPropertySkyOn					34
#define kMapPropertySkyType					35
#define kMapPropertySkyRadius				36
#define kMapPropertyDomeY					37
#define kMapPropertyDomeMirror				38
#define kMapPropertyTextureRepeat			39
#define kMapPropertyTextureShift			40
#define kMapPropertyTextureFill				41
#define kMapPropertyTextureBottomFill		42
#define kMapPropertyTextureNorthFill		43
#define kMapPropertyTextureSouthFill		44
#define kMapPropertyTextureEastFill			45
#define kMapPropertyTextureWestFill			46

#define kMapPropertyBackgroundOn			50
#define kMapPropertyBackgroundFrontFill		51
#define kMapPropertyBackgroundFrontShift	52
#define kMapPropertyBackgroundMiddleFill	53
#define kMapPropertyBackgroundMiddleShift	54
#define kMapPropertyBackgroundBackFill		55
#define kMapPropertyBackgroundBackShift		56

#define kMapPropertyFogOn					60
#define kMapPropertyFogCount				61
#define kMapPropertyFogInnerRadius			62
#define kMapPropertyFogOuterRadius			63
#define kMapPropertyFogHigh					64
#define kMapPropertyFogDrop					65
#define kMapPropertyFogTextureIndex			66
#define kMapPropertyFogTextureSpeed			67
#define kMapPropertyFogTextureXFact			68
#define kMapPropertyFogTextureYFact			69
#define kMapPropertyFogColor				70
#define kMapPropertyFogAlpha				71
#define kMapPropertyFogUseSolidColor		72

#define kMapPropertyRainOn					75
#define kMapPropertyRainDensity				76
#define kMapPropertyRainRadius				77
#define kMapPropertyRainHeight				78
#define kMapPropertyRainSpeed				79
#define kMapPropertyRainWidth				80
#define kMapPropertyRainLength				81
#define kMapPropertyRainAlpha				82
#define kMapPropertyRainStartColor			83
#define kMapPropertyRainEndColor			84
#define kMapPropertyRainSlantAdd			85
#define kMapPropertyRainSlantTimeMSec		86
#define kMapPropertyRainSlantChangeMSec		87

#define kMapPropertyEditorTextureFactor		90
#define kMapPropertyEditorViewNearZ			91
#define kMapPropertyEditorViewFarZ			92
#define kMapPropertyEditorLinkStartAlways	93

#define kMapPropertyParamsStart				100
#define kMapPropertyParamsEnd				109

extern map_type					map;
extern editor_state_type		state;
extern editor_setup_type		setup;

extern list_palette_type		property_palette;

char							map_property_light_map_size_list[][name_str_len]={"256","512","1024",""},
								map_property_camera_mode_list[][name_str_len]={"FPP","Chase","Static","Chase Static",""},
								map_property_media_type_list[][name_str_len]={"None","Chooser","Title","Cinema",""},
								map_property_sky_type_list[][name_str_len]={"Panoramic","Dome Hemisphere","Cube",""};

/* =======================================================

      Property Palette Fill Map
      
======================================================= */

// supergumba -- to do -- fog

/*
typedef struct		{
						int									count,inner_radius,outer_radius,
															high,drop,texture_idx;
						float								speed,txt_x_fact,txt_y_fact,
															alpha;
						bool								on,use_solid_color;
						d3col								col;
					} map_fog_type;

*/

void property_palette_fill_map(void)
{
	int				n,size;
	char			name[256],str[256];
	d3fpnt			uv;
	
		// info

	list_palette_add_header(&property_palette,0,"Map Info");
	list_palette_add_string(&property_palette,-1,"Name",map.info.name,TRUE);
	list_palette_add_string(&property_palette,kMapPropertyInfoTitle,"Title",map.info.title,FALSE);
	list_palette_add_string(&property_palette,kMapPropertyInfoAuthor,"Author",map.info.author,FALSE);

		// settings

	list_palette_add_header(&property_palette,0,"Map Settings");
	list_palette_add_string_float(&property_palette,kMapPropertyGravity,"Gravity",map.settings.gravity,FALSE);
	list_palette_add_string_float(&property_palette,kMapPropertyGravityMaxPower,"Gravity Max Power",map.settings.gravity_max_power,FALSE);
	list_palette_add_string_float(&property_palette,kMapPropertyGravityMaxSpeed,"Gravity Max Speed",map.settings.gravity_max_speed,FALSE);
	list_palette_add_string_float(&property_palette,kMapPropertyResistance,"Resistance",map.settings.resistance,FALSE);
	list_palette_add_checkbox(&property_palette,kMapPropertyNeverCull,"Never Cull",map.settings.never_cull,FALSE);
	list_palette_add_checkbox(&property_palette,kMapPropertyDisableShaders,"Disable Shaders",map.settings.no_shaders,FALSE);
	list_palette_add_string(&property_palette,kMapPropertyNetworkGameList,"Net Game List",map.settings.network_game_list,FALSE);

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
	list_palette_add_string_int(&property_palette,kMapPropertyLightMapQuality,"Quality",map.light_map.quality,FALSE);
	list_palette_add_string(&property_palette,kMapPropertyLightMapSize,"Texture Size",map_property_light_map_size_list[size],FALSE);
	list_palette_add_string_int(&property_palette,kMapPropertyLightMapBorderCount,"Pixel Border Count",map.light_map.pixel_border_count,FALSE);
	list_palette_add_string_int(&property_palette,kMapPropertyLightMapBlurCount,"Blur Count",map.light_map.blur_count,FALSE);

		// camera settings

	list_palette_add_header(&property_palette,0,"Map Camera Settings");
	list_palette_add_string(&property_palette,kMapPropertyCameraMode,"Mode",map_property_camera_mode_list[map.camera.mode],FALSE);
	list_palette_add_angle(&property_palette,kMapPropertyCameraAngle,"Offset Angle",&map.camera.ang,FALSE);

		// camera plane

	list_palette_add_header(&property_palette,0,"Map Camera Plane");
	list_palette_add_string_float(&property_palette,kMapPropertyCameraFOV,"FOV",map.camera.plane.fov,FALSE);
	list_palette_add_string_float(&property_palette,kMapPropertyCameraAspectRatio,"Aspect Ratio",map.camera.plane.aspect_ratio,FALSE);
	list_palette_add_string_int(&property_palette,kMapPropertyCameraNearZ,"Near Z",map.camera.plane.near_z,FALSE);
	list_palette_add_string_int(&property_palette,kMapPropertyCameraFarZ,"Far Z",map.camera.plane.far_z,FALSE);
	list_palette_add_string_int(&property_palette,kMapPropertyCameraNearZOffset,"Near Z Offset",map.camera.plane.near_z_offset,FALSE);

		// camera chase

	list_palette_add_header(&property_palette,0,"Map Camera Chase");
	list_palette_add_string_int(&property_palette,kMapPropertyCameraChaseDistance,"Distance",map.camera.chase.distance,FALSE);
	list_palette_add_string_float(&property_palette,kMapPropertyCameraChaseTrackSpeed,"Track Speed",map.camera.chase.track_speed,FALSE);
	list_palette_add_angle(&property_palette,kMapPropertyCameraChaseSlop,"Slop",&map.camera.chase.slop,FALSE);

		// camera static

	list_palette_add_header(&property_palette,0,"Map Camera Static");
	list_palette_add_checkbox(&property_palette,kMapPropertyCameraStaticFollow,"Follow Player",map.camera.c_static.follow,FALSE);
	list_palette_add_string(&property_palette,kMapPropertyCameraStaticAttachNode,"Attach Node",map.camera.c_static.attach_node,FALSE);

		// media

	list_palette_add_header(&property_palette,0,"Map Media");
	list_palette_add_string(&property_palette,kMapPropertyMediaType,"Startup Type",map_property_media_type_list[map.media.type],FALSE);
	list_palette_add_string(&property_palette,kMapPropertyMediaName,"Startup Name",map.media.name,FALSE);
	list_palette_add_string(&property_palette,kMapPropertyMediaTitleSound,"Startup Title Sound",map.media.title_sound_name,FALSE);

		// music

	list_palette_add_header(&property_palette,0,"Map Music");
	list_palette_add_string(&property_palette,kMapPropertyMusicName,"Name",map.music.name,FALSE);
	list_palette_add_string_int(&property_palette,kMapPropertyMusicFadeTime,"Fade In Time",map.music.fade_msec,FALSE);

		// sky

	list_palette_add_header(&property_palette,0,"Map Sky");
	list_palette_add_checkbox(&property_palette,kMapPropertySkyOn,"On",map.sky.on,FALSE);
	list_palette_add_string(&property_palette,kMapPropertySkyType,"Type",map_property_sky_type_list[map.sky.type],FALSE);
	list_palette_add_string_int(&property_palette,kMapPropertySkyRadius,"Radius",map.sky.radius,FALSE);
	list_palette_add_string_int(&property_palette,kMapPropertyDomeY,"Dome Height",map.sky.dome_y,FALSE);
	list_palette_add_checkbox(&property_palette,kMapPropertyDomeMirror,"Mirror Dome",map.sky.dome_mirror,FALSE);
	list_palette_add_string_float(&property_palette,kMapPropertyTextureRepeat,"Texture Repeat",map.sky.txt_fact,FALSE);
	uv.x=map.sky.txt_x_shift;
	uv.y=map.sky.txt_y_shift;
	list_palette_add_uv(&property_palette,kMapPropertyTextureShift,"Texture Shift",&uv,FALSE);
	list_palette_add_texture(&property_palette,kMapPropertyTextureFill,"Fill/Cube Top Fill",map.sky.fill,FALSE);
	list_palette_add_texture(&property_palette,kMapPropertyTextureBottomFill,"Cube Bottom Fill",map.sky.bottom_fill,FALSE);
	list_palette_add_texture(&property_palette,kMapPropertyTextureNorthFill,"Cube North Fill",map.sky.north_fill,FALSE);
	list_palette_add_texture(&property_palette,kMapPropertyTextureSouthFill,"Cube South Fill",map.sky.south_fill,FALSE);
	list_palette_add_texture(&property_palette,kMapPropertyTextureEastFill,"Cube East Fill",map.sky.east_fill,FALSE);
	list_palette_add_texture(&property_palette,kMapPropertyTextureWestFill,"Cube West Fill",map.sky.west_fill,FALSE);

		// background

	list_palette_add_header(&property_palette,0,"Map Background");
	list_palette_add_checkbox(&property_palette,kMapPropertyBackgroundOn,"On",map.background.on,FALSE);
	uv.x=map.background.front.x_scroll_fact;
	uv.y=map.background.front.y_scroll_fact;
	list_palette_add_uv(&property_palette,kMapPropertyBackgroundFrontShift,"Front Scroll",&uv,FALSE);
	list_palette_add_texture(&property_palette,kMapPropertyBackgroundFrontFill,"Front Fill",map.background.front.fill,FALSE);
	uv.x=map.background.middle.x_scroll_fact;
	uv.y=map.background.middle.y_scroll_fact;
	list_palette_add_uv(&property_palette,kMapPropertyBackgroundMiddleShift,"Middle Scroll",&uv,FALSE);
	list_palette_add_texture(&property_palette,kMapPropertyBackgroundMiddleFill,"Middle Fill",map.background.middle.fill,FALSE);
	uv.x=map.background.back.x_scroll_fact;
	uv.y=map.background.back.y_scroll_fact;
	list_palette_add_uv(&property_palette,kMapPropertyBackgroundBackShift,"Back Scroll",&uv,FALSE);
	list_palette_add_texture(&property_palette,kMapPropertyBackgroundBackFill,"Back Fill",map.background.back.fill,FALSE);




		// rain

	list_palette_add_header(&property_palette,0,"Map Rain");
	list_palette_add_checkbox(&property_palette,kMapPropertyRainOn,"On",map.rain.on,FALSE);
	list_palette_add_string_int(&property_palette,kMapPropertyRainDensity,"Density",map.rain.density,FALSE);
	list_palette_add_string_int(&property_palette,kMapPropertyRainRadius,"Radius",map.rain.radius,FALSE);
	list_palette_add_string_int(&property_palette,kMapPropertyRainHeight,"Height",map.rain.height,FALSE);
	list_palette_add_string_int(&property_palette,kMapPropertyRainSpeed,"Speed",map.rain.speed,FALSE);
	list_palette_add_string_int(&property_palette,kMapPropertyRainWidth,"Line Width",map.rain.line_width,FALSE);
	list_palette_add_string_int(&property_palette,kMapPropertyRainLength,"Line Length",map.rain.line_length,FALSE);
	list_palette_add_string_float(&property_palette,kMapPropertyRainAlpha,"Line Alpha",map.rain.alpha,FALSE);
	list_palette_add_pick_color(&property_palette,kMapPropertyRainStartColor,"Line Start Color",&map.rain.start_color,FALSE);
	list_palette_add_pick_color(&property_palette,kMapPropertyRainEndColor,"Line End Color",&map.rain.end_color,FALSE);
	list_palette_add_string_int(&property_palette,kMapPropertyRainSlantAdd,"Slant Add",map.rain.slant_add,FALSE);
	list_palette_add_string_int(&property_palette,kMapPropertyRainSlantTimeMSec,"Slant Time",map.rain.slant_time_msec,FALSE);
	list_palette_add_string_int(&property_palette,kMapPropertyRainSlantChangeMSec,"Slant Change",map.rain.slant_change_msec,FALSE);

		// parameters

	list_palette_add_header(&property_palette,0,"Map Parameters");
	for (n=0;n!=10;n++) {
		sprintf(name,"Param %d",n);
		property_palette_get_parameter(n,map.settings.params,str);
		list_palette_add_string(&property_palette,(kMapPropertyParamsStart+n),name,str,FALSE);
	}

		// editor setup

	list_palette_add_header(&property_palette,0,"Map Editor Setup");
	uv.x=map.editor_setup.txt_scale_x;
	uv.y=map.editor_setup.txt_scale_y;
	list_palette_add_uv(&property_palette,kMapPropertyEditorTextureFactor,"Default Texture Scale",&uv,FALSE);
	list_palette_add_string_int(&property_palette,kMapPropertyEditorViewNearZ,"Near Z",map.editor_setup.view_near_dist,FALSE);
	list_palette_add_string_int(&property_palette,kMapPropertyEditorViewFarZ,"Far Z",map.editor_setup.view_far_dist,FALSE);
	list_palette_add_checkbox(&property_palette,kMapPropertyEditorLinkStartAlways,"On",map.editor_setup.link_always_start,FALSE);

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

void property_palette_click_map(int id)
{
	int				param_idx,size;
	char			str[256];
	d3fpnt			uv;

		// parameters

	if ((id>=kMapPropertyParamsStart) && (id<=kMapPropertyParamsEnd)) {
		param_idx=(id-kMapPropertyParamsStart);
		
		property_palette_get_parameter(param_idx,map.settings.params,str);
		dialog_property_string_run(list_string_value_string,(void*)str,256,0,0);
		property_palette_set_parameter(param_idx,map.settings.params,str);

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

			// settings

		case kMapPropertyGravity:
			dialog_property_string_run(list_string_value_positive_float,(void*)&map.settings.gravity,0,0,0);
			break;

		case kMapPropertyGravityMaxPower:
			dialog_property_string_run(list_string_value_positive_float,(void*)&map.settings.gravity_max_power,0,0,0);
			break;

		case kMapPropertyGravityMaxSpeed:
			dialog_property_string_run(list_string_value_positive_float,(void*)&map.settings.gravity_max_speed,0,0,0);
			break;

		case kMapPropertyResistance:
			dialog_property_string_run(list_string_value_positive_float,(void*)&map.settings.resistance,0,0,0);
			break;

		case kMapPropertyNeverCull:
			map.settings.never_cull=!map.settings.never_cull;
			break;

		case kMapPropertyDisableShaders:
			map.settings.no_shaders=!map.settings.no_shaders;
			break;

		case kMapPropertyNetworkGameList:
			dialog_property_string_run(list_string_value_string,(void*)map.settings.network_game_list,256,0,0);
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
			dialog_property_string_run(list_string_value_range_int,(void*)&map.light_map.quality,0,50,200);
			break;
			
		case kMapPropertyLightMapSize:
			size=((int)log2(map.light_map.size))-8;
			if ((size<0) || (size>2)) size=0;
			property_palette_pick_list((char*)map_property_light_map_size_list,&size);
			map.light_map.size=(int)pow(2,(size+8));
			break;
			
		case kMapPropertyLightMapBorderCount:
			dialog_property_string_run(list_string_value_range_int,(void*)&map.light_map.pixel_border_count,0,0,10);
			break;
			
		case kMapPropertyLightMapBlurCount:
			dialog_property_string_run(list_string_value_range_int,(void*)&map.light_map.blur_count,0,0,6);
			break;

			// camera settings

		case kMapPropertyCameraMode:
			property_palette_pick_list((char*)map_property_camera_mode_list,&map.camera.mode);
			break;
			
		case kMapPropertyCameraAngle:
			dialog_property_string_run(list_string_value_angle,(void*)&map.camera.ang,0,0,0);
			break;

			// camera plane
			
		case kMapPropertyCameraFOV:
			dialog_property_string_run(list_string_value_positive_float,(void*)&map.camera.plane.fov,0,0,0);
			break;

		case kMapPropertyCameraAspectRatio:
			dialog_property_string_run(list_string_value_positive_float,(void*)&map.camera.plane.aspect_ratio,0,0,0);
			break;

		case kMapPropertyCameraNearZ:
			dialog_property_string_run(list_string_value_positive_int,(void*)&map.camera.plane.near_z,0,0,0);
			break;

		case kMapPropertyCameraFarZ:
			dialog_property_string_run(list_string_value_positive_int,(void*)&map.camera.plane.far_z,0,0,0);
			break;

		case kMapPropertyCameraNearZOffset:
			dialog_property_string_run(list_string_value_int,(void*)&map.camera.plane.near_z_offset,0,0,0);
			break;

			// camera chase settings

		case kMapPropertyCameraChaseDistance:
			dialog_property_string_run(list_string_value_positive_int,(void*)&map.camera.chase.distance,0,0,0);
			break;

		case kMapPropertyCameraChaseTrackSpeed:
			dialog_property_string_run(list_string_value_positive_float,(void*)&map.camera.chase.track_speed,0,0,0);
			break;

		case kMapPropertyCameraChaseSlop:
			dialog_property_string_run(list_string_value_angle,(void*)&map.camera.chase.slop,0,0,0);
			break;

			// camera static settings

		case kMapPropertyCameraStaticFollow:
			map.camera.c_static.follow=!map.camera.c_static.follow;
			break;

		case kMapPropertyCameraStaticAttachNode:
			property_palette_pick_node(map.camera.c_static.attach_node);
			break;
	
			// map media

		case kMapPropertyMediaType:
			property_palette_pick_list((char*)map_property_media_type_list,&map.media.type);
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

			// map sky

		case kMapPropertySkyOn:
			map.sky.on=!map.sky.on;
			break;

		case kMapPropertySkyType:
			property_palette_pick_list((char*)map_property_sky_type_list,&map.sky.type);
			break;

		case kMapPropertySkyRadius:
			dialog_property_string_run(list_string_value_positive_int,(void*)&map.sky.radius,0,0,0);
			break;

		case kMapPropertyDomeY:
			dialog_property_string_run(list_string_value_positive_int,(void*)&map.sky.dome_y,0,0,0);
			break;

		case kMapPropertyDomeMirror:
			map.sky.dome_mirror=!map.sky.dome_mirror;
			break;

		case kMapPropertyTextureRepeat:
			dialog_property_string_run(list_string_value_positive_float,(void*)&map.sky.txt_fact,0,0,0);
			break;

		case kMapPropertyTextureShift:
			uv.x=map.sky.txt_x_shift;
			uv.y=map.sky.txt_y_shift;
			dialog_property_string_run(list_string_value_uv,(void*)&uv,0,0,0);
			map.sky.txt_x_shift=uv.x;
			map.sky.txt_y_shift=uv.y;
			break;

		case kMapPropertyTextureFill:
			property_palette_pick_texture(&map.sky.fill);
			break;

		case kMapPropertyTextureBottomFill:
			property_palette_pick_texture(&map.sky.bottom_fill);
			break;

		case kMapPropertyTextureNorthFill:
			property_palette_pick_texture(&map.sky.north_fill);
			break;

		case kMapPropertyTextureSouthFill:
			property_palette_pick_texture(&map.sky.south_fill);
			break;

		case kMapPropertyTextureEastFill:
			property_palette_pick_texture(&map.sky.east_fill);
			break;

		case kMapPropertyTextureWestFill:
			property_palette_pick_texture(&map.sky.west_fill);
			break;

			// map background

		case kMapPropertyBackgroundOn:
			map.background.on=!map.background.on;
			break;

		case kMapPropertyBackgroundFrontShift:
			uv.x=map.background.front.x_scroll_fact;
			uv.y=map.background.front.y_scroll_fact;
			dialog_property_string_run(list_string_value_uv,(void*)&uv,0,0,0);
			map.background.front.x_scroll_fact=uv.x;
			map.background.front.y_scroll_fact=uv.y;
			break;

		case kMapPropertyBackgroundFrontFill:
			property_palette_pick_texture(&map.background.front.fill);
			break;

		case kMapPropertyBackgroundMiddleShift:
			uv.x=map.background.middle.x_scroll_fact;
			uv.y=map.background.middle.y_scroll_fact;
			dialog_property_string_run(list_string_value_uv,(void*)&uv,0,0,0);
			map.background.middle.x_scroll_fact=uv.x;
			map.background.middle.y_scroll_fact=uv.y;
			break;

		case kMapPropertyBackgroundMiddleFill:
			property_palette_pick_texture(&map.background.middle.fill);
			break;

		case kMapPropertyBackgroundBackShift:
			uv.x=map.background.back.x_scroll_fact;
			uv.y=map.background.back.y_scroll_fact;
			dialog_property_string_run(list_string_value_uv,(void*)&uv,0,0,0);
			map.background.back.x_scroll_fact=uv.x;
			map.background.back.y_scroll_fact=uv.y;
			break;

		case kMapPropertyBackgroundBackFill:
			property_palette_pick_texture(&map.background.back.fill);
			break;



		// rain

		case kMapPropertyRainOn:
			map.rain.on=!map.rain.on;
			break;

		case kMapPropertyRainDensity:
			dialog_property_string_run(list_string_value_positive_int,(void*)&map.rain.density,0,0,0);
			break;

		case kMapPropertyRainRadius:
			dialog_property_string_run(list_string_value_positive_int,(void*)&map.rain.radius,0,0,0);
			break;

		case kMapPropertyRainHeight:
			dialog_property_string_run(list_string_value_positive_int,(void*)&map.rain.height,0,0,0);
			break;

		case kMapPropertyRainSpeed:
			dialog_property_string_run(list_string_value_positive_int,(void*)&map.rain.speed,0,0,0);
			break;

		case kMapPropertyRainWidth:
			dialog_property_string_run(list_string_value_positive_int,(void*)&map.rain.line_width,0,0,0);
			break;

		case kMapPropertyRainLength:
			dialog_property_string_run(list_string_value_positive_int,(void*)&map.rain.line_length,0,0,0);
			break;

		case kMapPropertyRainAlpha:
			dialog_property_string_run(list_string_value_0_to_1_float,(void*)&map.rain.alpha,0,0,0);
			break;

		case kMapPropertyRainStartColor:
			os_pick_color(&map.rain.start_color);
			break;

		case kMapPropertyRainEndColor:
			os_pick_color(&map.rain.end_color);
			break;

		case kMapPropertyRainSlantAdd:
			dialog_property_string_run(list_string_value_positive_int,(void*)&map.rain.slant_add,0,0,0);
			break;

		case kMapPropertyRainSlantTimeMSec:
			dialog_property_string_run(list_string_value_positive_int,(void*)&map.rain.slant_time_msec,0,0,0);
			break;

		case kMapPropertyRainSlantChangeMSec:
			dialog_property_string_run(list_string_value_positive_int,(void*)&map.rain.slant_change_msec,0,0,0);
			break;

			// editor setup

		case kMapPropertyEditorTextureFactor:
			uv.x=map.editor_setup.txt_scale_x;
			uv.y=map.editor_setup.txt_scale_y;
			dialog_property_string_run(list_string_value_uv,(void*)&uv,0,0,0);
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

