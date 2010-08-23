/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Walk View Selection Drawing Routines

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

#include "common_view.h"
#include "walk_view.h"

extern map_type					map;
extern setup_type				setup;
extern editor_state_type		state;

/* =======================================================

      Draw Selection for Mesh
      
======================================================= */

void walk_view_draw_select_mesh_get_grow_handles(int mesh_idx,int *px,int *py,int *pz)
{
	d3pnt			min,max;
	
	map_mesh_calculate_extent(&map,mesh_idx,&min,&max);
	
	px[0]=px[3]=px[4]=px[7]=min.x;
	pz[0]=pz[1]=pz[4]=pz[5]=min.z;
	px[1]=px[2]=px[5]=px[6]=max.x;
	pz[2]=pz[3]=pz[6]=pz[7]=max.z;
	py[0]=py[1]=py[2]=py[3]=min.y;
	py[4]=py[5]=py[6]=py[7]=max.y;
}

void walk_view_draw_select_mesh(int mesh_idx)
{
	int						n,k,t,px[8],py[8],pz[8];
	d3pnt					*pt;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*mesh_poly;
	
	mesh=&map.mesh.meshes[mesh_idx];
	
		// draw selected mesh
		
	glColor4f(setup.col.mesh_sel.r,setup.col.mesh_sel.g,setup.col.mesh_sel.b,1.0f);
	
	glEnable(GL_DEPTH_TEST);
	
	mesh_poly=mesh->polys;
	
	for (k=0;k!=mesh->npoly;k++) {
	
		glBegin(GL_LINE_LOOP);
		
		for (t=0;t!=mesh_poly->ptsz;t++) {
			pt=&mesh->vertexes[mesh_poly->v[t]];
			glVertex3i(pt->x,pt->y,pt->z);
		}
		
		glEnd();
		
		mesh_poly++;
	}
	
		// is mesh only, draw resize handles
		
	if (state.drag_mode==drag_mode_mesh) {
	
		glEnable(GL_DEPTH_TEST);

		glPointSize(view_handle_size);
		
		walk_view_draw_select_mesh_get_grow_handles(mesh_idx,px,py,pz);
		
		glBegin(GL_POINTS);

		for (n=0;n!=8;n++) {
			if (state.drag_handle_idx==n) {
				glColor4f(setup.col.poly_sel.r,setup.col.poly_sel.g,setup.col.poly_sel.b,1.0f);
			}
			else {
				glColor4f(setup.col.mesh_sel.r,setup.col.mesh_sel.g,setup.col.mesh_sel.b,1.0f);
			}
			glVertex3i(px[n],py[n],pz[n]);
		}

		glEnd();
	
		return;
	}
}

void walk_view_draw_select_mesh_vertex(int mesh_idx)
{
	int						n;
	d3pnt					*pt;
	map_mesh_type			*mesh;
	
	glEnable(GL_DEPTH_TEST);

	glPointSize(view_handle_size);
	
	mesh=&map.mesh.meshes[mesh_idx];
	pt=mesh->vertexes;
	
	glBegin(GL_POINTS);

	for (n=0;n!=mesh->nvertex;n++) {
		if (state.drag_handle_idx==n) {
			glColor4f(setup.col.poly_sel.r,setup.col.poly_sel.g,setup.col.poly_sel.b,1.0f);
		}
		else {
			glColor4f(setup.col.mesh_sel.r,setup.col.mesh_sel.g,setup.col.mesh_sel.b,1.0f);
		}
		glVertex3i(pt->x,pt->y,pt->z);
		
		pt++;
	}

	glEnd();
}

void walk_view_draw_select_mesh_poly(int mesh_idx,int poly_idx)
{
	int						n;
	d3pnt					*pt;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*mesh_poly;
	
	glDisable(GL_DEPTH_TEST);
	
	glLineWidth(2.0f);

	glColor4f(setup.col.poly_sel.r,setup.col.poly_sel.g,setup.col.poly_sel.b,1.0f);
	
	mesh=&map.mesh.meshes[mesh_idx];
	mesh_poly=&mesh->polys[poly_idx];
	
	glBegin(GL_LINE_LOOP);
	
	for (n=0;n!=mesh_poly->ptsz;n++) {
		pt=&mesh->vertexes[mesh_poly->v[n]];
		glVertex3i(pt->x,pt->y,pt->z);
	}
	
	glEnd();

	glLineWidth(1.0f);
}

/* =======================================================

      Draw Selection for Liquid
      
======================================================= */

void walk_view_draw_select_liquid_get_grow_handles(int liquid_idx,int *px,int *py,int *pz)
{
	map_liquid_type			*liq;
	
	liq=&map.liquid.liquids[liquid_idx];
	
	px[0]=px[3]=liq->lft;
	px[1]=px[2]=liq->rgt;
	pz[0]=pz[1]=liq->top;
	pz[2]=pz[3]=liq->bot;
	py[0]=py[1]=py[2]=py[3]=liq->y;
}

