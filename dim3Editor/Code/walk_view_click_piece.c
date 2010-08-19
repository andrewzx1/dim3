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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3editor.h"
#endif

#include "interface.h"
#include "common_view.h"
#include "walk_view.h"

extern int					txt_pixel_sz;
extern d3rect				main_wind_box;

extern map_type				map;
extern setup_type			setup;
extern editor_state_type	state;

int							walk_view_vport[4];
double						walk_view_mod_matrix[16],walk_view_proj_matrix[16];

int							pick_count,pick_col[3];
view_picker_type			*picks;

/* =======================================================

      View Picking
      
======================================================= */

bool walk_view_pick_list_start(editor_view_type *view,int count)
{
		// start picker list

	picks=(view_picker_type*)malloc(sizeof(view_picker_type)*count);
	if (picks==NULL) return(FALSE);

		// no picks set yet

	pick_count=0;

		// start the pick color

	pick_col[0]=0x1;
	pick_col[1]=0x0;
	pick_col[2]=0x0;

		// start the back buffer drawing
		// disable smoothing so colors stay true

	walk_view_set_viewport(view,TRUE,TRUE);
	walk_view_set_3D_projection(view,map.settings.editor.view_near_dist,map.settings.editor.view_far_dist,walk_view_near_offset);

	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glDisable(GL_SMOOTH);
	glDisable(GL_BLEND);

	glDisable(GL_TEXTURE_2D);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glDisable(GL_ALPHA_TEST);

	return(TRUE);
}

void walk_view_pick_list_end(editor_view_type *view,d3pnt *pnt,int *type,int *main_idx,int *sub_idx)
{
	int					n;
	unsigned char		pixel[3];
	d3pnt				click_pnt;
	d3rect				box;
	view_picker_type	*pick;

		// flush the drawing

	glFinish();

		// need to make click point global
		// and swap Y for OpenGL read

	walk_view_get_pixel_box(view,&box);

	click_pnt.x=pnt->x+box.lx;
	click_pnt.y=pnt->y+box.ty;

	os_get_window_box(&box);

	click_pnt.y=box.by-click_pnt.y;

		// find the color at the clicked point

	glReadPixels(click_pnt.x,click_pnt.y,1,1,GL_RGB,GL_UNSIGNED_BYTE,(void*)pixel);

		// find which pick it represents
		
	*type=-1;

	pick=picks;

	for (n=0;n!=pick_count;n++) {

		if ((pick->col[0]==pixel[0]) && (pick->col[1]==pixel[1]) && (pick->col[2]==pixel[2])) {
			*type=pick->type;
			*main_idx=pick->main_idx;
			*sub_idx=pick->sub_idx;
			break;
		}

		pick++;
	}

	free(picks);

		// restore smoothing

	glEnable(GL_SMOOTH);
}

void walk_view_pick_list_add(int type,int main_idx,int sub_idx)
{
	view_picker_type	*pick;

		// set color

	glColor3ub((unsigned char)pick_col[0],(unsigned char)pick_col[1],(unsigned char)pick_col[2]);

		// remember color in pick list

	pick=&picks[pick_count];

	pick->type=type;
	pick->main_idx=(short)main_idx;
	pick->sub_idx=(short)sub_idx;

	pick->col[0]=(unsigned char)pick_col[0];
	pick->col[1]=(unsigned char)pick_col[1];
	pick->col[2]=(unsigned char)pick_col[2];

		// next color

	pick_col[0]++;
	if (pick_col[0]>0xFF) {
		pick_col[0]=0x0;
		pick_col[1]++;
		if (pick_col[1]>0xFF) {
			pick_col[1]=0x0;
			pick_col[2]++;
		}
	}

		// next picker

	pick_count++;
}

