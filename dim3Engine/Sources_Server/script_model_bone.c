/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: model.bone object

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
#include "projectiles.h"
#include "models.h"

extern js_type			js;

JSValueRef js_model_bone_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_model_bone_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_model_bone_find_offset_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_model_bone_find_position_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_model_bone_get_brightness_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

script_js_function	model_bone_functions[]={
							{"findOffset",			js_model_bone_find_offset_func,			2},
							{"findPosition",		js_model_bone_find_position_func,		2},
							{"getBrightness",		js_model_bone_get_brightness_func,		2},
							{0}};

JSClassRef			model_bone_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_model_bone_object(void)
{
	model_bone_class=script_create_class("model_bone_class",js_model_bone_get_property,js_model_bone_set_property);
}

void script_free_model_bone_object(void)
{
	script_free_class(model_bone_class);
}

JSObjectRef script_add_model_bone_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,model_bone_class,"bone",NULL,model_bone_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_model_bone_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,NULL));
}

bool js_model_bone_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,NULL));
}

/* =======================================================

      Setup Draw For Bone Functions
      
======================================================= */

model_draw* script_bone_function_setup(JSValueRef *exception)
{
	obj_type			*obj;
	weapon_type			*weap;
	proj_type			*proj;
	
	switch (js.attach.thing_type) {
	
		case thing_type_object:
			obj=object_find_uid(js.attach.thing_uid);
			model_draw_setup_object(js.time.current_tick,obj);
			return(&obj->draw);
			
		case thing_type_weapon:
			weap=weapon_find_uid(js.attach.thing_uid);
			obj=object_find_uid(weap->obj_uid);
			model_draw_setup_weapon(js.time.current_tick,obj,weap,FALSE,FALSE);
			return(&weap->draw);
			
		case thing_type_projectile:
			proj=projectile_find_uid(js.attach.thing_uid);
			model_draw_setup_projectile(js.time.current_tick,proj);
			return(&proj->draw);
			
		case thing_type_projectile_setup:
			*exception=script_create_exception("There is no model to get bone positions from");
			return(NULL);
			
	}
	
	return(NULL);
}

/* =======================================================

      Bone Exceptions
      
======================================================= */

JSValueRef js_model_bone_name_exception(char *pose_name,char *bone_name)
{
	char			err_str[256];

	sprintf(err_str,"Named pose or bone does not exist: %s,%s",pose_name,bone_name);
	return(script_create_exception(err_str));
}

/* =======================================================

      Model Bone Functions
      
======================================================= */

JSValueRef js_model_bone_find_offset_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char				pose_name[name_str_len],bone_name[name_str_len];
	int					x,y,z;
	model_draw			*draw;
	
		// get proper draw setup
		
	draw=script_bone_function_setup(exception);
	if (draw==NULL) return(script_null_to_value());
	
		// get bone offset
		
	script_value_to_string(argv[0],pose_name,name_str_len);
	script_value_to_string(argv[1],bone_name,name_str_len);
	
	if (!model_find_bone_offset(draw,pose_name,bone_name,&x,&y,&z)) {
		*exception=js_model_bone_name_exception(pose_name,bone_name);
		return(script_null_to_value());
	}
	
	return(script_point_to_value(x,y,z));
}

JSValueRef js_model_bone_find_position_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char				pose_name[name_str_len],bone_name[name_str_len];
	int					x,y,z;
	model_draw			*draw;
	
		// get proper draw setup
		
	draw=script_bone_function_setup(exception);
	if (draw==NULL) return(script_null_to_value());
	
		// get bone position
	
	script_value_to_string(argv[0],pose_name,name_str_len);
	script_value_to_string(argv[1],bone_name,name_str_len);
	
	if (!model_find_bone_position(draw,pose_name,bone_name,&x,&y,&z)) {
		*exception=js_model_bone_name_exception(pose_name,bone_name);
		return(script_null_to_value());
	}
	
	return(script_point_to_value(x,y,z));
}

JSValueRef js_model_bone_get_brightness_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			pose_name[name_str_len],bone_name[name_str_len];
	float			bright;
	model_draw		*draw;
	
		// get proper draw setup
		
	draw=script_bone_function_setup(exception);
	if (draw==NULL) return(script_null_to_value());
	
		// get bone light

	script_value_to_string(argv[0],pose_name,name_str_len);
	script_value_to_string(argv[1],bone_name,name_str_len);
	
	if (!model_get_bone_brightness(draw,pose_name,bone_name,&bright)) {
		*exception=js_model_bone_name_exception(pose_name,bone_name);
		return(script_null_to_value());
	}
	
	return(script_float_to_value(bright));
}

