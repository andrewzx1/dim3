/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Shadow Drawing

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
	#include "dim3engine.h"
#endif

#include "interface.h"

extern map_type			map;
extern server_type		server;
extern view_type		view;
extern setup_type		setup;

d3pnt					*shadow_spt,*shadow_hpt;
d3vct					*shadow_vct;
poly_pointer_type		*shadow_poly_ptrs;

/* =======================================================

      Initialize and Shutdown Shadows
      
======================================================= */

bool shadow_initialize(void)
{
		// memory for ray tracing lists
		
	shadow_spt=(d3pnt*)malloc(sizeof(d3pnt)*view_shadows_vertex_count);
	if (shadow_spt==NULL) return(FALSE);

	shadow_hpt=(d3pnt*)malloc(sizeof(d3pnt)*view_shadows_vertex_count);
	if (shadow_hpt==NULL) return(FALSE);
	
	shadow_vct=(d3vct*)malloc(sizeof(d3vct)*view_shadows_vertex_count);
	if (shadow_vct==NULL) return(FALSE);
	
		// memory for poly ptrs
	
	shadow_poly_ptrs=(poly_pointer_type*)malloc(view_shadows_poly_count*sizeof(poly_pointer_type));
	if (shadow_poly_ptrs==NULL) return(FALSE);

	return(TRUE);
}

void shadow_shutdown(void)
{
	free(shadow_poly_ptrs);

	free(shadow_spt);
	free(shadow_vct);
	free(shadow_hpt);
}

/* =======================================================

      Shadow Light
      
======================================================= */

void shadow_get_light_point(d3pnt *pnt,int high,d3pnt *light_pnt,int *light_intensity)
{
	view_light_spot_type			*lspot;

		// get closest light

	lspot=gl_light_find_closest_light(pnt->x,pnt->y,pnt->z);
	if (lspot!=NULL) {
		memmove(light_pnt,&lspot->pnt,sizeof(d3pnt));
		*light_intensity=lspot->i_intensity;
		return;
	}

		// if no light, get light directly above

	memmove(light_pnt,pnt,sizeof(d3pnt));
	light_pnt->y-=15000;
	*light_intensity=15000+high;
}

/* =======================================================

      Build Shadow Draw Volume
      
======================================================= */

bool shadow_get_volume(d3pnt *pnt,int high,d3pnt *light_pnt,int light_intensity,d3pnt *min,d3pnt *max)
{
	int							n;
	float						f_dist;
	bool						hits[8];
	d3pnt						spt[8],ept[8],hpt[8],*sp,*ep;
	d3vct						ray_move;
	ray_trace_contact_type		base_contact;

	spt[0].x=spt[1].x=spt[4].x=spt[5].x=min->x;
	spt[2].x=spt[3].x=spt[6].x=spt[7].x=max->x;
	
	spt[0].y=spt[1].y=spt[2].y=spt[3].y=min->y;
	spt[4].y=spt[5].y=spt[6].y=spt[7].y=max->y;

	spt[1].z=spt[2].z=spt[5].z=spt[6].z=min->z;
	spt[0].z=spt[3].z=spt[4].z=spt[7].z=max->z;

		// ray trace bounding box
		
	sp=spt;
	ep=ept;
	
	f_dist=(float)light_intensity;

	for (n=0;n!=8;n++) {
		vector_create(&ray_move,light_pnt->x,light_pnt->y,light_pnt->z,sp->x,sp->y,sp->z);
				
		ep->x=light_pnt->x-(int)(ray_move.x*f_dist);
		ep->y=light_pnt->y-(int)(ray_move.y*f_dist);
		ep->z=light_pnt->z-(int)(ray_move.z*f_dist);
		
		sp++;
		ep++;
	}

	base_contact.obj.on=FALSE;
	base_contact.obj.ignore_idx=-1;

	base_contact.proj.on=FALSE;
	base_contact.proj.ignore_idx=-1;

	base_contact.origin=poly_ray_trace_origin_object;

	ray_trace_map_by_point_array_no_contact(8,spt,ept,hpt,hits,&base_contact);
	
		// if the light intensity is high, then
		// we need to clip no hits back to the original
		// point to stop strong, low light sources
		// from projecting infinitely
		
	if (light_intensity>=view_shadows_clip_infinite_distance) {
	
		for (n=0;n!=8;n++) {
			if (!hits[n]) {
				hpt[n].x=spt[n].x;
				hpt[n].y=spt[n].y;
				hpt[n].z=spt[n].z;
			}
		}
	}
	
		// set the volume

	min->x=max->x=hpt[0].x;
	min->y=max->y=hpt[0].y;
	min->z=max->z=hpt[0].z;
	
	for (n=1;n!=8;n++) {
		if (hpt[n].x<min->x) min->x=hpt[n].x;
		if (hpt[n].x>max->x) max->x=hpt[n].x;
		if (hpt[n].y<min->y) min->y=hpt[n].y;
		if (hpt[n].y>max->y) max->y=hpt[n].y;
		if (hpt[n].z<min->z) min->z=hpt[n].z;
		if (hpt[n].z>max->z) max->z=hpt[n].z;
	}

	return(TRUE);
}

