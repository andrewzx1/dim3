#include "ray_os_defs.h"
#include "ray.h"
#include "ray_interface.h"

extern ray_global_type				ray_global;

/* =======================================================

      Find Material Index for ID
      
======================================================= */

int ray_scene_get_index(int sceneId)
{
	int						n;

	for (n=0;n!=ray_global.scene_list.count;n++) {
		if (ray_global.scene_list.scenes[n]->id==sceneId) return(n);
	}

	return(-1);
}

/* =======================================================

      Scene 3D to 2D
      
======================================================= */

void ray_scene_3D_to_2D_point(ray_scene_type *scene,ray_point_type *pnt_3d,ray_2d_point_type *pnt_2d)
{
	float				d,t;
	ray_vector_type		vct;

		// find the vector from the eye to the point
		// and turn it to face the eye plane

	ray_vector_create_from_points(&vct,pnt_3d,&scene->eye.pnt);
	rlMatrixVectorMultiply(&scene->eye.matrix,&vct);

		// solve for z=eye min distance

	d=ray_distance_between_points(&scene->eye.pnt,pnt_3d);
	t=scene->eye.min_dist/d;

		// find the intersection with the
		// eye plane and then center into
		// the view

	pnt_2d->x=((int)(scene->eye.pnt.x*t))+(scene->buffer.wid>>1);
	pnt_2d->y=((int)(scene->eye.pnt.y*t))+(scene->buffer.high>>1);
}

/* =======================================================

      Create a Scene

 	  Returns:
	   If >=0, then a scene ID
	   RL_ERROR_UNKNOWN_TARGET
	   RL_ERROR_UNKNOWN_FORMAT
	   RL_ERROR_OUT_OF_MEMORY
	   RL_ERROR_THREADING_ERROR
     
======================================================= */

int rlSceneAdd(ray_2d_point_type *size,int target,int format,void *attachment,unsigned long flags)
{
	int					n,k,split,x_add,y_add;
	ray_scene_type		*scene;

		// right now only one target/format

	if (target!=RL_SCENE_TARGET_MEMORY) return(RL_ERROR_UNKNOWN_TARGET);
	if (format!=RL_SCENE_FORMAT_32_RGBA) return(RL_ERROR_UNKNOWN_FORMAT);

		// new scene

	scene=(ray_scene_type*)malloc(sizeof(ray_scene_type));
	if (scene==NULL) return(RL_ERROR_OUT_OF_MEMORY);

		// clear thread states

	ray_render_clear_threads(scene);

		// the buffer

	scene->buffer.target=target;
	scene->buffer.format=format;
	scene->buffer.wid=size->x;
	scene->buffer.high=size->y;

	scene->buffer.data=(unsigned long*)malloc((scene->buffer.wid<<2)*scene->buffer.high);
	if (scene->buffer.data==NULL) {
		free(scene);
		return(RL_ERROR_OUT_OF_MEMORY);
	}
	
		// precalc any thread info settings,
		// like parent pointer and the drawing rect

	split=(int)sqrtf(ray_global.settings.thread_count);

	x_add=scene->buffer.wid/split;
	y_add=scene->buffer.high/split;

	for (n=0;n!=ray_global.settings.thread_count;n++) {
	
		k=n%split;
		scene->render.thread_info[n].pixel_start.x=x_add*k;
		scene->render.thread_info[n].pixel_end.x=scene->render.thread_info[n].pixel_start.x+x_add;
		if (k==(split-1)) scene->render.thread_info[n].pixel_end.x=scene->buffer.wid;
		
		k=n/split;
		scene->render.thread_info[n].pixel_start.y=y_add*k;
		scene->render.thread_info[n].pixel_end.y=scene->render.thread_info[n].pixel_start.y+y_add;
		if (k==(split-1)) scene->render.thread_info[n].pixel_end.y=scene->buffer.high;

			// each thread info needs a pointer
			// back to it's scene as thread_info is
			// what's passed to each thread

		scene->render.thread_info[n].parent_scene=scene;
	}

		// setup

	scene->ambient_col.r=scene->ambient_col.g=scene->ambient_col.b=0.0f;

		// any lists

	scene->light_list.count=0;
	scene->light_list.next_id=1;

	scene->mesh_list.count=0;
	scene->mesh_list.next_id=1;
	
	scene->overlay_list.count=0;
	scene->overlay_list.next_id=1;

		// clear threads

	ray_scene_clear_threads(scene);

		// create the scene lock mutex

		// note: for pthread implementations,
		// we need another mutex for pthread suspend/resume

#ifndef WIN32
	if (pthread_mutex_init(&scene->render.scene_lock,NULL)==-1) {
		free(scene->buffer.data);
		free(scene);
		return(RL_ERROR_THREADING_ERROR);
	}
#else
	scene->render.scene_lock=CreateMutex(NULL,FALSE,NULL);
	if (scene->render.scene_lock==NULL) {
		free(scene->buffer.data);
		free(scene);
		return(RL_ERROR_THREADING_ERROR);
	}
#endif

		// set the id
		
	scene->id=ray_global.scene_list.next_id;
	ray_global.scene_list.next_id++;
	
		// add to list

	ray_global.scene_list.scenes[ray_global.scene_list.count]=scene;
	ray_global.scene_list.count++;

		// build the threads

	if (!ray_scene_create_threads(scene)) {
		rlSceneDelete(scene->id);
		return(RL_ERROR_THREADING_ERROR);
	}
	
	return(scene->id);
}

