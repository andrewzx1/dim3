/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Walk View Draw Piece Routines

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

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "interface.h"

extern map_type				map;
extern app_state_type		state;
extern app_pref_type		pref;
extern bitmap_type			spot_bitmap,scenery_bitmap,node_bitmap,node_defined_bitmap,
							light_bitmap,sound_bitmap,particle_bitmap;

int							view_mesh_sort_count;
view_mesh_sort_list_type	*view_mesh_sort_list;

extern bool obscure_mesh_view_bit_get(unsigned char *visibility_flag,int idx);

/* =======================================================

      Walk View Sprites Drawing
      
======================================================= */

void view_get_sprite_vertexes(d3pnt *pnt,d3ang *ang,d3pnt *v_pnts)
{
    int				n;
	float			fx[8],fy[8],fz[8];
	matrix_type		mat;
	
	fx[0]=fx[3]=fx[4]=fx[7]=-(float)view_sprite_size;
	fx[1]=fx[2]=fx[5]=fx[6]=(float)view_sprite_size;
	
	fy[0]=fy[1]=fy[2]=fy[3]=-(float)(view_sprite_size*2);
	fy[4]=fy[5]=fy[6]=fy[7]=0.0f;
	
	fz[0]=fz[1]=fz[4]=fz[5]=-(float)view_sprite_size;
	fz[2]=fz[3]=fz[6]=fz[7]=(float)view_sprite_size;
	
	if (ang!=NULL) matrix_rotate_xyz(&mat,ang->x,ang->y,ang->z);
		
	for (n=0;n!=8;n++) {
		if (ang!=NULL) matrix_vertex_multiply(&mat,&fx[n],&fy[n],&fz[n]);
		
		v_pnts[n].x=pnt->x+(int)fx[n];
		v_pnts[n].y=pnt->y+(int)fy[n];
		v_pnts[n].z=pnt->z+(int)fz[n];
	}
}

void view_draw_sprite(d3pnt *pnt,d3ang *ang,unsigned long gl_id)
{
	int				n,idx,face_idx[24]={0,1,2,3,4,5,6,7,0,1,5,4,3,2,6,7,1,2,6,5,0,3,7,4};
	float			vertexes[24*3],uvs[24*2];
	float			*pv,*pt;
	d3pnt			v_pnts[8];
	
	view_get_sprite_vertexes(pnt,ang,v_pnts);
	
		// draw sprite

	glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,gl_id);
   
	glColor4f(1.0f,1.0f,1.0f,1.0f);
	
	pv=vertexes;
	pt=uvs;
	
	for (n=0;n!=24;n+=4) {
	
		idx=face_idx[n];
		*pv++=(float)v_pnts[idx].x;
		*pv++=(float)v_pnts[idx].y;
		*pv++=(float)v_pnts[idx].z;
		*pt++=0.0f;
		*pt++=0.0f;
		
		idx=face_idx[n+1];
		*pv++=(float)v_pnts[idx].x;
		*pv++=(float)v_pnts[idx].y;
		*pv++=(float)v_pnts[idx].z;
		*pt++=1.0f;
		*pt++=0.0f;
	
		idx=face_idx[n+2];
		*pv++=(float)v_pnts[idx].x;
		*pv++=(float)v_pnts[idx].y;
		*pv++=(float)v_pnts[idx].z;
		*pt++=1.0f;
		*pt++=1.0f;
		
		idx=face_idx[n+3];
		*pv++=(float)v_pnts[idx].x;
		*pv++=(float)v_pnts[idx].y;
		*pv++=(float)v_pnts[idx].z;
		*pt++=0.0f;
		*pt++=1.0f;
	}
	
	glVertexPointer(3,GL_FLOAT,0,vertexes);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,uvs);

	glDrawArrays(GL_QUADS,0,24);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glDisable(GL_TEXTURE_2D);
}

