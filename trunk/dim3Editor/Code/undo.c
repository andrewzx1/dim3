/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Map Undo

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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3editor.h"
#endif

#include "common_view.h"
#include "interface.h"

extern map_type						map;

int					undo_level=0;
undo_type			undos[max_undo_level];

/* =======================================================

      Initialize Undo
      
======================================================= */

void undo_initialize(void)
{
	int			n;
	undo_type	*undo;
	
	undo_level=0;
	
	undo=undos;
	
	for (n=0;n!=max_undo_level;n++) {
		undo->nmesh=0;
		undo->nliquid=0;
		undo->nspot=0;
		undo->nscenery=0;
		undo->nnode=0;
		undo->nlight=0;
		undo->nsound=0;
		undo->nparticle=0;

		undo->meshes=NULL;
		undo->liquids=NULL;
		undo->spots=NULL;
		undo->sceneries=NULL;
		undo->nodes=NULL;
		undo->lights=NULL;
		undo->sounds=NULL;
		undo->particles=NULL;
	}
}

/* =======================================================

      Clear Undos
      
======================================================= */

void undo_clear(int idx)
{
	int				n;
	undo_type		*undo;
	map_mesh_type	*mesh;
	
	undo=&undos[idx];
	
		// clear meshes
	
	if (undo->meshes!=NULL) {
	
		mesh=undo->meshes;
		
		for (n=0;n!=undo->nmesh;n++) {
			if (mesh->vertexes!=NULL) free(mesh->vertexes);
			if (mesh->polys!=NULL) free(mesh->polys);
			
			mesh++;
		}
		
		free(undo->meshes);
		
		undo->nmesh=0;
		undo->meshes=NULL;
	}
	
		// clear spots, scenery, nodes
		
	if (undo->spots!=NULL) {
		free(undo->spots);
		undo->nspot=0;
		undo->spots=NULL;
	}
	
	if (undo->sceneries!=NULL) {
		free(undo->sceneries);
		undo->nscenery=0;
		undo->sceneries=NULL;
	}
	
	if (undo->nodes!=NULL) {
		free(undo->nodes);
		undo->nnode=0;
		undo->nodes=NULL;
	}
}

void undo_clear_all(void)
{
	int			n;
	
	for (n=0;n!=max_undo_level;n++) {
		undo_clear(n);
	}
	
	os_enable_menu_item_undo(FALSE);
}

/* =======================================================

      Copy Undos
      
======================================================= */

void undo_copy(int srce_idx,int dest_idx)
{
	undo_type		*srce_undo,*dest_undo;
	
	srce_undo=&undos[srce_idx];
	dest_undo=&undos[dest_idx];
	
	dest_undo->nmesh=srce_undo->nmesh;
	dest_undo->meshes=srce_undo->meshes;
	
	dest_undo->nliquid=srce_undo->nliquid;
	dest_undo->liquids=srce_undo->liquids;
	
	dest_undo->nspot=srce_undo->nspot;
	dest_undo->spots=srce_undo->spots;

	dest_undo->nscenery=srce_undo->nscenery;
	dest_undo->sceneries=srce_undo->sceneries;

	dest_undo->nnode=srce_undo->nnode;
	dest_undo->nodes=srce_undo->nodes;

	dest_undo->nlight=srce_undo->nlight;
	dest_undo->lights=srce_undo->lights;

	dest_undo->nsound=srce_undo->nsound;
	dest_undo->sounds=srce_undo->sounds;

	dest_undo->nparticle=srce_undo->nparticle;
	dest_undo->particles=srce_undo->particles;
}

/* =======================================================

      Push Undo
      
======================================================= */

