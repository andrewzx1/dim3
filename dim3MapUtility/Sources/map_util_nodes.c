/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Node Routines

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

      Calculate Node Distance
      
======================================================= */

int map_node_to_node_distance(map_type *map,int from_idx,int to_idx)
{
	int			x,y,z,dist,idx;
	char		node_hit[max_node];
	node_type	*node;

	if ((from_idx<0) || (to_idx<0)) return(-1);
	if (from_idx==to_idx) return(0);
	
		// detect circular references
		
	bzero(node_hit,max_node);
	
	node_hit[from_idx]=0x1;

		// starting position

	node=&map->nodes[from_idx];

	x=node->pnt.x;
	y=node->pnt.y;
	z=node->pnt.z;

	idx=from_idx;
	dist=0;

		// run through all the distances

	while (TRUE) {
		idx=map_find_next_node_in_path(map,idx,to_idx);
		if (idx==-1) return(0);
		
		if (node_hit[idx]==0x1) return(0);			// circular, get out

		node=&map->nodes[idx];
		dist+=distance_get(x,y,z,node->pnt.x,node->pnt.y,node->pnt.z);

		x=node->pnt.x;
		y=node->pnt.y;
		z=node->pnt.z;

		if (idx==to_idx) break;
		
		node_hit[idx]=0x1;
	}

	return(dist);
}

/* =======================================================

      Find Nodes
      
======================================================= */

int map_find_node(map_type *map,char *name)
{
	int				n,nnode;
	node_type		*node;
	
	nnode=map->nnode;
	node=map->nodes;
	
	for (n=0;n!=nnode;n++) {
		if (strcmp(node->name,name)==0) return(n);
		node++;
	}
	
	return(-1);
}

int map_find_nearest_node_by_point(map_type *map,d3pnt *pnt)
{
	int					n,idx,nnode,d,dist;
	node_type			*node;

	idx=-1;
    dist=-1;
	
	nnode=map->nnode;
	node=map->nodes;
    
	for (n=0;n!=nnode;n++) {
		node=&map->nodes[n];
	
		d=distance_get(pnt->x,pnt->y,pnt->z,node->pnt.x,node->pnt.y,node->pnt.z);
		if ((dist==-1) || (d<dist)) {
			dist=d;
			idx=n;
		}
		
		node++;
	}
	
	return(idx);
}

int map_find_nearest_node_in_path(map_type *map,int from_idx,char *name,int *dist)
{
	int					n,nnode,idx,d,cur_dist;
	node_type			*node;

	idx=-1;
	cur_dist=-1;
	
	nnode=map->nnode;
    
	for (n=0;n!=nnode;n++) {
		if (n==from_idx) continue;
		
			// check if same name and in path
		
		node=&map->nodes[n];
		if (node->path_hint[from_idx]==-1) continue;
		if (strcasecmp(name,node->name)!=0) continue;
		
			// smallest distance
			
		d=map_node_to_node_distance(map,from_idx,n);
		if ((cur_dist==-1) || (d<cur_dist)) {
			cur_dist=d;
			idx=n;
		}
	}
			
	*dist=cur_dist;

	return(idx);
}

int map_count_node(map_type *map,char *name,int skip_idx)
{
	int				n,nnode,count;
	node_type		*node;
	
	count=0;

	node=map->nodes;
	nnode=map->nnode;
	
	for (n=0;n!=nnode;n++) {
		if (n!=skip_idx) {
			if (strcmp(node->name,name)==0) count++;
		}
		node++;
	}
	
	return(count);
}

int map_find_random_node(map_type *map,char *name,int skip_idx)
{
	int				n,nnode,count,node_count;
	node_type		*node;
	
	node_count=map_count_node(map,name,skip_idx);
	if (node_count==-1) return(-1);
	
	count=random_int(node_count);
	
	node=map->nodes;
	nnode=map->nnode;
	
	for (n=0;n!=nnode;n++) {
		if (n!=skip_idx) {
			if (strcmp(node->name,name)==0) {
				if (count==0) return(n);
				count--;
			}
		}
		node++;
	}
	
	return(-1);
}

int map_find_random_forward_node(map_type *map,char *name,int skip_idx,d3pnt *pnt,d3ang *ang)
{
	int				n,nnode,count;
	short			node_indexes[max_node];
	float			ang_y;
	node_type		*node;
	
		// gather up all nodes
		// that match

	count=0;
	nnode=map->nnode;
	
	for (n=0;n!=nnode;n++) {

			// skip node

		if (n==skip_idx) continue;

			// node names

		node=&map->nodes[n];
		if (strcmp(node->name,name)!=0) continue;

			// forward

		ang_y=angle_find(pnt->x,pnt->z,node->pnt.x,node->pnt.z);
		if ((ang_y>90.0f) && (ang_y<180.0f)) continue;

			// found a node!

		node_indexes[count++]=(short)n;
	}

		// any matches?

	if (count==0) return(-1);

		// get random node
	
	return((int)node_indexes[random_int(count)]);
}

/* =======================================================

      Next In Path
      
======================================================= */

int map_find_next_node_in_path(map_type *map,int from_idx,int to_idx)
{
	if ((from_idx==-1) || (to_idx==-1)) return(-1);
	
	return((int)map->nodes[from_idx].path_hint[to_idx]);
}

