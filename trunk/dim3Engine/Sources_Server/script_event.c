/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: *.event object

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
	#include "dim3engine.h"
#endif

#include "scripts.h"
#include "objects.h"
#include "weapons.h"
#include "projectiles.h"

extern server_type		server;
extern js_type			js;


JSValueRef js_event_start_timer_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_event_clear_timer_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_event_start_wait_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_event_start_wait_random_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_event_clear_wait_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_event_chain_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_event_clear_chain_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_event_send_message_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_event_send_message_to_player_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_event_send_message_to_object_by_id_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_event_send_message_to_object_by_name_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_event_send_message_to_course_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_event_send_message_to_game_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_event_send_message_to_held_weapon_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_event_send_message_to_spawn_weapon_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_event_set_message_data_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_event_get_message_data_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_event_call_object_by_id_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_event_call_course_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_event_call_game_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticFunction	event_functions[]={
							{"startTimer",					js_event_start_timer_func,						kJSPropertyAttributeDontDelete},
							{"clearTimer",					js_event_clear_timer_func,						kJSPropertyAttributeDontDelete},
							{"startWait",					js_event_start_wait_func,						kJSPropertyAttributeDontDelete},
							{"startWaitRandom",				js_event_start_wait_random_func,				kJSPropertyAttributeDontDelete},
							{"clearWait",					js_event_clear_wait_func,						kJSPropertyAttributeDontDelete},
							{"chain",						js_event_chain_func,							kJSPropertyAttributeDontDelete},
							{"clearChain",					js_event_clear_chain_func,						kJSPropertyAttributeDontDelete},
							{"sendMessage",					js_event_send_message_func,						kJSPropertyAttributeDontDelete},
							{"sendMessageToPlayer",			js_event_send_message_to_player_func,			kJSPropertyAttributeDontDelete},
							{"sendMessageToObjectById",		js_event_send_message_to_object_by_id_func,		kJSPropertyAttributeDontDelete},
							{"sendMessageToObjectByName",	js_event_send_message_to_object_by_name_func,	kJSPropertyAttributeDontDelete},
							{"sendMessageToCourse",			js_event_send_message_to_course_func,			kJSPropertyAttributeDontDelete},
							{"sendMessageToGame",			js_event_send_message_to_game_func,				kJSPropertyAttributeDontDelete},
							{"sendMessageToHeldWeapon",		js_event_send_message_to_held_weapon_func,		kJSPropertyAttributeDontDelete},
							{"sendMessageToSpawnWeapon",	js_event_send_message_to_spawn_weapon_func,		kJSPropertyAttributeDontDelete},
							{"setMessageData",				js_event_set_message_data_func,					kJSPropertyAttributeDontDelete},
							{"getMessageData",				js_event_get_message_data_func,					kJSPropertyAttributeDontDelete},
							{"callObjectById",				js_event_call_object_by_id_func,				kJSPropertyAttributeDontDelete},
							{"callCourse",					js_event_call_course_func,						kJSPropertyAttributeDontDelete},
							{"callGame",					js_event_call_game_func,						kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			event_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_event_object(void)
{
	event_class=script_create_class("event_class",NULL,event_functions);
}

void script_free_event_object(void)
{
	script_free_class(event_class);
}

JSObjectRef script_add_event_object(JSContextRef cx,JSObjectRef parent_obj)
{
	return(script_create_child_object(cx,parent_obj,event_class,"event"));
}

/* =======================================================

      Start/Clear Timer Functions
      
======================================================= */

JSValueRef js_event_start_timer_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			err_str[256];

	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));

	if (!timers_add(&js.attach,script_value_to_int(cx,argv[0]),script_value_to_int(cx,argv[1]),NULL,timer_mode_repeat,err_str)) {
		*exception=script_create_exception(cx,err_str);
		return(script_bool_to_value(cx,FALSE));
	}

	return(script_bool_to_value(cx,TRUE));
}

JSValueRef js_event_clear_timer_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	timers_clear(&js.attach,timer_mode_repeat);
    return(script_null_to_value(cx));
}

/* =======================================================

      Start/Clear Wait Functions
      
======================================================= */

