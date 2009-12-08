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
#define light_map_smear_count						6

typedef struct		{
						unsigned char				*block,*pixel_data,*pixel_touch,
													*backup_block,*backup_pixel_data,*backup_pixel_touch;
					} light_map_texture_type;
					
typedef struct		{
						int							x[8],y[8],
													x_shift,y_shift,
													x_sz,y_sz;
						bool						has_light;
						d3rect						box;
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
		light_map_textures[n].pixel_data=NULL;
		light_map_textures[n].pixel_touch=NULL;
		light_map_textures[n].backup_block=NULL;
		light_map_textures[n].backup_pixel_data=NULL;
		light_map_textures[n].backup_pixel_touch=NULL;
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
	
	lmap->backup_block=(unsigned char*)malloc(sz);
	
		// pixel data
		
	sz=(map.settings.light_map_size*map.settings.light_map_size)*3;
	lmap->pixel_data=(unsigned char*)malloc(sz);
	bzero(lmap->pixel_data,sz);
	
	lmap->backup_pixel_data=(unsigned char*)malloc(sz);
	
		// pixel touch data
		
	sz=map.settings.light_map_size*map.settings.light_map_size;
	lmap->pixel_touch=(unsigned char*)malloc(sz);
	bzero(lmap->pixel_touch,sz);
	
	lmap->backup_pixel_touch=(unsigned char*)malloc(sz);
	
		// all light maps have the very
		// first block reserved and set to black so polygons
		// that have no light can be optimized to
		// have their light map here
		
	*lmap->block=0x1;
	
	return(idx);
}

void light_map_textures_backup(light_map_texture_type *lmap)
{
	int						sz;
	
	sz=map.settings.light_map_size/light_map_texture_block_size;
	sz=sz*sz;
	memmove(lmap->backup_block,lmap->block,sz);
	
	sz=(map.settings.light_map_size*map.settings.light_map_size)*3;
	memmove(lmap->backup_pixel_data,lmap->pixel_data,sz);
	
	sz=map.settings.light_map_size*map.settings.light_map_size;
	memmove(lmap->backup_pixel_touch,lmap->pixel_touch,sz);
}

void light_map_textures_restore(light_map_texture_type *lmap)
{
	int						sz;
	
	sz=map.settings.light_map_size/light_map_texture_block_size;
	sz=sz*sz;
	memmove(lmap->block,lmap->backup_block,sz);
	
	sz=(map.settings.light_map_size*map.settings.light_map_size)*3;
	memmove(lmap->pixel_data,lmap->backup_pixel_data,sz);
	
	sz=map.settings.light_map_size*map.settings.light_map_size;
	memmove(lmap->pixel_touch,lmap->backup_pixel_touch,sz);
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
		if (light_map_textures[n].backup_block!=NULL) free(light_map_textures[n].backup_block);
		if (light_map_textures[n].backup_pixel_data!=NULL) free(light_map_textures[n].backup_pixel_data);
		if (light_map_textures[n].backup_pixel_touch!=NULL) free(light_map_textures[n].backup_pixel_touch);
	}
	
	free(light_map_textures);
}

/* =======================================================

      Light Texture Bluring
      
======================================================= */

