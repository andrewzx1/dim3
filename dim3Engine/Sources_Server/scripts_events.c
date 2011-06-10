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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "interface.h"
#include "scripts.h"

bool						scripts_event_lock;

extern server_type			server;
extern js_type				js;
extern setup_type			setup;

/* =======================================================

      Script Events Setup
      
======================================================= */

void scripts_setup_events(script_type *script)
{
	script->event_func=script_get_single_function(script->cx,script->global_obj,"event");
}

bool scripts_setup_event_attach(int script_idx,int main_event,char *func_name,char *err_str)
{
	int					idx;
	script_type			*script;
	JSObjectRef			func;

		// get the script

	script=js.script_list.scripts[script_idx];

		// get the func and determine if
		// it exists and is callable
		
	func=script_get_single_function(script->cx,script->global_obj,func_name);
	if (func==NULL) {
		sprintf(err_str,"[%s] '%s' does not exist or is not a callable function",script->name,func_name);
		return(FALSE);
	}

		// legal event?

	if ((main_event<event_main_id_start) || (main_event>event_main_id_end)) {
		strcpy(err_str,"Attempting to setup an unknown event");
		return(FALSE);
	}

		// attach, any attachment turns on
		// this version of event calling

	idx=main_event-event_main_id_start;

	script->event_attach_list.on=TRUE;
	script->event_attach_list.func[idx]=func;

	return(TRUE);
}

/* =======================================================

      Locking and Unlocking Events
      
======================================================= */

inline void scripts_lock_events(void)
{
		// force the core to never call events
		// this is mostly used when restoring a saved
		// game (need to load game/map but not affect script state)

	scripts_event_lock=TRUE;
}

inline void scripts_unlock_events(void)
{
	scripts_event_lock=FALSE;
}

/* =======================================================

      Scripts Recursion Checks
      
======================================================= */

bool scripts_recursion_in(script_type *script,char *err_str)
{
	if (script->recursive.count==js_max_recursive_count) {
		sprintf(err_str,"[%s] Script has recursed to deep",script->name);
		return(FALSE);
	}

	script->recursive.count++;

	return(TRUE);
}

void scripts_recursion_out(script_type *script)
{
	script->recursive.count--;
}

/* =======================================================

      Script Events
      
======================================================= */

bool scripts_post_event_on_attach(int script_idx,int override_proj_idx,int main_event,int sub_event,int id,char *err_str)
{
	int						event_idx,tick,old_proj_idx;
	JSValueRef				rval,exception,argv[5];
	script_type				*script;
	
		// no error
		
	err_str[0]=0x0;

		// are events locked?  If so, don't call
		// any events, but act as if it was OK

	if (scripts_event_lock) return(TRUE);
	
		// find script
		
	script=js.script_list.scripts[script_idx];

		// is this an attached event?

	event_idx=main_event-event_main_id_start;

	if (script->event_attach_list.on) {
		if (script->event_attach_list.func[event_idx]==NULL) return(TRUE);
	}
	else {
		if (script->event_func==NULL) return(TRUE);
	}
	
		// can't re-enter an event we are already in
		// this is a silent failure as it usually happens
		// in cascading damage events
		
	if (script->recursive.in_event[event_idx]) return(TRUE);

		// enter recursion

	if (!scripts_recursion_in(script,err_str)) return(FALSE);
	
		// in event
		
	script->recursive.in_event[event_idx]=TRUE;
	
		// attach to proper script
		
	tick=game_time_get();

	script->event_state.main_event=main_event;
	script->event_state.sub_event=sub_event;
	script->event_state.id=id;
	script->event_state.tick=tick;

		// handle any proj_setup/proj swaps

	if (override_proj_idx!=-1) {
		old_proj_idx=script->attach.proj_idx;
		script->attach.proj_idx=override_proj_idx;
	}

		// run the event function
		// supergumba -- for now we handle both methods, but
		// in the future we should replace with a single method
		
	if (script->event_attach_list.on) {
		argv[0]=(JSValueRef)script->obj;
		argv[1]=script_int_to_value(script->cx,sub_event);
		argv[2]=script_int_to_value(script->cx,id);
		argv[3]=script_int_to_value(script->cx,tick);
		
		rval=JSObjectCallAsFunction(script->cx,script->event_attach_list.func[event_idx],NULL,4,argv,&exception);
		if (rval==NULL) script_exception_to_string(script->cx,main_event,exception,err_str,256);
	}

	else {
		argv[0]=(JSValueRef)script->obj;
		argv[1]=script_int_to_value(script->cx,main_event);
		argv[2]=script_int_to_value(script->cx,sub_event);
		argv[3]=script_int_to_value(script->cx,id);
		argv[4]=script_int_to_value(script->cx,tick);

		rval=JSObjectCallAsFunction(script->cx,script->event_func,NULL,5,argv,&exception);
		if (rval==NULL) script_exception_to_string(script->cx,main_event,exception,err_str,256);
	}

		// switch back any saved proj_idx

	if (override_proj_idx!=-1) script->attach.proj_idx=old_proj_idx;
	
		// leave event
		
	script->recursive.in_event[event_idx]=FALSE;

		// leave recursion

	scripts_recursion_out(script);
	
	return(err_str[0]==0x0);
}

