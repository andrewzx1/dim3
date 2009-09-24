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

extern map_type				map;
extern view_type			view;
extern setup_type			setup;

int							gl_shader_current_idx,gl_shader_current_txt_idx,gl_shader_current_frame,
							gl_shader_current_extra_txt_idx;

extern int game_time_get(void);
extern float game_time_fequency_second_get(int start_tick);

/* =======================================================

      Shader Loading Utilities
      
======================================================= */

char* gl_shader_open_file(char *path)
{
	int				sz;
	char			*data;
	FILE			*file;
	struct stat		sb;
	
		// get file size
		
	if (stat(path,&sb)!=0) return(NULL);
	sz=sb.st_size;
	
		// open file
		
	file=fopen(path,"rb");
	if (file==NULL) return(NULL);
    
	data=(char*)malloc(sz+1);
    if (data==NULL) {
        fclose(file);
        return(NULL);
    }
	
	fread(data,1,sz,file);
	fclose(file);
	
	data[sz]=0x0;
    
	return(data);
}

/* =======================================================

      Setup Shader Variables
      
======================================================= */

void gl_shader_set_instance_variables(view_shader_type *shader)
{
	int							n;
	GLint						var;
	view_shader_custom_var_type	*cvar;
	
		// need to use program before calling these
		
	glUseProgramObjectARB(shader->program_obj);

		// texture pointers
		
	var=glGetUniformLocationARB(shader->program_obj,"dim3Tex");
	if (var!=-1) glUniform1iARB(var,0);

	var=glGetUniformLocationARB(shader->program_obj,"dim3TexBump");
	if (var!=-1) glUniform1iARB(var,1);
	
	var=glGetUniformLocationARB(shader->program_obj,"dim3TexSpecular");
	if (var!=-1) glUniform1iARB(var,2);
	
	var=glGetUniformLocationARB(shader->program_obj,"dim3TexExtra");
	if (var!=-1) glUniform1iARB(var,3);

		// custom variables
		
	for (n=0;n!=max_view_shader_custom_vars;n++) {
		cvar=&shader->custom_vars[n];

		if (cvar->name[0]==0x0) continue;
		
		var=glGetUniformLocationARB(shader->program_obj,cvar->name);
		if (var==-1) continue;
		
		switch (cvar->var_type) {
		
			case shader_var_type_int:
				glUniform1iARB(var,cvar->value.i);
				break;
				
			case shader_var_type_float:
				glUniform1fARB(var,cvar->value.f);
				break;
				
			case shader_var_type_vec3:
				glUniform3fARB(var,cvar->value.vec3[0],cvar->value.vec3[1],cvar->value.vec3[2]);
				break;
				
			case shader_var_type_vec4:
				glUniform4fARB(var,cvar->value.vec4[0],cvar->value.vec4[1],cvar->value.vec4[2],cvar->value.vec4[3]);
				break;
		}
	}
	
		// cancel program
		
	glUseProgramObjectARB(0);
}

