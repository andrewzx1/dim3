/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Model Texture Importer

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

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#include "import.h"

extern int						cur_mesh;
extern file_path_setup_type		file_path_setup;
extern model_type				model;

/* =======================================================

      Clear Material Structures
      
======================================================= */

void clear_materials(void)
{
    int						i;
	model_material_type		*material;
    
	material=model.meshes[cur_mesh].materials;
    
    for (i=0;i!=max_model_texture;i++) {
		material->trig_start=material->trig_count=0;
		material++;
    }
}

/* =======================================================

      Texture Counts
      
======================================================= */

int texture_count(void)
{
	int				n,count;
	texture_type	*texture;
	
	count=0;
	texture=model.textures;
	
    for (n=0;n!=max_model_texture;n++) {
		if (texture->frames[0].bitmap.gl_id!=-1) count++;
		texture++;
	}
	
	return(count);
}

int texture_find_free(void)
{
	int				n;
	texture_type	*texture;
	
	texture=model.textures;
	
    for (n=0;n!=max_model_texture;n++) {
		if (texture->frames[0].bitmap.gl_id==-1) return(n);
		texture++;
	}
	
	return(0);
}

/* =======================================================

      Texture Pick
      
======================================================= */

int texture_pick(char *material_name,char *err_str)
{
	int				idx;
    char			path[1024],path2[1024],sub_path[1024],txt[256];
	texture_type	*texture;
	
		// find a free texture
		
	idx=texture_find_free();
	texture=&model.textures[idx];
	
		// pick a bitmap
		
	sprintf(txt,"A material named '%s' has been found.  Please select a PNG file to be used for this material.  The PNG file must be 32-bit and have width and height that are squares of 2 (2, 4, 8, 16, 32, 64, 128, 256, etc).",material_name);
	dialog_alert("Material Found - Select Bitmap",txt,NULL,NULL);
	
	if (!nav_open_file("png",path)) {
		strcpy(err_str,"No texture was choosen.");
		return(-1);
	}
	
		// is it good?
		
	if (!bitmap_check(path,err_str)) return(-1);
	
		// open texture
	
	if (!bitmap_open(&texture->frames[0].bitmap,path,anisotropic_mode_none,mipmap_mode_none,texture->pixelated,FALSE)) {
		strcpy(err_str,"Unable to open bitmap.");
		return(-1);
	}
	
	strcpy(texture->frames[0].name,material_name);
	
		// copy bitmap to model folder
		
	sprintf(sub_path,"Models/%s/Textures",model.name);
	file_paths_data_default(&file_path_setup,path2,sub_path,texture->frames[0].name,"png");
	
	bitmap_copy(path,path2);

	return(idx);
}

/* =======================================================

      Multiple Material Alert
      
======================================================= */

bool texture_use_single(void)
{
	return(dialog_alert("Multiple Materials in Model","Do you want to use a single texture for the entire model or pick a new texture for each material?","Single","Multiple")==0);
}




