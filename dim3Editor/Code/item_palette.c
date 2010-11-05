/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Item Palette

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
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "interface.h"
#include "view.h"
#include "dialog.h"

#define item_max_count			4096
#define item_font_size			12.0f
#define item_font_high			14
#define item_palette_border_sz	10
#define item_palette_tree_sz	150

typedef struct		{
						int						x,y,piece_type,piece_idx;
						bool					selected,is_col;
						char					name[name_str_len];
						d3col					col;
					} item_palette_item_type;

extern map_type					map;
extern editor_state_type		state;
extern editor_setup_type		setup;
extern file_path_setup_type		file_path_setup;

extern int						tool_palette_pixel_sz,txt_palette_pixel_sz;

int								item_palette_pixel_sz,item_palette_item_count,
								item_palette_piece_type,item_palette_piece_idx;
bool							item_palette_open;
d3rect							item_palette_box;

item_palette_item_type			*item_palette_items;

/* =======================================================

      Item Palette Setup
      
======================================================= */

void item_palette_initialize(void)
{
	item_palette_open=TRUE;

	item_palette_piece_type=spot_piece;
	item_palette_piece_idx=-1;

	item_palette_items=(item_palette_item_type*)malloc(item_max_count*sizeof(item_palette_item_type));
}

void item_palette_shutdown(void)
{
	if (item_palette_items!=NULL) free(item_palette_items);
}

void item_palette_setup(void)
{
	d3rect			wbox;
	
	os_get_window_box(&wbox);

	if (item_palette_open) {
		item_palette_pixel_sz=item_palette_tree_sz;
	}
	else {
		item_palette_pixel_sz=item_palette_border_sz;
	}

	item_palette_box.lx=wbox.rx-item_palette_pixel_sz;
	item_palette_box.rx=wbox.rx;
	item_palette_box.ty=wbox.ty+(tool_palette_pixel_sz+1);
	item_palette_box.by=wbox.by-txt_palette_pixel_sz;
}

/* =======================================================

      Item Palette Items
      
======================================================= */

void item_palette_add_item(int indent,int piece_type,int piece_idx,char *name,d3col *col,bool selected)
{
	item_palette_item_type		*item;

	item=&item_palette_items[item_palette_item_count];
	item_palette_item_count++;

	item->x=indent+(item_palette_box.lx+(item_palette_border_sz+4));
	item->y=(item_palette_item_count*item_font_high)+item_palette_box.ty;

	item->piece_type=piece_type;
	item->piece_idx=piece_idx;
	item->selected=selected;

	if (name!=NULL) {
		strcpy(item->name,name);
	}
	else {
		item->name[0]=0x0;
	}

	if (col!=NULL) {
		item->is_col=TRUE;
		memmove(&item->col,col,sizeof(d3col));
	}
	else {
		item->is_col=FALSE;
		item->col.r=item->col.g=item->col.b=0.0f;
	}
}

void item_palette_delete_all_items(void)
{
	item_palette_item_count=0;
}

/* =======================================================

      Item Palette Fill Tree
      
======================================================= */

