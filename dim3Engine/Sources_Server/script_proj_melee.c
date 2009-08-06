/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: proj.melee object

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

extern js_type			js;

JSBool js_proj_melee_get_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp);
JSBool js_proj_melee_set_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp);
bool js_proj_melee_get_strikeBoneTag(JSValueRef *vp);
bool js_proj_melee_get_strikePoseName(JSValueRef *vp);
bool js_proj_melee_get_radius(JSValueRef *vp);
bool js_proj_melee_get_distance(JSValueRef *vp);
bool js_proj_melee_get_damage(JSValueRef *vp);
bool js_proj_melee_get_force(JSValueRef *vp);
bool js_proj_melee_get_fallOff(JSValueRef *vp);
bool js_proj_melee_set_strikeBoneTag(JSValueRef *vp);
bool js_proj_melee_set_strikePoseName(JSValueRef *vp);
bool js_proj_melee_set_radius(JSValueRef *vp);
bool js_proj_melee_set_distance(JSValueRef *vp);
bool js_proj_melee_set_damage(JSValueRef *vp);
bool js_proj_melee_set_force(JSValueRef *vp);
bool js_proj_melee_set_fallOff(JSValueRef *vp);

JSBool js_proj_melee_spawn_from_projectile_bone_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);
JSBool js_proj_melee_spawn_from_position_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);

script_js_property	proj_melee_props[]={
							{"strikeBoneTag",			js_proj_melee_get_strikeBoneTag,		js_proj_melee_set_strikeBoneTag},
							{"strikePoseName",			js_proj_melee_get_strikePoseName,		js_proj_melee_set_strikePoseName},
							{"radius",					js_proj_melee_get_radius,				js_proj_melee_set_radius},
							{"distance",				js_proj_melee_get_distance,				js_proj_melee_set_distance},
							{"damage",					js_proj_melee_get_damage,				js_proj_melee_set_damage},
							{"force",					js_proj_melee_get_force,				js_proj_melee_set_force},
							{"fallOff",					js_proj_melee_get_fallOff,				js_proj_melee_set_fallOff},
							{0}};

script_js_function	proj_melee_functions[]={
							{"spawnFromProjectileBone",	js_proj_melee_spawn_from_projectile_bone_func,	0},
							{"spawnFromPosition",		js_proj_melee_spawn_from_position_func,			3},
							{0}};

JSClassRef			proj_melee_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_proj_melee_object(void)
{
	proj_melee_class=script_create_class("proj_melee_class",js_proj_melee_get_property,js_proj_melee_set_property);
}

void script_free_proj_melee_object(void)
{
	script_free_class(proj_melee_class);
}

JSObject* script_add_proj_melee_object(JSObject *parent_obj)
{
	return(script_create_child_object(parent_obj,proj_melee_class,"melee",proj_melee_props,proj_melee_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_proj_melee_get_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp)
{
	return(script_get_property(cx,j_obj,id,vp,proj_melee_props));
}

JSBool js_proj_melee_set_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp)
{
	return(script_set_property(cx,j_obj,id,vp,proj_melee_props));
}

/* =======================================================

      Getters
      
======================================================= */

bool js_proj_melee_get_strikeBoneTag(JSValueRef *vp)
{
	char				str[32];
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
	model_tag_to_text(proj_setup->melee.strike_bone_tag,str);
	*vp=script_string_to_value(str);
	
	return(TRUE);
}

bool js_proj_melee_get_strikePoseName(JSValueRef *vp)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
	*vp=script_string_to_value(proj_setup->melee.strike_pose_name);
	
	return(TRUE);
}

bool js_proj_melee_get_radius(JSValueRef *vp)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
	*vp=script_int_to_value(proj_setup->melee.radius);
	
	return(TRUE);
}

bool js_proj_melee_get_distance(JSValueRef *vp)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
	*vp=script_int_to_value(proj_setup->melee.distance);
	
	return(TRUE);
}

bool js_proj_melee_get_damage(JSValueRef *vp)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
	*vp=script_int_to_value(proj_setup->melee.damage);
	
	return(TRUE);
}

bool js_proj_melee_get_force(JSValueRef *vp)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
	*vp=script_int_to_value(proj_setup->melee.force);
	
	return(TRUE);
}

bool js_proj_melee_get_fallOff(JSValueRef *vp)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
	*vp=script_bool_to_value(proj_setup->melee.fall_off);
	
	return(TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

bool js_proj_melee_set_strikeBoneTag(JSValueRef *vp)
{
	char				str[32];
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
	script_value_to_string(*vp,str,32);
	proj_setup->melee.strike_bone_tag=text_to_model_tag(str);
	
	return(TRUE);
}

bool js_proj_melee_set_strikePoseName(JSValueRef *vp)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
	script_value_to_string(*vp,proj_setup->melee.strike_pose_name,name_str_len);
	
	return(TRUE);
}

bool js_proj_melee_set_radius(JSValueRef *vp)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
	proj_setup->melee.radius=script_value_to_int(*vp);
	
	return(TRUE);
}

bool js_proj_melee_set_distance(JSValueRef *vp)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
	proj_setup->melee.distance=script_value_to_int(*vp);
	
	return(TRUE);
}

bool js_proj_melee_set_damage(JSValueRef *vp)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
	proj_setup->melee.damage=script_value_to_int(*vp);
	
	return(TRUE);
}

bool js_proj_melee_set_force(JSValueRef *vp)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
	proj_setup->melee.force=script_value_to_int(*vp);
	
	return(TRUE);
}

bool js_proj_melee_set_fallOff(JSValueRef *vp)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
	proj_setup->melee.fall_off=script_value_to_bool(*vp);
	
	return(TRUE);
}

/* =======================================================

      Spawn Melee
      
======================================================= */

JSBool js_proj_melee_spawn_from_projectile_bone_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	char				err_str[256];
    obj_type			*obj;
	weapon_type			*weap;
	proj_type			*proj;
	proj_setup_type		*proj_setup;

	proj=proj_get_attach();
	if (proj==NULL) return(TRUE);

	proj_setup=proj_setups_find_uid(proj->proj_setup_uid);
	if (proj_setup==NULL) return(TRUE);
	
    obj=object_find_uid(proj_setup->obj_uid);
	weap=weapon_find_uid(proj_setup->weap_uid);

	if (!melee_script_spawn_projectile_model(js.time.current_tick,obj,weap,proj_setup,proj,err_str)) {
		JS_ReportError(js.cx,err_str);
		return(FALSE);
	}

	return(TRUE);
}

JSBool js_proj_melee_spawn_from_position_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	d3pnt				pt;
    obj_type			*obj;
	weapon_type			*weap;
	proj_type			*proj;
	proj_setup_type		*proj_setup;

	proj=proj_get_attach();
	if (proj==NULL) return(TRUE);

	proj_setup=proj_setups_find_uid(proj->proj_setup_uid);
	if (proj_setup==NULL) return(TRUE);
	
    obj=object_find_uid(proj_setup->obj_uid);
	weap=weapon_find_uid(proj_setup->weap_uid);
	
	pt.x=script_value_to_int(argv[0]);
	pt.z=script_value_to_int(argv[1]);
	pt.y=script_value_to_int(argv[2]);
	
	melee_add(obj,weap,&pt,&proj->ang,&proj_setup->melee,-1);

	return(TRUE);
}