bool undo_push_internal(void)
{
	int				n,nmesh;
	undo_type		*undo;
	map_mesh_type	*org_mesh,*mesh;
	
		// clear last undo
	
	undo_clear(max_undo_level-1);
	
		// push undo stack down
		
	for (n=(max_undo_level-2);n>=0;n--) {
		undo_copy(n,(n+1));
	}
	
		// start with clear
		
	undo=&undos[0];
	
	undo->nmesh=0;
	undo->meshes=NULL;
	
	undo->nliquid=0;
	undo->liquids=NULL;
	
	undo->nspot=0;
	undo->spots=NULL;
	
	undo->nscenery=0;
	undo->sceneries=NULL;
	
	undo->nnode=0;
	undo->nodes=NULL;
	
	undo->nlight=0;
	undo->lights=NULL;

	undo->nsound=0;
	undo->sounds=NULL;

	undo->nparticle=0;
	undo->particles=NULL;
	
		// save all meshes
		
	nmesh=map.mesh.nmesh;
	if (nmesh!=0) {
	
		undo->meshes=(map_mesh_type*)malloc(nmesh*sizeof(map_mesh_type));
		if (undo->meshes==NULL) return(FALSE);
				
		org_mesh=map.mesh.meshes;
		mesh=undo->meshes;
		
		for (n=0;n!=nmesh;n++) {
		
			memmove(mesh,org_mesh,sizeof(map_mesh_type));
			
				// need to dup the vertexes and polygons
				
			mesh->vertexes=(d3pnt*)malloc(org_mesh->nvertex*sizeof(d3pnt));
			if (mesh->vertexes==NULL) {
				free(undo->meshes);
				undo->meshes=NULL;
				return(FALSE);
			}
			
			memmove(mesh->vertexes,org_mesh->vertexes,(org_mesh->nvertex*sizeof(d3pnt)));
			
			mesh->polys=(map_mesh_poly_type*)malloc(org_mesh->npoly*sizeof(map_mesh_poly_type));
			if (mesh->polys==NULL) {
				free(undo->meshes);
				undo->meshes=NULL;
				return(FALSE);
			}
			
			memmove(mesh->polys,org_mesh->polys,(org_mesh->npoly*sizeof(map_mesh_poly_type)));
						
			org_mesh++;
			mesh++;
		}
		
		undo->nmesh=nmesh;
	}
	
		// liquids
		
	if (map.liquid.nliquid!=0) {
		undo->liquids=(map_liquid_type*)malloc(map.liquid.nliquid*sizeof(map_liquid_type));
		if (undo->liquids==NULL) return(FALSE);
		
		memmove(undo->liquids,map.liquid.liquids,(map.liquid.nliquid*sizeof(map_liquid_type)));
		undo->nliquid=map.liquid.nliquid;
	}
	
		// save spots, scenery,nodes, lights, sounds, and particles
		
	if (map.nspot!=0) {
		undo->spots=(spot_type*)malloc(map.nspot*sizeof(spot_type));
		if (undo->spots==NULL) return(FALSE);
		
		memmove(undo->spots,map.spots,(map.nspot*sizeof(spot_type)));
		undo->nspot=map.nspot;
	}
			
	if (map.nscenery!=0) {
		undo->sceneries=(map_scenery_type*)malloc(map.nscenery*sizeof(map_scenery_type));
		if (undo->sceneries==NULL) return(FALSE);
		
		memmove(undo->sceneries,map.sceneries,(map.nscenery*sizeof(map_scenery_type)));
		undo->nscenery=map.nscenery;
	}
	
	if (map.nnode!=0) {
		undo->nodes=(node_type*)malloc(map.nnode*sizeof(node_type));
		if (undo->nodes==NULL) return(FALSE);
		
		memmove(undo->nodes,map.nodes,(map.nnode*sizeof(node_type)));
		undo->nnode=map.nnode;
	}

	if (map.nlight!=0) {
		undo->lights=(map_light_type*)malloc(map.nlight*sizeof(map_light_type));
		if (undo->lights==NULL) return(FALSE);
		
		memmove(undo->lights,map.lights,(map.nlight*sizeof(map_light_type)));
		undo->nlight=map.nlight;
	}

	if (map.nsound!=0) {
		undo->sounds=(map_sound_type*)malloc(map.nsound*sizeof(map_sound_type));
		if (undo->sounds==NULL) return(FALSE);
		
		memmove(undo->sounds,map.sounds,(map.nsound*sizeof(map_sound_type)));
		undo->nsound=map.nsound;
	}

	if (map.nparticle!=0) {
		undo->particles=(map_particle_type*)malloc(map.nparticle*sizeof(map_particle_type));
		if (undo->particles==NULL) return(FALSE);
		
		memmove(undo->particles,map.particles,(map.nparticle*sizeof(map_particle_type)));
		undo->nparticle=map.nparticle;
	}
	
		// move up undo level
		
	undo_level++;
	if (undo_level>max_undo_level) undo_level=max_undo_level;
	
	return(TRUE);
}

