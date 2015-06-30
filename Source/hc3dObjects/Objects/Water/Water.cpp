#include "StdAfx.h"
#include "Water.h"
#include "hc3dObjects.h"
#include "hc3dMath.h"


using namespace hc3d;

Water::Water(void)
{
}


Water::~Water(void)
{
}

GLuint* Water::texture;

void Water::reflect() {

	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D,texture[0]);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, Info::GetMaxWater(), Info::GetMaxWater(), 0);
}

void Water::refract() {

	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, Info::width(), Info::height()/*Info::GetMaxWater(), Info::GetMaxWater()*/, 0);

}

void Water::setShader() {
			Vector3D pos = Camera::getPosition();
			Vector3D sun = Info::GetSun();
			
			glActiveTexture(GL_TEXTURE8);
			glBindTexture(GL_TEXTURE_2D, texture[0]);
	        glUniform1i(glGetUniformLocation(
				shaderprogram, "envMap"), 8);

			glActiveTexture(GL_TEXTURE0 + Info::GetCurBindTexture());
			glBindTexture(GL_TEXTURE_2D, texture[2]);
			glUniform1i(glGetUniformLocation(
				shaderprogram, "normalMap"), Info::GetCurBindTexture());
			Info::RaiseCurBindTexture();

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

			glActiveTexture(GL_TEXTURE0 + Info::GetCurBindTexture());
	        glBindTexture(GL_TEXTURE_2D, texture[3]);
			glUniform1i(glGetUniformLocation(
				shaderprogram, "waterTex"), Info::GetCurBindTexture());
			Info::RaiseCurBindTexture();

			glActiveTexture(GL_TEXTURE9);
			glBindTexture(GL_TEXTURE_2D, texture[1]);
	        glUniform1i(glGetUniformLocation(
				shaderprogram, "envMap2"), 9);
	    
	        int time = glGetUniformLocation(shaderprogram, "time");
			float tt = Info::getTime()/5000.0;
	        glUniform1f(time, tt);
			const int shadowing = Info::GetShadow() ? 1 : 0;
			glUniform1i(glGetUniformLocation(
				shaderprogram, "shadowing"), shadowing);
	        glUniform1i(glGetUniformLocation(
	        		shaderprogram, "waving"), 1);
	        glUniform1f(glGetUniformLocation(
	        		shaderprogram, "waterHeight"), 0);
	        glUniform1f(glGetUniformLocation(
	        		shaderprogram, "wavenum"), 1);
	        glUniform1i(glGetUniformLocation(
	        		shaderprogram, "numWaves"), 1);
	        glUniform1f(glGetUniformLocation(
				shaderprogram, "Height"), Info::height());
		    glUniform1f(glGetUniformLocation(
				shaderprogram, "Width"), Info::width());
	        glUniform3f(glGetUniformLocation(
	        		shaderprogram, "eyePos"), pos.x, pos.y, pos.z);
			glUniform3f(glGetUniformLocation(
				shaderprogram, "atmosphere"), Info::GetAtmoColor().x, Info::GetAtmoColor().y, Info::GetAtmoColor().z);
	        glUniform3f(glGetUniformLocation(
	        		shaderprogram, "lightPos"), sun.x, sun.y, sun.z);
			glUniform1i(glGetUniformLocation(
	        		shaderprogram, "flash"), Info::GetFlash());
			glUniform3f(glGetUniformLocation(
				shaderprogram, "eyeNorm"), Info::GetEyeNormal().x, Info::GetEyeNormal().y, Info::GetEyeNormal().z);

}


float Water::dist(Vector3D a, Vector3D b) {
	Vector3D c = a - b;
	return sqrt(c.x *c.x + c.y *c.y);
}

