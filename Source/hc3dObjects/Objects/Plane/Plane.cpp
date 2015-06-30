#include "StdAfx.h"
#include "Plane.h"
#include "../Terrain/Terrain.h"
#include "../Water/Water.h"
#include "hc3dObjects.h"
#include <fstream>
#include <iostream>
#include <string>

using namespace hc3d;

Plane::Plane(void)
{
	state = 0;
}


Plane::~Plane(void)
{
}

float Plane::distance(Vector3D a, Vector3D b) {
	Vector3D c;
	c.x = (a.x - b.x);
	c.y = (a.y - b.y);
	c.z = (a.z - b.z);
	return (float)sqrt((c.x *c.x + c.y*c.y + c.z*c.z));
}

void Plane::Init(){
	model = new Model[10];
	model[0].setScaleTexCoord(1,-1.0);
	model[1].setScaleTexCoord(1,-1.0);
	model[2].setScaleTexCoord(1,-1.0);
	model[3].setScaleTexCoord(1,-1.0);
	model[4].setScaleTexCoord(1,-1.0);

	model[0].init("Models/plane/plane.obj", "Texture/plane/body.png", "Texture/plane/body_normal.png", "null");
	model[1].init("Models/plane/aileron.obj", "Texture/plane/body.png", "Texture/plane/body_normal.png", "null");
	model[2].init("Models/plane/rudder.obj", "Texture/plane/body.png", "Texture/plane/body_normal.png", "null");
	model[3].init("Models/plane/glass.obj", "Texture/plane/glass.png", "Texture/plane/body_normal.png", "null");
	model[4].init("Models/plane/tail.obj", "Texture/plane/body.png", "Texture/plane/body_normal.png", "null");
	model[4].AddState("Models/plane/tail_down.obj", 1);
	model[4].AddState("Models/plane/tail_up.obj", 2);

	model[0].AddHitBoxModel("Models/box.obj");
	model[0].AddModelMass(1000.0f);
	matrix = model[0].addCollision(Vector3D(1000, 1000, 300.0), Vector3D(0, 0, 0), Vector3D(1.5, 1.5, 1.5), false);

	glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
	Info::RaiseCurTextureNum();
	tex = LoadTexture("Texture/checkPoint.png");
	Info::RaiseCurTextureNum();
	
}
float Plane::rnd() {
	return float(rand() % 100000) / 100000.0;
}

void Plane::setShader() {
	glActiveTexture(GL_TEXTURE0 + Info::GetCurBindTexture());
	glBindTexture(GL_TEXTURE_2D, tex);
	glUniform1i(glGetUniformLocation(
		shaderprogram, "tex"), Info::GetCurBindTexture());
	Info::RaiseCurBindTexture();
}

void Plane::DetectCheckPoints() {
	auto plane = Collision::GetVehiclePosition();
	if (plane.z < CalcTerHeight(plane)) {
		Info::AddMessage("You lose. Plane has crashed. Try again", Vector3D(1.0, 0.0, 0.0), 100);
		Info::EndRound(false);
	}
	bool flag = true;
	for (int i = 0; i < list.size(); i++) {
		if (distance(list[i], plane) < 100.0f && checked[i] == 0) {
			for (int j = 0; j < i; j++) {
				if (checked[j] == 0) {
					checked[j] = 2;
					Info::AddMessage("Checkpoint missed (+5 second)", Vector3D(1.0, 0.4, 0.4));
					Info::SubPlayerScore(5);
				}
			}
			checked[i] = 1;
			Info::AddMessage("Checkpoint reached (-1 second)", Vector3D(0.4, 1.0, 0.4));
			Info::AddPlayerScore(1);
		}
	}

	for (int i = 0; i < checked.size(); i++) {
		if (checked[i] == 0) {
			flag = false;
			break;
		}
	}
	if (flag) {
		
		for (int i = 0; i < list.size(); i++) {
			checked[i] = 0;
		}
		Info::EndRound();
		Info::AddMessage("You Win!", Vector3D(0.7, 0.7, 0.1));
	}
}

void Plane::Draw() {
	if (Info::GetPlaneState() != state) {
		state = Info::GetPlaneState();
		model[4].SetState(state, 2.0f);
	}
	float pi = 3.141592;
	Vector3D player = Camera::getPosition();

	//float dst = distance(stone[i], player);
	//if (yaw == 0.0 && dst > 2500.0) continue;
	//else if (yaw == 3.0 && dst > 500.0) continue;
	//Vector3D a = stone[i] - Camera::getPosition();
	//a.Normalize();
	//Vector3D b = Info::GetEyeNormal();
	//b.Normalize();
	//float norm_vec = a*b;
	//if (yaw == 0) if (norm_vec < 0.5 && dst > 60.0) continue;


	glPushMatrix();
	btScalar *m = Collision::GetObjectMatrix(matrix);
	glMultMatrixf(m);
	delete[] m;
	glScaled(1.5, 1.5, 1.5);

	for (int i = 0; i < 5; i++)
	model[i].draw();

	glPopMatrix();
}
