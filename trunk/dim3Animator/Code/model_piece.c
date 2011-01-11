/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Model Add or Delete Pieces

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
	#include "dim3animator.h"
#endif

#include "glue.h"

extern model_type				model;
extern animator_state_type		state;

/* =======================================================

      Add or Delete Hit Boxes
      
======================================================= */

void model_piece_add_hit_box(void)
{
	state.hit_box=TRUE;

	state.cur_hit_box_idx=model_hit_box_add(&model);
	if (state.cur_hit_box_idx!=-1) state.cur_item=item_hit_box;
}

void model_piece_delete_hit_box(int hit_box_idx)
{
	model_hit_box_delete(&model,hit_box_idx);
	state.cur_item=item_model;
}
