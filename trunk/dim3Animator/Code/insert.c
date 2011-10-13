/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Model Import and Insert

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

model_type						ins_model;

extern model_type				model;
extern file_path_setup_type		file_path_setup;
extern animator_state_type		state;

/* =======================================================

      Insert One Model Into Another
      
======================================================= */

void insert_model(char *file_name)
{
	int						i,k,t,b_off,v_off,t_off,nvertex,ntrig,idx;
	char					path[1024],path2[1024],sub_path[1024];
	model_bone_type			*bone,*ins_bone;
	model_mesh_type			*mesh,*ins_mesh;
	model_vertex_type		*vertex;
	model_trig_type			*trig;
	texture_type			*texture,*ins_texture;
	
		// open model
		
	model_setup(&file_path_setup,anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE);
	model_open(&ins_model,file_name,TRUE);
	
		// bring in the bones

	b_off=model.nbone;
	ins_bone=ins_model.bones;
	
	for (i=0;i!=ins_model.nbone;i++) {
		idx=model_bone_add(&model,0,0,0);
		if (idx==-1) break;
		
		bone=&model.bones[idx];
		
		memmove(bone,ins_bone,sizeof(model_bone_type));
		if (bone->parent_idx!=-1) bone->parent_idx+=b_off;
		
			// fix duplicate names
			
		for (k=0;k!=b_off;k++) {
			if (model.bones[k].tag==bone->tag) {
				bone->tag=model_bone_create_tag(&model,idx);
			}
		}
		
		ins_bone++;
	}

		// model mesh

	mesh=&model.meshes[state.cur_mesh_idx];
	ins_mesh=&ins_model.meshes[0];
	
		// bring in the vertexes
		
	v_off=mesh->nvertex;
	nvertex=ins_mesh->nvertex;
	
	model_mesh_set_vertex_count(&model,state.cur_mesh_idx,(v_off+nvertex));
	
	vertex=&mesh->vertexes[v_off];
	memmove(vertex,ins_mesh->vertexes,(nvertex*sizeof(model_vertex_type)));
	
	for (i=0;i!=nvertex;i++) {
		if (vertex->major_bone_idx!=-1) vertex->major_bone_idx+=b_off;
		if (vertex->minor_bone_idx!=-1) vertex->minor_bone_idx+=b_off;
		vertex++;
	}
	
		// bring in the trigs
		
	t_off=mesh->ntrig;
	ntrig=ins_mesh->ntrig;
	
	model_mesh_set_trig_count(&model,state.cur_mesh_idx,(t_off+ntrig));
	
	trig=&mesh->trigs[t_off];
	memmove(trig,ins_mesh->trigs,(ntrig*sizeof(model_trig_type)));
	
	for (i=0;i!=ntrig;i++) {
		trig->v[0]+=v_off;
		trig->v[1]+=v_off;
		trig->v[2]+=v_off;
		trig++;
	}
	
		// bring in the textures
		
	ins_texture=ins_model.textures;
	
	for (i=0;i!=max_model_texture;i++) {
	
		if (ins_texture->frames[0].bitmap.gl_id==-1) continue;
	
			// find next blank texture
			
		k=0;
		while (k<max_model_texture) {
			if (model.textures[k].frames[0].bitmap.gl_id==-1) break;
			k++;
		}
		
		if (k>=max_model_texture) break;
		
			// copy texture
			
		texture=&model.textures[k];
		memmove(texture,ins_texture,sizeof(texture_type));
		
		for (t=0;t!=max_texture_frame;t++) {
			if (texture->frames[t].bitmap.gl_id!=-1) {
				sprintf(texture->frames[t].name,"Image_%d_%d",k,t);
			}
		}

			// fix the imported triangles texture index

		trig=&mesh->trigs[t_off];

		for (t=t_off;t!=mesh->ntrig;t++) {
			if (trig->txt_idx==i) trig->txt_idx=k;
			t_off++;
		}
		
			// force bitmap to not close when closing insert model
					
		for (t=0;t!=max_texture_frame;t++) {
			ins_texture->frames[t].bitmap.gl_id=-1;
		}
		
			// copy over bitmap

		for (t=0;t!=max_texture_frame;t++) {
			if (texture->frames[t].bitmap.gl_id!=-1) {
				sprintf(sub_path,"Models/%s/Textures",file_name);
				file_paths_data_default(&file_path_setup,path,sub_path,ins_texture->frames[t].name,"png");	

				sprintf(sub_path,"Models/%s/Textures",model.name);
				file_paths_data_default(&file_path_setup,path2,sub_path,texture->frames[t].name,"png");
				
				bitmap_copy(path,path2);
			}
		}
		
		ins_texture++;
	}
	
		// close model
		
	model_close(&ins_model);
}

