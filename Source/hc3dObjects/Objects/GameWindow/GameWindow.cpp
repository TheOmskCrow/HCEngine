#include "StdAfx.h"
#include "GameWindow.h"
#include "../Atmosphere/Atmosphere.h"
#include <iostream>
#define pi 3.1415

using namespace hc3d;

GameWindow::GameWindow(void)
{
}


GameWindow::~GameWindow(void)
{
}
void GameWindow::Init() {
	//TODO: синхронизировать со входными данными и консолью

	num = 1;
	frames = 1;
	start_coord[0] = 0.35;
	start_coord[1] = 0.35;
	radius = 1.16;
	end_coord[0] = 0.65;
	end_coord[1] = 0.65;
	text_size = 100;
	glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
	Info::RaiseCurTextureNum();
	cloud1 = CreateTexture(GL_RGB,2048);
	glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
	Info::RaiseCurTextureNum();
	cloud2 = CreateTexture(GL_RGB,2048);
	glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
	Info::RaiseCurTextureNum();
	depth1 = LoadTexture("Texture/halo.png");
	glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
	Info::RaiseCurTextureNum();
	depth2 = LoadTexture("Texture/b.png");
	glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
	Info::RaiseCurTextureNum();
	screen_noise = LoadTexture("Texture/noise.png");
	glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
	Info::RaiseCurTextureNum();
	font = LoadTexture("Texture/panel.png");
	glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
	Info::RaiseCurTextureNum();
	g_blur = CreateTexture(GL_RGB,2048);
	glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
	Info::RaiseCurTextureNum();
	D_O_F = CreateTexture(GL_DEPTH_COMPONENT,2048);
	glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
	Info::RaiseCurTextureNum();
	D_O_F_prev = CreateTexture(GL_RGB, 2048);
	glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
	Info::RaiseCurTextureNum();
	l_depth = CreateTexture(GL_RGB,2048);
	glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
	Info::RaiseCurTextureNum();
	normal = CreateTexture(GL_RGB,2048);
	glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
	Info::RaiseCurTextureNum();
	SSAOnormal = CreateTexture(GL_RGB,2048);
	glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
	Info::RaiseCurTextureNum();
	abberation = CreateTexture(GL_RGB,2048);

	shaderprogram = LoadShader("Shaders/screen.vert","Shaders/screen.frag");
	shaderprogram2 = LoadShader("Shaders/bokeh.vert","Shaders/bokeh.frag");
	//shaderprogram3 = LoadShader("Shaders/b-y.vert","Shaders/b-y.frag");
	shaderprogram4 = LoadShader("Shaders/linearise.vert","Shaders/linearise.frag");
	shaderprogram5 = LoadShader("Shaders/default.vert","Shaders/default.frag");
	shaderprogram6 = LoadShader("Shaders/normal.vert","Shaders/normal.frag");
	shaderprogram7 = LoadShader("Shaders/abberation.vert","Shaders/abberation.frag");
	shaderprogram8 = LoadShader("Shaders/SSAO.vert","Shaders/SSAO.frag");
	shaderprogram9 = LoadShader("Shaders/linearise.vert", "Shaders/updatedepth.frag");
	shaderprogram10 = LoadShader("Shaders/shadow.vert", "Shaders/shadow.frag");
}

void GameWindow::init_text(char *txt) {

}
void GameWindow::render_text() {
	if(frames < text_size) frames+=5;
}

void GameWindow::calc_font_coord(int i,int str) {
}

