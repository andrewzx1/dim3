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

d3fpnt					*shadow_spt,*shadow_hpt;
d3vct					*shadow_vct;
poly_pointer_type		*shadow_poly_ptrs;

/* =======================================================

      Initialize and Shutdown Shadows
      
======================================================= */

bool shadow_initialize(void)
{
		// memory for ray tracing lists
		
	shadow_spt=(d3fpnt*)malloc(sizeof(d3fpnt)*view_shadows_model_vertex_count);
	if (shadow_spt==NULL) return(FALSE);
	
	shadow_hpt=(d3fpnt*)malloc(sizeof(d3fpnt)*view_shadows_model_vertex_count);
	if (shadow_hpt==NULL) return(FALSE);
	
	shadow_vct=(d3vct*)malloc(sizeof(d3vct)*view_shadows_model_vertex_count);
	if (shadow_vct==NULL) return(FALSE);
	
		// memory for map poly ptrs
	
	shadow_poly_ptrs=(poly_pointer_type*)malloc(view_shadows_map_poly_count*sizeof(poly_pointer_type));
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

      Build Shadow Bound Box For Obscuring
      
======================================================= */

void shadow_get_bound_box(d3pnt *pnt,int high,d3pnt *light_pnt,int light_intensity,d3pnt *min,d3pnt *max)
{
	int							n;
	float						f_dist;
	d3pnt						*sp;
	d3pnt						spt[8],ept;
	d3vct						ray_move;

	spt[0].x=spt[1].x=spt[4].x=spt[5].x=min->x;
	spt[2].x=spt[3].x=spt[6].x=spt[7].x=max->x;
	
	spt[0].y=spt[1].y=spt[2].y=spt[3].y=min->y;
	spt[4].y=spt[5].y=spt[6].y=spt[7].y=max->y;

	spt[1].z=spt[2].z=spt[5].z=spt[6].z=min->z;
	spt[0].z=spt[3].z=spt[4].z=spt[7].z=max->z;

		// ray trace bounding box
		
	sp=spt;

	min->x=max->x=spt[0].x;
	min->y=max->y=spt[0].y;
	min->z=max->z=spt[0].z;
	
	f_dist=(float)light_intensity;

	for (n=0;n!=8;n++) {
		vector_create(&ray_move,light_pnt->x,light_pnt->y,light_pnt->z,sp->x,sp->y,sp->z);
				
		ept.x=light_pnt->x-(int)(ray_move.x*f_dist);
		ept.y=light_pnt->y-(int)(ray_move.y*f_dist);
		ept.z=light_pnt->z-(int)(ray_move.z*f_dist);

		if (sp->x<min->x) min->x=sp->x;
		if (ept.x<min->x) min->x=ept.x;
		if (sp->x>max->x) max->x=sp->x;
		if (ept.x>max->x) max->x=ept.x;

		if (sp->z<min->z) min->z=sp->z;
		if (ept.z<min->z) min->z=ept.z;
		if (sp->z>max->z) max->z=sp->z;
		if (ept.z>max->z) max->z=ept.z;

		if (sp->y<min->y) min->y=sp->y;
		if (ept.y<min->y) min->y=ept.y;
		if (sp->y>max->y) max->y=sp->y;
		if (ept.y>max->y) max->y=ept.y;
		
		sp++;
	}
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
	glLineWidth(1.0f);
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
				// by camera culling and
				// light direction culling
				
			if (((poly->tangent_space.normal.x*(float)(poly->box.mid.x-view.render->camera.pnt.x))+(poly->tangent_space.normal.y*(float)(poly->box.mid.y-view.render->camera.pnt.y))+(poly->tangent_space.normal.z*(float)(poly->box.mid.z-view.render->camera.pnt.z)))>map.optimize.cull_angle) continue;
			if (((poly->tangent_space.normal.x*(float)(poly->box.mid.x-light_pnt->x))+(poly->tangent_space.normal.y*(float)(poly->box.mid.y-light_pnt->y))+(poly->tangent_space.normal.z*(float)(poly->box.mid.z-light_pnt->z)))>map.optimize.cull_angle) continue;

				// add to shadow list
				
			shadow_poly_ptrs[cnt].mesh_idx=view.render->draw_list.items[n].idx;
			shadow_poly_ptrs[cnt].poly_idx=k;
			cnt++;
			
				// can only shadow for max stencil count
				
			if (cnt==view_shadows_map_poly_count) return(cnt);
		}
	}
	
	return(cnt);
}

