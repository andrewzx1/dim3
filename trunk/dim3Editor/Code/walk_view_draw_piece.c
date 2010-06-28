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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3editor.h"
#endif

#include "interface.h"
#include "common_view.h"
#include "walk_view.h"

extern int					txt_palette_high;

extern map_type				map;
extern setup_type			setup;
extern editor_state_type	state;
extern bitmap_type			spot_bitmap,scenery_bitmap,node_bitmap,node_defined_bitmap,
							light_bitmap,sound_bitmap,particle_bitmap;
						
extern bool obscure_mesh_view_bit_get(unsigned char *visibility_flag,int idx);

/* =======================================================

      Walk View Sprites Drawing
      
======================================================= */

void walk_view_draw_sprite(d3pnt *pnt,float ang_y,unsigned long gl_id)
{
    int			px[4],pz[4],wid,high;
	
    wid=map_enlarge*3;
    high=map_enlarge*4;
	
	px[0]=px[3]=pnt->x-wid;
	px[1]=px[2]=pnt->x+wid;
	pz[0]=pz[1]=pnt->z-wid;
	pz[2]=pz[3]=pnt->z+wid;
	
	rotate_2D_polygon(4,px,pz,pnt->x,pnt->z,ang_y);
    
	glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,gl_id);
    
	glColor4f(1,1,1,1);
	
	glBegin(GL_QUADS);

    glTexCoord2f(0,0);
	glVertex3i(px[0],(pnt->y-high),pz[0]);
    glTexCoord2f(1,0);
	glVertex3i(px[1],(pnt->y-high),pz[1]);
    glTexCoord2f(1,1);
	glVertex3i(px[1],pnt->y,pz[1]);
    glTexCoord2f(0,1);
	glVertex3i(px[0],pnt->y,pz[0]);

    glTexCoord2f(0,0);
	glVertex3i(px[3],(pnt->y-high),pz[3]);
    glTexCoord2f(1,0);
	glVertex3i(px[2],(pnt->y-high),pz[2]);
    glTexCoord2f(1,1);
	glVertex3i(px[2],pnt->y,pz[2]);
    glTexCoord2f(0,1);
	glVertex3i(px[3],pnt->y,pz[3]);

    glTexCoord2f(0,0);
	glVertex3i(px[0],(pnt->y-high),pz[0]);
    glTexCoord2f(1,0);
	glVertex3i(px[3],(pnt->y-high),pz[3]);
    glTexCoord2f(1,1);
	glVertex3i(px[3],pnt->y,pz[3]);
    glTexCoord2f(0,1);
	glVertex3i(px[0],pnt->y,pz[0]);

    glTexCoord2f(0,0);
	glVertex3i(px[1],(pnt->y-high),pz[1]);
    glTexCoord2f(1,0);
	glVertex3i(px[2],(pnt->y-high),pz[2]);
    glTexCoord2f(1,1);
	glVertex3i(px[2],pnt->y,pz[2]);
    glTexCoord2f(0,1);
	glVertex3i(px[1],pnt->y,pz[1]);

    glTexCoord2f(0,0);
	glVertex3i(px[0],(pnt->y-high),pz[0]);
    glTexCoord2f(1,0);
	glVertex3i(px[1],(pnt->y-high),pz[1]);
    glTexCoord2f(1,1);
	glVertex3i(px[2],(pnt->y-high),pz[2]);
    glTexCoord2f(0,1);
	glVertex3i(px[3],(pnt->y-high),pz[3]);

    glTexCoord2f(0,0);
	glVertex3i(px[0],pnt->y,pz[0]);
    glTexCoord2f(1,0);
	glVertex3i(px[1],pnt->y,pz[1]);
    glTexCoord2f(1,1);
	glVertex3i(px[2],pnt->y,pz[2]);
    glTexCoord2f(0,1);
	glVertex3i(px[3],pnt->y,pz[3]);
	
	glEnd();
	
	glDisable(GL_TEXTURE_2D);
}

void walk_view_draw_circle(d3pnt *pnt,d3col *col,int dist)
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

      Poly Culling
      
