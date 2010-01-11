/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View Bitmaps

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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "objects.h"
#include "remotes.h"
#include "consoles.h"
#include "interfaces.h"
#include "effects.h"
#include "video.h"

extern int game_time_get(void);

extern server_type			server;
extern view_type			view;
extern hud_type				hud;
extern setup_type			setup;
extern network_setup_type	net_setup;

extern int					remote_slow_image_idx,remote_talk_image_idx;

bitmap_type					empty_bitmap;

/* =======================================================

      Initialize Images
      
======================================================= */

void view_images_initialize(void)
{
	int				n;
	d3col			col;
	view_image_type	*image;
	
	image=view.images;
	
	for (n=0;n!=max_view_image;n++) {
		image->path[n]=0x0;
		image++;
	}
		
		// first bitmap is a empty bitmap
		// to be used when file is missing
		
	image=&view.images[0];
	
	col.r=col.b=col.g=1.0f;
	bitmap_color(&image->bitmaps[0].bitmap,&col);
	
	image->nbitmap=1;
	strcpy(image->path,"_dim3_core_empty_bitmap");
}

void view_images_shutdown(void)
{
	view_image_type	*image;

	image=&view.images[0];
	
	image->path[0]=0x0;
	bitmap_close(&image->bitmaps[0].bitmap);
}

/* =======================================================

      Load Single Image
      
======================================================= */

int view_images_find_path(char *path)
{
	int					n;
	view_image_type		*image;
	
	image=view.images;

	for (n=0;n!=max_view_image;n++) {
		if (image->path[0]!=0x0) {
			if (strcmp(image->path,path)==0) return(n);
		}
		image++;
	}
	
	return(-1);
}

int view_images_find_first_free(void)
{
	int					n;
	view_image_type		*image;
	
	image=view.images;

	for (n=0;n!=max_view_image;n++) {
		if (image->path[0]==0x0) return(n);
		image++;
	}
	
	return(-1);
}

/* =======================================================

      Load Single Image
      
======================================================= */

bool view_images_load_single_normal(view_image_type *image,char *path,bool simple)
{
	image->nbitmap=1;
	image->total_msec=0;
	
	if (simple) return(bitmap_open(&image->bitmaps[0].bitmap,path,anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE,FALSE,FALSE));
	return(bitmap_open(&image->bitmaps[0].bitmap,path,setup.anisotropic_mode,setup.mipmap_mode,FALSE,FALSE,FALSE,FALSE));
}

bool view_images_load_single_animated(view_image_type *image,char *path,bool simple)
{
	int				n,animations_head_tag,animation_tag;
	char			*c,xml_path[1024],bitmap_path[1024],name[256];
	
	image->nbitmap=0;
	
		// get rid of .png
		
	c=strrchr(path,'.');
	if (c!=NULL) *c=0x0;
	
		// decode animation xml
		
	sprintf(xml_path,"%s/animation.xml",path);
	if (!xml_open_file(xml_path)) return(FALSE);
	
    animations_head_tag=xml_findrootchild("Animations");
    if (animations_head_tag==-1) {
		xml_close_file();
		return(FALSE);
	}
	
	image->total_msec=0;
	image->nbitmap=xml_countchildren(animations_head_tag);

	animation_tag=xml_findfirstchild("Animation",animations_head_tag);
	
	for (n=0;n!=image->nbitmap;n++) {
		xml_get_attribute_text(animation_tag,"file",name,256);
		
		sprintf(bitmap_path,"%s/%s.png",path,name);
		
		if (simple) {
			if (!bitmap_open(&image->bitmaps[n].bitmap,bitmap_path,anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE,FALSE,FALSE)) return(FALSE);
		}
		else {
			if (!bitmap_open(&image->bitmaps[n].bitmap,bitmap_path,setup.anisotropic_mode,setup.mipmap_mode,FALSE,FALSE,FALSE,FALSE)) return(FALSE);
		}
		
		image->bitmaps[n].msec=xml_get_attribute_int(animation_tag,"msec");
		image->total_msec+=image->bitmaps[n].msec;		
		
		animation_tag=xml_findnextchild(animation_tag);
	}
	
	return(TRUE);
}

