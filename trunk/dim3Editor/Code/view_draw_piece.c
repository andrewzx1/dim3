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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3editor.h"
#endif

#include "interface.h"
#include "view.h"

extern map_type				map;
extern editor_setup_type	setup;
extern editor_state_type	state;
extern bitmap_type			spot_bitmap,scenery_bitmap,node_bitmap,node_defined_bitmap,
							light_bitmap,sound_bitmap,particle_bitmap;
						
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
	d3pnt			v_pnts[8];
	
	view_get_sprite_vertexes(pnt,ang,v_pnts);
	
		// draw sprite
		
	glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,gl_id);
    
	glColor4f(1.0f,1.0f,1.0f,1.0f);
	
	glBegin(GL_QUADS);
	
	for (n=0;n!=24;n+=4) {
	
		idx=face_idx[n];
		glTexCoord2f(0,0);
		glVertex3i(v_pnts[idx].x,v_pnts[idx].y,v_pnts[idx].z);
		
		idx=face_idx[n+1];
		glTexCoord2f(1,0);
		glVertex3i(v_pnts[idx].x,v_pnts[idx].y,v_pnts[idx].z);
		
		idx=face_idx[n+2];
		glTexCoord2f(1,1);
		glVertex3i(v_pnts[idx].x,v_pnts[idx].y,v_pnts[idx].z);
		
		idx=face_idx[n+3];
		glTexCoord2f(0,1);
		glVertex3i(v_pnts[idx].x,v_pnts[idx].y,v_pnts[idx].z);
	}
	
	glEnd();
	
	glDisable(GL_TEXTURE_2D);
}

void view_draw_circle(d3pnt *pnt,d3col *col,int dist)
{
    int				n,kx,ky,kz;
	
	glLineWidth(4.0f);
	glColor4f(col->r,col->g,col->b,0.5f);
	
	glBegin(GL_LINE_LOOP);
	
	for (n=0;n!=360;n+=10) {
		ky=dist;
		kz=0;
		rotate_2D_point_center(&ky,&kz,(float)n);
		glVertex3i(pnt->x,(pnt->y+ky),(pnt->z+kz));
	}
	
	glEnd();
	
	glBegin(GL_LINE_LOOP);
	
	for (n=0;n!=360;n+=10) {
		kx=dist;
		kz=0;
		rotate_2D_point_center(&kx,&kz,(float)n);
		glVertex3i((pnt->x+kx),pnt->y,(pnt->z+kz));
	}
	
	glEnd();
	
	glBegin(GL_LINE_LOOP);
	
	for (n=0;n!=360;n+=10) {
		kx=dist;
		ky=0;
		rotate_2D_point_center(&kx,&ky,(float)n);
		glVertex3i((pnt->x+kx),(pnt->y+ky),pnt->z);
	}

	glEnd();
	
	glLineWidth(1.0f);
	glColor4f(1.0f,1.0f,1.0f,1.0f);
}

/* =======================================================

      Poly Culling and Clipping
      
======================================================= */

bool view_cull_poly(editor_view_type *view,map_mesh_type *mesh,map_mesh_poly_type *poly)
{
	int			n;
	d3pnt		center,camera_pnt;
	d3vct		face_vct;
	
	if (!view->cull) return(FALSE);
	if (poly->ptsz==0) return(FALSE);
	if (poly->never_cull) return(FALSE);
	
		// get center
		
	center.x=center.y=center.z=0;
	
	for (n=0;n!=poly->ptsz;n++) {
		center.x+=mesh->vertexes[poly->v[n]].x;
		center.y+=mesh->vertexes[poly->v[n]].y;
		center.z+=mesh->vertexes[poly->v[n]].z;
	}
	
	center.x/=poly->ptsz;
	center.y/=poly->ptsz;
	center.z/=poly->ptsz;
	
		// is normal facing away?
	
	view_get_position(&camera_pnt);
	
	vector_create(&face_vct,center.x,center.y,center.z,camera_pnt.x,camera_pnt.y,camera_pnt.z);
	return(vector_dot_product(&poly->tangent_space.normal,&face_vct)>0.0f);
}

