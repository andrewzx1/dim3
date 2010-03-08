/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Model Drawing

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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "consoles.h"
#include "models.h"
#include "lights.h"
#include "video.h"
#include "timing.h"

extern bool				dim3_debug;

extern map_type			map;
extern server_type		server;
extern view_type		view;
extern setup_type		setup;

extern bool fog_solid_on(void);

int	t_offset,b_offset,n_offset;

/* =======================================================

      Model Colors and Normals
      
======================================================= */

void render_model_create_color_vertexes(model_type *mdl,int mesh_mask,model_draw *draw)
{
	int				n,k;
	float			*cp,*vp,fx,fy,fz;
	bool			only_ambient;
	d3fpnt			cnt;
	d3col			col;
	matrix_type		mat;
	model_mesh_type	*mesh;

		// setup vertex calcing

	only_ambient=!gl_lights_calc_vertex_setup_model(draw);

		// need color lists

	for (n=0;n!=mdl->nmesh;n++) {
		if ((mesh_mask&(0x1<<n))==0) continue;
		
			// shaders don't need color lists

		mesh=&mdl->meshes[n];
		if ((!dim3_debug) && (mesh->draw.only_shaders)) continue;
		
			// setup color list
			
		cp=draw->setup.mesh_arrays[n].gl_color_array;

			// debug draw
			
		if (dim3_debug) {
		
			for (k=0;k!=mesh->nvertex;k++) {
				*cp++=1.0f;
				*cp++=1.0f;
				*cp++=1.0f;
			}
			
			continue;
		}
		
			// hilited meshes

		if (mesh->no_lighting) {

			if (mesh->tintable) {
				for (k=0;k!=mesh->nvertex;k++) {
					*cp++=draw->tint.r;
					*cp++=draw->tint.g;
					*cp++=draw->tint.b;
				}
			}
			else {
				for (k=0;k!=mesh->nvertex;k++) {
					*cp++=1.0f;
					*cp++=1.0f;
					*cp++=1.0f;
				}
			}

			continue;
		}
		
			// ambient only
			
		if (only_ambient) {
		
			gl_lights_get_ambient(&col,FALSE);
		
			for (k=0;k!=mesh->nvertex;k++) {
				*cp++=col.r*draw->tint.r;
				*cp++=col.g*draw->tint.g;
				*cp++=col.b*draw->tint.b;
			}
			
			continue;
		}

			// vertex lighting

			// if it's a no rotation model, we need
			// to re-apply the rotation so the lighting is
			// in the right space

		vp=draw->setup.mesh_arrays[n].gl_vertex_array;

		if (!draw->no_rot.on) {

			for (k=0;k!=mesh->nvertex;k++) {
				gl_lights_calc_vertex((double)*vp,(double)*(vp+1),(double)*(vp+2),FALSE,cp);
				cp+=3;
				vp+=3;
			}

		}
		else {
			cnt.x=(float)draw->no_rot.center.x;
			cnt.y=(float)draw->no_rot.center.y;
			cnt.z=(float)draw->no_rot.center.z;

			matrix_rotate_xzy(&mat,draw->no_rot.ang.x,draw->no_rot.ang.y,draw->no_rot.ang.z);

			for (k=0;k!=mesh->nvertex;k++) {
				fx=(*vp++)-cnt.x;
				fy=(*vp++)-cnt.y;
				fz=(*vp++)-cnt.z;
				matrix_vertex_multiply(&mat,&fx,&fy,&fz);
				
				gl_lights_calc_vertex((double)(fx+cnt.x),(double)(fy+cnt.y),(double)(fz+cnt.z),FALSE,cp);
				cp+=3;
			}
		}

			// tints

		if (mesh->tintable) {

			cp=draw->setup.mesh_arrays[n].gl_color_array;

			for (k=0;k!=mesh->nvertex;k++) {
				*cp++=(*cp)*draw->tint.r;
				*cp++=(*cp)*draw->tint.g;
				*cp++=(*cp)*draw->tint.b;
			}
		}

	}
}

