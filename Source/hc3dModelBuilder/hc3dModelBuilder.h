#pragma once
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the HC3DMODELBUILDER_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// HC3DMODELBUILDER_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef HC3DMODELBUILDER_EXPORTS
#define HC3DMODEL_API __declspec(dllexport)
#else
#define HC3DMODEL_API __declspec(dllimport)
#endif

#include <string.h>
#include <vector>
#include <hc3dMath.h>
#include <hc3dContentLoader.h>
#include <hc3dPhysics.h>

namespace hc3d {

	class HC3DMODEL_API Model
	{
	public:
		Model(void);
		~Model(void);
		int init(const char* name, char* texture_name, char* normal_map_name, char* height_map_name);
		Vector3D get_first_vert();
		Vector3D *get_faces_list();
		Vector3D *get_normal_list();
		int normal_list_size();
		int faces_list_size();
		int addCollision(Vector3D translation, Vector3D rotation, Vector3D scale, bool isStatic = true);
		void AddHitBoxModel(const char* name);
		void AddModelMass(float mass);
		void AddLodModels(std::vector<std::string> names, std::vector<float> distance);
		void draw(float distance = 0.0f);
		void CompileLod(int i);
		void no_shaders();
		void enable_spec();
		void setScale(float);
		void setScaleTexCoord(float);
	private:
		int list_id;
		void convert(const char* input);
		void setShader();
		void refract();
		bool collision, shaders;
		float **lodVertex;
		float **lodNormal;
		float **lodTexCoord;
		int **lodFaces;
		int *lodFacesSize;
		float* lodDistance;
		int* lodLists;
		int lodNum;
		float mass;

		float *vertex; int vertex_size;
		float *vertex_n; int vertex_n_size;
		float *vertex_t; int vertex_t_size;
		int *faces; int faces_size, trans;

		float *collisionVertex;
		int *collisionFaces;
		int collisionFacesNum;
		float _mdl_x, _mdl_y, _mdl_z, level, texCoordScale;
		GLuint *texture;
		GLuint shaderProgram;
		Vector3D mdl_pos;
		Vector3D mdl_center;
		float mdl_scale;
		bool load_m, mdl_change, height_as_spec;

	};
}


