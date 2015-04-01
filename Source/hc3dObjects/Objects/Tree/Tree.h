#pragma once
#include <Object.h>
#include "../../../hc3dModelBuilder/hc3dModelBuilder.h"

namespace hc3d {

	class Tree : public Object{
	private:
		Model *model;
		GLuint shaderprogram, shaderprogram1, tex, tex1, tex2, tex3;
		int vert_shader, frag_shader;
		int Tree_num;
		Vector3D *tree, pos;
		float offset;
		float *cadr;
		float *angle;
		float *gr_size;
		float *gr_width;
		float *type;
		Vector3D* qSort(Vector3D *A, int low, int high);
		float distance(Vector3D a, Vector3D b);
		void Tree_init();
		float rnd();
		void setShader(int);
	public:
		void Init();
		void Draw();
		Tree(void);
		~Tree(void);
	};

}