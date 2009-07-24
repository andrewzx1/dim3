/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script Object Creation

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

extern js_type			js;

/* =======================================================

      Initialize/Release Classes
      
======================================================= */

void script_initialize_classes(void)
{
	script_init_global_map_object();
	script_init_global_multiplayer_object();
	script_init_global_camera_object();
	script_init_global_interface_object();
	script_init_global_data_object();
	script_init_global_sound_object();
	script_init_global_spawn_object();
	script_init_global_utility_object();

	script_init_game_setting_object();
	script_init_game_score_object();
	script_init_game_join_object();

	script_init_model_object();

	script_init_obj_setting_object();
	script_init_obj_position_object();
	script_init_obj_angle_object();
	script_init_obj_lock_object();
	script_init_obj_status_object();
	script_init_obj_motion_vector_object();
	script_init_obj_motion_angle_object();
	script_init_obj_forward_speed_object();
	script_init_obj_side_speed_object();
	script_init_obj_vert_speed_object();
	script_init_obj_turn_speed_object();
	script_init_obj_object_speed_object();
	script_init_obj_thrust_object();
	script_init_obj_look_object();
	script_init_obj_sight_object();
	script_init_obj_health_object();
	script_init_obj_size_object();
	script_init_obj_rigid_body_object();
	script_init_obj_vehicle_object();
	script_init_obj_radar_object();
	script_init_obj_score_object();
	script_init_obj_touch_object();
	script_init_obj_touch_position_object();
	script_init_obj_touch_angle_object();
	script_init_obj_hit_object();
	script_init_obj_hit_position_object();
	script_init_obj_hit_angle_object();
	script_init_obj_click_object();
	script_init_obj_held_object();
	script_init_obj_pickup_object();
	script_init_obj_watch_object();
	script_init_obj_weapon_object();
	script_init_obj_weapon_fire_object();
	script_init_obj_melee_object();

	script_init_weap_setting_object();
	script_init_weap_hand_object();
	script_init_weap_hand_position_object();
	script_init_weap_hand_angle_object();
	script_init_weap_projectile_object();
	script_init_weap_crosshair_object();
	script_init_weap_ammo_object();
	script_init_weap_alt_ammo_object();
	script_init_weap_kickback_object();
	script_init_weap_recoil_object();
	script_init_weap_melee_object();
	script_init_weap_fire_object();
	script_init_weap_dual_object();
	script_init_weap_target_object();
	script_init_weap_zoom_object();

	script_init_proj_setting_object();
	script_init_proj_position_object();
	script_init_proj_origin_object();
	script_init_proj_motion_vector_object();
	script_init_proj_motion_angle_object();
	script_init_proj_speed_object();
	script_init_proj_hit_object();
	script_init_proj_action_object();
	script_init_proj_push_object();
	script_init_proj_size_object();
	script_init_proj_mark_object();
	script_init_proj_melee_object();

	script_init_event_object();
}

void script_release_classes(void)
{
	script_free_global_map_object();
	script_free_global_multiplayer_object();
	script_free_global_camera_object();
	script_free_global_interface_object();
	script_free_global_data_object();
	script_free_global_sound_object();
	script_free_global_spawn_object();
	script_free_global_utility_object();

	script_free_game_setting_object();
	script_free_game_score_object();
	script_free_game_join_object();

	script_free_model_object();

	script_free_obj_setting_object();
	script_free_obj_position_object();
	script_free_obj_angle_object();
	script_free_obj_lock_object();
	script_free_obj_status_object();
	script_free_obj_motion_vector_object();
	script_free_obj_motion_angle_object();
	script_free_obj_forward_speed_object();
	script_free_obj_side_speed_object();
	script_free_obj_vert_speed_object();
	script_free_obj_turn_speed_object();
	script_free_obj_object_speed_object();
	script_free_obj_thrust_object();
	script_free_obj_look_object();
	script_free_obj_sight_object();
	script_free_obj_health_object();
	script_free_obj_size_object();
	script_free_obj_rigid_body_object();
	script_free_obj_vehicle_object();
	script_free_obj_radar_object();
	script_free_obj_score_object();
	script_free_obj_touch_object();
	script_free_obj_touch_position_object();
	script_free_obj_touch_angle_object();
	script_free_obj_hit_object();
	script_free_obj_hit_position_object();
	script_free_obj_hit_angle_object();
	script_free_obj_click_object();
	script_free_obj_held_object();
	script_free_obj_pickup_object();
	script_free_obj_watch_object();
	script_free_obj_weapon_object();
	script_free_obj_weapon_fire_object();
	script_free_obj_melee_object();

	script_free_weap_setting_object();
	script_free_weap_hand_object();
	script_free_weap_hand_position_object();
	script_free_weap_hand_angle_object();
	script_free_weap_projectile_object();
	script_free_weap_crosshair_object();
	script_free_weap_ammo_object();
	script_free_weap_alt_ammo_object();
	script_free_weap_kickback_object();
	script_free_weap_recoil_object();
	script_free_weap_melee_object();
	script_free_weap_fire_object();
	script_free_weap_dual_object();
	script_free_weap_target_object();
	script_free_weap_zoom_object();

	script_free_proj_setting_object();
	script_free_proj_position_object();
	script_free_proj_origin_object();
	script_free_proj_motion_vector_object();
	script_free_proj_motion_angle_object();
	script_free_proj_speed_object();
	script_free_proj_hit_object();
	script_free_proj_action_object();
	script_free_proj_push_object();
	script_free_proj_size_object();
	script_free_proj_mark_object();
	script_free_proj_melee_object();

	script_free_event_object();
}

