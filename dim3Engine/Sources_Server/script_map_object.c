/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: map.object object

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
#include "objects.h"
#include "models.h"

extern server_type		server;
extern js_type			js;

JSValueRef js_map_object_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception);
bool js_map_object_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception);
JSValueRef js_map_object_find_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_find_player_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_find_all_players_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_nearest_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_nearest_skip_object_id_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_nearest_player_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_nearest_player_skip_object_id_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_nearest_remote_player_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_nearest_team_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_get_name_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_get_type_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_get_team_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_get_team_name_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_get_team_color_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_get_distance_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_get_angle_to_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_get_angle_to_id_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_is_facing_id_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_get_position_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_get_angle_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_get_size_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_get_health_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_is_dead_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_is_hidden_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_is_contact_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_is_max_health_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_get_last_damage_object_id_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_was_telefrag_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_move_to_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_shove_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_shove_direct_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_add_goal_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_set_contact_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_set_hidden_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_set_model_light_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_set_model_halo_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_set_model_mesh_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_spawn_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);
JSValueRef js_map_object_remove_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception);

script_js_function	map_object_functions[]={
							{"find",						js_map_object_find_func,							1},
							{"findPlayer",					js_map_object_find_player_func,						0},
							{"findAllPlayers",				js_map_object_find_all_players_func,				0},
							{"nearest",						js_map_object_nearest_func,							9},
							{"nearestSkipObjectId",			js_map_object_nearest_skip_object_id_func,			10},
							{"nearestPlayer",				js_map_object_nearest_player_func,					7},
							{"nearestPlayerSkipObjectId",	js_map_object_nearest_player_skip_object_id_func,	8},
							{"nearestRemotePlayer",			js_map_object_nearest_remote_player_func,			7},
							{"nearestTeam",					js_map_object_nearest_team_func,					8},
							{"getName",						js_map_object_get_name_func,						1},
							{"getType",						js_map_object_get_type_func,						1},
							{"getTeam",						js_map_object_get_team_func,						1},
							{"getTeamName",					js_map_object_get_team_name_func,					1},
							{"getTeamColor",				js_map_object_get_team_color_func,					1},
							{"getDistance",					js_map_object_get_distance_func,					4},
							{"getAngleTo",					js_map_object_get_angle_to_func,					4},
							{"getAngleToId",				js_map_object_get_angle_to_id_func,					2},
							{"isFacingId",					js_map_object_is_facing_id_func,					3},
							{"getPosition",					js_map_object_get_position_func,					1},
							{"getAngle",					js_map_object_get_angle_func,						1},
							{"getSize",						js_map_object_get_size_func,						1},
							{"getHealth",					js_map_object_get_health_func,						1},
							{"isDead",						js_map_object_is_dead_func,							1},
							{"isHidden",					js_map_object_is_hidden_func,						1},
							{"isContact",					js_map_object_is_contact_func,						1},
							{"isMaxHealth",					js_map_object_is_max_health_func,					1},
							{"getLastDamageObjectId",		js_map_object_get_last_damage_object_id_func,		1},
							{"wasTelefrag",					js_map_object_was_telefrag_func,					1},
							{"moveTo",						js_map_object_move_to_func,							5},
							{"shove",						js_map_object_shove_func,							5},
							{"shoveDirect",					js_map_object_shove_direct_func,					4},
							{"addGoal",						js_map_object_add_goal_func,						1},
							{"setContact",					js_map_object_set_contact_func,						2},
							{"setHidden",					js_map_object_set_hidden_func,						2},
							{"setModelLight",				js_map_object_set_model_light_func,					3},
							{"setModelHalo",				js_map_object_set_model_halo_func,					3},
							{"setModelMesh",				js_map_object_set_model_mesh_func,					3},
							{"spawn",						js_map_object_spawn_func,							10},
							{"remove",						js_map_object_remove_func,							1},
							{0}};

JSClassRef			map_object_class;

/* =======================================================

      Create Object
      
======================================================= */

void script_init_map_object_object(void)
{
	map_object_class=script_create_class("map_object_class",js_map_object_get_property,js_map_object_set_property);
}

void script_free_map_object_object(void)
{
	script_free_class(map_object_class);
}

JSObjectRef script_add_map_object_object(JSObjectRef parent_obj)
{
	return(script_create_child_object(parent_obj,map_object_class,"object",NULL,map_object_functions));
}

