/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Property Palette Map Camera

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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "ui_common.h"
#include "interface.h"

#define kMapPropertyCameraMode				0
#define kMapPropertyCameraAngle				1

#define kMapPropertyCameraFOV				10
#define kMapPropertyCameraAspectRatio		11
#define kMapPropertyCameraNearZ				12
#define kMapPropertyCameraFarZ				13
#define kMapPropertyCameraNearZOffset		14

#define kMapPropertyCameraChaseDistance		20
#define kMapPropertyCameraChaseTrackSpeed	21
#define kMapPropertyCameraChaseSlop			22

#define kMapPropertyCameraStaticFollow		30
#define kMapPropertyCameraStaticAttachNode	31

extern map_type					map;
extern editor_state_type		state;
extern editor_setup_type		setup;

extern list_palette_type		property_palette;

char							map_property_camera_mode_list[][name_str_len]={"FPP","Chase","Static","Chase Static",""};

/* =======================================================

      Property Palette Fill Map Camera
      
======================================================= */

void property_palette_fill_camera(void)
{
	list_palette_set_title(&property_palette,"Map Camera");

		// camera settings

	list_palette_add_header(&property_palette,0,"Map Camera Settings");
	list_palette_add_string(&property_palette,kMapPropertyCameraMode,"Mode",map_property_camera_mode_list[map.camera.mode],FALSE);
	list_palette_add_angle(&property_palette,kMapPropertyCameraAngle,"Offset Angle",&map.camera.ang,FALSE);

		// camera plane

	list_palette_add_header(&property_palette,0,"Map Camera Plane");
	list_palette_add_string_float(&property_palette,kMapPropertyCameraFOV,"FOV",map.camera.plane.fov,FALSE);
	list_palette_add_string_float(&property_palette,kMapPropertyCameraAspectRatio,"Aspect Ratio",map.camera.plane.aspect_ratio,FALSE);
	list_palette_add_string_int(&property_palette,kMapPropertyCameraNearZ,"Near Z",map.camera.plane.near_z,FALSE);
	list_palette_add_string_int(&property_palette,kMapPropertyCameraFarZ,"Far Z",map.camera.plane.far_z,FALSE);
	list_palette_add_string_int(&property_palette,kMapPropertyCameraNearZOffset,"Near Z Offset",map.camera.plane.near_z_offset,FALSE);

		// camera chase

	list_palette_add_header(&property_palette,0,"Map Camera Chase");
	list_palette_add_string_int(&property_palette,kMapPropertyCameraChaseDistance,"Distance",map.camera.chase.distance,FALSE);
	list_palette_add_string_float(&property_palette,kMapPropertyCameraChaseTrackSpeed,"Track Speed",map.camera.chase.track_speed,FALSE);
	list_palette_add_angle(&property_palette,kMapPropertyCameraChaseSlop,"Slop",&map.camera.chase.slop,FALSE);

		// camera static

	list_palette_add_header(&property_palette,0,"Map Camera Static");
	list_palette_add_checkbox(&property_palette,kMapPropertyCameraStaticFollow,"Follow Player",map.camera.c_static.follow,FALSE);
	list_palette_add_string(&property_palette,kMapPropertyCameraStaticAttachNode,"Attach Node",map.camera.c_static.attach_node,FALSE);
}

/* =======================================================

      Property Palette Click Map Camera
      
======================================================= */

void property_palette_click_camera(int id,bool double_click)
{
	if (!double_click) return;

	switch (id) {

			// camera settings

		case kMapPropertyCameraMode:
			property_pick_list("Pick a Camera Mode",(char*)map_property_camera_mode_list,&map.camera.mode);
			break;
			
		case kMapPropertyCameraAngle:
			dialog_property_chord_run(list_chord_value_angle,(void*)&map.camera.ang);
			break;

			// camera plane
			
		case kMapPropertyCameraFOV:
			dialog_property_string_run(list_string_value_positive_float,(void*)&map.camera.plane.fov,0,0,0);
			break;

		case kMapPropertyCameraAspectRatio:
			dialog_property_string_run(list_string_value_positive_float,(void*)&map.camera.plane.aspect_ratio,0,0,0);
			break;

		case kMapPropertyCameraNearZ:
			dialog_property_string_run(list_string_value_positive_int,(void*)&map.camera.plane.near_z,0,0,0);
			break;

		case kMapPropertyCameraFarZ:
			dialog_property_string_run(list_string_value_positive_int,(void*)&map.camera.plane.far_z,0,0,0);
			break;

		case kMapPropertyCameraNearZOffset:
			dialog_property_string_run(list_string_value_int,(void*)&map.camera.plane.near_z_offset,0,0,0);
			break;

			// camera chase settings

		case kMapPropertyCameraChaseDistance:
			dialog_property_string_run(list_string_value_positive_int,(void*)&map.camera.chase.distance,0,0,0);
			break;

		case kMapPropertyCameraChaseTrackSpeed:
			dialog_property_string_run(list_string_value_positive_float,(void*)&map.camera.chase.track_speed,0,0,0);
			break;

		case kMapPropertyCameraChaseSlop:
			dialog_property_chord_run(list_chord_value_angle,(void*)&map.camera.chase.slop);
			break;

			// camera static settings

		case kMapPropertyCameraStaticFollow:
			map.camera.c_static.follow=!map.camera.c_static.follow;
			break;

		case kMapPropertyCameraStaticAttachNode:
			property_palette_pick_node(map.camera.c_static.attach_node);
			break;
	
	}

	main_wind_draw();
}
