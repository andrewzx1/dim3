/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: OBJ Import

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
application screen and in a single piece of the documentation
3. It is not resold, in it's current form or modified, as an
engine-only product

This code is presented as is. The author of dim3 takes no
responsibilities for any version of this code.

Any non-engine product (games, etc) created with this code is free
from any and all payment and/or royalties to the author of dim3,
and can be sold or given away.

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "interface.h"

extern map_type					map;
extern app_state_type			state;
extern app_pref_type			pref;

extern file_path_setup_type		file_path_setup;

#define import_max_material								64

typedef struct		{
						int								txt_idx;
						char							name[name_str_len];
					} obj_import_state_material_type;
					
typedef struct		{
						int								start_line_idx,end_line_idx,
														vertex_pos,uv_pos,normal_pos;
						char							name[name_str_len],cur_material_name[name_str_len];
					} obj_import_state_group_type;
					
typedef struct		{
						int								nline,nmaterial,
														ngroup,group_idx,
														nvertex,npoly,nuv,nnormal;
						char							obj_name[name_str_len];
						bool							texture_cancel;
						float							*uvs,*normals;
						d3fpnt							*vertexes;
						d3fpnt							obj_min,obj_max,mesh_min,mesh_max;
						obj_import_state_material_type	*materials;
						obj_import_state_group_type		*groups;
					} obj_import_state_type;

/* =======================================================

      OBJ Replacementment
      
======================================================= */

bool piece_add_obj_is_replace_ok(d3pnt *min,d3pnt *max)
{
	int				n,sel_count,
					type,main_idx,sub_idx;
	bool			hit;
	d3pnt			t_min,t_max;
	
	sel_count=select_count();
	if (sel_count==0) return(FALSE);
	
	hit=FALSE;
	
	min->x=min->z=min->y=map_max_size;
	max->x=max->z=max->y=-map_max_size;
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&main_idx,&sub_idx);
		if (type!=item_map_mesh) continue;
		
		hit=TRUE;

		map_mesh_calculate_extent(&map,main_idx,&t_min,&t_max);
			
		if (t_min.x<(min->x)) min->x=t_min.x;
		if (t_min.y<(min->y)) min->y=t_min.y;
		if (t_min.z<(min->z)) min->z=t_min.z;
		if (t_max.x>(max->x)) max->x=t_max.x;
		if (t_max.y>(max->y)) max->y=t_max.y;
		if (t_max.z>(max->z)) max->z=t_max.z;
	}
	
	return(hit);
}

void piece_add_obj_replace_delete_existing(void)
{
	int				n,type,mesh_idx,poly_idx,sel_cnt;
	
	sel_cnt=select_count();
	if (sel_cnt==0) return;

		// sort segment so higher indexes are deleted first
		
	select_sort();
		
	for (n=0;n!=sel_cnt;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type==item_map_mesh) {
			view_vbo_mesh_free(mesh_idx);
			map_mesh_delete(&map,mesh_idx);
		}
	}
	
	select_clear();
}

/* =======================================================

      Textures in Materials
      
======================================================= */

void import_texture_copy(char *path,char *name,char *sub_name)
{
    char			*c,dest_name[256],srce_path[1024],dest_path[1024];

	strcpy(srce_path,path);
	
	if (sub_name!=NULL) {
		c=strrchr(srce_path,'.');
		if (c==NULL) return;
		
		*c=0x0;
		strcat(srce_path,sub_name);
		strcat(srce_path,".png");
	}
	
	strcpy(dest_name,name);
	
	if (sub_name!=NULL) strcat(dest_name,sub_name);
	
	file_paths_data_default(&file_path_setup,dest_path,"Bitmaps/Textures",dest_name,"png");
	bitmap_copy(srce_path,dest_path);
}

