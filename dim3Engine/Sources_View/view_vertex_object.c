/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Vertex Objects

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit �Created with dim3 Technology� is given on a single
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

// supergumba -- move this

#define view_vertex_object_count			8

extern map_type				map;
extern view_type			view;
extern setup_type			setup;

int							cur_vbo_cache_idx;
bool						vbo_locked[view_vertex_object_count];
GLuint						vbo_cache[view_vertex_object_count];

/* =======================================================

      Create and Dispose Vertex Objects
      
======================================================= */

void view_create_vertex_objects(void)
{
	int				n;
	
	cur_vbo_cache_idx=0;
	glGenBuffersARB(view_vertex_object_count,vbo_cache);
	
		// vbos can be locked and kept out of
		// rotation if they are used more than once
		
	for (n=0;n!=view_vertex_object_count;n++) {
		vbo_locked[n]=FALSE;
	}
}

void view_dispose_vertex_objects(void)
{
	glDeleteBuffersARB(view_vertex_object_count,vbo_cache);
}

/* =======================================================

      Use Map OpenGL Lists
      
======================================================= */

inline void view_next_vertex_object(void)
{
	int			org_idx;

	org_idx=cur_vbo_cache_idx;

		// based on the code, all vbos should never be
		// locked, but just in case (some weird condition)
		// we only loop round once
		
	while (TRUE) {

		cur_vbo_cache_idx++;
		if (cur_vbo_cache_idx==view_vertex_object_count) cur_vbo_cache_idx=0;
		
			// is it unlocked?

		if (!vbo_locked[cur_vbo_cache_idx]) break;

			// have we gone in a complete circle?

		if (cur_vbo_cache_idx==org_idx) break;
	}
}

inline void view_bind_current_vertex_object(void)
{
	glBindBufferARB(GL_ARRAY_BUFFER_ARB,vbo_cache[cur_vbo_cache_idx]);
}

inline void view_bind_specific_vertex_object(int vbo_cache_idx)
{
	glBindBufferARB(GL_ARRAY_BUFFER_ARB,vbo_cache[vbo_cache_idx]);
}

inline void view_unbind_current_vertex_object(void)
{
	glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);
}

inline int view_lock_current_vertext_object(void)
{
	vbo_locked[cur_vbo_cache_idx]=TRUE;
	return(cur_vbo_cache_idx);
}

inline void view_unlock_current_vertext_object(int vbo_cache_idx)
{
	vbo_locked[vbo_cache_idx]=FALSE;
}


