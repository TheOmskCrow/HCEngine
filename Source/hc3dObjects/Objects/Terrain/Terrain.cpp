#include "StdAfx.h"
#include "Terrain.h"
#include <Windows.h>
#include <hc3dContentLoader.h>
#include <hc3dToolkit.h>
#include <hc3dObjects.h>
#include <hc3dMath.h>
#include "Material.h"
#include "ShaderLib.h"
//#include "Collision.h"
#include <iostream>
#include <thread>
#include <mutex>
 
#define HMAP_LOAD 1
#define HMAP_DELETE 2
#define COLLISION_LOAD 3
#define COLLISION_DELETE 4

#define TEXEL_SIZE 64
#define MAP_DIMENSION 512
#define LOD_LEVELS 4
#define VISIBILITY_THRESHOLD 8000
#define ACCESSIBILITY_THRESHOLD 1000
#define TERRAIN_SCALE 2

using namespace hc3d;

std::mutex iMutex;
int Terrain::dimension = 512; //duplication
GLuint* Terrain::caust = 0;

Terrain::Terrain(std::string mapName, Vector3D offset) {
	this->mapName = mapName;
	this->offset = offset;
	pol = 20;
	loadedHM = false;
	loadedCol = false;
	busy = false;
}

void Terrain::InitMaterials() {
	if (!caust) {
		caust = new GLuint[32];
		for (int i = 0; i < 32; i++) {
			glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
			Info::RaiseCurTextureNum();
			std::string path;
			char num[10];
			path += "Texture/caust/";
			_itoa(i + 1, num, 10);
			path += num;
			path += ".png";

			caust[i] = LoadTexture(path.c_str());
		}
	}
	exploded = false;
	list_id = 0;

	if (!MatLib::IsUsed("beach1")) {
		Material mat;
		glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
		Info::RaiseCurTextureNum();
		mat.SetDiffuse("Texture/beach/beach1.png", Vector3D(0.745f, 0.674f, 0.513f), 150.0f);

		glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
		Info::RaiseCurTextureNum();
		mat.SetHeight("Texture/beach/beach1_height.png", 0.005f);
		mat.SetAmbient(ATMOSPHERE);

		MatLib::Add("beach1", mat);
	}

	if (!MatLib::IsUsed("mountain1")) {
		Material mat;
		glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
		Info::RaiseCurTextureNum();
		mat.SetDiffuse("Texture/rock/rock1.png", Vector3D(0.431f, 0.427f, 0.423f), 2000.0f);

		glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
		Info::RaiseCurTextureNum();
		mat.SetNormal("Texture/rock/rock1_normal.png", 0.3f);

		mat.SetAmbient(ATMOSPHERE);
		MatLib::Add("mountain1", mat);
	}
	if (!MatLib::IsUsed("snow1")) {
		Material mat;
		glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
		Info::RaiseCurTextureNum();
		mat.SetDiffuse("Texture/snow/snow1.png", Vector3D(0.811f, 0.811f, 0.815f), 200.0f);

		glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
		Info::RaiseCurTextureNum();
		mat.SetNormal("Texture/rock/rock1_normal.png", 0.1f);

		glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
		Info::RaiseCurTextureNum();
		mat.SetSpecular("Texture/rock/rock1_specular.png", 15.0f, 1.0f);
		mat.SetAmbient(ATMOSPHERE);
		MatLib::Add("snow1", mat);
	}
	if (!MatLib::IsUsed("grass1")) {
		Material mat;
		glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
		Info::RaiseCurTextureNum();
		mat.SetDiffuse("Texture/grass/grass1.png", Vector3D(0.454f, 0.529f, 0.141f), 600.0f);

		glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
		Info::RaiseCurTextureNum();
		mat.SetNormal("Texture/grass/grass1_normal.png", 0.3f);

		mat.SetAmbient(ATMOSPHERE);
		MatLib::Add("grass1", mat);
	}
	if (!MatLib::IsUsed("mask1")) {
		Material mat;
		glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
		Info::RaiseCurTextureNum();
		mat.SetDiffuse("Texture/hei1.png", Vector3D(0.5f, 0.5f, 0.5f), 5000.0f);
		mat.SetAmbient(NONE);

		MatLib::Add("mask1", mat);
	}
	if (!MatLib::IsUsed("mask2")) {
		Material mat;
		glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
		Info::RaiseCurTextureNum();
		mat.SetDiffuse("Texture/height2.png", Vector3D(0.5f, 0.5f, 0.5f), 5000.0f);
		mat.SetAmbient(NONE);
		MatLib::Add("mask2", mat);
	}
	if (!ShaderLib::IsUsed("ground")) ShaderLib::Add("ground", "Shaders/ground.vert", "Shaders/ground.frag");
}

