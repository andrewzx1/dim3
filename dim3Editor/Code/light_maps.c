/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Light Map Routines

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

#include "interface.h"
#include "dialog.h"
#include "common_view.h"

extern map_type					map;

extern file_path_setup_type		file_path_setup;

#define max_light_map_textures						64
#define max_light_map_solid_color					128
#define light_map_render_margin						5

typedef struct		{
						unsigned char				*block,*pixel_data,
													*pixel_touch,*pixel_ignore;
					} light_map_texture_type;
					
typedef struct		{
						int							ptsz,txt_idx,
													mesh_idx,poly_idx,liquid_idx,
													x[8],y[8],
													x_shift,y_shift,
													x_sz,y_sz;
						bool						solid_color;
						d3pnt						pt[8];
						d3rect						box;
					} light_map_poly_type;
					
typedef struct		{
						unsigned char				*data;
					} light_map_map_texture_alpha_type;
					
typedef struct		{
						int							txt_idx,x_shift,y_shift;
						unsigned char				col[3];
					} light_map_solid_color_cache_type;

int									light_map_poly_count,light_map_solid_color_cache_count;
light_map_texture_type				*light_map_textures;
light_map_poly_type					*light_map_polys;
light_map_solid_color_cache_type	*light_map_solid_color_cache;
light_map_map_texture_alpha_type	light_map_map_texture_alphas[max_map_texture];

/* =======================================================

      Light Maps Textures
      
======================================================= */

bool light_map_textures_start(char *err_str)
{
	int						n,sz;
	
		// textures
		
	sz=sizeof(light_map_texture_type)*max_light_map_textures;
	light_map_textures=(light_map_texture_type*)malloc(sz);
	if (light_map_textures==NULL) {
		strcpy(err_str,"Out of Memory");
		return(FALSE);
	}
	
	for (n=0;n!=max_light_map_textures;n++) {
		light_map_textures[n].block=NULL;
		light_map_textures[n].pixel_data=NULL;
		light_map_textures[n].pixel_touch=NULL;
		light_map_textures[n].pixel_ignore=NULL;
	}
	
		// color cache
		
	light_map_solid_color_cache_count=0;
	sz=sizeof(light_map_solid_color_cache_type)*max_light_map_solid_color;
	
	light_map_solid_color_cache=(light_map_solid_color_cache_type*)malloc(sz);
	if (light_map_solid_color_cache==NULL) {
		free(light_map_textures);
		strcpy(err_str,"Out of Memory");
		return(FALSE);
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
		if (light_map_textures[n].pixel_data==NULL) {
			idx=n;
			break;
		}
	}
	
	if (idx==-1) return(-1);
	
		// setup texture
		
	lmap=&light_map_textures[idx];
	
		// blocking data
		// this is used to find empty slots to put in polygons
		
	sz=map.settings.light_map_size/light_map_texture_block_size;
	sz=sz*sz;
	lmap->block=(unsigned char*)malloc(sz);
	bzero(lmap->block,sz);
	
		// pixel data
		
	sz=(map.settings.light_map_size*map.settings.light_map_size)*3;
	lmap->pixel_data=(unsigned char*)malloc(sz);
	bzero(lmap->pixel_data,sz);
	
		// pixel touch data
		// this tells if a pixel was written
		
	sz=map.settings.light_map_size*map.settings.light_map_size;
	lmap->pixel_touch=(unsigned char*)malloc(sz);
	bzero(lmap->pixel_touch,sz);
	
		// pixel ignore data
		// special flag to deal with solid color
		// blocks (shared between polygons)
		// these blocks are ignored by blur & pixel borders
		
	lmap->pixel_ignore=(unsigned char*)malloc(sz);
	bzero(lmap->pixel_ignore,sz);
	
	return(idx);
}

void light_map_textures_save(char *base_path)
{
	int						n,txt_idx;
	char					*c,map_name[256],bitmap_name[256],path[1024];
	
		// delete any old textures
		
	map_delete_texture(&map,(max_map_texture-max_light_map_textures),max_map_texture);
	
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
		dialog_progress_next();
		
		if (light_map_textures[n].pixel_data==NULL) break;
		
			// save bitmap
		
		sprintf(bitmap_name,"LightMaps/lm_%s_%d",map_name,n);
		sprintf(path,"%s/%s.png",base_path,bitmap_name);
		bitmap_write_png_data(light_map_textures[n].pixel_data,map.settings.light_map_size,map.settings.light_map_size,FALSE,path);
		
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
		if (light_map_textures[n].pixel_data!=NULL) free(light_map_textures[n].pixel_data);
		if (light_map_textures[n].pixel_touch!=NULL) free(light_map_textures[n].pixel_touch);
		if (light_map_textures[n].pixel_ignore!=NULL) free(light_map_textures[n].pixel_ignore);
	}
	
	free(light_map_textures);
	
	free(light_map_solid_color_cache);
}

/* =======================================================

      Light Texture Borders and Blur
      
======================================================= */

