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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "interface.h"

extern map_type					map;

ag_state_type					ag_state;

extern bool ag_initialize(void);
extern void ag_release(void);
extern int ag_shape_find(char *name);
extern int ag_shape_get_connector_index(ag_shape_type *shape,int v1_idx,int v2_idx);
extern void ag_random_seed(int seed);
extern int ag_random_int(int max);
extern void ag_generate_mirror_meshes(void);
extern bool ag_generate_is_poly_straight_wall(int mesh_idx,int poly_idx);
extern void ag_generate_delete_shared_polygons(void);
extern void ag_generate_spots_add(void);
extern void ag_generate_additional_stories(void);

/* =======================================================

      Auto Generate Utilities
      
======================================================= */

void ag_map_clear(void)
{
	int				n;

		// clear static memory

	map.nspot=0;
	map.nnode=0;
	map.nscenery=0;
	map.nlight=0;
	map.nsound=0;
	map.nparticle=0;

		// meshes and liquids

	if (map.mesh.meshes!=NULL) {
		for (n=0;n!=map.mesh.nmesh;n++) {
			if (map.mesh.meshes[n].polys!=NULL) free(map.mesh.meshes[n].polys);
		}

		free(map.mesh.meshes);
		map.mesh.nmesh=0;
		map.mesh.meshes=NULL;
	}

	if (map.liquid.liquids!=NULL) {
		free(map.liquid.liquids);
		map.liquid.nliquid=0;
		map.liquid.liquids=NULL;
	}
	
		// groups, movements, cinemas

	if (map.group.groups!=NULL) {
		free(map.group.groups);
		map.group.ngroup=0;
		map.group.groups=NULL;
	}

	if (map.movement.movements!=NULL) {
		for (n=0;n!=map.movement.nmovement;n++) {
			if (map.movement.movements[n].moves!=NULL) free(map.movement.movements[n].moves);
		}

		free(map.movement.movements);
		map.movement.nmovement=0;
		map.movement.movements=NULL;
	}
		
	if (map.cinema.cinemas!=NULL) {
		for (n=0;n!=map.cinema.ncinema;n++) {
			if (map.cinema.cinemas[n].actions!=NULL) free(map.cinema.cinemas[n].actions);
		}

		free(map.cinema.cinemas);
		map.cinema.ncinema=0;
		map.cinema.cinemas=NULL;
	}
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

bool ag_room_mirror_blocked(d3pnt *min,d3pnt *max)
{
	ag_room_type	*room;

		// mirrors can't be below
		// or to the right of original room

	room=&ag_state.rooms[0];

	if (min->x<room->min.x) return(TRUE);
	return(max->z>room->max.z);
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

int ag_get_room_shape(int style_idx,bool skip_corridors)
{
	int				idx,count;
	ag_style_type	*style;

	style=&ag_state.styles[style_idx];

	count=ag_random_int(style->nshape);

		// if any room, just return random room

	if (!skip_corridors) return(style->shape_list[count]);

		// otherwise skip all corridors

	idx=0;

	while (TRUE) {
		if (!ag_state.shapes[style->shape_list[idx]].corridor) {
			if (count==0) break;
			count--;
		}

		idx++;
		if (idx==style->nshape) idx=0;
	}

	return(style->shape_list[idx]);
}

int ag_get_room_position(int shape_idx,d3pnt *pnt,d3vct *size,bool mirror)
{
	int						n,connect_idx,connect2_idx,nhit,opposite_cnt_type;
	float					dist_fact;
	bool					connect_hit[ag_max_shape_connector];
	d3pnt					min,max;
	d3vct					org_size;
	ag_shape_type			*shape,*shape2;
	ag_shape_connector_type	*connector,*connector2;
	ag_room_type			*room2;

	shape=&ag_state.shapes[shape_idx];

		// run through and try each corridor

	for (n=0;n!=ag_max_shape_connector;n++) {
		connect_hit[n]=FALSE;
	}

		// if it's a mirror, then only build
		// on left-bottom connectors

	nhit=0;

	if (mirror) {

		for (n=0;n!=shape->nconnector;n++) {
			if ((shape->connectors[n].type==ag_connector_type_max_x) || (shape->connectors[n].type==ag_connector_type_min_z)) {
				connect_hit[n]=TRUE;
				nhit++;
			}
		}

		if (nhit==0) return(-1);
	}

		// start at a random corridor

	connect_idx=ag_random_int(shape->nconnector);

		// find a place

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

			if ((shape->corridor) && (room2->corridor)) continue;

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

				// if it's a mirror, it can never be
				// past the left or bottom or original room

			if (mirror) {
				if (ag_room_mirror_blocked(&min,&max)) continue;
			}

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

	return(-1);
}

/* =======================================================

      Auto Generate Add Rooms
      
======================================================= */

void ag_add_room(int shape_idx,int connect_idx,d3pnt *pnt,d3vct *size)
{
	int					n,k,x,z,mesh_idx;
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

	shape=&ag_state.shapes[shape_idx];

	room->shape_idx=shape_idx;
	room->corridor=shape->corridor;
	room->mesh_idx=mesh_idx;

	room->multi_story=FALSE;

	room->min.x=pnt->x;
	room->max.x=pnt->x+(int)(100.0f*size->x);

	room->min.y=pnt->y-(int)(100.0f*size->y);
	room->max.y=pnt->y;

	room->min.z=pnt->z;
	room->max.z=pnt->z+(int)(100.0f*size->z);

	memmove(&room->size,size,sizeof(d3vct));

		// mark off the connection we are
		// using

	for (n=0;n!=ag_max_shape_connector;n++) {
		room->connectors_used[n]=FALSE;
	}

	if (connect_idx!=-1) room->connectors_used[connect_idx]=TRUE;

		// add walls

	for (n=0;n!=shape->nvertex;n++) {
		k=n+1;
		if (k==shape->nvertex) k=0;

			// is this a connector?

		connect_idx=ag_shape_get_connector_index(shape,n,k);

			// build the wall

		px[0]=px[3]=(int)(((float)shape->vertexes[n].x)*size->x)+pnt->x;
		px[1]=px[2]=(int)(((float)shape->vertexes[k].x)*size->x)+pnt->x;
		pz[0]=pz[3]=(int)(((float)shape->vertexes[n].z)*size->z)+pnt->z;
		pz[1]=pz[2]=(int)(((float)shape->vertexes[k].z)*size->z)+pnt->z;
		py[0]=py[1]=room->min.y;
		py[2]=py[3]=room->max.y;

		gx[0]=gx[3]=0.0f;
		gx[1]=gx[2]=1.0f;
		gy[0]=gy[1]=0.0f;
		gy[2]=gy[3]=1.0f;
	
		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,((connect_idx!=-1)?ag_texture_wall_temp:ag_texture_wall));
	}

		// add floors and ceilings

	shape_poly=shape->polys;

	for (n=0;n!=shape->npoly;n++) {

		for (k=0;k!=shape_poly->npt;k++) {
			x=shape->vertexes[shape_poly->v[k]].x;
			z=shape->vertexes[shape_poly->v[k]].z;

			px[k]=(int)(((float)x)*size->x)+pnt->x;
			pz[k]=(int)(((float)z)*size->z)+pnt->z;
			py[k]=room->max.y;

			gx[k]=((float)x)/100.0f;
			gy[k]=((float)z)/100.0f;
		}
	
		map_mesh_add_poly(&map,mesh_idx,shape_poly->npt,px,py,pz,gx,gy,ag_texture_floor);

		if (room->corridor) {
			for (k=0;k!=shape_poly->npt;k++) {
				py[k]=room->min.y;
			}

			map_mesh_add_poly(&map,mesh_idx,shape_poly->npt,px,py,pz,gx,gy,ag_texture_ceiling);
		}

		shape_poly++;
	}
}

void ag_add_extra_corridor_room(d3pnt *min,d3pnt *max,int wall_sz)
{
	int					n,lx,rx,tz,bz,mesh_idx,poly_idx;
	int					px[4],py[4],pz[4];
	float				gx[4],gy[4];
	ag_room_type		*room;

	if (ag_state.nroom==ag_max_room) return;

		// add shape mesh to map

	mesh_idx=map_mesh_add(&map);

		// add to room

	room=&ag_state.rooms[ag_state.nroom];
	ag_state.nroom++;

	room->shape_idx=0;
	room->corridor=TRUE;
	room->mesh_idx=mesh_idx;

	memmove(&room->min,min,sizeof(d3pnt));
	memmove(&room->max,max,sizeof(d3pnt));

	room->size.x=room->size.y=room->size.z=0.0f;

		// connections are ignored here

	for (n=0;n!=ag_max_shape_connector;n++) {
		room->connectors_used[n]=FALSE;
	}

		// add walls

	lx=min->x;

	while (TRUE) {

		rx=lx+wall_sz;
		if (rx>max->x) rx=max->x;

		px[0]=px[3]=lx;
		px[1]=px[2]=rx;
		pz[0]=pz[1]=pz[2]=pz[3]=min->z;
		py[0]=py[1]=min->y;
		py[2]=py[3]=max->y;

		gx[0]=gx[3]=0.0f;
		gx[1]=gx[2]=1.0f;
		gy[0]=gy[1]=0.0f;
		gy[2]=gy[3]=1.0f;
	
		poly_idx=map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_wall);

		px[0]=px[3]=lx;
		px[1]=px[2]=rx;
		pz[0]=pz[1]=pz[2]=pz[3]=max->z;
		py[0]=py[1]=min->y;
		py[2]=py[3]=max->y;

		gx[0]=gx[3]=0.0f;
		gx[1]=gx[2]=1.0f;
		gy[0]=gy[1]=0.0f;
		gy[2]=gy[3]=1.0f;
	
		poly_idx=map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_wall);

		lx=rx;
		if (lx>=max->x) break;
	}

	tz=min->z;

	while (TRUE) {

		bz=tz+wall_sz;
		if (bz>max->z) bz=max->z;

		px[0]=px[1]=px[2]=px[3]=min->x;
		pz[0]=pz[3]=tz;
		pz[1]=pz[2]=bz;
		py[0]=py[1]=min->y;
		py[2]=py[3]=max->y;

		gx[0]=gx[3]=0.0f;
		gx[1]=gx[2]=1.0f;
		gy[0]=gy[1]=0.0f;
		gy[2]=gy[3]=1.0f;
	
		poly_idx=map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_wall);

		px[0]=px[1]=px[2]=px[3]=max->x;
		pz[0]=pz[3]=tz;
		pz[1]=pz[2]=bz;
		py[0]=py[1]=min->y;
		py[2]=py[3]=max->y;

		gx[0]=gx[3]=0.0f;
		gx[1]=gx[2]=1.0f;
		gy[0]=gy[1]=0.0f;
		gy[2]=gy[3]=1.0f;
	
		poly_idx=map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_wall);

		tz=bz;
		if (tz>=max->z) break;
	}

		// add floors and ceilings

	lx=min->x;
	tz=min->z;

	while (TRUE) {

		rx=lx+wall_sz;
		bz=tz+wall_sz;

		px[0]=px[3]=lx;
		px[1]=px[2]=rx;
		pz[0]=pz[1]=tz;
		pz[2]=pz[3]=bz;

		gx[0]=gx[3]=0.0f;
		gx[1]=gx[2]=1.0f;
		gy[0]=gy[1]=0.0f;
		gy[2]=gy[3]=1.0f;
	
		py[0]=py[1]=py[2]=py[3]=max->y;
		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_floor);

		py[0]=py[1]=py[2]=py[3]=min->y;
		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_ceiling);

		lx=rx;
		if (lx>=max->x) {
			lx=min->x;

			tz=bz;
			if (tz>=max->z) break;
		}
	}
}

