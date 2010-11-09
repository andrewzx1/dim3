/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Model Window Clicking

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

#include "model.h"
#include "interface.h"

extern int						cur_mesh,cur_pose,cur_bone,magnify_z;
extern bool						shift_on,rotate_on,size_on,drag_sel_on;
extern d3pnt					shift;
extern d3ang					ang;
extern d3rect					model_box,drag_sel_box;

extern animator_state_type		state;

extern model_type				model;
extern model_draw_setup			draw_setup;

extern void draw_model_bones_get_handle_rot(model_type *model,model_draw_setup *draw_setup,int bone_idx,d3ang *rot);
extern float draw_model_bones_drag_handle_offset(model_type *model);
extern void draw_model_bones_drag_handle_calc(float x,float y,float z,d3vct *vct,d3ang *ang,float *hx,float *hy,float *hz);

// supergumba -- this code repeats draw setups and only uses the
//               view picker for trig hits, fix all this

/* =======================================================

      Model Vertex Selection
      
======================================================= */

void model_sel_vertex(float *pv,d3rect *box,bool chg_sel,double *mod_matrix,double *proj_matrix,double *vport)
{
	int					n,x,y,nt;
	double				sx,sy,sz,dx,dy,dz;
	d3rect				wbox;
	model_vertex_type	*vertex;
	model_mesh_type		*mesh;
	
	os_get_window_box(&wbox);

		// find selection
		
	mesh=&model.meshes[cur_mesh];
		
	nt=mesh->nvertex;
	vertex=mesh->vertexes;

	for (n=0;n!=nt;n++) {
		sx=(int)*pv++;
		sy=(int)*pv++;
		sz=(int)*pv++;
		gluProject(sx,sy,sz,mod_matrix,proj_matrix,(GLint*)vport,&dx,&dy,&dz);
		x=(int)dx;
		y=(wbox.by-((int)dy))-model_box.ty;
		
		if ((x>=box->lx) && (x<=box->rx) && (y>=box->ty) && (y<=box->by)) {
			if (!vertex_check_hide_mask(cur_mesh,n)) vertex_set_sel_mask(cur_mesh,n,chg_sel);
		}
		
		vertex++;
	}
}

void select_model_wind_save_sel_state(char *vertex_sel)
{
	int					n,nt;
	model_vertex_type	*vertex;

	nt=model.meshes[cur_mesh].nvertex;
	vertex=model.meshes[cur_mesh].vertexes;

	for (n=0;n!=nt;n++) {
		vertex_sel[n]=(char)vertex_check_sel_mask(cur_mesh,n);
		vertex++;
	}
}

void select_model_wind_restore_sel_state(char *vertex_sel)
{
	int					n,nt;
	model_vertex_type	*vertex;

	nt=model.meshes[cur_mesh].nvertex;
	vertex=model.meshes[cur_mesh].vertexes;

	for (n=0;n!=nt;n++) {
		vertex_set_sel_mask(cur_mesh,n,(vertex_sel[n]!=0));
		vertex++;
	}
}

void select_model_wind_vertex(d3pnt *start_pnt,unsigned long modifiers,float *pv,double *mod_matrix,double *proj_matrix,double *vport)
{
	char					*org_vertex_sel;
	bool					chg_sel;
	d3pnt					pnt,last_pnt;
	
		// turn on or off?
		
	org_vertex_sel=(char*)malloc(model.meshes[cur_mesh].nvertex);
		
	if ((modifiers&shiftKey)!=0) {
		select_model_wind_save_sel_state(org_vertex_sel);
		SetThemeCursor(kThemePlusCursor);
		chg_sel=TRUE;
	}
	else {
		if ((modifiers&controlKey)!=0) {
			select_model_wind_save_sel_state(org_vertex_sel);
			SetThemeCursor(kThemePoofCursor);
			chg_sel=FALSE;
		}
		else {
			memset(org_vertex_sel,0x0,model.meshes[cur_mesh].nvertex);
			os_set_arrow_cursor();
			chg_sel=TRUE;
		}
	}
	
		// drag the selection

	last_pnt.x=last_pnt.y=-1;
	
	drag_sel_on=TRUE;
	
	while (!os_track_mouse_location(&pnt,&model_box)) {
		
		if ((last_pnt.x==pnt.x) && (last_pnt.y==pnt.y)) continue;
		memmove(&last_pnt,&pnt,sizeof(d3pnt));
		
		if (start_pnt->x<last_pnt.x) {
			drag_sel_box.lx=start_pnt->x;
			drag_sel_box.rx=last_pnt.x;
		}
		else {
			drag_sel_box.rx=start_pnt->x;
			drag_sel_box.lx=last_pnt.x;
		}
		if (start_pnt->y<last_pnt.y) {
			drag_sel_box.ty=start_pnt->y;
			drag_sel_box.by=last_pnt.y;
		}
		else {
			drag_sel_box.by=start_pnt->y;
			drag_sel_box.ty=last_pnt.y;
		}
		
		select_model_wind_restore_sel_state(org_vertex_sel);
		model_sel_vertex(pv,&drag_sel_box,chg_sel,mod_matrix,proj_matrix,vport);
		
		main_wind_draw();
	}
	
	drag_sel_on=FALSE;
	
	free(org_vertex_sel);
}

