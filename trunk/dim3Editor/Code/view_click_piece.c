/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Walk View Piece Click Routines

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
#include "ui_common.h"

extern map_type					map;
extern editor_setup_type		setup;
extern editor_state_type		state;

extern list_palette_type		property_palette;

/* =======================================================

      Grid and Snap
      
======================================================= */

int view_get_grid(void)
{
	switch (state.grid_mode) {
		case grid_mode_small:
			return(576);				// historical based on the old 144 number
		case grid_mode_large:
			return(2304);
	}

	return(1);
}

void view_click_grid(d3pnt *pt)
{
	int			sz;
	
	sz=view_get_grid();
	
	pt->x/=sz;
	pt->y/=sz;
	pt->z/=sz;
	
	pt->x*=sz;
	pt->y*=sz;
	pt->z*=sz;
}

bool view_click_snap(int mesh_idx,int liquid_idx,d3pnt *pt)
{
	int				n,t,y,snap_sz;
	d3pnt			*dpt;
	map_mesh_type	*mesh;
	map_liquid_type	*liq;
	
	if (state.vertex_mode!=vertex_mode_snap) return(FALSE);
	
		// snap size
		
	snap_sz=(setup.snap_size*view_snap_clip_size_factor);
	
		// any mesh vertexes to snap to?
		
	mesh=map.mesh.meshes;
		
	for (n=0;n!=map.mesh.nmesh;n++) {
		
		if (n==mesh_idx) {
			mesh++;
			continue;
		}

		dpt=mesh->vertexes;
		
		for (t=0;t!=mesh->nvertex;t++) {
		
			if (abs(dpt->x-pt->x)>snap_sz) {
				dpt++;
				continue;
			}
			
			if (abs(dpt->z-pt->z)>snap_sz) {
				dpt++;
				continue;
			}
			
			if (abs(dpt->y-pt->y)>snap_sz) {
				dpt++;
				continue;
			}
		
			if (distance_get(dpt->x,dpt->y,dpt->z,pt->x,pt->y,pt->z)<snap_sz) {
				memmove(pt,dpt,sizeof(d3pnt));
				return(TRUE);
			}
			
			dpt++;
		}
	
		mesh++;
	}

		// any liquid vertexes to snap to?

	liq=map.liquid.liquids;

	for (n=0;n!=map.liquid.nliquid;n++) {

		if (n==liquid_idx) {
			liq++;
			continue;
		}

		if (distance_get(liq->lft,liq->y,liq->top,pt->x,pt->y,pt->z)<snap_sz) {
			pt->x=liq->lft;
			pt->y=liq->y;
			pt->z=liq->top;
			return(TRUE);
		}

		if (distance_get(liq->rgt,liq->y,liq->top,pt->x,pt->y,pt->z)<snap_sz) {
			pt->x=liq->rgt;
			pt->y=liq->y;
			pt->z=liq->top;
			return(TRUE);
		}

		if (distance_get(liq->lft,liq->y,liq->bot,pt->x,pt->y,pt->z)<snap_sz) {
			pt->x=liq->lft;
			pt->y=liq->y;
			pt->z=liq->bot;
			return(TRUE);
		}

		if (distance_get(liq->rgt,liq->y,liq->bot,pt->x,pt->y,pt->z)<snap_sz) {
			pt->x=liq->rgt;
			pt->y=liq->y;
			pt->z=liq->bot;
			return(TRUE);
		}

		y=liq->y+liq->depth;

		if (distance_get(liq->lft,y,liq->top,pt->x,pt->y,pt->z)<snap_sz) {
			pt->x=liq->lft;
			pt->y=y;
			pt->z=liq->top;
			return(TRUE);
		}

		if (distance_get(liq->rgt,y,liq->top,pt->x,pt->y,pt->z)<snap_sz) {
			pt->x=liq->rgt;
			pt->y=y;
			pt->z=liq->top;
			return(TRUE);
		}

		if (distance_get(liq->lft,y,liq->bot,pt->x,pt->y,pt->z)<snap_sz) {
			pt->x=liq->lft;
			pt->y=y;
			pt->z=liq->bot;
			return(TRUE);
		}

		if (distance_get(liq->rgt,y,liq->bot,pt->x,pt->y,pt->z)<snap_sz) {
			pt->x=liq->rgt;
			pt->y=y;
			pt->z=liq->bot;
			return(TRUE);
		}
		
		liq++;
	}
	
	return(FALSE);
}

