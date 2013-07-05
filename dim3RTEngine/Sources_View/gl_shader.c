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

(c) 2000-2012 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "interface.h"

extern map_type				map;
extern view_type			view;
extern setup_type			setup;
extern file_path_setup_type	file_path_setup;

int							gl_shader_cur_mesh_idx;
shader_type					*gl_shader_current;

extern shader_type			color_shader,gradient_shader,black_shader,
							bitmap_shader;

extern float				gl_proj_matrix[16],gl_model_view_matrix[16],
							gl_normal_matrix[9],
							light_shader_direction[7][3];

extern bitmap_type			lmap_black_bitmap,lmap_white_bitmap;

/* =======================================================

      Initialize Shaders
      
======================================================= */

void gl_shader_initialize(void)
{
	gl_shader_current=NULL;
}

/* =======================================================

      Setup Shader Variables
      
======================================================= */

void gl_shader_set_instance_variables(shader_type *shader)
{
	GLint					var;
	
		// texture pointers
		
	var=glGetUniformLocation(shader->program_obj,"dim3Tex");
	if (var!=-1) glUniform1i(var,0);

	var=glGetUniformLocation(shader->program_obj,"dim3TexBump");
	if (var!=-1) glUniform1i(var,1);
	
	var=glGetUniformLocation(shader->program_obj,"dim3TexSpecular");
	if (var!=-1) glUniform1i(var,2);

	var=glGetUniformLocation(shader->program_obj,"dim3TexGlow");
	if (var!=-1) glUniform1i(var,3);
	
	var=glGetUniformLocation(shader->program_obj,"dim3TexLightMap");
	if (var!=-1) glUniform1i(var,4);
}

void gl_shader_reset_light_values(shader_type *shader)
{
	int				n;

	shader->current_hilite=0;		// 0 = not set, 1 = on, -1 = off

	shader->var_values.nlight=-1;

	for (n=0;n!=max_shader_light;n++) {
		shader->var_values.lights[n].intensity=-1.0f;
		shader->var_values.lights[n].invertIntensity=-1.0f;
		shader->var_values.lights[n].exponent=-1.0f;
		shader->var_values.lights[n].position.x=-1.0f;
		shader->var_values.lights[n].position.y=-1.0f;
		shader->var_values.lights[n].position.z=-1.0f;
		shader->var_values.lights[n].direction=-1;
		shader->var_values.lights[n].color.r=-1.0f;
		shader->var_values.lights[n].color.g=-1.0f;
		shader->var_values.lights[n].color.b=-1.0f;
	}
}

