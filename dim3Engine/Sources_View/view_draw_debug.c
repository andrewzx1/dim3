/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View Debug Drawing

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit �Created with dim3 Technology� is given on a single
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

#include "interface.h"
#include "objects.h"

extern map_type				map;
extern camera_type			camera;
extern view_type			view;
extern server_type			server;
extern iface_type			iface;
extern setup_type			setup;
extern render_info_type		render_info;

/* =======================================================

      Debug Model Drawing
      
======================================================= */

void view_draw_debug_bounding_box(d3pnt *pnt,d3ang *ang,d3pnt *size)
{
	int				n,xsz,zsz,px[8],py[8],pz[8];
	float			fx,fy,fz;
	matrix_type		rot_x_mat,rot_y_mat,rot_z_mat;
	d3col			col;

		// bounding box

	xsz=size->x>>1;
	zsz=size->z>>1;

	px[0]=px[1]=px[4]=px[5]=-xsz;
	px[2]=px[3]=px[6]=px[7]=xsz;

	py[0]=py[1]=py[2]=py[3]=-size->y;
	py[4]=py[5]=py[6]=py[7]=0;

	pz[1]=pz[2]=pz[5]=pz[6]=-zsz;
	pz[0]=pz[3]=pz[4]=pz[7]=zsz;

	matrix_rotate_x(&rot_x_mat,ang->x);
	matrix_rotate_z(&rot_z_mat,ang->z);
	matrix_rotate_y(&rot_y_mat,ang->y);

	for (n=0;n!=8;n++) {
		fx=(float)px[n];
		fy=(float)py[n];
		fz=(float)pz[n];
		
		matrix_vertex_multiply(&rot_x_mat,&fx,&fy,&fz);
		matrix_vertex_multiply(&rot_z_mat,&fx,&fy,&fz);
		matrix_vertex_multiply(&rot_y_mat,&fx,&fy,&fz);
		
		px[n]=(int)fx;
		py[n]=(int)fy;
		pz[n]=(int)fz;
	}
	
	for (n=0;n!=8;n++) {
		px[n]=px[n]+pnt->x;
		py[n]=py[n]+pnt->y;
		pz[n]=pz[n]+pnt->z;
	}

		// draw box

	col.r=col.g=0.2f;
	col.b=1.0f;

	glLineWidth(2.0f);
	view_draw_next_vertex_object_3D_line_cube(&col,1.0f,px,py,pz);
	glLineWidth(1.0f);
}

void view_draw_debug_info(char *name,char *info,d3pnt *pnt,d3pnt *size)
{
	int						x,y,z,dist,font_size;
	d3col					col;
	d3pnt					spt,ept,hpt;
	ray_trace_contact_type	contact;

		// get size and fade

	x=pnt->x;
	y=pnt->y-(size->y>>1);
	z=pnt->z;

	dist=distance_get(x,y,z,view.render->camera.pnt.x,view.render->camera.pnt.y,view.render->camera.pnt.z);
		
	font_size=iface.font.text_size_medium-(int)((float)(iface.font.text_size_medium*dist)/(float)(remote_name_max_distance-remote_name_min_distance));
	if (font_size<10) font_size=10;

		// ray trace check

	spt.x=x;
	spt.y=y;
	spt.z=z;

	ept.x=view.render->camera.pnt.x;
	ept.y=view.render->camera.pnt.y;
	ept.z=view.render->camera.pnt.z;

	contact.obj.on=FALSE;
	contact.proj.on=FALSE;

	contact.origin=poly_ray_trace_origin_object;

	if (ray_trace_map_by_point(&spt,&ept,&hpt,&contact)) return;

		// project the mid point

	gl_project_point(&x,&y,&z);

		// draw the info

	gl_2D_view_interface();
	
	glDisable(GL_DEPTH_TEST);
	
		// covert to interface resolution

	x=(x*iface.scale_x)/setup.screen.x_sz;
	y=((setup.screen.y_sz-y)*iface.scale_y)/setup.screen.y_sz;

		// draw text

	col.r=col.b=1.0f;
	col.g=0.0f;
	
	gl_text_start(font_hud_index,font_size);
	gl_text_draw(x,y,name,tx_center,FALSE,&col,1.0f);
	if (info!=NULL) {
		if (info[0]!=0x0) {
			y+=font_size;
			gl_text_draw(x,y,info,tx_center,FALSE,&col,1.0f);
		}
	}
	gl_text_end();

	glEnable(GL_DEPTH_TEST);

		// restore original projection

	gl_3D_view();
	gl_3D_rotate(&view.render->camera.pnt,&view.render->camera.ang);
	gl_setup_project();
}

/* =======================================================

      Draw Debug Collisions and Paths
      
======================================================= */

void view_draw_debug_object_path(obj_type *obj)
{
	int				yadd;
	d3pnt			pnt;
	d3col			col;

	if (!object_auto_walk_get_seek_position(obj,&pnt)) return;

	col.r=col.b=0.0f;
	col.g=1.0f;

	yadd=obj->size.y>>1;

	glLineWidth(2.0f);
	view_draw_next_vertex_object_3D_line(&col,1.0f,obj->pnt.x,(obj->pnt.y-yadd),obj->pnt.z,pnt.x,(pnt.y-yadd),pnt.z);
	glLineWidth(1.0f);
}

/* =======================================================

      Draw Debug Specific Type
      
======================================================= */

void view_draw_debug_object(obj_type *obj)
{
	d3pnt			size;

	size.x=obj->size.x;
	size.y=obj->size.y;
	size.z=obj->size.z;

	if (obj->duck.mode!=dm_stand) size.y-=obj->duck.y_move;
	if (obj->liquid.mode==lm_float) size.y+=obj->liquid.bob_y_move;

	view_draw_debug_bounding_box(&obj->draw.pnt,&obj->draw.setup.ang,&size);
	view_draw_debug_object_path(obj);
	view_draw_debug_info(obj->name,obj->debug.str,&obj->draw.pnt,&size);
}

void view_draw_debug_projectile(proj_type *proj)
{
	d3pnt			size;
	proj_setup_type	*proj_setup;

	size.x=proj->size.x;
	size.y=proj->size.y;
	size.z=proj->size.z;

	view_draw_debug_bounding_box(&proj->draw.pnt,&proj->draw.setup.ang,&size);

	proj_setup=server.obj_list.objs[proj->obj_idx]->weap_list.weaps[proj->weap_idx]->proj_setup_list.proj_setups[proj->proj_setup_idx];
	view_draw_debug_info(proj_setup->name,NULL,&proj->draw.pnt,&size);
}

