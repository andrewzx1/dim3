/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: OpenGL Textures

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
	#include "dim3baseutility.h"
#endif

/* =======================================================

      Set Texture Properties
      
======================================================= */

void bitmap_texture_set_anisotropic_mode(int gl_bindtype,int anisotropic_mode)
{
	switch (anisotropic_mode) {
	
		case anisotropic_mode_none:
			glTexParameterf(gl_bindtype,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.0);
			break;
			
		case anisotropic_mode_low:
			glTexParameterf(gl_bindtype,GL_TEXTURE_MAX_ANISOTROPY_EXT,2.0);
			break;
			
		case anisotropic_mode_medium:
			glTexParameterf(gl_bindtype,GL_TEXTURE_MAX_ANISOTROPY_EXT,4.0);
			break;
			
		case anisotropic_mode_high:
			glTexParameterf(gl_bindtype,GL_TEXTURE_MAX_ANISOTROPY_EXT,8.0);
			break;
			
	}
}

void bitmap_texture_set_mipmap_filter(int gl_bindtype,int mipmap_mode,bool pixelated)
{
		// pixelated textures are always nearest
		
	if (pixelated) {
		glTexParameterf(gl_bindtype,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexParameterf(gl_bindtype,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		return;
	}
	
		// no mipmap mode for opengles
		
#ifdef D3_OPENGL_ES
	mipmap_mode=mipmap_mode_none;
#endif
			
		// regular mipmap modes
		
	switch (mipmap_mode) {
	
		case mipmap_mode_none:
			glTexParameterf(gl_bindtype,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			glTexParameterf(gl_bindtype,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			break;
			
		case mipmap_mode_bilinear:
			glTexParameterf(gl_bindtype,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
			glTexParameterf(gl_bindtype,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			break;
			
		case mipmap_mode_trilinear:
			glTexParameterf(gl_bindtype,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
			glTexParameterf(gl_bindtype,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			break;
			
	}
}

/* =======================================================

      Open OpenGL Textures
      
======================================================= */

bool bitmap_texture_open(bitmap_type *bitmap,unsigned char *data,int anisotropic_mode,int mipmap_mode,bool compress,bool rectangle,bool pixelated)
{
	int					gl_txtformat,gl_txttype,gl_bindtype;
	GLuint				gl_id;
	
		// if no bitmap data then no texture
		
	if (data==NULL) return(FALSE);

		// bind the texture
		// opengl es doesn't support rectangular textures
		
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1,&gl_id);
		
#ifndef D3_OPENGL_ES
	gl_bindtype=rectangle?GL_TEXTURE_RECTANGLE_ARB:GL_TEXTURE_2D;
#else
	gl_bindtype=GL_TEXTURE_2D;
#endif

	glBindTexture(gl_bindtype,gl_id);

		// storage and settings
	
	bitmap_texture_set_mipmap_filter(gl_bindtype,mipmap_mode,pixelated);
	bitmap_texture_set_anisotropic_mode(gl_bindtype,anisotropic_mode);
	
		// texture type
		// opengl es doesn't support compression

#ifndef D3_OPENGL_ES
	if (bitmap->alpha_mode==alpha_mode_none) {
		gl_txttype=GL_RGB;
		gl_txtformat=compress?GL_COMPRESSED_RGB:GL_RGB;
	}
	else {
		gl_txttype=GL_RGBA;
		gl_txtformat=compress?GL_COMPRESSED_RGBA:GL_RGBA;
	}
#else
	if (bitmap->alpha_mode==alpha_mode_none) {
		gl_txttype=gl_txtformat=GL_RGB;
	}
	else {
		gl_txttype=gl_txtformat=GL_RGBA;
	}
#endif

		// load texture

	glTexImage2D(gl_bindtype,0,gl_txtformat,bitmap->wid,bitmap->high,0,gl_txttype,GL_UNSIGNED_BYTE,data);

	if ((mipmap_mode!=mipmap_mode_none) && (!rectangle) && (!pixelated)) {
		glHint(GL_GENERATE_MIPMAP_HINT,GL_NICEST);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

		// set to bitmap
		
	bitmap->gl_id=gl_id;

		// unbind

	glBindTexture(gl_bindtype,0);

	return(TRUE);
}

/* =======================================================

      Close OpenGL Textures
      
======================================================= */

void bitmap_texture_close(bitmap_type *bitmap)
{
	GLuint			texture;
	
	texture=bitmap->gl_id;
	glDeleteTextures(1,&texture);
}

/* =======================================================

      Setup Textures
      
======================================================= */

void bitmap_texture_set_glow(texture_type *texture,int tick)
{
	float			glow_tick,glow_half_tick;

	if (texture->glow.rate<=0) {
		texture->glow.current_color=texture->glow.max;
		return;
	}

	glow_tick=(float)(tick%texture->glow.rate);
	glow_half_tick=(float)(texture->glow.rate>>1);
	
	if (glow_tick<glow_half_tick) {
		texture->glow.current_color=glow_tick/(glow_half_tick);
	}
	else {
		texture->glow.current_color=1.0f-((glow_tick-glow_half_tick)/glow_half_tick);
	}
	
	texture->glow.current_color=texture->glow.min+(texture->glow.current_color*(texture->glow.max-texture->glow.min));
}

int bitmap_texture_get_current_frame(texture_type *texture,int tick)
{
	int				n;
	
	if (texture->animate.total_wait==0) return(0);
	
	tick=tick%texture->animate.total_wait;

	for (n=0;n!=max_texture_frame;n++) {
		if (tick<texture->animate.end_tick[n]) return(n);
	}

	return(0);
}

void bitmap_texture_setup_animation(texture_type *texture,int texture_count,int tick)
{
	int				n;
	
	for (n=0;n!=texture_count;n++) {
	
		if (texture->frames[0].bitmap.gl_id==-1) {
			texture++;
			continue;
		}

			// set the texture frame

		if (texture->animate.on) {
			texture->animate.current_frame=bitmap_texture_get_current_frame(texture,tick);
		}
		else {
			texture->animate.current_frame=0;
		}
		
			// set the glow

		if (texture->frames[0].glowmap.gl_id!=-1) bitmap_texture_set_glow(texture,tick);
		
		texture++;
	}
}

/* =======================================================

      Recalc Texture Animation Numbers
      
======================================================= */

void bitmap_texture_recalc_animation(texture_type *texture)
{
	int				n;
	
	texture->animate.current_frame=0;
	texture->animate.total_wait=0;
		
	for (n=0;n!=max_texture_frame;n++) {
		if (texture->frames[n].bitmap.gl_id!=-1) texture->animate.end_tick[n]=texture->animate.total_wait=texture->animate.total_wait+texture->animate.wait[n];
	}
}

/* =======================================================

      Clear Texture
      
======================================================= */

void bitmap_texture_clear(texture_type *texture)
{
	int						n;
	texture_frame_type		*frame;
	
	texture->shader_idx=-1;
	strcpy(texture->shader_name,"Default");

	texture->shine_factor=10.0f;
	texture->additive=FALSE;
	texture->pixelated=FALSE;
	texture->compress=FALSE;
	texture->keep_quality=FALSE;

	texture->material_name[0]=0x0;

	texture->animate.on=FALSE;
	texture->glow.rate=100;
	texture->glow.min=0.0f;
	texture->glow.max=1.0f;

	texture->scale.on=FALSE;
	texture->scale.lock_offset=FALSE;
	texture->scale.x=texture->scale.y=0.04f;
    
	frame=texture->frames;

    for (n=0;n!=max_texture_frame;n++) {
		bitmap_new(&frame->bitmap);
		bitmap_new(&frame->bumpmap);
		bitmap_new(&frame->specularmap);
		bitmap_new(&frame->glowmap);

		texture->animate.wait[n]=100;
		
		frame++;
    }
}

/* =======================================================

      Textures XML
      
======================================================= */

void bitmap_texture_read_xml(texture_type *texture,int main_tag,bool read_scale)
{
	int						k,main_image_tag,image_tag,frame_count;

		// settings
		
	texture->animate.on=xml_get_attribute_boolean(main_tag,"animate");
	texture->additive=xml_get_attribute_boolean(main_tag,"additive");
	texture->pixelated=xml_get_attribute_boolean(main_tag,"pixelated");
	texture->compress=xml_get_attribute_boolean(main_tag,"compress");
	texture->keep_quality=xml_get_attribute_boolean(main_tag,"keep_quality");

	texture->shine_factor=xml_get_attribute_float_default(main_tag,"shine_factor",10.0f);

	texture->glow.rate=xml_get_attribute_int(main_tag,"glow_rate");
	texture->glow.min=xml_get_attribute_float_default(main_tag,"glow_min",0.25f);
	texture->glow.max=xml_get_attribute_float_default(main_tag,"glow_max",0.75f);

	xml_get_attribute_text(main_tag,"shader",texture->shader_name,name_str_len);
	xml_get_attribute_text(main_tag,"material_name",texture->material_name,name_str_len);
	
	if (read_scale) {
		texture->scale.x=xml_get_attribute_float_default(main_tag,"txt_scale_x",0.04f);
		texture->scale.y=xml_get_attribute_float_default(main_tag,"txt_scale_y",0.04f);
		texture->scale.on=xml_get_attribute_boolean(main_tag,"txt_scale_on");
		texture->scale.lock_offset=xml_get_attribute_boolean(main_tag,"txt_scale_lock_offset");
	}
	
		// images
		
	main_image_tag=xml_findfirstchild("Images",main_tag);
	if (main_image_tag!=-1) {
	
		texture->animate.current_frame=0;
		texture->animate.total_wait=0;
		
		frame_count=xml_countchildren(main_image_tag);
		image_tag=xml_findfirstchild("Image",main_image_tag);
		
		for (k=0;k!=frame_count;k++) {
			texture->animate.wait[k]=xml_get_attribute_int_default(image_tag,"wait",0);
			texture->animate.end_tick[k]=texture->animate.total_wait=texture->animate.total_wait+texture->animate.wait[k];
			xml_get_attribute_text(image_tag,"bitmap",texture->frames[k].name,file_str_len);

			image_tag=xml_findnextchild(image_tag);
		}
	}
}

void bitmap_texture_write_xml(texture_type *texture,int frame_count,bool write_scale)
{
	int						k;
	
		// settings
		
	xml_add_attribute_boolean("animate",texture->animate.on);
	xml_add_attribute_boolean("additive",texture->additive);
	xml_add_attribute_boolean("pixelated",texture->pixelated);
	xml_add_attribute_boolean("compress",texture->compress);
	xml_add_attribute_boolean("keep_quality",texture->keep_quality);

	xml_add_attribute_float("shine_factor",texture->shine_factor);
	
	xml_add_attribute_int("glow_rate",texture->glow.rate);
	xml_add_attribute_float("glow_min",texture->glow.min);
	xml_add_attribute_float("glow_max",texture->glow.max);
	
	xml_add_attribute_text("shader",texture->shader_name);
	xml_add_attribute_text("material_name",texture->material_name);
	
	if (write_scale) {
		xml_add_attribute_float("txt_scale_x",texture->scale.x);
		xml_add_attribute_float("txt_scale_y",texture->scale.y);
		xml_add_attribute_boolean("txt_scale_on",texture->scale.on);
		xml_add_attribute_boolean("txt_scale_lock_offset",texture->scale.lock_offset);
	}
	
	xml_add_tagend(FALSE);
	
		// images
		
	xml_add_tagstart("Images");
	xml_add_tagend(FALSE);
		
	for (k=0;k!=frame_count;k++) {
		xml_add_tagstart("Image");
		if (texture->animate.wait[k]!=0) xml_add_attribute_int("wait",texture->animate.wait[k]);
		if (texture->frames[k].name[0]!=0x0) xml_add_attribute_text("bitmap",texture->frames[k].name);
		xml_add_tagend(TRUE);
	}
	
	xml_add_tagclose("Images");
}

