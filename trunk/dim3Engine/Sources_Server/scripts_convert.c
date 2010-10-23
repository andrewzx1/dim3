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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "scripts.h"

extern js_type			js;

/* =======================================================

      NULL JSValueRef
      
======================================================= */

inline JSValueRef script_null_to_value(JSContextRef cx)
{
	return(JSValueMakeNull(cx));
}

inline bool script_is_value_null(JSContextRef cx,JSValueRef val)
{
	return(JSValueIsNull(cx,val));
}

/* =======================================================

      Int-JSValueRef Translations
      
======================================================= */

inline int script_value_to_int(JSContextRef cx,JSValueRef val)
{
	return((int)JSValueToNumber(cx,val,NULL));
}

inline JSValueRef script_int_to_value(JSContextRef cx,int i)
{
	return(JSValueMakeNumber(cx,(float)i));
}

/* =======================================================

      Float-JSValueRef Translations
      
======================================================= */

inline float script_value_to_float(JSContextRef cx,JSValueRef val)
{
	return((float)JSValueToNumber(cx,val,NULL));
}

inline JSValueRef script_float_to_value(JSContextRef cx,float f)
{
	return(JSValueMakeNumber(cx,(double)f));
}

/* =======================================================

      Bool-JSValueRef Translations
      
======================================================= */

inline bool script_value_to_bool(JSContextRef cx,JSValueRef val)
{
	return(JSValueToBoolean(cx,val));
}

inline JSValueRef script_bool_to_value(JSContextRef cx,bool b)
{
	return(JSValueMakeBoolean(cx,b));
}

/* =======================================================

      String-JSValueRef Translations
      
======================================================= */

void script_value_to_string(JSContextRef cx,JSValueRef val,char *str,int len)
{
	JSStringRef		js_str;
	
	if (JSValueIsNull(cx,val)) {
		str[0]=0x0;
		return;
	}

	js_str=JSValueToStringCopy(cx,val,NULL);
	if (js_str==NULL) {
		str[0]=0x0;
		return;
	}

	JSStringGetUTF8CString(js_str,str,len);
	JSStringRelease(js_str);

	str[len-1]=0x0;
}

JSValueRef script_string_to_value(JSContextRef cx,char *str)
{
	JSStringRef		js_str;

	js_str=JSStringCreateWithUTF8CString(str);
	return(JSValueMakeString(cx,js_str));
}

/* =======================================================

      Array-JSValueRef Translations
      
======================================================= */

JSValueRef script_int_array_to_value(JSContextRef cx,int cnt,int *values)
{
	int				n;
	JSValueRef		js_vals[64];

		// 64 is the longest array

	if (cnt>64) cnt=64;

		// create the values

	for (n=0;n!=cnt;n++) {
		js_vals[n]=script_int_to_value(cx,values[n]);
	}

	return((JSValueRef)JSObjectMakeArray(cx,cnt,js_vals,NULL));
}

/* =======================================================

      Exceptions
      
======================================================= */

JSValueRef script_create_exception(JSContextRef cx,char *str)
{
	JSValueRef			argv[1];
	
	argv[0]=script_string_to_value(cx,str);
	return((JSValueRef)JSObjectMakeError(cx,1,argv,NULL));
}

void script_exception_to_string(JSContextRef cx,JSValueRef ex_val,char *str,int len)
{
	char				txt[256];
	JSObjectRef			ex_obj;
	JSValueRef			vp;

	ex_obj=JSValueToObject(cx,ex_val,NULL);

		// get the source and line number

	strcpy(str,"[");

	vp=script_get_single_property(cx,ex_obj,"sourceURL");
	script_value_to_string(cx,vp,txt,256);
	strcat(str,txt);

	strcat(str,":");

	vp=script_get_single_property(cx,ex_obj,"line");
	script_value_to_string(cx,vp,txt,256);
	strcat(str,txt);

	strcat(str,"] ");

		// get message

	vp=script_get_single_property(cx,ex_obj,"message");
	if (vp==NULL) {
		strncat(str,"Unknown Error",len);
		str[len-1]=0x0;
	}
	else {
		script_value_to_string(cx,vp,txt,len);
		strncat(str,txt,len);
		str[len-1]=0x0;
	}
}

/* =======================================================

      Create dim3 Primitives
      
======================================================= */

JSValueRef script_angle_to_value(JSContextRef cx,float x,float y,float z)
{
	JSObjectRef		j_obj;

	j_obj=JSObjectMake(cx,NULL,NULL);

	script_set_single_property(cx,j_obj,"x",script_float_to_value(cx,x),(kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete));
	script_set_single_property(cx,j_obj,"y",script_float_to_value(cx,y),(kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete));
	script_set_single_property(cx,j_obj,"z",script_float_to_value(cx,z),(kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete));
	
	return((JSValueRef)j_obj);
}

JSValueRef script_point_to_value(JSContextRef cx,int x,int y,int z)
{
	JSObjectRef		j_obj;

	j_obj=JSObjectMake(cx,NULL,NULL);

	script_set_single_property(cx,j_obj,"x",script_int_to_value(cx,x),(kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete));
	script_set_single_property(cx,j_obj,"y",script_int_to_value(cx,y),(kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete));
	script_set_single_property(cx,j_obj,"z",script_int_to_value(cx,z),(kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete));
	
	return((JSValueRef)j_obj);
}

JSValueRef script_color_to_value(JSContextRef cx,d3col *col)
{
	JSObjectRef		j_obj;

	j_obj=JSObjectMake(cx,NULL,NULL);

	script_set_single_property(cx,j_obj,"r",script_float_to_value(cx,col->r),(kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete));
	script_set_single_property(cx,j_obj,"g",script_float_to_value(cx,col->g),(kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete));
	script_set_single_property(cx,j_obj,"b",script_float_to_value(cx,col->b),(kJSPropertyAttributeReadOnly|kJSPropertyAttributeDontDelete));
	
	return((JSValueRef)j_obj);
}