void gl_shader_cache_dynamic_variable_locations(view_shader_type *shader)
{
	shader->var_dim3TimeMillisec=glGetUniformLocationARB(shader->program_obj,"dim3TimeMillisec");
	shader->var_dim3FrequencySecond=glGetUniformLocationARB(shader->program_obj,"dim3FrequencySecond");
	shader->var_dim3CameraPosition=glGetUniformLocationARB(shader->program_obj,"dim3CameraPosition");
	shader->var_dim3AmbientColor=glGetUniformLocationARB(shader->program_obj,"dim3AmbientColor");
	shader->var_dim3BumpFactor=glGetUniformLocationARB(shader->program_obj,"dim3BumpFactor");
	shader->var_dim3SpecularFactor=glGetUniformLocationARB(shader->program_obj,"dim3SpecularFactor");
	shader->var_dim3TexColor=glGetUniformLocationARB(shader->program_obj,"dim3TexColor");
	shader->var_dim3LightPosition=glGetUniformLocationARB(shader->program_obj,"dim3LightPosition");
	shader->var_dim3LightColor=glGetUniformLocationARB(shader->program_obj,"dim3LightColor");
	shader->var_dim3LightIntensity=glGetUniformLocationARB(shader->program_obj,"dim3LightIntensity");
	shader->var_dim3LightExponent=glGetUniformLocationARB(shader->program_obj,"dim3LightExponent");
	shader->var_dim3LightDirection=glGetUniformLocationARB(shader->program_obj,"dim3LightDirection");
	shader->var_dim3TintColor=glGetUniformLocationARB(shader->program_obj,"dim3TintColor");
	shader->var_dim3DarkFactor=glGetUniformLocationARB(shader->program_obj,"dim3DarkFactor");
	shader->var_dim3Alpha=glGetUniformLocationARB(shader->program_obj,"dim3Alpha");
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

      Shader Init/Shutdown
      
======================================================= */

bool gl_shader_initialize(char *err_str)
{
	int					n;
	bool				ok;
	char				path[1024];
	char				*vertex_data,*fragment_data;
	const GLcharARB		*vertex_ptr,*fragment_ptr;
	view_shader_type	*shader;

		// shaders on?

	if (!gl_check_shader_ok()) return(TRUE);

		// clear the shaders

	shader=view.shaders;
	
	for (n=0;n!=view.count.shader;n++) {
		shader->vertex_obj=shader->fragment_obj=shader->program_obj=0;
		shader++;
	}
	
		// load the shaders

	shader=view.shaders;
	
	for (n=0;n!=view.count.shader;n++) {

			// load the shaders
			
		file_paths_data(&setup.file_path_setup,path,"Shaders",shader->vertex_name,"vert");
		vertex_data=gl_shader_open_file(path);
		if (vertex_data==NULL) {
			sprintf(err_str,"GLSL Error: Can not open shader file '%s'",shader->vertex_name);
			return(FALSE);
		}
		
		file_paths_data(&setup.file_path_setup,path,"Shaders",shader->fragment_name,"frag");
		fragment_data=gl_shader_open_file(path);
		if (fragment_data==NULL) {
			sprintf(err_str,"GLSL Error: Can not open shader file '%s'",shader->fragment_name);
			return(FALSE);
		}
	
			// create the shader
			
		shader->program_obj=glCreateProgramObjectARB();
			
		shader->vertex_obj=glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
		
		vertex_ptr=vertex_data;
		glShaderSourceARB(shader->vertex_obj,1,&vertex_ptr,NULL);
	
		glCompileShaderARB(shader->vertex_obj);
		ok=gl_shader_report_error(err_str,shader->vertex_name,NULL,shader->vertex_obj,"Vertex",vertex_data,GL_OBJECT_COMPILE_STATUS_ARB);
		free(vertex_data);
		
		if (!ok) {
			gl_shader_shutdown();
			free(fragment_data);
			return(FALSE);
		}
	
		glAttachObjectARB(shader->program_obj,shader->vertex_obj);
		
		shader->fragment_obj=glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
		
		fragment_ptr=fragment_data;
		glShaderSourceARB(shader->fragment_obj,1,&fragment_ptr,NULL);
		
		glCompileShaderARB(shader->fragment_obj);
		ok=gl_shader_report_error(err_str,NULL,shader->fragment_name,shader->fragment_obj,"Fragment",fragment_data,GL_OBJECT_COMPILE_STATUS_ARB);
		free(fragment_data);
		
		if (!ok) {
			gl_shader_shutdown();
			return(FALSE);
		}
		
		glAttachObjectARB(shader->program_obj,shader->fragment_obj);
		
			// link shaders into program
			
		glLinkProgramARB(shader->program_obj);
		if (!gl_shader_report_error(err_str,shader->vertex_name,shader->fragment_name,shader->program_obj,"Program",NULL,GL_OBJECT_LINK_STATUS_ARB)) {
			gl_shader_shutdown();
			return(FALSE);
		}

			// per instance shader variables

		gl_shader_set_instance_variables(shader);
		
			// cache the location of the dymaic variables
			
		gl_shader_cache_dynamic_variable_locations(shader);
		
			// timing only works for full screen shaders
			
		shader->start_tick=0;

		shader++;
	}

	return(TRUE);
}

void gl_shader_shutdown(void)
{
	int							n;
	view_shader_type			*shader;

	if (!gl_check_shader_ok()) return;

		// shutdown shaders

	shader=view.shaders;
	
	for (n=0;n!=view.count.shader;n++) {
		if (shader->vertex_obj!=0) {
			glDetachObjectARB(shader->program_obj,shader->vertex_obj);
			glDeleteObjectARB(shader->vertex_obj);
		}

		if (shader->fragment_obj!=0) {
			glDetachObjectARB(shader->program_obj,shader->fragment_obj);
			glDeleteObjectARB(shader->fragment_obj);
		}

		if (shader->program_obj!=0) glDeleteObjectARB(shader->program_obj);

		shader++;
	}
}

/* =======================================================

      Attach Shaders to Textures
      
======================================================= */

int gl_shader_find(char *name)
{
	int							n;
	view_shader_type			*shader;

	shader=view.shaders;
	
	for (n=0;n!=view.count.shader;n++) {
		if (strcasecmp(name,shader->name)==0) return(n);
		shader++;
	}
	
	return(-1);
}

void gl_shader_attach_map(void)
{
	int					n;
	bool				shader_on;
	texture_type		*texture;
	
	shader_on=gl_check_shader_ok();

	texture=map.textures;
	
	for (n=0;n!=max_map_texture;n++) {
		texture->shader_idx=-1;
		if ((shader_on) && (texture->shader_name[0]!=0x0)) texture->shader_idx=gl_shader_find(texture->shader_name);
		texture++;
	}
}

void gl_shader_attach_model(model_type *mdl)
{
	int					n;
	bool				shader_on,has_no_shader;
	texture_type		*texture;
	
	shader_on=gl_check_shader_ok();

	has_no_shader=TRUE;

	texture=mdl->textures;
	
	for (n=0;n!=max_model_texture;n++) {
		texture->shader_idx=-1;
		if ((shader_on) && (texture->shader_name[0]!=0x0)) {
			texture->shader_idx=gl_shader_find(texture->shader_name);
			if (texture->shader_idx!=-1) has_no_shader=FALSE;
		}
		texture++;
	}

	mdl->has_no_shader=has_no_shader;
}

/* =======================================================

      Shader Variables
      
======================================================= */

void gl_shader_set_scene_variables(view_shader_type *shader)
{
	if (shader->var_dim3TimeMillisec!=-1) glUniform1iARB(shader->var_dim3TimeMillisec,(game_time_get()-shader->start_tick));
	if (shader->var_dim3FrequencySecond!=-1) glUniform1fARB(shader->var_dim3FrequencySecond,game_time_fequency_second_get(shader->start_tick));
	if (shader->var_dim3CameraPosition!=-1) glUniform3fARB(shader->var_dim3CameraPosition,(float)view.render->camera.pnt.x,(float)view.render->camera.pnt.y,(float)view.render->camera.pnt.z);
	if (shader->var_dim3AmbientColor!=-1) glUniform3fARB(shader->var_dim3AmbientColor,(map.ambient.light_color.r+setup.gamma),(map.ambient.light_color.g+setup.gamma),(map.ambient.light_color.b+setup.gamma));
}

void gl_shader_set_texture_variables(view_shader_type *shader,texture_type *texture)
{
	if (shader->var_dim3BumpFactor!=-1) glUniform1fARB(shader->var_dim3BumpFactor,texture->bump_factor);
	if (shader->var_dim3SpecularFactor!=-1) glUniform1fARB(shader->var_dim3SpecularFactor,texture->specular_factor);
	if (shader->var_dim3TexColor!=-1) glUniform3fARB(shader->var_dim3TexColor,texture->col.r,texture->col.g,texture->col.b);
}

void gl_shader_set_poly_variables(view_shader_type *shader,float dark_factor,float alpha,view_glsl_light_list_type *light_list,d3col *tint_col)
{
	if (light_list!=NULL) {
		if (shader->var_dim3LightPosition!=-1) glUniform3fvARB(shader->var_dim3LightPosition,max_shader_light,light_list->pos);
		if (shader->var_dim3LightColor!=-1) glUniform3fvARB(shader->var_dim3LightColor,max_shader_light,light_list->col);
		if (shader->var_dim3LightIntensity!=-1) glUniform1fvARB(shader->var_dim3LightIntensity,max_shader_light,light_list->intensity);
		if (shader->var_dim3LightExponent!=-1) glUniform1fvARB(shader->var_dim3LightExponent,max_shader_light,light_list->exponent);
		if (shader->var_dim3LightDirection!=-1) glUniform3fvARB(shader->var_dim3LightDirection,max_shader_light,light_list->direction);
	}
	
		// set tint color
		
	if (shader->var_dim3TintColor!=-1) {
	
		if (tint_col==NULL) {
			if ((shader->cur_tint_col.r!=1.0f) || (shader->cur_tint_col.g!=1.0f) || (shader->cur_tint_col.b!=1.0f)) {
				shader->cur_tint_col.r=shader->cur_tint_col.g=shader->cur_tint_col.b=1.0f;
				glUniform3fARB(shader->var_dim3TintColor,1.0f,1.0f,1.0f);
			}
		}
		else {
			if ((shader->cur_tint_col.r!=tint_col->r) || (shader->cur_tint_col.g!=tint_col->g) || (shader->cur_tint_col.b!=tint_col->b)) {
				memmove(&shader->cur_tint_col,tint_col,sizeof(d3col));
				glUniform3fARB(shader->var_dim3TintColor,tint_col->r,tint_col->g,tint_col->b);
			}
		}
	}
	
		// set dark and alpha if they've changed

	if (shader->var_dim3DarkFactor!=-1) {
		if (shader->cur_dark_factor!=dark_factor) {
			shader->cur_dark_factor=dark_factor;
			glUniform1fARB(shader->var_dim3DarkFactor,dark_factor);
		}
	}
	
	if (shader->var_dim3Alpha!=-1) {
		if (shader->cur_alpha!=alpha) {
			shader->cur_alpha=alpha;
			glUniform1fARB(shader->var_dim3Alpha,alpha);
		}
	}
}

/* =======================================================

      Shader Drawing Initialize/Start/Stop
      
======================================================= */

void gl_shader_draw_scene_initialize(void)
{
	int					n;
	view_shader_type	*shader;

	shader=view.shaders;

	for (n=0;n!=view.count.shader;n++) {
	
			// use this flag to mark scene only variables
			// as needing a load.  In this way we optimize
			// out the amount of variable setting we need to do
		
		shader->per_scene_vars_set=FALSE;
			
			// also setup some per poly current values
			// so we can skip setting if the values haven't changed

		shader->cur_light_idx[0]=-1000;
		shader->cur_in_hilite=FALSE;
		shader->cur_tint_col.r=shader->cur_tint_col.g=shader->cur_tint_col.b=-1.0f;
		shader->cur_dark_factor=-1.0f;
		shader->cur_alpha=-1.0f;
		
		shader++;
	}
}

void gl_shader_draw_start(void)
{
		// remember current shader

	gl_shader_current_idx=-1;

		// only reset textures when
		// needed
	
	gl_shader_current_txt_idx=-1;
	gl_shader_current_frame=-1;
	gl_shader_current_extra_txt_idx=-1;

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
		
	if (gl_shader_current_idx!=-1) glUseProgramObjectARB(0);
	
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

void gl_shader_texture_set(view_shader_type *shader,texture_type *texture,int txt_idx,int extra_txt_idx,int frame)
{
	GLuint			gl_id;

		// any changes?

	if ((gl_shader_current_txt_idx==txt_idx) && (gl_shader_current_frame==frame) && (gl_shader_current_extra_txt_idx==extra_txt_idx)) return;

	gl_shader_current_txt_idx=txt_idx;
	gl_shader_current_frame=frame;
	gl_shader_current_extra_txt_idx=extra_txt_idx;
	
		// extra texture map

	if (extra_txt_idx!=-1) {
		gl_id=map.textures[extra_txt_idx].frames[0].bitmap.gl_id;

		if (gl_id!=-1) {
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D,gl_id);
		}
	}
	
		// spec map

	gl_id=texture->frames[frame].specularmap.gl_id;

	if (gl_id!=-1) {
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D,gl_id);
	}

		// bump map

	gl_id=texture->frames[frame].bumpmap.gl_id;

	if (gl_id!=-1) {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D,gl_id);
	}
	
		// color map

	gl_id=texture->frames[frame].bitmap.gl_id;

	if (gl_id!=-1) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,gl_id);
	}

		// set per-texture specific variables

	gl_shader_set_texture_variables(shader,texture);
}