void render_model_create_normal_vertexes(model_type *mdl,int mesh_mask,model_draw *draw)
{
	int				n,k,ts_count;
	float			*tl,*bl,*nl;
	matrix_type		mat;

	for (n=0;n!=mdl->nmesh;n++) {
		if ((mesh_mask&(0x1<<n))==0) continue;

			// non-shader drawing doesn't need normals

		if ((dim3_debug) || (mdl->meshes[n].draw.has_no_shader)) continue;

			// create the normals for the pose

		model_create_draw_normals(mdl,n,&draw->setup);

			// no rotate models (hand weapons)
			// need to fix tangent space

		if (draw->no_rot.on) {
		
			matrix_rotate_xzy(&mat,draw->no_rot.ang.x,draw->no_rot.ang.y,draw->no_rot.ang.z);

			tl=draw->setup.mesh_arrays[n].gl_tangent_array;
			bl=draw->setup.mesh_arrays[n].gl_binormal_array;
			nl=draw->setup.mesh_arrays[n].gl_normal_array;

			ts_count=mdl->meshes[n].ntrig*3;

			for (k=0;k!=ts_count;k++) {
				matrix_vertex_multiply(&mat,tl,(tl+1),(tl+2));
				matrix_vertex_multiply(&mat,bl,(bl+1),(bl+2));
				matrix_vertex_multiply(&mat,nl,(nl+1),(nl+2));
				tl+=3;
				bl+=3;
				nl+=3;
			}
		}
	}
}

/* =======================================================

      Model Drawing Arrays
      
======================================================= */

bool render_model_initialize_vertex_objects(model_type *mdl,int mesh_idx,model_draw *draw)
{
	int				n,k,offset,mem_sz;
	float			*vl,*ul,*cl,*tl,*bl,*nl,
					*vp,*cp,*gx,*gy,
					*vp_start,*cp_start,
					*vertex_ptr;
    model_trig_type	*trig;
	model_mesh_type	*mesh;
	
 		// construct VBO
		// non-shaders have vertex, uv, color
		// shaders have vertex, uv, tangent space

		// also remember some offsets for later pointer work

	mesh=&mdl->meshes[mesh_idx];

	if (mesh->draw.has_no_shader) {
		mem_sz=(mesh->ntrig*3)*(3+2+3);

		draw->setup.vbo_offset.color=((mesh->ntrig*3)*(3+2))*sizeof(float);
		draw->setup.vbo_offset.tangent=0;
		draw->setup.vbo_offset.binormal=0;
		draw->setup.vbo_offset.normal=0;
	}
	else {
		mem_sz=(mesh->ntrig*3)*(3+2+3+3+3);

		draw->setup.vbo_offset.color=0;
		draw->setup.vbo_offset.tangent=((mesh->ntrig*3)*(3+2))*sizeof(float);
		draw->setup.vbo_offset.binormal=((mesh->ntrig*3)*(3+2+3))*sizeof(float);
		draw->setup.vbo_offset.normal=((mesh->ntrig*3)*(3+2+3+3))*sizeof(float);
	}

	vertex_ptr=view_bind_map_next_vertex_object(mem_sz);
	if (vertex_ptr==NULL) return(FALSE);
	
		// build the vertexes, indexes, and colors
		// into the VBO, and the tangent space as
		// pointers

	vl=vertex_ptr;
	ul=vertex_ptr+((mesh->ntrig*3)*3);

	vp_start=draw->setup.mesh_arrays[mesh_idx].gl_vertex_array;
	cp_start=draw->setup.mesh_arrays[mesh_idx].gl_color_array;

		// non-shader drawing requires
		// vertexes, UVs, and colors

	trig=mesh->trigs;

	if (mesh->draw.has_no_shader) {

		cl=vertex_ptr+((mesh->ntrig*3)*(3+2));

		for (n=0;n!=mesh->ntrig;n++) {
		
			gx=trig->gx;
			gy=trig->gy;

			for (k=0;k!=3;k++) {
				offset=trig->v[k]*3;
				
				vp=vp_start+offset;
				cp=cp_start+offset;

				*vl++=*vp++;
				*vl++=*vp++;
				*vl++=*vp;

				*ul++=*gx++;
				*ul++=*gy++;

				*cl++=*cp++;
				*cl++=*cp++;
				*cl++=*cp;
			}

			trig++;
		}
	}

		// shader drawing requires
		// vertexes, UVs, and tangent space

	else {

		for (n=0;n!=mesh->ntrig;n++) {
		
			gx=trig->gx;
			gy=trig->gy;

			for (k=0;k!=3;k++) {
				offset=trig->v[k]*3;
				
				vp=vp_start+offset;

				*vl++=*vp++;
				*vl++=*vp++;
				*vl++=*vp;

				*ul++=*gx++;
				*ul++=*gy++;
			}

			trig++;
		}

			// tangent space already in trig-vertex array

		mem_sz=((mesh->ntrig*3)*3)*sizeof(float);

		tl=vertex_ptr+((mesh->ntrig*3)*(3+2));
		memmove(tl,draw->setup.mesh_arrays[mesh_idx].gl_tangent_array,mem_sz);

		bl=vertex_ptr+((mesh->ntrig*3)*(3+2+3));
		memmove(bl,draw->setup.mesh_arrays[mesh_idx].gl_binormal_array,mem_sz);

		nl=vertex_ptr+((mesh->ntrig*3)*(3+2+3+3));
		memmove(nl,draw->setup.mesh_arrays[mesh_idx].gl_normal_array,mem_sz);
	}

		// unmap VBO

	view_unmap_current_vertex_object();

		// set the pointers
		// glow maps use two texture units

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,(void*)0);

	glClientActiveTexture(GL_TEXTURE1);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(void*)(((mesh->ntrig*3)*3)*sizeof(float)));
	
	glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(void*)(((mesh->ntrig*3)*3)*sizeof(float)));

	return(TRUE);
}

