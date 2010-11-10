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
#define item_font_high			16
#define item_palette_border_sz	10
#define item_palette_tree_sz	150
#define item_scroll_button_high	20

typedef struct		{
						int						x,y,piece_type,piece_idx;
						bool					selected,header,is_col;
						char					name[name_str_len];
						d3col					col;
					} item_palette_item_type;

extern map_type					map;
extern editor_state_type		state;
extern editor_setup_type		setup;
extern file_path_setup_type		file_path_setup;

extern int						tool_palette_pixel_sz,txt_palette_pixel_sz;

int								item_palette_pixel_sz,item_palette_page_high,
								item_palette_item_count,
								item_palette_scroll_page,item_palette_total_high,
								item_palette_piece_type,item_palette_piece_idx;
bool							item_palette_open,item_palette_piece_open[piece_count];
d3rect							item_palette_box;

item_palette_item_type			*item_palette_items;

/* =======================================================

      Item Palette Setup
      
======================================================= */

void item_palette_initialize(void)
{
	int				n;

	item_palette_open=TRUE;

	for (n=0;n!=piece_count;n++) {
		item_palette_piece_open[n]=FALSE;
	}

	item_palette_piece_open[spot_piece]=TRUE;

	item_palette_scroll_page=0;
	item_palette_total_high=0;

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

	item_palette_page_high=((item_palette_box.by-item_palette_box.ty)-(item_scroll_button_high*2))-item_font_high;
}

/* =======================================================

      Item Palette Items
      
======================================================= */

void item_palette_add_item(int piece_type,int piece_idx,char *name,d3col *col,bool selected,bool header)
{
	item_palette_item_type		*item;

	item=&item_palette_items[item_palette_item_count];
	item_palette_item_count++;

	item->x=item_palette_box.lx+(item_palette_border_sz+4);
	if (!header) item->x+=10;

	item->y=((item_palette_item_count*item_font_high)+item_scroll_button_high)+item_palette_box.ty;
	item->y-=(item_palette_scroll_page*item_palette_page_high);

	item_palette_total_high+=item_font_high;

	item->piece_type=piece_type;
	item->piece_idx=piece_idx;
	item->selected=selected;
	item->header=header;

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
	item_palette_total_high=0;
}

/* =======================================================

      Item Palette Fill Tree
      
======================================================= */