/* =======================================================

      Add Global Objects to Script
      
======================================================= */

bool script_add_global_object(script_type *script,char *err_str)
{
		// must remove other globals to stop parenting problems
		
	JS_SetGlobalObject(js.cx,NULL);
	
		// create global for this script
	
	script->global=JS_NewObject(js.cx,NULL,NULL,NULL);
	if (script->global==NULL) {
		strcpy(err_str,"Not enough memory to create global object");
		return(FALSE);
	}

		// set sub objects

	script_add_global_map_object(script->global);
	script_add_global_multiplayer_object(script->global);
	script_add_global_camera_object(script->global);
	script_add_global_interface_object(script->global);
	script_add_global_data_object(script->global);
	script_add_global_sound_object(script->global);
	script_add_global_spawn_object(script->global);
	script_add_global_utility_object(script->global);
	
		// set global

	JS_SetGlobalObject(js.cx,script->global);

		// initialize standard classes

	if (!JS_InitStandardClasses(js.cx,script->global)) {
		strcpy(err_str,"Could not initialize standard classes");
		return(FALSE);
	}

	return(TRUE);
}

/* =======================================================

      Create dim3 Script Classes
      
======================================================= */

JSClass* script_create_class(const char *name,JSPropertyOp getter,JSPropertyOp setter)
{
	JSClass			*cls;

	cls=(JSClass*)malloc(sizeof(JSClass));
	bzero(cls,sizeof(JSClass));

	cls->name=name;
	
	cls->addProperty=JS_PropertyStub;
	cls->delProperty=JS_PropertyStub;
	cls->getProperty=getter;
	cls->setProperty=setter;
	cls->enumerate=JS_EnumerateStub;
	cls->resolve=JS_ResolveStub;
	cls->convert=JS_ConvertStub;
	cls->finalize=JS_FinalizeStub;

	return(cls);
}

void script_free_class(JSClass *cls)
{
	free(cls);
}

/* =======================================================

      Create dim3 Script Objects
      
======================================================= */

JSObject* script_create_main_object(attach_type *attach)
{
	JSObject		*j_obj;

	j_obj=JS_NewObject(js.cx,NULL,NULL,NULL);
	if (j_obj==NULL) return(NULL);

		// add sub-objects

	switch (attach->thing_type) {

		case thing_type_game:
			script_add_game_setting_object(j_obj);
			script_add_event_object(j_obj);
			script_add_game_score_object(j_obj);
			script_add_game_join_object(j_obj);
			break;

		case thing_type_course:
			script_add_event_object(j_obj);
			break;

		case thing_type_object:
			script_add_obj_setting_object(j_obj);
			script_add_event_object(j_obj);
			script_add_obj_position_object(j_obj);
			script_add_obj_angle_object(j_obj);
			script_add_obj_lock_object(j_obj);
			script_add_obj_status_object(j_obj);
			script_add_obj_motion_vector_object(j_obj);
			script_add_obj_motion_angle_object(j_obj);
			script_add_obj_forward_speed_object(j_obj);
			script_add_obj_side_speed_object(j_obj);
			script_add_obj_vert_speed_object(j_obj);
			script_add_obj_turn_speed_object(j_obj);
			script_add_obj_object_speed_object(j_obj);
			script_add_obj_thrust_object(j_obj);
			script_add_obj_look_object(j_obj);
			script_add_obj_sight_object(j_obj);
			script_add_obj_health_object(j_obj);
			script_add_obj_size_object(j_obj);
			script_add_obj_rigid_body_object(j_obj);
			script_add_obj_vehicle_object(j_obj);
			script_add_obj_radar_object(j_obj);
			script_add_obj_score_object(j_obj);
			script_add_obj_touch_object(j_obj);
			script_add_obj_touch_position_object(j_obj);
			script_add_obj_touch_angle_object(j_obj);
			script_add_obj_hit_object(j_obj);
			script_add_obj_hit_position_object(j_obj);
			script_add_obj_hit_angle_object(j_obj);
			script_add_obj_click_object(j_obj);
			script_add_obj_held_object(j_obj);
			script_add_obj_pickup_object(j_obj);
			script_add_obj_watch_object(j_obj);
			script_add_obj_weapon_object(j_obj);
			script_add_obj_weapon_fire_object(j_obj);
			script_add_obj_melee_object(j_obj);
			script_add_model_object(j_obj);
			break;

		case thing_type_weapon:
			script_add_weap_setting_object(j_obj);
			script_add_event_object(j_obj);
			script_add_weap_hand_object(j_obj);
			script_add_weap_hand_position_object(j_obj);
			script_add_weap_hand_angle_object(j_obj);
			script_add_weap_projectile_object(j_obj);
			script_add_weap_crosshair_object(j_obj);
			script_add_weap_kickback_object(j_obj);
			script_add_weap_recoil_object(j_obj);
			script_add_weap_ammo_object(j_obj);
			script_add_weap_alt_ammo_object(j_obj);
			script_add_weap_melee_object(j_obj);
			script_add_weap_fire_object(j_obj);
			script_add_weap_dual_object(j_obj);
			script_add_weap_target_object(j_obj);
			script_add_weap_zoom_object(j_obj);
			script_add_model_object(j_obj);
			break;

		case thing_type_projectile:
		case thing_type_projectile_setup:
			script_add_proj_setting_object(j_obj);
			script_add_event_object(j_obj);
			script_add_proj_position_object(j_obj);
			script_add_proj_origin_object(j_obj);
			script_add_proj_motion_vector_object(j_obj);
			script_add_proj_motion_angle_object(j_obj);
			script_add_proj_speed_object(j_obj);
			script_add_proj_hit_object(j_obj);
			script_add_proj_action_object(j_obj);
			script_add_proj_push_object(j_obj);
			script_add_proj_size_object(j_obj);
			script_add_proj_mark_object(j_obj);
			script_add_proj_melee_object(j_obj);
			script_add_model_object(j_obj);
			break;

	}
	
	return(j_obj);
}

