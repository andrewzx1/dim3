/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Item Palette

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
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

#define item_font_size			12.0f
#define item_font_high			14
#define item_palette_border_sz	10
#define item_palette_tree_sz	200

extern map_type					map;
extern editor_state_type		state;
extern editor_setup_type		setup;
extern file_path_setup_type		file_path_setup;

extern int						tool_palette_pixel_sz,txt_palette_pixel_sz;

int								item_palette_pixel_sz;
bool							item_palette_open;
d3rect							item_palette_box;

/* =======================================================

      Item Palette Setup
      
======================================================= */

void item_palette_initialize(void)
{
	item_palette_open=TRUE;
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

      Item Palette Tree
      
======================================================= */

bool item_palette_draw_tree_item(int x,int y,char *name)
{
	if (name[0]==0x0) return(FALSE);

	if ((y<item_palette_box.ty) || ((y-item_font_high)>item_palette_box.by)) return(TRUE);

	text_draw(x,y,item_font_size,FALSE,name);
	return(TRUE);
}

void item_palette_draw_tree(void)
{
	int			n,x,x2,y;

	x=item_palette_box.lx+(item_palette_border_sz+4);
	x2=x+10;
	y=item_palette_box.ty+item_font_high;

		// spots

	item_palette_draw_tree_item(x,y,"Spots");
	y+=item_font_high;

	for (n=0;n!=map.nspot;n++) {
		if (item_palette_draw_tree_item(x2,y,map.spots[n].name)) {
			y+=item_font_high;
		}
	}

		// sceneries

	item_palette_draw_tree_item(x,y,"Scenery");
	y+=item_font_high;

	for (n=0;n!=map.nscenery;n++) {
		if (item_palette_draw_tree_item(x2,y,map.sceneries[n].model_name)) {
			y+=item_font_high;
		}
	}

		// nodes

	item_palette_draw_tree_item(x,y,"Nodes");
	y+=item_font_high;

	for (n=0;n!=map.nnode;n++) {
		if (item_palette_draw_tree_item(x2,y,map.nodes[n].name)) {
			y+=item_font_high;
		}
	}
}

/* =======================================================

      Item Palette Draw
      
======================================================= */

void item_palette_draw(void)
{
	d3rect			wbox;

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

	item_palette_draw_tree();
	
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

      Item Palette Click
      
======================================================= */

void item_palette_click(d3pnt *pnt)
{
	pnt->x-=item_palette_box.lx;
	pnt->y-=item_palette_box.ty;

		// click in close border

	if (pnt->x<=item_palette_border_sz) {
		item_palette_open=!item_palette_open;
		item_palette_setup();

		main_wind_draw();
		return;
	}
}