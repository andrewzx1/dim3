/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Main Interface Setup

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

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3baseutility.h"
#endif

d3col							default_tint_cols[8]={{1.0f,1.0f,1.0f},{1.0f,1.0f,0.0f},{1.0f,0.6f,0.0f},{1.0f,0.0f,0.0f},{0.0f,1.0f,0.0f},{0.0f,0.0f,1.0f},{1.0f,0.0f,1.0f},{0.6f,0.4f,0.0f}};
file_path_setup_type			iface_file_path_setup;

/* =======================================================

      Interface Start
      
======================================================= */

bool iface_initialize(iface_type *iface,file_path_setup_type *path_setup)
{
		// remember path setup

	memmove(&iface_file_path_setup,path_setup,sizeof(file_path_setup_type));

		// initialize counts

	iface->bitmap_list.nbitmap=0;
	iface->text_list.ntext=0;
	iface->bar_list.nbar=0;
	iface->particle_list.nparticle=0;
	iface->ring_list.nring=0;
	iface->mark_list.nmark=0;
	iface->halo_list.nhalo=0;
	iface->crosshair_list.ncrosshair=0;
	iface->sound_list.nsound=0;
	iface->shader_list.nshader=0;

		// initialize pointers
		
	iface->bitmap_list.bitmaps=NULL;
	iface->text_list.texts=NULL;
	iface->bar_list.bars=NULL;
	iface->menu_list.menus=NULL;
	iface->chooser_list.choosers=NULL;
	iface->particle_list.particles=NULL;
	iface->ring_list.rings=NULL;
	iface->mark_list.marks=NULL;
	iface->halo_list.halos=NULL;
	iface->crosshair_list.crosshairs=NULL;
	iface->action_display_list.action_displays=NULL;
	iface->sound_list.sounds=NULL;
	iface->shader_list.shaders=NULL;

		// iface pointers

	iface->bitmap_list.bitmaps=(iface_bitmap_type*)malloc(max_iface_bitmap*sizeof(iface_bitmap_type));
	if (iface->bitmap_list.bitmaps==NULL) return(FALSE);
	
	iface->text_list.texts=(iface_text_type*)malloc(max_iface_text*sizeof(iface_text_type));
	if (iface->text_list.texts==NULL) return(FALSE);
	
	iface->bar_list.bars=(iface_bar_type*)malloc(max_iface_bar*sizeof(iface_bar_type));
	if (iface->bar_list.bars==NULL) return(FALSE);

	iface->menu_list.menus=(iface_menu_type*)malloc(max_iface_menu*sizeof(iface_menu_type));
	if (iface->menu_list.menus==NULL) return(FALSE);
	
	iface->chooser_list.choosers=(iface_chooser_type*)malloc(max_iface_chooser*sizeof(iface_chooser_type));
	if (iface->chooser_list.choosers==NULL) return(FALSE);

	iface->particle_list.particles=(iface_particle_type*)malloc(max_iface_particle*sizeof(iface_particle_type));
	if (iface->particle_list.particles==NULL) return(FALSE);

	iface->ring_list.rings=(iface_ring_type*)malloc(max_iface_ring*sizeof(iface_ring_type));
	if (iface->ring_list.rings==NULL) return(FALSE);

	iface->mark_list.marks=(iface_mark_type*)malloc(max_iface_mark*sizeof(iface_mark_type));
	if (iface->mark_list.marks==NULL) return(FALSE);

	iface->halo_list.halos=(iface_halo_type*)malloc(max_iface_halo*sizeof(iface_halo_type));
	if (iface->halo_list.halos==NULL) return(FALSE);

	iface->crosshair_list.crosshairs=(iface_crosshair_type*)malloc(max_iface_crosshair*sizeof(iface_crosshair_type));
	if (iface->crosshair_list.crosshairs==NULL) return(FALSE);

	iface->action_display_list.action_displays=(iface_action_display_type*)malloc(max_iface_action*sizeof(iface_action_display_type));
	if (iface->action_display_list.action_displays==NULL) return(FALSE);

	iface->sound_list.sounds=(iface_sound_type*)malloc(max_iface_sound*sizeof(iface_sound_type));
	if (iface->sound_list.sounds==NULL) return(FALSE);

	iface->shader_list.shaders=(iface_shader_type*)malloc(max_iface_user_shader*sizeof(iface_shader_type));
	if (iface->shader_list.shaders==NULL) return(FALSE);
	
		// zero memory
		
	bzero(iface->bitmap_list.bitmaps,(max_iface_bitmap*sizeof(iface_bitmap_type)));
	bzero(iface->text_list.texts,(max_iface_text*sizeof(iface_text_type)));
	bzero(iface->bar_list.bars,(max_iface_bar*sizeof(iface_bar_type)));
	bzero(iface->menu_list.menus,(max_iface_menu*sizeof(iface_menu_type)));
	bzero(iface->chooser_list.choosers,(max_iface_chooser*sizeof(iface_chooser_type)));
	bzero(iface->particle_list.particles,(max_iface_particle*sizeof(iface_particle_type)));
	bzero(iface->ring_list.rings,(max_iface_ring*sizeof(iface_ring_type)));
	bzero(iface->mark_list.marks,(max_iface_mark*sizeof(iface_mark_type)));
	bzero(iface->halo_list.halos,(max_iface_halo*sizeof(iface_halo_type)));
	bzero(iface->crosshair_list.crosshairs,(max_iface_crosshair*sizeof(iface_crosshair_type)));
	bzero(iface->action_display_list.action_displays,(max_iface_action*sizeof(iface_action_display_type)));
	bzero(iface->sound_list.sounds,(max_iface_sound*sizeof(iface_sound_type)));
	bzero(iface->shader_list.shaders,(max_iface_user_shader*sizeof(iface_shader_type)));

	return(TRUE);
}

