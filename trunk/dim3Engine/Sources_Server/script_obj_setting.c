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

JSValueRef js_obj_setting_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_obj_setting_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_obj_setting_get_id(void);
JSValueRef js_obj_setting_get_name(void);
JSValueRef js_obj_setting_get_characterName(void);
JSValueRef js_obj_setting_get_characterModel(void);
JSValueRef js_obj_setting_get_characterParameter(void);
JSValueRef js_obj_setting_get_team(void);
JSValueRef js_obj_setting_get_hidden(void);
JSValueRef js_obj_setting_get_suspend(void);
JSValueRef js_obj_setting_get_fly(void);
JSValueRef js_obj_setting_get_find(void);
JSValueRef js_obj_setting_get_contact(void);
JSValueRef js_obj_setting_get_contactObject(void);
JSValueRef js_obj_setting_get_contactProjectile(void);
JSValueRef js_obj_setting_get_contactForce(void);
JSValueRef js_obj_setting_get_hitBox(void);
JSValueRef js_obj_setting_get_damage(void);
JSValueRef js_obj_setting_get_crushable(void);
JSValueRef js_obj_setting_get_invincible(void);
JSValueRef js_obj_setting_get_clickable(void);
JSValueRef js_obj_setting_get_pickup(void);
JSValueRef js_obj_setting_get_ignorePickUpItems(void);
JSValueRef js_obj_setting_get_ignoreMouse(void);
JSValueRef js_obj_setting_get_turnOnlyWhenMoving(void);
JSValueRef js_obj_setting_get_restrictPlayerTurning(void);
JSValueRef js_obj_setting_get_quickReverse(void);
JSValueRef js_obj_setting_get_sideStep(void);
JSValueRef js_obj_setting_get_jump(void);
JSValueRef js_obj_setting_get_duck(void);
JSValueRef js_obj_setting_get_crawl(void);
JSValueRef js_obj_setting_get_singleSpeed(void);
JSValueRef js_obj_setting_get_bumpUp(void);
JSValueRef js_obj_setting_get_slopeGravity(void);
JSValueRef js_obj_setting_get_pushable(void);
JSValueRef js_obj_setting_get_openDoors(void);
JSValueRef js_obj_setting_get_inputMode(void);
void js_obj_setting_set_team(JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_hidden(JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_suspend(JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_fly(JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_find(JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_contact(JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_contactObject(JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_contactProjectile(JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_contactForce(JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_hitBox(JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_damage(JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_crushable(JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_invincible(JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_clickable(JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_pickup(JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_ignorePickUpItems(JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_ignoreMouse(JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_turnOnlyWhenMoving(JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_restrictPlayerTurning(JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_quickReverse(JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_sideStep(JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_jump(JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_duck(JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_crawl(JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_singleSpeed(JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_bumpUp(JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_slopeGravity(JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_pushable(JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_openDoors(JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_inputMode(JSValueRef vp,JSValueRef *exception);
JSValueRef js_obj_get_parameter_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_set_ambient_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_change_ambient_pitch_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_clear_ambient_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

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

JSObjectRef script_add_obj_setting_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,obj_setting_class,"setting",obj_setting_props,obj_setting_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_obj_setting_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,obj_setting_props));
}

bool js_obj_setting_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,obj_setting_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_obj_setting_get_id(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_int_to_value(obj->uid));
}

JSValueRef js_obj_setting_get_name(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_string_to_value(obj->name));
}

JSValueRef js_obj_setting_get_characterName(void)
{
	obj_type		*obj;

	if (hud.character.ncharacter==0) return(script_null_to_value());

	obj=object_find_uid(js.attach.thing_uid);
	return(script_string_to_value(hud.character.characters[obj->character_idx].name));
}

JSValueRef js_obj_setting_get_characterModel(void)
{
	obj_type		*obj;
	
	if (hud.character.ncharacter==0) return(script_null_to_value());

	obj=object_find_uid(js.attach.thing_uid);
	return(script_string_to_value(hud.character.characters[obj->character_idx].model_name));
}

JSValueRef js_obj_setting_get_characterParameter(void)
{
	obj_type		*obj;

	if (hud.character.ncharacter==0) return(script_null_to_value());

	obj=object_find_uid(js.attach.thing_uid);
	return(script_string_to_value(hud.character.characters[obj->character_idx].param));
}

JSValueRef js_obj_setting_get_team(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_int_to_value(obj->team_idx+sd_team_none));
}

JSValueRef js_obj_setting_get_hidden(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(obj->hidden));
}

JSValueRef js_obj_setting_get_suspend(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(obj->suspend));
}

JSValueRef js_obj_setting_get_fly(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(obj->fly));
}

JSValueRef js_obj_setting_get_find(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(obj->find_on));
}

JSValueRef js_obj_setting_get_contact(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value((obj->contact.object_on) || (obj->contact.projectile_on) || (obj->contact.force_on)));
}

JSValueRef js_obj_setting_get_contactObject(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(obj->contact.object_on));
}

JSValueRef js_obj_setting_get_contactProjectile(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(obj->contact.projectile_on));
}

JSValueRef js_obj_setting_get_contactForce(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(obj->contact.force_on));
}

JSValueRef js_obj_setting_get_hitBox(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(obj->hit_box.on));
}

JSValueRef js_obj_setting_get_damage(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(obj->damage.on));
}

JSValueRef js_obj_setting_get_crushable(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(obj->damage.crushable));
}

JSValueRef js_obj_setting_get_invincible(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(obj->damage.invincible));
}

JSValueRef js_obj_setting_get_clickable(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(obj->click.on));
}

JSValueRef js_obj_setting_get_pickup(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(obj->pickup.on));
}

JSValueRef js_obj_setting_get_ignorePickUpItems(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(obj->pickup.ignore));
}

JSValueRef js_obj_setting_get_ignoreMouse(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(obj->turn.ignore_mouse));
}

JSValueRef js_obj_setting_get_turnOnlyWhenMoving(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(obj->turn.only_when_moving));
}

JSValueRef js_obj_setting_get_restrictPlayerTurning(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(obj->turn.restrict_player_turning));
}

JSValueRef js_obj_setting_get_quickReverse(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(obj->quick_reverse));
}

JSValueRef js_obj_setting_get_sideStep(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(obj->side_step));
}

JSValueRef js_obj_setting_get_jump(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(obj->jump.on));
}

JSValueRef js_obj_setting_get_duck(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(obj->duck.on));
}

JSValueRef js_obj_setting_get_crawl(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(obj->crawl));
}

