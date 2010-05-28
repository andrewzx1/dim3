/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Projectile Setups

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
#include "weapons.h"
#include "projectiles.h"
#include "models.h"
#include "effects.h"
#include "consoles.h"

extern server_type			server;
extern js_type				js;

extern int mark_find(char *name);

/* =======================================================

      Find Projectile Setup
      
======================================================= */

proj_setup_type* find_proj_setups(weapon_type *weap,char *name)
{
	int				n;
	proj_setup_type	*proj_setup;

	for (n=0;n!=max_proj_setup_list;n++) {
		proj_setup=weap->proj_setup_list.proj_setups[n];
		if (proj_setup==NULL) continue;
		
		if (strcasecmp(proj_setup->name,name)==0) return(proj_setup);
	}
	
	return(NULL);
}

/* =======================================================

      Create and Dispose Projectile Setup
      
======================================================= */

bool proj_setup_create(obj_type *obj,weapon_type *weap,char *name)
{
	int					n,idx;
	char				err_str[256];
	proj_setup_type		*proj_setup;
	
		// find a free proj setup
		
	idx=-1;
	
	for (n=0;n!=max_proj_setup_list;n++) {
		if (weap->proj_setup_list.proj_setups[n]==NULL) {
			idx=n;
			break;
		}
	}
	
	if (idx==-1) return(FALSE);

		// create memory for new projectile setup
		
	proj_setup=(proj_setup_type*)malloc(sizeof(proj_setup_type));
	if (proj_setup==NULL) return(FALSE);

		// initialize projectile setup
	
	proj_setup->idx=idx;
	
	proj_setup->obj_idx=obj->idx;
	proj_setup->weap_idx=weap->idx;
	
	strcpy(proj_setup->name,name);
	
	proj_setup->mark.on=FALSE;
	proj_setup->mark.size=map_enlarge;
	proj_setup->mark.alpha=1;
	proj_setup->mark.idx=-1;
	proj_setup->mark.name[0]=0x0;

	proj_setup->speed=map_enlarge;
	proj_setup->decel_speed=0;
	proj_setup->decel_grace=0;
	proj_setup->decel_min_speed=0;
	proj_setup->inherit_motion_factor=0.0f;
	
	proj_setup->collision=FALSE;
	proj_setup->reset_angle=FALSE;
	proj_setup->damage=0;
	
	proj_setup->action.hit_tick=0;
	proj_setup->action.bounce=FALSE;
	proj_setup->action.bounce_min_move=10.0f;
	proj_setup->action.bounce_reduce=1.0f;
	proj_setup->action.reflect=FALSE;

	object_clear_size(&proj_setup->size);
	
	proj_setup->hitscan.on=FALSE;
	proj_setup->hitscan.max_dist=map_enlarge*100;
	
	proj_setup->push.on=FALSE;
	proj_setup->push.force=0;
	
	proj_setup->melee.strike_bone_tag=model_null_tag;
	proj_setup->melee.strike_pose_name[0]=0x0;
	proj_setup->melee.radius=0;
	proj_setup->melee.distance=0;
	proj_setup->melee.damage=0;
	proj_setup->melee.force=0;
	proj_setup->melee.fall_off=TRUE;
	
	object_clear_draw(&proj_setup->draw);
	
		// add to list
		
	weap->proj_setup_list.proj_setups[idx]=proj_setup;
	
		// start the script
		// and load the models
		
	proj_setup->attach.script_uid=-1;
	proj_setup->attach.thing_type=thing_type_projectile_setup;
	proj_setup->attach.obj_idx=obj->idx;
	proj_setup->attach.weap_idx=weap->idx;
	proj_setup->attach.proj_idx=-1;
	proj_setup->attach.proj_setup_idx=idx;

	scripts_clear_attach_data(&proj_setup->attach);

	if (scripts_add(&proj_setup->attach,"Projectiles",proj_setup->name,NULL,err_str)) {
		if (model_draw_load(&proj_setup->draw,"Projectile",proj_setup->name,err_str)) {
			return(TRUE);
		}
	}
	
		// there was an error
		// clean up and remove this projectile setup
	
	console_add_error(err_str);
	
	free(proj_setup);
	weap->proj_setup_list.proj_setups[idx]=NULL;
	
	return(FALSE);
}

void proj_setup_dispose(weapon_type *weap,int idx)
{
	proj_setup_type		*proj_setup;

	proj_setup=weap->proj_setup_list.proj_setups[idx];
	if (proj_setup==NULL) return;

		// clear scripts and models

	scripts_dispose(proj_setup->attach.script_uid);
	model_draw_dispose(&proj_setup->draw);
	
		// free and empty from list
		
	free(proj_setup);
	weap->proj_setup_list.proj_setups[idx]=NULL;
}

/* =======================================================

      Projectile Radius
      
======================================================= */

void proj_setup_set_radius(proj_setup_type *proj_setup)
{
	int			radius;
	
	radius=proj_setup->size.x;
	if (proj_setup->size.z>radius) radius=proj_setup->size.z;
	
	proj_setup->size.radius=radius>>1;
}

/* =======================================================

      Attaching Mark
      
======================================================= */

void proj_setup_attach_mark(proj_setup_type *proj_setup)
{
	proj_setup->mark.idx=mark_find(proj_setup->mark.name);
}

/* =======================================================

      Get Script Projectile Setup
      
======================================================= */

proj_setup_type* proj_setup_get_attach(void)
{
	weapon_type			*weap;
	proj_type			*proj;
	
	if (js.attach.thing_type==thing_type_projectile_setup) {
		weap=weapon_script_lookup();
		return(weap->proj_setup_list.proj_setups[js.attach.proj_setup_idx]);
	}
	
	if (js.attach.thing_type==thing_type_projectile) {
		weap=weapon_script_lookup();
		proj=projectile_script_lookup();
		return(weap->proj_setup_list.proj_setups[proj->proj_setup_idx]);
	}
	
	return(NULL);
}

proj_type* proj_get_attach(void)
{
	if (js.attach.thing_type==thing_type_projectile) {
		return(projectile_script_lookup());
	}
	
	return(NULL);
}

