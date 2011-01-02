/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Piece Mesh

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
#include "dialog.h"

#define import_obj_float_to_int			1000.0f
#define import_obj_max_dimension		5000

extern map_type					map;
extern editor_state_type		state;

extern file_path_setup_type		file_path_setup;


typedef struct		{
						int						nline,
												nvertex,npoly,nuv,nnormal;
						char					obj_name[name_str_len];
						float					scale;
						d3pnt					pnt;
					} obj_import_state_type;


/* =======================================================

      Finish Mesh Addition
      
======================================================= */

void piece_add_mesh_finish(int mesh_idx)
{
	if (mesh_idx==-1) return;
	
		// make selection
		
	select_clear();
	select_add(mesh_piece,mesh_idx,0);

		// change mode to move entire mesh
		
	state.drag_mode=drag_mode_mesh;
	
		// if import hasn't already locked UV,
		// lock it if auto texture state is off
		// otherwise, reset the UVs
	
	if ((state.auto_texture) && (!map.mesh.meshes[mesh_idx].flag.lock_uv)) {
		map_mesh_reset_uv(&map,mesh_idx);
	}
	else {
		map.mesh.meshes[mesh_idx].flag.lock_uv=TRUE;
	}
	
	main_wind_draw();
}

/* =======================================================

      OBJ Replacementment
      
======================================================= */

bool piece_add_obj_is_replace_ok(float *old_scale)
{
	int				n,type,mesh_idx,poly_idx,sel_cnt,hit_mesh_idx;
	
	sel_cnt=select_count();
	if (sel_cnt==0) return(FALSE);

	hit_mesh_idx=-1;
		
	for (n=0;n!=sel_cnt;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type!=mesh_piece) continue;
		
		if (hit_mesh_idx!=-1) {
			if (hit_mesh_idx!=mesh_idx) return(FALSE);			// can only replace one mesh
		}
		
		hit_mesh_idx=mesh_idx;
		
		*old_scale=map.mesh.meshes[mesh_idx].import.factor;
	}
	
	return(hit_mesh_idx!=-1);
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
		
	sprintf(title,"Material %s Found - Select Bitmap",material_name);
	os_dialog_alert(title,"Please select a PNG file to be used for this material.\nThe PNG file must be 32-bit and have width and height that are squares of 2 (2, 4, 8, 16, 32, 64, 128, 256, etc).");
	
	if (!os_load_file(path,"png")) return(0);
	
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

/* =======================================================

      Add OBJ Mesh
      
======================================================= */

