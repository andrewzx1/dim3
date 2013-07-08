/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View Sky Boxes

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

extern map_type			map;
extern setup_type		setup;
extern view_type		view;

/* =======================================================

      Draw Cube Sky
      
======================================================= */

void sky_draw_cube_setup(void)
{
	/* supergumba -- will need new version
	int					sz;
    float				f_radius,txt_fact;
	float				*vertex_ptr,*uv_ptr;

		// construct VBO

	sz=6*4;
	view_create_sky_vertex_object(((sz*(3+2+4))*sizeof(float)));

	view_bind_sky_vertex_object();

	vertex_ptr=(float*)view_map_sky_vertex_object();
	if (vertex_ptr==NULL) {
		view_unbind_sky_vertex_object();
		return;
	}

	uv_ptr=vertex_ptr+(sz*3);

	txt_fact=map.sky.txt_fact;

		// setup cube quads
		
	f_radius=(float)map.sky.radius;
		
		// top
		
	if (map.sky.fill!=-1) {
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=-f_radius;

		*uv_ptr++=0.0f;
		*uv_ptr++=txt_fact;

		*vertex_ptr++=-f_radius;
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=f_radius;

		*uv_ptr++=0.0f;
		*uv_ptr++=0.0f;

		*vertex_ptr++=f_radius;
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=-f_radius;

		*uv_ptr++=txt_fact;
		*uv_ptr++=txt_fact;

		*vertex_ptr++=f_radius;
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=f_radius;

		*uv_ptr++=txt_fact;
		*uv_ptr++=0.0f;
	}
	
		// bottom
		
	if (map.sky.bottom_fill!=-1) {
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=f_radius;
		*vertex_ptr++=-f_radius;

		*uv_ptr++=0.0f;
		*uv_ptr++=txt_fact;

		*vertex_ptr++=-f_radius;
		*vertex_ptr++=f_radius;
		*vertex_ptr++=f_radius;

		*uv_ptr++=0.0f;
		*uv_ptr++=0.0f;

		*vertex_ptr++=f_radius;
		*vertex_ptr++=f_radius;
		*vertex_ptr++=-f_radius;

		*uv_ptr++=txt_fact;
		*uv_ptr++=txt_fact;

		*vertex_ptr++=f_radius;
		*vertex_ptr++=f_radius;
		*vertex_ptr++=f_radius;

		*uv_ptr++=txt_fact;
		*uv_ptr++=0.0f;
	}
	
		// north
		
	if (map.sky.north_fill!=-1) {
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=f_radius;

		*uv_ptr++=0.00f;
		*uv_ptr++=0.00f;

		*vertex_ptr++=-f_radius;
		*vertex_ptr++=f_radius;
		*vertex_ptr++=f_radius;

		*uv_ptr++=0.00f;
		*uv_ptr++=txt_fact;

		*vertex_ptr++=f_radius;
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=f_radius;

		*uv_ptr++=txt_fact;
		*uv_ptr++=0.00f;

		*vertex_ptr++=f_radius;
		*vertex_ptr++=f_radius;
		*vertex_ptr++=f_radius;

		*uv_ptr++=txt_fact;
		*uv_ptr++=txt_fact;
	}
	
		// east

	if (map.sky.east_fill!=-1) {
		*vertex_ptr++=f_radius;
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=f_radius;

		*uv_ptr++=0.0f;
		*uv_ptr++=0.0f;

		*vertex_ptr++=f_radius;
		*vertex_ptr++=f_radius;
		*vertex_ptr++=f_radius;

		*uv_ptr++=0.0f;
		*uv_ptr++=txt_fact;

		*vertex_ptr++=f_radius;
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=-f_radius;

		*uv_ptr++=txt_fact;
		*uv_ptr++=0.0f;

		*vertex_ptr++=f_radius;
		*vertex_ptr++=f_radius;
		*vertex_ptr++=-f_radius;

		*uv_ptr++=txt_fact;
		*uv_ptr++=txt_fact;
	}
	
		// south

	if (map.sky.south_fill!=-1) {
		*vertex_ptr++=f_radius;
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=-f_radius;

		*uv_ptr++=0.0f;
		*uv_ptr++=0.0f;

		*vertex_ptr++=f_radius;
		*vertex_ptr++=f_radius;
		*vertex_ptr++=-f_radius;

		*uv_ptr++=0.0f;
		*uv_ptr++=txt_fact;

		*vertex_ptr++=-f_radius;
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=-f_radius;

		*uv_ptr++=txt_fact;
		*uv_ptr++=0.0f;

		*vertex_ptr++=-f_radius;
		*vertex_ptr++=f_radius;
		*vertex_ptr++=-f_radius;

		*uv_ptr++=txt_fact;
		*uv_ptr++=txt_fact;
	}
	
		// west

	if (map.sky.west_fill!=-1) {
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=-f_radius;

		*uv_ptr++=0.0f;
		*uv_ptr++=0.0f;

		*vertex_ptr++=-f_radius;
		*vertex_ptr++=f_radius;
		*vertex_ptr++=-f_radius;

		*uv_ptr++=0.0f;
		*uv_ptr++=txt_fact;

		*vertex_ptr++=-f_radius;
		*vertex_ptr++=-f_radius;
		*vertex_ptr++=f_radius;

		*uv_ptr++=txt_fact;
		*uv_ptr++=0.0f;

		*vertex_ptr++=-f_radius;
		*vertex_ptr++=f_radius;
		*vertex_ptr++=f_radius;

		*uv_ptr++=txt_fact;
		*uv_ptr++=txt_fact;
	}
 
		// unmap and unbind vbo

	view_unmap_sky_vertex_object();
	view_unbind_sky_vertex_object();
	*/
}

