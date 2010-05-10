/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Scenery

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

#include "objects.h"
#include "models.h"
#include "consoles.h"

extern map_type				map;
extern server_type			server;

/* =======================================================

      Create Scenery Objects
      
======================================================= */

void scenery_create(void)
{
	int					n;
	char				err_str[256];
	map_scenery_type	*map_scenery;
	obj_type			*obj;
	model_type			*model;

	map_scenery=map.sceneries;

	for (n=0;n!=map.nscenery;n++) {
		map_scenery=&map.sceneries[n];
	
			// no object is no model
			
		if (map_scenery->model_name[0]==0x0) continue;

			// create new object

		obj=object_create(map_scenery->model_name,object_type_object,bt_map,-1);
		if (obj==NULL) break;

			// no scripts, events, etc

		obj->attach.script_uid=-1;

			// create as scenery

		obj->scenery.on=TRUE;
		obj->scenery.idx=n;

		obj->suspend=TRUE;
		obj->hidden=FALSE;

			// run the setup

		memmove(&obj->pnt,&map_scenery->pnt,sizeof(d3pnt));
		memmove(&obj->ang,&map_scenery->ang,sizeof(d3ang));

		obj->contact.object_on=map_scenery->contact_object_on;
		obj->contact.projectile_on=map_scenery->contact_projectile_on;
		obj->contact.force_on=FALSE;
		obj->hit_box.on=map_scenery->contact_hit_box;

		strcpy(obj->draw.name,map_scenery->model_name);
		
		obj->draw.on=TRUE;
		obj->draw.face_forward=map_scenery->face_forward;
		obj->draw.shadow.on=map_scenery->shadow;
		obj->draw.resize=map_scenery->resize;

			// needs current mesh

		object_set_current_mesh(obj);
		
			// load the model
			
		if (!model_load_and_init(&obj->draw,"Scenery",obj->name,err_str)) {
			console_add_error(err_str);
			object_dispose_single(object_find_index_uid(obj->uid));
			continue;
		}
		
			// check if there are no hit boxes
			// if so, turn off hit box only checking
			
		if (obj->hit_box.on) {
			model=model_find_uid(obj->draw.uid);
			if (model==NULL) {
				obj->hit_box.on=FALSE;
			}
			else {
				if (model->nhit_box==0) obj->hit_box.on=FALSE;
			}
		}
	}
}

/* =======================================================

      Start Scenery Animation and Sizes
      
======================================================= */

void scenery_start(void)
{
	int					n,k;
	map_scenery_type	*map_scenery;
	obj_type			*obj;

	obj=server.objs;

	for (n=0;n!=server.count.obj;n++) {
	
		if (obj->scenery.on) {
		
			map_scenery=&map.sceneries[obj->scenery.idx];
			
			obj->size.x=obj->draw.size.x;
			obj->size.y=obj->draw.size.y;
			obj->size.z=obj->draw.size.z;
		
			object_set_radius(obj);
			
				// change texture frames
				
			for (k=0;k!=max_map_scenery_model_texture_frame;k++) {
				obj->draw.cur_texture_frame[k]=(unsigned char)map_scenery->texture_frame[k];
			}
				
				// start scenery animation
				
			if (map_scenery->animation_name[0]!=0x0) model_start_animation(&obj->draw,map_scenery->animation_name);
		}
		
		obj++;
	}
}
