/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
 Usage: Private Header

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit �Created with dim3 Technology� is given on a single
application screen and in a single piece of the documentation
3. It is not resold, in it's current form or modified, as an
engine-only product

This code is presented as is. The author of dim3 takes no
responsibilities for any version of this code.

Any non-engine product (games, etc) created with this code is free
from any and all payment and/or royalties to the author of dim3,
and can be sold or given away.

(c) 2000-2010 Klink! Software www.klinksoftware.com
 
*********************************************************************/

//
// Functions
//

extern bool model_read_xml(model_type *model);
extern bool model_write_xml(model_type *model,char *err_str);

extern bool model_read_v2_mesh_xml(model_type *model);
extern bool model_read_v2_pose_xml(model_type *model);
extern bool model_read_v2_animate_xml(model_type *model);

extern void model_textures_clear(model_type *model);
extern void model_textures_read(model_type *model);
extern void model_textures_close(model_type *model);
extern void model_calculate_parents(model_type *model);
