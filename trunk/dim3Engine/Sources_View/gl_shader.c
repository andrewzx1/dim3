/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Shader Routines

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
	#include "dim3engine.h"
#endif

#include "lights.h"
#include "cameras.h"
#include "consoles.h"
#include "video.h"
#include "timing.h"

extern map_type				map;
extern view_type			view;
extern setup_type			setup;

shader_type					*gl_shader_current;

extern int					nuser_shader;
extern shader_type			user_shaders[max_user_shader];

extern float				light_shader_direction[7][3];

extern bitmap_type			lmap_hilite_bitmap;

/* =======================================================

      Setup Shader Variables
      
======================================================= */

void gl_shader_set_instance_variables(shader_type *shader)
{
	GLint					var;
	
		// need to use program before calling these
		
	glUseProgramObjectARB(shader->program_obj);

		// texture pointers
		
	var=glGetUniformLocationARB(shader->program_obj,"dim3Tex");
	if (var!=-1) glUniform1iARB(var,0);

	var=glGetUniformLocationARB(shader->program_obj,"dim3TexBump");
	if (var!=-1) glUniform1iARB(var,1);
	
	var=glGetUniformLocationARB(shader->program_obj,"dim3TexSpecular");
	if (var!=-1) glUniform1iARB(var,2);
	
	var=glGetUniformLocationARB(shader->program_obj,"dim3TexLightMap");
	if (var!=-1) glUniform1iARB(var,3);
	
		// cancel program
		
	glUseProgramObjectARB(0);
}

void gl_shader_cache_dynamic_variable_locations(shader_type *shader)
{
	int				n;
	char			var_name[256];
	
	shader->var_locs.dim3FrequencySecond=glGetUniformLocationARB(shader->program_obj,"dim3FrequencySecond");
	shader->var_locs.dim3CameraPosition=glGetUniformLocationARB(shader->program_obj,"dim3CameraPosition");
	shader->var_locs.dim3AmbientColor=glGetUniformLocationARB(shader->program_obj,"dim3AmbientColor");
	shader->var_locs.dim3LightMapBoost=glGetUniformLocationARB(shader->program_obj,"dim3LightMapBoost");
	shader->var_locs.dim3ShineFactor=glGetUniformLocationARB(shader->program_obj,"dim3ShineFactor");
	shader->var_locs.dim3TintColor=glGetUniformLocationARB(shader->program_obj,"dim3TintColor");
	shader->var_locs.dim3Alpha=glGetUniformLocationARB(shader->program_obj,"dim3Alpha");

	shader->var_locs.dim3Tangent=glGetUniformLocationARB(shader->program_obj,"dim3Tangent");
	shader->var_locs.dim3Normal=glGetUniformLocationARB(shader->program_obj,"dim3Normal");

	shader->var_locs.dim3VertexTangent=glGetAttribLocationARB(shader->program_obj,"dim3VertexTangent");
	shader->var_locs.dim3VertexNormal=glGetAttribLocationARB(shader->program_obj,"dim3VertexNormal");
	
	for (n=0;n!=max_shader_light;n++) {
		sprintf(var_name,"dim3Light_%d.position",n);
		shader->var_locs.dim3Lights[n].position=glGetUniformLocationARB(shader->program_obj,var_name);
		sprintf(var_name,"dim3Light_%d.color",n);
		shader->var_locs.dim3Lights[n].color=glGetUniformLocationARB(shader->program_obj,var_name);
		sprintf(var_name,"dim3Light_%d.intensity",n);
		shader->var_locs.dim3Lights[n].intensity=glGetUniformLocationARB(shader->program_obj,var_name);
		sprintf(var_name,"dim3Light_%d.exponent",n);
		shader->var_locs.dim3Lights[n].exponent=glGetUniformLocationARB(shader->program_obj,var_name);
		sprintf(var_name,"dim3Light_%d.direction",n);
		shader->var_locs.dim3Lights[n].direction=glGetUniformLocationARB(shader->program_obj,var_name);
		sprintf(var_name,"dim3Light_%d.inLightMap",n);
		shader->var_locs.dim3Lights[n].inLightMap=glGetUniformLocationARB(shader->program_obj,var_name);
	}
}

/* =======================================================

      Shader Error Reporter
      
======================================================= */