/* =======================================================

      Corridor Clipping
      
======================================================= */

void ag_generate_clip_bad_corridors(void)
{
	int				n,k,hit,mesh_idx,sz;
	ag_room_type	*room,*chk_room;

		// find all the rooms to clip

	for (n=0;n!=ag_state.nroom;n++) {

			// check corridors

		room=&ag_state.rooms[n];
		room->clip=FALSE;

		if (!room->corridor) continue;

			// check for used connectors

		hit=0;

		for (k=0;k!=ag_max_shape_connector;k++) {
			if (room->connectors_used[k]) hit++;
		}

		room->clip=(hit<2);
	}

		// clip them

	n=0;

	while (n<ag_state.nroom) {
		if (!ag_state.rooms[n].clip) {
			n++;
			continue;
		}

			// delete the mesh

		mesh_idx=ag_state.rooms[n].mesh_idx;
		map_mesh_delete(&map,mesh_idx);

			// fix other indexes

		for (k=0;k!=ag_state.nroom;k++) {
			if (k==n) continue;

			chk_room=&ag_state.rooms[k];
			if (chk_room->mesh_idx>mesh_idx) chk_room->mesh_idx--;
		}

			// remove the room

		sz=((ag_state.nroom-n)-1)*sizeof(ag_room_type);
		if (sz>0) memmove(&ag_state.rooms[n],&ag_state.rooms[n+1],sz);

		ag_state.nroom--;
	}
}

