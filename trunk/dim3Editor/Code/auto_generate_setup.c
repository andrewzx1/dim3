/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Auto Generation Setup

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

extern map_type					map;
extern file_path_setup_type		file_path_setup;

extern ag_state_type			ag_state;

/* =======================================================

      Shape Utilities
      
======================================================= */

int ag_shape_find(char *name)
{
	int				n;

	for (n=0;n!=ag_state.nshape;n++) {
		if (strcasecmp(name,ag_state.shapes[n].name)==0) return(n);
	}

	return(-1);
}

int ag_shape_get_connector_index(ag_shape_type *shape,int v1_idx,int v2_idx)
{
	int				n;

	for (n=0;n!=shape->nconnector;n++) {
		if ((shape->connectors[n].v[0]==v1_idx) && (shape->connectors[n].v[1]==v2_idx)) return(n);
		if ((shape->connectors[n].v[0]==v2_idx) && (shape->connectors[n].v[1]==v1_idx)) return(n);
	}

	return(-1);
}

/* =======================================================

      Auto Generate PreCalc
      
======================================================= */

void ag_read_settings_setup_connector(ag_shape_type *shape,ag_shape_connector_type *connector)
{
		// connecting on X

	if (shape->vertexes[connector->v[0]].x==shape->vertexes[connector->v[1]].x) {

		if (shape->vertexes[connector->v[0]].x==0) {
			connector->type=ag_connector_type_min_x;
		}
		else {
			connector->type=ag_connector_type_max_x;
		}

		connector->dist=abs(shape->vertexes[connector->v[0]].z-shape->vertexes[connector->v[1]].z);
	}

		// connecting on Z

	else {

		if (shape->vertexes[connector->v[0]].z==0) {
			connector->type=ag_connector_type_min_z;
		}
		else {
			connector->type=ag_connector_type_max_z;
		}
		
		connector->dist=abs(shape->vertexes[connector->v[0]].x-shape->vertexes[connector->v[1]].x);
	}

		// create top left

	if (shape->vertexes[connector->v[0]].x<shape->vertexes[connector->v[1]].x) {
		connector->top_left.x=shape->vertexes[connector->v[0]].x;
	}
	else {
		connector->top_left.x=shape->vertexes[connector->v[1]].x;
	}

	if (shape->vertexes[connector->v[0]].z<shape->vertexes[connector->v[1]].z) {
		connector->top_left.z=shape->vertexes[connector->v[0]].z;
	}
	else {
		connector->top_left.z=shape->vertexes[connector->v[1]].z;
	}
}

/* =======================================================

      Read Auto Generate Settings
      
======================================================= */

