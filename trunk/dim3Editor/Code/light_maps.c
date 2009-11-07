/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Light Map Routines

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

#include "interface.h"
#include "dialog.h"
#include "common_view.h"

extern map_type					map;

extern file_path_setup_type		file_path_setup;

#define max_light_map_textures				64

typedef struct		{
						int					x,y,next_row_y;
						unsigned char		*data,*block;
					} light_map_texture_type;

light_map_texture_type			light_map_textures[max_light_map_textures];

int	test_color=0;	// supergumba -- testing

/* =======================================================

      Light Maps Textures
      
======================================================= */

void light_map_textures_clear(void)
{
	int						n;
	
	for (n=0;n!=max_light_map_textures;n++) {
		light_map_textures[n].block=NULL;
		light_map_textures[n].data=NULL;
	}
}

int light_map_textures_create(void)
{
	int						n,idx,sz;
	light_map_texture_type	*lmap;
	
		// find free texture
		
	idx=-1;
	
	for (n=0;n!=max_light_map_textures;n++) {
		if (light_map_textures[n].data==NULL) {
			idx=n;
			break;
		}
	}
	
	if (idx==-1) return(-1);
	
		// setup texture
		
	lmap=&light_map_textures[idx];
	
	lmap->x=0;
	lmap->y=0;
	lmap->next_row_y=0;
	
		// blocking data
		// this is used to find empty slots to put in polygons
		
	sz=light_map_texture_pixel_size/light_map_texture_block_size;
	sz=sz*sz;
	lmap->block=(unsigned char*)malloc(sz);
	bzero(lmap->block,sz);
	
		// pixel data
		
	sz=(light_map_texture_pixel_size*light_map_texture_pixel_size)*3;
	lmap->data=(unsigned char*)malloc(sz);
	bzero(lmap->data,sz);
	
	return(idx);
}

void light_map_textures_save(char *base_path)
{
	int						n;
	char					*c,name[256],path[1024];
	
		// get better name
		
	strcpy(name,map.info.name);
	
	while (TRUE) {
		c=strchr(name,' ');
		if (c==NULL) break;
		*c='_';
	}
	
		// save bitmaps
	
	for (n=0;n!=max_light_map_textures;n++) {
		if (light_map_textures[n].data==NULL) continue;
		
		sprintf(path,"%s/lm_%s_%d.png",base_path,name,n);
		bitmap_write_png_data(light_map_textures[n].data,light_map_texture_pixel_size,light_map_texture_pixel_size,FALSE,path);		
	}
}

void light_map_textures_free(void)
{
	int						n;
	
	for (n=0;n!=max_light_map_textures;n++) {
		if (light_map_textures[n].block!=NULL) free(light_map_textures[n].block);
		if (light_map_textures[n].data!=NULL) free(light_map_textures[n].data);
	}
}

/* =======================================================

      Find Open Area in Light Map
      
======================================================= */

bool light_map_texture_find_open_area(int x_sz,int y_sz,int *kx,int *ky)
{

// light_map_texture_block_size

	return(TRUE);
}

/* =======================================================

      Light Map Triangle Render
      
======================================================= */

void light_map_render_triangle(int *px,int *py,light_map_texture_type *lmap)
{
	int				n,x,y,ty,by,x1,x2,x_count,y1,y2,
					top_idx,bot_idx,l1_start_idx,l1_end_idx,l2_start_idx,l2_end_idx;
	unsigned char	*pixel;
	unsigned char	col1,col2,col3;
	
		// supergumba -- temporary testing
		
	col1=col2=col3=0xFF;
	
	switch (test_color) {
		case 0:
			col2=col3=0x0;
			break;
		case 1:
			col1=col3=0x0;
			break;
		case 2:
			col1=col2=0x0;
			break;
	}

		// determine the top and bottom vertex of the triangle

	top_idx=bot_idx=0;

	for (n=1;n!=3;n++) {
		if (py[n]<py[top_idx]) top_idx=n;
		if (py[n]>py[bot_idx]) bot_idx=n;
	}

		// find the line indexes

	l1_start_idx=top_idx;
	l1_end_idx=l1_start_idx-1;
	if (l1_end_idx<0) l1_end_idx=2;

	l2_start_idx=top_idx;
	l2_end_idx=l2_start_idx+1;
	if (l2_end_idx>2) l2_end_idx=0;

		// scan through the triangle

	ty=py[top_idx];
	by=py[bot_idx];

	for (y=ty;y<=by;y++) {
		
			// have the lines changed?

		if (y==py[l1_end_idx]) {
			l1_start_idx=l1_end_idx;
			l1_end_idx=l1_start_idx-1;
			if (l1_end_idx<0) l1_end_idx=2;
		}

		if (y==py[l2_end_idx]) {
			l2_start_idx=l2_end_idx;
			l2_end_idx=l2_start_idx+1;
			if (l2_end_idx>2) l2_end_idx=0;
		}

			// get points

		y1=py[l1_end_idx]-py[l1_start_idx];
		if (y1==0) {
			x1=px[l1_start_idx];
		}
		else {
			x1=px[l1_start_idx]+(((px[l1_end_idx]-px[l1_start_idx])*(y-py[l1_start_idx]))/y1);
		}
		
		y2=py[l2_end_idx]-py[l2_start_idx];
		if (y2==0) {
			x2=px[l2_start_idx];
		}
		else {
			x2=px[l2_start_idx]+(((px[l2_end_idx]-px[l2_start_idx])*(y-py[l2_start_idx]))/y2);
		}
		
			// draw the scan line
			
		if (x1<x2) {
			pixel=lmap->data+(((light_map_texture_pixel_size*3)*y)+(x1*3));
			x_count=x2-x1;
		}
		else {
			pixel=lmap->data+(((light_map_texture_pixel_size*3)*y)+(x2*3));
			x_count=x1-x2;
		}
		
		for (x=0;x<x_count;x++) {
			*pixel++=col1;
			*pixel++=col2;
			*pixel++=col3;
		}

	}
}