int view_images_load_single(char *path,bool simple)
{
	int					idx;
	FILE				*file;
	view_image_type		*image;
	
		// null paths get empty image
		
	if (path==NULL) return(0);

		// already loaded?

	idx=view_images_find_path(path);
	if (idx!=-1) return(idx);
	
		// it's a new image
		// if no free spots, return empty image
		
	idx=view_images_find_first_free();
	if (idx==-1) return(0);
		
	image=&view.images[idx];
	strcpy(image->path,path);

		// if the PNG doesn't exist, then
		// it's a folder based animated image
		
	file=fopen(path,"rb");
	if (file==NULL) {
		if (!view_images_load_single_animated(image,path,simple)) {
			image->path[0]=0x0;
			return(0);
		}
		
		return(idx);
	}
	
	fclose(file);
	
		// normal non-animated file
		
	if (!view_images_load_single_normal(image,path,simple)) {
		image->path[0]=0x0;
		return(0);
	}
	
	return(idx);
}

void view_images_free_single(int idx)
{
	int					n;
	view_image_type		*image;
	
		// don't remove first item as it's always the empty one
		// or -1, which is an error state
		
	if (idx<=0) return;
	
	image=&view.images[idx];
	if (image->path[0]==0x0) return;
	
		// mark as free
		
	image->path[0]=0x0;
   
		// close the bitmaps
	
	for (n=0;n<image->nbitmap;n++) {
		bitmap_close(&image->bitmaps[n].bitmap);
	}
}

/* =======================================================

      Load and Free All Cached Images
      
======================================================= */

void view_images_cached_load(void)
{
	int					n;
	char				path[1024];
	hud_bitmap_type		*hud_bitmap;
	hud_radar_icon_type	*icon;
	halo_type			*halo;
	mark_type			*mark;
	crosshair_type		*crosshair;
	particle_type		*particle;
	ring_type			*ring;
    
		// hud bitmaps

	hud_bitmap=hud.bitmaps;
	
	for (n=0;n!=hud.count.bitmap;n++) {
		file_paths_data(&setup.file_path_setup,path,"Bitmaps/Interface",hud_bitmap->filename,"png");
		hud_bitmap->image_idx=view_images_load_single(path,TRUE);
		hud_bitmap++;
	}

		// radar

	if (hud.radar.on) {
	
			// radar background

		file_paths_data(&setup.file_path_setup,path,"Bitmaps/Radar",hud.radar.background_bitmap_name,"png");
		hud.radar.background_image_idx=view_images_load_single(path,TRUE);

			// radar icons
		
		icon=hud.radar.icons;
		
		for (n=0;n!=hud.radar.nicon;n++) {
			file_paths_data(&setup.file_path_setup,path,"Bitmaps/Radar",icon->bitmap_name,"png");
			icon->image_idx=view_images_load_single(path,TRUE);
			icon++;
		}
	}

		// halos

	halo=server.halos;
	
	for (n=0;n!=server.count.halo;n++) {
		file_paths_data(&setup.file_path_setup,path,"Bitmaps/Halos",halo->bitmap_name,"png");
		halo->image_idx=view_images_load_single(path,FALSE);
		halo++;
	}

		// marks

	mark=server.marks;
	
	for (n=0;n!=server.count.mark;n++) {
		file_paths_data(&setup.file_path_setup,path,"Bitmaps/Marks",mark->bitmap_name,"png");
		mark->image_idx=view_images_load_single(path,FALSE);
		mark++;
	}

		// crosshairs

	crosshair=server.crosshairs;
	
	for (n=0;n!=server.count.crosshair;n++) {
		file_paths_data(&setup.file_path_setup,path,"Bitmaps/Crosshairs",crosshair->bitmap_name,"png");
		crosshair->image_idx=view_images_load_single(path,TRUE);
		crosshair++;
	}

		// particles

	particle=server.particles;
	
	for (n=0;n!=server.count.particle;n++) {
		file_paths_data(&setup.file_path_setup,path,"Bitmaps/Particles",particle->bitmap_name,"png");
		particle->image_idx=view_images_load_single(path,FALSE);
		particle++;
	}
	
		// rings

	ring=server.rings;
	
	for (n=0;n!=server.count.ring;n++) {
		file_paths_data(&setup.file_path_setup,path,"Bitmaps/Rings",ring->bitmap_name,"png");
		ring->image_idx=view_images_load_single(path,FALSE);
		ring++;
	}

		// remote bitmaps

	if (net_setup.client.joined) {
		file_paths_data(&setup.file_path_setup,path,"Bitmaps/Network","slow","png");
		remote_slow_image_idx=view_images_load_single(path,TRUE);
	
		file_paths_data(&setup.file_path_setup,path,"Bitmaps/Network","talk","png");
		remote_talk_image_idx=view_images_load_single(path,TRUE);
	}
}

