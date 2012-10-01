	// types

#ifndef _ray_types
	#include "ray_types.h"
#endif

	// scene targets

#define RL_SCENE_TARGET_MEMORY						0

	// scene formats

#define RL_SCENE_FORMAT_32_RGBA						0

	// scene states

#define RL_SCENE_STATE_IDLE							0
#define RL_SCENE_STATE_RENDERING					1

	// material buffer targets

#define RL_MATERIAL_TARGET_COLOR					0
#define RL_MATERIAL_TARGET_NORMAL					1
#define RL_MATERIAL_TARGET_SPECULAR					2
#define RL_MATERIAL_TARGET_REFLECTION				3

	// material formats

#define RL_MATERIAL_FORMAT_32_RGBA					0
#define RL_MATERIAL_FORMAT_24_RGB					1

	// mesh formats

#define RL_MESH_FORMAT_VERTEX_3_FLOAT				0

#define RL_MESH_FORMAT_UV_2_FLOAT					0

#define RL_MESH_FORMAT_NORMAL_3_FLOAT				0
#define RL_MESH_FORMAT_TANGENT_3_FLOAT				0

#define RL_MESH_FORMAT_POLY_SHORT_VERTEX_UV_NORMAL	0

	// mesh flags

#define RL_MESH_FLAG_NONE							0x00000000
#define RL_MESH_FLAG_NON_LIGHT_BLOCKING				0x00000001
#define RL_MESH_FLAG_HIGHLIGHT						0x00000002

	// errors

#define RL_ERROR_OK									0
#define RL_ERROR_UNKNOWN_SCENE_ID					-1
#define RL_ERROR_UNKNOWN_MATERIAL_ID				-2
#define RL_ERROR_UNKNOWN_LIGHT_ID					-3
#define RL_ERROR_UNKNOWN_MESH_ID					-4
#define RL_ERROR_UNKNOWN_OVERLAY_ID					-5
#define RL_ERROR_OUT_OF_MEMORY						-6
#define RL_ERROR_UNKNOWN_TARGET						-7
#define RL_ERROR_UNKNOWN_FORMAT						-8
#define RL_ERROR_FILE_NOT_FOUND						-9
#define RL_ERROR_FILE_PNG_BAD_HEADER				-10
#define RL_ERROR_FILE_PNG_LESS_24_BITS				-11
#define RL_ERROR_FILE_PNG_WRONG_WIDTH_HEIGHT		-12
#define RL_ERROR_SCENE_IN_USE						-13
#define RL_ERROR_MATERIAL_ATTACHED_TO_MESH			-14
#define RL_ERROR_MATERIAL_ATTACHED_TO_OVERLAY		-15
#define RL_ERROR_THREADING_ERROR					-16

//
// functions
//

	// initialize and shutdown
	
extern int rlInitialize(void);
extern int rlShutdown(void);

	// materials

extern int rlMaterialAdd(int wid,int high,unsigned long flags);
extern int rlMaterialDelete(int materialId);
extern int rlMaterialAttachBufferData(int materialId,int target,int format,unsigned char* data);
extern int rlMaterialAttachBufferColor(int materialId,int target,ray_color_type *col);
extern int rlMaterialSetShineFactor(int materialId,float shineFactor);

	// scenes

extern int rlSceneAdd(int wid,int high,int target,int format,unsigned long flags);
extern int rlSceneDelete(int sceneId);
extern int rlSceneClearBuffer(int sceneId,ray_color_type *col);
extern int rlSceneGetBuffer(int sceneId,void **buffer);
extern int rlSceneRender(int sceneId);
extern int rlSceneRenderState(int sceneId);

	// scene eye position
	
extern int rlSceneEyePositionSet(int sceneId,ray_point_type *pnt,ray_matrix_type *rot_matrix,float eye_min_dist,float eye_max_dist);

	// scene lights

extern int rlSceneLightAdd(int sceneId);
extern int rlSceneLightDelete(int sceneId,int lightId);
extern int rlSceneLightDeleteAll(int scenedId);
extern int rlSceneLightSetPosition(int sceneId,int lightId,ray_point_type *pnt);
extern int rlSceneLightSetColor(int sceneId,int lightId,ray_color_type *col);
extern int rlSceneLightSetIntensity(int sceneId,int lightId,float intensity,float exponent);

	// scene meshes

extern int rlSceneMeshAdd(int sceneId,unsigned long flags);
extern int rlSceneMeshDelete(int sceneId,int meshId);
extern int rlSceneMeshDeleteAll(int scenedId);
extern int rlSceneMeshSetHidden(int sceneId,int meshId,bool hidden);
extern int rlSceneMeshSetVertex(int sceneId,int meshId,int format,int count,void *vertex_data);
extern int rlSceneMeshSetUV(int sceneId,int meshId,int format,int count,void *uv_data);
extern int rlSceneMeshSetNormal(int sceneId,int meshId,int format,int count,void *normal_data);
extern int rlSceneMeshSetPoly(int sceneId,int meshId,int format,int count,void *poly_data);

	// scene overlays
	
extern int rlSceneOverlayAdd(int sceneId,int materialId,unsigned long flags);
extern int rlSceneOverlayDelete(int sceneId,int overlayId);
extern int rlSceneOverlayDeleteAll(int sceneId);
extern int rlSceneOverlaySetPosition(int sceneId,int overlayId,ray_2d_point_type *pnt);
extern int rlSceneOverlaySetSize(int sceneId,int overlayId,int width,int height);
extern int rlSceneOverlaySetUV(int sceneId,int overlayId,ray_uv_type *uv);
extern int rlSceneOverlaySetUVStamp(int sceneId,int overlayId,ray_uv_type *uv);
extern int rlSceneOverlaySetMaterial(int sceneId,int overlayId,int materialId);

	// math utilities

extern void rlMatrixIdentity(ray_matrix_type *mat);
extern void rlMatrixMultiply(ray_matrix_type *mat_1,ray_matrix_type *mat_2);
extern void rlMatrixVectorMultiply(ray_matrix_type *mat,ray_vector_type *v);
extern void rlMatrixRotateX(ray_matrix_type *mat,float ang);
extern void rlMatrixRotateY(ray_matrix_type *mat,float ang);
extern void rlMatrixRotateZ(ray_matrix_type *mat,float ang);
extern void rlMatrixScale(ray_matrix_type *mat,ray_vector_type *v);

