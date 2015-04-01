#pragma once

#ifdef HC3DPHYSICS_EXPORTS
#define HC3DPHYSICS_API __declspec(dllexport)
#else
#define HC3DPHYSICS_API __declspec(dllimport)
#endif