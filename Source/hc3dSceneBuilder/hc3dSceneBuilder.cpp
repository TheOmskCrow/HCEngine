#include "stdafx.h"
#include "hc3dSceneBuilder.h"
#include "Control\Camera.h"
#include "Control\Info.h"
#include <iostream>

using namespace hc3d;

Vector3D SceneBuilder::offset = Vector3D(0, 0, 0);
float SceneBuilder::eyeNormScale = 1.0f;
std::vector<Model> SceneBuilder::models;
std::vector<int> SceneBuilder::modelNum;
std::vector<Vector3D> SceneBuilder::positions;
std::vector<Vector3D> SceneBuilder::rotation;
std::vector<Vector3D> SceneBuilder::size;

int SceneBuilder::LoadModel(std::string modelName) {
	Model model;
	models.push_back(model);
	int ret = models[models.size() - 1].init(modelName.c_str(), "Texture/rock7.png", "null", "null");
	if (ret == 1) {
		models.pop_back();
		return 1;
	}
	
//	std::cout << "MODEL LOADED" << std::endl;
	if (positions.size() == 0) {
		positions.push_back(Camera::getPosition());
		rotation.push_back(Vector3D(0, 0, 0));
		size.push_back(Vector3D(1, 1, 1));
		modelNum.push_back(0);
	}
	return 0;
}

void SceneBuilder::Draw() {
	const size_t posSize = positions.size();
	if (posSize == 0) return;
	positions[posSize - 1] = Camera::getPosition() + Info::GetEyeNormal() * eyeNormScale *  10.0f * size[posSize - 1].x + offset;

	for (size_t i = 0; i < posSize; i++) {
		glPushMatrix();
		glTranslatef(positions[i].x, positions[i].y, positions[i].z);
		glRotated(rotation[i].x, 1, 0, 0);
		glRotated(rotation[i].y, 0, 1, 0);
		glRotated(rotation[i].z, 0, 0, 1);
		glScalef(size[i].x, size[i].y, size[i].z);
		models[modelNum[i]].draw();
		glPopMatrix();
	}
}

int SceneBuilder::Release(std::string configFileName) {
	FILE* fileId;
	fopen_s(&fileId, configFileName.c_str(), "w");
	if (!fileId) return 1;
	for (size_t i = 0; i < positions.size(); i++) {
		fprintf(fileId, "%d %f %f %f %f %f %f %f %f %f\n", modelNum[i], positions[i].x, positions[i].y, positions[i].z, rotation[i].x, rotation[i].y, rotation[i].z, size[i].x, size[i].y, size[i].z);
	}
	fclose(fileId);
	Reload();
	return 0;
}

void SceneBuilder::Reload() {
	models.clear();
	modelNum.clear();
	positions.clear();
	size.clear();
	rotation.clear();
}

void SceneBuilder::Rotate(Vector3D additionalDir) {
	if (positions.size() == 0) return;
	rotation[rotation.size() - 1] += additionalDir;
}

void SceneBuilder::Resize(Vector3D multiplySize) {
	if (positions.size() == 0) return;
	size[size.size() - 1].x *= multiplySize.x;
	size[size.size() - 1].y *= multiplySize.y;
	size[size.size() - 1].z *= multiplySize.z;
}

void SceneBuilder::Add() {
//	std::cout << "PUSHING BACK MODEL" << std::endl;
	offset = Vector3D(0, 0, 0);
	if (positions.size() == 0) return;
	positions.push_back(Camera::getPosition());
	if (rotation.size() == 0) {
		rotation.push_back(Vector3D(0, 0, 0));
		size.push_back(Vector3D(1, 1, 1));
		modelNum.push_back(0);
	}
	else {
		rotation.push_back(rotation[rotation.size() - 1]);
		size.push_back(size[size.size() - 1]);
		modelNum.push_back(modelNum[modelNum.size() - 1]);
	}
}