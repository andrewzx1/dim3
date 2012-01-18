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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "interface.h"

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
	strcat(buf," float invertIntensity;\n");
	strcat(buf," float exponent;\n");
	strcat(buf," vec3 direction;\n");
	strcat(buf," float lightMapMask;\n");
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

char* gl_core_map_shader_build_vert(int nlight,bool fog,bool bump,bool spec)
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
	if ((bump) || (spec)) strcat(buf,",dim3VertexTangent");
	strcat(buf,";\n");
	
	strcat(buf,"varying vec3 dirNormal;\n");
	if (fog) strcat(buf,"varying float fogFactor;\n");

	for (n=0;n!=nlight;n++) {
		sprintf(strchr(buf,0),"varying vec3 lightVector_%d",n);
		if (bump) sprintf(strchr(buf,0),",lightVertexVector_%d",n);
		strcat(buf,";\n");
	}

	if (spec) strcat(buf,"varying vec3 eyeVector;\n");
	
	strcat(buf,"void main(void)\n");
	strcat(buf,"{\n");

	strcat(buf,"gl_Position=ftransform();\n");
	strcat(buf,"gl_TexCoord[0]=gl_MultiTexCoord0;\n");
	strcat(buf,"gl_TexCoord[1]=gl_MultiTexCoord1;\n");
	
	strcat(buf,"dirNormal=normalize(dim3VertexNormal);\n");

	strcat(buf,"vec3 vtx=vec3(gl_ModelViewMatrix*gl_Vertex);\n");
	
	if ((bump) || (spec)) {
		strcat(buf,"vec3 tangentSpaceTangent=normalize(gl_NormalMatrix*dim3VertexTangent);\n");
		strcat(buf,"vec3 tangentSpaceBinormal=normalize(gl_NormalMatrix*cross(dim3VertexNormal,dim3VertexTangent));\n");
		strcat(buf,"vec3 tangentSpaceNormal=normalize(gl_NormalMatrix*dim3VertexNormal);\n");
	}
	
	if (spec) {
		strcat(buf,"eyeVector.x=dot(-vtx,tangentSpaceTangent);\n");
		strcat(buf,"eyeVector.y=dot(-vtx,tangentSpaceBinormal);\n");
		strcat(buf,"eyeVector.z=dot(-vtx,tangentSpaceNormal);\n");
	}
	
	for (n=0;n!=nlight;n++) {
		sprintf(strchr(buf,0),"lightVector_%d=dim3Light_%d.position-vtx;\n",n,n);
		
		if (bump) {
			sprintf(strchr(buf,0),"lightVertexVector_%d.x=dot(lightVector_%d,tangentSpaceTangent);\n",n,n);
			sprintf(strchr(buf,0),"lightVertexVector_%d.y=dot(lightVector_%d,tangentSpaceBinormal);\n",n,n);
			sprintf(strchr(buf,0),"lightVertexVector_%d.z=dot(lightVector_%d,tangentSpaceNormal);\n",n,n);
		}
	}
	
	if (fog) strcat(buf,"fogFactor=clamp(((gl_Fog.end-distance(gl_Vertex.xyz,dim3CameraPosition))*gl_Fog.scale),0.0,1.0);\n");

	strcat(buf,"}\n");

	return(buf);
}