bool piece_add_obj_mesh_import_obj(obj_import_state_type *import_state,char *err_str)
{
	int					n,k,npt,
						v_idx,uv_idx,normal_idx,normal_count,
						mesh_idx,poly_idx,txt_idx,old_nmesh,x,y,z;
	int					px[8],py[8],pz[8];
	char				*c,txt[256],material_name[256],
						vstr[256],uvstr[256],normalstr[256];
	float				fx,fy,fz,gx[8],gy[8];
	float				*uvs,*uv,*normals,*normal;
	bool				mesh_add;
	d3pnt				*vertexes,*dpt;
	d3vct				n_v;
	map_mesh_type		*mesh;

		// start progress
		
	progress_start("OBJ Import",8);
	
		// get the vertexes
		
	progress_next_title("Obj Import: Loading Vertexes");

	vertexes=(d3pnt*)malloc(sizeof(d3pnt)*import_state->nvertex);
	if (vertexes==NULL) {
		progress_end();
		strcpy(err_str,"Out of Memory.");
		return(FALSE);
    }
	
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
		
		dpt->x=(int)(fx*import_state->scale);
		dpt->y=(int)(fy*import_state->scale);
		dpt->z=(int)(fz*import_state->scale);
		
		dpt++;
	}
		
		// recenter the vertexes
		
	progress_next();
		
	x=y=z=0;
	
	dpt=vertexes;
	
	for (n=0;n!=import_state->nvertex;n++) {
		x+=dpt->x;
		y+=dpt->y;
		z+=dpt->z;
		dpt++;
	}
	
	x/=import_state->nvertex;
	y/=import_state->nvertex;
	z/=import_state->nvertex;
		
	dpt=vertexes;
	
	for (n=0;n!=import_state->nvertex;n++) {
		dpt->x=(dpt->x-x)+import_state->pnt.x;
		dpt->y=(dpt->y-y)+import_state->pnt.y;
		dpt->z=(dpt->z-z)+import_state->pnt.z;
		dpt++;
	}
	
		// get the UVs
		
	progress_next_title("Obj Import: Loading UVs");
		
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
		
	progress_next_title("Obj Import: Loading Normals");
		
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

		// need to split up meshes by materials
		
	mesh=NULL;
	txt_idx=0;
	
		// remember the number of meshes
		// we add so we can fix some things later
		
	old_nmesh=map.mesh.nmesh;
	
		// get the polys
		
	progress_next_title("Obj Import: Building Polygons");

    for (n=0;n!=import_state->nline;n++) {
	
       textdecode_get_piece(n,0,txt);
	
			// new material, add a new mesh
			// if we don't have one yet and if we do,
			// the older one actually has polygons
			
		if (strcmp(txt,"usemtl")==0) {
		
				// get material and texture
				
			textdecode_get_piece(n,1,material_name);
			txt_idx=import_texture_pick(material_name);
		
				// add the mesh
				
			mesh_add=TRUE;
			
			if (mesh!=NULL) {
				mesh_add=(mesh->npoly!=0);
			}
				
			if (mesh_add) {
			
				mesh_idx=map_mesh_add(&map);
				
				if (mesh_idx==-1) {
					progress_end();
					strcpy(err_str,"Not enough memory to create mesh.");
					return(FALSE);
				}
	
				mesh=&map.mesh.meshes[mesh_idx];
				
					// set the scale for replacements
					
				mesh->import.factor=import_state->scale;
				strcpy(mesh->import.obj_name,import_state->obj_name);
				
					// set UV and Normal locks if uvs and normals
					// exist in obj
					
				mesh->flag.lock_uv=(import_state->nuv!=0);
				if (import_state->nnormal!=0) map.mesh.meshes[mesh_idx].normal_mode=mesh_normal_mode_lock;
			}
		}
		
			// skip if we don't have a mesh
			// we have to encounter a material first
			
		if (mesh==NULL) continue;

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

		memmove(&map.mesh.meshes[mesh_idx].polys[poly_idx].tangent_space.normal,&n_v,sizeof(d3vct));
	}
	
	free(vertexes);
	if (import_state->nuv!=0) free(uvs);
	if (import_state->nnormal!=0) free(normals);
	
		// were any meshes imported?
		
	if (old_nmesh==map.mesh.nmesh) {
		os_dialog_alert("Import Failed","Could not find mesh.  OBJ is missing a material (usemtl) deceleration.");
		progress_end();
		return(FALSE);
	}
	
		// if no uvs, force auto-texture
		
	progress_next();
		
	if (import_state->nuv==0) {
		for (n=old_nmesh;n!=map.mesh.nmesh;n++) {
			map_mesh_reset_uv(&map,n);
		}
	}

		// calc the normals
		// and lock if this import had normals
		
	progress_next();

	for (n=old_nmesh;n!=map.mesh.nmesh;n++) {
		map_recalc_normals_mesh(&map.mesh.meshes[n],(import_state->nnormal!=0));
	}
	
		// finish up
		
	progress_next();
		
	piece_add_mesh_finish(old_nmesh);
	
	progress_end();
	
	return(TRUE);
}

