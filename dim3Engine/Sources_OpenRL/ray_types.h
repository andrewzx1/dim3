	// force ray.h/ray_interface.h to compile correctly
	// when they are used together

#define _ray_types

	// types

typedef struct		{
                        float						x,y,z;
                    } ray_vector_type;
					
typedef struct		{
                        float						x,y,z;
                    } ray_point_type;
					
typedef struct		{
                        float						x,y;
                    } ray_uv_type;
					
typedef struct		{
						float						r,g,b,a;
					} ray_color_type;

typedef struct		{
                        ray_point_type				min,max;
                    } ray_bound_type;
					
typedef struct		{
						float						data[4][4];
					} ray_matrix_type;
					
typedef struct		{
						int							x,y;
					} ray_2d_point_type;
					
