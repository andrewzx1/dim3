	// types

#ifndef _ray_types
	#include "ray_types.h"
#endif

	// scene targets

#define RL_SCENE_TARGET_MEMORY								0

	// scene formats

#define RL_SCENE_FORMAT_32_RGBA								0

	// scene states

#define RL_SCENE_STATE_IDLE									0
#define RL_SCENE_STATE_RENDERING							1

	// material buffer targets

#define RL_MATERIAL_TARGET_COLOR							0
#define RL_MATERIAL_TARGET_NORMAL							1
#define RL_MATERIAL_TARGET_SPECULAR							2
#define RL_MATERIAL_TARGET_REFLECTION						3

	// material formats

#define RL_MATERIAL_FORMAT_32_RGBA							0
#define RL_MATERIAL_FORMAT_24_RGB							1

	// mesh array formats

#define RL_MESH_FORMAT_VERTEX_3_FLOAT						0
#define RL_MESH_FORMAT_UV_2_FLOAT							0
#define RL_MESH_FORMAT_NORMAL_3_FLOAT						0
#define RL_MESH_FORMAT_TANGENT_3_FLOAT						0

#define RL_MESH_FORMAT_POLY_SHORT_VERTEX_UV_NORMAL_TANGENT	0
#define RL_MESH_FORMAT_POLY_SHORT_VERTEX_UV					1

	// mesh flags

#define RL_MESH_FLAG_NONE									0x00000000
#define RL_MESH_FLAG_NON_LIGHT_BLOCKING						0x00000001
#define RL_MESH_FLAG_NON_RAY_TRACE_BLOCKING					0x00000002
#define RL_MESH_FLAG_HIGHLIGHT								0x00000004

	// errors

#define RL_ERROR_OK											0
#define RL_ERROR_UNKNOWN_SCENE_ID							-1
#define RL_ERROR_UNKNOWN_MATERIAL_ID						-2
#define RL_ERROR_UNKNOWN_LIGHT_ID							-3
#define RL_ERROR_UNKNOWN_MESH_ID							-4
#define RL_ERROR_UNKNOWN_OVERLAY_ID							-5
#define RL_ERROR_OUT_OF_MEMORY								-6
#define RL_ERROR_UNKNOWN_TARGET								-7
#define RL_ERROR_UNKNOWN_FORMAT								-8
#define RL_ERROR_SCENE_IN_USE								-9
#define RL_ERROR_MATERIAL_ATTACHED_TO_MESH					-10
#define RL_ERROR_MATERIAL_ATTACHED_TO_OVERLAY				-11
#define RL_ERROR_THREADING_ERROR							-12

//
// functions
//

	// initialize and shutdown
	
extern int rlInitialize(int reserveThreadCount);
extern int rlShutdown(void);

	// materials

extern int rlMaterialAdd(int wid,int high,unsigned long flags);
extern int rlMaterialDelete(int materialId);
extern int rlMaterialDeleteAll(void);
extern int rlMaterialAttachBufferData(int materialId,int target,int format,unsigned char* data);
extern int rlMaterialAttachBufferColor(int materialId,int target,rlColor *col);
extern int rlMaterialSetShineFactor(int materialId,float shineFactor);
extern int rlMaterialBuildMipMaps(int materialId);

	// scenes

extern int rlSceneAdd(rl2DPoint *size,int target,int format,void *attachment,unsigned long flags);
extern int rlSceneDelete(int sceneId);
extern int rlSceneClearBuffer(int sceneId,rlColor *col);
extern int rlSceneGetBuffer(int sceneId,void **buffer);
extern int rlSceneRender(int sceneId);
extern int rlSceneRenderState(int sceneId);

	// scene eye position
	
extern int rlSceneEyePositionSet(int sceneId,rlPoint *pnt,rlMatrix *rot_matrix,float eye_min_dist,float eye_max_dist);

	// scene lights

extern int rlSceneAmbient(int sceneId,rlColor *col);
extern int rlSceneLightAdd(int sceneId);
extern int rlSceneLightDelete(int sceneId,int lightId);
extern int rlSceneLightDeleteAll(int scenedId);
extern int rlSceneLightSetPosition(int sceneId,int lightId,rlPoint *pnt);
extern int rlSceneLightSetColor(int sceneId,int lightId,rlColor *col);
extern int rlSceneLightSetIntensity(int sceneId,int lightId,float intensity,float exponent);
extern int rlSceneLightSetDirection(int sceneId,int lightId,rlVector *vector,float angle,bool active);

	// scene meshes

extern int rlSceneMeshAdd(int sceneId,unsigned long flags);
extern int rlSceneMeshDelete(int sceneId,int meshId);
extern int rlSceneMeshDeleteAll(int scenedId);

extern int rlSceneMeshSetHidden(int sceneId,int meshId,bool hidden);
extern int rlSceneMeshSetTintColor(int sceneId,int meshId,ray_color_type *col);

extern int rlSceneMeshSetVertex(int sceneId,int meshId,int format,int count,void *vertex_data);
extern int rlSceneMeshMapVertexPointer(int sceneId,int meshId,void **vertex_data);
extern int rlSceneMeshUnMapVertexPointer(int sceneId,int meshId);

extern int rlSceneMeshSetUV(int sceneId,int meshId,int format,int count,void *uv_data);
extern int rlSceneMeshMapUVPointer(int sceneId,int meshId,void **uv_data);
extern int rlSceneMeshUnMapUVPointer(int sceneId,int meshId);

extern int rlSceneMeshSetNormal(int sceneId,int meshId,int format,int count,void *normal_data);
extern int rlSceneMeshMapNormalPointer(int sceneId,int meshId,void **normal_data);
extern int rlSceneMeshUnMapNormalPointer(int sceneId,int meshId);

extern int rlSceneMeshSetTangent(int sceneId,int meshId,int format,int count,void *tangent_data);
extern int rlSceneMeshMapTangentPointer(int sceneId,int meshId,void **tangent_data);
extern int rlSceneMeshUnMapTangentPointer(int sceneId,int meshId);

extern int rlSceneMeshSetPoly(int sceneId,int meshId,int format,int count,void *poly_data);

	// scene overlays
	
extern int rlSceneOverlayAdd(int sceneId,int materialId,unsigned long flags);
extern int rlSceneOverlayDelete(int sceneId,int overlayId);
extern int rlSceneOverlayDeleteAll(int sceneId);
extern int rlSceneOverlaySetPosition(int sceneId,int overlayId,rl2DPoint *pnt);
extern int rlSceneOverlaySetSize(int sceneId,int overlayId,rl2DPoint *pnt);
extern int rlSceneOverlaySetUV(int sceneId,int overlayId,rlUV *uv);
extern int rlSceneOverlaySetUVStamp(int sceneId,int overlayId,rlUV *uv);
extern int rlSceneOverlaySetMaterial(int sceneId,int overlayId,int materialId);
extern int rlSceneOverlayColor(int sceneId,int overlayId,rlColor *col);

	// math utilities

extern void rlMatrixIdentity(rlMatrix *mat);
extern void rlMatrixMultiply(rlMatrix *mat_1,rlMatrix *mat_2);
extern void rlMatrixVectorMultiply(rlMatrix *mat,rlVector *v);
extern void rlMatrixRotateX(rlMatrix *mat,float ang);
extern void rlMatrixRotateY(rlMatrix *mat,float ang);
extern void rlMatrixRotateZ(rlMatrix *mat,float ang);
extern void rlMatrixScale(rlMatrix *mat,rlVector *v);

