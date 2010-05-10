/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Mesh Palette

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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#include "interface.h"
#include "common_view.h"
#include "dialog.h"

#define kMeshSettingTabCount					3
#define kMeshSettingTab							FOUR_CHAR_CODE('tabb')

#define kMeshSettingOn							FOUR_CHAR_CODE('fson')
#define kMeshSettingPassThrough					FOUR_CHAR_CODE('fpth')
#define kMeshSettingMovable						FOUR_CHAR_CODE('fmov')
#define kMeshSettingShiftable					FOUR_CHAR_CODE('fsht')
#define kMeshSettingHilite						FOUR_CHAR_CODE('hilt')
#define kMeshSettingLockUV						FOUR_CHAR_CODE('lkuv')
#define kMeshSettingLockMove					FOUR_CHAR_CODE('lkps')
#define kMeshSettingShadow						FOUR_CHAR_CODE('shad')
#define kMeshSettingNeverObscure				FOUR_CHAR_CODE('nvob')
#define kMeshSettingRotIndependent				FOUR_CHAR_CODE('rtip')
#define kMeshSettingNoLightMap					FOUR_CHAR_CODE('nlmp')
#define kMeshSettingSkipLightMapTrace			FOUR_CHAR_CODE('skrt')

#define kMeshSettingRotX						FOUR_CHAR_CODE('rotx')
#define kMeshSettingRotY						FOUR_CHAR_CODE('roty')
#define kMeshSettingRotZ						FOUR_CHAR_CODE('rotz')

#define kMeshSettingGroup						FOUR_CHAR_CODE('grop')
#define kMeshSettingHideMode					FOUR_CHAR_CODE('hdmd')
#define kMeshSettingNormalMode					FOUR_CHAR_CODE('nlmd')

#define kMeshSendMessageEnter					FOUR_CHAR_CODE('smen')
#define kMeshSendMessageEnterId					FOUR_CHAR_CODE('meid')
#define kMeshSendMessageExit					FOUR_CHAR_CODE('smex')
#define kMeshSendMessageExitId					FOUR_CHAR_CODE('mxid')
#define kMeshSendMessageMapChange				FOUR_CHAR_CODE('samc')
#define kMeshSendMessageMapChangeName			FOUR_CHAR_CODE('amnm')
#define kMeshSendMessageMapChangeSpotName		FOUR_CHAR_CODE('amsn')
#define kMeshSendMessageBase					FOUR_CHAR_CODE('tbon')
#define kMeshSendMessageBaseTeam				FOUR_CHAR_CODE('tbtx')

extern map_type				map;

WindowRef					palette_mesh_wind;

/* =======================================================

      Palette Mesh Data
      
======================================================= */

