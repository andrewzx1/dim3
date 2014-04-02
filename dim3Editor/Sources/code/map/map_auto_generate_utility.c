/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Auto Generation Utility

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
extern file_path_setup_type		file_path_setup;

extern ag_state_type			ag_state;

/* =======================================================

      Random Utilities
      
======================================================= */

void ag_random_seed(void)
{
#ifdef D3_OS_WINDOWS
	ag_state.seed=GetTickCount();
#else
	ag_state.seed=TickCount();
#endif

	srandom(ag_state.seed);
}

void ag_random_previous_seed(void)
{
	ag_state.seed--;
	srandom(ag_state.seed);
}

void ag_random_next_seed(void)
{
	ag_state.seed++;
	srandom(ag_state.seed);
}

int ag_random_int(int max)
{
	float			f;

	f=(float)random()/(float)RAND_MAX;
	return((int)(f*(float)max));
}

bool ag_random_bool(void)
{
	return(ag_random_int(100)>=50);
}

/* =======================================================

      Delete Shared Polygons
      
======================================================= */

bool ag_generate_delete_shared_polygons_compare(map_mesh_type *mesh_1,int poly_1_idx,map_mesh_type *mesh_2,int poly_2_idx)
{
	int					n,k;
	bool				hit;
	d3pnt				*pt_1,*pt_2;
	map_mesh_poly_type	*poly_1,*poly_2;

	poly_1=&mesh_1->polys[poly_1_idx];
	poly_2=&mesh_2->polys[poly_2_idx];

		// same vertex count?

	if (poly_1->ptsz!=poly_2->ptsz) return(FALSE);

		// ignore ceilings or floors

	map_prepare_mesh_poly(&map,mesh_1,poly_1);
	if (poly_1->box.flat) return(FALSE);

	map_prepare_mesh_poly(&map,mesh_2,poly_2);
	if (poly_2->box.flat) return(FALSE);

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

void ag_generate_delete_shared_polygons(void)
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
					if (ag_generate_delete_shared_polygons_compare(mesh_1,poly_1_idx,mesh_2,n)) {
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

      Collisions
      
======================================================= */

bool ag_generate_mesh_collision(d3pnt *min,d3pnt *max,int start_cmp_mesh_idx,int end_cmp_mesh_idx)
{
	int				n;
	d3pnt			cmp_min,cmp_max;

	for (n=start_cmp_mesh_idx;n<end_cmp_mesh_idx;n++) {

		map_mesh_calculate_extent(&map,n,&cmp_min,&cmp_max);
		if ((min->x>=cmp_max.x) || (max->x<=cmp_min.x)) continue;
		if ((min->y>=cmp_max.y) || (max->y<=cmp_min.y)) continue;
		if ((min->z>=cmp_max.z) || (max->z<=cmp_min.z)) continue;
		
		return(TRUE);
	}

	return(FALSE);
}

/* =======================================================

      Decorations
      
======================================================= */

void ag_generate_decoration_box(d3pnt *pnt,int start_cmp_mesh_idx)
{
	int				n,mesh_idx,x,z,sz;
	int				px[8],py[8],pz[8];
	float			gx[8],gy[8];
	bool			hit;
	d3pnt			mpt,min,max;
	d3ang			ang;

		// find a random start position
	
	hit=FALSE;
	sz=ag_size_stack_box>>1;

	for (n=0;n!=10;n++) {
		x=pnt->x+(ag_random_int(4000)-8000);
		z=pnt->z+(ag_random_int(4000)-8000);

		min.x=x-sz;
		max.x=x+sz;
		min.y=pnt->y-ag_size_stack_box;
		max.y=pnt->y;
		min.z=z-sz;
		max.z=z+sz;

		if (!ag_generate_mesh_collision(&min,&max,start_cmp_mesh_idx,map.mesh.nmesh)) {
			hit=TRUE;
			break;
		}
	}

	if (!hit) return;

		// box mesh

	mesh_idx=map_mesh_add(&map);

	map.mesh.meshes[mesh_idx].flag.lock_uv=TRUE;

		// sides

	px[0]=px[3]=min.x;
	px[1]=px[2]=min.x;
	pz[0]=pz[3]=min.z;
	pz[1]=pz[2]=max.z;
	py[0]=py[1]=min.y;
	py[2]=py[3]=max.y;

	gx[0]=gx[3]=0.0f;
	gx[1]=gx[2]=1.0f;
	gy[0]=gy[1]=0.0f;
	gy[2]=gy[3]=1.0f;

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_decoration);

	px[0]=px[3]=max.x;
	px[1]=px[2]=max.x;
	pz[0]=pz[3]=min.z;
	pz[1]=pz[2]=max.z;
	py[0]=py[1]=min.y;
	py[2]=py[3]=max.y;

	gx[0]=gx[3]=0.0f;
	gx[1]=gx[2]=1.0f;
	gy[0]=gy[1]=0.0f;
	gy[2]=gy[3]=1.0f;

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_decoration);

	px[0]=px[3]=min.x;
	px[1]=px[2]=max.x;
	pz[0]=pz[3]=min.z;
	pz[1]=pz[2]=min.z;
	py[0]=py[1]=min.y;
	py[2]=py[3]=max.y;

	gx[0]=gx[3]=0.0f;
	gx[1]=gx[2]=1.0f;
	gy[0]=gy[1]=0.0f;
	gy[2]=gy[3]=1.0f;

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_decoration);

	px[0]=px[3]=min.x;
	px[1]=px[2]=max.x;
	pz[0]=pz[3]=max.z;
	pz[1]=pz[2]=max.z;
	py[0]=py[1]=min.y;
	py[2]=py[3]=max.y;

	gx[0]=gx[3]=0.0f;
	gx[1]=gx[2]=1.0f;
	gy[0]=gy[1]=0.0f;
	gy[2]=gy[3]=1.0f;

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_decoration);

		// top and bottom

	px[0]=px[3]=min.x;
	px[1]=px[2]=max.x;
	pz[0]=pz[1]=min.z;
	pz[2]=pz[3]=max.z;
	py[0]=py[1]=py[2]=py[3]=max.y;

	gx[0]=gx[3]=0.0f;
	gx[1]=gx[2]=1.0f;
	gy[0]=gy[1]=0.0f;
	gy[2]=gy[3]=1.0f;

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_decoration);

	py[0]=py[1]=py[2]=py[3]=min.y;

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_decoration);

		// possibly rotate box

	if (ag_random_bool()) {
		ang.x=ang.z=0.0f;
		ang.y=(float)ag_random_int(30);

		map_mesh_calculate_center(&map,mesh_idx,&mpt);
		map_mesh_rotate(&map,mesh_idx,&mpt,&ang);
	}
}