/* =======================================================

      Add Extra Corridors
      
======================================================= */

void ag_generate_extra_corridors(void)
{
	int					n,k,i,t,nroom,npoly,nchk_poly;
	d3pnt				min,max;
	ag_room_type		*room,*chk_room;
	map_mesh_type		*mesh,*chk_mesh;
	map_mesh_poly_type	*poly,*chk_poly;

		// we need min max, so prepare the polys

	for (n=0;n!=ag_state.nroom;n++) {

		room=&ag_state.rooms[n];
		mesh=&map.mesh.meshes[room->mesh_idx];

		poly=mesh->polys;

		for (k=0;k!=mesh->npoly;k++) {
			map_prepare_mesh_poly(mesh,poly);
			poly++;
		}
	}

		// only look at original rooms

	nroom=ag_state.nroom;

		// check rooms polys against each other
		// we only compare wall like polygons that are straight
		// vertical or horizontal

	for (n=0;n!=nroom;n++) {

		room=&ag_state.rooms[n];
		npoly=map.mesh.meshes[room->mesh_idx].npoly;

		for (k=0;k!=npoly;k++) {

			if (!ag_generate_is_poly_straight_wall(room->mesh_idx,k)) continue;

			for (i=0;i!=nroom;i++) {
				if (i==n) continue;

				chk_room=&ag_state.rooms[i];
				nchk_poly=map.mesh.meshes[chk_room->mesh_idx].npoly;

				for (t=0;t!=nchk_poly;t++) {
					
					if (!ag_generate_is_poly_straight_wall(chk_room->mesh_idx,t)) continue;

						// have to always get the pointers as adding
						// a mesh can move memory around

					mesh=&map.mesh.meshes[room->mesh_idx];
					poly=&mesh->polys[k];

					chk_mesh=&map.mesh.meshes[chk_room->mesh_idx];
					chk_poly=&chk_mesh->polys[t];

						// horizontal

					if (poly->box.min.z!=poly->box.max.z) {
						if ((poly->box.min.z==chk_poly->box.min.z) && (poly->box.max.z==chk_poly->box.max.z)) {
							if (poly->box.max.x!=chk_poly->box.min.x) {

								min.z=poly->box.min.z;
								max.z=poly->box.max.z;

								if (poly->box.max.x<chk_poly->box.min.x) {
									min.x=poly->box.max.x;
									max.x=chk_poly->box.min.x;
								}
								else {
									min.x=chk_poly->box.max.x;
									max.x=poly->box.min.x;
								}

								if (!ag_room_space_blocked(&min,&max)) {
									min.y=room->min.y;
									max.y=room->max.y;
									ag_add_extra_corridor_room(&min,&max,(max.z-min.z));
									continue;
								}
							}
						}
					}

						// vertical

					if (poly->box.min.x!=poly->box.max.x) {
						if ((poly->box.min.x==chk_poly->box.min.x) && (poly->box.max.x==chk_poly->box.max.x)) {
							if (poly->box.max.z!=chk_poly->box.min.z) {

								min.x=poly->box.min.x;
								max.x=poly->box.max.x;

								if (poly->box.max.z<chk_poly->box.min.z) {
									min.z=poly->box.max.z;
									max.z=chk_poly->box.min.z;
								}
								else {
									min.z=chk_poly->box.max.z;
									max.z=poly->box.min.z;
								}

								if (!ag_room_space_blocked(&min,&max)) {
									min.y=room->min.y;
									max.y=room->max.y;
									ag_add_extra_corridor_room(&min,&max,(max.x-min.x));
									continue;
								}
							}
						}
					}
				}
			}
		}
	}
}

