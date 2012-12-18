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
	
	p0=&mesh->vertex_block.vertexes[trig->idxs[0].vertex];
	
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

void ray_intersect_mesh_list_initial(ray_scene_type *scene,ray_draw_scene_thread_info *thread_info,ray_point_type *eye_point,ray_vector_type *eye_vector,ray_collision_type *collision)
{
	int								n,mesh_idx,poly_idx,trig_idx;
	float							it,iu,iv;
	ray_point_type					trig_pnt;
	ray_mesh_type					*mesh;
	ray_poly_type					*poly;
	ray_trig_type					*trig;
	ray_collision_type				alpha_collision;
	
		// clear collision
		// anything > 1.0f is outside
		// the max eye distance
	
	collision->mesh_idx=-1;
	collision->poly_idx=-1;
	collision->trig_idx=-1;

	collision->t=1.0f;

		// run through the meshes
		
		// for initial collisions, we use the
		// thread mesh list, which has been pared
		// down for the thread's pixel box,
		// hidden, and non-ray trace blocking

	for (n=0;n!=thread_info->draw_mesh_index_block.count;n++) {

		mesh_idx=thread_info->draw_mesh_index_block.indexes[n];
		mesh=scene->mesh_list.meshes[mesh_idx];

			// mesh bounds check

		if (!ray_bound_ray_collision(eye_point,eye_vector,&mesh->bound)) continue;

			// run through the polys

		for (poly_idx=0;poly_idx!=mesh->poly_block.count;poly_idx++) {
			
				// bounds check

				// thread based lists have poly eliminations
				// in them
				
			poly=&mesh->poly_block.polys[poly_idx];
			if (poly->thread_render_mask[thread_info->idx]==0x0) continue;

			if (!ray_bound_ray_collision(eye_point,eye_vector,&poly->bound)) continue;
			
				// check triangle/ray intersection
				// we don't do bound checking as it's
				// about as fast as the intersection test
				// first hit exits out of polygons as you
				// can only hit one triangle of a polygon
				
			for (trig_idx=0;trig_idx!=poly->trig_block.count;trig_idx++) {
			
				trig=&poly->trig_block.trigs[trig_idx];
				
					// have we intersected this triangle
					// closer to the last hit?
					
				if (!ray_intersect_triangle(scene,eye_point,eye_vector,mesh,trig,&it,&iu,&iv)) continue;
				if (it>collision->t) continue;
				
					// special check for
					// alpha==0.0f, which is a skip

				if (!ray_global.material_list.materials[poly->material_idx]->no_alpha) {
					alpha_collision.t=it;
					alpha_collision.u=iu;
					alpha_collision.v=iv;
					alpha_collision.mesh_idx=mesh_idx;
					alpha_collision.poly_idx=poly_idx;
					alpha_collision.trig_idx=trig_idx;

					ray_vector_find_line_point_for_T(eye_point,eye_vector,it,&trig_pnt);
					if (ray_get_material_alpha(scene,eye_point,&trig_pnt,&alpha_collision)==0.0f) break;
				}

					// set the hit and exit out
					// of the loop
				
				collision->t=it;
				collision->u=iu;
				collision->v=iv;
				collision->mesh_idx=mesh_idx;
				collision->poly_idx=poly_idx;
				collision->trig_idx=trig_idx;

				break;
			}
		}
	}
}