void view_draw_circle(d3pnt *pnt,d3ang *ang,d3col *col,int dist,bool checkpoint)
{
    int				n,kx,ky,kz;
	float			vertexes[36*3];
	float			*pv;

	glLineWidth(4.0f);
	if (col==NULL) {
		glColor4f(0.25f,0.25f,1.0f,1.0f);
	}
	else {
		glColor4f(col->r,col->g,col->b,0.5f);
	}

		// circle

	if (!checkpoint) {
		pv=vertexes;
		
		for (n=0;n!=360;n+=10) {
			ky=dist;
			kz=0;
			rotate_2D_point_center(&ky,&kz,(float)n);
			*pv++=(float)pnt->x;
			*pv++=(float)(pnt->y+ky);
			*pv++=(float)(pnt->z+kz);
		}
		
		glVertexPointer(3,GL_FLOAT,0,vertexes);
		glDrawArrays(GL_LINE_LOOP,0,36);
	}

	pv=vertexes;
	
	for (n=0;n!=360;n+=10) {
		kx=dist;
		kz=0;
		rotate_2D_point_center(&kx,&kz,(float)n);
		*pv++=(float)(pnt->x+kx);
		*pv++=(float)pnt->y;
		*pv++=(float)(pnt->z+kz);
	}
	
	glVertexPointer(3,GL_FLOAT,0,vertexes);
	glDrawArrays(GL_LINE_LOOP,0,36);

	if (!checkpoint) {
		pv=vertexes;
		
		for (n=0;n!=360;n+=10) {
			kx=dist;
			ky=0;
			rotate_2D_point_center(&kx,&ky,(float)n);
			*pv++=(float)(pnt->x+kx);
			*pv++=(float)(pnt->y+ky);
			*pv++=(float)pnt->z;
		}

		glVertexPointer(3,GL_FLOAT,0,vertexes);
		glDrawArrays(GL_LINE_LOOP,0,36);
	}

		// checkpoint view line

	if (checkpoint) {
		pv=vertexes;

		*pv++=(float)pnt->x;
		*pv++=(float)pnt->y;
		*pv++=(float)pnt->z;

		kx=0;
		kz=dist;
		rotate_2D_point_center(&kx,&kz,ang->y);
		*pv++=(float)(pnt->x-kx);
		*pv++=(float)pnt->y;
		*pv++=(float)(pnt->z-kz);

		glVertexPointer(3,GL_FLOAT,0,vertexes);
		glDrawArrays(GL_LINES,0,2);
	}

	glLineWidth(1.0f);
	glColor4f(1.0f,1.0f,1.0f,1.0f);
}

/* =======================================================

      Mesh & Poly Culling and Clipping
      
======================================================= */

bool view_cull_poly(editor_view_type *view,map_mesh_type *mesh,map_mesh_poly_type *poly,int mn,int mk)
{
	d3vct		face_vct;
	
	if (!view->cull) return(FALSE);
	if (mesh->flag.never_cull) return(FALSE);
	if (poly->ptsz==0) return(FALSE);
	if (poly->flag.never_cull) return(FALSE);
	
		// zero normals are always culled
		
	if ((poly->tangent_space.normal.x==0.0f) && (poly->tangent_space.normal.y==0.0f) && (poly->tangent_space.normal.z==0.0f)) return(TRUE);
	
		// is normal facing away?
		
	vector_create(&face_vct,poly->box.mid.x,poly->box.mid.y,poly->box.mid.z,view->pnt.x,view->pnt.y,view->pnt.z);
	return(vector_dot_product(&poly->tangent_space.normal,&face_vct)>map.optimize.cull_angle);
}

bool view_clip_poly(editor_view_type *view,map_mesh_poly_type *poly)
{
	int			dist;

	if (!view->clip) return(FALSE);
	
		// if top or bottom, only clip Y
		
	if ((view->ang.x<=270.0f) && (view->ang.x>=45.0f)) {
		dist=abs(view->pnt.y-poly->box.mid.y);
	}
	else {
		dist=distance_2D_get(view->pnt.x,view->pnt.z,poly->box.mid.x,poly->box.mid.z);
	}
	
		// get distance
		
	return(dist<(pref.clip_distance*view_snap_clip_size_factor));
}

bool view_clip_liquid(editor_view_type *view,map_liquid_type *liq)
{
	int			x,z,dist;

	if (!view->clip) return(FALSE);
	
		// if top or bottom, only clip Y
		
	if ((view->ang.x<=270.0f) && (view->ang.x>=45.0f)) {
		dist=abs(view->pnt.y-liq->y);
	}
	else {
		x=(liq->lft+liq->rgt)>>1;
		z=(liq->top+liq->bot)>>1;
		dist=distance_2D_get(view->pnt.x,view->pnt.z,x,z);
	}
	
		// get distance
		
	return(dist<(pref.clip_distance*view_snap_clip_size_factor));
}

