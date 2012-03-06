/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Chooser

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

#define kChooserPropertyName					0
#define kChooserPropertyTemplateName			1

#define kChooserPropertyFrameOn					10
#define kChooserPropertyFrameX					11
#define kChooserPropertyFrameY					12
#define kChooserPropertyFrameWid				13
#define kChooserPropertyFrameHigh				14
#define kChooserPropertyFrameTitle				15

#define kChooserPropertyOKID					20
#define kChooserPropertyCancelID				21

#define kChooserPropertyChooserItemAdd			100

#define kChooserPropertyChooserItemName			1000
#define kChooserPropertyChooserItemDelete		2000

extern iface_type				iface;
extern setup_state_type			state;
extern list_palette_type		property_palette;

char							chooser_type_str[][32]={"Text","Item","Model","Button",""};

/* =======================================================

      Property Palette Fill Chooser
      
======================================================= */

void property_palette_fill_chooser(int chooser_idx)
{
	int						n;
	char					str[1024];
	iface_chooser_type		*chooser;

	chooser=&iface.chooser_list.choosers[chooser_idx];

	list_palette_set_title(&property_palette,"Choosers",NULL,"Chooser",chooser->name,NULL,NULL);

		// settings

	list_palette_add_header(&property_palette,0,"Settings");
	list_palette_add_string(&property_palette,kChooserPropertyName,"Name",chooser->name,FALSE);
	list_palette_add_string(&property_palette,kChooserPropertyTemplateName,"Template",chooser->template_name,FALSE);

		// frame

	list_palette_add_header(&property_palette,0,"Frame");
	list_palette_add_checkbox(&property_palette,kChooserPropertyFrameOn,"On",&chooser->frame.on,FALSE);
	list_palette_add_int(&property_palette,kChooserPropertyFrameX,"X",&chooser->frame.x,FALSE);
	list_palette_add_int(&property_palette,kChooserPropertyFrameY,"Y",&chooser->frame.y,FALSE);
	list_palette_add_int(&property_palette,kChooserPropertyFrameWid,"Wid",&chooser->frame.wid,FALSE);
	list_palette_add_int(&property_palette,kChooserPropertyFrameHigh,"High",&chooser->frame.high,FALSE);
	list_palette_add_string(&property_palette,kChooserPropertyFrameTitle,"Title",chooser->frame.title,FALSE);

		// keys

	list_palette_add_header(&property_palette,0,"Keys");
	list_palette_add_int(&property_palette,kChooserPropertyOKID,"OK Id",&chooser->key.ok_id,FALSE);
	list_palette_add_int(&property_palette,kChooserPropertyCancelID,"Cancel Id",&chooser->key.cancel_id,FALSE);

		// chooser items

	list_palette_add_header_button(&property_palette,kChooserPropertyChooserItemAdd,"Items",list_button_plus);

	list_palette_sort_mark_start(&property_palette);
	
	for (n=0;n!=chooser->npiece;n++) {

		strcpy(str,chooser_type_str[chooser->pieces[n].type]);
		strcat(str," (");

		switch (chooser->pieces[n].type) {

			case chooser_piece_type_text:
				strcat(str,chooser->pieces[n].data.text.str);
				break;

			case chooser_piece_type_item:
				strcat(str,chooser->pieces[n].data.item.file);
				break;

			case chooser_piece_type_model:
				strcat(str,chooser->pieces[n].data.model.model_name);
				break;

			case chooser_piece_type_button:
				strcat(str,chooser->pieces[n].data.button.name);
				break;

		}

		strcat(str,")");

		list_palette_add_string_selectable_button(&property_palette,(kChooserPropertyChooserItemName+n),list_button_minus,(kChooserPropertyChooserItemDelete+n),str,NULL,(state.cur_chooser_piece_idx==n),FALSE);
	}

	list_palette_sort(&property_palette);
}

/* =======================================================

      Property Palette Click Chooser
      
======================================================= */

void property_palette_click_chooser(int chooser_idx,int id,bool double_click)
{
	int						idx,sz;
	iface_chooser_type		*chooser;

	chooser=&iface.chooser_list.choosers[chooser_idx];

		// select piece

	if ((id>=kChooserPropertyChooserItemName) && (id<(kChooserPropertyChooserItemName+max_chooser_piece))) {
		state.cur_chooser_piece_idx=id-kChooserPropertyChooserItemName;
		if (double_click) list_palette_set_level(&property_palette,3);
		return;
	}

		// delete piece

	if ((id>=kChooserPropertyChooserItemDelete) && (id<(kChooserPropertyChooserItemDelete+max_chooser_piece))) {
		state.cur_chooser_piece_idx=-1;

		idx=id-kChooserPropertyChooserItemDelete;

		sz=(chooser->npiece-idx)-1;
		if (sz>0) memmove(&chooser->pieces[idx],&chooser->pieces[idx+1],(sz*sizeof(iface_chooser_piece_type)));

		chooser->npiece--;

		return;
	}

		// add piece

	if (id==kChooserPropertyChooserItemAdd) {
		state.cur_chooser_piece_idx=-1;

		if (chooser->npiece>=max_chooser_piece) {
			os_dialog_alert("Add Chooser Piece","Reached the maximum number of chooser pieces");
			return;
		}

		idx=chooser->npiece;
		chooser->npiece++;

		bzero(&chooser->pieces[idx],sizeof(iface_chooser_piece_type));

		chooser->pieces[idx].type=chooser_piece_type_text;
		chooser->pieces[idx].id=-1;
		chooser->pieces[idx].x=0;
		chooser->pieces[idx].y=0;
		chooser->pieces[idx].wid=-1;
		chooser->pieces[idx].high=-1;
		chooser->pieces[idx].clickable=FALSE;
		chooser->pieces[idx].goto_name[0]=0x0;
		
		bzero(&chooser->pieces[idx].data,sizeof(iface_chooser_piece_data_type));

		state.cur_chooser_idx=chooser_idx;
		state.cur_chooser_piece_idx=idx;

		list_palette_set_level(&property_palette,3);

		list_palette_start_picking_mode(&property_palette,"Pick a Chooser Piece Type",(char*)chooser_type_str,-1,name_str_len,0,FALSE,FALSE,&chooser->pieces[idx].type,NULL);

		return;
	}

		// regular picks, always
		// disable selection

	if (!double_click) return;

	state.cur_chooser_piece_idx=-1;

	switch (id) {

			// settings

		case kChooserPropertyName:
			dialog_property_string_run(list_string_value_string,(void*)chooser->name,name_str_len,0,0);
			break;

		case kChooserPropertyTemplateName:
			dialog_property_string_run(list_string_value_string,(void*)chooser->template_name,name_str_len,0,0);
			break;

			// frame

		case kChooserPropertyFrameTitle:
			dialog_property_string_run(list_string_value_string,(void*)chooser->frame.title,max_chooser_frame_text_sz,0,0);
			break;

	}
}