bool view_clip_poly(editor_view_type *view,map_mesh_type *mesh,map_mesh_poly_type *poly)
{
	int			n,dist;
	d3pnt		center;

	if (!view->clip) return(FALSE);
	
		// get center
		
	center.x=center.y=center.z=0;
	
	for (n=0;n!=poly->ptsz;n++) {
		center.x+=mesh->vertexes[poly->v[n]].x;
		center.y+=mesh->vertexes[poly->v[n]].y;
		center.z+=mesh->vertexes[poly->v[n]].z;
	}
	
	center.x/=poly->ptsz;
	center.y/=poly->ptsz;
	center.z/=poly->ptsz;
	
		// if top or bottom, only clip Y
		
	if ((view->ang.x<=270.0f) && (view->ang.x>=45.0f)) {
		dist=abs(view->pnt.y-center.y);
	}
	else {
		dist=distance_2D_get(view->pnt.x,view->pnt.z,center.x,center.z);
	}
	
		// get distance
		
	return(dist<(map_enlarge*setup.clip_distance));
}

bool view_hidden_poly(editor_view_type *view,map_mesh_type *mesh,map_mesh_poly_type *poly)
{
	int			n,wid,high;
	bool		above_z,behind_z,lft,rgt,top,bot;
	d3pnt		pnt;
	d3rect		box;
	
		// check if points are behind z
		
	above_z=FALSE;
	behind_z=FALSE;
	
	for (n=0;n!=poly->ptsz;n++) {
		if (view_project_point_in_z(&mesh->vertexes[poly->v[n]])) {
			above_z=TRUE;
		}
		else {
			behind_z=TRUE;
		}
	}
	
	if (!above_z) return(TRUE);
	
		// if it's split on the z, just keep
		
	if ((above_z) && (behind_z)) return(FALSE);
	
		// are points grouped completely
		// off one side of the screen?

	view_get_pixel_box(view,&box);
	
	wid=box.rx-box.lx;
	high=box.by-box.ty;

	lft=rgt=top=bot=TRUE;

	for (n=0;n!=poly->ptsz;n++) {
		memmove(&pnt,&mesh->vertexes[poly->v[n]],sizeof(d3pnt));
		view_project_point(view,&pnt);
		lft=lft&&(pnt.x<box.lx);
		rgt=rgt&&(pnt.x>=box.rx);
		top=top&&(pnt.y<box.ty);
		bot=bot&&(pnt.y>=box.by);
	}

	return(lft||rgt||top||bot);
}

/* =======================================================

      Walk View Mesh Drawing
      
======================================================= */