bool view_clip_point(editor_view_type *view,d3pnt *pnt)
{
	int			dist;

	if (!view->clip) return(FALSE);
	
		// if top or bottom, only clip Y
		
	if ((view->ang.x<=270.0f) && (view->ang.x>=45.0f)) {
		dist=abs(view->pnt.y-pnt->y);
	}
	else {
		dist=distance_2D_get(view->pnt.x,view->pnt.z,pnt->x,pnt->z);
	}
	
		// get distance
		
	return(dist<(pref.clip_distance*view_snap_clip_size_factor));
}

/* =======================================================

      View Mesh Draw List
      
======================================================= */

void view_draw_create_mesh_sort_list(editor_view_type *view)
{
	int					n,k,sz,idx,
						dist;
	d3pnt				min,max;
	map_mesh_type		*mesh;

		// sort it

	view_mesh_sort_count=0;

	for (n=0;n!=map.mesh.nmesh;n++) {

			// clip meshes to view
			
		map_mesh_calculate_extent(&map,n,&min,&max);
		if (!view_cull_boundbox_in_frustum(&min,&max)) continue;
			
			// get sorting distance

		mesh=&map.mesh.meshes[n];
		dist=map_mesh_calculate_distance(mesh,&view->pnt);

			// add to list, near ones first

		idx=-1;

		for (k=0;k!=view_mesh_sort_count;k++) {
			if (view_mesh_sort_list[k].dist>dist) {
				idx=k;
				break;
			}
		}

			// insert at end of list
			
		if (idx==-1) {
			view_mesh_sort_list[view_mesh_sort_count].idx=n;
			view_mesh_sort_list[view_mesh_sort_count].dist=dist;
		}
		
			// insert in list
		
		else {
			sz=sizeof(view_mesh_sort_list_type)*(view_mesh_sort_count-idx);
			memmove(&view_mesh_sort_list[idx+1],&view_mesh_sort_list[idx],sz);
			
			view_mesh_sort_list[idx].idx=n;
			view_mesh_sort_list[idx].dist=dist;
		}

			// add up list

		view_mesh_sort_count++;
		if (view_mesh_sort_count==view_mesh_sort_max_mesh) break;
	}
}

/* =======================================================

      Walk View Mesh Drawing
      
======================================================= */

void view_draw_meshes_texture(editor_view_type *view,bool opaque)
{
	int						n,k,mesh_idx;
	GLuint					old_gl_id;
	bool					culled;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;
	texture_type			*texture;
							
		// no depth buffer for transparent segments

	glEnable(GL_TEXTURE_2D);

	if (opaque) {
		glDisable(GL_BLEND);
	}
	else {
		glEnable(GL_BLEND);
	}
	
	glEnable(GL_DEPTH_TEST);
	if (opaque) {
		glDepthMask(GL_TRUE);
	}
	else {
		glDepthMask(GL_FALSE);
	}
	
	glColor4f(1.0f,1.0f,1.0f,1.0f);
	
	old_gl_id=-1;

		// draw meshes
		
	for (n=0;n!=view_mesh_sort_count;n++) {

		mesh_idx=view_mesh_sort_list[n].idx;
		mesh=&map.mesh.meshes[mesh_idx];
	
			// skip any meshes that don't have
			// light maps if on light maps
			
		if ((view->uv_layer==uv_layer_light_map) && (mesh->flag.no_light_map)) continue;

			// mesh VBO

		glBindBuffer(GL_ARRAY_BUFFER,mesh->vbo.vertex);
		glVertexPointer(3,GL_FLOAT,((3+2+2)*sizeof(float)),(GLvoid*)0);

		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		if (view->uv_layer==uv_layer_normal) {
			glTexCoordPointer(2,GL_FLOAT,((3+2+2)*sizeof(float)),(GLvoid*)(3*sizeof(float)));
		}
		else {
			glTexCoordPointer(2,GL_FLOAT,((3+2+2)*sizeof(float)),(GLvoid*)((3+2)*sizeof(float)));
		}

			// draw polys
	
		for (k=0;k!=mesh->npoly;k++) {
		
			poly=&mesh->polys[k];
			
				// opaque or transparent flag
				
			texture=&map.textures[poly->txt_idx];
			if (opaque!=texture->frames[0].bitmap.opaque) continue;
			
				// need to switch to light map?
				
			if (view->uv_layer==uv_layer_light_map) {
				if (poly->lmap_txt_idx==-1) continue;
				texture=&map.textures[poly->lmap_txt_idx];
			}
			
				// check for clipping
				
			if (view_clip_poly(view,poly)) continue;
			
				// culling
			
			culled=view_cull_poly(view,mesh,poly,mesh_idx,k);
		
				// setup texture
				
			if (!culled) {
				if (texture->frames[0].bitmap.gl_id!=old_gl_id) {
					old_gl_id=texture->frames[0].bitmap.gl_id;
					glBindTexture(GL_TEXTURE_2D,old_gl_id);
				}

				if ((poly->flag.obscuring) && (view->cull)) glColor4f(0.0f,1.0f,0.0f,1.0f);		// obscured polygons are tinted in green
			}
			else {
				glDisable(GL_TEXTURE_2D);
				glColor4f(0.9f,0.9f,0.9f,1.0f);
			}
		
				// draw polygon

			glDrawArrays(GL_POLYGON,poly->draw.vertex_offset,poly->ptsz);

				// if culled, turn back on texture
				
			if (culled) {
				glEnable(GL_TEXTURE_2D);
				glColor4f(1.0f,1.0f,1.0f,1.0f);
			}
			else {
				if ((poly->flag.obscuring) && (view->cull)) glColor4f(1.0f,1.0f,1.0f,1.0f);
			}
		}
	}

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	
	if (opaque) glEnable(GL_BLEND);
	if (!opaque) glDepthMask(GL_TRUE);

	glDisable(GL_TEXTURE_2D);

		// unbind any VBO

	glBindBuffer(GL_ARRAY_BUFFER,0);
}

