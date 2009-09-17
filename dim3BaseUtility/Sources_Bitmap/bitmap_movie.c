/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Movies to Textures

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
application screen and in a single piece of the documentation
3. It is not resold, in it's current form or modified, as an
engine-only product

This code is presented as is. The author of dim3 takes no
responsibilities for any version of this code.

Any non-engine product (games, etc) created with this code is free
from any and all payment and/or royalties to the author of dim3,
and can be sold or given away.

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3baseutility.h"
#endif

#ifdef D3_OS_MAC
Movie							movie;
CGContextRef					movie_bitmap_ctx;
char							*movie_bitmap_data;
GLuint							movie_gl_id;
#endif

/* =======================================================

      Movie Play Start and Stop
      
======================================================= */

bool bitmap_movie_start(char *path,int *wid,int *high)
{
#ifndef D3_OS_MAC
	return(FALSE);
#else
	int					err,psz,row_byte_sz;
	short				ref_num;
	float				fx,fy;
	FSSpec				fspec;
	FSRef				fsref;
	Rect				box,movie_box;
	MatrixRecord		matrix;
	CGColorSpaceRef		color_space;
	
		// start movies
		
	err=EnterMovies();
	if (err!=noErr) return(FALSE);
	
		// load the movie
		
	FSPathMakeRef((unsigned char*)path,&fsref,NULL);
	FSGetCatalogInfo(&fsref,kFSCatInfoNone,NULL,NULL,&fspec,NULL);
	
	err=OpenMovieFile(&fspec,&ref_num,fsRdPerm);
	if (err!=noErr) {
		ExitMovies();
		return(FALSE);
	}
	
	err=NewMovieFromFile(&movie,ref_num,NULL,NULL,newMovieActive,NULL);
	CloseMovieFile(ref_num);
	if (err!=noErr) {
		ExitMovies();
		return(FALSE);
	}
	
		// data for bitmap

	row_byte_sz=movie_wid<<2;
	data_sz=row_byte_sz*movie_high;
	
	movie_bitmap_data=malloc(data_sz);
	if (movie_bitmap_data==NULL) {
		ExitMovies();
		return(FALSE);
	}

	bzero(movie_bitmap_data,data_sz);
	
		// create bitmap context
		
	color_space=CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
	movie_bitmap_ctx=CGBitmapContextCreate(movie_bitmap_data,movie_wid,movie_high,8,row_byte_sz,color_space,kCGImageAlphaPremultipliedLast);
	CGColorSpaceRelease(color_space);
	
	SetMovieGWorld(movie,(CGrafPtr)movie_bitmap_ctx,NULL);
	
		// scale the movie to the gworld
		
	GetMovieBox(movie,&movie_box);

	*wid=movie_box.right-movie_box.left;
	*high=movie_box.bottom-movie_box.top;
	
	fx=(float)movie_wid/(float)(movie_box.right-movie_box.left);
	fy=(float)movie_high/(float)(movie_box.bottom-movie_box.top);
	
	SetIdentityMatrix(&matrix);
	ScaleMatrix(&matrix,X2Fix(fx),X2Fix(fy),X2Fix(0.0),X2Fix(0.0));
	SetMovieMatrix(movie,&matrix);
	
	SetMovieBox(movie,&box);
	
		// setup the texture
		
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1,&movie_gl_id);
	
    glBindTexture(GL_TEXTURE_2D,movie_gl_id);
	
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	
		// load texture
		
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,movie_wid,movie_high,0,GL_RGBA,GL_UNSIGNED_BYTE,movie_data);
	
		// start the movie playing
		
	GoToBeginningOfMovie(movie);
	StartMovie(movie);
	
	return(TRUE);
#endif
}

void bitmap_movie_end(void)
{
#ifdef D3_OS_MAC
	StopMovie(movie);
	
	CGContextRelease(movie_bitmap_ctx);
	free(movie_bitmap_data);

	ExitMovies();
#endif
}

/* =======================================================

      Movie Refresh Texture
      
======================================================= */

int bitmap_movie_get_texture_id(void)
{
#ifndef D3_OS_MAC
	return(-1);
#else
	return(movie_gl_id);
#endif
}

bool bitmap_movie_playing(void)
{
#ifndef D3_OS_MAC
	return(FALSE);
#else
	return(!IsMovieDone(movie));
#endif
}

void bitmap_movie_texture_refresh(void)
{
#ifndef D3_OS_MAC
	return;
#else
		// update the movie
		
	MoviesTask(movie,10);
		
		// change the texture
		
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,movie_gl_id);
		
	glTexSubImage2D(GL_TEXTURE_2D,0,0,0,movie_wid,movie_high,GL_RGBA,GL_UNSIGNED_BYTE,movie_bitmap_data);
#endif
}

