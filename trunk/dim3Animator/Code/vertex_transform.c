/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Model Vertex Transformations

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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3animator.h"
#endif

#include "glue.h"
#include "interface.h"
#include "dialog.h"

d3vct							cur_set_normal={0.0f,-1.0f,0.0f};

extern model_type				model;
extern animator_state_type		state;

/* =======================================================

      Vertex Center
      
======================================================= */

void vertex_find_center_sel_vertexes(int mesh_idx,int *p_cx,int *p_cy,int *p_cz)
{
	int					i,nt,cx,cz,cy,cnt;
	model_vertex_type	*vertex;
	
	nt=model.meshes[mesh_idx].nvertex;
	vertex=model.meshes[mesh_idx].vertexes;
	
	cx=cy=cz=0;
	cnt=0;
	
	for (i=0;i!=nt;i++) {
		if (vertex_check_sel_mask(mesh_idx,i)) {
			cx+=vertex->pnt.x;
			cy+=vertex->pnt.y;
			cz+=vertex->pnt.z;
			cnt++;
		}
		vertex++;
	}
	
	if (cnt==0) {
		*p_cx=*p_cy=*p_cz=0;
		return;
	}
	
	*p_cx=cx/cnt;
	*p_cy=cy/cnt;
	*p_cz=cz/cnt;
}

/* =======================================================

      Invert Normal Vertexes
      
======================================================= */

void vertex_invert_normals_vertexes(int mesh_idx)
{
	int					n,k,ntrig;
	model_trig_type		*trig;
	
	ntrig=model.meshes[mesh_idx].ntrig;
	trig=model.meshes[mesh_idx].trigs;
	
	for (n=0;n!=ntrig;n++) {
	
		for (k=0;k!=3;k++) {
		
				// this triangle vertex in the select list?
				
			if (!vertex_check_sel_mask(mesh_idx,trig->v[k])) continue;
			
			trig->tangent_space[k].tangent.x=-trig->tangent_space[k].tangent.x;
			trig->tangent_space[k].tangent.y=-trig->tangent_space[k].tangent.y;
			trig->tangent_space[k].tangent.z=-trig->tangent_space[k].tangent.z;

			trig->tangent_space[k].binormal.x=-trig->tangent_space[k].binormal.x;
			trig->tangent_space[k].binormal.y=-trig->tangent_space[k].binormal.y;
			trig->tangent_space[k].binormal.z=-trig->tangent_space[k].binormal.z;

			trig->tangent_space[k].normal.x=-trig->tangent_space[k].normal.x;
			trig->tangent_space[k].normal.y=-trig->tangent_space[k].normal.y;
			trig->tangent_space[k].normal.z=-trig->tangent_space[k].normal.z;
		}
		
		trig++;
	}
}

void vertex_invert_normals_trigs(int mesh_idx)
{
	int					n;
	model_trig_type		*trig;
	
		// is there a trig selection?
		
	if (state.sel_trig_idx==-1) return;
	
		// get trig
		
	trig=&model.meshes[mesh_idx].trigs[state.sel_trig_idx];
	
	for (n=0;n!=3;n++) {
	
			// this triangle vertex in the select list?
			
		if (!vertex_check_sel_mask(mesh_idx,trig->v[n])) continue;
		
		trig->tangent_space[n].tangent.x=-trig->tangent_space[n].tangent.x;
		trig->tangent_space[n].tangent.y=-trig->tangent_space[n].tangent.y;
		trig->tangent_space[n].tangent.z=-trig->tangent_space[n].tangent.z;

		trig->tangent_space[n].binormal.x=-trig->tangent_space[n].binormal.x;
		trig->tangent_space[n].binormal.y=-trig->tangent_space[n].binormal.y;
		trig->tangent_space[n].binormal.z=-trig->tangent_space[n].binormal.z;

		trig->tangent_space[n].normal.x=-trig->tangent_space[n].normal.x;
		trig->tangent_space[n].normal.y=-trig->tangent_space[n].normal.y;
		trig->tangent_space[n].normal.z=-trig->tangent_space[n].normal.z;
	}
}