void item_palette_fill_tree(void)
{
	int			n;

	item_palette_delete_all_items();

		// spots

	item_palette_add_item(0,spot_piece,-1,"Spots",NULL,(item_palette_piece_type==spot_piece));

	if (item_palette_piece_type==spot_piece) {
		for (n=0;n!=map.nspot;n++) {
			item_palette_add_item(10,spot_piece,n,map.spots[n].name,NULL,(n==item_palette_piece_idx));
		}
	}

		// lights

	item_palette_add_item(0,light_piece,-1,"Lights",NULL,(item_palette_piece_type==light_piece));

	if (item_palette_piece_type==light_piece) {
		for (n=0;n!=map.nlight;n++) {
			item_palette_add_item(10,light_piece,n,NULL,&map.lights[n].col,(n==item_palette_piece_idx));
		}
	}

		// sounds

	item_palette_add_item(0,sound_piece,-1,"Sounds",NULL,(item_palette_piece_type==sound_piece));

	if (item_palette_piece_type==sound_piece) {
		for (n=0;n!=map.nsound;n++) {
			item_palette_add_item(10,sound_piece,n,map.sounds[n].name,NULL,(n==item_palette_piece_idx));
		}
	}

		// particles

	item_palette_add_item(0,particle_piece,-1,"Particles",NULL,(item_palette_piece_type==particle_piece));

	if (item_palette_piece_type==particle_piece) {
		for (n=0;n!=map.nparticle;n++) {
			item_palette_add_item(10,particle_piece,n,map.particles[n].name,NULL,(n==item_palette_piece_idx));
		}
	}

		// sceneries

	item_palette_add_item(0,scenery_piece,-1,"Scenery",NULL,(item_palette_piece_type==scenery_piece));

	if (item_palette_piece_type==scenery_piece) {
		for (n=0;n!=map.nscenery;n++) {
			item_palette_add_item(10,scenery_piece,n,map.sceneries[n].model_name,NULL,(n==item_palette_piece_idx));
		}
	}

		// nodes

	item_palette_add_item(0,node_piece,-1,"Nodes",NULL,(item_palette_piece_type==node_piece));

	if (item_palette_piece_type==node_piece) {
		for (n=0;n!=map.nnode;n++) {
			if (map.nodes[n].name[0]!=0x0) item_palette_add_item(10,node_piece,n,map.nodes[n].name,NULL,(n==item_palette_piece_idx));
		}
	}

		// groups

	item_palette_add_item(0,group_piece,-1,"Groups",NULL,(item_palette_piece_type==group_piece));

	if (item_palette_piece_type==group_piece) {
		for (n=0;n!=map.ngroup;n++) {
			item_palette_add_item(10,group_piece,n,map.groups[n].name,NULL,(n==item_palette_piece_idx));
		}
	}

		// movements

	item_palette_add_item(0,movement_piece,-1,"Movements",NULL,(item_palette_piece_type==movement_piece));

	if (item_palette_piece_type==movement_piece) {
		for (n=0;n!=map.nmovement;n++) {
			item_palette_add_item(10,movement_piece,n,map.movements[n].name,NULL,(n==item_palette_piece_idx));
		}
	}

		// cinemas

	item_palette_add_item(0,cinema_piece,-1,"Cinemas",NULL,(item_palette_piece_type==cinema_piece));

	if (item_palette_piece_type==cinema_piece) {
		for (n=0;n!=map.cinema.ncinema;n++) {
			item_palette_add_item(10,cinema_piece,n,map.cinema.cinemas[n].name,NULL,(n==item_palette_piece_idx));
		}
	}
}

/* =======================================================

      Item Palette Draw
      
======================================================= */

void item_palette_draw_tree_item(item_palette_item_type *item)
{
		// early exits

	if ((item->y<item_palette_box.ty) || ((item->y-item_font_high)>item_palette_box.by)) return;

		// draw selection

	if (item->selected) {
		glColor4f(1.0f,1.0f,0.0f,1.0f);
		
		glBegin(GL_QUADS);
		glVertex2i(item_palette_box.lx,(item->y-item_font_high));
		glVertex2i(item_palette_box.rx,(item->y-item_font_high));
		glVertex2i(item_palette_box.rx,item->y);
		glVertex2i(item_palette_box.lx,item->y);
		glEnd();
	}

		// draw text

	if (!item->is_col) {
		text_draw(item->x,item->y,item_font_size,FALSE,item->name);
	}

		// draw color

	else {
		glColor4f(item->col.r,item->col.g,item->col.b,1.0f);
		
		glBegin(GL_QUADS);
		glVertex2i(item->x,((item->y-item_font_high)+1));
		glVertex2i((item->x+item_font_high),((item->y-item_font_high)+1));
		glVertex2i((item->x+item_font_high),(item->y-1));
		glVertex2i(item->x,(item->y-1));
		glEnd();

		glDisable(GL_BLEND);

		glColor4f(0.0f,0.0f,0.0f,1.0f);

		glBegin(GL_LINE_LOOP);
		glVertex2i(item->x,((item->y-item_font_high)+1));
		glVertex2i((item->x+item_font_high),((item->y-item_font_high)+1));
		glVertex2i((item->x+item_font_high),(item->y-1));
		glVertex2i(item->x,(item->y-1));
		glEnd();

		glEnable(GL_BLEND);
	}
}

