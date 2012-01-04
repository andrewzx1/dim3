/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Alt Property Palette Shader

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
	#include "dim3setup.h"
#endif

#include "glue.h"
#include "ui_common.h"
#include "interface.h"

#define kShaderSettingsName						0
#define kShaderCodeVertName						1
#define kShaderCodeFragName						2

extern iface_type				iface;
extern setup_state_type			state;
extern list_palette_type		alt_property_palette;

/* =======================================================

      Property Palette Fill Shader
      
======================================================= */

void alt_property_palette_fill_shader(int shader_idx)
{
	iface_shader_type			*shader;

	shader=&iface.shader_list.shaders[shader_idx];

	list_palette_set_title(&alt_property_palette,"Shaders",NULL,"Shader",shader->name,NULL,NULL);

		// settings

	list_palette_add_header(&alt_property_palette,0,"Settings");
	list_palette_add_string(&alt_property_palette,kShaderSettingsName,"Name",shader->name,FALSE);

		// code

	list_palette_add_header(&alt_property_palette,0,"Code");
	list_palette_add_string(&alt_property_palette,kShaderCodeVertName,"Vertex",shader->vert_name,FALSE);
	list_palette_add_string(&alt_property_palette,kShaderCodeFragName,"Fragment",shader->frag_name,FALSE);
}

/* =======================================================

      Property Palette Click Shader
      
======================================================= */

void alt_property_palette_click_shader(int shader_idx,int id,bool double_click)
{
	iface_shader_type			*shader;

	if (!double_click) return;

	shader=&iface.shader_list.shaders[shader_idx];

	switch (id) {

			// settings

		case kShaderSettingsName:
			dialog_property_string_run(list_string_value_string,(void*)shader->name,name_str_len,0,0);
			break;

			// code

		case kShaderCodeVertName:
			property_pick_file("Pick a Vertex Shader","Shaders","vert",NULL,shader->vert_name);
			break;

		case kShaderCodeFragName:
			property_pick_file("Pick a Fragment Shader","Shaders","frag",NULL,shader->frag_name);
			break;

	}
}