void item_palette_fill_tree(void)
{
	int			n;

	item_palette_delete_all_items();

		// spots

	item_palette_add_item(spot_piece,-1,"Spots",NULL,(item_palette_piece_type==spot_piece),TRUE);

	if (item_palette_piece_open[spot_piece]) {
		for (n=0;n!=map.nspot;n++) {
			item_palette_add_item(spot_piece,n,map.spots[n].name,NULL,((item_palette_piece_type==spot_piece)&&(n==item_palette_piece_idx)),FALSE);
		}
	}

		// lights

	item_palette_add_item(light_piece,-1,"Lights",NULL,(item_palette_piece_type==light_piece),TRUE);

	if (item_palette_piece_open[light_piece]) {
		for (n=0;n!=map.nlight;n++) {
			item_palette_add_item(light_piece,n,NULL,&map.lights[n].col,((item_palette_piece_type==light_piece)&&(n==item_palette_piece_idx)),FALSE);
		}
	}

		// sounds

	item_palette_add_item(sound_piece,-1,"Sounds",NULL,(item_palette_piece_type==sound_piece),TRUE);

	if (item_palette_piece_open[sound_piece]) {
		for (n=0;n!=map.nsound;n++) {
			item_palette_add_item(sound_piece,n,map.sounds[n].name,NULL,((item_palette_piece_type==sound_piece)&&(n==item_palette_piece_idx)),FALSE);
		}
	}

		// particles

	item_palette_add_item(particle_piece,-1,"Particles",NULL,(item_palette_piece_type==particle_piece),TRUE);

	if (item_palette_piece_open[particle_piece]) {
		for (n=0;n!=map.nparticle;n++) {
			item_palette_add_item(particle_piece,n,map.particles[n].name,NULL,((item_palette_piece_type==particle_piece)&&(n==item_palette_piece_idx)),FALSE);
		}
	}

		// sceneries

	item_palette_add_item(scenery_piece,-1,"Scenery",NULL,(item_palette_piece_type==scenery_piece),TRUE);

	if (item_palette_piece_open[scenery_piece]) {
		for (n=0;n!=map.nscenery;n++) {
			item_palette_add_item(scenery_piece,n,map.sceneries[n].model_name,NULL,((item_palette_piece_type==scenery_piece)&&(n==item_palette_piece_idx)),FALSE);
		}
	}

		// nodes

	item_palette_add_item(node_piece,-1,"Nodes",NULL,(item_palette_piece_type==node_piece),TRUE);

	if (item_palette_piece_open[node_piece]) {
		for (n=0;n!=map.nnode;n++) {
			if (map.nodes[n].name[0]!=0x0) item_palette_add_item(node_piece,n,map.nodes[n].name,NULL,((item_palette_piece_type==node_piece)&&(n==item_palette_piece_idx)),FALSE);
		}
	}

		// groups

	item_palette_add_item(group_piece,-1,"Groups",NULL,(item_palette_piece_type==group_piece),TRUE);

	if (item_palette_piece_open[group_piece]) {
		for (n=0;n!=map.group.ngroup;n++) {
			item_palette_add_item(group_piece,n,map.group.groups[n].name,NULL,((item_palette_piece_type==group_piece)&&(n==item_palette_piece_idx)),FALSE);
		}
	}

		// movements

	item_palette_add_item(movement_piece,-1,"Movements",NULL,(item_palette_piece_type==movement_piece),TRUE);

	if (item_palette_piece_open[movement_piece]) {
		for (n=0;n!=map.movement.nmovement;n++) {
			item_palette_add_item(movement_piece,n,map.movement.movements[n].name,NULL,((item_palette_piece_type==movement_piece)&&(n==item_palette_piece_idx)),FALSE);
		}
	}

		// cinemas

	item_palette_add_item(cinema_piece,-1,"Cinemas",NULL,(item_palette_piece_type==cinema_piece),TRUE);

	if (item_palette_piece_open[cinema_piece]) {
		for (n=0;n!=map.cinema.ncinema;n++) {
			item_palette_add_item(cinema_piece,n,map.cinema.cinemas[n].name,NULL,((item_palette_piece_type==cinema_piece)&&(n==item_palette_piece_idx)),FALSE);
		}
	}
}

/* =======================================================

      Item Palette Draw
      
======================================================= */