#pragma region TERRAIN_LOAD

bool Terrain::Visible() {
	return dist(offset + Vector3D(dimension, dimension, 0), Camera::getPosition()) < VISIBILITY_THRESHOLD;
}

bool Terrain::Accessible() {
	return dist(offset + Vector3D(dimension, dimension, 0), Camera::getPosition()) < ACCESSIBILITY_THRESHOLD;
}

DWORD_PTR WINAPI SingleTerrainProcessThread(void* p) {
	Terrain *ptr = (Terrain *)p;
	ptr->Process();
	return 0;
}

HANDLE Terrain::RunMultiThreadTask() {
	DWORD_PTR cpuNum = hc3dToolkit::GetNumCPUs();
	HANDLE m_thread;
	Terrain* iparam = this;

	DWORD_PTR i = rand() % cpuNum;
	DWORD m_id = 0;
	DWORD m_mask = 1 << i;
	m_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SingleTerrainProcessThread, (LPVOID)iparam, NULL, &m_id);
	SetThreadAffinityMask(m_thread, m_mask);
	return m_thread;
}

HANDLE Terrain::InitHM() { // heightmap init
	if (busy) return nullptr;
	else busy = true;

	bool visibility = Visible();

	if (!visibility && loadedHM) state = HMAP_DELETE;
	else if (!visibility || loadedHM) {
		busy = false;
		return nullptr;
	}
	else state = HMAP_LOAD;

	return RunMultiThreadTask();
}

HANDLE Terrain::InitCollision() {
	if (busy) return nullptr;
	else busy = true;

	bool accessibility = Accessible();

	if (!accessibility && loadedCol) state = COLLISION_DELETE;
	else if (!accessibility || loadedCol) {
		busy = false;
		return nullptr;
	}
	else state = COLLISION_LOAD;

	return RunMultiThreadTask();
}

void Terrain::Init() {

}

