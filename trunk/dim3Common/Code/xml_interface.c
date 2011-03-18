/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Interface XML Setup

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

#ifdef D3_PCH
	#ifdef D3_ENGINE
		#include "dim3engine.h"
	#endif
	#ifdef D3_EDITOR
		#include "dim3Editor.h"
	#endif
	#ifdef D3_ANIMATOR
		#include "dim3Animator.h"
	#endif
	#ifdef D3_SETUP
		#include "dim3Setup.h"
	#endif
#endif

#include "interface.h"

extern iface_type				iface;
extern setup_type				setup;
extern network_setup_type		net_setup;

d3col							default_tint_cols[8]={{1.0f,1.0f,1.0f},{1.0f,1.0f,0.0f},{1.0f,0.6f,0.0f},{1.0f,0.0f,0.0f},{0.0f,1.0f,0.0f},{0.0f,0.0f,1.0f},{1.0f,0.0f,1.0f},{0.6f,0.4f,0.0f}};

/* =======================================================

      Interface Start
      
======================================================= */

bool interface_initialize(void)
{
		// initialize counts

	iface.bitmap_list.nbitmap=0;
	iface.text_list.ntext=0;
	iface.bar_list.nbar=0;
	iface.particle_list.nparticle=0;
	iface.ring_list.nring=0;
	iface.mark_list.nmark=0;
	iface.halo_list.nhalo=0;
	iface.crosshair_list.ncrosshair=0;
	iface.sound_list.nsound=0;
	iface.shader_list.nshader=0;

		// initialize pointers
		
	iface.chat.lines=NULL;

	iface.bitmap_list.bitmaps=NULL;
	iface.text_list.texts=NULL;
	iface.bar_list.bars=NULL;
	iface.menu_list.menus=NULL;
	iface.chooser_list.choosers=NULL;
	iface.particle_list.particles=NULL;
	iface.ring_list.rings=NULL;
	iface.mark_list.marks=NULL;
	iface.halo_list.halos=NULL;
	iface.crosshair_list.crosshairs=NULL;
	iface.action_display_list.action_displays=NULL;
	iface.sound_list.sounds=NULL;
	iface.shader_list.shaders=NULL;

		// iface pointers

	iface.bitmap_list.bitmaps=(iface_bitmap_type*)malloc(max_iface_bitmap*sizeof(iface_bitmap_type));
	if (iface.bitmap_list.bitmaps==NULL) return(FALSE);
	
	iface.text_list.texts=(iface_text_type*)malloc(max_iface_text*sizeof(iface_text_type));
	if (iface.text_list.texts==NULL) return(FALSE);
	
	iface.bar_list.bars=(iface_bar_type*)malloc(max_iface_bar*sizeof(iface_bar_type));
	if (iface.bar_list.bars==NULL) return(FALSE);

	iface.menu_list.menus=(iface_menu_type*)malloc(max_iface_menu*sizeof(iface_menu_type));
	if (iface.menu_list.menus==NULL) return(FALSE);
	
	iface.chooser_list.choosers=(iface_chooser_type*)malloc(max_iface_chooser*sizeof(iface_chooser_type));
	if (iface.chooser_list.choosers==NULL) return(FALSE);
	
	iface.chat.lines=(hud_chat_line_type*)malloc(max_chat_lines*sizeof(hud_chat_line_type));
	if (iface.chat.lines==NULL) return(FALSE);

	iface.particle_list.particles=(iface_particle_type*)malloc(max_iface_particle*sizeof(iface_particle_type));
	if (iface.particle_list.particles==NULL) return(FALSE);

	iface.ring_list.rings=(iface_ring_type*)malloc(max_iface_ring*sizeof(iface_ring_type));
	if (iface.ring_list.rings==NULL) return(FALSE);

	iface.mark_list.marks=(iface_mark_type*)malloc(max_iface_mark*sizeof(iface_mark_type));
	if (iface.mark_list.marks==NULL) return(FALSE);

	iface.halo_list.halos=(iface_halo_type*)malloc(max_iface_halo*sizeof(iface_halo_type));
	if (iface.halo_list.halos==NULL) return(FALSE);

	iface.crosshair_list.crosshairs=(iface_crosshair_type*)malloc(max_iface_crosshair*sizeof(iface_crosshair_type));
	if (iface.crosshair_list.crosshairs==NULL) return(FALSE);

	iface.action_display_list.action_displays=(iface_action_display_type*)malloc(max_iface_action*sizeof(iface_action_display_type));
	if (iface.action_display_list.action_displays==NULL) return(FALSE);

	iface.sound_list.sounds=(iface_sound_type*)malloc(max_iface_sound*sizeof(iface_sound_type));
	if (iface.sound_list.sounds==NULL) return(FALSE);

	iface.shader_list.shaders=(iface_shader_type*)malloc(max_iface_user_shader*sizeof(iface_shader_type));
	if (iface.shader_list.shaders==NULL) return(FALSE);
	
		// zero memory
		
	bzero(iface.bitmap_list.bitmaps,(max_iface_bitmap*sizeof(iface_bitmap_type)));
	bzero(iface.text_list.texts,(max_iface_text*sizeof(iface_text_type)));
	bzero(iface.bar_list.bars,(max_iface_bar*sizeof(iface_bar_type)));
	bzero(iface.menu_list.menus,(max_iface_menu*sizeof(iface_menu_type)));
	bzero(iface.chooser_list.choosers,(max_iface_chooser*sizeof(iface_chooser_type)));
	bzero(iface.chat.lines,(max_chat_lines*sizeof(hud_chat_line_type)));

	bzero(iface.particle_list.particles,(max_iface_particle*sizeof(iface_particle_type)));
	bzero(iface.ring_list.rings,(max_iface_ring*sizeof(iface_ring_type)));
	bzero(iface.mark_list.marks,(max_iface_mark*sizeof(iface_mark_type)));
	bzero(iface.halo_list.halos,(max_iface_halo*sizeof(iface_halo_type)));
	bzero(iface.crosshair_list.crosshairs,(max_iface_crosshair*sizeof(iface_crosshair_type)));
	bzero(iface.action_display_list.action_displays,(max_iface_action*sizeof(iface_action_display_type)));
	bzero(iface.sound_list.sounds,(max_iface_sound*sizeof(iface_sound_type)));
	bzero(iface.shader_list.shaders,(max_iface_user_shader*sizeof(iface_shader_type)));

	return(TRUE);
}

