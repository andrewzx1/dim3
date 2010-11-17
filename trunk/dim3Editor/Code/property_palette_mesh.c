/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Property Palette Mesh

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

#define kMeshPropertyOn							0
#define kMeshPropertyPassThrough				1
#define kMeshPropertyMovable					2
#define kMeshPropertyShiftable					3
#define kMeshPropertyHilite						4
#define kMeshPropertyLockUV						5
#define kMeshPropertyLockMove					6
#define kMeshPropertyShadow						7
#define kMeshPropertyNeverObscure				8
#define kMeshPropertyRotIndependent				9
#define kMeshPropertyNoLightMap					10
#define kMeshPropertySkipLightMapTrace			11

#define kMeshPropertyRotX						12
#define kMeshPropertyRotY						13
#define kMeshPropertyRotZ						14

#define kMeshPropertyGroup						15
#define kMeshPropertyHideMode					16
#define kMeshPropertyNormalMode					17

#define kMeshPropertyMessageEnter				18
#define kMeshPropertyMessageEnterId				19
#define kMeshPropertyMessageExit				20
#define kMeshPropertyMessageExitId				21
#define kMeshPropertyMessageMapChange			22
#define kMeshPropertyMessageMapChangeName		23
#define kMeshPropertyMessageMapChangeSpotName	24
#define kMeshPropertyMessageBase				25
#define kMeshPropertyMessageBaseTeam			26

extern map_type					map;
extern editor_state_type		state;
extern editor_setup_type		setup;
extern file_path_setup_type		file_path_setup;

extern list_palette_type		property_palette;

char							mesh_property_hide_list[][name_str_len]={"Never","Single Player","Multiplayer",""},
								mesh_property_normal_list[][name_str_len]={"Auto","In","Out","In & Out","Edge","Locked",""};

/* =======================================================

      Property Palette Fill Mesh
      
======================================================= */

/*

	dialog_set_int(palette_mesh_wind,kMeshSettingRotX,0,mesh->rot_off.x);
	dialog_set_int(palette_mesh_wind,kMeshSettingRotY,0,mesh->rot_off.y);
	dialog_set_int(palette_mesh_wind,kMeshSettingRotZ,0,mesh->rot_off.z);
	
	dialog_fill_group_combo(palette_mesh_wind,kMeshSettingGroup,0,mesh->group_idx);
	
	dialog_set_boolean(palette_mesh_wind,kMeshSendMessageEnter,0,mesh->msg.entry_on);
	dialog_set_int(palette_mesh_wind,kMeshSendMessageEnterId,0,mesh->msg.entry_id);
	dialog_set_boolean(palette_mesh_wind,kMeshSendMessageExit,0,mesh->msg.exit_on);
	dialog_set_int(palette_mesh_wind,kMeshSendMessageExitId,0,mesh->msg.exit_id);
	dialog_set_boolean(palette_mesh_wind,kMeshSendMessageMapChange,0,mesh->msg.map_change_on);
	dialog_set_text(palette_mesh_wind,kMeshSendMessageMapChangeName,0,mesh->msg.map_name);
	dialog_set_text(palette_mesh_wind,kMeshSendMessageMapChangeSpotName,0,mesh->msg.map_spot_name);
	dialog_set_boolean(palette_mesh_wind,kMeshSendMessageBase,0,mesh->msg.base_on);
	dialog_set_combo(palette_mesh_wind,kMeshSendMessageBaseTeam,0,mesh->msg.base_team);
	
*/

void property_palette_fill_mesh(map_mesh_type *mesh)
{
	list_palette_add_header(&property_palette,0,"Mesh Settings");

	list_palette_add_checkbox(&property_palette,kMeshPropertyOn,"On",mesh->flag.on);
	list_palette_add_checkbox(&property_palette,kMeshPropertyPassThrough,"Pass Through",mesh->flag.pass_through);
	list_palette_add_checkbox(&property_palette,kMeshPropertyMovable,"Movable",mesh->flag.moveable);
	list_palette_add_checkbox(&property_palette,kMeshPropertyHilite,"Hight Lighted",mesh->flag.hilite);
	list_palette_add_checkbox(&property_palette,kMeshPropertyLockUV,"Lock UVs",mesh->flag.lock_uv);
	list_palette_add_checkbox(&property_palette,kMeshPropertyLockMove,"Lock Position",mesh->flag.lock_move);
	list_palette_add_checkbox(&property_palette,kMeshPropertyShadow,"Shadow",mesh->flag.shadow);
	list_palette_add_checkbox(&property_palette,kMeshPropertyNeverObscure,"Never Obscure",mesh->flag.never_obscure);
	list_palette_add_checkbox(&property_palette,kMeshPropertyRotIndependent,"Rotate Independently",mesh->flag.rot_independent);
	list_palette_add_checkbox(&property_palette,kMeshPropertyNoLightMap,"No Light Map",mesh->flag.no_light_map);
	list_palette_add_checkbox(&property_palette,kMeshPropertySkipLightMapTrace,"Skip Light Map Trace",mesh->flag.skip_light_map_trace);

	list_palette_add_header(&property_palette,0,"Mesh Modes");
	list_palette_add_combo(&property_palette,kMeshPropertyHideMode,"Hide",(char*)mesh_property_hide_list,mesh->hide_mode);
	list_palette_add_combo(&property_palette,kMeshPropertyNormalMode,"Normals",(char*)mesh_property_normal_list,mesh->normal_mode);

	list_palette_add_header(&property_palette,0,"Mesh Group");

	list_palette_add_header(&property_palette,0,"Mesh Rotation Center");
	list_palette_add_header(&property_palette,0,"Mesh Messages");

}

/* =======================================================

      Property Palette Click Mesh
      
======================================================= */

void property_palette_click_mesh(map_mesh_type *mesh,int id)
{
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

	}

	main_wind_draw();
}

