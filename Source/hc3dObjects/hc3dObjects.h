#pragma once
#include "Objects\Atmosphere\Atmosphere.h"
#include "Objects\Terrain\Terrain.h"
#include "Objects\Butterfly\Butterfly.h"
#include "Objects\Grass\Grass.h"
#include "Objects\Clouds\Clouds.h"
#include "Objects\Tree\Tree.h"
#include "Objects\GameWindow\GameWindow.h"
#include "Objects\Water\Water.h"
#include "Objects\Star\Star.h"
#include "Objects\Stone\Stone.h"
#include "Objects\Fish\Fish.h"
#include "Objects\Ocean\Ocean.h"
#include "Objects\Plane\Plane.h"
#include <hc3dMath.h>
#include <map>


namespace hc3d {
	HC3D_API Atmosphere* GetAtmo();

	HC3D_API Terrain* GetTer(std::string mapName, Vector3D offset);

	HC3D_API Terrains* GetTerrain();

	HC3D_API Butterfly* GetBut();

	HC3D_API Grass* GetGrass();

	HC3D_API Clouds* GetClouds();

	HC3D_API Tree* GetTrees();

	HC3D_API Plane* GetPlane();

	HC3D_API GameWindow* GetGameWindow();

	HC3D_API Water* GetWater();

	HC3D_API Star* GetStar();

	HC3D_API Stone* GetStone();

	HC3D_API Fish* GetFish();

	HC3D_API cOcean* GetOcean();

	HC3D_API float CalcTerHeight(Vector3D position);

	HC3D_API Vector3D CalcTerNormal(Vector3D position);

	HC3D_API void AddTerrain(std::string path, Vector3D size, Vector3D location);

	HC3D_API void SetGrassWind(float windWave, Vector3D windCenter);

	HC3D_API void Reflect();

	HC3D_API void Refract();
}