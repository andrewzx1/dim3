/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View OpenRL Patches

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
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

	#define view_rl_buffer_wid		320
	#define view_rl_buffer_high		200

	int								view_rl_scene_id,
									view_rl_purple_material_id,view_rl_player_light_id;
	GLuint							view_rl_gl_id;

#endif

/* =======================================================

      Blank Patches
      
======================================================= */

#ifndef D3_OPENRL

bool view_openrl_initialize(char *err_str) { return(TRUE); }
void view_openrl_shutdown(void) {}
void view_openrl_map_setup(void) {}
void view_openrl_render(void) {}
void view_openrl_model_setup(void) {}
void view_openrl_mesh_cleanup(void) {}

#else

/* =======================================================

      View OpenRL Init/Shutdown
      
======================================================= */

bool view_openrl_initialize(char *err_str)
{
	int					n,sz;
	unsigned char		*data,*dptr;
	ray_color_type		col;

		// initialize OpenRL

	if (rlInitialize()!=RL_ERROR_OK) {
		strcpy(err_str,"Unable to initialize OpenRL");
		return(FALSE);
	}

		// make the scene

	view_rl_scene_id=rlSceneAdd(view_rl_buffer_wid,view_rl_buffer_high,RL_SCENE_TARGET_MEMORY,RL_SCENE_FORMAT_32_RGBA,0);
	if (view_rl_scene_id<0) {
		strcpy(err_str,"Unable to create OpenRL scene");
		rlShutdown();
		return(FALSE);
	}

		// test material

	col.r=1.0f;
	col.g=0.0f;
	col.b=1.0f;
	col.a=1.0f;
	view_rl_purple_material_id=rlMaterialAdd(1,1,0);
	rlMaterialAttachBufferColor(view_rl_purple_material_id,RL_MATERIAL_TARGET_COLOR,&col);

		// single player light
		
	view_rl_player_light_id=rlSceneLightAdd(view_rl_scene_id);
	rlSceneLightSetIntensity(view_rl_scene_id,view_rl_player_light_id,50000.0f,1.0f);
	
	col.r=1.0f;
	col.g=1.0f;
	col.b=1.0f;
	rlSceneLightSetColor(view_rl_scene_id,view_rl_player_light_id,&col);

		// we need a texture to transfer
		// the scene to opengl raster

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1,&view_rl_gl_id);
	glBindTexture(GL_TEXTURE_2D,view_rl_gl_id);

	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	sz=(view_rl_buffer_wid*4)*view_rl_buffer_high;
	data=malloc(sz);
	if (data==NULL) {
		strcpy(err_str,"Out of memory");
		rlShutdown();
		return(FALSE);
	}
	bzero(data,sz);

	dptr=data;
	for (n=0;n!=(sz/4);n++) {
		*dptr++=0xFF;
		*dptr++=0x0;
		*dptr++=0x0;
		*dptr++=0xFF;
	}

	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,view_rl_buffer_wid,view_rl_buffer_high,0,GL_RGBA,GL_UNSIGNED_BYTE,data);

	free(data);

	glBindTexture(GL_TEXTURE_2D,0);

	return(TRUE);
}

void view_openrl_shutdown(void)
{
	glDeleteTextures(1,&view_rl_gl_id);

	rlSceneDelete(view_rl_scene_id);
	rlShutdown();
}

/* =======================================================

      OpenRL Map and Model Mesh Setup
      
======================================================= */

int view_openrl_create_material(char *sub_path,texture_frame_type *frame)
{
	int					material_id,wid,high;
	bool				alpha_channel;
	char				name[256],path[1024];
	unsigned char		*png_data;

		// create material, use original
		// bitmap for wid/high
		
	file_paths_data(&file_path_setup,path,sub_path,frame->name,"png");
		
	png_data=png_utility_read(path,&wid,&high,&alpha_channel);
	if (png_data==NULL) return(-1);
	
	material_id=rlMaterialAdd(wid,high,0);
	rlMaterialAttachBufferData(material_id,RL_MATERIAL_TARGET_COLOR,(alpha_channel?RL_MATERIAL_FORMAT_32_RGBA:RL_MATERIAL_FORMAT_24_RGB),png_data);
	
	free(png_data);
	
		// normal map
		
	sprintf(name,"%s_n",frame->name);
	file_paths_data(&file_path_setup,path,sub_path,name,"png");

	png_data=png_utility_read(path,&wid,&high,&alpha_channel);
	if (png_data!=NULL) {
		rlMaterialAttachBufferData(material_id,RL_MATERIAL_TARGET_NORMAL,(alpha_channel?RL_MATERIAL_FORMAT_32_RGBA:RL_MATERIAL_FORMAT_24_RGB),png_data);
		free(png_data);
	}

		// specular map
		
	sprintf(name,"%s_s",frame->name);
	file_paths_data(&file_path_setup,path,sub_path,name,"png");

	png_data=png_utility_read(path,&wid,&high,&alpha_channel);
	if (png_data!=NULL) {
		rlMaterialAttachBufferData(material_id,RL_MATERIAL_TARGET_SPECULAR,(alpha_channel?RL_MATERIAL_FORMAT_32_RGBA:RL_MATERIAL_FORMAT_24_RGB),png_data);
		free(png_data);
	}
	
	return(material_id);
}