inline void render_model_enable_color_array(model_draw *draw)
{
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(3,GL_FLOAT,0,(void*)draw->setup.vbo_offset.color);
}

inline void render_model_disable_color_array(void)
{
	glDisableClientState(GL_COLOR_ARRAY);
}

void render_model_release_vertex_objects(void)
{
	glClientActiveTexture(GL_TEXTURE1);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glClientActiveTexture(GL_TEXTURE0);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glDisableClientState(GL_VERTEX_ARRAY);

	view_unbind_current_vertex_object();
}

/* =======================================================

      Draw Model Trigs
      
======================================================= */

void render_model_opaque_simple_trigs(model_type *mdl,int mesh_idx,model_draw *draw)
{
	int						n,frame,trig_count,trig_idx;
	float					alpha;
	bool					enabled;
	model_mesh_type			*mesh;
    texture_type			*texture;
	model_material_type		*material;
	
	mesh=&mdl->meshes[mesh_idx];
	if ((!dim3_debug) && (mesh->draw.only_shaders)) return;
	
		// setup drawing

	glDisable(GL_BLEND);
		
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);

	enabled=FALSE;

	gl_texture_opaque_start();

		// run through the materials

	for (n=0;n!=max_model_texture;n++) {
	
		texture=&mdl->textures[n];
		material=&mesh->materials[n];

			// skip shader textures

		if ((!dim3_debug) && (!mesh->no_lighting) && (texture->shader_idx!=-1)) continue;
	
			// any opaque trigs?
			
		if (texture->animate.on) {
			frame=texture->animate.current_frame;
		}
		else {
			frame=(int)draw->cur_texture_frame[n];
		}
		
		alpha=draw->alpha;
		if (draw->mesh_fades[mesh_idx].on) alpha=draw->mesh_fades[mesh_idx].alpha;
			
		if ((texture->frames[frame].bitmap.alpha_mode==alpha_mode_transparent) || (alpha!=1.0)) continue;

			// trig count

		trig_count=material->trig_count;
		if (trig_count==0) continue;

		trig_idx=material->trig_start*3;

			// first color pointer enable?

		if (!enabled) {
			enabled=TRUE;
			render_model_enable_color_array(draw);
		}

			// draw texture

		gl_texture_opaque_set(texture->frames[frame].bitmap.gl_id);
		glDrawArrays(GL_TRIANGLES,trig_idx,(trig_count*3));
	}

	if (enabled) render_model_disable_color_array();
			
	gl_texture_opaque_end();
}

