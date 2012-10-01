#include "ray_os_defs.h"
#include "ray.h"
#include "ray_interface.h"

#include "png.h"

extern ray_global_type				ray_global;

/* =======================================================

      Find Material Index for ID
      
======================================================= */

int ray_material_get_index(int materialId)
{
	int						n;

	for (n=0;n!=ray_global.material_list.count;n++) {
		if (ray_global.material_list.materials[n]->id==materialId) return(n);
	}

	return(-1);
}

/* =======================================================

      Get Texture RGB
      
======================================================= */

void ray_get_material_rgb(ray_scene_type *scene,ray_collision_type *collision,ray_material_pixel_type *pixel)
{
	int						x,y,offset;
	unsigned long			buf;
	float					inv,fx,fy;
	ray_mesh_type			*mesh;
	ray_poly_type			*poly;
	ray_trig_type			*trig;
	ray_uv_type				*uv0,*uv1,*uv2;
	ray_vector_type			*n0,*n1,*n2;
	ray_material_type		*material;

		// get mesh/poly/trig and materials
		
	mesh=scene->mesh_list.meshes[collision->mesh_idx];
	poly=&mesh->poly_block.polys[collision->poly_idx];
	trig=&poly->trig_block.trigs[collision->trig_idx];
	
	material=ray_global.material_list.materials[poly->material_idx];
	
		// sanity check for bad materials
		
	if (material->color.data==NULL) {
		pixel->color.on=FALSE;
		pixel->normal.on=FALSE;
		pixel->specular.on=FALSE;
		pixel->reflection.on=FALSE;
		return;
	}
	
		// calculate the uv
		
	uv0=&mesh->uv_block.uvs[trig->uv_idx[0]];
	uv1=&mesh->uv_block.uvs[trig->uv_idx[1]];
	uv2=&mesh->uv_block.uvs[trig->uv_idx[2]];
		
	inv=(1-collision->u)-collision->v;
	fx=(inv*uv0->x)+(collision->u*uv1->x)+(collision->v*uv2->x);
	fy=(inv*uv0->y)+(collision->u*uv1->y)+(collision->v*uv2->y);
	
		// calculate the surface normal
		
	n0=&mesh->normal_block.normals[trig->normal_idx[0]];
	n1=&mesh->normal_block.normals[trig->normal_idx[1]];
	n2=&mesh->normal_block.normals[trig->normal_idx[2]];
		
	pixel->surface_normal.x=(inv*n0->x)+(collision->u*n1->x)+(collision->v*n2->x);
	pixel->surface_normal.y=(inv*n0->y)+(collision->u*n1->y)+(collision->v*n2->y);
	pixel->surface_normal.z=(inv*n0->z)+(collision->u*n1->z)+(collision->v*n2->z);
	
	ray_vector_normalize(&pixel->surface_normal);
		
		// change to texture coordinate
		
	x=(int)(fx*material->wid_scale);
	y=(int)(fy*material->high_scale);
	
	x=x&material->wid_mask;
	y=y&material->high_mask;
	
	offset=(material->wid*y)+x;
	
		// get color

	pixel->color.on=TRUE;
	buf=*(((unsigned long*)material->color.data)+offset);
	ray_create_float_color_from_ulong_no_alpha(buf,&pixel->color.rgb);
	
		// get normal
		
	if (material->normal.data==NULL) {
		pixel->normal.on=FALSE;
	}
	else {
		pixel->normal.on=TRUE;
		buf=*(((unsigned long*)material->normal.data)+offset);
		ray_create_float_color_from_ulong_no_alpha(buf,&pixel->normal.rgb);
	}
	
		// get specular
		
	if (material->specular.data==NULL) {
		pixel->specular.on=FALSE;
	}
	else {
		pixel->specular.on=TRUE;
		pixel->shine_factor=material->shine_factor;
		buf=*(((unsigned long*)material->specular.data)+offset);
		ray_create_float_color_from_ulong_no_alpha(buf,&pixel->specular.rgb);
	}
}

/* =======================================================

      Adds a New Material with No Attachments

	  Returns:
	   If >=0, then a material ID
	   RL_ERROR_OUT_OF_MEMORY
	   RL_ERROR_UNKNOWN_TARGET
	   RL_ERROR_UNKNOWN_FORMAT
      
======================================================= */

int rlMaterialAdd(int wid,int high,unsigned long flags)
{
	ray_material_type		*material;
	
		// add material

	material=(ray_material_type*)malloc(sizeof(ray_material_type));
	if (material==NULL) return(RL_ERROR_OUT_OF_MEMORY);
		
		// setup

	material->wid=wid;
	material->high=high;
	
	material->wid_scale=(float)wid;
	material->high_scale=(float)high;
	
	material->wid_mask=wid-1;
	material->high_mask=high-1;

	material->row_bytes=wid<<2;
	
		// start with no attachments

	material->color.data=NULL;
	material->normal.data=NULL;
	material->specular.data=NULL;
	material->reflection.data=NULL;

		// set id

	material->id=ray_global.material_list.next_id;
	ray_global.material_list.next_id++;

		// add to list

	ray_global.material_list.materials[ray_global.material_list.count]=material;
	ray_global.material_list.count++;
	
	return(material->id);
}

/* =======================================================

      Delete Material

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_MATERIAL_ID
	   RL_ERROR_MATERIAL_ATTACHED_TO_MESH
	   RL_ERROR_MATERIAL_ATTACHED_TO_OVERLAY
      
======================================================= */

