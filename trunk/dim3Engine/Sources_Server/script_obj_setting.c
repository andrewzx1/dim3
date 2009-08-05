/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.setting object

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

extern js_type			js;
extern hud_type			hud;

JSBool js_obj_setting_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
JSBool js_obj_setting_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);
bool js_obj_setting_get_id(jsval *vp);
bool js_obj_setting_get_name(jsval *vp);
bool js_obj_setting_get_characterName(jsval *vp);
bool js_obj_setting_get_characterModel(jsval *vp);
bool js_obj_setting_get_characterParameter(jsval *vp);
bool js_obj_setting_get_team(jsval *vp);
bool js_obj_setting_get_hidden(jsval *vp);
bool js_obj_setting_get_suspend(jsval *vp);
bool js_obj_setting_get_fly(jsval *vp);
bool js_obj_setting_get_find(jsval *vp);
bool js_obj_setting_get_contact(jsval *vp);
bool js_obj_setting_get_contactObject(jsval *vp);
bool js_obj_setting_get_contactProjectile(jsval *vp);
bool js_obj_setting_get_contactForce(jsval *vp);
bool js_obj_setting_get_hitBox(jsval *vp);
bool js_obj_setting_get_damage(jsval *vp);
bool js_obj_setting_get_crushable(jsval *vp);
bool js_obj_setting_get_invincible(jsval *vp);
bool js_obj_setting_get_clickable(jsval *vp);
bool js_obj_setting_get_pickup(jsval *vp);
bool js_obj_setting_get_ignorePickUpItems(jsval *vp);
bool js_obj_setting_get_ignoreMouse(jsval *vp);
bool js_obj_setting_get_turnOnlyWhenMoving(jsval *vp);
bool js_obj_setting_get_restrictPlayerTurning(jsval *vp);
bool js_obj_setting_get_quickReverse(jsval *vp);
bool js_obj_setting_get_sideStep(jsval *vp);
bool js_obj_setting_get_jump(jsval *vp);
bool js_obj_setting_get_duck(jsval *vp);
bool js_obj_setting_get_crawl(jsval *vp);
bool js_obj_setting_get_singleSpeed(jsval *vp);
bool js_obj_setting_get_bumpUp(jsval *vp);
bool js_obj_setting_get_slopeGravity(jsval *vp);
bool js_obj_setting_get_pushable(jsval *vp);
bool js_obj_setting_get_openDoors(jsval *vp);
bool js_obj_setting_get_inputMode(jsval *vp);
bool js_obj_setting_set_team(jsval *vp);
bool js_obj_setting_set_hidden(jsval *vp);
bool js_obj_setting_set_suspend(jsval *vp);
bool js_obj_setting_set_fly(jsval *vp);
bool js_obj_setting_set_find(jsval *vp);
bool js_obj_setting_set_contact(jsval *vp);
bool js_obj_setting_set_contactObject(jsval *vp);
bool js_obj_setting_set_contactProjectile(jsval *vp);
bool js_obj_setting_set_contactForce(jsval *vp);
bool js_obj_setting_set_hitBox(jsval *vp);
bool js_obj_setting_set_damage(jsval *vp);
bool js_obj_setting_set_crushable(jsval *vp);
bool js_obj_setting_set_invincible(jsval *vp);
bool js_obj_setting_set_clickable(jsval *vp);
bool js_obj_setting_set_pickup(jsval *vp);
bool js_obj_setting_set_ignorePickUpItems(jsval *vp);
bool js_obj_setting_set_ignoreMouse(jsval *vp);
bool js_obj_setting_set_turnOnlyWhenMoving(jsval *vp);
bool js_obj_setting_set_restrictPlayerTurning(jsval *vp);
bool js_obj_setting_set_quickReverse(jsval *vp);
bool js_obj_setting_set_sideStep(jsval *vp);
bool js_obj_setting_set_jump(jsval *vp);
bool js_obj_setting_set_duck(jsval *vp);
bool js_obj_setting_set_crawl(jsval *vp);
bool js_obj_setting_set_singleSpeed(jsval *vp);
bool js_obj_setting_set_bumpUp(jsval *vp);
bool js_obj_setting_set_slopeGravity(jsval *vp);
bool js_obj_setting_set_pushable(jsval *vp);
bool js_obj_setting_set_openDoors(jsval *vp);
bool js_obj_setting_set_inputMode(jsval *vp);
JSBool js_obj_get_parameter_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_set_ambient_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_change_ambient_pitch_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_obj_clear_ambient_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