/* =======================================================

      Object Getter and Setter
      
======================================================= */

JSValueRef js_map_object_get_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef *exception)
{
	return(script_get_property(cx,j_obj,name,NULL));
}

bool js_map_object_set_property(JSContextRef cx,JSObjectRef j_obj,JSStringRef name,JSValueRef vp,JSValueRef *exception)
{
	return(script_set_property(cx,j_obj,name,vp,exception,NULL));
}

/* =======================================================

      Find Object Function
      
======================================================= */

JSValueRef js_map_object_find_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char				name[name_str_len];
	obj_type			*obj;
	
	script_value_to_string(argv[0],name,name_str_len);
	
	obj=object_find_name(name);
	if (obj==NULL) return(script_int_to_value(-1));

	return(script_int_to_value(obj->uid));
}

/* =======================================================

      Find Player Object Function
      
======================================================= */

JSValueRef js_map_object_find_player_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	return(script_int_to_value(server.player_obj_uid));
}

JSValueRef js_map_object_find_all_players_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int			n,cnt,uids[max_object];
	obj_type	*obj;

	cnt=0;
	obj=server.objs;

	for (n=0;n!=server.count.obj;n++) {
		if (obj->player) uids[cnt++]=obj->uid;
		obj++;
	}

	if (cnt==0) return(script_null_to_value());

	return(script_int_array_to_value(cnt,uids));
}

/* =======================================================

      Find Nearest Object Functions
      
======================================================= */

JSValueRef js_map_object_nearest_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int					min_dist,max_dist;
	char				*name_ptr,name[name_str_len],
						*type_ptr,type[name_str_len];
	float				ang,ang_sweep;
	d3pnt				pt;
	obj_type			*obj;
	
		// x,z,y
		
	pt.x=script_value_to_int(argv[0]);
	pt.z=script_value_to_int(argv[1]);
	pt.y=script_value_to_int(argv[2]);
	
		// name
		
	name_ptr=NULL;
	if (!script_is_value_null(argv[3])) {
		script_value_to_string(argv[3],name,name_str_len);
		name_ptr=name;
	}
	
		// type
		
	type_ptr=NULL;
	if (!script_is_value_null(argv[4])) {
		script_value_to_string(argv[4],type,name_str_len);
		type_ptr=type;
	}

		// angle and sweep
	
	ang=-1;
	ang_sweep=360;
	if (!script_is_value_null(argv[5])) ang=script_value_to_float(argv[5]);
	if (!script_is_value_null(argv[6])) ang_sweep=script_value_to_float(argv[6]);
	
		// distances
		
	min_dist=script_value_to_int(argv[7]);
	max_dist=script_value_to_int(argv[8]);
	
		// find object

	obj=object_find_nearest(&pt,name_ptr,type_ptr,-1,ang,ang_sweep,min_dist,max_dist,FALSE,FALSE,-1);
	if (obj==NULL) return(script_int_to_value(-1));
	
	return(script_int_to_value(obj->uid));
}

JSValueRef js_map_object_nearest_skip_object_id_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int					min_dist,max_dist;
	char				*name_ptr,name[name_str_len],
						*type_ptr,type[name_str_len];
	float				ang,ang_sweep;
	d3pnt				pt;
	obj_type			*obj;
	
		// x,z,y
		
	pt.x=script_value_to_int(argv[0]);
	pt.z=script_value_to_int(argv[1]);
	pt.y=script_value_to_int(argv[2]);
	
		// name
		
	name_ptr=NULL;
	if (!script_is_value_null(argv[3])) {
		script_value_to_string(argv[3],name,name_str_len);
		name_ptr=name;
	}
	
		// type
		
	type_ptr=NULL;
	if (!script_is_value_null(argv[4])) {
		script_value_to_string(argv[4],type,name_str_len);
		type_ptr=type;
	}

		// angle and sweep
	
	ang=-1;
	ang_sweep=360;
	if (!script_is_value_null(argv[5])) ang=script_value_to_float(argv[5]);
	if (!script_is_value_null(argv[6])) ang_sweep=script_value_to_float(argv[6]);
	
		// distances
		
	min_dist=script_value_to_int(argv[7]);
	max_dist=script_value_to_int(argv[8]);
	
		// find object

	obj=object_find_nearest(&pt,name_ptr,type_ptr,-1,ang,ang_sweep,min_dist,max_dist,FALSE,FALSE,script_value_to_int(argv[9]));
	if (obj==NULL) return(script_int_to_value(-1));
	
	return(script_int_to_value(obj->uid));
}

