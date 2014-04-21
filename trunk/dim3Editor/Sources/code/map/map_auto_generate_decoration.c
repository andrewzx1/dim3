/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Auto Generation Utility

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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3editor.h"
#endif

#include "glue.h"
#include "interface.h"

extern map_type					map;

extern ag_state_type			ag_state;

extern int ag_random_int(int max);
extern bool ag_random_bool(void);

/* =======================================================

      Collisions
      
======================================================= */

bool ag_generate_mesh_collision(d3pnt *min,d3pnt *max,int start_cmp_mesh_idx,int end_cmp_mesh_idx)
{
	int				n;
	d3pnt			cmp_min,cmp_max;

	for (n=start_cmp_mesh_idx;n<end_cmp_mesh_idx;n++) {

		map_mesh_calculate_extent(&map,n,&cmp_min,&cmp_max);
		if ((min->x>=cmp_max.x) || (max->x<=cmp_min.x)) continue;
		if ((min->y>=cmp_max.y) || (max->y<=cmp_min.y)) continue;
		if ((min->z>=cmp_max.z) || (max->z<=cmp_min.z)) continue;
		
		return(TRUE);
	}

	return(FALSE);
}

/* =======================================================

      Decoration Locations
      
======================================================= */

void ag_generate_decoration_location_random(int room_idx,d3pnt *pnt)
{
	d3pnt			min,max,sz,margin;
	ag_room_type	*room;

		// get current room dimensions

	room=&ag_state.rooms[room_idx];
	map_mesh_calculate_extent(&map,room->mesh_idx,&min,&max);

		// room size and margin

	sz.x=max.x-min.x;
	sz.z=max.z-min.z;

	margin.x=(int)(((float)sz.x)*0.25f);
	margin.z=(int)(((float)sz.z)*0.25f);

		// get random location

	pnt->x=(min.x+margin.x)+(ag_random_int(sz.x-(margin.x*2)));
	pnt->y=max.y;
	pnt->z=(min.z+margin.z)+(ag_random_int(sz.z-(margin.z*2)));
}

void ag_generate_decoration_location_angle(int room_idx,float ang,d3pnt *pnt)
{
	d3pnt			min,max,sz;
	float			fx,fz,rang;
	ag_room_type	*room;

		// get current room dimensions

	room=&ag_state.rooms[room_idx];
	map_mesh_calculate_extent(&map,room->mesh_idx,&min,&max);

		// room size and radius

	sz.x=max.x-min.x;
	sz.z=max.z-min.z;

	fx=((float)sz.x)*0.33f;
	fz=((float)sz.z)*0.33f;

		// get point

	pnt->x=(min.x+max.x)>>1;
	pnt->y=max.y;
	pnt->z=(min.z+max.z)>>1;

	rang=ang*ANG_to_RAD;

	pnt->x+=(int)(fx*sinf(rang));
	pnt->z-=(int)(fz*cosf(rang));
}


/* =======================================================

      Box Stack Decorations
      
======================================================= */

