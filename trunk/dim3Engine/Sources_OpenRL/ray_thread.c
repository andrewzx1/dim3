#include "ray_os_defs.h"
#include "ray.h"
#include "ray_interface.h"

extern ray_global_type				ray_global;

/* =======================================================

      Thread Utilities
      
======================================================= */

void ray_scene_clear_threads(ray_scene_type *scene)
{
	int				n;

	for (n=0;n!=ray_global.settings.thread_count;n++) {
		scene->render.thread_info[n].thread=0;
	}
}

void ray_scene_wait_shutdown_threads(ray_scene_type *scene)
{
	int				n;
	bool			not_shutdown;

	while (TRUE) {

		not_shutdown=FALSE;

		for (n=0;n!=ray_global.settings.thread_count;n++) {
			if (!scene->render.thread_info[n].shutdown_done) {
				not_shutdown=TRUE;
				break;
			}
		}
		
		if (!not_shutdown) break;

		usleep(1000);
	}
}

/* =======================================================

      OS Specific Thread Utilities
      
======================================================= */

#ifndef WIN32

void ray_scene_resume_threads(ray_scene_type *scene,int mode)
{
	scene->thread_mode=mode;
	
	pthread_mutex_lock(&scene->render.thread_lock);
	pthread_cond_broadcast(&scene->render.thread_cond);
	pthread_mutex_unlock(&scene->render.thread_lock);
}

void ray_scene_release_threads(ray_scene_type *scene)
{
		// resume in shutdown mode

	ray_scene_resume_threads(scene,ray_thread_mode_shutdown);

		// wait for shutdown_done

	ray_scene_wait_shutdown_threads(scene);

		// release condition and mutex

	pthread_mutex_destroy(&scene->render.thread_lock);
	pthread_cond_destroy(&scene->render.thread_cond);
}

bool ray_scene_create_threads(ray_scene_type *scene)
{
	int				n;

		// start in suspended mode

	scene->thread_mode=ray_thread_mode_suspend;

		// the mutex and condition

	if (pthread_mutex_init(&scene->render.thread_lock,NULL)!=0) return(FALSE);

	if (pthread_cond_init(&scene->render.thread_cond,NULL)!=0) {
		pthread_mutex_destroy(&scene->render.thread_lock);
		return(FALSE);
	}

		// create threads

	for (n=0;n!=ray_global.settings.thread_count;n++) {

		if (pthread_create(&scene->render.thread_info[n].thread,0,ray_render_thread,(void*)&scene->render.thread_info[n])!=0) {
			pthread_mutex_destroy(&scene->render.thread_lock);
			pthread_cond_destroy(&scene->render.thread_cond);
			return(FALSE);
		}

		pthread_detach(scene->render.thread_info[n].thread);
	}

	return(TRUE);
}

#else

void ray_scene_resume_threads(ray_scene_type *scene,int mode)
{
	int				n;

	scene->thread_mode=mode;

	for (n=0;n!=ray_global.settings.thread_count;n++) {
		if (scene->render.thread_info[n].thread!=0) ResumeThread(scene->render.thread_info[n].thread);
	}
}

void ray_scene_release_threads(ray_scene_type *scene)
{
	int				n;

		// resume in shutdown mode

	ray_scene_resume_threads(scene,ray_thread_mode_shutdown);

		// wait for shutdown_done

	ray_scene_wait_shutdown_threads(scene);

		// close threads

	for (n=0;n!=ray_global.settings.thread_count;n++) {
		if (scene->render.thread_info[n].thread!=0) CloseHandle(scene->render.thread_info[n].thread);
	}
}

bool ray_scene_create_threads(ray_scene_type *scene)
{
	int				n;

		// start in suspended mode

	scene->thread_mode=ray_thread_mode_suspend;

		// create threads

	for (n=0;n!=ray_global.settings.thread_count;n++) {
		scene->render.thread_info[n].thread=(HANDLE)_beginthreadex(NULL,0,ray_render_thread,(void*)&scene->render.thread_info[n],0,NULL);
		if (scene->render.thread_info[n].thread==0) return(FALSE);
	}

	return(TRUE);
}

#endif

