#include "stdafx.h"
#include "ShaderLib.h"


using namespace hc3d;
Shaders ShaderLib::shaders;

bool ShaderLib::Add(IN const char* name, IN const GLuint shaderprogram) {
	return shaders.insert(Shaders::value_type(name, shaderprogram)).second;
}

bool ShaderLib::Add(IN const char* name, IN const char* vertexFileName, IN const char* fragmentFileName) {
	auto shaderprogram = LoadShader(vertexFileName, fragmentFileName);
	return shaders.insert(Shaders::value_type(name, shaderprogram)).second;
}

bool ShaderLib::Get(IN const char* name, OUT GLuint& shaderprogram) {
	auto it = shaders.find(std::string(name));
	if (it != shaders.end()) {
		shaderprogram = (*it).second;
		return true;
	}
	return false;
}

bool ShaderLib::IsUsed(const char* name) {
	auto it = shaders.find(std::string(name));
	if (it != shaders.end()) return true;
	return false;
}
