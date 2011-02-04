/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Interface XML

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
	#include "dim3engine.h"
#endif

#include "xmls.h"
#include "interfaces.h"

extern hud_type					hud;
extern setup_type				setup;
extern network_setup_type		net_setup;

char							just_mode_str[][32]={"left","center","right"},
								text_special_str[][32]=text_special_list_def;
d3col							default_tint_cols[8]={{1.0f,1.0f,1.0f},{1.0f,1.0f,0.0f},{1.0f,0.6f,0.0f},{1.0f,0.0f,0.0f},{0.0f,1.0f,0.0f},{0.0f,0.0f,1.0f},{1.0f,0.0f,1.0f},{0.6f,0.4f,0.0f}};


extern int menu_add(char *name);
extern void menu_add_item(int menu_idx,int item_id,char *data,char *sub_menu,bool multiplayer_disable,bool quit);
extern int chooser_find(char *name);
extern int chooser_add(char *name);
extern void chooser_copy_template(int idx,int template_idx);
extern void chooser_add_text(int chooser_idx,int template_idx,int id,char *str,int x,int y,int size,int just,bool clickable,char *goto_name);
extern void chooser_add_item(int chooser_idx,int template_idx,int id,char *file,int x,int y,int wid,int high,bool clickable,char *goto_name);
extern void chooser_add_model(int chooser_idx,int template_idx,int id,char *model_name,char *animate_name,int x,int y,d3ang *rot,float resize,bool clickable,char *goto_name);
extern void chooser_add_button(int chooser_idx,int template_idx,int id,char *name,int x,int y,int wid,int high,char *goto_name);

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

	hud.scale_x=640;
	hud.scale_y=480;

		// items
		
	hud.count.bitmap=0;
	hud.count.text=0;
	hud.count.bar=0;
	hud.count.menu=0;
	hud.count.chooser=0;

		// radar

	hud.radar.on=FALSE;
	
		// project setup
		
	hud.skill=FALSE;
	
		// sounds and music
		
	hud.click_sound[0]=0x0;
	hud.intro_music[0]=0x0;

		// debug mode

	hud.debug=FALSE;
	
		// colors
		
	hud.color.dialog_background.r=hud.color.dialog_background.g=hud.color.dialog_background.b=0.0f;
	hud.color.dialog_base.r=hud.color.dialog_base.g=hud.color.dialog_base.b=0.6f;
	hud.color.dialog_dimmed.r=hud.color.dialog_dimmed.g=hud.color.dialog_dimmed.b=0.4f;
	hud.color.dialog_outline.r=hud.color.dialog_outline.g=hud.color.dialog_outline.b=0.8f;
	
	hud.color.control_label.r=hud.color.control_label.g=hud.color.control_label.b=0.8f;
	hud.color.control_text.r=hud.color.control_text.g=hud.color.control_text.b=0.0f;
	hud.color.control_fill.r=hud.color.control_fill.g=hud.color.control_fill.b=0.6f;
	hud.color.control_header.r=hud.color.control_header.b=1.0f;
	hud.color.control_header.g=0.3f;
	hud.color.control_outline.r=hud.color.control_outline.g=hud.color.control_outline.b=0.0f;
	
	hud.color.control_mouse_over.b=1.0f;
	hud.color.control_mouse_over.r=hud.color.control_mouse_over.g=0.3f;
	hud.color.control_hilite.r=hud.color.control_hilite.b=1.0f;
	hud.color.control_hilite.g=0.3f;
	hud.color.control_disabled.r=hud.color.control_disabled.g=hud.color.control_disabled.b=0.3f;

	hud.color.button_fill.r=hud.color.button_fill.g=hud.color.button_fill.b=0.6f;
	hud.color.button_text.r=hud.color.button_text.g=hud.color.button_text.b=0.0f;
	hud.color.button_outline.r=hud.color.button_outline.g=hud.color.button_outline.b=0.0f;
	
	hud.color.default_tint.r=hud.color.default_tint.g=hud.color.default_tint.b=1.0f;

	for (n=0;n!=8;n++) {
		memmove(&hud.color.tints[n],&default_tint_cols[n],sizeof(d3col));
	}
	
		// fonts

	strcpy(hud.font.interface_name,"Arial");
	strcpy(hud.font.alt_interface_name,"Courier");

	strcpy(hud.font.hud_name,"Arial");
	strcpy(hud.font.alt_hud_name,"Courier");
	
		// progress
		
	hud.progress.lx=0;
	hud.progress.rx=640;
	hud.progress.ty=450;
	hud.progress.by=470;
	hud.progress.outline=TRUE;
	hud.progress.base_color_start.r=0.4f;
	hud.progress.base_color_start.g=hud.progress.base_color_start.b=0.0f;
	hud.progress.base_color_end.r=1.0f;
	hud.progress.base_color_end.g=hud.progress.base_color_end.b=0.0f;
	hud.progress.hilite_color_start.g=0.4f;
	hud.progress.hilite_color_start.r=hud.progress.hilite_color_start.b=0.0f;
	hud.progress.hilite_color_end.g=1.0f;
	hud.progress.hilite_color_end.r=hud.progress.hilite_color_end.b=0.0f;
	hud.progress.text_size=20;
	hud.progress.text_color.r=hud.progress.text_color.g=hud.progress.text_color.b=0.0f;
	hud.progress.outline_color.r=hud.progress.outline_color.g=hud.progress.outline_color.b=1.0f;
	
		// chat
		
	hud.chat.x=637;
	hud.chat.y=479;
	hud.chat.last_add_life_sec=15;
	hud.chat.next_life_sec=5;

		// fades

	hud.fade.title_msec=1000;
	hud.fade.map_msec=300;
	
		// buttons
	
	default_settings_interface_button(&hud.intro.button_game_new,0,0,TRUE);
	default_settings_interface_button(&hud.intro.button_game_load,0,32,TRUE);
	default_settings_interface_button(&hud.intro.button_game_setup,0,64,TRUE);
	default_settings_interface_button(&hud.intro.button_game_new_easy,128,0,TRUE);
	default_settings_interface_button(&hud.intro.button_game_new_medium,128,32,TRUE);
	default_settings_interface_button(&hud.intro.button_game_new_hard,128,64,TRUE);
	default_settings_interface_button(&hud.intro.button_game_new_cancel,128,96,TRUE);

	default_settings_interface_button(&hud.intro.button_multiplayer_host,0,96,TRUE);
	default_settings_interface_button(&hud.intro.button_multiplayer_join,0,128,TRUE);
	default_settings_interface_button(&hud.intro.button_multiplayer_setup,0,160,TRUE);

	default_settings_interface_button(&hud.intro.button_credit,0,192,TRUE);
	default_settings_interface_button(&hud.intro.button_quit,0,224,TRUE);
	
	for (n=0;n!=max_simple_save_spot;n++) {
		default_settings_interface_button(&hud.intro.simple_save[n].button_start,0,(n*32),FALSE);
		default_settings_interface_button(&hud.intro.simple_save[n].button_erase,40,(n*32),FALSE);
		hud.intro.simple_save[n].desc.x=80;
		hud.intro.simple_save[n].desc.y=(n*32);
		hud.intro.simple_save[n].desc.text_size=20;
	}

		// player models

	hud.character.ncharacter=0;

		// multiplayer

	hud.net_game.ngame=1;
	strcpy(hud.net_game.games[0].name,"Deathmatch");

	hud.net_option.noption=0;
		
	hud.net_bot.on=TRUE;

	for (n=0;n!=max_net_bot;n++) {
		hud.net_bot.bots[n].name[0]=0x0;
	}
	
	hud.net_news.host[0]=0x0;
	hud.net_news.port=80;
	hud.net_news.url[0]=0x0;
}

