#include "StdAfx.h"
#include "Fish.h"
#include "hc3dObjects.h"
#include <fstream>
#include <iostream>

#define pi 3.1415
using namespace std;
using namespace hc3d;

Fish::Fish(void)
{
}


Fish::~Fish(void)
{
}

void Fish::setShader() {
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D,Shadow::GetShadowMap()[0]);
	        glUniform1i(glGetUniformLocation(
	        		shaderprogram, "_shadow0"), 1);

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, Shadow::GetShadowMap()[1]);
	        glUniform1i(glGetUniformLocation(
	        		shaderprogram, "_shadow1"), 2);

			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, Shadow::GetShadowMap()[2]);
	        glUniform1i(glGetUniformLocation(
	        		shaderprogram, "_shadow2"), 3);

			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, Shadow::GetShadowMap()[3]);
	        glUniform1i(glGetUniformLocation(
	        		shaderprogram, "_shadow3"), 4);

			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_2D, Shadow::GetShadowMap()[4]);
	        glUniform1i(glGetUniformLocation(
	        		shaderprogram, "_shadow4"), 5);

			glActiveTexture(GL_TEXTURE6);
			glBindTexture(GL_TEXTURE_2D, Shadow::GetShadowMap()[5]);
	        glUniform1i(glGetUniformLocation(
	        		shaderprogram, "_shadow5"), 6);

			glActiveTexture(GL_TEXTURE7);
			glBindTexture(GL_TEXTURE_2D, Shadow::GetShadowMap()[6]);
	        glUniform1i(glGetUniformLocation(
	        		shaderprogram, "_shadow6"), 7);

			glUniform1f(glGetUniformLocation(
				shaderprogram, "Width"), Info::width());
			glUniform1f(glGetUniformLocation(
				shaderprogram, "value"), 2.0);
			glUniform1f(glGetUniformLocation(
				shaderprogram, "Height"), Info::height());
			glActiveTexture(GL_TEXTURE0 + Info::GetCurBindTexture());
	        glBindTexture(GL_TEXTURE_2D, texture[0]);
	        glUniform1i(glGetUniformLocation(
				shaderprogram, "boat"), Info::GetCurBindTexture());
			Info::RaiseCurBindTexture();

			Vector3D sun = Info::GetSun();
			glUniform3f(glGetUniformLocation(
	        		shaderprogram, "lightPos"), sun.x, sun.y, sun.z);
			Vector3D player = Camera::getPosition();
			glUniform3f(glGetUniformLocation(
	        		shaderprogram, "eyePos"), player.x, player.y, player.z);
			glUniform1i(glGetUniformLocation(
	        		shaderprogram, "flash"), Info::GetFlash());
			glUniform3f(glGetUniformLocation(
				shaderprogram, "eyeNorm"), Info::GetEyeNormal().x, Info::GetEyeNormal().y, Info::GetEyeNormal().z);

}
float Fish::dist(Vector3D a, Vector3D b) {
	Vector3D c = a - b;
	return sqrt(c.x*c.x + c.y*c.y);
}
void Fish::Init() {
	Vector3D off;
	while (1) {
		off.x = ((rand() % 10000) / 10.0 + 1200);
		off.y = ((rand() % 10000) / 10.0 + 1000);
		off.z = -4.0;
		if (CalcTerHeight(off) < -5) break;
	}
	list_id = 0;
	shaderprogram = LoadShader("Shaders/boat.vert","Shaders/boat.frag");
	texture = new GLuint[2];
	glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
	Info::RaiseCurTextureNum();
	texture[0] = LoadTexture("Texture/fish.png");
	fish_num = 50;
	offset = new Vector3D[fish_num];
	float *sign = new float[3];
	for(int i = 0; i < fish_num; i++) {
		if(rand()%1000 > 500) sign[0] = 1.0;
		else sign[0] = -1.0;
		if(rand()%1000 > 500) sign[1] = 1.0;
		else sign[1] = -1.0;
		if(rand()%1000 > 500) sign[2] = 1.0;
		else sign[2] = -1.0;
	//	while(1) {
			offset[i].x = sign[0] * ((rand() % 2000) / 1000.0 + 0.2);
			offset[i].y = sign[1] * ((rand() % 2000) / 1000.0 + 0.2);
			offset[i].z = sign[2] * ((rand() % 2000) / 1000.0 - 1.0);
		//	if (Terrain::calc_height(offset[i] + off) < (offset[i].z - 1)) break;
	//	}
	//	offset[i] += off;
	}
	_mdl_x = new float[fish_num];
	_mdl_y = new float[fish_num];
	_mdl_z = new float[fish_num];
	stamina_boost = new float[fish_num];
	mdl_pos = new Vector3D[fish_num];
	mdl_center = new Vector3D[fish_num];
	mdl_memory = new Vector3D[fish_num];
	mdl_direction = new Vector3D[fish_num];
	mdl_speed = new Vector3D[fish_num];
	for(int i = 0; i < fish_num; i++) {
		stamina_boost[i] = 1.0;
		_mdl_x[i]= 0 ,_mdl_y[i] = 0,_mdl_z[i] = 0;
		mdl_pos[i] = Vector3D(0,0,0);
		mdl_center[i] = Vector3D(20,90.5,-1)+offset[i];
		mdl_memory[i] = offset[i] + off;
		mdl_direction[i] = Vector3D(0,0,rand()%10);
		mdl_speed[i] = Vector3D(0,0,0);
	}
	mdl_scale = 0.01;

	load_m = false,mdl_change = true;
    ifstream file("Models/Fish.mdl");
    int size;
	if(!file.is_open()) cout << "can not load 'Fish.mdl'";
    file >> size;
    vertex_size = size*3;
    vertex = new GLfloat[size*3];
	vertex2 = new GLfloat[size*3];
    for(int i = 0; i < size*3; i++) {
        file >> vertex[i];
		vertex2[i] = vertex[i];
    }
    file >> size;
    vertex_n_size = size*3;
    vertex_n = new GLfloat[size*3];
	vertex2_n = new GLfloat[size*3];
    for(int i = 0; i < size*3; i++) {
        file >> vertex_n[i];
		vertex2_n[i] = vertex_n[i];
    }
    file >> size;
    vertex_t_size = size*3;
    vertex_t = new GLfloat[size*3];
    for(int i = 0; i < size*3; i++) {
        file >> vertex_t[i];
    }
    file >> size;
    faces_size = size*9;
    faces = new GLint[size*9];
    for(int i = 0; i < size*9; i++) {
        file >> faces[i];
		
		
    }
}

