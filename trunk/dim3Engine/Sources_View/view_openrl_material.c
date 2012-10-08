/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View OpenRL Material

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

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "interface.h"

#ifdef D3_OPENRL
	#include "ray_interface.h"
#endif

extern map_type				map;
extern server_type			server;
extern camera_type			camera;
extern view_type			view;
extern iface_type			iface;
extern setup_type			setup;
extern network_setup_type	net_setup;
extern file_path_setup_type	file_path_setup;

#ifdef D3_OPENRL

	extern int						view_rl_scene_id;

#endif

/* =======================================================

      Blank Patches
      
======================================================= */

#ifndef D3_OPENRL

#else

/* =======================================================

      OpenRL Create Material
      
======================================================= */

int view_openrl_create_material(char *sub_path,texture_type *texture,texture_frame_type *frame)
{
	int					material_id,wid,high;
	bool				alpha_channel;
	char				name[256],path[1024];
	unsigned char		*png_data;

		// create material, use original
		// bitmap for wid/high
		
	file_paths_data(&file_path_setup,path,sub_path,frame->name,"png");
		
	png_data=png_utility_read(path,&wid,&high,&alpha_channel);
	if (png_data==NULL) return(-1);
	
	material_id=rlMaterialAdd(wid,high,0);
	rlMaterialAttachBufferData(material_id,RL_MATERIAL_TARGET_COLOR,(alpha_channel?RL_MATERIAL_FORMAT_32_RGBA:RL_MATERIAL_FORMAT_24_RGB),png_data);
	
	free(png_data);
	
		// normal map
		
	sprintf(name,"%s_n",frame->name);
	file_paths_data(&file_path_setup,path,sub_path,name,"png");

	png_data=png_utility_read(path,&wid,&high,&alpha_channel);
	if (png_data!=NULL) {
		rlMaterialAttachBufferData(material_id,RL_MATERIAL_TARGET_NORMAL,(alpha_channel?RL_MATERIAL_FORMAT_32_RGBA:RL_MATERIAL_FORMAT_24_RGB),png_data);
		free(png_data);
	}

		// specular map
		
	sprintf(name,"%s_s",frame->name);
	file_paths_data(&file_path_setup,path,sub_path,name,"png");

	png_data=png_utility_read(path,&wid,&high,&alpha_channel);
	if (png_data!=NULL) {
		rlMaterialAttachBufferData(material_id,RL_MATERIAL_TARGET_SPECULAR,(alpha_channel?RL_MATERIAL_FORMAT_32_RGBA:RL_MATERIAL_FORMAT_24_RGB),png_data);
		free(png_data);
	}

	rlMaterialSetShineFactor(material_id,texture->shine_factor);

		// mipmaps

	rlMaterialBuildMipMaps(material_id);
	
	return(material_id);
}

#endif