JSValueRef js_map_object_nearest_player_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int					min_dist,max_dist;
	float				ang,ang_sweep;
	d3pnt				pt;
	obj_type			*obj;
	
		// x,z,y
		
	pt.x=script_value_to_int(argv[0]);
	pt.z=script_value_to_int(argv[1]);
	pt.y=script_value_to_int(argv[2]);

		// angle and sweep
	
	ang=-1;
	ang_sweep=360;
	if (!script_is_value_null(argv[3])) ang=script_value_to_float(argv[3]);
	if (!script_is_value_null(argv[4])) ang_sweep=script_value_to_float(argv[4]);
	
		// distances
		
	min_dist=script_value_to_int(argv[5]);
	max_dist=script_value_to_int(argv[6]);
	
		// find object

	obj=object_find_nearest(&pt,NULL,NULL,-1,ang,ang_sweep,min_dist,max_dist,TRUE,FALSE,-1);
	if (obj==NULL) return(script_int_to_value(-1));
	
	return(script_int_to_value(obj->uid));
}

JSValueRef js_map_object_nearest_player_skip_object_id_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int					min_dist,max_dist;
	float				ang,ang_sweep;
	d3pnt				pt;
	obj_type			*obj;
	
		// x,z,y
		
	pt.x=script_value_to_int(argv[0]);
	pt.z=script_value_to_int(argv[1]);
	pt.y=script_value_to_int(argv[2]);

		// angle and sweep
	
	ang=-1;
	ang_sweep=360;
	if (!script_is_value_null(argv[3])) ang=script_value_to_float(argv[3]);
	if (!script_is_value_null(argv[4])) ang_sweep=script_value_to_float(argv[4]);
	
		// distances
		
	min_dist=script_value_to_int(argv[5]);
	max_dist=script_value_to_int(argv[6]);
	
		// find object

	obj=object_find_nearest(&pt,NULL,NULL,-1,ang,ang_sweep,min_dist,max_dist,TRUE,FALSE,script_value_to_int(argv[7]));
	if (obj==NULL) return(script_int_to_value(-1));
	
	return(script_int_to_value(obj->uid));
}

JSValueRef js_map_object_nearest_remote_player_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int					min_dist,max_dist;
	float				ang,ang_sweep;
	d3pnt				pt;
	obj_type			*obj;
	
		// x,z,y
		
	pt.x=script_value_to_int(argv[0]);
	pt.z=script_value_to_int(argv[1]);
	pt.y=script_value_to_int(argv[2]);

		// angle and sweep
	
	ang=-1;
	ang_sweep=360;
	if (!script_is_value_null(argv[3])) ang=script_value_to_float(argv[3]);
	if (!script_is_value_null(argv[4])) ang_sweep=script_value_to_float(argv[4]);
	
		// distances
		
	min_dist=script_value_to_int(argv[5]);
	max_dist=script_value_to_int(argv[6]);
	
		// find object

	obj=object_find_nearest(&pt,NULL,NULL,-1,ang,ang_sweep,min_dist,max_dist,TRUE,TRUE,-1);
	if (obj==NULL) return(script_int_to_value(-1));
	
	return(script_int_to_value(obj->uid));
}

JSValueRef js_map_object_nearest_team_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int					min_dist,max_dist,team_idx;
	float				ang,ang_sweep;
	d3pnt				pt;
	obj_type			*obj;
	
		// x,z,y
		
	pt.x=script_value_to_int(argv[0]);
	pt.z=script_value_to_int(argv[1]);
	pt.y=script_value_to_int(argv[2]);
	
		// team
		
	team_idx=script_value_to_int(argv[3])-sd_team_none;

		// angle and sweep
	
	ang=-1;
	ang_sweep=360;
	if (!script_is_value_null(argv[4])) ang=script_value_to_float(argv[4]);
	if (!script_is_value_null(argv[5])) ang_sweep=script_value_to_float(argv[5]);
	
		// distances
		
	min_dist=script_value_to_int(argv[6]);
	max_dist=script_value_to_int(argv[7]);
	
		// find object

	obj=object_find_nearest(&pt,NULL,NULL,team_idx,ang,ang_sweep,min_dist,max_dist,FALSE,FALSE,-1);
	if (obj==NULL) return(script_int_to_value(-1));
	
	return(script_int_to_value(obj->uid));
}

