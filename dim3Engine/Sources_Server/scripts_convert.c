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


/* supergumba -- JS -- array code
    JSStringRef array = JSStringCreateWithUTF8CString("Array");
    JSObjectRef arrayConstructor = JSValueToObject(context, JSObjectGetProperty(context, globalObject, array, NULL), NULL);
    JSStringRelease(array);
    result = JSObjectCallAsConstructor(context, arrayConstructor, 0, NULL, NULL);
*/

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
	JSString		js_str;

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
	JSString		js_str;

	js_str=JSStringCreateWithUTF8CString(str);
	return(JSValueMakeString(js.cx,js_str));
}

/* =======================================================

      Array-JSValueRef Translations
      
======================================================= */

JSValueRef script_int_array_to_value(int cnt,int *values)
{
	int				n;
	JSObject		*j_obj;
	JSValueRef			j_vals[64];

		// 64 is the longest array

	if (cnt>64) cnt=64;

		// create values

	for (n=0;n!=cnt;n++) {
		j_vals[n]=script_int_to_value(values[n]);
	}

		// create array

	j_obj=JS_NewArrayObject(js.cx,cnt,j_vals);
	return(OBJECT_TO_JSVAL(j_obj));
}

/* =======================================================

      Exceptions
      
======================================================= */

JSValueRef script_create_exception(char *str)
{
	JSObjectRef			ex_obj;

	ex_obj=JSObjectMake(js.cx,NULL,NULL);
	JSSetProperty(js.cx,ex_obj,script_string_to_value("message"),script_string_to_value(str),kJSPropertyAttributeNone,NULL);

	return((JSValueRef)ex_obj);
}

void script_exception_to_string(JSValueRef ex_obj,char *str,int len)
{
	JSValueRef			msg;

		// get line number

	// supergumba -- js -- get line number here and sourceURL  (.line, .sourceURL)

		// get message

	msg=JSObjectGetProperty(js.cx,ex_obj,script_string_to_value("message"),NULL);
	if (msg==NULL) {
		strncpy(str,len,"Unknown Error");
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
	JSObject		*j_obj;

	j_obj=JS_NewObject(js.cx,NULL,NULL,NULL);
	if (j_obj==NULL) return(JSVAL_NULL);

	JS_DefineProperty(js.cx,j_obj,"x",script_float_to_value(x),NULL,NULL,JSPROP_READONLY|JSPROP_PERMANENT);
	JS_DefineProperty(js.cx,j_obj,"y",script_float_to_value(y),NULL,NULL,JSPROP_READONLY|JSPROP_PERMANENT);
	JS_DefineProperty(js.cx,j_obj,"z",script_float_to_value(z),NULL,NULL,JSPROP_READONLY|JSPROP_PERMANENT);
	
	return(OBJECT_TO_JSVAL(j_obj));
}

JSValueRef script_point_to_value(int x,int y,int z)
{
	JSObject		*j_obj;

	j_obj=JS_NewObject(js.cx,NULL,NULL,NULL);
	if (j_obj==NULL) return(JSVAL_NULL);

	JS_DefineProperty(js.cx,j_obj,"x",script_int_to_value(x),NULL,NULL,JSPROP_READONLY|JSPROP_PERMANENT);
	JS_DefineProperty(js.cx,j_obj,"y",script_int_to_value(y),NULL,NULL,JSPROP_READONLY|JSPROP_PERMANENT);
	JS_DefineProperty(js.cx,j_obj,"z",script_int_to_value(z),NULL,NULL,JSPROP_READONLY|JSPROP_PERMANENT);
	
	return(OBJECT_TO_JSVAL(j_obj));
}

JSValueRef script_color_to_value(d3col *col)
{
	JSObject		*j_obj;

	j_obj=JS_NewObject(js.cx,NULL,NULL,NULL);
	if (j_obj==NULL) return(JSVAL_NULL);

	JS_DefineProperty(js.cx,j_obj,"r",script_float_to_value(col->r),NULL,NULL,JSPROP_READONLY|JSPROP_PERMANENT);
	JS_DefineProperty(js.cx,j_obj,"g",script_float_to_value(col->g),NULL,NULL,JSPROP_READONLY|JSPROP_PERMANENT);
	JS_DefineProperty(js.cx,j_obj,"b",script_float_to_value(col->b),NULL,NULL,JSPROP_READONLY|JSPROP_PERMANENT);
	
	return(OBJECT_TO_JSVAL(j_obj));
}