JSObject* script_create_child_object(JSObject *parent_obj,char *name,script_js_property *props,script_js_function *funcs)
{
	int					flags;
	script_js_property	*prop;
	script_js_function	*func;
	JSObject			*j_obj;

		// object

	j_obj=JS_DefineObject(js.cx,parent_obj,name,NULL,NULL,0);

		// properties

	if (props!=NULL) {

		prop=props;

		while (prop->name!=NULL) {

			flags=JSPROP_PERMANENT|JSPROP_SHARED;
			if (prop->setter==NULL) flags|=JSPROP_READONLY;

			JS_DefineProperty(js.cx,j_obj,prop->name,JSVAL_NULL,prop->getter,prop->setter,flags);

			prop++;
		}
	}

		// functions

	if (funcs!=NULL) {

		func=funcs;

		while (func->name!=NULL) {
			JS_DefineFunction(js.cx,j_obj,func->name,func->call,func->nargs,0);
			func++;
		}
	}

	return(j_obj);
}

// supergumba -- replace original with this
JSObject* script_create_child_object_2(JSObject *parent_obj,JSClass *cls,char *name,script_js_property *props,script_js_function *funcs)
{
	int					flags;
	script_js_property	*prop;
	script_js_function	*func;
	JSObject			*j_obj;

		// object

	j_obj=JS_DefineObject(js.cx,parent_obj,name,cls,NULL,0);

		// properties

	if (props!=NULL) {

		prop=props;

		while (prop->name!=NULL) {

			flags=JSPROP_PERMANENT|JSPROP_SHARED;
			if (prop->setter==NULL) flags|=JSPROP_READONLY;

			JS_DefineProperty(js.cx,j_obj,prop->name,JSVAL_NULL,prop->getter,prop->setter,flags);

			prop++;
		}
	}

		// functions

	if (funcs!=NULL) {

		func=funcs;

		while (func->name!=NULL) {
			JS_DefineFunction(js.cx,j_obj,func->name,func->call,func->nargs,0);
			func++;
		}
	}

	return(j_obj);
}

/* =======================================================

      Object Setters/Getters
      
======================================================= */

inline int script_find_object_property_index(jsval id,script_js_property *props)
{
	int					idx;
	char				name[64];

	if (!JSVAL_IS_STRING(id)) return(-1);

	script_value_to_string(id,name,64);

	idx=0;

	while (props[idx].name!=NULL) {
		fprintf(stdout,"%s = %s\n",props[idx].name,name);
		if (strcmp(props[idx].name,name)==0) return(idx);
		idx++;
	}

	return(-1);
}

JSBool script_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp,script_js_property *props)
{
	int			idx;

	idx=script_find_object_property_index(id,props);
	if (idx==-1) return(JS_TRUE);


	return(JS_TRUE);
}

JSBool script_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp,script_js_property *props)
{
	int			idx;

	idx=script_find_object_property_index(id,props);
	if (idx==-1) return(JS_TRUE);


	return(JS_TRUE);
}
