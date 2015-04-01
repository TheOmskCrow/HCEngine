#pragma once

#include <Object.h>
#include "../../../hc3dModelBuilder/hc3dModelBuilder.h"

namespace hc3d {

	class Stone : public Object {
	private:
		Model *model;
		int modelNum;
		GLuint shaderprogram, tex, tex1, tex2;
		int vert_shader, frag_shader;
		int Stone_num;
		Vector3D *stone, pos;
		float offset;
		float *cadr;
		int *matrix;
		Vector3D *angle;
		Vector3D *gr_size;
		int *type;
		Vector3D* qSort(Vector3D *A, int low, int high);
		float distance(Vector3D a, Vector3D b);
		void Stone_init();
		float rnd();
		void setShader();
	public:
		void Init();
		void Draw();
		Stone(void);
		~Stone(void);
	};
}