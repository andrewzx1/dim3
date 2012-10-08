/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View OpenRL Models

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

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "interface.h"

#ifdef D3_OPENRL
	#include "ray_interface.h"
#endif

extern map_type				map;
extern server_type			server;
extern camera_type			camera;
extern view_type			view;
extern iface_type			iface;
extern setup_type			setup;
extern network_setup_type	net_setup;
extern file_path_setup_type	file_path_setup;

#ifdef D3_OPENRL

	extern int						view_rl_scene_id;

	extern int view_openrl_create_material(char *sub_path,texture_type *texture,texture_frame_type *frame);

#endif

/* =======================================================

      Blank Patches
      
======================================================= */

#ifndef D3_OPENRL

void view_openrl_model_setup(void) {}

#else

/* =======================================================

      OpenRL Model Mesh Setup
      
======================================================= */

void view_openrl_model_setup(void)
{
	int					n,k,i,t,mesh_id,uv_count;
	float				*uvs,*vt;
	short				*vk,*ray_polys;
	char				sub_path[1024];
	obj_type			*obj;
	model_draw			*draw;
	model_type			*mdl;
	model_mesh_type		*mesh;
	model_poly_type		*poly;
	texture_type		*texture;
	texture_frame_type	*frame;
	
		// model materials

	for (k=0;k!=max_model_list;k++) {

		mdl=server.model_list.models[k];
		if (mdl==NULL) continue;

		sprintf(sub_path,"Models/%s/Textures",mdl->name);

		for (n=0;n!=max_model_texture;n++) {
			texture=&mdl->textures[n];
			
			frame=&texture->frames[0];
			if (frame->name[0]==0x0) continue;
			
			frame->bitmap.rl_material_id=view_openrl_create_material(sub_path,texture,frame);
		}
	}
		
		// object models
	
	for (n=0;n!=max_obj_list;n++) {
	
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;
		
		draw=&obj->draw;
		obj->draw.openrl_mesh_id=-1;
		
			// get the model
			
		if ((draw->model_idx==-1) || (!draw->on)) continue;
	
		mdl=server.model_list.models[draw->model_idx];
		mesh=&mdl->meshes[0];
			
			// add the mesh

		mesh_id=rlSceneMeshAdd(view_rl_scene_id,0);
		if (mesh_id<0) continue;
		
		rlSceneMeshSetHidden(view_rl_scene_id,mesh_id,TRUE);
		
			// we set the UVs and polys at the beginning
			// and only change the vertexes and normals
			// while rendering

			// the UVs

		uvs=(float*)malloc((mesh->npoly*(8*2))*sizeof(float));		// supergumba -- this will work but chews up a lot of memory
		vt=uvs;

		uv_count=0;
		poly=mesh->polys;
	
		for (i=0;i!=mesh->npoly;i++) {
			for (t=0;t!=poly->ptsz;t++) {
				*vt++=poly->gx[t];
				*vt++=poly->gy[t];
			}
			uv_count+=poly->ptsz;
			poly++;
		}
			
		rlSceneMeshSetUV(view_rl_scene_id,mesh_id,RL_MESH_FORMAT_UV_2_FLOAT,uv_count,uvs);
		free(uvs);

			// polygons

		ray_polys=(short*)malloc((mesh->npoly*(2+(4*8)))*sizeof(short));		// supergumba -- this will work but chews up a lot of memory
		vk=ray_polys;

		uv_count=0;
		poly=mesh->polys;
	
		for (i=0;i!=mesh->npoly;i++) {
			*vk++=poly->ptsz;
			*vk++=(short)mdl->textures[poly->txt_idx].frames[0].bitmap.rl_material_id;

			for (t=0;t!=poly->ptsz;t++) {
				*vk++=(short)poly->v[t];	// vertex
				*vk++=uv_count;				// uv, each vertex has unique uv count
				*vk++=(short)poly->v[t];	// normal are parallel to vertexes
				*vk++=(short)poly->v[t];	// tangents are parallel to vertexes
				uv_count++;
			}

			poly++;
		}

		rlSceneMeshSetPoly(view_rl_scene_id,mesh_id,RL_MESH_FORMAT_POLY_SHORT_VERTEX_UV_NORMAL_TANGENT,mesh->npoly,ray_polys);
		free(ray_polys);

			// set the draw's mesh id
			
		draw->openrl_mesh_id=mesh_id;
	}
}

/* =======================================================

      OpenRL Model Mesh Update
      
======================================================= */

void view_openrl_model_update(void)
{
	int					n;
	obj_type			*obj;
	model_draw			*draw;
	model_type			*mdl;
	model_mesh_type		*mesh;

	for (n=0;n!=max_obj_list;n++) {
	
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;
		
		if (obj->hidden) {
			rlSceneMeshSetHidden(view_rl_scene_id,draw->openrl_mesh_id,TRUE);
			continue;
		}

	//	if ((map.camera.mode==cv_fpp) && (obj->idx==camera.obj_idx)) continue;		// supergumba -- we need to get this in scene anyway
		
			// get the openrl mesh
			// and model
			
		draw=&obj->draw;
		if (draw->openrl_mesh_id==-1) continue;
		
		rlSceneMeshSetHidden(view_rl_scene_id,draw->openrl_mesh_id,FALSE);
	
		mdl=server.model_list.models[draw->model_idx];
		mesh=&mdl->meshes[0];
			
			// the UVs and Polys are
			// already set, we just need to
			// update the vertexes and normals

		rlSceneMeshSetVertex(view_rl_scene_id,draw->openrl_mesh_id,RL_MESH_FORMAT_VERTEX_3_FLOAT,mesh->nvertex,draw->setup.mesh_arrays[0].gl_vertex_array);
		rlSceneMeshSetNormal(view_rl_scene_id,draw->openrl_mesh_id,RL_MESH_FORMAT_NORMAL_3_FLOAT,mesh->nvertex,draw->setup.mesh_arrays[0].gl_normal_array);
		rlSceneMeshSetTangent(view_rl_scene_id,draw->openrl_mesh_id,RL_MESH_FORMAT_NORMAL_3_FLOAT,mesh->nvertex,draw->setup.mesh_arrays[0].gl_tangent_array);
	}
}

#endif