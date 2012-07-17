/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: OpenGL ES Patches

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

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "interface.h"

typedef struct		{
						float			x,y,z;
					} d3_d_vct;

// supergumba -- remove all this!


/* =======================================================

      OpenGL ES Missing Functions
	  gluLookAt
      
======================================================= */

void glu_patch_vector_normalize(d3_d_vct *v)
{
	float			dist,fx,fy,fz;

	fx=v->x*v->x;
	fy=v->y*v->y;
	fz=v->z*v->z;
	
	dist=sqrtf(fx+fy+fz);
	if (dist==0.0f) return;
	
	dist=1.0f/dist;
	v->x*=dist;
	v->y*=dist;
	v->z*=dist;
}

void glu_patch_vector_cross_product(d3_d_vct *v,d3_d_vct *v1,d3_d_vct *v2)
{
	v->x=(v1->y*v2->z)-(v2->y*v1->z);
    v->y=(v1->z*v2->x)-(v2->z*v1->x);
    v->z=(v1->x*v2->y)-(v2->x*v1->y);
}

 
/* =======================================================

      OpenGL ES Missing Functions
	  gluProject
      
======================================================= */

void glu_patch_matrix_mult_vector(float mat[16],float v_in[4],float v_out[4])
{
	v_out[0]=(v_in[0]*mat[0])+(v_in[1]*mat[4])+(v_in[2]*mat[8])+(v_in[3]*mat[12]);
	v_out[1]=(v_in[0]*mat[1])+(v_in[1]*mat[5])+(v_in[2]*mat[9])+(v_in[3]*mat[13]);
	v_out[2]=(v_in[0]*mat[2])+(v_in[1]*mat[6])+(v_in[2]*mat[10])+(v_in[3]*mat[14]);
	v_out[3]=(v_in[0]*mat[3])+(v_in[1]*mat[7])+(v_in[2]*mat[11])+(v_in[3]*mat[15]);
}

void glu_patch_matrix_mult_matrix(float mat_1[16],float mat_2[16],float mat_out[16])
{
	int			n;

	for (n=0;n!=4;n++) {
		mat_out[(n*4)]=
			(mat_1[(n*4)]*mat_2[0])+
			(mat_1[(n*4)+1]*mat_2[4])+
			(mat_1[(n*4)+2]*mat_2[8])+
			(mat_1[(n*4)+3]*mat_2[12]);
		mat_out[(n*4)+1]=
			(mat_1[(n*4)]*mat_2[1])+
			(mat_1[(n*4)+1]*mat_2[5])+
			(mat_1[(n*4)+2]*mat_2[9])+
			(mat_1[(n*4)+3]*mat_2[13]);
		mat_out[(n*4)+2]=
			(mat_1[(n*4)]*mat_2[2])+
			(mat_1[(n*4)+1]*mat_2[6])+
			(mat_1[(n*4)+2]*mat_2[10])+
			(mat_1[(n*4)+3]*mat_2[14]);
		mat_out[(n*4)+3]=
			(mat_1[(n*4)]*mat_2[3])+
			(mat_1[(n*4)+1]*mat_2[7])+
			(mat_1[(n*4)+2]*mat_2[11])+
			(mat_1[(n*4)+3]*mat_2[15]);
	}
}

