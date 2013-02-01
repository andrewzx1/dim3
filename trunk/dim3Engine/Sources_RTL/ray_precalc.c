//
// dim3RTL
// by Brian Barnes (bbarnes@klinksoftware.com)
// free software offered as is for any use
//

#include "ray_os_defs.h"
#include "ray.h"
#include "ray_interface.h"

ray_global_type					ray_global;

/* =======================================================

      Precalculate Mesh Bounds
      
======================================================= */

void ray_precalc_mesh_bounds(ray_mesh_type *mesh)
{
	int					n;
	ray_point_type		*pnt;
	ray_bound_type		*bnd;
	
	bnd=&mesh->bound;
	
	pnt=mesh->vertex_block.vertexes;

	bnd->min.x=bnd->max.x=pnt->x;
	bnd->min.y=bnd->max.y=pnt->y;
	bnd->min.z=bnd->max.z=pnt->z;
	
	pnt++;
	
	for (n=1;n<mesh->vertex_block.count;n++) {
		if (pnt->x<bnd->min.x) bnd->min.x=pnt->x;
		if (pnt->x>bnd->max.x) bnd->max.x=pnt->x;

		if (pnt->y<bnd->min.y) bnd->min.y=pnt->y;
		if (pnt->y>bnd->max.y) bnd->max.y=pnt->y;

		if (pnt->z<bnd->min.z) bnd->min.z=pnt->z;
		if (pnt->z>bnd->max.z) bnd->max.z=pnt->z;
		
		pnt++;
	}

	bnd->mid.x=(bnd->min.x+bnd->max.x)*0.5f;
	bnd->mid.y=(bnd->min.y+bnd->max.y)*0.5f;
	bnd->mid.z=(bnd->min.z+bnd->max.z)*0.5f;
}

/* =======================================================

      Precalculate Polygon Bounds and Normals
      
======================================================= */

void ray_precalc_polygon_bounds(ray_mesh_type *mesh,ray_poly_type *poly)
{
	int					n;
	ray_point_type		*pnt;
	ray_bound_type		*bnd;
	
	bnd=&poly->bound;
	
	pnt=&mesh->vertex_block.vertexes[poly->idxs[0].vertex];

	bnd->min.x=bnd->max.x=pnt->x;
	bnd->min.y=bnd->max.y=pnt->y;
	bnd->min.z=bnd->max.z=pnt->z;
	
	pnt++;
	
	for (n=1;n<poly->nvertex;n++) {
		pnt=&mesh->vertex_block.vertexes[poly->idxs[n].vertex];
		
		if (pnt->x<bnd->min.x) bnd->min.x=pnt->x;
		if (pnt->x>bnd->max.x) bnd->max.x=pnt->x;

		if (pnt->y<bnd->min.y) bnd->min.y=pnt->y;
		if (pnt->y>bnd->max.y) bnd->max.y=pnt->y;

		if (pnt->z<bnd->min.z) bnd->min.z=pnt->z;
		if (pnt->z>bnd->max.z) bnd->max.z=pnt->z;
		
		pnt++;
	}

		// calc the mid

	bnd->mid.x=(bnd->min.x+bnd->max.x)*0.5f;
	bnd->mid.y=(bnd->min.y+bnd->max.y)*0.5f;
	bnd->mid.z=(bnd->min.z+bnd->max.z)*0.5f;
}

void ray_precalc_polygon_normal(ray_mesh_type *mesh,ray_poly_type *poly)
{
	int					n;
	float				f;
	ray_vector_type		*normal;
	
	if (mesh->normal_block.count==0) {
		poly->surface_normal.x=0.0f;
		poly->surface_normal.y=0.0f;
		poly->surface_normal.z=0.0f;
		return;
	}

	poly->surface_normal.x=0.0f;
	poly->surface_normal.y=0.0f;
	poly->surface_normal.z=0.0f;
	
	for (n=0;n!=poly->nvertex;n++) {
		normal=&mesh->normal_block.normals[poly->idxs[n].normal];
		poly->surface_normal.x+=normal->x;
		poly->surface_normal.y+=normal->y;
		poly->surface_normal.z+=normal->z;
	}
	
	f=(float)poly->nvertex;
	poly->surface_normal.x/=f;
	poly->surface_normal.y/=f;
	poly->surface_normal.z/=f;
	
	ray_vector_normalize(&poly->surface_normal);
}

