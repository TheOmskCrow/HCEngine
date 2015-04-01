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
	lodNum = 0;
	collisionFacesNum = 0;
	mass = 0.0f;
	mdl_scale = 0.0;
	_mdl_x = 0;
	_mdl_y = 0;
	_mdl_z = pi / 2.0;
	collision = true;
	shaders = true;
	texCoordScale = 1.0;
	height_as_spec = false;
}


Model::~Model(void)
{
}
void Model::setScaleTexCoord(float scale) {
	texCoordScale = scale;
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
		glTexCoord2d(*(vertex_t0)* texCoordScale, *(vertex_t0 + 1) * texCoordScale);
		glVertex3f(*(vertex0), *(vertex0 + 1), *(vertex0 + 2));

		const int facesI3 = (*(faces0 + 3) - 1) * 3;
		const int facesI4 = (*(faces0 + 4) - 1) * 3;
		const int facesI5 = (*(faces0 + 5) - 1) * 3;

		const float* vertex_n1 = vertex_nX + facesI5;
		const float* vertex_t1 = vertex_tX + facesI4;
		const float* vertex1 = vertexX + facesI3;

		glNormal3f(*(vertex_n1), *(vertex_n1 + 1), *(vertex_n1 + 2));
		glTexCoord2d(*(vertex_t1)* texCoordScale, *(vertex_t1 + 1) * texCoordScale);
		glVertex3f(*(vertex1), *(vertex1 + 1), *(vertex1 + 2));

		const int facesI6 = (*(faces0 + 6) - 1) * 3;
		const int facesI7 = (*(faces0 + 7) - 1) * 3;
		const int facesI8 = (*(faces0 + 8) - 1) * 3;

		const float* vertex_n2 = vertex_nX + facesI8;
		const float* vertex_t2 = vertex_tX + facesI7;
		const float* vertex2 = vertexX + facesI6;

		glNormal3f(*(vertex_n2), *(vertex_n2 + 1), *(vertex_n2 + 2));
		glTexCoord2d(*(vertex_t2)* texCoordScale, *(vertex_t2 + 1) * texCoordScale);
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

	if (list_id){
		glCallList(list_id);
	}
	else {
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
			glTexCoord2d(*(vertex_t0) * texCoordScale, *(vertex_t0 + 1) * texCoordScale);
			glVertex3f(*(vertex0), *(vertex0 + 1), *(vertex0 + 2));

			const int facesI3 = (*(faces0 + 3) - 1) * 3;
			const int facesI4 = (*(faces0 + 4) - 1) * 3;
			const int facesI5 = (*(faces0 + 5) - 1) * 3;

			const float* vertex_n1 = vertex_nX + facesI5;
			const float* vertex_t1 = vertex_tX + facesI4;
			const float* vertex1 = vertexX + facesI3;

			glNormal3f(*(vertex_n1), *(vertex_n1 + 1), *(vertex_n1 + 2));
			glTexCoord2d(*(vertex_t1)* texCoordScale, *(vertex_t1 + 1) * texCoordScale);
			glVertex3f(*(vertex1), *(vertex1 + 1), *(vertex1 + 2));

			const int facesI6 = (*(faces0 + 6) - 1) * 3;
			const int facesI7 = (*(faces0 + 7) - 1) * 3;
			const int facesI8 = (*(faces0 + 8) - 1) * 3;

			const float* vertex_n2 = vertex_nX + facesI8;
			const float* vertex_t2 = vertex_tX + facesI7;
			const float* vertex2 = vertexX + facesI6;

			glNormal3f(*(vertex_n2), *(vertex_n2 + 1), *(vertex_n2 + 2));
			glTexCoord2d(*(vertex_t2)* texCoordScale, *(vertex_t2 + 1) * texCoordScale);
			glVertex3f(*(vertex2), *(vertex2 + 1), *(vertex2 + 2));
		}
		glEnd();
		glDisable(GL_DEPTH_TEST);
		glEndList();
	}


	if (Info::shader && shaders) glUseProgram(0);

}

