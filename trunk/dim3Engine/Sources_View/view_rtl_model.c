/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View dim3RTL Models

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
#include "ray_interface.h"

extern map_type				map;
extern server_type			server;
extern camera_type			camera;
extern view_type			view;
extern iface_type			iface;
extern setup_type			setup;
extern network_setup_type	net_setup;
extern file_path_setup_type	file_path_setup;

extern int					view_rtl_scene_id;

extern int view_dim3rtl_create_material_from_texture(char *sub_path,texture_type *texture,texture_frame_type *frame,int alpha_type);
extern int gl_light_get_intensity(int tick,int light_type,int intensity);

/* =======================================================

      dim3RTL Single Model Mesh
      
======================================================= */

void view_dim3rtl_model_setup_single_model(model_draw *draw,bool hidden,bool no_ray_trace_block,bool no_bounce_trace_block,bool no_light_trace_block)
{
	int					n,k,i,mesh_id,overlay_id,uv_count;
	unsigned long		flags,mesh_flags;
	float				*uv;
	short				*vk,*ray_polys;
	rtl2DPoint			p_pnt;
	model_type			*mdl;
	model_mesh_type		*mesh;
	model_poly_type		*poly;
	
		// clear dim3rtl ids

	for (n=0;n!=max_model_mesh;n++) {
		draw->meshes[n].rtl_mesh_id=-1;
	}
	
	for (n=0;n!=max_model_light;n++) {
		draw->lights[n].rtl_light_id=-1;
	}

	for (n=0;n!=max_model_halo;n++) {
		draw->halos[n].rtl_overlay_id=-1;
	}

		// get the model
		
	if ((draw->model_idx==-1) || (!draw->on)) return;

	mdl=server.model_list.models[draw->model_idx];

		// global mesh flags

	flags=0;
	if (no_ray_trace_block) flags|=RL_MESH_FLAG_NON_RAY_TRACE_BLOCKING;
	if (no_bounce_trace_block) flags|=RL_MESH_FLAG_NON_BOUNCE_TRACE_BLOCKING;
	if (no_light_trace_block) flags|=RL_MESH_FLAG_NON_LIGHT_TRACE_BLOCKING;

		// run through the model meshes

	for (i=0;i!=mdl->nmesh;i++) {
		mesh=&mdl->meshes[i];
			
			// add the mesh
			
		mesh_flags=flags;
		if (mesh->rt_non_light_blocking) mesh_flags|=RL_MESH_FLAG_NON_LIGHT_TRACE_BLOCKING;

		mesh_id=rtlSceneMeshAdd(view_rtl_scene_id,mesh_flags);
		if (mesh_id<0) return;
		
		if ((hidden) || ((draw->render_mesh_mask&(0x1<<n))==0)) rtlSceneMeshSetHidden(view_rtl_scene_id,mesh_id,TRUE);
		
			// we set the UVs and polys at the beginning
			// and only change the vertexes and normals
			// while rendering

			// the UVs

		uv_count=0;
		poly=mesh->polys;

		for (n=0;n!=mesh->npoly;n++) {
			uv_count+=poly->ptsz;
			poly++;
		}

		rtlSceneMeshSetUV(view_rtl_scene_id,mesh_id,RL_MESH_FORMAT_UV_2_FLOAT,uv_count,NULL);
		rtlSceneMeshMapUVPointer(view_rtl_scene_id,mesh_id,(void**)&uv);

		poly=mesh->polys;

		for (n=0;n!=mesh->npoly;n++) {
			for (k=0;k!=poly->ptsz;k++) {
				*uv++=poly->gx[k];
				*uv++=poly->gy[k];
			}
			poly++;
		}

		rtlSceneMeshUnMapUVPointer(view_rtl_scene_id,mesh_id);

			// polygons

		ray_polys=(short*)malloc((mesh->npoly*(2+(4*8)))*sizeof(short));		// supergumba -- this will work but chews up a lot of memory
		vk=ray_polys;

		uv_count=0;
		poly=mesh->polys;

		for (n=0;n!=mesh->npoly;n++) {
			*vk++=poly->ptsz;
			*vk++=(short)mdl->textures[poly->txt_idx].frames[0].bitmap.rl_material_id;

			for (k=0;k!=poly->ptsz;k++) {
				*vk++=(short)poly->v[k];	// vertex
				*vk++=uv_count;				// uv, each vertex has unique uv count
				*vk++=(short)poly->v[k];	// normal are parallel to vertexes
				*vk++=(short)poly->v[k];	// tangents are parallel to vertexes
				uv_count++;
			}

			poly++;
		}

		rtlSceneMeshSetPoly(view_rtl_scene_id,mesh_id,RL_MESH_FORMAT_POLY_SHORT_VERTEX_UV_NORMAL_TANGENT,mesh->npoly,ray_polys);
		free(ray_polys);

			// set the draw's mesh id
			
		draw->meshes[i].rtl_mesh_id=mesh_id;
	}

		// lights are dynmaic, so no setup
	
		// setup any halo overlays

	for (n=0;n!=max_model_halo;n++) {
		if ((!draw->halos[n].on) || (draw->halos[n].idx==-1)) continue;

		overlay_id=rtlSceneOverlayAdd(view_rtl_scene_id,iface.halo_list.halos[draw->halos[n].idx].rtl_material_id,0);
		if (overlay_id<0) return;

		p_pnt.x=0;
		p_pnt.y=0;

		rtlSceneOverlaySetPosition(view_rtl_scene_id,overlay_id,&p_pnt);
		rtlSceneOverlaySetSize(view_rtl_scene_id,overlay_id,&p_pnt);
		rtlSceneOverlaySetQuadCount(view_rtl_scene_id,overlay_id,1);
		rtlSceneOverlaySetQuadPosition(view_rtl_scene_id,overlay_id,0,&p_pnt);
		rtlSceneOverlaySetQuadSize(view_rtl_scene_id,overlay_id,0,&p_pnt);

		rtlSceneOverlaySetHidden(view_rtl_scene_id,overlay_id,TRUE);

		draw->halos[n].rtl_overlay_id=overlay_id;
	}
}