void ray_precalc_polygon_plane(ray_mesh_type *mesh,ray_poly_type *poly)
{
	ray_point_type			*pnt;
	
		// plane equation is just the normal
		// plus a distance calc
		
	poly->plane.a=poly->surface_normal.x;
	poly->plane.b=poly->surface_normal.y;
	poly->plane.c=poly->surface_normal.z;
	
	pnt=&mesh->vertex_block.vertexes[poly->idxs[0].vertex];
	poly->plane.d=-((poly->plane.a*pnt->x)+(poly->plane.b*pnt->y)+(poly->plane.c*pnt->z));
}

/* =======================================================

      Precalculate Triangle Bounds
      
======================================================= */

void ray_precalc_triangle_bounds(ray_mesh_type *mesh,ray_trig_type *trig)
{
	ray_point_type		*p0,*p1,*p2;
	ray_bound_type		*bnd;
	
		// get the points
		
	p0=&mesh->vertex_block.vertexes[trig->idxs[0].vertex];
	p1=&mesh->vertex_block.vertexes[trig->idxs[1].vertex];
	p2=&mesh->vertex_block.vertexes[trig->idxs[2].vertex];

		// find bounds

	bnd=&trig->bound;

	bnd->min.x=bnd->max.x=p0->x;
	bnd->min.y=bnd->max.y=p0->y;
	bnd->min.z=bnd->max.z=p0->z;

	if (p1->x<bnd->min.x) bnd->min.x=p1->x;
	if (p1->x>bnd->max.x) bnd->max.x=p1->x;

	if (p1->y<bnd->min.y) bnd->min.y=p1->y;
	if (p1->y>bnd->max.y) bnd->max.y=p1->y;

	if (p1->z<bnd->min.z) bnd->min.z=p1->z;
	if (p1->z>bnd->max.z) bnd->max.z=p1->z;

	if (p2->x<bnd->min.x) bnd->min.x=p2->x;
	if (p2->x>bnd->max.x) bnd->max.x=p2->x;

	if (p2->y<bnd->min.y) bnd->min.y=p2->y;
	if (p2->y>bnd->max.y) bnd->max.y=p2->y;

	if (p2->z<bnd->min.z) bnd->min.z=p2->z;
	if (p2->z>bnd->max.z) bnd->max.z=p2->z;

		// calc the mid

	bnd->mid.x=(bnd->min.x+bnd->max.x)*0.5f;
	bnd->mid.y=(bnd->min.y+bnd->max.y)*0.5f;
	bnd->mid.z=(bnd->min.z+bnd->max.z)*0.5f;
}

/* =======================================================

      Precalculate Light Bounds
      
======================================================= */

void ray_precalc_light_bounds(ray_light_type *light)
{
	light->bound.min.x=light->pnt.x-light->intensity;
	light->bound.min.y=light->pnt.y-light->intensity;
	light->bound.min.z=light->pnt.z-light->intensity;
	
	light->bound.max.x=light->pnt.x+light->intensity;
	light->bound.max.y=light->pnt.y+light->intensity;
	light->bound.max.z=light->pnt.z+light->intensity;
	
	light->bound.mid.x=(light->bound.min.x+light->bound.max.x)*0.5f;
	light->bound.mid.y=(light->bound.min.y+light->bound.max.y)*0.5f;
	light->bound.mid.z=(light->bound.min.z+light->bound.max.z)*0.5f;
}

/* =======================================================

      Precalculate Triangle Vectors
      
======================================================= */

void ray_precalc_triangle_vectors(ray_mesh_type *mesh,ray_trig_type *trig)
{
	ray_point_type		*p0,*p1,*p2;
	
		// get the points
		
	p0=&mesh->vertex_block.vertexes[trig->idxs[0].vertex];
	p1=&mesh->vertex_block.vertexes[trig->idxs[1].vertex];
	p2=&mesh->vertex_block.vertexes[trig->idxs[2].vertex];

		// precalc the vectors sharing
		// triangle around vertex 0
		
	ray_vector_create_from_points(&trig->v1,p1,p0);
	ray_vector_create_from_points(&trig->v2,p2,p0);
}

