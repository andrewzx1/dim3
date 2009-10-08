/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script Events

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
#include "interfaces.h"

extern server_type			server;
extern js_type				js;
extern setup_type			setup;

/* =======================================================

      Script Events Setup
      
======================================================= */

bool scripts_setup_events(script_type *script,char *err_str)
{
		// cache event function
		
	script->event_func=(JSObjectRef)script_get_single_property(script->cx,script->global_obj,"event");
	
		// determine if event exists and is callable
		
	if (!JSValueIsObject(script->cx,script->event_func)) {
		script->event_func=NULL;
	}
	else {
		if (!JSObjectIsFunction(script->cx,script->event_func)) script->event_func=NULL;
	}
	
	if (script->event_func==NULL) {
		sprintf(err_str,"[%s] 'event' function is required",script->name);
		return(FALSE);
	}

		// set recursive count to 0

	script->recursive_count=0;
	
	return(TRUE);
}

/* =======================================================

      Scripts Recursion Checks
      
======================================================= */

bool scripts_recursion_in(script_type *script,char *err_str)
{
	if (script->recursive_count==js_max_recursive_count) {
		sprintf(err_str,"[%s] Script has recursed to deep",script->name);
		return(FALSE);
	}

	script->recursive_count++;

	return(TRUE);
}

void scripts_recursion_out(script_type *script)
{
	script->recursive_count--;
}

/* =======================================================

      Script Events
      
======================================================= */

bool scripts_post_event(attach_type *attach,int main_event,int sub_event,int id,char *err_str)
{
	int				idx;
	JSValueRef		rval,exception,argv[5];
	script_type		*script;
	attach_type		old_attach;
	
		// no error
		
	err_str[0]=0x0;
	
		// find script
		
	if (attach->script_uid==-1) return(TRUE);
	
	idx=scripts_find_uid(attach->script_uid);
	if (idx==-1) return(TRUE);
	
	script=&js.scripts[idx];

		// enter recursion

	if (!scripts_recursion_in(script,err_str)) return(FALSE);
	
		// save current attach in case event called within another script
		
	memmove(&old_attach,&js.attach,sizeof(attach_type));
	
		// attach to proper script
		
	memmove(&js.attach,attach,sizeof(attach_type));

		// run the event function
		
	argv[0]=(JSValueRef)script->obj;
	argv[1]=script_int_to_value(script->cx,main_event);
	argv[2]=script_int_to_value(script->cx,sub_event);
	argv[3]=script_int_to_value(script->cx,id);
	argv[4]=script_int_to_value(script->cx,js.time.current_tick);

// supergumba -- testing for bugs	
//	fprintf(stdout,"Event (script:%s) (id:%d.%d)\n",script->name,main_event,sub_event);
//	fflush(stdout);

	rval=JSObjectCallAsFunction(script->cx,script->event_func,NULL,5,argv,&exception);
	if (rval==NULL) {
		script_exception_to_string(script->cx,exception,err_str,256);
	}
		
		// restore old attach
		
	memmove(&js.attach,&old_attach,sizeof(attach_type));

		// leave recursion

	scripts_recursion_out(script);
	
	return(err_str[0]==0x0);
}

void scripts_post_event_console(attach_type *attach,int main_event,int sub_event,int id)
{
	char			err_str[256];
	
	if (!scripts_post_event(attach,main_event,sub_event,id,err_str)) {
		console_add_error(err_str);
	}
}

/* =======================================================

      Script Chains
      
======================================================= */

bool scripts_chain(attach_type *attach,char *func_name,char *err_str)
{
	int				idx;
	JSValueRef		rval,exception,argv[2];
	JSObjectRef		func_obj;
	script_type		*script;
	attach_type		old_attach;
	
		// no error
		
	err_str[0]=0x0;
	
		// find script
		
	if (attach->script_uid==-1) return(TRUE);
	
	idx=scripts_find_uid(attach->script_uid);
	if (idx==-1) return(TRUE);
	
	script=&js.scripts[idx];

		// enter recursion

	if (!scripts_recursion_in(script,err_str)) return(FALSE);
	
		// get the function
		
	func_obj=(JSObjectRef)script_get_single_property(script->cx,script->global_obj,func_name);
	if (func_obj==NULL) {
		sprintf(err_str,"Chaining failed, unknown function '%s'",func_name);
		return(FALSE);
	}
	
		// save current attach in case event called within another script
		
	memmove(&old_attach,&js.attach,sizeof(attach_type));
	
		// attach to proper script
		
	memmove(&js.attach,attach,sizeof(attach_type));

		// run the event function
		
	argv[0]=(JSValueRef)script->obj;
	argv[1]=script_int_to_value(script->cx,js.time.current_tick);

	rval=JSObjectCallAsFunction(script->cx,func_obj,NULL,2,argv,&exception);
	if (rval==NULL) {
		script_exception_to_string(script->cx,exception,err_str,256);
	}
		
		// restore old attach
		
	memmove(&js.attach,&old_attach,sizeof(attach_type));

		// leave recursion

	scripts_recursion_out(script);
	
	return(err_str[0]==0x0);
}

void scripts_chain_console(attach_type *attach,char *func_name)
{
	char			err_str[256];
	
	if (!scripts_chain(attach,func_name,err_str)) {
		console_add_error(err_str);
	}
}

/* =======================================================

      Script Direct Call
      
======================================================= */

JSValueRef scripts_direct_call(attach_type *attach,char *func_name,int arg_count,JSValueRef *args,JSValueRef *exception)
{
	int				n,idx;
	char			err_str[256];
	JSValueRef		rval,argv[5];
	JSObjectRef		func_obj;
	script_type		*script;
	attach_type		old_attach;
	
		// find script
		
	idx=scripts_find_uid(attach->script_uid);
	script=&js.scripts[idx];

		// enter recursion

	if (!scripts_recursion_in(script,err_str)) {
		*exception=script_create_exception(script->cx,err_str);
		return(script_null_to_value(script->cx));
	}

		// find function

	func_obj=(JSObjectRef)script_get_single_property(script->cx,script->global_obj,func_name);
	if (func_obj==NULL) {
		sprintf(err_str,"Call failed, unknown function: %s",func_name);
		*exception=script_create_exception(script->cx,err_str);
		return(script_null_to_value(script->cx));
	}
	
		// save current attach in case event called within another script
		
	memmove(&old_attach,&js.attach,sizeof(attach_type));
	
		// attach to proper script
		
	memmove(&js.attach,attach,sizeof(attach_type));

		// run the event function
		
	argv[0]=(JSValueRef)script->obj;

	for (n=0;n!=arg_count;n++) { 
		argv[n+1]=args[n];
	}

	rval=JSObjectCallAsFunction(script->cx,func_obj,NULL,(arg_count+1),argv,exception);
	if (rval==NULL) rval=script_null_to_value(script->cx);
		
		// restore old attach
		
	memmove(&js.attach,&old_attach,sizeof(attach_type));

		// leave recursion

	scripts_recursion_out(script);
	
	return(rval);
}
