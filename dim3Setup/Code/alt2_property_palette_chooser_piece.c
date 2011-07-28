/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Alt2 Property Palette Chooser Piece

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

#define kChooserPieceSettingsType				0
#define kChooserPieceSettingsId					1
#define kChooserPieceSettingsClickable			2
#define kChooserPieceSettingsGoto				3

#define kChooserPiecePositionX					4
#define kChooserPiecePositionY					5
#define kChooserPiecePositionWid				6
#define kChooserPiecePositionHigh				7

#define kChooserPieceTextStr					8
#define kChooserPieceTextSize					9
#define kChooserPieceTextJust					10
#define kChooserPieceItemFile					11
#define kChooserPieceModelModelName				12
#define kChooserPieceModelAnimateName			13
#define kChooserPieceModelResize				14
#define kChooserPieceModelRot					15
#define kChooserPieceButtonText					16

extern iface_type				iface;
extern setup_state_type			state;
extern list_palette_type		alt2_property_palette;

extern char						chooser_type_str[][32],
								hud_text_just_type_str[][32];

/* =======================================================

      Property Palette Fill Chooser Piece
      
======================================================= */

void alt2_property_palette_fill_chooser_piece(int chooser_idx,int chooser_piece_idx)
{
	iface_chooser_type				*chooser;
	iface_chooser_piece_type		*piece;

	chooser=&iface.chooser_list.choosers[chooser_idx];
	piece=&chooser->pieces[chooser_piece_idx];

	list_palette_set_sub2_title(&alt2_property_palette,"Chooser Piece",chooser->name,chooser_type_str[piece->type]);

		// settings

	list_palette_add_header(&alt2_property_palette,0,"Settings");
	list_palette_add_string(&alt2_property_palette,kChooserPieceSettingsType,"Type",chooser_type_str[piece->type],FALSE);
	list_palette_add_string_int(&alt2_property_palette,kChooserPieceSettingsId,"Id",piece->id,FALSE);
	list_palette_add_checkbox(&alt2_property_palette,kChooserPieceSettingsClickable,"Clickable",piece->clickable,FALSE);
	list_palette_add_string(&alt2_property_palette,kChooserPieceSettingsGoto,"Goto Chooser",piece->goto_name,FALSE);

		// position

	list_palette_add_header(&alt2_property_palette,0,"Position");
	list_palette_add_string_int(&alt2_property_palette,kChooserPiecePositionX,"X",piece->x,FALSE);
	list_palette_add_string_int(&alt2_property_palette,kChooserPiecePositionY,"Y",piece->y,FALSE);
	list_palette_add_string_int(&alt2_property_palette,kChooserPiecePositionWid,"Width",piece->wid,FALSE);
	list_palette_add_string_int(&alt2_property_palette,kChooserPiecePositionHigh,"Height",piece->high,FALSE);

		// types

	switch (piece->type) {

		case chooser_piece_type_text:
			list_palette_add_header(&alt2_property_palette,0,"Text");
			list_palette_add_string(&alt2_property_palette,kChooserPieceTextStr,"Text",piece->data.text.str,FALSE);
			list_palette_add_string_int(&alt2_property_palette,kChooserPieceTextSize,"Size",piece->data.text.size,FALSE);
			list_palette_add_string(&alt2_property_palette,kChooserPieceTextJust,"Justification",hud_text_just_type_str[piece->data.text.just],FALSE);
			break;

		case chooser_piece_type_item:
			list_palette_add_header(&alt2_property_palette,0,"Item");
			list_palette_add_string(&alt2_property_palette,kChooserPieceItemFile,"Bitmap",piece->data.item.file,FALSE);
			break;

		case chooser_piece_type_model:
			list_palette_add_header(&alt2_property_palette,0,"Model");
			list_palette_add_string(&alt2_property_palette,kChooserPieceModelModelName,"Model",piece->data.model.model_name,FALSE);
			list_palette_add_string(&alt2_property_palette,kChooserPieceModelAnimateName,"Animation",piece->data.model.animate_name,FALSE);
			list_palette_add_string_float(&alt2_property_palette,kChooserPieceModelResize,"Resize",piece->data.model.resize,FALSE);
			list_palette_add_angle(&alt2_property_palette,kChooserPieceModelRot,"Rotate",&piece->data.model.rot,FALSE);
			break;

		case chooser_piece_type_button:
			list_palette_add_header(&alt2_property_palette,0,"Button");
			list_palette_add_string(&alt2_property_palette,kChooserPieceButtonText,"Text",piece->data.button.name,FALSE);
			break;

	}
}

/* =======================================================

      Property Palette Click Chooser Piece
      
======================================================= */

void alt2_property_palette_click_chooser_piece(int chooser_idx,int chooser_piece_idx,int id)
{
	iface_chooser_piece_type		*piece;

	piece=&iface.chooser_list.choosers[chooser_idx].pieces[chooser_piece_idx];

	switch (id) {

			// settings

		case kChooserPieceSettingsType:
			bzero(&piece->data,sizeof(iface_chooser_piece_data_type));		// need to clear data or it'll crash
			property_pick_list("Pick a Chooser Piece Type",(char*)chooser_type_str,&piece->type);
			break;

		case kChooserPieceSettingsId:
			dialog_property_string_run(list_string_value_int,(void*)&piece->id,0,0,0);
			break;

		case kChooserPieceSettingsClickable:
			piece->clickable=!piece->clickable;
			break;

		case kChooserPieceSettingsGoto:
			dialog_property_string_run(list_string_value_string,(void*)piece->goto_name,name_str_len,0,0);
			break;

			// position

		case kChooserPiecePositionX:
			dialog_property_string_run(list_string_value_positive_int,(void*)&piece->x,0,0,0);
			break;

		case kChooserPiecePositionY:
			dialog_property_string_run(list_string_value_positive_int,(void*)&piece->y,0,0,0);
			break;

		case kChooserPiecePositionWid:
			dialog_property_string_run(list_string_value_int,(void*)&piece->wid,0,0,0);
			break;

		case kChooserPiecePositionHigh:
			dialog_property_string_run(list_string_value_int,(void*)&piece->high,0,0,0);
			break;

			// types

		case kChooserPieceTextStr:
			dialog_property_string_run(list_string_value_string,(void*)piece->data.text.str,max_chooser_text_data_sz,0,0);
			break;

		case kChooserPieceTextSize:
			dialog_property_string_run(list_string_value_positive_int,(void*)&piece->data.text.size,0,0,0);
			break;

		case kChooserPieceTextJust:
			property_pick_list("Pick a Justification",(char*)hud_text_just_type_str,&piece->data.text.just);
			break;

		case kChooserPieceItemFile:
			property_pick_file("Pick a Chooser Bitmap","Chooser","png",NULL,piece->data.item.file);
			break;

		case kChooserPieceModelModelName:
			property_pick_file("Pick a Model","Models",NULL,"Mesh.xml",piece->data.model.model_name);
			break;

		case kChooserPieceModelAnimateName:
			dialog_property_string_run(list_string_value_string,(void*)piece->data.model.animate_name,name_str_len,0,0);
			break;

		case kChooserPieceModelResize:
			dialog_property_string_run(list_string_value_positive_float,(void*)&piece->data.model.resize,0,0,0);
			break;

		case kChooserPieceModelRot:
			dialog_property_chord_run(list_chord_value_angle,(void*)&piece->data.model.rot);
			break;

		case kChooserPieceButtonText:
			dialog_property_string_run(list_string_value_string,(void*)piece->data.button.name,max_chooser_button_text_sz,0,0);
			break;

	}

		// redraw

	main_wind_draw();
}

