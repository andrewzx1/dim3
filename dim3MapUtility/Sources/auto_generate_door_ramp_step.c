/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Auto-Generate Doors, Steps, Ramps, and Windows

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

extern bool map_auto_generate_mesh_start(map_type *map,int group_idx,bool moveable);
extern void map_auto_generate_mesh_set_portal_mesh(int box_idx);
extern void map_auto_generate_mesh_set_lock(map_type *map);
extern void map_auto_generate_mesh_set_rot_offset(map_type *map,int x,int y,int z);
extern bool map_auto_generate_mesh_add_poly(map_type *map,int box_idx,int txt_sz,int ptsz,int *x,int *y,int *z,float *gx,float *gy);
extern void map_auto_generate_mesh_get_last_poly_index(map_type *map,int *mesh_idx,int *poly_idx);
extern void map_auto_generate_mesh_get_poly_points(map_type *map,int mesh_idx,int poly_idx,int *ptsz,int *px,int *py,int *pz);
extern void map_auto_generate_mesh_poly_punch_hole(map_type *map,int mesh_idx,int poly_idx,int x,int z,int txt_idx);

extern void map_auto_generate_poly_from_square_wall(int lx,int lz,int rx,int rz,int ty,int by,int *x,int *y,int *z,float *gx,float *gy);
extern void map_auto_generate_poly_from_top_trig_wall(int lx,int lz,int rx,int rz,int ty,int by,int *x,int *y,int *z,float *gx,float *gy);
extern void map_auto_generate_poly_from_square_floor(int lx,int lz,int rx,int rz,int fy,int *x,int *y,int *z,float *gx,float *gy);
extern void map_auto_generate_poly_from_square_floor_slant(int lx,int lz,int rx,int rz,int fy,int yadd,int lower_mode,bool reverse_slant,int *x,int *y,int *z,float *gx,float *gy);

extern int								ag_box_count,map_ag_mesh_idx;

extern auto_generate_settings_type		ag_settings;
extern auto_generate_box_type			ag_boxes[max_ag_box];

auto_generate_window_wall_type			ag_window_wall_list[ag_max_window_per_portal];

/* =======================================================

      Stair Meshes
      
======================================================= */

int map_auto_generate_steps_get_length(int ty,int by,int step_size,int step_high)
{
	int				step_cnt;
	
	step_cnt=(by-ty)/step_high;
	return(step_cnt*step_size);
}

