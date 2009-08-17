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

extern js_type				js;
extern setup_type			setup;

// supergumba -- js -- probably not necessary anymore
// NOTE: Scripts aren't like other objects which have a list that is compressed as objects
// are deleted from it.  SpiderMonkey requires rooting, which requires a pointer.  The pointers
// must remain correct otherwise spidermonkey will crash.  Therefore, the script list can never
// be changed while other scripts are live.
//

/* =======================================================

      JavaScript Initialize/Shutdown
      
======================================================= */

bool scripts_engine_initialize(char *err_str)
{
		// initialize classes

	script_initialize_classes();

		// load up the define file
		
	script_load_user_defines();
	
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

void scripts_initialize(void)
{
	int			n;
	script_type	*script;
	
		// current uid
		
	js.script_current_uid=0;
	
		// no scripts slots used
		
	script=js.scripts;
	
	for (n=0;n!=max_scripts;n++) {
		script->used=FALSE;
		script++;
	}
}

/* =======================================================

      Find Scripts
      
======================================================= */

int scripts_find_free(void)
{
	int				n;
	script_type		*script;
	
	script=js.scripts;
	
	for (n=0;n!=max_scripts;n++) {
		if (!script->used) return(n);
		script++;
	}
	
	return(-1);
}

int scripts_find_uid(int uid)
{
	int				n;
	script_type		*script;
	
	script=js.scripts;
	
	for (n=0;n!=max_scripts;n++) {
		if ((script->used) && (script->uid==uid)) return(n);
		script++;
	}
	
	return(-1);
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
	
		// execute
		
	memmove(&js.attach,attach,sizeof(attach_type));
	
	j_script_data=JSStringCreateWithUTF8CString(script->data);
	j_script_name=JSStringCreateWithUTF8CString(script->name);

	rval=JSEvaluateScript(script->cx,j_script_data,script->global_obj,j_script_name,0,&exception);

	JSStringRelease(j_script_name);
	JSStringRelease(j_script_data);

	if (rval==NULL) {
		script_value_to_string(script->cx,exception,err_str,256);
		return(FALSE);
	}

		// get a pointer to the event object
		
	scripts_setup_events(script->uid);
	
		// send the construct event
		
	return(scripts_post_event(attach,sd_event_construct,0,0,err_str));
}

/* =======================================================

      Add A Script
      
======================================================= */
	
bool scripts_add(attach_type *attach,char *sub_dir,char *name,char *params,char *err_str)
{
	int						idx;
	bool					ok;
	script_type				*script;

		// no script
		
	attach->script_uid=-1;
	
		// find a unused script
	
	idx=scripts_find_free();
	if (idx==-1) {
		strcpy(err_str,"JavaScript Engine: Reached the maximum number of scripts");
		return(FALSE);
	}
	
		// start the script
		
	script=&js.scripts[idx];
	script->used=TRUE;
	
	strcpy(script->name,name);
	if (params!=NULL) {
		strcpy(script->params,params);
	}
	else {
		script->params[0]=0x0;
	}

		// script attachments
		
	script->uid=js.script_current_uid;
	js.script_current_uid++;
	
	attach->script_uid=script->uid;
	
		// create the context
		// and remember the global object
		
	script->cx=JSGlobalContextCreate(NULL);
	script->global_obj=JSContextGetGlobalObject(script->cx);
	
		// load in script

	if (!script_load_file(script,sub_dir,name,err_str)) {
		script->used=FALSE;
		return(FALSE);
	}
	
		// create the global object

	if (!script_add_global_object(script,err_str)) {
		script_free_file(script);
		script->used=FALSE;
		return(FALSE);
	}
	
		// create the object
	
	script->obj=script_create_main_object(script->cx,attach);
	if (script->obj==NULL) {
		strcpy(err_str,"JavaScript Engine: Not enough memory to create an object");
		script_free_file(script);
		script->used=FALSE;
		return(FALSE);
	}
	
		// compile and execute the construct function
	
	ok=scripts_execute(attach,script,err_str);
	script_free_file(script);
	
	if (!ok) {
		script->used=FALSE;
		return(FALSE);
	}
	
		// root the object
		
	JSValueProtect(script->cx,(JSValueRef)script->obj);
		
	return(TRUE);
}

bool scripts_add_console(attach_type *attach,char *sub_dir,char *name,char *params,char *err_str)
{
	if (!scripts_add(attach,sub_dir,name,params,err_str)) {
		console_add_error(err_str);
		if (setup.debug_console) console_trigger_set();
		return(FALSE);
	}
	
	return(TRUE);
}

/* =======================================================

      Dispose Scripts
      
======================================================= */

void scripts_dispose(int uid)
{
	int			idx;
	script_type	*script;
	
		// no script loaded
		
	if (uid==-1) return;
	
		// find script

	idx=scripts_find_uid(uid);
	if (idx==-1) return;

		// dispose all script timers

	timers_script_dispose(uid);

		// unroot the object and clean up
		// the context

	script=&js.scripts[idx];
	
	JSValueUnprotect(script->cx,(JSValueRef)script->obj);
	JSGlobalContextRelease(script->cx);
	
		// force clean-up of removed scripts
	
	JSGarbageCollect(NULL);
	
		// script is now free
		
	script->used=FALSE;
}




/* =======================================================

      JS Glue Code
      
======================================================= */

// supergumba -- js -- as a patch?
// and TEST!

bool JSValueIsArray(JSContextRef ctx,JSValueRef value)
{
/*
	JSObjectRef			arrayObj,protoObj;
	
	if (!JSValueIsObject(ctx,value)) return(FALSE);
	
	arrayObj=JSValueToObject(ctx,value,NULL);
	if (arrayObj==NULL) return(FALSE);

	protoObj=JSObjectGetPrototype(ctx,arrayObj);
	*/
	
	return(FALSE);
	
}