void view_draw_meshes_line(editor_view_type *view,bool opaque)
{
	int					n,k,mesh_idx;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	texture_type		*texture;

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);

	glDepthMask(GL_FALSE);
	
	glColor4f(pref.col.mesh_line.r,pref.col.mesh_line.g,pref.col.mesh_line.b,1.0f);

		// draw portal mesh lines

	for (n=0;n!=view_mesh_sort_count;n++) {

		mesh_idx=view_mesh_sort_list[n].idx;
		mesh=&map.mesh.meshes[mesh_idx];

			// mesh VBO

		glBindBuffer(GL_ARRAY_BUFFER,mesh->vbo.vertex);
		glVertexPointer(3,GL_FLOAT,((3+2+2)*sizeof(float)),(GLvoid*)0);

			// draw polys
	
		for (k=0;k!=mesh->npoly;k++) {

			poly=&mesh->polys[k];
			
				// opaque or transparent flag

			texture=&map.textures[poly->txt_idx];
			if (opaque!=texture->frames[0].bitmap.opaque) continue;
			
				// check for clipping
				
			if (view_clip_poly(view,poly)) continue;

				// draw mesh line

			glDrawArrays(GL_LINE_LOOP,poly->draw.vertex_offset,poly->ptsz);
		}
	}

	glDepthMask(GL_TRUE);

		// unbind any VBO

	glBindBuffer(GL_ARRAY_BUFFER,0);
}

/* =======================================================

      Walk View Liquid Drawing
      
======================================================= */

