//
// dim3RTL
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

      Adds a New Overlay to a Scene

	  Note:
	   Default is 0,0-0,0, uv 0..1,
	   no material, and color 0xFFFFFFFF
	  
	  Returns:
	   If >=0, then a overlay ID
	   RL_ERROR_UNKNOWN_SCENE_ID
	   RL_ERROR_UNKNOWN_MATERIAL_ID
	   RL_ERROR_UNKNOWN_OVERLAY_TYPE
	   RL_ERROR_OUT_OF_MEMORY
      
======================================================= */

int rtlSceneOverlayAdd(int sceneId,int overlayType,unsigned long flags)
{
	int					idx;
	ray_overlay_type	*overlay;
	ray_scene_type		*scene;
	
		// get scene

	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);

	scene=ray_global.scene_list.scenes[idx];

		// lookup type

	if ((overlayType!=RL_OVERLAY_TYPE_QUAD_COLOR) && (overlayType!=RL_OVERLAY_TYPE_QUAD_HORIZONTAL_GRADIENT) && (overlayType!=RL_OVERLAY_TYPE_QUAD_VERTICAL_GRADIENT) && (overlayType!=RL_OVERLAY_TYPE_QUAD_MATERIAL) && (overlayType!=RL_OVERLAY_TYPE_LINE_COLOR)) return(RL_ERROR_UNKNOWN_OVERLAY_TYPE);

		// create new overlay

	overlay=(ray_overlay_type*)malloc(sizeof(ray_overlay_type));
	if (overlay==NULL) return(RL_ERROR_OUT_OF_MEMORY);

		// add overlay
		
	overlay->overlay_type=overlayType;
	overlay->hidden=FALSE;
	
	overlay->tint.r=1.0f;
	overlay->tint.g=1.0f;
	overlay->tint.b=1.0f;
	overlay->tint.a=1.0f;

		// default for types

	switch (overlayType) {

		case RL_OVERLAY_TYPE_QUAD_COLOR:
			overlay->setup.quad_color.pnt.x=0;
			overlay->setup.quad_color.pnt.y=0;
			overlay->setup.quad_color.pnt_size.x=0;
			overlay->setup.quad_color.pnt_size.y=0;
			break;

		case RL_OVERLAY_TYPE_QUAD_HORIZONTAL_GRADIENT:
		case RL_OVERLAY_TYPE_QUAD_VERTICAL_GRADIENT:
			overlay->setup.quad_gradient.pnt.x=0;
			overlay->setup.quad_gradient.pnt.y=0;
			overlay->setup.quad_gradient.pnt_size.x=0;
			overlay->setup.quad_gradient.pnt_size.y=0;
			overlay->setup.quad_gradient.color_stop_count=1;
			overlay->setup.quad_gradient.color_stops[0].r=1.0f;
			overlay->setup.quad_gradient.color_stops[0].g=1.0f;
			overlay->setup.quad_gradient.color_stops[0].b=1.0f;
			break;

		case RL_OVERLAY_TYPE_QUAD_MATERIAL:
			overlay->setup.quad_material.material_idx=-1;
			overlay->setup.quad_material.pnt.x=0;
			overlay->setup.quad_material.pnt.y=0;
			overlay->setup.quad_material.pnt_size.x=0;
			overlay->setup.quad_material.pnt_size.y=0;
			overlay->setup.quad_material.uv.x=0.0f;
			overlay->setup.quad_material.uv.y=0.0f;
			overlay->setup.quad_material.uv_size.x=1.0f;
			overlay->setup.quad_material.uv_size.y=1.0f;
			break;

		case RL_OVERLAY_TYPE_LINE_COLOR:
			overlay->setup.line.start_pnt.x=0;
			overlay->setup.line.start_pnt.y=0;
			overlay->setup.line.end_pnt.x=0;
			overlay->setup.line.end_pnt.y=0;
			break;

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

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
	   RL_ERROR_UNKNOWN_OVERLAY_ID
      
======================================================= */

int rtlSceneOverlayDelete(int sceneId,int overlayId)
{
	int					n,idx,count;
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

int rtlSceneOverlayDeleteAll(int sceneId)
{
	int				idx,err;
	ray_scene_type	*scene;

		// get scene

	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);

	scene=ray_global.scene_list.scenes[idx];

		// clear the overlays

	while (scene->overlay_list.count!=0) {
		err=rtlSceneOverlayDelete(sceneId,scene->overlay_list.overlays[0]->id);
		if (err!=RL_ERROR_OK) return(err);
	}
	
	return(RL_ERROR_OK);
}

/* =======================================================

      Changes Material of Overlay Already in a Scene

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
	   RL_ERROR_UNKNOWN_OVERLAY_ID
	   RL_ERROR_ILLEGAL_ON_OVERLAY_TYPE
	   RL_ERROR_UNKNOWN_MATERIAL_ID
      
======================================================= */

int rtlSceneOverlaySetMaterial(int sceneId,int overlayId,int materialId)
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

	if (overlay->overlay_type!=RL_OVERLAY_TYPE_QUAD_MATERIAL) return(RL_ERROR_ILLEGAL_ON_OVERLAY_TYPE);

		// reset material
		
	material_idx=ray_material_get_index(materialId);
	if (material_idx==-1) return(RL_ERROR_UNKNOWN_MATERIAL_ID);
	
	overlay->setup.quad_material.material_idx=material_idx;

	return(RL_ERROR_OK);
}