int shadow_build_poly_cross_volume_set(d3pnt *light_pnt,d3pnt *volume_min,d3pnt *volume_max,d3pnt *min,d3pnt *max,int skip_mesh_idx)
{
	int					n,k,cnt;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	
		// force box to be constrained
		// against item and make sure it's not
		// too large

	if (((volume_min->x+volume_max->x)>>1)<light_pnt->x) {
		max->x=volume_max->x;
	}
	else {
		min->x=volume_min->x;
	}
	
	if (((volume_min->y+volume_max->y)>>1)<light_pnt->y) {
		max->y=volume_max->y;
	}
	else {
		min->y=volume_min->y;
	}
	
	if (((volume_min->z+volume_max->z)>>1)<light_pnt->z) {
		max->z=volume_max->z;
	}
	else {
		min->z=volume_min->z;
	}

		// testing -- shadow box
	/*
	glColor4f(0.0f,1.0f,0.0f,1.0f);
	glLineWidth(0.0f);
	glBegin(GL_LINE_LOOP);
	glVertex3i(min->x,min->y,min->z);
	glVertex3i(max->x,min->y,min->z);
	glVertex3i(max->x,min->y,max->z);
	glVertex3i(min->x,min->y,max->z);
	glEnd();
	glBegin(GL_LINE_LOOP);
	glVertex3i(min->x,max->y,min->z);
	glVertex3i(max->x,max->y,min->z);
	glVertex3i(max->x,max->y,max->z);
	glVertex3i(min->x,max->y,max->z);
	glEnd();
	glBegin(GL_LINES);
	glVertex3i(min->x,min->y,min->z);
	glVertex3i(min->x,max->y,min->z);
	glVertex3i(max->x,min->y,min->z);
	glVertex3i(max->x,max->y,min->z);
	glVertex3i(max->x,min->y,max->z);
	glVertex3i(max->x,max->y,max->z);
	glVertex3i(min->x,min->y,max->z);
	glVertex3i(min->x,max->y,max->z);
	glEnd();
	glLineWidth(1.0f);
	*/
	
		// check meshes
		
	cnt=0;
	
	for (n=0;n!=view.render->draw_list.count;n++) {
	
		if (view.render->draw_list.items[n].type!=view_render_type_mesh) continue;
		if (view.render->draw_list.items[n].idx==skip_mesh_idx) continue;

			// find mesh colliding with shadow

		mesh=&map.mesh.meshes[view.render->draw_list.items[n].idx];

		if ((mesh->box.max.x<min->x) || (mesh->box.min.x>max->x)) continue;
		if ((mesh->box.max.z<min->z) || (mesh->box.min.z>max->z)) continue;
		if ((mesh->box.max.y<min->y) || (mesh->box.min.y>max->y)) continue;		// check Y last as X/Z are usually better eliminations
		
			// find polys colliding with shadow
			
		for (k=0;k!=mesh->npoly;k++) {
			poly=&mesh->polys[k];
			
				// skip all transparent polys
				
			if (poly->draw.transparent_on) continue;

				// check if within shadow volume
				
			if ((poly->box.max.x<min->x) || (poly->box.min.x>max->x)) continue;
			if ((poly->box.max.z<min->z) || (poly->box.min.z>max->z)) continue;
			if ((poly->box.max.y<min->y) || (poly->box.min.y>max->y)) continue;		// check Y last as X/Z are usually better eliminations
			
				// use normals to cull
				
			if (((poly->tangent_space.normal.x*(float)(poly->box.mid.x-light_pnt->x))+(poly->tangent_space.normal.y*(float)(poly->box.mid.y-light_pnt->y))+(poly->tangent_space.normal.z*(float)(poly->box.mid.z-light_pnt->z)))>map.optimize.cull_angle) continue;

				// add to shadow list
				
			shadow_poly_ptrs[cnt].mesh_idx=view.render->draw_list.items[n].idx;
			shadow_poly_ptrs[cnt].poly_idx=k;
			cnt++;
			
				// can only shadow for max stencil count
				
			if (cnt==view_shadows_poly_count) return(cnt);
		}
	}
	
	return(cnt);
}