void render_model_opaque_shader_trigs(model_type *mdl,int mesh_idx,model_draw *draw,view_light_list_type *light_list)
{
	int						n,trig_count,frame,trig_idx;
	float					alpha;
	model_mesh_type			*mesh;
    texture_type			*texture;
	model_material_type		*material;
	
	mesh=&mdl->meshes[mesh_idx];
	if ((dim3_debug) || (mesh->draw.has_no_shader)) return;
	
		// setup drawing

	glDisable(GL_BLEND);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
	
	glEnable(GL_DEPTH_TEST); 
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);

	gl_shader_draw_start();

		// run through the materials

	for (n=0;n!=max_model_texture;n++) {
	
		texture=&mdl->textures[n];
		material=&mesh->materials[n];

			// skip non-shader textures

		if ((texture->shader_idx==-1) || (mesh->no_lighting)) continue;
		
			// any opaque trigs?
			
		if (texture->animate.on) {
			frame=texture->animate.current_frame;
		}
		else {
			frame=(int)draw->cur_texture_frame[n];
		}
		
		alpha=draw->alpha;
		if (draw->mesh_fades[mesh_idx].on) alpha=draw->mesh_fades[mesh_idx].alpha;
			
		if ((texture->frames[frame].bitmap.alpha_mode==alpha_mode_transparent) || (alpha!=1.0)) continue;

			// don't draw if no trigs

		trig_count=material->trig_count;
		if (trig_count==0) continue;

		trig_idx=material->trig_start*3;
		
			// run the shader
			
		gl_shader_draw_execute(FALSE,texture,n,frame,-1,1.0f,light_list,NULL,&draw->tint,NULL,&draw->setup.vbo_offset);
		
		glDrawArrays(GL_TRIANGLES,trig_idx,(trig_count*3));
	}
			
	gl_shader_draw_end();
}

void render_model_transparent_simple_trigs(model_type *mdl,int mesh_idx,model_draw *draw)
{
	int						n,frame,trig_count,trig_idx;
	float					alpha;
	bool					enabled,cur_additive,is_additive;
	model_mesh_type			*mesh;
    texture_type			*texture;
	model_material_type		*material;

	mesh=&mdl->meshes[mesh_idx];
	if ((!dim3_debug) && (mesh->draw.only_shaders)) return;

		// setup drawing

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
		
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);
	
	gl_texture_transparent_start();

		// minimize state changes

	enabled=FALSE;
	cur_additive=FALSE;
	
		// run through textures

	for (n=0;n!=max_model_texture;n++) {
	
		texture=&mdl->textures[n];
		material=&mesh->materials[n];

			// skip shader textures

		if ((!dim3_debug) && (!mesh->no_lighting) && (texture->shader_idx!=-1)) continue;
	
			// any transparent trigs?
			
		if (texture->animate.on) {
			frame=texture->animate.current_frame;
		}
		else {
			frame=(int)draw->cur_texture_frame[n];
		}
		
		alpha=draw->alpha;
		if (draw->mesh_fades[mesh_idx].on) alpha=draw->mesh_fades[mesh_idx].alpha;
		
		if (!((texture->frames[frame].bitmap.alpha_mode==alpha_mode_transparent) || (alpha!=1.0))) continue;
	
			// don't draw if no trigs

		trig_count=material->trig_count;
		if (trig_count==0) continue;

		trig_idx=material->trig_start*3;
		
			// transparent textures
			
		is_additive=(mesh->blend_add) || (texture->additive);
		if (is_additive!=cur_additive) {
			cur_additive=is_additive;
			if (cur_additive) {
				glBlendFunc(GL_SRC_ALPHA,GL_ONE);
			}
			else {
				glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			}
		}

			// first color pointer enable?

		if (!enabled) {
			enabled=TRUE;
			render_model_enable_color_array(draw);
		}

			// draw texture

		gl_texture_transparent_set(texture->frames[frame].bitmap.gl_id,alpha);
		glDrawArrays(GL_TRIANGLES,trig_idx,(trig_count*3));
	}

	if (enabled) render_model_disable_color_array();

	gl_texture_transparent_end();
}

