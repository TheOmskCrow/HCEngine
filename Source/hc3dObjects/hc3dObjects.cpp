// hc3dObjects.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "hc3dObjects.h"

using namespace hc3d;

TerrainList Terrains::list;

Atmosphere* hc3d::GetAtmo() {
	return new Atmosphere();
}

Terrain* hc3d::GetTer(std::string mapName, Vector3D offset) {
	return new Terrain(mapName, offset);
}

Butterfly* hc3d::GetBut() {
	return new Butterfly();
}

Grass* hc3d::GetGrass() {
	return new Grass();
}

Clouds* hc3d::GetClouds() {
	return new Clouds();
}

Tree* hc3d::GetTrees() {
	return new Tree();
}

GameWindow* hc3d::GetGameWindow() {
	return new GameWindow();
}

Water* hc3d::GetWater() {
	return new Water();
}

Star* hc3d::GetStar() {
	return new Star();
}

Stone* hc3d::GetStone() {
	return new Stone();
}

Fish* hc3d::GetFish() {
	return new Fish();
}

cOcean* hc3d::GetOcean() {
	return new cOcean();
}

float hc3d::CalcTerHeight(Vector3D position) {
	for (TerrainList::iterator it = Terrains::list.begin(); it != Terrains::list.end(); ++it) {
		if ((it->second.x - Terrain::dimension <= position.x) && (it->second.x + Terrain::dimension - 0.1 > position.x) &&
			(it->second.y - Terrain::dimension <= position.y) && (it->second.y + Terrain::dimension - 0.1 > position.y) && it->first->Loaded())
			return it->first->calc_height(position);
	}
	return -10;
}

Vector3D hc3d::CalcTerNormal(Vector3D position) {
	for (TerrainList::iterator it = Terrains::list.begin(); it != Terrains::list.end(); ++it) {
		if ((it->second.x - Terrain::dimension <= position.x) && (it->second.x + Terrain::dimension - 0.1 > position.x) &&
			(it->second.y - Terrain::dimension <= position.y) && (it->second.y + Terrain::dimension - 0.1 > position.y) && it->first->Loaded())
			return it->first->calc_normal(position);
	}
	return Vector3D(0.0, 0.0, 1.0);
}

void hc3d::SetGrassWind(float windWave, Vector3D windCenter) {
	Grass::windWave = 0.0;
	Grass::windCenter = windCenter;
}

void hc3d::Reflect() {
	Water::reflect();
}
void hc3d::Refract() {
	Water::refract();
}

void hc3d::SetRefract(bool value) {
	Terrain::refract = value;
}

void hc3d::SetReflect(bool value) {
	Terrain::reflect = value;
}