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

extern bool fog_solid_on(void);

extern map_type				map;
extern setup_type			setup;
extern view_type			view;
extern render_info_type		render_info;

shader_type					core_shaders[max_shader_light+1][max_core_shader];

/* =======================================================

      Generic GLSL Light Structure
      
======================================================= */

void gl_core_shader_build_generic_light_struct(int nlight,char *buf)
{
	int			n;
	
	if (nlight==0) return;
	
	strcat(buf,"struct dim3LightType {\n");
	strcat(buf," vec3 position;\n");
	strcat(buf," vec3 color;\n");
	strcat(buf," float intensity;\n");
	strcat(buf," float exponent;\n");
	strcat(buf," vec3 direction;\n");
	strcat(buf," bool inLightMap;\n");
	strcat(buf,"};\n");
	
	strcat(buf,"uniform dim3LightType ");
	
	for (n=0;n!=nlight;n++) {
		if (n!=0) strcat(buf,",");
		sprintf(strchr(buf,0),"dim3Light_%d",n);
	}
	
	strcat(buf,";\n");
}

/* =======================================================

      Build Single Core Map Shaders
      
======================================================= */

char* gl_core_map_shader_build_vert(int nlight,bool fog,bool light_map,bool bump,bool spec)
{
	int				n;
	char			*buf;

		// memory for shader

	buf=(char*)malloc(max_core_shader_data_sz);
	if (buf==NULL) return(NULL);

	bzero(buf,max_core_shader_data_sz);

		// build vert shader
		
	gl_core_shader_build_generic_light_struct(nlight,buf);

	strcat(buf,"uniform vec3 dim3CameraPosition");
	if ((bump) || (spec)) strcat(buf,",dim3Tangent,dim3Binormal,dim3Normal");
	strcat(buf,";\n");
	
	if (fog) strcat(buf,"varying float fogFactor;\n");

	sprintf(strchr(buf,0),"varying vec3 lightVector[%d]",max_shader_light);
	if (bump) sprintf(strchr(buf,0),",lightVertexVector[%d]",max_shader_light);
	if (spec) strcat(buf,",eyeVector");
	strcat(buf,";\n");
	
	strcat(buf,"void main(void)\n");
	strcat(buf,"{\n");

	strcat(buf,"gl_Position=ftransform();\n");
	strcat(buf,"gl_TexCoord[0]=gl_MultiTexCoord0;\n");
	if (light_map) strcat(buf,"gl_TexCoord[1]=gl_MultiTexCoord1;\n");

	strcat(buf,"vec3 vtx=vec3(gl_ModelViewMatrix*gl_Vertex);\n");
	
	if ((bump) || (spec)) {
		strcat(buf,"vec3 tangentSpaceTangent=normalize(gl_NormalMatrix*dim3Tangent);\n");
		strcat(buf,"vec3 tangentSpaceBinormal=normalize(gl_NormalMatrix*dim3Binormal);\n");
		strcat(buf,"vec3 tangentSpaceNormal=normalize(gl_NormalMatrix*dim3Normal);\n");
	}
	
	if (spec) {
		strcat(buf,"eyeVector.x=dot(-vtx,tangentSpaceTangent);\n");
		strcat(buf,"eyeVector.y=dot(-vtx,tangentSpaceBinormal);\n");
		strcat(buf,"eyeVector.z=dot(-vtx,tangentSpaceNormal);\n");
	}
	
	for (n=0;n!=nlight;n++) {
		sprintf(strchr(buf,0),"lightVector[%d]=dim3Light_%d.position-vtx;\n",n,n);
		
		if (bump) {
			sprintf(strchr(buf,0),"lightVertexVector[%d].x=dot(lightVector[%d],tangentSpaceTangent);\n",n,n);
			sprintf(strchr(buf,0),"lightVertexVector[%d].y=dot(lightVector[%d],tangentSpaceBinormal);\n",n,n);
			sprintf(strchr(buf,0),"lightVertexVector[%d].z=dot(lightVector[%d],tangentSpaceNormal);\n",n,n);
		}
	}
	
	if (fog) strcat(buf,"fogFactor=clamp(((gl_Fog.end-distance(gl_Vertex.xyz,dim3CameraPosition))*gl_Fog.scale),0.0,1.0);\n");

	strcat(buf,"}\n");

	return(buf);
}

