/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Property Palette Cinema

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
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "ui_common.h"
#include "interface.h"
#include "dialog.h"

#define kCinemaPropertyName					0
#define kCinemaPropertyLength				1
#define kCinemaPropertyFreezeInput			2
#define kCinemaPropertyShowHUD				3

#define kCinemaPropertyActionAdd			10

#define kCinemaPropertyAction				1000
#define kCinemaPropertyActionDelete			2000

extern map_type					map;
extern editor_state_type		state;
extern editor_setup_type		setup;

extern list_palette_type		property_palette;

/*
typedef struct		{
						int									action,actor_type,
															start_msec,end_msec;
						bool								move_reverse;
						char								actor_name[name_str_len],
															animation_name[name_str_len],
															next_animation_name[name_str_len],
															node_name[name_str_len],
															text_str[256];
					} map_cinema_action_type;

typedef struct		{
						int									naction,len_msec;
						bool								freeze_input,show_hud;
						char								name[name_str_len];
						map_cinema_action_type				*actions;
					} map_cinema_type;
*/

/* =======================================================

      Property Palette Fill Cinema
      
======================================================= */

void property_palette_fill_cinema(int cinema_idx)
{
	int						n;
	char					str[256];
	char					actor_type_str[][32]={"Camera","Player","Object","Movement","Particle","HUD Text","HUD Bitmap"},
							action_type_str[][32]={"None","Place","Move","Stop","Show","Hide"};
	map_cinema_type			*cinema;
	map_cinema_action_type	*action;

	cinema=&map.cinema.cinemas[cinema_idx];

	list_palette_add_header(&property_palette,0,"Cinema Options");
	list_palette_add_string(&property_palette,kCinemaPropertyName,"Name",cinema->name,FALSE);
	list_palette_add_string_int(&property_palette,kCinemaPropertyLength,"Total Length",cinema->len_msec,FALSE);

	list_palette_add_header(&property_palette,0,"Cinema Settings");
	list_palette_add_checkbox(&property_palette,kCinemaPropertyFreezeInput,"Freeze Input",cinema->freeze_input,FALSE);
	list_palette_add_checkbox(&property_palette,kCinemaPropertyShowHUD,"Show HUD",cinema->show_hud,FALSE);

	list_palette_add_header_button(&property_palette,kCinemaPropertyActionAdd,"Cinema Actions",list_button_plus);

	action=cinema->actions;

	for (n=0;n!=cinema->naction;n++) {

		if ((action->actor_type!=cinema_actor_camera) && (action->actor_type!=cinema_actor_player)) {
			sprintf(str,"%s: %s",actor_type_str[action->actor_type],action->actor_name);
		}
		else {
			strcpy(str,actor_type_str[action->actor_type]);
		}

		strcat(str," ");
		strcat(str,action_type_str[action->action]);

		list_palette_add_string_selectable_button(&property_palette,(kCinemaPropertyAction+n),list_button_minus,(kCinemaPropertyActionDelete+n),str,NULL,(state.cur_cinema_action_idx==n),FALSE);
	
		action++;
	}
}

/* =======================================================

      Property Palette Click Cinema
      
======================================================= */

void property_palette_click_cinema(int cinema_idx,int id)
{
	map_cinema_type		*cinema;

	cinema=&map.cinema.cinemas[cinema_idx];

		// click action

	if ((id>=kCinemaPropertyAction) && (id<kCinemaPropertyActionDelete)) {
		state.cur_cinema_action_idx=id-kCinemaPropertyAction;
		main_wind_draw();
		return;
	}

		// add action

	if (id==kCinemaPropertyActionAdd) {
		state.cur_cinema_action_idx=map_cinema_add_action(&map,cinema_idx);
		main_wind_draw();
		return;
	}

		// delete action

	if (id>=kCinemaPropertyActionDelete) {
		state.cur_cinema_action_idx=-1;
		map_cinema_delete_action(&map,cinema_idx,(id-kCinemaPropertyActionDelete));
		main_wind_draw();
		return;
	}

		// regular items

	switch (id) {

		case kCinemaPropertyName:
			dialog_property_string_run(list_string_value_string,(void*)cinema->name,name_str_len,0,0);
			break;

		case kCinemaPropertyLength:
			dialog_property_string_run(list_string_value_positive_int,(void*)&cinema->len_msec,0,0,0);
			break;

		case kCinemaPropertyFreezeInput:
			cinema->freeze_input=!cinema->freeze_input;
			break;

		case kCinemaPropertyShowHUD:
			cinema->show_hud=!cinema->show_hud;
			break;

	}

	main_wind_draw();
}

