/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Auto-Generate Map Routines

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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3maputility.h"
#endif

extern int map_auto_generate_random_int(int max);
extern bool map_auto_generate_second_story_exist(int rn);

extern int								ag_box_count;

extern auto_generate_settings_type		ag_settings;
extern auto_generate_box_type			ag_boxes[max_ag_box];

/* =======================================================

      Create Lights
      
======================================================= */

void map_auto_generate_lights(map_type *map)
{
	int						n,x,z,y,intensity;
	float					exp;
	double					dx,dz;
	auto_generate_box_type	*portal;
	map_light_type			*lit;
	
	if (ag_settings.light_boost==0.0f) return;
	
	for (n=0;n!=ag_box_count;n++) {

		if (map->nlight>=max_map_light) break;
		
		portal=&ag_boxes[n];
		
			// if corridor is to small, then no light
			
		if (portal->corridor_flag==ag_corridor_flag_horizontal) {
			if ((portal->max.x-portal->min.x)<ag_constant_corridor_is_small_size) continue;
		}
		if (portal->corridor_flag==ag_corridor_flag_vertical) {
			if ((portal->max.z-portal->min.z)<ag_constant_corridor_is_small_size) continue;
		}
	
			// find light position
			
		x=(portal->max.x+portal->min.x)>>1;
		z=(portal->max.z+portal->min.z)>>1;
		y=portal->min.y+((portal->max.y-portal->min.y)>>2);		// near top of room
		
			// get intensity
			// corridors always stay within corridor
			// and have hard lights
			
		dx=(portal->max.x-portal->min.x);
		dz=(portal->max.z-portal->min.z);

		if (portal->corridor_flag!=ag_corridor_flag_portal) {
			if (dx>dz) {
				intensity=(int)(((float)dx)*0.45f);
			}
			else {
				intensity=(int)(((float)dz)*0.45f);
			}
			exp=0.0f;
		}
		else {
			intensity=(int)((float)(sqrt((dx*dx)+(dz*dz)))*ag_settings.light_boost);
			exp=1.0f;
		}

		if (intensity<0) continue;
		
			// create the light
	
		lit=&map->lights[map->nlight];
		map->nlight++;
		
		lit->pnt.x=x;
		lit->pnt.y=y;
		lit->pnt.z=z;
		lit->type=lt_normal;
		lit->light_map=ag_settings.light_map;
		lit->direction=ld_all;
		lit->col.r=lit->col.g=lit->col.b=1.0f;
		lit->intensity=intensity;
		lit->exponent=exp;
		lit->name[0]=0x0;
		lit->on=TRUE;
	}
}

/* =======================================================

      Add Nodes
      
======================================================= */

void map_auto_generate_nodes_add(map_type *map,int x,int y,int z)
{
	int						n;
	node_type				*node;

	if (map->nnode>max_node) return;

	node=&map->nodes[map->nnode];

	node->idx=map->nnode;
	node->event_id=0;

	for (n=0;n!=max_node_link;n++) {
		node->link[n]=-1;
	}

	node->name[0]=0x0;

	node->pnt.x=x;
	node->pnt.y=y;
	node->pnt.z=z;

	node->ang.x=node->ang.y=node->ang.z=0.0f;

	map->nnode++;
}

