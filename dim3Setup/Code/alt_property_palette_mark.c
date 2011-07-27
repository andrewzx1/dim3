/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Alt Property Palette Mark

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

#define kMarkSettingsName						0
#define kMarkSettingsFileName					1

#define kMarkOptionNoRotate						2
#define kMarkOptionNoTransparent				3
#define kMarkOptionNoOpaque						4
#define kMarkOptionHilite						5
#define kMarkOptionBlendAdd						6
#define kMarkOptionTeamTint						7
#define kMarkOptionColor						8

#define kMarkFadeInTick							9
#define kMarkFadeLifeTick						10
#define kMarkFadeOutTick						11

#define kMarkImageCount							12
#define kMarkImageMillisecond					13
#define kMarkImageLoop							14
#define kMarkImageLoopBack						15

extern iface_type				iface;
extern setup_state_type			state;
extern list_palette_type		alt_property_palette;

/* =======================================================

      Property Palette Fill Mark
      
======================================================= */

void alt_property_palette_fill_mark(int mark_idx)
{
	iface_mark_type			*mark;

	mark=&iface.mark_list.marks[mark_idx];

	list_palette_set_sub_title(&alt_property_palette,"Mark",mark->name);

		// settings

	list_palette_add_header(&alt_property_palette,0,"Settings");
	list_palette_add_string(&alt_property_palette,kMarkSettingsName,"Name",mark->name,FALSE);
	list_palette_add_string(&alt_property_palette,kMarkSettingsFileName,"Bitmap",mark->bitmap_name,FALSE);

		// options

	list_palette_add_header(&alt_property_palette,0,"Options");
	list_palette_add_checkbox(&alt_property_palette,kMarkOptionNoRotate,"No Rotate",mark->no_rotate,FALSE);
	list_palette_add_checkbox(&alt_property_palette,kMarkOptionNoTransparent,"Ignore Transaparent Polys",mark->no_transparent,FALSE);
	list_palette_add_checkbox(&alt_property_palette,kMarkOptionNoOpaque,"Ignore Opaque Polys",mark->no_opaque,FALSE);
	list_palette_add_checkbox(&alt_property_palette,kMarkOptionHilite,"High Light",mark->hilite,FALSE);
	list_palette_add_checkbox(&alt_property_palette,kMarkOptionBlendAdd,"Additive Blending",mark->blend_add,FALSE);
	list_palette_add_checkbox(&alt_property_palette,kMarkOptionTeamTint,"Team Tinting",mark->team_tint,FALSE);
	list_palette_add_pick_color(&alt_property_palette,kMarkOptionColor,"Color",&mark->color,FALSE);

		// fade

	list_palette_add_header(&alt_property_palette,0,"Fade");
	list_palette_add_string_int(&alt_property_palette,kMarkFadeInTick,"Fade In Milliseconds",mark->fade_in_msec,FALSE);
	list_palette_add_string_int(&alt_property_palette,kMarkFadeLifeTick,"Life Milliseconds",mark->life_msec,FALSE);
	list_palette_add_string_int(&alt_property_palette,kMarkFadeOutTick,"Fade Out Milliseconds",mark->fade_out_msec,FALSE);

		// animation

	list_palette_add_header(&alt_property_palette,0,"Animation");
	list_palette_add_string_int(&alt_property_palette,kMarkImageCount,"Count",mark->animate.image_count,FALSE);
	list_palette_add_string_int(&alt_property_palette,kMarkImageMillisecond,"Display Milliseconds",mark->animate.msec,FALSE);
	list_palette_add_checkbox(&alt_property_palette,kMarkImageLoop,"Loop",mark->animate.loop,FALSE);
	list_palette_add_checkbox(&alt_property_palette,kMarkImageLoopBack,"Loop Back",mark->animate.loop_back,FALSE);
}

/* =======================================================

      Property Palette Click Mark
      
======================================================= */

void alt_property_palette_click_mark(int mark_idx,int id)
{
	iface_mark_type			*mark;

	mark=&iface.mark_list.marks[mark_idx];

	switch (id) {

			// settings

		case kMarkSettingsName:
			dialog_property_string_run(list_string_value_string,(void*)mark->name,name_str_len,0,0);
			break;

		case kMarkSettingsFileName:
			property_pick_file("Pick a Mark Bitmap","Bitmaps/Marks","png",NULL,mark->bitmap_name);
			break;

			// options

		case kMarkOptionNoRotate:
			mark->no_rotate=!mark->no_rotate;
			break;

		case kMarkOptionNoTransparent:
			mark->no_transparent=!mark->no_transparent;
			break;

		case kMarkOptionNoOpaque:
			mark->no_opaque=!mark->no_opaque;
			break;

		case kMarkOptionHilite:
			mark->hilite=!mark->hilite;
			break;

		case kMarkOptionBlendAdd:
			mark->blend_add=!mark->blend_add;
			break;

		case kMarkOptionTeamTint:
			mark->team_tint=!mark->team_tint;
			break;

		case kMarkOptionColor:
			os_pick_color(&mark->color);
			break;

			// fade

		case kMarkFadeInTick:
			dialog_property_string_run(list_string_value_positive_int,(void*)&mark->fade_in_msec,0,0,0);
			break;

		case kMarkFadeLifeTick:
			dialog_property_string_run(list_string_value_positive_int,(void*)&mark->life_msec,0,0,0);
			break;

		case kMarkFadeOutTick:
			dialog_property_string_run(list_string_value_positive_int,(void*)&mark->fade_out_msec,0,0,0);
			break;

			// animation

		case kMarkImageCount:
			dialog_property_string_run(list_string_value_positive_int,(void*)&mark->animate.image_count,0,0,0);
			break;

		case kMarkImageMillisecond:
			dialog_property_string_run(list_string_value_positive_int,(void*)&mark->animate.msec,0,0,0);
			break;

		case kMarkImageLoop:
			mark->animate.loop=!mark->animate.loop;
			break;

		case kMarkImageLoopBack:
			mark->animate.loop_back=!mark->animate.loop_back;
			break;

	}

		// redraw

	main_wind_draw();
}

