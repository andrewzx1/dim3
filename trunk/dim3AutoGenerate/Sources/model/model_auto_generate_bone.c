/****************************** File *********************************

Module: dim3 Auto Generator
Author: Brian Barnes
 Usage: Model Auto Generation Bones

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

(c) 2000-2014 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3autogenerate.h"
#endif

#define ag_model_cube_head_x_size			180
#define ag_model_cube_head_y_size			200
#define ag_model_cube_head_z_size			150
#define ag_model_cube_hips_x_size			250
#define ag_model_cube_hips_y_size			200
#define ag_model_cube_hips_z_size			150
#define ag_model_cube_hand_x_size			100
#define ag_model_cube_hand_y_size			150
#define ag_model_cube_hand_z_size			80
#define ag_model_cube_foot_x_size			100
#define ag_model_cube_foot_y_size			50
#define ag_model_cube_foot_z_size			200

#define ag_model_cube_extra_size			40

/* =======================================================

      Bone Stats
      
======================================================= */

bool ag_model_bone_is_special(model_type *model,int bone_idx)
{
	int					n;
	model_bone_type		*bone;

	bone=&model->bones[bone_idx];

		// base bone

	if ((bone->pnt.x==0) && (bone->pnt.y==0) && (bone->pnt.z==0)) return(TRUE);
	if (strcmp(bone->name,"Base")==0) return(TRUE);

		// connector bones

	if (bone_idx==model->bone_connect.name_bone_idx) return(TRUE);

	for (n=0;n!=max_model_light;n++) {
		if (bone_idx==model->bone_connect.light_bone_idx[n]) return(TRUE);
	}

	for (n=0;n!=max_model_halo;n++) {
		if (bone_idx==model->bone_connect.halo_bone_idx[n]) return(TRUE);
	}

	return(FALSE);
}

bool ag_model_bone_is_body(model_type *model,int bone_idx)
{
	model_bone_type		*bone,*parent_bone;

	bone=&model->bones[bone_idx];
	if (bone->parent_idx==-1) return(FALSE);

	parent_bone=&model->bones[bone->parent_idx];

	if ((strcmp(bone->name,"Torso")==0) && (strcmp(parent_bone->name,"Hips")==0)) return(TRUE);
	return((strcmp(bone->name,"Hips")==0) && (strcmp(parent_bone->name,"Torse")==0));
}

bool ag_model_bone_is_hip(model_type *model,int bone_idx)
{
	model_bone_type		*bone;

	bone=&model->bones[bone_idx];
	return(strcmp(bone->name,"Hips")==0);
}

bool ag_model_bone_is_connected_to_hips(model_type *model,int bone_idx)
{
	model_bone_type		*bone,*parent_bone;

	bone=&model->bones[bone_idx];
	if (bone->parent_idx==-1) return(FALSE);

	parent_bone=&model->bones[bone->parent_idx];
	return(strcmp(parent_bone->name,"Hips")==0);
}

bool ag_model_bone_is_shallow_y(model_type *model,int bone_idx)
{
	model_bone_type		*bone;

	bone=&model->bones[bone_idx];

	if (bone->parent_idx==-1) return(FALSE);
	return(abs(bone->pnt.y-(model->bones[bone->parent_idx].pnt.y))<20);
}

bool ag_model_bone_is_cubed(model_type *model,int bone_idx,d3pnt *sz)
{
	if (strcmp(model->bones[bone_idx].name,"Head")==0) {
		sz->x=ag_model_cube_head_x_size+ag_random_int(ag_model_cube_extra_size);
		sz->y=ag_model_cube_head_y_size+ag_random_int(ag_model_cube_extra_size);
		sz->z=ag_model_cube_head_z_size+ag_random_int(ag_model_cube_extra_size);
		return(TRUE);
	}

	if (strstr(model->bones[bone_idx].name,"Hand")!=NULL) {
		sz->x=ag_model_cube_hand_x_size+ag_random_int(ag_model_cube_extra_size);
		sz->y=ag_model_cube_hand_y_size+ag_random_int(ag_model_cube_extra_size);
		sz->z=ag_model_cube_hand_z_size+ag_random_int(ag_model_cube_extra_size);
		return(TRUE);
	}

	if (strstr(model->bones[bone_idx].name,"Foot")!=NULL) {
		sz->x=ag_model_cube_foot_x_size+ag_random_int(ag_model_cube_extra_size);
		sz->y=ag_model_cube_foot_y_size+ag_random_int(ag_model_cube_extra_size);
		sz->z=ag_model_cube_foot_z_size+ag_random_int(ag_model_cube_extra_size);
		return(TRUE);
	}
	
	return(FALSE);
}