void view_draw_liquids(editor_view_type *view,bool opaque)
{
	int					n,nliquid,y,y2,lx,rx,tz,bz;
	float				vertexes[8*3],uvs[8*2];
	float				*pv,*pt;
	GLuint				old_gl_id;
	texture_type		*texture;
	map_liquid_type		*liquid;
	map_liquid_uv_type	*uv;
	
	if (!state.map.show_liquid) return;
	
		// no depth buffer for transparent segments
		
	if (opaque) {
		glDisable(GL_BLEND);
	}
	else {
		glEnable(GL_BLEND);
	}
	
	glEnable(GL_DEPTH_TEST);
	if (opaque) {
		glDepthMask(GL_TRUE);
	}
	else {
		glDepthMask(GL_FALSE);
	}
	
	old_gl_id=-1;

		// run through the liquids
		
	nliquid=map.liquid.nliquid;
	
	for (n=0;n!=nliquid;n++) {
		liquid=&map.liquid.liquids[n];
		
				// clipping
				
		if (view_clip_liquid(view,liquid)) continue;

			// no light map?
				
		if ((view->uv_layer==uv_layer_light_map) && (liquid->lmap_txt_idx==-1)) continue;
			
			// get texture.  If in second UV, we use light map
			// texture for display if it exists
			
		if (view->uv_layer==uv_layer_normal) {
			texture=&map.textures[liquid->txt_idx];
			uv=&liquid->main_uv;
		}
		else {
			texture=&map.textures[liquid->lmap_txt_idx];
			uv=&liquid->lmap_uv;
		}
	
		if (opaque!=texture->frames[0].bitmap.opaque) continue;
		
		if (texture->frames[0].bitmap.gl_id!=old_gl_id) {
			old_gl_id=texture->frames[0].bitmap.gl_id;
			glBindTexture(GL_TEXTURE_2D,old_gl_id);
		}
		
			// dimensions
			
		y=liquid->y;
		y2=liquid->y+liquid->depth;
		
		lx=liquid->lft;
		rx=liquid->rgt;
		tz=liquid->top;
		bz=liquid->bot;

		glColor4f(1.0f,1.0f,1.0f,1.0f);
		
		pv=vertexes;
		pt=uvs;
		
			// bottom
			
		*pv++=(float)lx;
		*pv++=(float)y2;
		*pv++=(float)tz;
		*pt++=uv->offset.x;
		*pt++=uv->offset.y;

		*pv++=(float)rx;
		*pv++=(float)y2;
		*pv++=(float)tz;
		*pt++=(uv->offset.x+uv->size.x);
		*pt++=uv->offset.y;

		*pv++=(float)rx;
		*pv++=(float)y2;
		*pv++=(float)bz;
		*pt++=(uv->offset.x+uv->size.x);
		*pt++=(uv->offset.y+uv->size.y);

		*pv++=(float)lx;
		*pv++=(float)y2;
		*pv++=(float)bz;
		*pt++=uv->offset.x;
		*pt++=(uv->offset.y+uv->size.y);

			// top
			
		*pv++=(float)lx;
		*pv++=(float)y;
		*pv++=(float)tz;
		*pt++=uv->offset.x;
		*pt++=uv->offset.y;

		*pv++=(float)rx;
		*pv++=(float)y;
		*pv++=(float)tz;
		*pt++=(uv->offset.x+uv->size.x);
		*pt++=uv->offset.y;

		*pv++=(float)rx;
		*pv++=(float)y;
		*pv++=(float)bz;
		*pt++=(uv->offset.x+uv->size.x);
		*pt++=(uv->offset.y+uv->size.y);

		*pv++=(float)lx;
		*pv++=(float)y;
		*pv++=(float)bz;
		*pt++=uv->offset.x;
		*pt++=(uv->offset.y+uv->size.y);

		glEnable(GL_TEXTURE_2D);
		glColor4f(1.0f,1.0f,1.0f,1.0f);

		glVertexPointer(3,GL_FLOAT,0,vertexes);

		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2,GL_FLOAT,0,uvs);

		glDrawArrays(GL_QUADS,0,8);

		glDisableClientState(GL_TEXTURE_COORD_ARRAY);

		glDisable(GL_TEXTURE_2D);
		
			// depth lines
	
		glColor4f(pref.col.mesh_line.r,pref.col.mesh_line.g,pref.col.mesh_line.b,1.0f);

		pv=vertexes;
		
		*pv++=(float)liquid->lft;
		*pv++=(float)y;
		*pv++=(float)liquid->top;

		*pv++=(float)liquid->lft;
		*pv++=(float)y2;
		*pv++=(float)liquid->top;

		*pv++=(float)liquid->rgt;
		*pv++=(float)y;
		*pv++=(float)liquid->top;

		*pv++=(float)liquid->rgt;
		*pv++=(float)y2;
		*pv++=(float)liquid->top;

		*pv++=(float)liquid->lft;
		*pv++=(float)y;
		*pv++=(float)liquid->bot;

		*pv++=(float)liquid->lft;
		*pv++=(float)y2;
		*pv++=(float)liquid->bot;

		*pv++=(float)liquid->rgt;
		*pv++=(float)y;
		*pv++=(float)liquid->bot;

		*pv++=(float)liquid->rgt;
		*pv++=(float)y2;
		*pv++=(float)liquid->bot;

		glVertexPointer(3,GL_FLOAT,0,vertexes);
		glDrawArrays(GL_LINES,0,8);
	}
	
	if (opaque) glEnable(GL_BLEND);
	if (!opaque) glDepthMask(GL_TRUE);
}

/* =======================================================

      Walk View Normal Drawing
      
======================================================= */

