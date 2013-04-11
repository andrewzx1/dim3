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

	if ((overlayType!=RL_OVERLAY_TYPE_QUAD_COLOR) && (overlayType!=RL_OVERLAY_TYPE_QUAD_MATERIAL) && (overlayType!=RL_OVERLAY_TYPE_LINE_COLOR)) return(RL_ERROR_UNKNOWN_OVERLAY_TYPE);

		// create new overlay

	overlay=(ray_overlay_type*)malloc(sizeof(ray_overlay_type));
	if (overlay==NULL) return(RL_ERROR_OUT_OF_MEMORY);

		// add overlay
		
	overlay->material_idx=-1;
	overlay->overlay_type=overlayType;
	overlay->hidden=FALSE;
	
	overlay->tint.r=1.0f;
	overlay->tint.g=1.0f;
	overlay->tint.b=1.0f;
	overlay->tint.a=1.0f;

		// default for types

	switch (overlayType) {

		case RL_OVERLAY_TYPE_QUAD_COLOR:
		case RL_OVERLAY_TYPE_QUAD_MATERIAL:
			overlay->setup.quad.pnt.x=0;
			overlay->setup.quad.pnt.y=0;
			overlay->setup.quad.pnt_size.x=0;
			overlay->setup.quad.pnt_size.y=0;
			overlay->setup.quad.uv.x=0.0f;
			overlay->setup.quad.uv.y=0.0f;
			overlay->setup.quad.uv_size.x=1.0f;
			overlay->setup.quad.uv_size.y=1.0f;
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

		// reset material
		
	material_idx=ray_material_get_index(materialId);
	if (material_idx==-1) return(RL_ERROR_UNKNOWN_MATERIAL_ID);
	
	overlay->material_idx=material_idx;

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
		
	overlay->setup.quad.pnt.x=pnt->x;
	overlay->setup.quad.pnt.y=pnt->y;
	overlay->setup.quad.pnt_size.x=size->x;
	overlay->setup.quad.pnt_size.y=size->y;

	return(RL_ERROR_OK);
}

/* =======================================================

      Changes UV of Quad Overlay Already in a Scene

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
	   RL_ERROR_UNKNOWN_OVERLAY_ID
      
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
		
	overlay->setup.quad.uv.x=uv->x;
	overlay->setup.quad.uv.y=uv->y;
	overlay->setup.quad.uv_size.x=stamp->x;
	overlay->setup.quad.uv_size.y=stamp->y;

	return(RL_ERROR_OK);
}

/* =======================================================

      Changes Position of Line Overlay Already in a Scene

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
	   RL_ERROR_UNKNOWN_OVERLAY_ID
      
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

      Draws the Overlay into the Scene
	  
	  Returns:
	   RL_ERROR_UNKNOWN_SCENE_ID
      
======================================================= */