void interface_shutdown(void)
{
		// hud pointers
		
	if (iface.bitmap_list.bitmaps!=NULL) free(iface.bitmap_list.bitmaps);
	if (iface.text_list.texts!=NULL) free(iface.text_list.texts);
	if (iface.bar_list.bars!=NULL) free(iface.bar_list.bars);
	if (iface.menu_list.menus!=NULL) free(iface.menu_list.menus);
	if (iface.chooser_list.choosers!=NULL) free(iface.chooser_list.choosers);
	if (iface.chat.lines!=NULL) free(iface.chat.lines);

	if (iface.particle_list.particles!=NULL) free(iface.particle_list.particles);
	if (iface.ring_list.rings!=NULL) free(iface.ring_list.rings);
	if (iface.mark_list.marks!=NULL) free(iface.mark_list.marks);
	if (iface.halo_list.halos!=NULL) free(iface.halo_list.halos);
	if (iface.crosshair_list.crosshairs!=NULL) free(iface.crosshair_list.crosshairs);
	if (iface.action_display_list.action_displays!=NULL) free(iface.action_display_list.action_displays);
	if (iface.sound_list.sounds!=NULL) free(iface.sound_list.sounds);
	if (iface.shader_list.shaders!=NULL) free(iface.shader_list.shaders);
}

/* =======================================================

      Default Interface Settings
      
======================================================= */

void default_settings_interface_button(hud_intro_button_type *button,int x,int y,bool on)
{
	button->x=x;
	button->y=y;
	button->wid=128;
	button->high=32;
	button->on=on;
}

