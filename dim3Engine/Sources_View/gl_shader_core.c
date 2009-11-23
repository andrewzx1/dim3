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

char* gl_core_shader_build_vert(int shader_type,int nlight)
{
	int				n;
	char			*buf;

		// memory for shader

	buf=(char*)malloc(max_core_shader_data_sz);
	if (buf==NULL) return(NULL);

	bzero(buf,max_core_shader_data_sz);

		// built vert shader

	strcat(buf,"uniform vec3 dim3CameraPosition;\n");
	strcat(buf,"uniform vec3 dim3LightPosition[4];\n");
	strcat(buf,"varying vec3 viewVector,lightHalfVector;\n");
	sprintf(strchr(buf,0),"varying vec3 lightVector[%d];\n",nlight);

	strcat(buf,"void main(void)\n");
	strcat(buf,"{\n");
	strcat(buf,"viewVector=gl_Vertex.xyz-dim3CameraPosition;\n");
	strcat(buf,"lightHalfVector=(dim3LightPosition[0]-dim3CameraPosition)+viewVector;\n");

	for (n=0;n!=nlight;n++) {
		sprintf(strchr(buf,0),"lightVector[%d]=dim3LightPosition[%d]-gl_Vertex.xyz;\n",n,n);
	}

	strcat(buf,"gl_Position=ftransform();\n");
	strcat(buf,"gl_TexCoord[0]=gl_MultiTexCoord0;\n");
	strcat(buf,"gl_TexCoord[1]=gl_MultiTexCoord1;\n");
	strcat(buf,"}\n");

	return(buf);
}

char* gl_core_shader_build_frag(int shader_type,int nlight)
{
	int				n;
	char			*buf;

		// memory for shader

	buf=(char*)malloc(max_core_shader_data_sz);
	if (buf==NULL) return(NULL);

	bzero(buf,max_core_shader_data_sz);

		// build frag shader
	
	strcat(buf,"uniform sampler2D dim3Tex;\n");
	strcat(buf,"uniform sampler2D dim3TexBump;\n");
	strcat(buf,"uniform sampler2D dim3TexSpecular;\n");
	strcat(buf,"uniform sampler2D dim3TexLightMap;\n");
	strcat(buf,"uniform float dim3Alpha;\n");
	strcat(buf,"uniform float dim3DarkFactor;\n");
	strcat(buf,"uniform float dim3BumpFactor;\n");
	strcat(buf,"uniform float dim3SpecularFactor;\n");
	strcat(buf,"uniform vec3 dim3AmbientColor;\n");

	sprintf(strchr(buf,0),"uniform vec3 dim3LightColor[%d];\n",nlight);
	sprintf(strchr(buf,0),"uniform float dim3LightIntensity[%d];\n",nlight);
	sprintf(strchr(buf,0),"uniform float dim3LightExponent[%d];\n",nlight);
	sprintf(strchr(buf,0),"uniform vec3 dim3LightDirection[%d];\n",nlight);

	strcat(buf,"varying vec3 viewVector,lightHalfVector;\n");
	strcat(buf,"varying vec3 lightVector[4];\n");

	strcat(buf,"void main(void)\n");
	strcat(buf,"{\n");
	strcat(buf,"float att,dist,pixelAtt,bump,spec,specMap;\n");
	strcat(buf,"vec3 ambient=dim3AmbientColor;\n");
	strcat(buf,"vec3 bumpVector=vec3(0.5,0.5,0.5);\n");
	strcat(buf,"vec3 bumpMap,specVector,specLightHalfVector;\n");
	strcat(buf,"vec4 tex;\n");

		// the light map

	strcat(buf,"ambient+=texture2D(dim3TexLightMap,gl_TexCoord[1].st).rgb;\n");

		// the texture lighting
		
	for (n=0;n!=nlight;n++) {
		sprintf(strchr(buf,0),"dist=length(lightVector[%d]);\n",n);
		sprintf(strchr(buf,0),"if (dist<dim3LightIntensity[%d]) {\n",n);
		sprintf(strchr(buf,0)," if (dot(lightVector[%d],dim3LightDirection[%d])>=0.0) {\n",n,n);
		sprintf(strchr(buf,0),"  att=1.0-(dist/dim3LightIntensity[%d]);\n",n);
		sprintf(strchr(buf,0),"  att+=pow(att,dim3LightExponent[%d]);\n",n);
		sprintf(strchr(buf,0),"  ambient+=(dim3LightColor[%d]*att);\n",n);
		sprintf(strchr(buf,0),"  bumpVector+=((bumpVector+lightVector[%d])*0.5);\n",n);
		strcat(buf," }\n");
		strcat(buf,"}\n");
	}

	strcat(buf,"pixelAtt=min((ambient.r+ambient.g+ambient.b),1.0);\n");
	
		// texture
		
	strcat(buf,"tex=texture2D(dim3Tex,gl_TexCoord[0].st);\n");
	
		// bump
		
	strcat(buf,"bumpVector=max(abs(normalize(bumpVector)),0.5);\n");
	strcat(buf,"bumpVector=normalize((bumpVector*pixelAtt)+(vec3(0.5,0.5,0.5)*(1.0-pixelAtt)));\n");
	strcat(buf,"bumpMap=texture2D(dim3TexBump,gl_TexCoord[0].st).rgb;\n");
	strcat(buf,"bumpMap=(bumpMap-0.5)*2.0;\n");
	strcat(buf,"bump=max(dot(bumpMap,bumpVector),0.0)*dim3BumpFactor;\n");
		
		// specular
		
	strcat(buf,"specVector=normalize(viewVector);\n");
	strcat(buf,"specLightHalfVector=normalize(lightHalfVector);\n");
	strcat(buf,"specMap=texture2D(dim3TexSpecular,gl_TexCoord[0].st).r;\n");
	strcat(buf,"spec=(specMap*pow(max(dot(specLightHalfVector,specVector),0.0),10.0))*dim3SpecularFactor*pixelAtt;\n");
	
		// output the fragment

	strcat(buf,"gl_FragColor.rgb=(((tex.rgb*ambient)*bump)+spec)*dim3DarkFactor;\n");
	strcat(buf,"gl_FragColor.a=tex.a*dim3Alpha;\n");
	strcat(buf,"}\n");

	return(buf);
}