void view_draw_meshes_normals(editor_view_type *view)
{
	int					n,k,dist;
	float				p_sz,vertexes[2*3];
	d3vct				binormal;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

	glDisable(GL_TEXTURE_2D);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glLineWidth(2.0f);
	
	mesh=map.mesh.meshes;
	
	for (n=0;n!=map.mesh.nmesh;n++) {
		
			// draw polys
	
		for (k=0;k!=mesh->npoly;k++) {
		
			poly=&mesh->polys[k];
			
			if (pref.show_tangent_binormal) {
			
					// draw the tangent

				vertexes[0]=(float)poly->box.mid.x;
				vertexes[1]=(float)poly->box.mid.y;
				vertexes[2]=(float)poly->box.mid.z;
				vertexes[3]=vertexes[0]+(poly->tangent_space.tangent.x*normal_vector_scale);
				vertexes[4]=vertexes[1]+(poly->tangent_space.tangent.y*normal_vector_scale);
				vertexes[5]=vertexes[2]+(poly->tangent_space.tangent.z*normal_vector_scale);

				glVertexPointer(3,GL_FLOAT,0,vertexes);

				glColor4f(1.0f,0.0f,0.0f,1.0f);
				glDrawArrays(GL_LINES,0,2);
				
					// draw the binormal

				vector_cross_product(&binormal,&poly->tangent_space.tangent,&poly->tangent_space.normal);

				vertexes[0]=(float)poly->box.mid.x;
				vertexes[1]=(float)poly->box.mid.y;
				vertexes[2]=(float)poly->box.mid.z;
				vertexes[3]=vertexes[0]+(binormal.x*normal_vector_scale);
				vertexes[4]=vertexes[1]+(binormal.y*normal_vector_scale);
				vertexes[5]=vertexes[2]+(binormal.z*normal_vector_scale);

				glVertexPointer(3,GL_FLOAT,0,vertexes);

				glColor4f(0.0f,0.0f,1.0f,1.0f);
				glDrawArrays(GL_LINES,0,2);
			}

				// draw normal

			vertexes[0]=(float)poly->box.mid.x;
			vertexes[1]=(float)poly->box.mid.y;
			vertexes[2]=(float)poly->box.mid.z;
			vertexes[3]=vertexes[0]+(poly->tangent_space.normal.x*normal_vector_scale);
			vertexes[4]=vertexes[1]+(poly->tangent_space.normal.y*normal_vector_scale);
			vertexes[5]=vertexes[2]+(poly->tangent_space.normal.z*normal_vector_scale);

			glVertexPointer(3,GL_FLOAT,0,vertexes);

			glColor4f(1.0f,0.0f,1.0f,1.0f);
			glDrawArrays(GL_LINES,0,2);

				// draw the center

			dist=distance_get(view->pnt.x,view->pnt.y,view->pnt.z,poly->box.mid.x,poly->box.mid.y,poly->box.mid.z);

			p_sz=10.0f-(((float)dist)*0.0001f);
			if (p_sz>2.0f) {
			
				glPointSize(p_sz);
				
				vertexes[0]=(float)poly->box.mid.x;
				vertexes[1]=(float)poly->box.mid.y;
				vertexes[2]=(float)poly->box.mid.z;

				glVertexPointer(3,GL_FLOAT,0,vertexes);

				glColor4f(0.6f,0.0f,0.6f,1.0f);
				glDrawArrays(GL_POINTS,0,1);
			}
		}
	
		mesh++;
	}
	
	glLineWidth(1.0f);
	glPointSize(1.0f);
}

/* =======================================================

      Walk View Movements
      
======================================================= */

