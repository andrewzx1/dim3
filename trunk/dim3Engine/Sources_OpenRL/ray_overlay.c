//
// OpenRL
// by Brian Barnes (bbarnes@klinksoftware.com)
// free software offered as is for any use
//

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

bool ray_get_overlay_rgb(ray_scene_type *scene,int x,int y,ray_color_type *col)
{
	int							n,k,px,py,offset;
	unsigned long				buf;
	float						f,fx,fy;
	bool						hit;
	ray_overlay_type			*overlay;
	ray_overlay_quad_type		*overlay_quad;
	ray_material_type			*material;
	ray_material_mipmap_type	*mipmap;
	ray_color_type				o_col;

	if (scene->overlay_list.count==0) return(FALSE);

		// no hits yet

	hit=FALSE;

		// search overlay lists backwards
		// so front to back ordering is
		// used for rendering

	for (n=(scene->overlay_list.count-1);n>=0;n--) {
		overlay=scene->overlay_list.overlays[n];
		if (overlay->hidden) continue;

			// skip if no quads

		if (overlay->quad_list.count==0) continue;
		
			// determine if we are in overlay
			
		if (x<overlay->pnt.x) continue;
		if (x>(overlay->pnt.x+overlay->pnt_size.x)) continue;
		if (y<overlay->pnt.y) continue;
		if (y>(overlay->pnt.y+overlay->pnt_size.y)) continue;

			// sanity check for materials
			
		material=ray_global.material_list.materials[overlay->material_idx];

		mipmap=&material->mipmap_list.mipmaps[overlay->mm_level];
		if (mipmap->data.color==NULL) continue;

			// determine the quad we are in

		for (k=(overlay->quad_list.count-1);k>=0;k--) {
			overlay_quad=overlay->quad_list.quads[k];

				// in this quad?
				// quad points are offsets of overlay box

			px=overlay->pnt.x+overlay_quad->offset_pnt.x;
			if (x<px) continue;
			if (x>(px+overlay_quad->pnt_size.x)) continue;

			py=overlay->pnt.y+overlay_quad->offset_pnt.y;
			if (y<py) continue;
			if (y>(py+overlay_quad->pnt_size.y)) continue;
				
				// get uv
				
			f=(float)(x-px)/(float)(overlay_quad->pnt_size.x);
			fx=overlay_quad->uv.x+((overlay_quad->uv_size.x)*f);
			
			f=(float)(y-py)/(float)(overlay_quad->pnt_size.y);
			fy=overlay_quad->uv.y+((overlay_quad->uv_size.y)*f);
			
				// change to texture coordinate
			
			fx-=floorf(fx);
			px=(int)(fx*mipmap->wid_scale);

			fy-=floorf(fy);
			py=(int)(fy*mipmap->high_scale);
		
			offset=(mipmap->wid*py)+px;
		
				// get color
				
			buf=*(((unsigned long*)mipmap->data.color)+offset);
			ray_create_float_color_from_ulong(buf,&o_col);

				// multiply in the tint

			o_col.r*=overlay_quad->col.r;
			o_col.g*=overlay_quad->col.g;
			o_col.b*=overlay_quad->col.b;
			o_col.a*=overlay_quad->col.a;

				// skip if no alpha

			if (o_col.a==0.0f) continue;

				// if no hits, this becomes
				// the new color

			if (!hit) {
				col->r=o_col.r;
				col->g=o_col.g;
				col->b=o_col.b;
			}

				// otherwise, we need to
				// mix the colors using the
				// previous alpha

			else {
				f=1.0f-col->a;
				col->r=(col->r*col->a)+(o_col.r*f);
				col->g=(col->g*col->a)+(o_col.g*f);
				col->b=(col->b*col->a)+(o_col.b*f);
			}

				// always adopt new alpha

			col->a=o_col.a;

			hit=TRUE;
		}
	}

	return(hit);
}

/* =======================================================

      Setup Overlays Before Drawing
      
======================================================= */

void ray_overlay_setup_all(ray_scene_type *scene)
{
	int						n,k,wid;
	ray_overlay_type		*overlay;
	ray_overlay_quad_type	*overlay_quad;
	ray_material_type		*material;

	for (n=0;n!=scene->overlay_list.count;n++) {
		overlay=scene->overlay_list.overlays[n];

			// no need for levels if hidden
			// or no quads

		if (overlay->hidden) continue;
		if (overlay->quad_list.count==0) continue;

			// get drawing size
			// if UV was 0...1
			// use first quad in list to determine mipmap level
			// (this assumes all quads are of relative size)

		overlay_quad=overlay->quad_list.quads[0];
		wid=(int)(((float)overlay_quad->pnt_size.x)*(1.0f/(overlay_quad->uv_size.x-overlay_quad->uv.x)));

			// get mipmap level
			// closer but >= to size
			
		material=ray_global.material_list.materials[overlay->material_idx];

		overlay->mm_level=0;

		for (k=0;k!=material->mipmap_list.count;k++) {
			if (material->mipmap_list.mipmaps[k].wid<wid) {
				overlay->mm_level=k-1;
				if (overlay->mm_level<0) overlay->mm_level=0;
				break;
			}
		}
	}
}

