/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Auto-Generate Map Routines

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
	#include "dim3maputility.h"
#endif

extern void map_auto_generate_random_seed(int seed);
extern int map_auto_generate_random_int(int max);

extern bool map_auto_generate_portal_collision(int x,int z,int ex,int ez,int skip_idx,bool corridor_only);
extern int map_auto_generate_portal_horz_edge_block(int skip_portal_idx,int z,int ez,int x);
extern int map_auto_generate_portal_vert_edge_block(int skip_portal_idx,int x,int ex,int z);
extern bool map_auto_generate_portal_horz_edge_touch(int skip_portal_idx,int z,int ez,int x);
extern bool map_auto_generate_portal_vert_edge_touch(int skip_portal_idx,int x,int ex,int z);
extern bool map_auto_generate_portal_touching_left(int portal_idx);
extern bool map_auto_generate_portal_touching_right(int portal_idx);
extern bool map_auto_generate_portal_touching_top(int portal_idx);
extern bool map_auto_generate_portal_touching_bottom(int portal_idx);
extern bool map_auto_generate_portal_touching_any(int portal_idx);
extern bool map_auto_generate_portal_on_map_left_edge(int portal_idx);
extern bool map_auto_generate_portal_on_map_right_edge(int portal_idx);
extern bool map_auto_generate_portal_on_map_top_edge(int portal_idx);
extern bool map_auto_generate_portal_on_map_bottom_edge(int portal_idx);

extern bool map_auto_generate_block_collision(auto_generate_settings_type *ags,int x,int z,int ex,int ez,bool corridor_only);

extern void map_auto_generate_poly_from_square_wall(int lx,int lz,int rx,int rz,int ty,int by,int *x,int *y,int *z,float *gx,float *gy);
extern void map_auto_generate_poly_from_top_trig_wall(int lx,int lz,int rx,int rz,int ty,int by,int *x,int *y,int *z,float *gx,float *gy);
extern void map_auto_generate_poly_from_bot_trig_wall(int lx,int lz,int rx,int rz,int ty,int by,int *x,int *y,int *z,float *gx,float *gy);
extern void map_auto_generate_poly_from_square_floor(int lx,int lz,int rx,int rz,int fy,int *x,int *y,int *z,float *gx,float *gy);
extern void map_auto_generate_poly_from_square_floor_slant(int lx,int lz,int rx,int rz,int fy,int yadd,int lower_mode,bool reverse_slant,int *x,int *y,int *z,float *gx,float *gy);

extern void map_auto_generate_mesh_set_portal_mesh(int box_idx);
extern void map_auto_generate_mesh_set_lock(map_type *map);
extern bool map_auto_generate_mesh_add_poly(map_type *map,int box_idx,int txt_idx,int ptsz,int *x,int *y,int *z,float *gx,float *gy);
extern void map_auto_generate_mesh_effect_uv_last_poly(map_type *map,float mult_x,float mult_y,bool rot);

extern int map_auto_generate_steps_get_length(int ty,int by,int step_size,int step_high);
extern void map_auto_generate_steps_mesh(map_type *map,int rn,int step_type,int step_sz,int step_high,int ty,int by,int kx,int kz,float ang_y);
extern void map_auto_generate_lift(map_type *map,int rn,int step_high,int ty,int by,int lx,int rx,int lz,int rz);

extern void map_auto_generate_lights(map_type *map);
extern void map_auto_generate_nodes(map_type *map);
extern void map_auto_generate_spots(map_type *map);
extern void map_auto_generate_ramps(map_type *map);
extern void map_auto_generate_corridor_to_portal_connections(map_type *map);
extern void map_auto_generate_doors(map_type *map);
extern void map_auto_generate_mesh_clear_window_list(void);
extern void map_auto_generate_mesh_last_poly_add_window_list(map_type *map,bool window_ok,int rn,int x,int z,int ty,int by,int direction);
extern void map_auto_generate_mesh_add_windows(map_type *map);

extern bool map_auto_generate_second_story_exist(int rn);
extern void map_auto_generate_second_story(map_type *map);

int									ag_box_count;

auto_generate_settings_type			ag_settings;
auto_generate_box_type				ag_boxes[max_ag_box];

/* =======================================================

      Generate Initial Unconnected Portals
      
======================================================= */

void map_auto_generate_initial_portals(map_type *map)
{
	int			i,x,z,ex,ez,initial_count,try_count,split_factor,
				map_x_sz,map_z_sz,portal_sz,portal_rand_sz,portal_min_sz;

		// sizes

	map_x_sz=ag_settings.map.right-ag_settings.map.left;
	map_z_sz=ag_settings.map.bottom-ag_settings.map.top;

	portal_sz=(int)(((float)ag_settings.map.map_sz)*ag_constant_portal_percent);
	portal_rand_sz=(int)(((float)portal_sz)*ag_constant_portal_random_percent);
	portal_min_sz=portal_sz-portal_rand_sz;
	
	split_factor=(int)(((float)portal_sz)*ag_constant_portal_split_factor_percent);
	
		// initial count of portals
	
	initial_count=(ag_settings.map.map_sz/portal_sz)*4;

		// create portals

	ag_box_count=0;

	for (i=0;i!=initial_count;i++) {
		
		try_count=0;

		while (TRUE) {

				// create portal start

			x=ag_settings.map.left+map_auto_generate_random_int(map_x_sz);
			z=ag_settings.map.top+map_auto_generate_random_int(map_z_sz);
			
				// create portal size
				
			ex=x+(portal_min_sz+map_auto_generate_random_int(portal_rand_sz));
			ez=z+(portal_min_sz+map_auto_generate_random_int(portal_rand_sz));

				// fix right/bottom overflows

			if (ex>ag_settings.map.right) {
				x=ag_settings.map.right-(ex-x);
				ex=ag_settings.map.right;
			}
			if (ez>ag_settings.map.bottom) {
				z=ag_settings.map.bottom-(ez-z);
				ez=ag_settings.map.bottom;
			}
	
				// use splits as grid
				
			x/=split_factor;
			x*=split_factor;
			
			z/=split_factor;
			z*=split_factor;
			
			ex/=split_factor;
			ex*=split_factor;
			
			ez/=split_factor;
			ez*=split_factor;

				// check for collisions

			if ((!map_auto_generate_block_collision(&ag_settings,x,z,ex,ez,FALSE)) && (!map_auto_generate_portal_collision(x,z,ex,ez,-1,FALSE))) break;
			
				// try to much?
				
			try_count++;
			if (try_count==100) return;
		}

			// create portal

		ag_boxes[ag_box_count].min.x=x;
		ag_boxes[ag_box_count].max.x=ex;
		ag_boxes[ag_box_count].min.z=z;
		ag_boxes[ag_box_count].max.z=ez;
		
		ag_boxes[ag_box_count].corridor_flag=ag_corridor_flag_portal;
		ag_boxes[ag_box_count].mesh_idx=map_mesh_add(map);
		map->mesh.meshes[ag_boxes[ag_box_count].mesh_idx].normal_mode=mesh_normal_mode_in;

		ag_box_count++;
	}
}

/* =======================================================

      Merge Close Portals
      
======================================================= */