/* =======================================================

      Get Object Information
      
======================================================= */

JSValueRef js_map_object_get_name_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	
		// uid
	
	obj=script_find_obj_from_uid_arg(argv[0],exception);
	if (obj==NULL) return(script_null_to_value());

	return(script_string_to_value(obj->name));
}

JSValueRef js_map_object_get_type_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	
		// uid
	
	obj=script_find_obj_from_uid_arg(argv[0],exception);
	if (obj==NULL) return(script_null_to_value());

	return(script_string_to_value(obj->type));
}

JSValueRef js_map_object_get_team_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=script_find_obj_from_uid_arg(argv[0],exception);
	if (obj==NULL) return(script_null_to_value());

	return(script_int_to_value(obj->team_idx+sd_team_none));
}

JSValueRef js_map_object_get_team_name_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	char			str[32];
	obj_type		*obj;
	
	obj=script_find_obj_from_uid_arg(argv[0],exception);
	if (obj==NULL) return(script_null_to_value());
	
	object_team_get_name(obj->team_idx,str);
	return(script_string_to_value(str));
}

JSValueRef js_map_object_get_team_color_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	d3col			col;
	obj_type		*obj;
	
	obj=script_find_obj_from_uid_arg(argv[0],exception);
	if (obj==NULL) return(script_null_to_value());
	
	object_team_get_tint(obj->team_idx,&col);

	return(script_color_to_value(&col));
}

/* =======================================================

      Object Distance and Angle To
      
======================================================= */

JSValueRef js_map_object_get_distance_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				x,z,y;
	obj_type		*obj;
	
		// uid
	
	obj=script_find_obj_from_uid_arg(argv[0],exception);
	if (obj==NULL) return(script_null_to_value());
	
		// x,z,y
		
	x=script_value_to_int(argv[1]);
	z=script_value_to_int(argv[2]);
	y=script_value_to_int(argv[3]);
	
		// get distance
		
	return(script_int_to_value(distance_get(obj->pnt.x,obj->pnt.y,obj->pnt.z,x,y,z)));
}

JSValueRef js_map_object_get_angle_to_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				x,z,y;
	float			ang_x,ang_z,ang_y;
	obj_type		*obj;
	
		// uid
	
	obj=script_find_obj_from_uid_arg(argv[0],exception);
	if (obj==NULL) return(script_null_to_value());

		// x,z,y
		
	x=script_value_to_int(argv[1]);
	z=script_value_to_int(argv[2]);
	y=script_value_to_int(argv[3]);
	
		// get angles to
		
	ang_x=angle_find(y,z,obj->pnt.y,obj->pnt.z);
	ang_y=angle_find(x,z,obj->pnt.x,obj->pnt.z);
	ang_z=angle_find(x,y,obj->pnt.x,obj->pnt.y);
		
	return(script_angle_to_value(ang_x,ang_y,ang_z));
}

JSValueRef js_map_object_get_angle_to_id_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	float			ang_x,ang_z,ang_y;
	obj_type		*obj,*obj2;
	
		// uids
	
	obj=script_find_obj_from_uid_arg(argv[0],exception);
	if (obj==NULL) return(script_null_to_value());

	obj2=script_find_obj_from_uid_arg(argv[1],exception);
	if (obj2==NULL) return(script_null_to_value());
	
		// get angles to
		
	ang_x=angle_find(obj2->pnt.y,obj2->pnt.z,obj->pnt.y,obj->pnt.z);
	ang_y=angle_find(obj2->pnt.x,obj2->pnt.z,obj->pnt.x,obj->pnt.z);
	ang_z=angle_find(obj2->pnt.x,obj2->pnt.y,obj->pnt.x,obj->pnt.y);
		
	return(script_angle_to_value(ang_x,ang_y,ang_z));
}

/* =======================================================

      Facing
      
======================================================= */

JSValueRef js_map_object_is_facing_id_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	float			ang_y,ang_dif;
	bool			cwise;
	obj_type		*obj,*obj2;
	
		// uids
	
	obj=script_find_obj_from_uid_arg(argv[0],exception);
	if (obj==NULL) return(script_null_to_value());

	obj2=script_find_obj_from_uid_arg(argv[1],exception);
	if (obj2==NULL) return(script_null_to_value());
	
		// get angles to
		
	ang_y=angle_find(obj->pnt.x,obj->pnt.z,obj2->pnt.x,obj2->pnt.z);

	ang_dif=angle_dif(ang_y,obj->ang.y,&cwise);
	return(script_bool_to_value(ang_dif<script_value_to_float(argv[2])));
}

