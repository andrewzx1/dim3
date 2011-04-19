/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Alt Property Palette Halo

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

#define kHaloSettingsName						0
#define kHaloSettingsFileName					1

#define kHaloDistanceMin						2
#define kHaloDistanceMax						3
#define kHaloSizeMin							4
#define kHaloSizeMax							5
#define kHaloAlphaMin							6
#define kHaloAlphaMax							7
#define kHaloOptionNoClipObject					8
#define kHaloOptionNoClipSelf					9

extern iface_type				iface;
extern setup_state_type			state;
extern list_palette_type		alt_property_palette;

/* =======================================================

      Property Palette Fill Halo
      
======================================================= */

/*
typedef struct		{
						int								image_idx,
														min_dist,max_dist,
														min_size,max_size;
						float							min_alpha,max_alpha;
						bool							no_clip_object,no_clip_self;
						char							name[name_str_len],bitmap_name[name_str_len];
					} iface_halo_type;
*/

void alt_property_palette_fill_halo(int halo_idx)
{
	iface_halo_type			*halo;

	halo=&iface.halo_list.halos[halo_idx];

		// settings

	list_palette_add_header(&alt_property_palette,0,"Settings");
	list_palette_add_string(&alt_property_palette,kHaloSettingsName,"Name",halo->name,FALSE);
	list_palette_add_string(&alt_property_palette,kHaloSettingsFileName,"Bitmap",halo->bitmap_name,FALSE);

		// distance

	list_palette_add_header(&alt_property_palette,0,"Distance");
	list_palette_add_string_int(&alt_property_palette,kHaloDistanceMin,"Minimum",halo->min_dist,FALSE);
	list_palette_add_string_int(&alt_property_palette,kHaloDistanceMax,"Maximum",halo->max_dist,FALSE);

		// size

	list_palette_add_header(&alt_property_palette,0,"Size");
	list_palette_add_string_int(&alt_property_palette,kHaloSizeMin,"Minimum",halo->min_size,FALSE);
	list_palette_add_string_int(&alt_property_palette,kHaloSizeMax,"Maximum",halo->max_size,FALSE);

		// alpha

	list_palette_add_header(&alt_property_palette,0,"Alpha");
	list_palette_add_string_float(&alt_property_palette,kHaloAlphaMin,"Minimum",halo->min_alpha,FALSE);
	list_palette_add_string_float(&alt_property_palette,kHaloAlphaMax,"Maximum",halo->max_alpha,FALSE);

		// options

	list_palette_add_header(&alt_property_palette,0,"Options");
	list_palette_add_checkbox(&alt_property_palette,kHaloOptionNoClipObject,"No Clip Object",halo->no_clip_object,FALSE);
	list_palette_add_checkbox(&alt_property_palette,kHaloOptionNoClipSelf,"No Clip Self",halo->no_clip_self,FALSE);
}

/* =======================================================

      Property Palette Click Halo
      
======================================================= */

void alt_property_palette_click_halo(int halo_idx,int id)
{
	char					file_name[file_str_len];
	iface_halo_type			*halo;

	halo=&iface.halo_list.halos[halo_idx];

	switch (id) {

			// settings

		case kHaloSettingsName:
			dialog_property_string_run(list_string_value_string,(void*)halo->name,name_str_len,0,0);
			break;

		case kHaloSettingsFileName:
			strcpy(file_name,halo->bitmap_name);
			if (dialog_file_open_run("Pick a Halo Bitmap","Bitmaps/Halos","png",NULL,file_name)) strcpy(halo->bitmap_name,file_name);
			break;

			// distance

		case kHaloDistanceMin:
			dialog_property_string_run(list_string_value_positive_int,(void*)&halo->min_dist,0,0,0);
			break;

		case kHaloDistanceMax:
			dialog_property_string_run(list_string_value_positive_int,(void*)&halo->max_dist,0,0,0);
			break;

			// size

		case kHaloSizeMin:
			dialog_property_string_run(list_string_value_positive_int,(void*)&halo->min_size,0,0,0);
			break;

		case kHaloSizeMax:
			dialog_property_string_run(list_string_value_positive_int,(void*)&halo->max_size,0,0,0);
			break;

			// alpha

		case kHaloAlphaMin:
			dialog_property_string_run(list_string_value_0_to_1_float,(void*)&halo->min_alpha,0,0,0);
			break;

		case kHaloAlphaMax:
			dialog_property_string_run(list_string_value_0_to_1_float,(void*)&halo->max_alpha,0,0,0);
			break;

			// options

		case kHaloOptionNoClipObject:
			halo->no_clip_object=!halo->no_clip_object;
			break;

		case kHaloOptionNoClipSelf:
			halo->no_clip_self=!halo->no_clip_self;
			break;

	}

		// redraw

	main_wind_draw();
}

