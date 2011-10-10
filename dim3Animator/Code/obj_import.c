/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Model OBJ importer

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
#include "interface.h"
#include "ui_common.h"

#define obj_max_face_vertex		128

extern model_type				model;
extern animator_state_type		state;

/* =======================================================

      Import OBJ Re-Rig Vertexes
      
======================================================= */

void import_obj_rerig(model_mesh_type *mesh,int old_nvertex,model_vertex_type *old_vertex)
{
	int						n,k,v_idx,
							cur_dist,dist,dist2;
	model_vertex_type		*v1,*v2;
	model_bone_type			*bone;
		
		// first find vertexes that are equal

	v1=mesh->vertexes;

	for (n=0;n!=mesh->nvertex;n++) {
	
		v2=old_vertex;
	
		for (k=0;k!=old_nvertex;k++) {
			
			if ((v1->pnt.x==v2->pnt.x) && (v1->pnt.y==v2->pnt.y) && (v1->pnt.z==v2->pnt.z)) {
				v1->major_bone_idx=v2->major_bone_idx;
				v1->minor_bone_idx=v2->minor_bone_idx;
				v1->bone_factor=v2->bone_factor;
				break;
			}
			
			v2++;
		}
		
		v1++;
	}
	
		// now deal with vertexes within
		// 10% of original distance
		
	v1=mesh->vertexes;

	for (n=0;n!=mesh->nvertex;n++) {
	
			// find the closest vertex
			// by distance
			
		v_idx=-1;
		cur_dist=0;
		
		v2=old_vertex;
	
		for (k=0;k!=old_nvertex;k++) {
		
				// skip if not attached
			
			if (v2->major_bone_idx==-1) {
				v2++;
				continue;
			}
			
				// get distance
				
			dist=distance_get(v1->pnt.x,v1->pnt.y,v1->pnt.z,v2->pnt.x,v2->pnt.y,v2->pnt.z);
			if ((v_idx==-1) || (dist<cur_dist)) {
				v_idx=k;
				cur_dist=dist;
			}
			
			v2++;
		}
		
			// if there was a hit, figure
			// out if it's within 10% of the
			// bone distance
			
		if (v_idx!=-1) {
			v2=&old_vertex[v_idx];
			bone=&model.bones[v2->major_bone_idx];
			
			dist=distance_get(bone->pnt.x,bone->pnt.y,bone->pnt.z,v2->pnt.x,v2->pnt.y,v2->pnt.z);
			dist2=distance_get(bone->pnt.x,bone->pnt.y,bone->pnt.z,v1->pnt.x,v1->pnt.y,v1->pnt.z);
						
			if (abs(dist2-dist)<(dist/10)) {
				v1->major_bone_idx=v2->major_bone_idx;
				v1->minor_bone_idx=v2->minor_bone_idx;
				v1->bone_factor=v2->bone_factor;
			}
		}
		
		v1++;
	}
}

/* =======================================================

      Import OBJ File
      
======================================================= */

