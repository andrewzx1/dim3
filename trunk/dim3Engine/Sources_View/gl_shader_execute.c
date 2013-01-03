/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Shader Execution

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

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "interface.h"

extern map_type				map;
extern view_type			view;
extern setup_type			setup;
extern file_path_setup_type	file_path_setup;

extern int					nuser_shader;
extern shader_type			*gl_shader_current,
							user_shaders[max_iface_user_shader],
							core_shaders[max_shader_light+1][max_core_shader],
							color_shader,gradient_shader,black_shader,
							bitmap_shader;

/* =======================================================

      Shader Generic Set Program and Variables
      
======================================================= */

void gl_shader_draw_execute_set_program(shader_type *shader)
{
	if (shader!=gl_shader_current) {
		gl_shader_current=shader;
		glUseProgram(shader->program_obj);
	}
}

/* =======================================================

      Execute Simple Color Shaders
      
======================================================= */

void gl_shader_draw_execute_simple_color_set_color(d3col *col,float alpha)
{
	if ((color_shader.var_values.simple_color.r!=col->r) || (color_shader.var_values.simple_color.g!=col->g) || (color_shader.var_values.simple_color.b!=col->b) || (color_shader.var_values.simple_color.a!=alpha)) {
		color_shader.var_values.simple_color.r=col->r;
		color_shader.var_values.simple_color.g=col->g;
		color_shader.var_values.simple_color.b=col->b;
		color_shader.var_values.simple_color.a=alpha;
		glUniform4f(color_shader.var_locs.dim3SimpleColor,col->r,col->g,col->b,alpha);
	}
}

void gl_shader_draw_execute_simple_color_ptr(int vertex_size,float *vertexes,d3col *col,float alpha)
{
	gl_shader_draw_execute_set_program(&color_shader);
	gl_shader_set_draw_matrix_variables(&color_shader);
	gl_shader_draw_execute_simple_color_set_color(col,alpha);

	glVertexAttribPointer(color_shader.var_locs.dim3Vertex,vertex_size,GL_FLOAT,GL_FALSE,0,(void*)vertexes);
}

void gl_shader_draw_execute_simple_color_vbo(int vertex_size,int vertex_offset,d3col *col,float alpha)
{
	gl_shader_draw_execute_set_program(&color_shader);
	gl_shader_set_draw_matrix_variables(&color_shader);
	gl_shader_draw_execute_simple_color_set_color(col,alpha);
		
	glVertexAttribPointer(color_shader.var_locs.dim3Vertex,vertex_size,GL_FLOAT,GL_FALSE,0,(void*)vertex_offset);
}

/* =======================================================

      Execute Simple Gradient Shaders
      
======================================================= */

void gl_shader_draw_execute_simple_gradient_vbo(int vertex_size,int vertex_offset,int color_offset)
{
	gl_shader_draw_execute_set_program(&gradient_shader);
	gl_shader_set_draw_matrix_variables(&gradient_shader);
		
	glVertexAttribPointer(gradient_shader.var_locs.dim3Vertex,vertex_size,GL_FLOAT,GL_FALSE,0,(void*)vertex_offset);
	glVertexAttribPointer(gradient_shader.var_locs.dim3VertexColor,4,GL_UNSIGNED_BYTE,GL_TRUE,0,(void*)color_offset);
}

/* =======================================================

      Execute Simple Black Shaders
      
======================================================= */

void gl_shader_draw_execute_simple_black_set_alpha(float alpha)
{
	if (black_shader.var_values.alpha!=alpha) {
		black_shader.var_values.alpha=alpha;
		glUniform1f(black_shader.var_locs.dim3Alpha,alpha);
	}
}

void gl_shader_draw_execute_simple_black_ptr(int vertex_size,float *vertexes,float alpha)
{
	gl_shader_draw_execute_set_program(&black_shader);
	gl_shader_set_draw_matrix_variables(&black_shader);
	gl_shader_draw_execute_simple_black_set_alpha(alpha);
		
	glVertexAttribPointer(black_shader.var_locs.dim3Vertex,vertex_size,GL_FLOAT,GL_FALSE,0,(void*)vertexes);
}

