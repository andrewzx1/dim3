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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "interface.h"

extern map_type			map;
extern server_type		server;
extern view_type		view;
extern setup_type		setup;

/* =======================================================

      Texture and Alpha Utilities
      
======================================================= */

inline int render_model_get_texture_frame(model_type *mdl,model_draw *draw,int txt_idx)
{
	texture_type		*texture;

		// regular texture animation
		
	texture=&mdl->textures[txt_idx];
	if (texture->animate.on) return(texture->animate.current_frame);
	
		// if script based animation is on, replace
		// frame
		
	if (draw->textures[txt_idx].animation_on) draw->textures[txt_idx].frame=bitmap_texture_get_current_frame(texture,draw->textures[txt_idx].animation_reverse,game_time_get());
	return(draw->textures[txt_idx].frame);
}

inline float render_model_get_mesh_alpha(model_draw *draw,int mesh_idx)
{
	if (!draw->meshes[mesh_idx].fade.on) return(draw->alpha);
	return(draw->meshes[mesh_idx].fade.alpha);
}

/* =======================================================

      Model Colors and Normals
      
======================================================= */

void render_model_create_color_vertexes(model_type *mdl,int mesh_mask,model_draw *draw)
{
	int				n,k;
	float			fx,fy,fz;
	float			*vp,*cp;
	d3fpnt			cnt;
	d3col			col;
	matrix_type		mat;
	model_mesh_type	*mesh;

		// color lists for non-shader paths

	for (n=0;n!=mdl->nmesh;n++) {
		if ((mesh_mask&(0x1<<n))==0) continue;
		
		mesh=&mdl->meshes[n];
		
			// setup color list
		
		cp=draw->setup.mesh_arrays[n].gl_color_array;

			// debug draw
			
		if (setup.debug_on) {
		
			for (k=0;k!=mesh->nvertex;k++) {
				*cp++=1.0f;
				*cp++=1.0f;
				*cp++=1.0f;
			}
			
			continue;
		}
	
			// hilited meshes

		if ((mesh->no_lighting) || (draw->ui_lighting)) {

				// set vertexes to white

			for (k=0;k!=mesh->nvertex;k++) {
				*cp++=1.0f;
				*cp++=1.0f;
				*cp++=1.0f;
			}

			continue;
		}
		
			// ambient only
			
		if (draw->light_cache.count==0) {
		
				// set vertexes to ambient color

			gl_lights_calc_ambient_color(&col);
		
			for (k=0;k!=mesh->nvertex;k++) {
				*cp++=col.r;
				*cp++=col.g;
				*cp++=col.b;
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
				gl_lights_calc_color_light_cache_float(draw->light_cache.count,draw->light_cache.indexes,FALSE,*vp,*(vp+1),*(vp+2),cp);
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
				
				gl_lights_calc_color_light_cache_float(draw->light_cache.count,draw->light_cache.indexes,FALSE,(fx+cnt.x),(fy+cnt.y),(fz+cnt.z),cp);
				cp+=3;
			}
		}
	}
}

void render_model_create_normal_vertexes(model_type *mdl,int mesh_mask,model_draw *draw,bool shader_off)
{
	int				n,k,nvertex;
	float			*tl,*nl;
	matrix_type		mat;

	for (n=0;n!=mdl->nmesh;n++) {
		if ((mesh_mask&(0x1<<n))==0) continue;

			// create the normals for the pose

		model_create_draw_normals(mdl,n,&draw->setup,shader_off);

			// no rotate models (hand weapons)
			// need to fix tangent space

		if (draw->no_rot.on) {
			matrix_rotate_xzy(&mat,draw->no_rot.ang.x,draw->no_rot.ang.y,draw->no_rot.ang.z);

			tl=draw->setup.mesh_arrays[n].gl_tangent_array;
			nl=draw->setup.mesh_arrays[n].gl_normal_array;

			nvertex=mdl->meshes[n].nvertex;

			for (k=0;k!=nvertex;k++) {
				matrix_vertex_multiply_ignore_transform(&mat,tl,(tl+1),(tl+2));
				matrix_vertex_multiply_ignore_transform(&mat,nl,(nl+1),(nl+2));
				tl+=3;
				nl+=3;
			}
		}
	}
}