/* =======================================================

      Model Trig Selection
      
======================================================= */

bool select_model_wind_polygon(d3pnt *start_pnt)
{
	int					n,k,idx,ntrig;
	float				*pv;
	d3pnt				v_pnts[3];
    model_trig_type		*trig;
	model_mesh_type		*mesh;
	
		// clicking mesh
		
	mesh=&model.meshes[cur_mesh];
	
		// draw and pick the triangles
		
	if (!model_pick_list_start(mesh->ntrig)) return(FALSE);
	
		// draw the mesh
		
	model_draw_setup_initialize(&model,&draw_setup,TRUE);
	draw_model_setup_bones_vertexes(&model,cur_mesh,&draw_setup);

	ntrig=mesh->ntrig;
	trig=mesh->trigs;
    
    for (n=0;n!=ntrig;n++) {
	
		if (!vertex_check_hide_mask_trig(cur_mesh,trig)) {
			
			for (k=0;k!=3;k++) {
				pv=draw_setup.mesh_arrays[cur_mesh].gl_vertex_array+(trig->v[k]*3);
				v_pnts[k].x=(int)*pv++;
				v_pnts[k].y=(int)*pv++;
				v_pnts[k].z=(int)*pv;
			}
			
			model_pick_list_add_trig(n,v_pnts);
		}
		
		trig++;
    }
	
	model_draw_setup_shutdown(&model,&draw_setup);
	
	model_pick_list_end(start_pnt,&idx);
	
	state.sel_trig_idx=idx;

	if (state.sel_trig_idx==-1) return(FALSE);
	
		// select all the vertexes attached to trig
	
	vertex_clear_sel_mask(cur_mesh);
	
	trig=&mesh->trigs[idx];
	
	for (k=0;k!=3;k++) {
		vertex_set_sel_mask(cur_mesh,trig->v[k],TRUE);
	}

	return(TRUE);
}

/* =======================================================

      Model Select Main Line
      
======================================================= */

void select_model_wind(d3pnt *start_pnt,unsigned long modifiers)
{
	int				sz;
	float			*pv;
	double			mod_matrix[16],proj_matrix[16],vport[4];
	
		// no playing while selecting
		
	model_wind_play(FALSE,FALSE);
	
		// get the draw vertexes
		// need to save off array as drawing will reuse
		// array and free it
		
	model_draw_setup_initialize(&model,&draw_setup,TRUE);
		
	draw_model_setup_pose(&model,&draw_setup,cur_pose);
	
	model_create_draw_bones(&model,&draw_setup);
	model_create_draw_vertexes(&model,cur_mesh,&draw_setup);
	
	sz=(model.meshes[cur_mesh].nvertex*3)*sizeof(float);
	pv=(float*)malloc(sz);
	if (pv==NULL) return;
	
	memmove(pv,draw_setup.mesh_arrays[cur_mesh].gl_vertex_array,sz);
		
	model_draw_setup_shutdown(&model,&draw_setup);
	
		// setup transforms
		
	draw_model_gl_setup(&model,0);
	glGetDoublev(GL_MODELVIEW_MATRIX,mod_matrix);
	glGetDoublev(GL_PROJECTION_MATRIX,proj_matrix);
	glGetIntegerv(GL_VIEWPORT,(GLint*)vport);

		// run the correct click
		
	if (state.select_mode==select_mode_polygon) {
		if (!select_model_wind_polygon(start_pnt)) {
			select_model_wind_vertex(start_pnt,modifiers,pv,mod_matrix,proj_matrix,vport);
		}
	}
	else {
		select_model_wind_vertex(start_pnt,modifiers,pv,mod_matrix,proj_matrix,vport);
	}
	
		// free the saved vertexes

	free(pv);

		// redraw the model
		
	main_wind_draw();
		
		// reset the data browser
		
	hilite_vertex_rows();

	os_set_arrow_cursor();	
}