void gl_shader_draw_execute_simple_black_vbo(int vertex_size,int vertex_offset,float alpha)
{
	gl_shader_draw_execute_set_program(&black_shader);
	gl_shader_set_draw_matrix_variables(&black_shader);
	gl_shader_draw_execute_simple_black_set_alpha(alpha);
		
	glVertexAttribPointer(black_shader.var_locs.dim3Vertex,vertex_size,GL_FLOAT,GL_FALSE,0,(void*)vertex_offset);
}

/* =======================================================

      Execute Simple Bitmap Shaders
      
======================================================= */

void gl_shader_draw_execute_simple_bitmap_set_color(d3col *col,float alpha)
{
	if ((bitmap_shader.var_values.simple_color.r!=col->r) || (bitmap_shader.var_values.simple_color.g!=col->g) || (bitmap_shader.var_values.simple_color.b!=col->b) || (bitmap_shader.var_values.simple_color.a!=alpha)) {
		bitmap_shader.var_values.simple_color.r=col->r;
		bitmap_shader.var_values.simple_color.g=col->g;
		bitmap_shader.var_values.simple_color.b=col->b;
		bitmap_shader.var_values.simple_color.a=alpha;
		glUniform4f(bitmap_shader.var_locs.dim3SimpleColor,col->r,col->g,col->b,alpha);
	}
}

void gl_shader_draw_execute_simple_bitmap_set_texture(unsigned long gl_id)
{
	gl_texture_bind(0,gl_id);
}

void gl_shader_draw_execute_simple_bitmap_ptr(unsigned long gl_id,int vertex_size,float *vertexes,float *uvs,d3col *col,float alpha)
{
	gl_shader_draw_execute_set_program(&bitmap_shader);
	gl_shader_set_draw_matrix_variables(&bitmap_shader);
	gl_shader_draw_execute_simple_bitmap_set_texture(gl_id);
	gl_shader_draw_execute_simple_bitmap_set_color(col,alpha);
		
	glVertexAttribPointer(bitmap_shader.var_locs.dim3Vertex,vertex_size,GL_FLOAT,GL_FALSE,0,(void*)vertexes);
	glVertexAttribPointer(bitmap_shader.var_locs.dim3VertexUV,2,GL_FLOAT,GL_FALSE,0,(void*)uvs);
}

void gl_shader_draw_execute_simple_bitmap_vbo_attribute(int vertex_size,int vertex_offset,int uv_offset,int stride,d3col *col,float alpha)
{
	gl_shader_draw_execute_set_program(&bitmap_shader);
	gl_shader_set_draw_matrix_variables(&bitmap_shader);
	gl_shader_draw_execute_simple_bitmap_set_color(col,alpha);
	
	glVertexAttribPointer(bitmap_shader.var_locs.dim3Vertex,vertex_size,GL_FLOAT,GL_FALSE,stride,(void*)vertex_offset);
	glVertexAttribPointer(bitmap_shader.var_locs.dim3VertexUV,2,GL_FLOAT,GL_FALSE,stride,(void*)uv_offset);
}

/* =======================================================

      VBO Attribute Offset Cached Setting

	  The simple shaders (which use pointers, not
	  VBOs) always reset themselves (the data is always
	  going to be different.)  Map, liquid, and model
	  VBO offset type shaders remember their settings
	  to cut down on multiple sets.

	  We have a special function to reset all these
	  cahced offsets because VBO changes will force us
	  to rebuild them.
      
======================================================= */

inline void gl_shader_draw_execute_reset_cached_offsets_shader(shader_type *shader)
{
	shader->var_values.attrib.vertex_offset=-1;
	shader->var_values.attrib.color_offset=-1;
	shader->var_values.attrib.uv_offset=-1;
	shader->var_values.attrib.lmap_uv_offset=-1;
	shader->var_values.attrib.tangent_offset=-1;
	shader->var_values.attrib.normal_offset=-1;
}

void gl_shader_draw_execute_reset_cached_offsets(void)
{
	int					n,k;
	shader_type			*shader;

	for (k=0;k!=(max_shader_light+1);k++) {
		for (n=0;n!=max_core_shader;n++) {
			gl_shader_draw_execute_reset_cached_offsets_shader(&core_shaders[k][n]);
		}
	}

	shader=user_shaders;

	for (n=0;n!=nuser_shader;n++) {
		gl_shader_draw_execute_reset_cached_offsets_shader(shader);
		shader++;
	}
}

