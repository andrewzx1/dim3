/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Auto-Generate Second Story Routines

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

extern int map_auto_generate_random_int(int max);

extern bool map_auto_generate_portal_collision(int x,int z,int ex,int ez,int skip_idx,bool corridor_only);
extern bool map_auto_generate_portal_horz_edge_touch(int skip_portal_idx,int z,int ez,int x);
extern bool map_auto_generate_portal_vert_edge_touch(int skip_portal_idx,int x,int ex,int z);
extern bool map_auto_generate_portal_touching_left(int portal_idx);
extern bool map_auto_generate_portal_touching_right(int portal_idx);
extern bool map_auto_generate_portal_touching_top(int portal_idx);
extern bool map_auto_generate_portal_touching_bottom(int portal_idx);

extern void map_auto_generate_poly_from_square_wall(int lx,int lz,int rx,int rz,int ty,int by,int *x,int *y,int *z,float *gx,float *gy);
extern void map_auto_generate_poly_from_square_floor(int lx,int lz,int rx,int rz,int fy,int *x,int *y,int *z,float *gx,float *gy);

extern bool map_auto_generate_mesh_start(map_type *map,int group_idx,int normal_mode,bool moveable);
extern void map_auto_generate_mesh_set_lock(map_type *map);
extern bool map_auto_generate_mesh_add_poly(map_type *map,int box_idx,int txt_idx,int ptsz,int *x,int *y,int *z,float *gx,float *gy);
extern void map_auto_generate_mesh_effect_uv_last_poly(map_type *map,float mult_x,float mult_y,bool rot);

extern int map_auto_generate_steps_get_length(int ty,int by,int step_size,int step_high);
extern void map_auto_generate_steps_mesh(map_type *map,int rn,int step_type,int step_sz,int step_high,int ty,int by,int kx,int kz,float ang_y);
extern void map_auto_generate_lift(map_type *map,int rn,int step_high,int ty,int by,int lx,int rx,int lz,int rz);

extern int							ag_box_count;

extern auto_generate_settings_type	ag_settings;
extern auto_generate_box_type		ag_boxes[max_ag_box];

/* =======================================================

      Determine if Portal has Second Story
      
======================================================= */

bool map_auto_generate_second_story_exist(int rn)
{
		// second story on?

	if (!ag_settings.second_story) return(FALSE);

		// ony rooms (not corridors)

	if (ag_boxes[rn].corridor_flag!=ag_corridor_flag_portal) return(FALSE);

		// and only rooms that are directly connected
		// to another

	if (map_auto_generate_portal_touching_left(rn)) return(TRUE);
	if (map_auto_generate_portal_touching_right(rn)) return(TRUE);
	if (map_auto_generate_portal_touching_top(rn)) return(TRUE);
	if (map_auto_generate_portal_touching_bottom(rn)) return(TRUE);

	return(FALSE);
}

/* =======================================================

      Second Story Pillars
      
======================================================= */

