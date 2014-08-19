/****************************** File *********************************

Module: dim3 Auto Generator
Author: Brian Barnes
 Usage: Model Auto Generation Pieces

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

(c) 2000-2014 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3autogenerate.h"
#endif

#define ag_model_piece_foot_x_size			100
#define ag_model_piece_foot_y_size			100
#define ag_model_piece_foot_z_size			250

/* =======================================================

      Set Vertex and Polys
      
======================================================= */

void ag_model_piece_add_vertex(model_type *model,model_vertex_type *vertex,int bone_idx,int x_off,int y_off,int z_off)
{
	model_bone_type		*bone;

	bone=&model->bones[bone_idx];

	vertex->pnt.x=bone->pnt.x+x_off;
	vertex->pnt.y=bone->pnt.y+y_off;
	vertex->pnt.z=bone->pnt.z+z_off;

		// bone connection

	vertex->major_bone_idx=bone_idx;
	vertex->minor_bone_idx=-1;
	vertex->bone_factor=1.0f;

		// tangent space

	vector_create(&vertex->tangent_space.normal,vertex->pnt.x,vertex->pnt.y,vertex->pnt.z,bone->pnt.x,bone->pnt.y,bone->pnt.z);
}

void ag_model_piece_add_quad(model_type *model,model_poly_type *poly,int v_off,int v1_idx,int v2_idx,int v3_idx,int v4_idx)
{
	poly->ptsz=4;
	poly->txt_idx=0;

	poly->v[0]=v_off+v1_idx;
	poly->v[1]=v_off+v2_idx;
	poly->v[2]=v_off+v3_idx;
	poly->v[3]=v_off+v4_idx;

	poly->gx[0]=poly->gx[3]=0.0f;
	poly->gx[1]=poly->gx[2]=0.5f;
	poly->gy[0]=poly->gy[1]=0.0f;
	poly->gy[2]=poly->gy[3]=0.5f;
}

/* =======================================================

      Hand
      
======================================================= */

void ag_model_piece_bone_hand(model_type *model,model_mesh_type *mesh,int bone_idx,int limb_radius)
{
}

/* =======================================================

      Feet
      
======================================================= */

void ag_model_piece_bone_foot(model_type *model,model_mesh_type *mesh,int bone_idx,int limb_radius)
{
	int					vertex_idx,poly_idx,
						z_front,z_mid,z_back,y_top,y_mid,high;
	model_vertex_type	*vertex;
	model_poly_type		*poly;

		// add the vertexes and polys

	vertex_idx=mesh->nvertex;
	model_mesh_set_vertex_count(model,0,(vertex_idx+12));

	poly_idx=mesh->npoly;
	model_mesh_set_poly_count(model,0,(poly_idx+10));

		// y and z coordinates

	z_front=-ag_model_piece_foot_z_size;
	z_mid=-limb_radius;
	z_back=limb_radius<<1;

	high=model->bones[bone_idx].pnt.y;
	if (model->bones[bone_idx].parent_idx!=-1) high=model->bones[model->bones[bone_idx].parent_idx].pnt.y;

	y_mid=high>>1;
	y_top=high;

		// build the vertexes
	
	vertex=&mesh->vertexes[vertex_idx];

	ag_model_piece_add_vertex(model,vertex++,bone_idx,ag_model_piece_foot_x_size,y_mid,z_front);
	ag_model_piece_add_vertex(model,vertex++,bone_idx,ag_model_piece_foot_x_size,y_top,z_mid);
	ag_model_piece_add_vertex(model,vertex++,bone_idx,ag_model_piece_foot_x_size,y_top,z_back);
	ag_model_piece_add_vertex(model,vertex++,bone_idx,ag_model_piece_foot_x_size,0,z_back);
	ag_model_piece_add_vertex(model,vertex++,bone_idx,ag_model_piece_foot_x_size,0,z_mid);
	ag_model_piece_add_vertex(model,vertex++,bone_idx,ag_model_piece_foot_x_size,0,z_front);

	ag_model_piece_add_vertex(model,vertex++,bone_idx,-ag_model_piece_foot_x_size,y_mid,z_front);
	ag_model_piece_add_vertex(model,vertex++,bone_idx,-ag_model_piece_foot_x_size,y_top,z_mid);
	ag_model_piece_add_vertex(model,vertex++,bone_idx,-ag_model_piece_foot_x_size,y_top,z_back);
	ag_model_piece_add_vertex(model,vertex++,bone_idx,-ag_model_piece_foot_x_size,0,z_back);
	ag_model_piece_add_vertex(model,vertex++,bone_idx,-ag_model_piece_foot_x_size,0,z_mid);
	ag_model_piece_add_vertex(model,vertex++,bone_idx,-ag_model_piece_foot_x_size,0,z_front);

		// build the polys

	poly=&mesh->polys[poly_idx];

	ag_model_piece_add_quad(model,poly++,vertex_idx,0,1,4,5);
	ag_model_piece_add_quad(model,poly++,vertex_idx,1,2,3,4);

	ag_model_piece_add_quad(model,poly++,vertex_idx,6,7,10,11);
	ag_model_piece_add_quad(model,poly++,vertex_idx,7,8,9,10);

	ag_model_piece_add_quad(model,poly++,vertex_idx,6,0,5,11);
	ag_model_piece_add_quad(model,poly++,vertex_idx,7,1,0,6);
	ag_model_piece_add_quad(model,poly++,vertex_idx,8,2,1,7);
	ag_model_piece_add_quad(model,poly++,vertex_idx,8,2,9,3);
	ag_model_piece_add_quad(model,poly++,vertex_idx,9,3,10,4);
	ag_model_piece_add_quad(model,poly++,vertex_idx,10,4,5,11);
}