void view_draw_movements(editor_view_type *view)
{
	int					n,k,cnt;
	d3pnt				group_pnt,pnt;
	map_mesh_type		*mesh;
	movement_type		*movement;
	movement_move_type	*move;
	
	if (!state.map.show_movements) return;

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	
	glLineWidth(2.0f);
	glPointSize(10.0f);
	
	glColor4f(0.0f,0.0f,1.0f,1.0f);

		// draw movements
	
	movement=map.movement.movements;

	for (n=0;n!=map.movement.nmovement;n++) {
	
			// get movement group start
			
		group_pnt.x=group_pnt.y=group_pnt.z=0;
		
		cnt=0;
		mesh=map.mesh.meshes;
			
		for (k=0;k!=map.mesh.nmesh;k++) {
			if (mesh->group_idx==movement->group_idx) {
				group_pnt.x+=mesh->box.mid.x;
				group_pnt.y+=mesh->box.mid.y;
				group_pnt.z+=mesh->box.mid.z;
				cnt++;
			}
			mesh++;
		}
		
		if (cnt==0) continue;
		
		group_pnt.x/=cnt;
		group_pnt.y/=cnt;
		group_pnt.z/=cnt;
		
			// the lines

		move=movement->moves;
		
		memmove(&pnt,&group_pnt,sizeof(d3pnt));
			
		glBegin(GL_LINES);
		
		for (k=0;k!=movement->nmove;k++) {
			glVertex3i(pnt.x,pnt.y,pnt.z);
			pnt.x+=move->mov.x;
			pnt.y+=move->mov.y;
			pnt.z+=move->mov.z;
			glVertex3i(pnt.x,pnt.y,pnt.z);
			move++;
		}
				
		glEnd();
		
			// the points

		move=movement->moves;
		
		memmove(&pnt,&group_pnt,sizeof(d3pnt));
			
		glBegin(GL_POINTS);
		
		glVertex3i(pnt.x,pnt.y,pnt.z);
		
		for (k=0;k!=movement->nmove;k++) {
			pnt.x+=move->mov.x;
			pnt.y+=move->mov.y;
			pnt.z+=move->mov.z;
			glVertex3i(pnt.x,pnt.y,pnt.z);
			move++;
		}
				
		glEnd();
		
		movement++;
	}
	
	glLineWidth(1.0f);
	glColor4f(0.0f,0.0f,0.0f,1.0f);
}

/* =======================================================

      Walk View Nodes, Scenery, etc Drawing
      
======================================================= */

void view_draw_nodes(editor_view_type *view)
{
	int			n,k;
	float		fx,fy,fz,vertexes[2*3];
	node_type	*node,*lnode;
	matrix_type	mat;

	if (!state.map.show_node) return;
	
		// angles
		
	glLineWidth(3.0f);
	glColor4f(1.0f,0.7f,0.0f,1.0f);
	
	for (n=0;n!=map.nnode;n++) {
		node=&map.nodes[n];
		
		if (view_clip_point(view,&node->pnt)) continue;
	
		fx=0.0f;
		fy=0.0f;
		fz=-(float)1000.0f;

		matrix_rotate_x(&mat,node->ang.x);
		matrix_vertex_multiply(&mat,&fx,&fy,&fz);
		
		matrix_rotate_y(&mat,node->ang.y);
		matrix_vertex_multiply(&mat,&fx,&fy,&fz);

		matrix_rotate_z(&mat,node->ang.z);
		matrix_vertex_multiply(&mat,&fx,&fy,&fz);
			
		vertexes[0]=(float)node->pnt.x;
		vertexes[1]=(float)(node->pnt.y-300);
		vertexes[2]=(float)node->pnt.z;
		vertexes[3]=(float)(node->pnt.x+(int)fx);
		vertexes[4]=(float)((node->pnt.y-300)+(int)fy);
		vertexes[5]=(float)(node->pnt.z+(int)fz);
		
		glVertexPointer(3,GL_FLOAT,0,vertexes);
		glDrawArrays(GL_LINES,0,2);
	}
	
		// connections
		
	glColor4f(0.0f,1.0f,0.0f,1.0f);
	
	for (n=0;n!=map.nnode;n++) {
		node=&map.nodes[n];
		
		if (view_clip_point(view,&node->pnt)) continue;
			
		for (k=0;k!=max_node_link;k++) {
		
			if (node->link[k]!=-1) {
				lnode=&map.nodes[node->link[k]];

				vertexes[0]=(float)node->pnt.x;
				vertexes[1]=(float)(node->pnt.y-300);
				vertexes[2]=(float)node->pnt.z;

				vertexes[3]=(float)lnode->pnt.x;
				vertexes[4]=(float)(lnode->pnt.y-300);
				vertexes[5]=(float)lnode->pnt.z;
		
				glVertexPointer(3,GL_FLOAT,0,vertexes);
				glDrawArrays(GL_LINES,0,2);
			}
		}
	}

	glLineWidth(1.0f);
	glColor4f(1.0f,1.0f,1.0f,1.0f);
		
		// nodes
	
	for (n=0;n!=map.nnode;n++) {
		node=&map.nodes[n];
		
		if (view_clip_point(view,&node->pnt)) continue;
		
		if (node->name[0]==0x0) {
			view_draw_sprite(&node->pnt,NULL,node_bitmap.gl_id);
		}
		else {
			view_draw_sprite(&node->pnt,NULL,node_defined_bitmap.gl_id);
		}
	}
}

