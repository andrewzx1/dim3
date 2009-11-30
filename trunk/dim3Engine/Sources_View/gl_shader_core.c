/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Core Shader Routines

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

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "video.h"

extern map_type				map;
extern setup_type			setup;
extern view_type			view;
extern render_info_type		render_info;

shader_type					core_shaders[max_core_shader];

/* =======================================================

      Build Single Core Shaders
      
======================================================= */

char* gl_core_shader_build_vert(int nlight,bool light_map,bool diffuse,bool bump,bool spec)
{
	int				n;
	char			*buf;

		// memory for shader

	buf=(char*)malloc(max_core_shader_data_sz);
	if (buf==NULL) return(NULL);

	bzero(buf,max_core_shader_data_sz);

		// built vert shader

	strcat(buf,"uniform vec3 dim3CameraPosition");
	if (nlight>0) sprintf(strchr(buf,0),",dim3LightPosition[%d]",nlight);
	strcat(buf,";\n");

	if ((diffuse) || (nlight>0)) {
		strcat(buf,"varying vec3 ");
		if (diffuse) strcat(buf,"normal");
		if (nlight>0) {
			if (diffuse) strcat(buf,",");
			sprintf(strchr(buf,0),"lightVector[%d]",nlight);
		}
		strcat(buf,";\n");
	}
	
	strcat(buf,"void main(void)\n");
	strcat(buf,"{\n");

	for (n=0;n!=nlight;n++) {
		sprintf(strchr(buf,0),"lightVector[%d]=dim3LightPosition[%d]-gl_Vertex.xyz;\n",n,n);
	}
	
	if (diffuse) strcat(buf,"normal=normalize(gl_Normal);\n");

	strcat(buf,"gl_Position=ftransform();\n");
	strcat(buf,"gl_TexCoord[0]=gl_MultiTexCoord0;\n");
	if (light_map) strcat(buf,"gl_TexCoord[1]=gl_MultiTexCoord1;\n");
	strcat(buf,"}\n");

	return(buf);
}