/* =======================================================

      Model Movement
      
======================================================= */

void change_model_wind(d3pnt *start_pnt)
{
	int						old_magnify_z;
	d3pnt					pnt,last_pnt,old_shift;
	d3ang					old_ang;
	
		// rotate, shift, or size the model

	last_pnt.x=last_pnt.y=-1;
	
	old_shift.x=shift.x;
	old_shift.y=shift.y;
	
	old_ang.x=ang.x;
	old_ang.y=ang.y;
	
	old_magnify_z=magnify_z;
	
	while (!os_track_mouse_location(&pnt,&model_box)) {
		
		if ((last_pnt.x==pnt.x) && (last_pnt.y==pnt.y)) continue;
		memmove(&last_pnt,&pnt,sizeof(d3pnt));
			
		if (shift_on) {
			shift.x=old_shift.x+((last_pnt.x-start_pnt->x)*4);
			shift.y=old_shift.y-((last_pnt.y-start_pnt->y)*4);
		}
		if (rotate_on) {
			ang.x=old_ang.x-(float)((last_pnt.y-start_pnt->y)/5);
			ang.y=old_ang.y+(float)((last_pnt.x-start_pnt->x)/5);
		}
		if (size_on) {
			magnify_z=old_magnify_z+((last_pnt.y-start_pnt->y)*2);
		}
		
		if (!state.playing) main_wind_draw();
	
	}
	
	if (!state.playing) main_wind_draw();
}

/* =======================================================

      Model Window Bone Clicks
      
======================================================= */

bool draw_bone_model_wind_click_box(d3pnt *start_pnt,float x,float y,float z)
{
	int				ix,iy;
	double			dx,dy,dz,mod_matrix[16],proj_matrix[16],vport[4];
	d3rect			wbox;
	
	os_get_window_box(&wbox);
	
		// reverse the projection
		
	glGetDoublev(GL_MODELVIEW_MATRIX,mod_matrix);
	glGetDoublev(GL_PROJECTION_MATRIX,proj_matrix);
	glGetIntegerv(GL_VIEWPORT,(GLint*)vport);
	
	gluProject(x,y,z,mod_matrix,proj_matrix,(GLint*)vport,&dx,&dy,&dz);
	ix=(int)dx;
	iy=(wbox.by-((int)dy))-model_box.ty;

		// check box
		
	return((start_pnt->x>=(ix-6)) && (start_pnt->x<=(ix+6)) && (start_pnt->y>=(iy-6)) && (start_pnt->y<=(iy+6)));
}