void ray_intersect_mesh_list_bounce(ray_scene_type *scene,ray_draw_scene_thread_info *thread_info,ray_point_type *eye_point,ray_vector_type *eye_vector,ray_collision_type *collision)
{
	int								n,k,mesh_idx,poly_idx,trig_idx;
	float							it,iu,iv;
	bool							skip;
	ray_point_type					trig_pnt;
	ray_mesh_type					*mesh;
	ray_poly_type					*poly;
	ray_trig_type					*trig;
	ray_collision_type				alpha_collision;
	
		// clear collision
		// anything > 1.0f is outside
		// the max eye distance
	
	collision->mesh_idx=-1;
	collision->poly_idx=-1;
	collision->trig_idx=-1;

	collision->t=1.0f;
	
		// run through the meshes

		// for bounces, we need to retreat to the
		// main scene list because bounces could go
		// in any direction

	for (n=0;n!=scene->draw_mesh_index_block.count;n++) {

		mesh_idx=scene->draw_mesh_index_block.indexes[n];
		mesh=scene->mesh_list.meshes[mesh_idx];

			// main list isn't pared down
			// for flags, so we need to do that

		if ((mesh->flags&RL_MESH_FLAG_NON_BOUNCE_TRACE_BLOCKING)!=0) continue;

			// mesh bounds check

		if (!ray_bound_ray_collision(eye_point,eye_vector,&mesh->bound)) continue;

			// run through the polys

		for (poly_idx=0;poly_idx!=mesh->poly_block.count;poly_idx++) {
			
				// bounds check
				
			poly=&mesh->poly_block.polys[poly_idx];
			if (!ray_bound_ray_collision(eye_point,eye_vector,&poly->bound)) continue;
				
				// skiping polys, this is used
				// so reflections or pass throughs
				// so we don't re-hit any polys we've
				// already hit.  note this means we will
				// only bounce off a surface once but we
				// won't have problems with overlayed polygons
				
			skip=FALSE;
			
			for (k=0;k!=collision->skip_block.count;k++) {
				if ((mesh_idx==collision->skip_block.skips[k].mesh_idx) && (poly_idx==collision->skip_block.skips[k].poly_idx)) {
					skip=TRUE;
					break;
				}
			}
			
			if (skip) continue;
			
				// check triangle/ray intersection
				// we don't do bound checking as it's
				// about as fast as the intersection test
				// first hit exits out of polygons as you
				// can only hit one triangle of a polygon
				
			for (trig_idx=0;trig_idx!=poly->trig_block.count;trig_idx++) {
			
				trig=&poly->trig_block.trigs[trig_idx];
				
					// have we intersected this triangle
					// closer to the last hit?
					
				if (!ray_intersect_triangle(scene,eye_point,eye_vector,mesh,trig,&it,&iu,&iv)) continue;
				if (it>collision->t) continue;
				
					// special check for
					// alpha==0.0f, which is a skip

				if (!ray_global.material_list.materials[poly->material_idx]->no_alpha) {
					alpha_collision.t=it;
					alpha_collision.u=iu;
					alpha_collision.v=iv;
					alpha_collision.mesh_idx=mesh_idx;
					alpha_collision.poly_idx=poly_idx;
					alpha_collision.trig_idx=trig_idx;

					ray_vector_find_line_point_for_T(eye_point,eye_vector,it,&trig_pnt);
					if (ray_get_material_alpha(scene,eye_point,&trig_pnt,&alpha_collision)==0.0f) break;
				}

					// set the hit and exit out
					// of the loop
				
				collision->t=it;
				collision->u=iu;
				collision->v=iv;
				collision->mesh_idx=mesh_idx;
				collision->poly_idx=poly_idx;
				collision->trig_idx=trig_idx;

				break;
			}
		}
	}
}

/* =======================================================

      Ray Blocks
      
======================================================= */