void palette_mesh_load(void)
{
	int						type,mesh_idx,poly_idx;
	map_mesh_type			*mesh;

		// get mesh
		
	select_get(0,&type,&mesh_idx,&poly_idx);
	mesh=&map.mesh.meshes[mesh_idx];

		// set controls
	
	dialog_set_boolean(palette_mesh_wind,kMeshSettingOn,0,mesh->flag.on);
	dialog_set_boolean(palette_mesh_wind,kMeshSettingPassThrough,0,mesh->flag.pass_through);
	dialog_set_boolean(palette_mesh_wind,kMeshSettingMovable,0,mesh->flag.moveable);
	dialog_set_boolean(palette_mesh_wind,kMeshSettingHilite,0,mesh->flag.hilite);
	dialog_set_boolean(palette_mesh_wind,kMeshSettingLockUV,0,mesh->flag.lock_uv);
	dialog_set_boolean(palette_mesh_wind,kMeshSettingLockMove,0,mesh->flag.lock_move);
	dialog_set_boolean(palette_mesh_wind,kMeshSettingShadow,0,mesh->flag.shadow);
	dialog_set_boolean(palette_mesh_wind,kMeshSettingNeverObscure,0,mesh->flag.never_obscure);
	dialog_set_boolean(palette_mesh_wind,kMeshSettingRotIndependent,0,mesh->flag.rot_independent);
	dialog_set_boolean(palette_mesh_wind,kMeshSettingNoLightMap,0,mesh->flag.no_light_map);
	dialog_set_boolean(palette_mesh_wind,kMeshSettingSkipLightMapTrace,0,mesh->flag.skip_light_map_trace);

	dialog_set_int(palette_mesh_wind,kMeshSettingRotX,0,mesh->rot_off.x);
	dialog_set_int(palette_mesh_wind,kMeshSettingRotY,0,mesh->rot_off.y);
	dialog_set_int(palette_mesh_wind,kMeshSettingRotZ,0,mesh->rot_off.z);
	
	dialog_fill_group_combo(palette_mesh_wind,kMeshSettingGroup,0,mesh->group_idx);
	
	dialog_set_boolean(palette_mesh_wind,kMeshSendMessageEnter,0,mesh->msg.entry_on);
	dialog_set_int(palette_mesh_wind,kMeshSendMessageEnterId,0,mesh->msg.entry_id);
	dialog_set_boolean(palette_mesh_wind,kMeshSendMessageExit,0,mesh->msg.exit_on);
	dialog_set_int(palette_mesh_wind,kMeshSendMessageExitId,0,mesh->msg.exit_id);
	dialog_set_boolean(palette_mesh_wind,kMeshSendMessageMapChange,0,mesh->msg.map_change_on);
	dialog_special_combo_fill_map(palette_mesh_wind,kMeshSendMessageMapChangeName,0,mesh->msg.map_name);
	dialog_set_text(palette_mesh_wind,kMeshSendMessageMapChangeSpotName,0,mesh->msg.map_spot_name);
	dialog_set_boolean(palette_mesh_wind,kMeshSendMessageBase,0,mesh->msg.base_on);
	dialog_set_combo(palette_mesh_wind,kMeshSendMessageBaseTeam,0,mesh->msg.base_team);
	
	dialog_set_combo(palette_mesh_wind,kMeshSettingHideMode,0,mesh->hide_mode);
	dialog_set_combo(palette_mesh_wind,kMeshSettingNormalMode,0,mesh->normal_mode);

	DrawControls(palette_mesh_wind);
}

void palette_mesh_save(void)
{
	int						type,mesh_idx,poly_idx,normal_mode;
	map_mesh_type			*mesh;

		// get mesh
		
	select_get(0,&type,&mesh_idx,&poly_idx);
	mesh=&map.mesh.meshes[mesh_idx];
		
		// get controls

	mesh->flag.on=dialog_get_boolean(palette_mesh_wind,kMeshSettingOn,0);
	mesh->flag.pass_through=dialog_get_boolean(palette_mesh_wind,kMeshSettingPassThrough,0);
	mesh->flag.moveable=dialog_get_boolean(palette_mesh_wind,kMeshSettingMovable,0);
	mesh->flag.hilite=dialog_get_boolean(palette_mesh_wind,kMeshSettingHilite,0);
	mesh->flag.lock_uv=dialog_get_boolean(palette_mesh_wind,kMeshSettingLockUV,0);
	mesh->flag.lock_move=dialog_get_boolean(palette_mesh_wind,kMeshSettingLockMove,0);
	mesh->flag.shadow=dialog_get_boolean(palette_mesh_wind,kMeshSettingShadow,0);
	mesh->flag.never_obscure=dialog_get_boolean(palette_mesh_wind,kMeshSettingNeverObscure,0);
	mesh->flag.rot_independent=dialog_get_boolean(palette_mesh_wind,kMeshSettingRotIndependent,0);
	mesh->flag.no_light_map=dialog_get_boolean(palette_mesh_wind,kMeshSettingNoLightMap,0);
	mesh->flag.skip_light_map_trace=dialog_get_boolean(palette_mesh_wind,kMeshSettingSkipLightMapTrace,0);
	
	mesh->rot_off.x=dialog_get_int(palette_mesh_wind,kMeshSettingRotX,0);
	mesh->rot_off.y=dialog_get_int(palette_mesh_wind,kMeshSettingRotY,0);
	mesh->rot_off.z=dialog_get_int(palette_mesh_wind,kMeshSettingRotZ,0);
	
	mesh->group_idx=dialog_get_group_combo(palette_mesh_wind,kMeshSettingGroup,0);
	
	mesh->msg.entry_on=dialog_get_boolean(palette_mesh_wind,kMeshSendMessageEnter,0);
	mesh->msg.entry_id=dialog_get_int(palette_mesh_wind,kMeshSendMessageEnterId,0);
	mesh->msg.exit_on=dialog_get_boolean(palette_mesh_wind,kMeshSendMessageExit,0);
	mesh->msg.exit_id=dialog_get_int(palette_mesh_wind,kMeshSendMessageExitId,0);
	mesh->msg.map_change_on=dialog_get_boolean(palette_mesh_wind,kMeshSendMessageMapChange,0);
	dialog_special_combo_get_map(palette_mesh_wind,kMeshSendMessageMapChangeName,0,mesh->msg.map_name,file_str_len);
	dialog_get_text(palette_mesh_wind,kMeshSendMessageMapChangeSpotName,0,mesh->msg.map_spot_name,name_str_len);
	mesh->msg.base_on=dialog_get_boolean(palette_mesh_wind,kMeshSendMessageBase,0);
	mesh->msg.base_team=dialog_get_combo(palette_mesh_wind,kMeshSendMessageBaseTeam,0);
	
	mesh->hide_mode=dialog_get_combo(palette_mesh_wind,kMeshSettingHideMode,0);
	
	normal_mode=mesh->normal_mode;
	mesh->normal_mode=dialog_get_combo(palette_mesh_wind,kMeshSettingNormalMode,0);
	
	if ((normal_mode!=mesh->normal_mode) && (mesh->normal_mode!=mesh_normal_mode_lock)) {
		map_recalc_normals_mesh(mesh,FALSE);
		main_wind_draw();
	}
}