void render_model_diffuse_color_vertexes(model_type *mdl,int mesh_mask,model_draw *draw)
{
	int					n,k,nvertex;
	float				diffuse,min_diffuse,boost;
	float				*na,*cp;
	d3col				ambient_col;
	d3vct				diffuse_vct;
	model_mesh_type		*mesh;
	
		// setup diffuse
		
	if (!draw->ui_lighting) {
		gl_lights_calc_diffuse_vector(&draw->pnt,draw->light_cache.count,draw->light_cache.indexes,&diffuse_vct);

		gl_lights_calc_ambient_color(&ambient_col);
		min_diffuse=(ambient_col.r+ambient_col.g+ambient_col.b)*(0.33f*gl_diffuse_ambient_factor);
	}
	else {
		diffuse_vct.x=mdl->ui.diffuse_vct.x;
		diffuse_vct.y=mdl->ui.diffuse_vct.y;
		diffuse_vct.z=mdl->ui.diffuse_vct.z;
		min_diffuse=mdl->ui.min_diffuse;
	}
	
	boost=mdl->diffuse_boost;

		// run the colors
		
	for (n=0;n!=mdl->nmesh;n++) {
		if ((mesh_mask&(0x1<<n))==0) continue;
		
		mesh=&mdl->meshes[n];
		
			// is this a diffuse mesh?
			
		if ((!mesh->diffuse) || (mesh->no_lighting)) continue;
		
			// get colors and normals
		
		na=draw->setup.mesh_arrays[n].gl_normal_array;
		cp=draw->setup.mesh_arrays[n].gl_color_array;
		
			// run through the vertexes
			
		nvertex=mesh->nvertex;
		
		for (k=0;k!=nvertex;k++) {

				// get the diffuse from
				// the dot product and clamp it
				
			diffuse=(diffuse_vct.x*(*na++));
			diffuse+=(diffuse_vct.y*(*na++));
			diffuse+=(diffuse_vct.z*(*na++));

			diffuse=((diffuse+1.0f)*0.5f)+boost;

			if (diffuse<min_diffuse) diffuse=min_diffuse;
			if (diffuse>=1.0f) diffuse=1.0f;
		
				// apply diffuse
			
			*cp++=(*cp)*diffuse;
			*cp++=(*cp)*diffuse;
			*cp++=(*cp)*diffuse;
		}
	}
}

/* =======================================================

      Specific Model Drawing Arrays
      
======================================================= */

void render_model_vertex_object_no_shader(model_type *mdl,int mesh_idx,model_draw *draw,unsigned char *vertex_ptr)
{
	int					n,k,offset,stride;
	float				*gx,*gy,*pf,
						*va,*va_start,*ca,*ca_start;
	unsigned char		*vp,*pc;
	model_mesh_type		*mesh;
	model_poly_type		*poly;
	
	mesh=&mdl->meshes[mesh_idx];
	
	vp=vertex_ptr;

	va_start=draw->setup.mesh_arrays[mesh_idx].gl_vertex_array;
	ca_start=draw->setup.mesh_arrays[mesh_idx].gl_color_array;

	stride=draw->vbo[mesh_idx].vertex_stride;

	poly=mesh->polys;
	
	for (n=0;n!=mesh->npoly;n++) {
	
		gx=poly->gx;
		gy=poly->gy;

		for (k=0;k!=poly->ptsz;k++) {
			offset=poly->v[k]*3;
			
			va=va_start+offset;
			ca=ca_start+offset;

			pf=(float*)vp;

			*pf++=*va++;
			*pf++=*va++;
			*pf++=*va;

			*pf++=*gx++;
			*pf++=*gy++;

			pc=(unsigned char*)pf;
			*pc++=(unsigned char)((*ca++)*255.0f);
			*pc++=(unsigned char)((*ca++)*255.0f);
			*pc++=(unsigned char)((*ca)*255.0f);
			*pc=0xFF;

			vp+=stride;
		}

		poly++;
	}
}

