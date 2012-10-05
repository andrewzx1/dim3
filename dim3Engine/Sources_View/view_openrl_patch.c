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
									view_rl_purple_material_id,
									view_rl_font_material_id,
									view_rl_lx,view_rl_rx,
									view_rl_ty,view_rl_by,
									view_rl_last_msec,view_rl_msec_display,
									view_rl_msec,view_rl_msec_count,
									view_rl_overlay[4];
	GLuint							view_rl_gl_id;

	texture_font_size_type			view_rl_font;

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
	int					n,sz,wid,high;
	float				f;
	unsigned char		*data,*dptr;
	rlColor				col;
	rl2DPoint			p_pnt,s_pnt;

		// initialize OpenRL

	if (rlInitialize(0)!=RL_ERROR_OK) {
		strcpy(err_str,"Unable to initialize OpenRL");
		return(FALSE);
	}

		// make the scene

	s_pnt.x=view_rl_buffer_wid;
	s_pnt.y=view_rl_buffer_high;

	view_rl_scene_id=rlSceneAdd(&s_pnt,RL_SCENE_TARGET_MEMORY,RL_SCENE_FORMAT_32_RGBA,NULL,0);
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

		// text material

	data=bitmap_text_size_data(&view_rl_font,"Arial",48,1024,512);
	view_rl_font_material_id=rlMaterialAdd(1024,512,0);
	rlMaterialAttachBufferData(view_rl_font_material_id,RL_MATERIAL_TARGET_COLOR,RL_MATERIAL_FORMAT_32_RGBA,data);
	free(data);

	rlMaterialBuildMipMaps(view_rl_font_material_id);

		// overlays for fps

	p_pnt.x=5;
	p_pnt.y=view_rl_buffer_high-25;
	s_pnt.x=15;
	s_pnt.y=20;

	col.r=1.0f;
	col.g=1.0f;
	col.b=0.0f;
	col.a=1.0f;

	for (n=0;n!=4;n++) {
		view_rl_overlay[n]=rlSceneOverlayAdd(view_rl_scene_id,view_rl_font_material_id,0);
		rlSceneOverlaySetPosition(view_rl_scene_id,view_rl_overlay[n],&p_pnt);
		rlSceneOverlaySetSize(view_rl_scene_id,view_rl_overlay[n],&s_pnt);
		rlSceneOverlayColor(view_rl_scene_id,view_rl_overlay[n],&col);
		p_pnt.x+=s_pnt.x;
	}

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

		// get drawing size

//	f=((float)view.screen.x_sz)/((float)view_rl_buffer_wid);
	f=1.0f;

	wid=(int)(((float)view_rl_buffer_wid)*f);
	view_rl_lx=(view.screen.x_sz-wid)>>1;
	view_rl_rx=view_rl_lx+wid;

	high=(int)(((float)view_rl_buffer_high)*f);
	view_rl_ty=(view.screen.y_sz-high)>>1;
	view_rl_by=view_rl_ty+high;

	view_rl_msec_count=0;
	view_rl_msec=0;
	view_rl_msec_display=0;
	view_rl_last_msec=game_time_get_raw();

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

int view_openrl_create_material(char *sub_path,texture_type *texture,texture_frame_type *frame)
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

	rlMaterialSetShineFactor(material_id,texture->shine_factor);

		// mipmaps

	rlMaterialBuildMipMaps(material_id);
	
	return(material_id);
}

