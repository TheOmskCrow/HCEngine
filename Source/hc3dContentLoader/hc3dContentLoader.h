#pragma once

#ifdef HC3D_EXPORTS
#define HC3D_API __declspec(dllexport) 
#else
#define HC3D_API __declspec(dllimport) 
#endif

#include <cstdlib>
#include <glew.h>
#include <freeglut.h>
#include <gl\GL.h>
#include <gl\GLU.h>
#include <glext.h>
#include <comutil.h>


/*
Загрузчик текстур, шейдеров. Возвращает id загруженного ресурса
*/
namespace hc3d {
	/// Загрузка текстуры из картинки
	HC3D_API GLuint LoadTexture(IN _bstr_t fileName);

	/// Создание пустой текстуры заданной размерности size x size
	HC3D_API GLuint CreateTexture(IN GLint format, int size);

	/// Загрузка шейдеров из файлов
	HC3D_API GLuint LoadShader(IN _bstr_t vertexShaderFileName, IN _bstr_t fragmentShaderFileName);
}