void render_model_vertex_object_shader(model_type *mdl,int mesh_idx,model_draw *draw,unsigned char *vertex_ptr)
{
	int					n,k,offset,stride;
	float				*gx,*gy,*pf,
						*va,*va_start,*ta,*ta_start,*na,*na_start;
	unsigned char		*vp;
	model_mesh_type		*mesh;
	model_poly_type		*poly;
	
	mesh=&mdl->meshes[mesh_idx];
	
	vp=vertex_ptr;

	va_start=draw->setup.mesh_arrays[mesh_idx].gl_vertex_array;
	ta_start=draw->setup.mesh_arrays[mesh_idx].gl_tangent_array;
	na_start=draw->setup.mesh_arrays[mesh_idx].gl_normal_array;

	stride=draw->vbo[mesh_idx].vertex_stride;

	poly=mesh->polys;

	for (n=0;n!=mesh->npoly;n++) {
	
		gx=poly->gx;
		gy=poly->gy;

		for (k=0;k!=poly->ptsz;k++) {
			offset=poly->v[k]*3;
			
			va=va_start+offset;
			ta=ta_start+offset;
			na=na_start+offset;

			pf=(float*)vp;

			*pf++=*va++;
			*pf++=*va++;
			*pf++=*va;

			*pf++=*gx++;
			*pf++=*gy++;

			*pf++=*ta++;
			*pf++=*ta++;
			*pf++=*ta;

			*pf++=*na++;
			*pf++=*na++;
			*pf++=*na;

			vp+=stride;
		}

		poly++;
	}
}

/* =======================================================

      Model Drawing Arrays
      
======================================================= */

bool render_model_initialize_vertex_objects(model_type *mdl,int mesh_idx,model_draw *draw)
{
	int					stride;
	unsigned char		*vertex_ptr;
	bool				shader_on;
	model_mesh_type		*mesh;
	
 		// construct VBO
		// non-shaders have vertex, uv, color
		// shaders have vertex, uv, tangent space

		// also remember some offsets for later pointer work
		
	shader_on=view_shader_on()&&(!draw->no_shader);
		
	mesh=&mdl->meshes[mesh_idx];

	view_bind_model_vertex_object(draw,mesh_idx);
	vertex_ptr=(unsigned char*)view_map_model_vertex_object();
	if (vertex_ptr==NULL) {
		view_unbind_model_vertex_object();
		return(FALSE);
	}
	
		// non-shader drawing requires
		// vertexes, UVs, and colors

	if (!shader_on) {
		render_model_vertex_object_no_shader(mdl,mesh_idx,draw,vertex_ptr);
	}

		// shader drawing requires
		// vertexes, UVs, and tangent space

	else {
		render_model_vertex_object_shader(mdl,mesh_idx,draw,vertex_ptr);
	}

		// unmap VBO

	view_unmap_model_vertex_object();

		// set the pointers
		// glow maps use two texture units

	stride=draw->vbo[mesh_idx].vertex_stride;

	glVertexPointer(3,GL_FLOAT,stride,(GLvoid*)0);

	glClientActiveTexture(GL_TEXTURE1);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,stride,(GLvoid*)(3*sizeof(float)));

	glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,stride,(GLvoid*)(3*sizeof(float)));
	
	if (!shader_on) {
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4,GL_UNSIGNED_BYTE,stride,(GLvoid*)((3+2)*sizeof(float)));
	}

	return(TRUE);
}

void render_model_release_vertex_objects(void)
{
	view_unbind_model_vertex_object();

	glDisableClientState(GL_COLOR_ARRAY);
		
	glClientActiveTexture(GL_TEXTURE1);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glClientActiveTexture(GL_TEXTURE0);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

/* =======================================================

      Draw Model Polys
      
======================================================= */

void render_model_opaque_normal(model_type *mdl,int mesh_idx,model_draw *draw)
{
	int						n,v_idx,frame,txt_idx;
	unsigned char			*cull_ptr;
	model_mesh_type			*mesh;
	model_poly_type			*poly;
	model_draw_mesh_type	*draw_mesh;
    texture_type			*texture;
	
	mesh=&mdl->meshes[mesh_idx];
	draw_mesh=&draw->meshes[mesh_idx];
	
		// setup drawing

	glDisable(GL_BLEND);
		
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);

	gl_texture_opaque_start();

		// run through the polys

	v_idx=0;
	cull_ptr=draw->setup.mesh_arrays[mesh_idx].poly_cull_array;

	poly=mesh->polys;

	for (n=0;n!=mesh->npoly;n++) {
	
			// polygon culling
			
		if (*cull_ptr++==0x1) {
			v_idx+=poly->ptsz;
			poly++;
			continue;
		}

			// is this poly texture opaque

		txt_idx=poly->txt_idx;

		if (!draw_mesh->textures[txt_idx].opaque) {
			v_idx+=poly->ptsz;
			poly++;
			continue;
		}
	
			// set texture

		frame=draw_mesh->textures[txt_idx].frame;
		texture=&mdl->textures[txt_idx];

		gl_texture_opaque_set(texture->frames[frame].bitmap.gl_id);

			// draw poly
			
		glDrawArrays(GL_TRIANGLE_FAN,v_idx,poly->ptsz);
		
		v_idx+=poly->ptsz;
		poly++;

		view.count.model_poly++;
	}

	gl_texture_opaque_end();
}