void view_draw_meshes_texture(editor_view_type *view,bool opaque)
{
	int						n,k,t;
	unsigned long			old_gl_id;
	bool					culled;
	d3pnt					*pt;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*mesh_poly;
	map_mesh_poly_uv_type	*uv;
	texture_type			*texture;
							
		// no depth buffer for transparent segments
		
	glEnable(GL_TEXTURE_2D);

	if (opaque) {
		glDisable(GL_BLEND);
	}
	else {
		glEnable(GL_BLEND);
	}
	
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
	
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
		
	mesh=map.mesh.meshes;
	
	for (n=0;n!=map.mesh.nmesh;n++) {
	
			// skip any meshes that don't have
			// light maps if on light maps
			
		if ((view->uv_layer==uv_layer_light_map) && (mesh->flag.no_light_map)) {
			mesh++;
			continue;
		}
		
			// draw polys
	
		for (k=0;k!=mesh->npoly;k++) {
		
			mesh_poly=&mesh->polys[k];
			
				// clipping
				
			if (view_clip_poly(view,mesh,mesh_poly)) continue;
			
				// hidden
				
			if (view_hidden_poly(view,mesh,mesh_poly)) continue;
			
				// no light map?
				
			if ((view->uv_layer==uv_layer_light_map) && (mesh_poly->lmap_txt_idx==-1)) continue;
			
				// get texture.  If in second UV, we use light map
				// texture for display if it exists
				
			if (view->uv_layer==uv_layer_normal) {
				texture=&map.textures[mesh_poly->txt_idx];
				uv=&mesh_poly->main_uv;
			}
			else {
				texture=&map.textures[mesh_poly->lmap_txt_idx];
				uv=&mesh_poly->lmap_uv;
			}
		
				// opaque or transparent flag
				
			if (opaque) {
				if (texture->frames[0].bitmap.alpha_mode==alpha_mode_transparent) continue;
			}
			else {
				if (texture->frames[0].bitmap.alpha_mode!=alpha_mode_transparent) continue;
			}
			
				// culling
			
			culled=view_cull_poly(view,mesh,mesh_poly);
		
				// setup texture
				
			if (!culled) {
				if (texture->frames[0].bitmap.gl_id!=old_gl_id) {
					old_gl_id=texture->frames[0].bitmap.gl_id;
					glBindTexture(GL_TEXTURE_2D,old_gl_id);
				}
			}
			else {
				glDisable(GL_TEXTURE_2D);
				glColor4f(0.9f,0.9f,0.9f,1.0f);
			}
		
				// draw polygon
				
			glBegin(GL_POLYGON);
			
			for (t=0;t!=mesh_poly->ptsz;t++) {
				pt=&mesh->vertexes[mesh_poly->v[t]];
				glTexCoord2f(uv->x[t],uv->y[t]);
				glVertex3i(pt->x,pt->y,pt->z);
			}
			
			glEnd();
			
				// if culled, turn back on texture
				
			if (culled) {
				glEnable(GL_TEXTURE_2D);
				glColor4f(1.0f,1.0f,1.0f,1.0f);
			}
		}
	
		mesh++;
	}
	
	glDisable(GL_ALPHA_TEST);
	if (opaque) glEnable(GL_BLEND);
	if (!opaque) glDepthMask(GL_TRUE);

	glDisable(GL_TEXTURE_2D);
}

void view_draw_meshes_line(editor_view_type *view,bool opaque)
{
	int					n,k,t;
	d3pnt				*pt;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*mesh_poly;
	texture_type		*texture;
							
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_ALPHA_TEST);
	
	glColor4f(setup.col.mesh_line.r,setup.col.mesh_line.g,setup.col.mesh_line.b,1.0f);
	
		// draw portal mesh lines

	mesh=map.mesh.meshes;
	
	for (n=0;n!=map.mesh.nmesh;n++) {
		
			// draw polys
	
		for (k=0;k!=mesh->npoly;k++) {
		
			mesh_poly=&mesh->polys[k];
			texture=&map.textures[mesh_poly->txt_idx];
			
				// clipping
				
			if (view_clip_poly(view,mesh,mesh_poly)) continue;
			
				// hidden
				
			if (view_hidden_poly(view,mesh,mesh_poly)) continue;
			
				// opaque or transparent flag
		
			if (opaque) {
				if (texture->frames[0].bitmap.alpha_mode==alpha_mode_transparent) continue;
			}
			else {
				if (texture->frames[0].bitmap.alpha_mode!=alpha_mode_transparent) continue;
			}
			
			glBegin(GL_LINE_LOOP);
			
			for (t=0;t!=mesh_poly->ptsz;t++) {
				pt=&mesh->vertexes[mesh_poly->v[t]];
				glVertex3i(pt->x,pt->y,pt->z);
			}
			
			glEnd();
		}
	
		mesh++;
	}
}

/* =======================================================

      Walk View Liquid Drawing
      
======================================================= */

