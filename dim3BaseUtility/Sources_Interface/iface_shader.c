/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Shader XML

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
	#include "dim3baseutility.h"
#endif

extern file_path_setup_type		iface_file_path_setup;

/* =======================================================

      Read Shader XML
      
======================================================= */

void iface_read_settings_shader(iface_type *iface)
{
	int						nshader,shaders_head_tag,shader_tag,tag;
	char					path[1024];
	iface_shader_type		*iface_shader;

		// no marks yet

	iface->shader_list.nshader=0;
	
		// read in interface from setting files
		
	file_paths_data(&iface_file_path_setup,path,"Settings","Shaders","xml");
	if (!xml_open_file(path)) return;
	
		// get counts
		
    shaders_head_tag=xml_findrootchild("Shaders");
    if (shaders_head_tag==-1) {
		xml_close_file();
		return;
	}

	nshader=xml_countchildren(shaders_head_tag);

	if (nshader==0) {
		xml_close_file();
		return;
	}

		// read the shaders
	
	shader_tag=xml_findfirstchild("Shader",shaders_head_tag);
	
	while (shader_tag!=-1) {
	
		iface_shader=&iface->shader_list.shaders[iface->shader_list.nshader];
		
		xml_get_attribute_text(shader_tag,"name",iface_shader->name,name_str_len);
		
			// shader files
			
		tag=xml_findfirstchild("Code",shader_tag);
		if (tag!=-1) {
			xml_get_attribute_text(tag,"vert",iface_shader->vert_name,file_str_len);
			xml_get_attribute_text(tag,"frag",iface_shader->frag_name,file_str_len);
		}
		
			// move on to next shader
			
		iface->shader_list.nshader++;
		
		shader_tag=xml_findnextchild(shader_tag);
	}
	
	xml_close_file();
}