int shadow_build_poly_set_model(model_type *mdl,model_draw *draw,d3pnt *light_pnt,int light_intensity)
{
	d3pnt			volume_min,volume_max,min,max;
	
	model_get_view_complex_bounding_volume(mdl,&draw->pnt,&draw->setup.ang,&volume_min,&volume_max);
	memmove(&min,&volume_min,sizeof(d3pnt));
	memmove(&max,&volume_max,sizeof(d3pnt));

	if (!shadow_get_volume(&draw->pnt,draw->size.y,light_pnt,light_intensity,&min,&max)) return(0);
	return(shadow_build_poly_cross_volume_set(light_pnt,&volume_min,&volume_max,&min,&max,-1));	
}

int shadow_build_poly_set_mesh(int mesh_idx,d3pnt *light_pnt,int light_intensity)
{
	d3pnt			min,max;
	map_mesh_type	*mesh;
	
		// get the shadow volume
		
	mesh=&map.mesh.meshes[mesh_idx];
	memmove(&min,&mesh->box.min,sizeof(d3pnt));
	memmove(&max,&mesh->box.max,sizeof(d3pnt));

	if (!shadow_get_volume(&mesh->box.mid,(mesh->box.max.y-mesh->box.min.y),light_pnt,light_intensity,&min,&max)) return(0);
	return(shadow_build_poly_cross_volume_set(light_pnt,&mesh->box.min,&mesh->box.max,&min,&max,mesh_idx));	
}

/* =======================================================

      Render Model Bounds Eliminations
      
======================================================= */

void shadow_render_prepare_bounds_check(poly_pointer_type *poly_ptr,d3pnt *min,d3pnt *max)
{
	map_mesh_poly_type		*poly;

		// we need to add slop to the bounds check because
		// there's the problem of two 2D planes laying on top of
		// each other
		
	poly=&map.mesh.meshes[poly_ptr->mesh_idx].polys[poly_ptr->poly_idx];

	min->x=poly->box.min.x-view_shadows_bounds_check_slop;
	max->x=poly->box.max.x+view_shadows_bounds_check_slop;
	min->y=poly->box.min.y-view_shadows_bounds_check_slop;
	max->y=poly->box.max.y+view_shadows_bounds_check_slop;
	min->z=poly->box.min.z-view_shadows_bounds_check_slop;
	max->z=poly->box.max.z+view_shadows_bounds_check_slop;
}

bool shadow_render_model_trig_bounds_check(d3pnt *min,d3pnt *max,int v_idx0,int v_idx1,int v_idx2)
{
	d3pnt			*pt0,*pt1,*pt2;

	pt0=&shadow_hpt[v_idx0];
	pt1=&shadow_hpt[v_idx1];
	pt2=&shadow_hpt[v_idx2];

	if ((pt0->x<min->x) && (pt1->x<min->x) && (pt2->x<min->x)) return(FALSE);
	if ((pt0->x>max->x) && (pt1->x>max->x) && (pt2->x>max->x)) return(FALSE);
	if ((pt0->z<min->z) && (pt1->z<min->z) && (pt2->z<min->z)) return(FALSE);
	if ((pt0->z>max->z) && (pt1->z>max->z) && (pt2->z>max->z)) return(FALSE);
	if ((pt0->y<min->y) && (pt1->y<min->y) && (pt2->y<min->y)) return(FALSE);
	if ((pt0->y>max->y) && (pt1->y>max->y) && (pt2->y>max->y)) return(FALSE);

	return(TRUE);
}

