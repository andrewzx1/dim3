/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Model Vertex Select/Hide Masks

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

unsigned char					*vertex_mask_ptr;

/* =======================================================

      Initialize and Shutdown Vertex Masks
      
======================================================= */

bool vertex_mask_initialize(void)
{
	vertex_mask_ptr=(unsigned char*)malloc(animator_max_vertex*max_model_mesh);
	return(vertex_mask_ptr!=NULL);
}

void vertex_mask_shutdown(void)
{
	free(vertex_mask_ptr);
}

unsigned char vertex_mask_get(int mesh_idx,int vertex_idx)
{
	return(*(vertex_mask_ptr+((animator_max_vertex*mesh_idx)+vertex_idx)));
}

void vertex_mask_set(int mesh_idx,int vertex_idx,unsigned char mask)
{
	*(vertex_mask_ptr+((animator_max_vertex*mesh_idx)+vertex_idx))=mask;
}

/* =======================================================

      Select Masks
      
======================================================= */

void vertex_clear_sel_mask(int mesh_idx)
{
	int				n;
	unsigned char	mask;
	model_mesh_type	*mesh;
	
	mesh=&model.meshes[mesh_idx];
	
	for (n=0;n!=mesh->nvertex;n++) {
		mask=vertex_mask_get(mesh_idx,n);
		mask&=(animator_mask_flag_sel^0xFF);
		vertex_mask_set(mesh_idx,n,mask);
	}
}

void vertex_set_sel_mask(int mesh_idx,int vertex_idx,bool value)
{
	unsigned char	mask;
	
	mask=vertex_mask_get(mesh_idx,vertex_idx);
	
	if (value) {
		mask|=animator_mask_flag_sel;
	}
	else {
		mask&=(animator_mask_flag_sel^0xFF);
	}
	
	vertex_mask_set(mesh_idx,vertex_idx,mask);
}

void vertex_set_sel_mask_all(int mesh_idx)
{
	int				n;
	unsigned char	mask;
	model_mesh_type	*mesh;
	
	mesh=&model.meshes[mesh_idx];
	
	for (n=0;n!=mesh->nvertex;n++) {
		mask=vertex_mask_get(mesh_idx,n);
		mask|=animator_mask_flag_sel;
		vertex_mask_set(mesh_idx,n,mask);
	}
}

bool vertex_check_sel_any(int mesh_idx)
{
	int				n;
	unsigned char	mask;
	model_mesh_type	*mesh;
	
	mesh=&model.meshes[mesh_idx];
	
	for (n=0;n!=mesh->nvertex;n++) {
		mask=vertex_mask_get(mesh_idx,n);
		if ((mask&animator_mask_flag_sel)!=0x0) return(TRUE);
	}
	
	return(FALSE);
}

bool vertex_check_sel_mask(int mesh_idx,int vertex_idx)
{
	unsigned char	mask;
	
	mask=vertex_mask_get(mesh_idx,vertex_idx);
	return((mask&animator_mask_flag_sel)!=0x0);
}

/* =======================================================

      Hide Masks
      
======================================================= */

void vertex_clear_hide_mask(int mesh_idx)
{
	int				n;
	unsigned char	mask;
	model_mesh_type	*mesh;
	
	mesh=&model.meshes[mesh_idx];
	
	for (n=0;n!=mesh->nvertex;n++) {
		mask=vertex_mask_get(mesh_idx,n);
		mask&=(animator_mask_flag_hide^0xFF);
		vertex_mask_set(mesh_idx,n,mask);
	}
}

void vertex_set_hide_mask(int mesh_idx,int vertex_idx,bool value)
{
	unsigned char	mask;
	
	mask=vertex_mask_get(mesh_idx,vertex_idx);

	if (value) {
		mask|=animator_mask_flag_hide;
	}
	else {
		mask&=(animator_mask_flag_hide^0xFF);
	}
	
	vertex_mask_set(mesh_idx,vertex_idx,mask);
}

bool vertex_check_hide_mask(int mesh_idx,int vertex_idx)
{
	unsigned char	mask;

	mask=vertex_mask_get(mesh_idx,vertex_idx);
	return((mask&animator_mask_flag_hide)!=0x0);
}

/* =======================================================

      Hide/Select Interactions
      
======================================================= */

void vertex_hide_mask_set_sel_vertexes(int mesh_idx)
{
	int				n,nt;
	
	nt=model.meshes[mesh_idx].nvertex;
	
	for (n=0;n!=nt;n++) {
		if (vertex_check_sel_mask(mesh_idx,n)) vertex_set_hide_mask(mesh_idx,n,TRUE);
	}
	
	vertex_clear_sel_mask(mesh_idx);
}

void vertex_hide_mask_set_non_sel_vertexes(int mesh_idx)
{
	int				n,nt;
	
	nt=model.meshes[mesh_idx].nvertex;
	
	for (n=0;n!=nt;n++) {
		if (!vertex_check_sel_mask(mesh_idx,n)) vertex_set_hide_mask(mesh_idx,n,TRUE);
	}
}

void vertex_hide_mask_show_all_vertexes(int mesh_idx)
{
	vertex_clear_hide_mask(mesh_idx);
}

/* =======================================================

      Hide/Trig Interactions
      
======================================================= */