bool gl_shader_report_error(char *err_str,char *vertex_name,char *fragment_name,GLhandleARB hand,char *err_type,char *code,int check_type)
{
	int				line;
	GLint			result,len;
	char			*c,*str,log_line[256];
	FILE			*file;
	struct tm		*tm;
	time_t			curtime;
	
		// operation OK?
		
	glGetObjectParameterivARB(hand,check_type,&result);
	if (result==1) return(TRUE);
	
		// start or append log file
		
	file=fopen("glsl_error_log.txt","a");
	if (file==NULL) {
		strcpy(err_str,"GLSL Error: Could not write log file");
		return(FALSE);
	}
	
		// header
		
	fwrite("***\n",1,4,file);

	sprintf(log_line,"Error Type: %s\n",err_type);
	fwrite(log_line,1,strlen(log_line),file);
	
	if (vertex_name!=NULL) {
		sprintf(log_line,"Vertex: %s\n",vertex_name);
		fwrite(log_line,1,strlen(log_line),file);
	}

	if (fragment_name!=NULL) {
		sprintf(log_line,"Fragment: %s\n",fragment_name);
		fwrite(log_line,1,strlen(log_line),file);
	}

	curtime=time(NULL);
	tm=localtime(&curtime);
	
	sprintf(log_line,"Time: %.4d %.2d %.2d %.2d:%.2d.%.2d\n",(tm->tm_year+1900),(tm->tm_mon+1),tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec);
	fwrite(log_line,1,strlen(log_line),file);

		// the error
		
	glGetObjectParameterivARB(hand,GL_OBJECT_INFO_LOG_LENGTH_ARB,&len);
	if (len>0) {
		str=malloc(len);
		if (str!=NULL) {
			glGetInfoLogARB(hand,len,NULL,str);

			fwrite(str,1,len,file);
			fwrite("\n",1,1,file);

			free(str);
		}
	}
	
		// the code
		
	if (code!=NULL) {
	
		c=code;
		line=2;
		
		fwrite("001:",1,4,file);
		
		while (*c!=0x0) {
			
			if (*c=='\r') {
				c++;
				continue;
			}
			
			fwrite(c,1,1,file);
			
			if (*c=='\n') {
				sprintf(log_line,"%.3d:",line);
				fwrite(log_line,1,4,file);
				line++;
			}

			c++;
		}
	}
	
	fwrite("\n",1,1,file);
		
		// finish log

	fclose(file);

		// fatal error string

	strcpy(err_str,"GLSL: Could not compile or link code, check out glsl_error_log.txt for more information");
	
	return(FALSE);
}

/* =======================================================

      Compile Shader
      
======================================================= */

void gl_shader_code_clear(shader_type *shader)
{
	shader->vertex_obj=0;
	shader->fragment_obj=0;
	shader->program_obj=0;
}

bool gl_shader_code_compile(shader_type *shader,char *vertex_data,char *fragment_data,char *err_str)
{
	const GLcharARB			*vertex_ptr,*fragment_ptr;

		// create the shader
			
	shader->program_obj=glCreateProgramObjectARB();
		
	shader->vertex_obj=glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	
	vertex_ptr=vertex_data;
	glShaderSourceARB(shader->vertex_obj,1,&vertex_ptr,NULL);

	glCompileShaderARB(shader->vertex_obj);
	if (!gl_shader_report_error(err_str,shader->vertex_name,NULL,shader->vertex_obj,"Vertex",vertex_data,GL_OBJECT_COMPILE_STATUS_ARB)) return(FALSE);

	glAttachObjectARB(shader->program_obj,shader->vertex_obj);
	
	shader->fragment_obj=glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
	
	fragment_ptr=fragment_data;
	glShaderSourceARB(shader->fragment_obj,1,&fragment_ptr,NULL);
	
	glCompileShaderARB(shader->fragment_obj);
	if (!gl_shader_report_error(err_str,NULL,shader->fragment_name,shader->fragment_obj,"Fragment",fragment_data,GL_OBJECT_COMPILE_STATUS_ARB)) return(FALSE);
	
	glAttachObjectARB(shader->program_obj,shader->fragment_obj);
	
		// link shaders into program
		
	glLinkProgramARB(shader->program_obj);
	if (!gl_shader_report_error(err_str,shader->vertex_name,shader->fragment_name,shader->program_obj,"Program",NULL,GL_OBJECT_LINK_STATUS_ARB)) return(FALSE);

		// per instance shader variables

	gl_shader_set_instance_variables(shader);
	
		// cache the location of the dymaic variables
		
	gl_shader_cache_dynamic_variable_locations(shader);
	
		// timing only works for full screen shaders
		
	shader->start_tick=0;

	return(TRUE);
}

