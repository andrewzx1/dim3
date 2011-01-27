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

#define kCinmeaPropertySort					10

#define kCinemaPropertyActionAdd			20

#define kCinemaPropertyAction				1000
#define kCinemaPropertyActionDelete			2000

extern map_type					map;
extern editor_state_type		state;
extern editor_setup_type		setup;

extern list_palette_type		property_palette;

char							action_actor_type_str[][32]={"Camera","Player","Object","Movement","Particle","Text","Bitmap"},
								action_action_type_str[][32]={"None","Place","Move","Stop","Show","Hide"};

/* =======================================================

      Property Palette Fill Cinema
      
======================================================= */

void property_palette_fill_cinema(int cinema_idx)
{
	int						n;
	char					str[256],str2[256];
	map_cinema_type			*cinema;
	map_cinema_action_type	*action;

	cinema=&map.cinema.cinemas[cinema_idx];

	list_palette_add_header(&property_palette,0,"Cinema Options");
	list_palette_add_string(&property_palette,kCinemaPropertyName,"Name",cinema->name,FALSE);
	list_palette_add_string_int(&property_palette,kCinemaPropertyLength,"Total Length",cinema->len_msec,FALSE);

	list_palette_add_header(&property_palette,0,"Cinema Settings");
	list_palette_add_checkbox(&property_palette,kCinemaPropertyFreezeInput,"Freeze Input",cinema->freeze_input,FALSE);
	list_palette_add_checkbox(&property_palette,kCinemaPropertyShowHUD,"Show HUD",cinema->show_hud,FALSE);

	list_palette_add_header(&property_palette,0,"Cinema Sort");
	list_palette_add_string_selectable_button(&property_palette,kCinmeaPropertySort,list_button_set,kCinmeaPropertySort,"Sort Actions",NULL,FALSE,FALSE);

	list_palette_add_header_button(&property_palette,kCinemaPropertyActionAdd,"Cinema Actions",list_button_plus);

	action=cinema->actions;

	for (n=0;n!=cinema->naction;n++) {

		sprintf(str,"%d: ",action->start_msec);
		if (action->action!=cinema_action_none) {
			strcat(str,action_action_type_str[action->action]);
			strcat(str," ");
		}

		if ((action->actor_type!=cinema_actor_camera) && (action->actor_type!=cinema_actor_player)) {
			sprintf(str2,"%s(%s)",action_actor_type_str[action->actor_type],action->actor_name);
		}
		else {
			strcpy(str2,action_actor_type_str[action->actor_type]);
		}

		strcat(str,str2);

		list_palette_add_string_selectable_button(&property_palette,(kCinemaPropertyAction+n),list_button_minus,(kCinemaPropertyActionDelete+n),str,NULL,(state.cur_cinema_action_idx==n),FALSE);
	
		action++;
	}
}

/* =======================================================

      Sort Cinemas
      
======================================================= */

void cinemas_action_sort(int cinema_idx)
{
	int						n,k,idx,count,sz;
	int						*sort_list;
	map_cinema_type			*cinema;
	map_cinema_action_type	*action,*a_ptr;
	
	cinema=&map.cinema.cinemas[cinema_idx];
	if (cinema->naction<=1) return;
	
		// sort the items
		
	sort_list=(int*)malloc((cinema->naction+1)*sizeof(int));
	if (sort_list==NULL) return;
	
	count=0;
	action=cinema->actions;
	
	for (n=0;n!=cinema->naction;n++) {
	
		idx=count;
		
		for (k=0;k!=count;k++) {
			if (cinema->actions[sort_list[k]].start_msec>action->start_msec) {
				idx=k;
				break;
			}
		}
		
		sz=(count-idx)*sizeof(int);
		if (sz>0) memmove(&sort_list[idx+1],&sort_list[idx],sz);
		
		sort_list[idx]=n;
		count++;

		action++;
	}
	
		// find the new index
		
	for (n=0;n!=cinema->naction;n++) {
		if (sort_list[n]==state.cur_cinema_action_idx) {
			state.cur_cinema_action_idx=n;
			break;
		}
	}
	
		// sort the list
		
	a_ptr=(map_cinema_action_type*)malloc(cinema->naction*sizeof(map_cinema_action_type));
	if (a_ptr==NULL) {
		free(sort_list);
		return;
	}
	
	for (n=0;n!=cinema->naction;n++) {
		memmove(&a_ptr[n],&cinema->actions[sort_list[n]],sizeof(map_cinema_action_type));
	}
	
	memmove(cinema->actions,a_ptr,(cinema->naction*sizeof(map_cinema_action_type)));
	free(a_ptr);
		
	free(sort_list);
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

		// sort action

	if (id==kCinmeaPropertySort) {
		cinemas_action_sort(cinema_idx);
		main_wind_draw();
		return;
	}

		// regular items

	state.cur_cinema_action_idx=-1;

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

