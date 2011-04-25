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

#define import_svg_wall_floor				0
#define import_svg_wall_only				1
#define import_svg_floor_only				2

extern map_type					map;
extern editor_state_type		state;

extern file_path_setup_type		file_path_setup;

/* =======================================================

      Center Imported Meshes
      
======================================================= */

void import_svg_recenter_meshes(int mesh_start_idx,d3pnt *import_pnt)
{
	int				n,nmesh;
	d3pnt			min,max,min2,max2,mov_pnt;

		// find the total extent

	nmesh=map.mesh.nmesh;
	if (mesh_start_idx==nmesh) return;

	map_mesh_calculate_extent(&map,mesh_start_idx,&min,&max);

	for (n=(mesh_start_idx+1);n<nmesh;n++) {
		map_mesh_calculate_extent(&map,n,&min2,&max2);
		if (min2.x<min.x) min.x=min2.x;
		if (max2.x>max.x) max.x=max2.x;
		if (min2.z<min.z) min.z=min2.z;
		if (max2.z>max.z) max.z=max2.z;
	}

		// get move point

	mov_pnt.x=import_pnt->x-((min.x+max.x)>>1);
	mov_pnt.y=0;
	mov_pnt.z=import_pnt->z-((min.z+max.z)>>1);

		// center meshes around import point

	for (n=mesh_start_idx;n<nmesh;n++) {
		map_mesh_move(&map,n,&mov_pnt);
	}
}

/* =======================================================

      Delete Shared Polygons
      
======================================================= */

bool import_svg_delete_shared_polygons_compare(map_mesh_type *mesh_1,int poly_1_idx,map_mesh_type *mesh_2,int poly_2_idx)
{
	int					n,k;
	bool				hit;
	d3pnt				*pt_1,*pt_2;
	map_mesh_poly_type	*poly_1,*poly_2;

	poly_1=&mesh_1->polys[poly_1_idx];
	poly_2=&mesh_2->polys[poly_2_idx];

		// same vertex count?

	if (poly_1->ptsz!=poly_2->ptsz) return(FALSE);

		// might be in different orders, so look
		// for equal points anywhere in polygon

	for (n=0;n!=poly_1->ptsz;n++) {
		pt_1=&mesh_1->vertexes[poly_1->v[n]];

		hit=FALSE;
		for (k=0;k!=poly_2->ptsz;k++) {
			pt_2=&mesh_2->vertexes[poly_2->v[k]];
			if ((pt_1->x==pt_2->x) && (pt_1->y==pt_2->y) && (pt_1->z==pt_2->z)) {
				hit=TRUE;
				break;
			}
		}

		if (!hit) return(FALSE);
	}

	return(TRUE);
}

void import_svg_delete_shared_polygons(int start_mesh_idx)
{
	int					n,nmesh,mesh_1_idx,poly_1_idx,
						mesh_2_idx,poly_2_idx;
	map_mesh_type		*mesh_1,*mesh_2;
	
	nmesh=map.mesh.nmesh;

	for (mesh_1_idx=start_mesh_idx;mesh_1_idx<nmesh;mesh_1_idx++) {

		mesh_1=&map.mesh.meshes[mesh_1_idx];

		poly_1_idx=0;

		while (poly_1_idx<mesh_1->npoly) {

			poly_2_idx=-1;

			for (mesh_2_idx=start_mesh_idx;mesh_2_idx<nmesh;mesh_2_idx++) {

				mesh_2=&map.mesh.meshes[mesh_2_idx];

				for (n=0;n!=mesh_2->npoly;n++) {
				
					if ((mesh_1_idx==mesh_2_idx) && (poly_1_idx==n)) continue;
					
					if (import_svg_delete_shared_polygons_compare(mesh_1,poly_1_idx,mesh_2,n)) {
						poly_2_idx=n;
						break;
					}
				}

				if (poly_2_idx!=-1) break;
			}

			if (poly_2_idx!=-1) {
				map_mesh_delete_poly(&map,mesh_1_idx,poly_1_idx);
				if (mesh_1_idx==mesh_2_idx) {
					if (poly_1_idx<poly_2_idx) poly_2_idx--;		// need to fix index if same mesh
				}
				map_mesh_delete_poly(&map,mesh_2_idx,poly_2_idx);
				continue;
			}

			poly_1_idx++;
		}
	}
}

