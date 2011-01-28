/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: SVG Import

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
#include "dialog.h"

#define supergumba_temp_svg_high			(map_enlarge*20)

extern map_type					map;
extern editor_state_type		state;

extern file_path_setup_type		file_path_setup;

/* =======================================================

      Get OBJ Size
      
======================================================= */
/*
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
*/
/* =======================================================

      Create Mesh from OBJ Group
      
======================================================= */
/*
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
*/

/* =======================================================

      Create Meshes from SVG Path
      
======================================================= */

void import_svg_build_path(char *path_str,d3pnt *offset,int ty,int by)
{
	int					n,k,npnt,mesh_idx;
	int					px[4],py[4],pz[4];
	float				fx,fz,f_sz,gx[4],gy[4];
	bool				relative;
	char				c,pnt_str[64];
	char				*str,*str2,*c2;
	d3fpnt				pnts[128],min,max;
	
		// only do M or m
		
	if ((path_str[0]!='M') && (path_str[0]!='m')) return;
	
	relative=(path_str[0]=='m');
	
		// enlargement
		
	f_sz=10.0f;
	
		// decode points
		
	npnt=0;
	str=path_str+2;
	
	while (TRUE) {
		c=*str;
		
			// path over
		
		if (c==0x0) break;
		if (((c<'0') || (c>'9')) && (c!='-')) break;
		
			// get next path
			
		strncpy(pnt_str,str,64);
		pnt_str[63]=0x0;
		
		c2=strchr(pnt_str,' ');
		if (c2==NULL) break;
		
		*c2=0x0;
		
		c2=strchr(pnt_str,',');
		if (c2==NULL) break;
		
		*c2=0x0;
		
		pnts[npnt].x=atof(pnt_str);
		pnts[npnt].z=atof(c2+1);
		
		npnt++;
		
			// next part
			
		str2=strchr(str,' ');
		if (str2==NULL) break;
		str=str2+1;
	}
	
	if (npnt==0) return;
	
		// make relative paths
		// into absolute paths
		
	if (relative) {
	
		fx=fz=0;
		
		for (n=0;n!=npnt;n++) {
			pnts[n].x+=fx;
			pnts[n].z+=fz;
			fx=pnts[n].x;
			fz=pnts[n].z;
		}
	}
	
		// center points
		
	min.x=max.x=pnts[0].x;
	min.z=max.z=pnts[0].z;
		
	for (n=1;n<npnt;n++) {
		if (pnts[n].x<min.x) min.x=pnts[n].x;
		if (pnts[n].x>max.x) max.x=pnts[n].x;
		if (pnts[n].z<min.z) min.z=pnts[n].z;
		if (pnts[n].z>max.z) max.z=pnts[n].z;
	}
	
	fx=max.x-min.x;
	fz=max.z-min.z;
	
	for (n=0;n!=npnt;n++) {
		pnts[n].x-=fx;
		pnts[n].z-=fz;
	}
	
		// create the mesh
		
	mesh_idx=map_mesh_add(&map);
	
		// walls
		
	for (n=0;n!=npnt;n++) {
		k=n+1;
		if (k==npnt) k=0;
		
		px[0]=px[3]=(int)(pnts[n].x*f_sz)+offset->x;
		px[1]=px[2]=(int)(pnts[k].x*f_sz)+offset->x;
		pz[0]=pz[3]=(int)(pnts[n].z*f_sz)+offset->z;
		pz[1]=pz[2]=(int)(pnts[k].z*f_sz)+offset->z;
		py[0]=py[1]=ty;
		py[2]=py[3]=by;

		gx[0]=gx[3]=0.0f;
		gx[1]=gx[2]=1.0f;
		gy[0]=gy[1]=0.0f;
		gy[2]=gy[3]=1.0f;
	
		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,0);
	}
	
		// calc the normals
		
	map_recalc_normals_mesh(&map.mesh.meshes[mesh_idx],FALSE);
}

/* =======================================================

      Import SVG
      
======================================================= */

bool import_svg(char *path,char *err_str)
{
	int					ty,by,g_offset,path_offset,
						svg_tag,g_tag,path_tag;
	char				path_str[1024];
	d3pnt				pnt;
	
		// open the SVG file
		
	if (!xml_open_file(path)) {
 		strcpy(err_str,"Could not open SVG file.");
		return(FALSE);
    }
	
  	os_set_wait_cursor();
 
		// decode the file
       
    svg_tag=xml_findflat("svg",0);
    if (svg_tag==-1) {
		xml_close_file();
 		strcpy(err_str,"SVG file is missing SVG tag.");
		return(FALSE);
    }
	
		// import location
		
	piece_create_get_spot(&pnt);
	by=pnt.y+supergumba_temp_svg_high;
	
		// run through the layers
		
	g_offset=0;
	
	while (TRUE) {
	
			// get next group
			
		g_tag=xml_findflatinparent("g",svg_tag,g_offset);
		if (g_tag==-1) break;
		
		by-=supergumba_temp_svg_high;
		ty=by-supergumba_temp_svg_high;
		
			// get the paths
			
		path_offset=0;
		
		while (TRUE) {
		
				// get next path
				
			path_tag=xml_findflatinparent("path",g_tag,path_offset);
			if (path_tag==-1) break;
			
				// path definition
				
			if (xml_get_attribute_text(path_tag,"d",path_str,1024)) {
				import_svg_build_path(path_str,&pnt,ty,by);
			}

			path_offset++;
		}

		g_offset++;
	}
	
		// finish up
		
	xml_close_file();
	
	select_clear();
	os_set_arrow_cursor();
	
	return(TRUE);
}

