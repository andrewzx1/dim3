/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Weapon Creation/Dispose/Find

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit �Created with dim3 Technology� is given on a single
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
#include "weapons.h"
#include "models.h"
#include "interfaces.h"
#include "consoles.h"

extern server_type			server;

/* =======================================================

      Initialize Weapon List
      
======================================================= */

void weapon_initialize_list(void)
{
	server.weapons=NULL;
	server.count.weapon=0;
	server.uid.weapon=0;
}

/* =======================================================

      Find Weapon
      
======================================================= */

weapon_type* weapon_find_uid(int uid)
{
	int				n;
	weapon_type		*weap;

	weap=server.weapons;
	
	for (n=0;n!=server.count.weapon;n++) {
		if (weap->uid==uid) return(weap);
		weap++;
	}
	
	return(NULL);
}

int weapon_index_find_uid(int uid)
{
	int				n;
	weapon_type		*weap;

	weap=server.weapons;
	
	for (n=0;n!=server.count.weapon;n++) {
		if (weap->uid==uid) return(n);
		weap++;
	}
	
	return(-1);
}

weapon_type* weapon_find_name(obj_type *obj,char *name)
{
	int				n,obj_uid;
	weapon_type		*weap;

	obj_uid=obj->uid;
	weap=server.weapons;
	
	for (n=0;n!=server.count.weapon;n++) {
		if (weap->obj_uid==obj_uid) {
			if (strcasecmp(weap->name,name)==0) return(weap);
		}
		weap++;
	}
	
	return(NULL);
}

weapon_type* weapon_find_current(obj_type *obj)
{
	return(weapon_find_uid(obj->held_weapon.current_uid));
}

weapon_type* weapon_find_offset(obj_type *obj,int offset)
{
	int				n,obj_uid;
	weapon_type		*weap;

	obj_uid=obj->uid;
	weap=server.weapons;
	
	for (n=0;n!=server.count.weapon;n++) {
	
		if (weap->obj_uid==obj_uid) {
			offset--;
			if (offset<0) return(weap);
		}
		
		weap++;
	}
	
	return(NULL);
}

int weapon_held_count(obj_type *obj)
{
	int				n,obj_uid,count;
	weapon_type		*weap;

	count=0;
	
	obj_uid=obj->uid;
	weap=server.weapons;
	
	for (n=0;n!=server.count.weapon;n++) {
		if ((weap->obj_uid==obj_uid) && (!weap->hidden)) count++;		
		weap++;
	}
	
	return(count);
}

/* =======================================================

      Initial Ammo
      
======================================================= */

void weapon_clear_ammo(weap_ammo_type *ammo,bool use)
{
	ammo->use_ammo=use;
	ammo->use_clips=FALSE;
	ammo->init_count=10;
	ammo->max_count=100;
	ammo->init_clip_count=0;
	ammo->max_clip_count=0;
	ammo->last_reload_tick=-1;
}

/* =======================================================

      Add Weapon to List
      
======================================================= */