script_js_property	obj_setting_props[]={
							{"id",						js_obj_setting_get_id,						NULL},
							{"name",					js_obj_setting_get_name,					NULL},
							{"characterName",			js_obj_setting_get_characterName,			NULL},
							{"characterModel",			js_obj_setting_get_characterModel,			NULL},
							{"characterParameter",		js_obj_setting_get_characterParameter,		NULL},
							{"team",					js_obj_setting_get_team,					js_obj_setting_set_team},
							{"hidden",					js_obj_setting_get_hidden,					js_obj_setting_set_hidden},
							{"suspend",					js_obj_setting_get_suspend,					js_obj_setting_set_suspend},
							{"fly",						js_obj_setting_get_fly,						js_obj_setting_set_fly},
							{"find",					js_obj_setting_get_find,					js_obj_setting_set_find},
							{"contact",					js_obj_setting_get_contact,					js_obj_setting_set_contact},
							{"contactObject",			js_obj_setting_get_contactObject,			js_obj_setting_set_contactObject},
							{"contactProjectile",		js_obj_setting_get_contactProjectile,		js_obj_setting_set_contactProjectile},
							{"contactForce",			js_obj_setting_get_contactForce,			js_obj_setting_set_contactForce},
							{"hitBox",					js_obj_setting_get_hitBox,					js_obj_setting_set_hitBox},
							{"damage",					js_obj_setting_get_damage,					js_obj_setting_set_damage},
							{"crushable",				js_obj_setting_get_crushable,				js_obj_setting_set_crushable},
							{"invincible",				js_obj_setting_get_invincible,				js_obj_setting_set_invincible},
							{"clickable",				js_obj_setting_get_clickable,				js_obj_setting_set_clickable},
							{"pickUp",					js_obj_setting_get_pickup,					js_obj_setting_set_pickup},
							{"ignorePickUpItems",		js_obj_setting_get_ignorePickUpItems,		js_obj_setting_set_ignorePickUpItems},
							{"ignoreMouse",				js_obj_setting_get_ignoreMouse,				js_obj_setting_set_ignoreMouse},
							{"turnOnlyWhenMoving",		js_obj_setting_get_turnOnlyWhenMoving,		js_obj_setting_set_turnOnlyWhenMoving},
							{"restrictPlayerTurning",	js_obj_setting_get_restrictPlayerTurning,	js_obj_setting_set_restrictPlayerTurning},
							{"quickReverse",			js_obj_setting_get_quickReverse,			js_obj_setting_set_quickReverse},
							{"sideStep",				js_obj_setting_get_sideStep,				js_obj_setting_set_sideStep},
							{"jump",					js_obj_setting_get_jump,					js_obj_setting_set_jump},
							{"duck",					js_obj_setting_get_duck,					js_obj_setting_set_duck},
							{"crawl",					js_obj_setting_get_crawl,					js_obj_setting_set_crawl},
							{"singleSpeed",				js_obj_setting_get_singleSpeed,				js_obj_setting_set_singleSpeed},
							{"bumpUp",					js_obj_setting_get_bumpUp,					js_obj_setting_set_bumpUp},
							{"slopeGravity",			js_obj_setting_get_slopeGravity,			js_obj_setting_set_slopeGravity},
							{"pushable",				js_obj_setting_get_pushable,				js_obj_setting_set_pushable},
							{"openDoors",				js_obj_setting_get_openDoors,				js_obj_setting_set_openDoors},
							{"inputMode",				js_obj_setting_get_inputMode,				js_obj_setting_set_inputMode},
							{0}};

