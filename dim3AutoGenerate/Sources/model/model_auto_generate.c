/****************************** File *********************************

Module: dim3 Auto Generator
Author: Brian Barnes
 Usage: Model Auto Generation Main Line

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

#define ag_model_cylinder_side_count		8
#define ag_model_cylinder_limb_radius		100
#define ag_model_cylinder_body_radius_x		300
#define ag_model_cylinder_body_radius_z		150
#define ag_model_cylinder_hip_radius_x		250
#define ag_model_cylinder_hip_radius_z		150
#define ag_model_cylinder_hip_high			300
#define ag_model_cylinder_extra_size		40

/* =======================================================

      Clear Model
      
======================================================= */

void ag_model_clear(model_type *model)
{
	int				n;

		// clear first mesh

	if (model->nmesh==0) {
		model_mesh_add(model);
	}
	else {
		model_mesh_set_vertex_count(model,0,0);
		model_mesh_set_poly_count(model,0,0);
	}

		// clear bones

	/* supergumba -- we need to keep bones and animations
	 in a specific order for now as we are using existing animations, so
	 this code is a temporary hack, replace with the better code below
	 later
 
	while (model->nbone!=0) {
		model_bone_delete(model,0);
	}
	*/

	free(model->bones);
	model->bones=NULL;
	model->nbone=0;

		// clear bone connections

	model->bone_connect.name_bone_idx=-1;

	for (n=0;n!=max_model_light;n++) {
		model->bone_connect.light_bone_idx[n]=-1;
	}

	for (n=0;n!=max_model_halo;n++) {
		model->bone_connect.halo_bone_idx[n]=-1;
	}
}

/* =======================================================

      Model Cylinders Vertex Positions
      
======================================================= */

void ag_model_bone_cylinder_position_x(model_bone_type *bone,int side_idx,float radius,d3pnt *pnt)
{
	float			ang;

		// angle offset

	ang=((2.0f*TRIG_PI)/((float)ag_model_cylinder_side_count))*side_idx;

	pnt->x=bone->pnt.x;
	pnt->y=bone->pnt.y+(int)(radius*sinf(ang));
	pnt->z=bone->pnt.z-(int)(radius*cosf(ang));
}

void ag_model_bone_cylinder_position_y(model_bone_type *bone,int side_idx,float radius_x,float radius_z,d3pnt *pnt)
{
	float			ang;

		// angle offset

	ang=((2.0f*TRIG_PI)/((float)ag_model_cylinder_side_count))*side_idx;

	pnt->x=bone->pnt.x+(int)(radius_x*sinf(ang));
	pnt->y=bone->pnt.y;
	pnt->z=bone->pnt.z-(int)(radius_z*cosf(ang));
}

void ag_model_bone_cylinder_position_z(model_bone_type *bone,int side_idx,float radius,d3pnt *pnt)
{
	float			ang;

		// angle offset

	ang=((2.0f*TRIG_PI)/((float)ag_model_cylinder_side_count))*side_idx;

	pnt->x=bone->pnt.x+(int)(radius*sinf(ang));
	pnt->y=bone->pnt.y-(int)(radius*cosf(ang));
	pnt->z=bone->pnt.z;
}

/* =======================================================

      Primitives around bones
      
======================================================= */

int ag_model_bone_cylinder_vertexes(model_type *model,model_mesh_type *mesh,int bone_idx,int radius_x,int radius_z,int y_off)
{
	int					n,v_idx;
	model_bone_type		*bone;
	model_vertex_type	*vertex;

		// add vertexes

	v_idx=mesh->nvertex;
	model_mesh_set_vertex_count(model,0,(v_idx+ag_model_cylinder_side_count));

		// set cylinder vertexes
		// around bones

	bone=&model->bones[bone_idx];
	vertex=&mesh->vertexes[v_idx];

	for (n=0;n!=ag_model_cylinder_side_count;n++) {
		ag_model_bone_cylinder_position_y(bone,n,(float)radius_x,(float)radius_z,&vertex->pnt);
		vertex->pnt.y+=y_off;

			// bone connection

		vertex->major_bone_idx=bone_idx;
		vertex->minor_bone_idx=-1;
		vertex->bone_factor=1.0f;

			// tangent space

		vector_create(&vertex->tangent_space.normal,vertex->pnt.x,vertex->pnt.y,vertex->pnt.z,bone->pnt.x,bone->pnt.y,bone->pnt.z);

		vertex++;
	}

	return(v_idx);
}