char* gl_core_map_shader_build_frag(int nlight,bool fog,bool light_map,bool bump,bool spec)
{
	int				n;
	char			*buf;

		// memory for shader

	buf=(char*)malloc(max_core_shader_data_sz);
	if (buf==NULL) return(NULL);

	bzero(buf,max_core_shader_data_sz);

		// build frag shader
		
	gl_core_shader_build_generic_light_struct(nlight,buf);
	
	strcat(buf,"uniform sampler2D dim3Tex");
	if (bump) strcat(buf,",dim3TexBump");
	if (spec) strcat(buf,",dim3TexSpecular");
	if (light_map) strcat(buf,",dim3TexLightMap");
	strcat(buf,";\n");

	strcat(buf,"uniform float dim3Alpha");
	if (light_map) strcat(buf,",dim3LightMapBoost");
	if (spec) strcat(buf,",dim3ShineFactor");
	strcat(buf,";\n");
	
	strcat(buf,"uniform vec3 dim3AmbientColor;\n");
	
	if (fog) strcat(buf,"varying float fogFactor;\n");
	
	sprintf(strchr(buf,0),"varying vec3 lightVector[%d]",max_shader_light);
	if (bump) sprintf(strchr(buf,0),",lightVertexVector[%d]",max_shader_light);
	if (spec) strcat(buf,",eyeVector");
	strcat(buf,";\n");
	
	strcat(buf,"void main(void)\n");
	strcat(buf,"{\n");
	
	if ((bump) || (spec)) strcat(buf,"int combineCount=0;\n");
	
	strcat(buf,"float att,dist");
	if (bump) strcat(buf,",bump");
	if (spec) strcat(buf,",specFactor");
	strcat(buf,";\n");

	strcat(buf,"vec3 ambient=dim3AmbientColor");
	if (light_map) strcat(buf,",lmap");
	if (bump) strcat(buf,",bumpVertexVector=vec3(0.0,0.0,0.0),bumpMap");
	if (spec) strcat(buf,",specHalfVector=vec3(0.0,0.0,0.0),specMap,spec");
	if (fog) strcat(buf,",frag");
	strcat(buf,";\n");

	strcat(buf,"vec4 tex;\n");

		// the light map

	if (light_map) {
		strcat(buf,"lmap=texture2D(dim3TexLightMap,gl_TexCoord[1].st).rgb;\n");
		strcat(buf,"ambient+=(lmap+(lmap*dim3LightMapBoost));\n");
	}
	
		// the texture lighting
		
	for (n=0;n!=nlight;n++) {
		sprintf(strchr(buf,0),"dist=length(lightVector[%d]);\n",n);
		sprintf(strchr(buf,0),"if (dist<dim3Light_%d.intensity) {\n",n);
		sprintf(strchr(buf,0)," if (dot(lightVector[%d],dim3Light_%d.direction)>=0.0) {\n",n,n);
		sprintf(strchr(buf,0),"  if (!dim3Light_%d.inLightMap) {\n",n);
		sprintf(strchr(buf,0),"   att=1.0-(dist/dim3Light_%d.intensity);\n",n);
		sprintf(strchr(buf,0),"   att+=pow(att,dim3Light_%d.exponent);\n",n);
		sprintf(strchr(buf,0),"   ambient+=(dim3Light_%d.color*att);\n",n);
		strcat(buf,"  }\n");
		if (bump) {
			if (bump) sprintf(strchr(buf,0),"  bumpVertexVector+=lightVertexVector[%d];\n",n);
			strcat(buf,"  combineCount++;\n");
		}
		strcat(buf," }\n");
		strcat(buf,"}\n");
	}
	
		// need a pixel darkening to help even out
		// switches between light sources
		
	if ((bump) || (spec)) strcat(buf,"float pixelAtt=(ambient.r+ambient.g+ambient.b)/3.0;\n");
	
		// normalize the combined light vec3
		
	if (bump) strcat(buf,"if (combineCount!=0) bumpVertexVector=normalize(bumpVertexVector/float(combineCount));\n");
	
		// texture
		
	strcat(buf,"tex=texture2D(dim3Tex,gl_TexCoord[0].st);\n");
	
		// bump
		
	if (bump) {
		strcat(buf,"bumpMap=normalize((texture2D(dim3TexBump,gl_TexCoord[0].st).rgb*2.0)-1.0);\n");
		strcat(buf,"bumpMap.y=-bumpMap.y;\n");
		strcat(buf,"bump=max(dot(bumpVertexVector,bumpMap),0.0);\n");
	}

		// specular
		
	if (spec) {
		strcat(buf,"specHalfVector=normalize(normalize(eyeVector)+bumpVertexVector);\n");
		strcat(buf,"specMap=texture2D(dim3TexSpecular,gl_TexCoord[0].st).rgb;\n");
		strcat(buf,"specFactor=max(dot(bumpMap,normalize(specHalfVector)),0.0);\n");
		strcat(buf,"spec=(specMap*pow(specFactor,dim3ShineFactor))*pixelAtt;\n");
	}

		// output the fragment

	if (!fog) {
		strcat(buf,"gl_FragColor.rgb=");
	}
	else {
		strcat(buf,"frag=");
	}
	
	if (bump) strcat(buf,"(");
	if (spec) strcat(buf,"(");
	strcat(buf,"(tex.rgb*ambient)");
	if (bump) strcat(buf,"*bump)");
	if (spec) strcat(buf,"+spec)");
	strcat(buf,";\n");
	
	if (fog) strcat(buf,"gl_FragColor.rgb=mix(gl_Fog.color.rgb,frag,fogFactor);\n");
	
	strcat(buf,"gl_FragColor.a=tex.a*dim3Alpha;\n");
	strcat(buf,"}\n");

	return(buf);
}