void gl_shader_cache_dynamic_variable_locations(shader_type *shader)
{
	int				n;
	char			var_name[256];

		// cache attribute locations

	shader->var_locs.dim3Vertex=glGetAttribLocation(shader->program_obj,"dim3Vertex");
	shader->var_locs.dim3VertexColor=glGetAttribLocation(shader->program_obj,"dim3VertexColor");
	shader->var_locs.dim3VertexUV=glGetAttribLocation(shader->program_obj,"dim3VertexUV");
	shader->var_locs.dim3VertexLightMapUV=glGetAttribLocation(shader->program_obj,"dim3VertexLightMapUV");
	shader->var_locs.dim3VertexTangent=glGetAttribLocation(shader->program_obj,"dim3VertexTangent");
	shader->var_locs.dim3VertexNormal=glGetAttribLocation(shader->program_obj,"dim3VertexNormal");
	
		// cache uniform locations
		
	shader->var_locs.dim3ProjectionMatrix=glGetUniformLocation(shader->program_obj,"dim3ProjectionMatrix");
	shader->var_locs.dim3ModelViewMatrix=glGetUniformLocation(shader->program_obj,"dim3ModelViewMatrix");
	shader->var_locs.dim3NormalMatrix=glGetUniformLocation(shader->program_obj,"dim3NormalMatrix");
		
	shader->var_locs.dim3FrequencySecond=glGetUniformLocation(shader->program_obj,"dim3FrequencySecond");
	shader->var_locs.dim3CameraPosition=glGetUniformLocation(shader->program_obj,"dim3CameraPosition");
	shader->var_locs.dim3AmbientColor=glGetUniformLocation(shader->program_obj,"dim3AmbientColor");
	shader->var_locs.dim3ShineFactor=glGetUniformLocation(shader->program_obj,"dim3ShineFactor");
	shader->var_locs.dim3GlowFactor=glGetUniformLocation(shader->program_obj,"dim3GlowFactor");
	shader->var_locs.dim3Alpha=glGetUniformLocation(shader->program_obj,"dim3Alpha");
	shader->var_locs.dim3DiffuseVector=glGetUniformLocation(shader->program_obj,"dim3DiffuseVector");

	shader->var_locs.dim3FogEnd=glGetUniformLocation(shader->program_obj,"dim3FogEnd");
	shader->var_locs.dim3FogScale=glGetUniformLocation(shader->program_obj,"dim3FogScale");
	shader->var_locs.dim3FogColor=glGetUniformLocation(shader->program_obj,"dim3FogColor");

	shader->var_locs.dim3SimpleColor=glGetUniformLocation(shader->program_obj,"dim3SimpleColor");
	
	for (n=0;n!=max_shader_light;n++) {
		sprintf(var_name,"dim3Light_%d.position",n);
		shader->var_locs.dim3Lights[n].position=glGetUniformLocation(shader->program_obj,var_name);
		sprintf(var_name,"dim3Light_%d.color",n);
		shader->var_locs.dim3Lights[n].color=glGetUniformLocation(shader->program_obj,var_name);
		sprintf(var_name,"dim3Light_%d.intensity",n);
		shader->var_locs.dim3Lights[n].intensity=glGetUniformLocation(shader->program_obj,var_name);
		sprintf(var_name,"dim3Light_%d.invertIntensity",n);
		shader->var_locs.dim3Lights[n].invertIntensity=glGetUniformLocation(shader->program_obj,var_name);
		sprintf(var_name,"dim3Light_%d.exponent",n);
		shader->var_locs.dim3Lights[n].exponent=glGetUniformLocation(shader->program_obj,var_name);
		sprintf(var_name,"dim3Light_%d.direction",n);
		shader->var_locs.dim3Lights[n].direction=glGetUniformLocation(shader->program_obj,var_name);
	}
	
		// we remember all the last settings
		// so we don't reset state when no necessary
			
	shader->var_values.attrib.vertex_offset=-1;
	shader->var_values.attrib.color_offset=-1;
	shader->var_values.attrib.uv_offset=-1;
	shader->var_values.attrib.lmap_uv_offset=-1;
	shader->var_values.attrib.tangent_offset=-1;
	shader->var_values.attrib.normal_offset=-1;
	
	shader->var_values.simple_color.r=shader->var_values.simple_color.g=shader->var_values.simple_color.b=shader->var_values.simple_color.a=-1.0f;

	shader->var_values.alpha=-1.0f;
	shader->var_values.shine_factor=-1.0f;
	shader->var_values.glow_factor=-1.0f;
	shader->var_values.diffuse_vct.x=shader->var_values.diffuse_vct.y=shader->var_values.diffuse_vct.z=-1.0f;

	gl_shader_reset_light_values(shader);
}

/* =======================================================

      Shader Error Reporter
      
======================================================= */