/* =======================================================

      Read Bitmap XML
      
======================================================= */

void read_settings_interface_bitmap(int bitmap_tag)
{
	int					tag;
	hud_bitmap_type		*bitmap;
    
		// read next bitmap
		
	bitmap=&hud.bitmaps[hud.count.bitmap];
	
		// read attributes
		
	xml_get_attribute_text(bitmap_tag,"name",bitmap->name,name_str_len);
	
	tag=xml_findfirstchild("Image",bitmap_tag);
	if (tag==-1) return;
	
	xml_get_attribute_text(tag,"file",bitmap->filename,file_str_len);
	bitmap->animate.image_count=xml_get_attribute_int_default(tag,"count",1);
	bitmap->animate.image_per_row=(int)sqrt((float)bitmap->animate.image_count);
	bitmap->animate.msec=xml_get_attribute_int(tag,"time");
    bitmap->animate.loop=xml_get_attribute_boolean(tag,"loop");
    bitmap->animate.loop_back=xml_get_attribute_boolean(tag,"loop_back");
	bitmap->rot=xml_get_attribute_float_default(tag,"rot",0.0f);
	
	bitmap->show_tick=0;

	bitmap->x=bitmap->y=0;
	
	tag=xml_findfirstchild("Position",bitmap_tag);
	if (tag!=-1) {
		bitmap->x=xml_get_attribute_int(tag,"x");
		bitmap->y=xml_get_attribute_int(tag,"y");
	}
	
	bitmap->x_size=bitmap->y_size=-1;
	
	tag=xml_findfirstchild("Size",bitmap_tag);
	if (tag!=-1) {
		bitmap->x_size=xml_get_attribute_int(tag,"width");
		bitmap->y_size=xml_get_attribute_int(tag,"height");
	}
	
	bitmap->alpha=1.0f;
	bitmap->flip_horz=bitmap->flip_vert=FALSE;
	bitmap->team_tint=FALSE;
	bitmap->show=TRUE;
	
	tag=xml_findfirstchild("Settings",bitmap_tag);
	if (tag!=-1) {
		bitmap->alpha=xml_get_attribute_float_default(tag,"alpha",1.0f);
		bitmap->flip_horz=xml_get_attribute_boolean(tag,"flip_horizontal");
		bitmap->flip_vert=xml_get_attribute_boolean(tag,"flip_vertical");
		bitmap->team_tint=xml_get_attribute_boolean(tag,"team_tint");
		bitmap->show=!xml_get_attribute_boolean(tag,"hide");
		bitmap->flash=xml_get_attribute_boolean(tag,"flash");
	}
	
	bitmap->repeat.on=FALSE;
	bitmap->repeat.count=0;
	bitmap->repeat.x_add=bitmap->repeat.y_add=0;
	bitmap->repeat.col=bitmap->repeat.row=0;
	
	tag=xml_findfirstchild("Repeat",bitmap_tag);
	if (tag!=-1) {
		bitmap->repeat.on=xml_get_attribute_boolean(tag,"on");
		bitmap->repeat.count=xml_get_attribute_int(tag,"count");
		bitmap->repeat.x_add=xml_get_attribute_int(tag,"x");
		bitmap->repeat.y_add=xml_get_attribute_int(tag,"y");
		bitmap->repeat.col=xml_get_attribute_int(tag,"col");
		bitmap->repeat.row=xml_get_attribute_int(tag,"row");
	}
	
	bitmap->fade.on=FALSE;
	bitmap->fade.fade_in_tick=bitmap->fade.life_tick=bitmap->fade.fade_out_tick=100;
	
	tag=xml_findfirstchild("Fade",bitmap_tag);
	if (tag!=-1) {
		bitmap->fade.on=xml_get_attribute_boolean(tag,"on");
		bitmap->fade.fade_in_tick=xml_get_attribute_int(tag,"fade_in_msec");
		bitmap->fade.life_tick=xml_get_attribute_int(tag,"life_msec");
		bitmap->fade.fade_out_tick=xml_get_attribute_int(tag,"fade_out_msec");
	}
	
		// move on to next bitmap
		
	hud.count.bitmap++;
}

