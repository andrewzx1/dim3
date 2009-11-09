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

#define max_light_map_textures						64

typedef struct		{
						unsigned char				*data,*block;
					} light_map_texture_type;
					
typedef struct		{
						int							x[8],y[8],
													x_shift,y_shift,
													x_sz,y_sz;
					} light_map_mesh_poly_type;
					
typedef struct		{
						int							txt_idx;
						light_map_mesh_poly_type	*polys;
					} light_map_mesh_type;

light_map_texture_type			*light_map_textures;
light_map_mesh_type				*light_map_meshes;

/* =======================================================

      Light Maps Textures
      
======================================================= */

bool light_map_textures_start(char *err_str)
{
	int						n,sz;
	
	sz=sizeof(light_map_texture_type)*max_light_map_textures;
	light_map_textures=(light_map_texture_type*)malloc(sz);
	if (light_map_textures==NULL) {
		strcpy(err_str,"Out of Memory");
		return(FALSE);
	}
	
	for (n=0;n!=max_light_map_textures;n++) {
		light_map_textures[n].block=NULL;
		light_map_textures[n].data=NULL;
	}
	
	return(TRUE);
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
	int						n,txt_idx;
	char					*c,map_name[256],bitmap_name[256],path[1024];
	
		// get better name
		
	strcpy(map_name,map.info.name);
	
	while (TRUE) {
		c=strchr(map_name,' ');
		if (c==NULL) break;
		*c='_';
	}
	
		// write textures
		
	txt_idx=max_map_texture-max_light_map_textures;
	
	for (n=0;n!=max_light_map_textures;n++) {
		if (light_map_textures[n].data==NULL) break;
		
			// save bitmap
		
		sprintf(bitmap_name,"LightMaps/lm_%s_%d",map_name,n);
		sprintf(path,"%s/%s.png",base_path,bitmap_name);
		bitmap_write_png_data(light_map_textures[n].data,light_map_texture_pixel_size,light_map_texture_pixel_size,FALSE,path);
		
			// put in texture list
			
		map_replace_texture(&map,txt_idx,bitmap_name);
		txt_idx++;
	}
}

void light_map_textures_free(void)
{
	int						n;
	
	for (n=0;n!=max_light_map_textures;n++) {
		if (light_map_textures[n].block!=NULL) free(light_map_textures[n].block);
		if (light_map_textures[n].data!=NULL) free(light_map_textures[n].data);
	}
	
	free(light_map_textures);
}

/* =======================================================

      Light Maps Poly Setup
      
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

bool light_map_mesh_poly_start(char *err_str)
{
	int							n,k,t,sz;
	map_mesh_type				*mesh;
	map_mesh_poly_type			*poly;
	light_map_mesh_type			*lm_mesh;
	light_map_mesh_poly_type	*lm_poly;
	
		// mesh memory
		
	sz=sizeof(light_map_mesh_type)*map.mesh.nmesh;
	light_map_meshes=(light_map_mesh_type*)malloc(sz);
	
	if (light_map_meshes==NULL) {
		strcpy(err_str,"Out of Memory");
		return(FALSE);
	}
	
		// poly memory
		
	mesh=map.mesh.meshes;
	lm_mesh=light_map_meshes;
	
	for (n=0;n!=map.mesh.nmesh;n++) {
	
			// prepare meshes
			
		map_prepare_mesh_box(mesh);

			// create poly memory
			
		sz=sizeof(light_map_mesh_poly_type)*mesh->npoly;
		lm_mesh->polys=(light_map_mesh_poly_type*)malloc(sz);
		
		if (lm_mesh->polys==NULL) {
			strcpy(err_str,"Out of Memory");
			return(FALSE);
		}

			// build poly 2D vertexes
			// and 2D size
			
		poly=mesh->polys;
		lm_poly=lm_mesh->polys;
	
		for (k=0;k!=mesh->npoly;k++) {
			map_prepare_mesh_poly(mesh,poly);
			
			lm_poly->x_sz=0;
			lm_poly->y_sz=0;
			
			for (t=0;t!=poly->ptsz;t++) {
				light_map_create_mesh_poly_flatten(mesh,poly,t,&lm_poly->x[t],&lm_poly->y[t]);
				if (lm_poly->x[t]>lm_poly->x_sz) lm_poly->x_sz=lm_poly->x[t];
				if (lm_poly->y[t]>lm_poly->y_sz) lm_poly->y_sz=lm_poly->y[t];
			}
			
			if ((lm_poly->x_sz>=light_map_texture_pixel_size) || (lm_poly->y_sz>=light_map_texture_pixel_size)) {
				sprintf(err_str,"Mesh poly (%d.%d) is too big to fit within a single light map texture",n,k);
				return(FALSE);
			}

			poly++;
			lm_poly++;
		}
		
		mesh++;
		lm_mesh++;
	}
	
	return(TRUE);
}

void light_map_mesh_poly_free(void)
{
	int							n;
	light_map_mesh_type			*lm_mesh;
	
	lm_mesh=light_map_meshes;
	
	for (n=0;n!=map.mesh.nmesh;n++) {
		free(lm_mesh->polys);
		lm_mesh++;
	}
	
	free(light_map_meshes);
}

/* =======================================================

      Find Open Area in Light Map
      
======================================================= */

