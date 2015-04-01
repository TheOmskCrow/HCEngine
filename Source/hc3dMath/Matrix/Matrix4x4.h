#pragma once
#include "Vector/Vector3D.h"

namespace hc3d {
	class HC3D_API Matrix4x4
	{
	public:
		float m[16];
		void set(float a0, float a1, float a2, float a3, float a4, float a5, float a6, float a7, float a8, float a9, float a10, float a11, float a12, float a13, float a14, float a15);
		void setToIdentity();
		void copy(Matrix4x4 M);
		void setToTranslation(Vector3D v);
		Matrix4x4 mult(Matrix4x4 a);
		Vector3D mult(Vector3D b);
		Matrix4x4(void);
	};

}