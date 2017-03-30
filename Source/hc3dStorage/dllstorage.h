#pragma once

#ifdef HC3DSTORAGE_EXPORTS
#define HC3DSTORAGE_API __declspec(dllexport)
#else
#define HC3DSTORAGE_API __declspec(dllimport)
#endif

#include <cstdlib>
#include <glew.h>
#include <freeglut.h>
#include <gl\GL.h>
#include <gl\GLU.h>
#include <glext.h>
#include <comutil.h>