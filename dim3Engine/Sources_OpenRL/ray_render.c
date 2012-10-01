#include "ray_os_defs.h"
#include "ray.h"
#include "ray_interface.h"

ray_global_type					ray_global;

/* =======================================================

      Ray/Triangle Intersections
      
======================================================= */

bool ray_intersect_triangle(ray_scene_type *scene,ray_point_type *eye_point,ray_vector_type *eye_vector,ray_mesh_type *mesh,ray_trig_type *trig,float *ptr_t,float *ptr_u,float *ptr_v)
{
	float				det,invDet,t,u,v;
	ray_vector_type		perpVector,lineToTrigPointVector,lineToTrigPerpVector;
	ray_point_type		*p0;
	
		// calculate the determinate

	ray_vector_cross_product(&perpVector,eye_vector,&trig->v2);
	det=ray_vector_dot_product(&trig->v1,&perpVector);
	
		// is line on the same plane as triangle?
		
	if ((det>-0.00001f) && (det<0.00001f)) return(FALSE);

		// get the inverse determinate

	invDet=1.0f/det;

		// calculate triangle U and test
	
	p0=&mesh->vertex_block.vertexes[trig->vertex_idx[0]];
	
	ray_vector_create_from_points(&lineToTrigPointVector,eye_point,p0);
	u=invDet*ray_vector_dot_product(&lineToTrigPointVector,&perpVector);
	if ((u<0.0f) || (u>1.0f)) return(FALSE);
	
		// calculate triangle V and test

	ray_vector_cross_product(&lineToTrigPerpVector,&lineToTrigPointVector,&trig->v1);
	v=invDet*ray_vector_dot_product(eye_vector,&lineToTrigPerpVector);
	if ((v<0.0f) || ((u+v)>1.0f)) return(FALSE);
	
		// get line T for point(t) = eye_point + (eye_vector*t)
		// -t are on the negative vector behind the point, so ignore

	t=invDet*ray_vector_dot_product(&trig->v2,&lineToTrigPerpVector);
	if (t<0.0f) return(FALSE);
	
		// return t and uv values
		
	*ptr_t=t;
	*ptr_u=u;
	*ptr_v=v;
	
	return(TRUE);
}

/* =======================================================

      Ray Intersects Mesh List
      
======================================================= */

// supergumba -- find so way to use T to eliminate meshes that are further
// out than the original hit point, and maybe sort the meshes (that might be too costly)

void ray_intersect_mesh_list(ray_scene_type *scene,ray_point_type *eye_point,ray_vector_type *eye_vector,ray_vector_type *eye_normal,ray_mesh_index_block *index_block,ray_collision_type *collision)
{
	int					n,k,i,mesh_idx;
	float				it,iu,iv;
	ray_mesh_type		*mesh;
	ray_poly_type		*poly;
	ray_trig_type		*trig;
	
		// clear collision
	
	collision->mesh_idx=-1;
	collision->poly_idx=-1;
	collision->trig_idx=-1;

	collision->t=collision->max_t;
	
		// we need a normalized
		// eye vector

	for (n=0;n!=index_block->count;n++) {
	
		mesh_idx=index_block->indexes[n];
		mesh=scene->mesh_list.meshes[mesh_idx];
	
			// bounds check
			
		if (!ray_bound_ray_collision(eye_point,eye_vector,&mesh->bound)) continue;

			// check the polys
			
		for (k=0;k!=mesh->poly_block.count;k++) {
		
				// bounds check
				
			poly=&mesh->poly_block.polys[k];
			if (!ray_bound_ray_collision(eye_point,eye_vector,&poly->bound)) continue;
			
				// check trigs
				
			for (i=0;i!=poly->trig_block.count;i++) {
			
				trig=&poly->trig_block.trigs[i];
				if (!ray_bound_ray_collision(eye_point,eye_vector,&trig->bound)) continue;
				
					// supergumba -- can we figure out earlier triangles quicker here?
					
					// check triangle/ray intersection
					// first hit exits out of polygons as you
					// can only hit one triangle
				
				if (ray_intersect_triangle(scene,eye_point,eye_vector,mesh,trig,&it,&iu,&iv)) {
					if (it<collision->t) {
						collision->t=it;
						collision->u=iu;
						collision->v=iv;
						collision->mesh_idx=mesh_idx;
						collision->poly_idx=k;
						collision->trig_idx=i;
						break;
					}
				}
			}
		}
	}
}

