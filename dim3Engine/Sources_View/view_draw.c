/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View Drawing

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
	#include "dim3engine.h"
#endif

#include "objects.h"
#include "remotes.h"
#include "weapons.h"
#include "projectiles.h"
#include "models.h"
#include "lights.h"
#include "effects.h"
#include "cameras.h"
#include "consoles.h"
#include "interfaces.h"
#include "video.h"
#include "timing.h"

extern map_type				map;
extern camera_type			camera;
extern view_type			view;
extern server_type			server;
extern setup_type			setup;
extern hud_type				hud;
extern render_info_type		render_info;

extern view_render_type		view_camera_render,view_node_render;

float						shake_ang_x[16]={-1,0,1,2,1,0,-1,-2,-4,-2,0,4,8,12,8,4};

extern void draw_weapon_hand(obj_type *obj,weapon_type *weap);
extern void draw_background(void);
extern void draw_sky(void);
extern void model_calc_pose_bones(model_draw *draw);
extern void render_map_mesh_opaque(void);
extern void render_map_mesh_transparent(void);
extern void rain_draw(void);
extern bool fog_solid_on(void);
extern void fog_draw_textured(void);
extern void fog_solid_start(void);
extern void fog_solid_end(void);
extern void polygon_segment_start(void);
extern void polygon_segment_end(void);
extern void render_model_setup(model_draw *draw);
extern void render_model_build_vertex_lists(model_draw *draw);
extern void render_model_opaque(model_draw *draw);
extern void render_model_transparent(model_draw *draw);
extern void render_model_target(model_draw *draw,d3col *col);
extern void view_draw_liquid_tint(int liquid_idx);
extern void view_draw_effect_tint(void);
extern void view_draw_fade_draw(void);
extern void render_map_liquid_opaque(void);
extern void render_map_liquid_transparent(void);
extern void decal_render(void);
extern void view_create_area_mask(void);
extern void view_start_draw_list(void);
extern void view_add_mesh_draw_list(void);
extern void view_add_liquid_draw_list(void);
extern void view_setup_objects(int tick);
extern void view_setup_projectiles(int tick);
extern void view_add_effect_draw_list(int tick);
extern void view_add_halos(void);
extern bool view_compile_mesh_gl_lists(void);
extern void view_calculate_scope(obj_type *obj,obj_type *camera_obj);
extern void view_calculate_recoil(obj_type *obj);
extern void view_calculate_shakes(obj_type *obj);
extern void view_calculate_sways(obj_type *obj);
extern void view_calculate_bump(obj_type *obj);
extern void shadow_render_model(int item_type,int item_idx,model_draw *draw);
extern void shadow_render_mesh(int mesh_idx);

/* =======================================================

      Draw Debug Object
      
======================================================= */

void view_draw_object_debug_bounding_box(obj_type *obj)
{
	int				n,xsz,ysz,zsz,px[8],py[8],pz[8];
	float			fx,fy,fz;
	matrix_type		rot_x_mat,rot_y_mat,rot_z_mat;
	d3col			col;

		// bounding box

	xsz=obj->size.x>>1;
	zsz=obj->size.z>>1;
	
	ysz=obj->size.y;
	if (obj->duck.mode!=dm_stand) ysz-=obj->duck.y_move;
	if (obj->liquid.mode==lm_float) ysz+=obj->liquid.bob_y_move;

	px[0]=px[1]=px[4]=px[5]=-xsz;
	px[2]=px[3]=px[6]=px[7]=xsz;

	py[0]=py[1]=py[2]=py[3]=-ysz;
	py[4]=py[5]=py[6]=py[7]=0;

	pz[1]=pz[2]=pz[5]=pz[6]=-zsz;
	pz[0]=pz[3]=pz[4]=pz[7]=zsz;

	matrix_rotate_x(&rot_x_mat,obj->draw.setup.ang.x);
	matrix_rotate_z(&rot_z_mat,obj->draw.setup.ang.z);
	matrix_rotate_y(&rot_y_mat,obj->draw.setup.ang.y);

	for (n=0;n!=8;n++) {
		fx=(float)px[n];
		fy=(float)py[n];
		fz=(float)pz[n];
		
		matrix_vertex_multiply(&rot_x_mat,&fx,&fy,&fz);
		matrix_vertex_multiply(&rot_z_mat,&fx,&fy,&fz);
		matrix_vertex_multiply(&rot_y_mat,&fx,&fy,&fz);
		
		px[n]=(int)fx;
		py[n]=(int)fy;
		pz[n]=(int)fz;
	}
	
	for (n=0;n!=8;n++) {
		px[n]=px[n]+obj->pnt.x;
		py[n]=py[n]+obj->pnt.y;
		pz[n]=pz[n]+obj->pnt.z;
	}

		// draw box

	col.r=col.g=0.2f;
	col.b=1.0f;

	glLineWidth(2.0f);
	view_draw_next_vertex_object_3D_line_cube(&col,1.0f,px,py,pz);
	glLineWidth(1.0f);
}