/* =======================================================

      Sets tint (color) of Overlay Already in a Scene

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
	   RL_ERROR_UNKNOWN_OVERLAY_ID
      
======================================================= */

int rtlSceneOverlaySetTint(int sceneId,int overlayId,ray_color_type *col)
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

		// reset hidden flag

	overlay->tint.r=col->r;
	overlay->tint.g=col->g;
	overlay->tint.b=col->b;
	overlay->tint.a=col->a;

	return(RL_ERROR_OK);
}

/* =======================================================

      Sets an Overlay to Show or Hide

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
	   RL_ERROR_UNKNOWN_OVERLAY_ID
      
======================================================= */

int rtlSceneOverlaySetHidden(int sceneId,int overlayId,bool hidden)
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

		// reset hidden flag

	overlay->hidden=hidden;

	return(RL_ERROR_OK);
}

/* =======================================================

      Changes Position of Quad Overlay Already in a Scene

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
	   RL_ERROR_UNKNOWN_OVERLAY_ID
	   RL_ERROR_ILLEGAL_ON_OVERLAY_TYPE
      
======================================================= */

int rtlSceneOverlaySetQuadPosition(int sceneId,int overlayId,ray_2d_point_type *pnt,ray_2d_point_type *size)
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
		
	switch (overlay->overlay_type) {

		case RL_OVERLAY_TYPE_QUAD_COLOR:
			overlay->setup.quad_color.pnt.x=pnt->x;
			overlay->setup.quad_color.pnt.y=pnt->y;
			overlay->setup.quad_color.pnt_size.x=size->x;
			overlay->setup.quad_color.pnt_size.y=size->y;
			break;

		case RL_OVERLAY_TYPE_QUAD_HORIZONTAL_GRADIENT:
		case RL_OVERLAY_TYPE_QUAD_VERTICAL_GRADIENT:
			overlay->setup.quad_gradient.pnt.x=pnt->x;
			overlay->setup.quad_gradient.pnt.y=pnt->y;
			overlay->setup.quad_gradient.pnt_size.x=size->x;
			overlay->setup.quad_gradient.pnt_size.y=size->y;
			break;

		case RL_OVERLAY_TYPE_QUAD_MATERIAL:
			overlay->setup.quad_material.pnt.x=pnt->x;
			overlay->setup.quad_material.pnt.y=pnt->y;
			overlay->setup.quad_material.pnt_size.x=size->x;
			overlay->setup.quad_material.pnt_size.y=size->y;
			break;

		default:
			return(RL_ERROR_ILLEGAL_ON_OVERLAY_TYPE);

	}

	return(RL_ERROR_OK);
}

/* =======================================================

      Changes UV of Quad Overlay Already in a Scene

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
	   RL_ERROR_UNKNOWN_OVERLAY_ID
	   RL_ERROR_ILLEGAL_ON_OVERLAY_TYPE
      
======================================================= */

