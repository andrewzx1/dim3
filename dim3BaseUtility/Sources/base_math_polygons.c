/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Polygon Functions

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

int					cpi_ptsz,cpi_x[8],cpi_y[8],
					cpi_lft,cpi_rgt,cpi_top,cpi_bot;

/* =======================================================

      Find 3D Polygon Box, Center, Enlarge
      
======================================================= */

void polygon_get_box(int ptsz,int *px,int *py,int *pz,int *p_min_x,int *p_max_x,int *p_min_y,int *p_max_y,int *p_min_z,int *p_max_z)
{
	int			n,x,y,z,
				min_x,max_x,min_y,max_y,min_z,max_z;
	
	min_x=max_x=px[0];
	min_y=max_y=py[0];
	min_z=max_z=pz[0];
	
	for (n=1;n<ptsz;n++) {
		x=px[n];
		y=py[n];
		z=pz[n];
		if (x<min_x) min_x=x;
		if (x>max_x) max_x=x;
		if (y<min_y) min_y=y;
		if (y>max_y) max_y=y;
		if (z<min_z) min_z=z;
		if (z>max_z) max_z=z;
	}
	
	*p_min_x=min_x;
	*p_max_x=max_x;
	*p_min_y=min_y;
	*p_max_y=max_y;
	*p_min_z=min_z;
	*p_max_z=max_z;
}

void polygon_find_center(int ptsz,int *px,int *py,int *pz,int *mx,int *my,int *mz)
{
	int			min_x,max_x,min_y,max_y,min_z,max_z;

	polygon_get_box(ptsz,px,py,pz,&min_x,&max_x,&min_y,&max_y,&min_z,&max_z);
	
	*mx=(min_x+max_x)>>1;
	*my=(min_y+max_y)>>1;
	*mz=(min_z+max_z)>>1;
}

/* =======================================================

      Get 2D Polygon Box
      
======================================================= */

void polygon_2D_get_box(int ptsz,int *px,int *py,int *p_lft,int *p_rgt,int *p_top,int *p_bot)
{
	int			n,x,y,lft,rgt,top,bot;
	
	lft=rgt=px[0];
	top=bot=py[0];
	
	for (n=1;n<ptsz;n++) {
		x=px[n];
		y=py[n];
		if (x<lft) lft=x;
		if (x>rgt) rgt=x;
		if (y<top) top=y;
		if (y>bot) bot=y;
	}
	
	*p_lft=lft;
	*p_rgt=rgt;
	*p_top=top;
	*p_bot=bot;
}

/* =======================================================

      Points in 2D Polygon
      
======================================================= */

bool polygon_2D_point_inside(int ptsz,int *px,int *py,int x,int y)
{
	int			i,j,cnt;
	float		fx,fy,
				fpx[8],fpy[8];
	
		// convert to floats
		
	fx=(float)x;
	fy=(float)y;
	
	for (i=0;i!=ptsz;i++) {
		fpx[i]=(float)px[i];
		fpy[i]=(float)py[i];
	}
	
		// find the crossings
	
	cnt=0;
	j=ptsz-1;
	
	for (i=0;i!=ptsz;i++) {
        if ((((fpy[i]<=fy) && (fy<fpy[j])) || ((fpy[j]<=fy) && (fy<fpy[i]))) && (fx<(fpx[j]-fpx[i])*(fy-fpy[i])/(fpy[j]-fpy[i])+fpx[i])) cnt=!cnt;
		j=i;
	}
	
		// inside if crossing are odd
		
	return(cnt!=0);
}

bool polygon_2D_point_inside_f(int ptsz,d3fpnt *poly_pnt,d3fpnt *pnt)
{
	int			i,j,cnt;
	
		// find the crossings
	
	cnt=0;
	j=ptsz-1;
	
	for (i=0;i!=ptsz;i++) {
        if ((((poly_pnt[i].y<=pnt->y) && (pnt->y<poly_pnt[j].y)) || ((poly_pnt[j].y<=pnt->y) && (pnt->y<poly_pnt[i].y))) && (pnt->x<(poly_pnt[j].x-poly_pnt[i].x)*(pnt->y-poly_pnt[i].y)/(poly_pnt[j].y-poly_pnt[i].y)+poly_pnt[i].x)) cnt=!cnt;
		j=i;
	}
	
		// inside if crossing are odd
		
	return(cnt!=0);
}

bool polygon_2D_polygon_points_inside(int ptsz,int *px,int *py,int ptsz_2,int *px_2,int *py_2)
{
	int			n;
	
	for (n=0;n<ptsz_2;n++) {
		if (polygon_2D_point_inside(ptsz,px,py,px_2[n],py_2[n])) return(TRUE);
    }

	for (n=0;n<ptsz;n++) {
		if (polygon_2D_point_inside(ptsz_2,px_2,py_2,px[n],py[n])) return(TRUE);
    }
	
	return(FALSE);
}

/* =======================================================

      Lines in 2D Polygon
      
======================================================= */

bool polygon_2D_polygon_lines_intersect(int ptsz,int *px,int *py,int ptsz_2,int *px_2,int *py_2)
{
	int				n,n2,k,k2;
	
	for (n=0;n<ptsz;n++) {

		k=n+1;
		if (k==ptsz) k=0;

		for (n2=0;n2<ptsz_2;n2++) {
			
			k2=n2+1;
			if (k2==ptsz_2) k2=0;

			if (line_2D_test_intersect(px[n],py[n],px[k],py[k],px_2[n2],py_2[n2],px_2[k2],py_2[k2])) return(TRUE);
		}
	}

	return(FALSE);
}