void Terrain::InitCache(std::string normalMapName, std::string heightMapName) {
	int pol = 10;
	FILE * pFile;
	FILE* nmapFile;
	FILE* hmapFile;
	float **tmp = new float*[MAP_DIMENSION];
	Vector3D **tmp2 = new Vector3D*[MAP_DIMENSION];
	for (int i = 0; i < MAP_DIMENSION; i++) {
			tmp[i] = new float[MAP_DIMENSION];
			tmp2[i] = new Vector3D[MAP_DIMENSION];
	}
	size_t result;
	char* tbmpbuffer;
	BITMAPFILEHEADER                tBMP;
	if (!hc3dToolkit::FileExists(_bstr_t(mapName.c_str()))) {
		MessageBoxA(0, mapName.c_str(), "heightmap does not exist", MB_OK);
		exit(1);
	}

	const char *filename = mapName.c_str();

	fopen_s(&pFile, filename , "rb" );
	if (pFile==NULL) {fputs ("File error",stderr); exit (1);}

	fseek (pFile , 0 , SEEK_END);
	long lSize = ftell (pFile);
	rewind (pFile);

	tbmpbuffer = (char*) malloc (sizeof(char)*lSize);
	if (tbmpbuffer == NULL) {fputs ("Memory error",stderr); exit (2);}

	result = fread (tbmpbuffer,1,lSize,pFile);
	if (result != (size_t)lSize) {fputs ("Reading error",stderr); exit (3);}

	memcpy(&tBMP,tbmpbuffer,sizeof(BITMAPFILEHEADER));

	fclose (pFile);

	unsigned char C;
	float z = 0;

	for (int i = 0; i<MAP_DIMENSION; i++) {
		for (int j = 0; j < MAP_DIMENSION; j++) {
			memcpy(&C, tbmpbuffer + tBMP.bfOffBits + 0 + i * 3 + MAP_DIMENSION*j * 3, 1);
			//z += C * 256 * 256;
			memcpy(&C, tbmpbuffer + tBMP.bfOffBits + 1 + i * 3 + MAP_DIMENSION*j * 3, 1);
			//z += C * 256;
			memcpy(&C, tbmpbuffer + tBMP.bfOffBits + 2 + i * 3 + MAP_DIMENSION*j * 3, 1);
			z += C;
			tmp[i][j] = z * 2.0 - 5.0;
			if (tmp[i][j] < 0.0) tmp[i][j] -= 2.0;
			else tmp[i][j] += 2.0;
			hmap[i][j] = 0;//+(rand()%1000)/1000.0-1.0;
			//	printf("hmap[i][j] = %.2f\n", hmap[i][j]);
			z = 0.0;
		}
	}
	const int core = 1;
	const float coeff = 9.0;
	for(int i=0;i<MAP_DIMENSION;i++)
	for (int j = 0; j < MAP_DIMENSION; j++)
	{
	hmap[i][j] = 0.0;
	for (int x = -core; x <= core; x++) {
	for (int y = -core; y <= core; y++) {
		int _x = Math::hc3dMin(Math::hc3dMax(i + x, 0), MAP_DIMENSION - 1);
		int _y = Math::hc3dMin(Math::hc3dMax(j + y, 0), MAP_DIMENSION - 1);
	hmap[i][j] += tmp[_x][_y];
	}
	}
	hmap[i][j] /= coeff;
	}

	for(int i = 0; i < MAP_DIMENSION; i++) {
	for(int j = 0; j < MAP_DIMENSION; j++) {
	if(i < 1 || j < 1 || i > MAP_DIMENSION - 2 || j > MAP_DIMENSION - 2) {
	tmp2[i][j] = Vector3D(0.0,0.0,1.0);
	}
	else {
	Vector3D n;
	tmp2[i][j] = n.normal(Vector3D(0,0,(hmap[i-1][j-1])),Vector3D(10,0,(hmap[i+1][j-1])),Vector3D(5,10,(hmap[i][j+1])));
	}
	//tmap[i][j] = Vector3D(float(i)/pol,float(j)/pol,0.0);
	}
	}

	for(int i=0;i<MAP_DIMENSION;i++)
	for(int j=0;j<MAP_DIMENSION;j++)
	{
	nmap[i][j] = Vector3D(0.0, 0.0, 0.0);
	for (int x = -core; x <= core; x++) {
	for (int y = -core; y <= core; y++) {
		int _x = Math::hc3dMin(Math::hc3dMax(i + x, 0), MAP_DIMENSION - 1);
		int _y = Math::hc3dMin(Math::hc3dMax(j + y, 0), MAP_DIMENSION - 1);
	nmap[i][j] += tmp2[_x][_y];
	}
	}
	nmap[i][j] /= coeff;
	nmap[i][j] = nmap[i][j].Normalize();
	}

	for(int i = 0; i < MAP_DIMENSION; i++) {
	delete[] tmp[i];
	delete[] tmp2[i];
	}
	delete[] tmp;
	delete[] tmp2;

	nmapFile = fopen(normalMapName.c_str(), "w");
	hmapFile = fopen(heightMapName.c_str(), "w");

	if(nmapFile == 0 || hmapFile == 0) {
	fprintf(stderr,"no normal or height map created\n");
	MessageBoxA(0, "Can not create normal or height map", "dafuq", MB_OK);
	exit(1);
	}
	for(int i=0;i<MAP_DIMENSION;i++) {
		for(int j=0;j<MAP_DIMENSION;j++) {
			fprintf(nmapFile,"%.1f %.1f %.1f\n",nmap[i][j].x,nmap[i][j].y,nmap[i][j].z);
			fprintf(hmapFile,"%.1f\n",hmap[i][j]);
			//	printf("%d %d\n", i, j);
			}
	}
	fclose(nmapFile);
	fclose(hmapFile);
	//
}