void render_model_opaque_shader(model_type *mdl,int mesh_idx,model_draw *draw,view_light_list_type *light_list)
{
	int						n,v_idx,frame,txt_idx,stride;
	unsigned char			*cull_ptr;
	model_mesh_type			*mesh;
	model_poly_type			*poly;
 	model_draw_mesh_type	*draw_mesh;
	texture_type			*texture;
	
	mesh=&mdl->meshes[mesh_idx];
	draw_mesh=&draw->meshes[mesh_idx];
	
		// setup drawing

	glDisable(GL_BLEND);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
	
	glEnable(GL_DEPTH_TEST); 
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);

	gl_shader_draw_start();
	gl_shader_draw_reset_normal_tangent_attrib();

	stride=draw->vbo[mesh_idx].vertex_stride;

		// run through the polys

	v_idx=0;
	cull_ptr=draw->setup.mesh_arrays[mesh_idx].poly_cull_array;

	poly=mesh->polys;

	for (n=0;n!=mesh->npoly;n++) {

			// polygon culling
			
		if (*cull_ptr++==0x1) {
			v_idx+=poly->ptsz;
			poly++;
			continue;
		}

			// is this poly texture opaque

		txt_idx=poly->txt_idx;

		if (!draw_mesh->textures[txt_idx].opaque) {
			v_idx+=poly->ptsz;
			poly++;
			continue;
		}

			// set texture

		frame=draw_mesh->textures[txt_idx].frame;
		texture=&mdl->textures[txt_idx];

			// set hilite and tint on per
			// mesh basis
					
		light_list->hilite=((mesh->no_lighting)||(draw->ui_lighting));
		if (!mesh->diffuse) {
			light_list->diffuse_boost=1.0f;
		}
		else {
			light_list->diffuse_boost=mdl->diffuse_boost;
		}

			// run the shader
			
		gl_shader_draw_execute(core_shader_group_model,texture,txt_idx,frame,-1,1.0f,light_list,(5*sizeof(float)),(8*sizeof(float)),stride);
		glDrawArrays(GL_TRIANGLE_FAN,v_idx,poly->ptsz);
		
		v_idx+=poly->ptsz;
		poly++;

		view.count.model_poly++;
	}
			
	gl_shader_draw_end();
}

void render_model_transparent_normal(model_type *mdl,int mesh_idx,model_draw *draw)
{
	int						n,v_idx,frame,txt_idx;
	bool					cur_additive,is_additive;
	model_mesh_type			*mesh;
	model_poly_type			*poly;
 	model_draw_mesh_type	*draw_mesh;
    texture_type			*texture;
	
	mesh=&mdl->meshes[mesh_idx];
	draw_mesh=&draw->meshes[mesh_idx];

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

	cur_additive=FALSE;
	
		// run through the polys

	v_idx=0;
	poly=mesh->polys;

	for (n=0;n!=mesh->npoly;n++) {

			// is this poly texture transparent

		txt_idx=poly->txt_idx;

		if (!draw_mesh->textures[txt_idx].transparent) {
			v_idx+=poly->ptsz;
			poly++;
			continue;
		}
	
			// set texture

		frame=draw_mesh->textures[txt_idx].frame;
		texture=&mdl->textures[txt_idx];

			// blending changes
			
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

		gl_texture_transparent_set(texture->frames[frame].bitmap.gl_id,draw_mesh->alpha);

			// draw poly
			
		glDrawArrays(GL_TRIANGLE_FAN,v_idx,poly->ptsz);
		
		v_idx+=poly->ptsz;
		poly++;

		view.count.model_poly++;
	}
	
	if (cur_additive) glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	gl_texture_transparent_end();
}