/* =======================================================

      Core Shader Initialize/Shutdown
      
======================================================= */

bool gl_core_shader_initialize(void)
{
	int					n;
	shader_type			*shader;

		// clear core shaders

	shader=core_shaders;

	for (n=0;n!=max_core_shader;n++) {
		gl_shader_code_clear(shader);
		shader++;
	}


//	buf=gl_core_shader_build_vert(gl_core_shader_light,4);
//	buf=gl_core_shader_build_frag(gl_core_shader_light,4);

	return(TRUE);
}

void gl_core_shader_shutdown(void)
{
}


/*

viewVector,lightHalfVector = spec only

*/

/* VERT

uniform vec3 dim3CameraPosition;
uniform vec3 dim3LightPosition[4];

varying vec3 viewVector,lightHalfVector;
varying vec3 lightVector[4];

void main(void)
{
	viewVector=gl_Vertex.xyz-dim3CameraPosition;
	lightHalfVector=(dim3LightPosition[0]-dim3CameraPosition)+viewVector;

	lightVector[0]=dim3LightPosition[0]-gl_Vertex.xyz;
	lightVector[1]=dim3LightPosition[1]-gl_Vertex.xyz;
	lightVector[2]=dim3LightPosition[2]-gl_Vertex.xyz;
	lightVector[3]=dim3LightPosition[3]-gl_Vertex.xyz;

	gl_Position=ftransform();
	gl_TexCoord[0]=gl_MultiTexCoord0;
	gl_TexCoord[1]=gl_MultiTexCoord1;
}

*/

