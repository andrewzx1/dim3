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

//
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
	js.rt=NULL;
	js.cx=NULL;
	
		// create runtime
		
	js.rt=JS_NewRuntime(js_script_memory_size);
	if (js.rt==NULL) {
		strcpy(err_str,"JavaScript Engine: Not enough memory to create Script RunTime");
		return(FALSE);
	}
	
		// create context
		
	js.cx=JS_NewContext(js.rt,10240);
	if (js.cx==NULL) {
		strcpy(err_str,"JavaScript Engine: Not enough memory to create Script Context");
		return(FALSE);
	}
	
		// set context options
		
	JS_SetOptions(js.cx,JS_GetOptions(js.cx)|JSOPTION_STRICT|JSOPTION_VAROBJFIX|JSOPTION_WERROR);

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
	
	if (js.cx!=NULL) JS_DestroyContext(js.cx);
	if (js.rt!=NULL) JS_DestroyRuntime(js.rt);
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

      Script Roots
      
======================================================= */

void script_add_roots(script_type *script)
{
	JS_AddRoot(js.cx,&script->global);
	JS_AddRoot(js.cx,&script->obj);
}

void script_remove_roots(script_type *script)
{
	JS_RemoveRoot(js.cx,&script->global);
	JS_RemoveRoot(js.cx,&script->obj);
}

void scripts_clean_up_roots(void)
{
	JS_GC(js.cx);
}

/* =======================================================

      Script Clear Attach Data
      
======================================================= */

void scripts_clear_attach_data(attach_type *attach)
{
	int				n;

	for (n=0;n!=max_msg_data;n++) {
		attach->set_msg_data[n].type=d3_jsval_type_int;
		attach->set_msg_data[n].data.d3_int=0;
		attach->get_msg_data[n].type=d3_jsval_type_int;
		attach->get_msg_data[n].data.d3_int=0;
	}
}

/* =======================================================

      Compile and Execute Scripts
      
======================================================= */

bool scripts_execute(attach_type *attach,script_type *script,char *err_str)
{
	JSValueRef		rval;
	
		// execute
		
	memmove(&js.attach,attach,sizeof(attach_type));
	
	if (!JS_EvaluateScript(js.cx,script->global,script->data,script->data_len,script->name,0,&rval)) {
		script_value_to_string(exception,err_str,256);
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

	script->global=NULL;
	script->obj=NULL;

		// script attachments
		
	script->uid=js.script_current_uid;
	js.script_current_uid++;
	
	attach->script_uid=script->uid;
	
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
	
	script->obj=script_create_main_object(attach);
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
	
		// root the objects
		
	script_add_roots(script);
		
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

		// dispose

	script=&js.scripts[idx];
	
	script_remove_roots(script);
	script->used=FALSE;
	
		// force clean-up of removed scripts
	
	scripts_clean_up_roots();
}

