/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Property Palette Liquid

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

#include "glue.h"
#include "ui_common.h"
#include "interface.h"

#define kLiquidPropertyLockUV					0
#define kLiquidPropertyNeverObscure				1
#define kLiquidPropertyNeverCull				2
#define kLiquidPropertyNoReflectionMap			3
#define kLiquidPropertyNoDraw					4

#define kLiquidPropertyColor					5
#define kLiquidPropertyTintAlpha				6
#define kLiquidPropertySpeedAlter				7
#define kLiquidPropertySoundName				8

#define kLiquidPropertyTideSize					9
#define kLiquidPropertyTideRate					10
#define kLiquidPropertyTideUVShift				11

#define kLiquidPropertyWaveOn					20
#define kLiquidPropertyWaveDirNorthSouth		21
#define kLiquidPropertyWaveLength				22
#define kLiquidPropertyWaveHigh					23
#define kLiquidPropertyWavePeriodMSec			24
#define kLiquidPropertyWaveReset				25

#define kLiquidPropertyHarm						30
#define kLiquidPropertyDrownTick				31
#define kLiquidPropertyDrownHarm				32

#define kLiquidPropertyReflectTextureSize		40
#define kLiquidPropertyReflectXRefract			41
#define kLiquidPropertyReflectZRefract			42
#define kLiquidPropertyReflectNoHitColor		43
#define kLiquidPropertyReflectAlpha				44

#define kLiquidPropertyOverlayOn				50
#define kLiquidPropertyOverlayTexture			51
#define kLiquidPropertyOverlayStampSize			52
#define kLiquidPropertyOverlayShift				53

#define kLiquidPropertyGroup					60

#define kLiquidPropertyOff						70
#define kLiquidPropertySize						71
#define kLiquidPropertyShift					72
#define kLiquidPropertyCamera					73

extern map_type					map;
extern editor_state_type		state;
extern editor_setup_type		setup;

extern list_palette_type		property_palette;
extern char						map_property_light_map_size_list[][name_str_len];

int								pal_liquid_index;
d3pnt							pal_liquid_pnt,pal_liquid_size;

/* =======================================================

      Property Palette Fill Liquid
      
======================================================= */

