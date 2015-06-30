#include "StdAfx.h"
#include "Camera.h"
#include "Info.h"
#include <iostream>

#define pi 3.141592

using namespace hc3d;

Vector3D Camera::mousex = Vector3D(0, 0, 0);
Vector3D Camera::mousey = Vector3D(0, 0, 0);
Vector3D Camera::cam_speed = Vector3D(0,0,0);
Vector3D Camera::position = Vector3D(0,0,0);
Vector3D Camera::eyePosition = Vector3D(0,0,0);
Vector3D Camera::direction = Vector3D(1,0,0);
float Camera::yaw = 0;
float Camera::frames = 0;
float Camera::pitch = 0;
float Camera::roll = 0;

float Camera::boat_yaw = 0;
float Camera::boat_pitch = 0;
float Camera::boat_roll = 0;

bool Camera::goa = false;
bool Camera::gow = false;
bool Camera::gos = false;
bool Camera::god = false;
float Camera::shake = 0.0;
float Camera::boat_yaw_s = 0.0;
float Camera::boat_pitch_s = 0.0;
float Camera::boat_roll_s = 0.0;
float Camera::PLAYER_SPEED = 0.0;
float Camera::sensivity = 0.0;
float Camera::speed = 0;
float Camera::TP_ammount = -1.0;
bool Camera::boat_mode = false;
bool Camera::FP = true;
bool Camera::camera_inited = false;
Vector3D Camera::lookAt(0,0,0);
Vector3D Camera::rollBy(0, 0, 0);

Vector3D Camera::getPosition() {
	return position;
}

void Camera::setPosition(Vector3D val) {
	position = val;
}

Vector3D Camera::getEyePosition() {
	return Camera::eyePosition;
}

float Camera::getTP() {
	return Camera::TP_ammount;
}

float Camera::GetYaw() {
	return yaw;
}

float Camera::GetPitch() {
	return pitch;
}

float Camera::GetRoll() {
	return roll;
}

Vector3D Camera::GetSpeed() {
	return cam_speed;
}

float Camera::GetBoatYaw() {
	return boat_yaw;
}

Camera::Camera(void)
{
}

void Camera::init() {
	yaw = 0;
	pitch = 0;
	roll = 0;
	cam_speed = Vector3D(0.0, 0.0, 0.0);
	goa = false; gos = false; god = false; gow = false;
	PLAYER_SPEED = 20.0; sensivity = 1.0;
	position.Set(2000,2000,40);
	direction = Vector3D(1,0,0);
}

float Camera::rnd() {
	return (float(rand()%1000)/100.0-5.0)/500.0;
}

void Camera::SetEuler(float yaw, float pitch, float roll) {
	Camera::yaw = yaw;
	Camera::pitch = pitch;
	Camera::roll = roll;
}

void Camera::SetLookAt(Vector3D lookAt) {
	Camera::lookAt = lookAt;
}

Vector3D Camera::GetLookAt() {
	return Camera::lookAt;
}


void Camera::SetRollBy(Vector3D rollBy) {
	Camera::rollBy = rollBy;
}

void Camera::set_camera(int x, int y,int w, int h) {
	// mouse smooth
	mousex.x = mousex.y;
	mousex.y = mousex.z;
	mousex.z = x;

	mousey.x = mousey.y;
	mousey.y = mousey.z;
	mousey.z = y;

	x = (mousex.x + mousex.y + mousex.z) / 3.0;
	y = (mousey.x + mousey.y + mousey.z) / 3.0;

	if(TP_ammount > -1.0) TP_ammount += 0.1;
	
	yaw += -x*sensivity*0.01;
	pitch += -y*sensivity*0.01;
	pitch = max(min(pitch,pi/2.01),-pi/2.01); // 2.01 - необходимое допущение погрешности
	shake = 0;
	yaw += rnd()*shake;
	pitch += rnd()*shake;
	
//	std::cout << x << " " << y << std::endl;
//	std::cout << Info::width() << Info::height() << std::endl;
//	glutWarpPointer(Info::width()/2, Info::height()/2);	
	
	boat_yaw -= sin(frames)*(speed+0.5)/26.0;
	boat_pitch -= cos(frames*1.5)/32.0;
	frames += 0.1*(speed+1.0)*Info::elapsed_time;
	boat_yaw += sin(frames)*(speed+0.5)/26.0;
	boat_pitch += cos(frames*1.5)/32.0;
	roll = 0;
	
	if(gow){
			cam_speed += 4.0*Vector3D(cos(yaw)*cos(pitch)*PLAYER_SPEED*Info::elapsed_time,
							  sin(yaw)*cos(pitch)*PLAYER_SPEED*Info::elapsed_time,
							  0);
	}
	if(gos){
			cam_speed += 4.0*Vector3D(-cos(yaw)*cos(pitch)*PLAYER_SPEED*Info::elapsed_time,
							  -sin(yaw)*cos(pitch)*PLAYER_SPEED*Info::elapsed_time,
							  0);
	}
	if(goa){
			cam_speed += 4.0*Vector3D(-sin(yaw)*PLAYER_SPEED*Info::elapsed_time,+cos(yaw)*PLAYER_SPEED*Info::elapsed_time,0);
	}
	if(god){
			cam_speed += 4.0*Vector3D(sin(yaw)*PLAYER_SPEED*Info::elapsed_time,-cos(yaw)*PLAYER_SPEED*Info::elapsed_time,0);
	}
	float ctrl = 4.0; // make class var
	float cur_speed = sqrt(cam_speed.x*cam_speed.x + cam_speed.y*cam_speed.y);
	if(cur_speed > PLAYER_SPEED*3.0) {
		cam_speed.x *=  (PLAYER_SPEED*3.0)/cur_speed;
		cam_speed.y *=  (PLAYER_SPEED*3.0)/cur_speed;
	}
	cam_speed.z = min(max(cam_speed.z, -PLAYER_SPEED),PLAYER_SPEED);

	cam_speed.x *= 0.75;
	cam_speed.y *= 0.75;
	if(!FP) {
		eyePosition = position + Info::eye_normal*TP_ammount;
	}
	else eyePosition = position;
	
	
	if(!camera_inited) {
			yaw = -boat_roll;
			roll = boat_pitch;
			pitch = -boat_yaw;
			camera_inited = true;
	}
	cam_speed.z /= 100.0;
}

Vector3D Camera::GetCamSpeed() {
	return cam_speed;
}

Camera::~Camera(void)
{
}
