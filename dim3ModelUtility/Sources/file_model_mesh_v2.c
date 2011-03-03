/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
 Usage: Mesh File v2

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

(c) 2000-2010 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3modelutility.h"
#endif

extern char					deform_mode_str[][32],model_bump_mode_str[][32];

/* =======================================================

      Decode Mesh v2 XML
      
======================================================= */

void decode_mesh_v2_xml(model_type *model,int model_head)
{
	int						i,n,k,j,bone_idx,nbone,hit_box_idx,nhit_box,
							import_tag,ui_tag,mesh_idx,nmesh,nfill,trig_idx,
							tag,hit_box_tag,rigid_body_tag,meshes_tag,mesh_tag,
							vertex_tag,bone_tag,vtag,trig_tag,
							materials_tag,material_tag,fills_tag,fill_tag;
	bool					had_tangent;
	char					tag_name[32];
	model_tag				*major_bone_tags,*minor_bone_tags,
							*bone_parent_tags;
	model_hit_box_type		*hit_box;
	model_mesh_type			*mesh;
    model_vertex_type		*vertex;
	model_material_type		*material;
    model_bone_type			*bone;
    model_trig_type			*trig;
    texture_type			*texture;

        // options
    
    tag=xml_findfirstchild("Options",model_head);
    if (tag!=-1) {
		model->deform_mode=xml_get_attribute_list(tag,"deform",(char*)deform_mode_str);
		model->diffuse_boost=xml_get_attribute_float_default(tag,"diffuse_boost",0.0f);
	}
	
        // center
    
    tag=xml_findfirstchild("Center",model_head);
    if (tag!=-1) {
		xml_get_attribute_3_coord_int(tag,"offset",&model->center.x,&model->center.y,&model->center.z);
    }
    
        // boxes
    
    tag=xml_findfirstchild("View_Box",model_head);
    if (tag!=-1) {
        xml_get_attribute_3_coord_int(tag,"size",&model->view_box.size.x,&model->view_box.size.y,&model->view_box.size.z);
        xml_get_attribute_3_coord_int(tag,"offset",&model->view_box.offset.x,&model->view_box.offset.y,&model->view_box.offset.z);
    }
	
        // light
        
    tag=xml_findfirstchild("Light",model_head);
    if (tag!=-1) {

		for (k=0;k!=max_model_light;k++) {
			if (k==0) {
				strcpy(tag_name,"light_bone");
			}
			else {
				sprintf(tag_name,"light_bone_%d",k);
			}
			model->tags.light_bone_tag[k]=xml_get_attribute_model_tag(tag,tag_name);
		}

		for (k=0;k!=max_model_halo;k++) {
			if (k==0) {
				strcpy(tag_name,"halo_bone");
			}
			else {
				sprintf(tag_name,"halo_bone_%d",k);
			}
			model->tags.halo_bone_tag[k]=xml_get_attribute_model_tag(tag,tag_name);
		}

        model->tags.name_bone_tag=xml_get_attribute_model_tag(tag,"name_bone");
    }
	
        // hit boxes
    
    hit_box_tag=xml_findfirstchild("Hit_Boxes",model_head);
	if (hit_box_tag!=-1) {
	
		nhit_box=xml_countchildren(hit_box_tag);
		tag=xml_findfirstchild("Hit_Box",hit_box_tag);
		
		for (i=0;i!=nhit_box;i++) {
 				
				// add new hit box

			hit_box_idx=model_hit_box_add(model);
			if (hit_box_idx==-1) break;

				// fill in hit box

			hit_box=&model->hit_boxes[hit_box_idx];

			xml_get_attribute_text(tag,"name",hit_box->name,64);
			xml_get_attribute_3_coord_int(tag,"size",&hit_box->box.size.x,&hit_box->box.size.y,&hit_box->box.size.z);
			xml_get_attribute_3_coord_int(tag,"offset",&hit_box->box.offset.x,&hit_box->box.offset.y,&hit_box->box.offset.z);
			
			tag=xml_findnextchild(tag);
		}
    }
	else {
		model->nhit_box=0;
	}

		// rigid body

    rigid_body_tag=xml_findfirstchild("Rigid_Body",model_head);
	if (rigid_body_tag!=-1) {

		model->rigid_body.on=xml_get_attribute_boolean(rigid_body_tag,"on");

		model->rigid_body.y.max_ang=0.0f;			// unused, but in struct
		model->rigid_body.y.reset_factor=xml_get_attribute_float_default(rigid_body_tag,"y_factor",0.8f);
		model->rigid_body.y.smooth_factor=xml_get_attribute_float_default(rigid_body_tag,"y_smooth",0.2f);
		model->rigid_body.x.max_ang=xml_get_attribute_float_default(rigid_body_tag,"x_max_ang",45.0f);
		model->rigid_body.x.reset_factor=xml_get_attribute_float_default(rigid_body_tag,"x_factor",0.8f);
		model->rigid_body.x.smooth_factor=xml_get_attribute_float_default(rigid_body_tag,"x_smooth",0.2f);
		model->rigid_body.z.max_ang=xml_get_attribute_float_default(rigid_body_tag,"z_max_ang",45.0f);
		model->rigid_body.z.reset_factor=xml_get_attribute_float_default(rigid_body_tag,"z_factor",0.8f);
		model->rigid_body.z.smooth_factor=xml_get_attribute_float_default(rigid_body_tag,"z_smooth",0.2f);

	}
	else {
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
	}
	
		// importing
		
    import_tag=xml_findfirstchild("Import",model_head);
	if (import_tag!=-1) {
		model->import.factor=xml_get_attribute_float_default(import_tag,"factor",1.0f);
	}
	else {
		model->import.factor=1.0f;
	}
	
  		// ui
		
    ui_tag=xml_findfirstchild("UI",model_head);
	if (ui_tag!=-1) {
		xml_get_attribute_color(ui_tag,"ambient",&model->ui.ambient);
		xml_get_attribute_3_coord_float(ui_tag,"diffuse_vector",&model->ui.diffuse_vct.x,&model->ui.diffuse_vct.y,&model->ui.diffuse_vct.z);
	}
	else {
		model->ui.ambient.r=0.75f;
		model->ui.ambient.g=0.75f;
		model->ui.ambient.b=0.75f;

		model->ui.diffuse_vct.x=0.0f;
		model->ui.diffuse_vct.y=-1.0f;
		model->ui.diffuse_vct.z=0.0f;
	}
 
        // bones
 
    bone_tag=xml_findfirstchild("Bones",model_head);

    nbone=xml_countchildren(bone_tag);
	tag=xml_findfirstchild("Bone",bone_tag);
	
	bone_parent_tags=(model_tag*)malloc(max_model_bone*sizeof(model_tag));
    
    for (i=0;i!=nbone;i++) {

 			// add new bone

		bone_idx=model_bone_add(model,0,0,0);
		if (bone_idx==-1) break;

			// fill in bone

		bone=&model->bones[bone_idx];
		
		bone->tag=xml_get_attribute_model_tag(tag,"tag");
		xml_get_attribute_text(tag,"name",bone->name,name_str_len);
        
        xml_get_attribute_3_coord_int(tag,"c3",&bone->pnt.x,&bone->pnt.y,&bone->pnt.z);
        bone_parent_tags[i]=xml_get_attribute_model_tag(tag,"parent");
    
		tag=xml_findnextchild(tag);
    }
	
		// find the light bone
		
	for (k=0;k!=max_model_light;k++) {
		model->tags.light_bone_idx[k]=model_find_bone(model,model->tags.light_bone_tag[k]);
	}

	for (k=0;k!=max_model_halo;k++) {
		model->tags.halo_bone_idx[k]=model_find_bone(model,model->tags.halo_bone_tag[k]);
	}

	model->tags.name_bone_idx=model_find_bone(model,model->tags.name_bone_tag);
		
		// reset the bones from tags to indexes
		
	bone=model->bones;
	
	for (i=0;i!=model->nbone;i++) {
		bone->parent_idx=model_find_bone(model,bone_parent_tags[i]);
		bone++;
	}
	
	free(bone_parent_tags);
	
		// meshes
		
	had_tangent=FALSE;
		
	meshes_tag=xml_findfirstchild("Meshes",model_head);

    nmesh=xml_countchildren(meshes_tag);
	mesh_tag=xml_findfirstchild("Mesh",meshes_tag);
	
	model->nmesh=1;				// new models have a single mesh
	
	for (j=0;j!=nmesh;j++) {
	
			// need new mesh?
			
		mesh_idx=j;
		if (j>0) {
			mesh_idx=model_mesh_add(model);
		}
		
		if (mesh_idx==-1) break;
	
			// mesh settings
			
		mesh=&model->meshes[mesh_idx];
		xml_get_attribute_text(mesh_tag,"name",mesh->name,name_str_len);
		mesh->no_lighting=xml_get_attribute_boolean(mesh_tag,"no_lighting");
		mesh->diffuse=xml_get_attribute_boolean(mesh_tag,"diffuse");
		mesh->blend_add=xml_get_attribute_boolean(mesh_tag,"additive");
		mesh->tintable=xml_get_attribute_boolean(mesh_tag,"tintable");
		xml_get_attribute_3_coord_int(mesh_tag,"import_move",&mesh->import_move.x,&mesh->import_move.y,&mesh->import_move.z);
		
			// don't allow blank mesh names
			
		if (mesh->name[0]==0x0) {
			if (mesh_idx==0) {
				strcpy(mesh->name,"Default");
			}
			else {
				sprintf(mesh->name,"Mesh %d",mesh_idx);
			}
		}
		
			// vertexes
		
		vertex_tag=xml_findfirstchild("Vertexes",mesh_tag);

		mesh->nvertex=xml_countchildren(vertex_tag);
		tag=xml_findfirstchild("v",vertex_tag);

		model_mesh_set_vertex_count(model,mesh_idx,mesh->nvertex);
		
		major_bone_tags=(model_tag*)malloc(mesh->nvertex*sizeof(model_tag));
		minor_bone_tags=(model_tag*)malloc(mesh->nvertex*sizeof(model_tag));
		
		vertex=mesh->vertexes;
		
		for (i=0;i!=mesh->nvertex;i++) {
			xml_get_attribute_3_coord_int(tag,"c3",&vertex->pnt.x,&vertex->pnt.y,&vertex->pnt.z);
			
			major_bone_tags[i]=xml_get_attribute_model_tag(tag,"major");
			minor_bone_tags[i]=xml_get_attribute_model_tag(tag,"minor");
			vertex->bone_factor=xml_get_attribute_float_default(tag,"factor",1);
		
			vertex++;
			tag=xml_findnextchild(tag);
		}
		
			// count the trigs
			
		materials_tag=xml_findfirstchild("Materials",mesh_tag);

		nfill=xml_countchildren(materials_tag);
		material_tag=xml_findfirstchild("Material",materials_tag);

		mesh->ntrig=0;
		
		for (n=0;n!=nfill;n++) {
			trig_tag=xml_findfirstchild("Triangles",material_tag);
			mesh->ntrig+=(xml_countchildren(trig_tag)/3);
			material_tag=xml_findnextchild(material_tag);
		}

		model_mesh_set_trig_count(model,mesh_idx,mesh->ntrig);

			// run the materials
		
		trig_idx=0;
		trig=mesh->trigs;

		texture=model->textures;
		material=mesh->materials;
		
		material_tag=xml_findfirstchild("Material",materials_tag);
		
		for (n=0;n!=nfill;n++) {
		
			trig_tag=xml_findfirstchild("Triangles",material_tag);
			
			material->trig_start=trig_idx;
			material->trig_count=xml_countchildren(trig_tag)/3;
			
			vtag=xml_findfirstchild("v",trig_tag);
		
			for (i=0;i!=material->trig_count;i++) {
		
				for (k=0;k!=3;k++) {
					trig->v[k]=xml_get_attribute_int(vtag,"id");
					xml_get_attribute_2_coord_float(vtag,"uv",&trig->gx[k],&trig->gy[k]);

					had_tangent=xml_get_attribute_3_coord_float(vtag,"t3",&trig->tangent_space[k].tangent.x,&trig->tangent_space[k].tangent.y,&trig->tangent_space[k].tangent.z);
					xml_get_attribute_3_coord_float(vtag,"b3",&trig->tangent_space[k].binormal.x,&trig->tangent_space[k].binormal.y,&trig->tangent_space[k].binormal.z);
					xml_get_attribute_3_coord_float(vtag,"n3",&trig->tangent_space[k].normal.x,&trig->tangent_space[k].normal.y,&trig->tangent_space[k].normal.z);

					vtag=xml_findnextchild(vtag);
				}
				
				trig++;
				trig_idx++;
			}
			
			texture++;
			material++;
			
			material_tag=xml_findnextchild(material_tag);
		}
		
			// reset the vertexes from tags to indexes
			
		vertex=mesh->vertexes;
		
		for (i=0;i!=mesh->nvertex;i++) {
			vertex->major_bone_idx=model_find_bone(model,major_bone_tags[i]);
			vertex->minor_bone_idx=model_find_bone(model,minor_bone_tags[i]);
			if ((vertex->major_bone_idx==-1) || (vertex->minor_bone_idx==-1)) vertex->bone_factor=1;
			vertex++;
		}
		
		free(major_bone_tags);
		free(minor_bone_tags);
		
		mesh_tag=xml_findnextchild(mesh_tag);
	}
	
	model->nmesh=nmesh;
  
        // fills

    fills_tag=xml_findfirstchild("Fills",model_head);
	nfill=xml_countchildren(fills_tag);

	texture=model->textures;
	
	fill_tag=xml_findfirstchild("Fill",fills_tag);
    
    for (n=0;n!=nfill;n++) {
	
		texture->animate.current_frame=0;
		texture->animate.total_wait=0;
		
		bitmap_texture_read_xml(texture,fill_tag,FALSE);
		texture++;
		
		fill_tag=xml_findnextchild(fill_tag);
    }
	
		// if no tangent space, then calculate it
		
	if (!had_tangent) model_recalc_normals(model,FALSE);
}