void gl_shader_code_shutdown(shader_type *shader)
{
	if (shader->vertex_obj!=0) {
		glDetachObjectARB(shader->program_obj,shader->vertex_obj);
		glDeleteObjectARB(shader->vertex_obj);
	}

	if (shader->fragment_obj!=0) {
		glDetachObjectARB(shader->program_obj,shader->fragment_obj);
		glDeleteObjectARB(shader->fragment_obj);
	}

	if (shader->program_obj!=0) glDeleteObjectARB(shader->program_obj);
}

/* =======================================================

      Attach Shaders to Textures
      
======================================================= */

void gl_shader_attach_map(void)
{
	int					n;
	texture_type		*texture;
	
	texture=map.textures;
	
	for (n=0;n!=max_map_texture;n++) {
		texture->shader_idx=gl_shader_core_index;
		
		if (texture->shader_name[0]!=0x0) {
			texture->shader_idx=gl_user_shader_find(texture->shader_name);
		}
		
		texture++;
	}
}

void gl_shader_attach_model(model_type *mdl)
{
	int					n;
	texture_type		*texture;
	
	texture=mdl->textures;
	
	for (n=0;n!=max_model_texture;n++) {
		texture->shader_idx=gl_shader_core_index;
		
		if (texture->shader_name[0]!=0x0) {
			texture->shader_idx=gl_user_shader_find(texture->shader_name);
		}
		
		texture++;
	}
}

/* =======================================================

      Shader Variables
      
======================================================= */

void gl_shader_set_scene_variables(shader_type *shader)
{
	d3col			col;
	
	if (shader->var_locs.dim3FrequencySecond!=-1) glUniform1fARB(shader->var_locs.dim3FrequencySecond,game_time_fequency_second_get(shader->start_tick));
	if (shader->var_locs.dim3CameraPosition!=-1) glUniform3fARB(shader->var_locs.dim3CameraPosition,(float)view.render->camera.pnt.x,(float)view.render->camera.pnt.y,(float)view.render->camera.pnt.z);

	if (shader->var_locs.dim3AmbientColor!=-1) {
		gl_lights_calc_ambient_color(&col);
		glUniform3fARB(shader->var_locs.dim3AmbientColor,col.r,col.g,col.b);
	}
	
	if (shader->var_locs.dim3LightMapBoost!=-1) glUniform1fARB(shader->var_locs.dim3LightMapBoost,map.ambient.light_map_boost);
}

void gl_shader_ambient_hilite_override(shader_type *shader,bool hilite)
{
	d3col			col;
	
	if (shader->var_locs.dim3AmbientColor==-1) return;
	
	if (hilite) {
		glUniform3fARB(shader->var_locs.dim3AmbientColor,1.0f,1.0f,1.0f);
		return;
	}
	
	gl_lights_calc_ambient_color(&col);
	glUniform3fARB(shader->var_locs.dim3AmbientColor,col.r,col.g,col.b);
}

