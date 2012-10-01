#include "ray_os_defs.h"
#include "ray.h"
#include "ray_interface.h"

ray_global_type					ray_global;

/* =======================================================

      Find Overlay Index for ID
      
======================================================= */

int ray_scene_overlay_get_index(ray_scene_type *scene,int overlayId)
{
	int					n;

	for (n=0;n!=scene->overlay_list.count;n++) {
		if (scene->overlay_list.overlays[n]->id==overlayId) return(n);
	}

	return(-1);
}

/* =======================================================

      Find Overlay Pixel
      
======================================================= */

bool ray_scene_overlay_get_pixel(ray_scene_type *scene,int x,int y,ray_color_type *col)
{
	int					n,px,py,offset;
	unsigned long		buf;
	float				f,fx,fy;
	ray_overlay_type	*overlay;
	ray_material_type	*material;
	
	for (n=0;n!=scene->overlay_list.count;n++) {
		overlay=scene->overlay_list.overlays[n];
		
			// determine if we are in overlay
			
		if (x<overlay->pnt.x) continue;
		if (x>=(overlay->pnt.x+overlay->pnt_size.x)) continue;
		if (y<overlay->pnt.y) continue;
		if (y>=(overlay->pnt.y+overlay->pnt_size.y)) continue;
		
			// sanity check for materials
			
		material=ray_global.material_list.materials[overlay->material_idx];
		if (material->color.data==NULL) continue;
		
			// get uv
			
		f=(float)(x-overlay->pnt.x)/(float)(overlay->pnt_size.x);
		fx=overlay->uv.x+((overlay->uv_size.x)*f);
		
		f=(float)(y-overlay->pnt.y)/(float)(overlay->pnt_size.y);
		fy=overlay->uv.y+((overlay->uv_size.y)*f);
		
			// change to texture coordinate
		
		px=(int)(fx*material->wid_scale);
		py=(int)(fy*material->high_scale);
	
		px=px&material->wid_mask;
		py=py&material->high_mask;
	
		offset=(material->wid*py)+px;
	
			// get color
			
		buf=*(((unsigned long*)material->color.data)+offset);
		ray_create_float_color_from_ulong(buf,col);

		return(TRUE);
	}

	return(FALSE);
}

/* =======================================================

      Adds a New Overlay to a Scene

	  Returns:
	   If >=0, then a overlay ID
	   RL_ERROR_UNKNOWN_SCENE_ID
	   RL_ERROR_UNKNOWN_MATERIAL_ID
	   RL_ERROR_OUT_OF_MEMORY
      
======================================================= */

int rlSceneOverlayAdd(int sceneId,int materialId,unsigned long flags)
{
	int					idx,material_idx;
	ray_overlay_type	*overlay;
	ray_scene_type		*scene;
	
		// get scene

	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);

	scene=ray_global.scene_list.scenes[idx];
	
		// lookup material
		
	material_idx=ray_material_get_index(materialId);
	if (material_idx==-1) return(RL_ERROR_UNKNOWN_MATERIAL_ID);

		// create new overlay

	overlay=(ray_overlay_type*)malloc(sizeof(ray_overlay_type));
	if (overlay==NULL) return(RL_ERROR_OUT_OF_MEMORY);

		// add overlay
		
	overlay->material_idx=material_idx;
		
	overlay->pnt.x=0;
	overlay->pnt.y=0;
	
	overlay->pnt_size.x=0;
	overlay->pnt_size.y=0;
	
	overlay->uv.x=0.0f;
	overlay->uv.y=0.0f;

	overlay->uv_size.x=1.0f;
	overlay->uv_size.y=1.0f;

		// set id

	overlay->id=scene->overlay_list.next_id;
	scene->overlay_list.next_id++;

		// add overlay to list

	scene->overlay_list.overlays[scene->overlay_list.count]=overlay;
	scene->overlay_list.count++;
	
	return(overlay->id);
}

/* =======================================================

      Deletes a Overlay from a Scene

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
	   RL_ERROR_UNKNOWN_OVERLAY_ID
	   RL_ERROR_SCENE_IN_USE
      
======================================================= */

int rlSceneOverlayDelete(int sceneId,int overlayId)
{
	int					n,idx,count;
	ray_overlay_type	*overlay;
	ray_scene_type		*scene;

		// get scene

	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);

	scene=ray_global.scene_list.scenes[idx];

		// can't delete if scene in use

	if (rlSceneRenderState(sceneId)==RL_SCENE_STATE_RENDERING) return(RL_ERROR_SCENE_IN_USE);

		// get the overlay

	idx=ray_scene_overlay_get_index(scene,overlayId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_OVERLAY_ID);

	overlay=scene->overlay_list.overlays[idx];

		// remove overlay

	free(overlay);

	count=scene->overlay_list.count-2;

	for (n=idx;n<=count;n++) {
		scene->overlay_list.overlays[n]=scene->overlay_list.overlays[n+1];
	}

	scene->overlay_list.count--;

	return(RL_ERROR_OK);
}