void walk_view_pick_list_add_cube(int *px,int *py,int *pz,int type,int main_idx,int sub_idx)
{
	int				n,idx,quad_list[24]={0,1,2,3,4,5,6,7,3,2,5,6,0,1,4,7,0,7,6,3,1,2,5,4};

		// set the color

	walk_view_pick_list_add(type,main_idx,sub_idx);

		// draw the cube

	glBegin(GL_QUADS);
	
	for (n=0;n!=24;n++) {
		idx=quad_list[n];
		glVertex3i(px[idx],py[idx],pz[idx]);
	}

	glEnd();
}

void walk_view_pick_list_add_handle(int x,int y,int z,int type,int main_idx,int sub_idx)
{
		// set the color

	walk_view_pick_list_add(type,main_idx,sub_idx);

		// draw the point

	glPointSize(walk_view_handle_size);
	
	glBegin(GL_POINTS);
	glVertex3i(x,y,z);
	glEnd();
	
	glPointSize(1.0f);
}

/* =======================================================

      Grid and Snap
      
======================================================= */

int walk_view_get_grid(void)
{
	switch (state.grid_mode) {
		case grid_mode_small:
			return(map_enlarge);
		case grid_mode_medium:
			return(map_enlarge<<1);
		case grid_mode_large:
			return(map_enlarge<<3);
	}

	return(1);
}

void walk_view_click_grid(d3pnt *pt)
{
	int			sz;
	
	sz=walk_view_get_grid();
	
	pt->x/=sz;
	pt->y/=sz;
	pt->z/=sz;
	
	pt->x*=sz;
	pt->y*=sz;
	pt->z*=sz;
}

bool walk_view_click_snap(int mesh_idx,d3pnt *pt)
{
	int				n,t;
	d3pnt			*dpt;
	map_mesh_type	*mesh;
	
	if (state.vertex_mode!=vertex_mode_snap) return(FALSE);
	
		// any vertexes to snap to?
		
	mesh=map.mesh.meshes;
		
	for (n=0;n!=map.mesh.nmesh;n++) {
		
		if (n==mesh_idx) {
			mesh++;
			continue;
		}

		dpt=mesh->vertexes;
		
		for (t=0;t!=mesh->nvertex;t++) {
		
			if (distance_get(dpt->x,dpt->y,dpt->z,pt->x,pt->y,pt->z)<(setup.snap_size*map_enlarge)) {
				memmove(pt,dpt,sizeof(d3pnt));
				return(TRUE);
			}
			
			dpt++;
		}
	
		mesh++;
	}
	
	return(FALSE);
}

bool walk_view_click_snap_poly(int mesh_idx,int poly_idx,d3pnt *pt)
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
		
		if (walk_view_click_snap(mesh_idx,&hpt)) {
			d=distance_get(pt->x,pt->y,pt->z,hpt.x,hpt.y,hpt.z);
			if (d>(setup.snap_size*map_enlarge)) continue;
			
			if ((d<cur_dist) || (cur_dist<0)) {
				cur_dist=d;
				memmove(pt,&hpt,sizeof(d3pnt));
			}
		}
	}
	
	return(cur_dist!=-1);
}

bool walk_view_click_snap_mesh(int mesh_idx,d3pnt *pt)
{
	int					n,d,cur_dist;
	d3pnt				hpt;
	map_mesh_type		*mesh;
	
	cur_dist=-1;
	
	mesh=&map.mesh.meshes[mesh_idx];
	
	for (n=0;n!=mesh->nvertex;n++) {
		memmove(&hpt,pt,sizeof(d3pnt));
		
		if (walk_view_click_snap(mesh_idx,&hpt)) {
			d=distance_get(pt->x,pt->y,pt->z,hpt.x,hpt.y,hpt.z);
			if (d>(setup.snap_size*map_enlarge)) continue;
			
			if ((d<cur_dist) || (cur_dist<0)) {
				cur_dist=d;
				memmove(pt,&hpt,sizeof(d3pnt));
			}
		}
	}
	
	return(cur_dist!=-1);
}

/* =======================================================

      View Rotation Handles Clicking
      
======================================================= */