void iface_shutdown(iface_type *iface)
{
		// hud pointers
		
	if (iface->bitmap_list.bitmaps!=NULL) free(iface->bitmap_list.bitmaps);
	if (iface->text_list.texts!=NULL) free(iface->text_list.texts);
	if (iface->bar_list.bars!=NULL) free(iface->bar_list.bars);
	if (iface->menu_list.menus!=NULL) free(iface->menu_list.menus);
	if (iface->chooser_list.choosers!=NULL) free(iface->chooser_list.choosers);
	if (iface->particle_list.particles!=NULL) free(iface->particle_list.particles);
	if (iface->ring_list.rings!=NULL) free(iface->ring_list.rings);
	if (iface->mark_list.marks!=NULL) free(iface->mark_list.marks);
	if (iface->halo_list.halos!=NULL) free(iface->halo_list.halos);
	if (iface->crosshair_list.crosshairs!=NULL) free(iface->crosshair_list.crosshairs);
	if (iface->action_display_list.action_displays!=NULL) free(iface->action_display_list.action_displays);
	if (iface->sound_list.sounds!=NULL) free(iface->sound_list.sounds);
	if (iface->shader_list.shaders!=NULL) free(iface->shader_list.shaders);
}

/* =======================================================

      Default Interface Settings
      
======================================================= */

void iface_default_settings_button(iface_intro_button_type *button,int x,int y,bool on)
{
	button->x=x;
	button->y=y;
	button->mobile_x=-1;
	button->mobile_y=-1;
	button->wid=128;
	button->high=32;
	button->on=on;
	button->mobile_hide=FALSE;
}

