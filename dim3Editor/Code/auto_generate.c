/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Auto Generation Main Line

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

extern map_type					map;

ag_state_type					ag_state;

extern bool ag_initialize(void);
extern void ag_release(void);

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

      Auto Generate Room Utilities
      
======================================================= */

bool ag_room_space_blocked(d3pnt *min,d3pnt *max)
{
	int				n;
	ag_room_type	*room;

	for (n=0;n!=ag_state.nroom;n++) {
		room=&ag_state.rooms[n];

		if (min->x>=room->max.x) continue;
		if (max->x<=room->min.x) continue;
		if (min->z>=room->max.z) continue;
		if (max->z<=room->min.z) continue;

		return(TRUE);
	}

	return(FALSE);
}

/* =======================================================

      Auto Generate Connection Utilities
      
======================================================= */

int ag_connector_get_opposite_type(ag_shape_connector_type *connector)
{
	switch (connector->type) {
		case ag_connector_type_min_x:
			return(ag_connector_type_max_x);
		case ag_connector_type_max_x:
			return(ag_connector_type_min_x);
		case ag_connector_type_min_z:
			return(ag_connector_type_max_z);
		case ag_connector_type_max_z:
			return(ag_connector_type_min_z);
	}

	return(ag_connector_type_min_x);
}

int ag_shape_has_connector_type(ag_room_type *room,int connect_type)
{
	int				n;
	ag_shape_type	*shape;

	shape=&ag_state.shapes[room->shape_idx];

	for (n=0;n!=shape->nconnector;n++) {

			// skip connectors that have been used

		if (room->connectors_used[n]) continue;

			// is this the correct type?

		if (shape->connectors[n].type==connect_type) return(n);
	}

	return(-1);
}

/* =======================================================

      Auto Generate Setup Rooms
      
======================================================= */

int ag_get_room_shape(bool skip_corridors)
{
	int			idx,count;

	count=ag_random_int(ag_state.nshape);

		// if any room, just return random room

	if (!skip_corridors) return(count);

		// otherwise skip all corridors

	idx=0;

	while (TRUE) {
		if (!ag_state.shapes[idx].corridor) {
			if (count==0) break;
			count--;
		}

		idx++;
		if (idx==ag_state.nshape) idx=0;
	}

	return(idx);
}

