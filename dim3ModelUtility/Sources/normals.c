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

(c) 2000-2010 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3modelutility.h"
#endif

#define normal_near_vertex_center_dist		250

/* =======================================================

      Calculate Normals for Model
      
======================================================= */

int model_recalc_normals_find_material(model_type *model,int mesh_idx,int trig_idx)
{
	int						n;
	model_material_type		*material;
    texture_type			*texture;

	texture=model->textures;
	material=model->meshes[mesh_idx].materials;

	for (n=0;n!=max_model_texture;n++) {
		if (texture->frames[0].name[0]!=0x0) {
			if ((trig_idx>=material->trig_start) && (trig_idx<(material->trig_start+material->trig_count))) return(n);
		}
		texture++;
		material++;
	}

	return(0);
}

bool model_recalc_normals_compare_sign(float f1,float f2)
{
	if ((f1<0.0f) && (f2<0.0f)) return(TRUE);
	if ((f1>=0.0f) && (f2>=0.0f)) return(TRUE);
	return(FALSE);
}

void map_recalc_normals_get_trig_box(model_type *model,int mesh_idx,int trig_idx,d3pnt *min,d3pnt *max)
{
	int					k,j,trig_material_idx;
	bool				first_hit;
	d3pnt				*pnt;
	model_mesh_type		*mesh;
	model_trig_type		*trig,*chk_trig;

	mesh=&model->meshes[mesh_idx];
	trig=&mesh->trigs[trig_idx];
	
	first_hit=FALSE;
	min->x=min->y=min->z=0;
	max->x=max->y=max->z=0;

	chk_trig=mesh->trigs;

	trig_material_idx=model_recalc_normals_find_material(model,mesh_idx,trig_idx);

	for (k=0;k!=mesh->ntrig;k++) {

		chk_trig=&mesh->trigs[k];
		if (model_recalc_normals_find_material(model,mesh_idx,k)!=trig_material_idx) continue;

		for (j=0;j!=3;j++) {

			pnt=&mesh->vertexes[chk_trig->v[j]].pnt;

			if (!first_hit) {
				min->x=max->x=pnt->x;
				min->y=max->y=pnt->y;
				min->z=max->z=pnt->z;
				first_hit=TRUE;
			}
			else {
				if (pnt->x<min->x) min->x=pnt->x;
				if (pnt->x>max->x) max->x=pnt->x;
				if (pnt->y<min->y) min->y=pnt->y;
				if (pnt->y>max->y) max->y=pnt->y;
				if (pnt->z<min->z) min->z=pnt->z;
				if (pnt->z>max->z) max->z=pnt->z;
			}
		}
	}
}