bool view_click_snap_poly(int mesh_idx,int poly_idx,d3pnt *pt)
{
	int					n,d,cur_dist;
	d3pnt				hpt;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	
	cur_dist=-1;
	
	mesh=&map.mesh.meshes[mesh_idx];
	poly=&mesh->polys[poly_idx];
	
	for (n=0;n!=poly->ptsz;n++) {
		memmove(&hpt,pt,sizeof(d3pnt));
		
		if (view_click_snap(mesh_idx,-1,&hpt)) {
			d=distance_get(pt->x,pt->y,pt->z,hpt.x,hpt.y,hpt.z);
			if (d>(setup.snap_size*view_snap_clip_size_factor)) continue;
			
			if ((d<cur_dist) || (cur_dist<0)) {
				cur_dist=d;
				memmove(pt,&hpt,sizeof(d3pnt));
			}
		}
	}
	
	return(cur_dist!=-1);
}

bool view_click_snap_mesh(int mesh_idx,d3pnt *pt)
{
	int					n,d,cur_dist;
	d3pnt				hpt;
	map_mesh_type		*mesh;
	
	cur_dist=-1;
	
	mesh=&map.mesh.meshes[mesh_idx];
	
	for (n=0;n!=mesh->nvertex;n++) {
		memmove(&hpt,pt,sizeof(d3pnt));
		
		if (view_click_snap(mesh_idx,-1,&hpt)) {
			d=distance_get(pt->x,pt->y,pt->z,hpt.x,hpt.y,hpt.z);
			if (d>(setup.snap_size*view_snap_clip_size_factor)) continue;
			
			if ((d<cur_dist) || (cur_dist<0)) {
				cur_dist=d;
				memmove(pt,&hpt,sizeof(d3pnt));
			}
		}
	}
	
	return(cur_dist!=-1);
}

/* =======================================================

      Rot Handle Rotation Run
      
======================================================= */

void view_click_rot_handle_rotate_run_axis(float *value,float org_value,float ang_add)
{
	int				k;
	
	*value=angle_add(org_value,ang_add);
	
	if (os_key_shift_down()) {
		k=(int)*value;
		k/=45;
		*value=(float)(k*45);
	}
}

void view_click_rot_handle_rotate_run(d3ang *ang,d3ang *org_ang,float ang_add,int which_axis)
{
	switch (which_axis) {
		case 0:
			view_click_rot_handle_rotate_run_axis(&ang->x,org_ang->x,ang_add);
			break;
		case 1:
			view_click_rot_handle_rotate_run_axis(&ang->y,org_ang->y,ang_add);
			break;
		case 2:
			view_click_rot_handle_rotate_run_axis(&ang->z,org_ang->z,ang_add);
			break;
	}
}

/* =======================================================

      Rot Handle Move Run
      
======================================================= */

void view_click_rot_handle_move_run_axis(int *value,int org_value,int mv_add)
{
	*value=org_value+mv_add;
}

void view_click_rot_handle_move_run(d3pnt *pnt,d3pnt *org_pnt,int mv_add,int which_axis)
{
	mv_add=-(mv_add*view_handle_move_scale);
	
	switch (which_axis) {
		case 0:
			view_click_rot_handle_move_run_axis(&pnt->x,org_pnt->x,mv_add);
			break;
		case 1:
			view_click_rot_handle_move_run_axis(&pnt->y,org_pnt->y,mv_add);
			break;
		case 2:
			view_click_rot_handle_move_run_axis(&pnt->z,org_pnt->z,mv_add);
			break;
	}
}

/* =======================================================

      Rot Handle Clicking Main
      
======================================================= */

