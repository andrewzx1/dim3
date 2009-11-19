/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Map Vertex Lists

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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "lights.h"
#include "video.h"

extern bool					dim3_debug;

extern map_type				map;
extern view_type			view;
extern setup_type			setup;

/* =======================================================

      Setup Vertex, UV, and possible Color List for Meshes
      
======================================================= */

bool view_compile_mesh_gl_list_init(void)
{
	int					n,k,t,uv_idx,vertex_cnt,i_idx;
	unsigned int		v_poly_start_idx;
	unsigned int		*index_ptr;
	float				x_shift_offset,y_shift_offset,dark_factor;
	float				*vertex_ptr,*pv,*pp,*pc;
	d3pnt				*pnt;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

		// get total number of vertexes and indexes
		// and their offsets to setup vertex object for map
		
	vertex_cnt=0;
	
		// setup meshes
		
	mesh=map.mesh.meshes;

	for (n=0;n!=map.mesh.nmesh;n++) {

			// mesh has not been moved

		mesh->draw.moved=FALSE;

			// setup offsets

		mesh->draw.vertex_offset=vertex_cnt;

			// poly vertexes and uv counts
			
		poly=mesh->polys;
		
		for (k=0;k!=mesh->npoly;k++) {
			poly->draw.vertex_offset=vertex_cnt;
			vertex_cnt+=poly->ptsz;
			poly++;
		}
		
		mesh++;
	}

		// total vertexes

	map.mesh.vbo_vertex_count=vertex_cnt;

		// initial vertex VBO
		// we need a UV list for every possible layer
		
	view_init_map_vertex_object((vertex_cnt*(3+3))+((vertex_cnt*max_mesh_poly_uv_layer)*2));

	vertex_ptr=view_bind_map_map_vertex_object();
	if (vertex_ptr==NULL) return(FALSE);

	pv=vertex_ptr;
	pc=pv+(vertex_cnt*3);
	pp=pv+(vertex_cnt*(3+3));
	
		// vertexes and color
		// we run this separate from the UVs
		// as they are grouped for layers

	mesh=map.mesh.meshes;

	for (n=0;n!=map.mesh.nmesh;n++) {
	
			// vertexes and colors

		poly=mesh->polys;
		
		for (k=0;k!=mesh->npoly;k++) {
		
			dark_factor=poly->dark_factor;

			for (t=0;t!=poly->ptsz;t++) {
			
				pnt=&mesh->vertexes[poly->v[t]];

				*pv++=(float)pnt->x;
				*pv++=(float)pnt->y;
				*pv++=(float)pnt->z;

				*pc++=dark_factor;
				*pc++=dark_factor;
				*pc++=dark_factor;
			}

			poly++;
		}
		
		mesh++;
	}
	
		// uvs
				
	for (uv_idx=0;uv_idx!=max_mesh_poly_uv_layer;uv_idx++) {

		mesh=map.mesh.meshes;

		for (n=0;n!=map.mesh.nmesh;n++) {
		
			poly=mesh->polys;

				// mesh has a UV for this layer

			if (mesh->nuv>uv_idx) {
				
				for (k=0;k!=mesh->npoly;k++) {
				
					if (uv_idx==0) {
						x_shift_offset=poly->draw.x_shift_offset;
						y_shift_offset=poly->draw.y_shift_offset;
					}
					else {
						x_shift_offset=y_shift_offset=0.0f;
					}
					
					for (t=0;t!=poly->ptsz;t++) {
						*pp++=poly->uv[uv_idx].x[t]+x_shift_offset;
						*pp++=poly->uv[uv_idx].y[t]+y_shift_offset;
					}

					poly++;
				}
			}

				// mesh does not have a UV for this layer

			else {

				for (k=0;k!=mesh->npoly;k++) {
					for (t=0;t!=poly->ptsz;t++) {
						*pp++=0.0f;
						*pp++=0.0f;
					}

					poly++;
				}
			}

			mesh++;
		}
	}

		// unmap vertex VBO
		
	view_unmap_map_vertex_object();
	view_unbind_map_vertex_object();

		// initialize index VBO
		
	view_init_map_index_object(vertex_cnt);
		
	index_ptr=view_bind_map_map_index_object();
	if (index_ptr==NULL) {
		view_unmap_map_vertex_object();
		view_unbind_map_vertex_object();
		return(FALSE);
	}

		// map the indexes

	i_idx=0;
	
	mesh=map.mesh.meshes;

	for (n=0;n!=map.mesh.nmesh;n++) {
	
			// run through the polys

		poly=mesh->polys;
		
		for (k=0;k!=mesh->npoly;k++) {
		
				// polygon indexes
				
			v_poly_start_idx=poly->draw.vertex_offset;
			poly->draw.gl_poly_index_offset=i_idx*sizeof(unsigned int);
				
			for (t=0;t!=poly->ptsz;t++) {
				*index_ptr++=v_poly_start_idx+t;
				i_idx++;
			}
			
				// min/max for range element draws
				
			poly->draw.gl_poly_index_min=v_poly_start_idx;
			poly->draw.gl_poly_index_max=v_poly_start_idx+poly->ptsz;

			poly++;
		}
		
		mesh++;
	}
	
		// unmap index VBO

	view_unmap_map_index_object();
	view_unbind_map_index_object();

		// create color cache for non-shader coloring

	mesh=map.mesh.meshes;

	for (n=0;n!=map.mesh.nmesh;n++) {
		if (!map_mesh_create_colors_cache(mesh)) return(FALSE);
		mesh++;
	}

	return(TRUE);
}