void import_texture_fill_materials(obj_import_state_type *import_state)
{
	int				n,k,txt_idx,free_txt_idx;
	char			txt[256],material_name[256],file_name[256],
					title[256],path[1024],err_str[256];
	char			*c;
	texture_type	*texture;
	
	import_state->nmaterial=0;
	
    for (n=0;n!=import_state->nline;n++) {
	
			// is this a material
			
		textdecode_get_piece(n,0,txt);
		if (strcmp(txt,"usemtl")!=0) continue;
		
			// get name
			
		textdecode_get_piece(n,1,material_name);
		material_name[name_str_len-1]=0x0;
		
			// already a material?
			
		txt_idx=-1;
			
		for (k=0;k!=import_state->nmaterial;k++) {
			if (strcmp(import_state->materials[k].name,material_name)==0) {
				txt_idx=k;
				break;
			}
		}
		
		if (txt_idx!=-1) continue;
		
			// special blank or null textures
		
		if ((material_name[0]==0x0) || (strcmp(material_name,"null")==0) || (strcmp(material_name,"(null)")==0) || (strcmp(material_name,"default")==0)) {
			txt_idx=map_texture_palette_get_selected_texture();
			if (txt_idx==-1) txt_idx=0;
		}
		
			// does texture already exist

		free_txt_idx=-1;
			
		if (txt_idx==-1) {
		
			texture=map.textures;
			
			for (k=0;k!=max_map_texture;k++) {
			
					// see if we've already loaded this
					// texture, otherwise mark the first
					// free textures
					
				if (texture->frames[0].bitmap.gl_id!=-1) {
					if (strcasecmp(texture->material_name,material_name)==0) {
						txt_idx=k;
						break;
					}
				}
				else {
					if (free_txt_idx==-1) free_txt_idx=k;
				}

				texture++;
			}
		}
		
			// any more spots?
			
		if ((txt_idx==-1) || (free_txt_idx==-1)) {
			if (import_state->nmaterial>=import_max_material) break;
		}
		
			// if not, pick one
		
		if (txt_idx==-1) {

				// load texture and remember name
			
			texture=&map.textures[free_txt_idx];
			strcpy(texture->material_name,material_name);
			
				// pick a bitmap

			txt_idx=0;
				
			sprintf(title,"Select a PNG for Material: %s",material_name);
			
			if (os_load_file(title,path,"png")) {
			
					// is it good?

				if (bitmap_check(path,err_str)) {
			
						// get the actual file name
						
					c=strrchr(path,'/');
					if (c==NULL) c=strrchr(path,'\\');
					if (c==NULL) {
						strcpy(file_name,"unknown");
					}
					else {
						strcpy(file_name,(c+1));
						c=strrchr(file_name,'.');
						if (c!=NULL) *c=0x0;
					}
			
						// copy bitmaps to map folder
						// copy selected bitmap and any addition _n, _s, or _g files
						
					strcpy(texture->frames[0].name,file_name);
						
					import_texture_copy(path,texture->frames[0].name,NULL);
					import_texture_copy(path,texture->frames[0].name,"_n");
					import_texture_copy(path,texture->frames[0].name,"_s");
					import_texture_copy(path,texture->frames[0].name,"_g");
					
						// open the textures
						
					map_refresh_textures(&map);

					txt_idx=free_txt_idx;
				}
				else {
					os_dialog_alert("Bad PNG",err_str);
				}
			}
		}
		
			// setup material
			
		strcpy(import_state->materials[import_state->nmaterial].name,material_name);
		import_state->materials[import_state->nmaterial].txt_idx=txt_idx;
		
		import_state->nmaterial++;
	}
}

int import_texture_get_material_texture_idx(obj_import_state_type *import_state,char *material_name)
{
	int						n;
	
	for (n=0;n!=import_state->nmaterial;n++) {
		if (strcmp(import_state->materials[n].name,material_name)==0) return(import_state->materials[n].txt_idx);
	}
	
	return(0);
}

/* =======================================================

      Import Data
      
======================================================= */

