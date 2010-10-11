/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Auto Generation Routines

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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "interface.h"
#include "dialog.h"
#include "walk_view.h"

#define ag_max_shape_point			64

typedef struct		{
						int						npnt;
						char					name[name_str_len];
						d3pnt					pnts[ag_max_shape_point];
					} ag_shape_type;

extern map_type					map;
extern file_path_setup_type		file_path_setup;

int								ag_nshape;
ag_shape_type					*ag_shapes;

/* =======================================================

      Auto Generate Utilities
      
======================================================= */

void ag_map_clear(void)
{
		// clear map

	map.nspot=0;
	map.nnode=0;
	map.nscenery=0;
	map.nlight=0;
	map.nsound=0;
	map.nparticle=0;
	map.nmovement=0;
	map.ngroup=0;

	map.mesh.nmesh=0;
	if (map.mesh.meshes!=NULL) free(map.mesh.meshes);

	map.liquid.nliquid=0;
	if (map.liquid.liquids!=NULL) free(map.liquid.liquids);
}

/* supergumba
void map_auto_generate_random_seed(int seed)
{
	srandom(seed);
}

int map_auto_generate_random_int(int max)
{
	float			f;

	f=(float)random()/(float)RAND_MAX;
	return((int)(f*(float)max));
}
*/

/* =======================================================

      Auto Generate XML
      
======================================================= */

bool ag_read_xml(void)
{
	int					n,k,head_tag,shapes_tag,shape_tag,
						points_tag,point_tag;
	char				path[1024],sub_path[1024];
	ag_shape_type		*shape;
	
		// null memory

	ag_shapes=NULL;

        // find the auto generate.xml file

	os_get_support_file_path(sub_path,"Editor");
	strcat(sub_path,"/Data");

	file_paths_app(&file_path_setup,path,sub_path,"Auto Generate","xml");

	if (!xml_open_file(path)) return(FALSE);
    
		// decode the file
      
    head_tag=xml_findrootchild("auto_generate");
    if (head_tag==-1) {
		xml_close_file();
		return(FALSE);
    }

		// shapes

	shapes_tag=xml_findfirstchild("shapes",head_tag);
	if (shapes_tag==-1) {
		xml_close_file();
		return(FALSE);
	}

	ag_nshape=xml_countchildren(shapes_tag);

	ag_shapes=(ag_shape_type*)malloc(ag_nshape*sizeof(ag_shape_type));
	if (ag_shapes==NULL) return(FALSE);

	shape=ag_shapes;
	shape_tag=xml_findfirstchild("shape",shapes_tag);

	for (n=0;n!=ag_nshape;n++) {
		xml_get_attribute_text(shape_tag,"name",shape->name,256);

		shape->npnt=0;

		points_tag=xml_findfirstchild("points",shape_tag);
		if (points_tag!=-1) {

			shape->npnt=xml_countchildren(points_tag);
			point_tag=xml_findfirstchild("point",points_tag);

			for (k=0;k!=shape->npnt;k++) {
				xml_get_attribute_2_coord_int(point_tag,"v",&shape->pnts[k].x,&shape->pnts[k].z);
				point_tag=xml_findnextchild(point_tag);
			}
		}

		shape++;
		shape_tag=xml_findnextchild(shape_tag);
	}
  
	xml_close_file();
	
	return(TRUE);
}

void ag_close_xml(void)
{
	if (ag_shapes!=NULL) free(ag_shapes);
	ag_shapes=NULL;
}

/* =======================================================

      ???
      
======================================================= */

void tester(void)
{
	int				n,k,mesh_idx,shape_sz,
					cx,cy,cz;
	int				px[4],py[4],pz[4];
	float			gx[4],gy[4];
	ag_shape_type	*shape;

	cx=map_max_size>>1;
	cz=map_max_size>>1;
	cy=10000;

	shape_sz=10000;

	shape=&ag_shapes[0];

	mesh_idx=map_mesh_add(&map);

	for (n=0;n!=shape->npnt;n++) {
		k=n+1;
		if (k==shape->npnt) k=0;

		px[0]=px[3]=((shape->pnts[n].x*shape_sz)/100)+cx;
		px[1]=px[2]=((shape->pnts[k].x*shape_sz)/100)+cx;
		pz[0]=pz[3]=((shape->pnts[n].z*shape_sz)/100)+cz;
		pz[1]=pz[2]=((shape->pnts[k].z*shape_sz)/100)+cz;
		py[0]=py[1]=cy;
		py[2]=py[3]=cy+2000;

		gx[0]=gx[3]=0.0f;
		gx[1]=gx[2]=1.0f;
		gy[0]=gy[1]=0.0f;
		gy[2]=gy[3]=1.0f;
	
		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,0);
	}

}

/* =======================================================

      Auto Generate Main Line
      
======================================================= */

void auto_generate_map_2(void)
{
		// read the auto generate XML

	if (!ag_read_xml()) {
		dialog_alert("Error","Unable to load auto generate XML file");
		return;
	}

		// clear map

	ag_map_clear();

	tester();






		// finish up, close the xml
		// structs, center views, and redraw

	ag_close_xml();

	walk_view_goto_map_center_all();
	main_wind_draw();
		
}

