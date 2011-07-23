/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
 Usage: Draw Normal Routines

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

(c) 2000-2010 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3modelutility.h"
#endif

/* =======================================================

      Position Model Normals
      
======================================================= */

void model_move_single_tangent_space_vector(model_draw_bone_type *draw_bones,model_vertex_type *vertex,d3vct *v)
{
	float					bone_factor,
							majx,majy,majz,minx,miny,minz;
	model_draw_bone_type	*major_bone,*minor_bone;

		// find major bone rotation

	major_bone=&draw_bones[vertex->major_bone_idx];
	
	majx=(v->x*major_bone->rot_mat.data[0][0])+(v->y*major_bone->rot_mat.data[0][1])+(v->z*major_bone->rot_mat.data[0][2]);
	majy=(v->x*major_bone->rot_mat.data[1][0])+(v->y*major_bone->rot_mat.data[1][1])+(v->z*major_bone->rot_mat.data[1][2]);
	majz=(v->x*major_bone->rot_mat.data[2][0])+(v->y*major_bone->rot_mat.data[2][1])+(v->z*major_bone->rot_mat.data[2][2]);

		// find minor bone
		
	bone_factor=vertex->bone_factor;
		
	if ((vertex->minor_bone_idx==-1) || (bone_factor==1)) {
		v->x=majx;
		v->y=majy;
		v->z=majz;
		return;
	}
		
		// find minor bone rotation
		
	minor_bone=&draw_bones[vertex->minor_bone_idx];
	
	minx=(v->x*minor_bone->rot_mat.data[0][0])+(v->y*minor_bone->rot_mat.data[0][1])+(v->z*minor_bone->rot_mat.data[0][2]);
	miny=(v->x*minor_bone->rot_mat.data[1][0])+(v->y*minor_bone->rot_mat.data[1][1])+(v->z*minor_bone->rot_mat.data[1][2]);
	minz=(v->x*minor_bone->rot_mat.data[2][0])+(v->y*minor_bone->rot_mat.data[2][1])+(v->z*minor_bone->rot_mat.data[2][2]);

		// average the points
		
	v->x=minx+((majx-minx)*bone_factor);
	v->y=miny+((majy-miny)*bone_factor);
	v->z=minz+((majz-minz)*bone_factor);
}

static inline void model_move_single_tangent_space(model_mesh_type *mesh,model_draw_bone_type *draw_bones,model_trig_type *trig,int idx,tangent_space_type *space,bool normal_only)
{
	model_vertex_type		*vertex;

		// get vertex

	vertex=&mesh->vertexes[trig->v[idx]];
	
		// default space
		
	if (!normal_only) {
		space->tangent.x=trig->tangent_space[idx].tangent.x;
		space->tangent.y=trig->tangent_space[idx].tangent.y;
		space->tangent.z=trig->tangent_space[idx].tangent.z;
	}
	
	space->normal.x=trig->tangent_space[idx].normal.x;
	space->normal.y=trig->tangent_space[idx].normal.y;
	space->normal.z=trig->tangent_space[idx].normal.z;

		// bone rotations
		
	if (vertex->major_bone_idx!=-1) {
		if (!normal_only) model_move_single_tangent_space_vector(draw_bones,vertex,&space->tangent);
		model_move_single_tangent_space_vector(draw_bones,vertex,&space->normal);
	}
}

/* =======================================================

      Build Normal Lists
      
======================================================= */

