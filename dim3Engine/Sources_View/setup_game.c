/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: GUI Setup

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
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
	#include "dim3engine.h"
#endif

#include "interface.h"

#define setup_pane_video					0
#define setup_pane_audio					1
#define setup_pane_mouse					2
#define setup_pane_action					3
#define setup_pane_debug					4

#define ctrl_screen_size_id					0
#define ctrl_fsaa_id						1
#define ctrl_decal_on_id					4
#define ctrl_shadow_on_id					5
#define ctrl_anisotropic_id					6
#define ctrl_mipmap_id						7
#define ctrl_texture_quality_id				8
#define ctrl_gamma_id						9

#define ctrl_sound_volume_id				30
#define ctrl_music_on_id					31
#define ctrl_music_volume_id				32

#define ctrl_always_run_id					40
#define ctrl_toggle_run_id					41
#define ctrl_invert_look_id					42
#define ctrl_mouse_speed_id					43
#define ctrl_mouse_accel_id					44
#define ctrl_mouse_smooth_id				45
#define ctrl_joystick_speed_id				46
#define ctrl_joystick_accel_id				47

#define ctrl_action_id						60

#define ctrl_debug_console_id				70
#define ctrl_debug_engine_windowed_id		71
#define ctrl_debug_editor_windowed_id		72
#define ctrl_debug_metrics_on_id			73
#define ctrl_debug_disable_shaders_id		74

#define ctrl_tab_id							80

#define setup_action_set_button				90
#define setup_action_clear_button			91

#define setup_game_ok_button				100
#define setup_game_cancel_button			101
#define setup_game_default_button			102

extern int view_search_screen_size_list(int wid,int high);
extern bool view_reset_display(char *err_str);
extern int setup_find_action_in_setup(int action_idx);
extern bool setup_xml_reset(void);

extern bool					game_loop_quit;

extern render_info_type		render_info;
extern server_type			server;
extern iface_type			iface;
extern setup_type			setup;

int							setup_tab_value,setup_action_scroll_pos,
							setup_key_control_to_action_index_list[ncontrol];

char						setup_screen_size_list[max_screen_size][32],
							setup_anisotropic_mode_list[][32]=anisotropic_mode_setup_list_def,
							setup_mipmap_mode_list[][32]=mipmap_mode_setup_list_def,
							setup_texture_quality_mode_list[][32]=texture_quality_mode_setup_list_def,
							setup_fsaa_mode_list[][32]=setup_fsaa_mode_list_def,
							setup_control_names[][32]=control_names,
							setup_action_list[ncontrol+1][128];
							
bool						setup_action_set_flag,
							setup_in_game,setup_close_save_flag;
setup_type					setup_backup;

/* =======================================================

      Setup Panes
      
======================================================= */

void setup_game_video_pane(void)
{
	int			n,idx,wid,high,
				x,y,control_y_add,control_y_sz;
	
	control_y_add=element_get_control_high();
	control_y_sz=control_y_add*8;
	
	x=(int)(((float)iface.scale_x)*0.4f);
	y=(iface.scale_y>>1)-(control_y_sz>>1);
	
		// setup screen size list
		
	for (n=0;n!=render_info.nscreen_size;n++) {
		wid=render_info.screen_sizes[n].wid;
		high=render_info.screen_sizes[n].high;
		if (gl_is_size_widescreen(wid,high)) {
			sprintf(setup_screen_size_list[n],"%dx%d Widescreen",wid,high);
		}
		else {
			sprintf(setup_screen_size_list[n],"%dx%d",wid,high);
		}
	}
	idx=view_search_screen_size_list(setup.screen_wid,setup.screen_high);
	if (idx==-1) idx=0;
	
	element_combo_add("Screen Size",(char*)setup_screen_size_list,idx,ctrl_screen_size_id,x,y,TRUE);
	y+=control_y_add;
	element_combo_add("Full-Screen Anti-Aliasing",(char*)setup_fsaa_mode_list,setup.fsaa_mode,ctrl_fsaa_id,x,y,TRUE);
	element_enable(ctrl_fsaa_id,gl_check_fsaa_ok());
	y+=control_y_add;

	element_checkbox_add("Decals",setup.decal_on,ctrl_decal_on_id,x,y,TRUE);
	y+=control_y_add;
	element_checkbox_add("Shadow",setup.shadow_on,ctrl_shadow_on_id,x,y,TRUE);
	y+=control_y_add;

	element_combo_add("Anisotropic Filtering",(char*)setup_anisotropic_mode_list,setup.anisotropic_mode,ctrl_anisotropic_id,x,y,TRUE);
	element_enable(ctrl_anisotropic_id,gl_check_texture_anisotropic_filter_ok());
	y+=control_y_add;
	element_combo_add("MipMap Filtering",(char*)setup_mipmap_mode_list,setup.mipmap_mode,ctrl_mipmap_id,x,y,TRUE);
	y+=control_y_add;
	element_combo_add("Texture Quality",(char*)setup_texture_quality_mode_list,setup.texture_quality_mode,ctrl_texture_quality_id,x,y,TRUE);
	y+=control_y_add;

	element_slider_add("Gamma",setup.gamma,-0.5f,0.5f,ctrl_gamma_id,x,y,TRUE);
}