void Terrain::Process() {
	switch (state) {
	case HMAP_LOAD:
		LoadHM();
		break;
	case HMAP_DELETE:
		DeleteHM();
		break;
	case COLLISION_LOAD:
		LoadCollision();
		break;
	case COLLISION_DELETE:
		DeleteCollision();
		break;
	default:
		cout << "invalid state: " << state << endl;
	}
}

void Terrain::LoadHM() {
	std::string normalMapName = mapName + std::string(".normal");
	std::string heightMapName = mapName + std::string(".height");

	listNum = MAP_DIMENSION * MAP_DIMENSION / TEXEL_SIZE / TEXEL_SIZE;
	long lSize;
	static_lists = new int*[listNum];
	for (int i = 0; i < listNum; i++) {
		static_lists[i] = new int[LOD_LEVELS];//64x64 6 levels
		for (int j = 0; j < LOD_LEVELS; j++) {
			static_lists[i][j] = 0;
		}
	}
	list_center = new Vector3D[listNum];//numbers -> variables
	hmap = new float*[MAP_DIMENSION];
	nmap = new Vector3D*[MAP_DIMENSION];
	//tmap = new Vector3D*[MAP_DIMENSION];
	for (int i = 0; i < MAP_DIMENSION; i++) {
		hmap[i] = new float[MAP_DIMENSION];
		nmap[i] = new Vector3D[MAP_DIMENSION];
		//	tmap[i] = new Vector3D[MAP_DIMENSION];
	}
	FILE* nmapFile;
	FILE* hmapFile;


	if (!hc3d::hc3dToolkit::FileExists(_bstr_t(normalMapName.c_str())) || !hc3d::hc3dToolkit::FileExists(_bstr_t(normalMapName.c_str()))) InitCache(normalMapName, heightMapName);
	nmapFile = fopen(normalMapName.c_str(), "r");
	hmapFile = fopen(heightMapName.c_str(), "r");

	if (nmapFile == 0 || hmapFile == 0) {
		fprintf(stderr, "no readable normal or height map exists\n");
		MessageBoxA(0, heightMapName.c_str(), "dafuq", MB_OK);
		exit(1);
	}
	for (int i = 0; i < MAP_DIMENSION; i++) {
		for (int j = 0; j < MAP_DIMENSION; j++) {
			if (i < MAP_DIMENSION && j < MAP_DIMENSION) {
				fscanf_s(nmapFile, "%f %f %f\n", &nmap[i][j].x, &nmap[i][j].y, &nmap[i][j].z);
				fscanf_s(hmapFile, "%f\n", &hmap[i][j]);
				hmap[i][j] /= 2.0;
			}
			else if (i == MAP_DIMENSION) {
				hmap[i][j] = hmap[i - 1][j];
				nmap[i][j] = nmap[i - 1][j];
			}
			else if (j == MAP_DIMENSION) {
				hmap[i][j] = hmap[i][j - 1];
				nmap[i][j] = nmap[i][j - 1];
			}

			//tmap[i][j] = Vector3D(float(i) / pol, float(j) / pol, 0.0);
		}
	}
	fclose(nmapFile);
	fclose(hmapFile);
	for (int i = 0; i < listNum; i++) {
		CenterGenerate(i);
	}
	for (int i = 0; i < listNum; i++) {
		for (int j = 0; j < LOD_LEVELS; j++) {
			static_lists[i][j] = 0;
		}
	}
	loadedHM = true;
	state = 0;
	busy = false;
}

void Terrain::LoadCollision() {
	const int heightfieldDim = MAP_DIMENSION + 1;
	float *heightfield = new (float[heightfieldDim * heightfieldDim]);

	int highest = -999999, j = 0;
	for (int i = 0; i < heightfieldDim; i++) {
		for (int j = 0; j < heightfieldDim; j++) {
			float z = CalcTerHeight(Vector3D(i * 2 + offset.x, j * 2 + offset.y, 0)) + 20.0;
			heightfield[j * heightfieldDim + i] = z;
			if (z > highest)
				highest = z;
		}
	}
	terrainCollisionBox = Collision::AddTerrain(heightfieldDim, heightfield, offset);
	loadedCol = true;
	state = 0;
	busy = false;
}

