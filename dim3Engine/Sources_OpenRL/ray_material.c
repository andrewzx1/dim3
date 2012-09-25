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

		// get mesh/trig and materials
		
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
	
		// calcualte the surface normal
		
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

			poly=mesh->poly_block.polys;

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
	int						idx,sz;
	unsigned char			*rgb_data;
	ray_material_type		*material;

		// get material

	idx=ray_material_get_index(materialId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_MATERIAL_ID);

	material=ray_global.material_list.materials[idx];

		// validate target and format

	if ((target!=RL_MATERIAL_BUFFER_COLOR) && (target!=RL_MATERIAL_BUFFER_NORMAL) && (target!=RL_MATERIAL_BUFFER_SPECULAR) && (target!=RL_MATERIAL_BUFFER_REFLECTION)) return(RL_ERROR_UNKNOWN_TARGET);
	if (format!=RL_MATERIAL_FORMAT_32_RGBA) return(RL_ERROR_UNKNOWN_FORMAT);
		
		// save new material target
		
	sz=(material->wid*material->high)<<2;
	
	rgb_data=malloc(sz);
	if (rgb_data==NULL) return(RL_ERROR_OUT_OF_MEMORY);
	memmove(rgb_data,data,sz);
	
	switch (target) {
	
		case RL_MATERIAL_BUFFER_COLOR:
			material->color.data=rgb_data;
			break;
			
		case RL_MATERIAL_BUFFER_NORMAL:
			material->normal.data=rgb_data;
			break;
			
		case RL_MATERIAL_BUFFER_SPECULAR:
			material->specular.data=rgb_data;
			break;
			
		case RL_MATERIAL_BUFFER_REFLECTION:
			material->reflection.data=rgb_data;
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

int rlMaterialAttachBufferColor(int materialId,int target,float r,float g,float b)
{
	int						n,sz,idx,err;
	unsigned long			pixel;
	unsigned long			*pd,*pixel_data;
	ray_color_type			col;
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
	
	col.r=r;
	col.g=g;
	col.b=b;
	col.a=1.0f;
	pixel=ray_create_ulong_color_from_float(&col);

	for (n=0;n!=sz;n++) {
		*pd++=pixel;
	}

		// attach to material

	err=rlMaterialAttachBufferData(materialId,target,RL_MATERIAL_FORMAT_32_RGBA,(unsigned char*)pixel_data);

	free(pixel_data);

	return(err);
}

/* =======================================================

      PNG IO Replacements
      
======================================================= */

void rlMaterialPNGfRead(png_structp png_ptr,png_bytep data,png_size_t length)
{
	FILE			*file;

	file=(FILE*)png_get_io_ptr(png_ptr);
	fread(data,1,length,file);
}

/* =======================================================

      Attach a Material Target from a PNG File

	  Returns:
	   RL_ERROR_OK
	   RL_ERROR_UNKNOWN_MATERIAL_ID
	   RL_ERROR_FILE_NOT_FOUND
	   RL_ERROR_OUT_OF_MEMORY
	   RL_ERROR_FILE_PNG_BAD_HEADER
	   RL_ERROR_FILE_PNG_LESS_24_BITS
	   RL_ERROR_FILE_PNG_WRONG_WIDTH_HEIGHT
      
======================================================= */

int rlMaterialAttachBufferPNG(int materialId,int target,char *path)
{
	int						x,y,k,idx,bit_depth,err,
							rowbytes,channels,wid,high;
	unsigned char			header[8];
	unsigned char			*pixel_data,*ptr,*rp;
	FILE					*file;
	png_structp				png_ptr;
	png_infop				info_ptr;
	png_bytep				*rptrs;
	ray_material_type		*material;

		// get material

	idx=ray_material_get_index(materialId);
	if (idx==-1) return(RL_ERROR_UNKNOWN_MATERIAL_ID);

	material=ray_global.material_list.materials[idx];

		// open file

	file=fopen(path,"rb");
	if (file==NULL) return(RL_ERROR_FILE_NOT_FOUND);
	
	fread(header,1,8,file);
	if (png_sig_cmp(header,0,8)) return(RL_ERROR_FILE_PNG_BAD_HEADER);
	
		// get info
		
	png_ptr=png_create_read_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);
	if (png_ptr==NULL) {
		fclose(file);
		return(RL_ERROR_OUT_OF_MEMORY);
	}
	
	info_ptr=png_create_info_struct(png_ptr);
	if (info_ptr==NULL) {
		png_destroy_read_struct(&png_ptr,NULL,NULL);
		fclose(file);
		return(RL_ERROR_OUT_OF_MEMORY);
	}
	
	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
		fclose(file);
		return(RL_ERROR_OUT_OF_MEMORY);
	}
	
		// we have to force libPNG to call back
		// for fread and fwrite because changes
		// in the libraries in win32 cause all sorts of havoc

	png_set_read_fn(png_ptr,file,rlMaterialPNGfRead);
	png_set_sig_bytes(png_ptr,8);
	
	png_read_info(png_ptr,info_ptr);
	
	wid=png_get_image_width(png_ptr,info_ptr);
	high=png_get_image_height(png_ptr,info_ptr);
	rowbytes=(int)png_get_rowbytes(png_ptr,info_ptr);
	channels=png_get_channels(png_ptr,info_ptr);
	bit_depth=png_get_bit_depth(png_ptr,info_ptr)*channels;

		// check for width/height with material

	if ((wid!=material->wid) || (high!=material->high)) {
		png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
		fclose(file);
		return(RL_ERROR_FILE_PNG_WRONG_WIDTH_HEIGHT);
	}

		// check for at least 24 bits

	if ((bit_depth!=24) && (bit_depth!=32)) {
		png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
		fclose(file);
		return(RL_ERROR_FILE_PNG_LESS_24_BITS);
	}

		// create the bitmap
		
	pixel_data=malloc((wid<<2)*high);
	if (pixel_data==NULL) {
		png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
		fclose(file);
		return(RL_ERROR_OUT_OF_MEMORY);
	}

		// read the file

	png_set_interlace_handling(png_ptr);
	png_read_update_info(png_ptr,info_ptr);
		
	rptrs=(png_bytep*)malloc(sizeof(png_bytep)*high);
	if (rptrs==NULL) {
		png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
		fclose(file);
		return(RL_ERROR_OUT_OF_MEMORY);
	}
	
	for (y=0;y!=high;y++) {
		rptrs[y]=(png_byte*)malloc(rowbytes);
		if (rptrs[y]==NULL) {
			for (k=0;k<y;k++) {
				free(rptrs[y]);
			}
			png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
			free(rptrs);
			fclose(file);
			return(RL_ERROR_OUT_OF_MEMORY);
		}
	}
	
	png_read_image(png_ptr,rptrs);
	
		// translate
		
	ptr=pixel_data;
	
	if (channels==4) {
	
			// RGBA
			
		for (y=0;y!=high;y++) {
			memmove(ptr,rptrs[y],rowbytes);
			ptr+=rowbytes;
		}
	}
	else {
	
			// RGB
			
		for (y=0;y!=high;y++) {
			rp=rptrs[y];
			for (x=0;x!=wid;x++) {
				*ptr++=*rp++;
				*ptr++=*rp++;
				*ptr++=*rp++;
				*ptr++=0xFF;
			}
		}
	}

		// clean up
		
	for (y=0;y!=high;y++) {
		free(rptrs[y]);
	}
	
	free(rptrs);
		
	png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
	fclose(file);
	
		// attach to material
	
	err=rlMaterialAttachBufferData(materialId,target,RL_MATERIAL_FORMAT_32_RGBA,pixel_data);

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

