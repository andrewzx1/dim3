/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View MainLine

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit ÒCreated with dim3 TechnologyÓ is given on a single
application screen and in a single piece of the documentation
3. It is not resold, in it's current form or modified, as an
engine-only product

This code is presented as is. The author of dim3 takes no
responsibilities for any version of this code.

Any non-engine product (games, etc) created with this code is free
from any and all payment and/or royalties to the author of dim3,
and can be sold or given away.

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "objects.h"
#include "remotes.h"
#include "weapons.h"
#include "projectiles.h"
#include "lights.h"
#include "effects.h"
#include "cameras.h"
#include "consoles.h"
#include "interfaces.h"
#include "xmls.h"
#include "video.h"
#include "sounds.h"
#include "inputs.h"
#include "timing.h"

view_type					view;
view_render_type			view_camera_render,view_node_render;
render_info_type			render_info;

extern map_type				map;
extern server_type			server;
extern setup_type			setup;
extern hud_type				hud;
extern network_setup_type	net_setup;

extern void game_file_initialize(void);
extern void menu_input(void);
extern void file_input(void);
extern void debug_input(void);
extern void view_draw(void);
extern void chat_clear_messages(void);
extern bool fog_solid_on(void);
extern bool shadow_initialize(void);
extern void shadow_shutdown(void);
extern void menu_draw(void);

/* =======================================================

      View Memory
      
======================================================= */

bool view_memory_allocate(void)
{
		// initialize pointers
		
	view.images=NULL;
	view.rain_draws=NULL;
	view.obscure.grid=NULL;
	
		// view pointers

	view.images=(view_image_type*)malloc(max_view_image*sizeof(view_image_type));
	if (view.images==NULL) return(FALSE);

	view.rain_draws=(rain_draw_type*)malloc(max_rain_density*sizeof(rain_draw_type));
	if (view.rain_draws==NULL) return(FALSE);

	view.obscure.grid=(unsigned char*)malloc(obscure_grid_byte_size);
	if (view.obscure.grid==NULL) return(FALSE);
	
		// clear pointers

	bzero(view.images,(max_view_image*sizeof(view_image_type)));
	bzero(view.rain_draws,(max_rain_density*sizeof(rain_draw_type)));

		// start with debug off

	view.debug.on=FALSE;
			
	return(TRUE);
}

void view_memory_release(void)
{
	if (view.images!=NULL) free(view.images);
	if (view.rain_draws!=NULL) free(view.rain_draws);
	if (view.obscure.grid!=NULL) free(view.obscure.grid);
}

/* =======================================================

      Screen Size List
      
======================================================= */

void view_create_screen_size_list(void)
{
	int				n,k,i,nscreen_size;
	float			ratio;
	bool			hit;
	SDL_Rect		**modes;
		
	modes=SDL_ListModes(NULL,SDL_OPENGL|SDL_FULLSCREEN);
	
		// if no modes, then 640x480 is the only mode
		
	if ((modes==(SDL_Rect**)0) || (modes==(SDL_Rect**)-1)) {
		render_info.screen_sizes[0].wid=640;
		render_info.screen_sizes[0].high=480;
		render_info.nscreen_size=1;
		return;
	}
	
		// get mode count
		
	nscreen_size=0;
	while (modes[nscreen_size]!=0) {
		nscreen_size++;
	}
	
		// create screen list
		
	k=0;
	
	for (n=(nscreen_size-1);n>=0;n--) {

			// knock out any less than 640x480 or when height >= width

		if (modes[n]->w<640) continue;
		if (modes[n]->h<480) continue;
		if (modes[n]->h>=modes[n]->w) continue;

			// is this screen already in list?

		hit=FALSE;

		for (i=0;i!=k;i++) {
			if ((render_info.screen_sizes[i].wid==modes[n]->w) && (render_info.screen_sizes[i].high==modes[n]->h)) {
				hit=TRUE;
				break;
			}
		}

			// add to list if ratio is equal or
			// better than 4:3 (0.75) and not less
			// then 1:85:1 (0.54)

		if (!hit) {
			ratio=(float)modes[n]->h/(float)modes[n]->w;
			if ((ratio>=0.54) && (ratio<=0.75f)) {
				render_info.screen_sizes[k].wid=modes[n]->w;
				render_info.screen_sizes[k].high=modes[n]->h;

				k++;
				if (k>=max_screen_size) break;
			}
		}
	}
	
	render_info.nscreen_size=k;
}

