/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Checking For Item In View Bounds

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

#include "interface.h"
#include "objects.h"

extern map_type				map;
extern camera_type			camera;
extern view_type			view;
extern server_type			server;
extern iface_type			iface;
extern setup_type			setup;

/* =======================================================

      Bounding Boxes in View
      
======================================================= */

bool view_cull_check_complex_boundbox(int *cbx,int *cby,int *cbz)
{
	int			n,pt_count,
				px[14],py[14],pz[14];
	bool		above_z,behind_z,lft,rgt,top,bot;
	
		// check if points are behind z
		
	above_z=FALSE;
	behind_z=FALSE;
	
	for (n=0;n!=8;n++) {
		if (gl_project_in_view_z(cbx[n],cby[n],cbz[n])) {
			above_z=TRUE;
		}
		else {
			behind_z=TRUE;
		}
	}
	
	if (!above_z) return(FALSE);
	
		// create additional points to
		// deal with meshes that are split
		// behind and above the z
		
	pt_count=8;
	
	memmove(px,cbx,(sizeof(int)*8));
	memmove(py,cby,(sizeof(int)*8));
	memmove(pz,cbz,(sizeof(int)*8));
	
	if ((above_z) && (behind_z)) {
		px[8]=(px[0]+px[1]+px[2]+px[3])>>2;
		py[8]=(py[0]+py[1]+py[2]+py[3])>>2;
		pz[8]=(pz[0]+pz[1]+pz[2]+pz[3])>>2;

		px[9]=(px[0]+px[1]+px[5]+px[4])>>2;
		py[9]=(py[0]+py[1]+py[5]+py[4])>>2;
		pz[9]=(pz[0]+pz[1]+pz[5]+pz[4])>>2;

		px[10]=(px[1]+px[2]+px[6]+px[5])>>2;
		py[10]=(py[1]+py[2]+py[6]+py[5])>>2;
		pz[10]=(pz[1]+pz[2]+pz[6]+pz[5])>>2;

		px[11]=(px[3]+px[2]+px[6]+px[7])>>2;
		py[11]=(py[3]+py[2]+py[6]+py[7])>>2;
		pz[11]=(pz[3]+pz[2]+pz[6]+pz[7])>>2;

		px[12]=(px[0]+px[3]+px[7]+px[4])>>2;
		py[12]=(py[0]+py[3]+py[7]+py[4])>>2;
		pz[12]=(pz[0]+pz[3]+pz[7]+pz[4])>>2;

		px[13]=(px[4]+px[5]+px[6]+px[7])>>2;
		py[13]=(py[4]+py[5]+py[6]+py[7])>>2;
		pz[13]=(pz[4]+pz[5]+pz[6]+pz[7])>>2;
		
		pt_count=14;
	}

		// project to screen

	gl_project_poly(pt_count,px,py,pz);
	
		// are points grouped completely
		// off one side of the screen?
		
	lft=rgt=top=bot=TRUE;

	for (n=0;n!=pt_count;n++) {
		lft=lft&&(px[n]<0);
		rgt=rgt&&(px[n]>=setup.screen.x_sz);
		top=top&&(py[n]<0);
		bot=bot&&(py[n]>=setup.screen.y_sz);
	}

	return(!(lft||rgt||top||bot));
}

bool view_cull_check_boundbox(int x,int z,int ex,int ez,int ty,int by)
{
	int				px[8],py[8],pz[8];
	
	px[0]=px[3]=px[4]=px[7]=x;
	px[1]=px[2]=px[5]=px[6]=ex;

	pz[0]=pz[1]=pz[4]=pz[5]=z;
	pz[2]=pz[3]=pz[6]=pz[7]=ez;

	py[0]=py[1]=py[2]=py[3]=ty;
	py[4]=py[5]=py[6]=py[7]=by;
	
	return(view_cull_check_complex_boundbox(px,py,pz));
}

bool view_cull_check_boundbox_2(d3pnt *min,d3pnt *max)
{
	int				px[8],py[8],pz[8];
	
	px[0]=px[3]=px[4]=px[7]=min->x;
	px[1]=px[2]=px[5]=px[6]=max->x;

	pz[0]=pz[1]=pz[4]=pz[5]=min->z;
	pz[2]=pz[3]=pz[6]=pz[7]=max->z;

	py[0]=py[1]=py[2]=py[3]=min->y;
	py[4]=py[5]=py[6]=py[7]=max->y;
	
	return(view_cull_check_complex_boundbox(px,py,pz));
}

/* =======================================================

      Distances to View Center
      
======================================================= */

double view_cull_distance_to_view_center(int x,int y,int z)
{
	double			dx,dz,dy;
	
	dx=(double)(x-view.render->camera.pnt.x);
	dy=(double)(y-view.render->camera.pnt.y);
	dz=(double)(z-view.render->camera.pnt.z);
	return(sqrt((dx*dx)+(dy*dy)+(dz*dz)));
}

/* =======================================================

      Pieces in View
      
======================================================= */