void map_auto_generate_merge_portals(void)
{
	int						i,n,k,dist,portal_merge_distance,
							split_factor,merge_try_count,portal_sz;
	bool					moved;
	auto_generate_box_type	*chk_portal,*merge_portal;

	portal_sz=(int)(((float)ag_settings.map.map_sz)*ag_constant_portal_percent);
	portal_merge_distance=(int)(((float)portal_sz)*ag_constant_portal_merge_percent);

		// attempt to merge portals within a certain distance
		// together.  All portals are grided on the split
		// factor, so we merge on that

		// only keep moving for a limited time
		// so portals that bounce between two don't cause
		// an infinite loop
		
	split_factor=(int)(((float)portal_sz)*ag_constant_portal_split_factor_percent);
	merge_try_count=(portal_merge_distance/split_factor)*2;

	for (i=0;i!=merge_try_count;i++) {

		moved=FALSE;		// keep looping until no more merging

		for (n=0;n!=ag_box_count;n++) {
			chk_portal=&ag_boxes[n];

				// only move portals in one direction

				// slowly move portals towards merges
				// so they can merge on multiple sides and
				// not interfere with other merges

			for (k=0;k!=ag_box_count;k++) {
				if (k==n) break;
				
				merge_portal=&ag_boxes[k];

					// merge left

				dist=merge_portal->min.x-chk_portal->max.x;
				if ((dist>0) && (dist<=portal_merge_distance)) {
					if (!map_auto_generate_portal_collision((merge_portal->min.x-split_factor),merge_portal->min.z,merge_portal->max.x,merge_portal->max.z,k,FALSE)) {
						merge_portal->min.x-=split_factor;
						moved=TRUE;
						break;
					}
				}

					// merge right

				dist=chk_portal->min.x-merge_portal->max.x;
				if ((dist>0) && (dist<=portal_merge_distance)) {
					if (!map_auto_generate_portal_collision(merge_portal->min.x,merge_portal->min.z,(merge_portal->max.x+split_factor),merge_portal->max.z,k,FALSE)) {
						merge_portal->max.x+=split_factor;
						moved=TRUE;
						break;
					}
				}

					// merge top

				dist=merge_portal->min.z-chk_portal->max.z;
				if ((dist>0) && (dist<=portal_merge_distance)) {
					if (!map_auto_generate_portal_collision(merge_portal->min.x,(merge_portal->min.z-split_factor),merge_portal->max.x,merge_portal->max.z,k,FALSE)) {
						merge_portal->min.z-=split_factor;
						moved=TRUE;
						break;
					}
				}

					// merge bottom

				dist=chk_portal->min.z-merge_portal->max.z;
				if ((dist>0) && (dist<=portal_merge_distance)) {
					if (!map_auto_generate_portal_collision(merge_portal->min.x,merge_portal->min.z,merge_portal->max.x,(merge_portal->max.z+split_factor),k,FALSE)) {
						merge_portal->max.z+=split_factor;
						moved=TRUE;
						break;
					}
				}
			}
		}

		if (!moved) break;
	}
}

void map_auto_generate_merge_portals_center(void)
{
	int						n,x,z,cx,cz,move_cnt,portal_sz,split_factor,
							xadd[max_portal],zadd[max_portal];
	bool					moved;
	auto_generate_box_type	*portal;

		// movement variables

	portal_sz=(int)(((float)ag_settings.map.map_sz)*ag_constant_portal_percent);
	split_factor=(int)(((float)portal_sz)*ag_constant_portal_split_factor_percent);

		// find center

	cx=cz=0;

	for (n=0;n!=ag_box_count;n++) {
		portal=&ag_boxes[n];

		cx+=((portal->min.x+portal->max.x)>>1);
		cz+=((portal->min.z+portal->max.z)>>1);
	}

	cx/=ag_box_count;
	cz/=ag_box_count;

		// get movement directions

	for (n=0;n!=ag_box_count;n++) {
		portal=&ag_boxes[n];

		if (((portal->min.x+portal->max.x)>>1)<cx) {
			xadd[n]=split_factor;
		}
		else {
			xadd[n]=-split_factor;
		}

		if (((portal->min.z+portal->max.z)>>1)<cz) {
			zadd[n]=split_factor;
		}
		else {
			zadd[n]=-split_factor;
		}
	}

		// move all portals towards center
		// until no more movement or over a move count

	move_cnt=0;

	while (move_cnt<100) {

		moved=FALSE;

		for (n=0;n!=ag_box_count;n++) {
			portal=&ag_boxes[n];

			x=(portal->min.x+portal->max.x)>>1;
			z=(portal->min.z+portal->max.z)>>1;

			if (((xadd[n]>0) && (x<cx)) || ((xadd[n]<0) && (x>cx))) {
				if (!map_auto_generate_portal_collision((portal->min.x+xadd[n]),portal->min.z,(portal->max.x+xadd[n]),portal->max.z,n,FALSE)) {
					portal->min.x+=xadd[n];
					portal->max.x+=xadd[n];
					moved=TRUE;
				}
			}

			if (((zadd[n]>0) && (z<cz)) || ((zadd[n]<0) && (z>cz))) {
				if (!map_auto_generate_portal_collision(portal->min.x,(portal->min.z+zadd[n]),portal->max.x,(portal->max.z+zadd[n]),n,FALSE)) {
					portal->min.z+=zadd[n];
					portal->max.z+=zadd[n];
					moved=TRUE;
				}
			}

		}

		if (!moved) break;

		move_cnt++;
	}
}

/* =======================================================

      Connect Portals
      
======================================================= */

