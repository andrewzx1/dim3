/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Model Texture Importer

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

/* =======================================================

      Texture Copy
      
======================================================= */

void texture_copy(char *path,char *name,char *sub_name)
{
    char			*c,dest_name[256],srce_path[1024],dest_path[1024],
					sub_path[1024];

	strcpy(srce_path,path);
	
	if (sub_name!=NULL) {
		c=strrchr(srce_path,'.');
		if (c==NULL) return;
		
		*c=0x0;
		strcat(srce_path,sub_name);
		strcat(srce_path,".png");
	}
	
	strcpy(dest_name,name);
	
	if (sub_name!=NULL) strcat(dest_name,sub_name);
	
	sprintf(sub_path,"Models/%s/Textures",model.name);
	file_paths_data_default(&file_path_setup,dest_path,sub_path,dest_name,"png");
	bitmap_copy(srce_path,dest_path);
}

/* =======================================================

      Texture Pick
      
======================================================= */

int texture_pick(char *material_name,char *err_str)
{
	int				n,idx;
    char			*c,path[1024],title[256],file_name[256];
	texture_type	*texture;
	
		// find a free texture or texture
		// that already has material name
		
	idx=-1;
	
	texture=model.textures;
	
    for (n=0;n!=max_model_texture;n++) {
	
			// a free texture
			
		if ((texture->frames[0].bitmap.gl_id==-1) && (idx==-1)) {
			idx=n;
		}
		
			// a used texture, check to see if
			// material already exists
			
		else {
			if (strcasecmp(texture->material_name,material_name)==0) return(n);
		}
		
		texture++;
	}
	
	if (idx==-1) return(0);
	
	texture=&model.textures[idx];
	
		// remember material name
		
	strcpy(texture->material_name,material_name);
	
		// pick a bitmap
		
	sprintf(title,"Material %s Found - Select a PNG Bitmap",material_name);
	os_dialog_alert(title,"Please select a PNG file to be used for this material.  The PNG file must be 32-bit and have width and height that are squares of 2 (2, 4, 8, 16, 32, 64, 128, 256, etc).");
	
	if (!os_load_file(path,"png")) {
		strcpy(err_str,"No texture was choosen.");
		return(-1);
	}
	
		// is it good?
		
	if (!bitmap_check(path,err_str)) return(-1);
	
		// get the actual file name
		
	c=strrchr(path,'/');
	strcpy(file_name,c);
	c=strrchr(file_name,'.');
	*c=0x0;
	
		// copy bitmaps to model folder
		// copy selected bitmap and any addition _n, _s, or _g files
		
	strcpy(texture->frames[0].name,file_name);
		
	texture_copy(path,texture->frames[0].name,NULL);
	texture_copy(path,texture->frames[0].name,"_n");
	texture_copy(path,texture->frames[0].name,"_s");
	texture_copy(path,texture->frames[0].name,"_g");
	
		// open the textures
		
	model_refresh_textures(&model);

	return(idx);
}

/* =======================================================

      Multiple Material Alert
      
======================================================= */

bool texture_use_single(void)
{
	return(os_dialog_confirm("Multiple Materials in Model","Click Yes to pick a new texture for each material, or No to use a single texture.",FALSE)!=0);
}