void light_map_texture_single_blur_edges(light_map_texture_type *lmap,int blur_count,bool smear)
{
	int				n,x,y,cx,cy,i_col[3],col_count;
	unsigned char	*pixel,*pixel_touch,
					*back,*back_touch,
					*blur,*blur_touch;
	
	for (n=0;n!=blur_count;n++) {
	
		for (y=0;y!=map.settings.light_map_size;y++) {
		
			pixel=lmap->pixel_data+((map.settings.light_map_size*3)*y);
			pixel_touch=lmap->pixel_touch+(map.settings.light_map_size*y);
			
			back=lmap->backup_pixel_data+((map.settings.light_map_size*3)*y);
			back_touch=lmap->backup_pixel_touch+(map.settings.light_map_size*y);
			
			for (x=0;x!=map.settings.light_map_size;x++) {
			
					// smearing only effects non-touched pixels
					// blur effects all pixels
					
				if (smear) {
					if (*pixel_touch!=0x0) {
						*back++=*pixel++;
						*back++=*pixel++;
						*back++=*pixel++;
						pixel_touch++;
						back_touch++;
						continue;
					}
				}
				
					// get blur from 8 surrounding pixels
					
				col_count=0;
				i_col[0]=i_col[1]=i_col[2]=0;
				
				for (cy=(y-1);cy!=(y+2);cy++) {
					for (cx=(x-1);cx!=(x+2);cx++) {
					
						if ((cy==y) && (cx==x)) continue;
						if ((cy<0) || (cy>=map.settings.light_map_size) || (cx<0) || (cx>=map.settings.light_map_size)) continue;
					
							// if in smear mode, only blur from touched
							// pixels to smear the touched pixels colors
							
						if (smear) {
							blur_touch=lmap->pixel_touch+((map.settings.light_map_size*cy)+cx);
							if (*blur_touch==0x0) continue;
						}
						
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
					
					if (smear) *back_touch=0x1;		// next time this is part of the smear
				}
				else {
					back+=3;
				}
				
				pixel+=3;
				
				pixel_touch++;
				back_touch++;
			}
			
		}
		
		memmove(lmap->pixel_data,lmap->backup_pixel_data,((map.settings.light_map_size*3)*map.settings.light_map_size));
		memmove(lmap->pixel_touch,lmap->backup_pixel_touch,(map.settings.light_map_size*map.settings.light_map_size));
	}
		
}

void light_map_textures_blur_edges(void)
{
	int							n;
	light_map_texture_type		*lmap;
	
		// this is a two-step process.  First we smear the texture
		// by using out touch flag as a mask and smearing the touched
		// values.  This eliminates the polygon edge gray areas
		// second, we blur the entire light map to fix any stark light changes
		// within the light map itself
	
	lmap=light_map_textures;
	
	for (n=0;n!=max_light_map_textures;n++) {
		if (lmap->pixel_data!=NULL) {
			light_map_texture_single_blur_edges(lmap,light_map_smear_count,TRUE);
			light_map_texture_single_blur_edges(lmap,map.settings.light_map_blur_count,FALSE);
		}
		lmap++;
	}
}

/* =======================================================

      Light Maps Poly Setup
      
======================================================= */

void light_map_create_mesh_poly_flatten(map_mesh_type *mesh,map_mesh_poly_type *poly,int v_idx,int *px,int *py)
{
	float				factor;
	double				dx,dz;
	d3pnt				*pt;
	
	pt=&mesh->vertexes[poly->v[v_idx]];

	if (poly->box.wall_like) {
		dx=(double)(pt->x-poly->line.lx);
		dz=(double)(pt->z-poly->line.lz);
		
		*px=(int)sqrt((dx*dx)+(dz*dz));
		*py=pt->y-poly->box.min.y;
	}
	else {
		*px=pt->x-poly->box.min.x;
		*py=pt->z-poly->box.min.z;
	}
	
		// reduce to pixel factor (size of render)
		
	factor=((float)map.settings.light_map_quality)/light_map_quality_to_pixel_factor;
	
	*px=(int)((float)(*px)*factor);
	*py=(int)((float)(*py)*factor);
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
			
				// can this poly by itself fit in map?
				
			if ((lm_poly->x_sz>=map.settings.light_map_size) || (lm_poly->y_sz>=map.settings.light_map_size)) {
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

bool light_map_texture_find_open_area(int x_sz,int y_sz,int *kx,int *ky,d3rect *box,unsigned char blk,light_map_texture_type *lmap)
{
	int				x,y,bx,by,block_count,b_x_sz,b_y_sz;
	bool			hit;
	unsigned char	*bptr;
	
		// get block size
		
	b_x_sz=(x_sz/light_map_texture_block_size);
	if ((x_sz%light_map_texture_block_size)!=0) b_x_sz++;
	
	b_y_sz=(y_sz/light_map_texture_block_size);
	if ((y_sz%light_map_texture_block_size)!=0) b_y_sz++;
	
	block_count=map.settings.light_map_size/light_map_texture_block_size;
	
		// always take one extra block on each
		// side so we can better center the polygon
		// in the middle and smear the colors
		
	b_x_sz++;
	b_y_sz++;
	
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
					*bptr++=blk;
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
			
				// move x,y into center so we have more
				// room to smear at top/left
				
			*kx=x+(light_map_texture_block_size>>1);
			*ky=y+(light_map_texture_block_size>>1);

			return(TRUE);
		}
	}

	return(FALSE);
}

void light_map_texture_clear_block_type(unsigned char blk,light_map_texture_type *lmap)
{
	int				n,block_count;
	unsigned char	*bptr;
	
	block_count=map.settings.light_map_size/light_map_texture_block_size;
	block_count*=block_count;
		
	bptr=lmap->block;
	
	for (n=0;n!=block_count;n++) {
		if ((*bptr)==blk) *bptr=0x0;
		bptr++;
	}
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

bool light_map_ray_trace_triangle(d3pnt *spt,d3vct *vct,int *x,int *y,int *z)
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
		
	if ((det>-0.00001f) && (det<0.00001f)) return(FALSE);

		// get the inverse determinate

	invDet=1.0f/det;

		// calculate triangle U and test
	
	ray_trace_create_vector_from_points(&lineToTrigPointVector,spt->x,spt->y,spt->z,x[0],y[0],z[0]);
	u=invDet*ray_trace_vector_inner_product(&lineToTrigPointVector,&perpVector);
	if ((u<0.0f) || (u>1.0f)) return(FALSE);
	
		// calculate triangle V and test

	ray_trace_vector_cross_product(&lineToTrigPerpVector,&lineToTrigPointVector,&v1);
	v=invDet*ray_trace_vector_inner_product(vct,&lineToTrigPerpVector);
	if ((v<0.0f) || ((u+v)>1.0f)) return(FALSE);
	
		// get line T for point(t) =  start_point + (vector*t)
		// -t are on the negative vector behind the point, so ignore

	t=invDet*ray_trace_vector_inner_product(&v2,&lineToTrigPerpVector);
	if (t<0.0f) return(FALSE);
	
		// a hit!
		// ignore 0.0 hits to stop edge to edge polygon
		// collisions
		
	return((t>0.0f) && (t<=1.0f));
}

bool light_map_ray_trace_mesh_polygon(d3pnt *spt,d3vct *vct,map_mesh_type *mesh,map_mesh_poly_type *poly)
{
	int			n,trig_count;
	int			px[3],py[3],pz[3];
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
			
		if (light_map_ray_trace_triangle(spt,vct,px,py,pz)) return(TRUE);
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
			
		for (k=0;k!=mesh->npoly;k++) {
			poly=&mesh->polys[k];
		
			if ((poly->box.min.x>max.x) || (poly->box.max.x<min.x)) continue;
			if ((poly->box.min.y>max.y) || (poly->box.max.y<min.y)) continue;
			if ((poly->box.min.z>max.z) || (poly->box.max.z<min.z)) continue;
			
			if ((poly->alpha!=1.0f) || (map.textures[poly->txt_idx].frames[0].bitmap.alpha_mode!=alpha_mode_none)) continue;
	
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

bool light_map_render_poly(int mesh_idx,int poly_idx,int ptsz,int *px,int *py,d3pnt *pt,light_map_texture_type *lmap)
{
	int				n,x,y,ty,by,x1,x2,x_start,x_end,x_count,y1,y2,zero_check,
					top_idx,bot_idx,l1_start_idx,l1_end_idx,l2_start_idx,l2_end_idx;
	d3pnt			pt1,pt2,rpt;
	unsigned char	*pixel,*touch;
	unsigned char	col[3];

		// determine the top and bottom vertex of the polygon

	top_idx=bot_idx=0;

	for (n=1;n<ptsz;n++) {
		if (py[n]<py[top_idx]) top_idx=n;
		if (py[n]>py[bot_idx]) bot_idx=n;
	}

		// find the line indexes

	l1_start_idx=top_idx;
	l1_end_idx=l1_start_idx-1;
	if (l1_end_idx<0) l1_end_idx=ptsz-1;

	l2_start_idx=top_idx;
	l2_end_idx=l2_start_idx+1;
	if (l2_end_idx==ptsz) l2_end_idx=0;
	
		// special check if there are
		// no colors in map
		
	zero_check=0;

		// scan through the polygon

	ty=py[top_idx];
	by=py[bot_idx];

	for (y=ty;y<=by;y++) {
		
			// have the lines changed?

		if (y==py[l1_end_idx]) {
			l1_start_idx=l1_end_idx;
			l1_end_idx=l1_start_idx-1;
			if (l1_end_idx<0) l1_end_idx=ptsz-1;
		}

		if (y==py[l2_end_idx]) {
			l2_start_idx=l2_end_idx;
			l2_end_idx=l2_start_idx+1;
			if (l2_end_idx==ptsz) l2_end_idx=0;
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
		
		pixel=lmap->pixel_data+(((map.settings.light_map_size*3)*y)+(x_start*3));
		touch=lmap->pixel_touch+((map.settings.light_map_size*y)+x_start);
		
			// draw the scan line
			
		for (x=x_start;x<=x_end;x++) {
		
				// find 3D point for 2D point
				
			x_count=x_end-x_start;
			
			if (x_count==0) {
				memmove(&rpt,&pt1,sizeof(d3pnt));
			}
			else {
				rpt.x=pt1.x+(((pt2.x-pt1.x)*(x-x_start))/x_count);
				rpt.y=pt1.y+(((pt2.y-pt1.y)*(x-x_start))/x_count);
				rpt.z=pt1.z+(((pt2.z-pt1.z)*(x-x_start))/x_count);
			}
			
			light_map_ray_trace(mesh_idx,poly_idx,&rpt,col);
			
			*pixel++=col[0];
			*pixel++=col[1];
			*pixel++=col[2];
			
			zero_check+=(int)col[0];
			zero_check+=(int)col[1];
			zero_check+=(int)col[2];
			
			*touch++=0x1;
		}

	}
	
	return(zero_check!=0);
}

/* =======================================================

      Create Light Map for Mesh
      
======================================================= */

void light_map_create_mesh_poly(int mesh_idx,int poly_idx,light_map_texture_type *lmap)
{
	int							n,x,y,px[8],py[8];
	d3pnt						pt[8];
	d3rect						box;
	map_mesh_type				*mesh;
	map_mesh_poly_type			*poly;
	light_map_mesh_type			*lm_mesh;
	light_map_mesh_poly_type	*lm_poly;
	
	mesh=&map.mesh.meshes[mesh_idx];
	poly=&mesh->polys[poly_idx];
	
	lm_mesh=&light_map_meshes[mesh_idx];
	lm_poly=&lm_mesh->polys[poly_idx];
	
		// backup the current light map
		// context.  If there are no hits
		// in the light map, we use restore
		// the old light map and use the
		// light map's special all black block
		
	light_map_textures_backup(lmap);
	
		// get rendering spot (need to get again as this
		// might be a new texture and block it off
		// from further rendering
		
	if (!light_map_texture_find_open_area(lm_poly->x_sz,lm_poly->y_sz,&x,&y,&box,0x1,lmap)) return;
		
		// remember the shift for creating the UVs
		
	lm_poly->x_shift=x;
	lm_poly->y_shift=y;
	
	memmove(&lm_poly->box,&box,sizeof(d3rect));

		// render the polygon
		
	for (n=0;n!=poly->ptsz;n++) {
		px[n]=lm_poly->x[n]+x;
		py[n]=lm_poly->y[n]+y;
		memmove(&pt[n],&mesh->vertexes[poly->v[n]],sizeof(d3pnt));
	}
		
	lm_poly->has_light=light_map_render_poly(mesh_idx,poly_idx,poly->ptsz,px,py,pt,lmap);

		// if no light, use special all
		// black light map
		
	if (!lm_poly->has_light) light_map_textures_restore(lmap);
}

bool light_map_create_mesh_fit_texture(int mesh_idx,light_map_texture_type *lmap)
{
	int							k,x,y;
	bool						txt_ok;
	map_mesh_type				*mesh;
	light_map_mesh_type			*lm_mesh;
	light_map_mesh_poly_type	*lm_poly;
	
	mesh=&map.mesh.meshes[mesh_idx];
	lm_mesh=&light_map_meshes[mesh_idx];

		// try to block out the meshes using 0x2 (so not
		// to interfere with previous 0x1 blocks) 
		
	txt_ok=TRUE;
	lm_poly=lm_mesh->polys;
	
	for (k=0;k!=mesh->npoly;k++) {
		if (!light_map_texture_find_open_area(lm_poly->x_sz,lm_poly->y_sz,&x,&y,NULL,0x2,lmap)) {
			txt_ok=FALSE;
			break;
		}
	
		lm_poly++;
	}
	
	light_map_texture_clear_block_type(0x2,lmap);

	return(txt_ok);
}

bool light_map_create_mesh(int mesh_idx,char *err_str)
{
	int							n,txt_idx;
	map_mesh_type				*mesh;
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
		if (lmap->pixel_data==NULL) continue;
		
		if (light_map_create_mesh_fit_texture(mesh_idx,lmap)) {
			txt_idx=n;
			break;
		}
	}

		// need to create a new one?
		
	if (txt_idx==-1) {
	
		txt_idx=light_map_textures_create();
		if (txt_idx==-1) {
			sprintf(err_str,"Too many polys in map, would create more than %d light maps",max_light_map_textures);
			return(FALSE);
		}
		
			// are we too big to fit this mesh in a single texture?
			
		if (!light_map_create_mesh_fit_texture(mesh_idx,&light_map_textures[txt_idx])) {
			sprintf(err_str,"The quality is too high and will cause a mesh %d to not be able to fit within a single light map",mesh_idx);
			return(FALSE);
		}
	}
	
	lmap=&light_map_textures[txt_idx];
	
		// remember this light map
		
	lm_mesh->txt_idx=txt_idx;
	
		// create the light map for each poly

	for (n=0;n!=mesh->npoly;n++) {
		light_map_create_mesh_poly(mesh_idx,n,lmap);
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
	
		// set light map texture
		
	mesh->lmap_txt_idx=(max_map_texture-max_light_map_textures)+lm_mesh->txt_idx;
	
		// make sure there are two UVs
		
	mesh->nuv=2;
	
		// add the alternate UVs
		
	f_pixel_size=(float)map.settings.light_map_size;
		
	poly=mesh->polys;
	lm_poly=lm_mesh->polys;
	
	for (n=0;n!=mesh->npoly;n++) {
	
			// if no light, use special black
			// block in all light maps
			
		if (!lm_poly->has_light) {
			for (k=0;k!=poly->ptsz;k++) {
				poly->uv[1].x[k]=poly->uv[1].y[k]=((float)(light_map_texture_block_size>>1))/f_pixel_size;
			}
		}
		
			// regular light mapping uvs
			
		else {
			for (k=0;k!=poly->ptsz;k++) {
				poly->uv[1].x[k]=((float)(lm_poly->x[k]+lm_poly->x_shift))/f_pixel_size;
				poly->uv[1].y[k]=((float)(lm_poly->y[k]+lm_poly->y_shift))/f_pixel_size;
			}
		}
		
		poly++;
		lm_poly++;
	}
	
}

/* =======================================================

      Run Light Maps
      
======================================================= */

bool light_maps_create(char *err_str)
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
		
	light_map_textures_blur_edges();
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

