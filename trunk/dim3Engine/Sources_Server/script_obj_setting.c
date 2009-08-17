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
JSValueRef js_obj_setting_get_id(JSContextRef cx);
JSValueRef js_obj_setting_get_name(JSContextRef cx);
JSValueRef js_obj_setting_get_characterName(JSContextRef cx);
JSValueRef js_obj_setting_get_characterModel(JSContextRef cx);
JSValueRef js_obj_setting_get_characterParameter(JSContextRef cx);
JSValueRef js_obj_setting_get_team(JSContextRef cx);
JSValueRef js_obj_setting_get_hidden(JSContextRef cx);
JSValueRef js_obj_setting_get_suspend(JSContextRef cx);
JSValueRef js_obj_setting_get_fly(JSContextRef cx);
JSValueRef js_obj_setting_get_find(JSContextRef cx);
JSValueRef js_obj_setting_get_contact(JSContextRef cx);
JSValueRef js_obj_setting_get_contactObject(JSContextRef cx);
JSValueRef js_obj_setting_get_contactProjectile(JSContextRef cx);
JSValueRef js_obj_setting_get_contactForce(JSContextRef cx);
JSValueRef js_obj_setting_get_hitBox(JSContextRef cx);
JSValueRef js_obj_setting_get_damage(JSContextRef cx);
JSValueRef js_obj_setting_get_crushable(JSContextRef cx);
JSValueRef js_obj_setting_get_invincible(JSContextRef cx);
JSValueRef js_obj_setting_get_clickable(JSContextRef cx);
JSValueRef js_obj_setting_get_pickup(JSContextRef cx);
JSValueRef js_obj_setting_get_ignorePickUpItems(JSContextRef cx);
JSValueRef js_obj_setting_get_ignoreMouse(JSContextRef cx);
JSValueRef js_obj_setting_get_turnOnlyWhenMoving(JSContextRef cx);
JSValueRef js_obj_setting_get_restrictPlayerTurning(JSContextRef cx);
JSValueRef js_obj_setting_get_quickReverse(JSContextRef cx);
JSValueRef js_obj_setting_get_sideStep(JSContextRef cx);
JSValueRef js_obj_setting_get_jump(JSContextRef cx);
JSValueRef js_obj_setting_get_duck(JSContextRef cx);
JSValueRef js_obj_setting_get_crawl(JSContextRef cx);
JSValueRef js_obj_setting_get_singleSpeed(JSContextRef cx);
JSValueRef js_obj_setting_get_bumpUp(JSContextRef cx);
JSValueRef js_obj_setting_get_slopeGravity(JSContextRef cx);
JSValueRef js_obj_setting_get_pushable(JSContextRef cx);
JSValueRef js_obj_setting_get_openDoors(JSContextRef cx);
JSValueRef js_obj_setting_get_inputMode(JSContextRef cx);
void js_obj_setting_set_team(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_hidden(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_suspend(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_fly(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_find(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_contact(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_contactObject(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_contactProjectile(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_contactForce(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_hitBox(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_damage(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_crushable(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_invincible(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_clickable(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_pickup(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_ignorePickUpItems(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_ignoreMouse(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_turnOnlyWhenMoving(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_restrictPlayerTurning(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_quickReverse(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_sideStep(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_jump(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_duck(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_crawl(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_singleSpeed(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_bumpUp(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_slopeGravity(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_pushable(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_openDoors(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
void js_obj_setting_set_inputMode(JSContextRef cx,JSValueRef vp,JSValueRef *exception);
JSValueRef js_obj_get_parameter_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_set_ambient_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_change_ambient_pitch_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_obj_clear_ambient_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticValue 		obj_setting_props[]={
							{"id",						js_obj_setting_get_id,						NULL,										kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"name",					js_obj_setting_get_name,					NULL,										kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"characterName",			js_obj_setting_get_characterName,			NULL,										kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"characterModel",			js_obj_setting_get_characterModel,			NULL,										kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"characterParameter",		js_obj_setting_get_characterParameter,		NULL,										kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete},
							{"team",					js_obj_setting_get_team,					js_obj_setting_set_team,					kJSPropertyAttributeDontDelete},
							{"hidden",					js_obj_setting_get_hidden,					js_obj_setting_set_hidden,					kJSPropertyAttributeDontDelete},
							{"suspend",					js_obj_setting_get_suspend,					js_obj_setting_set_suspend,					kJSPropertyAttributeDontDelete},
							{"fly",						js_obj_setting_get_fly,						js_obj_setting_set_fly,						kJSPropertyAttributeDontDelete},
							{"find",					js_obj_setting_get_find,					js_obj_setting_set_find,					kJSPropertyAttributeDontDelete},
							{"contact",					js_obj_setting_get_contact,					js_obj_setting_set_contact,					kJSPropertyAttributeDontDelete},
							{"contactObject",			js_obj_setting_get_contactObject,			js_obj_setting_set_contactObject,			kJSPropertyAttributeDontDelete},
							{"contactProjectile",		js_obj_setting_get_contactProjectile,		js_obj_setting_set_contactProjectile,		kJSPropertyAttributeDontDelete},
							{"contactForce",			js_obj_setting_get_contactForce,			js_obj_setting_set_contactForce,			kJSPropertyAttributeDontDelete},
							{"hitBox",					js_obj_setting_get_hitBox,					js_obj_setting_set_hitBox,					kJSPropertyAttributeDontDelete},
							{"damage",					js_obj_setting_get_damage,					js_obj_setting_set_damage,					kJSPropertyAttributeDontDelete},
							{"crushable",				js_obj_setting_get_crushable,				js_obj_setting_set_crushable,				kJSPropertyAttributeDontDelete},
							{"invincible",				js_obj_setting_get_invincible,				js_obj_setting_set_invincible,				kJSPropertyAttributeDontDelete},
							{"clickable",				js_obj_setting_get_clickable,				js_obj_setting_set_clickable,				kJSPropertyAttributeDontDelete},
							{"pickUp",					js_obj_setting_get_pickup,					js_obj_setting_set_pickup,					kJSPropertyAttributeDontDelete},
							{"ignorePickUpItems",		js_obj_setting_get_ignorePickUpItems,		js_obj_setting_set_ignorePickUpItems,		kJSPropertyAttributeDontDelete},
							{"ignoreMouse",				js_obj_setting_get_ignoreMouse,				js_obj_setting_set_ignoreMouse,				kJSPropertyAttributeDontDelete},
							{"turnOnlyWhenMoving",		js_obj_setting_get_turnOnlyWhenMoving,		js_obj_setting_set_turnOnlyWhenMoving,		kJSPropertyAttributeDontDelete},
							{"restrictPlayerTurning",	js_obj_setting_get_restrictPlayerTurning,	js_obj_setting_set_restrictPlayerTurning,	kJSPropertyAttributeDontDelete},
							{"quickReverse",			js_obj_setting_get_quickReverse,			js_obj_setting_set_quickReverse,			kJSPropertyAttributeDontDelete},
							{"sideStep",				js_obj_setting_get_sideStep,				js_obj_setting_set_sideStep,				kJSPropertyAttributeDontDelete},
							{"jump",					js_obj_setting_get_jump,					js_obj_setting_set_jump,					kJSPropertyAttributeDontDelete},
							{"duck",					js_obj_setting_get_duck,					js_obj_setting_set_duck,					kJSPropertyAttributeDontDelete},
							{"crawl",					js_obj_setting_get_crawl,					js_obj_setting_set_crawl,					kJSPropertyAttributeDontDelete},
							{"singleSpeed",				js_obj_setting_get_singleSpeed,				js_obj_setting_set_singleSpeed,				kJSPropertyAttributeDontDelete},
							{"bumpUp",					js_obj_setting_get_bumpUp,					js_obj_setting_set_bumpUp,					kJSPropertyAttributeDontDelete},
							{"slopeGravity",			js_obj_setting_get_slopeGravity,			js_obj_setting_set_slopeGravity,			kJSPropertyAttributeDontDelete},
							{"pushable",				js_obj_setting_get_pushable,				js_obj_setting_set_pushable,				kJSPropertyAttributeDontDelete},
							{"openDoors",				js_obj_setting_get_openDoors,				js_obj_setting_set_openDoors,				kJSPropertyAttributeDontDelete},
							{"inputMode",				js_obj_setting_get_inputMode,				js_obj_setting_set_inputMode,				kJSPropertyAttributeDontDelete},
							{0,0,0,0}};

JSStaticFunction	obj_setting_functions[]={
							{"getParameter",			js_obj_get_parameter_func,					kJSPropertyAttributeDontDelete},
							{"setAmbient",				js_obj_set_ambient_func,					kJSPropertyAttributeDontDelete},
							{"changeAmbientPitch",		js_obj_change_ambient_pitch_func,			kJSPropertyAttributeDontDelete},
							{"clearAmbient",			js_obj_clear_ambient_func,					kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			obj_setting_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_setting_object(void)
{
	obj_setting_class=script_create_class("obj_setting_class",obj_setting_props,obj_setting_functions);
}

void script_free_obj_setting_object(void)
{
	script_free_class(obj_setting_class);
}

JSObjectRef script_add_obj_setting_object(JSContextRef cx,JSObjectRef parent_obj)
{
	return(script_create_child_object(cx,parent_obj,obj_setting_class,"setting",obj_setting_props,obj_setting_functions));
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

JSValueRef js_obj_setting_get_id(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_int_to_value(cx,obj->uid));
}

JSValueRef js_obj_setting_get_name(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_string_to_value(cx,obj->name));
}

JSValueRef js_obj_setting_get_characterName(JSContextRef cx)
{
	obj_type		*obj;

	if (hud.character.ncharacter==0) return(script_null_to_value(cx));

	obj=object_find_uid(js.attach.thing_uid);
	return(script_string_to_value(cx,hud.character.characters[obj->character_idx].name));
}

JSValueRef js_obj_setting_get_characterModel(JSContextRef cx)
{
	obj_type		*obj;
	
	if (hud.character.ncharacter==0) return(script_null_to_value(cx));

	obj=object_find_uid(js.attach.thing_uid);
	return(script_string_to_value(cx,hud.character.characters[obj->character_idx].model_name));
}

JSValueRef js_obj_setting_get_characterParameter(JSContextRef cx)
{
	obj_type		*obj;

	if (hud.character.ncharacter==0) return(script_null_to_value(cx));

	obj=object_find_uid(js.attach.thing_uid);
	return(script_string_to_value(cx,hud.character.characters[obj->character_idx].param));
}

JSValueRef js_obj_setting_get_team(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_int_to_value(cx,obj->team_idx+sd_team_none));
}

JSValueRef js_obj_setting_get_hidden(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(cx,obj->hidden));
}

JSValueRef js_obj_setting_get_suspend(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(cx,obj->suspend));
}

JSValueRef js_obj_setting_get_fly(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(cx,obj->fly));
}

JSValueRef js_obj_setting_get_find(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(cx,obj->find_on));
}

JSValueRef js_obj_setting_get_contact(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(cx,(obj->contact.object_on) || (obj->contact.projectile_on) || (obj->contact.force_on)));
}

JSValueRef js_obj_setting_get_contactObject(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(cx,obj->contact.object_on));
}

JSValueRef js_obj_setting_get_contactProjectile(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(cx,obj->contact.projectile_on));
}

JSValueRef js_obj_setting_get_contactForce(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(cx,obj->contact.force_on));
}

JSValueRef js_obj_setting_get_hitBox(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(cx,obj->hit_box.on));
}

JSValueRef js_obj_setting_get_damage(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(cx,obj->damage.on));
}

JSValueRef js_obj_setting_get_crushable(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(cx,obj->damage.crushable));
}

JSValueRef js_obj_setting_get_invincible(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(cx,obj->damage.invincible));
}

JSValueRef js_obj_setting_get_clickable(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(cx,obj->click.on));
}

JSValueRef js_obj_setting_get_pickup(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(cx,obj->pickup.on));
}

JSValueRef js_obj_setting_get_ignorePickUpItems(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(cx,obj->pickup.ignore));
}

JSValueRef js_obj_setting_get_ignoreMouse(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(cx,obj->turn.ignore_mouse));
}

JSValueRef js_obj_setting_get_turnOnlyWhenMoving(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(cx,obj->turn.only_when_moving));
}

JSValueRef js_obj_setting_get_restrictPlayerTurning(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(cx,obj->turn.restrict_player_turning));
}

JSValueRef js_obj_setting_get_quickReverse(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(cx,obj->quick_reverse));
}

JSValueRef js_obj_setting_get_sideStep(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(cx,obj->side_step));
}

JSValueRef js_obj_setting_get_jump(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(cx,obj->jump.on));
}

JSValueRef js_obj_setting_get_duck(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(cx,obj->duck.on));
}

JSValueRef js_obj_setting_get_crawl(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(cx,obj->crawl));
}

JSValueRef js_obj_setting_get_singleSpeed(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(cx,obj->single_speed));
}

JSValueRef js_obj_setting_get_bumpUp(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(cx,obj->bump.on));
}

JSValueRef js_obj_setting_get_slopeGravity(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(cx,obj->slope_gravity));
}

JSValueRef js_obj_setting_get_pushable(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(cx,obj->contact.pushable));
}

JSValueRef js_obj_setting_get_openDoors(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_bool_to_value(cx,obj->open_doors));
}

JSValueRef js_obj_setting_get_inputMode(JSContextRef cx)
{
	obj_type		*obj;

	obj=object_find_uid(js.attach.thing_uid);
	return(script_int_to_value(cx,obj->input_mode+sd_input_mode_fpp));
}

/* =======================================================

      Setters
      
======================================================= */

void js_obj_setting_set_team(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->team_idx=script_value_to_int(cx,vp)-sd_team_none;
}

void js_obj_setting_set_hidden(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	object_hide(obj,script_value_to_bool(cx,vp));
}

void js_obj_setting_set_suspend(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->suspend=script_value_to_bool(cx,vp);
}

void js_obj_setting_set_fly(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->fly=script_value_to_bool(cx,vp);
}

void js_obj_setting_set_find(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->find_on=script_value_to_bool(cx,vp);
}

void js_obj_setting_set_contact(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	bool			on;
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);

	on=script_value_to_bool(cx,vp);
	obj->contact.object_on=on;
	obj->contact.projectile_on=on;
	obj->contact.force_on=on;
}