void walk_view_draw_select_liquid(int liquid_idx)
{
	int						n,px[4],py[4],pz[4];
	map_liquid_type			*liq;
	
	liq=&map.liquid.liquids[liquid_idx];
	
	glEnable(GL_DEPTH_TEST);

	glColor4f(setup.col.mesh_sel.r,setup.col.mesh_sel.g,setup.col.mesh_sel.b,1.0f);
	
		// outline
		
	glBegin(GL_LINE_LOOP);
	glVertex3i(liq->lft,liq->y,liq->top);
	glVertex3i(liq->rgt,liq->y,liq->top);
	glVertex3i(liq->rgt,liq->y,liq->bot);
	glVertex3i(liq->lft,liq->y,liq->bot);
	glEnd();
	
		// handles

	glPointSize(view_handle_size);
		
	walk_view_draw_select_liquid_get_grow_handles(liquid_idx,px,py,pz);
		
	glBegin(GL_POINTS);

	for (n=0;n!=4;n++) {
		if (state.drag_handle_idx==n) {
			glColor4f(setup.col.poly_sel.r,setup.col.poly_sel.g,setup.col.poly_sel.b,1.0f);
		}
		else {
			glColor4f(setup.col.mesh_sel.r,setup.col.mesh_sel.g,setup.col.mesh_sel.b,1.0f);
		}
		glVertex3i(px[n],py[n],pz[n]);
	}

	glEnd();
}

/* =======================================================

      Draw Selection for Cube Items
      
======================================================= */

void walk_view_draw_select_cube(d3pnt *v_pnts)
{
    glLineWidth(3.0f);
	glColor4f(1.0f,0.0f,0.0f,1.0f);
    
	glBegin(GL_LINE_LOOP);
	glVertex3i(v_pnts[0].x,v_pnts[0].y,v_pnts[0].z);
	glVertex3i(v_pnts[1].x,v_pnts[1].y,v_pnts[1].z);
	glVertex3i(v_pnts[2].x,v_pnts[2].y,v_pnts[2].z);
	glVertex3i(v_pnts[3].x,v_pnts[3].y,v_pnts[3].z);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3i(v_pnts[4].x,v_pnts[4].y,v_pnts[4].z);
	glVertex3i(v_pnts[5].x,v_pnts[5].y,v_pnts[5].z);
	glVertex3i(v_pnts[6].x,v_pnts[6].y,v_pnts[6].z);
	glVertex3i(v_pnts[7].x,v_pnts[7].y,v_pnts[7].z);
	glEnd();
	
	glBegin(GL_LINES);
	glVertex3i(v_pnts[0].x,v_pnts[0].y,v_pnts[0].z);
	glVertex3i(v_pnts[7].x,v_pnts[7].y,v_pnts[7].z);
	glVertex3i(v_pnts[1].x,v_pnts[1].y,v_pnts[1].z);
	glVertex3i(v_pnts[4].x,v_pnts[4].y,v_pnts[4].z);
	glVertex3i(v_pnts[2].x,v_pnts[2].y,v_pnts[2].z);
	glVertex3i(v_pnts[5].x,v_pnts[5].y,v_pnts[5].z);
	glVertex3i(v_pnts[3].x,v_pnts[3].y,v_pnts[3].z);
	glVertex3i(v_pnts[6].x,v_pnts[6].y,v_pnts[6].z);
	glEnd();
    
    glLineWidth(1.0f);
}

/* =======================================================

      Draw Rotation and Move Handles
      
======================================================= */

void walk_view_draw_select_2D_rot_handle(d3pnt *pnt,d3pnt *hand_pnt,d3col *col)
{
    glLineWidth(view_handle_line_width);
	
	glColor4f(col->r,col->g,col->b,1.0f);
	
	glBegin(GL_LINES);
	glVertex2i(pnt->x,pnt->y);
	glVertex2i(hand_pnt->x,hand_pnt->y);
	glEnd();
	
    glLineWidth(1.0f);
	
	glPointSize(view_handle_size);
	
	glBegin(GL_POINTS);
	glVertex2i(hand_pnt->x,hand_pnt->y);
	glEnd();
	
	glPointSize(1.0f);
}

void walk_view_draw_select_rot_handles(editor_view_type *view)
{
	d3pnt			pnt,hand_pnt[3];
	d3col			col;

		// create rot handles
		
	if (!view_handle_create_rot_handle(view,&pnt,hand_pnt)) return;

		// draw points

	walk_view_set_2D_projection(view);

	glDisable(GL_DEPTH_TEST);
	
	col.r=1.0f;
	col.g=0.0f;
	col.b=0.0f;
	
	walk_view_draw_select_2D_rot_handle(&pnt,&hand_pnt[0],&col);
	
	col.r=0.0f;
	col.g=1.0f;
	col.b=0.0f;
	
	walk_view_draw_select_2D_rot_handle(&pnt,&hand_pnt[1],&col);
	
	col.r=0.0f;
	col.g=0.0f;
	col.b=1.0f;
	
	walk_view_draw_select_2D_rot_handle(&pnt,&hand_pnt[2],&col);
}

