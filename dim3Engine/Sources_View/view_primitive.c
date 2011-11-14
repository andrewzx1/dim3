/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View Primitives

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

extern map_type				map;
extern view_type			view;
extern setup_type			setup;

/* =======================================================

      Screen Tint
      
======================================================= */

void view_primitive_2D_tint_screen(void)
{
	float			vertexes[8];
	
	vertexes[0]=0.0f;
	vertexes[1]=0.0f;
	vertexes[2]=0.0f;
	vertexes[3]=(float)view.screen.y_sz;
	vertexes[4]=(float)view.screen.x_sz;
	vertexes[5]=0.0f;
	vertexes[6]=(float)view.screen.x_sz;
	vertexes[7]=(float)view.screen.y_sz;

	glVertexPointer(2,GL_FLOAT,0,(GLvoid*)vertexes);

	glDrawArrays(GL_TRIANGLE_STRIP,0,4);
}

/* =======================================================

      2D Fills
      
======================================================= */

void view_primitive_2D_color_poly(int x0,int y0,d3col *col0,int x1,int y1,d3col *col1,int x2,int y2,d3col *col2,int x3,int y3,d3col *col3,float alpha)
{
	float			vertexes[8];
	unsigned char	uc_alpha,colors[16];
								
	vertexes[0]=(float)x0;
	vertexes[1]=(float)y0;
	vertexes[2]=(float)x3;
	vertexes[3]=(float)y3;
	vertexes[4]=(float)x1;
	vertexes[5]=(float)y1;
	vertexes[6]=(float)x2;
	vertexes[7]=(float)y2;
	
	uc_alpha=(unsigned char)(alpha*255.0f);
	
	colors[0]=(unsigned char)(col0->r*255.0f);
	colors[1]=(unsigned char)(col0->g*255.0f);
	colors[2]=(unsigned char)(col0->b*255.0f);
	colors[3]=uc_alpha;
	colors[4]=(unsigned char)(col3->r*255.0f);
	colors[5]=(unsigned char)(col3->g*255.0f);
	colors[6]=(unsigned char)(col3->b*255.0f);
	colors[7]=uc_alpha;
	colors[8]=(unsigned char)(col1->r*255.0f);
	colors[9]=(unsigned char)(col1->g*255.0f);
	colors[10]=(unsigned char)(col1->b*255.0f);
	colors[11]=uc_alpha;
	colors[12]=(unsigned char)(col2->r*255.0f);
	colors[13]=(unsigned char)(col2->g*255.0f);
	colors[14]=(unsigned char)(col2->b*255.0f);
	colors[15]=uc_alpha;
		
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glDisable(GL_DEPTH_TEST);

		// draw the polygon

	glEnableClientState(GL_COLOR_ARRAY);
	
	glVertexPointer(2,GL_FLOAT,0,(GLvoid*)vertexes);
	glColorPointer(4,GL_UNSIGNED_BYTE,0,(GLvoid*)colors);

	glDrawArrays(GL_TRIANGLE_STRIP,0,4);

 	glDisableClientState(GL_COLOR_ARRAY);
		
		// finish draw

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
}

void view_primitive_2D_color_quad(d3col *col,float alpha,int lft,int rgt,int top,int bot)
{
	view_primitive_2D_color_poly(lft,top,col,rgt,top,col,rgt,bot,col,lft,bot,col,alpha);
}

void view_primitive_2D_color_trig(d3col *col,float alpha,int lft,int rgt,int top,int bot,int dir)
{
	float			vertexes[6];

	switch (dir) {

		case 0:
			vertexes[0]=(float)lft;
			vertexes[1]=(float)bot;
			vertexes[2]=(float)rgt;
			vertexes[3]=(float)bot;
			vertexes[4]=((float)(lft+rgt))*0.5f;
			vertexes[5]=(float)top;
			break;

		case 1:
			vertexes[0]=(float)lft;
			vertexes[1]=(float)top;
			vertexes[2]=(float)rgt;
			vertexes[3]=((float)(top+bot))*0.5f;
			vertexes[4]=(float)lft;
			vertexes[5]=(float)bot;
			break;

		case 2:
			vertexes[0]=(float)lft;
			vertexes[1]=(float)top;
			vertexes[2]=(float)rgt;
			vertexes[3]=(float)top;
			vertexes[4]=((float)(lft+rgt))*0.5f;
			vertexes[5]=(float)bot;
			break;

		case 3:
			vertexes[0]=(float)rgt;
			vertexes[1]=(float)top;
			vertexes[2]=(float)lft;
			vertexes[3]=((float)(top+bot))*0.5f;
			vertexes[4]=(float)rgt;
			vertexes[5]=(float)bot;
			break;
	}

		// setup draw
		
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glDisable(GL_DEPTH_TEST);

	glColor4f(col->r,col->g,col->b,alpha);

		// draw the trig
		
	glVertexPointer(2,GL_FLOAT,0,(GLvoid*)vertexes);

	glDrawArrays(GL_TRIANGLES,0,3);
	
		// finish draw

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
}

