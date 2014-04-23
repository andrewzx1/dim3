/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Map Ambients

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

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "interface.h"
#include "objects.h"

int							map_ambient_idx,map_ambient_buffer_idx;
float						map_ambient_pitch;
char						map_ambient_name[name_str_len];

extern map_type				map;
extern server_type			server;

/* =======================================================

      Ambients for Map, Objects, and Liquids
      
======================================================= */

void ambient_add_map_sounds(void)
{
	int					n;
	map_sound_type		*mapsound;
		
	mapsound=map.sounds;
		
	for (n=0;n!=map.nsound;n++) {
		if ((mapsound->on) && (mapsound->buffer_idx!=-1)) {
			al_ambient_list_add(mapsound->buffer_idx,&mapsound->pnt,mapsound->pitch);
		}
		mapsound++;
	}
}

void ambient_add_objects(void)
{
	int					n;
	obj_type			*obj;
	obj_snd_ambient		*ambient;
	
	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;

		if (obj->hidden) continue;		// no ambients for hidden objects

		ambient=&obj->ambient;
		if ((ambient->on) && (ambient->buffer_idx!=-1)) {
			al_ambient_list_add(ambient->buffer_idx,&obj->pnt,ambient->pitch);
		}
	}
}

void ambient_add_liquids(void)
{
	obj_type			*player_obj;
	map_liquid_type		*liq;
	
	player_obj=server.obj_list.objs[server.player_obj_idx];
	if (player_obj==NULL) return;
	
	if ((player_obj->liquid.mode!=lm_under) || (player_obj->contact.liquid_idx==-1)) return;
	
	liq=&map.liquid.liquids[player_obj->contact.liquid_idx];
	if (liq->ambient.buffer_idx==-1) return;
	
	al_ambient_list_add(liq->ambient.buffer_idx,&player_obj->pnt,1.0f);
}

/* =======================================================

      Full Map Ambients
      
======================================================= */

void map_start_ambient(void)
{
	map_ambient_idx=-1;
	map_ambient_buffer_idx=-1;
	map_ambient_name[0]=0x0;
}

void map_end_ambient(void)
{
	if (map_ambient_idx!=-1) {
		al_stop_source(map_ambient_idx);
		map_ambient_idx=-1;
		map_ambient_name[0]=0x0;
	}
}

void map_set_ambient(char *name,float pitch)
{
	strcpy(map_ambient_name,name);
	map_ambient_pitch=pitch;
	
	map_ambient_buffer_idx=al_find_buffer(name);
	if (map_ambient_buffer_idx==-1) {
		map_ambient_idx=-1;
		map_ambient_name[0]=0x0;
		return;
	}

	map_ambient_idx=al_play_source(map_ambient_buffer_idx,NULL,pitch,TRUE,FALSE,TRUE,FALSE);
}

void map_restart_ambient(void)
{
	if (map_ambient_idx!=-1) {
		map_ambient_idx=al_play_source(map_ambient_buffer_idx,NULL,map_ambient_pitch,TRUE,FALSE,TRUE,FALSE);
	}
}

void map_clear_ambient(void)
{
	map_end_ambient();
}


