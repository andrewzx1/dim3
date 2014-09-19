/****************************** File *********************************

Module: dim3 Auto Generator
Author: Brian Barnes
 Usage: Model Auto Generation Pieces

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

(c) 2000-2014 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3autogenerate.h"
#endif

#define ag_model_piece_hand_y_size			200
#define ag_model_piece_hand_y_size_extra	150

#define ag_model_piece_foot_x_size			100
#define ag_model_piece_foot_x_size_extra	50
#define ag_model_piece_foot_z_size			250
#define ag_model_piece_foot_z_size_extra	250

int 					ag_model_piece_hand_high,
						ag_model_piece_foot_x_sz,ag_model_piece_foot_z_front;

/* =======================================================

      Set Vertex and Polys
      
======================================================= */

void ag_model_piece_add_vertex(model_type *model,model_vertex_type *vertex,int center_bone_idx,int attach_bone_idx,int x_off,int y_off,int z_off)
{
	model_bone_type		*bone;

	bone=&model->bones[center_bone_idx];

	vertex->pnt.x=bone->pnt.x+x_off;
	vertex->pnt.y=bone->pnt.y+y_off;
	vertex->pnt.z=bone->pnt.z+z_off;

		// bone connection

	vertex->major_bone_idx=attach_bone_idx;
	vertex->minor_bone_idx=-1;
	vertex->bone_factor=1.0f;

		// tangent space

	vector_create(&vertex->tangent_space.normal,vertex->pnt.x,vertex->pnt.y,vertex->pnt.z,bone->pnt.x,bone->pnt.y,bone->pnt.z);
}

void ag_model_piece_add_quad(model_type *model,model_poly_type *poly,int v_off,int v1_idx,int v2_idx,int v3_idx,int v4_idx,float gx,float gx_sz,float gy,float gy_sz)
{
	poly->ptsz=4;
	poly->txt_idx=0;

	poly->v[0]=v_off+v1_idx;
	poly->v[1]=v_off+v2_idx;
	poly->v[2]=v_off+v3_idx;
	poly->v[3]=v_off+v4_idx;

	poly->gx[0]=poly->gx[3]=gx;
	poly->gx[1]=poly->gx[2]=gx+gx_sz;
	poly->gy[0]=poly->gy[1]=gy;
	poly->gy[2]=poly->gy[3]=gy+gy_sz;
}

void ag_model_piece_add_trig(model_type *model,model_poly_type *poly,int v_off,int v1_idx,int v2_idx,int v3_idx)
{
	poly->ptsz=3;
	poly->txt_idx=0;

	poly->v[0]=v_off+v1_idx;
	poly->v[1]=v_off+v2_idx;
	poly->v[2]=v_off+v3_idx;

	poly->gx[0]=poly->gx[1]=0.0f;
	poly->gx[2]=0.5f;
	poly->gy[0]=poly->gy[1]=0.0f;
	poly->gy[2]=0.5f;
}

/* =======================================================

      Piece Setups
      
======================================================= */

void ag_model_piece_setup(void)
{
		// setup certain random elements early so
		// they are the same across all times they are
		// used

	ag_model_piece_hand_high=ag_model_piece_hand_y_size+ag_random_int(ag_model_piece_hand_y_size_extra);

	ag_model_piece_foot_x_sz=ag_model_piece_foot_x_size+ag_random_int(ag_model_piece_foot_x_size_extra);
	ag_model_piece_foot_z_front=-(ag_model_piece_foot_z_size+ag_random_int(ag_model_piece_foot_z_size_extra));
}

/* =======================================================

      Head
      
======================================================= */