int view_search_screen_size_list(int wid,int high)
{
	int				n;
	
	for (n=0;n!=render_info.nscreen_size;n++) {
		if ((render_info.screen_sizes[n].wid==wid) && (render_info.screen_sizes[n].high==high)) return(n);
	}
	
	return(-1);
}

/* =======================================================

      View Initialize and Shutdown Display
      
======================================================= */

bool view_initialize_display(char *err_str)
{
		// is screen size legal?
		
	if (view_search_screen_size_list(setup.screen_wid,setup.screen_high)==-1) {
		setup.screen_wid=640;
		setup.screen_high=480;
	}
	
		// start openGL
		
	if (!gl_initialize(setup.screen_wid,setup.screen_high,setup.lock_fps_refresh,setup.fsaa_mode,FALSE,err_str)) {
		view_memory_release();
		SDL_Quit();
		return(FALSE);
	}

		// fix some OpenGL settings if not supported by card

	if (!gl_check_fsaa_ok()) setup.fsaa_mode=fsaa_mode_none;
	if (!gl_check_texture_anisotropic_filter_ok()) setup.anisotropic_mode=anisotropic_mode_none;

		// shadows

	if (!shadow_initialize()) {
		strcpy(err_str,"Out of Memory");
		gl_shutdown();
		SDL_Quit();
		return(FALSE);
	}
	
		// start the shaders

	if (!gl_core_shader_initialize(err_str)) {
		gl_shutdown();
		SDL_Quit();
		return(FALSE);
	}
		
	read_settings_shader();
	if (!gl_user_shader_initialize(err_str)) {
		gl_shutdown();
		SDL_Quit();
		return(FALSE);
	}

		// initialize text
	
	gl_text_initialize();
	
		// back renderer
		
	gl_back_render_initialize();

		// vertex objects
	
	view_create_vertex_objects();

	return(TRUE);
}

void view_shutdown_display(void)
{
	view_dispose_vertex_objects();
	shadow_shutdown();
	gl_core_shader_shutdown();
	gl_user_shader_shutdown();
	gl_back_render_shutdown();
	gl_text_shutdown();
	gl_shutdown();
}

bool view_reset_display(char *err_str)
{
	view_shutdown_display();
	return(view_initialize_display(err_str));
}

/* =======================================================

      View Initialize and Shutdown Main
      
======================================================= */

bool view_initialize(char *err_str)
{
	int				tick,rate;
	
		// clear view structure
		
	memset(&view,0x0,sizeof(view_type));

		// allocate memory
		
	if (!view_memory_allocate()) {
		view_memory_release();
		strcpy(err_str,"Out of Memory");
		return(FALSE);
	}

		// start SDL

	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_JOYSTICK|SDL_INIT_NOPARACHUTE)==-1) {
		view_memory_release();
		sprintf(err_str,"SDL: Could not init (Error: %s)\n",SDL_GetError());
		return(FALSE);
	}
	
		// create screen sizes
		
	view_create_screen_size_list();
	
		// gl initialize

	if (!view_initialize_display(err_str)) return(FALSE);

		// sound initialize
		
	if (!al_initialize(err_str)) {
		view_shutdown_display();
		view_memory_release();
		SDL_Quit();
		return(FALSE);
	}

	al_set_volume(setup.sound_volume);
	al_music_set_volume(setup.music_volume);
	al_music_set_state(setup.music_on);
	
		// intialize the view bitmap lists
		
	view_images_initialize();
	
		// read in the sounds
		
	read_settings_sound();
	
		// connect the input
		
	read_settings_action();
	
	input_initialize(gl_in_window_mode());
	setup_to_input();
	
		// hook up current render
		// to camera render -- some routines like face
		// forward need this to be valid
		
	view.render=&view_camera_render;

		// states

	view.menu.active=FALSE;
	
		// draw and input timing
		// we use raw timing so input and
		// view can work through pauses
		
	tick=game_time_get_raw();

	view.time.input_tick=tick;
	view.time.draw_tick=tick;
	
	rate=render_info.monitor_refresh_rate;
	if (!setup.lock_fps_refresh) rate=max_fps;
	
	if (rate>max_fps) rate=max_fps;
	view.time.draw_time=1000/rate;
	
	return(TRUE);
}

