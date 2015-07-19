#include "stdafx.h"
#include "SandBox.h"
#include <atltypes.h>

using namespace hc3d;

bool SandBox::hidedWindow = false;
Objects SandBox::objects;
Objects SandBox::shadowingObjects;
Object* SandBox::gWindow;
Object* SandBox::gWater;
Object* SandBox::gStone;
int SandBox::cursorX = 0;
int SandBox::cursorY = 0;

SandBox::SandBox()
{
}

float max(float a, float b) {
	return a > b ? a : b;
}

float min(float a, float b) {
	return a < b ? a : b;
}

void SandBox::Init() {
	//hc3dToolkit::XYZtoBMP();
	//exit(0);
	int MaxTextureUnits;
	glGetIntegerv(GL_MAX_TEXTURE_UNITS, &MaxTextureUnits);

	std::cout << "Max Texture Units" << MaxTextureUnits << std::endl;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &MaxTextureUnits);

	std::cout << "Max Texture Image Units" << MaxTextureUnits << std::endl;

	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &MaxTextureUnits);
	std::cout << "Max Combined Texture Image Units" << MaxTextureUnits << std::endl;

	objects.push_back(GetAtmo());
	objects.push_back(GetStar());

	const float terScale = 2.0f;
	Collision::Init();
	int a = 1;
	for (int j = 35; j >= 0; j--) {
		for (int i = 0; i < 42; i++) {
			std::string path;
			char num[10];
			path += "HeightMaps/swiss_";
			_itoa(a, num, 10);
			if (a < 10) path += "0";
			path += num;
			path += ".bmp";
			if (i <= 20 && j <= 20) AddTerrain(path, Vector3D(i * 512 * terScale, j * 512 * terScale, 0), Vector3D((256 + i * 512) * terScale, (256 + j * 512) * terScale, 0));
			a++;
		}
	}	

	objects.push_back(GetTerrain());
	objects.push_back(GetGrass());
//	objects.push_back(GetOcean());
//	for (int i = 0; i < 10; i++) objects.push_back(GetFish());
	gWater = GetWater();
	shadowingObjects.push_back(GetTrees());
	shadowingObjects.push_back(GetClouds());
	shadowingObjects.push_back(GetBut());
	
//	shadowingObjects.push_back(GetPlane());
	


	gWindow = GetGameWindow();
//	gStone = GetStone();
	objects.Init();
	//gStone->Init();

	shadowingObjects.Init();
	
	gWater->Init();
	gWindow->Init();
	Sleep(3000);
}

