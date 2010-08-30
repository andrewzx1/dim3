/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script Lookup Utilities

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
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
#include "interfaces.h"

extern int group_find_by_index(char *name);

extern char				object_type_str[][32];

extern server_type		server;
extern map_type			map;
extern js_type			js;

/* =======================================================

      Objects and Weapons
      
======================================================= */

obj_type* script_find_obj_from_uid_arg(JSContextRef cx,JSValueRef arg,JSValueRef *exception)
{
	int				idx;
	char			err_str[256];
	obj_type		*obj;

	idx=script_value_to_int(cx,arg);
	
	obj=server.obj_list.objs[idx];
	if (obj==NULL) {
		sprintf(err_str,"No object exists with this ID: %d",idx);
		*exception=script_create_exception(cx,err_str);
		return(NULL);
	}
	
	return(obj);
}	

weapon_type* script_find_weapon_from_name_arg(JSContextRef cx,obj_type *obj,JSValueRef arg,JSValueRef *exception)
{
	char			name[name_str_len];
	char			err_str[256];
	weapon_type		*weap;

	script_value_to_string(cx,arg,name,name_str_len);
	
	weap=weapon_find_name(obj,name);
	if (weap==NULL) {
		sprintf(err_str,"Named weapon does not exist in object: %s",name);
		*exception=script_create_exception(cx,err_str);
		return(NULL);
	}
	
	return(weap);
}	

/* =======================================================

      Spots, Nodes and Lights
      
======================================================= */

spot_type* script_find_spot_from_idx_arg(JSContextRef cx,JSValueRef arg,JSValueRef *exception)
{
	int				idx;
	char			err_str[256];

	idx=script_value_to_int(cx,arg);
	if ((idx<0) || (idx>=map.nspot)) {
		sprintf(err_str,"No spot exists with this id: %d",idx);
		*exception=script_create_exception(cx,err_str);
		return(NULL);
	}
	
	return(&map.spots[idx]);
}

spot_type* script_find_spot_from_name_type(JSContextRef cx,JSValueRef arg_0,JSValueRef arg_1,JSValueRef *exception)
{
	int				idx,spawn_type;
	char			name[name_str_len],err_str[256];

	script_value_to_string(cx,arg_0,name,name_str_len);
	spawn_type=script_value_to_int(cx,arg_1);
	
	idx=map_find_random_spot(&map,name,spawn_type);
	if (idx==-1) {
		sprintf(err_str,"No spot exists with this name-type: %s-%s",name,object_type_str[spawn_type]);
		*exception=script_create_exception(cx,err_str);
		return(NULL);
	}
	
	return(&map.spots[idx]);
}

spot_type* script_find_network_spot(JSContextRef cx,obj_type *obj,JSValueRef *exception)
{
	int				idx;
	char			err_str[256];
	
	idx=object_choose_spawn_spot(obj,err_str);
	if (idx==-1) {
		*exception=script_create_exception(cx,err_str);
		return(NULL);
	}
	
	return(&map.spots[idx]);
}

node_type* script_find_node_from_idx_arg(JSContextRef cx,JSValueRef arg,JSValueRef *exception)
{
	int				idx;
	char			err_str[256];

	idx=script_value_to_int(cx,arg);
	if ((idx<0) || (idx>=map.nnode)) {
		sprintf(err_str,"No node exists with this id: %d",idx);
		*exception=script_create_exception(cx,err_str);
		return(NULL);
	}
	
	return(&map.nodes[idx]);
}

map_light_type* script_find_light_from_name(JSContextRef cx,JSValueRef arg,JSValueRef *exception)
{
	int				idx;
	char			name[name_str_len],err_str[256];

	script_value_to_string(cx,arg,name,name_str_len);
	
	idx=map_find_light(&map,name);
	if (idx==-1) {
		sprintf(err_str,"No light exists with this name: %s",name);
		*exception=script_create_exception(cx,err_str);
		return(NULL);
	}
	
	return(&map.lights[idx]);
}

/* =======================================================

      HUD Elements
      
======================================================= */

hud_text_type* script_find_text_from_name(JSContextRef cx,JSValueRef arg,JSValueRef *exception)
{
	char			name[name_str_len],err_str[256];
	hud_text_type	*text;

	script_value_to_string(cx,arg,name,name_str_len);
	text=hud_texts_find(name);
	if (text==NULL) {
		sprintf(err_str,"No interface text exists with the name: %s",name);
		*exception=script_create_exception(cx,err_str);
		return(NULL);
	}
	
	return(text);
}