void light_map_texture_single_pixel_border(int idx,int pixel_border_count)
{
	int						n,x,y,cx,cy,sz,i_col[3],col_count;
	unsigned char			*pixel,*pixel_touch,*pixel_ignore,
							*back_pixel_data,*back_pixel_touch,
							*back,*back_touch,
							*blur,*pixel_border_touch;
	light_map_texture_type	*lmap;
	
		// get light map
		
	lmap=&light_map_textures[idx];
	if (lmap->pixel_data==NULL) return;
					
		// back pixel/touch data
		
	sz=(map.settings.light_map_size*map.settings.light_map_size)*3;
	back_pixel_data=(unsigned char*)malloc(sz);
		
	sz=map.settings.light_map_size*map.settings.light_map_size;
	back_pixel_touch=(unsigned char*)malloc(sz);

		// add pixel border
		
	for (n=0;n!=pixel_border_count;n++) {
	
		for (y=0;y!=map.settings.light_map_size;y++) {
		
			for (x=0;x!=map.settings.light_map_size;x++) {
			
					// get pixels and default to leaving
					// the same
					
				pixel_touch=lmap->pixel_touch+((map.settings.light_map_size*y)+x);
				back_touch=back_pixel_touch+((map.settings.light_map_size*y)+x);
				
				*back_touch=*pixel_touch;
				
				pixel=lmap->pixel_data+(((map.settings.light_map_size*3)*y)+(x*3));
				back=back_pixel_data+(((map.settings.light_map_size*3)*y)+(x*3));
				
				*back=*pixel;
				*(back+1)=*(pixel+1);
				*(back+2)=*(pixel+2);
				
					// pixel borders only effect non-touched pixels
					// or pixels set to ignore (for solid color shared maps)
					
				pixel_ignore=lmap->pixel_ignore+((map.settings.light_map_size*y)+x);

				if ((*pixel_touch!=0x0) || (*pixel_ignore!=0x0)) continue;
			
					// find largest hilited pixel to
					// make border from
					
				col_count=0;
				i_col[0]=i_col[1]=i_col[2]=0;
				
				for (cy=(y-1);cy!=(y+2);cy++) {
					for (cx=(x-1);cx!=(x+2);cx++) {
					
						if ((cy==y) && (cx==x)) continue;
						if ((cy<0) || (cy>=map.settings.light_map_size) || (cx<0) || (cx>=map.settings.light_map_size)) continue;
					
						pixel_border_touch=lmap->pixel_touch+((map.settings.light_map_size*cy)+cx);
						if (*pixel_border_touch==0x0) continue;
						
							// is this a bigger hilite?
							
						blur=lmap->pixel_data+(((map.settings.light_map_size*3)*cy)+(cx*3));
						i_col[0]+=(int)*blur;
						i_col[1]+=(int)*(blur+1);
						i_col[2]+=(int)*(blur+2);
						col_count++;
					}
				}
				
				if (col_count!=0) {
					i_col[0]=i_col[0]/col_count;
					if (i_col[0]>255) i_col[0]=255;
					
					i_col[1]=i_col[1]/col_count;
					if (i_col[1]>255) i_col[1]=255;
					
					i_col[2]=i_col[2]/col_count;
					if (i_col[2]>255) i_col[2]=255;
					
					*back++=(unsigned char)i_col[0];
					*back++=(unsigned char)i_col[1];
					*back++=(unsigned char)i_col[2];
					
					*back_touch=0x1;		// next time this is part of the smear
				}
			}
		}
		
		memmove(lmap->pixel_data,back_pixel_data,((map.settings.light_map_size*3)*map.settings.light_map_size));
		memmove(lmap->pixel_touch,back_pixel_touch,(map.settings.light_map_size*map.settings.light_map_size));
	}
		
		// free backgrounds
		
	free(back_pixel_data);
	free(back_pixel_touch);
}

void light_map_textures_pixel_border(void)
{
	int						n;
	
	for (n=0;n!=max_light_map_textures;n++) {
		dialog_progress_next();
		light_map_texture_single_pixel_border(n,map.settings.light_map_pixel_border_count);
	}
}

void light_map_texture_single_blur(int idx,int blur_count)
{
	int						n,x,y,cx,cy,sz,i_col[3],col_count;
	unsigned char			*pixel,*back_pixel_data,
							*pixel_ignore,*back,*blur;
	light_map_texture_type	*lmap;
	
		// get light map
	
	lmap=&light_map_textures[idx];
	if (lmap->pixel_data==NULL) return;
					
		// back pixel data
		
	sz=(map.settings.light_map_size*map.settings.light_map_size)*3;
	back_pixel_data=(unsigned char*)malloc(sz);

		// blur pixels
		
	for (n=0;n!=blur_count;n++) {
	
		for (y=0;y!=map.settings.light_map_size;y++) {
		
			for (x=0;x!=map.settings.light_map_size;x++) {
			
					// default to back being same as pixels
					
				pixel=lmap->pixel_data+(((map.settings.light_map_size*3)*y)+(x*3));
				back=back_pixel_data+(((map.settings.light_map_size*3)*y)+(x*3));
				
				*back=*pixel;
				*(back+1)=*(pixel+1);
				*(back+2)=*(pixel+2);
			
					// skip all shared solid colors
					
				pixel_ignore=lmap->pixel_ignore+((map.settings.light_map_size*y)+x);
				if (*pixel_ignore!=0x0) continue;
										
					// get blur from 8 surrounding pixels
					
				col_count=0;
				i_col[0]=i_col[1]=i_col[2]=0;
				
				for (cy=(y-1);cy!=(y+2);cy++) {
					for (cx=(x-1);cx!=(x+2);cx++) {
					
						if ((cy==y) && (cx==x)) continue;
						if ((cy<0) || (cy>=map.settings.light_map_size) || (cx<0) || (cx>=map.settings.light_map_size)) continue;
						
							// add up blur
							
						blur=lmap->pixel_data+(((map.settings.light_map_size*3)*cy)+(cx*3));
						i_col[0]+=(int)*blur;
						i_col[1]+=(int)*(blur+1);
						i_col[2]+=(int)*(blur+2);
						col_count++;
					}
				}
				
				if (col_count!=0) {
					i_col[0]=i_col[0]/col_count;
					if (i_col[0]>255) i_col[0]=255;
					
					i_col[1]=i_col[1]/col_count;
					if (i_col[1]>255) i_col[1]=255;
					
					i_col[2]=i_col[2]/col_count;
					if (i_col[2]>255) i_col[2]=255;
					
					*back++=(unsigned char)i_col[0];
					*back++=(unsigned char)i_col[1];
					*back++=(unsigned char)i_col[2];
				}
			}
			
		}
		
		memmove(lmap->pixel_data,back_pixel_data,((map.settings.light_map_size*3)*map.settings.light_map_size));
	}
		
		// free backgrounds
		
	free(back_pixel_data);
}

void light_map_textures_blur(void)
{
	int						n;
	
	for (n=0;n!=max_light_map_textures;n++) {
		dialog_progress_next();
		light_map_texture_single_blur(n,map.settings.light_map_blur_count);
	}
}

/* =======================================================

      Create Mesh Light Map Polys
      
======================================================= */

void light_map_create_mesh_poly_flatten_setup_size(light_map_poly_type *lm_poly)
{
	int				n;
	
	lm_poly->x_sz=0;
	lm_poly->y_sz=0;
	
	for (n=0;n!=lm_poly->ptsz;n++) {
		if (lm_poly->x[n]>lm_poly->x_sz) lm_poly->x_sz=lm_poly->x[n];
		if (lm_poly->y[n]>lm_poly->y_sz) lm_poly->y_sz=lm_poly->y[n];
	}
}

void light_map_create_poly_flatten(light_map_poly_type *lm_poly)
{
	int				n,max_sz,min_x,min_y;
	float			factor;
	
		// flatten
		
	light_map_create_mesh_poly_flatten_setup_size(lm_poly);
	
		// largest poly is 1/4 of texture size
		
	max_sz=map.settings.light_map_size>>2;
	if ((lm_poly->x_sz<=max_sz) && (lm_poly->y_sz<=max_sz)) return;
	
		// reduce to 1/4 size
		
	if (lm_poly->x_sz>lm_poly->y_sz) {
		factor=(float)max_sz/(float)lm_poly->x_sz;
	}
	else {
		factor=(float)max_sz/(float)lm_poly->y_sz;
	}
	
	min_x=lm_poly->x_sz;
	min_y=lm_poly->y_sz;
	
	for (n=0;n!=lm_poly->ptsz;n++) {
		lm_poly->x[n]=(int)(((float)lm_poly->x[n])*factor);
		lm_poly->y[n]=(int)(((float)lm_poly->y[n])*factor);
	
		if (lm_poly->x[n]<min_x) min_x=lm_poly->x[n];
		if (lm_poly->y[n]<min_y) min_y=lm_poly->y[n];
	}
	
		// push reduced polygon back to top-left
		// and rebuild poly size
		
	for (n=0;n!=lm_poly->ptsz;n++) {
		lm_poly->x[n]-=min_x;
		lm_poly->y[n]-=min_y;
	}
	
	light_map_create_mesh_poly_flatten_setup_size(lm_poly);
}

