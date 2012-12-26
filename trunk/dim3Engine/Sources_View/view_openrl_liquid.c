/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View OpenRL Map Liquid

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

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "interface.h"

#ifdef D3_OPENRL

#include "ray_interface.h"

extern map_type				map;
extern server_type			server;
extern camera_type			camera;
extern view_type			view;
extern iface_type			iface;
extern setup_type			setup;
extern network_setup_type	net_setup;
extern file_path_setup_type	file_path_setup;

extern int					view_rl_scene_id;
extern float				liquid_normal_cycle_xz[4],
							liquid_normal_cycle_y[4];

extern void liquid_wave_get_high(map_liquid_type *liq,float *wave_y);
extern void liquid_wave_get_normal_x(map_liquid_type *liq,int div,float *wave_y,int lft_add,d3vct *normal);
extern void liquid_wave_get_normal_z(map_liquid_type *liq,int div,float *wave_y,int top_add,d3vct *normal);

/* =======================================================

      OpenRL Liquid Draw Data Setup
      
======================================================= */

void view_openrl_map_liquid_create_draw_data(map_liquid_type *liq)
{
	int				k,div,div_count,
					lft,rgt,top,bot,
					top_add,lft_add;
	float			fy,f_tick,wave_y[4],
					gx,gy,gx2,gy2,gx_add,gy_add;
	float			*vp,*vu,*vn,*vt;
	d3vct			normal;
			
		// setup waves

	div_count=liquid_wave_get_divisions(liq);
	liquid_wave_get_high(liq,wave_y);

		// map pointers
		
	rlSceneMeshMapVertexPointer(view_rl_scene_id,liq->openrl_mesh_id,(void**)&vp);
	rlSceneMeshMapUVPointer(view_rl_scene_id,liq->openrl_mesh_id,(void**)&vu);
	rlSceneMeshMapNormalPointer(view_rl_scene_id,liq->openrl_mesh_id,(void**)&vn);
	rlSceneMeshMapTangentPointer(view_rl_scene_id,liq->openrl_mesh_id,(void**)&vt);

		// get current UVs
		
	f_tick=((float)game_time_get())*0.001f;
		
	gx_add=f_tick*liq->shift.x;
	k=(int)gx_add;
	gx_add=gx_add-(float)k;
	
	gy_add=f_tick*liq->shift.y;
	k=(int)gy_add;
	gy_add=gy_add-(float)k;

	gx=liq->main_uv.offset.x+gx_add;
	gy=liq->main_uv.offset.y+gy_add;

	gx2=gx+liq->main_uv.size.x;
	gy2=gy+liq->main_uv.size.y;

		// get div count and setup
		// the division calculations

	lft=liq->lft;
	rgt=liq->rgt;
	top=liq->top;
	bot=liq->bot;

	lft_add=rgt-lft;
	gx_add=gx2-gx;

	top_add=bot-top;
	gy_add=gy2-gy;

	if (liq->wave.on) {
		if (liq->wave.dir_north_south) {
			top_add=liq->wave.length;
			gy_add=(gy2-gy)/((float)div_count);
		}
		else {
			lft_add=liq->wave.length;
			gx_add=(gx2-gx)/((float)div_count);
		}
	}

		// create the divisions

	for (div=0;div<=div_count;div++) {

		fy=wave_y[div&0x3];

			// north-south (z) waves
		
		if (liq->wave.dir_north_south) {
		
				// the normal

			liquid_wave_get_normal_z(liq,div,wave_y,top_add,&normal);

				// left-top
			
			*vp++=(float)lft;
			*vp++=fy;
			*vp++=(float)top;

			*vu++=gx;
			*vu++=gy;
		
			*vn++=normal.x;
			*vn++=normal.y;
			*vn++=normal.z;

			*vt++=1.0f;
			*vt++=0.0f;
			*vt++=0.0f;

				// right-top

			*vp++=(float)rgt;
			*vp++=fy;
			*vp++=(float)top;

			*vu++=gx2;
			*vu++=gy;
		
			*vn++=normal.x;
			*vn++=normal.y;
			*vn++=normal.z;

			*vt++=1.0f;
			*vt++=0.0f;
			*vt++=0.0f;

				// division change

			top+=top_add;
			if (top>liq->bot) top=liq->bot;

			gy+=gy_add;
			if (gy>gy2) gy=gy2;
		}

			// east-west (x) waves

		else {

				// the normal

			liquid_wave_get_normal_x(liq,div,wave_y,lft_add,&normal);
			
				// left-top

			*vp++=(float)lft;
			*vp++=fy;
			*vp++=(float)top;

			*vu++=gx;
			*vu++=gy;
		
			*vn++=normal.x;
			*vn++=normal.y;
			*vn++=normal.z;
			
			*vt++=1.0f;
			*vt++=0.0f;
			*vt++=0.0f;

				// left-bottom

			*vp++=(float)lft;
			*vp++=fy;
			*vp++=(float)bot;

			*vu++=gx;
			*vu++=gy2;
		
			*vn++=normal.x;
			*vn++=normal.y;
			*vn++=normal.z;
		
			*vt++=1.0f;
			*vt++=0.0f;
			*vt++=0.0f;

				// division changes

			lft+=lft_add;
			if (lft>liq->rgt) lft=liq->rgt;

			gx+=gx_add;
			if (gx>gx2) gx=gx2;
		}
	}

			// unmap the pointers

	rlSceneMeshUnMapVertexPointer(view_rl_scene_id,liq->openrl_mesh_id);
	rlSceneMeshUnMapUVPointer(view_rl_scene_id,liq->openrl_mesh_id);
	rlSceneMeshUnMapNormalPointer(view_rl_scene_id,liq->openrl_mesh_id);
	rlSceneMeshUnMapTangentPointer(view_rl_scene_id,liq->openrl_mesh_id);
}

