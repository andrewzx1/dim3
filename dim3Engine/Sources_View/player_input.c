/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Player Routines

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
	#include "dim3engine.h"
#endif

#include "network.h"
#include "scripts.h"
#include "objects.h"
#include "remotes.h"
#include "weapons.h"
#include "cameras.h"
#include "inputs.h"

extern map_type				map;
extern view_type			view;
extern server_type			server;
extern camera_type			camera;
extern hud_type				hud;
extern js_type				js;
extern setup_type			setup;
extern network_setup_type	net_setup;

int							mouse_last_read_tick;
bool						weapon_change_key_down,weapon_target_key_down,weapon_zoom_key_down,
							enter_exit_key_down,network_score_key_down,toggle_run_state,
							fire_key_down[4],command_key_down[20],player_key_down[20];

extern void chat_add_message(char *name,char *str,d3col *col);

/* =======================================================

      Clear Input
      
======================================================= */

void player_clear_input(void)
{
	int			i;
	
	mouse_last_read_tick=-1;

	weapon_change_key_down=FALSE;
	weapon_target_key_down=FALSE;
	weapon_zoom_key_down=FALSE;
	enter_exit_key_down=FALSE;
	network_score_key_down=FALSE;
	toggle_run_state=FALSE;
	
	for (i=0;i!=4;i++) {
		fire_key_down[i]=FALSE;
	}

    for (i=0;i!=20;i++) {
        command_key_down[i]=FALSE;
        player_key_down[i]=FALSE;
    }
	
	input_clear();

	hud.chat.type_on=FALSE;
}

/* =======================================================

      Player Command Keys
      
======================================================= */

void player_command_input(obj_type *obj)
{
	int			i,k;

        // game keys
        
	for (i=nc_game_start;i<=nc_game_end;i++) {
    
        k=i-nc_game_start;
        
        if (input_action_get_state(i)) {
            if (!command_key_down[k]) {
                command_key_down[k]=TRUE;
				scripts_post_event_console(&js.game_attach,sd_event_message,sd_event_message_from_key_down,k);
            }
        }
        else {
            if (command_key_down[k]) {
                command_key_down[k]=FALSE;
				scripts_post_event_console(&js.game_attach,sd_event_message,sd_event_message_from_key_up,k);
            }
        }
    }
    
        // player keys
        
	for (i=nc_player_start;i<=nc_player_end;i++) {
    
        k=i-nc_player_start;
        
        if (input_action_get_state(i)) {
            if (!player_key_down[k]) {
                player_key_down[k]=TRUE;
				scripts_post_event_console(&obj->attach,sd_event_message,sd_event_message_from_key_down,k);
            }
        }
        else {
            if (player_key_down[k]) {
                player_key_down[k]=FALSE;
				scripts_post_event_console(&obj->attach,sd_event_message,sd_event_message_from_key_up,k);
            }
        }
    }
}

/* =======================================================

      Y Movements 
      
======================================================= */

void player_movement_fpp_y_input(obj_type *obj)
{
    if (input_action_get_state(nc_down)) {
        obj->vert_move.moving=TRUE;
        if (obj->vert_move.reverse) obj->vert_move.speed=0;
        obj->vert_move.reverse=FALSE;
		return;
    }

	if (input_action_get_state(nc_up)) {
		obj->vert_move.moving=TRUE;
        if (!obj->vert_move.reverse) obj->vert_move.speed=0;
        obj->vert_move.reverse=TRUE;
		return;
    }
}

void player_movement_fly_swim_y_input(obj_type *obj)
{
	if (!obj->forward_move.moving) return;

    if (obj->view_ang.x<-obj->vert_move.slop) {
        obj->vert_move.moving=TRUE;
        if (obj->vert_move.reverse==obj->forward_move.reverse) obj->vert_move.speed=0;
        obj->vert_move.reverse=!obj->forward_move.reverse;
		return;
    }
    
	if (obj->view_ang.x>obj->vert_move.slop) {
		obj->vert_move.moving=TRUE;
        if (!obj->vert_move.reverse==obj->forward_move.reverse) obj->vert_move.speed=0;
        obj->vert_move.reverse=obj->forward_move.reverse;
		return;
    }
}

