/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Setup XML

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
	#include "dim3animator.h"
#endif

extern file_path_setup_type		file_path_setup;

animator_setup_type				setup;

/* =======================================================

      Default Preferences
      
======================================================= */

void setup_xml_default(void)
{
	setup.show_tangent_binormal=FALSE;

	setup.col.mesh_line.r=0.5f;
	setup.col.mesh_line.g=0.5f;
	setup.col.mesh_line.b=1.0f;
	
	setup.col.mesh_sel.r=1.0f;
	setup.col.mesh_sel.g=1.0f;
	setup.col.mesh_sel.b=0.0f;
	
	setup.col.background.r=0.75f;
	setup.col.background.g=0.75f;
	setup.col.background.b=0.75f;
}

/* =======================================================

      Read Setup
      
======================================================= */

bool setup_xml_read(void)
{
	int							setup_tag;
	char						path[1024];
	
		// default prefs
		
	setup_xml_default();
    
        // find the setup.xml file
		
	file_paths_dim3_app_data(&file_path_setup,path,"dim3Animator","xml");
	if (!xml_open_file(path)) return(FALSE);
    
		// decode the file
       
    setup_tag=xml_findrootchild("Setup");
    if (setup_tag==-1) {
		xml_close_file();
		return(FALSE);
    }
	
		// keys

 	xml_key_read_boolean(setup_tag,"Show_Tangent_Binormal",&setup.show_tangent_binormal);
	xml_key_read_color(setup_tag,"Color_Mesh_Line",&setup.col.mesh_line);
	xml_key_read_color(setup_tag,"Color_Mesh_Sel",&setup.col.mesh_sel);
	xml_key_read_color(setup_tag,"Color_Background",&setup.col.background);
  
	xml_close_file();
	
	return(TRUE);
}

/* =======================================================

      Write Key
      
======================================================= */

bool setup_xml_write(void)
{
	char				path[1024],err_str[256];
	bool				ok;
	
		// start the setup file
		
    xml_new_file();
    
    xml_add_tagstart("Setup");
    xml_add_tagend(FALSE);
	
		// keys
		
 	xml_key_write_boolean("Show_Tangent_Binormal",setup.show_tangent_binormal);
	xml_key_write_color("Color_Mesh_Line",&setup.col.mesh_line);
	xml_key_write_color("Color_Mesh_Sel",&setup.col.mesh_sel);
	xml_key_write_color("Color_Background",&setup.col.background);

        // save the setup
		// always save to user specific data
		
	file_paths_dim3_app_data(&file_path_setup,path,"dim3Animator","xml");
	ok=xml_save_file(path,err_str);
    xml_close_file();
	
	return(ok);
}
