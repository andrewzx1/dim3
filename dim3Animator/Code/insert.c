/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Model Import and Insert

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit �Created with dim3 Technology� is given on a single
application screen and in a single piece of the documentation
3. It is not resold, in it's current form or modified, as an
engine-only product

This code is presented as is. The author of dim3 takes no
responsibilities for any version of this code.

Any non-engine product (games, etc) created with this code is free
from any and all payment and/or royalties to the author of dim3,
and can be sold or given away.

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

model_type						ins_model;

extern int						cur_mesh;
extern model_type				model;

extern file_path_setup_type		file_path_setup;

/* =======================================================

      Insert One Model Into Another
      
======================================================= */

void insert_model(char *file_name)
{
	int						i,k,t,b_off,v_off,t_off,idx;
	char					str[8],path[1024],path2[1024],sub_path[1024];
	bool					bone_ok;
	model_bone_type			*bone,*ins_bone;
	model_vertex_type		*vertex,*ins_vertex;
	model_trig_type			*trig,*ins_trig;
	texture_type			*texture,*ins_texture;
	model_material_type		*material,*ins_material;
	
		// open model
		
	model_setup(&file_path_setup,anisotropic_mode_none,mipmap_mode_none,texture_quality_mode_high,FALSE,FALSE);
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
			
		while (true) {
			
			bone_ok=TRUE;
			
			for (k=0;k!=b_off;k++) {
				if (model.bones[k].tag==bone->tag) {
					model_tag_to_text(bone->tag,str);
					if ((str[3]<'0') || (str[3]>'9')) {
						str[3]='1';
					}
					else {
						str[3]+=1;
					}
					bone->tag=text_to_model_tag(str);
					bone_ok=FALSE;
					break;
				}
			}
			
			if (bone_ok) break;
		}
		
		ins_bone++;
	}
	
		// bring in the vertexes
		
	v_off=model.meshes[cur_mesh].nvertex;
	
	model_mesh_set_vertex_count(&model,cur_mesh,(model.meshes[cur_mesh].nvertex+ins_model.meshes[0].nvertex));
	
	vertex=&model.meshes[cur_mesh].vertexes[v_off];
	ins_vertex=ins_model.meshes[0].vertexes;
	
	for (i=0;i!=ins_model.meshes[0].nvertex;i++) {
		memmove(vertex,ins_vertex,sizeof(model_vertex_type));
		
		if (vertex->major_bone_idx!=-1) vertex->major_bone_idx+=b_off;
		if (vertex->minor_bone_idx!=-1) vertex->minor_bone_idx+=b_off;
		
		vertex++;
		ins_vertex++;
	}
	
		// bring in the trigs
		
	t_off=model.meshes[cur_mesh].ntrig;
	
	model_mesh_set_trig_count(&model,cur_mesh,(model.meshes[cur_mesh].ntrig+ins_model.meshes[0].ntrig));
	
	trig=&model.meshes[cur_mesh].trigs[t_off];
	ins_trig=ins_model.meshes[0].trigs;
	
	for (i=0;i!=ins_model.meshes[0].ntrig;i++) {
		memmove(trig,ins_trig,sizeof(model_trig_type));
		
		trig->v[0]+=v_off;
		trig->v[1]+=v_off;
		trig->v[2]+=v_off;
		
		trig++;
		ins_trig++;
	}
	
		// bring in the textures
		
	ins_texture=ins_model.textures;
	ins_material=ins_model.meshes[0].materials;
	
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
		
		for (t=0;t!=max_model_texture;t++) {
			if (texture->frames[t].bitmap.gl_id!=-1) {
				sprintf(texture->frames[t].name,"Image_%d_%d",k,t);
			}
		}
		
		material=&model.meshes[cur_mesh].materials[k];
		memmove(material,ins_material,sizeof(model_material_type));

		material->trig_start+=t_off;
		
			// force bitmap to not close when closing insert model
					
		for (t=0;t!=max_model_texture;t++) {
			ins_texture->frames[t].bitmap.gl_id=-1;
		}
		
			// copy over bitmap

		for (t=0;t!=max_model_texture;t++) {
			if (texture->frames[t].bitmap.gl_id!=-1) {
				sprintf(sub_path,"Models/%s/Textures",file_name);
				file_paths_data_default(&file_path_setup,path,sub_path,ins_texture->frames[t].name,"png");	

				sprintf(sub_path,"Models/%s/Textures",model.name);
				file_paths_data_default(&file_path_setup,path2,sub_path,texture->frames[t].name,"png");
				
				bitmap_copy(path,path2);
			}
		}
		
		ins_texture++;
		ins_material++;
	}
	
		// close model
		
	model_close(&ins_model);
}

