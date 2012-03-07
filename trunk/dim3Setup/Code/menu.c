/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Menus

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

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3setup.h"
#endif

#include "glue.h"
#include "interface.h"

extern iface_type				iface;

/* =======================================================

      Menu Enable/Disable
      
======================================================= */

void menu_update(void)
{
}

/* =======================================================

      Menu Save
      
======================================================= */

bool menu_save(void)
{
	char		err_str[256];

	if (iface_write(&iface,err_str)) return(TRUE);
	
	os_dialog_alert("Could not save setup files",err_str);
	return(FALSE);
}

bool menu_quit_save(void)
{
	int			choice;

	choice=os_dialog_confirm("Save Changes?","Do you want to save the changes to this project?",TRUE);
	if (choice==1) return(FALSE);
	
	if (choice==0) return(menu_save());
	
	return(TRUE);
}

/* =======================================================

      Menu Events
      
======================================================= */

bool menu_event_run(int cmd)
{
	switch (cmd) {
	
			// apple menu
   
        case kCommandAbout:
			dialog_about_run();
            return(TRUE);
		
			// file menu
			
		case kCommandFileSave:
			menu_save();
			return(TRUE);						

		case kCommandFileQuit:
			if (menu_quit_save()) os_application_quit();
			return(TRUE);
							
	}

	return(FALSE);
}


