/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: obj.held object

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

extern server_type		server;
extern js_type			js;

JSBool js_obj_held_get_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp);
JSBool js_obj_held_set_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp);
JSBool js_obj_held_add_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);
JSBool js_obj_held_drop_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval);

script_js_function	obj_held_functions[]={
							{"add",					js_obj_held_add_func,				4},
							{"drop",				js_obj_held_drop_func,				3},
							{0}};

JSClassRef			obj_held_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_obj_held_object(void)
{
	obj_held_class=script_create_class("obj_held_class",js_obj_held_get_property,js_obj_held_set_property);
}

void script_free_obj_held_object(void)
{
	script_free_class(obj_held_class);
}

JSObject* script_add_obj_held_object(JSObject *parent_obj)
{
	return(script_create_child_object(parent_obj,obj_held_class,"held",NULL,obj_held_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSBool js_obj_held_get_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp)
{
	return(script_get_property(cx,j_obj,id,vp,NULL));
}

JSBool js_obj_held_set_property(JSContextRef cx,JSObject *j_obj,JSValueRef id,JSValueRef *vp)
{
	return(script_set_property(cx,j_obj,id,vp,NULL));
}

/* =======================================================

      Add/Drop Functions
      
======================================================= */

JSBool js_obj_held_add_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	int			uid;
	char		name[name_str_len],type[name_str_len],
				script[file_str_len],params[param_str_len],
				err_str[256];
	obj_type	*obj;
	
	obj=object_find_uid(js.attach.thing_uid);

	script_value_to_string(argv[0],name,name_str_len);
	script_value_to_string(argv[1],type,name_str_len);
	script_value_to_string(argv[2],script,file_str_len);
	script_value_to_string(argv[3],params,param_str_len);

	uid=object_held_add(obj,name,type,script,params,err_str);
	if (uid==-1) {
		JS_ReportError(js.cx,err_str);
		return(FALSE);
	}

	*rval=script_int_to_value(uid);
	
	return(TRUE);
}

JSBool js_obj_held_drop_func(JSContextRef cx,JSObject *j_obj,uintN argc,JSValueRef *argv,JSValueRef *rval)
{
	int			uid,y_change;
	float		y_ang;
	char		err_str[256];
	obj_type	*obj;
	
	obj=object_find_uid(js.attach.thing_uid);

	uid=script_value_to_int(argv[0]);
	y_ang=script_value_to_float(argv[1]);
	y_change=script_value_to_int(argv[2]);

	if (!object_held_drop(obj,uid,y_ang,y_change,err_str)) {
		JS_ReportError(js.cx,err_str);
		return(FALSE);
	}

	return(TRUE);
}

