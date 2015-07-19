#include "stdafx.h"
#include "Info.h"
#include "Camera.h"
#include <iostream>

#define YBORDER 20
#define XBORDER 10
#define STRSIZE 30.0
#define LOOP_WINDOW 3100

using namespace std;
using namespace hc3d;

bool Info::modelShaderInited = false;
GLuint Info::modelShader = 0;
float Info::latitude = 0;
float Info::longitude = 0;
float Info::shadowDist = 100.0;
float Info::num_of_lightning = 0;
Vector3D Info::eye_normal = Vector3D(0,0,0);
Vector3D Info::yaw_pitch = Vector3D(0,0,0);
Vector3D Info::gun1 = Vector3D(0,0,0);
Vector3D Info::gun1_rotation = Vector3D(0,0,0);
int Info::max_water = 4096;
int Info::cur_loading_texture = 16;
int Info::LoD = 0;
int Info::flash = 0;
bool Info::jet_gas = false;
bool Info::collision = false;
bool Info::jet_stop = false;
int Info::jetpack = 100000;
int Info::planeState = 0;
GLuint Info::shaderprogram = 0;
int Info::shadow = 0;
int Info::min_quad = 2;
bool Info::shader = true;
clock_t Info::prev_time = clock();
clock_t Info::real_time = clock();
clock_t Info::spawn_time = clock();
clock_t Info::time_frame = 0.1;
double Info::time = 0.0;
float Info::grass_time = 0.0;
bool Info::console_run = false;
float Info::cloud_time = 0.0;
float Info::ter_time = 0.0;
float Info::tree_time = 0.0;
float Info::water_time = 0.0;
float Info::atmo_time = 0.0;
float Info::window_time = 0.0;
float Info::models_time = 0.0;
float Info::elapsed_time = 0.0;
float Info::planet_z = 0.0;
float Info::gravity = -9.8;
float Info::global_scale = 0.0002;
ConsoleWord Info::cur_word;
ConsoleLine Info::cur_line;
ConsoleText Info::console_output;
Vector3D Info::atmo_color(0,0,0);
Vector3D Info::sun(0, 0, 0);
int Info::cur_time = 0;
int Info::start_time = 0;
int Info::frames = 0;
int Info::_width = 0;
int Info::_height = 0;
float Info::fps = 0.0;
bool Info::AA = false;
bool Info::godRays = false;
bool Info::bloom = false;
bool Info::filmGrain = false;
bool Info::depthOfField = false;
bool Info::toneMapping = false;
bool Info::SSAO = false;
bool Info::atmosphereRun = false;
bool Info::sceneBuild = false;
bool Info::refract = false;
bool Info::reflect = false;
float Info::wait = 0.0;
GLuint Info::curBindTexture = 17;
clock_t Info::roundTime = 0;
clock_t Info::startRoundTime = 0;
bool Info::runned = false;
bool Info::finished = false;
std::vector<std::string> Info::messages;
std::vector<Vector3D> Info::messageColors;
std::vector<float> Info::messageLife;
float Info::vehSpeed = 0.0f;
Vector3D Info::forestLocation = Vector3D(0,0,0);
float Info::coeff = 0.03;
float Info::coeff1 = 0.035;

float Info::GetCoeff() {
	return coeff;
}

float Info::GetCoeff1() {
	return coeff1;
}

void Info::SetVehSpeed(float khSpeed) {
	vehSpeed = khSpeed;
}
		

bool Info::IsRunned() {
	return runned;
}

void Info::DropMessages() {
	messages.clear();
	messageColors.clear();
	messageLife.clear();
}

void Info::AddMessage(std::string message, Vector3D color, float time) {
	if (messages.size() > 5) {
		messages.erase(messages.begin());
		messageColors.erase(messageColors.begin());
		messageLife.erase(messageLife.begin());
	}
	messages.push_back(message);
	messageColors.push_back(color);
	messageLife.push_back(time);
}

int Info::GetPlaneState() {
	return planeState;
}

GLuint Info::GetCurBindTexture() {
	return curBindTexture;
}

void Info::RaiseCurBindTexture() {
	curBindTexture++;
	if (curBindTexture > 30) curBindTexture = 17;
}

bool Info::GetModelShaderInited() {
	return modelShaderInited;
}

void Info::SetModelShaderInited(bool val) {
	modelShaderInited = val;
}