void render_model_transparent_shader(model_type *mdl,int mesh_idx,model_draw *draw,view_light_list_type *light_list)
{
	int						n,v_idx,frame,txt_idx,stride;
	bool					cur_additive,is_additive;
	model_mesh_type			*mesh;
	model_poly_type			*poly;
 	model_draw_mesh_type	*draw_mesh;
    texture_type			*texture;
	
	mesh=&mdl->meshes[mesh_idx];
	draw_mesh=&draw->meshes[mesh_idx];

		// setup drawing

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
		
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);
	
	gl_shader_draw_start();
	gl_shader_draw_reset_normal_tangent_attrib();

	stride=draw->vbo[mesh_idx].vertex_stride;
	
		// minimize state changes

	cur_additive=FALSE;

		// run through the polys

	v_idx=0;
	poly=mesh->polys;

	for (n=0;n!=mesh->npoly;n++) {

			// is this poly texture transparent

		txt_idx=poly->txt_idx;

		if (!draw_mesh->textures[txt_idx].transparent) {
			v_idx+=poly->ptsz;
			poly++;
			continue;
		}
	
			// get texture

		frame=draw_mesh->textures[txt_idx].frame;
		texture=&mdl->textures[txt_idx];

			// blending changes
			
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

			// set hilite and diffuse on a per
			// mesh basis
		
		light_list->hilite=((mesh->no_lighting)||(draw->ui_lighting));
		if (!mesh->diffuse) {
			light_list->diffuse_boost=1.0f;
		}
		else {
			light_list->diffuse_boost=mdl->diffuse_boost;
		}

			// draw poly
			
		gl_shader_draw_execute(core_shader_group_model,texture,txt_idx,frame,-1,draw_mesh->alpha,light_list,(5*sizeof(float)),(8*sizeof(float)),stride);
		glDrawArrays(GL_TRIANGLE_FAN,v_idx,poly->ptsz);
		
		v_idx+=poly->ptsz;
		poly++;

		view.count.model_poly++;
	}
	
	if (cur_additive) glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			
	gl_shader_draw_end();
}

void render_model_glow(model_type *mdl,int mesh_idx,model_draw *draw)
{
	int						n,v_idx,frame,txt_idx;
	model_mesh_type			*mesh;
 	model_poly_type			*poly;
	model_draw_mesh_type	*draw_mesh;
    texture_type			*texture;
	
	mesh=&mdl->meshes[mesh_idx];
	draw_mesh=&draw->meshes[mesh_idx];

		// setup drawing

	glDisable(GL_BLEND);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glEnable(GL_DEPTH_TEST); 
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);

	gl_texture_glow_start();

		// run through the polys

	v_idx=0;
	poly=mesh->polys;

	for (n=0;n!=mesh->npoly;n++) {

			// is this poly texture glow

		txt_idx=poly->txt_idx;

		if (!draw_mesh->textures[txt_idx].glow) {
			v_idx+=poly->ptsz;
			poly++;
			continue;
		}

			// get texture

		frame=draw_mesh->textures[txt_idx].frame;
		texture=&mdl->textures[txt_idx];

		gl_texture_glow_set(texture->frames[frame].bitmap.gl_id,texture->frames[frame].glowmap.gl_id,texture->glow.current_color);

			// draw poly
		
		glDrawArrays(GL_TRIANGLE_FAN,v_idx,poly->ptsz);
		
		v_idx+=poly->ptsz;
		poly++;

		view.count.model_poly++;
	}
	
	gl_texture_glow_end();
}

/* =======================================================

      Debug for Testing Normals
      
======================================================= */