/* =======================================================

      Adds a New Overlay to a Scene
	  
	  Notes:
	   If the scene is currently rendering, this API
	   will stall until it's finished

	  Returns:
	   If >=0, then a overlay ID
	   RL_ERROR_UNKNOWN_SCENE_ID
	   RL_ERROR_UNKNOWN_MATERIAL_ID
	   RL_ERROR_OUT_OF_MEMORY
      
======================================================= */

int rlSceneOverlayAdd(int sceneId,int materialId,unsigned long flags)
{
	int					n,idx,material_idx;
	ray_overlay_type	*overlay;
	ray_scene_type		*scene;
	
		// get scene

	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);

	scene=ray_global.scene_list.scenes[idx];
	
		// stall rendering so it finishes
		
	ray_render_stall(scene);
	
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
	
	overlay->hidden=FALSE;

		// no quads

	overlay->quad_list.count=0;

	for (n=0;n!=ray_max_scene_overlay_quad;n++) {
		overlay->quad_list.quads[n]=NULL;
	}

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
	  
	  Notes:
	   If the scene is currently rendering, this API
	   will stall until it's finished

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
	   RL_ERROR_UNKNOWN_OVERLAY_ID
      
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
	
		// stall rendering so it finishes
		
	ray_render_stall(scene);

		// get the overlay

	idx=ray_scene_overlay_get_index(scene,overlayId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_OVERLAY_ID);

	overlay=scene->overlay_list.overlays[idx];

		// remove quads

	for (n=0;n!=ray_max_scene_overlay_quad;n++) {
		if (overlay->quad_list.quads[n]!=NULL) free(overlay->quad_list.quads[n]);
	}

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
	  
	  Notes:
	   If the scene is currently rendering, this API
	   will stall until it's finished

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

      Sets Number of Quads in an Overlay
	  
	  Notes:
	   If the scene is currently rendering, this API
	   will stall until it's finished

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
	   RL_ERROR_UNKNOWN_OVERLAY_ID
	   RL_ERROR_OUT_OF_MEMORY
      
======================================================= */

int rlSceneOverlaySetQuadCount(int sceneId,int overlayId,int count)
{
	int						n,idx;
	ray_overlay_type		*overlay;
	ray_overlay_quad_type	*overlay_quad;
	ray_scene_type			*scene;

		// too many quads?

	if (count>ray_max_scene_overlay_quad) return(RL_ERROR_OUT_OF_MEMORY);

		// get scene

	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);

	scene=ray_global.scene_list.scenes[idx];
	
		// stall rendering so it finishes
		
	ray_render_stall(scene);

		// get the overlay

	idx=ray_scene_overlay_get_index(scene,overlayId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_OVERLAY_ID);

	overlay=scene->overlay_list.overlays[idx];

		// if we had more quads than we started
		// with, clear the old ones

	for (n=count;n<overlay->quad_list.count;n++) {
		if (overlay->quad_list.quads[n]!=NULL) free(overlay->quad_list.quads[n]);
		overlay->quad_list.quads[n]=NULL;
	}

		// add any that we need

	for (n=0;n!=count;n++) {
		if (overlay->quad_list.quads[n]!=NULL) continue;
		
		overlay->quad_list.quads[n]=(ray_overlay_quad_type*)malloc(sizeof(ray_overlay_quad_type));
		if (overlay->quad_list.quads[n]==NULL) {
			overlay->quad_list.count=0;
			return(RL_ERROR_OUT_OF_MEMORY);
		}
	}

		// setup quads

	overlay->quad_list.count=count;

	for (n=0;n!=count;n++) {
		overlay_quad=overlay->quad_list.quads[n];

		overlay_quad->offset_pnt.x=0;
		overlay_quad->offset_pnt.y=0;

		overlay_quad->pnt_size.x=overlay->pnt_size.x;
		overlay_quad->pnt_size.y=overlay->pnt_size.y;

		overlay_quad->uv.x=0.0f;
		overlay_quad->uv.y=0.0f;

		overlay_quad->uv_size.x=1.0f;
		overlay_quad->uv_size.y=1.0f;

		overlay_quad->col.r=1.0f;
		overlay_quad->col.g=1.0f;
		overlay_quad->col.b=1.0f;
		overlay_quad->col.a=1.0f;
	}

	return(RL_ERROR_OK);
}