void GameWindow::setShader(float width, float height){
	   // System.out.println("num = " + num);
			GLfloat* mv = new GLfloat[16];
			GLfloat* pr = new GLfloat[16];
			glGetFloatv (GL_MODELVIEW_MATRIX,  mv);
    		glGetFloatv (GL_PROJECTION_MATRIX, pr);
			glUniformMatrix4fv(glGetUniformLocation(
	        		shaderprogram, "mv"), 1,false,mv);
			glUniformMatrix4fv(glGetUniformLocation(
	        		shaderprogram, "pr"), 1,false,pr);
			glUniform1i(glGetUniformLocation(
	        		shaderprogram, "AA"), Info::GetAA());
		    glUniform3f(glGetUniformLocation(
				shaderprogram, "eyePos"), Camera::getPosition().x, Camera::getPosition().y, Camera::getPosition().z);
			glUniform1i(glGetUniformLocation(
	        		shaderprogram, "bloom_"),Info::GetBloom());
			glUniform1i(glGetUniformLocation(
	        		shaderprogram, "godRays"), Info::GetGodRays());
			glUniform1i(glGetUniformLocation(
	        		shaderprogram, "toneMapping"), Info::GetToneMapping());
			glUniform1i(glGetUniformLocation(
	        		shaderprogram, "filmGrain"), Info::GetFilmGrain());
			glUniform1i(glGetUniformLocation(
	        		shaderprogram, "depthOfField"), Info::GetDOF());
		    glUniform1i(glGetUniformLocation(
	        		shaderprogram, "tex1"),5);
		    glUniform1i(glGetUniformLocation(
	        		shaderprogram, "tex2"),5);
		    glUniform1i(glGetUniformLocation(
	        		shaderprogram, "screen_noise"), 7);
			glUniform1i(glGetUniformLocation(
	        		shaderprogram, "abberation"), 30);
			glUniform1i(glGetUniformLocation(
	        		shaderprogram, "font"), 19);
			glUniform1i(glGetUniformLocation(
	        		shaderprogram, "exposure"), 25);
			glUniform1i(glGetUniformLocation(
	        		shaderprogram, "g_blur"), 10);
			glUniform1i(glGetUniformLocation(
	        		shaderprogram, "D_O_F"), 4);
			glUniform1i(glGetUniformLocation(
	        		shaderprogram, "D_O_F_prev"), 23);
		    glUniform1i(glGetUniformLocation(
	        		shaderprogram, "draw_info"), 0);
		    glUniform1i(glGetUniformLocation(
	        		shaderprogram, "frames"), 1000);
		    glUniform1i(glGetUniformLocation(
	        		shaderprogram, "spec1"), 5);
		    glUniform1i(glGetUniformLocation(
	        		shaderprogram, "spec2"), 6);
	        glUniform1i(glGetUniformLocation(
	        		shaderprogram, "num"), 1);
			glUniform1i(glGetUniformLocation(
	        		shaderprogram, "text_size"), frames);
	        glUniform1f(glGetUniformLocation(
	        		shaderprogram, "brightness"), 1.2);
	        glUniform1f(glGetUniformLocation(
	        		shaderprogram, "flash"), flash);
			glUniform1f(glGetUniformLocation(
				shaderprogram, "time"), Info::getTime()/1000.0);
	        glUniform1f(glGetUniformLocation(
	        		shaderprogram, "blur"), 0.2);
	        glUniform1f(glGetUniformLocation(
	        		shaderprogram, "rnd_x"), (rand()%1000)/1000.0);
	        glUniform1f(glGetUniformLocation(
	        		shaderprogram, "rnd_y"), (rand()%1000)/1000.0);
	        glUniform1f(glGetUniformLocation(
	        		shaderprogram, "Width"), width);
	        glUniform1f(glGetUniformLocation(
	        		shaderprogram, "Height"), height);
	        glUniform4f(glGetUniformLocation(
	        		shaderprogram, "sun"), sun.x*100.0, sun.y*100.0, sun.z*100.0,1.0);
			glUniform4f(glGetUniformLocation(
	        		shaderprogram, "sun2"), sun2.x*100.0, sun2.y*100.0, sun2.z*100.0,1.0);
			glUniform1fv(glGetUniformLocation(
	        		shaderprogram, "start_coord"), 2,start_coord);
			glUniform1fv(glGetUniformLocation(
	        		shaderprogram, "end_coord"), 2,end_coord);
			delete[] mv;
			delete[] pr;
	 }