char* gl_core_map_shader_build_frag(int nlight,bool fog,bool bump,bool spec)
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
	strcat(buf,",dim3TexLightMap;\n");

	strcat(buf,"uniform float dim3Alpha,dim3LightMapBoost");
	if (spec) strcat(buf,",dim3ShineFactor");
	strcat(buf,";\n");
	
	strcat(buf,"uniform vec3 dim3AmbientColor;\n");
	
	strcat(buf,"varying vec3 dirNormal;\n");
	if (fog) strcat(buf,"varying float fogFactor;\n");
	
	for (n=0;n!=nlight;n++) {
		sprintf(strchr(buf,0),"varying vec3 lightVector_%d",n);
		if (bump) sprintf(strchr(buf,0),",lightVertexVector_%d",n);
		strcat(buf,";\n");
	}

	if (spec) strcat(buf,"varying vec3 eyeVector;\n");
	
	strcat(buf,"void main(void)\n");
	strcat(buf,"{\n");
	
	strcat(buf,"float att,dist;\n");
	strcat(buf,"vec3 ambient=dim3AmbientColor;\n");

		// the light map

	strcat(buf,"vec3 lmap=texture2D(dim3TexLightMap,gl_TexCoord[1].st).rgb;\n");
	strcat(buf,"ambient+=(lmap+(lmap*dim3LightMapBoost));\n");
	
		// the texture map
		
	strcat(buf,"vec4 tex=texture2D(dim3Tex,gl_TexCoord[0].st);\n");
		
		// the bump map
		
	if (bump) {
		strcat(buf,"vec3 bumpMap=normalize((texture2D(dim3TexBump,gl_TexCoord[0].st).rgb*2.0)-1.0);\n");
		strcat(buf,"bumpMap.y=-bumpMap.y;\n");
		strcat(buf,"float bump=dot(vec3(0.0,0.0,0.5),bumpMap);\n");
	}
	
		// the spec map
		
	if (spec) {
		strcat(buf,"vec3 spec=vec3(0.0,0.0,0.0),specHalfVector;\n");
		strcat(buf,"vec3 specMap=texture2D(dim3TexSpecular,gl_TexCoord[0].st).rgb;\n");
		strcat(buf,"float specFactor;\n");
	}
	
		// the texture lighting
		
	for (n=0;n!=nlight;n++) {
		sprintf(strchr(buf,0),"dist=length(lightVector_%d);\n",n);
		sprintf(strchr(buf,0),"if (dist<dim3Light_%d.intensity) {\n",n);
		sprintf(strchr(buf,0)," if (dot(dirNormal,dim3Light_%d.direction)>=0.0) {\n",n);
		sprintf(strchr(buf,0),"  att=1.0-(dist*dim3Light_%d.invertIntensity);\n",n);
		sprintf(strchr(buf,0),"  att+=pow(att,dim3Light_%d.exponent);\n",n);
		sprintf(strchr(buf,0),"  ambient+=((dim3Light_%d.color*att)*dim3Light_%d.lightMapMask);\n",n,n);
		
			// per-light bump calc
			
		if (bump) sprintf(strchr(buf,0),"  bump+=(dot(normalize(lightVertexVector_%d),bumpMap)*att);\n",n);
		
			// per-light spec count
		
		if (spec) {
			sprintf(strchr(buf,0),"  specHalfVector=normalize(normalize(eyeVector)+normalize(lightVertexVector_%d));\n",n);
			strcat(buf,"  specFactor=max(dot(bumpMap,specHalfVector),0.0);\n");
			strcat(buf,"  spec+=((specMap*pow(specFactor,dim3ShineFactor))*att);\n");
		}
		
		strcat(buf," }\n");
		strcat(buf,"}\n");
	}

		// finish the spec by making sure
		// it's dimmed in dark areas
		
	if (spec) strcat(buf,"spec=min(spec,1.0)*((ambient.r+ambient.g+ambient.b)*0.33);\n");
	
		// make the total ambient out of
		// ambient * bump,
		// and make sure pixel ambient is never
		// less than map ambient
		
	strcat(buf,"ambient=clamp(");
	if (bump) strcat(buf,"(");
	strcat(buf,"ambient");
	if (bump) strcat(buf,"*bump)");
	strcat(buf,",(dim3AmbientColor*0.9),vec3(1.0));\n");

		// output the fragment
		// and add in the spec

	if (!fog) {
		strcat(buf,"gl_FragColor.rgb=");
	}
	else {
		strcat(buf,"vec3 frag=");
	}

	if (spec) strcat(buf,"(");
	strcat(buf,"tex.rgb*ambient");
	if (spec) strcat(buf,"+spec)");
	strcat(buf,";\n");

	if (fog) strcat(buf,"gl_FragColor.rgb=mix(gl_Fog.color.rgb,frag,fogFactor);\n");
	
	strcat(buf,"gl_FragColor.a=tex.a*dim3Alpha;\n");
	strcat(buf,"}\n");

	return(buf);
}