void ag_model_piece_bone_head(model_type *model,model_mesh_type *mesh,int bone_idx,int limb_radius)
{
	int					vertex_idx,poly_idx,center_bone_idx,
						x_sz,x_h_sz,z_sz,z_h_sz,
						y_top,y_tip,y_neck;
	model_vertex_type	*vertex;
	model_poly_type		*poly;

		// get neck bone

	center_bone_idx=model->bones[bone_idx].parent_idx;
	if (center_bone_idx==-1) return;

		// add the vertexes and polys

	vertex_idx=mesh->nvertex;
	model_mesh_set_vertex_count(model,0,(vertex_idx+18));

	poly_idx=mesh->npoly;
	model_mesh_set_poly_count(model,0,(poly_idx+24));

		// y and z coordinates

	x_sz=150+ag_random_int(250);
	x_h_sz=x_sz>>1;

	z_sz=150+ag_random_int(250);
	z_h_sz=z_sz>>1;

	y_top=350+ag_random_int(350);
	y_tip=y_top+(50+ag_random_int(150));
	y_neck=20;

		// build the vertexes
	
	vertex=&mesh->vertexes[vertex_idx];

	ag_model_piece_add_vertex(model,vertex++,bone_idx,bone_idx,-x_sz,-y_top,z_h_sz);
	ag_model_piece_add_vertex(model,vertex++,bone_idx,bone_idx,-x_h_sz,-y_top,z_sz);
	ag_model_piece_add_vertex(model,vertex++,bone_idx,bone_idx,x_h_sz,-y_top,z_sz);
	ag_model_piece_add_vertex(model,vertex++,bone_idx,bone_idx,x_sz,-y_top,z_h_sz);
	ag_model_piece_add_vertex(model,vertex++,bone_idx,bone_idx,x_sz,-y_top,-z_h_sz);
	ag_model_piece_add_vertex(model,vertex++,bone_idx,bone_idx,x_h_sz,-y_top,-z_sz);
	ag_model_piece_add_vertex(model,vertex++,bone_idx,bone_idx,-x_h_sz,-y_top,-z_sz);
	ag_model_piece_add_vertex(model,vertex++,bone_idx,bone_idx,-x_sz,-y_top,-z_h_sz);

	ag_model_piece_add_vertex(model,vertex++,bone_idx,bone_idx,-x_sz,0,z_h_sz);
	ag_model_piece_add_vertex(model,vertex++,bone_idx,bone_idx,-x_h_sz,0,z_sz);
	ag_model_piece_add_vertex(model,vertex++,bone_idx,bone_idx,x_h_sz,0,z_sz);
	ag_model_piece_add_vertex(model,vertex++,bone_idx,bone_idx,x_sz,0,z_h_sz);
	ag_model_piece_add_vertex(model,vertex++,bone_idx,bone_idx,x_sz,0,-z_h_sz);
	ag_model_piece_add_vertex(model,vertex++,bone_idx,bone_idx,x_h_sz,0,-z_sz);
	ag_model_piece_add_vertex(model,vertex++,bone_idx,bone_idx,-x_h_sz,0,-z_sz);
	ag_model_piece_add_vertex(model,vertex++,bone_idx,bone_idx,-x_sz,0,-z_h_sz);

	ag_model_piece_add_vertex(model,vertex++,bone_idx,bone_idx,0,-y_tip,0);
	ag_model_piece_add_vertex(model,vertex++,bone_idx,bone_idx,0,y_neck,0);

		// build the polys

	poly=&mesh->polys[poly_idx];

	ag_model_piece_add_quad(model,poly++,vertex_idx,0,1,9,8,0.0f,0.5f,0.0f,0.5f);
	ag_model_piece_add_quad(model,poly++,vertex_idx,1,2,10,9,0.0f,0.5f,0.0f,0.5f);
	ag_model_piece_add_quad(model,poly++,vertex_idx,2,3,11,10,0.0f,0.5f,0.0f,0.5f);
	ag_model_piece_add_quad(model,poly++,vertex_idx,3,4,12,11,0.0f,0.5f,0.0f,0.5f);
	ag_model_piece_add_quad(model,poly++,vertex_idx,4,5,13,12,0.5f,0.15f,0.0f,0.5f);
	ag_model_piece_add_quad(model,poly++,vertex_idx,5,6,14,13,0.65f,0.19f,0.0f,0.5f);
	ag_model_piece_add_quad(model,poly++,vertex_idx,6,7,15,14,0.84f,0.15f,0.0f,0.5f);
	ag_model_piece_add_quad(model,poly++,vertex_idx,7,0,8,15,0.0f,0.5f,0.0f,0.5f);

	ag_model_piece_add_trig(model,poly++,vertex_idx,0,1,16);
	ag_model_piece_add_trig(model,poly++,vertex_idx,1,2,16);
	ag_model_piece_add_trig(model,poly++,vertex_idx,2,3,16);
	ag_model_piece_add_trig(model,poly++,vertex_idx,3,4,16);
	ag_model_piece_add_trig(model,poly++,vertex_idx,4,5,16);
	ag_model_piece_add_trig(model,poly++,vertex_idx,5,6,16);
	ag_model_piece_add_trig(model,poly++,vertex_idx,6,7,16);
	ag_model_piece_add_trig(model,poly++,vertex_idx,7,0,16);

	ag_model_piece_add_trig(model,poly++,vertex_idx,8,9,17);
	ag_model_piece_add_trig(model,poly++,vertex_idx,9,10,17);
	ag_model_piece_add_trig(model,poly++,vertex_idx,10,11,17);
	ag_model_piece_add_trig(model,poly++,vertex_idx,11,12,17);
	ag_model_piece_add_trig(model,poly++,vertex_idx,12,13,17);
	ag_model_piece_add_trig(model,poly++,vertex_idx,13,14,17);
	ag_model_piece_add_trig(model,poly++,vertex_idx,14,15,17);
	ag_model_piece_add_trig(model,poly++,vertex_idx,15,0,17);
}