JSValueRef js_obj_setting_get_singleSpeed(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(obj->single_speed));
}

JSValueRef js_obj_setting_get_bumpUp(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(obj->bump.on));
}

JSValueRef js_obj_setting_get_slopeGravity(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(obj->slope_gravity));
}

JSValueRef js_obj_setting_get_pushable(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(obj->contact.pushable));
}

JSValueRef js_obj_setting_get_openDoors(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(obj->open_doors));
}

JSValueRef js_obj_setting_get_inputMode(void)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_int_to_value(obj->input_mode+sd_input_mode_fpp));
}

/* =======================================================

      Setters
      
======================================================= */

void js_obj_setting_set_team(JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->team_idx=script_value_to_int(vp)-sd_team_none;
}

void js_obj_setting_set_hidden(JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	object_hide(obj,script_value_to_bool(vp));
}

void js_obj_setting_set_suspend(JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->suspend=script_value_to_bool(vp);
}

void js_obj_setting_set_fly(JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->fly=script_value_to_bool(vp);
}

void js_obj_setting_set_find(JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->find_on=script_value_to_bool(vp);
}

void js_obj_setting_set_contact(JSValueRef vp,JSValueRef *exception)
{
	bool			on;
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);

	on=script_value_to_bool(vp);
	obj->contact.object_on=on;
	obj->contact.projectile_on=on;
	obj->contact.force_on=on;
}

