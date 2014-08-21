/****************************** File *********************************

Module: dim3 Auto Generator
Author: Brian Barnes
 Usage: Auto Generator Private Model Defines

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

(c) 2000-2014 Klink! Software www.klinksoftware.com
 
*********************************************************************/

//
// bones
//

extern bool ag_model_bone_is_special(model_type *model,int bone_idx);
extern bool ag_model_bone_is_body(model_type *model,int bone_idx);
extern bool ag_model_bone_is_hip(model_type *model,int bone_idx);
extern bool ag_model_bone_is_connected_to_hips(model_type *model,int bone_idx);
extern bool ag_model_bone_is_shallow_y(model_type *model,int bone_idx);
extern bool ag_model_bone_is_head(model_type *model,int bone_idx);
extern bool ag_model_bone_is_hand(model_type *model,int bone_idx);
extern bool ag_model_bone_is_foot(model_type *model,int bone_idx);
extern bool ag_model_bone_is_decoration_ok(model_type *model,int bone_idx);
extern int ag_model_bone_get_torso_width(model_type *model);
extern int ag_model_bone_get_hip_width(model_type *model);
extern void ag_model_add_bones(model_type *model);

//
// pieces
//

extern void ag_model_piece_setup(void);
extern void ag_model_piece_bone_head(model_type *model,model_mesh_type *mesh,int bone_idx,int limb_radius);
extern void ag_model_piece_bone_hand(model_type *model,model_mesh_type *mesh,int bone_idx,int limb_radius);
extern void ag_model_piece_bone_foot(model_type *model,model_mesh_type *mesh,int bone_idx,int limb_radius);

//
// decorations
//

void ag_model_add_decorations(model_type *model);