bool import_obj(char *path,bool replace,bool *found_normals,char *err_str)
{
	int						n,k,i,idx,nvertex,ntrig,nuv,nnormal,nline,nmaterial,
							texture_idx,high,npt,old_nvertex,sz,
							pvtx[obj_max_face_vertex];
	float					fy,f_ty,f_by;
	char					txt[256],*c,vstr[256],vtstr[256],vnstr[256],
							material_name[256],
							material_list[max_model_texture][name_str_len];
	bool					hit_material,in_material,all_material_exists;
	d3uv					*uv,*uv_ptr,pt_uv[obj_max_face_vertex];
	d3vct					*normal,*normal_ptr,pnormal[obj_max_face_vertex];
	model_mesh_type			*mesh;
	model_vertex_type		*vertex,*old_vertex;
    model_trig_type			*trig;
	model_material_type		*material;
	
	mesh=&model.meshes[state.cur_mesh_idx];
	
		// clear mesh materials
    
    clear_materials();
	
		// load the file
		
	if (!textdecode_open(path,0x20)) {
		strcpy(err_str,"Couldn't open file.");
		return(FALSE);
    }
	
	progress_start("OBJ Import",(replace?4:3));
	progress_next_title("Obj Import: Parsing OBJ");
	
    nline=textdecode_count();
	
		// count materials and get a list
		// and see if we already have a material
		// for all the textures
		
	nmaterial=0;
	all_material_exists=TRUE;
	
    for (n=0;n!=nline;n++) {
        textdecode_get_piece(n,0,txt);
        if (strcmp(txt,"usemtl")==0) {
		
            textdecode_get_piece(n,1,material_name);
			material_name[name_str_len-1]=0x0;

				// already doing this material?

			hit_material=FALSE;

			for (i=0;i!=nmaterial;i++) {
				if (strcasecmp(material_list[i],material_name)==0) {
					hit_material=TRUE;
					break;
				}
			}

			if (!hit_material) {
				strcpy(material_list[nmaterial],material_name);
				nmaterial++;
			}

			all_material_exists&=texture_exists(material_name);
		}
	}

		// can't import if no textures
		
	if (nmaterial==0) {
		progress_end();
		textdecode_close();
		strcpy(err_str,"OBJ contains no materials.  Animator can only import texture-mapped OBJs.");
		return(FALSE);
	}
	
		// can't import if too many materials?
		
	if ((nmaterial+texture_count())>=max_model_texture) {
		progress_end();
		textdecode_close();
		sprintf(err_str,"Too many materials, models can have a maximum of %d materials.",max_model_texture);
		return(FALSE);
	}
	
		// count the vertexes, uvs, normals, and trigs
		// and find the total height
		
	nvertex=0;
	nuv=0;
	nnormal=0;
	ntrig=0;
	
	f_ty=f_by=0;

    for (n=0;n!=nline;n++) {
        textdecode_get_piece(n,0,txt);
		
        if (strcmp(txt,"v")==0) {
			
			textdecode_get_piece(n,2,txt);
			fy=(float)strtod(txt,NULL);
			
			if (nvertex==0) {
				f_ty=f_by=fy;
			}
			else {
				if (fy<f_ty) f_ty=fy;
				if (fy>f_by) f_by=fy;
			}
			
			nvertex++;
			continue;
		}
		
        if (strcmp(txt,"vt")==0) {
			nuv++;
			continue;
		}
		
        if (strcmp(txt,"vn")==0) {
			nnormal++;
			continue;
		}

		if (strcmp(txt,"f")==0) {
        
			npt=0;
        
			for (k=0;k!=obj_max_face_vertex;k++) {
				textdecode_get_piece(n,(k+1),txt);
				if (txt[0]==0x0) break;
				
				npt++;
			}
			
			ntrig+=(npt-2);
		}
	}
	
		// can't import if no UVs
		
	if (nuv==0) {
		progress_end();
		textdecode_close();
		sprintf(err_str,"There are no UVs in this OBJ, please texture map the model before importing.");
		return(FALSE);
	}
	
		// get the scale if not a replace
		
	if (!replace) {
		high=dialog_import_finish_run();
		model.import.factor=((float)high)/fabsf(f_by-f_ty);
	}

		// if a replacement, remember the old
		// vertexes so we can reattach bones
		
	if (replace) {
		old_nvertex=mesh->nvertex;
		sz=sizeof(model_vertex_type)*old_nvertex;
		old_vertex=(model_vertex_type*)malloc(sz);
		memmove(old_vertex,mesh->vertexes,sz);
	}
	
		// model memory
		
	progress_next_title("Obj Import: Building New Model");
		
	model_mesh_set_vertex_count(&model,state.cur_mesh_idx,nvertex);
	model_mesh_set_trig_count(&model,state.cur_mesh_idx,ntrig);
	uv_ptr=(d3uv*)malloc(nuv*sizeof(d3uv));
	
	if (nnormal==0) {
		normal_ptr=(d3vct*)malloc(sizeof(d3vct));
	}
	else {
		normal_ptr=(d3vct*)malloc(nnormal*sizeof(d3vct));
	}
	
		// get the vertexes, uv, and normals

	vertex=mesh->vertexes;
    
    uv=uv_ptr;
	normal=normal_ptr;
   
    for (n=0;n!=nline;n++) {

        textdecode_get_piece(n,0,txt);
      
            // a vertex
            
        if (strcmp(txt,"v")==0) {
			textdecode_get_piece(n,1,txt);
			vertex->pnt.x=-(int)(strtod(txt,NULL)*model.import.factor);
			textdecode_get_piece(n,2,txt);
			vertex->pnt.y=-(int)(strtod(txt,NULL)*model.import.factor);
			textdecode_get_piece(n,3,txt);
			vertex->pnt.z=-(int)(strtod(txt,NULL)*model.import.factor);
            
            vertex->major_bone_idx=vertex->minor_bone_idx=-1;
            vertex->bone_factor=1;
       
            vertex++;
        }
        else {
        
            // a uv
            
            if (strcmp(txt,"vt")==0) {
                textdecode_get_piece(n,1,txt);
                uv->x=(float)strtod(txt,NULL);
                textdecode_get_piece(n,2,txt);
                uv->y=(float)strtod(txt,NULL);
                
 				uv++;
            }
			else {
			
				// a normal
				
				if (strcmp(txt,"vn")==0) {
					textdecode_get_piece(n,1,txt);
					normal->x=-(float)strtod(txt,NULL);
					textdecode_get_piece(n,2,txt);
					normal->y=-(float)strtod(txt,NULL);
					textdecode_get_piece(n,3,txt);
					normal->z=-(float)strtod(txt,NULL);
					
					normal++;
				}
			}
        }
    }

	*found_normals=(nnormal!=0);

		// get the triangles
		// we run through the materials to
		// combine triangles that are on different
		// materials but with the same name

    ntrig=0;
	trig=mesh->trigs;

	for (i=0;i!=nmaterial;i++) {

            // get texture
  
		texture_idx=texture_pick(material_list[i],err_str);
		if (texture_idx==-1) {
			progress_end();
			textdecode_close();
			return(FALSE);
		}

		material=&mesh->materials[texture_idx];
		material->trig_start=ntrig;

		in_material=FALSE;

			// run through the trigs

		for (n=0;n!=nline;n++) {

			textdecode_get_piece(n,0,txt);
	        
				// material change
	            
			if (strcmp(txt,"usemtl")==0) {
				textdecode_get_piece(n,1,material_name);
				material_name[name_str_len-1]=0x0;

				in_material=(strcasecmp(material_name,material_list[i])==0);
				continue;
			}

			if (!in_material) continue;

				// a face
	            
			if (strcmp(txt,"f")!=0) continue;
	        
				// get the face points
	        
			npt=0;
	        
			for (k=0;k!=obj_max_face_vertex;k++) {
				textdecode_get_piece(n,(k+1),txt);
				if (txt[0]==0x0) break;
	            
					// seperate into vertex, UV, and normal
					
				vtstr[0]=0x0;
				vnstr[0]=0x0;
	            
				strcpy(vstr,txt);
				c=strchr(vstr,'/');
				if (c!=NULL) {
					strcpy(vtstr,(c+1));
					*c=0x0;
				}
				c=strchr(vtstr,'/');
				if (c!=NULL) {
					strcpy(vnstr,(c+1));
					*c=0x0;
				}
				
				pvtx[npt]=atoi(vstr);
				if (pvtx[npt]>0) {
					pvtx[npt]--;
				}
				else {
					pvtx[npt]=nvertex+pvtx[npt];
				}
	            
				if (vtstr[0]==0x0) {
					pt_uv[npt].x=pt_uv[npt].y=0.0f;
				}
				else {
					idx=atoi(vtstr);
					if (idx>0) {
						idx--;
					}
					else {
						idx=nuv+idx;
					}
					
					pt_uv[npt].x=uv_ptr[idx].x;
					pt_uv[npt].y=1.0f-uv_ptr[idx].y;
				}
					
				if (vnstr[0]!=0x0) {
					idx=atoi(vnstr);
					if (idx>0) {
						idx--;
					}
					else {
						idx=nnormal+idx;
					}
					
					memmove(&pnormal[npt],&normal_ptr[idx],sizeof(d3vct));
				}
				
				npt++;
			}
			
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
				
				memmove(&trig->tangent_space[0].normal,&pnormal[0],sizeof(d3vct));
				memmove(&trig->tangent_space[1].normal,&pnormal[k+1],sizeof(d3vct));
				memmove(&trig->tangent_space[2].normal,&pnormal[k+2],sizeof(d3vct));
	            
				trig++;
				ntrig++;
			}
		}

		material->trig_count=ntrig-material->trig_start;
	}
	
	mesh->ntrig=ntrig;
	
	free(uv_ptr);
	free(normal_ptr);
	
	textdecode_close();
	
		// finish up
		
	vertex_delete_unused_vertexes(state.cur_mesh_idx);

	model_center_xz(&model,state.cur_mesh_idx);
	model_floor(&model,state.cur_mesh_idx);
	model_recalc_boxes(&model);
    model_recalc_normals(&model,(*found_normals));

		// if replace, fix by moving
		// any user mesh moves

	if (replace) {
		vertex=mesh->vertexes;

		for (n=0;n!=mesh->nvertex;n++) {
			vertex->pnt.x+=mesh->import_move.x;
 			vertex->pnt.y+=mesh->import_move.y;
			vertex->pnt.z+=mesh->import_move.z;
       
            vertex++;
        }
	}
	else {
		mesh->import_move.x=0;
		mesh->import_move.y=0;
		mesh->import_move.z=0;
	}

		// if replacement, fix all the
		// bone attachments
		
	if (replace) {
		progress_next_title("Obj Import: Fixing Bone Rigging");
		import_obj_rerig(mesh,old_nvertex,old_vertex);
		free(old_vertex);
	}
	
	model_calculate_parents(&model);
	
	progress_end();

	return(TRUE);
}

