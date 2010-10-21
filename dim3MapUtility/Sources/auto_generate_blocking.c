/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Auto-Generate Blocking

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit �Created with dim3 Technology� is given on a single
application screen and in a single piece of the documentation
3. It is not resold, in it's current form or modified, as an
engine-only product

This code is presented as is. The author of dim3 takes no
responsibilities for any version of this code.

Any non-engine product (games, etc) created with this code is free
from any and all payment and/or royalties to the author of dim3,
and can be sold or given away.

(c) 2000-2010 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3maputility.h"
#endif

unsigned char		ag_flow_data_none[max_ag_flow_sz][max_ag_flow_sz]=ag_flow_data_none_bytes,
					ag_flow_data_circle[max_ag_flow_sz][max_ag_flow_sz]=ag_flow_data_circle_bytes,
					ag_flow_data_u[max_ag_flow_sz][max_ag_flow_sz]=ag_flow_data_u_bytes,
					ag_flow_data_h[max_ag_flow_sz][max_ag_flow_sz]=ag_flow_data_h_bytes,
					ag_flow_data_l[max_ag_flow_sz][max_ag_flow_sz]=ag_flow_data_l_bytes,
					ag_flow_data_i[max_ag_flow_sz][max_ag_flow_sz]=ag_flow_data_i_bytes;

/* =======================================================

      Auto-Generate Dectect Block Collisions
      
======================================================= */

bool map_auto_generate_block_collision(auto_generate_settings_type *ags,int x,int z,int ex,int ez,bool corridor_only)
{
	int				blck_x,blck_z,lx,rx,tz,bz,map_x_factor,map_z_factor;
	unsigned char	blk,*block;
	
		// get block data
		
	switch (ags->flow) {
	
		case ag_flow_none:
			block=(unsigned char*)ag_flow_data_none;
			break;
		
		case ag_flow_circle:
			block=(unsigned char*)ag_flow_data_circle;
			break;
			
		case ag_flow_u:
			block=(unsigned char*)ag_flow_data_u;
			break;
			
		case ag_flow_preset_h:
			block=(unsigned char*)ag_flow_data_h;
			break;
			
		case ag_flow_preset_l:
			block=(unsigned char*)ag_flow_data_l;
			break;

		case ag_flow_preset_i:
			block=(unsigned char*)ag_flow_data_i;
			break;
			
	}
		
		// calculate block

	map_x_factor=(ags->map.right-ags->map.left)/max_ag_flow_sz;
	map_z_factor=(ags->map.bottom-ags->map.top)/max_ag_flow_sz;

	bz=ags->map.top;

	for (blck_z=0;blck_z!=max_ag_flow_sz;blck_z++) {

		tz=bz;
		bz+=map_z_factor;

		rx=ags->map.left;

		for (blck_x=0;blck_x!=max_ag_flow_sz;blck_x++) {

			lx=rx;
			rx+=map_x_factor;

			blk=block[(blck_z*max_ag_flow_sz)+blck_x];
			if (blk==0x2) {
				if (corridor_only) {
					blk=0x0;
				}
				else {
					blk=0x1;
				}
			}

			if (blk!=0x0) {
				if (ez<=tz) continue;
				if (ex<=lx) continue;
				if (x>=rx) continue;
				if (z>=bz) continue;

				return(TRUE);
			}
		}
	}

	return(FALSE);
}

