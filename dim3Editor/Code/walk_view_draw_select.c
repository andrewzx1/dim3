/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Walk View Selection Drawing Routines

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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3editor.h"
#endif

#include "common_view.h"
#include "walk_view.h"

extern d3pnt					view_pnt;

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

		glPointSize(walk_view_handle_size);
		
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

	glPointSize(walk_view_handle_size);
	
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

	glPointSize(walk_view_handle_size);
		
	walk_view_draw_select_liquid_get_grow_handles(liquid_idx,px,py,pz);
		
	glBegin(GL_POINTS);

	for (n=0;n!=4;n++) {
		glVertex3i(px[n],py[n],pz[n]);
	}

	glEnd();
}

/* =======================================================

      Draw Selection for Sprite
      
======================================================= */

void walk_view_sprite_select_size(d3pnt *pnt,int *px,int *py,int *pz)
{
	int				wid;
	
    wid=map_enlarge*3;

	px[0]=px[3]=px[6]=px[7]=pnt->x-wid;
	px[1]=px[2]=px[4]=px[5]=pnt->x+wid;
	
	py[0]=py[1]=py[2]=py[3]=pnt->y-(map_enlarge*4);
	py[4]=py[5]=py[6]=py[7]=pnt->y;
		
	pz[0]=pz[1]=pz[4]=pz[7]=pnt->z-wid;
	pz[2]=pz[3]=pz[5]=pz[6]=pnt->z+wid;
}

void walk_view_draw_select_sprite(d3pnt *pnt)
{
    int			wid,high;
  
    wid=map_enlarge*3;
    high=map_enlarge*4;
    
 	glColor4f(1,0,0,1);

	glEnable(GL_DEPTH_TEST);
	glLineWidth(2);
    
	glBegin(GL_LINE_LOOP);
	glVertex3i((pnt->x-wid),(pnt->y-high),(pnt->z-wid));
	glVertex3i((pnt->x+wid),(pnt->y-high),(pnt->z-wid));
	glVertex3i((pnt->x+wid),pnt->y,(pnt->z-wid));
	glVertex3i((pnt->x-wid),pnt->y,(pnt->z-wid));
	glEnd();

	glBegin(GL_LINE_LOOP);
 	glVertex3i((pnt->x-wid),(pnt->y-high),(pnt->z+wid));
	glVertex3i((pnt->x+wid),(pnt->y-high),(pnt->z+wid));
	glVertex3i((pnt->x+wid),pnt->y,(pnt->z+wid));
	glVertex3i((pnt->x-wid),pnt->y,(pnt->z+wid));
	glEnd();
	
	glBegin(GL_LINE_LOOP);
 	glVertex3i((pnt->x-wid),(pnt->y-high),(pnt->z-wid));
	glVertex3i((pnt->x-wid),(pnt->y-high),(pnt->z+wid));
	glVertex3i((pnt->x-wid),pnt->y,(pnt->z+wid));
	glVertex3i((pnt->x-wid),pnt->y,(pnt->z-wid));
	glEnd();
	
	glBegin(GL_LINE_LOOP);
	glVertex3i((pnt->x+wid),(pnt->y-high),(pnt->z-wid));
	glVertex3i((pnt->x+wid),(pnt->y-high),(pnt->z+wid));
	glVertex3i((pnt->x+wid),pnt->y,(pnt->z+wid));
	glVertex3i((pnt->x+wid),pnt->y,(pnt->z-wid));
	glEnd();
    
    glLineWidth(1);
}

/* =======================================================

      Draw Rotation Handles
      
======================================================= */

void walk_view_draw_select_single_rot_handles(d3pnt *pnt,d3vct *vct,d3ang *ang,int y_size,d3col *col)
{
	int				y;
	matrix_type		mat;

		// y location
		
	y=pnt->y-y_size;
	
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
	
		// draw line
		
    glLineWidth(2);
	
	glColor4f(col->r,col->g,col->b,1.0f);
	
	glBegin(GL_LINES);
	glVertex3i(pnt->x,y,pnt->z);
	glVertex3i((pnt->x+(int)vct->x),(y+(int)vct->y),(pnt->z+(int)vct->z));
	glEnd();
	
	glPointSize(10);
	
	glBegin(GL_POINTS);
	glVertex3i((pnt->x+(int)vct->x),(y+(int)vct->y),(pnt->z+(int)vct->z));
	glEnd();
	
	glPointSize(1);
	
    glLineWidth(1);
}