bool ray_block_light(ray_scene_type *scene,ray_point_type *pnt,ray_vector_type *vct,ray_collision_type *collision,int light_idx)
{
	int					n,mesh_idx,poly_idx,trig_idx;
	float				t,u,v;
	ray_point_type		trig_pnt;
	ray_mesh_type		*mesh;
	ray_poly_type		*poly;
	ray_trig_type		*trig;
	ray_light_type		*light;
	ray_collision_type	lit_collision;
	
	light=scene->light_list.lights[light_idx];
	
	if (light->collide_meshes_list.count==0) return(FALSE);
	
			// check the list of possible
			// light to mesh collision

			// indexes in this mesh list have
			// already been pared down non-render,
			// non-hidden, and non-light blocking
			// and only things within the light cone
	
	for (n=0;n!=light->collide_meshes_list.count;n++) {

		mesh_idx=light->collide_meshes_list.indexes[n];
		mesh=scene->mesh_list.meshes[mesh_idx];

			// mesh bounds check

		if (!ray_bound_ray_collision(pnt,vct,&mesh->bound)) continue;

			// run through the polys

		for (poly_idx=0;poly_idx!=mesh->poly_block.count;poly_idx++) {

				// bound collision

			poly=&mesh->poly_block.polys[poly_idx];
			if (!ray_bound_ray_collision(pnt,vct,&poly->bound)) continue;
				
				// skip self
				
			if ((collision->mesh_idx==mesh_idx) && (collision->poly_idx==poly_idx)) continue;
			
				// check trigs
				
			for (trig_idx=0;trig_idx!=poly->trig_block.count;trig_idx++) {
			
				trig=&poly->trig_block.trigs[trig_idx];

					// check for intersection, but
					// only except t that is less 1.0f, otherwise
					// we've gone past the light
					
				if (!ray_intersect_triangle(scene,pnt,vct,mesh,trig,&t,&u,&v)) continue;
				if (t>=1.0f) continue;
				
					// if no alpha, then it's
					// automatically a blocking hit
				
				if (ray_global.material_list.materials[poly->material_idx]->no_alpha) return(TRUE);
				
					// setup the collision
					// so we can check the alpha

				lit_collision.t=t;
				lit_collision.u=u;
				lit_collision.v=v;
				lit_collision.mesh_idx=mesh_idx;
				lit_collision.poly_idx=poly_idx;
				lit_collision.trig_idx=trig_idx;

				ray_vector_find_line_point_for_T(pnt,vct,t,&trig_pnt);
				if (ray_get_material_alpha(scene,pnt,&trig_pnt,&lit_collision)==1.0f) return(TRUE);
			}
		}
	}

	return(FALSE);
}

/* =======================================================

      Special Mesh Lighting
      
======================================================= */

bool ray_mesh_special_lighting_conditions(ray_scene_type *scene,ray_point_type *eye_pnt,ray_point_type *trig_pnt,int mesh_idx,ray_collision_type *collision,ray_color_type *pixel_col)
{
	ray_mesh_type			*mesh;
	ray_material_pixel_type	material_pixel;

	mesh=scene->mesh_list.meshes[mesh_idx];

		// highlighting

	if ((mesh->flags&RL_MESH_FLAG_HIGHLIGHT)!=0) {
		ray_get_material_rgb(scene,eye_pnt,trig_pnt,collision,&material_pixel);
		pixel_col->r=material_pixel.color.rgb.r;
		pixel_col->g=material_pixel.color.rgb.g;
		pixel_col->b=material_pixel.color.rgb.b;
		pixel_col->a=material_pixel.color.rgb.a;
		return(TRUE);
	}

	return(FALSE);
}

/* =======================================================

      Lighting Tracing
      
======================================================= */

