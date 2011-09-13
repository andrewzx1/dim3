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

#define kMapPropertyAmbientColor			0
#define kMapPropertyAmbientLightMapBoost	1
#define kMapPropertyAmbientSound			2
#define kMapPropertyAmbientSoundPitch		3

#define kMapPropertyLightMapQuality			10
#define kMapPropertyLightMapSize			11
#define kMapPropertyLightMapUseNormals		12
#define kMapPropertyLightMapSkipGlows		13
#define kMapPropertyLightMapDiffuseBoost	14

#define kMapPropertyMediaType				20
#define kMapPropertyMediaEventId			21
#define kMapPropertyMediaName				22
#define kMapPropertyMediaTitleSound			23

#define kMapPropertyMusicName				30
#define kMapPropertyMusicFadeTime			31
#define kMapPropertyMusicPreloadName		32

extern map_type					map;
extern editor_state_type		state;
extern editor_setup_type		setup;

extern list_palette_type		property_palette;

char							map_property_light_map_size_list[][name_str_len]={"256","512","1024",""},
								map_property_media_type_list[][name_str_len]={"None","Chooser","Title","Cinema",""},
								map_property_light_map_quality_list[][name_str_len]={"Very Low","Low","Medium","High","Very High",""};

/* =======================================================

      Property Palette Fill Light Media
      
======================================================= */

void property_palette_fill_light_media(void)
{
	int				n,size;
	char			name[256];

	list_palette_set_title(&property_palette,"Map Light Media Settings");

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
	list_palette_add_checkbox(&property_palette,kMapPropertyLightMapUseNormals,"Use Normals (Hard Edges)",map.light_map.use_normals,FALSE);
	list_palette_add_checkbox(&property_palette,kMapPropertyLightMapSkipGlows,"Skip Glow Mapped Polys",map.light_map.skip_glows,FALSE);
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
}

/* =======================================================

      Property Palette Click Light Media
      
======================================================= */

void property_palette_click_light_media(int id,bool double_click)
{
	int				music_idx,size;

	if (!double_click) return;

		// preload music

	if ((id>=kMapPropertyMusicPreloadName) && (id<(kMapPropertyMusicPreloadName+max_music_preload))) {
		music_idx=(id-kMapPropertyMusicPreloadName);
		dialog_property_string_run(list_string_value_string,(void*)map.music.preload_name[music_idx],name_str_len,0,0);
		main_wind_draw();
		return;
	}
	
		// regular properties

	switch (id) {

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
			
		case kMapPropertyLightMapUseNormals:
			map.light_map.use_normals=!map.light_map.use_normals;
			break;
			
		case kMapPropertyLightMapSkipGlows:
			map.light_map.skip_glows=!map.light_map.skip_glows;
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

	}

	main_wind_draw();
}