void map_auto_generate_nodes(map_type *map)
{
	int						n,x,y,z,x_sz,z_sz,sz,
							portal_sz,portal_high,split_factor,ss_ty;
	auto_generate_box_type	*portal;

		// some factors

	portal_sz=(int)(((float)ag_settings.map.map_sz)*ag_constant_portal_percent);
	portal_high=(int)(((float)portal_sz)*ag_constant_portal_high_percent);
	split_factor=(int)(((float)portal_sz)*ag_constant_portal_split_factor_percent);
	ss_ty=(map_max_size>>1)-((portal_high>>1)+ag_constant_step_story_size);

		// create the nodes

	portal=ag_boxes;
	
	for (n=0;n!=ag_box_count;n++) {

			// corridors get single node

		if (portal->corridor_flag!=ag_corridor_flag_portal) {
			x=(portal->max.x+portal->min.x)>>1;
			y=portal->max.y;
			z=(portal->max.z+portal->min.z)>>1;

			map_auto_generate_nodes_add(map,x,y,z);
		}

			// rooms get multiple nodes

		else {

				// first story nodes

			x_sz=(portal->max.x-portal->min.x)>>2;
			z_sz=(portal->max.z-portal->min.z)>>2;

			y=portal->max.y;

			map_auto_generate_nodes_add(map,(portal->min.x+x_sz),y,(portal->min.z+z_sz));
			map_auto_generate_nodes_add(map,(portal->max.x-x_sz),y,(portal->min.z+z_sz));
			map_auto_generate_nodes_add(map,(portal->min.x+x_sz),y,(portal->max.z-z_sz));
			map_auto_generate_nodes_add(map,(portal->max.x-x_sz),y,(portal->max.z-z_sz));

				// second story nodes

			if ((ag_settings.second_story) && (map_auto_generate_second_story_exist(n))) {

				map_auto_generate_nodes_add(map,(portal->min.x+(split_factor*2)),ss_ty,(portal->min.z+split_factor));
				map_auto_generate_nodes_add(map,((portal->min.x+portal->max.x)>>1),ss_ty,(portal->min.z+split_factor));
				map_auto_generate_nodes_add(map,(portal->max.x-(split_factor*2)),ss_ty,(portal->min.z+split_factor));

				map_auto_generate_nodes_add(map,(portal->min.x+(split_factor*2)),ss_ty,(portal->max.z-split_factor));
				map_auto_generate_nodes_add(map,((portal->min.x+portal->max.x)>>1),ss_ty,(portal->max.z-split_factor));
				map_auto_generate_nodes_add(map,(portal->max.x-(split_factor*2)),ss_ty,(portal->max.z-split_factor));

				map_auto_generate_nodes_add(map,(portal->min.x+split_factor),ss_ty,(portal->min.z+(split_factor*2)));
				map_auto_generate_nodes_add(map,(portal->min.x+split_factor),ss_ty,((portal->min.z+portal->max.z)>>1));
				map_auto_generate_nodes_add(map,(portal->min.x+split_factor),ss_ty,(portal->max.z-(split_factor*2)));

				map_auto_generate_nodes_add(map,(portal->max.x-split_factor),ss_ty,(portal->min.z+(split_factor*2)));
				map_auto_generate_nodes_add(map,(portal->max.x-split_factor),ss_ty,((portal->min.z+portal->max.z)>>1));
				map_auto_generate_nodes_add(map,(portal->max.x-split_factor),ss_ty,(portal->max.z-(split_factor*2)));

				sz=(split_factor*5)/4;
				map_auto_generate_nodes_add(map,(portal->min.x+sz),ss_ty,(portal->min.z+sz));
				map_auto_generate_nodes_add(map,(portal->max.x-sz),ss_ty,(portal->min.z+sz));
				map_auto_generate_nodes_add(map,(portal->min.x+sz),ss_ty,(portal->max.z-sz));
				map_auto_generate_nodes_add(map,(portal->max.x-sz),ss_ty,(portal->max.z-sz));
			}
		}

		portal++;
	}
}

/* =======================================================

      Add Spots
      
======================================================= */

/* supergumba -- spot generation needs to be fixed

void map_auto_generate_spots(map_type *map)
{
	int				n,k,x,y,z,idx;
	segment_type	*seg;
	spot_type		*spot;
	
	if (!ag_settings.spots) return;
	
	for (n=0;n!=ag_settings.spot_count;n++) {
		idx=map_auto_generate_rough_floor_find_floor(map,TRUE);
		seg=&map->segments[idx];
		
			// get middle
			
		x=y=z=0;
		
		for (k=0;k!=seg->data.fc.ptsz;k++) {
			x+=seg->data.fc.x[k];
			y+=seg->data.fc.y[k];
			z+=seg->data.fc.z[k];
		}
		
		x/=seg->data.fc.ptsz;
		y/=seg->data.fc.ptsz;
		z/=seg->data.fc.ptsz;
		
			// create spot
			
		if (map->nspot>max_spot) return;
		
		spot=&map->spots[map->nspot];
		
		spot->pos.rn=seg->rn;
		spot->pos.x=x;
		spot->pos.z=z;
		spot->pos.y=y;
		spot->ang.y=0;
		sprintf(spot->name,"Spot %d",map->nspot);
		strcpy(spot->type,"Spot");
		spot->script[0]=0x0;
		spot->display_model[0]=0x0;
		spot->params[0]=0x0;
		
		map->nspot++;
	}
}
	*/


