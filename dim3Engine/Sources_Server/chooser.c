/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Chooser

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

extern server_type			server;
extern hud_type				hud;
extern setup_type			setup;

/* =======================================================

      Add Chooser
      
======================================================= */

int chooser_add(char *name)
{
	chooser_type	*chooser;
	
	if (hud.count.chooser>=max_chooser) return(-1);
	
	chooser=&hud.choosers[hud.count.chooser];
	hud.count.chooser++;
	
	strcpy(chooser->name,name);

	chooser->npiece=0;
	chooser->frame.on=FALSE;
	chooser->key.ok_id=-1;
	chooser->key.cancel_id=-1;
	
	return(hud.count.chooser-1);
}

void chooser_copy_template(int idx,int template_idx)
{
	char			name[name_str_len];
	
	strcpy(name,hud.choosers[idx].name);
	memmove(&hud.choosers[idx],&hud.choosers[template_idx],sizeof(chooser_type));
	strcpy(hud.choosers[idx].name,name);
}

/* =======================================================

      Find Chooser
      
======================================================= */

int chooser_find(char *name)
{
	int				n;
	chooser_type	*chooser;
	
	chooser=hud.choosers;
	
	for (n=0;n!=hud.count.chooser;n++) {
		if (strcasecmp(chooser->name,name)==0) return(n);
		chooser++;
	}
	
	return(-1);
}

int chooser_find_piece(chooser_type *chooser,int id)
{
	int				n;
	chooser_piece_type	*piece;

	piece=chooser->pieces;

	for (n=0;n!=chooser->npiece;n++) {
		if (piece->id==id) return(n);
		piece++;
	}

	return(-1);
}

/* =======================================================

      Add Chooser Items
      
======================================================= */

void chooser_add_text(int chooser_idx,int template_idx,int id,char *str,int x,int y,int size,int just,bool clickable,char *goto_name)
{
	int					piece_idx;
	chooser_type		*chooser;
	chooser_piece_type	*piece;
	
	chooser=&hud.choosers[chooser_idx];

		// is this a template substitution?

	if (template_idx!=-1) {
		piece_idx=chooser_find_piece(chooser,id);
		if (piece_idx!=-1) {
			piece=&chooser->pieces[piece_idx];
			strcpy(piece->data.text.str,str);
			return;
		}
	}

		// add a new piece
	
	if (chooser->npiece>=max_chooser_piece) return;
	
	piece=&chooser->pieces[chooser->npiece];
	chooser->npiece++;
	
	piece->type=chooser_piece_type_text;
	piece->id=id;
	strcpy(piece->data.text.str,str);
	piece->x=x;
	piece->y=y;
	piece->data.text.size=size;
	piece->data.text.just=just;

	piece->clickable=clickable;
	strcpy(piece->goto_name,goto_name);
}

void chooser_add_item(int chooser_idx,int template_idx,int id,char *file,int x,int y,int wid,int high,bool clickable,char *goto_name)
{
	int					piece_idx;
	chooser_type		*chooser;
	chooser_piece_type	*piece;
	
	chooser=&hud.choosers[chooser_idx];

		// is this a template substitution?

	if (template_idx!=-1) {
		piece_idx=chooser_find_piece(chooser,id);
		if (piece_idx!=-1) {
			piece=&chooser->pieces[piece_idx];
			strcpy(piece->data.item.file,file);
			return;
		}
	}

		// add a new piece

	if (chooser->npiece>=max_chooser_piece) return;
	
	piece=&chooser->pieces[chooser->npiece];
	chooser->npiece++;
	
	piece->type=chooser_piece_type_item;
	piece->id=id;
	strcpy(piece->data.item.file,file);
	piece->x=x;
	piece->y=y;
	piece->wid=wid;
	piece->high=high;

	piece->clickable=clickable;
	strcpy(piece->goto_name,goto_name);
}

void chooser_add_model(int chooser_idx,int template_idx,int id,char *model_name,char *animate_name,int x,int y,d3ang *rot,float resize,bool clickable,char *goto_name)
{
	int					piece_idx;
	chooser_type		*chooser;
	chooser_piece_type	*piece;
	
	chooser=&hud.choosers[chooser_idx];
	
		// is this a template substitution?

	if (template_idx!=-1) {
		piece_idx=chooser_find_piece(chooser,id);
		if (piece_idx!=-1) {
			piece=&chooser->pieces[piece_idx];
			strcpy(piece->data.model.model_name,model_name);
			strcpy(piece->data.model.animate_name,animate_name);
			return;
		}
	}

		// add a new piece

	if (chooser->npiece>=max_chooser_piece) return;
	
	piece=&chooser->pieces[chooser->npiece];
	chooser->npiece++;
	
	piece->type=chooser_piece_type_model;
	piece->id=id;
	strcpy(piece->data.model.model_name,model_name);
	strcpy(piece->data.model.animate_name,animate_name);

	memmove(&piece->data.model.rot,rot,sizeof(d3ang));
	piece->data.model.resize=resize;

	piece->x=x;
	piece->y=y;

	piece->clickable=clickable;
	strcpy(piece->goto_name,goto_name);
}

void chooser_add_button(int chooser_idx,int template_idx,int id,char *name,int x,int y,int wid,int high,char *goto_name)
{
	int					piece_idx;
	chooser_type		*chooser;
	chooser_piece_type	*piece;
	
	chooser=&hud.choosers[chooser_idx];
	
		// is this a template substitution?

	if (template_idx!=-1) {
		piece_idx=chooser_find_piece(chooser,id);
		if (piece_idx!=-1) {
			piece=&chooser->pieces[piece_idx];
			strcpy(piece->data.button.name,name);
			return;
		}
	}

		// add a new piece

	if (chooser->npiece>=max_chooser_piece) return;
	
	piece=&chooser->pieces[chooser->npiece];
	chooser->npiece++;
	
	piece->type=chooser_piece_type_button;
	piece->id=id;
	strcpy(piece->data.button.name,name);
	piece->x=x;
	piece->y=y;
	piece->wid=wid;
	piece->high=high;

	piece->clickable=TRUE;
	strcpy(piece->goto_name,goto_name);
}