bool view_click_rot_handles(editor_view_type *view,d3pnt *click_pt)
{
	int			n,k,idx,type,main_idx,sub_idx,which_axis,sel_count,item_count;
	int			*type_list,*idx_list;
	bool		first_drag;
	d3pnt		pt,center_pnt,*hand_pnts,
				*pnt,org_pnt;
	d3ang		*ang,org_ang;
	d3rect		box;

		// need to create the items outside
		// of the pick call so rendering doesn't get
		// reset on us
		
		// first count the items

	item_count=0;
	sel_count=select_count();

	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&main_idx,&sub_idx);
		if ((type==node_piece) || (type==spot_piece) || (type==scenery_piece)) item_count++;
	}

	if (item_count==0) return(FALSE);
	
		// build the item list
		
	hand_pnts=(d3pnt*)malloc(sizeof(d3pnt)*(item_count*3));
	type_list=(int*)malloc(sizeof(int)*item_count);
	idx_list=(int*)malloc(sizeof(int)*item_count);
	
		// get the handles
		
	item_count=0;
	
	for (n=0;n!=sel_count;n++) {
	
		select_get(n,&type,&main_idx,&sub_idx);
		
		switch (type) {
		
			case node_piece:
				if (view_handle_create_rot_handle(view,&map.nodes[main_idx].pnt,&map.nodes[main_idx].ang,&center_pnt,&hand_pnts[item_count*3])) {
					type_list[item_count]=type;
					idx_list[item_count]=main_idx;
					item_count++;
				}
				break;
				
			case spot_piece:
				if (view_handle_create_rot_handle(view,&map.spots[main_idx].pnt,&map.spots[main_idx].ang,&center_pnt,&hand_pnts[item_count*3])) {
					type_list[item_count]=type;
					idx_list[item_count]=main_idx;
					item_count++;
				}
				break;
				
			case scenery_piece:
				if (view_handle_create_rot_handle(view,&map.sceneries[main_idx].pnt,&map.sceneries[main_idx].ang,&center_pnt,&hand_pnts[item_count*3])) {
					type_list[item_count]=type;
					idx_list[item_count]=main_idx;
					item_count++;
				}
				break;

		}
	}
	
	if (item_count==0) {
		free(hand_pnts);
		free(type_list);
		free(idx_list);
		return(FALSE);
	}

		// run pick list for handles
	
	view_pick_list_start(view,TRUE,(item_count*3));

	for (n=0;n!=item_count;n++) {
		idx=n*3;
		for (k=0;k!=3;k++) {
			view_pick_list_add_2D_handle(&hand_pnts[idx],type_list[n],idx_list[n],k);
			idx++;
		}
	}
	
	free(hand_pnts);
	free(type_list);
	free(idx_list);
	
	view_pick_list_end(view,click_pt,&type,&main_idx,&which_axis);
	if (type==-1) return(FALSE);

		// setup the drag

	switch (type) {
	
		case node_piece:
			pnt=&map.nodes[main_idx].pnt;
			ang=&map.nodes[main_idx].ang;
			break;
			
		case spot_piece:
			pnt=&map.spots[main_idx].pnt;
			ang=&map.spots[main_idx].ang;
			break;
			
		case scenery_piece:
			pnt=&map.sceneries[main_idx].pnt;
			ang=&map.sceneries[main_idx].ang;
			break;
	}

	memmove(&org_pnt,pnt,sizeof(d3pnt));
	memmove(&org_ang,ang,sizeof(d3ang));
	
		// handle drag
	
    if (!os_button_down()) return(FALSE);
	
	undo_push();
	
	first_drag=TRUE;
	
	view_get_pixel_box(view,&box);
	
	while (!os_track_mouse_location(&pt,&box)) {
		
			// turn on drag cursor
			
		if (first_drag) {
			os_set_drag_cursor();
			first_drag=FALSE;
		}

			// handle movement
			
		if (state.handle_mode==handle_mode_rotate) {
			view_click_rot_handle_rotate_run(ang,&org_ang,(float)(click_pt->x-pt.x),which_axis);
		}
		else {
			view_click_rot_handle_move_run(pnt,&org_pnt,(click_pt->x-pt.x),which_axis);
		}
		
        main_wind_draw();
	}
	
	os_set_arrow_cursor();
	
	return(!first_drag);
}

/* =======================================================

      Run Pick List for Map Items
      
======================================================= */