void player_movement_ladder_y_input(obj_type *obj)
{
	if ((!obj->climb.on) || (!obj->forward_move.moving)) return;
			
    if (obj->view_ang.x<-obj->vert_move.slop) {
        obj->vert_move.moving=TRUE;
        if (obj->vert_move.reverse==obj->forward_move.reverse) obj->vert_move.speed=0;
        obj->vert_move.reverse=!obj->forward_move.reverse;
		return;
    }
    
	if (obj->view_ang.x>obj->vert_move.slop) {
		obj->vert_move.moving=TRUE;
        if (!obj->vert_move.reverse==obj->forward_move.reverse) obj->vert_move.speed=0;
        obj->vert_move.reverse=obj->forward_move.reverse;
		return;
    }
}

/* =======================================================

      Player Weapon Input
      
======================================================= */

void player_weapon_target_input(obj_type *obj,weapon_type *weap)
{
	bool				target_next,target_previous;
 	
		// targetting on?
		
	if (!weap->target.on) return;
	
		// any zoom keys down?
		
	target_next=input_action_get_state(nc_weapon_target_next);
	target_previous=input_action_get_state(nc_weapon_target_previous);
	
	if ((!target_next) && (!target_previous)) {
		weapon_target_key_down=FALSE;
		return;
	}
	
		// key held down, ignore
		
	if (weapon_target_key_down) return;
	
		// change targets

	if (target_next) {
		weapon_target_next_object(obj,weap);
		weapon_target_key_down=TRUE;
		return;
	}

	if (target_previous) {
		weapon_target_previous_object(obj,weap);
		weapon_target_key_down=TRUE;
		return;
	}
}

void player_weapon_zoom_input(obj_type *obj,weapon_type *weap)
{
	bool				zoom_key,zoom_increase_key,zoom_decrease_key;
 	
		// zooms allowed?
		
	if (!weap->zoom.on) return;
	
		// any zoom keys down?
		
	zoom_key=input_action_get_state(nc_weapon_zoom);
	zoom_increase_key=input_action_get_state(nc_weapon_zoom_increase);
	zoom_decrease_key=input_action_get_state(nc_weapon_zoom_decrease);
	
	if ((!zoom_key) && (!zoom_increase_key) && (!zoom_decrease_key)) {
		weapon_zoom_key_down=FALSE;
		return;
	}
	
		// key held down, ignore
		
	if (weapon_zoom_key_down) return;
	
		// check zoom activate/deactive key
		
	if (zoom_key) {
		weapon_zoom_key(obj,weap);
		weapon_zoom_key_down=TRUE;
		return;
	}
		
		// if zoom active, check increase/decrease keys
		
	if (weap->zoom.mode!=zoom_mode_on) return;
	
	if ((zoom_increase_key) && (weap->zoom.current_step<(weap->zoom.step_count-1))) {
		weap->zoom.current_step++;
		weapon_zoom_key_down=TRUE;
		return;
	}

	if ((zoom_decrease_key) && (weap->zoom.current_step>0)) {
		weap->zoom.current_step--;
		weapon_zoom_key_down=TRUE;
		return;
	}
}