void setup_game_audio_pane(void)
{
	int			x,y,control_y_add,control_y_sz;
	
	control_y_add=element_get_control_high();
	control_y_sz=control_y_add*2;
	
	x=(int)(((float)iface.scale_x)*0.4f);
	y=(iface.scale_y>>1)-(control_y_sz>>1);
	
	element_slider_add("Sound Volume",setup.sound_volume,0.0f,1.0f,ctrl_sound_volume_id,x,y,TRUE);
	y+=control_y_add;

	element_checkbox_add("Music",setup.music_on,ctrl_music_on_id,x,y,TRUE);
	y+=control_y_add;
	element_slider_add("Music Volume",setup.music_volume,0.0f,1.0f,ctrl_music_volume_id,x,y,TRUE);
}

void setup_game_mouse_pane(void)
{
	int			x,y,control_y_add,control_y_sz;
	
	control_y_add=element_get_control_high();
	control_y_sz=8*control_y_add;
	
	x=(int)(((float)iface.scale_x)*0.4f);
	y=(iface.scale_y>>1)-(control_y_sz>>1);
	
	element_checkbox_add("Always Run",setup.always_run,ctrl_always_run_id,x,y,TRUE);
	y+=control_y_add;
	element_checkbox_add("Toggle Run",setup.toggle_run,ctrl_toggle_run_id,x,y,TRUE);
	y+=control_y_add;
	
	element_checkbox_add("Invert Look",setup.invert_look,ctrl_invert_look_id,x,y,TRUE);
	y+=control_y_add;
	element_checkbox_add("Mouse Smoothing",setup.mouse_smooth,ctrl_mouse_smooth_id,x,y,TRUE);
	y+=control_y_add;

	element_slider_add("Mouse Speed",setup.mouse.speed,0.0f,0.1f,ctrl_mouse_speed_id,x,y,TRUE);
	y+=control_y_add;
	element_slider_add("Mouse Acceleration",setup.mouse.acceleration,0.0f,1.0f,ctrl_mouse_accel_id,x,y,TRUE);
	y+=control_y_add;

	element_slider_add("Joystick Speed",setup.joystick.speed,0.0f,0.1f,ctrl_joystick_speed_id,x,y,TRUE);
	y+=control_y_add;
	element_slider_add("Joystick Acceleration",setup.joystick.acceleration,0.0f,1.0f,ctrl_joystick_accel_id,x,y,TRUE);
}