void view_compile_mesh_gl_list_free(void)
{
}

/* =======================================================

      Compile OpenGL Lists MainLine
      
======================================================= */

bool view_compile_mesh_gl_lists(int tick)
{
	int							n,k,t,uv_idx,vertex_cnt;
	float						x_shift_offset,y_shift_offset,dark_factor;
	float						*vertex_ptr,*pv,*pp,*pc,*pc2;
	bool						only_ambient;
	d3col						col;
	d3pnt						*pnt;
	map_mesh_type				*mesh;
	map_mesh_poly_type			*poly;

		// total number of vertexes

	vertex_cnt=map.mesh.vbo_vertex_count;

		// map VBO to memory

	vertex_ptr=view_bind_map_map_vertex_object();
	if (vertex_ptr==NULL) return(FALSE);
	
		// run throught the meshes
		// in this scene and update any
		// relevant data

	for (n=0;n!=view.render->draw_list.count;n++) {

		if (view.render->draw_list.items[n].type!=view_render_type_mesh) continue;

		mesh=&map.mesh.meshes[view.render->draw_list.items[n].idx];

			// recalculate the vertexes if this
			// mesh is moving

		if (mesh->draw.moved) {

			pv=vertex_ptr+(mesh->draw.vertex_offset*3);

			poly=mesh->polys;
			
			for (k=0;k!=mesh->npoly;k++) {

					// polygon vertexes

				for (t=0;t!=poly->ptsz;t++) {
				
					pnt=&mesh->vertexes[poly->v[t]];

					*pv++=(float)pnt->x;
					*pv++=(float)pnt->y;
					*pv++=(float)pnt->z;
				}

				poly++;
			}
		}

			// recalculate the uvs if this
			// mesh has shiftable uvs

		if (mesh->flag.shiftable) {

				// the UV layers
			
			for (uv_idx=0;uv_idx!=mesh->nuv;uv_idx++) {
			
					// only redo layers that are in use

				if (mesh->nuv>uv_idx) {
					
					pp=vertex_ptr+((vertex_cnt*(3*3))+((vertex_cnt*uv_idx)*2))+(mesh->draw.vertex_offset*2);
					poly=mesh->polys;

					for (k=0;k!=mesh->npoly;k++) {

						if (uv_idx==0) {
							x_shift_offset=poly->draw.x_shift_offset;
							y_shift_offset=poly->draw.y_shift_offset;
						}
						else {
							x_shift_offset=y_shift_offset=0.0f;
						}

						for (t=0;t!=poly->ptsz;t++) {
							*pp++=poly->uv[uv_idx].x[t]+x_shift_offset;
							*pp++=poly->uv[uv_idx].y[t]+y_shift_offset;
						}

						poly++;
					}
				}
			}
		}

			// if in debug, we are using high lighting,
			// so recalculate the lights for the mesh

		if ((dim3_debug) || (mesh->flag.hilite)) {
			
			pc=vertex_ptr+((vertex_cnt*3)+(mesh->draw.vertex_offset*3));

			poly=mesh->polys;
				
			for (k=0;k!=mesh->npoly;k++) {
			
				dark_factor=poly->dark_factor;
				
				for (t=0;t!=poly->ptsz;t++) {
					*pc++=dark_factor;
					*pc++=dark_factor;
					*pc++=dark_factor;
				}
				
				poly++;
			}
		}

			// if the mesh has some non-shaders in it,
			// recalculate the lighting
			// supergumba -- this needs to be optimized 

		else {

			if (mesh->draw.has_no_shader) {

					// create colors for each vertexes

				only_ambient=!gl_lights_calc_vertex_setup_mesh(mesh);

				pc=mesh->colors_cache;
				
					// colors when only ambient lighting
					
				if (only_ambient) {
					gl_lights_get_ambient(&col);
					
					for (k=0;k!=mesh->nvertex;k++) {
						*pc++=col.r;
						*pc++=col.g;
						*pc++=col.b;
					}
				}
				
					// colors hit by lights
					
				else {
					pnt=mesh->vertexes;

					for (k=0;k!=mesh->nvertex;k++) {
						gl_lights_calc_vertex((double)pnt->x,(double)pnt->y,(double)pnt->z,pc);
						pc+=3;
						pnt++;
					}
				}
				
					// create per poly colors

				pc=vertex_ptr+((vertex_cnt*3)+(mesh->draw.vertex_offset*3));

				poly=mesh->polys;
				
				for (k=0;k!=mesh->npoly;k++) {
				
					dark_factor=poly->dark_factor;

					for (t=0;t!=poly->ptsz;t++) {
						pc2=mesh->colors_cache+(poly->v[t]*3);
						*pc++=(*pc2++)*dark_factor;
						*pc++=(*pc2++)*dark_factor;
						*pc++=(*pc2)*dark_factor;
					}

					poly++;
				}
			}
		}

			// reset the moved list so we'll catch
			// and update any moved vertexes next time
			// we hit this function

		mesh->draw.moved=FALSE;

		mesh++;
	}

		// unmap VBO

	view_unmap_map_vertex_object();
	view_unbind_map_vertex_object();
	
	return(TRUE);
}