void view_openrl_map_setup(void)
{
	int					n,k,i,t,uv_count,meshId;
	float				*vertexes,*vp,*uvs,*vt,*normals,*vn;
	short				*vk,*ray_polys;
	d3pnt				*pnt;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	texture_type		*texture;
	texture_frame_type	*frame;
	
		// build the materials
		
	for (n=0;n!=max_map_texture;n++) {
		texture=&map.textures[n];
		
		frame=&texture->frames[0];
		if (frame->name[0]==0x0) continue;
		
		frame->bitmap.rl_material_id=view_openrl_create_material("Bitmaps/Textures",frame);
	}
			
		// build the meshes

	for (n=0;n!=map.mesh.nmesh;n++) {
		mesh=&map.mesh.meshes[n];
		if (!mesh->flag.on) continue;
			
			// add the mesh

		meshId=rlSceneMeshAdd(view_rl_scene_id,0);
		if (meshId<0) return;

			// the vertexes

		vertexes=(float*)malloc((mesh->nvertex*3)*sizeof(float));
		vp=vertexes;

		pnt=mesh->vertexes;

		for (k=0;k!=mesh->nvertex;k++) {
			*vp++=(float)pnt->x;
			*vp++=(float)pnt->y;
			*vp++=(float)pnt->z;
			pnt++;
		}
		
		rlSceneMeshSetVertex(view_rl_scene_id,meshId,RL_MESH_FORMAT_VERTEX_3_FLOAT,mesh->nvertex,vertexes);
		free(vertexes);

			// the UVs

		uvs=(float*)malloc((mesh->npoly*(8*2))*sizeof(float));		// supergumba -- this will work but chews up a lot of memory
		vt=uvs;

		uv_count=0;
		poly=mesh->polys;
	
		for (i=0;i!=mesh->npoly;i++) {
			for (t=0;t!=poly->ptsz;t++) {
				*vt++=poly->main_uv.uvs[t].x;
				*vt++=poly->main_uv.uvs[t].y;
			}
			uv_count+=poly->ptsz;
			poly++;
		}
			
		rlSceneMeshSetUV(view_rl_scene_id,meshId,RL_MESH_FORMAT_UV_2_FLOAT,uv_count,uvs);
		free(uvs);

			// the normals

		normals=(float*)malloc((mesh->npoly*3)*sizeof(float));
		vn=normals;

		poly=mesh->polys;
	
		for (i=0;i!=mesh->npoly;i++) {
			*vn++=poly->tangent_space.normal.x;
			*vn++=poly->tangent_space.normal.y;
			*vn++=poly->tangent_space.normal.z;
			poly++;
		}
		
		rlSceneMeshSetNormal(view_rl_scene_id,meshId,RL_MESH_FORMAT_NORMAL_3_FLOAT,mesh->npoly,normals);
		free(normals);

			// polygons

		ray_polys=(short*)malloc((mesh->npoly*(2+(3*8)))*sizeof(short));		// supergumba -- this will work but chews up a lot of memory
		vk=ray_polys;

		uv_count=0;
		poly=mesh->polys;
	
		for (i=0;i!=mesh->npoly;i++) {
			*vk++=poly->ptsz;
			*vk++=(short)map.textures[poly->txt_idx].frames[0].bitmap.rl_material_id;

			for (t=0;t!=poly->ptsz;t++) {
				*vk++=(short)poly->v[t];	// vertex
				*vk++=uv_count;				// uv, each vertex has unique uv count
				*vk++=(short)i;				// normal, one normal for each poly
				uv_count++;
			}

			poly++;
		}

		rlSceneMeshSetPoly(view_rl_scene_id,meshId,RL_MESH_FORMAT_POLY_SHORT_VERTEX_UV_NORMAL,mesh->npoly,ray_polys);
		free(ray_polys);
	}
}

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
			
			frame->bitmap.rl_material_id=view_openrl_create_material(sub_path,frame);
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

		ray_polys=(short*)malloc((mesh->npoly*(2+(3*8)))*sizeof(short));		// supergumba -- this will work but chews up a lot of memory
		vk=ray_polys;

		uv_count=0;
		poly=mesh->polys;
	
		for (i=0;i!=mesh->npoly;i++) {
			*vk++=poly->ptsz;
			*vk++=mdl->textures[poly->txt_idx].frames[0].bitmap.rl_material_id;

			for (t=0;t!=poly->ptsz;t++) {
				*vk++=(short)poly->v[t];	// vertex
				*vk++=uv_count;				// uv, each vertex has unique uv count
				*vk++=(short)i;				// normal, one normal for each poly
				uv_count++;
			}

			poly++;
		}

		rlSceneMeshSetPoly(view_rl_scene_id,mesh_id,RL_MESH_FORMAT_POLY_SHORT_VERTEX_UV_NORMAL,mesh->npoly,ray_polys);
		free(ray_polys);

			// set the draw's mesh id
			
		draw->openrl_mesh_id=mesh_id;
	}
}

