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

/* =======================================================

      Bone Stats
      
======================================================= */

bool ag_model_bone_is_special(model_type *model,int bone_idx)
{
	int					n;
	model_bone_type		*bone;

	bone=&model->bones[bone_idx];

		// base bone

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

bool ag_model_bone_is_head(model_type *model,int bone_idx)
{
	return(strcmp(model->bones[bone_idx].name,"Head")==0);
}

bool ag_model_bone_is_hand(model_type *model,int bone_idx)
{
	return(strstr(model->bones[bone_idx].name,"Hand")!=NULL);
}

bool ag_model_bone_is_foot(model_type *model,int bone_idx)
{
	return(strstr(model->bones[bone_idx].name,"Foot")!=NULL);
}

bool ag_model_bone_is_decoration_ok(model_type *model,int bone_idx)
{
	if (strcmp(model->bones[bone_idx].name,"Head")==0) return(FALSE);
	if (strcmp(model->bones[bone_idx].name,"Base")==0) return(FALSE);
	if (strcmp(model->bones[bone_idx].name,"Name")==0) return(FALSE);
	if (strstr(model->bones[bone_idx].name,"Hand")!=NULL) return(FALSE);
	if (strstr(model->bones[bone_idx].name,"Foot")!=NULL) return(FALSE);

	return(TRUE);
}

/* =======================================================

      Distances
      
======================================================= */

int ag_model_bone_get_torso_width(model_type *model)
{
	int					n,min,max;
	model_bone_type		*bone;

	bone=model->bones;

	min=max=0;

	for (n=0;n!=model->nbone;n++) {
		if (strcmp(bone->name,"Left Shoulder")==0) min=bone->pnt.x;
		if (strcmp(bone->name,"Right Shoulder")==0) max=bone->pnt.x;
		bone++;
	}

	return(abs(max-min)>>1);
}

int ag_model_bone_get_hip_width(model_type *model)
{
	int					n,min,max;
	model_bone_type		*bone;

	bone=model->bones;

	min=max=0;

	for (n=0;n!=model->nbone;n++) {
		if (strcmp(bone->name,"Left Hip")==0) min=bone->pnt.x;
		if (strcmp(bone->name,"Right Hip")==0) max=bone->pnt.x;
		bone++;
	}

	return(abs(max-min)>>1);
}

/* =======================================================

      Add Bones
      
======================================================= */

int ag_model_add_bone_single(model_type *model,char *name,int parent_idx,int x,int y,int z)
{
	int				bone_idx;
	model_bone_type	*bone,*ptr;

	/*
	// supergumba -- normally adding a bone deletes
	// any pose moves, so for now we need to do this by hand,
	// eventually add back the real way

	bone_idx=model_bone_add(model,x,y,z);
	*/

	ptr=(model_bone_type*)malloc(sizeof(model_bone_type)*(model->nbone+1));
	if (model->bones!=NULL) {
		memmove(ptr,model->bones,(sizeof(model_bone_type)*model->nbone));
		free(model->bones);
	}
	model->bones=ptr;

	bone_idx=model->nbone;
	model->nbone++;

	bone=&model->bones[bone_idx];
	bzero(bone,sizeof(model_bone_type));
	
	bone->parent_idx=-1;
	bone->pnt.x=x;
	bone->pnt.y=y;
	bone->pnt.z=z;

	bone->natural_offset.x=bone->natural_offset.y=bone->natural_offset.z=0;
	bone->natural_rot.x=bone->natural_rot.y=bone->natural_rot.z=0.0f;
// end of stuff to delete later


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
	int			hip_high,torso_high,torso_radius,arm_swing,
				head_high,hand_high,elbow_high;

		// random sizes

	hip_high=1000+ag_random_int(1500);
	torso_high=hip_high+(500+ag_random_int(500));
	torso_radius=300+ag_random_int(350);
	arm_swing=40+ag_random_int(40);
	head_high=torso_high+(150+ag_random_int(200));
	hand_high=800+ag_random_int(1000);
	elbow_high=((torso_high-hand_high)>>1)+hand_high;

		// supergumba -- for now need to create in same
		// order as animations that exist

	hip_bone_idx=ag_model_add_bone_single(model,"Hips",-1,0,-hip_high,0);		// supergumba -- have to reset this later
	torso_bone_idx=ag_model_add_bone_single(model,"Torso",hip_bone_idx,0,-torso_high,0);

	left_shoulder_bone_idx=ag_model_add_bone_single(model,"Left Shoulder",torso_bone_idx,torso_radius,-torso_high,60);
	right_shoulder_bone_idx=ag_model_add_bone_single(model,"Right Shoulder",torso_bone_idx,-torso_radius,-torso_high,60);

	left_elbow_bone_idx=ag_model_add_bone_single(model,"Left Elbow",left_shoulder_bone_idx,(torso_radius+arm_swing),-elbow_high,60);
	right_elbow_bone_idx=ag_model_add_bone_single(model,"Right Elbow",right_shoulder_bone_idx,-(torso_radius+arm_swing),-elbow_high,60);

	left_wrist_bone_idx=ag_model_add_bone_single(model,"Left Wrist",left_elbow_bone_idx,((torso_radius+arm_swing)+30),-(hand_high+150),0);
	right_wrist_bone_idx=ag_model_add_bone_single(model,"Right Wrist",right_elbow_bone_idx,-((torso_radius+arm_swing)+30),-(hand_high+150),0);

	ag_model_add_bone_single(model,"Left Hand",left_wrist_bone_idx,((torso_radius+arm_swing)+30),-hand_high,0);
	ag_model_add_bone_single(model,"Right Hand",right_wrist_bone_idx,-((torso_radius+arm_swing)+30),-hand_high,0);

	right_hip_bone_idx=ag_model_add_bone_single(model,"Right Hip",hip_bone_idx,-150,-(hip_high-200),0);
	left_hip_bone_idx=ag_model_add_bone_single(model,"Left Hip",hip_bone_idx,150,-(hip_high-200),0);

	left_knee_bone_idx=ag_model_add_bone_single(model,"Left Knee",left_hip_bone_idx,120,-(hip_high>>1),45);
	right_knee_bone_idx=ag_model_add_bone_single(model,"Right Knee",right_hip_bone_idx,-120,-(hip_high>>1),45);

	left_ankle_bone_idx=ag_model_add_bone_single(model,"Left Ankle",left_knee_bone_idx,120,-240,90);
	right_ankle_bone_idx=ag_model_add_bone_single(model,"Right Ankle",right_knee_bone_idx,-120,-240,90);

	ag_model_add_bone_single(model,"Left Foot",left_ankle_bone_idx,120,-60,0);
	ag_model_add_bone_single(model,"Right Foot",right_ankle_bone_idx,-120,-60,0);

	head_bone_idx=ag_model_add_bone_single(model,"Head",torso_bone_idx,0,-head_high,0);
	base_bone_idx=ag_model_add_bone_single(model,"Base",-1,0,0,0);
	model->bone_connect.name_bone_idx=ag_model_add_bone_single(model,"Name",head_bone_idx,0,-(head_high+1000),0);

	model->bones[0].parent_idx=base_bone_idx;		// supergumba -- re-arrange these later when animations are created
}