script_js_function	obj_setting_functions[]={
							{"getParameter",			js_obj_get_parameter_func,					1},
							{"setAmbient",				js_obj_set_ambient_func,					2},
							{"changeAmbientPitch",		js_obj_change_ambient_pitch_func,			1},
							{"clearAmbient",			js_obj_clear_ambient_func,					0},
							{0}};

JSClassRef			obj_setting_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_setting_object(void)
{
	obj_setting_class=script_create_class("obj_setting_class",js_obj_setting_get_property,js_obj_setting_set_property);
}

void script_free_obj_setting_object(void)
{
	script_free_class(obj_setting_class);
}

JSObject* script_add_obj_setting_object(JSObject *parent_obj)
{
	return(script_create_child_object(parent_obj,obj_setting_class,"setting",obj_setting_props,obj_setting_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_obj_setting_get_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_get_property(cx,j_obj,id,vp,obj_setting_props));
}

JSBool js_obj_setting_set_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	return(script_set_property(cx,j_obj,id,vp,obj_setting_props));
}

/* =======================================================

      Getters
      
======================================================= */

bool js_obj_setting_get_id(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(obj->uid);
	
	return(JS_TRUE);
}

bool js_obj_setting_get_name(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_string_to_value(obj->name);
	
	return(JS_TRUE);
}

bool js_obj_setting_get_characterName(jsval *vp)
{
	obj_type		*obj;

	if (hud.character.ncharacter==0) {
		*vp=script_null_to_value();
	}
	else {
		obj=object_find_uid(js.attach.thing_uid);
		*vp=script_string_to_value(hud.character.characters[obj->character_idx].name);
	}

	return(JS_TRUE);
}

bool js_obj_setting_get_characterModel(jsval *vp)
{
	obj_type		*obj;

	if (hud.character.ncharacter==0) {
		*vp=script_null_to_value();
	}
	else {
		obj=object_find_uid(js.attach.thing_uid);
		*vp=script_string_to_value(hud.character.characters[obj->character_idx].model_name);
	}

	return(JS_TRUE);
}

bool js_obj_setting_get_characterParameter(jsval *vp)
{
	obj_type		*obj;

	if (hud.character.ncharacter==0) {
		*vp=script_null_to_value();
	}
	else {
		obj=object_find_uid(js.attach.thing_uid);
		*vp=script_string_to_value(hud.character.characters[obj->character_idx].param);
	}

	return(JS_TRUE);
}

bool js_obj_setting_get_team(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(obj->team_idx+sd_team_none);
	
	return(JS_TRUE);
}

bool js_obj_setting_get_hidden(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(obj->hidden);
	
	return(JS_TRUE);
}

bool js_obj_setting_get_suspend(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(obj->suspend);
	
	return(JS_TRUE);
}

bool js_obj_setting_get_fly(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(obj->fly);
	
	return(JS_TRUE);
}

bool js_obj_setting_get_find(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(obj->find_on);
	
	return(JS_TRUE);
}

bool js_obj_setting_get_contact(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value((obj->contact.object_on) || (obj->contact.projectile_on) || (obj->contact.force_on));
	
	return(JS_TRUE);
}

bool js_obj_setting_get_contactObject(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(obj->contact.object_on);
	
	return(JS_TRUE);
}

bool js_obj_setting_get_contactProjectile(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(obj->contact.projectile_on);
	
	return(JS_TRUE);
}

bool js_obj_setting_get_contactForce(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(obj->contact.force_on);
	
	return(JS_TRUE);
}

bool js_obj_setting_get_hitBox(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(obj->hit_box.on);
	
	return(JS_TRUE);
}

bool js_obj_setting_get_damage(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(obj->damage.on);
	
	return(JS_TRUE);
}

bool js_obj_setting_get_crushable(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(obj->damage.crushable);
	
	return(JS_TRUE);
}