/* =======================================================

      2D Lines
      
======================================================= */

void view_primitive_2D_line(d3col *col,float alpha,int x0,int y0,int x1,int y1)
{
	float			vertexes[4];

	vertexes[0]=(float)x0;
	vertexes[1]=(float)y0;
	vertexes[2]=(float)x1;
	vertexes[3]=(float)y1;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glDisable(GL_DEPTH_TEST);

	glColor4f(col->r,col->g,col->b,alpha);

		// draw the quad
		
	glVertexPointer(2,GL_FLOAT,0,(GLvoid*)vertexes);

	glDrawArrays(GL_LINES,0,2);

		// finish draw

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
}

void view_primitive_2D_line_poly(d3col *col,float alpha,int x0,int y0,int x1,int y1,int x2,int y2,int x3,int y3)
{
	float			vertexes[8];

	vertexes[0]=(float)x0;
	vertexes[1]=(float)y0;
	vertexes[2]=(float)x1;
	vertexes[3]=(float)y1;
	vertexes[4]=(float)x2;
	vertexes[5]=(float)y2;
	vertexes[6]=(float)x3;
	vertexes[7]=(float)y3;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glDisable(GL_DEPTH_TEST);

	glColor4f(col->r,col->g,col->b,alpha);

		// draw the quad

	glVertexPointer(2,GL_FLOAT,0,(GLvoid*)vertexes);
	glDrawArrays(GL_LINE_LOOP,0,4);

		// finish draw

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
}

void view_primitive_2D_line_quad(d3col *col,float alpha,int lft,int rgt,int top,int bot)
{
	view_primitive_2D_line_poly(col,alpha,lft,top,rgt,top,rgt,bot,lft,bot);
}

void view_primitive_2D_line_trig(d3col *col,float alpha,int lft,int rgt,int top,int bot,int dir)
{
	float			vertexes[6];
	
		// get the vertexes

	switch (dir) {

		case 0:
			vertexes[0]=(float)lft;
			vertexes[1]=(float)bot;
			vertexes[2]=(float)rgt;
			vertexes[3]=(float)bot;
			vertexes[4]=((float)(lft+rgt))*0.5f;
			vertexes[5]=(float)top;
			break;

		case 1:
			vertexes[0]=(float)lft;
			vertexes[1]=(float)top;
			vertexes[2]=(float)rgt;
			vertexes[3]=((float)(top+bot))*0.5f;
			vertexes[4]=(float)lft;
			vertexes[5]=(float)bot;
			break;

		case 2:
			vertexes[0]=(float)lft;
			vertexes[1]=(float)top;
			vertexes[2]=(float)rgt;
			vertexes[3]=(float)top;
			vertexes[4]=((float)(lft+rgt))*0.5f;
			vertexes[5]=(float)bot;
			break;

		case 3:
			vertexes[0]=(float)rgt;
			vertexes[1]=(float)top;
			vertexes[2]=(float)lft;
			vertexes[3]=((float)(top+bot))*0.5f;
			vertexes[4]=(float)rgt;
			vertexes[5]=(float)bot;
			break;
	}

		// setup draw

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glDisable(GL_DEPTH_TEST);

	glColor4f(col->r,col->g,col->b,alpha);

		// draw the quad
		
	glVertexPointer(2,GL_FLOAT,0,(GLvoid*)vertexes);

	glDrawArrays(GL_LINE_LOOP,0,3);

		// finish draw

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
}

/* =======================================================

      3D Lines
      
======================================================= */

void view_primitive_3D_line(d3col *col,float alpha,int x0,int y0,int z0,int x1,int y1,int z1)
{
	float			vertexes[6];

	vertexes[0]=(float)x0;
	vertexes[1]=(float)y0;
	vertexes[2]=(float)z0;
	vertexes[3]=(float)x1;
	vertexes[4]=(float)y1;
	vertexes[5]=(float)z1;

		// setup draw

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glColor4f(col->r,col->g,col->b,alpha);

		// draw the quad
		
	glVertexPointer(3,GL_FLOAT,0,(GLvoid*)vertexes);

	glDrawArrays(GL_LINES,0,2);

		// finish draw

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
}

void view_primitive_3D_line_cube(d3col *col,float alpha,int *px,int *py,int *pz)
{
	int				n;
	unsigned short	cube_indexes[24]={0,1,1,2,2,3,3,0,4,5,5,6,6,7,7,4,0,4,1,5,2,6,3,7};
	float			vertexes[24];
	float			*vptr;

		// get the vertexes
		
	vptr=vertexes;
	
	for (n=0;n!=8;n++) {
		*vptr++=(float)px[n];
		*vptr++=(float)py[n];
		*vptr++=(float)pz[n];
	}
	
		// setup draw

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glColor4f(col->r,col->g,col->b,alpha);

		// draw the quad
		
	glVertexPointer(3,GL_FLOAT,0,(GLvoid*)vertexes);

	glDrawElements(GL_LINES,24,GL_UNSIGNED_SHORT,(GLvoid*)cube_indexes);

		// finish draw

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
}