bool scripts_post_event(int script_idx,int override_proj_idx,int main_event,int sub_event,int id,char *err_str)
{
	int						event_idx;
	script_type				*script,*parent_script;
	
		// ignore if no script

	if (script_idx==-1) return(TRUE);

		// can't do this if not using event attachments
	
	script=js.script_list.scripts[script_idx];
	if (!script->event_attach_list.on) return(scripts_post_event_on_attach(script_idx,override_proj_idx,main_event,sub_event,id,err_str));

	event_idx=main_event-event_main_id_start;

		// try event is on child

	if (script->event_attach_list.func[event_idx]!=NULL) return(scripts_post_event_on_attach(script_idx,override_proj_idx,main_event,sub_event,id,err_str));	

		// now try parent

	if (script->parent_idx==-1) return(TRUE);

	parent_script=js.script_list.scripts[script->parent_idx];
	if (parent_script->event_attach_list.func[event_idx]==NULL) return(TRUE);

	return(scripts_post_event_on_attach(parent_script->idx,override_proj_idx,main_event,sub_event,id,err_str));
}

void scripts_post_event_console(int script_idx,int override_proj_idx,int main_event,int sub_event,int id)
{
	char			err_str[256];
	
	if (!scripts_post_event(script_idx,override_proj_idx,main_event,sub_event,id,err_str)) {
		console_add_error(err_str);
	}
}

/* =======================================================

      Script Parent Calls
      
======================================================= */

bool scripts_post_event_call_parent(int script_idx,char *err_str)
{
	int				override_proj_idx;
	script_type		*script;
	
		// get the parent script

	script=js.script_list.scripts[script_idx];
	if (script->parent_idx==-1) {
		strcpy(err_str,"Can not call parent; this script does not implement a parent script");
		return(FALSE);
	}
		// get the override

	override_proj_idx=-1;
	if (script->attach.thing_type==thing_type_projectile) override_proj_idx=script->attach.proj_idx;

		// call on parent script

	return(scripts_post_event_on_attach(script->parent_idx,override_proj_idx,script->event_state.main_event,script->event_state.sub_event,script->event_state.id,err_str));
}

JSValueRef scripts_get_parent_variable(int script_idx,char *prop_name,char *err_str)
{
	script_type		*script,*parent_script;
	JSValueRef		rval;
	
		// get the parent script

	script=js.script_list.scripts[script_idx];
	if (script->parent_idx==-1) {
		strcpy(err_str,"Can not get parent variable; this script does not implement a parent script");
		return(FALSE);
	}
	
	parent_script=js.script_list.scripts[script->parent_idx];

		// get variable

	rval=script_get_single_property_with_has_check(parent_script->cx,parent_script->global_obj,prop_name);
	if (rval==NULL) {
		sprintf(err_str,"Parent has no variable named: %s",prop_name);
		return(NULL);
	}

	return(rval);
}

JSValueRef scripts_call_parent_function(int script_idx,char *func_name,int arg_count,JSValueRef *args,char *err_str)
{
	int				override_proj_idx;
	script_type		*script,*parent_script;
	
		// get the parent script

	script=js.script_list.scripts[script_idx];
	if (script->parent_idx==-1) {
		strcpy(err_str,"Can not call parent function; this script does not implement a parent script");
		return(FALSE);
	}
	
		// save the event state
		
	parent_script=js.script_list.scripts[script->parent_idx];
	memmove(&parent_script->event_state,&script->event_state,sizeof(script_event_state_type));

		// get the override

	override_proj_idx=-1;
	if (script->attach.thing_type==thing_type_projectile) override_proj_idx=script->attach.proj_idx;

		// call the function

	return(scripts_direct_call(script->parent_idx,override_proj_idx,func_name,arg_count,args,err_str));
}

/* =======================================================

      Script Child Calls
      
======================================================= */

