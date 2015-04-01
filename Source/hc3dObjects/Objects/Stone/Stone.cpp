#include "StdAfx.h"
#include "Stone.h"
#include "../Terrain/Terrain.h"
#include "../Water/Water.h"
#include <fstream>
#include <iostream>
#include <string>

using namespace hc3d;

Stone::Stone(void)
{
}


Stone::~Stone(void)
{
}

float Stone::distance(Vector3D a, Vector3D b) {
    Vector3D c;
    c.x = (a.x  - b.x );
    c.y = (a.y - b.y);
    c.z = (a.z - b.z);
    return (float) sqrt((c.x *c.x  + c.y*c.y + c.z*c.z));
}
Vector3D* Stone::qSort(Vector3D *A, int low, int high) {
          int i = low;
          int j = high;
          float x = distance(A[(low+high)/2],pos);
          do {
              while(distance(A[i],pos) > x) ++i;
              while(distance(A[j],pos) < x) --j;
              if(i <= j){
                  Vector3D temp;
                  temp = A[i];
                  A[i] = A[j];
                  A[j] = temp;
                  i++; j--;
              }
          } while(i < j);
          if(low < j) A = qSort(A, low, j);
          if(i < high) A = qSort(A, i, high);
          return A;
}

void Stone::Init(){
	modelNum = 0;
	FILE *fp;
	fopen_s(&fp, "stones.mdls", "r");
	if (fp == 0) {
		MessageBoxA(0, "Unable to open file stones.mdls", "core", MB_OK);
		exit(0);
	}

	fscanf(fp, "%d\n", &modelNum);
	if (modelNum < 1) {
		MessageBoxA(0, "Invalid model number in stones.mdls", "core", MB_OK);
		exit(0);
	}

	model = new Model[modelNum];

	for (int i = 0; i < modelNum; i++) {
		char modelName[256], textureName[256], bumpName[256], heightName[256], hitBoxName[256], lodName1[256], lodName2[256];
		float lodDist1, lodDist2;
		fscanf(fp, "%s %s %s %s %s %s %f %s %f\n", modelName, textureName, bumpName, heightName, hitBoxName, lodName1, &lodDist1, lodName2, &lodDist2);
		model[i].init(modelName, textureName, bumpName, heightName);
		if (strcmp(hitBoxName, "null")) {
			model[i].AddHitBoxModel(hitBoxName);
		}
		if (strcmp(lodName1, "null")) {
			std::vector<std::string> stoneNames;
			std::vector<float> stoneDist;

			stoneNames.push_back(lodName1);
			stoneDist.push_back(lodDist1);

			if (strcmp(lodName2, "null")) {
				stoneNames.push_back(lodName2);
				stoneDist.push_back(lodDist2);
			}
			model[i].AddLodModels(stoneNames, stoneDist);
			stoneNames.clear();
			stoneDist.clear();
		}
	}


	//model[0].init("Models/rocks/rock1.obj","null","null","null",0.02);
	//model[1].init("Models/rocks/rock2.obj", "null", "null", "null", 0.01);
	//model[2].init("Models/rocks/rock5.obj", "null", "null", "null", 0.01);
	//model[3].init("Models/rocks/rock5.obj", "null", "null", "null", 0.01);
	//model[4].init("Models/rocks/rock5.obj", "null", "null", "null", 0.02);

	//model[0].init("Models/stones/stone1.obj", "Texture/rock7.png", "Texture/rock7.png", "Texture/rock7.png");
	//model[0].AddHitBoxModel("Models/stones/stone1.obj");
	//model[0].AddModelMass(10.0f);
	//	model[0][1].init("Models/box.obj", "Texture/rock4.png", "Texture/rock4.png", "Texture/rock4.png"/*, 0.02*/);
	//	model[0][2].init("Models/box.obj", "Texture/rock4.png", "Texture/rock4.png", "Texture/rock4.png"/*, 0.02*/);

	//std::vector<std::string> stoneNames;
	//std::vector<float> stoneDist;
	//stoneDist.push_back(550);
	//	stoneNames.push_back("Models/tree2/tree3.obj");
	//model[1].init("Models/stones/stone2.obj", "Texture/rock6.png", "null", "null");
	//	model[1][1].init("Models/stones/stone1_1.obj", "Texture/rock6.png", "null", "null");
	//	model[1][2].init("Models/stones/stone1_1.obj", "Texture/rock6.png", "null", "null");
	//	model[6].init("Models/stones/stone2.obj", "null", "null", "null", 0.01);
	//	model[7].init("Models/stones/stone1_1.obj", "null", "null", "null", 0.01);
	//	model[8].init("Models/stones/stone2_1.obj", "null", "null", "null", 0.01);


	Stone_init();
}
float Stone::rnd() {
    return float(rand()%100000)/100000.0;
}
void Stone::Stone_init() {
	FILE *fp;
	fopen_s(&fp, "stones.plcs", "r");
	if (fp == 0) {
		MessageBoxA(0, "Unable to open file stones.plcs", "core", MB_OK);
		exit(0);
	}

	fscanf(fp, "%d\n", &Stone_num);

	offset = 0.0;
	stone = new Vector3D[Stone_num];
	type = new int[Stone_num];
	angle = new Vector3D[Stone_num];
	cadr = new float[Stone_num];
	gr_size = new Vector3D[Stone_num];
	matrix = new int[Stone_num];
	for (int i = 0; i < Stone_num; i++) {
		float x, y, z, rotX, rotY, rotZ, scX, scY, scZ;
		int modelNum;
		fscanf(fp, "%d %f %f %f %f %f %f %f %f %f\n", &modelNum, &x, &y, &z, &rotX, &rotY, &rotZ, &scX, &scY, &scZ);
		type[i] = modelNum;
		stone[i] = Vector3D(x, y, z);
		angle[i] = Vector3D(rotX, rotY, rotZ);
		gr_size[i] = Vector3D(scX, scY, scZ);
		//model[1].addCollision();
		matrix[i] = model[0].addCollision(stone[i], angle[i], gr_size[i], true);
		//Collision::AddRigidBox(gr_size[i] / 2.0,stone[i], 1);
	//	model[modelNum][2].addCollision(stone[i], angle[i], gr_size[i]);
	}
}

