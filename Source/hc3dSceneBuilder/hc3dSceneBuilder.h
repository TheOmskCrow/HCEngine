#pragma once
#include <hc3dContentLoader.h>
#include <hc3dModelBuilder.h>
#include <hc3dMath.h>
#include <vector>
#include <time.h>
#include "dllmain.h"

#pragma warning(disable : 4251)

namespace hc3d {
	class HC3DSCENEBUILDER_API SceneBuilder
	{
	public:
		static int LoadModel(std::string modelName);
		static void Draw();
		static int Release(std::string configFileName);
		static void Add();
		static void Rotate(Vector3D additionalDir);
		static void Resize(Vector3D multiplySize);
	public:
		static Vector3D offset;
		static float eyeNormScale;
	private:

		static void Reload();
	private:
		static std::vector<Model> models;
		static std::vector<int> modelNum;
		static std::vector<Vector3D> positions;
		static std::vector<Vector3D> rotation;
		static std::vector<Vector3D> size;
	};

}