void Water::Init() {
	list_id = 0;
	texture = new GLuint[4];

	glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
	Info::RaiseCurTextureNum();
	texture[0] = CreateTexture(GL_RGBA,Info::GetMaxWater());
	glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
	Info::RaiseCurTextureNum();
	texture[1] = CreateTexture(GL_RGBA, Info::GetMaxWater());
	glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
	Info::RaiseCurTextureNum();
	texture[2] = LoadTexture("Texture/waterNormal2.png");
	glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
	Info::RaiseCurTextureNum();
	texture[3] = LoadTexture("Texture/waterTex.png");
	shaderprogram = LoadShader("Shaders/water.vert","Shaders/water.frag");
	static_lists = new int*[1024];
	for (int i = 0; i < 1024; i++) {
		static_lists[i] = new int[6];//64x64 6 levels
	}
	list_center = new Vector3D[1024];//numbers -> variables

	for (int i = 0; i < 1024; i++) {
		CenterGenerate(i);
	}
	for (int i = 0; i < 1024; i++) {
		for (int j = 0; j < 6; j++) {
			static_lists[i][j] = 0;
		}
	}
}

void Water::CenterGenerate(int num) {
	int level = 0;
	int size = 2;
	int i_start = 0 + 64 * (num % 32);
	int i_end = hc3d::Math::hc3dMin(64 + 64 * (num % 32), 2048 - int(pow(2.0, level)));
	int j_start = 0 + 64 * (num / 32);
	int j_end = hc3d::Math::hc3dMin(64 + 64 * (num / 32), 2048 - int(pow(2.0, level)));
	list_center[num] = Vector3D((i_start + i_end) / 2 * size, (j_start + j_end) / 2 * size, CalcTerHeight(Vector3D((i_start + i_end) / 2 * size, (j_start + j_end) / 2 * size, 0)));
}

void Water::LodGenerate(int num, int level) {
	glPushMatrix();
	glLoadIdentity();
	int i_start = 0 + 64 * (num % 32);
	int i_end = 64 + 64 * (num % 32);
	int j_start = 0 + 64 * (num / 32);
	int j_end = 64 + 64 * (num / 32);
	if (level % 2 == 1) {
		i_start = hc3d::Math::hc3dMax(0, i_start - level);
		j_start = hc3d::Math::hc3dMax(0, j_start - level);
		i_end = hc3d::Math::hc3dMin(2048 - int(pow(2.0, level)), i_end + level);
		j_end = hc3d::Math::hc3dMin(2048 - int(pow(2.0, level)), j_end + level);
	}

	i_start += Camera::getPosition().x;
	i_end += Camera::getPosition().x;
	j_start += Camera::getPosition().y;
	j_end += Camera::getPosition().y;
	int size = 2; // make class variable
//	static_lists[num][level] = glGenLists(1);
//	glNewList(static_lists[num][level], GL_COMPILE);
	level = int(pow(2.0, level));
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glBegin(GL_TRIANGLES);
	float hei;
	for (int i = i_start; i < i_end; i += level) {
		for (int j = j_start; j < j_end; j += level) {
			i *= size;
			j *= size;
			level *= size;
			const float xy0 = CalcTerHeight(Vector3D(i, j, 0));
			if (level == 2.0) {
				if (xy0 > 10.0) {
					i /= size;
					j /= size;
					level /= size;
					continue;
				}
			}
			else if (xy0 > 40.0) {
				i /= size;
				j /= size;
				level /= size;
				continue;
			}

			if (level == 2.0) hei = xy0 / 100.0;  else hei = -10.0;
			glNormal3f(0, 1.0, hei);//this is not really normal. just using attribute
			glTexCoord2d(i, j);
			glVertex3f(i, j, 0);

			const float xPolY0 = CalcTerHeight(Vector3D(i + level, j, 0)) / 100.0;
			const float xYPol0 = CalcTerHeight(Vector3D(i, j + level, 0)) / 100.0;
			const float xPolYPol0 = CalcTerHeight(Vector3D(i + level, j + level, 0)) / 100.0;

			if (level == 2.0) hei = xPolY0; else hei = -10.0;
			glNormal3f(0, 1.0, hei);
			glTexCoord2d(i + level, j);
			glVertex3f(i + level, j, 0);

			if (level == 2.0) hei = xYPol0; else hei = -10.0;
			glNormal3f(0, 1.0, hei);
			glTexCoord2d(i, (j + level));
			glVertex3f(i, j + level, 0);

			if (level == 2.0) hei = xYPol0; else hei = -10.0;
			glNormal3f(0, 1.0, hei);
			glTexCoord2d(i, (j + level));
			glVertex3f(i, j + level, 0);

			if (level == 2.0) hei = xPolY0; else hei = -10.0;
			glNormal3f(0, 1.0, hei);
			glTexCoord2d((i + level), j);
			glVertex3f(i + level, j, 0);

			if (level == 2.0) hei = xPolYPol0; else hei = -10.0;
			glNormal3f(0, 1.0, hei);
			glTexCoord2d((i + level), (j + level));
			glVertex3f(i + level, j + level, 0);

			// back


			if (level == 2.0) hei = xy0 / 100.0; else hei = -10.0;
			glNormal3f(0, -1.0, hei);
			glTexCoord2d(i, j);
			glVertex3f(i, j, 0);

			if (level == 2.0) hei = xYPol0; else hei = -10.0;
			glNormal3f(0, -1.0, hei);
			glTexCoord2d(i, (j + level));
			glVertex3f(i, j + level, 0);

			if (level == 2.0) hei = xPolY0; else hei = -10.0;
			glNormal3f(0, -1.0, hei);
			glTexCoord2d(i + level, j);
			glVertex3f(i + level, j, 0);

			if (level == 2.0) hei = xYPol0; else hei = -10.0;
			glNormal3f(0, -1.0, hei);
			glTexCoord2d(i, (j + level));
			glVertex3f(i, j + level, 0);

			if (level == 2.0) hei = xPolYPol0; else hei = -10.0;
			glNormal3f(0, -1.0, hei);
			glTexCoord2d((i + level), (j + level));
			glVertex3f(i + level, j + level, 0);

			if (level == 2.0) hei = xPolY0; else hei = -10.0;
			glNormal3f(0, -1.0, hei);
			glTexCoord2d((i + level), j);
			glVertex3f(i + level, j, 0);
			i /= size;
			j /= size;
			level /= size;
		}
	}
	glEnd();
	glDisable(GL_DEPTH_TEST);
//	glEndList();
	glPopMatrix();
}