void SandBox::Draw() {
	Info::prev_time = Info::real_time;
	Camera::set_camera(cursorX - Info::width() / 2, cursorY - Info::height() / 2, Info::width(), Info::height());
	Collision::ResolveCollision();
	Info::global_scale = min(max((Camera::getPosition().z - 4900.0f) / 5000.0f, 0.0002f), 1.0f);
	glutSetCursor(GLUT_CURSOR_NONE);
	POINT cursorP;
	GetCursorPos(&cursorP);
	cursorX = cursorP.x;
	cursorY = cursorP.y;
	glClearColor(0.0, .0, .0, 1.0);


	if (Info::shadow) {
//		shadowingObjects.push_back(gStone);
	//	shadowingObjects.push_back(objects[2]);
		shadowingObjects.CreateShadows();
		//shadowingObjects.pop_back();
	}
	GLenum err = glGetError();
	//if (err) std::cout << "(shadow.draw) GL error : " << err << std::endl;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	float widthHeightRatio = (float)Info::width() / (float)Info::height();
	Vector3D sun = Info::sun;
	glLoadIdentity();


	Vector3D rolling(0, 0, 1);
	//rolling = Vector3D(Camera::pitch*-sin(Camera::roll), Camera::pitch*-cos(Camera::roll), 1);


	gluPerspective(60, widthHeightRatio, 5000.0*Info::global_scale, 50000000000.0*Info::global_scale);
	Info::eye_normal = Vector3D(cos(Camera::yaw)*cos(Camera::pitch), sin(Camera::yaw)*cos(Camera::pitch), sin(Camera::pitch));
	Vector3D position = Camera::getPosition();
	Vector3D eyePosition = position;
	if (Collision::IsBinded()) {
		gluLookAt(eyePosition.x, eyePosition.y, eyePosition.z,
			Camera::lookAt.x,
			Camera::lookAt.y,
			Camera::lookAt.z,
			Camera::rollBy.x, Camera::rollBy.y, Camera::rollBy.z);
	}
	else {
		if (Camera::FP) {
			//std::cout << Camera::yaw << " " << Camera::pitch << std::endl;
			gluLookAt(position.x, position.y, position.z,
				position.x + cos(Camera::yaw)*cos(Camera::pitch),
				position.y + sin(Camera::yaw)*cos(Camera::pitch),
				(position.z + sin(Camera::pitch)),
				rolling.x, rolling.y, 1);

		}
		else {
			eyePosition = position + Info::eye_normal * Camera::getTP();
			gluLookAt(eyePosition.x, eyePosition.y, eyePosition.z,
				position.x + cos(Camera::yaw)*cos(Camera::pitch),
				position.y + sin(Camera::yaw)*cos(Camera::pitch),
				(position.z + sin(Camera::pitch)),
				rolling.x, rolling.y, 1);
		}
	}
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glViewport(0, 0, Info::GetMaxWater(), Info::GetMaxWater());
	glScalef(1, 1, -1);
	glCullFace(GL_FRONT);

	GLdouble clip_plane[4] = { 0.0, 0.0, 1.0, -1.0 };
	GLdouble clip_plane2[4] = { 0.0, 0.0, -1.0, 1.0 };

	if (eyePosition.z > 0.0) glClipPlane(GL_CLIP_PLANE0, clip_plane);
	else glClipPlane(GL_CLIP_PLANE0, clip_plane2);

	glEnable(GL_CLIP_PLANE0);
	Info::SetReflect(true);
	objects.Draw();
	err = glGetError();
	//if (err) std::cout << "(objects.draw) GL error : " << err << std::endl;
//	gStone->Draw();
	Reflect();
	err = glGetError();
	//if (err) std::cout << "(reflect) GL error : " << err << std::endl;
	Info::SetReflect(false);

	glDisable(GL_CLIP_PLANE0);
	glCullFace(GL_BACK);
	glScalef(1, 1, -1);


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glViewport(0, 0, Info::width(), Info::height());
	
	objects.Draw();
//	gStone->Draw();
	if (Info::sceneBuild) SceneBuilder::Draw();
	Refract();
	err = glGetError();
	//if (err) std::cout << "(refract) GL error : " << err << std::endl;
	gWater->Draw();
	err = glGetError();
	//if (err) std::cout << "(water.draw) GL error : " << err << std::endl;
	shadowingObjects.Draw();
	//Collision::dynamicsWorld->debugDrawWorld();
	gWindow->Draw();
	err = glGetError();
	if (!Info::console_run) {
		Info::run(Info::width(), Info::height());
	}	
	else {
		Info::console();
	}
	Info::real_time = clock();
	Info::time_frame = Info::real_time - Info::prev_time;
	if (cursorP.x != (Info::width() / 2) || (cursorP.y != Info::height() / 2)) {
		if(!hidedWindow) SetCursorPos(Info::width() / 2, Info::height() / 2);
	}
	glutSwapBuffers();
	glutPostRedisplay();
}

void SandBox::Reshape(int w, int h) {
	glViewport(0, 0, w, h);
	glutPostRedisplay();
}

void SandBox::KeyPress(unsigned char key, int lparam, int rparam) {
	if (key == '`' || key == '~') {
		if (Info::console_run) Info::console_run = false;
		else Info::console_run = true;
		return;
	}
	if (Info::console_run) Info::inputChar(key);
	else if (Collision::IsBinded()){
		Collision::Keyboard(key, lparam, rparam);
	}
	else if (key == 'h' || key == 'H') {
		if (hidedWindow) {
			hidedWindow = false;
			glutShowWindow();
		}
		else {
			hidedWindow = true;
			glutHideWindow();
		}
	}
	else if (key == 'q') exit(0);
	else if (key == 'w' || key == 'W') Camera::gow = true;
	else if (key == 'r' || key == 'R') {
		if (Info::SSAO) Info::SSAO = false;
		else Info::SSAO = true;
	}
	//else if (key == 'e' || key == 'E') fire = true;
	else if (key == 's' || key == 'S') Camera::gos = true;
	else if (key == 'f' || key == 'F') {
		if (Info::flash == 0) Info::flash = 1;
		else Info::flash = 0;
	}
	/*else if (key == 'h' || key == 'H') {
		if (cloud.sort) cloud.sort = false;
		else cloud.sort = true;
	}*/
	else if (key == 'a' || key == 'A') Camera::goa = true;
	else if (key == 'p' || key == 'P')
	if (!Camera::FP) Camera::FP = true;
	else Camera::FP = false;
	//else if (key == 'l' || key == 'L') light_draw = true;
	else if (key == 'd' || key == 'D') Camera::god = true;
	else if (key == '0' || key == ')') Info::planeState = 0;
	else if (key == '1' || key == '!') {
		Info::coeff *= 1.1;
		std::cout << "coeff = " << Info::coeff << std::endl;
	}
	else if (key == '2' || key == '@') {
		Info::coeff /= 1.1;
		std::cout << "coeff = " << Info::coeff << std::endl;
	}
	else if (key == '3' || key == '#') {
		Info::coeff1 *= 1.1;
		std::cout << "coeff1 = " << Info::coeff1 << std::endl;
	}
	else if (key == '4' || key == '$') {
		Info::coeff1 /= 1.1;
		std::cout << "coeff1 = " << Info::coeff1 << std::endl;
	}
	/*else if (key == '1' || key == '!') Info::planeState = 1;
	else if (key == '2' || key == '@') Info::planeState = 2;
	else if (key == '3' || key == '#') Info::planet_z += 100;
	else if (key == '4' || key == '$') Info::planet_z -= 100;*/
	//else if (key == '5' || key == '%') cloud_offset.y += 10;
	//else if (key == '6' || key == '^') cloud_offset.y -= 10;
	else if (key == '7' || key == '&') Camera::PLAYER_SPEED *= 1.1;
	else if (key == '8' || key == '*') Camera::PLAYER_SPEED /= 1.1;
	else if (key == ' ') {
		Info::jet_gas = true;
		Collision::Explode(Camera::getPosition(), 10000.0f);
	}
	else if (key == 'c' || key == 'C') Info::jet_stop = true;
	else if (key == 'v' || key == 'V') Camera::cam_speed = Vector3D(0, 0, 0);
	if (Info::jet_gas) {
		if (Info::jetpack > 0) {
			Camera::cam_speed.z += 1.0f*Info::elapsed_time;
			Info::jetpack--;
		}
	}
	else if (Info::jet_stop) {
		Camera::cam_speed.z -= 1.0f*Info::elapsed_time;
		Info::jetpack--;
	}
	
}