/* =======================================================

      Frustum Culling
      
======================================================= */

void ray_precalc_build_frustum_plane_single(ray_plane_type *plane,ray_point_type *p0,ray_point_type *p1,ray_point_type *p2)
{
	ray_vector_type			v,u,normal;
	
		// get the normal
		
	ray_vector_create_from_points(&v,p1,p0);
	ray_vector_create_from_points(&u,p2,p0);
	
	ray_vector_cross_product(&normal,&v,&u);
	ray_vector_normalize(&normal);

		// get the a,b,c

	plane->a=normal.x;
	plane->b=normal.y;
	plane->c=normal.z;

		// calculate d

	plane->d=-((plane->a*p0->x)+(plane->b*p0->y)+(plane->c*p0->z));
}

void ray_precalc_build_frustum_planes(ray_scene_type *scene,ray_draw_scene_thread_info *thread_info,ray_plane_type *planes)
{
	float					lx,rx,ty,by,wid,high;
	ray_point_type			*eye_point;
	ray_point_type			view_plane_point,
							top_lft_near_pnt,top_rgt_near_pnt,
							bot_lft_near_pnt,bot_rgt_near_pnt,
							top_lft_far_pnt,top_rgt_far_pnt,
							bot_lft_far_pnt,bot_rgt_far_pnt;
	ray_vector_type			center_vct,
							top_lft_vct,top_rgt_vct,
							bot_lft_vct,bot_rgt_vct;
	
		// projection plane size

	lx=(float)thread_info->pixel_start.x;
	rx=(float)thread_info->pixel_end.x;
	ty=(float)thread_info->pixel_start.y;
	by=(float)thread_info->pixel_end.y;

	wid=(float)(scene->buffer.wid>>1);
	high=(float)(scene->buffer.high>>1);
		
		// the the four vectors that
		// pass through the four corners of
		// the near and far plane
		
	eye_point=&scene->eye.pnt;

	view_plane_point.z=eye_point->z+scene->eye.min_dist;

	view_plane_point.x=eye_point->x;
	view_plane_point.y=eye_point->y;
	ray_vector_create_from_points(&center_vct,&view_plane_point,eye_point);
	rtlMatrixVectorMultiply(&scene->eye.matrix,&center_vct);
	ray_vector_normalize(&center_vct);

	view_plane_point.x=(eye_point->x-wid)+lx;
	view_plane_point.y=(eye_point->y-high)+ty;
	ray_vector_create_from_points(&top_lft_vct,&view_plane_point,eye_point);
	rtlMatrixVectorMultiply(&scene->eye.matrix,&top_lft_vct);
	ray_vector_normalize(&top_lft_vct);

	view_plane_point.x=(eye_point->x-wid)+lx;
	view_plane_point.y=(eye_point->y-high)+by;
	ray_vector_create_from_points(&bot_lft_vct,&view_plane_point,eye_point);
	rtlMatrixVectorMultiply(&scene->eye.matrix,&bot_lft_vct);
	ray_vector_normalize(&bot_lft_vct);

	view_plane_point.x=(eye_point->x-wid)+rx;
	view_plane_point.y=(eye_point->y-high)+ty;
	ray_vector_create_from_points(&top_rgt_vct,&view_plane_point,eye_point);
	rtlMatrixVectorMultiply(&scene->eye.matrix,&top_rgt_vct);
	ray_vector_normalize(&top_rgt_vct);

	view_plane_point.x=(eye_point->x-wid)+rx;
	view_plane_point.y=(eye_point->y-high)+by;
	ray_vector_create_from_points(&bot_rgt_vct,&view_plane_point,eye_point);
	rtlMatrixVectorMultiply(&scene->eye.matrix,&bot_rgt_vct);
	ray_vector_normalize(&bot_rgt_vct);
	
		// get the 8 plane points

	top_lft_near_pnt.x=eye_point->x+(top_lft_vct.x*scene->eye.min_dist);
	top_lft_near_pnt.y=eye_point->y+(top_lft_vct.y*scene->eye.min_dist);
	top_lft_near_pnt.z=eye_point->z+(top_lft_vct.z*scene->eye.min_dist);
		
	top_lft_far_pnt.x=eye_point->x+(top_lft_vct.x*scene->eye.max_dist);
	top_lft_far_pnt.y=eye_point->y+(top_lft_vct.y*scene->eye.max_dist);
	top_lft_far_pnt.z=eye_point->z+(top_lft_vct.z*scene->eye.max_dist);
	
	top_rgt_near_pnt.x=eye_point->x+(top_rgt_vct.x*scene->eye.min_dist);
	top_rgt_near_pnt.y=eye_point->y+(top_rgt_vct.y*scene->eye.min_dist);
	top_rgt_near_pnt.z=eye_point->z+(top_rgt_vct.z*scene->eye.min_dist);
		
	top_rgt_far_pnt.x=eye_point->x+(top_rgt_vct.x*scene->eye.max_dist);
	top_rgt_far_pnt.y=eye_point->y+(top_rgt_vct.y*scene->eye.max_dist);
	top_rgt_far_pnt.z=eye_point->z+(top_rgt_vct.z*scene->eye.max_dist);
	
	bot_lft_near_pnt.x=eye_point->x+(bot_lft_vct.x*scene->eye.min_dist);
	bot_lft_near_pnt.y=eye_point->y+(bot_lft_vct.y*scene->eye.min_dist);
	bot_lft_near_pnt.z=eye_point->z+(bot_lft_vct.z*scene->eye.min_dist);
		
	bot_lft_far_pnt.x=eye_point->x+(bot_lft_vct.x*scene->eye.max_dist);
	bot_lft_far_pnt.y=eye_point->y+(bot_lft_vct.y*scene->eye.max_dist);
	bot_lft_far_pnt.z=eye_point->z+(bot_lft_vct.z*scene->eye.max_dist);
	
	bot_rgt_near_pnt.x=eye_point->x+(bot_rgt_vct.x*scene->eye.min_dist);
	bot_rgt_near_pnt.y=eye_point->y+(bot_rgt_vct.y*scene->eye.min_dist);
	bot_rgt_near_pnt.z=eye_point->z+(bot_rgt_vct.z*scene->eye.min_dist);
		
	bot_rgt_far_pnt.x=eye_point->x+(bot_rgt_vct.x*scene->eye.max_dist);
	bot_rgt_far_pnt.y=eye_point->y+(bot_rgt_vct.y*scene->eye.max_dist);
	bot_rgt_far_pnt.z=eye_point->z+(bot_rgt_vct.z*scene->eye.max_dist);
	
		// get the 6 frustum planes
		// left, right, top, bottom, near, and far
		
	ray_precalc_build_frustum_plane_single(&planes[0],&bot_lft_near_pnt,&top_lft_near_pnt,&bot_lft_far_pnt);
	ray_precalc_build_frustum_plane_single(&planes[1],&top_rgt_near_pnt,&bot_rgt_near_pnt,&top_rgt_far_pnt);
	ray_precalc_build_frustum_plane_single(&planes[2],&top_lft_near_pnt,&top_rgt_near_pnt,&top_lft_far_pnt);
	ray_precalc_build_frustum_plane_single(&planes[3],&bot_rgt_near_pnt,&bot_lft_near_pnt,&bot_rgt_far_pnt);
	ray_precalc_build_frustum_plane_single(&planes[4],&top_lft_near_pnt,&bot_lft_near_pnt,&top_rgt_near_pnt);
	ray_precalc_build_frustum_plane_single(&planes[5],&bot_lft_far_pnt,&top_lft_far_pnt,&bot_rgt_far_pnt);
}

