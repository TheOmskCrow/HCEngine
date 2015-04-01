#pragma once

#include <Object.h>

namespace hc3d {
	class Star : public Object
	{
	private:
		int num;
		float longitude, latitude;
		Vector3D **matrcies;
		GLuint shaderprogram;
		GLuint *texture;
		Vector3D **pos;
		float *yaw;
		float *pitch;
		float *roll;
		float *size;
		int list_id;
		void setShader();
		void rebuildMatrix();
	public:
		void Init();
		void Draw();
		Star(void);
		~Star(void);
	};

}