GLuint Info::GetModelShader() {
	return modelShader;
}

void Info::SetModelShader(GLuint val) {
	modelShader = val;
}

float Info::GetLatitude() {
	return latitude;
}

float Info::GetLongitude() {
	return longitude;
}

void Info::SetLatitude(float val) {
	latitude = val;
}

void Info::SetLongitude(float val) {
	longitude = val;
}

double Info::getTime() {
	return time;
}

float Info::GetElapsedTime() {
	return elapsed_time;
}

bool Info::GetConsoleRun() {
	return console_run;
}

void Info::SetShader(bool _shader) {
	shader = _shader;
}

float Info::dist(Vector3D start, Vector3D end) {
	Vector3D c = end - start;
	return sqrt(c.x*c.x + c.y*c.y + c.z*c.z);
}

void Info::SetSun(Vector3D _sun) {
	sun = _sun;
}

float Info::GetShadowDist() {
	return shadowDist;
}

bool Info::GetAA() {
	return AA;
}

bool Info::GetGodRays() {
	return godRays;
}

bool Info::GetBloom() {
	return bloom;
}

bool Info::GetFilmGrain() {
	return filmGrain;
}

bool Info::GetDOF() {
	return depthOfField;
}

bool Info::GetToneMapping() {
	return toneMapping;
}

int Info::GetMaxWater() {
	return max_water;
}

bool Info::GetRefract() {
	return refract;
}

void Info::SetRefract(bool refract) {
	Info::refract = refract;
}

bool Info::GetReflect() {
	return reflect;
}

void Info::SetReflect(bool reflect) {
	Info::reflect = reflect;
}

void Info::StartRound() {
	DropMessages();
	startRoundTime = clock();
	finished = false;
	runned = true;
}

void Info::EndRound(bool success) {
	roundTime = clock() - startRoundTime;
	
	if (success) {
		int curRes = roundTime;
		FILE *fp;
		fopen_s(&fp, "results.txt", "r");

		int result;
		fscanf(fp, "%d\n", &result);
		fclose(fp);

		char curResC[100];
		char resC[100];

		int curTime = curRes;
		int mSec = curTime % 1000;
		curTime /= 1000;
		int sec = curTime % 60;
		curTime /= 60;
		int min = curTime % 60;
		curTime /= 60;
		int hour = curTime % 24;
		sprintf(curResC, "time: %d:%d:%d:%d\n", hour, min, sec, mSec);

		curTime = result;
		mSec = curTime % 1000;
		curTime /= 1000;
		sec = curTime % 60;
		curTime /= 60;
		min = curTime % 60;
		curTime /= 60;
		hour = curTime % 24;
		sprintf(resC, "time: %d:%d:%d:%d\n", hour, min, sec, mSec);
		DropMessages();

		if (curRes < result) {
			FILE *fp;
			fopen_s(&fp, "results.txt", "w");

			int result;
			fprintf(fp, "%d\n", curRes);
			fclose(fp);

			AddMessage("New record!", Vector3D(1.0, 0.5, 0.5), 300);
			AddMessage(string("Your ") + curResC, Vector3D(0.5, 1.0, 0.5), 300);
			AddMessage(string("Best ") + resC, Vector3D(1.0, 0.0, 0.0), 300);
		}
		else {
			AddMessage(string("Your ") + curResC, Vector3D(1.0, 0.0, 0.0), 300);
			AddMessage(string("Best ") + resC, Vector3D(0.5, 1.0, 0.5), 300);
		}
	}
	runned = false;
	finished = true;
}

void Info::AddPlayerScore(int score) {
	startRoundTime += score * 1000;
}

void Info::SubPlayerScore(int score) {
	startRoundTime -= score * 1000;
}