int shadow_build_poly_set_model(model_type *mdl,model_draw *draw)
{
	d3pnt			volume_min,volume_max,min,max;
	
		// get bounding box of model
		
	model_get_view_complex_bounding_volume(mdl,&draw->pnt,&draw->setup.ang,&volume_min,&volume_max);
	
		// get the shadow volume
		
	memmove(&min,&volume_min,sizeof(d3pnt));
	memmove(&max,&volume_max,sizeof(d3pnt));
	shadow_get_volume(&draw->pnt,draw->size.y,&draw->shadow.light_pnt,draw->shadow.light_intensity,&min,&max);
	
		// get the polys that cross that volume
		
	return(shadow_build_poly_cross_volume_set(&draw->shadow.light_pnt,&volume_min,&volume_max,&min,&max,-1));	
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

bool shadow_render_model_poly_bounds_check_skip(d3pnt *min,d3pnt *max,model_poly_type *poly)
{
	int				n,x,y,z;
	bool			min_x,max_x,min_y,max_y,min_z,max_z;
	
	min_x=max_x=min_y=max_y=min_z=max_z=TRUE;
	
	for (n=0;n!=poly->ptsz;n++) {
	
		x=(int)shadow_hpt[poly->v[n]].x;
		min_x&=(x<min->x);
		max_x&=(x>max->x);
		
		y=(int)shadow_hpt[poly->v[n]].y;
		min_y&=(y<min->y);
		max_y&=(y>max->y);
		
		z=(int)shadow_hpt[poly->v[n]].z;
		min_z&=(z<min->z);
		max_z&=(z>max->z);
	}
	
	return(min_x||max_x||min_y||max_y||min_z||max_z);
}

/* =======================================================

      Stencil Polys with Shadows
      
======================================================= */

int shadow_render_stencil_poly_setup(map_mesh_type *mesh,map_mesh_poly_type *poly,float *vertexes)
{
	int				n;
	d3pnt			*pnt;

	for (n=0;n!=poly->ptsz;n++) {
		pnt=&mesh->vertexes[poly->v[n]];
		*vertexes++=(float)pnt->x;
		*vertexes++=(float)pnt->y;
		*vertexes++=(float)pnt->z;
	}
	
	return(poly->ptsz);
}

void shadow_render_stencil_poly_draw(int ptsz,float *vertexes,int stencil_idx)
{
	glDisable(GL_BLEND);
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glVertexPointer(3,GL_FLOAT,0,(GLvoid*)vertexes);

	glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
	glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);
	glStencilFunc(GL_ALWAYS,stencil_idx,0xFF);

	glDrawArrays(GL_TRIANGLE_FAN,0,ptsz);
		
	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
}

/* =======================================================

      Render Model Shadows
      
======================================================= */