void map_import_obj_load_data(obj_import_state_type *import_state)
{
	int						n,group_idx,
							vertex_pos,uv_pos,normal_pos;
	char					txt[256],material_name[256];
	float					*uv,*normal;
	d3fpnt					*dpt;
		
		// load data
		
	group_idx=0;
	
	dpt=import_state->vertexes;
	vertex_pos=0;
	
	uv=import_state->uvs;
	uv_pos=0;
	
	normal=import_state->normals;
	normal_pos=0;
	
	material_name[0]=0x0;
		
    for (n=0;n!=import_state->nline;n++) {
	
        textdecode_get_piece(n,0,txt);
		
			// remember current material because
			// it could be before group start
		
		if (strcmp(txt,"usemtl")==0) {
		
			textdecode_get_piece(n,1,material_name);
			material_name[name_str_len-1]=0x0;
			
			continue;
		}
		
			// counts
			
		if (strcmp(txt,"v")==0) {
			textdecode_get_piece(n,1,txt);
			dpt->x=(float)strtod(txt,NULL);
			textdecode_get_piece(n,2,txt);
			dpt->y=-(float)strtod(txt,NULL);
			textdecode_get_piece(n,3,txt);
			dpt->z=(float)strtod(txt,NULL);
			
			dpt++;
			vertex_pos++;
			continue;
		}
		
		if (strcmp(txt,"vt")==0) {
			textdecode_get_piece(n,1,txt);
			*uv++=(float)strtod(txt,NULL);
			textdecode_get_piece(n,2,txt);
			*uv++=1.0f-(float)strtod(txt,NULL);
			
			uv_pos++;
			continue;
		}
		
		if (strcmp(txt,"vn")==0) {
			textdecode_get_piece(n,1,txt);
			*normal++=((float)strtod(txt,NULL));
			textdecode_get_piece(n,2,txt);
			*normal++=((float)strtod(txt,NULL));
			textdecode_get_piece(n,3,txt);
			*normal++=((float)strtod(txt,NULL));

			normal_pos++;
			continue;
		}
		
			// is this a group?
			
        if (strcmp(txt,"g")!=0) continue;
		
			// setup the lines and vertex counts
		
		if (group_idx!=0) import_state->groups[group_idx-1].end_line_idx=n-1;
		import_state->groups[group_idx].start_line_idx=n+1;
		
		import_state->groups[group_idx].vertex_pos=vertex_pos;
		import_state->groups[group_idx].uv_pos=uv_pos;
		import_state->groups[group_idx].normal_pos=normal_pos;
		
		strcpy(import_state->groups[group_idx].cur_material_name,material_name);
		
			// setup the name
		
		textdecode_get_piece(n,1,txt);
		strncpy(import_state->groups[group_idx].name,txt,name_str_len);
		import_state->groups[group_idx].name[name_str_len-1]=0x0;
		
		group_idx++;
	}
	
	import_state->groups[group_idx-1].end_line_idx=import_state->nline;
}

/* =======================================================

      Face Offsets
      
======================================================= */

int import_convert_face_index(char *str,int pos)
{
	int				idx;
	
	idx=atoi(str);
	if (idx>0) return(idx-1);
	return(pos+idx);
}

/* =======================================================

      Get OBJ Size
      
======================================================= */