void ray_trace_lights(ray_scene_type *scene,ray_point_type *eye_pnt,ray_point_type *trig_pnt,ray_collision_type *collision,ray_color_type *col)
{
	int							n,light_idx;
	float						dist,cone_diffuse,att,
								diffuse,spec_factor;
	bool						hit;
	ray_material_pixel_type		material_pixel;
	ray_color_type				light_col,spec_col;
	ray_vector_type				light_vector,light_vector_normal,
								light_poly_space_vector,bump_map_normal,
								eye_vector,half_vector,half_poly_space_vector;
	ray_mesh_type				*mesh;
	ray_light_type				*light;
	
		// get material pixels
		
	ray_get_material_rgb(scene,eye_pnt,trig_pnt,collision,&material_pixel);
	
		// starting mix color

	col->r=col->g=col->b=0.0f;
	col->a=material_pixel.color.rgb.a;
	
		// if there is a bump, unpack it

	if (material_pixel.normal.on) {
		bump_map_normal.x=(material_pixel.normal.rgb.r*2.0f)-1.0f;
		bump_map_normal.y=(material_pixel.normal.rgb.g*2.0f)-1.0f;
		bump_map_normal.z=(material_pixel.normal.rgb.b*2.0f)-1.0f;
		ray_vector_normalize(&bump_map_normal);
	}

		// default to no specular
		
	spec_col.r=0.0f;
	spec_col.g=0.0f;
	spec_col.b=0.0f;
	
		// find lights that can hit
		// this mesh
		// use collision list to cut down
		// on the lights we look at
		
	mesh=scene->mesh_list.meshes[collision->mesh_idx];

	hit=FALSE;
	
	for (n=0;n!=mesh->collide_lights_list.count;n++) {
		
		light_idx=mesh->collide_lights_list.indexes[n];
		light=scene->light_list.lights[light_idx];

			// outside of intensity globe?

		dist=ray_distance_between_points(trig_pnt,&light->pnt);
		if (dist>light->intensity) continue;
		
			// get light vector
			// normalize it for cone, bump
			// and spec calculations
			
		ray_vector_create_from_points(&light_vector,&light->pnt,trig_pnt);

			// get a normalized version of
			// the light vector for cone, bump
			// and specular calculations

		light_vector_normal.x=light_vector.x;
		light_vector_normal.y=light_vector.y;
		light_vector_normal.z=light_vector.z;
		ray_vector_normalize(&light_vector_normal);

			// are we within the light cone?

		if (light->direction.on) {
			cone_diffuse=1.0f-ray_vector_dot_product(&light_vector_normal,&light->direction.vct);
			if (cone_diffuse>light->direction.cos_sweep) continue;

			cone_diffuse=1.0f-(cone_diffuse/light->direction.cos_sweep);
		}
		else {
			cone_diffuse=1.0f;
		}

			// check for mesh collides
			// blocking light

		if (ray_block_light(scene,trig_pnt,&light_vector,collision,light_idx)) continue;

			// attenuate the light for distance

		att=1.0f-(dist/light->intensity);
		att=powf(att,light->exponent);

		light_col.r=light->col.r*att;
		light_col.g=light->col.g*att;
		light_col.b=light->col.b*att;
		
			// handle any diffuse calcs
			// it will either be the surface normal
			// and light normal, or the surface normal
			// plus the normal map and the light normal
			
		if (material_pixel.normal.on) {

				// translate light normal to
				// same space as normal map

			light_poly_space_vector.x=ray_vector_dot_product(&light_vector_normal,&material_pixel.surface.tangent);
			light_poly_space_vector.y=ray_vector_dot_product(&light_vector_normal,&material_pixel.surface.binormal);
			light_poly_space_vector.z=ray_vector_dot_product(&light_vector_normal,&material_pixel.surface.normal);
			ray_vector_normalize(&light_poly_space_vector);

				// the diffuse is the dot between the
				// light vector (now in the same space
				// as the bump normal) and the bump normal
				// (both have to be normalized)

			diffuse=ray_vector_dot_product(&light_poly_space_vector,&bump_map_normal);

				// get the spec
				// specular calculations require the bump maps
				// so we only calculate this if we've calced
				// the bumps

			if (material_pixel.specular.on) {

					// the specular is shined on surfaces
					// that are facing the half vector between
					// the eye to the point and the light to the
					// point

				ray_vector_create_from_points(&eye_vector,eye_pnt,trig_pnt);
				ray_vector_normalize(&eye_vector);

				half_vector.x=light_vector_normal.x+eye_vector.x;
				half_vector.y=light_vector_normal.y+eye_vector.y;
				half_vector.z=light_vector_normal.z+eye_vector.z;
				ray_vector_normalize(&half_vector);

					// move half vector into polygon space

				half_poly_space_vector.x=ray_vector_dot_product(&half_vector,&material_pixel.surface.tangent);
				half_poly_space_vector.y=ray_vector_dot_product(&half_vector,&material_pixel.surface.binormal);
				half_poly_space_vector.z=ray_vector_dot_product(&half_vector,&material_pixel.surface.normal);
				ray_vector_normalize(&half_poly_space_vector);

					// calculate the spec factor
					// which is the dot product between the
					// half vector and the bump
					// add in the lighting attenuation
					// to make a better effect

				spec_factor=fabsf(ray_vector_dot_product(&half_poly_space_vector,&bump_map_normal));
				spec_factor=powf(spec_factor,material_pixel.shine_factor)*att;
				
					// and mix it with the specular map
				
				spec_col.r=(material_pixel.specular.rgb.r*spec_factor);
				spec_col.g=(material_pixel.specular.rgb.g*spec_factor);
				spec_col.b=(material_pixel.specular.rgb.b*spec_factor);
			}

				// move the diffuse to from -1..1
				// to 0..1
			
			diffuse=(diffuse+1.0f)*0.5f;
		}
		else {
			diffuse=1.0f;	// no normals, so no diffuse
		}

			// and the lighting cone
			// into the diffuse
	
		diffuse*=cone_diffuse;
		
			// mix with material and
			// add to pixel
			
		col->r+=((material_pixel.color.rgb.r*((light_col.r+scene->ambient_col.r)*diffuse))+spec_col.r);
		col->g+=((material_pixel.color.rgb.g*((light_col.g+scene->ambient_col.g)*diffuse))+spec_col.g);
		col->b+=((material_pixel.color.rgb.b*((light_col.b+scene->ambient_col.b)*diffuse))+spec_col.b);
		
			// we hit a light

		hit=TRUE;
	}

		// if no hit, then the color is
		// flat head on bump, spec, and the
		// ambient color

	if (!hit) {

		if (material_pixel.normal.on) {
			light_poly_space_vector.x=0.0f;
			light_poly_space_vector.y=0.0f;
			light_poly_space_vector.z=1.0f;
			diffuse=ray_vector_dot_product(&light_poly_space_vector,&bump_map_normal);
		}
		else {
			diffuse=1.0f;
		}

		col->r=material_pixel.color.rgb.r*(scene->ambient_col.r*diffuse);
		col->g=material_pixel.color.rgb.g*(scene->ambient_col.g*diffuse);
		col->b=material_pixel.color.rgb.b*(scene->ambient_col.b*diffuse);
	}

		// finish with the glow

	if (material_pixel.glow.on) {
		col->r+=material_pixel.glow.rgb.r;
		col->g+=material_pixel.glow.rgb.g;
		col->b+=material_pixel.glow.rgb.b;
	}
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

		// clamping

	if (pixel_col->r>1.0f) pixel_col->r=1.0f;
	if (pixel_col->g>1.0f) pixel_col->g=1.0f;
	if (pixel_col->b>1.0f) pixel_col->b=1.0f;

		// add to buffer
		
	*buf=ray_create_ulong_color_from_float_no_alpha(pixel_col);
}
	