void map_auto_generate_connect_portals(map_type *map)
{
	int							n,k,corridor_sz,corridor_rand_sz,corridor_min_sz,portal_sz,
								portal_merge_distance,portal_connect_distance,connect_sz,
								x,z,ex,ez,x2,z2,ex2,ez2,dist,nportal,cnt,split_factor;
	auto_generate_box_type		*chk_portal,*cnt_portal;

		// get sizes

	portal_sz=(int)(((float)ag_settings.map.map_sz)*ag_constant_portal_percent);
	corridor_sz=(int)(((float)portal_sz)*ag_constant_corridor_size_percent);
	corridor_rand_sz=(int)(((float)corridor_sz)*ag_constant_corridor_random_percent);
	corridor_min_sz=corridor_sz-corridor_rand_sz;

	portal_merge_distance=(int)(((float)portal_sz)*ag_constant_portal_merge_percent);
	portal_connect_distance=(int)(((float)portal_sz)*ag_constant_portal_connect_percent);
	
	split_factor=(int)(((float)portal_sz)*ag_constant_portal_split_factor_percent);

		// get original portal count

	nportal=ag_box_count;

		// check against all portals

	for (n=0;n!=nportal;n++) {
		chk_portal=&ag_boxes[n];

			// check for portals that fit horizontal connections

		cnt=0;

		for (k=0;k!=nportal;k++) {
			cnt_portal=&ag_boxes[k];

				// any more portals?

			if (ag_box_count>=max_ag_box) return;

				// portals miss each other?

			if (chk_portal->min.z>=cnt_portal->max.z) continue;
			if (chk_portal->max.z<=cnt_portal->min.z) continue;

				// too close or too far away?

			dist=cnt_portal->min.x-chk_portal->max.x;
			if (dist<=0) continue;
			if (dist<portal_merge_distance) continue;
			if (dist>portal_connect_distance) continue;

				// get portal size

			x=chk_portal->max.x;
			ex=cnt_portal->min.x;

			connect_sz=corridor_min_sz+map_auto_generate_random_int(corridor_rand_sz);
			if (connect_sz<(split_factor*2)) connect_sz=split_factor*2;

			z=(((chk_portal->min.z+chk_portal->max.z)/2)+((cnt_portal->min.z+cnt_portal->max.z)/2))/2;
			z-=(connect_sz/2);
			ez=z+connect_sz;

			if (z<chk_portal->min.z) z=chk_portal->min.z;
			if (z<cnt_portal->min.z) z=cnt_portal->min.z;
			if (ez>chk_portal->max.z) ez=chk_portal->max.z;
			if (ez>cnt_portal->max.z) ez=cnt_portal->max.z;
			
				// fix vertical

			z/=split_factor;
			z*=split_factor;
			
			ez/=split_factor;
			ez*=split_factor;

				// make sure corridor isn't in corner

			if ((z==chk_portal->min.z) || (z==cnt_portal->min.z)) z+=split_factor;
			if ((ez==chk_portal->max.z) || (ez==cnt_portal->max.z)) ez-=split_factor;

			if (z>=ez) continue;

				// not enough connection size for portals?

			if ((ez-z)<corridor_min_sz) continue;
			
				// check for collisions
				
			z2=z-split_factor;
			ez2=ez+split_factor;			// don't create if z's will touch other portals, then it's no longer a corridor

			if ((map_auto_generate_block_collision(&ag_settings,x,z2,ex,ez2,TRUE)) || (map_auto_generate_portal_collision(x,z2,ex,ez2,-1,FALSE))) continue;

				// create portal

			ag_boxes[ag_box_count].min.x=x;
			ag_boxes[ag_box_count].max.x=ex;
			ag_boxes[ag_box_count].min.z=z;
			ag_boxes[ag_box_count].max.z=ez;
			
			ag_boxes[ag_box_count].corridor_flag=ag_corridor_flag_horizontal;
			
			ag_boxes[ag_box_count].mesh_idx=map_mesh_add(map);
			map->mesh.meshes[ag_boxes[ag_box_count].mesh_idx].normal_mode=mesh_normal_mode_in;

			ag_boxes[ag_box_count].corridor_connect_box_idx[0]=n;
			ag_boxes[ag_box_count].corridor_connect_box_idx[1]=k;

			ag_box_count++;

				// only allow up to 2 connections in a direction

			cnt++;
			if (cnt==2) break;
		}

			// check for portals that fit vertical connections

		cnt=0;

		for (k=0;k!=nportal;k++) {
			cnt_portal=&ag_boxes[k];

				// any more portals?

			if (ag_box_count>=max_ag_box) return;

				// portals miss each other?

			if (chk_portal->min.x>=cnt_portal->max.x) continue;
			if (chk_portal->max.x<=cnt_portal->min.x) continue;

				// too close or too far away?

			dist=cnt_portal->min.z-chk_portal->max.z;
			if (dist<=0) continue;
			if (dist<portal_merge_distance) continue;
			if (dist>portal_connect_distance) continue;

				// get portal size

			z=chk_portal->max.z;
			ez=cnt_portal->min.z;

			connect_sz=corridor_min_sz+map_auto_generate_random_int(corridor_rand_sz);
			if (connect_sz<(split_factor*2)) connect_sz=split_factor*2;

			x=(((chk_portal->min.x+chk_portal->max.x)/2)+((cnt_portal->min.x+cnt_portal->max.x)/2))/2;
			x-=(connect_sz/2);
			ex=x+connect_sz;

			if (x<chk_portal->min.x) x=chk_portal->min.x;
			if (x<cnt_portal->min.x) x=cnt_portal->min.x;
			if (ex>chk_portal->max.x) ex=chk_portal->max.x;
			if (ex>cnt_portal->max.x) ex=cnt_portal->max.x;
			
				// fix horizontal

			x/=split_factor;
			x*=split_factor;
			
			ex/=split_factor;
			ex*=split_factor;

				// make sure corridor isn't in corner

			if ((x==chk_portal->min.x) || (x==cnt_portal->min.x)) x+=split_factor;
			if ((ex==chk_portal->max.x) || (ex==cnt_portal->max.x)) ex-=split_factor;

			if (x>=ex) continue;

				// not enough connection size for portals?

			if ((ex-x)<corridor_min_sz) continue;
			
				// check for collisions
				
			x2=x-split_factor;
			ex2=ex+split_factor;			// don't create if x's will touch other portals, then it's no longer a corridor

			if ((map_auto_generate_block_collision(&ag_settings,x2,z,ex2,ez,TRUE)) || (map_auto_generate_portal_collision(x2,z,ex2,ez,-1,FALSE))) continue;

				// create portal
				
			ag_boxes[ag_box_count].min.x=x;
			ag_boxes[ag_box_count].max.x=ex;
			ag_boxes[ag_box_count].min.z=z;
			ag_boxes[ag_box_count].max.z=ez;
			
			ag_boxes[ag_box_count].corridor_flag=ag_corridor_flag_vertical;

			ag_boxes[ag_box_count].mesh_idx=map_mesh_add(map);
			map->mesh.meshes[ag_boxes[ag_box_count].mesh_idx].normal_mode=mesh_normal_mode_in;

			ag_boxes[ag_box_count].corridor_connect_box_idx[0]=n;
			ag_boxes[ag_box_count].corridor_connect_box_idx[1]=k;

			ag_box_count++;

				// only allow up to 2 connections in a direction

			cnt++;
			if (cnt==2) break;
		}
	}
}

/* =======================================================

      Setup Random Y for Portals
      
======================================================= */

void map_auto_generate_portal_y(void)
{
	int							n,by_add,portal_high,portal_high_story_add,
								corridor_high,y,y2,ty,by,ss_ty,extra_ty,extra_by,portal_sz;
	auto_generate_box_type		*portal;
	
		// portal sizes

	portal_sz=(int)(((float)ag_settings.map.map_sz)*ag_constant_portal_percent);
	portal_high=(int)(((float)portal_sz)*ag_constant_portal_high_percent);
	portal_high_story_add=(int)(((float)portal_sz)*ag_constant_portal_story_high_add_percent);

	corridor_high=(int)(((float)portal_sz)*ag_constant_corridor_high_percent);

	ty=(map_max_size/2)-(portal_high/2);
	by=(map_max_size/2)+(portal_high/2);
	
	extra_ty=(int)(((float)portal_high)*ag_constant_portal_high_extra_top);
	extra_by=(int)(((float)portal_high)*ag_constant_portal_high_extra_bottom);

		// all second story portals have same height

	ss_ty=ty-(map_auto_generate_random_int(extra_ty)+portal_high_story_add);

		// run through portals

	portal=ag_boxes;

	for (n=0;n!=ag_box_count;n++) {
		
			// room portals

		if (portal->corridor_flag==ag_corridor_flag_portal) {

				// touching rooms get second stories
				
			if ((map_auto_generate_portal_touching_any(n)) && (ag_settings.second_story)) {
				portal->min.y=ss_ty;
			}
			else {
				portal->min.y=ty-map_auto_generate_random_int(extra_ty);
			}
			
			by_add=map_auto_generate_random_int(extra_by);
			if (by_add<=ag_constant_step_corridor_high) {
				by_add=0;
			}
			else {
				by_add/=ag_constant_step_corridor_high;
				if (by_add!=0) by_add=(by_add*ag_constant_step_corridor_high);
			}

			portal->max.y=by+by_add;

				// fit within map_enlarge

			portal->min.y/=map_enlarge;
			portal->min.y*=map_enlarge;

			portal->max.y/=map_enlarge;
			portal->max.y*=map_enlarge;
		}

			// corridor portals

		else {

				// possibly put corridor on second story

			portal->corridor_second_story=FALSE;

			if ((map_auto_generate_second_story_exist(portal->corridor_connect_box_idx[0])) && (map_auto_generate_second_story_exist(portal->corridor_connect_box_idx[1]))) {
				portal->corridor_second_story=(map_auto_generate_random_int(100)>50);
			}

				// regular corridor stick to highest
				// room on one of their sides

			if (!portal->corridor_second_story) {
				y=ag_boxes[portal->corridor_connect_box_idx[0]].max.y;
				y2=ag_boxes[portal->corridor_connect_box_idx[1]].max.y;
				if (y2<y) y=y2;
			}

				// second story start directly on second story

			else {
				y=((map_max_size>>1)-((portal_high>>1)+ag_constant_step_story_size))+map_enlarge;
			}

			portal->min.y=y-corridor_high;
			portal->max.y=y;

				// fit within map_enlarge

			portal->min.y/=map_enlarge;
			portal->min.y*=map_enlarge;

			portal->max.y/=map_enlarge;
			portal->max.y*=map_enlarge;
		}

		portal++;
	}
}