/* =======================================================

      Read Text XML
      
======================================================= */

void read_settings_interface_text(int text_tag)
{
	int					tag;
	char				data[max_hud_text_str_sz];
	hud_text_type		*text;
	
		// read next text
		
	text=&hud.texts[hud.count.text];
	
		// read attributes

	xml_get_attribute_text(text_tag,"name",text->name,name_str_len);
	
	text->x=text->y=0;
	
	tag=xml_findfirstchild("Position",text_tag);
	if (tag!=-1) {
		text->x=xml_get_attribute_int(tag,"x");
		text->y=xml_get_attribute_int(tag,"y");
	}
	
	text->size=hud.font.text_size_small;
	text->color.r=text->color.g=text->color.b=1;
	text->show=TRUE;
	text->special=text_special_none;
	text->just=tx_left;
	text->alpha=1.0f;

	data[0]=0x0;
	
	tag=xml_findfirstchild("Settings",text_tag);
	if (tag!=-1) {
		text->alpha=xml_get_attribute_float_default(tag,"alpha",1.0f);
		xml_get_attribute_text(tag,"data",data,max_hud_text_str_sz);
		text->size=xml_get_attribute_int_default(tag,"size",hud.font.text_size_small);
		xml_get_attribute_color(tag,"color",&text->color);
		text->just=xml_get_attribute_list(tag,"just",(char*)just_mode_str);
		text->show=!xml_get_attribute_boolean(tag,"hide");
		text->special=xml_get_attribute_list(tag,"special",(char*)text_special_str);
	}

	text->fade.on=FALSE;
	text->fade.fade_in_tick=text->fade.life_tick=text->fade.fade_out_tick=100;
	
	tag=xml_findfirstchild("Fade",text_tag);
	if (tag!=-1) {
		text->fade.on=xml_get_attribute_boolean(tag,"on");
		text->fade.fade_in_tick=xml_get_attribute_int(tag,"fade_in_msec");
		text->fade.life_tick=xml_get_attribute_int(tag,"life_msec");
		text->fade.fade_out_tick=xml_get_attribute_int(tag,"fade_out_msec");
	}

	hud_text_set(text,data);
	
		// move on to next text
		
	hud.count.text++;
}

/* =======================================================

      Read Bar XML
      
======================================================= */

void read_settings_interface_bar(int bar_tag)
{
	int					tag;
	hud_bar_type		*bar;
    
		// read next bar
		
	bar=&hud.bars[hud.count.bar];
	
		// read attributes
		
	xml_get_attribute_text(bar_tag,"name",bar->name,name_str_len);
		
	bar->x=bar->y=0;
	
	tag=xml_findfirstchild("Position",bar_tag);
	if (tag!=-1) {
		bar->x=xml_get_attribute_int(tag,"x");
		bar->y=xml_get_attribute_int(tag,"y");
	}
	
	bar->x_size=bar->y_size=-1;
	
	tag=xml_findfirstchild("Size",bar_tag);
	if (tag!=-1) {
		bar->x_size=xml_get_attribute_int(tag,"width");
		bar->y_size=xml_get_attribute_int(tag,"height");
	}
	
	bar->outline=FALSE;
	bar->outline_alpha=1;
	bar->outline_color.r=bar->outline_color.g=bar->outline_color.b=0;
	
	tag=xml_findfirstchild("Outline",bar_tag);
	if (tag!=-1) {
		bar->outline=xml_get_attribute_boolean(tag,"on");
		bar->outline_alpha=xml_get_attribute_float_default(tag,"alpha",1.0f);
		xml_get_attribute_color(tag,"color",&bar->outline_color);
	}
	
	bar->fill_alpha=1;
	bar->fill_start_color.r=bar->fill_start_color.g=bar->fill_start_color.b=0;
	bar->fill_end_color.r=bar->fill_end_color.g=bar->fill_end_color.b=0;
	
	tag=xml_findfirstchild("Fill",bar_tag);
	if (tag!=-1) {
		bar->fill_alpha=xml_get_attribute_float_default(tag,"alpha",1.0f);
		xml_get_attribute_color(tag,"start_color",&bar->fill_start_color);
		xml_get_attribute_color(tag,"end_color",&bar->fill_end_color);
	}
	
	bar->vert=FALSE;
	bar->show=TRUE;
	
	tag=xml_findfirstchild("Settings",bar_tag);
	if (tag!=-1) {
		bar->vert=xml_get_attribute_boolean(tag,"vert");
		bar->show=!xml_get_attribute_boolean(tag,"hide");
	}
	
		// value starts at 1
		
	bar->value=1;
	
		// move on to next bar
		
	hud.count.bar++;
}

