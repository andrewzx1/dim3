/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Save Dialog

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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#include "interface.h"
#include "dialog.h"

extern map_type				map;

/* =======================================================

      Run Save Dialog
      
======================================================= */

bool dialog_save_run(void)
{
	bool				dialog_save_ok;

	SetThemeCursor(kThemeWatchCursor);

	dialog_progress_start("Saving...",2);
	
	dialog_progress_next();
	node_path_rebuild();
	
	dialog_progress_next();
	dialog_save_ok=map_save(&map);
	
	dialog_progress_end();
	
	return(dialog_save_ok);
}