/* =======================================================

      Build Pass Through, Reflection and Refraction Vectors
      
======================================================= */

void ray_build_alpha_reflect_vector(ray_scene_type *scene,ray_point_type *ray_origin,ray_vector_type *ray_vector,ray_point_type *trig_pnt,ray_collision_type *collision)
{
	float					f;
	ray_vector_type			v,normal;

		// get the normal

	ray_get_material_normal(scene,ray_origin,trig_pnt,collision,&normal);

		// calculate reflection vector
		// (-2*((v dot n)*n))+v

	f=ray_vector_dot_product(ray_vector,&normal);
	ray_vector_scalar_multiply(&v,&normal,f);
	ray_vector_scalar_multiply(&v,&v,-2.0f);
	ray_vector_add(ray_vector,&v,ray_vector);
}

void ray_build_alpha_refract_vector(ray_scene_type *scene,ray_point_type *ray_origin,ray_vector_type *ray_vector,ray_point_type *trig_pnt,ray_collision_type *collision,float refract_factor)
{
	ray_vector_type			v,normal;

		// get the normal
		// and flip it so it's
		// facing through the
		// surface

	ray_get_material_normal(scene,ray_origin,trig_pnt,collision,&normal);

	normal.x=-normal.x;
	normal.y=-normal.y;
	normal.z=-normal.z;

		// get the line from the
		// reverse normal end point
		// and the normalized
		// ray vector end point

		// this will be the line between
		// the two vectors and something
		// we can put the factor on

	v.x=ray_vector->x;
	v.y=ray_vector->y;
	v.z=ray_vector->z;
	ray_vector_normalize(&v);

	v.x-=normal.x;
	v.y-=normal.y;
	v.z-=normal.z;

		// now apply the factor

	ray_vector_scalar_multiply(&v,&v,refract_factor);

		// and rebuild what the new
		// ray by adding it to the
		// normal to get the new
		// cast, and then scale to
		// max dist

	ray_vector->x=v.x+normal.x;
	ray_vector->y=v.y+normal.y;
	ray_vector->z=v.z+normal.z;

	ray_vector_normalize(ray_vector);
	ray_vector_scalar_multiply(ray_vector,ray_vector,scene->eye.max_dist);
}