/* =======================================================

      Ray Blocks
      
======================================================= */

bool ray_block_mesh_list(ray_scene_type *scene,ray_point_type *pnt,ray_vector_type *vct,ray_vector_type *vct_normal,ray_mesh_index_block *index_block,ray_collision_type *collision)
{
	int					n,k,i,mesh_idx;
	float				t,u,v;
	ray_mesh_type		*mesh;
	ray_poly_type		*poly;
	ray_trig_type		*trig;
	
	for (n=0;n!=index_block->count;n++) {
	
			// indexes in this mesh list have
			// already been pared down non-render
			// and non-light blocking
			
		mesh_idx=index_block->indexes[n];
		mesh=scene->mesh_list.meshes[mesh_idx];
		
			// bounds check
			
		if (!ray_bound_ray_collision(pnt,vct,&mesh->bound)) continue;

			// run through the polys
			
		for (k=0;k!=mesh->poly_block.count;k++) {
		
			poly=&mesh->poly_block.polys[k];
			if (!ray_bound_ray_collision(pnt,vct,&poly->bound)) continue;
			
				// skip self
				
			if ((collision->mesh_idx==mesh_idx) && (collision->poly_idx==k)) continue;
			
				// check trigs
				
			for (i=0;i!=poly->trig_block.count;i++) {
			
				trig=&poly->trig_block.trigs[i];
				if (!ray_bound_ray_collision(pnt,vct,&trig->bound)) continue;

					// check for intersection, but
					// only except t that is less 1.0f, otherwise
					// we've gone past the light
					
				if (ray_intersect_triangle(scene,pnt,vct,mesh,trig,&t,&u,&v)) {
					if (t<1.0f) return(TRUE);
				}
			}
		}
	}

	return(FALSE);
}

/* =======================================================

      Special Mesh Lighting
      
======================================================= */

bool ray_mesh_special_lighting_conditions(ray_scene_type *scene,int mesh_idx,ray_collision_type *collision,ray_color_type *pixel_col)
{
	ray_mesh_type			*mesh;
	ray_material_pixel_type	material_pixel;

	mesh=scene->mesh_list.meshes[mesh_idx];

		// highlighting

	if ((mesh->flags&RL_MESH_FLAG_HIGHLIGHT)!=0) {
		ray_get_material_rgb(scene,collision,&material_pixel);
		pixel_col->r=material_pixel.color.rgb.r;
		pixel_col->g=material_pixel.color.rgb.g;
		pixel_col->b=material_pixel.color.rgb.b;
		return(TRUE);
	}

	return(FALSE);
}

/* =======================================================

      Lighting Tracing
      
======================================================= */

