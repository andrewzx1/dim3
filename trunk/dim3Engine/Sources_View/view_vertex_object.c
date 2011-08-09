/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Vertex Objects

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

extern map_type				map;
extern view_type			view;
extern server_type			server;
extern setup_type			setup;

int							cur_vbo_cache_idx,cur_vbo_cache_index_idx;
GLuint						vbo_sky,
							vbo_cache[view_vertex_object_count],
							vbo_cache_index[view_vertex_object_count];

/* =======================================================

      Create and Dispose Vertex Objects
      
======================================================= */

void view_create_vertex_objects(void)
{
		// sky vbo

	glGenBuffers(1,&vbo_sky);

		// misc vbos

	glGenBuffers(view_vertex_object_count,vbo_cache);
	glGenBuffers(view_vertex_object_count,vbo_cache_index);

		// start at first misc vbo

	cur_vbo_cache_idx=0;
	cur_vbo_cache_index_idx=0;
}

void view_dispose_vertex_objects(void)
{
	glDeleteBuffers(1,&vbo_sky);

	glDeleteBuffers(view_vertex_object_count,vbo_cache);
	glDeleteBuffers(view_vertex_object_count,vbo_cache_index);
}

/* =======================================================

      Mesh and Liquid VBOs
      
======================================================= */

void view_create_mesh_liquid_vertex_object(map_vbo_type *vbo,int vertex_count,int vertex_mem_sz,int index_count)
{
	glGenBuffers(1,&vbo->vertex);
	glGenBuffers(1,&vbo->index);

		// init the vertex buffer

	vbo->vertex_count=vertex_count;

	glBindBuffer(GL_ARRAY_BUFFER,vbo->vertex);
	glBufferData(GL_ARRAY_BUFFER,vertex_mem_sz,NULL,GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER,0);

		// init the index buffer

	vbo->index_count=index_count;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vbo->index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,(index_count*sizeof(unsigned short)),NULL,GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
}

void view_dispose_mesh_liquid_vertex_object(map_vbo_type *vbo)
{
	glDeleteBuffers(1,&vbo->vertex);
	glDeleteBuffers(1,&vbo->index);
}

void view_bind_mesh_liquid_vertex_object(map_vbo_type *vbo)
{
	glBindBuffer(GL_ARRAY_BUFFER,vbo->vertex);
}

unsigned char* view_map_mesh_liquid_vertex_object(map_vbo_type *vbo)
{
	unsigned char		*vertex_ptr;

	vertex_ptr=(unsigned char*)glMapBuffer(GL_ARRAY_BUFFER,GL_WRITE_ONLY);
	if (vertex_ptr==NULL) {
		glBindBuffer(GL_ARRAY_BUFFER,0);
		return(NULL);
	}

	return(vertex_ptr);
}

void view_unmap_mesh_liquid_vertex_object(void)
{
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void view_unbind_mesh_liquid_vertex_object(void)
{
	glBindBuffer(GL_ARRAY_BUFFER,0);
}

void view_bind_mesh_liquid_index_object(map_vbo_type *vbo)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vbo->index);
}

unsigned short* view_map_mesh_liquid_index_object(map_vbo_type *vbo)
{
	unsigned short		*index_ptr;

	index_ptr=(unsigned short*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER,GL_WRITE_ONLY);
	if (index_ptr==NULL) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
		return(NULL);
	}

	return(index_ptr);
}

void view_unmap_mesh_liquid_index_object(void)
{
	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
}

void view_unbind_mesh_liquid_index_object(void)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
}

/* =======================================================

      Model VBOs
      
======================================================= */

void view_create_model_vertex_object(model_draw *draw)
{
	int					n,vertex_cnt,mem_sz;
	bool				shader_on;
	model_type			*mdl;
	model_mesh_type		*mesh;
	
	shader_on=view_shader_on()&&(!draw->no_shader);
	
		// each mesh has own VBO
		
	mdl=server.model_list.models[draw->model_idx];

	for (n=0;n!=mdl->nmesh;n++) {
		glGenBuffers(1,&draw->vbo[n].vertex);
			
			// get the size
			
		mesh=&mdl->meshes[n];
		vertex_cnt=mesh->ntrig*3;
			
		mem_sz=(vertex_cnt*(3+2))*sizeof(float);
		if (!shader_on) {
			mem_sz+=((vertex_cnt*4)*sizeof(unsigned char));		// colors
		}
		else {
			mem_sz+=((vertex_cnt*(3+3))*sizeof(float));			// tangents and normals
		}

			// init the vertex buffer

		draw->vbo[n].vertex_count=vertex_cnt;

		glBindBuffer(GL_ARRAY_BUFFER,draw->vbo[n].vertex);
		glBufferData(GL_ARRAY_BUFFER,mem_sz,NULL,GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER,0);
	}
}

