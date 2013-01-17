//
// dim3RTL
// by Brian Barnes (bbarnes@klinksoftware.com)
// free software offered as is for any use
//

#include "ray_os_defs.h"
#include "ray.h"
#include "ray_interface.h"

extern ray_global_type				ray_global;

/* =======================================================

      dim3RTL Initialize

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_OUT_OF_MEMORY
      
======================================================= */

int rtlInitialize(void)
{
	int					thread_count;
#ifdef __APPLE__
	int					names[2];
	size_t				len;
	uint32_t			count;
#endif
#ifdef WIN32
	SYSTEM_INFO			info;
#endif

		// initialize the scene list

	ray_global.scene_list.count=0;
	ray_global.scene_list.next_id=1;

		// initialize the material list

	ray_global.material_list.count=0;
	ray_global.material_list.next_id=1;

		// determine the number of
		// cores avaiable

#ifdef __APPLE__
	names[0]=CTL_HW;
	names[1]=HW_AVAILCPU;
	len=4;
	
    sysctl(names,2,&count,&len,NULL,0);

    if (count<1) {
        names[1]=HW_NCPU;
        sysctl(names,2,&count,&len,NULL,0);
        if (count<1) count=1;
    }
	
	thread_count=count;
#endif

#ifdef __linux__
	thread_count=sysconf(_SC_NPROCESSORS_ONLN);
#endif

#ifdef WIN32
	GetSystemInfo(&info);
	thread_count=info.dwNumberOfProcessors;
#endif

		// reverve some threads
		// threads always need to be a power
		// of 2 to equally split the screen
		
		// for now, we run this relatively
		// hardcoded setup.  Will need
		// better math here
		
	if (thread_count<=4) {
		ray_global.settings.thread_count=16;
	}
	else {
		if (thread_count<=8) {
			ray_global.settings.thread_count=25;
		}
		else {
			ray_global.settings.thread_count=ray_render_max_thread_count;
		}
	}

	return(RL_ERROR_OK);
}

/* =======================================================

      dim3RTL Shutdown

	  Returns:
	   RL_ERROR_OK
      
======================================================= */

int rtlShutdown(void)
{
	int				id,err;

		// free scenes

	while (ray_global.scene_list.count!=0) {

		id=ray_global.scene_list.scenes[0]->id;

			// need to wait out any rendering

		while (rtlSceneRenderState(id)==RL_SCENE_STATE_RENDERING) {
			usleep(100);
		}

			// delete scene

		err=rtlSceneDelete(id);
		if (err!=RL_ERROR_OK) return(err);
	}

		// free materials

	while (ray_global.material_list.count!=0) {
		err=rtlMaterialDelete(ray_global.material_list.materials[0]->id);
		if (err!=RL_ERROR_OK) return(err);
	}
	
	return(RL_ERROR_OK);
}