void import_calc_obj_vertex_size(obj_import_state_type *import_state,d3fpnt *min,d3fpnt *max,bool all_groups)
{
	int					n,k,v_idx,vertex_pos,group_idx;
	float				fx,fy,fz;
	bool				in_group;
	char				*c,txt[256],vstr[256];
	unsigned char		*vertex_mark;
	
		// some OBJ creators -- especially blender --
		// like to leave this unconnected and crazy vertexes
		// in the OBJs.  We need to scan for them.
		
	vertex_mark=(unsigned char*)malloc(import_state->nvertex);
	bzero(vertex_mark,import_state->nvertex);
	
	group_idx=0;
	in_group=FALSE;
	
	vertex_pos=0;
		
    for (n=0;n!=import_state->nline;n++) {
	
        textdecode_get_piece(n,0,txt);
		
			// need to count vertexes for offset type OBJs
			
		if (strcmp(txt,"v")==0) {
			vertex_pos++;
			continue;
		}
		
			// if we are only looking at a group
			
		if (!all_groups) {
		
			if (strcmp(txt,"g")==0) {
				if (in_group) break;
				
				if (group_idx==import_state->group_idx) in_group=TRUE;
				
				group_idx++;
			}
			
			if (!in_group) continue;
		}
        
			// got a face
			
        if (strcmp(txt,"f")==0) {
		
			for (k=0;k!=8;k++) {
				textdecode_get_piece(n,(k+1),vstr);
				if (vstr[0]==0x0) break;
            
				c=strchr(vstr,'/');
				if (c!=NULL) *c=0x0;

				v_idx=import_convert_face_index(vstr,vertex_pos);
				vertex_mark[v_idx]=0x1;
			}
		}
	}
	
		// let's find total size -- we ignore
		// any non-connected stray vertexes
		
	min->x=min->y=min->z=0.0f;
	max->x=max->y=max->z=0.0f;
	
	v_idx=0;
	
	for (n=0;n!=import_state->nline;n++) {
		textdecode_get_piece(n,0,txt);
		
		if (strcmp(txt,"v")==0) {
			
			if (vertex_mark[v_idx]==0x0) {
				v_idx++;
				continue;
			}
			
			textdecode_get_piece(n,1,txt);
			fx=(float)strtod(txt,NULL);
			textdecode_get_piece(n,2,txt);
			fy=-(float)strtod(txt,NULL);
			textdecode_get_piece(n,3,txt);
			fz=(float)strtod(txt,NULL);
			
			if (v_idx==0) {
				min->x=max->x=fx;
				min->y=max->y=fy;
				min->z=max->z=fz;
			}
			else {
				if (fx<min->x) min->x=fx;
				if (fx>max->x) max->x=fx;
				if (fy<min->y) min->y=fy;
				if (fy>max->y) max->y=fy;
				if (fz<min->z) min->z=fz;
				if (fz>max->z) max->z=fz;
			}
			
			v_idx++;
		}
	}
	
	free(vertex_mark);
}

void import_calc_obj_group_size(obj_import_state_type *import_state,d3fpnt *min,d3fpnt *max)
{
	import_calc_obj_vertex_size(import_state,min,max,FALSE);
}

void import_calc_obj_size(obj_import_state_type *import_state,d3fpnt *min,d3fpnt *max)
{
	import_calc_obj_vertex_size(import_state,min,max,TRUE);
}

void import_get_scale_from_axis_unit(obj_import_state_type *import_state,int scale_axis,int scale_unit)
{
	d3pnt				pnt;
	d3fpnt				scale;
	
	switch (scale_axis) {
		case 0:
			scale.x=(float)scale_unit;
			scale.y=(float)((fabsf(import_state->obj_max.y-import_state->obj_min.y)*((float)scale_unit))/fabsf(import_state->obj_max.x-import_state->obj_min.x));
			scale.z=(float)((fabsf(import_state->obj_max.z-import_state->obj_min.z)*((float)scale_unit))/fabsf(import_state->obj_max.z-import_state->obj_min.z));
			break;
		case 1:
			scale.x=(float)((fabsf(import_state->obj_max.x-import_state->obj_min.x)*((float)scale_unit))/fabsf(import_state->obj_max.y-import_state->obj_min.y));
			scale.y=(float)scale_unit;
			scale.z=(float)((fabsf(import_state->obj_max.z-import_state->obj_min.z)*((float)scale_unit))/fabsf(import_state->obj_max.y-import_state->obj_min.y));
			break;
		case 2:
			scale.x=(float)((fabsf(import_state->obj_max.x-import_state->obj_min.x)*((float)scale_unit))/fabsf(import_state->obj_max.z-import_state->obj_min.z));
			scale.y=(float)((fabsf(import_state->obj_max.y-import_state->obj_min.y)*((float)scale_unit))/fabsf(import_state->obj_max.z-import_state->obj_min.z));
			scale.z=(float)scale_unit;
			break;
	}
	
	piece_create_get_spot(&pnt);
	
	import_state->mesh_min.x=((float)pnt.x)-(scale.x*0.5f);
	import_state->mesh_max.x=((float)pnt.x)+(scale.x*0.5f);
	import_state->mesh_min.y=((float)pnt.y)-(scale.y*0.5f);
	import_state->mesh_max.y=((float)pnt.y)+(scale.y*0.5f);
	import_state->mesh_min.z=((float)pnt.z)-(scale.z*0.5f);
	import_state->mesh_max.z=((float)pnt.z)+(scale.z*0.5f);
}

