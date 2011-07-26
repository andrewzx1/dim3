/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Setup

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
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

#define kSetupPropertyGameVideo				0
#define kSetupPropertyGameAudio				1
#define kSetupPropertyGameControl			2
#define kSetupPropertyGameAction			3
#define kSetupPropertyGameDebug				4

#define kSetupPropertyNetPlayer				10
#define kSetupPropertyNetHost				11

extern iface_type				iface;
extern setup_state_type			state;
extern list_palette_type		property_palette;

/* =======================================================

      Property Palette Fill Setup
      
======================================================= */

void property_palette_fill_setup(void)
{
	list_palette_set_title(&property_palette,"Setup");

		// game setup

	list_palette_add_header(&property_palette,0,"Game Setup");
	list_palette_add_checkbox(&property_palette,kSetupPropertyGameVideo,"Video Tab",iface.setup.game_video,FALSE);
	list_palette_add_checkbox(&property_palette,kSetupPropertyGameAudio,"Audio Tab",iface.setup.game_audio,FALSE);
	list_palette_add_checkbox(&property_palette,kSetupPropertyGameControl,"Control Tab",iface.setup.game_control,FALSE);
	list_palette_add_checkbox(&property_palette,kSetupPropertyGameAction,"Action Tab",iface.setup.game_action,FALSE);
	list_palette_add_checkbox(&property_palette,kSetupPropertyGameDebug,"Debug Tab",iface.setup.game_debug,FALSE);

		// net setup

	list_palette_add_header(&property_palette,0,"Net Setup");
	list_palette_add_checkbox(&property_palette,kSetupPropertyNetPlayer,"Player Tab",iface.setup.net_player,FALSE);
	list_palette_add_checkbox(&property_palette,kSetupPropertyNetHost,"Host Tab",iface.setup.net_host,FALSE);
}

/* =======================================================

      Property Palette Click Setup
      
======================================================= */

void property_palette_click_setup(int id)
{
	switch (id) {

			// game setup

		case kSetupPropertyGameVideo:
			iface.setup.game_video=!iface.setup.game_video;
			break;

		case kSetupPropertyGameAudio:
			iface.setup.game_audio=!iface.setup.game_audio;
			break;

		case kSetupPropertyGameControl:
			iface.setup.game_control=!iface.setup.game_control;
			break;

		case kSetupPropertyGameAction:
			iface.setup.game_action=!iface.setup.game_action;
			break;

		case kSetupPropertyGameDebug:
			iface.setup.game_debug=!iface.setup.game_debug;
			break;

			// net setup

		case kSetupPropertyNetPlayer:
			iface.setup.net_player=!iface.setup.net_player;
			break;

		case kSetupPropertyNetHost:
			iface.setup.net_host=!iface.setup.net_host;
			break;

	}

		// redraw

	main_wind_draw();
}