/* =======================================================

      Read Radar XML
      
======================================================= */

void read_settings_interface_radar(int radar_tag)
{
	int					tag,icons_head_tag,icon_tag;

		// defaults

	hud.radar.on=TRUE;
	hud.radar.rot=TRUE;
	hud.radar.team_tint=FALSE;
	hud.radar.no_motion_fade=0;
	
	hud.radar.x=hud.radar.y=0;
	hud.radar.display_radius=32;
	hud.radar.view_radius=map_enlarge*100;
	hud.radar.nicon=0;
	hud.radar.background_bitmap_name[0]=0x0;

		// read settings

	tag=xml_findfirstchild("Setting",radar_tag);
	if (tag!=-1) {
		hud.radar.on=xml_get_attribute_boolean(tag,"on");		// only use on switch if it exists
		hud.radar.rot=!xml_get_attribute_boolean(tag,"no_rot");
		hud.radar.team_tint=xml_get_attribute_boolean(tag,"team_tint");
		hud.radar.no_motion_fade=xml_get_attribute_int(tag,"no_motion_fade");
	}

	tag=xml_findfirstchild("Position",radar_tag);
	if (tag!=-1) {
		hud.radar.x=xml_get_attribute_int(tag,"x");
		hud.radar.y=xml_get_attribute_int(tag,"y");
		hud.radar.display_radius=xml_get_attribute_int(tag,"radius");
	}

	tag=xml_findfirstchild("View",radar_tag);
	if (tag!=-1) {
		hud.radar.view_radius=xml_get_attribute_int(tag,"radius");
	}

	tag=xml_findfirstchild("Background",radar_tag);
	if (tag!=-1) {
		xml_get_attribute_text(tag,"file",hud.radar.background_bitmap_name,name_str_len);
	}

		// get the icons

	icons_head_tag=xml_findfirstchild("Icons",radar_tag);
	if (icons_head_tag==-1) return;
	
	icon_tag=xml_findfirstchild("Icon",icons_head_tag);
	
	while (icon_tag!=-1) {
		xml_get_attribute_text(icon_tag,"name",hud.radar.icons[hud.radar.nicon].name,name_str_len);
		xml_get_attribute_text(icon_tag,"file",hud.radar.icons[hud.radar.nicon].bitmap_name,name_str_len);
		hud.radar.icons[hud.radar.nicon].size=xml_get_attribute_int(icon_tag,"size");
		hud.radar.icons[hud.radar.nicon].rot=xml_get_attribute_boolean(icon_tag,"rot");
		hud.radar.nicon++;

		icon_tag=xml_findnextchild(icon_tag);
	}
}

/* =======================================================

      Read Menu XML
      
======================================================= */

void read_settings_interface_menu(int menu_tag)
{
	int					idx,items_head_tag,item_tag,item_id;
	char				name[name_str_len],sub_menu[name_str_len],
						data[max_menu_item_data_sz];
	bool				multiplayer_disable,quit;

	xml_get_attribute_text(menu_tag,"name",name,name_str_len);
	
	idx=menu_add(name);
	if (idx==-1) return;
	
	items_head_tag=xml_findfirstchild("Items",menu_tag);
	if (items_head_tag==-1) return;
	
	item_tag=xml_findfirstchild("Item",items_head_tag);
	
	while (item_tag!=-1) {
		item_id=xml_get_attribute_int(item_tag,"id");
		xml_get_attribute_text(item_tag,"data",data,max_menu_item_data_sz);
		xml_get_attribute_text(item_tag,"sub_menu",sub_menu,name_str_len);
		multiplayer_disable=xml_get_attribute_boolean(item_tag,"multiplayer_disable");
		quit=xml_get_attribute_boolean(item_tag,"quit");
		
		menu_add_item(idx,item_id,data,sub_menu,multiplayer_disable,quit);
		
		item_tag=xml_findnextchild(item_tag);
	}
}

/* =======================================================

      Read Chooser XML
      
======================================================= */

