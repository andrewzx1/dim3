


int main(int argc,char *argv[])
{
	int						wid,high,sdl_flags;
	SDL_DisplayMode			sdl_mode;
	SDL_Window					*sdl_wind;
	SDL_GLContext				*sdl_gl_ctx;
	
	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_JOYSTICK|SDL_INIT_NOPARACHUTE)==-1) return(0);
	
	SDL_GetDesktopDisplayMode(0,&sdl_mode);
	wid=sdl_mode.w;
	high=sdl_mode.h;
	
		// normal attributes
		
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE,8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE,8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
	
		// start window or full screen

	sdl_flags=SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN|SDL_WINDOW_FULLSCREEN;
	
	sdl_wind=SDL_CreateWindow("dim3",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,wid,high,sdl_flags);
	if (sdl_wind==NULL) return(0);
	
	sdl_gl_ctx=SDL_GL_CreateContext(sdl_wind);
	
	glClearColor(1,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT);
   
	SDL_GL_SwapWindow(sdl_wind);

	sleep(1);

	glClearColor(0,1,0,0);
	glClear(GL_COLOR_BUFFER_BIT);
   
	SDL_GL_SwapWindow(sdl_wind);

	sleep(1);
	glClearColor(0,0,1,0);
	glClear(GL_COLOR_BUFFER_BIT);
   
	SDL_GL_SwapWindow(sdl_wind);

	sleep(1);




	
	SDL_Quit();

	
	
	return(0);
}

