/****************************** File *********************************

Module: dim3 Animator
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
	#include "dim3animator.h"
#endif

#include "glue.h"
#include "ui_common.h"
#include "interface.h"

#define kMeshPropertyName					0
#define kMeshPropertyDiffuse				1
#define kMeshPropertyNoLighting				2
#define kMeshPropertyAdditive				3
#define kMeshPropertyLocked					4
#define kMeshPropertyMovement				5

#define kMeshPropertyTrigUV					10
#define kMeshPropertyTrigNormal				20
#define kMeshPropertyTrigTangent			30

extern model_type				model;
extern animator_state_type		state;
extern file_path_setup_type		file_path_setup;

extern list_palette_type		property_palette;

/* =======================================================

      Property Palette Fill Mesh
      
======================================================= */

void property_palette_fill_mesh(int mesh_idx)
{
	int					n;
	char				str[256];
	d3fpnt				uv;
	model_mesh_type		*mesh;
	model_trig_type		*trig;
	
	mesh=&model.meshes[mesh_idx];

	list_palette_set_sub_title(&property_palette,"Mesh",mesh->name);

	list_palette_add_header(&property_palette,0,"Mesh Options");
	list_palette_add_string(&property_palette,kMeshPropertyName,"Name",mesh->name,FALSE);

	list_palette_add_header(&property_palette,0,"Mesh Settings");
	list_palette_add_checkbox(&property_palette,kMeshPropertyDiffuse,"Diffuse Lighting",mesh->diffuse,FALSE);
	list_palette_add_checkbox(&property_palette,kMeshPropertyNoLighting,"Highlighted",mesh->no_lighting,FALSE);
	list_palette_add_checkbox(&property_palette,kMeshPropertyAdditive,"Alpha is Additive",mesh->blend_add,FALSE);
	list_palette_add_checkbox(&property_palette,kMeshPropertyLocked,"Locked",mesh->locked,FALSE);

	list_palette_add_header(&property_palette,0,"Replace OBJ");
	list_palette_add_point(&property_palette,kMeshPropertyMovement,"Movement",&mesh->import_move,FALSE);

		// any selected trig

	if (state.select_mode==select_mode_polygon) {
		trig=trig_mask_get_single_select(mesh_idx);
		if (trig!=NULL) {

			list_palette_add_header(&property_palette,0,"Selected Trig UVs");

			for (n=0;n!=3;n++) {
				uv.x=trig->gx[n];
				uv.y=trig->gy[n];
				sprintf(str,"Vertex %d",n);
				list_palette_add_uv(&property_palette,(kMeshPropertyTrigUV+n),str,&uv,FALSE);
			}

			list_palette_add_header(&property_palette,0,"Selected Trig Normals");

			for (n=0;n!=3;n++) {
				sprintf(str,"Normal %d",n);
				list_palette_add_vector(&property_palette,(kMeshPropertyTrigNormal+n),str,&trig->tangent_space[n].normal,FALSE);
			}

			list_palette_add_header(&property_palette,0,"Selected Trig Normals");

			for (n=0;n!=3;n++) {
				sprintf(str,"Tangent %d",n);
				list_palette_add_vector(&property_palette,(kMeshPropertyTrigTangent+n),str,&trig->tangent_space[n].tangent,FALSE);
			}

		}
	}

	list_palette_add_header(&property_palette,0,"Mesh Info");
	list_palette_add_string_int(&property_palette,-1,"Vertexes",mesh->nvertex,TRUE);
	list_palette_add_string_int(&property_palette,-1,"Triangles",mesh->ntrig,TRUE);
}

/* =======================================================

      Property Palette Click Mesh
      
======================================================= */

void property_palette_click_mesh(int mesh_idx,int id,bool double_click)
{
	int						n,idx;
	d3pnt					import_move,move_pnt;
	d3fpnt					uv;
	model_vertex_type		*vtx;
	model_trig_type			*trig;
	model_bone_type			*bone;
	model_mesh_type			*mesh;
	
	if (!double_click) return;

	mesh=&model.meshes[mesh_idx];

	switch (id) {

		case kMeshPropertyName:
			dialog_property_string_run(list_string_value_string,(void*)mesh->name,name_str_len,0,0);
			break;

		case kMeshPropertyDiffuse:
			mesh->diffuse=!mesh->diffuse;
			break;

		case kMeshPropertyNoLighting:
			mesh->no_lighting=!mesh->no_lighting;
			break;

		case kMeshPropertyAdditive:
			mesh->blend_add=!mesh->blend_add;
			break;

		case kMeshPropertyLocked:
			mesh->locked=!mesh->locked;
			break;

		case kMeshPropertyMovement:
		
				// get change
				
			memmove(&import_move,&mesh->import_move,sizeof(d3pnt));
			dialog_property_chord_run(list_chord_value_point,(void*)&import_move);
			
			move_pnt.x=import_move.x-mesh->import_move.x;
			move_pnt.y=import_move.y-mesh->import_move.y;
			move_pnt.z=import_move.z-mesh->import_move.z;
			memmove(&mesh->import_move,&import_move,sizeof(d3pnt));
			
				// now set the vertexes
				
			vtx=mesh->vertexes;

			for (n=0;n!=mesh->nvertex;n++) {
				vtx->pnt.x+=move_pnt.x;
				vtx->pnt.y+=move_pnt.y;
				vtx->pnt.z+=move_pnt.z;

				vtx++;
			}

				// if this is mesh 0, then move the
				// bones

			if (mesh_idx==0) {

				bone=model.bones;

				for (n=0;n!=model.nbone;n++) {
					bone->pnt.x+=move_pnt.x;
					bone->pnt.y+=move_pnt.y;
					bone->pnt.z+=move_pnt.z;

					bone++;
				}
			}
				
			break;

		case kMeshPropertyTrigUV:
		case (kMeshPropertyTrigUV+1):
		case (kMeshPropertyTrigUV+2):
			trig=trig_mask_get_single_select(mesh_idx);
			if (trig==NULL) break;

			idx=id-kMeshPropertyTrigUV;

			uv.x=trig->gx[idx];
			uv.y=trig->gy[idx];
			dialog_property_chord_run(list_chord_value_uv,(void*)&uv);
			trig->gx[idx]=uv.x;
			trig->gy[idx]=uv.y;

			break;

		case kMeshPropertyTrigNormal:
		case (kMeshPropertyTrigNormal+1):
		case (kMeshPropertyTrigNormal+2):
			trig=trig_mask_get_single_select(mesh_idx);
			if (trig==NULL) break;

			idx=id-kMeshPropertyTrigNormal;
			dialog_property_chord_run(list_chord_value_vector,(void*)&trig->tangent_space[idx].normal);
			vector_normalize(&trig->tangent_space[idx].normal);
			break;

		case kMeshPropertyTrigTangent:
		case (kMeshPropertyTrigTangent+1):
		case (kMeshPropertyTrigTangent+2):
			trig=trig_mask_get_single_select(mesh_idx);
			if (trig==NULL) break;

			idx=id-kMeshPropertyTrigTangent;
			dialog_property_chord_run(list_chord_value_vector,(void*)&trig->tangent_space[idx].tangent);
			vector_normalize(&trig->tangent_space[idx].tangent);
			break;
			
	}

		// redraw

	main_wind_draw();
}

