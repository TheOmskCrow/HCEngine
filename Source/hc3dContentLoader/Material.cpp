#include "stdafx.h"
#include "Material.h"
#include <iostream>

using namespace hc3d;

Materials MatLib::materials;
GLuintMap MatLib::usedTextureID;

Material::Material() : ambientType(NONE),
_initialized(false), _useBump(false), _useParallax(false), _useSpecular(false)
{}


Material::~Material()
{
}

bool Material::SetDiffuse(GLuint diffuseMap) {
	if (MatLib::IsUsed(diffuseMap) || _initialized) return false;
	_initialized = true;
	this->diffuseMap = diffuseMap;
	return MatLib::Use(diffuseMap);
}

bool Material::SetDiffuse(std::string diffuseMapName) {
	if (_initialized) return false;
	_initialized = true;
	try {
		diffuseMap = LoadTexture(diffuseMapName.c_str());
		std::cout << "diffuseMap " << diffuseMap << std::endl;
		return MatLib::Use(diffuseMap);
	}
	catch (...) { return false; }
}

bool Material::SetNormal(GLuint normalMap, float bumpVal) {
	if (MatLib::IsUsed(normalMap) || _useBump) return false;

	this->normalMap = normalMap;
	_useBump = true;
	this->bumpVal = bumpVal;
	return MatLib::Use(normalMap);
}

bool Material::SetNormal(std::string normalMapName, float bumpVal) {
	if (_useBump) return false;

	try {
		normalMap = LoadTexture(normalMapName.c_str());
		_useBump = true;
		this->bumpVal = bumpVal;
		return MatLib::Use(normalMap);
	}
	catch (...) { return false; }
}

bool Material::SetHeight(GLuint heightMap, float parallaxOffset) {
	if (MatLib::IsUsed(heightMap) || _useParallax) return false;

	this->heightMap = heightMap;
	_useParallax = true;
	this->parallaxOffset = parallaxOffset;
	return MatLib::Use(heightMap);
}

bool Material::SetHeight(std::string heightMapName, float parallaxOffset) {
	if (_useParallax) return false;

	try {
		heightMap = LoadTexture(heightMapName.c_str());
		_useParallax = true;
		this->parallaxOffset = parallaxOffset;
		return MatLib::Use(heightMap);
	}
	catch (...) { return false; }
}

bool Material::SetSpecular(GLuint specularMap, float specPow, float specVal) {
	if (MatLib::IsUsed(specularMap) || _useSpecular) return false;

	this->specularMap = specularMap;
	_useSpecular = true;
	this->specPow = specPow;
	this->specVal = specVal;
	return MatLib::Use(specularMap);
}

bool Material::SetSpecular(std::string specularMapName, float specPow, float specVal) {
	if (_useSpecular) return false;

	try {
		specularMap = LoadTexture(specularMapName.c_str());
		_useSpecular = true;
		this->specPow = specPow;
		this->specVal = specVal;
		return MatLib::Use(specularMap);
	}
	catch (...) { return false; }
}

bool Material::SetAmbient(int ambientType) {
	this->ambientType = ambientType;
	return true;
}
bool Material::DeleteMaterial() {
	try {
		if (_initialized) {
			_initialized = false;
			MatLib::UnUse(diffuseMap);
			glDeleteTextures(1, &diffuseMap);
		}
		if (_useBump) {
			_useBump = false;
			MatLib::UnUse(normalMap);
			glDeleteTextures(1, &normalMap);
		}
		if (_useParallax) {
			_useParallax = false;
			MatLib::UnUse(heightMap);
			glDeleteTextures(1, &heightMap);
		}
		if (_useSpecular) {
			_useSpecular = false;
			MatLib::UnUse(specularMap);
			glDeleteTextures(1, &specularMap);
		}
	}
	catch (...) { return false; }
	return true;
}

///Matlib methods

bool MatLib::Add(const std::string name, IN const Material data) {
	return materials.insert(Materials::value_type(name, data)).second;
}

bool MatLib::Get(const std::string& name, OUT Material& data) {
	Materials::iterator it = materials.find(name);
	if (it != materials.end()) {
		data = (*it).second;
		return true;
	}
	return false;
}
bool MatLib::IsUsed(const GLuint& textureID) {
	GLuintMap::iterator it = usedTextureID.find(textureID);
	return it != usedTextureID.end();
}

bool MatLib::IsUsed(const std::string name) {
	Materials::iterator it = materials.find(name);
	return it != materials.end();
}

bool MatLib::Use(const GLuint textureID) {
	return usedTextureID.insert(GLuintMap::value_type(textureID, true)).second;
}

bool MatLib::UnUse(const GLuint textureID) {
	GLuintMap::iterator it = usedTextureID.find(textureID);
	if (it == usedTextureID.end()) return false;

	usedTextureID.erase(it);
	return true;
}