bool shadow_render_mesh_poly_bounds_check(d3pnt *min,d3pnt *max,map_mesh_poly_type *poly)
{
	int				n;
	bool			min_x,max_x,min_y,max_y,min_z,max_z;
	d3pnt			*pt;

	min_x=max_x=TRUE;
	min_y=max_y=TRUE;
	min_z=max_z=TRUE;

	for (n=0;n!=poly->ptsz;n++) {
		pt=&shadow_hpt[poly->v[n]];

		min_x=min_x||(pt->x<min->x);
		max_x=max_x||(pt->x>max->x);
		min_y=min_y||(pt->y<min->y);
		max_y=max_y||(pt->y>max->y);
		min_z=min_z||(pt->z<min->z);
		max_z=max_z||(pt->z>max->z);
	}

	return(min_x||max_x||min_y||max_y||min_z||max_z);
}

/* =======================================================

      Stencil Polys with Shadows
      
======================================================= */

void shadow_render_generic_stencil_poly(int poly_ptsz,int stencil_idx)
{
		// stencil the polygon
		
	glDisable(GL_BLEND);
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glVertexPointer(3,GL_FLOAT,0,0);
	
	glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
	glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);
	
	glStencilFunc(GL_ALWAYS,stencil_idx,0xFF);
	glDrawArrays(GL_TRIANGLE_FAN,0,poly_ptsz);
		
	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
}

/* =======================================================

      Render Model Shadows
      
======================================================= */

