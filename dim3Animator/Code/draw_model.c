/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Draw A Model

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
	#include "dim3animator.h"
#endif

#include "glue.h"
#include "interface.h"

int								cur_txt_idx;

extern model_type				model;
extern model_draw_setup			draw_setup;
extern animator_state_type		state;

/* =======================================================

      Setup Model Textures
      
======================================================= */

void model_texture_start(void)
{
		// setup texturing

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);

	glColor3f(1.0f,1.0f,1.0f);

	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);
	
	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_REPLACE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB,GL_SRC_COLOR);
	
	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_REPLACE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_ALPHA,GL_SRC_ALPHA);

		// no texture set

	cur_txt_idx=-1;
}

void model_texture_set(int txt_idx)
{
	GLuint			gl_id;
	texture_type	*texture;
	
	if (txt_idx==cur_txt_idx) return;

	cur_txt_idx=txt_idx;
	texture=&model.textures[txt_idx];

		// blending
		
	if (texture->additive) {
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	}
	else {
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	}
	
		// get texture ids
		
	if (state.playing) {
		gl_id=texture->frames[texture->animate.current_frame].bitmap.gl_id;
	}
	else {
		gl_id=texture->frames[0].bitmap.gl_id;
	}
	
	glBindTexture(GL_TEXTURE_2D,gl_id);
}

void model_texture_end(void)
{
		// reset blending and combine
		
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

		// turn off texturing
		
	glDisable(GL_TEXTURE_2D);
}

/* =======================================================

      Draw Model
      
======================================================= */

void draw_model_mesh_triangles(int mesh_idx,bool opaque)
{
	int					n,k;
	float				vertexes[8*3],uvs[8*2];
	float				*pa,*pv,*pt;
	model_mesh_type		*mesh;
    model_poly_type		*poly;

	mesh=&model.meshes[mesh_idx];

	for (n=0;n!=mesh->npoly;n++) {
		poly=&mesh->polys[n];

			// hidden?

		if (vertex_mask_check_hide_poly(mesh_idx,poly)) continue;

			// opaque?

		if (opaque) {
			if (model.textures[poly->txt_idx].frames[0].bitmap.alpha_mode==alpha_mode_transparent) continue;
		}
		else {
			if (model.textures[poly->txt_idx].frames[0].bitmap.alpha_mode!=alpha_mode_transparent) continue;
		}

			// switch texture?

		model_texture_set(poly->txt_idx);

			// setup and draw triangle

		pv=vertexes;
		pt=uvs;

		for (k=0;k!=poly->ptsz;k++) {
			pa=draw_setup.mesh_arrays[mesh_idx].gl_vertex_array+(poly->v[k]*3);
			*pv++=*pa++;
			*pv++=*pa++;
			*pv++=*pa;
			*pt++=poly->gx[k];
			*pt++=poly->gy[k];
		}

		glVertexPointer(3,GL_FLOAT,0,vertexes);
		glTexCoordPointer(2,GL_FLOAT,0,uvs);

		glDrawArrays(GL_TRIANGLES,0,poly->ptsz);
	}
}

void draw_model(int mesh_idx)
{
	model_mesh_type			*mesh;
	
		// model vertexes and normal arrays
		
	mesh=&model.meshes[mesh_idx];
	
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
	
		// setup the current texture frames
		
	if (state.playing) {
		model_setup_animated_textures(&model,time_get());
	}
	
	glColor4f(1.0f,1.0f,1.0f,1.0f);

	model_texture_start();
    
        // run through the opaque textures

	glDisable(GL_BLEND);

	draw_model_mesh_triangles(mesh_idx,TRUE);
	
		// run through the transparent textures
		
	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);
	
	draw_model_mesh_triangles(mesh_idx,FALSE);
	
	glDepthMask(GL_TRUE);
    
	glDisable(GL_ALPHA_TEST);

		// finish texturing

	model_texture_end();	
}