void walk_view_click_item_rot_handles_run_single(editor_view_type *view_setup,int type,int main_idx,int sub_idx,d3pnt *pnt,d3vct *vct,d3ang *ang,int y_size)
{
	matrix_type		mat;

		// rotations
	
	if (ang->x!=0) {
		matrix_rotate_x(&mat,ang->x);
		matrix_vertex_multiply(&mat,&vct->x,&vct->y,&vct->z);
	}
	
	if (ang->y!=0) {
		matrix_rotate_y(&mat,ang->y);
		matrix_vertex_multiply(&mat,&vct->x,&vct->y,&vct->z);
	}
	
	if (ang->z!=0) {
		matrix_rotate_z(&mat,ang->z);
		matrix_vertex_multiply(&mat,&vct->x,&vct->y,&vct->z);
	}
	
		// click in position
		
	walk_view_pick_list_add_handle((pnt->x+(int)vct->x),((pnt->y-y_size)+(int)vct->y),(pnt->z+(int)vct->z),type,main_idx,sub_idx);
}

void walk_view_click_item_rot_handles_run(editor_view_type *view,int type,int main_idx,d3pnt *pnt,d3ang *ang,int y_size,bool y_only)
{
	float			len;
	d3vct			vct;
	
	len=(float)(map_enlarge*4);
	
		// x rot
		
	if (!y_only) {
		vct.x=len;
		vct.y=0.0f;
		vct.z=0.0f;
		walk_view_click_item_rot_handles_run_single(view,type,main_idx,0,pnt,&vct,ang,y_size);
	}
	
		// y rot
		
	vct.x=0.0f;
	vct.y=-len;
	vct.z=0.0f;
	
	walk_view_click_item_rot_handles_run_single(view,type,main_idx,1,pnt,&vct,ang,y_size);
	
		// z rot
	
	if (!y_only) {
		vct.x=0.0f;
		vct.y=0.0f;
		vct.z=len;
		
		walk_view_click_item_rot_handles_run_single(view,type,main_idx,2,pnt,&vct,ang,y_size);
	}
}

bool walk_view_click_item_rot_handles(editor_view_type *view,d3pnt *click_pt)
{
	int			n,ncount,type,main_idx,sub_idx,
				y_size,which_axis;
	float		ang_add;
	bool		first_drag;
	d3pnt		pt;
	d3ang		*ang,old_ang;
	d3rect		box;

		// check if any rotatable items is in
		// the selection

	ncount=select_count();
	if (ncount==0) return(FALSE);

	if ((!select_has_type(node_piece)) && (!select_has_type(spot_piece)) && (!select_has_type(scenery_piece))) return(FALSE);

		// start pick list
	
	walk_view_pick_list_start(view,(ncount*3));

		// draw rot handles for all selected items

	for (n=0;n!=ncount;n++) {
		select_get(n,&type,&main_idx,&sub_idx);
		
		switch (type) {
		
			case node_piece:
				walk_view_click_item_rot_handles_run(view,type,main_idx,&map.nodes[main_idx].pnt,&map.nodes[main_idx].ang,(map_enlarge*5),FALSE);
				break;
				
			case spot_piece:
				y_size=walk_view_model_rot_y_size(&map.spots[main_idx].pnt,&map.spots[main_idx].ang,map.spots[main_idx].display_model);
				walk_view_click_item_rot_handles_run(view,type,main_idx,&map.spots[main_idx].pnt,&map.spots[main_idx].ang,y_size,TRUE);
				break;
				
			case scenery_piece:
				y_size=walk_view_model_rot_y_size(&map.sceneries[main_idx].pnt,&map.sceneries[main_idx].ang,map.sceneries[main_idx].model_name);
				walk_view_click_item_rot_handles_run(view,type,main_idx,&map.sceneries[main_idx].pnt,&map.sceneries[main_idx].ang,y_size,FALSE);
				break;
				
		}
	}

		// get the pick

	type=-1;

	walk_view_pick_list_end(view,click_pt,&type,&main_idx,&which_axis);
	if (type==-1) return(FALSE);

	ang=NULL;
	
	switch (type) {
		case node_piece:
			ang=&map.nodes[main_idx].ang;
			break;
				
		case spot_piece:
			ang=&map.spots[main_idx].ang;
			break;
				
		case scenery_piece:
			ang=&map.sceneries[main_idx].ang;
			break;
	}

	if (ang==NULL) return(FALSE);

		// handle drag
	
    if (!os_button_down()) return(FALSE);
	
	undo_push();
	
	first_drag=TRUE;
	
	walk_view_get_pixel_box(view,&box);
	memmove(&old_ang,ang,sizeof(d3ang));
	
	while (!os_track_mouse_location(&pt,&box)) {
		
		ang_add=(float)(click_pt->x-pt.x);		
		if (ang_add==0.0f) continue;
		
			// turn on drag cursor
			
		if (first_drag) {
			os_set_drag_cursor();
			first_drag=FALSE;
		}

			// move vertex
			
		switch (which_axis) {
			case 0:
				ang->x=angle_add(old_ang.x,ang_add);
				break;
			case 1:
				ang->y=angle_add(old_ang.y,ang_add);
				break;
			case 2:
				ang->z=angle_add(old_ang.z,ang_add);
				break;
		}

        main_wind_draw();
	}
	
	os_set_arrow_cursor();
	
	return(!first_drag);
}

