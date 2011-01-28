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

      Create Meshes from SVG Path
      
======================================================= */

void import_svg_build_floor_ceiling(int mesh_idx,int npnt,d3fpnt *pnts,int y)
{
	int				n,px[4],py[4],pz[4];
	float			gx[4],gy[4];

		// quads

	if (npnt==4) {
		for (n=0;n!=4;n++) {
			px[n]=(int)pnts[n].x;
			pz[n]=(int)pnts[n].z;
			py[n]=y;
		}

		gx[0]=gx[1]=0.0f;
		gx[2]=gx[3]=1.0f;
		gy[0]=gy[1]=0.0f;
		gy[2]=gy[3]=1.0f;
	
		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,1);

		return;
	}

		// split into triangles



}

void import_svg_build_path(char *path_str,int ty,int by)
{
	int					n,k,npnt,mesh_idx;
	int					px[4],py[4],pz[4];
	float				fx,fz,f_sz,gx[4],gy[4];
	bool				relative;
	char				c,pnt_str[64];
	char				*str,*str2,*c2;
	d3fpnt				pnts[128];
	
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
		
		pnts[npnt].x=(float)atof(pnt_str);
		pnts[npnt].z=(float)atof(c2+1);
		
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

		// enlarge points

	for (n=0;n!=npnt;n++) {
		pnts[n].x*=f_sz;
		pnts[n].z*=f_sz;
	}

		// create the mesh
		
	mesh_idx=map_mesh_add(&map);
	
		// walls
		
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

		// floors

	import_svg_build_floor_ceiling(mesh_idx,npnt,pnts,by);
	
		// calc the uv and normals
		
	map_mesh_reset_uv(&map,mesh_idx);
	map_recalc_normals_mesh(&map.mesh.meshes[mesh_idx],FALSE);
}

/* =======================================================

      Import SVG
      
======================================================= */

bool import_svg(char *path,char *err_str)
{
	int					mesh_start_idx,ty,by,g_offset,path_offset,
						svg_tag,g_tag,path_tag;
	char				path_str[1024];
	d3pnt				import_pnt;
	
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
		
	piece_create_get_spot(&import_pnt);
	by=import_pnt.y+supergumba_temp_svg_high;

		// first mesh

	mesh_start_idx=map.mesh.nmesh;
	
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
				import_svg_build_path(path_str,ty,by);
			}

			path_offset++;
		}

		g_offset++;
	}
	
		// close XML
		
	xml_close_file();

		// recenter around import point

	import_svg_recenter_meshes(mesh_start_idx,&import_pnt);

		// finish up
	
	select_clear();
	os_set_arrow_cursor();
	
	return(TRUE);
}