bool js_obj_setting_get_invincible(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(obj->damage.invincible);
	
	return(JS_TRUE);
}

bool js_obj_setting_get_clickable(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(obj->click.on);
	
	return(JS_TRUE);
}

bool js_obj_setting_get_pickup(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(obj->pickup.on);
	
	return(JS_TRUE);
}

bool js_obj_setting_get_ignorePickUpItems(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(obj->pickup.ignore);
	
	return(JS_TRUE);
}

bool js_obj_setting_get_ignoreMouse(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(obj->turn.ignore_mouse);
	
	return(JS_TRUE);
}

bool js_obj_setting_get_turnOnlyWhenMoving(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(obj->turn.only_when_moving);
	
	return(JS_TRUE);
}

bool js_obj_setting_get_restrictPlayerTurning(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(obj->turn.restrict_player_turning);
	
	return(JS_TRUE);
}

bool js_obj_setting_get_quickReverse(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(obj->quick_reverse);
	
	return(JS_TRUE);
}

bool js_obj_setting_get_sideStep(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(obj->side_step);
	
	return(JS_TRUE);
}

bool js_obj_setting_get_jump(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(obj->jump.on);
	
	return(JS_TRUE);
}

bool js_obj_setting_get_duck(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(obj->duck.on);
	
	return(JS_TRUE);
}

bool js_obj_setting_get_crawl(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(obj->crawl);
	
	return(JS_TRUE);
}

bool js_obj_setting_get_singleSpeed(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(obj->single_speed);
	
	return(JS_TRUE);
}

bool js_obj_setting_get_bumpUp(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(obj->bump.on);
	
	return(JS_TRUE);
}

bool js_obj_setting_get_slopeGravity(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(obj->slope_gravity);
	
	return(JS_TRUE);
}

bool js_obj_setting_get_pushable(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(obj->contact.pushable);
	
	return(JS_TRUE);
}

bool js_obj_setting_get_openDoors(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_bool_to_value(obj->open_doors);
	
	return(JS_TRUE);
}

bool js_obj_setting_get_inputMode(jsval *vp)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	*vp=script_int_to_value(obj->input_mode+sd_input_mode_fpp);
	
	return(JS_TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

bool js_obj_setting_set_team(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->team_idx=script_value_to_int(*vp)-sd_team_none;

	return(JS_TRUE);
}

bool js_obj_setting_set_hidden(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	object_hide(obj,script_value_to_bool(*vp));
	
	return(JS_TRUE);
}

bool js_obj_setting_set_suspend(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->suspend=script_value_to_bool(*vp);
	
	return(JS_TRUE);
}

bool js_obj_setting_set_fly(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->fly=script_value_to_bool(*vp);
	
	return(JS_TRUE);
}

bool js_obj_setting_set_find(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->find_on=script_value_to_bool(*vp);
	
	return(JS_TRUE);
}

bool js_obj_setting_set_contact(jsval *vp)
{
	bool			on;
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);

	on=script_value_to_bool(*vp);
	obj->contact.object_on=on;
	obj->contact.projectile_on=on;
	obj->contact.force_on=on;
	
	return(JS_TRUE);
}

bool js_obj_setting_set_contactObject(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->contact.object_on=script_value_to_bool(*vp);
	
	return(JS_TRUE);
}

bool js_obj_setting_set_contactProjectile(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->contact.projectile_on=script_value_to_bool(*vp);
	
	return(JS_TRUE);
}

bool js_obj_setting_set_contactForce(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->contact.force_on=script_value_to_bool(*vp);
	
	return(JS_TRUE);
}

bool js_obj_setting_set_hitBox(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->hit_box.on=script_value_to_bool(*vp);
	
	return(JS_TRUE);
}

bool js_obj_setting_set_damage(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->damage.on=script_value_to_bool(*vp);
	
	return(JS_TRUE);
}

bool js_obj_setting_set_crushable(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->damage.crushable=script_value_to_bool(*vp);
	
	return(JS_TRUE);
}

