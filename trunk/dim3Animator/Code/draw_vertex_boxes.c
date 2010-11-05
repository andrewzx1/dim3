/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Draw Mesh, Bones, Vertexes, Boxes

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

#define draw_model_normal_size			50.0f

extern animator_state_type	state;

/* =======================================================

      Draw Selected Vertexes
      
======================================================= */

void draw_model_selected_vertexes(model_type *model,int mesh_idx,model_draw_setup *draw_setup)
{
	int				n,nvertex;
	float			*pv;
	
	glColor4f(0.0f,0.0f,0.0f,1.0f);
	glPointSize(5.0f);
	
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
	glLineWidth(5.0f);
	
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
	glPointSize(5.0f);
	
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

      Draw Model Box
      
======================================================= */

void draw_model_box(model_box_type *box,model_draw_setup *draw_setup,bool draw_floor,bool draw_handles)
{
	int				n,xsz,zsz,ysz,offx,offz,offy,
					x[8],y[8],z[8];
	
    xsz=box->size.x/2;
    offx=box->offset.x;
    zsz=box->size.z/2;
    offz=box->offset.z;
    ysz=box->size.y;
    offy=box->offset.y;
	
	x[0]=x[1]=x[4]=x[5]=offx-xsz;
	x[2]=x[3]=x[6]=x[7]=offx+xsz;
	y[0]=y[1]=y[2]=y[3]=offy-ysz;
	y[4]=y[5]=y[6]=y[7]=offy;
	z[0]=z[3]=z[4]=z[7]=offz-zsz;
	z[1]=z[2]=z[5]=z[6]=offz+zsz;
	
	for (n=0;n!=8;n++) {
		model_get_point_position(draw_setup,&x[n],&y[n],&z[n]);
	}
    
    glLineWidth(4);
    
    glBegin(GL_LINE_LOOP);
	for (n=0;n!=4;n++) {
		glVertex3i(x[n],y[n],z[n]);
	}
    glEnd();
    
    glBegin(GL_LINE_LOOP);
	for (n=4;n!=8;n++) {
		glVertex3i(x[n],y[n],z[n]);
	}
    glEnd();
    
    glBegin(GL_LINES);
	for (n=0;n!=4;n++) {
		glVertex3i(x[n],y[n],z[n]);
		glVertex3i(x[n+4],y[n+4],z[n+4]);
	}
    glEnd();
		
    glLineWidth(1);
	
	if (draw_handles) {
		glDisable(GL_DEPTH_TEST);
		
		glColor4f(0,0,0,1);
		glPointSize(10);
		
		glBegin(GL_POINTS);
		
		for (n=0;n!=8;n++) {
			glVertex3f(x[n],y[n],z[n]);
		}
		
		glEnd();
		glPointSize(1);
		
		glEnable(GL_DEPTH_TEST);
	}
    
    if (draw_floor) {
		glColor4f(0.75,0.75,0.75,0.5);
		
		glBegin(GL_POLYGON);
		for (n=4;n!=8;n++) {
			glVertex3i(x[n],y[n],z[n]);
		}
		glEnd();
	}
}

void draw_model_box_view(model_type *model,model_draw_setup *draw_setup)
{
	glColor4f(0,1,0,0.5);
	draw_model_box(&model->view_box,draw_setup,TRUE,FALSE);
}

void draw_model_box_hit_boxes(model_type *model,model_draw_setup *draw_setup)
{
	int				n;
	
	for (n=0;n<model->nhit_box;n++) {
		glColor4f(1,1,0,0.5);
		draw_model_box(&model->hit_boxes[n].box,draw_setup,FALSE,TRUE);
	}
}

/* =======================================================

      Draw Model Axis
      
======================================================= */

void draw_model_axis(model_type *model)
{
		// x axis
		
	glColor4f(1,0,0,1);
	
    glBegin(GL_LINES);
	glVertex3i(-20000,model->center.y,model->center.z);
	glVertex3i(20000,model->center.y,model->center.z);
	glEnd();
	
		// y axis
		
	glColor4f(0,1,0,1);
	
    glBegin(GL_LINES);
	glVertex3i(model->center.x,-20000,model->center.z);
	glVertex3i(model->center.x,20000,model->center.z);
    glEnd();

		// z axis
		
	glColor4f(0,0,1,1);
	
    glBegin(GL_LINES);
	glVertex3i(model->center.x,model->center.y,-20000);
	glVertex3i(model->center.x,model->center.y,20000);
    glEnd();
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
	
	glLineWidth(2.0f);
	
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
			
			fx2=fx+((*pn++)*draw_model_normal_size);
			fy2=fy+((*pn++)*draw_model_normal_size);
			fz2=fz+((*pn++)*draw_model_normal_size);

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
	
	glLineWidth(2.0f);
	
	glColor4f(1.0f,0.0f,1.0f,1.0f);
	
	glBegin(GL_LINES);
	
	pn=draw_setup->mesh_arrays[mesh_idx].gl_normal_array+(state.sel_trig_idx*9);
		
	for (n=0;n!=3;n++) {
		
		pv=draw_setup->mesh_arrays[mesh_idx].gl_vertex_array+(trig->v[n]*3);
		fx=*pv++;
		fy=*pv++;
		fz=*pv;
			
		glVertex3f(fx,fy,fz);
		
		fx2=fx+((*pn++)*draw_model_normal_size);
		fy2=fy+((*pn++)*draw_model_normal_size);
		fz2=fz+((*pn++)*draw_model_normal_size);

		glVertex3f(fx2,fy2,fz2);
	}
	
	glEnd();
	
	glLineWidth(1.0f);
}

