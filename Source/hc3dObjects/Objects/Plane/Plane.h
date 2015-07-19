#pragma once

#include <Object.h>
#include "../../../hc3dModelBuilder/hc3dModelBuilder.h"

namespace hc3d {

	class Plane : public Object {
	
	public:
		void Init();
		void Draw();
		Plane(void);
		~Plane(void);
	private:
		std::vector<Vector3D> list;
		std::vector<int> checked;
		Model *model;
		GLuint shaderprogram, tex;
		int vert_shader, frag_shader;
		int state;
		BodyData* matrix;
	private:
		void DetectCheckPoints();
		float distance(Vector3D a, Vector3D b);
		float rnd();
		void setShader();
	};
}