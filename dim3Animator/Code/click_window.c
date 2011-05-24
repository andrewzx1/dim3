/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Model Window Clicking

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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3animator.h"
#endif

#include "glue.h"
#include "interface.h"

extern model_type				model;
extern model_draw_setup			draw_setup;
extern animator_state_type		state;

// supergumba -- this code repeats draw setups and only uses the
//               view picker for trig hits, fix all this

/* =======================================================

      Model Drag Selection
      
======================================================= */

void model_drag_sel_vertex(float *pv,d3rect *box,bool chg_sel)
{
	int					n,nt;
	d3fpnt				pnt;
	d3pnt				tran_pnt;
	model_mesh_type		*mesh;

		// find selection
		
	mesh=&model.meshes[state.cur_mesh_idx];
		
	nt=mesh->nvertex;

	for (n=0;n!=nt;n++) {
	
		pnt.x=*pv++;
		pnt.y=*pv++;
		pnt.z=*pv++;
		
		draw_model_2D_transform(&pnt,&tran_pnt);
		
		if ((tran_pnt.x>=box->lx) && (tran_pnt.x<=box->rx) && (tran_pnt.y>=box->ty) && (tran_pnt.y<=box->by)) {
			if (!vertex_check_hide_mask(state.cur_mesh_idx,n)) vertex_set_sel_mask(state.cur_mesh_idx,n,chg_sel);
		}
	}
}

void select_model_wind_save_drag_sel_state(char *vertex_sel)
{
	int					n,nt;
	model_vertex_type	*vertex;

	nt=model.meshes[state.cur_mesh_idx].nvertex;
	vertex=model.meshes[state.cur_mesh_idx].vertexes;

	for (n=0;n!=nt;n++) {
		vertex_sel[n]=(char)vertex_check_sel_mask(state.cur_mesh_idx,n);
		vertex++;
	}
}

void select_model_wind_restore_drag_sel_state(char *vertex_sel)
{
	int					n,nt;
	model_vertex_type	*vertex;

	nt=model.meshes[state.cur_mesh_idx].nvertex;
	vertex=model.meshes[state.cur_mesh_idx].vertexes;

	for (n=0;n!=nt;n++) {
		vertex_set_sel_mask(state.cur_mesh_idx,n,(vertex_sel[n]!=0));
		vertex++;
	}
}

void select_model_wind_select_trig_for_vertex_drag_sel(void)
{
	int					n,k,nt;
	model_trig_type		*trig;

	trig_clear_sel_mask(state.cur_mesh_idx);

	nt=model.meshes[state.cur_mesh_idx].ntrig;
	trig=model.meshes[state.cur_mesh_idx].trigs;

	for (n=0;n!=nt;n++) {
		for (k=0;k!=3;k++) {
			if (vertex_check_sel_mask(state.cur_mesh_idx,trig->v[k])) {
				trig_set_sel_mask(state.cur_mesh_idx,n,TRUE);
				break;
			}
		}
		trig++;
	}
}

