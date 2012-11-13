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

      Precalculate Polygon Bounds and Culling Info
      
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

      Precalc For Scene Rendering

	  Mesh in scene list
	  Includes a mesh/light cross collision list
	  Mipmap poly cache
      
======================================================= */

void ray_precalc_render_scene_setup(ray_scene_type *scene)
{
	int				n,k,mesh_idx;
	float			d;
	double			dx,dy,dz;
	ray_mesh_type	*mesh;
	ray_light_type	*light;

		// create a list of meshes within
		// the eye max_dist, centered around
		// the eye point.  These are the drawing
		
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
	
		// clear masks and setup
		// poly mipmap level cahce
		
	for (n=0;n!=scene->draw_mesh_index_block.count;n++) {

		mesh_idx=scene->draw_mesh_index_block.indexes[n];
		mesh=scene->mesh_list.meshes[mesh_idx];

		memset(mesh->light_collide_mask,0x0,ray_max_scene_light);

		for (k=0;k!=mesh->poly_block.count;k++) {
			mesh->poly_block.polys[k].mm_level=-1;
		}
	}
	
	for (n=0;n!=scene->light_list.count;n++) {
		memset(scene->light_list.lights[n]->mesh_collide_mask,0x0,ray_max_scene_mesh);
	}
	
		// find the cross collisions
		// we do a quick elimination of
		// non light trace blocking meshes
		// and hidden meshes
		
	for (n=0;n!=scene->draw_mesh_index_block.count;n++) {

		mesh_idx=scene->draw_mesh_index_block.indexes[n];
		mesh=scene->mesh_list.meshes[mesh_idx];
		
		for (k=0;k!=scene->light_list.count;k++) {
			light=scene->light_list.lights[k];
			
			if (ray_bound_bound_collision(&mesh->bound,&light->bound)) {
				if ((mesh->flags&RL_MESH_FLAG_NON_LIGHT_TRACE_BLOCKING)==0x0) light->mesh_collide_mask[n]=0x1;
				mesh->light_collide_mask[k]=0x1;
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
	  meshes in scene list, as this list is created
	  by intersecting the original ray box
      
======================================================= */

void ray_precalc_render_scene_thread_setup(ray_scene_type *scene,ray_draw_scene_thread_info *thread_info)
{
	int						n,mesh_idx;
	float					x,y,z,lx,rx,ty,by,wid,high;
	ray_point_type			*eye_point;
	ray_point_type			view_plane_point;
	ray_vector_type			eye_vector[4];
	ray_bound_type			bound;
	ray_mesh_type			*mesh;
	
		// get 2D drawing sizes
		
	lx=(float)thread_info->pixel_start.x;
	rx=(float)thread_info->pixel_end.x;
	ty=(float)thread_info->pixel_start.y;
	by=(float)thread_info->pixel_end.y;
	
	wid=(float)(scene->buffer.wid>>1);
	high=(float)(scene->buffer.high>>1);
		
		// get four corners of
		// thread bounds
		
	eye_point=&scene->eye.pnt;

	view_plane_point.z=eye_point->z+scene->eye.min_dist;

	view_plane_point.x=(eye_point->x-wid)+lx;
	view_plane_point.y=(eye_point->y-high)+ty;
	ray_vector_create_from_points(&eye_vector[0],&view_plane_point,eye_point);
	rlMatrixVectorMultiply(&scene->eye.matrix,&eye_vector[0]);

	view_plane_point.x=(eye_point->x-wid)+lx;
	view_plane_point.y=(eye_point->y-high)+by;
	ray_vector_create_from_points(&eye_vector[1],&view_plane_point,eye_point);
	rlMatrixVectorMultiply(&scene->eye.matrix,&eye_vector[1]);

	view_plane_point.x=(eye_point->x-wid)+rx;
	view_plane_point.y=(eye_point->y-high)+ty;
	ray_vector_create_from_points(&eye_vector[2],&view_plane_point,eye_point);
	rlMatrixVectorMultiply(&scene->eye.matrix,&eye_vector[2]);

	view_plane_point.x=(eye_point->x-wid)+rx;
	view_plane_point.y=(eye_point->y-high)+by;
	ray_vector_create_from_points(&eye_vector[3],&view_plane_point,eye_point);
	rlMatrixVectorMultiply(&scene->eye.matrix,&eye_vector[3]);
	
		// create bound
		
	bound.min.x=bound.max.x=eye_point->x;
	bound.min.y=bound.max.y=eye_point->y;
	bound.min.z=bound.max.z=eye_point->z;
		
	for (n=0;n!=4;n++) {
		ray_vector_normalize(&eye_vector[n]);

		x=eye_point->x+(eye_vector[n].x*scene->eye.max_dist);
		if (x<bound.min.x) bound.min.x=x;
		if (x>bound.max.x) bound.max.x=x;
		
		y=eye_point->y+(eye_vector[n].y*scene->eye.max_dist);
		if (y<bound.min.y) bound.min.y=y;
		if (y>bound.max.y) bound.max.y=y;
		
		z=eye_point->z+(eye_vector[n].z*scene->eye.max_dist);
		if (z<bound.min.z) bound.min.z=z;
		if (z>bound.max.z) bound.max.z=z;
	}
	
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
			
		if (!ray_bound_bound_collision(&bound,&mesh->bound)) continue;

			// insert into list
			
		thread_info->draw_mesh_index_block.indexes[thread_info->draw_mesh_index_block.count]=mesh_idx;
		thread_info->draw_mesh_index_block.count++;
	}
}