/* =======================================================

      Auto-Generate Walls
      
======================================================= */

void map_auto_generate_portal_walls_add_single(map_type *map,int rn,int x,int z,int ex,int ez,int ty,int by,int *px,int *py,int *pz,int direction,int corridor_portal_idx,bool window_ok)
{
	int				top_ty,top_by,bot_ty,bot_by,portal_sz,portal_high;
	float			gx[8],gy[8];

		// regular rooms

	if (!map_auto_generate_second_story_exist(rn)) {

		if (corridor_portal_idx!=-1) {
			by=ag_boxes[corridor_portal_idx].min.y;
		}

		map_auto_generate_poly_from_square_wall(x,z,ex,ez,ty,by,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.portal_wall,4,px,py,pz,gx,gy);
		map_auto_generate_mesh_last_poly_add_window_list(map,window_ok,rn,x,z,ty,by,direction);

		return;
	}

		// second story rooms

	portal_sz=(int)(((float)ag_settings.map.map_sz)*ag_constant_portal_percent);
	portal_high=(int)(((float)portal_sz)*ag_constant_portal_high_percent);

	top_ty=ty;
	top_by=(map_max_size>>1)-((portal_high>>1)+ag_constant_step_story_size);
	bot_ty=top_by+ag_constant_story_floor_high;
	bot_by=by;

		// if it's a corridor hit, adjust polys
		// around the door

	if (corridor_portal_idx!=-1) {
		if (ag_boxes[corridor_portal_idx].corridor_second_story) {
			top_by=ag_boxes[corridor_portal_idx].min.y;
		}
		else {
			bot_by=ag_boxes[corridor_portal_idx].min.y;
		}
	}

		// make regular and second story polys

	if (bot_ty<bot_by) {
		map_auto_generate_poly_from_square_wall(x,z,ex,ez,bot_ty,bot_by,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.portal_wall,4,px,py,pz,gx,gy);
		map_auto_generate_mesh_last_poly_add_window_list(map,window_ok,rn,x,z,bot_ty,bot_by,direction);
	}

	if (top_by>top_ty) {
		map_auto_generate_poly_from_square_wall(x,z,ex,ez,top_ty,top_by,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.portal_wall_2,4,px,py,pz,gx,gy);
		map_auto_generate_mesh_last_poly_add_window_list(map,window_ok,rn,x,z,top_ty,top_by,direction);
	}
}

void map_auto_generate_portal_walls_add(map_type *map,int rn,auto_generate_box_type *portal)
{
	int				x,z,xsz,zsz,ty,by,split_factor,portal_sz,block_portal_idx,
					px[8],py[8],pz[8];
	bool			lft_edge,rgt_edge,top_edge,bot_edge,window_ok,corridor_edge;

		// how we split the walls into a mesh

	portal_sz=(int)(((float)ag_settings.map.map_sz)*ag_constant_portal_percent);
	split_factor=(int)(((float)portal_sz)*ag_constant_portal_split_factor_percent);

		// clear the window-wall list

	map_auto_generate_mesh_clear_window_list();

		// start mesh
		
	map_auto_generate_mesh_set_portal_mesh(rn);
	
		// detect if we are on an outside edge of the map
	
	lft_edge=map_auto_generate_portal_on_map_left_edge(rn);
	rgt_edge=map_auto_generate_portal_on_map_right_edge(rn);
	top_edge=map_auto_generate_portal_on_map_top_edge(rn);
	bot_edge=map_auto_generate_portal_on_map_bottom_edge(rn);
	
		// portal height

	ty=portal->min.y;
	by=portal->max.y;
	
		// portal size

	xsz=portal->max.x-portal->min.x;
	zsz=portal->max.z-portal->min.z;

		// build left/right walls

	for (z=0;z<zsz;z+=split_factor) {

			// left

		corridor_edge=FALSE;

		block_portal_idx=map_auto_generate_portal_horz_edge_block(rn,(z+portal->min.z),((z+portal->min.z)+split_factor),portal->min.x);
		if (block_portal_idx!=-1) corridor_edge=ag_boxes[block_portal_idx].corridor_flag!=ag_corridor_flag_portal;

		if ((block_portal_idx==-1) || (corridor_edge)) {
			window_ok=(lft_edge)&&(z!=0)&&(z!=(zsz-split_factor));
			map_auto_generate_portal_walls_add_single(map,rn,0,z,0,(z+split_factor),ty,by,px,py,pz,ag_window_direction_left,block_portal_idx,window_ok);
		}

			// right

		corridor_edge=FALSE;

		block_portal_idx=map_auto_generate_portal_horz_edge_block(rn,(z+portal->min.z),((z+portal->min.z)+split_factor),portal->max.x);
		if (block_portal_idx!=-1) corridor_edge=ag_boxes[block_portal_idx].corridor_flag!=ag_corridor_flag_portal;

		if ((block_portal_idx==-1) || (corridor_edge)) {
			window_ok=(rgt_edge) && (z!=0) && (z!=(zsz-split_factor));
			map_auto_generate_portal_walls_add_single(map,rn,xsz,z,xsz,(z+split_factor),ty,by,px,py,pz,ag_window_direction_right,block_portal_idx,window_ok);
		}
	}

		// build top/bottom walls

	for (x=0;x<xsz;x+=split_factor) {

			// top

		corridor_edge=FALSE;

		block_portal_idx=map_auto_generate_portal_vert_edge_block(rn,(x+portal->min.x),((x+portal->min.x)+split_factor),portal->min.z);
		if (block_portal_idx!=-1) corridor_edge=ag_boxes[block_portal_idx].corridor_flag!=ag_corridor_flag_portal;

		if ((block_portal_idx==-1) || (corridor_edge)) {
			window_ok=(top_edge) && (x!=0) && (x!=(xsz-split_factor));
			map_auto_generate_portal_walls_add_single(map,rn,x,0,(x+split_factor),0,ty,by,px,py,pz,ag_window_direction_top,block_portal_idx,window_ok);
		}

			// bottom

		corridor_edge=FALSE;

		block_portal_idx=map_auto_generate_portal_vert_edge_block(rn,(x+portal->min.x),((x+portal->min.x)+split_factor),portal->max.z);
		if (block_portal_idx!=-1) corridor_edge=ag_boxes[block_portal_idx].corridor_flag!=ag_corridor_flag_portal;

		if ((block_portal_idx==-1) || (corridor_edge)) {
			window_ok=(bot_edge) && (x!=0) && (x!=(xsz-split_factor));
			map_auto_generate_portal_walls_add_single(map,rn,x,zsz,(x+split_factor),zsz,ty,by,px,py,pz,ag_window_direction_bottom,block_portal_idx,window_ok);
		}
	}

		// add any windows put into the list

	map_auto_generate_mesh_add_windows(map);
}

