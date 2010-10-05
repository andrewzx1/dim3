#include "dim3Editor.h"

#include "walk_view.h"
#include "common_view.h"
#include "interface.h"

extern bool						quit;
extern editor_state_type		state;

extern HINSTANCE				hinst;
extern HWND						wnd;

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

void test_debug2(char *str,int i)
{
	char	str2[256];
	sprintf(str2,"%s\t%d",str,i);
	test_debug(str2);
}

void editor_menu_create(void)
{
	HMENU					menu;

	menu=LoadMenu(hinst,MAKEINTRESOURCE(128));
	SetMenu(wnd,menu);

/* supergumba
test_debug2("kCommandFileNew",FOUR_CHAR_CODE('new '));
test_debug2("kCommandFileOpen",FOUR_CHAR_CODE('open'));
test_debug2("kCommandFileClose",FOUR_CHAR_CODE('clse'));
test_debug2("kCommandFileSave",FOUR_CHAR_CODE('save'));
test_debug2("kCommandFileAutoGenerateMap",FOUR_CHAR_CODE('auto'));
test_debug2("kCommandFileQuit",FOUR_CHAR_CODE('quit'));

test_debug2("kCommandEditUndo",FOUR_CHAR_CODE('undo'));
test_debug2("kCommandEditDuplicate",FOUR_CHAR_CODE('pdup'));
test_debug2("kCommandEditDelete",FOUR_CHAR_CODE('pdel'));
test_debug2("kCommandEditSelectMore",FOUR_CHAR_CODE('selm'));

test_debug2("kCommandViewFront",FOUR_CHAR_CODE('vw01'));
test_debug2("kCommandViewLeft",FOUR_CHAR_CODE('vw02'));
test_debug2("kCommandViewRight",FOUR_CHAR_CODE('vw03'));
test_debug2("kCommandViewBack",FOUR_CHAR_CODE('vw04'));
test_debug2("kCommandViewTop",FOUR_CHAR_CODE('vw05'));
test_debug2("kCommandViewBottom",FOUR_CHAR_CODE('vw06'));
test_debug2("kCommandViewPerspective",FOUR_CHAR_CODE('vwps'));
test_debug2("kCommandViewOrtho",FOUR_CHAR_CODE('vwot'));
test_debug2("kCommandViewUVLayer1",FOUR_CHAR_CODE('uvl1'));
test_debug2("kCommandViewUVLayer2",FOUR_CHAR_CODE('uvl2'));
test_debug2("kCommandViewGotoSelect",FOUR_CHAR_CODE('goto'));
test_debug2("kCommandViewGotoMapCenter",FOUR_CHAR_CODE('gotc'));
test_debug2("kCommandViewClip",FOUR_CHAR_CODE('vwcp'));
test_debug2("kCommandViewShowHideLiquids",FOUR_CHAR_CODE('shlq'));
test_debug2("kCommandViewShowHideSpots",FOUR_CHAR_CODE('shss'));
test_debug2("kCommandViewShowHideLights",FOUR_CHAR_CODE('shls'));
test_debug2("kCommandViewShowHideNodes",FOUR_CHAR_CODE('shnd'));
test_debug2("kCommandViewSplitHorizontal",FOUR_CHAR_CODE('sphz'));
test_debug2("kCommandViewSplitVertical",FOUR_CHAR_CODE('spvt'));
test_debug2("kCommandViewRemoveSplit",FOUR_CHAR_CODE('sprm'));

test_debug2("kCommandMapSettings",FOUR_CHAR_CODE('mset'));
test_debug2("kCommandMapRaiseY",FOUR_CHAR_CODE('mrsy'));
test_debug2("kCommandMapLowerY",FOUR_CHAR_CODE('mlwy'));
test_debug2("kCommandMapCenter",FOUR_CHAR_CODE('mctr'));
test_debug2("kCommandMapResetUV",FOUR_CHAR_CODE('mpuv'));
test_debug2("kCommandMapOptimize",FOUR_CHAR_CODE('mopt'));
test_debug2("kCommandMapCreateNormals",FOUR_CHAR_CODE('rmnm'));
test_debug2("kCommandClearLightMaps",FOUR_CHAR_CODE('clmp'));
test_debug2("kCommandBuildLightMaps",FOUR_CHAR_CODE('lmap'));
test_debug2("kCommandRun",FOUR_CHAR_CODE('mrun'));

test_debug2("kCommandMeshCombine",FOUR_CHAR_CODE('pcmb'));
test_debug2("kCommandMeshSplit",FOUR_CHAR_CODE('pspt'));
test_debug2("kCommandMeshTesselate",FOUR_CHAR_CODE('ptsl'));
test_debug2("kCommandMeshResize",FOUR_CHAR_CODE('mrsz'));
test_debug2("kCommandMeshReposition",FOUR_CHAR_CODE('mrps'));
test_debug2("kCommandMeshSkew",FOUR_CHAR_CODE('mskw'));
test_debug2("kCommandMeshFlipX",FOUR_CHAR_CODE('pfpx'));
test_debug2("kCommandMeshFlipY",FOUR_CHAR_CODE('pfpy'));
test_debug2("kCommandMeshFlipZ",FOUR_CHAR_CODE('pfpz'));
test_debug2("kCommandMeshRotateX",FOUR_CHAR_CODE('prtx'));
test_debug2("kCommandMeshRotateY",FOUR_CHAR_CODE('prty'));
test_debug2("kCommandMeshRotateZ",FOUR_CHAR_CODE('prtz'));
test_debug2("kCommandMeshFreeRotate",FOUR_CHAR_CODE('prtf'));
test_debug2("kCommandMeshRaiseY",FOUR_CHAR_CODE('prsy'));
test_debug2("kCommandMeshLowerY",FOUR_CHAR_CODE('plry'));
test_debug2("kCommandMeshSelectAllPoly",FOUR_CHAR_CODE('msll'));
test_debug2("kCommandMeshSnapToGrid",FOUR_CHAR_CODE('msnp'));
test_debug2("kCommandMeshSnapClosestVertex",FOUR_CHAR_CODE('mscv'));
test_debug2("kCommandMeshResetUV",FOUR_CHAR_CODE('mruv'));
test_debug2("kCommandMeshWholeUV",FOUR_CHAR_CODE('mrwv'));
test_debug2("kCommandMeshSingleUV",FOUR_CHAR_CODE('mrss'));
test_debug2("kCommandMeshCreateNormals",FOUR_CHAR_CODE('rsnm'));
test_debug2("kCommandMeshInvertNormals",FOUR_CHAR_CODE('isnm'));

test_debug2("kCommandPolygonHole",FOUR_CHAR_CODE('fhl1'));
test_debug2("kCommandPolygonSnapToGrid",FOUR_CHAR_CODE('psnp'));
test_debug2("kCommandPolygonRotateUV",FOUR_CHAR_CODE('pyru'));
test_debug2("kCommandPolygonFlipU",FOUR_CHAR_CODE('pyfu'));
test_debug2("kCommandPolygonFlipV",FOUR_CHAR_CODE('pyfv'));
test_debug2("kCommandPolygonInvertNormal",FOUR_CHAR_CODE('inrm'));
test_debug2("kCommandPolygonResetUV",FOUR_CHAR_CODE('pyuv'));
test_debug2("kCommandPolygonWholeUV",FOUR_CHAR_CODE('pywv'));
test_debug2("kCommandPolygonSingleUV",FOUR_CHAR_CODE('pyss'));

test_debug2("kCommandVertexSnapToGrid",FOUR_CHAR_CODE('vsnp'));

test_debug2("kCommandGroups",FOUR_CHAR_CODE('grrp'));
test_debug2("kCommandGroupMovements",FOUR_CHAR_CODE('mmov'));
*/
}
