#pragma once
#include <Object.h>

namespace hc3d {

	class Water : public Object
	{
	public:
		Water(void);
		~Water(void);
		static void reflect();
		static void refract();
		void Draw();
		void Init();

	private:

		void LodGenerate(int num, int level);
		void DrawMain();
		void setShader();
		void CenterGenerate(int num);
		float dist(Vector3D a, Vector3D b);

	private:

		GLuint shaderprogram;
		static GLuint *texture;
		int **static_lists;
		Vector3D *list_center;
		int list_id;
	};

}