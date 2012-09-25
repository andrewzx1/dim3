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
extern view_type			view;
extern iface_type			iface;
extern setup_type			setup;
extern network_setup_type	net_setup;
extern file_path_setup_type	file_path_setup;

#ifdef D3_OPENRL

	#define view_rl_buffer_wid		320
	#define view_rl_buffer_high		200

	int								view_rl_scene_id,
									view_rl_purple_material_id,view_rl_yellow_material_id,
									view_rl_player_light_id;
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

#else

/* =======================================================

      View OpenRL Init/Shutdown
      
======================================================= */

bool view_openrl_initialize(char *err_str)
{
	int					n,sz;
	unsigned char		*data,*dptr;

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

	view_rl_purple_material_id=rlMaterialAdd(1,1,0);
	rlMaterialAttachBufferColor(view_rl_purple_material_id,RL_MATERIAL_BUFFER_COLOR,1.0f,0.0f,1.0f);

	view_rl_yellow_material_id=rlMaterialAdd(1,1,0);
	rlMaterialAttachBufferColor(view_rl_yellow_material_id,RL_MATERIAL_BUFFER_COLOR,1.0f,1.0f,0.0f);

		// single player light

	view_rl_player_light_id=rlSceneLightAdd(view_rl_scene_id);
	rlSceneLightSetPosition(view_rl_scene_id,view_rl_player_light_id,0.0f,0.0f,0.0f);
	rlSceneLightSetIntensity(view_rl_scene_id,view_rl_player_light_id,50000.0f,1.0f);
	rlSceneLightSetColor(view_rl_scene_id,view_rl_player_light_id,1.0f,1.0f,1.0f);

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

      OpenRL Mesh Setup
      
======================================================= */

void view_openrl_map_setup(void)
{
	int					n,k,i,t,uv_count,meshId;
	float				*vertexes,*vp,*uvs,*vt,*normals,*vn;
	short				*vk,*ray_polys;
	d3pnt				*pnt;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

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
			*vk++=(short)(poly->box.wall_like?view_rl_yellow_material_id:view_rl_purple_material_id);

			for (t=0;t!=poly->ptsz;t++) {
				*vk++=(short)poly->v[t];	// vertex
				*vk++=uv_count;				// uv, each vertex has unique uv count
				*vk++=(short)i;				// normal, one normal for each poly
				uv_count++;
			}

			poly++;
		}

		rlSceneMeshSetPoly(view_rl_scene_id,meshId,RL_MESH_FORMAT_POLY_SHORT_VERTEX_UV_NORMAL,mesh->npoly,ray_polys);
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
	ray_point_type	pnt;
	ray_matrix_type	mat,x_mat;

		// position light over player

	rlSceneLightSetPosition(view_rl_scene_id,view_rl_player_light_id,(float)view.render->camera.pnt.x,(float)view.render->camera.pnt.y,(float)view.render->camera.pnt.z);

		// build the eye point

	pnt.x=(float)view.render->camera.pnt.x;
	pnt.y=(float)view.render->camera.pnt.y;
	pnt.z=(float)view.render->camera.pnt.z;

		// build the rotation matrix

	ang_y=angle_add(view.render->camera.ang.y,180.0f);
	rlMatrixRotateY(&mat,ang_y);

	rlMatrixRotateX(&x_mat,-view.render->camera.ang.x);
	rlMatrixMultiply(&mat,&x_mat);

		// set the eye position

	rlSceneEyePositionSet(view_rl_scene_id,&pnt,&mat,200.0f,1000000.0f);

		// render

	rlSceneClearBuffer(view_rl_scene_id,0.0f,0.0f,0.0f,1.0f);

	if (rlSceneRender(view_rl_scene_id)!=RL_ERROR_OK) return;

		// wait for render to end

	while (rlSceneRenderState(view_rl_scene_id)==RL_SCENE_STATE_RENDERING) {
		usleep(10);
	}

		// transfer to OpenGL

	view_openrl_transfer_to_opengl();
}

#endif