/* =======================================================

      Create and Pick Meshes
      
======================================================= */

int import_svg_get_mesh(int start_mesh_idx,char *fill_str)
{
	int				n,nmesh,mesh_idx;
	map_mesh_type	*mesh;
	
		// any old mesh with same fill?
		
	nmesh=map.mesh.nmesh;
	mesh=&map.mesh.meshes[start_mesh_idx];
	
	for (n=start_mesh_idx;n<nmesh;n++) {
		if (strcmp(mesh->import.group_name,fill_str)==0) return(n);
		mesh++;
	}
	
		// need a new mesh
		
	mesh_idx=map_mesh_add(&map);
	
	mesh=&map.mesh.meshes[mesh_idx];
	
	strcpy(mesh->import.obj_name,"SVG");
	strcpy(mesh->import.group_name,fill_str);
	
	return(mesh_idx);
}

/* =======================================================

      Create Walls and Floors
      
======================================================= */

void import_svg_build_wall(int mesh_idx,int npnt,d3fpnt *pnts,int ty,int by)
{
	int				n,k,px[4],py[4],pz[4];
	float			gx[4],gy[4];

	for (n=0;n!=npnt;n++) {
		k=n+1;
		if (k==npnt) k=0;
		
		px[0]=px[3]=(int)pnts[n].x;
		px[1]=px[2]=(int)pnts[k].x;
		pz[0]=pz[3]=(int)pnts[n].z;
		pz[1]=pz[2]=(int)pnts[k].z;
		py[0]=py[1]=ty;
		py[2]=py[3]=by;

		gx[0]=gx[3]=0.0f;
		gx[1]=gx[2]=1.0f;
		gy[0]=gy[1]=0.0f;
		gy[2]=gy[3]=1.0f;
	
		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,0);
	}
}

void import_svg_build_floor(int mesh_idx,int npnt,d3fpnt *pnts,int y)
{
	int				n,px[4],py[4],pz[4];
	float			gx[4],gy[4];

	for (n=0;n!=npnt;n++) {
		px[n]=(int)pnts[n].x;
		pz[n]=(int)pnts[n].z;
		py[n]=y;
		gx[n]=gy[n]=0.0f;
	}

	map_mesh_add_poly(&map,mesh_idx,npnt,px,py,pz,gx,gy,1);
}

/* =======================================================

      Decode Paths
      
======================================================= */

void import_svg_build_path(int start_mesh_idx,float f_sz,char *path_str,int ty,int by,char *fill_str,int import_mode)
{
	int					n,npnt,mesh_idx;
	float				fx,fz;
	bool				relative;
	char				c,pnt_str[64];
	char				*str,*str2,*c2;
	d3fpnt				pnts[128];
	
		// only do M or m
		
	if ((path_str[0]!='M') && (path_str[0]!='m')) return;
	
	relative=(path_str[0]=='m');
	
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
		
		pnts[npnt].x=(float)atof(pnt_str);
		pnts[npnt].z=(float)atof(c2+1);
		
		npnt++;
		
			// next part
			
		str2=strchr(str,' ');
		if (str2==NULL) break;
		str=str2+1;
	}
	
	if (npnt==0) return;
	
		// only do quads and triangles
		
	if ((npnt!=4) && (npnt!=3)) return;
	
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

		// enlarge points

	for (n=0;n!=npnt;n++) {
		pnts[n].x*=f_sz;
		pnts[n].z*=f_sz;
	}

		// create the pieces
		
	mesh_idx=import_svg_get_mesh(start_mesh_idx,fill_str);
		
	if (import_mode!=import_svg_floor_only) import_svg_build_wall(mesh_idx,npnt,pnts,ty,by);
	if (import_mode!=import_svg_wall_only) import_svg_build_floor(mesh_idx,npnt,pnts,by);
	
		// calc the uv and normals
		
	map_mesh_reset_uv(&map,mesh_idx);
	
	map_recalc_normals_mesh(&map.mesh.meshes[mesh_idx],normal_mode_none,FALSE);
}