/* =======================================================

      Deletes all Overlays from a Scene

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
      
======================================================= */

int rlSceneOverlayDeleteAll(int sceneId)
{
	int				idx,err;
	ray_scene_type	*scene;

		// get scene

	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);

	scene=ray_global.scene_list.scenes[idx];

		// clear the overlays

	while (scene->overlay_list.count!=0) {
		err=rlSceneOverlayDelete(sceneId,scene->overlay_list.overlays[0]->id);
		if (err!=RL_ERROR_OK) return(err);
	}
	
	return(RL_ERROR_OK);
}

/* =======================================================

      Changes Position of Overlay Already in a Scene

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
	   RL_ERROR_UNKNOWN_OVERLAY_ID
      
======================================================= */

int rlSceneOverlaySetPosition(int sceneId,int overlayId,ray_2d_point_type *pnt)
{
	int					idx;
	ray_overlay_type	*overlay;
	ray_scene_type		*scene;

		// get scene

	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);

	scene=ray_global.scene_list.scenes[idx];

		// get the overlay

	idx=ray_scene_overlay_get_index(scene,overlayId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_OVERLAY_ID);

	overlay=scene->overlay_list.overlays[idx];

		// reset point
		
	overlay->pnt.x=pnt->x;
	overlay->pnt.y=pnt->y;

	return(RL_ERROR_OK);
}

/* =======================================================

      Changes Size of Overlay Already in a Scene

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
	   RL_ERROR_UNKNOWN_OVERLAY_ID
      
======================================================= */

int rlSceneOverlaySetSize(int sceneId,int overlayId,int width,int height)
{
	int					idx;
	ray_overlay_type	*overlay;
	ray_scene_type		*scene;

		// get scene

	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);

	scene=ray_global.scene_list.scenes[idx];

		// get the overlay

	idx=ray_scene_overlay_get_index(scene,overlayId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_OVERLAY_ID);

	overlay=scene->overlay_list.overlays[idx];

		// reset point
		
	overlay->pnt_size.x=width;
	overlay->pnt_size.y=height;

	return(RL_ERROR_OK);
}

/* =======================================================

      Changes UV of Overlay Already in a Scene

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
	   RL_ERROR_UNKNOWN_OVERLAY_ID
      
======================================================= */

int rlSceneOverlaySetUV(int sceneId,int overlayId,ray_uv_type *uv)
{
	int					idx;
	ray_overlay_type	*overlay;
	ray_scene_type		*scene;

		// get scene

	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);

	scene=ray_global.scene_list.scenes[idx];

		// get the overlay

	idx=ray_scene_overlay_get_index(scene,overlayId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_OVERLAY_ID);

	overlay=scene->overlay_list.overlays[idx];

		// reset UV
		
	overlay->uv.x=uv->x;
	overlay->uv.y=uv->y;

	return(RL_ERROR_OK);
}

/* =======================================================

      Changes UV Stamp of Overlay Already in a Scene

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
	   RL_ERROR_UNKNOWN_OVERLAY_ID
      
======================================================= */

int rlSceneOverlaySetUVStamp(int sceneId,int overlayId,ray_uv_type *uv)
{
	int					idx;
	ray_overlay_type	*overlay;
	ray_scene_type		*scene;

		// get scene

	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);

	scene=ray_global.scene_list.scenes[idx];

		// get the overlay

	idx=ray_scene_overlay_get_index(scene,overlayId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_OVERLAY_ID);

	overlay=scene->overlay_list.overlays[idx];

		// reset UV
		
	overlay->uv_size.x=uv->x;
	overlay->uv_size.y=uv->y;

	return(RL_ERROR_OK);
}

/* =======================================================

      Changes Material of Overlay Already in a Scene

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
	   RL_ERROR_UNKNOWN_OVERLAY_ID
	   RL_ERROR_UNKNOWN_MATERIAL_ID
      
======================================================= */

int rlSceneOverlaySetMaterial(int sceneId,int overlayId,int materialId)
{
	int					idx,material_idx;
	ray_overlay_type	*overlay;
	ray_scene_type		*scene;

		// get scene

	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);

	scene=ray_global.scene_list.scenes[idx];

		// get the overlay

	idx=ray_scene_overlay_get_index(scene,overlayId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_OVERLAY_ID);

	overlay=scene->overlay_list.overlays[idx];

		// reset material
		
	material_idx=ray_material_get_index(materialId);
	if (material_idx==-1) return(RL_ERROR_UNKNOWN_MATERIAL_ID);
	
	overlay->material_idx=material_idx;

	return(RL_ERROR_OK);
}