void item_palette_draw(void)
{
	int						n;
	d3rect					wbox;
	item_palette_item_type	*item;

		// viewport setup
		
	os_get_window_box(&wbox);

	glEnable(GL_SCISSOR_TEST);
	glScissor(item_palette_box.lx,(wbox.by-item_palette_box.by),(item_palette_box.rx-item_palette_box.lx),(item_palette_box.by-item_palette_box.ty));

	glViewport(item_palette_box.lx,(wbox.by-item_palette_box.by),(item_palette_box.rx-item_palette_box.lx),(item_palette_box.by-item_palette_box.ty));
		
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho((GLdouble)item_palette_box.lx,(GLdouble)item_palette_box.rx,(GLdouble)item_palette_box.by,(GLdouble)item_palette_box.ty,-1.0,1.0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

		// draw setup

	glDisable(GL_DEPTH_TEST);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

		// background

	glColor4f(1.0f,1.0f,1.0f,1.0f);
		
	glBegin(GL_QUADS);
	glVertex2i(item_palette_box.lx,item_palette_box.ty);
	glVertex2i(item_palette_box.rx,item_palette_box.ty);
	glVertex2i(item_palette_box.rx,item_palette_box.by);
	glVertex2i(item_palette_box.lx,item_palette_box.by);
	glEnd();

		// items

	item_palette_fill_tree();
	
	item=item_palette_items;

	for (n=0;n!=item_palette_item_count;n++) {
		item_palette_draw_tree_item(item);
		item++;
	}
	
		// click-close border
		
	glColor4f(0.0f,0.0f,0.5f,1.0f);

	glBegin(GL_QUADS);
	glVertex2i(item_palette_box.lx,item_palette_box.ty);
	glVertex2i((item_palette_box.lx+item_palette_border_sz),item_palette_box.ty);
	glVertex2i((item_palette_box.lx+item_palette_border_sz),item_palette_box.by);
	glVertex2i(item_palette_box.lx,item_palette_box.by);
	glEnd();

	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
}

/* =======================================================

      Item Palette Reset For Selection Change
      
======================================================= */

void item_palette_reset(void)
{
	int				sel_type,main_idx,sub_idx;

	if (select_count()==0) return;

	select_get(0,&sel_type,&main_idx,&sub_idx);
	if ((sel_type==mesh_piece) || (sel_type==liquid_piece)) return;

	item_palette_piece_type=sel_type;
	item_palette_piece_idx=main_idx;
}

/* =======================================================

      Item Palette Click
      
======================================================= */

void item_palette_click(d3pnt *pnt,bool dblclick)
{
	int						item_idx;
	item_palette_item_type	*item;

	pnt->x-=item_palette_box.lx;
	pnt->y-=item_palette_box.ty;

		// click in close border

	if (pnt->x<=item_palette_border_sz) {
		item_palette_open=!item_palette_open;
		item_palette_setup();

		main_wind_draw();
		return;
	}

		// click in item

	item_idx=pnt->y/item_font_high;
	if ((item_idx<0) || (item_idx>=item_palette_item_count)) return;

		// if we click on a header that
		// is already open, close it

	item=&item_palette_items[item_idx];

	if ((item->piece_type==item_palette_piece_type) && (item->piece_idx==-1)) {
		item_palette_piece_type=-1;
		item_palette_piece_idx=-1;

		main_wind_draw();
		return;
	}

		// select clicked item

	item_palette_piece_type=item->piece_type;
	item_palette_piece_idx=item->piece_idx;

	if ((item->piece_idx!=-1) && (item->piece_type!=cinema_piece)) {
		select_clear();

		switch (item->piece_type) {
			case group_piece:
				select_add_group(item->piece_idx);
				break;
			case movement_piece:
				select_add_movement(item->piece_idx);
				break;
			default:
				select_add(item->piece_type,item->piece_idx,-1);
				break;
		}

		view_goto_select();
	}

		// turn on any hidden items

	switch (item_palette_piece_type) {

		case spot_piece:
		case scenery_piece:
			state.show_object=TRUE;
			break;

		case light_piece:
		case sound_piece:
		case particle_piece:
			state.show_lightsoundparticle=TRUE;
			break;

		case node_piece:
			state.show_node=TRUE;
			break;
	}
	
		// redraw

	main_wind_draw();

		// if double click, edit

	if (!dblclick) return;

	switch (item_palette_piece_type) {

		case cinema_piece:
			dialog_map_cinemas_run(item_palette_piece_idx);
			break;

		case group_piece:
			dialog_map_groups_run(item_palette_piece_idx);
			break;

		case movement_piece:
			dialog_map_movements_run(item_palette_piece_idx);
			break;

	}
}