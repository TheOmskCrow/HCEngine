#pragma once

#include <vector>
#include <map>
#include <hc3dContentLoader.h>
#include <hc3dMath.h>
#include <dllmain.h>
#include <Material.h>

namespace hc3d {
	class HC3D_API Renderer
	{
	public:
		static void SetShader(GLuint shaderprogram);
		static bool UseMaterial(Material& data);
		static bool UseMaterial(std::string name);
		static void Process(bool useShader = true);
		static void DropSettings();

	private:
		static std::vector<Material> materials;
		static GLuint shaderprogram;

	private:
		static void SendMaterials();
	};
}
