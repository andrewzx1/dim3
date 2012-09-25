#include "ray_os_defs.h"
#include "ray.h"
#include "ray_interface.h"

extern ray_global_type		ray_global;

/* =======================================================

      Vector and Point Functions
      
======================================================= */

void ray_vector_normalize(ray_vector_type *v)
{
	float			f;
	
	f=sqrtf((v->x*v->x)+(v->y*v->y)+(v->z*v->z));
	if (f==0.0f) return;
	
	f=1.0f/f;
	v->x*=f;
	v->y*=f;
	v->z*=f;
}

void ray_vector_create_from_points(ray_vector_type *v,ray_point_type *p1,ray_point_type *p2)
{
	v->x=p1->x-p2->x;
	v->y=p1->y-p2->y;
	v->z=p1->z-p2->z;
}

void ray_vector_cross_product(ray_vector_type *cp,ray_vector_type *v1,ray_vector_type *v2)
{
	cp->x=(v1->y*v2->z)-(v2->y*v1->z);
	cp->y=(v1->z*v2->x)-(v2->z*v1->x);
	cp->z=(v1->x*v2->y)-(v2->x*v1->y);
}

float ray_vector_dot_product(ray_vector_type *v1,ray_vector_type *v2)
{
	return((v1->x*v2->x)+(v1->y*v2->y)+(v1->z*v2->z));
}

void find_triangle_point_for_UV(ray_mesh_type *mesh,ray_trig_type *trig,float u,float v,ray_point_type *p)
{
	float				uv;
	ray_point_type		*p0,*p1,*p2;

		// get the ray points
		
	p0=&mesh->vertex_block.vertexes[trig->vertex_idx[0]];
	p1=&mesh->vertex_block.vertexes[trig->vertex_idx[1]];
	p2=&mesh->vertex_block.vertexes[trig->vertex_idx[2]];
	
		// calculate the point
		
	uv=(1-u)-v;
	p->x=(uv*p0->x)+(u*p1->x)+(v*p2->x);
	p->y=(uv*p0->y)+(u*p1->y)+(v*p2->y);
	p->z=(uv*p0->z)+(u*p1->z)+(v*p2->z);
}

void ray_vector_find_line_point_for_T(ray_point_type *p,ray_vector_type *v,float t,ray_point_type *lp)
{
	lp->x=p->x+(v->x*t);
	lp->y=p->y+(v->y*t);
	lp->z=p->z+(v->z*t);
}

/* =======================================================

      Identity Matrix
      
======================================================= */

void rlMatrixIdentity(ray_matrix_type *mat)
{
	memset(mat,0x0,sizeof(ray_matrix_type));
	mat->data[0][0]=mat->data[1][1]=mat->data[2][2]=mat->data[3][3]=1;
}

/* =======================================================

      Multiple Matrix
      
======================================================= */

void rlMatrixMultiply(ray_matrix_type *mat_1,ray_matrix_type *mat_2)
{
	int					i;
	ray_matrix_type		mat;
	
	for (i=0;i!=4;i++) {
		mat.data[i][0]=
			(mat_1->data[i][0]*mat_2->data[0][0])+
			(mat_1->data[i][1]*mat_2->data[1][0])+
			(mat_1->data[i][2]*mat_2->data[2][0])+
			(mat_1->data[i][3]*mat_2->data[3][0]);

		mat.data[i][1]=
			(mat_1->data[i][0]*mat_2->data[0][1])+
			(mat_1->data[i][1]*mat_2->data[1][1])+
			(mat_1->data[i][2]*mat_2->data[2][1])+
			(mat_1->data[i][3]*mat_2->data[3][1]);

		mat.data[i][2]=
			(mat_1->data[i][0]*mat_2->data[0][2])+
			(mat_1->data[i][1]*mat_2->data[1][2])+
			(mat_1->data[i][2]*mat_2->data[2][2])+
			(mat_1->data[i][3]*mat_2->data[3][2]);

		mat.data[i][3]=
			(mat_1->data[i][0]*mat_2->data[0][3])+
			(mat_1->data[i][1]*mat_2->data[1][3])+
			(mat_1->data[i][2]*mat_2->data[2][3])+
			(mat_1->data[i][3]*mat_2->data[3][3]);
	}
	
	memmove(mat_1,&mat,sizeof(ray_matrix_type));
}

void rlMatrixVectorMultiply(ray_matrix_type *mat,ray_vector_type *v)
{
	float		fx,fy,fz;
	
	fx=v->x;
	fy=v->y;
	fz=v->z;
	
	v->x=(fx*mat->data[0][0])+(fy*mat->data[0][1])+(fz*mat->data[0][2])+mat->data[0][3];
	v->y=(fx*mat->data[1][0])+(fy*mat->data[1][1])+(fz*mat->data[1][2])+mat->data[1][3];
	v->z=(fx*mat->data[2][0])+(fy*mat->data[2][1])+(fz*mat->data[2][2])+mat->data[2][3];
}

/* =======================================================

      Rotation Matrixes
      
======================================================= */

void rlMatrixRotateX(ray_matrix_type *mat,float ang)
{
	float		r_cos,r_sin;
	double		rad;
	
	rad=(double)(ang*ANG_to_RAD);
	r_cos=(float)cos(rad);
	r_sin=(float)sin(rad);
	
	rlMatrixIdentity(mat);

	mat->data[1][1]=r_cos;
	mat->data[1][2]=r_sin;
	mat->data[2][1]=-r_sin;
	mat->data[2][2]=r_cos;
}