/* =======================================================

      Textured Fills
      
======================================================= */

void view_primitive_2D_texture_quad(GLuint gl_id,d3col *col,float alpha,int lft,int rgt,int top,int bot,float gx,float gx2,float gy,float gy2)
{
	float			vertexes[8],uvs[8];

	vertexes[0]=(float)lft;
	vertexes[1]=(float)top;
	vertexes[2]=(float)lft;
	vertexes[3]=(float)bot;
	vertexes[4]=(float)rgt;
	vertexes[5]=(float)top;
	vertexes[6]=(float)rgt;
	vertexes[7]=(float)bot;
	
	uvs[0]=gx;
	uvs[1]=gy;
	uvs[2]=gx;
	uvs[3]=gy2;
	uvs[4]=gx2;
	uvs[5]=gy;
	uvs[6]=gx2;
	uvs[7]=gy2;

		// setup texture draw

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glDisable(GL_DEPTH_TEST);

	if (col==NULL) {
		glColor4f(1.0f,1.0f,1.0f,alpha);
	}
	else {
		glColor4f(col->r,col->g,col->b,alpha);
	}
	
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	gl_texture_bind(0,gl_id);

		// draw the quad

	glVertexPointer(2,GL_FLOAT,0,(GLvoid*)vertexes);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(GLvoid*)uvs);

	glDrawArrays(GL_TRIANGLE_STRIP,0,4);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

		// finish texture draw
	
	glDisable(GL_TEXTURE_2D);

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
}

void view_primitive_2D_texture_quad_rot(GLuint gl_id,d3col *col,float alpha,int lft,int rgt,int top,int bot,float ang,float gx,float gx2,float gy,float gy2)
{
	int				px[4],py[4];
	float			vertexes[8],uvs[8];
	
	px[0]=px[3]=lft;
	px[1]=px[2]=rgt;
	py[0]=py[1]=top;
	py[2]=py[3]=bot;

	if (ang!=0.0f) rotate_2D_polygon(4,px,py,((lft+rgt)>>1),((top+bot)>>1),ang);

	vertexes[0]=(float)px[0];
	vertexes[1]=(float)py[0];
	vertexes[2]=(float)px[3];
	vertexes[3]=(float)py[3];
	vertexes[4]=(float)px[1];
	vertexes[5]=(float)py[1];
	vertexes[6]=(float)px[2];
	vertexes[7]=(float)py[2];
	
	uvs[0]=gx;
	uvs[1]=gy;
	uvs[2]=gx;
	uvs[3]=gy2;
	uvs[4]=gx2;
	uvs[5]=gy;
	uvs[6]=gx2;
	uvs[7]=gy2;

		// setup texture draw

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glDisable(GL_DEPTH_TEST);

	if (col==NULL) {
		glColor4f(1.0f,1.0f,1.0f,alpha);
	}
	else {
		glColor4f(col->r,col->g,col->b,alpha);
	}
	
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	gl_texture_bind(0,gl_id);

		// draw the quad

	glVertexPointer(2,GL_FLOAT,0,(GLvoid*)vertexes);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(GLvoid*)uvs);

	glDrawArrays(GL_TRIANGLE_STRIP,0,4);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

		// finish texture draw
	
	glDisable(GL_TEXTURE_2D);

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
}

void view_primitive_2D_texture_quad_rectangle(GLuint gl_id,float alpha,int lft,int rgt,int top,int bot,int pixel_wid,int pixel_high)
{
#if defined(D3_OS_IPHONE) || defined(D3_OS_ANDRIOD)
	view_primitive_2D_texture_quad(gl_id,NULL,alpha,lft,rgt,top,bot,0.0f,1.0f,0.0f,1.0f);
#else
	float			vertexes[8],uvs[8];

	vertexes[0]=(float)lft;
	vertexes[1]=(float)top;
	vertexes[2]=(float)lft;
	vertexes[3]=(float)bot;
	vertexes[4]=(float)rgt;
	vertexes[5]=(float)top;
	vertexes[6]=(float)rgt;
	vertexes[7]=(float)bot;
	
	uvs[0]=0.0f;
	uvs[1]=0.0f;
	uvs[2]=0.0f;
	uvs[3]=(float)pixel_high;
	uvs[4]=(float)pixel_wid;
	uvs[5]=0.0f;
	uvs[6]=(float)pixel_wid;
	uvs[7]=(float)pixel_high;
	
		// setup texture draw

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glDisable(GL_DEPTH_TEST);

	glColor4f(1.0f,0.0f,1.0f,alpha);
	
	gl_texture_clear(0);

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_RECTANGLE);
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
	glBindTexture(GL_TEXTURE_RECTANGLE,gl_id);

		// draw the quad

	glVertexPointer(2,GL_FLOAT,0,(GLvoid*)vertexes);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(GLvoid*)uvs);

	glDrawArrays(GL_TRIANGLE_STRIP,0,4);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

		// finish texture draw
	
	glBindTexture(GL_TEXTURE_RECTANGLE,0);
	glDisable(GL_TEXTURE_RECTANGLE);

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	
#endif
}

