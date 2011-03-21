/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Halo XML

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
	#ifdef D3_ENGINE
		#include "dim3engine.h"
	#endif
	#ifdef D3_SETUP
		#include "dim3Setup.h"
	#endif
#endif

#include "interface.h"

extern file_path_setup_type		iface_file_path_setup;

/* =======================================================

      Read Halos XML
      
======================================================= */

void iface_read_settings_halo(iface_type *iface)
{
	int					nhalo,halos_head_tag,halo_tag,tag;
	char				path[1024];
	iface_halo_type		*halo;

		// read in interface from setting files
		
	file_paths_data(&iface_file_path_setup,path,"Settings","Halos","xml");
	if (!xml_open_file(path)) return;
	
		// get counts
		
    halos_head_tag=xml_findrootchild("Halos");
    if (halos_head_tag==-1) {
		xml_close_file();
		return;
	}

	nhalo=xml_countchildren(halos_head_tag);

	if (nhalo==0) {
		xml_close_file();
		return;
	}

		// read the halos

	halo_tag=xml_findfirstchild("Halo",halos_head_tag);
	
	while (halo_tag!=-1) {
	
			// create a new halo

		if (iface->halo_list.nhalo>=max_iface_halo) {
			xml_close_file();
			return;
		}
			
		halo=&iface->halo_list.halos[iface->halo_list.nhalo];
		iface->halo_list.nhalo++;
			
			// read settings
			
		xml_get_attribute_text(halo_tag,"name",halo->name,name_str_len);
		
		tag=xml_findfirstchild("Image",halo_tag);
		if (tag!=-1) {
			xml_get_attribute_text(tag,"file",halo->bitmap_name,file_str_len);
		}
		
			// move on to next halo
			
		halo_tag=xml_findnextchild(halo_tag);
	}
	
	xml_close_file();
}