/* =======================================================

      Use Map OpenGL Lists

	  Simple is for single pass non-shader textures,
	  only need a single UV list.

	  Light map is for light mapped non-shader
	  textures, the light map is texture 0, the texture
	  is texture 1

	  Shader is for shaders, where the texture is in 0
	  and the light map in 1 (reverse of light map)

	  Glow is for glow textures
      
======================================================= */

void view_compile_gl_list_attach(void)
{
		// use last compiled buffer

	view_bind_map_vertex_object();
	view_bind_map_index_object();

		// vertexes

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,(void*)0);
}

void view_compile_gl_list_attach_uv_simple(void)
{
	int			offset;

	offset=(map.mesh.vbo_vertex_count*(3+3))*sizeof(float);
	
	glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(void*)offset);
}

void view_compile_gl_list_attach_uv_light_map(void)
{
	int			offset;

	offset=(map.mesh.vbo_vertex_count*(3+3))*sizeof(float);

	glClientActiveTexture(GL_TEXTURE1);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(void*)offset);
	
	offset=((map.mesh.vbo_vertex_count*(3+3))+(map.mesh.vbo_vertex_count*2))*sizeof(float);

	glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(void*)offset);
}

void view_compile_gl_list_attach_uv_shader(void)
{
	int			offset;

	offset=((map.mesh.vbo_vertex_count*(3+3))+(map.mesh.vbo_vertex_count*2))*sizeof(float);

	glClientActiveTexture(GL_TEXTURE1);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(void*)offset);
	
	offset=(map.mesh.vbo_vertex_count*(3+3))*sizeof(float);
	
	glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(void*)offset);
}

void view_compile_gl_list_attach_uv_glow(void)
{
	int			offset;

	offset=(map.mesh.vbo_vertex_count*(3+3))*sizeof(float);
	
	glClientActiveTexture(GL_TEXTURE1);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(void*)offset);
	
	glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(void*)offset);
}

void view_compile_gl_list_enable_color(void)
{
	int			offset;

	offset=(map.mesh.vbo_vertex_count*3)*sizeof(float);

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(3,GL_FLOAT,0,(void*)offset);
}

void view_compile_gl_list_disable_color(void)
{
	glDisableClientState(GL_COLOR_ARRAY);
}

void view_compile_gl_list_dettach(void)
{
	glClientActiveTexture(GL_TEXTURE1);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glClientActiveTexture(GL_TEXTURE0);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glDisableClientState(GL_VERTEX_ARRAY);

	view_unbind_map_index_object();
	view_unbind_map_vertex_object();
}
