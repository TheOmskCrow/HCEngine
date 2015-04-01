#pragma once

#ifdef HC3D_EXPORTS
#define HC3D_API __declspec(dllexport) 
#else
#define HC3D_API __declspec(dllimport) 
#endif
