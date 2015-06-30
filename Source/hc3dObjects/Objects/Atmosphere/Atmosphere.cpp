#include "StdAfx.h"
#include "Atmosphere.h"
#include "../Terrain/Terrain.h"

#define pi 3.1415

using namespace hc3d;

Vector3D Atmosphere::sun = Vector3D(0,0,0);
Vector3D Atmosphere::sun2 = Vector3D(0,0,0);

Atmosphere::Atmosphere(void)
{

}

Vector3D Atmosphere::getSun(){
		return sun;
}

Vector3D Atmosphere::getSun2(){
	return sun2;
}

void Atmosphere::SunInit() {
	sun.Set(2000,0,0);
	sun2.Set(2000,0,0);
}

void Atmosphere::Init(){
	shaderprogram = LoadShader("Shaders/atmosphere.vert","Shaders/atmosphere.frag");
	Info::SetLongitude(45 * pi / 180);
	Info::SetLatitude(-40 * pi / 180);
	SunInit();
	model.no_shaders();
	model.init("Models/moon.obj","null","null","null"/*,32000.0*/);

	model1.no_shaders();
	model1.init("Models/milkyway.obj","null","null","null"/*,1000002.0*/);

	model2.enable_spec();
	model2.init("Models/planet.obj","Texture/planet.png","Texture/planet_normal.png","Texture/planet_height.png"/*,1.0/18.0*/);

	model3.no_shaders();
	model3.init("Models/planet.obj","null","null","null"/*,1.01/18.0*/);
	glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
	Info::RaiseCurTextureNum();
	tex = LoadTexture("Texture/europa.png");
	glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
	Info::RaiseCurTextureNum();
	tex2 = LoadTexture("Texture/ganymede.png");
	glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
	Info::RaiseCurTextureNum();
	tex3 = LoadTexture("Texture/callisto.png");
	glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
	Info::RaiseCurTextureNum();
	tex1 = LoadTexture("Texture/milky.png");
	shaderprogram2 = LoadShader("Shaders/moon.vert","Shaders/moon.frag");
	shaderprogram3 = LoadShader("Shaders/moon.vert","Shaders/milky.frag");
}
	
