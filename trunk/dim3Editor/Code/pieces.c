/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Piece Routines

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

extern map_type				map;
extern editor_setup_type	setup;
extern editor_state_type	state;

/* =======================================================

      Duplicate Piece
      
======================================================= */

void piece_duplicate_offset(d3pnt *pnt)
{
	editor_view_type		*view;
	
	view=view_get_current_view();
	
	if (fabs(view->ang.x)<15.0f) {
		pnt->x=setup.duplicate_offset*view_snap_clip_size_factor;
		pnt->z=0;
		pnt->y=0;
		rotate_2D_point_center(&pnt->x,&pnt->z,view->ang.y);
		
		return;
	}
	
	pnt->x=setup.duplicate_offset*view_snap_clip_size_factor;
	pnt->y=0;
	pnt->z=setup.duplicate_offset*view_snap_clip_size_factor;
}

void piece_duplicate(void)
{
	int				n,i,nsel_count,type,main_idx,sub_idx,
					index;
	d3pnt			mpt,mov_pt;

	undo_push();
	
		// duplicate selection
		
	select_duplicate_clear();
	
		// get duplicate offset
		
	piece_duplicate_offset(&mov_pt);
	
		// duplicate pieces
		
	nsel_count=select_count();
	
	for (n=0;n!=nsel_count;n++) {
		select_get(n,&type,&main_idx,&sub_idx);
		
		switch (type) {
			
			case mesh_piece:
				index=map_mesh_duplicate(&map,main_idx);
				if (index==-1) {
					os_dialog_alert("Can Not Create Mesh","Not enough memory.");
					return;
				}
				
				map_mesh_calculate_center(&map,index,&mpt);
				map_mesh_move(&map,index,&mov_pt);
				
				select_duplicate_add(mesh_piece,index,0);
				break;
			
			case liquid_piece:
				index=map_liquid_duplicate(&map,main_idx);
				if (index==-1) {
					os_dialog_alert("Can Not Create Liquid","Not enough memory.");
					return;
				}
				
				map_liquid_calculate_center(&map,index,&mpt);
				map_liquid_move(&map,index,&mov_pt);
				
				select_duplicate_add(liquid_piece,index,0);
				break;
				
			case spot_piece:
				if (map.nspot==max_spot) {
					os_dialog_alert("Can Not Create Spot","You've reached the maximum number of spots for a map.");
					return;
				}
				
				map.spots[map.nspot]=map.spots[main_idx];
				map.spots[map.nspot].pnt.x+=mov_pt.x;
				map.spots[map.nspot].pnt.y+=mov_pt.y;
				map.spots[map.nspot].pnt.z+=mov_pt.z;
				select_duplicate_add(spot_piece,map.nspot,0);
				map.nspot++;
				break;
				
			case scenery_piece:
				if (map.nscenery==max_map_scenery) {
					os_dialog_alert("Can Not Create Scenery","You've reached the maximum number of scenery for a map.");
					return;
				}
				
				map.sceneries[map.nscenery]=map.sceneries[main_idx];
				map.sceneries[map.nscenery].pnt.x+=mov_pt.x;
				map.sceneries[map.nscenery].pnt.y+=mov_pt.y;
				map.sceneries[map.nscenery].pnt.z+=mov_pt.z;
				select_duplicate_add(scenery_piece,map.nscenery,0);
				map.nscenery++;
				break;
				
			case node_piece:
				if (map.nnode==max_node) {
					os_dialog_alert("Can Not Create Node","You've reached the maximum number of nodes for a map.");
					return;
				}

				map.nodes[map.nnode]=map.nodes[main_idx];
				map.nodes[map.nnode].pnt.x+=mov_pt.x;
				map.nodes[map.nnode].pnt.y+=mov_pt.y;
				map.nodes[map.nnode].pnt.z+=mov_pt.z;
				for (i=0;i!=max_node_link;i++) {
					map.nodes[map.nnode].link[i]=-1;
				}
				select_duplicate_add(node_piece,map.nnode,0);
				map.nnode++;
				break;
				
			case light_piece:
				if (map.nlight==max_map_light) {
					os_dialog_alert("Can Not Create Light","You've reached the maximum number of lights for a map.");
					return;
				}

				map.lights[map.nlight]=map.lights[main_idx];
				map.lights[map.nlight].pnt.x+=mov_pt.x;
				map.lights[map.nlight].pnt.y+=mov_pt.y;
				map.lights[map.nlight].pnt.z+=mov_pt.z;
				select_duplicate_add(light_piece,map.nlight,0);
				map.nlight++;
				break;
				
			case sound_piece:
				if (map.nsound==max_map_sound) {
					os_dialog_alert("Can Not Create Sound","You've reached the maximum number of sounds for a map.");
					return;
				}

				map.sounds[map.nsound]=map.sounds[main_idx];
				map.sounds[map.nsound].pnt.x+=mov_pt.x;
				map.sounds[map.nsound].pnt.y+=mov_pt.y;
				map.sounds[map.nsound].pnt.z+=mov_pt.z;
				select_duplicate_add(sound_piece,map.nsound,0);
				map.nsound++;
				break;
				
			case particle_piece:
				if (map.nparticle==max_map_particle) {
					os_dialog_alert("Can Not Create Particle","You've reached the maximum number of particles for a map.");
					return;
				}

				map.particles[map.nparticle]=map.particles[main_idx];
				map.particles[map.nparticle].pnt.x+=mov_pt.x;
				map.particles[map.nparticle].pnt.y+=mov_pt.y;
				map.particles[map.nparticle].pnt.z+=mov_pt.z;
				select_duplicate_add(particle_piece,map.nparticle,0);
				map.nparticle++;
				break;
				
		}
    }
	
		// switch selections
		
	select_duplicate_copy();
	
	property_palette_reset();
	
	main_wind_draw();
}