bool ag_read_settings(char *path,char *err_str)
{
	int					n,k,t,
						head_tag,size_tag,option_tag,
						shapes_tag,shape_tag,
						vertexes_tag,vertex_tag,
						polys_tag,poly_tag,
						connectors_tag,connector_tag;
	ag_shape_type		*shape;
	ag_shape_poly_type	*shape_poly;

		// decode the file

	if (!xml_open_file(path)) {
		strcpy(err_str,"Missing XML file");
		return(FALSE);
	}

	head_tag=xml_findrootchild("auto_generate");
    if (head_tag==-1) {
		strcpy(err_str,"Illegal XML file");
		xml_close_file();
		return(FALSE);
    }

		// size

	size_tag=xml_findfirstchild("size",head_tag);
	if (size_tag==-1) {
		strcpy(err_str,"Illegal XML file");
		xml_close_file();
		return(FALSE);
	}

	ag_state.size.room_min_count=xml_get_attribute_int(size_tag,"room_min_count");
	ag_state.size.room_max_count=xml_get_attribute_int(size_tag,"room_max_count");
	ag_state.size.room_sz=xml_get_attribute_int(size_tag,"room_size");
	ag_state.size.room_high=xml_get_attribute_int(size_tag,"room_high");
	ag_state.size.story_count=xml_get_attribute_int(size_tag,"story_count");

		// option

	option_tag=xml_findfirstchild("option",head_tag);
	if (option_tag==-1) {
		strcpy(err_str,"Illegal XML file");
		xml_close_file();
		return(FALSE);
	}

	ag_state.option.mirror=xml_get_attribute_boolean(option_tag,"mirror");
	ag_state.option.auto_connect=xml_get_attribute_boolean(option_tag,"auto_connect");

		// shapes

	shapes_tag=xml_findfirstchild("shapes",head_tag);
	if (shapes_tag==-1) {
		strcpy(err_str,"Illegal XML file");
		xml_close_file();
		return(FALSE);
	}

	ag_state.nshape=xml_countchildren(shapes_tag);

	shape=ag_state.shapes;
	shape_tag=xml_findfirstchild("shape",shapes_tag);

	for (n=0;n!=ag_state.nshape;n++) {
		xml_get_attribute_text(shape_tag,"name",shape->name,256);
		shape->single_floor=xml_get_attribute_boolean(shape_tag,"single_floor");
		shape->spawn_spots=xml_get_attribute_boolean(shape_tag,"spawn_spots");

			// shape vertexes

		shape->nvertex=0;

		vertexes_tag=xml_findfirstchild("vertexes",shape_tag);
		if (vertexes_tag!=-1) {

			shape->nvertex=xml_countchildren(vertexes_tag);
			vertex_tag=xml_findfirstchild("vertex",vertexes_tag);

			for (k=0;k!=shape->nvertex;k++) {
				xml_get_attribute_2_coord_int(vertex_tag,"c2",&shape->vertexes[k].x,&shape->vertexes[k].z);
				vertex_tag=xml_findnextchild(vertex_tag);
			}
		}

			// shape polys

		shape->npoly=0;

		polys_tag=xml_findfirstchild("polys",shape_tag);
		if (polys_tag!=-1) {

			shape->npoly=xml_countchildren(polys_tag);
			poly_tag=xml_findfirstchild("poly",polys_tag);

			shape_poly=shape->polys;

			for (k=0;k!=shape->npoly;k++) {
				shape_poly->npt=xml_get_attribute_int_array(poly_tag,"v",shape_poly->v,4);
					
					// determine top or bottom
					// for polygon

				shape_poly->top=shape_poly->bottom=TRUE;

				for (t=0;t!=shape_poly->npt;t++) {
					if (shape->vertexes[shape_poly->v[t]].z>=50) shape_poly->top=FALSE;
					if (shape->vertexes[shape_poly->v[t]].z<=50) shape_poly->bottom=FALSE;
				}

				shape_poly++;
				poly_tag=xml_findnextchild(poly_tag);
			}
		}

			// shape connectors

		shape->nconnector=0;

		connectors_tag=xml_findfirstchild("connectors",shape_tag);
		if (connectors_tag!=-1) {

			shape->nconnector=xml_countchildren(connectors_tag);
			connector_tag=xml_findfirstchild("connector",connectors_tag);

			for (k=0;k!=shape->nconnector;k++) {
				xml_get_attribute_int_array(connector_tag,"v",shape->connectors[k].v,2);
				shape->connectors[k].prefer=xml_get_attribute_boolean(connector_tag,"prefer");
				ag_read_settings_setup_connector(shape,&shape->connectors[k]);
				connector_tag=xml_findnextchild(connector_tag);
			}
		}

			// next shape

		shape++;
		shape_tag=xml_findnextchild(shape_tag);
	}

		// finished

	xml_close_file();
	
	return(TRUE);
}

/* =======================================================

      Auto Generate Memory
      
======================================================= */

bool ag_initialize(char *path,char *err_str)
{
	ag_state.shapes=NULL;
	ag_state.rooms=NULL;

		// memory

	ag_state.nshape=0;
	ag_state.shapes=(ag_shape_type*)malloc(ag_max_shape*sizeof(ag_shape_type));
	if (ag_state.shapes==NULL) {
		strcpy(err_str,"Out of Memory");
		return(FALSE);
	}

	ag_state.nroom=0;
	ag_state.rooms=(ag_room_type*)malloc(ag_max_room*sizeof(ag_room_type));
	if (ag_state.rooms==NULL) {
		free(ag_state.shapes);
		strcpy(err_str,"Out of Memory");
		return(FALSE);
	}

		// read settings file

	return(ag_read_settings(path,err_str));
}

void ag_release(void)
{
	if (ag_state.shapes!=NULL) free(ag_state.shapes);
	ag_state.shapes=NULL;

	if (ag_state.rooms!=NULL) free(ag_state.rooms);
	ag_state.rooms=NULL;
}