void render_model_debug_normals(model_type *mdl,int mesh_idx,model_draw *draw)
{
	int						n,k,idx;
	float					*va,*na;
	d3pnt					pnt1,pnt2;
	d3col					col;
	model_mesh_type			*mesh;
 	model_poly_type			*poly;
	
	glEnable(GL_DEPTH_TEST); 
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	
	col.r=1.0f;
	col.g=0.0f;
	col.b=1.0f;

	mesh=&mdl->meshes[mesh_idx];
	poly=mesh->polys;

	for (n=0;n!=mesh->npoly;n++) {
	
		for (k=0;k!=poly->ptsz;k++) {
			idx=poly->v[k]*3;

			va=draw->setup.mesh_arrays[mesh_idx].gl_vertex_array+idx;
			pnt1.x=(int)*va++;
			pnt1.y=(int)*va++;
			pnt1.z=(int)*va;

			na=draw->setup.mesh_arrays[mesh_idx].gl_normal_array+idx;
			pnt2.x=pnt1.x+(int)(100.0f*(*na++));
			pnt2.y=pnt1.y+(int)(100.0f*(*na++));
			pnt2.z=pnt1.z+(int)(100.0f*(*na));

			view_primitive_3D_line(&col,1.0f,pnt1.x,pnt1.y,pnt1.z,pnt2.x,pnt2.y,pnt2.z);
		}

		poly++;
	}
}

/* =======================================================

      Setup Model and Vertex Lists
      
======================================================= */

void render_model_setup(model_draw *draw,int tick)
{
	int					n,k,frame,txt_idx;
	bool				texture_hits[max_model_texture];
	model_type			*mdl;
	model_mesh_type		*mesh;
	model_poly_type		*poly;
    texture_type		*texture;
	
		// get model
	
	if ((draw->model_idx==-1) || (!draw->on)) return;
	
	mdl=server.model_list.models[draw->model_idx];

		// setup animated textures

	model_setup_animated_textures(mdl,tick);

		// setup the rendering mesh mask

	draw->render_mesh_mask=draw->mesh_mask;

	for (n=0;n!=mdl->nmesh;n++) {
		if (draw->meshes[n].fade.on) {
			draw->render_mesh_mask|=(0x1<<n);
		}
	}

		// hasn't build vertex list yet

	draw->built_vertex_list=FALSE;

		// run through the meshes and
		// setup flags

		// we setup glow, opaque, and transparent
		// flag for textures, meshes, and drawing as
		// a whole to quickly eliminate some paths

		// we also setup current texture frames
		// and current mesh alphas (can be changed
		// by fades.)

	draw->has_opaque=FALSE;
	draw->has_transparent=FALSE;
	draw->has_glow=FALSE;
	
	for (n=0;n!=mdl->nmesh;n++) {
		if ((draw->render_mesh_mask&(0x1<<n))==0) continue;
		
		mesh=&mdl->meshes[n];

			// get alpha
		
		draw->meshes[n].alpha=render_model_get_mesh_alpha(draw,n);

			// setup textures

		texture=mdl->textures;
		
		for (k=0;k!=max_model_texture;k++) {
			texture_hits[k]=FALSE;

			frame=render_model_get_texture_frame(mdl,draw,k);
			draw->meshes[n].textures[k].frame=frame;

			draw->meshes[n].textures[k].glow=(texture->frames[frame].glowmap.gl_id!=-1);
			if ((texture->frames[frame].bitmap.alpha_mode==alpha_mode_transparent) || (draw->meshes[n].alpha!=1.0f)) {
				draw->meshes[n].textures[k].opaque=FALSE;
				draw->meshes[n].textures[k].transparent=TRUE;
			}
			else {
				draw->meshes[n].textures[k].opaque=TRUE;
				draw->meshes[n].textures[k].transparent=FALSE;
			}

			texture++;
		}
		
			// check for any transparent textures
			// or shaders
		
		draw->meshes[n].has_opaque=FALSE;
		draw->meshes[n].has_glow=FALSE;

		if (draw->meshes[n].alpha!=1.0f) {
			draw->meshes[n].has_transparent=TRUE;
			draw->has_transparent=TRUE;
		}
		else {
			draw->meshes[n].has_transparent=FALSE;
		}

		poly=mesh->polys;

		for (k=0;k!=mesh->npoly;k++) {

				// only check once per texture

			txt_idx=poly->txt_idx;
			if (texture_hits[txt_idx]) {
				poly++;
				continue;
			}

			texture_hits[txt_idx]=TRUE;

			texture=&mdl->textures[txt_idx];
			frame=draw->meshes[n].textures[txt_idx].frame;

				// look for glows, opaque, and transparent
			
			if (texture->frames[frame].glowmap.gl_id!=-1) {
				draw->meshes[n].has_glow=TRUE;
				draw->has_glow=TRUE;
			}
	
			if (texture->frames[frame].bitmap.alpha_mode==alpha_mode_transparent) {
				draw->meshes[n].has_transparent=TRUE;
				draw->has_transparent=TRUE;
			}
			else {
				draw->meshes[n].has_opaque=TRUE;
				draw->has_opaque=TRUE;
			}
			
			poly++;
		}
	}
}