bool gl_core_map_shader_create(shader_type *shader,int nlight,bool fog,bool light_map,bool bump,bool spec,char *err_str)
{
	char				*vertex_data,*fragment_data;
	bool				ok;
	
		// create the shader code

	vertex_data=gl_core_map_shader_build_vert(nlight,fog,light_map,bump,spec);
	if (vertex_data==NULL) {
		strcpy(err_str,"Out of Memory");
		return(FALSE);
	}

	fragment_data=gl_core_map_shader_build_frag(nlight,fog,light_map,bump,spec);
	if (fragment_data==NULL) {
		free(vertex_data);
		strcpy(err_str,"Out of Memory");
		return(FALSE);
	}
	
		// create the name
		
	sprintf(shader->name,"core_map_light:%d",nlight);
	if (fog) strcat(shader->name,"_fog");
	if (light_map) strcat(shader->name,"_lightmap");
	if (bump) strcat(shader->name,"_bump");
	if (spec) strcat(shader->name,"_spec");
	
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

      Build Single Core Map Shaders
      
======================================================= */

char* gl_core_model_shader_build_vert(int nlight,bool fog,bool bump,bool spec)
{
	int				n;
	char			*buf;

		// memory for shader

	buf=(char*)malloc(max_core_shader_data_sz);
	if (buf==NULL) return(NULL);

	bzero(buf,max_core_shader_data_sz);

		// build vert shader
		
	gl_core_shader_build_generic_light_struct(nlight,buf);

	strcat(buf,"uniform vec3 dim3CameraPosition;\n");

	strcat(buf,"attribute vec3 dim3VertexNormal");
	if ((bump) || (spec)) strcat(buf,",dim3VertexTangent,dim3VertexBinormal");
	strcat(buf,";\n");
	
	if (fog) strcat(buf,"varying float fogFactor;\n");
	
	sprintf(strchr(buf,0),"varying vec3 tangentSpaceNormal,lightVector[%d]",max_shader_light);
	if (bump) sprintf(strchr(buf,0),",lightVertexVector[%d]",max_shader_light);
	if (spec) strcat(buf,",eyeVector");
	strcat(buf,";\n");
	
	strcat(buf,"void main(void)\n");
	strcat(buf,"{\n");

	strcat(buf,"gl_Position=ftransform();\n");
	strcat(buf,"gl_TexCoord[0]=gl_MultiTexCoord0;\n");
	
	strcat(buf,"vec3 vtx=vec3(gl_ModelViewMatrix*gl_Vertex);\n");
	
	if ((bump) || (spec)) {
		strcat(buf,"vec3 tangentSpaceTangent=normalize(gl_NormalMatrix*dim3VertexTangent);\n");
		strcat(buf,"vec3 tangentSpaceBinormal=normalize(gl_NormalMatrix*dim3VertexBinormal);\n");
	}
	
	strcat(buf,"tangentSpaceNormal=normalize(gl_NormalMatrix*dim3VertexNormal);\n");	// always need normal for diffuse
	
	if (spec) {
		strcat(buf,"eyeVector.x=dot(-vtx,tangentSpaceTangent);\n");
		strcat(buf,"eyeVector.y=dot(-vtx,tangentSpaceBinormal);\n");
		strcat(buf,"eyeVector.z=dot(-vtx,tangentSpaceNormal);\n");
	}
	
	for (n=0;n!=nlight;n++) {
		sprintf(strchr(buf,0),"lightVector[%d]=dim3Light_%d.position-vtx;\n",n,n);
		
		if (bump) {
			sprintf(strchr(buf,0),"lightVertexVector[%d].x=dot(lightVector[%d],tangentSpaceTangent);\n",n,n);
			sprintf(strchr(buf,0),"lightVertexVector[%d].y=dot(lightVector[%d],tangentSpaceBinormal);\n",n,n);
			sprintf(strchr(buf,0),"lightVertexVector[%d].z=dot(lightVector[%d],tangentSpaceNormal);\n",n,n);
		}
	}
	
	if (fog) strcat(buf,"fogFactor=clamp(((gl_Fog.end-distance(gl_Vertex.xyz,dim3CameraPosition))*gl_Fog.scale),0.0,1.0);\n");
	
	strcat(buf,"}\n");

	return(buf);
}

char* gl_core_model_shader_build_frag(int nlight,bool fog,bool bump,bool spec)
{
	int				n;
	char			*buf;

		// memory for shader

	buf=(char*)malloc(max_core_shader_data_sz);
	if (buf==NULL) return(NULL);

	bzero(buf,max_core_shader_data_sz);

		// build frag shader
		
	gl_core_shader_build_generic_light_struct(nlight,buf);
	
	strcat(buf,"uniform sampler2D dim3Tex");
	if (bump) strcat(buf,",dim3TexBump");
	if (spec) strcat(buf,",dim3TexSpecular");
	strcat(buf,";\n");

	strcat(buf,"uniform float dim3Alpha");
	if (spec) strcat(buf,",dim3ShineFactor");
	strcat(buf,";\n");
	
	strcat(buf,"uniform vec3 dim3AmbientColor;\n");
	
	if (fog) strcat(buf,"varying float fogFactor;\n");
	
	sprintf(strchr(buf,0),"varying vec3 tangentSpaceNormal,lightVector[%d]",max_shader_light);
	if (bump) sprintf(strchr(buf,0),",lightVertexVector[%d]",max_shader_light);
	if (spec) strcat(buf,",eyeVector");
	strcat(buf,";\n");
	
	strcat(buf,"void main(void)\n");
	strcat(buf,"{\n");
	
	strcat(buf,"int combineCount=0;\n");
	
	strcat(buf,"float att,dist");
	if (bump) strcat(buf,",bump");
	if (spec) strcat(buf,",specFactor");
	strcat(buf,";\n");

	strcat(buf,"vec3 ambient=dim3AmbientColor,diffuse,lightNormal,combineLightVector=vec3(0.0,0.0,0.0)");
	if (bump) strcat(buf,",bumpVertexVector=vec3(0.0,0.0,0.0),bumpMap");
	if (spec) strcat(buf,",specHalfVector=vec3(0.0,0.0,0.0),specMap,spec");
	if (fog) strcat(buf,",frag");
	strcat(buf,";\n");

	strcat(buf,"vec4 tex;\n");
	
		// the texture lighting
		
	for (n=0;n!=nlight;n++) {
		sprintf(strchr(buf,0),"dist=length(lightVector[%d]);\n",n);
		sprintf(strchr(buf,0),"if (dist<dim3Light_%d.intensity) {\n",n);
		sprintf(strchr(buf,0)," if (dot(lightVector[%d],dim3Light_%d.direction)>=0.0) {\n",n,n);
		sprintf(strchr(buf,0),"  if (!dim3Light_%d.inLightMap) {\n",n);
		sprintf(strchr(buf,0),"   att=1.0-(dist/dim3Light_%d.intensity);\n",n);
		sprintf(strchr(buf,0),"   att+=pow(att,dim3Light_%d.exponent);\n",n);
		sprintf(strchr(buf,0),"   ambient+=(dim3Light_%d.color*att);\n",n);
		strcat(buf,"  }\n");
		sprintf(strchr(buf,0),"  combineLightVector+=lightVector[%d];\n",n);
		if (bump) sprintf(strchr(buf,0),"  bumpVertexVector+=lightVertexVector[%d];\n",n);
		strcat(buf,"  combineCount++;\n");
		strcat(buf," }\n");
		strcat(buf,"}\n");
	}
	
		// need a pixel darkening to help even out
		// switches between light sources
		
	if ((bump) || (spec)) strcat(buf,"float pixelAtt=(ambient.r+ambient.g+ambient.b)/3.0;\n");
	
		// normalize the combined light vec3
		
	strcat(buf,"if (combineCount!=0) {\n");
	strcat(buf," float invCombineCount=1.0/float(combineCount);\n");
	strcat(buf," combineLightVector=normalize(combineLightVector*invCombineCount);\n");
	if (bump) strcat(buf," bumpVertexVector=normalize(bumpVertexVector*invCombineCount);\n");
	strcat(buf,"}\n");
	
		// texture
		
	strcat(buf,"tex=texture2D(dim3Tex,gl_TexCoord[0].st);\n");
	
		// diffuse
		
	strcat(buf,"diffuse=vec3(max(dot(combineLightVector,tangentSpaceNormal),0.0));\n");
	strcat(buf,"diffuse=max((diffuse*1.2),dim3AmbientColor);\n");
	strcat(buf,"diffuse=clamp(diffuse,0.0,1.0);\n");
	
		// bump
		
	if (bump) {
		strcat(buf,"bumpMap=normalize((texture2D(dim3TexBump,gl_TexCoord[0].st).rgb*2.0)-1.0);\n");
		strcat(buf,"bumpMap.y=-bumpMap.y;\n");
		strcat(buf,"bump=max(dot(bumpVertexVector,bumpMap),0.0);\n");
	}

		// specular
		
	if (spec) {
		strcat(buf,"specHalfVector=normalize(normalize(eyeVector)+bumpVertexVector);\n");
		strcat(buf,"specMap=texture2D(dim3TexSpecular,gl_TexCoord[0].st).rgb;\n");
		strcat(buf,"specFactor=max(dot(bumpMap,normalize(specHalfVector)),0.0);\n");
		strcat(buf,"spec=(specMap*pow(specFactor,dim3ShineFactor))*pixelAtt;\n");
	}

		// output the fragment

	if (!fog) {
		strcat(buf,"gl_FragColor.rgb=");
	}
	else {
		strcat(buf,"frag=");
	}
	
	if (bump) strcat(buf,"(");
	if (spec) strcat(buf,"(");
	strcat(buf,"(tex.rgb*ambient)");
	if (bump) strcat(buf,"*bump)");
	if (spec) strcat(buf,"+spec)");
	strcat(buf,"*diffuse;\n");
	
	if (fog) strcat(buf,"gl_FragColor.rgb=mix(gl_Fog.color.rgb,frag,fogFactor);\n");
	
	strcat(buf,"gl_FragColor.a=tex.a*dim3Alpha;\n");
	strcat(buf,"}\n");

	return(buf);
}

bool gl_core_model_shader_create(shader_type *shader,int nlight,bool fog,bool bump,bool spec,char *err_str)
{
	char				*vertex_data,*fragment_data;
	bool				ok;
	
		// create the shader code

	vertex_data=gl_core_model_shader_build_vert(nlight,fog,bump,spec);
	if (vertex_data==NULL) {
		strcpy(err_str,"Out of Memory");
		return(FALSE);
	}

	fragment_data=gl_core_model_shader_build_frag(nlight,fog,bump,spec);
	if (fragment_data==NULL) {
		free(vertex_data);
		strcpy(err_str,"Out of Memory");
		return(FALSE);
	}
	
		// create the name
		
	sprintf(shader->name,"core_model_light:%d",nlight);
	if (fog) strcat(shader->name,"_fog");
	if (bump) strcat(shader->name,"_bump");
	if (spec) strcat(shader->name,"_spec");
	
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

bool gl_core_shader_initialize_per_light(int nlight,char *err_str)
{
		// gl_core_map_shader_light

	if (!gl_core_map_shader_create(&core_shaders[nlight][gl_core_map_shader_light],nlight,FALSE,FALSE,FALSE,FALSE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_map_shader_light_bump

	if (!gl_core_map_shader_create(&core_shaders[nlight][gl_core_map_shader_light_bump],nlight,FALSE,FALSE,TRUE,FALSE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_map_shader_light_bump_spec

	if (!gl_core_map_shader_create(&core_shaders[nlight][gl_core_map_shader_light_bump_spec],nlight,FALSE,FALSE,TRUE,TRUE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}
	
		// gl_core_map_shader_light_map

	if (!gl_core_map_shader_create(&core_shaders[nlight][gl_core_map_shader_light_map],nlight,FALSE,TRUE,FALSE,FALSE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}
	
		// gl_core_map_shader_light_map_bump

	if (!gl_core_map_shader_create(&core_shaders[nlight][gl_core_map_shader_light_map_bump],nlight,FALSE,TRUE,TRUE,FALSE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_map_shader_light_map_bump_spec

	if (!gl_core_map_shader_create(&core_shaders[nlight][gl_core_map_shader_light_map_bump_spec],nlight,FALSE,TRUE,TRUE,TRUE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_model_shader_light

	if (!gl_core_model_shader_create(&core_shaders[nlight][gl_core_model_shader_light],nlight,FALSE,FALSE,FALSE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}
	
		// gl_core_model_shader_light_bump

	if (!gl_core_model_shader_create(&core_shaders[nlight][gl_core_model_shader_light_bump],nlight,FALSE,TRUE,FALSE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_model_shader_light_bump_spec

	if (!gl_core_model_shader_create(&core_shaders[nlight][gl_core_model_shader_light_bump_spec],nlight,FALSE,TRUE,TRUE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_map_shader_fog_light

	if (!gl_core_map_shader_create(&core_shaders[nlight][gl_core_map_shader_fog_light],nlight,TRUE,FALSE,FALSE,FALSE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}
	
		// gl_core_map_shader_fog_light_bump

	if (!gl_core_map_shader_create(&core_shaders[nlight][gl_core_map_shader_fog_light_bump],nlight,TRUE,FALSE,TRUE,FALSE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_map_shader_fog_light_bump_spec

	if (!gl_core_map_shader_create(&core_shaders[nlight][gl_core_map_shader_fog_light_bump_spec],nlight,TRUE,FALSE,TRUE,TRUE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_map_shader_fog_light_map

	if (!gl_core_map_shader_create(&core_shaders[nlight][gl_core_map_shader_fog_light_map],nlight,TRUE,TRUE,FALSE,FALSE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}
	
		// gl_core_map_shader_fog_light_map_bump

	if (!gl_core_map_shader_create(&core_shaders[nlight][gl_core_map_shader_fog_light_map_bump],nlight,TRUE,TRUE,TRUE,FALSE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_map_shader_fog_light_map_bump_spec

	if (!gl_core_map_shader_create(&core_shaders[nlight][gl_core_map_shader_fog_light_map_bump_spec],nlight,TRUE,TRUE,TRUE,TRUE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_model_shader_fog_light

	if (!gl_core_model_shader_create(&core_shaders[nlight][gl_core_model_shader_fog_light],nlight,TRUE,FALSE,FALSE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_model_shader_fog_light_bump

	if (!gl_core_model_shader_create(&core_shaders[nlight][gl_core_model_shader_fog_light_bump],nlight,TRUE,TRUE,FALSE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_model_shader_fog_light_bump_spec

	if (!gl_core_model_shader_create(&core_shaders[nlight][gl_core_model_shader_fog_light_bump_spec],nlight,TRUE,TRUE,TRUE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}
	
	return(TRUE);
}

bool gl_core_shader_initialize(char *err_str)
{
	int					n,k;

	if (!gl_check_shader_ok()) return(TRUE);
	
		// note: we use max_shader_light+1 as
		// the first spot is 0 lights, and then 1
		// through max_shader_light

		// clear core shaders

	for (k=0;k!=(max_shader_light+1);k++) {
		for (n=0;n!=max_core_shader;n++) {
			gl_shader_code_clear(&core_shaders[k][n]);
		}
	}

		// initialize light varients of shaders		
		
	for (k=0;k!=(max_shader_light+1);k++) {
		if (!gl_core_shader_initialize_per_light(k,err_str)) return(FALSE);
	}	
	
	return(TRUE);
}

void gl_core_shader_shutdown(void)
{
	int					n,k;

	if (!gl_check_shader_ok()) return;

		// shutdown shaders

	for (k=0;k!=(max_shader_light+1);k++) {
		for (n=0;n!=max_core_shader;n++) {
			gl_shader_code_shutdown(&core_shaders[k][n]);
		}
	}
}

/* =======================================================

      Per-Scene Core Shader Initialization
      
======================================================= */

void gl_core_shader_draw_scene_initialize(void)
{
	int					n,k;
	
	for (k=0;k!=(max_shader_light+1);k++) {
		for (n=0;n!=max_core_shader;n++) {
			gl_shader_draw_scene_initialize_code(&core_shaders[k][n]);
		}
	}
}

/* =======================================================

      Core Shader Utilities
      
======================================================= */

inline int gl_core_shader_find_for_mode(bool map_shader,texture_type *texture,bool light_map)
{
	bool				bump,spec;
	
		// are bump and spec on?
		
	bump=(texture->frames[0].bumpmap.gl_id!=-1);
	spec=(texture->frames[0].specularmap.gl_id!=-1);
	
		// need spec for bump
		
	if ((spec) && (!bump)) bump=spec=FALSE;
	
		// non-fog shaders
		
	if (!fog_solid_on()) {
		if (!map_shader) {
			if ((bump) && (spec)) return(gl_core_model_shader_light_bump_spec);
			if ((bump) && (!spec)) return(gl_core_model_shader_light_bump);
			return(gl_core_model_shader_light);
		}

		if (light_map) {
			if ((bump) && (spec)) return(gl_core_map_shader_light_map_bump_spec);
			if ((bump) && (!spec)) return(gl_core_map_shader_light_map_bump);
			return(gl_core_map_shader_light_map);
		}

		if ((bump) && (spec)) return(gl_core_map_shader_light_bump_spec);
		if ((bump) && (!spec)) return(gl_core_map_shader_light_bump);
		return(gl_core_map_shader_light);
	}
	
		// fog shaders
	
	if (!map_shader) {
		if ((bump) && (spec)) return(gl_core_model_shader_fog_light_bump_spec);
		if ((bump) && (!spec)) return(gl_core_model_shader_fog_light_bump);
		return(gl_core_model_shader_fog_light);
	}

	if (light_map) {
		if ((bump) && (spec)) return(gl_core_map_shader_fog_light_map_bump_spec);
		if ((bump) && (!spec)) return(gl_core_map_shader_fog_light_map_bump);
		return(gl_core_map_shader_fog_light_map);
	}

	if ((bump) && (spec)) return(gl_core_map_shader_fog_light_bump_spec);
	if ((bump) && (!spec)) return(gl_core_map_shader_fog_light_bump);
	return(gl_core_map_shader_fog_light);
}

shader_type* gl_core_shader_find_ptr(int nlight,bool map_shader,texture_type *texture,bool light_map)
{
	int				which_varient;
	
	which_varient=gl_core_shader_find_for_mode(map_shader,texture,light_map);
	return(&core_shaders[nlight][which_varient]);
}

