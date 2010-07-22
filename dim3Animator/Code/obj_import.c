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

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#include "dialog.h"
#include "import.h"

#define obj_max_face_vertex		128

extern int						cur_mesh;
extern model_type				model;

/* =======================================================

      Import OBJ File
      
======================================================= */

bool import_obj(char *path,bool *found_normals,char *err_str)
{
	int						n,k,t,nvertex,ntrig,nobj_uv,nobj_normal,nline,ntexture,texture_idx,
							pvtx[obj_max_face_vertex],npt;
	char					txt[256],*c,vstr[256],vtstr[256],vnstr[256],
							material_name[256],last_material_name[256];
	bool					single_material,first_material;
	d3uv					*uv,*uv_ptr,pt_uv[obj_max_face_vertex];
	d3vct					*normal,*normal_ptr,pnormal[obj_max_face_vertex];
	model_vertex_type		*vertex;
    model_trig_type			*trig;
	model_material_type		*material;
	
		// clear mesh materials
    
    clear_materials();
	
		// load the file
		
	if (!textdecode_open(path,0x20)) {
		strcpy(err_str,"Couldn't open file.");
		return(FALSE);
    }
	
    nline=textdecode_count();
	
		// count materials
		
	ntexture=0;
	
    for (n=0;n!=nline;n++) {
        textdecode_get_piece(n,0,txt);
        if (strcmp(txt,"usemtl")==0) ntexture++;
	}

		// can't import if no textures
		
	if (ntexture==0) {
		textdecode_close();
		strcpy(err_str,"OBJ contains no materials.  Animator can only import texture-mapped OBJs.");
		return(FALSE);
	}
	
		// can't import if too many materials?
		
	if ((ntexture+texture_count())>=max_model_texture) {
		textdecode_close();
		sprintf(err_str,"Too many materials, models can have a maximum of %d materials.",max_model_texture);
		return(FALSE);
	}
	
		// count the vertexes and trigs
		
	nvertex=0;
	ntrig=0;

    for (n=0;n!=nline;n++) {
        textdecode_get_piece(n,0,txt);
		
        if (strcmp(txt,"v")==0) {
			nvertex++;
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
	
	model_mesh_set_vertex_count(&model,cur_mesh,nvertex);
	model_mesh_set_trig_count(&model,cur_mesh,ntrig);
	
		// uv memory
		
	uv_ptr=(d3uv*)malloc(nvertex*sizeof(d3uv));
	normal_ptr=(d3vct*)malloc(nvertex*sizeof(d3vct));
	
		// get the vertex and uv

	vertex=model.meshes[cur_mesh].vertexes;
    
    nobj_uv=0;
    uv=uv_ptr;
	
	nobj_normal=0;
	normal=normal_ptr;
   
    for (n=0;n!=nline;n++) {

        textdecode_get_piece(n,0,txt);
      
            // a vertex
            
        if (strcmp(txt,"v")==0) {
			textdecode_get_piece(n,1,txt);
			vertex->pnt.x=-(int)(strtod(txt,NULL)*import_scale_factor);
			textdecode_get_piece(n,2,txt);
			vertex->pnt.y=-(int)(strtod(txt,NULL)*import_scale_factor);
			textdecode_get_piece(n,3,txt);
			vertex->pnt.z=-(int)(strtod(txt,NULL)*import_scale_factor);
            
            vertex->major_bone_idx=vertex->minor_bone_idx=-1;
            vertex->bone_factor=1;
        
            vertex++;
            nvertex++;
        }
        else {
        
            // a uv
            
            if (strcmp(txt,"vt")==0) {
                textdecode_get_piece(n,1,txt);
                uv->x=strtod(txt,NULL);
                textdecode_get_piece(n,2,txt);
                uv->x=strtod(txt,NULL);
                
                nobj_uv++;
				uv++;
            }
			else {
			
				// a normal
				
				if (strcmp(txt,"vn")==0) {
					textdecode_get_piece(n,1,txt);
					normal->x=-strtod(txt,NULL);
					textdecode_get_piece(n,2,txt);
					normal->y=-strtod(txt,NULL);
					textdecode_get_piece(n,3,txt);
					normal->z=-strtod(txt,NULL);
					
					normal++;
					nobj_normal++;
				}
			}
        }
    }
	
		// can't import if no UVs
		
	if (nobj_uv==0) {
		textdecode_close();
		free(uv_ptr);
		free(normal_ptr);
		sprintf(err_str,"There are no UVs in this OBJ, please texture map the model before importing.");
		return(FALSE);
	}
	
		// single material import?
		
	single_material=FALSE;
	
	if (ntexture>1) {
		single_material=texture_use_single();
	}

		// get the triangles
		
	*found_normals=(nobj_normal!=0);

	first_material=TRUE;
	material=NULL;
    
    ntrig=0;
	trig=model.meshes[cur_mesh].trigs;
	
	last_material_name[0]=0x0;
        
    for (n=0;n!=nline;n++) {

        textdecode_get_piece(n,0,txt);
        
            // material change
            
        if (strcmp(txt,"usemtl")==0) {
		
            textdecode_get_piece(n,1,material_name);
		
			if (!first_material) {
			
					// single material?
				
				if (single_material) continue;
			
					// same as last material?
				
				if (strcmp(material_name,last_material_name)==0) continue;
				
				strcpy(last_material_name,material_name);
			}
            
                // close last material
            
            if (!first_material) {
				material->trig_count=ntrig-material->trig_start;
            }
			
			first_material=FALSE;
            
                // start new material
             
            texture_idx=texture_pick(material_name,err_str);
			if (texture_idx==-1) {
				textdecode_close();
				return(FALSE);
			}
            
			material=&model.meshes[cur_mesh].materials[texture_idx];
            material->trig_start=ntrig;
            
            continue;
        }

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
			
            pvtx[npt]=atoi(vstr)-1;
            
			if (vtstr[0]==0x0) {
				pt_uv[npt].x=pt_uv[npt].y=0.0f;
			}
			else {
				t=atoi(vtstr)-1;
				pt_uv[npt].x=uv_ptr[t].x;
				pt_uv[npt].y=1.0f-uv_ptr[t].y;
            }
				
			if (vnstr[0]!=0x0) {
				t=atoi(vnstr)-1;
				memmove(&pnormal[npt],&normal_ptr[t],sizeof(d3vct));
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
    
    if (!first_material) {
        material->trig_count=ntrig-material->trig_start;
    }
	
	model.meshes[cur_mesh].ntrig=ntrig;
	
	free(uv_ptr);
	free(normal_ptr);
	
	textdecode_close();

	return(TRUE);
}

