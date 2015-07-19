#pragma once
#include "../dllstorage.h"
#include <map>
#include <string>
#include "hc3dContentLoader.h"
#include "Material.h"

namespace hc3d {

	typedef std::map<std::string, GLuint> Shaders;

	class HC3DSTORAGE_API ShaderLib{
	private:
		static Shaders shaders;
	public:
		static bool Add(const char* name, IN const GLuint shaderprogram);
		static bool Add(const char* name, IN const char* vertexFileName, IN const char* fragmentFileName);
		static bool Get(const char* name, OUT GLuint& shaderprogram);
		static bool IsUsed(const char* name); //here we need only to check existance
	};
}