void view_shutdown(void)
{
		// shutdown view

	input_shutdown();
	al_shutdown();
	view_images_shutdown();
	view_shutdown_display();

		// shutdown SDL
		
	SDL_Quit();
	
		// release memory

	view_memory_release();
}

/* =======================================================

      View Game Start and Stop
      
======================================================= */

void view_game_start(void)
{
		// cameras and globals
		
    camera_initialize();
	game_file_initialize();
	
		// load images for hud bitmaps, radar, particles,
		// rings, halos, marks, crosshairs and remote icons
	
	view_images_cached_load();

		// precalculate particles

	particle_precalculate_all();
	
		// clear chat messages
		
	chat_clear_messages();
}

void view_game_stop(void)
{
		// stop any music
	
	al_music_stop();
	
		// free images for hud bitmaps, radar, particles,
		// rings, halos, marks, crosshairs and remote icons
	
	view_images_cached_free();
}

/* =======================================================

      Reset Timing when Loading Saved Game
      
======================================================= */

void view_game_reset_timing(void)
{
	int			tick;
	
	tick=game_time_get_raw();

	view.time.input_tick=tick;
	view.time.draw_tick=tick;
}

/* =======================================================

      View Loop Input
      
======================================================= */

void view_loop_input(void)
{
	int				raw_tick;

		// time to check input?
		// use raw ticks so it works through pauses
		
	raw_tick=game_time_get_raw();
	if (raw_tick<view.time.input_tick) return;

	view.time.input_tick=raw_tick+input_tick_rate;
	
		// pump input
		
	input_event_pump();

		// player input
	
	player_get_input();
	
		// system input
	
	console_input();
	menu_input();
	file_input();
	debug_input();
}

/* =======================================================

      Normal Playing View Draw
      
======================================================= */

void view_loop_draw(void)
{
	int			raw_tick,tick,y_add;

		// time for view draw?
		// use raw ticks so it works through pauses
		
	raw_tick=game_time_get_raw();
	if (raw_tick<view.time.draw_tick) return;

	view.time.draw_tick=raw_tick+view.time.draw_time;

		// shader flag

	view.shader_on=(!setup.disable_shaders) && (gl_check_shader_ok()) && (!map.settings.no_shaders);

		// texture setup
	
	tick=game_time_get();

	map_setup_animated_textures(&map,tick);
	map_mesh_poly_run_shifts(&map,tick);

		// start frame
		
	gl_frame_clear(TRUE);
	
		// squish for open console
		
	if (view.console.on) {
		y_add=(int)(((float)setup.screen.y_sz)*console_screen_percent);
		glViewport(render_info.view_x,(render_info.view_y+y_add),setup.screen.x_sz,(setup.screen.y_sz-y_add));
	}
	
		// draw view

	view_draw();

		// draw hud and interface elements
		// unless we are in a cinema

	if (server.state!=gs_cinema) {
		hud_draw();
		radar_draw();
		network_draw();
	}
	
		// menu

	menu_draw();

		// restore if console open
		// and draw console
		
	if (view.console.on) {
		glViewport(render_info.view_x,render_info.view_y,setup.screen.x_sz,setup.screen.y_sz);
		console_draw();
	}

		// swap frame buffers
	
	gl_frame_swap();

	view.fps.count++;
}

/* =======================================================

      Dedicated Host View Draw
      
======================================================= */

void view_loop_draw_dedicated_host(int tick)
{
	int				raw_tick;

		// time for dedicate host draw
		// use raw ticks so it works through pauses

	raw_tick=game_time_get_raw();
	if (raw_tick<view.time.draw_tick) return;

	view.time.draw_tick=raw_tick+view.time.draw_time;

		// shader flag

	view.shader_on=(!setup.disable_shaders) && (gl_check_shader_ok()) && (!map.settings.no_shaders);

		// draw dedicated host screen

	gl_frame_clear(FALSE);
	network_draw();
	menu_draw();
	gl_frame_swap();
}

/* =======================================================

      View Draw for Screen Captures
      
======================================================= */

void view_capture_draw(char *path)
{
		// shader flag

	view.shader_on=(!setup.disable_shaders) && (gl_check_shader_ok()) && (!map.settings.no_shaders);

		// draw view

	gl_frame_clear(FALSE);
	view_draw();
	
		// make screenshot

	gl_screen_shot(render_info.view_x,render_info.view_y,setup.screen.x_sz,setup.screen.y_sz,TRUE,path);
}

