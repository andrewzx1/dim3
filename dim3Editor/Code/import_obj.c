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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "interface.h"
#include "ui_common.h"

extern map_type					map;
extern editor_state_type		state;

extern file_path_setup_type		file_path_setup;

#define import_max_material								32

typedef struct		{
						int								txt_idx;
						char							name[name_str_len];
					} obj_import_state_material_type;
					
typedef struct		{
						int								nline,nmaterial,
														ngroup,group_idx,
														nvertex,npoly,nuv,nnormal;
						char							obj_name[name_str_len],
														group_name[name_str_len];
						d3fpnt							obj_min,obj_max,mesh_min,mesh_max;
						obj_import_state_material_type	materials[32];
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
		if (type!=mesh_piece) continue;
		
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
		if (type==mesh_piece) map_mesh_delete(&map,mesh_idx);
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

int import_texture_pick(char *material_name)
{
	int				n,idx;
    char			*c,path[1024],title[256],
					file_name[256],err_str[256];
	texture_type	*texture;
	
		// check for null or missing materials
		// use selected texture
		
	idx=texture_palette_get_selected_texture();
	if (idx==-1) idx=0;
	
	if (material_name[0]==0x0) return(idx);
	if (strstr(material_name,"null")==material_name) return(idx);
	if (strstr(material_name,"(null)")==material_name) return(idx);
	if (strstr(material_name,"default")==material_name) return(idx);
	
		// find a free texture or texture
		// that already has material name
		
	idx=-1;
	
	texture=map.textures;
	
    for (n=0;n!=max_map_texture;n++) {
	
			// a free texture
			
		if ((texture->frames[0].bitmap.gl_id==-1) && (idx==-1)) {
			idx=n;
		}
		
			// a used texture, check to see if
			// material already exists
			
		else {
			if (strcasecmp(texture->material_name,material_name)==0) return(n);
		}
		
		texture++;
	}
	
	if (idx==-1) return(0);
	
	texture=&map.textures[idx];
	
		// remember material name
		
	strcpy(texture->material_name,material_name);
	
		// pick a bitmap
		
	sprintf(title,"Select a PNG for Material: %s",material_name);
	
	if (!os_load_file(title,path,"png")) return(0);
	
		// is it good?

	if (!bitmap_check(path,err_str)) {
		os_dialog_alert("Error",err_str);
		return(0);
	}
	
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
	
		// copy bitmaps to texture folder
		// copy selected bitmap and any addition _n, _s, or _g files
		
	strcpy(texture->frames[0].name,file_name);
		
	import_texture_copy(path,texture->frames[0].name,NULL);
	import_texture_copy(path,texture->frames[0].name,"_n");
	import_texture_copy(path,texture->frames[0].name,"_s");
	import_texture_copy(path,texture->frames[0].name,"_g");
	
		// open the textures
		
	map_refresh_textures(&map);

	return(idx);
}

void import_texture_fill_materials(obj_import_state_type *import_state)
{
	int						n,txt_idx;
	char					txt[256],material_name[256];
	
	import_state->nmaterial=0;
	
    for (n=0;n!=import_state->nline;n++) {
	
			// is this a material
			
		textdecode_get_piece(n,0,txt);
		if (strcmp(txt,"usemtl")!=0) continue;
		
			// get name
			
		textdecode_get_piece(n,1,material_name);
		material_name[name_str_len-1]=0x0;
		
			// get texture
			
		txt_idx=import_texture_pick(material_name);
		if (txt_idx==-1) return;
		
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

      Groups
      
======================================================= */

void import_find_group_name(obj_import_state_type *import_state)
{
	int						n,group_idx;
	char					txt[256];
	
	group_idx=0;
	import_state->group_name[0]=0x0;
	
    for (n=0;n!=import_state->nline;n++) {
	
		textdecode_get_piece(n,0,txt);
		if (strcmp(txt,"g")!=0) continue;
		
		if (import_state->group_idx==group_idx) {
			textdecode_get_piece(n,1,txt);
			
			strncpy(import_state->group_name,txt,name_str_len);
			import_state->group_name[name_str_len-1]=0x0;
			
			return;
		}
		
		group_idx++;
	}
}		

/* =======================================================

      Get OBJ Size
      
======================================================= */

void import_calc_obj_vertex_size(obj_import_state_type *import_state,d3fpnt *min,d3fpnt *max,bool all_groups)
{
	int					n,k,v_idx,group_idx;
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
		
    for (n=0;n!=import_state->nline;n++) {
	
        textdecode_get_piece(n,0,txt);
		
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

				v_idx=atoi(vstr)-1;
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
			scale.y=(float)((fabs(import_state->obj_max.y-import_state->obj_min.y)*((double)scale_unit))/fabs(import_state->obj_max.x-import_state->obj_min.x));
			scale.z=(float)((fabs(import_state->obj_max.z-import_state->obj_min.z)*((double)scale_unit))/fabs(import_state->obj_max.z-import_state->obj_min.z));
			break;
		case 1:
			scale.x=(float)((fabs(import_state->obj_max.x-import_state->obj_min.x)*((double)scale_unit))/fabs(import_state->obj_max.y-import_state->obj_min.y));
			scale.y=(float)scale_unit;
			scale.z=(float)((fabs(import_state->obj_max.z-import_state->obj_min.z)*((double)scale_unit))/fabs(import_state->obj_max.y-import_state->obj_min.y));
			break;
		case 2:
			scale.x=(float)((fabs(import_state->obj_max.x-import_state->obj_min.x)*((double)scale_unit))/fabs(import_state->obj_max.z-import_state->obj_min.z));
			scale.y=(float)((fabs(import_state->obj_max.y-import_state->obj_min.y)*((double)scale_unit))/fabs(import_state->obj_max.z-import_state->obj_min.z));
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
	int					n,k,npt,group_idx,start_line_idx,end_line_idx,
						v_idx,uv_idx,normal_idx,normal_count,
						mesh_idx,poly_idx,txt_idx,old_nmesh;
	int					px[8],py[8],pz[8];
	char				*c,txt[256],material_name[256],
						vstr[256],uvstr[256],normalstr[256];
	float				fx,fy,fz,gx[8],gy[8];
	float				*uvs,*uv,*normals,*normal;
	d3pnt				*vertexes,*dpt;
	d3fpnt				factor;
	d3vct				n_v;
	map_mesh_type		*mesh;

		// start progress
		
	progress_start("OBJ Import",7);
	
		// get the vertexes
		
	progress_next_title("OBJ Import: Loading Vertexes");

	vertexes=(d3pnt*)malloc(sizeof(d3pnt)*import_state->nvertex);
	if (vertexes==NULL) {
		progress_end();
		strcpy(err_str,"Out of Memory.");
		return(FALSE);
    }
	
		// get the multiply factor
		
	factor.x=(float)(fabs(import_state->mesh_max.x-import_state->mesh_min.x)/fabs(import_state->obj_max.x-import_state->obj_min.x));
	factor.y=(float)(fabs(import_state->mesh_max.y-import_state->mesh_min.y)/fabs(import_state->obj_max.y-import_state->obj_min.y));
	factor.z=(float)(fabs(import_state->mesh_max.z-import_state->mesh_min.z)/fabs(import_state->obj_max.z-import_state->obj_min.z));

		// get the vertexes
	
	dpt=vertexes;

	for (n=0;n!=import_state->nline;n++) {

		textdecode_get_piece(n,0,txt);
		if (strcmp(txt,"v")!=0) continue;
				
		textdecode_get_piece(n,1,txt);
		fx=(float)strtod(txt,NULL);
		textdecode_get_piece(n,2,txt);
		fy=-(float)strtod(txt,NULL);
		textdecode_get_piece(n,3,txt);
		fz=(float)strtod(txt,NULL);
		
		dpt->x=(int)(import_state->mesh_min.x+((fx-import_state->obj_min.x)*factor.x));
		dpt->y=(int)(import_state->mesh_min.y+((fy-import_state->obj_min.y)*factor.y));
		dpt->z=(int)(import_state->mesh_min.z+((fz-import_state->obj_min.z)*factor.z));
		
		dpt++;
	}
	
		// get the UVs
		
	progress_next_title("OBJ Import: Loading UVs");
		
	if (import_state->nuv!=0) {
		uvs=(float*)malloc(sizeof(float)*(2*import_state->nuv));
		if (uvs==NULL) {
			progress_end();
			strcpy(err_str,"Out of Memory.");
			return(FALSE);
		}

		uv=uvs;

		for (n=0;n!=import_state->nline;n++) {
			textdecode_get_piece(n,0,txt);
			if (strcmp(txt,"vt")!=0) continue;
					
			textdecode_get_piece(n,1,uvstr);
			*uv++=(float)strtod(uvstr,NULL);
			textdecode_get_piece(n,2,uvstr);
			*uv++=1.0f-(float)strtod(uvstr,NULL);
		}
	}

		// get the normals
		
	progress_next_title("OBJ Import: Loading Normals");
		
	if (import_state->nnormal!=0) {
		normals=(float*)malloc(sizeof(float)*(3*import_state->nnormal));
		if (normals==NULL) {
			progress_end();
			strcpy(err_str,"Out of Memory.");
			return(FALSE);
		}

		normal=normals;

		for (n=0;n!=import_state->nline;n++) {
			textdecode_get_piece(n,0,txt);
			if (strcmp(txt,"vn")!=0) continue;
					
			textdecode_get_piece(n,1,normalstr);
			*normal++=(float)strtod(normalstr,NULL);
			textdecode_get_piece(n,2,normalstr);
			*normal++=(float)strtod(normalstr,NULL);
			textdecode_get_piece(n,3,normalstr);
			*normal++=(float)strtod(normalstr,NULL);
		}
	}

		// find the correct group to import
	
	sprintf(txt,"OBJ Import: Finding Group %d",import_state->group_idx);
	progress_next_title(txt);

	txt_idx=0;
	group_idx=0;
	
	start_line_idx=end_line_idx=-1;
		
    for (n=0;n!=import_state->nline;n++) {
	
       textdecode_get_piece(n,0,txt);
	
			// new material, change texture
			// so we have the right material for
			// this group
			
		if (strcmp(txt,"usemtl")==0) {
		
			textdecode_get_piece(n,1,material_name);
			material_name[name_str_len-1]=0x0;

			txt_idx=import_texture_get_material_texture_idx(import_state,material_name);
			
			continue;
		}
		
			// mark the groups boundry
		
		if (strcmp(txt,"g")==0) {
		
				// looking for end
				
			if (start_line_idx!=-1) {
				end_line_idx=n;
				break;
			}
				
				// looking for beginning
				
			if (group_idx==import_state->group_idx) {
				start_line_idx=n+1;
			}
			
			group_idx++;
		}
	}
	
		// do we have anything to use?
		
	if (start_line_idx==-1) return(TRUE);
	if (end_line_idx==-1) end_line_idx=import_state->nline;
	
		// remember the number of meshes
		// we add so we can fix some things later
		
	old_nmesh=map.mesh.nmesh;
	
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
	strcpy(mesh->import.group_name,import_state->group_name);
		
		// set UV and Normal locks if uvs and normals
		// exist in obj
		
	mesh->flag.lock_uv=(import_state->nuv!=0);
	if (import_state->nnormal!=0) mesh->normal_mode=mesh_normal_mode_lock;
	
		// get the polys
		
	progress_next_title("OBJ Import: Building Polygons");
	
    for (n=start_line_idx;n<end_line_idx;n++) {
	
			// a face?
			
		textdecode_get_piece(n,0,txt);
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

            v_idx=atoi(vstr)-1;
			dpt=vertexes+v_idx;
			
			px[npt]=dpt->x;
			py[npt]=dpt->y;
			pz[npt]=dpt->z;
            
				// the UV

			if ((uvstr[0]==0x0) || (import_state->nuv==0)) {
				gx[npt]=gy[npt]=0.0f;
			}
			else {
				uv_idx=atoi(uvstr)-1;
				
				uv=uvs+(uv_idx*2);
                gx[npt]=*uv++;
                gy[npt]=*uv;
            }

				// the normal

			if ((normalstr[0]!=0x0) && (import_state->nnormal!=0)) {
				normal_idx=atoi(normalstr)-1;

				normal=normals+(normal_idx*3);
				n_v.x+=*normal++;
				n_v.y+=*normal++;
				n_v.z+=*normal;

				normal_count++;
			}
			
            npt++;
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
	
	free(vertexes);
	if (import_state->nuv!=0) free(uvs);
	if (import_state->nnormal!=0) free(normals);
	
		// if no uvs, force auto-texture
		
	progress_next();
	if (!mesh->flag.lock_uv) map_mesh_reset_uv(&map,mesh_idx);

		// calc the normals
		// or only bi/tangent if normals come with OBJ
		
	progress_next();
	map_recalc_normals_mesh(&map.mesh.meshes[mesh_idx],(mesh->normal_mode==mesh_normal_mode_lock));
	
		// finish up
		
	progress_end();
	
	select_add(mesh_piece,mesh_idx,0);
	
	return(TRUE);
}

/* =======================================================

      Import OBJ
      
======================================================= */

bool import_obj(char *path,char *err_str)
{
	int						n,k,nmesh,
							import_mode,scale_axis,scale_unit;
	char					*c,txt[256],file_name[256];
	unsigned char			*mesh_mark;
	bool					ok,replace_ok;
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
	
		// imporper OBJ errors
		
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
	
		// load up the materials
		
	import_texture_fill_materials(&import_state);
	
		// calculate the min and max of the
		// OBJ we are importing
		
	os_set_arrow_cursor();
	
		// run the import dialog

	replace_ok=piece_add_obj_is_replace_ok(&replace_min,&replace_max);
	import_mode=dialog_obj_import_run(&scale_axis,&scale_unit);
	
	if ((import_mode==import_mode_replace) && (!replace_ok)) import_mode=-1;
	
	if (import_mode==-1) {
		textdecode_close();
		return(TRUE);
    }
	
		// always switch to drag mode 
		
	state.drag_mode=drag_mode_mesh;
	
		// import new
		
	if (import_mode==import_mode_new) {
		
		os_set_wait_cursor();

		import_calc_obj_size(&import_state,&import_state.obj_min,&import_state.obj_max);
		import_get_scale_from_axis_unit(&import_state,scale_axis,scale_unit);
		
		select_clear();
		
		for (n=0;n!=import_state.ngroup;n++) {
		
			import_state.group_idx=n;
			import_find_group_name(&import_state);
			
			ok=import_create_mesh_from_obj_group(&import_state,err_str);
			if (!ok) break;
		}
		
		textdecode_close();
		
		os_set_arrow_cursor();

		return(ok);
	}
	
		// import replace

	if (import_mode==import_mode_replace) {

		os_set_wait_cursor();
		
		piece_add_obj_replace_delete_existing();
		
		select_clear();
	
		import_calc_obj_size(&import_state,&import_state.obj_min,&import_state.obj_max);
		import_state.mesh_min.x=(float)replace_min.x;
		import_state.mesh_min.y=(float)replace_min.y;
		import_state.mesh_min.z=(float)replace_min.z;
		import_state.mesh_max.x=(float)replace_max.x;
		import_state.mesh_max.y=(float)replace_max.y;
		import_state.mesh_max.z=(float)replace_max.z;

		for (n=0;n!=import_state.ngroup;n++) {
		
			import_state.group_idx=n;
			import_find_group_name(&import_state);
			
			ok=import_create_mesh_from_obj_group(&import_state,err_str);
			if (!ok) break;
		}
		
		textdecode_close();
		
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

	for (n=0;n!=import_state.ngroup;n++) {
	
		import_state.group_idx=n;
		import_find_group_name(&import_state);
		
		import_calc_obj_group_size(&import_state,&import_state.obj_min,&import_state.obj_max);
		
		ok=TRUE;
	
		for (k=0;k!=nmesh;k++) {
		
				// same imported OBJ?
				
			if (strcasecmp(map.mesh.meshes[k].import.obj_name,import_state.obj_name)!=0) continue;
			if (strcasecmp(map.mesh.meshes[k].import.group_name,import_state.group_name)!=0) continue;
			
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
			map_mesh_delete(&map,n);
		}
	}

	free(mesh_mark);
	
	select_clear();

	os_set_arrow_cursor();
	
	return(ok);
}