int rtlSceneOverlaySetQuadUV(int sceneId,int overlayId,ray_uv_type *uv,ray_uv_type *stamp)
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

	if (overlay->overlay_type!=RL_OVERLAY_TYPE_QUAD_MATERIAL) return(RL_ERROR_ILLEGAL_ON_OVERLAY_TYPE);
		
	overlay->setup.quad_material.uv.x=uv->x;
	overlay->setup.quad_material.uv.y=uv->y;
	overlay->setup.quad_material.uv_size.x=stamp->x;
	overlay->setup.quad_material.uv_size.y=stamp->y;

	return(RL_ERROR_OK);
}

/* =======================================================

      Sets color stops of an Overlay Already in a Scene

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
	   RL_ERROR_UNKNOWN_OVERLAY_ID
	   RL_ERROR_ILLEGAL_ON_OVERLAY_TYPE
      
======================================================= */

int rtlSceneOverlaySetGradientColorStops(int sceneId,int overlayId,int colorStopCount,ray_color_type *colorStops)
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

		// reset color stops

	if ((overlay->overlay_type!=RL_OVERLAY_TYPE_QUAD_HORIZONTAL_GRADIENT) && (overlay->overlay_type!=RL_OVERLAY_TYPE_QUAD_VERTICAL_GRADIENT)) return(RL_ERROR_ILLEGAL_ON_OVERLAY_TYPE);
	
	if (colorStopCount>ray_max_scene_overlay_color_stop) colorStopCount=ray_max_scene_overlay_color_stop;

	overlay->setup.quad_gradient.color_stop_count=colorStopCount;
	memmove(overlay->setup.quad_gradient.color_stops,colorStops,(sizeof(ray_color_type)*colorStopCount));

	return(RL_ERROR_OK);
}

/* =======================================================

      Changes Position of Line Overlay Already in a Scene

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
	   RL_ERROR_UNKNOWN_OVERLAY_ID
	   RL_ERROR_ILLEGAL_ON_OVERLAY_TYPE
      
======================================================= */

int rtlSceneOverlaySetLinePosition(int sceneId,int overlayId,ray_2d_point_type *startPnt,ray_2d_point_type *endPnt)
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

	if (overlay->overlay_type!=RL_OVERLAY_TYPE_LINE_COLOR) return(RL_ERROR_ILLEGAL_ON_OVERLAY_TYPE);
		
	overlay->setup.line.start_pnt.x=startPnt->x;
	overlay->setup.line.start_pnt.y=startPnt->y;
	overlay->setup.line.end_pnt.x=endPnt->x;
	overlay->setup.line.end_pnt.y=endPnt->y;

	return(RL_ERROR_OK);
}

/* =======================================================

      Sets the drawing scale of the overlay
	  
	  Note:
	   This defaults to the render's buffer width
	   and height
	  
	  Returns:
	   RL_ERROR_UNKNOWN_SCENE_ID
      
======================================================= */

int rtlSceneOverlaySetScale(int sceneId,rtl2DPoint *size)
{
	int					idx;
	ray_scene_type		*scene;

		// get scene

	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);

	scene=ray_global.scene_list.scenes[idx];
	
		// set the scale
		
	scene->overlay_scale.x=size->x;
	scene->overlay_scale.y=size->y;
	
	return(RL_ERROR_OK);
}

/* =======================================================

      Internal Draw Utilities
      
======================================================= */

