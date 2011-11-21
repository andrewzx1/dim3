/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: View VBO Routines

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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "interface.h"

extern map_type				map;
extern editor_setup_type	setup;
extern editor_state_type	state;

/* =======================================================

      VBO Mesh Setup
      
======================================================= */

void view_vbo_mesh_rebuild(int mesh_idx)
{
	int						k,t,cnt;
	float					*pv;
	d3pnt					*pnt;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;

	mesh=&map.mesh.meshes[mesh_idx];
	
	if (mesh->npoly==0) return;
	
	map_prepare_mesh_box(mesh);


		// get the total vertex count
		// and calculate the min/max/center
		// to speed up some drawing operations

	cnt=0;
	poly=mesh->polys;

//	mesh->box.mid.x=mesh->box.mid.y=mesh->box.mid.z=0;

	for (k=0;k!=mesh->npoly;k++) {

			// remember poly index for drawing

		poly->draw.vertex_offset=cnt;

			// calc center

	//	poly->box.mid.x=poly->box.mid.y=poly->box.mid.z=0;

	//	for (t=0;t!=poly->ptsz;t++) {
			pnt=&mesh->vertexes[poly->v[t]];
		//	poly->box.mid.x+=pnt->x;
		//	poly->box.mid.y+=pnt->y;
		//	poly->box.mid.z+=pnt->z;
	//	}

	////	poly->box.mid.x/=poly->ptsz;
	//	poly->box.mid.y/=poly->ptsz;
	//	poly->box.mid.z/=poly->ptsz;
		
		map_prepare_mesh_poly(&map,mesh,poly);

//		mesh->box.mid.x+=poly->box.mid.x;
//		mesh->box.mid.y+=poly->box.mid.y;
//		mesh->box.mid.z+=poly->box.mid.z;

			// next poly index

		cnt+=poly->ptsz;
		poly++;
	}

		// finish with mesh center

//	mesh->box.mid.x/=mesh->npoly;
//	mesh->box.mid.y/=mesh->npoly;
//	mesh->box.mid.z/=mesh->npoly;
	

		// setup the VBO

	mesh->vbo.vertex_count=cnt;

	glBindBuffer(GL_ARRAY_BUFFER,mesh->vbo.vertex);

	glBufferData(GL_ARRAY_BUFFER,((mesh->vbo.vertex_count*(3+2+2))*sizeof(float)),NULL,GL_DYNAMIC_DRAW);
	pv=(float*)glMapBuffer(GL_ARRAY_BUFFER,GL_WRITE_ONLY);
	if (pv==NULL) return;

	poly=mesh->polys;

	for (k=0;k!=mesh->npoly;k++) {

		for (t=0;t!=poly->ptsz;t++) {

			pnt=&mesh->vertexes[poly->v[t]];
			*pv++=(float)pnt->x;
			*pv++=(float)pnt->y;
			*pv++=(float)pnt->z;

			*pv++=poly->main_uv.x[t];
			*pv++=poly->main_uv.y[t];

			*pv++=poly->lmap_uv.x[t];
			*pv++=poly->lmap_uv.y[t];
		}

		poly++;
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER,0);
}

void view_vbo_mesh_initialize(int mesh_idx)
{
	glGenBuffers(1,&map.mesh.meshes[mesh_idx].vbo.vertex);
	view_vbo_mesh_rebuild(mesh_idx);
}

void view_vbo_mesh_free(int mesh_idx)
{
	glDeleteBuffers(1,&map.mesh.meshes[mesh_idx].vbo.vertex);
}

/* =======================================================

      VBO Initialize/Free
      
======================================================= */

void view_vbo_map_initialize(void)
{
	int						n;

	for (n=0;n!=map.mesh.nmesh;n++) {
		view_vbo_mesh_initialize(n);
	}
}

void view_vbo_map_free(void)
{
	int						n;
	
	for (n=0;n!=map.mesh.nmesh;n++) {
		view_vbo_mesh_free(n);
	}
}

