/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Alt Property Palette Animate Pose Move

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

#define kAnimatePoseMovePropertyPose						0
#define kAnimatePoseMovePropertyMilliseconds				1
#define kAnimatePoseMovePropertyAcceleration				2

#define kAnimatePoseMovePropertyMove						3
#define kAnimatePoseMovePropertySway						4

#define kAnimatePoseMovePropertyLoopStart					5
#define kAnimatePoseMovePropertyLoopEnd						6

#define kAnimationPoseMovePropertySoundBone					7
#define kAnimationPoseMovePropertySoundName					8
#define kAnimationPoseMovePropertySoundPitch				9
#define kAnimationPoseMovePropertySoundGlobal				10

#define kAnimationPoseMovePropertyFadeName					11
#define kAnimationPoseMovePropertyFadeMesh					12
#define kAnimationPoseMovePropertyFadeIn					13
#define kAnimationPoseMovePropertyFadeLife					14
#define kAnimationPoseMovePropertyFadeOut					15

#define kAnimationPoseMovePropertyFlashBone					16
#define kAnimationPoseMovePropertyFlashIntensity			17
#define kAnimationPoseMovePropertyFlashFlash				18
#define kAnimationPoseMovePropertyFlashFade					19
#define kAnimationPoseMovePropertyFlashExponent				20
#define kAnimationPoseMovePropertyFlashColor				21

#define kAnimationPoseMovePropertyShakeDistance				22
#define kAnimationPoseMovePropertyShakeSize					23
#define kAnimationPoseMovePropertyShakeLife					24

#define kAnimationPoseMovePropertyParticleAdd				30
#define kAnimationPoseMovePropertyRingAdd					31

#define kAnimationPoseMovePropertyParticle					1000
#define kAnimationPoseMovePropertyParticleDelete			1100

#define kAnimationPoseMovePropertyRing						2000
#define kAnimationPoseMovePropertyRingDelete				2100

extern model_type				model;
extern animator_state_type		state;
extern file_path_setup_type		file_path_setup;

extern list_palette_type		alt_property_palette;

/* =======================================================

      Alt Property Palette Fill Animate Pose Move
      
======================================================= */