int rlMaterialDelete(int materialId)
{
	int						n,k,t,idx,count;
	ray_mesh_type			*mesh;
	ray_poly_type			*poly;
	ray_material_type		*material;
	ray_scene_type			*scene;

		// get material

	idx=ray_material_get_index(materialId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_MATERIAL_ID);

	material=ray_global.material_list.materials[idx];

		// check to see if held by a mesh
		// or overlay

	for (n=0;n!=ray_global.scene_list.count;n++) {
		scene=ray_global.scene_list.scenes[n];

		for (k=0;k!=scene->mesh_list.count;k++) {
			mesh=scene->mesh_list.meshes[k];

			poly=mesh->poly_block.polys;			// don't need to parent, we will check the parents as we get them

			for (t=0;t!=mesh->poly_block.count;t++) {
				if (poly->material_idx==idx) return(RL_ERROR_MATERIAL_ATTACHED_TO_MESH);
				poly++;
			}
		}
		
		for (k=0;k!=scene->overlay_list.count;k++) {
			if (scene->overlay_list.overlays[k]->material_idx==idx) return(RL_ERROR_MATERIAL_ATTACHED_TO_OVERLAY);
		}
	}

		// clear buffers

	if (material->color.data!=NULL) free(material->color.data);
	if (material->normal.data!=NULL) free(material->normal.data);
	if (material->specular.data!=NULL) free(material->specular.data);
	if (material->reflection.data!=NULL) free(material->reflection.data);

		// remove material

	free(material);

	count=ray_global.material_list.count-2;

	for (n=idx;n<=count;n++) {
		ray_global.material_list.materials[n]=ray_global.material_list.materials[n+1];
	}

	ray_global.material_list.count--;

	return(RL_ERROR_OK);
}

/* =======================================================

      Attach a Material Target from a Memory Block

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_MATERIAL_ID
	   RL_ERROR_OUT_OF_MEMORY
	   RL_ERROR_UNKNOWN_TARGET
	   RL_ERROR_UNKNOWN_FORMAT
      
======================================================= */

int rlMaterialAttachBufferData(int materialId,int target,int format,unsigned char* data)
{
	int						n,pixel_cnt,idx,sz;
	unsigned char			*rgba_data,*rptr,*dptr;
	ray_material_type		*material;

		// get material

	idx=ray_material_get_index(materialId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_MATERIAL_ID);

	material=ray_global.material_list.materials[idx];

		// validate target and format

	if ((target!=RL_MATERIAL_TARGET_COLOR) && (target!=RL_MATERIAL_TARGET_NORMAL) && (target!=RL_MATERIAL_TARGET_SPECULAR) && (target!=RL_MATERIAL_TARGET_REFLECTION)) return(RL_ERROR_UNKNOWN_TARGET);
	if ((format!=RL_MATERIAL_FORMAT_32_RGBA) && (format!=RL_MATERIAL_FORMAT_24_RGB)) return(RL_ERROR_UNKNOWN_FORMAT);

		// memory for rgba data
		
	sz=(material->wid*material->high)<<2;
	
	rgba_data=malloc(sz);
	if (rgba_data==NULL) return(RL_ERROR_OUT_OF_MEMORY);
	
		// all materials are in 32 bit RGBA
		// if material is not that format, than
		// we need to translate
		
	if (format==RL_MATERIAL_FORMAT_32_RGBA) {
		memmove(rgba_data,data,sz);
	}
	else {
	
		rptr=rgba_data;
		dptr=data;
		
		pixel_cnt=material->wid*material->high;
		
		for (n=0;n!=pixel_cnt;n++) {
			*rptr++=*dptr++;
			*rptr++=*dptr++;
			*rptr++=*dptr++;
			*rptr++=0xFF;
		}
	}
		
		// save new material target
	
	switch (target) {
	
		case RL_MATERIAL_TARGET_COLOR:
			material->color.data=rgba_data;
			break;
			
		case RL_MATERIAL_TARGET_NORMAL:
			material->normal.data=rgba_data;
			break;
			
		case RL_MATERIAL_TARGET_SPECULAR:
			material->specular.data=rgba_data;
			break;
			
		case RL_MATERIAL_TARGET_REFLECTION:
			material->reflection.data=rgba_data;
			break;
			
	}
	
	return(RL_ERROR_OK);
}

/* =======================================================

      Attach a Material Target from a Solid Color

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_MATERIAL_ID
	   RL_ERROR_OUT_OF_MEMORY
      
======================================================= */

int rlMaterialAttachBufferColor(int materialId,int target,ray_color_type *col)
{
	int						n,sz,idx,err;
	unsigned long			pixel;
	unsigned long			*pd,*pixel_data;
				ray_material_type		*material;

		// get material

	idx=ray_material_get_index(materialId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_MATERIAL_ID);

	material=ray_global.material_list.materials[idx];

		// create the color block

	sz=material->wid*material->high;

	pixel_data=(unsigned long*)malloc(sz<<2);
	if (pixel_data==NULL) return(RL_ERROR_OUT_OF_MEMORY);

	pd=pixel_data;
	
	pixel=ray_create_ulong_color_from_float(col);

	for (n=0;n!=sz;n++) {
		*pd++=pixel;
	}

		// attach to material

	err=rlMaterialAttachBufferData(materialId,target,RL_MATERIAL_FORMAT_32_RGBA,(unsigned char*)pixel_data);

	free(pixel_data);

	return(err);
}

/* =======================================================

      Sets the Shine Factor for Speculars

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_MATERIAL_ID
      
======================================================= */

int rlMaterialSetShineFactor(int materialId,float shineFactor)
{
	int						idx;
	ray_material_type		*material;

		// get material

	idx=ray_material_get_index(materialId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_MATERIAL_ID);

	material=ray_global.material_list.materials[idx];
	material->shine_factor=shineFactor;
	
	return(RL_ERROR_OK);
}

