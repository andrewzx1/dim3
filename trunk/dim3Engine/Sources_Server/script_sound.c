/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: sound object

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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "scripts.h"
#include "objects.h"
#include "weapons.h"
#include "sounds.h"
#include "network.h"

extern js_type				js;
extern setup_type			setup;
extern server_type			server;
extern network_setup_type	net_setup;

JSBool js_sound_get_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp);
JSBool js_sound_set_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp);
JSBool js_sound_play_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);
JSBool js_sound_play_at_object_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);
JSBool js_sound_play_global_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);
JSBool js_sound_play_global_player_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);
JSBool js_sound_start_music_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);
JSBool js_sound_stop_music_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);
JSBool js_sound_fade_in_music_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);
JSBool js_sound_fade_out_music_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);
JSBool js_sound_fade_out_fade_in_music_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);

script_js_function	sound_functions[]={
							{"play",				js_sound_play_func,						5},
							{"playAtObject",		js_sound_play_at_object_func,			3},
							{"playGlobal",			js_sound_play_global_func,				2},
							{"playGlobalPlayer",	js_sound_play_global_player_func,		2},
							{"startMusic",			js_sound_start_music_func,				1},
							{"stopMusic",			js_sound_stop_music_func,				0},
							{"fadeInMusic",			js_sound_fade_in_music_func,			2},
							{"fadeOutMusic",		js_sound_fade_out_music_func,			1},
							{"fadeOutFadeInMusic",	js_sound_fade_out_fade_in_music_func,	1},
							{0}};

JSClassRef			sound_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_global_sound_object(void)
{
	sound_class=script_create_class("sound_class",js_sound_get_property,js_sound_set_property);
}

void script_free_global_sound_object(void)
{
	script_free_class(sound_class);
}

JSObject* script_add_global_sound_object(JSObject *parent_obj)
{
	return(script_create_child_object(parent_obj,sound_class,"sound",NULL,sound_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_sound_get_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp)
{
	return(script_get_property(cx,j_obj,id,vp,NULL));
}

JSBool js_sound_set_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp)
{
	return(script_set_property(cx,j_obj,id,vp,NULL));
}

/* =======================================================

      Sound Functions
      
======================================================= */

bool script_sound_play(char *name,d3pnt *pt,float pitch,bool global,bool atplayer)
{
	int				buffer_idx,sound_obj_uid;
	bool			remote_ok,player;
	obj_type		*obj;
	weapon_type		*weap;

		// check if this is player

	if (atplayer) {
		player=TRUE;
		sound_obj_uid=server.player_obj_uid;
	}
	else {
		player=FALSE;
		sound_obj_uid=-1;

		if (js.attach.thing_type==thing_type_object) {
			obj=object_find_uid(js.attach.thing_uid);
			sound_obj_uid=obj->uid;
			player=obj->player;
		}
	}
	
		// turn off global if you aren't the player

	if (!player) global=FALSE;
	
		// play sound
		
	buffer_idx=al_find_buffer(name);
	if (buffer_idx==-1) {
		JS_ReportError(js.cx,"Named sound does not exist: %s",name);
		return(FALSE);
	}

	al_play_source(buffer_idx,pt,pitch,FALSE,FALSE,global,player);

		// run sound watches

	if (sound_obj_uid!=-1) object_watch_sound_alert(pt,sound_obj_uid,name);
	
		// detect if sound should be remoted
		
	if (net_setup.client.joined) {
	
		remote_ok=FALSE;
		
		switch (js.attach.thing_type) {
		
			case thing_type_object:
				obj=object_find_uid(js.attach.thing_uid);
				if (obj!=NULL) remote_ok=(((obj->player) || (obj->bot)) && (!obj->remote.on));
				break;
				
			case thing_type_weapon:
				weap=weapon_find_uid(js.attach.thing_uid);
				if (weap!=NULL) {
					obj=object_find_uid(weap->obj_uid);
					if (obj!=NULL) remote_ok=(((obj->player) || (obj->bot)) && (!obj->remote.on));
				}
				break;
		}
		
		if (remote_ok) net_client_send_sound(obj->remote.uid,pt->x,pt->y,pt->z,pitch,name);
	}

	return(TRUE);
}

JSBool js_sound_play_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	float			pitch;
	char			name[name_str_len];
	d3pnt			pt;
	
	script_value_to_string(argv[0],name,name_str_len);
	pt.x=script_value_to_int(argv[1]);
	pt.z=script_value_to_int(argv[2]);
	pt.y=script_value_to_int(argv[3]);
	pitch=script_value_to_float(argv[4]);

	if (!script_sound_play(name,&pt,pitch,FALSE,FALSE)) return(FALSE);
	
	return(TRUE);
}

