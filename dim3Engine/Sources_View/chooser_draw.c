/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: GUI Chooser

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

#include "scripts.h"
#include "models.h"
#include "interfaces.h"
#include "consoles.h"
#include "inputs.h"
#include "video.h"

extern server_type			server;
extern js_type				js;
extern hud_type				hud;
extern setup_type			setup;

int							chooser_idx;
char						chooser_sub_txt[max_chooser_sub_txt][max_chooser_text_data_sz];

extern int chooser_find(char *name);
extern int chooser_find_piece(chooser_type *chooser,int id);

/* =======================================================

      Text Substitutions
      
======================================================= */

void chooser_text_substitute(char *init_str,char *sub_str,int max_len)
{
	int				idx,len,sz;
	char			ch,*c,*c2;
	
	c=init_str;
	c2=sub_str;
	
	while (*c!=0x0) {
	
			// are we over text string max size?
			
		if ((int)(c2-sub_str)>=(max_len-1)) break;
	
			// are we hitting a {0..9} substitution string?
			
		if (*c=='{') {
		
			ch=*(c+1);
			idx=(int)(ch-'0');
			
			if ((idx>=0) && (idx<max_chooser_sub_txt) && (*(c+2)=='}')) {
			
				if (chooser_sub_txt[idx][0]!=0x0) {
					sz=(max_len-1)-(int)(c-init_str);
					len=strlen(chooser_sub_txt[idx]);
					if (len>sz) break;
					
					*c2=0x0;
					strcpy(c2,chooser_sub_txt[idx]);
					c2+=len;
				}
				
				c+=3;
				continue;
			}
		}
		
		*c2++=*c++;
	}
	
	*c2=0x0;
}

/* =======================================================

      Start and Stop Models
      
======================================================= */

void chooser_init_models(void)
{
	int					n;
	char				err_str[256];
	model_draw			*draw;
	chooser_type		*chooser;
	chooser_piece_type	*piece;
	
	chooser=&hud.choosers[chooser_idx];

		// initialize any models to draw in
		// this chooser

	piece=chooser->pieces;

	for (n=0;n!=chooser->npiece;n++) {

		if (piece->type==chooser_piece_type_model) {

			piece->data.model.draw=NULL;

			draw=(model_draw*)malloc(sizeof(model_draw));
			if (draw!=NULL) {
				strcpy(draw->name,piece->data.model.model_name);
				if (!model_draw_load(draw,"Interface","Chooser",err_str)) {
					console_add_error(err_str);
					free(draw);
				}
				else {
					piece->data.model.draw=draw;
				}
			}
		}

		piece++;
	}
}

void chooser_free_models(void)
{
	int					n;
	chooser_type		*chooser;
	chooser_piece_type	*piece;
	
	chooser=&hud.choosers[chooser_idx];

		// free any loaded models

	piece=chooser->pieces;

	for (n=0;n!=chooser->npiece;n++) {

		if (piece->type==chooser_piece_type_model) {
			if (piece->data.model.draw!=NULL) {
				model_draw_dispose(piece->data.model.draw);
				free(piece->data.model.draw);
			}

			piece->data.model.draw=NULL;
		}

		piece++;
	}
}

/* =======================================================

      Chooser Operations
      
======================================================= */

void chooser_create_elements(void)
{
	int					n;
	char				path[1024],path2[1024],fname[256],
						title[max_chooser_frame_text_sz],str[max_chooser_text_data_sz];
	chooser_type		*chooser;
	chooser_piece_type	*piece;
	chooser_frame_type	frame;
	
	chooser=&hud.choosers[chooser_idx];
	
		// setup frame
		
	memmove(&frame,&chooser->frame,sizeof(chooser_frame_type));

	chooser_text_substitute(chooser->frame.title,title,max_chooser_frame_text_sz);
	strcpy(frame.title,title);
	
	gui_set_frame(&frame);
		
		// pieces

	piece=chooser->pieces;

	for (n=0;n!=chooser->npiece;n++) {

		switch (piece->type) {

			case chooser_piece_type_text:
				chooser_text_substitute(piece->data.text.str,str,max_chooser_text_data_sz);
				element_text_add(str,piece->id,piece->x,piece->y,piece->data.text.size,piece->data.text.just,piece->clickable,FALSE);
				break;

			case chooser_piece_type_item:
				file_paths_data(&setup.file_path_setup,path,"Chooser",piece->data.item.file,"png");
				
				if (piece->clickable) {
					sprintf(fname,"%s_selected",piece->data.item.file);
					file_paths_data(&setup.file_path_setup,path2,"Chooser",fname,"png");
					element_button_bitmap_add(path,path2,piece->id,piece->x,piece->y,piece->wid,piece->high,element_pos_left,element_pos_top);
				}
				else {
					element_bitmap_add(path,0,piece->x,piece->y,piece->wid,piece->high,FALSE);
				}
				break;

			case chooser_piece_type_button:
				element_button_text_add(piece->data.button.name,piece->id,piece->x,piece->y,piece->wid,piece->high,element_pos_left,element_pos_top);
				break;

		}

		piece++;
	}
}