/* =======================================================

      Create Mesh from OBJ Group
      
======================================================= */

bool import_create_mesh_from_obj_group(obj_import_state_type *import_state,char *err_str)
{
	int							n,k,npt,
								v_idx,uv_idx,normal_idx,normal_count,
								mesh_idx,poly_idx,txt_idx;
	int							px[8],py[8],pz[8];
	char						*c,txt[256],material_name[256],
								vstr[256],uvstr[256],normalstr[256];
	float						gx[8],gy[8];
	float						*uv,*normal;
	bool						face_error;
	d3fpnt						*dpt;
	d3fpnt						factor;
	d3vct						n_v;
	map_mesh_type				*mesh;
	obj_import_state_group_type	*group;
	
	group=&import_state->groups[import_state->group_idx];

		// get the vertexes
		
	sprintf(txt,"OBJ Import: %s\n",group->name);
	progress_next_title(txt);

		// get the multiply factor
		
	factor.x=fabsf(import_state->mesh_max.x-import_state->mesh_min.x)/fabsf(import_state->obj_max.x-import_state->obj_min.x);
	factor.y=fabsf(import_state->mesh_max.y-import_state->mesh_min.y)/fabsf(import_state->obj_max.y-import_state->obj_min.y);
	factor.z=fabsf(import_state->mesh_max.z-import_state->mesh_min.z)/fabsf(import_state->obj_max.z-import_state->obj_min.z);

		// add the mesh
		
	mesh_idx=map_mesh_add(&map);
		
	if (mesh_idx==-1) {
		progress_end();
		strcpy(err_str,"Not enough memory to create mesh.");
		return(FALSE);
	}

	mesh=&map.mesh.meshes[mesh_idx];
		
		// set the import settings
			
	strcpy(mesh->import.obj_name,import_state->obj_name);
	strcpy(mesh->import.group_name,group->name);
		
		// set UV lock if uvs exist in obj
		
	mesh->flag.lock_uv=(import_state->nuv!=0);
	
		// get the current material before
		// this group starts
		
	if (group->cur_material_name[0]==0x0) {
		txt_idx=0;
	}
	else {
		txt_idx=import_texture_get_material_texture_idx(import_state,group->cur_material_name);
	}
	
		// get the polys

	face_error=FALSE;
		
    for (n=group->start_line_idx;n<=group->end_line_idx;n++) {
	
		textdecode_get_piece(n,0,txt);
		
			// a material
			
		if (strcmp(txt,"usemtl")==0) {
		
			textdecode_get_piece(n,1,material_name);
			material_name[name_str_len-1]=0x0;

			txt_idx=import_texture_get_material_texture_idx(import_state,material_name);
			
			continue;
		}
	
			// a face?
			
		if (strcmp(txt,"f")!=0) continue;
		
            // get the face points
        
        npt=0;

		n_v.x=n_v.y=n_v.z=0.0f;
		normal_count=0;
        
        for (k=0;k!=8;k++) {
            textdecode_get_piece(n,(k+1),txt);
            if (txt[0]==0x0) break;
            
			uvstr[0]=0x0;
			normalstr[0]=0x0;
            
            strcpy(vstr,txt);
            c=strchr(vstr,'/');
            if (c!=NULL) {
                strcpy(uvstr,(c+1));
                *c=0x0;
            }
            c=strchr(uvstr,'/');
            if (c!=NULL) {
				strcpy(normalstr,(c+1));
				*c=0x0;
            }
            
				// the vertex

            v_idx=import_convert_face_index(vstr,group->vertex_pos);

			dpt=import_state->vertexes+v_idx;
			
			px[npt]=(int)(import_state->mesh_min.x+((dpt->x-import_state->obj_min.x)*factor.x));
			py[npt]=(int)(import_state->mesh_min.y+((dpt->y-import_state->obj_min.y)*factor.y));
			pz[npt]=(int)(import_state->mesh_min.z+((dpt->z-import_state->obj_min.z)*factor.z));
            
				// the UV

			if ((uvstr[0]==0x0) || (import_state->nuv==0)) {
				gx[npt]=gy[npt]=0.0f;
			}
			else {
				uv_idx=import_convert_face_index(uvstr,group->uv_pos);
				
				uv=import_state->uvs+(uv_idx*2);
                gx[npt]=*uv++;
                gy[npt]=*uv;
            }

				// the normal

			if ((normalstr[0]!=0x0) && (import_state->nnormal!=0)) {
				normal_idx=import_convert_face_index(normalstr,group->normal_pos);
				
				normal=import_state->normals+(normal_idx*3);
				n_v.x+=*normal++;
				n_v.y+=*normal++;
				n_v.z+=*normal;

				normal_count++;
			}
			
            npt++;
        }
		
			// is there at least 3 points?
			
		if (npt<3) {
			face_error=TRUE;
			continue;
		}

			// create the poly

		poly_idx=map_mesh_add_poly(&map,mesh_idx,npt,px,py,pz,gx,gy,txt_idx);

			// set the normal

		n_v.x/=(float)normal_count;
		n_v.y/=(float)normal_count;
		n_v.z/=(float)normal_count;
	
		vector_normalize(&n_v);

		memmove(&mesh->polys[poly_idx].tangent_space.normal,&n_v,sizeof(d3vct));
	}
	
		// any polys?
		
	if (mesh->npoly==0) {
		map_mesh_delete(&map,mesh_idx);
		return(TRUE);
	}

		// face errors

	if (face_error) os_dialog_alert("OBJ Warning","There were polygons with less than 3 vertexes in this OBJ.  Those polygons were ignored.");

		// calc the normals
		// or only tangent if normals come with OBJ
		
	map_recalc_normals_mesh(&map,&map.mesh.meshes[mesh_idx],normal_mode_none,(import_state->nnormal!=0));
	
		// if no uvs, force auto-texture
		
	if (!mesh->flag.lock_uv) map_mesh_reset_uv(&map,mesh_idx);
	
		// vbo
		
	view_vbo_mesh_initialize(mesh_idx);
	
	return(TRUE);
}

