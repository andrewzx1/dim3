/****************************** File *********************************

Module: dim3 Editor
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
	#include "dim3editor.h"
#endif

extern file_path_setup_type		file_path_setup;

app_pref_type					pref;

/* =======================================================

      Default Preferences
      
======================================================= */

void setup_xml_default(void)
{
	strcpy(pref.map.engine_name,"dim3 Engine");
	
	pref.map.free_look=TRUE;
	pref.map.auto_texture=TRUE;
	pref.map.big_texture=FALSE;
	pref.map.show_tangent_binormal=FALSE;

	pref.map.duplicate_offset=5;
	pref.map.snap_size=5;
	pref.map.clip_distance=200;
	
	pref.map.flip_horz_movement=FALSE;
	pref.map.flip_vert_movement=FALSE;
	pref.map.flip_horz_turn=FALSE;
	pref.map.flip_vert_turn=FALSE;
	pref.map.flip_forward_movement=FALSE;
	
	pref.map.col.mesh_line.r=0.5f;
	pref.map.col.mesh_line.g=0.5f;
	pref.map.col.mesh_line.b=1.0f;
	
	pref.map.col.mesh_sel.r=1.0f;
	pref.map.col.mesh_sel.g=1.0f;
	pref.map.col.mesh_sel.b=0.0f;
	
	pref.map.col.poly_sel.r=1.0f;
	pref.map.col.poly_sel.g=0.0f;
	pref.map.col.poly_sel.b=0.0f;
	
	pref.map.col.poly_cull.r=0.9f;
	pref.map.col.poly_cull.g=0.9f;
	pref.map.col.poly_cull.b=0.9f;
	
	pref.map.col.background.r=0.75f;
	pref.map.col.background.g=0.75f;
	pref.map.col.background.b=0.75f;
	
	pref.map.import_normal_factor.x=1.0f;
	pref.map.import_normal_factor.y=1.0f;
	pref.map.import_normal_factor.z=1.0f;
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
		
	file_paths_dim3_app_data(&file_path_setup,path,"dim3Editor","xml");
	if (!xml_open_file(path)) return(FALSE);
    
		// decode the file
       
    setup_tag=xml_findrootchild("Setup");
    if (setup_tag==-1) {
		xml_close_file();
		return(FALSE);
    }
	
		// keys

 	xml_key_read_boolean(setup_tag,"Free_Look",&pref.map.free_look);
 	xml_key_read_boolean(setup_tag,"Auto_Texture",&pref.map.auto_texture);
 	xml_key_read_boolean(setup_tag,"Big_Texture",&pref.map.big_texture);
 	xml_key_read_boolean(setup_tag,"Show_Tangent_Binormal",&pref.map.show_tangent_binormal);
 	xml_key_read_boolean(setup_tag,"Flip_Horz_Movement",&pref.map.flip_horz_movement);
 	xml_key_read_boolean(setup_tag,"Flip_Vert_Movement",&pref.map.flip_vert_movement);
 	xml_key_read_boolean(setup_tag,"Flip_Horz_Turn",&pref.map.flip_horz_turn);
 	xml_key_read_boolean(setup_tag,"Flip_Vert_Turn",&pref.map.flip_vert_turn);
 	xml_key_read_boolean(setup_tag,"Flip_Forward_Movement",&pref.map.flip_forward_movement);
	xml_key_read_int(setup_tag,"Duplicate_Offset",&pref.map.duplicate_offset);
	xml_key_read_int(setup_tag,"Snap_Size",&pref.map.snap_size);
	xml_key_read_int(setup_tag,"Clip_Distance",&pref.map.clip_distance);
	xml_key_read_text(setup_tag,"Engine_Name",pref.map.engine_name,256);
	xml_key_read_color(setup_tag,"Color_Mesh_Line",&pref.map.col.mesh_line);
	xml_key_read_color(setup_tag,"Color_Mesh_Sel",&pref.map.col.mesh_sel);
	xml_key_read_color(setup_tag,"Color_Poly_Sel",&pref.map.col.poly_sel);
	xml_key_read_color(setup_tag,"Color_Poly_Cull",&pref.map.col.poly_cull);
	xml_key_read_color(setup_tag,"Color_Background",&pref.map.col.background);
    xml_key_read_float(setup_tag,"Import_Normal_Factor_X",&pref.map.import_normal_factor.x);
	xml_key_read_float(setup_tag,"Import_Normal_Factor_Y",&pref.map.import_normal_factor.y);
    xml_key_read_float(setup_tag,"Import_Normal_Factor_Z",&pref.map.import_normal_factor.z);
 
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
		
 	xml_key_write_boolean("Free_Look",pref.map.free_look);
 	xml_key_write_boolean("Auto_Texture",pref.map.auto_texture);
 	xml_key_write_boolean("Big_Texture",pref.map.big_texture);
  	xml_key_write_boolean("Show_Tangent_Binormal",pref.map.show_tangent_binormal);
	xml_key_write_boolean("Flip_Horz_Movement",pref.map.flip_horz_movement);
 	xml_key_write_boolean("Flip_Vert_Movement",pref.map.flip_vert_movement);
 	xml_key_write_boolean("Flip_Horz_Turn",pref.map.flip_horz_turn);
 	xml_key_write_boolean("Flip_Vert_Turn",pref.map.flip_vert_turn);
 	xml_key_write_boolean("Flip_Forward_Movement",pref.map.flip_forward_movement);
	xml_key_write_int("Duplicate_Offset",pref.map.duplicate_offset);
	xml_key_write_int("Snap_Size",pref.map.snap_size);
	xml_key_write_int("Clip_Distance",pref.map.clip_distance);
	xml_key_write_text("Engine_Name",pref.map.engine_name);
	xml_key_write_color("Color_Mesh_Line",&pref.map.col.mesh_line);
	xml_key_write_color("Color_Mesh_Sel",&pref.map.col.mesh_sel);
	xml_key_write_color("Color_Poly_Sel",&pref.map.col.poly_sel);
	xml_key_write_color("Color_Poly_Cull",&pref.map.col.poly_cull);
	xml_key_write_color("Color_Background",&pref.map.col.background);
    xml_key_write_float("Import_Normal_Factor_X",pref.map.import_normal_factor.x);
    xml_key_write_float("Import_Normal_Factor_Y",pref.map.import_normal_factor.y);
    xml_key_write_float("Import_Normal_Factor_Z",pref.map.import_normal_factor.z);

        // save the setup
		// always save to user specific data
		
	file_paths_dim3_app_data(&file_path_setup,path,"dim3Editor","xml");
	ok=xml_save_file(path,err_str);
    xml_close_file();
	
	return(ok);
}
