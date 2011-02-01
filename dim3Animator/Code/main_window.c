/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Main Window

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
	#include "dim3animator.h"
#endif

#include "glue.h"
#include "interface.h"
#include "ui_common.h"

model_type						model;
model_draw_setup				draw_setup;
file_path_setup_type			file_path_setup;
animator_state_type				state;

extern d3rect					tool_palette_box,txt_palette_box;
extern list_palette_type		item_palette,property_palette,alt_property_palette;

/* =======================================================

      Initialize and Shutdown
	        
======================================================= */

void main_wind_initialize(void)
{
		// initializations
		
	text_initialize();
	
	tool_palette_initialize("Animator");
	list_palette_initialize("Animator");
	item_palette_initialize();
	property_palette_initialize();
	alt_property_palette_initialize();

		// size setups
		
	tool_palette_setup();
	texture_palette_setup();
	
	item_palette_setup();
	property_palette_setup();
	alt_property_palette_setup();

		// vertex and trig masks
		
	vertex_mask_initialize();
	trig_mask_initialize();
}

void main_wind_shutdown(void)
{
		// free vertex and trig masks
		
	vertex_mask_shutdown();
	trig_mask_shutdown();
	
		// shutdown palettes
		
	alt_property_palette_shutdown();
	property_palette_shutdown();
	item_palette_shutdown();
	list_palette_shutdown();
	
	tool_palette_shutdown();
	
	text_shutdown();
}

/* =======================================================

      GL Setup
      
======================================================= */

void main_wind_gl_setup(void)
{
	glClearColor(0.9f,0.9f,0.9f,0.0f);

	glEnable(GL_SMOOTH);
	glDisable(GL_DITHER);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_ALPHA_TEST);
	
	glEnableClientState(GL_VERTEX_ARRAY);
}

/* =======================================================

      Main Window Draw
      
======================================================= */

void main_wind_draw_play_calc_animation(int cur_tick,int animate_idx,int blend_idx,bool non_blend_setup)
{
	int						first_pose,last_pose,tick,nxt_tick,
							pose_move_1_idx,pose_move_2_idx,msec;
	float					pose_factor;
	model_animate_type		*animate;
	model_pose_move_type	*pose_move_1,*pose_move_2;

		// calc animation
		
	animate=&model.animates[animate_idx];
	
	first_pose=0;
	if (animate->loop_start!=-1) first_pose=animate->loop_start;

	last_pose=animate->npose_move-1;
	if (animate->loop_end!=-1) last_pose=animate->loop_end;
	
	tick=state.blend[blend_idx].tick;
	pose_move_1_idx=state.blend[blend_idx].pose_move_idx;
	
	msec=animate->pose_moves[pose_move_1_idx].msec;
	nxt_tick=tick+msec;
	
	while (nxt_tick<cur_tick) {				// skip ahead till we find correct pose
		pose_move_1_idx++;
		if (pose_move_1_idx>last_pose) pose_move_1_idx=first_pose;
		
		msec=animate->pose_moves[pose_move_1_idx].msec;

		tick=nxt_tick;
		nxt_tick=tick+msec;
	}
	
	state.blend[blend_idx].tick=tick;
	state.blend[blend_idx].pose_move_idx=pose_move_1_idx;
	
	pose_move_2_idx=pose_move_1_idx+1;
	if (pose_move_2_idx>last_pose) pose_move_2_idx=first_pose;
	
		// get the pose moves
		
	pose_move_1=&animate->pose_moves[pose_move_1_idx];
	pose_move_2=&animate->pose_moves[pose_move_2_idx];
	
		// setup the poses
	
	draw_setup.poses[blend_idx].idx_1=pose_move_1->pose_idx;
	draw_setup.poses[blend_idx].idx_2=pose_move_2->pose_idx;
	
	pose_factor=(float)(nxt_tick-cur_tick)/(float)msec;
	
    draw_setup.poses[blend_idx].factor=pose_factor;
	draw_setup.poses[blend_idx].acceleration=pose_move_1->acceleration;
	
		// non-blended setup
		
	if (non_blend_setup) {
		draw_setup.ang.x=0;
		draw_setup.ang.y=0;
		draw_setup.ang.z=0;
		
		draw_setup.sway.x=pose_move_2->sway.x+((pose_move_1->sway.x-pose_move_2->sway.x)*pose_factor);
		draw_setup.sway.y=pose_move_2->sway.y+((pose_move_1->sway.y-pose_move_2->sway.y)*pose_factor);
		draw_setup.sway.z=pose_move_2->sway.z+((pose_move_1->sway.z-pose_move_2->sway.z)*pose_factor);
		draw_setup.move.x=pose_move_2->mov.x+((pose_move_1->mov.x-pose_move_2->mov.x)*pose_factor);
		draw_setup.move.y=pose_move_2->mov.y+((pose_move_1->mov.y-pose_move_2->mov.y)*pose_factor);
		draw_setup.move.z=pose_move_2->mov.z+((pose_move_1->mov.z-pose_move_2->mov.z)*pose_factor);
	}
}