bool js_obj_setting_set_invincible(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->damage.invincible=script_value_to_bool(*vp);
	
	return(JS_TRUE);
}

bool js_obj_setting_set_clickable(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->click.on=script_value_to_bool(*vp);
	
	return(JS_TRUE);
}

bool js_obj_setting_set_pickup(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->pickup.on=script_value_to_bool(*vp);
	
	return(JS_TRUE);
}

bool js_obj_setting_set_ignorePickUpItems(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->pickup.ignore=script_value_to_bool(*vp);
	
	return(JS_TRUE);
}

bool js_obj_setting_set_ignoreMouse(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->turn.ignore_mouse=script_value_to_bool(*vp);
	
	return(JS_TRUE);
}

bool js_obj_setting_set_turnOnlyWhenMoving(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->turn.only_when_moving=script_value_to_bool(*vp);
	
	return(JS_TRUE);
}

bool js_obj_setting_set_restrictPlayerTurning(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->turn.restrict_player_turning=script_value_to_bool(*vp);
	
	return(JS_TRUE);
}

bool js_obj_setting_set_quickReverse(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->quick_reverse=script_value_to_bool(*vp);
	
	return(JS_TRUE);
}

bool js_obj_setting_set_sideStep(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->side_step=script_value_to_bool(*vp);
	
	return(JS_TRUE);
}

bool js_obj_setting_set_jump(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->jump.on=script_value_to_bool(*vp);
	
	return(JS_TRUE);
}

bool js_obj_setting_set_duck(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->duck.on=script_value_to_bool(*vp);
	
	return(JS_TRUE);
}

bool js_obj_setting_set_crawl(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->crawl=script_value_to_bool(*vp);
	
	return(JS_TRUE);
}

bool js_obj_setting_set_singleSpeed(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->single_speed=script_value_to_bool(*vp);
	
	return(JS_TRUE);
}

bool js_obj_setting_set_bumpUp(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->bump.on=script_value_to_bool(*vp);
	
	return(JS_TRUE);
}

bool js_obj_setting_set_slopeGravity(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->slope_gravity=script_value_to_bool(*vp);
	
	return(JS_TRUE);
}

bool js_obj_setting_set_pushable(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->contact.pushable=script_value_to_bool(*vp);
	
	return(JS_TRUE);
}

bool js_obj_setting_set_openDoors(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->open_doors=script_value_to_bool(*vp);
	
	return(JS_TRUE);
}

bool js_obj_setting_set_inputMode(jsval *vp)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->input_mode=script_value_to_int(*vp)-sd_input_mode_fpp;
	
	return(JS_TRUE);
}

/* =======================================================

      Functions
      
======================================================= */

JSBool js_obj_get_parameter_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				idx,k;
    char			*c,str[256];
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	
	idx=scripts_find_uid(obj->attach.script_uid);
	if (idx==-1) {
		*rval=script_null_to_value();
		return(JS_TRUE);
    }
	
    k=script_value_to_int(argv[0]);
    if (k<0) k=0;
    
    c=js.scripts[idx].params;
    while (k!=0) {
        c=strchr(c,'|');
        if (c==NULL) break;
        c++;
        k--;
    }
    
    if (c==NULL) {
        *rval=script_null_to_value();
        return(JS_TRUE);
    }
	
	strncpy(str,c,256);
	str[255]=0x0;
    
    c=strchr(str,'|');
    if (c!=NULL) *c=0x0;
    
    *rval=script_string_to_value(str);

	return(JS_TRUE);
}

JSBool js_obj_set_ambient_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	char			name[name_str_len];
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	
	script_value_to_string(argv[0],name,name_str_len);
	object_set_ambient(obj,name,script_value_to_float(argv[1]));

	return(JS_TRUE);
}

JSBool js_obj_change_ambient_pitch_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	
	object_change_ambient_pitch(obj,script_value_to_float(argv[0]));

	return(JS_TRUE);
}

JSBool js_obj_clear_ambient_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	
	object_clear_ambient(obj);

	return(JS_TRUE);
}