void view_click_piece_map_pick_start(editor_view_type *view)
{
	int					n,k,t,count;
	float				vertexes[8*3];
	float				*pv;
	d3pnt				*pt;
	d3pnt				v_pnts[8];
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	map_liquid_type		*liq;
	spot_type			*spot;
	map_scenery_type	*scenery;
	map_light_type		*map_light;
	map_sound_type		*map_sound;
	map_particle_type	*map_particle;
	node_type			*node;

		// get picker count
		// we create a list that marks the color
		// for each item we are going to pick
		// from

	count=0;

	mesh=map.mesh.meshes;

	for (n=0;n!=map.mesh.nmesh;n++) {
		count+=mesh->npoly;
		mesh++;
	}
	
	if (state.show_liquid) count+=map.liquid.nliquid;
	
	if (state.show_object) {
		count+=map.nspot;
		count+=map.nscenery;
	}
	
	if (state.show_lightsoundparticle) {
		count+=map.nlight;
		count+=map.nsound;
		count+=map.nparticle;
	}
	
	if (state.show_node) count+=map.nnode;

		// start the pick list

	view_pick_list_start(view,FALSE,count);

		// run through all the items
		// in the map and draw them to the
		// back buffer with unique colors

		// meshes
		
	mesh=map.mesh.meshes;
	
	for (n=0;n!=map.mesh.nmesh;n++) {
	
			// hidden mesh
		
		if (view_hidden_mesh(view,n)) {
			mesh++;
			continue;
		}

		poly=mesh->polys;
	
		for (k=0;k!=mesh->npoly;k++) {
					
				// clipping
				
			if (view_clip_poly(view,poly)) {
				poly++;
				continue;
			}
			
				// hidden
				
			if (view_hidden_poly(view,mesh,poly)) {
				poly++;
				continue;
			}
			
				// draw color poly

			view_pick_list_add(mesh_piece,n,k);

			pv=vertexes;
			
			for (t=0;t!=poly->ptsz;t++) {
				pt=&mesh->vertexes[poly->v[t]];
				*pv++=(float)pt->x;
				*pv++=(float)pt->y;
				*pv++=(float)pt->z;
			}
			
			glVertexPointer(3,GL_FLOAT,0,vertexes);
			glDrawArrays(GL_POLYGON,0,poly->ptsz);

			poly++;
		}

		mesh++;
	}

		// liquids
		
	if (state.show_liquid) {

		liq=map.liquid.liquids;
		
		for (n=0;n!=map.liquid.nliquid;n++) {

					// clipping
				
			if (view_clip_liquid(view,liq)) {
				liq++;
				continue;
			}

				// add to pick list

			view_pick_list_add(liquid_piece,n,-1);

			vertexes[0]=vertexes[9]=(float)liq->lft;
			vertexes[3]=vertexes[6]=(float)liq->rgt;
			vertexes[1]=vertexes[4]=vertexes[7]=vertexes[10]=(float)liq->y;
			vertexes[2]=vertexes[5]=(float)liq->top;
			vertexes[8]=vertexes[11]=(float)liq->bot;

			glVertexPointer(3,GL_FLOAT,0,vertexes);
			glDrawArrays(GL_QUADS,0,4);

			vertexes[0]=vertexes[9]=(float)liq->lft;
			vertexes[3]=vertexes[6]=(float)liq->rgt;
			vertexes[1]=vertexes[4]=vertexes[7]=vertexes[10]=(float)(liq->y+liq->depth);
			vertexes[2]=vertexes[5]=(float)liq->top;
			vertexes[8]=vertexes[11]=(float)liq->bot;

			glVertexPointer(3,GL_FLOAT,0,vertexes);
			glDrawArrays(GL_QUADS,0,4);

			liq++;
		}
	}

		// objects and scenery

	if (state.show_object) {

		spot=map.spots;

		for (n=0;n!=map.nspot;n++) {
			view_model_cube_vertexes(spot->display_model,&spot->pnt,&spot->ang,1.0f,v_pnts);
			view_pick_list_add_cube(v_pnts,spot_piece,n,-1);
			spot++;
		}
			
		scenery=map.sceneries;

		for (n=0;n!=map.nscenery;n++) {
			view_model_cube_vertexes(scenery->model_name,&scenery->pnt,&scenery->ang,scenery->resize,v_pnts);
			view_pick_list_add_cube(v_pnts,scenery_piece,n,-1);
			scenery++;
		}

	}

		// lights, sounds, and particles

	if (state.show_lightsoundparticle) {
	
		map_light=map.lights;

		for (n=0;n!=map.nlight;n++) {
			view_get_sprite_vertexes(&map_light->pnt,NULL,v_pnts);
			view_pick_list_add_cube(v_pnts,light_piece,n,-1);
			map_light++;
		}
		
		map_sound=map.sounds;

		for (n=0;n!=map.nsound;n++) {
			view_get_sprite_vertexes(&map_sound->pnt,NULL,v_pnts);
			view_pick_list_add_cube(v_pnts,sound_piece,n,-1);
			map_sound++;
		}
		
		map_particle=map.particles;

		for (n=0;n!=map.nparticle;n++) {
			view_get_sprite_vertexes(&map_particle->pnt,NULL,v_pnts);
			view_pick_list_add_cube(v_pnts,particle_piece,n,-1);
			map_particle++;
		}
	}

		// nodes

	if (state.show_node) {
		node=map.nodes;

		for (n=0;n!=map.nnode;n++) {
			view_get_sprite_vertexes(&node->pnt,NULL,v_pnts);
			view_pick_list_add_cube(v_pnts,node_piece,n,-1);
			node++;
		}
	}
}
	