void ray_scene_overlay_draw_quad_material(ray_scene_type *scene,ray_overlay_type *overlay)
{
	int							n,wid,mm_level,
								x,y,lx,rx,ty,by,px,py,
								clip_lx,clip_rx,clip_ty,clip_by,
								y_material_buf_offset;
	float						fx,fy,fx_add,fy_add,inv_a;
	unsigned long				*buf;
	unsigned long				mat_buf;
	ray_material_type			*material;
	ray_material_mipmap_type	*mipmap;
	ray_color_type				buf_col,overlay_col;

		// if no alpha, skip

	if (overlay->tint.a==0.0f) return;

		// get the mipmap level
		
	if (overlay->setup.quad_material.material_idx==-1) return;
	material=ray_global.material_list.materials[overlay->setup.quad_material.material_idx];
	
	wid=(overlay->setup.quad_material.pnt_size.x*scene->buffer.wid)/scene->overlay_scale.x;
	wid=(int)(((float)wid)*(1.0f/(overlay->setup.quad_material.uv_size.x-overlay->setup.quad_material.uv.x)));

	mm_level=0;

	for (n=0;n!=material->mipmap_list.count;n++) {
		if (material->mipmap_list.mipmaps[n].wid<wid) {
			mm_level=n-1;
			if (mm_level<0) mm_level=0;
			break;
		}
	}

	mipmap=&material->mipmap_list.mipmaps[mm_level];
	if (mipmap->data.color==NULL) return;

		// quad size

	lx=overlay->setup.quad_material.pnt.x;
	rx=lx+overlay->setup.quad_material.pnt_size.x;
	ty=overlay->setup.quad_material.pnt.y;
	by=ty+overlay->setup.quad_material.pnt_size.y;
	
		// scale and clip
		
	lx=(lx*scene->buffer.wid)/scene->overlay_scale.x;
	rx=(rx*scene->buffer.wid)/scene->overlay_scale.x;
	ty=(ty*scene->buffer.high)/scene->overlay_scale.y;
	by=(by*scene->buffer.high)/scene->overlay_scale.y;
	
	clip_lx=lx;
	if (clip_lx<0) clip_lx=0;

	clip_rx=rx;
	if (clip_rx>=scene->buffer.wid) clip_rx=scene->buffer.wid-1;

	clip_ty=ty;
	if (clip_ty<0) clip_ty=0;

	clip_by=by;
	if (clip_by>=scene->buffer.high) clip_by=scene->buffer.high-1;

		// uv calcs

	fx_add=overlay->setup.quad_material.uv_size.x/(float)(rx-lx);
	fy_add=overlay->setup.quad_material.uv_size.y/(float)(by-ty);

		// draw a quad that has no alpha
		// this is the easiest route

	if ((material->no_alpha) && (overlay->tint.a==1.0f)) {

		fy=overlay->setup.quad_material.uv.y+((clip_ty-ty)*fy_add);
		
		for (y=clip_ty;y<clip_by;y++) {

				// get y texture coordinate
				
			py=(int)((fy-floorf(fy))*mipmap->high_scale);
			fy+=fy_add;

				// buffer lines

			buf=scene->buffer.data+((y*scene->buffer.wid)+clip_lx);

			y_material_buf_offset=mipmap->wid*py;

				// draw the row

			fx=overlay->setup.quad_material.uv.x+((clip_lx-lx)*fx_add);

			for (x=clip_lx;x<clip_rx;x++) {

					// get x texture coordinate
					
				px=(int)((fx-floorf(fx))*mipmap->wid_scale);
				fx+=fx_add;

					// get color
			
				*buf++=*(((unsigned long*)mipmap->data.color)+(y_material_buf_offset+px));
			}
		}

		return;
	}

		// draw quad with alpha
		// this is the slower path

	fy=overlay->setup.quad_material.uv.y+((clip_ty-ty)*fy_add);
	
	for (y=clip_ty;y<clip_by;y++) {

			// get y texture coordinate
			
		py=(int)((fy-floorf(fy))*mipmap->high_scale);
		fy+=fy_add;

			// buffer lines

		buf=scene->buffer.data+((y*scene->buffer.wid)+clip_lx);

		y_material_buf_offset=mipmap->wid*py;

			// draw the row

		fx=overlay->setup.quad_material.uv.x+((clip_lx-lx)*fx_add);

		for (x=clip_lx;x<clip_rx;x++) {

				// get x texture coordinate
				
			px=(int)((fx-floorf(fx))*mipmap->wid_scale);
			fx+=fx_add;

				// get color
		
			mat_buf=*(((unsigned long*)mipmap->data.color)+(y_material_buf_offset+px));
			ray_create_float_color_from_ulong(mat_buf,&overlay_col);

				// multiply in the tint

			overlay_col.r*=overlay->tint.r;
			overlay_col.g*=overlay->tint.g;
			overlay_col.b*=overlay->tint.b;
			overlay_col.a*=overlay->tint.a;

				// overwrite if alpha=1.0f

			if (overlay_col.a==1.0f) {
				*buf++=ray_create_ulong_color_from_float_no_alpha(&overlay_col);
				continue;
			}

				// skip if no alpha

			if (overlay_col.a==0.0f) {
				buf++;
				continue;
			}

				// mix with previous values

			ray_create_float_color_from_ulong_no_alpha(*buf,&buf_col);

			inv_a=1.0f-overlay_col.a;
			buf_col.r=(buf_col.r*inv_a)+(overlay_col.r*overlay_col.a);
			buf_col.g=(buf_col.g*inv_a)+(overlay_col.g*overlay_col.a);
			buf_col.b=(buf_col.b*inv_a)+(overlay_col.b*overlay_col.a);

			*buf=ray_create_ulong_color_from_float_no_alpha(&buf_col);

			buf++;
		}
	}
}