/* =======================================================

      Hand
      
======================================================= */

void ag_model_piece_bone_hand(model_type *model,model_mesh_type *mesh,int bone_idx,int limb_radius)
{
	int					vertex_idx,poly_idx,center_bone_idx,
						y_mid,y_bot;
	model_vertex_type	*vertex;
	model_poly_type		*poly;

		// get wrist bone

	center_bone_idx=model->bones[bone_idx].parent_idx;
	if (center_bone_idx==-1) return;

		// add the vertexes and polys

	vertex_idx=mesh->nvertex;
	model_mesh_set_vertex_count(model,0,(vertex_idx+12));

	poly_idx=mesh->npoly;
	model_mesh_set_poly_count(model,0,(poly_idx+10));

		// y and z coordinates

	y_mid=ag_model_piece_hand_high>>1;
	y_bot=ag_model_piece_hand_high;

		// build the vertexes
	
	vertex=&mesh->vertexes[vertex_idx];

	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,limb_radius,0,-limb_radius);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,limb_radius,0,limb_radius);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,limb_radius,y_mid,(limb_radius+50));
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,limb_radius,y_bot,limb_radius);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,limb_radius,y_bot,-limb_radius);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,limb_radius,y_mid,-(limb_radius+50));

	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-limb_radius,0,-limb_radius);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-limb_radius,0,limb_radius);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-limb_radius,y_mid,(limb_radius+50));
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-limb_radius,y_bot,limb_radius);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-limb_radius,y_bot,-limb_radius);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-limb_radius,y_mid,-(limb_radius+50));

		// build the polys

	poly=&mesh->polys[poly_idx];

	ag_model_piece_add_quad(model,poly++,vertex_idx,0,1,7,6,0.0f,0.5f,0.5f,1.0f);
	ag_model_piece_add_quad(model,poly++,vertex_idx,1,2,8,7,0.0f,0.5f,0.5f,1.0f);
	ag_model_piece_add_quad(model,poly++,vertex_idx,2,3,9,8,0.0f,0.5f,0.5f,1.0f);
	ag_model_piece_add_quad(model,poly++,vertex_idx,3,4,10,9,0.0f,0.5f,0.5f,1.0f);
	ag_model_piece_add_quad(model,poly++,vertex_idx,4,5,11,10,0.0f,0.5f,0.5f,1.0f);
	ag_model_piece_add_quad(model,poly++,vertex_idx,5,0,6,11,0.0f,0.5f,0.5f,1.0f);

	ag_model_piece_add_quad(model,poly++,vertex_idx,0,1,2,5,0.0f,0.5f,0.5f,1.0f);
	ag_model_piece_add_quad(model,poly++,vertex_idx,2,3,4,5,0.0f,0.5f,0.5f,1.0f);
	ag_model_piece_add_quad(model,poly++,vertex_idx,6,7,8,11,0.0f,0.5f,0.5f,1.0f);
	ag_model_piece_add_quad(model,poly++,vertex_idx,8,9,10,11,0.0f,0.5f,0.5f,1.0f);
}