void Stone::Draw() {
	float yaw = 0;
	if (!Info::GetShader() || Terrain::refract || Terrain::reflect) yaw = 3.0;
	if (yaw == 0 && !Terrain::reflect && !Terrain::refract) offset += (rnd() / 5.0 + 0.05)*Info::GetElapsedTime();
	float pi = 3.141592;
	Vector3D player = Camera::getPosition();
	int samples = 0;

	for (int i = 0; i < Stone_num; i++) {
		if (Terrain::refract) {
			if (stone[i].z > 1.0) continue;
		}
		float dst = distance(stone[i], player);
		//if (yaw == 0.0 && dst > 2500.0) continue;
		//else if (yaw == 3.0 && dst > 500.0) continue;
		Vector3D a = stone[i] - Camera::getPosition();
		a.Normalize();
		Vector3D b = Info::GetEyeNormal();
		b.Normalize();
		float norm_vec = a*b;
		//if (yaw == 0) if (norm_vec < 0.5 && dst > 60.0) continue;

		float size = 1.5;
		float offset = 0;
		if (size >= 1) offset = 0.5;
		glPushMatrix();
		btScalar *m = Collision::GetObjectMatrix(matrix[i]);
		glMultMatrixf(m);
		delete[] m;
		glScaled(gr_size[i].x, gr_size[i].y, gr_size[i].z);
	//	glTranslated(stone[i].x, stone[i].y, stone[i].z);
	//	glRotatef(angle[i].x, 1, 0, 0);
	//	glRotatef(angle[i].y, 0, 1, 0);
	//	glRotatef(angle[i].z, 0, 0, 1);
	//	glScaled(gr_size[i].x, gr_size[i].y, gr_size[i].z);
		
		samples++;
		//if (dst < 500) {
		//		if (dst < 150) model[type[i]][0].draw();
		//		else model[type[i]][1].draw();
	//	}
		//else
			model[type[i]].draw();

		glPopMatrix();
	}
}