void select_model_wind_vertex_drag_sel(d3pnt *start_pnt,float *pv)
{
	char					*org_vertex_sel;
	bool					chg_sel;
	d3rect					mbox;
	d3pnt					pnt,last_pnt;
	
		// turn on or off?
		
	org_vertex_sel=(char*)malloc(model.meshes[state.cur_mesh_idx].nvertex);
		
	if (os_key_shift_down()) {
		select_model_wind_save_drag_sel_state(org_vertex_sel);
		os_set_add_cursor();
		chg_sel=TRUE;
	}
	else {
		if (os_key_control_down()) {
			select_model_wind_save_drag_sel_state(org_vertex_sel);
			os_set_subtract_cursor();
			chg_sel=FALSE;
		}
		else {
			memset(org_vertex_sel,0x0,model.meshes[state.cur_mesh_idx].nvertex);
			os_set_arrow_cursor();
			chg_sel=TRUE;
		}
	}
	
		// drag the selection

	last_pnt.x=last_pnt.y=-1;
	
	state.drag_sel_on=TRUE;
	model_wind_get_box(&mbox);
	
	while (!os_track_mouse_location(&pnt,&mbox)) {
		
		if ((last_pnt.x==pnt.x) && (last_pnt.y==pnt.y)) continue;
		memmove(&last_pnt,&pnt,sizeof(d3pnt));
		
		if (start_pnt->x<last_pnt.x) {
			state.drag_sel_box.lx=start_pnt->x;
			state.drag_sel_box.rx=last_pnt.x;
		}
		else {
			state.drag_sel_box.rx=start_pnt->x;
			state.drag_sel_box.lx=last_pnt.x;
		}
		if (start_pnt->y<last_pnt.y) {
			state.drag_sel_box.ty=start_pnt->y;
			state.drag_sel_box.by=last_pnt.y;
		}
		else {
			state.drag_sel_box.by=start_pnt->y;
			state.drag_sel_box.ty=last_pnt.y;
		}
		
		select_model_wind_restore_drag_sel_state(org_vertex_sel);
		model_drag_sel_vertex(pv,&state.drag_sel_box,chg_sel);

		if (state.select_mode==select_mode_polygon) select_model_wind_select_trig_for_vertex_drag_sel();
		
		main_wind_draw();
	}
	
	state.drag_sel_on=FALSE;
	
	free(org_vertex_sel);
}

/* =======================================================

      Model Vertex Selection
      
======================================================= */

bool select_model_wind_vertex(d3pnt *start_pnt,float *pv)
{
	int					n,nt,idx;
	d3fpnt				pnt;
	d3pnt				tran_pnt;
	model_mesh_type		*mesh;

		// clicked on a vertex?
		
	mesh=&model.meshes[state.cur_mesh_idx];
		
	idx=-1;
	nt=mesh->nvertex;

	for (n=0;n!=nt;n++) {
	
		pnt.x=*pv++;
		pnt.y=*pv++;
		pnt.z=*pv++;
		
		draw_model_2D_transform(&pnt,&tran_pnt);
		
		if ((start_pnt->x>=(tran_pnt.x-5)) && (start_pnt->x<=(tran_pnt.x+5)) && (start_pnt->y>=(tran_pnt.y-5)) && (start_pnt->y<=(tran_pnt.y+5))) {
			idx=n;
			break;
		}
	}

	if (idx==-1) return(FALSE);

	if (!os_key_shift_down()) vertex_clear_sel_mask(state.cur_mesh_idx);
	vertex_set_sel_mask(state.cur_mesh_idx,idx,TRUE);

	return(TRUE);
}

/* =======================================================

      Model Trig Selection
      
======================================================= */

