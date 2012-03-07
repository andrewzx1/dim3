/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Property Palette Scenery

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

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
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

int								pal_scenery_index;

/* =======================================================

      Property Palette Fill Scenery
      
======================================================= */

void property_palette_fill_scenery(int scenery_idx)
{
	int							n;
	char						name[256];
	map_scenery_type			*scenery;

	scenery=&map.sceneries[scenery_idx];

	list_palette_set_title(&property_palette,"Scenery",scenery->model_name,NULL,NULL,NULL,NULL);

	list_palette_add_header(&property_palette,0,"Scenery Model");
	list_palette_add_picker_file(&property_palette,kSceneryPropertyModelName,list_button_none,0,"Model","Models","","Mesh.xml;Model.xml",scenery->model_name,FALSE);
	list_palette_add_string(&property_palette,kSceneryPropertyAnimationName,"Animation",scenery->animation_name,FALSE);
	list_palette_add_float(&property_palette,kSceneryPropertyResize,"Resize",&scenery->resize,FALSE);

	list_palette_add_header(&property_palette,0,"Scenery Settings");
	list_palette_add_checkbox(&property_palette,kSceneryPropertyContactObject,"Contact Object",&scenery->contact_object_on,FALSE);
	list_palette_add_checkbox(&property_palette,kSceneryPropertyContactProjectile,"Contact Projectile",&scenery->contact_projectile_on,FALSE);
	list_palette_add_checkbox(&property_palette,kSceneryPropertyContactHitBox,"Use Hit Box",&scenery->contact_hit_box,FALSE);
	list_palette_add_checkbox(&property_palette,kSceneryPropertyFaceFront,"Face Front",&scenery->face_forward,FALSE);
	list_palette_add_checkbox(&property_palette,kSceneryPropertyShadow,"Shadow",&scenery->shadow,FALSE);
	
	list_palette_add_header(&property_palette,0,"Scenery Mesh Frames");
	for (n=0;n!=max_map_scenery_model_texture_frame;n++) {
		sprintf(name,"Mesh %d",n);
		list_palette_add_int(&property_palette,(kSceneryPropertyFramesStart+n),name,&scenery->texture_frame[n],FALSE);
	}
	
	pal_scenery_index=scenery_idx;
	
	list_palette_add_header(&property_palette,0,"Scenery Info");
	list_palette_add_int(&property_palette,-1,"Index",&pal_scenery_index,TRUE);
	list_palette_add_point(&property_palette,-1,"Position",&scenery->pnt,TRUE);
	list_palette_add_angle(&property_palette,-1,"Angle",&scenery->ang,TRUE);
}

/* =======================================================

      Property Palette Click Scenery
      
======================================================= */

void property_palette_click_scenery(int scenery_idx,bool double_click)
{
	map_scenery_type			*scenery;

	if (!double_click) return;

	scenery=&map.sceneries[scenery_idx];

	switch (property_palette.item_pane.click.id) {

		case kSceneryPropertyAnimationName:
			dialog_property_string_run(list_string_value_string,(void*)scenery->animation_name,name_str_len,0,0);
			break;

	}
}

