/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Alt Property Palette Ring

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
	#include "dim3setup.h"
#endif

#include "glue.h"
#include "ui_common.h"
#include "interface.h"

#define kRingSettingsName						0
#define kRingSettingsBitmapName					1
#define kRingSettingsLife						2

#define kRingRingStartOuterSize					3
#define kRingRingEndOuterSize					4
#define kRingRingStartInnerSize					5
#define kRingRingEndInnerSize					6
#define kRingRingStartAlpha						7
#define kRingRingEndAlpha						8
#define kRingRingStartColor						9
#define kRingRingEndColor						10

#define kRingMotionMove							11
#define kRingMotionRot							12
#define kRingMotionRotAdd						13
#define kRingMotionRotAccel						14

#define kRingOptionBlendAdd						15
#define kRingOptionTeamTint						16

#define kRingImageCount							17
#define kRingImageMillisecond					18
#define kRingImageLoop							19
#define kRingImageLoopBack						20

extern iface_type				iface;
extern setup_state_type			state;
extern list_palette_type		alt_property_palette;

/* =======================================================

      Property Palette Fill Ring
      
======================================================= */

void alt_property_palette_fill_ring(int ring_idx)
{
	iface_ring_type			*ring;

	ring=&iface.ring_list.rings[ring_idx];

	list_palette_set_sub_title(&alt_property_palette,"Ring",ring->name);

		// settings

	list_palette_add_header(&alt_property_palette,0,"Settings");
	list_palette_add_string(&alt_property_palette,kRingSettingsName,"Name",ring->name,FALSE);
	list_palette_add_string(&alt_property_palette,kRingSettingsBitmapName,"Bitmap",ring->bitmap_name,FALSE);
	list_palette_add_string_int(&alt_property_palette,kRingSettingsLife,"Life Milliseconds",ring->life_msec,FALSE);

		// ring

	list_palette_add_header(&alt_property_palette,0,"Ring");
	list_palette_add_string_int(&alt_property_palette,kRingRingStartOuterSize,"Start Outer Size",ring->start_outer_size,FALSE);
	list_palette_add_string_int(&alt_property_palette,kRingRingEndOuterSize,"End Outer Size",ring->end_outer_size,FALSE);
	list_palette_add_string_int(&alt_property_palette,kRingRingStartInnerSize,"Start Inner Size",ring->start_inner_size,FALSE);
	list_palette_add_string_int(&alt_property_palette,kRingRingEndInnerSize,"End Inner Size",ring->end_inner_size,FALSE);
	list_palette_add_string_float(&alt_property_palette,kRingRingStartAlpha,"Start Alpha",ring->start_alpha,FALSE);
	list_palette_add_string_float(&alt_property_palette,kRingRingEndAlpha,"End Alpha",ring->end_alpha,FALSE);
	list_palette_add_pick_color(&alt_property_palette,kRingRingStartColor,"Start Color",&ring->start_color,FALSE);
	list_palette_add_pick_color(&alt_property_palette,kRingRingEndColor,"End Color",&ring->end_color,FALSE);

		// motion

	list_palette_add_header(&alt_property_palette,0,"Motion");
	list_palette_add_vector(&alt_property_palette,kRingMotionMove,"Move",&ring->vct,FALSE);
	list_palette_add_angle(&alt_property_palette,kRingMotionRot,"Rotation",&ring->ang,FALSE);
	list_palette_add_angle(&alt_property_palette,kRingMotionRotAdd,"Rotation Add",&ring->rot,FALSE);
	list_palette_add_vector(&alt_property_palette,kRingMotionRotAccel,"Rotation Accel",&ring->rot_accel,FALSE);

		// options

	list_palette_add_header(&alt_property_palette,0,"Options");
	list_palette_add_checkbox(&alt_property_palette,kRingOptionBlendAdd,"Additive Blend",ring->blend_add,FALSE);
	list_palette_add_checkbox(&alt_property_palette,kRingOptionTeamTint,"Team Tint",ring->team_tint,FALSE);

		// animation

	list_palette_add_header(&alt_property_palette,0,"Animation");
	list_palette_add_string_int(&alt_property_palette,kRingImageCount,"Count",ring->animate.image_count,FALSE);
	list_palette_add_string_int(&alt_property_palette,kRingImageMillisecond,"Display Milliseconds",ring->animate.msec,FALSE);
	list_palette_add_checkbox(&alt_property_palette,kRingImageLoop,"Loop",ring->animate.loop,FALSE);
	list_palette_add_checkbox(&alt_property_palette,kRingImageLoopBack,"Loop Back",ring->animate.loop_back,FALSE);
}

