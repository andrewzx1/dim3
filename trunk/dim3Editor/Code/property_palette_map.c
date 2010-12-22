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

#define kMapPropertyOn						34
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



/*
typedef struct		{
						int									fill;
						float								x_scroll_fact,y_scroll_fact;
					} map_background_layer_type;					
					
typedef struct		{
						bool								on;
						map_background_layer_type			front,middle,back;
					} map_background_type;					
					
*/

void property_palette_fill_map(void)
{
	int				size;
	d3fpnt			uv;
	
	list_palette_add_header(&property_palette,0,"Map Info");
	list_palette_add_string(&property_palette,-1,"Name",map.info.name,TRUE);
	list_palette_add_string(&property_palette,kMapPropertyInfoTitle,"Title",map.info.title,FALSE);
	list_palette_add_string(&property_palette,kMapPropertyInfoAuthor,"Author",map.info.author,FALSE);

	list_palette_add_header(&property_palette,0,"Map Settings");
	list_palette_add_string_float(&property_palette,kMapPropertyGravity,"Gravity",map.settings.gravity,FALSE);
	list_palette_add_string_float(&property_palette,kMapPropertyGravityMaxPower,"Gravity Max Power",map.settings.gravity_max_power,FALSE);
	list_palette_add_string_float(&property_palette,kMapPropertyGravityMaxSpeed,"Gravity Max Speed",map.settings.gravity_max_speed,FALSE);
	list_palette_add_string_float(&property_palette,kMapPropertyResistance,"Resistance",map.settings.resistance,FALSE);
	list_palette_add_checkbox(&property_palette,kMapPropertyNeverCull,"Never Cull",map.settings.never_cull,FALSE);
	list_palette_add_checkbox(&property_palette,kMapPropertyDisableShaders,"Disable Shaders",map.settings.no_shaders,FALSE);
	list_palette_add_string(&property_palette,kMapPropertyNetworkGameList,"Net Game List",map.settings.network_game_list,FALSE);

	list_palette_add_header(&property_palette,0,"Map Ambient");
	list_palette_add_pick_color(&property_palette,kMapPropertyAmbientColor,"Color",&map.ambient.light_color,FALSE);
	list_palette_add_string_float(&property_palette,kMapPropertyAmbientLightMapBoost,"Light Map Boost",map.ambient.light_map_boost,FALSE);
	list_palette_add_string(&property_palette,kMapPropertyAmbientSound,"Sound",map.ambient.sound_name,FALSE);
	list_palette_add_string_float(&property_palette,kMapPropertyAmbientSoundPitch,"Sound Pitch",map.ambient.sound_pitch,FALSE);

	size=((int)log2(map.light_map.size))-8;
	if ((size<0) || (size>2)) size=0;

	list_palette_add_header(&property_palette,0,"Map Light Map");
	list_palette_add_string_int(&property_palette,kMapPropertyLightMapQuality,"Quality",map.light_map.quality,FALSE);
	list_palette_add_string(&property_palette,kMapPropertyLightMapSize,"Texture Size",map_property_light_map_size_list[size],FALSE);
	list_palette_add_string_int(&property_palette,kMapPropertyLightMapBorderCount,"Pixel Border Count",map.light_map.pixel_border_count,FALSE);
	list_palette_add_string_int(&property_palette,kMapPropertyLightMapBlurCount,"Blur Count",map.light_map.blur_count,FALSE);

	list_palette_add_header(&property_palette,0,"Map Camera Settings");
	list_palette_add_string(&property_palette,kMapPropertyCameraMode,"Mode",map_property_camera_mode_list[map.camera.mode],FALSE);
	list_palette_add_angle(&property_palette,kMapPropertyCameraAngle,"Offset Angle",&map.camera.ang,FALSE);

	list_palette_add_header(&property_palette,0,"Map Camera Plane");
	list_palette_add_string_float(&property_palette,kMapPropertyCameraFOV,"FOV",map.camera.plane.fov,FALSE);
	list_palette_add_string_float(&property_palette,kMapPropertyCameraAspectRatio,"Aspect Ratio",map.camera.plane.aspect_ratio,FALSE);
	list_palette_add_string_int(&property_palette,kMapPropertyCameraNearZ,"Near Z",map.camera.plane.near_z,FALSE);
	list_palette_add_string_int(&property_palette,kMapPropertyCameraFarZ,"Far Z",map.camera.plane.far_z,FALSE);
	list_palette_add_string_int(&property_palette,kMapPropertyCameraNearZOffset,"Near Z Offset",map.camera.plane.near_z_offset,FALSE);

	list_palette_add_header(&property_palette,0,"Map Camera Chase");
	list_palette_add_string_int(&property_palette,kMapPropertyCameraChaseDistance,"Distance",map.camera.chase.distance,FALSE);
	list_palette_add_string_float(&property_palette,kMapPropertyCameraChaseTrackSpeed,"Track Speed",map.camera.chase.track_speed,FALSE);
	list_palette_add_angle(&property_palette,kMapPropertyCameraChaseSlop,"Slop",&map.camera.chase.slop,FALSE);

	list_palette_add_header(&property_palette,0,"Map Camera Static");
	list_palette_add_checkbox(&property_palette,kMapPropertyCameraStaticFollow,"Follow Player",map.camera.c_static.follow,FALSE);
	list_palette_add_string(&property_palette,kMapPropertyCameraStaticAttachNode,"Attach Node",map.camera.c_static.attach_node,FALSE);

	list_palette_add_header(&property_palette,0,"Map Media");
	list_palette_add_string(&property_palette,kMapPropertyMediaType,"Startup Type",map_property_media_type_list[map.media.type],FALSE);
	list_palette_add_string(&property_palette,kMapPropertyMediaName,"Startup Name",map.media.name,FALSE);
	list_palette_add_string(&property_palette,kMapPropertyMediaTitleSound,"Startup Title Sound",map.media.title_sound_name,FALSE);

	list_palette_add_header(&property_palette,0,"Map Music");
	list_palette_add_string(&property_palette,kMapPropertyMusicName,"Name",map.music.name,FALSE);
	list_palette_add_string_int(&property_palette,kMapPropertyMusicFadeTime,"Fade In Time",map.music.fade_msec,FALSE);

	list_palette_add_header(&property_palette,0,"Map Sky");
	list_palette_add_checkbox(&property_palette,kMapPropertyOn,"On",map.sky.on,FALSE);
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
}

/* =======================================================

      Property Palette Click Map
      
======================================================= */

void property_palette_click_map(int id)
{
	int				size;
	d3fpnt			uv;
	
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

		case kMapPropertyOn:
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

	}

	main_wind_draw();
}

