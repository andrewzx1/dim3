/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: spawn object

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
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

#include "objects.h"
#include "effects.h"
#include "scripts.h"
#include "physics.h"

extern server_type		server;
extern js_type			js;

JSValueRef js_spawn_particle_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_spawn_particle_moving_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_spawn_particle_line_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_spawn_ring_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_spawn_ring_line_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_spawn_flash_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_spawn_lightning_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_spawn_ray_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_spawn_ray_team_color_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_spawn_shake_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_spawn_push_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

JSStaticFunction	spawn_functions[]={
							{"particle",			js_spawn_particle_func,				kJSPropertyAttributeDontDelete},
							{"particleMoving",		js_spawn_particle_moving_func,		kJSPropertyAttributeDontDelete},
							{"particleLine",		js_spawn_particle_line_func,		kJSPropertyAttributeDontDelete},
							{"ring",				js_spawn_ring_func,					kJSPropertyAttributeDontDelete},
							{"ringLine",			js_spawn_ring_line_func,			kJSPropertyAttributeDontDelete},
							{"flash",				js_spawn_flash_func,				kJSPropertyAttributeDontDelete},
							{"lightning",			js_spawn_lightning_func,			kJSPropertyAttributeDontDelete},
							{"ray",					js_spawn_ray_func,					kJSPropertyAttributeDontDelete},
							{"rayTeamColor",		js_spawn_ray_team_color_func,		kJSPropertyAttributeDontDelete},
							{"shake",				js_spawn_shake_func,				kJSPropertyAttributeDontDelete},
							{"push",				js_spawn_push_func,					kJSPropertyAttributeDontDelete},
							{0,0,0}};

JSClassRef			spawn_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_global_spawn_object(void)
{
	spawn_class=script_create_class("spawn_class",NULL,spawn_functions);
}

void script_free_global_spawn_object(void)
{
	script_free_class(spawn_class);
}

JSObjectRef script_add_global_spawn_object(JSContextRef cx,JSObjectRef parent_obj)
{
	return(script_create_child_object(cx,parent_obj,spawn_class,"spawn"));
}

/* =======================================================

      Spawn Exceptions
      
======================================================= */

JSValueRef js_particle_name_exception(JSContextRef cx,char *name)
{
	char			err_str[256];

	sprintf(err_str,"Named particle does not exist: %s",name);
	return(script_create_exception(cx,err_str));
}

JSValueRef js_ring_name_exception(JSContextRef cx,char *name)
{
	char			err_str[256];

	sprintf(err_str,"Named ring does not exist: %s",name);
	return(script_create_exception(cx,err_str));
}

/* =======================================================

      Spawn Particle
      
======================================================= */

JSValueRef js_spawn_particle_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				idx;
	char			name[name_str_len];
	d3pnt			pt;
	
	if (!script_check_param_count(cx,func,argc,4,exception)) return(script_null_to_value(cx));
	
	pt.x=script_value_to_int(cx,argv[0]);
	pt.z=script_value_to_int(cx,argv[1]);
	pt.y=script_value_to_int(cx,argv[2]);
	
	script_value_to_string(cx,argv[3],name,name_str_len);
	
	idx=particle_find_index(name);
	if (idx==-1) {
		*exception=js_particle_name_exception(cx,name);
	}
	else {
		script_bool_to_value(cx,particle_spawn(idx,script_get_attached_object_uid(),&pt,NULL,NULL));
	}

	return(script_null_to_value(cx));
}

JSValueRef js_spawn_particle_moving_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				idx;
	char			name[name_str_len];
	d3pnt			pt;
	particle_motion	motion;
	
	if (!script_check_param_count(cx,func,argc,7,exception)) return(script_null_to_value(cx));
	
	pt.x=script_value_to_int(cx,argv[0]);
	pt.z=script_value_to_int(cx,argv[1]);
	pt.y=script_value_to_int(cx,argv[2]);
	
	motion.bone_idx=-1;
	motion.vct.x=script_value_to_float(cx,argv[3]);
	motion.vct.z=script_value_to_float(cx,argv[4]);
	motion.vct.y=script_value_to_float(cx,argv[5]);

	script_value_to_string(cx,argv[6],name,name_str_len);
	
	idx=particle_find_index(name);
	if (idx==-1) {
		*exception=js_particle_name_exception(cx,name);
	}
	else {
		script_bool_to_value(cx,particle_spawn(idx,script_get_attached_object_uid(),&pt,NULL,&motion));
	}

	return(script_null_to_value(cx));
}