void setup_game_action_pane(void)
{
	int					n,k,list_cnt,idx,x,y,wid,high,
						margin,padding;
	element_column_type	cols[2];
	
	margin=element_get_tab_margin();
	padding=element_get_padding();
	
	x=margin+padding;
	y=(margin+element_get_tab_control_high())+padding;

	wid=iface.scale_x-((margin+padding)*2);
	high=(int)(((float)iface.scale_y)*0.8f)-y;
	
		// setup action list
		
	list_cnt=0;
	
	for (n=0;n!=ncontrol;n++) {
	
			// is item showing
			
		if (!iface.action_display_list.action_displays[n].show) continue;
		
			// hook action index to control index
			
		setup_key_control_to_action_index_list[list_cnt]=n;
	
			// action
			
		strcpy(setup_action_list[list_cnt],iface.action_display_list.action_displays[n].display_name);
		strcat(setup_action_list[list_cnt],"\t");
		
			// attachments
			
		idx=setup_find_action_in_setup(n);
		if (idx!=-1) {
			for (k=0;k!=max_setup_action_attach;k++) {
				if (setup.action_list.actions[idx].attach[k][0]!=0x0) {
					strcat(setup_action_list[list_cnt],setup.action_list.actions[idx].attach[k]);
					strcat(setup_action_list[list_cnt],"  ");
				}
			}
		}
		
		list_cnt++;
	}
	
	setup_action_list[list_cnt][0]=0x0;

	strcpy(cols[0].name,"Action");
	cols[0].percent_size=0.50f;
	strcpy(cols[1].name,"Keys");
	cols[1].percent_size=0.50f;

	element_table_add(cols,(char*)setup_action_list,ctrl_action_id,2,x,y,wid,high,FALSE,element_table_bitmap_none);
	
		// action buttons
		
	padding=element_get_padding();

	x=iface.scale_x>>1;
	y+=high;
	
	wid=(int)(((float)iface.scale_x)*0.2f);
	high=(int)(((float)iface.scale_x)*0.04f);
	
	y+=((padding/2)+(high/2));
	
	element_button_text_add("Set Action",setup_action_set_button,(x-(padding/2)),y,wid,high,element_pos_right,element_pos_center);
	element_enable(setup_action_set_button,FALSE);
	
	element_button_text_add("Clear Action",setup_action_clear_button,(x+(padding/2)),y,wid,high,element_pos_left,element_pos_center);
	element_enable(setup_action_clear_button,FALSE);
}

void setup_game_debug_pane(void)
{
	int			x,y,control_y_add,control_y_sz;
	
	control_y_add=element_get_control_high();
	control_y_sz=control_y_add*5;
	
	x=(int)(((float)iface.scale_x)*0.5f);
	y=(iface.scale_y>>1)-(control_y_sz>>1);
	
	element_checkbox_add("Script Errors Shows Console",setup.debug_console,ctrl_debug_console_id,x,y,TRUE);
	y+=control_y_add;
	element_checkbox_add("Engine Windowed Mode (requires restart)",setup.window,ctrl_debug_engine_windowed_id,x,y,TRUE);
	y+=control_y_add;
	element_checkbox_add("Editor Windowed Run Mode",setup.window_editor,ctrl_debug_editor_windowed_id,x,y,TRUE);
	y+=control_y_add;
	element_checkbox_add("Display Metrics",setup.metrics_on,ctrl_debug_metrics_on_id,x,y,TRUE);
	y+=control_y_add;
	element_checkbox_add("Disable Shaders",setup.disable_shaders,ctrl_debug_disable_shaders_id,x,y,TRUE);
}