void model_create_draw_normals(model_type *model,int mesh_idx,model_draw_setup *draw_setup,bool normal_only)
{
	int						n,k,ntrig;
	float					*pt,*pn,fx,fy,fz;
	bool					no_sway;
	model_mesh_type			*mesh;
	model_trig_type			*trig;
	matrix_type				rot_mat,sway_mat;
	tangent_space_type		space;
	
	mesh=&model->meshes[mesh_idx];
	
		// rotations and sways
		
	matrix_rotate_xzy(&rot_mat,draw_setup->ang.x,draw_setup->ang.y,draw_setup->ang.z);
		
		// setup list
		
	ntrig=mesh->ntrig;
	trig=mesh->trigs;

	pt=draw_setup->mesh_arrays[mesh_idx].gl_tangent_array;
	pn=draw_setup->mesh_arrays[mesh_idx].gl_normal_array;

		// normals with sways

	no_sway=((draw_setup->sway.x==0) && (draw_setup->sway.y==0) && (draw_setup->sway.z==0));

	if (!no_sway) {
	
		matrix_rotate_zyx(&sway_mat,draw_setup->sway.x,draw_setup->sway.y,draw_setup->sway.z);
		
		for (n=0;n!=ntrig;n++) {

			for (k=0;k!=3;k++) {
				model_move_single_tangent_space(mesh,draw_setup->bones,trig,k,&space,normal_only);
				
				if (!normal_only) {
					fx=(space.tangent.x*sway_mat.data[0][0])+(space.tangent.y*sway_mat.data[0][1])+(space.tangent.z*sway_mat.data[0][2]);
					fy=(space.tangent.x*sway_mat.data[1][0])+(space.tangent.y*sway_mat.data[1][1])+(space.tangent.z*sway_mat.data[1][2]);
					fz=(space.tangent.x*sway_mat.data[2][0])+(space.tangent.y*sway_mat.data[2][1])+(space.tangent.z*sway_mat.data[2][2]);
				
					*pt++=(fx*rot_mat.data[0][0])+(fy*rot_mat.data[0][1])+(fz*rot_mat.data[0][2]);
					*pt++=(fx*rot_mat.data[1][0])+(fy*rot_mat.data[1][1])+(fz*rot_mat.data[1][2]);
					*pt++=(fx*rot_mat.data[2][0])+(fy*rot_mat.data[2][1])+(fz*rot_mat.data[2][2]);
				}
				
				fx=(space.normal.x*sway_mat.data[0][0])+(space.normal.y*sway_mat.data[0][1])+(space.normal.z*sway_mat.data[0][2]);
				fy=(space.normal.x*sway_mat.data[1][0])+(space.normal.y*sway_mat.data[1][1])+(space.normal.z*sway_mat.data[1][2]);
				fz=(space.normal.x*sway_mat.data[2][0])+(space.normal.y*sway_mat.data[2][1])+(space.normal.z*sway_mat.data[2][2]);
			
				*pn++=(fx*rot_mat.data[0][0])+(fy*rot_mat.data[0][1])+(fz*rot_mat.data[0][2]);
				*pn++=(fx*rot_mat.data[1][0])+(fy*rot_mat.data[1][1])+(fz*rot_mat.data[1][2]);
				*pn++=(fx*rot_mat.data[2][0])+(fy*rot_mat.data[2][1])+(fz*rot_mat.data[2][2]);
			}
			
			trig++;
		}
		
		return;
	}

		// normals with no sways
		
	for (n=0;n!=ntrig;n++) {
		
		for (k=0;k!=3;k++) {
			model_move_single_tangent_space(mesh,draw_setup->bones,trig,k,&space,normal_only);
	
			if (!normal_only) {
				*pt++=(space.tangent.x*rot_mat.data[0][0])+(space.tangent.y*rot_mat.data[0][1])+(space.tangent.z*rot_mat.data[0][2]);
				*pt++=(space.tangent.x*rot_mat.data[1][0])+(space.tangent.y*rot_mat.data[1][1])+(space.tangent.z*rot_mat.data[1][2]);
				*pt++=(space.tangent.x*rot_mat.data[2][0])+(space.tangent.y*rot_mat.data[2][1])+(space.tangent.z*rot_mat.data[2][2]);
			}
			
			*pn++=(space.normal.x*rot_mat.data[0][0])+(space.normal.y*rot_mat.data[0][1])+(space.normal.z*rot_mat.data[0][2]);
			*pn++=(space.normal.x*rot_mat.data[1][0])+(space.normal.y*rot_mat.data[1][1])+(space.normal.z*rot_mat.data[1][2]);
			*pn++=(space.normal.x*rot_mat.data[2][0])+(space.normal.y*rot_mat.data[2][1])+(space.normal.z*rot_mat.data[2][2]);
		}

		trig++;
	}
}
