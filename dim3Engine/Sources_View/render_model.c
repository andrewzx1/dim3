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

      Model Colors and Normals
      
======================================================= */

void render_model_create_color_vertexes(model_type *mdl,int mesh_mask,model_draw *draw)
{
	int				n,k;
	float			fx,fy,fz;
	float			*cp,*vp;
	d3fpnt			cnt;
	d3col			col;
	matrix_type		mat;
	model_mesh_type	*mesh;

		// need color lists

	for (n=0;n!=mdl->nmesh;n++) {
		if ((mesh_mask&(0x1<<n))==0) continue;
		
		mesh=&mdl->meshes[n];
		
			// setup color list
		
		cp=draw->setup.mesh_arrays[n].gl_color_array;

			// debug draw
			
		if (view.debug.on) {
		
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
				gl_lights_calc_color_light_cache(draw->light_cache.count,draw->light_cache.indexes,FALSE,(double)*vp,(double)*(vp+1),(double)*(vp+2),cp);
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
				
				gl_lights_calc_color_light_cache(draw->light_cache.count,draw->light_cache.indexes,FALSE,(double)(fx+cnt.x),(double)(fy+cnt.y),(double)(fz+cnt.z),cp);
				cp+=3;
			}
		}
	}
}

void render_model_create_normal_vertexes(model_type *mdl,int mesh_mask,model_draw *draw,bool shader_off)
{
	int				n,k,ts_count;
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

			ts_count=mdl->meshes[n].ntrig*3;

			for (k=0;k!=ts_count;k++) {
				matrix_vertex_multiply(&mat,tl,(tl+1),(tl+2));
				matrix_vertex_multiply(&mat,nl,(nl+1),(nl+2));
				tl+=3;
				nl+=3;
			}
		}
	}
}

/* =======================================================

      Specific Model Drawing Arrays
      
======================================================= */

void render_model_vertex_object_no_shader(model_type *mdl,int mesh_idx,model_draw *draw,float *vertex_ptr)
{
	int					n,k,offset;
	float				*gx,*gy,*vp,*cp,*vl,*ul,*cl,
						*vp_start,*cp_start;
	model_trig_type		*trig;
	model_mesh_type		*mesh;
	
	mesh=&mdl->meshes[mesh_idx];
	
	vl=vertex_ptr;
	ul=vertex_ptr+((mesh->ntrig*3)*3);
	cl=vertex_ptr+((mesh->ntrig*3)*(3+2));

	vp_start=draw->setup.mesh_arrays[mesh_idx].gl_vertex_array;
	cp_start=draw->setup.mesh_arrays[mesh_idx].gl_color_array;

	trig=mesh->trigs;
	
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

void render_model_vertex_object_no_shader_diffuse(model_type *mdl,int mesh_idx,model_draw *draw,float *vertex_ptr)
{
	int					n,k,offset;
	float				diffuse,min_diffuse,boost,
						*gx,*gy,*vp,*cp,*vl,*ul,*cl,*nl,
						*vp_start,*cp_start;
	d3vct				diffuse_vct;
	d3col				ambient_col;
	model_trig_type		*trig;
	model_mesh_type		*mesh;
	
	mesh=&mdl->meshes[mesh_idx];
	
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

		// create the vertex object
	
	vl=vertex_ptr;
	ul=vertex_ptr+((mesh->ntrig*3)*3);
	cl=vertex_ptr+((mesh->ntrig*3)*(3+2));

	vp_start=draw->setup.mesh_arrays[mesh_idx].gl_vertex_array;
	cp_start=draw->setup.mesh_arrays[mesh_idx].gl_color_array;
	nl=draw->setup.mesh_arrays[mesh_idx].gl_normal_array;

	trig=mesh->trigs;
	
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
			
				// get the diffuse from
				// the dot product and clamp it
				
			diffuse=(diffuse_vct.x*(*nl))+(diffuse_vct.y*(*(nl+1)))+(diffuse_vct.z*(*(nl+2)));
			diffuse=((diffuse+1.0f)*0.5f)+boost;
			if (diffuse<min_diffuse) diffuse=min_diffuse;

			nl+=3;
		
				// apply diffuse
				
			*cl++=(*cp++)*diffuse;
			*cl++=(*cp++)*diffuse;
			*cl++=(*cp)*diffuse;
		}

		trig++;
	}
}

