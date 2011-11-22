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
#include "ui_common.h"
#include "interface.h"

#define kMeshPropertyOn							0
#define kMeshPropertyPassThrough				1
#define kMeshPropertyMovable					2
#define kMeshPropertyShiftable					3
#define kMeshPropertyHilite						4
#define kMeshPropertySimpleCollision			5
#define kMeshPropertyLockUV						6
#define kMeshPropertyLockMove					7
#define kMeshPropertyNeverObscure				8
#define kMeshPropertyNeverCull					9
#define kMeshPropertyRotIndependent				10
#define kMeshPropertyNoLightMap					11
#define kMeshPropertySkipLightMapTrace			12
#define kMeshPropertyNoHaloObscure				13

#define kMeshPropertyGroup						20

#define kMeshPropertyHideMode					30
#define kMeshPropertyHarm						31
#define kMeshPropertyRot						32

#define kMeshPropertyImportOBJName				40
#define kMeshPropertyImportGroupName			41

#define kMeshPropertyMessageEnter				50
#define kMeshPropertyMessageEnterId				51
#define kMeshPropertyMessageExit				52
#define kMeshPropertyMessageExitId				53
#define kMeshPropertyMessageMapChange			54
#define kMeshPropertyMessageMapChangeName		55
#define kMeshPropertyMessageMapChangeSpotName	56

#define kMeshPropertyBaseOn						57
#define kMeshPropertyBaseTeam					58

#define kMeshPolyPropertyClimbable				60
#define kMeshPolyPropertyNeverCull				61
#define kMeshPolyPropertyObscuring				62

#define kMeshPolyPropertyOff					70
#define kMeshPolyPropertySize					71
#define kMeshPolyPropertyShift					72

#define kMeshPolyPropertyTangent				73
#define kMeshPolyPropertyBinormal				74
#define kMeshPolyPropertyNormal					75

#define kMeshPolyPropertyCamera					80

extern map_type					map;
extern editor_state_type		state;
extern editor_setup_type		setup;

extern list_palette_type		property_palette;

char							mesh_property_hide_list[][name_str_len]={"Never","Single Player","Multiplayer",""},
								mesh_property_team_list[][name_str_len]={"None","Red","Blue","Common",""};

int								pal_mesh_index,pal_poly_index;
d3pnt							pal_mesh_pnt,pal_mesh_size;
d3vct							pal_mesh_binormal;
d3fpnt							pal_mesh_uv_offset,pal_mesh_uv_size,
								pal_mesh_prev_uv_offset,pal_mesh_prev_uv_size;

/* =======================================================

      Property Palette Fill Mesh
      
======================================================= */

