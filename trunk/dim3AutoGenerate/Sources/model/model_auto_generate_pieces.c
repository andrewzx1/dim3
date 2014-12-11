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

void ag_model_piece_add_vertex(model_type *model,model_vertex_type *vertex,int center_bone_idx,int major_bone_idx,int minor_bone_idx,float bone_factor,int x_off,int y_off,int z_off)
{
	model_bone_type		*bone;

	bone=&model->bones[center_bone_idx];

	vertex->pnt.x=bone->pnt.x+x_off;
	vertex->pnt.y=bone->pnt.y+y_off;
	vertex->pnt.z=bone->pnt.z+z_off;

		// bone connection

	vertex->major_bone_idx=major_bone_idx;
	vertex->minor_bone_idx=minor_bone_idx;
	vertex->bone_factor=bone_factor;

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

      Build Poly Objects
      
======================================================= */

void ag_model_piece_complex_cylinder(model_type *model,model_mesh_type *mesh,int bone_idx,int parent_bone_idx,int min_stack,int extra_stack,d3pnt *sz_start,d3pnt *sz_extra,int y_off,float gx_offset,float gy_offset,float cap_gx_offset,float cap_gy_offset,bool force_end_size)
{
	int					n,k,y,v_idx,vc_idx,major_bone_idx,minor_bone_idx,
						vertex_idx,poly_idx,center_bone_idx,
						stack_count,head_high,y_add,
						x_sz,x_h_sz,z_sz,z_h_sz,
						va_idx[4];
	float				bone_factor,ang,
						gx,gy,gx_add,gy_add,uv_x[8],uv_y[8];
	model_vertex_type	*vertex;
	model_poly_type		*poly;

		// get center bone

	center_bone_idx=model->bones[bone_idx].parent_idx;
	if (center_bone_idx==-1) return;

		// how many stacks?

	stack_count=min_stack+ag_random_int(extra_stack);

		// add the vertexes and polys
		// need 1 extra for top row of vertexes

	vertex_idx=mesh->nvertex;
	model_mesh_set_vertex_count(model,0,(vertex_idx+(8*(stack_count+1))));

	poly_idx=mesh->npoly;
	model_mesh_set_poly_count(model,0,(poly_idx+((8*stack_count)+(2*6))));

		// head height

	head_high=sz_start->y+ag_random_int(sz_extra->y);
	y_add=(head_high/stack_count);

	gy_add=0.5f/((float)stack_count);

		// build the vertexes
	
	vertex=&mesh->vertexes[vertex_idx];

	y=y_off-(y_add*stack_count);

	for (n=0;n!=(stack_count+1);n++) {

			// randomize x-z coordinates

		x_sz=sz_start->x;
		z_sz=sz_start->z;

		if (!((force_end_size) && ((n==0) || (n==stack_count)))) {
			x_sz+=ag_random_int(sz_extra->x);
			z_sz+=ag_random_int(sz_extra->z);
		}

		x_h_sz=x_sz>>1;
		z_h_sz=z_sz>>1;

			// minor bone

		if (parent_bone_idx!=-1) {
			if (n==stack_count) {
				major_bone_idx=bone_idx;
				minor_bone_idx=-1;
				bone_factor=1.0f;
			}
			else {
				if (n==0) {
					major_bone_idx=parent_bone_idx;
					minor_bone_idx=-1;
					bone_factor=1.0f;
				}
				else {
					major_bone_idx=bone_idx;
					minor_bone_idx=parent_bone_idx;
					bone_factor=1.0f-(((float)n)/((float)stack_count));
				}
			}
		}
		else {
			major_bone_idx=bone_idx;
			minor_bone_idx=-1;
			bone_factor=1.0f;
		}

			// the vertexes

		ag_model_piece_add_vertex(model,vertex++,bone_idx,major_bone_idx,minor_bone_idx,bone_factor,-x_sz,y,z_h_sz);
		ag_model_piece_add_vertex(model,vertex++,bone_idx,major_bone_idx,minor_bone_idx,bone_factor,-x_h_sz,y,z_sz);
		ag_model_piece_add_vertex(model,vertex++,bone_idx,major_bone_idx,minor_bone_idx,bone_factor,x_h_sz,y,z_sz);
		ag_model_piece_add_vertex(model,vertex++,bone_idx,major_bone_idx,minor_bone_idx,bone_factor,x_sz,y,z_h_sz);
		ag_model_piece_add_vertex(model,vertex++,bone_idx,major_bone_idx,minor_bone_idx,bone_factor,x_sz,y,-z_h_sz);
		ag_model_piece_add_vertex(model,vertex++,bone_idx,major_bone_idx,minor_bone_idx,bone_factor,x_h_sz,y,-z_sz);
		ag_model_piece_add_vertex(model,vertex++,bone_idx,major_bone_idx,minor_bone_idx,bone_factor,-x_h_sz,y,-z_sz);
		ag_model_piece_add_vertex(model,vertex++,bone_idx,major_bone_idx,minor_bone_idx,bone_factor,-x_sz,y,-z_h_sz);

		y+=y_add;
	}

		// build the side polys

	poly=&mesh->polys[poly_idx];

	v_idx=0;

	gx_add=0.5f/8.0f;
	gy=gy_offset;

	for (n=0;n!=stack_count;n++) {

		va_idx[0]=0;
		va_idx[1]=1;
		va_idx[2]=9;
		va_idx[3]=8;

		gx=gx_offset;

		for (k=0;k!=8;k++) {
			ag_model_piece_add_quad(model,poly++,vertex_idx,(v_idx+va_idx[0]),(v_idx+va_idx[1]),(v_idx+va_idx[2]),(v_idx+va_idx[3]),gx,gx_add,gy,gy_add);

			va_idx[0]++;
			va_idx[1]++;
			if (va_idx[1]==8) va_idx[1]=0;
			va_idx[2]++;
			if (va_idx[2]==16) va_idx[2]=8;
			va_idx[3]++;

			gx+=gx_add;
		}

		v_idx+=8;
		gy+=gy_add;
	}

		// top and bottom UVs

	for (n=0;n!=8;n++) {
		ang=(TRIG_PI/8.0f)*(float)n;
		uv_x[n]=cap_gx_offset+fabsf(0.5f*sinf(ang));
		uv_y[n]=cap_gy_offset+fabsf(0.5f*cosf(ang));
	}

		// build the top and bottom cap polys

	v_idx=vc_idx=vertex_idx;

	for (n=0;n!=6;n++) {
		poly->ptsz=3;
		poly->txt_idx=0;

		poly->v[0]=vc_idx;
		poly->v[1]=v_idx+1;
		poly->v[2]=v_idx+2;

		poly->gx[0]=uv_x[0];
		poly->gy[0]=uv_y[0];
		poly->gx[1]=uv_x[n+1];
		poly->gy[1]=uv_y[n+1];
		poly->gx[2]=uv_x[n+2];
		poly->gy[2]=uv_y[n+2];

		poly++;
		v_idx++;
	}

	v_idx=vc_idx=vertex_idx+(stack_count*8);

	for (n=0;n!=6;n++) {
		poly->ptsz=3;
		poly->txt_idx=0;

		poly->v[0]=vc_idx;
		poly->v[1]=v_idx+1;
		poly->v[2]=v_idx+2;

		poly->gx[0]=uv_x[0];
		poly->gy[0]=uv_y[0];
		poly->gx[1]=uv_x[n+1];
		poly->gy[1]=uv_y[n+1];
		poly->gx[2]=uv_x[n+2];
		poly->gy[2]=uv_y[n+2];

		poly++;
		v_idx++;
	}
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
	d3pnt			sz_start,sz_extra;

	sz_start.x=150;
	sz_extra.x=250;

	sz_start.y=350;
	sz_extra.y=350;

	sz_start.z=150;
	sz_extra.z=250;

	ag_model_piece_complex_cylinder(model,mesh,bone_idx,-1,2,3,&sz_start,&sz_extra,0,0.5f,0.0f,0.0f,0.0f,FALSE);
}

/* =======================================================

      Head
      
======================================================= */

extern void ag_model_piece_bone_body(model_type *model,model_mesh_type *mesh,int body_bone_idx,int hip_bone_idx,int limb_radius)
{
	int					n,x,y,y_off,ty,by;
	d3pnt				min,max,sz_start,sz_extra;
	model_bone_type		*bone;

		// find body size by bones

	min.x=min.y=1000000;
	max.x=max.y=0;

	ty=by=0;

	bone=model->bones;

	for (n=0;n!=model->nbone;n++) {
		if (strcmp(bone->name,"Torso")==0) {
			y=-bone->pnt.y;
			if (y<min.y) min.y=y;
		}
		if (strcmp(bone->name,"Left Shoulder")==0) {
			x=-bone->pnt.x;
			y=-bone->pnt.y;
			if (y<min.y) min.y=y;
			if (x<min.x) min.x=x;
		}
		if (strcmp(bone->name,"Right Shoulder")==0) {
			x=-bone->pnt.x;
			y=-bone->pnt.y;
			if (y<min.y) min.y=y;
			if (x>max.x) max.x=x;
		}
		if (strcmp(bone->name,"Hips")==0) {
			y=ty=-bone->pnt.y;
			if (y>max.y) max.y=y;
		}
		if (strcmp(bone->name,"Left Hip")==0) {
			x=-bone->pnt.x;
			y=by=-bone->pnt.y;
			if (y>max.y) max.y=y;
			if (x<min.x) min.x=x;
		}
		if (strcmp(bone->name,"Right Hip")==0) {
			x=-bone->pnt.x;
			y=by=-bone->pnt.y;
			if (y>max.y) max.y=y;
			if (x>max.x) max.x=x;
		}
		bone++;
	}

	y_off=abs(by-ty);

	sz_start.x=(abs(max.x-min.x)>>1)-100;
	sz_extra.x=100;

	sz_start.y=abs(max.y-min.y)+y_off;
	sz_extra.y=50;

	sz_start.z=limb_radius;
	sz_extra.z=limb_radius;

	ag_model_piece_complex_cylinder(model,mesh,hip_bone_idx,-1,3,4,&sz_start,&sz_extra,y_off,0.5f,0.5f,0.5f,0.5f,FALSE);
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

	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-1,1.0f,limb_radius,0,-limb_radius);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-1,1.0f,limb_radius,0,limb_radius);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-1,1.0f,limb_radius,y_mid,(limb_radius+50));
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-1,1.0f,limb_radius,y_bot,limb_radius);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-1,1.0f,limb_radius,y_bot,-limb_radius);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-1,1.0f,limb_radius,y_mid,-(limb_radius+50));

	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-1,1.0f,-limb_radius,0,-limb_radius);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-1,1.0f,-limb_radius,0,limb_radius);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-1,1.0f,-limb_radius,y_mid,(limb_radius+50));
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-1,1.0f,-limb_radius,y_bot,limb_radius);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-1,1.0f,-limb_radius,y_bot,-limb_radius);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-1,1.0f,-limb_radius,y_mid,-(limb_radius+50));

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

	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-1,1.0f,ag_model_piece_foot_x_sz,y_mid,ag_model_piece_foot_z_front);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-1,1.0f,ag_model_piece_foot_x_sz,0,z_mid);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-1,1.0f,ag_model_piece_foot_x_sz,0,z_back);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-1,1.0f,ag_model_piece_foot_x_sz,y_bot,z_back);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-1,1.0f,ag_model_piece_foot_x_sz,y_bot,z_mid);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-1,1.0f,ag_model_piece_foot_x_sz,y_bot,ag_model_piece_foot_z_front);

	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-1,1.0f,-ag_model_piece_foot_x_sz,y_mid,ag_model_piece_foot_z_front);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-1,1.0f,-ag_model_piece_foot_x_sz,0,z_mid);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-1,1.0f,-ag_model_piece_foot_x_sz,0,z_back);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-1,1.0f,-ag_model_piece_foot_x_sz,y_bot,z_back);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-1,1.0f,-ag_model_piece_foot_x_sz,y_bot,z_mid);
	ag_model_piece_add_vertex(model,vertex++,center_bone_idx,bone_idx,-1,1.0f,-ag_model_piece_foot_x_sz,y_bot,ag_model_piece_foot_z_front);

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