JSBool js_sound_play_at_object_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	float			pitch;
	char			name[name_str_len];
	obj_type		*obj;

	obj=script_find_obj_from_uid_arg(argv[1]);
	if (obj==NULL) return(FALSE);
	
	script_value_to_string(argv[0],name,name_str_len);
	pitch=script_value_to_float(argv[2]);

	if (!script_sound_play(name,&obj->pnt,pitch,FALSE,FALSE)) return(FALSE);
	
	return(TRUE);
}

JSBool js_sound_play_global_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	float			pitch;
	char			name[name_str_len];
	d3pnt			pt;
	
	script_value_to_string(argv[0],name,name_str_len);
	pitch=script_value_to_float(argv[1]);
	
	pt.x=pt.y=pt.z=0;

	if (!script_sound_play(name,&pt,pitch,TRUE,FALSE)) return(FALSE);
	
	return(TRUE);
}

JSBool js_sound_play_global_player_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	float			pitch;
	char			name[name_str_len];
	d3pnt			pt;
	
	script_value_to_string(argv[0],name,name_str_len);
	pitch=script_value_to_float(argv[1]);
	
	pt.x=pt.y=pt.z=0;

	if (!script_sound_play(name,&pt,pitch,TRUE,TRUE)) return(FALSE);
	
	return(TRUE);
}

/* =======================================================

      Music Functions
      
======================================================= */

JSBool js_sound_start_music_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	char			name[name_str_len],wave_path[1024];

	if (!setup.music_on) return(TRUE);
	
	script_value_to_string(argv[0],name,name_str_len);

	file_paths_data(&setup.file_path_setup,wave_path,"Music",name,"wav");
	if (!al_music_play(name,wave_path)) {
		JS_ReportError(js.cx,"Named music does not exist or could not be played: %s",name);
		return(FALSE);
	}

	return(TRUE);
}

JSBool js_sound_stop_music_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	if (!setup.music_on) return(TRUE);

	al_music_stop();
	return(TRUE);
}

JSBool js_sound_fade_in_music_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	int				msec;
	char			name[name_str_len],wave_path[1024];
	
	if (!setup.music_on) return(TRUE);

	script_value_to_string(argv[0],name,name_str_len);
	msec=script_value_to_int(argv[1]);

	file_paths_data(&setup.file_path_setup,wave_path,"Music",name,"wav");
	if (!al_music_fade_in(js.time.current_tick,name,wave_path,msec)) {
		JS_ReportError(js.cx,"Named music does not exist or could not be played: %s",name);
		return(FALSE);
	}

	return(TRUE);
}

JSBool js_sound_fade_out_music_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	int					msec;
	
	if (!setup.music_on) return(TRUE);

	msec=script_value_to_int(argv[0]);
	al_music_fade_out(js.time.current_tick,msec);

	return(TRUE);
}

JSBool js_sound_fade_out_fade_in_music_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	int					fade_out_msec,fade_in_msec;
	char				name[name_str_len],wave_path[1024];
	
	if (!setup.music_on) return(TRUE);

	script_value_to_string(argv[0],name,name_str_len);
	fade_out_msec=script_value_to_int(argv[1]);
	fade_in_msec=script_value_to_int(argv[2]);

	file_paths_data(&setup.file_path_setup,wave_path,"Music",name,"wav");
	if (!al_music_fade_out_fade_in(js.time.current_tick,name,wave_path,fade_out_msec,fade_in_msec)) {
		JS_ReportError(js.cx,"Named music does not exist or could not be played: %s",name);
		return(FALSE);
	}

	return(TRUE);
}