bool ray_trace_lights(ray_scene_type *scene,ray_point_type *pnt,ray_vector_type *eye_normal,ray_collision_type *collision,ray_color_type *col)
{
	int							n;
	float						dist,att,diffuse,spec_factor;
	bool						hit;
	ray_material_pixel_type		material_pixel;
	ray_color_type				light_col,spec_col;
	ray_vector_type				light_vector,light_normal,
								surface_normal,half_vector;
	ray_light_type				*light;
	
	col->r=col->g=col->b=0.0f;
	
		// get material pixels
		
	ray_get_material_rgb(scene,collision,&material_pixel);
	
		// if this material has a normal
		// map, precalc the surface normals
		
	if (material_pixel.normal.on) {
		surface_normal.x=material_pixel.surface_normal.x+material_pixel.normal.rgb.r;
		surface_normal.y=material_pixel.surface_normal.y+material_pixel.normal.rgb.g;
		surface_normal.z=material_pixel.surface_normal.z+material_pixel.normal.rgb.b;
		ray_vector_normalize(&surface_normal);
	}
	
		// defualt to no specular
		
	spec_col.r=0.0f;
	spec_col.g=0.0f;
	spec_col.b=0.0f;

		// find lights

	hit=FALSE;
	
	for (n=0;n!=scene->light_list.count;n++) {
		light=scene->light_list.lights[n];

			// outside of intensity globe?

		dist=ray_distance_between_points(pnt,&light->pnt);
		if (dist>light->intensity) continue;
		
			// get light vector
			// and normalize version for
			// effects and culling
			
		ray_vector_create_from_points(&light_vector,&light->pnt,pnt);
		
		light_normal.x=light_vector.x;
		light_normal.y=light_vector.y;
		light_normal.z=light_vector.z;
		ray_vector_normalize(&light_normal);
	
			// check for mesh collides
			// blocking light

		if (ray_block_mesh_list(scene,pnt,&light_vector,&light_normal,&light->mesh_index_block,collision)) continue;

			// attenuate the light for distance

		att=1.0f-(dist/light->intensity);

		light_col.r=light->col.r*att;
		light_col.g=light->col.g*att;
		light_col.b=light->col.b*att;

			// add in the exponent
			
		att=powf(att,light->exponent);
		
			// handle any diffuse calcs
			// it will either be the surface normal
			// and light normal, or the surface normal
			// plus the normal map and the light normal
			
		if (material_pixel.normal.on) {
			diffuse=ray_vector_dot_product(&light_normal,&surface_normal);
			
				// get the spec
				// specular calculations require the bump maps
				// we need the half vector (between eye and light)
				// to calculate this
				
			if (material_pixel.specular.on) {
				half_vector.x=eye_normal->x+light_normal.x;
				half_vector.y=eye_normal->y+light_normal.y;
				half_vector.z=eye_normal->z+light_normal.z;
				ray_vector_normalize(&half_vector);
			
				spec_factor=ray_vector_dot_product(&light_normal,(ray_vector_type*)&material_pixel.normal.rgb);
				if (spec_factor<0.0f) spec_factor=0.0f;
				spec_factor=powf(spec_factor,material_pixel.shine_factor);
				spec_factor*=att;
				
				if (spec_factor>1.0f) spec_factor=1.0f;
				
				spec_col.r=material_pixel.specular.rgb.r*spec_factor;
				spec_col.g=material_pixel.specular.rgb.g*spec_factor;
				spec_col.b=material_pixel.specular.rgb.b*spec_factor;
			}
		}
		else {
			diffuse=ray_vector_dot_product(&light_normal,&material_pixel.surface_normal);
		}
	
		diffuse=(diffuse+1.0f)*0.5f;
		
			// mix with material and
			// add to pixel
			
		col->r+=(material_pixel.color.rgb.r*(light_col.r*diffuse))+spec_col.r;
		col->g+=(material_pixel.color.rgb.g*(light_col.g*diffuse))+spec_col.g;
		col->b+=(material_pixel.color.rgb.b*(light_col.b*diffuse))+spec_col.b;
				
			// we hit a light

		hit=TRUE;
	}
	
	return(hit);
}

/* =======================================================

      Set Draw Buffer
      
======================================================= */