void js_obj_setting_set_contactObject(JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->contact.object_on=script_value_to_bool(vp);
}

void js_obj_setting_set_contactProjectile(JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->contact.projectile_on=script_value_to_bool(vp);
}

void js_obj_setting_set_contactForce(JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->contact.force_on=script_value_to_bool(vp);
}

void js_obj_setting_set_hitBox(JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->hit_box.on=script_value_to_bool(vp);
}

void js_obj_setting_set_damage(JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->damage.on=script_value_to_bool(vp);
}

void js_obj_setting_set_crushable(JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->damage.crushable=script_value_to_bool(vp);
}

void js_obj_setting_set_invincible(JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->damage.invincible=script_value_to_bool(vp);
}

void js_obj_setting_set_clickable(JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->click.on=script_value_to_bool(vp);
}

void js_obj_setting_set_pickup(JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->pickup.on=script_value_to_bool(vp);
}

void js_obj_setting_set_ignorePickUpItems(JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->pickup.ignore=script_value_to_bool(vp);
}

void js_obj_setting_set_ignoreMouse(JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->turn.ignore_mouse=script_value_to_bool(vp);
}

void js_obj_setting_set_turnOnlyWhenMoving(JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->turn.only_when_moving=script_value_to_bool(vp);
}

void js_obj_setting_set_restrictPlayerTurning(JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->turn.restrict_player_turning=script_value_to_bool(vp);
}

void js_obj_setting_set_quickReverse(JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->quick_reverse=script_value_to_bool(vp);
}

void js_obj_setting_set_sideStep(JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->side_step=script_value_to_bool(vp);
}

void js_obj_setting_set_jump(JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->jump.on=script_value_to_bool(vp);
}

void js_obj_setting_set_duck(JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->duck.on=script_value_to_bool(vp);
}

void js_obj_setting_set_crawl(JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->crawl=script_value_to_bool(vp);
}

void js_obj_setting_set_singleSpeed(JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->single_speed=script_value_to_bool(vp);
}

void js_obj_setting_set_bumpUp(JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->bump.on=script_value_to_bool(vp);
}

void js_obj_setting_set_slopeGravity(JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->slope_gravity=script_value_to_bool(vp);
}

void js_obj_setting_set_pushable(JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->contact.pushable=script_value_to_bool(vp);
}

void js_obj_setting_set_openDoors(JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->open_doors=script_value_to_bool(vp);
}

void js_obj_setting_set_inputMode(JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->input_mode=script_value_to_int(vp)-sd_input_mode_fpp;
}

/* =======================================================

      Functions
      
======================================================= */

JSValueRef js_obj_get_parameter_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				idx,k;
    char			*c,str[256];
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	
	idx=scripts_find_uid(obj->attach.script_uid);
	if (idx==-1) return(script_null_to_value());
	
    k=script_value_to_int(argv[0]);
    if (k<0) k=0;
    
    c=js.scripts[idx].params;
    while (k!=0) {
        c=strchr(c,'|');
        if (c==NULL) break;
        c++;
        k--;
    }
    
    if (c==NULL) return(script_null_to_value());
	
	strncpy(str,c,256);
	str[255]=0x0;
    
    c=strchr(str,'|');
    if (c!=NULL) *c=0x0;
    
    return(script_string_to_value(str));
}

JSValueRef js_obj_set_ambient_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			name[name_str_len];
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	
	script_value_to_string(argv[0],name,name_str_len);
	object_set_ambient(obj,name,script_value_to_float(argv[1]));

	return(script_null_to_value());
}

JSValueRef js_obj_change_ambient_pitch_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	object_change_ambient_pitch(obj,script_value_to_float(argv[0]));

	return(script_null_to_value());
}

JSValueRef js_obj_clear_ambient_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	object_clear_ambient(obj);

	return(script_null_to_value());
}