/* =======================================================

      Import OBJ Memory
      
======================================================= */

bool map_import_obj_state_initialize(obj_import_state_type *import_state)
{
		// import data

	import_state->vertexes=(d3fpnt*)malloc(sizeof(d3fpnt)*import_state->nvertex);
	if (import_state->vertexes==NULL) return(FALSE);
	
	import_state->uvs=(float*)malloc(sizeof(float)*(import_state->nuv*2));
	if (import_state->uvs==NULL) {
		free(import_state->vertexes);
		return(FALSE);
	}
	
	import_state->normals=(float*)malloc(sizeof(float)*(import_state->nnormal*3));
	if (import_state->normals==NULL) {
		free(import_state->vertexes);
		free(import_state->uvs);
		return(FALSE);
	}
	
	import_state->groups=(obj_import_state_group_type*)malloc(sizeof(obj_import_state_group_type)*import_state->ngroup);
	if (import_state->groups==NULL) {
		free(import_state->vertexes);
		free(import_state->uvs);
		free(import_state->normals);
		return(FALSE);
    }
	
	import_state->materials=(obj_import_state_material_type*)malloc(sizeof(obj_import_state_material_type)*import_max_material);
	if (import_state->materials==NULL) {
		free(import_state->vertexes);
		free(import_state->uvs);
		free(import_state->normals);
		free(import_state->groups);
		return(FALSE);
	}
	
	return(TRUE);
}

