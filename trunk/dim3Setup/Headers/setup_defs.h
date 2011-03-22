/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Main Defines for Setup

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

//
// Menus
//

#define app_menu_apple					128
#define app_menu_file					129

#define kCommandAbout					FOUR_CHAR_CODE('abot')

#define kCommandFileQuit				FOUR_CHAR_CODE('quit')

//
// Item types
//

#define item_interface					0
#define item_interface_settings			1
#define item_interface_hud				2
#define item_interface_intro			3
#define item_interface_menu				4
#define item_interface_choosers			5
#define item_interface_radar			6
#define item_interface_multiplayer		7

#define item_sound						1000

#define item_particle					2000

#define item_ring						3000

#define item_halo						4000

#define item_mark						5000

#define item_crosshair					6000

#define item_action						7000

//
// setup state
//

typedef struct		{
						int							cur_item,cur_idx;
					} setup_state_type;