JSValueRef js_spawn_particle_line_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				idx,count;
	char			name[name_str_len];
	d3pnt			start_pt,end_pt;
	
	if (!script_check_param_count(cx,func,argc,8,exception)) return(script_null_to_value(cx));
	
	start_pt.x=script_value_to_int(cx,argv[0]);
	start_pt.z=script_value_to_int(cx,argv[1]);
	start_pt.y=script_value_to_int(cx,argv[2]);

	end_pt.x=script_value_to_int(cx,argv[3]);
	end_pt.z=script_value_to_int(cx,argv[4]);
	end_pt.y=script_value_to_int(cx,argv[5]);

	count=script_value_to_int(cx,argv[6]);

	script_value_to_string(cx,argv[7],name,name_str_len);
	
	idx=particle_find_index(name);
	if (idx==-1) {
		*exception=js_particle_name_exception(cx,name);
	}
	else {
		script_bool_to_value(cx,particle_line_spawn(idx,script_get_attached_object_uid(),&start_pt,&end_pt,count));
	}

	return(script_null_to_value(cx));
}

/* =======================================================

      Spawn Ring
      
======================================================= */

JSValueRef js_spawn_ring_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				idx;
	char			name[name_str_len];
	d3pnt			pt;
	
	if (!script_check_param_count(cx,func,argc,4,exception)) return(script_null_to_value(cx));
	
	pt.x=script_value_to_int(cx,argv[0]);
	pt.z=script_value_to_int(cx,argv[1]);
	pt.y=script_value_to_int(cx,argv[2]);
	
	script_value_to_string(cx,argv[3],name,name_str_len);
	
	idx=ring_find_index(name);
	if (idx==-1) {
		*exception=js_ring_name_exception(cx,name);
	}
	else {
		script_bool_to_value(cx,ring_spawn(idx,script_get_attached_object_uid(),&pt,NULL));
	}

	return(script_null_to_value(cx));
}

JSValueRef js_spawn_ring_line_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				idx,count;
	char			name[name_str_len];
	d3pnt			start_pt,end_pt;
	
	if (!script_check_param_count(cx,func,argc,8,exception)) return(script_null_to_value(cx));
	
	start_pt.x=script_value_to_int(cx,argv[0]);
	start_pt.z=script_value_to_int(cx,argv[1]);
	start_pt.y=script_value_to_int(cx,argv[2]);

	end_pt.x=script_value_to_int(cx,argv[3]);
	end_pt.z=script_value_to_int(cx,argv[4]);
	end_pt.y=script_value_to_int(cx,argv[5]);

	count=script_value_to_int(cx,argv[6]);

	script_value_to_string(cx,argv[7],name,name_str_len);
	
	idx=ring_find_index(name);
	if (idx==-1) {
		*exception=js_ring_name_exception(cx,name);
	}
	else {
		script_bool_to_value(cx,ring_line_spawn(idx,script_get_attached_object_uid(),&start_pt,&end_pt,count));
	}

	return(script_null_to_value(cx));
}

/* =======================================================

      Spawn Flash
      
======================================================= */

JSValueRef js_spawn_flash_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				intensity,flash_msec,fade_msec;
	d3pnt			pt;
	d3col			col;
	
	if (!script_check_param_count(cx,func,argc,9,exception)) return(script_null_to_value(cx));
	
	pt.x=script_value_to_int(cx,argv[0]);
	pt.z=script_value_to_int(cx,argv[1]);
	pt.y=script_value_to_int(cx,argv[2]);

	col.r=script_value_to_float(cx,argv[3]);
	col.g=script_value_to_float(cx,argv[4]);
	col.b=script_value_to_float(cx,argv[5]);

	intensity=script_value_to_int(cx,argv[6]);
	flash_msec=script_value_to_int(cx,argv[7]);
	fade_msec=script_value_to_int(cx,argv[8]);

	script_bool_to_value(cx,effect_spawn_flash(&pt,&col,intensity,1.0f,flash_msec,fade_msec));
    
	return(script_null_to_value(cx));
}

/* =======================================================

      Spawn Lightning
      
======================================================= */

