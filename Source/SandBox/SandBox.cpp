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
	int MaxTextureUnits;
	glGetIntegerv(GL_MAX_TEXTURE_UNITS, &MaxTextureUnits);

	std::cout << "Max Texture Units" << MaxTextureUnits << std::endl;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &MaxTextureUnits);

	std::cout << "Max Texture Image Units" << MaxTextureUnits << std::endl;

	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &MaxTextureUnits);
	std::cout << "Max Combined Texture Image Units" << MaxTextureUnits << std::endl;

	objects.push_back(GetAtmo());
	objects.push_back(GetStar());

	Collision::Init();
	Terrain *ter;
	for (int i = 0; i < 32; i++) {
		for (int j = 0; j < 32; j++) {
			std::string path;
			char num[10];
			path += "HeightMaps/terrain";
			_itoa(i, num, 10);
			path += ".";
			path += num;
			_itoa(j, num, 10);
			path += ".";
			path += num;
			path += ".bmp";
			ter = GetTer(path, Vector3D(i * 1024, j * 1024, 0));
			Terrains::list.insert(TerrainList::value_type(ter, Vector3D(512 + i * 1024, 512 + j * 1024, 0)));
			objects.push_back(ter);
		}
	}	

	objects.push_back(GetGrass());
	objects.push_back(GetBut());
//	objects.push_back(GetOcean());
//	for (int i = 0; i < 10; i++) objects.push_back(GetFish());
	gWater = GetWater();
	shadowingObjects.push_back(GetTrees());
	shadowingObjects.push_back(GetClouds());
	


	gWindow = GetGameWindow();
	gStone = GetStone();
	objects.Init();
	gStone->Init();

	shadowingObjects.Init();
	
	gWater->Init();
	gWindow->Init();
	Sleep(3000);
}

