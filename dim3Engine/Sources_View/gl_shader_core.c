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
	sprintf(strchr(buf,0),",dim3LightPosition[%d]",nlight);
	strcat(buf,";\n");

	strcat(buf,"varying vec3 ");
	if (spec) strcat(buf,"viewVector,lightHalfVector,");
	sprintf(strchr(buf,0),"lightVector[%d];\n",nlight);

	strcat(buf,"void main(void)\n");
	strcat(buf,"{\n");

	if (spec) {
		strcat(buf,"viewVector=gl_Vertex.xyz-dim3CameraPosition;\n");
		strcat(buf,"lightHalfVector=(dim3LightPosition[0]-dim3CameraPosition)+viewVector;\n");
	}

	for (n=0;n!=nlight;n++) {
		sprintf(strchr(buf,0),"lightVector[%d]=dim3LightPosition[%d]-gl_Vertex.xyz;\n",n,n);
	}

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

	strcat(buf,"uniform float dim3Alpha,dim3DarkFactor");
	if (bump) strcat(buf,",dim3BumpFactor");
	if (spec) strcat(buf,",dim3SpecularFactor");
	strcat(buf,";\n");

	strcat(buf,"uniform vec3 dim3AmbientColor;\n");

	sprintf(strchr(buf,0),"uniform float dim3LightIntensity[%d],dim3LightExponent[%d];\n",nlight,nlight);
	sprintf(strchr(buf,0),"uniform vec3 dim3LightColor[%d],dim3LightDirection[%d];\n",nlight,nlight);

	strcat(buf,"varying vec3 ");
	if (spec) strcat(buf,"viewVector,lightHalfVector,");
	sprintf(strchr(buf,0),"lightVector[%d];\n",nlight);

	strcat(buf,"void main(void)\n");
	strcat(buf,"{\n");
	strcat(buf,"float att,dist");
	if ((bump) || (spec)) strcat(buf,",pixelAtt");
	if (bump) strcat(buf,",bump");
	if (spec) strcat(buf,",spec,specMap");
	strcat(buf,";\n");

	strcat(buf,"vec3 ambient=dim3AmbientColor");
	if (bump) strcat(buf,",bumpVector=vec3(0.5,0.5,0.5),bumpMap");
	if (spec) strcat(buf,",specVector,specLightHalfVector");
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
		if (bump) sprintf(strchr(buf,0),"  bumpVector+=((bumpVector+lightVector[%d])*0.5);\n",n);
		strcat(buf," }\n");
		strcat(buf,"}\n");
	}

		// the factor for bump/spec effect based on light

	if ((bump) || (spec)) strcat(buf,"pixelAtt=min((ambient.r+ambient.g+ambient.b),1.0);\n");
	
		// texture
		
	strcat(buf,"tex=texture2D(dim3Tex,gl_TexCoord[0].st);\n");
	
		// bump
		
	if (bump) {
		strcat(buf,"bumpVector=max(abs(normalize(bumpVector)),0.5);\n");
		strcat(buf,"bumpVector=normalize((bumpVector*pixelAtt)+(vec3(0.5,0.5,0.5)*(1.0-pixelAtt)));\n");
		strcat(buf,"bumpMap=texture2D(dim3TexBump,gl_TexCoord[0].st).rgb;\n");
		strcat(buf,"bumpMap=(bumpMap-0.5)*2.0;\n");
		strcat(buf,"bump=max(dot(bumpMap,bumpVector),0.0)*dim3BumpFactor;\n");
	}

		// specular
		
	if (spec) {
		strcat(buf,"specVector=normalize(viewVector);\n");
		strcat(buf,"specLightHalfVector=normalize(lightHalfVector);\n");
		strcat(buf,"specMap=texture2D(dim3TexSpecular,gl_TexCoord[0].st).r;\n");
		strcat(buf,"spec=(specMap*pow(max(dot(specLightHalfVector,specVector),0.0),10.0))*dim3SpecularFactor*pixelAtt;\n");
	}

		// output the fragment

	strcat(buf,"gl_FragColor.rgb=");
	if (bump) strcat(buf,"(");
	if (spec) strcat(buf,"(");
	strcat(buf,"(tex.rgb*ambient)");
	if (bump) strcat(buf,"*bump)");
	if (spec) strcat(buf,"+spec)");
	strcat(buf,"*dim3DarkFactor;\n");

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
	shader_type			*shader;

	if (!gl_check_shader_ok()) return(TRUE);

		// clear core shaders

	shader=core_shaders;

	for (n=0;n!=max_core_shader;n++) {
		gl_shader_code_clear(shader);
		shader++;

		// gl_core_shader_light

	if (!gl_core_shader_create(&core_shaders[gl_core_shader_light],max_shader_light,FALSE,FALSE,FALSE,FALSE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_shader_light_bump

	if (!gl_core_shader_create(&core_shaders[gl_core_shader_light],max_shader_light,FALSE,FALSE,TRUE,FALSE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_shader_light_spec

	if (!gl_core_shader_create(&core_shaders[gl_core_shader_light],max_shader_light,FALSE,FALSE,FALSE,TRUE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_shader_light_bump_spec

	if (!gl_core_shader_create(&core_shaders[gl_core_shader_light],max_shader_light,FALSE,FALSE,TRUE,TRUE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_shader_light_map

	if (!gl_core_shader_create(&core_shaders[gl_core_shader_light],max_shader_light,TRUE,FALSE,FALSE,FALSE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_shader_light_map_bump

	if (!gl_core_shader_create(&core_shaders[gl_core_shader_light],max_shader_light,TRUE,FALSE,TRUE,FALSE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_shader_light_map_spec

	if (!gl_core_shader_create(&core_shaders[gl_core_shader_light],max_shader_light,TRUE,FALSE,FALSE,TRUE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_shader_light_map_bump_spec

	if (!gl_core_shader_create(&core_shaders[gl_core_shader_light],max_shader_light,TRUE,FALSE,TRUE,TRUE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_shader_light_diffuse

	if (!gl_core_shader_create(&core_shaders[gl_core_shader_light],max_shader_light,FALSE,TRUE,FALSE,FALSE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_shader_light_diffuse_bump

	if (!gl_core_shader_create(&core_shaders[gl_core_shader_light],max_shader_light,FALSE,TRUE,TRUE,FALSE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_shader_light_diffuse_spec

	if (!gl_core_shader_create(&core_shaders[gl_core_shader_light],max_shader_light,FALSE,TRUE,FALSE,TRUE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_shader_light_diffuse_bump_spec

	if (!gl_core_shader_create(&core_shaders[gl_core_shader_light],max_shader_light,FALSE,TRUE,TRUE,TRUE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

	return(TRUE);
}

void gl_core_shader_shutdown(void)
{
	int					n;
	shader_type			*shader;

	if (!gl_check_shader_ok()) return;

		// shutdown shaders

	shader=core_shaders;

	for (n=0;n!=max_core_shader;n++) {
		gl_shader_code_shutdown(shader);
		shader++;
	}
}

/* =======================================================

      Core Shader Utilities
      
======================================================= */

int gl_core_shader_find(texture_type *texture,bool diffuse,bool light_map)
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

