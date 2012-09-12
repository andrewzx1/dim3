/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Label

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
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "interface.h"

extern iface_type				iface;
extern list_palette_type		project_palette;

/* =======================================================

      Property Palette Fill Label
      
======================================================= */

void project_palette_fill_label(void)
{
	list_palette_set_title(&project_palette,"Label",NULL,NULL,NULL,NULL,NULL);

		// settings
		
	list_palette_add_header(&project_palette,0,"Settings");
	list_palette_add_int(&project_palette,-1,"Minimum View Distance",&iface.label.min_dist,FALSE);
	list_palette_add_int(&project_palette,-1,"Maximum View Distance",&iface.label.max_dist,FALSE);
	
		// text
		
	list_palette_add_header(&project_palette,0,"Text");
	list_palette_add_int(&project_palette,-1,"Size",&iface.label.text.size,FALSE);
	list_palette_add_pick_color(&project_palette,-1,"Color",&iface.label.text.col,FALSE);

		// bitmap
		
	list_palette_add_header(&project_palette,0,"Bitmap");
	list_palette_add_int(&project_palette,-1,"Size",&iface.label.bitmap.size,FALSE);

		// health
		
	list_palette_add_header(&project_palette,0,"Health");
	list_palette_add_int(&project_palette,-1,"Width",&iface.label.health.wid,FALSE);
	list_palette_add_int(&project_palette,-1,"Height",&iface.label.health.high,FALSE);
	list_palette_add_checkbox(&project_palette,-1,"Border",&iface.label.health.border_on,FALSE);
	list_palette_add_pick_color(&project_palette,-1,"Border Color",&iface.label.health.border_col,FALSE);
	list_palette_add_checkbox(&project_palette,-1,"Background",&iface.label.health.background_on,FALSE);
	list_palette_add_pick_color(&project_palette,-1,"Background Color",&iface.label.health.background_col,FALSE);
	list_palette_add_pick_color(&project_palette,-1,"Bar Color",&iface.label.health.bar_col,FALSE);
}

/* =======================================================

      Property Palette Click Label
      
======================================================= */

void project_palette_click_label(bool double_click)
{
}