void ray_set_buffer(unsigned long *buf,ray_color_type *pixel_col,ray_color_type *overlay_col)
{
	float			inv_a;
	
		// add in the overlay
		
	if (overlay_col!=NULL) {
		inv_a=1.0f-overlay_col->a;
		pixel_col->r=(pixel_col->r*inv_a)+(overlay_col->r*overlay_col->a);
		pixel_col->g=(pixel_col->g*inv_a)+(overlay_col->g*overlay_col->a);
		pixel_col->b=(pixel_col->b*inv_a)+(overlay_col->b*overlay_col->a);
	}
	
		// clamp
		
	if (pixel_col->r>1.0f) pixel_col->r=1.0f;
	if (pixel_col->g>1.0f) pixel_col->g=1.0f;
	if (pixel_col->b>1.0f) pixel_col->b=1.0f;

		// add to buffer
		
	*buf=ray_create_ulong_color_from_float_no_alpha(pixel_col);
}
	
/* =======================================================

      Ray Rendering Main Thread
      
======================================================= */

#ifndef WIN32
void* ray_render_thread(void *arg)
#else
void ray_render_thread(void *arg)
#endif
{
	int							x,y,xsz,y_start,y_end;
	float						xadd,yadd,zadd;
	unsigned long				ambient_ulong;
	unsigned long				*buf;
	ray_point_type				*eye_point,trig_point,view_plane_point;
	ray_vector_type				eye_vector,eye_normal;
	ray_color_type				pixel_col,overlay_col;
	ray_collision_type			collision;
	ray_draw_scene_thread_info	*thread_info;
	ray_scene_type				*scene;

		// get the render info

	thread_info=(ray_draw_scene_thread_info*)arg;
	scene=thread_info->parent_scene;

		// ambient color

	ambient_ulong=ray_create_ulong_black();
	
		// get eye point
		
	eye_point=&scene->eye.pnt;
	
		// get 2D drawing sizes
		
	y_start=thread_info->y_start;
	y_end=thread_info->y_end;

	xsz=scene->buffer.wid;
		
		// eye point movement

	xadd=eye_point->x-(float)(scene->buffer.wid>>1);
	yadd=eye_point->y-(float)(scene->buffer.high>>1);
	zadd=eye_point->z+scene->eye.min_dist;
	
		// setup the precalced mesh indexes
		// that are used to reduce the number of meshes
		// we have to check
		
	ray_precalc_thread_mesh_indexes_all(scene,thread_info);
	
		// the collision struct
		
	collision.max_t=scene->eye.max_dist;
	
		// draw
		
	for (y=y_start;y!=y_end;y++) {
	
		for (x=0;x!=xsz;x++) {
		
				// determine if in overlay
				// do an early exit if no alpha
				
			if (ray_scene_overlay_get_pixel(scene,x,y,&overlay_col)) {
				if (overlay_col.a==1.0f) {
					buf=scene->buffer.data+((y*scene->buffer.wid)+x);		// buffer is unsigned long
					ray_set_buffer(buf,&overlay_col,NULL);
					continue;
				}
			}
			else {
				overlay_col.r=overlay_col.g=overlay_col.b=1.0f;
				overlay_col.a=0.0f;
			}
		
				// get eye vector
				
			view_plane_point.x=xadd+(float)x;
			view_plane_point.y=yadd+(float)y;
			view_plane_point.z=zadd;
			
			ray_vector_create_from_points(&eye_vector,&view_plane_point,eye_point);
			
				// need normalized eye vector
				// for culling and effects
				
			eye_normal.x=eye_vector.x;
			eye_normal.y=eye_vector.y;
			eye_normal.z=eye_vector.z;
			ray_vector_normalize(&eye_normal);

				// rotate eye vector around eye
				
			rlMatrixVectorMultiply(&scene->eye.matrix,&eye_vector);

				// find nearest mesh-trig intersection
				
			ray_intersect_mesh_list(scene,eye_point,&eye_vector,&eye_normal,&thread_info->mesh_index_block,&collision);
			if (collision.trig_idx==-1) continue;
			
				// get buffer
				
			buf=scene->buffer.data+((y*scene->buffer.wid)+x);		// buffer is unsigned long

				// check for special lighting conditions

			if (ray_mesh_special_lighting_conditions(scene,collision.mesh_idx,&collision,&pixel_col)) {
				ray_set_buffer(buf,&pixel_col,&overlay_col);
				continue;
			}
			
				// run regular lighting

			ray_vector_find_line_point_for_T(eye_point,&eye_vector,collision.t,&trig_point);
			if (ray_trace_lights(scene,&trig_point,&eye_normal,&collision,&pixel_col)) {
				ray_set_buffer(buf,&pixel_col,&overlay_col);
				continue;
			}
			
				// otherwise buffer is
				// just the ambient
				
			*buf=ambient_ulong;
		}
	}

		// mark this thread as finished

	thread_info->done=TRUE;

		// end the thread

#ifndef WIN32
	pthread_exit(0);
	return(0);
#else
	_endthread();
#endif
}