/* =======================================================

      Changes Material of Overlay Already in a Scene
	  
	  Notes:
	   If the scene is currently rendering, this API
	   will stall until it's finished

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
	
		// stall rendering so it finishes
		
	ray_render_stall(scene);

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

/* =======================================================

      Sets an Overlay to Show or Hide
	  
	  Notes:
	   If the scene is currently rendering, this API
	   will stall until it's finished

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
	   RL_ERROR_UNKNOWN_OVERLAY_ID
      
======================================================= */

int rlSceneOverlaySetHidden(int sceneId,int overlayId,bool hidden)
{
	int					idx;
	ray_overlay_type	*overlay;
	ray_scene_type		*scene;

		// get scene

	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);

	scene=ray_global.scene_list.scenes[idx];
	
		// stall rendering so it finishes
		
	ray_render_stall(scene);

		// get the overlay

	idx=ray_scene_overlay_get_index(scene,overlayId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_OVERLAY_ID);

	overlay=scene->overlay_list.overlays[idx];

		// reset hidden flag

	overlay->hidden=hidden;

	return(RL_ERROR_OK);
}

/* =======================================================

      Changes Position of Overlay Already in a Scene
	  
	  Notes:
	   If the scene is currently rendering, this API
	   will stall until it's finished

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
	
		// stall rendering so it finishes
		
	ray_render_stall(scene);

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
	  
	  Notes:
	   If the scene is currently rendering, this API
	   will stall until it's finished

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
	   RL_ERROR_UNKNOWN_OVERLAY_ID
      
======================================================= */

int rlSceneOverlaySetSize(int sceneId,int overlayId,ray_2d_point_type *pnt)
{
	int					idx;
	ray_overlay_type	*overlay;
	ray_scene_type		*scene;

		// get scene

	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);

	scene=ray_global.scene_list.scenes[idx];
	
		// stall rendering so it finishes
		
	ray_render_stall(scene);

		// get the overlay

	idx=ray_scene_overlay_get_index(scene,overlayId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_OVERLAY_ID);

	overlay=scene->overlay_list.overlays[idx];

		// reset point
		
	overlay->pnt_size.x=pnt->x;
	overlay->pnt_size.y=pnt->y;

	return(RL_ERROR_OK);
}

/* =======================================================

      Changes Offset Position of a Quad in an Overlay
	  
	  Notes:
	   If the scene is currently rendering, this API
	   will stall until it's finished

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
	   RL_ERROR_UNKNOWN_OVERLAY_ID
	   RL_ERROR_OVERLAY_QUAD_INDEX_OUT_OF_BOUNDS
      
======================================================= */

int rlSceneOverlaySetQuadPosition(int sceneId,int overlayId,int quadIndex,ray_2d_point_type *pnt)
{
	int						idx;
	ray_overlay_type		*overlay;
	ray_overlay_quad_type	*overlay_quad;
	ray_scene_type			*scene;

		// get scene

	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);

	scene=ray_global.scene_list.scenes[idx];
	
		// stall rendering so it finishes
		
	ray_render_stall(scene);

		// get the overlay

	idx=ray_scene_overlay_get_index(scene,overlayId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_OVERLAY_ID);

	overlay=scene->overlay_list.overlays[idx];

		// get quad

	if ((quadIndex<0) || (quadIndex>=overlay->quad_list.count)) return(RL_ERROR_OVERLAY_QUAD_INDEX_OUT_OF_BOUNDS);
	overlay_quad=overlay->quad_list.quads[quadIndex];

		// reset point
		
	overlay_quad->offset_pnt.x=pnt->x;
	overlay_quad->offset_pnt.y=pnt->y;

	return(RL_ERROR_OK);
}

/* =======================================================

      Changes Size of a Quad in an Overlay
	  
	  Notes:
	   If the scene is currently rendering, this API
	   will stall until it's finished

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
	   RL_ERROR_UNKNOWN_OVERLAY_ID
	   RL_ERROR_OVERLAY_QUAD_INDEX_OUT_OF_BOUNDS
      
======================================================= */

int rlSceneOverlaySetQuadSize(int sceneId,int overlayId,int quadIndex,ray_2d_point_type *pnt)
{
	int						idx;
	ray_overlay_type		*overlay;
	ray_overlay_quad_type	*overlay_quad;
	ray_scene_type			*scene;

		// get scene

	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);

	scene=ray_global.scene_list.scenes[idx];
	
		// stall rendering so it finishes
		
	ray_render_stall(scene);

		// get the overlay

	idx=ray_scene_overlay_get_index(scene,overlayId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_OVERLAY_ID);

	overlay=scene->overlay_list.overlays[idx];

		// get quad

	if ((quadIndex<0) || (quadIndex>=overlay->quad_list.count)) return(RL_ERROR_OVERLAY_QUAD_INDEX_OUT_OF_BOUNDS);
	overlay_quad=overlay->quad_list.quads[quadIndex];

		// reset point
		
	overlay_quad->pnt_size.x=pnt->x;
	overlay_quad->pnt_size.y=pnt->y;

	return(RL_ERROR_OK);
}

