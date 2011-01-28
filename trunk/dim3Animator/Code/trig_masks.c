/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Model Trig Select/Hide Masks

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

unsigned char					*trig_mask_ptr;

/* =======================================================

      Initialize and Shutdown Trig Masks
      
======================================================= */

bool trig_mask_initialize(void)
{
	trig_mask_ptr=(unsigned char*)malloc(animator_max_trig*max_model_mesh);
	return(trig_mask_ptr!=NULL);
}

void trig_mask_shutdown(void)
{
	free(trig_mask_ptr);
}

unsigned char trig_mask_get(int mesh_idx,int trig_idx)
{
	return(*(trig_mask_ptr+((animator_max_trig*mesh_idx)+trig_idx)));
}

void trig_mask_set(int mesh_idx,int trig_idx,unsigned char mask)
{
	*(trig_mask_ptr+((animator_max_trig*mesh_idx)+trig_idx))=mask;
}

/* =======================================================

      Select Masks
      
======================================================= */

void trig_clear_sel_mask(int mesh_idx)
{
	int				n;
	unsigned char	mask;
	model_mesh_type	*mesh;
	
	mesh=&model.meshes[mesh_idx];
	
	for (n=0;n!=mesh->ntrig;n++) {
		mask=trig_mask_get(mesh_idx,n);
		mask&=(animator_mask_flag_sel^0xFF);
		trig_mask_set(mesh_idx,n,mask);
	}
}

void trig_set_sel_mask(int mesh_idx,int trig_idx,bool value)
{
	unsigned char	mask;
	
	mask=trig_mask_get(mesh_idx,trig_idx);
	
	if (value) {
		mask|=animator_mask_flag_sel;
	}
	else {
		mask&=(animator_mask_flag_sel^0xFF);
	}
	
	trig_mask_set(mesh_idx,trig_idx,mask);
}

bool trig_check_sel_mask(int mesh_idx,int trig_idx)
{
	unsigned char	mask;
	
	mask=trig_mask_get(mesh_idx,trig_idx);
	return((mask&animator_mask_flag_sel)!=0x0);
}

/* =======================================================

      Hide Masks
      
======================================================= */

void trig_clear_hide_mask(int mesh_idx)
{
	int				n;
	unsigned char	mask;
	model_mesh_type	*mesh;
	
	mesh=&model.meshes[mesh_idx];
	
	for (n=0;n!=mesh->nvertex;n++) {
		mask=trig_mask_get(mesh_idx,n);
		mask&=(animator_mask_flag_hide^0xFF);
		trig_mask_set(mesh_idx,n,mask);
	}
}

void trig_set_hide_mask(int mesh_idx,int trig_idx,bool value)
{
	unsigned char	mask;
	
	mask=trig_mask_get(mesh_idx,trig_idx);

	if (value) {
		mask|=animator_mask_flag_hide;
	}
	else {
		mask&=(animator_mask_flag_hide^0xFF);
	}
	
	trig_mask_set(mesh_idx,trig_idx,mask);
}

bool trig_check_hide_mask(int mesh_idx,int trig_idx)
{
	unsigned char	mask;

	mask=trig_mask_get(mesh_idx,trig_idx);
	return((mask&animator_mask_flag_hide)!=0x0);
}

/* =======================================================

      Select More
      
======================================================= */

void trig_mask_select_more(int mesh_idx)
{
	int					n,k,i,i2,t,t2,ntrig;
	bool				hit;
	unsigned char		*sel_mask;
	model_mesh_type		*mesh;
	model_trig_type		*trig,*trig2;

	mesh=&model.meshes[mesh_idx];
	ntrig=mesh->ntrig;

		// need sel mask outside of
		// regular sel mask

	sel_mask=(unsigned char*)malloc(ntrig);
	if (sel_mask==NULL) return;

	bzero(sel_mask,ntrig);

		// find hit triangles
	
	for (n=0;n!=ntrig;n++) {

		if ((!trig_check_sel_mask(mesh_idx,n)) || (trig_check_hide_mask(mesh_idx,n))) continue;
		trig=&model.meshes[mesh_idx].trigs[n];

		for (k=0;k!=ntrig;k++) {

			if (k==n) continue;

			if (trig_check_hide_mask(mesh_idx,k)) continue;
			trig2=&model.meshes[mesh_idx].trigs[k];

				// check for shared edges

			hit=FALSE;

			for (i=0;i!=3;i++) {
				i2=i+1;
				if (i2==3) i2=0;

				for (t=0;t!=3;t++) {
					t2=t+1;
					if (t2==3) t2=0;

					if ((trig2->v[t]==trig->v[i]) && (trig2->v[t2]==trig->v[i2])) {
						hit=TRUE;
						break;
					}

					if ((trig2->v[t]==trig->v[i2]) && (trig2->v[t2]==trig->v[i])) {
						hit=TRUE;
						break;
					}
				}

				if (hit) break;
			}

			if (hit) sel_mask[k]=0x1;
		}
	}

		// select triangles

	for (n=0;n!=ntrig;n++) {
		if (sel_mask[n]!=0x0) trig_set_sel_mask(mesh_idx,n,TRUE);
	}

	free(sel_mask);
	
		// get the new vertexes

	vertex_set_sel_mask_trig_mask(mesh_idx);
}
