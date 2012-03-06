/****************************** File *********************************

Module: dim3 Setup
Author: Brian Barnes
 Usage: Property Palette Particle Group

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

#define kGroupParticleSettingsName				0

#define kGroupParticlePropertyAdd				10

#define kGroupParticleProperyName				1000
#define kGroupParticleProperyDelete				2000

extern iface_type				iface;
extern setup_state_type			state;
extern list_palette_type		property_palette;

/* =======================================================

      Property Palette Fill Particle Group
      
======================================================= */

void property_palette_fill_particle_group(int particle_idx)
{
	int						n;
	iface_particle_type		*particle;

	particle=&iface.particle_list.particles[particle_idx];

	list_palette_set_title(&property_palette,"Particles",NULL,"Particle Group",particle->name,NULL,NULL);

	list_palette_add_header(&property_palette,0,"Settings");
	list_palette_add_string(&property_palette,kGroupParticleSettingsName,"Name",particle->name,FALSE);

		// particles list

	list_palette_add_header_button(&property_palette,kGroupParticlePropertyAdd,"Particles",list_button_plus);

	list_palette_sort_mark_start(&property_palette);
	
	for (n=0;n!=particle->group.count;n++) {
		list_palette_add_string_selectable_button(&property_palette,(kGroupParticleProperyName+n),list_button_minus,(kGroupParticleProperyDelete+n),particle->group.particles[n].name,NULL,(state.cur_group_particle_idx==n),FALSE);
	}

	list_palette_sort(&property_palette);
}

/* =======================================================

      Property Palette Click Particle Group
      
======================================================= */

void property_palette_click_particle_group(int particle_idx,int id,bool double_click)
{
	int						idx,sz;
	iface_particle_type		*particle;

	particle=&iface.particle_list.particles[particle_idx];

		// particle group edit
		
	if ((id>=kGroupParticleProperyName) && (id<(kGroupParticleProperyName+max_particle_group))) {
		state.cur_group_particle_idx=id-kGroupParticleProperyName;
		if (double_click) list_palette_set_level(&property_palette,3);
		return;
	}
	
		// particle group delete
		
	if ((id>=kGroupParticleProperyDelete) && (id<(kGroupParticleProperyDelete+max_particle_group))) {
		state.cur_group_particle_idx=-1;

		idx=id-kGroupParticleProperyDelete;

		sz=(particle->group.count-idx)-1;
		if (sz>0) memmove(&particle->group.particles[idx],&particle->group.particles[idx+1],(sz*sizeof(iface_particle_group_piece_type)));

		particle->group.count--;

		return;
	}
	
		// particle group add

	if (id==kGroupParticlePropertyAdd) {
		state.cur_group_particle_idx=-1;

		if (particle->group.count>=max_particle_group) {
			os_dialog_alert("Add Group Particle","Reached the maximum number of group particles");
			return;
		}

		idx=particle->group.count;
		particle->group.count++;

		state.cur_group_particle_idx=idx;
	
		particle->group.particles[idx].name[0]=0x0;
		particle->group.particles[idx].shift=0;

		list_palette_set_level(&property_palette,3);
		property_palette_pick_particle(particle->group.particles[idx].name);

		return;
	}
	
	switch (id) {

			// settings

		case kGroupParticleSettingsName:
			dialog_property_string_run(list_string_value_string,(void*)particle->name,name_str_len,0,0);
			break;
			
	}
}

