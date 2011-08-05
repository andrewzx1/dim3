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

/* =======================================================

      Property Palette Fill Liquid
      
======================================================= */

void property_palette_fill_liquid(int liq_idx)
{
	char					str[32];
	d3pnt					pnt,size;
	d3fpnt					uv_offset,uv_size,uv_shift;
	map_liquid_type			*liq;
	editor_view_type		*view;

		// liquid settings
		
	liq=&map.liquid.liquids[liq_idx];

	sprintf(str,"%d",liq_idx);
	list_palette_set_sub_title(&property_palette,"Liquid",str);

	list_palette_add_header(&property_palette,0,"Liquid Settings");
	list_palette_add_checkbox(&property_palette,kLiquidPropertyLockUV,"Lock UV",liq->flag.lock_uv,FALSE);
	list_palette_add_checkbox(&property_palette,kLiquidPropertyNeverObscure,"Never Obscure",liq->flag.never_obscure,FALSE);
	list_palette_add_checkbox(&property_palette,kLiquidPropertyNeverCull,"Never Cull",liq->flag.never_cull,FALSE);
	list_palette_add_checkbox(&property_palette,kLiquidPropertyNoDraw,"No Draw (Volume Only)",liq->flag.no_draw,FALSE);

	list_palette_add_header(&property_palette,0,"Liquid Under");
	list_palette_add_pick_color(&property_palette,kLiquidPropertyColor,"Color",&liq->col,FALSE);
	list_palette_add_string_float(&property_palette,kLiquidPropertyTintAlpha,"Tint Alpha",liq->tint_alpha,FALSE);
	list_palette_add_string_float(&property_palette,kLiquidPropertySpeedAlter,"Speed Alter",liq->speed_alter,FALSE);
	list_palette_add_string(&property_palette,kLiquidPropertySoundName,"Sound",liq->ambient.sound_name,FALSE);

	list_palette_add_header(&property_palette,0,"Liquid Tides");
	list_palette_add_string_int(&property_palette,kLiquidPropertyTideSize,"Size",liq->tide.high,FALSE);
	list_palette_add_string_int(&property_palette,kLiquidPropertyTideRate,"Rate",liq->tide.rate,FALSE);
	list_palette_add_string_float(&property_palette,kLiquidPropertyTideUVShift,"UV Shift",liq->tide.uv_shift,FALSE);

	list_palette_add_header(&property_palette,0,"Liquid Waves");
	list_palette_add_checkbox(&property_palette,kLiquidPropertyWaveOn,"On",liq->wave.on,FALSE);
	list_palette_add_checkbox(&property_palette,kLiquidPropertyWaveDirNorthSouth,"North-South Dir",liq->wave.dir_north_south,FALSE);
	list_palette_add_string_int(&property_palette,kLiquidPropertyWaveLength,"Length",liq->wave.length,FALSE);
	list_palette_add_string_int(&property_palette,kLiquidPropertyWaveHigh,"High",liq->wave.high,FALSE);
	list_palette_add_string_int(&property_palette,kLiquidPropertyWavePeriodMSec,"Period msec",liq->wave.period_msec,FALSE);
	
	list_palette_add_header(&property_palette,0,"Liquid Harm");
	list_palette_add_string_int(&property_palette,kLiquidPropertyHarm,"In Damage",liq->harm.in_harm,FALSE);
	list_palette_add_string_int(&property_palette,kLiquidPropertyDrownTick,"Drowning Tick",liq->harm.drown_tick,FALSE);
	list_palette_add_string_int(&property_palette,kLiquidPropertyDrownHarm,"Drowning Damage",liq->harm.drown_harm,FALSE);

	list_palette_add_header(&property_palette,0,"Liquid Reflection");
	list_palette_add_string_int(&property_palette,kLiquidPropertyReflectTextureSize,"Texture Size",liq->reflect.texture_size,FALSE);
	list_palette_add_string_int(&property_palette,kLiquidPropertyReflectXRefract,"X Refraction Factor",liq->reflect.x_refract_factor,FALSE);
	list_palette_add_string_int(&property_palette,kLiquidPropertyReflectZRefract,"Z Refraction Factor",liq->reflect.z_refract_factor,FALSE);
	list_palette_add_pick_color(&property_palette,kLiquidPropertyReflectNoHitColor,"No Hit Color",&liq->reflect.no_hit_col,FALSE);
	list_palette_add_string_float(&property_palette,kLiquidPropertyReflectAlpha,"Alpha",liq->reflect.alpha,FALSE);

	list_palette_add_header(&property_palette,0,"Liquid Overlay");
	list_palette_add_checkbox(&property_palette,kLiquidPropertyOverlayOn,"On",liq->overlay.on,FALSE);
	list_palette_add_texture(&property_palette,map.textures,kLiquidPropertyOverlayTexture,"Texture",liq->overlay.txt_idx,FALSE);
	list_palette_add_string_int(&property_palette,kLiquidPropertyOverlayStampSize,"Stamp Size",liq->overlay.stamp_size,FALSE);

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
		uv_offset.x=liq->main_uv.x_offset;
		uv_offset.y=liq->main_uv.y_offset;
		uv_size.x=liq->main_uv.x_size;
		uv_size.y=liq->main_uv.y_size;
	}
	else {
		uv_offset.x=liq->lmap_uv.x_offset;
		uv_offset.y=liq->lmap_uv.y_offset;
		uv_size.x=liq->lmap_uv.x_size;
		uv_size.y=liq->lmap_uv.y_size;
	}
	
	uv_shift.x=liq->x_shift;
	uv_shift.y=liq->y_shift;

	list_palette_add_uv(&property_palette,kLiquidPropertyOff,"Offset",&uv_offset,FALSE);
	list_palette_add_uv(&property_palette,kLiquidPropertySize,"Size",&uv_size,FALSE);
	list_palette_add_uv(&property_palette,kLiquidPropertyShift,"Shift",&uv_shift,FALSE);

	list_palette_add_header(&property_palette,0,"Liquid Camera");
	list_palette_add_string(&property_palette,kLiquidPropertyCamera,"Node",liq->camera,FALSE);
	
		// info
		
	pnt.x=liq->lft;
	pnt.y=liq->y;
	pnt.z=liq->top;
	
	size.x=liq->rgt-liq->lft;
	size.y=liq->depth;
	size.z=liq->bot-liq->top;
		
	list_palette_add_header(&property_palette,0,"Liquid Info");
	list_palette_add_string_int(&property_palette,-1,"Index",liq_idx,FALSE);
	list_palette_add_point(&property_palette,-1,"Position",&pnt,TRUE);
	list_palette_add_point(&property_palette,-1,"Size",&size,TRUE);
}