void view_draw_liquids(editor_view_type *view,bool opaque)
{
	int					n,nliquid,x,y,z,y2,lx,rx,tz,bz;
	unsigned long		old_gl_id;
	texture_type		*texture;
	map_liquid_type		*liquid;
	map_liquid_uv_type	*uv;
	
	if (!state.show_liquid) return;
	
		// no depth buffer for transparent segments
		
	glEnable(GL_TEXTURE_2D);

	if (opaque) {
		glDisable(GL_BLEND);
	}
	else {
		glEnable(GL_BLEND);
	}
	
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
	
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
	
	glEnable(GL_TEXTURE_2D);
	glColor4f(1.0f,1.0f,1.0f,1.0f);
	
	for (n=0;n!=nliquid;n++) {
		liquid=&map.liquid.liquids[n];
		
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
	
		if (opaque) {
			if (texture->frames[0].bitmap.alpha_mode==alpha_mode_transparent) continue;
		}
		else {
			if (texture->frames[0].bitmap.alpha_mode!=alpha_mode_transparent) continue;
		}
		
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
		
		glBegin(GL_QUADS);
		
			// bottom
			
		glTexCoord2f(uv->x_offset,uv->y_offset);
		glVertex3i(lx,y2,tz);
		glTexCoord2f((uv->x_offset+uv->x_size),uv->y_offset);
		glVertex3i(rx,y2,tz);
		glTexCoord2f((uv->x_offset+uv->x_size),(uv->y_offset+uv->y_size));
		glVertex3i(rx,y2,bz);
		glTexCoord2f(uv->x_offset,(uv->y_offset+uv->y_size));
		glVertex3i(lx,y2,bz);

			// top
			
		glTexCoord2f(uv->x_offset,uv->y_offset);
		glVertex3i(lx,y,tz);
		glTexCoord2f((uv->x_offset+uv->x_size),uv->y_offset);
		glVertex3i(rx,y,tz);
		glTexCoord2f((uv->x_offset+uv->x_size),(uv->y_offset+uv->y_size));
		glVertex3i(rx,y,bz);
		glTexCoord2f(uv->x_offset,(uv->y_offset+uv->y_size));
		glVertex3i(lx,y,bz);

		glEnd();
		
			// depth lines
	
		glColor4f(setup.col.mesh_line.r,setup.col.mesh_line.g,setup.col.mesh_line.b,1.0f);
		
		glBegin(GL_LINES);
		
		x=liquid->lft;
		z=liquid->top;
		glVertex3i(x,y,z);
		glVertex3i(x,y2,z);

		x=liquid->rgt;
		z=liquid->top;
		glVertex3i(x,y,z);
		glVertex3i(x,y2,z);

		x=liquid->lft;
		z=liquid->bot;
		glVertex3i(x,y,z);
		glVertex3i(x,y2,z);

		x=liquid->rgt;
		z=liquid->bot;
		glVertex3i(x,y,z);
		glVertex3i(x,y2,z);

		glEnd();
	}
	
	glDisable(GL_ALPHA_TEST);
	if (opaque) glEnable(GL_BLEND);
	if (!opaque) glDepthMask(GL_TRUE);
	
	glDisable(GL_TEXTURE_2D);
}

/* =======================================================

      Walk View Normal Drawing
      
======================================================= */