bool gl_core_map_shader_create(shader_type *shader,int nlight,bool fog,bool bump,bool spec,char *err_str)
{
	char				*vertex_data,*fragment_data;
	bool				ok;
	
		// create the shader code

	vertex_data=gl_core_map_shader_build_vert(nlight,fog,bump,spec);
	if (vertex_data==NULL) {
		strcpy(err_str,"Out of Memory");
		return(FALSE);
	}

	fragment_data=gl_core_map_shader_build_frag(nlight,fog,bump,spec);
	if (fragment_data==NULL) {
		free(vertex_data);
		strcpy(err_str,"Out of Memory");
		return(FALSE);
	}
	
		// create the name
		
	sprintf(shader->name,"core_map_light:%d",nlight);
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

      Build Single Core Liquid Shaders
      
======================================================= */

char* gl_core_liquid_shader_build_vert(int nlight)
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
	strcat(buf,"attribute vec3 dim3VertexNormal;\n");
	
	strcat(buf,"varying vec3 dirNormal;\n");

	for (n=0;n!=nlight;n++) {
		sprintf(strchr(buf,0),"varying vec3 lightVector_%d;\n",n);
	}
	
	strcat(buf,"void main(void)\n");
	strcat(buf,"{\n");

	strcat(buf,"gl_Position=ftransform();\n");
	strcat(buf,"gl_TexCoord[0]=gl_MultiTexCoord0;\n");
	strcat(buf,"gl_TexCoord[1]=gl_MultiTexCoord1;\n");
	
	strcat(buf,"dirNormal=normalize(dim3VertexNormal);\n");

	strcat(buf,"vec3 vtx=vec3(gl_ModelViewMatrix*gl_Vertex);\n");
	
	for (n=0;n!=nlight;n++) {
		sprintf(strchr(buf,0),"lightVector_%d=dim3Light_%d.position-vtx;\n",n,n);
	}

	strcat(buf,"}\n");

	return(buf);
}

char* gl_core_liquid_shader_build_frag(int nlight)
{
	int				n;
	char			*buf;

		// memory for shader

	buf=(char*)malloc(max_core_shader_data_sz);
	if (buf==NULL) return(NULL);

	bzero(buf,max_core_shader_data_sz);

		// build frag shader
		
	gl_core_shader_build_generic_light_struct(nlight,buf);
	
	strcat(buf,"uniform sampler2D dim3Tex,dim3TexLightMap;\n");
	strcat(buf,"uniform float dim3Alpha,dim3LightMapBoost;\n");
	strcat(buf,"uniform vec3 dim3AmbientColor;\n");
	
	strcat(buf,"varying vec3 dirNormal;\n");
	
	for (n=0;n!=nlight;n++) {
		sprintf(strchr(buf,0),"varying vec3 lightVector_%d;\n",n);
	}
	
	strcat(buf,"void main(void)\n");
	strcat(buf,"{\n");
	
	strcat(buf,"float att,dist;\n");
	strcat(buf,"vec3 ambient=dim3AmbientColor;\n");

		// the light map

	strcat(buf,"vec3 lmap=texture2D(dim3TexLightMap,gl_TexCoord[1].st).rgb;\n");
	strcat(buf,"ambient+=(lmap+(lmap*dim3LightMapBoost));\n");
	
		// the texture map
		
	strcat(buf,"vec4 tex=texture2D(dim3Tex,gl_TexCoord[0].st);\n");
	
		// the texture lighting
		
	for (n=0;n!=nlight;n++) {
		sprintf(strchr(buf,0),"dist=length(lightVector_%d);\n",n);
		sprintf(strchr(buf,0),"if (dist<dim3Light_%d.intensity) {\n",n);
		sprintf(strchr(buf,0)," if (dot(dirNormal,dim3Light_%d.direction)>=0.0) {\n",n);
		sprintf(strchr(buf,0),"  att=1.0-(dist*dim3Light_%d.invertIntensity);\n",n);
		sprintf(strchr(buf,0),"  att+=pow(att,dim3Light_%d.exponent);\n",n);
		sprintf(strchr(buf,0),"  ambient+=((dim3Light_%d.color*att)*dim3Light_%d.lightMapMask);\n",n,n);
		strcat(buf," }\n");
		strcat(buf,"}\n");
	}

		// output the fragment

	strcat(buf,"gl_FragColor.rgb=(tex.rgb*ambient);\n");
	strcat(buf,"gl_FragColor.a=tex.a*dim3Alpha;\n");
	strcat(buf,"}\n");

	return(buf);
}