JSValueRef js_event_start_wait_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			err_str[256];

	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	if (!timers_add(&js.attach,script_value_to_int(cx,argv[0]),script_value_to_int(cx,argv[1]),NULL,timer_mode_single,err_str)) {
		*exception=script_create_exception(cx,err_str);
		return(script_bool_to_value(cx,FALSE));
	}

	return(script_bool_to_value(cx,TRUE));
}

JSValueRef js_event_start_wait_random_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				min,max,tick;
	char			err_str[256];
	
	if (!script_check_param_count(cx,func,argc,3,exception)) return(script_null_to_value(cx));
	
	min=script_value_to_int(cx,argv[0]);
	max=script_value_to_int(cx,argv[1]);
	tick=random_int(abs(max-min))+min;
	
	if (!timers_add(&js.attach,tick,script_value_to_int(cx,argv[2]),NULL,timer_mode_single,err_str)) {
		*exception=script_create_exception(cx,err_str);
		return(script_bool_to_value(cx,FALSE));
	}

	return(script_bool_to_value(cx,TRUE));
}

JSValueRef js_event_clear_wait_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	timers_clear(&js.attach,timer_mode_single);
    return(script_null_to_value(cx));
}

/* =======================================================

      Chain Functions
      
======================================================= */

JSValueRef js_event_chain_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			chain_func_name[64],err_str[256];
	
	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	script_value_to_string(cx,argv[1],chain_func_name,64);
	
	if (!timers_add(&js.attach,script_value_to_int(cx,argv[0]),0,chain_func_name,timer_mode_chain,err_str)) {
		*exception=script_create_exception(cx,err_str);
		return(script_bool_to_value(cx,FALSE));
	}

	return(script_bool_to_value(cx,TRUE));
}

JSValueRef js_event_clear_chain_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	if (!script_check_param_count(cx,func,argc,0,exception)) return(script_null_to_value(cx));
	
	timers_clear(&js.attach,timer_mode_chain);
    return(script_null_to_value(cx));
}

/* =======================================================

      Send Message Functions
      
======================================================= */

JSValueRef js_event_send_message_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				msg_to,id;
	char			name[name_str_len];
	obj_type		*obj;
	JSValueRef		vp;

	if (!script_check_param_count(cx,func,argc,3,exception)) return(script_null_to_value(cx));
	
	msg_to=script_value_to_int(cx,argv[0]);
	id=script_value_to_int(cx,argv[2]);
	
	vp=script_bool_to_value(cx,TRUE);
	
	switch (msg_to) {
	
		case sd_message_to_player:
			obj=server.obj_list.objs[server.player_obj_idx];
			memmove(&obj->attach.msg_data.get,&js.attach.msg_data.set,(sizeof(attach_msg_type)*max_attach_msg_data));
			scripts_post_event_console(&obj->attach,sd_event_message,sd_event_message_from_script,id);
			break;
	
		case sd_message_to_object:
			script_value_to_string(cx,argv[1],name,name_str_len);
			obj=object_find_name(name);
			if (obj==NULL) {
				vp=script_bool_to_value(cx,FALSE);
				break;
			}

			memmove(&obj->attach.msg_data.get,&js.attach.msg_data.set,(sizeof(attach_msg_type)*max_attach_msg_data));
			scripts_post_event_console(&obj->attach,sd_event_message,sd_event_message_from_script,id);
			break;
			
		case sd_message_to_course:
			memmove(&js.course_attach.msg_data.get,&js.attach.msg_data.set,(sizeof(attach_msg_type)*max_attach_msg_data));
			scripts_post_event_console(&js.course_attach,sd_event_message,sd_event_message_from_script,id);
			break;
		
		case sd_message_to_game:
			memmove(&js.game_attach.msg_data.get,&js.attach.msg_data.set,(sizeof(attach_msg_type)*max_attach_msg_data));
			scripts_post_event_console(&js.game_attach,sd_event_message,sd_event_message_from_script,id);
			break;
	
	}

	return(vp);
}

JSValueRef js_event_send_message_to_player_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	obj=server.obj_list.objs[server.player_obj_idx];
	memmove(&obj->attach.msg_data.get,&js.attach.msg_data.set,(sizeof(attach_msg_type)*max_attach_msg_data));
	scripts_post_event_console(&obj->attach,sd_event_message,sd_event_message_from_script,script_value_to_int(cx,argv[0]));
	return(script_null_to_value(cx));
}

