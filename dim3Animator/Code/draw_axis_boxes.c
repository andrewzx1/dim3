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
	float			vertexes[2*3];

		// x axis

	vertexes[0]=-20000.0f;
	vertexes[1]=(float)model.center.y;
	vertexes[2]=(float)model.center.z;
	vertexes[3]=20000.0f;
	vertexes[4]=(float)model.center.y;
	vertexes[5]=(float)model.center.z;
	
	glVertexPointer(3,GL_FLOAT,0,vertexes);

	glColor4f(1.0f,0.0f,0.0f,1.0f);
	glDrawArrays(GL_LINES,0,4);
	
		// y axis
		
    vertexes[0]=(float)model.center.x;
	vertexes[1]=-20000.0f;
	vertexes[2]=(float)model.center.z;
	vertexes[3]=(float)model.center.x;
	vertexes[4]=20000.0f;
	vertexes[5]=(float)model.center.z;

	glVertexPointer(3,GL_FLOAT,0,vertexes);

	glColor4f(0.0f,1.0f,0.0f,1.0f);
	glDrawArrays(GL_LINES,0,4);

		// z axis
		
    vertexes[0]=(float)model.center.x;
	vertexes[1]=(float)model.center.y;
	vertexes[2]=-20000.0f;
	vertexes[3]=(float)model.center.x;
	vertexes[4]=(float)model.center.y;
	vertexes[5]=(float)20000.0f;

	glVertexPointer(3,GL_FLOAT,0,vertexes);

	glColor4f(0.0f,0.0f,1.0f,1.0f);
	glDrawArrays(GL_LINES,0,4);
}

/* =======================================================

      Draw View Box
      
======================================================= */

void draw_model_box_view(void)
{
	int				n,xsz,zsz,ysz,offx,offz,offy,
					x[8],y[8],z[8];
	float			vertexes[8*3];
	float			*pv;

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

	pv=vertexes;
    
	for (n=0;n!=4;n++) {
		*pv++=(float)x[n];
		*pv++=(float)y[n];
		*pv++=(float)z[n];
	}

	glVertexPointer(3,GL_FLOAT,0,vertexes);
	glDrawArrays(GL_LINE_LOOP,0,4);
    
	pv=vertexes;
    
	for (n=4;n!=8;n++) {
		*pv++=(float)x[n];
		*pv++=(float)y[n];
		*pv++=(float)z[n];
	}

	glVertexPointer(3,GL_FLOAT,0,vertexes);
	glDrawArrays(GL_LINE_LOOP,0,4);
    
	pv=vertexes;

	for (n=0;n!=4;n++) {
		*pv++=(float)x[n];
		*pv++=(float)y[n];
		*pv++=(float)z[n];
		*pv++=(float)x[n+4];
		*pv++=(float)y[n+4];
		*pv++=(float)z[n+4];
	}

	glVertexPointer(3,GL_FLOAT,0,vertexes);
	glDrawArrays(GL_LINES,0,8);
		
    glLineWidth(1.0f);

		// draw floor

	glColor4f(0.5f,0.5f,0.5f,0.5f);
		
	pv=vertexes;

	for (n=4;n!=8;n++) {
		*pv++=(float)x[n];
		*pv++=(float)y[n];
		*pv++=(float)z[n];
	}

	glVertexPointer(3,GL_FLOAT,0,vertexes);
	glDrawArrays(GL_QUADS,0,4);
}

/* =======================================================

      Draw Model Box
      
======================================================= */

void draw_model_box_hit_boxes(void)
{
	int				n,k,x[8],y[8],z[8];
	float			vertexes[8*3];
	float			*pv;
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
	    
		pv=vertexes;

		for (k=0;k!=4;k++) {
			*pv++=(float)x[k];
			*pv++=(float)y[k];
			*pv++=(float)z[k];
		}
		
		glVertexPointer(3,GL_FLOAT,0,vertexes);
		glDrawArrays(GL_LINE_LOOP,0,4);
	    
		pv=vertexes;

		for (k=4;k!=8;k++) {
			*pv++=(float)x[k];
			*pv++=(float)y[k];
			*pv++=(float)z[k];
		}

		glVertexPointer(3,GL_FLOAT,0,vertexes);
		glDrawArrays(GL_LINE_LOOP,0,4);
	    
		pv=vertexes;

		for (k=0;k!=4;k++) {
			*pv++=(float)x[k];
			*pv++=(float)y[k];
			*pv++=(float)z[k];
			*pv++=(float)x[k+4];
			*pv++=(float)y[k+4];
			*pv++=(float)z[k+4];
		}
		
		glVertexPointer(3,GL_FLOAT,0,vertexes);
		glDrawArrays(GL_LINES,0,8);
		
		glLineWidth(1.0f);
	
			// draw handles

		glDisable(GL_DEPTH_TEST);
		
		glColor4f(0.0f,0.0f,0.0f,1.0f);
		glPointSize(10.0f);
		
		pv=vertexes;

		for (k=0;k!=8;k++) {
			*pv++=(float)x[k];
			*pv++=(float)y[k];
			*pv++=(float)z[k];
		}
		
		glVertexPointer(3,GL_FLOAT,0,vertexes);
		glDrawArrays(GL_POINTS,0,8);

		glPointSize(1.0f);
		
		glEnable(GL_DEPTH_TEST);
	}
}