void GameWindow::setTex(float _width, float _height) {
			int width = _width;
			int height = _height;
			int x_offset = (2048 - width)/2;
			int y_offset = (2048 - height)/2;
			//width = 2048; height = 2048;
			glEnable(GL_TEXTURE_2D);
			GLenum err = glGetError();
			if (err) std::cout << "(window.draw.EnableTexture_2D) GL error : " << err << std::endl;
			glActiveTexture(GL_TEXTURE3);
		    glBindTexture(GL_TEXTURE_2D,cloud1);
				//glCopyTexSubImage2D(GL_TEXTURE_2D, 0,x_offset,y_offset,0,0, 1024,1024);
				//glCopyTexSubImage2D(GL_TEXTURE_2D, 0,x_offset,y_offset,0,0, int(width),int(height));
			glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, int(width), int(height), 0);
				err = glGetError();
				if (err) std::cout << "(window.draw.TEXTURE3 CopyTexImage) GL error : " << err << std::endl;
//				glGenerateMipmap(GL_TEXTURE_2D);
//				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
//				GL_LINEAR_MIPMAP_LINEAR);
				//glHint(GL_GENERATE_MIPMAP_HINT, GL_FASTEST);
		    //glCopyTexImage2D (GL_TEXTURE_2D, 0,  GL_DEPTH_COMPONENT, 0, 0, 1024, 1024, 0 );
		    glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D,g_blur);
		//	glGenerateMipmap(GL_TEXTURE_2D);
		   // glCopyTexSubImage2D(GL_TEXTURE_2D, 0, x_offset,y_offset,0, 0,int(width),int(height));
			glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, int(width), int(height), 0);
			err = glGetError();
			if (err) std::cout << "(window.draw.TEXTURE4 CopyTexImage) GL error : " << err << std::endl;
			glActiveTexture(GL_TEXTURE10);
			glBindTexture(GL_TEXTURE_2D, g_blur);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, D_O_F);
			glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 0, 0, int(width), int(height), 0);
			//glCopyTexSubImage2D(GL_TEXTURE_2D, 0, x_offset, y_offset, 0, 0, int(width), int(height));
			err = glGetError();
			if (err) std::cout << "(window.draw.TEXTURE1 CopyTexImage) GL error : " << err << std::endl;
			glActiveTexture(GL_TEXTURE26);
			glBindTexture(GL_TEXTURE_2D, D_O_F);

		    glActiveTexture(GL_TEXTURE4);
		    glBindTexture(GL_TEXTURE_2D,cloud2);
			err = glGetError();
			if (err) std::cout << "(window.draw.TEXTURE4) GL error : " << err << std::endl;

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D,normal);
			glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, int(width), int(height), 0);
			//glCopyTexSubImage2D(GL_TEXTURE_2D, 0, x_offset,y_offset,0, 0,int(width),int(height));
			err = glGetError();
			if (err) std::cout << "(window.draw.TEXTURE2 CopyTexImage) GL error : " << err << std::endl;
			glActiveTexture(GL_TEXTURE25);
			glBindTexture(GL_TEXTURE_2D, normal);

		    glActiveTexture(GL_TEXTURE5);
		    glBindTexture(GL_TEXTURE_2D,depth1);
			err = glGetError();
			if (err) std::cout << "(window.draw.TEXTURE5) GL error : " << err << std::endl;

		    glActiveTexture(GL_TEXTURE6);
		    glBindTexture(GL_TEXTURE_2D,depth2);
			err = glGetError();
			if (err) std::cout << "(window.draw.TEXTURE6) GL error : " << err << std::endl;

		    glActiveTexture(GL_TEXTURE7);
		    glBindTexture(GL_TEXTURE_2D,screen_noise);
			err = glGetError();
			if (err) std::cout << "(window.draw.TEXTURE7) GL error : " << err << std::endl;
		  
}
void GameWindow::Draw() {

		 		float w, h;
		 		w = float(Info::width())/float(Info::height())*radius*(2048.0/float(Info::width()));
		 		h = radius*(2048.0/float(Info::height()));
		 		float yaw = Camera::GetYaw();
		 		float pitch = Camera::GetPitch();
				float roll = Camera::GetRoll()*cos(Camera::GetPitch() + Camera::GetBoatYaw());
				pos = Camera::getPosition();
				sun = Atmosphere::getSun();
				sun2 = Atmosphere::getSun2();
		 		center = Vector3D(pos.x,pos.y,pos.z);
				glEnable(GL_DEPTH_TEST);
			    setTex(Info::width(),Info::height());
				GLenum err = glGetError();
				if (err) std::cout << "(window.draw.setTex) GL error : " << err << std::endl;
				float width = Info::width();
				float height = Info::height();
				int x_offset = (2048 - width)/2;
				int y_offset = (2048 - height)/2;
				num++;
			    if(num > 2) num = 1;
				float ww = 1.0;// 2048.0 / Info::width();
				
				float hh = 1.0;// 2048.0 / Info::height();
				/*
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



				glUseProgram(shaderprogram7);
				glUniform1i(glGetUniformLocation(
	        		shaderprogram7, "tex"), 3);
				glUniform1i(glGetUniformLocation(
	        		shaderprogram7, "Width"), width);
				glUniform1i(glGetUniformLocation(
	        		shaderprogram7, "Height"), height);

				glEnable(GL_DEPTH_TEST);
				
			    glMatrixMode(GL_PROJECTION);
				glPushMatrix();
				glLoadIdentity();
				glMatrixMode(GL_MODELVIEW);
				glPushMatrix();
				glLoadIdentity();
			    glBegin(GL_QUADS);

					glColor3f (1.0f,1.0f,1.0f);
			    	glTexCoord2d(0,0);
			    	//glVertex3f(point1.x+center.x,point1.y+center.y,point1.z+center.z);
					glVertex2f(-1*ww,-1*hh);

			    	glTexCoord2d(1,0);
			    	//glVertex3f(point2.x+center.x,point2.y+center.y,point2.z+center.z);
					glVertex2f(1*ww,-1*hh);

			    	glTexCoord2d(1,1);
			    	//glVertex3f(point3.x+center.x,point3.y+center.y,point3.z+center.z);
					glVertex2f(1*ww,1*hh);

			    	glTexCoord2d(0,1);
			    	//glVertex3f(point4.x+center.x,point4.y+center.y,point4.z+center.z);
					glVertex2f(-1*ww,1*hh);
			    glEnd();
				glMatrixMode(GL_PROJECTION);
				glPopMatrix();
				glMatrixMode(GL_MODELVIEW);
				glPopMatrix();
				glDisable(GL_DEPTH_TEST);
				glUseProgram(0);

				glActiveTexture(GL_TEXTURE30);
				glBindTexture(GL_TEXTURE_2D,abberation);
				glCopyTexSubImage2D(GL_TEXTURE_2D, 0, x_offset,y_offset,0, 0,int(width),int(height));
			
				*/
				//SSAO


				/*shadow*/

				if (Info::GetShadow()) {
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				//	glEnable(GL_DEPTH_TEST);
					glUseProgram(shaderprogram10);

					GLfloat* mv = new GLfloat[16];
					GLfloat* pr = new GLfloat[16];
					glGetFloatv(GL_MODELVIEW_MATRIX, mv);
					glGetFloatv(GL_PROJECTION_MATRIX, pr);

					glUniform1f(glGetUniformLocation(
						shaderprogram10, "Width"), float(Info::width()));
					glUniform1f(glGetUniformLocation(
						shaderprogram10, "Height"), float(Info::height()));

					glUniformMatrix4fv(glGetUniformLocation(
						shaderprogram10, "mv"), 1, false, mv);
					glUniformMatrix4fv(glGetUniformLocation(
						shaderprogram10, "pr"), 1, false, pr);

					glUniform1i(glGetUniformLocation(
						shaderprogram10, "depth_texture"), 26);


					glUniform1f(glGetUniformLocation(
						shaderprogram10, "zNear"), 500.0*Info::GetGlobalScale());
					glUniform1f(glGetUniformLocation(
						shaderprogram10, "zFar"), 5000000000.0*Info::GetGlobalScale());

					glUniform3f(glGetUniformLocation(
						shaderprogram10, "eyePos"), Camera::getPosition().x, Camera::getPosition().y, Camera::getPosition().z);

					glActiveTexture(GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D, Shadow::GetShadowMap()[0]);
					glUniform1i(glGetUniformLocation(
						shaderprogram10, "_shadow0"), 1);

					glActiveTexture(GL_TEXTURE2);
					glBindTexture(GL_TEXTURE_2D, Shadow::GetShadowMap()[1]);
					glUniform1i(glGetUniformLocation(
						shaderprogram10, "_shadow1"), 2);

					glActiveTexture(GL_TEXTURE3);
					glBindTexture(GL_TEXTURE_2D, Shadow::GetShadowMap()[2]);
					glUniform1i(glGetUniformLocation(
						shaderprogram10, "_shadow2"), 3);

					glActiveTexture(GL_TEXTURE5);
					glBindTexture(GL_TEXTURE_2D, cloud1);
					glUniform1i(glGetUniformLocation(
						shaderprogram10, "color"), 5);

					glActiveTexture(GL_TEXTURE7);
					glBindTexture(GL_TEXTURE_2D, screen_noise);
					glUniform1i(glGetUniformLocation(
						shaderprogram10, "noise_texture"), 7);





				//	glEnable(GL_DEPTH_TEST);
					glMatrixMode(GL_PROJECTION);
					glPushMatrix();
					glLoadIdentity();
					glMatrixMode(GL_MODELVIEW);
					glPushMatrix();
					glLoadIdentity();
					glBegin(GL_QUADS);
					glColor3f(1.0f, 1.0f, 1.0f);
					glTexCoord2d(0, 0);
					glVertex2f(-1 * ww, -1 * hh);

					glTexCoord2d(1, 0);
					glVertex2f(1 * ww, -1 * hh);

					glTexCoord2d(1, 1);
					glVertex2f(1 * ww, 1 * hh);

					glTexCoord2d(0, 1);
					glVertex2f(-1 * ww, 1 * hh);
					glEnd();
					glMatrixMode(GL_PROJECTION);
					glPopMatrix();
					glMatrixMode(GL_MODELVIEW);
					glPopMatrix();
			//		glDisable(GL_DEPTH_TEST);
					glUseProgram(0);
					glActiveTexture(GL_TEXTURE5);
					glBindTexture(GL_TEXTURE_2D, cloud1);
					glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, int(width), int(height), 0);
					//glCopyTexSubImage2D(GL_TEXTURE_2D, 0, x_offset, y_offset, 0, 0, int(width), int(height));
					err = glGetError();
					if (err) std::cout << "(window.draw.shadow) GL error : " << err << std::endl;
					delete[] mv;
					delete[] pr;
				}
				
				/**/

				glActiveTexture(GL_TEXTURE5);
				glBindTexture(GL_TEXTURE_2D, cloud1);
				err = glGetError();
				if (err) std::cout << "(window.draw.cloud1 set to 5) GL error : " << err << std::endl;
				if (Info::GetSSAO())  {
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
					//glEnable(GL_DEPTH_TEST);
					glUseProgram(shaderprogram8);

					glUniform1i(glGetUniformLocation(
						shaderprogram8, "gdepth"), 26);

					glUniform1i(glGetUniformLocation(
						shaderprogram8, "gblur"), 10);
					glUniform1i(glGetUniformLocation(
						shaderprogram8, "gdiffuse"), 5);
					glUniform1f(glGetUniformLocation(
						shaderprogram8, "zNear"), 500.0*Info::GetGlobalScale());
					glUniform1f(glGetUniformLocation(
						shaderprogram8, "zFar"), 5000000000.0*Info::GetGlobalScale());
					glUniform1f(glGetUniformLocation(
						shaderprogram8, "Width"), width);
					glUniform1f(glGetUniformLocation(
						shaderprogram8, "Height"), height);

					glActiveTexture(GL_TEXTURE7);
					glBindTexture(GL_TEXTURE_2D, screen_noise);
					glUniform1i(glGetUniformLocation(
						shaderprogram8, "grandom"), 7);





					//glEnable(GL_DEPTH_TEST);
					glMatrixMode(GL_PROJECTION);
					glPushMatrix();
					glLoadIdentity();
					glMatrixMode(GL_MODELVIEW);
					glPushMatrix();
					glLoadIdentity();
					glBegin(GL_QUADS);
					glColor3f(1.0f, 1.0f, 1.0f);
					glTexCoord2d(0, 0);
					glVertex2f(-1 * ww, -1 * hh);

					glTexCoord2d(1, 0);
					glVertex2f(1 * ww, -1 * hh);

					glTexCoord2d(1, 1);
					glVertex2f(1 * ww, 1 * hh);

					glTexCoord2d(0, 1);
					glVertex2f(-1 * ww, 1 * hh);
					glEnd();
					glMatrixMode(GL_PROJECTION);
					glPopMatrix();
					glMatrixMode(GL_MODELVIEW);
					glPopMatrix();
					//glDisable(GL_DEPTH_TEST);
					glUseProgram(0);
					glActiveTexture(GL_TEXTURE5);
					glBindTexture(GL_TEXTURE_2D, cloud1);
					glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, int(width), int(height), 0);
					//glCopyTexSubImage2D(GL_TEXTURE_2D, 0, x_offset, y_offset, 0, 0, int(width), int(height));
					err = glGetError();
					if (err) std::cout << "(window.draw.SSAO) GL error : " << err << std::endl;
				}
				
				//1 blur depth pass
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



				glUseProgram(shaderprogram2);
				glUniform1i(glGetUniformLocation(
					shaderprogram2, "offsetType"), 0);
				glUniform1i(glGetUniformLocation(
					shaderprogram2, "texture"), 5);
				glUniform1f(glGetUniformLocation(
					shaderprogram2, "Width"), float(Info::width()));
				glUniform1f(glGetUniformLocation(
					shaderprogram2, "Height"), float(Info::height()));
				glUniform2f(glGetUniformLocation(
					shaderprogram2, "direction"), 0.0, 1.0);

				//glEnable(GL_DEPTH_TEST);

				glMatrixMode(GL_PROJECTION);
				glPushMatrix();
				glLoadIdentity();
				glMatrixMode(GL_MODELVIEW);
				glPushMatrix();
				glLoadIdentity();
				glBegin(GL_QUADS);

				glColor3f(1.0f, 1.0f, 1.0f);
				glTexCoord2d(0, 0);
				glVertex2f(-1 * ww, -1 * hh);

				glTexCoord2d(1, 0);
				glVertex2f(1 * ww, -1 * hh);

				glTexCoord2d(1, 1);
				glVertex2f(1 * ww, 1 * hh);

				glTexCoord2d(0, 1);
				glVertex2f(-1 * ww, 1 * hh);
				glEnd();
				glMatrixMode(GL_PROJECTION);
				glPopMatrix();
				glMatrixMode(GL_MODELVIEW);
				glPopMatrix();
				glUseProgram(0);

				glActiveTexture(GL_TEXTURE10);
				glBindTexture(GL_TEXTURE_2D, g_blur);
				glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, int(width), int(height), 0);
				err = glGetError();
				if (err) std::cout << "(window.draw.1 blur pass) GL error : " << err << std::endl;
				//2 blur pass

				//return;
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



				glUseProgram(shaderprogram2);
				glActiveTexture(GL_TEXTURE10);
				glBindTexture(GL_TEXTURE_2D, g_blur);
				glUniform1i(glGetUniformLocation(
					shaderprogram2, "offsetType"), 1);
				glUniform1i(glGetUniformLocation(
					shaderprogram2, "texture"), 10);
				glUniform1f(glGetUniformLocation(
					shaderprogram2, "Width"), float(Info::width()));
				glUniform1f(glGetUniformLocation(
					shaderprogram2, "Height"), float(Info::height()));
				glUniform2f(glGetUniformLocation(
					shaderprogram2, "direction"), 0.866, 0.5);

				glMatrixMode(GL_PROJECTION);
				glPushMatrix();
				glLoadIdentity();
				glMatrixMode(GL_MODELVIEW);
				glPushMatrix();
				glLoadIdentity();
				glBegin(GL_QUADS);

				glColor3f(1.0f, 1.0f, 1.0f);
				glTexCoord2d(0, 0);
				glVertex2f(-1 * ww, -1 * hh);

				glTexCoord2d(1, 0);
				glVertex2f(1 * ww, -1 * hh);

				glTexCoord2d(1, 1);
				glVertex2f(1 * ww, 1 * hh);

				glTexCoord2d(0, 1);
				glVertex2f(-1 * ww, 1 * hh);
				glEnd();
				glMatrixMode(GL_PROJECTION);
				glPopMatrix();
				glMatrixMode(GL_MODELVIEW);
				glPopMatrix();
				glUseProgram(0);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, g_blur);
				glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, int(width), int(height), 0);

				err = glGetError();
				if (err) std::cout << "(window.draw.2 blur pass) GL error : " << err << std::endl;

				//blur 3 pass


				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



				glUseProgram(shaderprogram2);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, g_blur);
				glUniform1i(glGetUniformLocation(
					shaderprogram2, "offsetType"), 2);
				glUniform1i(glGetUniformLocation(
					shaderprogram2, "texture"), 0);
				glUniform1f(glGetUniformLocation(
					shaderprogram2, "Width"), float(Info::width()));
				glUniform1f(glGetUniformLocation(
					shaderprogram2, "Height"), float(Info::height()));
				glUniform2f(glGetUniformLocation(
					shaderprogram2, "direction"), 0.866, -0.5);

				glMatrixMode(GL_PROJECTION);
				glPushMatrix();
				glLoadIdentity();
				glMatrixMode(GL_MODELVIEW);
				glPushMatrix();
				glLoadIdentity();
				glBegin(GL_QUADS);

				glColor3f(1.0f, 1.0f, 1.0f);
				glTexCoord2d(0, 0);
				glVertex2f(-1 * ww, -1 * hh);

				glTexCoord2d(1, 0);
				glVertex2f(1 * ww, -1 * hh);

				glTexCoord2d(1, 1);
				glVertex2f(1 * ww, 1 * hh);

				glTexCoord2d(0, 1);
				glVertex2f(-1 * ww, 1 * hh);
				glEnd();
				glMatrixMode(GL_PROJECTION);
				glPopMatrix();
				glMatrixMode(GL_MODELVIEW);
				glPopMatrix();
				glUseProgram(0);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, g_blur);
				glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, int(width), int(height), 0);

				err = glGetError();
				if (err) std::cout << "(window.draw.2 blur pass) GL error : " << err << std::endl;



				//if (Info::GetDOF()) {
					/*linearize depth*/

					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
					//glEnable(GL_DEPTH_TEST);
					glUseProgram(shaderprogram4);

					glActiveTexture(GL_TEXTURE26);
					glBindTexture(GL_TEXTURE_2D, D_O_F);
					glUniform1i(glGetUniformLocation(
						shaderprogram4, "depth"), 26);
					glUniform1f(glGetUniformLocation(
						shaderprogram4, "zNear"), 500.0*Info::GetGlobalScale());
					glUniform1f(glGetUniformLocation(
						shaderprogram4, "zFar"), 5000000000.0*Info::GetGlobalScale());

					glMatrixMode(GL_PROJECTION);
					glPushMatrix();
					glLoadIdentity();
					glMatrixMode(GL_MODELVIEW);
					glPushMatrix();
					glLoadIdentity();
					glBegin(GL_QUADS);
					glColor3f(1.0f, 1.0f, 1.0f);
					glTexCoord2d(0, 0);
					glVertex2f(-1 * ww, -1 * hh);

					glTexCoord2d(1, 0);
					glVertex2f(1 * ww, -1 * hh);

					glTexCoord2d(1, 1);
					glVertex2f(1 * ww, 1 * hh);

					glTexCoord2d(0, 1);
					glVertex2f(-1 * ww, 1 * hh);
					glEnd();
					glMatrixMode(GL_PROJECTION);
					glPopMatrix();
					glMatrixMode(GL_MODELVIEW);
					glPopMatrix();
					//glDisable(GL_DEPTH_TEST);
					glUseProgram(0);
					glActiveTexture(GL_TEXTURE4);
					glBindTexture(GL_TEXTURE_2D, cloud2);
					glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, int(width), int(height), 0);
					//glCopyTexSubImage2D(GL_TEXTURE_2D, 0, x_offset, y_offset, 0, 0, int(width), int(height));
					err = glGetError();
					if (err) std::cout << "(window.draw.DOF) GL error : " << err << std::endl;
					//return;
					/*update depth for current field*/


					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				//	glEnable(GL_DEPTH_TEST);
					glUseProgram(shaderprogram9);

					glActiveTexture(GL_TEXTURE4);
					glBindTexture(GL_TEXTURE_2D, cloud2);
					glActiveTexture(GL_TEXTURE23);
					glBindTexture(GL_TEXTURE_2D, D_O_F_prev);
					glUniform1i(glGetUniformLocation(
						shaderprogram9, "depth"), 4);
					glUniform1i(glGetUniformLocation(
						shaderprogram9, "curDepth"), 23);
					if (!Info::GetConsoleRun()) {
						glUniform1f(glGetUniformLocation(
							shaderprogram9, "time"), Info::GetElapsedTime() * 0.2);
						glUniform1i(glGetUniformLocation(
							shaderprogram9, "console"), 0);
					}
					else {
						glUniform1f(glGetUniformLocation(
							shaderprogram9, "time"), 0.1);
						glUniform1i(glGetUniformLocation(
							shaderprogram9, "console"), 1);
					}

				//	glEnable(GL_DEPTH_TEST);
					glMatrixMode(GL_PROJECTION);
					glPushMatrix();
					glLoadIdentity();
					glMatrixMode(GL_MODELVIEW);
					glPushMatrix();
					glLoadIdentity();
					glBegin(GL_QUADS);
					glColor3f(1.0f, 1.0f, 1.0f);
					glTexCoord2d(0, 0);
					glVertex2f(-1 * ww, -1 * hh);

					glTexCoord2d(1, 0);
					glVertex2f(1 * ww, -1 * hh);

					glTexCoord2d(1, 1);
					glVertex2f(1 * ww, 1 * hh);

					glTexCoord2d(0, 1);
					glVertex2f(-1 * ww, 1 * hh);
					glEnd();
					glMatrixMode(GL_PROJECTION);
					glPopMatrix();
					glMatrixMode(GL_MODELVIEW);
					glPopMatrix();
				//	glDisable(GL_DEPTH_TEST);
					glUseProgram(0);
					glActiveTexture(GL_TEXTURE7);
					glBindTexture(GL_TEXTURE_2D, D_O_F_prev);
					glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, int(width), int(height), 0);
					//glCopyTexSubImage2D(GL_TEXTURE_2D, 0, x_offset, y_offset, 0, 0, int(width), int(height));
					err = glGetError();
					if (err) std::cout << "(window.draw.updateDOF) GL error : " << err << std::endl;
					glActiveTexture(GL_TEXTURE23);
					glBindTexture(GL_TEXTURE_2D, D_O_F_prev);
				//	return;
				//}
				/*main pass*/
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glViewport(0,0,Info::width(),Info::height());
				glUseProgram(shaderprogram);
			    setShader(Info::width(),Info::height());
				
				glMatrixMode(GL_PROJECTION);
				glPushMatrix();
				glLoadIdentity();
				glMatrixMode(GL_MODELVIEW);
				glPushMatrix();
				glLoadIdentity();
			    glBegin(GL_QUADS);

					glColor3f (1.0f,1.0f,1.0f);
			    	glTexCoord2d(0,0);
			    	//glVertex3f(point1.x+center.x,point1.y+center.y,point1.z+center.z);
					glVertex2f(-1*ww,-1*hh);

			    	glTexCoord2d(1,0);
			    	//glVertex3f(point2.x+center.x,point2.y+center.y,point2.z+center.z);
					glVertex2f(1*ww,-1*hh);

			    	glTexCoord2d(1,1);
			    	//glVertex3f(point3.x+center.x,point3.y+center.y,point3.z+center.z);
					glVertex2f(1*ww,1*hh);

			    	glTexCoord2d(0,1);
			    	//glVertex3f(point4.x+center.x,point4.y+center.y,point4.z+center.z);
					glVertex2f(-1*ww,1*hh);
			    glEnd();
				glMatrixMode(GL_PROJECTION);
				glPopMatrix();
				glMatrixMode(GL_MODELVIEW);
				glPopMatrix();
				glDisable(GL_DEPTH_TEST);
				glUseProgram(0);
				err = glGetError();
				if (err) std::cout << "(window.draw.main pass) GL error : " << err << std::endl;
}