void ag_model_bone_cylinder_polygons(model_type *model,model_mesh_type *mesh,int v1_idx,int v2_idx,bool secondary_texture)
{
	int				n,k,poly_idx;
	float			gx,gx2,gx_add;
	model_poly_type	*poly;

		// add polys

	poly_idx=mesh->npoly;
	model_mesh_set_poly_count(model,0,(poly_idx+ag_model_cylinder_side_count));

		// build polys
		// skin is top-left of texture

	gx=secondary_texture?0.5f:0.0f;
	gx_add=0.5f/(float)ag_model_cylinder_side_count;

	poly=&mesh->polys[poly_idx];

	for (n=0;n!=ag_model_cylinder_side_count;n++) {

		poly->ptsz=4;
		poly->txt_idx=0;

			// the UV

		if ((n+1)==ag_model_cylinder_side_count) {
			gx2=secondary_texture?1.0f:0.5f;
		}
		else {
			gx2=gx+gx_add;
		}

		poly->gx[0]=poly->gx[3]=gx;
		poly->gx[1]=poly->gx[2]=gx2;
		if (secondary_texture) {
			poly->gy[0]=poly->gy[1]=0.5f;
			poly->gy[2]=poly->gy[3]=1.0f;
		}
		else {
			poly->gy[0]=poly->gy[1]=0.0f;
			poly->gy[2]=poly->gy[3]=0.5f;
		}

		gx=gx2;

			// the poly indexes

		k=n+1;
		if (k==ag_model_cylinder_side_count) k=0;

		poly->v[0]=v1_idx+n;
		poly->v[1]=v1_idx+k;
		poly->v[2]=v2_idx+k;
		poly->v[3]=v2_idx+n;

		poly++;
	}
}

/* =======================================================

      Primitives around bone point
      
======================================================= */

