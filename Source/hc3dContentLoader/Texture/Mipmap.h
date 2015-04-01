#pragma once
#include <cstdlib>
#include <gl\GL.h>
#include <gl\GLU.h>
#include <freeglut.h>
#include <glext.h>
#include <stdexcept>
#include <comutil.h>
#include <iostream>
#include <string.h>
#include <png.h>
#include <zlib.h>

namespace hc3d {
	class Mipmap
	{
	public:
		Mipmap(void);
		GLuint LoadImage(IN _bstr_t& fileName);
		GLuint LoadImage(GLint, int);

	private:
		std::pair<int, int> Mipmap::LoadPng(png_bytep &buffer, char* fileName);
		GLuint tex;
		void* _data;
		int width, height;
	};

}