void property_palette_fill_mesh(int mesh_idx,int poly_idx)
{
	char					str[32];
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;
	editor_view_type		*view;
	
		// mesh settings

	mesh=&map.mesh.meshes[mesh_idx];

	sprintf(str,"%d",mesh_idx);
	list_palette_set_sub_title(&property_palette,"Mesh",str);

		// settings
		
	list_palette_add_header(&property_palette,0,"Mesh Settings");
	list_palette_add_checkbox(&property_palette,kMeshPropertyOn,"On",&mesh->flag.on,FALSE);
	list_palette_add_checkbox(&property_palette,kMeshPropertyPassThrough,"Pass Through",&mesh->flag.pass_through,FALSE);
	list_palette_add_checkbox(&property_palette,kMeshPropertyMovable,"Movable",&mesh->flag.moveable,FALSE);
	list_palette_add_checkbox(&property_palette,kMeshPropertyHilite,"High Lighted",&mesh->flag.hilite,FALSE);
	list_palette_add_checkbox(&property_palette,kMeshPropertySimpleCollision,"Simple Box Collision",&mesh->flag.simple_collision,FALSE);
	list_palette_add_checkbox(&property_palette,kMeshPropertyLockUV,"Lock UVs",&mesh->flag.lock_uv,FALSE);
	list_palette_add_checkbox(&property_palette,kMeshPropertyLockMove,"Lock Position",&mesh->flag.lock_move,FALSE);
	list_palette_add_checkbox(&property_palette,kMeshPropertyNeverObscure,"Never Obscure",&mesh->flag.never_obscure,FALSE);
	list_palette_add_checkbox(&property_palette,kMeshPropertyNeverCull,"Never Cull",&mesh->flag.never_cull,FALSE);
	list_palette_add_checkbox(&property_palette,kMeshPropertyRotIndependent,"Rotate Independently",&mesh->flag.rot_independent,FALSE);
	list_palette_add_checkbox(&property_palette,kMeshPropertyNoLightMap,"No Light Map",&mesh->flag.no_light_map,FALSE);
	list_palette_add_checkbox(&property_palette,kMeshPropertySkipLightMapTrace,"Skip Light Map Trace",&mesh->flag.skip_light_map_trace,FALSE);
	list_palette_add_checkbox(&property_palette,kMeshPropertyNoHaloObscure,"No Halo Obscure",&mesh->flag.no_halo_obscure,FALSE);

		// options
		
	list_palette_add_header(&property_palette,0,"Mesh Options");
	list_palette_add_picker_list_int(&property_palette,kMeshPropertyHideMode,"Hide",(char*)mesh_property_hide_list,-1,name_str_len,0,FALSE,&mesh->hide_mode,FALSE);
	list_palette_add_int(&property_palette,kMeshPropertyHarm,"Harm",&mesh->harm,FALSE);
	list_palette_add_point(&property_palette,kMeshPropertyRot,"Rotational Center",&mesh->rot_off,FALSE);

	list_palette_add_header(&property_palette,0,"Mesh Group");
	if (mesh->group_idx==-1) {
		list_palette_add_string(&property_palette,kMeshPropertyGroup,"Group","",FALSE);
	}
	else {
		list_palette_add_string(&property_palette,kMeshPropertyGroup,"Group",map.group.groups[mesh->group_idx].name,FALSE);
	}

		// messages
		
	list_palette_add_header(&property_palette,0,"Mesh Messages");
	list_palette_add_checkbox(&property_palette,kMeshPropertyMessageEnter,"Entry On",&mesh->msg.entry_on,FALSE);
	list_palette_add_int(&property_palette,kMeshPropertyMessageEnterId,"Entry ID",&mesh->msg.entry_id,FALSE);
	list_palette_add_checkbox(&property_palette,kMeshPropertyMessageExit,"Exit On",&mesh->msg.exit_on,FALSE);
	list_palette_add_int(&property_palette,kMeshPropertyMessageExitId,"Exit ID",&mesh->msg.exit_id,FALSE);
	list_palette_add_checkbox(&property_palette,kMeshPropertyMessageMapChange,"Map Change On",&mesh->msg.map_change_on,FALSE);
	list_palette_add_string(&property_palette,kMeshPropertyMessageMapChangeName,"Map Name",mesh->msg.map_name,FALSE);
	list_palette_add_string(&property_palette,kMeshPropertyMessageMapChangeSpotName,"Map Spot Name",mesh->msg.map_spot_name,FALSE);
	
		// bases
		
	list_palette_add_header(&property_palette,0,"Mesh Base");
	list_palette_add_checkbox(&property_palette,kMeshPropertyBaseOn,"On",&mesh->msg.base_on,FALSE);
	list_palette_add_picker_list_int(&property_palette,kMeshPropertyBaseTeam,"Team",(char*)mesh_property_team_list,-1,name_str_len,0,FALSE,&mesh->msg.base_team,FALSE);

		// import
		
	list_palette_add_header(&property_palette,0,"Mesh Import");
	list_palette_add_string(&property_palette,kMeshPropertyImportOBJName,"OBJ Name",mesh->import.obj_name,FALSE);
	list_palette_add_string(&property_palette,kMeshPropertyImportGroupName,"Group Name",mesh->import.group_name,FALSE);
	
		// info

	map_mesh_calculate_extent(&map,mesh_idx,&pal_mesh_pnt,&pal_mesh_size);
	pal_mesh_size.x-=pal_mesh_pnt.x;
	pal_mesh_size.y-=pal_mesh_pnt.y;
	pal_mesh_size.z-=pal_mesh_pnt.z;
	
	pal_mesh_index=mesh_idx;
		
	list_palette_add_header(&property_palette,0,"Mesh Info");
	list_palette_add_int(&property_palette,-1,"Mesh Index",&pal_mesh_index,TRUE);
	list_palette_add_int(&property_palette,-1,"Poly Count",&mesh->npoly,TRUE);
	list_palette_add_point(&property_palette,-1,"Position",&pal_mesh_pnt,TRUE);
	list_palette_add_point(&property_palette,-1,"Size",&pal_mesh_size,TRUE);

		// polygon settings

	if (poly_idx!=-1) {

		view=view_get_current_view();
		poly=&mesh->polys[poly_idx];
		
		list_palette_add_header(&property_palette,0,"Poly Settings");
		list_palette_add_checkbox(&property_palette,kMeshPolyPropertyClimbable,"Cimbable",&poly->flag.climbable,FALSE);
		list_palette_add_checkbox(&property_palette,kMeshPolyPropertyNeverCull,"Never Cull",&poly->flag.never_cull,FALSE);
		list_palette_add_checkbox(&property_palette,kMeshPolyPropertyObscuring,"Obscuring",&poly->flag.obscuring,FALSE);

		memmove(&pal_mesh_prev_uv_offset,&pal_mesh_uv_offset,sizeof(d3fpnt));		// need to remember previous as list redraw will reset before we get change
		memmove(&pal_mesh_prev_uv_size,&pal_mesh_uv_size,sizeof(d3fpnt));
		map_mesh_get_poly_uv_as_box(&map,mesh_idx,poly_idx,(view->uv_layer==uv_layer_light_map),&pal_mesh_uv_offset.x,&pal_mesh_uv_offset.y,&pal_mesh_uv_size.x,&pal_mesh_uv_size.y);
		
		list_palette_add_header(&property_palette,0,"Poly UVs");
		list_palette_add_uv(&property_palette,kMeshPolyPropertyOff,"Offset",&pal_mesh_uv_offset.x,&pal_mesh_uv_offset.y,FALSE);
		list_palette_add_uv(&property_palette,kMeshPolyPropertySize,"Size",&pal_mesh_uv_size.x,&pal_mesh_uv_size.y,FALSE);
		list_palette_add_uv(&property_palette,kMeshPolyPropertyShift,"Shift",&poly->x_shift,&poly->y_shift,FALSE);
		
		list_palette_add_header(&property_palette,0,"Poly Tangent Space");
		list_palette_add_normal_vector(&property_palette,kMeshPolyPropertyTangent,"Tangent",&poly->tangent_space.tangent,FALSE);
		vector_cross_product(&pal_mesh_binormal,&poly->tangent_space.tangent,&poly->tangent_space.normal);
		list_palette_add_normal_vector(&property_palette,kMeshPolyPropertyBinormal,"Binormal",&pal_mesh_binormal,TRUE);
		list_palette_add_normal_vector(&property_palette,kMeshPolyPropertyNormal,"Normal",&poly->tangent_space.normal,FALSE);

		list_palette_add_header(&property_palette,0,"Poly Camera");
		list_palette_add_string(&property_palette,kMeshPolyPropertyCamera,"Node",poly->camera,FALSE);
		
		pal_poly_index=poly_idx;
		
		list_palette_add_header(&property_palette,0,"Poly Info");
		list_palette_add_int(&property_palette,-1,"Index",&pal_poly_index,TRUE);
		list_palette_add_int(&property_palette,-1,"Vertexes",&poly->ptsz,TRUE);
	}
}

