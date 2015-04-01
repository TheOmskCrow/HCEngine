#pragma once
#include <cstdlib>
#include <glew.h>
#include <gl\GL.h>
#include <gl\GLU.h>
#include <freeglut.h>
#include <glext.h>
#include <stdexcept>
#include <comutil.h>
#include <iostream>
#include <string.h>

namespace hc3d {
	class Shader
	{
	public:
		Shader(void);
		GLuint LoadShader(const _bstr_t&, const _bstr_t&);
	private:
		GLuint programObject;
	};

}