/* =======================================================

      Property Palette Click Ring
      
======================================================= */

void alt_property_palette_click_ring(int ring_idx,int id)
{
	char					file_name[file_str_len];
	iface_ring_type			*ring;

	ring=&iface.ring_list.rings[ring_idx];

	switch (id) {

			// settings

		case kRingSettingsName:
			dialog_property_string_run(list_string_value_string,(void*)ring->name,name_str_len,0,0);
			break;

		case kRingSettingsBitmapName:
			strcpy(file_name,ring->bitmap_name);
			if (dialog_file_open_run("Pick a Ring Bitmap","Bitmaps/Rings","png",NULL,file_name)) strcpy(ring->bitmap_name,file_name);
			break;

		case kRingSettingsLife:
			dialog_property_string_run(list_string_value_positive_int,(void*)&ring->life_msec,0,0,0);
			break;

			// ring

		case kRingRingStartOuterSize:
			dialog_property_string_run(list_string_value_positive_int,(void*)&ring->start_outer_size,0,0,0);
			break;

		case kRingRingEndOuterSize:
			dialog_property_string_run(list_string_value_positive_int,(void*)&ring->end_outer_size,0,0,0);
			break;

		case kRingRingStartInnerSize:
			dialog_property_string_run(list_string_value_positive_int,(void*)&ring->start_inner_size,0,0,0);
			break;

		case kRingRingEndInnerSize:
			dialog_property_string_run(list_string_value_positive_int,(void*)&ring->end_inner_size,0,0,0);
			break;

		case kRingRingStartAlpha:
			dialog_property_string_run(list_string_value_0_to_1_float,(void*)&ring->start_alpha,0,0,0);
			break;

		case kRingRingEndAlpha:
			dialog_property_string_run(list_string_value_0_to_1_float,(void*)&ring->end_alpha,0,0,0);
			break;

		case kRingRingStartColor:
			os_pick_color(&ring->start_color);
			break;

		case kRingRingEndColor:
			os_pick_color(&ring->end_color);
			break;

			// motion

		case kRingMotionMove:
			dialog_property_chord_run(list_chord_value_vector,(void*)&ring->vct);
			break;

		case kRingMotionRot:
			dialog_property_chord_run(list_chord_value_angle,(void*)&ring->ang);
			break;

		case kRingMotionRotAdd:
			dialog_property_chord_run(list_chord_value_angle,(void*)&ring->rot);
			break;

		case kRingMotionRotAccel:
			dialog_property_chord_run(list_chord_value_vector,(void*)&ring->rot_accel);
			break;

			// options

		case kRingOptionBlendAdd:
			ring->blend_add=!ring->blend_add;
			break;

		case kRingOptionTeamTint:
			ring->team_tint=!ring->team_tint;
			break;

			// animation

		case kRingImageCount:
			dialog_property_string_run(list_string_value_positive_int,(void*)&ring->animate.image_count,0,0,0);
			break;

		case kRingImageMillisecond:
			dialog_property_string_run(list_string_value_positive_int,(void*)&ring->animate.msec,0,0,0);
			break;

		case kRingImageLoop:
			ring->animate.loop=!ring->animate.loop;
			break;

		case kRingImageLoopBack:
			ring->animate.loop_back=!ring->animate.loop_back;
			break;

	}

		// redraw

	main_wind_draw();
}