/* =======================================================

      Object Position, Angle, Size
      
======================================================= */

JSValueRef js_map_object_get_position_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;

		// uid
	
	obj=script_find_obj_from_uid_arg(argv[0],exception);
	if (obj==NULL) return(script_null_to_value());
	
		// get position
		
	return(script_point_to_value(obj->pnt.x,obj->pnt.y,obj->pnt.z));
}

JSValueRef js_map_object_get_angle_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;

		// uid
	
	obj=script_find_obj_from_uid_arg(argv[0],exception);
	if (obj==NULL) return(script_null_to_value());
	
		// get angle
		
	return(script_angle_to_value(obj->ang.x,obj->ang.y,obj->ang.z));
}

JSValueRef js_map_object_get_size_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;

		// uid
	
	obj=script_find_obj_from_uid_arg(argv[0],exception);
	if (obj==NULL) return(script_null_to_value());
	
		// get size
		
	return(script_point_to_value(obj->size.x,obj->size.y,obj->size.z));
}

JSValueRef js_map_object_get_health_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;

		// uid
	
	obj=script_find_obj_from_uid_arg(argv[0],exception);
	if (obj==NULL) return(script_null_to_value());
	
		// get size
		
	return(script_int_to_value(obj->status.health));
}

JSValueRef js_map_object_is_dead_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;

	obj=script_find_obj_from_uid_arg(argv[0],exception);
	if (obj==NULL) return(script_null_to_value());
		
	return(script_bool_to_value(obj->status.health<=0));
}

JSValueRef js_map_object_is_hidden_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;

	obj=script_find_obj_from_uid_arg(argv[0],exception);
	if (obj==NULL) return(script_null_to_value());
		
	return(script_bool_to_value(obj->hidden));
}

JSValueRef js_map_object_is_contact_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;

	obj=script_find_obj_from_uid_arg(argv[0],exception);
	if (obj==NULL) return(script_null_to_value());
		
	return(script_bool_to_value((obj->contact.object_on) || (obj->contact.projectile_on) || (obj->contact.force_on)));
}

JSValueRef js_map_object_is_max_health_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;

	obj=script_find_obj_from_uid_arg(argv[0],exception);
	if (obj==NULL) return(script_null_to_value());
		
	return(script_bool_to_value(obj->status.health>=obj->status.max_health));
}

JSValueRef js_map_object_get_last_damage_object_id_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;

	obj=script_find_obj_from_uid_arg(argv[0],exception);
	if (obj==NULL) return(script_null_to_value());
		
	return(script_int_to_value(obj->damage_obj_uid));
}

JSValueRef js_map_object_was_telefrag_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;

	obj=script_find_obj_from_uid_arg(argv[0],exception);
	if (obj==NULL) return(script_null_to_value());
		
	return(script_bool_to_value(obj->death_telefrag));
}

/* =======================================================

      Object Moving and Shoving
      
======================================================= */

JSValueRef js_map_object_move_to_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;

		// uid
	
	obj=script_find_obj_from_uid_arg(argv[0],exception);
	if (obj==NULL) return(script_null_to_value());

		// reposition

	object_set_position(obj,script_value_to_int(argv[1]),script_value_to_int(argv[3]),script_value_to_int(argv[2]),script_value_to_float(argv[4]),0);
	object_telefrag_players(obj,FALSE);

	return(script_null_to_value());
}

JSValueRef js_map_object_shove_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	float			speed;
	d3ang			ang;
	obj_type		*obj;

		// uid
	
	obj=script_find_obj_from_uid_arg(argv[0],exception);
	if (obj==NULL) return(script_null_to_value());
	
		// get angle
		
	ang.x=script_value_to_float(argv[1]);
	ang.z=script_value_to_float(argv[2]);
	ang.y=script_value_to_float(argv[3]);
	
	speed=script_value_to_float(argv[4]);
	
		// shove object
		
	object_shove(obj,&ang,speed);
	return(script_null_to_value());
}

JSValueRef js_map_object_shove_direct_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	d3vct			vct;
	obj_type		*obj;

		// uid
	
	obj=script_find_obj_from_uid_arg(argv[0],exception);
	if (obj==NULL) return(script_null_to_value());
	
		// get direction
		
	vct.x=script_value_to_float(argv[1]);
	vct.z=script_value_to_float(argv[2]);
	vct.y=script_value_to_float(argv[3]);
	
		// shove object
		
	object_shove_direct(obj,&vct);
	return(script_null_to_value());
}

