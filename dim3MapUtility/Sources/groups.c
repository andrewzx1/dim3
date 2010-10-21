/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Deconstruct Curved Segments into Multiple Segments

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit �Created with dim3 Technology� is given on a single
application screen and in a single piece of the documentation
3. It is not resold, in it's current form or modified, as an
engine-only product

This code is presented as is. The author of dim3 takes no
responsibilities for any version of this code.

Any non-engine product (games, etc) created with this code is free
from any and all payment and/or royalties to the author of dim3,
and can be sold or given away.

(c) 2000-2010 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3maputility.h"
#endif

/* =======================================================

      Create and Dispose Single Group Unit List
      
======================================================= */

bool map_group_create_single_unit_list(map_type *map,int group_idx)
{
	int				n,unit_cnt,nmesh,nliq;
	map_mesh_type	*mesh;
	map_liquid_type	*liq;
	group_type		*group;
	group_unit_type	*unit_list;

	group=&map->groups[group_idx];
	
		// default setting
		
	group->unit_count=0;
	group->unit_list=NULL;
	
		// count meshes and liquids

	unit_cnt=0;

	nmesh=map->mesh.nmesh;
	mesh=map->mesh.meshes;

	for (n=0;n!=nmesh;n++) {
		if (mesh->group_idx==group_idx) unit_cnt++;
		mesh++;
	}

	nliq=map->liquid.nliquid;
	liq=map->liquid.liquids;

	for (n=0;n!=nliq;n++) {
		if (liq->group_idx==group_idx) unit_cnt++;
		liq++;
	}
			
	if (unit_cnt==0) return(TRUE);
	
		// create unit list
		
	unit_list=(group_unit_type*)malloc(unit_cnt*sizeof(group_unit_type));
	if (unit_list==NULL) return(FALSE);
	
	bzero(unit_list,(unit_cnt*sizeof(group_unit_type)));
	
	group->unit_count=unit_cnt;
	group->unit_list=unit_list;

		// fill in unit list
	
	nmesh=map->mesh.nmesh;
	mesh=map->mesh.meshes;

	for (n=0;n!=nmesh;n++) {
		if (mesh->group_idx==group_idx) {
			unit_list->type=group_type_mesh;
			unit_list->idx=n;
			unit_list++;
		}
		mesh++;
	}

	nliq=map->liquid.nliquid;
	liq=map->liquid.liquids;

	for (n=0;n!=nliq;n++) {
		if (liq->group_idx==group_idx) {
			unit_list->type=group_type_liquid;
			unit_list->idx=n;
			unit_list++;
		}
		liq++;
	}

	return(TRUE);
}

/* =======================================================

      Setup Group Unit Lists
      
======================================================= */

bool map_group_create_unit_list(map_type *map)
{
	int				n;
	
	for (n=0;n!=map->ngroup;n++) {
		if (!map_group_create_single_unit_list(map,n)) return(FALSE);
	}
	
	return(TRUE);
}

void map_group_dispose_unit_list(map_type *map)
{
	int				n;
	group_type		*group;
	
	group=map->groups;
	
	for (n=0;n!=map->ngroup;n++) {
		if (group->unit_list!=NULL) free(group->unit_list);
	
		group->unit_count=0;
		group->unit_list=NULL;
		
		group++;
	}
}

/* =======================================================

      Find Center Point for Group
      
======================================================= */

void map_group_get_center(map_type *map,int group_idx,d3pnt *center_pnt)
{
	int					n,unit_cnt,mx,my,mz;
	d3pnt				*pt;
	group_type			*group;
	group_unit_type		*unit_list;
	map_liquid_type		*liq;
	
	group=&map->groups[group_idx];
	
	unit_cnt=group->unit_count;
	if (unit_cnt==0) {
		center_pnt->x=center_pnt->y=center_pnt->z=0;
		return;
	}
	
	unit_list=group->unit_list;
	mx=my=mz=0;
	
	for (n=0;n!=unit_cnt;n++) {

		switch (unit_list->type) {

			case group_type_mesh:
				pt=&map->mesh.meshes[unit_list->idx].box.mid;
				mx+=pt->x;
				my+=pt->y;
				mz+=pt->z;
				break;

			case group_type_liquid:
				liq=&map->liquid.liquids[unit_list->idx];
				mx+=(liq->lft+liq->rgt)>>1;
				my+=liq->y;
				mz+=(liq->top+liq->bot)>>1;
				break;
		}
		
		unit_list++;
	}
	
	center_pnt->x=mx/unit_cnt;
	center_pnt->y=my/unit_cnt;
	center_pnt->z=mz/unit_cnt;
}