void piece_add_obj_mesh(void)
{
	int					n,k,v_idx,nmesh,
						import_mode,scale_axis,scale_unit;
	char				*c,txt[256],file_name[256],
						vstr[256],path[1024];
	unsigned char		*vertex_mark,*mesh_mark;
	float				fx,fy,fz,old_scale;
	bool				replace_ok;
	d3fpnt				min,max;
	
	bool					ok;
	char					err_str[256];
	obj_import_state_type	import_state;
	
	if (!piece_create_texture_ok()) return;
	
		// get import location

	piece_create_get_spot(&import_state.pnt);
	
		// import the file
		
	if (!os_load_file(path,"obj")) return;

	if (!textdecode_open(path,0x20)) {
		os_dialog_alert("Import Failed","Could not open OBJ file.");
		return;
    }
	
	os_set_wait_cursor();
	
		// get file name
		
	c=strrchr(path,'/');
	if (c==NULL) c=strrchr(path,'\\');
	if (c==NULL) {
		file_name[0]=0x0;
	}
	else {
		strcpy(file_name,(c+1));
		c=strrchr(file_name,'.');
		if (c!=NULL) *c=0x0;
	}
	
	strncpy(import_state.obj_name,file_name,name_str_len);
	import_state.obj_name[name_str_len-1]=0x0;
	
		// count vertexes, faces, uvs, and normals
		
    import_state.nline=textdecode_count();
		
	import_state.nvertex=0;
	import_state.npoly=0;
	import_state.nuv=0;
	import_state.nnormal=0;
	
    for (n=0;n!=import_state.nline;n++) {
        textdecode_get_piece(n,0,txt);
        
        if (strcmp(txt,"v")==0) {
			import_state.nvertex++;
		}
		else {
			if (strcmp(txt,"f")==0) {
				import_state.npoly++;
			}
			else {
				if (strcmp(txt,"vt")==0) {
					import_state.nuv++;
				}
				else {
					if (strcmp(txt,"vn")==0) {
						import_state.nnormal++;
					}
				}
			}
		}
	}
	
		// imporper OBJ errors
		
	if (import_state.nvertex==0) {
		textdecode_close();
		os_dialog_alert("Import Failed","No vertexes in OBJ.");
		return;
    }
	if (import_state.npoly==0) {
		textdecode_close();
		os_dialog_alert("Import Failed","No faces in OBJ.");
		return;
    }
	
		// some OBJ creators -- especially blender --
		// like to leave this unconnected and crazy vertexes
		// in the OBJs.  We need to scan for them.
		
	vertex_mark=(unsigned char*)malloc(import_state.nvertex);
	bzero(vertex_mark,import_state.nvertex);
		
    for (n=0;n!=import_state.nline;n++) {
        textdecode_get_piece(n,0,txt);
        
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
		
		// mesh add will actually eliminate these
		// vertexes but we need to keep them around for
		// now so face numbers line up
		
	min.x=min.y=min.z=0.0f;
	max.x=max.y=max.z=0.0f;
	
	v_idx=0;
	
	for (n=0;n!=import_state.nline;n++) {
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
				min.x=max.x=fx;
				min.y=max.y=fy;
				min.z=max.z=fz;
			}
			else {
				if (fx<min.x) min.x=fx;
				if (fx>max.x) max.x=fx;
				if (fy<min.y) min.y=fy;
				if (fy>max.y) max.y=fy;
				if (fz<min.z) min.z=fz;
				if (fz>max.z) max.z=fz;
			}
			
			v_idx++;
		}
	}
	
	free(vertex_mark);
	
	os_set_arrow_cursor();
	
		// run the import dialog

	replace_ok=piece_add_obj_is_replace_ok(&old_scale);
	import_mode=dialog_mesh_scale_run(replace_ok,&scale_axis,&scale_unit);
	
	if (import_mode==-1) {
		textdecode_close();
		return;
    }
	
	import_state.scale=1.0f;
	
		// scale import
		
	if (import_mode==import_mode_scale) {
		
		switch (scale_axis) {
			case 0:
				import_state.scale=((float)scale_unit)/(max.x-min.x);
				break;
			case 1:
				import_state.scale=((float)scale_unit)/(max.y-min.y);
				break;
			case 2:
				import_state.scale=((float)scale_unit)/(max.z-min.z);
				break;
		}
	
		os_set_wait_cursor();
		ok=piece_add_obj_mesh_import_obj(&import_state,err_str);
		os_set_arrow_cursor();
		
		textdecode_close();

		if (!ok) os_dialog_alert("Import Error",err_str);

		return;
	}
	
		// replacement import

	if (import_mode==import_mode_replace) {
		import_state.scale=old_scale;
		if (import_state.scale==0.0f) import_state.scale=1.0f;

		os_set_wait_cursor();
		piece_add_obj_replace_delete_existing();
		ok=piece_add_obj_mesh_import_obj(&import_state,err_str);
		os_set_arrow_cursor();
			
		textdecode_close();
		
		if (!ok) os_dialog_alert("Import Error",err_str);

		return;
	}
	
		// replace all
		
	os_set_wait_cursor();

	nmesh=map.mesh.nmesh;			// operation will add meshes, so we need to not go into them
		
	mesh_mark=(unsigned char*)malloc(nmesh);
	if (mesh_mark==NULL) {
		textdecode_close();
		os_set_arrow_cursor();
		os_dialog_alert("Import Error","Out of Memory");
		return;
	}
	
	bzero(mesh_mark,nmesh);
	
	for (n=0;n!=nmesh;n++) {
	
			// same imported OBJ?
			
		if (strcasecmp(map.mesh.meshes[n].import.obj_name,import_state.obj_name)!=0) continue;
		
			// get scale and mark for delete
		
		import_state.scale=map.mesh.meshes[n].import.factor;
		if (import_state.scale==0.0f) import_state.scale=1.0f;
			
		map_mesh_calculate_center(&map,n,&import_state.pnt);
		mesh_mark[n]=0x1;
	
			// import
	
		ok=piece_add_obj_mesh_import_obj(&import_state,err_str);
		
		if (!ok) break;
	}
	
	textdecode_close();
		
		// delete marked meshes

	for (n=(nmesh-1);n>=0;n--) {
		if (mesh_mark[n]==0x1) map_mesh_delete(&map,n);
	}

	free(mesh_mark);
	
	select_clear();
	
	os_set_arrow_cursor();
	
	if (!ok) os_dialog_alert("Import Error",err_str);
}

