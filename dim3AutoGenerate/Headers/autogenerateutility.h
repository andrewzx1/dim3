/****************************** File *********************************

Module: dim3 Auto Generator
Author: Brian Barnes
 Usage: Auto Generator Utility Defines

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

(c) 2000-2014 Klink! Software www.klinksoftware.com
 
*********************************************************************/

//
// auto-generator random numbers
//

extern void ag_random_seed(void);
extern void ag_random_previous_seed(void);
extern void ag_random_next_seed(void);
extern int ag_random_int(int max);
extern bool ag_random_bool(void);

//
// map generation
//

extern bool auto_generate_map(char *err_str);
extern bool auto_generate_previous_map(char *err_str);
extern bool auto_generate_next_map(char *err_str);
extern int auto_generate_get_seed(void);
extern void ag_map_create_texture_set(char *base_path);
