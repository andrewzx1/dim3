/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: View Clipping Routines

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
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "interface.h"

extern map_type				map;
extern editor_setup_type	setup;
extern editor_state_type	state;

extern double				view_mod_matrix[16],view_proj_matrix[16];

float						view_cull_frustum[6][4];

/* =======================================================

      Frustum/Bound Box Clipping
      
======================================================= */

void view_cull_setup_frustum_clipping_planes(void)
{
	int			n;
	float		f,clip[16];

		// combine the matrixes

	clip[0]=(float)((view_mod_matrix[0]*view_proj_matrix[0])+(view_mod_matrix[1]*view_proj_matrix[4])+(view_mod_matrix[2]*view_proj_matrix[8])+(view_mod_matrix[3]*view_proj_matrix[12]));
	clip[1]=(float)((view_mod_matrix[0]*view_proj_matrix[1])+(view_mod_matrix[1]*view_proj_matrix[5])+(view_mod_matrix[2]*view_proj_matrix[9])+(view_mod_matrix[3]*view_proj_matrix[13]));
	clip[2]=(float)((view_mod_matrix[0]*view_proj_matrix[2])+(view_mod_matrix[1]*view_proj_matrix[6])+(view_mod_matrix[2]*view_proj_matrix[10])+(view_mod_matrix[3]*view_proj_matrix[14]));
	clip[3]=(float)((view_mod_matrix[0]*view_proj_matrix[3])+(view_mod_matrix[1]*view_proj_matrix[7])+(view_mod_matrix[2]*view_proj_matrix[11])+(view_mod_matrix[3]*view_proj_matrix[15]));

	clip[4]=(float)((view_mod_matrix[4]*view_proj_matrix[0])+(view_mod_matrix[5]*view_proj_matrix[4])+(view_mod_matrix[6]*view_proj_matrix[8])+(view_mod_matrix[7]*view_proj_matrix[12]));
	clip[5]=(float)((view_mod_matrix[4]*view_proj_matrix[1])+(view_mod_matrix[5]*view_proj_matrix[5])+(view_mod_matrix[6]*view_proj_matrix[9])+(view_mod_matrix[7]*view_proj_matrix[13]));
	clip[6]=(float)((view_mod_matrix[4]*view_proj_matrix[2])+(view_mod_matrix[5]*view_proj_matrix[6])+(view_mod_matrix[6]*view_proj_matrix[10])+(view_mod_matrix[7]*view_proj_matrix[14]));
	clip[7]=(float)((view_mod_matrix[4]*view_proj_matrix[3])+(view_mod_matrix[5]*view_proj_matrix[7])+(view_mod_matrix[6]*view_proj_matrix[11])+(view_mod_matrix[7]*view_proj_matrix[15]));

	clip[8]=(float)((view_mod_matrix[8]*view_proj_matrix[0])+(view_mod_matrix[9]*view_proj_matrix[4])+(view_mod_matrix[10]*view_proj_matrix[8])+(view_mod_matrix[11]*view_proj_matrix[12]));
	clip[9]=(float)((view_mod_matrix[8]*view_proj_matrix[1])+(view_mod_matrix[9]*view_proj_matrix[5])+(view_mod_matrix[10]*view_proj_matrix[9])+(view_mod_matrix[11]*view_proj_matrix[13]));
	clip[10]=(float)((view_mod_matrix[8]*view_proj_matrix[2])+(view_mod_matrix[9]*view_proj_matrix[6])+(view_mod_matrix[10]*view_proj_matrix[10])+(view_mod_matrix[11]*view_proj_matrix[14]));
	clip[11]=(float)((view_mod_matrix[8]*view_proj_matrix[3])+(view_mod_matrix[9]*view_proj_matrix[7])+(view_mod_matrix[10]*view_proj_matrix[11])+(view_mod_matrix[11]*view_proj_matrix[15]));

	clip[12]=(float)((view_mod_matrix[12]*view_proj_matrix[0])+(view_mod_matrix[13]*view_proj_matrix[4])+(view_mod_matrix[14]*view_proj_matrix[8])+(view_mod_matrix[15]*view_proj_matrix[12]));
	clip[13]=(float)((view_mod_matrix[12]*view_proj_matrix[1])+(view_mod_matrix[13]*view_proj_matrix[5])+(view_mod_matrix[14]*view_proj_matrix[9])+(view_mod_matrix[15]*view_proj_matrix[13]));
	clip[14]=(float)((view_mod_matrix[12]*view_proj_matrix[2])+(view_mod_matrix[13]*view_proj_matrix[6])+(view_mod_matrix[14]*view_proj_matrix[10])+(view_mod_matrix[15]*view_proj_matrix[14]));
	clip[15]=(float)((view_mod_matrix[12]*view_proj_matrix[3])+(view_mod_matrix[13]*view_proj_matrix[7])+(view_mod_matrix[14]*view_proj_matrix[11])+(view_mod_matrix[15]*view_proj_matrix[15]));

		// left plane
		
	view_cull_frustum[0][0]=clip[3]+clip[0];
	view_cull_frustum[0][1]=clip[7]+clip[4];
	view_cull_frustum[0][2]=clip[11]+clip[8];
	view_cull_frustum[0][3]=clip[15]+clip[12];

		// right plane
		
	view_cull_frustum[1][0]=clip[3]-clip[0];
	view_cull_frustum[1][1]=clip[7]-clip[4];
	view_cull_frustum[1][2]=clip[11]-clip[8];
	view_cull_frustum[1][3]=clip[15]-clip[12];

		// top plane
		
	view_cull_frustum[2][0]=clip[3]-clip[1];
	view_cull_frustum[2][1]=clip[7]-clip[5];
	view_cull_frustum[2][2]=clip[11]-clip[9];
	view_cull_frustum[2][3]=clip[15]-clip[13];

		// bottom plane
		
	view_cull_frustum[3][0]=clip[3]+clip[1];
	view_cull_frustum[3][1]=clip[7]+clip[5];
	view_cull_frustum[3][2]=clip[11]+clip[9];
	view_cull_frustum[3][3]=clip[15]+clip[13];

		// near plane
		
	view_cull_frustum[4][0]=clip[3]+clip[2];
	view_cull_frustum[4][1]=clip[7]+clip[6];
	view_cull_frustum[4][2]=clip[11]+clip[10];
	view_cull_frustum[4][3]=clip[15]+clip[14];

		// far plane
		
	view_cull_frustum[5][0]=clip[3]-clip[2];
	view_cull_frustum[5][1]=clip[7]-clip[6];
	view_cull_frustum[5][2]=clip[11]-clip[10];
	view_cull_frustum[5][3]=clip[15]-clip[14];
	
		// normalize the ABCD plane
		
	for (n=0;n!=6;n++) {
		f=sqrtf((view_cull_frustum[n][0]*view_cull_frustum[n][0])+(view_cull_frustum[n][1]*view_cull_frustum[n][1])+(view_cull_frustum[n][2]*view_cull_frustum[n][2]));
		view_cull_frustum[n][0]/=f;
		view_cull_frustum[n][1]/=f;
		view_cull_frustum[n][2]/=f;
		view_cull_frustum[n][3]/=f;
	}
}

