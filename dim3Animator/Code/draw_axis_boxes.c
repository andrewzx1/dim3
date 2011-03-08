/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Draw Model Axis and Boxes

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
	#include "dim3animator.h"
#endif

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
		
	glColor4f(1.0f,0.0f,0.0f,1.0f);
	
    glBegin(GL_LINES);
	glVertex3i(-20000,model.center.y,model.center.z);
	glVertex3i(20000,model.center.y,model.center.z);
	glEnd();
	
		// y axis
		
	glColor4f(0.0f,1.0f,0.0f,1.0f);
	
    glBegin(GL_LINES);
	glVertex3i(model.center.x,-20000,model.center.z);
	glVertex3i(model.center.x,20000,model.center.z);
    glEnd();

		// z axis
		
	glColor4f(0.0f,0.0f,1.0f,1.0f);
	
    glBegin(GL_LINES);
	glVertex3i(model.center.x,model.center.y,-20000);
	glVertex3i(model.center.x,model.center.y,20000);
    glEnd();
}

/* =======================================================

      Draw View Box
      
======================================================= */

void draw_model_box_view(void)
{
	int				n,xsz,zsz,ysz,offx,offz,offy,
					x[8],y[8],z[8];
	
		// get box

    xsz=model.view_box.size.x/2;
    offx=model.view_box.offset.x;
    zsz=model.view_box.size.z/2;
    offz=model.view_box.offset.z;
    ysz=model.view_box.size.y;
    offy=model.view_box.offset.y;
	
	x[0]=x[1]=x[4]=x[5]=offx-xsz;
	x[2]=x[3]=x[6]=x[7]=offx+xsz;
	y[0]=y[1]=y[2]=y[3]=offy-ysz;
	y[4]=y[5]=y[6]=y[7]=offy;
	z[0]=z[3]=z[4]=z[7]=offz-zsz;
	z[1]=z[2]=z[5]=z[6]=offz+zsz;
	
	for (n=0;n!=8;n++) {
		model_get_point_position(&draw_setup,&x[n],&y[n],&z[n]);
	}
    
		// draw box

 	glColor4f(0.0f,1.0f,0.0f,0.5f);
	glLineWidth(4.0f);
    
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
		
    glLineWidth(1.0f);

		// draw floor

	glColor4f(0.5f,0.5f,0.5f,0.5f);
		
	glBegin(GL_POLYGON);
	for (n=4;n!=8;n++) {
		glVertex3i(x[n],y[n],z[n]);
	}
	glEnd();
}



/* =======================================================

      Draw Model Box
      
======================================================= */

void draw_model_box_hit_boxes(void)
{
	int				n,k,x[8],y[8],z[8];
	d3pnt			pnt,min,max;

	pnt.x=pnt.y=pnt.z=0;
	
	for (n=0;n<model.nhit_box;n++) {

			// get box

		model_hit_box_get_box(&model,n,&pnt,&draw_setup,&min,&max);

		x[0]=x[1]=x[4]=x[5]=min.x;
		x[2]=x[3]=x[6]=x[7]=max.x;
		y[0]=y[1]=y[2]=y[3]=min.y;
		y[4]=y[5]=y[6]=y[7]=max.y;
		z[0]=z[3]=z[4]=z[7]=min.z;
		z[1]=z[2]=z[5]=z[6]=max.z;
		
		for (k=0;k!=8;k++) {
			model_get_point_position(&draw_setup,&x[k],&y[k],&z[k]);
		}
	    
			// draw box

		glColor4f(1.0f,1.0f,0.0f,0.5f);
		glLineWidth(4.0f);
	    
		glBegin(GL_LINE_LOOP);
		for (k=0;k!=4;k++) {
			glVertex3i(x[k],y[k],z[k]);
		}
		glEnd();
	    
		glBegin(GL_LINE_LOOP);
		for (k=4;k!=8;k++) {
			glVertex3i(x[k],y[k],z[k]);
		}
		glEnd();
	    
		glBegin(GL_LINES);
		for (k=0;k!=4;k++) {
			glVertex3i(x[k],y[k],z[k]);
			glVertex3i(x[k+4],y[k+4],z[k+4]);
		}
		glEnd();
		
		glLineWidth(1.0f);
	
			// draw handles

		glDisable(GL_DEPTH_TEST);
		
		glColor4f(0.0f,0.0f,0.0f,1.0f);
		glPointSize(10.0f);
		
		glBegin(GL_POINTS);
		
		for (k=0;k!=8;k++) {
			glVertex3i(x[k],y[k],z[k]);
		}
		
		glEnd();
		glPointSize(1.0f);
		
		glEnable(GL_DEPTH_TEST);
	}
}