/* =======================================================

      Feet
      
======================================================= */

void ag_model_piece_bone_foot(model_type *model,model_mesh_type *mesh,int bone_idx,int limb_radius)
{
	int					vertex_idx,poly_idx,center_bone_idx,
						z_mid,z_back,y_bot,y_mid,high;
	model_vertex_type	*vertex;
	model_poly_type		*poly;

		// get ankle bone

	center_bone_idx=model->bones[bone_idx].parent_idx;
	if (center_bone_idx==-1) return;

		// add the vertexes and polys

	vertex_idx=mesh->nvertex;
	model_mesh_set_vertex_count(model,0,(vertex_idx+12));

	poly_idx=mesh->npoly;
	model_mesh_set_poly_count(model,0,(poly_idx+10));

		// y and z coordinates

	z_mid=-(limb_radius<<1);
	z_back=limb_radius;

	high=-model->bones[center_bone_idx].pnt.y;

	y_mid=high>>1;
	y_bot=high;

		// build the vertexes
	
	vertex=&mesh->vertexes[vertex_idx];

	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,ag_model_piece_foot_x_sz,y_mid,ag_model_piece_foot_z_front);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,ag_model_piece_foot_x_sz,0,z_mid);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,ag_model_piece_foot_x_sz,0,z_back);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,ag_model_piece_foot_x_sz,y_bot,z_back);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,ag_model_piece_foot_x_sz,y_bot,z_mid);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,ag_model_piece_foot_x_sz,y_bot,ag_model_piece_foot_z_front);

	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-ag_model_piece_foot_x_sz,y_mid,ag_model_piece_foot_z_front);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-ag_model_piece_foot_x_sz,0,z_mid);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-ag_model_piece_foot_x_sz,0,z_back);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-ag_model_piece_foot_x_sz,y_bot,z_back);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-ag_model_piece_foot_x_sz,y_bot,z_mid);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-ag_model_piece_foot_x_sz,y_bot,ag_model_piece_foot_z_front);

		// build the polys

	poly=&mesh->polys[poly_idx];

	ag_model_piece_add_quad(model,poly++,vertex_idx,0,1,4,5,0.5f,0.5f,0.5f,0.5f);
	ag_model_piece_add_quad(model,poly++,vertex_idx,1,2,3,4,0.5f,0.5f,0.5f,0.5f);

	ag_model_piece_add_quad(model,poly++,vertex_idx,6,7,10,11,0.5f,0.5f,0.5f,0.5f);
	ag_model_piece_add_quad(model,poly++,vertex_idx,7,8,9,10,0.5f,0.5f,0.5f,0.5f);

	ag_model_piece_add_quad(model,poly++,vertex_idx,6,0,5,11,0.5f,0.5f,0.5f,0.5f);
	ag_model_piece_add_quad(model,poly++,vertex_idx,7,1,0,6,0.5f,0.5f,0.5f,0.5f);
	ag_model_piece_add_quad(model,poly++,vertex_idx,8,2,1,7,0.5f,0.5f,0.5f,0.5f);
	ag_model_piece_add_quad(model,poly++,vertex_idx,8,2,3,9,0.5f,0.5f,0.5f,0.5f);
	ag_model_piece_add_quad(model,poly++,vertex_idx,9,3,4,10,0.5f,0.5f,0.5f,0.5f);
	ag_model_piece_add_quad(model,poly++,vertex_idx,10,4,5,11,0.5f,0.5f,0.5f,0.5f);
}

