/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Map File Reading (v3 [mesh, no portal] maps)

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

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3maputility.h"
#endif

extern maputility_settings_type		maputility_settings;

extern char				media_type_str[][32],
						sky_type_str[][32],
						gl_fog_type_str[][32],
						mesh_hide_mode_str[][32],
						mesh_normal_mode_str[][32],
						liquid_tide_direction_str[][32],
                        light_type_str[][32],
						light_direction_str[][32],
						spot_type_str[][32],
						skill_type_str[][32],
						spawn_type_str[][32],
						map_bump_mode_str[][32];

/* =======================================================

      Read Meshes and Liquids
      
======================================================= */

bool read_single_mesh_v3(map_type *map,int mesh_idx,int mesh_tag)
{
	int					n,nvertex,npoly,old_mesh_lmap_txt_idx,
						msg_tag,main_vertex_tag,vertex_tag,main_poly_tag,poly_tag,tag;
	bool				mesh_climbable;
	d3pnt				*pt;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

	mesh=&map->mesh.meshes[mesh_idx];
	
		// older versions had light maps on a per mesh level
		// check to see if that exists and if it does, override the
		// polygons
		
	old_mesh_lmap_txt_idx=xml_get_attribute_int_default(mesh_tag,"lmap_txt_idx",-1);

		// mesh settings

	mesh->group_idx=xml_get_attribute_int_default(mesh_tag,"group",-1);

	mesh->flag.on=!xml_get_attribute_boolean(mesh_tag,"off");
	mesh->flag.pass_through=xml_get_attribute_boolean(mesh_tag,"pass");
	mesh->flag.moveable=xml_get_attribute_boolean(mesh_tag,"moveable");
	mesh->flag.hilite=xml_get_attribute_boolean(mesh_tag,"hilite");
	mesh->flag.lock_uv=xml_get_attribute_boolean(mesh_tag,"lock_uv");
	mesh->flag.lock_move=xml_get_attribute_boolean(mesh_tag,"lock_move");
	mesh->flag.never_obscure=xml_get_attribute_boolean(mesh_tag,"never_obscure");
	mesh->flag.rot_independent=xml_get_attribute_boolean(mesh_tag,"rot_independent");
	mesh->flag.shadow=xml_get_attribute_boolean(mesh_tag,"shadow");
	mesh->flag.no_light_map=xml_get_attribute_boolean(mesh_tag,"no_light_map");
	mesh->flag.skip_light_map_trace=xml_get_attribute_boolean(mesh_tag,"skip_light_map_trace");
	
	mesh->hide_mode=xml_get_attribute_list(mesh_tag,"hide",(char*)mesh_hide_mode_str);
	mesh->normal_mode=xml_get_attribute_list(mesh_tag,"normal",(char*)mesh_normal_mode_str);

	xml_get_attribute_3_coord_int(mesh_tag,"rot_off",&mesh->rot_off.x,&mesh->rot_off.y,&mesh->rot_off.z);
	
		// old version of climbable at mesh level
		// push it to polys if it's still there
		
	mesh_climbable=xml_get_attribute_boolean(mesh_tag,"climbable");

		// messages

    mesh->msg.entry_on=mesh->msg.exit_on=mesh->msg.base_on=mesh->msg.map_change_on=FALSE;
    mesh->msg.entry_id=mesh->msg.exit_id=0;
	mesh->msg.map_name[0]=0x0;
	strcpy(mesh->msg.map_spot_name,"Start");
    
    msg_tag=xml_findfirstchild("Messages",mesh_tag);
    if (msg_tag!=-1) {
        tag=xml_findfirstchild("Entry",msg_tag);
        if (tag!=-1) {
            mesh->msg.entry_on=xml_get_attribute_boolean(tag,"on");
            mesh->msg.entry_id=xml_get_attribute_int(tag,"id");
        }
        tag=xml_findfirstchild("Exit",msg_tag);
        if (tag!=-1) {
            mesh->msg.exit_on=xml_get_attribute_boolean(tag,"on");
            mesh->msg.exit_id=xml_get_attribute_int(tag,"id");
        }
        tag=xml_findfirstchild("Base",msg_tag);
        if (tag!=-1) {
            mesh->msg.base_on=xml_get_attribute_boolean(tag,"on");
            mesh->msg.base_team=xml_get_attribute_int(tag,"team");
        }
        tag=xml_findfirstchild("Map",msg_tag);
        if (tag!=-1) {
            mesh->msg.map_change_on=xml_get_attribute_boolean(tag,"on");
			xml_get_attribute_text(tag,"name",mesh->msg.map_name,name_str_len);
			xml_get_attribute_text(tag,"spot_name",mesh->msg.map_spot_name,name_str_len);
        }
	}

		// vertexes

    main_vertex_tag=xml_findfirstchild("Vertexes",mesh_tag);
    if (main_vertex_tag!=-1) {
    
        nvertex=xml_countchildren(main_vertex_tag);
		vertex_tag=xml_findfirstchild("v",main_vertex_tag);

		if (!map_mesh_set_vertex_count(map,mesh_idx,nvertex)) return(FALSE);

		pt=mesh->vertexes;

		for (n=0;n!=nvertex;n++) {
			xml_get_attribute_3_coord_int(vertex_tag,"c3",&pt->x,&pt->y,&pt->z);
			pt++;
			vertex_tag=xml_findnextchild(vertex_tag);
		}
	}

		// polys

    main_poly_tag=xml_findfirstchild("Polys",mesh_tag);
    if (main_poly_tag!=-1) {
    
        npoly=xml_countchildren(main_poly_tag);
		poly_tag=xml_findfirstchild("p",main_poly_tag);

		if (!map_mesh_set_poly_count(map,mesh_idx,npoly)) return(FALSE);

		poly=mesh->polys;

		for (n=0;n!=npoly;n++) {
			poly->ptsz=xml_get_attribute_int_array(poly_tag,"v",poly->v,8);
			
			poly->txt_idx=xml_get_attribute_int(poly_tag,"txt");
			poly->lmap_txt_idx=xml_get_attribute_int_default(poly_tag,"lmap_txt_idx",old_mesh_lmap_txt_idx);

			xml_get_attribute_3_coord_float(poly_tag,"t3",&poly->tangent_space.tangent.x,&poly->tangent_space.tangent.y,&poly->tangent_space.tangent.z);
			xml_get_attribute_3_coord_float(poly_tag,"b3",&poly->tangent_space.binormal.x,&poly->tangent_space.binormal.y,&poly->tangent_space.binormal.z);
			xml_get_attribute_3_coord_float(poly_tag,"n3",&poly->tangent_space.normal.x,&poly->tangent_space.normal.y,&poly->tangent_space.normal.z);

			xml_get_attribute_float_array(poly_tag,"x",poly->main_uv.x,8);
			xml_get_attribute_float_array(poly_tag,"y",poly->main_uv.y,8);
			xml_get_attribute_2_coord_float(poly_tag,"shift",&poly->x_shift,&poly->y_shift);

			if (poly->lmap_txt_idx!=-1) {
				xml_get_attribute_float_array(poly_tag,"x_1",poly->lmap_uv.x,8);
				xml_get_attribute_float_array(poly_tag,"y_1",poly->lmap_uv.y,8);
			}

			poly->climbable=xml_get_attribute_boolean(poly_tag,"climbable") || (mesh_climbable);
			poly->never_cull=xml_get_attribute_boolean(poly_tag,"never_cull");

			xml_get_attribute_text_default_blank(poly_tag,"camera",poly->camera,name_str_len);

			poly++;
			poly_tag=xml_findnextchild(poly_tag);
		}
	}

	return(TRUE);
}