/* =======================================================

      Run Pick List for Map Items
      
======================================================= */

void walk_view_click_piece_map_pick(editor_view_type *view,d3pnt *click_pt,int *type,int *main_idx,int *sub_idx,bool sel_only)
{
	int					n,k,t,count,
						px[8],py[8],pz[8];
	d3pnt				*pt;
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

	walk_view_pick_list_start(view,count);

		// run through all the items
		// in the map and draw them to the
		// back buffer with unique colors

		// meshes
		
	mesh=map.mesh.meshes;
	
	for (n=0;n!=map.mesh.nmesh;n++) {

		poly=mesh->polys;
	
		for (k=0;k!=mesh->npoly;k++) {
					
				// clipping
				
			if (walk_view_clip_poly(view,mesh,poly)) {
				poly++;
				continue;
			}
			
				// draw color poly

			walk_view_pick_list_add(mesh_piece,n,k);

			glBegin(GL_POLYGON);
			
			for (t=0;t!=poly->ptsz;t++) {
				pt=&mesh->vertexes[poly->v[t]];
				glVertex3i(pt->x,pt->y,pt->z);
			}
			
			glEnd();

			poly++;
		}

		mesh++;
	}

		// liquids
		
	if (state.show_liquid) {

		liq=map.liquid.liquids;
		
		for (n=0;n!=map.liquid.nliquid;n++) {
		
			walk_view_pick_list_add(liquid_piece,n,-1);

			glBegin(GL_POLYGON);
			glVertex3i(liq->lft,liq->y,liq->top);
			glVertex3i(liq->rgt,liq->y,liq->top);
			glVertex3i(liq->rgt,liq->y,liq->bot);
			glVertex3i(liq->lft,liq->y,liq->bot);
			glEnd();
			
			liq++;
		}
	}

		// objects and scenery

	if (state.show_object) {

		spot=map.spots;

		for (n=0;n!=map.nspot;n++) {
			if (walk_view_model_click_select_size(spot->display_model,&spot->pnt,&spot->ang,px,py,pz)) {
				walk_view_pick_list_add_cube(px,py,pz,spot_piece,n,-1);
			}
			spot++;
		}
			
		scenery=map.sceneries;

		for (n=0;n!=map.nscenery;n++) {
			if (walk_view_model_click_select_size(scenery->model_name,&scenery->pnt,&scenery->ang,px,py,pz)) {
				walk_view_pick_list_add_cube(px,py,pz,scenery_piece,n,-1);
			}
			scenery++;
		}

	}

		// lights, sounds, and particles

	if (state.show_lightsoundparticle) {
	
		map_light=map.lights;

		for (n=0;n!=map.nlight;n++) {
			walk_view_sprite_select_size(&map_light->pnt,px,py,pz);
			walk_view_pick_list_add_cube(px,py,pz,light_piece,n,-1);
			map_light++;
		}
		
		map_sound=map.sounds;

		for (n=0;n!=map.nsound;n++) {
			walk_view_sprite_select_size(&map_sound->pnt,px,py,pz);
			walk_view_pick_list_add_cube(px,py,pz,sound_piece,n,-1);
			map_sound++;
		}
		
		map_particle=map.particles;

		for (n=0;n!=map.nparticle;n++) {
			walk_view_sprite_select_size(&map_particle->pnt,px,py,pz);
			walk_view_pick_list_add_cube(px,py,pz,particle_piece,n,-1);
			map_particle++;
		}
	}

		// nodes

	if (state.show_node) {
		node=map.nodes;

		for (n=0;n!=map.nnode;n++) {
			walk_view_sprite_select_size(&node->pnt,px,py,pz);
			walk_view_pick_list_add_cube(px,py,pz,node_piece,n,-1);
			node++;
		}
	}
	
		// find the picked item

	walk_view_pick_list_end(view,click_pt,type,main_idx,sub_idx);
}
	
