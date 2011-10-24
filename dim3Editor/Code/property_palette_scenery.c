/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Property Palette Scenery

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
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "ui_common.h"
#include "interface.h"

#define kSceneryPropertyModelName				0
#define kSceneryPropertyAnimationName			1
#define kSceneryPropertyResize					2
#define kSceneryPropertyContactObject			3
#define kSceneryPropertyContactProjectile		4
#define kSceneryPropertyContactHitBox			5
#define kSceneryPropertyFaceFront				6
#define kSceneryPropertyShadow					7

#define kSceneryPropertyFramesStart				10
#define kSceneryPropertyFramesEnd				17

extern map_type					map;
extern editor_state_type		state;
extern editor_setup_type		setup;

extern list_palette_type		property_palette;

/* =======================================================

      Property Palette Fill Scenery
      
======================================================= */

void property_palette_fill_scenery(int scenery_idx)
{
	int							n;
	char						name[256];
	map_scenery_type			*scenery;

	scenery=&map.sceneries[scenery_idx];

	list_palette_set_sub_title(&property_palette,"Scenery",scenery->model_name);

	list_palette_add_header(&property_palette,0,"Scenery Model");
	list_palette_add_string(&property_palette,kSceneryPropertyModelName,"Model",scenery->model_name,FALSE);
	list_palette_add_string(&property_palette,kSceneryPropertyAnimationName,"Animation",scenery->animation_name,FALSE);
	list_palette_add_string_float(&property_palette,kSceneryPropertyResize,"Resize",scenery->resize,FALSE);

	list_palette_add_header(&property_palette,0,"Scenery Settings");
	list_palette_add_checkbox(&property_palette,kSceneryPropertyContactObject,"Contact Object",&scenery->contact_object_on,FALSE);
	list_palette_add_checkbox(&property_palette,kSceneryPropertyContactProjectile,"Contact Projectile",&scenery->contact_projectile_on,FALSE);
	list_palette_add_checkbox(&property_palette,kSceneryPropertyContactHitBox,"Use Hit Box",&scenery->contact_hit_box,FALSE);
	list_palette_add_checkbox(&property_palette,kSceneryPropertyFaceFront,"Face Front",&scenery->face_forward,FALSE);
	list_palette_add_checkbox(&property_palette,kSceneryPropertyShadow,"Shadow",&scenery->shadow,FALSE);
	
	list_palette_add_header(&property_palette,0,"Scenery Mesh Frames");
	for (n=0;n!=max_map_scenery_model_texture_frame;n++) {
		sprintf(name,"Mesh %d",n);
		list_palette_add_string_int(&property_palette,(kSceneryPropertyFramesStart+n),name,(int)scenery->texture_frame[n],FALSE);
	}
	
	list_palette_add_header(&property_palette,0,"Scenery Info");
	list_palette_add_string_int(&property_palette,-1,"Index",scenery_idx,TRUE);
	list_palette_add_point(&property_palette,-1,"Position",&scenery->pnt,TRUE);
	list_palette_add_angle(&property_palette,-1,"Angle",&scenery->ang,TRUE);
}

/* =======================================================

      Property Palette Click Scenery
      
======================================================= */

void property_palette_click_scenery(int scenery_idx,int id,bool double_click)
{
	int							frame_idx,frame;
	map_scenery_type			*scenery;

	if (!double_click) return;

	scenery=&map.sceneries[scenery_idx];

		// parameters

	if ((id>=kSceneryPropertyFramesStart) && (id<=kSceneryPropertyFramesEnd)) {
		frame_idx=(id-kSceneryPropertyFramesStart);
		
		frame=(int)scenery->texture_frame[frame_idx];
		dialog_property_string_run(list_string_value_positive_int,(void*)&frame,0,0,0);
		scenery->texture_frame[frame_idx]=(char)frame;

		return;
	}

		// regular properties

	switch (id) {

		case kSceneryPropertyModelName:
			property_pick_file("Pick a Model","Models",NULL,"Mesh.xml",scenery->model_name);
			break;

		case kSceneryPropertyAnimationName:
			dialog_property_string_run(list_string_value_string,(void*)scenery->animation_name,name_str_len,0,0);
			break;
			
		case kSceneryPropertyResize:
			dialog_property_string_run(list_string_value_positive_float,(void*)&scenery->resize,0,0,0);
			break;

	}
}

