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

extern map_type				map;
extern server_type			server;
extern camera_type			camera;
extern view_type			view;
extern iface_type			iface;
extern setup_type			setup;
extern network_setup_type	net_setup;
extern file_path_setup_type	file_path_setup;

extern int					view_rl_scene_id;

extern texture_font_type	view_rl_fonts[2];

extern bool bitmap_text_font_exist(char *name);

/* =======================================================

      OpenRL Create Material from Texture
      
======================================================= */

int view_openrl_create_material_from_texture(char *sub_path,texture_type *texture,texture_frame_type *frame,int alpha_type)
{
	int					material_id,wid,high,png_wid,png_high;
	bool				alpha_channel;
	char				name[256],path[1024];
	unsigned char		*png_data;

		// create material, use original
		// bitmap for wid/high
		
	file_paths_data(&file_path_setup,path,sub_path,frame->name,"png");
		
	png_data=png_utility_read(path,&wid,&high,&alpha_channel);
	if (png_data==NULL) return(-1);
	
	material_id=rlMaterialAdd(wid,high,alpha_type,0);
	rlMaterialAttachBufferData(material_id,RL_MATERIAL_TARGET_COLOR,(alpha_channel?RL_MATERIAL_FORMAT_32_RGBA:RL_MATERIAL_FORMAT_24_RGB),png_data);
	
	free(png_data);
	
		// normal map
		
	sprintf(name,"%s_n",frame->name);
	file_paths_data(&file_path_setup,path,sub_path,name,"png");

	png_data=png_utility_read(path,&png_wid,&png_high,&alpha_channel);
	if (png_data!=NULL) {
		if ((png_wid!=wid) || (png_high!=high)) {
			fprintf(stdout,"%s: Not correct size for material\n",path);
		}
		else {
			rlMaterialAttachBufferData(material_id,RL_MATERIAL_TARGET_NORMAL,(alpha_channel?RL_MATERIAL_FORMAT_32_RGBA:RL_MATERIAL_FORMAT_24_RGB),png_data);
		}
		free(png_data);
	}

		// specular map
		
	sprintf(name,"%s_s",frame->name);
	file_paths_data(&file_path_setup,path,sub_path,name,"png");

	png_data=png_utility_read(path,&png_wid,&png_high,&alpha_channel);
	if (png_data!=NULL) {
		if ((png_wid!=wid) || (png_high!=high)) {
			fprintf(stdout,"%s: Not correct size for material\n",path);
		}
		else {
			rlMaterialAttachBufferData(material_id,RL_MATERIAL_TARGET_SPECULAR,(alpha_channel?RL_MATERIAL_FORMAT_32_RGBA:RL_MATERIAL_FORMAT_24_RGB),png_data);
		}
		free(png_data);
	}

	rlMaterialSetShineFactor(material_id,texture->shine_factor);

		// mipmaps

	rlMaterialBuildMipMaps(material_id);
	
	return(material_id);
}

/* =======================================================

      OpenRL Create Material from Path
      
======================================================= */

int view_openrl_create_material_from_path(char *path)
{
	int					material_id,wid,high;
	bool				alpha_channel;
	unsigned char		*png_data;

		// create material directly
		// from PNG
		
	png_data=png_utility_read(path,&wid,&high,&alpha_channel);
	if (png_data==NULL) return(-1);
	
	material_id=rlMaterialAdd(wid,high,RL_MATERIAL_ALPHA_PASS_THROUGH,0);
	rlMaterialAttachBufferData(material_id,RL_MATERIAL_TARGET_COLOR,(alpha_channel?RL_MATERIAL_FORMAT_32_RGBA:RL_MATERIAL_FORMAT_24_RGB),png_data);
	
	free(png_data);

	return(material_id);
}

/* =======================================================

      View OpenRL Text Materials
      
======================================================= */

void view_openrl_material_text_start_single_font_size(texture_font_size_type *font_size,char *name,int txt_size,int wid,int high)
{
	unsigned char		*data;

	data=bitmap_text_size_data(font_size,name,txt_size,wid,high);

	font_size->openrl_material_id=rlMaterialAdd(wid,high,RL_MATERIAL_ALPHA_PASS_THROUGH,0);
	rlMaterialAttachBufferData(font_size->openrl_material_id,RL_MATERIAL_TARGET_COLOR,RL_MATERIAL_FORMAT_32_RGBA,data);
	free(data);

	rlMaterialBuildMipMaps(font_size->openrl_material_id);
}

void view_openrl_material_text_start_single_font(texture_font_type *d3_font)
{
	int				n,idx;

		// determine which font exists, and use that

	idx=0;

	for (n=0;n!=max_iface_font_variant;n++) {
		if (bitmap_text_font_exist(d3_font->name[n])) {
			idx=n;
			break;
		}
	}

		// load the font

	view_openrl_material_text_start_single_font_size(&d3_font->size_24,d3_font->name[idx],24,512,256);
	view_openrl_material_text_start_single_font_size(&d3_font->size_48,d3_font->name[idx],48,1024,512);
}

void view_openrl_material_text_start(void)
{
	int				n;

	for (n=0;n!=max_iface_font_variant;n++) {
		strcpy(view_rl_fonts[font_interface_index].name[n],iface.font.interface_name[n]);
		strcpy(view_rl_fonts[font_hud_index].name[n],iface.font.hud_name[n]);
	}

	view_openrl_material_text_start_single_font(&view_rl_fonts[font_interface_index]);
	view_openrl_material_text_start_single_font(&view_rl_fonts[font_hud_index]);
}

void view_openrl_material_text_stop(void)
{
	rlMaterialDelete(view_rl_fonts[font_interface_index].size_24.openrl_material_id);
	rlMaterialDelete(view_rl_fonts[font_interface_index].size_48.openrl_material_id);
	rlMaterialDelete(view_rl_fonts[font_hud_index].size_24.openrl_material_id);
	rlMaterialDelete(view_rl_fonts[font_hud_index].size_48.openrl_material_id);
}

texture_font_size_type* view_openrl_material_text_get_font(int text_font,int text_size)
{
	if (text_size<=24) return(&view_rl_fonts[text_font].size_24);
	return(&view_rl_fonts[text_font].size_48);
}

#endif