void view_images_cached_free(void)
{
	int					n;
	hud_bitmap_type		*hud_bitmap;
	hud_radar_icon_type	*icon;
	halo_type			*halo;
	mark_type			*mark;
	crosshair_type		*crosshair;
	particle_type		*particle;
	ring_type			*ring;
    
		// hud bitmaps

	hud_bitmap=hud.bitmaps;
	
	for (n=0;n!=hud.count.bitmap;n++) {
		view_images_free_single(hud_bitmap->image_idx);
	}

		// radar

	if (hud.radar.on) {
	
			// radar background

		view_images_free_single(hud.radar.background_image_idx);

			// radar icons
		
		icon=hud.radar.icons;
		
		for (n=0;n!=hud.radar.nicon;n++) {
			view_images_free_single(icon->image_idx);
			icon++;
		}
	}

		// halos

	halo=server.halos;
	
	for (n=0;n!=server.count.halo;n++) {
		view_images_free_single(halo->image_idx);
		halo++;
	}

		// marks

	mark=server.marks;
	
	for (n=0;n!=server.count.mark;n++) {
		view_images_free_single(mark->image_idx);
		mark++;
	}

		// crosshairs

	crosshair=server.crosshairs;
	
	for (n=0;n!=server.count.crosshair;n++) {
		view_images_free_single(crosshair->image_idx);
		crosshair++;
	}

		// particles

	particle=server.particles;
	
	for (n=0;n!=server.count.particle;n++) {
		view_images_free_single(particle->image_idx);
		particle++;
	}
	
		// rings

	ring=server.rings;
	
	for (n=0;n!=server.count.ring;n++) {
		view_images_free_single(ring->image_idx);
		ring++;
	}

		// remote bitmaps

	if (net_setup.client.joined) {
		view_images_free_single(remote_slow_image_idx);
		view_images_free_single(remote_talk_image_idx);
	}
}

/* =======================================================

      Get Images
      
======================================================= */

bitmap_type* view_images_get_bitmap(int idx)
{
	int						n,tick,msec;
	view_image_type			*image;
	view_image_bitmap_type	*ibm;
	
		// sanity check for bad bitmaps
		
	if ((idx<0) || (idx>=max_view_image)) idx=0;
	
	image=&view.images[idx];
	
		// no animations
		
	if ((image->nbitmap<=1) || (image->total_msec==0)) return(&image->bitmaps[0].bitmap);
	
		// get tick, get it from game
		// if there (fast) or calculate (slow)
	
	if ((server.time.paused) || (server.state!=gs_running)) {
		tick=time_get();
	}
	else {
		tick=game_time_get();
	}
	
		// run animation
		
	msec=0;
	tick=(tick%image->total_msec);
	
	ibm=image->bitmaps;
	
	for (n=0;n!=image->nbitmap;n++) {
		msec+=ibm->msec;
		if (tick<=msec) return(&image->bitmaps[n].bitmap);
		ibm++;
	}
	
	return(&image->bitmaps[0].bitmap);
}

unsigned long view_images_get_gl_id(int idx)
{
	bitmap_type			*bitmap;
	
	bitmap=view_images_get_bitmap(idx);
	return(bitmap->gl_id);
}