bool select_model_wind_polygon(d3pnt *start_pnt,bool check_only)
{
	int					n,k,idx,ntrig;
	float				*pv;
	d3pnt				v_pnts[3];
    model_trig_type		*trig;
	model_mesh_type		*mesh;
	
		// clicking mesh
		
	mesh=&model.meshes[state.cur_mesh_idx];
	
		// draw and pick the triangles
		
	if (!model_pick_list_start(mesh->ntrig)) return(FALSE);
	
		// draw the mesh
		
	model_draw_setup_initialize(&model,&draw_setup,TRUE);
	draw_model_setup_bones_vertexes(state.cur_mesh_idx);

	ntrig=mesh->ntrig;
	trig=mesh->trigs;
    
    for (n=0;n!=ntrig;n++) {
	
		if (!vertex_check_hide_mask_trig(state.cur_mesh_idx,trig)) {
			
			for (k=0;k!=3;k++) {
				pv=draw_setup.mesh_arrays[state.cur_mesh_idx].gl_vertex_array+(trig->v[k]*3);
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

	if (idx==-1) return(FALSE);

		// are we checking only?

	if (check_only) return(TRUE);

		// run the selection

	if (!os_key_shift_down()) trig_clear_sel_mask(state.cur_mesh_idx);
	trig_set_sel_mask(state.cur_mesh_idx,idx,TRUE);
	
		// select all the vertexes attached to trig

	vertex_set_sel_mask_trig_mask(state.cur_mesh_idx);

	return(TRUE);
}

/* =======================================================

      Model Mesh Selection
      
======================================================= */

void select_model_get_drag_direction(d3pnt *pnt)
{
	float			fx,fy,fz;
	matrix_type		mat;
	
	fx=(float)pnt->x;
	fy=(float)pnt->y;
	fz=(float)pnt->z;

	if (state.ang.x!=0) {
		matrix_rotate_x(&mat,state.ang.x);
		matrix_vertex_multiply(&mat,&fx,&fy,&fz);
	}
	
	if (state.ang.y!=0) {
		matrix_rotate_y(&mat,angle_add(state.ang.y,180.0f));
		matrix_vertex_multiply(&mat,&fx,&fy,&fz);
	}
	
	if (state.ang.z!=0) {
		matrix_rotate_z(&mat,state.ang.z);
		matrix_vertex_multiply(&mat,&fx,&fy,&fz);
	}
	
	pnt->x=(int)fx;
	pnt->y=(int)fy;
	pnt->z=(int)fz;
}

void select_model_wind_mesh(d3pnt *start_pnt)
{
	int					n,sz;
	d3pnt				pnt,last_pnt,shift,old_import_move;
	d3rect				mbox;
	model_vertex_type	*vtx,*old_vtx,*old_vertexes;
	model_bone_type		*bone,*old_bone,*old_bones;
	model_mesh_type		*mesh;

		// determine if we clicked on
		// anything

	if (!select_model_wind_polygon(start_pnt,TRUE)) return;
	
		// backup vertexes and bones
		
	mesh=&model.meshes[state.cur_mesh_idx];

	sz=mesh->nvertex*sizeof(model_vertex_type);

	old_vertexes=(model_vertex_type*)malloc(sz);
	if (old_vertexes==NULL) return;

	memmove(old_vertexes,mesh->vertexes,sz);

	if (state.cur_mesh_idx==0) {
		sz=model.nbone*sizeof(model_bone_type);

		old_bones=(model_bone_type*)malloc(sz);
		if (old_bones==NULL) {
			free(old_vertexes);
			return;
		}

		memmove(old_bones,model.bones,sz);
	}

		// run the drag

	shift.x=shift.y=shift.z=0;
	last_pnt.x=last_pnt.y=-1;

	memmove(&old_import_move,&mesh->import_move,sizeof(d3pnt));

	model_wind_get_box(&mbox);
	
	while (!os_track_mouse_location(&pnt,&mbox)) {
		
		if ((last_pnt.x==pnt.x) && (last_pnt.y==pnt.y)) continue;
		memmove(&last_pnt,&pnt,sizeof(d3pnt));
			
		shift.x=-((last_pnt.x-start_pnt->x)*4);
		shift.y=((last_pnt.y-start_pnt->y)*4);
		shift.z=0;

		select_model_get_drag_direction(&shift);

			// move the vertexes

		vtx=mesh->vertexes;
		old_vtx=old_vertexes;

		for (n=0;n!=mesh->nvertex;n++) {
			vtx->pnt.x=old_vtx->pnt.x+shift.x;
			vtx->pnt.y=old_vtx->pnt.y+shift.y;
			vtx->pnt.z=old_vtx->pnt.z+shift.z;

			vtx++;
			old_vtx++;
		}

			// if this is mesh 0, then move the
			// bones

		if (state.cur_mesh_idx==0) {

			bone=model.bones;
			old_bone=old_bones;

			for (n=0;n!=model.nbone;n++) {
				bone->pnt.x=old_bone->pnt.x+shift.x;
				bone->pnt.y=old_bone->pnt.y+shift.y;
				bone->pnt.z=old_bone->pnt.z+shift.z;

				bone++;
				old_bone++;
			}
		}

			// recalc the vertexes
		
		model_calculate_parents(&model);

			// save movement for future imports

		mesh->import_move.x=old_import_move.x+shift.x;
		mesh->import_move.y=old_import_move.y+shift.y;
		mesh->import_move.z=old_import_move.z+shift.z;

			// redraw
		
		if (!state.playing) main_wind_draw();
	}
	
	if (!state.playing) main_wind_draw();

	free(old_vertexes);
	if (state.cur_mesh_idx==0) free(old_bones);
}

/* =======================================================

      Model Select Main Line
      
======================================================= */

void select_model_wind(d3pnt *start_pnt)
{
	int				sz;
	float			*pv;
	
		// no playing while selecting
		
	main_wind_play(FALSE,FALSE);
	
		// get the draw vertexes
		// need to save off array as drawing will reuse
		// array and free it
		
	model_draw_setup_initialize(&model,&draw_setup,TRUE);
		
	draw_model_setup_pose(state.cur_pose_idx);
	
	model_create_draw_bones(&model,&draw_setup);
	model_create_draw_vertexes(&model,state.cur_mesh_idx,&draw_setup);
	
	sz=(model.meshes[state.cur_mesh_idx].nvertex*3)*sizeof(float);
	pv=(float*)malloc(sz);
	if (pv==NULL) return;
	
	memmove(pv,draw_setup.mesh_arrays[state.cur_mesh_idx].gl_vertex_array,sz);
		
	model_draw_setup_shutdown(&model,&draw_setup);
	
		// setup transforms
		
	draw_model_gl_setup(0);
	draw_model_2D_transform_setup();

		// run the correct click
		
	switch (state.select_mode) {

		case select_mode_mesh:
			select_model_wind_mesh(start_pnt);
			break;

		case select_mode_polygon:
			if (!select_model_wind_vertex(start_pnt,pv)) {
				if (!select_model_wind_polygon(start_pnt,FALSE)) {
					select_model_wind_vertex_drag_sel(start_pnt,pv);
				}
			}
			break;

		case select_mode_vertex:
			if (!select_model_wind_vertex(start_pnt,pv)) {
				select_model_wind_vertex_drag_sel(start_pnt,pv);
			}
			break;
	}
	
		// free the saved vertexes

	free(pv);

		// redraw the model
		
	main_wind_draw();

	os_set_arrow_cursor();	
}

/* =======================================================

      Model Movement
      
======================================================= */

void change_model_wind(d3pnt *start_pnt,bool shift_on,bool rotate_on,bool size_on)
{
	int						old_magnify_z;
	d3rect					mbox;
	d3pnt					pnt,last_pnt,old_shift;
	d3ang					old_ang;
	
		// rotate, shift, or size the model

	last_pnt.x=last_pnt.y=-1;
	
	old_shift.x=state.shift.x;
	old_shift.y=state.shift.y;
	
	old_ang.x=state.ang.x;
	old_ang.y=state.ang.y;
	
	old_magnify_z=state.magnify_z;

	model_wind_get_box(&mbox);
	
	while (!os_track_mouse_location(&pnt,&mbox)) {
		
		if ((last_pnt.x==pnt.x) && (last_pnt.y==pnt.y)) continue;
		memmove(&last_pnt,&pnt,sizeof(d3pnt));
			
		if (shift_on) {
			state.shift.x=old_shift.x+((last_pnt.x-start_pnt->x)*4);
			state.shift.y=old_shift.y-((last_pnt.y-start_pnt->y)*4);
		}
		if (rotate_on) {
			state.ang.x=old_ang.x-(float)((last_pnt.y-start_pnt->y)/5);
			state.ang.y=old_ang.y+(float)((last_pnt.x-start_pnt->x)/5);
		}
		if (size_on) {
			state.magnify_z=old_magnify_z+((last_pnt.y-start_pnt->y)*2);
		}
		
		if (!state.playing) main_wind_draw();
	
	}
	
	if (!state.playing) main_wind_draw();
}

/* =======================================================

      Model Window Bone Clicks
      
======================================================= */

bool draw_bone_model_wind_click_box(d3pnt *click_pnt,d3fpnt *pnt)
{
	d3pnt			tran_pnt;
	
	draw_model_2D_transform(pnt,&tran_pnt);
	return((click_pnt->x>=(tran_pnt.x-6)) && (click_pnt->x<=(tran_pnt.x+6)) && (click_pnt->y>=(tran_pnt.y-6)) && (click_pnt->y<=(tran_pnt.y+6)));
}

bool drag_bone_model_wind(d3pnt *start_pnt)
{
	int						n,k,x,drag_handle;
	float					org_ang,org_mov,
							bone_drag_handle_offset;
	float					*ang,*mov;
	d3rect					mbox;
	d3pnt					pnt,last_pnt;
	d3fpnt					bone_pnt,hand_pnt;
	d3vct					vct;
	d3ang					hang,rot;
	model_draw_bone_type	*draw_bone;
	
	if (model.nbone==0) return(FALSE);
	
	main_wind_play(FALSE,FALSE);
	
		// setup the draw pose
		
	draw_model_setup_pose(state.cur_pose_idx);
	model_create_draw_bones(&model,&draw_setup);
	
		// setup transforms
		
	draw_model_gl_setup(0);
	draw_model_2D_transform_setup();
	
		// click on any drag handles?
		
	drag_handle=drag_handle_none;
	
	if ((state.cur_pose_idx==-1) || (state.cur_bone_idx!=-1)) {
	
		draw_bone=&draw_setup.bones[state.cur_bone_idx];
		
		bone_pnt.x=draw_bone->fpnt.x+draw_setup.move.x;
		bone_pnt.y=draw_bone->fpnt.y+draw_setup.move.y;
		bone_pnt.z=draw_bone->fpnt.z+draw_setup.move.z;
		
		bone_drag_handle_offset=draw_model_bones_drag_handle_offset();

		draw_model_bones_get_handle_rot(state.cur_bone_idx,&rot);
		
			// x drag bone

		vct.x=bone_drag_handle_offset;
		vct.y=0;
		vct.z=0;
		hang.x=0;
		hang.y=rot.y;
		hang.z=rot.z;
		draw_model_bones_drag_handle_calc(&bone_pnt,&vct,&hang,&hand_pnt);
		if (draw_bone_model_wind_click_box(start_pnt,&hand_pnt)) drag_handle=drag_handle_x;
		
			// y drag bone
			
		vct.x=0;
		vct.y=bone_drag_handle_offset;
		vct.z=0;
		hang.x=rot.x;
		hang.y=0;
		hang.z=rot.z;
		draw_model_bones_drag_handle_calc(&bone_pnt,&vct,&hang,&hand_pnt);
		if (draw_bone_model_wind_click_box(start_pnt,&hand_pnt)) drag_handle=drag_handle_y;
		
			// z drag bone
			
		vct.x=0;
		vct.y=0;
		vct.z=bone_drag_handle_offset;
		hang.x=rot.x;
		hang.y=rot.y;
		hang.z=0;
		draw_model_bones_drag_handle_calc(&bone_pnt,&vct,&hang,&hand_pnt);
		if (draw_bone_model_wind_click_box(start_pnt,&hand_pnt)) drag_handle=drag_handle_z;
	}
	
		// click on any bones?
		
	if (drag_handle==drag_handle_none) {
	
		k=-1;
		draw_bone=draw_setup.bones;
			
		for (n=0;n!=model.nbone;n++) {
			bone_pnt.x=draw_bone->fpnt.x+draw_setup.move.x;
			bone_pnt.y=draw_bone->fpnt.y+draw_setup.move.y;
			bone_pnt.z=draw_bone->fpnt.z+draw_setup.move.z;
			
			if (draw_bone_model_wind_click_box(start_pnt,&bone_pnt)) {
				k=n;
				break;
			}

			draw_bone++;
		}
		
		if (k==-1) return(FALSE);
	
			// select as current bone
			
		state.cur_item=item_bone;
		state.cur_bone_idx=k;
		item_palette_scroll_into_view(item_bone,state.cur_bone_idx);

		main_wind_draw();

		return(TRUE);
	}
	
		// get drag angle
		
	switch (drag_handle) {
	
		case drag_handle_x:
			ang=&model.poses[state.cur_pose_idx].bone_moves[state.cur_bone_idx].rot.x;
			mov=&model.poses[state.cur_pose_idx].bone_moves[state.cur_bone_idx].mov.x;
			break;
		
		case drag_handle_y:
			ang=&model.poses[state.cur_pose_idx].bone_moves[state.cur_bone_idx].rot.y;
			mov=&model.poses[state.cur_pose_idx].bone_moves[state.cur_bone_idx].mov.y;
			break;
			
		case drag_handle_z:
			ang=&model.poses[state.cur_pose_idx].bone_moves[state.cur_bone_idx].rot.z;
			mov=&model.poses[state.cur_pose_idx].bone_moves[state.cur_bone_idx].mov.z;
			break;
		
		default:
			return(TRUE);
			
	}
	
		// drag bone

	org_ang=*ang;
	org_mov=*mov;
	last_pnt.x=last_pnt.y=-1;
	
	undo_set_bone_move(state.cur_pose_idx,state.cur_bone_idx);
	
	os_set_drag_cursor();
	model_wind_get_box(&mbox);
		
	while (!os_track_mouse_location(&pnt,&mbox)) {
		
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
	d3rect					mbox;
	d3pnt					org_pnt,org_cnt,last_pnt,pnt;
	d3fpnt					hand_pnt;
	model_box_type			*box;
	
	main_wind_play(FALSE,FALSE);
	
		// setup the draw pose
		
	draw_model_setup_pose(state.cur_pose_idx);
	model_create_draw_bones(&model,&draw_setup);
	
		// setup transforms
		
	draw_model_gl_setup(0);

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
			hand_pnt.x=(float)x[k];
			hand_pnt.y=(float)y[k];
			hand_pnt.z=(float)z[k];
			if (draw_bone_model_wind_click_box(start_pnt,&hand_pnt)) {
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
	
	os_set_drag_cursor();
	model_wind_get_box(&mbox);
		
	while (!os_track_mouse_location(&pnt,&mbox)) {
		
		if ((last_pnt.x==pnt.x) && (last_pnt.y==pnt.y)) continue;
		memmove(&last_pnt,&pnt,sizeof(d3pnt));
		
		kx=(pnt.x-start_pnt->x)*5;
		ky=(pnt.y-start_pnt->y)*5;
		kz=0;
		
		rotate_point_center(&kx,&ky,&kz,state.ang.x,state.ang.y,0.0f);

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

void model_wind_click(d3pnt *pnt)
{
	bool			shift_on,rotate_on,size_on;
	d3rect			mbox;

		// get click within window

	model_wind_get_box(&mbox);
		
	pnt->x-=mbox.lx;
	pnt->y-=mbox.ty;
	
		// handle the clicks
		
	shift_on=os_key_space_down();
	rotate_on=os_key_command_down();
	size_on=os_key_option_down();
	
	if ((shift_on) || (rotate_on) || (size_on)) {
		change_model_wind(pnt,shift_on,rotate_on,size_on);
		return;
	}
	
	if (state.bone) {
		if (drag_bone_model_wind(pnt)) return;
	}
	
	if (state.hit_box) {
		drag_hit_box_handle_model_wind(pnt);
		return;
	}
		
	select_model_wind(pnt);
}