inline void gl_shader_draw_execute_set_vertex_offset(shader_type *shader,int vertex_offset,int stride)
{
	if (shader->var_values.attrib.vertex_offset!=vertex_offset) {
		shader->var_values.attrib.vertex_offset=vertex_offset;
		glVertexAttribPointer(shader->var_locs.dim3Vertex,3,GL_FLOAT,GL_FALSE,stride,(void*)vertex_offset);
	}
}

inline void gl_shader_draw_execute_set_uv_offset(shader_type *shader,int uv_offset,int stride)
{
	if (shader->var_values.attrib.uv_offset!=uv_offset) {
		shader->var_values.attrib.uv_offset=uv_offset;
		glVertexAttribPointer(shader->var_locs.dim3VertexUV,2,GL_FLOAT,GL_FALSE,stride,(void*)uv_offset);
	}
}

inline void gl_shader_draw_execute_set_lmap_uv_offset(shader_type *shader,int lmap_uv_offset,int stride)
{
	if (shader->var_values.attrib.lmap_uv_offset!=lmap_uv_offset) {
		shader->var_values.attrib.lmap_uv_offset=lmap_uv_offset;
		glVertexAttribPointer(shader->var_locs.dim3VertexLightMapUV,2,GL_FLOAT,GL_FALSE,stride,(void*)lmap_uv_offset);
	}
}

inline void gl_shader_draw_execute_set_normal_offset(shader_type *shader,int normal_offset,int stride)
{
	if (shader->var_values.attrib.normal_offset!=normal_offset) {
		shader->var_values.attrib.normal_offset=normal_offset;
		glVertexAttribPointer(shader->var_locs.dim3VertexNormal,3,GL_FLOAT,GL_FALSE,stride,(void*)normal_offset);
	}
}

inline void gl_shader_draw_execute_set_tangent_offset(shader_type *shader,int tangent_offset,int stride)
{
	if (shader->var_locs.dim3VertexTangent==-1) return;

	if (shader->var_values.attrib.tangent_offset!=tangent_offset) {
		shader->var_values.attrib.tangent_offset=tangent_offset;
		glVertexAttribPointer(shader->var_locs.dim3VertexTangent,3,GL_FLOAT,GL_FALSE,stride,(void*)tangent_offset);
	}
}

/* =======================================================

      Execute Map Shaders
      
======================================================= */

void gl_shader_draw_execute_map(texture_type *texture,int txt_idx,int frame,int lmap_txt_idx,float alpha,int vertex_offset,int uv_offset,int lmap_uv_offset,int tangent_offset,int normal_offset,int stride,view_glsl_light_list_type *light_list)
{
	bool						is_core;
	shader_type					*shader;
	
		// get shader based on number of lights
		
	if (texture->shader_idx==gl_shader_core_index) {
		shader=gl_core_shader_find_ptr(light_list->nlight,core_shader_group_map,texture);
		is_core=TRUE;
	}
	else {
		shader=&user_shaders[texture->shader_idx];
		is_core=FALSE;
	}
	
		// if we are not in this shader, then
		// change over
		
	if (shader!=gl_shader_current) {
	
			// set in the new program
			
		gl_shader_current=shader;
		glUseProgram(shader->program_obj);
			
			// set per-frame variables, only do this once
			// as they don't change per scene
		
		if (shader->need_frame_reset) {
			shader->need_frame_reset=FALSE;
			gl_shader_set_scene_variables(shader);
		}
	}
	
		// matrix uniforms
		
	gl_shader_set_draw_matrix_variables(shader);
	
		// bind the required attributes

	gl_shader_draw_execute_set_vertex_offset(shader,vertex_offset,stride);
	gl_shader_draw_execute_set_uv_offset(shader,uv_offset,stride);
	gl_shader_draw_execute_set_lmap_uv_offset(shader,lmap_uv_offset,stride);
	gl_shader_draw_execute_set_normal_offset(shader,normal_offset,stride);
	gl_shader_draw_execute_set_tangent_offset(shader,tangent_offset,stride);
	
		// setup variables

	gl_shader_hilite_override(shader,light_list);
	gl_shader_set_texture(shader,core_shader_group_map,texture,txt_idx,lmap_txt_idx,frame);
	gl_shader_set_poly_variables(shader,alpha);
	gl_shader_set_light_variables(shader,core_shader_group_map,is_core,light_list);
}