void view_openrl_mesh_cleanup(void)
{
	rlSceneMeshDeleteAll(view_rl_scene_id);
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
		
		if (obj->hidden) continue;
		if ((map.camera.mode==cv_fpp) && (obj->idx==camera.obj_idx)) continue;		// supergumba -- we need to get this in scene anyway
		
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
	}
}

/* =======================================================

      OpenRL Rendering
      
======================================================= */

void view_openrl_transfer_to_opengl(void)
{
	int				lx,rx,ty,by,err;
	unsigned char	*data;

		// draws on 2D screen

	gl_2D_view_screen();

	lx=view.screen.x_sz-view_rl_buffer_wid;
	rx=view.screen.x_sz;
	ty=0;
	by=view_rl_buffer_high;

		// get the scene buffer
		// and push it to a texture
		// scene memory buffers was set
		// to RL_SCENE_FORMAT_32_RGBA

	err=rlSceneGetBuffer(view_rl_scene_id,(void**)&data);
	if (err!=RL_ERROR_OK) return;

	gl_texture_bind(0,view_rl_gl_id);
	glTexSubImage2D(GL_TEXTURE_2D,0,0,0,view_rl_buffer_wid,view_rl_buffer_high,GL_RGBA,GL_UNSIGNED_BYTE,data);

		// build the vertex and uv list

	view_primitive_2D_texture_quad(view_rl_gl_id,NULL,1.0f,lx,rx,ty,by,0.0f,1.0f,0.0f,1.0f,TRUE);
}

void view_openrl_render(void)
{
	float			ang_y;
	ray_point_type	pnt,light_pnt;
	ray_vector_type	scale,light_vct;
	ray_matrix_type	mat,x_mat,scale_mat;
	ray_color_type	col;

		// build the eye point

	pnt.x=(float)view.render->camera.pnt.x;
	pnt.y=(float)view.render->camera.pnt.y;
	pnt.z=(float)view.render->camera.pnt.z;

		// build the rotation matrix
		// dim3 always had a backwards look, so
		// we need to fix that with the matrix
		// normally it wouldn't be this complex

	ang_y=angle_add(view.render->camera.ang.y,180.0f);
	rlMatrixRotateY(&mat,ang_y);

	rlMatrixRotateX(&x_mat,-view.render->camera.ang.x);
	rlMatrixMultiply(&mat,&x_mat);

	scale.x=-1.0f;
	scale.y=1.0f;
	scale.z=1.0f;
	rlMatrixScale(&scale_mat,&scale);
	rlMatrixMultiply(&mat,&scale_mat);

		// set the eye position

	rlSceneEyePositionSet(view_rl_scene_id,&pnt,&mat,200.0f,1000000.0f);
	
		// update the player light
		// position
		
	light_vct.x=0.0f;
	light_vct.y=0.0f;
	light_vct.z=10000.0f;
	
	rlMatrixVectorMultiply(&mat,&light_vct);
		
	light_pnt.x=((float)view.render->camera.pnt.x)+light_vct.x;
	light_pnt.y=((float)view.render->camera.pnt.y)+light_vct.y;
	light_pnt.z=((float)view.render->camera.pnt.z)+light_vct.z;
	
	rlSceneLightSetPosition(view_rl_scene_id,view_rl_player_light_id,&light_pnt);
	
		// update the models
		
	view_openrl_model_update();

		// render

	col.r=0.0f;
	col.g=0.0f;
	col.b=0.0f;
	col.a=1.0f;
	rlSceneClearBuffer(view_rl_scene_id,&col);

	if (rlSceneRender(view_rl_scene_id)!=RL_ERROR_OK) return;

		// wait for render to end

	while (rlSceneRenderState(view_rl_scene_id)==RL_SCENE_STATE_RENDERING) {
		usleep(10);
	}
	
		// transfer to OpenGL

	view_openrl_transfer_to_opengl();
}

#endif