char* gl_core_shader_build_frag(int nlight,bool light_map,bool diffuse,bool bump,bool spec)
{
	int				n;
	char			*buf;

		// memory for shader

	buf=(char*)malloc(max_core_shader_data_sz);
	if (buf==NULL) return(NULL);

	bzero(buf,max_core_shader_data_sz);

		// build frag shader
	
	strcat(buf,"uniform sampler2D dim3Tex");
	if (bump) strcat(buf,",dim3TexBump");
	if (spec) strcat(buf,",dim3TexSpecular");
	if (light_map) strcat(buf,",dim3TexLightMap");
	strcat(buf,";\n");

	strcat(buf,"uniform float dim3Alpha");
	if (!diffuse) strcat(buf,",dim3DarkFactor");
	if (bump) strcat(buf,",dim3BumpFactor");
	if (spec) strcat(buf,",dim3SpecularFactor");
	strcat(buf,";\n");

	strcat(buf,"uniform vec3 dim3AmbientColor;\n");

	if (nlight>0) {
		sprintf(strchr(buf,0),"uniform float dim3LightIntensity[%d],dim3LightExponent[%d];\n",nlight,nlight);
		sprintf(strchr(buf,0),"uniform vec3 dim3LightColor[%d],dim3LightDirection[%d];\n",nlight,nlight);
	}
	
	if ((diffuse) || (nlight>0)) {
		strcat(buf,"varying vec3 ");
		if (diffuse) strcat(buf,"normal");
		if (nlight>0) {
			if (diffuse) strcat(buf,",");
			sprintf(strchr(buf,0),"lightVector[%d]",nlight);
		}
		strcat(buf,";\n");
	}
	
	strcat(buf,"void main(void)\n");
	strcat(buf,"{\n");
	
	if ((bump) || (spec) || (diffuse)) strcat(buf,"int combineCount=0;\n");
	
	strcat(buf,"float att,dist");
	if ((bump) || (spec)) strcat(buf,",pixelAtt");
	if (bump) strcat(buf,",bump");
	if (spec) strcat(buf,",spec,shineFactor,specMap");
	strcat(buf,";\n");

	strcat(buf,"vec3 ambient=dim3AmbientColor");
	if (diffuse) strcat(buf,",diffuse,lightNormal");
	if ((bump) || (diffuse)) strcat(buf,",combineLightVector=vec3(0.0,0.0,0.0)");
	if (bump) strcat(buf,",bumpVector,bumpMap");
	strcat(buf,";\n");

	strcat(buf,"vec4 tex;\n");

		// the light map

	if (light_map) strcat(buf,"ambient+=texture2D(dim3TexLightMap,gl_TexCoord[1].st).rgb;\n");

		// the texture lighting
		
	for (n=0;n!=nlight;n++) {
		sprintf(strchr(buf,0),"dist=length(lightVector[%d]);\n",n);
		sprintf(strchr(buf,0),"if (dist<dim3LightIntensity[%d]) {\n",n);
		sprintf(strchr(buf,0)," if (dot(lightVector[%d],dim3LightDirection[%d])>=0.0) {\n",n,n);
		sprintf(strchr(buf,0),"  att=1.0-(dist/dim3LightIntensity[%d]);\n",n);
		sprintf(strchr(buf,0),"  att+=pow(att,dim3LightExponent[%d]);\n",n);
		sprintf(strchr(buf,0),"  ambient+=(dim3LightColor[%d]*att);\n",n);
		if ((bump) || (diffuse)) {
			sprintf(strchr(buf,0),"  combineLightVector+=lightVector[%d];\n",n);
			strcat(buf,"  combineCount++;\n");
		}
		strcat(buf," }\n");
		strcat(buf,"}\n");
	}

		// the factor for bump/spec effect based on light

	if ((bump) || (spec)) strcat(buf,"pixelAtt=min(((ambient.r+ambient.g+ambient.b)*0.33),1.0);\n");
	
		// normalize the combined light vec3
		
	if ((bump) || (diffuse)) strcat(buf,"if (combineCount!=0) combineLightVector=normalize(combineLightVector/float(combineCount));\n");
	
		// texture
		
	strcat(buf,"tex=texture2D(dim3Tex,gl_TexCoord[0].st);\n");
	
		// diffuse
		
	if (diffuse) {
		strcat(buf,"diffuse=vec3(max(dot(normal,combineLightVector),0.0));\n");
		strcat(buf,"diffuse=max((diffuse*1.2),dim3AmbientColor);\n");
		strcat(buf,"diffuse=clamp(diffuse,0.0,1.0);\n");
	}
	
		// bump
		
		// we use the pixelAtt (which is how much light is on the pixel) to
		// create a smooth transition between areas with no bumps vs bumps
		
	if (bump) {
		strcat(buf,"bumpVector=max(abs(combineLightVector),0.5);\n");
		strcat(buf,"bumpVector=normalize((bumpVector*pixelAtt)+(vec3(0.5,0.5,0.5)*(1.0-pixelAtt)));\n");
		strcat(buf,"bumpMap=texture2D(dim3TexBump,gl_TexCoord[0].st).rgb;\n");
		strcat(buf,"bumpMap=(bumpMap-0.5)*2.0;\n");
		strcat(buf,"bump=max(dot(bumpMap,bumpVector),0.0)*dim3BumpFactor;\n");
	}

		// specular
		
		// we are doing this a cheap and quick way.  We calculate a center of screen area
		// (i.e., looking directly at surface) to light surface more.  This gives a simple
		// but ok effect of the surface becoming more shiny
		
	if (spec) {
		strcat(buf,"spec=texture2D(dim3TexSpecular,gl_TexCoord[0].st).r*dim3SpecularFactor*pixelAtt;\n");
		sprintf(strchr(buf,0),"shineFactor=1.0-(distance(gl_FragCoord.xy,vec2(%d.0,%d.0))/%d.0);\n",(setup.screen.x_sz>>1),(setup.screen.y_sz>>1),(int)(sqrt((setup.screen.x_sz*setup.screen.x_sz)+(setup.screen.y_sz+setup.screen.y_sz))*0.25));
		strcat(buf,"spec=(spec+clamp((spec*shineFactor),0.0,1.0))");
		if (bump) strcat(buf,"*bump");
		strcat(buf,";\n");
	}

		// output the fragment

	strcat(buf,"gl_FragColor.rgb=");
	if (bump) strcat(buf,"(");
	if (spec) strcat(buf,"(");
	strcat(buf,"(tex.rgb*ambient)");
	if (bump) strcat(buf,"*bump)");
	if (spec) strcat(buf,"+spec)");
	if (diffuse) {
		strcat(buf,"*diffuse;\n");
	}
	else {
		strcat(buf,"*dim3DarkFactor;\n");
	}
	
	strcat(buf,"gl_FragColor.a=tex.a*dim3Alpha;\n");
	strcat(buf,"}\n");

	return(buf);
}

/* =======================================================

      Create Single Core Shader
      
======================================================= */

bool gl_core_shader_create(shader_type *shader,int nlight,bool light_map,bool diffuse,bool bump,bool spec,char *err_str)
{
	char				*vertex_data,*fragment_data;
	bool				ok;

		// create the shader code

	vertex_data=gl_core_shader_build_vert(nlight,light_map,diffuse,bump,spec);
	if (vertex_data==NULL) {
		strcpy(err_str,"Out of Memory");
		return(FALSE);
	}

	fragment_data=gl_core_shader_build_frag(nlight,light_map,diffuse,bump,spec);
	if (fragment_data==NULL) {
		free(vertex_data);
		strcpy(err_str,"Out of Memory");
		return(FALSE);
	}
	
	sprintf(shader->name,"core_%d_%s_%s_%s_%s",nlight,(light_map?"T":"F"),(diffuse?"T":"F"),(bump?"T":"F"),(spec?"T":"F"));
	sprintf(shader->vertex_name,"%s_vert",shader->name);
	sprintf(shader->fragment_name,"%s_frag",shader->name);
	
		// compile the code

	ok=gl_shader_code_compile(shader,vertex_data,fragment_data,err_str);

		// free the code

	free(vertex_data);
	free(fragment_data);

	return(ok);
}

