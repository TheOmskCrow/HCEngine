#include "StdAfx.h"
#include "Grass.h"
#include <fstream>
#include <iostream>
#include <string>
#include "hc3dObjects.h"
#include "hc3dMath.h"
#include "../Water/Water.h"

using namespace hc3d;

float Grass::windWave = 0.0;
Vector3D Grass::windCenter = Vector3D(0, 0, 0);
Grass::Grass(void)
{
}


Grass::~Grass(void)
{
}

inline float Grass::distance(const Vector3D &a, const Vector3D &b) {
	const Vector3D c = a - b;
	return sqrt(c*c);
}
void Grass::qSort(vector<Vector3D> &A, vector<int> &B, int low, int high) {
	int i = low;
	int j = high;
	float x = distance(A[(low + high) / 2], pos);
	do {
		while (distance(A[i], pos) > x) ++i;
		while (distance(A[j], pos) < x) --j;
		if (i <= j){
			swap(A[i], A[j]);
			swap(B[i], B[j]);
			i++; j--;
		}
	} while (i < j);
	if (low < j)  qSort(A, B, low, j);
	if (i < high) qSort(A, B, i, high);
}

void Grass::Init(){
	model = new Model[8];
	for (int i = 0; i < 8; i++) {
		model[i].no_shaders();
	}
	model[0].init("Models/grass/grass4.obj", "null", "null", "null");
	model[3].init("Models/fl2.obj", "null", "null", "null");

	model[4].init("Models/bushes/autumn.obj", "null", "null", "null"/*,0.25*/);
	model[5].init("Models/bushes/autumn1.obj", "null", "null", "null"/*,0.25*/);
	model[6].init("Models/bushes/autumn2.obj", "null", "null", "null"/*,0.25*/);
	model[7].init("Models/fl3.obj", "null", "null", "null");
	//model[7].init("Models/fern1.obj", "null", "null", "null");
	grass_num = 101000;
	offset = 0.0;
	terNormal = new Vector3D[grass_num];
	borders = new Vector3D*[grass_num];
	for (int i = 0; i < grass_num; i++) borders[i] = new Vector3D[4];
	grass = new Vector3D[grass_num];
	angle = new float[grass_num];
	type = new int[grass_num];
	cadr = new float[grass_num];
	gr_size = new float[grass_num];
	grass_init();
	glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
	Info::RaiseCurTextureNum();
	tex = LoadTexture("Texture/grass4.png");
	glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
	Info::RaiseCurTextureNum();
	tex1 = LoadTexture("Texture/fl2.png");
	glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
	Info::RaiseCurTextureNum();
	tex2 = LoadTexture("Texture/autumn_bush.png");
	glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
	Info::RaiseCurTextureNum();
	tex3 = LoadTexture("Texture/autumn_bush1.png");
	glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
	Info::RaiseCurTextureNum();
	tex4 = LoadTexture("Texture/autumn_bush2.png");
	glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
	Info::RaiseCurTextureNum();
	tex5 = LoadTexture("Texture/fl1.png");
//	tex5 = LoadTexture("Texture/plants/fern1.png");
	glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
	Info::RaiseCurTextureNum();
	tex6 = LoadTexture("Texture/grass4.png");
	shaderprogram = LoadShader("Shaders/grass.vert", "Shaders/grass.frag");
}
float rnd() {
	return float(rand() % 100000) / 100000.0;
}
void Grass::grass_init() {
	for (int i = 0; i < grass_num; i++) {
		while (1) {
			Vector3D data;
			data.x = float(rand()) * float(rand() % 10 + 1);
			data.y = float(rand()) * float(rand() % 10 + 1);
			data.z = CalcTerHeight(Vector3D(data.x, data.y, 0.0));
			Vector3D ter_normal = CalcTerNormal(Vector3D(data.x, data.y, 0.0));
			ter_normal /= ter_normal*ter_normal;

			if (ter_normal * Vector3D(0.0, 0.0, 1.0) < 0.992) continue;
			terNormal[i] = ter_normal;

			//std::cout << ter_normal*Vector3D(0.0, 0.0, 1.0) << std::endl;
			float grass_coeff = hc3d::Math::hc3dMax(0.0, hc3d::Math::hc3dMin(1.0, (ter_normal*Vector3D(0.0, 0.0, 1.0))))*100.0;
			grass_coeff -= 0.975;
			grass_coeff *= 40.0;
			grass[i] = data;
			int r = rand() % 1600;
			type[i] = 0;
			//	if (distance(Vector3D(1200, 2000, 0), grass[i]) < 300) {
			if (r > 60 && r < 80) type[i] = 5;
			//	}
			//if (distance(Vector3D(1000, 1800, 0), grass[i]) < 300) {
			else if (r > 80 && r < 95) type[i] = 1;
			//	}
			//if(r > 82 && r < 95) type[i] = 1;			
			//if (distance(Vector3D(2800, 750, 0), grass[i]) < 400 || distance(Vector3D(1800, 630, 0), grass[i]) < 300 || distance(Vector3D(1100, 980, 0), grass[i]) < 200 || distance(Vector3D(500, 1700, 0), grass[i]) < 500) {
				else if (r == 95)type[i] = 2;
				else if (r == 96)type[i] = 3;
				else if (r == 97)type[i] = 4;
			//}
			if (type[i] != 0) grass[i].z--;
			angle[i] = rnd()*360.0;
			cadr[i] = (data.x + data.y) / 100.0;
			gr_size[i] = rnd()*2.0 + 1.0;
			if (data.z < 150.0 && data.z > 3.0) {
				//printf("grass_coeff = %.2f\n",grass_coeff);
				int number = 0;
				for (int p = data.x - 5; p <= data.x + 5; p += 10) {
					for (int k = data.y - 5; k <= data.y + 5; k += 10) {
						borders[i][number].z = CalcTerHeight(Vector3D(p, k, 0.0));
						borders[i][number].x = p;
						borders[i][number].y = k;
						number++;
					}
				}
				break;
			}
		}
	}
	/*
	for (int i = grass_num - 1000; i < grass_num - 500; i++) {
	while (1) {
	Vector3D data;
	data.x = (((rand() % 3000)  + 9000 )/ 10.0);
	data.y = (((rand() % 3000) + 17000 )/ 10.0);
	data.z = Terrain::calc_height(Vector3D(data.x, data.y, 0.0));
	if (distance(Vector3D(1050, 1850, 0), data) > 150) continue;
	Vector3D ter_normal = Terrain::calc_normal(Vector3D(data.x, data.y, 0.0));
	ter_normal /= ter_normal*ter_normal;

	if (ter_normal * Vector3D(0.0, 0.0, 1.0) < 0.992) continue;
	terNormal[i] = ter_normal;

	//std::cout << ter_normal*Vector3D(0.0, 0.0, 1.0) << std::endl;
	float grass_coeff = max(0.0, min(1.0, (ter_normal*Vector3D(0.0, 0.0, 1.0))))*100.0;
	grass_coeff -= 0.975;
	grass_coeff *= 40.0;
	grass[i] = data;
	//	if (distance(Vector3D(1200, 2000, 0), grass[i]) < 300) {
	type[i] = 1;
	//	}
	//if (distance(Vector3D(1000, 1800, 0), grass[i]) < 300) {
	grass[i].z--;
	angle[i] = rnd()*360.0;
	cadr[i] = (data.x + data.y) / 100.0;
	gr_size[i] = rnd()*2.0 + 1.0;
	if (data.z < 150.0 && data.z > 3.0) {
	//printf("grass_coeff = %.2f\n",grass_coeff);
	break;
	}
	}
	}
	for (int i = grass_num - 500; i < grass_num; i++) {
	while (1) {
	Vector3D data;
	data.x = (((rand() % 3000) + 8500) / 10.0);
	data.y = (((rand() % 3000) + 16500) / 10.0);
	data.z = Terrain::calc_height(Vector3D(data.x, data.y, 0.0));
	if (distance(Vector3D(1000, 1800, 0), data) > 150) continue;
	Vector3D ter_normal = Terrain::calc_normal(Vector3D(data.x, data.y, 0.0));
	ter_normal /= ter_normal*ter_normal;

	if (ter_normal * Vector3D(0.0, 0.0, 1.0) < 0.992) continue;
	terNormal[i] = ter_normal;

	//std::cout << ter_normal*Vector3D(0.0, 0.0, 1.0) << std::endl;
	float grass_coeff = max(0.0, min(1.0, (ter_normal*Vector3D(0.0, 0.0, 1.0))))*100.0;
	grass_coeff -= 0.975;
	grass_coeff *= 40.0;
	grass[i] = data;
	//	if (distance(Vector3D(1200, 2000, 0), grass[i]) < 300) {
	type[i] = 5;
	//	}
	//if (distance(Vector3D(1000, 1800, 0), grass[i]) < 300) {
	grass[i].z--;
	angle[i] = rnd()*360.0;
	cadr[i] = (data.x + data.y) / 100.0;
	gr_size[i] = rnd()*2.0 + 1.0;
	if (data.z < 150.0 && data.z > 3.0) {
	//printf("grass_coeff = %.2f\n",grass_coeff);
	break;
	}
	}
	}*/
}
void Grass::setShader() {
	//Shadow shadow;
	glUniform1i(glGetUniformLocation(
		shaderprogram, "shadowing"), Info::GetShadow());
	glUniform1f(glGetUniformLocation(
		shaderprogram, "distanse"), windWave);
	glUniform1f(glGetUniformLocation(
		shaderprogram, "dstbl"), 2.0f);
	glUniform1f(glGetUniformLocation(
		shaderprogram, "offset"), offset);
	glUniform1f(glGetUniformLocation(
		shaderprogram, "shadowDist"), Info::GetShadowDist());
	glUniform3f(glGetUniformLocation(
		shaderprogram, "atmosphere"), Info::GetAtmoColor().x, Info::GetAtmoColor().y, Info::GetAtmoColor().z);
	glUniform1f(glGetUniformLocation(
		shaderprogram, "Width"), Info::width());
	glUniform1f(glGetUniformLocation(
		shaderprogram, "Height"), Info::height());
	Vector3D sun = 0.5*(Info::GetSun());
	glUniform3f(glGetUniformLocation(
		shaderprogram, "lightPos"), sun.x, sun.y, sun.z);
	glUniform3f(glGetUniformLocation(
		shaderprogram, "windCenter"), windCenter.x, windCenter.y, windCenter.z);
	Vector3D player = Camera::getPosition();
	glUniform3f(glGetUniformLocation(
		shaderprogram, "eyePos"), player.x, player.y, player.z);
	glUniform1i(glGetUniformLocation(
		shaderprogram, "flash"), Info::GetFlash());
	glUniform3f(glGetUniformLocation(
		shaderprogram, "eyeNorm"), Info::GetEyeNormal().x, Info::GetEyeNormal().y, Info::GetEyeNormal().z);

	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, tex);
	glActiveTexture(GL_TEXTURE11);
	glBindTexture(GL_TEXTURE_2D, tex1);
	glActiveTexture(GL_TEXTURE12);
	glBindTexture(GL_TEXTURE_2D, tex2);
	glActiveTexture(GL_TEXTURE13);
	glBindTexture(GL_TEXTURE_2D, tex3);
	glActiveTexture(GL_TEXTURE14);
	glBindTexture(GL_TEXTURE_2D, tex4);
	glActiveTexture(GL_TEXTURE15);
	glBindTexture(GL_TEXTURE_2D, tex5);
	glActiveTexture(GL_TEXTURE16);
	glBindTexture(GL_TEXTURE_2D, tex6);
}
void Grass::Draw() {
	//        grass = qSort(grass,0,grass_num-1);
	vector<Vector3D> newgrass;
	vector<int> number;
	float newgrass_size = 0;
	float yaw = 0.0;
	if (!Info::GetShader() || Info::GetRefract() || Info::GetReflect()) yaw = 3.0;
	if (Info::GetShader()) {
		glUseProgram(shaderprogram);
		setShader();
	}
	const GLint u_model0 = glGetUniformLocation(shaderprogram, "model");
	const GLint u_ter0 = glGetUniformLocation(shaderprogram, "terNormal");
	const GLint u_type0 = glGetUniformLocation(shaderprogram, "type");

	bool tmp = Info::GetShader();
	Info::SetShader(false);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//  glDepthMask(false);
	if (yaw == 0) offset += (rnd() / 5.0 + 0.05)*Info::GetElapsedTime();
	float pi = 3.141592;
	const Vector3D player = Camera::getPosition();
	int samples = 0;
	float *distVec = new float[grass_num];
	const float* dist0 = &distVec[0];
	Vector3D b = Info::GetEyeNormal();
	b.Normalize();
	Vector3D *grass0 = &grass[0];

	newgrass.reserve(grass_num * 0.25);
	number.reserve(grass_num * 0.25);

	for (int i = 0; i < grass_num; i++) {
		const float dst = distance(*(grass0 + i), player);
		distVec[i] = dst;
		if (dst > 500.0) continue;
		if (yaw == 1.0 && dst > 40.0) continue;
		else if (yaw == 2.0 && dst > 100.0) continue;
		else if (yaw == 3.0 && dst > 200.0) continue;

		Vector3D a = *(grass0 + i) - player;
		a.Normalize();

		float norm_vec = a*b;
		if (yaw == 0.0) if (norm_vec < 0.7 && dst > 30.0) continue;
		newgrass.push_back(*(grass0 + i));
		number.push_back(i);
		newgrass_size++;
	}
	//if(newgrass_size > 5) qSort(newgrass,number,0,newgrass_size-1);
	const int *number0 = &number[0];
	const int *type0 = &type[0];
	const float *angle0 = &angle[0];
	for (int i = 0; i < newgrass_size; i++) {
		const int numberI = *(number0 + i);
		const float dst = *(dist0 + numberI);
		if (dst > 500) continue;
		glPushMatrix();
		glTranslated(newgrass[i].x, newgrass[i].y, newgrass[i].z - 0.3);
		glRotated(*(angle0 + numberI), 0, 0, 1);
		if (*(type0 + numberI) == 0) {

			if (yaw == 0) {
				if ((*(angle0 + numberI)) > 50.0) {
					glScaled(1.0, 1.0, 1.3);
					glUniform1i(u_model0, 16);
				}
				else {
					glUniform1i(u_model0, 10);
				}
				glUniform1i(u_type0, 0);
				glUniform3f(u_ter0, terNormal[numberI].x, terNormal[numberI].y, terNormal[numberI].z);
				glUniform3f(glGetUniformLocation(
					shaderprogram, "border1"), borders[numberI][0].x, borders[numberI][0].y, borders[numberI][0].z);
				glUniform3f(glGetUniformLocation(
					shaderprogram, "border2"), borders[numberI][1].x, borders[numberI][1].y, borders[numberI][1].z);
				glUniform3f(glGetUniformLocation(
					shaderprogram, "border3"), borders[numberI][2].x, borders[numberI][2].y, borders[numberI][2].z);
				glUniform3f(glGetUniformLocation(
					shaderprogram, "border4"), borders[numberI][3].x, borders[numberI][3].y, borders[numberI][3].z);
				glUniform3f(glGetUniformLocation(
					shaderprogram, "center"), newgrass[i].x, newgrass[i].y, newgrass[i].z);
				//std::cout << borders[numberI][0].x << " " << borders[numberI][0].y << " " << borders[numberI][0].z << " " << newgrass[i].x << " " << newgrass[i].y << " " << newgrass[i].z << std::endl;


				//if(dst < 30) {
				model[0].draw();
				samples += 480;

				if ((*(angle0 + numberI)) > 50.0) glScaled(1.0, 1.0, 1.0 / 1.3);
			}
		}
		else if (type[numberI] == 1){
			glUniform1i(u_model0, 11);
			glUniform1i(u_type0, 1);
			if (dst < 500) {
				glTranslated(0, 0, 0.3);
				model[3].draw();
				glTranslated(0, 0, -0.3);
			}
			samples += 2936;
		}
		else if (type[numberI] == 2){
			glUniform1i(u_model0, 12);
			glUniform1i(u_type0, 1);
			glTranslated(0, 0, 0.3);
				model[4].draw();
			glTranslated(0, 0, -0.3);
			samples += 2936;
		}
		else if (type[numberI] == 3){
			glUniform1i(u_model0, 13);
			glUniform1i(u_type0, 1);
			glTranslated(0, 0, 0.3);
				model[5].draw();
			glTranslated(0, 0, -0.3);
			samples += 2936;
		}
		else if (type[numberI] == 4){
			glUniform1i(u_model0, 14);
			glUniform1i(u_type0, 1);
			glTranslated(0, 0, 0.3);
				model[6].draw();
			glTranslated(0, 0, -0.3);
			samples += 2936;
		}
		else if (type[numberI] == 5){
			glUniform1i(u_model0, 15);
			glUniform1i(u_type0, 1);
			if (dst < 500) {
				glTranslated(0, 0, 0.3);
				model[7].draw();
				glTranslated(0, 0, -0.3);
			}
			samples += 2936;
		}
		glRotated(-*(angle0 + numberI), 0, 0, 1);
		glTranslated(-newgrass[i].x, -newgrass[i].y, -newgrass[i].z + 0.3);
		glPopMatrix();
	}
	//	printf("grass triangles drawing: %d\n",samples);
	//   glDepthMask(true);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	newgrass.clear();
	number.clear();
	Info::SetShader(tmp);
	//		if(Info::shader)  
	glUseProgram(0);
	if (yaw == 0) windWave += 5.0 * Info::GetElapsedTime();
	delete[] distVec;
}