/* =======================================================

      View Map Clicking
      
======================================================= */

void walk_view_click_piece(editor_view_type *view,d3pnt *pt,bool dblclick)
{
	int				type,main_idx,sub_idx;
	bool			toggle_select;
	d3rect			box;

		// convert point to view

	walk_view_get_pixel_box(view,&box);

	pt->x-=box.lx;
	pt->y-=box.ty;

		// rotation handles

	if (walk_view_click_item_rot_handles(view,pt)) return;
	
		// liquid vertex drags
		
	if (walk_view_click_drag_liquid_vertex(view,pt)) return;
	
		// mesh vertex drags
		
	switch (state.drag_mode) {
	
		case drag_mode_vertex:
			if (walk_view_click_drag_vertex(view,pt)) return;
			break;
	
		case drag_mode_mesh:
			if (walk_view_click_drag_mesh_handle(view,pt)) return;
			break;
			
	}

		// pick clicked map item (mesh, liquid, nodes, etc)
		
	walk_view_click_piece_map_pick(view,pt,&type,&main_idx,&sub_idx,FALSE);
	
		// if a node, check link
		// connections
		
	if ((type==node_piece) && (state.node_mode!=node_mode_select)) {
		if (node_link_click(main_idx)) {
			main_wind_draw();
			return;
		}
	}
	
		// regular or toggle selection
		
	toggle_select=os_key_shift_down() || state.select_toggle_mode;
	
		// clear or add to selection
		
	if (type==-1) {
		if (!toggle_select) select_clear();
	}
	else {
		if (!toggle_select) {
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
		
	menu_fix_enable();
	
	main_wind_draw();
	texture_palette_reset();
	palette_reset();

		// if no item or item was toggled off,
		// nothing to drag

	if (type==-1) return;
	if (!select_check(type,main_idx,sub_idx)) return;

		// item (spots, lights, sounds, etc) drags

	if (walk_view_click_drag_item(view,pt)) return;
	
		// liquid drags
		
	if (walk_view_click_drag_liquid(view,pt)) return;
	
		// mesh or poly drags

	switch (state.drag_mode) {
	
		case drag_mode_mesh:
			if (!os_key_control_down()) {
				walk_view_click_drag_mesh(view,pt);
			}
			else {
				walk_view_click_drag_texture(view,pt,TRUE);
			}
			break;

		case drag_mode_polygon:
			if (!os_key_control_down()) {
				walk_view_click_drag_mesh_poly(view,pt);
			}
			else {
				walk_view_click_drag_texture(view,pt,FALSE);
			}
			break;
			
	}
}



