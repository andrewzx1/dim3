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
	map_cinema_type		*cinema;

	cinema=&map.cinema.cinemas[cinema_idx];

	list_palette_add_header(&property_palette,0,"Cinema Options");
	list_palette_add_string(&property_palette,kCinemaPropertyName,"Name",cinema->name,FALSE);
	list_palette_add_string_int(&property_palette,kCinemaPropertyLength,"Total Length",cinema->len_msec,FALSE);

	list_palette_add_header(&property_palette,0,"Cinema Settings");
	list_palette_add_checkbox(&property_palette,kCinemaPropertyFreezeInput,"Freeze Input",cinema->freeze_input,FALSE);
	list_palette_add_checkbox(&property_palette,kCinemaPropertyShowHUD,"Show HUD",cinema->show_hud,FALSE);

	list_palette_add_header_button(&property_palette,kCinemaPropertyActionAdd,"Cinema Actions",list_button_plus);
}

/* =======================================================

      Property Palette Click Cinema
      
======================================================= */

void property_palette_click_cinema(int cinema_idx,int id)
{
	/*
	map_cinema_type		*cinema;

	cinema=&map.cinema.cinemas[cinema_idx];

	switch (id) {

		case kSoundPropertyOn:
			sound->on=!sound->on;
			break;

		case kSoundPropertyName:
			property_palette_pick_sound(sound->name,FALSE);
			break;

		case kSoundPropertyPitch:
			dialog_property_string_run(list_string_value_positive_float,(void*)&sound->pitch,0,0,0);
			break;

	}
*/
	main_wind_draw();
}