/* =======================================================

      View Map Clicking
      
======================================================= */

void view_click_piece(editor_view_type *view,d3pnt *pt,bool double_click)
{
	int				type,main_idx,sub_idx;
	d3rect			box;

		// convert point to view

	view_get_pixel_box(view,&box);

	pt->x-=box.lx;
	pt->y-=box.ty;

		// rotation handles

	if (view_click_rot_handles(view,pt)) return;
	
		// liquid vertex drags
		
	if (view_click_drag_liquid_vertex(view,pt)) return;
	
		// mesh vertex drags
		
	switch (state.drag_mode) {
	
		case drag_mode_vertex:
			if (view_click_drag_vertex(view,pt)) return;
			break;
	
		case drag_mode_mesh:
			if (view_click_drag_mesh_handle(view,pt)) return;
			break;
			
	}

		// pick clicked map item (mesh, liquid, nodes, etc)
		
	view_click_piece_map_pick_start(view);
	view_pick_list_end(view,pt,&type,&main_idx,&sub_idx);
	
		// special normal flip if in normal hide 
		// mode and polygon selection
		
	if ((type==mesh_piece) && (view->cull) && (state.show_normals) && (state.drag_mode==drag_mode_polygon)) {
		piece_mesh_poly_invert_normals(&map.mesh.meshes[main_idx].polys[sub_idx]);
	}
	
		// if a node, check link
		// connections
		
	if ((type==node_piece) && (state.node_mode!=node_mode_select)) {
		if (node_link_click(main_idx)) {
			main_wind_draw();
			return;
		}
	}
	
		// drag selects or toggle selection
		
	if (type==-1) {
		if (!view_click_box_select(view,pt)) {
			if (!state.select_add) select_clear();
		}
	}
	else {
		if ((!state.select_add) && (!os_key_shift_down())) {
			if (!select_check(type,main_idx,sub_idx)) {			// keep selection if selecting an already selected piece
				select_clear();	
				select_add(type,main_idx,sub_idx);
			}
		}
		else {
			select_flip(type,main_idx,sub_idx);
		}
	}
	
		// redraw and reset palettes and menus

	state.in_preference=FALSE;
		
	menu_fix_enable();
	
	texture_palette_reset();
	property_palette_reset();

	list_palette_set_level(&property_palette,1);

	main_wind_draw();

		// if no item or item was toggled off,
		// nothing to drag

	if (type==-1) return;
	if (!select_check(type,main_idx,sub_idx)) return;

		// item (spots, lights, sounds, etc) drags

	if (view_click_drag_item(view,pt)) return;
	
		// liquid drags
		
	if (view_click_drag_liquid(view,pt)) return;
	
		// mesh or poly drags

	switch (state.drag_mode) {
	
		case drag_mode_mesh:
			if (!os_key_control_down()) {
				view_click_drag_mesh(view,pt);
			}
			else {
				view_click_drag_texture_uv(view,pt,TRUE);
			}
			break;

		case drag_mode_polygon:
			if (!os_key_control_down()) {
				view_click_drag_mesh_poly(view,pt);
			}
			else {
				view_click_drag_texture_uv(view,pt,FALSE);
			}
			break;
			
	}
}



