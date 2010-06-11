/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script Save/Load State

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

extern server_type		server;
extern js_type			js;

extern bool game_file_add_chunk(void *data,int count,int sz);
extern char* game_file_get_chunk(void *data);

/* =======================================================

      Setup Script States for Save
      
======================================================= */

void script_state_save_single(attach_type *attach)
{
	int						n,count,str_len;
	char					prop_name[256],err_str[256];
	char					*str;
	script_type				*script;
	JSPropertyNameArrayRef	js_names;
	JSStringRef				js_str;

		// send save state event

	scripts_post_event(attach,sd_event_state,sd_event_state_save,0,err_str);

		// get the script

	if (attach->script_idx==-1) return;

	script=js.script_list.scripts[attach->script_idx];

		// run through the properities

	fprintf(stdout,"------------------- %s \n",script->name);

	js_names=JSObjectCopyPropertyNames(script->cx,script->global_obj);
	count=JSPropertyNameArrayGetCount(js_names);

	for (n=0;n!=count;n++) {

		js_str=JSPropertyNameArrayGetNameAtIndex(js_names,n);
		JSStringGetUTF8CString(js_str,prop_name,256);
		JSStringRelease(js_str);

		prop_name[255]=0x0;

			// skip all DIM3 defines

		if (script_is_prop_define(prop_name)) continue;

		// supergumba -- need to remove GLOBALS (iface, etc.)

			

		fprintf(stdout," %d: %s\n",n,prop_name);
	}

	JSPropertyNameArrayRelease(js_names);

/*
		// convert the global into a string using JSON

	js_str=JSValueCreateJSONString(script->cx,(JSValueRef)script->global_obj,0,NULL);
	if (js_str==NULL) return;

	str_len=JSStringGetMaximumUTF8CStringSize(js_str);
	str=(char*)malloc(str_len);
	if (str==NULL) {
		JSStringRelease(js_str);
		return;
	}

	str_len=JSStringGetUTF8CString(js_str,str,str_len);
	JSStringRelease(js_str);

		// save it into file

	if (str[0]==0x0) {
		str_len=0;
		game_file_add_chunk(&str_len,1,sizeof(int));
		return;
	}

	(&str_len,1,sizeof(int));
	game_file_add_chunk(str,1,str_len);

		// testing

	fprintf(stdout,"------- %s -------\n",script->name);
	fprintf(stdout,"%s\n",str);
	fprintf(stdout,"-------\n");

	js_names=JSObjectCopyPropertyNames(script->cx,script->global_obj);
	count=JSPropertyNameArrayGetCount(js_names);
	for (n=0;n!=count;n++) {
		js_str=JSPropertyNameArrayGetNameAtIndex(js_names,n);
		str_len=JSStringGetUTF8CString(js_str,name,256);
		name[255]=0x0;
		fprintf(stdout," %s\n",name);
		JSStringRelease(js_str);
	}
	JSPropertyNameArrayRelease(js_names);

	fprintf(stdout,"-------\n");
	*/
}

void script_state_save(void)
{
	int				n,k,i;
	obj_type		*obj;
	weapon_type		*weap;
	proj_setup_type	*proj_setup;

	script_state_save_single(&js.game_attach);
	script_state_save_single(&js.course_attach);
	
	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;

		script_state_save_single(&obj->attach);

		for (k=0;k!=max_weap_list;k++) {
			weap=obj->weap_list.weaps[k];
			if (weap==NULL) continue;
			
			script_state_save_single(&weap->attach);
		
			for (i=0;i!=max_proj_setup_list;i++) {
				proj_setup=weap->proj_setup_list.proj_setups[i];
				if (proj_setup==NULL) continue;
				
				script_state_save_single(&proj_setup->attach);
			}
		}
	}
}

/* =======================================================

      Setup Script States after Load
      
======================================================= */

bool script_state_load_single(attach_type *attach)
{
	int				str_len;
	char			err_str[256];
	char			*str;
	script_type		*script;
	JSStringRef		js_str;
	JSObjectRef		js_global;

		// get the script

	if (attach->script_idx==-1) return(TRUE);

	script=js.script_list.scripts[attach->script_idx];

		// get JSON string from file

	game_file_get_chunk(&str_len);
	if (str_len==0) return(TRUE);

	str=(char*)malloc(str_len);
	if (str==NULL) return(FALSE);

	game_file_get_chunk(str);

		// reset global object

	js_str=JSStringCreateWithUTF8CString(str);
	js_global=(JSObjectRef)JSValueMakeFromJSONString(script->cx,js_str);
	JSStringRelease(js_str);

	// supergumba -- what to do?  Copy to global?



		// send load event to script

	scripts_post_event(attach,sd_event_state,sd_event_state_load,0,err_str);

	return(TRUE);
}

void script_state_load(void)
{
	int				n,k,i;
	obj_type		*obj;
	weapon_type		*weap;
	proj_setup_type	*proj_setup;

	if (!script_state_load_single(&js.game_attach)) return;
	if (!script_state_load_single(&js.course_attach)) return;
	
	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;

		if (!script_state_load_single(&obj->attach)) return;

		for (k=0;k!=max_weap_list;k++) {
			weap=obj->weap_list.weaps[k];
			if (weap==NULL) continue;
			
			if (!script_state_load_single(&weap->attach)) return;
		
			for (i=0;i!=max_proj_setup_list;i++) {
				proj_setup=weap->proj_setup_list.proj_setups[i];
				if (proj_setup==NULL) continue;
				
				if (!script_state_load_single(&proj_setup->attach)) return;
			}
		}
	}
}