void map_auto_generate_corridor_walls_add(map_type *map,int rn,auto_generate_box_type *portal)
{
	int				xsz,zsz,ty,by,split_factor,slant_sz,portal_sz,
					px[8],py[8],pz[8];
	float			gx[8],gy[8];

		// how we split the walls into a mesh

	portal_sz=(int)(((float)ag_settings.map.map_sz)*ag_constant_portal_percent);
	split_factor=(int)(((float)portal_sz)*ag_constant_portal_split_factor_percent);

		// start mesh
		
	map_auto_generate_mesh_set_portal_mesh(rn);
	
		// portal height

	ty=portal->min.y;
	by=portal->max.y;
	
		// fix cooridors with slants in them
		
	slant_sz=(portal->max.y-portal->min.y)>>2;
	if ((ag_settings.corridor_type==ag_corridor_type_slanted_ceiling) || (ag_settings.corridor_type==ag_corridor_type_octagon)) ty+=slant_sz;
	if (ag_settings.corridor_type==ag_corridor_type_octagon) by-=slant_sz;
	
		// portal size

	xsz=portal->max.x-portal->min.x;
	zsz=portal->max.z-portal->min.z;

		// build left/right walls

	if (portal->corridor_flag==ag_corridor_flag_vertical) {
		map_auto_generate_poly_from_square_wall(0,0,0,zsz,ty,by,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.corridor_wall_ceiling,4,px,py,pz,gx,gy);
		map_auto_generate_mesh_effect_uv_last_poly(map,(float)(zsz/split_factor),1.0f,FALSE);

		map_auto_generate_poly_from_square_wall(xsz,0,xsz,zsz,ty,by,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.corridor_wall_ceiling,4,px,py,pz,gx,gy);
		map_auto_generate_mesh_effect_uv_last_poly(map,(float)(zsz/split_factor),1.0f,FALSE);
	}

		// build top/bottom walls

	if (portal->corridor_flag==ag_corridor_flag_horizontal) {
		map_auto_generate_poly_from_square_wall(0,0,xsz,0,ty,by,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.corridor_wall_ceiling,4,px,py,pz,gx,gy);
		map_auto_generate_mesh_effect_uv_last_poly(map,(float)(xsz/split_factor),1.0f,FALSE);

		map_auto_generate_poly_from_square_wall(0,zsz,xsz,zsz,ty,by,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.corridor_wall_ceiling,4,px,py,pz,gx,gy);
		map_auto_generate_mesh_effect_uv_last_poly(map,(float)(xsz/split_factor),1.0f,FALSE);
	}
	
		// if a corridor, we need to lock uvs
		
	map_auto_generate_mesh_set_lock(map);
}

void map_auto_generate_walls(map_type *map)
{
	int							n;
	auto_generate_box_type		*portal;

		// create surrounding walls for portals

	portal=ag_boxes;

	for (n=0;n!=ag_box_count;n++) {

		if (portal->corridor_flag==ag_corridor_flag_portal) {
			map_auto_generate_portal_walls_add(map,n,portal);
		}
		else {
			map_auto_generate_corridor_walls_add(map,n,portal);
		}

		portal++;
	}
}

/* =======================================================

      Auto-Generate Height Connecting Walls
      
======================================================= */

void map_auto_generate_height_walls(map_type *map)
{
	int							n,k,x,ex,kx,z,ez,kz,xsz,zsz,split_factor,portal_sz,
								px[8],py[8],pz[8];
	auto_generate_box_type		*portal,*chk_portal;

		// how we split the walls into a mesh

	portal_sz=(int)(((float)ag_settings.map.map_sz)*ag_constant_portal_percent);
	split_factor=(int)(((float)portal_sz)*ag_constant_portal_split_factor_percent);

		// create walls for any portals with different heights

	portal=ag_boxes;

	for (n=0;n!=ag_box_count;n++) {

			// only do this to rooms, not corridors

		if (portal->corridor_flag!=ag_corridor_flag_portal) {
			portal++;
			continue;
		}
		
			// portal size

		xsz=portal->max.x-portal->min.x;
		zsz=portal->max.z-portal->min.z;
		
			// check for touching portals
	
		for (k=0;k!=ag_box_count;k++) {
			if (k==n) continue;

				// skip corridors

			chk_portal=&ag_boxes[k];
			if (chk_portal!=ag_corridor_flag_portal) continue;
			
				// only put height connecting walls in rooms that are higher
				
			if (portal->min.y>=chk_portal->min.y) continue;
			
				// mesh
			
			map_auto_generate_mesh_set_portal_mesh(n);
			
				// portals touching top
				
			if (portal->min.z==chk_portal->max.z) {
				x=portal->min.x;
				if (chk_portal->min.x>x) x=chk_portal->min.x;
				ex=portal->max.x;
				if (chk_portal->max.x<ex) ex=chk_portal->max.x;
				if (x>ex) continue;
				
				x-=portal->min.x;
				ex-=portal->min.x;

				for (kx=x;kx<ex;kx+=split_factor) {
					map_auto_generate_portal_walls_add_single(map,n,kx,0,(kx+split_factor),0,portal->min.y,chk_portal->min.y,px,py,pz,0,-1,FALSE);
				}
				continue;
			}
			
				// portals touching bottom
				
			if (portal->max.z==chk_portal->min.z) {
				x=portal->min.x;
				if (chk_portal->min.x>x) x=chk_portal->min.x;
				ex=portal->max.x;
				if (chk_portal->max.x<ex) ex=chk_portal->max.x;
				if (x>ex) continue;
				
				x-=portal->min.x;
				ex-=portal->min.x;

				for (kx=x;kx<ex;kx+=split_factor) {
					map_auto_generate_portal_walls_add_single(map,n,kx,zsz,(kx+split_factor),zsz,portal->min.y,chk_portal->min.y,px,py,pz,0,-1,FALSE);
				}
				continue;
			}
			
				// portals touching left
				
			if (portal->min.x==chk_portal->max.x) {
				z=portal->min.z;
				if (chk_portal->min.z>z) z=chk_portal->min.z;
				ez=portal->max.z;
				if (chk_portal->max.z<ez) ez=chk_portal->max.z;
				if (z>ez) continue;
				
				z-=portal->min.z;
				ez-=portal->min.z;

				for (kz=z;kz<ez;kz+=split_factor) {
					map_auto_generate_portal_walls_add_single(map,n,0,kz,0,(kz+split_factor),portal->min.y,chk_portal->min.y,px,py,pz,0,-1,FALSE);
				}
				continue;
			}
			
				// portals touching right
				
			if (portal->max.x==chk_portal->min.x) {
				z=portal->min.z;
				if (chk_portal->min.z>z) z=chk_portal->min.z;
				ez=portal->max.z;
				if (chk_portal->max.z<ez) ez=chk_portal->max.z;
				if (z>ez) continue;
				
				z-=portal->min.z;
				ez-=portal->min.z;

				for (kz=z;kz<ez;kz+=split_factor) {
					map_auto_generate_portal_walls_add_single(map,n,xsz,kz,xsz,(kz+split_factor),portal->min.y,chk_portal->min.y,px,py,pz,0,-1,FALSE);
				}
				continue;
			}
			
		}
	
		portal++;
	}
}

/* =======================================================

      Auto-Generate Clipping Corridor Walls
      
======================================================= */