/* =======================================================

      Thread Utilities
      
======================================================= */

void ray_render_clear_threads(ray_scene_type *scene)
{
	int				n;

	for (n=0;n!=ray_global.settings.thread_count;n++) {
		scene->render.thread_info[n].done=TRUE;
	}
}

bool ray_render_check_threads_done(ray_scene_type *scene)
{
	int				n;

	for (n=0;n!=ray_global.settings.thread_count;n++) {
		if (!scene->render.thread_info[n].done) return(FALSE);
	}

	return(TRUE);
}

/* =======================================================

      Render a Scene

 	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
	   RL_ERROR_SCENE_IN_USE
     
======================================================= */

int rlSceneRender(int sceneId)
{
	int						n,idx;
	ray_scene_type			*scene;
#ifndef WIN32
	pthread_t				thread;
#endif

		// get the scene

	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);

	scene=ray_global.scene_list.scenes[idx];

		// we need to keep this next
		// section locked so the in use flags
		// don't stomp on each other

#ifndef WIN32
	pthread_mutex_lock(&scene->render.lock);
#else
	WaitForSingleObject(scene->render.lock,INFINITE);
#endif

		// can not render if we are already rendering

	if (!ray_render_check_threads_done(scene)) {

		#ifndef WIN32
			pthread_mutex_unlock(&scene->render.lock);
		#else
			ReleaseMutex(scene->render.lock);
		#endif

		return(RL_ERROR_SCENE_IN_USE);
	}

		// setup the render info
		// do this here so mutex isn't
		// held as long

	for (n=0;n!=ray_global.settings.thread_count;n++) {
		scene->render.thread_info[n].done=FALSE;
	}

		// unlock rendering lock

#ifndef WIN32
	pthread_mutex_unlock(&scene->render.lock);
#else
	ReleaseMutex(scene->render.lock);
#endif

		// setup the precalced light mesh indexes
		// that are used to reduce the number of meshes
		// we have to check
		
	ray_precalc_light_mesh_indexes_all(scene);

		// run rendering in threads
		
	for (n=0;n!=ray_global.settings.thread_count;n++) {

		#ifndef WIN32
			pthread_create(&thread,0,ray_render_thread,(void*)&scene->render.thread_info[n]);
			pthread_detach(thread);
		#else
			_beginthread(ray_render_thread,0,(void*)&scene->render.thread_info[n]);
		#endif

	}

	return(RL_ERROR_OK);
}

/* =======================================================

      Check Rendering State

 	  Returns:
	   RL_SCENE_STATE_IDLE
	   RL_SCENE_STATE_RENDERING
	   RL_ERROR_UNKNOWN_SCENE_ID
     
======================================================= */

int rlSceneRenderState(int sceneId)
{
	int						idx;

		// get the scene

	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);

		// check rendering state

	if (ray_render_check_threads_done(ray_global.scene_list.scenes[idx])) return(RL_SCENE_STATE_IDLE);

	return(RL_SCENE_STATE_RENDERING);
}
