#include "stdafx.h"
#include "Renderer.h"

#define MAX_MATERIALS 16
using namespace hc3d;

std::vector<Material> Renderer::materials;
GLuint Renderer::shaderprogram = 0;

void Renderer::SetShader(GLuint shaderprogram) {
	Renderer::shaderprogram = shaderprogram;
}

void Renderer::SetShader(const char* name) {
	ShaderLib::Get(name, Renderer::shaderprogram);
}

bool Renderer::UseMaterial(Material& data) {
	if (materials.size() == MAX_MATERIALS) return false;
	Renderer::materials.push_back(data);
	return true;
}

bool Renderer::UseMaterial(std::string name) {
	if (materials.size() == MAX_MATERIALS) return false;
	Material mat;
	if (!MatLib::Get(name, mat)) return false;
	Renderer::materials.push_back(mat);
	return true;
}

void Renderer::Process(bool useShader) {
	if(useShader) glUseProgram(shaderprogram);
	SendMaterials();
}

void Renderer::SendMaterials() {

	int *diffuseSamplers = new int[materials.size()];
	int *heightSamplers = new int[materials.size()];
	int *normalSamplers = new int[materials.size()];
	int *specularSamplers = new int[materials.size()];

	GLfloat* bumpVal = new float[materials.size()];
	GLfloat* parallaxOffset = new float[materials.size()];
	GLfloat* specPow = new float[materials.size()];
	GLfloat* specVal = new float[materials.size()];
	GLfloat* farColor = new float[materials.size() * 3];
	GLfloat* farDist = new float[materials.size()];
	int* ambientType = new int[materials.size()];

	int *_useDiffuse = new int[materials.size()];
	int *_useHeight = new int[materials.size()];
	int *_useNormal = new int[materials.size()];
	int *_useSpecular = new int[materials.size()];

	for (size_t i = 0; i < materials.size(); i++) {
		_useDiffuse[i] = materials[i]._initialized;
		_useHeight[i] = materials[i]._useParallax;
		_useNormal[i] = materials[i]._useBump;
		_useSpecular[i] = materials[i]._useSpecular;

		if (_useDiffuse[i]) {
			glActiveTexture(GL_TEXTURE0 + materials[i].diffuseMap);
			glBindTexture(GL_TEXTURE_2D, materials[i].diffuseMap);
			diffuseSamplers[i] = materials[i].diffuseMap;
			farColor[i * 3] = materials[i].diffuseFarColor.x;
			farColor[i * 3 + 1] = materials[i].diffuseFarColor.y;
			farColor[i * 3 + 2] = materials[i].diffuseFarColor.z;
			farDist[i] = materials[i].diffuseFarDist;
		}

		if (_useHeight[i]) {
			glActiveTexture(GL_TEXTURE0 + materials[i].heightMap);
			glBindTexture(GL_TEXTURE_2D, materials[i].heightMap);
			heightSamplers[i] = materials[i].heightMap;
		}

		if (_useNormal[i]) {
			glActiveTexture(GL_TEXTURE0 + materials[i].normalMap);
			glBindTexture(GL_TEXTURE_2D, materials[i].normalMap);
			normalSamplers[i] = materials[i].normalMap;
		}

		if (_useSpecular[i]) {
			glActiveTexture(GL_TEXTURE0 + materials[i].specularMap);
			glBindTexture(GL_TEXTURE_2D, materials[i].specularMap);
			specularSamplers[i] = materials[i].specularMap;
		}

		bumpVal[i] = materials[i].bumpVal;
		parallaxOffset[i] = materials[i].parallaxOffset;
		specPow[i] = materials[i].specPow;
		specVal[i] = materials[i].specVal;
		ambientType[i] = materials[i].ambientType;
	}
	glUniform1iv(glGetUniformLocation(
		shaderprogram, "diffuseSamplers"), materials.size(), diffuseSamplers);
	glUniform1iv(glGetUniformLocation(
		shaderprogram, "heightSamplers"), materials.size(), heightSamplers);
	glUniform1iv(glGetUniformLocation(
		shaderprogram, "normalSamplers"), materials.size(), normalSamplers);
	glUniform1iv(glGetUniformLocation(
		shaderprogram, "specularSamplers"), materials.size(), specularSamplers);

	glUniform1fv(glGetUniformLocation(
		shaderprogram, "bumpVal"), materials.size(), bumpVal);
	glUniform1fv(glGetUniformLocation(
		shaderprogram, "parallaxOffset"), materials.size(), parallaxOffset);
	glUniform1fv(glGetUniformLocation(
		shaderprogram, "specPow"), materials.size(), specPow);
	glUniform1fv(glGetUniformLocation(
		shaderprogram, "specVal"), materials.size(), specVal);
	glUniform3fv(glGetUniformLocation(
		shaderprogram, "farColor"), materials.size(), farColor);
	glUniform1fv(glGetUniformLocation(
		shaderprogram, "farDist"), materials.size(), farDist);
	glUniform1iv(glGetUniformLocation(
		shaderprogram, "ambientType"), materials.size(), ambientType);

	glUniform1iv(glGetUniformLocation(
		shaderprogram, "_useDiffuse"), materials.size(), _useDiffuse);
	glUniform1iv(glGetUniformLocation(
		shaderprogram, "_useHeight"), materials.size(), _useHeight);
	glUniform1iv(glGetUniformLocation(
		shaderprogram, "_useNormal"), materials.size(), _useNormal);
	glUniform1iv(glGetUniformLocation(
		shaderprogram, "_useSpecular"), materials.size(), _useSpecular);
	glUniform1i(glGetUniformLocation(
		shaderprogram, "materialNum"), materials.size());

	/// other must be parameters

	glUniform1f(glGetUniformLocation(
		shaderprogram, "time"), (Info::getTime() / 100.0)); 
	glUniform1f(glGetUniformLocation(
		shaderprogram, "Width"), Info::width());//screen width
	glUniform1f(glGetUniformLocation(
		shaderprogram, "Height"), Info::height());//screen height
	glUniform3f(glGetUniformLocation(
		shaderprogram, "eyeNorm"), Info::GetEyeNormal().x, Info::GetEyeNormal().y, Info::GetEyeNormal().z);
	glUniform3f(glGetUniformLocation(
		shaderprogram, "eyePos"), Camera::getPosition().x, Camera::getPosition().y, Camera::getPosition().z);
	glUniform3f(glGetUniformLocation(
		shaderprogram, "atmosphere"), Info::GetAtmoColor().x, Info::GetAtmoColor().y, Info::GetAtmoColor().z); //atmo color
	if (Info::GetReflect())
		glUniform3f(glGetUniformLocation(
		shaderprogram, "lightPos"), Info::GetSun().x, Info::GetSun().y, -Info::GetSun().z); //sun pos
	else
		glUniform3f(glGetUniformLocation(
		shaderprogram, "lightPos"), Info::GetSun().x, Info::GetSun().y, Info::GetSun().z);
	glUniform1i(glGetUniformLocation(
		shaderprogram, "flash"), Info::GetFlash()); //flashlight enable

	delete[] diffuseSamplers;
	delete[] heightSamplers;
	delete[] normalSamplers;
	delete[] specularSamplers;
	delete[] bumpVal;
	delete[] parallaxOffset;
	delete[] specPow;
	delete[] specVal;
	delete[] ambientType;
	delete[] _useDiffuse;
	delete[] _useHeight;
	delete[] _useNormal;
	delete[] _useSpecular;
	delete[] farColor;
	delete[] farDist;

}

void Renderer::DropSettings() {
	glUseProgram(0);
	Renderer::materials.clear();
	Renderer::shaderprogram = 0;
}

