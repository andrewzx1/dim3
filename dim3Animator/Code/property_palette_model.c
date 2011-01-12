/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Property Palette Model

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
	#include "dim3animator.h"
#endif

#include "glue.h"
#include "ui_common.h"
#include "interface.h"
#include "dialog.h"

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

#define kModelPropertyImportSingleTexture		15
#define kModelPropertyImportScale				16

extern model_type				model;
extern animator_state_type		state;
extern file_path_setup_type		file_path_setup;

extern list_palette_type		property_palette;

/* =======================================================

      Property Palette Fill Model
      
======================================================= */

void property_palette_fill_model(void)
{
	int				n,idx;
	char			str[256];

		// model options

	list_palette_add_header(&property_palette,0,"Model Options");
	list_palette_add_checkbox(&property_palette,kModelPropertyRotateMode,"Cumulative Rotation",(model.deform_mode==deform_mode_comulative_rotate),FALSE);
	list_palette_add_string_float(&property_palette,kModelPropertyDiffuseBoost,"Diffuse Boost",model.diffuse_boost,FALSE);

	list_palette_add_header(&property_palette,0,"Model Positions");
	list_palette_add_point(&property_palette,kModelPropertyCenterOffset,"Center",&model.center,FALSE);
	list_palette_add_point(&property_palette,kModelPropertyViewBoxOffset,"View Box Offset",&model.view_box.offset,FALSE);
	list_palette_add_point(&property_palette,kModelPropertyViewBoxSize,"View Box Size",&model.view_box.size,FALSE);

	list_palette_add_header(&property_palette,0,"Model Name Bone");
	str[0]=0x0;
	idx=model_find_bone(&model,model.tags.name_bone_tag);
	if (idx!=-1) strcpy(str,model.bones[idx].name);
	list_palette_add_string(&property_palette,kModelPropertyNameBone,"Bone",str,FALSE);

	list_palette_add_header(&property_palette,0,"Model Light Bones");
	for (n=0;n!=max_model_light;n++) {
		str[0]=0x0;
		idx=model_find_bone(&model,model.tags.light_bone_tag[n]);
		if (idx!=-1) strcpy(str,model.bones[idx].name);
		list_palette_add_string(&property_palette,(kModelPropertyLightBoneStart+n),"Bone",str,FALSE);
	}

	list_palette_add_header(&property_palette,0,"Model Halo Bones");
	for (n=0;n!=max_model_halo;n++) {
		str[0]=0x0;
		idx=model_find_bone(&model,model.tags.halo_bone_tag[n]);
		if (idx!=-1) strcpy(str,model.bones[idx].name);
		list_palette_add_string(&property_palette,(kModelPropertyHaloBoneStart+n),"Bone",str,FALSE);
	}

	list_palette_add_header(&property_palette,0,"Model Rigid Body");
	list_palette_add_checkbox(&property_palette,kModelPropertyRigidBodyOn,"On",model.rigid_body.on,FALSE);
	list_palette_add_string_float(&property_palette,kModelPropertyRigidBodyYResetFact,"Y Reset Factor",model.rigid_body.y.reset_factor,FALSE);
	list_palette_add_string_float(&property_palette,kModelPropertyRigidBodyYSmoothFact,"Y Smooth Factor",model.rigid_body.y.smooth_factor,FALSE);
	list_palette_add_string_float(&property_palette,kModelPropertyRigidBodyXMaxAngle,"X Max Angle",model.rigid_body.x.max_ang,FALSE);
	list_palette_add_string_float(&property_palette,kModelPropertyRigidBodyXResetFact,"X Reset Factor",model.rigid_body.x.reset_factor,FALSE);
	list_palette_add_string_float(&property_palette,kModelPropertyRigidBodyXSmoothFact,"X Smooth Factor",model.rigid_body.x.smooth_factor,FALSE);
	list_palette_add_string_float(&property_palette,kModelPropertyRigidBodyZMaxAngle,"Z Max Angle",model.rigid_body.z.max_ang,FALSE);
	list_palette_add_string_float(&property_palette,kModelPropertyRigidBodyZResetFact,"Z Reset Fact",model.rigid_body.z.reset_factor,FALSE);
	list_palette_add_string_float(&property_palette,kModelPropertyRigidBodyZSmoothFact,"Z Smooth Fact",model.rigid_body.z.smooth_factor,FALSE);

	list_palette_add_header(&property_palette,0,"Model Import");
	list_palette_add_checkbox(&property_palette,kModelPropertyImportSingleTexture,"Single Texture",model.import.single_texture,FALSE);
	list_palette_add_string_float(&property_palette,kModelPropertyImportScale,"Scale",model.import.factor,FALSE);
}

