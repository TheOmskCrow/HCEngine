#pragma once
#include <Object.h>

namespace hc3d {
	class Fish : public Object
	{
	public:
		Fish(void);
		~Fish(void);
		void Init();
		void Draw();
		void move(int);
		void AI(int);
		float dist(Vector3D, Vector3D);
		void setShader();

		GLfloat *vertex; GLfloat *vertex2; int vertex_size;
		GLfloat *vertex_n; GLfloat *vertex2_n; int vertex_n_size;
		GLfloat *vertex_t; int vertex_t_size;
		GLfloat *r_vertex; int r_vertex_size;
		GLfloat *r_vertex_n; int r_vertex_n_size;
		GLfloat *r_vertex_t; int r_vertex_t_size;
		GLuint* BufferName;

		GLint *faces; int faces_size;
		float *_mdl_x, *_mdl_y, *_mdl_z, *stamina_boost;
		GLuint *texture;
		GLuint shaderprogram;

		int list_id;

		Vector3D *mdl_pos;
		Vector3D *mdl_center;
		Vector3D *mdl_memory;
		Vector3D *mdl_direction, *mdl_speed;
		Vector3D *offset;
		int fish_num;
		float mdl_scale;
		bool load_m, mdl_change;

	};
}