void Terrain::DeleteHM() {
	loadedHM = false;
	for (int i = 0; i < listNum; i++) {
		for (int j = 0; j < LOD_LEVELS; j++) {
			if (static_lists[i][j] != 0) {
				glDeleteLists(static_lists[i][j], 1);
			}
		}
		delete[] static_lists[i];
	}
	delete[] static_lists;

	for (int i = 0; i < MAP_DIMENSION; i++) {
		delete[] hmap[i];
		delete[] nmap[i];
	}

	delete[] list_center;
	delete[] hmap;
	delete[] nmap;
	state = 0;
	busy = false;
}

void Terrain::DeleteCollision() {
	loadedCol = false;
	Collision::DeleteTerrain(terrainCollisionBox);
	state = 0;
	busy = false;
}

void Terrain::CenterGenerate(int num) {
	int level = 0;
	int size = 2;
	int i_start = 0 + TEXEL_SIZE * (num % (MAP_DIMENSION / TEXEL_SIZE));
	int i_end = Math::hc3dMin(TEXEL_SIZE + TEXEL_SIZE * (num % (MAP_DIMENSION / TEXEL_SIZE)), MAP_DIMENSION);
	int j_start = 0 + TEXEL_SIZE * (num / (MAP_DIMENSION / TEXEL_SIZE));
	int j_end = Math::hc3dMin(TEXEL_SIZE + TEXEL_SIZE * (num / (MAP_DIMENSION / TEXEL_SIZE)), MAP_DIMENSION);
	list_center[num] = offset + Vector3D((i_start + i_end) / 2 * size, (j_start + j_end) / 2 * size, CalcTerHeight(Vector3D(i_start + i_end, j_start + j_end, 0.0) + offset));
}
void Terrain::LodGenerate(int num, int level) {
	int i_start = TEXEL_SIZE * (num % (MAP_DIMENSION / TEXEL_SIZE));
	int i_end = Math::hc3dMin(TEXEL_SIZE + TEXEL_SIZE * (num % (MAP_DIMENSION / TEXEL_SIZE)), MAP_DIMENSION);
	int j_start = TEXEL_SIZE * (num / (MAP_DIMENSION / TEXEL_SIZE));
	int j_end = Math::hc3dMin(TEXEL_SIZE + TEXEL_SIZE * (num / (MAP_DIMENSION / TEXEL_SIZE)), MAP_DIMENSION);
	if (level % 2 == 1) {
		i_start = Math::hc3dMax(0, i_start - level);
		j_start = Math::hc3dMax(0, j_start - level);
		i_end = Math::hc3dMin(MAP_DIMENSION, i_end + level);
		j_end = Math::hc3dMin(MAP_DIMENSION, j_end + level);
	}

	float size = TERRAIN_SCALE; 
	static_lists[num][level] = glGenLists(1);
	glNewList(static_lists[num][level], GL_COMPILE);
	level = int(pow(2.0, level));
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glBegin(GL_TRIANGLES);
	for (int i = i_start; i < i_end; i += level) {
		for (int j = j_start; j < j_end; j += level) {
			int nextI = Math::hc3dMin((i + level), MAP_DIMENSION);
			int nextJ = Math::hc3dMin((j + level), MAP_DIMENSION);

			Vector3D An, Bn, Cn, Dn;
			Vector3D A, B, C, D;
			Vector3D At, Bt, Ct, Dt;
			A = Vector3D((i)*size, (j)*size, CalcTerHeight(Vector3D(i * size, j * size, .0) + offset)) + offset;
			B = Vector3D(((i + level)*size), (j)*size, CalcTerHeight(Vector3D(nextI * size, j * size, .0) + offset)) + offset;
			C = Vector3D((i + level)*size, ((j + level)*size), CalcTerHeight(Vector3D(nextI * size, nextJ * size, .0) + offset)) + offset;
			D = Vector3D((i)*size, ((j + level)*size), CalcTerHeight(Vector3D(i * size, nextJ * size, .0) + offset)) + offset;
			An = CalcTerNormal(Vector3D(i * size, j * size, 0.0) + offset);
			Bn = CalcTerNormal(Vector3D(nextI * size, j * size, 0.0) + offset);
			Cn = CalcTerNormal(Vector3D(nextI * size, nextJ * size, 0.0) + offset);
			Dn = CalcTerNormal(Vector3D(i * size, nextJ * size, 0.0) + offset);
			At = Vector3D(A.x / pol, A.y / pol, 0.0);
			Bt = Vector3D(B.x / pol, B.y / pol, 0.0);
			Ct = Vector3D(C.x / pol, C.y / pol, 0.0);
			Dt = Vector3D(D.x / pol, D.y / pol, 0.0);

			glNormal3f(An.x, An.y, An.z);
			glTexCoord2f(At.x, At.y);
			glVertex3f(A.x, A.y, A.z);

			glNormal3f(Bn.x, Bn.y, Bn.z);
			glTexCoord2f(Bt.x, Bt.y);
			glVertex3f(B.x, B.y, B.z);

			glNormal3f(Cn.x, Cn.y, Cn.z);
			glTexCoord2f(Ct.x, Ct.y);
			glVertex3f(C.x, C.y, C.z);


			glNormal3f(An.x, An.y, An.z);
			glTexCoord2f(At.x, At.y);
			glVertex3f(A.x, A.y, A.z);

			glNormal3f(Cn.x, Cn.y, Cn.z);
			glTexCoord2f(Ct.x, Ct.y);
			glVertex3f(C.x, C.y, C.z);

			glNormal3f(Dn.x, Dn.y, Dn.z);
			glTexCoord2f(Dt.x, Dt.y);
			glVertex3f(D.x, D.y, D.z);
			//	printf("D.z = %.2f ",D.z);
		}
	}
	glEnd();
	glDisable(GL_DEPTH_TEST);
	glEndList();
}