void ray_scene_overlay_draw_quad_color(ray_scene_type *scene,ray_overlay_type *overlay)
{
	int							x,y,lx,rx,ty,by,
								clip_lx,clip_rx,clip_ty,clip_by;
	float						inv_a;
	unsigned long				*buf;
	unsigned long				uc_col;
	ray_color_type				buf_col;

		// if no alpha, skip

	if (overlay->tint.a==0.0f) return;

		// quad size

	lx=overlay->setup.quad_color.pnt.x;
	rx=lx+overlay->setup.quad_color.pnt_size.x;
	ty=overlay->setup.quad_color.pnt.y;
	by=ty+overlay->setup.quad_color.pnt_size.y;
	
		// scale and clip
		
	lx=(lx*scene->buffer.wid)/scene->overlay_scale.x;
	rx=(rx*scene->buffer.wid)/scene->overlay_scale.x;
	ty=(ty*scene->buffer.high)/scene->overlay_scale.y;
	by=(by*scene->buffer.high)/scene->overlay_scale.y;

	clip_lx=lx;
	if (clip_lx<0) clip_lx=0;

	clip_rx=rx;
	if (clip_rx>=scene->buffer.wid) clip_rx=scene->buffer.wid-1;

	clip_ty=ty;
	if (clip_ty<0) clip_ty=0;

	clip_by=by;
	if (clip_by>=scene->buffer.high) clip_by=scene->buffer.high-1;

		// draw a quad that has no alpha
		// this is the easiest route

	if (overlay->tint.a==1.0f) {
		
		uc_col=ray_create_ulong_color_from_float_no_alpha(&overlay->tint);

		for (y=clip_ty;y<clip_by;y++) {

			buf=scene->buffer.data+((y*scene->buffer.wid)+clip_lx);

			for (x=clip_lx;x<clip_rx;x++) {
				*buf++=uc_col;
			}
		}

		return;
	}

		// draw quad that has alpha
		// this is the slower path
		
	for (y=clip_ty;y<clip_by;y++) {

		buf=scene->buffer.data+((y*scene->buffer.wid)+clip_lx);

		for (x=clip_lx;x<clip_rx;x++) {

				// mix with previous values

			ray_create_float_color_from_ulong_no_alpha(*buf,&buf_col);

			inv_a=1.0f-overlay->tint.a;
			buf_col.r=(buf_col.r*inv_a)+(overlay->tint.r*overlay->tint.a);
			buf_col.g=(buf_col.g*inv_a)+(overlay->tint.g*overlay->tint.a);
			buf_col.b=(buf_col.b*inv_a)+(overlay->tint.b*overlay->tint.a);

			*buf=ray_create_ulong_color_from_float_no_alpha(&buf_col);

			buf++;
		}
	}
}

void ray_scene_overlay_draw_quad_gradient_calc(ray_overlay_type *overlay,int x,int wid,ray_color_type *col)
{
	int				idx1,idx2,x_add;
	float			f;

		// no color stops

	if (overlay->setup.quad_gradient.color_stop_count==0) {
		col->r=overlay->tint.r;
		col->g=overlay->tint.g;
		col->b=overlay->tint.b;
		col->a=overlay->tint.a;
		return;
	}

		// get color stops

	x_add=(wid/(overlay->setup.quad_gradient.color_stop_count-1));

	idx1=x/x_add;
	idx2=idx1+1;
	if (idx2>=overlay->setup.quad_gradient.color_stop_count) idx2=overlay->setup.quad_gradient.color_stop_count-1;

		// calculate the color

	f=((float)(x%x_add))/(float)x_add;

	col->r=overlay->setup.quad_gradient.color_stops[idx1].r+(f*(overlay->setup.quad_gradient.color_stops[idx2].r-overlay->setup.quad_gradient.color_stops[idx1].r));
	col->g=overlay->setup.quad_gradient.color_stops[idx1].g+(f*(overlay->setup.quad_gradient.color_stops[idx2].g-overlay->setup.quad_gradient.color_stops[idx1].g));
	col->b=overlay->setup.quad_gradient.color_stops[idx1].b+(f*(overlay->setup.quad_gradient.color_stops[idx2].b-overlay->setup.quad_gradient.color_stops[idx1].b));

		// add in the tint

	col->r*=overlay->tint.r;
	col->g*=overlay->tint.g;
	col->b*=overlay->tint.b;
	col->a=overlay->tint.a;
}

