/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Main Model

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
	server.count.model=0;
	server.uid.model=0;
}

/* =======================================================

      Find Models
      
======================================================= */

int model_find_uid_index(int uid)
{
	int				i;
	model_type		*mdl;

	mdl=server.models;
	
	for (i=0;i!=server.count.model;i++) {
		if (mdl->uid==uid) return(i);
		mdl++;
	}
	
	return(-1);
}

model_type* model_find_uid(int uid)
{
	int				i;
	model_type		*mdl;

	mdl=server.models;
	
	for (i=0;i!=server.count.model;i++) {
		if (mdl->uid==uid) return(mdl);
		mdl++;
	}
	
	return(NULL);
}

model_type* model_find(char *name)
{
	int				i;
	model_type		*mdl;

	mdl=server.models;
	
	for (i=0;i!=server.count.model;i++) {
		if (strcasecmp(mdl->name,name)==0) return(mdl);
		mdl++;
	}
	
	return(NULL);
}

/* =======================================================

      Write Model Shader Errors to Console
      
======================================================= */

void model_shader_errors_write_console(model_type *model)
{
	int				n;
	texture_type	*texture;
	
	texture=model->textures;
	
	for (n=0;n!=max_model_texture;n++) {
		if ((texture->shader.on) && (texture->shader.init_error[0]!=0x0)) console_add_error(texture->shader.init_error);
		texture++;
	}
}

/* =======================================================

      Open Models
      
======================================================= */

model_type* model_load_single(char *name)
{
	int					n;
	bool				load_shaders;
	model_type			*mdl;
	
		// has model been already loaded?
		// if so, return model and increment reference count
	
	mdl=model_find(name);
	if (mdl!=NULL) {
		mdl->reference_count++;
		return(mdl);
	}
	
		// new model
		
	if (server.count.model>=max_model) return(NULL);
	
	mdl=&server.models[server.count.model];
	
		// load model

	load_shaders=gl_check_shader_ok();
	
	model_setup(&setup.file_path_setup,setup.anisotropic_mode,setup.texture_quality_mode,setup.mipmap_mode,setup.mipmap_card_generated,setup.texture_compression);
	if (!model_open(mdl,name,TRUE,TRUE,load_shaders)) return(NULL);

		// deal with shader errors or shaders turned off

	if (!load_shaders) {
		for (n=0;n!=max_model_texture;n++) {
			mdl->textures[n].shader.on=FALSE;
		}
	}
	else {
		model_shader_errors_write_console(mdl);
	}

		// setup some animation indexes to avoid name lookups

	model_animation_effect_setup(mdl);

		// start reference count at 1

	mdl->reference_count=1;
	
		// new unique ID
		
	mdl->uid=server.uid.model;
	server.uid.model++;
	
	server.count.model++;
	
	return(mdl);
}

void model_load_and_init(model_draw *draw)
{
	char				err_str[256];
	model_type			*mdl;
	
	draw->uid=-1;
	draw->center.x=draw->center.y=draw->center.z=0;
	draw->size.x=draw->size.y=draw->size.z=0;
	
	if (draw->name[0]!=0x0) {
		mdl=model_load_single(draw->name);
		if (mdl!=NULL) {
			draw->uid=mdl->uid;
			model_get_size(mdl,&draw->size.x,&draw->size.y,&draw->size.z);
			memmove(&draw->center,&mdl->center,sizeof(d3pnt));
		}
		else {
			draw->on=FALSE;
			sprintf(err_str,"Unable to load model named %s",draw->name);
			console_add_error(err_str);
		}
	}
	else {
		draw->on=FALSE;
	}
	
	model_stop_animation(draw);
	model_fade_clear(draw);
}

/* =======================================================

      Dispose Models
      
======================================================= */

void models_dispose(int uid)
{
	int					idx;
	model_type			*model;

		// find model index

	idx=model_find_uid_index(uid);
	if (idx==-1) return;

		// decrement reference count

	model=&server.models[idx];
		
	model->reference_count--;
	if (model->reference_count>0) return;

		// dispose

	model_close(model);
	
	if (idx<(server.count.model-1)) {
		memmove(&server.models[idx],&server.models[idx+1],(sizeof(model_type)*((server.count.model-idx)-1)));
	}
		
	server.count.model--;
}

/* =======================================================

      Reset Model UIDs after Load
      
======================================================= */

void models_reset_uid_single(model_draw *draw)
{
	model_type			*mdl;

	mdl=model_find(draw->name);
	if (mdl==NULL) {
		draw->on=FALSE;
		return;
	}

	draw->uid=mdl->uid;
}

void models_reset_uid(void)
{
	int					i;
	obj_type			*obj;
	weapon_type			*weap;
	proj_setup_type		*proj_setup;
	proj_type			*proj;
	
	obj=server.objs;
	
	for (i=0;i!=server.count.obj;i++) {
		models_reset_uid_single(&obj->draw);
		obj++;
	}

	weap=server.weapons;
	
	for (i=0;i!=server.count.weapon;i++) {
		models_reset_uid_single(&weap->draw);
		weap++;
	}

	proj_setup=server.proj_setups;
	
    for (i=0;i!=server.count.proj_setup;i++) {
		models_reset_uid_single(&proj_setup->draw);
		proj_setup++;
	}

	proj=server.projs;
	
    for (i=0;i!=server.count.proj;i++) {
		models_reset_uid_single(&proj->draw);
		proj++;
	}
}

