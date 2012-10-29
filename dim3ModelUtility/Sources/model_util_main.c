/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
 Usage: Main Routines

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

(c) 2000-2010 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3modelutility.h"
#endif

extern file_path_setup_type	file_path_setup;

/* =======================================================

      New Model
      
======================================================= */

bool model_new(model_type *model,char *name)
{
	int				n;
	char			sub_path[1024];
	
    memset(model,0x0,sizeof(model_type));
	
		// model name and default path
		
	strcpy(model->name,name);
	
	sprintf(sub_path,"Models/%s",name);
	file_paths_data_default(&file_path_setup,model->load_base_path,sub_path,NULL,NULL);
		
		// default settings
	
	model->comulative_rotation=TRUE;
	
	for (n=0;n!=max_model_light;n++) {
		model->bone_connect.light_bone_idx[n]=-1;
	}

	for (n=0;n!=max_model_halo;n++) {
		model->bone_connect.halo_bone_idx[n]=-1;
	}

	model->bone_connect.name_bone_idx=-1;
	model->bone_connect.label_text_bone_idx=-1;
	model->bone_connect.label_bitmap_bone_idx=-1;
	model->bone_connect.label_health_bone_idx=-1;

	model->rigid_body.on=FALSE;

	model->rigid_body.y.max_ang=0.0f;			// unused, but in struct
	model->rigid_body.y.reset_factor=0.8f;
	model->rigid_body.y.smooth_factor=0.2f;
	model->rigid_body.x.max_ang=45.0f;
	model->rigid_body.x.reset_factor=0.8f;
	model->rigid_body.x.smooth_factor=0.2f;
	model->rigid_body.z.max_ang=45.0f;
	model->rigid_body.z.reset_factor=0.8f;
	model->rigid_body.z.smooth_factor=0.2f;

	model->ui.shader.light_intensity=10000;
	model->ui.shader.light_exponent=1.0f;
	model->ui.shader.light_offset.x=0;
	model->ui.shader.light_offset.y=-5000;
	model->ui.shader.light_offset.z=0;
	model->ui.shader.light_col.r=1.0f;
	model->ui.shader.light_col.g=1.0f;
	model->ui.shader.light_col.b=1.0f;

	model->ui.fixed.min_diffuse=0.75f;
	model->ui.fixed.diffuse_vct.x=0.0f;
	model->ui.fixed.diffuse_vct.y=-1.0f;
	model->ui.fixed.diffuse_vct.z=0.0f;

		// setup first mesh

	model->nmesh=1;

	model->meshes[0].nvertex=0;
	model->meshes[0].npoly=0;
		
	model->meshes[0].vertexes=NULL;
	model->meshes[0].polys=NULL;

	model->meshes[0].no_lighting=FALSE;
	model->meshes[0].diffuse=TRUE;
	model->meshes[0].blend_add=FALSE;
	model->meshes[0].never_cull=TRUE;
	model->meshes[0].locked=FALSE;

	model->meshes[0].import_move.x=0;
	model->meshes[0].import_move.y=0;
	model->meshes[0].import_move.z=0;

		// other model structures

	model->nbone=0;
	model->npose=0;
	model->nanimate=0;
	model->nhit_box=0;

	model->bones=NULL;
	model->poses=NULL;
	model->animates=NULL;
	model->hit_boxes=NULL;

		// textures

	model->textures=malloc(max_model_texture*sizeof(texture_type));
	if (model->textures==NULL) return(FALSE);

	bzero(model->textures,(max_model_texture*sizeof(texture_type)));

	model_textures_clear(model);
	
	return(TRUE);
}

/* =======================================================

      Open Model
      
======================================================= */

bool model_open(model_type *model,char *name,bool load_bitmaps)
{
	char			sub_path[1024];

		// create model
		
	if (!model_new(model,name)) return(FALSE);
	
		// load base path is location of mesh.xml
		
	sprintf(sub_path,"Models/%s",model->name);
	file_paths_data(&file_path_setup,model->load_base_path,sub_path,NULL,NULL);

		// read XML for new format

	if (!model_read_xml(model)) {
	
			// try v2 XML
			
		if (!model_read_v2_mesh_xml(model)) {
			model_close(model);
			return(FALSE);
		}
		
		model_read_v2_pose_xml(model);
		model_read_v2_animate_xml(model);
	}
	
		// pre-calculate parents
		
	model_calculate_parents(model);
	
		// read bitmaps

	if (load_bitmaps) model_textures_read(model);
	
	return(TRUE);
}

/* =======================================================

      Save Model
      
======================================================= */

bool model_save(model_type *model,char *err_str)
{
	return(model_write_xml(model,err_str));
}

/* =======================================================

      Close Model
      
======================================================= */

void model_close(model_type *model)
{
	int				n;
	
	model_textures_close(model);
	
	for (n=0;n!=model->nmesh;n++) {
		if (model->meshes[n].vertexes!=NULL) free(model->meshes[n].vertexes);
		if (model->meshes[n].polys!=NULL) free(model->meshes[n].polys);
	}

	free(model->textures);
	
	if (model->bones!=NULL) free(model->bones);
	if (model->poses!=NULL) free(model->poses);
	if (model->animates!=NULL) free(model->animates);
	if (model->hit_boxes!=NULL) free(model->hit_boxes);
}

/* =======================================================

      Refresh Model Textures
      
======================================================= */

void model_refresh_textures(model_type *model)
{
	model_textures_close(model);
	model_textures_read(model);
}

/* =======================================================

      Model Memory Size
      
======================================================= */

int model_memory_size(model_type *model)
{
	int			n,sz;

	sz=0;

	for (n=0;n!=model->nmesh;n++) {
		sz+=(model->meshes[n].nvertex*sizeof(model_vertex_type));
		sz+=(model->meshes[n].npoly*sizeof(model_poly_type));

		sz+=((model->meshes[n].nvertex*3)*sizeof(float));
		sz+=((model->meshes[n].nvertex*3)*sizeof(float));
		sz+=((model->meshes[n].nvertex*3)*sizeof(float));
		sz+=((model->meshes[n].nvertex*3)*sizeof(float));
	}

	sz+=(max_model_texture*sizeof(texture_type));
	
	sz+=(model->nbone*sizeof(model_bone_type));
	sz+=(model->npose*sizeof(model_pose_type));
	sz+=(model->nanimate*sizeof(model_animate_type));
	sz+=(model->nhit_box*sizeof(model_hit_box_type));

	return(sz);
}
