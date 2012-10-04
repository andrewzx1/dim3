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

void ray_get_material_rgb(ray_scene_type *scene,ray_point_type *eye_pnt,ray_point_type *trig_pnt,ray_collision_type *collision,ray_material_pixel_type *pixel)
{
	int							x,y,offset,mm_level;
	unsigned long				buf;
	float						inv,fx,fy,mm_dist,mm_fact;
	ray_mesh_type				*mesh;
	ray_poly_type				*poly;
	ray_trig_type				*trig;
	ray_uv_type					*uv0,*uv1,*uv2;
	ray_vector_type				*n0,*n1,*n2;
	ray_material_type			*material;
	ray_material_mipmap_type	*mipmap;

		// get mesh/poly/trig and materials
		
	mesh=scene->mesh_list.meshes[collision->mesh_idx];
	poly=&mesh->poly_block.polys[collision->poly_idx];
	trig=&poly->trig_block.trigs[collision->trig_idx];
	
	material=ray_global.material_list.materials[poly->material_idx];

		// get the mipmap level
		// we have an exponential factor
		// here so we can make a better than
		// linear mipmapping drop off

	mm_dist=ray_distance_between_points(eye_pnt,trig_pnt);
	mm_fact=1.0f-(mm_dist/scene->eye.max_dist);

	mm_fact=1.0f-(mm_fact*powf(mm_fact,ray_mipmap_distance_exponent));

	mm_level=(int)(((float)(material->mipmap_list.count+1))*mm_fact);
	if (mm_level<0) mm_level=0;
	if (mm_level>=material->mipmap_list.count) mm_level=material->mipmap_list.count-1;

	mipmap=&material->mipmap_list.mipmaps[mm_level];
	
		// sanity check for bad materials
		
	if (mipmap->data.color==NULL) {
		pixel->color.on=FALSE;
		pixel->normal.on=FALSE;
		pixel->specular.on=FALSE;
		pixel->reflection.on=FALSE;
		return;
	}
	
		// calculate the uv
		
	uv0=&mesh->uv_block.uvs[trig->idxs[0].uv];
	uv1=&mesh->uv_block.uvs[trig->idxs[1].uv];
	uv2=&mesh->uv_block.uvs[trig->idxs[2].uv];
		
	inv=(1-collision->u)-collision->v;
	fx=(inv*uv0->x)+(collision->u*uv1->x)+(collision->v*uv2->x);
	fy=(inv*uv0->y)+(collision->u*uv1->y)+(collision->v*uv2->y);
	
		// calculate the surface normal
		
	n0=&mesh->normal_block.normals[trig->idxs[0].normal];
	n1=&mesh->normal_block.normals[trig->idxs[1].normal];
	n2=&mesh->normal_block.normals[trig->idxs[2].normal];
		
	pixel->surface.normal.x=(inv*n0->x)+(collision->u*n1->x)+(collision->v*n2->x);
	pixel->surface.normal.y=(inv*n0->y)+(collision->u*n1->y)+(collision->v*n2->y);
	pixel->surface.normal.z=(inv*n0->z)+(collision->u*n1->z)+(collision->v*n2->z);
	
	ray_vector_normalize(&pixel->surface.normal);

		// calculate the surface tangent
		
	n0=&mesh->tangent_block.tangents[trig->idxs[0].tangent];
	n1=&mesh->tangent_block.tangents[trig->idxs[1].tangent];
	n2=&mesh->tangent_block.tangents[trig->idxs[2].tangent];
		
	pixel->surface.tangent.x=(inv*n0->x)+(collision->u*n1->x)+(collision->v*n2->x);
	pixel->surface.tangent.y=(inv*n0->y)+(collision->u*n1->y)+(collision->v*n2->y);
	pixel->surface.tangent.z=(inv*n0->z)+(collision->u*n1->z)+(collision->v*n2->z);
	
	ray_vector_normalize(&pixel->surface.tangent);

		// calculate the binormal

	ray_vector_cross_product(&pixel->surface.binormal,&pixel->surface.normal,&pixel->surface.tangent);
	ray_vector_normalize(&pixel->surface.binormal);

		// change to texture coordinate
		
	fx-=floorf(fx);
	x=(int)(fx*mipmap->wid_scale);

	fy-=floorf(fy);
	y=(int)(fy*mipmap->high_scale);
	
	offset=(mipmap->wid*y)+x;
	
		// get color

	pixel->color.on=TRUE;
	buf=*(((unsigned long*)mipmap->data.color)+offset);
	ray_create_float_color_from_ulong_no_alpha(buf,&pixel->color.rgb);
	
		// get normal
		
	if (mipmap->data.normal==NULL) {
		pixel->normal.on=FALSE;
	}
	else {
		pixel->normal.on=TRUE;
		buf=*(((unsigned long*)mipmap->data.normal)+offset);
		ray_create_float_color_from_ulong_no_alpha(buf,&pixel->normal.rgb);
	}
	
		// get specular
		
	if (mipmap->data.specular==NULL) {
		pixel->specular.on=FALSE;
	}
	else {
		pixel->specular.on=TRUE;
		pixel->shine_factor=material->shine_factor;
		buf=*(((unsigned long*)mipmap->data.specular)+offset);
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
	int							n;
	ray_material_type			*material;
	ray_material_mipmap_type	*mipmap;
	
		// add material

	material=(ray_material_type*)malloc(sizeof(ray_material_type));
	if (material==NULL) return(RL_ERROR_OUT_OF_MEMORY);
		
		// no mipmaps yet

	material->mipmap_list.count=0;

		// original wid/high

	material->wid=wid;
	material->high=high;

		// start with no attachments

	mipmap=material->mipmap_list.mipmaps;

	for (n=0;n!=ray_mipmap_max_level;n++) {
		mipmap->data.color=NULL;
		mipmap->data.normal=NULL;
		mipmap->data.specular=NULL;
		mipmap->data.reflection=NULL;
		mipmap++;
	}

		// rendering setup

	material->shine_factor=1.0f;

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
	int							n,k,t,idx,count;
	ray_mesh_type				*mesh;
	ray_poly_type				*poly;
	ray_material_type			*material;
	ray_material_mipmap_type	*mipmap;
	ray_scene_type				*scene;

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

	mipmap=material->mipmap_list.mipmaps;

	for (n=0;n!=material->mipmap_list.count;n++) {
		if (mipmap->data.color!=NULL) free(mipmap->data.color);
		if (mipmap->data.normal!=NULL) free(mipmap->data.normal);
		if (mipmap->data.specular!=NULL) free(mipmap->data.specular);
		if (mipmap->data.reflection!=NULL) free(mipmap->data.reflection);
		mipmap++;
	}

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
	int							n,pixel_cnt,idx,sz;
	unsigned char				*rgba_data,*rptr,*dptr;
	ray_material_type			*material;
	ray_material_mipmap_type	*mipmap;

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

		// any buffer attachment clears
		// all mipmap data

	mipmap=&material->mipmap_list.mipmaps[1];

	for (n=1;n!=ray_mipmap_max_level;n++) {
		if (mipmap->data.color!=NULL) free(mipmap->data.color);
		if (mipmap->data.normal!=NULL) free(mipmap->data.normal);
		if (mipmap->data.specular!=NULL) free(mipmap->data.specular);
		if (mipmap->data.reflection!=NULL) free(mipmap->data.reflection);
		mipmap++;
	}

	material->mipmap_list.count=1;

		// attachments are always mipmap
		// level 0

	mipmap=&material->mipmap_list.mipmaps[0];

		// build the UV texturing data

	mipmap->wid=material->wid;
	mipmap->high=material->high;
	
	mipmap->wid_scale=(float)mipmap->wid;
	mipmap->high_scale=(float)mipmap->high;
		
		// save new material target
	
	switch (target) {
	
		case RL_MATERIAL_TARGET_COLOR:
			if (mipmap->data.color!=NULL) free(mipmap->data.color);
			mipmap->data.color=rgba_data;
			break;
			
		case RL_MATERIAL_TARGET_NORMAL:
			if (mipmap->data.normal!=NULL) free(mipmap->data.normal);
			mipmap->data.normal=rgba_data;
			break;
			
		case RL_MATERIAL_TARGET_SPECULAR:
			if (mipmap->data.specular!=NULL) free(mipmap->data.specular);
			mipmap->data.specular=rgba_data;
			break;
			
		case RL_MATERIAL_TARGET_REFLECTION:
			if (mipmap->data.reflection!=NULL) free(mipmap->data.reflection);
			mipmap->data.reflection=rgba_data;
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

		// set the shine

	material->shine_factor=shineFactor;
	
	return(RL_ERROR_OK);
}

/* =======================================================

      Builds Mipmaps For Material

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_MATERIAL_ID
	   RL_ERROR_OUT_OF_MEMORY
      
======================================================= */

int rlMaterialBuildMipMaps(int materialId)
{
	int							idx,factor;
	ray_material_type			*material;
	ray_material_mipmap_type	*mipmap;

		// get material

	idx=ray_material_get_index(materialId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_MATERIAL_ID);

	material=ray_global.material_list.materials[idx];

		// build the mipmaps

	factor=2;

	while (TRUE) {
			
			// have we reached the smallest
			// mipmap yet?

		if ((material->wid/factor)<2) break;
		if ((material->high/factor)<2) break;
		
			// new mipmap

		mipmap=&material->mipmap_list.mipmaps[material->mipmap_list.count];

		mipmap->wid=material->wid/factor;
		mipmap->high=material->high/factor;
		
		mipmap->wid_scale=(float)mipmap->wid;
		mipmap->high_scale=(float)mipmap->high;

			// color data

		mipmap->data.color=ray_bitmap_reduction(factor,material->wid,material->high,material->mipmap_list.mipmaps[0].data.color);
		if (mipmap->data.color==NULL) return(RL_ERROR_OUT_OF_MEMORY);

		if (material->mipmap_list.mipmaps[0].data.normal!=NULL) {
			mipmap->data.normal=ray_bitmap_reduction(factor,material->wid,material->high,material->mipmap_list.mipmaps[0].data.normal);
			if (mipmap->data.normal==NULL) {
				free(mipmap->data.color);
				return(RL_ERROR_OUT_OF_MEMORY);
			}
		}

		if (material->mipmap_list.mipmaps[0].data.specular!=NULL) {
			mipmap->data.specular=ray_bitmap_reduction(factor,material->wid,material->high,material->mipmap_list.mipmaps[0].data.specular);
			if (mipmap->data.specular==NULL) {
				free(mipmap->data.color);
				if (mipmap->data.normal!=NULL) free(mipmap->data.normal);
				return(RL_ERROR_OUT_OF_MEMORY);
			}
		}

		if (material->mipmap_list.mipmaps[0].data.reflection!=NULL) {
			mipmap->data.reflection=ray_bitmap_reduction(factor,material->wid,material->high,material->mipmap_list.mipmaps[0].data.reflection);
			if (mipmap->data.reflection==NULL) {
				free(mipmap->data.color);
				if (mipmap->data.normal!=NULL) free(mipmap->data.normal);
				if (mipmap->data.specular!=NULL) free(mipmap->data.specular);
				return(RL_ERROR_OUT_OF_MEMORY);
			}
		}

			// completed a mipmap, move up the levels

		factor*=2;
		material->mipmap_list.count++;
		if (material->mipmap_list.count>=ray_mipmap_max_level) break;
	}

	return(RL_ERROR_OK);
}