void Water::DrawMain() {
	glPushMatrix();
	glLoadIdentity();
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glBegin(GL_TRIANGLES);

	
	int polyNum = 8;
	int majorPolyNum = 32;
	int level = 8;
	int majorPoly = polyNum * level * 2;
	Vector3D offset = Vector3D(((int)Camera::getPosition().x / level) * level, ((int)Camera::getPosition().y / level) * level, ((int)Camera::getPosition().z / level) * level);

	int i_start = -polyNum * level + offset.x;
	int i_end = polyNum * level + offset.x;
	int j_start = -polyNum * level + offset.y;
	int j_end = polyNum * level + offset.y;

	int im_start = -majorPolyNum * majorPoly + i_start;
	int im_end = majorPolyNum * majorPoly + i_end;
	int jm_start = -majorPolyNum * majorPoly + j_start;
	int jm_end = majorPolyNum * majorPoly + j_end;
	float hei;
	
	for (int i = i_start; i < i_end; i += level) {
		for (int j = j_start; j < j_end; j += level) {
			const float xy0 = CalcTerHeight(Vector3D(i, j, 0));

			hei = xy0 / 100.0;
			glNormal3f(0, 1.0, hei);//this is not really normal. just using attribute
			glTexCoord2d(i, j);
			glVertex3f(i, j, 0);

			const float xPolY0 = CalcTerHeight(Vector3D(i + level, j, 0)) / 100.0;
			const float xYPol0 = CalcTerHeight(Vector3D(i, j + level, 0)) / 100.0;
			const float xPolYPol0 = CalcTerHeight(Vector3D(i + level, j + level, 0)) / 100.0;

			glNormal3f(0, 1.0, xPolY0);
			glTexCoord2d(i + level, j);
			glVertex3f(i + level, j, 0);

			glNormal3f(0, 1.0, xYPol0);
			glTexCoord2d(i, (j + level));
			glVertex3f(i, j + level, 0);

			glNormal3f(0, 1.0, xYPol0);
			glTexCoord2d(i, (j + level));
			glVertex3f(i, j + level, 0);

			glNormal3f(0, 1.0, xPolY0);
			glTexCoord2d((i + level), j);
			glVertex3f(i + level, j, 0);

			glNormal3f(0, 1.0, xPolYPol0);
			glTexCoord2d((i + level), (j + level));
			glVertex3f(i + level, j + level, 0);

			// back


			glNormal3f(0, -1.0, xy0 / 100.0);
			glTexCoord2d(i, j);
			glVertex3f(i, j, 0);

			glNormal3f(0, -1.0, xYPol0);
			glTexCoord2d(i, (j + level));
			glVertex3f(i, j + level, 0);

			glNormal3f(0, -1.0, xPolY0);
			glTexCoord2d(i + level, j);
			glVertex3f(i + level, j, 0);

			glNormal3f(0, -1.0, xYPol0);
			glTexCoord2d(i, (j + level));
			glVertex3f(i, j + level, 0);

			glNormal3f(0, -1.0, xPolYPol0);
			glTexCoord2d((i + level), (j + level));
			glVertex3f(i + level, j + level, 0);

			glNormal3f(0, -1.0, xPolY0);
			glTexCoord2d((i + level), j);
			glVertex3f(i + level, j, 0);
		}
	}
	//majorPoly
	level = majorPoly;
	for (int i = im_start; i < im_end; i += majorPoly) {
		for (int j = jm_start; j < jm_end; j += majorPoly) {

			if (i >= i_start && i < i_end && j >= j_start && j < j_end) continue;
			hei = -0.1;
			glNormal3f(0, 1.0, hei);//this is not really normal. just using attribute
			glTexCoord2d(i, j);
			glVertex3f(i, j, 0);

			glNormal3f(0, 1.0, hei);
			glTexCoord2d(i + level, j);
			glVertex3f(i + level, j, 0);

			glNormal3f(0, 1.0, hei);
			glTexCoord2d(i, (j + level));
			glVertex3f(i, j + level, 0);

			glNormal3f(0, 1.0, hei);
			glTexCoord2d(i, (j + level));
			glVertex3f(i, j + level, 0);

			glNormal3f(0, 1.0, hei);
			glTexCoord2d((i + level), j);
			glVertex3f(i + level, j, 0);

			glNormal3f(0, 1.0, hei);
			glTexCoord2d((i + level), (j + level));
			glVertex3f(i + level, j + level, 0);

			// back


			glNormal3f(0, 1.0, hei);
			glTexCoord2d(i, j);
			glVertex3f(i, j, 0);

			glNormal3f(0, 1.0, hei);
			glTexCoord2d(i, (j + level));
			glVertex3f(i, j + level, 0);

			glNormal3f(0, 1.0, hei);
			glTexCoord2d(i + level, j);
			glVertex3f(i + level, j, 0);

			glNormal3f(0, 1.0, hei);
			glTexCoord2d(i, (j + level));
			glVertex3f(i, j + level, 0);

			glNormal3f(0, 1.0, hei);
			glTexCoord2d((i + level), (j + level));
			glVertex3f(i + level, j + level, 0);

			glNormal3f(0, 1.0, hei);
			glTexCoord2d((i + level), j);
			glVertex3f(i + level, j, 0);
		}
	}
	
	glEnd();
	glDisable(GL_DEPTH_TEST);
	glPopMatrix();
}

void Water::Draw() {
    
    /*if(list_id){
        glCallList(list_id);
    }else {
        list_id=glGenLists(1);
        glNewList(list_id, GL_COMPILE_AND_EXECUTE);*/
	glEnable(GL_DEPTH_TEST);
    float dx,dy;
    dx=dy=1;	
	glEnable(GL_CULL_FACE);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(shaderprogram);
	setShader();
	DrawMain();
	glUseProgram(0);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
    /*glEndList();
    }*/

    
}