/* =======================================================

      Add OBJ Mesh UV
      
======================================================= */

void piece_add_obj_mesh_uv(void)
{
	int						n,k,type,nline,nvertex,npoly,nuv,npt,uv_idx,
							mesh_idx,poly_idx;
	char					*c,txt[256],vstr[256],uvstr[256],path[1024];
	float					fx,fy,fz;
	float					*uvs,*uv;
	d3fpnt					min,max;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;
	map_mesh_poly_uv_type	*uv_ptr;
	
		// get mesh to replace UVs on
		
	select_get(0,&type,&mesh_idx,&poly_idx);
	if (type!=mesh_piece) {
		os_dialog_alert("No mesh selected","You need to select a mesh to replace the UVs on.");
		return;
	}
	
	mesh=&map.mesh.meshes[mesh_idx];
	
		// import the file
		
	if (!os_load_file(path,"obj")) return;

	if (!textdecode_open(path,0x20)) {
		os_dialog_alert("Import Failed","Could not open OBJ file.");
		return;
    }
	
    nline=textdecode_count();
	
		// count vertexes, uvs, and faces
		
	nvertex=0;
	npoly=0;
	nuv=0;
	
	min.x=min.y=min.z=0.0f;
	max.x=max.y=max.z=0.0f;
	
    for (n=0;n!=nline;n++) {
        textdecode_get_piece(n,0,txt);
        
        if (strcmp(txt,"v")==0) {
			nvertex++;
			
			textdecode_get_piece(n,1,txt);
			fx=(float)strtod(txt,NULL);
			textdecode_get_piece(n,2,txt);
			fy=(float)strtod(txt,NULL);
			textdecode_get_piece(n,3,txt);
			fz=(float)strtod(txt,NULL);
			
			if (fx<min.x) min.x=fx;
			if (fx>max.x) max.x=fx;
			if (fy<min.y) min.y=fy;
			if (fy>max.y) max.y=fy;
			if (fz<min.z) min.z=fz;
			if (fz>max.z) max.z=fz;
		}
		else {
			if (strcmp(txt,"f")==0) {
				npoly++;
			}
			else {
				if (strcmp(txt,"vt")==0) {
					nuv++;
				}
			}
		}
	}
	
		// does this OBJ match OBJ importing over?
		
	if (nuv==0) {
		textdecode_close();
		os_dialog_alert("Import Failed","The imported OBJ has no UVs.");
		return;
    }
		
	if (npoly!=mesh->npoly) {
		textdecode_close();
		os_dialog_alert("Import Failed","The imported OBJ must have the same number of polygons as the selected mesh.");
		return;
    }
	
		// get the UVs
		
	uvs=(float*)malloc(sizeof(float)*(2*nuv));
	if (uvs==NULL) {
		textdecode_close();
		os_dialog_alert("Import Failed","Out of Memory.");
		return;
	}

	uv=uvs;

	for (n=0;n!=nline;n++) {
		textdecode_get_piece(n,0,txt);
		if (strcmp(txt,"vt")!=0) continue;
				
		textdecode_get_piece(n,1,uvstr);
		*uv++=(float)strtod(uvstr,NULL);
		textdecode_get_piece(n,2,uvstr);
		*uv++=1.0f-(float)strtod(uvstr,NULL);
	}

		// replace the UVs
		
	poly_idx=0;
	
    for (n=0;n!=nline;n++) {
	
			// a face?
			
		textdecode_get_piece(n,0,txt);
		if (strcmp(txt,"f")!=0) continue;
		
			// out of polys?
			
		if (poly_idx>=mesh->npoly) break;
		
		poly=&mesh->polys[poly_idx];
		poly_idx++;
		
            // get the face points
        
        npt=0;
		
		if (view_get_uv_layer()==uv_layer_normal) {
			uv_ptr=&poly->main_uv;
		}
		else {
			uv_ptr=&poly->lmap_uv;
		}
        
        for (k=0;k!=8;k++) {
            textdecode_get_piece(n,(k+1),txt);
            if (txt[0]==0x0) break;
            
            uvstr[0]=0x0;
            
            strcpy(vstr,txt);
            c=strchr(vstr,'/');
            if (c!=NULL) {
                strcpy(uvstr,(c+1));
                *c=0x0;
            }
            c=strchr(uvstr,'/');
            if (c!=NULL) *c=0x0;
            
			if (uvstr[0]==0x0) {
				uv_ptr->x[npt]=uv_ptr->y[npt]=0.0f;
			}
			else {
				uv_idx=atoi(uvstr)-1;
				
				uv=uvs+(uv_idx*2);
                uv_ptr->x[npt]=*uv++;
                uv_ptr->y[npt]=*uv;
            }
			
            npt++;
        }
	}
	
	free(uvs);
	
	textdecode_close();
	
		// finish up
		
	main_wind_draw();
}