void walk_view_draw_select_rot_handles(d3pnt *pnt,d3ang *ang,int y_size,bool y_only)
{
	float			len;
	d3vct			vct;
	d3col			col;
	
	len=(float)(map_enlarge*4);
	
		// x rot
		
	vct.x=len;
	vct.y=0.0f;
	vct.z=0.0f;
	
	if (y_only) {
		col.r=col.g=col.b=0.5f;
	}
	else {
		col.r=1.0f;
		col.g=0.0f;
		col.b=0.0f;
	}
	
	walk_view_draw_select_single_rot_handles(pnt,&vct,ang,y_size,&col);
	
		// y rot
		
	vct.x=0.0f;
	vct.y=-len;
	vct.z=0.0f;
	
	col.r=0.0f;
	col.g=1.0f;
	col.b=0.0f;
	
	walk_view_draw_select_single_rot_handles(pnt,&vct,ang,y_size,&col);
	
		// z rot
	
	vct.x=0.0f;
	vct.y=0.0f;
	vct.z=len;
	
	if (y_only) {
		col.r=col.g=col.b=0.5f;
	}
	else {
		col.r=0.0f;
		col.g=0.0f;
		col.b=1.0f;
	}
	
	walk_view_draw_select_single_rot_handles(pnt,&vct,ang,y_size,&col);
}

/* =======================================================

      Draw Selections for Map
      
======================================================= */

void walk_view_draw_select(d3pnt *cpt,bool rot_on)
{
	int						n,sel_count,y_size,
							type,main_idx,sub_idx;
	unsigned char			draw_mesh_once[max_mesh];
	
	sel_count=select_count();
	if (sel_count==0) return;
	
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
				walk_view_draw_select_sprite(&map.nodes[main_idx].pnt);
				y_size=map_enlarge*5;
				if (rot_on) walk_view_draw_select_rot_handles(&map.nodes[main_idx].pnt,&map.nodes[main_idx].ang,y_size,FALSE);
				break;
				
			case spot_piece:
				if (!walk_view_model_draw_select(&map.spots[main_idx].pnt,&map.spots[main_idx].ang,map.spots[main_idx].display_model)) {
					walk_view_draw_select_sprite(&map.spots[main_idx].pnt);
				}
				y_size=walk_view_model_rot_y_size(&map.spots[main_idx].pnt,&map.spots[main_idx].ang,map.spots[main_idx].display_model);
				if (rot_on) walk_view_draw_select_rot_handles(&map.spots[main_idx].pnt,&map.spots[main_idx].ang,y_size,TRUE);
				break;
				
			case scenery_piece:
				if (!walk_view_model_draw_select(&map.sceneries[main_idx].pnt,&map.sceneries[main_idx].ang,map.sceneries[main_idx].model_name)) {
					walk_view_draw_select_sprite(&map.sceneries[main_idx].pnt);
				}
				y_size=walk_view_model_rot_y_size(&map.sceneries[main_idx].pnt,&map.sceneries[main_idx].ang,map.sceneries[main_idx].model_name);
				if (rot_on) walk_view_draw_select_rot_handles(&map.sceneries[main_idx].pnt,&map.sceneries[main_idx].ang,y_size,FALSE);
				break;
				
			case light_piece:
				walk_view_draw_select_sprite(&map.lights[main_idx].pnt);
				break;
				
			case sound_piece:
				walk_view_draw_select_sprite(&map.sounds[main_idx].pnt);
				break;
				
			case particle_piece:
				walk_view_draw_select_sprite(&map.particles[main_idx].pnt);
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
	
		// finish with selecte mesh vertexes
		
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
}
			