void light_map_create_mesh_poly_flatten(map_mesh_type *mesh,map_mesh_poly_type *poly,light_map_poly_type *lm_poly)
{
	int					n,x,y;
	float				factor;
	double				dx,dz;
	d3pnt				*pt;
	
	factor=((float)map.settings.light_map_quality)/light_map_quality_to_pixel_factor;
			
		// flatten the poly
		// and get 3D points
	
	lm_poly->ptsz=poly->ptsz;
		
	for (n=0;n!=poly->ptsz;n++) {
	
		pt=&mesh->vertexes[poly->v[n]];
		memmove(&lm_poly->pt[n],pt,sizeof(d3pnt));

		if (poly->box.wall_like) {
			dx=(double)(pt->x-poly->line.lx);
			dz=(double)(pt->z-poly->line.lz);
			
			x=(int)sqrt((dx*dx)+(dz*dz));
			y=pt->y-poly->box.min.y;
		}
		else {
			x=pt->x-poly->box.min.x;
			y=pt->z-poly->box.min.z;
		}
		
			// reduce to pixel factor (size of render)
			
		lm_poly->x[n]=(int)((float)x*factor);
		lm_poly->y[n]=(int)((float)y*factor);
	}
}

void light_map_create_liquid_poly_flatten(map_liquid_type *liq,light_map_poly_type *lm_poly)
{
	float				factor;
	
	factor=((float)map.settings.light_map_quality)/light_map_quality_to_pixel_factor;
			
		// flatten the poly
		// and get 3D points
	
	lm_poly->ptsz=4;
	
	lm_poly->pt[0].x=lm_poly->pt[3].x=liq->lft;
	lm_poly->pt[1].x=lm_poly->pt[2].x=liq->rgt;
	lm_poly->pt[0].z=lm_poly->pt[1].z=liq->top;
	lm_poly->pt[2].z=lm_poly->pt[3].z=liq->bot;
	lm_poly->pt[0].y=lm_poly->pt[1].y=lm_poly->pt[2].y=lm_poly->pt[3].y=liq->y;
	
	lm_poly->x[0]=lm_poly->x[3]=0.0f;
	lm_poly->x[1]=lm_poly->x[2]=(int)((float)(liq->rgt-liq->lft)*factor);
	lm_poly->y[0]=lm_poly->y[1]=0.0f;
	lm_poly->y[2]=lm_poly->y[3]=(int)((float)(liq->bot-liq->top)*factor);
}

int light_map_get_poly_count(void)
{
	int					n,count;
	map_mesh_type		*mesh;
	
	count=0;
		
		// meshes
		
	mesh=map.mesh.meshes;
		
	for (n=0;n!=map.mesh.nmesh;n++) {
		if (!mesh->flag.no_light_map) count+=mesh->npoly;
		mesh++;
	}
	
		// liquids
		
	count+=map.liquid.nliquid;
	
	return(count);
}

bool light_map_polys_start(char *err_str)
{
	int							n,k,sz;
	map_mesh_type				*mesh;
	map_mesh_poly_type			*poly;
	map_liquid_type				*liq;
	light_map_poly_type			*lm_poly;
	
		// get poly count
	
	light_map_poly_count=light_map_get_poly_count();

	sz=sizeof(light_map_poly_type)*light_map_poly_count;
	light_map_polys=(light_map_poly_type*)malloc(sz);
	
	if (light_map_polys==NULL) {
		strcpy(err_str,"Out of Memory");
		return(FALSE);
	}
	
		// build polys
		
	lm_poly=light_map_polys;
	
		// mesh polys
		
	for (n=0;n!=map.mesh.nmesh;n++) {
	
		mesh=&map.mesh.meshes[n];
		
			// clear all light map textures
			
		poly=mesh->polys;
		for (k=0;k!=mesh->npoly;k++) {
			poly->lmap_txt_idx=-1;
			poly++;
		}
		
			// no light map mesh?
			
		if (mesh->flag.no_light_map) continue;
	
			// prepare meshes
			
		map_prepare_mesh_box(mesh);

			// build poly 2D vertexes
			// and 2D size
			
		poly=mesh->polys;
	
		for (k=0;k!=mesh->npoly;k++) {
			
			lm_poly->mesh_idx=n;
			lm_poly->poly_idx=k;
			
				// flatten the poly
				
			map_prepare_mesh_poly(mesh,poly);
			light_map_create_mesh_poly_flatten(mesh,poly,lm_poly);
			light_map_create_poly_flatten(lm_poly);

			poly++;
			lm_poly++;
		}
	}
	
		// liquid polys

	liq=map.liquid.liquids;
	
	for (n=0;n!=map.liquid.nliquid;n++) {
	
			// clear light map texture
			
		liq->lmap_txt_idx=-1;
		
			// setup poly
			
		lm_poly->mesh_idx=-1;
		lm_poly->poly_idx=-1;
		lm_poly->liquid_idx=n;
		
			// flatten the poly
			
		light_map_create_liquid_poly_flatten(liq,lm_poly);
		light_map_create_poly_flatten(lm_poly);

		liq++;
		lm_poly++;
	}

	return(TRUE);
}

void light_map_polys_free(void)
{
	free(light_map_polys);
}

/* =======================================================

      Find Open Area in Light Map
      
======================================================= */