void map_auto_generate_second_story_pillar(map_type *map,int rn,int lx,int lz,int rx,int rz,int ty,int by)
{
	int							k,split_factor,wall_sz,p_ty,p_by,portal_sz,
								px[8],py[8],pz[8];
	float						gx[8],gy[8];
	auto_generate_box_type		*portal;

	portal=&ag_boxes[rn];

		// get sizes
		
	portal_sz=(int)(((float)ag_settings.map.map_sz)*ag_constant_portal_percent);
	split_factor=(int)(((float)portal_sz)*ag_constant_portal_split_factor_percent);
	wall_sz=(int)((float)(portal->max.y-portal->min.y)*ag_constant_portal_ceiling_slant_percent);

		// create pillar

	k=(rx-lx)>>4;
	lx+=k;
	rx-=k;

	k=(rz-lz)>>4;
	lz+=k;
	rz-=k;

	k=wall_sz>>2;
	p_ty=ty+k;
	p_by=by-k;

	if (!map_auto_generate_mesh_start(map,-1,mesh_normal_mode_out,FALSE)) return;
	map_auto_generate_mesh_set_lock(map);

		// top of pillar
		
	map_auto_generate_poly_from_square_wall(lx,lz,rx,lz,ty,p_ty,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.column_base,4,px,py,pz,gx,gy);
	map_auto_generate_mesh_effect_uv_last_poly(map,2,1,FALSE);

	map_auto_generate_poly_from_square_wall(lx,rz,rx,rz,ty,p_ty,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.column_base,4,px,py,pz,gx,gy);
	map_auto_generate_mesh_effect_uv_last_poly(map,2,1,FALSE);

	map_auto_generate_poly_from_square_wall(lx,lz,lx,rz,ty,p_ty,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.column_base,4,px,py,pz,gx,gy);
	map_auto_generate_mesh_effect_uv_last_poly(map,2,1,FALSE);

	map_auto_generate_poly_from_square_wall(rx,lz,rx,rz,ty,p_ty,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.column_base,4,px,py,pz,gx,gy);
	map_auto_generate_mesh_effect_uv_last_poly(map,2,1,FALSE);

	map_auto_generate_poly_from_square_floor(lx,lz,rx,rz,p_ty,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.column_base,4,px,py,pz,gx,gy);
	map_auto_generate_mesh_effect_uv_last_poly(map,2,2,FALSE);

		// bottom of pillar

	map_auto_generate_poly_from_square_wall(lx,lz,rx,lz,p_by,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.column_base,4,px,py,pz,gx,gy);
	map_auto_generate_mesh_effect_uv_last_poly(map,2,1,FALSE);

	map_auto_generate_poly_from_square_wall(lx,rz,rx,rz,p_by,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.column_base,4,px,py,pz,gx,gy);
	map_auto_generate_mesh_effect_uv_last_poly(map,2,1,FALSE);

	map_auto_generate_poly_from_square_wall(lx,lz,lx,rz,p_by,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.column_base,4,px,py,pz,gx,gy);
	map_auto_generate_mesh_effect_uv_last_poly(map,2,1,FALSE);

	map_auto_generate_poly_from_square_wall(rx,lz,rx,rz,p_by,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.column_base,4,px,py,pz,gx,gy);
	map_auto_generate_mesh_effect_uv_last_poly(map,2,1,FALSE);

	map_auto_generate_poly_from_square_floor(lx,lz,rx,rz,p_by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.column_base,4,px,py,pz,gx,gy);
	map_auto_generate_mesh_effect_uv_last_poly(map,2,2,FALSE);

		// pillar

	k=(rx-lx)>>3;
	lx+=k;
	rx-=k;

	k=(rz-lz)>>3;
	lz+=k;
	rz-=k;

	map_auto_generate_poly_from_square_wall(lx,lz,rx,lz,p_ty,p_by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.column,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_wall(lx,rz,rx,rz,p_ty,p_by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.column,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_wall(lx,lz,lx,rz,p_ty,p_by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.column,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_wall(rx,lz,rx,rz,p_ty,p_by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.column,4,px,py,pz,gx,gy);
}

/* =======================================================

      Second Story Railings
      
======================================================= */

void map_auto_generate_railings_support(map_type *map,int rn,int x,int z,int by)
{
	int							ty,px[8],py[8],pz[8];
	float						gx[8],gy[8];

	ty=by-ag_constant_rail_high;

	map_auto_generate_poly_from_square_wall(x,z,x,(z+ag_constant_rail_size),ty,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.rails,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_wall((x+ag_constant_rail_size),z,(x+ag_constant_rail_size),(z+ag_constant_rail_size),ty,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.rails,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_wall(x,z,(x+ag_constant_rail_size),z,ty,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.rails,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_wall(x,(z+ag_constant_rail_size),(x+ag_constant_rail_size),(z+ag_constant_rail_size),ty,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.rails,4,px,py,pz,gx,gy);
}

void map_auto_generate_railings_bar(map_type *map,int rn,int lx,int lz,int rx,int rz,int by)
{
	int							ty,px[8],py[8],pz[8];
	float						gx[8],gy[8];

	by-=ag_constant_rail_high;
	ty=by-ag_constant_rail_size;

	map_auto_generate_poly_from_square_floor(lx,lz,rx,rz,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.rails,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_floor(lx,lz,rx,rz,ty,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.rails,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_wall(lx,lz,rx,lz,ty,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.rails,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_wall(lx,rz,rx,rz,ty,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.rails,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_wall(lx,lz,lx,rz,ty,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.rails,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_wall(rx,lz,rx,rz,ty,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,rn,ag_settings.texture.rails,4,px,py,pz,gx,gy);
}

void map_auto_generate_railings(map_type *map,int rn,int lx,int lz,int rx,int rz,int by)
{
	int							x,z,sz,m_lx,m_rx,m_lz,m_rz;

	if (!ag_settings.rails) return;

		// inside edges of supports

	sz=ag_constant_step_story_size+(ag_constant_step_side_wid*2)+ag_constant_rail_size;

	m_lx=m_rx=(lx+rx)>>1;
	m_lx-=sz;
	m_rx+=sz;

	m_lz=m_rz=(lz+rz)>>1;
	m_lz-=sz;
	m_rz+=sz;

		// x rails

	if ((m_lx-lx)>=(ag_constant_rail_size*15)) {

		z=lz-(ag_constant_rail_size*2);
		
		if (!map_auto_generate_mesh_start(map,-1,mesh_normal_mode_out,FALSE)) return;
		map_auto_generate_railings_support(map,rn,(lx+sz),z,by);
		map_auto_generate_railings_support(map,rn,(m_lx-(ag_constant_rail_size*2)),z,by);
		map_auto_generate_railings_bar(map,rn,((lx+sz)-ag_constant_rail_size),z,m_lx,(z+ag_constant_rail_size),by);

		if (!map_auto_generate_mesh_start(map,-1,mesh_normal_mode_out,FALSE)) return;
		map_auto_generate_railings_support(map,rn,(m_rx+ag_constant_rail_size),z,by);
		map_auto_generate_railings_support(map,rn,(rx-(sz+ag_constant_rail_size)),z,by);
		map_auto_generate_railings_bar(map,rn,m_rx,z,((rx-sz)+ag_constant_rail_size),(z+ag_constant_rail_size),by);

		z=rz+ag_constant_rail_size;
		
		if (!map_auto_generate_mesh_start(map,-1,mesh_normal_mode_out,FALSE)) return;
		map_auto_generate_railings_support(map,rn,(lx+sz),z,by);
		map_auto_generate_railings_support(map,rn,(m_lx-(ag_constant_rail_size*2)),z,by);
		map_auto_generate_railings_bar(map,rn,((lx+sz)-ag_constant_rail_size),z,m_lx,(z+ag_constant_rail_size),by);

		if (!map_auto_generate_mesh_start(map,-1,mesh_normal_mode_out,FALSE)) return;
		map_auto_generate_railings_support(map,rn,(m_rx+ag_constant_rail_size),z,by);
		map_auto_generate_railings_support(map,rn,(rx-(sz+ag_constant_rail_size)),z,by);
		map_auto_generate_railings_bar(map,rn,m_rx,z,((rx-sz)+ag_constant_rail_size),(z+ag_constant_rail_size),by);
	}

		// z rails

	if ((m_lz-lz)>=(ag_constant_rail_size*15)) {
	
		x=lx-(ag_constant_rail_size*2);
		
		if (!map_auto_generate_mesh_start(map,-1,mesh_normal_mode_out,FALSE)) return;
		map_auto_generate_railings_support(map,rn,x,(lz+sz),by);
		map_auto_generate_railings_support(map,rn,x,(m_lz-(ag_constant_rail_size*2)),by);
		map_auto_generate_railings_bar(map,rn,x,((lz+sz)-ag_constant_rail_size),(x+ag_constant_rail_size),m_lz,by);

		if (!map_auto_generate_mesh_start(map,-1,mesh_normal_mode_out,FALSE)) return;
		map_auto_generate_railings_support(map,rn,x,(m_rz+ag_constant_rail_size),by);
		map_auto_generate_railings_support(map,rn,x,(rz-(sz+ag_constant_rail_size)),by);
		map_auto_generate_railings_bar(map,rn,x,m_rz,(x+ag_constant_rail_size),((rz-sz)+ag_constant_rail_size),by);

		x=rx+(ag_constant_rail_size*2);
		
		if (!map_auto_generate_mesh_start(map,-1,mesh_normal_mode_out,FALSE)) return;
		map_auto_generate_railings_support(map,rn,x,(lz+sz),by);
		map_auto_generate_railings_support(map,rn,x,(m_lz-(ag_constant_rail_size*2)),by);
		map_auto_generate_railings_bar(map,rn,x,((lz+sz)-ag_constant_rail_size),(x+ag_constant_rail_size),m_lz,by);

		if (!map_auto_generate_mesh_start(map,-1,mesh_normal_mode_out,FALSE)) return;
		map_auto_generate_railings_support(map,rn,x,(m_rz+ag_constant_rail_size),by);
		map_auto_generate_railings_support(map,rn,x,(rz-(sz+ag_constant_rail_size)),by);
		map_auto_generate_railings_bar(map,rn,x,m_rz,(x+ag_constant_rail_size),((rz-sz)+ag_constant_rail_size),by);
	}
}

/* =======================================================

      Second Story Generate
      
======================================================= */

void map_auto_generate_second_story(map_type *map)
{
	int							n,portal_high,extra_ty,split_factor,step_wid,step_len,sz,
								x,z,ty,by,s_ty,xsz,zsz,portal_sz,px[8],py[8],pz[8];
	float						gx[8],gy[8];
	auto_generate_box_type		*portal;
	
	if (!ag_settings.second_story) return;

		// get sizes
		
	portal_sz=(int)(((float)ag_settings.map.map_sz)*ag_constant_portal_percent);
	portal_high=(int)(((float)portal_sz)*ag_constant_portal_high_percent);
	extra_ty=(int)(((float)portal_high)*ag_constant_portal_high_extra_top);

	split_factor=(int)(((float)portal_sz)*ag_constant_portal_split_factor_percent);

		// create second story in touching portals

	for (n=0;n!=ag_box_count;n++) {

		if (!map_auto_generate_second_story_exist(n)) continue;
		
			// create polygons

		portal=&ag_boxes[n];

		xsz=portal->max.x-portal->min.x;
		zsz=portal->max.z-portal->min.z;

		ty=(map_max_size>>1)-((portal_high>>1)+ag_constant_step_story_size);
		by=ty+ag_constant_story_floor_high;

		sz=split_factor*2;

		if (!map_auto_generate_mesh_start(map,-1,mesh_normal_mode_in_out,FALSE)) return;

			// floors and ceilings corners

		map_auto_generate_poly_from_square_floor(0,0,sz,sz,ty,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,n,ag_settings.texture.second_story,4,px,py,pz,gx,gy);

		map_auto_generate_poly_from_square_floor(0,0,sz,sz,by,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,n,ag_settings.texture.second_story,4,px,py,pz,gx,gy);

		map_auto_generate_poly_from_square_floor((xsz-sz),0,xsz,sz,ty,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,n,ag_settings.texture.second_story,4,px,py,pz,gx,gy);

		map_auto_generate_poly_from_square_floor((xsz-sz),0,xsz,sz,by,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,n,ag_settings.texture.second_story,4,px,py,pz,gx,gy);

		map_auto_generate_poly_from_square_floor(0,(zsz-sz),sz,zsz,ty,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,n,ag_settings.texture.second_story,4,px,py,pz,gx,gy);

		map_auto_generate_poly_from_square_floor(0,(zsz-sz),sz,zsz,by,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,n,ag_settings.texture.second_story,4,px,py,pz,gx,gy);

		map_auto_generate_poly_from_square_floor((xsz-sz),(zsz-sz),xsz,zsz,ty,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,n,ag_settings.texture.second_story,4,px,py,pz,gx,gy);

		map_auto_generate_poly_from_square_floor((xsz-sz),(zsz-sz),xsz,zsz,by,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,n,ag_settings.texture.second_story,4,px,py,pz,gx,gy);

			// floors and ceilings sides

		map_auto_generate_poly_from_square_floor(sz,0,(xsz-sz),sz,ty,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,n,ag_settings.texture.second_story,4,px,py,pz,gx,gy);

		map_auto_generate_poly_from_square_floor(sz,0,(xsz-sz),sz,by,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,n,ag_settings.texture.second_story,4,px,py,pz,gx,gy);

		map_auto_generate_poly_from_square_floor(sz,(zsz-sz),(xsz-sz),zsz,ty,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,n,ag_settings.texture.second_story,4,px,py,pz,gx,gy);

		map_auto_generate_poly_from_square_floor(sz,(zsz-sz),(xsz-sz),zsz,by,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,n,ag_settings.texture.second_story,4,px,py,pz,gx,gy);

		map_auto_generate_poly_from_square_floor(0,sz,sz,(zsz-sz),ty,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,n,ag_settings.texture.second_story,4,px,py,pz,gx,gy);

		map_auto_generate_poly_from_square_floor(0,sz,sz,(zsz-sz),by,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,n,ag_settings.texture.second_story,4,px,py,pz,gx,gy);

		map_auto_generate_poly_from_square_floor((xsz-sz),sz,xsz,(zsz-sz),ty,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,n,ag_settings.texture.second_story,4,px,py,pz,gx,gy);

		map_auto_generate_poly_from_square_floor((xsz-sz),sz,xsz,(zsz-sz),by,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,n,ag_settings.texture.second_story,4,px,py,pz,gx,gy);

			// walls

		map_auto_generate_poly_from_square_wall(sz,sz,(xsz-sz),sz,ty,by,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,n,ag_settings.texture.second_story,4,px,py,pz,gx,gy);

		map_auto_generate_poly_from_square_wall(sz,(zsz-sz),(xsz-sz),(zsz-sz),ty,by,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,n,ag_settings.texture.second_story,4,px,py,pz,gx,gy);

		map_auto_generate_poly_from_square_wall(sz,sz,sz,(zsz-sz),ty,by,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,n,ag_settings.texture.second_story,4,px,py,pz,gx,gy);

		map_auto_generate_poly_from_square_wall((xsz-sz),sz,(xsz-sz),(zsz-sz),ty,by,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,n,ag_settings.texture.second_story,4,px,py,pz,gx,gy);

			// railings

		map_auto_generate_railings(map,n,sz,sz,(xsz-sz),(zsz-sz),ty);

			// position steps so they follow the longest
			// part of the portal and randonly against an edge
			// lifts are always in the corner

		s_ty=ty+(ag_constant_story_floor_high>>1);
		by=portal->max.y;

		step_wid=split_factor*ag_constant_story_steps_split_factor;

			// stairs

		if (ag_settings.stair_type==ag_stair_type_stair) {
			step_len=map_auto_generate_steps_get_length(ty,by,ag_constant_step_story_size,ag_constant_step_story_high);

			if (xsz>zsz) {
				z=(zsz/2)-(step_wid/2);

				if (map_auto_generate_random_int(100)<50) {
					x=split_factor*2;
					map_auto_generate_steps_mesh(map,n,ag_step_second_story,step_wid,ag_constant_step_story_high,s_ty,by,x,z,270.0f);
				}
				else {
					x=xsz-(split_factor*2);
					map_auto_generate_steps_mesh(map,n,ag_step_second_story,step_wid,ag_constant_step_story_high,s_ty,by,x,z,90.0f);
				}
			}
			else {
				x=(xsz/2)-(step_wid/2);

				if (map_auto_generate_random_int(100)<50) {
					z=split_factor*2;
					map_auto_generate_steps_mesh(map,n,ag_step_second_story,step_wid,ag_constant_step_story_high,s_ty,by,x,z,0.0f);
				}
				else {
					z=zsz-(split_factor*2);
					map_auto_generate_steps_mesh(map,n,ag_step_second_story,step_wid,ag_constant_step_story_high,s_ty,by,x,z,180.0f);
				}
			}
		}

			// lifts

		if (ag_settings.stair_type==ag_stair_type_lift) {
			
			switch (map_auto_generate_random_int(4)) {
				case 0:
					x=split_factor*2;
					z=split_factor*2;
					break;
				case 1:
					x=xsz-((split_factor*2)+step_wid);
					z=split_factor*2;
					break;
				case 2:
					x=split_factor*2;
					z=zsz-((split_factor*2)+step_wid);
					break;
				case 3:
					x=xsz-((split_factor*2)+step_wid);
					z=zsz-((split_factor*2)+step_wid);
					break;
			}

			map_auto_generate_lift(map,n,ag_constant_step_story_high,ty,by,x,(x+step_wid),z,(z+step_wid));
		}

			// add first story pillars
			// don't put a pillar if it is too near a corridoor

		ty+=ag_constant_story_floor_high;

			// top-left

		if (!map_auto_generate_portal_collision((portal->min.x-split_factor),(portal->min.z-split_factor),(portal->min.x+(split_factor*3)),(portal->min.z+(split_factor*3)),n,TRUE)) {
			map_auto_generate_second_story_pillar(map,n,split_factor,split_factor,(split_factor+split_factor),(split_factor+split_factor),ty,by);
		}

			// top-right

		if (!map_auto_generate_portal_collision((portal->max.x-(split_factor*3)),(portal->min.z-split_factor),(portal->max.x+split_factor),(portal->min.z+(split_factor*3)),n,TRUE)) {
			map_auto_generate_second_story_pillar(map,n,(xsz-sz),split_factor,((xsz-sz)+split_factor),(split_factor+split_factor),ty,by);
		}

			// bottom-left

		if (!map_auto_generate_portal_collision((portal->min.x-split_factor),(portal->max.z-(split_factor*3)),(portal->min.x+(split_factor*3)),(portal->max.z+split_factor),n,TRUE)) {
			map_auto_generate_second_story_pillar(map,n,split_factor,(zsz-sz),(split_factor+split_factor),((zsz-sz)+split_factor),ty,by);
		}

			// bottom-right

		if (!map_auto_generate_portal_collision((portal->max.x-(split_factor*3)),(portal->max.z-(split_factor*3)),(portal->max.x+split_factor),(portal->max.z+split_factor),n,TRUE)) {
			map_auto_generate_second_story_pillar(map,n,(xsz-sz),(zsz-sz),((xsz-sz)+split_factor),((zsz-sz)+split_factor),ty,by);
		}
	}
}


