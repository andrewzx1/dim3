/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Cinema Action Setting Dialog

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

#include "dialog.h"

extern map_type				map;

#define kCinemaActionActorType				FOUR_CHAR_CODE('actt')
#define kCinemaActionActorName				FOUR_CHAR_CODE('actr')
#define kCinemaActionAction					FOUR_CHAR_CODE('actn')
#define kCinemaActionNode					FOUR_CHAR_CODE('node')
#define kCinemaActionTimeStart				FOUR_CHAR_CODE('tsrt')
#define kCinemaActionTimeEnd				FOUR_CHAR_CODE('tend')
#define kCinemaActionAnimation				FOUR_CHAR_CODE('anim')
#define kCinemaActionNextAnimation			FOUR_CHAR_CODE('nanm')
#define kCinemaActionText					FOUR_CHAR_CODE('text')
#define kCinemaActionReverseMove			FOUR_CHAR_CODE('rmov')

bool						dialog_cinema_action_settings_cancel;
map_cinema_action_type		*dialog_cinema_action_action;
ControlRef					dialog_cinema_actor_type_ctrl,
							dialog_cinema_action_type_ctrl;
WindowRef					dialog_cinema_action_settings_wind;

/* =======================================================

      Cinema Action Setting Actor Type Setup
      
======================================================= */

void cinema_action_setting_enable_setup(void)
{
	int				actor_type,action_type;
	
	actor_type=dialog_get_combo(dialog_cinema_action_settings_wind,kCinemaActionActorType,0);
	action_type=dialog_get_combo(dialog_cinema_action_settings_wind,kCinemaActionAction,0);

	dialog_enable(dialog_cinema_action_settings_wind,kCinemaActionActorName,0,((actor_type!=cinema_actor_camera) && (actor_type!=cinema_actor_player)));
	
	if (action_type==cinema_action_move) {
		dialog_enable(dialog_cinema_action_settings_wind,kCinemaActionTimeEnd,0,TRUE);
		dialog_set_int(dialog_cinema_action_settings_wind,kCinemaActionTimeEnd,0,dialog_cinema_action_action->end_msec);
	}
	else {
		dialog_enable(dialog_cinema_action_settings_wind,kCinemaActionTimeEnd,0,FALSE);
		dialog_set_text(dialog_cinema_action_settings_wind,kCinemaActionTimeEnd,0,"");
	}
	
	dialog_enable(dialog_cinema_action_settings_wind,kCinemaActionNode,0,((action_type==cinema_action_place) || (action_type==cinema_action_move)));
		
	dialog_enable(dialog_cinema_action_settings_wind,kCinemaActionText,0,(actor_type==cinema_actor_hud_text));
	dialog_enable(dialog_cinema_action_settings_wind,kCinemaActionAnimation,0,((actor_type==cinema_actor_player) || (actor_type==cinema_actor_object)));
	dialog_enable(dialog_cinema_action_settings_wind,kCinemaActionNextAnimation,0,((actor_type==cinema_actor_player) || (actor_type==cinema_actor_object)));
}

void cinema_action_setting_actor_type_setup(void)
{
	int				actor_type;
	
	actor_type=dialog_get_combo(dialog_cinema_action_settings_wind,kCinemaActionActorType,0);
	
	switch (actor_type) {
	
		case cinema_actor_camera:
		case cinema_actor_player:
			dialog_clear_combo(dialog_cinema_action_settings_wind,kCinemaActionActorName,0);
			dialog_set_combo(dialog_cinema_action_settings_wind,kCinemaActionActorName,0,0);
			break;
			
		case cinema_actor_object:
			dialog_special_combo_fill_spot(dialog_cinema_action_settings_wind,kCinemaActionActorName,0,dialog_cinema_action_action->actor_name);
			break;
		
		case cinema_actor_movement:
			dialog_special_combo_fill_movement(dialog_cinema_action_settings_wind,kCinemaActionActorName,0,dialog_cinema_action_action->actor_name);
			break;
		
		case cinema_actor_particle:
			dialog_special_combo_fill_particle(dialog_cinema_action_settings_wind,kCinemaActionActorName,0,dialog_cinema_action_action->actor_name);
			break;
			
		case cinema_actor_hud_text:
			dialog_special_combo_fill_hud_text(dialog_cinema_action_settings_wind,kCinemaActionActorName,0,dialog_cinema_action_action->actor_name);
			break;
			
		case cinema_actor_hud_bitmap:
			dialog_special_combo_fill_hud_bitmap(dialog_cinema_action_settings_wind,kCinemaActionActorName,0,dialog_cinema_action_action->actor_name);
			break;
		
	}
}

void cinema_action_setting_actor_type_get(char *name)
{
	int				actor_type;
	
	name[0]=0x0;
	
	actor_type=dialog_get_combo(dialog_cinema_action_settings_wind,kCinemaActionActorType,0);
	
	switch (actor_type) {
	
		case cinema_actor_object:
			dialog_special_combo_get_spot(dialog_cinema_action_settings_wind,kCinemaActionActorName,0,name,name_str_len);
			break;
		
		case cinema_actor_movement:
			dialog_special_combo_get_movement(dialog_cinema_action_settings_wind,kCinemaActionActorName,0,name,name_str_len);
			break;
		
		case cinema_actor_particle:
			dialog_special_combo_get_particle(dialog_cinema_action_settings_wind,kCinemaActionActorName,0,name,name_str_len);
			break;
			
		case cinema_actor_hud_text:
			dialog_special_combo_get_hud_text(dialog_cinema_action_settings_wind,kCinemaActionActorName,0,name,name_str_len);
			break;
			
		case cinema_actor_hud_bitmap:
			dialog_special_combo_get_hud_bitmap(dialog_cinema_action_settings_wind,kCinemaActionActorName,0,name,name_str_len);
			break;
		
	}
}