/* =======================================================

      Draw Selection Box
      
======================================================= */

void walk_view_draw_select_box(editor_view_type *view)
{
	int				lx,rx,ty,by;
	d3rect			box;

		// selection in this view?

	if ((!state.select_box_on) || (view!=walk_view_get_current_view())) return;

		// need the box to position

	walk_view_get_pixel_box(view,&box);

	lx=state.select_box_start_pnt.x+box.lx;
	rx=state.select_box_end_pnt.x+box.lx;
	ty=box.by-state.select_box_start_pnt.y;
	by=box.by-state.select_box_end_pnt.y;

		// draw the selection

	walk_view_set_2D_projection(view);
	
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glEnable(GL_BLEND);

	glColor4f(0.5f,0.5f,0.5f,0.7f);
	
	glBegin(GL_QUADS);
	glVertex2i(lx,ty);
	glVertex2i(rx,ty);
	glVertex2i(rx,by);
	glVertex2i(lx,by);
	glEnd();

	glColor4f(1.0f,1.0f,1.0f,0.7f);

	glBegin(GL_LINE_LOOP);
	glVertex2i(lx,ty);
	glVertex2i(rx,ty);
	glVertex2i(rx,by);
	glVertex2i(lx,by);
	glEnd();

	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
}

/* =======================================================

      Draw Selections for Map
      
======================================================= */

void walk_view_draw_select(editor_view_type *view)
{
	int						n,sel_count,
							type,main_idx,sub_idx;
	unsigned char			draw_mesh_once[max_mesh];
	d3pnt					v_pnts[8];
	
		// if no selection, only the box
		// select can draw

	sel_count=select_count();
	if (sel_count==0) {
		walk_view_draw_select_box(view);
		return;
	}

		// only draw mesh selections once
		// as there could be multiple polygon selections
		// for a single mesh
		
	memset(draw_mesh_once,0x0,max_mesh);
	
		// draw the selection of meshes, liquids,
		// and other pieces, drawing the selected
		// polygons later
		
	for (n=(sel_count-1);n>=0;n--) {
	
		select_get(n,&type,&main_idx,&sub_idx);
		
			// draw selection
			
		switch (type) {
		
			case mesh_piece:
				if (draw_mesh_once[main_idx]==0x0) {
					draw_mesh_once[main_idx]=0x1;
					walk_view_draw_select_mesh(main_idx);
				}
				break;
				
			case liquid_piece:
				walk_view_draw_select_liquid(main_idx);
				break;
				
			case node_piece:
				view_get_sprite_vertexes(&map.nodes[main_idx].pnt,NULL,v_pnts);
				walk_view_draw_select_cube(v_pnts);
				break;
				
			case spot_piece:
				walk_view_model_cube_vertexes(map.spots[main_idx].display_model,&map.spots[main_idx].pnt,&map.spots[main_idx].ang,v_pnts);
				walk_view_draw_select_cube(v_pnts);
				break;
				
			case scenery_piece:
				walk_view_model_cube_vertexes(map.sceneries[main_idx].model_name,&map.sceneries[main_idx].pnt,&map.sceneries[main_idx].ang,v_pnts);
				walk_view_draw_select_cube(v_pnts);
				break;
				
			case light_piece:
				view_get_sprite_vertexes(&map.lights[main_idx].pnt,NULL,v_pnts);
				walk_view_draw_select_cube(v_pnts);
				break;
				
			case sound_piece:
				view_get_sprite_vertexes(&map.sounds[main_idx].pnt,NULL,v_pnts);
				walk_view_draw_select_cube(v_pnts);
				break;
				
			case particle_piece:
				view_get_sprite_vertexes(&map.particles[main_idx].pnt,NULL,v_pnts);
				walk_view_draw_select_cube(v_pnts);
				break;

		}
	}
	
		// draw selected polygons
		// draw afterwards so mesh highlighting doesn't
		// effect selection
		
	if (state.drag_mode==drag_mode_polygon) {
	
		for (n=(sel_count-1);n>=0;n--) {
		
			select_get(n,&type,&main_idx,&sub_idx);
			if (type!=mesh_piece) continue;
			
			walk_view_draw_select_mesh_poly(main_idx,sub_idx);
		}
		
	}
	
		// draw selected mesh vertexes
		
	if (state.drag_mode==drag_mode_vertex) {

		memset(draw_mesh_once,0x0,max_mesh);
			
		for (n=(sel_count-1);n>=0;n--) {
		
			select_get(n,&type,&main_idx,&sub_idx);
			if (type!=mesh_piece) continue;
			
			if (draw_mesh_once[main_idx]==0x0) {
				draw_mesh_once[main_idx]=0x1;
				walk_view_draw_select_mesh_vertex(main_idx);
			}
		}
	}

		// draw selection handle
	
	walk_view_draw_select_rot_handles(view);

		// draw box selection

	walk_view_draw_select_box(view);
}
			