/* =======================================================

      Write Mesh v2 XML
      
======================================================= */

void encode_mesh_v2_xml(model_type *model)
{
	int						i,n,k,j,frame_count;
	char					tag_name[32];
	model_hit_box_type		*hit_box;
	model_mesh_type			*mesh;
    model_vertex_type		*vertex;
	model_material_type		*material;
    model_bone_type			*bone;
    model_trig_type			*trig;
    texture_type			*texture;
    
        // model info
    
    xml_add_tagstart("Creator");
    xml_add_attribute_text("name","dim3 Animator");
    xml_add_attribute_text("version","1.0");
    xml_add_tagend(TRUE);
	
        // options
    
    xml_add_tagstart("Options");
	xml_add_attribute_list("deform",(char*)deform_mode_str,model->deform_mode);
	xml_add_attribute_float("diffuse_boost",model->diffuse_boost);
    xml_add_tagend(TRUE);
 	
        // center
	
    xml_add_tagstart("Center");
    xml_add_attribute_3_coord_int("offset",model->center.x,model->center.y,model->center.z);
    xml_add_tagend(TRUE);
    
        // boxes
    
    xml_add_tagstart("View_Box");
    xml_add_attribute_3_coord_int("size",model->view_box.size.x,model->view_box.size.y,model->view_box.size.z);
    xml_add_attribute_3_coord_int("offset",model->view_box.offset.x,model->view_box.offset.y,model->view_box.offset.z);
    xml_add_tagend(TRUE);
	
		// light
	
    xml_add_tagstart("Light");

	for (k=0;k!=max_model_light;k++) {
		if (k==0) {
			strcpy(tag_name,"light_bone");
		}
		else {
			sprintf(tag_name,"light_bone_%d",k);
		}
		xml_add_attribute_model_tag(tag_name,model->tags.light_bone_tag[k]);
	}

	for (k=0;k!=max_model_halo;k++) {
		if (k==0) {
			strcpy(tag_name,"halo_bone");
		}
		else {
			sprintf(tag_name,"halo_bone_%d",k);
		}
 		xml_add_attribute_model_tag(tag_name,model->tags.halo_bone_tag[k]);
	}

 	xml_add_attribute_model_tag("name_bone",model->tags.name_bone_tag);

	xml_add_tagend(TRUE);
	
		// hit boxes
		
    xml_add_tagstart("Hit_Boxes");
    xml_add_tagend(FALSE);
    
    hit_box=model->hit_boxes;
    
    for (i=0;i!=model->nhit_box;i++) {
		xml_add_tagstart("Hit_Box");
		xml_add_attribute_text("name",hit_box->name);
		xml_add_attribute_3_coord_int("size",hit_box->box.size.x,hit_box->box.size.y,hit_box->box.size.z);
		xml_add_attribute_3_coord_int("offset",hit_box->box.offset.x,hit_box->box.offset.y,hit_box->box.offset.z);
		xml_add_tagend(TRUE);
		
		hit_box++;
	}
	
    xml_add_tagclose("Hit_Boxes");

		// rigid body

    xml_add_tagstart("Rigid_Body");
	xml_add_attribute_boolean("on",model->rigid_body.on);
	xml_add_attribute_float("y_factor",model->rigid_body.y.reset_factor);
	xml_add_attribute_float("y_smooth",model->rigid_body.y.smooth_factor);
	xml_add_attribute_float("x_max_ang",model->rigid_body.x.max_ang);
	xml_add_attribute_float("x_factor",model->rigid_body.x.reset_factor);
	xml_add_attribute_float("x_smooth",model->rigid_body.x.smooth_factor);
	xml_add_attribute_float("z_max_ang",model->rigid_body.z.max_ang);
	xml_add_attribute_float("z_factor",model->rigid_body.z.reset_factor);
	xml_add_attribute_float("z_smooth",model->rigid_body.z.smooth_factor);
    xml_add_tagend(TRUE);
	
		// import
		
    xml_add_tagstart("Import");
	xml_add_attribute_float("factor",model->import.factor);
    xml_add_tagend(TRUE);

  		// ui
		
    xml_add_tagstart("UI");
	xml_add_attribute_color("ambient",&model->ui.ambient);
	xml_add_attribute_3_coord_float("diffuse_vector",model->ui.diffuse_vct.x,model->ui.diffuse_vct.y,model->ui.diffuse_vct.z);
	xml_add_tagend(TRUE);
 
        // bones
        
    xml_add_tagstart("Bones");
    xml_add_tagend(FALSE);
    
    bone=model->bones;
    
    for (i=0;i!=model->nbone;i++) {
    
        xml_add_tagstart("Bone");
        
        xml_add_attribute_model_tag("tag",bone->tag);
		xml_add_attribute_text("name",bone->name);
		
        xml_add_attribute_3_coord_int("c3",bone->pnt.x,bone->pnt.y,bone->pnt.z);
		
        if (bone->parent_idx!=-1) xml_add_attribute_model_tag("parent",model->bones[bone->parent_idx].tag);
        
        xml_add_tagend(TRUE);
    
        bone++;
    }

    xml_add_tagclose("Bones");
	
        // meshes
        
    xml_add_tagstart("Meshes");
    xml_add_tagend(FALSE);
	
	for (j=0;j!=model->nmesh;j++) {
	
		mesh=&model->meshes[j];
	
		xml_add_tagstart("Mesh");
		xml_add_attribute_text("name",mesh->name);
		xml_add_attribute_boolean("no_lighting",mesh->no_lighting);
		xml_add_attribute_boolean("diffuse",mesh->diffuse);
		xml_add_attribute_boolean("additive",mesh->blend_add);
		xml_add_attribute_boolean("tintable",mesh->tintable);
		xml_add_attribute_3_coord_int("import_move",mesh->import_move.x,mesh->import_move.y,mesh->import_move.z);
		xml_add_tagend(FALSE);
	   
			// vertexes
			
		xml_add_tagstart("Vertexes");
		xml_add_tagend(FALSE);
		
		vertex=mesh->vertexes;
		
		for (i=0;i!=mesh->nvertex;i++) {
		
			xml_add_tagstart("v");
			
			xml_add_attribute_3_coord_int("c3",vertex->pnt.x,vertex->pnt.y,vertex->pnt.z);

			if (vertex->major_bone_idx!=-1) {
				xml_add_attribute_model_tag("major",model->bones[vertex->major_bone_idx].tag);
			}
			if (vertex->minor_bone_idx!=-1) {
				xml_add_attribute_model_tag("minor",model->bones[vertex->minor_bone_idx].tag);
				xml_add_attribute_float("factor",vertex->bone_factor);
			}
			
			xml_add_tagend(TRUE);
		
			vertex++;
		}
						
		xml_add_tagclose("Vertexes");
		
			// materials
			
		xml_add_tagstart("Materials");
		xml_add_tagend(FALSE);
		
		texture=model->textures;
		material=mesh->materials;
		
		for (n=0;n!=max_model_texture;n++) {
		
			if (texture->frames[0].name[0]==0x0) {
				texture++;
				material++;
				continue;
			}
			
			xml_add_tagstart("Material");
			xml_add_tagend(FALSE);

				// triangles
				
			xml_add_tagstart("Triangles");
			xml_add_tagend(FALSE);
		
			trig=&mesh->trigs[material->trig_start];
		
			for (i=0;i!=material->trig_count;i++) {
		
				for (k=0;k!=3;k++) {
					xml_add_tagstart("v");
					
					xml_add_attribute_int("id",trig->v[k]);
					xml_add_attribute_2_coord_float("uv",trig->gx[k],trig->gy[k]);

					xml_add_attribute_3_coord_float("t3",trig->tangent_space[k].tangent.x,trig->tangent_space[k].tangent.y,trig->tangent_space[k].tangent.z);
					xml_add_attribute_3_coord_float("b3",trig->tangent_space[k].binormal.x,trig->tangent_space[k].binormal.y,trig->tangent_space[k].binormal.z);
					xml_add_attribute_3_coord_float("n3",trig->tangent_space[k].normal.x,trig->tangent_space[k].normal.y,trig->tangent_space[k].normal.z);
					
					xml_add_tagend(TRUE);
				}
				
				trig++;
			}
			
			xml_add_tagclose("Triangles");
			xml_add_tagclose("Material");
			
			texture++;
			material++;
		}
		
		xml_add_tagclose("Materials");
		
		xml_add_tagclose("Mesh");
	}
	
	xml_add_tagclose("Meshes");
   
         // fills
         
    xml_add_tagstart("Fills");
    xml_add_tagend(FALSE);
    
    texture=model->textures;
    
    for (n=0;n!=max_model_texture;n++) {
    
        if (texture->frames[0].name[0]==0x0) {
            texture++;
            continue;
        }

		frame_count=model_count_texture_frames(model,n);
        
        xml_add_tagstart("Fill");
		bitmap_texture_write_xml(texture,frame_count,FALSE);
        xml_add_tagclose("Fill");
        
		texture++;
    }
    
    xml_add_tagclose("Fills");
}