void read_settings_interface_chooser(int chooser_tag)
{
	int				idx,template_idx,just,tag,texts_head_tag,text_tag,
					items_head_tag,item_tag,models_head_tag,model_item_tag,
					buttons_head_tag,button_tag,
					x,y,wid,high,id,text_size;
	float			resize;
	char			name[name_str_len],file[file_str_len],
					model_name[name_str_len],animate_name[name_str_len],
					template_name[name_str_len],goto_name[name_str_len],
					btn_name[max_chooser_button_text_sz],
					data[max_chooser_text_data_sz];
	bool			clickable;
	d3ang			rot;
	chooser_type	*chooser;

		// name and template

	xml_get_attribute_text(chooser_tag,"name",name,name_str_len);
	xml_get_attribute_text(chooser_tag,"template",template_name,name_str_len);
	
	idx=chooser_add(name);
	if (idx==-1) return;
	
	chooser=&hud.choosers[idx];

		// run any templates
		// templates MUST be in order

	template_idx=-1;

	if (template_name[0]!=0x0) {
		template_idx=chooser_find(template_name);

		if (template_idx!=-1) chooser_copy_template(idx,template_idx);
	}
	
		// frames and keys
		
	tag=xml_findfirstchild("Frame",chooser_tag);
	if (tag!=-1) {
		chooser->frame.on=xml_get_attribute_boolean(tag,"on");
		xml_get_attribute_text(tag,"title",chooser->frame.title,max_chooser_frame_text_sz);
		chooser->frame.x=xml_get_attribute_int(tag,"x");
		chooser->frame.y=xml_get_attribute_int(tag,"y");
		chooser->frame.wid=xml_get_attribute_int(tag,"width");
		chooser->frame.high=xml_get_attribute_int(tag,"height");
		xml_get_attribute_color(tag,"background_color",&chooser->frame.background_col);
	}
	
	tag=xml_findfirstchild("Key",chooser_tag);
	if (tag!=-1) {
		chooser->key.ok_id=xml_get_attribute_int(tag,"ok_id");
		chooser->key.cancel_id=xml_get_attribute_int(tag,"cancel_id");
	}
	
		// items
	
	items_head_tag=xml_findfirstchild("Items",chooser_tag);
	if (items_head_tag!=-1) {
	
		item_tag=xml_findfirstchild("Item",items_head_tag);
		
		while (item_tag!=-1) {
			id=xml_get_attribute_int(item_tag,"id");
			xml_get_attribute_text(item_tag,"file",file,file_str_len);
			x=xml_get_attribute_int(item_tag,"x");
			y=xml_get_attribute_int(item_tag,"y");
			wid=xml_get_attribute_int_default(item_tag,"width",-1);
			high=xml_get_attribute_int_default(item_tag,"height",-1);
			clickable=xml_get_attribute_boolean(item_tag,"clickable");
			xml_get_attribute_text(item_tag,"goto",goto_name,name_str_len);
			
			chooser_add_item(idx,template_idx,id,file,x,y,wid,high,clickable,goto_name);
			
			item_tag=xml_findnextchild(item_tag);
		}
	}
	
		// models
	
	models_head_tag=xml_findfirstchild("Models",chooser_tag);
	if (models_head_tag!=-1) {
	
		model_item_tag=xml_findfirstchild("Model",models_head_tag);
		
		while (model_item_tag!=-1) {
			id=xml_get_attribute_int(model_item_tag,"id");
			xml_get_attribute_text(model_item_tag,"model",model_name,name_str_len);
			xml_get_attribute_text(model_item_tag,"animate",animate_name,name_str_len);
			x=xml_get_attribute_int(model_item_tag,"x");
			y=xml_get_attribute_int(model_item_tag,"y");
			xml_get_attribute_3_coord_float(model_item_tag,"rot",&rot.x,&rot.y,&rot.z);
			resize=xml_get_attribute_float_default(model_item_tag,"resize",1.0f);

			clickable=xml_get_attribute_boolean(model_item_tag,"clickable");
			xml_get_attribute_text(model_item_tag,"goto",goto_name,name_str_len);
			
			chooser_add_model(idx,template_idx,id,model_name,animate_name,x,y,&rot,resize,clickable,goto_name);
			
			model_item_tag=xml_findnextchild(model_item_tag);
		}
	}

		// text

	texts_head_tag=xml_findfirstchild("Texts",chooser_tag);
	if (texts_head_tag!=-1) {
	
		text_tag=xml_findfirstchild("Text",texts_head_tag);
		
		while (text_tag!=-1) {

			data[0]=0x0;

			id=xml_get_attribute_int(text_tag,"id");
			xml_get_attribute_text(text_tag,"data",data,max_chooser_text_data_sz);
			text_size=xml_get_attribute_int_default(text_tag,"size",hud.font.text_size_small);
			just=xml_get_attribute_list(text_tag,"just",(char*)just_mode_str);
			x=xml_get_attribute_int(text_tag,"x");
			y=xml_get_attribute_int(text_tag,"y");
			clickable=xml_get_attribute_boolean(text_tag,"clickable");
			xml_get_attribute_text(text_tag,"goto",goto_name,name_str_len);

			chooser_add_text(idx,template_idx,id,data,x,y,text_size,just,clickable,goto_name);

			text_tag=xml_findnextchild(text_tag);
		}
	}
	
		// buttons
	
	buttons_head_tag=xml_findfirstchild("Buttons",chooser_tag);
	if (buttons_head_tag!=-1) {
	
		button_tag=xml_findfirstchild("Button",buttons_head_tag);
		
		while (button_tag!=-1) {
			id=xml_get_attribute_int(button_tag,"id");
			xml_get_attribute_text(button_tag,"name",btn_name,max_chooser_button_text_sz);
			x=xml_get_attribute_int(button_tag,"x");
			y=xml_get_attribute_int(button_tag,"y");
			wid=xml_get_attribute_int_default(button_tag,"width",-1);
			high=xml_get_attribute_int_default(button_tag,"height",-1);
			xml_get_attribute_text(button_tag,"goto",goto_name,name_str_len);
			
			chooser_add_button(idx,template_idx,id,btn_name,x,y,wid,high,goto_name);
			
			button_tag=xml_findnextchild(button_tag);
		}
	}
}

/* =======================================================

      Read Button XML
      
======================================================= */

void read_settings_interface_button(int tag,hud_intro_button_type *btn,hud_intro_simple_save_desc_type *desc)
{
	if (tag==-1) return;
	
	btn->x=xml_get_attribute_int(tag,"x");
	btn->y=xml_get_attribute_int(tag,"y");
	btn->wid=xml_get_attribute_int_default(tag,"width",-1);
	btn->high=xml_get_attribute_int_default(tag,"height",-1);
	btn->on=xml_get_attribute_boolean(tag,"on");

	if (desc!=NULL) {
		desc->x=xml_get_attribute_int(tag,"desc_x");
		desc->y=xml_get_attribute_int(tag,"desc_y");
		desc->text_size=xml_get_attribute_int_default(tag,"desc_text_size",20);
	}
}