void vertex_invert_normals(int mesh_idx)
{
	if (state.select_mode==select_mode_vertex) {
		vertex_invert_normals_vertexes(mesh_idx);
	}
	else {
		vertex_invert_normals_trigs(mesh_idx);
	}
}

void vertex_set_normals_vertexes(int mesh_idx,d3vct *normal)
{
	int					n,k,ntrig;
	model_trig_type		*trig;

	ntrig=model.meshes[mesh_idx].ntrig;
	trig=model.meshes[mesh_idx].trigs;
	
	for (n=0;n!=ntrig;n++) {
	
		for (k=0;k!=3;k++) {
		
				// this triangle vertex in the select list?
				
			if (!vertex_check_sel_mask(mesh_idx,trig->v[k])) continue;
			
			trig->tangent_space[k].normal.x=normal->x;
			trig->tangent_space[k].normal.y=normal->y;
			trig->tangent_space[k].normal.z=normal->z;
		}
		
		trig++;
	}
}

void vertex_set_normals_trigs(int mesh_idx,d3vct *normal)
{
	int					n;
	model_trig_type		*trig;
	
		// is there a trig selection?
		
	if (state.sel_trig_idx==-1) return;
	
		// get trig
		
	trig=&model.meshes[mesh_idx].trigs[state.sel_trig_idx];
	
	for (n=0;n!=3;n++) {
	
			// this triangle vertex in the select list?
			
		if (!vertex_check_sel_mask(mesh_idx,trig->v[n])) continue;
		
		trig->tangent_space[n].normal.x=normal->x;
		trig->tangent_space[n].normal.y=normal->y;
		trig->tangent_space[n].normal.z=normal->z;
	}
}

void vertex_set_normals(int mesh_idx)
{
	d3vct			normal;
	
	memmove(&normal,&cur_set_normal,sizeof(d3vct));
	
	if (!dialog_set_normal_run(&normal)) return;
	
	memmove(&cur_set_normal,&normal,sizeof(d3vct));

	if (state.select_mode==select_mode_vertex) {
		vertex_set_normals_vertexes(mesh_idx,&normal);
	}
	else {
		vertex_set_normals_trigs(mesh_idx,&normal);
	}
}

/* =======================================================

      Clear Bones From Vertexes
      
======================================================= */

void vertex_clear_bone_attachments_sel_vertexes(int mesh_idx)
{
	int					n,nt;
	model_vertex_type	*vertex;
	
	nt=model.meshes[mesh_idx].nvertex;
	vertex=model.meshes[mesh_idx].vertexes;
	
	for (n=0;n!=nt;n++) {
		if (vertex_check_sel_mask(mesh_idx,n)) {
			vertex->major_bone_idx=-1;
			vertex->minor_bone_idx=-1;
		}
		vertex++;
	}
}

/* =======================================================

      Automatically Set Vertexes for Bones
      
======================================================= */

void vertex_auto_bone_attachments(int mesh_idx)
{
	int					n,k,nt,bone_idx,
						cur_dist,dist;
	model_bone_type		*bone;
	model_vertex_type	*vertex;
	
	nt=model.meshes[mesh_idx].nvertex;
	vertex=model.meshes[mesh_idx].vertexes;
	
	for (n=0;n!=nt;n++) {
	
		if (vertex->major_bone_idx!=-1) {
			vertex++;
			continue;
		}
		
		bone_idx=-1;
		cur_dist=0;
		
		bone=model.bones;
		
		for (k=0;k!=model.nbone;k++) {
			dist=distance_get(bone->pnt.x,bone->pnt.y,bone->pnt.z,vertex->pnt.x,vertex->pnt.y,vertex->pnt.z);
			if ((bone_idx==-1) || (dist<cur_dist)) {
				bone_idx=k;
				cur_dist=dist;
			}
			
			bone++;
		}
		
		if (bone_idx!=-1) {
			vertex->major_bone_idx=bone_idx;
			vertex->minor_bone_idx=-1;
			vertex->bone_factor=1.0f;
		}
		
		vertex++;
	}
}

/* =======================================================

      Remove Vertexes
      
======================================================= */

