/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Angle Functions

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit �Created with dim3 Technology� is given on a single
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
	#include "dim3baseutility.h"
#endif

/* =======================================================

      Adding Angles
      
======================================================= */

float angle_add(float ang1,float ang2)
{
	ang1+=ang2;
	return(ang1-(360.0f*floorf(ang1/360.0f)));
}

float angle_dif(float ang1,float ang2,bool *cwise)
{
	float		subway,addway;
	
	if (cwise!=NULL) *cwise=TRUE;
		
	if (ang1==ang2) return(0.0f);
	
		// which way is faster?
	
	if (ang1>ang2) {
		subway=ang1-ang2;
		addway=360.0f-(ang1-ang2);
	}
	else {
		subway=360.0f-(ang2-ang1);
		addway=ang2-ang1;
	}
		
		// subtraction is faster
		
	if (subway<addway) {
		if (cwise!=NULL) *cwise=FALSE;
		return(subway);
	}
	
	return(addway);
}

/* =======================================================

      Get X,Z and Y movement for speed at angle
      
======================================================= */

void angle_get_movement(float ang,int mv,int *xadd,int *zadd)
{
	float			rad,fmv;

	rad=ang*ANG_to_RAD;
	fmv=(float)mv;

	*xadd=(int)(fmv*sinf(rad));
	*zadd=-(int)(fmv*cosf(rad));
}

void angle_get_movement_float(float ang,float fmv,float *xadd,float *zadd)
{
	float			rad,dmv;
	
	rad=ang*ANG_to_RAD;
	dmv=(float)fmv;
	
	*xadd=(fmv*sinf(rad));
	*zadd=-(fmv*cosf(rad));
}

void angle_add_movement(float ang,int mv,int *x,int *z)
{
	float			rad,fmv;

	rad=ang*ANG_to_RAD;
	fmv=(float)mv;

	*x+=(int)(fmv*sinf(rad));
	*z-=(int)(fmv*cosf(rad));
}

/* =======================================================

      Find Angle from one point to another
      
======================================================= */

float angle_find(int x,int z,int tox,int toz)
{
	float		fang,fx,fz;
	
	if (x==tox) {
		if (toz>z) return(180.0f);
		return(0.0f);
	}
	if (z==toz) {
		if (tox>x) return(90.0f);
		return(270.0f);
	}

	fx=(float)abs(tox-x);
	fz=(float)abs(toz-z);
	fang=atan2f(fx,fz)*RAD_to_ANG;
	
	if (toz>z) {
		if (tox<x) return(180.0f+fang);
		return(180.0f-fang);
	}
	
	if (tox<x) return(360.0f-fang);
	return(fang);
}

float angle_turn_toward(float ang,float rang,float mv)
{
	float		subway,addway;
	
	if (ang==rang) return(ang);
	
		// which way is faster?
	
	if (ang>rang) {
		subway=ang-rang;
		addway=360.0f-(ang-rang);
	}
	else {
		subway=360.0f-(rang-ang);
		addway=rang-ang;
	}
		
		// subtraction is faster
		
	if (subway<addway) {
		if (mv>=subway) return(rang);
		return(angle_add(ang,-mv));
	}
	
		// addition is faster
		
	if (mv>=addway) return(rang);
	return(angle_add(ang,mv));
}

