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

#define ag_max_room					256
#define ag_max_shape				128
#define ag_max_shape_point			64
#define ag_max_shape_connector		32
#define ag_max_shape_poly			64

#define ag_connector_type_horz		0
#define ag_connector_type_vert		1

typedef struct		{
						int						npt,v[4];
					} ag_shape_poly_type;

typedef struct		{
						int						type,v[2];
					} ag_shape_connector_type;

typedef struct		{
						int						nvertex,npoly,nconnector;
						char					name[name_str_len];
						d3pnt					vertexes[ag_max_shape_point];
						ag_shape_poly_type		polys[ag_max_shape_poly];
						ag_shape_connector_type	connectors[ag_max_shape_connector];
					} ag_shape_type;

typedef struct		{
						int						mesh_idx;
						d3pnt					min,max;
					} ag_room_type;

typedef struct		{
						int						nshape,nroom;
						ag_shape_type			*shapes;
						ag_room_type			*rooms;
					} ag_state_type;

extern map_type					map;
extern file_path_setup_type		file_path_setup;

ag_state_type					ag_state;

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

void ag_random_seed(int seed)
{
	srandom(seed);
}

int ag_random_int(int max)
{
	float			f;

	f=(float)random()/(float)RAND_MAX;
	return((int)(f*(float)max));
}

/* =======================================================

      Auto Generate Settings
      
======================================================= */

bool ag_read_settings(void)
{
	int					n,k,head_tag,shapes_tag,shape_tag,
						vertexes_tag,vertex_tag,
						polys_tag,poly_tag,
						connectors_tag,connector_tag;
	char				path[1024],sub_path[1024];
	ag_shape_type		*shape;
	
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

	ag_state.nshape=xml_countchildren(shapes_tag);

	shape=ag_state.shapes;
	shape_tag=xml_findfirstchild("shape",shapes_tag);

	for (n=0;n!=ag_state.nshape;n++) {
		xml_get_attribute_text(shape_tag,"name",shape->name,256);

			// shape vertexes

		shape->nvertex=0;

		vertexes_tag=xml_findfirstchild("vertexes",shape_tag);
		if (vertexes_tag!=-1) {

			shape->nvertex=xml_countchildren(vertexes_tag);
			vertex_tag=xml_findfirstchild("vertex",vertexes_tag);

			for (k=0;k!=shape->nvertex;k++) {
				xml_get_attribute_2_coord_int(vertex_tag,"c2",&shape->vertexes[k].x,&shape->vertexes[k].z);
				vertex_tag=xml_findnextchild(vertex_tag);
			}
		}

			// shape polys

		shape->npoly=0;

		polys_tag=xml_findfirstchild("polys",shape_tag);
		if (polys_tag!=-1) {

			shape->npoly=xml_countchildren(polys_tag);
			poly_tag=xml_findfirstchild("poly",polys_tag);

			for (k=0;k!=shape->npoly;k++) {
				shape->polys[k].npt=xml_get_attribute_int_array(poly_tag,"v",shape->polys[k].v,4);
				poly_tag=xml_findnextchild(poly_tag);
			}
		}

			// shape connectors

		shape->nconnector=0;

		connectors_tag=xml_findfirstchild("connectors",shape_tag);
		if (connectors_tag!=-1) {

			shape->nconnector=xml_countchildren(connectors_tag);
			connector_tag=xml_findfirstchild("connector",connectors_tag);

			for (k=0;k!=shape->nconnector;k++) {
				xml_get_attribute_int_array(connector_tag,"v",shape->connectors[k].v,2);
				shape->connectors[k].type=(shape->vertexes[shape->connectors[k].v[0]].x==shape->vertexes[shape->connectors[k].v[1]].x)?ag_connector_type_horz:ag_connector_type_vert;
				connector_tag=xml_findnextchild(connector_tag);
			}
		}

			// next shape

		shape++;
		shape_tag=xml_findnextchild(shape_tag);
	}
  
	xml_close_file();
	
	return(TRUE);
}

/* =======================================================

      Auto Generate Memory
      
======================================================= */

bool ag_initialize(void)
{
		// memory

	ag_state.nroom=0;
	ag_state.rooms=(ag_room_type*)malloc(ag_max_room*sizeof(ag_room_type));
	if (ag_state.rooms==NULL) return(FALSE);

	ag_state.nshape=0;
	ag_state.shapes=(ag_shape_type*)malloc(ag_max_shape*sizeof(ag_shape_type));
	if (ag_state.shapes==NULL) {
		free(ag_state.rooms);
		ag_state.rooms=NULL;
		return(FALSE);
	}

		// read settings file

	if (!ag_read_settings()) {
		free(ag_state.rooms);
		ag_state.rooms=NULL;
		free(ag_state.shapes);
		ag_state.shapes=NULL;
		return(FALSE);
	}

	return(TRUE);
}

void ag_release(void)
{
	if (ag_state.rooms!=NULL) free(ag_state.rooms);
	ag_state.rooms=NULL;

	if (ag_state.shapes!=NULL) free(ag_state.shapes);
	ag_state.shapes=NULL;
}

/* =======================================================

      Auto Generate Rooms
      
======================================================= */