void iface_default_settings(iface_type *iface)
{
	int				n;
	
		// project setup
	
	iface->project.name[0]=0x0;
	iface->project.modernize=TRUE;
	iface->project.skill=FALSE;
	
		// scale

	iface->scale_x=640;
	iface->scale_y=480;

		// items
		
	iface->bitmap_list.nbitmap=0;
	iface->text_list.ntext=0;
	iface->bar_list.nbar=0;
	iface->menu_list.nmenu=0;
	iface->chooser_list.nchooser=0;
	iface->particle_list.nparticle=0;
	iface->ring_list.nring=0;
	iface->mark_list.nmark=0;
	iface->halo_list.nhalo=0;
	iface->crosshair_list.ncrosshair=0;
	iface->sound_list.nsound=0;
	iface->shader_list.nshader=0;
	
		// virtual controls
		
	for (n=0;n!=max_virtual_stick;n++) {
		iface->virtual_control.sticks[n].on=FALSE;
		iface->virtual_control.sticks[n].color.r=iface->virtual_control.sticks[n].color.g=iface->virtual_control.sticks[n].color.b=1.0f;
	}

	for (n=0;n!=max_virtual_button;n++) {
		iface->virtual_control.buttons[n].on=FALSE;
		iface->virtual_control.buttons[n].control_idx=0;
		iface->virtual_control.buttons[n].color.r=iface->virtual_control.buttons[n].color.g=iface->virtual_control.buttons[n].color.b=1.0f;
	}

		// radar

	iface->radar.on=FALSE;
	
		// sounds
		
	iface->click_sound[0]=0x0;

		// setup

	iface->setup.game_video=TRUE;
	iface->setup.game_audio=TRUE;
	iface->setup.game_control=TRUE;
	iface->setup.game_action=TRUE;
	iface->setup.game_debug=FALSE;
	iface->setup.net_player=TRUE;
	iface->setup.net_host=TRUE;		
	iface->setup.no_resolution_switch=FALSE;
	iface->setup.allow_auto_aim=FALSE;
	
		// colors
		
	iface->color.background.r=iface->color.background.g=iface->color.background.b=0.0f;
	
	iface->color.menu.text.r=iface->color.menu.text.g=iface->color.menu.text.b=0.0f;
	iface->color.menu.mouse_over.r=iface->color.menu.mouse_over.g=iface->color.menu.mouse_over.b=0.9f;
	iface->color.menu.background.r=iface->color.menu.background.g=iface->color.menu.background.b=0.7f;
	iface->color.menu.outline.r=iface->color.menu.outline.g=iface->color.menu.outline.b=1.0f;
		
	iface->color.dialog.background.r=iface->color.dialog.background.g=iface->color.dialog.background.b=0.0f;
	iface->color.dialog.header.r=iface->color.dialog.header.g=iface->color.dialog.header.b=0.7f;
	iface->color.dialog.outline.r=iface->color.dialog.outline.g=iface->color.dialog.outline.b=0.8f;
	iface->color.dialog.title.r=iface->color.dialog.title.g=iface->color.dialog.title.b=1.0f;

	iface->color.tab.background.r=iface->color.tab.background.g=iface->color.tab.background.b=0.6f;
	iface->color.tab.dimmed.r=iface->color.tab.dimmed.g=iface->color.tab.dimmed.b=0.4f;
	iface->color.tab.outline.r=iface->color.tab.outline.g=iface->color.tab.outline.b=1.0f;
	iface->color.tab.text.r=iface->color.tab.text.g=iface->color.tab.text.b=1.0f;
	iface->color.tab.text_mouse_over.r=iface->color.tab.text_mouse_over.g=iface->color.tab.text_mouse_over.b=0.3f;
	iface->color.tab.text_dimmed.r=iface->color.tab.text_dimmed.g=iface->color.tab.text_dimmed.b=0.1f;
	
	iface->color.control.label.r=iface->color.control.label.g=iface->color.control.label.b=0.8f;
	iface->color.control.text.r=iface->color.control.text.g=iface->color.control.text.b=0.0f;
	iface->color.control.fill.r=iface->color.control.fill.g=iface->color.control.fill.b=0.6f;
	iface->color.control.header.r=iface->color.control.header.b=1.0f;
	iface->color.control.header.g=0.3f;
	iface->color.control.outline.r=iface->color.control.outline.g=iface->color.control.outline.b=0.0f;
	
	iface->color.control.mouse_over.b=1.0f;
	iface->color.control.mouse_over.r=iface->color.control.mouse_over.g=0.3f;
	iface->color.control.hilite.r=iface->color.control.hilite.b=1.0f;
	iface->color.control.hilite.g=0.3f;
	iface->color.control.disabled.r=iface->color.control.disabled.g=iface->color.control.disabled.b=0.3f;

	iface->color.scrollbar.background.r=iface->color.scrollbar.background.g=iface->color.scrollbar.background.b=0.7f;
	iface->color.scrollbar.thumb.r=iface->color.scrollbar.thumb.g=iface->color.scrollbar.thumb.b=0.5f;

	iface->color.button.fill.r=iface->color.button.fill.g=iface->color.button.fill.b=0.6f;
	iface->color.button.text.r=iface->color.button.text.g=iface->color.button.text.b=0.0f;
	iface->color.button.outline.r=iface->color.button.outline.g=iface->color.button.outline.b=0.0f;

	iface->color.system.metric.r=iface->color.system.metric.g=iface->color.system.metric.b=1.0f;
	
	iface->color.default_tint.r=iface->color.default_tint.g=iface->color.default_tint.b=1.0f;

	for (n=0;n!=8;n++) {
		memmove(&iface->color.tints[n],&default_tint_cols[n],sizeof(d3col));
	}
	
		// fonts

	for (n=0;n!=max_iface_font_variant;n++) {
		strcpy(iface->font.interface_name[n],"Arial");
		strcpy(iface->font.hud_name[n],"Arial");
	}

		// progress
		
	iface->progress.lx=0;
	iface->progress.rx=640;
	iface->progress.ty=450;
	iface->progress.by=470;
	iface->progress.outline=TRUE;
	iface->progress.overlay=FALSE;
	iface->progress.base_color_start.r=0.4f;
	iface->progress.base_color_start.g=iface->progress.base_color_start.b=0.0f;
	iface->progress.base_color_end.r=1.0f;
	iface->progress.base_color_end.g=iface->progress.base_color_end.b=0.0f;
	iface->progress.hilite_color_start.g=0.4f;
	iface->progress.hilite_color_start.r=iface->progress.hilite_color_start.b=0.0f;
	iface->progress.hilite_color_end.g=1.0f;
	iface->progress.hilite_color_end.r=iface->progress.hilite_color_end.b=0.0f;
	iface->progress.outline_color.r=iface->progress.outline_color.g=iface->progress.outline_color.b=1.0f;
	
		// chat
		
	iface->chat.x=637;
	iface->chat.y=479;
	iface->chat.last_add_life_sec=15;
	iface->chat.next_life_sec=5;

		// fades

	iface->fade.title_msec=1000;
	iface->fade.map_msec=300;
	
		// intro

	iface_default_settings_button(&iface->intro.button_game_new,0,0,TRUE);
	iface_default_settings_button(&iface->intro.button_game_load,0,32,TRUE);
	iface_default_settings_button(&iface->intro.button_game_setup,0,64,TRUE);
	iface_default_settings_button(&iface->intro.button_game_new_easy,128,0,TRUE);
	iface_default_settings_button(&iface->intro.button_game_new_medium,128,32,TRUE);
	iface_default_settings_button(&iface->intro.button_game_new_hard,128,64,TRUE);
	iface_default_settings_button(&iface->intro.button_game_new_cancel,128,96,TRUE);

	iface_default_settings_button(&iface->intro.button_multiplayer_host,0,96,TRUE);
	iface_default_settings_button(&iface->intro.button_multiplayer_join,0,128,TRUE);
	iface_default_settings_button(&iface->intro.button_multiplayer_setup,0,160,TRUE);

	iface_default_settings_button(&iface->intro.button_credit,0,192,TRUE);
	iface_default_settings_button(&iface->intro.button_quit,0,224,TRUE);

	iface->intro.simple_save_list.desc.text_size=20;
	
	iface->intro.simple_save_list.progress.on=FALSE;
	iface->intro.simple_save_list.progress.max_point=100;
	iface->intro.simple_save_list.progress.max_bitmap=5;
	iface->intro.simple_save_list.progress.wid=-1;
	iface->intro.simple_save_list.progress.high=-1;
	iface->intro.simple_save_list.progress.bitmap_add=16;
	iface->intro.simple_save_list.progress.horizontal=TRUE;
	iface->intro.simple_save_list.progress.wrap_count=-1;
	iface->intro.simple_save_list.progress.bitmap_name[0]=0x0;
	iface->intro.simple_save_list.progress.bitmap_disable_name[0]=0x0;

	iface->intro.score.on=FALSE;
	iface->intro.score.x=0;
	iface->intro.score.y=0;
	iface->intro.score.wid=150;
	iface->intro.score.high=200;
	iface->intro.score.text_size=20;
	iface->intro.score.col.r=iface->intro.score.col.g=iface->intro.score.col.b=1.0f;
	
	for (n=0;n!=max_simple_save_spot;n++) {
		iface_default_settings_button(&iface->intro.simple_save_list.saves[n].button_start,0,(n*32),FALSE);
		iface_default_settings_button(&iface->intro.simple_save_list.saves[n].button_erase,40,(n*32),FALSE);
		iface->intro.simple_save_list.saves[n].desc.x=80;
		iface->intro.simple_save_list.saves[n].desc.y=(n*32);
		iface->intro.simple_save_list.saves[n].progress.x=120;
		iface->intro.simple_save_list.saves[n].progress.y=(n*32);
	}
	
	iface->intro.confirm.x=0;
	iface->intro.confirm.y=0;

	iface->intro.model_list.nmodel=0;
	
	iface->intro.music[0]=0x0;

	iface->logo.name[0]=0x0;
	iface->logo.sound[0]=0x0;
	iface->logo.life_msec=2000;

		// player models

	iface->character.ncharacter=0;

		// multiplayer

	iface->net_game.ngame=1;
	strcpy(iface->net_game.games[0].name,"Deathmatch");

	iface->net_option.noption=0;
		
	iface->net_bot.on=TRUE;

	for (n=0;n!=max_net_bot;n++) {
		iface->net_bot.bots[n].name[0]=0x0;
	}
	
	iface->net_news.host[0]=0x0;
	iface->net_news.port=80;
	iface->net_news.url[0]=0x0;
}