bool drag_bone_model_wind(d3pnt *start_pnt)
{
	int						n,k,drag_handle;
	float					x,y,z,hx,hy,hz,org_ang,org_mov,
							bone_drag_handle_offset;
	float					*ang,*mov;
	d3pnt					pnt,last_pnt;
	d3vct					vct;
	d3ang					hang,rot;
	model_draw_bone_type	*draw_bone;
	
	if (model.nbone==0) return(FALSE);
	
	model_wind_play(FALSE,FALSE);
	
		// setup the draw pose
		
	draw_model_setup_pose(&model,&draw_setup,cur_pose);
	model_create_draw_bones(&model,&draw_setup);
	
		// setup transforms
		
	draw_model_gl_setup(&model,0);
	
		// click on any drag handles?
		
	drag_handle=drag_handle_none;
	
	if ((cur_pose==-1) || (cur_bone!=-1)) {
	
		draw_bone=&draw_setup.bones[cur_bone];
		
		x=draw_bone->fpnt.x+draw_setup.move.x;
		y=draw_bone->fpnt.y+draw_setup.move.y;
		z=draw_bone->fpnt.z+draw_setup.move.z;
		
		bone_drag_handle_offset=draw_model_bones_drag_handle_offset(&model);

		draw_model_bones_get_handle_rot(&model,&draw_setup,cur_bone,&rot);
		
			// x drag bone

		vct.x=bone_drag_handle_offset;
		vct.y=0;
		vct.z=0;
		hang.x=0;
		hang.y=rot.y;
		hang.z=rot.z;
		draw_model_bones_drag_handle_calc(x,y,z,&vct,&hang,&hx,&hy,&hz);
		if (draw_bone_model_wind_click_box(start_pnt,hx,hy,hz)) drag_handle=drag_handle_x;
		
			// y drag bone
			
		vct.x=0;
		vct.y=bone_drag_handle_offset;
		vct.z=0;
		hang.x=rot.x;
		hang.y=0;
		hang.z=rot.z;
		draw_model_bones_drag_handle_calc(x,y,z,&vct,&hang,&hx,&hy,&hz);
		if (draw_bone_model_wind_click_box(start_pnt,hx,hy,hz)) drag_handle=drag_handle_y;
		
			// z drag bone
			
		vct.x=0;
		vct.y=0;
		vct.z=bone_drag_handle_offset;
		hang.x=rot.x;
		hang.y=rot.y;
		hang.z=0;
		draw_model_bones_drag_handle_calc(x,y,z,&vct,&hang,&hx,&hy,&hz);
		if (draw_bone_model_wind_click_box(start_pnt,hx,hy,hz)) drag_handle=drag_handle_z;
	}
	
		// click on any bones?
		
	if (drag_handle==drag_handle_none) {
	
		k=-1;
		draw_bone=draw_setup.bones;
			
		for (n=0;n!=model.nbone;n++) {
			x=draw_bone->fpnt.x+draw_setup.move.x;
			y=draw_bone->fpnt.y+draw_setup.move.y;
			z=draw_bone->fpnt.z+draw_setup.move.z;
			
			if (draw_bone_model_wind_click_box(start_pnt,x,y,z)) {
				k=n;
				break;
			}

			draw_bone++;
		}
		
		if (k==-1) return(FALSE);
	
			// select as current bone
			
		cur_bone=k;
		reset_bone_list();
		main_wind_draw();

		return(TRUE);
	}
	
		// get drag angle
		
	switch (drag_handle) {
	
		case drag_handle_x:
			ang=&model.poses[cur_pose].bone_moves[cur_bone].rot.x;
			mov=&model.poses[cur_pose].bone_moves[cur_bone].mov.x;
			break;
		
		case drag_handle_y:
			ang=&model.poses[cur_pose].bone_moves[cur_bone].rot.y;
			mov=&model.poses[cur_pose].bone_moves[cur_bone].mov.y;
			break;
			
		case drag_handle_z:
			ang=&model.poses[cur_pose].bone_moves[cur_bone].rot.z;
			mov=&model.poses[cur_pose].bone_moves[cur_bone].mov.z;
			break;
		
		default:
			return(TRUE);
			
	}

	reset_bone_list();
	
		// drag bone

	org_ang=*ang;
	org_mov=*mov;
	last_pnt.x=last_pnt.y=-1;
	
	undo_set_bone_move(cur_pose,cur_bone);
	
	SetThemeCursor(kThemeClosedHandCursor);
		
	while (!os_track_mouse_location(&pnt,&model_box)) {
		
		if ((last_pnt.x==pnt.x) && (last_pnt.y==pnt.y)) continue;
		memmove(&last_pnt,&pnt,sizeof(d3pnt));
		
		x=pnt.x-start_pnt->x;
		if (x<-180) x=-180;
		if (x>180) x=180;
		
		if (state.drag_bone_mode==drag_bone_mode_rotate) {
			*ang=org_ang+(((float)x)/2.0f);
		}
		else {
			*mov=org_mov+(((float)x)/20.0f);
		}
		
			// draw the model
			
		main_wind_draw();
		
		reset_bone_list();
		redraw_bone_list();

	}

	os_set_arrow_cursor();

		// redraw model
		
	main_wind_draw();

	return(TRUE);
}

/* =======================================================

      Model Window Hit Box Clicks
      
======================================================= */

