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
#include "dialog.h"

#define kMeshPropertyName					0
#define kMeshPropertyDiffuse				1
#define kMeshPropertyNoLighting				2
#define kMeshPropertyAdditive				3
#define kMeshPropertyTintable				4

extern model_type				model;
extern animator_state_type		state;
extern file_path_setup_type		file_path_setup;

extern list_palette_type		property_palette;

/* =======================================================

      Property Palette Fill Mesh
      
======================================================= */

void property_palette_fill_mesh(int mesh_idx)
{
	model_mesh_type			*mesh;
	
	mesh=&model.meshes[mesh_idx];

	list_palette_add_header(&property_palette,0,"Mesh Options");
	list_palette_add_string(&property_palette,kMeshPropertyName,"Name",mesh->name,FALSE);

	list_palette_add_header(&property_palette,0,"Mesh Settings");
	list_palette_add_checkbox(&property_palette,kMeshPropertyDiffuse,"Diffuse Lighting",mesh->diffuse,FALSE);
	list_palette_add_checkbox(&property_palette,kMeshPropertyNoLighting,"Highlighted",mesh->no_lighting,FALSE);
	list_palette_add_checkbox(&property_palette,kMeshPropertyAdditive,"Alpha is Additive",mesh->blend_add,FALSE);
	list_palette_add_checkbox(&property_palette,kMeshPropertyTintable,"Multiplayer Tintable",mesh->tintable,FALSE);
}

/* =======================================================

      Property Palette Click Mesh
      
======================================================= */

void property_palette_click_mesh(int mesh_idx,int id)
{
	model_mesh_type			*mesh;
	
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

		case kMeshPropertyTintable:
			mesh->tintable=!mesh->tintable;
			break;
			
	}

		// redraw

	main_wind_draw();
}