JSValueRef js_spawn_lightning_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				wid,life_msec;
	float			varient;
	d3pnt			start_pt,end_pt;
	d3col			col;
	
	if (!script_check_param_count(cx,func,argc,12,exception)) return(script_null_to_value(cx));
	
	start_pt.x=script_value_to_int(cx,argv[0]);
	start_pt.z=script_value_to_int(cx,argv[1]);
	start_pt.y=script_value_to_int(cx,argv[2]);

	end_pt.x=script_value_to_int(cx,argv[3]);
	end_pt.z=script_value_to_int(cx,argv[4]);
	end_pt.y=script_value_to_int(cx,argv[5]);
	
	wid=script_value_to_int(cx,argv[6]);
	varient=script_value_to_float(cx,argv[7]);

	col.r=script_value_to_float(cx,argv[8]);
	col.g=script_value_to_float(cx,argv[9]);
	col.b=script_value_to_float(cx,argv[10]);

	life_msec=script_value_to_int(cx,argv[11]);

	script_bool_to_value(cx,effect_spawn_lightning(&start_pt,&end_pt,wid,varient,&col,life_msec));
    
	return(script_null_to_value(cx));
}

/* =======================================================

      Spawn Ray
      
======================================================= */

JSValueRef js_spawn_ray_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				wid,life_msec;
	d3pnt			start_pt,end_pt;
	d3col			col;
	
	if (!script_check_param_count(cx,func,argc,11,exception)) return(script_null_to_value(cx));
	
	start_pt.x=script_value_to_int(cx,argv[0]);
	start_pt.z=script_value_to_int(cx,argv[1]);
	start_pt.y=script_value_to_int(cx,argv[2]);

	end_pt.x=script_value_to_int(cx,argv[3]);
	end_pt.z=script_value_to_int(cx,argv[4]);
	end_pt.y=script_value_to_int(cx,argv[5]);
	
	wid=script_value_to_int(cx,argv[6]);

	col.r=script_value_to_float(cx,argv[7]);
	col.g=script_value_to_float(cx,argv[8]);
	col.b=script_value_to_float(cx,argv[9]);

	life_msec=script_value_to_int(cx,argv[10]);

	script_bool_to_value(cx,effect_spawn_ray(&start_pt,&end_pt,wid,&col,life_msec));
    
	return(script_null_to_value(cx));
}

JSValueRef js_spawn_ray_team_color_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				wid,life_msec,obj_idx;
	d3pnt			start_pt,end_pt;
	d3col			col;
	obj_type		*obj;
	
	if (!script_check_param_count(cx,func,argc,8,exception)) return(script_null_to_value(cx));
	
	start_pt.x=script_value_to_int(cx,argv[0]);
	start_pt.z=script_value_to_int(cx,argv[1]);
	start_pt.y=script_value_to_int(cx,argv[2]);

	end_pt.x=script_value_to_int(cx,argv[3]);
	end_pt.z=script_value_to_int(cx,argv[4]);
	end_pt.y=script_value_to_int(cx,argv[5]);
	
	wid=script_value_to_int(cx,argv[6]);

	life_msec=script_value_to_int(cx,argv[7]);

		// team color

	col.r=col.g=col.b=1.0f;

	obj_idx=script_get_attached_object_uid();
	if (obj_idx!=-1) {
		obj=server.obj_list.objs[obj_idx];
		if (obj!=NULL) object_get_tint(obj,&col);
	}

	script_bool_to_value(cx,effect_spawn_ray(&start_pt,&end_pt,wid,&col,life_msec));
    
	return(script_null_to_value(cx));
}

/* =======================================================

      Spawn Shake
      
======================================================= */

JSValueRef js_spawn_shake_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				dist,sz,life_msec;
	d3pnt			pt;
	
	if (!script_check_param_count(cx,func,argc,6,exception)) return(script_null_to_value(cx));
	
	pt.x=script_value_to_int(cx,argv[0]);
	pt.z=script_value_to_int(cx,argv[1]);
	pt.y=script_value_to_int(cx,argv[2]);
    
    dist=script_value_to_int(cx,argv[3]);
	
	sz=script_value_to_int(cx,argv[4]);
	life_msec=script_value_to_int(cx,argv[5]);
	
	script_bool_to_value(cx,effect_spawn_shake(&pt,dist,sz,life_msec));
    
	return(script_null_to_value(cx));
}

/* =======================================================

      Spawn Push
      
======================================================= */

JSValueRef js_spawn_push_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				x,z,y,radius,force;
	
	if (!script_check_param_count(cx,func,argc,5,exception)) return(script_null_to_value(cx));
	
	x=script_value_to_int(cx,argv[0]);
	z=script_value_to_int(cx,argv[1]);
	y=script_value_to_int(cx,argv[2]);
	
	radius=script_value_to_int(cx,argv[3]);
	force=script_value_to_int(cx,argv[4]);

	collide_push_objects(x,y,z,radius,force);
	return(script_bool_to_value(cx,TRUE));
}

