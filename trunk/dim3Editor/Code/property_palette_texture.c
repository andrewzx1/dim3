/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Property Palette Texture

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

#define kTexturePropertyAnimate				0
#define kTexturePropertyAlphaAdditive		1
#define kTexturePropertyPixelated			2
#define kTexturePropertyCompress			3

#define kTexturePropertyShader				5
#define kTexturePropertyGlowRate			6
#define kTexturePropertyGlowMin				7
#define kTexturePropertyGlowMax				8
#define kTexturePropertyShineFactor			9
#define kTexturePropertyMaterialName		10
#define kTexturePropertyScaleOn				11
#define kTexturePropertyScaleLockOffset		12
#define kTexturePropertyScale				13

#define kTexturePropertyFrameWaitStart		20
#define kTexturePropertyFrameWaitEnd		(20+(max_texture_frame-1))

extern map_type					map;
extern editor_state_type		state;
extern editor_setup_type		setup;

extern list_palette_type		property_palette;

/* =======================================================

      Property Palette Fill Texture
      
======================================================= */

void property_palette_fill_texture(int texture_idx)
{
	int					n;
	char				name[32];
	d3fpnt				uv;
	texture_type		*texture;

	texture=&map.textures[texture_idx];

	list_palette_set_sub_title(&property_palette,"Texture",texture->frames[0].name);

	list_palette_add_header(&property_palette,0,"Texture Settings");
	list_palette_add_checkbox(&property_palette,kTexturePropertyAnimate,"Animated",&texture->animate.on,FALSE);
	list_palette_add_checkbox(&property_palette,kTexturePropertyAlphaAdditive,"Alpha Additive",&texture->additive,FALSE);
	list_palette_add_checkbox(&property_palette,kTexturePropertyPixelated,"Pixelated",&texture->pixelated,FALSE);
	list_palette_add_checkbox(&property_palette,kTexturePropertyCompress,"Compressed",&texture->compress,FALSE);

	list_palette_add_header(&property_palette,0,"Texture Options");
	list_palette_add_shader(&property_palette,kTexturePropertyShader,"Shader",texture->shader_name,FALSE);
	list_palette_add_string_int(&property_palette,kTexturePropertyGlowRate,"Glow Rate",texture->glow.rate,FALSE);
	list_palette_add_string_float(&property_palette,kTexturePropertyGlowMin,"Glow Min",texture->glow.min,FALSE);
	list_palette_add_string_float(&property_palette,kTexturePropertyGlowMax,"Glow Max",texture->glow.max,FALSE);
	list_palette_add_string_float(&property_palette,kTexturePropertyShineFactor,"Shine Factor",texture->shine_factor,FALSE);
	list_palette_add_string(&property_palette,kTexturePropertyMaterialName,"Material Name",texture->material_name,FALSE);

	list_palette_add_header(&property_palette,0,"Texture Frame Waits");
	for (n=0;n!=max_texture_frame;n++) {
		sprintf(name,"Frame %d Wait Time",n);
		list_palette_add_string_int(&property_palette,(kTexturePropertyFrameWaitStart+n),name,texture->animate.wait[n],FALSE);
	}

	list_palette_add_header(&property_palette,0,"Texture Auto UVs");
	list_palette_add_checkbox(&property_palette,kTexturePropertyScaleOn,"Use Texture Scale",&texture->scale.on,FALSE);
	list_palette_add_checkbox(&property_palette,kTexturePropertyScaleLockOffset,"Lock Offsets",&texture->scale.lock_offset,FALSE);
	uv.x=texture->scale.x;
	uv.y=texture->scale.y;
	list_palette_add_uv(&property_palette,kTexturePropertyScale,"Scale",&uv,FALSE);
}

/* =======================================================

      Property Palette Click Texture
      
======================================================= */

void property_palette_click_texture(int texture_idx,int id,bool double_click)
{
	d3fpnt				uv;
	texture_type		*texture;

	if (!double_click) return;

	texture=&map.textures[texture_idx];

		// waits

	if ((id>=kTexturePropertyFrameWaitStart) && (id<=kTexturePropertyFrameWaitEnd)) {
		dialog_property_string_run(list_string_value_positive_int,(void*)&texture->animate.wait[id-kTexturePropertyFrameWaitStart],0,0,0);
		return;
	}

		// other properties

	switch (id) {

		case kTexturePropertyShader:
			property_palette_pick_shader(texture->shader_name);
			break;

		case kTexturePropertyGlowRate:
			dialog_property_string_run(list_string_value_positive_int,(void*)&texture->glow.rate,0,0,0);
			break;

		case kTexturePropertyGlowMin:
			dialog_property_string_run(list_string_value_0_to_1_float,(void*)&texture->glow.min,0,0,0);
			break;

		case kTexturePropertyGlowMax:
			dialog_property_string_run(list_string_value_0_to_1_float,(void*)&texture->glow.max,0,0,0);
			break;

		case kTexturePropertyShineFactor:
			dialog_property_string_run(list_string_value_float,(void*)&texture->shine_factor,0,0,0);
			break;

		case kTexturePropertyMaterialName:
			dialog_property_string_run(list_string_value_string,(void*)texture->material_name,name_str_len,0,0);
			break;

		case kTexturePropertyScale:
			uv.x=texture->scale.x;
			uv.y=texture->scale.y;
			dialog_property_chord_run(list_chord_value_uv,(void*)&uv);
			texture->scale.x=uv.x;
			texture->scale.y=uv.y;
			break;

	}
}

