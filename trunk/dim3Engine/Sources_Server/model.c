/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Main Model

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

#include "projectiles.h"
#include "models.h"
#include "consoles.h"
#include "video.h"

extern server_type		server;
extern setup_type		setup;

/* =======================================================

      Start Model Data
      
======================================================= */

void model_initialize(void)
{
	server.models=NULL;
	server.count.model=0;
	server.uid.model=0;
}

/* =======================================================

      Find Models
      
======================================================= */

int model_find_uid_index(int uid)
{
	int				n;
	model_type		*mdl;

	mdl=server.models;
	
	for (n=0;n!=server.count.model;n++) {
		if (mdl->uid==uid) return(n);
		mdl++;
	}
	
	return(-1);
}

model_type* model_find_uid(int uid)
{
	int				n;
	model_type		*mdl;

	mdl=server.models;
	
	for (n=0;n!=server.count.model;n++) {
		if (mdl->uid==uid) return(mdl);
		mdl++;
	}

	return(NULL);
}

model_type* model_find(char *name)
{
	int				n;
	model_type		*mdl;

	mdl=server.models;
	
	for (n=0;n!=server.count.model;n++) {
		if (strcasecmp(mdl->name,name)==0) return(mdl);
		mdl++;
	}
	
	return(NULL);
}

int model_find_index(char *name)
{
	int				n;
	model_type		*mdl;

	mdl=server.models;
	
	for (n=0;n!=server.count.model;n++) {
		if (strcasecmp(mdl->name,name)==0) return(n);
		mdl++;
	}
	
	return(-1);
}

/* =======================================================

      Open Models
      
======================================================= */

int model_load(char *name)
{
	int				idx;
	model_type		*mdl,*ptr;

		// has model been already loaded?
		// if so, return model and increment reference count
	
	idx=model_find_index(name);
	if (idx!=-1) {
		server.models[idx].reference_count++;
		return(idx);
	}

		// only allow a maximum number of models

	if (server.count.model>=max_model) return(NULL);

		// create memory for new model

	ptr=(model_type*)malloc(sizeof(model_type)*(server.count.model+1));
	if (ptr==NULL) return(NULL);

	if (server.models!=NULL) {
		memmove(ptr,server.models,(sizeof(model_type)*server.count.model));
		free(server.models);
	}

	server.models=ptr;

	idx=server.count.model;

	mdl=&server.models[idx];
	server.count.model++;

		// load model

	model_setup(&setup.file_path_setup,setup.anisotropic_mode,setup.mipmap_mode,setup.texture_quality_mode,setup.compress_on,TRUE);

	if (!model_open(mdl,name,TRUE)) {
		server.count.model--;		// error loading, leave memory as is an fix next model load
		return(-1);
	}

	gl_shader_attach_model(mdl);

		// setup some animation indexes to avoid name lookups

	model_animation_effect_setup(mdl);

		// start reference count at 1

	mdl->reference_count=1;

		// new unique ID
		
	mdl->uid=server.uid.model;
	server.uid.model++;
	
	return(idx);
}

bool model_draw_load(model_draw *draw,char *item_type,char *item_name,char *err_str)
{
	int					n;
	bool				ok;
	model_type			*mdl;

	draw->model_idx=-1;
	draw->center.x=draw->center.y=draw->center.z=0;
	draw->size.x=draw->size.y=draw->size.z=0;

	draw->script_animation_idx=0;
	draw->script_halo_idx=0;
	draw->script_light_idx=0;
	
	ok=TRUE;
	mdl=NULL;
	
	if (draw->name[0]!=0x0) {
		draw->model_idx=model_load(draw->name);
		if (draw->model_idx!=-1) {
			mdl=&server.models[draw->model_idx];
			model_get_size(mdl,&draw->size.x,&draw->size.y,&draw->size.z);
			memmove(&draw->center,&mdl->center,sizeof(d3pnt));
		}
		else {
			draw->on=FALSE;
			sprintf(err_str,"Unable to load model named %s for %s: %s",draw->name,item_type,item_name);
			ok=FALSE;
		}
	}
	else {
		draw->on=FALSE;
	}
	
		// initialize draw memory
		
	if (mdl!=NULL) model_draw_setup_initialize(mdl,&draw->setup,TRUE);

		// stop all animations and fades

	for (n=0;n!=max_model_blend_animation;n++) {
		draw->script_animation_idx=n;
		model_stop_animation(draw);
	}

	draw->script_animation_idx=0;

	model_fade_clear(draw);

	return(ok);
}