void Fish::move(int j) {	
	mdl_center[j] = mdl_memory[j];// + offset[j];
	float time = float(Info::getTime())/500.0;
	_mdl_x[j] = 0+mdl_direction[j].x;
	_mdl_y[j] = 0+mdl_direction[j].y;
	_mdl_z[j] = sin(time*stamina_boost[j]+j)/5.0+mdl_direction[j].z;
	float mdl_x = _mdl_x[j];
	float mdl_y = _mdl_y[j];
	float mdl_z = _mdl_z[j];
	
    Matrix4x4 mdl_matx;
	mdl_matx.set(1,0,0,0,
                   0,cos(mdl_x),-sin(mdl_x),0,
                   0,sin(mdl_x),cos(mdl_x),0,
                   0,0,0,1);
    Matrix4x4 mdl_maty;
	mdl_maty.set(cos(mdl_y),0,sin(mdl_y),0,
                   0,1,0,0,
                    -sin(mdl_y),0,cos(mdl_y),0,
                   0,0,0,1);
    Matrix4x4 mdl_matz;
	mdl_matz.set(cos(mdl_z),-sin(mdl_z),0,0,
                    sin(mdl_z),cos(mdl_z),0,0,
                   0,0,1,0,
                   0,0,0,1);
    for(int i = 0; i < vertex_size; i+=3) {
        Vector3D mdl_tmp(vertex2[i],vertex2[i+1],vertex2[i+2]);
		//mdl_maty = mdl_matx.mult(mdl_maty);
		//mdl_matz = mdl_maty.mult(mdl_matz);

		mdl_tmp = mdl_matx.mult(mdl_tmp);
		mdl_tmp = mdl_maty.mult(mdl_tmp);
		mdl_tmp = mdl_matz.mult(mdl_tmp);

        mdl_tmp.x = (mdl_tmp.x*mdl_scale+mdl_pos[j].x+mdl_center[j].x);
        mdl_tmp.y = (mdl_tmp.y*mdl_scale+mdl_pos[j].y+mdl_center[j].y);
        mdl_tmp.z = (mdl_tmp.z*mdl_scale+mdl_pos[j].z+mdl_center[j].z);
		vertex[i] = mdl_tmp.x;
		vertex[i+1] = mdl_tmp.y;
		vertex[i+2] = mdl_tmp.z;

    }
	for(int i = 0; i < vertex_n_size; i+=3) {
        Vector3D mdl_tmp(vertex2_n[i],vertex2_n[i+1],vertex2_n[i+2]);

		mdl_tmp = mdl_matx.mult(mdl_tmp);
		mdl_tmp = mdl_maty.mult(mdl_tmp);
		mdl_tmp = mdl_matz.mult(mdl_tmp);

		vertex_n[i] = mdl_tmp.x;
		vertex_n[i+1] = mdl_tmp.y;
		vertex_n[i+2] = mdl_tmp.z;

    }
	
    mdl_center[j].x = (mdl_center[j].x+mdl_pos[j].x);
    mdl_center[j].y = (mdl_center[j].y+mdl_pos[j].y);
    mdl_center[j].z = (mdl_center[j].z+mdl_pos[j].z);
    mdl_pos[j].x = (0);
    mdl_pos[j].y = (0);
    mdl_pos[j].z = (0);
	
}