void undo_push(void)
{
	if (undo_push_internal()) {
		os_enable_menu_item_undo(TRUE);
		return;
	}
	
	dialog_alert("Undo","Not enough memory to setup undo");
	undo_clear_all();
	
	os_enable_menu_item_undo(FALSE);
}

/* =======================================================

      Push Undo
      
======================================================= */

void undo_pull(void)
{
	int				n;
	undo_type		*undo;
	map_mesh_type	*org_mesh,*mesh;
	
	if (undo_level==0) return;
	
	undo=&undos[0];
	
		// restore all meshes
		
	if (undo->nmesh!=0) {
		mesh=map.mesh.meshes;
		org_mesh=undo->meshes;
		
		for (n=0;n!=undo->nmesh;n++) {
		
				// just copy over from backup mesh
				// and then null out the pointers
				// so close doesn't erase them
				
			if (mesh->vertexes!=NULL) free(mesh->vertexes);
			if (mesh->polys!=NULL) free(mesh->polys);
				
			memmove(mesh,org_mesh,sizeof(map_mesh_type));

			org_mesh->vertexes=NULL;
			org_mesh->polys=NULL;
						
			org_mesh++;
			mesh++;
		}
		
		map.mesh.nmesh=undo->nmesh;
	}
	
		// restore liquids
		
	if (undo->nliquid!=0) {
		map.liquid.nliquid=undo->nliquid;
		memmove(map.liquid.liquids,undo->liquids,(undo->nliquid*sizeof(map_liquid_type)));
	}
	
		// restore spots, scenery, nodes, lights, sounds, and particles
		
	if (undo->nspot!=0) {
		map.nspot=undo->nspot;
		memmove(map.spots,undo->spots,(undo->nspot*sizeof(spot_type)));
	}
	
	if (undo->nscenery!=0) {
		map.nscenery=undo->nscenery;
		memmove(map.sceneries,undo->sceneries,(undo->nscenery*sizeof(map_scenery_type)));
	}
	
	if (undo->nnode!=0) {
		map.nnode=undo->nnode;
		memmove(map.nodes,undo->nodes,(undo->nnode*sizeof(node_type)));
	}

	if (undo->nlight!=0) {
		map.nlight=undo->nlight;
		memmove(map.lights,undo->lights,(undo->nlight*sizeof(spot_type)));
	}

	if (undo->nsound!=0) {
		map.nsound=undo->nsound;
		memmove(map.sounds,undo->sounds,(undo->nsound*sizeof(spot_type)));
	}

	if (undo->nparticle!=0) {
		map.nparticle=undo->nparticle;
		memmove(map.particles,undo->particles,(undo->nparticle*sizeof(spot_type)));
	}
	
		// clear level
		// and move stack up
		
	undo_clear(0);
	
	for (n=1;n!=max_undo_level;n++) {
		undo_copy(n,(n-1));
	}
	
		// move down undo level
		
	undo_level--;
	if (undo_level==0) os_enable_menu_item_undo(FALSE);
	
		// redraw windows
		
	palette_reset();
	main_wind_draw();
}