/* =======================================================

      Add Height Map Mesh
      
======================================================= */

float piece_add_height_map_mesh_get_height(unsigned char *data,int wid,int high,bool alpha_channel,int x,int z,int div_cnt)
{
	int				y;
	unsigned char	*p;
	
		// find offset in portal
		
	x=(int)(((float)x/(float)div_cnt)*(float)wid);
	y=(int)(((float)z/(float)div_cnt)*(float)high);
	
	if (x<0) x=0;
	if (x>=wid) x=wid-1;
	if (y<0) y=0;
	if (y>=high) y=high-1;
	
	if (alpha_channel) {
		p=data+((y*(wid<<2))+(x<<2));
	}
	else {
		p=data+((y*(wid*3))+(x*3));
	}
	
	return(((float)*p)/255.0f);
}

void piece_add_height_map_mesh(void)
{
	int					x,z,px,pz,div_cnt,div_sz,total_sz,bwid,bhigh,high,
						kx[4],ky[4],kz[4],y[4],
						mesh_idx,txt_idx;
	float				f_portal_y_sz,gx[4],gy[4];
	char				path[1024];
	unsigned char		*data;
	bool				alpha_channel;
	d3pnt				pnt;
	
		// get the png
		
	if (!os_load_file(path,"png")) return;
	data=bitmap_load_png_data(path,&bwid,&bhigh,&alpha_channel);
	if (data==NULL) return;
	
		// division and sizes
		
	if (!dialog_height_import_run(&div_cnt,&total_sz,&high)) {
		free(data);
		return;
	}
		
	div_sz=total_sz/div_cnt;
	f_portal_y_sz=(float)high;
	
	piece_create_get_spot(&pnt);
	
		// texture
		
	txt_idx=texture_palette_get_selected_texture();
	if (txt_idx==-1) txt_idx=0;
	
		// polygons
		
	mesh_idx=map_mesh_add(&map);
	if (mesh_idx==-1) {
		free(data);
		return;
	}
	
	os_set_wait_cursor();
	
	for (z=0;z!=div_cnt;z++) {
		for (x=0;x!=div_cnt;x++) {
			
			px=(pnt.x-(total_sz/2))+(x*div_sz);
			pz=(pnt.z-(total_sz/2))+(z*div_sz);
			
				// floors
				
			y[0]=pnt.y-(int)(f_portal_y_sz*piece_add_height_map_mesh_get_height(data,bwid,bhigh,alpha_channel,x,z,div_cnt));
			y[1]=pnt.y-(int)(f_portal_y_sz*piece_add_height_map_mesh_get_height(data,bwid,bhigh,alpha_channel,(x+1),z,div_cnt));
			y[2]=pnt.y-(int)(f_portal_y_sz*piece_add_height_map_mesh_get_height(data,bwid,bhigh,alpha_channel,(x+1),(z+1),div_cnt));
			y[3]=pnt.y-(int)(f_portal_y_sz*piece_add_height_map_mesh_get_height(data,bwid,bhigh,alpha_channel,x,(z+1),div_cnt));
										
			gx[0]=gx[1]=gx[2]=0.0f;
			gy[0]=gy[1]=gy[2]=0.0f;
					
			if (((x+z)&0x1)!=0) {
				kx[0]=px;
				kx[1]=kx[2]=px+div_sz;
				kz[0]=kz[1]=pz;
				kz[2]=pz+div_sz;
				ky[0]=y[0];
				ky[1]=y[1];
				ky[2]=y[2];
				
				map_mesh_add_poly(&map,mesh_idx,3,kx,ky,kz,gx,gy,txt_idx);
			
				kx[0]=kx[2]=px;
				kx[1]=px+div_sz;
				kz[0]=pz;
				kz[1]=kz[2]=pz+div_sz;
				ky[0]=y[0];
				ky[1]=y[2];
				ky[2]=y[3];
				
				map_mesh_add_poly(&map,mesh_idx,3,kx,ky,kz,gx,gy,txt_idx);
			}
			else {
				kx[0]=kx[2]=px;
				kx[1]=px+div_sz;
				kz[0]=kz[1]=pz;
				kz[2]=pz+div_sz;
				ky[0]=y[0];
				ky[1]=y[1];
				ky[2]=y[3];
				
				map_mesh_add_poly(&map,mesh_idx,3,kx,ky,kz,gx,gy,txt_idx);
			
				kx[0]=kx[1]=px+div_sz;
				kx[2]=px;
				kz[0]=pz;
				kz[1]=kz[2]=pz+div_sz;
				ky[0]=y[1];
				ky[1]=y[2];
				ky[2]=y[3];
				
				map_mesh_add_poly(&map,mesh_idx,3,kx,ky,kz,gx,gy,txt_idx);
			}
		}
	}
	
		// reset UVs
				
	map_mesh_reset_uv(&map,mesh_idx);
		
	free(data);
	
	os_set_arrow_cursor();
	
		// finish up
		
	piece_add_mesh_finish(mesh_idx);
}

