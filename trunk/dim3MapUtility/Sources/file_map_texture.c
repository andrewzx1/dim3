/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Map Texture Reading

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
	#include "dim3maputility.h"
#endif

extern maputility_settings_type		maputility_settings;

/* =======================================================

      New Map Textures
      
======================================================= */

void map_textures_new(map_type *map)
{
	int					n;
	texture_type		*texture;
	
	texture=map->textures;
	
	for (n=0;n!=max_map_texture;n++) {
		bitmap_texture_clear(texture);
		texture++;
	}
}

/* =======================================================

      Read Map Textures
      
======================================================= */

void map_textures_read_setup(map_type *map)
{
	int					n,k;
	texture_type		*texture;
	texture_frame_type	*frame;

		// clear all textures

	texture=map->textures;
	
	for (n=0;n!=max_map_texture;n++) {
			
		frame=texture->frames;
		
		for (k=0;k!=max_texture_frame;k++) {
			bitmap_new(&frame->bitmap);
			bitmap_new(&frame->bumpmap);
			bitmap_new(&frame->specularmap);
			bitmap_new(&frame->glowmap);
			
			frame++;
		}

		texture++;
	}
}

bool map_textures_read_texture_ok(map_type *map,int txt_idx)
{
	int					n,k;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	map_liquid_type		*liq;

		// check meshes

	mesh=map->mesh.meshes;
		
	for (n=0;n!=map->mesh.nmesh;n++) {
			
		poly=mesh->polys;
			
		for (k=0;k!=mesh->npoly;k++) {
			if ((poly->txt_idx==txt_idx) || (poly->lmap_txt_idx==txt_idx)) return(TRUE);
			poly++;
		}
			
		mesh++;
	}
		
		// check liquids

	liq=map->liquid.liquids;
		
	for (n=0;n!=map->liquid.nliquid;n++) {
		if ((liq->txt_idx==txt_idx) || (liq->lmap_txt_idx==txt_idx) || (liq->overlay.txt_idx==txt_idx)) return(TRUE);
		liq++;
	}
		
		// fogs, backgrounds, and sky

	if (map->fog.texture_idx==txt_idx) return(TRUE);
	if (map->background.on) {
		if (map->background.front.fill==txt_idx) return(TRUE);
		if (map->background.middle.fill==txt_idx) return(TRUE);
		if (map->background.back.fill==txt_idx) return(TRUE);
	}
	if (map->sky.on) {
		if (map->sky.fill==txt_idx) return(TRUE);
		if (map->sky.bottom_fill==txt_idx) return(TRUE);
		if (map->sky.north_fill==txt_idx) return(TRUE);
		if (map->sky.south_fill==txt_idx) return(TRUE);
		if (map->sky.east_fill==txt_idx) return(TRUE);
		if (map->sky.west_fill==txt_idx) return(TRUE);
	}

	return(FALSE);
}

void map_textures_read_texture(map_type *map,int txt_idx)
{
	int					n,texture_quality_mode;
	char				path[1024],path2[1024],name[256];
	texture_type		*texture;
	texture_frame_type	*frame;
						
		// if in engine, then only load textures
		// directly hooked up to elements

	if (maputility_settings.in_engine) {
		if (!map_textures_read_texture_ok(map,txt_idx)) return;
	}

		// load textures
	
	texture=&map->textures[txt_idx];

		// get texture quality

	texture_quality_mode=maputility_settings.texture_quality_mode;
	if (texture->keep_quality) texture_quality_mode=texture_quality_mode_high;
			
		// load texture

	frame=texture->frames;
	
	for (n=0;n!=max_texture_frame;n++) {
	
		if (frame->name[0]!=0x0) {
		
				// if in engine and no shader,
				// then combine bitmap and bump into bitmap
			
			if ((maputility_settings.in_engine) && (!maputility_settings.shader_on)) {
			
				file_paths_data(&maputility_settings.file_path_setup,path,"Bitmaps/Textures",frame->name,"png");
				
				sprintf(name,"%s_n",frame->name);
				file_paths_data(&maputility_settings.file_path_setup,path2,"Bitmaps/Textures",name,"png");
				bitmap_combine(&frame->bitmap,path,path2,maputility_settings.anisotropic_mode,maputility_settings.mipmap_mode,texture_quality_mode,texture->compress,texture->pixelated);
			}
			
				// else load all maps
				
			else {
		
					// bitmap
					
				file_paths_data(&maputility_settings.file_path_setup,path,"Bitmaps/Textures",frame->name,"png");
				bitmap_open(&frame->bitmap,path,maputility_settings.anisotropic_mode,maputility_settings.mipmap_mode,texture_quality_mode,texture->compress,FALSE,texture->pixelated,FALSE);
				
					// bumpmap
					
				sprintf(name,"%s_n",frame->name);
				file_paths_data(&maputility_settings.file_path_setup,path,"Bitmaps/Textures",name,"png");		// compresses messes up normals
				bitmap_open(&frame->bumpmap,path,anisotropic_mode_none,maputility_settings.mipmap_mode,texture_quality_mode,FALSE,FALSE,texture->pixelated,FALSE);
								
					// specular map
					
				sprintf(name,"%s_s",frame->name);
				file_paths_data(&maputility_settings.file_path_setup,path,"Bitmaps/Textures",name,"png");
				bitmap_open(&frame->specularmap,path,maputility_settings.anisotropic_mode,maputility_settings.mipmap_mode,texture_quality_mode,texture->compress,FALSE,texture->pixelated,FALSE);
			}
			
				// glow map
				
			sprintf(name,"%s_g",frame->name);
			file_paths_data(&maputility_settings.file_path_setup,path,"Bitmaps/Textures",name,"png");
			bitmap_open(&frame->glowmap,path,maputility_settings.anisotropic_mode,maputility_settings.mipmap_mode,texture_quality_mode,texture->compress,FALSE,texture->pixelated,TRUE);
		}
		
		frame++;
	}
}

void map_textures_read_complete(map_type *map)
{
	int				n;

	map_textures_read_setup(map);

	for (n=0;n!=max_map_texture;n++) {
		map_textures_read_texture(map,n);
	}
}

/* =======================================================

      Close Map Textures
      
======================================================= */

void map_textures_close(map_type *map)
{
	int						i,k;
	texture_type			*texture;
	texture_frame_type		*frame;
	
	texture=map->textures;
	
	for (i=0;i!=max_map_texture;i++) {
		
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