bool light_map_texture_find_open_area(int x_sz,int y_sz,int *kx,int *ky,light_map_texture_type *lmap)
{
	int				x,y,bx,by,block_count,b_x_sz,b_y_sz;
	bool			hit;
	unsigned char	*bptr;
	
		// get block size
		
	b_x_sz=(x_sz/light_map_texture_block_size);
	if ((x_sz%light_map_texture_block_size)!=0) b_x_sz++;
	
	b_y_sz=(y_sz/light_map_texture_block_size);
	if ((y_sz%light_map_texture_block_size)!=0) b_y_sz++;
	
	block_count=light_map_texture_pixel_size/light_map_texture_block_size;
	
		// find free block
		
	for (y=0;y<(block_count-b_y_sz);y++) {
		for (x=0;x<(block_count-b_x_sz);x++) {
			bptr=lmap->block+((y*block_count)+x);
			if (*bptr!=0x0) continue;
			
				// is block of this size open?
				
			hit=FALSE;
				
			for (by=y;by<(y+b_y_sz);by++) {
				for (bx=x;bx<(x+b_x_sz);bx++) {
					bptr=lmap->block+((by*block_count)+bx);
					if (*bptr!=0x0) {
						hit=TRUE;
						break;
					}
				}
				if (hit) break;
			}
			
			if (hit) continue;
			
				// found block!
				
			*kx=x*light_map_texture_block_size;
			*ky=y*light_map_texture_block_size;
			return(TRUE);
		}
	}

	return(FALSE);
}

void light_map_texture_block_area(int x,int y,int x_sz,int y_sz,unsigned char blk,light_map_texture_type *lmap)
{
	int				bx,by,b_x_sz,b_y_sz,block_count;
	unsigned char	*bptr;
	
		// get block size
		
	b_x_sz=(x_sz/light_map_texture_block_size);
	if ((x_sz%light_map_texture_block_size)!=0) b_x_sz++;
	
	b_y_sz=(y_sz/light_map_texture_block_size);
	if ((y_sz%light_map_texture_block_size)!=0) b_y_sz++;
	
		// get block starts
		
	x/=light_map_texture_block_size;
	y/=light_map_texture_block_size;
	
	block_count=light_map_texture_pixel_size/light_map_texture_block_size;
	
		// block out area
		
	for (by=y;by<(y+b_y_sz);by++) {
		bptr=lmap->block+((by*block_count)+x);
		for (bx=x;bx<(x+b_x_sz);bx++) {
			*bptr++=blk;
		}
	}
}