void ray_scene_overlay_draw_quad_horizontal_gradient(ray_scene_type *scene,ray_overlay_type *overlay)
{
	int							x,y,lx,rx,ty,by,
								clip_lx,clip_rx,clip_ty,clip_by;
	float						inv_a;
	unsigned long				*buf;
	ray_color_type				col,buf_col;

		// if no alpha, skip

	if (overlay->tint.a==0.0f) return;

		// quad size

	lx=overlay->setup.quad_gradient.pnt.x;
	rx=lx+overlay->setup.quad_gradient.pnt_size.x;
	ty=overlay->setup.quad_gradient.pnt.y;
	by=ty+overlay->setup.quad_gradient.pnt_size.y;
	
		// scale and clip it
		
	lx=(lx*scene->buffer.wid)/scene->overlay_scale.x;
	rx=(rx*scene->buffer.wid)/scene->overlay_scale.x;
	ty=(ty*scene->buffer.high)/scene->overlay_scale.y;
	by=(by*scene->buffer.high)/scene->overlay_scale.y;

	clip_lx=lx;
	if (clip_lx<0) clip_lx=0;

	clip_rx=rx;
	if (clip_rx>=scene->buffer.wid) clip_rx=scene->buffer.wid-1;

	clip_ty=ty;
	if (clip_ty<0) clip_ty=0;

	clip_by=by;
	if (clip_by>=scene->buffer.high) clip_by=scene->buffer.high-1;

		// draw a quad that has no alpha
		// this is the easiest route

	if (overlay->tint.a==1.0f) {
		
		for (y=clip_ty;y<clip_by;y++) {

			buf=scene->buffer.data+((y*scene->buffer.wid)+clip_lx);

			for (x=clip_lx;x<clip_rx;x++) {
				ray_scene_overlay_draw_quad_gradient_calc(overlay,(x-lx),(rx-lx),&col);
				*buf++=ray_create_ulong_color_from_float_no_alpha(&col);
			}
		}

		return;
	}

		// draw quad that has alpha
		// this is the slower path
		
	for (y=clip_ty;y<clip_by;y++) {

		buf=scene->buffer.data+((y*scene->buffer.wid)+clip_lx);

		for (x=clip_lx;x<clip_rx;x++) {

				// mix with previous values

			ray_scene_overlay_draw_quad_gradient_calc(overlay,(x-lx),(rx-lx),&col);
			ray_create_float_color_from_ulong_no_alpha(*buf,&buf_col);

			inv_a=1.0f-col.a;
			buf_col.r=(buf_col.r*inv_a)+(col.r*col.a);
			buf_col.g=(buf_col.g*inv_a)+(col.g*col.a);
			buf_col.b=(buf_col.b*inv_a)+(col.b*col.a);

			*buf=ray_create_ulong_color_from_float_no_alpha(&buf_col);

			buf++;
		}
	}
}

