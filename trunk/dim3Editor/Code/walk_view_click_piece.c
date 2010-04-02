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

extern d3pnt				view_pnt;
extern d3rect				main_wind_box;

extern map_type				map;
extern setup_type			setup;
extern editor_state_type	state;

int							walk_view_vport[4];
double						walk_view_mod_matrix[16],walk_view_proj_matrix[16];

extern bool obscure_mesh_view_bit_get(unsigned char *visibility_flag,int idx);

/* =======================================================

      Click Utilities
      
======================================================= */

void walk_view_click_setup_project(editor_3D_view_setup *view_setup)
{
		// setup walk view
		
	main_wind_set_viewport(&view_setup->box,TRUE,TRUE);
	main_wind_set_3D_projection(view_setup,map.settings.editor.view_near_dist,map.settings.editor.view_far_dist,walk_view_near_offset);
	
		// get projection
		
	glGetDoublev(GL_MODELVIEW_MATRIX,walk_view_mod_matrix);
	glGetDoublev(GL_PROJECTION_MATRIX,walk_view_proj_matrix);
	glGetIntegerv(GL_VIEWPORT,(GLint*)walk_view_vport);
}

bool walk_view_click_rotate_polygon_in_z(int x,int y,int z)
{
	return(((((double)x)*walk_view_mod_matrix[2])+(((double)y)*walk_view_mod_matrix[6])+(((double)z)*walk_view_mod_matrix[10])+walk_view_mod_matrix[14])>(float)walk_view_near_offset);
}

void walk_view_click_project_point(d3rect *box,int *x,int *y,int *z)
{
	double		dx,dy,dz;
	
	gluProject(*x,*y,*z,walk_view_mod_matrix,walk_view_proj_matrix,(GLint*)walk_view_vport,&dx,&dy,&dz);
	*x=((int)dx)-box->lx;
	*y=(main_wind_box.by-((int)dy))-box->ty;
	*z=(int)((dz)*10000.0f);
}

/* =======================================================

      Walk View Drag Movement
      
======================================================= */

void walk_view_click_drag_movement(editor_3D_view_setup *view_setup,int view_move_dir,int x,int y,int *xadd,int *yadd,int *zadd)
{
	int				sz;
	
	switch (view_move_dir) {
	
		case vm_dir_forward:
			*xadd=-(int)(((float)x)*mouse_forward_view_scale);
			*yadd=-(int)(((float)y)*mouse_forward_view_scale);
			*zadd=0;
			rotate_2D_point_center(xadd,zadd,-view_setup->ang.y);
			return;
			
		case vm_dir_side:
			*xadd=(int)(((float)x)*mouse_side_view_scale);
			*yadd=-(int)((((float)y)*mouse_side_view_scale));
			*zadd=0;
			rotate_2D_point_center(xadd,zadd,view_setup->ang.y);
			return;
			
		case vm_dir_top:
			sz=(int)((float)(magnify_factor_max-state.magnify_factor)*mouse_top_view_scale);
			if (sz<10) sz=10;
			
			*xadd=-(x*sz);
			*yadd=0;
			*zadd=-(y*sz);
			rotate_2D_point_center(xadd,zadd,view_setup->ang.y);
			return;
			
		default:
			*xadd=*yadd=*zadd=0;
			return;
	}
}

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

bool walk_view_click_item_single_rot_handles(editor_3D_view_setup *view_setup,d3pnt *click_pt,d3pnt *pnt,d3vct *vct,d3ang *ang,int y_size)
{
	int				x,y,z,sz;
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
	
		// click in position
		
	x=pnt->x+(int)vct->x;
	y=y+(int)vct->y;
	z=pnt->z+(int)vct->z;
	
	sz=(int)walk_view_handle_size;
	
	if (walk_view_click_rotate_polygon_in_z(x,y,z)) {
		walk_view_click_project_point(&view_setup->box,&x,&y,&z);
		if ((click_pt->x>=(x-sz)) && (click_pt->x<=(x+sz)) && (click_pt->y>=(y-sz)) && (click_pt->y<=(y+sz))) return(TRUE);
	}
	
	return(FALSE);
}

