/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: utility.angle object

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

extern js_type			js;

JSValueRef js_utility_angle_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_utility_angle_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_utility_angle_add_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_utility_angle_sub_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_utility_angle_dif_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticFunction	utility_angle_functions[]={
							{"add",					js_utility_angle_add_func,			kJSPropertyAttributeDontDelete},
							{"sub",					js_utility_angle_sub_func,			kJSPropertyAttributeDontDelete},
							{"dif",					js_utility_angle_dif_func,			kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			utility_angle_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_utility_angle_object(void)
{
	utility_angle_class=script_create_class("utility_angle_class",NULL,utility_angle_functions);
}

void script_free_utility_angle_object(void)
{
	script_free_class(utility_angle_class);
}

JSObjectRef script_add_utility_angle_object(JSContextRef cx,JSObjectRef parent_obj)
{
	return(script_create_child_object(cx,parent_obj,utility_angle_class,"angle",NULL,utility_angle_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_utility_angle_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,NULL));
}

bool js_utility_angle_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,NULL));
}

/* =======================================================

      Angle Functions
      
======================================================= */

JSValueRef js_utility_angle_add_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	float			ang1,ang2,rang;
    
    ang1=script_value_to_float(cx,argv[0]);
	ang2=script_value_to_float(cx,argv[1]);
	
	rang=angle_add(ang1,ang2);
    return(script_float_to_value(cx,rang));
}

JSValueRef js_utility_angle_sub_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	float			ang1,ang2,rang;
	bool			cwise;

    ang1=script_value_to_float(cx,argv[0]);
    ang2=script_value_to_float(cx,argv[1]);
	
	rang=angle_dif(ang1,ang2,&cwise);
	if (!cwise) rang=-rang;
    
	return(script_float_to_value(cx,rang));
}

JSValueRef js_utility_angle_dif_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	float			ang1,ang2,rang;
	bool			cwise;

    ang1=script_value_to_float(cx,argv[0]);
    ang2=script_value_to_float(cx,argv[1]);
	
	rang=angle_dif(ang1,ang2,&cwise);
    return(script_float_to_value(cx,rang));
}