/* =======================================================

      Read Interface XML
      
======================================================= */

void read_settings_interface(void)
{
	int						n,cnt,interface_head_tag,scale_tag,
							bitmap_head_tag,bitmap_tag,text_head_tag,text_tag,bar_head_tag,bar_tag,
							radar_head_tag,menu_head_tag,menu_tag,chooser_head_tag,chooser_tag,
							color_tag,font_tag,progress_tag,chat_tag,fade_tag,button_tag,sound_tag,music_tag,
							proj_tag,debug_tag,games_head_tag,game_tag,options_head_tag,option_tag,
							character_head_tag,character_item_tag,bot_head_tag,bot_tag,news_tag;
	char					path[1024],name[256];
	hud_character_item_type	*hud_character;

	default_settings_interface();
	
		// read in interface from setting files
		
	file_paths_data(&setup.file_path_setup,path,"Settings","Interface","xml");
	if (!xml_open_file(path)) return;
	
		// decode the file
      
    interface_head_tag=xml_findrootchild("Interface");
    if (interface_head_tag==-1) {
		xml_close_file();
		return;
	}

		// scale

	scale_tag=xml_findfirstchild("Scale",interface_head_tag);
	if (scale_tag!=-1) {
		hud.scale_x=xml_get_attribute_int_default(scale_tag,"x",640);
		hud.scale_y=xml_get_attribute_int_default(scale_tag,"y",480);
	}

		// some initial text sizes

	hud.font.text_size_small=(int)(((float)hud.scale_x)*text_small_wid_factor);
	hud.font.text_size_medium=(int)(((float)hud.scale_x)*text_medium_wid_factor);
	hud.font.text_size_large=(int)(((float)hud.scale_x)*text_large_wid_factor);
	
		// bitmaps
	
    bitmap_head_tag=xml_findfirstchild("Bitmaps",interface_head_tag);
    if (bitmap_head_tag!=-1) {
	
		bitmap_tag=xml_findfirstchild("Bitmap",bitmap_head_tag);
		
		while (bitmap_tag!=-1) {
			read_settings_interface_bitmap(bitmap_tag);
			bitmap_tag=xml_findnextchild(bitmap_tag);
		}
	}
	
		// text
	
	text_head_tag=xml_findfirstchild("Texts",interface_head_tag);
    if (text_head_tag!=-1) {
	
		text_tag=xml_findfirstchild("Text",text_head_tag);
		
		while (text_tag!=-1) {
			read_settings_interface_text(text_tag);
			text_tag=xml_findnextchild(text_tag);
		}
	}
	
		// bars
	
    bar_head_tag=xml_findfirstchild("Bars",interface_head_tag);
    if (bar_head_tag!=-1) {
	
		bar_tag=xml_findfirstchild("Bar",bar_head_tag);
		
		while (bar_tag!=-1) {
			read_settings_interface_bar(bar_tag);
			bar_tag=xml_findnextchild(bar_tag);
		}
	}

		// radar

	radar_head_tag=xml_findfirstchild("Radar",interface_head_tag);
	if (radar_head_tag!=-1) read_settings_interface_radar(radar_head_tag);
	
		// menus
		
	menu_head_tag=xml_findfirstchild("Menus",interface_head_tag);
    if (menu_head_tag!=-1) {
	
		menu_tag=xml_findfirstchild("Menu",menu_head_tag);
		
		while (menu_tag!=-1) {
			read_settings_interface_menu(menu_tag);
			menu_tag=xml_findnextchild(menu_tag);
		}
	}
	
		// choosers
		
	chooser_head_tag=xml_findfirstchild("Choosers",interface_head_tag);
    if (chooser_head_tag!=-1) {
	
		chooser_tag=xml_findfirstchild("Chooser",chooser_head_tag);
		
		while (chooser_tag!=-1) {
			read_settings_interface_chooser(chooser_tag);
			chooser_tag=xml_findnextchild(chooser_tag);
		}
	}
	
		// colors
		
	color_tag=xml_findfirstchild("Color",interface_head_tag);
	if (color_tag!=-1) {
		xml_get_attribute_color(color_tag,"dialog_background",&hud.color.dialog_background);
		xml_get_attribute_color(color_tag,"dialog_base",&hud.color.dialog_base);
		xml_get_attribute_color(color_tag,"dialog_dimmed",&hud.color.dialog_dimmed);
		xml_get_attribute_color(color_tag,"dialog_outline",&hud.color.dialog_outline);
		xml_get_attribute_color(color_tag,"control_label",&hud.color.control_label);
		xml_get_attribute_color(color_tag,"control_text",&hud.color.control_text);
		xml_get_attribute_color(color_tag,"control_fill",&hud.color.control_fill);
		xml_get_attribute_color(color_tag,"control_header",&hud.color.control_header);
		xml_get_attribute_color(color_tag,"control_outline",&hud.color.control_outline);
		xml_get_attribute_color(color_tag,"control_mouse_over",&hud.color.control_mouse_over);
		xml_get_attribute_color(color_tag,"control_hilite",&hud.color.control_hilite);
		xml_get_attribute_color(color_tag,"control_disabled",&hud.color.control_disabled);
		xml_get_attribute_color(color_tag,"button_fill",&hud.color.button_fill);
		xml_get_attribute_color(color_tag,"button_text",&hud.color.button_text);
		xml_get_attribute_color(color_tag,"button_outline",&hud.color.button_outline);
		xml_get_attribute_color(color_tag,"default_tint",&hud.color.default_tint);
	}

		// first tint is always the default tint

	memmove(&hud.color.tints[0],&hud.color.default_tint,sizeof(d3col));

		// fonts

	font_tag=xml_findfirstchild("Font",interface_head_tag);
	if (font_tag!=-1) {
		xml_get_attribute_text(font_tag,"name",hud.font.interface_name,name_str_len);
		xml_get_attribute_text(font_tag,"alt_name",hud.font.alt_interface_name,name_str_len);

		xml_get_attribute_text(font_tag,"hud_name",hud.font.hud_name,name_str_len);
		xml_get_attribute_text(font_tag,"alt_hud_name",hud.font.alt_hud_name,name_str_len);
	}
	
		// progress

	progress_tag=xml_findfirstchild("Progress",interface_head_tag);
	if (progress_tag!=-1) {
		hud.progress.lx=xml_get_attribute_int(progress_tag,"left_x");
		hud.progress.rx=xml_get_attribute_int(progress_tag,"right_x");
		hud.progress.ty=xml_get_attribute_int(progress_tag,"top_y");
		hud.progress.by=xml_get_attribute_int(progress_tag,"bottom_y");
		hud.progress.text_size=xml_get_attribute_int(progress_tag,"text_size");
		hud.progress.outline=xml_get_attribute_boolean(progress_tag,"outline");
		xml_get_attribute_color(progress_tag,"base_color_start",&hud.progress.base_color_start);
		xml_get_attribute_color(progress_tag,"base_color_end",&hud.progress.base_color_end);
		xml_get_attribute_color(progress_tag,"hilite_color_start",&hud.progress.hilite_color_start);
		xml_get_attribute_color(progress_tag,"hilite_color_end",&hud.progress.hilite_color_end);
		xml_get_attribute_color(progress_tag,"text_color",&hud.progress.text_color);
		xml_get_attribute_color(progress_tag,"outline_color",&hud.progress.outline_color);
	}
	
		// chat

	chat_tag=xml_findfirstchild("Chat",interface_head_tag);
	if (chat_tag!=-1) {
		hud.chat.x=xml_get_attribute_int(chat_tag,"x");
		hud.chat.y=xml_get_attribute_int(chat_tag,"y");
		hud.chat.last_add_life_sec=xml_get_attribute_int(chat_tag,"last_add_life_sec");
		hud.chat.next_life_sec=xml_get_attribute_int(chat_tag,"next_life_sec");
	}
	
		// fade

	fade_tag=xml_findfirstchild("Fade",interface_head_tag);
	if (fade_tag!=-1) {
		hud.fade.title_msec=xml_get_attribute_int(fade_tag,"title_msec");
		hud.fade.map_msec=xml_get_attribute_int(fade_tag,"map_msec");
	}
	
		// buttons
		
	button_tag=xml_findfirstchild("Buttons",interface_head_tag);
	if (button_tag!=-1) {
		read_settings_interface_button(xml_findfirstchild("Game_New",button_tag),&hud.intro.button_game_new,NULL);
		read_settings_interface_button(xml_findfirstchild("Game_Load",button_tag),&hud.intro.button_game_load,NULL);
		read_settings_interface_button(xml_findfirstchild("Game_Setup",button_tag),&hud.intro.button_game_setup,NULL);
		read_settings_interface_button(xml_findfirstchild("Game_New_Easy",button_tag),&hud.intro.button_game_new_easy,NULL);
		read_settings_interface_button(xml_findfirstchild("Game_New_Medium",button_tag),&hud.intro.button_game_new_medium,NULL);
		read_settings_interface_button(xml_findfirstchild("Game_New_Hard",button_tag),&hud.intro.button_game_new_hard,NULL);
		read_settings_interface_button(xml_findfirstchild("Game_New_Cancel",button_tag),&hud.intro.button_game_new_cancel,NULL);
		read_settings_interface_button(xml_findfirstchild("Multiplayer_Host",button_tag),&hud.intro.button_multiplayer_host,NULL);
		read_settings_interface_button(xml_findfirstchild("Multiplayer_Join",button_tag),&hud.intro.button_multiplayer_join,NULL);
		read_settings_interface_button(xml_findfirstchild("Multiplayer_Setup",button_tag),&hud.intro.button_multiplayer_setup,NULL);
		read_settings_interface_button(xml_findfirstchild("Credit",button_tag),&hud.intro.button_credit,NULL);
		read_settings_interface_button(xml_findfirstchild("Quit",button_tag),&hud.intro.button_quit,NULL);
		
		for (n=0;n!=max_simple_save_spot;n++) {
			sprintf(name,"Simple_Start_%d",n);
			read_settings_interface_button(xml_findfirstchild(name,button_tag),&hud.intro.simple_save[n].button_start,&hud.intro.simple_save[n].desc);
			sprintf(name,"Simple_Erase_%d",n);
			read_settings_interface_button(xml_findfirstchild(name,button_tag),&hud.intro.simple_save[n].button_erase,NULL);
		}
	}
	
		// sound
		
	sound_tag=xml_findfirstchild("Sound",interface_head_tag);
	if (sound_tag!=-1) {
		xml_get_attribute_text(sound_tag,"click",hud.click_sound,name_str_len);
	}
	
		// music
		
	music_tag=xml_findfirstchild("Music",interface_head_tag);
	if (music_tag!=-1) {
		xml_get_attribute_text(music_tag,"intro",hud.intro_music,name_str_len);
	}

		// project setup
		
	proj_tag=xml_findfirstchild("Project",interface_head_tag);
	if (proj_tag!=-1) {
		xml_get_attribute_text(proj_tag,"name",hud.project_name,name_str_len);
		hud.skill=xml_get_attribute_boolean(proj_tag,"skill");
	}
	
		// debug setup

	debug_tag=xml_findfirstchild("Debug",interface_head_tag);
	if (debug_tag!=-1) {
		hud.debug=xml_get_attribute_boolean(debug_tag,"on");
	}

		// network games

	games_head_tag=xml_findfirstchild("Games",interface_head_tag);
	if (games_head_tag!=-1) {
		
		hud.net_game.ngame=0;
		
		game_tag=xml_findfirstchild("Game",games_head_tag);
		while (game_tag!=-1) {
		
			xml_get_attribute_text(game_tag,"type",hud.net_game.games[hud.net_game.ngame].name,name_str_len);
			hud.net_game.games[hud.net_game.ngame].use_teams=xml_get_attribute_boolean(game_tag,"use_teams");
			hud.net_game.games[hud.net_game.ngame].monsters=xml_get_attribute_boolean(game_tag,"monsters");
			
			hud.net_game.ngame++;
			if (hud.net_game.ngame==max_net_game) break;

			game_tag=xml_findnextchild(game_tag);
		}
	}

		// network options

	options_head_tag=xml_findfirstchild("Options",interface_head_tag);
	if (options_head_tag!=-1) {
		
		hud.net_option.noption=0;
		
		option_tag=xml_findfirstchild("Option",options_head_tag);
		while (option_tag!=-1) {
		
			xml_get_attribute_text(option_tag,"name",hud.net_option.options[hud.net_option.noption].name,name_str_len);
			xml_get_attribute_text(option_tag,"description",hud.net_option.options[hud.net_option.noption].descript,64);
			
			hud.net_option.noption++;
			if (hud.net_option.noption==max_net_option) break;

			option_tag=xml_findnextchild(option_tag);
		}
	}

		// characters

    character_head_tag=xml_findfirstchild("Characters",interface_head_tag);
    if (character_head_tag!=-1) {

		hud.character.ncharacter=0;
		character_item_tag=xml_findfirstchild("Character",character_head_tag);
		
		while (character_item_tag!=-1) {
			hud_character=&hud.character.characters[hud.character.ncharacter];
			
			xml_get_attribute_text(character_item_tag,"name",hud_character->name,name_str_len);
			xml_get_attribute_text(character_item_tag,"model",hud_character->model_name,name_str_len);
			xml_get_attribute_text(character_item_tag,"parameter",hud_character->param,name_str_len);
			hud_character->interface_resize=xml_get_attribute_float_default(character_item_tag,"interface_resize",1.0f);
			xml_get_attribute_3_coord_int(character_item_tag,"interface_offset",&hud_character->interface_offset.x,&hud_character->interface_offset.y,&hud_character->interface_offset.z);
			
			hud.character.ncharacter++;
			character_item_tag=xml_findnextchild(character_item_tag);
		}
	}
	
		// bot names
	
    bot_head_tag=xml_findfirstchild("Bots",interface_head_tag);
    if (bot_head_tag!=-1) {

		hud.net_bot.on=!xml_get_attribute_boolean(bot_head_tag,"hide");
	
		cnt=0;
		bot_tag=xml_findfirstchild("Bot",bot_head_tag);
		
		while (bot_tag!=-1) {
			xml_get_attribute_text(bot_tag,"name",hud.net_bot.bots[cnt].name,name_str_len);
			cnt++;
			bot_tag=xml_findnextchild(bot_tag);
		}
	}
	
		// news
		
    news_tag=xml_findfirstchild("News",interface_head_tag);
    if (news_tag!=-1) {
		xml_get_attribute_text(news_tag,"host",hud.net_news.host,64);
		hud.net_news.port=xml_get_attribute_int_default(news_tag,"port",80);
		xml_get_attribute_text(news_tag,"url",hud.net_news.url,256);
	}

	xml_close_file();
}

/* =======================================================

      Read Interface XML
	  For Project Name, so we can put it in the
	  File Path so project name can override the application's
	  name (therefore we can have multiple data files
	  per same application)
      
======================================================= */

void read_settings_interface_project_name(void)
{
	int						interface_head_tag,proj_tag;
	char					path[1024];
	
		// default project name is blank
		// (will use application name)
		
	hud.project_name[0]=0x0;

		// read in interface from setting files
		
	file_paths_data(&setup.file_path_setup,path,"Settings","Interface","xml");
	if (!xml_open_file(path)) return;
	
		// decode the file
      
    interface_head_tag=xml_findrootchild("Interface");
    if (interface_head_tag==-1) {
		xml_close_file();
		return;
	}

		// project setup
		
	proj_tag=xml_findfirstchild("Project",interface_head_tag);
	if (proj_tag!=-1) {
		xml_get_attribute_text(proj_tag,"name",setup.file_path_setup.proj_name,256);
	}
	

	xml_close_file();
}

