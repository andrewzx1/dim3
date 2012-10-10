/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View OpenRL Effects

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
#include "objects.h"

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

	extern int						view_rl_scene_id,
									view_rl_purple_material_id;

	extern int view_openrl_create_material(char *sub_path,texture_type *texture,texture_frame_type *frame);

#endif

/* =======================================================

      Blank Patches
      
======================================================= */

#ifndef D3_OPENRL

	void view_openrl_effect_mesh_setup(effect_type *effect) {}
	void view_openrl_effect_mesh_close(effect_type *effect) {}

#else

/* =======================================================

      OpenRL Effect Meshes
      
======================================================= */

void view_openrl_effect_mesh_setup(effect_type *effect)
{
	effect->openrl_mesh_id=rlSceneMeshAdd(view_rl_scene_id,(RL_MESH_FLAG_NON_LIGHT_BLOCKING|RL_MESH_FLAG_HIGHLIGHT));
}

void view_openrl_effect_mesh_close(effect_type *effect)
{
	if (effect->openrl_mesh_id!=-1) rlSceneMeshDelete(view_rl_scene_id,effect->openrl_mesh_id);
}

void view_openrl_effect_mesh_update(void)
{
	int					n,k;
	short				polys[10],*vk;
	float				vs[12],uvs[8];
	effect_type			*effect;

	for (n=0;n!=max_effect_list;n++) {
		effect=server.effect_list.effects[n];
		if (effect==NULL) continue;

		if (!effect->on) continue;
		if (effect->effecttype!=ef_particle) continue;
		if (effect->openrl_mesh_id==-1) continue;

			// build the particles

		vs[0]=vs[9]=(float)(effect->pnt.x-100);
		vs[3]=vs[6]=(float)(effect->pnt.x+100);

		vs[1]=vs[4]=(float)(effect->pnt.y-100);
		vs[7]=vs[10]=(float)(effect->pnt.y+100);

		vs[2]=vs[5]=vs[8]=vs[11]=(float)effect->pnt.z;

		rlSceneMeshSetVertex(view_rl_scene_id,effect->openrl_mesh_id,RL_MESH_FORMAT_VERTEX_3_FLOAT,4,vs);

		uvs[0]=uvs[1]=uvs[3]=uvs[6]=0.0;
		uvs[2]=uvs[4]=uvs[5]=uvs[7]=1.0f;
		rlSceneMeshSetUV(view_rl_scene_id,effect->openrl_mesh_id,RL_MESH_FORMAT_UV_2_FLOAT,4,uvs);

			// there's no normals or tangents
			// as these are all highlighted

		vk=polys;

		*vk++=4;
		*vk++=view_rl_purple_material_id;

		for (k=0;k!=4;k++) {
			*vk++=k;
			*vk++=k;
		}
		
		rlSceneMeshSetPoly(view_rl_scene_id,effect->openrl_mesh_id,RL_MESH_FORMAT_POLY_SHORT_VERTEX_UV,1,polys);
	}
}

#endif