/* =======================================================

      Add Grid Mesh
      
======================================================= */

void piece_add_grid_mesh(void)
{
	int				mesh_idx,xdiv,ydiv,zdiv,
					x,y,z,sz,txt_idx,
					px[4],py[4],pz[4];
	float			gx[4],gy[4];
	d3pnt			pnt;
	
	if (!piece_create_texture_ok()) return;

		// get mesh divisions
		
	if (!dialog_create_grid_mesh_run(&xdiv,&ydiv,&zdiv)) return;
	
		// texture and size
		
	txt_idx=texture_palette_get_selected_texture();
	if (txt_idx==-1) txt_idx=0;
	
	sz=map_enlarge*5;

	piece_create_get_spot(&pnt);
	pnt.x-=((xdiv*sz)/2);
	pnt.y-=((ydiv*sz)/2);
	pnt.z-=((zdiv*sz)/2);

		// create mesh
		
	mesh_idx=map_mesh_add(&map);
	if (mesh_idx==-1) return;
	
		// default UVs
		
	gx[0]=gx[1]=gx[2]=gx[3]=0.0f;
	gy[0]=gy[1]=gy[2]=gy[3]=0.0f;
	
	os_set_wait_cursor();
	
		// add top and bottom polys
		
	for (z=0;z!=zdiv;z++) {
		for (x=0;x!=xdiv;x++) {
			px[0]=px[3]=(x*sz)+pnt.x;
			px[1]=px[2]=((x+1)*sz)+pnt.x;
			pz[0]=pz[1]=(z*sz)+pnt.z;
			pz[2]=pz[3]=((z+1)*sz)+pnt.z;

			py[0]=py[1]=py[2]=py[3]=pnt.y;
			map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,txt_idx);
			
			if (ydiv!=0) {
				py[0]=py[1]=py[2]=py[3]=(ydiv*sz)+pnt.y;
				map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,txt_idx);
			}
		}
	}
	
		// sides
		
	for (x=0;x!=xdiv;x++) {
		for (y=0;y!=ydiv;y++) {
			px[0]=px[3]=(x*sz)+pnt.x;
			px[1]=px[2]=((x+1)*sz)+pnt.x;
			py[0]=py[1]=(y*sz)+pnt.y;
			py[2]=py[3]=((y+1)*sz)+pnt.y;
			
			pz[0]=pz[1]=pz[2]=pz[3]=pnt.z;
			map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,txt_idx);
			
			if (zdiv!=0) {
				pz[0]=pz[1]=pz[2]=pz[3]=(zdiv*sz)+pnt.z;
				map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,txt_idx);
			}
		}
	}
	
	for (z=0;z!=zdiv;z++) {
		for (y=0;y!=ydiv;y++) {
			pz[0]=pz[1]=(z*sz)+pnt.z;
			pz[2]=pz[3]=((z+1)*sz)+pnt.z;
			py[0]=py[3]=(y*sz)+pnt.y;
			py[1]=py[2]=((y+1)*sz)+pnt.y;
			
			px[0]=px[1]=px[2]=px[3]=pnt.x;
			map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,txt_idx);
			
			if (xdiv!=0) {
				px[0]=px[1]=px[2]=px[3]=(xdiv*sz)+pnt.x;
				map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,txt_idx);
			}
		}
	}
	
		// reset UVs
		
	map_mesh_reset_uv(&map,mesh_idx);
	
	os_set_arrow_cursor();
	
		// finish up
		
	piece_add_mesh_finish(mesh_idx);
}

