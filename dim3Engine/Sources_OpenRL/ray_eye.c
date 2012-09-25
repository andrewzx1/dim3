#include "ray_os_defs.h"
#include "ray.h"
#include "ray_interface.h"

ray_global_type					ray_global;

/* =======================================================

      Setup Eye

	  Note: Rotate matrix is in column major order
      
======================================================= */

int rlSceneEyePositionSet(int sceneId,ray_point_type *pnt,ray_matrix_type *rot_matrix,float eye_min_dist,float eye_max_dist)
{
	int				idx;
	ray_scene_type	*scene;
	
		// get scene

	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);

		// can not set if in rendering

	if (rlSceneRenderState(sceneId)==RL_SCENE_STATE_RENDERING) return(RL_ERROR_SCENE_IN_USE);

		// set eye position

	scene=ray_global.scene_list.scenes[idx];

	memmove(&scene->eye.pnt,pnt,sizeof(ray_point_type));
	memmove(&scene->eye.matrix,rot_matrix,sizeof(ray_matrix_type));
	
	scene->eye.min_dist=eye_min_dist;
	scene->eye.max_dist=eye_max_dist;
	
	return(RL_ERROR_OK);
}