/* =======================================================

      Add Bones
      
======================================================= */

int ag_model_add_bone_single(model_type *model,char *name,int parent_idx,int x,int y,int z)
{
	int				bone_idx;
	model_bone_type	*bone;

	bone_idx=model_bone_add(model,x,y,z);

	bone=&model->bones[bone_idx];
	strcpy(bone->name,name);
	bone->parent_idx=parent_idx;

	return(bone_idx);
}

void ag_model_add_bones(model_type *model)
{
	int			base_bone_idx,hip_bone_idx,
				torso_bone_idx,head_bone_idx,
				left_shoulder_bone_idx,right_shoulder_bone_idx,
				left_elbow_bone_idx,right_elbow_bone_idx,
				left_wrist_bone_idx,right_wrist_bone_idx,
				left_hip_bone_idx,right_hip_bone_idx,
				left_knee_bone_idx,right_knee_bone_idx,
				left_ankle_bone_idx,right_ankle_bone_idx;

		// supergumba -- for now need to create in same
		// order as animations that exist

	hip_bone_idx=ag_model_add_bone_single(model,"Hips",-1,0,-1800,0);		// supergumba -- have to reset this later
	torso_bone_idx=ag_model_add_bone_single(model,"Torso",hip_bone_idx,0,-2625,0);

	left_shoulder_bone_idx=ag_model_add_bone_single(model,"Left Shoulder",torso_bone_idx,360,-2625,60);
	right_shoulder_bone_idx=ag_model_add_bone_single(model,"Right Shoulder",torso_bone_idx,-360,-2625,60);

	left_elbow_bone_idx=ag_model_add_bone_single(model,"Left Elbow",left_shoulder_bone_idx,420,-2115,60);
	right_elbow_bone_idx=ag_model_add_bone_single(model,"Right Elbow",right_shoulder_bone_idx,-420,-2115,60);

	left_wrist_bone_idx=ag_model_add_bone_single(model,"Left Wrist",left_elbow_bone_idx,450,-1650,0);
	right_wrist_bone_idx=ag_model_add_bone_single(model,"Right Wrist",right_elbow_bone_idx,-450,-1650,0);

	ag_model_add_bone_single(model,"Left Hand",left_wrist_bone_idx,450,-1500,0);
	ag_model_add_bone_single(model,"Right Hand",right_wrist_bone_idx,-450,-1500,0);

	right_hip_bone_idx=ag_model_add_bone_single(model,"Right Hip",hip_bone_idx,-150,-1650,0);
	left_hip_bone_idx=ag_model_add_bone_single(model,"Left Hip",hip_bone_idx,150,-1650,0);

	left_knee_bone_idx=ag_model_add_bone_single(model,"Left Knee",left_hip_bone_idx,120,-975,45);
	right_knee_bone_idx=ag_model_add_bone_single(model,"Right Knee",right_hip_bone_idx,-120,-975,45);

	left_ankle_bone_idx=ag_model_add_bone_single(model,"Left Ankle",left_knee_bone_idx,120,-240,90);
	right_ankle_bone_idx=ag_model_add_bone_single(model,"Right Ankle",right_knee_bone_idx,-120,-240,90);

	ag_model_add_bone_single(model,"Left Foot",left_ankle_bone_idx,120,-60,0);
	ag_model_add_bone_single(model,"Right Foot",right_ankle_bone_idx,-120,-60,0);

	head_bone_idx=ag_model_add_bone_single(model,"Head",torso_bone_idx,0,-2850,0);
	base_bone_idx=ag_model_add_bone_single(model,"Base",-1,0,0,0);
	model->bone_connect.name_bone_idx=ag_model_add_bone_single(model,"Name",head_bone_idx,0,-3600,0);

	model->bones[0].parent_idx=base_bone_idx;		// supergumba -- re-arrange these later when animations are created

		// fix parenting animation distances

	model_calculate_parents(model);
}