void render_model_build_vertex_lists(model_draw *draw,bool always_build)
{
	int					n,k,t,idx;
	bool				shader_off;
	float				f,*va,*na;
	unsigned char		*cull_ptr;
	d3fpnt				pnt,camera_pnt;
	d3vct				normal;
	model_type			*mdl;
	model_mesh_type		*mesh;
	model_poly_type		*poly;

		// already built list?
		// this happens if we've already draw for
		// the model but need it again for shadows
		// or other rendering

	if ((!always_build) && (draw->built_vertex_list)) return;

	draw->built_vertex_list=TRUE;

		// get model
		
	if ((draw->model_idx==-1) || (!draw->on)) return;

	mdl=server.model_list.models[draw->model_idx];

		// create vertex and uv lists
		
	for (n=0;n!=mdl->nmesh;n++) {
		if ((draw->render_mesh_mask&(0x1<<n))==0) continue;

			// build model vertex list

		model_create_draw_vertexes(mdl,n,&draw->setup);
		if (draw->resize!=1.0f) model_resize_draw_vertex(mdl,n,draw->resize,&draw->setup);
		if (draw->flip_x) model_flip_draw_vertex(mdl,n,&draw->setup);

			// translate vertex to view
			
		model_translate_draw_vertex(mdl,n,draw->pnt.x,draw->pnt.y,draw->pnt.z,&draw->setup);
	}

		// setup the colors and normals
		// shaders don't need color list

	shader_off=((!view_shader_on()) || (draw->no_shader));
	
	render_model_create_normal_vertexes(mdl,draw->render_mesh_mask,draw,shader_off);
	
	if (shader_off) {
		render_model_create_color_vertexes(mdl,draw->render_mesh_mask,draw);
		render_model_diffuse_color_vertexes(mdl,draw->render_mesh_mask,draw);
	}

		// setup culling

	camera_pnt.x=(float)view.render->camera.pnt.x;
	camera_pnt.y=(float)view.render->camera.pnt.y;
	camera_pnt.z=(float)view.render->camera.pnt.z;

	for (n=0;n!=mdl->nmesh;n++) {
		if ((draw->render_mesh_mask&(0x1<<n))==0) continue;

		mesh=&mdl->meshes[n];

			// no culling
			// initial array is set to 0s so we
			// can just skip out

		if ((mesh->never_cull) || (draw->no_culling)) continue;

			// culled

		poly=mesh->polys;
		cull_ptr=draw->setup.mesh_arrays[n].poly_cull_array;

		for (k=0;k!=mesh->npoly;k++) {

				// get poly normal
				// don't need to normalize or divide
				// out average for normal, but have to
				// for point

			pnt.x=pnt.y=pnt.z=0.0f;
			normal.x=normal.y=normal.z=0.0f;

			for (t=0;t!=poly->ptsz;t++) {
				idx=poly->v[t]*3;

				va=draw->setup.mesh_arrays[n].gl_vertex_array+idx;
				pnt.x+=*va++;
				pnt.y+=*va++;
				pnt.z+=*va;

				na=draw->setup.mesh_arrays[n].gl_normal_array+idx;
				normal.x+=*na++;
				normal.y+=*na++;
				normal.z+=*na;
			}

			f=(float)poly->ptsz;
			pnt.x=(pnt.x/f)-camera_pnt.x;
			pnt.y=(pnt.y/f)-camera_pnt.y;
			pnt.z=(pnt.z/f)-camera_pnt.z;

			*cull_ptr++=(((normal.x*pnt.x)+(normal.y*pnt.y)+(normal.z*pnt.z))>0.0f)?0x1:0x0;

			poly++;
		}
	}
}

