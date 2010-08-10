/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: OpenGL ES Patches

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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "video.h"

/* =======================================================

      OpenGL ES Missing Functions
	  gluPerspective
      
======================================================= */

void es_patch_gluPerspective(float fovy,float aspect,float zNear,float zFar)
{
	float		x_min,x_max,y_min,y_max;

	y_max=zNear*(float)tan((fovy*TRIG_PI)/360.0f);
	y_min=-y_max;
	x_min=y_min*aspect;
	x_max=y_max*aspect;

	glFrustum(x_min,x_max,y_min,y_max,zNear,zFar);
}

/* =======================================================

      OpenGL ES Missing Functions
	  gluLookAt
      
======================================================= */

 void es_patch_gluLookAt(float eyex,float eyey,float eyez,float centerx,float centery,float centerz,float upx,float upy,float upz)
 {
	 float			mat[16];
	 d3vct			x,y,z;

		// create the z vector

	z.x=eyex-centerx;
	z.y=eyey-centery;
	z.z=eyez-centerz;
	vector_normalize(&z);

		// create the y vector

	y.x=upx;
	y.y=upy;
	y.z=upz;
		
		// create x vector, y cross z

	vector_cross_product(&x,&y,&z);

		// recreate the y from z cross x

	vector_cross_product(&y,&z,&x);

		// normalize x and y

	vector_normalize(&x);
	vector_normalize(&y);

		// create the rotate matrix
	
	mat[0+0]=x.x;
	mat[0+4]=x.y;
	mat[0+8]=x.z;
	mat[0+12]=0.0f;

	mat[1+0]=y.x;
	mat[1+4]=y.y;
	mat[1+8]=y.z;
	mat[1+12]=0.0f;

	mat[2+0]=z.x;
	mat[2+4]=z.y;
	mat[2+8]=z.z;
	mat[2+12]=0.0f;

	mat[3+0]=0.0f;
	mat[3+4]=0.0f;
	mat[3+8]=0.0f;
	mat[3+12]=1.0f;

	glMultMatrixf(mat);

		// translate eye to origin

    glTranslatef(-eyex,-eyey,-eyez);
}