/* =======================================================

      Style Decoding
      
======================================================= */

void import_svg_style_decode_fill(char *style_str,char *fill_str)
{
	char		*c;
	
	fill_str[0]=0x0;
	
	c=strstr(style_str,"fill:");
	if (c==NULL) return;
	
	strncpy(fill_str,(c+5),7);
	fill_str[7]=0x0;
}

int import_svg_style_decode_stroke(char *style_str)
{
	char		*c;
	
	if (strstr(style_str,"stroke:none")!=NULL) return(import_svg_floor_only);
	
	c=strstr(style_str,"stroke-width:");
	if (c!=NULL) {
		c+=13;
		if (*c!='1') return(import_svg_wall_only);
	}
	
	return(import_svg_wall_floor);
}

/* =======================================================

      Import SVG
      
======================================================= */

bool import_svg(char *path,char *err_str)
{
	int					start_mesh_idx,ty,by,g_count,g_offset,path_offset,
						svg_tag,g_tag,path_tag,import_mode,import_high;
	float				import_scale;
	char				style_str[256],path_str[256],fill_str[32],str[64];
	d3pnt				import_pnt;
	
		// open the SVG file
		
	if (!xml_open_file(path)) {
 		strcpy(err_str,"Could not open SVG file.");
		return(FALSE);
    }
	
		// get settings
		
	if (!dialog_svg_import_run(&import_scale,&import_high)) {
		xml_close_file();
		return(TRUE);
	}
	
  	os_set_wait_cursor();
 
		// decode the file
       
    svg_tag=xml_findflat("svg",0);
    if (svg_tag==-1) {
		xml_close_file();
 		strcpy(err_str,"SVG file is missing SVG tag.");
		return(FALSE);
    }
	
		// count the layers
	
	g_count=0;
	
	while (TRUE) {
		g_tag=xml_findflatinparent("g",svg_tag,g_count);
		if (g_tag==-1) break;
		g_count++;
	}

		// start progress
		
	progress_start("SVG Import",(3+g_count));
	progress_next_title("SVG Import: Decoding XML");

	
		// import location
		
	piece_create_get_spot(&import_pnt);
	by=import_pnt.y+import_high;

		// first mesh

	start_mesh_idx=map.mesh.nmesh;
	
		// run through the layers
		
	g_offset=0;
	
	while (TRUE) {
		
		sprintf(str,"SVG Import: Importing Layer %d",(g_offset+1));
		progress_next_title(str);

			// get next group
			
		g_tag=xml_findflatinparent("g",svg_tag,g_offset);
		if (g_tag==-1) break;
		
		by-=import_high;
		ty=by-import_high;
		
			// get the paths
			
		path_offset=0;
		
		while (TRUE) {
		
				// get next path
				
			path_tag=xml_findflatinparent("path",g_tag,path_offset);
			if (path_tag==-1) break;
			
				// path definition
				
			if (xml_get_attribute_text(path_tag,"d",path_str,256)) {
				xml_get_attribute_text(path_tag,"style",style_str,256);
				import_svg_style_decode_fill(style_str,fill_str);
				import_mode=import_svg_style_decode_stroke(style_str);
				import_svg_build_path(start_mesh_idx,import_scale,path_str,ty,by,fill_str,import_mode);
			}

			path_offset++;
		}

		g_offset++;
	}
	
		// close XML
		
	progress_next_title("SVG Import: Finishing Up");

	xml_close_file();

		// recenter around import point

	import_svg_recenter_meshes(start_mesh_idx,&import_pnt);
	
		// delete any shared polygons
		
	import_svg_delete_shared_polygons(start_mesh_idx);

		// finish up
	
	select_clear();
	
	progress_end();
	os_set_arrow_cursor();
	
	return(TRUE);
}