void ag_generate_decoration_box(d3pnt *pnt,int stack_offset,int start_cmp_mesh_idx)
{
	int				n,mesh_idx,x,z,sz;
	int				px[8],py[8],pz[8];
	float			gx[8],gy[8];
	bool			hit;
	d3pnt			mpt,min,max;
	d3ang			ang;

		// find a random start position
	
	hit=FALSE;
	sz=ag_size_stack_box>>1;

	for (n=0;n!=10;n++) {
		x=pnt->x+(ag_random_int(stack_offset>>1)-stack_offset);
		z=pnt->z+(ag_random_int(stack_offset>>1)-stack_offset);

		min.x=x-sz;
		max.x=x+sz;
		min.y=pnt->y-ag_size_stack_box;
		max.y=pnt->y;
		min.z=z-sz;
		max.z=z+sz;

		if (start_cmp_mesh_idx!=-1) {
			if (!ag_generate_mesh_collision(&min,&max,start_cmp_mesh_idx,map.mesh.nmesh)) {
				hit=TRUE;
				break;
			}
		}
	}

	if (!hit) return;

		// box mesh

	mesh_idx=map_mesh_add(&map);

	map.mesh.meshes[mesh_idx].flag.lock_uv=TRUE;

	gx[0]=gx[3]=0.0f;
	gx[1]=gx[2]=1.0f;
	gy[0]=gy[1]=0.0f;
	gy[2]=gy[3]=1.0f;

		// sides

	px[0]=px[1]=px[2]=px[3]=min.x;
	pz[0]=pz[3]=min.z;
	pz[1]=pz[2]=max.z;
	py[0]=py[1]=min.y;
	py[2]=py[3]=max.y;

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_decoration_box);

	px[0]=px[1]=px[2]=px[3]=max.x;

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_decoration_box);

	px[0]=px[3]=min.x;
	px[1]=px[2]=max.x;
	pz[0]=pz[1]=pz[2]=pz[3]=min.z;
	py[0]=py[1]=min.y;
	py[2]=py[3]=max.y;

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_decoration_box);

	pz[0]=pz[1]=pz[2]=pz[3]=max.z;

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_decoration_box);

		// top and bottom

	px[0]=px[3]=min.x;
	px[1]=px[2]=max.x;
	pz[0]=pz[1]=min.z;
	pz[2]=pz[3]=max.z;
	py[0]=py[1]=py[2]=py[3]=max.y;

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_decoration_box);

	py[0]=py[1]=py[2]=py[3]=min.y;

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_decoration_box);

		// possibly rotate box

	if (ag_random_bool()) {
		ang.x=ang.z=0.0f;
		ang.y=(float)ag_random_int(30);

		map_mesh_calculate_center(&map,mesh_idx,&mpt);
		map_mesh_rotate(&map,mesh_idx,&mpt,&ang);
	}

		// reset normals

	map_recalc_normals_mesh(&map,&map.mesh.meshes[mesh_idx],normal_mode_out,FALSE);
}

void ag_generate_decoration_box_stack(int room_idx,int cmp_mesh_idx)
{
	int				n,row_count,stack_offset;
	d3pnt			pnt;

		// get random stack count
		// and size

	row_count=ag_count_stack_box_row_start+ag_random_int(ag_count_stack_box_row_extra);
	stack_offset=ag_size_stack_box*(row_count+2);

		// randomize location
		// move it so within stack of boxes

	ag_generate_decoration_location_random(room_idx,&pnt);

	pnt.x+=(stack_offset>>1);
	pnt.z+=(stack_offset>>1);

		// box stack

	while (row_count>ag_count_stack_box_row_min) {

		for (n=0;n!=row_count;n++) {
			ag_generate_decoration_box(&pnt,stack_offset,cmp_mesh_idx);
		}

		row_count--;
		stack_offset-=ag_size_stack_box;

		pnt.y-=ag_size_stack_box;
	}
}

void ag_generate_decoration_box_stacks(int room_idx)
{
	int				n,box_stack_count,cmp_mesh_idx;
	
	box_stack_count=ag_count_stack_box_start+ag_random_int(ag_count_stack_box_extra);

	cmp_mesh_idx=map.mesh.nmesh;

	for (n=0;n!=box_stack_count;n++) {
		ag_generate_decoration_box_stack(room_idx,cmp_mesh_idx);
	}
}

/* =======================================================

      Column Decorations
      
======================================================= */

void ag_generate_decoration_column(d3pnt *pnt,float radius,int high)
{
	int				n,mesh_idx;
	int				px[4],py[4],pz[4];
	float			ang,next_ang,ang_add;
	float			gx[4],gy[4];

		// the mesh

	mesh_idx=map_mesh_add(&map);

		// column

	gx[0]=gx[3]=0.0f;
	gx[1]=gx[2]=1.0f;
	gy[0]=gy[1]=0.0f;
	gy[2]=gy[3]=1.0f;

	ang=0.0f;
	ang_add=(2.0f*TRIG_PI)/10.0f;

	for (n=0;n!=10;n++) {

		next_ang=ang+ang_add;
		if (n==9) next_ang=0.0f;

		px[0]=px[3]=pnt->x+(int)(radius*sinf(ang));
		px[1]=px[2]=pnt->x+(int)(radius*sinf(next_ang));
		pz[0]=pz[3]=pnt->z-(int)(radius*cosf(ang));
		pz[1]=pz[2]=pnt->z-(int)(radius*cosf(next_ang));

		py[0]=py[1]=pnt->y-high;
		py[2]=py[3]=pnt->y;

		map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,ag_texture_decoration_pillar);

			// exit early if we reconnect early

		ang=next_ang;
	}

		// reset normals

	map_recalc_normals_mesh(&map,&map.mesh.meshes[mesh_idx],normal_mode_out,FALSE);
}