void map_auto_generate_steps_mesh(map_type *map,int rn,int step_type,int step_sz,int step_high,int ty,int by,int kx,int kz,float ang_y)
{
	int				txt_idx,y,y2,ty2,z,ez,step_wid,px[8],py[8],pz[8];
	float			gx[8],gy[8];
	d3pnt			pt;
	d3ang			ang;
	map_mesh_type	*mesh;

		// create new mesh for steps

	if (!map_auto_generate_mesh_start(map,-1,FALSE)) return;
		
	step_wid=step_sz-ag_constant_step_side_wid;

		// steps or ramps

	if (step_type!=ag_step_ramp) {
		z=0;
		y=ty;

		while (TRUE) {
			y2=y+step_high;

			map_auto_generate_poly_from_square_wall(ag_constant_step_side_wid,(z+ag_constant_step_story_size),step_wid,(z+ag_constant_step_story_size),y,y2,px,py,pz,gx,gy);
			map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.steps,4,px,py,pz,gx,gy);

			map_auto_generate_poly_from_square_floor(ag_constant_step_side_wid,(z+ag_constant_step_story_size),step_wid,z,y,px,py,pz,gx,gy);
			map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.steps,4,px,py,pz,gx,gy);

			z+=ag_constant_step_story_size;

			if (y2>=by) break;

			y=y2;
		}

		txt_idx=ag_settings.texture.steps;
	}
	else {
		z=ag_constant_ramp_length;
		map_auto_generate_poly_from_square_floor_slant(ag_constant_step_side_wid,0,step_wid,z,ty,(by-ty),ag_ceiling_lower_pos_z,TRUE,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.ramp,4,px,py,pz,gx,gy);

		txt_idx=ag_settings.texture.ramp;
	}
	
		// sides
		
	ez=z+ag_constant_step_side_wid;
		
	if (step_type==ag_step_second_story) {
		y=ty-(step_high<<1);
		ty2=by-(step_high<<1);
		
			// back wall
			
		map_auto_generate_poly_from_square_wall(ag_constant_step_side_wid,0,step_wid,0,ty,by,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,rn,txt_idx,4,px,py,pz,gx,gy);

			// left side
			
		map_auto_generate_poly_from_square_floor_slant(0,0,ag_constant_step_side_wid,ez,y,(by-ty),ag_ceiling_lower_pos_z,TRUE,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,rn,txt_idx,4,px,py,pz,gx,gy);

		map_auto_generate_poly_from_square_wall(0,0,0,ez,y,by,px,py,pz,gx,gy);
		py[1]=ty2;
		map_auto_generate_mesh_add_poly(map,rn,txt_idx,4,px,py,pz,gx,gy);

		map_auto_generate_poly_from_square_wall(ag_constant_step_side_wid,0,ag_constant_step_side_wid,ez,y,by,px,py,pz,gx,gy);
		py[1]=ty2;
		map_auto_generate_mesh_add_poly(map,rn,txt_idx,4,px,py,pz,gx,gy);

		map_auto_generate_poly_from_square_wall(0,ez,ag_constant_step_side_wid,ez,ty2,by,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,rn,txt_idx,4,px,py,pz,gx,gy);

		map_auto_generate_poly_from_square_wall(0,0,ag_constant_step_side_wid,0,y,by,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,rn,txt_idx,4,px,py,pz,gx,gy);
		
			// right side
			
		map_auto_generate_poly_from_square_floor_slant(step_wid,0,(step_wid+ag_constant_step_side_wid),ez,y,(by-ty),ag_ceiling_lower_pos_z,TRUE,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,rn,txt_idx,4,px,py,pz,gx,gy);

		map_auto_generate_poly_from_square_wall(step_wid,0,step_wid,ez,y,by,px,py,pz,gx,gy);
		py[1]=ty2;
		map_auto_generate_mesh_add_poly(map,rn,txt_idx,4,px,py,pz,gx,gy);

		map_auto_generate_poly_from_square_wall((step_wid+ag_constant_step_side_wid),0,(step_wid+ag_constant_step_side_wid),ez,y,by,px,py,pz,gx,gy);
		py[1]=ty2;
		map_auto_generate_mesh_add_poly(map,rn,txt_idx,4,px,py,pz,gx,gy);

		map_auto_generate_poly_from_square_wall(step_wid,ez,(step_wid+ag_constant_step_side_wid),ez,ty2,by,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,rn,txt_idx,4,px,py,pz,gx,gy);

		map_auto_generate_poly_from_square_wall(step_wid,0,(step_wid+ag_constant_step_side_wid),0,y,by,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,rn,txt_idx,4,px,py,pz,gx,gy);
	}
	else {
		
			// left side
			
		map_auto_generate_poly_from_square_floor(0,0,ag_constant_step_side_wid,ez,ty,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,rn,txt_idx,4,px,py,pz,gx,gy);

		map_auto_generate_poly_from_square_wall(0,0,0,ez,ty,by,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,rn,txt_idx,4,px,py,pz,gx,gy);

		map_auto_generate_poly_from_square_wall(ag_constant_step_side_wid,0,ag_constant_step_side_wid,ez,ty,by,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,rn,txt_idx,4,px,py,pz,gx,gy);

		map_auto_generate_poly_from_square_wall(0,ez,ag_constant_step_side_wid,ez,ty,by,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,rn,txt_idx,4,px,py,pz,gx,gy);

		if (step_type==ag_step_ramp) {
			map_auto_generate_poly_from_square_wall(0,0,ag_constant_step_side_wid,0,ty,by,px,py,pz,gx,gy);
			map_auto_generate_mesh_add_poly(map,rn,txt_idx,4,px,py,pz,gx,gy);
		}
		
			// right side
			
		map_auto_generate_poly_from_square_floor(step_wid,0,(step_wid+ag_constant_step_side_wid),ez,ty,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,rn,txt_idx,4,px,py,pz,gx,gy);

		map_auto_generate_poly_from_square_wall(step_wid,0,step_wid,ez,ty,by,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,rn,txt_idx,4,px,py,pz,gx,gy);

		map_auto_generate_poly_from_square_wall((step_wid+ag_constant_step_side_wid),0,(step_wid+ag_constant_step_side_wid),ez,ty,by,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,rn,txt_idx,4,px,py,pz,gx,gy);

		map_auto_generate_poly_from_square_wall(step_wid,ez,(step_wid+ag_constant_step_side_wid),ez,ty,by,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,rn,txt_idx,4,px,py,pz,gx,gy);
		
		if (step_type==ag_step_ramp) {
			map_auto_generate_poly_from_square_wall(step_wid,0,(step_wid+ag_constant_step_side_wid),0,ty,by,px,py,pz,gx,gy);
			map_auto_generate_mesh_add_poly(map,rn,txt_idx,4,px,py,pz,gx,gy);
		}
	}
	
		// get to correct position
		// some angles need the steps moved into
		// a certain position before rotating

	mesh=&map->mesh.meshes[map_ag_mesh_idx];
	
	map_prepare_mesh_box(mesh);
	
	switch ((int)ang_y) {
		case 0:
		case 180:
			pt.x=(mesh->box.min.x+mesh->box.max.x)>>1;
			pt.y=0;
			pt.z=mesh->box.min.z;
			break;
		case 90:
			pt.x=mesh->box.min.x;
			pt.y=0;
			pt.z=mesh->box.min.z;
			break;
		case 270:
			pt.x=0;
			pt.y=0;
			pt.z=step_sz;
			map_mesh_move(map,map_ag_mesh_idx,&pt);
			
			map_prepare_mesh_box(mesh);
			
			pt.x=mesh->box.min.x;
			pt.y=0;
			pt.z=mesh->box.min.z;
			break;
	}
	
	ang.x=ang.z=0.0f;
	ang.y=ang_y;
	
	map_mesh_rotate(map,map_ag_mesh_idx,&pt,&ang);
		
	pt.x=kx;
	pt.y=0;
	pt.z=kz;
		
	map_mesh_move(map,map_ag_mesh_idx,&pt);
}

/* =======================================================

      Ramps
      
======================================================= */

