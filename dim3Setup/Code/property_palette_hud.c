/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette HUD

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
	#include "dim3setup.h"
#endif

#include "glue.h"
#include "ui_common.h"
#include "interface.h"

#define kHUDPropertyChatX						0
#define kHUDPropertyChatY						1
#define kHUDPropertyChatLastAddLifeSeconds		2
#define kHUDPropertyChatNextLifeSeconds			3

#define kHUDPropertyBitmapAdd					1000
#define kHUDPropertyBitmapName					2000
#define kHUDPropertyBitmapDelete				3000

#define kHUDPropertyTextAdd						4000
#define kHUDPropertyTextName					5000
#define kHUDPropertyTextDelete					6000

#define kHUDPropertyBarAdd						7000
#define kHUDPropertyBarName						8000
#define kHUDPropertyBarDelete					9000

extern iface_type				iface;
extern setup_state_type			state;
extern list_palette_type		property_palette;

/* =======================================================

      Property Palette Fill HUD
      
======================================================= */

void property_palette_fill_hud(void)
{
	int				n;

		// bitmaps

	list_palette_add_header_button(&property_palette,kHUDPropertyBitmapAdd,"Bitmaps",list_button_plus);

	list_palette_sort_mark_start(&property_palette);
	
	for (n=0;n!=iface.bitmap_list.nbitmap;n++) {
		list_palette_add_string_selectable_button(&property_palette,(kHUDPropertyBitmapName+n),list_button_minus,(kHUDPropertyBitmapDelete+n),iface.bitmap_list.bitmaps[n].name,NULL,(state.cur_hud_bitmap_idx==n),FALSE);
	}

	list_palette_sort(&property_palette);

		// texts

	list_palette_add_header_button(&property_palette,kHUDPropertyTextAdd,"Texts",list_button_plus);

	list_palette_sort_mark_start(&property_palette);
	
	for (n=0;n!=iface.text_list.ntext;n++) {
		list_palette_add_string_selectable_button(&property_palette,(kHUDPropertyTextName+n),list_button_minus,(kHUDPropertyTextDelete+n),iface.text_list.texts[n].name,NULL,(state.cur_hud_text_idx==n),FALSE);
	}

	list_palette_sort(&property_palette);

		// bars

	list_palette_add_header_button(&property_palette,kHUDPropertyBarAdd,"Bars",list_button_plus);

	list_palette_sort_mark_start(&property_palette);
	
	for (n=0;n!=iface.bar_list.nbar;n++) {
		list_palette_add_string_selectable_button(&property_palette,(kHUDPropertyBarName+n),list_button_minus,(kHUDPropertyBarDelete+n),iface.bar_list.bars[n].name,NULL,(state.cur_hud_bar_idx==n),FALSE);
	}

	list_palette_sort(&property_palette);
	
		// chat
		
	list_palette_add_header(&property_palette,0,"Chat");
	list_palette_add_string_int(&property_palette,kHUDPropertyChatX,"X",iface.chat.x,FALSE);
	list_palette_add_string_int(&property_palette,kHUDPropertyChatY,"Y",iface.chat.y,FALSE);
	list_palette_add_string_int(&property_palette,kHUDPropertyChatLastAddLifeSeconds,"Last Add Life Seconds",iface.chat.last_add_life_sec,FALSE);
	list_palette_add_string_int(&property_palette,kHUDPropertyChatNextLifeSeconds,"Next Life Seconds",iface.chat.next_life_sec,FALSE);
}

/* =======================================================

      Property Palette Click HUD
      
======================================================= */