void ray_build_alpha_vector(ray_scene_type *scene,ray_point_type *ray_origin,ray_vector_type *ray_vector,ray_point_type *trig_pnt,ray_collision_type *collision)
{
	ray_poly_type			*poly;
	ray_material_type		*material;

		// get the material

	poly=&scene->mesh_list.meshes[collision->mesh_idx]->poly_block.polys[collision->poly_idx];
	material=ray_global.material_list.materials[poly->material_idx];

		// build the correct vector
		// pass through does nothing as
		// it doesn't change the vector

	switch (material->alpha_type) {

		case RL_MATERIAL_ALPHA_REFLECT:
			ray_build_alpha_reflect_vector(scene,ray_origin,ray_vector,trig_pnt,collision);
			break;

		case RL_MATERIAL_ALPHA_REFRACT:
			ray_build_alpha_refract_vector(scene,ray_origin,ray_vector,trig_pnt,collision,material->refract_factor);
			break;

	}

		// move new point to origin

	ray_origin->x=trig_pnt->x;
	ray_origin->y=trig_pnt->y;
	ray_origin->z=trig_pnt->z;

		// add to the current skip polygons
		// will never go over count as count
		// is equal to maximum number of bounces

	collision->skip_block.skips[collision->skip_block.count].mesh_idx=collision->mesh_idx;
	collision->skip_block.skips[collision->skip_block.count].poly_idx=collision->poly_idx;
	
	collision->skip_block.count++;

	collision->in_bounce=TRUE;
}

/* =======================================================

      Ray Rendering Main Thread
      
======================================================= */