/* =======================================================

      Finds
      
======================================================= */

iface_bitmap_type* iface_bitmaps_find(iface_type *iface,char *name)
{
	int					n;
	iface_bitmap_type	*bitmap;
	
	bitmap=iface->bitmap_list.bitmaps;
	
	for (n=0;n!=iface->bitmap_list.nbitmap;n++) {
		if (strcasecmp(bitmap->name,name)==0) return(bitmap);
		bitmap++;
	}
	
	return(NULL);
}

iface_text_type* iface_texts_find(iface_type *iface,char *name)
{
	int					n;
	iface_text_type		*text;
	
	text=iface->text_list.texts;
	
	for (n=0;n!=iface->text_list.ntext;n++) {
		if (strcasecmp(text->name,name)==0) return(text);
		text++;
	}
	
	return(NULL);
}

iface_bar_type* iface_bars_find(iface_type *iface,char *name)
{
	int					n;
	iface_bar_type		*bar;
	
	bar=iface->bar_list.bars;
	
	for (n=0;n!=iface->bar_list.nbar;n++) {
		if (strcasecmp(bar->name,name)==0) return(bar);
		bar++;
	}
	
	return(NULL);
}

int iface_menu_find_idx(iface_type *iface,char *name)
{
	int				n;
	iface_menu_type	*menu;
	
	menu=iface->menu_list.menus;
	
	for (n=0;n!=iface->menu_list.nmenu;n++) {
		if (strcasecmp(menu->name,name)==0) return(n);
		menu++;
	}
	
	return(-1);
}