void shadow_render_model_mesh(model_type *mdl,int model_mesh_idx,model_draw *draw)
{
	int							n,k,i,map_mesh_idx,map_poly_idx,
								map_poly_count,draw_trig_count,
								light_intensity;
	double						dx,dy,dz,d_alpha;
	float						*vp,*vl,*cl,*vertex_ptr;
	d3vct						*vct;
	d3pnt						*spt,*hpt,*pt,bound_min,bound_max,light_pnt;
	map_mesh_type				*map_mesh;
	map_mesh_poly_type			*map_poly;
	model_mesh_type				*model_mesh;
    model_trig_type				*model_trig;
	
	model_mesh=&mdl->meshes[model_mesh_idx];
	
		// get light

	shadow_get_light_point(&draw->pnt,draw->size.y,&light_pnt,&light_intensity);
	
		// adjust light if angle would be it infinite
		
	if ((light_pnt.y>=(draw->pnt.y-(draw->size.y+view_shadows_infinite_light_shift))) && (light_pnt.y<=(draw->pnt.y+view_shadows_infinite_light_shift))) {
		light_pnt.y=(draw->pnt.y-draw->size.y)-view_shadows_infinite_light_shift;
	}
	
		// find all polys the shadow ray hits

	map_poly_count=shadow_build_poly_set_model(mdl,draw,&light_pnt,light_intensity);
	if (map_poly_count==0) return;

		// get distance alpha factor

	d_alpha=(double)light_intensity;
	d_alpha=1.0/(d_alpha*d_alpha);

		// setup the rays

	spt=shadow_spt;
	vct=shadow_vct;

	if (model_mesh->nvertex>=view_shadows_vertex_count) return;

	vp=draw->setup.mesh_arrays[model_mesh_idx].gl_vertex_array;
			
	for (n=0;n!=model_mesh->nvertex;n++) {
		spt->x=(int)*vp++;
		spt->y=(int)*vp++;
		spt->z=(int)*vp++;
				
		vct->x=(float)((spt->x-light_pnt.x)*100);
		vct->y=(float)((spt->y-light_pnt.y)*100);
		vct->z=(float)((spt->z-light_pnt.z)*100);
				
		spt++;
		vct++;
	}
	
		// start stenciling
		// never write to the depth buffer during this operation

	glEnable(GL_STENCIL_TEST);
	glDisable(GL_ALPHA_TEST);

	glDepthMask(GL_FALSE);
	
		// enable vertex array
		
	glEnableClientState(GL_VERTEX_ARRAY);

		// ray trace the mesh against
		// the plane of the polygon

		// build the vertexes, colors and indexes
		// for each poly and store in one list
		
	for (n=0;n!=map_poly_count;n++) {
	
		map_mesh_idx=shadow_poly_ptrs[n].mesh_idx;
		map_poly_idx=shadow_poly_ptrs[n].poly_idx;

			// ray trace the vertexes

		if (!ray_trace_mesh_poly_plane_by_vector(model_mesh->nvertex,shadow_spt,shadow_vct,shadow_hpt,map_mesh_idx,map_poly_idx)) continue;
		
			// setup bounding eliminations
		
		shadow_render_prepare_bounds_check(&shadow_poly_ptrs[n],&bound_min,&bound_max);

			// create the vertexes
			// to make it faster, we put the poly's vertexes at the top
			// so we can stencil with the same vbo
			//
			// skip triangles with no collisions
			
		map_mesh=&map.mesh.meshes[map_mesh_idx];
		map_poly=&map_mesh->polys[map_poly_idx];

		vertex_ptr=view_bind_map_next_vertex_object(((model_mesh->ntrig*3)*(3+4))+(map_poly->ptsz*3));
		if (vertex_ptr==NULL) return;

			// the poly vertexes
			
		vl=vertex_ptr;
		
		for (k=0;k!=map_poly->ptsz;k++) {
			pt=&map_mesh->vertexes[map_poly->v[k]];
			*vl++=(float)pt->x;
			*vl++=(float)pt->y;
			*vl++=(float)pt->z;
		}

			// the shadow vertexes and colors
			
		draw_trig_count=0;
			
		cl=vertex_ptr+(((model_mesh->ntrig*3)*3)+(map_poly->ptsz*3));
	
		for (k=0;k!=model_mesh->ntrig;k++) {
			model_trig=&model_mesh->trigs[k];

				// do a bounds check for quick eliminations

			if (!shadow_render_model_trig_bounds_check(&bound_min,&bound_max,model_trig->v[0],model_trig->v[1],model_trig->v[2])) continue;

				// triangle indexes
				
			for (i=0;i!=3;i++) {
				
				hpt=&shadow_hpt[model_trig->v[i]];
			
					// vertex

				*vl++=(float)hpt->x;
				*vl++=(float)hpt->y;
				*vl++=(float)hpt->z;

					// color

				dx=(hpt->x-light_pnt.x);
				dy=(hpt->y-light_pnt.y);
				dz=(hpt->z-light_pnt.z);

				*cl++=0.0f;
				*cl++=0.0f;
				*cl++=0.0f;
				*cl++=1.0f-(float)(((dx*dx)+(dy*dy)+(dz*dz))*d_alpha);
			}

			draw_trig_count++;
		}
		
		view_unmap_current_vertex_object();

			// if no trigs to draw, skip this mesh

		if (draw_trig_count==0) {
			view_unbind_current_vertex_object();
			continue;
		}
		
			// stencil in the polygon
			
		shadow_render_generic_stencil_poly(map_poly->ptsz,1);

			// setup arrays

		glVertexPointer(3,GL_FLOAT,0,(void*)((map_poly->ptsz*3)*sizeof(float)));

		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4,GL_FLOAT,0,(void*)((((model_mesh->ntrig*3)*3)+(map_poly->ptsz*3))*sizeof(float)));

			// run through all the stenciled polygons
			// and draw their trigs

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);

		glStencilOp(GL_KEEP,GL_KEEP,GL_ZERO);

		glColor4f(0.0f,0.0f,0.0f,1.0f);
				
		glStencilFunc(GL_EQUAL,1,0xFF);
		glDrawArrays(GL_TRIANGLES,0,(draw_trig_count*3));
		
		view.count.poly+=draw_trig_count;

			// disable the color array

		glDisableClientState(GL_COLOR_ARRAY);
				
			// clear the stencils

		shadow_render_generic_stencil_poly(map_poly->ptsz,0);

			// unbind the vertex and index object
				
		view_unbind_current_vertex_object();
	}

		// disable vertex array
		
	glDisableClientState(GL_VERTEX_ARRAY);

		// restore depth buffer and turn
		// off stenciling
			
	glDepthMask(GL_TRUE);
	glDisable(GL_STENCIL_TEST);
	
		// count the shadow
		
	view.count.shadow++;
}

void shadow_render_model(model_draw *draw)
{
	int						n;
	model_type				*mdl;
	
		// get model

	if ((draw->model_idx==-1) || (!draw->on)) return;
	
	mdl=server.model_list.models[draw->model_idx];

		// run through the meshes
		
	for (n=0;n!=mdl->nmesh;n++) {
		if ((draw->render_mesh_mask&(0x1<<n))!=0) shadow_render_model_mesh(mdl,n,draw);
	}
}

