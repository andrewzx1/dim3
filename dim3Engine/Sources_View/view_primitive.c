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

float						view_primitive_vertexes[24],
							view_primitive_uvs[24];
unsigned short				view_primitive_cube_indexes[24]={0,1,1,2,2,3,3,0,4,5,5,6,6,7,7,4,0,4,1,5,2,6,3,7},
							view_primitive_line_poly_indexes[8]={0,1,1,2,2,3,3,0};
unsigned char				view_primitive_colors[24];

/* =======================================================

      Screen Tint
      
======================================================= */

void view_draw_next_vertex_object_2D_tint_screen(void)
{
	view_primitive_vertexes[0]=0.0f;
	view_primitive_vertexes[1]=0.0f;
	view_primitive_vertexes[2]=0.0f;
	view_primitive_vertexes[3]=(float)setup.screen.y_sz;
	view_primitive_vertexes[4]=(float)setup.screen.x_sz;
	view_primitive_vertexes[5]=0.0f;
	view_primitive_vertexes[6]=(float)setup.screen.x_sz;
	view_primitive_vertexes[7]=(float)setup.screen.y_sz;

	glVertexPointer(2,GL_FLOAT,0,(GLvoid*)view_primitive_vertexes);

	glDrawArrays(GL_TRIANGLE_STRIP,0,4);
}

/* =======================================================

      2D Fills
      
======================================================= */

void view_draw_next_vertex_object_2D_color_poly(int x0,int y0,d3col *col0,int x1,int y1,d3col *col1,int x2,int y2,d3col *col2,int x3,int y3,d3col *col3,float alpha)
{
	unsigned char	uc_alpha;
								
	view_primitive_vertexes[0]=x0;
	view_primitive_vertexes[1]=y0;
	view_primitive_vertexes[2]=x3;
	view_primitive_vertexes[3]=y3;
	view_primitive_vertexes[4]=x1;
	view_primitive_vertexes[5]=y1;
	view_primitive_vertexes[6]=x2;
	view_primitive_vertexes[7]=y2;
	
	uc_alpha=(unsigned char)(alpha*255.0f);
	
	view_primitive_colors[0]=(unsigned char)(col0->r*255.0f);
	view_primitive_colors[1]=(unsigned char)(col0->g*255.0f);
	view_primitive_colors[2]=(unsigned char)(col0->b*255.0f);
	view_primitive_colors[3]=uc_alpha;
	view_primitive_colors[4]=(unsigned char)(col3->r*255.0f);
	view_primitive_colors[5]=(unsigned char)(col3->g*255.0f);
	view_primitive_colors[6]=(unsigned char)(col3->b*255.0f);
	view_primitive_colors[7]=uc_alpha;
	view_primitive_colors[8]=(unsigned char)(col1->r*255.0f);
	view_primitive_colors[9]=(unsigned char)(col1->g*255.0f);
	view_primitive_colors[10]=(unsigned char)(col1->b*255.0f);
	view_primitive_colors[11]=uc_alpha;
	view_primitive_colors[12]=(unsigned char)(col2->r*255.0f);
	view_primitive_colors[13]=(unsigned char)(col2->g*255.0f);
	view_primitive_colors[14]=(unsigned char)(col2->b*255.0f);
	view_primitive_colors[15]=uc_alpha;
		
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glDisable(GL_DEPTH_TEST);

		// draw the polygon

	glEnableClientState(GL_COLOR_ARRAY);
	
	glVertexPointer(2,GL_FLOAT,0,(GLvoid*)view_primitive_vertexes);
	glColorPointer(4,GL_UNSIGNED_BYTE,0,(GLvoid*)view_primitive_colors);

	glDrawArrays(GL_TRIANGLE_STRIP,0,4);

 	glDisableClientState(GL_COLOR_ARRAY);
		
		// finish draw

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
}

void view_draw_next_vertex_object_2D_color_quad(d3col *col,float alpha,int lft,int rgt,int top,int bot)
{
	view_draw_next_vertex_object_2D_color_poly(lft,top,col,rgt,top,col,rgt,bot,col,lft,bot,col,alpha);
}