/* =======================================================

      Object Scoring
      
======================================================= */

JSValueRef js_map_object_add_goal_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=script_find_obj_from_uid_arg(argv[0],exception);
	if (obj!=NULL) object_score_goal(obj);

	return(script_null_to_value());
}

/* =======================================================

      Object Setting
      
======================================================= */

JSValueRef js_map_object_set_contact_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	bool			on;
	obj_type		*obj;
	
	obj=script_find_obj_from_uid_arg(argv[0],exception);
	if (obj!=NULL) {
		on=script_value_to_bool(argv[1]);

		obj->contact.object_on=on;
		obj->contact.projectile_on=on;
		obj->contact.force_on=on;
	}

	return(script_null_to_value());
}

JSValueRef js_map_object_set_hidden_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	obj_type		*obj;
	
	obj=script_find_obj_from_uid_arg(argv[0],exception);
	if (obj!=NULL) obj->hidden=script_value_to_bool(argv[1]);

	return(script_null_to_value());
}

JSValueRef js_map_object_set_model_light_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int					idx;
	obj_type			*obj;
	
	obj=script_find_obj_from_uid_arg(argv[0],exception);
	if (obj!=NULL) {
		idx=script_value_to_int(argv[1]);
		if ((idx>=0) && (idx<max_model_light)) obj->draw.lights[idx].on=script_value_to_bool(argv[2]);
	}

	return(script_null_to_value());
}

JSValueRef js_map_object_set_model_halo_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int					idx;
	obj_type			*obj;
	
	obj=script_find_obj_from_uid_arg(argv[0],exception);
	if (obj!=NULL) {
		idx=script_value_to_int(argv[1]);
		if ((idx>=0) && (idx<max_model_halo)) obj->draw.halos[idx].on=script_value_to_bool(argv[2]);
	}

	return(script_null_to_value());
}

JSValueRef js_map_object_set_model_mesh_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int					idx;
	char				name[name_str_len];
	obj_type			*obj;
	
	obj=script_find_obj_from_uid_arg(argv[0],exception);
	if (obj==NULL) return(script_null_to_value());
	
	script_value_to_string(argv[1],name,name_str_len);
	idx=model_find_mesh_from_draw(&obj->draw,name);
	if (idx==-1) return(script_null_to_value());

	if (script_value_to_bool(argv[2])) {
		obj->draw.mesh_mask|=(0x1<<idx);
	}
	else {
		obj->draw.mesh_mask&=((0x1<<idx)^0xFF);
	}

	return(script_null_to_value());
}

/* =======================================================

      Spawning and Removing
      
======================================================= */

JSValueRef js_map_object_spawn_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				uid;
	char			name[name_str_len],type[name_str_len],err_str[256],
					script[name_str_len],params[256];
	d3pnt			pnt;
	d3ang			ang;

		// spawn values

	script_value_to_string(argv[0],name,name_str_len);
	script_value_to_string(argv[1],type,name_str_len);
	script_value_to_string(argv[2],script,name_str_len);
	script_value_to_string(argv[3],params,256);

	pnt.x=script_value_to_int(argv[4]);
	pnt.z=script_value_to_int(argv[5]);
	pnt.y=script_value_to_int(argv[6]);

	ang.x=script_value_to_float(argv[7]);
	ang.z=script_value_to_float(argv[8]);
	ang.y=script_value_to_float(argv[9]);

		// spawn

	uid=object_script_spawn(name,type,script,params,&pnt,&ang,FALSE,err_str);
	if (uid==-1) {
		*exception=script_create_exception(err_str);
		return(script_null_to_value());
	}

		// return UID

	return(script_int_to_value(uid));
}

JSValueRef js_map_object_remove_func(JSContextRef cx,JSObjectRef func,JSObjectRef j_onj,size_t argc,const JSValueRef argv[],JSValueRef *exception)
{
	int				uid;
	char			err_str[256];
	obj_type		*obj;

	obj=script_find_obj_from_uid_arg(argv[0],exception);
	if (obj==NULL) return(script_null_to_value());

	uid=obj->uid;

	if (!object_script_remove(uid,err_str)) {
		*exception=script_create_exception(err_str);
	}

	return(script_null_to_value());
}