void map_auto_generate_corridor_clip_walls(map_type *map)
{
	int							n,xsz,zsz,ty,by,
								px[8],py[8],pz[8],clip_sz;
	float						gx[8],gy[8];
	auto_generate_box_type		*portal;
	
		// add in the clip walls
		
	portal=ag_boxes;

	for (n=0;n!=ag_box_count;n++) {
		
		if (portal->corridor_flag==ag_corridor_flag_portal) {
			portal++;
			continue;
		}
		
			// portal size

		xsz=portal->max.x-portal->min.x;
		zsz=portal->max.z-portal->min.z;
		
		clip_sz=(portal->max.y-portal->min.y)>>2;

			// top walls
			
		if ((ag_settings.corridor_type==ag_corridor_type_slanted_ceiling) || (ag_settings.corridor_type==ag_corridor_type_octagon)) {

			map_auto_generate_mesh_set_portal_mesh(n);

			ty=portal->min.y;
			by=ty+clip_sz;
		
			if (portal->corridor_flag==ag_corridor_flag_horizontal) {
				map_auto_generate_poly_from_bot_trig_wall(0,0,0,clip_sz,ty,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,n,ag_settings.texture.corridor_wall_ceiling,3,px,py,pz,gx,gy);

				map_auto_generate_poly_from_bot_trig_wall(0,zsz,0,(zsz-clip_sz),ty,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,n,ag_settings.texture.corridor_wall_ceiling,3,px,py,pz,gx,gy);

				map_auto_generate_poly_from_bot_trig_wall(xsz,0,xsz,clip_sz,ty,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,n,ag_settings.texture.corridor_wall_ceiling,3,px,py,pz,gx,gy);

				map_auto_generate_poly_from_bot_trig_wall(xsz,zsz,xsz,(zsz-clip_sz),ty,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,n,ag_settings.texture.corridor_wall_ceiling,3,px,py,pz,gx,gy);
			}

			if (portal->corridor_flag==ag_corridor_flag_vertical) {
				map_auto_generate_poly_from_bot_trig_wall(0,0,clip_sz,0,ty,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,n,ag_settings.texture.corridor_wall_ceiling,3,px,py,pz,gx,gy);

				map_auto_generate_poly_from_bot_trig_wall(xsz,0,(xsz-clip_sz),0,ty,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,n,ag_settings.texture.corridor_wall_ceiling,3,px,py,pz,gx,gy);
				
				map_auto_generate_poly_from_bot_trig_wall(0,zsz,clip_sz,zsz,ty,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,n,ag_settings.texture.corridor_wall_ceiling,3,px,py,pz,gx,gy);

				map_auto_generate_poly_from_bot_trig_wall(xsz,zsz,(xsz-clip_sz),zsz,ty,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,n,ag_settings.texture.corridor_wall_ceiling,3,px,py,pz,gx,gy);
			}

		}
		
			// bottom walls
			
		if (ag_settings.corridor_type==ag_corridor_type_octagon) {
		
			ty=portal->max.y-clip_sz;
			by=portal->max.y;
		
			if (portal->corridor_flag==ag_corridor_flag_horizontal) {
				map_auto_generate_poly_from_top_trig_wall(0,0,0,clip_sz,ty,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,n,ag_settings.texture.corridor_wall_ceiling,3,px,py,pz,gx,gy);

				map_auto_generate_poly_from_top_trig_wall(0,zsz,0,(zsz-clip_sz),ty,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,n,ag_settings.texture.corridor_wall_ceiling,3,px,py,pz,gx,gy);

				map_auto_generate_poly_from_top_trig_wall(xsz,0,xsz,clip_sz,ty,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,n,ag_settings.texture.corridor_wall_ceiling,3,px,py,pz,gx,gy);

				map_auto_generate_poly_from_top_trig_wall(xsz,zsz,xsz,(zsz-clip_sz),ty,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,n,ag_settings.texture.corridor_wall_ceiling,3,px,py,pz,gx,gy);
			}

			if (portal->corridor_flag==ag_corridor_flag_vertical) {
				map_auto_generate_poly_from_top_trig_wall(0,0,clip_sz,0,ty,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,n,ag_settings.texture.corridor_wall_ceiling,3,px,py,pz,gx,gy);

				map_auto_generate_poly_from_top_trig_wall(xsz,0,(xsz-clip_sz),0,ty,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,n,ag_settings.texture.corridor_wall_ceiling,3,px,py,pz,gx,gy);
				
				map_auto_generate_poly_from_top_trig_wall(0,zsz,clip_sz,zsz,ty,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,n,ag_settings.texture.corridor_wall_ceiling,3,px,py,pz,gx,gy);

				map_auto_generate_poly_from_top_trig_wall(xsz,zsz,(xsz-clip_sz),zsz,ty,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,n,ag_settings.texture.corridor_wall_ceiling,3,px,py,pz,gx,gy);
			}

		}
			
		portal++;
	}
}

/* =======================================================

      Auto-Generate Ceilings
      
======================================================= */

