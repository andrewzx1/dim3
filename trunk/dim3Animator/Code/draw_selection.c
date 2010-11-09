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

#include "model.h"

extern animator_state_type	state;

/* =======================================================

      Draw Selected Vertexes
      
======================================================= */

void draw_model_selected_vertexes(model_type *model,int mesh_idx,model_draw_setup *draw_setup)
{
	int				n,nvertex;
	float			*pv;
	
	glColor4f(0.0f,0.0f,0.0f,1.0f);
	glPointSize(draw_vertex_handle_size);
	
	nvertex=model->meshes[mesh_idx].nvertex;
	pv=draw_setup->mesh_arrays[mesh_idx].gl_vertex_array;

	glBegin(GL_POINTS);
	
	for (n=0;n!=nvertex;n++) {
		if ((vertex_check_sel_mask(mesh_idx,n)) && (!vertex_check_hide_mask(mesh_idx,n))) glVertex3f(*pv,*(pv+1),*(pv+2));
		pv+=3;
	}
	
	glEnd();
	
	glPointSize(1.0f);
}

/* =======================================================

      Draw Selected Trig
      
======================================================= */

void draw_model_selected_trig(model_type *model,int mesh_idx,model_draw_setup *draw_setup)
{
	int					n,vertex_idx;
	float				*pv;
	model_trig_type		*trig;

	if (state.sel_trig_idx==-1) return;
	
		// draw the selected trig
		
	glColor4f(1.0f,1.0f,0.0f,1.0f);
	glLineWidth(draw_trig_select_line_size);
	
	trig=&model->meshes[mesh_idx].trigs[state.sel_trig_idx];

	glBegin(GL_LINE_LOOP);
	
	for (n=0;n!=3;n++) {
		pv=draw_setup->mesh_arrays[mesh_idx].gl_vertex_array+(3*trig->v[n]);
		glVertex3f(*pv,*(pv+1),*(pv+2));
	}
	
	glEnd();
	
	glLineWidth(1.0f);
	
		// draw any selected vertexes
		// on the trig
	
	glColor4f(0.0f,0.0f,0.0f,1.0f);
	glPointSize(draw_vertex_handle_size);
	
	glBegin(GL_POINTS);
	
	for (n=0;n!=3;n++) {
		vertex_idx=trig->v[n];
		if ((vertex_check_sel_mask(mesh_idx,vertex_idx)) && (!vertex_check_hide_mask(mesh_idx,vertex_idx))) {
			pv=draw_setup->mesh_arrays[mesh_idx].gl_vertex_array+(3*vertex_idx);
			glVertex3f(*pv,*(pv+1),*(pv+2));
		}
	}
	
	glEnd();
	
	glPointSize(1.0f);
}

/* =======================================================

      Draw Model Normals
      
======================================================= */

void draw_model_normals_vertexes(model_type *model,int mesh_idx,model_draw_setup *draw_setup)
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
	
	ntrig=model->meshes[mesh_idx].ntrig;

	for (n=0;n!=ntrig;n++) {
	
		trig=&model->meshes[mesh_idx].trigs[n];
	
			// only show normals for triangles
			// with all vertexes showing
			
		if (vertex_check_hide_mask_trig(mesh_idx,trig)) continue;
		
		pn=draw_setup->mesh_arrays[mesh_idx].gl_normal_array+(n*9);
		
			// vertex normals
			
		for (k=0;k!=3;k++) {
				
			if (has_sel) {
				if (!vertex_check_sel_mask(mesh_idx,trig->v[k])) {
					pn+=3;
					continue;
				}
			}
			
			pv=draw_setup->mesh_arrays[mesh_idx].gl_vertex_array+(trig->v[k]*3);
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

void draw_model_normals_trig(model_type *model,int mesh_idx,model_draw_setup *draw_setup)
{
	int				n;
	float			fx,fy,fz,fx2,fy2,fz2;
	float			*pv,*pn;
	model_trig_type	*trig;
	
		// is there a trig selection?
		
	if (state.sel_trig_idx==-1) return;
	
		// get trig
		
	trig=&model->meshes[mesh_idx].trigs[state.sel_trig_idx];
	
		// draw trig normals
	
	glLineWidth(draw_model_normal_size);
	
	glColor4f(1.0f,0.0f,1.0f,1.0f);
	
	glBegin(GL_LINES);
	
	pn=draw_setup->mesh_arrays[mesh_idx].gl_normal_array+(state.sel_trig_idx*9);
		
	for (n=0;n!=3;n++) {
		
		pv=draw_setup->mesh_arrays[mesh_idx].gl_vertex_array+(trig->v[n]*3);
		fx=*pv++;
		fy=*pv++;
		fz=*pv;
			
		glVertex3f(fx,fy,fz);
		
		fx2=fx+((*pn++)*draw_model_normal_len);
		fy2=fy+((*pn++)*draw_model_normal_len);
		fz2=fz+((*pn++)*draw_model_normal_len);

		glVertex3f(fx2,fy2,fz2);
	}
	
	glEnd();
	
	glLineWidth(1.0f);
}