void chooser_open(void)
{
	gui_initialize(NULL,NULL);
	chooser_create_elements();
	chooser_init_models();
}

void chooser_close(void)
{
	chooser_free_models();
	gui_shutdown();
}

bool chooser_setup(char *name,char *sub_txt,char *err_str)
{
	int				n;

		// find chooser

	chooser_idx=chooser_find(name);
	if (chooser_idx==-1) {
		sprintf(err_str,"Chooser does not exist: %s",name);
		return(FALSE);
	}

		// setup text substitutions

	if (sub_txt!=NULL) {
		for (n=0;n!=max_chooser_sub_txt;n++) {
			strcpy(chooser_sub_txt[n],(char*)&sub_txt[max_chooser_text_data_sz*n]);
		}
	}
	
		// run chooser
		
	server.next_state=gs_chooser;
	
	return(TRUE);
}

/* =======================================================

      Chooser Draw Models
      
======================================================= */

void chooser_model_draw(void)
{
	int					n,k,t,yoff,model_idx;
	float				ratio;
	d3pnt				*pnt;
	chooser_type		*chooser;
	chooser_piece_type	*piece;
	model_type			*mdl;
	model_mesh_type		*mesh;
	model_trig_type		*trig;
	
	chooser=&hud.choosers[chooser_idx];
	
		// pieces

	piece=chooser->pieces;

	for (n=0;n!=chooser->npiece;n++) {

		piece=&chooser->pieces[n];
		if (piece->type!=chooser_piece_type_model) continue;

			// get model

		model_idx=piece->data.model.draw->model_idx;
		if (model_idx==-1) continue;

		mdl=server.model_list.models[model_idx];
		if (mdl==NULL) continue;

			// setup drawing

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		ratio=((float)setup.screen.x_sz)/((float)setup.screen.y_sz);
		gluPerspective(45.0,ratio,(float)100,(float)25000);
	//	glScalef(1.0f,1.0f,1.0f);

		yoff=mdl->view_box.size.y/2;
	//	glTranslatef(-((GLfloat)piece->x),-((GLfloat)(piece->y-yoff)),(GLfloat)1000);

		glTranslatef(0,0,1000);
	
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	
//	glRotatef(-ang_x,1.0f,0.0f,0.0f);
//	glRotatef(angle_add(ang_y,180.0f),0.0f,1.0f,0.0f);

		mesh=&mdl->meshes[0];

glDisable(GL_DEPTH_TEST);

		glColor4f(1.0f,0.5f,0.0f,1.0f);

		glBegin(GL_TRIANGLES);

		trig=mesh->trigs;

		for (k=0;k!=mesh->ntrig;k++) {
			for (t=0;t!=3;t++) {
				pnt=&mesh->vertexes[trig->v[t]].pnt;
				glVertex3i(pnt->x,pnt->y,pnt->z);
			}

			trig++;
		}

		glEnd();

	}
}

/* =======================================================

      Chooser Input
      
======================================================= */

void chooser_click(void)
{
	int					id,idx,next_idx,ch;
	chooser_piece_type	*piece;
	
	id=-1;
	
		// check for ok/cancel keys
		
	ch=input_gui_get_keyboard_key(FALSE);
	if (ch!=0x0) {
		if (ch==0x1B) id=hud.choosers[chooser_idx].key.cancel_id;
		if (ch==0xD) id=hud.choosers[chooser_idx].key.ok_id;
	}
	
	if (id!=-1) {			// wait for key up
		while (input_gui_get_keyboard_key(FALSE)==ch) {}
	}

		// if no key check clicking
		
	if (id==-1) {
		id=gui_click();
		if (id==-1) return;
	}
	
		// run click
		
	hud_click();
	
		// check for any goto clicks
		
	idx=chooser_find_piece(&hud.choosers[chooser_idx],id);
	
	if (idx!=-1) {
	
		piece=&hud.choosers[chooser_idx].pieces[idx];
		next_idx=chooser_find(piece->goto_name);
		
		if (next_idx!=-1) {
			element_clear();
			chooser_idx=next_idx;
			chooser_create_elements();
			return;
		}
	}
	
		// set the state here as event
		// might reset it to something else
		
	server.next_state=gs_running;
	
	scripts_post_event_console(&js.game_attach,sd_event_chooser,0,id);
}

/* =======================================================

      Run Chooser
      
======================================================= */

void chooser_run(void)
{
	gui_draw(1.0f,TRUE,TRUE);
	chooser_model_draw();
	gl_frame_swap();

	chooser_click();
}
