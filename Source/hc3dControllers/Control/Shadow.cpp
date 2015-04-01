#include "StdAfx.h"
#include "Shadow.h"
#include <Windows.h>
#include "Info.h"

using namespace hc3d;

GLuint *Shadow::shad_map = 0;

Shadow::Shadow(void)
{
}


Shadow::~Shadow(void)
{
}
void Shadow::init() {
	shad_map = new GLuint[7];
	for(int i = 0; i < 7; i++) {
		glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
		Info::RaiseCurTextureNum();
		shad_map[i] = CreateTexture(GL_DEPTH_COMPONENT,2048);
	}
}
void Shadow::shadow_shot(int num) {
	int width = Info::width();
	int height = Info::height();
	int x_offset = (2048 - width)/2;
	int y_offset = (2048 - height)/2;
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0 + num);
    glBindTexture(GL_TEXTURE_2D,shad_map[num - 1]);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 0, 0, int(width), int(height), 0);
	//glCopyTexSubImage2D(GL_TEXTURE_2D, 0,x_offset,y_offset,0,0, int(width),int(height));
}

GLuint* Shadow::GetShadowMap() {
	return shad_map;
}