bool light_map_texture_find_open_area(int x_sz,int y_sz,int *kx,int *ky,d3rect *box,bool solid_color,light_map_texture_type *lmap)
{
	int				x,y,bx,by,block_count,b_x_sz,b_y_sz;
	bool			hit;
	unsigned char	*bptr;
	
	block_count=map.settings.light_map_size/light_map_texture_block_size;
	
		// get block size
	
	if (!solid_color) {
		b_x_sz=(x_sz/light_map_texture_block_size);
		if ((x_sz%light_map_texture_block_size)!=0) b_x_sz++;
		
		b_y_sz=(y_sz/light_map_texture_block_size);
		if ((y_sz%light_map_texture_block_size)!=0) b_y_sz++;
		
			// always take one extra block on each
			// side so we can better center the polygon
			// in the middle and smear the colors
			
		b_x_sz+=2;
		b_y_sz+=2;
	}
	else {
		b_x_sz=b_y_sz=1;
	}
	
		// find free block
		
	for (y=0;y<(block_count-b_y_sz);y++) {
		for (x=0;x<(block_count-b_x_sz);x++) {
			bptr=lmap->block+((y*block_count)+x);
			if (*bptr!=0x0) continue;
			
				// is block of this size open?
				
			hit=FALSE;
				
			for (by=y;by<(y+b_y_sz);by++) {
				bptr=lmap->block+((by*block_count)+x);
				for (bx=x;bx<(x+b_x_sz);bx++) {
					if (*bptr!=0x0) {
						hit=TRUE;
						break;
					}
					bptr++;
				}
				if (hit) break;
			}
			
			if (hit) continue;
			
				// found block!
				// mark off the block
				
			for (by=y;by<(y+b_y_sz);by++) {
				bptr=lmap->block+((by*block_count)+x);
				for (bx=x;bx<(x+b_x_sz);bx++) {
					*bptr++=0x1;
				}
			}
			
				// remember the block so we can smear
				// the colors laters to eliminate edge bleeding
				
			x=(x*light_map_texture_block_size);
			y=(y*light_map_texture_block_size);
				
			if (box!=NULL) {
				box->lx=x;
				box->rx=(x+(b_x_sz*light_map_texture_block_size))-1;
				box->ty=y;
				box->by=(y+(b_y_sz*light_map_texture_block_size))-1;
			}
			
				// solid colors point towards beginning
				// of single block
				
			if (solid_color) {
				*kx=x;
				*ky=y;
			}
			
				// normal blocks move x,y into center of
				// extra border blocks so we have more room to smear
				
			else {
				*kx=x+light_map_texture_block_size;
				*ky=y+light_map_texture_block_size;
			}
			
			return(TRUE);
		}
	}

	return(FALSE);
}

void light_map_texture_fill_solid_color(unsigned char *col,int bx,int by,light_map_texture_type *lmap)
{
	int				x,y;
	unsigned char	*pixel,*pixel_ignore;
	
		// fill it
		// we mark the pixel ignore because these shouldn't
		// add a border or be blured
		
	for (y=0;y!=light_map_texture_block_size;y++) {
	
		pixel=lmap->pixel_data+(((by+y)*(map.settings.light_map_size*3))+(bx*3));
		pixel_ignore=lmap->pixel_ignore+(((by+y)*map.settings.light_map_size)+bx);
		
		for (x=0;x!=light_map_texture_block_size;x++) {
			*pixel++=col[0];
			*pixel++=col[1];
			*pixel++=col[2];
			
			*pixel_ignore++=0x1;
		}
	}
}

/* =======================================================

      Find Ray Trace Hit on 
      
======================================================= */

void light_map_bitmap_transparency_start(void)
{
	int				n;
	
	for (n=0;n!=max_map_texture;n++) {
		light_map_map_texture_alphas[n].data=NULL;
	}
}

void light_map_bitmap_transparency_free(void)
{
	int				n;
	
	for (n=0;n!=max_map_texture;n++) {
		if (light_map_map_texture_alphas[n].data!=NULL) free(light_map_map_texture_alphas[n].data);
	}
}

bool light_map_bitmap_transparency_check(d3pnt *spt,d3vct *vct,map_mesh_type *mesh,map_mesh_poly_type *poly,float hit_t)
{
	int				n,txt_idx,txt_sz,x,y,
					lft_idx,rgt_idx,top_idx,bot_idx;
	float			fx,fy,min_gx,max_gx,min_gy,max_gy;
	double			dx,dz,d1,d2;
	unsigned char	*aptr;
	d3pnt			hpt;
	d3pnt			*pt;
	bitmap_type		*bitmap;
	
		// get texture
		
	txt_idx=poly->txt_idx;
	if ((txt_idx<0) || (txt_idx>=max_map_texture)) return(TRUE);
	
	bitmap=&map.textures[txt_idx].frames[0].bitmap;

		// if no alpha or not loaded for some reason, it's immediately a hit
	
	if ((bitmap->gl_id==-1) || (bitmap->alpha_mode==alpha_mode_none)) return(TRUE);
	
		// need to load up this bitmap
		
	if (light_map_map_texture_alphas[txt_idx].data==NULL) {
	
		txt_sz=bitmap->wid*bitmap->high;
		light_map_map_texture_alphas[txt_idx].data=(unsigned char*)malloc(txt_sz);
		bzero(light_map_map_texture_alphas[txt_idx].data,txt_sz);
		
		glBindTexture(GL_TEXTURE_2D,bitmap->gl_id);
		glGetTexImage(GL_TEXTURE_2D,0,GL_ALPHA,GL_UNSIGNED_BYTE,(GLvoid*)light_map_map_texture_alphas[txt_idx].data);
	}
	
		// get the hit point
		
	hpt.x=spt->x+(int)(vct->x*hit_t);
	hpt.y=spt->y+(int)(vct->y*hit_t);
	hpt.z=spt->z+(int)(vct->z*hit_t);
	
		// hits on wall like polygons
		
	if (poly->box.wall_like) {
	
			// find points for uv extents
			
		lft_idx=rgt_idx=0;
		top_idx=bot_idx=0;
		
		for (n=1;n<poly->ptsz;n++) {
			pt=&mesh->vertexes[poly->v[n]];
			if ((pt->x==poly->line.lx) && (pt->z==poly->line.lz)) lft_idx=n;
			if ((pt->x==poly->line.rx) && (pt->z==poly->line.rz)) rgt_idx=n;
			if (pt->y==poly->box.min.y) top_idx=n;
			if (pt->y==poly->box.max.y) bot_idx=n;
		}
		
			// find distances that hit point is on
			// wall x/z plane
			
		dx=(double)(poly->line.rx-poly->line.lx);
		dz=(double)(poly->line.rz-poly->line.lz);
		d1=sqrt((dx*dx)+(dz*dz));
		
		dx=(double)(hpt.x-poly->line.lx);
		dz=(double)(hpt.z-poly->line.lz);
		d2=sqrt((dx*dx)+(dz*dz));
		
		fx=(float)(d2/d1);
		fx=poly->main_uv.x[lft_idx]+(fx*(poly->main_uv.x[rgt_idx]-poly->main_uv.x[lft_idx]));
		
			// find the distances for hit point on y plane
			
		fy=(float)(hpt.y-poly->box.min.y)/(float)(poly->box.max.y-poly->box.min.y);
		fy=poly->main_uv.y[top_idx]+(fy*(poly->main_uv.y[bot_idx]-poly->main_uv.y[top_idx]));
	}
	
		// hits on floor like polygons
		
	else {
	
			// find points for uv extents
			
		min_gx=max_gx=poly->main_uv.x[0];
		min_gy=max_gy=poly->main_uv.y[0];
		
		for (n=1;n<poly->ptsz;n++) {
			if (poly->main_uv.x[n]<min_gx) min_gx=poly->main_uv.x[n];
			if (poly->main_uv.x[n]>max_gx) max_gx=poly->main_uv.x[n];
			if (poly->main_uv.y[n]<min_gx) min_gy=poly->main_uv.y[n];
			if (poly->main_uv.y[n]>max_gx) max_gy=poly->main_uv.y[n];
		}
		
			// find the hit points in the box
			
		fx=(float)(hpt.x-poly->box.min.x)/(float)(poly->box.max.x-poly->box.min.x);
		fx=min_gx+(fx*(max_gx-min_gx));
	
		fy=(float)(hpt.z-poly->box.min.z)/(float)(poly->box.max.z-poly->box.min.z);
		fy=min_gy+(fy*(max_gy-min_gy));
	}

		// get the alpha

	fx-=floor(fx);
	fy-=floor(fy);

	x=(int)(((float)bitmap->wid)*fx);
	y=(int)(((float)bitmap->high)*fy);
	
	aptr=light_map_map_texture_alphas[txt_idx].data+(x+(y*bitmap->wid));
	return(*aptr==0x0);	
}