void Info::atmosphere_calc() {
	atmo_color = Vector3D(0,0,0);
    float poz = 0.0;
	Vector3D pozit = Vector3D(0,0,0);
	pozit.z = 0.0;
	Vector3D sunz = sun;
	Vector3D sunz2 = sunz;
	float height = 400.0;
	Vector3D pos2 = Vector3D(5000,0,height);
	float poz2 = pos2.z;
    bool lower = false;
    Vector3D sun = sunz + pozit;
    Vector3D sun2 = sunz2 + pozit;
    Vector3D poser = pos2;
    if((pos2.z - poz) < 0.0) {
        lower = true;
       pos2.z = poz;
    }
	lower = false;
    float cosinus = 5000.0/dist(Vector3D(pos2.x,pos2.y,0),Vector3D(pozit.x,pozit.y,0));
    float tetta = acos((pos2.z - pozit.z)/dist(pozit,pos2));
    float tetta_s = acos((sun.z - pozit.z)/dist(pozit,sun));
    Vector3D a = sun - pozit;
    Vector3D b = pos2 - pozit;
    float gamma = a*b/(a*a*b*b);
    float step = 1.0 - (5000.0 - sunz.z)/5000.0;
    if(step <= 0.1) step = step + (0.1 - step)/1.2;
    step = max(0.0,step);
    float blue = 0.0;
	pos2.z = poz2;
    float zz = (pos2.z - pozit.z)*cosinus;
    if(zz <= 1000.0) {
        zz = 1.0 - (zz/1000.0);
        blue = zz;
    }
    else blue = 0.0;
    float steper = step;
    if(step <= 0.2) {
        steper = 1.0 - (steper*5.0);
        blue *= steper;
    }
    else blue = 0.0;
    Vector3D Yz(0.05*step-poz/200000.0,0.05*step-poz/200000.0,0.25*step-poz/40000.0);//vec4(0.04-poz/100000.0,0.05-poz/80000.0,0.25-poz/4000.0,1.0);
    Yz.x = min(max(0.0,Yz.x),1.0);
    Yz.y = min(max(0.0,Yz.y),1.0);
    Yz.z = min(max(0.0,Yz.z),1.0);
    if(blue != 0.0) {
        Yz.z -= Yz.z*blue;
        Yz.y -= Yz.y*blue/2.0;
    }
    Vector3D Yc = Yz*((0.91+10.0*exp(-3.0*gamma)+0.45*pow(double(cos(gamma)),2.0))*(1.0-exp(-0.32/cos(tetta))))/((0.91+10.0*exp(-3.0*tetta_s)+0.45*pow(double(cos(tetta_s)),2.0))*(1.0-exp(-0.32)));
    Yc.x = min(max(0.0,Yc.x),1.0);
    Yc.y = min(max(0.0,Yc.y),1.0);
    Yc.z = min(max(0.0,Yc.z),1.0);
    atmo_color = Yc;
	float blend_c = 0.45;
	if(sun.z <= 0.0) blend_c /= (abs(sun.z)+250.0)/250.0;
	if(height * cosinus >= 0.0 && height * cosinus <= 2000.0) {
		float blend = (2000.0 - height*cosinus)/2000.0;	
		float bl = blend_c*pow(double(blend),2.0);
		atmo_color += bl*Vector3D(0.4,0.7,0.6);
	}
	else if(height*cosinus < 0.0) {
		atmo_color += blend_c*Vector3D(0.4,0.7,0.6);
	}
}

Info::Info(void)
{
}

void Info::init() {
	//TODO: настроить прием из параметров и из консоли
	shadow = 0;
	shadowDist = 500.0;

	RECT display;
	const HWND hDisplay = GetDesktopWindow();
	GetWindowRect(hDisplay, &display);
	_width = display.right;
	_height = display.bottom;
	while (_width < max_water || _height < max_water) max_water /= 2;
	while (_width > min_quad || _height > min_quad) min_quad *= 2;
	shaderprogram = LoadShader("Shaders/info.vert", "Shaders/info.frag");
}
int Info::width() {
	//cout << "width " << _width << endl;
	return _width;
}


int Info::height() {
	//cout << "height " << _height << endl;
	return _height;
}

Vector3D Info::GetSun() {
	return sun;
}
int Info::GetShadow() {
	return shadow;
}

Vector3D Info::GetAtmoColor() {
	return atmo_color;
}

int Info::GetFlash() {
	return flash;
}

bool Info::GetShader() {
	return shader;
}

int Info::GetCurTextureNum() {
	return cur_loading_texture;
}

void Info::RaiseCurTextureNum() {
	cur_loading_texture++;
}

Vector3D Info::GetEyeNormal() {
	return eye_normal;
}

bool Info::GetSSAO() {
	return SSAO;
}

Info::~Info(void)
{
}

Vector3D Info::GetForestLocation() {
	return forestLocation;
}