/* =======================================================

      Draw Normal Model
      
======================================================= */

void render_model_opaque(model_draw *draw)
{
	int						n;
	bool					shader_on;
	model_type				*mdl;
	view_light_list_type	light_list;
	
		// count model
		
	view.count.model++;

		// any opaque?

	if ((draw->model_idx==-1) || (!draw->on)) return;
	if (!draw->has_opaque) return;
	
		// get model

	mdl=server.model_list.models[draw->model_idx];

		// start glsl lighting
		
	shader_on=view_shader_on()&&(!draw->no_shader);

	if (shader_on) gl_lights_build_model_light_list(mdl,draw,&light_list);

		// draw opaque materials
	
	for (n=0;n!=mdl->nmesh;n++) {
		if ((draw->render_mesh_mask&(0x1<<n))==0) continue;
		if (!draw->meshes[n].has_opaque) continue;
	
			// create VBO for this mesh

		if (!render_model_initialize_vertex_objects(mdl,n,draw)) return;

			// render opaque segments

		if (!shader_on) {
			render_model_opaque_normal(mdl,n,draw);
		}
		else {
			render_model_opaque_shader(mdl,n,draw,&light_list);
		}
		
			// render glow segments

		if (draw->has_glow) {
			if (draw->meshes[n].has_glow) render_model_glow(mdl,n,draw);
		}

		render_model_release_vertex_objects();
		
			// debugging for normals
		
	//	render_model_debug_normals(mdl,n,draw);
	}
}

void render_model_transparent(model_draw *draw)
{
	int						n;
	bool					shader_on;
	model_type				*mdl;
	view_light_list_type	light_list;

		// any transparent?

	if ((draw->model_idx==-1) || (!draw->on)) return;
	if (!draw->has_transparent) return;
	
		// get model

	mdl=server.model_list.models[draw->model_idx];
	
		// start lighting
		
	shader_on=view_shader_on()&&(!draw->no_shader);

	if (shader_on) gl_lights_build_model_light_list(mdl,draw,&light_list);

		// draw transparent materials

	for (n=0;n!=mdl->nmesh;n++) {
		if ((draw->render_mesh_mask&(0x1<<n))==0) continue;
		if (!draw->meshes[n].has_transparent) continue;

			// setup the VBO

		if (!render_model_initialize_vertex_objects(mdl,n,draw)) return;	

			// draw transparent mesh

		if ((!shader_on) || (draw->no_shader)) {
			render_model_transparent_normal(mdl,n,draw);
		}
		else {
			render_model_transparent_shader(mdl,n,draw,&light_list);
		}
			
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
	float			rang,vertexes[12];
	model_type		*mdl;
	
		// get model
		
	if ((draw->model_idx==-1) || (!draw->on)) return;

		// check distance obscure
		
	if (map.optimize.obscure_dist.model!=0) {
		if (distance_get(draw->pnt.x,draw->pnt.y,draw->pnt.z,view.render->camera.pnt.x,view.render->camera.pnt.y,view.render->camera.pnt.z)>map.optimize.obscure_dist.model) return;
	}

	mdl=server.model_list.models[draw->model_idx];

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

		// get the vertexes

	vertexes[0]=(float)lx;
	vertexes[1]=(float)ty;
	vertexes[2]=(float)lz;

	vertexes[3]=(float)rx;
	vertexes[4]=(float)ty;
	vertexes[5]=(float)rz;

	vertexes[6]=(float)rx;
	vertexes[7]=(float)by;
	vertexes[8]=(float)rz;

	vertexes[9]=(float)lx;
	vertexes[10]=(float)by;
	vertexes[11]=(float)lz;

		// setup draw

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
			
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glColor4f(col->r,col->g,col->b,1.0f);

		// draw target
		
	glVertexPointer(3,GL_FLOAT,0,(GLvoid*)vertexes);
	glDrawArrays(GL_LINE_LOOP,0,4);
}
