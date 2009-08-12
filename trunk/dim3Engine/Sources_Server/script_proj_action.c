/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: proj.action object

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

extern server_type		server;
extern js_type			js;

JSValueRef js_proj_action_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_proj_action_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_proj_action_get_damage(void);
JSValueRef js_proj_action_get_collision(void);
JSValueRef js_proj_action_get_auto_hitTick(void);
JSValueRef js_proj_action_get_auto_bounce(void);
JSValueRef js_proj_action_get_auto_bounceMinMove(void);
JSValueRef js_proj_action_get_auto_bounceReduce(void);
JSValueRef js_proj_action_get_auto_reflect(void);
void js_proj_action_set_damage(JSValueRef vp,JSValueRef *exception);
void js_proj_action_set_collision(JSValueRef vp,JSValueRef *exception);
void js_proj_action_set_auto_hitTick(JSValueRef vp,JSValueRef *exception);
void js_proj_action_set_auto_bounce(JSValueRef vp,JSValueRef *exception);
void js_proj_action_set_auto_bounceMinMove(JSValueRef vp,JSValueRef *exception);
void js_proj_action_set_auto_bounceReduce(JSValueRef vp,JSValueRef *exception);
void js_proj_action_set_auto_reflect(JSValueRef vp,JSValueRef *exception);
JSValueRef js_proj_action_rotate_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_proj_action_turn_towards_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_proj_action_seek_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_proj_action_seek_target_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_proj_action_bounce_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_proj_action_reflect_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_proj_action_stick_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_proj_action_destroy_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

script_js_property	proj_action_props[]={
							{"damage",				js_proj_action_get_damage,				js_proj_action_set_damage},
							{"collision",			js_proj_action_get_collision,			js_proj_action_set_collision},
							{"autoHitTick",			js_proj_action_get_auto_hitTick,		js_proj_action_set_auto_hitTick},
							{"autoBounce",			js_proj_action_get_auto_bounce,			js_proj_action_set_auto_bounce},
							{"autoBounceMinMove",	js_proj_action_get_auto_bounceMinMove,	js_proj_action_set_auto_bounceMinMove},
							{"autoBounceReduce",	js_proj_action_get_auto_bounceReduce,	js_proj_action_set_auto_bounceReduce},
							{"autoReflect",			js_proj_action_get_auto_reflect,		js_proj_action_set_auto_reflect},
							{0}};

script_js_function	proj_action_functions[]={
							{"rotate",				js_proj_action_rotate_func,				1},
							{"turnTowards",			js_proj_action_turn_towards_func,		2},
							{"seek",				js_proj_action_seek_func,				3},
							{"seekTarget",			js_proj_action_seek_target_func,		2},
							{"bounce",				js_proj_action_bounce_func,				2},
							{"reflect",				js_proj_action_reflect_func,			0},
							{"stick",				js_proj_action_stick_func,				0},
							{"destroy",				js_proj_action_destroy_func,			0},
							{0}};

JSClassRef			proj_action_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_proj_action_object(void)
{
	proj_action_class=script_create_class("proj_action_class",js_proj_action_get_property,js_proj_action_set_property);
}

void script_free_proj_action_object(void)
{
	script_free_class(proj_action_class);
}

JSObjectRef script_add_proj_action_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,proj_action_class,"action",proj_action_props,proj_action_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_proj_action_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,proj_action_props));
}

bool js_proj_action_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,proj_action_props));
}

/* =======================================================

      Getters
      
======================================================= */

JSValueRef js_proj_action_get_damage(void)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
	*vp=script_int_to_value(proj_setup->damage);
	
	return(TRUE);
}

JSValueRef js_proj_action_get_collision(void)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
	*vp=script_bool_to_value(proj_setup->collision);
	
	return(TRUE);
}

JSValueRef js_proj_action_get_auto_hitTick(void)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
	*vp=script_int_to_value(proj_setup->action.hit_tick);
	
	return(TRUE);
}

JSValueRef js_proj_action_get_auto_bounce(void)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
	*vp=script_bool_to_value(proj_setup->action.bounce);
	
	return(TRUE);
}

JSValueRef js_proj_action_get_auto_bounceMinMove(void)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
	*vp=script_float_to_value(proj_setup->action.bounce_min_move);
	
	return(TRUE);
}

JSValueRef js_proj_action_get_auto_bounceReduce(void)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
	*vp=script_float_to_value(proj_setup->action.bounce_reduce);
	
	return(TRUE);
}