/* =======================================================

      Changes UV of a Quad in an Overlay
	  
	  Notes:
	   If the scene is currently rendering, this API
	   will stall until it's finished

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
	   RL_ERROR_UNKNOWN_OVERLAY_ID
	   RL_ERROR_OVERLAY_QUAD_INDEX_OUT_OF_BOUNDS
      
======================================================= */

int rlSceneOverlaySetQuadUV(int sceneId,int overlayId,int quadIndex,ray_uv_type *uv)
{
	int						idx;
	ray_overlay_type		*overlay;
	ray_overlay_quad_type	*overlay_quad;
	ray_scene_type			*scene;

		// get scene

	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);

	scene=ray_global.scene_list.scenes[idx];
	
		// stall rendering so it finishes
		
	ray_render_stall(scene);

		// get the overlay

	idx=ray_scene_overlay_get_index(scene,overlayId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_OVERLAY_ID);

	overlay=scene->overlay_list.overlays[idx];

		// get quad

	if ((quadIndex<0) || (quadIndex>=overlay->quad_list.count)) return(RL_ERROR_OVERLAY_QUAD_INDEX_OUT_OF_BOUNDS);
	overlay_quad=overlay->quad_list.quads[quadIndex];

		// reset UV
		
	overlay_quad->uv.x=uv->x;
	overlay_quad->uv.y=uv->y;

	return(RL_ERROR_OK);
}

/* =======================================================

      Changes UV Stamp of a Quad in an Overlay
	  
	  Notes:
	   If the scene is currently rendering, this API
	   will stall until it's finished

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
	   RL_ERROR_UNKNOWN_OVERLAY_ID
	   RL_ERROR_OVERLAY_QUAD_INDEX_OUT_OF_BOUNDS
      
======================================================= */

int rlSceneOverlaySetQuadUVStamp(int sceneId,int overlayId,int quadIndex,ray_uv_type *uv)
{
	int						idx;
	ray_overlay_type		*overlay;
	ray_overlay_quad_type	*overlay_quad;
	ray_scene_type			*scene;

		// get scene

	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);

	scene=ray_global.scene_list.scenes[idx];
	
		// stall rendering so it finishes
		
	ray_render_stall(scene);

		// get the overlay

	idx=ray_scene_overlay_get_index(scene,overlayId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_OVERLAY_ID);

	overlay=scene->overlay_list.overlays[idx];

		// get quad

	if ((quadIndex<0) || (quadIndex>=overlay->quad_list.count)) return(RL_ERROR_OVERLAY_QUAD_INDEX_OUT_OF_BOUNDS);
	overlay_quad=overlay->quad_list.quads[quadIndex];

		// reset UV
		
	overlay_quad->uv_size.x=uv->x;
	overlay_quad->uv_size.y=uv->y;

	return(RL_ERROR_OK);
}


/* =======================================================

      Changes Color Tint of a Quad in an Overlay
	  
	  Notes:
	   If the scene is currently rendering, this API
	   will stall until it's finished

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
	   RL_ERROR_UNKNOWN_OVERLAY_ID
	   RL_ERROR_OVERLAY_QUAD_INDEX_OUT_OF_BOUNDS
      
======================================================= */

int rlSceneOverlaySetQuadColor(int sceneId,int overlayId,int quadIndex,ray_color_type *col)
{
	int						idx;
	ray_overlay_type		*overlay;
	ray_overlay_quad_type	*overlay_quad;
	ray_scene_type			*scene;

		// get scene

	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);

	scene=ray_global.scene_list.scenes[idx];
	
		// stall rendering so it finishes
		
	ray_render_stall(scene);

		// get the overlay

	idx=ray_scene_overlay_get_index(scene,overlayId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_OVERLAY_ID);

	overlay=scene->overlay_list.overlays[idx];

		// get quad

	if ((quadIndex<0) || (quadIndex>=overlay->quad_list.count)) return(RL_ERROR_OVERLAY_QUAD_INDEX_OUT_OF_BOUNDS);
	overlay_quad=overlay->quad_list.quads[quadIndex];

		// reset color

	memmove(&overlay_quad->col,col,sizeof(ray_color_type));

	return(RL_ERROR_OK);
}

