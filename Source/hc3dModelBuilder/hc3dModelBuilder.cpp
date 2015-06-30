// hc3dModelBuilder.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "hc3dModelBuilder.h"
#include <hc3dControllers.h>

#include "StdAfx.h"
#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>
#include <gl\GL.h>
#include <gl\GLU.h>
#include <freeglut.h>
#include <glext.h>

#define pi 3.1415

using namespace std;
using namespace hc3d;


Model::Model(void)
{
	state = 0;
	prevState = 0;
	changeTimeRemaining = 0.0f;
	changeTimeAll = 0.0f;
	lodNum = 0;
	collisionFacesNum = 0;
	mass = 0.0f;
	mdl_scale = 0.0;
	_mdl_x = 0;
	_mdl_y = 0;
	_mdl_z = pi / 2.0;
	collision = true;
	shaders = true;
	texCoordScale = Vector2D(1,1);
	height_as_spec = false;
}


Model::~Model(void)
{
}

void Model::setScaleTexCoord(Vector2D scale) {
	texCoordScale = scale;
}

void Model::setScaleTexCoord(float x, float y) {
	texCoordScale = Vector2D(x, y);
}

void Model::setScaleTexCoord(float x) {
	texCoordScale = Vector2D(x, x);
}

void Model::refract() {
	glActiveTexture(GL_TEXTURE0 + Info::GetCurBindTexture());
	Info::RaiseCurBindTexture();
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, Info::max_water, Info::max_water, 0);
}

void Model::enable_spec() {
	height_as_spec = true;
}
void Model::setShader() {
	glUniform1i(glGetUniformLocation(
		shaderProgram, "shadowing"), Info::shadow);
	glUniform1f(glGetUniformLocation(
		shaderProgram, "Width"), Info::width());
	glUniform1f(glGetUniformLocation(
		shaderProgram, "Height"), Info::height());

	glActiveTexture(GL_TEXTURE25);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glUniform1i(glGetUniformLocation(
		shaderProgram, "model"),25);
	//Info::RaiseCurBindTexture();

	glActiveTexture(GL_TEXTURE26);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glUniform1i(glGetUniformLocation(
		shaderProgram, "normalMap"), 26);
	//Info::RaiseCurBindTexture();

	glActiveTexture(GL_TEXTURE27);
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glUniform1i(glGetUniformLocation(
		shaderProgram, "heightMap"), 27);
	if (height_as_spec) {
		glUniform1i(glGetUniformLocation(
			shaderProgram, "specEnabled"), 1);
		glUniform1i(glGetUniformLocation(
			shaderProgram, "specularMap"), 27);
	}
//	Info::RaiseCurBindTexture();

	Vector3D sun = 0.5*(Info::GetSun());
	glUniform3f(glGetUniformLocation(
		shaderProgram, "lightPos"), sun.x, sun.y, sun.z);
	Camera camera;
	Vector3D player = camera.getPosition();
	glUniform3f(glGetUniformLocation(
		shaderProgram, "eyePos"), player.x, player.y, player.z);
	glUniform1i(glGetUniformLocation(
		shaderProgram, "flash"), Info::flash);
	glUniform3f(glGetUniformLocation(
		shaderProgram, "eyeNorm"), Info::eye_normal.x, Info::eye_normal.y, Info::eye_normal.z);

}

void Model::AddModelMass(float mass) {
	this->mass = mass;
}