bool gl_shader_report_shader_error(char *err_str,char *vertex_name,char *fragment_name,GLuint shader_obj,char *err_type,char *code,int check_type)
{
	int				line;
	GLint			result,len;
	char			*c,*str,path[1024],log_line[256];
	FILE			*file;
	struct tm		*tm;
	time_t			curtime;
	
		// operation OK?
		
	glGetShaderiv(shader_obj,check_type,&result);
	if (result==1) return(TRUE);
	
		// start or append log file
		
	file_paths_app_data(&file_path_setup,path,"Debug","glsl_error_log","txt");

	file=fopen(path,"w");
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
		
	glGetShaderiv(shader_obj,GL_INFO_LOG_LENGTH,&len);
	if (len>0) {
		str=malloc(len);
		if (str!=NULL) {
			glGetShaderInfoLog(shader_obj,len,NULL,str);

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

	strcpy(err_str,"GLSL: Could not compile shader.\nSee Debug\\glsl_error_log.txt for more information");
	
	return(FALSE);
}

bool gl_shader_report_program_error(char *err_str,char *vertex_name,char *fragment_name,GLuint program_obj,char *err_type,char *code,int check_type)
{
	int				line;
	GLint			result,len;
	char			*c,*str,path[1024],log_line[256];
	FILE			*file;
	struct tm		*tm;
	time_t			curtime;
	
		// operation OK?
		
	glGetProgramiv(program_obj,check_type,&result);
	if (result==1) return(TRUE);
	
		// start or append log file
		
	file_paths_app_data(&file_path_setup,path,"Debug","glsl_error_log","txt");

	file=fopen(path,"w");
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
		
	glGetProgramiv(program_obj,GL_INFO_LOG_LENGTH,&len);
	if (len>0) {
		str=malloc(len);
		if (str!=NULL) {
			glGetProgramInfoLog(program_obj,len,NULL,str);

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

	strcpy(err_str,"GLSL: Could not link shader program.\nSee Debug\\glsl_error_log.txt for more information");
	
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
	const GLchar			*vertex_ptr,*fragment_ptr;

		// create the shader
			
	shader->program_obj=glCreateProgram();
		
	shader->vertex_obj=glCreateShader(GL_VERTEX_SHADER);
	
	vertex_ptr=vertex_data;
	glShaderSource(shader->vertex_obj,1,&vertex_ptr,NULL);

	glCompileShader(shader->vertex_obj);
	if (!gl_shader_report_shader_error(err_str,shader->vertex_name,NULL,shader->vertex_obj,"Vertex",vertex_data,GL_COMPILE_STATUS)) return(FALSE);

	glAttachShader(shader->program_obj,shader->vertex_obj);
	
	shader->fragment_obj=glCreateShader(GL_FRAGMENT_SHADER);
	
	fragment_ptr=fragment_data;
	glShaderSource(shader->fragment_obj,1,&fragment_ptr,NULL);
	
	glCompileShader(shader->fragment_obj);
	if (!gl_shader_report_shader_error(err_str,NULL,shader->fragment_name,shader->fragment_obj,"Fragment",fragment_data,GL_COMPILE_STATUS)) return(FALSE);
	
	glAttachShader(shader->program_obj,shader->fragment_obj);
	
		// link shaders into program
		
	glLinkProgram(shader->program_obj);
	if (!gl_shader_report_program_error(err_str,shader->vertex_name,shader->fragment_name,shader->program_obj,"Program",NULL,GL_LINK_STATUS)) return(FALSE);

		// set the shader and then
		// setup the uniforms and attributes

	glUseProgram(shader->program_obj);

	gl_shader_set_instance_variables(shader);
	gl_shader_cache_dynamic_variable_locations(shader);

	glUseProgram(0);
	
		// timing only works for full screen shaders
		
	shader->start_tick=0;

	return(TRUE);
}

void gl_shader_code_shutdown(shader_type *shader)
{
	if (shader->vertex_obj!=0) {
		glDetachShader(shader->program_obj,shader->vertex_obj);
		glDeleteShader(shader->vertex_obj);
	}

	if (shader->fragment_obj!=0) {
		glDetachShader(shader->program_obj,shader->fragment_obj);
		glDeleteShader(shader->fragment_obj);
	}

	if (shader->program_obj!=0) glDeleteProgram(shader->program_obj);
}

/* =======================================================

      Drawing Matrix Shader Variables
      
======================================================= */

void gl_shader_set_draw_matrix_variables(shader_type *shader)
{
		// only change matrix when we've reset any
		// of them, which happens per frame and anytime
		// certain view calls are made
		
	if (!shader->need_matrix_reset) return;
	
		// reset the matrixes
		
	shader->need_matrix_reset=FALSE;
	
	if (shader->var_locs.dim3ProjectionMatrix!=-1) glUniformMatrix4fv(shader->var_locs.dim3ProjectionMatrix,1,GL_FALSE,gl_proj_matrix);
	if (shader->var_locs.dim3ModelViewMatrix!=-1) glUniformMatrix4fv(shader->var_locs.dim3ModelViewMatrix,1,GL_FALSE,gl_model_view_matrix);
	if (shader->var_locs.dim3NormalMatrix!=-1) glUniformMatrix3fv(shader->var_locs.dim3NormalMatrix,1,GL_FALSE,gl_normal_matrix);
}

/* =======================================================

      Shader Per Frame Setup
      
======================================================= */

inline void gl_shader_frame_start_per_shader(shader_type *shader)
{
		// this flag marks when we need to reset
		// the matrixes (projection, model view, etc)
		// it's set per frame and then can get reset
		// if any of the specific view calls are hit
		
	shader->need_matrix_reset=TRUE;
	
		// use this flag to mark frame only variables
		// as needing a load.  In this way we optimize
		// out the amount of variable setting we need to do
	
	shader->need_frame_reset=TRUE;
		
		// dim3 tries to remember if the same lights
		// we set by looking into the light list.  The
		// light list is generated per frame, so it needs
		// to be reset here
		
		// need to set current hilite to 0, which means
		// it hasn't been set yet

	shader->var_values.nlight=-1;
	shader->current_hilite=0;
}

void gl_shader_frame_start(void)
{
	gl_shader_frame_start_per_shader(&color_shader);
	gl_shader_frame_start_per_shader(&gradient_shader);
	gl_shader_frame_start_per_shader(&black_shader);
	gl_shader_frame_start_per_shader(&bitmap_shader);
}

void gl_shader_force_matrix_resets(void)
{
	color_shader.need_matrix_reset=TRUE;
	gradient_shader.need_matrix_reset=TRUE;
	black_shader.need_matrix_reset=TRUE;
	bitmap_shader.need_matrix_reset=TRUE;
}