#pragma endregion TERRAIN_LOAD

void Terrain::setShader(){
	glActiveTexture(GL_TEXTURE0 + Info::GetCurBindTexture());

	GLuint shaderprogram;
	ShaderLib::Get("ground", shaderprogram);

	int curTex = (int)(((int)(Info::getTime()*80.0) % 1000) / 31.25);
	glBindTexture(GL_TEXTURE_2D, caust[curTex]);
	glUniform1i(glGetUniformLocation(
		shaderprogram, "caustics"), Info::GetCurBindTexture());
	Info::RaiseCurBindTexture();
}

float Terrain::calc_height(Vector3D pos) {
	pos -= offset;
	pos /= TERRAIN_SCALE;
	int posX = pos.x;
	int posY = pos.y;
	const float x = pos.x - posX;
	const float y = pos.y - posY;
	float z = 0;
	float hmap00 = hmap[posX][posY];
	float hmap01, hmap10, hmap11;
	bool flag = false;
	if (posX + 1 >= MAP_DIMENSION) {
		hmap10 = CalcTerHeight(Vector3D((posX + 1) * TERRAIN_SCALE, posY * TERRAIN_SCALE, 0));
		flag = true;
	}
	else hmap10 = hmap[posX + 1][posY];
	if (posY + 1 >= MAP_DIMENSION) {
		hmap01 = CalcTerHeight(Vector3D(posX * TERRAIN_SCALE, (posY + 1) * TERRAIN_SCALE, 0));
		flag = true;
	}
	else hmap01 = hmap[posX][posY + 1];
	if (flag) hmap11 = CalcTerHeight(Vector3D((posX + 1) * TERRAIN_SCALE, (posY + 1) * TERRAIN_SCALE, 0));
	else hmap11 = hmap[posX + 1][posY + 1];

	z += hmap00 * (1.0 - x)*(1.0 - y);
	z += hmap10 * (x)*(1.0 - y);
	z += hmap11 * (x)*(y);
	z += hmap01 * (1.0 - x)*(y);
	return z * TERRAIN_SCALE / 2.0;
}