/* =======================================================

      Delete Piece
      
======================================================= */

void piece_delete(void)
{
	int				n,i,k,nsel_count,
					type,main_idx,sub_idx;
	unsigned char	*mesh_mask;
	
	undo_push();
	
		// sort segment so higher indexes are deleted first
		
	select_sort();
	
		// selections have multiple polygons in them
		// so keep a mesh list
	
	mesh_mask=(unsigned char*)malloc(map.mesh.nmesh);
	if (mesh_mask==NULL) return;
	
	bzero(mesh_mask,map.mesh.nmesh);
	
		// delete selection
	
	nsel_count=select_count();
	
	for (n=0;n!=nsel_count;n++) {
		select_get(n,&type,&main_idx,&sub_idx);
	
		switch (type) {
			
			case mesh_piece:
				if (state.drag_mode==drag_mode_polygon) {
					map_mesh_delete_poly(&map,main_idx,sub_idx);
					select_delete_move_index(mesh_piece,main_idx,sub_idx);
					break;
				}
				if (state.drag_mode==drag_mode_mesh) {
					if (mesh_mask[main_idx]==0x0) map_mesh_delete(&map,main_idx);
					mesh_mask[main_idx]=0x1;
					break;
				}
				break;
				
			case liquid_piece:
				map_liquid_delete(&map,main_idx);
				break;
				
			case spot_piece:
				for (i=main_idx;i<map.nspot;i++) {
					map.spots[i]=map.spots[i+1];
				}
				map.nspot--;
				break;
				
			case scenery_piece:
				for (i=main_idx;i<map.nscenery;i++) {
					map.sceneries[i]=map.sceneries[i+1];
				}
				map.nscenery--;
				break;
				
			case node_piece:
				for (i=0;i!=map.nnode;i++) {			// clear all linkage
					if (i==main_idx) continue;

					for (k=0;k!=max_node_link;k++) {
						if (map.nodes[i].link[k]==main_idx) map.nodes[i].link[k]=-1;
						if (map.nodes[i].link[k]>main_idx) map.nodes[i].link[k]--;
					}
				}

				for (i=main_idx;i<map.nnode;i++) {
					map.nodes[i]=map.nodes[i+1];
				}
				
				map.nnode--;
				break;
				
			case light_piece:
				for (i=main_idx;i<map.nlight;i++) {
					map.lights[i]=map.lights[i+1];
				}
				map.nlight--;
				break;
				
			case sound_piece:
				for (i=main_idx;i<map.nsound;i++) {
					map.sounds[i]=map.sounds[i+1];
				}
				map.nsound--;
				break;
				
			case particle_piece:
				for (i=main_idx;i<map.nparticle;i++) {
					map.particles[i]=map.particles[i+1];
				}
				map.nparticle--;
				break;
				
		}
	}
	
	free(mesh_mask);
	
	select_clear();
	
	property_palette_reset();
    
	main_wind_draw();
}