static pascal OSStatus palette_mesh_tab_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	palette_mesh_save();
	
	if ((GetEventClass(event)==kEventClassControl) && (GetEventKind(event)==kEventControlHit)) {
		dialog_switch_tab(palette_mesh_wind,kMeshSettingTab,0,kMeshSettingTabCount);
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Palette Mesh Open/Close
      
======================================================= */

void palette_mesh_open(int x,int y)
{
	ControlRef				ctrl;
	ControlID				ctrl_id;
	EventHandlerUPP			tab_event_upp;
	EventTypeSpec			tab_event_list[]={{kEventClassCommand,kEventProcessCommand},
											  {kEventClassControl,kEventControlHit},
											  {kEventClassKeyboard,kEventRawKeyUp}};

		// open the window
		
	dialog_open(&palette_mesh_wind,"MeshPalette");
	MoveWindow(palette_mesh_wind,x,y,FALSE);
	
		// setup the tabs
		
	dialog_set_tab(palette_mesh_wind,kMeshSettingTab,0,0,kMeshSettingTabCount);

	ctrl_id.signature=kMeshSettingTab;
	ctrl_id.id=0;
	GetControlByID(palette_mesh_wind,&ctrl_id,&ctrl);
	
	tab_event_upp=NewEventHandlerUPP(palette_mesh_tab_proc);
	InstallControlEventHandler(ctrl,tab_event_upp,GetEventTypeCount(tab_event_list),tab_event_list,palette_mesh_wind,NULL);

		// numeric only controls

	dialog_set_numeric_only(palette_mesh_wind,kMeshSettingRotX,0);
	dialog_set_numeric_only(palette_mesh_wind,kMeshSettingRotY,0);
	dialog_set_numeric_only(palette_mesh_wind,kMeshSettingRotZ,0);
	dialog_set_numeric_only(palette_mesh_wind,kMeshSendMessageEnterId,0);
	dialog_set_numeric_only(palette_mesh_wind,kMeshSendMessageExitId,0);

		// show palette
		
	ShowWindow(palette_mesh_wind);
}

void palette_mesh_close(int *x,int *y)
{
	Rect			box;
	
	GetWindowBounds(palette_mesh_wind,kWindowGlobalPortRgn,&box);
	*x=box.left;
	*y=box.top;
	
	DisposeWindow(palette_mesh_wind);
}


