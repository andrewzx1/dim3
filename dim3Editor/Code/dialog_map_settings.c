/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Map Setting Dialog

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

#include "interface.h"
#include "dialog.h"

extern map_type				map;

#define kMapSettingTabCount							8
#define kMapSettingTab								FOUR_CHAR_CODE('tabb')

	// general
	
#define kMapTitle									FOUR_CHAR_CODE('tite')
#define kMapAuthor									FOUR_CHAR_CODE('auth')
#define kMapMeshCount								FOUR_CHAR_CODE('mcnt')
#define kMapPolyCount								FOUR_CHAR_CODE('pcnt')
#define kMapGravity									FOUR_CHAR_CODE('grav')
#define kMapGravityPower							FOUR_CHAR_CODE('gpwr')
#define kMapGravitySpeed							FOUR_CHAR_CODE('gspd')
#define kMapResistance								FOUR_CHAR_CODE('rest')
#define kMapNeverCull								FOUR_CHAR_CODE('ncul')
#define kMapNoShaders								FOUR_CHAR_CODE('nshd')
#define kMapGameTypeList							FOUR_CHAR_CODE('gtyp')

	// ambients
	
#define kMapLightColor								FOUR_CHAR_CODE('camb')
#define kMapLightMapBoost							FOUR_CHAR_CODE('lmbs')
#define kMapSoundName								FOUR_CHAR_CODE('samb')
#define kMapSoundPitch								FOUR_CHAR_CODE('pith')

	// media
	
#define kMapMediaOpenType							FOUR_CHAR_CODE('otyp')
#define kMapMediaOpenName							FOUR_CHAR_CODE('onme')
#define kMapMediaTitleSoundName						FOUR_CHAR_CODE('otsd')
#define kMapMediaMusicName							FOUR_CHAR_CODE('musc')
#define kMapMediaMusicFadeIn						FOUR_CHAR_CODE('mfdi')

	// camera
	
#define kMapCameraMode								FOUR_CHAR_CODE('cmde')
#define kMapCameraFOV								FOUR_CHAR_CODE('cfov')
#define kMapCameraAspectRatio						FOUR_CHAR_CODE('cart')
#define kMapCameraNearZ								FOUR_CHAR_CODE('cnrz')
#define kMapCameraFarZ								FOUR_CHAR_CODE('cfrz')
#define kMapCameraNearOffsetZ						FOUR_CHAR_CODE('cnoz')
#define kMapCameraAngleX							FOUR_CHAR_CODE('cagx')
#define kMapCameraAngleY							FOUR_CHAR_CODE('cagy')
#define kMapCameraAngleZ							FOUR_CHAR_CODE('cagz')
#define kMapCameraDistance							FOUR_CHAR_CODE('cdst')
#define kMapCameraTrackSpeed						FOUR_CHAR_CODE('ctsp')
#define kMapCameraSlopX								FOUR_CHAR_CODE('cslx')
#define kMapCameraSlopY								FOUR_CHAR_CODE('csly')
#define kMapCameraSlopZ								FOUR_CHAR_CODE('cslz')
#define kMapCameraAttachNode						FOUR_CHAR_CODE('cand')
#define kMapCameraFollowCameraObject				FOUR_CHAR_CODE('cfco')

	// sky
	
#define kSkySettingSkyOn							FOUR_CHAR_CODE('syon')
#define kSkySettingSkyType							FOUR_CHAR_CODE('syty')
#define kSkySettingSkyTextureIndex					FOUR_CHAR_CODE('syti')
#define kSkySettingSkyTextureBottomIndex			FOUR_CHAR_CODE('stxb')
#define kSkySettingSkyTextureNorthIndex				FOUR_CHAR_CODE('stxn')
#define kSkySettingSkyTextureSouthIndex				FOUR_CHAR_CODE('stxs')
#define kSkySettingSkyTextureEastIndex				FOUR_CHAR_CODE('stxe')
#define kSkySettingSkyTextureWestIndex				FOUR_CHAR_CODE('stxw')
#define kSkySettingSkyRadius						FOUR_CHAR_CODE('syrd')
#define kSkySettingSkyDomeY							FOUR_CHAR_CODE('sdmy')
#define kSkySettingSkyDomeMirror					FOUR_CHAR_CODE('smdm')
#define kSkySettingSkyTextureRepeat					FOUR_CHAR_CODE('sytr')
#define kSkySettingSkyTextureXShift					FOUR_CHAR_CODE('stxx')
#define kSkySettingSkyTextureYShift					FOUR_CHAR_CODE('styy')

	// background
	
