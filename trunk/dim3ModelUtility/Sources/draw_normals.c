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

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
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
	
	majx=v->x;
	majy=v->y;
	majz=v->z;
	
	matrix_vertex_multiply_ignore_transform(&major_bone->rot_mat,&majx,&majy,&majz);	// commulative has a transform that needs to be ignored

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

	minx=v->x;
	miny=v->y;
	minz=v->z;
	
	matrix_vertex_multiply_ignore_transform(&minor_bone->rot_mat,&minx,&miny,&minz);	// commulative has a transform that needs to be ignored

		// average the points
		
	v->x=minx+((majx-minx)*bone_factor);
	v->y=miny+((majy-miny)*bone_factor);
	v->z=minz+((majz-minz)*bone_factor);
}

void model_move_single_tangent_space(model_mesh_type *mesh,model_draw_bone_type *draw_bones,model_trig_type *trig,int idx,tangent_space_type *space)
{
	model_vertex_type		*vertex;

		// get vertex

	vertex=&mesh->vertexes[trig->v[idx]];

		// start with original space

	space->tangent.x=trig->tangent_space[idx].tangent.x;
	space->tangent.y=trig->tangent_space[idx].tangent.y;
	space->tangent.z=trig->tangent_space[idx].tangent.z;

	space->binormal.x=trig->tangent_space[idx].binormal.x;
	space->binormal.y=trig->tangent_space[idx].binormal.y;
	space->binormal.z=trig->tangent_space[idx].binormal.z;

	space->normal.x=trig->tangent_space[idx].normal.x;
	space->normal.y=trig->tangent_space[idx].normal.y;
	space->normal.z=trig->tangent_space[idx].normal.z;

		// if no major bone, then no rotation
		
	if (vertex->major_bone_idx==-1) return;

		// move the space

	model_move_single_tangent_space_vector(draw_bones,vertex,&space->tangent);
	model_move_single_tangent_space_vector(draw_bones,vertex,&space->binormal);
	model_move_single_tangent_space_vector(draw_bones,vertex,&space->normal);
}

/* =======================================================

      Build Normal Lists
      
======================================================= */

void model_create_draw_normals(model_type *model,int mesh_idx,model_draw_setup *draw_setup)
{
	int						n,k,ntrig;
	float					*pt,*pb,*pn;
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
	pb=draw_setup->mesh_arrays[mesh_idx].gl_binormal_array;
	pn=draw_setup->mesh_arrays[mesh_idx].gl_normal_array;

		// normals with sways

	no_sway=((draw_setup->sway.x==0) && (draw_setup->sway.y==0) && (draw_setup->sway.z==0));

	if (!no_sway) {
	
		matrix_rotate_zyx(&sway_mat,draw_setup->sway.x,draw_setup->sway.y,draw_setup->sway.z);
		
		for (n=0;n!=ntrig;n++) {

			for (k=0;k!=3;k++) {
				model_move_single_tangent_space(mesh,draw_setup->bones,trig,k,&space);

				matrix_vertex_multiply(&sway_mat,&space.tangent.x,&space.tangent.y,&space.tangent.z);
				matrix_vertex_multiply(&sway_mat,&space.binormal.x,&space.binormal.y,&space.binormal.z);
				matrix_vertex_multiply(&sway_mat,&space.normal.x,&space.normal.y,&space.normal.z);

				matrix_vertex_multiply(&rot_mat,&space.tangent.x,&space.tangent.y,&space.tangent.z);
				matrix_vertex_multiply(&rot_mat,&space.binormal.x,&space.binormal.y,&space.binormal.z);
				matrix_vertex_multiply(&rot_mat,&space.normal.x,&space.normal.y,&space.normal.z);

				*pt++=space.tangent.x;
				*pt++=space.tangent.y;
				*pt++=space.tangent.z;

				*pb++=space.binormal.x;
				*pb++=space.binormal.y;
				*pb++=space.binormal.z;

				*pn++=space.normal.x;
				*pn++=space.normal.y;
				*pn++=space.normal.z;
			}
			
			trig++;
		}
		
		return;
	}

		// normals with no sways
		
	for (n=0;n!=ntrig;n++) {
		
		for (k=0;k!=3;k++) {
			model_move_single_tangent_space(mesh,draw_setup->bones,trig,k,&space);

			matrix_vertex_multiply(&rot_mat,&space.tangent.x,&space.tangent.y,&space.tangent.z);
			matrix_vertex_multiply(&rot_mat,&space.binormal.x,&space.binormal.y,&space.binormal.z);
			matrix_vertex_multiply(&rot_mat,&space.normal.x,&space.normal.y,&space.normal.z);

			*pt++=space.tangent.x;
			*pt++=space.tangent.y;
			*pt++=space.tangent.z;

			*pb++=space.binormal.x;
			*pb++=space.binormal.y;
			*pb++=space.binormal.z;

			*pn++=space.normal.x;
			*pn++=space.normal.y;
			*pn++=space.normal.z;
		}

		trig++;
	}
}
