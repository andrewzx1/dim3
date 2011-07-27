/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Map Vertex Lists

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

extern map_type				map;
extern view_type			view;
extern setup_type			setup;

/* =======================================================

      Initialize VBOs For Map
      
======================================================= */

bool view_map_vbo_initialize_mesh(map_mesh_type *mesh)
{
	int					n,k,vertex_cnt;
	unsigned short		idx;
	unsigned short		*index_ptr;
	float				*vertex_ptr,*pv,*pt,*pn,*pc,*pp;
	bool				shader_on;
	d3pnt				*pnt;
	map_mesh_poly_type	*poly;

	shader_on=view_shader_on();

		// setup vertex pointer

	vertex_cnt=mesh->vbo.count;

	view_bind_mesh_liquid_vertex_object(&mesh->vbo);
	vertex_ptr=view_map_mesh_liquid_vertex_object(&mesh->vbo);
	if (vertex_ptr==NULL) return(FALSE);

	pv=vertex_ptr;
	
	if (shader_on) {
		pt=vertex_ptr+(vertex_cnt*(3+2+2));
		pn=vertex_ptr+(vertex_cnt*(3+3+2+2));
	}
	else {
		pc=vertex_ptr+(vertex_cnt*(3+2+2));
	}

		// vertexes, tangents, normals and color
		// we run this separate from the UVs
		// as they are grouped for layers

	poly=mesh->polys;
	
	for (n=0;n!=mesh->npoly;n++) {
	
		for (k=0;k!=poly->ptsz;k++) {
		
			pnt=&mesh->vertexes[poly->v[k]];

			*pv++=(float)pnt->x;
			*pv++=(float)pnt->y;
			*pv++=(float)pnt->z;

			if (shader_on) {
				*pt++=poly->tangent_space.tangent.x;
				*pt++=poly->tangent_space.tangent.y;
				*pt++=poly->tangent_space.tangent.z;
				*pn++=poly->tangent_space.normal.x;
				*pn++=poly->tangent_space.normal.y;
				*pn++=poly->tangent_space.normal.z;
			}
			else {
				*pc++=1.0f;
				*pc++=1.0f;
				*pc++=1.0f;
			}
		}

		poly++;
	}
	
		// main UVs

	pp=vertex_ptr+(vertex_cnt*3);

	poly=mesh->polys;
			
	for (n=0;n!=mesh->npoly;n++) {
			
		for (k=0;k!=poly->ptsz;k++) {
			*pp++=poly->main_uv.x[k]+poly->draw.x_shift_offset;
			*pp++=poly->main_uv.y[k]+poly->draw.y_shift_offset;
		}

		poly++;
	}

		// light map UVs

	poly=mesh->polys;
		
	for (n=0;n!=mesh->npoly;n++) {
		
		for (k=0;k!=poly->ptsz;k++) {
			*pp++=poly->lmap_uv.x[k];
			*pp++=poly->lmap_uv.y[k];
		}

		poly++;
	}
	
	view_unmap_mesh_liquid_vertex_object();
	view_unbind_mesh_liquid_vertex_object();

		// create the indexes

	view_bind_mesh_liquid_index_object(&mesh->vbo);
	
	index_ptr=view_map_mesh_liquid_index_object(&mesh->vbo);
	if (index_ptr==NULL) return(FALSE);

	idx=0;

	poly=mesh->polys;
		
	for (n=0;n!=mesh->npoly;n++) {
	
		poly->vbo.index_min=idx;
		poly->vbo.index_max=idx+poly->ptsz;
		
		poly->vbo.index_offset=idx*sizeof(unsigned short);
		
		for (k=0;k!=poly->ptsz;k++) {
			*index_ptr++=(unsigned short)idx;
			idx++;
		}

		poly++;
	}

	view_unmap_mesh_liquid_index_object();
	view_unbind_mesh_liquid_index_object();

		// reset some flags that
		// optimize when the VBO is rebuilt

	mesh->draw.moved=FALSE;
	mesh->draw.cur_ambient_only=FALSE;

	return(TRUE);
}