int ag_get_room_position(int shape_idx,d3pnt *pnt,d3vct *size)
{
	int						n,connect_idx,connect2_idx,nhit,opposite_cnt_type;
	float					dist_fact;
	bool					connect_hit[ag_max_shape_connector];
	d3pnt					min,max;
	d3vct					org_size;
	ag_shape_type			*shape,*shape2;
	ag_shape_connector_type	*connector,*connector2;
	ag_room_type			*room2;

		// run through and try each corridor

	for (n=0;n!=ag_max_shape_connector;n++) {
		connect_hit[n]=FALSE;
	}

		// start at a random corridor

	shape=&ag_state.shapes[shape_idx];
	connect_idx=ag_random_int(shape->nconnector);

		// find a place

	nhit=0;

	memmove(&org_size,size,sizeof(d3vct));

	while (TRUE) {

			// already hit?

		if (connect_hit[connect_idx]) {
			connect_idx++;
			if (connect_idx==shape->nconnector) connect_idx=0;
			continue;
		}

		connector=&shape->connectors[connect_idx];

			// get opposite type

		opposite_cnt_type=ag_connector_get_opposite_type(connector);

			// check other connectors

		for (n=0;n!=ag_state.nroom;n++) {

				// do we have the opposite type

			room2=&ag_state.rooms[n];
			connect2_idx=ag_shape_has_connector_type(room2,opposite_cnt_type);
			if (connect2_idx==-1) continue;

			shape2=&ag_state.shapes[room2->shape_idx];
			connector2=&shape2->connectors[connect2_idx];

				// if this new room is a corridor type,
				// then only connect to other rooms

			if ((shape->corridor) && (shape2->corridor)) continue;

				// get a size that makes the connectors
				// equal to each other

			dist_fact=(float)(connector2->dist)/(float)(connector->dist);

			if ((opposite_cnt_type==ag_connector_type_min_x) || (opposite_cnt_type==ag_connector_type_max_x)) {
				size->x=size->z=room2->size.z*dist_fact;
			}
			else {
				size->x=size->z=room2->size.x*dist_fact;
			}

				// get position

			switch (opposite_cnt_type) {
				case ag_connector_type_min_x:
					pnt->x=room2->min.x-(int)(100.0f*size->x);
					pnt->z=room2->min.z+((int)(((float)connector2->top_left.z)*room2->size.z)-(int)(((float)connector->top_left.z)*size->z));
					break;
				case ag_connector_type_max_x:
					pnt->x=room2->max.x;
					pnt->z=room2->min.z+((int)(((float)connector2->top_left.z)*room2->size.z)-(int)(((float)connector->top_left.z)*size->z));
					break;
				case ag_connector_type_min_z:
					pnt->x=room2->min.x+((int)(((float)connector2->top_left.x)*room2->size.x)-(int)(((float)connector->top_left.x)*size->x));
					pnt->z=room2->min.z-(int)(100.0f*size->z);
					break;
				case ag_connector_type_max_z:
					pnt->x=room2->min.x+((int)(((float)connector2->top_left.x)*room2->size.x)-(int)(((float)connector->top_left.x)*size->x));
					pnt->z=room2->max.z;
					break;
			}

				// is this position clear?

			min.x=pnt->x;
			min.z=pnt->z;
			max.x=min.x+(int)(100.0f*size->x);
			max.z=min.z+(int)(100.0f*size->z);

			if (ag_room_space_blocked(&min,&max)) continue;

				// mark room connector as used

			room2->connectors_used[connect2_idx]=TRUE;

			return(connect_idx);
		}

			// mark this connector as tried

		connect_hit[connect_idx]=TRUE;

			// have we tried all connectors?

		nhit++;
		if (nhit==shape->nconnector) return(-1);
	}

	return(0);
}

/* =======================================================

      Auto Generate Add Rooms
      
======================================================= */

void ag_add_room(int shape_idx,int connect_idx,d3pnt *pnt,d3vct *size)
{
	int					n,k,t,x,z,mesh_idx,poly_idx;
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

	room->shape_idx=shape_idx;
	room->mesh_idx=mesh_idx;

	memmove(&room->min,pnt,sizeof(d3pnt));
	room->max.x=room->min.x+(int)(100.0f*size->x);
	room->max.y=room->min.y+(int)(100.0f*size->y);
	room->max.z=room->min.z+(int)(100.0f*size->z);

	memmove(&room->size,size,sizeof(d3vct));

		// mark off the connection we are
		// using

	for (n=0;n!=ag_max_shape_connector;n++) {
		room->connectors_used[n]=FALSE;
	}

	if (connect_idx!=-1) room->connectors_used[connect_idx]=TRUE;

		// add walls

	shape=&ag_state.shapes[shape_idx];

	for (n=0;n!=shape->nvertex;n++) {
		k=n+1;
		if (k==shape->nvertex) k=0;

			// is this a connector?

		connect_idx=-1;

		for (t=0;t!=shape->nconnector;t++) {
			if ((shape->connectors[t].v[0]==n) && (shape->connectors[t].v[1]==k)) {
				connect_idx=t;
				break;
			}
			if ((shape->connectors[t].v[0]==k) && (shape->connectors[t].v[1]==n)) {
				connect_idx=t;
				break;
			}
		}

			// build the wall

		px[0]=px[3]=(int)(((float)shape->vertexes[n].x)*size->x)+pnt->x;
		px[1]=px[2]=(int)(((float)shape->vertexes[k].x)*size->x)+pnt->x;
		pz[0]=pz[3]=(int)(((float)shape->vertexes[n].z)*size->z)+pnt->z;
		pz[1]=pz[2]=(int)(((float)shape->vertexes[k].z)*size->z)+pnt->z;
		py[0]=py[1]=pnt->y-(int)(100.0f*size->y);
		py[2]=py[3]=pnt->y;

		gx[0]=gx[3]=0.0f;
		gx[1]=gx[2]=1.0f;
		gy[0]=gy[1]=0.0f;
		gy[2]=gy[3]=1.0f;
	
		poly_idx=map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,((connect_idx!=-1)?4:0));
	}

		// add floors

	shape_poly=shape->polys;

	for (n=0;n!=shape->npoly;n++) {

		for (k=0;k!=shape_poly->npt;k++) {
			x=shape->vertexes[shape_poly->v[k]].x;
			z=shape->vertexes[shape_poly->v[k]].z;

			px[k]=(int)(((float)x)*size->x)+pnt->x;
			pz[k]=(int)(((float)z)*size->z)+pnt->z;
			py[k]=pnt->y;

			gx[k]=((float)x)/100.0f;
			gy[k]=((float)z)/100.0f;
		}
	
		map_mesh_add_poly(&map,mesh_idx,shape_poly->npt,px,py,pz,gx,gy,1);

		shape_poly++;
	}
}

