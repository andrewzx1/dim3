/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Property Palette Spot

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
#include "interface.h"
#include "view.h"
#include "dialog.h"

#define kSpotPropertyName						0
#define kSpotPropertyType						1
#define kSpotPropertyScript						2
#define kSpotPropertySkill						3
#define kSpotPropertySpawn						4
#define kSpotPropertyDisplayModel				5

#define kSpotPropertyParamsStart				10
#define kSpotPropertyParamsEnd					19

extern map_type					map;
extern editor_state_type		state;
extern editor_setup_type		setup;

extern list_palette_type		property_palette;

char							spot_property_type_list[][name_str_len]={"Object","Bot","Player","Spawn",""},
								spot_property_skill_list[][name_str_len]={"Easy","Medium","Hard",""},
								spot_property_spawn_list[][name_str_len]={"Always","Single Player Only","Multiplayer Only",""};

/* =======================================================

      Property Palette Fill Spot Parameters
      
======================================================= */

void palette_palette_spot_get_parameter(int idx,char *param_list,char *str)
{
	int				n;
	char			*c;
	
	c=param_list;
	
	for (n=0;n!=idx;n++) {
		c=strchr(c,'|');
		if (c==NULL) return;
	}

	strncpy(str,c,256);
	str[255]=0x0;

	c=strchr(str,'|');
	if (c!=NULL) *c=0x0;
}

void palette_palette_spot_set_parameter(int idx,char *param_list,char *str)
{
	int				n,count;
	char			*c,*c2,tstr[10][256];
	
		// clear param list

	for (n=0;n!=10;n++) {
		tstr[n][0]=0x0;
	}

		// break up param list

	c=str;
	
	for (n=0;n!=10;n++) {
		if (c==0x0) break;
		
		c2=strchr(c,'|');
		if (c2==NULL) {
			strcpy(tstr[n],c);
			break;
		}
		
		strcpy(tstr[n],c);
		c=strchr(tstr[n],'|');
		*c=0x0;
		
		c=c2+1;
	}

		// fix the list

	strncpy(tstr[idx],str,256);
	tstr[idx][255]=0x0;

		// find last item

	count=0;

	for (n=9;n>=0;n++) {
		if (tstr[n][0]!=0x0) count=n+1;
	}

		// rebuild the list

	param_list[0]=0x0;

	for (n=0;n!=count;n++) {
		if (n!=0) strncpy(param_list,"|",param_str_len);
		strncpy(param_list,tstr[idx],param_str_len);
	}

	param_list[param_str_len-1]=0x0;
}

/* =======================================================

      Property Palette Fill Spot
      
======================================================= */

void property_palette_fill_spot(int spot_idx)
{
	int				n;
	char			name[256],str[256];
	spot_type		*spot;

	spot=&map.spots[spot_idx];

	list_palette_add_header(&property_palette,0,"Spot Settings");
	list_palette_add_string(&property_palette,kSpotPropertyName,"Name",spot->name);
	list_palette_add_string(&property_palette,kSpotPropertyType,"Type",spot_property_type_list[spot->type]);
	list_palette_add_string(&property_palette,kSpotPropertyScript,"Script",spot->script);
	list_palette_add_string(&property_palette,kSpotPropertySkill,"Skill",spot_property_skill_list[spot->skill]);
	list_palette_add_string(&property_palette,kSpotPropertySpawn,"Spawn",spot_property_spawn_list[spot->spawn]);
	list_palette_add_string(&property_palette,kSpotPropertyDisplayModel,"Model",spot->display_model);

	list_palette_add_header(&property_palette,0,"Spot Parameters");
	for (n=0;n!=10;n++) {
		sprintf(name,"%d",n);
		palette_palette_spot_get_parameter(n,spot->params,str);
		list_palette_add_string(&property_palette,(kSpotPropertyParamsStart+n),name,str);
	}
}

/* =======================================================

      Property Palette Click Spot
      
======================================================= */

void property_palette_click_spot(int spot_idx,int id)
{
	spot_type		*spot;

	spot=&map.spots[spot_idx];

		// parameters

	if ((id>=kSpotPropertyParamsStart) && (id<=kSpotPropertyParamsEnd)) {

		main_wind_draw();
		return;
	}

	switch (id) {

		case kSpotPropertyName:
		case kSpotPropertyType:
		case kSpotPropertyScript:
		case kSpotPropertySkill:
		case kSpotPropertySpawn:
		case kSpotPropertyDisplayModel:
			break;

	}

	main_wind_draw();
}