/* =======================================================

      Add Polygon Mesh
      
======================================================= */

void piece_add_polygon_mesh(void)
{
	int				mesh_idx,sz,txt_idx,
					px[4],py[4],pz[4];
	float			gx[4],gy[4];
	d3pnt			pnt;
	
	if (!piece_create_texture_ok()) return;
	
		// texture and size
		
	txt_idx=texture_palette_get_selected_texture();
	if (txt_idx==-1) txt_idx=0;
	
		// create mesh
		
	mesh_idx=map_mesh_add(&map);
	if (mesh_idx==-1) return;
	
		// create polygon
		
	piece_create_get_spot(&pnt);

	sz=map_enlarge*5;

	px[0]=px[3]=pnt.x-sz;
	px[1]=px[2]=pnt.x+sz;
	pz[0]=pz[1]=pnt.z-sz;
	pz[2]=pz[3]=pnt.z+sz;
	py[0]=py[1]=py[2]=py[3]=pnt.y;
	
	gx[0]=gx[1]=gx[2]=gx[3]=0.0f;
	gy[0]=gy[1]=gy[2]=gy[3]=0.0f;

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,txt_idx);
	
		// reset UVs
		
	map_mesh_reset_uv(&map,mesh_idx);
	
		// finish up
		
	piece_add_mesh_finish(mesh_idx);
}

/* =======================================================

      Combine Meshes
      
======================================================= */