void ag_model_bone_point_cube(model_type *model,model_mesh_type *mesh,int bone_idx,d3pnt *sz)
{
	int					n,nvertex,npoly;
	int					x_off[8]={-1,1,1,-1,-1,1,1,-1},
						y_off[8]={-1,-1,1,1,-1,-1,1,1},
						z_off[8]={-1,-1,-1,-1,1,1,1,1};
	bool				has_face;
	model_bone_type		*bone;
	model_vertex_type	*vertex;
	model_poly_type		*poly;

		// add the 8 vertexes
		// and 6 polys

	nvertex=mesh->nvertex;
	model_mesh_set_vertex_count(model,0,(nvertex+8));

	npoly=mesh->npoly;
	model_mesh_set_poly_count(model,0,(npoly+6));

		// need face?

	bone=&model->bones[bone_idx];

	has_face=(strcmp(bone->name,"Head")==0);

		// build the cube vertexes

	vertex=&mesh->vertexes[nvertex];

	for (n=0;n!=8;n++) {

		vertex->pnt.x=(bone->pnt.x+x_off[n]*sz->x);
		vertex->pnt.y=(bone->pnt.y+y_off[n]*sz->y);
		vertex->pnt.z=(bone->pnt.z+z_off[n]*sz->z);

			// bone connection

		vertex->major_bone_idx=bone_idx;
		vertex->minor_bone_idx=-1;
		vertex->bone_factor=1.0f;

			// tangent space

		vector_create(&vertex->tangent_space.normal,vertex->pnt.x,vertex->pnt.y,vertex->pnt.z,bone->pnt.x,bone->pnt.y,bone->pnt.z);

		vertex++;
	}

		// build the cube polys

	poly=&mesh->polys[npoly];

	poly->ptsz=4;
	poly->txt_idx=0;

	poly->v[0]=nvertex;
	poly->v[1]=nvertex+1;
	poly->v[2]=nvertex+2;
	poly->v[3]=nvertex+3;

	if (has_face) {
		poly->gx[0]=poly->gx[3]=1.0f;		// face is top-right of texture
		poly->gx[1]=poly->gx[2]=0.5f;
	}
	else {
		poly->gx[0]=poly->gx[3]=0.0f;
		poly->gx[1]=poly->gx[2]=0.5f;
	}
	poly->gy[0]=poly->gy[1]=0.0f;
	poly->gy[2]=poly->gy[3]=0.5f;

	poly++;

	poly->ptsz=4;
	poly->txt_idx=0;

	poly->v[0]=nvertex+1;
	poly->v[1]=nvertex+2;
	poly->v[2]=nvertex+6;
	poly->v[3]=nvertex+5;

	poly->gx[0]=poly->gx[3]=0.0f;
	poly->gx[1]=poly->gx[2]=0.5f;
	poly->gy[0]=poly->gy[1]=0.0f;
	poly->gy[2]=poly->gy[3]=0.5f;

	poly++;

	poly->ptsz=4;
	poly->txt_idx=0;

	poly->v[0]=nvertex;
	poly->v[1]=nvertex+3;
	poly->v[2]=nvertex+7;
	poly->v[3]=nvertex+4;

	poly->gx[0]=poly->gx[3]=0.0f;
	poly->gx[1]=poly->gx[2]=0.5f;
	poly->gy[0]=poly->gy[1]=0.0f;
	poly->gy[2]=poly->gy[3]=0.5f;

	poly++;

	poly->ptsz=4;
	poly->txt_idx=0;

	poly->v[0]=nvertex+4;
	poly->v[1]=nvertex+5;
	poly->v[2]=nvertex+6;
	poly->v[3]=nvertex+7;

	poly->gx[0]=poly->gx[3]=0.0f;
	poly->gx[1]=poly->gx[2]=0.5f;
	poly->gy[0]=poly->gy[1]=0.0f;
	poly->gy[2]=poly->gy[3]=0.5f;

	poly++;

	poly->ptsz=4;
	poly->txt_idx=0;

	poly->v[0]=nvertex;
	poly->v[1]=nvertex+1;
	poly->v[2]=nvertex+5;
	poly->v[3]=nvertex+4;

	poly->gx[0]=poly->gx[3]=0.0f;
	poly->gx[1]=poly->gx[2]=0.5f;
	poly->gy[0]=poly->gy[1]=0.0f;
	poly->gy[2]=poly->gy[3]=0.5f;

	poly++;

	poly->ptsz=4;
	poly->txt_idx=0;

	poly->v[0]=nvertex+3;
	poly->v[1]=nvertex+2;
	poly->v[2]=nvertex+6;
	poly->v[3]=nvertex+7;

	poly->gx[0]=poly->gx[3]=0.0f;
	poly->gx[1]=poly->gx[2]=0.5f;
	poly->gy[0]=poly->gy[1]=0.0f;
	poly->gy[2]=poly->gy[3]=0.5f;
}

void ag_model_bone_point_cylinder(model_type *model,model_mesh_type *mesh,int bone_idx,int body_v2_idx,d3pnt *sz)
{
}

/* =======================================================

      Build Primitives around bones
      
======================================================= */