bool ray_precalc_frustum_plane_bound_cull(ray_plane_type *planes,ray_bound_type *bnd)
{
	int				n;

	for (n=0;n!=6;n++) {
		if (((planes[n].a*bnd->min.x)+(planes[n].b*bnd->min.y)+(planes[n].c*bnd->min.z)+planes[n].d)>0.0f) continue;
		if (((planes[n].a*bnd->max.x)+(planes[n].b*bnd->min.y)+(planes[n].c*bnd->min.z)+planes[n].d)>0.0f) continue;
		if (((planes[n].a*bnd->min.x)+(planes[n].b*bnd->max.y)+(planes[n].c*bnd->min.z)+planes[n].d)>0.0f) continue;
		if (((planes[n].a*bnd->max.x)+(planes[n].b*bnd->max.y)+(planes[n].c*bnd->min.z)+planes[n].d)>0.0f) continue;
		if (((planes[n].a*bnd->min.x)+(planes[n].b*bnd->min.y)+(planes[n].c*bnd->max.z)+planes[n].d)>0.0f) continue;
		if (((planes[n].a*bnd->max.x)+(planes[n].b*bnd->min.y)+(planes[n].c*bnd->max.z)+planes[n].d)>0.0f) continue;
		if (((planes[n].a*bnd->min.x)+(planes[n].b*bnd->max.y)+(planes[n].c*bnd->max.z)+planes[n].d)>0.0f) continue;
		if (((planes[n].a*bnd->max.x)+(planes[n].b*bnd->max.y)+(planes[n].c*bnd->max.z)+planes[n].d)>0.0f) continue;
		return(FALSE);
	}

	return(TRUE);
}