JSValueRef scripts_get_child_variable(int script_idx,char *prop_name,char *err_str)
{
	script_type		*script,*child_script;
	JSValueRef		rval;
	
		// get the child script

	script=js.script_list.scripts[script_idx];
	if (script->child_idx==-1) {
		strcpy(err_str,"Can not get child variable; this script is not implemented from a parent script");
		return(FALSE);
	}
	
	child_script=js.script_list.scripts[script->child_idx];

		// get variable

	rval=script_get_single_property_with_has_check(child_script->cx,child_script->global_obj,prop_name);
	if (rval==NULL) {
		sprintf(err_str,"Child has no variable named: %s",prop_name);
		return(NULL);
	}

	return(rval);
}

JSValueRef scripts_call_child_function(int script_idx,char *func_name,int arg_count,JSValueRef *args,char *err_str)
{
	int				override_proj_idx;
	script_type		*script,*child_script;
	
		// get the child script

	script=js.script_list.scripts[script_idx];
	if (script->child_idx==-1) {
		strcpy(err_str,"Can not get call child function; this script is not implemented from a parent script");
		return(FALSE);
	}
	
		// save the event state
		
	child_script=js.script_list.scripts[script->child_idx];
	memmove(&child_script->event_state,&script->event_state,sizeof(script_event_state_type));

		// get the override

	override_proj_idx=-1;
	if (script->attach.thing_type==thing_type_projectile) override_proj_idx=script->attach.proj_idx;

		// call the function

	return(scripts_direct_call(script->child_idx,override_proj_idx,func_name,arg_count,args,err_str));
}

/* =======================================================

      Script Chains
      
======================================================= */

bool scripts_chain(int script_idx,int override_proj_idx,char *func_name,char *err_str)
{
	int				old_proj_idx;
	JSValueRef		rval,exception,argv[2];
	JSObjectRef		func_obj;
	script_type		*script;
	
		// no error
		
	err_str[0]=0x0;
	
		// find script
		
	script=js.script_list.scripts[script_idx];
	
		// is the chain a good function?
		
	func_obj=script_get_single_function(script->cx,script->global_obj,func_name);
	if (func_obj==NULL) {
		sprintf(err_str,"Timer: Chaining failed, unknown or non-callable function: %s",func_name);
		return(FALSE);
	}

		// enter recursion

	if (!scripts_recursion_in(script,err_str)) return(FALSE);

		// handle any proj_setup/proj swaps

	if (override_proj_idx!=-1) {
		old_proj_idx=script->attach.proj_idx;
		script->attach.proj_idx=override_proj_idx;
	}

		// run the event function
		
	argv[0]=(JSValueRef)script->obj;
	argv[1]=script_int_to_value(script->cx,game_time_get());

	rval=JSObjectCallAsFunction(script->cx,func_obj,NULL,2,argv,&exception);
	if (rval==NULL) script_exception_to_string(script->cx,script->event_state.main_event,exception,err_str,256);

		// switch back any saved proj_idx

	if (override_proj_idx!=-1) script->attach.proj_idx=old_proj_idx;

		// leave recursion

	scripts_recursion_out(script);
	
	return(err_str[0]==0x0);
}

void scripts_chain_console(int script_idx,int override_proj_idx,char *func_name)
{
	char			err_str[256];
	
	if (!scripts_chain(script_idx,override_proj_idx,func_name,err_str)) console_add_error(err_str);
}

/* =======================================================

      Script Direct Call
      
======================================================= */

JSValueRef scripts_direct_call(int script_idx,int override_proj_idx,char *func_name,int arg_count,JSValueRef *args,char *err_str)
{
	int				n,old_proj_idx;
	JSValueRef		rval,exception,argv[5];
	JSObjectRef		func_obj;
	script_type		*script;
	
		// find script
		
	script=js.script_list.scripts[script_idx];

		// enter recursion

	if (!scripts_recursion_in(script,err_str)) return(NULL);

		// handle any proj_setup/proj swaps

	if (override_proj_idx!=-1) {
		old_proj_idx=script->attach.proj_idx;
		script->attach.proj_idx=override_proj_idx;
	}

		// find function

	func_obj=script_get_single_function(script->cx,script->global_obj,func_name);
	if (func_obj==NULL) {
		sprintf(err_str,"Call failed, unknown function: %s",func_name);
		return(NULL);
	}

		// call the function
		
	argv[0]=(JSValueRef)script->obj;

	for (n=0;n!=arg_count;n++) { 
		argv[n+1]=args[n];
	}

	rval=JSObjectCallAsFunction(script->cx,func_obj,NULL,(arg_count+1),argv,&exception);
	if (rval==NULL) script_exception_to_string(script->cx,script->event_state.main_event,exception,err_str,256);

		// switch back any saved proj_idx

	if (override_proj_idx!=-1) script->attach.proj_idx=old_proj_idx;

		// leave recursion

	scripts_recursion_out(script);
	
	return(rval);
}