void Model::convert(const char* input) {
	bool vn = false, vt = false, v = false, f = false;
	int vn_num = 0, vt_num = 0, v_num = 0, f_num = 0;
	string vn_buf, vt_buf, v_buf, f_buf;
	const char* name = input;
	ifstream file(name);
	char buffer, old, very_old;
	if (!file.is_open()) {
		cout << "no such file\n";
		return;
	}
	file >> std::noskipws;
	file >> old;
	ofstream file2("output.mdl");
	while (1) {
		file >> buffer;
		if (file.eof()) break;
		if (old == 'v' && buffer == 'n' && very_old == '\n')
			vn = true;
		if (old == 'v' && buffer == 't' && very_old == '\n')
			vt = true;
		if (old == 'v' && buffer == ' ' && very_old == '\n')
			v = true;
		if (old == 'f' && buffer == ' ' && very_old == '\n')
			f = true;
		if (vn)
		if ((buffer <= '9' && buffer >= '0') || buffer == '\n' || buffer == '\t' || buffer == ' ' || buffer == '-' || buffer == '.') {
			vn_buf += buffer;
		}
		if (vt)
		if ((buffer <= '9' && buffer >= '0') || buffer == '\n' || buffer == '\t' || buffer == ' ' || buffer == '-' || buffer == '.') {
			vt_buf += buffer;

		}
		if (v)
		if ((buffer <= '9' && buffer >= '0') || buffer == '\n' || buffer == '\t' || buffer == ' ' || buffer == '-' || buffer == '.') {
			v_buf += buffer;

		}
		if (f) {
			if (buffer == '/') buffer = ' ';
			if ((buffer <= '9' && buffer >= '0') || buffer == '\n' || buffer == '\t' || buffer == ' ' || buffer == '-' || buffer == '.') {
				f_buf += buffer;
			}
		}
		if (buffer == '\n') {
			if (vn) {
				vn = false;
				vn_num++;
			}
			if (v) {
				v = false;
				v_num++;
			}
			if (vt) {
				vt = false;
				vt_num++;
			}
			if (f) {
				f = false;
				f_num++;
			}
		}
		very_old = old;
		old = buffer;


	}
	file2 << v_num << '\n' << v_buf << vn_num << '\n' << vn_buf << vt_num << '\n' << vt_buf << f_num << '\n' << f_buf;
	return;
}
void Model::setScale(float val) {
	mdl_scale = val;
}

void Model::AddHitBoxModel(const char* name) {
	remove("output.mdl");
	convert(name);
	std::ifstream file("output.mdl");
	int size;
	if (!file.is_open()) {
		cout << "can not load '" << name << "'";
		return;
	}
	file >> size;
	collisionVertex = new float[size * 3];
	for (int j = 0; j < size * 3; j++) {
		file >> collisionVertex[j];
	}
	file >> size;
	for (int j = 0; j < size * 3; j++) {
		float hole;
		file >> hole;
	}
	file >> size;
	for (int j = 0; j < size * 3; j++) {
		float hole;
		file >> hole;
	}
	file >> size;
	collisionFaces = new int[size * 9];
	collisionFacesNum = size * 9;
	for (int j = 0; j < size * 9; j++) {
		file >> collisionFaces[j];
	}
}

void Model::AddState(std::string name, int num) {
	stateVertex.push_back(0);
	stateNormal.push_back(0);
	stateTexCoord.push_back(0);
	stateFaces.push_back(0);
	stateFacesSize.push_back(0);

	int curState = stateVertex.size() - 1;
	stateNum.insert(std::pair<int, int>(num, curState));
	

	remove("output.mdl");
	convert(name.c_str());
	std::ifstream file("output.mdl");
	int size;
	if (!file.is_open()) {
		cout << "can not load '" << name.c_str() << "'";
		return;
	}
	file >> size;
	stateVertex[curState] = new float[size * 3];
	for (int j = 0; j < size * 3; j++) {
		file >> stateVertex[curState][j];
	}
	file >> size;
	stateNormal[curState] = new float[size * 3];
	for (int j = 0; j < size * 3; j++) {
		file >> stateNormal[curState][j];
	}
	file >> size;
	stateTexCoord[curState] = new float[size * 3];
	for (int j = 0; j < size * 3; j++) {
		file >> stateTexCoord[curState][j];
	}
	file >> size;
	stateFaces[curState] = new int[size * 9];
	stateFacesSize[curState] = size * 9;
	for (int j = 0; j < size * 9; j++) {
		file >> stateFaces[curState][j];
	}
}

void Model::SetState(int state, float changeTime) {
	prevState = this->state;
	this->state = state;
	
	changeTimeRemaining = changeTime;
	changeTimeAll = changeTime;
}