void player_weapon_change_input(obj_type *obj,weapon_type *weap)
{
	int				i,k;
	bool			down_key,weapon_key[(nc_weapon_end-nc_weapon_start)+1],
					next_weapon_key,previous_weapon_key;
	
		// special check if weapon change key is same as the
		// zoom or targetting keys.  If either are on, don't
		// change the weapon
		
	next_weapon_key=input_action_get_state(nc_next_weapon);
	previous_weapon_key=input_action_get_state(nc_previous_weapon);
		
	if ((weap->zoom.on) && (weap->zoom.mode!=zoom_mode_off)) {
		if (input_check_action_same_attachment(nc_next_weapon,nc_weapon_zoom_increase)) next_weapon_key=FALSE;
		if (input_check_action_same_attachment(nc_next_weapon,nc_weapon_zoom_decrease)) next_weapon_key=FALSE;
		if (input_check_action_same_attachment(nc_previous_weapon,nc_weapon_zoom_increase)) previous_weapon_key=FALSE;
		if (input_check_action_same_attachment(nc_previous_weapon,nc_weapon_zoom_decrease)) previous_weapon_key=FALSE;
	}
	if (weap->target.on) {
		if (input_check_action_same_attachment(nc_next_weapon,nc_weapon_target_next)) next_weapon_key=FALSE;
		if (input_check_action_same_attachment(nc_next_weapon,nc_weapon_target_previous)) next_weapon_key=FALSE;
		if (input_check_action_same_attachment(nc_previous_weapon,nc_weapon_target_next)) previous_weapon_key=FALSE;
		if (input_check_action_same_attachment(nc_previous_weapon,nc_weapon_target_previous)) previous_weapon_key=FALSE;
	}
	
		// any weapon change keys down?
		
	down_key=next_weapon_key||previous_weapon_key;

	for (i=nc_weapon_start;i<=nc_weapon_end;i++) {
		k=i-nc_weapon_start;
		weapon_key[k]=input_action_get_state(i);
		down_key=down_key||weapon_key[k];
	}
	
	if (!down_key) {
		weapon_change_key_down=FALSE;
		return;
	}
	
		// key held down, ignore
		
	if (weapon_change_key_down) return;
	
		// change weapons

	if (weap->zoom.mode!=zoom_mode_off) weapon_zoom_off(obj,weap);
	if (weap->target.on) weapon_target_end(obj,weap);

	if (next_weapon_key) {
		weapon_switch(obj,1);
		weapon_change_key_down=TRUE;
		return;
	}

	if (previous_weapon_key) {
		weapon_switch(obj,-1);
		weapon_change_key_down=TRUE;
		return;
	}
	
	for ((i=nc_weapon_start);(i<=nc_weapon_end);i++) {
		k=i-nc_weapon_start;
		if (weapon_key[k]) {
			weapon_pick(obj,k);
			weapon_change_key_down=TRUE;
			return;
		}
	}
}

void player_weapon_fire_input(obj_type *obj,weapon_type *weap)
{
	int					i,method;
	
		// no firing if no weapon is being held
		
	if (obj->held_weapon.mode!=wm_held) return;
		
		// check keys

	for (i=nc_fire_start;i<=nc_fire_end;i++) {
	
		method=i-nc_fire_start;
		
		if (input_action_get_state(i)) {
			if (!fire_key_down[method]) {
				fire_key_down[method]=TRUE;
				weapon_player_fire_down(obj,weap,method);
			}
			else {
				weapon_player_fire_repeat(obj,weap);
			}
		}
		else {
			if (fire_key_down[method]) {
				fire_key_down[method]=FALSE;
				weapon_player_fire_up(obj,weap,method);
			}
		}
	}
}

void player_weapon_input(obj_type *obj)
{
	weapon_type			*weap;
	
		// get held weapon
		
	weap=weapon_find_uid(obj->held_weapon.current_uid);
	if (weap==NULL) return;

		// weapon keys
		
	if ((obj->held_weapon.mode==wm_held) && (weap!=NULL)) {
		player_weapon_target_input(obj,weap);
		player_weapon_zoom_input(obj,weap);
		player_weapon_fire_input(obj,weap);
	}

	player_weapon_change_input(obj,weap);
}

/* =======================================================

      Player Enter-Exit
      
======================================================= */