void render_model_transparent_shader_trigs(model_type *mdl,int mesh_idx,model_draw *draw,view_light_list_type *light_list)
{
	int						n,frame,trig_count,trig_idx;
	float					alpha;
	bool					cur_additive,is_additive;
	model_mesh_type			*mesh;
    texture_type			*texture;
	model_material_type		*material;

	mesh=&mdl->meshes[mesh_idx];
	if ((dim3_debug) || (mesh->draw.has_no_shader)) return;

		// setup drawing

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
		
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);
	
	gl_shader_draw_start();
	
		// minimize state changes

	cur_additive=FALSE;

		// run through the materials

	for (n=0;n!=max_model_texture;n++) {
	
		texture=&mdl->textures[n];
		material=&mesh->materials[n];

			// skip non-shader textures

		if ((texture->shader_idx==-1) || (mesh->no_lighting)) continue;
		
			// any transparent trigs?
			
		if (texture->animate.on) {
			frame=texture->animate.current_frame;
		}
		else {
			frame=(int)draw->cur_texture_frame[n];
		}
		
		alpha=draw->alpha;
		if (draw->mesh_fades[mesh_idx].on) alpha=draw->mesh_fades[mesh_idx].alpha;
			
		if (!((texture->frames[frame].bitmap.alpha_mode==alpha_mode_transparent) || (alpha!=1.0))) continue;

			// don't draw if no trigs

		trig_count=material->trig_count;
		if (trig_count==0) continue;

		trig_idx=material->trig_start*3;
		
			// transparent textures
			
		is_additive=(mesh->blend_add) || (texture->additive);
		if (is_additive!=cur_additive) {
			cur_additive=is_additive;
			if (cur_additive) {
				glBlendFunc(GL_SRC_ALPHA,GL_ONE);
			}
			else {
				glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			}
		}
		
			// run the shader
			
		gl_shader_draw_execute(FALSE,texture,n,frame,-1,alpha,light_list,NULL,&draw->tint,NULL,&draw->setup.vbo_offset);
		
		glDrawArrays(GL_TRIANGLES,trig_idx,(trig_count*3));
	}
			
	gl_shader_draw_end();
}

void render_model_glow_trigs(model_type *mdl,int mesh_idx,model_draw *draw)
{
	int						n,frame,trig_count,trig_idx;
	model_mesh_type			*mesh;
    texture_type			*texture;
	model_material_type		*material;
	
	mesh=&mdl->meshes[mesh_idx];

		// setup drawing

	glDisable(GL_BLEND);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glEnable(GL_DEPTH_TEST); 
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);

	gl_texture_glow_start();
	
		// run through the materials

	for (n=0;n!=max_model_texture;n++) {
	
			// only draw glow textures

		texture=&mdl->textures[n];
		
		if (texture->animate.on) {
			frame=texture->animate.current_frame;
		}
		else {
			frame=(int)draw->cur_texture_frame[n];
		}

		if (texture->frames[frame].glowmap.gl_id==-1) continue;

		material=&mesh->materials[n];
	
			// trig count

		trig_count=material->trig_count;
		if (trig_count==0) continue;

		trig_idx=material->trig_start*3;

			// draw glow texture
		
		gl_texture_glow_set(texture->frames[frame].bitmap.gl_id,texture->frames[frame].glowmap.gl_id,texture->glow.current_color);
		
		glDrawArrays(GL_TRIANGLES,trig_idx,(trig_count*3));
	}
	
	gl_texture_glow_end();
}