void view_dim3rtl_model_update_single_model(model_draw *draw,bool hidden)
{
	int					n,mesh_id,tick,intensity,
						pixel_sz,pixel_off;
	float				alpha;
	d3pnt				pnt;
	model_type			*mdl;
	model_mesh_type		*mesh;
	model_draw_light	*lit;
	model_draw_halo		*halo;
	rtlPoint			lit_pnt,halo_pnt;
	rtl2DPoint			p_pnt,s_pnt;
	rtlColor			lit_col,halo_col;

		// get model

	if (draw->model_idx==-1) return;
	mdl=server.model_list.models[draw->model_idx];

		// update the meshes

	for (n=0;n!=mdl->nmesh;n++) {
		mesh_id=draw->meshes[n].rtl_mesh_id;
		if (mesh_id==-1) continue;
	
			// hidden state

		if ((hidden) || ((draw->render_mesh_mask&(0x1<<n))==0)) {
			rtlSceneMeshSetHidden(view_rtl_scene_id,mesh_id,TRUE);
			continue;
		}
			
		rtlSceneMeshSetHidden(view_rtl_scene_id,mesh_id,FALSE);
			
			// the UVs and Polys are
			// already set, we just need to
			// update the vertexes, normals, and tangents

		mesh=&mdl->meshes[n];

		rtlSceneMeshSetVertex(view_rtl_scene_id,mesh_id,RL_MESH_FORMAT_VERTEX_3_FLOAT,mesh->nvertex,draw->setup.mesh_arrays[n].gl_vertex_array);
		rtlSceneMeshSetNormal(view_rtl_scene_id,mesh_id,RL_MESH_FORMAT_NORMAL_3_FLOAT,mesh->nvertex,draw->setup.mesh_arrays[n].gl_normal_array);
		rtlSceneMeshSetTangent(view_rtl_scene_id,mesh_id,RL_MESH_FORMAT_TANGENT_3_FLOAT,mesh->nvertex,draw->setup.mesh_arrays[n].gl_tangent_array);
	}
	
		// update the lights
		
	tick=game_time_get();
		
	for (n=0;n!=max_model_light;n++) {
	
		lit=&draw->lights[n];
		
			// if light is off, delete it if
			// it exists in scene
			
		if (!lit->on) {
			if (lit->rtl_light_id!=-1) {
				rtlSceneLightDelete(view_rtl_scene_id,lit->rtl_light_id);
				lit->rtl_light_id=-1;
			}
			continue;
		}
		
			// otherwise we need to add it
			
		if (lit->rtl_light_id==-1) {
			lit->rtl_light_id=rtlSceneLightAdd(view_rtl_scene_id);
		
			lit_col.r=lit->col.r;
			lit_col.g=lit->col.g;
			lit_col.b=lit->col.b;
			rtlSceneLightSetColor(view_rtl_scene_id,lit->rtl_light_id,&lit_col);
		}
		
			// change setup
		
		memmove(&pnt,&draw->pnt,sizeof(d3pnt));
		model_get_light_position(mdl,&draw->setup,n,&pnt);

		lit_pnt.x=(float)pnt.x;
		lit_pnt.y=(float)pnt.y;
		lit_pnt.z=(float)pnt.z;
		rtlSceneLightSetPosition(view_rtl_scene_id,lit->rtl_light_id,&lit_pnt);
		
		intensity=gl_light_get_intensity(tick,lit->type,lit->intensity);
		rtlSceneLightSetIntensity(view_rtl_scene_id,lit->rtl_light_id,(float)intensity,lit->exponent);
	}

		// update the halos

	for (n=0;n!=max_model_halo;n++) {
		if ((!draw->halos[n].on) || (draw->halos[n].rtl_overlay_id==-1)) continue;
	
		halo=&draw->halos[n];

			// get position and cull

		pnt.x=draw->pnt.x;
		pnt.y=draw->pnt.y;
		pnt.z=draw->pnt.z;

		model_get_halo_position(mdl,&draw->setup,n,&pnt);

		if (!halo_draw_setup_cull(&iface.halo_list.halos[halo->idx],draw->connect.obj_idx,&pnt,&pixel_sz,&alpha)) {
			rtlSceneOverlaySetHidden(view_rtl_scene_id,halo->rtl_overlay_id,TRUE);
			continue;
		}

			// project and check if
			// on screen

		halo_pnt.x=(float)pnt.x;
		halo_pnt.y=(float)pnt.y;
		halo_pnt.z=(float)pnt.z;

		if (rtlSceneEyeTranslatePoint(view_rtl_scene_id,&halo_pnt,&p_pnt)==RL_ERROR_POINT_BEHIND_EYE) {
			rtlSceneOverlaySetHidden(view_rtl_scene_id,halo->rtl_overlay_id,TRUE);
			continue;
		}

		if (((p_pnt.x+pixel_sz)<0) || ((p_pnt.y+pixel_sz)<0) || ((p_pnt.x-pixel_sz)>=view.screen.x_sz) || ((p_pnt.y-pixel_sz)>=view.screen.y_sz)) {
			rtlSceneOverlaySetHidden(view_rtl_scene_id,halo->rtl_overlay_id,TRUE);
			continue;
		}

			// setup the overlay

		rtlSceneOverlaySetHidden(view_rtl_scene_id,halo->rtl_overlay_id,FALSE);

		pixel_off=pixel_sz>>1;
		p_pnt.x-=pixel_off;
		p_pnt.y-=pixel_off;
		s_pnt.x=s_pnt.y=pixel_sz;

		halo_col.r=halo_col.g=halo_col.b=1.0f;
		halo_col.a=alpha;

		rtlSceneOverlaySetPosition(view_rtl_scene_id,halo->rtl_overlay_id,&p_pnt);
		rtlSceneOverlaySetSize(view_rtl_scene_id,halo->rtl_overlay_id,&s_pnt);

		rtlSceneOverlaySetQuadSize(view_rtl_scene_id,halo->rtl_overlay_id,0,&s_pnt);
		rtlSceneOverlaySetQuadColor(view_rtl_scene_id,halo->rtl_overlay_id,0,&halo_col);
	}
}