/* =======================================================

      Light Map Ray Trace
      
======================================================= */

inline void ray_trace_create_vector_from_points(d3vct *v,int x1,int y1,int z1,int x2,int y2,int z2)
{
	v->x=(float)(x1-x2);
	v->y=(float)(y1-y2);
	v->z=(float)(z1-z2);
}

inline void ray_trace_vector_cross_product(d3vct *cp,d3vct *v1,d3vct *v2)
{
	cp->x=(v1->y*v2->z)-(v2->y*v1->z);
	cp->y=(v1->z*v2->x)-(v2->z*v1->x);
	cp->z=(v1->x*v2->y)-(v2->x*v1->y);
}

inline float ray_trace_vector_inner_product(d3vct *v1,d3vct *v2)
{
	return((v1->x*v2->x)+(v1->y*v2->y)+(v1->z*v2->z));
}

float light_map_ray_trace_triangle(d3pnt *spt,d3vct *vct,int *x,int *y,int *z)
{
	float				det,invDet,t,u,v;
	d3vct				perpVector,lineToTrigPointVector,lineToTrigPerpVector,v1,v2;
	
		// get triangle vectors
		
	ray_trace_create_vector_from_points(&v1,x[1],y[1],z[1],x[0],y[0],z[0]);
	ray_trace_create_vector_from_points(&v2,x[2],y[2],z[2],x[0],y[0],z[0]);
	
		// calculate the determinate

	ray_trace_vector_cross_product(&perpVector,vct,&v2);
	det=ray_trace_vector_inner_product(&v1,&perpVector);
	
		// is line on the same plane as triangle?
		
	if ((det>-0.00001f) && (det<0.00001f)) return(-1.0f);

		// get the inverse determinate

	invDet=1.0f/det;

		// calculate triangle U and test
	
	ray_trace_create_vector_from_points(&lineToTrigPointVector,spt->x,spt->y,spt->z,x[0],y[0],z[0]);
	u=invDet*ray_trace_vector_inner_product(&lineToTrigPointVector,&perpVector);
	if ((u<0.0f) || (u>1.0f)) return(-1.0f);
	
		// calculate triangle V and test

	ray_trace_vector_cross_product(&lineToTrigPerpVector,&lineToTrigPointVector,&v1);
	v=invDet*ray_trace_vector_inner_product(vct,&lineToTrigPerpVector);
	if ((v<0.0f) || ((u+v)>1.0f)) return(-1.0f);
	
		// get line T for point(t) =  start_point + (vector*t)
		// -t are on the negative vector behind the point, so ignore

	t=invDet*ray_trace_vector_inner_product(&v2,&lineToTrigPerpVector);
	if (t<0.0f) return(-1.0f);
	
		// a hit!
		
	return(t);
}

bool light_map_ray_trace_mesh_polygon(d3pnt *spt,d3vct *vct,map_mesh_type *mesh,map_mesh_poly_type *poly)
{
	int			n,trig_count;
	int			px[3],py[3],pz[3];
	float		hit_t;
	d3pnt		*m_pt;
	
		// first vertex is always 0

	m_pt=&mesh->vertexes[poly->v[0]];
		
	px[0]=m_pt->x;
	py[0]=m_pt->y;
	pz[0]=m_pt->z;
	
		// run through all the triangles of the polygon
		
	trig_count=poly->ptsz-2;
	
	for (n=0;n<trig_count;n++) {
		m_pt=&mesh->vertexes[poly->v[n+1]];
		px[1]=m_pt->x;
		py[1]=m_pt->y;
		pz[1]=m_pt->z;

		m_pt=&mesh->vertexes[poly->v[n+2]];
		px[2]=m_pt->x;
		py[2]=m_pt->y;
		pz[2]=m_pt->z;
		
			// check for hit
			
		hit_t=light_map_ray_trace_triangle(spt,vct,px,py,pz);
		
			// ignore 0.0 hits to stop edge to edge polygon
			// collisions
		
		if ((hit_t>0.0f) && (hit_t<=1.0f)) {
			if (light_map_bitmap_transparency_check(spt,vct,mesh,poly,hit_t)) return(TRUE);
		}
	}
	
	return(FALSE);
}

bool light_map_ray_trace_map(int mesh_idx,int poly_idx,d3pnt *spt,d3pnt *ept)
{
	int							n,k;
	d3pnt						min,max;
	d3vct						vct;
	map_mesh_type				*mesh;
	map_mesh_poly_type			*poly;
	
		// ray trace vector
		
	vct.x=(float)(ept->x-spt->x);
	vct.y=(float)(ept->y-spt->y);
	vct.z=(float)(ept->z-spt->z);
	
		// ray trace bounds
		
	if (spt->x<ept->x) {
		min.x=spt->x;
		max.x=ept->x;
	}
	else {
		min.x=ept->x;
		max.x=spt->x;
	}
	if (spt->y<ept->y) {
		min.y=spt->y;
		max.y=ept->y;
	}
	else {
		min.y=ept->y;
		max.y=spt->y;
	}
	if (spt->z<ept->z) {
		min.z=spt->z;
		max.z=ept->z;
	}
	else {
		min.z=ept->z;
		max.z=spt->z;
	}
	
		// look for poly collisions
		// ony check opaque, non-cut-outs
	
	for (n=0;n!=map.mesh.nmesh;n++) {
		mesh=&map.mesh.meshes[n];
	
		if ((mesh->box.min.x>max.x) || (mesh->box.max.x<min.x)) continue;
		if ((mesh->box.min.y>max.y) || (mesh->box.max.y<min.y)) continue;
		if ((mesh->box.min.z>max.z) || (mesh->box.max.z<min.z)) continue;
		
		if (mesh->flag.skip_light_map_trace) continue;
			
		for (k=0;k!=mesh->npoly;k++) {
			poly=&mesh->polys[k];
		
			if ((poly->box.min.x>max.x) || (poly->box.max.x<min.x)) continue;
			if ((poly->box.min.y>max.y) || (poly->box.max.y<min.y)) continue;
			if ((poly->box.min.z>max.z) || (poly->box.max.z<min.z)) continue;
	
			if ((n==mesh_idx) && (k==poly_idx)) continue;		// don't check original poly
			
			if (light_map_ray_trace_mesh_polygon(spt,&vct,mesh,poly)) return(TRUE);
		}
	}

	return(FALSE);
}