/* =======================================================

      Setup Model and Vertex Lists
      
======================================================= */

void render_model_setup(model_draw *draw)
{
	int					n,t,frame;
	float				alpha;
	model_type			*mdl;
	model_mesh_type		*mesh;
    texture_type		*texture;
	
		// get model

	mdl=model_find_uid(draw->uid);
	if (mdl==NULL) return;

		// setup animated textures

	model_setup_animated_textures(mdl,game_time_get());

		// setup the rendering mesh mask

	draw->render_mesh_mask=draw->mesh_mask;

	for (n=0;n!=mdl->nmesh;n++) {
		if (draw->mesh_fades[n].on) {
			draw->render_mesh_mask|=(0x1<<n);
		}
	}

		// check for opaque/transparent draws

	draw->has_opaque=FALSE;
	draw->has_transparent=FALSE;
	
	for (n=0;n!=mdl->nmesh;n++) {
		if ((draw->render_mesh_mask&(0x1<<n))==0) continue;
		
			// check for any transparent textures
			// or shaders
		
		mesh=&mdl->meshes[n];
			
		mesh->draw.has_no_shader=TRUE;
		mesh->draw.only_shaders=TRUE;
			
		texture=mdl->textures;
		
		for (t=0;t!=max_model_texture;t++) {
		
				// texture used in this mesh?
				
			if (mdl->meshes[n].materials[t].trig_count==0) {
				texture++;
				continue;
			}
			
				// check for fade
				
			alpha=draw->alpha;
			if (draw->mesh_fades[n].on) alpha=draw->mesh_fades[n].alpha;
				
				// check texture for transparencies
				
			if (texture->animate.on) {
				frame=texture->animate.current_frame;
			}
			else {
				frame=(int)draw->cur_texture_frame[t];
			}
	
			if ((texture->frames[frame].bitmap.alpha_mode==alpha_mode_transparent) || (alpha!=1.0)) {
				draw->has_transparent=TRUE;
			}
			else {
				draw->has_opaque=TRUE;
			}
			
				// check for shaders
				
			if ((texture->shader_idx!=-1) && (!mesh->no_lighting)) {
				mesh->draw.has_no_shader=FALSE;
			}
			else {
				mesh->draw.only_shaders=FALSE;
			}
			
			texture++;
		}
		
		if (mesh->draw.has_no_shader) mesh->draw.only_shaders=FALSE;
		
		if ((draw->has_transparent) && (draw->has_opaque)) break;
	}
}

void render_model_build_vertex_lists(model_draw *draw)
{
	int					n;
	model_type			*mdl;
	
		// get model

	mdl=model_find_uid(draw->uid);
	if (mdl==NULL) return;

		// create vertex and uv lists
		
	for (n=0;n!=mdl->nmesh;n++) {
		if ((draw->render_mesh_mask&(0x1<<n))==0) continue;

			// build model vertex list
			
		model_create_draw_vertexes(mdl,n,&draw->setup);
		if (draw->resize!=1) model_resize_draw_vertex(mdl,n,draw->resize,&draw->setup);
		if (draw->flip_x) model_flip_draw_vertex(mdl,n,&draw->setup);

			// translate vertex to view
			
		model_translate_draw_vertex(mdl,n,draw->pnt.x,draw->pnt.y,draw->pnt.z,&draw->setup);
	}

		// setup the colors and normals

	render_model_create_color_vertexes(mdl,draw->render_mesh_mask,draw);
	render_model_create_normal_vertexes(mdl,draw->render_mesh_mask,draw);
}

/* =======================================================

      Draw Normal Model
      
======================================================= */

