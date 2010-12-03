/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Property Palette Particle

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

#define kParticleOn								0
#define kParticlePropertyName					1
#define kParticlePropertySpawnTick				2
#define kParticlePropertySlopTick				3

extern map_type					map;
extern editor_state_type		state;
extern editor_setup_type		setup;

extern list_palette_type		property_palette;

/* =======================================================

      Property Palette Fill Particle
      
======================================================= */

void property_palette_fill_particle(int particle_idx)
{
	map_particle_type		*particle;

	particle=&map.particles[particle_idx];

	list_palette_add_header(&property_palette,0,"Particle Settings");
	list_palette_add_checkbox(&property_palette,kParticleOn,"On",particle->on);

	list_palette_add_header(&property_palette,0,"Particle Type");
	list_palette_add_string(&property_palette,kParticlePropertyName,"Name",particle->name);
	list_palette_add_string_int(&property_palette,kParticlePropertySpawnTick,"Spawn Tick",particle->spawn_tick);
	list_palette_add_string_int(&property_palette,kParticlePropertySlopTick,"Slop Tick",particle->slop_tick);
}

/* =======================================================

      Property Palette Click Particle
      
======================================================= */

void property_palette_click_particle(int particle_idx,int id)
{
	map_particle_type		*particle;

	particle=&map.particles[particle_idx];

	switch (id) {

		case kParticleOn:
			particle->on=!particle->on;
			break;

		case kParticlePropertyName:
			property_palette_pick_particle(particle->name);
			break;

		case kParticlePropertySpawnTick:
		case kParticlePropertySlopTick:
			break;

	}

	main_wind_draw();
}