bool glu_patch_matrix_invert(float mat_in[16],float mat_out[16])
{
	int					n;
	float				mat_inv[16],det;

    mat_inv[0]=
		(mat_in[5]*mat_in[10]*mat_in[15])-
		(mat_in[5]*mat_in[11]*mat_in[14])-
		(mat_in[9]*mat_in[6]*mat_in[15])+
		(mat_in[9]*mat_in[7]*mat_in[14])+
		(mat_in[13]*mat_in[6]*mat_in[11])-
		(mat_in[13]*mat_in[7]*mat_in[10]);
    mat_inv[4]=
		(-mat_in[4]*mat_in[10]*mat_in[15])+
		(mat_in[4]*mat_in[11]*mat_in[14])+
		(mat_in[8]*mat_in[6]*mat_in[15])-
		(mat_in[8]*mat_in[7]*mat_in[14])-
		(mat_in[12]*mat_in[6]*mat_in[11])+
		(mat_in[12]*mat_in[7]*mat_in[10]);
    mat_inv[8]=
		(mat_in[4]*mat_in[9]*mat_in[15])-
		(mat_in[4]*mat_in[11]*mat_in[13])-
		(mat_in[8]*mat_in[5]*mat_in[15])+
		(mat_in[8]*mat_in[7]*mat_in[13])+
		(mat_in[12]*mat_in[5]*mat_in[11])-
		(mat_in[12]*mat_in[7]*mat_in[9]);
    mat_inv[12]=
		(-mat_in[4]*mat_in[9]*mat_in[14])+
		(mat_in[4]*mat_in[10]*mat_in[13])+
		(mat_in[8]*mat_in[5]*mat_in[14])-
		(mat_in[8]*mat_in[6]*mat_in[13])-
		(mat_in[12]*mat_in[5]*mat_in[10])+
		(mat_in[12]*mat_in[6]*mat_in[9]);
    mat_inv[1]=
		(-mat_in[1]*mat_in[10]*mat_in[15])+
		(mat_in[1]*mat_in[11]*mat_in[14])+
		(mat_in[9]*mat_in[2]*mat_in[15])-
		(mat_in[9]*mat_in[3]*mat_in[14])-
		(mat_in[13]*mat_in[2]*mat_in[11])+
		(mat_in[13]*mat_in[3]*mat_in[10]);
    mat_inv[5]=
		(mat_in[0]*mat_in[10]*mat_in[15])-
		(mat_in[0]*mat_in[11]*mat_in[14])-
		(mat_in[8]*mat_in[2]*mat_in[15])+
		(mat_in[8]*mat_in[3]*mat_in[14])+
		(mat_in[12]*mat_in[2]*mat_in[11])-
		(mat_in[12]*mat_in[3]*mat_in[10]);
    mat_inv[9]=
		(-mat_in[0]*mat_in[9]*mat_in[15])+
		(mat_in[0]*mat_in[11]*mat_in[13])+
		(mat_in[8]*mat_in[1]*mat_in[15])-
		(mat_in[8]*mat_in[3]*mat_in[13])-
		(mat_in[12]*mat_in[1]*mat_in[11])+
		(mat_in[12]*mat_in[3]*mat_in[9]);
    mat_inv[13]=
		(mat_in[0]*mat_in[9]*mat_in[14])-
		(mat_in[0]*mat_in[10]*mat_in[13])-
		(mat_in[8]*mat_in[1]*mat_in[14])+
		(mat_in[8]*mat_in[2]*mat_in[13])+
		(mat_in[12]*mat_in[1]*mat_in[10])-
		(mat_in[12]*mat_in[2]*mat_in[9]);
    mat_inv[2]=
		(mat_in[1]*mat_in[6]*mat_in[15])-
		(mat_in[1]*mat_in[7]*mat_in[14])-
		(mat_in[5]*mat_in[2]*mat_in[15])+
		(mat_in[5]*mat_in[3]*mat_in[14])+
		(mat_in[13]*mat_in[2]*mat_in[7])-
		(mat_in[13]*mat_in[3]*mat_in[6]);
    mat_inv[6]=
		(-mat_in[0]*mat_in[6]*mat_in[15])+
		(mat_in[0]*mat_in[7]*mat_in[14])+
		(mat_in[4]*mat_in[2]*mat_in[15])-
		(mat_in[4]*mat_in[3]*mat_in[14])-
		(mat_in[12]*mat_in[2]*mat_in[7])+
		(mat_in[12]*mat_in[3]*mat_in[6]);
    mat_inv[10]=
		(mat_in[0]*mat_in[5]*mat_in[15])-
		(mat_in[0]*mat_in[7]*mat_in[13])-
		(mat_in[4]*mat_in[1]*mat_in[15])+
		(mat_in[4]*mat_in[3]*mat_in[13])+
		(mat_in[12]*mat_in[1]*mat_in[7])-
		(mat_in[12]*mat_in[3]*mat_in[5]);
    mat_inv[14]=
		(-mat_in[0]*mat_in[5]*mat_in[14])+
		(mat_in[0]*mat_in[6]*mat_in[13])+
		(mat_in[4]*mat_in[1]*mat_in[14])-
		(mat_in[4]*mat_in[2]*mat_in[13])-
		(mat_in[12]*mat_in[1]*mat_in[6])+
		(mat_in[12]*mat_in[2]*mat_in[5]);
    mat_inv[3]=
		(-mat_in[1]*mat_in[6]*mat_in[11])+
		(mat_in[1]*mat_in[7]*mat_in[10])+
		(mat_in[5]*mat_in[2]*mat_in[11])-
		(mat_in[5]*mat_in[3]*mat_in[10])-
		(mat_in[9]*mat_in[2]*mat_in[7])+
		(mat_in[9]*mat_in[3]*mat_in[6]);
    mat_inv[7]=
		(mat_in[0]*mat_in[6]*mat_in[11])-
		(mat_in[0]*mat_in[7]*mat_in[10])-
		(mat_in[4]*mat_in[2]*mat_in[11])+
		(mat_in[4]*mat_in[3]*mat_in[10])+
		(mat_in[8]*mat_in[2]*mat_in[7])-
		(mat_in[8]*mat_in[3]*mat_in[6]);
    mat_inv[11]=
		(-mat_in[0]*mat_in[5]*mat_in[11])+
		(mat_in[0]*mat_in[7]*mat_in[9])+
		(mat_in[4]*mat_in[1]*mat_in[11])-
		(mat_in[4]*mat_in[3]*mat_in[9])-
		(mat_in[8]*mat_in[1]*mat_in[7])+
		(mat_in[8]*mat_in[3]*mat_in[5]);
    mat_inv[15]=
		(mat_in[0]*mat_in[5]*mat_in[10])-
		(mat_in[0]*mat_in[6]*mat_in[9])-
		(mat_in[4]*mat_in[1]*mat_in[10])+
		(mat_in[4]*mat_in[2]*mat_in[9])+
		(mat_in[8]*mat_in[1]*mat_in[6])-
		(mat_in[8]*mat_in[2]*mat_in[5]);

    det=(mat_in[0]*mat_inv[0])+(mat_in[1]*mat_inv[4])+(mat_in[2]*mat_inv[8])+(mat_in[3]*mat_inv[12]);
    if (det==0.0f) return(FALSE);

    det=1.0f/det;

	for (n=0;n!=16;n++) {
        mat_out[n]=mat_inv[n]*det;
	}

    return(TRUE);
}

bool glu_patch_gluProject(float objx,float objy,float objz,float modelMatrix[16],float projMatrix[16],int viewport[4],float *winx,float *winy,float *winz)
{
    float		in[4],out[4];

    in[0]=objx;
    in[1]=objy;
    in[2]=objz;
    in[3]=1.0f;

    glu_patch_matrix_mult_vector(modelMatrix,in,out);
    glu_patch_matrix_mult_vector(projMatrix,out,in);
    if (in[3]==0.0f) return(FALSE);

	in[0]=((in[0]/in[3])*0.5f)+0.5f;
    in[1]=((in[1]/in[3])*0.5f)+0.5f;
    in[2]=((in[2]/in[3])*0.5f)+0.5f;

    in[0]=(in[0]*viewport[2])+viewport[0];
    in[1]=(in[1]*viewport[3])+viewport[1];

    *winx=in[0];
    *winy=in[1];
    *winz=in[2];

    return(TRUE);
}

