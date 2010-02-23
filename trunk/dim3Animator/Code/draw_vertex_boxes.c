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

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#include "model.h"

/* =======================================================

      Draw Vertexes
      
======================================================= */

void draw_model_selected_vertexes(model_type *model,int mesh_idx,model_draw_setup *draw_setup)
{
	int				n,nvertex;
	float			*pv;
	
	glColor4f(0,0,0,1);
	glPointSize(5);
	
	nvertex=model->meshes[mesh_idx].nvertex;
	pv=draw_setup->mesh_arrays[mesh_idx].gl_vertex_array;

	glBegin(GL_POINTS);
	
	for (n=0;n!=nvertex;n++) {
		if ((vertex_check_sel_mask(mesh_idx,n)) && (!vertex_check_hide_mask(mesh_idx,n))) glVertex3f(*pv,*(pv+1),*(pv+2));
		pv+=3;
	}
	
	glEnd();
	glPointSize(1);
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

      Draw Model Normals
      
======================================================= */

void draw_model_normals(model_type *model,int mesh_idx,model_draw_setup *draw_setup)
{
	int				n,k,ntrig,
					sx,sy,sz,lsz;
	float			fx,fy,fz,flsz;
	float			*pv,*pt,*pb,*pn;
	model_trig_type	*trig;
	
		// find normal line size
		
	model_get_size(model,&sx,&sy,&sz);
	lsz=sx;
	if (sz>lsz) lsz=sz;
	if (sy>lsz) lsz=sy;
	
	flsz=((float)lsz)/15.0f;
	
		// draw normals
	
	glLineWidth(2.0f);
	
	pt=draw_setup->mesh_arrays[mesh_idx].gl_tangent_array;
	pb=draw_setup->mesh_arrays[mesh_idx].gl_binormal_array;
	pn=draw_setup->mesh_arrays[mesh_idx].gl_normal_array;

	glColor4f(1,0,1,1);
	
	glBegin(GL_LINES);
	
	ntrig=model->meshes[mesh_idx].ntrig;
	trig=model->meshes[mesh_idx].trigs;

	for (n=0;n!=ntrig;n++) {
	
		for (k=0;k!=3;k++) {
		
				// vertex hidden?
				
			if (vertex_check_hide_mask(mesh_idx,trig->v[k])) {
				pt+=3;
				pb+=3;
				pn+=3;
				continue;
			}
			
				// vertex
				
			pv=draw_setup->mesh_arrays[mesh_idx].gl_vertex_array+(trig->v[k]*3);
			fx=*pv++;
			fy=*pv++;
			fz=*pv;
			
/*		
				// tangent

			glColor4f(1.0f,0.0f,0.0f,1.0f);
			
			glVertex3f(fx,fy,fz);
			
			fx+=((*pt++)*flsz);
			fy+=((*pt++)*flsz);
			fz+=((*pt++)*flsz);
			glVertex3f(fx,fy,fz);
			
				// binormal
				
			glColor4f(0.0f,0.0f,1.0f,1.0f);
			
			glVertex3f(fx,fy,fz);
			
			fx+=((*pb++)*flsz);
			fy+=((*pb++)*flsz);
			fz+=((*pb++)*flsz);
			glVertex3f(fx,fy,fz);
	*/
				// normal
				
			glColor4f(1.0f,0.0f,1.0f,1.0f);
			
			glVertex3f(fx,fy,fz);
			
			fx+=((*pn++)*flsz);
			fy+=((*pn++)*flsz);
			fz+=((*pn++)*flsz);
			glVertex3f(fx,fy,fz);
		}
		
		trig++;
	}
	
	glEnd();
	
	glLineWidth(1.0f);
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

