/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Model C4D XML importer

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

#include "interface.h"
#include "dialog.h"

#define obj_max_face_vertex		128

extern model_type				model;
extern animator_state_type		state;

/* =======================================================

      Import C4D XML File
      
======================================================= */

bool import_c4d_xml(char *path,char *err_str)
{
	int						n,k,nuv,npoly,nvertex,ntrig,nbone,uv_idx,texture_idx,
							tag,uv_tag,poly_tag,vertex_tag,bone_tag,x,y,z,nattach,
							pvtx[obj_max_face_vertex],npt;
	int						*v_major_bone_idx,*v_minor_bone_idx;
	float					*v_attach,*v_major_bone_value,*v_minor_bone_value;
	char					name[32];
	d3uv					*uv_ptr,pt_uv[obj_max_face_vertex];
	model_mesh_type			*mesh;
	model_vertex_type		*vertex;
    model_trig_type			*trig;
	model_bone_type			*bone;
	model_material_type		*material;
	
		// clear mesh materials
    
    clear_materials();
	
		// load the file
		
	if (!xml_open_file(path)) {
		strcpy(err_str,"Couldn't open file.");
		return(FALSE);
    }
	
	mesh=&model.meshes[state.cur_mesh_idx];
	
		// supergumba -- we need a lot of work here, for now let's assume 1 texture
		
		// get the material
		
	texture_idx=texture_pick("Default",err_str);
	if (texture_idx==-1) {
		xml_close_file();
		return(FALSE);
	}

	material=&mesh->materials[texture_idx];
	
		// get the tags
		// uvs at tag_uv/tag_uv/vectorarray
		
	uv_tag=xml_findflat("tag_uvw",0);
	uv_tag=xml_findfirstchild("tag_uvw",uv_tag);
	uv_tag=xml_findfirstchild("vectorarray",uv_tag);
	
		// polys at tag_polygon/tag_polygon/polygonarray
		
	poly_tag=xml_findflat("tag_polygon",0);
	poly_tag=xml_findfirstchild("tag_polygon",poly_tag);
	poly_tag=xml_findfirstchild("polygonarray",poly_tag);
	
		// vertexes at tag_point/tag_point/vectorarray

	vertex_tag=xml_findflat("tag_point",0);
	vertex_tag=xml_findfirstchild("tag_point",vertex_tag);
	vertex_tag=xml_findfirstchild("vectorarray",vertex_tag);
	
		// get the number of vertexes
		
	nvertex=xml_countchildren(vertex_tag);
	
	model_mesh_set_vertex_count(&model,state.cur_mesh_idx,nvertex);
	
		// bone arrays
		
	v_major_bone_idx=(int*)malloc(nvertex*sizeof(int));
	v_minor_bone_idx=(int*)malloc(nvertex*sizeof(int));
	v_attach=(float*)malloc(nvertex*sizeof(float));
	v_major_bone_value=(float*)malloc(nvertex*sizeof(float));
	v_minor_bone_value=(float*)malloc(nvertex*sizeof(float));
	
		// bring in uvs
	
	nuv=xml_countchildren(uv_tag);
	
	uv_ptr=(d3uv*)malloc(nuv*sizeof(d3uv));
	
	tag=xml_findfirstchild("vector",uv_tag);
	
	for (n=0;n!=nuv;n++) {
		uv_ptr[n].x=xml_get_attribute_float_default(tag,"x",0.0f);
		uv_ptr[n].y=xml_get_attribute_float_default(tag,"y",0.0f);
		tag=xml_findnextchild(tag);
	}
	
		// bring in vertexes
		
	vertex=mesh->vertexes;
	
	tag=xml_findfirstchild("vector",vertex_tag);
	
	for (n=0;n!=nvertex;n++) {
		vertex->pnt.x=(int)(xml_get_attribute_float_default(tag,"x",0.0f)*import_scale_factor);
		vertex->pnt.y=-(int)(xml_get_attribute_float_default(tag,"y",0.0f)*import_scale_factor);
		vertex->pnt.z=-(int)(xml_get_attribute_float_default(tag,"z",0.0f)*import_scale_factor);
		
		vertex->major_bone_idx=vertex->minor_bone_idx=-1;
		vertex->bone_factor=1.0f;
		
		vertex++;
		
		tag=xml_findnextchild(tag);
	}
	
		// bring in polygons
		
	npoly=xml_countchildren(poly_tag);
	
		// count polygons
		
	tag=xml_findfirstchild("polygon",poly_tag);

	ntrig=0;
	
	for (n=0;n!=npoly;n++) {
			
		npt=0;
		strcpy(name,"a");
		
		while (npt<obj_max_face_vertex) {
			k=xml_get_attribute_int_default(tag,name,-1);
			if (k==-1) break;
			
			name[0]++;
			npt++;
		}
		
		ntrig+=(npt-2);
		
		tag=xml_findnextchild(tag);
	}
	
	model_mesh_set_trig_count(&model,state.cur_mesh_idx,ntrig);
		
		// load polygons
	
	tag=xml_findfirstchild("polygon",poly_tag);

	ntrig=0;
	trig=mesh->trigs;
	
	uv_idx=0;
	
	for (n=0;n!=npoly;n++) {
	
			// find all the polygon vertexes
			
		npt=0;
		strcpy(name,"a");
		
		while (npt<obj_max_face_vertex) {
			k=xml_get_attribute_int_default(tag,name,-1);
			if (k==-1) break;
			
			pvtx[npt]=k;
			pt_uv[npt].x=uv_ptr[uv_idx].x;
			pt_uv[npt].y=uv_ptr[uv_idx++].y;
			
			name[0]++;
			npt++;
		}
		
			// tesellate into triangles
			
        for (k=0;k!=(npt-2);k++) {
            trig->v[0]=pvtx[0];
            trig->v[1]=pvtx[k+1];
            trig->v[2]=pvtx[k+2];
                
            trig->gx[0]=pt_uv[0].x;
			trig->gx[1]=pt_uv[k+1].x;
            trig->gx[2]=pt_uv[k+2].x;

			trig->gy[0]=pt_uv[0].y;
            trig->gy[1]=pt_uv[k+1].y;
            trig->gy[2]=pt_uv[k+2].y;
            
            trig++;
            ntrig++;
        }
		
		tag=xml_findnextchild(tag);
	}
	
		// don't need uv list anymore
		
	free(uv_ptr);
	
		// set trigs in material
		
	material->trig_start=0;
	material->trig_count=ntrig;
	
		// clear bone vertex attach list
		
	for (n=0;n!=nvertex;n++) {
		v_major_bone_idx[n]=-1;
		v_minor_bone_idx[n]=-1;
	}
	
		// grab the bones
		
	nbone=0;
	bone=model.bones;
	
	while (TRUE) {
		bone_tag=xml_findflat("tag_vertexmap",((nbone*2)+1));		// always skip ahead as bones have two definition blocks
		if (bone_tag==-1) break;
		
			// get bone name
			
		tag=xml_findfirstchild("string",bone_tag);
		xml_get_attribute_text(tag,"v",bone->name,name_str_len);
		
			// grab attached vertexes
			
		x=y=z=0;
		nattach=0;
		
		tag=xml_findfirstchild("realarray",bone_tag);
		tag=xml_findfirstchild("real",tag);
		
		for (k=0;k!=nvertex;k++) {
			v_attach[k]=xml_get_attribute_float_default(tag,"v",0.0f);
			
			if (v_attach[k]!=0.0f) {
			
					// get vertex data for bone position
					
				vertex=&mesh->vertexes[k];
				
				x+=vertex->pnt.x;
				y+=vertex->pnt.y;
				z+=vertex->pnt.z;
				nattach++;
				
					// find best attachments for vertexes
					// if no major bone, then make attachment the major
					// if there is a major, if attachment is greator, make major the
					// minor bone and attachment the major.  Otherwise, replace
					// the minor if attachment is greater
				
				if (v_major_bone_idx[k]==-1) {
					v_major_bone_idx[k]=nbone;
					v_major_bone_value[k]=v_attach[k];
				}
				else {
					if (v_attach[k]>v_major_bone_value[k]) {
						v_minor_bone_idx[k]=v_major_bone_idx[k];	// move old major bone to minor
						v_minor_bone_value[k]=v_major_bone_value[k];
						v_major_bone_idx[k]=nbone;
						v_major_bone_value[k]=v_attach[k];
					}
					else {
						if (v_attach[k]>v_minor_bone_value[k]) {
							v_minor_bone_idx[k]=nbone;
							v_minor_bone_value[k]=v_attach[k];
						}
					}
				}
			}
			
			tag=xml_findnextchild(tag);
		}
		
		if (nattach!=0) {
			x/=nattach;
			y/=nattach;
			z/=nattach;
		}
			
			// setup bone
		
		bone->parent_idx=-1;
		bone->pnt.x=x;
		bone->pnt.y=y;
		bone->pnt.z=z;
		
		if (nbone<10) {
			sprintf(name,"bn0%d",nbone);
		}
		else {
			sprintf(name,"bn%d",nbone);
		}
		bone->tag=text_to_model_tag(name);
		
		bone++;
		nbone++;
		
		if (nbone>=max_model_bone) {
			xml_close_file();
			sprintf(err_str,"Too many bones, models can have a maximum of %d bones.",max_model_bone);
			return(FALSE);
		}
	}
	
	model.nbone=nbone;
	
		// attach all the vertexes to the bones
		
	vertex=mesh->vertexes;
	
	for (n=0;n!=nvertex;n++) {
		
		if (v_major_bone_idx[n]!=-1) {
			vertex->major_bone_idx=v_major_bone_idx[n];
			
			if (v_minor_bone_idx[n]!=-1) {
				vertex->minor_bone_idx=v_minor_bone_idx[n];
				vertex->bone_factor=1.0f-(v_minor_bone_value[n]*(1.0/v_major_bone_value[n]));
			}
		}
	
		vertex++;
	}
	
		// don't need bone lists
		
	free(v_major_bone_idx);
	free(v_minor_bone_idx);
	free(v_attach);
	free(v_major_bone_value);
	free(v_minor_bone_value);
	
		// close xml file
		
	xml_close_file();

	return(TRUE);
}