void light_map_ray_trace(int mesh_idx,int poly_idx,d3pnt *rpt,unsigned char *uc_col)
{
	int					n;
	float				f;
	d3col				col;
	double				d,d_intensity,dist,dx,dy,dz;
	map_light_type		*lit;
	
		// check the lights
		
	col.r=col.g=col.b=0.0f;
	
	for (n=0;n!=map.nlight;n++) {
		lit=&map.lights[n];
		if (!lit->light_map) continue;
		
			// light within radius?
			
		d_intensity=(double)lit->intensity;
			
		dx=(lit->pnt.x-rpt->x);
		dy=(lit->pnt.y-rpt->y);
		dz=(lit->pnt.z-rpt->z);
		dist=sqrt((dx*dx)+(dy*dy)+(dz*dz));
		if (dist>d_intensity) continue;

			// is it visible?
			
		if (light_map_ray_trace_map(mesh_idx,poly_idx,rpt,&lit->pnt)) continue;
		
			// get color
			
		d=1.0-(dist/d_intensity);
		d+=pow(d,(double)lit->exponent);
		f=(float)d;
		
		col.r+=lit->col.r*f;
		col.g+=lit->col.g*f;
		col.b+=lit->col.b*f;
	}
	
	if (col.r>1.0f) col.r=1.0f;
	if (col.g>1.0f) col.g=1.0f;
	if (col.b>1.0f) col.b=1.0f;
	
	uc_col[0]=(unsigned char)(col.r*255.0f);
	uc_col[1]=(unsigned char)(col.g*255.0f);
	uc_col[2]=(unsigned char)(col.b*255.0f);
}

/* =======================================================

      Light Map Polygon Render
      
======================================================= */

void light_map_render_poly_get_point_add_margin(light_map_poly_type *lm_poly,d3pnt *pt)
{
	int			n;
	d3pnt		mpt;
	
		// get extents and center
	
	mpt.x=mpt.y=mpt.z=0;
	
	for (n=0;n!=lm_poly->ptsz;n++) {
		memmove(&pt[n],&lm_poly->pt[n],sizeof(d3pnt));
		mpt.x+=pt[n].x;
		mpt.y+=pt[n].y;
		mpt.z+=pt[n].z;
	}
	
	mpt.x/=lm_poly->ptsz;
	mpt.y/=lm_poly->ptsz;
	mpt.z/=lm_poly->ptsz;
	
		// move inside center
		
	for (n=0;n!=lm_poly->ptsz;n++) {
	
		if (pt[n].x!=mpt.x) {
			if (pt[n].x<mpt.x) {
				pt[n].x+=light_map_render_margin;
			}
			else {
				pt[n].x-=light_map_render_margin;
			}
		}
		if (pt[n].y!=mpt.y) {
			if (pt[n].y<mpt.y) {
				pt[n].y+=light_map_render_margin;
			}
			else {
				pt[n].y-=light_map_render_margin;
			}
		}
		if (pt[n].z!=mpt.z) {
			if (pt[n].z<mpt.z) {
				pt[n].z+=light_map_render_margin;
			}
			else {
				pt[n].z-=light_map_render_margin;
			}
		}
	}
}