void ray_scene_overlay_draw_quad_vertical_gradient(ray_scene_type *scene,ray_overlay_type *overlay)
{
	int							x,y,lx,rx,ty,by,
								clip_lx,clip_rx,clip_ty,clip_by;
	float						inv_a;
	unsigned long				*buf;
	unsigned long				uc_col;
	ray_color_type				col,buf_col;

		// if no alpha, skip

	if (overlay->tint.a==0.0f) return;

		// quad size

	lx=overlay->setup.quad_gradient.pnt.x;
	rx=lx+overlay->setup.quad_gradient.pnt_size.x;
	ty=overlay->setup.quad_gradient.pnt.y;
	by=ty+overlay->setup.quad_gradient.pnt_size.y;
	
		// scale and clip it
		
	lx=(lx*scene->buffer.wid)/scene->overlay_scale.x;
	rx=(rx*scene->buffer.wid)/scene->overlay_scale.x;
	ty=(ty*scene->buffer.high)/scene->overlay_scale.y;
	by=(by*scene->buffer.high)/scene->overlay_scale.y;

	clip_lx=lx;
	if (clip_lx<0) clip_lx=0;

	clip_rx=rx;
	if (clip_rx>=scene->buffer.wid) clip_rx=scene->buffer.wid-1;

	clip_ty=ty;
	if (clip_ty<0) clip_ty=0;

	clip_by=by;
	if (clip_by>=scene->buffer.high) clip_by=scene->buffer.high-1;

		// draw a quad that has no alpha
		// this is the easiest route

	if (overlay->tint.a==1.0f) {
		
		for (y=clip_ty;y<clip_by;y++) {

			ray_scene_overlay_draw_quad_gradient_calc(overlay,(y-ty),(by-ty),&col);
			uc_col=ray_create_ulong_color_from_float_no_alpha(&col);

			buf=scene->buffer.data+((y*scene->buffer.wid)+clip_lx);

			for (x=clip_lx;x<clip_rx;x++) {
				*buf++=uc_col;
			}
		}

		return;
	}

		// draw quad that has alpha
		// this is the slower path
		
	for (y=clip_ty;y<clip_by;y++) {

		ray_scene_overlay_draw_quad_gradient_calc(overlay,(y-ty),(by-ty),&col);
		buf=scene->buffer.data+((y*scene->buffer.wid)+clip_lx);

		for (x=clip_lx;x<clip_rx;x++) {

				// mix with previous values

			ray_create_float_color_from_ulong_no_alpha(*buf,&buf_col);

			inv_a=1.0f-col.a;
			buf_col.r=(buf_col.r*inv_a)+(col.r*col.a);
			buf_col.g=(buf_col.g*inv_a)+(col.g*col.a);
			buf_col.b=(buf_col.b*inv_a)+(col.b*col.a);

			*buf=ray_create_ulong_color_from_float_no_alpha(&buf_col);

			buf++;
		}
	}
}