void property_palette_fill_liquid(int liq_idx)
{
	char					str[32];
	map_liquid_type			*liq;
	editor_view_type		*view;

		// liquid settings
		
	liq=&map.liquid.liquids[liq_idx];

	sprintf(str,"%d",liq_idx);
	list_palette_set_sub_title(&property_palette,"Liquid",str);

	list_palette_add_header(&property_palette,0,"Liquid Settings");
	list_palette_add_checkbox(&property_palette,kLiquidPropertyLockUV,"Lock UV",&liq->flag.lock_uv,FALSE);
	list_palette_add_checkbox(&property_palette,kLiquidPropertyNeverObscure,"Never Obscure",&liq->flag.never_obscure,FALSE);
	list_palette_add_checkbox(&property_palette,kLiquidPropertyNeverCull,"Never Cull",&liq->flag.never_cull,FALSE);
	list_palette_add_checkbox(&property_palette,kLiquidPropertyNoDraw,"No Draw (Volume Only)",&liq->flag.no_draw,FALSE);

	list_palette_add_header(&property_palette,0,"Liquid Under");
	list_palette_add_pick_color(&property_palette,kLiquidPropertyColor,"Color",&liq->col,FALSE);
	list_palette_add_float(&property_palette,kLiquidPropertyTintAlpha,"Tint Alpha",&liq->tint_alpha,FALSE);
	list_palette_add_float(&property_palette,kLiquidPropertySpeedAlter,"Speed Alter",&liq->speed_alter,FALSE);
	list_palette_add_string(&property_palette,kLiquidPropertySoundName,"Sound",liq->ambient.sound_name,FALSE);

	list_palette_add_header(&property_palette,0,"Liquid Tides");
	list_palette_add_int(&property_palette,kLiquidPropertyTideSize,"Size",&liq->tide.high,FALSE);
	list_palette_add_int(&property_palette,kLiquidPropertyTideRate,"Rate",&liq->tide.rate,FALSE);
	list_palette_add_float(&property_palette,kLiquidPropertyTideUVShift,"UV Shift",&liq->tide.uv_shift,FALSE);

	list_palette_add_header(&property_palette,0,"Liquid Waves");
	list_palette_add_checkbox(&property_palette,kLiquidPropertyWaveOn,"On",&liq->wave.on,FALSE);
	list_palette_add_checkbox(&property_palette,kLiquidPropertyWaveDirNorthSouth,"North-South Dir",&liq->wave.dir_north_south,FALSE);
	list_palette_add_int(&property_palette,kLiquidPropertyWaveLength,"Length",&liq->wave.length,FALSE);
	list_palette_add_int(&property_palette,kLiquidPropertyWaveHigh,"High",&liq->wave.high,FALSE);
	list_palette_add_int(&property_palette,kLiquidPropertyWavePeriodMSec,"Period msec",&liq->wave.period_msec,FALSE);
	list_palette_add_string_selectable_button(&property_palette,kLiquidPropertyWaveReset,list_button_set,kLiquidPropertyWaveReset,"Resize Liquid To Fit Wave Length",NULL,FALSE,FALSE);
	
	list_palette_add_header(&property_palette,0,"Liquid Harm");
	list_palette_add_int(&property_palette,kLiquidPropertyHarm,"In Damage",&liq->harm.in_harm,FALSE);
	list_palette_add_int(&property_palette,kLiquidPropertyDrownTick,"Drowning Tick",&liq->harm.drown_tick,FALSE);
	list_palette_add_int(&property_palette,kLiquidPropertyDrownHarm,"Drowning Damage",&liq->harm.drown_harm,FALSE);

	list_palette_add_header(&property_palette,0,"Liquid Reflection");
	list_palette_add_string(&property_palette,kLiquidPropertyReflectTextureSize,"Texture Size",map_property_light_map_size_list[liq->reflect.texture_size],FALSE);
	list_palette_add_int(&property_palette,kLiquidPropertyReflectXRefract,"X Refraction Factor",&liq->reflect.x_refract_factor,FALSE);
	list_palette_add_int(&property_palette,kLiquidPropertyReflectZRefract,"Z Refraction Factor",&liq->reflect.z_refract_factor,FALSE);
	list_palette_add_pick_color(&property_palette,kLiquidPropertyReflectNoHitColor,"No Hit Color",&liq->reflect.no_hit_col,FALSE);
	list_palette_add_float(&property_palette,kLiquidPropertyReflectAlpha,"Alpha",&liq->reflect.alpha,FALSE);

	list_palette_add_header(&property_palette,0,"Liquid Overlay");
	list_palette_add_checkbox(&property_palette,kLiquidPropertyOverlayOn,"On",&liq->overlay.on,FALSE);
	list_palette_add_texture(&property_palette,map.textures,kLiquidPropertyOverlayTexture,"Texture",liq->overlay.txt_idx,FALSE);
	list_palette_add_int(&property_palette,kLiquidPropertyOverlayStampSize,"Stamp Size",&liq->overlay.stamp_size,FALSE);
	list_palette_add_uv(&property_palette,kLiquidPropertyOverlayShift,"Shift",&liq->overlay.x_shift,&liq->overlay.y_shift,FALSE);

	list_palette_add_header(&property_palette,0,"Liquid Group");
	if (liq->group_idx==-1) {
		list_palette_add_string(&property_palette,kLiquidPropertyGroup,"Group","",FALSE);
	}
	else {
		list_palette_add_string(&property_palette,kLiquidPropertyGroup,"Group",map.group.groups[liq->group_idx].name,FALSE);
	}

		// polygon like settings
		
	view=view_get_current_view();
	
	list_palette_add_header(&property_palette,0,"Liquid UVs");

	if (view->uv_layer==uv_layer_normal) {
		list_palette_add_uv(&property_palette,kLiquidPropertyOff,"Offset",&liq->main_uv.x_offset,&liq->main_uv.y_offset,FALSE);
		list_palette_add_uv(&property_palette,kLiquidPropertySize,"Size",&liq->main_uv.x_size,&liq->main_uv.y_size,FALSE);
	}
	else {
		list_palette_add_uv(&property_palette,kLiquidPropertyOff,"Offset",&liq->lmap_uv.x_offset,&liq->lmap_uv.y_offset,FALSE);
		list_palette_add_uv(&property_palette,kLiquidPropertySize,"Size",&liq->lmap_uv.x_size,&liq->lmap_uv.y_size,FALSE);
	}
	
	list_palette_add_uv(&property_palette,kLiquidPropertyShift,"Shift",&liq->x_shift,&liq->y_shift,FALSE);

	list_palette_add_header(&property_palette,0,"Liquid Camera");
	list_palette_add_string(&property_palette,kLiquidPropertyCamera,"Node",liq->camera,FALSE);
	
		// info
	
	pal_liquid_index=liq_idx;
		
	pal_liquid_pnt.x=liq->lft;
	pal_liquid_pnt.y=liq->y;
	pal_liquid_pnt.z=liq->top;
	
	pal_liquid_size.x=liq->rgt-liq->lft;
	pal_liquid_size.y=liq->depth;
	pal_liquid_size.z=liq->bot-liq->top;
		
	list_palette_add_header(&property_palette,0,"Liquid Info");
	list_palette_add_int(&property_palette,-1,"Index",&pal_liquid_index,FALSE);
	list_palette_add_point(&property_palette,-1,"Position",&pal_liquid_pnt,TRUE);
	list_palette_add_point(&property_palette,-1,"Size",&pal_liquid_size,TRUE);
}

/* =======================================================

      Property Palette Click Liquid
      
======================================================= */

void property_palette_click_liquid(int liq_idx,int id,bool double_click)
{
	map_liquid_type			*liq;
	editor_view_type		*view;

	liq=&map.liquid.liquids[liq_idx];

		// wave reset

	if (id==kLiquidPropertyWaveReset) {
		piece_liquid_reset_size(liq);
		return;
	}

		// regular clicks

	if (!double_click) return;

	view=view_get_current_view();

	switch (id) {
	
			// options

		case kLiquidPropertySoundName:
			property_palette_pick_sound(liq->ambient.sound_name,TRUE);
			break;

			// reflect
			
		case kLiquidPropertyReflectTextureSize:
			property_pick_list("Pick a Liquid Reflect Map Size",(char*)map_property_light_map_size_list,&liq->reflect.texture_size);
			break;

			// overlay
			
		case kLiquidPropertyOverlayTexture:
			property_palette_pick_texture(NULL,&liq->overlay.txt_idx);
			break;

			// group
			
		case kLiquidPropertyGroup:
			property_palette_pick_group(&liq->group_idx);
			break;
			
		case kLiquidPropertyCamera:
			property_palette_pick_node(liq->camera);
			break;

	}
}