void Fish::AI(int i) {
	Vector3D player_pos = Camera::getPosition();
	float rnd = 1;
	float rnd2 = (rand()%1000 - 500.0)/500.0;
	if(rand()%1000 - 10 > 0) rnd = -1;
	if(stamina_boost[i] > 1.1) stamina_boost[i] -= 0.02;
	mdl_memory[i] += mdl_speed[i];
	//std::cout << mdl_memory[i].x << " " << mdl_memory[i].y << " " << mdl_memory[i].z << std::endl;
//	std::cout << "fish_num = " << i << " = " << Terrain::calc_height(mdl_memory[i]) << "\n";
	if (CalcTerHeight(mdl_memory[i]) + 1.5 > mdl_memory[i].z || CalcTerHeight(mdl_memory[i]) < -4.3 || dist(Vector3D(20, 90.5, -1), mdl_memory[i]) > 60.0) {
		while(1) {
			mdl_direction[i].z += 0.001*Info::GetElapsedTime();
			mdl_memory[i] -= mdl_speed[i];
			mdl_speed[i].x = cos(-mdl_direction[i].z)*stamina_boost[i] * 0.01*Info::GetElapsedTime();
			mdl_speed[i].y = sin(-mdl_direction[i].z)*stamina_boost[i] * 0.01*Info::GetElapsedTime();
			mdl_memory[i] += mdl_speed[i];
			if (!(CalcTerHeight(mdl_memory[i]) + 1.5 > mdl_memory[i].z)) return;
		}
	}
	
	else if(dist(player_pos,Vector3D(mdl_memory[i].x,mdl_memory[i].y,player_pos.z)) < 10.0 && stamina_boost[i] < 4.0) {
		stamina_boost[i] = 5.0;
		mdl_direction[i].z *= -1;
	}
	else if (rnd < 0) mdl_direction[i].z += 0.03*rnd2*Info::GetElapsedTime();
	mdl_memory[i] -= mdl_speed[i];
	//mdl_direction[i].z += 0.005*Info::elapsed_time;
	mdl_speed[i].x = cos(-mdl_direction[i].z)*stamina_boost[i] * 0.01*Info::GetElapsedTime();
	mdl_speed[i].y = sin(-mdl_direction[i].z)*stamina_boost[i] * 0.01*Info::GetElapsedTime();
	mdl_memory[i] += mdl_speed[i];
	
}
void Fish::Draw() {
	for(int i = 0; i < fish_num; i++) AI(i);	
	glEnable(GL_DEPTH_TEST);
	if(Info::GetShader()) {
		glUseProgram(shaderprogram);
		setShader();
	}
	

	for(int i = 0; i < fish_num; i++) {
		move(i);
		glBegin(GL_TRIANGLES);
		for(int i = 0; i < faces_size; i+=9) {
	        glNormal3f(vertex_n[(faces[i+2]-1)*3],vertex_n[(faces[i+2]-1)*3+1],vertex_n[(faces[i+2]-1)*3+2]);
			glTexCoord2d(vertex_t[(faces[i+1]-1)*3],vertex_t[(faces[i+1]-1)*3+1]);
			glVertex3f(vertex[(faces[i]-1)*3],vertex[(faces[i]-1)*3+1],vertex[(faces[i]-1)*3+2]);
	
	
			glNormal3f(vertex_n[(faces[i+5]-1)*3],vertex_n[(faces[i+5]-1)*3+1],vertex_n[(faces[i+5]-1)*3+2]);
			glTexCoord2d(vertex_t[(faces[i+4]-1)*3],vertex_t[(faces[i+4]-1)*3+1]);
			glVertex3f(vertex[(faces[i+3]-1)*3],vertex[(faces[i+3]-1)*3+1],vertex[(faces[i+3]-1)*3+2]);
	
	
			glNormal3f(vertex_n[(faces[i+8]-1)*3],vertex_n[(faces[i+8]-1)*3+1],vertex_n[(faces[i+8]-1)*3+2]);
			glTexCoord2d(vertex_t[(faces[i+7]-1)*3],vertex_t[(faces[i+7]-1)*3+1]);
			glVertex3f(vertex[(faces[i+6]-1)*3],vertex[(faces[i+6]-1)*3+1],vertex[(faces[i+6]-1)*3+2]);
	
	
	    }
	    glEnd();
	    
	}
	glUseProgram(0);
	glDisable(GL_DEPTH_TEST);

}