void view_draw_meshes_normals(editor_view_type *view_setup)
{
	int					n,k,t;
	d3pnt				*pt,cnt;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
							
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_ALPHA_TEST);
	
	glLineWidth(2.0f);
	
	mesh=map.mesh.meshes;
	
	for (n=0;n!=map.mesh.nmesh;n++) {
		
			// draw polys
	
		for (k=0;k!=mesh->npoly;k++) {
		
			poly=&mesh->polys[k];
			
				// get center
				
			
			cnt.x=cnt.y=cnt.z=0;
			
			for (t=0;t!=poly->ptsz;t++) {
				pt=&mesh->vertexes[poly->v[t]];
				cnt.x+=pt->x;
				cnt.y+=pt->y;
				cnt.z+=pt->z;
			}
			
			cnt.x/=poly->ptsz;
			cnt.y/=poly->ptsz;
			cnt.z/=poly->ptsz;
			
				// draw the tangent
			/*	
			glColor4f(1.0f,0.0f,0.0f,1.0f);
			
			glBegin(GL_LINES);
			glVertex3i(cnt.x,cnt.y,cnt.z);
			glVertex3i((cnt.x+(int)(poly->tangent_space.tangent.x*normal_vector_scale)),(cnt.y+(int)(poly->tangent_space.tangent.y*normal_vector_scale)),(cnt.z+(int)(poly->tangent_space.tangent.z*normal_vector_scale)));
			glEnd();
			
				// draw the binormal
				
			glColor4f(0.0f,0.0f,1.0f,1.0f);
			
			glBegin(GL_LINES);
			glVertex3i(cnt.x,cnt.y,cnt.z);
			glVertex3i((cnt.x+(int)(poly->tangent_space.binormal.x*normal_vector_scale)),(cnt.y+(int)(poly->tangent_space.binormal.y*normal_vector_scale)),(cnt.z+(int)(poly->tangent_space.binormal.z*normal_vector_scale)));
			glEnd();
			*/
				// draw normal
				
			glColor4f(1.0f,0.0f,1.0f,1.0f);
			
			glBegin(GL_LINES);
			glVertex3i(cnt.x,cnt.y,cnt.z);
			glVertex3i((cnt.x+(int)(poly->tangent_space.normal.x*normal_vector_scale)),(cnt.y+(int)(poly->tangent_space.normal.y*normal_vector_scale)),(cnt.z+(int)(poly->tangent_space.normal.z*normal_vector_scale)));
			glEnd();
		}
	
		mesh++;
	}
	
	glLineWidth(1.0f);
}

/* =======================================================

      Walk View Nodes, Scenery, etc Drawing
      
======================================================= */

void view_draw_nodes(editor_view_type *view_setup)
{
	int			n,k;
	float		fx,fy,fz;
	node_type	*node,*lnode;
	matrix_type	mat;

	if (!state.show_node) return;
	
		// angles
		
	glDisable(GL_ALPHA_TEST);
		
	glLineWidth(3.0f);
	glColor4f(1.0f,0.7f,0.0f,1.0f);
	
	glBegin(GL_LINES);
		
	for (n=0;n!=map.nnode;n++) {
		node=&map.nodes[n];
	
		fx=0.0f;
		fy=0.0f;
		fz=-(float)(map_enlarge*8);
	
		matrix_rotate_xyz(&mat,node->ang.x,node->ang.y,node->ang.z);
		matrix_vertex_multiply(&mat,&fx,&fy,&fz);
			
		glVertex3i(node->pnt.x,(node->pnt.y-(map_enlarge*2)),node->pnt.z);
		glVertex3i((node->pnt.x+(int)fx),((node->pnt.y-(map_enlarge*2))+(int)fy),(node->pnt.z+(int)fz));
	}
	
	glEnd();
	
		// connections
		
	glColor4f(0.0f,1.0f,0.0f,1.0f);
	
	glBegin(GL_LINES);
		
	for (n=0;n!=map.nnode;n++) {
		node=&map.nodes[n];
			
		for (k=0;k!=max_node_link;k++) {
		
			if (node->link[k]!=-1) {
				glVertex3i(node->pnt.x,(node->pnt.y-(map_enlarge*2)),node->pnt.z);
				lnode=&map.nodes[node->link[k]];
				glVertex3i(lnode->pnt.x,(lnode->pnt.y-(map_enlarge*2)),lnode->pnt.z);
			}
		}
	}
	
	glEnd();

	glLineWidth(1.0f);
	glColor4f(1.0f,1.0f,1.0f,1.0f);
		
		// nodes
	
	for (n=0;n!=map.nnode;n++) {
		node=&map.nodes[n];
		
		if (node->name[0]==0x0) {
			view_draw_sprite(&node->pnt,NULL,node_bitmap.gl_id);
		}
		else {
			view_draw_sprite(&node->pnt,NULL,node_defined_bitmap.gl_id);
		}
	}
}