void render_model_opaque(model_draw *draw)
{
	int						n;
	model_type				*mdl;
	view_light_list_type	light_list;

		// any opaque?

	if (!draw->has_opaque) return;
	
		// get model

	mdl=model_find_uid(draw->uid);
	if (mdl==NULL) return;

		// start lighting

	gl_lights_build_from_model(draw,&light_list);

		// draw opaque materials

	for (n=0;n!=mdl->nmesh;n++) {
		if ((draw->render_mesh_mask&(0x1<<n))==0) continue;

			// create VBO for this mesh

		if (!render_model_initialize_vertex_objects(mdl,n,draw)) return;

			// render opaque segments

		render_model_opaque_simple_trigs(mdl,n,draw);
		if (!dim3_debug) render_model_opaque_shader_trigs(mdl,n,draw,&light_list);

			// render glow segments

		render_model_glow_trigs(mdl,n,draw);

		render_model_release_vertex_objects();
	}
}

void render_model_transparent(model_draw *draw)
{
	int						n;
	model_type				*mdl;
	view_light_list_type	light_list;

		// any transparent?

	if (!draw->has_transparent) return;
	
		// get model

	mdl=model_find_uid(draw->uid);
	if (mdl==NULL) return;
	
		// start lighting

	gl_lights_build_from_model(draw,&light_list);

		// draw transparent materials

	for (n=0;n!=mdl->nmesh;n++) {
		if ((draw->render_mesh_mask&(0x1<<n))==0) continue;

			// setup the VBO

		if (!render_model_initialize_vertex_objects(mdl,n,draw)) return;	

			// draw transparent mesh

		render_model_transparent_simple_trigs(mdl,n,draw);
		if (!dim3_debug) render_model_transparent_shader_trigs(mdl,n,draw,&light_list);
			
			// release the vbo

		render_model_release_vertex_objects();
	}
}

/* =======================================================

      Draw Target Model
      
======================================================= */

void render_model_target(model_draw *draw,d3col *col)
{
	int				ty,by,lx,rx,lz,rz,wid,xadd,zadd;
	float			rang;
	float			*vertex_ptr;
	model_type		*mdl;
	
		// get model

	mdl=model_find_uid(draw->uid);
	if (mdl==NULL) return;

		// get draw coordinates

	ty=draw->pnt.y-mdl->view_box.size.y;
	by=draw->pnt.y;

	rang=angle_find(view.render->camera.pnt.x,view.render->camera.pnt.z,draw->pnt.x,draw->pnt.z);

	wid=(mdl->view_box.size.x+mdl->view_box.size.z)>>2;

	lx=-wid;
	rx=wid;
	lz=rz=0;

	rotate_2D_point_center(&lx,&lz,rang);
	rotate_2D_point_center(&rx,&rz,rang);

	lx+=draw->pnt.x;
	rx+=draw->pnt.x;
	lz+=draw->pnt.z;
	rz+=draw->pnt.z;

		// move towards camera

	angle_get_movement(rang,wid,&xadd,&zadd);

	lx-=xadd;
	rx-=xadd;
	lz-=zadd;
	rz-=zadd;

		// build vertexes

	vertex_ptr=view_bind_map_next_vertex_object(4*3);
	if (vertex_ptr==NULL) return;

		// get the vertexes

	*vertex_ptr++=(float)lx;
	*vertex_ptr++=(float)ty;
	*vertex_ptr++=(float)lz;

	*vertex_ptr++=(float)rx;
	*vertex_ptr++=(float)ty;
	*vertex_ptr++=(float)rz;

	*vertex_ptr++=(float)rx;
	*vertex_ptr++=(float)by;
	*vertex_ptr++=(float)rz;

	*vertex_ptr++=(float)lx;
	*vertex_ptr++=(float)by;
	*vertex_ptr++=(float)lz;

  	view_unmap_current_vertex_object();

		// setup draw

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
			
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glColor4f(col->r,col->g,col->b,1.0f);

		// draw target
		
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,(void*)0);

	glDrawArrays(GL_LINE_LOOP,0,4);

	glDisableClientState(GL_VERTEX_ARRAY);

		// unbind the vbo

	view_unbind_current_vertex_object();
}