void map_auto_generate_portal_ceiling_add(map_type *map,int rn,int lx,int lz,int rx,int rz,int ty)
{
	int							portal_sz,split_factor,wall_sz,
								px[8],py[8],pz[8];
	float						gx[8],gy[8];
	auto_generate_box_type		*portal;
		
	portal=&ag_boxes[rn];
	
		// get sizes
		
	portal_sz=(int)(((float)ag_settings.map.map_sz)*ag_constant_portal_percent);
	split_factor=(int)(((float)portal_sz)*ag_constant_portal_split_factor_percent);
	wall_sz=(int)((float)(portal->max.y-portal->min.y)*ag_constant_portal_ceiling_slant_percent);

		// ceilings are part of room

	map_auto_generate_mesh_set_portal_mesh(rn);

		// create the outer ring ceiling
	
	map_auto_generate_poly_from_square_floor(lx,(lz+split_factor),(lx+split_factor),(rz-split_factor),ty,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.portal_ceiling_1,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_floor((rx-split_factor),(lz+split_factor),rx,(rz-split_factor),ty,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.portal_ceiling_1,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_floor((lx+split_factor),lz,(rx-split_factor),(lz+split_factor),ty,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.portal_ceiling_1,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_floor((lx+split_factor),(rz-split_factor),(rx-split_factor),rz,ty,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.portal_ceiling_1,4,px,py,pz,gx,gy);

		// the corners
	
	map_auto_generate_poly_from_square_floor(lx,lz,(lx+split_factor),(lz+split_factor),ty,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.portal_ceiling_1,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_floor(lx,(rz-split_factor),(lx+split_factor),rz,ty,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.portal_ceiling_1,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_floor((rx-split_factor),lz,rx,(lz+split_factor),ty,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.portal_ceiling_1,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_floor((rx-split_factor),(rz-split_factor),rx,rz,ty,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.portal_ceiling_1,4,px,py,pz,gx,gy);

		// create the outer ring walls

	map_auto_generate_poly_from_square_wall((lx+split_factor),(lz+split_factor),(lx+split_factor),(rz-split_factor),(portal->min.y-wall_sz),portal->min.y,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.portal_ceiling_2,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_wall((rx-split_factor),(lz+split_factor),(rx-split_factor),(rz-split_factor),(portal->min.y-wall_sz),portal->min.y,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.portal_ceiling_2,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_wall((lx+split_factor),(lz+split_factor),(rx-split_factor),(lz+split_factor),(portal->min.y-wall_sz),portal->min.y,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.portal_ceiling_2,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_wall((lx+split_factor),(rz-split_factor),(rx-split_factor),(rz-split_factor),(portal->min.y-wall_sz),portal->min.y,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.portal_ceiling_2,4,px,py,pz,gx,gy);

		// top
		
	if (ag_settings.ceiling_type==ag_ceiling_type_closed) {
		map_auto_generate_poly_from_square_floor((lx+split_factor),(lz+split_factor),(rx-split_factor),(rz-split_factor),(portal->min.y-wall_sz),px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.portal_ceiling_1,4,px,py,pz,gx,gy);
	}
}

void map_auto_generate_corridor_ceiling_add(map_type *map,int rn,int lx,int lz,int rx,int rz,int ty)
{
	int							split_factor,lx2,rx2,lz2,rz2,slant_sz,portal_sz,
								px[8],py[8],pz[8];
	float						gx[8],gy[8];
	bool						slant_piece;
	auto_generate_box_type		*portal;
	
		// get sizes
		
	portal_sz=(int)(((float)ag_settings.map.map_sz)*ag_constant_portal_percent);
	split_factor=(int)(((float)portal_sz)*ag_constant_portal_split_factor_percent);
	
	slant_piece=(ag_settings.corridor_type==ag_corridor_type_slanted_ceiling) || (ag_settings.corridor_type==ag_corridor_type_octagon);
		
	   // get portal slants

	portal=&ag_boxes[rn];
	
	slant_sz=(portal->max.y-portal->min.y)>>2;
	
		// create regular ceiling polygon
	
	lz2=lz;
	rz2=rz;
		
	if ((slant_piece) && (portal->corridor_flag==ag_corridor_flag_horizontal)) {
		lz2+=slant_sz;
		rz2-=slant_sz;
	}
	
	lx2=lx;
	rx2=rx;
		
	if ((slant_piece) && (portal->corridor_flag==ag_corridor_flag_vertical)) {
		lx2+=slant_sz;
		rx2-=slant_sz;
	}
	
	map_auto_generate_mesh_set_portal_mesh(rn);
	map_auto_generate_poly_from_square_floor(lx2,lz2,rx2,rz2,ty,px,py,pz,gx,gy);
	if (!map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.corridor_wall_ceiling,4,px,py,pz,gx,gy)) return;
	map_auto_generate_mesh_effect_uv_last_poly(map,(float)((rx-lx)/split_factor),(float)((rz-lz)/split_factor),(portal->corridor_flag==ag_corridor_flag_vertical));
	
		// additional slanted ceilings for corridors
		
	if (slant_piece) {
		
		map_auto_generate_mesh_set_portal_mesh(rn);

		if (portal->corridor_flag==ag_corridor_flag_horizontal) {
			map_auto_generate_poly_from_square_floor_slant(lx,lz,rx,(lz+slant_sz),ty,slant_sz,ag_ceiling_lower_neg_z,TRUE,px,py,pz,gx,gy);
			if (!map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.corridor_wall_ceiling,4,px,py,pz,gx,gy)) return;
			map_auto_generate_mesh_effect_uv_last_poly(map,(float)((rx-lx)/split_factor),1.0f,FALSE);

			map_auto_generate_poly_from_square_floor_slant(lx,(rz-slant_sz),rx,rz,ty,slant_sz,ag_ceiling_lower_pos_z,TRUE,px,py,pz,gx,gy);
			if (!map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.corridor_wall_ceiling,4,px,py,pz,gx,gy)) return;
			map_auto_generate_mesh_effect_uv_last_poly(map,(float)((rx-lx)/split_factor),1.0f,FALSE);
		}
		else {
			map_auto_generate_poly_from_square_floor_slant(lx,lz,(lx+slant_sz),rz,ty,slant_sz,ag_ceiling_lower_neg_x,TRUE,px,py,pz,gx,gy);
			if (!map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.corridor_wall_ceiling,4,px,py,pz,gx,gy)) return;
			map_auto_generate_mesh_effect_uv_last_poly(map,1.0f,(float)((rz-lz)/split_factor),TRUE);

			map_auto_generate_poly_from_square_floor_slant((rx-slant_sz),lz,rx,rz,ty,slant_sz,ag_ceiling_lower_pos_x,TRUE,px,py,pz,gx,gy);
			if (!map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.corridor_wall_ceiling,4,px,py,pz,gx,gy)) return;
			map_auto_generate_mesh_effect_uv_last_poly(map,1.0f,(float)((rz-lz)/split_factor),TRUE);
		}
	}
}

void map_auto_generate_ceilings(map_type *map)
{
	int							n,xsz,zsz;
	auto_generate_box_type		*portal;

	portal=ag_boxes;

	for (n=0;n!=ag_box_count;n++) {

			// portal size

		xsz=portal->max.x-portal->min.x;
		zsz=portal->max.z-portal->min.z;

			// ceilings
			
		if (portal->corridor_flag==ag_corridor_flag_portal) {
			map_auto_generate_portal_ceiling_add(map,n,0,0,xsz,zsz,portal->min.y);
		}
		else {
			map_auto_generate_corridor_ceiling_add(map,n,0,0,xsz,zsz,portal->min.y);
		}
		
		portal++;
	}
}

/* =======================================================

      Auto-Generate Floors 
      
======================================================= */

void map_auto_generate_portal_floor_add(map_type *map,int rn,int lx,int lz,int rx,int rz,int by)
{
	int							px[8],py[8],pz[8];
	float						gx[8],gy[8];
		
	map_auto_generate_mesh_set_portal_mesh(rn);
	map_auto_generate_poly_from_square_floor(lx,lz,rx,rz,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.portal_floor,4,px,py,pz,gx,gy);
}

void map_auto_generate_corridor_floor_add(map_type *map,int rn,int lx,int lz,int rx,int rz,int by)
{
	int							split_factor,lx2,rx2,lz2,rz2,slant_sz,portal_sz,
								px[8],py[8],pz[8];
	float						gx[8],gy[8];
	bool						slant_piece;
	auto_generate_box_type		*portal;
	
		// get sizes
		
	portal_sz=(int)(((float)ag_settings.map.map_sz)*ag_constant_portal_percent);
	split_factor=(int)(((float)portal_sz)*ag_constant_portal_split_factor_percent);
	
	slant_piece=(ag_settings.corridor_type==ag_corridor_type_octagon);
		
	   // get portal slants

	portal=&ag_boxes[rn];
	
	slant_sz=(portal->max.y-portal->min.y)>>2;
	
		// create regular floor polygon

	lz2=lz;
	rz2=rz;
	
	if ((slant_piece) && (portal->corridor_flag==ag_corridor_flag_horizontal)) {
		lz2+=slant_sz;
		rz2-=slant_sz;
	}
	
	lx2=lx;
	rx2=rx;

	if ((slant_piece) && (portal->corridor_flag==ag_corridor_flag_vertical)) {
		lx2+=slant_sz;
		rx2-=slant_sz;
	}

	map_auto_generate_mesh_set_portal_mesh(rn);
	map_auto_generate_poly_from_square_floor(lx2,lz2,rx2,rz2,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.corridor_floor,4,px,py,pz,gx,gy);
	map_auto_generate_mesh_effect_uv_last_poly(map,(float)((rx-lx)/split_factor),(float)((rz-lz)/split_factor),(portal->corridor_flag==ag_corridor_flag_vertical));
	
		// additional slanted floors for corridors
		
	if (slant_piece) {
		
		map_auto_generate_mesh_set_portal_mesh(rn);

		if (portal->corridor_flag==ag_corridor_flag_horizontal) {
			map_auto_generate_poly_from_square_floor_slant(lx,lz,rx,(lz+slant_sz),by,slant_sz,ag_ceiling_lower_neg_z,FALSE,px,py,pz,gx,gy);
			map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.corridor_wall_ceiling,4,px,py,pz,gx,gy);
			map_auto_generate_mesh_effect_uv_last_poly(map,(float)((rx-lx)/split_factor),1.0f,FALSE);

			map_auto_generate_poly_from_square_floor_slant(lx,(rz-slant_sz),rx,rz,by,slant_sz,ag_ceiling_lower_pos_z,FALSE,px,py,pz,gx,gy);
			map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.corridor_wall_ceiling,4,px,py,pz,gx,gy);
			map_auto_generate_mesh_effect_uv_last_poly(map,(float)((rx-lx)/split_factor),1.0f,FALSE);
		}
		else {
			map_auto_generate_poly_from_square_floor_slant(lx,lz,(lx+slant_sz),rz,by,slant_sz,ag_ceiling_lower_neg_x,FALSE,px,py,pz,gx,gy);
			map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.corridor_wall_ceiling,4,px,py,pz,gx,gy);
			map_auto_generate_mesh_effect_uv_last_poly(map,1.0f,(float)((rz-lz)/split_factor),TRUE);

			map_auto_generate_poly_from_square_floor_slant((rx-slant_sz),lz,rx,rz,by,slant_sz,ag_ceiling_lower_pos_x,FALSE,px,py,pz,gx,gy);
			map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.corridor_wall_ceiling,4,px,py,pz,gx,gy);
			map_auto_generate_mesh_effect_uv_last_poly(map,1.0f,(float)((rz-lz)/split_factor),TRUE);
		}
	}
}