void map_auto_generate_ramps_position(int *x,int *ex)
{
	int			mx,wid,split_factor,sz,portal_sz;

	wid=(*ex)-(*x);
	sz=wid>>1;

		// make sure ramp is at least a certain width
		
	portal_sz=(int)(((float)ag_settings.map.map_sz)*ag_constant_portal_percent);
	split_factor=(int)(((float)portal_sz)*ag_constant_portal_split_factor_percent);

	if (sz<split_factor) {
		sz=split_factor;
		if (sz>wid) sz=wid;
	}

		// shift ramp to a side or middle

	switch (map_auto_generate_random_int(3)) {

			// ramp to left

		case 0:
			*ex=(*x)+sz;
			break;

			// ramp to right

		case 1:
			*x=(*ex)-sz;
			break;

			// ramp in middle

		default:
			mx=((*x)+(*ex))/2;
			sz=sz>>1;
			*x=mx-sz;
			*ex=mx+sz;
			break;

	}
}

void map_auto_generate_ramps(map_type *map)
{
	int							n,k,x,ex,z,ez,xsz,zsz,kx,kz,
								split_factor,high,portal_sz,
								px[8],py[8],pz[8];
	float						gx[8],gy[8];
	auto_generate_box_type		*portal,*chk_portal;

		// how we split the walls into a mesh

	portal_sz=(int)(((float)ag_settings.map.map_sz)*ag_constant_portal_percent);
	split_factor=(int)(((float)portal_sz)*ag_constant_portal_split_factor_percent);
	
		// create ramps

	for (n=0;n!=ag_box_count;n++) {

		portal=&ag_boxes[n];
		if (portal->corridor_flag!=ag_corridor_flag_portal) continue;
		
			// portal size

		xsz=portal->max.x-portal->min.x;
		zsz=portal->max.z-portal->min.z;
		
			// check for touching portals
	
		for (k=0;k!=ag_box_count;k++) {
			if (k==n) continue;
			
			chk_portal=&ag_boxes[k];
			if (chk_portal->corridor_flag!=ag_corridor_flag_portal) continue;
			
				// only put ramps in rooms that are lower
				
			if (portal->max.y<=chk_portal->max.y) continue;

			high=portal->max.y-chk_portal->max.y;

				// portals touching top
				
			if (portal->min.z==chk_portal->max.z) {
				x=portal->min.x;
				if (chk_portal->min.x>x) x=chk_portal->min.x;
				ex=portal->max.x;
				if (chk_portal->max.x<ex) ex=chk_portal->max.x;
				if (x>ex) continue;

				x-=portal->min.x;
				ex-=portal->min.x;

					// portal wall

				map_auto_generate_mesh_set_portal_mesh(n);

				for (kx=x;kx<ex;kx+=split_factor) {
					map_auto_generate_poly_from_square_wall(kx,0,(kx+split_factor),0,chk_portal->max.y,portal->max.y,px,py,pz,gx,gy);
					map_auto_generate_mesh_add_poly(map,n,ag_settings.texture.portal_wall,4,px,py,pz,gx,gy);
				}

					// ramp

				if (high>=ag_constant_ramp_min_high) {
					if (!map_auto_generate_mesh_start(map,-1,FALSE)) return;

					map_auto_generate_ramps_position(&x,&ex);
					map_auto_generate_steps_mesh(map,n,ag_step_ramp,(ex-x),ag_constant_step_story_high,chk_portal->max.y,portal->max.y,x,0,0.0f);
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

					// portal wall

				map_auto_generate_mesh_set_portal_mesh(n);
			
				for (kx=x;kx<ex;kx+=split_factor) {
					map_auto_generate_poly_from_square_wall(kx,(portal->max.z-portal->min.z),(kx+split_factor),(portal->max.z-portal->min.z),chk_portal->max.y,portal->max.y,px,py,pz,gx,gy);
					map_auto_generate_mesh_add_poly(map,n,ag_settings.texture.portal_wall,4,px,py,pz,gx,gy);
				}

					// ramp

				if (high>=ag_constant_ramp_min_high) {
					if (!map_auto_generate_mesh_start(map,-1,FALSE)) return;

					map_auto_generate_ramps_position(&x,&ex);
					map_auto_generate_steps_mesh(map,n,ag_step_ramp,(ex-x),ag_constant_step_story_high,chk_portal->max.y,portal->max.y,x,(portal->max.z-portal->min.z),180.0f);
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

					// portal wall

				map_auto_generate_mesh_set_portal_mesh(n);

				for (kz=z;kz<ez;kz+=split_factor) {
					map_auto_generate_poly_from_square_wall(0,kz,0,(kz+split_factor),chk_portal->max.y,portal->max.y,px,py,pz,gx,gy);
					map_auto_generate_mesh_add_poly(map,n,ag_settings.texture.portal_wall,4,px,py,pz,gx,gy);
				}

					// ramp

				if (high>=ag_constant_ramp_min_high) {
					if (!map_auto_generate_mesh_start(map,-1,FALSE)) return;

					map_auto_generate_ramps_position(&z,&ez);
					map_auto_generate_steps_mesh(map,n,ag_step_ramp,(ez-z),ag_constant_step_story_high,chk_portal->max.y,portal->max.y,0,z,270.0f);
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

					// portal wall

				map_auto_generate_mesh_set_portal_mesh(n);
				
				for (kz=z;kz<ez;kz+=split_factor) {
					map_auto_generate_poly_from_square_wall((portal->max.x-portal->min.x),kz,(portal->max.x-portal->min.x),(kz+split_factor),chk_portal->max.y,portal->max.y,px,py,pz,gx,gy);
					map_auto_generate_mesh_add_poly(map,n,ag_settings.texture.portal_wall,4,px,py,pz,gx,gy);
				}

					// ramp

				if (high>=ag_constant_ramp_min_high) {
					if (!map_auto_generate_mesh_start(map,-1,FALSE)) return;

					map_auto_generate_ramps_position(&z,&ez);
					map_auto_generate_steps_mesh(map,n,ag_step_ramp,(ez-z),ag_constant_step_story_high,chk_portal->max.y,portal->max.y,(portal->max.x-portal->min.x),z,90.0f);
				}
				
				continue;
			}
			
		}
	
		portal++;
	}
}

/* =======================================================

      Frames
      
======================================================= */

void map_auto_generate_vert_frame_mesh(map_type *map,int rn,int ty,int by,int x,int z,int frame_sz,bool reverse,bool full_frame,bool window)
{
	int							zadd,y,y2,oy,lx,rx,px[8],py[8],pz[8];
	float						gx[8],gy[8];

	if (!ag_settings.frame) return;

		// create mesh

	if (!map_auto_generate_mesh_start(map,-1,FALSE)) return;

		// direction

	zadd=ag_constant_door_frame_depth;
	if (reverse) zadd=-zadd;

		// outside frame

	y=ty-ag_constant_step_side_wid;
	y2=by+ag_constant_step_side_wid;

	oy=by;
	if (full_frame) oy=y2;

	map_auto_generate_poly_from_square_wall(x,z,x,(z+zadd),y,oy,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.frame,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_wall((x+frame_sz),z,(x+frame_sz),(z+zadd),y,oy,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.frame,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_floor(x,z,(x+frame_sz),(z+zadd),y,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.frame,4,px,py,pz,gx,gy);

	if (full_frame) {
		map_auto_generate_poly_from_square_floor(x,z,(x+frame_sz),(z+zadd),y2,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.frame,4,px,py,pz,gx,gy);
	}

		// inside frame

	lx=x+ag_constant_step_side_wid;
	rx=(x+frame_sz)-ag_constant_step_side_wid;

	map_auto_generate_poly_from_square_wall(lx,z,lx,(z+zadd),ty,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.frame,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_wall(rx,z,rx,(z+zadd),ty,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.frame,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_floor(lx,z,rx,(z+zadd),ty,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.frame,4,px,py,pz,gx,gy);

	if (full_frame) {
		map_auto_generate_poly_from_square_floor(lx,z,rx,(z+zadd),by,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.frame,4,px,py,pz,gx,gy);
	}

		// walls

	map_auto_generate_poly_from_square_wall(lx,(z+zadd),x,(z+zadd),ty,by,px,py,pz,gx,gy);
	py[1]=y;
	if (full_frame) py[2]=y2;
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.frame,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_wall(rx,(z+zadd),(x+frame_sz),(z+zadd),ty,by,px,py,pz,gx,gy);
	py[1]=y;
	if (full_frame) py[2]=y2;
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.frame,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_wall(x,(z+zadd),(x+frame_sz),(z+zadd),y,ty,px,py,pz,gx,gy);
	px[2]+=(rx-(x+frame_sz));
	px[3]+=(lx-x);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.frame,4,px,py,pz,gx,gy);

	if (full_frame) {
		map_auto_generate_poly_from_square_wall(lx,(z+zadd),rx,(z+zadd),y2,by,px,py,pz,gx,gy);
		px[0]+=(rx-(x+frame_sz));
		px[1]+=(lx-x);
		map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.frame,4,px,py,pz,gx,gy);
	}

		// optional window texture

	if (window) {
		map_auto_generate_poly_from_square_wall(lx,z,rx,z,ty,by,px,py,pz,gx,gy);
		gx[0]=gx[3]=gy[0]=gy[1]=0.0f;
		gx[1]=gx[2]=gy[2]=gy[3]=1.0f;

		map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.window,4,px,py,pz,gx,gy);
	}
}

void map_auto_generate_horz_frame_mesh(map_type *map,int rn,int ty,int by,int x,int z,int frame_sz,bool reverse,bool full_frame,bool window)
{
	int							xadd,y,y2,oy,lz,rz,px[8],py[8],pz[8];
	float						gx[8],gy[8];

	if (!ag_settings.frame) return;

		// create mesh

	if (!map_auto_generate_mesh_start(map,-1,FALSE)) return;

		// direction

	xadd=ag_constant_door_frame_depth;
	if (reverse) xadd=-xadd;

		// outside frame

	y=ty-ag_constant_step_side_wid;
	y2=by+ag_constant_step_side_wid;

	oy=by;
	if (full_frame) oy=y2;

	map_auto_generate_poly_from_square_wall(x,z,(x+xadd),z,y,oy,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.frame,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_wall(x,(z+frame_sz),(x+xadd),(z+frame_sz),y,oy,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.frame,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_floor(x,z,(x+xadd),(z+frame_sz),y,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.frame,4,px,py,pz,gx,gy);

	if (full_frame) {
		map_auto_generate_poly_from_square_floor(x,z,(x+xadd),(z+frame_sz),y2,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.frame,4,px,py,pz,gx,gy);
	}

		// inside frame

	lz=z+ag_constant_step_side_wid;
	rz=(z+frame_sz)-ag_constant_step_side_wid;

	map_auto_generate_poly_from_square_wall(x,lz,(x+xadd),lz,ty,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.frame,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_wall(x,rz,(x+xadd),rz,ty,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.frame,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_floor(x,lz,(x+xadd),rz,ty,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.frame,4,px,py,pz,gx,gy);

	if (full_frame) {
		map_auto_generate_poly_from_square_floor(x,lz,(x+xadd),rz,by,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.frame,4,px,py,pz,gx,gy);
	}

		// walls

	map_auto_generate_poly_from_square_wall((x+xadd),lz,(x+xadd),z,ty,by,px,py,pz,gx,gy);
	py[1]=y;
	if (full_frame) py[2]=y2;
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.frame,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_wall((x+xadd),rz,(x+xadd),(z+frame_sz),ty,by,px,py,pz,gx,gy);
	py[1]=y;
	if (full_frame) py[2]=y2;
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.frame,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_wall((x+xadd),z,(x+xadd),(z+frame_sz),y,ty,px,py,pz,gx,gy);
	pz[2]+=(rz-(z+frame_sz));
	pz[3]+=(lz-z);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.frame,4,px,py,pz,gx,gy);

	if (full_frame) {
		map_auto_generate_poly_from_square_wall((x+xadd),lz,(x+xadd),rz,y2,by,px,py,pz,gx,gy);
		pz[0]+=(rz-(z+frame_sz));
		pz[1]+=(lz-z);
		map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.frame,4,px,py,pz,gx,gy);
	}

		// optional window texture

	if (window) {
		map_auto_generate_poly_from_square_wall(x,lz,x,rz,ty,by,px,py,pz,gx,gy);
		gx[0]=gx[3]=gy[0]=gy[1]=0.0f;
		gx[1]=gx[2]=gy[2]=gy[3]=1.0f;

		map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.window,4,px,py,pz,gx,gy);
	}
}

/* =======================================================

      Corridor and Portal Connections
      
======================================================= */

void map_auto_generate_corridor_to_portal_connections(map_type *map)
{
	int							n,k,x,z,door_sz;
	auto_generate_box_type		*portal,*chk_portal;
	
	for (n=0;n!=ag_box_count;n++) {
	
		portal=&ag_boxes[n];
		if (portal->corridor_flag!=ag_corridor_flag_portal) continue;

			// find connecting corridors

		for (k=0;k!=ag_box_count;k++) {
			if (n==k) continue;
			
			chk_portal=&ag_boxes[k];
			if (chk_portal->corridor_flag==ag_corridor_flag_portal) continue;
			
				// steps

			if ((chk_portal->max.y<portal->max.y) && (!chk_portal->corridor_second_story)) {
				if ((chk_portal->min.x>=portal->min.x) && (chk_portal->min.x<=portal->max.x)) {
					x=(chk_portal->min.x-portal->min.x)-ag_constant_door_frame_depth;
					door_sz=(chk_portal->max.x-chk_portal->min.x)+(ag_constant_door_frame_depth<<1);

					if (chk_portal->max.z==portal->min.z) map_auto_generate_steps_mesh(map,n,ag_step_corridor,door_sz,ag_constant_step_corridor_high,chk_portal->max.y,portal->max.y,x,0,0.0f);
					if (chk_portal->min.z==portal->max.z) map_auto_generate_steps_mesh(map,n,ag_step_corridor,door_sz,ag_constant_step_corridor_high,chk_portal->max.y,portal->max.y,x,(portal->max.z-portal->min.z),180.0f);
				}
				
				if ((chk_portal->min.z>=portal->min.z) && (chk_portal->min.z<=chk_portal->max.z)) {
					z=(chk_portal->min.z-portal->min.z)-ag_constant_door_frame_depth;
					door_sz=(chk_portal->max.z-chk_portal->min.z)+(ag_constant_door_frame_depth<<1);

					if (chk_portal->max.x==portal->min.x) map_auto_generate_steps_mesh(map,n,ag_step_corridor,door_sz,ag_constant_step_corridor_high,chk_portal->max.y,portal->max.y,0,z,270.0f);
					if (chk_portal->min.x==portal->max.x) map_auto_generate_steps_mesh(map,n,ag_step_corridor,door_sz,ag_constant_step_corridor_high,chk_portal->max.y,portal->max.y,(portal->max.x-portal->min.x),z,90.0f);
				}
			}

				// frames

			if ((chk_portal->min.x>=portal->min.x) && (chk_portal->min.x<=portal->max.x)) {
				x=(chk_portal->min.x-portal->min.x)-ag_constant_door_frame_depth;
				door_sz=(chk_portal->max.x-chk_portal->min.x)+(ag_constant_door_frame_depth<<1);

				if (chk_portal->max.z==portal->min.z) map_auto_generate_vert_frame_mesh(map,n,chk_portal->min.y,chk_portal->max.y,x,0,door_sz,FALSE,FALSE,FALSE);
				if (chk_portal->min.z==portal->max.z) map_auto_generate_vert_frame_mesh(map,n,chk_portal->min.y,chk_portal->max.y,x,(portal->max.z-portal->min.z),door_sz,TRUE,FALSE,FALSE);
			}
			
			if ((chk_portal->min.z>=portal->min.z) && (chk_portal->min.z<=chk_portal->max.z)) {
				z=(chk_portal->min.z-portal->min.z)-ag_constant_door_frame_depth;
				door_sz=(chk_portal->max.z-chk_portal->min.z)+(ag_constant_door_frame_depth<<1);

				if (chk_portal->max.x==portal->min.x) map_auto_generate_horz_frame_mesh(map,n,chk_portal->min.y,chk_portal->max.y,0,z,door_sz,FALSE,FALSE,FALSE);
				if (chk_portal->min.x==portal->max.x) map_auto_generate_horz_frame_mesh(map,n,chk_portal->min.y,chk_portal->max.y,(portal->max.x-portal->min.x),z,door_sz,TRUE,FALSE,FALSE);
			}

		}
	}
}

/* =======================================================

      Lifts
      
======================================================= */

void map_auto_generate_lift(map_type *map,int rn,int step_high,int ty,int by,int lx,int rx,int lz,int rz)
{
	int						group_idx,movement_idx,move_idx,high,
							px[8],py[8],pz[8];
	float					gx[8],gy[8];
	char					name[256];
	movement_type			*movement;
	movement_move_type		*move;

		// name

	sprintf(name,"Lift_%d",rn);

		// group

	group_idx=map->ngroup;
	map->ngroup++;
		
	strcpy(map->groups[group_idx].name,name);

		// create new mesh for elevator

	high=(by-ty)-step_high;
	ty+=high;
	by+=high;

	if (!map_auto_generate_mesh_start(map,group_idx,TRUE)) return;

	map_auto_generate_poly_from_square_wall(lx,lz,rx,lz,ty,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.steps,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_wall(lx,rz,rx,rz,ty,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.steps,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_wall(lx,lz,lx,rz,ty,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.steps,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_wall(rx,lz,rx,rz,ty,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.steps,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_floor(lx,lz,rx,rz,ty,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.steps,4,px,py,pz,gx,gy);

		// create the movement

	movement_idx=map_movement_add(map);
	movement=&map->movements[movement_idx];
	
	strcpy(movement->name,name);
	movement->group_idx=group_idx;
	movement->auto_open_stand=TRUE;
		
	move_idx=map_movement_move_add(map,movement_idx);

	move=&movement->moves[move_idx];
	move->mov.y=-high;
	move->msec=ag_constant_lift_move_millisec;
	strcpy(move->sound_name,ag_settings.sound.lift);
	
	move_idx=map_movement_move_add(map,movement_idx);

	move=&movement->moves[move_idx];
	move->msec=ag_constant_lift_move_millisec;
}

/* =======================================================

      Doors
      
======================================================= */

int map_auto_generate_doors_horizontal(map_type *map,int rn,int x,int lz,int rz,int rot_off_z,int ty,int by,char *name,char *ext_name)
{
	int			group_idx,px[8],py[8],pz[8];
	float		gx[8],gy[8];

		// group

	group_idx=map->ngroup;
	map->ngroup++;
		
	strcpy(map->groups[group_idx].name,name);
	if (ext_name!=NULL) strcat(map->groups[group_idx].name,ext_name);

		// mesh

	if (!map_auto_generate_mesh_start(map,group_idx,TRUE)) return(-1);
	map_auto_generate_mesh_set_lock(map);
	map_auto_generate_mesh_set_rot_offset(map,0,0,rot_off_z);

		// walls

	map_auto_generate_poly_from_square_wall(x,lz,x,rz,ty,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.door,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_wall((x+ag_constant_door_width),lz,(x+ag_constant_door_width),rz,ty,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.door,4,px,py,pz,gx,gy);
	
	map_auto_generate_poly_from_square_wall(x,lz,(x+ag_constant_door_width),lz,ty,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.door,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_wall(x,rz,(x+ag_constant_door_width),rz,ty,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.door,4,px,py,pz,gx,gy);

		// top and bottom

	map_auto_generate_poly_from_square_floor(x,lz,(x+ag_constant_door_width),rz,ty,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.door,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_floor(x,lz,(x+ag_constant_door_width),rz,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.door,4,px,py,pz,gx,gy);

	return(group_idx);
}

int map_auto_generate_doors_vertical(map_type *map,int rn,int z,int lx,int rx,int rot_off_x,int ty,int by,char *name,char *ext_name)
{
	int			group_idx,px[8],py[8],pz[8];
	float		gx[8],gy[8];

		// group

	group_idx=map->ngroup;
	map->ngroup++;
		
	strcpy(map->groups[group_idx].name,name);
	if (ext_name!=NULL) strcat(map->groups[group_idx].name,ext_name);

		// mesh

	if (!map_auto_generate_mesh_start(map,group_idx,TRUE)) return(-1);
	map_auto_generate_mesh_set_lock(map);
	map_auto_generate_mesh_set_rot_offset(map,rot_off_x,0,0);

		// walls

	map_auto_generate_poly_from_square_wall(lx,z,rx,z,ty,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.door,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_wall(lx,(z+ag_constant_door_width),rx,(z+ag_constant_door_width),ty,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.door,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_wall(lx,z,lx,(z+ag_constant_door_width),ty,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.door,4,px,py,pz,gx,gy);
	
	map_auto_generate_poly_from_square_wall(rx,z,rx,(z+ag_constant_door_width),ty,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.door,4,px,py,pz,gx,gy);
	
		// top and bottom

	map_auto_generate_poly_from_square_floor(lx,z,rx,(z+ag_constant_door_width),ty,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.door,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_floor(lx,z,rx,(z+ag_constant_door_width),by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.door,4,px,py,pz,gx,gy);

	return(group_idx);
}

void map_auto_generate_door_create_movement(map_type *map,int rn,char *name,int group_idx,int reverse_group_idx)
{
	int						movement_idx,move_idx;
	movement_type			*movement;
	movement_move_type		*move;
	auto_generate_box_type	*portal;
	
	portal=&ag_boxes[rn];

			// create the movement
		
	movement_idx=map_movement_add(map);
	movement=&map->movements[movement_idx];
	
	strcpy(movement->name,name);
	movement->group_idx=group_idx;
	if (reverse_group_idx!=-1) movement->reverse_group_idx=reverse_group_idx;
	movement->auto_open=TRUE;
	movement->auto_open_distance=20000;
		
	move_idx=map_movement_move_add(map,movement_idx);
	move=&movement->moves[move_idx];
	
	switch (ag_settings.door_type) {

		case ag_door_type_slide:
			move->mov.y=-(portal->max.y-portal->min.y)-(map_enlarge>>1);
			break;

		case ag_door_type_split:
			if (portal->corridor_flag==ag_corridor_flag_horizontal) {
				move->mov.z=-((portal->max.z-portal->min.z)>>1)-(map_enlarge>>1);
			}
			else {
				move->mov.x=-((portal->max.x-portal->min.x)>>1)-(map_enlarge>>1);
			}
			break;

		case ag_door_type_swing:
			move->rot.y=90.0f;
			break;

	}

	move->msec=ag_constant_door_open_millisec;
	strcpy(move->sound_name,ag_settings.sound.door);
}

void map_auto_generate_doors(map_type *map)
{
	int						n,x,z,xsz,zsz,ty,by,x_rot_off,z_rot_off,
							door_cnt,group_idx,reverse_group_idx;
	char					name[name_str_len];
	auto_generate_box_type	*portal;
	
	if (ag_settings.door_type==ag_door_type_none) return;
	
		// run through corridors
		
	door_cnt=0;
		
	for (n=0;n!=ag_box_count;n++) {
	
		portal=&ag_boxes[n];
	
			// is this a corridor?
			
		if (portal->corridor_flag==ag_corridor_flag_portal) continue;
		
			// enough groups and movements?
			
		if (map->ngroup>=max_group) return;
		if (map->nmovement>=max_movement) return;
		
			// group and movement name

		sprintf(name,"Door%.2d",(door_cnt+1));
		
			// door size
			
		xsz=portal->max.x-portal->min.x;
		zsz=portal->max.z-portal->min.z;

		ty=portal->min.y;
		by=portal->max.y;

			// skip corridors that are too short

		if (portal->corridor_flag==ag_corridor_flag_horizontal) {
			if (xsz<ag_constant_corridor_is_small_size) continue;
		}
		else {
			if (zsz<ag_constant_corridor_is_small_size) continue;
		}
		
			// create door mesh

		group_idx=reverse_group_idx=-1;
			
		if (portal->corridor_flag==ag_corridor_flag_horizontal) {
			
			switch (ag_settings.door_type) {

				case ag_door_type_slide:
					x=ag_constant_door_width*4;
					group_idx=map_auto_generate_doors_horizontal(map,n,x,0,zsz,0,ty,by,name,"_1");
					map_auto_generate_door_create_movement(map,n,name,group_idx,-1);

					x=xsz-(ag_constant_door_width*4);
					group_idx=map_auto_generate_doors_horizontal(map,n,x,0,zsz,0,ty,by,name,"_2");
					map_auto_generate_door_create_movement(map,n,name,group_idx,-1);
					break;

				case ag_door_type_split:
				case ag_door_type_swing:
					z_rot_off=zsz/6;

					x=ag_constant_door_width*4;
					group_idx=map_auto_generate_doors_horizontal(map,n,x,0,(zsz>>1),-z_rot_off,ty,by,name,"_1");
					reverse_group_idx=map_auto_generate_doors_horizontal(map,n,x,(zsz>>1),zsz,z_rot_off,ty,by,name,"_2");
					map_auto_generate_door_create_movement(map,n,name,group_idx,reverse_group_idx);

					x=xsz-(ag_constant_door_width*4);
					group_idx=map_auto_generate_doors_horizontal(map,n,x,0,(zsz>>1),-z_rot_off,ty,by,name,"_3");
					reverse_group_idx=map_auto_generate_doors_horizontal(map,n,x,(zsz>>1),zsz,z_rot_off,ty,by,name,"_4");
					map_auto_generate_door_create_movement(map,n,name,group_idx,reverse_group_idx);
					break;
			}

		}
		else {

			switch (ag_settings.door_type) {

				case ag_door_type_slide:
					z=ag_constant_door_width*4;
					group_idx=map_auto_generate_doors_vertical(map,n,z,0,xsz,0,ty,by,name,"_1");
					map_auto_generate_door_create_movement(map,n,name,group_idx,-1);

					z=zsz-(ag_constant_door_width*4);
					group_idx=map_auto_generate_doors_vertical(map,n,z,0,xsz,0,ty,by,name,"_2");
					map_auto_generate_door_create_movement(map,n,name,group_idx,-1);
					break;

				case ag_door_type_split:
				case ag_door_type_swing:
					x_rot_off=xsz/6;

					z=ag_constant_door_width*4;
					group_idx=map_auto_generate_doors_vertical(map,n,z,0,(xsz>>1),-x_rot_off,ty,by,name,"_1");
					reverse_group_idx=map_auto_generate_doors_vertical(map,n,z,(xsz>>1),xsz,x_rot_off,ty,by,name,"_2");
					map_auto_generate_door_create_movement(map,n,name,group_idx,reverse_group_idx);

					z=zsz-(ag_constant_door_width*4);
					group_idx=map_auto_generate_doors_vertical(map,n,z,0,(xsz>>1),-x_rot_off,ty,by,name,"_3");
					reverse_group_idx=map_auto_generate_doors_vertical(map,n,z,(xsz>>1),xsz,x_rot_off,ty,by,name,"_4");
					map_auto_generate_door_create_movement(map,n,name,group_idx,reverse_group_idx);
					break;
			}
		}
		
			// next door
			
		door_cnt++;
	}
}

/* =======================================================

      Windows
      
======================================================= */

void map_auto_generate_mesh_clear_window_list(void)
{
	int				n;

	for (n=0;n!=ag_max_window_per_portal;n++) {
		ag_window_wall_list[n].mesh_idx=-1;
	}
}

void map_auto_generate_mesh_last_poly_add_window_list(map_type *map,bool window_ok,int rn,int x,int z,int ty,int by,int direction)
{
	int				n,idx;

		// is this a polygon with a window?

	if (!window_ok) return;
	if (!ag_settings.window) return;
	if (map_auto_generate_random_int(100)>(int)(100.0f*ag_constant_window_percent)) return;

		// get next empty spot

	idx=-1;

	for (n=0;n!=ag_max_window_per_portal;n++) {
		if (ag_window_wall_list[n].mesh_idx==-1) {
			idx=n;
			break;
		}
	}

	if (idx==-1) return;

		// add to list

	map_auto_generate_mesh_get_last_poly_index(map,&ag_window_wall_list[idx].mesh_idx,&ag_window_wall_list[idx].poly_idx);

	ag_window_wall_list[idx].rn=rn;
	ag_window_wall_list[idx].x=x;
	ag_window_wall_list[idx].z=z;
	ag_window_wall_list[idx].ty=ty;
	ag_window_wall_list[idx].by=by;
	ag_window_wall_list[idx].direction=direction;
}

void map_auto_generate_mesh_window_vert_frame_mesh(map_type *map,int rn,int ty,int by,int x,int z,int wid,bool reverse)
{
	int			high;

	x+=wid>>2;
	wid=wid>>1;

	x-=ag_constant_door_frame_depth;
	wid+=(ag_constant_door_frame_depth<<1);

	high=(by-ty)>>2;
	ty+=high;
	by-=high;

	map_auto_generate_vert_frame_mesh(map,rn,ty,by,x,z,wid,reverse,TRUE,TRUE);
}

void map_auto_generate_mesh_window_horz_frame_mesh(map_type *map,int rn,int ty,int by,int x,int z,int wid,bool reverse)
{
	int			high;

	z+=wid>>2;
	wid=wid>>1;

	z-=ag_constant_door_frame_depth;
	wid+=(ag_constant_door_frame_depth<<1);

	high=(by-ty)>>2;
	ty+=high;
	by-=high;

	map_auto_generate_horz_frame_mesh(map,rn,ty,by,x,z,wid,reverse,TRUE,TRUE);
}

void map_auto_generate_mesh_add_windows(map_type *map)
{
	int				n,rn,x,z,ty,by,mesh_idx,poly_idx,poly_offset,
					portal_sz,split_factor;

		// get the creation factors

	portal_sz=(int)(((float)ag_settings.map.map_sz)*ag_constant_portal_percent);
	split_factor=(int)(((float)portal_sz)*ag_constant_portal_split_factor_percent);

		// punching holes removes the original poly
		// and adds the new polys to the end of the list
		// so we need to offset the polys as we move through them

	poly_offset=0;

		// run through the window list

	for (n=0;n!=ag_max_window_per_portal;n++) {

			// out of windows?

		if (ag_window_wall_list[n].mesh_idx==-1) break;

			// get settings

		rn=ag_window_wall_list[n].rn;
		x=ag_window_wall_list[n].x;
		z=ag_window_wall_list[n].z;
		ty=ag_window_wall_list[n].ty;
		by=ag_window_wall_list[n].by;

		mesh_idx=ag_window_wall_list[n].mesh_idx;
		poly_idx=ag_window_wall_list[n].poly_idx-poly_offset;

			// build window

		switch (ag_window_wall_list[n].direction) {

			case ag_window_direction_left:
				map_auto_generate_mesh_poly_punch_hole(map,mesh_idx,poly_idx,-ag_constant_window_depth,0,ag_settings.texture.frame);
				map_auto_generate_mesh_window_horz_frame_mesh(map,rn,ty,by,x,z,split_factor,FALSE);
				break;

			case ag_window_direction_right:
				map_auto_generate_mesh_poly_punch_hole(map,mesh_idx,poly_idx,ag_constant_window_depth,0,ag_settings.texture.frame);
				map_auto_generate_mesh_window_horz_frame_mesh(map,rn,ty,by,x,z,split_factor,TRUE);
				break;

			case ag_window_direction_top:
				map_auto_generate_mesh_poly_punch_hole(map,mesh_idx,poly_idx,0,-ag_constant_window_depth,ag_settings.texture.frame);
				map_auto_generate_mesh_window_vert_frame_mesh(map,rn,ty,by,x,z,split_factor,FALSE);
				break;

			case ag_window_direction_bottom:
				map_auto_generate_mesh_poly_punch_hole(map,mesh_idx,poly_idx,0,ag_constant_window_depth,ag_settings.texture.frame);
				map_auto_generate_mesh_window_vert_frame_mesh(map,rn,ty,by,x,z,split_factor,TRUE);
				break;
		}

			// fix next poly_idx for the removed poly

		poly_offset++;
	}

}
