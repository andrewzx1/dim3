/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Main Window Panel Setup

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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#include "interface.h"
#include "common_view.h"
#include "walk_view.h"

extern int						txt_palette_y,txt_palette_high;

extern map_type					map;
extern setup_type				setup;
extern editor_state_type		state;

/* =======================================================

      Forward Panels
      
======================================================= */

void main_wind_setup_panel_forward(editor_3D_view_setup *view_setup)
{
	d3rect				wbox;
	
	os_get_window_box(&wbox);

	view_setup->box.ty=wbox.ty+(toolbar_high+2);
	view_setup->box.by=(wbox.ty+toolbar_high)+(((wbox.by-(txt_palette_high+info_high))-(wbox.ty+toolbar_high))/2);
	view_setup->box.lx=wbox.lx+2;
	view_setup->box.rx=(wbox.rx-palette_wid)-(((wbox.rx-palette_wid)-wbox.lx)/2);
	
	view_setup->ang.x=0.0f;
	view_setup->ang.y=state.swap_panel_forward?180.0f:0.0f;
	view_setup->ang.z=0.0f;
}

void main_wind_setup_panel_forward_frame(editor_3D_view_setup *view_setup)
{
	d3rect				wbox;
	
	os_get_window_box(&wbox);

	view_setup->box.ty=wbox.ty+(toolbar_high+2);
	view_setup->box.by=(wbox.ty+toolbar_high)+(((wbox.by-(txt_palette_high+info_high))-(wbox.ty+toolbar_high))/2);
	view_setup->box.lx=wbox.lx+2;
	view_setup->box.rx=(wbox.rx-palette_wid)-(((wbox.rx-palette_wid)-wbox.lx)/2);

	view_setup->ang.x=0.0f;
	view_setup->ang.y=state.swap_panel_forward?180.0f:0.0f;
	view_setup->ang.z=0.0f;
}

void main_wind_setup_panel_forward_full(editor_3D_view_setup *view_setup)
{
	d3rect				wbox;
	
	os_get_window_box(&wbox);

	view_setup->box.ty=wbox.ty+toolbar_high;
	view_setup->box.by=wbox.by-(txt_palette_high+info_high);
	view_setup->box.lx=wbox.lx;
	view_setup->box.rx=wbox.rx-palette_wid;
}

/* =======================================================

      Side Panels
      
======================================================= */

void main_wind_setup_panel_side(editor_3D_view_setup *view_setup)
{
	d3rect				wbox;
	
	os_get_window_box(&wbox);

	view_setup->box.ty=wbox.ty+(toolbar_high+2);
	view_setup->box.by=(wbox.ty+toolbar_high)+(((wbox.by-(txt_palette_high+info_high))-(wbox.ty+toolbar_high))/2);
	view_setup->box.lx=((wbox.rx-palette_wid)-(((wbox.rx-palette_wid)-wbox.lx)/2))+2;
	view_setup->box.rx=wbox.rx-(palette_wid+2);

	view_setup->ang.x=0.0f;
	view_setup->ang.y=state.swap_panel_side?90.0f:270.0f;
	view_setup->ang.z=0.0f;
}

void main_wind_setup_panel_side_frame(editor_3D_view_setup *view_setup)
{
	d3rect				wbox;
	
	os_get_window_box(&wbox);

	view_setup->box.ty=wbox.ty+(toolbar_high+2);
	view_setup->box.by=(wbox.ty+toolbar_high)+(((wbox.by-(txt_palette_high+info_high))-(wbox.ty+toolbar_high))/2);
	view_setup->box.lx=((wbox.rx-palette_wid)-(((wbox.rx-palette_wid)-wbox.lx)/2))+2;
	view_setup->box.rx=wbox.rx-(palette_wid+2);

	view_setup->ang.x=0.0f;
	view_setup->ang.y=state.swap_panel_side?90.0f:270.0f;
	view_setup->ang.z=0.0f;
	
	view_setup->fov=walk_view_side_fov;
}

/* =======================================================

      Top Panels
      
======================================================= */

void main_wind_setup_panel_top(editor_3D_view_setup *view_setup)
{
	int					mag_sz;
	d3rect				wbox;
	
	os_get_window_box(&wbox);

	view_setup->box.ty=(wbox.ty+toolbar_high)+((((wbox.by-(txt_palette_high+info_high))-(wbox.ty+toolbar_high))/2)+2);
	view_setup->box.by=wbox.by-((txt_palette_high+info_high)+2);
	view_setup->box.lx=wbox.lx+2;
	view_setup->box.rx=wbox.rx-(palette_wid+2);
	
	mag_sz=(magnify_factor_max-state.magnify_factor)*(map_enlarge<<1);
	
	view_setup->ang.x=state.swap_panel_top?(90.0f):(-90.0f);
	view_setup->ang.y=0.0f;
	view_setup->ang.z=0.0f;
	
	view_setup->fov=top_view_fov;
}

void main_wind_setup_panel_top_frame(editor_3D_view_setup *view_setup)
{
	d3rect				wbox;
	
	os_get_window_box(&wbox);

	view_setup->box.ty=(wbox.ty+toolbar_high)+((((wbox.by-(txt_palette_high+info_high))-(wbox.ty+toolbar_high))/2)+2);
	view_setup->box.by=wbox.by-((txt_palette_high+info_high)+2);
	view_setup->box.lx=wbox.lx+2;
	view_setup->box.rx=(wbox.rx-palette_wid)-(((wbox.rx-palette_wid)-wbox.lx)/2);
	
	view_setup->ang.x=-90.0f;
	view_setup->ang.y=0.0f;
	view_setup->ang.z=0.0f;
	
	view_setup->fov=top_view_fov;
}

void main_wind_setup_panel_top_full(editor_3D_view_setup *view_setup)
{
	int					mag_sz;
	
	os_get_window_box(&view_setup->box);
	
	view_setup->box.ty+=toolbar_high;
	view_setup->box.by-=(txt_palette_high+info_high);
	view_setup->box.rx-=palette_wid;

	mag_sz=(magnify_factor_max-state.magnify_factor)*(map_enlarge<<1);

	view_setup->ang.x=-90.0f;
	view_setup->ang.y=0.0f;
	view_setup->ang.z=0.0f;
	
	view_setup->fov=top_view_fov;
}

/* =======================================================

      Walk Panels
      
======================================================= */

void main_wind_setup_panel_walk(editor_3D_view_setup *view_setup)
{
	d3rect				wbox;

	os_get_window_box(&wbox);

	view_setup->box.ty=(wbox.ty+toolbar_high)+((((wbox.by-(txt_palette_high+info_high))-(wbox.ty+toolbar_high))/2)+2);
	view_setup->box.by=wbox.by-((txt_palette_high+info_high)+2);
	view_setup->box.lx=((wbox.rx-palette_wid)-(((wbox.rx-palette_wid)-wbox.lx)/2))+2;
	view_setup->box.rx=wbox.rx-(palette_wid+2);
	
	view_setup->fov=walk_view_forward_fov;
}