void player_enter_exit_input(obj_type *obj)
{
	if (!input_action_get_state(nc_enter_exit)) {
		enter_exit_key_down=FALSE;
		return;
	}
	
	if (enter_exit_key_down) return;			// only check on key downs
	
	enter_exit_key_down=TRUE;
	
		// exit a vehicle?
		
	if (obj->vehicle.attach_obj_uid!=-1) {
		object_exit_vehicle(obj,FALSE,NULL);
		return;
	}
		
		// enter a vehicle?
	
	object_enter_vehicle(obj,NULL);
}

/* =======================================================

      Player Messages
      
======================================================= */

bool player_message_input(obj_type *obj)
{
	int				len;
	char			ch,nstr[chat_str_len];
	char			*c,*msg;
	d3col			col;
	
	if (net_setup.mode==net_mode_none) return(TRUE);
	
		// scores
		
	if (input_action_get_state(nc_score)) {
		if (!network_score_key_down) {
			hud.score.on=!hud.score.on;
			network_score_key_down=TRUE;
		}
	}
	else {
		network_score_key_down=FALSE;
	}
		
		// chatting

	if (hud.chat.type_on) {
	
			// get last raw key as character
			
		ch=input_get_last_raw_key();
		input_clear_last_raw_key();
		
		if (ch==0x0) return(FALSE);
		
			// get current message len
			
		msg=hud.chat.type_str;
		len=strlen(msg);
		
			// escape cancels
			
		if (ch==0x1B) {
			hud.chat.type_on=FALSE;
			return(FALSE);
		}
		
			// return ends entry
			
		if (ch==0xD) {
			hud.chat.type_on=FALSE;
			if (len>1) {
				msg[len-1]=0x0;
				net_client_send_chat(obj,msg);
				object_get_tint(obj,&col);
				chat_add_message(obj->name,msg,&col);
			}
			return(FALSE);
		}
		
			// delete
			
		if (ch==0x8) {
			len-=2;
			if (len<0) len=0;

			msg[len++]='_';
			msg[len]=0x0;

			return(FALSE);
		}
		
			// eliminate certain characters
			
		if (((ch<'!') || (ch>'z')) && (ch!=0x20)) return(FALSE);
		
			// force start a new line if too many characters
			
		if (len>=(chat_str_len-1)) {
			nstr[0]=0x0;
			
			c=strrchr(msg,' ');
			if (c!=NULL) {
				if ((int)(c-msg)>5) {
					strcpy(nstr,(c+1));
					*c=0x0;
				}
			}
			
			msg[len-1]=0x0;
			net_client_send_chat(obj,msg);
			object_get_tint(obj,&col);
			chat_add_message(obj->name,msg,&col);
			
			strcpy(msg,nstr);
			len=strlen(msg);
		}
		
			// add new character
			
		if (len>0) len--;
		
		msg[len++]=ch;
		msg[len++]='_';
		msg[len]=0x0;
	
		return(FALSE);
	}
	
	if (input_action_get_state(nc_message)) {
		hud.chat.type_on=TRUE;
		
		input_clear_all_last_raw_key();
		
		hud.chat.type_str[0]='_';
		hud.chat.type_str[1]=0x0;
		
		return(FALSE);
	}
	
	return(TRUE);
}

/* =======================================================

      Input Utilities
      
======================================================= */

float player_mouse_smooth(float mouse_ang,float turn_ang)
{
	if (!setup.mouse_smooth) return(mouse_ang);
	
	if (mouse_ang>0) {
		if ((mouse_ang<turn_ang) && (turn_ang>=0)) {
			return(turn_ang/2.0f);
		}
		
		return(mouse_ang);
	}

	if (mouse_ang<0) {
		if ((mouse_ang>turn_ang) && (turn_ang<=0)) {
			return(turn_ang/2.0f);
		}
		
		return(mouse_ang);
	}
	
	return(mouse_ang);
}

