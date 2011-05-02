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

bool scripts_setup_event_attach(attach_type *attach,int main_event,char *func_name,char *err_str)
{
	int					idx;
	script_type			*script;
	JSObjectRef			func;

		// get the script

	if (attach->script_idx==-1) return(TRUE);
	script=js.script_list.scripts[attach->script_idx];

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

      Scripts Projectile Attachs

	  Projects share scripts with their projectile setup.
	  Because of this, we have to switch the proj_idx in the
	  projectile setup attach when calling into functions,
	  and switch it back when leaving (in case we are calling
	  from within another script.)
      
======================================================= */

int scripts_projectile_start(JSObjectRef j_obj,attach_type *attach)
{
	int						old_proj_idx;
	attach_type				*data_attach;

	if (attach->thing_type!=thing_type_projectile) return(-1);

	data_attach=(attach_type*)JSObjectGetPrivate(j_obj);
	if (data_attach->thing_type!=thing_type_projectile) return(-1);

	old_proj_idx=data_attach->proj_idx;
	data_attach->proj_idx=attach->proj_idx;

	return(old_proj_idx);
}

void script_projectile_finish(JSObjectRef j_obj,int old_proj_idx)
{
	attach_type				*data_attach;

	if (old_proj_idx==-1) return;

	data_attach=(attach_type*)JSObjectGetPrivate(j_obj);
	data_attach->proj_idx=old_proj_idx;
}

/* =======================================================

      Script Events
      
======================================================= */

bool scripts_post_event(attach_type *attach,int main_event,int sub_event,int id,char *err_str)
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
		
	if (attach->script_idx==-1) return(TRUE);
	
	script=js.script_list.scripts[attach->script_idx];

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

	attach->event_state.main_event=main_event;
	attach->event_state.sub_event=sub_event;
	attach->event_state.id=id;
	attach->event_state.tick=tick;

		// handle any proj_setup/proj swaps

	old_proj_idx=scripts_projectile_start(script->obj,attach);

		// run the event function
		// supergumba -- for now we handle both methods, but
		// in the future we should replace with a single method

	if (script->event_attach_list.on) {
		argv[0]=(JSValueRef)script->obj;
		argv[1]=script_int_to_value(script->cx,sub_event);
		argv[2]=script_int_to_value(script->cx,id);
		argv[3]=script_int_to_value(script->cx,tick);

		rval=JSObjectCallAsFunction(script->cx,script->event_attach_list.func[event_idx],NULL,4,argv,&exception);
		if (rval==NULL) script_exception_to_string(script->cx,exception,err_str,256);
	}

	else {
		argv[0]=(JSValueRef)script->obj;
		argv[1]=script_int_to_value(script->cx,main_event);
		argv[2]=script_int_to_value(script->cx,sub_event);
		argv[3]=script_int_to_value(script->cx,id);
		argv[4]=script_int_to_value(script->cx,tick);

		rval=JSObjectCallAsFunction(script->cx,script->event_func,NULL,5,argv,&exception);
		if (rval==NULL) script_exception_to_string(script->cx,exception,err_str,256);
	}

		// switch back any saved proj_idx

	script_projectile_finish(script->obj,old_proj_idx);
	
		// leave event
		
	script->recursive.in_event[event_idx]=FALSE;

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

bool scripts_post_event_call_parent(attach_type *attach,char *err_str)
{
	attach_type		parent_attach;
	script_type		*script;

	if (attach->script_idx==-1) return(TRUE);
	
		// get the parent script

	script=js.script_list.scripts[attach->script_idx];
	if (script->parent_idx==-1) {
		strcpy(err_str,"Can not call parent; this script does not implement a parent script");
		return(FALSE);
	}

		// create attach

	memmove(&parent_attach,attach,sizeof(attach_type));
	parent_attach.script_idx=script->parent_idx;

		// call on parent script

	return(scripts_post_event(&parent_attach,attach->event_state.main_event,attach->event_state.sub_event,attach->event_state.id,err_str));
}

JSValueRef scripts_get_parent_variable(attach_type *attach,char *prop_name,char *err_str)
{
	script_type		*script,*parent_script;
	JSValueRef		rval;

	if (attach->script_idx==-1) {
		strcpy(err_str,"Script in bad state");
		return(NULL);
	}
	
		// get the parent script

	script=js.script_list.scripts[attach->script_idx];
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

JSValueRef scripts_call_parent_function(attach_type *attach,char *func_name,int arg_count,JSValueRef *args,char *err_str)
{
	attach_type		parent_attach;
	script_type		*script;

	if (attach->script_idx==-1) {
		strcpy(err_str,"Script in bad state");
		return(NULL);
	}
	
		// get the parent script

	script=js.script_list.scripts[attach->script_idx];
	if (script->parent_idx==-1) {
		strcpy(err_str,"Can not get parent variable; this script does not implement a parent script");
		return(FALSE);
	}
	
		// create attach

	memmove(&parent_attach,attach,sizeof(attach_type));
	parent_attach.script_idx=script->parent_idx;

		// call the function

	return(scripts_direct_call(&parent_attach,func_name,arg_count,args,err_str));
}

/* =======================================================

      Script Chains
      
======================================================= */

bool scripts_chain(attach_type *attach,char *func_name,char *err_str)
{
	int				old_proj_idx;
	JSValueRef		rval,exception,argv[2];
	JSObjectRef		func_obj;
	script_type		*script;
	
		// no error
		
	err_str[0]=0x0;
	
		// find script
		
	if (attach->script_idx==-1) return(TRUE);
	
	script=js.script_list.scripts[attach->script_idx];
	
		// is the chain a good function?
		
	func_obj=script_get_single_function(script->cx,script->global_obj,func_name);
	if (func_obj==NULL) {
		sprintf(err_str,"Timer: Chaining failed, unknown or non-callable function: %s",func_name);
		return(FALSE);
	}

		// enter recursion

	if (!scripts_recursion_in(script,err_str)) return(FALSE);

		// handle any proj_setup/proj swaps

	old_proj_idx=scripts_projectile_start(script->obj,attach);

		// run the event function
		
	argv[0]=(JSValueRef)script->obj;
	argv[1]=script_int_to_value(script->cx,game_time_get());

	rval=JSObjectCallAsFunction(script->cx,func_obj,NULL,2,argv,&exception);
	if (rval==NULL) script_exception_to_string(script->cx,exception,err_str,256);

		// switch back any saved proj_idx

	script_projectile_finish(script->obj,old_proj_idx);

		// leave recursion

	scripts_recursion_out(script);
	
	return(err_str[0]==0x0);
}

void scripts_chain_console(attach_type *attach,char *func_name)
{
	char			err_str[256];
	
	if (!scripts_chain(attach,func_name,err_str)) console_add_error(err_str);
}

/* =======================================================

      Script Direct Call
      
======================================================= */

JSValueRef scripts_direct_call(attach_type *attach,char *func_name,int arg_count,JSValueRef *args,char *err_str)
{
	int				n,old_proj_idx;
	JSValueRef		rval,exception,argv[5];
	JSObjectRef		func_obj;
	script_type		*script;
	
		// find script
		
	script=js.script_list.scripts[attach->script_idx];

		// enter recursion

	if (!scripts_recursion_in(script,err_str)) return(NULL);

		// handle any proj_setup/proj swaps

	old_proj_idx=scripts_projectile_start(script->obj,attach);

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
	if (rval==NULL) script_exception_to_string(script->cx,exception,err_str,256);

		// switch back any saved proj_idx

	script_projectile_finish(script->obj,old_proj_idx);

		// leave recursion

	scripts_recursion_out(script);
	
	return(rval);
}
