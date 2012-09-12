/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Model Tool Palette

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
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "interface.h"

#define model_tool_count		20

extern map_type					map;
extern app_state_type			state;
extern app_pref_type			pref;
extern file_path_setup_type		file_path_setup;

tool_palette_type				model_tool_palette;

tool_palette_tool_type			model_tool_palette_setup[model_tool_count]=
									{
										{"Model/Tool Textured",FALSE,"Show Textured Model"},
										{"Model/Tool Mesh",FALSE,"Show Model Mesh"},
										{"Model/Tool Bones",FALSE,"Show Model Bones"},
										{"Model/Tool Hit Boxes",FALSE,"Show Model Hit Boxes"},
										{"Model/Tool Edit Mesh",TRUE,"Select Whole Mesh"},
										{"Model/Tool Edit Polygons",FALSE,"Select Polygons"},
										{"Model/Tool Edit Vertexes",FALSE,"Select Vertexes"},
										{"Model/Tool Boxes",TRUE,"Show View Box"},
										{"Model/Tool Normals",FALSE,"Show Normals"},
										{"Model/Tool Show Bone Names",FALSE,"Show Bone Names"},
										{"Model/Tool Select Vertex With Bone",TRUE,"Select Attached Vertexes When Selecting Bones"},
										{"Model/Tool Rotate Mode",TRUE,"Bone Rotate Mode"},
										{"Model/Tool Move Mode",FALSE,"Bone Move Mode"},
										{"",FALSE,""},
										{"Model/Tool Play Stop",FALSE,"Stop Playing Animation"},
										{"Model/Tool Play",TRUE,"Play Animation"},
										{"Model/Tool Play Blend",FALSE,"Play Multiple Blended Animations"},
										{"Model/Tool Play Slow",FALSE,"Play Animation Slowly"},
										{"Model/Tool Play Previous",TRUE,"Previous Pose in Animation"},
										{"Model/Tool Play Next",FALSE,"Next Pose in Animation"},
									};

/* =======================================================

      Model Tool Palette Initialize/Shutdown
      
======================================================= */

void model_tool_palette_initialize(void)
{
	tool_palette_initialize(&model_tool_palette,model_tool_count,model_tool_palette_setup);
}

void model_tool_palette_shutdown(void)
{
	tool_palette_shutdown(&model_tool_palette);
}

/* =======================================================

      Model Tool Palette State
      
======================================================= */

void model_tool_palette_set_state(void)
{
//	tool_palette_set_state(&model_tool_palette,0,TRUE,FALSE);
}

/* =======================================================

      Model Tool Palette Clicking
      
======================================================= */

void model_tool_palette_click(int tool_idx)
{
	switch (tool_idx) {
			
						
	}
	
	main_wind_draw();
}

/* =======================================================

      Project Tool Palette Default State
      
======================================================= */

void model_tool_default(void)
{
}