void Model::AddLodModels(std::vector<std::string> names, std::vector<float> distance) {
	if (names.size() != distance.size()) {
		std::runtime_error("Unable to load LOD for models");
		exit(0);
	}

	lodNum = names.size();
	if (lodNum == 0) return;

	lodVertex = new float*[lodNum];
	lodNormal = new float*[lodNum];
	lodTexCoord = new float*[lodNum];
	lodFaces = new int*[lodNum];

	lodLists = new int[lodNum];
	lodDistance = new float[lodNum];
	lodFacesSize = new int[lodNum];

	for (size_t i = 0; i < names.size(); i++) {
		lodLists[i] = 0;
		lodDistance[i] = distance[i];
		remove("output.mdl");
		convert(names[i].c_str());
		std::ifstream file("output.mdl");
		int size;
		if (!file.is_open()) {
			cout << "can not load '" << names[i].c_str() << "'";
			return;
		}
		file >> size;
		lodVertex[i] = new float[size * 3];
		for (int j = 0; j < size * 3; j++) {
			file >> lodVertex[i][j];
		}
		file >> size;
		lodNormal[i] = new float[size * 3];
		for (int j = 0; j < size * 3; j++) {
			file >> lodNormal[i][j];
		}
		file >> size;
		lodTexCoord[i] = new float[size * 3];
		for (int j = 0; j < size * 3; j++) {
			file >> lodTexCoord[i][j];
		}
		file >> size;
		lodFaces[i] = new int[size * 9];
		lodFacesSize[i] = size * 9;
		for (int j = 0; j < size * 9; j++) {
			file >> lodFaces[i][j];
		}
	}
}

void Model::no_shaders() {
	shaders = false;
}
int Model::init(const char* name, char* texture_name, char* normal_map_name, char* height_map_name) {
	list_id = 0;
	if (shaders) {
		if (!Info::GetModelShaderInited()) {
			shaderProgram = LoadShader("Shaders/model.vert", "Shaders/model.frag");
			Info::SetModelShaderInited(true);
			Info::SetModelShader(shaderProgram);
			std::cout << "model shaders loaded" << std::endl;
		}
		else {
			shaderProgram = Info::GetModelShader();
			std::cout << "model shaders picked" << std::endl;
		}

		texture = new GLuint[3];
		glActiveTexture(GL_TEXTURE0 + Info::cur_loading_texture);
		Info::cur_loading_texture++;
		if (strcmp(texture_name, "null") != 0)
			texture[0] = LoadTexture(texture_name);
		else texture[0] = CreateTexture(GL_RGBA, 1);

		glActiveTexture(GL_TEXTURE0 + Info::cur_loading_texture);
		Info::cur_loading_texture++;
		if (strcmp(normal_map_name, "null") != 0)
			texture[1] = LoadTexture(normal_map_name);
		else texture[1] = CreateTexture(GL_RGBA, 1);

		glActiveTexture(GL_TEXTURE0 + Info::cur_loading_texture);
		Info::cur_loading_texture++;
		if (strcmp(height_map_name, "null") != 0)
			texture[2] = LoadTexture(height_map_name);
		else texture[2] = CreateTexture(GL_RGBA, 1);
	}
	mdl_pos = Vector3D(0, 0, 0);
	mdl_center = Vector3D(0, 0, 0);
	if (mdl_scale == 0.0) mdl_scale = 0.03;
	load_m = false, mdl_change = true;
	remove("output.mdl");
	convert(name);
	std::ifstream file("output.mdl");
	int size;
	if (!file.is_open()) {
		cout << "can not load '" << name << "'";
		return 1;
	}
	file >> size;
	vertex_size = size * 3;
	vertex = new float[size * 3];
	for (int i = 0; i < size * 3; i++) {
		file >> vertex[i];
	}
	file >> size;
	vertex_n_size = size * 3;
	vertex_n = new float[size * 3];
	for (int i = 0; i < size * 3; i++) {
		file >> vertex_n[i];
	}
	file >> size;
	vertex_t_size = size * 3;
	vertex_t = new float[size * 3];
	for (int i = 0; i < size * 3; i++) {
		file >> vertex_t[i];
	}
	file >> size;
	faces_size = size * 9;
	faces = new int[size * 9];
	for (int i = 0; i < size * 9; i++) {
		file >> faces[i];
	}
	return 0;
}