/* =======================================================

      Normal Culling
      
======================================================= */

bool ray_precalc_normal_cull(ray_scene_type *scene,ray_draw_scene_thread_info *thread_info,ray_mesh_type *mesh,ray_poly_type *poly)
{
	float					wid,high;
	ray_point_type			pnt;
	ray_vector_type			eye_vct;
	
		// skip meshes without normals
		
	if (mesh->normal_block.count==0) return(TRUE);
	
		// get midpoint of thread box
		
	wid=(float)(scene->buffer.wid>>1);
	high=(float)(scene->buffer.high>>1);

	pnt.x=(scene->eye.pnt.x-wid)+((thread_info->pixel_start.x+thread_info->pixel_start.x)>>1);
	pnt.y=(scene->eye.pnt.y-high)+((thread_info->pixel_start.y+thread_info->pixel_start.y)>>1);
	pnt.z=scene->eye.pnt.z+scene->eye.min_dist;
	
	eye_vct.x=poly->bound.mid.x-pnt.x;
	eye_vct.y=poly->bound.mid.y-pnt.y;
	eye_vct.z=poly->bound.mid.z-pnt.z;

		// run the dot product against
		// the poly normal
		
	return(((poly->surface_normal.x*eye_vct.x)+(poly->surface_normal.y*eye_vct.y)+(poly->surface_normal.z*eye_vct.z))<=0.0f);
}

/* =======================================================

      Precalc For Scene Rendering

	  Mesh in scene list
	  Includes a mesh/light cross collision list
	  Mipmap poly cache
      
======================================================= */