void view_dim3rtl_model_close_single_model(model_draw *draw)
{
	int				n;
	model_type		*mdl;

		// get model

	if (draw->model_idx==-1) return;
	mdl=server.model_list.models[draw->model_idx];

		// delete the meshes

	for (n=0;n!=mdl->nmesh;n++) {
		if (draw->meshes[n].rtl_mesh_id!=-1) rtlSceneMeshDelete(view_rtl_scene_id,draw->meshes[n].rtl_mesh_id);
	}
	
		// delete the lights
		
	for (n=0;n!=max_model_light;n++) {
		if (draw->lights[n].rtl_light_id!=-1) rtlSceneLightDelete(view_rtl_scene_id,draw->lights[n].rtl_light_id);
	}

		// delete the halos
		
	for (n=0;n!=max_model_halo;n++) {
		if (draw->halos[n].rtl_overlay_id!=-1) rtlSceneOverlayDelete(view_rtl_scene_id,draw->halos[n].rtl_overlay_id);
	}
}

/* =======================================================

      dim3RTL Map Models
      
======================================================= */

void view_dim3rtl_map_model_mesh_start(void)
{
	int					n,k;
	char				sub_path[1024];
	bool				no_ray_trace_block;
	obj_type			*obj;
	weapon_type			*weap;
	model_type			*mdl;
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
			
			frame->bitmap.rl_material_id=view_dim3rtl_create_material_from_texture(sub_path,texture,frame,RL_MATERIAL_ALPHA_PASS_THROUGH);

			progress_update();
		}
	}
		
		// object models
	
	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;
		
		no_ray_trace_block=((map.camera.mode==cv_fpp) && (obj->idx==camera.obj_idx));
		view_dim3rtl_model_setup_single_model(&obj->draw,TRUE,no_ray_trace_block,FALSE,FALSE);

		progress_update();
	}

		// player weapon models

	if (!setup.no_draw_weapon) {

		obj=server.obj_list.objs[server.player_obj_idx];

		for (n=0;n!=max_weap_list;n++) {
			weap=obj->weap_list.weaps[n];
			if (weap==NULL) continue;
			
			view_dim3rtl_model_setup_single_model(&weap->draw,TRUE,FALSE,TRUE,TRUE);
			progress_update();
		}
	}
}

