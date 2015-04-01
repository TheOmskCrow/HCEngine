#include "StdAfx.h"
#include "Butterfly.h"
#include <fstream>
#include <iostream>
#include <string>
#include "hc3dObjects.h"
#include "hc3dMath.h"
#include "../Water/Water.h"

#define GROUND_OFFSET 0.2

using namespace hc3d;

Butterfly::Butterfly(void)
{
}


Butterfly::~Butterfly(void)
{
}

float Butterfly::distance(Vector3D a, Vector3D b) {
    Vector3D c;
    c.x = (a.x  - b.x );
    c.y = (a.y - b.y);
    c.z = (a.z - b.z);
    return (float) sqrt((c.x *c.x  + c.y*c.y + c.z*c.z));
}
Vector3D* Butterfly::qSort(Vector3D *A, int low, int high) {
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

void Butterfly::Init(){
//		model = new Model[3];
//		model[0].init("Models/but1.obj","null","null","null");
//		model[1].init("Models/but2.obj","null","null","null");
//		model[2].init("Models/but3.obj","null","null","null");
        but_num = 5000;
        offset = 0.0;
        but = new Vector3D[but_num];
		angle = new float[but_num];
        cadr = new float[but_num];
		gr_size = new float[but_num];
		sitting = new bool[but_num];
		reverse = new bool[but_num];
		hue = new float[but_num];
		hue_speed = new float[but_num];
		blink_speed = new float[but_num];
		size = new float[but_num];
		but_yaw = new float[but_num];
		rotating = new bool[but_num];
		rotate_angle = new float[but_num];
        but_init();
		glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
		Info::RaiseCurTextureNum();
		tex = LoadTexture("Texture/butter.png");
		glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
		Info::RaiseCurTextureNum();
		tex2 = LoadTexture("Texture/butter2.png");
		shaderprogram = LoadShader("Shaders/but.vert","Shaders/but.frag");
}
float Butterfly::rnd() {
    return float(rand()%100000)/100000.0;
}
void Butterfly::but_init(){
        for(int i = 0; i < but_num; i++) {
			while(1) {
            Vector3D data;
            data.x = ((rand()%40900)/10.0);
            data.y = ((rand()%40900)/10.0);
			data.z = CalcTerHeight(Vector3D(data.x, data.y, 0.0));
			float but_coeff = hc3d::Math::hc3dMax(0.0, hc3d::Math::hc3dMin(1.0, (CalcTerNormal(Vector3D(data.x, data.y, 0.0))*Vector3D(0.0, 0.0, 1.0))));
			but_coeff -= 0.975;
			but_coeff *= 40.0;
			data.z += 4.0;
            but[i] = data;
			sitting[i] = false;
			reverse[i] = false;
			but_yaw[i] = 45.0;
			rotating[i] = false;
			data.z -= 4.0;
			angle[i] = rnd()*360.0;
            cadr[i] = rnd()*360.0;
			size[i] = float(rand()%1000)/1000.0 + 0.3;
			//std::cout << size[i] << " ";
			blink_speed[i] = (1.3 - size[i] + 0.3)*2.0;
			//std::cout << blink_speed[i] << std::endl;
			hue[i] = rnd();
			gr_size[i] = rnd();
			if(gr_size[i] < 0.25) hue_speed[i] = rnd() / 10.0;
			else hue_speed[i] = 0.0;
			if(data.z < 120.0 && data.z > 6.0) {
			//	printf("x = %.2f y = %.2f z = %.2f\n",data.x,data.y,data.z);
				break; 
			}
			}
        }
    }
void Butterfly::setShader() {
			//Shadow shadow;
			glUniform1i(glGetUniformLocation(
				shaderprogram, "shadowing"), Info::GetShadow());
			glUniform1f(glGetUniformLocation(
				shaderprogram, "offset"), offset);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, Shadow::GetShadowMap()[0]);
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
				shaderprogram, "shadowDist"), Info::GetShadowDist());
			glUniform1f(glGetUniformLocation(
				shaderprogram, "Width"), Info::width());
			glUniform1f(glGetUniformLocation(
				shaderprogram, "Height"), Info::height());

			glActiveTexture(GL_TEXTURE0 + Info::GetCurBindTexture());
	        glBindTexture(GL_TEXTURE_2D, tex);
			glUniform1i(glGetUniformLocation(
				shaderprogram, "model"), Info::GetCurBindTexture());
			Info::RaiseCurBindTexture();

			glUniform3f(glGetUniformLocation(
				shaderprogram, "atmosphere"), Info::GetAtmoColor().x, Info::GetAtmoColor().y, Info::GetAtmoColor().z);
	        
			glActiveTexture(GL_TEXTURE0 + Info::GetCurBindTexture());
	        glBindTexture(GL_TEXTURE_2D, tex2);
	        glUniform1i(glGetUniformLocation(
				shaderprogram, "model2"), Info::GetCurBindTexture());
			Info::RaiseCurBindTexture();

			Vector3D sun = (Info::GetSun());
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
void Butterfly::Draw() {
        //        but = qSort(but,0,but_num-1);
				//TODO: define yaw as identifier in Info
				float yaw = 0;
				if (!Info::GetShader() || Terrain::refract || Terrain::reflect) yaw = 3.0;
				if(Info::GetShader()) {
					glUseProgram(shaderprogram);
					setShader();
				}
				bool tmp = Info::GetShader();
				Info::SetShader(false);
				glEnable(GL_DEPTH_TEST);
                glEnable(GL_ALPHA_TEST);
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
                //glDepthMask(false);
				if(yaw == 0) offset+=(rnd()/5.0 + 0.05)*Info::GetElapsedTime();
                float pi = 3.141592;
				Vector3D player = Camera::getPosition();
				int samples = 0;
                for(int i = 0; i < but_num; i++) {
					//AI
					float dst = distance(but[i], player);
					if(dst > 400.0) continue;
					if (yaw == 0) {
						hue[i] += hue_speed[i] * Info::GetElapsedTime();
						if (hue[i] > 1.0) hue[i] -= 1.0;
						float curGround = hc3d::Math::hc3dMax(CalcTerHeight(Vector3D(but[i].x, but[i].y, 0.0)), 0.0) + 4.0;
						float but_speed = 0.5;
						if (but[i].z > 40.0) {
							if (!reverse[i]) {
								angle[i] *= -1.0;
								reverse[i] = true;
							}
						}
						else if (but[i].z < 39.0) reverse[i] = false;

						if (rnd() < 0.005) {
							if (rotating[i]) rotating[i] = false;
							else {//starting rotate must be rarely than stopping
								rotating[i] = true;
								rotate_angle[i] = (rnd() - 0.5)*2.0;
							}
						}
						if (rotating[i]) angle[i] += rotate_angle[i] * Info::GetElapsedTime();
						if (rnd() < 0.002) if (curGround != 4.0) {
							if (sitting[i] && Info::GetSun().z > 0.0) sitting[i] = false;//if night - butters sleep
							else sitting[i] = true;
						}
						if (!sitting[i]) {
							but[i] += Vector3D(cos(angle[i] * pi / 180.0 + pi / 2.0)*Info::GetElapsedTime()*but_speed,
								sin(angle[i] * pi / 180.0 + pi / 2.0)*Info::GetElapsedTime()*but_speed,
								0);
							but_yaw[i] += Info::GetElapsedTime()*2.0;
							but_yaw[i] = hc3d::Math::hc3dMin(but_yaw[i], 45.0);
							but[i].z = hc3d::Math::hc3dMax(but[i].z, curGround - 4.0);
							but[i].z = hc3d::Math::hc3dMin(but[i].z, curGround + 6.0);
							if (but[i].z < curGround - 0.5) but[i].z += 0.1*Info::GetElapsedTime();
							else if (but[i].z > curGround + 0.5) but[i].z -= 0.1*Info::GetElapsedTime();
							cadr[i] += 2.0*Info::GetElapsedTime()*blink_speed[i];
						}
						else {//sitting
							if (curGround != 4.0) {
								if (but[i].z > curGround - 4.0 + GROUND_OFFSET) {
									but[i] += Vector3D(cos(angle[i] * pi / 180.0 + pi / 2.0)*Info::GetElapsedTime()*but_speed,
										sin(angle[i] * pi / 180.0 + pi / 2.0)*Info::GetElapsedTime()*but_speed,
										0);
									but[i].z -= 0.1*Info::GetElapsedTime();
									but_yaw[i] -= Info::GetElapsedTime()*2.0;
									but_yaw[i] = hc3d::Math::hc3dMax(but_yaw[i], 0.0);
									cadr[i] += 2.0*Info::GetElapsedTime()*blink_speed[i];
								}
								else {
									but[i].z = curGround - 4.0 + GROUND_OFFSET;
									cadr[i] += 0.3*Info::GetElapsedTime();
								}
							}
							else sitting[i] = false;
						}
					}
					if (dst < 25.0 && (Camera::GetCamSpeed() * Camera::GetCamSpeed()) != 0.0) sitting[i] = false; //more reason to scare butters
					//end AI
					
					
					if(dst > 300.0) continue;
					Vector3D a = but[i] - Camera::getPosition();
					a.Normalize();
					Vector3D b = Info::GetEyeNormal();
					b.Normalize();
					float norm_vec = a*b;
					if(norm_vec < 0.7 && dst > 10.0) continue;
					glPushMatrix();
					glTranslated(but[i].x,but[i].y,but[i].z);
					glRotatef(angle[i],0,0,1);
					glRotatef(but_yaw[i],1,0,0);
					glScalef(0.75 * size[i],0.75 * size[i],0.75 * size[i]);
					Vector3D A(-0.75,0.5,0.0),
							 B(-0.75,-0.5,0.0),
							 C(0.0,0.5,0.0),
							 D(0.0,-0.5,0.0),
							 E(0.75,0.5,0.0),
							 F(0.75,-0.5,0.0);
					A.z += 0.5*sin(cadr[i]);
					B.z += 0.5*sin(cadr[i]);
					E.z += 0.5*sin(cadr[i]);
					F.z += 0.5*sin(cadr[i]);
					glUniform1f(glGetUniformLocation(
						shaderprogram, "tx"), gr_size[i]);
					glUniform1f(glGetUniformLocation(
						shaderprogram, "hue"), hue[i]);

					//std::cout << hue[i] << " " << hue_speed[i] << std::endl;
				///	printf("but = %.2f\n",gr_size[i]);
					glBegin(GL_TRIANGLES);
						glNormal3f(gr_size[i],0.0,1.0);
						glTexCoord2d(0.0,0.0);
						glVertex3f(A.x,A.y,A.z);

						glNormal3f(gr_size[i],0.0,1.0);
						glTexCoord2d(0.0,1.0);
						glVertex3f(B.x,B.y,B.z);

						glNormal3f(gr_size[i],0.0,1.0);
						glTexCoord2d(0.5,1.0);
						glVertex3f(D.x,D.y,D.z);
	
						//
						glNormal3f(gr_size[i],0.0,1.0);
						glTexCoord2d(0.0,0.0);
						glVertex3f(A.x,A.y,A.z);

						glNormal3f(gr_size[i],0.0,1.0);
						glTexCoord2d(0.5,1.0);
						glVertex3f(D.x,D.y,D.z);

						glNormal3f(gr_size[i],0.0,1.0);
						glTexCoord2d(0.5,0.0);
						glVertex3f(C.x,C.y,C.z);
	//
						glNormal3f(gr_size[i],0.0,1.0);
						glTexCoord2d(0.5,0.0);
						glVertex3f(C.x,C.y,C.z);

						glNormal3f(gr_size[i],0.0,1.0);
						glTexCoord2d(0.5,1.0);
						glVertex3f(D.x,D.y,D.z);

						glNormal3f(gr_size[i],0.0,1.0);
						glTexCoord2d(1.0,1.0);
						glVertex3f(F.x,F.y,F.z);
						//
						glNormal3f(gr_size[i],0.0,1.0);
						glTexCoord2d(0.5,0.0);
						glVertex3f(C.x,C.y,C.z);

						glNormal3f(gr_size[i],0.0,1.0);
						glTexCoord2d(1.0,1.0);
						glVertex3f(F.x,F.y,F.z);

						glNormal3f(gr_size[i],0.0,1.0);
						glTexCoord2d(1.0,0.0);
						glVertex3f(E.x,E.y,E.z);
					glEnd();
					glPopMatrix();

                }
		//		printf("but triangles drawing: %d\n",samples);
                //glDepthMask(true);
				glDisable(GL_DEPTH_TEST);
                glDisable(GL_ALPHA_TEST);
                glDisable(GL_BLEND);
				Info::SetShader(tmp);
				if(Info::GetShader())  glUseProgram(0);
}
