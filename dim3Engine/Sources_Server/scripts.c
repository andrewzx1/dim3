/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script Engine Main Code

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
#include "consoles.h"

extern bool					scripts_event_lock;

extern js_type				js;
extern setup_type			setup;

/* =======================================================

      JavaScript Initialize/Shutdown
      
======================================================= */

bool scripts_engine_initialize(char *err_str)
{
		// initialize classes

	script_initialize_classes();

		// load up the define file
		
	script_load_user_defines();

		// current events are not locked out

	scripts_event_lock=FALSE;
	
	return(TRUE);
}

void scripts_engine_shutdown(void)
{
	script_free_user_defines();
	script_release_classes();
}

/* =======================================================

      Initialize Scripts
      
======================================================= */

void scripts_initialize_list(void)
{
	int			n;
	
	for (n=0;n!=max_script_list;n++) {
		js.script_list.scripts[n]=NULL;
	}
}

void scripts_free_list(void)
{
	int			n;
	
	for (n=0;n!=max_script_list;n++) {
		if (js.script_list.scripts[n]!=NULL) free(js.script_list.scripts[n]);
	}
}

/* =======================================================

      Script Clear Attach Data
      
======================================================= */

void scripts_clear_attach_data(attach_type *attach)
{
	int				n;

	for (n=0;n!=max_msg_data;n++) {
		attach->set_msg_data[n].type=d3_jsval_type_number;
		attach->set_msg_data[n].data.d3_number=0.0f;
		attach->get_msg_data[n].type=d3_jsval_type_number;
		attach->get_msg_data[n].data.d3_number=0.0f;
	}
}

/* =======================================================

      Compile and Execute Scripts
      
======================================================= */

bool scripts_execute(attach_type *attach,script_type *script,char *err_str)
{
	JSStringRef		j_script_data,j_script_name;
	JSValueRef		rval,exception;
	
		// make sure UTF8 conversion didn't go crazy
		// on some hidden characters
	
	j_script_data=JSStringCreateWithUTF8CString(script->data);
	if (JSStringGetLength(j_script_data)!=strlen(script->data)) {
		JSStringRelease(j_script_data);
		sprintf(err_str,"[%s] contains extraneous control characters",script->name);
		return(FALSE);
	}
	
		// evaulate script
		
	j_script_name=JSStringCreateWithUTF8CString(script->name);
	
	rval=JSEvaluateScript(script->cx,j_script_data,NULL,j_script_name,0,&exception);

	JSStringRelease(j_script_name);
	JSStringRelease(j_script_data);

	if (rval==NULL) {
		script_exception_to_string(script->cx,exception,err_str,256);
		return(FALSE);
	}

		// get a pointer to the event object
		
	if (!scripts_setup_events(script,err_str)) return(FALSE);
	
		// send the construct event
		
	return(scripts_post_event(attach,sd_event_construct,0,0,err_str));
}

/* =======================================================

      Add A Script
      
======================================================= */
	
bool scripts_add(attach_type *attach,char *sub_dir,char *name,char *err_str)
{
	int						n,idx;
	bool					ok;
	script_type				*script;

		// no script
		
	attach->script_idx=-1;
	
		// find a unused script
		
	idx=-1;
	
	for (n=0;n!=max_script_list;n++) {
		script=js.script_list.scripts[n];
		if (script==NULL) {
			idx=n;
			break;
		}
	}
	
	if (idx==-1) {
		strcpy(err_str,"JavaScript Engine: Reached the maximum number of scripts");
		return(FALSE);
	}
	
		// create it
		
	js.script_list.scripts[idx]=(script_type*)malloc(sizeof(script_type));
	if (js.script_list.scripts[idx]==NULL) {
		strcpy(err_str,"JavaScript Engine: Out of memory");
		return(FALSE);
	}

		// add it

	script=js.script_list.scripts[idx];

	script->idx=idx;
	
	strcpy(script->name,name);

		// original object script attachments
		
	attach->script_idx=idx;
	
		// create the context
		// and remember the global object
		
	script->cx=JSGlobalContextCreate(NULL);
	script->global_obj=JSContextGetGlobalObject(script->cx);
	
		// load in script

	if (!script_load_file(script,sub_dir,name,err_str)) {
		free(js.script_list.scripts[idx]);
		js.script_list.scripts[idx]=NULL;
		return(FALSE);
	}
	
		// create the global object

	if (!script_add_global_object(script,err_str)) {
		script_free_file(script);
		free(js.script_list.scripts[idx]);
		js.script_list.scripts[idx]=NULL;
		return(FALSE);
	}

		// add in any constants

	script_defines_create_constants(script);
	
		// create the object

	script->obj=script_create_main_object(script->cx,attach);
	if (script->obj==NULL) {
		strcpy(err_str,"JavaScript Engine: Not enough memory to create an object");
		script_free_file(script);
		free(js.script_list.scripts[idx]);
		js.script_list.scripts[idx]=NULL;
		return(FALSE);
	}
	
		// compile and execute the construct function
	
	ok=scripts_execute(attach,script,err_str);
	script_free_file(script);
	
	if (!ok) {
		free(js.script_list.scripts[idx]);
		js.script_list.scripts[idx]=NULL;
		return(FALSE);
	}
	
		// root the object
		
	JSValueProtect(script->cx,(JSValueRef)script->obj);
		
	return(TRUE);
}

/* =======================================================

      Dispose Scripts
      
======================================================= */

void scripts_dispose(int idx)
{
	script_type		*script;
	
		// no script loaded
		
	if (idx==-1) return;
	
		// dispose all script timers

	timers_script_dispose(idx);

		// unroot the object and clean up
		// the context

	script=js.script_list.scripts[idx];
	
	JSValueUnprotect(script->cx,(JSValueRef)script->obj);
	JSGlobalContextRelease(script->cx);
	
		// force clean-up of removed scripts
	
	JSGarbageCollect(NULL);
	
		// script is now free
		
	free(js.script_list.scripts[idx]);
	js.script_list.scripts[idx]=NULL;
}

/* =======================================================

      JS Glue Code
      
======================================================= */

// supergumba -- js -- as a patch?
// and TEST!

bool JSValueIsArray(JSContextRef ctx,JSValueRef value)
{
	char				*ch,str[64];
	JSStringRef			jsStr;
	JSObjectRef			arrayObj;
	JSValueRef			val;
	
		// only check objects

	if (!JSValueIsObject(ctx,value)) return(FALSE);
	
		// get constructor

	arrayObj=JSValueToObject(ctx,value,NULL);
	if (arrayObj==NULL) return(FALSE);

	jsStr=JSStringCreateWithUTF8CString("constructor");
	val=JSObjectGetProperty(ctx,arrayObj,jsStr,NULL);
	JSStringRelease(jsStr);

	if (val==NULL) return(FALSE);

		// get constructor as a function

	jsStr=JSValueToStringCopy(ctx,val,NULL);
	if (jsStr==NULL) return(FALSE);

	JSStringGetUTF8CString(jsStr,str,64);
	str[63]=0x0;

	JSStringRelease(jsStr);

		// special check to make sure we don't have Array
		// anywhere else in a function body or there just
		// instead a function body

	ch=strchr(str,'{');
	if (ch==NULL) return(FALSE);

	*ch=0x0;

		// look for array in string

	return(strstr(str,"Array")!=NULL);
}