void rlMatrixRotateY(ray_matrix_type *mat,float ang)
{
	float		r_cos,r_sin;
	double		rad;
	
	rad=(double)(ang*ANG_to_RAD);
	r_cos=(float)cos(rad);
	r_sin=(float)sin(rad);
	
	rlMatrixIdentity(mat);
	
	mat->data[0][0]=r_cos;
	mat->data[0][2]=-r_sin;
	mat->data[2][0]=r_sin;
	mat->data[2][2]=r_cos;
}

void rlMatrixRotateZ(ray_matrix_type *mat,float ang)
{
	float		r_cos,r_sin;
	double		rad;
	
	rad=(double)(ang*ANG_to_RAD);
	r_cos=(float)cos(rad);
	r_sin=(float)sin(rad);
	
	rlMatrixIdentity(mat);
	
	mat->data[0][0]=r_cos;
	mat->data[0][1]=r_sin;
	mat->data[1][0]=-r_sin;
	mat->data[1][1]=r_cos;
}

/* =======================================================

      Normal Culling
      
======================================================= */

bool ray_normal_cull(ray_point_type *eye_point,ray_point_type *poly_mid_point,ray_vector_type *normal)
{
	return(((normal->x*(poly_mid_point->x-eye_point->x))+(normal->y*(poly_mid_point->y-eye_point->y))+(normal->z*(poly_mid_point->z-eye_point->z)))<=0.0f);
}

/* =======================================================

      Bound Collisions
      
======================================================= */

bool ray_bound_bound_collision(ray_bound_type *bnd_1,ray_bound_type *bnd_2)
{
	if (bnd_1->max.x<bnd_2->min.x) return(FALSE);
	if (bnd_1->max.y<bnd_2->min.y) return(FALSE);
	if (bnd_1->max.z<bnd_2->min.z) return(FALSE);

	if (bnd_1->min.x>bnd_2->max.x) return(FALSE);
	if (bnd_1->min.y>bnd_2->max.y) return(FALSE);
	if (bnd_1->min.z>bnd_2->max.z) return(FALSE);
	
	return(TRUE);
}

bool ray_bound_ray_collision(ray_point_type *p,ray_vector_type *v,ray_bound_type *bnd)
{
	float			f;
	ray_point_type	p1,p2;
	
		// start with simple bounding
		// eliminations
	
	if (v->x<0.0f) {
		if (p->x<bnd->min.x) return(FALSE);
	}
	else {
		if (p->x>bnd->max.x) return(FALSE);
	}
	
	if (v->y<0.0f) {
		if (p->y<bnd->min.y) return(FALSE);
	}
	else {
		if (p->y>bnd->max.y) return(FALSE);
	}
	
	if (v->z<0.0f) {
		if (p->z<bnd->min.z) return(FALSE);
	}
	else {
		if (p->z>bnd->max.z) return(FALSE);
	}
	
		// find set of two X points on line,
		// and compare them with Y and Z to
		// see if the vector passes these bounds
		
	f=(bnd->min.x-p->x)/v->x;
	
	p1.y=p->y+(v->y*f);
	p1.z=p->z+(v->z*f);
		
	f=(bnd->max.x-p->x)/v->x;
	
	p2.y=p->y+(v->y*f);
	p2.z=p->z+(v->z*f);
	
	if ((p1.y<bnd->min.y) && (p2.y<bnd->min.y)) return(FALSE);
	if ((p1.y>bnd->max.y) && (p2.y>bnd->max.y)) return(FALSE);
	
	if ((p1.z<bnd->min.z) && (p2.z<bnd->min.z)) return(FALSE);
	if ((p1.z>bnd->max.z) && (p2.z>bnd->max.z)) return(FALSE);
	
		// repeat for Y
		
	f=(bnd->min.y-p->y)/v->y;
	
	p1.x=p->x+(v->x*f);
	p1.z=p->z+(v->z*f);
		
	f=(bnd->max.y-p->y)/v->y;
	
	p2.x=p->x+(v->x*f);
	p2.z=p->z+(v->z*f);
	
	if ((p1.x<bnd->min.x) && (p2.x<bnd->min.x)) return(FALSE);
	if ((p1.x>bnd->max.x) && (p2.x>bnd->max.x)) return(FALSE);
	
	if ((p1.z<bnd->min.z) && (p2.z<bnd->min.z)) return(FALSE);
	if ((p1.z>bnd->max.z) && (p2.z>bnd->max.z)) return(FALSE);
	
		// repeat for Z
		
	f=(bnd->min.z-p->z)/v->z;
	
	p1.x=p->x+(v->x*f);
	p1.y=p->y+(v->y*f);
		
	f=(bnd->max.z-p->z)/v->z;
	
	p2.x=p->x+(v->x*f);
	p2.y=p->y+(v->y*f);
	
	if ((p1.x<bnd->min.x) && (p2.x<bnd->min.x)) return(FALSE);
	if ((p1.x>bnd->max.x) && (p2.x>bnd->max.x)) return(FALSE);
	
	if ((p1.y<bnd->min.y) && (p2.y<bnd->min.y)) return(FALSE);
	if ((p1.y>bnd->max.y) && (p2.y>bnd->max.y)) return(FALSE);
	
	return(TRUE);
}

/* =======================================================

      Distance Functions
      
======================================================= */

float ray_distance_between_points(ray_point_type *p1,ray_point_type *p2)
{
	double			dx,dy,dz;

	dx=(double)(p1->x-p2->x);
	dy=(double)(p1->y-p2->y);
	dz=(double)(p1->z-p2->z);

	return((float)sqrt((dx*dx)+(dy*dy)+(dz*dz)));
}