/* FRAG

uniform sampler2D dim3Tex;
uniform sampler2D dim3TexBump;
uniform sampler2D dim3TexSpecular;
uniform sampler2D dim3TexExtra;
uniform float dim3Alpha;
uniform float dim3DarkFactor;
uniform float dim3BumpFactor;
uniform float dim3SpecularFactor;

uniform vec3 dim3LightColor[4];
uniform float dim3LightIntensity[4];
uniform float dim3LightExponent[4];
uniform vec3 dim3LightDirection[4];

varying vec3 viewVector,lightHalfVector;
varying vec3 lightVector[4];

void main(void)
{
	float att,dist,bump,spec,specMap;
	float specAtt=0.0;
	vec3 ambient=vec3(0.0,0.0,0.0);
	vec3 bumpVector=vec3(0.5,0.5,0.5);
	vec3 colMap,bumpMap,shadowMap,specVector,specLightHalfVector;
	vec4 tex;

		// the texture lighting
		
	dist=length(lightVector[0]);
	if (dist<dim3LightIntensity[0]) {
		if (dot(lightVector[0],dim3LightDirection[0])>=0.0) {
			att=1.0-(dist/dim3LightIntensity[0]);
			att+=pow(att,dim3LightExponent[0]);
			specAtt=att;
			ambient=dim3LightColor[0]*att;
			bumpVector=lightVector[0];
		}
	}
	
	dist=length(lightVector[1]);
	if (dist<dim3LightIntensity[1]) {
		if (dot(lightVector[1],dim3LightDirection[1])>=0.0) {
			att=1.0-(dist/dim3LightIntensity[1]);
			att+=pow(att,dim3LightExponent[1]);
			specAtt+=att;
			ambient+=dim3LightColor[1]*att;
			bumpVector=(bumpVector+lightVector[1])*0.5;
		}
	}
	
	dist=length(lightVector[2]);
	if (dist<dim3LightIntensity[2]) {
		if (dot(lightVector[2],dim3LightDirection[2])>=0.0) {
			att=1.0-(dist/dim3LightIntensity[2]);
			att+=pow(att,dim3LightExponent[2]);
			specAtt+=att;
			ambient+=dim3LightColor[2]*att;
			bumpVector=(bumpVector+lightVector[2])*0.5;
		}
	}
	
	dist=length(lightVector[3]);
	if (dist<dim3LightIntensity[3]) {
		if (dot(lightVector[3],dim3LightDirection[3])>=0.0) {
			att=1.0-(dist/dim3LightIntensity[3]);
			att+=pow(att,dim3LightExponent[3]);
			specAtt+=att;
			ambient+=dim3LightColor[3]*att;
			bumpVector=(bumpVector+lightVector[3])*0.5;
		}
	}
	
	specAtt=min(specAtt,1.0);
	
		// texture
		
	tex=texture2D(dim3Tex,gl_TexCoord[0].st);
	colMap=tex.rgb*ambient;
	
		// bump
		
	bumpVector=max(abs(normalize(bumpVector)),0.5);
	bumpVector=normalize((bumpVector*specAtt)+(vec3(0.5,0.5,0.5)*(1.0-specAtt)));
		
	bumpMap=texture2D(dim3TexBump,gl_TexCoord[0].st).rgb;
	bumpMap=(bumpMap-0.5)*2.0;
	bump=max(dot(bumpMap,bumpVector),0.0)*dim3BumpFactor;
		
		// specular
		
	specVector=normalize(viewVector);
	specLightHalfVector=normalize(lightHalfVector);
	
	specMap=texture2D(dim3TexSpecular,gl_TexCoord[0].st).r;
	spec=(specMap*pow(max(dot(specLightHalfVector,specVector),0.0),10.0))*dim3SpecularFactor*specAtt;
		
		// the multiplied shadow texture
		
	shadowMap=texture2D(dim3TexExtra,gl_TexCoord[1].st).rgb;
	
		// output the fragment

	gl_FragColor.rgb=(((colMap*shadowMap)*bump)+spec)*dim3DarkFactor;
	gl_FragColor.a=tex.a*dim3Alpha;
}

*/