bool light_map_render_poly(int lm_poly_idx,unsigned char *solid_color,light_map_texture_type *lmap)
{
	int							n,x,y,ty,by,x1,x2,x_start,x_end,x_count,y1,y2,
								top_idx,bot_idx,l1_start_idx,l1_end_idx,l2_start_idx,l2_end_idx,
								px[8],py[8];
	bool						first_col,match_col;
	d3pnt						pt[8],pt1,pt2,rpt;
	unsigned char				*pixel,*touch;
	unsigned char				col[3];
	light_map_poly_type			*lm_poly;
	
		// get the poly to render
		
	lm_poly=&light_map_polys[lm_poly_idx];
		
		// create the 2D drawing points and
		// determine the top and bottom vertex of the polygon
		
	top_idx=bot_idx=0;
		
	for (n=0;n!=lm_poly->ptsz;n++) {
		px[n]=lm_poly->x[n]+lm_poly->x_shift;
		py[n]=lm_poly->y[n]+lm_poly->y_shift;
		if (py[n]<py[top_idx]) top_idx=n;
		if (py[n]>py[bot_idx]) bot_idx=n;
	}
	
		// get the 3D drawing points and
		// add some margins to the points
		// to fix potential edge collisions
		
	light_map_render_poly_get_point_add_margin(lm_poly,pt);

		// find the line indexes

	l1_start_idx=top_idx;
	l1_end_idx=l1_start_idx-1;
	if (l1_end_idx<0) l1_end_idx=lm_poly->ptsz-1;

	l2_start_idx=top_idx;
	l2_end_idx=l2_start_idx+1;
	if (l2_end_idx==lm_poly->ptsz) l2_end_idx=0;
	
		// special check for solid colors
		
	first_col=TRUE;
	match_col=TRUE;
	
	if (solid_color!=NULL) solid_color[0]=solid_color[1]=solid_color[2]=0x0;

		// scan through the polygon
		
	ty=py[top_idx];
	by=py[bot_idx];
	
	for (y=ty;y<=by;y++) {
		
			// have the lines changed?

		if (y==py[l1_end_idx]) {
			l1_start_idx=l1_end_idx;
			l1_end_idx=l1_start_idx-1;
			if (l1_end_idx<0) l1_end_idx=lm_poly->ptsz-1;
		}

		if (y==py[l2_end_idx]) {
			l2_start_idx=l2_end_idx;
			l2_end_idx=l2_start_idx+1;
			if (l2_end_idx==lm_poly->ptsz) l2_end_idx=0;
		}

			// get points

		y1=py[l1_end_idx]-py[l1_start_idx];
		if (y1==0) {
			x1=px[l1_start_idx];
			memmove(&pt1,&pt[l1_start_idx],sizeof(d3pnt));
		}
		else {
			x1=px[l1_start_idx]+(((px[l1_end_idx]-px[l1_start_idx])*(y-py[l1_start_idx]))/y1);
			pt1.x=pt[l1_start_idx].x+(((pt[l1_end_idx].x-pt[l1_start_idx].x)*(y-py[l1_start_idx]))/y1);
			pt1.y=pt[l1_start_idx].y+(((pt[l1_end_idx].y-pt[l1_start_idx].y)*(y-py[l1_start_idx]))/y1);
			pt1.z=pt[l1_start_idx].z+(((pt[l1_end_idx].z-pt[l1_start_idx].z)*(y-py[l1_start_idx]))/y1);
		}
		
		y2=py[l2_end_idx]-py[l2_start_idx];
		if (y2==0) {
			x2=px[l2_start_idx];
			memmove(&pt2,&pt[l2_start_idx],sizeof(d3pnt));
		}
		else {
			x2=px[l2_start_idx]+(((px[l2_end_idx]-px[l2_start_idx])*(y-py[l2_start_idx]))/y2);
			pt2.x=pt[l2_start_idx].x+(((pt[l2_end_idx].x-pt[l2_start_idx].x)*(y-py[l2_start_idx]))/y2);
			pt2.y=pt[l2_start_idx].y+(((pt[l2_end_idx].y-pt[l2_start_idx].y)*(y-py[l2_start_idx]))/y2);
			pt2.z=pt[l2_start_idx].z+(((pt[l2_end_idx].z-pt[l2_start_idx].z)*(y-py[l2_start_idx]))/y2);
		}
		
			// get the scan line
			
		if (x1<x2) {
			x_start=x1;
			x_end=x2;
		}
		else {
			x_start=x2;
			x_end=x1;
			memmove(&rpt,&pt1,sizeof(d3pnt));
			memmove(&pt1,&pt2,sizeof(d3pnt));
			memmove(&pt2,&rpt,sizeof(d3pnt));
		}
		
			// NULL lmap means just check if it's all in black
			
		if (lmap!=NULL) {
			pixel=lmap->pixel_data+(((map.settings.light_map_size*3)*y)+(x_start*3));
			touch=lmap->pixel_touch+((map.settings.light_map_size*y)+x_start);
		}
		
			// draw the scan line
			
		for (x=x_start;x<=x_end;x++) {
		
				// find 3D point for 2D point
				
			x_count=x_end-x_start;
			
			if (x_count==0) {
				rpt.x=(pt1.x+pt2.x)*0.5f;
				rpt.y=(pt1.y+pt2.y)*0.5f;
				rpt.z=(pt1.z+pt2.z)*0.5f;
			}
			else {
				rpt.x=pt1.x+(((pt2.x-pt1.x)*(x-x_start))/x_count);
				rpt.y=pt1.y+(((pt2.y-pt1.y)*(x-x_start))/x_count);
				rpt.z=pt1.z+(((pt2.z-pt1.z)*(x-x_start))/x_count);
			}
			
			light_map_ray_trace(lm_poly->mesh_idx,lm_poly->poly_idx,&rpt,col);
			
			if (lmap!=NULL) {
				*pixel++=col[0];
				*pixel++=col[1];
				*pixel++=col[2];
				
				*touch++=0x1;
			}
			
			if (solid_color!=NULL) {
				if (first_col) {
					solid_color[0]=col[0];
					solid_color[1]=col[1];
					solid_color[2]=col[2];
					first_col=FALSE;
				}
				else {
					match_col=match_col&&(col[0]==solid_color[0])&&(col[1]==solid_color[1])&&(col[2]==solid_color[2]);
				}
			}
		}

	}
	
	if (solid_color!=NULL) {
		if (match_col) return(TRUE);
	}
	
	return(FALSE);
}

/* =======================================================

      Solid Color Cache
      
======================================================= */

bool light_map_find_solid_color(unsigned char *col,int *txt_idx,int *x,int *y)
{
	int									n;
	light_map_solid_color_cache_type	*cache;

	cache=light_map_solid_color_cache;
	
	for (n=0;n!=light_map_solid_color_cache_count;n++) {
		if ((cache->col[0]==col[0]) && (cache->col[1]==col[1]) && (cache->col[2]==col[2])) {
			*txt_idx=cache->txt_idx;
			*x=cache->x_shift;
			*y=cache->y_shift;
			return(TRUE);
		}
		cache++;
	}
	
	return(FALSE);
}

void light_map_add_solid_color(unsigned char *col,int txt_idx,int x,int y)
{
	light_map_solid_color_cache_type		*cache;
	
	if (light_map_solid_color_cache_count==max_light_map_solid_color) return;
	
	cache=&light_map_solid_color_cache[light_map_solid_color_cache_count];
	light_map_solid_color_cache_count++;
	
	cache->txt_idx=txt_idx;
	cache->x_shift=x;
	cache->y_shift=y;
	
	memmove(cache->col,col,3);
}

/* =======================================================

      Create Light Map for Mesh
      
======================================================= */

bool light_map_run_for_poly(int lm_poly_idx,char *err_str)
{
	int							n,x,y,txt_idx;
	unsigned char				solid_col[3];
	d3rect						box;
	light_map_poly_type			*lm_poly;
	light_map_texture_type		*lmap;

	lm_poly=&light_map_polys[lm_poly_idx];
	
		// detect if this poly's
		// light map is a solid color
	
	lm_poly->x_shift=lm_poly->y_shift=0;
	lm_poly->txt_idx=0;
		
	lm_poly->solid_color=light_map_render_poly(lm_poly_idx,solid_col,NULL);
	
		// solid color lightmaps only take 1 block
		// check to see if we already made a solid block
		// for this, if so, use that and skip out
		// if it's new, add it to the cache
		
	if (lm_poly->solid_color) {
		lm_poly->x_sz=lm_poly->y_sz=light_map_texture_block_size;
		
		if (light_map_find_solid_color(solid_col,&txt_idx,&x,&y)) {
			lm_poly->txt_idx=txt_idx;
			lm_poly->x_shift=x;
			lm_poly->y_shift=y;
			return(TRUE);
		}
	}
	
		// can this polygon fit in any of the current
		// light maps?
		
	txt_idx=-1;
	
	for (n=0;n!=max_light_map_textures;n++) {
		lmap=&light_map_textures[n];
		if (lmap->pixel_data==NULL) continue;
		
		if (light_map_texture_find_open_area(lm_poly->x_sz,lm_poly->y_sz,&x,&y,&box,lm_poly->solid_color,lmap)) {
			txt_idx=n;
			break;
		}
	}

		// need to create a new light map?
		
	if (txt_idx==-1) {
	
		txt_idx=light_map_textures_create();
		if (txt_idx==-1) {
			sprintf(err_str,"Too many polys in map, would create more than %d light maps",max_light_map_textures);
			return(FALSE);
		}
		
		light_map_texture_find_open_area(lm_poly->x_sz,lm_poly->y_sz,&x,&y,&box,lm_poly->solid_color,&light_map_textures[txt_idx]);
	}
	
	lmap=&light_map_textures[txt_idx];
	
		// remember the shift for creating the UVs
		
	lm_poly->x_shift=x;
	lm_poly->y_shift=y;
	
	memmove(&lm_poly->box,&box,sizeof(d3rect));
	
	lm_poly->txt_idx=txt_idx;
	
		// special check for all solid colors
		// light maps
		
	if (lm_poly->solid_color) {
		light_map_texture_fill_solid_color(solid_col,x,y,lmap);
		light_map_add_solid_color(solid_col,txt_idx,x,y);
		return(TRUE);
	}

		// render the polygon
		
	light_map_render_poly(lm_poly_idx,NULL,lmap);
	
	return(TRUE);
}