hud_bitmap_type* script_find_bitmap_from_name(JSContextRef cx,JSValueRef arg,JSValueRef *exception)
{
	char			name[name_str_len],err_str[256];
	hud_bitmap_type	*bitmap;

	script_value_to_string(cx,arg,name,name_str_len);
	bitmap=hud_bitmaps_find(name);
	if (bitmap==NULL) {
		sprintf(err_str,"No interface bitmap exists with the name: %s",name);
		*exception=script_create_exception(cx,err_str);
		return(NULL);
	}
	
	return(bitmap);
}

hud_bar_type* script_find_bar_from_name(JSContextRef cx,JSValueRef arg,JSValueRef *exception)
{
	char			name[name_str_len],err_str[256];
	hud_bar_type	*bar;

	script_value_to_string(cx,arg,name,name_str_len);
	bar=hud_bars_find(name);
	if (bar==NULL) {
		sprintf(err_str,"No interface bar exists with the name: %s",name);
		*exception=script_create_exception(cx,err_str);
		return(NULL);
	}
	
	return(bar);
}

/* =======================================================

      Segment Groups
      
======================================================= */

int script_find_group_from_name(JSContextRef cx,JSValueRef arg,JSValueRef *exception)
{
	int				idx;
	char			name[name_str_len],err_str[256];
	
	script_value_to_string(cx,arg,name,name_str_len);
	idx=group_find_by_index(name);
	if (idx==-1) {
		sprintf(err_str,"No segment group exists with the name: %s",name);
		*exception=script_create_exception(cx,err_str);
		return(-1);
	}
	
	return(idx);
}

/* =======================================================

      Map Movements
      
======================================================= */

int script_find_map_movement_from_name(JSContextRef cx,JSValueRef arg,JSValueRef *exception)
{
	int				idx;
	char			name[name_str_len],err_str[256];
	
	script_value_to_string(cx,arg,name,name_str_len);
	idx=map_movement_find(&map,name);
	if (idx==-1) {
		sprintf(err_str,"No map movement exists with the name: %s",name);
		*exception=script_create_exception(cx,err_str);
		return(-1);
	}
	
	return(idx);
}

/* =======================================================

      Attached Objects
      
======================================================= */

int script_get_attached_object_uid(void)
{
	weapon_type		*weap;
	proj_type		*proj;

	if (js.attach.thing_type==thing_type_object) return(js.attach.obj_idx);

	if (js.attach.thing_type==thing_type_weapon) {
		weap=weapon_script_lookup();
		if (weap!=NULL) return(weap->obj_idx);
	}

	if (js.attach.thing_type==thing_type_projectile) {
		proj=projectile_script_lookup();
		if (proj->on) return(proj->obj_idx);
	}

	return(-1);
}

/* =======================================================

      Models
      
======================================================= */

model_draw* script_find_model_draw(JSObjectRef j_obj)
{
	int					script_idx;
	obj_type			*obj;
	weapon_type			*weap;
	proj_type			*proj;
	proj_setup_type		*proj_setup;
	script_type			*script;
	
	script_idx=(int)JSObjectGetPrivate(j_obj);
	script=js.script_list.scripts[script_idx];
	
		// get correct model from attachment
		
	switch (script->attach.type) {
	
		case script_type_object:
			obj=server.obj_list.objs[script->attach.obj_idx];
			return(&obj->draw);
			
		case script_type_weapon:
			obj=server.obj_list.objs[script->attach.obj_idx];
			weap=obj->weap_list.weaps[script->attach.weap_idx];
			if (weap->dual.in_dual) return(&weap->draw_dual);
			return(&weap->draw);
			
		case script_type_projectile_setup:
			obj=server.obj_list.objs[script->attach.obj_idx];
			weap=obj->weap_list.weaps[script->attach.weap_idx];
			proj_setup=weap->proj_setup_list.proj_setups[script->attach.proj_setup_idx];
			return(&proj_setup->draw);
			
		case script_type_projectile:
			proj=server.proj_list.projs[script->attach.proj_idx];
			return(&proj->draw);
			
	}
	
	return(NULL);
}
	