JSValueRef js_event_send_message_to_object_by_id_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;

	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	obj=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (obj==NULL) return(script_bool_to_value(cx,FALSE));

	memmove(&obj->attach.msg_data.get,&js.attach.msg_data.set,(sizeof(attach_msg_type)*max_attach_msg_data));
	scripts_post_event_console(&obj->attach,sd_event_message,sd_event_message_from_script,script_value_to_int(cx,argv[1]));
	return(script_bool_to_value(cx,TRUE));
}

JSValueRef js_event_send_message_to_object_by_name_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			name[name_str_len];
	obj_type		*obj;

	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
	script_value_to_string(cx,argv[0],name,name_str_len);
	obj=object_find_name(name);
	if (obj==NULL) return(script_bool_to_value(cx,FALSE));

	memmove(&obj->attach.msg_data.get,&js.attach.msg_data.set,(sizeof(attach_msg_type)*max_attach_msg_data));
	scripts_post_event_console(&obj->attach,sd_event_message,sd_event_message_from_script,script_value_to_int(cx,argv[1]));
	return(script_bool_to_value(cx,TRUE));
}

JSValueRef js_event_send_message_to_course_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	memmove(&js.course_attach.msg_data.get,&js.attach.msg_data.set,(sizeof(attach_msg_type)*max_attach_msg_data));
	scripts_post_event_console(&js.course_attach,sd_event_message,sd_event_message_from_script,script_value_to_int(cx,argv[0]));

	return(script_null_to_value(cx));
}

JSValueRef js_event_send_message_to_game_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	memmove(&js.game_attach.msg_data.get,&js.attach.msg_data.set,(sizeof(attach_msg_type)*max_attach_msg_data));
	scripts_post_event_console(&js.game_attach,sd_event_message,sd_event_message_from_script,script_value_to_int(cx,argv[0]));

	return(script_null_to_value(cx));
}

JSValueRef js_event_send_message_to_held_weapon_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	weapon_type		*weap;

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	if (js.attach.thing_type!=thing_type_object) {
		*exception=script_create_exception(cx,"Not an object script");
		return(script_null_to_value(cx));
	}

	obj=object_script_lookup();
	weap=weapon_find_current(obj);
	if (weap==NULL) {
		*exception=script_create_exception(cx,"No held weapon");
		return(script_null_to_value(cx));
	}

	memmove(&weap->attach.msg_data.get,&js.attach.msg_data.set,(sizeof(attach_msg_type)*max_attach_msg_data));
	scripts_post_event_console(&weap->attach,sd_event_message,sd_event_message_from_script,script_value_to_int(cx,argv[0]));

	return(script_null_to_value(cx));
}

JSValueRef js_event_send_message_to_spawn_weapon_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	proj_type		*proj;
	weapon_type		*weap;

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
	if (js.attach.thing_type!=thing_type_projectile) {
		*exception=script_create_exception(cx,"Not an projectile script");
		return(script_null_to_value(cx));
	}

	proj=projectile_script_lookup();
	if (!proj->on) {
		*exception=script_create_exception(cx,"Could not find projectile");
		return(script_null_to_value(cx));
	}

	weap=weapon_script_lookup();
	if (weap==NULL) {
		*exception=script_create_exception(cx,"Could not find weapon");
		return(script_null_to_value(cx));
	}

	memmove(&weap->attach.msg_data.get,&js.attach.msg_data.set,(sizeof(attach_msg_type)*max_attach_msg_data));
	scripts_post_event_console(&weap->attach,sd_event_message,sd_event_message_from_script,script_value_to_int(cx,argv[0]));
	return(script_null_to_value(cx));
}

/* =======================================================

      Message Data
      
======================================================= */

