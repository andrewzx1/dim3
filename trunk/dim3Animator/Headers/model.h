/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
 Usage: Model Edit, Draw, Click Functions

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
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

extern void draw_model(int mesh_idx);
extern void draw_model_mesh(int mesh_idx);
extern void draw_model_bones(int sel_bone_idx);

extern void draw_model_selected_vertexes(int mesh_idx);
extern void draw_model_selected_trig(int mesh_idx);
extern void draw_model_box_view(void);
extern void draw_model_box_hit_boxes(void);
extern void draw_model_axis(void);
extern void draw_model_normals_vertexes(int mesh_idx);
extern void draw_model_normals_trig(int mesh_idx);

extern void draw_model_gl_setup(int z_offset);
extern void draw_model_setup_pose(int pose_idx);
extern void draw_model_wind_pose(int mesh_idx,int pose_idx);

extern bool vertex_mask_initialize(void);
extern void vertex_mask_shutdown(void);
extern void vertex_clear_sel_mask(int mesh_idx);
extern void vertex_set_sel_mask(int mesh_idx,int vertex_idx,bool value);
extern void vertex_set_sel_mask_all(int mesh_idx);
extern bool vertex_check_any(int mesh_idx);
extern bool vertex_check_sel_mask(int mesh_idx,int vertex_idx);
extern void vertex_clear_hide_mask(int mesh_idx);
extern void vertex_set_hide_mask(int mesh_idx,int vertex_idx,bool value);
extern bool vertex_check_hide_mask(int mesh_idx,int vertex_idx);
extern void vertex_hide_mask_set_sel_vertexes(int mesh_idx);
extern void vertex_hide_mask_set_non_sel_vertexes(int mesh_idx);
extern void vertex_hide_mask_show_all_vertexes(int mesh_idx);
extern bool vertex_check_hide_mask_trig(int mesh_idx,model_trig_type *trig);
extern void vertex_set_sel_mask_bone(int mesh_idx,int bone_idx);
extern void vertex_set_sel_mask_no_bone(int mesh_idx);
extern void vertex_set_sel_mask_near_bone(int mesh_idx,int bone_idx,float percentage);
extern void vertex_set_sel_vertex_to_bone(int mesh_idx,int major_bone_idx,int minor_bone_idx,float factor);
extern void vertex_set_sel_mask_material(int mesh_idx,int material_idx);

extern void vertex_find_center_sel_vertexes(int mesh_idx,int *p_cx,int *p_cy,int *p_cz);
extern void vertex_move_sel_vertexes(int mesh_idx,int x,int y,int z);
extern void vertex_scale_sel_vertexes(int mesh_idx,float x,float y,float z);
extern void vertex_rotate_sel_vertexes(int mesh_idx,float ang_x,float ang_y,float ang_z);
extern void vertex_invert_normals(int mesh_idx);
extern void vertex_clear_bone_attachments_sel_vertexes(int mesh_idx);
extern void vertex_delete_sel_vertex(int mesh_idx);
extern void vertex_delete_unused_vertexes(int mesh_idx);

extern bool model_pick_list_start(int count);
extern void model_pick_list_end(d3pnt *pnt,int *idx);
extern void model_pick_list_add_trig(int idx,d3pnt *v_pnts);