void view_openrl_map_setup(void)
{
	int					n,k,i,t,uv_count,mesh_id,light_id;
	float				*vertexes,*vp,*uvs,*vt,*normals,*tangents,*vn;
	short				*vk,*ray_polys;
	d3pnt				*pnt;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	texture_type		*texture;
	texture_frame_type	*frame;
	map_light_type		*lit;
	rlPoint				lit_pnt;
	rlColor				lit_col;
	
		// build the materials
		
	for (n=0;n!=max_map_texture;n++) {
		texture=&map.textures[n];
		
		frame=&texture->frames[0];
		if (frame->name[0]==0x0) continue;
		
		frame->bitmap.rl_material_id=view_openrl_create_material("Bitmaps/Textures",texture,frame);
	}
			
		// build the meshes

	for (n=0;n!=map.mesh.nmesh;n++) {
		mesh=&map.mesh.meshes[n];
		if (!mesh->flag.on) continue;
			
			// add the mesh

		mesh_id=rlSceneMeshAdd(view_rl_scene_id,0);
		if (mesh_id<0) return;

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
		
		rlSceneMeshSetVertex(view_rl_scene_id,mesh_id,RL_MESH_FORMAT_VERTEX_3_FLOAT,mesh->nvertex,vertexes);
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
			
		rlSceneMeshSetUV(view_rl_scene_id,mesh_id,RL_MESH_FORMAT_UV_2_FLOAT,uv_count,uvs);
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
		
		rlSceneMeshSetNormal(view_rl_scene_id,mesh_id,RL_MESH_FORMAT_NORMAL_3_FLOAT,mesh->npoly,normals);
		free(normals);

			// the tangents

		tangents=(float*)malloc((mesh->npoly*3)*sizeof(float));
		vn=tangents;

		poly=mesh->polys;
	
		for (i=0;i!=mesh->npoly;i++) {
			*vn++=poly->tangent_space.tangent.x;
			*vn++=poly->tangent_space.tangent.y;
			*vn++=poly->tangent_space.tangent.z;
			poly++;
		}
		
		rlSceneMeshSetTangent(view_rl_scene_id,mesh_id,RL_MESH_FORMAT_TANGENT_3_FLOAT,mesh->npoly,tangents);
		free(tangents);

			// polygons

		ray_polys=(short*)malloc((mesh->npoly*(2+(4*8)))*sizeof(short));		// supergumba -- this will work but chews up a lot of memory
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
				*vk++=(short)i;				// tangent, one tangent for each poly
				uv_count++;
			}

			poly++;
		}

		rlSceneMeshSetPoly(view_rl_scene_id,mesh_id,RL_MESH_FORMAT_POLY_SHORT_VERTEX_UV_NORMAL_TANGENT,mesh->npoly,ray_polys);
		free(ray_polys);
	}

		// the ambient

	lit_col.r=map.ambient.light_color.r;
	lit_col.g=map.ambient.light_color.g;
	lit_col.b=map.ambient.light_color.b;
	rlSceneAmbient(view_rl_scene_id,&lit_col);
	
		// build the lights
		
	for (n=0;n!=map.nlight;n++) {
		lit=&map.lights[n];
		
		light_id=rlSceneLightAdd(view_rl_scene_id);
		rlSceneLightSetIntensity(view_rl_scene_id,light_id,(float)lit->setting.intensity,lit->setting.exponent);
	
		lit_col.r=lit->setting.col.r;
		lit_col.g=lit->setting.col.g;
		lit_col.b=lit->setting.col.b;
		rlSceneLightSetColor(view_rl_scene_id,light_id,&lit_col);

		lit_pnt.x=(float)lit->pnt.x;
		lit_pnt.y=(float)lit->pnt.y;
		lit_pnt.z=(float)lit->pnt.z;
		rlSceneLightSetPosition(view_rl_scene_id,light_id,&lit_pnt);
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
		rlSceneMeshSetTangent(view_rl_scene_id,draw->openrl_mesh_id,RL_MESH_FORMAT_NORMAL_3_FLOAT,mesh->nvertex,draw->setup.mesh_arrays[0].gl_tangent_array);
	}
}

/* =======================================================

      OpenRL Overlay Setup
      
======================================================= */

void view_openrl_update_overlays(void)
{
	int			n,xoff,yoff;
	rlUV		uv,uv_size;
	char		ch,str[32];

		// update timing

	view_rl_msec_count++;
	view_rl_msec+=(game_time_get_raw()-view_rl_last_msec);

	if (view_rl_msec_count==10) {
		view_rl_msec_display=view_rl_msec/view_rl_msec_count;
		view_rl_msec=0;
		view_rl_msec_count=0;
	}

		// update overlay

	sprintf(str,"%4d",view_rl_msec_display);

	for (n=0;n!=4;n++) {
		ch=str[n]-'!';

		yoff=ch/view_rl_font.char_per_line;
		xoff=ch-(yoff*view_rl_font.char_per_line);

		uv.x=((float)xoff)*view_rl_font.gl_xoff;
		uv.y=((float)yoff)*view_rl_font.gl_yoff;
		rlSceneOverlaySetUV(view_rl_scene_id,view_rl_overlay[n],&uv);

		uv_size.x=view_rl_font.gl_xadd;
		uv_size.y=view_rl_font.gl_yadd;
		rlSceneOverlaySetUVStamp(view_rl_scene_id,view_rl_overlay[n],&uv_size);
	}
}

/* =======================================================

      OpenRL Rendering
      
======================================================= */

void view_openrl_transfer_to_opengl(void)
{
	int				err;
	unsigned char	*data;

		// draws on 2D screen

	gl_2D_view_screen();

		// get the scene buffer
		// and push it to a texture
		// scene memory buffers was set
		// to RL_SCENE_FORMAT_32_RGBA

	err=rlSceneGetBuffer(view_rl_scene_id,(void**)&data);
	if (err!=RL_ERROR_OK) return;

	gl_texture_bind(0,view_rl_gl_id);
	glTexSubImage2D(GL_TEXTURE_2D,0,0,0,view_rl_buffer_wid,view_rl_buffer_high,GL_RGBA,GL_UNSIGNED_BYTE,data);

		// build the vertex and uv list

	view_primitive_2D_texture_quad(view_rl_gl_id,NULL,1.0f,view_rl_lx,view_rl_rx,view_rl_ty,view_rl_by,0.0f,1.0f,0.0f,1.0f,TRUE);
}

void view_openrl_render(void)
{
	float			ang_y;
	rlPoint			pnt;
	rlVector		scale;
	rlMatrix		mat,x_mat,scale_mat;
	rlColor			col;

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

	rlSceneEyePositionSet(view_rl_scene_id,&pnt,&mat,200.0f,300000.0f);

		// update the scene
		
	view_openrl_model_update();
	view_openrl_update_overlays();

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

		// update timer

	view_rl_last_msec=game_time_get_raw();
}

#endif