void shadow_render_model_mesh(model_type *mdl,int model_mesh_idx,model_draw *draw)
{
	int							n,k,i,map_mesh_idx,map_poly_idx,
								map_poly_count;
	unsigned short				indexes[8];
	float						alpha,f_light_intensity,stencil_poly_vertexes[8*3];
	float						*pf,*va;
	unsigned char				*vertex_ptr;
	d3vct						*vct;
	d3pnt						bound_min,bound_max;
	d3fpnt						*spt;
	d3fpnt						f_light_pnt;
	map_mesh_type				*map_mesh;
	map_mesh_poly_type			*map_poly;
	model_mesh_type				*model_mesh;
    model_poly_type				*model_poly;

	model_mesh=&mdl->meshes[model_mesh_idx];
	if (model_mesh->nvertex>=view_shadows_model_vertex_count) return;
	
		// find all polys the shadow ray hits

	map_poly_count=shadow_build_poly_set_model(mdl,draw);
	if (map_poly_count==0) return;

		// setup the rays
		// clip them at the light intensity
		// distance

	spt=shadow_spt;
	vct=shadow_vct;

	f_light_intensity=(float)draw->shadow.light_intensity;
	f_light_pnt.x=(float)draw->shadow.light_pnt.x;
	f_light_pnt.y=(float)draw->shadow.light_pnt.y;
	f_light_pnt.z=(float)draw->shadow.light_pnt.z;

	va=draw->setup.mesh_arrays[model_mesh_idx].gl_vertex_array;
			
	for (n=0;n!=model_mesh->nvertex;n++) {
	
		spt->x=*va++;
		spt->y=*va++;
		spt->z=*va++;

			// setup vector
			
		vct->x=spt->x-f_light_pnt.x;
		vct->y=spt->y-f_light_pnt.y;
		vct->z=spt->z-f_light_pnt.z;

		vector_normalize(vct);
		vct->x*=f_light_intensity;
		vct->y*=f_light_intensity;
		vct->z*=f_light_intensity;
				
		spt++;
		vct++;
	}
	
		// start stenciling
		// never write to the depth buffer during this operation

	glEnable(GL_STENCIL_TEST);
	glDisable(GL_ALPHA_TEST);

	glDepthMask(GL_FALSE);

		// ray trace the mesh against
		// the plane of the polygon

		// build the vertexes, colors and indexes
		// for each poly and store in one list
		
	for (n=0;n!=map_poly_count;n++) {
	
		map_mesh_idx=shadow_poly_ptrs[n].mesh_idx;
		map_poly_idx=shadow_poly_ptrs[n].poly_idx;

		map_mesh=&map.mesh.meshes[map_mesh_idx];
		map_poly=&map_mesh->polys[map_poly_idx];
				
			// stencil in the polygon shadow is crossing
			
		shadow_render_stencil_poly_setup(map_mesh,map_poly,stencil_poly_vertexes);
		shadow_render_stencil_poly_draw(map_poly->ptsz,stencil_poly_vertexes,1);

			// create the vertexes
			
		view_bind_model_shadow_vertex_object(draw,model_mesh_idx);

		vertex_ptr=view_map_model_shadow_vertex_object();
		if (vertex_ptr==NULL) {
			view_unbind_model_shadow_vertex_object();
			return;
		}
		
			// build the vertex list
	
		pf=(float*)vertex_ptr;
		ray_trace_mesh_poly_plane_by_vector_to_buffer(model_mesh->nvertex,shadow_spt,shadow_vct,pf,map_mesh_idx,map_poly_idx);

/* supergumba, get rid of shadow_hpt after this works
		hpt=shadow_hpt;
		
		for (k=0;k!=model_mesh->nvertex;k++) {
			*pf++=hpt->x;
			*pf++=hpt->y;
			*pf++=hpt->z;

			hpt++;
		}
		*/
		view_unmap_model_shadow_vertex_object();
		
			// the color
			
		alpha=1.0f-((float)distance_get(draw->pnt.x,draw->pnt.y,draw->pnt.z,draw->shadow.light_pnt.x,draw->shadow.light_pnt.y,draw->shadow.light_pnt.z))/f_light_intensity;
		glColor4f(0.0f,0.0f,0.0f,alpha);

			// setup arrays

		glVertexPointer(3,GL_FLOAT,0,(GLvoid*)0);

			// draw the trigs onto the
			// stenciled polygon

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);

		glStencilOp(GL_KEEP,GL_KEEP,GL_ZERO);

		glStencilFunc(GL_EQUAL,1,0xFF);
		
			// setup bounding eliminations
		
		shadow_render_prepare_bounds_check(&shadow_poly_ptrs[n],&bound_min,&bound_max);

			// run through the shadow polygons
			// skipping any we can

		for (k=0;k!=model_mesh->npoly;k++) {
			model_poly=&model_mesh->polys[k];

				// do a bounds check for quick eliminations
		
			if (shadow_render_model_poly_bounds_check_skip(&bound_min,&bound_max,model_poly)) continue;

				// polygon indexes
				
			for (i=0;i!=model_poly->ptsz;i++) {
				indexes[i]=(unsigned short)model_poly->v[i];
			}

			glDrawElements(GL_TRIANGLE_FAN,model_poly->ptsz,GL_UNSIGNED_SHORT,(GLvoid*)indexes);

			view.count.shadow_poly++;
		}

			// reset colors

		glColor4f(0.0f,0.0f,0.0f,1.0f);

			// unbind the vertex and index object
				
		view_unbind_model_shadow_vertex_object();
				
			// clear the stencils

		shadow_render_stencil_poly_draw(map_poly->ptsz,stencil_poly_vertexes,0);
	}

		// restore depth buffer and turn
		// off stenciling
			
	glDepthMask(GL_TRUE);
	glDisable(GL_STENCIL_TEST);
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

