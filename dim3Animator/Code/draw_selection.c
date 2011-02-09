/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Draw Model Selection and Normals

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
extern animator_setup_type		setup;
extern animator_state_type		state;

/* =======================================================

      Draw Selected Vertexes
      
======================================================= */

void draw_model_selected_vertexes(int mesh_idx)
{
	int				n,nvertex;
	float			*pv;
	model_mesh_type	*mesh;
	
	glPointSize(draw_vertex_handle_size);
	
	mesh=&model.meshes[mesh_idx];
	nvertex=mesh->nvertex;
	
	pv=draw_setup.mesh_arrays[mesh_idx].gl_vertex_array;

	glBegin(GL_POINTS);
	
	for (n=0;n!=nvertex;n++) {
	
		if ((!vertex_check_sel_mask(mesh_idx,n)) || (vertex_check_hide_mask(mesh_idx,n))) {
			pv+=3;
			continue;
		}
		
		if (mesh->vertexes[n].major_bone_idx!=-1) {
			if ((mesh->vertexes[n].major_bone_idx==state.cur_bone_idx) && (state.cur_bone_idx!=-1)) {
				glColor4f(1.0f,0.2f,1.0f,1.0f);
			}
			else {
				glColor4f(0.0f,0.0f,0.0f,1.0f);
			}
		}
		else {
			glColor4f(0.5f,0.5f,0.5f,1.0f);
		}
		
		glVertex3f(*pv,*(pv+1),*(pv+2));
		
		pv+=3;
	}
	
	glEnd();
	
	glPointSize(1.0f);
}

/* =======================================================

      Draw Selected Trig
      
======================================================= */

void draw_model_selected_trig(int mesh_idx)
{
	int					n,k,ntrig,vertex_idx;
	float				*pv;
	model_mesh_type		*mesh;
	model_trig_type		*trig;

	mesh=&model.meshes[mesh_idx];
	ntrig=mesh->ntrig;

		// selection
	
	glColor4f(setup.col.mesh_sel.r,setup.col.mesh_sel.g,setup.col.mesh_sel.b,1.0f);
	glLineWidth(draw_trig_select_line_size);

	for (n=0;n!=ntrig;n++) {

		if ((!trig_check_sel_mask(mesh_idx,n)) || (trig_check_hide_mask(mesh_idx,n))) continue;
		
		trig=&model.meshes[mesh_idx].trigs[n];

			// draw the selected trig
			
		glBegin(GL_LINE_LOOP);

		for (k=0;k!=3;k++) {
			pv=draw_setup.mesh_arrays[mesh_idx].gl_vertex_array+(3*trig->v[k]);
			glVertex3f(*pv,*(pv+1),*(pv+2));
		}
	
		glEnd();
	}
		
	glLineWidth(1.0f);

		// points
	
	glColor4f(0.0f,0.0f,0.0f,1.0f);
	glPointSize(draw_vertex_handle_size);
		
	glBegin(GL_POINTS);

	for (n=0;n!=ntrig;n++) {

		if ((!trig_check_sel_mask(mesh_idx,n)) || (trig_check_hide_mask(mesh_idx,n))) continue;
		
		trig=&model.meshes[mesh_idx].trigs[n];
		
			// draw any selected vertexes
			// on the trig
		
		for (k=0;k!=3;k++) {
			vertex_idx=trig->v[k];
			if ((vertex_check_sel_mask(mesh_idx,vertex_idx)) && (!vertex_check_hide_mask(mesh_idx,vertex_idx))) {
				pv=draw_setup.mesh_arrays[mesh_idx].gl_vertex_array+(3*vertex_idx);
				glVertex3f(*pv,*(pv+1),*(pv+2));
			}
		}
	}
		
	glEnd();
		
	glPointSize(1.0f);
}

/* =======================================================

      Draw Model Normals
      
======================================================= */

void draw_model_normals_vertexes(int mesh_idx)
{
	int				n,k,ntrig;
	float			fx,fy,fz,fx2,fy2,fz2;
	float			*pv,*pn;
	bool			has_sel;
	model_trig_type	*trig;
	
		// is there a vertex selection?
		
	has_sel=vertex_check_any(mesh_idx);
	
		// draw normals
	
	glLineWidth(draw_model_normal_size);
	glColor4f(1.0f,0.0f,1.0f,1.0f);
	
	glBegin(GL_LINES);
	
	ntrig=model.meshes[mesh_idx].ntrig;

	for (n=0;n!=ntrig;n++) {
	
		trig=&model.meshes[mesh_idx].trigs[n];
	
			// only show normals for triangles
			// with all vertexes showing
			
		if (vertex_check_hide_mask_trig(mesh_idx,trig)) continue;
		
		pn=draw_setup.mesh_arrays[mesh_idx].gl_normal_array+(n*9);
		
			// vertex normals
			
		for (k=0;k!=3;k++) {
				
			if (has_sel) {
				if (!vertex_check_sel_mask(mesh_idx,trig->v[k])) {
					pn+=3;
					continue;
				}
			}
			
			pv=draw_setup.mesh_arrays[mesh_idx].gl_vertex_array+(trig->v[k]*3);
			fx=*pv++;
			fy=*pv++;
			fz=*pv;
				
			glVertex3f(fx,fy,fz);
			
			fx2=fx+((*pn++)*draw_model_normal_len);
			fy2=fy+((*pn++)*draw_model_normal_len);
			fz2=fz+((*pn++)*draw_model_normal_len);

			glVertex3f(fx2,fy2,fz2);
		}
	}
	
	glEnd();
	
	glLineWidth(1.0f);
}

void draw_model_normals_trig(int mesh_idx)
{
	int				n,k,ntrig;
	float			fx,fy,fz,fx2,fy2,fz2;
	float			*pv,*pn;
	model_trig_type	*trig;
	
	glLineWidth(draw_model_normal_size);
	glColor4f(1.0f,0.0f,1.0f,1.0f);
	
	glBegin(GL_LINES);

	ntrig=model.meshes[mesh_idx].ntrig;

	for (n=0;n!=ntrig;n++) {
	
		trig=&model.meshes[mesh_idx].trigs[n];
	
			// draw trig normals
		
		pn=draw_setup.mesh_arrays[mesh_idx].gl_normal_array+(n*9);
			
		for (k=0;k!=3;k++) {
			
			pv=draw_setup.mesh_arrays[mesh_idx].gl_vertex_array+(trig->v[k]*3);
			fx=*pv++;
			fy=*pv++;
			fz=*pv;
				
			glVertex3f(fx,fy,fz);
			
			fx2=fx+((*pn++)*draw_model_normal_len);
			fy2=fy+((*pn++)*draw_model_normal_len);
			fz2=fz+((*pn++)*draw_model_normal_len);

			glVertex3f(fx2,fy2,fz2);
		}
	}

	glEnd();
	glLineWidth(1.0f);
}

