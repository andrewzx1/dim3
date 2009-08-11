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

JSValueRef js_sound_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_sound_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_sound_play_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_sound_play_at_object_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_sound_play_global_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_sound_play_global_player_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_sound_start_music_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_sound_stop_music_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_sound_fade_in_music_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_sound_fade_out_music_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_sound_fade_out_fade_in_music_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

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

JSObjectRef script_add_global_sound_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,sound_class,"sound",NULL,sound_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_sound_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,NULL));
}

bool js_sound_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,NULL));
}

/* =======================================================

      Sound Exceptions
      
======================================================= */

JSValueRef js_sound_name_exception(char *name)
{
	char			err_str[256];

	sprintf(err_str,"Named sound does not exist: %s",name);
	return(script_create_exception(err_str));
}

JSValueRef js_sound_music_name_exception(char *name)
{
	char			err_str[256];

	sprintf(err_str,"Named music does not exist or could not be played: %s",name);
	return(script_create_exception(err_str));
}

/* =======================================================

      Main Sound Play
      
======================================================= */

void script_sound_play(char *name,d3pnt *pt,float pitch,bool global,bool atplayer,JSValueRef *exception)
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
		*exception=js_sound_name_exception(name);
		return;
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
}

/* =======================================================

      Sound Functions
      
======================================================= */

JSValueRef js_sound_play_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	float			pitch;
	char			name[name_str_len];
	d3pnt			pt;
	
	script_value_to_string(argv[0],name,name_str_len);
	pt.x=script_value_to_int(argv[1]);
	pt.z=script_value_to_int(argv[2]);
	pt.y=script_value_to_int(argv[3]);
	pitch=script_value_to_float(argv[4]);

	script_sound_play(name,&pt,pitch,FALSE,FALSE,exception);
	
	return(script_null_to_value());
}

JSValueRef js_sound_play_at_object_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	float			pitch;
	char			name[name_str_len];
	obj_type		*obj;

	obj=script_find_obj_from_uid_arg(argv[1],exception);
	if (obj==NULL) return(FALSE);
	
	script_value_to_string(argv[0],name,name_str_len);
	pitch=script_value_to_float(argv[2]);

	script_sound_play(name,&obj->pnt,pitch,FALSE,FALSE,exception);
	
	return(script_null_to_value());
}

JSValueRef js_sound_play_global_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	float			pitch;
	char			name[name_str_len];
	d3pnt			pt;
	
	script_value_to_string(argv[0],name,name_str_len);
	pitch=script_value_to_float(argv[1]);
	
	pt.x=pt.y=pt.z=0;

	script_sound_play(name,&pt,pitch,TRUE,FALSE,exception);
	
	return(script_null_to_value());
}

JSValueRef js_sound_play_global_player_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	float			pitch;
	char			name[name_str_len];
	d3pnt			pt;
	
	script_value_to_string(argv[0],name,name_str_len);
	pitch=script_value_to_float(argv[1]);
	
	pt.x=pt.y=pt.z=0;

	script_sound_play(name,&pt,pitch,TRUE,TRUE,exception);
	
	return(script_null_to_value());
}

/* =======================================================

      Music Functions
      
======================================================= */

JSValueRef js_sound_start_music_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			name[name_str_len],wave_path[1024];

	if (setup.music_on) {
	
		script_value_to_string(argv[0],name,name_str_len);

		file_paths_data(&setup.file_path_setup,wave_path,"Music",name,"wav");
		if (!al_music_play(name,wave_path)) {
			*exception=js_sound_music_name_exception(name);
		}
	}

	return(script_null_to_value());
}

JSValueRef js_sound_stop_music_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	if (setup.music_on) al_music_stop();

	return(script_null_to_value());
}

JSValueRef js_sound_fade_in_music_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				msec;
	char			name[name_str_len],wave_path[1024];
	
	if (setup.music_on) {

		script_value_to_string(argv[0],name,name_str_len);
		msec=script_value_to_int(argv[1]);

		file_paths_data(&setup.file_path_setup,wave_path,"Music",name,"wav");
		if (!al_music_fade_in(js.time.current_tick,name,wave_path,msec)) {
			*exception=js_sound_music_name_exception(name);
		}
	}

	return(script_null_to_value());
}

JSValueRef js_sound_fade_out_music_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int					msec;
	
	if (setup.music_on) {

		msec=script_value_to_int(argv[0]);
		al_music_fade_out(js.time.current_tick,msec);
	}

	return(script_null_to_value());
}

JSValueRef js_sound_fade_out_fade_in_music_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int					fade_out_msec,fade_in_msec;
	char				name[name_str_len],wave_path[1024];
	
	if (setup.music_on) {

		script_value_to_string(argv[0],name,name_str_len);
		fade_out_msec=script_value_to_int(argv[1]);
		fade_in_msec=script_value_to_int(argv[2]);

		file_paths_data(&setup.file_path_setup,wave_path,"Music",name,"wav");
		if (!al_music_fade_out_fade_in(js.time.current_tick,name,wave_path,fade_out_msec,fade_in_msec)) {
			*exception=js_sound_music_name_exception(name);
		}
	}

	return(script_null_to_value());
}