void view_draw_spots_scenery(editor_view_type *view)
{
	int					n;
	spot_type			*spot;
	map_scenery_type	*scenery;
	
    if (!state.map.show_object) return;
    
	for (n=0;n!=map.nspot;n++) {
		spot=&map.spots[n];
		
		if (view_clip_point(view,&spot->pnt)) continue;
	
		if (!view_model_draw(&spot->pnt,&spot->ang,spot->display_model,1.0f,NULL,0)) {
			view_draw_sprite(&spot->pnt,&spot->ang,spot_bitmap.gl_id);
		}

		if (spot->type==spot_type_checkpoint) {
			if (select_check(item_map_spot,n,-1)) view_draw_circle(&spot->pnt,&spot->ang,NULL,spot->checkpoint.radius,TRUE);
		}
	}		
    
	for (n=0;n!=map.nscenery;n++) {
		scenery=&map.sceneries[n];
		
		if (view_clip_point(view,&scenery->pnt)) continue;
	
		if (!view_model_draw(&scenery->pnt,&scenery->ang,scenery->model_name,scenery->resize,scenery->texture_frame,max_map_scenery_model_texture_frame)) {
			view_draw_sprite(&scenery->pnt,&scenery->ang,scenery_bitmap.gl_id);
		}
	}		
}

void view_draw_lights_sounds_particles(editor_view_type *view)
{
	int				n;
	
	if (!state.map.show_lightsoundparticle) return;
	
	for (n=0;n!=map.nlight;n++) {
		if (view_clip_point(view,&map.lights[n].pnt)) continue;
		view_draw_sprite(&map.lights[n].pnt,NULL,light_bitmap.gl_id);
		if (select_check(item_map_light,n,-1)) view_draw_circle(&map.lights[n].pnt,NULL,&map.lights[n].setting.col,map.lights[n].setting.intensity,FALSE);
	}
	
	for (n=0;n!=map.nsound;n++) {
		if (view_clip_point(view,&map.sounds[n].pnt)) continue;
		view_draw_sprite(&map.sounds[n].pnt,NULL,sound_bitmap.gl_id);
	}
	
	for (n=0;n!=map.nparticle;n++) {
		if (view_clip_point(view,&map.particles[n].pnt)) continue;
		view_draw_sprite(&map.particles[n].pnt,NULL,particle_bitmap.gl_id);
		if ((select_check(item_map_particle,n,-1)) && (map.particles[n].light_setting.on)) view_draw_circle(&map.particles[n].pnt,NULL,&map.particles[n].light_setting.col,map.particles[n].light_setting.intensity,FALSE);
	}
}

/* =======================================================

      Draw Walk View
      
======================================================= */

void map_view_draw_view(editor_view_type *view)
{
       // 3D view
        
	view_set_viewport(view,TRUE,TRUE);
	view_set_3D_projection(view,map.editor_setup.view_near_dist,map.editor_setup.view_far_dist,view_near_offset);
	
	view_cull_setup_frustum_clipping_planes();

		// setup item drawing

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

		// create the sort list

	view_draw_create_mesh_sort_list(view);

        // draw opaque meshes and liquids
        
	view_draw_meshes_texture(view,TRUE);
	view_draw_nodes(view);
	view_draw_spots_scenery(view);
	view_draw_lights_sounds_particles(view);
	view_draw_liquids(view,TRUE);
	
	view_draw_movements(view);
	
		// draw opaque mesh lines
		// push view forward to better z-buffer lines
		
	view_set_3D_projection(view,(map.editor_setup.view_near_dist+10),(map.editor_setup.view_far_dist-10),view_near_offset);
	view_draw_meshes_line(view,TRUE);

        // draw transparent meshes and liquids
        
	view_set_3D_projection(view,map.editor_setup.view_near_dist,map.editor_setup.view_far_dist,view_near_offset);

	view_draw_meshes_texture(view,FALSE);
	view_draw_liquids(view,FALSE);
 	
        // draw transparent mesh lines
		// push view forward to better z-buffer lines
        
	view_set_3D_projection(view,(map.editor_setup.view_near_dist+10),(map.editor_setup.view_far_dist-10),view_near_offset);
	view_draw_meshes_line(view,FALSE);
	
        // draw normals
		// push view forward to better z-buffer lines
      
	if (state.map.show_normals) {
		view_set_3D_projection(view,map.editor_setup.view_near_dist,map.editor_setup.view_far_dist,view_near_offset);
		view_draw_meshes_normals(view);
	}
		
		// draw selection
		
	map_view_draw_select(view);
}