/* =======================================================

      Property Palette Click Model
      
======================================================= */

void property_palette_click_model(int id)
{
	int					idx;

		// light and halo bones

	if ((id>=kModelPropertyLightBoneStart) && (id<=kModelPropertyLightBoneEnd)) {
		idx=id-kModelPropertyLightBoneStart;
		property_palette_pick_bone_tag((unsigned long*)&model.tags.light_bone_tag[idx]);
		main_wind_draw();
		return;
	}

	if ((id>=kModelPropertyHaloBoneStart) && (id<=kModelPropertyHaloBoneEnd)) {
		idx=id-kModelPropertyHaloBoneStart;
		property_palette_pick_bone_tag((unsigned long*)&model.tags.halo_bone_tag[idx]);
		main_wind_draw();
		return;
	}


		// regular ids

	switch (id) {

		case kModelPropertyRotateMode:
			if (model.deform_mode!=deform_mode_comulative_rotate) {
				model.deform_mode=deform_mode_comulative_rotate;
			}
			else {
				model.deform_mode=deform_mode_single_rotate;
			}
			break;
			
		case kModelPropertyDiffuseBoost:
			dialog_property_string_run(list_string_value_float,(void*)&model.diffuse_boost,0,0,0);
			break;

		case kModelPropertyCenterOffset:
			dialog_property_string_run(list_string_value_point,(void*)&model.center,0,0,0);
			break;

		case kModelPropertyViewBoxOffset:
			dialog_property_string_run(list_string_value_point,(void*)&model.view_box.offset,0,0,0);
			break;

		case kModelPropertyViewBoxSize:
			dialog_property_string_run(list_string_value_point,(void*)&model.view_box.size,0,0,0);
			break;

		case kModelPropertyNameBone:
			property_palette_pick_bone_tag((unsigned long*)&model.tags.name_bone_tag);
			break;

		case kModelPropertyRigidBodyOn:
			model.rigid_body.on=!model.rigid_body.on;
			break;

		case kModelPropertyRigidBodyYResetFact:
			dialog_property_string_run(list_string_value_positive_float,(void*)&model.rigid_body.y.reset_factor,0,0,0);
			break;

		case kModelPropertyRigidBodyYSmoothFact:
			dialog_property_string_run(list_string_value_positive_float,(void*)&model.rigid_body.y.smooth_factor,0,0,0);
			break;

		case kModelPropertyRigidBodyXMaxAngle:
			dialog_property_string_run(list_string_value_positive_float,(void*)&model.rigid_body.x.max_ang,0,0,0);
			break;

		case kModelPropertyRigidBodyXResetFact:
			dialog_property_string_run(list_string_value_positive_float,(void*)&model.rigid_body.x.reset_factor,0,0,0);
			break;

		case kModelPropertyRigidBodyXSmoothFact:
			dialog_property_string_run(list_string_value_positive_float,(void*)&model.rigid_body.x.smooth_factor,0,0,0);
			break;

		case kModelPropertyRigidBodyZMaxAngle:
			dialog_property_string_run(list_string_value_positive_float,(void*)&model.rigid_body.z.max_ang,0,0,0);
			break;

		case kModelPropertyRigidBodyZResetFact:
			dialog_property_string_run(list_string_value_positive_float,(void*)&model.rigid_body.z.reset_factor,0,0,0);
			break;

		case kModelPropertyRigidBodyZSmoothFact:
			dialog_property_string_run(list_string_value_positive_float,(void*)&model.rigid_body.z.smooth_factor,0,0,0);
			break;

		case kModelPropertyImportSingleTexture:
			model.import.single_texture=!model.import.single_texture;
			break;

		case kModelPropertyImportScale:
			dialog_property_string_run(list_string_value_positive_float,(void*)&model.import.factor,0,0,0);
			break;

	}

		// redraw

	main_wind_draw();
}

