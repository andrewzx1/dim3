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

extern model_type				model;
extern model_draw_setup			draw_setup;
extern animator_state_type		state;

/* =======================================================

      Setup Model Textures
      
======================================================= */

void model_start_texture(texture_type *texture)
{
	int			bitmap_gl_id;
	
		// blending
		
	if (texture->additive) {
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	}
	else {
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	}
	
		// get texture ids
		
	if (state.playing) {
		bitmap_gl_id=texture->frames[texture->animate.current_frame].bitmap.gl_id;
	}
	else {
		bitmap_gl_id=texture->frames[0].bitmap.gl_id;
	}
	
		// setup texturing
		
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D,bitmap_gl_id);
	
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);
	
	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_REPLACE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB,GL_SRC_COLOR);
	
	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_REPLACE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_ALPHA,GL_SRC_ALPHA);
}

void model_end_texture(texture_type *texture)
{
		// reset blending
		
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

		// turn off texturing
		
	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_2D);
	
		// reset texture combine
		
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
}

/* =======================================================

      Draw Model
      
======================================================= */

void draw_model_material(int mesh_idx,texture_type *texture,model_material_type *material)
{
	int					n,k,trig_count;
	float				vertexes[3*3],uvs[3*2];
	float				*pa,*pv,*pt;
    model_trig_type		*trig;

	trig_count=material->trig_count;
	if (trig_count==0) return;
	
	glColor3f(0.5f,0.5f,0.5f);

	model_start_texture(texture);
	
	trig=&model.meshes[mesh_idx].trigs[material->trig_start];

	for (n=0;n!=trig_count;n++) {

		if (vertex_check_hide_mask_trig(mesh_idx,trig)) {
			trig++;
			continue;
		}

		pv=vertexes;
		pt=uvs;

		for (k=0;k!=3;k++) {
			pa=draw_setup.mesh_arrays[mesh_idx].gl_vertex_array+(trig->v[k]*3);
			*pv++=*pa++;
			*pv++=*pa++;
			*pv++=*pa;
			*pt++=trig->gx[k];
			*pt++=trig->gy[k];
		}

		glVertexPointer(3,GL_FLOAT,0,vertexes);
		glTexCoordPointer(2,GL_FLOAT,0,uvs);

		glDrawArrays(GL_TRIANGLES,0,3);

		trig++;
	}
	
	model_end_texture(texture);
	
	glColor3f(1.0f,1.0f,1.0f);
}

void draw_model(int mesh_idx)
{
	int						n;
    texture_type			*texture;
	model_material_type		*material;
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
    
        // run through the opaque textures

	glDisable(GL_BLEND);

    texture=model.textures;
	material=mesh->materials;
    
    for (n=0;n!=max_model_texture;n++) {
		if (texture->frames[0].bitmap.alpha_mode!=alpha_mode_transparent) draw_model_material(mesh_idx,texture,material);
		texture++;
		material++;
	}
	
		// run through the transparent textures
		
	glEnable(GL_BLEND);

	glDepthMask(GL_FALSE);
	
	texture=model.textures;
	material=mesh->materials;
    
    for (n=0;n!=max_model_texture;n++) {
		if (texture->frames[0].bitmap.alpha_mode==alpha_mode_transparent) draw_model_material(mesh_idx,texture,material);
		texture++;
		material++;
	}
	
	glDepthMask(GL_TRUE);
    
	glDisable(GL_ALPHA_TEST);
}