bool view_map_vbo_initialize(void)
{
	int					n,k,vertex_cnt,index_cnt;
	bool				shader_on;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	map_liquid_type		*liq;

	shader_on=view_shader_on();

		// meshes

	mesh=map.mesh.meshes;

	for (n=0;n!=map.mesh.nmesh;n++) {

			// get index count

		index_cnt=0;
		poly=mesh->polys;
		
		for (k=0;k!=mesh->npoly;k++) {
			index_cnt+=poly->ptsz;
			poly++;
		}

			// get vertex count

		vertex_cnt=index_cnt*(3+2+2);
		if (shader_on) {
			vertex_cnt+=(index_cnt*(3+3));
		}
		else {
			vertex_cnt+=(index_cnt*3);
		}

			// cretae the VBO

		mesh->vbo.count=index_cnt;
		view_create_mesh_liquid_vertex_object(&mesh->vbo,vertex_cnt,index_cnt);

		if (!view_map_vbo_initialize_mesh(mesh)) return(FALSE);
		
		if (!map_mesh_create_colors_cache(mesh)) return(FALSE);
		mesh++;
	}

		// liquids

	if (shader_on) {
		vertex_cnt=4*(3+2+2+3+3);
	}
	else {
		vertex_cnt=4*(3+2+2+3);
	}

	liq=map.liquid.liquids;

	for (n=0;n!=map.liquid.nliquid;n++) {
		liq->vbo.count=4;
		view_create_mesh_liquid_vertex_object(&liq->vbo,vertex_cnt,4);
		liq++;
	}

	return(TRUE);
}

void view_map_vbo_release(void)
{
	int					n;
	map_mesh_type		*mesh;
	map_liquid_type		*liq;

		// release meshes
		// color caches get released when mesh is destroyed

	mesh=map.mesh.meshes;

	for (n=0;n!=map.mesh.nmesh;n++) {
		view_dispose_mesh_liquid_vertex_object(&mesh->vbo);
		mesh++;
	}

		// release liquids

	liq=map.liquid.liquids;

	for (n=0;n!=map.liquid.nliquid;n++) {
		view_dispose_mesh_liquid_vertex_object(&liq->vbo);
		liq++;
	}

}

/* =======================================================

      Rebuild VBOs For Map Changes
      
======================================================= */

void view_map_vbo_rebuild_mesh(map_mesh_type *mesh)
{
	int					n,k;
	float				x_shift_offset,y_shift_offset;
	float				*vertex_ptr,*pv,*pt,*pn,*pp,*pc,*pc2;
	bool				shader_on,only_ambient,skip;
	d3pnt				*pnt;
	d3col				col;
	map_mesh_poly_type	*poly;
	
	shader_on=view_shader_on();

		// we don't immediately bind and map
		// until we actually require a change
		
	vertex_ptr=NULL;
	
	if ((mesh->flag.moveable) && (mesh->draw.moved)) {
	
		if (vertex_ptr==NULL) {
			view_bind_mesh_liquid_vertex_object(&mesh->vbo);
			vertex_ptr=view_map_mesh_liquid_vertex_object(&mesh->vbo);
			if (vertex_ptr==NULL) return;
		}

		pv=vertex_ptr;
		
		if (shader_on) {
			pt=vertex_ptr+(mesh->vbo.count*(3+2+2));
			pn=vertex_ptr+(mesh->vbo.count*(3+3+2+2));
		}

		poly=mesh->polys;
		
		for (n=0;n!=mesh->npoly;n++) {

				// polygon vertexes

			for (k=0;k!=poly->ptsz;k++) {
			
				pnt=&mesh->vertexes[poly->v[k]];

				*pv++=(float)pnt->x;
				*pv++=(float)pnt->y;
				*pv++=(float)pnt->z;
				
				if (shader_on) {
					*pt++=poly->tangent_space.tangent.x;
					*pt++=poly->tangent_space.tangent.y;
					*pt++=poly->tangent_space.tangent.z;
					*pn++=poly->tangent_space.normal.x;
					*pn++=poly->tangent_space.normal.y;
					*pn++=poly->tangent_space.normal.z;
				}
			}

			poly++;
		}
	}

		// recalculate the uvs if this
		// mesh has shiftable uvs

	if (mesh->flag.shiftable) {
	
		if (vertex_ptr==NULL) {
			view_bind_mesh_liquid_vertex_object(&mesh->vbo);
			vertex_ptr=view_map_mesh_liquid_vertex_object(&mesh->vbo);
			if (vertex_ptr==NULL) return;
		}

			// only shift main UVs (not light mapped ones)

		pp=vertex_ptr+(mesh->vbo.count*3);
		
		poly=mesh->polys;

		for (n=0;n!=mesh->npoly;n++) {

			x_shift_offset=poly->draw.x_shift_offset;
			y_shift_offset=poly->draw.y_shift_offset;

			for (k=0;k!=poly->ptsz;k++) {
				*pp++=poly->main_uv.x[k]+x_shift_offset;
				*pp++=poly->main_uv.y[k]+y_shift_offset;
			}

			poly++;
		}
	}

		// non-shader lighting
		
	if (!shader_on) {
	
			// get the lights for this mesh
			// we have a special check for ambient only
			// lighting which can skip a number of stages
			
		if (view.debug.on) {
			only_ambient=TRUE;
			col.r=col.g=col.b=1.0f;
		}
		else {
			only_ambient=(mesh->light_cache.count==0);
		}

			// we have a special flag to tell if we've already
			// set it to ambient only the last draw and then skip it

		skip=FALSE;
		
		if (only_ambient) {
			if (mesh->draw.cur_ambient_only) {
				skip=TRUE;
			}
			else {
				mesh->draw.cur_ambient_only=TRUE;
				gl_lights_calc_ambient_color(&col);
			}
		}
		else {
			mesh->draw.cur_ambient_only=FALSE;
		}
			
			// set the colors
			
		if (!skip) {
		
			pc=mesh->colors_cache;
			
				// colors when only ambient lighting
				
			if (only_ambient) {
				for (n=0;n!=mesh->nvertex;n++) {
					*pc++=col.r;
					*pc++=col.g;
					*pc++=col.b;
				}
			}
			
				// colors hit by lights
				
			else {
				pnt=mesh->vertexes;

				for (n=0;n!=mesh->nvertex;n++) {
					gl_lights_calc_color_light_cache(mesh->light_cache.count,mesh->light_cache.indexes,TRUE,(double)pnt->x,(double)pnt->y,(double)pnt->z,pc);
					pc+=3;
					pnt++;
				}
			}

				// create per poly colors
				
			if (vertex_ptr==NULL) {
				view_bind_mesh_liquid_vertex_object(&mesh->vbo);
				vertex_ptr=view_map_mesh_liquid_vertex_object(&mesh->vbo);
				if (vertex_ptr==NULL) return;
			}

			pc=vertex_ptr+(mesh->vbo.count*(3+2+2));

			poly=mesh->polys;
			
			for (n=0;n!=mesh->npoly;n++) {
			
				for (k=0;k!=poly->ptsz;k++) {
					pc2=mesh->colors_cache+(poly->v[k]*3);
					*pc++=*pc2++;
					*pc++=*pc2++;
					*pc++=*pc2;
				}

				poly++;
			}
		}
	}
	
		// if we made a change, then it got
		// mapped and we need to unmap it
		
	if (vertex_ptr!=NULL) {
		view_unmap_mesh_liquid_vertex_object();
		view_unbind_mesh_liquid_vertex_object();
	}
}