bool vertex_check_hide_mask_trig(int mesh_idx,model_trig_type *trig)
{
	int				n;
	
	for (n=0;n!=3;n++) {
		if (vertex_check_hide_mask(mesh_idx,trig->v[n])) return(TRUE);
	}
	
	return(FALSE);
}

/* =======================================================

      Sel/Bone Interactions
      
======================================================= */

void vertex_set_sel_mask_bone(int mesh_idx,int bone_idx)
{
	int					i,nt;
	model_vertex_type	*vertex;
	
		// clear selection
		
	vertex_clear_sel_mask(mesh_idx);
	
		// find vertexes attached to bone

	nt=model.meshes[mesh_idx].nvertex;
		
	vertex=model.meshes[mesh_idx].vertexes;
	
	for (i=0;i!=nt;i++) {
	
		if ((vertex->major_bone_idx==bone_idx) || (vertex->minor_bone_idx==bone_idx)) {
			if (!vertex_check_hide_mask(mesh_idx,i)) vertex_set_sel_mask(mesh_idx,i,TRUE);
		}
		
		vertex++;
	}
}

void vertex_set_sel_mask_no_bone(int mesh_idx)
{
	int					i,nt;
	model_vertex_type	*vertex;
	
		// clear selection
		
	vertex_clear_sel_mask(mesh_idx);
	
		// find vertexes to no bone

	nt=model.meshes[mesh_idx].nvertex;
		
	vertex=model.meshes[mesh_idx].vertexes;
	
	for (i=0;i!=nt;i++) {
	
		if ((vertex->major_bone_idx==-1) && (vertex->minor_bone_idx==-1)) {
			if (!vertex_check_hide_mask(mesh_idx,i)) vertex_set_sel_mask(mesh_idx,i,TRUE);
		}
		
		vertex++;
	}
}

void vertex_set_sel_mask_near_bone(int mesh_idx,int bone_idx,float percentage)
{
	int					i,nt,x,y,z,v_dist,dist;
	model_bone_type		*bone;
	model_vertex_type	*vertex;
	
		// get bone to check
		
	bone=&model.bones[bone_idx];
	
		// clear selection
		
	vertex_clear_sel_mask(mesh_idx);

		// get check distance
		
	x=model.view_box.size.x;
	z=model.view_box.size.z;
	y=model.view_box.size.y;
	
	v_dist=(int)(sqrt((x*x)+(z*z)+(y*y)));
	v_dist=(int)((float)v_dist*percentage);
	
	nt=model.meshes[mesh_idx].nvertex;
	vertex=model.meshes[mesh_idx].vertexes;
	
	for (i=0;i!=nt;i++) {
	
		x=labs(bone->pnt.x-vertex->pnt.x);
		z=labs(bone->pnt.z-vertex->pnt.z);
		y=labs(bone->pnt.y-vertex->pnt.y);
		dist=(int)(sqrt((x*x)+(z*z)+(y*y)));
	
		if (dist<v_dist) vertex_set_sel_mask(mesh_idx,i,TRUE);
	
		vertex++;
	}
	
}

/* =======================================================

      Sel/Trig Interactions
      
======================================================= */

void vertex_set_sel_mask_trig_mask(int mesh_idx)
{
	int					n,k,ntrig;
	model_mesh_type		*mesh;
	model_trig_type		*trig;

		// clear vertexes

	vertex_clear_sel_mask(mesh_idx);

		// set for all trigs

	mesh=&model.meshes[mesh_idx];
	ntrig=mesh->ntrig;

		// selection
	
	for (n=0;n!=ntrig;n++) {

		if ((!trig_check_sel_mask(mesh_idx,n)) || (trig_check_hide_mask(mesh_idx,n))) continue;
		
		trig=&model.meshes[mesh_idx].trigs[n];
	
		for (k=0;k!=3;k++) {
			vertex_set_sel_mask(mesh_idx,trig->v[k],TRUE);
		}
	}
}

/* =======================================================

      Set Vertexes To Bone
      
======================================================= */

void vertex_set_sel_vertex_to_bone(int mesh_idx,int major_bone_idx,int minor_bone_idx,float factor)
{
	int					i,nt;
	model_vertex_type	*vertex;
	
		// attach vertexes to bone
		
	nt=model.meshes[mesh_idx].nvertex;
	vertex=model.meshes[mesh_idx].vertexes;
	
	for (i=0;i!=nt;i++) {
	
		if (vertex_check_sel_mask(mesh_idx,i)) {
			vertex->major_bone_idx=major_bone_idx;
			vertex->minor_bone_idx=minor_bone_idx;
			vertex->bone_factor=factor;
		}
		
		vertex++;
	}
}

/* =======================================================

      Sel/Material Interactions
      
======================================================= */

void vertex_set_sel_mask_material(int mesh_idx,int material_idx)
{
	int					i,k,nt;
	model_trig_type		*trig;
	model_material_type	*material;
	
		// clear selection
		
	vertex_clear_sel_mask(mesh_idx);
	
		// select vertexes in trigs

	material=&model.meshes[mesh_idx].materials[material_idx];
	trig=&model.meshes[mesh_idx].trigs[material->trig_start];
	
	nt=material->trig_count;
	
	for (i=0;i!=nt;i++) {
	
		for (k=0;k!=3;k++) {
			if (!vertex_check_hide_mask(mesh_idx,trig->v[k])) vertex_set_sel_mask(mesh_idx,trig->v[k],TRUE);
		}
		
		trig++;
	}
}