void player_get_6_way_input(obj_type *obj,float *mouse_x,float *mouse_y,bool *go_forward,bool *go_backward,bool *go_side_left,bool *go_side_right)
{
		// get input

	if (!obj->turn.ignore_mouse) {
		input_get_mouse_movement(mouse_x,mouse_y);
	}
	else {
		*mouse_x=*mouse_y=0.0f;
	}

	*go_forward=input_action_get_state(nc_move_forward);
	*go_backward=input_action_get_state(nc_move_backward);
	*go_side_left=input_action_get_state(nc_sidestep_left);
	*go_side_right=input_action_get_state(nc_sidestep_right);

	if (input_check_joystick_ok()) {
		(*mouse_x)-=input_get_joystick_axis(2);
		(*mouse_y)+=input_get_joystick_axis(3);

		(*go_forward)|=input_get_joystick_axis_as_button_min(1);
		(*go_backward)|=input_get_joystick_axis_as_button_max(1);
		(*go_side_left)|=input_get_joystick_axis_as_button_min(0);
		(*go_side_right)|=input_get_joystick_axis_as_button_max(0);
	}

	if (input_action_get_state(nc_turn_left)) (*mouse_x)-=obj->turn.key_speed;
	if (input_action_get_state(nc_turn_right)) (*mouse_x)+=obj->turn.key_speed;

	if (*go_forward) *go_backward=FALSE;
	if (*go_side_left) *go_side_right=FALSE;

		// quick reverse locks

	if (!obj->quick_reverse) {
		if (*go_forward) {
			if ((obj->forward_move.reverse) && (obj->forward_move.speed!=0)) *go_forward=FALSE;
		}

		if (*go_backward) {
			if ((!obj->forward_move.reverse) && (obj->forward_move.speed!=0)) *go_backward=FALSE;
		}
	}
}

void player_get_4_way_input(bool *go_left,bool *go_right,bool *go_up,bool *go_down)
{
	*go_left=input_action_get_state(nc_turn_left);
	*go_right=input_action_get_state(nc_turn_right);
	*go_up=input_action_get_state(nc_up);
	*go_down=input_action_get_state(nc_down);

	if (input_check_joystick_ok()) {
		(*go_left)|=input_get_joystick_axis_as_button_min(0);
		(*go_right)|=input_get_joystick_axis_as_button_max(0);
		(*go_up)|=input_get_joystick_axis_as_button_min(1);
		(*go_down)|=input_get_joystick_axis_as_button_max(1);
	}

	if (*go_right) *go_left=FALSE;
	if (*go_down) *go_up=FALSE;
}

/* =======================================================

      Input Run/Walk State
      
======================================================= */

void player_set_run_walk_state(obj_type *obj)
{
	if (obj->single_speed) {
		obj->forward_move.running=FALSE;
		return;
	}

	if (!setup.toggle_run) {
		toggle_run_state=input_action_get_state(nc_run);
	}
	else {
		if (input_action_get_state_single(nc_run)) toggle_run_state=!toggle_run_state;
	}
	
	if (setup.always_run) {
		obj->forward_move.running=!toggle_run_state;
	}
	else {
		obj->forward_move.running=toggle_run_state;
	}
}

/* =======================================================

      FPP Input Type
      
======================================================= */