void piece_combine_mesh(void)
{
	int				n,k,nsel,org_idx,mesh_combine_idx,new_mesh_combine_idx,
					type,mesh_idx,poly_idx;
	int				sel_mesh_idx[select_max_item];
	bool			skip;
	
		// get all selected meshes
		// select will change as meshes are deleted so
		// we need our own list.  we also have to check
		// for duplicates as multiple polygons can be in list
		
	nsel=0;
	
	for (n=0;n!=select_count();n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type!=mesh_piece) continue;
		
		skip=FALSE;
		
		for (k=0;k!=nsel;k++) {
			if (sel_mesh_idx[k]==mesh_idx) {
				skip=TRUE;
				break;
			}
		}
		
		if (!skip) sel_mesh_idx[nsel++]=mesh_idx;
	}
	
	if (nsel<2) return;
	
	
		// clear selection
		
	select_clear();
	
		// combine all meshes
		
	mesh_combine_idx=sel_mesh_idx[0];
	
	for (n=1;n<nsel;n++) {
	
			// combine
			
		mesh_idx=sel_mesh_idx[n];
		new_mesh_combine_idx=map_mesh_combine(&map,mesh_combine_idx,mesh_idx);
		if (new_mesh_combine_idx==-1) return;
		
			// move other meshes in select as combine deleted them
			
		for (k=(n+1);k<nsel;k++) {
			org_idx=sel_mesh_idx[k];
			if (org_idx>mesh_idx) sel_mesh_idx[k]--;
			if (org_idx>mesh_combine_idx) sel_mesh_idx[k]--;
		}
		
		mesh_combine_idx=new_mesh_combine_idx;
	}
	
	if (mesh_combine_idx!=-1) select_add(mesh_piece,mesh_combine_idx,0);
}

/* =======================================================

      Split Meshes
      
======================================================= */

void piece_split_mesh(void)
{
	int					n,k,mesh_idx,x[8],y[8],z[8],
						type,add_mesh_idx,add_poly_idx;
	bool				first_mesh;
	d3pnt				*pt;
	map_mesh_type		*mesh,*copy_mesh;
	map_mesh_poly_type	*poly;
	
		// create new mesh
		
	mesh_idx=map_mesh_add(&map);
	if (mesh_idx==-1) return;
	
		// add in the selected polygons
		
	first_mesh=FALSE;
	mesh=&map.mesh.meshes[mesh_idx];
		
	for (n=0;n!=select_count();n++) {
		select_get(n,&type,&add_mesh_idx,&add_poly_idx);
		if (type!=mesh_piece) continue;
		
			// if first mesh hit, copy mesh settings
			
		copy_mesh=&map.mesh.meshes[add_mesh_idx];
		
		if (first_mesh) {
			mesh->group_idx=copy_mesh->group_idx;
			memmove(&mesh->flag,&copy_mesh->flag,sizeof(map_mesh_flag_type));
			memmove(&mesh->msg,&copy_mesh->msg,sizeof(map_mesh_message_type));
			
			first_mesh=FALSE;
		}
		
			// add the polygon
		
		poly=&map.mesh.meshes[add_mesh_idx].polys[add_poly_idx];
		
		for (k=0;k!=poly->ptsz;k++) {
			pt=&copy_mesh->vertexes[poly->v[k]];
			x[k]=pt->x;
			y[k]=pt->y;
			z[k]=pt->z;
		}
		
		map_mesh_add_poly(&map,mesh_idx,poly->ptsz,x,y,z,poly->main_uv.x,poly->main_uv.y,poly->txt_idx);
		
			// delete poly from mesh
			
		select_delete_move_index(mesh_piece,add_mesh_idx,add_poly_idx);
		map_mesh_delete_poly(&map,add_mesh_idx,add_poly_idx);
	}
	
		// if no polygons, delete mesh
		
	if (map.mesh.meshes[mesh_idx].npoly==0) {
		map_mesh_delete(&map,mesh_idx);
		return;
	}
	
		// make new mesh selection
		
	select_clear();
	select_add(mesh_piece,mesh_idx,0);
	
		// auto-switch to mesh drag mode
		
	state.drag_mode=drag_mode_mesh;

	main_wind_draw();
}

/* =======================================================

      Map Changes
      
======================================================= */

void map_mesh_move_all(int x,int y,int z)
{
	int				n;
	d3pnt			pt;
	
	pt.x=x;
	pt.y=y;
	pt.z=z;
	
	for (n=0;n!=map.mesh.nmesh;n++) {
		map_mesh_move(&map,n,&pt);
	}
}

void map_mesh_reset_uv_all(void)
{
	int				n;
	
	for (n=0;n!=map.mesh.nmesh;n++) {
		if (!map.mesh.meshes[n].flag.lock_uv) map_mesh_reset_uv(&map,n);
	}
}