void Atmosphere::Draw(){
		static float m1 = -110.0, m2 = 50.0, m3 = -120.0, m4 = 0.0;
		m1 += Info::GetElapsedTime() * 0.0012; // ~ 12 days full cycle
		m2 += Info::GetElapsedTime() * 0.0008; // ~ 18 days full cycle
		m3 += Info::GetElapsedTime() * 0.00095; // ~ 14 days full cycle
		float perspective_threshold = 1.0;
		SKY_SIZE = 450000000.0 * Info::GetGlobalScale();
		double glob_scale = Info::GetEarthZ() / 100.0 + 1.0;
		glScaled(1.0 / glob_scale, 1.0 / glob_scale, 1.0 / glob_scale);
		Vector3D position = Camera::getPosition();
		SunInit();
		sun = SunCalc(position, Info::GetLongitude(), Info::GetLatitude());
		Info::SetSun(sun);
		float x = position.x;
	    float y = position.y;
	    float z = position.z;
		glEnable(GL_DEPTH_TEST);
	    glUseProgram(shaderprogram);
	    SetShader(shaderprogram);
		//
	    glColor3f(1, 0, 0);
	    glBegin(GL_QUADS);
	    glNormal3f(0,0,-1);
	    glTexCoord2d(0,1);
	    glVertex3f(x-SKY_SIZE,y+SKY_SIZE,z+SKY_SIZE);
	    glTexCoord2d(0,0);
	    glNormal3f(0,0,-1);
	    glVertex3f(x+SKY_SIZE,y+SKY_SIZE,z+SKY_SIZE);
	    glTexCoord2d(1,0);
	    glNormal3f(0,0,-1);
	    glVertex3f(x+SKY_SIZE,y-SKY_SIZE,z+SKY_SIZE);
	    glTexCoord2d(1,1);
	    glNormal3f(0,0,-1);
	    glVertex3f(x-SKY_SIZE,y-SKY_SIZE,z+SKY_SIZE);
	    //
	    glNormal3f(-1,0,0);
	    glTexCoord2d(0,1);
	    glVertex3f(x+SKY_SIZE,y+SKY_SIZE,z+SKY_SIZE);
	    glTexCoord2d(0,0);
	    glNormal3f(-1,0,0);
	    glVertex3f(x+SKY_SIZE,y+SKY_SIZE,z-SKY_SIZE);
	    glTexCoord2d(1,0);
	    glNormal3f(-1,0,0);
	    glVertex3f(x+SKY_SIZE,y-SKY_SIZE,z-SKY_SIZE);
	    glTexCoord2d(1,1);
	    glNormal3f(-1,0,0);
	    glVertex3f(x+SKY_SIZE,y-SKY_SIZE,z+SKY_SIZE);
	    //
	    glNormal3f(0,1,0);
	    glTexCoord2d(0,1);
	    glVertex3f(x+SKY_SIZE,y-SKY_SIZE,z+SKY_SIZE);
	    glTexCoord2d(0,0);
	    glNormal3f(0,1,0);
	    glVertex3f(x+SKY_SIZE,y-SKY_SIZE,z-SKY_SIZE);
	    glTexCoord2d(1,0);
	    glNormal3f(0,1,0);
	    glVertex3f(x-SKY_SIZE,y-SKY_SIZE,z-SKY_SIZE);
	    glTexCoord2d(1,1);
	    glNormal3f(0,1,0);
	    glVertex3f(x-SKY_SIZE,y-SKY_SIZE,z+SKY_SIZE);
	    //
	    glNormal3f(0,-1,0);
	    glTexCoord2d(0,1);
	    glVertex3f(x-SKY_SIZE,y+SKY_SIZE,z+SKY_SIZE);
	    glTexCoord2d(0,0);
	    glNormal3f(0,-1,0);
	    glVertex3f(x-SKY_SIZE,y+SKY_SIZE,z-SKY_SIZE);
	    glTexCoord2d(1,0);
	    glNormal3f(0,-1,0);
	    glVertex3f(x+SKY_SIZE,y+SKY_SIZE,z-SKY_SIZE);
	    glTexCoord2d(1,1);
	    glNormal3f(0,-1,0);
	    glVertex3f(x+SKY_SIZE,y+SKY_SIZE,z+SKY_SIZE);
	    //
	    glNormal3f(1,0,0);
	    glTexCoord2d(0,1);
	    glVertex3f(x-SKY_SIZE,y-SKY_SIZE,z+SKY_SIZE);
	    glTexCoord2d(0,0);
	    glNormal3f(1,0,0);
	    glVertex3f(x-SKY_SIZE,y-SKY_SIZE,z-SKY_SIZE);
	    glTexCoord2d(1,0);
	    glNormal3f(1,0,0);
	    glVertex3f(x-SKY_SIZE,y+SKY_SIZE,z-SKY_SIZE);
	    glTexCoord2d(1,1);
	    glNormal3f(1,0,0);
	    glVertex3f(x-SKY_SIZE,y+SKY_SIZE,z+SKY_SIZE);


	    glNormal3f(0,0,1);
	    glTexCoord2d(0,1);
	    glVertex3f(x+SKY_SIZE,y+SKY_SIZE,z-SKY_SIZE);
	    glTexCoord2d(0,0);
	    glNormal3f(0,0,1);
	    glVertex3f(x-SKY_SIZE,y+SKY_SIZE,z-SKY_SIZE);
	    glTexCoord2d(1,0);
	    glNormal3f(0,0,1);
	    glVertex3f(x-SKY_SIZE,y-SKY_SIZE,z-SKY_SIZE);
	    glTexCoord2d(1,1);
	    glNormal3f(0,0,1);
	    glVertex3f(x+SKY_SIZE,y-SKY_SIZE,z-SKY_SIZE);

		glEnd();
		glUseProgram(0);
		glDisable(GL_DEPTH_TEST);

		
		Vector3D cam = Camera::getPosition();
		glPushMatrix();
		
		if (Info::GetGlobalScale() < perspective_threshold) glTranslated(cam.x, cam.y, cam.z);
		glRotatef(90-Info::GetLatitude()/pi*180,1,0,0);
		glRotatef((Info::GetLongitude())/pi*180 + m1,0,0,1);
		glTranslated(-80000000.0 * Info::GetGlobalScale(), 0, 0);
		
		glScaled(1.0 * 128000 * Info::GetGlobalScale(), 1.0 * 128000 * Info::GetGlobalScale(), 1.0 * 128000 * Info::GetGlobalScale());
		SunInit();
		sun = SunCalc(position, -m1/180.0*pi, 90/180.0*pi-Info::GetLatitude());
		glUseProgram(shaderprogram2);
		SetShader(shaderprogram2);
		glActiveTexture(GL_TEXTURE16);//заменить все числа
		glBindTexture(GL_TEXTURE_2D,tex);
	    glUniform1i(glGetUniformLocation(
	       		shaderprogram2, "tex"), 16);
		model.draw();
		glUseProgram(0);
		glPopMatrix();

		glPushMatrix();
		
		
		if (Info::GetGlobalScale() < perspective_threshold) glTranslated(cam.x, cam.y, cam.z);
		glRotatef((90-(Info::GetLatitude())/pi*180),1,0,0);
		glRotatef((Info::GetLongitude())/pi*180 + m2,0,0,1);
		glTranslated(-80000000.0 * Info::GetGlobalScale(), 0, 0);
		glScaled(2.0 * 128000 * Info::GetGlobalScale(), 2.0 * 128000 * Info::GetGlobalScale(), 2.0 * 128000 * Info::GetGlobalScale());
		SunInit();
		sun = SunCalc(position, -m2/180.0*pi, 90/180.0*pi-Info::GetLatitude());
		glUseProgram(shaderprogram2);
		SetShader(shaderprogram2);
		glActiveTexture(GL_TEXTURE18);
		glBindTexture(GL_TEXTURE_2D,tex2);
	    glUniform1i(glGetUniformLocation(
	       		shaderprogram2, "tex"), 18);
		model.draw();
		glUseProgram(0);
		glPopMatrix();

		glPushMatrix();
		
		if (Info::GetGlobalScale() < perspective_threshold) glTranslated(cam.x, cam.y, cam.z);
		glRotatef((90-(Info::GetLatitude())/pi*180),1,0,0);
		glRotatef((Info::GetLongitude())/pi*180 + m3,0,0,1);
		glTranslated(-80000000.0 * Info::GetGlobalScale(), 0, 0);
		glScaled(0.7 * 128000 * Info::GetGlobalScale(), 0.5 * 128000 * Info::GetGlobalScale(), 0.5 * 128000 * Info::GetGlobalScale());
		SunInit();
		sun = SunCalc(position,-m3/180.0*pi, 90/180.0*pi-Info::GetLatitude());
		glUseProgram(shaderprogram2);
		SetShader(shaderprogram2);
		glActiveTexture(GL_TEXTURE19);
		glBindTexture(GL_TEXTURE_2D,tex3);
	    glUniform1i(glGetUniformLocation(
	       		shaderprogram2, "tex"), 19);
		model.draw();
		glUseProgram(0);
		glPopMatrix();

		glPushMatrix();
		glTranslated(cam.x,cam.y,cam.z);
		glRotatef((90-(Info::GetLatitude())/pi*180),1,0,0);
		glRotatef((Info::GetLongitude()) / pi * 180 + m4, 0, 0, 1);
		glScaled(4000000.0 * Info::GetGlobalScale(), 4000000.0 * Info::GetGlobalScale(), 4000000.0 * Info::GetGlobalScale());
		glUseProgram(shaderprogram3);
		SetShader(shaderprogram3);
		glActiveTexture(GL_TEXTURE17);
		glBindTexture(GL_TEXTURE_2D,tex1);
	    glUniform1i(glGetUniformLocation(
	       		shaderprogram3, "tex"), 17);
		model1.draw();
		glUseProgram(0);
		glPopMatrix();

		SunInit();
		sun = SunCalc(position,Info::GetLongitude(), Info::GetLatitude());

		glPushMatrix();
		glTranslated(31000.0, 1054000.0, -4350000.0 + 5128000 + Info::GetEarthZ());
		glRotatef((20+Info::GetLatitude())/pi*180,1,0,0);
		glScaled(3000000.0,3000000.0,3000000.0);
	//	model2.draw();

		glUseProgram(shaderprogram);
	    SetShader(shaderprogram);
	//	model3.draw();
		glUseProgram(0);
		glPopMatrix();
		//glScaled(glob_scale, glob_scale, glob_scale);
	}
	
	 void Atmosphere::SetShader(GLuint shaderprogram){
			Vector3D position = Camera::getPosition();
			if(Info::GetReflect()) position.z *= -1.0;
	        glUniform1f(glGetUniformLocation(
	        		shaderprogram, "poz"), position.z);
	        glUniform3f(glGetUniformLocation(
	        		shaderprogram, "pozit"), position.x
	        		, position.y, position.z);
	        glUniform3f(glGetUniformLocation(
	        		shaderprogram, "sunz"), sun.x, sun.y, sun.z);
			glUniform3f(glGetUniformLocation(
	        		shaderprogram, "sunz2"), sun2.x, sun2.y, sun2.z);
			glUniform3f(glGetUniformLocation(
				shaderprogram, "light"), light.x,light.y,light.z);
			int refl = 0;
			if (Info::GetReflect()) refl = 1;
			glUniform1i(glGetUniformLocation(
				shaderprogram, "reflection"), refl);
	 }
	 Vector3D Atmosphere::SunCalc(Vector3D player,float longitude,float latitude) {
		longitude -= 0.0;//(0.01*pi/180);//if you want real-time atmosphere
	    Vector3D sun1;
		light = Vector3D(2000,0,0);
	    float angle = 0;
	    Matrix4x4 matx;
	    matx.set(1,0,0,0,
	             0,cos(Info::GetLatitude()),-sin(Info::GetLatitude()),0,
                 0,sin(Info::GetLatitude()),cos(Info::GetLatitude()),0,
	             0,0,0,1);
	    Matrix4x4 maty;
	    maty.set(cos(Info::GetLongitude()),0,sin(Info::GetLongitude()),0,
	                   0,1,0,0,
	                   -sin(Info::GetLongitude()),0,cos(Info::GetLongitude()),0,
	                   0,0,0,1);
	    
	    Matrix4x4 matz;
	    matz.set(cos(angle),-sin(angle),0,0,
	    		sin(angle),cos(angle),0,0,
	                   0,0,1,0,
	                   0,0,0,1);
		Matrix4x4 multi = matx.mult(maty);
		multi = multi.mult(matz);
	    sun1 = multi.mult(sun);
		sun2 = multi.mult(sun2);

		matx.set(1,0,0,0,
			0, cos(latitude + 1.5*Info::GetLatitude()), -sin(latitude + 1.5*Info::GetLatitude()), 0,
			0, sin(latitude + 1.5*Info::GetLatitude()), cos(latitude + 1.5*Info::GetLatitude()), 0,
	             0,0,0,1);
		maty.set(cos(longitude),0,sin(longitude),0,
	             0,1,0,0,
	             -sin(longitude),0,cos(longitude),0,
	             0,0,0,1);
		multi = matx.mult(maty);
		light = multi.mult(light);
	    return sun1;
	}