void read_single_liquid_v3(map_type *map,int liquid_idx,int liquid_tag)
{
	int					tag;
	map_liquid_type		*liq;

	liq=&map->liquid.liquids[liquid_idx];

		// liquid settings

	liq->group_idx=xml_get_attribute_int_default(liquid_tag,"group",-1);
	liq->never_obscure=xml_get_attribute_boolean(liquid_tag,"never_obscure");
	liq->never_cull=xml_get_attribute_boolean(liquid_tag,"never_cull");
	liq->no_draw=xml_get_attribute_boolean(liquid_tag,"no_draw");

		// polygon

    tag=xml_findfirstchild("Poly",liquid_tag);
    if (tag!=-1) {
		liq->txt_idx=xml_get_attribute_int(tag,"txt");
		liq->lmap_txt_idx=xml_get_attribute_int_default(tag,"lmap_txt_idx",-1);
		xml_get_attribute_3_coord_int(tag,"v1",&liq->lft,&liq->y,&liq->top);
		xml_get_attribute_3_coord_int(tag,"v2",&liq->rgt,&liq->y,&liq->bot);
		liq->depth=xml_get_attribute_int(tag,"depth");

		xml_get_attribute_2_coord_float(tag,"uv_off",&liq->main_uv.x_offset,&liq->main_uv.y_offset);
		xml_get_attribute_2_coord_float(tag,"uv_size",&liq->main_uv.x_size,&liq->main_uv.y_size);
		if (liq->lmap_txt_idx!=-1) {
			xml_get_attribute_2_coord_float(tag,"uv_1_off",&liq->lmap_uv.x_offset,&liq->lmap_uv.y_offset);
			xml_get_attribute_2_coord_float(tag,"uv_1_size",&liq->lmap_uv.x_size,&liq->lmap_uv.y_size);
		}

		xml_get_attribute_color(tag,"rgb",&liq->col);
		liq->tint_alpha=xml_get_attribute_float(tag,"tint_alpha");
		xml_get_attribute_2_coord_float(tag,"shift",&liq->x_shift,&liq->y_shift);
		
		xml_get_attribute_text_default_blank(tag,"camera",liq->camera,name_str_len);
		xml_get_attribute_text_default_blank(tag,"ambient_sound_name",liq->ambient.sound_name,name_str_len);
	}

		// physics

    tag=xml_findfirstchild("Physic",liquid_tag);
    if (tag!=-1) {
		liq->speed_alter=xml_get_attribute_float(tag,"speed_alter");
	}

		// harm

    tag=xml_findfirstchild("Harm",liquid_tag);
    if (tag!=-1) {
		liq->harm.in_harm=xml_get_attribute_int(tag,"harm");
		liq->harm.drown_harm=xml_get_attribute_int(tag,"drown_harm");
		liq->harm.drown_tick=xml_get_attribute_int(tag,"drown_tick");
	}

		// tides

    tag=xml_findfirstchild("Tide",liquid_tag);
    if (tag!=-1) {
		liq->tide.division=xml_get_attribute_int_default(tag,"division",liquid_min_division);
		liq->tide.rate=xml_get_attribute_int(tag,"rate");
		liq->tide.high=xml_get_attribute_int(tag,"high");
		liq->tide.direction=xml_get_attribute_list(tag,"tide_direction",(char*)liquid_tide_direction_str);
        liq->tide.flat=xml_get_attribute_boolean(tag,"flat");
	}
}

