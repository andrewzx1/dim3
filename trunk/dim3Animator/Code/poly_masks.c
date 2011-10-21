/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Model Poly Select/Hide Masks

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
#include "interface.h"

extern model_type				model;

unsigned char					*poly_mask_ptr;

/* =======================================================

      Initialize and Shutdown Poly Masks
      
======================================================= */

bool poly_mask_initialize(void)
{
	poly_mask_ptr=(unsigned char*)malloc(animator_max_poly*max_model_mesh);
	return(poly_mask_ptr!=NULL);
}

void poly_mask_shutdown(void)
{
	free(poly_mask_ptr);
}

unsigned char poly_mask_get(int mesh_idx,int poly_idx)
{
	return(*(poly_mask_ptr+((animator_max_poly*mesh_idx)+poly_idx)));
}

void poly_mask_set(int mesh_idx,int poly_idx,unsigned char mask)
{
	*(poly_mask_ptr+((animator_max_poly*mesh_idx)+poly_idx))=mask;
}

/* =======================================================

      Select Masks
      
======================================================= */

void poly_mask_clear_sel(int mesh_idx)
{
	int				n;
	unsigned char	mask;
	model_mesh_type	*mesh;
	
	mesh=&model.meshes[mesh_idx];
	
	for (n=0;n!=mesh->npoly;n++) {
		mask=poly_mask_get(mesh_idx,n);
		mask&=(animator_mask_flag_sel^0xFF);
		poly_mask_set(mesh_idx,n,mask);
	}
}

void poly_mask_set_sel(int mesh_idx,int poly_idx,bool value)
{
	unsigned char	mask;
	
	mask=poly_mask_get(mesh_idx,poly_idx);
	
	if (value) {
		mask|=animator_mask_flag_sel;
	}
	else {
		mask&=(animator_mask_flag_sel^0xFF);
	}
	
	poly_mask_set(mesh_idx,poly_idx,mask);
}

bool poly_mask_check_sel(int mesh_idx,int poly_idx)
{
	unsigned char	mask;
	
	mask=poly_mask_get(mesh_idx,poly_idx);
	return((mask&animator_mask_flag_sel)!=0x0);
}

/* =======================================================

      Hide Masks
      
======================================================= */

void poly_mask_clear_hide(int mesh_idx)
{
	int				n;
	unsigned char	mask;
	model_mesh_type	*mesh;
	
	mesh=&model.meshes[mesh_idx];
	
	for (n=0;n!=mesh->nvertex;n++) {
		mask=poly_mask_get(mesh_idx,n);
		mask&=(animator_mask_flag_hide^0xFF);
		poly_mask_set(mesh_idx,n,mask);
	}
}

void poly_mask_set_hide(int mesh_idx,int poly_idx,bool value)
{
	unsigned char	mask;
	
	mask=poly_mask_get(mesh_idx,poly_idx);

	if (value) {
		mask|=animator_mask_flag_hide;
	}
	else {
		mask&=(animator_mask_flag_hide^0xFF);
	}
	
	poly_mask_set(mesh_idx,poly_idx,mask);
}

bool poly_mask_check_hide(int mesh_idx,int poly_idx)
{
	unsigned char	mask;

	mask=poly_mask_get(mesh_idx,poly_idx);
	return((mask&animator_mask_flag_hide)!=0x0);
}

/* =======================================================

      Single Poly Selection
      
======================================================= */

model_poly_type* poly_mask_get_single_select(int mesh_idx)
{
	int					n,npoly,idx;
	model_mesh_type		*mesh;

	mesh=&model.meshes[mesh_idx];
	npoly=mesh->npoly;

		// find first and only selection

	idx=-1;

	for (n=0;n!=npoly;n++) {
		if ((poly_mask_check_sel(mesh_idx,n)) && (!poly_mask_check_hide(mesh_idx,n))) {
			if (idx!=-1) return(NULL);		// more than one!
			idx=n;
		}
	}

	if (idx==-1) return(NULL);

	return(&mesh->polys[idx]);
}

/* =======================================================

      Select More
      
======================================================= */

void poly_mask_select_more(int mesh_idx)
{
	int					n,k,i,i2,t,t2,npoly;
	bool				hit;
	unsigned char		*sel_mask;
	model_mesh_type		*mesh;
	model_poly_type		*poly,*poly2;

	mesh=&model.meshes[mesh_idx];
	npoly=mesh->npoly;

		// need sel mask outside of
		// regular sel mask

	sel_mask=(unsigned char*)malloc(npoly);
	if (sel_mask==NULL) return;

	bzero(sel_mask,npoly);

		// find hit triangles
	
	for (n=0;n!=npoly;n++) {

		if ((!poly_mask_check_sel(mesh_idx,n)) || (poly_mask_check_hide(mesh_idx,n))) continue;
		poly=&model.meshes[mesh_idx].polys[n];

		for (k=0;k!=npoly;k++) {

			if (k==n) continue;

			if (poly_mask_check_hide(mesh_idx,k)) continue;
			poly2=&model.meshes[mesh_idx].polys[k];

				// check for shared edges

			hit=FALSE;

			for (i=0;i!=poly->ptsz;i++) {
				i2=i+1;
				if (i2==poly->ptsz) i2=0;

				for (t=0;t!=poly2->ptsz;t++) {
					t2=t+1;
					if (t2==poly2->ptsz) t2=0;

					if ((poly2->v[t]==poly->v[i]) && (poly2->v[t2]==poly->v[i2])) {
						hit=TRUE;
						break;
					}

					if ((poly2->v[t]==poly->v[i2]) && (poly2->v[t2]==poly->v[i])) {
						hit=TRUE;
						break;
					}
				}

				if (hit) break;
			}

			if (hit) sel_mask[k]=0x1;
		}
	}

		// select polygons

	for (n=0;n!=npoly;n++) {
		if (sel_mask[n]!=0x0) poly_mask_set_sel(mesh_idx,n,TRUE);
	}

	free(sel_mask);
	
		// get the new vertexes

	vertex_mask_set_sel_poly_mask(mesh_idx);
}