void map_auto_generate_floors(map_type *map)
{
	int							n,xsz,zsz;
	auto_generate_box_type		*portal;

	portal=ag_boxes;

	for (n=0;n!=ag_box_count;n++) {

			// portal size

		xsz=portal->max.x-portal->min.x;
		zsz=portal->max.z-portal->min.z;

			// floors and ceilings
			
		if (portal->corridor_flag==ag_corridor_flag_portal) {
			map_auto_generate_portal_floor_add(map,n,0,0,xsz,zsz,portal->max.y);
		}
		else {
			map_auto_generate_corridor_floor_add(map,n,0,0,xsz,zsz,portal->max.y);
		}
		
		portal++;
	}
}

/* =======================================================

      Initialize Flags
      
======================================================= */

void map_auto_generate_clear_flags(void)
{
	int				n;
	
	for (n=0;n!=max_portal;n++) {
		ag_boxes[n].corridor_flag=ag_corridor_flag_portal;
	}
}

/* =======================================================

      Auto-Generate MainLine
      
======================================================= */

void map_auto_generate(map_type *map,auto_generate_settings_type *ags)
{
	int			sz;
	
		// setup global

	memmove(&ag_settings,ags,sizeof(auto_generate_settings_type));

		// clear the map
		
	map_auto_generate_clear(map);

		// some additional setup

	map_auto_generate_random_seed(ag_settings.seed);
	
	sz=ag_settings.map.map_sz/2;

	ag_settings.map.left=(map_max_size/2)-sz;
	ag_settings.map.right=(map_max_size/2)+sz;
	ag_settings.map.top=(map_max_size/2)-sz;
	ag_settings.map.bottom=(map_max_size/2)+sz;
	
	map_auto_generate_clear_flags();

		// create portals

	map_auto_generate_initial_portals(map);

	if (ag_settings.type==ag_type_room_and_corridor) {
		map_auto_generate_merge_portals();
		map_auto_generate_connect_portals(map);
	}
	else {
		map_auto_generate_merge_portals_center();
	}

	map_auto_generate_portal_y();
	
		// walls and floors

	map_auto_generate_walls(map);
	map_auto_generate_height_walls(map);
	map_auto_generate_corridor_clip_walls(map);
	map_auto_generate_floors(map);
	
		// second stories and ceilings
		
	map_auto_generate_second_story(map);
	map_auto_generate_ceilings(map);
	
		// create ramps, steps, doors and frames
		
	map_auto_generate_ramps(map);
	map_auto_generate_corridor_to_portal_connections(map);
	map_auto_generate_doors(map);
		
		// rebuild the UVs
		
	map_auto_generate_reset_UVs(map);
	
		// create lights, nodes, and
		// player spot
		
	map_auto_generate_lights(map);
	map_auto_generate_nodes(map);
	map_auto_generate_add_player_spot(map);
	
		// center map
		
	map_center(map);

		// calculate the normals

	map_recalc_normals(map,FALSE);
}

/* =======================================================

      Auto-Generate Test

	  This is just used to auto generate a map
	  from within engine for testing purposes
      
======================================================= */

bool map_auto_generate_test(map_type *map)
{
	auto_generate_settings_type		ags;

		// start new map

	if (!map_new(map,"DM Map")) return(FALSE);

		// setup textures

	strcpy(map->textures[0].frames[0].name,"Walls/wall_1");
	strcpy(map->textures[1].frames[0].name,"Walls/wall_2");
	strcpy(map->textures[2].frames[0].name,"Floors/floor_1");
	strcpy(map->textures[3].frames[0].name,"Ceilings/ceiling_1");
	strcpy(map->textures[4].frames[0].name,"Ceilings/ceiling_2");
	strcpy(map->textures[5].frames[0].name,"Walls/wall_3");
	strcpy(map->textures[6].frames[0].name,"Floors/floor_3");
	strcpy(map->textures[7].frames[0].name,"Floors/floor_2");
	strcpy(map->textures[8].frames[0].name,"Misc/frame");
	strcpy(map->textures[9].frames[0].name,"Misc/door");
	strcpy(map->textures[10].frames[0].name,"Misc/chain_link");
	strcpy(map->textures[11].frames[0].name,"Misc/stairs");
	strcpy(map->textures[12].frames[0].name,"Misc/ramp");
	strcpy(map->textures[13].frames[0].name,"Misc/column");
	strcpy(map->textures[14].frames[0].name,"Misc/column_base");
	strcpy(map->textures[15].frames[0].name,"Misc/rails");
	strcpy(map->textures[16].frames[0].name,"Misc/water");
	strcpy(map->textures[31].frames[0].name,"Misc/sky");

		// setup auto generation values

#ifndef D3_OS_WINDOWS
	ags.seed=TickCount();
#else
	ags.seed=GetTickCount();
#endif

	ags.type=ag_type_room_and_corridor;
	ags.flow=ag_flow_none;

	ags.map.map_sz=250000;

	ags.light_map=FALSE;
	ags.light_boost=0.7f;

	ags.second_story=TRUE;
	ags.window=TRUE;
	ags.frame=TRUE;
	ags.rails=TRUE;

	ags.ceiling_type=ag_ceiling_type_open;
	ags.corridor_type=ag_corridor_type_octagon;
	ags.door_type=ag_door_type_split;
	ags.stair_type=ag_stair_type_stair;
	
	ags.texture.portal_wall=0;
	ags.texture.portal_wall_2=1;
	ags.texture.portal_floor=2;
	ags.texture.portal_ceiling_1=3;
	ags.texture.portal_ceiling_2=4;
	ags.texture.corridor_wall_ceiling=5;
	ags.texture.corridor_floor=6;
	ags.texture.second_story=7;
	ags.texture.frame=8;
	ags.texture.door=9;
	ags.texture.window=10;
	ags.texture.steps=11;
	ags.texture.ramp=12;
	ags.texture.column=13;
	ags.texture.column_base=14;
	ags.texture.rails=15;
	
	strcpy(ags.sound.door,"Door");
	strcpy(ags.sound.lift,"Lift");

		// create the map

	map_auto_generate(map,&ags);

		// load textures

	if (!map_textures_read(map)) return(FALSE);

	return(TRUE);
}