#define kBackSettingBackgroundOn					FOUR_CHAR_CODE('bgon')
#define kBackSettingBackgroundTextureIndex			FOUR_CHAR_CODE('bgti')
#define kBackSettingBackgroundXShift				FOUR_CHAR_CODE('bgxs')
#define kBackSettingBackgroundYShift				FOUR_CHAR_CODE('bgys')

	// fog
	
#define kFogOn										FOUR_CHAR_CODE('fgon')
#define kFogCount									FOUR_CHAR_CODE('fcnt')
#define kFogInnerRadius								FOUR_CHAR_CODE('fird')
#define kFogOuterRadius								FOUR_CHAR_CODE('ford')
#define kFogHigh									FOUR_CHAR_CODE('fhgh')
#define kFogDrop									FOUR_CHAR_CODE('fdrp')
#define kFogTextureIndex							FOUR_CHAR_CODE('fgtx')
#define kFogTextureSpeed							FOUR_CHAR_CODE('fgss')
#define kFogTextureXFact							FOUR_CHAR_CODE('fxsz')
#define kFogTextureYFact							FOUR_CHAR_CODE('fysz')
#define kFogColor									FOUR_CHAR_CODE('fclr')
#define kFogAlpha									FOUR_CHAR_CODE('fala')
#define kFogUseSolidColor							FOUR_CHAR_CODE('fscl')

	// rain
	
#define kRainDensity								FOUR_CHAR_CODE('dnst')
#define kRainRadius									FOUR_CHAR_CODE('rads')
#define kRainHeight									FOUR_CHAR_CODE('high')
#define kRainSpeed									FOUR_CHAR_CODE('sped')
#define kRainWidth									FOUR_CHAR_CODE('widt')
#define kRainLength									FOUR_CHAR_CODE('leng')
#define kRainAlpha									FOUR_CHAR_CODE('apha')
#define kRainStartColor								FOUR_CHAR_CODE('rnsc')
#define kRainEndColor								FOUR_CHAR_CODE('rnec')
#define kRainSlantAdd								FOUR_CHAR_CODE('slnt')
#define kRainSlantTimeMSec							FOUR_CHAR_CODE('slms')
#define kRainSlantChangeMSec						FOUR_CHAR_CODE('slct')
#define kRainOn										FOUR_CHAR_CODE('rnon')

	// editor
	
#define kMapTextureXFactor							FOUR_CHAR_CODE('trxf')
#define kMapTextureYFactor							FOUR_CHAR_CODE('tryf')
#define kMapEditorViewNearZ							FOUR_CHAR_CODE('enrz')
#define kMapEditorViewFarZ							FOUR_CHAR_CODE('efrz')
#define kMapEditorLinkStartAlways					FOUR_CHAR_CODE('elss')

bool						dialog_map_settings_cancel;
WindowRef					dialog_map_settings_wind;

/* =======================================================

      Map Setting Setting Event Handlers
      
======================================================= */

static pascal OSStatus map_map_setting_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
				
				case kHICommandCancel:
					dialog_map_settings_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_map_settings_wind);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_map_settings_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

static pascal OSStatus map_map_setting_tab_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	dialog_switch_tab(dialog_map_settings_wind,kMapSettingTab,0,kMapSettingTabCount);
	return(eventNotHandledErr);
}

/* =======================================================

      Run Map Setting
      
======================================================= */