/* =======================================================

      Read Map XML
      
======================================================= */

bool decode_map_v3_xml(map_type *map,int map_head)
{
	int						k,j,idx,nmesh,mesh_idx,nliquid,liquid_idx,
							main_mesh_tag,mesh_tag,main_liquid_tag,liquid_tag,
							main_light_tag,light_tag,main_sound_tag,sound_tag,
							main_particle_tag,particle_tag,main_node_tag,node_tag,
							main_obj_tag,obj_tag,tag,cnt;
	char					str[32];
    map_light_type			*light;
    map_sound_type			*sound;
	map_particle_type		*particle;
    node_type				*node;
    spot_type				*spot;
	map_scenery_type		*scenery;

        // meshes

	map->mesh.nmesh=0;
	map->mesh.meshes=NULL;

	main_mesh_tag=xml_findfirstchild("Meshes",map_head);
	if (main_mesh_tag!=-1) {

		nmesh=xml_countchildren(main_mesh_tag);
		if (nmesh!=0) {
			
			mesh_tag=xml_findfirstchild("Mesh",main_mesh_tag);

			for (k=0;k!=nmesh;k++) {

					// create new mesh

				mesh_idx=map_mesh_add(map);
				if (mesh_idx==-1) return(FALSE);

					// read mesh

				if (!read_single_mesh_v3(map,mesh_idx,mesh_tag)) return(FALSE);
				
				mesh_tag=xml_findnextchild(mesh_tag);
			}
		}
	}

		// liquids

	map->liquid.nliquid=0;
	map->liquid.liquids=NULL;

	main_liquid_tag=xml_findfirstchild("Liquids",map_head);
	if (main_liquid_tag!=-1) {

		nliquid=xml_countchildren(main_liquid_tag);
		if (nliquid!=0) {
			
			liquid_tag=xml_findfirstchild("Liquid",main_liquid_tag);

			for (k=0;k!=nliquid;k++) {

					// create new liquid

				liquid_idx=map_liquid_add(map);
				if (liquid_idx==-1) return(FALSE);

					// read liquid

				read_single_liquid_v3(map,liquid_idx,liquid_tag);
				
				liquid_tag=xml_findnextchild(liquid_tag);
			}
		}
	}

		// scenery

	main_obj_tag=xml_findfirstchild("Sceneries",map_head);
	if (main_obj_tag!=-1) {
	
		cnt=xml_countchildren(main_obj_tag);
		obj_tag=xml_findfirstchild("Scenery",main_obj_tag);
		
		for (k=0;k!=cnt;k++) {
			if (map->nscenery>=max_map_scenery) break;
			
			scenery=&map->sceneries[map->nscenery];
			map->nscenery++;
			
			xml_get_attribute_3_coord_int(obj_tag,"c3",&scenery->pnt.x,&scenery->pnt.y,&scenery->pnt.z);
			
			xml_get_attribute_text(obj_tag,"model_name",scenery->model_name,name_str_len);
			xml_get_attribute_text(obj_tag,"animation_name",scenery->animation_name,name_str_len);
			xml_get_attribute_3_coord_float(obj_tag,"angle",&scenery->ang.x,&scenery->ang.y,&scenery->ang.z);
			scenery->contact_object_on=xml_get_attribute_boolean(obj_tag,"contact");
			scenery->contact_projectile_on=xml_get_attribute_boolean(obj_tag,"contact_projectile");
			scenery->contact_hit_box=xml_get_attribute_boolean(obj_tag,"contact_hit_box");
			scenery->face_forward=xml_get_attribute_boolean(obj_tag,"face_forward");
			scenery->shadow=xml_get_attribute_boolean(obj_tag,"shadow");
			
			xml_get_attribute_short_array(obj_tag,"frames",scenery->texture_frame,max_map_scenery_model_texture_frame);
			
			scenery->resize=xml_get_attribute_float_default(obj_tag,"resize",1.0f);
			
			obj_tag=xml_findnextchild(obj_tag);
		}
	}

		// lights
	
	main_light_tag=xml_findfirstchild("Lights",map_head);
	if (main_light_tag!=-1) {
	
		cnt=xml_countchildren(main_light_tag);
		light_tag=xml_findfirstchild("Light",main_light_tag);
		
		for (k=0;k!=cnt;k++) {
			if (map->nlight>=max_map_light) break;
			
			light=&map->lights[map->nlight];
			map->nlight++;

			xml_get_attribute_text(light_tag,"name",light->name,name_str_len);

			light->type=xml_get_attribute_list(light_tag,"type",(char*)light_type_str);
			light->light_map=xml_get_attribute_boolean(light_tag,"light_map");

			if (xml_get_attribute_text(light_tag,"filter",str,32)) {
				if (strcasecmp(str,"not_mesh")==0) light->light_map=TRUE;		// supergumba -- temporary as we change over attributes here
			}

			light->direction=xml_get_attribute_list(light_tag,"direction",(char*)light_direction_str);
			xml_get_attribute_3_coord_int(light_tag,"c3",&light->pnt.x,&light->pnt.y,&light->pnt.z);
			light->intensity=xml_get_attribute_int(light_tag,"intensity");
			light->exponent=xml_get_attribute_float_default(light_tag,"exponent",1.0f);
			xml_get_attribute_color(light_tag,"rgb",&light->col);

			light->on=!xml_get_attribute_boolean(light_tag,"off");
			
			if (light->intensity<0) light->intensity=1;
		
			light_tag=xml_findnextchild(light_tag);
		}
	}
	
		// sounds
		
	main_sound_tag=xml_findfirstchild("Sounds",map_head);
	if (main_sound_tag!=-1) {
	
		cnt=xml_countchildren(main_sound_tag);
		sound_tag=xml_findfirstchild("Sound",main_sound_tag);
		
		for (k=0;k!=cnt;k++) {
			if (map->nsound>=max_map_sound) break;
			
			sound=&map->sounds[map->nsound];
			map->nsound++;
			
			xml_get_attribute_text(sound_tag,"name",sound->name,file_str_len);
			xml_get_attribute_3_coord_int(sound_tag,"c3",&sound->pnt.x,&sound->pnt.y,&sound->pnt.z);
			sound->pitch=xml_get_attribute_float(sound_tag,"pitch");
			sound->on=!xml_get_attribute_boolean(sound_tag,"off");
			
			sound_tag=xml_findnextchild(sound_tag);
		}
	}
	
		// particles
		
	main_particle_tag=xml_findfirstchild("Particles",map_head);
	if (main_particle_tag!=-1) {
	
		cnt=xml_countchildren(main_particle_tag);
		particle_tag=xml_findfirstchild("Particle",main_particle_tag);
		
		for (k=0;k!=cnt;k++) {
			if (map->nparticle>=max_map_particle) break;
			
			particle=&map->particles[map->nparticle];
			map->nparticle++;
			
			xml_get_attribute_text(particle_tag,"name",particle->name,file_str_len);
			xml_get_attribute_3_coord_int(particle_tag,"c3",&particle->pnt.x,&particle->pnt.y,&particle->pnt.z);
			particle->spawn_tick=xml_get_attribute_int(particle_tag,"spawn_tick");
			particle->slop_tick=xml_get_attribute_int(particle_tag,"slop_tick");
			particle->on=!xml_get_attribute_boolean(particle_tag,"off");
			
			particle_tag=xml_findnextchild(particle_tag);
		}
	}

		// nodes

	main_node_tag=xml_findfirstchild("Nodes",map_head);
	if (main_node_tag!=-1) {
	
		cnt=xml_countchildren(main_node_tag);
		node_tag=xml_findfirstchild("Node",main_node_tag);
		
		for (k=0;k!=cnt;k++) {
		
			idx=xml_get_attribute_int(node_tag,"id");
			node=&map->nodes[idx];
			if (idx>=map->nnode) map->nnode=idx+1;

			node->idx=idx;
			
			for (j=0;j!=max_node_link;j++) {
				node->link[j]=-1;
			}
			for (j=0;j!=max_node;j++) {
				node->path_hint[j]=-1;
			}
			
			xml_get_attribute_3_coord_int(node_tag,"c3",&node->pnt.x,&node->pnt.y,&node->pnt.z);

			xml_get_attribute_3_coord_float(node_tag,"angle",&node->ang.x,&node->ang.y,&node->ang.z);
			node->follow_camera=xml_get_attribute_boolean(node_tag,"follow_camera");
			node->alpha=xml_get_attribute_float_default(node_tag,"alpha",1.0f);

			xml_get_attribute_text(node_tag,"name",node->name,name_str_len);
			node->event_id=xml_get_attribute_int_default(node_tag,"event_id",0);
			
			tag=xml_findfirstchild("Link",node_tag);
			xml_get_attribute_short_array(tag,"node",node->link,max_node_link);
			
			tag=xml_findfirstchild("Hint",node_tag);
			xml_get_attribute_short_array(tag,"node",node->path_hint,max_node);

			node_tag=xml_findnextchild(node_tag);
		}
	}

		// spots

	main_obj_tag=xml_findfirstchild("Spots",map_head);
	if (main_obj_tag!=-1) {
	
		cnt=xml_countchildren(main_obj_tag);
		obj_tag=xml_findfirstchild("Spot",main_obj_tag);
		
		for (k=0;k!=cnt;k++) {
		
			idx=xml_get_attribute_int(obj_tag,"id");
			spot=&map->spots[idx];
			if (idx>=map->nspot) map->nspot=idx+1;
			
			xml_get_attribute_3_coord_int(obj_tag,"c3",&spot->pnt.x,&spot->pnt.y,&spot->pnt.z);
			
			xml_get_attribute_text(obj_tag,"name",spot->name,name_str_len);
			spot->type=xml_get_attribute_list(obj_tag,"type",(char*)spot_type_str);
			xml_get_attribute_text(obj_tag,"script",spot->script,name_str_len);
			xml_get_attribute_text(obj_tag,"display_model",spot->display_model,name_str_len);
			xml_get_attribute_text(obj_tag,"params",spot->params,param_str_len);
			spot->ang.y=xml_get_attribute_float(obj_tag,"angle");
			
			spot->skill=xml_get_attribute_list(obj_tag,"skill",(char*)skill_type_str);
			spot->spawn=xml_get_attribute_list(obj_tag,"spawn",(char*)spawn_type_str);
			
			obj_tag=xml_findnextchild(obj_tag);
		}
	}

	return(TRUE);
}