void gl_shader_texture_override(GLuint gl_id)
{
		// normally used to override for back rendering

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,gl_id);

		// need to force a reset for next time

	gl_shader_current_txt_idx=-1;
	gl_shader_current_frame=-1;
	gl_shader_current_extra_txt_idx=-1;
}

/* =======================================================

      Execute Shader
      
======================================================= */

void gl_shader_draw_execute(texture_type *texture,int txt_idx,int frame,int extra_txt_idx,float dark_factor,float alpha,int *light_idx,d3pnt *pnt,d3col *tint_col)
{
	int								n;
	bool							light_change;
	view_shader_type				*shader;
	view_glsl_light_list_type		light_list;
	
		// get shader
		
	shader=&view.shaders[texture->shader_idx];
	
		// if we are not in this shader, then
		// change over
		
	if (texture->shader_idx!=gl_shader_current_idx) {
	
			// set in the new program
			
		gl_shader_current_idx=texture->shader_idx;
		glUseProgramObjectARB(shader->program_obj);
			
			// set per-scene variables, only do this once
			// as they don't change per scene
		
		if (!shader->per_scene_vars_set) {
			shader->per_scene_vars_set=TRUE;
			gl_shader_set_scene_variables(shader);
		}

			// a shader change will force a texture
			// change as certain variables might not
			// be loaded in the texture

		gl_shader_current_txt_idx=-1;
	}
	
		// textures and per-texture variables
		
	gl_shader_texture_set(shader,texture,txt_idx,extra_txt_idx,frame);
	
		// lighting variables
		// this version is for shaders lite by view lights
		
	if (light_idx!=NULL) {
	
			// any changes in lighting (different
			// light list or coming out of a hilite)
			
		light_change=shader->cur_in_hilite;
		
		for (n=0;n!=max_shader_light;n++) {
			if (shader->cur_light_idx[n]!=light_idx[n]) light_change=TRUE;
			shader->cur_light_idx[n]=light_idx[n];
		}
		
		shader->cur_in_hilite=FALSE;
		
		if (!light_change) {
			gl_shader_set_poly_variables(shader,dark_factor,alpha,NULL,tint_col);
		}
		else {
			gl_lights_idx_to_light_list(light_idx,&light_list);
			gl_shader_set_poly_variables(shader,dark_factor,alpha,&light_list,tint_col);
		}
	}

		// lighting by highlight
		// this version uses a single large light

	else {
		
			// already in hilite?
			
		if (shader->cur_in_hilite) {
			gl_shader_set_poly_variables(shader,dark_factor,alpha,NULL,tint_col);
		}
		else {
			bzero(&light_list,sizeof(view_glsl_light_list_type));

			light_list.pos[0]=(float)view.render->camera.pnt.x;
			light_list.pos[1]=(float)view.render->camera.pnt.y;
			light_list.pos[2]=(float)view.render->camera.pnt.z;
			
			light_list.col[0]=light_list.col[1]=light_list.col[2]=1.0f;

			light_list.intensity[0]=(float)map_max_size;
			light_list.exponent[0]=0.0f;		// hard light

			light_list.direction[0]=0.0f;
			light_list.direction[1]=0.0f;
			light_list.direction[2]=0.0f;
			
			shader->cur_in_hilite=TRUE;
			
			gl_shader_set_poly_variables(shader,dark_factor,alpha,&light_list,tint_col);
		}
	}
}