/* =======================================================

      Create Light Map for Mesh
      
======================================================= */

void light_map_create_mesh_poly_flatten(map_mesh_type *mesh,map_mesh_poly_type *poly,int v_idx,int *px,int *py)
{
	double				dx,dz;
	d3pnt				*pt;
	
	pt=&mesh->vertexes[poly->v[v_idx]];

	if (poly->box.wall_like) {
		dx=(double)(pt->x-poly->box.min.x);
		dz=(double)(pt->z-poly->box.min.z);
		
		*px=(int)sqrt((dx*dx)+(dz*dz));
		*py=pt->y-poly->box.min.y;
	}
	else {
		*px=pt->x-poly->box.min.x;
		*py=pt->z-poly->box.min.z;
	}
	
		// reduce to pixel factor (size of render)
		
	*px=(int)((float)(*px)*light_map_pixel_factor);
	*py=(int)((float)(*py)*light_map_pixel_factor);
}

void light_map_create_mesh_poly(map_mesh_type *mesh,map_mesh_poly_type *poly,light_map_texture_type *lmap)
{
	int						n,k,x,y,ex,ey,px[3],py[3];
	
		// prepare mesh for boxes and settings
		
	map_prepare_mesh_poly(mesh,poly);
	
		// find the rendering extents
		
	ex=ey=0;

	for (n=0;n!=poly->ptsz;n++) {
		light_map_create_mesh_poly_flatten(mesh,poly,n,&x,&y);
		if (x>ex) ex=x;
		if (y>ey) ey=y;
	}
	
		// will this go over the X if we render?
		
	if ((lmap->x+ex)>light_map_texture_pixel_size) {
		lmap->x=0;
		lmap->y=lmap->next_row_y;
	}
	
		// check if next y has changed
		// we have to keep a running total of the Ys so we
		// move down enough to avoid all the polygons in that
		// row
		
	if ((lmap->y+ey)>lmap->next_row_y) lmap->next_row_y=lmap->y+ey;
	
	// supergumba -- temporary -- bale out if out of room
	
	if ((lmap->y+ey)>light_map_texture_pixel_size) return;
	
		// render by triangles
		
	for (n=0;n!=(poly->ptsz-2);n++) {
		
			// get 2D points
			
		light_map_create_mesh_poly_flatten(mesh,poly,0,&px[0],&py[0]);
		light_map_create_mesh_poly_flatten(mesh,poly,(n+1),&px[1],&py[1]);
		light_map_create_mesh_poly_flatten(mesh,poly,(n+2),&px[2],&py[2]);
		
		for (k=0;k!=3;k++) {
			px[k]+=lmap->x;
			py[k]+=lmap->y;
		}
		
		light_map_render_triangle(px,py,lmap);
	}
	
		
	test_color=(test_color+1)&0x3;	// supergumba -- testing
	
		// move to next spot
	
	lmap->x+=ex;
}

bool light_map_create_mesh(map_mesh_type *mesh)
{
	int						n,txt_idx;
	map_mesh_poly_type		*poly;
	light_map_texture_type	*lmap;
	
		// get texture map
		// supergumba -- here we need to search open maps and find a spot that can fit
		
	txt_idx=light_map_textures_create();
	if (txt_idx==-1) return(FALSE);
	
	lmap=&light_map_textures[txt_idx];
	
		// create the light map for the mesh

	poly=mesh->polys;
	
	for (n=0;n!=mesh->npoly;n++) {
		light_map_create_mesh_poly(mesh,poly,lmap);
		poly++;
	}
	
	return(TRUE);
}

/* =======================================================

      Run Light Maps
      
======================================================= */

void light_maps_create(void)
{
	int				n;
	char			base_path[1024];
	map_mesh_type	*mesh;
	
	SetThemeCursor(kThemeWatchCursor);
	
		// base path
		
	file_paths_data_default(&file_path_setup,base_path,"Bitmaps/Textures",NULL,NULL);
	strcat(base_path,"/LightMaps");
	
		// create folder if it doesn't exist
		
	mkdir(base_path,S_IRWXU|S_IRWXG|S_IRWXO);
		
		// clear the textures
		
	light_map_textures_clear();

		// run all the meshes through
		
	mesh=map.mesh.meshes;
	
	for (n=0;n!=map.mesh.nmesh;n++) {
	
		if (!light_map_create_mesh(mesh)) {
			dialog_alert("Can not build light maps","You have to more polygons then will fit in texture slots.",NULL,NULL);
			light_map_textures_free();
			return;
		}
		
		break;
		mesh++;
	}
	
		// write all the textures
		
	light_map_textures_save(base_path);
	light_map_textures_free();
	
	SetThemeCursor(kThemeArrowCursor);
}