void gl_shader_set_light_normal_variables(shader_type *shader,bool is_core,view_light_list_type *light_list)
{
	int								n,k,count,in_light_map,max_light;
	view_light_spot_type			*lspot;
	shader_cached_var_light_loc		*loc_light;
	shader_current_var_light_value	*cur_light;
	
		// if in core and no lights,
		// skip all this as core shaders ignore lights
		
	if ((is_core) && (light_list->nlight==0)) return;
	
		// have lights changed?
		
	if (light_list->nlight==shader->var_values.nlight) {
		
		count=0;
	
		for (n=0;n!=light_list->nlight;n++) {
			for (k=0;k!=shader->var_values.nlight;k++) {
				if (light_list->light_idx[n]==shader->var_values.light_idx[k]) {
					count++;
					break;
				}
			}
		}
		
		if (count==light_list->nlight) return;
	}
	
		// light changed, remember it for next time
		
	for (n=0;n!=light_list->nlight;n++) {
		shader->var_values.light_idx[n]=light_list->light_idx[n];
	}
	
	shader->var_values.nlight=light_list->nlight;
	
		// core shaders ignore lights outside their
		// range, so we don't need to look through max
		// lights for core shaders
		
	if (is_core) {
		max_light=light_list->nlight;
	}
	else {
		max_light=max_shader_light;
	}
	
		// set the lights uniforms
		
	for (n=0;n!=max_light;n++) {

		loc_light=&shader->var_locs.dim3Lights[n];
		cur_light=&shader->var_values.lights[n];
	
			// lights that are off
			// have intensity of 0
			
		if (n>=light_list->nlight) {
			if (loc_light->intensity==-1) continue;

			if (cur_light->intensity!=0.0f) {
				cur_light->intensity=0.0f;
				glUniform1fARB(loc_light->intensity,0.0f);
			}

			continue;
		}
		
			// set regular light
			
		lspot=&view.render->light.spots[light_list->light_idx[n]];
		
		if (loc_light->position!=-1) {
			if ((cur_light->position.x!=lspot->pnt_eye_space.x) || (cur_light->position.y!=lspot->pnt_eye_space.y) || (cur_light->position.z!=lspot->pnt_eye_space.z)) {
				cur_light->position.x=lspot->pnt_eye_space.x;
				cur_light->position.y=lspot->pnt_eye_space.y;
				cur_light->position.z=lspot->pnt_eye_space.z;
				glUniform3fARB(loc_light->position,lspot->pnt_eye_space.x,lspot->pnt_eye_space.y,lspot->pnt_eye_space.z);
			}
		}

		if (loc_light->color!=-1) {
			if ((cur_light->color.r!=lspot->col.r) || (cur_light->color.g!=lspot->col.g) || (cur_light->color.b!=lspot->col.b)) {
				cur_light->color.r=lspot->col.r;
				cur_light->color.g=lspot->col.g;
				cur_light->color.b=lspot->col.b;
				glUniform3fARB(loc_light->color,lspot->col.r,lspot->col.g,lspot->col.b);
			}
		}

		if (loc_light->intensity!=-1) {
			if (cur_light->intensity!=lspot->f_intensity) {
				cur_light->intensity=lspot->f_intensity;
				glUniform1fARB(loc_light->intensity,lspot->f_intensity);
			}
		}

		if (loc_light->exponent!=-1) {
			if (cur_light->exponent!=lspot->exponent) {
				cur_light->exponent=lspot->exponent;
				glUniform1fARB(loc_light->exponent,lspot->exponent);
			}
		}

		if (loc_light->direction!=-1) {
			if (cur_light->direction!=lspot->direction) {
				cur_light->direction=lspot->direction;
				glUniform3fvARB(loc_light->direction,3,light_shader_direction[lspot->direction]);
			}
		}

		if (loc_light->inLightMap!=-1) {
			in_light_map=(lspot->light_map?1:0);
			if (cur_light->light_map!=in_light_map) {
				cur_light->light_map=in_light_map;
				glUniform1iARB(loc_light->inLightMap,in_light_map);
			}
		}
	}
}

void gl_shader_set_poly_variables(shader_type *shader,float alpha,d3col *tint_col,tangent_space_type *tangent_space,model_draw_vbo_offset_type *vbo_offset)
{
		// set tint color
		// only for model shaders
		
	if (tint_col!=NULL) {
		if (shader->var_locs.dim3TintColor!=-1) {
			if ((shader->var_values.tint_col.r!=tint_col->r) || (shader->var_values.tint_col.g!=tint_col->g) || (shader->var_values.tint_col.b!=tint_col->b)) {
				memmove(&shader->var_values.tint_col,tint_col,sizeof(d3col));
				glUniform3fARB(shader->var_locs.dim3TintColor,tint_col->r,tint_col->g,tint_col->b);
			}
		}
	}
	
		// alpha
		
	if (shader->var_locs.dim3Alpha!=-1) {
		if (shader->var_values.alpha!=alpha) {
			shader->var_values.alpha=alpha;
			glUniform1fARB(shader->var_locs.dim3Alpha,alpha);
		}
	}

		// tangent spaces

	if (tangent_space!=NULL) {
	
		if (shader->var_locs.dim3Tangent!=-1) {
			if ((shader->var_values.tangent.x!=tangent_space->tangent.x) || (shader->var_values.tangent.y!=tangent_space->tangent.y) || (shader->var_values.tangent.z!=tangent_space->tangent.z)) {
				shader->var_values.tangent.x=tangent_space->tangent.x;
				shader->var_values.tangent.y=tangent_space->tangent.y;
				shader->var_values.tangent.z=tangent_space->tangent.z;
				glUniform3fARB(shader->var_locs.dim3Tangent,tangent_space->tangent.x,tangent_space->tangent.y,tangent_space->tangent.z);
			}
		}
		
		if (shader->var_locs.dim3Normal!=-1) {
			if ((shader->var_values.normal.x!=tangent_space->normal.x) || (shader->var_values.normal.y!=tangent_space->normal.y) || (shader->var_values.normal.z!=tangent_space->normal.z)) {
				shader->var_values.normal.x=tangent_space->normal.x;
				shader->var_values.normal.y=tangent_space->normal.y;
				shader->var_values.normal.z=tangent_space->normal.z;
				glUniform3fARB(shader->var_locs.dim3Normal,tangent_space->normal.x,tangent_space->normal.y,tangent_space->normal.z);
			}
		}
	}

	if (vbo_offset!=NULL) {
		if (!shader->normal_vertex_attrib_active) {
			shader->normal_vertex_attrib_active=TRUE;
			if (shader->var_locs.dim3VertexTangent!=-1) glEnableVertexAttribArrayARB(shader->var_locs.dim3VertexTangent); 
			if (shader->var_locs.dim3VertexNormal!=-1) glEnableVertexAttribArrayARB(shader->var_locs.dim3VertexNormal); 
		}

		if (shader->var_locs.dim3VertexTangent!=-1) glVertexAttribPointerARB(shader->var_locs.dim3VertexTangent,3,GL_FLOAT,0,0,(void*)vbo_offset->tangent);
		if (shader->var_locs.dim3VertexNormal!=-1) glVertexAttribPointerARB(shader->var_locs.dim3VertexNormal,3,GL_FLOAT,0,0,(void*)vbo_offset->normal);
	}
	else {
		if (shader->normal_vertex_attrib_active) {
			shader->normal_vertex_attrib_active=FALSE;
			if (shader->var_locs.dim3VertexTangent!=-1) glDisableVertexAttribArrayARB(shader->var_locs.dim3VertexTangent); 
			if (shader->var_locs.dim3VertexNormal!=-1) glDisableVertexAttribArrayARB(shader->var_locs.dim3VertexNormal);
		}
	}
}

