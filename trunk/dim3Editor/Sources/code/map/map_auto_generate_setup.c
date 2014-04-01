/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Auto Generation Setup

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
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "interface.h"

extern map_type					map;

extern ag_state_type			ag_state;

/* =======================================================

      Auto Generate Memory
      
======================================================= */

bool ag_initialize(char *err_str)
{
	ag_state.rooms=NULL;

	ag_state.rooms=(ag_room_type*)malloc(ag_max_room*sizeof(ag_room_type));
	if (ag_state.rooms==NULL) {
		strcpy(err_str,"Out of Memory");
		return(FALSE);
	}

	return(TRUE);
}

void ag_release(void)
{
	if (ag_state.rooms!=NULL) free(ag_state.rooms);
	ag_state.rooms=NULL;
}

/* =======================================================

      Check Required Textures
      
======================================================= */

bool ag_check_required_textures(char *err_str)
{
	int			n;

	for (n=0;n!=ag_texture_required_count;n++) {
		if (map.textures[n].frames[0].bitmap.gl_id==-1) {
			sprintf(err_str,"Auto Generator requires at least the first %d texture spots to be filled",ag_texture_required_count);
			return(FALSE);
		}
	}

	return(TRUE);
}