Vector3D Terrain::calc_normal(Vector3D pos) {
	Vector3D z(0, 0, 0);

	pos -= offset;
	pos /= TERRAIN_SCALE;
	int posX = pos.x;
	int posY = pos.y;
	const float x = pos.x - posX;
	const float y = pos.y - posY;

	Vector3D nmap00 = nmap[posX][posY];
	Vector3D nmap01, nmap10, nmap11;
	bool flag = false;
	if (posX + 1 >= MAP_DIMENSION) {
		nmap10 = CalcTerNormal(Vector3D((posX + 1) * TERRAIN_SCALE, posY * TERRAIN_SCALE, 0));
		flag = true;
	}
	else nmap10 = nmap[posX + 1][posY];
	if (posY + 1 >= MAP_DIMENSION) {
		nmap01 = CalcTerNormal(Vector3D(posX * TERRAIN_SCALE, (posY + 1) * TERRAIN_SCALE, 0));
		flag = true;
	}
	else nmap01 = nmap[posX][posY + 1];
	if (flag) nmap11 = CalcTerNormal(Vector3D((posX + 1) * TERRAIN_SCALE, (posY + 1) * TERRAIN_SCALE, 0));
	else nmap11 = nmap[posX + 1][posY + 1];

	z += nmap00 * (1.0 - x)*(1.0 - y);
	z += nmap10 * (x)*(1.0 - y);
	z += nmap11 * (x)*(y);
	z += nmap01 * (1.0 - x)*(y);
	return z;
}
float Terrain::dist(Vector3D a, Vector3D b) {
	Vector3D c = b - a;
	return sqrt(c.x*c.x + c.y*c.y + c.z * c.z);
}
float Terrain::dot(Vector3D a, Vector3D b) {
	return a * b;
}
void Terrain::Draw(){
	Init();
	if (!loadedHM) return;


	Renderer::SetShader("ground");
	Renderer::UseMaterial("beach1");
	Renderer::UseMaterial("mountain1");
	Renderer::UseMaterial("snow1");
	Renderer::UseMaterial("grass1");
	Renderer::UseMaterial("mask1");
	Renderer::UseMaterial("mask2");
	Renderer::Process();

	if (Info::GetShader()) {
		setShader();
	}

	for (int i = 0; i < listNum; i++) {
		if (Info::GetRefract()) {
			if (list_center[i].z > 40.0) continue;
		}
		float dst = dist(Camera::getPosition(), list_center[i]);
		Vector3D a = list_center[i] - Camera::getPosition();
		a.Normalize();
		Vector3D b = Info::GetEyeNormal();
		b.Normalize();
		float norm_vec = a*b;
		if (norm_vec < 0.1 && dst > 300) continue;
		if (Info::GetReflect()) if (norm_vec < 0.5 && dst > 192) continue;
		LoD = 4;
		if (dst < 192 * TERRAIN_SCALE / 2) LoD = 0;
		else if (dst < 380 * TERRAIN_SCALE / 2) LoD = 1;
		else if (dst < 512 * TERRAIN_SCALE / 2) LoD = 2;
		else if (dst < 1024 * TERRAIN_SCALE / 2) LoD = 3;

		if (static_lists[i][LoD] == 0) {
			for (int j = 0; j < LOD_LEVELS; j++) {
				if (static_lists[i][j] != 0) {
					glDeleteLists(static_lists[i][j], 1);
					static_lists[i][j] = 0;
				}
			}
			LodGenerate(i, LoD);
			glCallList(static_lists[i][LoD]);
		}
		else glCallList(static_lists[i][LoD]);
	}

	Renderer::DropSettings();
	glUseProgram(0);

}


/*Terrains methods*/
TerrainList Terrains::list;

Terrains::Terrains() {
	wait = true;
}

void Terrains::Init() {
	for (size_t i = 0; i < list.size(); i++) list[i].first->InitMaterials();

	for (size_t i = 0; i < list.size(); i++) 
		list[i].first->InitHM();

	if (wait)
	while (1) {
		bool finished = true;
		for (size_t i = 0; i < list.size(); i++) {
			if (list[i].first->Busy()) {
				finished = false;
				break;
			}
		}
		if (finished) break;
	}

	for (size_t i = 0; i < list.size(); i++)
		list[i].first->InitCollision();

	if (wait)
	while (1) {
		bool finished = true;
		for (size_t i = 0; i < list.size(); i++) {
			if (list[i].first->Busy()) {
				finished = false;
				break;
			}
		}
		if (finished) break;
	}
	wait = false;
}

void Terrains::Draw() {
	Init();
	for (size_t i = 0; i < list.size(); i++) list[i].first->Draw();
}

void Terrains::AddTerrain(std::string path, Vector3D size, Vector3D location) {
	auto ter = GetTer(path, size);
	Terrains::list.push_back(TerrainList::value_type(ter, location));
}