void light_map_texture_clear_block_type(unsigned char blk,light_map_texture_type *lmap)
{
	int				n,block_count;
	unsigned char	*bptr;
	
	block_count=light_map_texture_pixel_size/light_map_texture_block_size;
	block_count*=block_count;
		
	bptr=lmap->block;
	
	for (n=0;n!=block_count;n++) {
		if ((*bptr)==blk) *bptr=0x0;
		bptr++;
	}
}

/* =======================================================

      Light Map Triangle Render
      
======================================================= */

void light_map_render_triangle(int *px,int *py,light_map_texture_type *lmap)
{
	int				n,x,y,ty,by,x1,x2,x_count,y1,y2,
					top_idx,bot_idx,l1_start_idx,l1_end_idx,l2_start_idx,l2_end_idx;
	unsigned char	*pixel;
	unsigned char	col;
	
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
		
		if ((by-ty)==0) {		// supergumba -- testing
			col=0x0;
		}
		else {
			col=(unsigned char)(((float)(y-ty)/(float)(by-ty))*256.0f);
		}
			
		for (x=0;x<x_count;x++) {
			*pixel++=col;
			*pixel++=col;
			*pixel++=col;
		}

	}
}

/* =======================================================

      Create Light Map for Mesh
      
======================================================= */

void light_map_create_mesh_poly(map_mesh_type *mesh,map_mesh_poly_type *poly,light_map_mesh_poly_type *lm_poly,light_map_texture_type *lmap)
{
	int						n,x,y,px[3],py[3];
	
		// get rendering spot (need to get again as this
		// might be a new texture and block it off
		// from further rendering
		
	if (!light_map_texture_find_open_area(lm_poly->x_sz,lm_poly->y_sz,&x,&y,lmap)) return;
	light_map_texture_block_area(x,y,lm_poly->x_sz,lm_poly->y_sz,0x1,lmap);
		
		// remember the shift for creating the UVs
		
	lm_poly->x_shift=x;
	lm_poly->y_shift=y;
	
		// render by triangles
		
	for (n=0;n!=(poly->ptsz-2);n++) {
		
		px[0]=lm_poly->x[0]+x;
		py[0]=lm_poly->y[0]+y;
		px[1]=lm_poly->x[n+1]+x;
		py[1]=lm_poly->y[n+1]+y;
		px[2]=lm_poly->x[n+2]+x;
		py[2]=lm_poly->y[n+2]+y;
		
		light_map_render_triangle(px,py,lmap);
	}
}

bool light_map_create_mesh(int mesh_idx,char *err_str)
{
	int							n,k,x,y,txt_idx;
	bool						txt_ok;
	light_map_mesh_poly_type	*lm_poly;
	map_mesh_type				*mesh;
	map_mesh_poly_type			*poly;
	light_map_mesh_type			*lm_mesh;
	light_map_texture_type		*lmap;
	
	mesh=&map.mesh.meshes[mesh_idx];
	lm_mesh=&light_map_meshes[mesh_idx];
	
		// find if it can fit in any of the
		// existing light map textures
		
		// go through the light maps blocking off with 0x2
		// so it can be cleared at the end of the test
		
	txt_idx=-1;
	
	for (n=0;n!=max_light_map_textures;n++) {
		lmap=&light_map_textures[n];
		if (lmap->data==NULL) continue;
		
		txt_ok=TRUE;
		lm_poly=lm_mesh->polys;
		
		for (k=0;k!=mesh->npoly;k++) {
			if (!light_map_texture_find_open_area(lm_poly->x_sz,lm_poly->y_sz,&x,&y,lmap)) {
				txt_ok=FALSE;
				break;
			}
			light_map_texture_block_area(x,y,lm_poly->x_sz,lm_poly->y_sz,0x2,lmap);
		
			lm_poly++;
		}
		
		light_map_texture_clear_block_type(0x2,lmap);
		
		if (txt_ok) {
			txt_idx=n;
			break;
		}
	}

		// need to create a new one?
		
	if (txt_idx==-1) txt_idx=light_map_textures_create();
	
		// bail out if no texture
		
	if (txt_idx==-1) {
		sprintf(err_str,"Too many polys in map, would create more than %d light maps",max_light_map_textures);
		return(FALSE);
	}
	
		// remember this light map
		
	lmap=&light_map_textures[txt_idx];
	lm_mesh->txt_idx=txt_idx;
	
		// create the light map for each poly

	poly=mesh->polys;
	lm_poly=lm_mesh->polys;
	
	for (n=0;n!=mesh->npoly;n++) {
		light_map_create_mesh_poly(mesh,poly,lm_poly,lmap);
		poly++;
		lm_poly++;
	}
	
	return(TRUE);
}