void default_settings_interface(void)
{
	int				n;
	
		// scale

	iface.scale_x=640;
	iface.scale_y=480;

		// items
		
	iface.bitmap_list.nbitmap=0;
	iface.text_list.ntext=0;
	iface.bar_list.nbar=0;
	iface.menu_list.nmenu=0;
	iface.chooser_list.nchooser=0;
	iface.particle_list.nparticle=0;
	iface.ring_list.nring=0;
	iface.mark_list.nmark=0;
	iface.halo_list.nhalo=0;
	iface.crosshair_list.ncrosshair=0;
	iface.sound_list.nsound=0;
	iface.shader_list.nshader=0;

		// radar

	iface.radar.on=FALSE;
	
		// project setup
		
	iface.skill=FALSE;
	
		// sounds and music
		
	iface.click_sound[0]=0x0;
	iface.intro_music[0]=0x0;

		// debug mode

	iface.debug=FALSE;
	
		// colors
		
	iface.color.dialog_background.r=iface.color.dialog_background.g=iface.color.dialog_background.b=0.0f;
	iface.color.dialog_base.r=iface.color.dialog_base.g=iface.color.dialog_base.b=0.6f;
	iface.color.dialog_dimmed.r=iface.color.dialog_dimmed.g=iface.color.dialog_dimmed.b=0.4f;
	iface.color.dialog_outline.r=iface.color.dialog_outline.g=iface.color.dialog_outline.b=0.8f;
	
	iface.color.control_label.r=iface.color.control_label.g=iface.color.control_label.b=0.8f;
	iface.color.control_text.r=iface.color.control_text.g=iface.color.control_text.b=0.0f;
	iface.color.control_fill.r=iface.color.control_fill.g=iface.color.control_fill.b=0.6f;
	iface.color.control_header.r=iface.color.control_header.b=1.0f;
	iface.color.control_header.g=0.3f;
	iface.color.control_outline.r=iface.color.control_outline.g=iface.color.control_outline.b=0.0f;
	
	iface.color.control_mouse_over.b=1.0f;
	iface.color.control_mouse_over.r=iface.color.control_mouse_over.g=0.3f;
	iface.color.control_hilite.r=iface.color.control_hilite.b=1.0f;
	iface.color.control_hilite.g=0.3f;
	iface.color.control_disabled.r=iface.color.control_disabled.g=iface.color.control_disabled.b=0.3f;

	iface.color.button_fill.r=iface.color.button_fill.g=iface.color.button_fill.b=0.6f;
	iface.color.button_text.r=iface.color.button_text.g=iface.color.button_text.b=0.0f;
	iface.color.button_outline.r=iface.color.button_outline.g=iface.color.button_outline.b=0.0f;
	
	iface.color.default_tint.r=iface.color.default_tint.g=iface.color.default_tint.b=1.0f;

	for (n=0;n!=8;n++) {
		memmove(&iface.color.tints[n],&default_tint_cols[n],sizeof(d3col));
	}
	
		// fonts

	strcpy(iface.font.interface_name,"Arial");
	strcpy(iface.font.hud_name,"Arial");
	
		// progress
		
	iface.progress.lx=0;
	iface.progress.rx=640;
	iface.progress.ty=450;
	iface.progress.by=470;
	iface.progress.outline=TRUE;
	iface.progress.base_color_start.r=0.4f;
	iface.progress.base_color_start.g=iface.progress.base_color_start.b=0.0f;
	iface.progress.base_color_end.r=1.0f;
	iface.progress.base_color_end.g=iface.progress.base_color_end.b=0.0f;
	iface.progress.hilite_color_start.g=0.4f;
	iface.progress.hilite_color_start.r=iface.progress.hilite_color_start.b=0.0f;
	iface.progress.hilite_color_end.g=1.0f;
	iface.progress.hilite_color_end.r=iface.progress.hilite_color_end.b=0.0f;
	iface.progress.text_size=20;
	iface.progress.text_color.r=iface.progress.text_color.g=iface.progress.text_color.b=0.0f;
	iface.progress.outline_color.r=iface.progress.outline_color.g=iface.progress.outline_color.b=1.0f;
	
		// chat
		
	iface.chat.x=637;
	iface.chat.y=479;
	iface.chat.last_add_life_sec=15;
	iface.chat.next_life_sec=5;

		// fades

	iface.fade.title_msec=1000;
	iface.fade.map_msec=300;
	
		// intro

	default_settings_interface_button(&iface.intro.button_game_new,0,0,TRUE);
	default_settings_interface_button(&iface.intro.button_game_load,0,32,TRUE);
	default_settings_interface_button(&iface.intro.button_game_setup,0,64,TRUE);
	default_settings_interface_button(&iface.intro.button_game_new_easy,128,0,TRUE);
	default_settings_interface_button(&iface.intro.button_game_new_medium,128,32,TRUE);
	default_settings_interface_button(&iface.intro.button_game_new_hard,128,64,TRUE);
	default_settings_interface_button(&iface.intro.button_game_new_cancel,128,96,TRUE);

	default_settings_interface_button(&iface.intro.button_multiplayer_host,0,96,TRUE);
	default_settings_interface_button(&iface.intro.button_multiplayer_join,0,128,TRUE);
	default_settings_interface_button(&iface.intro.button_multiplayer_setup,0,160,TRUE);

	default_settings_interface_button(&iface.intro.button_credit,0,192,TRUE);
	default_settings_interface_button(&iface.intro.button_quit,0,224,TRUE);
	
	for (n=0;n!=max_simple_save_spot;n++) {
		default_settings_interface_button(&iface.intro.simple_save[n].button_start,0,(n*32),FALSE);
		default_settings_interface_button(&iface.intro.simple_save[n].button_erase,40,(n*32),FALSE);
		iface.intro.simple_save[n].desc.x=80;
		iface.intro.simple_save[n].desc.y=(n*32);
		iface.intro.simple_save[n].desc.text_size=20;
	}

	iface.intro.model.nmodel=0;

		// player models

	iface.character.ncharacter=0;

		// multiplayer

	iface.net_game.ngame=1;
	strcpy(iface.net_game.games[0].name,"Deathmatch");

	iface.net_option.noption=0;
		
	iface.net_bot.on=TRUE;

	for (n=0;n!=max_net_bot;n++) {
		iface.net_bot.bots[n].name[0]=0x0;
	}
	
	iface.net_news.host[0]=0x0;
	iface.net_news.port=80;
	iface.net_news.url[0]=0x0;
}

