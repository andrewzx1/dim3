/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
 Usage: Texture File Loading

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

(c) 2000-2010 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3modelutility.h"
#endif

extern modelutility_settings_type		modelutility_settings;

/* =======================================================

      Clear Textures
      
======================================================= */

void model_textures_clear(model_type *model)
{
	int						n;
    texture_type			*texture;
	
    texture=model->textures;
    
	for (n=0;n!=max_model_texture;n++) {
		bitmap_texture_clear(texture);
        texture++;
	}
}

/* =======================================================

      Load Textures
      
======================================================= */

void model_textures_read(model_type *model)
{
    int						i,k;
    char					sub_path[1024],path[1024],name[file_str_len];
	texture_type			*texture;
	texture_frame_type		*frame;
   
        // load the fills
        
    texture=model->textures;
    
	for (i=0;i!=max_model_texture;i++) {
	
			// clear textures
			
 		frame=texture->frames;
  
        for (k=0;k!=max_texture_frame;k++) {
			bitmap_new(&frame->bitmap);
			bitmap_new(&frame->bumpmap);
			bitmap_new(&frame->specularmap);
			bitmap_new(&frame->glowmap);
			bitmap_new(&frame->combinemap);
			
			frame++;
        }
   
			// load textures
			
		frame=texture->frames;
        
        for (k=0;k!=max_texture_frame;k++) {
		
			if (frame->name[0]!=0x0) {

				sprintf(sub_path,"Models/%s/Textures",model->name);
				file_paths_data(&modelutility_settings.file_path_setup,path,sub_path,frame->name,"png");
				bitmap_open(&frame->bitmap,path,modelutility_settings.anisotropic_mode,modelutility_settings.mipmap_mode,modelutility_settings.texture_quality_mode,texture->compress,FALSE,texture->pixelated,FALSE,FALSE);

					// bumpmap

				sprintf(name,"%s_n",frame->name);
				file_paths_data(&modelutility_settings.file_path_setup,path,sub_path,name,"png");	// compress messes up normals
				bitmap_open(&frame->bumpmap,path,modelutility_settings.anisotropic_mode,modelutility_settings.mipmap_mode,modelutility_settings.texture_quality_mode,FALSE,FALSE,texture->pixelated,FALSE,FALSE);
				
					// specular map

				sprintf(name,"%s_s",frame->name);
				file_paths_data(&modelutility_settings.file_path_setup,path,sub_path,name,"png");
				bitmap_open(&frame->specularmap,path,modelutility_settings.anisotropic_mode,modelutility_settings.mipmap_mode,modelutility_settings.texture_quality_mode,texture->compress,FALSE,texture->pixelated,FALSE,FALSE);
				
					// glow map

				sprintf(name,"%s_g",frame->name);
				file_paths_data(&modelutility_settings.file_path_setup,path,sub_path,name,"png");
				bitmap_open(&frame->glowmap,path,modelutility_settings.anisotropic_mode,modelutility_settings.mipmap_mode,modelutility_settings.texture_quality_mode,texture->compress,FALSE,texture->pixelated,TRUE,FALSE);
			}
			
			frame++;
        }
        
        texture++;
	}
}

/* =======================================================

      Close Textures
      
======================================================= */

void model_textures_close(model_type *model)
{
    int						i,k;
    texture_type			*texture;
	texture_frame_type		*frame;

    texture=model->textures;
    
	for (i=0;i!=max_model_texture;i++) {
    
		frame=texture->frames;
  
        for (k=0;k!=max_texture_frame;k++) {
			bitmap_close(&frame->bitmap);
			bitmap_close(&frame->bumpmap);
			bitmap_close(&frame->specularmap);
			bitmap_close(&frame->glowmap);
			
			frame++;
        }
        
        texture++;
	}
}