void map_import_obj_state_shutdown(obj_import_state_type *import_state)
{
		// free the data

	free(import_state->vertexes);
	free(import_state->uvs);
	free(import_state->normals);
	free(import_state->groups);
	free(import_state->materials);
}

/* =======================================================

      Import OBJ
      
======================================================= */

bool map_import_obj(char *path,char *err_str)
{
	int						n,k,nmesh,old_nmesh,
							import_mode,scale_axis,scale_unit;
	char					*c,txt[256],file_name[256];
	unsigned char			*mesh_mark;
	bool					ok,replace_ok,force_grid;
	d3pnt					replace_min,replace_max;
	obj_import_state_type	import_state;

		// import the file

	if (!textdecode_open(path,0x20)) {
		strcpy(err_str,"Could not open OBJ file.");
		return(FALSE);
    }
	
	os_set_wait_cursor();
	
		// get file name
		
	c=strrchr(path,'/');
	if (c==NULL) c=strrchr(path,'\\');
	if (c==NULL) {
		strcpy(file_name,"unknown");
	}
	else {
		strcpy(file_name,(c+1));
		c=strrchr(file_name,'.');
		if (c!=NULL) *c=0x0;
	}
	
	strncpy(import_state.obj_name,file_name,name_str_len);
	import_state.obj_name[name_str_len-1]=0x0;
	
		// count groups, vertexes, faces, uvs, and normals
		
    import_state.nline=textdecode_count();
		
	import_state.ngroup=0;
	import_state.nvertex=0;
	import_state.npoly=0;
	import_state.nuv=0;
	import_state.nnormal=0;
	import_state.texture_cancel=FALSE;
	
    for (n=0;n!=import_state.nline;n++) {
        textdecode_get_piece(n,0,txt);
		
        if (strcmp(txt,"g")==0) {
			import_state.ngroup++;
			continue;
		}
        
        if (strcmp(txt,"v")==0) {
			import_state.nvertex++;
			continue;
		}

		if (strcmp(txt,"f")==0) {
			import_state.npoly++;
			continue;
		}

		if (strcmp(txt,"vt")==0) {
			import_state.nuv++;
			continue;
		}

		if (strcmp(txt,"vn")==0) {
			import_state.nnormal++;
			continue;
		}
	}
	
		// improper OBJ errors
		
	if (import_state.ngroup==0) {
		textdecode_close();
		strcpy(err_str,"No groups in OBJ.");
		return(FALSE);
    }
	if (import_state.nvertex==0) {
		textdecode_close();
		strcpy(err_str,"No vertexes in OBJ.");
		return(FALSE);
    }
	if (import_state.npoly==0) {
		textdecode_close();
		strcpy(err_str,"No faces in OBJ.");
		return(FALSE);
    }
	
		// create memory
		
	if (!map_import_obj_state_initialize(&import_state)) {
		textdecode_close();
		strcpy(err_str,"Out of Memory.");
		return(FALSE);
    }
	
		// load the OBJ data
		
	map_import_obj_load_data(&import_state);		
	
		// load up the materials
		
	import_texture_fill_materials(&import_state);
	
		// calculate the min and max of the
		// OBJ we are importing
		
	os_set_arrow_cursor();
	
		// run the import dialog

	replace_ok=piece_add_obj_is_replace_ok(&replace_min,&replace_max);
	if (!dialog_obj_import_run(&import_mode,&scale_axis,&scale_unit,&force_grid)) {
		textdecode_close();
		map_import_obj_state_shutdown(&import_state);
		return(TRUE);
	}
	
	if ((import_mode==import_mode_replace) && (!replace_ok)) import_mode=-1;
	
		// always switch to mesh select mode 
		
	state.map.select_mode=select_mode_mesh;
	
		// import new
		
	if (import_mode==import_mode_new) {
		
		os_set_wait_cursor();

		import_calc_obj_size(&import_state,&import_state.obj_min,&import_state.obj_max);
		import_get_scale_from_axis_unit(&import_state,scale_axis,scale_unit);
		
		select_clear();
		old_nmesh=map.mesh.nmesh;
		
		progress_start("OBJ Import",import_state.ngroup);
		
		for (n=0;n!=import_state.ngroup;n++) {
			import_state.group_idx=n;
			ok=import_create_mesh_from_obj_group(&import_state,err_str);
			if (!ok) break;
		}
		
		textdecode_close();
		
		for (n=old_nmesh;n<map.mesh.nmesh;n++) {
			if (force_grid) view_force_grid(n,FALSE);
			select_add(item_map_mesh,n,0);
		}
		
		map_import_obj_state_shutdown(&import_state);
		
		progress_end();
		os_set_arrow_cursor();

		return(ok);
	}
	
		// import replace

	if (import_mode==import_mode_replace) {

		os_set_wait_cursor();
		
		piece_add_obj_replace_delete_existing();
		
		select_clear();
		old_nmesh=map.mesh.nmesh;
		
		progress_start("OBJ Import",import_state.ngroup);
	
		import_calc_obj_size(&import_state,&import_state.obj_min,&import_state.obj_max);
		import_state.mesh_min.x=(float)replace_min.x;
		import_state.mesh_min.y=(float)replace_min.y;
		import_state.mesh_min.z=(float)replace_min.z;
		import_state.mesh_max.x=(float)replace_max.x;
		import_state.mesh_max.y=(float)replace_max.y;
		import_state.mesh_max.z=(float)replace_max.z;

		for (n=0;n!=import_state.ngroup;n++) {
			import_state.group_idx=n;
			ok=import_create_mesh_from_obj_group(&import_state,err_str);
			if (!ok) break;
		}
		
		textdecode_close();
		
		for (n=old_nmesh;n<map.mesh.nmesh;n++) {
			select_add(item_map_mesh,n,0);
		}
		
		map_import_obj_state_shutdown(&import_state);
		
		progress_end();
		os_set_arrow_cursor();

		return(ok);
	}
	
		// import replace all
		
	os_set_wait_cursor();

	nmesh=map.mesh.nmesh;			// operation will add meshes, so we need to not go into them
		
	mesh_mark=(unsigned char*)malloc(nmesh);
	if (mesh_mark==NULL) {
		textdecode_close();
		os_set_arrow_cursor();
		strcpy(err_str,"Out of Memory");
		return(FALSE);
	}
	
	bzero(mesh_mark,nmesh);
	
	select_clear();
	
	progress_start("OBJ Import",import_state.ngroup);

	for (n=0;n!=import_state.ngroup;n++) {
	
		import_state.group_idx=n;
		
		import_calc_obj_group_size(&import_state,&import_state.obj_min,&import_state.obj_max);
		
		ok=TRUE;
	
		for (k=0;k!=nmesh;k++) {
		
				// same imported OBJ?
				
			if (strcasecmp(map.mesh.meshes[k].import.obj_name,import_state.obj_name)!=0) continue;
			if (strcasecmp(map.mesh.meshes[k].import.group_name,import_state.groups[import_state.group_idx].name)!=0) continue;
			
				// get scale and mark for delete
				
			map_mesh_calculate_extent(&map,k,&replace_min,&replace_max);
			import_state.mesh_min.x=(float)replace_min.x;
			import_state.mesh_min.y=(float)replace_min.y;
			import_state.mesh_min.z=(float)replace_min.z;
			import_state.mesh_max.x=(float)replace_max.x;
			import_state.mesh_max.y=(float)replace_max.y;
			import_state.mesh_max.z=(float)replace_max.z;

			mesh_mark[k]=0x1;
		
				// import
				
			ok=import_create_mesh_from_obj_group(&import_state,err_str);
			
			if (!ok) break;
		}
	}

	textdecode_close();
		
		// delete marked meshes

	for (n=(nmesh-1);n>=0;n--) {
		if (mesh_mark[n]==0x1) {
			view_vbo_mesh_free(n);
			map_mesh_delete(&map,n);
		}
	}

	free(mesh_mark);
	
	select_clear();
	
	map_import_obj_state_shutdown(&import_state);

	progress_end();
	os_set_arrow_cursor();
	
	return(ok);
}