/* =======================================================

      Core Shader Initialize/Shutdown
      
======================================================= */

bool gl_core_shader_initialize(char *err_str)
{
	int					n;

	if (!gl_check_shader_ok()) return(TRUE);

		// clear core shaders

	for (n=0;n!=max_core_shader;n++) {
		gl_shader_code_clear(&core_shaders[n]);
	}
	
		// gl_core_shader_light

	if (!gl_core_shader_create(&core_shaders[gl_core_shader_light],max_shader_light,FALSE,FALSE,FALSE,FALSE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_shader_light_bump

	if (!gl_core_shader_create(&core_shaders[gl_core_shader_light_bump],max_shader_light,FALSE,FALSE,TRUE,FALSE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_shader_light_spec

	if (!gl_core_shader_create(&core_shaders[gl_core_shader_light_spec],max_shader_light,FALSE,FALSE,FALSE,TRUE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_shader_light_bump_spec

	if (!gl_core_shader_create(&core_shaders[gl_core_shader_light_bump_spec],max_shader_light,FALSE,FALSE,TRUE,TRUE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_shader_light_map

	if (!gl_core_shader_create(&core_shaders[gl_core_shader_light_map],max_shader_light,TRUE,FALSE,FALSE,FALSE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_shader_light_map_bump

	if (!gl_core_shader_create(&core_shaders[gl_core_shader_light_map_bump],max_shader_light,TRUE,FALSE,TRUE,FALSE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_shader_light_map_spec

	if (!gl_core_shader_create(&core_shaders[gl_core_shader_light_map_spec],max_shader_light,TRUE,FALSE,FALSE,TRUE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_shader_light_map_bump_spec

	if (!gl_core_shader_create(&core_shaders[gl_core_shader_light_map_bump_spec],max_shader_light,TRUE,FALSE,TRUE,TRUE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_shader_light_diffuse

	if (!gl_core_shader_create(&core_shaders[gl_core_shader_light_diffuse],max_shader_light,FALSE,TRUE,FALSE,FALSE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_shader_light_diffuse_bump

	if (!gl_core_shader_create(&core_shaders[gl_core_shader_light_diffuse_bump],max_shader_light,FALSE,TRUE,TRUE,FALSE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_shader_light_diffuse_spec

	if (!gl_core_shader_create(&core_shaders[gl_core_shader_light_diffuse_spec],max_shader_light,FALSE,TRUE,FALSE,TRUE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_shader_light_diffuse_bump_spec

	if (!gl_core_shader_create(&core_shaders[gl_core_shader_light_diffuse_bump_spec],max_shader_light,FALSE,TRUE,TRUE,TRUE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}
	
	return(TRUE);
}

void gl_core_shader_shutdown(void)
{
	int					n;

	if (!gl_check_shader_ok()) return;

		// shutdown shaders

	for (n=0;n!=max_core_shader;n++) {
		gl_shader_code_shutdown(&core_shaders[n]);
	}
}

/* =======================================================

      Per-Scene Core Shader Initialization
      
======================================================= */

void gl_core_shader_draw_scene_initialize(void)
{
	int					n;
	
	for (n=0;n!=max_core_shader;n++) {
		gl_shader_draw_scene_initialize_code(&core_shaders[n]);
	}
}

/* =======================================================

      Core Shader Utilities
      
======================================================= */

int gl_core_shader_find_for_mode(texture_type *texture,bool diffuse,bool light_map)
{
	bool				bump,spec;
	
		// are bump and spec on?
		
	bump=(texture->frames[0].bumpmap.gl_id!=-1);
	spec=(texture->frames[0].specularmap.gl_id!=-1);
	
		// pick correct shader
	
	if (diffuse) {
		if ((bump) && (spec)) return(gl_core_shader_light_diffuse_bump_spec);
		if ((bump) && (!spec)) return(gl_core_shader_light_diffuse_bump);
		if ((!bump) && (spec)) return(gl_core_shader_light_diffuse_spec);
		return(gl_core_shader_light_diffuse);
	}

	if (light_map) {
		if ((bump) && (spec)) return(gl_core_shader_light_map_bump_spec);
		if ((bump) && (!spec)) return(gl_core_shader_light_map_bump);
		if ((!bump) && (spec)) return(gl_core_shader_light_map_spec);
		return(gl_core_shader_light_map);
	}

	if ((bump) && (spec)) return(gl_core_shader_light_bump_spec);
	if ((bump) && (!spec)) return(gl_core_shader_light_bump);
	if ((!bump) && (spec)) return(gl_core_shader_light_spec);
	return(gl_core_shader_light);
}

shader_type* gl_core_shader_find_ptr(int nlight,texture_type *texture,bool diffuse,bool light_map)
{
	int				idx;
	
	idx=gl_core_shader_find_for_mode(texture,diffuse,light_map);
	return(&core_shaders[idx]);
}

