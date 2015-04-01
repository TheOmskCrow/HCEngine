#pragma once
#include <vector>
#include <Object.h>
//#include <hc3dModelBuilder.h>
#include "../../../hc3dModelBuilder/hc3dModelBuilder.h" // сука блять студия ебанутая

using namespace std;
namespace hc3d {

	class Grass : public Object{
	private:
		Model *model;
		GLuint shaderprogram, tex, tex1, tex2, tex3, tex4, tex5, tex6;
		int vert_shader, frag_shader;
		int grass_num;
		Vector3D *grass, pos, *terNormal;
		Vector3D **borders;
		float offset;
		int *type;
		float *cadr;
		float *angle;
		float *gr_size;
		void qSort(vector<Vector3D> &A, vector<int> &B, int low, int high);
		float distance(const Vector3D &a, const Vector3D &b);
		void grass_init();
		void setShader();
	public:
		static float windWave;
		static Vector3D windCenter;
		void Init();
		void Draw();
		Grass(void);
		~Grass(void);
	};
}