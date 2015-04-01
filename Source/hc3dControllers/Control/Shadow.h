#pragma once
#include <hc3dContentLoader.h>
#include <hc3dMath.h>
#include <vector>
#include <time.h>
#include <dllmain.h>

namespace hc3d {

	class HC3D_API Shadow
	{
	public:
		Shadow(void);
		~Shadow(void);
		static void shadow_shot(int);
		static void init();
		static GLuint *GetShadowMap();
	private:
		static GLuint *shad_map;
	};

}