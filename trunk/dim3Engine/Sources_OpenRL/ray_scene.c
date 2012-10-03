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

      Scene Mesh Indexes
      
======================================================= */

void ray_scene_free_mesh_indexes(ray_scene_type *scene)
{
	int				n;

	for (n=0;n!=ray_render_max_thread_count;n++) {
		if (scene->render.thread_info[n].mesh_index_block.indexes!=NULL) free(scene->render.thread_info[n].mesh_index_block.indexes);
	}

}

bool ray_scene_initialize_mesh_indexes(ray_scene_type *scene)
{
	int				n;
	
	for (n=0;n!=ray_render_max_thread_count;n++) {
		scene->render.thread_info[n].mesh_index_block.indexes=NULL;
	}
	
	for (n=0;n!=ray_render_max_thread_count;n++) {
		scene->render.thread_info[n].mesh_index_block.indexes=(short*)malloc(ray_max_scene_mesh*sizeof(short));
		if (scene->render.thread_info[n].mesh_index_block.indexes==NULL) return(FALSE);
	}
	
	return(TRUE);
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

int rlSceneAdd(int wid,int high,int target,int format,void *attachment,unsigned long flags)
{
	int					n,y,y_add;
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
	scene->buffer.wid=wid;
	scene->buffer.high=high;

	scene->buffer.data=(unsigned long*)malloc((wid<<2)*high);
	if (scene->buffer.data==NULL) {
		free(scene);
		return(RL_ERROR_OUT_OF_MEMORY);
	}
	
		// space for mesh indexes
		
	if (!ray_scene_initialize_mesh_indexes(scene)) {
		ray_scene_free_mesh_indexes(scene);
		free(scene->buffer.data);
		free(scene);
		return(RL_ERROR_OUT_OF_MEMORY);
	}
	
		// precalc any thread info settings,
		// like parent pointer or y splits
		
	y=0;
	y_add=high/ray_global.settings.thread_count;

	for (n=0;n!=ray_global.settings.thread_count;n++) {

		scene->render.thread_info[n].y_start=y;
		
		y+=y_add;
		if (n==(ray_global.settings.thread_count-1)) y=scene->buffer.high;
		scene->render.thread_info[n].y_end=y;
		
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

		// create the scene lock mutex

#ifndef WIN32
	if (pthread_mutex_init(&scene->render.lock,NULL)==-1) {
#else
	scene->render.lock=CreateMutex(NULL,FALSE,NULL);
	if (scene->render.lock==NULL) {
#endif
		ray_scene_free_mesh_indexes(scene);
		free(scene->buffer.data);
		free(scene);
		return(RL_ERROR_THREADING_ERROR);
	}

		// set the id
		
	scene->id=ray_global.scene_list.next_id;
	ray_global.scene_list.next_id++;
	
		// add to list

	ray_global.scene_list.scenes[ray_global.scene_list.count]=scene;
	ray_global.scene_list.count++;
	
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

		// clear scene memory

	scene=ray_global.scene_list.scenes[idx];
	
	ray_scene_free_mesh_indexes(scene);
	free(scene->buffer.data);

		// clear render lock

#ifndef WIN32
	pthread_mutex_destroy(&scene->render.lock);
#else
	CloseHandle(scene->render.lock);
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