void js_obj_setting_set_contactObject(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->contact.object_on=script_value_to_bool(cx,vp);
}

void js_obj_setting_set_contactProjectile(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->contact.projectile_on=script_value_to_bool(cx,vp);
}

void js_obj_setting_set_contactForce(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->contact.force_on=script_value_to_bool(cx,vp);
}

void js_obj_setting_set_hitBox(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->hit_box.on=script_value_to_bool(cx,vp);
}

void js_obj_setting_set_damage(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->damage.on=script_value_to_bool(cx,vp);
}

void js_obj_setting_set_crushable(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->damage.crushable=script_value_to_bool(cx,vp);
}

void js_obj_setting_set_invincible(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->damage.invincible=script_value_to_bool(cx,vp);
}

void js_obj_setting_set_clickable(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->click.on=script_value_to_bool(cx,vp);
}

void js_obj_setting_set_pickup(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->pickup.on=script_value_to_bool(cx,vp);
}

void js_obj_setting_set_ignorePickUpItems(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->pickup.ignore=script_value_to_bool(cx,vp);
}

void js_obj_setting_set_ignoreMouse(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->turn.ignore_mouse=script_value_to_bool(cx,vp);
}

void js_obj_setting_set_turnOnlyWhenMoving(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->turn.only_when_moving=script_value_to_bool(cx,vp);
}

