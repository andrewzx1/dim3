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
#include "dialog.h"
#include "walk_view.h"

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

bool ag_read_settings(void)
{
	int					n,k,nshape,
						head_tag,shapes_tag,shape_tag,
						vertexes_tag,vertex_tag,
						polys_tag,poly_tag,
						connectors_tag,connector_tag,
						corridor_tag,styles_tag,style_tag;
	char				path[1024],sub_path[1024],name[256];
	ag_shape_type		*shape;
	ag_style_type		*style;
	
        // find the auto generate.xml file

	os_get_support_file_path(sub_path,"Editor");
	strcat(sub_path,"/Data");

	file_paths_app(&file_path_setup,path,sub_path,"Auto Generate","xml");

	if (!xml_open_file(path)) return(FALSE);
    
		// decode the file
      
    head_tag=xml_findrootchild("auto_generate");
    if (head_tag==-1) {
		xml_close_file();
		return(FALSE);
    }

		// shapes

	shapes_tag=xml_findfirstchild("shapes",head_tag);
	if (shapes_tag==-1) {
		xml_close_file();
		return(FALSE);
	}

	ag_state.nshape=xml_countchildren(shapes_tag);

	shape=ag_state.shapes;
	shape_tag=xml_findfirstchild("shape",shapes_tag);

	for (n=0;n!=ag_state.nshape;n++) {
		xml_get_attribute_text(shape_tag,"name",shape->name,256);

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

			for (k=0;k!=shape->npoly;k++) {
				shape->polys[k].npt=xml_get_attribute_int_array(poly_tag,"v",shape->polys[k].v,4);
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
				ag_read_settings_setup_connector(shape,&shape->connectors[k]);
				connector_tag=xml_findnextchild(connector_tag);
			}
		}

			// corridors

		shape->corridor=FALSE;

		corridor_tag=xml_findfirstchild("corridor",shape_tag);
		if (corridor_tag!=-1) {
			shape->corridor=TRUE;
		}

			// next shape

		shape++;
		shape_tag=xml_findnextchild(shape_tag);
	}

		// styles

	styles_tag=xml_findfirstchild("styles",head_tag);
	if (styles_tag==-1) {
		xml_close_file();
		return(FALSE);
	}

	ag_state.nstyle=xml_countchildren(styles_tag);

	style=ag_state.styles;
	style_tag=xml_findfirstchild("style",styles_tag);

	for (n=0;n!=ag_state.nstyle;n++) {
		xml_get_attribute_text(style_tag,"name",style->name,256);

			// load in the shape list

		style->nshape=0;

		shapes_tag=xml_findfirstchild("shapes",style_tag);
		if (shapes_tag!=-1) {

			nshape=xml_countchildren(shapes_tag);

			shape_tag=xml_findfirstchild("shape",shapes_tag);

			for (k=0;k!=nshape;k++) {
				xml_get_attribute_text(shape_tag,"name",name,256);

				style->shape_list[style->nshape]=ag_shape_find(name);
				if (style->shape_list[style->nshape]!=-1) style->nshape++;

				shape_tag=xml_findnextchild(shape_tag);
			}
		}

  			// next style

		style++;
		style_tag=xml_findnextchild(style_tag);
	}

		// finished

	xml_close_file();
	
	return(TRUE);
}

/* =======================================================

      Auto Generate Memory
      
======================================================= */

bool ag_initialize(void)
{
	ag_state.styles=NULL;
	ag_state.shapes=NULL;
	ag_state.rooms=NULL;

		// memory

	ag_state.nstyle=0;
	ag_state.styles=(ag_style_type*)malloc(ag_max_style*sizeof(ag_style_type));
	if (ag_state.styles==NULL) return(FALSE);

	ag_state.nshape=0;
	ag_state.shapes=(ag_shape_type*)malloc(ag_max_shape*sizeof(ag_shape_type));
	if (ag_state.shapes==NULL) return(FALSE);

	ag_state.nroom=0;
	ag_state.rooms=(ag_room_type*)malloc(ag_max_room*sizeof(ag_room_type));
	if (ag_state.rooms==NULL) return(FALSE);

		// read settings file

	return(ag_read_settings());
}

void ag_release(void)
{
	if (ag_state.styles!=NULL) free(ag_state.styles);
	ag_state.styles=NULL;

	if (ag_state.shapes!=NULL) free(ag_state.shapes);
	ag_state.shapes=NULL;

	if (ag_state.rooms!=NULL) free(ag_state.rooms);
	ag_state.rooms=NULL;
}

/* =======================================================

      Auto Generate UI Utilities
      
======================================================= */

int ag_get_styles(char *style_list)
{
	int				n,nstyle;

	ag_initialize();

	nstyle=ag_state.nstyle;

	for (n=0;n!=nstyle;n++) {
		strcpy((char*)&style_list[n*name_str_len],ag_state.styles[n].name);
	}

	ag_release();

	return(nstyle);
}