void ag_generate_decoration_box_stack(void)
{
	int				n,idx,cmp_mesh_idx,
					stack_level,stack_count;
	d3pnt			pnt;
	ag_room_type	*room;

		// randomize location

	idx=ag_random_int(ag_state.room_count);
	room=&ag_state.rooms[idx];

	pnt.x=(room->min.x+ag_size_decoration_margin)+(ag_random_int(room->max.x-room->min.x)-(ag_size_decoration_margin*2));
	pnt.y=ag_map_bottom_y;
	pnt.z=(room->min.z+ag_size_decoration_margin)+(ag_random_int(room->max.z-room->min.z)-(ag_size_decoration_margin*2));

		// box stack

	stack_count=5;
	cmp_mesh_idx=map.mesh.nmesh;

	for (stack_level=0;stack_level!=3;stack_level++) {

		for (n=0;n!=stack_count;n++) {
			ag_generate_decoration_box(&pnt,cmp_mesh_idx);
		}

		cmp_mesh_idx+=stack_count;
		stack_count--;

		pnt.y-=ag_size_stack_box;
	}
}

/* =======================================================

      Add Spots
      
======================================================= */

void ag_generate_spots_add_single(char *name,int spot_obj_type,char *script_name)
{
	int					idx,mx,mz;
	ag_room_type		*room;
	spot_type			*spot;

		// add the spot

	spot=&map.spots[map.nspot];
	map.nspot++;

		// randomize location

	idx=ag_random_int(ag_state.room_count);
	room=&ag_state.rooms[idx];

	mx=(room->min.x+room->max.x)>>1;
	mz=(room->min.z+room->max.z)>>1;

	spot->pnt.x=mx+(ag_random_int((int)(room->size.x*50.0f))-((int)(room->size.x*25.0f)));
	spot->pnt.y=room->max.y;
	spot->pnt.z=mz+(ag_random_int((int)(room->size.z*50.0f))-((int)(room->size.z*25.0f)));

	spot->ang.x=0.0f;
	spot->ang.y=angle_find(spot->pnt.x,spot->pnt.z,mx,mz);
	spot->ang.z=0.0f;

		// name and type
	
	strcpy(spot->name,name);
	spot->type=spot_obj_type;
	strcpy(spot->script,script_name);
	spot->display_model[0]=0x0;
	spot->params[0]=0x0;
}

void ag_generate_spots_add(void)
{
		// player spot

	ag_generate_spots_add_single("Start",spot_type_player,"Player");

		// other spots

}