void setup_game_create_pane(void)
{
	int			x,y,wid,high,ntab,stab,pane;
	char		tab_list[][name_str_len]={"Video","Audio","Control","Actions","Debug"};
							
	element_clear();
	
		// tabs
		
	if (!setup_in_game) {
		stab=0;
		if (iface.debug) {
			ntab=5;
		}
		else {
			ntab=4;
		}
	}
	else {
		stab=1;
		ntab=3;
	}
	
	element_tab_add((char*)&tab_list[stab][0],setup_tab_value,ctrl_tab_id,ntab);
	
		// buttons
		
	wid=(int)(((float)iface.scale_x)*0.2f);
	high=(int)(((float)iface.scale_x)*0.04f);
	
	element_get_button_bottom_left(&x,&y,wid,high);
	element_button_text_add("Default",setup_game_default_button,x,y,wid,high,element_pos_left,element_pos_bottom);

	wid=(int)(((float)iface.scale_x)*0.1f);
	
	element_get_button_bottom_right(&x,&y,wid,high);
	element_button_text_add("OK",setup_game_ok_button,x,y,wid,high,element_pos_right,element_pos_bottom);

	x=element_get_x_position(setup_game_ok_button)-element_get_padding();
	element_button_text_add("Cancel",setup_game_cancel_button,x,y,wid,high,element_pos_right,element_pos_bottom);
	
		// specific pane controls
		
	pane=element_get_value(ctrl_tab_id);
	if (setup_in_game) pane++;
		
	switch (pane) {
		case setup_pane_video:
			setup_game_video_pane();
			break;
		case setup_pane_audio:
			setup_game_audio_pane();
			break;
		case setup_pane_mouse:
			setup_game_mouse_pane();
			break;
		case setup_pane_action:
			setup_game_action_pane();
			break;
		case setup_pane_debug:
			setup_game_debug_pane();
			break;
	}
}

/* =======================================================

      Setup Actions
      
======================================================= */

int setup_game_action_find_free_attach(setup_action_type *action)
{
	int					n;
	
	for (n=0;n!=max_setup_action_attach;n++) {
		if (action->attach[n][0]==0x0) return(n);
	}
	
	return(-1);
}
	
void setup_game_action_enable_buttons(void)
{
	int					ctrl_idx,action_idx;
	setup_action_type	*action;
	
	element_enable(setup_action_set_button,FALSE);
	element_enable(setup_action_clear_button,FALSE);
	
		// get current action
		
	ctrl_idx=element_get_value(ctrl_action_id);
	if (ctrl_idx==-1) return;
	
	action_idx=setup_find_action_in_setup(setup_key_control_to_action_index_list[ctrl_idx]);
	
		// clear enabled if selection
		
	element_enable(setup_action_clear_button,TRUE);
	
		// set enabled if empty attachment
		
	action=&setup.action_list.actions[action_idx];
	if (setup_game_action_find_free_attach(action)!=-1) element_enable(setup_action_set_button,TRUE);
}

void setup_game_action_clear(void)
{
	int					n,ctrl_idx,action_idx;
	setup_action_type	*action;
	
		// clear attachments
		
	ctrl_idx=element_get_value(ctrl_action_id);
	if (ctrl_idx==-1) return;
	
	action_idx=setup_find_action_in_setup(setup_key_control_to_action_index_list[ctrl_idx]);
	action=&setup.action_list.actions[action_idx];
	
	for (n=0;n!=max_setup_action_attach;n++) {
		action->attach[n][0]=0x0;
	}
	
		// rebuild list
		
	setup_game_create_pane();
	element_set_value(ctrl_action_id,ctrl_idx);
	setup_game_action_enable_buttons();
}

void setup_game_action_wait_for_input(void)
{
	int					n,ctrl_idx,action_idx;
	char				name[32];
	bool				already_set;
	setup_action_type	*action;
	
		// any input?
		
	if (!input_set_key_wait(name)) return;
	
		// set current action
		
	ctrl_idx=element_get_value(ctrl_action_id);
	if (ctrl_idx==-1) return;
	
	action_idx=setup_find_action_in_setup(setup_key_control_to_action_index_list[ctrl_idx]);
	
	if (action_idx==-1) {					// action doesn't exit, so create new one
		action_idx=setup.action_list.naction;
		strcpy(setup.action_list.actions[action_idx].name,setup_control_names[setup_key_control_to_action_index_list[ctrl_idx]]);
		setup.action_list.naction++;
	}
	
	action=&setup.action_list.actions[action_idx];
	
		// already picked?
		
	already_set=FALSE;
	
	for (n=0;n!=max_setup_action_attach;n++) {
		if (strcasecmp(action->attach[n],name)==0) already_set=TRUE;
	}

		// if not add
	
	if (!already_set) {
		n=setup_game_action_find_free_attach(action);
		if (n!=-1) strcpy(action->attach[n],name);
	}
	
		// don't let set key get back to interface
	
	input_clear();
	
		// rebuild list
		
	setup_game_create_pane();
	element_set_value(ctrl_action_id,ctrl_idx);
	element_set_scroll_position(ctrl_action_id,setup_action_scroll_pos);
	setup_game_action_enable_buttons();
	
		// got action
		
	setup_action_set_flag=FALSE;
}