/* =======================================================

      Dispose Models
      
======================================================= */

void models_dispose(int idx)
{
	int					idx;
	model_type			*mdl,*ptr;

		// find model
		
	mdl=&server.models[idx];

		// decrement reference count

	mdl->reference_count--;
	if (mdl->reference_count>0) return;

		// dispose model

	model_close(mdl);

		// is the list completely empty?

	if (server.count.model==1) {
		free(server.models);
		server.models=NULL;
		server.count.model=0;
		return;
	}

		// if for some reason we can't create new
		// memory, just shuffle the list and wait
		// until next time

	ptr=(model_type*)malloc(sizeof(model_type)*(server.count.model-1));

	if (ptr==NULL) {
		if (idx<(server.count.model-1)) {
			memmove(&server.models[idx],&server.models[idx+1],(sizeof(model_type)*((server.count.model-idx)-1)));
		}
	}
	else {

		if (idx>0) {
			memmove(ptr,server.models,(sizeof(model_type)*idx));
		}
		if (idx<(server.count.model-1)) {
			memmove(&ptr[idx],&server.models[idx+1],(sizeof(model_type)*((server.count.model-idx)-1)));
		}

		free(server.models);
		server.models=ptr;
	}
	
	server.count.model--;
}

void models_draw_dispose(model_draw *draw)
{
		// find model
		
	if (draw->model_idx==-1) return;
	
		// clear draw memory
		
	model_draw_setup_shutdown(&server.models[draw->model_idx],&draw->setup);

		// dispose model

	model_dispose(draw->model_idx);
}

/* =======================================================

      Reset Model UIDs after Load
      
======================================================= */

void models_reset_single(model_draw *draw)
{
	draw->model_idx=model_find_index(draw->name);

		// if model is loaded, update
		// reference count

	if (draw->model_idx!=-1) {
		server.models[draw->model_idx].reference_count++;
	}

		// try to load it

	else {
		draw->model_idx=model_load(draw->name);
		if (draw->model_idx==-1) draw->on=FALSE;
	}

		// and create the draw setup memory

	if (draw->model_idx!=-1) {
		model_draw_setup_initialize(&server.models[draw->model_idx],&draw->setup,TRUE);
	}
}

void models_reset(void)
{
	int					n;
	obj_type			*obj;
	weapon_type			*weap;
	proj_setup_type		*proj_setup;
	proj_type			*proj;
	model_type			*mdl;

		// will need to reset all model
		// reference counts, we don't know
		// where they are left off after load

	mdl=server.models;
	
	for (n=0;n!=server.count.model;n++) {
		mdl->reference_count=0;
		mdl++;
	}

		// fix model indexes
	
	obj=server.objs;
	
	for (n=0;n!=server.count.obj;n++) {
		models_reset_single(&obj->draw);
		obj++;
	}

	weap=server.weapons;
	
	for (n=0;n!=server.count.weapon;n++) {
		models_reset_single(&weap->draw);
		weap++;
	}

	proj_setup=server.proj_setups;
	
    for (n=0;n!=server.count.proj_setup;n++) {
		models_reset_single(&proj_setup->draw);
		proj_setup++;
	}

	proj=server.projs;
	
    for (n=0;n!=server.count.proj;n++) {
		models_reset_single(&proj->draw);
		proj++;
	}

		// now remove all models with zero
		// reference count, our loaded file
		// isn't using them

	n=0;

	while (n<server.count.model) {
		mdl=&server.models[n];
		if (mdl->reference_count<=0) {
			models_dispose(n);
			continue;
		}
		n++;
	}
}