void view_draw_spots_scenery(editor_view_type *view_setup)
{
	int					n;
	spot_type			*spot;
	map_scenery_type	*scenery;
	
    if (!state.show_object) return;
    
	for (n=0;n!=map.nspot;n++) {
		spot=&map.spots[n];
	
		if (!view_model_draw(&spot->pnt,&spot->ang,spot->display_model,1.0f,NULL,0)) {
			view_draw_sprite(&spot->pnt,&spot->ang,spot_bitmap.gl_id);
		}
	}		
    
	for (n=0;n!=map.nscenery;n++) {
		scenery=&map.sceneries[n];
	
		if (!view_model_draw(&scenery->pnt,&scenery->ang,scenery->model_name,scenery->resize,scenery->texture_frame,max_map_scenery_model_texture_frame)) {
			view_draw_sprite(&scenery->pnt,&scenery->ang,scenery_bitmap.gl_id);
		}
	}		
}

void view_draw_lights_sounds_particles(editor_view_type *view_setup)
{
	int				n;
	
	if (!state.show_lightsoundparticle) return;
	
	for (n=0;n!=map.nlight;n++) {
		view_draw_sprite(&map.lights[n].pnt,NULL,light_bitmap.gl_id);
		if (select_check(light_piece,n,-1)) view_draw_circle(&map.lights[n].pnt,&map.lights[n].col,map.lights[n].intensity);
	}
	
	for (n=0;n!=map.nsound;n++) {
		view_draw_sprite(&map.sounds[n].pnt,NULL,sound_bitmap.gl_id);
	}
	
	for (n=0;n!=map.nparticle;n++) {
		view_draw_sprite(&map.particles[n].pnt,NULL,particle_bitmap.gl_id);
	}
}

/* =======================================================

      Draw Walk View
      
======================================================= */

void view_draw_view(editor_view_type *view)
{
       // 3D view
        
	view_set_viewport(view,TRUE,TRUE);
	view_set_3D_projection(view,map.editor_setup.view_near_dist,map.editor_setup.view_far_dist,view_near_offset);

		// setup item drawing

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

        // draw opaque parts of portals in sight path
        
	view_draw_meshes_texture(view,TRUE);
	view_draw_nodes(view);
	view_draw_spots_scenery(view);
	view_draw_lights_sounds_particles(view);
	view_draw_liquids(view,TRUE);
	
		// draw opaque mesh lines
		// push view forward to better z-buffer lines
		
	view_set_3D_projection(view,(map.editor_setup.view_near_dist+10),(map.editor_setup.view_far_dist-10),view_near_offset);
	view_draw_meshes_line(view,TRUE);

        // draw transparent parts of portals in sight path
        
	view_set_3D_projection(view,map.editor_setup.view_near_dist,map.editor_setup.view_far_dist,view_near_offset);

	view_draw_meshes_texture(view,FALSE);
	view_draw_liquids(view,FALSE);
 	
        // draw transparent mesh lines
		// push view forward to better z-buffer lines
        
	view_set_3D_projection(view,(map.editor_setup.view_near_dist+10),(map.editor_setup.view_far_dist-10),view_near_offset);
	view_draw_meshes_line(view,TRUE);
	
        // draw normals mesh lines
		// push view forward to better z-buffer lines
      
	if (state.show_normals) {
		view_set_3D_projection(view,(map.editor_setup.view_near_dist+20),(map.editor_setup.view_far_dist-20),view_near_offset);
		view_draw_meshes_normals(view);
	}
		
		// draw selection
		
	view_draw_select(view);
}