/* =======================================================

      Property Palette Click Liquid
      
======================================================= */

void property_palette_click_liquid(int liq_idx,int id,bool double_click)
{
	int						size;
	d3fpnt					uv;
	map_liquid_type			*liq;
	editor_view_type		*view;

	if (!double_click) return;

	liq=&map.liquid.liquids[liq_idx];
	view=view_get_current_view();

	switch (id) {
	
			// flags

		case kLiquidPropertyLockUV:
			liq->flag.lock_uv=!liq->flag.lock_uv;
			break;

		case kLiquidPropertyNeverObscure:
			liq->flag.never_obscure=!liq->flag.never_obscure;
			break;

		case kLiquidPropertyNeverCull:
			liq->flag.never_cull=!liq->flag.never_cull;
			break;
			
		case kLiquidPropertyNoDraw:
			liq->flag.no_draw=!liq->flag.no_draw;
			break;
			
			// options

		case kLiquidPropertyColor:
			os_pick_color(&liq->col);
			break;
			
		case kLiquidPropertyTintAlpha:
			dialog_property_string_run(list_string_value_0_to_1_float,(void*)&liq->tint_alpha,0,0,0);
			break;
			
		case kLiquidPropertySpeedAlter:
			dialog_property_string_run(list_string_value_positive_float,(void*)&liq->speed_alter,0,0,0);
			break;

		case kLiquidPropertySoundName:
			property_palette_pick_sound(liq->ambient.sound_name,TRUE);
			break;

			// tides
			
		case kLiquidPropertyTideSize:
			dialog_property_string_run(list_string_value_positive_int,(void*)&liq->tide.high,0,0,0);
			break;
			
		case kLiquidPropertyTideRate:
			dialog_property_string_run(list_string_value_positive_int,(void*)&liq->tide.rate,0,0,0);
			break;

		case kLiquidPropertyTideUVShift:
			dialog_property_string_run(list_string_value_positive_float,(void*)&liq->tide.uv_shift,0,0,0);
			break;

			// waves

		case kLiquidPropertyWaveOn:
			liq->wave.on=!liq->wave.on;
			break;

		case kLiquidPropertyWaveDirNorthSouth:
			liq->wave.dir_north_south=!liq->wave.dir_north_south;
			break;

		case kLiquidPropertyWaveLength:
			dialog_property_string_run(list_string_value_positive_int,(void*)&liq->wave.length,0,0,0);
			break;

		case kLiquidPropertyWaveHigh:
			dialog_property_string_run(list_string_value_positive_int,(void*)&liq->wave.high,0,0,0);
			break;

		case kLiquidPropertyWavePeriodMSec:
			dialog_property_string_run(list_string_value_positive_int,(void*)&liq->wave.period_msec,0,0,0);
			break;

			// harm
			
		case kLiquidPropertyHarm:
			dialog_property_string_run(list_string_value_int,(void*)&liq->harm.in_harm,0,0,0);
			break;
			
		case kLiquidPropertyDrownTick:
			dialog_property_string_run(list_string_value_positive_int,(void*)&liq->harm.drown_tick,0,0,0);
			break;
			
		case kLiquidPropertyDrownHarm:
			dialog_property_string_run(list_string_value_int,(void*)&liq->harm.drown_harm,0,0,0);
			break;
			
			// reflect
			
		case kLiquidPropertyReflectTextureSize:
			size=((int)log2(liq->reflect.texture_size))-8;
			if ((size<0) || (size>2)) size=0;
			property_pick_list("Pick a Liquid Reflect Map Size",(char*)map_property_light_map_size_list,&size);
			liq->reflect.texture_size=(int)pow(2,(size+8));
			break;
			break;
			
		case kLiquidPropertyReflectXRefract:
			dialog_property_string_run(list_string_value_positive_int,(void*)&liq->reflect.x_refract_factor,0,0,0);
			break;
			
		case kLiquidPropertyReflectZRefract:
			dialog_property_string_run(list_string_value_positive_int,(void*)&liq->reflect.z_refract_factor,0,0,0);
			break;

		case kLiquidPropertyReflectNoHitColor:
			os_pick_color(&liq->reflect.no_hit_col);
			break;

		case kLiquidPropertyReflectAlpha:
			dialog_property_string_run(list_string_value_0_to_1_float,(void*)&liq->reflect.alpha,0,0,0);
			break;

			// overlay
			
		case kLiquidPropertyOverlayOn:
			liq->overlay.on=!liq->overlay.on;
			break;
			
		case kLiquidPropertyOverlayTexture:
			property_palette_pick_texture(NULL,&liq->overlay.txt_idx);
			break;
			
		case kLiquidPropertyOverlayStampSize:
			dialog_property_string_run(list_string_value_positive_int,(void*)&liq->overlay.stamp_size,0,0,0);
			break;

			// group
			
		case kLiquidPropertyGroup:
			property_palette_pick_group(&liq->group_idx);
			break;
			
		case kLiquidPropertyOff:
			if (view->uv_layer==uv_layer_normal) {
				uv.x=liq->main_uv.x_offset;
				uv.y=liq->main_uv.y_offset;
			}
			else {
				uv.x=liq->lmap_uv.x_offset;
				uv.y=liq->lmap_uv.y_offset;
			}
			dialog_property_chord_run(list_chord_value_uv,(void*)&uv);
			if (view->uv_layer==uv_layer_normal) {
				liq->main_uv.x_offset=uv.x;
				liq->main_uv.y_offset=uv.y;
			}
			else {
				liq->lmap_uv.x_offset=uv.x;
				liq->lmap_uv.y_offset=uv.y;
			}
			break;
			
		case kLiquidPropertySize:
			if (view->uv_layer==uv_layer_normal) {
				uv.x=liq->main_uv.x_size;
				uv.y=liq->main_uv.y_size;
			}
			else {
				uv.x=liq->lmap_uv.x_size;
				uv.y=liq->lmap_uv.y_size;
			}
			dialog_property_chord_run(list_chord_value_uv,(void*)&uv);
			if (view->uv_layer==uv_layer_normal) {
				liq->main_uv.x_size=uv.x;
				liq->main_uv.y_size=uv.y;
			}
			else {
				liq->lmap_uv.x_size=uv.x;
				liq->lmap_uv.y_size=uv.y;
			}
			break;
			
		case kLiquidPropertyShift:
			uv.x=liq->x_shift;
			uv.y=liq->y_shift;
			dialog_property_chord_run(list_chord_value_uv,(void*)&uv);
			liq->x_shift=uv.x;
			liq->y_shift=uv.y;
			break;

		case kLiquidPropertyCamera:
			property_palette_pick_node(liq->camera);
			break;

	}

	main_wind_draw();
}