/* =======================================================

      Setup Game Operations
      
======================================================= */

void setup_game_open(void)
{
		// setup gui
		
	gui_initialize(NULL,NULL);

		// remember if we came here
		// from within the game

	setup_in_game=(server.last_state==gs_running);
	
		// waiting for an action flag
		
	setup_action_set_flag=FALSE;
	
		// save/restore flag
		
	setup_close_save_flag=FALSE;
	
		// start with first tab
		
	setup_tab_value=0;
	setup_game_create_pane();
	
		// save setup
		
	memmove(&setup_backup,&setup,sizeof(setup_type));
}

void setup_game_close(void)
{
	char			err_str[256];
	bool			display_reset;

	gui_shutdown();
	
		// is it a restore?
		
	if (!setup_close_save_flag) {
		memmove(&setup,&setup_backup,sizeof(setup_type));
		return;
	}
	
		// save

		// fix control and sound changes
		
	setup_to_input();
	
	al_set_volume(setup.sound_volume);
	al_music_set_volume(setup.music_volume);
	al_music_set_state(setup.music_on);
	
		// need to reset SDL?
		// can't do it in game (not possible, anyway, but just in case)
		
	if (!setup_in_game) {
		display_reset=(setup_backup.screen_wid!=setup.screen_wid);
		display_reset=display_reset || (setup_backup.screen_high!=setup.screen_high);
		display_reset=display_reset || (setup_backup.fsaa_mode!=setup.fsaa_mode);
		display_reset=display_reset || (setup_backup.anisotropic_mode!=setup.anisotropic_mode);
		display_reset=display_reset || (setup_backup.mipmap_mode!=setup.mipmap_mode);
	
		if (display_reset) {
			if (!view_reset_display(err_str)) {
				game_loop_quit=TRUE;			// fatal error resetting display
				return;
			}

		}
	}
	
		// write setup
		
	setup_xml_write();
}

void setup_game_done(void)
{
		// continue game or intro

	if (setup_in_game) {
		server.next_state=gs_running;
	}
	else {
		server.next_state=gs_intro;
	}
}

void setup_game_default(void)
{
	setup_xml_reset();
	setup_game_create_pane();
}

/* =======================================================

      Setup Game Input
      
======================================================= */