void view_dispose_model_vertex_object(model_draw *draw)
{
	int					n;
	model_type			*mdl;
	
		// each mesh has own VBO
		
	mdl=server.model_list.models[draw->model_idx];

	for (n=0;n!=mdl->nmesh;n++) {
		glDeleteBuffers(1,&draw->vbo[n].vertex);
	}
}

void view_bind_model_vertex_object(model_draw *draw,int mesh_idx)
{
	glBindBuffer(GL_ARRAY_BUFFER,draw->vbo[mesh_idx].vertex);
}

unsigned char* view_map_model_vertex_object(void)
{
	unsigned char		*vertex_ptr;

	vertex_ptr=(unsigned char*)glMapBuffer(GL_ARRAY_BUFFER,GL_WRITE_ONLY);
	if (vertex_ptr==NULL) {
		glBindBuffer(GL_ARRAY_BUFFER,0);
		return(NULL);
	}

	return(vertex_ptr);
}

void view_unmap_model_vertex_object(void)
{
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void view_unbind_model_vertex_object(void)
{
	glBindBuffer(GL_ARRAY_BUFFER,0);
}

/* =======================================================

      Sky VBOs
      
======================================================= */

float* view_bind_map_sky_vertex_object(int sz)
{
	float		*vertex_ptr;

		// bind to sky specific VBO

	glBindBuffer(GL_ARRAY_BUFFER,vbo_sky);

		// resize VBO

	sz*=sizeof(float);
	glBufferData(GL_ARRAY_BUFFER,sz,NULL,GL_DYNAMIC_DRAW);

		// map pointer

	vertex_ptr=(float*)glMapBuffer(GL_ARRAY_BUFFER,GL_WRITE_ONLY);
	if (vertex_ptr==NULL) {
		glBindBuffer(GL_ARRAY_BUFFER,0);
		return(NULL);
	}

	return(vertex_ptr);
}

void view_bind_sky_vertex_object(void)
{
	glBindBuffer(GL_ARRAY_BUFFER,vbo_sky);
}

void view_unmap_sky_vertex_object(void)
{
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void view_unbind_sky_vertex_object(void)
{
	glBindBuffer(GL_ARRAY_BUFFER,0);
}

/* =======================================================

      Model, Particle, Etc VBOs
      
======================================================= */

float* view_bind_map_next_vertex_object(int sz)
{
	float			*vertex_ptr;

		// bind it

	glBindBuffer(GL_ARRAY_BUFFER,vbo_cache[cur_vbo_cache_idx]);

		// change size of buffer
		// we pass null to stop stalls

	sz*=sizeof(float);
	glBufferData(GL_ARRAY_BUFFER,sz,NULL,GL_DYNAMIC_DRAW);

		// map pointer

	vertex_ptr=(float*)glMapBuffer(GL_ARRAY_BUFFER,GL_WRITE_ONLY);
	if (vertex_ptr==NULL) {
		glBindBuffer(GL_ARRAY_BUFFER,0);
		return(NULL);
	}

		// get next object

	cur_vbo_cache_idx++;
	if (cur_vbo_cache_idx==view_vertex_object_count) cur_vbo_cache_idx=0;

	return(vertex_ptr);
}

void view_unmap_current_vertex_object(void)
{
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void view_unbind_current_vertex_object(void)
{
	glBindBuffer(GL_ARRAY_BUFFER,0);
}

unsigned short* view_bind_map_next_index_object(int sz)
{
	unsigned short		*index_ptr;

		// bind it

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vbo_cache_index[cur_vbo_cache_index_idx]);

		// change size of buffer
		// we pass null to stop stalls

	sz*=sizeof(unsigned short);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,sz,NULL,GL_STATIC_DRAW);

		// map pointer

	index_ptr=(unsigned short*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER,GL_WRITE_ONLY);
	if (index_ptr==NULL) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
		return(NULL);
	}

		// get next object

	cur_vbo_cache_index_idx++;
	if (cur_vbo_cache_index_idx==view_vertex_object_count) cur_vbo_cache_index_idx=0;

	return(index_ptr);
}

void view_unmap_current_index_object(void)
{
	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
}

void view_unbind_current_index_object(void)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
}