int iface_chooser_find_idx(iface_type *iface,char *name)
{
	int					n;
	iface_chooser_type	*chooser;
	
	chooser=iface->chooser_list.choosers;
	
	for (n=0;n!=iface->chooser_list.nchooser;n++) {
		if (strcasecmp(chooser->name,name)==0) return(n);
		chooser++;
	}
	
	return(-1);
}

int iface_chooser_find_piece_idx(iface_chooser_type *chooser,int id)
{
	int							n;
	iface_chooser_piece_type	*piece;

	piece=chooser->pieces;

	for (n=0;n!=chooser->npiece;n++) {
		if (piece->id==id) return(n);
		piece++;
	}

	return(-1);
}

int iface_crosshair_find(iface_type *iface,char *name)
{
	int						n;
	iface_crosshair_type	*crosshair;

	crosshair=iface->crosshair_list.crosshairs;

	for (n=0;n!=iface->crosshair_list.ncrosshair;n++) {
		if (strcasecmp(crosshair->name,name)==0)  return(n);
		crosshair++;
	}
	
	return(-1);
}

int iface_halo_find(iface_type *iface,char *name)
{
	int				n;
	iface_halo_type	*halo;

	halo=iface->halo_list.halos;

	for (n=0;n!=iface->halo_list.nhalo;n++) {
		if (strcasecmp(halo->name,name)==0)  return(n);
		halo++;
	}
	
	return(-1);
}