/* =======================================================

      Render Mesh Shadows
      
======================================================= */

void shadow_render_mesh(int shadow_mesh_idx)
{
	int							n,k,i,map_mesh_idx,map_poly_idx,
								map_poly_count,shadow_mesh_trig_count,draw_trig_count,light_intensity;
	double						dx,dy,dz,d_alpha;
	float						*vl,*cl,*vertex_ptr;
	d3vct						*vct;
	d3pnt						*spt,*hpt,*pt,bound_min,bound_max,light_pnt;
	map_mesh_type				*map_mesh,*shadow_mesh;
	map_mesh_poly_type			*map_poly,*shadow_poly;
	
	shadow_mesh=&map.mesh.meshes[shadow_mesh_idx];
	if (shadow_mesh->nvertex>=view_shadows_vertex_count) return;
	
		// get light
	
	shadow_get_light_point(&shadow_mesh->box.mid,(shadow_mesh->box.max.y-shadow_mesh->box.min.y),&light_pnt,&light_intensity);
	
		// adjust light if angle would be it infinite
	
	if ((light_pnt.y>=(shadow_mesh->box.min.y+view_shadows_infinite_light_shift)) && (light_pnt.y<=(shadow_mesh->box.max.y+view_shadows_infinite_light_shift))) {
		light_pnt.y=shadow_mesh->box.min.y-view_shadows_infinite_light_shift;
	}

		// find all polys the shadow ray hits

	map_poly_count=shadow_build_poly_set_mesh(shadow_mesh_idx,&light_pnt,light_intensity);
	if (map_poly_count==0) return;

		// get distance alpha factor

	d_alpha=(double)light_intensity;
	d_alpha=1.0/(d_alpha*d_alpha);

	spt=shadow_spt;
	vct=shadow_vct;
	pt=shadow_mesh->vertexes;
	
	for (n=0;n!=shadow_mesh->nvertex;n++) {
		spt->x=pt->x;
		spt->y=pt->y;
		spt->z=pt->z;
		
		vct->x=(float)((spt->x-light_pnt.x)*100);
		vct->y=(float)((spt->y-light_pnt.y)*100);
		vct->z=(float)((spt->z-light_pnt.z)*100);
		
		spt++;
		vct++;
		pt++;
	}
	
		// count the possible trigs
	
	shadow_mesh_trig_count=0;
	
	shadow_poly=shadow_mesh->polys;
	
	for (n=0;n!=shadow_mesh->npoly;n++) {
		shadow_mesh_trig_count+=(shadow_poly->ptsz-2);
		shadow_poly++;
	}
	
		// start stenciling
		// never write to the depth buffer during this operation

	glEnable(GL_STENCIL_TEST);
	glDisable(GL_ALPHA_TEST);

	glDepthMask(GL_FALSE);
	
		// enable vertex array
		
	glEnableClientState(GL_VERTEX_ARRAY);

		// ray trace the mesh against
		// the plane of the polygon

		// build the vertexes, colors and indexes
		// for each poly and store in one list

	for (n=0;n!=map_poly_count;n++) {
	
		map_mesh_idx=shadow_poly_ptrs[n].mesh_idx;
		map_poly_idx=shadow_poly_ptrs[n].poly_idx;

			// ray trace the vertexes

		if (!ray_trace_mesh_poly_plane_by_vector(shadow_mesh->nvertex,shadow_spt,shadow_vct,shadow_hpt,map_mesh_idx,map_poly_idx)) continue;
		
			// setup bounding eliminations
		
		shadow_render_prepare_bounds_check(&shadow_poly_ptrs[n],&bound_min,&bound_max);

			// create the vertexes
			// to make it faster, we put the poly's vertexes at the top
			// so we can stencil with the same vbo
			//
			// skip triangles with no collisions
			
		map_mesh=&map.mesh.meshes[map_mesh_idx];
		map_poly=&map_mesh->polys[map_poly_idx];

		vertex_ptr=view_bind_map_next_vertex_object(((shadow_mesh_trig_count*3)*(3+4))+(map_poly->ptsz*3));
		if (vertex_ptr==NULL) return;

			// the poly vertexes
			
		vl=vertex_ptr;
		
		for (k=0;k!=map_poly->ptsz;k++) {
			pt=&map_mesh->vertexes[map_poly->v[k]];
			*vl++=(float)pt->x;
			*vl++=(float)pt->y;
			*vl++=(float)pt->z;
		}

			// the shadow vertexes and colors
			
		draw_trig_count=0;
			
		cl=vertex_ptr+(((shadow_mesh_trig_count*3)*3)+(map_poly->ptsz*3));
	
		for (k=0;k!=shadow_mesh->npoly;k++) {
			shadow_poly=&shadow_mesh->polys[k];
			
				// polygon bounds elimination

			if (!shadow_render_mesh_poly_bounds_check(&bound_min,&bound_max,shadow_poly)) continue;

				// turn into triangles
				
			for (i=0;i!=(shadow_poly->ptsz-2);i++) {

					// vertex 1
					
				hpt=&shadow_hpt[shadow_poly->v[0]];

				*vl++=(float)hpt->x;
				*vl++=(float)hpt->y;
				*vl++=(float)hpt->z;

				dx=(hpt->x-light_pnt.x);
				dy=(hpt->y-light_pnt.y);
				dz=(hpt->z-light_pnt.z);

				*cl++=0.0f;
				*cl++=0.0f;
				*cl++=0.0f;
				*cl++=1.0f-(float)(((dx*dx)+(dy*dy)+(dz*dz))*d_alpha);

					// vertex 2
					
				hpt=&shadow_hpt[shadow_poly->v[i+1]];

				*vl++=(float)hpt->x;
				*vl++=(float)hpt->y;
				*vl++=(float)hpt->z;

				dx=(hpt->x-light_pnt.x);
				dy=(hpt->y-light_pnt.y);
				dz=(hpt->z-light_pnt.z);

				*cl++=0.0f;
				*cl++=0.0f;
				*cl++=0.0f;
				*cl++=1.0f-(float)(((dx*dx)+(dy*dy)+(dz*dz))*d_alpha);
				
					// vertex 3
					
				hpt=&shadow_hpt[shadow_poly->v[i+2]];

				*vl++=(float)hpt->x;
				*vl++=(float)hpt->y;
				*vl++=(float)hpt->z;

				dx=(hpt->x-light_pnt.x);
				dy=(hpt->y-light_pnt.y);
				dz=(hpt->z-light_pnt.z);

				*cl++=0.0f;
				*cl++=0.0f;
				*cl++=0.0f;
				*cl++=1.0f-(float)(((dx*dx)+(dy*dy)+(dz*dz))*d_alpha);

				draw_trig_count++;
			}
		}
		
		view_unmap_current_vertex_object();

			// if no trigs to draw, skip this mesh

		if (draw_trig_count==0) {
			view_unbind_current_vertex_object();
			continue;
		}
		
			// stencil in the polygon
			
		shadow_render_generic_stencil_poly(map_poly->ptsz,1);

			// setup arrays

		glVertexPointer(3,GL_FLOAT,0,(void*)((map_poly->ptsz*3)*sizeof(float)));

		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4,GL_FLOAT,0,(void*)((((shadow_mesh_trig_count*3)*3)+(map_poly->ptsz*3))*sizeof(float)));

			// run through all the stenciled polygons
			// and draw their trigs

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);

		glStencilOp(GL_KEEP,GL_KEEP,GL_ZERO);

		glColor4f(0.0f,0.0f,0.0f,1.0f);
				
		glStencilFunc(GL_EQUAL,1,0xFF);
		glDrawArrays(GL_TRIANGLES,0,(draw_trig_count*3));
		
		view.count.poly+=draw_trig_count;

			// disable the color array

		glDisableClientState(GL_COLOR_ARRAY);
				
			// clear the stencils

		shadow_render_generic_stencil_poly(map_poly->ptsz,0);

			// unbind the vertex and index object
				
		view_unbind_current_vertex_object();
	}
	
		// disable vertex array
		
	glDisableClientState(GL_VERTEX_ARRAY);

		// restore depth buffer and turn
		// off stenciling
			
	glDepthMask(GL_TRUE);
	glDisable(GL_STENCIL_TEST);
	
		// count the shadow
		
	view.count.shadow++;
}