/* =======================================================

      Property Palette Click Mesh
      
======================================================= */

void property_palette_click_mesh(int mesh_idx,int poly_idx,int id,bool double_click)
{
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;
	editor_view_type		*view;

	if (!double_click) return;

		// mesh settings

	mesh=&map.mesh.meshes[mesh_idx];

	switch (id) {

			// options
			
		case kMeshPropertyGroup:
			property_palette_pick_group(&mesh->group_idx);
			break;
			
			// messages

		case kMeshPropertyMessageMapChangeName:
			dialog_property_string_run(list_string_value_string,(void*)mesh->msg.map_name,name_str_len,0,0);
			break;

		case kMeshPropertyMessageMapChangeSpotName:
			dialog_property_string_run(list_string_value_string,(void*)mesh->msg.map_spot_name,name_str_len,0,0);
			break;
			
			// import
			
		case kMeshPropertyImportOBJName:
			dialog_property_string_run(list_string_value_string,(void*)mesh->import.obj_name,name_str_len,0,0);
			break;
			
		case kMeshPropertyImportGroupName:
			dialog_property_string_run(list_string_value_string,(void*)mesh->import.group_name,name_str_len,0,0);
			break;

	}

		// polygon settings

	if (poly_idx!=-1) {

		view=view_get_current_view();
		poly=&mesh->polys[poly_idx];

		switch (id) {

			case kMeshPolyPropertyOff:
			case kMeshPolyPropertySize:
				map_mesh_set_poly_uv_as_box(&map,mesh_idx,poly_idx,(view->uv_layer==uv_layer_light_map),pal_mesh_prev_uv_offset.x,pal_mesh_prev_uv_offset.y,pal_mesh_prev_uv_size.x,pal_mesh_prev_uv_size.y);
				view_vbo_mesh_rebuild(mesh_idx);
				break;
				
			case kMeshPolyPropertyCamera:
				property_palette_pick_node(poly->camera);
				break;

		}
	}
}

