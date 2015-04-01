// hc3dContentLoader.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "Shader\Shader.h"
#include "Texture\Mipmap.h"
#include "hc3dContentLoader.h"

using namespace hc3d;

GLuint hc3d::LoadTexture(IN _bstr_t fileName) {
	Mipmap m;
	return m.LoadImage(fileName);
}

GLuint hc3d::CreateTexture(IN GLint format, int size) {
	Mipmap m;
	return m.LoadImage(format, size);
}

GLuint hc3d::LoadShader(IN _bstr_t vertexShaderFileName, IN _bstr_t fragmentShaderFileName) {
	Shader sh;
	return sh.LoadShader(vertexShaderFileName, fragmentShaderFileName);
}