void Info::run(int w,int h) {
	Vector3D player = Camera::getPosition();
	auto dst = dist(player, forestLocation);
	if (dst > LOOP_WINDOW) {
		if (forestLocation.x - player.x < -LOOP_WINDOW) {
			forestLocation.x += LOOP_WINDOW * 2.0;
		}
		if (forestLocation.y - player.y < -LOOP_WINDOW) {
			forestLocation.y += LOOP_WINDOW * 2.0;
		}
		if (forestLocation.x - player.x > LOOP_WINDOW) {
			forestLocation.x -= LOOP_WINDOW * 2.0;
		}
		if (forestLocation.y - player.y > LOOP_WINDOW) {
			forestLocation.y -= LOOP_WINDOW * 2.0;
		}
	}
	elapsed_time = float(time_frame)/100.0;
	if (wait > 0.0) {
		elapsed_time += 30.0;
		wait -= 30.0;
	}
	calc_fps();
	DrawMessage(w, h);
	if (runned) {
		render_fps(20, 20, w, h);
	}
	atmosphere_calc();
	shader = true;
	curBindTexture = 17;
	//if (atmosphereRun) 
	longitude += (0.0125*3.1415 / 180) * elapsed_time;//2 real seconds = 1 game minute
	//if (!runned && !messages.size()) AddMessage("Press \"R\" to start game", Vector3D(1.0, 0.0, 0.0));
}
char* Info::init_fps_string(char* string) {
		Vector3D pos = Camera::getPosition();
		int tt = time_frame;
		std::string output = "";
		sprintf(string,"FPS = %.2f    One frame time %d ms\nPosition = %.2f(x) %.2f(y) %.2f(z)\nJetpack fuel = %d(u)\nLoD = %d\nz speed = %.2f\n offset %.2f %.2f\nplanet_z = %f km",fps,tt,pos.x,pos.y,pos.z,jetpack,LoD,Camera::cam_speed.z,yaw_pitch.x,yaw_pitch.y, planet_z);
		output += string;
		sprintf(string,"\natmo: %.2f ms\nmodels: %.2f ms\nterrain: %.2f ms\nclouds: %.2f ms\ntrees: %.2f ms\ngrass: %.2f ms\nwater: %.2f ms\nwindow: %.2f ms\n",atmo_time, models_time, ter_time, cloud_time, tree_time, grass_time, water_time, window_time);
		output += string;
		strcpy(string, output.c_str());
		return string;
}

char* Info::GetGameString(char * format) {
	clock_t curTime = clock() - startRoundTime;
	int mSec = curTime % 1000;
	curTime /= 1000;
	int sec = curTime % 60;
	curTime /= 60;
	int min = curTime % 60;
	curTime /= 60;
	int hour = curTime % 24;
	sprintf(format, "Time: %d:%d:%d:%d\nSpeed: %.1f km\/hour", hour, min, sec, mSec, vehSpeed);
	//std::cout << format << std::endl;
	return format;
}

void Info::inputChar(unsigned char key) {
	//char buf[10];
	//itoa(key,&buf[0], 10);

	if(cur_line.size() == 0) {
		cur_word.word = ":> ";
		cur_word.color = Vector3D(0.2,1.0,0.4);
		cur_word.y = -STRSIZE ;
		cur_word.y *= (float)console_output.size();
		cur_line.push_back(cur_word);
		cur_word.word.clear();
		cur_word.color = Vector3D(1.0,1.0,1.0);
	}
	if(key != 13) {		
		if(key != ' ') 
			cur_word.word += key;
		else 
			if(strcmp(cur_word.word.c_str(), "")) {
				cur_word.color = Vector3D(1.0,1.0,1.0);//color
				cur_word.y = -STRSIZE * console_output.size();
				cur_line.push_back(cur_word);
				cur_word.word.clear();
			}
	}
	else {
		if(strcmp(cur_word.word.c_str(), "")) {
				cur_word.color = Vector3D(1.0,1.0,1.0);//color
				cur_word.y = -STRSIZE * console_output.size();
				cur_line.push_back(cur_word);
		}
		console_output.push_back(cur_line);
		run_script(cur_line);
		cur_line.clear();
		//char buf[10];
		//itoa(cur_line.size() ,&buf[0], 10);
		//console_output += buf;
		//console_output += "\n";
		cur_word.word.clear();
	}
}

void Info::draw_string(std::string str, int offset, Vector3D color) {
	glUniform3f(glGetUniformLocation(
			        	shaderprogram, "color"), color.x, color.y, color.z);
	for (int i = 0; i != str.size(); ++i) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str[i]);
	}
}

