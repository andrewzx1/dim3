/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script C<-->JSValueRef Conversion

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

      NULL JSValueRef
      
======================================================= */

inline JSValueRef script_null_to_value(void)
{
	return(JSValueMakeNull(js.cx));
}

inline bool script_is_value_null(JSValueRef val)
{
	return(JSValueIsNull(js.cx,val));
}

/* =======================================================

      Int-JSValueRef Translations
      
======================================================= */

inline int script_value_to_int(JSValueRef val)
{
	return((int)JSValueToNumber(js.cx,val,NULL));
}

inline JSValueRef script_int_to_value(int i)
{
	return(JSValueMakeNumber(js.cx,(float)i));
}

/* =======================================================

      Float-JSValueRef Translations
      
======================================================= */

inline float script_value_to_float(JSValueRef val)
{
	return((float)JSValueToNumber(js.cx,val,NULL));
}

inline JSValueRef script_float_to_value(float f)
{
	return(JSValueMakeNumber(js.cx,(double)f));
}

/* =======================================================

      Bool-JSValueRef Translations
      
======================================================= */

inline bool script_value_to_bool(JSValueRef val)
{
	return(JSValueToBoolean(js.cx,val));
}

inline JSValueRef script_bool_to_value(bool b)
{
	return(JSValueMakeBoolean(js.cx,b));
}

/* =======================================================

      String-JSValueRef Translations
      
======================================================= */

void script_value_to_string(JSValueRef val,char *str,int len)
{
	JSStringRef		js_str;

	js_str=JSValueToStringCopy(js.cx,val,NULL);
	if (js_str==NULL) {
		str[0]=0x0;
		return;
	}

	JSStringGetUTF8CString(js_str,str,len);
	JSStringRelease(js_str);

	str[len-1]=0x0;
}

JSValueRef script_string_to_value(char *str)
{
	JSStringRef		js_str;

	js_str=JSStringCreateWithUTF8CString(str);
	return(JSValueMakeString(js.cx,js_str));
}

/* =======================================================

      Array-JSValueRef Translations
      
======================================================= */

JSValueRef script_int_array_to_value(int cnt,int *values)
{
	int				n;
	JSObjectRef		array_proto_obj,j_obj;
	JSStringRef		array_name;
	JSValueRef		array_val;
	script_type		*script;

		// 64 is the longest array

	if (cnt>64) cnt=64;

		// get array prototype

	script=&js.scripts[scripts_find_uid(js.attach.script_uid)];
	
	array_name=JSStringCreateWithUTF8CString("Array");
	array_val=JSObjectGetProperty(js.cx,script->global,array_name,NULL);
	array_proto_obj=JSValueToObject(js.cx,array_val,NULL);
	JSStringRelease(array_name);

		// create object

	j_obj=JSObjectCallAsConstructor(js.cx,array_proto_obj,0,NULL,NULL);

		// create values

	for (n=0;n!=cnt;n++) {
		JSObjectSetPropertyAtIndex(js.cx,j_obj,n,script_int_to_value(values[n]),NULL);
	}

	return((JSValueRef)j_obj);
}

/* =======================================================

      Exceptions
      
======================================================= */

JSValueRef script_create_exception(char *str)
{
	JSObjectRef			ex_obj;

	ex_obj=JSObjectMake(js.cx,NULL,NULL);
	script_set_single_property(ex_obj,"message",script_string_to_value(str),kJSPropertyAttributeNone);

	return((JSValueRef)ex_obj);
}

void script_exception_to_string(JSValueRef ex_val,char *str,int len)
{
	JSObjectRef			ex_obj;
	JSValueRef			msg;

	ex_obj=JSValueToObject(js.cx,ex_val,NULL);

		// get line number

	// supergumba -- js -- get line number here and sourceURL  (.line, .sourceURL)

		// get message

	msg=script_get_single_property(ex_obj,"message");
	if (msg==NULL) {
		strncpy(str,"Unknown Error",len);
		str[len-1]=0x0;
	}
	else {
		script_value_to_string(msg,str,len);
	}
}

/* =======================================================

      Create dim3 Primitives
      
======================================================= */

JSValueRef script_angle_to_value(float x,float y,float z)
{
	JSObjectRef		j_obj;

	j_obj=JSObjectMake(js.cx,NULL,NULL);

	script_set_single_property(j_obj,"x",script_float_to_value(x),(kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete));
	script_set_single_property(j_obj,"y",script_float_to_value(y),(kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete));
	script_set_single_property(j_obj,"z",script_float_to_value(z),(kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete));
	
	return((JSValueRef)j_obj);
}

JSValueRef script_point_to_value(int x,int y,int z)
{
	JSObjectRef		j_obj;

	j_obj=JSObjectMake(js.cx,NULL,NULL);

	script_set_single_property(j_obj,"x",script_int_to_value(x),(kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete));
	script_set_single_property(j_obj,"y",script_int_to_value(y),(kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete));
	script_set_single_property(j_obj,"z",script_int_to_value(z),(kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete));
	
	return((JSValueRef)j_obj);
}

JSValueRef script_color_to_value(d3col *col)
{
	JSObjectRef		j_obj;

	j_obj=JSObjectMake(js.cx,NULL,NULL);

	script_set_single_property(j_obj,"r",script_float_to_value(col->r),(kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete));
	script_set_single_property(j_obj,"g",script_float_to_value(col->g),(kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete));
	script_set_single_property(j_obj,"b",script_float_to_value(col->b),(kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete));
	
	return((JSValueRef)j_obj);
}