/* =======================================================

      Cinema Action Setting Event Handlers
      
======================================================= */

static pascal OSStatus cinema_action_settings_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
				
				case kHICommandCancel:
					dialog_cinema_action_settings_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_cinema_action_settings_wind);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_cinema_action_settings_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

static pascal OSStatus cinema_actor_type_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	cinema_action_setting_actor_type_setup();
	cinema_action_setting_enable_setup();
		
	return(eventNotHandledErr);
}

static pascal OSStatus cinema_action_type_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	cinema_action_setting_enable_setup();
		
	return(eventNotHandledErr);
}

/* =======================================================

      Run Cinema Action Setting
      
======================================================= */

bool dialog_cinema_action_settings_run(map_cinema_action_type *action)
{
	ControlID				ctrl_id;
	EventHandlerUPP			event_upp,actor_ctrl_event_upp,action_ctrl_event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}},
							actor_ctrl_event_list[]={{kEventClassControl,kEventControlHit}},
							action_ctrl_event_list[]={{kEventClassControl,kEventControlHit}};
	
		// open the dialog
		
	dialog_open(&dialog_cinema_action_settings_wind,"CinemaActionSettings");
	
	dialog_cinema_action_action=action;

		// set controls
		
	dialog_set_combo(dialog_cinema_action_settings_wind,kCinemaActionActorType,0,action->actor_type);
	cinema_action_setting_actor_type_setup();
	dialog_set_combo(dialog_cinema_action_settings_wind,kCinemaActionAction,0,action->action);
	dialog_special_combo_fill_node(dialog_cinema_action_settings_wind,kCinemaActionNode,0,action->node_name);
	
	dialog_set_int(dialog_cinema_action_settings_wind,kCinemaActionTimeStart,0,action->start_msec);
	dialog_set_int(dialog_cinema_action_settings_wind,kCinemaActionTimeEnd,0,action->end_msec);

	dialog_set_text(dialog_cinema_action_settings_wind,kCinemaActionAnimation,0,action->animation_name);
	dialog_set_text(dialog_cinema_action_settings_wind,kCinemaActionNextAnimation,0,action->next_animation_name);
	
	dialog_set_text(dialog_cinema_action_settings_wind,kCinemaActionText,0,action->text_str);

	dialog_set_boolean(dialog_cinema_action_settings_wind,kCinemaActionReverseMove,0,action->move_reverse);
	
	dialog_set_focus(dialog_cinema_action_settings_wind,kCinemaActionActorName,0);
	cinema_action_setting_enable_setup();
	
		// show window
	
	ShowWindow(dialog_cinema_action_settings_wind);
	
		// install event handlers
		
	event_upp=NewEventHandlerUPP(cinema_action_settings_event_proc);
	InstallWindowEventHandler(dialog_cinema_action_settings_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);

	ctrl_id.signature=kCinemaActionActorType;
	ctrl_id.id=0;
	GetControlByID(dialog_cinema_action_settings_wind,&ctrl_id,&dialog_cinema_actor_type_ctrl);
	
	actor_ctrl_event_upp=NewEventHandlerUPP(cinema_actor_type_event_proc);
	InstallControlEventHandler(dialog_cinema_actor_type_ctrl,actor_ctrl_event_upp,GetEventTypeCount(actor_ctrl_event_list),actor_ctrl_event_list,dialog_cinema_action_settings_wind,NULL);

	ctrl_id.signature=kCinemaActionActionType;
	ctrl_id.id=0;
	GetControlByID(dialog_cinema_action_settings_wind,&ctrl_id,&dialog_cinema_action_type_ctrl);
	
	action_ctrl_event_upp=NewEventHandlerUPP(cinema_action_type_event_proc);
	InstallControlEventHandler(dialog_cinema_action_type_ctrl,action_ctrl_event_upp,GetEventTypeCount(action_ctrl_event_list),action_ctrl_event_list,dialog_cinema_action_settings_wind,NULL);

		// modal window
		
	dialog_cinema_action_settings_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_cinema_action_settings_wind);
	
		// dialog to data
		
	if (!dialog_cinema_action_settings_cancel) {
		action->actor_type=dialog_get_combo(dialog_cinema_action_settings_wind,kCinemaActionActorType,0);
		cinema_action_setting_actor_type_get(action->actor_name);
		action->action=dialog_get_combo(dialog_cinema_action_settings_wind,kCinemaActionAction,0);
		dialog_special_combo_get_node(dialog_cinema_action_settings_wind,kCinemaActionNode,0,action->node_name,name_str_len);

		action->start_msec=dialog_get_int(dialog_cinema_action_settings_wind,kCinemaActionTimeStart,0);
		action->end_msec=dialog_get_int(dialog_cinema_action_settings_wind,kCinemaActionTimeEnd,0);
		
		dialog_get_text(dialog_cinema_action_settings_wind,kCinemaActionAnimation,0,action->animation_name,name_str_len);
		dialog_get_text(dialog_cinema_action_settings_wind,kCinemaActionNextAnimation,0,action->next_animation_name,name_str_len);
		
		dialog_get_text(dialog_cinema_action_settings_wind,kCinemaActionText,0,action->text_str,256);
	
		action->move_reverse=dialog_get_boolean(dialog_cinema_action_settings_wind,kCinemaActionReverseMove,0);
	}

		// close window
		
	DisposeWindow(dialog_cinema_action_settings_wind);
	
	return(!dialog_cinema_action_settings_cancel);
}