/* =======================================================

      Polygon Collisions
      
======================================================= */

void polygon_2D_collision_setup(int ptsz,int *px,int *py)
{
	polygon_2D_get_box(ptsz,px,py,&cpi_lft,&cpi_rgt,&cpi_top,&cpi_bot);
	
	memmove(cpi_x,px,sizeof(int)*ptsz);
	memmove(cpi_y,py,sizeof(int)*ptsz);
	cpi_ptsz=ptsz;
}

bool polygon_2D_collision_line(int lx,int lz,int rx,int rz)
{
	int			n,k;
	
		// do rough comparison with polygon box
		
	if (lx<rx) {
		if (rx<cpi_lft) return(FALSE);
		if (lx>cpi_rgt) return(FALSE);
	}
	else {
		if (lx<cpi_lft) return(FALSE);
		if (rx>cpi_rgt) return(FALSE);
	}

	if (lz<rz) {
		if (rz<cpi_top) return(FALSE);
		if (lz>cpi_bot) return(FALSE);
	}
	else {
		if (lz<cpi_top) return(FALSE);
		if (rz>cpi_bot) return(FALSE);
	}
	
		// intersection lines?
		
	k=cpi_ptsz-1;
	
	for (n=0;n!=cpi_ptsz;n++) {
		if (line_2D_test_intersect(cpi_x[n],cpi_y[n],cpi_x[k],cpi_y[k],lx,lz,rx,rz)) return(TRUE);
		k=n;
	}
	
		// line completely in polygon
		
	if (polygon_2D_point_inside(cpi_ptsz,cpi_x,cpi_y,lx,lz)) return(TRUE);
	if (polygon_2D_point_inside(cpi_ptsz,cpi_x,cpi_y,rx,rz)) return(TRUE);
	
	return(FALSE);
}

bool polygon_2D_collision_bound_box(int lft,int rgt,int top,int bot)
{
	if (lft>cpi_rgt) return(FALSE);
	if (rgt<cpi_lft) return(FALSE);
	if (top>cpi_bot) return(FALSE);
	return(!(bot<cpi_top));
}

bool polygon_2D_collision_bound_polygon(int ptsz,int *x,int *y,int lft,int rgt,int top,int bot)
{
		// do loose hit against bounding boxes
		
	if (lft>cpi_rgt) return(FALSE);
	if (rgt<cpi_lft) return(FALSE);
	if (top>cpi_bot) return(FALSE);
	if (bot<cpi_top) return(FALSE);
	
		// are any of the points of one polygon are inside the other
		
	if (polygon_2D_polygon_points_inside(cpi_ptsz,cpi_x,cpi_y,ptsz,x,y)) return(TRUE);

		// now check for line intersections

	return(polygon_2D_polygon_lines_intersect(cpi_ptsz,cpi_x,cpi_y,ptsz,x,y));
}

bool polygon_2D_collision_polygon(int ptsz,int *x,int *y)
{
	int			lft,rgt,top,bot;
	
	polygon_2D_get_box(ptsz,x,y,&lft,&rgt,&top,&bot);
	return(polygon_2D_collision_bound_polygon(ptsz,x,y,lft,rgt,top,bot));
}

/* =======================================================

      Find Y at X,Z in Polygon
      
======================================================= */

int polygon_find_y(int ptsz,int *px,int *py,int *pz,int kx,int kz)
{
	int				n,k,
					npt,tz,bz,nz,dz,lx,rx,ly,ry,x[2],y[2];
	
		// quick check for y lying on polygon points
	
	for (n=0;n<ptsz;n++) {
		if ((px[n]==kx) && (pz[n]==kz)) return(py[n]);
	}
	
		// find two z lines that intersect kz
		
	npt=0;
	
	for (n=0;n<ptsz;n++) {
		tz=pz[n];
		k=n+1;
		if (k==ptsz) k=0;

		bz=pz[k];
		
		if (tz<bz) {
			if ((kz<tz) || (kz>bz)) continue;
		}
		else {
			if ((kz<bz) || (kz>tz)) continue;
		}
		
			// found a good line
			
		lx=px[n];
		rx=px[k];
		ly=py[n];
		ry=py[k];
			
		if (tz==bz) {			// special check for horizontal line
			if (lx<=rx) {
				if ((kx<lx) || (kx>rx)) return(-1);		// outside of lines
			}
			else {
				if ((kx<rx) || (kx>lx)) return(-1);
			}
			return(ly+(((ry-ly)*abs(kx-lx))/abs(rx-lx)));
		}

		nz=abs(kz-tz);
		dz=abs(bz-tz);
		x[npt]=lx+(((rx-lx)*nz)/dz);
		y[npt]=ly+(((ry-ly)*nz)/dz);
		
		npt++;
		if (npt==2) break;
	}
	
	if (npt!=2) return(-1);			// line must be outside
	
	lx=x[0];
	ly=y[0];
	rx=x[1];
	ry=y[1];
	
	if (lx<=rx) {
		if ((kx<lx) || (kx>rx)) return(-1);		// outside of lines
	}
	else {
		if ((kx<rx) || (kx>lx)) return(-1);
	}
	
	k=abs(rx-lx);
	if (k==0) return(0);
	
	return(ly+(((ry-ly)*abs(kx-lx))/k));
}