void alt_property_palette_fill_animate_pose_move(int animate_idx,int pose_move_idx)
{
	int						n;
	model_animate_type		*animate;
	model_pose_move_type	*pose_move;

	if (pose_move_idx==-1) return;

	animate=&model.animates[animate_idx];
	pose_move=&animate->pose_moves[pose_move_idx];

	list_palette_set_title(&alt_property_palette,"Animation",animate->name,"Pose Move",model.poses[pose_move->pose_idx].name,NULL,NULL);

		// options

	list_palette_add_header(&alt_property_palette,0,"Animate Pose Options");
	list_palette_add_string(&alt_property_palette,kAnimatePoseMovePropertyPose,"Pose",model.poses[pose_move->pose_idx].name,FALSE);
	list_palette_add_int(&alt_property_palette,kAnimatePoseMovePropertyMilliseconds,"Milliseconds",&pose_move->msec,FALSE);
	list_palette_add_float(&alt_property_palette,kAnimatePoseMovePropertyAcceleration,"Acceleration",&pose_move->acceleration,FALSE);

		// position

	list_palette_add_header(&alt_property_palette,0,"Animate Pose Position");
	list_palette_add_vector(&alt_property_palette,kAnimatePoseMovePropertyMove,"Move",&pose_move->mov,FALSE);
	list_palette_add_vector(&alt_property_palette,kAnimatePoseMovePropertySway,"Sway",&pose_move->sway,FALSE);

		// loop

	list_palette_add_header(&alt_property_palette,0,"Animate Pose Loop");
	list_palette_add_string_selectable_button(&alt_property_palette,kAnimatePoseMovePropertyLoopStart,list_button_set,kAnimatePoseMovePropertyLoopStart,"Set Pose As Loop Start",NULL,FALSE,FALSE);
	list_palette_add_string_selectable_button(&alt_property_palette,kAnimatePoseMovePropertyLoopEnd,list_button_set,kAnimatePoseMovePropertyLoopEnd,"Set Pose As Loop End",NULL,FALSE,FALSE);

		// sound

	list_palette_add_header(&alt_property_palette,0,"Animate Pose Sound");
	list_palette_add_string(&alt_property_palette,kAnimationPoseMovePropertySoundName,"Sound",pose_move->sound.name,FALSE);
	property_palette_add_string_bone(&alt_property_palette,kAnimationPoseMovePropertySoundBone,"Bone",pose_move->sound.bone_idx,FALSE);
	list_palette_add_float(&alt_property_palette,kAnimationPoseMovePropertySoundPitch,"Pitch",&pose_move->sound.pitch,FALSE);
	list_palette_add_checkbox(&alt_property_palette,kAnimationPoseMovePropertySoundGlobal,"Play Globally (Player Only)",&pose_move->sound.no_position,FALSE);

		// mesh fades

	list_palette_add_header(&alt_property_palette,0,"Animate Pose Mesh Fade");
	property_palette_add_string_mesh(&alt_property_palette,kAnimationPoseMovePropertyFadeMesh,"Mesh",pose_move->mesh_fade.mesh_idx,FALSE);
	list_palette_add_int(&alt_property_palette,kAnimationPoseMovePropertyFadeIn,"Fade In Milliseconds",&pose_move->mesh_fade.fade_in_msec,FALSE);
	list_palette_add_int(&alt_property_palette,kAnimationPoseMovePropertyFadeLife,"Fade Life Milliseconds",&pose_move->mesh_fade.fade_life_msec,FALSE);
	list_palette_add_int(&alt_property_palette,kAnimationPoseMovePropertyFadeOut,"Fade Out Milliseconds",&pose_move->mesh_fade.fade_out_msec,FALSE);

		// flash

	list_palette_add_header(&alt_property_palette,0,"Animate Pose Flash");
	property_palette_add_string_bone(&alt_property_palette,kAnimationPoseMovePropertyFlashBone,"Bone",pose_move->flash.bone_idx,FALSE);
	list_palette_add_int(&alt_property_palette,kAnimationPoseMovePropertyFlashIntensity,"Intensity",&pose_move->flash.intensity,FALSE);
	list_palette_add_int(&alt_property_palette,kAnimationPoseMovePropertyFlashFlash,"Flash Milliseconds",&pose_move->flash.flash_msec,FALSE);
	list_palette_add_int(&alt_property_palette,kAnimationPoseMovePropertyFlashFade,"Fade Milliseconds",&pose_move->flash.fade_msec,FALSE);
	list_palette_add_float(&alt_property_palette,kAnimationPoseMovePropertyFlashExponent,"Exponent",&pose_move->flash.exponent,FALSE);
	list_palette_add_pick_color(&alt_property_palette,kAnimationPoseMovePropertyFlashColor,"Color",&pose_move->flash.col,FALSE);

		// shake

	list_palette_add_header(&alt_property_palette,0,"Animate Pose Shake");
	list_palette_add_int(&alt_property_palette,kAnimationPoseMovePropertyShakeDistance,"Distance",&pose_move->shake.distance,FALSE);
	list_palette_add_int(&alt_property_palette,kAnimationPoseMovePropertyShakeSize,"Size",&pose_move->shake.size,FALSE);
	list_palette_add_int(&alt_property_palette,kAnimationPoseMovePropertyShakeLife,"Life Milliseconds",&pose_move->shake.life_msec,FALSE);

		// particles

	list_palette_add_header_button(&alt_property_palette,kAnimationPoseMovePropertyParticleAdd,"Animate Pose Particles",list_button_plus);

	for (n=0;n!=pose_move->particle.count;n++) {
		list_palette_add_string_selectable_button(&alt_property_palette,(kAnimationPoseMovePropertyParticle+n),list_button_minus,(kAnimationPoseMovePropertyParticleDelete+n),pose_move->particle.particles[n].name,NULL,((state.cur_animate_idx==animate_idx) && (state.cur_animate_pose_move_idx==pose_move_idx) && (state.cur_animate_pose_move_particle_idx==n)),FALSE);
	}

		// rings

	list_palette_add_header_button(&alt_property_palette,kAnimationPoseMovePropertyRingAdd,"Animate Pose Rings",list_button_plus);

	for (n=0;n!=pose_move->ring.count;n++) {
		list_palette_add_string_selectable_button(&alt_property_palette,(kAnimationPoseMovePropertyRing+n),list_button_minus,(kAnimationPoseMovePropertyRingDelete+n),pose_move->ring.rings[n].name,NULL,((state.cur_animate_idx==animate_idx) && (state.cur_animate_pose_move_idx==pose_move_idx) && (state.cur_animate_pose_move_ring_idx==n)),FALSE);
	}
}