void player_fpp_input(obj_type *obj)
{
	float			mouse_x,mouse_y;
	bool			go_forward,go_backward,go_side_left,go_side_right;

		// start with no movement
		
    obj->forward_move.moving=FALSE;
	obj->side_move.moving=FALSE;
	obj->vert_move.moving=FALSE;

		// get input

	player_get_6_way_input(obj,&mouse_x,&mouse_y,&go_forward,&go_backward,&go_side_left,&go_side_right);

		// turning and looking

	obj->turn.ang_add.y=player_mouse_smooth(mouse_x,obj->turn.ang_add.y);
	obj->look.ang_add=player_mouse_smooth(mouse_y,obj->look.ang_add);
	
		// don't move if ducked and crawling disabled
		
	if ((!obj->crawl) && (obj->duck.mode!=dm_stand)) return;

		// forward/backwards movement

	if (go_forward) {
		obj->forward_move.moving=TRUE;
		if (obj->forward_move.reverse) obj->forward_move.speed=0;
		obj->forward_move.reverse=FALSE;
	}

	if (go_backward) {
		obj->forward_move.moving=TRUE;
		if (!obj->forward_move.reverse) obj->forward_move.speed=0;
		obj->forward_move.reverse=TRUE;
	}

		// run and walk state

	player_set_run_walk_state(obj);

		// side movement
	
	if (go_side_left) {
		obj->side_move.moving=TRUE;
		if (obj->side_move.reverse) obj->side_move.speed=0;
		obj->side_move.reverse=FALSE;
	}

	if (go_side_right) {
		obj->side_move.moving=TRUE;
		if (!obj->side_move.reverse) obj->side_move.speed=0;
		obj->side_move.reverse=TRUE;
	}

		// y movement

	if ((obj->fly) || (obj->liquid.mode==lm_under) || (obj->liquid.mode==lm_float)) {
		player_movement_fly_swim_y_input(obj);
	}
	else {
		player_movement_fpp_y_input(obj);
		player_movement_ladder_y_input(obj);
	}
}

/* =======================================================

      Side Scroll Input Type
      
======================================================= */

void player_side_scroll_input(obj_type *obj)
{
	bool			go_left,go_right,go_up,go_down;

		// get input

	player_get_4_way_input(&go_left,&go_right,&go_up,&go_down);

		// left-right movement
		
	if (go_left) object_player_turn_direct(obj,270);
	if (go_right) object_player_turn_direct(obj,90);
	
		// up-down movement
	
    if (go_down) {
        obj->vert_move.moving=TRUE;
        if (obj->vert_move.reverse) obj->vert_move.speed=0;
        obj->vert_move.reverse=FALSE;
    }

	if (go_up) {
        obj->vert_move.moving=TRUE;
        if (!obj->vert_move.reverse) obj->vert_move.speed=0;
        obj->vert_move.reverse=TRUE;
    }
}

/* =======================================================

      Top-Down Input Type
      
======================================================= */

void player_top_down_input(obj_type *obj)
{
	bool			go_left,go_right,go_up,go_down;

		// get input

	player_get_4_way_input(&go_left,&go_right,&go_up,&go_down);
	
		// all the movements
		
	if (go_left) {
		if (go_up) {
			object_player_turn_direct(obj,angle_add(obj->turn.top_down_ang_offset,315.0f));
			return;
		}
		if (go_down) {
			object_player_turn_direct(obj,angle_add(obj->turn.top_down_ang_offset,225.0f));
			return;
		}
		object_player_turn_direct(obj,angle_add(obj->turn.top_down_ang_offset,270.0f));
		return;
	}
	
	if (go_right) {
		if (go_up) {
			object_player_turn_direct(obj,angle_add(obj->turn.top_down_ang_offset,45.0f));
			return;
		}
		if (go_down) {
			object_player_turn_direct(obj,angle_add(obj->turn.top_down_ang_offset,135.0f));
			return;
		}
		object_player_turn_direct(obj,angle_add(obj->turn.top_down_ang_offset,90.0f));
		return;
	}
	
	if (go_up) {
		object_player_turn_direct(obj,angle_add(obj->turn.top_down_ang_offset,0.0f));
		return;
	}
	
	if (go_down) {
		object_player_turn_direct(obj,angle_add(obj->turn.top_down_ang_offset,180.0f));
		return;
	}
}

/* =======================================================

      Fly Input Type
      
======================================================= */