void setup_game_handle_click(int id)
{
	int			idx;
	
	switch (id) {
	
			// tab
			
		case ctrl_tab_id:
			setup_tab_value=element_get_value(ctrl_tab_id);
			setup_game_create_pane();
			return;
			
			// buttons
			
		case setup_game_ok_button:
			setup_close_save_flag=TRUE;
			setup_game_done();
			return;
			
		case setup_game_cancel_button:
			setup_close_save_flag=FALSE;
			setup_game_done();
			return;

		case setup_game_default_button:
			setup_game_default();
			return;
			
		case setup_action_set_button:
			setup_action_scroll_pos=element_get_scroll_position(ctrl_action_id);
			setup_action_set_flag=TRUE;
			input_clear();
			input_set_key_start();
			return;
			
		case setup_action_clear_button:
			setup_action_scroll_pos=element_get_scroll_position(ctrl_action_id);
			setup_game_action_clear();
			element_set_scroll_position(ctrl_action_id,setup_action_scroll_pos);
			return;
	
			// video pane
			
		case ctrl_screen_size_id:
			idx=element_get_value(ctrl_screen_size_id);
			setup.screen_wid=render_info.screen_sizes[idx].wid;
			setup.screen_high=render_info.screen_sizes[idx].high;
			break;
			
		case ctrl_decal_on_id:
			setup.decal_on=element_get_value(ctrl_decal_on_id);
			break;

		case ctrl_shadow_on_id:
			setup.shadow_on=element_get_value(ctrl_shadow_on_id);
			break;

		case ctrl_anisotropic_id:
			setup.anisotropic_mode=element_get_value(ctrl_anisotropic_id);
			break;
			
		case ctrl_mipmap_id:
			setup.mipmap_mode=element_get_value(ctrl_mipmap_id);
			break;
			
		case ctrl_texture_quality_id:
			setup.texture_quality_mode=element_get_value(ctrl_texture_quality_id);
			break;

		case ctrl_fsaa_id:
			setup.fsaa_mode=element_get_value(ctrl_fsaa_id);
			break;
			
		case ctrl_gamma_id:
			setup.gamma=element_get_slider_value(ctrl_gamma_id);
			break;
			
			// audio pane
			
		case ctrl_sound_volume_id:
			setup.sound_volume=element_get_slider_value(ctrl_sound_volume_id);
			break;

		case ctrl_music_on_id:
			setup.music_on=element_get_value(ctrl_music_on_id);
			break;
			
		case ctrl_music_volume_id:
			setup.music_volume=element_get_slider_value(ctrl_music_volume_id);
			break;
			
			// mouse pane
			
		case ctrl_always_run_id:
			setup.always_run=element_get_value(ctrl_always_run_id);
			break;
			
		case ctrl_toggle_run_id:
			setup.toggle_run=element_get_value(ctrl_toggle_run_id);
			break;
			
		case ctrl_invert_look_id:
			setup.invert_look=element_get_value(ctrl_invert_look_id);
			break;
			
		case ctrl_mouse_smooth_id:
			setup.mouse_smooth=element_get_value(ctrl_mouse_smooth_id);
			break;
			
		case ctrl_mouse_speed_id:
			setup.mouse.speed=element_get_slider_value(ctrl_mouse_speed_id);
			break;
			
		case ctrl_mouse_accel_id:
			setup.mouse.acceleration=element_get_slider_value(ctrl_mouse_accel_id);
			break;
			
		case ctrl_joystick_speed_id:
			setup.joystick.speed=element_get_slider_value(ctrl_joystick_speed_id);
			break;

		case ctrl_joystick_accel_id:
			setup.joystick.acceleration=element_get_slider_value(ctrl_joystick_accel_id);
			break;
		
			// action pane
			
		case ctrl_action_id:
			setup_game_action_enable_buttons();
			break;

			// debug pane

		case ctrl_debug_console_id:
			setup.debug_console=element_get_value(ctrl_debug_console_id);
			break;

		case ctrl_debug_engine_windowed_id:
			setup.window=element_get_value(ctrl_debug_engine_windowed_id);
			break;

		case ctrl_debug_editor_windowed_id:
			setup.window_editor=element_get_value(ctrl_debug_editor_windowed_id);
			break;

		case ctrl_debug_metrics_on_id:
			setup.metrics_on=element_get_value(ctrl_debug_metrics_on_id);
			break;

		case ctrl_debug_disable_shaders_id:
			setup.disable_shaders=element_get_value(ctrl_debug_disable_shaders_id);
			break;
			
	}
}

void setup_game_keyboard(void)
{
	int			id;

	id=gui_keyboard();
	if (id==-1) return;

	setup_game_handle_click(id);
}

void setup_game_click(void)
{
	int			id;
	
	id=gui_click();
	if (id==-1) return;
	
	hud_click();

	setup_game_handle_click(id);
}
	
/* =======================================================

      Run Game Setup
      
======================================================= */

void setup_game_run(void)
{
		// if input wait is on
		
	if (setup_action_set_flag) {
		gui_draw_message("Press a Key or Click a Button");
		setup_game_action_wait_for_input();
		return;
	}
	
		// draw UI
		
	gui_draw(1.0f,TRUE);

		// keyboard and clicking

	setup_game_keyboard();
	setup_game_click();
}

