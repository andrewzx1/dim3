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

      Setup Eye

	  Note:
	   Rotate matrix is in column major order
	   If the scene is currently rendering, this API
	   will stall until it's finished
	   
 	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_SCENE_ID
     
======================================================= */

int rtlSceneEyePositionSet(int sceneId,ray_point_type *pnt,ray_matrix_type *rot_matrix,float eye_min_dist,float eye_max_dist)
{
	int				idx;
	ray_scene_type	*scene;
	
		// get scene

	idx=ray_scene_get_index(sceneId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_SCENE_ID);
	
	scene=ray_global.scene_list.scenes[idx];

		// stall rendering so it finishes
		
	ray_render_stall(scene);

		// set eye position

	memmove(&scene->eye.pnt,pnt,sizeof(ray_point_type));
	memmove(&scene->eye.matrix,rot_matrix,sizeof(ray_matrix_type));
	
	scene->eye.min_dist=eye_min_dist;
	scene->eye.max_dist=eye_max_dist;
	
	return(RL_ERROR_OK);
}