void player_fly_input(obj_type *obj)
{
	float			mouse_x,mouse_y;
	bool			go_forward,go_backward,go_side_left,go_side_right;

		// start with no movement
		
    obj->forward_move.moving=FALSE;
	obj->side_move.moving=FALSE;
	obj->vert_move.moving=FALSE;

		// get input

	player_get_6_way_input(obj,&mouse_x,&mouse_y,&go_forward,&go_backward,&go_side_left,&go_side_right);

		// turning and looking

	obj->turn.ang_add.y=player_mouse_smooth(mouse_x,obj->turn.ang_add.y);
	obj->look.ang_add=player_mouse_smooth(mouse_y,obj->look.ang_add);

		// forward/backwards movement

	if (go_forward) {
		obj->forward_move.moving=TRUE;
		if (obj->forward_move.reverse) obj->forward_move.speed=0;
		obj->forward_move.reverse=FALSE;
	}

	if (go_backward) {
		obj->forward_move.moving=TRUE;
		if (!obj->forward_move.reverse) obj->forward_move.speed=0;
		obj->forward_move.reverse=TRUE;
	}

		// flying has a single speed

	obj->forward_move.running=FALSE;

		// y movement

	player_movement_fly_swim_y_input(obj);
}

/* =======================================================

      Thrust Input Type
      
======================================================= */

void player_thrust_input(obj_type *obj)
{
	float			mouse_x,mouse_y;
	bool			go_forward,go_backward,go_side_left,go_side_right;

		// start with no movement
		
    obj->forward_move.moving=FALSE;
	obj->side_move.moving=FALSE;
	obj->vert_move.moving=FALSE;

		// get input

	player_get_6_way_input(obj,&mouse_x,&mouse_y,&go_forward,&go_backward,&go_side_left,&go_side_right);

		// turning and looking

	obj->turn.ang_add.y=player_mouse_smooth(mouse_x,obj->turn.ang_add.y);
	obj->look.ang_add=player_mouse_smooth(mouse_y,obj->look.ang_add);

		// forward/backwards movement

	if (go_forward) {
		obj->forward_move.moving=TRUE;
		if (obj->forward_move.reverse) obj->forward_move.speed=0;
		obj->forward_move.reverse=FALSE;
	}

	if (go_backward) {
		obj->forward_move.moving=TRUE;
		if (!obj->forward_move.reverse) obj->forward_move.speed=0;
		obj->forward_move.reverse=TRUE;
	}

		// thrusting has a single speed

	obj->forward_move.running=FALSE;
}

/* =======================================================

      Get Player Input
      
======================================================= */

void player_get_input(void)
{
	obj_type		*obj;
	
		// get player object
	
	obj=object_find_uid(server.player_obj_uid);

		// no input if menu is on

	if (view.menu.active) {
	    obj->forward_move.moving=FALSE;
		obj->side_move.moving=FALSE;
		obj->vert_move.moving=FALSE;
		obj->turn.ang_add.y=0.0f;
		obj->look.ang_add=0.0f;
		return;
	}

		// message inputs
		
	if (!player_message_input(obj)) return;
		
		// commands (game and player)
		
	player_command_input(obj);
	
        // check for non-input states
        
	if ((obj->hidden) || (obj->input_freeze)) return;
	
		// proper movement routines
	
	switch (obj->input_mode) {
	
		case im_fpp:
			player_fpp_input(obj);
			break;
			
		case im_side_scroll:
			player_side_scroll_input(obj);
			break;
			
		case im_top_down:
			player_top_down_input(obj);
			break;

		case im_fly:
			player_fly_input(obj);
			break;

		case im_thrust:
			player_thrust_input(obj);
			break;
	
	}

        // jump and duck
		
	if (input_action_get_state(nc_jump)) object_start_jump(obj);
    
	if (input_action_get_state(nc_duck)) {
        object_start_duck(obj);
    }
    else {
        object_start_stand(obj);
    }
	
        // weapons
    
	player_weapon_input(obj);
	
		// enter and exit
		
	player_enter_exit_input(obj);
	
		// clear mouse wheel state (since it's single state instead of held, like buttons)
		
	input_clear_mouse_wheel_state();
}