void view_draw_object_debug_path(obj_type *obj)
{
	int				yadd;
	d3pnt			pnt;
	d3col			col;

	if (!object_auto_walk_get_seek_position(obj,&pnt)) return;

	col.r=col.b=0.0f;
	col.g=1.0f;

	yadd=obj->size.y>>1;

	glLineWidth(2.0f);
	view_draw_next_vertex_object_3D_line(&col,1.0f,obj->pnt.x,(obj->pnt.y-yadd),obj->pnt.z,pnt.x,(pnt.y-yadd),pnt.z);
	glLineWidth(1.0f);
}

void view_draw_object_debug_collision_ray(obj_type *obj)
{
	int			n;
	d3pnt		*spt,*ept;
	d3col		col;

	if (obj->suspend) return;
	if ((!obj->forward_move.moving) && (!obj->side_move.moving)) return;

	col.r=col.b=1.0f;
	col.g=0.0f;

	spt=obj->debug.collide_spt;
	ept=obj->debug.collide_ept;

	glLineWidth(2.0f);

	for (n=0;n!=collide_obj_ray_count;n++) {
		view_draw_next_vertex_object_3D_line(&col,1.0f,spt->x,spt->y,spt->z,ept->x,ept->y,ept->z);
		spt++;
		ept++;
	}

	glLineWidth(1.0f);
}

/* =======================================================

      Mesh Shadows
      
======================================================= */

void view_draw_mesh_shadows(void)
{
	int					n;

		// shadows on?
	
	if (!setup.shadow_on) return;

		// setup draw
		
	gl_3D_view();
	gl_3D_rotate(&view.render->camera.pnt,&view.render->camera.ang);
	gl_setup_project();

		// render the shadows

	for (n=0;n!=view.render->draw_list.count;n++) {
		if (view.render->draw_list.items[n].type!=view_render_type_mesh) continue;
		if (map.mesh.meshes[view.render->draw_list.items[n].idx].flag.shadow) shadow_render_mesh(view.render->draw_list.items[n].idx);
	}
}

/* =======================================================

      Draw Models in Scene
      
======================================================= */

void view_draw_model_opaque(void)
{
	int					n;
	obj_type			*obj;
	proj_type			*proj;
	
		// setup draw
		
	gl_3D_view();
	gl_3D_rotate(&view.render->camera.pnt,&view.render->camera.ang);
	gl_setup_project();

		// render the models

	for (n=0;n!=view.render->draw_list.count;n++) {

		switch (view.render->draw_list.items[n].type) {

			case view_render_type_object:
				obj=&server.objs[view.render->draw_list.items[n].idx];
				if (((view.render->draw_list.items[n].flag&view_list_item_flag_model_in_view)!=0x0) && (obj->draw.has_opaque)) {
					if (!obj->draw.built_vertex_list) {
						obj->draw.built_vertex_list=TRUE;
						render_model_build_vertex_lists(&obj->draw);
					}
					render_model_opaque(&obj->draw);
				}
				break;

			case view_render_type_projectile:
				proj=&server.projs[view.render->draw_list.items[n].idx];
				if (((view.render->draw_list.items[n].flag&view_list_item_flag_model_in_view)!=0x0) && (proj->draw.has_opaque)) {
					render_model_build_vertex_lists(&proj->draw);	// projectiles share vertex lists
					render_model_opaque(&proj->draw);
				}
				break;

		}
	}
}