void ag_model_build_around_bones(model_type *model)
{
	int				n,parent_idx,
					body_bone_idx,hip_bone_idx,
					v_idx,v2_idx,v3_idx;
	int				bone_vertex_idx[max_model_bone];
	d3pnt			sz;
	model_mesh_type	*mesh;

		// build into first mesh

	mesh=&model->meshes[0];

		// build the cylinder vertexes

	for (n=0;n!=model->nbone;n++) {
		bone_vertex_idx[n]=ag_model_bone_cylinder_vertexes(model,mesh,n,ag_model_cylinder_limb_radius,ag_model_cylinder_limb_radius,0);
	}

		// build the cylinder poly
		// for bones with parents and
		// no special or base bones, or
		// the body bone or any bone connected
		// directly to the hips

	body_bone_idx=-1;
	hip_bone_idx=-1;

	for (n=0;n!=model->nbone;n++) {

		if (model->bones[n].parent_idx==-1) continue;

		if (ag_model_bone_is_body(model,n)) {
			body_bone_idx=n;
			continue;
		}
		if (ag_model_bone_is_hip(model,n)) hip_bone_idx=n;
		if (ag_model_bone_is_connected_to_hips(model,n)) continue;

		if (ag_model_bone_is_special(model,n)) continue;
		if (ag_model_bone_is_special(model,model->bones[n].parent_idx)) continue;

		if (ag_model_bone_is_shallow_y(model,n)) continue;

			// create poly
		
		parent_idx=model->bones[n].parent_idx;
		ag_model_bone_cylinder_polygons(model,mesh,bone_vertex_idx[n],bone_vertex_idx[parent_idx],FALSE);
	}

		// build body bone

	if (body_bone_idx!=-1) {
		parent_idx=model->bones[body_bone_idx].parent_idx;

			// body bone

		v_idx=ag_model_bone_cylinder_vertexes(model,mesh,body_bone_idx,ag_model_cylinder_body_radius_x,ag_model_cylinder_body_radius_z,0);
		v2_idx=ag_model_bone_cylinder_vertexes(model,mesh,parent_idx,ag_model_cylinder_body_radius_x,ag_model_cylinder_body_radius_z,0);
		ag_model_bone_cylinder_polygons(model,mesh,v_idx,v2_idx,TRUE);
	
			// hip bone
			// connects to bottom of body bone

		if (hip_bone_idx!=-1) {
			v3_idx=ag_model_bone_cylinder_vertexes(model,mesh,hip_bone_idx,ag_model_cylinder_hip_radius_x,ag_model_cylinder_hip_radius_z,ag_model_cylinder_hip_high);
			ag_model_bone_cylinder_polygons(model,mesh,v2_idx,v3_idx,TRUE);
		}
	}

		// build cubed bone points

	for (n=0;n!=model->nbone;n++) {
		if (ag_model_bone_is_cubed(model,n,&sz)) ag_model_bone_point_cube(model,mesh,n,&sz);
	}
}

/* =======================================================

      Generate Textures
      
======================================================= */

texture_type* ag_model_create_texture(model_type *model,int idx,char *name,bool has_glow)
{
	int					n;
	texture_type		*texture;

		// close old texture

	model_textures_close_texture(model,idx);

		// start new texture

	texture=&model->textures[idx];
	
	sprintf(texture->material_name,name);
	texture->shader_name[0]=0x0;
	texture->additive=FALSE;
	texture->compress=FALSE;
	
	if (has_glow) {
		texture->glow.rate=2000;
		texture->glow.min=0.1f;
		texture->glow.max=0.3f;
	}

	for (n=0;n!=max_texture_frame;n++) {
		texture->frames[n].name[0]=0x0;
	}
	
	strcpy(texture->frames[0].name,texture->material_name);

	return(texture);
}

void auto_generate_model_create_texture_set(model_type *model,char *base_path)
{
	texture_type		*texture;

	ag_random_seed();

	texture=ag_model_create_texture(model,ag_texture_wall,"Skin",FALSE);
	bitmap_ag_texture_skin(&texture->frames[0],base_path,512);
	model_textures_read_texture(model,0);
}

/* =======================================================

      Generate Model
      
======================================================= */

void auto_generate_model_monster(model_type *model)
{
		// start with a random seed
		// and clear the current model

	ag_random_seed();
	ag_model_clear(model);

		// build the bones

	ag_model_add_bones(model);

		// build model around bones

	ag_model_build_around_bones(model);
	ag_model_add_decorations(model);

		// rebuild tangents
		// and animation settings

	model_recalc_normals(model,TRUE);
	model_calculate_parents(model);
}