void ray_scene_overlay_draw_line_color(ray_scene_type *scene,ray_overlay_type *overlay)
{
	int							k,x,y,dx,dy,lx,rx,ty,by;
	float						fx,fy,fx_add,fy_add,inv_a;
	unsigned long				*buf;
	unsigned long				uc_col;
	ray_color_type				buf_col;

		// if no alpha, skip

	if (overlay->tint.a==0.0f) return;
	
		// get points
		
	lx=overlay->setup.line.start_pnt.x;
	rx=overlay->setup.line.end_pnt.x;
	ty=overlay->setup.line.start_pnt.y;
	by=overlay->setup.line.end_pnt.y;
	
		// scale it
		
	lx=(lx*scene->buffer.wid)/scene->overlay_scale.x;
	rx=(rx*scene->buffer.wid)/scene->overlay_scale.x;
	ty=(ty*scene->buffer.high)/scene->overlay_scale.y;
	by=(by*scene->buffer.high)/scene->overlay_scale.y;

		// find which direction to draw
		
	dx=abs(rx-lx);
	dy=abs(by-ty);
	
		// horizontal
		
	if (dx>dy) {
	
		if (lx>rx) {
			k=lx;
			lx=rx;
			rx=k;
			k=ty;
			ty=by;
			by=k;
		}
		
		fy=(float)ty;
		fy_add=((float)(by-ty))/((float)dx);

			// draw horz line with no alpha

		if (overlay->tint.a==1.0f) {
			uc_col=ray_create_ulong_color_from_float_no_alpha(&overlay->tint);

			for (x=lx;x<rx;x++) {
			
				y=(int)fy;
				fy+=fy_add;
				
				if ((x<0) || (x>=scene->buffer.wid) || (y<0) || (y>=scene->buffer.high)) continue;

				buf=scene->buffer.data+((y*scene->buffer.wid)+x);
				*buf=uc_col;
			}

			return;
		}

			// draw horz line with alpha

		for (x=lx;x<rx;x++) {
		
			y=(int)fy;
			fy+=fy_add;
			
			if ((x<0) || (x>=scene->buffer.wid) || (y<0) || (y>=scene->buffer.high)) continue;

			buf=scene->buffer.data+((y*scene->buffer.wid)+x);
			
			ray_create_float_color_from_ulong_no_alpha(*buf,&buf_col);

			inv_a=1.0f-overlay->tint.a;
			buf_col.r=(buf_col.r*inv_a)+(overlay->tint.r*overlay->tint.a);
			buf_col.g=(buf_col.g*inv_a)+(overlay->tint.g*overlay->tint.a);
			buf_col.b=(buf_col.b*inv_a)+(overlay->tint.b*overlay->tint.a);

			*buf=ray_create_ulong_color_from_float_no_alpha(&buf_col);
		}
		
		return;
	}
	
		// vertical
	
	if (ty>by) {
		k=lx;
		lx=rx;
		rx=k;
		k=ty;
		ty=by;
		by=k;
	}
	
	fx=(float)lx;
	fx_add=((float)(rx-lx))/((float)dy);

		// draw vert line with no alpha

	if (overlay->tint.a==1.0f) {
		uc_col=ray_create_ulong_color_from_float_no_alpha(&overlay->tint);

		for (y=ty;y<by;y++) {
		
			x=(int)fx;
			fx+=fx_add;

			if ((x<0) || (x>=scene->buffer.wid) || (y<0) || (y>=scene->buffer.high)) continue;

			buf=scene->buffer.data+((y*scene->buffer.wid)+x);
			*buf=uc_col;
		}

		return;
	}

		// draw vert line with alpha

	for (y=ty;y<by;y++) {
	
		x=(int)fx;
		fx+=fx_add;

		if ((x<0) || (x>=scene->buffer.wid) || (y<0) || (y>=scene->buffer.high)) continue;

		buf=scene->buffer.data+((y*scene->buffer.wid)+x);
		
		ray_create_float_color_from_ulong_no_alpha(*buf,&buf_col);

		inv_a=1.0f-overlay->tint.a;
		buf_col.r=(buf_col.r*inv_a)+(overlay->tint.r*overlay->tint.a);
		buf_col.g=(buf_col.g*inv_a)+(overlay->tint.g*overlay->tint.a);
		buf_col.b=(buf_col.b*inv_a)+(overlay->tint.b*overlay->tint.a);

		*buf=ray_create_ulong_color_from_float_no_alpha(&buf_col);
	}
}

/* =======================================================

      Draws the Overlay into the Scene
	  
	  Returns:
	   RL_ERROR_UNKNOWN_SCENE_ID
      
======================================================= */

int rtlSceneOverlayDraw(int sceneId)
{
	int							n,idx;
	ray_scene_type				*scene;
	ray_overlay_type			*overlay;

//	int msec;
//	msec=GetTickCount();		// supergumba -- testing

		// get scene

	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);

	scene=ray_global.scene_list.scenes[idx];

		// draw the overlays

	for (n=0;n!=scene->overlay_list.count;n++) {
		overlay=scene->overlay_list.overlays[n];
		if (overlay->hidden) continue;

		switch (overlay->overlay_type) {

			case RL_OVERLAY_TYPE_QUAD_COLOR:
				ray_scene_overlay_draw_quad_color(scene,overlay);
				break;

			case RL_OVERLAY_TYPE_QUAD_HORIZONTAL_GRADIENT:
				ray_scene_overlay_draw_quad_horizontal_gradient(scene,overlay);
				break;

			case RL_OVERLAY_TYPE_QUAD_VERTICAL_GRADIENT:
				ray_scene_overlay_draw_quad_vertical_gradient(scene,overlay);
				break;

			case RL_OVERLAY_TYPE_QUAD_MATERIAL:
				ray_scene_overlay_draw_quad_material(scene,overlay);
				break;

			case RL_OVERLAY_TYPE_LINE_COLOR:
				ray_scene_overlay_draw_line_color(scene,overlay);
				break;

		}

	}

//	fprintf(stdout,"%d\n",(GetTickCount()-msec));	// supergumba -- testing

	return(RL_ERROR_OK);
}
