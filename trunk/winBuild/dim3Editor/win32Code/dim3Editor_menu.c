#include "dim3Editor.h"

#include "walk_view.h"
#include "common_view.h"
#include "interface.h"

// supergumba -- temporary
#define kCommandFileQuit					100

#define kCommandViewFront					200
#define kCommandViewLeft					201
#define kCommandViewRight					202
#define kCommandViewBack					203
#define kCommandViewTop						204
#define kCommandViewBottom					205
#define kCommandViewPerspective				206
#define kCommandViewOrtho					207
#define kCommandViewUVLayer1				208
#define kCommandViewUVLayer2				209
#define kCommandViewClip					210
#define kCommandViewShowHideLiquids			211
#define kCommandViewShowHideSpots			212
#define kCommandViewShowHideLights			213
#define kCommandViewShowHideNodes			214
#define kCommandViewSplitHorizontal			215
#define kCommandViewSplitVertical			216
#define kCommandViewRemoveSplit				217

extern bool						quit;
extern editor_state_type		state;

extern HWND						wnd;

/* =======================================================

      Menu Commands
      
======================================================= */

void editor_menu_commands(int id)
{
	switch (id) {

		case kCommandFileQuit:
			quit=TRUE;
			break;

		case kCommandViewFront:
			walk_view_face_front();
			main_wind_draw();
			break;

		case kCommandViewLeft:
			walk_view_face_left();
			main_wind_draw();
			break;

		case kCommandViewRight:
			walk_view_face_right();
			main_wind_draw();
			break;

		case kCommandViewBack:
			walk_view_face_back();
			main_wind_draw();
			break;

		case kCommandViewTop:
			walk_view_face_top();
			main_wind_draw();
			break;

		case kCommandViewBottom:
			walk_view_face_bottom();
			main_wind_draw();
			break;

		case kCommandViewPerspective:
		case kCommandViewOrtho:
			break;

		case kCommandViewUVLayer1:
			walk_view_set_uv_layer(uv_layer_normal);
			main_wind_draw();
			break;

		case kCommandViewUVLayer2:
			walk_view_set_uv_layer(uv_layer_light_map);
			main_wind_draw();
			break;

		case kCommandViewClip:
			walk_view_flip_clip();
			main_wind_draw();
			break;

		case kCommandViewShowHideLiquids:
			state.show_liquid=!state.show_liquid;
			main_wind_draw();
			break;

		case kCommandViewShowHideSpots:
			state.show_object=!state.show_object;
			main_wind_draw();
			break;

		case kCommandViewShowHideLights:
			state.show_lightsoundparticle=!state.show_lightsoundparticle;
			main_wind_draw();
			break;

		case kCommandViewShowHideNodes:
			state.show_node=!state.show_node;
			main_wind_draw();
			break;

		case kCommandViewSplitHorizontal:
			walk_view_split_horizontal();
			main_wind_draw();
			break;

		case kCommandViewSplitVertical:
			walk_view_split_vertical();
			main_wind_draw();
			break;

		case kCommandViewRemoveSplit:
			walk_view_remove();
			main_wind_draw();
			break;

	}
}

/* =======================================================

      Create Menu
      
======================================================= */

void editor_menu_create(void)
{
	HMENU					menu,sub_menu;

	menu=CreateMenu();

		// file menu

	sub_menu=CreatePopupMenu();

	AppendMenu(sub_menu,MF_STRING,kCommandFileQuit,"Exit");
	AppendMenu(menu,(MF_STRING|MF_POPUP),(UINT)sub_menu,"File");

		// view menu

	sub_menu=CreatePopupMenu();

	AppendMenu(sub_menu,MF_STRING,kCommandViewFront,"Front");
	AppendMenu(sub_menu,MF_STRING,kCommandViewLeft,"Left");
	AppendMenu(sub_menu,MF_STRING,kCommandViewRight,"Right");
	AppendMenu(sub_menu,MF_STRING,kCommandViewBack,"Back");
	AppendMenu(sub_menu,MF_STRING,kCommandViewTop,"Top");
	AppendMenu(sub_menu,MF_STRING,kCommandViewBottom,"Bottom");
	AppendMenu(sub_menu,MF_SEPARATOR,0,NULL);

	AppendMenu(sub_menu,MF_STRING,kCommandViewPerspective,"Perspective");
	AppendMenu(sub_menu,MF_STRING,kCommandViewOrtho,"Ortho");
	AppendMenu(sub_menu,MF_SEPARATOR,0,NULL);

	AppendMenu(sub_menu,MF_STRING,kCommandViewUVLayer1,"Normal Map");
	AppendMenu(sub_menu,MF_STRING,kCommandViewUVLayer2,"Light Map");
	AppendMenu(sub_menu,MF_SEPARATOR,0,NULL);

	AppendMenu(sub_menu,MF_STRING,kCommandViewClip,"Clip");
	AppendMenu(sub_menu,MF_SEPARATOR,0,NULL);

	AppendMenu(sub_menu,MF_STRING,kCommandViewShowHideLiquids,"Show Liquids");
	AppendMenu(sub_menu,MF_STRING,kCommandViewShowHideSpots,"Show Spots");
	AppendMenu(sub_menu,MF_STRING,kCommandViewShowHideLights,"Show Lights");
	AppendMenu(sub_menu,MF_STRING,kCommandViewShowHideNodes,"Show Nodes");
	AppendMenu(sub_menu,MF_SEPARATOR,0,NULL);

	AppendMenu(sub_menu,MF_STRING,kCommandViewSplitHorizontal,"Split Horz");
	AppendMenu(sub_menu,MF_STRING,kCommandViewSplitVertical,"Split Vert");
	AppendMenu(sub_menu,MF_STRING,kCommandViewRemoveSplit,"Remove");
	AppendMenu(menu,(MF_STRING|MF_POPUP),(UINT)sub_menu,"View");

		// temp menu



	SetMenu(wnd,menu);
}