void property_palette_click_hud(int id)
{
	int					idx,sz;
	char				name[name_str_len];

		// select bitmap

	if ((id>=kHUDPropertyBitmapName) && (id<(kHUDPropertyBitmapName+max_iface_bitmap))) {
		state.cur_hud_bitmap_idx=id-kHUDPropertyBitmapName;
		state.cur_hud_text_idx=-1;
		state.cur_hud_bar_idx=-1;
		main_wind_draw();
		return;
	}

		// delete bitmap

	if ((id>=kHUDPropertyBitmapDelete) && (id<(kHUDPropertyBitmapDelete+max_iface_bitmap))) {
		state.cur_hud_bitmap_idx=-1;
		state.cur_hud_text_idx=-1;
		state.cur_hud_bar_idx=-1;

		idx=id-kHUDPropertyBitmapDelete;

		sz=(iface.bitmap_list.nbitmap-idx)-1;
		if (sz>0) memmove(&iface.bitmap_list.bitmaps[idx],&iface.bitmap_list.bitmaps[idx+1],(sz*sizeof(iface_bitmap_type)));

		iface.bitmap_list.nbitmap--;

		main_wind_draw();
		return;
	}

		// add bitmap

	if (id==kHUDPropertyBitmapAdd) {
		state.cur_hud_text_idx=-1;
		state.cur_hud_bar_idx=-1;

		if (iface.bitmap_list.nbitmap>=max_iface_bitmap) {
			os_dialog_alert("Add HUD Bitmap","Reached the maximum number of bitmaps");
			return;
		}
		
		name[0]=0x0;
		dialog_property_string_run(list_string_value_string,(void*)name,name_str_len,0,0);
		if (name[0]==0x0) return;

		idx=iface.bitmap_list.nbitmap;
		iface.bitmap_list.nbitmap++;

		iface.bitmap_list.bitmaps[idx].x=0;
		iface.bitmap_list.bitmaps[idx].y=0;
		iface.bitmap_list.bitmaps[idx].x_size=-1;
		iface.bitmap_list.bitmaps[idx].y_size=-1;
		iface.bitmap_list.bitmaps[idx].alpha=0.0f;
		iface.bitmap_list.bitmaps[idx].rot=0.0f;
		strcpy(iface.bitmap_list.bitmaps[idx].name,name);
		iface.bitmap_list.bitmaps[idx].filename[0]=0x0;
		iface.bitmap_list.bitmaps[idx].show=TRUE;
		iface.bitmap_list.bitmaps[idx].flash=FALSE;
		iface.bitmap_list.bitmaps[idx].flip_horz=FALSE;
		iface.bitmap_list.bitmaps[idx].flip_vert=FALSE;
		iface.bitmap_list.bitmaps[idx].team_tint=FALSE;

		iface.bitmap_list.bitmaps[idx].repeat.on=FALSE;
		iface.bitmap_list.bitmaps[idx].repeat.count=0;
		iface.bitmap_list.bitmaps[idx].repeat.x_add=0;
		iface.bitmap_list.bitmaps[idx].repeat.y_add=0;
		iface.bitmap_list.bitmaps[idx].repeat.col=0;
		iface.bitmap_list.bitmaps[idx].repeat.row=0;
		
		iface.bitmap_list.bitmaps[idx].fade.on=FALSE;
		iface.bitmap_list.bitmaps[idx].fade.fade_in_tick=0;
		iface.bitmap_list.bitmaps[idx].fade.life_tick=0;
		iface.bitmap_list.bitmaps[idx].fade.fade_out_tick=0;

		iface.bitmap_list.bitmaps[idx].animate.image_count=1;
		iface.bitmap_list.bitmaps[idx].animate.msec=0;
		iface.bitmap_list.bitmaps[idx].animate.loop=FALSE;
		iface.bitmap_list.bitmaps[idx].animate.loop_back=FALSE;

		state.cur_hud_bitmap_idx=idx;

		main_wind_draw();
		return;
	}

		// select text

	if ((id>=kHUDPropertyTextName) && (id<(kHUDPropertyTextName+max_iface_text))) {
		state.cur_hud_bitmap_idx=-1;
		state.cur_hud_text_idx=id-kHUDPropertyTextName;
		state.cur_hud_bar_idx=-1;
		main_wind_draw();
		return;
	}

		// delete text

	if ((id>=kHUDPropertyTextDelete) && (id<(kHUDPropertyTextDelete+max_iface_text))) {
		state.cur_hud_bitmap_idx=-1;
		state.cur_hud_text_idx=-1;
		state.cur_hud_bar_idx=-1;

		idx=id-kHUDPropertyTextDelete;

		sz=(iface.text_list.ntext-idx)-1;
		if (sz>0) memmove(&iface.text_list.texts[idx],&iface.text_list.texts[idx+1],(sz*sizeof(iface_text_type)));

		iface.text_list.ntext--;

		main_wind_draw();
		return;
	}

		// add text

	if (id==kHUDPropertyTextAdd) {
		state.cur_hud_bitmap_idx=-1;
		state.cur_hud_bar_idx=-1;

		if (iface.text_list.ntext>=max_iface_text) {
			os_dialog_alert("Add HUD Text","Reached the maximum number of texts");
			return;
		}
		
		name[0]=0x0;
		dialog_property_string_run(list_string_value_string,(void*)name,name_str_len,0,0);
		if (name[0]==0x0) return;

		idx=iface.text_list.ntext;
		iface.text_list.ntext++;

		iface.text_list.texts[idx].x=0;
		iface.text_list.texts[idx].y=0;
		iface.text_list.texts[idx].size=20;
		iface.text_list.texts[idx].just=tx_left;
		iface.text_list.texts[idx].special=text_special_none;
		iface.text_list.texts[idx].alpha=0.0f;
		strcpy(iface.text_list.texts[idx].name,name);
		iface.text_list.texts[idx].data[0]=0x0;
		iface.text_list.texts[idx].show=TRUE;
		iface.text_list.texts[idx].has_return=FALSE;
		iface.text_list.texts[idx].color.r=1.0f;
		iface.text_list.texts[idx].color.g=1.0f;
		iface.text_list.texts[idx].color.b=1.0f;

		iface.text_list.texts[idx].fade.on=FALSE;
		iface.text_list.texts[idx].fade.fade_in_tick=0;
		iface.text_list.texts[idx].fade.life_tick=0;
		iface.text_list.texts[idx].fade.fade_out_tick=0;

		state.cur_hud_text_idx=idx;

		main_wind_draw();
		return;
	}

		// select bar

	if ((id>=kHUDPropertyBarName) && (id<(kHUDPropertyBarName+max_iface_bar))) {
		state.cur_hud_bitmap_idx=-1;
		state.cur_hud_text_idx=-1;
		state.cur_hud_bar_idx=id-kHUDPropertyBarName;
		main_wind_draw();
		return;
	}

		// delete bar

	if ((id>=kHUDPropertyBarDelete) && (id<(kHUDPropertyBarDelete+max_iface_bar))) {
		state.cur_hud_bitmap_idx=-1;
		state.cur_hud_text_idx=-1;
		state.cur_hud_bar_idx=-1;

		idx=id-kHUDPropertyBarDelete;

		sz=(iface.bar_list.nbar-idx)-1;
		if (sz>0) memmove(&iface.bar_list.bars[idx],&iface.bar_list.bars[idx+1],(sz*sizeof(iface_bar_type)));

		iface.bar_list.nbar--;

		main_wind_draw();
		return;
	}

		// add bar

	if (id==kHUDPropertyBarAdd) {
		state.cur_hud_bitmap_idx=-1;
		state.cur_hud_text_idx=-1;

		if (iface.bar_list.nbar>=max_iface_bar) {
			os_dialog_alert("Add HUD Bar","Reached the maximum number of bars");
			return;
		}
		
		name[0]=0x0;
		dialog_property_string_run(list_string_value_string,(void*)name,name_str_len,0,0);
		if (name[0]==0x0) return;

		idx=iface.bar_list.nbar;
		iface.bar_list.nbar++;

		iface.bar_list.bars[idx].x=0;
		iface.bar_list.bars[idx].y=0;
		iface.bar_list.bars[idx].x_size=20;
		iface.bar_list.bars[idx].y_size=20;
		iface.bar_list.bars[idx].fill_alpha=1.0f;
		iface.bar_list.bars[idx].outline_alpha=1.0f;
		iface.bar_list.bars[idx].value=0.0f;
		strcpy(iface.bar_list.bars[idx].name,name);
		iface.bar_list.bars[idx].show=TRUE;
		iface.bar_list.bars[idx].outline=TRUE;
		iface.bar_list.bars[idx].vert=FALSE;

		iface.bar_list.bars[idx].fill_start_color.r=0.5f;
		iface.bar_list.bars[idx].fill_start_color.g=0.5f;
		iface.bar_list.bars[idx].fill_start_color.b=0.5f;

		iface.bar_list.bars[idx].fill_end_color.r=0.8f;
		iface.bar_list.bars[idx].fill_end_color.g=0.8f;
		iface.bar_list.bars[idx].fill_end_color.b=0.8f;

		iface.bar_list.bars[idx].outline_color.r=1.0f;
		iface.bar_list.bars[idx].outline_color.g=1.0f;
		iface.bar_list.bars[idx].outline_color.b=1.0f;

		state.cur_hud_bar_idx=idx;

		main_wind_draw();
		return;
	}
	
		// regular settings

	switch (id) {

			// chat
			
		case kHUDPropertyChatX:
			dialog_property_string_run(list_string_value_positive_int,(void*)&iface.chat.x,0,0,0);
			break;
			
		case kHUDPropertyChatY:
			dialog_property_string_run(list_string_value_positive_int,(void*)&iface.chat.y,0,0,0);
			break;
			
		case kHUDPropertyChatLastAddLifeSeconds:
			dialog_property_string_run(list_string_value_positive_int,(void*)&iface.chat.last_add_life_sec,0,0,0);
			break;
			
		case kHUDPropertyChatNextLifeSeconds:
			dialog_property_string_run(list_string_value_positive_int,(void*)&iface.chat.next_life_sec,0,0,0);
			break;
			
	}

		// redraw

	main_wind_draw();
}