bool view_cull_boundbox_in_frustum(d3pnt *min,d3pnt *max)
{
	int				n;
	d3fpnt			f_min,f_max;
	
	f_min.x=(float)min->x;
	f_min.y=(float)min->y;
	f_min.z=(float)min->z;

	f_max.x=(float)max->x;
	f_max.y=(float)max->y;
	f_max.z=(float)max->z;

	for (n=0;n!=6;n++) {
		if (((view_cull_frustum[n][0]*f_min.x)+(view_cull_frustum[n][1]*f_min.y)+(view_cull_frustum[n][2]*f_min.z)+view_cull_frustum[n][3])>0.0f) continue;
		if (((view_cull_frustum[n][0]*f_max.x)+(view_cull_frustum[n][1]*f_min.y)+(view_cull_frustum[n][2]*f_min.z)+view_cull_frustum[n][3])>0.0f) continue;
		if (((view_cull_frustum[n][0]*f_min.x)+(view_cull_frustum[n][1]*f_max.y)+(view_cull_frustum[n][2]*f_min.z)+view_cull_frustum[n][3])>0.0f) continue;
		if (((view_cull_frustum[n][0]*f_max.x)+(view_cull_frustum[n][1]*f_max.y)+(view_cull_frustum[n][2]*f_min.z)+view_cull_frustum[n][3])>0.0f) continue;
		if (((view_cull_frustum[n][0]*f_min.x)+(view_cull_frustum[n][1]*f_min.y)+(view_cull_frustum[n][2]*f_max.z)+view_cull_frustum[n][3])>0.0f) continue;
		if (((view_cull_frustum[n][0]*f_max.x)+(view_cull_frustum[n][1]*f_min.y)+(view_cull_frustum[n][2]*f_max.z)+view_cull_frustum[n][3])>0.0f) continue;
		if (((view_cull_frustum[n][0]*f_min.x)+(view_cull_frustum[n][1]*f_max.y)+(view_cull_frustum[n][2]*f_max.z)+view_cull_frustum[n][3])>0.0f) continue;
		if (((view_cull_frustum[n][0]*f_max.x)+(view_cull_frustum[n][1]*f_max.y)+(view_cull_frustum[n][2]*f_max.z)+view_cull_frustum[n][3])>0.0f) continue;

		return(FALSE);
	}

	return(TRUE);
}