void main_wind_draw_play(void)
{
	int					n,cur_tick;
	
		// if no current animation, just do no pose for animated textures
		
	if ((state.cur_animate_idx==-1) && (!state.play_animate_blend)) {
		main_wind_draw();
		return;
	}
	
		// clear the draw setup
		
	model_draw_setup_clear(&model,&draw_setup);
	
		// calc the pose
		
	cur_tick=time_get();
	
	if (!state.play_animate_blend) {
		main_wind_draw_play_calc_animation(cur_tick,state.cur_animate_idx,0,TRUE);
	}
	else {
		for (n=0;n!=max_model_blend_animation;n++) {
			if (state.blend[n].animate_idx!=-1) {
				main_wind_draw_play_calc_animation(cur_tick,state.blend[n].animate_idx,n,(n==0));
			}
		}
	}
		
		// global draw setup
	
	draw_model_wind(state.cur_mesh_idx);
}

void main_wind_draw(void)
{
		// clear gl buffer
		
	glDisable(GL_SCISSOR_TEST);
	
	glClearColor(0.75f,0.75f,0.75f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	
		// model
		
	if (state.model_open) {
		if (state.texture_edit_idx==-1) {
			if (!state.playing) {
				draw_model_wind_pose(state.cur_mesh_idx,state.cur_pose_idx);
			}
			else {
				main_wind_draw_play();
			}
		}
		else {
			texture_edit_draw();
		}
	}

		// palettes
		
	if (state.model_open) {
		item_palette_draw();
		property_palette_draw();
		alt_property_palette_draw();

		tool_palette_draw();
		texture_palette_draw(model.textures);
	}
	
		// swap GL buffer
		
	os_swap_gl_buffer();
}

void main_wind_draw_no_swap(void)
{
		// clear gl buffer
		
	glDisable(GL_SCISSOR_TEST);
	
	glClearColor(1.0f,1.0f,1.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	
		// model
		
	if (state.model_open) {
		if (state.texture_edit_idx==-1) {
			if (!state.playing) {
				draw_model_wind_pose(state.cur_mesh_idx,state.cur_pose_idx);
			}
			else {
				main_wind_draw_play();
			}
		}
		else {
			texture_edit_draw();
		}
	}

		// palettes
		
	if (state.model_open) {
		item_palette_draw();
		property_palette_draw();
		alt_property_palette_draw();
		
		tool_palette_draw();
		texture_palette_draw(model.textures);
	}
}

/* =======================================================

      Model Playing
      
======================================================= */

void main_wind_play(bool play,bool blend)
{
	int					n,tick;
	
		// good animation?
		
	if (play) {
		if (state.cur_animate_idx==-1) return;
		if (model.animates[state.cur_animate_idx].npose_move==0) return;
	}
	
		// always turn off animation until setup is complete
		// as animation is on a timer
		
	state.playing=FALSE;
	
		// setup animation
		
	state.play_animate_blend=blend;
	
	time_start();
	tick=time_get();

	for (n=0;n!=max_model_blend_animation;n++) {
		state.blend[n].pose_move_idx=0;
		state.blend[n].tick=tick;
	}
	
		// turn on/off animation
		
	state.playing=play;
	
	if (state.model_open) main_wind_draw();
}

/* =======================================================

      Clicking
      
======================================================= */

void main_wind_click(d3pnt *pnt,bool double_click)
{
	bool			old_playing;

		// tool palette

	if (pnt->y<tool_palette_box.by) {
		tool_palette_click(pnt);
		return;
	}

		// texture palette

	if (pnt->y>=txt_palette_box.ty) {
		texture_palette_click(model.textures,pnt,double_click);
		return;
	}

		// item, property and alt property palettes

	if ((pnt->x>=item_palette.box.lx) && (pnt->x<=item_palette.box.rx) && (pnt->y>=item_palette.box.ty) && (pnt->y<item_palette.box.by)) {
		item_palette_click(pnt,double_click);
		return;
	}

	if ((pnt->x>=property_palette.box.lx) && (pnt->x<=property_palette.box.rx) && (pnt->y>=property_palette.box.ty) && (pnt->y<property_palette.box.by)) {
		property_palette_click(pnt,double_click);
		return;
	}

	if ((pnt->x>=alt_property_palette.box.lx) && (pnt->x<=alt_property_palette.box.rx) && (pnt->y>=alt_property_palette.box.ty) && (pnt->y<alt_property_palette.box.by)) {
		alt_property_palette_click(pnt,double_click);
		return;
	}

		// model clicks
		// turn off animation as it glitches
		// up the win32 timers

	old_playing=state.playing;
	state.playing=FALSE;

	if (state.texture_edit_idx==-1) {
		model_wind_click(pnt);
	}
	else {
		texture_edit_click(pnt,double_click);
	}

	state.playing=old_playing;
}

/* =======================================================

      Scroll Wheel
      
======================================================= */

void main_wind_scroll_wheel(d3pnt *pnt,int delta)
{
		// scroll wheel in item, property, or alt property palette

	if ((pnt->x>=item_palette.box.lx) && (pnt->x<=item_palette.box.rx) && (pnt->y>=item_palette.box.ty) && (pnt->y<item_palette.box.by)) {
		item_palette_scroll_wheel(pnt,delta);
		return;
	}

	if ((pnt->x>=property_palette.box.lx) && (pnt->x<=property_palette.box.rx) && (pnt->y>=property_palette.box.ty) && (pnt->y<property_palette.box.by)) {
		property_palette_scroll_wheel(pnt,delta);
		return;
	}

	if ((pnt->x>=alt_property_palette.box.lx) && (pnt->x<=alt_property_palette.box.rx) && (pnt->y>=alt_property_palette.box.ty) && (pnt->y<alt_property_palette.box.by)) {
		alt_property_palette_scroll_wheel(pnt,delta);
		return;
	}

		// scroll wheel in model

	if (state.texture_edit_idx==-1) {
		state.magnify_z+=(delta*20);
		main_wind_draw();
	}
	else {
		texture_edit_scroll_wheel(delta);
	}
}

/* =======================================================

      Cursors
      
======================================================= */

bool main_wind_cursor(void)
{
	if (!state.model_open) return(FALSE);

		// texture editing

	if (state.texture_edit_idx==-1) return(texture_edit_cursor());

		// model cursors

	if (os_key_space_down()) {
		os_set_hand_cursor();
		return(TRUE);
	}
	if (os_key_command_down()) {
		os_set_hand_cursor();
		return(TRUE);
	}
	if (os_key_option_down()) {
		os_set_resize_cursor();
		return(TRUE);
	}
	if (os_key_shift_down()) {
		os_set_add_cursor();
		return(TRUE);
	}
	if (os_key_control_down()) {
		os_set_subtract_cursor();
		return(TRUE);
	}
	
	os_set_arrow_cursor();
	return(TRUE);
}

/* =======================================================

      Keys
      
======================================================= */

void main_wind_key(char ch)
{
	if (!state.model_open) return;
	
		// esc key deselects
		
	if (ch==0x1B) {
		vertex_clear_sel_mask(state.cur_mesh_idx);
		main_wind_draw();
		return;
	}

		// check for deletes
		// on selected item tree

	if ((ch==D3_KEY_BACKSPACE) || (ch==D3_KEY_DELETE)) {
		if (item_palette_delete()) {
			main_wind_draw();
			return;
		}
	}
}

/* =======================================================

      Resize Setup
      
======================================================= */

void main_wind_resize(void)
{
	if (!state.model_open) return;

	tool_palette_setup();
	texture_palette_setup();
	item_palette_setup();
	property_palette_setup();
	alt_property_palette_setup();
}

