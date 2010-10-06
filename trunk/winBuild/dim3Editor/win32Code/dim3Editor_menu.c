#include "dim3Editor.h"

#include "walk_view.h"
#include "common_view.h"
#include "interface.h"

extern bool						quit;
extern editor_state_type		state;

extern HINSTANCE				hinst;
extern HWND						wnd;

// supergumba -- delete me after cross platform menu in

/* =======================================================

      Menu Commands
      
======================================================= */

void editor_menu_commands(int id)
{
	switch (id) {

		case kCommandFileSave:
			file_save_map();
			break;

		case kCommandFileQuit:
			quit=TRUE;
			break;

		case kCommandEditUndo:
			undo_pull();
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
	HMENU					menu;

	menu=LoadMenu(hinst,MAKEINTRESOURCE(128));
	SetMenu(wnd,menu);
}