void ag_add_room(int shape_idx,d3pnt *pnt,d3pnt *size)
{
	int					n,k,t,x,z,mesh_idx,txt_idx;
	int					px[4],py[4],pz[4];
	float				gx[4],gy[4];
	ag_shape_type		*shape;
	ag_shape_poly_type	*shape_poly;
	ag_room_type		*room;

	if (ag_state.nroom==ag_max_room) return;

		// add shape mesh to map

	mesh_idx=map_mesh_add(&map);

		// add to room

	room=&ag_state.rooms[ag_state.nroom];
	ag_state.nroom++;

	room->mesh_idx=mesh_idx;

	memmove(&room->min,pnt,sizeof(d3pnt));
	room->max.x=room->min.x+size->x;
	room->max.y=room->min.y+size->y;
	room->max.z=room->min.z+size->z;

		// add walls

	shape=&ag_state.shapes[shape_idx];

	for (n=0;n!=shape->nvertex;n++) {
		k=n+1;
		if (k==shape->nvertex) k=0;

			// supergumba -- temp, check if connector

		txt_idx=0;

		for (t=0;t!=shape->nconnector;t++) {
			if ((shape->connectors[t].v[0]==n) && (shape->connectors[t].v[1]==k)) {
				txt_idx=4;
				break;
			}
			if ((shape->connectors[t].v[0]==k) && (shape->connectors[t].v[1]==n)) {
				txt_idx=4;
				break;
			}
		}


		px[0]=px[3]=((shape->vertexes[n].x*size->x)/100)+pnt->x;
		px[1]=px[2]=((shape->vertexes[k].x*size->x)/100)+pnt->x;
		pz[0]=pz[3]=((shape->vertexes[n].z*size->z)/100)+pnt->z;
		pz[1]=pz[2]=((shape->vertexes[k].z*size->z)/100)+pnt->z;
		py[0]=py[1]=pnt->y-size->y;
		py[2]=py[3]=pnt->y;

		gx[0]=gx[3]=0.0f;
		gx[1]=gx[2]=1.0f;
		gy[0]=gy[1]=0.0f;
		gy[2]=gy[3]=1.0f;
	
		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,txt_idx);
	}

		// add floors

	shape_poly=shape->polys;

	for (n=0;n!=shape->npoly;n++) {

		for (k=0;k!=shape_poly->npt;k++) {
			x=shape->vertexes[shape_poly->v[k]].x;
			z=shape->vertexes[shape_poly->v[k]].z;

			px[k]=((x*size->x)/100)+pnt->x;
			pz[k]=((z*size->z)/100)+pnt->z;
			py[k]=pnt->y;

			gx[k]=((float)x)/100.0f;
			gy[k]=((float)z)/100.0f;
		}
	
		map_mesh_add_poly(&map,mesh_idx,shape_poly->npt,px,py,pz,gx,gy,1);

		shape_poly++;
	}
}

void ag_find_room_position(d3pnt *pnt,d3pnt *size)
{
	int					n,temp;
	bool				hit;
	d3pnt				mv_pt,min,max;
	ag_room_type		*room;

		// move randomly away from center
		// until we are clear of other rooms

	angle_get_movement((float)ag_random_int(359),map_enlarge,&mv_pt.x,&mv_pt.z);


	temp=0;

	while (TRUE) {
			
			// size of this room

		memmove(&min,pnt,sizeof(d3pnt));
		max.x=min.x+size->x;
		max.y=min.y+size->y;
		max.z=min.z+size->z;

			// any collisions?

		hit=FALSE;

		for (n=0;n!=ag_state.nroom;n++) {
			room=&ag_state.rooms[n];

			if ((min.x>=room->max.x) || (max.x<=room->min.x)) continue;
			if ((min.z>=room->max.z) || (max.z<=room->min.z)) continue;
		
			hit=TRUE;
			break;
		}

		if (!hit) break;
		temp++;
		if (temp==1000) break;	// supergumba -- for testing

		pnt->x+=mv_pt.x;
		pnt->z+=mv_pt.z;
	}
}

/* =======================================================

      ???
      
======================================================= */


void tester(void)
{
	int				n,shape_idx,
					cx,cy,cz;
	d3pnt			pnt,size;

	cx=map_max_size>>1;
	cz=map_max_size>>1;
	cy=10000;

	for (n=0;n!=20;n++) {
		shape_idx=ag_random_int(ag_state.nshape);

		pnt.x=cx;
		pnt.z=cz;
		pnt.y=10000;

		size.x=2000+ag_random_int(3000);
		size.y=2000+ag_random_int(3000);
		size.z=2000+ag_random_int(3000);

		ag_find_room_position(&pnt,&size);

		ag_add_room(shape_idx,&pnt,&size);
	}

}

/* =======================================================

      Auto Generate Main Line
      
======================================================= */

bool auto_generate_map_2(void)
{
	if (!ag_initialize()) return(FALSE);

	ag_random_seed(5);		// supergumba -- testing

		// clear map

	ag_map_clear();

	tester();






		// finish up, center views, and redraw

	ag_release();

	walk_view_goto_map_center_all();
	main_wind_draw();
		
	return(TRUE);
}

