#pragma once
#include <map>
#include <string>
#include "hc3dContentLoader.h"
#include "Material.h"

namespace hc3d {

	typedef std::map<std::string, GLuint> Shaders;

	class HC3D_API ShaderLib{
	private:
		static Shaders shaders;
		static GLuintMap shadersID;

	public:
		static bool Add(const std::string name, IN const GLuint shaderprogram);
		static bool Add(const std::string name, IN const std::string vertexFileName, IN const std::string fragmentFileName);
		static bool Get(const std::string& name, OUT GLuint& shaderprogram);
		static bool IsUsed(const std::string);
		static bool IsUsed(const GLuint& shaderID);

	private:
		static bool Use(const GLuint shaderID);
		static bool UnUse(const GLuint shaderID);
	};
}