/* =======================================================

      Select More Piece
      
======================================================= */

void piece_select_more_check_edge(d3pnt *k_pt1,d3pnt *k_pt2)
{
	int					n,k,t,t2;
	d3pnt				*pt1,*pt2;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

	mesh=map.mesh.meshes;
	
	for (n=0;n!=map.mesh.nmesh;n++) {
	
		poly=mesh->polys;
		
		for (k=0;k!=mesh->npoly;k++) {
		
			if (select_check(mesh_piece,n,k)) {
				poly++;
				continue;
			}
		
			for (t=0;t!=poly->ptsz;t++) {
				t2=t+1;
				if (t2==poly->ptsz) t2=0;
				
				pt1=&mesh->vertexes[poly->v[t]];
				pt2=&mesh->vertexes[poly->v[t2]];
				
				if (((pt1->x==k_pt1->x) && (pt1->y==k_pt1->y) && (pt1->z==k_pt1->z) && (pt2->x==k_pt2->x) && (pt2->y==k_pt2->y) && (pt2->z==k_pt2->z)) || ((pt1->x==k_pt2->x) && (pt1->y==k_pt2->y) && (pt1->z==k_pt2->z) && (pt2->x==k_pt1->x) && (pt2->y==k_pt1->y) && (pt2->z==k_pt1->z))) {
					select_add(mesh_piece,n,k);
				}
			}
			
			poly++;
		}
		
		mesh++;
	}
}

void piece_select_more(void)
{
	int					n,t,t2,sel_count,type,mesh_idx,poly_idx;
	d3pnt				*pt1,*pt2;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

		// select more polygons
		
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
	
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type!=mesh_piece) continue;
		
		mesh=&map.mesh.meshes[mesh_idx];
		poly=&mesh->polys[poly_idx];
		
		for (t=0;t!=poly->ptsz;t++) {
			t2=t+1;
			if (t2==poly->ptsz) t2=0;
			
			pt1=&mesh->vertexes[poly->v[t]];
			pt2=&mesh->vertexes[poly->v[t2]];
			
			piece_select_more_check_edge(pt1,pt2);
		}
	}
	
	main_wind_draw();
}

/* =======================================================

      Piece Tesselate
      
======================================================= */

void piece_tesselate(bool mesh)
{
	int				n,sel_count,type,mesh_idx,poly_idx;
	
	undo_push();
	
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type==mesh_piece) {
			if (mesh) {
				map_mesh_tesselate(&map,mesh_idx);
			}
			else {
				map_mesh_poly_tesselate(&map,mesh_idx,poly_idx);
			}
		}
	}

	select_clear();
	
	main_wind_draw();
}

/* =======================================================

      Piece Resize and Reposition
      
======================================================= */

void piece_resize(void)
{
	int				n,sel_count,type,mesh_idx,poly_idx;
	float			fct_x,fct_y,fct_z;
	d3pnt			min,max,mpt;
	
		// get the resize factor
		
    if (!dialog_resize_run(&fct_x,&fct_y,&fct_z)) return;
	
	fct_x=fct_x/100.0f;
	fct_y=fct_y/100.0f;
	fct_z=fct_z/100.0f;
	
	if ((fct_x<=0.0f) || (fct_y<=0.0f) || (fct_z<=0.0f)) return;
	
		// resize meshes
		
	undo_push();
		
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		
		if (type==mesh_piece) {
			map_mesh_calculate_extent(&map,mesh_idx,&min,&max);
			map_mesh_calculate_center(&map,mesh_idx,&mpt);
						
			min.x=(int)((float)(min.x-mpt.x)*fct_x)+mpt.x;
			min.y=(int)((float)(min.y-mpt.y)*fct_y)+mpt.y;
			min.z=(int)((float)(min.z-mpt.z)*fct_z)+mpt.z;
			
			max.x=(int)((float)(max.x-mpt.x)*fct_x)+mpt.x;
			max.y=(int)((float)(max.y-mpt.y)*fct_y)+mpt.y;
			max.z=(int)((float)(max.z-mpt.z)*fct_z)+mpt.z;

			map_mesh_resize(&map,mesh_idx,&min,&max);
		}
	}
	
	main_wind_draw();
}