void vertex_delete_sel_vertex(int mesh_idx)
{
	int					i,n,k,j,
						trig_start,trig_end,sz;
	char				*vertex_sel;
	bool				hit;

		// convert sels to boolean
		
	vertex_sel=(char*)malloc(model.meshes[mesh_idx].nvertex);
		
	for (i=0;i!=model.meshes[mesh_idx].nvertex;i++) {
		vertex_sel[i]=(char)vertex_check_sel_mask(mesh_idx,i);
	}
	
		// delete all vertexes
		
	i=0;
	
	while (i<model.meshes[mesh_idx].nvertex) {
	
		if (vertex_sel[i]==0) {
			i++;
			continue;
		}
		
			// delete the vertex
			
		sz=((model.meshes[mesh_idx].nvertex-1)-i);
		
		if (sz>0) {
			memmove(&model.meshes[mesh_idx].vertexes[i],&model.meshes[mesh_idx].vertexes[i+1],(sz*sizeof(model_vertex_type)));
			memmove(&vertex_sel[i],&vertex_sel[i+1],sz);
		}
		
		model.meshes[mesh_idx].nvertex--;
		
			// delete all trigs with vertex
			
		n=0;
		
		while (n<model.meshes[mesh_idx].ntrig) {
		
			hit=FALSE;
			
			for (j=0;j!=3;j++) {
				hit=hit||(model.meshes[mesh_idx].trigs[n].v[j]==i);
				if (model.meshes[mesh_idx].trigs[n].v[j]>i) model.meshes[mesh_idx].trigs[n].v[j]--;
			}
		
			if (!hit) {
				n++;
				continue;
			}
			
				// delete trig
					
			sz=((model.meshes[mesh_idx].ntrig-1)-n);
			
			if (sz>0) {
				memmove(&model.meshes[mesh_idx].trigs[n],&model.meshes[mesh_idx].trigs[n+1],(sz*sizeof(model_trig_type)));
			}
				
			model.meshes[mesh_idx].ntrig--;
		
				// move texture trig starts
					
			for (k=0;k!=max_model_texture;k++) {
			
				if (model.textures[k].frames[0].bitmap.gl_id!=-1) {
				
					trig_start=model.meshes[mesh_idx].materials[k].trig_start;
					trig_end=trig_start+model.meshes[mesh_idx].materials[k].trig_count;
					
					if (trig_start>n) {
						model.meshes[mesh_idx].materials[k].trig_start--;
					}
					else {
						if ((n>=trig_start) && (n<trig_end)) {
							model.meshes[mesh_idx].materials[k].trig_count--;
						}
					}
				
				}
				
			}
		}
	}
	
	free(vertex_sel);
	
		// clear sels
		
	vertex_clear_sel_mask(mesh_idx);
	vertex_clear_hide_mask(mesh_idx);
}

/* =======================================================

      Delete Unused Vertexes
      
======================================================= */

void vertex_delete_unused_vertexes(int mesh_idx)
{
	int					n,k,i,t,nvertex,ntrig,sz;
	unsigned char		*v_ok;
    model_trig_type		*trig;
	
		// vertex hit list
			
	nvertex=model.meshes[mesh_idx].nvertex;
	
	v_ok=(unsigned char*)malloc(nvertex);
	bzero(v_ok,nvertex);
	
		// find vertexes hit
		
	ntrig=model.meshes[mesh_idx].ntrig;
	trig=model.meshes[mesh_idx].trigs;
	
	for (n=0;n!=ntrig;n++) {
		for (k=0;k!=3;k++) {
			v_ok[trig->v[k]]=0x1;
		}
		trig++;
	}
	
		// delete unused vertexes
	
	for (n=(nvertex-1);n>=0;n--) {
	
		if (v_ok[n]==0x1) continue;
		
			// change all trigs vertex pointers
	
		trig=model.meshes[mesh_idx].trigs;
		
		for (i=0;i!=ntrig;i++) {
			for (t=0;t!=3;t++) {
				if (trig->v[t]>n) trig->v[t]--;
			}
			trig++;
		}
		
			// delete vertex
			
		sz=(nvertex-n)*sizeof(model_vertex_type);
		if (sz>0) memmove(&model.meshes[mesh_idx].vertexes[n],&model.meshes[mesh_idx].vertexes[n+1],sz);
		
		nvertex--;
	}
	
	model.meshes[mesh_idx].nvertex=nvertex;

	free(v_ok);
}