void ray_render_thread_run(ray_draw_scene_thread_info *thread_info)
{
	int							x,y,repeat_count;
	float						f,xadd,yadd,zadd;
	unsigned long				*buf;
	bool						no_hit;
	ray_point_type				*eye_point,ray_origin,trig_point,view_plane_point;
	ray_vector_type				ray_vector;
	ray_color_type				pixel_col,mat_col,overlay_col;
	ray_collision_type			collision;
	ray_scene_type				*scene;

		// get the scene

	scene=thread_info->parent_scene;

		// build some per-thread
		// precalcs
		
	ray_precalc_render_scene_thread_setup(scene,thread_info);
	
		// get eye point
		
	eye_point=&scene->eye.pnt;
			
		// eye point movement

	xadd=eye_point->x-(float)(scene->buffer.wid>>1);
	yadd=eye_point->y-(float)(scene->buffer.high>>1);
	zadd=eye_point->z+scene->eye.min_dist;
	
		// draw
		
	for (y=thread_info->pixel_start.y;y!=thread_info->pixel_end.y;y++) {
	
		for (x=thread_info->pixel_start.x;x!=thread_info->pixel_end.x;x++) {
		
				// determine if in overlay
				// do an early exit if no alpha
				
			if (ray_get_overlay_rgb(scene,x,y,&overlay_col)) {
				if (overlay_col.a==1.0f) {
					buf=scene->buffer.data+((y*scene->buffer.wid)+x);		// buffer is unsigned long
					ray_set_buffer(buf,&overlay_col,NULL);
					continue;
				}
			}
			else {
				overlay_col.r=overlay_col.g=overlay_col.b=overlay_col.a=0.0f;
			}

				// repeat through this to
				// capture all reflections or
				// alpha pass-throughs

			no_hit=TRUE;
			repeat_count=0;

			pixel_col.r=pixel_col.g=pixel_col.b=0.0f;
			
				// original ray

			view_plane_point.x=xadd+(float)x;
			view_plane_point.y=yadd+(float)y;
			view_plane_point.z=zadd;

			ray_origin.x=eye_point->x;
			ray_origin.y=eye_point->y;
			ray_origin.z=eye_point->z;
			
			ray_vector_create_from_points(&ray_vector,&view_plane_point,&ray_origin);
			
			rlMatrixVectorMultiply(&scene->eye.matrix,&ray_vector);
			
				// scale it to eye distance
				
			ray_vector_normalize(&ray_vector);
			ray_vector.x*=scene->eye.max_dist;
			ray_vector.y*=scene->eye.max_dist;
			ray_vector.z*=scene->eye.max_dist;
				
				// the collision struct
				
			collision.max_t=scene->eye.max_dist;
			collision.skip_block.count=0;
			collision.in_bounce=FALSE;
		
				// run the ray

			while (TRUE) {

					// find nearest mesh-trig intersection
					
				if (!collision.in_bounce) {
					ray_intersect_mesh_list_initial(scene,thread_info,&ray_origin,&ray_vector,&collision);
				}
				else {
					ray_intersect_mesh_list_bounce(scene,thread_info,&ray_origin,&ray_vector,&collision);
				}

				if (collision.trig_idx==-1) break;

				ray_vector_find_line_point_for_T(&ray_origin,&ray_vector,collision.t,&trig_point);
				
					// get buffer
					
				buf=scene->buffer.data+((y*scene->buffer.wid)+x);		// buffer is unsigned long

					// check for special lighting conditions
					// else run regular lighting

				if (!ray_mesh_special_lighting_conditions(scene,&ray_origin,&trig_point,collision.mesh_idx,&collision,&mat_col)) {
					ray_trace_lights(scene,&ray_origin,&trig_point,&collision,&mat_col);
				}

					// add in the new lighting
					// if it's the first hit, then pixel
					// equals coloring, otherwise we mix it
					// with the previous alpha

				if (no_hit) {
					pixel_col.r=mat_col.r;
					pixel_col.g=mat_col.g;
					pixel_col.b=mat_col.b;

					no_hit=FALSE;
				}
				else {
					f=1.0f-pixel_col.a;
					pixel_col.r=(pixel_col.r*pixel_col.a)+(mat_col.r*f);
					pixel_col.g=(pixel_col.g*pixel_col.a)+(mat_col.g*f);
					pixel_col.b=(pixel_col.b*pixel_col.a)+(mat_col.b*f);
				}

				pixel_col.a=mat_col.a;

					// do we need to repeat and run
					// ray again?

				if (mat_col.a==1.0f) break;

					// repeat the array

				repeat_count++;
				if (repeat_count==ray_max_bounce) break;

					// we have an alpha, need to rebuild
					// the vector and trace again

				ray_build_alpha_vector(scene,&ray_origin,&ray_vector,&trig_point,&collision);
			}

				// finally set the buffer

			if (!no_hit) ray_set_buffer(buf,&pixel_col,&overlay_col);
		}
	}
}

/* =======================================================

      Thread MainLine
      
======================================================= */

#ifndef WIN32

void* ray_render_thread(void *arg)
{
	ray_draw_scene_thread_info	*thread_info;
	ray_scene_type				*scene;

		// get the thread and scene

	thread_info=(ray_draw_scene_thread_info*)arg;
	scene=thread_info->parent_scene;

		// set some flags

	thread_info->shutdown_done=FALSE;

		// these are worker threads so
		// they suspend until needed

	while (TRUE) {

			// make sure to work around spurious
			// exits

		pthread_mutex_lock(&scene->render.thread_lock);

		while (TRUE) {
			pthread_cond_wait(&scene->render.thread_cond,&scene->render.thread_lock);
			if (scene->thread_mode!=ray_thread_mode_suspend) break;
		}

		pthread_mutex_unlock(&scene->render.thread_lock);

			// in shutdown?

		if (scene->thread_mode==ray_thread_mode_shutdown) {
			thread_info->shutdown_done=TRUE;
			pthread_exit(0);
			return(0);
		}

			// run the thread

		ray_render_thread_run(thread_info);
		
			// add up thread done count
			
		pthread_mutex_lock(&scene->render.scene_lock);
		scene->render.thread_done_count++;
		pthread_mutex_unlock(&scene->render.scene_lock);
	}
}