/* =======================================================

      Set UVs and Textures
      
======================================================= */

void light_map_set_texture_uv_mesh_poly(light_map_poly_type *lm_poly)
{
	int							n;
	float						f_pixel_size,gx,gy;
	map_mesh_type				*mesh;
	map_mesh_poly_type			*poly;

	mesh=&map.mesh.meshes[lm_poly->mesh_idx];
	poly=&mesh->polys[lm_poly->poly_idx];
			
		// set light map texture
	
	poly->lmap_txt_idx=(max_map_texture-max_light_map_textures)+lm_poly->txt_idx;
	
		// UVs across the light map texture size
		
	f_pixel_size=(float)map.settings.light_map_size;

		// solid color maps
		// put the UV in the center of the block
		
	if (lm_poly->solid_color) {
	
		gx=(float)(lm_poly->x_shift+(light_map_texture_block_size>>1))/f_pixel_size;
		gy=(float)(lm_poly->y_shift+(light_map_texture_block_size>>1))/f_pixel_size;
		
		for (n=0;n!=poly->ptsz;n++) {
			poly->lmap_uv.x[n]=gx;
			poly->lmap_uv.y[n]=gy;
		}
		
		return;
	}
	
		// regular light mapping uvs
		
	for (n=0;n!=poly->ptsz;n++) {
		poly->lmap_uv.x[n]=((float)(lm_poly->x[n]+lm_poly->x_shift))/f_pixel_size;
		poly->lmap_uv.y[n]=((float)(lm_poly->y[n]+lm_poly->y_shift))/f_pixel_size;
	}
}

void light_map_set_texture_uv_liquid(light_map_poly_type *lm_poly)
{
	float						f_pixel_size;
	map_liquid_type				*liq;

	liq=&map.liquid.liquids[lm_poly->liquid_idx];
			
		// set light map texture
	
	liq->lmap_txt_idx=(max_map_texture-max_light_map_textures)+lm_poly->txt_idx;
	
		// UVs across the light map texture size
		
	f_pixel_size=(float)map.settings.light_map_size;

		// solid color maps
		// put the UV in the center of the block
		
	if (lm_poly->solid_color) {
		liq->lmap_uv.x_offset=(float)(lm_poly->x_shift+(light_map_texture_block_size>>1))/f_pixel_size;
		liq->lmap_uv.y_offset=(float)(lm_poly->y_shift+(light_map_texture_block_size>>1))/f_pixel_size;
		liq->lmap_uv.x_size=liq->lmap_uv.y_size=0.0f;
		return;
	}
	
		// regular light mapping uvs
		
	liq->lmap_uv.x_offset=((float)(lm_poly->x[0]+lm_poly->x_shift))/f_pixel_size;
	liq->lmap_uv.y_offset=((float)(lm_poly->y[0]+lm_poly->y_shift))/f_pixel_size;
	liq->lmap_uv.x_size=((float)(lm_poly->x[1]-lm_poly->x[0]))/f_pixel_size;
	liq->lmap_uv.y_size=((float)(lm_poly->y[3]-lm_poly->y[0]))/f_pixel_size;
}

void light_map_set_texture_uv(int lm_poly_idx)
{
	light_map_poly_type			*lm_poly;

	lm_poly=&light_map_polys[lm_poly_idx];

	if (lm_poly->mesh_idx!=-1) {
		light_map_set_texture_uv_mesh_poly(lm_poly);
		return;
	}
	
	light_map_set_texture_uv_liquid(lm_poly);
}

/* =======================================================

      Run Light Maps
      
======================================================= */

bool light_maps_create_process(char *err_str)
{
	int				n;
	char			base_path[1024],dir_path[1024];
	
		// base path
		
	file_paths_data_default(&file_path_setup,base_path,"Bitmaps/Textures",NULL,NULL);
	
		// create folder if it doesn't exist
		
	sprintf(dir_path,"%s/LightMaps",base_path);
	mkdir(dir_path,S_IRWXU|S_IRWXG|S_IRWXO);
		
		// clear the textures and
		// start mesh-poly and/or liquid setup
		
	dialog_progress_next();
		
	if (!light_map_textures_start(err_str)) return(FALSE);
	
	if (!light_map_polys_start(err_str)) {
		light_map_textures_free();
		return(FALSE);
	}
	
	light_map_bitmap_transparency_start();
	
		// create a ray traced light map
		// for all the polys
		
	for (n=0;n!=light_map_poly_count;n++) {
		if ((n%20)==0) dialog_progress_next();
		
		if (!light_map_run_for_poly(n,err_str)) {
			light_map_bitmap_transparency_free();
			light_map_textures_free();
			light_map_polys_free();
			return(FALSE);
		}
		
	}
	
		// borders and blur
		
	light_map_textures_pixel_border();
	
	dialog_progress_next();
	light_map_textures_blur();
	
		// write all the textures
		
	dialog_progress_next();
	light_map_textures_save(base_path);
	
		// set light map textures
		// and UVs
		
	for (n=0;n!=light_map_poly_count;n++) {
		if ((n%20)==0) dialog_progress_next();
		light_map_set_texture_uv(n);
	}
	
		// free textures and polys
		
	dialog_progress_next();

	light_map_bitmap_transparency_free();
	light_map_textures_free();
	light_map_polys_free();

	main_wind_draw();
	
	return(TRUE);
}

bool light_maps_create(char *err_str)
{
	int				npoly;
	bool			ok;
	
	npoly=light_map_get_poly_count();
	
	dialog_progress_start("Generating Light Maps...",(3+(max_light_map_textures*3)+((npoly/20)*2)));
	ok=light_maps_create_process(err_str);
	dialog_progress_end();
	
	if (ok) file_save_map();
	
	return(ok);
}

void light_maps_clear(void)
{
	int					n,k;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

		// delete old textures
		
	map_delete_texture(&map,(max_map_texture-max_light_map_textures),max_map_texture);

		// clear polys
	
	mesh=map.mesh.meshes;
	
	for (n=0;n!=map.mesh.nmesh;n++) {
				
		poly=mesh->polys;
		
		for (k=0;k!=mesh->npoly;k++) {
			poly->lmap_txt_idx=-1;
			poly++;
		}
		
		mesh++;
	}
}