int walk_view_click_item_rot_handles(editor_3D_view_setup *view_setup,d3pnt *click_pt,d3pnt *pnt,d3ang *ang,int y_size,bool y_only)
{
	float			len;
	d3vct			vct;
	
	len=(float)(map_enlarge*4);
	
		// x rot
		
	if (!y_only) {
		vct.x=len;
		vct.y=0.0f;
		vct.z=0.0f;
		
		if (walk_view_click_item_single_rot_handles(view_setup,click_pt,pnt,&vct,ang,y_size)) return(0);
	}
	
		// y rot
		
	vct.x=0.0f;
	vct.y=-len;
	vct.z=0.0f;
	
	if (walk_view_click_item_single_rot_handles(view_setup,click_pt,pnt,&vct,ang,y_size)) return(1);
	
		// z rot
	
	if (!y_only) {
		vct.x=0.0f;
		vct.y=0.0f;
		vct.z=len;
		
		if (walk_view_click_item_single_rot_handles(view_setup,click_pt,pnt,&vct,ang,y_size)) return(2);
	}
	
	return(-1);
}

bool walk_view_click_rot_handles(editor_3D_view_setup *view_setup,d3pnt *click_pt)
{
	int			n,ncount,type,main_idx,sub_idx,
				y_size,which_axis;
	float		ang_add;
	bool		first_drag;
	d3pnt		pt;
	d3ang		*ang,old_ang;
	
	if (!view_setup->rot_on) return(FALSE);
	
		// check drags for all selections
		
	ncount=select_count();

	which_axis=-1;
	
	for (n=0;n!=ncount;n++) {
		select_get(n,&type,&main_idx,&sub_idx);
		
		which_axis=-1;
		
		switch (type) {
		
			case node_piece:
				ang=&map.nodes[main_idx].ang;
				y_size=map_enlarge*5;
				which_axis=walk_view_click_item_rot_handles(view_setup,click_pt,&map.nodes[main_idx].pnt,ang,y_size,FALSE);
				break;
				
			case spot_piece:
				ang=&map.spots[main_idx].ang;
				y_size=walk_view_model_rot_y_size(&map.spots[main_idx].pnt,ang,map.spots[main_idx].display_model);
				which_axis=walk_view_click_item_rot_handles(view_setup,click_pt,&map.spots[main_idx].pnt,ang,y_size,TRUE);
				break;
				
			case scenery_piece:
				ang=&map.sceneries[main_idx].ang;
				y_size=walk_view_model_rot_y_size(&map.sceneries[main_idx].pnt,ang,map.sceneries[main_idx].model_name);
				which_axis=walk_view_click_item_rot_handles(view_setup,click_pt,&map.sceneries[main_idx].pnt,ang,y_size,FALSE);
				break;
				
		}
		
		if (which_axis!=-1) break;
	}
	
		// had a selection?
		
	if (which_axis==-1) return(FALSE);
	
		// handle drag
	
    if (!os_button_down()) return(FALSE);
	
	undo_push();

	first_drag=TRUE;
	
	memmove(&old_ang,ang,sizeof(d3ang));
	
	while (!os_track_mouse_location(&pt,&view_setup->box)) {
		
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

      View Piece Clicking Utility
      
======================================================= */

bool walk_view_mesh_poly_click_index(editor_3D_view_setup *view_setup,d3pnt *click_pt,map_mesh_type *mesh,int poly_idx,int *hit_z)
{
	int					t,dist,hz,px[8],py[8],pz[8];
	double				dx,dy,dz;
	bool				clip_ok,off_left,off_right,off_top,off_bottom;
	d3pnt				*pt;
	map_mesh_poly_type	*mesh_poly;
	
	mesh_poly=&mesh->polys[poly_idx];

		// check Y clipping
		
	if (view_setup->clip_on) {
		
		clip_ok=TRUE;
		
		for (t=0;t!=mesh_poly->ptsz;t++) {
			if (mesh->vertexes[mesh_poly->v[t]].y>=view_setup->clip_y) {
				clip_ok=FALSE;
				break;
			}
		}
		
		if (clip_ok) return(FALSE);
	}
		
		// translate the points

	for (t=0;t!=mesh_poly->ptsz;t++) {
		pt=&mesh->vertexes[mesh_poly->v[t]];
		px[t]=pt->x;
		py[t]=pt->y;
		pz[t]=pt->z;

		if (!walk_view_click_rotate_polygon_in_z(px[t],py[t],pz[t])) return(FALSE);
				
		walk_view_click_project_point(&view_setup->box,&px[t],&py[t],&pz[t]);
	}
	
		// check if outside box
		
	off_left=off_right=off_top=off_bottom=TRUE;
	
	for (t=0;t!=mesh_poly->ptsz;t++) {
		off_left=off_left&&(px[t]<0);
		off_right=off_right&&(px[t]>(view_setup->box.rx-view_setup->box.lx));
		off_top=off_top&&(py[t]<0);
		off_bottom=off_bottom&&(py[t]>(view_setup->box.by-view_setup->box.ty));
	}

	if ((off_left) || (off_right) || (off_top) || (off_bottom)) return(FALSE);
	
		// check hits
		
	if (!polygon_2D_point_inside(mesh_poly->ptsz,px,py,click_pt->x,click_pt->y)) return(FALSE);
	
		// hit z the closest point to the camera
		
	hz=0;
	
	for (t=0;t!=mesh_poly->ptsz;t++) {
		pt=&mesh->vertexes[mesh_poly->v[t]];
		dx=(double)(view_setup->cpt.x-pt->x);
		dy=(double)(view_setup->cpt.y-pt->y);
		dz=(double)(view_setup->cpt.z-pt->z);
		
		dist=(int)sqrt((dx*dx)+(dy*dy)+(dz*dz));
		
		if (hz==0) {
			hz=dist;
			continue;
		}

		if (hz>dist) hz=dist;
	}
	
	*hit_z=hz;
	
	return(TRUE);
}

bool walk_view_quad_click_index(editor_3D_view_setup *view_setup,d3pnt *click_pt,int *px,int *py,int *pz,int *hit_z)
{
	int					t,fz;
	bool				off_left,off_right,off_top,off_bottom;
	
	fz=0;

	for (t=0;t!=4;t++) {
		if (!walk_view_click_rotate_polygon_in_z(px[t],py[t],pz[t])) return(FALSE);
		walk_view_click_project_point(&view_setup->box,&px[t],&py[t],&pz[t]);
		fz+=pz[t];
	}
	
		// check if outside box
		
	off_left=off_right=off_top=off_bottom=TRUE;
	
	for (t=0;t!=4;t++) {
		off_left=off_left&&(px[t]<0);
		off_right=off_right&&(px[t]>(view_setup->box.rx-view_setup->box.lx));
		off_top=off_top&&(py[t]<0);
		off_bottom=off_bottom&&(py[t]>(view_setup->box.by-view_setup->box.ty));
	}
	
	if ((off_left) || (off_right) || (off_top) || (off_bottom)) return(FALSE);
	
		// check hits
		
	if (!polygon_2D_point_inside(4,px,py,click_pt->x,click_pt->y)) return(FALSE);
	
	*hit_z=fz/4;
	
	return(TRUE);
}

void walk_view_cube_click_index_make_quad(int *x,int *y,int *z,int *px,int *py,int *pz,int v1,int v2,int v3,int v4)
{
	px[0]=x[v1];
	px[1]=x[v2];
	px[2]=x[v3];
	px[3]=x[v4];
	py[0]=y[v1];
	py[1]=y[v2];
	py[2]=y[v3];
	py[3]=y[v4];
	pz[0]=z[v1];
	pz[1]=z[v2];
	pz[2]=z[v3];
	pz[3]=z[v4];
}

bool walk_view_cube_click_index(editor_3D_view_setup *view_setup,d3pnt *click_pt,int *x,int *y,int *z,int *hit_z)
{
	int					px[4],py[4],pz[4];
	bool				hit;
	
	hit=FALSE;
	
		// top
		
	walk_view_cube_click_index_make_quad(x,y,z,px,py,pz,0,1,2,3);
	hit=hit||walk_view_quad_click_index(view_setup,click_pt,px,py,pz,hit_z);
	
		// bottom
		
	walk_view_cube_click_index_make_quad(x,y,z,px,py,pz,4,5,6,7);
	hit=hit||walk_view_quad_click_index(view_setup,click_pt,px,py,pz,hit_z);
	
		// front
		
	walk_view_cube_click_index_make_quad(x,y,z,px,py,pz,3,2,5,6);
	hit=hit||walk_view_quad_click_index(view_setup,click_pt,px,py,pz,hit_z);

		// back
		
	walk_view_cube_click_index_make_quad(x,y,z,px,py,pz,0,1,4,7);
	hit=hit||walk_view_quad_click_index(view_setup,click_pt,px,py,pz,hit_z);
	
		// left
		
	walk_view_cube_click_index_make_quad(x,y,z,px,py,pz,0,7,6,3);
	hit=hit||walk_view_quad_click_index(view_setup,click_pt,px,py,pz,hit_z);

		// right
		
	walk_view_cube_click_index_make_quad(x,y,z,px,py,pz,1,2,5,4);
	return(hit||walk_view_quad_click_index(view_setup,click_pt,px,py,pz,hit_z));
}

bool walk_view_liquid_click(editor_3D_view_setup *view_setup,d3pnt *click_pt,map_liquid_type *liq,int *hit_z)
{
	int				px[4],py[4],pz[4];

	px[0]=px[3]=liq->lft;
	px[1]=px[2]=liq->rgt;
	py[0]=py[1]=py[2]=py[3]=liq->y;
	pz[0]=pz[1]=liq->top;
	pz[2]=pz[3]=liq->bot;
	
	return(walk_view_quad_click_index(view_setup,click_pt,px,py,pz,hit_z));
}

/* =======================================================

      View Piece Get Clicked Index
      
======================================================= */

void walk_view_mesh_click_index(editor_3D_view_setup *view_setup,d3pnt *click_pt,int *type,int *main_idx,int *sub_idx,bool sel_only)
{
	int					n,k,fz,box_wid,box_high,
						px[8],py[8],pz[8],hit_z;
	map_mesh_type		*mesh;
	spot_type			*spot;
	map_scenery_type	*scenery;
	map_light_type		*map_light;
	map_sound_type		*map_sound;
	map_particle_type	*map_particle;
	node_type			*node;
	
	walk_view_click_setup_project(view_setup);
	
	box_wid=view_setup->box.rx-view_setup->box.lx;
	box_high=view_setup->box.by-view_setup->box.ty;
	
	*type=-1;
	hit_z=walk_view_max_z_click;
		
		// meshes
		
	mesh=map.mesh.meshes;
	
	for (n=0;n!=map.mesh.nmesh;n++) {
	
			// check polygons
			
		for (k=0;k!=mesh->npoly;k++) {
		
			if (walk_view_mesh_poly_click_index(view_setup,click_pt,mesh,k,&fz)) {
				if (fz<hit_z) {
					hit_z=fz;
					*type=mesh_piece;
					*main_idx=n;
					*sub_idx=k;
				}
			}

		}
	
		mesh++;
	}
	
		// liquids
		
	if (state.show_liquid) {
			
		for (n=0;n!=map.liquid.nliquid;n++) {
			if (walk_view_liquid_click(view_setup,click_pt,&map.liquid.liquids[n],&fz)) {
				if (fz<hit_z) {
					hit_z=fz;
					*type=liquid_piece;
					*main_idx=n;
					*sub_idx=-1;
				}
			}
		}
	}
	
	if (state.show_object) {
	
			// spots
			
		for (n=0;n!=map.nspot;n++) {
			
			spot=&map.spots[n];
			
			if (!walk_view_model_click_select_size(spot->display_model,&spot->pnt,&spot->ang,px,py,pz)) continue;
			
			if (walk_view_cube_click_index(view_setup,click_pt,px,py,pz,&fz)) {
				if (fz<hit_z) {
					hit_z=fz;
					*type=spot_piece;
					*main_idx=n;
					*sub_idx=-1;
				}
			}
		}
	
			// scenery
			
		for (n=0;n!=map.nscenery;n++) {
			
			scenery=&map.sceneries[n];
			
			if (!walk_view_model_click_select_size(scenery->model_name,&scenery->pnt,&scenery->ang,px,py,pz)) continue;
			
			if (walk_view_cube_click_index(view_setup,click_pt,px,py,pz,&fz)) {
				if (fz<hit_z) {
					hit_z=fz;
					*type=scenery_piece;
					*main_idx=n;
					*sub_idx=-1;
				}
			}
		}
	}
	
	if (state.show_lightsoundparticle) {
	
			// map lights
			
		for (n=0;n!=map.nlight;n++) {
		
			map_light=&map.lights[n];
			
			walk_view_sprite_select_size(&map_light->pnt,px,py,pz);
			
			if (walk_view_cube_click_index(view_setup,click_pt,px,py,pz,&fz)) {
				if (fz<hit_z) {
					hit_z=fz;
					*type=light_piece;
					*main_idx=n;
					*sub_idx=-1;
				}
			}
		}
		
			// map sounds
			
		for (n=0;n!=map.nsound;n++) {
		
			map_sound=&map.sounds[n];
			
			walk_view_sprite_select_size(&map_sound->pnt,px,py,pz);
			
			if (walk_view_cube_click_index(view_setup,click_pt,px,py,pz,&fz)) {
				if (fz<hit_z) {
					hit_z=fz;
					*type=sound_piece;
					*main_idx=n;
					*sub_idx=-1;
				}
			}
		}
		
			// map particles
			
		for (n=0;n!=map.nparticle;n++) {
		
			map_particle=&map.particles[n];
			
			walk_view_sprite_select_size(&map_particle->pnt,px,py,pz);
			
			if (walk_view_cube_click_index(view_setup,click_pt,px,py,pz,&fz)) {
				if (fz<hit_z) {
					hit_z=fz;
					*type=particle_piece;
					*main_idx=n;
					*sub_idx=-1;
				}
			}
		}
	}
	
	if (state.show_node) {
	
			// map nodes
			
		for (n=0;n!=map.nnode;n++) {
		
			node=&map.nodes[n];
			
			walk_view_sprite_select_size(&node->pnt,px,py,pz);
			
			if (walk_view_cube_click_index(view_setup,click_pt,px,py,pz,&fz)) {
				if (fz<hit_z) {
					hit_z=fz;
					*type=node_piece;
					*main_idx=n;
					*sub_idx=-1;
				}
			}
		}
	}
}
	
void walk_view_click_piece_normal(editor_3D_view_setup *view_setup,d3pnt *pt,bool dblclick)
{
	int				type,main_idx,sub_idx;
	bool			toggle_select;
	
		// anything clicked?
		
	walk_view_mesh_click_index(view_setup,pt,&type,&main_idx,&sub_idx,FALSE);
	
		// if a node, check link
		// connections
		
	if ((type==node_piece) && (state.node_mode!=node_mode_select)) node_link_click(main_idx);
	
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
	
		// redraw
		
	menu_fix_enable();
	
	main_wind_draw();
	texture_palette_reset();
}

/* =======================================================

      View Map Clicking
      
======================================================= */

void walk_view_click_piece(editor_3D_view_setup *view_setup,d3pnt *pt,int view_move_dir,bool dblclick)
{
		// put click within box
	
	pt->x-=view_setup->box.lx;
	pt->y-=view_setup->box.ty;
	
		// rotation handles
		
	if (walk_view_click_rot_handles(view_setup,pt)) return;
	
		// liquid vertex drags
		
	if (walk_view_click_drag_liquid_vertex(view_setup,pt,view_move_dir)) return;
	
		// mesh vertex drags
		
	switch (state.drag_mode) {
	
		case drag_mode_vertex:
			if (walk_view_click_drag_vertex(view_setup,pt,view_move_dir)) return;
			break;
	
		case drag_mode_mesh:
			if (walk_view_click_drag_mesh_handle(view_setup,pt,view_move_dir)) return;
			break;
			
	}
		
		// select mesh/polygon
		
	walk_view_click_piece_normal(view_setup,pt,dblclick);
	
		// changes in palette
		
	palette_reset();
	
		// item drags
			
	if (walk_view_click_drag_item(view_setup,pt,view_move_dir)) return;
	
		// liquid drags
		
	if (walk_view_click_drag_liquid(view_setup,pt,view_move_dir)) return;
	
		// mesh or poly drags
		
	switch (state.drag_mode) {
	
		case drag_mode_mesh:
			if (!os_key_control_down()) {
				walk_view_click_drag_mesh(view_setup,pt,view_move_dir);
			}
			else {
				walk_view_click_drag_texture(view_setup,pt,view_move_dir,TRUE);
			}
			break;

		case drag_mode_polygon:
			if (!os_key_control_down()) {
				walk_view_click_drag_mesh_poly(view_setup,pt,view_move_dir);
			}
			else {
				walk_view_click_drag_texture(view_setup,pt,view_move_dir,FALSE);
			}
			break;
			
	}
}