/* =======================================================

      OpenRL Liquid Mesh Setup
      
======================================================= */

void view_openrl_map_liquid_mesh_start(void)
{
	int					n,k,mesh_id,
						nvertex,div_count;
	short				v_idx;
	short				*vk,*ray_polys;
	map_liquid_type		*liq;
			
		// build the meshes

	for (n=0;n!=map.liquid.nliquid;n++) {
		liq=&map.liquid.liquids[n];

			// add the mesh

		mesh_id=rlSceneMeshAdd(view_rl_scene_id,0);
		if (mesh_id<0) return;

		liq->openrl_mesh_id=mesh_id;

			// setup data sizes

		div_count=liquid_wave_get_divisions(liq);
		nvertex=(div_count+1)*2;
		
		rlSceneMeshSetVertex(view_rl_scene_id,mesh_id,RL_MESH_FORMAT_VERTEX_3_FLOAT,nvertex,NULL);
		rlSceneMeshSetUV(view_rl_scene_id,mesh_id,RL_MESH_FORMAT_UV_2_FLOAT,nvertex,NULL);
		rlSceneMeshSetNormal(view_rl_scene_id,mesh_id,RL_MESH_FORMAT_NORMAL_3_FLOAT,nvertex,NULL);
		rlSceneMeshSetTangent(view_rl_scene_id,mesh_id,RL_MESH_FORMAT_TANGENT_3_FLOAT,nvertex,NULL);

			// add in the data

		view_openrl_map_liquid_create_draw_data(liq);

			// create the polygons
			// liquids are auto-generated, so we actually
			// have a single unique index for all
			// vertex, uv, normal, and tangents

		ray_polys=(short*)malloc((div_count*(2+(4*4)))*sizeof(short));
		vk=ray_polys;

		v_idx=0;

		for (k=0;k!=div_count;k++) {
			*vk++=4;
			*vk++=(short)map.textures[liq->txt_idx].frames[0].bitmap.rl_material_id;

			*vk++=v_idx;			// top-left
			*vk++=v_idx;
			*vk++=v_idx;
			*vk++=v_idx;

			*vk++=v_idx+1;			// bottom-left
			*vk++=v_idx+1;
			*vk++=v_idx+1;
			*vk++=v_idx+1;

			*vk++=v_idx+3;			// bottom-right
			*vk++=v_idx+3;
			*vk++=v_idx+3;
			*vk++=v_idx+3;

			*vk++=v_idx+2;			// top-right
			*vk++=v_idx+2;
			*vk++=v_idx+2;
			*vk++=v_idx+2;

			v_idx+=2;
		}

		rlSceneMeshSetPoly(view_rl_scene_id,mesh_id,RL_MESH_FORMAT_POLY_SHORT_VERTEX_UV_NORMAL_TANGENT,div_count,ray_polys);
		free(ray_polys);

		progress_update();
	}
}

void view_openrl_map_liquid_mesh_stop(void)
{
	int					n;
	map_liquid_type		*liq;
			
		// build the meshes

	for (n=0;n!=map.liquid.nliquid;n++) {
		liq=&map.liquid.liquids[n];
		rlSceneMeshDelete(view_rl_scene_id,liq->openrl_mesh_id);

		progress_update();
	}
}

/* =======================================================

      OpenRL Map Liquid Mesh Update
      
======================================================= */

void view_openrl_map_liquid_mesh_update(void)
{
	int					n;

		// update draw data for liquids
		// (tides, UVs, etc)

	for (n=0;n!=map.liquid.nliquid;n++) {
		view_openrl_map_liquid_create_draw_data(&map.liquid.liquids[n]);
	}
}

#endif