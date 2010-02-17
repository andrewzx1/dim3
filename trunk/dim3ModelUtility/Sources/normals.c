/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
 Usage: Normal Routines

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

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3modelutility.h"
#endif

/* =======================================================

      Calculate Normals for Model
      
======================================================= */

void model_recalc_normals_mesh(model_type *model,int mesh_idx,bool only_tangent_binormal)
{
	int					n,k,cnt;
    float				u10,u20,v10,v20,f_denom,f;
	d3vct				p10,p20,vlft,vrgt,v_num;
	d3pnt				*pt,*pt_1,*pt_2;
	model_mesh_type		*mesh;
    model_vertex_type	*vertex;
	model_trig_type		*trig;
	tangent_space_type	*space,*spc,avg_space;

	mesh=&model->meshes[mesh_idx];
	if ((mesh->nvertex==0) || (mesh->ntrig==0)) return;

		// memory for tangent space

	space=(tangent_space_type*)malloc(mesh->ntrig*sizeof(tangent_space_type));
	if (space==NULL) return;
	
        // find tangent and binormal for triangles
        
	trig=mesh->trigs;
	spc=space;
	
	for (n=0;n!=mesh->ntrig;n++) {
    
			// get the side vectors (p1-p0) and (p2-p0)

		pt=&mesh->vertexes[trig->v[0]].pnt;
		pt_1=&mesh->vertexes[trig->v[1]].pnt;
		pt_2=&mesh->vertexes[trig->v[2]].pnt;

		vector_create(&p10,pt_1->x,pt_1->y,pt_1->z,pt->x,pt->y,pt->z);
		vector_create(&p20,pt_2->x,pt_2->y,pt_2->z,pt->x,pt->y,pt->z);

			// get the UV scalars (u1-u0), (u2-u0), (v1-v0), (v2-v0)

		u10=trig->gx[1]-trig->gx[0];
		u20=trig->gx[2]-trig->gx[0];
		v10=trig->gy[1]-trig->gy[0];
		v20=trig->gy[2]-trig->gy[0];

			// calculate the tangent
			// (v20xp10)-(v10xp20) / (u10*v20)-(v10*u20)

		vector_scalar_multiply(&vlft,&p10,v20);
		vector_scalar_multiply(&vrgt,&p20,v10);
		vector_subtract(&v_num,&vlft,&vrgt);

		f_denom=(u10*v20)-(v10*u20);

		vector_scalar_multiply(&spc->tangent,&v_num,(1.0f/f_denom));
		vector_normalize(&spc->tangent);

			// calculate the binormal
			// (u20xp10)-(u10xp20) / (v10*u20)-(u10*v20)

		vector_scalar_multiply(&vlft,&p10,u20);
		vector_scalar_multiply(&vrgt,&p20,u10);
		vector_subtract(&v_num,&vlft,&vrgt);

		f_denom=(v10*u20)-(u10*v20);

		vector_scalar_multiply(&spc->binormal,&v_num,(1.0f/f_denom));
		vector_normalize(&spc->binormal);
		
			// normal is cross T and B
			
		vector_cross_product(&spc->normal,&spc->tangent,&spc->binormal);
			
		trig++;
		spc++;
	}
    
		// average tangent and binormal for vertexes
		// and get the normal from the cross product
		
	vertex=mesh->vertexes;

	for (n=0;n!=mesh->nvertex;n++) {
	
			// find all trigs that share this vertex
		
		cnt=0;

		avg_space.tangent.x=avg_space.tangent.y=avg_space.tangent.z=0.0f;
		avg_space.binormal.x=avg_space.binormal.y=avg_space.binormal.z=0.0f;
		avg_space.normal.x=avg_space.normal.y=avg_space.normal.z=0.0f;
		
		trig=mesh->trigs;
		spc=space;
		
		for (k=0;k!=mesh->ntrig;k++) {

			if ((trig->v[0]==n) || (trig->v[1]==n) || (trig->v[2]==n)) {
				avg_space.tangent.x+=spc->tangent.x;
				avg_space.tangent.y+=spc->tangent.y;
				avg_space.tangent.z+=spc->tangent.z;
				
				avg_space.binormal.x+=spc->binormal.x;
				avg_space.binormal.y+=spc->binormal.y;
				avg_space.binormal.z+=spc->binormal.z;
				
				avg_space.normal.x+=spc->normal.x;
				avg_space.normal.y+=spc->normal.y;
				avg_space.normal.z+=spc->normal.z;

                cnt++;
			}

			trig++;
			spc++;
		}
		
		if (cnt!=0) {
			f=(float)cnt;

			avg_space.tangent.x/=f;
			avg_space.tangent.y/=f;
			avg_space.tangent.z/=f;

			avg_space.binormal.x/=f;
			avg_space.binormal.y/=f;
			avg_space.binormal.z/=f;
			
			avg_space.normal.x/=f;
			avg_space.normal.y/=f;
			avg_space.normal.z/=f;
		}

		memmove(&vertex->tangent_space.tangent,&avg_space.tangent,sizeof(d3vct));
		memmove(&vertex->tangent_space.binormal,&avg_space.binormal,sizeof(d3vct));
		if (!only_tangent_binormal) memmove(&vertex->tangent_space.normal,&avg_space.normal,sizeof(d3vct));
		
		vertex++;
	}

		// free the tangent spaces

	free(space);
}

void model_recalc_normals(model_type *model,bool only_tangent_binormal)
{
	int				n;

	for (n=0;n!=model->nmesh;n++) {
		model_recalc_normals_mesh(model,n,only_tangent_binormal);
	}
}