/* =======================================================

      Finds
      
======================================================= */

iface_bitmap_type* iface_bitmaps_find(char *name)
{
	int					n;
	iface_bitmap_type	*bitmap;
	
	bitmap=iface.bitmap_list.bitmaps;
	
	for (n=0;n!=iface.bitmap_list.nbitmap;n++) {
		if (strcasecmp(bitmap->name,name)==0) return(bitmap);
		bitmap++;
	}
	
	return(NULL);
}

iface_text_type* iface_texts_find(char *name)
{
	int					n;
	iface_text_type		*text;
	
	text=iface.text_list.texts;
	
	for (n=0;n!=iface.text_list.ntext;n++) {
		if (strcasecmp(text->name,name)==0) return(text);
		text++;
	}
	
	return(NULL);
}

iface_bar_type* iface_bars_find(char *name)
{
	int					n;
	iface_bar_type		*bar;
	
	bar=iface.bar_list.bars;
	
	for (n=0;n!=iface.bar_list.nbar;n++) {
		if (strcasecmp(bar->name,name)==0) return(bar);
		bar++;
	}
	
	return(NULL);
}

int iface_menu_find_idx(char *name)
{
	int				n;
	iface_menu_type	*menu;
	
	menu=iface.menu_list.menus;
	
	for (n=0;n!=iface.menu_list.nmenu;n++) {
		if (strcasecmp(menu->name,name)==0) return(n);
		menu++;
	}
	
	return(-1);
}

int iface_chooser_find_idx(char *name)
{
	int					n;
	iface_chooser_type	*chooser;
	
	chooser=iface.chooser_list.choosers;
	
	for (n=0;n!=iface.chooser_list.nchooser;n++) {
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