void view_draw_model_transparent(void)
{
	int					n;
	obj_type			*obj;
	proj_type			*proj;
	
		// setup draw
		
	gl_3D_view();
	gl_3D_rotate(&view.render->camera.pnt,&view.render->camera.ang);
	gl_setup_project();

		// render the models
		// draw backwards to sort back to front

	for (n=(view.render->draw_list.count-1);n>=0;n--) {

		switch (view.render->draw_list.items[n].type) {

			case view_render_type_object:
				obj=&server.objs[view.render->draw_list.items[n].idx];
				if (((view.render->draw_list.items[n].flag&view_list_item_flag_model_in_view)!=0x0) && (obj->draw.has_transparent)) {
					if (!obj->draw.built_vertex_list) {
						obj->draw.built_vertex_list=TRUE;
						render_model_build_vertex_lists(&obj->draw);
					}
					render_model_transparent(&obj->draw);
				}
				break;

			case view_render_type_projectile:
				proj=&server.projs[view.render->draw_list.items[n].idx];
				if (((view.render->draw_list.items[n].flag&view_list_item_flag_model_in_view)!=0x0) && (proj->draw.has_transparent)) {
					render_model_build_vertex_lists(&proj->draw);		// projectiles share draw lists
					render_model_transparent(&proj->draw);
				}
				break;

		}
	}
}

void view_draw_models_final(void)
{
	int					n;
	bool				shadow_on;
	d3col				col;
	obj_type			*obj;
	proj_type			*proj;
	
		// setup draw
		
	gl_3D_view();
	gl_3D_rotate(&view.render->camera.pnt,&view.render->camera.ang);
	gl_setup_project();

		// shadow overrides

	shadow_on=setup.shadow_on;

		// render the shadows, remote names,
		// and any debugging information

	for (n=0;n!=view.render->draw_list.count;n++) {

		switch (view.render->draw_list.items[n].type) {

			case view_render_type_object:
				obj=&server.objs[view.render->draw_list.items[n].idx];
				
				if ((shadow_on) && (obj->draw.shadow.on)) {
					if ((view.render->draw_list.items[n].flag&view_list_item_flag_shadow_in_view)!=0x0) {
						if (!obj->draw.built_vertex_list) {
							obj->draw.built_vertex_list=TRUE;
							render_model_build_vertex_lists(&obj->draw);
						}
						shadow_render_model(view_render_type_object,view.render->draw_list.items[n].idx,&obj->draw);
					}
				}
				
				if ((view.render->draw_list.items[n].flag&view_list_item_flag_model_in_view)!=0x0) {
					if (obj->type_idx==object_type_remote) remote_draw_status(obj);
					if (object_is_targetted(obj,&col)) render_model_target(&obj->draw,&col);
					if (view.debug.on) {
						view_draw_object_debug_bounding_box(obj);
						view_draw_object_debug_path(obj);
						view_draw_object_debug_collision_ray(obj);
					}
				}
				break;

			case view_render_type_projectile:
				proj=&server.projs[view.render->draw_list.items[n].idx];
				if ((shadow_on) && (proj->draw.shadow.on)) {
					if ((view.render->draw_list.items[n].flag&view_list_item_flag_shadow_in_view)!=0x0) {
						render_model_setup(&proj->draw);
						render_model_build_vertex_lists(&proj->draw);
						shadow_render_model(view_render_type_projectile,view.render->draw_list.items[n].idx,&proj->draw);
					}
				}
				break;

		}
	}
}

/* =======================================================

      Drawing Mainline
      
======================================================= */

void view_draw_scene_build(void)
{
	int			tick;

	tick=game_time_get();		// supergumba -- move these ticks around

		// setup projection

	gl_3D_view();
	gl_3D_rotate(&view.render->camera.pnt,&view.render->camera.ang);
	gl_setup_project();
	
		// compile all lights in map
		
	gl_lights_compile(tick);

		// setup draw lists

	view_create_area_mask();
	view_start_draw_list();

		// add mesh and liquids to draw list

	view_add_mesh_draw_list();
	view_add_liquid_draw_list();
	
		// setup objects and projectiles
		// and add to draw list
		
	view_setup_objects(tick);
	view_setup_projectiles(tick);

		// add scene effects

	view_add_effect_draw_list(tick);

		// add scene halos
		
	halo_draw_clear();
	view_add_halos();
}