/* =======================================================

      Execute Liquid Shaders
      
======================================================= */

void gl_shader_draw_execute_liquid(texture_type *texture,int txt_idx,int frame,int lmap_txt_idx,float alpha,int vertex_offset,int uv_offset,int lmap_uv_offset,int tangent_offset,int normal_offset,int stride,view_glsl_light_list_type *light_list)
{
	bool						is_core;
	shader_type					*shader;
	
		// get shader based on number of lights
		
	if (texture->shader_idx==gl_shader_core_index) {
		shader=gl_core_shader_find_ptr(light_list->nlight,core_shader_group_liquid,texture);
		is_core=TRUE;
	}
	else {
		shader=&user_shaders[texture->shader_idx];
		is_core=FALSE;
	}
	
		// if we are not in this shader, then
		// change over
		
	if (shader!=gl_shader_current) {
	
			// set in the new program
			
		gl_shader_current=shader;
		glUseProgram(shader->program_obj);
			
			// set per-frame variables, only do this once
			// as they don't change per scene
		
		if (shader->need_frame_reset) {
			shader->need_frame_reset=FALSE;
			gl_shader_set_scene_variables(shader);
		}
	}
	
		// matrix uniforms
		
	gl_shader_set_draw_matrix_variables(shader);

		// bind the required attributes

	gl_shader_draw_execute_set_vertex_offset(shader,vertex_offset,stride);
	gl_shader_draw_execute_set_uv_offset(shader,uv_offset,stride);
	gl_shader_draw_execute_set_lmap_uv_offset(shader,lmap_uv_offset,stride);
	gl_shader_draw_execute_set_normal_offset(shader,normal_offset,stride);
	gl_shader_draw_execute_set_tangent_offset(shader,tangent_offset,stride);
	
		// setup variables

	gl_shader_hilite_override(shader,light_list);
	gl_shader_set_texture(shader,core_shader_group_liquid,texture,txt_idx,lmap_txt_idx,frame);
	gl_shader_set_poly_variables(shader,alpha);
	gl_shader_set_light_variables(shader,core_shader_group_liquid,is_core,light_list);
}

/* =======================================================

      Execute Model Shaders
      
======================================================= */

void gl_shader_draw_execute_model(texture_type *texture,int txt_idx,int frame,float alpha,int vertex_offset,int uv_offset,int tangent_offset,int normal_offset,int stride,view_glsl_light_list_type *light_list)
{
	bool						is_core;
	shader_type					*shader;
	
		// get shader based on number of lights
		
	if (texture->shader_idx==gl_shader_core_index) {
		shader=gl_core_shader_find_ptr(light_list->nlight,core_shader_group_model,texture);
		is_core=TRUE;
	}
	else {
		shader=&user_shaders[texture->shader_idx];
		is_core=FALSE;
	}
	
		// if we are not in this shader, then
		// change over
		
	if (shader!=gl_shader_current) {
	
			// set in the new program
			
		gl_shader_current=shader;
		glUseProgram(shader->program_obj);
			
			// set per-frame variables, only do this once
			// as they don't change per scene
		
		if (shader->need_frame_reset) {
			shader->need_frame_reset=FALSE;
			gl_shader_set_scene_variables(shader);
		}
	}
	
		// matrix uniforms
		
	gl_shader_set_draw_matrix_variables(shader);
	
		// bind the required attributes

	gl_shader_draw_execute_set_vertex_offset(shader,vertex_offset,stride);
	gl_shader_draw_execute_set_uv_offset(shader,uv_offset,stride);
	gl_shader_draw_execute_set_normal_offset(shader,normal_offset,stride);
	gl_shader_draw_execute_set_tangent_offset(shader,tangent_offset,stride);
	
		// setup variables

	gl_shader_hilite_override(shader,light_list);
	gl_shader_set_texture(shader,core_shader_group_model,texture,txt_idx,-1,frame);
	gl_shader_set_poly_variables(shader,alpha);
	gl_shader_set_diffuse_variables(shader,light_list);
	gl_shader_set_light_variables(shader,core_shader_group_model,is_core,light_list);
}

