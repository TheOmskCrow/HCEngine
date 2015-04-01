#include "ShaderLib.h"


using namespace hc3d;

bool ShaderLib::Add(const std::string name, IN const Material data) {
	return materials.insert(Materials::value_type(name, data)).second;
}

bool ShaderLib::Get(const std::string& name, OUT Material& data) {
	Materials::iterator it = materials.find(name);
	if (it != materials.end()) {
		data = (*it).second;
		return true;
	}
	return false;
}
bool ShaderLib::IsUsed(const GLuint& textureID) {
	GLuintMap::iterator it = usedTextureID.find(textureID);
	return it != usedTextureID.end();
}

bool ShaderLib::IsUsed(const std::string name) {
	Materials::iterator it = materials.find(name);
	return it != materials.end();
}

bool ShaderLib::Use(const GLuint textureID) {
	return usedTextureID.insert(GLuintMap::value_type(textureID, true)).second;
}

bool ShaderLib::UnUse(const GLuint textureID) {
	GLuintMap::iterator it = usedTextureID.find(textureID);
	if (it == usedTextureID.end()) return false;

	usedTextureID.erase(it);
	return true;
}