void item_palette_draw_tree_item(item_palette_item_type *item)
{
		// early exits

	if ((item->y<(item_palette_box.ty+item_scroll_button_high)) || ((item->y-item_font_high)>(item_palette_box.by-item_scroll_button_high))) return;

		// draw header
		
	if (item->header) {
	
		glBegin(GL_QUADS);
		glColor4f(0.9f,0.9f,0.9f,1.0f);
		glVertex2i(item_palette_box.lx,(item->y-item_font_high));
		glVertex2i(item_palette_box.rx,(item->y-item_font_high));
		glColor4f(0.8f,0.8f,0.8f,1.0f);
		glVertex2i(item_palette_box.rx,item->y);
		glVertex2i(item_palette_box.lx,item->y);
		glEnd();
		
		glColor4f(0.0f,0.0f,0.0f,1.0f);
		
		glBegin(GL_LINES);
		glVertex2i(item_palette_box.lx,(item->y-item_font_high));
		glVertex2i(item_palette_box.rx,(item->y-item_font_high));
		glVertex2i(item_palette_box.lx,(item->y+1));
		glVertex2i(item_palette_box.rx,(item->y+1));
		glEnd();
	
	}
		
		// draw selected item
	
	else {

		if (item->selected) {
			glColor4f(1.0f,1.0f,0.0f,1.0f);
			
			glBegin(GL_QUADS);
			glVertex2i(item_palette_box.lx,(item->y-item_font_high));
			glVertex2i(item_palette_box.rx,(item->y-item_font_high));
			glVertex2i(item_palette_box.rx,item->y);
			glVertex2i(item_palette_box.lx,item->y);
			glEnd();
		}
	}

		// draw text

	if (!item->is_col) {
		text_draw(item->x,(item->y-1),item_font_size,FALSE,item->name);
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
	int						n,lx,mx,rx,ty,by;
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

		// scroll up button

	ty=item_palette_box.ty;
	by=item_palette_box.ty+item_scroll_button_high;
	mx=(item_palette_box.lx+item_palette_box.rx)>>1;

	glColor4f(0.9f,0.9f,0.9f,1.0f);
		
	glBegin(GL_QUADS);
	glVertex2i(item_palette_box.lx,ty);
	glVertex2i(item_palette_box.rx,ty);
	glVertex2i(item_palette_box.rx,by);
	glVertex2i(item_palette_box.lx,by);
	glEnd();

	glDisable(GL_BLEND);

	glColor4f(0.0f,0.0f,0.0f,1.0f);
		
	glBegin(GL_LINE_LOOP);
	glVertex2i(item_palette_box.lx,ty);
	glVertex2i(item_palette_box.rx,ty);
	glVertex2i(item_palette_box.rx,by);
	glVertex2i(item_palette_box.lx,by);
	glEnd();

	glEnable(GL_BLEND);

	glBegin(GL_TRIANGLES);
	glColor4f(0.2f,0.2f,1.0f,1.0f);
	glVertex2i(mx,(ty+5));
	glColor4f(0.0f,0.0f,1.0f,1.0f);
	glVertex2i((mx+10),(by-5));
	glVertex2i((mx-10),(by-5));
	glEnd();

		// scroll down button

	ty=item_palette_box.by-item_scroll_button_high;
	by=item_palette_box.by;
	mx=(item_palette_box.lx+item_palette_box.rx)>>1;

	glColor4f(0.9f,0.9f,0.9f,1.0f);
		
	glBegin(GL_QUADS);
	glVertex2i(item_palette_box.lx,ty);
	glVertex2i(item_palette_box.rx,ty);
	glVertex2i(item_palette_box.rx,by);
	glVertex2i(item_palette_box.lx,by);
	glEnd();

	glDisable(GL_BLEND);

	glColor4f(0.0f,0.0f,0.0f,1.0f);
		
	glBegin(GL_LINE_LOOP);
	glVertex2i(item_palette_box.lx,ty);
	glVertex2i(item_palette_box.rx,ty);
	glVertex2i(item_palette_box.rx,by);
	glVertex2i(item_palette_box.lx,by);
	glEnd();

	glEnable(GL_BLEND);

	glBegin(GL_TRIANGLES);
	glColor4f(0.2f,0.2f,1.0f,1.0f);
	glVertex2i(mx,(by-5));
	glColor4f(0.0f,0.0f,1.0f,1.0f);
	glVertex2i((mx+10),(ty+5));
	glVertex2i((mx-10),(ty+5));
	glEnd();
	
		// click-close border
	
	lx=item_palette_box.lx;
	rx=item_palette_box.lx+item_palette_border_sz;
	mx=(lx+rx)>>1;
		
	glBegin(GL_QUADS);
	glColor4f(0.0f,0.0f,0.5f,1.0f);
	glVertex2i(lx,item_palette_box.by);
	glVertex2i(lx,item_palette_box.ty);
	glColor4f(0.0f,0.0f,1.0f,1.0f);
	glVertex2i(mx,item_palette_box.ty);
	glVertex2i(mx,item_palette_box.by);
	glColor4f(0.0f,0.0f,0.5f,1.0f);
	glVertex2i(rx,item_palette_box.by);
	glVertex2i(rx,item_palette_box.ty);
	glColor4f(0.0f,0.0f,1.0f,1.0f);
	glVertex2i(mx,item_palette_box.ty);
	glVertex2i(mx,item_palette_box.by);
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

	if (select_count()==0) {
		item_palette_piece_type=-1;
		item_palette_piece_idx=-1;
		return;
	}
	
	select_get(0,&sel_type,&main_idx,&sub_idx);
	if ((sel_type==mesh_piece) || (sel_type==liquid_piece)) {
		item_palette_piece_idx=-1;
		return;
	}

	item_palette_piece_type=sel_type;
	item_palette_piece_idx=main_idx;
}

void item_palette_select(int sel_type,int sel_idx)
{
	item_palette_piece_type=sel_type;
	item_palette_piece_idx=sel_idx;
	
	main_wind_draw();
}

/* =======================================================

      Item Palette Delete
      
======================================================= */

bool item_palette_delete(void)
{
		// anything to delete?

	if ((item_palette_piece_type==-1) || (item_palette_piece_idx==-1)) return(FALSE);

	switch (item_palette_piece_type) {

		case group_piece:
			if (os_dialog_confirm("Delete Group","Is it okay to delete this group?",FALSE)!=0) return(FALSE);
			map_group_delete(&map,item_palette_piece_idx);
			item_palette_reset();
			return(TRUE);

		case movement_piece:
			if (os_dialog_confirm("Delete Movement","Is it okay to delete this movement?",FALSE)!=0) return(FALSE);
			map_movement_delete(&map,item_palette_piece_idx);
			item_palette_reset();
			return(TRUE);

		case cinema_piece:
			if (os_dialog_confirm("Delete Cinema","Is it okay to delete this cinema?",FALSE)!=0) return(FALSE);
			map_cinema_delete(&map,item_palette_piece_idx);
			item_palette_reset();
			return(TRUE);

	}

	return(FALSE);
}

/* =======================================================

      Item Palette Scrolling
      
======================================================= */

void item_palette_scroll_up(void)
{
	if (item_palette_scroll_page>0) {
		item_palette_scroll_page--;
		main_wind_draw();
	}
}

void item_palette_scroll_down(void)
{
	if (item_palette_scroll_page<(item_palette_total_high/item_palette_page_high)) {
		item_palette_scroll_page++;
		main_wind_draw();
	}
}

void item_palette_scroll_wheel(d3pnt *pnt,int move)
{
	if (move>0) {
		item_palette_scroll_up();
		return;
	}

	if (move<0) {
		item_palette_scroll_down();
		return;
	}
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

		// click in scroll item

	if (pnt->y<=item_scroll_button_high) {
		item_palette_scroll_up();
		return;
	}

	if (pnt->y>=((item_palette_box.by-item_palette_box.ty)-item_scroll_button_high)) {
		item_palette_scroll_down();
		return;
	}

		// click in item

	item_idx=((pnt->y-item_scroll_button_high)+(item_palette_scroll_page*item_palette_page_high))/item_font_high;
	if ((item_idx<0) || (item_idx>=item_palette_item_count)) return;

		// clicking in header

	item=&item_palette_items[item_idx];

	if (item->piece_idx==-1) {
		item_palette_piece_open[item->piece_type]=!item_palette_piece_open[item->piece_type];
		main_wind_draw();
		return;
	}

		// select clicked item

	item_palette_piece_type=item->piece_type;
	item_palette_piece_idx=item->piece_idx;

	if (item->piece_idx!=-1) {
		select_clear();

		switch (item->piece_type) {
			case group_piece:
				select_add_group(item->piece_idx);
				break;
			case movement_piece:
				select_add_movement(item->piece_idx);
				break;
			case cinema_piece:
				select_add_cinema(item->piece_idx);
				break;
			default:
				select_add(item->piece_type,item->piece_idx,-1);
				break;
		}

		if (dblclick) view_goto_select();
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
	
		// reset the palette
		
	palette_reset();
	
		// just redraw if no double click

	if ((!dblclick) || (item_palette_piece_idx==-1)) {
		main_wind_draw();
		return;
	}
	
		// if double click, edit

	switch (item_palette_piece_type) {

		case cinema_piece:
			dialog_cinema_settings_run(item_palette_piece_idx);
			break;

		case group_piece:
			dialog_group_settings_run(item_palette_piece_idx);
			break;

		case movement_piece:
			dialog_movement_settings_run(item_palette_piece_idx);
			break;

	}
	
	main_wind_draw();
}