void view_dim3rtl_map_model_mesh_stop(void)
{
	int					n,k;
	obj_type			*obj;
	weapon_type			*weap;
	model_type			*mdl;
	texture_type		*texture;
	texture_frame_type	*frame;
	
		// delete object models
	
	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;
		
		view_dim3rtl_model_close_single_model(&obj->draw);
		progress_update();
	}

		// delete player weapon models

	if (!setup.no_draw_weapon) {
		obj=server.obj_list.objs[server.player_obj_idx];

		for (n=0;n!=max_weap_list;n++) {
			weap=obj->weap_list.weaps[n];
			if (weap==NULL) continue;
			
			view_dim3rtl_model_close_single_model(&weap->draw);
			progress_update();
		}
	}

		// delete materials
		// we do this last as we can't
		// delete attached materials

	for (k=0;k!=max_model_list;k++) {

		mdl=server.model_list.models[k];
		if (mdl==NULL) continue;

		for (n=0;n!=max_model_texture;n++) {
			texture=&mdl->textures[n];
			
			frame=&texture->frames[0];
			if (frame->name[0]==0x0) continue;

			if (frame->bitmap.rl_material_id==-1) continue;
			
			rtlMaterialDelete(frame->bitmap.rl_material_id);
			progress_update();
		}
	}	
}

void view_dim3rtl_map_model_update(void)
{
	int					n;
	obj_type			*obj;
	weapon_type			*weap;

	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj!=NULL) view_dim3rtl_model_update_single_model(&obj->draw,obj->hidden);
	}

		// player weapon models

	if (!setup.no_draw_weapon) {
		obj=server.obj_list.objs[server.player_obj_idx];

		for (n=0;n!=max_weap_list;n++) {
			weap=obj->weap_list.weaps[n];
			if (weap!=NULL) view_dim3rtl_model_update_single_model(&weap->draw,(n!=obj->held_weapon.current_idx));
		}
	}
}

/* =======================================================

      dim3RTL Projectile Models
      
======================================================= */

void view_dim3rtl_projectile_model_setup(proj_type *proj)
{
	view_dim3rtl_model_setup_single_model(&proj->draw,FALSE,FALSE,FALSE,FALSE);
}

void view_dim3rtl_projectile_model_close(proj_type *proj)
{
	view_dim3rtl_model_close_single_model(&proj->draw);
}

void view_dim3rtl_projectile_model_update(void)
{
	int					n;
	proj_type			*proj;

	for (n=0;n!=max_proj_list;n++) {
		proj=server.proj_list.projs[n];
		if (!proj->on) continue;

			// projectile drawings share
			// vertex lists

		render_model_build_vertex_lists(&proj->draw,TRUE);

			// rebuild the mesh

		view_dim3rtl_model_update_single_model(&proj->draw,FALSE);
	}
}
