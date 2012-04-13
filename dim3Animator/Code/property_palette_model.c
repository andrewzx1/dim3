/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Property Palette Model

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

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3animator.h"
#endif

#include "glue.h"
#include "ui_common.h"
#include "interface.h"

#define kModelPropertyRotateMode				0
#define kModelPropertyDiffuseBoost				1

#define kModelPropertyCenterOffset				2
#define kModelPropertyViewBoxOffset				3
#define kModelPropertyViewBoxSize				4

#define kModelPropertyNameBone					5
#define kModelPropertyLightBoneStart			100
#define kModelPropertyLightBoneEnd				(100+max_model_light)
#define kModelPropertyHaloBoneStart				200
#define kModelPropertyHaloBoneEnd				(200+max_model_halo)

#define kModelPropertyRigidBodyOn				6
#define kModelPropertyRigidBodyYResetFact		7
#define kModelPropertyRigidBodyYSmoothFact		8
#define kModelPropertyRigidBodyXMaxAngle		9
#define kModelPropertyRigidBodyXResetFact		10
#define kModelPropertyRigidBodyXSmoothFact		11
#define kModelPropertyRigidBodyZMaxAngle		12
#define kModelPropertyRigidBodyZResetFact		13
#define kModelPropertyRigidBodyZSmoothFact		14

#define kModelPropertyUIMinDiffuse				15
#define kModelPropertyUIDiffuseVector			16

#define kModelPropertyImportScale				17

extern model_type				model;
extern animator_state_type		state;
extern file_path_setup_type		file_path_setup;

extern list_palette_type		property_palette;

/* =======================================================

      Property Palette Fill Model
      
======================================================= */

void property_palette_fill_model(void)
{
	int				n;
	char			name[256];

	list_palette_set_title(&property_palette,"Model Properties",NULL,NULL,NULL,NULL,NULL);

		// model options
		
	list_palette_add_header(&property_palette,0,"Model Options");
	list_palette_add_checkbox(&property_palette,kModelPropertyRotateMode,"Cumulative Rotation",&model.comulative_rotation,FALSE);
	list_palette_add_float(&property_palette,kModelPropertyDiffuseBoost,"Diffuse+Bump Boost",&model.diffuse_boost,FALSE);

	list_palette_add_header(&property_palette,0,"Model Positions");
	list_palette_add_point(&property_palette,kModelPropertyCenterOffset,"Center",&model.center,FALSE);
	list_palette_add_point(&property_palette,kModelPropertyViewBoxOffset,"View Box Offset",&model.view_box.offset,FALSE);
	list_palette_add_point(&property_palette,kModelPropertyViewBoxSize,"View Box Size",&model.view_box.size,FALSE);

	list_palette_add_header(&property_palette,0,"Model Name Bone");
	list_palette_add_picker_list_int(&property_palette,kModelPropertyNameBone,"Bone",(char*)model.bones,model.nbone,sizeof(model_bone_type),(int)offsetof(model_bone_type,name),TRUE,&model.bone_connect.name_bone_idx,FALSE);

	list_palette_add_header(&property_palette,0,"Model Light Bones");
	for (n=0;n!=max_model_light;n++) {
		sprintf(name,"Bone %d",n);
		list_palette_add_picker_list_int(&property_palette,(kModelPropertyLightBoneStart+n),name,(char*)model.bones,model.nbone,sizeof(model_bone_type),(int)offsetof(model_bone_type,name),TRUE,&model.bone_connect.light_bone_idx[n],FALSE);
	}

	list_palette_add_header(&property_palette,0,"Model Halo Bones");
	for (n=0;n!=max_model_halo;n++) {
		sprintf(name,"Bone %d",n);
		list_palette_add_picker_list_int(&property_palette,(kModelPropertyHaloBoneStart+n),name,(char*)model.bones,model.nbone,sizeof(model_bone_type),(int)offsetof(model_bone_type,name),TRUE,&model.bone_connect.halo_bone_idx[n],FALSE);
	}

	list_palette_add_header(&property_palette,0,"Model Rigid Body");
	list_palette_add_checkbox(&property_palette,kModelPropertyRigidBodyOn,"On",&model.rigid_body.on,FALSE);
	list_palette_add_float(&property_palette,kModelPropertyRigidBodyYResetFact,"Y Reset Factor",&model.rigid_body.y.reset_factor,FALSE);
	list_palette_add_float(&property_palette,kModelPropertyRigidBodyYSmoothFact,"Y Smooth Factor",&model.rigid_body.y.smooth_factor,FALSE);
	list_palette_add_float(&property_palette,kModelPropertyRigidBodyXMaxAngle,"X Max Angle",&model.rigid_body.x.max_ang,FALSE);
	list_palette_add_float(&property_palette,kModelPropertyRigidBodyXResetFact,"X Reset Factor",&model.rigid_body.x.reset_factor,FALSE);
	list_palette_add_float(&property_palette,kModelPropertyRigidBodyXSmoothFact,"X Smooth Factor",&model.rigid_body.x.smooth_factor,FALSE);
	list_palette_add_float(&property_palette,kModelPropertyRigidBodyZMaxAngle,"Z Max Angle",&model.rigid_body.z.max_ang,FALSE);
	list_palette_add_float(&property_palette,kModelPropertyRigidBodyZResetFact,"Z Reset Fact",&model.rigid_body.z.reset_factor,FALSE);
	list_palette_add_float(&property_palette,kModelPropertyRigidBodyZSmoothFact,"Z Smooth Fact",&model.rigid_body.z.smooth_factor,FALSE);

	list_palette_add_header(&property_palette,0,"Model UI Fixed");
	list_palette_add_float(&property_palette,kModelPropertyUIMinDiffuse,"Minimum Diffuse",&model.ui.fixed.min_diffuse,FALSE);
	list_palette_add_normal_vector(&property_palette,kModelPropertyUIDiffuseVector,"Diffuse Vector",&model.ui.fixed.diffuse_vct,FALSE);

	list_palette_add_header(&property_palette,0,"Model UI Shader");
	list_palette_add_int(&property_palette,-1,"Light Intensity",&model.ui.shader.light_intensity,FALSE);
	list_palette_add_float(&property_palette,-1,"Light Exponent",&model.ui.shader.light_exponent,FALSE);
	list_palette_add_point(&property_palette,-1,"Light Offset",&model.ui.shader.light_offset,FALSE);
	list_palette_add_pick_color(&property_palette,-1,"Light Color",&model.ui.shader.light_color,FALSE);

	list_palette_add_header(&property_palette,0,"Model Import");
	list_palette_add_float(&property_palette,kModelPropertyImportScale,"Scale",&model.import.factor,FALSE);
}

/* =======================================================

      Property Palette Click Model
      
======================================================= */

void property_palette_click_model(bool double_click)
{
}