void Info::console() {
	if(cur_line.size() == 0) {
		cur_word.word = ":> ";
		cur_word.color = Vector3D(0.2,1.0,0.4);
		cur_word.y = -STRSIZE ;
		cur_word.y *= (float)console_output.size();
		cur_line.push_back(cur_word);
		cur_word.word.clear();
		cur_word.color = Vector3D(1.0,1.0,1.0);
	}
	int y_offset = console_output.size() * STRSIZE;//mouse wheel movement put here
	elapsed_time = 0.0;
	glUseProgram(0);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.0,_width,0.0,_height);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
			
	glUseProgram(shaderprogram);
		
	int a = 1;
	for (int i = 0; i != console_output.size(); ++i) {
		for(int j = 0; j != console_output[i].size(); ++j) {
			if(j == 0) {
					int y_pos = _height / 2 - YBORDER + console_output[i][j].y + y_offset;
					if(y_pos > (_height - 10) || y_pos < 10) break;
					glRasterPos2i(XBORDER, y_pos); 
			}
			draw_string(console_output[i][j].word, console_output[i][j].y + y_offset, console_output[i][j].color);
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, ' ');
		}
	}
	for(int i = 0; i != cur_line.size(); i++) {
		if(i == 0) { //cur_line always not empty
			int y_pos = _height / 2 - YBORDER + cur_line[i].y + y_offset;
			if(y_pos > (_height - 10) || y_pos < 10) break;
			glRasterPos2i(XBORDER, y_pos); 
		}
		draw_string(cur_line[i].word, cur_line[i].y + y_offset, cur_line[i].color);
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, ' ');
	}
	draw_string(cur_word.word, cur_word.y + y_offset, cur_word.color);
	glUseProgram(0);
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

}

void Info::DrawMessage(int w, int h) {
	int x = w / 2 - 30;
	int y = h / 2;
	glUseProgram(0);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.0, w, 0.0, h);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glUseProgram(shaderprogram);
	glRasterPos2i(x, y);

	int a = 1;

	
	for (int i = 0; i < messages.size();) {
		if (messageLife[i] < 0.0f) {
			messageLife.erase(messageLife.begin() + i);
			messageColors.erase(messageColors.begin() + i);
			messages.erase(messages.begin() + i);
		}
		else i++;
	}

	for (int i = 0; i < messages.size(); i++) {
		glUniform3f(glGetUniformLocation(
			shaderprogram, "color"), messageColors[i].x, messageColors[i].y, messageColors[i].z);
		auto _string = messages[i];
		messageLife[i] -= elapsed_time;
		//std::cout << _string.c_str() << std::endl;
		for (int i = 0; i < _string.size(); ++i) {
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, _string[i]);
		}
		glRasterPos2i(x, y - 20 * a);
		a++;
	}
	glUseProgram(0);
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

}

void Info::render_fps(int x, int y,int w, int h) {
			char *_string = new char[1000];
			//GetGameString(_string);
			init_fps_string(_string);
			glUseProgram(0);
			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();
			gluOrtho2D(0.0,w,0.0,h);

			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();
			
			glUseProgram(shaderprogram);
			glUniform3f(glGetUniformLocation(
			        	shaderprogram, "color"), 1.0, 1.0, 1.0);
			glRasterPos2i(x, h - 2*y);
		
			int a = 1;
			for (int i = 0; _string[i]!= '\0'; ++i) {
				//std::cout << "zbs";
				glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, _string[i]);
				if(_string[i] == '\n') {
					glRasterPos2i(x, h - 2*y - 20*a); 
					a++;
				}
			}
		//	glRasterPos2i(w/2, h/2); 
		//	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, '+');

			glUseProgram(0);
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();

			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			delete[] _string;
			
}
void Info::calc_fps() {
	if(start_time == 0) start_time = clock() - 0.5;
	time += elapsed_time;
	cur_time = int(time)%10000;
	if(frames == 10) {
		frames = 0;
		float tmp = 1.0/((cur_time - start_time)/10000.0);
		start_time = cur_time;
		if(tmp > 0) fps = tmp;
	}
	else frames++;
	//std::cout << fps << std::endl;
}

float Info::GetGlobalScale() {
	return global_scale;
}

float Info::GetEarthZ() {
	return planet_z;
}