/* =======================================================

      Map Utilities
      
======================================================= */

bool ag_delete_shared_polygons_compare(map_mesh_type *mesh_1,int poly_1_idx,map_mesh_type *mesh_2,int poly_2_idx)
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

void ag_delete_shared_polygons(void)
{
	int					n,mesh_1_idx,poly_1_idx,
						mesh_2_idx,poly_2_idx;
	map_mesh_type		*mesh_1,*mesh_2;

	for (mesh_1_idx=0;mesh_1_idx!=map.mesh.nmesh;mesh_1_idx++) {

		mesh_1=&map.mesh.meshes[mesh_1_idx];

		poly_1_idx=0;

		while (poly_1_idx<mesh_1->npoly) {

			poly_2_idx=-1;

			for (mesh_2_idx=0;mesh_2_idx!=map.mesh.nmesh;mesh_2_idx++) {
				if (mesh_2_idx==mesh_1_idx) continue;

				mesh_2=&map.mesh.meshes[mesh_2_idx];

				for (n=0;n!=mesh_2->npoly;n++) {
					if (ag_delete_shared_polygons_compare(mesh_1,poly_1_idx,mesh_2,n)) {
						poly_2_idx=n;
						break;
					}
				}

				if (poly_2_idx!=-1) break;
			}

			if (poly_2_idx!=-1) {
				map_mesh_delete_poly(&map,mesh_1_idx,poly_1_idx);
				map_mesh_delete_poly(&map,mesh_2_idx,poly_2_idx);
				continue;
			}

			poly_1_idx++;
		}
	}
}

/* =======================================================

      Generate Map
      
======================================================= */

void ag_generate_map(void)
{
	int				n,shape_idx,connect_idx;
	d3pnt			pnt;
	d3vct			size;

		// create the rooms

	for (n=0;n!=20;n++) {
			
			// the default size

		size.x=200.0f;
		size.y=50.0f;
		size.z=200.0f;

			// if this is the first room, pick a non-corridor
			// type room and put it at the center of the map.
			// give it the default size, all other rooms will
			// gain their size from connecting to this room

		if (n==0) {
			shape_idx=ag_get_room_shape(TRUE);

			pnt.x=map_max_size>>1;
			pnt.z=map_max_size>>1;
			pnt.y=10000;
			
			connect_idx=-1;
		}

			// otherwise, pick any room and randomly
			// connect it to an existing room

		else {
			shape_idx=ag_get_room_shape(FALSE);

				// get the connector

			connect_idx=ag_get_room_position(shape_idx,&pnt,&size);
			if (connect_idx==-1) continue;
		}

			// add the room

		ag_add_room(shape_idx,connect_idx,&pnt,&size);
	}

		// delete any polygons that share the
		// same space

	ag_delete_shared_polygons();
}

/* =======================================================

      Auto Generate Main Line
      
======================================================= */

bool auto_generate_map_2(void)
{
	if (!ag_initialize()) return(FALSE);

//	ag_random_seed(GetTickCount());		// supergumba -- testing
	ag_random_seed(5);		// supergumba -- testing

		// clear map

	ag_map_clear();

	ag_generate_map();

		// finish up, center views, and redraw

	ag_release();

	walk_view_goto_map_center_all();
	main_wind_draw();
		
	return(TRUE);
}

