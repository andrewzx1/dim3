/****************************** File *********************************

Module: dim3 Auto Generator
Author: Brian Barnes
 Usage: Model Auto Generation Main Line

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

#define ag_model_cylinder_side_count		8
#define ag_model_cylinder_radius			100

/* =======================================================

      Clear Model
      
======================================================= */

void ag_model_clear(model_type *model)
{
		// clear first mesh

	if (model->nmesh==0) {
		model_mesh_add(model);
	}
	else {
		model_mesh_set_vertex_count(model,0,0);
		model_mesh_set_poly_count(model,0,0);
	}
}

/* =======================================================

      Bone Stats
      
======================================================= */

bool ag_model_is_special_bone(model_type *model,int bone_idx)
{
	int				n;

		// base bone

	if ((model->bones[bone_idx].pnt.x==0) && (model->bones[bone_idx].pnt.y==0) && (model->bones[bone_idx].pnt.z==0)) return(TRUE);

		// connector bones

	if (bone_idx==model->bone_connect.name_bone_idx) return(TRUE);

	for (n=0;n!=max_model_light;n++) {
		if (bone_idx==model->bone_connect.light_bone_idx[n]) return(TRUE);
	}

	for (n=0;n!=max_model_halo;n++) {
		if (bone_idx==model->bone_connect.halo_bone_idx[n]) return(TRUE);
	}

	return(FALSE);
}