void SandBox::KeyRelease(unsigned char key, int lparam, int rparam) {
	if (Info::console_run) return;

	if (Collision::IsBinded()){
		Collision::KeyboardUp(key, lparam, rparam);
	}
	else if (key == 'w' || key == 'W') Camera::gow = false;
	else if (key == 's' || key == 'S') Camera::gos = false;
	else if (key == 'a' || key == 'A') Camera::goa = false;
	else if (key == 'd' || key == 'D') Camera::god = false;
	else if (key == ' ') Info::jet_gas = false;
	else if (key == 'c' || key == 'C') Info::jet_stop = false;
	
	//else if (key == 'l' || key == 'L') light_draw = false;
}

void SandBox::SpecKey(int key, int lparam, int rparam) {
	
	if (Info::sceneBuild) {
		if (key == GLUT_KEY_UP) {
			SceneBuilder::Rotate(Vector3D(0, 1, 0));
		}
		else if (key == GLUT_KEY_DOWN) {
			SceneBuilder::Rotate(Vector3D(0, -1, 0));
		}
		else if (key == GLUT_KEY_LEFT) {
			SceneBuilder::Rotate(Vector3D(0, 0, 1));
		}
		else if (key == GLUT_KEY_RIGHT) {
			SceneBuilder::Rotate(Vector3D(0, 0, -1));
		}
		else if (key == GLUT_KEY_F2) SceneBuilder::offset.x += 0.1;
		else if (key == GLUT_KEY_F3) SceneBuilder::offset.x -= 0.1;
		else if (key == GLUT_KEY_F4) SceneBuilder::offset.y += 0.1;
		else if (key == GLUT_KEY_F5) SceneBuilder::offset.y -= 0.1;
		else if (key == GLUT_KEY_F6) SceneBuilder::offset.z += 0.1;
		else if (key == GLUT_KEY_F7) SceneBuilder::offset.z -= 0.1;

		else if (key == GLUT_KEY_F8) SceneBuilder::eyeNormScale += 0.1;
		else if (key == GLUT_KEY_F9) SceneBuilder::eyeNormScale -= 0.1;
	}
	else if(Collision::IsBinded()){
		Collision::specialKeyboard(key, lparam, rparam);
	}
}

void SandBox::SpecKeyRelease(int key, int lparam, int rparam) {
	if (Collision::IsBinded())
		Collision::specialKeyboardUp(key, lparam, rparam);
}

void SandBox::MousePress(int button, int state, int x, int y) {
	//GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, or GLUT_RIGHT_BUTTON
	//if (state == GLUT_UP) return;
	if (state == GLUT_UP) return;
	if (Info::sceneBuild) {
		if (button == GLUT_LEFT_BUTTON) SceneBuilder::Add();
	}
}
void SandBox::MouseWheelFunc(int wheel, int direction, int x, int y) {
	if (Info::sceneBuild) {
		if (direction == 1) {
			SceneBuilder::Resize(Vector3D(1.1f, 1.1f, 1.1f));
		}
		else {
			SceneBuilder::Resize(Vector3D(1.0f / 1.1f, 1.0f / 1.1f, 1.0f / 1.1f));
		}
	}
}

void SandBox::MouseMotion(int x, int y) {
	cursorX = x; cursorY = y;
}

void SandBox::MouseMove(int x, int y) {
	cursorX = x; cursorY = y;
}