int Model::addCollision(Vector3D translation, Vector3D rotation, Vector3D scale, bool isStatic) {
	std::vector<Vector3D> vertexList;

	for (int i = 0; i < collisionFacesNum; i += 9) {
		vertexList.push_back(Vector3D(collisionVertex[(collisionFaces[i] - 1) * 3], collisionVertex[(collisionFaces[i] - 1) * 3 + 1], collisionVertex[(collisionFaces[i] - 1) * 3 + 2]));
		vertexList.push_back(Vector3D(collisionVertex[(collisionFaces[i + 3] - 1) * 3], collisionVertex[(collisionFaces[i + 3] - 1) * 3 + 1], collisionVertex[(collisionFaces[i + 3] - 1) * 3 + 2]));
		vertexList.push_back(Vector3D(collisionVertex[(collisionFaces[i + 6] - 1) * 3], collisionVertex[(collisionFaces[i + 6] - 1) * 3 + 1], collisionVertex[(collisionFaces[i + 6] - 1) * 3 + 2]));
	}

	int res = Collision::AddBody(scale, translation, rotation, mass, vertexList, isStatic); 

	vertexList.clear();
	return res;
//	else Collision::AddStaticBody();// addRigidBody
}

Vector3D Model::get_first_vert() {
	return Vector3D(vertex[0], vertex[1], vertex[2]);
}

Vector3D *Model::get_faces_list() {
	int num_of_faces = faces_size / 9;
	Vector3D *list = new Vector3D[num_of_faces];

	for (int i = 0; i < faces_size; i += 9) {
		Vector3D face = (Vector3D(vertex[(faces[i] - 1) * 3], vertex[(faces[i] - 1) * 3 + 1], vertex[(faces[i] - 1) * 3 + 2]) +
			Vector3D(vertex[(faces[i + 3] - 1) * 3], vertex[(faces[i + 3] - 1) * 3 + 1], vertex[(faces[i + 3] - 1) * 3 + 2]) +
			Vector3D(vertex[(faces[i + 6] - 1) * 3], vertex[(faces[i + 6] - 1) * 3 + 1], vertex[(faces[i + 6] - 1) * 3 + 2])) / 3.0;
		list[i / 9] = face;
	}
	return list;
}

Vector3D *Model::get_normal_list() {
	int num_of_normal = faces_size / 3;
	Vector3D *normal = new Vector3D[num_of_normal];
	for (int i = 0; i < faces_size; i += 9) {
		Vector3D n1 = Vector3D(vertex_n[(faces[i + 2] - 1) * 3], vertex_n[(faces[i + 2] - 1) * 3 + 1], vertex_n[(faces[i + 2] - 1) * 3 + 2]);
		Vector3D n2 = Vector3D(vertex_n[(faces[i + 5] - 1) * 3], vertex_n[(faces[i + 5] - 1) * 3 + 1], vertex_n[(faces[i + 5] - 1) * 3 + 2]);
		Vector3D n3 = Vector3D(vertex_n[(faces[i + 8] - 1) * 3], vertex_n[(faces[i + 8] - 1) * 3 + 1], vertex_n[(faces[i + 8] - 1) * 3 + 2]);
		normal[i / 3] = n1;
		normal[i / 3 + 1] = n2;
		normal[i / 3 + 2] = n3;
	}
	return normal;
}

int Model::faces_list_size() {
	return faces_size / 9;
}

int Model::normal_list_size() {
	return faces_size / 3;
}

void Model::CompileLod(int i) {
	lodLists[i] = glGenLists(1);
	glNewList(lodLists[i], GL_COMPILE);
	glEnable(GL_DEPTH_TEST);
	glBegin(GL_TRIANGLES);
	const int *facesX = &lodFaces[i][0];
	const float *vertexX = &lodVertex[i][0];
	const float *vertex_nX = &lodNormal[i][0];
	const float *vertex_tX = &lodTexCoord[i][0];

	for (int j = 0; j < lodFacesSize[i]; j += 9) {
		const int* faces0 = facesX + j;
		const int facesI0 = (*(faces0)-1) * 3;
		const int facesI1 = (*(faces0 + 1) - 1) * 3;
		const int facesI2 = (*(faces0 + 2) - 1) * 3;

		const float* vertex_n0 = vertex_nX + facesI2;
		const float* vertex_t0 = vertex_tX + facesI1;
		const float* vertex0 = vertexX + facesI0;

		glNormal3f(*(vertex_n0), *(vertex_n0 + 1), *(vertex_n0 + 2));
		glTexCoord2d(*(vertex_t0)* texCoordScale.x, *(vertex_t0 + 1) * texCoordScale.y);
		glVertex3f(*(vertex0), *(vertex0 + 1), *(vertex0 + 2));

		const int facesI3 = (*(faces0 + 3) - 1) * 3;
		const int facesI4 = (*(faces0 + 4) - 1) * 3;
		const int facesI5 = (*(faces0 + 5) - 1) * 3;

		const float* vertex_n1 = vertex_nX + facesI5;
		const float* vertex_t1 = vertex_tX + facesI4;
		const float* vertex1 = vertexX + facesI3;

		glNormal3f(*(vertex_n1), *(vertex_n1 + 1), *(vertex_n1 + 2));
		glTexCoord2d(*(vertex_t1)* texCoordScale.x, *(vertex_t1 + 1) * texCoordScale.y);
		glVertex3f(*(vertex1), *(vertex1 + 1), *(vertex1 + 2));

		const int facesI6 = (*(faces0 + 6) - 1) * 3;
		const int facesI7 = (*(faces0 + 7) - 1) * 3;
		const int facesI8 = (*(faces0 + 8) - 1) * 3;

		const float* vertex_n2 = vertex_nX + facesI8;
		const float* vertex_t2 = vertex_tX + facesI7;
		const float* vertex2 = vertexX + facesI6;

		glNormal3f(*(vertex_n2), *(vertex_n2 + 1), *(vertex_n2 + 2));
		glTexCoord2d(*(vertex_t2)* texCoordScale.x, *(vertex_t2 + 1) * texCoordScale.y);
		glVertex3f(*(vertex2), *(vertex2 + 1), *(vertex2 + 2));
	}
	glEnd();
	glDisable(GL_DEPTH_TEST);
	glEndList();
}