void ag_generate_decoration_columns(int room_idx)
{
	int				n,high;
	float			radius;
	d3pnt			pnt;
	ag_room_type	*room;

		// get height

	high=ag_size_room_high+ag_size_floor_high;
	if (ag_state.rooms[room_idx].second_story) high+=ag_size_room_high;

		// create columns

	switch (ag_random_int(3)) {

		case ag_dectoration_column_config_square:
			radius=(float)ag_size_column_normal_wid;
			ag_generate_decoration_location_angle(room_idx,45.0f,&pnt);
			ag_generate_decoration_column(&pnt,radius,high);
			ag_generate_decoration_location_angle(room_idx,135.0f,&pnt);
			ag_generate_decoration_column(&pnt,radius,high);
			ag_generate_decoration_location_angle(room_idx,225.0f,&pnt);
			ag_generate_decoration_column(&pnt,radius,high);
			ag_generate_decoration_location_angle(room_idx,315.0f,&pnt);
			ag_generate_decoration_column(&pnt,radius,high);
			break;

		case ag_dectoration_column_config_diamond:
			radius=(float)ag_size_column_normal_wid;
			ag_generate_decoration_location_angle(room_idx,0.0f,&pnt);
			ag_generate_decoration_column(&pnt,radius,high);
			ag_generate_decoration_location_angle(room_idx,90.0f,&pnt);
			ag_generate_decoration_column(&pnt,radius,high);
			ag_generate_decoration_location_angle(room_idx,180.0f,&pnt);
			ag_generate_decoration_column(&pnt,radius,high);
			ag_generate_decoration_location_angle(room_idx,270.0f,&pnt);
			ag_generate_decoration_column(&pnt,radius,high);
			break;

		case ag_dectoration_column_config_vertex:
			radius=(float)ag_size_column_vertex_wid;

			room=&ag_state.rooms[room_idx];
			for (n=0;n!=room->nvertex;n++) {
				ag_generate_decoration_column(&room->vertexes[n],radius,high);
			}
			break;

	}
}

/* =======================================================

      Equipment Decorations
      
======================================================= */

void ag_gnerate_decoration_equipment_piece(d3pnt *pnt,int wid_x,int wid_z,int high,int texture_idx)
{
	int				mesh_idx,sx,sz;
	int				px[8],py[8],pz[8];
	float			gx[8],gy[8];

		// equipment mesh

	mesh_idx=map_mesh_add(&map);

	map.mesh.meshes[mesh_idx].flag.lock_uv=TRUE;

	gx[0]=gx[3]=0.0f;
	gx[1]=gx[2]=1.0f;
	gy[0]=gy[1]=0.0f;
	gy[2]=gy[3]=1.0f;

	sx=wid_x-ag_size_equipment_separate_width;
	sz=wid_z-ag_size_equipment_separate_width;

		// sides

	px[0]=px[1]=px[2]=px[3]=pnt->x;
	pz[0]=pz[3]=pnt->z;
	pz[1]=pz[2]=pnt->z+sz;
	py[0]=py[1]=pnt->y-high;
	py[2]=py[3]=pnt->y;

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,texture_idx);

	px[0]=px[1]=px[2]=px[3]=pnt->x+sx;

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,texture_idx);

	px[0]=px[3]=pnt->x;
	px[1]=px[2]=pnt->x+sx;
	pz[0]=pz[1]=pz[2]=pz[3]=pnt->z;

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,texture_idx);

	pz[0]=pz[1]=pz[2]=pz[3]=pnt->z+sz;

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,texture_idx);

		// top

	px[0]=px[3]=pnt->x;
	px[1]=px[2]=pnt->x+sx;
	pz[0]=pz[1]=pnt->z;
	pz[2]=pz[3]=pnt->z+sz;
	py[0]=py[1]=py[2]=py[3]=pnt->y-high;

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,texture_idx);

		// reset normals

	map_recalc_normals_mesh(&map,&map.mesh.meshes[mesh_idx],normal_mode_out,FALSE);
}