/* =======================================================

      Generate Map
      
======================================================= */

bool ag_generate_map(ag_build_setup_type *build_setup)
{
	int				n,room_count,try_count,
					shape_idx,connect_idx;
	d3pnt			pnt;
	d3vct			size;

		// initialize auto generate structures

	if (!ag_initialize()) {
		ag_release();
		return(FALSE);
	}

	ag_random_seed(build_setup->seed);

		// clear map

	ag_map_clear();

		// cut room count in half if mirroring
		// but add one extra for center room

	room_count=build_setup->room_count;
	if (build_setup->mirror) room_count=(room_count>>1)+1;

		// create the rooms

	for (n=0;n!=room_count;n++) {
			
			// the default size

		size.x=size.z=(float)build_setup->room_sz;
		size.y=(float)build_setup->room_high;

			// if this is the first room, pick a non-corridor
			// type room and put it at the center of the map.
			// give it the default size, all other rooms will
			// gain their size from connecting to this room

		if (n==0) {
			shape_idx=ag_get_room_shape(build_setup->style_idx,TRUE);

			pnt.x=map_max_size>>1;
			pnt.z=map_max_size>>1;
			pnt.y=10000;
			
			connect_idx=-1;
		}

			// otherwise, pick any room and randomly
			// connect it to an existing room
			// might need to try a couple times to find
			// a proper fit

		else {

			try_count=0;

			while (try_count!=4) {
				shape_idx=ag_get_room_shape(build_setup->style_idx,FALSE);

					// get the connector

				connect_idx=ag_get_room_position(shape_idx,&pnt,&size,build_setup->mirror);
				if (connect_idx!=-1) break;

					// try again

				try_count++;
			}

			if (connect_idx==-1) break;
		}

			// add the room

		ag_add_room(shape_idx,connect_idx,&pnt,&size);
	}

		// remove corridors that aren't
		// connected to at least two rooms

	ag_generate_clip_bad_corridors();

		// add corridors where two
		// polygons are equal and facing each
		// other and not blocked

	for (n=0;n!=build_setup->merge_count;n++) {
		ag_generate_extra_corridors();
	}

		// add additional stories for rooms
		// that are connected to other rooms

	for (n=0;n!=build_setup->story_count;n++) {
		ag_generate_additional_stories();
	}

		// mirroring

	if (build_setup->mirror) ag_generate_mirror_meshes();

		// delete any polygons that share the
		// same space

	ag_generate_delete_shared_polygons();

		// add spots and nodes

	ag_generate_spots_add();

		// free auto generate structures

	ag_release();

		// center view, reset UVs
		// and redraw

	view_goto_map_center_all();
	map_mesh_reset_uv_all();

	main_wind_draw();

	return(TRUE);
}