bool model_recalc_normals_determine_vector_in_out(model_type *model,int mesh_idx,int trig_idx,int pt_idx)
{
	int					x,y,z,k,pos_dist,neg_dist;
	float				f_dist;
	bool				is_out;
	d3pnt				*pnt,min,max,center,pos_pt,neg_pt;
	d3vct				face_vct;
	model_mesh_type		*mesh;
	model_trig_type		*trig;

		// get box for trig.  This will be the combination
		// of vertexes with the same material

	map_recalc_normals_get_trig_box(model,mesh_idx,trig_idx,&min,&max);

		// get the box center

	mesh=&model->meshes[mesh_idx];
	trig=&mesh->trigs[trig_idx];
	pnt=&mesh->vertexes[trig->v[pt_idx]].pnt;

	center.x=(min.x+max.x)>>1;
	center.y=(min.y+max.y)>>1;
	center.z=(min.z+max.z)>>1;

		// the dot product is the fall back position
		// if these specialized checks fail

	vector_create(&face_vct,pnt->x,pnt->y,pnt->z,center.x,center.y,center.z);
	is_out=(vector_dot_product(&trig->tangent_space[pt_idx].normal,&face_vct)>0.0f);

		// get a point from the current normal vector
		// and inverse of the current normal vector, using 10%
		// of the distance to center
	
	f_dist=(float)distance_get(pnt->x,pnt->y,pnt->z,center.x,center.y,center.z);
	f_dist*=0.1f;

	pos_pt.x=pnt->x+(int)(trig->tangent_space[pt_idx].normal.x*f_dist);
	pos_pt.y=pnt->y+(int)(trig->tangent_space[pt_idx].normal.y*f_dist);
	pos_pt.z=pnt->z+(int)(trig->tangent_space[pt_idx].normal.z*f_dist);

	neg_pt.x=pnt->x-(int)(trig->tangent_space[pt_idx].normal.x*f_dist);
	neg_pt.y=pnt->y-(int)(trig->tangent_space[pt_idx].normal.y*f_dist);
	neg_pt.z=pnt->z-(int)(trig->tangent_space[pt_idx].normal.z*f_dist);

		// first we determine if we can think of the
		// poly's box (which is determined by all connected
		// polys) as a closed object in one direction

		// if one direction is at least 25% greater than the others
		// then consider it a tube like structure

		// if any distance calcs fail, fall back to dot product

	x=max.x-min.x;
	y=max.y-min.y;
	z=max.z-min.z;

	k=x-((x*25)/100);
	if ((x>y) && (x>z)) {
		pos_dist=distance_2D_get(pos_pt.y,pos_pt.z,center.y,center.z);
		neg_dist=distance_2D_get(neg_pt.y,neg_pt.z,center.y,center.z);
		if (pos_dist==neg_dist) return(is_out);

		return(pos_dist>neg_dist);
	}

	k=y-((y*25)/100);
	if ((y>x) && (y>z)) {
		pos_dist=distance_2D_get(pos_pt.x,pos_pt.z,center.x,center.z);
		neg_dist=distance_2D_get(neg_pt.x,neg_pt.z,center.x,center.z);
		if (pos_dist==neg_dist) return(is_out);

		return(pos_dist>neg_dist);
	}

	k=z-((z*25)/100);
	if ((z>x) && (z>y)) {
		pos_dist=distance_2D_get(pos_pt.x,pos_pt.y,center.x,center.y);
		neg_dist=distance_2D_get(neg_pt.x,neg_pt.y,center.x,center.y);
		if (pos_dist==neg_dist) return(is_out);

		return(pos_dist>neg_dist);
	}

		// finally fall back to dot product

	return(is_out);
}