void piece_reposition(void)
{
	int				type,mesh_idx,poly_idx;
	map_mesh_type	*mesh;
	
	if (select_count()==0) return;
	
	select_get(0,&type,&mesh_idx,&poly_idx);
	if (type!=mesh_piece) return;
	
	mesh=&map.mesh.meshes[mesh_idx];
	map_prepare_mesh_box(mesh);
	
		// get the reposition
		
    if (!dialog_reposition_run(&mesh->box.min,&mesh->box.max)) return;
	
		// reposition
		
	undo_push();
	
	map_mesh_resize(&map,mesh_idx,&mesh->box.min,&mesh->box.max);
	
	main_wind_draw();
}

/* =======================================================

      Piece Resize Texture
      
======================================================= */

void piece_resize_texture(void)
{
	int					n,k,t,sel_count,txt_idx,
						type,mesh_idx,poly_idx;
	float				fct_u,fct_v;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	
		// get the resize factor
		
    if (!dialog_resize_texture_run(&fct_u,&fct_v)) return;
	
	fct_u=fct_u/100.0f;
	fct_v=fct_v/100.0f;
	
	if ((fct_u<=0.0f) || (fct_v<=0.0f)) return;

		// get selected texture

	txt_idx=texture_palette_get_selected_texture();
	if (txt_idx==-1) return;
	
		// resize mesh textures
		
	undo_push();
		
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type!=mesh_piece) continue;

			// run through polys and alter
			// polygons hooked up to current texture

		mesh=&map.mesh.meshes[mesh_idx];

		for (k=0;k!=mesh->npoly;k++) {
			poly=&mesh->polys[k];
			if (poly->txt_idx!=txt_idx) continue;

			for (t=0;t!=poly->ptsz;t++) {
				poly->main_uv.x[t]*=fct_u;
				poly->main_uv.y[t]*=fct_v;
			}
		}
	}
	
	main_wind_draw();
}

/* =======================================================

      Piece Transforms
      
======================================================= */

void piece_flip(bool flip_x,bool flip_y,bool flip_z)
{
	int				n,sel_count,type,mesh_idx,poly_idx;
	
	undo_push();
	
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type==mesh_piece) map_mesh_flip(&map,mesh_idx,flip_x,flip_y,flip_z);
	}
	
	main_wind_draw();
}

void piece_rotate(float rot_x,float rot_y,float rot_z)
{
	int				n,sel_count,type,mesh_idx,poly_idx;
	d3pnt			center_pnt;
	d3ang			rot;
	
	undo_push();
	
	sel_count=select_count();
	
	rot.x=rot_x;
	rot.y=rot_y;
	rot.z=rot_z;
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type!=mesh_piece) continue;
		
		map_mesh_calculate_center(&map,mesh_idx,&center_pnt);
		map_mesh_rotate(&map,mesh_idx,&center_pnt,&rot);
	}
	
	main_wind_draw();
}

void piece_free_rotate(void)
{
	float			rot_x,rot_y,rot_z;
	
	if (!dialog_free_rotate_run(&rot_x,&rot_y,&rot_z)) return;
	
	undo_push();
	
	piece_rotate(rot_x,rot_y,rot_z);
}

void piece_move(int move_x,int move_y,int move_z)
{
	int				n,sel_count,type,mesh_idx,poly_idx;
	d3pnt			mov_pnt;
	
	undo_push();
	
	sel_count=select_count();
	
	mov_pnt.x=move_x;
	mov_pnt.y=move_y;
	mov_pnt.z=move_z;
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type==mesh_piece) {
			if (!map.mesh.meshes[mesh_idx].flag.lock_move) map_mesh_move(&map,mesh_idx,&mov_pnt);
		}
	}
	
	main_wind_draw();
}

/* =======================================================

      Select All Polys in Mesh
      
======================================================= */

void piece_mesh_select_all_poly(void)
{
	int				n,k,sel_count,type,mesh_idx,poly_idx;
	map_mesh_type	*mesh;
	
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type!=mesh_piece) continue;
		
		mesh=&map.mesh.meshes[mesh_idx];
		
		for (k=0;k!=mesh->npoly;k++) {
			if (!select_check(mesh_piece,mesh_idx,k)) select_add(mesh_piece,mesh_idx,k);
		}
	}
}