/* =======================================================

      Deletes a Scene

 	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
	   RL_ERROR_SCENE_IN_USE
     
======================================================= */

int rlSceneDelete(int sceneId)
{
	int				n,idx,count;
	ray_scene_type	*scene;
	
		// get scene

	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);

		// can not delete if in rendering

	if (rlSceneRenderState(sceneId)==RL_SCENE_STATE_RENDERING) return(RL_ERROR_SCENE_IN_USE);

	scene=ray_global.scene_list.scenes[idx];

		// exit the threads

	ray_scene_release_threads(scene);

		// clear scene memory
	
	free(scene->buffer.data);

		// clear render lock

#ifndef WIN32
	pthread_mutex_destroy(&scene->render.scene_lock);
#else
	CloseHandle(scene->render.scene_lock);
#endif

		// clear lights and meshes

	rlSceneLightDeleteAll(sceneId);
	rlSceneMeshDeleteAll(sceneId);
	rlSceneOverlayDeleteAll(sceneId);

		// delete scene

	free(scene);

	count=ray_global.scene_list.count-2;

	for (n=idx;n<=count;n++) {
		ray_global.scene_list.scenes[n]=ray_global.scene_list.scenes[n+1];
	}

	ray_global.scene_list.count--;

	return(RL_ERROR_OK);
}

/* =======================================================

      Clears a Scene

 	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
	   RL_ERROR_SCENE_IN_USE
     
======================================================= */

int rlSceneClearBuffer(int sceneId,ray_color_type *col)
{
	int					n,idx,sz;
	unsigned long		*data,ul;
	ray_scene_type		*scene;
	
		// get scene
	
	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);

	scene=ray_global.scene_list.scenes[idx];

		// can not clear if in rendering

	if (rlSceneRenderState(sceneId)==RL_SCENE_STATE_RENDERING) return(RL_ERROR_SCENE_IN_USE);
	
		// buffer clear color

	ul=ray_create_ulong_color_from_float(col);
	
		// clear the buffer
		
	sz=scene->buffer.wid*scene->buffer.high;
	data=scene->buffer.data;
	
	for (n=0;n!=sz;n++) {
		*data++=ul;
	}
	
	return(RL_ERROR_OK);
}

/* =======================================================

      Gets the Buffer Attached to a Scene if Possible

 	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
     
======================================================= */

int rlSceneGetBuffer(int sceneId,void **buffer)
{
	int			idx;
	
	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);

	*buffer=ray_global.scene_list.scenes[idx]->buffer.data;
	return(RL_ERROR_OK);
}