bool ag_model_is_boxed_bone(model_type *model,int bone_idx)
{
	if (strstr(model->bones[bone_idx].name,"Head")!=NULL) return(TRUE);
	if (strstr(model->bones[bone_idx].name,"Hand")!=NULL) return(TRUE);
	if (strstr(model->bones[bone_idx].name,"Foor")!=NULL) return(TRUE);
	
	return(FALSE);
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

void ag_model_bone_cylinder_position_y(model_bone_type *bone,int side_idx,float radius,d3pnt *pnt)
{
	float			ang;

		// angle offset

	ang=((2.0f*TRIG_PI)/((float)ag_model_cylinder_side_count))*side_idx;

	pnt->x=bone->pnt.x+(int)(radius*sinf(ang));
	pnt->y=bone->pnt.y;
	pnt->z=bone->pnt.z-(int)(radius*cosf(ang));
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

      Model Cylinders
      
======================================================= */

void ag_model_bone_cylinder_vertexes(model_type *model,model_mesh_type *mesh,int bone_idx)
{
	int					n;
	model_bone_type		*bone;
	model_vertex_type	*vertex;

		// build cylinder vertexes
		// around bones

	bone=&model->bones[bone_idx];
	vertex=&mesh->vertexes[bone_idx*ag_model_cylinder_side_count];

	for (n=0;n!=ag_model_cylinder_side_count;n++) {
		ag_model_bone_cylinder_position_y(bone,n,ag_model_cylinder_radius,&vertex->pnt);

			// bone connection

		vertex->major_bone_idx=bone_idx;
		vertex->minor_bone_idx=-1;
		vertex->bone_factor=1.0f;

			// tangent space

		vector_create(&vertex->tangent_space.normal,vertex->pnt.x,vertex->pnt.y,vertex->pnt.z,bone->pnt.x,bone->pnt.y,bone->pnt.z);

		vertex->tangent_space.tangent.x=0.0f;		// supergumba -- need to properly calc this
		vertex->tangent_space.tangent.y=1.0f;
		vertex->tangent_space.tangent.z=0.0f;

		vertex++;
	}
}

void ag_model_bone_cylinder_polygons(model_type *model,model_mesh_type *mesh,int bone_idx,int poly_idx)
{
	int				n,k,v1_idx,v2_idx;
	float			gx,gx2,gx_add;
	model_poly_type	*poly;

		// vertex start

	v1_idx=bone_idx*ag_model_cylinder_side_count;
	v2_idx=model->bones[bone_idx].parent_idx*ag_model_cylinder_side_count;

		// build polys

	gx=0.0f;
	gx_add=1.0f/(float)ag_model_cylinder_side_count;

	poly=&mesh->polys[poly_idx];

	for (n=0;n!=ag_model_cylinder_side_count;n++) {

		poly->ptsz=4;
		poly->txt_idx=0;

			// the UV

		if ((n+1)==ag_model_cylinder_side_count) {
			gx2=1.0f;
		}
		else {
			gx2=gx+gx_add;
		}

		poly->gx[0]=poly->gx[3]=gx;
		poly->gx[1]=poly->gx[2]=gx2;
		poly->gy[0]=poly->gy[1]=0.0f;
		poly->gy[2]=poly->gy[3]=1.0f;

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

void ag_model_bone_cube(model_type *model,model_mesh_type *mesh,int bone_idx)
{
	int					n,nvertex,npoly;
	int					x_off[8]={-200,200,200,-200,-200,200,200,-200},
						y_off[8]={-200,-200,200,200,-200,-200,200,200},
						z_off[8]={-200,-200,-200,-200,200,200,200,200};
	model_bone_type		*bone;
	model_vertex_type	*vertex;
	model_poly_type		*poly;

		// add the 8 vertexes
		// and 6 polys

	nvertex=mesh->nvertex;
	model_mesh_set_vertex_count(model,0,(nvertex+8));

	npoly=mesh->npoly;
	model_mesh_set_poly_count(model,0,(npoly+6));

		// build the cube vertexes

	vertex=&mesh->vertexes[nvertex];
	bone=&model->bones[bone_idx];

	for (n=0;n!=8;n++) {

		vertex->pnt.x=bone->pnt.x+x_off[n];
		vertex->pnt.y=bone->pnt.y+y_off[n];
		vertex->pnt.z=bone->pnt.z+z_off[n];

			// bone connection

		vertex->major_bone_idx=bone_idx;
		vertex->minor_bone_idx=-1;
		vertex->bone_factor=1.0f;

			// tangent space

		vector_create(&vertex->tangent_space.normal,vertex->pnt.x,vertex->pnt.y,vertex->pnt.z,bone->pnt.x,bone->pnt.y,bone->pnt.z);

		vertex->tangent_space.tangent.x=0.0f;		// supergumba -- need to properly calc this
		vertex->tangent_space.tangent.y=1.0f;
		vertex->tangent_space.tangent.z=0.0f;

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

	poly->gx[0]=poly->gx[3]=0.0f;
	poly->gx[1]=poly->gx[2]=1.0f;
	poly->gy[0]=poly->gy[1]=0.0f;
	poly->gy[2]=poly->gy[3]=1.0f;

	poly++;

	poly->ptsz=4;
	poly->txt_idx=0;

	poly->v[0]=nvertex+1;
	poly->v[1]=nvertex+2;
	poly->v[2]=nvertex+6;
	poly->v[3]=nvertex+5;

	poly->gx[0]=poly->gx[3]=0.0f;
	poly->gx[1]=poly->gx[2]=1.0f;
	poly->gy[0]=poly->gy[1]=0.0f;
	poly->gy[2]=poly->gy[3]=1.0f;

	poly++;

	poly->ptsz=4;
	poly->txt_idx=0;

	poly->v[0]=nvertex;
	poly->v[1]=nvertex+3;
	poly->v[2]=nvertex+7;
	poly->v[3]=nvertex+4;

	poly->gx[0]=poly->gx[3]=0.0f;
	poly->gx[1]=poly->gx[2]=1.0f;
	poly->gy[0]=poly->gy[1]=0.0f;
	poly->gy[2]=poly->gy[3]=1.0f;

	poly++;

	poly->ptsz=4;
	poly->txt_idx=0;

	poly->v[0]=nvertex+4;
	poly->v[1]=nvertex+5;
	poly->v[2]=nvertex+6;
	poly->v[3]=nvertex+7;

	poly->gx[0]=poly->gx[3]=0.0f;
	poly->gx[1]=poly->gx[2]=1.0f;
	poly->gy[0]=poly->gy[1]=0.0f;
	poly->gy[2]=poly->gy[3]=1.0f;

	poly++;

	poly->ptsz=4;
	poly->txt_idx=0;

	poly->v[0]=nvertex;
	poly->v[1]=nvertex+1;
	poly->v[2]=nvertex+5;
	poly->v[3]=nvertex+4;

	poly->gx[0]=poly->gx[3]=0.0f;
	poly->gx[1]=poly->gx[2]=1.0f;
	poly->gy[0]=poly->gy[1]=0.0f;
	poly->gy[2]=poly->gy[3]=1.0f;

	poly++;

	poly->ptsz=4;
	poly->txt_idx=0;

	poly->v[0]=nvertex+3;
	poly->v[1]=nvertex+2;
	poly->v[2]=nvertex+6;
	poly->v[3]=nvertex+7;

	poly->gx[0]=poly->gx[3]=0.0f;
	poly->gx[1]=poly->gx[2]=1.0f;
	poly->gy[0]=poly->gy[1]=0.0f;
	poly->gy[2]=poly->gy[3]=1.0f;
}

void ag_model_build_around_bones(model_type *model)
{
	int			n,nvertex,npoly,poly_idx;

		// build the cylinder vertexes

	nvertex=ag_model_cylinder_side_count*model->nbone;
	model_mesh_set_vertex_count(model,0,nvertex);

	for (n=0;n!=model->nbone;n++) {
		ag_model_bone_cylinder_vertexes(model,&model->meshes[0],n);
	}

		// build the cylinder poly
		// for bones with parents and
		// no special or base bones

	npoly=0;

	for (n=0;n!=model->nbone;n++) {

		if (model->bones[n].parent_idx==-1) continue;
		if (ag_model_is_special_bone(model,n)) continue;
		if (ag_model_is_special_bone(model,model->bones[n].parent_idx)) continue;

			// add polys

		poly_idx=npoly;
		npoly+=ag_model_cylinder_side_count;
		model_mesh_set_poly_count(model,0,npoly);

			// create poly
		
		ag_model_bone_cylinder_polygons(model,&model->meshes[0],n,poly_idx);
	}

		// build special boxed bones

	for (n=0;n!=model->nbone;n++) {
		if (ag_model_is_boxed_bone(model,n)) ag_model_bone_cube(model,&model->meshes[0],n);
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

	ag_model_build_around_bones(model);
}
