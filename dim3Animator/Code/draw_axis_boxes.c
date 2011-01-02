/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Draw Model Axis and Boxes

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

#include "glue.h"

extern model_type			model;
extern model_draw_setup		draw_setup;
extern animator_state_type	state;

/* =======================================================

      Draw Model Axis
      
======================================================= */

void draw_model_axis(void)
{
		// x axis
		
	glColor4f(1,0,0,1);
	
    glBegin(GL_LINES);
	glVertex3i(-20000,model.center.y,model.center.z);
	glVertex3i(20000,model.center.y,model.center.z);
	glEnd();
	
		// y axis
		
	glColor4f(0,1,0,1);
	
    glBegin(GL_LINES);
	glVertex3i(model.center.x,-20000,model.center.z);
	glVertex3i(model.center.x,20000,model.center.z);
    glEnd();

		// z axis
		
	glColor4f(0,0,1,1);
	
    glBegin(GL_LINES);
	glVertex3i(model.center.x,model.center.y,-20000);
	glVertex3i(model.center.x,model.center.y,20000);
    glEnd();
}

/* =======================================================

      Draw Model Box
      
======================================================= */

void draw_model_box(model_box_type *box,bool draw_floor,bool draw_handles)
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
		model_get_point_position(&draw_setup,&x[n],&y[n],&z[n]);
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

void draw_model_box_view(void)
{
	glColor4f(0,1,0,0.5);
	draw_model_box(&model.view_box,TRUE,FALSE);
}

void draw_model_box_hit_boxes(void)
{
	int				n;
	
	for (n=0;n<model.nhit_box;n++) {
		glColor4f(1,1,0,0.5);
		draw_model_box(&model.hit_boxes[n].box,FALSE,TRUE);
	}
}