JSValueRef js_event_set_message_data_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				idx,v_type;
	script_type		*script;

	if (!script_check_param_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));

	script=js.script_list.scripts[js.attach.script_idx];
	
		// get index

	idx=script_value_to_int(cx,argv[0]);
	if ((idx<0) || (idx>=max_attach_msg_data)) {
		*exception=script_create_exception(cx,"Message data index out of bounds");
		return(script_null_to_value(cx));
	}

		// set data

	v_type=JSValueGetType(cx,argv[1]);

	if (v_type==kJSTypeNumber) {
		js.attach.msg_data.set[idx].type=d3_jsval_type_number;
		js.attach.msg_data.set[idx].data.d3_number=script_value_to_float(cx,argv[1]);
		return(script_null_to_value(cx));
	}
	
	if (v_type==kJSTypeBoolean) {
		js.attach.msg_data.set[idx].type=d3_jsval_type_boolean;
		js.attach.msg_data.set[idx].data.d3_boolean=script_value_to_bool(cx,argv[1]);
		return(script_null_to_value(cx));
	}
	
	js.attach.msg_data.set[idx].type=d3_jsval_type_string;
	script_value_to_string(cx,argv[1],js.attach.msg_data.set[idx].data.d3_string,max_d3_jsval_str_len);	
    return(script_null_to_value(cx));
}

JSValueRef js_event_get_message_data_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				idx;
	script_type		*script;
	JSValueRef		vp;

	if (!script_check_param_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));

	script=js.script_list.scripts[js.attach.script_idx];
	
 		// get index

	idx=script_value_to_int(cx,argv[0]);
	if ((idx<0) || (idx>=max_attach_msg_data)) {
		*exception=script_create_exception(cx,"Message data index out of bounds");
		return(script_null_to_value(cx));
	}

		// get data

	vp=script_null_to_value(cx);

	switch (js.attach.msg_data.get[idx].type) {

		case d3_jsval_type_number:
			vp=script_float_to_value(cx,js.attach.msg_data.get[idx].data.d3_number);
			break;

		case d3_jsval_type_boolean:
			vp=script_bool_to_value(cx,js.attach.msg_data.get[idx].data.d3_boolean);
			break;

		case d3_jsval_type_string:
			vp=script_string_to_value(cx,js.attach.msg_data.get[idx].data.d3_string);
			break;

	}

	return(vp);
}

/* =======================================================

      Direct Function Call
      
======================================================= */

JSValueRef js_event_call_object_by_id_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				n,arg_count;
	char			func_name[64],err_str[256];
	JSValueRef		rval,args[20];
	obj_type		*obj;

	if (!script_check_param_at_least_count(cx,func,argc,2,exception)) return(script_null_to_value(cx));
	
		// get arguments

	obj=script_find_obj_from_uid_arg(cx,argv[0],exception);
	if (obj==NULL) return(script_null_to_value(cx));

	script_value_to_string(cx,argv[1],func_name,64);

	arg_count=argc-2;
	if (arg_count<0) arg_count=0;
	if (arg_count>20) arg_count=20;

	for (n=0;n!=arg_count;n++) {
		args[n]=argv[n+2];
	}

		// call function

	rval=scripts_direct_call(&obj->attach,func_name,arg_count,args,err_str);
	if (rval==NULL) {
		*exception=script_create_exception(cx,err_str);
		return(script_null_to_value(cx));
	}
	
	return(rval);
}

JSValueRef js_event_call_course_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				n,arg_count;
	char			func_name[64],err_str[256];
	JSValueRef		rval,args[20];

	if (!script_check_param_at_least_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
		// get arguments

	script_value_to_string(cx,argv[0],func_name,64);

	arg_count=argc-1;
	if (arg_count<0) arg_count=0;
	if (arg_count>20) arg_count=20;

	for (n=0;n!=arg_count;n++) {
		args[n]=argv[n+1];
	}

		// call function

	rval=scripts_direct_call(&js.course_attach,func_name,arg_count,args,err_str);
	if (rval==NULL) {
		*exception=script_create_exception(cx,err_str);
		return(script_null_to_value(cx));
	}
	
	return(rval);
}

JSValueRef js_event_call_game_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_obj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				n,arg_count;
	char			func_name[64],err_str[256];
	JSValueRef		rval,args[20];

	if (!script_check_param_at_least_count(cx,func,argc,1,exception)) return(script_null_to_value(cx));
	
		// get arguments

	script_value_to_string(cx,argv[0],func_name,64);

	arg_count=argc-1;
	if (arg_count<0) arg_count=0;
	if (arg_count>20) arg_count=20;

	for (n=0;n!=arg_count;n++) {
		args[n]=argv[n+1];
	}

		// call function

	rval=scripts_direct_call(&js.game_attach,func_name,arg_count,args,err_str);
	if (rval==NULL) {
		*exception=script_create_exception(cx,err_str);
		return(script_null_to_value(cx));
	}
	
	return(rval);
}