void light_map_finialize_mesh(int mesh_idx)
{
	int							n,k;
	float						f_pixel_size;
	light_map_mesh_poly_type	*lm_poly;
	map_mesh_type				*mesh;
	map_mesh_poly_type			*poly;
	light_map_mesh_type			*lm_mesh;
	
	mesh=&map.mesh.meshes[mesh_idx];
	lm_mesh=&light_map_meshes[mesh_idx];
	
		// set extra texture
		
	mesh->extra_txt_idx=(max_map_texture-max_light_map_textures)+lm_mesh->txt_idx;
	
		// make sure there are two UVs
		
	mesh->nuv=2;
	
		// add the alternate UVs
		
	f_pixel_size=(float)light_map_texture_pixel_size;
		
	poly=mesh->polys;
	lm_poly=lm_mesh->polys;
	
	for (n=0;n!=mesh->npoly;n++) {
		for (k=0;k!=poly->ptsz;k++) {
			poly->uv[1].x[k]=((float)(lm_poly->x[k]+lm_poly->x_shift))/f_pixel_size;
			poly->uv[1].y[k]=((float)(lm_poly->y[k]+lm_poly->y_shift))/f_pixel_size;
		}
		poly++;
		lm_poly++;
	}
	
}

/* =======================================================

      Run Light Maps
      
======================================================= */

bool light_maps_create_run(char *err_str)
{
	int				n;
	char			base_path[1024],dir_path[1024];
	
		// base path
		
	file_paths_data_default(&file_path_setup,base_path,"Bitmaps/Textures",NULL,NULL);
	
		// create folder if it doesn't exist
		
	sprintf(dir_path,"%s/LightMaps",base_path);
	mkdir(dir_path,S_IRWXU|S_IRWXG|S_IRWXO);
		
		// clear the textures and
		// start mesh-poly setup
		
	if (!light_map_textures_start(err_str)) return(FALSE);
	if (!light_map_mesh_poly_start(err_str)) {
		light_map_textures_free();
		return(FALSE);
	}
	
		// run all the meshes through
		
	for (n=0;n!=map.mesh.nmesh;n++) {
	
		if (!light_map_create_mesh(n,err_str)) {
			light_map_textures_free();
			light_map_mesh_poly_free();
			return(FALSE);
		}
		
	}
	
		// write all the textures
		
	light_map_textures_save(base_path);
	
		// fix all meshes (add UVs and
		// set alternate texture)
		
	for (n=0;n!=map.mesh.nmesh;n++) {
		light_map_finialize_mesh(n);
	}
	
		// free textures and polys
		
	light_map_textures_free();
	light_map_mesh_poly_free();
	
	main_wind_draw();
	
	return(TRUE);
}

void light_maps_create(void)
{
	bool			ok;
	char			err_str[256];
	
	SetThemeCursor(kThemeWatchCursor);
	ok=light_maps_create_run(err_str);
	SetThemeCursor(kThemeArrowCursor);
		
	if (!ok) dialog_alert("Can not build light maps",err_str,NULL,NULL);
}