======================================================= */

bool walk_view_draw_cull_poly(map_mesh_type *mesh,map_mesh_poly_type *poly)
{
	int			n;
	d3pnt		center,camera_pnt;
	d3vct		face_vct;
	
	if (!state.cull) return(FALSE);
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
	
	walk_view_get_position(&camera_pnt);
	
	vector_create(&face_vct,center.x,center.y,center.z,camera_pnt.x,camera_pnt.y,camera_pnt.z);
	return(vector_dot_product(&poly->tangent_space.normal,&face_vct)>0.0f);
}

/* =======================================================

      Walk View Mesh Drawing
      
======================================================= */

void walk_view_draw_meshes_texture(editor_view_type *view_setup,bool opaque)
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
			
		if ((state.uv_layer==uv_layer_light_map) && (mesh->flag.no_light_map)) {
			mesh++;
			continue;
		}
		
			// draw polys
	
		for (k=0;k!=mesh->npoly;k++) {
		
			mesh_poly=&mesh->polys[k];
			
				// no light map?
				
			if ((state.uv_layer==uv_layer_light_map) && (mesh_poly->lmap_txt_idx==-1)) continue;
			
				// get texture.  If in second UV, we use light map
				// texture for display if it exists
				
			if (state.uv_layer==uv_layer_normal) {
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
			
			culled=walk_view_draw_cull_poly(mesh,mesh_poly);
		
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
	
	if (!opaque) glDepthMask(GL_TRUE);
	
	glDisable(GL_TEXTURE_2D);
}

void walk_view_draw_meshes_line(editor_view_type *view_setup,bool opaque)
{
	int					n,k,t;
	d3pnt				*pt;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*mesh_poly;
	texture_type		*texture;
							
	glDisable(GL_TEXTURE_2D);

	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	
	glColor4f(setup.col.mesh_line.r,setup.col.mesh_line.g,setup.col.mesh_line.b,1.0f);
	
		// draw portal mesh lines

	mesh=map.mesh.meshes;
	
	for (n=0;n!=map.mesh.nmesh;n++) {
		
			// draw polys
	
		for (k=0;k!=mesh->npoly;k++) {
		
			mesh_poly=&mesh->polys[k];
			texture=&map.textures[mesh_poly->txt_idx];
			
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

void walk_view_draw_liquids(editor_view_type *view_setup,bool opaque)
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
				
		if ((state.uv_layer==uv_layer_light_map) && (liquid->lmap_txt_idx==-1)) continue;
			
			// get texture.  If in second UV, we use light map
			// texture for display if it exists
			
		if (state.uv_layer==uv_layer_normal) {
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
	
	if (!opaque) glDepthMask(GL_TRUE);
	
	glDisable(GL_ALPHA_TEST);
	if (opaque) glEnable(GL_BLEND);
	
	glDisable(GL_TEXTURE_2D);
}

/* =======================================================

      Walk View Normal Drawing
      
======================================================= */

void walk_view_draw_meshes_normals(editor_view_type *view_setup)
{
	int					n,k,t;
	d3pnt				*pt,cnt;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
							
	glDisable(GL_TEXTURE_2D);

	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	
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

void walk_view_draw_nodes(editor_view_type *view_setup)
{
	int			n,k;
	float		fx,fy,fz;
	node_type	*node,*lnode;
	matrix_type	mat;

	if (!state.show_node) return;
	
		// angles
		
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
		
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
			walk_view_draw_sprite(&node->pnt,0.0f,node_bitmap.gl_id);
		}
		else {
			walk_view_draw_sprite(&node->pnt,0.0f,node_defined_bitmap.gl_id);
		}
	}
}

void walk_view_draw_spots_scenery(editor_view_type *view_setup)
{
	int					n;
	spot_type			*spot;
	map_scenery_type	*scenery;
	
    if (!state.show_object) return;
    
	for (n=0;n!=map.nspot;n++) {
		spot=&map.spots[n];
	
		if (!walk_view_model_draw(&spot->pnt,&spot->ang,spot->display_model,NULL,0)) {
			walk_view_draw_sprite(&spot->pnt,spot->ang.y,spot_bitmap.gl_id);
		}
	}		
    
	for (n=0;n!=map.nscenery;n++) {
		scenery=&map.sceneries[n];
	
		if (!walk_view_model_draw(&scenery->pnt,&scenery->ang,scenery->model_name,scenery->texture_frame,max_map_scenery_model_texture_frame)) {
			walk_view_draw_sprite(&scenery->pnt,scenery->ang.y,scenery_bitmap.gl_id);
		}
	}		
}

void walk_view_draw_lights_sounds_particles(editor_view_type *view_setup)
{
	int				n;
	
	if (!state.show_lightsoundparticle) return;
	
	for (n=0;n!=map.nlight;n++) {
		walk_view_draw_sprite(&map.lights[n].pnt,0.0f,light_bitmap.gl_id);
		if (select_check(light_piece,n,-1)) walk_view_draw_circle(&map.lights[n].pnt,&map.lights[n].col,map.lights[n].intensity);
	}
	
	for (n=0;n!=map.nsound;n++) {
		walk_view_draw_sprite(&map.sounds[n].pnt,0.0f,sound_bitmap.gl_id);
	}
	
	for (n=0;n!=map.nparticle;n++) {
		walk_view_draw_sprite(&map.particles[n].pnt,0.0f,particle_bitmap.gl_id);
	}
}

/* =======================================================

      Walk View Drawing Setup
      
======================================================= */

void walk_view_gl_setup(editor_view_type *view_setup)
{
	walk_view_set_viewport(view_setup,TRUE,TRUE);
	walk_view_set_3D_projection(view_setup,map.settings.editor.view_near_dist,map.settings.editor.view_far_dist,walk_view_near_offset);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
}

/* =======================================================

      Draw Walk View
      
======================================================= */

void walk_view_draw_view(editor_view_type *view_setup)
{
       // 3D view
        
	walk_view_set_viewport(view_setup,TRUE,TRUE);
	walk_view_set_3D_projection(view_setup,map.settings.editor.view_near_dist,map.settings.editor.view_far_dist,walk_view_near_offset);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

        // draw opaque parts of portals in sight path
        
	walk_view_draw_meshes_texture(view_setup,TRUE);
	walk_view_draw_nodes(view_setup);
	walk_view_draw_spots_scenery(view_setup);
	walk_view_draw_lights_sounds_particles(view_setup);
	walk_view_draw_liquids(view_setup,TRUE);
	
		// draw opaque mesh lines
		// push view forward to better z-buffer lines
		
	walk_view_set_3D_projection(view_setup,(map.settings.editor.view_near_dist+10),(map.settings.editor.view_far_dist-10),walk_view_near_offset);
	walk_view_draw_meshes_line(view_setup,TRUE);

        // draw transparent parts of portals in sight path
        
	walk_view_set_3D_projection(view_setup,map.settings.editor.view_near_dist,map.settings.editor.view_far_dist,walk_view_near_offset);

	walk_view_draw_meshes_texture(view_setup,FALSE);
	walk_view_draw_liquids(view_setup,FALSE);
 	
        // draw transparent mesh lines
		// push view forward to better z-buffer lines
        
	walk_view_set_3D_projection(view_setup,(map.settings.editor.view_near_dist+10),(map.settings.editor.view_far_dist-10),walk_view_near_offset);
	walk_view_draw_meshes_line(view_setup,TRUE);
	
        // draw normals mesh lines
		// push view forward to better z-buffer lines
      
	if (state.show_normals) {
		walk_view_set_3D_projection(view_setup,(map.settings.editor.view_near_dist+20),(map.settings.editor.view_far_dist-20),walk_view_near_offset);
		walk_view_draw_meshes_normals(view_setup);
	}
		
		// draw selection
		
	walk_view_draw_select(&view_setup->pnt);

		// position
		
	walk_view_set_3D_projection(view_setup,map.settings.editor.view_near_dist,map.settings.editor.view_far_dist,walk_view_near_offset);
}