/* =======================================================

      Snap Mesh to Grid
      
======================================================= */

void mesh_snap_to_grid(int mesh_idx)
{
	int						n,nvertex,x,y,z;
	d3pnt					mpt,mpt2;
	d3pnt					*pt;
	map_mesh_type			*mesh;

	mesh=&map.mesh.meshes[mesh_idx];

	map_mesh_calculate_center(&map,mesh_idx,&mpt);
	memmove(&mpt2,&mpt,sizeof(d3pnt));
	view_click_grid(&mpt2);
	
	x=mpt2.x-mpt.x;
	y=mpt2.y-mpt.y;
	z=mpt2.z-mpt.z;
	
	nvertex=mesh->nvertex;
	pt=mesh->vertexes;

	for (n=0;n!=nvertex;n++) {
		pt->x+=x;
		pt->y+=y;
		pt->z+=z;
		pt++;
	}
}

void mesh_poly_snap_to_grid(int mesh_idx,int poly_idx)
{
	int						n;
	d3pnt					*pt;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;

	mesh=&map.mesh.meshes[mesh_idx];
	poly=&mesh->polys[poly_idx];
	
	for (n=0;n!=poly->ptsz;n++) {
		pt=&mesh->vertexes[poly->v[n]];
		view_click_grid(pt);
	}
}

void mesh_vertexes_snap_to_grid(int mesh_idx)
{
	int						n,nvertex;
	d3pnt					*pt;
	map_mesh_type			*mesh;

	mesh=&map.mesh.meshes[mesh_idx];

	nvertex=mesh->nvertex;
	pt=mesh->vertexes;

	for (n=0;n!=nvertex;n++) {
		view_click_grid(pt);
		pt++;
	}
}

void piece_mesh_snap_to_grid(void)
{
	int			n,sel_count,type,mesh_idx,poly_idx;

	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type!=mesh_piece) continue;
		
		mesh_snap_to_grid(mesh_idx);
	}
}

void piece_mesh_snap_closest_vertex(void)
{
	int					n,k,sel_count,type,mesh_idx,poly_idx,
						mesh_1_idx,poly_1_idx,
						mesh_2_idx,poly_2_idx,
						d,dist,x,y,z;
	d3pnt				*pt_1,*pt_2;
	map_mesh_type		*mesh_1,*mesh_2;
	map_mesh_poly_type	*poly_1,*poly_2;

		// find two portals to snap together
		
	mesh_1_idx=mesh_2_idx=-1;
	
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type!=mesh_piece) continue;
		
		if (mesh_1_idx==-1) {
			mesh_1_idx=mesh_idx;
			poly_1_idx=poly_idx;
		}
		else {
			mesh_2_idx=mesh_idx;
			poly_2_idx=poly_idx;
			break;
		}
	}
	
		// two meshes?
		
	if (mesh_2_idx==-1) return;
	
		// find closest vertexes
	
	x=y=z=0;	
	dist=-1;
	
	mesh_1=&map.mesh.meshes[mesh_1_idx];
	mesh_2=&map.mesh.meshes[mesh_2_idx];
	
		// find out of all vertexes
		
	if (poly_1_idx==-1) {
	
		pt_1=mesh_1->vertexes;
	
		for (n=0;n!=mesh_1->nvertex;n++) {

			pt_2=mesh_2->vertexes;
		
			for (k=0;k!=mesh_2->nvertex;k++) {
				d=distance_get(pt_1->x,pt_1->y,pt_1->z,pt_2->x,pt_2->y,pt_2->z);
				if ((d<dist) || (dist==-1)) {
					x=pt_2->x-pt_1->x;
					y=pt_2->y-pt_1->y;
					z=pt_2->z-pt_1->z;
					dist=d;
				}
				pt_2++;
			}
			
			pt_1++;
		}
		
	}
	
		// find out of poly vertexes only
		
	else {
	
		poly_1=&mesh_1->polys[poly_1_idx];
		poly_2=&mesh_2->polys[poly_2_idx];
	
		for (n=0;n!=poly_1->ptsz;n++) {

			pt_1=&mesh_1->vertexes[poly_1->v[n]];
		
			for (k=0;k!=poly_2->ptsz;k++) {
				pt_2=&mesh_2->vertexes[poly_2->v[k]];
				
				d=distance_get(pt_1->x,pt_1->y,pt_1->z,pt_2->x,pt_2->y,pt_2->z);
				if ((d<dist) || (dist==-1)) {
					x=pt_2->x-pt_1->x;
					y=pt_2->y-pt_1->y;
					z=pt_2->z-pt_1->z;
					dist=d;
				}
			}
		}
	
	}
	
		// move together
		
	pt_1=mesh_1->vertexes;
	
	for (n=0;n!=mesh_1->nvertex;n++) {
		pt_1->x+=x;
		pt_1->y+=y;
		pt_1->z+=z;
		pt_1++;
	}
}

