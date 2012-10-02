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
	
	pnt=&mesh->vertex_block.vertexes[poly->vertex_idx[0]];

	bnd->min.x=bnd->max.x=pnt->x;
	bnd->min.y=bnd->max.y=pnt->y;
	bnd->min.z=bnd->max.z=pnt->z;
	
	pnt++;
	
	for (n=1;n<poly->nvertex;n++) {
		pnt=&mesh->vertex_block.vertexes[poly->vertex_idx[n]];
		
		if (pnt->x<bnd->min.x) bnd->min.x=pnt->x;
		if (pnt->x>bnd->max.x) bnd->max.x=pnt->x;

		if (pnt->y<bnd->min.y) bnd->min.y=pnt->y;
		if (pnt->y>bnd->max.y) bnd->max.y=pnt->y;

		if (pnt->z<bnd->min.z) bnd->min.z=pnt->z;
		if (pnt->z>bnd->max.z) bnd->max.z=pnt->z;
		
		pnt++;
	}
}

/* =======================================================

      Precalculate Triangle Bounds
      
======================================================= */

void ray_precalc_triangle_bounds(ray_mesh_type *mesh,ray_trig_type *trig)
{
	ray_point_type		*p0,*p1,*p2;
	ray_bound_type		*bnd;
	
		// get the points
		
	p0=&mesh->vertex_block.vertexes[trig->vertex_idx[0]];
	p1=&mesh->vertex_block.vertexes[trig->vertex_idx[1]];
	p2=&mesh->vertex_block.vertexes[trig->vertex_idx[2]];

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
		
	p0=&mesh->vertex_block.vertexes[trig->vertex_idx[0]];
	p1=&mesh->vertex_block.vertexes[trig->vertex_idx[1]];
	p2=&mesh->vertex_block.vertexes[trig->vertex_idx[2]];

		// precalc the vectors sharing
		// triangle around vertex 0
		
	ray_vector_create_from_points(&trig->v1,p1,p0);
	ray_vector_create_from_points(&trig->v2,p2,p0);
}

/* =======================================================

      Build Mesh/Bound Intersection
      
======================================================= */

void ray_precalc_bound_mesh_indexes(ray_scene_type *scene,ray_bound_type *bnd,ray_mesh_index_block *index_block,bool ignore_skip_light)
{
	int					n;
	short				*idx;
	ray_mesh_type		*mesh;
	
	index_block->count=0;
	idx=index_block->indexes;
	
	for (n=0;n!=scene->mesh_list.count;n++) {
		mesh=scene->mesh_list.meshes[n];
		
			// special knock-out flags
			
		if (mesh->hidden) continue;
		if ((ignore_skip_light) && ((mesh->flags&RL_MESH_FLAG_NON_LIGHT_BLOCKING)!=0)) continue;
		
			// bound collisions
			
		//if (!ray_bound_bound_collision(bnd,&mesh->bound)) continue;
		
		*idx++=n;
		index_block->count++;
	}
}

/* =======================================================

      Build Light Mesh Indexes
      
======================================================= */

void ray_precalc_light_mesh_indexes_all(ray_scene_type *scene)
{
	int				n;
	ray_light_type	*light;
	
	for (n=0;n!=scene->light_list.count;n++) {
		light=scene->light_list.lights[n];
		ray_precalc_bound_mesh_indexes(scene,&light->bound,&light->mesh_index_block,TRUE);
	}
}

/* =======================================================

      Build Thread Mesh Indexes
 
======================================================= */

void ray_precalc_thread_mesh_indexes_all(ray_scene_type *scene,ray_draw_scene_thread_info *thread_info)
{
	int						n;
	float					x,y,z,ty,by,wid,high;
	ray_point_type			*eye_point;
	ray_point_type			view_plane_point;
	ray_vector_type			eye_vector[4];
	ray_bound_type			bound;
	
	
		// get 2D drawing sizes
		
	ty=(float)thread_info->y_start;
	by=(float)thread_info->y_end;
	
	wid=(float)(scene->buffer.wid>>1);
	high=(float)(scene->buffer.high>>1);
		
		// get four corners of
		// thread bounds
		
	eye_point=&scene->eye.pnt;

	view_plane_point.z=eye_point->z+scene->eye.min_dist;

	view_plane_point.x=eye_point->x-wid;
	view_plane_point.y=(eye_point->y-high)+ty;
	ray_vector_create_from_points(&eye_vector[0],&view_plane_point,eye_point);
	rlMatrixVectorMultiply(&scene->eye.matrix,&eye_vector[0]);

	view_plane_point.x=eye_point->x-wid;
	view_plane_point.y=(eye_point->y-high)+by;
	ray_vector_create_from_points(&eye_vector[1],&view_plane_point,eye_point);
	rlMatrixVectorMultiply(&scene->eye.matrix,&eye_vector[1]);

	view_plane_point.x=eye_point->x+wid;
	view_plane_point.y=(eye_point->y-high)+ty;
	ray_vector_create_from_points(&eye_vector[2],&view_plane_point,eye_point);
	rlMatrixVectorMultiply(&scene->eye.matrix,&eye_vector[2]);

	view_plane_point.x=eye_point->x+wid;
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
		
		// create bound intersections
		
	ray_precalc_bound_mesh_indexes(scene,&bound,&thread_info->mesh_index_block,FALSE);
}