#else

unsigned __stdcall ray_render_thread(void *arg)
{
	ray_draw_scene_thread_info	*thread_info;
	ray_scene_type				*scene;

		// get the thread and scene

	thread_info=(ray_draw_scene_thread_info*)arg;
	scene=thread_info->parent_scene;

		// set some flags

	thread_info->shutdown_done=FALSE;

		// these are worker threads so
		// they suspend until needed

	while (TRUE) {

		SuspendThread(thread_info->thread);

			// in case spurious wake-up

		if (scene->thread_mode==ray_thread_mode_suspend) continue;

			// in shutdown?

		if (scene->thread_mode==ray_thread_mode_shutdown) {
			thread_info->shutdown_done=TRUE;
			_endthreadex(0);
			return(0);
		}

			// run the thread

		ray_render_thread_run(thread_info);
		
			// add up thread done count
			
		WaitForSingleObject(scene->render.scene_lock,INFINITE);
		scene->render.thread_done_count++;
		ReleaseMutex(scene->render.scene_lock);
	}
}

#endif

/* =======================================================

      Thread Utilities
      
======================================================= */

void ray_render_clear_threads(ray_scene_type *scene)
{
	scene->render.thread_done_count=ray_global.settings.thread_count;
}

void ray_render_stall(ray_scene_type *scene)
{
	while (scene->render.thread_done_count<ray_global.settings.thread_count) {
		usleep(1);
	}
}

/* =======================================================

      Render a Scene
	  
	  Notes:
	   If there is a current render going on for this
	   scene, this new render will stall until the old
	   one is finished
	   All rendering functions (including checking or
	   stalling functions) must be called on the same thread

 	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
	   RL_ERROR_SCENE_IN_USE
     
======================================================= */

int rlSceneRender(int sceneId)
{
	int						idx;
	ray_scene_type			*scene;

		// get the scene

	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);

	scene=ray_global.scene_list.scenes[idx];
	
		// need to stall if already processing
		
	ray_render_stall(scene);

		// setup some precalcs for meshes
		// and lights, including collision
		// cross lists and mesh in scene
		// lists

	ray_precalc_render_scene_setup(scene);

		// some presetup for overlays

	ray_overlay_setup_all(scene);

		// we are now rendering

	scene->render.thread_done_count=0;

		// resume the worker threads

	ray_scene_resume_threads(scene,ray_thread_mode_rendering);

	return(RL_ERROR_OK);
}

/* =======================================================

      Check Rendering State
	  
	  Notes:
	   All rendering functions (including checking or
	   stalling functions) must be called on the same thread

 	  Returns:
	   RL_SCENE_STATE_IDLE
	   RL_SCENE_STATE_RENDERING
	   RL_ERROR_UNKNOWN_SCENE_ID
     
======================================================= */

int rlSceneRenderState(int sceneId)
{
	int					idx;
	ray_scene_type		*scene;

		// get the scene

	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);
	
	scene=ray_global.scene_list.scenes[idx];

		// check rendering state

	if (scene->render.thread_done_count==ray_global.settings.thread_count) return(RL_SCENE_STATE_IDLE);

	return(RL_SCENE_STATE_RENDERING);
}

/* =======================================================

      Finish all Rendering
	  
	  Notes:
	   Will stall until rendering is finished
	   All rendering functions (including checking or
	   stalling functions) must be called on the same thread

 	  Returns:
	   RL_ERROR_UNKNOWN_SCENE_ID
     
======================================================= */

int rlSceneRenderFinish(int sceneId)
{
	int					idx;
	ray_scene_type		*scene;

		// get the scene

	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);
	
	scene=ray_global.scene_list.scenes[idx];
	
		// stall
		
	ray_render_stall(scene);
	
	return(RL_ERROR_OK);
}