void render_model_vertex_object_shader(model_type *mdl,int mesh_idx,model_draw *draw,float *vertex_ptr)
{
	int					n,k,offset,mem_sz;
	float				*gx,*gy,*vp,*vl,*ul,*tl,*nl,
						*vp_start;
	model_trig_type		*trig;
	model_mesh_type		*mesh;
	
	mesh=&mdl->meshes[mesh_idx];
	
	vl=vertex_ptr;
	ul=vertex_ptr+((mesh->ntrig*3)*3);

	vp_start=draw->setup.mesh_arrays[mesh_idx].gl_vertex_array;

	trig=mesh->trigs;

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

	nl=vertex_ptr+((mesh->ntrig*3)*(3+2+3));
	memmove(nl,draw->setup.mesh_arrays[mesh_idx].gl_normal_array,mem_sz);
}

/* =======================================================

      Model Drawing Arrays
      
======================================================= */

bool render_model_initialize_vertex_objects(model_type *mdl,int mesh_idx,model_draw *draw)
{
	int					mem_sz;
	float				*vertex_ptr;
	bool				shader_on;
	model_mesh_type		*mesh;
	
 		// construct VBO
		// non-shaders have vertex, uv, color
		// shaders have vertex, uv, tangent space

		// also remember some offsets for later pointer work
		
	shader_on=view_shader_on();
		
	mesh=&mdl->meshes[mesh_idx];

	if ((!shader_on) || (draw->no_shader)) {
		mem_sz=(mesh->ntrig*3)*(3+2+3);

		draw->setup.vbo_offset.color=((mesh->ntrig*3)*(3+2))*sizeof(float);
		draw->setup.vbo_offset.tangent=0;
		draw->setup.vbo_offset.normal=0;
	}
	else {
		mem_sz=(mesh->ntrig*3)*(3+2+3+3);

		draw->setup.vbo_offset.color=0;
		draw->setup.vbo_offset.tangent=((mesh->ntrig*3)*(3+2))*sizeof(float);
		draw->setup.vbo_offset.normal=((mesh->ntrig*3)*(3+2+3))*sizeof(float);
	}

	vertex_ptr=view_bind_map_next_vertex_object(mem_sz);
	if (vertex_ptr==NULL) return(FALSE);
	
		// non-shader drawing requires
		// vertexes, UVs, and colors
		// and dim3 generates the diffuse

	if ((!shader_on) || (draw->no_shader)) {
		if ((mesh->diffuse) && (!mesh->no_lighting)) {
			render_model_vertex_object_no_shader_diffuse(mdl,mesh_idx,draw,vertex_ptr);
		
		}
		else {
			render_model_vertex_object_no_shader(mdl,mesh_idx,draw,vertex_ptr);
		}
	}

		// shader drawing requires
		// vertexes, UVs, and tangent space

	else {
		render_model_vertex_object_shader(mdl,mesh_idx,draw,vertex_ptr);
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

static inline void render_model_enable_color_array(model_draw *draw)
{
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(3,GL_FLOAT,0,(void*)draw->setup.vbo_offset.color);
}

static inline void render_model_disable_color_array(void)
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

void render_model_opaque_normal(model_type *mdl,int mesh_idx,model_draw *draw)
{
	int						n,frame,trig_count,trig_idx;
	bool					enabled;
	model_mesh_type			*mesh;
	model_draw_mesh_type	*draw_mesh;
    texture_type			*texture;
	model_material_type		*material;
	
	mesh=&mdl->meshes[mesh_idx];
	draw_mesh=&draw->meshes[mesh_idx];
	
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
	
			// any opaque trigs?
			
		if (draw_mesh->materials[n].has_transparent) continue;
	
			// get texture
			
		texture=&mdl->textures[n];
		material=&mesh->materials[n];
		
		frame=draw_mesh->materials[n].frame;

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
		
		view.count.poly+=trig_count;
	}

	if (enabled) render_model_disable_color_array();
			
	gl_texture_opaque_end();
}

void render_model_opaque_shader(model_type *mdl,int mesh_idx,model_draw *draw,view_light_list_type *light_list)
{
	int						n,trig_count,frame,trig_idx;
	model_mesh_type			*mesh;
 	model_draw_mesh_type	*draw_mesh;
	texture_type			*texture;
	model_material_type		*material;
	
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

		// run through the materials

	for (n=0;n!=max_model_texture;n++) {
	
			// any opaque trigs?
			
		if (draw_mesh->materials[n].has_transparent) continue;
	
			// get texture
			
		texture=&mdl->textures[n];
		material=&mesh->materials[n];
		
		frame=draw_mesh->materials[n].frame;

			// trig count

		trig_count=material->trig_count;
		if (trig_count==0) continue;

		trig_idx=material->trig_start*3;
		
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

		gl_shader_draw_execute(core_shader_group_model,texture,n,frame,-1,1.0f,light_list,draw->setup.vbo_offset.tangent,draw->setup.vbo_offset.normal);
		glDrawArrays(GL_TRIANGLES,trig_idx,(trig_count*3));
		
		view.count.poly+=trig_count;
	}
			
	gl_shader_draw_end();
}

void render_model_transparent_normal(model_type *mdl,int mesh_idx,model_draw *draw)
{
	int						n,frame,trig_count,trig_idx;
	bool					enabled,cur_additive,is_additive;
	model_mesh_type			*mesh;
 	model_draw_mesh_type	*draw_mesh;
    texture_type			*texture;
	model_material_type		*material;

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

	enabled=FALSE;
	cur_additive=FALSE;
	
		// run through textures

	for (n=0;n!=max_model_texture;n++) {
	
			// any transparent trigs?
			
		if (!draw_mesh->materials[n].has_transparent) continue;
	
			// get texture
			
		texture=&mdl->textures[n];
		material=&mesh->materials[n];
		
		frame=draw_mesh->materials[n].frame;
	
			// trig count

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

		gl_texture_transparent_set(texture->frames[frame].bitmap.gl_id,draw_mesh->materials[n].alpha);
		glDrawArrays(GL_TRIANGLES,trig_idx,(trig_count*3));
		
		view.count.poly+=trig_count;
	}
	
	if (cur_additive) glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	if (enabled) render_model_disable_color_array();

	gl_texture_transparent_end();
}

void render_model_transparent_shader(model_type *mdl,int mesh_idx,model_draw *draw,view_light_list_type *light_list)
{
	int						n,frame,trig_count,trig_idx;
	bool					cur_additive,is_additive;
	model_mesh_type			*mesh;
 	model_draw_mesh_type	*draw_mesh;
    texture_type			*texture;
	model_material_type		*material;

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
	
		// minimize state changes

	cur_additive=FALSE;

		// run through the materials

	for (n=0;n!=max_model_texture;n++) {
	
			// any transparent trigs?
			
		if (!draw_mesh->materials[n].has_transparent) continue;
	
			// get texture
			
		texture=&mdl->textures[n];
		material=&mesh->materials[n];
		
		frame=draw_mesh->materials[n].frame;
	
			// trig count

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
		
			// set hilite and diffuse on a per
			// mesh basis
		
		light_list->hilite=((mesh->no_lighting)||(draw->ui_lighting));
		if (!mesh->diffuse) {
			light_list->diffuse_boost=1.0f;
		}
		else {
			light_list->diffuse_boost=mdl->diffuse_boost;
		}

			// run the shader

		gl_shader_draw_execute(core_shader_group_model,texture,n,frame,-1,draw_mesh->materials[n].alpha,light_list,draw->setup.vbo_offset.tangent,draw->setup.vbo_offset.normal);
		glDrawArrays(GL_TRIANGLES,trig_idx,(trig_count*3));
		
		view.count.poly+=trig_count;
	}
	
	if (cur_additive) glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			
	gl_shader_draw_end();
}

void render_model_glow(model_type *mdl,int mesh_idx,model_draw *draw)
{
	int						n,frame,trig_count,trig_idx;
	model_mesh_type			*mesh;
 	model_draw_mesh_type	*draw_mesh;
    texture_type			*texture;
	model_material_type		*material;
	
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
	
		// run through the materials

	for (n=0;n!=max_model_texture;n++) {
	
			// any glow trigs?
			
		if (!draw_mesh->materials[n].has_glow) continue;
	
			// get texture
			
		texture=&mdl->textures[n];
		material=&mesh->materials[n];
		
		frame=draw_mesh->materials[n].frame;
		
			// trig count

		trig_count=material->trig_count;
		if (trig_count==0) continue;

		trig_idx=material->trig_start*3;

			// draw glow texture
		
		gl_texture_glow_set(texture->frames[frame].bitmap.gl_id,texture->frames[frame].glowmap.gl_id,texture->glow.current_color);
		glDrawArrays(GL_TRIANGLES,trig_idx,(trig_count*3));
		
		view.count.poly+=trig_count;
	}
	
	gl_texture_glow_end();
}

/* =======================================================

      Setup Model and Vertex Lists
      
======================================================= */

void render_model_setup(model_draw *draw,int tick)
{
	int					n,t,frame;
	float				alpha;
	model_type			*mdl;
	model_mesh_type		*mesh;
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

		// check for opaque/transparent draws

	draw->has_opaque=FALSE;
	draw->has_transparent=FALSE;
	draw->has_glow=FALSE;
	
	for (n=0;n!=mdl->nmesh;n++) {
		if ((draw->render_mesh_mask&(0x1<<n))==0) continue;
		
			// check for any transparent textures
			// or shaders
		
		mesh=&mdl->meshes[n];
		
		draw->meshes[n].has_opaque=FALSE;
		draw->meshes[n].has_transparent=FALSE;
		draw->meshes[n].has_glow=FALSE;
			
		texture=mdl->textures;
		
		for (t=0;t!=max_model_texture;t++) {
		
				// texture used in this mesh?
				
			if (mdl->meshes[n].materials[t].trig_count==0) {
				texture++;
				continue;
			}
			
				// check for fade
				
			alpha=draw->alpha;
			if (draw->meshes[n].fade.on) alpha=draw->meshes[n].fade.alpha;
				
				// check texture for transparencies
				
			if (texture->animate.on) {
				frame=texture->animate.current_frame;
			}
			else {
				frame=(int)draw->cur_texture_frame[t];
			}
			
			draw->meshes[n].materials[t].frame=frame;
			draw->meshes[n].materials[t].alpha=alpha;
			
			if (texture->frames[frame].glowmap.gl_id!=-1) {
				draw->meshes[n].materials[t].has_glow=TRUE;
				draw->meshes[n].has_glow=TRUE;
				draw->has_glow=TRUE;
			}
			else {
				draw->meshes[n].materials[t].has_glow=FALSE;
			}
	
			if ((texture->frames[frame].bitmap.alpha_mode==alpha_mode_transparent) || (alpha!=1.0)) {
				draw->meshes[n].materials[t].has_transparent=TRUE;
				draw->meshes[n].has_transparent=TRUE;
				draw->has_transparent=TRUE;
			}
			else {
				draw->meshes[n].materials[t].has_transparent=FALSE;
				draw->meshes[n].has_opaque=TRUE;
				draw->has_opaque=TRUE;
			}
			
			texture++;
		}
	}
}

void render_model_build_vertex_lists(model_draw *draw)
{
	int					n;
	bool				shader_off;
	model_type			*mdl;

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
	if (shader_off) render_model_create_color_vertexes(mdl,draw->render_mesh_mask,draw);
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
		
	shader_on=view_shader_on();

	if ((shader_on) && (!draw->no_shader)) gl_lights_build_model_light_list(mdl,draw,&light_list);

		// draw opaque materials

	for (n=0;n!=mdl->nmesh;n++) {
		if ((draw->render_mesh_mask&(0x1<<n))==0) continue;
		if (!draw->meshes[n].has_opaque) continue;

			// create VBO for this mesh

		if (!render_model_initialize_vertex_objects(mdl,n,draw)) return;

			// render opaque segments

		if ((!shader_on) || (draw->no_shader)) {
			render_model_opaque_normal(mdl,n,draw);
		}
		else {
			render_model_opaque_shader(mdl,n,draw,&light_list);
		}
		
			// render glow segments

		if (draw->meshes[n].has_glow) render_model_glow(mdl,n,draw);
		
		render_model_release_vertex_objects();
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
		
	shader_on=view_shader_on();

	if ((shader_on) && (!draw->no_shader)) gl_lights_build_model_light_list(mdl,draw,&light_list);

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
	float			rang;
	float			*vertex_ptr;
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