void sky_draw_cube(void)
{
	/* supergumba -- will need new version

    int					offset;
	d3col				col;
	texture_type		*texture;
					
		// setup texture
		
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	
	col.r=col.g=col.b=1.0f;
	
		// bind VBO

	view_bind_sky_vertex_object();

		// draw cube sides

	gl_shader_draw_execute_simple_bitmap_start(3,0,(((6*4)*3)*sizeof(float)),0,&col,1.0f);

	offset=0;

	if (map.sky.fill!=-1) {
		texture=&map.textures[map.sky.fill];
		gl_shader_draw_execute_simple_bitmap_set_texture(texture->frames[texture->animate.current_frame].bitmap.gl_id);
		
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
		glDrawArrays(GL_TRIANGLE_STRIP,0,4);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
		
		offset+=4;
	}

		// bottom
		
	if (map.sky.bottom_fill!=-1) {
		texture=&map.textures[map.sky.bottom_fill];
		gl_shader_draw_execute_simple_bitmap_set_texture(texture->frames[texture->animate.current_frame].bitmap.gl_id);
		
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
		glDrawArrays(GL_TRIANGLE_STRIP,offset,4);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
		
		offset+=4;
	}
	
		// north
		
	if (map.sky.north_fill!=-1) {
		texture=&map.textures[map.sky.north_fill];
		gl_shader_draw_execute_simple_bitmap_set_texture(texture->frames[texture->animate.current_frame].bitmap.gl_id);
		
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
		glDrawArrays(GL_TRIANGLE_STRIP,offset,4);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
		
		offset+=4;
	}

		// east

	if (map.sky.east_fill!=-1) {
		texture=&map.textures[map.sky.east_fill];
		gl_shader_draw_execute_simple_bitmap_set_texture(texture->frames[texture->animate.current_frame].bitmap.gl_id);
		
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
		glDrawArrays(GL_TRIANGLE_STRIP,offset,4);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
		
		offset+=4;
	}
	
		// south

	if (map.sky.south_fill!=-1) {
		texture=&map.textures[map.sky.south_fill];
		gl_shader_draw_execute_simple_bitmap_set_texture(texture->frames[texture->animate.current_frame].bitmap.gl_id);
		
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
		glDrawArrays(GL_TRIANGLE_STRIP,offset,4);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
		
		offset+=4;
	}
	
		// west

	if (map.sky.west_fill!=-1) {
		texture=&map.textures[map.sky.west_fill];
		gl_shader_draw_execute_simple_bitmap_set_texture(texture->frames[texture->animate.current_frame].bitmap.gl_id);
		
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
		glDrawArrays(GL_TRIANGLE_STRIP,offset,4);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	}

		// unbind the vbo

	gl_shader_draw_execute_simple_bitmap_end();

	view_unbind_sky_vertex_object();
	*/
}

/* =======================================================

      Initialize Sky Drawing
      
======================================================= */

void sky_draw_init(void)
{
	/* supergumba -- will need new version

		// is there a sky?
	
	if (!map.sky.on) return;

		// init vertex list

	switch (map.sky.type) {
	
		case st_dome_panoramic:
			sky_draw_dome_panoramic_setup();
			break;

		case st_dome_hemisphere:
			sky_draw_dome_hemisphere_setup();
			break;
			
		case st_cube:
			sky_draw_cube_setup();
			break;
	
	}
	*/
}

void sky_draw_release(void)
{
	// supergumba -- will need new version
//	if (map.sky.on) view_dispose_sky_vertex_object();
}

/* =======================================================

      Draw Sky Mainline
      
======================================================= */

void sky_draw(void)
{
	/* supergumba -- will need new version
	switch (map.sky.type) {
	
		case st_dome_panoramic:
			sky_draw_dome_panoramic();
			break;

		case st_dome_hemisphere:
			sky_draw_dome_hemisphere();
			break;
			
		case st_cube:
			sky_draw_cube();
			break;
	
	}
	*/
}