void Model::draw(float distance) {
	if (Info::shader && shaders) {
		glUseProgram(shaderProgram);
		setShader();
	}

	if (changeTimeRemaining == 0.0f && state == 0) {
		for (int i = lodNum - 1; i >= 0; i--) {
			if (lodDistance[i] <= distance) {
				if (lodLists[i]) {
					glCallList(lodLists[i]);
				}
				else {
					CompileLod(i);
				}
				return;
			}
		}
	}

	if (list_id && changeTimeRemaining == 0.0f) {
		glCallList(list_id);
	}
	else if (changeTimeRemaining == 0.0f && state == 0){
		if (list_id != 0) {
			glDeleteLists(list_id, 1);
			list_id = 0;
		}
		list_id = glGenLists(1);
		glNewList(list_id, GL_COMPILE);
		glEnable(GL_DEPTH_TEST);
		glBegin(GL_TRIANGLES);
		const int *facesX = &faces[0];
		const float *vertexX = &vertex[0];
		const float *vertex_nX = &vertex_n[0];
		const float *vertex_tX = &vertex_t[0];

		for (int i = 0; i < faces_size; i += 9) {
			const int* faces0 = facesX + i;
			const int facesI0 = (*(faces0) - 1) * 3;
			const int facesI1 = (*(faces0 + 1) - 1) * 3;
			const int facesI2 = (*(faces0 + 2) - 1) * 3;

			const float* vertex_n0 = vertex_nX + facesI2;
			const float* vertex_t0 = vertex_tX + facesI1;
			const float* vertex0 = vertexX + facesI0;

			glNormal3f(*(vertex_n0), *(vertex_n0 + 1), *(vertex_n0 + 2));
			glTexCoord2d(*(vertex_t0) * texCoordScale.x, *(vertex_t0 + 1) * texCoordScale.y);
			glVertex3f(*(vertex0), *(vertex0 + 1), *(vertex0 + 2));

			const int facesI3 = (*(faces0 + 3) - 1) * 3;
			const int facesI4 = (*(faces0 + 4) - 1) * 3;
			const int facesI5 = (*(faces0 + 5) - 1) * 3;

			const float* vertex_n1 = vertex_nX + facesI5;
			const float* vertex_t1 = vertex_tX + facesI4;
			const float* vertex1 = vertexX + facesI3;

			glNormal3f(*(vertex_n1), *(vertex_n1 + 1), *(vertex_n1 + 2));
			glTexCoord2d(*(vertex_t1)* texCoordScale.x, *(vertex_t1 + 1) * texCoordScale.y);
			glVertex3f(*(vertex1), *(vertex1 + 1), *(vertex1 + 2));

			const int facesI6 = (*(faces0 + 6) - 1) * 3;
			const int facesI7 = (*(faces0 + 7) - 1) * 3;
			const int facesI8 = (*(faces0 + 8) - 1) * 3;

			const float* vertex_n2 = vertex_nX + facesI8;
			const float* vertex_t2 = vertex_tX + facesI7;
			const float* vertex2 = vertexX + facesI6;

			glNormal3f(*(vertex_n2), *(vertex_n2 + 1), *(vertex_n2 + 2));
			glTexCoord2d(*(vertex_t2)* texCoordScale.x, *(vertex_t2 + 1) * texCoordScale.y);
			glVertex3f(*(vertex2), *(vertex2 + 1), *(vertex2 + 2));
		}
		glEnd();
		glDisable(GL_DEPTH_TEST);
		glEndList();
	}
	else {
		changeTimeRemaining -= Info::GetElapsedTime();
		if (changeTimeRemaining < 0.0f) changeTimeRemaining = 0.0f;
		float changePart = 1.0 - changeTimeRemaining / changeTimeAll;
		if (list_id != 0) {
			glDeleteLists(list_id, 1);
			list_id = 0;
		}
		list_id = glGenLists(1);
		glNewList(list_id, GL_COMPILE);
		glEnable(GL_DEPTH_TEST);
		glBegin(GL_TRIANGLES);
		int *facesX, *facesXn;
		float *vertexX, *vertexXn;
		float *vertex_nX, *vertex_nXn;
		float *vertex_tX, *vertex_tXn;

		if (prevState == 0) {
			facesX = &faces[0];
			vertexX = &vertex[0];
			vertex_nX = &vertex_n[0];
			vertex_tX = &vertex_t[0];
		}
		else {
			int stateAdd = stateNum[prevState];
			facesX = &stateFaces[stateAdd][0];
			vertexX = &stateVertex[stateAdd][0];
			vertex_nX = &stateNormal[stateAdd][0];
			vertex_tX = &stateTexCoord[stateAdd][0];
		}

		if (state == 0) {
			facesXn = &faces[0];
			vertexXn = &vertex[0];
			vertex_nXn = &vertex_n[0];
			vertex_tXn = &vertex_t[0];
		}
		else {
			int stateAdd = stateNum[state];
			facesXn = &stateFaces[stateAdd][0];
			vertexXn = &stateVertex[stateAdd][0];
			vertex_nXn = &stateNormal[stateAdd][0];
			vertex_tXn = &stateTexCoord[stateAdd][0];
		}

		for (int i = 0; i < faces_size; i += 9) {
			int* faces0 = facesX + i; int* faces0n = facesXn + i;
			int facesI0 = (*(faces0)-1) * 3; int facesI0n = (*(faces0n)-1) * 3;
			int facesI1 = (*(faces0 + 1) - 1) * 3; int facesI1n = (*(faces0n + 1) - 1) * 3;
			int facesI2 = (*(faces0 + 2) - 1) * 3; int facesI2n = (*(faces0n + 2) - 1) * 3;

			float* vertex_n0 = vertex_nX + facesI2; float* vertex_n0n = vertex_nXn + facesI2n;
			float* vertex_t0 = vertex_tX + facesI1; float* vertex_t0n = vertex_tXn + facesI1n;
			float* vertex0 = vertexX + facesI0; float* vertex0n = vertexXn + facesI0n;

			Vector3D n3f;
			n3f.x = (*(vertex_n0)) * (1.0 - changePart) + (*(vertex_n0n)) * changePart;
			n3f.y = (*(vertex_n0 + 1)) * (1.0 - changePart) + (*(vertex_n0n + 1)) * changePart;
			n3f.z = (*(vertex_n0 + 2)) * (1.0 - changePart) + (*(vertex_n0n + 2))  * changePart;
			Vector2D t2d;
			t2d.x = ((*(vertex_t0)) * (1.0 - changePart) + (*(vertex_t0n)) * changePart) * texCoordScale.x;
			t2d.y = ((*(vertex_t0 + 1)) * (1.0 - changePart) + (*(vertex_t0n + 1)) * changePart) * texCoordScale.y;
			Vector3D v3f;
			v3f.x = (*(vertex0)) * (1.0 - changePart) + (*(vertex0n)) * changePart;
			v3f.y = (*(vertex0 + 1)) * (1.0 - changePart) + (*(vertex0n + 1)) * changePart;
			v3f.z = (*(vertex0 + 2)) * (1.0 - changePart) + (*(vertex0n + 2)) * changePart;

			glNormal3f(n3f.x, n3f.y, n3f.z);
			glTexCoord2d(t2d.x, t2d.y);
			glVertex3f(v3f.x, v3f.y, v3f.z);

			///

			int facesI3 = (*(faces0 + 3) - 1) * 3; int facesI3n = (*(faces0n + 3) - 1) * 3;
			int facesI4 = (*(faces0 + 4) - 1) * 3; int facesI4n = (*(faces0n + 4) - 1) * 3;
			int facesI5 = (*(faces0 + 5) - 1) * 3; int facesI5n = (*(faces0n + 5) - 1) * 3;

			float* vertex_n1 = vertex_nX + facesI5; float* vertex_n1n = vertex_nXn + facesI5n;
			float* vertex_t1 = vertex_tX + facesI4; float* vertex_t1n = vertex_tXn + facesI4n;
			float* vertex1 = vertexX + facesI3; float* vertex1n = vertexXn + facesI3n;

			n3f.x = (*(vertex_n1)) * (1.0 - changePart) + (*(vertex_n1n)) * changePart;
			n3f.y = (*(vertex_n1 + 1)) * (1.0 - changePart) + (*(vertex_n1n + 1)) * changePart;
			n3f.z = (*(vertex_n1 + 2)) * (1.0 - changePart) + (*(vertex_n1n + 2))  * changePart;

			t2d.x = ((*(vertex_t1)) * (1.0 - changePart) + (*(vertex_t1n)) * changePart) * texCoordScale.x;
			t2d.y = ((*(vertex_t1 + 1)) * (1.0 - changePart) + (*(vertex_t1n + 1)) * changePart) * texCoordScale.y;
	
			v3f.x = (*(vertex1)) * (1.0 - changePart) + (*(vertex1n)) * changePart;
			v3f.y = (*(vertex1 + 1)) * (1.0 - changePart) + (*(vertex1n + 1)) * changePart;
			v3f.z = (*(vertex1 + 2)) * (1.0 - changePart) + (*(vertex1n + 2)) * changePart;

			glNormal3f(n3f.x, n3f.y, n3f.z);
			glTexCoord2d(t2d.x, t2d.y);
			glVertex3f(v3f.x, v3f.y, v3f.z);
			///

			int facesI6 = (*(faces0 + 6) - 1) * 3; int facesI6n = (*(faces0n + 6) - 1) * 3;
			int facesI7 = (*(faces0 + 7) - 1) * 3; int facesI7n = (*(faces0n + 7) - 1) * 3;
			int facesI8 = (*(faces0 + 8) - 1) * 3; int facesI8n = (*(faces0n + 8) - 1) * 3;

			float* vertex_n2 = vertex_nX + facesI8; float* vertex_n2n = vertex_nXn + facesI8n;
			float* vertex_t2 = vertex_tX + facesI7; float* vertex_t2n = vertex_tXn + facesI7n;
			float* vertex2 = vertexX + facesI6; float* vertex2n= vertexXn + facesI6n;

			n3f.x = (*(vertex_n2)) * (1.0 - changePart) + (*(vertex_n2n)) * changePart;
			n3f.y = (*(vertex_n2 + 1)) * (1.0 - changePart) + (*(vertex_n2n + 1)) * changePart;
			n3f.z = (*(vertex_n2 + 2)) * (1.0 - changePart) + (*(vertex_n2n + 2))  * changePart;

			t2d.x = ((*(vertex_t2)) * (1.0 - changePart) + (*(vertex_t2n)) * changePart) * texCoordScale.x;
			t2d.y = ((*(vertex_t2 + 1)) * (1.0 - changePart) + (*(vertex_t2n + 1)) * changePart) * texCoordScale.y;

			v3f.x = (*(vertex2)) * (1.0 - changePart) + (*(vertex2n)) * changePart;
			v3f.y = (*(vertex2 + 1)) * (1.0 - changePart) + (*(vertex2n + 1)) * changePart;
			v3f.z = (*(vertex2 + 2)) * (1.0 - changePart) + (*(vertex2n + 2)) * changePart;

			glNormal3f(n3f.x, n3f.y, n3f.z);
			glTexCoord2d(t2d.x, t2d.y);
			glVertex3f(v3f.x, v3f.y, v3f.z);
		}
		glEnd();
		glDisable(GL_DEPTH_TEST);
		glEndList();
		glCallList(list_id);
	}

	if (Info::shader && shaders) glUseProgram(0);

}