bool drag_hit_box_handle_model_wind(d3pnt *start_pnt)
{
	int						n,k,box_idx,pt_idx,xsz,zsz,ysz,offx,offz,offy,
							kx,ky,kz,x[8],y[8],z[8];
	bool					model_hit_box_drag_on;
	d3pnt					org_pnt,org_cnt,last_pnt,pnt;
	model_box_type			*box;
	
	model_wind_play(FALSE,FALSE);
	
		// setup the draw pose
		
	draw_model_setup_pose(&model,&draw_setup,cur_pose);
	model_create_draw_bones(&model,&draw_setup);
	
		// setup transforms
		
	draw_model_gl_setup(&model,0);

		// find a click
		
	box_idx=pt_idx=-1;
		
	for (n=0;n<model.nhit_box;n++) {
		box=&model.hit_boxes[n].box;

		xsz=box->size.x/2;
		offx=box->offset.x;
		zsz=box->size.z/2;
		offz=box->offset.z;
		ysz=box->size.y;
		offy=box->offset.y;
		
		x[0]=x[1]=x[4]=x[5]=offx-xsz;
		x[2]=x[3]=x[6]=x[7]=offx+xsz;
		y[0]=y[1]=y[2]=y[3]=offy-ysz;
		y[4]=y[5]=y[6]=y[7]=offy;
		z[0]=z[3]=z[4]=z[7]=offz-zsz;
		z[1]=z[2]=z[5]=z[6]=offz+zsz;
	
		for (k=0;k!=8;k++) {
			model_get_point_position(&draw_setup,&x[k],&y[k],&z[k]);
			if (draw_bone_model_wind_click_box(start_pnt,x[k],y[k],z[k])) {
				box_idx=n;
				pt_idx=k;
				break;
			}
		}
		
		if (box_idx!=-1) break;
	}
	
	if (box_idx==-1) return(FALSE);
	
		// get original size
		
	box=&model.hit_boxes[box_idx].box;
	memmove(&org_pnt,&box->size,sizeof(d3pnt));
	memmove(&org_cnt,&box->offset,sizeof(d3pnt));
	
		// drag handle
		
	model_hit_box_drag_on=FALSE;

	last_pnt.x=last_pnt.y=-1;
	
	SetThemeCursor(kThemeClosedHandCursor);
		
	while (!os_track_mouse_location(&pnt,&model_box)) {
		
		if ((last_pnt.x==pnt.x) && (last_pnt.y==pnt.y)) continue;
		memmove(&last_pnt,&pnt,sizeof(d3pnt));
		
		kx=(pnt.x-start_pnt->x)*5;
		ky=(pnt.y-start_pnt->y)*5;
		kz=0;
		
		rotate_point_center(&kx,&ky,&kz,ang.x,ang.y,0.0f);

		if ((pt_idx==0) || (pt_idx==1) || (pt_idx==4) || (pt_idx==5)) {
			box->size.x=org_pnt.x-kx;
		}
		else {
			box->size.x=org_pnt.x+kx;
		}
		if ((pt_idx==0) || (pt_idx==1) || (pt_idx==2) || (pt_idx==3)) {
			box->size.y=org_pnt.y-ky;
		}
		else {
			box->size.y=org_pnt.y+ky;
			box->offset.y=org_cnt.y+ky;
		}
		if ((pt_idx==0) || (pt_idx==3) || (pt_idx==4) || (pt_idx==7)) {
			box->size.z=org_pnt.z-kz;
		}
		else {
			box->size.z=org_pnt.z+kz;
		}
		
			// draw the model
			
		main_wind_draw();
		model_hit_box_drag_on=TRUE;
	}

	os_set_arrow_cursor();

		// redraw model
		
	main_wind_draw();

	return(model_hit_box_drag_on);
}

/* =======================================================

      Model Window Clicks
      
======================================================= */

void click_model_wind(d3pnt *pnt,unsigned long modifiers)
{
	model_wind_reset_modifiers();
	
		// get click within window
		
	pnt->x-=model_box.lx;
	pnt->y-=model_box.ty;
	
		// handle the clicks
	
	if ((shift_on) || (rotate_on) || (size_on)) {
		change_model_wind(pnt);
		return;
	}
	
	if (state.bone) {
		if (drag_bone_model_wind(pnt)) return;
	}
	
	if (state.hit_box) {
		drag_hit_box_handle_model_wind(pnt);
		return;
	}
		
	select_model_wind(pnt,modifiers);
}