void ray_precalc_render_scene_setup(ray_scene_type *scene)
{
	int				n,k,t,mesh_idx;
	float			d;
	double			dx,dy,dz;
	ray_mesh_type	*mesh;
	ray_poly_type	*poly;
	ray_light_type	*light;

		// create a list of meshes within
		// the eye max_dist, centered around
		// the eye point.  These are the drawing
		// meshes
		
	scene->draw_mesh_index_block.count=0;

	for (n=0;n!=scene->mesh_list.count;n++) {
		mesh=scene->mesh_list.meshes[n];
		if (mesh->hidden) continue;

		dx=(mesh->bound.mid.x-scene->eye.pnt.x);
		dy=(mesh->bound.mid.y-scene->eye.pnt.y);
		dz=(mesh->bound.mid.z-scene->eye.pnt.z);

		d=(float)sqrt((dx*dx)+(dy*dy)+(dz*dz));
		if (d<=scene->eye.max_dist) {
			scene->draw_mesh_index_block.indexes[scene->draw_mesh_index_block.count]=n;
			scene->draw_mesh_index_block.count++;
		}
	}
	
		// clear collide light list and setup
		// poly mipmap level cahce
		
	for (n=0;n!=scene->draw_mesh_index_block.count;n++) {

		mesh_idx=scene->draw_mesh_index_block.indexes[n];
		mesh=scene->mesh_list.meshes[mesh_idx];
		
		mesh->collide_lights_list.count=0;

		for (k=0;k!=mesh->poly_block.count;k++) {
			mesh->poly_block.polys[k].mm_level=-1;
		}
	}
	
	for (n=0;n!=scene->light_list.count;n++) {
		scene->light_list.lights[n]->collide_meshes_list.count=0;
	}
	
		// find the cross collisions
		// between lights and meshes
		// we do a quick elimination of
		// non light trace blocking meshes
		// and hidden meshes
		
	for (n=0;n!=scene->draw_mesh_index_block.count;n++) {

		mesh_idx=scene->draw_mesh_index_block.indexes[n];
		mesh=scene->mesh_list.meshes[mesh_idx];
		

		for (k=0;k!=scene->light_list.count;k++) {
			light=scene->light_list.lights[k];
			if (light->hidden) continue;
			
			if (ray_bound_bound_collision(&mesh->bound,&light->bound)) {

					// add this mesh to the light
					// collision list

				if ((mesh->flags&RL_MESH_FLAG_NON_LIGHT_TRACE_BLOCKING)==0x0) {
				
					if (light->collide_meshes_list.count<ray_max_mesh_per_light) {
						light->collide_meshes_list.indexes[light->collide_meshes_list.count]=mesh_idx;
						light->collide_meshes_list.count++;
					}

						// determine the polys the light
						// can hit

					poly=mesh->poly_block.polys;

					for (t=0;t!=mesh->poly_block.count;t++) {
						poly->light_render_mask[k]=ray_bound_bound_collision(&poly->bound,&light->bound)?0x1:0x0;
						poly++;
					}
				}

					// add this light to the mesh
					// collision list

				if (mesh->collide_lights_list.count<ray_max_light_per_mesh) {
					mesh->collide_lights_list.indexes[mesh->collide_lights_list.count]=k;
					mesh->collide_lights_list.count++;
				}
			}
		}
	}
}

/* =======================================================

      Precalc For Thread Mesh Rendering

	  This includes a smaller mesh list for the
	  initial meshes available for collisions in
	  the thread's pixel block
	  
	  If the ray bounces, we retreat to the
	  meshes in the larger scene list
      
======================================================= */

void ray_precalc_render_scene_thread_setup(ray_scene_type *scene,ray_draw_scene_thread_info *thread_info)
{
	int						n,k,mesh_idx;
	ray_mesh_type			*mesh;
	ray_poly_type			*poly;
	ray_plane_type			planes[6];
	
		// build the frustum planes
		// for this chunk of rendering

	ray_precalc_build_frustum_planes(scene,thread_info,planes);

		// build the mesh list for this thread
		// we use this one first hits, but then
		// retreat to the main list for bounces

	thread_info->draw_mesh_index_block.count=0;
	
	for (n=0;n!=scene->draw_mesh_index_block.count;n++) {
	
		mesh_idx=scene->draw_mesh_index_block.indexes[n];
		mesh=scene->mesh_list.meshes[mesh_idx];
		
			// special knock-out flags
			
		if (mesh->hidden) continue;
		if ((mesh->flags&RL_MESH_FLAG_NON_RAY_TRACE_BLOCKING)!=0) continue;
		
			// bound collisions

		if (!ray_precalc_frustum_plane_bound_cull(planes,&mesh->bound)) continue;

			// insert into list
			
		thread_info->draw_mesh_index_block.indexes[thread_info->draw_mesh_index_block.count]=mesh_idx;
		thread_info->draw_mesh_index_block.count++;

			// set up the poly rendering flags
			// for this thread, which is a list of which
			// polys can be seen from the frustum of
			// this thread and aren't culled by the poly
			// normal

		poly=mesh->poly_block.polys;

		for (k=0;k!=mesh->poly_block.count;k++) {
		
			if (!ray_precalc_frustum_plane_bound_cull(planes,&poly->bound)) {
				poly->thread_render_mask[thread_info->idx]=0x0;
			}
			else {
				if (!ray_precalc_normal_cull(scene,thread_info,mesh,poly)) {
					poly->thread_render_mask[thread_info->idx]=0x0;
				}
				else {
					poly->thread_render_mask[thread_info->idx]=0x1;
				}
			}
			
			poly++;
		}
	}
}