/* =======================================================

      Shader Drawing Initialize/Start/Stop
      
======================================================= */

void gl_shader_draw_scene_initialize_code(shader_type *shader)
{
	int					n;

		// use this flag to mark scene only variables
		// as needing a load.  In this way we optimize
		// out the amount of variable setting we need to do
	
	shader->per_scene_vars_set=FALSE;
	
		// some model drawings can override the ambient
		// for a highlight effect, need to remember
		// when we are in that mode
		
	shader->in_hilite=FALSE;
		
		// also setup some per poly current values
		// so we can skip setting if the values haven't changed

	shader->var_values.nlight=-1;
	shader->var_values.alpha=-1.0f;
	shader->var_values.shine_factor=-1.0f;
	shader->var_values.tint_col.r=shader->var_values.tint_col.g=shader->var_values.tint_col.b=-1.0f;
	
	shader->var_values.tangent.x=shader->var_values.tangent.y=shader->var_values.tangent.z=0.0f;
	shader->var_values.normal.x=shader->var_values.normal.y=shader->var_values.normal.z=0.0f;

	for (n=0;n!=max_shader_light;n++) {
		shader->var_values.lights[n].light_map=-1;
		shader->var_values.lights[n].intensity=-1.0f;
		shader->var_values.lights[n].exponent=-1.0f;
		shader->var_values.lights[n].position.x=-1.0f;
		shader->var_values.lights[n].position.y=-1.0f;
		shader->var_values.lights[n].position.z=-1.0f;
		shader->var_values.lights[n].direction=-1;
		shader->var_values.lights[n].color.r=-1.0f;
		shader->var_values.lights[n].color.g=-1.0f;
		shader->var_values.lights[n].color.b=-1.0f;
	}
	
		// check if the model tangent space attributes
		// have been attached
		
	shader->normal_vertex_attrib_active=FALSE;
}

void gl_shader_draw_scene_initialize(void)
{
	gl_core_shader_draw_scene_initialize();
	gl_user_shader_draw_scene_initialize();
}

void gl_shader_draw_start(void)
{
		// remember current shader

	gl_shader_current=NULL;

		// make all textures replace
		
	glColor4f(1.0f,0.0f,1.0f,1.0f);

	glActiveTexture(GL_TEXTURE3);
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
	
	glActiveTexture(GL_TEXTURE2);
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);

	glActiveTexture(GL_TEXTURE1);
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);

	glActiveTexture(GL_TEXTURE0);
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
}

