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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "interface.h"
#include "ui_common.h"

extern file_path_setup_type		file_path_setup;
extern editor_setup_type		setup;
extern editor_state_type		state;

map_type						map;

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
	state.map_opened=FALSE;

	state.texture_edit_idx=-1;
	state.in_preference=FALSE;

	main_wind_close();
	menu_fix_enable();
}

/* =======================================================

      New Map utilities
      
======================================================= */

bool file_create_course_script(char *file_name)
{
	int				sz;
	char			*data,path[1024],sub_path[1024];
	FILE			*file;
	struct stat		sb;
	
		// read the file
	
	os_get_support_file_path(sub_path,"Editor");
	strcat(sub_path,"/Defaults");

	file_paths_app(&file_path_setup,path,sub_path,"Course","js");
		
	if (stat(path,&sb)!=0) return(FALSE);
	sz=sb.st_size;
		
	file=fopen(path,"r");
	if (file==NULL) return(FALSE);
    
	data=(char*)malloc(sz);
    if (data==NULL) {
        fclose(file);
        return(FALSE);
    }
	
	fread(data,1,sz,file);
	fclose(file);
	
		// write the file
		
	file_paths_data_default(&file_path_setup,path,"Scripts/Courses",file_name,"js");
		
	file=fopen(path,"w");
	if (file==NULL) {
		free(data);
		return(FALSE);
	}
	
	fwrite(data,1,sz,file);
	fclose(file);
	
	return(TRUE);
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
		
	map_setup(&file_path_setup,anisotropic_mode_none,setup.mipmap_mode,texture_quality_mode_high,FALSE);
	
		// copy the course script
		
	if (!file_create_course_script(file_name)) {
		os_dialog_alert("dim3 Editor could not create the map script","The disk might be locked or a folder might be missing.\n\nIf you are running dim3 directly from the DMG file, then you need to move the files to your harddrive (DMGs are read-only).");
		return(FALSE);
	}
	
		// start the map
		
	select_clear();
	undo_clear();
		
    map_new(&map,file_name);
	
	state.map_opened=TRUE;
  	
    main_wind_open();
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
	char			file_name[256];
	bool			ok;
	
		// open the map
		
	if (!dialog_file_open_run("Open a Map","Maps","xml",NULL,file_name)) return(FALSE);
	
		// open the map
		
	os_set_wait_cursor();
	
	select_clear();
	
	main_wind_open();
	os_set_title_window(file_name);
	
	map_setup(&file_path_setup,anisotropic_mode_none,setup.mipmap_mode,texture_quality_mode_high,FALSE);
	
	ok=map_open(&map,file_name);

    os_set_arrow_cursor();
	
	if (!ok) {
		file_close_window();
		return(FALSE);
	}
	
		// if no views in map, then
		// set to default
		
	if (map.editor_views.count==0) 	view_setup_default_views();
	
		// clear selection
		
	select_clear();
	undo_clear();
	
		// start models
		
	view_models_start();
	view_models_reset();
	
		// set flags as opened
		
	state.map_opened=TRUE;
	
	file_reset_window();
	
	return(TRUE);
}

/* =======================================================

      Save Map
      
======================================================= */

void file_save_map(void)
{
	bool			save_ok;

	os_set_wait_cursor();

	progress_start("Saving...",3);
	
	progress_next_title("Saving: Rebuilding Paths");
	node_path_rebuild();
	
	progress_next_title("Saving: Recalculating Normals");
	map_recalc_normals(&map,FALSE);
	
	progress_next_title("Saving: Writing XML");
	save_ok=map_save(&map);
	
	progress_end();
	
	os_set_arrow_cursor();
	
	if (!save_ok) os_dialog_alert("dim3 Editor could not save map","The disk might be locked or a folder might be missing.\n\nIf you are running dim3 directly from the DMG file, then you need to move the files to your harddrive (DMGs are read-only).");
}

/* =======================================================

      Close Map
      
======================================================= */

void file_close_map(void)
{
	if (!state.map_opened) return;
	
	os_set_wait_cursor();

	map_close(&map);
	view_models_close();
	
	undo_clear();

	file_close_window();
	
	os_set_arrow_cursor();
}