void js_obj_setting_set_restrictPlayerTurning(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->turn.restrict_player_turning=script_value_to_bool(cx,vp);
}

void js_obj_setting_set_quickReverse(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->quick_reverse=script_value_to_bool(cx,vp);
}

void js_obj_setting_set_sideStep(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->side_step=script_value_to_bool(cx,vp);
}

void js_obj_setting_set_jump(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->jump.on=script_value_to_bool(cx,vp);
}

void js_obj_setting_set_duck(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->duck.on=script_value_to_bool(cx,vp);
}

void js_obj_setting_set_crawl(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->crawl=script_value_to_bool(cx,vp);
}

void js_obj_setting_set_singleSpeed(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->single_speed=script_value_to_bool(cx,vp);
}

void js_obj_setting_set_bumpUp(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->bump.on=script_value_to_bool(cx,vp);
}

void js_obj_setting_set_slopeGravity(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->slope_gravity=script_value_to_bool(cx,vp);
}

void js_obj_setting_set_pushable(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->contact.pushable=script_value_to_bool(cx,vp);
}

void js_obj_setting_set_openDoors(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->open_doors=script_value_to_bool(cx,vp);
}

void js_obj_setting_set_inputMode(JSContextRef cx,JSValueRef vp,JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	obj->input_mode=script_value_to_int(cx,vp)-sd_input_mode_fpp;
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
	if (idx==-1) return(script_null_to_value(cx));
	
    k=script_value_to_int(cx,argv[0]);
    if (k<0) k=0;
    
    c=js.scripts[idx].params;
    while (k!=0) {
        c=strchr(c,'|');
        if (c==NULL) break;
        c++;
        k--;
    }
    
    if (c==NULL) return(script_null_to_value(cx));
	
	strncpy(str,c,256);
	str[255]=0x0;
    
    c=strchr(str,'|');
    if (c!=NULL) *c=0x0;
    
    return(script_string_to_value(cx,str));
}

JSValueRef js_obj_set_ambient_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			name[name_str_len];
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	
	script_value_to_string(cx,argv[0],name,name_str_len);
	object_set_ambient(obj,name,script_value_to_float(cx,argv[1]));

	return(script_null_to_value(cx));
}

JSValueRef js_obj_change_ambient_pitch_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	object_change_ambient_pitch(obj,script_value_to_float(cx,argv[0]));

	return(script_null_to_value(cx));
}

JSValueRef js_obj_clear_ambient_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=object_find_uid(js.attach.thing_uid);
	object_clear_ambient(obj);

	return(script_null_to_value(cx));
}