/* =======================================================

      Misc Routines
      
======================================================= */

void iface_text_set(iface_text_type *text,char *data)
{
	strcpy(text->data,data);
	text->has_return=(strstr(data,"{r}")!=NULL);
}

void iface_bitmaps_hide_all(iface_type *iface)
{
	int					n;
	iface_bitmap_type	*bitmap;

	bitmap=iface->bitmap_list.bitmaps;
	
	for (n=0;n!=iface->bitmap_list.nbitmap;n++) {
		bitmap->show=FALSE;
		bitmap++;
	}
}

void iface_texts_hide_all(iface_type *iface)
{
	int				n;
	iface_text_type	*text;

	text=iface->text_list.texts;
	
	for (n=0;n!=iface->text_list.ntext;n++) {
		text->show=FALSE;
		text++;
	}
}

void iface_bars_hide_all(iface_type *iface)
{
	int				n;
	iface_bar_type	*bar;

	bar=iface->bar_list.bars;
	
	for (n=0;n!=iface->bar_list.nbar;n++) {
		bar->show=FALSE;
		bar++;
	}
}

/* =======================================================

      Read and Write
      
======================================================= */

void iface_read(iface_type *iface)
{
	iface_read_settings_interface(iface);
	iface_read_settings_chooser(iface);
	iface_read_settings_multiplayer(iface);
	iface_read_settings_particle(iface);
	iface_read_settings_ring(iface);
	iface_read_settings_halo(iface);
	iface_read_settings_mark(iface);
	iface_read_settings_crosshair(iface);
	iface_read_settings_sound(iface);
	iface_read_settings_action(iface);
	iface_read_settings_shader(iface);
}

void iface_refresh_hud_only(iface_type *iface)
{
	iface_refresh_settings_interface_hud_only(iface);
}

bool iface_write(iface_type *iface,char *err_str)
{
	if (!iface_write_settings_interface(iface,err_str)) return(FALSE);
	if (!iface_write_settings_chooser(iface,err_str)) return(FALSE);
	if (!iface_write_settings_multiplayer(iface,err_str)) return(FALSE);
	if (!iface_write_settings_particle(iface,err_str)) return(FALSE);
	if (!iface_write_settings_ring(iface,err_str)) return(FALSE);
	if (!iface_write_settings_halo(iface,err_str)) return(FALSE);
	if (!iface_write_settings_mark(iface,err_str)) return(FALSE);
	if (!iface_write_settings_crosshair(iface,err_str)) return(FALSE);
	if (!iface_write_settings_sound(iface,err_str)) return(FALSE);
	if (!iface_write_settings_action(iface,err_str)) return(FALSE);
	if (!iface_write_settings_shader(iface,err_str)) return(FALSE);
	
	return(TRUE);
}