bool view_cull_mesh(map_mesh_type *mesh)
{
	double			obscure_dist;

		// check obscure distance

	if (!fog_solid_on()) {
		obscure_dist=(double)(camera.setup.plane.far_z-camera.setup.plane.near_z);
	}
	else {
		obscure_dist=(double)((map.fog.outer_radius>>1)*3);
	}

	if (map_mesh_calculate_distance(mesh,&view.render->camera.pnt)>obscure_dist) return(FALSE);

		// check bounding box

	return(view_cull_check_boundbox_2(&mesh->box.min,&mesh->box.max));
}

bool view_cull_liquid(map_liquid_type *liq)
{
	double			obscure_dist;

		// check obscure distance

	if (!fog_solid_on()) {
		obscure_dist=(double)(camera.setup.plane.far_z-camera.setup.plane.near_z);
	}
	else {
		obscure_dist=(double)((map.fog.outer_radius>>1)*3);
	}
		
	if (map_liquid_calculate_distance(liq,&view.render->camera.pnt)>obscure_dist) return(FALSE);

		// check bounding box

	return(view_cull_check_boundbox(liq->lft,liq->top,liq->rgt,liq->bot,liq->y,liq->y));
}

bool view_cull_model(model_draw *draw)
{
	int				px[8],py[8],pz[8];
	double			obscure_dist;

		// no model

	if ((draw->model_idx==-1) || (!draw->on)) return(FALSE);

		// check obscure distance

	if (!fog_solid_on()) {
		obscure_dist=(double)(camera.setup.plane.far_z-camera.setup.plane.near_z);
	}
	else {
		obscure_dist=(double)((map.fog.outer_radius>>1)*3);
	}

	if (map.optimize.obscure_dist.model!=0) {
		if (map.optimize.obscure_dist.model<obscure_dist) obscure_dist=map.optimize.obscure_dist.model;
	}
		
	draw->draw_dist=view_cull_distance_to_view_center(draw->pnt.x,draw->pnt.y,draw->pnt.z);
	if (draw->draw_dist>=obscure_dist) return(FALSE);

		// check bounding box
	
	model_get_view_complex_bounding_box(server.model_list.models[draw->model_idx],&draw->pnt,&draw->setup.ang,px,py,pz);
	return(view_cull_check_complex_boundbox(px,py,pz));
}

bool view_model_shadow(model_draw *draw)
{
	int				light_intensity;
	double			dist,obscure_dist;
	d3pnt			min,max,light_pnt;

		// no model to draw

	if ((draw->model_idx==-1) || (!draw->on)) return(FALSE);

		// check obscure distance

	if (!fog_solid_on()) {
		obscure_dist=(double)(camera.setup.plane.far_z-camera.setup.plane.near_z);
	}
	else {
		obscure_dist=(double)((map.fog.outer_radius>>1)*3);
	}

	if (map.optimize.obscure_dist.shadow!=0) {
		if (map.optimize.obscure_dist.shadow<obscure_dist) obscure_dist=map.optimize.obscure_dist.shadow;
	}
		
	dist=view_cull_distance_to_view_center(draw->pnt.x,draw->pnt.y,draw->pnt.z);
	if (dist>=obscure_dist) return(FALSE);
	
		// get shadow volume

	model_get_view_complex_bounding_volume(server.model_list.models[draw->model_idx],&draw->pnt,&draw->setup.ang,&min,&max);
	shadow_get_light_point(&draw->pnt,draw->size.y,&light_pnt,&light_intensity);
	shadow_get_bound_box(&draw->pnt,draw->size.y,&light_pnt,light_intensity,&min,&max);

	return(view_cull_check_boundbox_2(&min,&max));
}

bool view_cull_effect(effect_type *effect,d3pnt *center_pnt)
{
	double				obscure_dist;
	d3pnt				min,max;

		// check obscure distance

	if (!fog_solid_on()) {
		obscure_dist=(double)(camera.setup.plane.far_z-camera.setup.plane.near_z);
	}
	else {
		obscure_dist=(double)((map.fog.outer_radius>>1)*3);
	}

	if (map.optimize.obscure_dist.effect!=0) {
		if (map.optimize.obscure_dist.effect<obscure_dist) obscure_dist=map.optimize.obscure_dist.effect;
	}
		
	if (view_cull_distance_to_view_center(effect->pnt.x,effect->pnt.y,effect->pnt.z)>obscure_dist) return(FALSE);
	
		// get box

	effect_draw_get_bound_box(effect,&min,&max);
	
		// remember center position for
		// distance calcs
		
	center_pnt->x=(min.x+max.x)>>1;
	center_pnt->y=(min.y+max.y)>>1;
	center_pnt->z=(min.z+max.z)>>1;
	
		// check bounds
		
	return(view_cull_check_boundbox_2(&min,&max));
}

bool view_cull_halo(d3pnt *pnt)
{
	double				obscure_dist;

		// check obscure distance

	if (!fog_solid_on()) {
		obscure_dist=(double)(camera.setup.plane.far_z-camera.setup.plane.near_z);
	}
	else {
		obscure_dist=(double)((map.fog.outer_radius>>1)*3);
	}

	if (map.optimize.obscure_dist.effect!=0) {
		if (map.optimize.obscure_dist.effect<obscure_dist) obscure_dist=map.optimize.obscure_dist.effect;
	}
		
	return(view_cull_distance_to_view_center(pnt->x,pnt->y,pnt->z)<obscure_dist);
}

