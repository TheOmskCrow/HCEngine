#pragma once
#include <Object.h>

namespace hc3d {
	class Clouds : public Object
	{
	public:
		void Init();
		void Draw();
		Clouds(void);
		~Clouds(void);
		bool sort;
	private:
		Vector3D cloudSpeed;
		GLuint shaderprogram;
		GLuint *texture;
		float layerHeight;
		Vector3D pos;
		float distance(Vector3D a, Vector3D b);
		Vector3D* qSort(Vector3D *A, int low, int high);
		typedef struct {
			Vector3D location;
			float value;
		} Data;
		Data *data;
		bool exploded;
		Vector3D *list, *speed, *normal, *normal_list;
		float* locSpeed;
		int *size, *type;
		int num_of_puff, num_of_normal;
		int _width, _height, _depth;
		Vector3D start;
		float yaw, pitch, roll;
		void setShader(Vector3D, float, int);
		float dist(Vector3D, Vector3D);
	};

}