void view_draw_scene_render(obj_type *obj,weapon_type *weap)
{
		// setup projection

	gl_3D_view();
	gl_3D_rotate(&view.render->camera.pnt,&view.render->camera.ang);
	gl_setup_project();

		// draw background and sky
	
	draw_background();
	draw_sky();
	
		// turn on obscuring fog
	
	if (fog_solid_on()) fog_solid_start();

		// setup per-scene shader variables

	gl_shader_draw_scene_initialize();

		// compile meshes for drawing
	
	if (!view_compile_mesh_gl_lists()) return;

		// draw opaque scene items

	render_map_mesh_opaque();
	render_map_liquid_opaque();
	view_draw_model_opaque();
	
		// additional mesh and model drawing
		// shadows, remote names, etc

	view_draw_mesh_shadows();
	view_draw_models_final();
	
		// draw transparent scene items

	render_map_mesh_transparent();
	view_draw_model_transparent();
	render_map_liquid_transparent();

		// draw decals

	decal_render();

		// effects

	effect_draw();
	
		// rain and fog
		
	rain_draw();
	fog_draw_textured();
	if (fog_solid_on()) fog_solid_end();
	
		// setup halos, crosshairs, zoom masks
		
	remote_draw_names_setup();
	halo_draw_setup();
	
	if ((obj!=NULL) && (weap!=NULL)) {
		crosshair_setup(obj,weap);
		zoom_setup(obj,weap);
	
			// draw the weapons in hand

		if (camera.mode==cv_fpp) draw_weapon_hand(obj,weap);
	}

		// draw the remote names, halos, crosshairs, and zoom masks
	
	remote_draw_names_render();
	halo_draw_render();
	
	if ((obj!=NULL) && (weap!=NULL)) {
		crosshair_draw(obj,weap);
		zoom_draw(obj,weap);
	}
}

void view_draw(void)
{
	obj_type		*obj,*camera_obj;
	weapon_type		*weap;

		// get player object and held weapon
		
	obj=object_find_uid(server.player_obj_uid);
	weap=weapon_find_current(obj);

		// camera render

	view.render=&view_camera_render;
	
		// set view camera
	
	camera_obj=object_find_uid(camera.obj_uid);
	camera_get_position(&view.render->camera.pnt,&view.render->camera.ang);

	view.render->camera.fov=camera.plane.fov;
	view.render->camera.flip=FALSE;
	view.render->camera.under_liquid_idx=camera_check_liquid(obj,&view.render->camera.pnt);
	view.render->camera.z_adjust=obj->camera_z_adjust;
	
	view.render->force_camera_obj=FALSE;

		// camera adjustments
	
	if (camera.mode==cv_fpp) {
		view_calculate_scope(obj,camera_obj);
		view_calculate_recoil(obj);
	}
	
	view_calculate_shakes(obj);
	view_calculate_sways(obj);
	view_calculate_bump(obj);
	
		// build the scene
		
	view_draw_scene_build();
	
		// do any back frame rendering
		
	gl_back_render_frame_start();

		// render the scene

	gl_fs_shader_render_begin();
	view_draw_scene_render(obj,weap);
	gl_fs_shader_render_finish();

		// draw tints and fades
		
	view_draw_liquid_tint(view.render->camera.under_liquid_idx);
	view_draw_effect_tint();
	view_draw_fade_draw();
}

bool view_draw_node(node_type *node)
{
	d3pnt			pnt;
	d3ang			ang;

		// switch out to node rendering

	view.render=&view_node_render;
	
		// camera position
		
	memmove(&view.render->camera.pnt,&node->pnt,sizeof(d3pnt));

	if (!node->follow_camera) {
		memmove(&view.render->camera.ang,&node->ang,sizeof(d3ang));
	}
	else {
		camera_get_position(&pnt,&ang);

		view.render->camera.ang.x=angle_find(node->pnt.y,node->pnt.z,pnt.y,pnt.z);
		view.render->camera.ang.y=angle_find(node->pnt.x,node->pnt.z,pnt.x,pnt.z);
		view.render->camera.ang.z=angle_find(node->pnt.x,node->pnt.y,pnt.x,pnt.y);
	}

	view.render->camera.fov=camera.plane.fov;
	view.render->camera.flip=TRUE;
	view.render->camera.under_liquid_idx=-1;
	view.render->camera.z_adjust=0;
	
	view.render->force_camera_obj=TRUE;

		// clear the frame
	
	gl_frame_clear(TRUE);
		
		// build the scene
		
	view_draw_scene_build();

		// render the scene

	view_draw_scene_render(NULL,NULL);

		// restore the old rendering
		
	view.render=&view_camera_render;
	
	return(TRUE);
}