void model_recalc_normals_mesh(model_type *model,int mesh_idx,bool only_tangent)
{
	int					n,k,t,j,cnt;
    float				u10,u20,v10,v20,f_denom,f;
	d3vct				p10,p20,vlft,vrgt,v_num;
	d3vct				*normals,*nptr,*tangents,*tptr;
	d3pnt				*pt,*pt_1,*pt_2,v_center;
	model_mesh_type		*mesh;
	model_trig_type		*trig,*chk_trig;
	tangent_space_type	avg_space;
	
	mesh=&model->meshes[mesh_idx];
	if ((mesh->nvertex==0) || (mesh->ntrig==0)) return;

		// memory for tangent space

	normals=(d3vct*)malloc(mesh->ntrig*sizeof(d3vct));
	if (normals==NULL) return;

	tangents=(d3vct*)malloc(mesh->ntrig*sizeof(d3vct));
	if (tangents==NULL) {
		free(normals);
		return;
	}
	
        // find tangent and binormal for triangles
		
	trig=mesh->trigs;

	nptr=normals;
	tptr=tangents;
	
	for (n=0;n!=mesh->ntrig;n++) {
    
			// get the side vectors (p1-p0) and (p2-p0)

		pt=&mesh->vertexes[trig->v[0]].pnt;
		pt_1=&mesh->vertexes[trig->v[1]].pnt;
		pt_2=&mesh->vertexes[trig->v[2]].pnt;

		vector_create(&p10,pt_1->x,pt_1->y,pt_1->z,pt->x,pt->y,pt->z);
		vector_create(&p20,pt_2->x,pt_2->y,pt_2->z,pt->x,pt->y,pt->z);
		
			// calculate the normal by the cross

		vector_cross_product(nptr,&p10,&p20);
		nptr++;

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
		if (f_denom!=0.0f) f_denom=1.0f/f_denom;
		vector_scalar_multiply(tptr,&v_num,f_denom);

		vector_normalize(tptr);
		tptr++;
			
		trig++;
	}
    
		// average tangent space for each
		// trig vertex that are within 45 degrees
		// of each other.  otherwise, treat as a hard
		// edge
		
	trig=mesh->trigs;

	for (n=0;n!=mesh->ntrig;n++) {

			// average each vertex of the trig

		for (t=0;t!=3;t++) {
	
			cnt=1;

			memmove(&avg_space.normal,(normals+n),sizeof(d3vct));
			memmove(&avg_space.tangent,(tangents+n),sizeof(d3vct));
			
			for (k=0;k!=mesh->ntrig;k++) {

					// same trig

				if (n==k) continue;

					// get trig

				chk_trig=&mesh->trigs[k];

				nptr=normals+k;
				tptr=tangents+k;

					// check for shared vertexes
					// and normals within 45 degrees

				for (j=0;j!=3;j++) {

					if (trig->v[t]==chk_trig->v[j]) {

						if (vector_dot_product(&avg_space.normal,nptr)<0.7f) continue;

						avg_space.normal.x+=nptr->x;
						avg_space.normal.y+=nptr->y;
						avg_space.normal.z+=nptr->z;

						avg_space.tangent.x+=tptr->x;
						avg_space.tangent.y+=tptr->y;
						avg_space.tangent.z+=tptr->z;

						cnt++;
						break;
					}
				}
			}
			
				// create average vector

			if (cnt>1) {
				f=(float)cnt;

				avg_space.normal.x/=f;
				avg_space.normal.y/=f;
				avg_space.normal.z/=f;
				vector_normalize(&avg_space.normal);

				avg_space.tangent.x/=f;
				avg_space.tangent.y/=f;
				avg_space.tangent.z/=f;
				vector_normalize(&avg_space.tangent);
			}

			if (!only_tangent) memmove(&trig->tangent_space[t].normal,&avg_space.normal,sizeof(d3vct));
			memmove(&trig->tangent_space[t].tangent,&avg_space.tangent,sizeof(d3vct));
		}

		trig++;
	}

		// free the tangent spaces

	free(normals);
	free(tangents);
	
		// fix any normals that are 0,0,0
		// this usually happens when there are bad
		// or no UVs
		
	cnt=0;
	v_center.x=v_center.y=v_center.z=0;
	
	if (mesh->nvertex!=0) {

		for (k=0;k!=mesh->nvertex;k++) {
			pt=&mesh->vertexes[k].pnt;
			v_center.x+=pt->x;
			v_center.y+=pt->y;
			v_center.z+=pt->z;
		}
		
		v_center.x/=mesh->nvertex;
		v_center.y/=mesh->nvertex;
		v_center.z/=mesh->nvertex;
	}
		
	trig=mesh->trigs;
	
	for (n=0;n!=mesh->ntrig;n++) {
		for (t=0;t!=3;t++) {
			if ((trig->tangent_space[t].normal.x==0.0f) && (trig->tangent_space[t].normal.y==0.0f) && (trig->tangent_space[t].normal.z==0.0f)) {
				pt=&mesh->vertexes[trig->v[t]].pnt;
				trig->tangent_space[t].normal.x=(float)(pt->x-v_center.x);
				trig->tangent_space[t].normal.y=(float)(pt->y-v_center.y);
				trig->tangent_space[t].normal.z=(float)(pt->z-v_center.z);
				vector_normalize(&trig->tangent_space[t].normal);
			}
		}
		trig++;
	}

		// determine in-out to map flips
		// skip if not calculating normals
		
	if (only_tangent) return;

		// determine in/out and invert
		
	trig=mesh->trigs;

	for (n=0;n!=mesh->ntrig;n++) {
	
		for (k=0;k!=3;k++) {
			if (!model_recalc_normals_determine_vector_in_out(model,mesh_idx,n,k)) {
				trig->tangent_space[k].normal.x=-trig->tangent_space[k].normal.x;
				trig->tangent_space[k].normal.y=-trig->tangent_space[k].normal.y;
				trig->tangent_space[k].normal.z=-trig->tangent_space[k].normal.z;
			}
		}
		
		trig++;
	}
}

void model_recalc_normals(model_type *model,bool only_tangent)
{
	int				n;

	for (n=0;n!=model->nmesh;n++) {
		model_recalc_normals_mesh(model,n,only_tangent);
	}
}