bool gl_core_liquid_shader_create(shader_type *shader,int nlight,char *err_str)
{
	char				*vertex_data,*fragment_data;
	bool				ok;
	
		// create the shader code

	vertex_data=gl_core_liquid_shader_build_vert(nlight);
	if (vertex_data==NULL) {
		strcpy(err_str,"Out of Memory");
		return(FALSE);
	}

	fragment_data=gl_core_liquid_shader_build_frag(nlight);
	if (fragment_data==NULL) {
		free(vertex_data);
		strcpy(err_str,"Out of Memory");
		return(FALSE);
	}
	
		// create the name
		
	sprintf(shader->name,"core_liquid_light:%d",nlight);
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

      Build Single Core Model Shaders
      
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
	if ((bump) || (spec)) strcat(buf,",dim3VertexTangent");
	strcat(buf,";\n");
	
	strcat(buf,"varying vec3 dirNormal,tangentSpaceNormal;\n");
	if (fog) strcat(buf,"varying float fogFactor;\n");

	for (n=0;n!=nlight;n++) {
		sprintf(strchr(buf,0),"varying vec3 lightVector_%d",n);
		if (bump) sprintf(strchr(buf,0),",lightVertexVector_%d",n);
		strcat(buf,";\n");
	}

	if (spec) strcat(buf,"varying vec3 eyeVector;\n");
	
	strcat(buf,"void main(void)\n");
	strcat(buf,"{\n");

	strcat(buf,"gl_Position=ftransform();\n");
	strcat(buf,"gl_TexCoord[0]=gl_MultiTexCoord0;\n");

	strcat(buf,"dirNormal=normalize(dim3VertexNormal);\n");
	
	strcat(buf,"vec3 vtx=vec3(gl_ModelViewMatrix*gl_Vertex);\n");
	
	if ((bump) || (spec)) {
		strcat(buf,"vec3 tangentSpaceTangent=normalize(gl_NormalMatrix*dim3VertexTangent);\n");
		strcat(buf,"vec3 tangentSpaceBinormal=normalize(gl_NormalMatrix*cross(dim3VertexNormal,dim3VertexTangent));\n");
	}
	
	strcat(buf,"tangentSpaceNormal=normalize(gl_NormalMatrix*dim3VertexNormal);\n");	// always need normal for diffuse
	
	if (spec) {
		strcat(buf,"eyeVector.x=dot(-vtx,tangentSpaceTangent);\n");
		strcat(buf,"eyeVector.y=dot(-vtx,tangentSpaceBinormal);\n");
		strcat(buf,"eyeVector.z=dot(-vtx,tangentSpaceNormal);\n");
	}
	
	for (n=0;n!=nlight;n++) {
		sprintf(strchr(buf,0),"lightVector_%d=dim3Light_%d.position-vtx;\n",n,n);
		
		if (bump) {
			sprintf(strchr(buf,0),"lightVertexVector_%d.x=dot(lightVector_%d,tangentSpaceTangent);\n",n,n);
			sprintf(strchr(buf,0),"lightVertexVector_%d.y=dot(lightVector_%d,tangentSpaceBinormal);\n",n,n);
			sprintf(strchr(buf,0),"lightVertexVector_%d.z=dot(lightVector_%d,tangentSpaceNormal);\n",n,n);
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

	strcat(buf,"uniform float dim3Alpha,dim3DiffuseBoost");
	if (spec) strcat(buf,",dim3ShineFactor");
	strcat(buf,";\n");
	
	strcat(buf,"uniform vec3 dim3AmbientColor,dim3DiffuseVector;\n");
	
	strcat(buf,"varying vec3 dirNormal,tangentSpaceNormal;\n");
	if (fog) strcat(buf,"varying float fogFactor;\n");

	for (n=0;n!=nlight;n++) {
		sprintf(strchr(buf,0),"varying vec3 lightVector_%d",n);
		if (bump) sprintf(strchr(buf,0),",lightVertexVector_%d",n);
		strcat(buf,";\n");
	}

	if (spec) strcat(buf,"varying vec3 eyeVector;\n");
	
	strcat(buf,"void main(void)\n");
	strcat(buf,"{\n");
	
	strcat(buf,"float att,dist;\n");
	strcat(buf,"vec3 ambient=dim3AmbientColor;\n");
	
		// the texture map
		
	strcat(buf,"vec4 tex=texture2D(dim3Tex,gl_TexCoord[0].st);\n");
	
		// the bump map
		
	if (bump) {
		strcat(buf,"vec3 bumpMap=normalize((texture2D(dim3TexBump,gl_TexCoord[0].st).rgb*2.0)-1.0);\n");
		strcat(buf,"bumpMap.y=-bumpMap.y;\n");
		strcat(buf,"float bump=dot(vec3(0.0,0.0,0.5),bumpMap);\n");
	}
	
		// the spec map
		
	if (spec) {
		strcat(buf,"vec3 spec=vec3(0.0,0.0,0.0),specHalfVector;\n");
		strcat(buf,"vec3 specMap=texture2D(dim3TexSpecular,gl_TexCoord[0].st).rgb;\n");
		strcat(buf,"float specFactor;\n");
	}
			
		// diffuse
		
	strcat(buf,"float diffuse=(dot(dim3DiffuseVector,tangentSpaceNormal)+1.0)*0.5;\n");
	
		// the texture lighting
		
	for (n=0;n!=nlight;n++) {
		sprintf(strchr(buf,0),"dist=length(lightVector_%d);\n",n);
		sprintf(strchr(buf,0),"if (dist<dim3Light_%d.intensity) {\n",n);
		sprintf(strchr(buf,0)," att=1.0-(dist*dim3Light_%d.invertIntensity);\n",n);
		sprintf(strchr(buf,0)," att+=pow(att,dim3Light_%d.exponent);\n",n);
		sprintf(strchr(buf,0)," if (dot(dirNormal,dim3Light_%d.direction)>=0.0) {\n",n);
		sprintf(strchr(buf,0),"  ambient+=(dim3Light_%d.color*att);\n",n);
		
			// bump and spec
			
		if (bump) sprintf(strchr(buf,0),"  bump+=(dot(normalize(lightVertexVector_%d),bumpMap)*att);\n",n);
		if (spec) {
			sprintf(strchr(buf,0),"  specHalfVector=normalize(normalize(eyeVector)+normalize(lightVertexVector_%d));\n",n);
			strcat(buf,"  specFactor=max(dot(bumpMap,specHalfVector),0.0);\n");
			strcat(buf,"  spec+=((specMap*pow(specFactor,dim3ShineFactor))*att);\n");
		}
		
		strcat(buf," }\n");
		strcat(buf,"}\n");
	}
	
		// make the total ambient out of
		// ambient * diffuse * bump,
		// and make sure pixel ambient is never
		// less than map ambient

	strcat(buf,"diffuse=");
	if (bump) strcat(buf,"(");
	strcat(buf,"diffuse");
	if (bump) strcat(buf,"*bump)");
	strcat(buf,"+0.5;\n");

	strcat(buf,"ambient=ambient*diffuse;\n");


// supergumba
//	strcat(buf,"ambient=clamp((ambient*diffuse");
//	if (bump) strcat(buf,"*bump");
//	strcat(buf,"),(dim3AmbientColor*0.9),vec3(1.0));\n");

		// output the fragment
		// and add in the spec

	if (!fog) {
		strcat(buf,"gl_FragColor.rgb=");
	}
	else {
		strcat(buf,"vec3 frag=");
	}

	if (spec) strcat(buf,"(");
	strcat(buf,"tex.rgb*ambient");
	if (spec) strcat(buf,")+spec");
	strcat(buf,";\n");
	
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
		// gl_core_map_shader_light_map

	if (!gl_core_map_shader_create(&core_shaders[nlight][gl_core_map_shader_light_map],nlight,FALSE,FALSE,FALSE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}
	
		// gl_core_map_shader_light_map_bump

	if (!gl_core_map_shader_create(&core_shaders[nlight][gl_core_map_shader_light_map_bump],nlight,FALSE,TRUE,FALSE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_map_shader_light_map_bump_spec

	if (!gl_core_map_shader_create(&core_shaders[nlight][gl_core_map_shader_light_map_bump_spec],nlight,FALSE,TRUE,TRUE,err_str)) {
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

		// gl_core_map_shader_fog_light_map

	if (!gl_core_map_shader_create(&core_shaders[nlight][gl_core_map_shader_fog_light_map],nlight,TRUE,FALSE,FALSE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}
	
		// gl_core_map_shader_fog_light_map_bump

	if (!gl_core_map_shader_create(&core_shaders[nlight][gl_core_map_shader_fog_light_map_bump],nlight,TRUE,TRUE,FALSE,err_str)) {
		gl_core_shader_shutdown();
		return(FALSE);
	}

		// gl_core_map_shader_fog_light_map_bump_spec

	if (!gl_core_map_shader_create(&core_shaders[nlight][gl_core_map_shader_fog_light_map_bump_spec],nlight,TRUE,TRUE,TRUE,err_str)) {
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

		// gl_core_map_shader_liquid

	if (!gl_core_liquid_shader_create(&core_shaders[nlight][gl_core_map_shader_liquid],nlight,err_str)) {
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

      Core Shader Utilities
      
======================================================= */

static inline int gl_core_shader_find_for_mode(int core_shader_group,texture_type *texture)
{
	bool				bump,spec;

		// liquid shaders

	if (core_shader_group==core_shader_group_liquid) return(gl_core_map_shader_liquid);
	
		// are bump and spec on?
		
	bump=(texture->frames[0].bumpmap.gl_id!=-1);
	spec=(texture->frames[0].specularmap.gl_id!=-1);
	
		// need spec for bump
		
	if ((spec) && (!bump)) bump=spec=FALSE;
	
		// non-fog shaders
		
	if (!fog_solid_on()) {
		if (core_shader_group==core_shader_group_model) {
			if ((bump) && (spec)) return(gl_core_model_shader_light_bump_spec);
			if ((bump) && (!spec)) return(gl_core_model_shader_light_bump);
			return(gl_core_model_shader_light);
		}

		if ((bump) && (spec)) return(gl_core_map_shader_light_map_bump_spec);
		if ((bump) && (!spec)) return(gl_core_map_shader_light_map_bump);
		return(gl_core_map_shader_light_map);
	}
	
		// fog shaders
	
	if (core_shader_group==core_shader_group_model) {
		if ((bump) && (spec)) return(gl_core_model_shader_fog_light_bump_spec);
		if ((bump) && (!spec)) return(gl_core_model_shader_fog_light_bump);
		return(gl_core_model_shader_fog_light);
	}

	if ((bump) && (spec)) return(gl_core_map_shader_fog_light_map_bump_spec);
	if ((bump) && (!spec)) return(gl_core_map_shader_fog_light_map_bump);
	return(gl_core_map_shader_fog_light_map);
}

shader_type* gl_core_shader_find_ptr(int nlight,int core_shader_group,texture_type *texture)
{
	int				which_varient;
	
	which_varient=gl_core_shader_find_for_mode(core_shader_group,texture);
	return(&core_shaders[nlight][which_varient]);
}