void piece_mesh_poly_snap_to_grid(void)
{
	int			n,sel_count,type,mesh_idx,poly_idx;

	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type!=mesh_piece) continue;
		
		mesh_poly_snap_to_grid(mesh_idx,poly_idx);
	}
}

void piece_mesh_vertexes_snap_to_grid(void)
{
	int			n,sel_count,type,mesh_idx,poly_idx;

	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type!=mesh_piece) continue;
		
		mesh_vertexes_snap_to_grid(mesh_idx);
	}
}

/* =======================================================

      Reset/Rotate Mesh UVs
      
======================================================= */

void piece_reset_uvs(bool poly_only)
{
	int						n,sel_count,
							type,mesh_idx,poly_idx;

	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type!=mesh_piece) continue;
		if (map.mesh.meshes[mesh_idx].flag.lock_uv) continue;
		
		if (poly_only) {
			map_mesh_reset_poly_uv(&map,mesh_idx,poly_idx);
		}
		else {
			map_mesh_reset_uv(&map,mesh_idx);
		}
	}
}

void piece_whole_uvs(bool poly_only)
{
	int						n,sel_count,
							type,mesh_idx,poly_idx;

	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type!=mesh_piece) continue;
		
		if (poly_only) {
			map_mesh_whole_poly_uv(&map,mesh_idx,poly_idx);
		}
		else {
			map_mesh_whole_uv(&map,mesh_idx);
		}
	}
}

void piece_single_uvs(bool poly_only)
{
	int						n,sel_count,
							type,mesh_idx,poly_idx;

	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type!=mesh_piece) continue;
		
		if (poly_only) {
			map_mesh_single_poly_uv(&map,mesh_idx,poly_idx);
		}
		else {
			map_mesh_single_uv(&map,mesh_idx);
		}
	}
}

void piece_rotate_uvs(void)
{
	int						n,sel_count,
							type,mesh_idx,poly_idx;

	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type!=mesh_piece) continue;

		map_mesh_rotate_poly_uv(&map,mesh_idx,poly_idx,90);
	}
}

void piece_flip_uvs(bool flip_u,bool flip_v)
{
	int						n,sel_count,
							type,mesh_idx,poly_idx;

	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type!=mesh_piece) continue;

		map_mesh_flip_poly_uv(&map,mesh_idx,poly_idx,flip_u,flip_v);
	}
}

/* =======================================================

      Piece Normals
      
======================================================= */

void piece_mesh_recalc_normals(void)
{
	int				n,sel_count,type,mesh_idx,poly_idx;
	
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type==mesh_piece) map_recalc_normals_mesh(&map.mesh.meshes[mesh_idx],normal_mode_none,FALSE);
	}
	
	main_wind_draw();
}

void piece_mesh_poly_invert_normals(map_mesh_poly_type *poly)
{
	poly->tangent_space.normal.x=-poly->tangent_space.normal.x;
	poly->tangent_space.normal.y=-poly->tangent_space.normal.y;
	poly->tangent_space.normal.z=-poly->tangent_space.normal.z;
}
	
void piece_mesh_invert_normals(bool poly_only)
{
	int				n,k,sel_count,type,mesh_idx,poly_idx;
	map_mesh_type	*mesh;
	
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type!=mesh_piece) continue;
		
			// invert the poly
			
		mesh=&map.mesh.meshes[mesh_idx];
		
		if (poly_only) {
			piece_mesh_poly_invert_normals(&mesh->polys[poly_idx]);
		}
		else {
			for (k=0;k!=mesh->npoly;k++) {
				piece_mesh_poly_invert_normals(&mesh->polys[k]);
			}
		}
	}
	
	main_wind_draw();
}