void SandBox::Draw() {
	Info::prev_time = Info::real_time;
	Camera::set_camera(cursorX - Info::width() / 2, cursorY - Info::height() / 2, Info::width(), Info::height());
	Collision::ResolveCollision();
	Info::global_scale = min(max((Camera::getPosition().z - 400.0f) / 500.0f, 0.0002f), 1.0f);
	glutSetCursor(GLUT_CURSOR_NONE);
	POINT cursorP;
	GetCursorPos(&cursorP);
	cursorX = cursorP.x;
	cursorY = cursorP.y;
	glClearColor(0.0, .0, .0, 1.0);


	if (Info::shadow) {
		shadowingObjects.push_back(gStone);
	//	shadowingObjects.push_back(objects[2]);
		shadowingObjects.CreateShadows();
		shadowingObjects.pop_back();
	}
	GLenum err = glGetError();
	if (err) std::cout << "(shadow.draw) GL error : " << err << std::endl;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	float widthHeightRatio = (float)Info::width() / (float)Info::height();
	Vector3D sun = Info::sun;
	glLoadIdentity();


	Vector3D rolling(0, 0, 1);
	if (Camera::boat_mode) {
		rolling = Vector3D(Camera::boat_pitch*-sin(Camera::boat_roll), Camera::boat_pitch*-cos(Camera::boat_roll), 1);
	}

	gluPerspective(60, widthHeightRatio, 500.0*Info::global_scale, 5000000000.0*Info::global_scale);
	Info::eye_normal = Vector3D(cos(Camera::yaw)*cos(Camera::pitch), sin(Camera::yaw)*cos(Camera::pitch), sin(Camera::pitch));
	Vector3D position = Camera::getPosition();
	Vector3D eyePosition = position;
	if (Camera::FP) {
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
	SetReflect(true);
	objects.Draw();
	err = glGetError();
	if (err) std::cout << "(objects.draw) GL error : " << err << std::endl;
	gStone->Draw();
	Reflect();
	err = glGetError();
	if (err) std::cout << "(reflect) GL error : " << err << std::endl;
	SetReflect(false);

	glDisable(GL_CLIP_PLANE0);
	glCullFace(GL_BACK);
	glScalef(1, 1, -1);


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glViewport(0, 0, Info::width(), Info::height());
	
	objects.Draw();
	gStone->Draw();
	if (Info::sceneBuild) SceneBuilder::Draw();
	Refract();
	err = glGetError();
	if (err) std::cout << "(refract) GL error : " << err << std::endl;
	gWater->Draw();
	err = glGetError();
	if (err) std::cout << "(water.draw) GL error : " << err << std::endl;
	shadowingObjects.Draw();
	//Collision::dynamicsWorld->debugDrawWorld();
	gWindow->Draw();
	err = glGetError();
	if (err) std::cout << "(window.draw) GL error : " << err << std::endl;
	/*if((clock() / 1000) - Info::spawn_time / 1000 > 20.0) {
	Info::spawn_time = clock();
	npc1 tmp;
	tmp.init();
	npc.push_back(tmp);
	npc_num++;
	}*/
	//atmo.longitude += 0.001 * Info::elapsed_time;
	/*clock_t local_timer;
	cloud_offset += Vector3D(0.1, 0.1, 0)*Info::elapsed_time;*/

	/*if (fire) {
		if (character.recoil == 0.0) {
			bullet.add_bullet();
			character.recoil = -5.0;
		}
	}
	fire = false;
	Vector3D position = Camera::getPosition();
	if (shadow) shadow_init();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	float widthHeightRatio = (float)Info::width() / (float)Info::height();
	Vector3D sun = atmo.getSun();
	glLoadIdentity();


	Vector3D rolling(0, 0, 1);
	if (Camera::boat_mode) {
		rolling = Vector3D(Camera::boat_pitch*-sin(Camera::boat_roll), Camera::boat_pitch*-cos(Camera::boat_roll), 1);
	}

	gluPerspective(60, 1.0 * scr_width / scr_height, 500.0*Info::global_scale, 5000000000.0*Info::global_scale);
	Info::eye_normal = Vector3D(cos(Camera::yaw)*cos(Camera::pitch), sin(Camera::yaw)*cos(Camera::pitch), sin(Camera::pitch));
	Vector3D eyePosition = position;
	if (Camera::FP) {
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
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	glViewport(0, 0, Info::max_water, Info::max_water);
	glScalef(1, 1, -1);
	glCullFace(GL_FRONT);

	GLdouble clip_plane[4] = { 0.0, 0.0, 1.0, -1.0 };
	GLdouble clip_plane2[4] = { 0.0, 0.0, -1.0, 1.0 };

	if (eyePosition.z > 0.0) glClipPlane(GL_CLIP_PLANE0, clip_plane);
	else glClipPlane(GL_CLIP_PLANE0, clip_plane2);
	glCullFace(GL_BACK);
	glEnable(GL_CLIP_PLANE0);
	ter.reflect = true;
	atmo.draw();
	ter.draw();
	if (light_draw || Info::num_of_lightning > 0) {
		for (int i = 0; i < Info::num_of_lightning; i++) light[i].draw(false, 0.1*(i + 1));
	}

	water.reflect();

	glDisable(GL_CLIP_PLANE0);
	glViewport(0, 0, Info::max_water, Info::max_water);
	glScalef(1, 1, -1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	if (eyePosition.z > 0.0) glClipPlane(GL_CLIP_PLANE1, clip_plane2);
	else glClipPlane(GL_CLIP_PLANE1, clip_plane);
	//	glEnable(GL_CLIP_PLANE1);
	ter.reflect = false;
	ter.refract = true;
	//	ter.reflect = true;
	//	ter.refract = false;
	character.draw();
	atmo.draw();
	ter.draw();
	tree.draw(0, Camera::pitch);
	cloud.draw(false, cloud_offset);
	character.draw();
	but.draw(0, 0);
	//stone.draw(0,0);
	ter.refract = false;

	water.refract();


	//	glDisable(GL_CLIP_PLANE1);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, Info::width(), Info::height());

	local_timer = clock();
	atmo.draw();
	star.draw();
	Info::atmo_time = clock() - local_timer;

	local_timer = clock();
	ter.draw();
	Info::ter_time = clock() - local_timer;

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for (int i = 0; i < model_num; i++) model[i].draw();
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	local_timer = clock();
	stone.draw(0, 0);
	Info::models_time = clock() - local_timer;

	local_timer = clock();
	water.draw();
	Info::water_time = clock() - local_timer;

	local_timer = clock();
	tree.draw(0, Camera::pitch);
	Info::tree_time = clock() - local_timer;

	local_timer = clock();
	grass.draw(0, Camera::pitch);
	Info::grass_time = clock() - local_timer;

	local_timer = clock();
	cloud.draw(false, cloud_offset);
	Info::cloud_time = clock() - local_timer;

	local_timer = clock();
	but.draw(0, 0);
	character.draw();
	Info::models_time += clock() - local_timer;

	if (light_draw)
	for (int i = 0; i < npc_num; i++) {
		if (sphere_inter(npc[i].getPosition(), 2.0, Camera::getPosition(), Info::eye_normal)) {
			//	printf("INTERSECTION\n");
			npc.erase(npc.begin() + i);
			npc_num--;
			break;
		}
	}
	bullet.draw(0, 0);
	if (light_draw || Info::num_of_lightning > 0) {
		if (light_draw) {
			Info::num_of_lightning += Info::elapsed_time*2.0;
			Info::num_of_lightning = min(Info::num_of_lightning, 5);
			character.spread -= Info::elapsed_time*1.5;
			character.spread = max(character.spread, -2.0);
		}
		else {
			Info::num_of_lightning--;
		}
		for (int i = 0; i < Info::num_of_lightning; i++) light[i].draw(false, 0.1*(i + 1));
	}



	for (int i = 0; i < npc_num; i++) npc[i].draw();
	local_timer = clock();
	
	
	cloud_offset = Vector3D(0, 0, 0);*/
	//Info::window_time = clock() - local_timer;
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
//	glutWarpPointer(Info::width() / 2, Info::height() / 2);
	glutSwapBuffers();
	//SetCursorPos(Info::width() / 2, Info::height() / 2);
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
	}
	else if (Info::console_run) Info::inputChar(key);
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
	//else if (key == '1' || key == '!') atmo.longitude += 0.01;
	//else if (key == '2' || key == '@') atmo.longitude -= 0.01;
	else if (key == '3' || key == '#') Info::planet_z += 100;
	else if (key == '4' || key == '$') Info::planet_z -= 100;
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
	if (Info::console_run);
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
	else {
		Collision::specialKeyboard(key, lparam, rparam);
	}
}

void SandBox::SpecKeyRelease(int key, int lparam, int rparam) {
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
//	std::cout << "HERE" << std::endl;
//	std::cout << x - Info::width() << " " << std::endl;
	
}