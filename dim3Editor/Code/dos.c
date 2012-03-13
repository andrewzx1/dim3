/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: File Menu Routines

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
#include "ui_common.h"

extern map_type					map;
extern file_path_setup_type		file_path_setup;
extern editor_setup_type		setup;
extern editor_state_type		state;

/* =======================================================

      Reset and Close Windows
            
======================================================= */

void file_reset_window(void)
{
        // no selection
        
	select_clear();
	
		// redraw
	
 	state.texture_edit_idx=-1;
	state.in_preference=FALSE;
	state.cur_cinema_idx=-1;
	state.cur_movement_idx=-1;

	tool_default();
	menu_update_view();
	menu_fix_enable();
	texture_palette_reset();
	
    os_select_window();
    
	main_wind_draw();
}

void file_close_window(void)
{
	state.map_open=FALSE;

	state.texture_edit_idx=-1;
	state.in_preference=FALSE;

	menu_fix_enable();
}

/* =======================================================

      New Map
      
======================================================= */

bool file_new_map(void)
{
	char		file_name[256];
	
		// get the map name
		
	strcpy(file_name,"NewMap");
	if (!dialog_file_new_run("Create a New Map",file_name)) return(FALSE);
	
		// set the map paths
		
	map_setup(&file_path_setup,FALSE,setup.mipmap_mode,FALSE,FALSE);
	
		// start the map
		
	select_clear();
	undo_clear();
		
    map_new(&map,file_name);
	
	state.map_open=TRUE;
	strcpy(state.map_file_name,file_name);
  	
	os_set_title_window(file_name);
	view_setup_default_views();
	
		// start models
		
	view_models_start();
		
		// redraw the window
    
	file_reset_window();
	
	return(TRUE);
}

/* =======================================================

      Open Map
      
======================================================= */

bool file_open_map(void)
{
	int			n;
	char		str[256],file_name[file_str_len];
	bool		ok;
	
		// open the map
		
	if (!dialog_file_open_run("Open a Map","Maps","xml",NULL,file_name)) {
		state.map_open=FALSE;
		menu_fix_enable();
		return(FALSE);
	}
	
		// open the map
		
	os_set_wait_cursor();
	
	select_clear();
	
	os_set_title_window(file_name);

	sprintf(str,"Loading %s...",file_name);
	progress_start("Loading...",11);
	
	map_setup(&file_path_setup,FALSE,setup.mipmap_mode,FALSE,FALSE);
	progress_next();
	
	ok=map_open(&map,file_name);
	progress_next();

	map_textures_read_setup(&map);

	for (n=0;n!=max_map_texture;n++) {
		map_textures_read_texture(&map,n);
		if ((n%32)==0) progress_next();
	}

	progress_end();

    os_set_arrow_cursor();
	
	if (!ok) {
		file_close_window();
		return(FALSE);
	}

		// start the VBOs

	view_vbo_map_initialize();
	
		// if no views in map, then
		// set to default
		
	if (map.editor_views.count==0) 	view_setup_default_views();
	
		// clear selection
		
	select_clear();
	undo_clear();
	
		// start models
		
	view_models_start();
	
		// set flags as opened
		
	state.map_open=TRUE;
	strcpy(state.map_file_name,file_name);
	
	file_reset_window();
	
	return(TRUE);
}

/* =======================================================

      Save Map
      
======================================================= */

void file_save_map(void)
{
	char			err_str[256];
	bool			save_ok;

	os_set_wait_cursor();

	progress_start("Saving...",3);
	
	progress_next();
	node_path_rebuild();
	
	progress_next();
	save_ok=map_save(&map,err_str);
	
	progress_next();
	progress_end();
	
	os_set_arrow_cursor();
	
	if (!save_ok) os_dialog_alert("dim3 Editor could not save map",err_str);
}

/* =======================================================

      Close Map
      
======================================================= */

bool file_close_map(void)
{
	int			choice;

		// if no map open, just return
		// that close is OK

	if (!state.map_open) return(TRUE);

		// confirm save
	
	choice=os_dialog_confirm("Save Changes?","Do you want to save the changes to this map?",TRUE);
	if (choice==1) return(FALSE);
	
	if (choice==0) file_save_map();

		// close map

	os_set_wait_cursor();

		// delete VBOs

	view_vbo_map_free();

		// close map

	map_close(&map);
	view_models_close();

		// clear undo
	
	undo_clear();

	file_close_window();
	
	os_set_arrow_cursor();

	return(TRUE);
}