void piece_mesh_set_normals_in_out(bool out)
{
	int					n,sel_count,normal_mode,
						type,mesh_idx,poly_idx;
	map_mesh_type		*mesh;

	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type!=mesh_piece) continue;
		
			// recalc the normals
			
		mesh=&map.mesh.meshes[mesh_idx];

		normal_mode=out?normal_mode_out:normal_mode_in;
		map_recalc_normals_mesh(&map.mesh.meshes[mesh_idx],normal_mode,FALSE);
	}
}

/* =======================================================

      Piece Holes
      
======================================================= */

void piece_poly_hole(void)
{
	int				n,sel_count,type,mesh_idx,poly_idx;
	
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type==mesh_piece) map_mesh_poly_punch_hole(&map,mesh_idx,poly_idx,NULL);
	}
	
	select_clear();
	
	main_wind_draw();
}

/* =======================================================

      Piece Keys
      
======================================================= */

void piece_key(char ch)
{
	int					n,sel_count,type,main_idx,sub_idx,mv;
	d3pnt				move_pnt;
	editor_view_type	*view;
	
		// special check for delete key
		
	if ((ch==D3_KEY_BACKSPACE) || (ch==D3_KEY_DELETE)) {
		piece_delete();
		item_palette_reset();
		property_palette_reset();
		main_wind_draw();
		menu_fix_enable();
		return;
	}
	
		// nudge works with grid
	
	mv=view_get_grid()*move_key_scale;
	if (!os_key_shift_down()) mv/=move_key_shift_reduce_scale;
	
	if (mv<1) mv=1;
	
		// nudge keys movement

	view=view_get_current_view();
	move_pnt.x=move_pnt.y=move_pnt.z=0;
		
	switch (ch) {
	
		case D3_KEY_LEFT:
			view_mouse_get_scroll_horizontal_axis(view,&move_pnt,-mv);
			break;
			
		case D3_KEY_RIGHT:
			view_mouse_get_scroll_horizontal_axis(view,&move_pnt,+mv);
			break;
			
		case D3_KEY_UP:
			view_mouse_get_scroll_vertical_axis(view,&move_pnt,-mv);
			break;
			
		case D3_KEY_DOWN:
			view_mouse_get_scroll_vertical_axis(view,&move_pnt,mv);
			break;
			
	}
	
	if ((move_pnt.x==0) && (move_pnt.y==0) && (move_pnt.z==0)) return;
	
		// move selection
	
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&main_idx,&sub_idx);

		switch (type) {
		
			case mesh_piece:
				if (map.mesh.meshes[main_idx].flag.lock_move) break;
				map_mesh_move(&map,main_idx,&move_pnt);
				break;
				
			case liquid_piece:
				map_liquid_move(&map,main_idx,&move_pnt);
				break;
				
			case node_piece:
				map.nodes[main_idx].pnt.x+=move_pnt.x;
				map.nodes[main_idx].pnt.y+=move_pnt.y;
				map.nodes[main_idx].pnt.z+=move_pnt.z;
				break;
				
			case spot_piece:
				map.spots[main_idx].pnt.x+=move_pnt.x;
				map.spots[main_idx].pnt.y+=move_pnt.y;
				map.spots[main_idx].pnt.z+=move_pnt.z;
				break;
				
			case scenery_piece:
				map.sceneries[main_idx].pnt.x+=move_pnt.x;
				map.sceneries[main_idx].pnt.y+=move_pnt.y;
				map.sceneries[main_idx].pnt.z+=move_pnt.z;
				break;
				
			case light_piece:
				map.lights[main_idx].pnt.x+=move_pnt.x;
				map.lights[main_idx].pnt.y+=move_pnt.y;
				map.lights[main_idx].pnt.z+=move_pnt.z;
				break;
				
			case sound_piece:
				map.sounds[main_idx].pnt.x+=move_pnt.x;
				map.sounds[main_idx].pnt.y+=move_pnt.y;
				map.sounds[main_idx].pnt.z+=move_pnt.z;
				break;
				
			case particle_piece:
				map.particles[main_idx].pnt.x+=move_pnt.x;
				map.particles[main_idx].pnt.y+=move_pnt.y;
				map.particles[main_idx].pnt.z+=move_pnt.z;
				break;
				
		}
	}
	
	main_wind_draw();
}