bool weapon_create(obj_type *obj,char *name)
{
	weapon_type		*weap,*ptr;
	
		// only allow a maximum number of weapons

	if (server.count.weapon>=max_weapon) return(FALSE);

		// create memory for new weapon

	ptr=(weapon_type*)malloc(sizeof(weapon_type)*(server.count.weapon+1));
	if (ptr==NULL) return(FALSE);

	if (server.weapons!=NULL) {
		memmove(ptr,server.weapons,(sizeof(weapon_type)*server.count.weapon));
		free(server.weapons);
	}

	server.weapons=ptr;

	weap=&server.weapons[server.count.weapon];
	server.count.weapon++;

		// initialize weapon
	
	weap->uid=server.uid.weapon;
	server.uid.weapon++;
	
	weap->obj_uid=obj->uid;
	
	weap->hidden=FALSE;
	weap->fail_in_liquid=FALSE;
	
	strcpy(weap->name,name);
	weap->ammo.reset=TRUE;

	weapon_clear_ammo(&weap->ammo,TRUE);
	weapon_clear_ammo(&weap->alt_ammo,FALSE);
	
	weap->proj.fire_bone_tag=model_null_tag;
	weap->proj.barrel_bone_tag=model_null_tag;
	weap->proj.fire_pose_name[0]=0x0;
	weap->proj.object_fire_bone_tag=model_null_tag;
	weap->proj.object_fire_pose_name[0]=0x0;
	weap->proj.repeat_on=FALSE;
	weap->proj.repeat_ok=TRUE;
	weap->proj.repeat_tick=1000;
	weap->proj.next_repeat_tick=0;

	weap->target.distance=25000;
	weap->target.on=FALSE;
	weap->target.col.r=weap->target.col.g=0.0f;
	weap->target.col.b=1.0f;
	
	weap->zoom.on=FALSE;
	weap->zoom.mode=zoom_mode_off;
	weap->zoom.fov_min=10.0f;
	weap->zoom.fov_max=30.0f;
	weap->zoom.step_count=3;
	weap->zoom.current_step=0;
	weap->zoom.turn_factor=0.3f;
	weap->zoom.crawl_turn_factor=0.1f;
	weap->zoom.look_factor=0.5f;
	weap->zoom.mask_name[0]=0x0;
	weap->zoom.mask_idx=-1;
	weap->zoom.show_weapon=FALSE;
	weap->zoom.tick=500;
	
	weap->crosshair.on=FALSE;
	weap->crosshair.type=ct_center;
	weap->crosshair.min_size=16;
	weap->crosshair.max_size=64;
	weap->crosshair.distance=map_enlarge*250;
	weap->crosshair.col.r=weap->crosshair.col.g=1;
	weap->crosshair.col.b=0;
	weap->crosshair.empty_col.r=weap->crosshair.empty_col.g=weap->crosshair.empty_col.b=0.5;
	weap->crosshair.pickup_col.r=1;
	weap->crosshair.pickup_col.g=weap->crosshair.pickup_col.b=0;
	weap->crosshair.fire_name[0]=0x0;
	weap->crosshair.fire_idx=-1;
	weap->crosshair.bone_tag=model_null_tag;
	weap->crosshair.bone_idx=-1;
	
	weap->hand.shift.x=0;
	weap->hand.shift.y=0;
	weap->hand.shift.z=0;
	weap->hand.ang.x=0;
	weap->hand.ang.y=0;
	weap->hand.ang.z=0;
	
	weap->hand.raise_tick=500;
	weap->hand.lower_tick=500;
	weap->hand.select_shift=map_enlarge<<1;
	weap->hand.bounce_ang=30.0f;
	weap->hand.bounce_speed=5.0f;
	
	weap->kickback.life_msec=0;
	weap->kickback.size=0;

	weap->recoil.min_ang.x=weap->recoil.min_ang.y=weap->recoil.min_ang.z=0.0f;
	weap->recoil.max_ang.x=weap->recoil.max_ang.y=weap->recoil.max_ang.z=0.0f;
	weap->recoil.reset_ang.x=weap->recoil.reset_ang.y=weap->recoil.reset_ang.z=0.01f;
	weap->recoil.look_reset=0.05f;
	weap->recoil.turn_reset=0.05f;

	weapon_recoil_clear(weap);
	
	weap->melee.strike_bone_tag=model_null_tag;
	weap->melee.strike_pose_name[0]=0x0;
	weap->melee.object_strike_bone_tag=model_null_tag;
	weap->melee.object_strike_pose_name[0]=0x0;
	weap->melee.radius=0;
	weap->melee.distance=0;
	weap->melee.damage=0;
	weap->melee.force=0;
	weap->melee.fall_off=TRUE;
	
	weap->fire.last_fire_tick=-1;
	weap->fire.last_fire_dual_tick=-1;
	
	weap->dual.on=FALSE;
	weap->dual.active=FALSE;
	weap->dual.in_dual=FALSE;
	weap->dual.hand_offset=0;

	object_clear_draw(&weap->draw);
	object_clear_draw(&weap->draw_dual);
	
	weap->attach.script_uid=-1;
	
	return(TRUE);
}

/* =======================================================

      Attaching Crosshairs and Masks
      
======================================================= */

void weapon_attach_fire_crosshair(weapon_type *weap)
{
	weap->crosshair.fire_idx=crosshair_find(weap->crosshair.fire_name);
}

void weapon_attach_zoom_mask(weapon_type *weap)
{
	weap->zoom.mask_idx=crosshair_find(weap->zoom.mask_name);
}

/* =======================================================

      Start/Dispose Weapons
      
======================================================= */

bool weapon_start(weapon_type *weap)
{
	char		err_str[256];

	weap->attach.thing_type=thing_type_weapon;
	weap->attach.thing_uid=weap->uid;
	
	scripts_clear_attach_data(&weap->attach);

	if (!scripts_add(&weap->attach,"Weapons",weap->name,NULL,err_str)) {
		console_add_error(err_str);
		return(FALSE);
	}

	if (!model_load_and_init(&weap->draw,"Weapon",weap->name,err_str)) {
		console_add_error(err_str);
		return(FALSE);
	}

	memmove(&weap->draw_dual,&weap->draw,sizeof(model_draw));		// make sure dual is setup the same
	
	weapon_reset_ammo(weap);

	return(TRUE);
}

void weapon_dispose(int idx)
{
	weapon_type		*weap,*ptr;

	weap=&server.weapons[idx];

		// clear weapon

	scripts_dispose(weap->attach.script_uid);
	models_dispose(&weap->draw);

		// is the list completely empty?

	if (server.count.weapon==1) {
		free(server.weapons);
		server.weapons=NULL;
		server.count.weapon=0;
		return;
	}

		// if for some reason we can't create new
		// memory, just shuffle the list and wait
		// until next time

	ptr=(weapon_type*)malloc(sizeof(weapon_type)*(server.count.weapon-1));

	if (ptr==NULL) {
		if (idx<(server.count.weapon-1)) {
			memmove(&server.weapons[idx],&server.weapons[idx+1],(sizeof(weapon_type)*((server.count.weapon-idx)-1)));
		}
	}
	else {

		if (idx>0) {
			memmove(ptr,server.weapons,(sizeof(weapon_type)*idx));
		}
		if (idx<(server.count.weapon-1)) {
			memmove(&ptr[idx],&server.weapons[idx+1],(sizeof(weapon_type)*((server.count.weapon-idx)-1)));
		}

		free(server.weapons);
		server.weapons=ptr;
	}
	
	server.count.weapon--;
}