bool dialog_map_settings_run(void)
{
	int						n,nmesh,npoly;
	ControlRef				ctrl;
	ControlID				ctrl_id;
	EventHandlerUPP			event_upp,tab_event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}},
							tab_event_list[]={{kEventClassControl,kEventControlHit}};
	
		// open the dialog
		
	dialog_open(&dialog_map_settings_wind,"MapSettings");
	
		// tab
		
	dialog_set_tab(dialog_map_settings_wind,kMapSettingTab,0,0,kMapSettingTabCount);
	
	ctrl_id.signature=kMapSettingTab;
	ctrl_id.id=0;
	GetControlByID(dialog_map_settings_wind,&ctrl_id,&ctrl);
	
	tab_event_upp=NewEventHandlerUPP(map_map_setting_tab_proc);
	InstallControlEventHandler(ctrl,tab_event_upp,GetEventTypeCount(tab_event_list),tab_event_list,dialog_map_settings_wind,NULL);

		// set general contrls
		
	dialog_set_text(dialog_map_settings_wind,kMapTitle,0,map.info.title);
	dialog_set_text(dialog_map_settings_wind,kMapAuthor,0,map.info.author);
	dialog_set_float(dialog_map_settings_wind,kMapGravity,0,map.settings.gravity);
	dialog_set_float(dialog_map_settings_wind,kMapGravityPower,0,map.settings.gravity_max_power);
	dialog_set_float(dialog_map_settings_wind,kMapGravitySpeed,0,map.settings.gravity_max_speed);
	dialog_set_float(dialog_map_settings_wind,kMapResistance,0,map.settings.resistance);
	dialog_set_boolean(dialog_map_settings_wind,kMapNeverCull,0,map.settings.never_cull);
	dialog_set_boolean(dialog_map_settings_wind,kMapNoShaders,0,map.settings.no_shaders);

	dialog_set_text(dialog_map_settings_wind,kMapGameTypeList,0,map.settings.network_game_list);
	
		// set ambient controls
		
	dialog_set_tab(dialog_map_settings_wind,kMapSettingTab,0,1,kMapSettingTabCount);
	dialog_set_color(dialog_map_settings_wind,kMapLightColor,0,&map.ambient.light_color);
	dialog_set_float(dialog_map_settings_wind,kMapLightMapBoost,0,map.ambient.light_map_boost);
	dialog_special_combo_fill_sound(dialog_map_settings_wind,kMapSoundName,0,map.ambient.sound_name);
	dialog_set_float(dialog_map_settings_wind,kMapSoundPitch,0,map.ambient.sound_pitch);
	dialog_set_tab(dialog_map_settings_wind,kMapSettingTab,0,0,kMapSettingTabCount);
	
		// set media controls
		
	dialog_set_combo(dialog_map_settings_wind,kMapMediaOpenType,0,map.media.type);
	dialog_set_text(dialog_map_settings_wind,kMapMediaOpenName,0,map.media.name);
	dialog_special_combo_fill_sound(dialog_map_settings_wind,kMapMediaTitleSoundName,0,map.media.title_sound_name);
	dialog_set_text(dialog_map_settings_wind,kMapMediaMusicName,0,map.music.name);
	dialog_set_int(dialog_map_settings_wind,kMapMediaMusicFadeIn,0,map.music.fade_msec);
	
		// camera
		
	dialog_set_combo(dialog_map_settings_wind,kMapCameraMode,0,map.camera.mode);
	dialog_set_float(dialog_map_settings_wind,kMapCameraFOV,0,map.camera.plane.fov);
	dialog_set_float(dialog_map_settings_wind,kMapCameraAspectRatio,0,map.camera.plane.aspect_ratio);
	dialog_set_int(dialog_map_settings_wind,kMapCameraNearZ,0,map.camera.plane.near_z);
	dialog_set_int(dialog_map_settings_wind,kMapCameraFarZ,0,map.camera.plane.far_z);
	dialog_set_int(dialog_map_settings_wind,kMapCameraNearOffsetZ,0,map.camera.plane.near_z_offset);
	
	dialog_set_float(dialog_map_settings_wind,kMapCameraAngleX,0,map.camera.ang.x);
	dialog_set_float(dialog_map_settings_wind,kMapCameraAngleY,0,map.camera.ang.y);
	dialog_set_float(dialog_map_settings_wind,kMapCameraAngleZ,0,map.camera.ang.z);

	dialog_set_int(dialog_map_settings_wind,kMapCameraDistance,0,map.camera.chase.distance);
	dialog_set_float(dialog_map_settings_wind,kMapCameraTrackSpeed,0,map.camera.chase.track_speed);
	dialog_set_float(dialog_map_settings_wind,kMapCameraSlopX,0,map.camera.chase.slop.x);
	dialog_set_float(dialog_map_settings_wind,kMapCameraSlopY,0,map.camera.chase.slop.y);
	dialog_set_float(dialog_map_settings_wind,kMapCameraSlopZ,0,map.camera.chase.slop.z);
	
	dialog_special_combo_fill_node(dialog_map_settings_wind,kMapCameraAttachNode,0,map.camera.c_static.attach_node);
	dialog_set_boolean(dialog_map_settings_wind,kMapCameraFollowCameraObject,0,map.camera.c_static.follow);

		// set sky controls
	
	dialog_set_boolean(dialog_map_settings_wind,kSkySettingSkyOn,0,map.sky.on);
	dialog_set_combo(dialog_map_settings_wind,kSkySettingSkyType,0,map.sky.type);
	dialog_fill_texture_combo(dialog_map_settings_wind,kSkySettingSkyTextureIndex,0,TRUE,map.sky.fill);
	dialog_fill_texture_combo(dialog_map_settings_wind,kSkySettingSkyTextureBottomIndex,0,TRUE,map.sky.bottom_fill);
	dialog_fill_texture_combo(dialog_map_settings_wind,kSkySettingSkyTextureNorthIndex,0,TRUE,map.sky.north_fill);
	dialog_fill_texture_combo(dialog_map_settings_wind,kSkySettingSkyTextureSouthIndex,0,TRUE,map.sky.south_fill);
	dialog_fill_texture_combo(dialog_map_settings_wind,kSkySettingSkyTextureEastIndex,0,TRUE,map.sky.east_fill);
	dialog_fill_texture_combo(dialog_map_settings_wind,kSkySettingSkyTextureWestIndex,0,TRUE,map.sky.west_fill);
	dialog_set_int(dialog_map_settings_wind,kSkySettingSkyRadius,0,map.sky.radius);
	dialog_set_int(dialog_map_settings_wind,kSkySettingSkyDomeY,0,map.sky.dome_y);
	dialog_set_boolean(dialog_map_settings_wind,kSkySettingSkyDomeMirror,0,map.sky.dome_mirror);
	dialog_set_float(dialog_map_settings_wind,kSkySettingSkyTextureRepeat,0,map.sky.txt_fact);
	dialog_set_float(dialog_map_settings_wind,kSkySettingSkyTextureXShift,0,map.sky.txt_x_shift);
	dialog_set_float(dialog_map_settings_wind,kSkySettingSkyTextureYShift,0,map.sky.txt_y_shift);

		// set background controls
		
	dialog_set_boolean(dialog_map_settings_wind,kBackSettingBackgroundOn,0,map.background.on);
	dialog_fill_texture_combo(dialog_map_settings_wind,kBackSettingBackgroundTextureIndex,0,TRUE,map.background.back.fill);
	dialog_set_float(dialog_map_settings_wind,kBackSettingBackgroundXShift,0,map.background.back.x_scroll_fact);
	dialog_set_float(dialog_map_settings_wind,kBackSettingBackgroundYShift,0,map.background.back.y_scroll_fact);
	dialog_fill_texture_combo(dialog_map_settings_wind,kBackSettingBackgroundTextureIndex,1,TRUE,map.background.middle.fill);
	dialog_set_float(dialog_map_settings_wind,kBackSettingBackgroundXShift,1,map.background.middle.x_scroll_fact);
	dialog_set_float(dialog_map_settings_wind,kBackSettingBackgroundYShift,1,map.background.middle.y_scroll_fact);
	dialog_fill_texture_combo(dialog_map_settings_wind,kBackSettingBackgroundTextureIndex,2,TRUE,map.background.front.fill);
	dialog_set_float(dialog_map_settings_wind,kBackSettingBackgroundXShift,2,map.background.front.x_scroll_fact);
	dialog_set_float(dialog_map_settings_wind,kBackSettingBackgroundYShift,2,map.background.front.y_scroll_fact);
	
		// set fog controls
		
	dialog_set_boolean(dialog_map_settings_wind,kFogOn,0,map.fog.on);
	dialog_set_int(dialog_map_settings_wind,kFogCount,0,map.fog.count);
	dialog_set_int(dialog_map_settings_wind,kFogInnerRadius,0,map.fog.inner_radius);
	dialog_set_int(dialog_map_settings_wind,kFogOuterRadius,0,map.fog.outer_radius);
	dialog_set_int(dialog_map_settings_wind,kFogHigh,0,map.fog.high);
	dialog_set_int(dialog_map_settings_wind,kFogDrop,0,map.fog.drop);

	dialog_fill_texture_combo(dialog_map_settings_wind,kFogTextureIndex,0,FALSE,map.fog.texture_idx);
	dialog_set_float(dialog_map_settings_wind,kFogTextureSpeed,0,map.fog.speed);
	dialog_set_float(dialog_map_settings_wind,kFogTextureXFact,0,map.fog.txt_x_fact);
	dialog_set_float(dialog_map_settings_wind,kFogTextureYFact,0,map.fog.txt_y_fact);
	
	dialog_set_color(dialog_map_settings_wind,kFogColor,0,&map.fog.col);

	dialog_set_boolean(dialog_map_settings_wind,kFogUseSolidColor,0,map.fog.use_solid_color);
	dialog_set_float(dialog_map_settings_wind,kFogAlpha,0,map.fog.alpha);

		// set rain controls
		
	dialog_set_value(dialog_map_settings_wind,kRainDensity,0,map.rain.density);
	dialog_set_int(dialog_map_settings_wind,kRainRadius,0,map.rain.radius);
	dialog_set_int(dialog_map_settings_wind,kRainHeight,0,map.rain.height);
	dialog_set_int(dialog_map_settings_wind,kRainSpeed,0,map.rain.speed);
	dialog_set_int(dialog_map_settings_wind,kRainWidth,0,map.rain.line_width);
	dialog_set_int(dialog_map_settings_wind,kRainLength,0,map.rain.line_length);
	dialog_set_float(dialog_map_settings_wind,kRainAlpha,0,map.rain.alpha);

	dialog_set_int(dialog_map_settings_wind,kRainSlantAdd,0,map.rain.slant_add);
	dialog_set_int(dialog_map_settings_wind,kRainSlantTimeMSec,0,map.rain.slant_time_msec);
	dialog_set_int(dialog_map_settings_wind,kRainSlantChangeMSec,0,map.rain.slant_change_msec);

	dialog_set_color(dialog_map_settings_wind,kRainStartColor,0,&map.rain.start_color);
	dialog_set_color(dialog_map_settings_wind,kRainEndColor,0,&map.rain.end_color);
	
	dialog_set_boolean(dialog_map_settings_wind,kRainOn,0,map.rain.on);
	
		// editor
		
	dialog_set_float(dialog_map_settings_wind,kMapTextureXFactor,0,map.settings.editor.txt_scale_x);
	dialog_set_float(dialog_map_settings_wind,kMapTextureYFactor,0,map.settings.editor.txt_scale_y);
	
	dialog_set_int(dialog_map_settings_wind,kMapEditorViewNearZ,0,map.settings.editor.view_near_dist);
	dialog_set_int(dialog_map_settings_wind,kMapEditorViewFarZ,0,map.settings.editor.view_far_dist);
	
	dialog_set_boolean(dialog_map_settings_wind,kMapEditorLinkStartAlways,0,map.settings.editor.link_always_start);

		// counts
		
	nmesh=map.mesh.nmesh;
	
	npoly=0;
	
	for (n=0;n!=map.mesh.nmesh;n++) {
		npoly+=map.mesh.meshes[n].npoly;
	}
	
	dialog_set_int(dialog_map_settings_wind,kMapMeshCount,0,nmesh);
	dialog_set_int(dialog_map_settings_wind,kMapPolyCount,0,npoly);
	
		// show window
	
	ShowWindow(dialog_map_settings_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(map_map_setting_event_proc);
	InstallWindowEventHandler(dialog_map_settings_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_map_settings_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_map_settings_wind);
	
		// dialog to data
		
	if (!dialog_map_settings_cancel) {
	
			// general controls
			
		dialog_get_text(dialog_map_settings_wind,kMapTitle,0,map.info.title,name_str_len);
		dialog_get_text(dialog_map_settings_wind,kMapAuthor,0,map.info.author,name_str_len);
		map.settings.gravity=dialog_get_float(dialog_map_settings_wind,kMapGravity,0);
		map.settings.gravity_max_power=dialog_get_float(dialog_map_settings_wind,kMapGravityPower,0);
		map.settings.gravity_max_speed=dialog_get_float(dialog_map_settings_wind,kMapGravitySpeed,0);
		map.settings.resistance=dialog_get_float(dialog_map_settings_wind,kMapResistance,0);
		map.settings.never_cull=dialog_get_boolean(dialog_map_settings_wind,kMapNeverCull,0);
		map.settings.no_shaders=dialog_get_boolean(dialog_map_settings_wind,kMapNoShaders,0);

		dialog_get_text(dialog_map_settings_wind,kMapGameTypeList,0,map.settings.network_game_list,256);
		
			// ambient controls
			
		dialog_get_color(dialog_map_settings_wind,kMapLightColor,0,&map.ambient.light_color);
		map.ambient.light_map_boost=dialog_get_float(dialog_map_settings_wind,kMapLightMapBoost,0);
		dialog_special_combo_get_sound(dialog_map_settings_wind,kMapSoundName,0,map.ambient.sound_name,name_str_len);
		map.ambient.sound_pitch=dialog_get_float(dialog_map_settings_wind,kMapSoundPitch,0);

			// media controls
			
		map.media.type=dialog_get_combo(dialog_map_settings_wind,kMapMediaOpenType,0);
		dialog_get_text(dialog_map_settings_wind,kMapMediaOpenName,0,map.media.name,name_str_len);
		dialog_special_combo_get_sound(dialog_map_settings_wind,kMapMediaTitleSoundName,0,map.media.title_sound_name,name_str_len);
		dialog_get_text(dialog_map_settings_wind,kMapMediaMusicName,0,map.music.name,name_str_len);
		map.music.fade_msec=dialog_get_int(dialog_map_settings_wind,kMapMediaMusicFadeIn,0);
		
			// camera
			
		map.camera.mode=dialog_get_combo(dialog_map_settings_wind,kMapCameraMode,0);
		map.camera.plane.fov=dialog_get_float(dialog_map_settings_wind,kMapCameraFOV,0);
		map.camera.plane.aspect_ratio=dialog_get_float(dialog_map_settings_wind,kMapCameraAspectRatio,0);
		map.camera.plane.near_z=dialog_get_int(dialog_map_settings_wind,kMapCameraNearZ,0);
		map.camera.plane.far_z=dialog_get_int(dialog_map_settings_wind,kMapCameraFarZ,0);
		map.camera.plane.near_z_offset=dialog_get_int(dialog_map_settings_wind,kMapCameraNearOffsetZ,0);

		map.camera.ang.x=dialog_get_float(dialog_map_settings_wind,kMapCameraAngleX,0);
		map.camera.ang.y=dialog_get_float(dialog_map_settings_wind,kMapCameraAngleY,0);
		map.camera.ang.z=dialog_get_float(dialog_map_settings_wind,kMapCameraAngleZ,0);
			
		map.camera.chase.distance=dialog_get_int(dialog_map_settings_wind,kMapCameraDistance,0);
		map.camera.chase.track_speed=dialog_get_float(dialog_map_settings_wind,kMapCameraTrackSpeed,0);
		map.camera.chase.slop.x=dialog_get_float(dialog_map_settings_wind,kMapCameraSlopX,0);
		map.camera.chase.slop.y=dialog_get_float(dialog_map_settings_wind,kMapCameraSlopY,0);
		map.camera.chase.slop.z=dialog_get_float(dialog_map_settings_wind,kMapCameraSlopZ,0);
	
		dialog_special_combo_get_node(dialog_map_settings_wind,kMapCameraAttachNode,0,map.camera.c_static.attach_node,name_str_len);
		map.camera.c_static.follow=dialog_get_boolean(dialog_map_settings_wind,kMapCameraFollowCameraObject,0);
		
			// sky
			
		map.sky.on=dialog_get_boolean(dialog_map_settings_wind,kSkySettingSkyOn,0);
		map.sky.type=dialog_get_combo(dialog_map_settings_wind,kSkySettingSkyType,0);
		map.sky.fill=dialog_get_texture_combo(dialog_map_settings_wind,kSkySettingSkyTextureIndex,0,TRUE);
		map.sky.bottom_fill=dialog_get_texture_combo(dialog_map_settings_wind,kSkySettingSkyTextureBottomIndex,0,TRUE);
		map.sky.north_fill=dialog_get_texture_combo(dialog_map_settings_wind,kSkySettingSkyTextureNorthIndex,0,TRUE);
		map.sky.south_fill=dialog_get_texture_combo(dialog_map_settings_wind,kSkySettingSkyTextureSouthIndex,0,TRUE);
		map.sky.east_fill=dialog_get_texture_combo(dialog_map_settings_wind,kSkySettingSkyTextureEastIndex,0,TRUE);
		map.sky.west_fill=dialog_get_texture_combo(dialog_map_settings_wind,kSkySettingSkyTextureWestIndex,0,TRUE);
		map.sky.radius=dialog_get_int(dialog_map_settings_wind,kSkySettingSkyRadius,0);
		map.sky.dome_y=dialog_get_int(dialog_map_settings_wind,kSkySettingSkyDomeY,0);
		map.sky.dome_mirror=dialog_get_boolean(dialog_map_settings_wind,kSkySettingSkyDomeMirror,0);
		map.sky.txt_fact=dialog_get_float(dialog_map_settings_wind,kSkySettingSkyTextureRepeat,0);
		map.sky.txt_x_shift=dialog_get_float(dialog_map_settings_wind,kSkySettingSkyTextureXShift,0);
		map.sky.txt_y_shift=dialog_get_float(dialog_map_settings_wind,kSkySettingSkyTextureYShift,0);
		
			// background
			
		map.background.on=dialog_get_boolean(dialog_map_settings_wind,kBackSettingBackgroundOn,0);
		map.background.back.fill=dialog_get_texture_combo(dialog_map_settings_wind,kBackSettingBackgroundTextureIndex,0,TRUE);
		map.background.back.x_scroll_fact=dialog_get_float(dialog_map_settings_wind,kBackSettingBackgroundXShift,0);
		map.background.back.y_scroll_fact=dialog_get_float(dialog_map_settings_wind,kBackSettingBackgroundYShift,0);
		map.background.middle.fill=dialog_get_texture_combo(dialog_map_settings_wind,kBackSettingBackgroundTextureIndex,1,TRUE);
		map.background.middle.x_scroll_fact=dialog_get_float(dialog_map_settings_wind,kBackSettingBackgroundXShift,1);
		map.background.middle.y_scroll_fact=dialog_get_float(dialog_map_settings_wind,kBackSettingBackgroundYShift,1);
		map.background.front.fill=dialog_get_texture_combo(dialog_map_settings_wind,kBackSettingBackgroundTextureIndex,2,TRUE);
		map.background.front.x_scroll_fact=dialog_get_float(dialog_map_settings_wind,kBackSettingBackgroundXShift,2);
		map.background.front.y_scroll_fact=dialog_get_float(dialog_map_settings_wind,kBackSettingBackgroundYShift,2);
		
			// fog
			
		map.fog.on=dialog_get_boolean(dialog_map_settings_wind,kFogOn,0);
		map.fog.count=dialog_get_int(dialog_map_settings_wind,kFogCount,0);
		map.fog.inner_radius=dialog_get_int(dialog_map_settings_wind,kFogInnerRadius,0);
		map.fog.outer_radius=dialog_get_int(dialog_map_settings_wind,kFogOuterRadius,0);
		map.fog.high=dialog_get_int(dialog_map_settings_wind,kFogHigh,0);
		map.fog.drop=dialog_get_int(dialog_map_settings_wind,kFogDrop,0);

		map.fog.texture_idx=dialog_get_texture_combo(dialog_map_settings_wind,kFogTextureIndex,0,FALSE);
		map.fog.speed=dialog_get_float(dialog_map_settings_wind,kFogTextureSpeed,0);
		map.fog.txt_x_fact=dialog_get_float(dialog_map_settings_wind,kFogTextureXFact,0);
		map.fog.txt_y_fact=dialog_get_float(dialog_map_settings_wind,kFogTextureYFact,0);
		
		dialog_get_color(dialog_map_settings_wind,kFogColor,0,&map.fog.col);

		map.fog.use_solid_color=dialog_get_boolean(dialog_map_settings_wind,kFogUseSolidColor,0);
		map.fog.alpha=dialog_get_float(dialog_map_settings_wind,kFogAlpha,0);

			// rain
			
		map.rain.density=dialog_get_value(dialog_map_settings_wind,kRainDensity,0);
		map.rain.radius=dialog_get_int(dialog_map_settings_wind,kRainRadius,0);
		map.rain.height=dialog_get_int(dialog_map_settings_wind,kRainHeight,0);
		map.rain.speed=dialog_get_int(dialog_map_settings_wind,kRainSpeed,0);
		map.rain.line_width=dialog_get_int(dialog_map_settings_wind,kRainWidth,0);
		map.rain.line_length=dialog_get_int(dialog_map_settings_wind,kRainLength,0);
		map.rain.alpha=dialog_get_float(dialog_map_settings_wind,kRainAlpha,0);
		
		dialog_get_color(dialog_map_settings_wind,kRainStartColor,0,&map.rain.start_color);
		dialog_get_color(dialog_map_settings_wind,kRainEndColor,0,&map.rain.end_color);
		
		map.rain.slant_add=dialog_get_int(dialog_map_settings_wind,kRainSlantAdd,0);
		map.rain.slant_time_msec=dialog_get_int(dialog_map_settings_wind,kRainSlantTimeMSec,0);
		map.rain.slant_change_msec=dialog_get_int(dialog_map_settings_wind,kRainSlantChangeMSec,0);
		
		map.rain.on=dialog_get_boolean(dialog_map_settings_wind,kRainOn,0);
		
			// editor
			
		map.settings.editor.txt_scale_x=dialog_get_float(dialog_map_settings_wind,kMapTextureXFactor,0);
		map.settings.editor.txt_scale_y=dialog_get_float(dialog_map_settings_wind,kMapTextureYFactor,0);
		
		map.settings.editor.view_near_dist=dialog_get_int(dialog_map_settings_wind,kMapEditorViewNearZ,0);
		map.settings.editor.view_far_dist=dialog_get_int(dialog_map_settings_wind,kMapEditorViewFarZ,0);
		
		map.settings.editor.link_always_start=dialog_get_boolean(dialog_map_settings_wind,kMapEditorLinkStartAlways,0);
	}

		// close window
		
	DisposeWindow(dialog_map_settings_wind);
	
	return(!dialog_map_settings_cancel);
}