void ray_scene_overlay_draw_quad_material(ray_scene_type *scene,ray_overlay_type *overlay)
{
	int							n,wid,mm_level,
								x,y,lx,rx,ty,by,px,py,
								y_buf_offset;
	float						f,fx,fy,inv_a;
	unsigned long				*buf;
	unsigned long				mat_buf;
	ray_material_type			*material;
	ray_material_mipmap_type	*mipmap;
	ray_color_type				buf_col,overlay_col;

		// get the mipmap level
		
	if (overlay->material_idx==-1) return;
	material=ray_global.material_list.materials[overlay->material_idx];
	
	wid=(overlay->setup.quad.pnt_size.x*scene->buffer.wid)/scene->overlay_scale.x;
	wid=(int)(((float)wid)*(1.0f/(overlay->setup.quad.uv_size.x-overlay->setup.quad.uv.x)));

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

	lx=overlay->setup.quad.pnt.x;
	rx=lx+overlay->setup.quad.pnt_size.x;
	ty=overlay->setup.quad.pnt.y;
	by=ty+overlay->setup.quad.pnt_size.y;
	
		// scale it
		
	lx=(lx*scene->buffer.wid)/scene->overlay_scale.x;
	rx=(rx*scene->buffer.wid)/scene->overlay_scale.x;
	ty=(ty*scene->buffer.high)/scene->overlay_scale.y;
	by=(by*scene->buffer.high)/scene->overlay_scale.y;

		// draw quad
		
	for (y=ty;y<by;y++) {

			// buffer line

		buf=scene->buffer.data+((y*scene->buffer.wid)+lx);

			// get y texture coordinate
			
		f=(float)(y-ty)/(float)(by-ty);
		fy=overlay->setup.quad.uv.y+((overlay->setup.quad.uv_size.y)*f);

		fy-=floorf(fy);
		py=(int)(fy*mipmap->high_scale);

		y_buf_offset=mipmap->wid*py;

			// draw the row

		for (x=lx;x<rx;x++) {

				// get x texture coordinate
				
			f=(float)(x-lx)/(float)(rx-lx);
			fx=overlay->setup.quad.uv.x+((overlay->setup.quad.uv_size.x)*f);
			
			fx-=floorf(fx);
			px=(int)(fx*mipmap->wid_scale);
		
				// get color
		
			mat_buf=*(((unsigned long*)mipmap->data.color)+(y_buf_offset+px));
			ray_create_float_color_from_ulong(mat_buf,&overlay_col);

				// multiply in the tint

			overlay_col.r*=overlay->tint.r;
			overlay_col.g*=overlay->tint.g;
			overlay_col.b*=overlay->tint.b;
			overlay_col.a*=overlay->tint.a;

				// overwrite if alpha=1.0f

			if (overlay_col.a==1.0f) {
				*buf=ray_create_ulong_color_from_float_no_alpha(&overlay_col);
				buf++;
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
	int							x,y,lx,rx,ty,by;
	float						inv_a;
	unsigned long				*buf;
	ray_color_type				buf_col;

		// quad size

	lx=overlay->setup.quad.pnt.x;
	rx=lx+overlay->setup.quad.pnt_size.x;
	ty=overlay->setup.quad.pnt.y;
	by=ty+overlay->setup.quad.pnt_size.y;
	
		// scale it
		
	lx=(lx*scene->buffer.wid)/scene->overlay_scale.x;
	rx=(rx*scene->buffer.wid)/scene->overlay_scale.x;
	ty=(ty*scene->buffer.high)/scene->overlay_scale.y;
	by=(by*scene->buffer.high)/scene->overlay_scale.y;

		// draw quad
		
	for (y=ty;y<by;y++) {

		buf=scene->buffer.data+((y*scene->buffer.wid)+lx);

		for (x=lx;x<rx;x++) {

				// overwrite if alpha=1.0f

			if (overlay->tint.a==1.0f) {
				*buf=ray_create_ulong_color_from_float_no_alpha(&overlay->tint);
				buf++;
				continue;
			}

				// skip if no alpha

			if (overlay->tint.a==0.0f) {
				buf++;
				continue;
			}

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

void ray_scene_overlay_draw_line_color(ray_scene_type *scene,ray_overlay_type *overlay)
{
	int							k,x,y,dx,dy,lx,rx,ty,by;
	float						fx,fy,fx_add,fy_add,inv_a;
	unsigned long				*buf;
	ray_color_type				buf_col;
	
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
	
		for (x=lx;x<rx;x++) {
		
			y=(int)fy;
			buf=scene->buffer.data+((y*scene->buffer.wid)+x);
			
			ray_create_float_color_from_ulong_no_alpha(*buf,&buf_col);

			inv_a=1.0f-overlay->tint.a;
			buf_col.r=(buf_col.r*inv_a)+(overlay->tint.r*overlay->tint.a);
			buf_col.g=(buf_col.g*inv_a)+(overlay->tint.g*overlay->tint.a);
			buf_col.b=(buf_col.b*inv_a)+(overlay->tint.b*overlay->tint.a);

			*buf=ray_create_ulong_color_from_float_no_alpha(&buf_col);

			fy+=fy_add;
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

	for (y=ty;y<by;y++) {
	
		x=(int)fx;
		buf=scene->buffer.data+((y*scene->buffer.wid)+x);
		
		ray_create_float_color_from_ulong_no_alpha(*buf,&buf_col);

		inv_a=1.0f-overlay->tint.a;
		buf_col.r=(buf_col.r*inv_a)+(overlay->tint.r*overlay->tint.a);
		buf_col.g=(buf_col.g*inv_a)+(overlay->tint.g*overlay->tint.a);
		buf_col.b=(buf_col.b*inv_a)+(overlay->tint.b*overlay->tint.a);

		*buf=ray_create_ulong_color_from_float_no_alpha(&buf_col);

		fx+=fx_add;
	}
}

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

			case RL_OVERLAY_TYPE_QUAD_MATERIAL:
				ray_scene_overlay_draw_quad_material(scene,overlay);
				break;

			case RL_OVERLAY_TYPE_LINE_COLOR:
				ray_scene_overlay_draw_line_color(scene,overlay);
				break;

		}

	}

//	fprintf(stdout,"%d\n",(GetTickCount()-msec));	// supergumba

	return(RL_ERROR_OK);
}
