/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: External Editor Launching

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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#include "common_view.h"
#include "interface.h"

extern map_type					map;
extern editor_setup_type		setup;
extern file_path_setup_type		file_path_setup;

/* =======================================================

      Launch dim3 Engine
      
======================================================= */

void launch_engine(void)
{
	char					path[1024];
	unsigned char			uc_len;
	d3pnt					pnt;
	d3ang					ang;
	CFURLRef				cf_url;
	FSRef					fsref;
	FILE					*file;
	
		// get path to engine
		
	file_paths_base(&file_path_setup,path,setup.engine_name,"app");

	if (FSPathMakeRef((unsigned char*)path,&fsref,NULL)!=0) {
		dialog_alert("Launch Engine","Could not find Engine, check engine name in preferences.");
		return;
    }
	
		// ok to save?
	
	if (dialog_confirm("Save Changes and Launch Engine?","You need to save changes to this map before you can launch the engine.  Click Run to save changes and launch the map in the engine.","Run","Cancel",NULL)!=0) return;

		// save the map
		
	file_save_map();
	
		// the link file
		
	file_paths_preferences(path,"dim3 Editor Link","tmp");

	file=fopen(path,"w");
	if (file==NULL) return;
	
		// map name
		
	uc_len=(unsigned char)strlen(map.info.name);
   	fwrite(&uc_len,1,1,file);
   	fwrite(map.info.name,1,strlen(map.info.name),file);
	
		// map position
		
	walk_view_get_position(&pnt);
	walk_view_get_angle(&ang);
		
	fwrite(&pnt,1,sizeof(d3pnt),file);
	fwrite(&ang,1,sizeof(d3ang),file);
	
	fclose(file);
	
		// run engine
		
	cf_url=CFURLCreateFromFSRef(kCFAllocatorDefault,&fsref);
	LSOpenCFURLRef(cf_url,NULL);
	CFRelease(cf_url);
}

/* =======================================================

      Launch Map Script Editor
      
======================================================= */

void launch_map_script_editor(void)
{
	char					path[1024];
	CFURLRef				cf_url;
	FSRef					fsref;
		
	file_paths_data(&file_path_setup,path,"Scripts/Courses",map.info.name,"js");
	FSPathMakeRef((unsigned char*)path,&fsref,NULL);
	
	cf_url=CFURLCreateFromFSRef(kCFAllocatorDefault,&fsref);
	if (cf_url==NULL) {
		dialog_alert("Could not locate script file","No script file with this name exists.");
		return;
	}

	LSOpenCFURLRef(cf_url,NULL);
	CFRelease(cf_url);
}

/* =======================================================

      Launch Spot Script Editor
      
======================================================= */

void launch_spot_script_editor(char *script_name)
{
	char					path[1024];
	CFURLRef				cf_url;
	FSRef					fsref;
	
	if (script_name[0]==0x0) return;
		
	file_paths_data(&file_path_setup,path,"Scripts/Objects",script_name,"js");
	FSPathMakeRef((unsigned char*)path,&fsref,NULL);
	
	cf_url=CFURLCreateFromFSRef(kCFAllocatorDefault,&fsref);
	if (cf_url==NULL) {
		dialog_alert("Could not locate script file","No script file with this name exists.");
		return;
	}
	
	LSOpenCFURLRef(cf_url,NULL);
	CFRelease(cf_url);
}