void view_draw_next_vertex_object_2D_color_trig(d3col *col,float alpha,int lft,int rgt,int top,int bot,int dir)
{
	switch (dir) {

		case 0:
			view_primitive_vertexes[0]=(float)lft;
			view_primitive_vertexes[1]=(float)bot;
			view_primitive_vertexes[2]=(float)rgt;
			view_primitive_vertexes[3]=(float)bot;
			view_primitive_vertexes[4]=((float)(lft+rgt))*0.5f;
			view_primitive_vertexes[5]=(float)top;
			break;

		case 1:
			view_primitive_vertexes[0]=(float)lft;
			view_primitive_vertexes[1]=(float)top;
			view_primitive_vertexes[2]=(float)rgt;
			view_primitive_vertexes[3]=((float)(top+bot))*0.5f;
			view_primitive_vertexes[4]=(float)lft;
			view_primitive_vertexes[5]=(float)bot;
			break;

		case 2:
			view_primitive_vertexes[0]=(float)lft;
			view_primitive_vertexes[1]=(float)top;
			view_primitive_vertexes[2]=(float)rgt;
			view_primitive_vertexes[3]=(float)top;
			view_primitive_vertexes[4]=((float)(lft+rgt))*0.5f;
			view_primitive_vertexes[5]=(float)bot;
			break;

		case 3:
			view_primitive_vertexes[0]=(float)rgt;
			view_primitive_vertexes[1]=(float)top;
			view_primitive_vertexes[2]=(float)lft;
			view_primitive_vertexes[3]=((float)(top+bot))*0.5f;
			view_primitive_vertexes[4]=(float)rgt;
			view_primitive_vertexes[5]=(float)bot;
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
		
	glVertexPointer(2,GL_FLOAT,0,(GLvoid*)view_primitive_vertexes);

	glDrawArrays(GL_TRIANGLES,0,3);
	
		// finish draw

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
}

/* =======================================================

      2D Lines
      
======================================================= */

void view_draw_next_vertex_object_2D_line(d3col *col,float alpha,int x0,int y0,int x1,int y1)
{
	view_primitive_vertexes[0]=x0;
	view_primitive_vertexes[1]=y0;
	view_primitive_vertexes[2]=x1;
	view_primitive_vertexes[3]=y1;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glDisable(GL_DEPTH_TEST);

	glColor4f(col->r,col->g,col->b,alpha);

		// draw the quad
		
	glVertexPointer(2,GL_FLOAT,0,(GLvoid*)view_primitive_vertexes);

	glDrawArrays(GL_LINES,0,2);

		// finish draw

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
}

void view_draw_next_vertex_object_2D_line_poly(d3col *col,float alpha,int x0,int y0,int x1,int y1,int x2,int y2,int x3,int y3)
{
	view_primitive_vertexes[0]=x0;
	view_primitive_vertexes[1]=y0;
	view_primitive_vertexes[2]=x1;
	view_primitive_vertexes[3]=y1;
	view_primitive_vertexes[4]=x2;
	view_primitive_vertexes[5]=y2;
	view_primitive_vertexes[6]=x3;
	view_primitive_vertexes[7]=y3;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glDisable(GL_DEPTH_TEST);

	glColor4f(col->r,col->g,col->b,alpha);

		// draw the quad

	glVertexPointer(2,GL_FLOAT,0,(GLvoid*)view_primitive_vertexes);
	
	glDrawElements(GL_LINES,8,GL_UNSIGNED_SHORT,(GLvoid*)view_primitive_line_poly_indexes);

		// finish draw

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
}

void view_draw_next_vertex_object_2D_line_quad(d3col *col,float alpha,int lft,int rgt,int top,int bot)
{
	view_draw_next_vertex_object_2D_line_poly(col,alpha,lft,top,rgt,top,rgt,bot,lft,bot);
}

void view_draw_next_vertex_object_2D_line_trig(d3col *col,float alpha,int lft,int rgt,int top,int bot,int dir)
{
		// get the vertexes

	switch (dir) {

		case 0:
			view_primitive_vertexes[0]=(float)lft;
			view_primitive_vertexes[1]=(float)bot;
			view_primitive_vertexes[2]=(float)rgt;
			view_primitive_vertexes[3]=(float)bot;
			view_primitive_vertexes[4]=((float)(lft+rgt))*0.5f;
			view_primitive_vertexes[5]=(float)top;
			break;

		case 1:
			view_primitive_vertexes[0]=(float)lft;
			view_primitive_vertexes[1]=(float)top;
			view_primitive_vertexes[2]=(float)rgt;
			view_primitive_vertexes[3]=((float)(top+bot))*0.5f;
			view_primitive_vertexes[4]=(float)lft;
			view_primitive_vertexes[5]=(float)bot;
			break;

		case 2:
			view_primitive_vertexes[0]=(float)lft;
			view_primitive_vertexes[1]=(float)top;
			view_primitive_vertexes[2]=(float)rgt;
			view_primitive_vertexes[3]=(float)top;
			view_primitive_vertexes[4]=((float)(lft+rgt))*0.5f;
			view_primitive_vertexes[5]=(float)bot;
			break;

		case 3:
			view_primitive_vertexes[0]=(float)rgt;
			view_primitive_vertexes[1]=(float)top;
			view_primitive_vertexes[2]=(float)lft;
			view_primitive_vertexes[3]=((float)(top+bot))*0.5f;
			view_primitive_vertexes[4]=(float)rgt;
			view_primitive_vertexes[5]=(float)bot;
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
		
	glVertexPointer(2,GL_FLOAT,0,(GLvoid*)view_primitive_vertexes);

	glDrawArrays(GL_LINE_LOOP,0,3);

		// finish draw

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
}

/* =======================================================

      3D Lines
      
======================================================= */

void view_draw_next_vertex_object_3D_line(d3col *col,float alpha,int x0,int y0,int z0,int x1,int y1,int z1)
{
	view_primitive_vertexes[0]=x0;
	view_primitive_vertexes[1]=y0;
	view_primitive_vertexes[2]=z0;
	view_primitive_vertexes[3]=x1;
	view_primitive_vertexes[4]=y1;
	view_primitive_vertexes[5]=z1;

		// setup draw

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glColor4f(col->r,col->g,col->b,alpha);

		// draw the quad
		
	glVertexPointer(3,GL_FLOAT,0,(GLvoid*)view_primitive_vertexes);

	glDrawArrays(GL_LINES,0,2);

		// finish draw

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
}

void view_draw_next_vertex_object_3D_line_cube(d3col *col,float alpha,int *px,int *py,int *pz)
{
	int				n;
	float			*vptr;

		// get the vertexes
		
	vptr=view_primitive_vertexes;
	
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
		
	glVertexPointer(3,GL_FLOAT,0,(GLvoid*)view_primitive_vertexes);

	glDrawElements(GL_LINES,24,GL_UNSIGNED_SHORT,(GLvoid*)view_primitive_cube_indexes);

		// finish draw

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
}

/* =======================================================

      Textured Fills
      
======================================================= */

void view_draw_next_vertex_object_2D_texture_quad(GLuint gl_id,d3col *col,float alpha,int lft,int rgt,int top,int bot,float gx,float gx2,float gy,float gy2)
{
	view_primitive_vertexes[0]=(float)lft;
	view_primitive_vertexes[1]=(float)top;
	view_primitive_vertexes[2]=(float)lft;
	view_primitive_vertexes[3]=(float)bot;
	view_primitive_vertexes[4]=(float)rgt;
	view_primitive_vertexes[5]=(float)top;
	view_primitive_vertexes[6]=(float)rgt;
	view_primitive_vertexes[7]=(float)bot;
	
	view_primitive_uvs[0]=gx;
	view_primitive_uvs[1]=gy;
	view_primitive_uvs[2]=gx;
	view_primitive_uvs[3]=gy2;
	view_primitive_uvs[4]=gx2;
	view_primitive_uvs[5]=gy;
	view_primitive_uvs[6]=gx2;
	view_primitive_uvs[7]=gy2;

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

	glVertexPointer(2,GL_FLOAT,0,(GLvoid*)view_primitive_vertexes);
	glTexCoordPointer(2,GL_FLOAT,0,(GLvoid*)view_primitive_uvs);

	glDrawArrays(GL_TRIANGLE_STRIP,0,4);

		// finish texture draw
	
	glDisable(GL_TEXTURE_2D);

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
}

void view_draw_next_vertex_object_2D_texture_quad_rectangle(GLuint gl_id,float alpha,int lft,int rgt,int top,int bot,int pixel_wid,int pixel_high)
{
#ifdef D3_OS_IPHONE
	view_draw_next_vertex_object_2D_texture_quad(gl_id,NULL,alpha,lft,rgt,top,bot,0.0f,1.0f,0.0f,1.0f);
#else
	view_primitive_vertexes[0]=(float)lft;
	view_primitive_vertexes[1]=(float)top;
	view_primitive_vertexes[2]=(float)lft;
	view_primitive_vertexes[3]=(float)bot;
	view_primitive_vertexes[4]=(float)rgt;
	view_primitive_vertexes[5]=(float)top;
	view_primitive_vertexes[6]=(float)rgt;
	view_primitive_vertexes[7]=(float)bot;
	
	view_primitive_uvs[0]=0.0f;
	view_primitive_uvs[1]=0.0f;
	view_primitive_uvs[2]=0.0f;
	view_primitive_uvs[3]=(float)pixel_high;
	view_primitive_uvs[4]=(float)pixel_wid;
	view_primitive_uvs[5]=0.0f;
	view_primitive_uvs[6]=(float)pixel_wid;
	view_primitive_uvs[7]=(float)pixel_high;
	
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

	glVertexPointer(2,GL_FLOAT,0,(GLvoid*)view_primitive_vertexes);
	glTexCoordPointer(2,GL_FLOAT,0,(GLvoid*)view_primitive_uvs);

	glDrawArrays(GL_TRIANGLE_STRIP,0,4);

		// finish texture draw
	
	glBindTexture(GL_TEXTURE_RECTANGLE,0);
	glDisable(GL_TEXTURE_RECTANGLE);

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	
#endif
}