void ag_gnerate_decoration_equipment(int room_idx)
{
	int				x,z,high,count,
					wid_x,wid_z;
	d3pnt			pnt,start_pnt,min,max;
	ag_room_type	*room;

	room=&ag_state.rooms[room_idx];

		// get height

	high=ag_size_equipment_high_start+ag_random_int(ag_size_equipment_high_extra);

		// get equipment count

	count=ag_random_int(3)+1;
	
	map_mesh_calculate_extent(&map,room->mesh_idx,&min,&max);
	wid_x=(max.x-min.x)/(count*4);
	wid_z=(max.z-min.z)/(count*4);

	start_pnt.x=((min.x+max.x)>>1)-((wid_x*count)>>1);
	start_pnt.z=((min.z+max.z)>>1)-((wid_z*count)>>1);

		// pad
		
	pnt.x=start_pnt.x-ag_size_equipment_separate_width;
	pnt.y=max.y;
	pnt.z=start_pnt.z-ag_size_equipment_separate_width;
		
	ag_gnerate_decoration_equipment_piece(&pnt,((wid_x*count)+(ag_size_equipment_separate_width*2)),((wid_z*count)+(ag_size_equipment_separate_width*2)),ag_size_floor_high,ag_texture_connect);

		// create equipment

	for (x=0;x!=count;x++) {
		for (z=0;z!=count;z++) {
			pnt.x=start_pnt.x+(x*wid_x);
			pnt.y=max.y-ag_size_floor_high;
			pnt.z=start_pnt.z+(z*wid_z);

			ag_gnerate_decoration_equipment_piece(&pnt,wid_x,wid_z,high,ag_texture_equipment);
		}
	}
}


/* =======================================================

      Decorations
      
======================================================= */

void ag_generate_decorations_add(void)
{
	int				n;
	ag_room_type	*room;

	for (n=0;n!=ag_state.nroom;n++) {
		room=&ag_state.rooms[n];

		switch (ag_random_int(4)) {

			case ag_decoration_type_none:
				break;

			case ag_decoration_type_columns:
				ag_generate_decoration_columns(n);
				break;

			case ag_decoration_type_box_stacks:
				ag_generate_decoration_box_stacks(n);
				break;

			case ag_decoration_type_equipment:
				ag_gnerate_decoration_equipment(n);
				break;

		}

	}
}

/* =======================================================

      Add Lights
      
======================================================= */

void ag_generate_lights_add(void)
{
	int				n;
	d3pnt			min,max;
	map_light_type	*lit;
	ag_room_type	*room;

	for (n=0;n!=ag_state.nroom;n++) {

			// exit if we run out of lights

		if (map.nlight==max_map_light) return;

			// add the light

		lit=&map.lights[map.nlight];
		map.nlight++;

		sprintf(lit->name,"light %d",n);
			
			// set the light

		room=&ag_state.rooms[n];
		map_mesh_calculate_extent(&map,room->mesh_idx,&min,&max);

		lit->pnt.x=(min.x+max.x)>>1;
		lit->pnt.y=max.y-(ag_size_room_high-ag_size_floor_high);
		if (room->second_story) lit->pnt.y-=ag_size_room_high;
		lit->pnt.z=(min.z+max.z)>>1;
		
		lit->setting.on=TRUE;
		lit->setting.light_map=TRUE;
		lit->setting.halo_name[0]=0x0;

		lit->setting.type=lt_normal;
		lit->setting.direction=ld_all;

		lit->setting.intensity=ag_size_light_width_start+ag_random_int(ag_size_light_width_extra);
		lit->setting.exponent=1.0f;

		lit->setting.col.r=1.0f-(((float)ag_random_int(25))/100.0f);
		lit->setting.col.g=1.0f-(((float)ag_random_int(25))/100.0f);
		lit->setting.col.b=1.0f-(((float)ag_random_int(25))/100.0f);
	}
}

/* =======================================================

      Add Spots
      
======================================================= */

void ag_generate_spots_add_single(char *name,int spot_obj_type,char *script_name)
{
	int					room_idx;
	spot_type			*spot;

		// add the spot

	spot=&map.spots[map.nspot];
	map.nspot++;

		// randomize location

	room_idx=ag_random_int(ag_state.nroom);
	ag_generate_decoration_location_random(room_idx,&spot->pnt);

	spot->ang.x=0.0f;
	spot->ang.y=0.0f;
	spot->ang.z=0.0f;

		// name and type
	
	strcpy(spot->name,name);
	spot->type=spot_obj_type;
	strcpy(spot->script,script_name);
	spot->display_model[0]=0x0;
	spot->params[0]=0x0;
}

void ag_generate_spots_add(void)
{
		// player spot

	ag_generate_spots_add_single("Start",spot_type_player,"Player");

		// other spots

}