void view_map_vbo_rebuild(void)
{
	int				n;
	map_mesh_type	*mesh;
	
		// run through all meshes in
		// view list
		
	for (n=0;n!=view.render->draw_list.count;n++) {
		if (view.render->draw_list.items[n].type!=view_render_type_mesh) continue;

		mesh=&map.mesh.meshes[view.render->draw_list.items[n].idx];
		view_map_vbo_rebuild_mesh(mesh);
	}
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

// supergumba -- delete all this

void view_compile_gl_list_attach(void)
{
/*
		// use last compiled buffer

	view_bind_map_vertex_object();
	view_bind_map_index_object();

		// vertexes

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,(void*)0);
	*/
}

void view_compile_gl_list_attach_uv_light_map(void)
{
/*
	int			offset;

	offset=(map.mesh.vbo_vertex_count*3)*sizeof(float);

	glClientActiveTexture(GL_TEXTURE1);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(void*)offset);
	
	offset=(map.mesh.vbo_vertex_count*(3+2))*sizeof(float);

	glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(void*)offset);
	*/
}

void view_compile_gl_list_attach_uv_shader(void)
{
/*
	int			offset;

	offset=(map.mesh.vbo_vertex_count*(3+2))*sizeof(float);

	glClientActiveTexture(GL_TEXTURE1);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(void*)offset);
	
	offset=(map.mesh.vbo_vertex_count*3)*sizeof(float);
	
	glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(void*)offset);
	*/
}

void view_compile_gl_list_attach_uv_glow(void)
{
/*
	int			offset;

	offset=(map.mesh.vbo_vertex_count*3)*sizeof(float);
	
	glClientActiveTexture(GL_TEXTURE1);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(void*)offset);
	
	glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(void*)offset);
	*/
}

void view_compile_gl_list_enable_color(void)
{
/*
	int			offset;

	offset=(map.mesh.vbo_vertex_count*(3+2+2))*sizeof(float);

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(3,GL_FLOAT,0,(void*)offset);
	*/
}

void view_compile_gl_list_disable_color(void)
{
//	glDisableClientState(GL_COLOR_ARRAY);
}

void view_compile_gl_list_dettach(void)
{
/*
	glClientActiveTexture(GL_TEXTURE1);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glClientActiveTexture(GL_TEXTURE0);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glDisableClientState(GL_VERTEX_ARRAY);

	view_unbind_map_index_object();
	view_unbind_map_vertex_object();
	*/
}

