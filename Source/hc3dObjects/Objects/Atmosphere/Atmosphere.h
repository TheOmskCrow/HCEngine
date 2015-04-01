#pragma once
#include <Object.h>
#include "../../../hc3dModelBuilder/hc3dModelBuilder.h"
namespace hc3d {

	class Atmosphere : public Object
	{
	public:
		Atmosphere(void);
		void Init();
		void Draw();
		static Vector3D getSun();
		static Vector3D getSun2();

	private:
		void SetShader(GLuint shaderprogram);
		void SunInit();
		Vector3D SunCalc(Vector3D player, float longitude, float latitude);

	private:
		static Vector3D sun;
		static Vector3D sun2;
		int SKY_SIZE;
		Model model, model1, model2, model3;
		Vector3D light;
		GLuint tex, tex1, tex2, tex3;
		GLuint shaderprogram, shaderprogram2, shaderprogram3;

	};
}