void gl_shader_draw_end(void)
{
		// deactivate any current shader
		
	if (gl_shader_current!=NULL) glUseProgramObjectARB(0);
	
		// turn off any used textures
		
	glActiveTexture(GL_TEXTURE3);
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	glDisable(GL_TEXTURE_2D);
	
	glActiveTexture(GL_TEXTURE2);
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	glDisable(GL_TEXTURE_2D);
	
	glActiveTexture(GL_TEXTURE1);
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	glDisable(GL_TEXTURE_2D);
	
	glActiveTexture(GL_TEXTURE0);
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	glDisable(GL_TEXTURE_2D);
}

/* =======================================================

      Set Shader Textures
      
======================================================= */

void gl_shader_texture_set(shader_type *shader,bool map_shader,texture_type *texture,int txt_idx,int lmap_txt_idx,int frame)
{
	GLuint			gl_id;

		// extra texture map

	if (map_shader) {
		if ((lmap_txt_idx==-1) || (view.debug.on)) {
			gl_id=lmap_hilite_bitmap.gl_id;
		}
		else {
			gl_id=map.textures[lmap_txt_idx].frames[0].bitmap.gl_id;
		}
		if (gl_id!=-1) gl_texture_bind(3,gl_id);
	}
	
		// spec map

	gl_id=texture->frames[frame].specularmap.gl_id;

	if (gl_id!=-1) {
		gl_texture_bind(2,gl_id);
		
		if (shader->var_values.shine_factor!=texture->shine_factor) {
			shader->var_values.shine_factor=texture->shine_factor;
			if (shader->var_locs.dim3ShineFactor!=-1) glUniform1fARB(shader->var_locs.dim3ShineFactor,texture->shine_factor);
		}
	}

		// bump map

	gl_id=texture->frames[frame].bumpmap.gl_id;
	if (gl_id!=-1) gl_texture_bind(1,gl_id);
	
		// color map

	gl_id=texture->frames[frame].bitmap.gl_id;
	if (gl_id!=-1) gl_texture_bind(0,gl_id);
}

void gl_shader_texture_override(GLuint gl_id,float alpha)
{
		// normally used to override for back rendering

	gl_texture_bind(0,gl_id);
	
		// nodes can override alpha
		// multiply in alpha so fade effects still work
		
	if (alpha==1.0f) return;
	
	if (gl_shader_current->var_locs.dim3Alpha!=-1) {
		alpha*=gl_shader_current->var_values.alpha;
		if (gl_shader_current->var_values.alpha!=alpha) {
			gl_shader_current->var_values.alpha=alpha;
			glUniform1fARB(gl_shader_current->var_locs.dim3Alpha,alpha);
		}
	}
}

/* =======================================================

      Execute Shader
      
======================================================= */

void gl_shader_draw_execute(bool map_shader,texture_type *texture,int txt_idx,int frame,int lmap_txt_idx,float alpha,view_light_list_type *light_list,bool hilite,d3pnt *pnt,d3col *tint_col,tangent_space_type *tangent_space,model_draw_vbo_offset_type *vbo_offset)
{
	bool						is_core;
	shader_type					*shader;
	
		// get shader based on number of lights
		
	if (texture->shader_idx==gl_shader_core_index) {
		shader=gl_core_shader_find_ptr(light_list->nlight,map_shader,texture);
		is_core=TRUE;
	}
	else {
		shader=&user_shaders[texture->shader_idx];
		is_core=FALSE;
	}
	
		// if we are not in this shader, then
		// change over
		
	if (shader!=gl_shader_current) {
	
			// set in the new program
			
		gl_shader_current=shader;
		glUseProgramObjectARB(shader->program_obj);
			
			// set per-scene variables, only do this once
			// as they don't change per scene
		
		if (!shader->per_scene_vars_set) {
			shader->per_scene_vars_set=TRUE;
			gl_shader_set_scene_variables(shader);
		}
	}
	
		// hiliting
		
	if ((!map_shader) && (hilite)) {
		shader->in_hilite=TRUE;
		gl_shader_ambient_hilite_override(shader,TRUE);
	}
	else {
		if (shader->in_hilite) {
			shader->in_hilite=FALSE;
			gl_shader_ambient_hilite_override(shader,FALSE);
		}
	}
	
		// textures and per-texture variables
		
	gl_shader_texture_set(shader,map_shader,texture,txt_idx,lmap_txt_idx,frame);
	
		// per polygon variables
		
	gl_shader_set_poly_variables(shader,alpha,tint_col,tangent_space,vbo_offset);
	
		// lighting variables
			
	gl_shader_set_light_normal_variables(shader,is_core,light_list);
}