JSValueRef js_proj_action_get_auto_reflect(void)
{
	proj_setup_type		*proj_setup;

	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
	*vp=script_bool_to_value(proj_setup->action.reflect);
	
	return(TRUE);
}

/* =======================================================

      Setters
      
======================================================= */

void js_proj_action_set_damage(JSValueRef vp,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
	proj_setup->damage=script_value_to_int(*vp);
}

void js_proj_action_set_collision(JSValueRef vp,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
	proj_setup->collision=script_value_to_bool(*vp);
}

void js_proj_action_set_auto_hitTick(JSValueRef vp,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
	proj_setup->action.hit_tick=script_value_to_int(*vp);
}

void js_proj_action_set_auto_bounce(JSValueRef vp,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
	proj_setup->action.bounce=script_value_to_bool(*vp);
}

void js_proj_action_set_auto_bounceMinMove(JSValueRef vp,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
	proj_setup->action.bounce_min_move=script_value_to_float(*vp);
}

void js_proj_action_set_auto_bounceReduce(JSValueRef vp,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
	proj_setup->action.bounce_reduce=script_value_to_float(*vp);
}

void js_proj_action_set_auto_reflect(JSValueRef vp,JSValueRef *exception)
{
	proj_setup_type		*proj_setup;
	
	proj_setup=proj_setup_get_attach();
	if (proj_setup==NULL) return(TRUE);
	
	proj_setup->action.reflect=script_value_to_bool(*vp);
}

/* =======================================================

      Projectile Changes
      
======================================================= */

JSValueRef js_proj_action_rotate_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	proj_type			*proj;

	proj=proj_get_attach();
	if (proj==NULL) return(TRUE);
	
    proj->ang.y=proj->motion.ang.y=script_value_to_float(argv[0]);
	
	return(TRUE);
}

JSValueRef js_proj_action_turn_towards_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	proj_type			*proj;
	obj_type			*to_obj;

	proj=proj_get_attach();
	if (proj==NULL) return(TRUE);
		
	to_obj=script_find_obj_from_uid_arg(argv[0],exception);
	if (to_obj==NULL) return(FALSE);
	
	projectile_turn_xz_towards(proj,to_obj,script_value_to_float(argv[1]));
	
	return(TRUE);
}

JSValueRef js_proj_action_seek_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	proj_type			*proj;
	obj_type			*to_obj;

	proj=proj_get_attach();
	if (proj==NULL) return(TRUE);
		
	to_obj=script_find_obj_from_uid_arg(argv[0],exception);
	if (to_obj==NULL) return(FALSE);
	
	projectile_seek(proj,to_obj,script_value_to_float(argv[1]),script_value_to_float(argv[2]));
	
	return(TRUE);
}

JSValueRef js_proj_action_seek_target_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type			*to_obj;
	weapon_type			*weap;
	proj_setup_type		*proj_setup;
	proj_type			*proj;

	proj=proj_get_attach();
	if (proj==NULL) return(TRUE);

	proj_setup=proj_setups_find_uid(proj->proj_setup_uid);
	if (proj_setup==NULL) return(TRUE);
	
	weap=weapon_find_uid(proj_setup->weap_uid);
		
	to_obj=object_find_uid(weap->target.obj_uid);
	if (to_obj==NULL) return(FALSE);
	
	projectile_seek(proj,to_obj,script_value_to_float(argv[0]),script_value_to_float(argv[1]));
	
	return(TRUE);
}

JSValueRef js_proj_action_bounce_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
    float				min_ymove,reduce;
	proj_type			*proj;

	proj=proj_get_attach();
	if (proj==NULL) return(TRUE);
    
    min_ymove=script_value_to_float(argv[0]);
	reduce=script_value_to_float(argv[1]);
	
	projectile_bounce(proj,min_ymove,reduce,FALSE);
    
	return(TRUE);
}

JSValueRef js_proj_action_reflect_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	proj_type			*proj;

	proj=proj_get_attach();
	if (proj==NULL) return(TRUE);
	
	projectile_reflect(proj,FALSE);
	     
	return(TRUE);
}

JSValueRef js_proj_action_stick_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	proj_type			*proj;

	proj=proj_get_attach();
	if (proj==NULL) return(TRUE);
    
    proj->stick=TRUE;
    proj->ang.x=0;
    
	return(TRUE);
}

JSValueRef js_proj_action_destroy_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	proj_type			*proj;

	proj=proj_get_attach();
	if (proj==NULL) return(TRUE);
	
	projectile_mark_dispose(proj);
    
	return(TRUE);
}