/* =======================================================

      Alt Property Palette Click Animate Pose Move
      
======================================================= */

void alt_property_palette_click_animate_pose_move(int animate_idx,int pose_move_idx,int id,bool double_click)
{
	model_animate_type		*animate;
	model_pose_move_type	*pose_move;

	if (pose_move_idx==-1) return;

	animate=&model.animates[animate_idx];
	pose_move=&animate->pose_moves[pose_move_idx];

		// particles
		
	if ((id>=kAnimationPoseMovePropertyParticle) && (id<(kAnimationPoseMovePropertyParticle+max_model_animate_particle))) {
		state.cur_animate_pose_move_particle_idx=id-kAnimationPoseMovePropertyParticle;
		state.cur_animate_pose_move_ring_idx=-1;
		if (double_click) list_palette_set_level(3);
		return;
	}
	
		// particle delete
		
	if ((id>=kAnimationPoseMovePropertyParticleDelete) && (id<(kAnimationPoseMovePropertyParticleDelete+max_model_animate_particle))) {
		state.cur_animate_pose_move_ring_idx=-1;
		state.cur_animate_pose_move_particle_idx=-1;
		model_piece_delete_animation_pose_move_particle(animate_idx,pose_move_idx,(id-kAnimationPoseMovePropertyParticleDelete));
		return;
	}
	
		// particle add
		
	if (id==kAnimationPoseMovePropertyParticleAdd) {
		state.cur_animate_pose_move_ring_idx=-1;
		state.cur_animate_pose_move_particle_idx=-1;
		model_piece_add_animation_pose_move_particle(animate_idx,pose_move_idx);
		return;
	}

		// rings
		
	if ((id>=kAnimationPoseMovePropertyRing) && (id<(kAnimationPoseMovePropertyRing+max_model_animate_ring))) {
		state.cur_animate_pose_move_particle_idx=-1;
		state.cur_animate_pose_move_ring_idx=id-kAnimationPoseMovePropertyRing;
		if (double_click) list_palette_set_level(3);
		return;
	}
	
		// ring delete
		
	if ((id>=kAnimationPoseMovePropertyRingDelete) && (id<(kAnimationPoseMovePropertyRingDelete+max_model_animate_ring))) {
		state.cur_animate_pose_move_particle_idx=-1;
		state.cur_animate_pose_move_ring_idx=-1;
		model_piece_delete_animation_pose_move_ring(animate_idx,pose_move_idx,(id-kAnimationPoseMovePropertyRingDelete));
		return;
	}
	
		// ring add
		
	if (id==kAnimationPoseMovePropertyRingAdd) {
		state.cur_animate_pose_move_particle_idx=-1;
		state.cur_animate_pose_move_ring_idx=-1;
		model_piece_add_animation_pose_move_ring(animate_idx,pose_move_idx);
		return;
	}

		// loop changes

	if (id==kAnimatePoseMovePropertyLoopStart) {
		model_animate_set_loop_start(&model,animate_idx,pose_move_idx);
		return;
	}

	if (id==kAnimatePoseMovePropertyLoopEnd) {
		model_animate_set_loop_end(&model,animate_idx,pose_move_idx);
		return;
	}

		// double click items

	if (!double_click) return;

	switch (id) {

			// options

		case kAnimatePoseMovePropertyPose:
			property_palette_pick_pose(&pose_move->pose_idx);
			break;

			// sound

		case kAnimationPoseMovePropertySoundBone:
			property_palette_pick_bone(&pose_move->sound.bone_idx,-1);
			break;

		case kAnimationPoseMovePropertySoundName:
			property_palette_pick_sound(pose_move->sound.name,TRUE);
			break;

			// mesh fade

		case kAnimationPoseMovePropertyFadeMesh:
			property_palette_pick_mesh(&pose_move->mesh_fade.mesh_idx);
			break;

			// mesh flash

		case kAnimationPoseMovePropertyFlashBone:
			property_palette_pick_bone(&pose_move->flash.bone_idx,-1);
			break;


	}
}

