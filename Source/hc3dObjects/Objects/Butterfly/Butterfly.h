#pragma once
#include <Object.h>

namespace hc3d {
	class Butterfly : public Object{
	private:
		GLuint shaderprogram, tex, tex2;
		int vert_shader, frag_shader;
		int but_num;
		Vector3D *but, *speed, pos;
		float offset;
		float *cadr;
		float *angle;
		float *hue;
		float *hue_speed;
		float *size;
		float *but_yaw;
		bool *rotating;
		float *rotate_angle;
		float *blink_speed;
		bool *sitting;
		bool *reverse;
		float *gr_size;
		bool *drawable;
		Vector3D* qSort(Vector3D *A, int low, int high);
		float distance(Vector3D a, Vector3D b);
		void ButsInit();
		void ButInit(int);
		void ButReset(int);
		float rnd();
		void setShader();
	public:
		void Init();
		void Draw();
		Butterfly(void);
		~Butterfly(void);
	};
}