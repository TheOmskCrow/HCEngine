#include "StdAfx.h"
#include "Terrain.h"
#include <Windows.h>
#include <hc3dContentLoader.h>
#include <hc3dToolkit.h>
#include <hc3dObjects.h>
#include <hc3dMath.h>
#include "Material.h"
//#include "Collision.h"
#include <iostream>
#include <thread>
#include <mutex>

#define TEXEL_SIZE 64
#define MAP_DIMENSION 512
#define LOD_LEVELS 6
#define VISIBILITY_THRESHOLD 6000
using namespace hc3d;

std::mutex iMutex;
GLuint Terrain::shaderprogram = 0;

bool Terrain::reflect = false;
bool Terrain::refract = false;
int Terrain::dimension = 512; //duplication
GLuint* Terrain::caust = 0;
GLuint* Terrain::texture = 0;

Terrain::Terrain(std::string mapName, Vector3D offset) {
	this->mapName = mapName;
	this->offset = offset;
	pol = 20;
	loaded = false;
	loading = false;
}

bool Terrain::Visible() {
	return dist(offset + Vector3D(MAP_DIMENSION, MAP_DIMENSION, 0), Camera::getPosition()) < VISIBILITY_THRESHOLD;
}

DWORD_PTR WINAPI SingleTerrainLoadThread(void* p) {
	Terrain *ptr = (Terrain *) p;
	ptr->Load();
	return 0;
}

void Terrain::Init(){
	if (loading) return;
	bool visibility = Visible();
	if (!visibility && loaded) return Delete();
	else if (!visibility || loaded) return;

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
		mat.SetDiffuse("Texture/beach/beach1.png");

		glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
		Info::RaiseCurTextureNum();
		mat.SetHeight("Texture/beach/beach1_height.png", 0.005f);
		mat.SetAmbient(ATMOSPHERE);
//		glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
//		Info::RaiseCurTextureNum();
//		mat.SetNormal("Texture/beach/beach1_normal.png", 0.3f);

//		glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
//		Info::RaiseCurTextureNum();
//		mat.SetSpecular("Texture/beach/beach1_specular.png", 9.0f, 1.0f);
		MatLib::Add("beach1", mat);
	}

	if (!MatLib::IsUsed("mountain1")) {
		Material mat;
		glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
		Info::RaiseCurTextureNum();
		mat.SetDiffuse("Texture/rock/rock1.png");

		//glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
		//Info::RaiseCurTextureNum();
		//mat.SetHeight("Texture/beach/beach1_height.png", 0.005f);

		glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
		Info::RaiseCurTextureNum();
		mat.SetNormal("Texture/rock/rock1_normal.png", 0.3f);

		glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
		Info::RaiseCurTextureNum();
		mat.SetSpecular("Texture/rock/rock1_specular.png", 20.0f, 0.7f);
		mat.SetAmbient(ATMOSPHERE);
		MatLib::Add("mountain1", mat);
	}
	if (!MatLib::IsUsed("snow1")) {
		Material mat;
		glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
		Info::RaiseCurTextureNum();
		mat.SetDiffuse("Texture/snow/snow1.png");

		//glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
		//Info::RaiseCurTextureNum();
		//mat.SetHeight("Texture/beach/beach1_height.png", 0.005f);

		glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
		Info::RaiseCurTextureNum();
		mat.SetNormal("Texture/snow/snow1_normal.png", 0.1f);

		glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
		Info::RaiseCurTextureNum();
		mat.SetSpecular("Texture/snow/snow1_specular.png", 15.0f, 1.0f);
		mat.SetAmbient(ATMOSPHERE);
		MatLib::Add("snow1", mat);
	}


	if (!texture) {
		texture = new GLuint[17];
		glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
		Info::RaiseCurTextureNum();
		//texture[0] = LoadTexture("Texture/stoneBeach2.png");
		glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
		Info::RaiseCurTextureNum();
		texture[1] = LoadTexture("Texture/rock22.png");
		glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
		Info::RaiseCurTextureNum();
		texture[2] = LoadTexture("Texture/caustics.png");
		glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
		Info::RaiseCurTextureNum();
		texture[3] = LoadTexture("Texture/hei1.png");
		glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
		Info::RaiseCurTextureNum();
		texture[4] = CreateTexture(GL_RGBA, 2);
		glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
		Info::RaiseCurTextureNum();
		texture[6] = LoadTexture("Texture/height2.png");
		glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
		Info::RaiseCurTextureNum();
		texture[8] = LoadTexture("Texture/grass1.png");
		glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
		Info::RaiseCurTextureNum();
		texture[9] = LoadTexture("Texture/rock5_height.png");
		glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
		Info::RaiseCurTextureNum();
	}
	if(!shaderprogram) shaderprogram = LoadShader("Shaders/ground.vert", "Shaders/ground.frag");

	DWORD_PTR cpuNum = hc3dToolkit::GetNumCPUs();
	HANDLE m_thread;
	Terrain* iparam = this;

	DWORD_PTR i = rand() % cpuNum;
	size_t activatedCpu = 0;
	DWORD thStatus;
	DWORD m_id = 0;
	DWORD m_mask = 1 << i;
	m_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SingleTerrainLoadThread, (LPVOID)iparam, NULL, &m_id);
	loading = true;
	SetThreadAffinityMask(m_thread, m_mask);

}

void Terrain::Delete() {
	loaded = false;
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
	Collision::DeleteTerrain(terrainCollisionBox);
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

	for(int i=0;i<MAP_DIMENSION;i++)
	for(int j=0;j<MAP_DIMENSION;j++)
	{
	memcpy(&C,tbmpbuffer + tBMP.bfOffBits + 0 + i*3 + MAP_DIMENSION*j*3, 1);
	z+=C;
	memcpy(&C,tbmpbuffer + tBMP.bfOffBits + 1 + i*3 + MAP_DIMENSION*j*3, 1);
	z+=C;
	memcpy(&C,tbmpbuffer + tBMP.bfOffBits + 2 + i*3 + MAP_DIMENSION*j*3, 1);
	z+=C;
	tmp[i][j] = z-20;
	hmap[i][j] = 0;//+(rand()%1000)/1000.0-1.0;
	//	printf("hmap[i][j] = %.2f\n", hmap[i][j]);
	z = 0.0;
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

void Terrain::Load(){
	std::lock_guard<std::mutex> lock(iMutex);
	std::string normalMapName = mapName + std::string(".normal");
	std::string heightMapName = mapName + std::string(".height");

	listNum = MAP_DIMENSION * MAP_DIMENSION / TEXEL_SIZE / TEXEL_SIZE;
	long lSize;
	static_lists = new int*[listNum];
	for (int i = 0; i < listNum; i++) {
		static_lists[i] = new int[LOD_LEVELS];//64x64 6 levels
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
		MessageBoxA(0, "Can not open readable normal or height map", "dafuq", MB_OK);
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
	terrainCollisionBox = Collision::AddTerrain(MAP_DIMENSION, hmap, offset);
	loading = false;
	loaded = true;
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

	float size = 2.0f; // make class variable
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
void Terrain::setShader(){
	Vector3D sun = Info::GetSun();
	Vector3D pos = Camera::getPosition();
	//Shadow shadow;
	glUniform1i(glGetUniformLocation(
		shaderprogram, "shadowing"), Info::GetShadow());
	glActiveTexture(GL_TEXTURE0 + Info::GetCurBindTexture());
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glUniform1i(glGetUniformLocation(
		shaderprogram, "road"), Info::GetCurBindTexture());
	Info::RaiseCurBindTexture();

	glActiveTexture(GL_TEXTURE0 + Info::GetCurBindTexture());
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glUniform1i(glGetUniformLocation(
		shaderprogram, "grass"), Info::GetCurBindTexture());
	Info::RaiseCurBindTexture();

	glActiveTexture(GL_TEXTURE0 + Info::GetCurBindTexture());

	int curTex = (int)(((int)(Info::getTime()*80.0) % 1000) / 31.25);
	glBindTexture(GL_TEXTURE_2D, caust[curTex]);
	glUniform1i(glGetUniformLocation(
		shaderprogram, "caustics"), Info::GetCurBindTexture());
	Info::RaiseCurBindTexture();

	glActiveTexture(GL_TEXTURE0 + Info::GetCurBindTexture());
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glUniform1i(glGetUniformLocation(
		shaderprogram, "shadowMap1"), Info::GetCurBindTexture());
	Info::RaiseCurBindTexture();

	glActiveTexture(GL_TEXTURE0 + Info::GetCurBindTexture());
	glBindTexture(GL_TEXTURE_2D, texture[8]);
	glUniform1i(glGetUniformLocation(
		shaderprogram, "grass2"), Info::GetCurBindTexture());
	Info::RaiseCurBindTexture();

	glActiveTexture(GL_TEXTURE0 + Info::GetCurBindTexture());
	glBindTexture(GL_TEXTURE_2D, texture[9]);
	glUniform1i(glGetUniformLocation(
		shaderprogram, "grass3"), Info::GetCurBindTexture());
	Info::RaiseCurBindTexture();

	if (reflect) glUniform1i(glGetUniformLocation(
		shaderprogram, "reflect"), 1);
	else glUniform1i(glGetUniformLocation(
		shaderprogram, "reflect"), 0);
	glActiveTexture(GL_TEXTURE0 + Info::GetCurBindTexture());
	glBindTexture(GL_TEXTURE_2D, texture[3]);
	glUniform1i(glGetUniformLocation(
		shaderprogram, "hei1"), Info::GetCurBindTexture());
	Info::RaiseCurBindTexture();

	glActiveTexture(GL_TEXTURE0 + Info::GetCurBindTexture());
	glBindTexture(GL_TEXTURE_2D, texture[4]);
	glUniform1i(glGetUniformLocation(
		shaderprogram, "hei2"), Info::GetCurBindTexture());
	Info::RaiseCurBindTexture();

	glUniform3f(glGetUniformLocation(
		shaderprogram, "atmosphere"), Info::GetAtmoColor().x, Info::GetAtmoColor().y, Info::GetAtmoColor().z);

	glActiveTexture(GL_TEXTURE0 + Info::GetCurBindTexture());
	glBindTexture(GL_TEXTURE_2D, texture[6]);
	glUniform1i(glGetUniformLocation(
		shaderprogram, "gr_hei"), Info::GetCurBindTexture());
	Info::RaiseCurBindTexture();

	glUniform1i(glGetUniformLocation(
		shaderprogram, "flash"), Info::GetFlash());
	glUniform1f(glGetUniformLocation(
		shaderprogram, "shadowDist"), Info::GetShadowDist());

	glUniform1f(glGetUniformLocation(
		shaderprogram, "time"), (Info::getTime() / 100.0));
	//std::cout << Info::getTime() << std::endl;
	glUniform1f(glGetUniformLocation(
		shaderprogram, "Width"), Info::width());
	glUniform1f(glGetUniformLocation(
		shaderprogram, "Height"), Info::height());
	glUniform3f(glGetUniformLocation(
		shaderprogram, "eyeNorm"), Info::GetEyeNormal().x, Info::GetEyeNormal().y, Info::GetEyeNormal().z);
	glUniform3f(glGetUniformLocation(
		shaderprogram, "eyePos"), pos.x, pos.y, pos.z);
	if (reflect)
		glUniform3f(glGetUniformLocation(
		shaderprogram, "lightPos"), sun.x, sun.y, -sun.z);
	else
		glUniform3f(glGetUniformLocation(
		shaderprogram, "lightPos"), sun.x, sun.y, sun.z);
}
void Terrain::shadow_shot(int num) {
	int width = Info::width();
	int height = Info::height();
	int x_offset = (2048 - width) / 2;
	int y_offset = (2048 - height) / 2;
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0 + num);
	glBindTexture(GL_TEXTURE_2D, texture[9 + num - 1]);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 0, 0, int(width), int(height), 0);
	//glCopyTexSubImage2D(GL_TEXTURE_2D, 0,x_offset,y_offset,0,0, int(width),int(height));
}
float Terrain::calc_height(Vector3D pos) {
	pos -= offset;
	pos /= 2.0;
	int posX = pos.x;
	int posY = pos.y;
	const float x = pos.x - posX;
	const float y = pos.y - posY;
	float z = 0;
	float hmap00 = hmap[posX][posY];
	float hmap01, hmap10, hmap11;
	bool flag = false;
	if (posX + 1 >= MAP_DIMENSION) {
		hmap10 = CalcTerHeight(Vector3D((posX + 1) * 2, posY * 2, 0));
		flag = true;
	}
	else hmap10 = hmap[posX + 1][posY];
	if (posY + 1 >= MAP_DIMENSION) {
		hmap01 = CalcTerHeight(Vector3D(posX * 2, (posY + 1) * 2, 0));
		flag = true;
	}
	else hmap01 = hmap[posX][posY + 1];
	if (flag) hmap11 = CalcTerHeight(Vector3D((posX + 1) * 2, (posY + 1) * 2, 0));
	else hmap11 = hmap[posX + 1][posY + 1];

	z += hmap00 * (1.0 - x)*(1.0 - y);
	z += hmap10 * (x)*(1.0 - y);
	z += hmap11 * (x)*(y);
	z += hmap01 * (1.0 - x)*(y);
	return z;
}
Vector3D Terrain::calc_normal(Vector3D pos) {
	Vector3D z(0, 0, 0);

	pos -= offset;
	pos /= 2.0;
	int posX = pos.x;
	int posY = pos.y;
	const float x = pos.x - posX;
	const float y = pos.y - posY;

	Vector3D nmap00 = nmap[posX][posY];
	Vector3D nmap01, nmap10, nmap11;
	bool flag = false;
	if (posX + 1 >= MAP_DIMENSION) {
		nmap10 = CalcTerNormal(Vector3D((posX + 1) * 2, posY * 2, 0));
		flag = true;
	}
	else nmap10 = nmap[posX + 1][posY];
	if (posY + 1 >= MAP_DIMENSION) {
		nmap01 = CalcTerNormal(Vector3D(posX * 2, (posY + 1) * 2, 0));
		flag = true;
	}
	else nmap01 = nmap[posX][posY + 1];
	if (flag) nmap11 = CalcTerNormal(Vector3D((posX + 1) * 2, (posY + 1) * 2, 0));
	else nmap11 = nmap[posX + 1][posY + 1];

	z += nmap00 * (1.0 - x)*(1.0 - y);
	z += nmap10 * (x)*(1.0 - y);
	z += nmap11 * (x)*(y);
	z += nmap01 * (1.0 - x)*(y);
	return z;
}
float Terrain::dist(Vector3D a, Vector3D b) {
	Vector3D c = b - a;
	return sqrt(c.x*c.x + c.y*c.y);
}
float Terrain::dot(Vector3D a, Vector3D b) {
	return a * b;
}
void Terrain::Draw(){
	Init();
	if (!loaded || loading) return;
	if (Info::GetShader()) {
		glUseProgram(shaderprogram);
		setShader();
	}

	Renderer::SetShader(shaderprogram);
	Renderer::UseMaterial("beach1");
	Renderer::UseMaterial("mountain1");
	Renderer::UseMaterial("snow1");
	Renderer::Process();

	for (int i = 0; i < listNum; i++) {
		if (refract) {
			if (list_center[i].z > 40.0) continue;
		}
		float dst = dist(Camera::getPosition(), list_center[i]);
		Vector3D a = list_center[i] - Camera::getPosition();
		a.Normalize();
		Vector3D b = Info::GetEyeNormal();
		b.Normalize();
		float norm_vec = a*b;
		if (norm_vec < 0.3 && dst > 192) continue;
		if (reflect) if (norm_vec < 0.5 && dst > 192) continue;
		LoD = 4;
		if (dst < 128) LoD = 0;
		//else if (dst > 256) LoD = 1;
		else if (dst < 380) LoD = 1;
		//else if(dst > 512) LoD = 4;
		else if(dst < 1024) LoD = 2; 
		//else if (dst < 1024) LoD = 3;
		else if (dst < 4096) LoD = 3;
		/*else if (dst > 384) LoD = 8;
		else if (dst > 448) LoD = 9;
		*/
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
	/*glEnable(GL_DEPTH_TEST);
	glBegin(GL_TRIANGLES);
	float pol_size = 2048;
	float grid_size = 2048;
	for (int a = -2048 - 2048 * 10; a <= 4096 + 2048 + 2048 * 10; a += 4096) {
		for (int b = -2048 - 2048 * 10; b <= 4096 + 2048 + 2048 * 10; b += 4096) {
			//if (a == 2048 && b == 2048) continue;
			if (dist(Camera::getPosition(), Vector3D(a, b, 0)) > 1000000) continue;
			for (float x = -grid_size + a; x < grid_size + a; x += pol_size) {
				for (float y = -grid_size + b; y < grid_size + b; y += pol_size) {
					float pol = 20.0;
					glNormal3f(0, 0, 1);
					glTexCoord2d(x / pol, y / pol);
					glVertex3f(x, y, -10);

					glNormal3f(0, 0, 1);
					glTexCoord2d((x + pol_size) / pol, y / pol);
					glVertex3f(x + pol_size, y, -10);

					glNormal3f(0, 0, 1);
					glTexCoord2d(x / pol, (y + pol_size) / pol);
					glVertex3f(x, y + pol_size, -10);

					glNormal3f(0, 0, 1);
					glTexCoord2d(x / pol, (y + pol_size) / pol);
					glVertex3f(x, y + pol_size, -10);

					glNormal3f(0, 0, 1);
					glTexCoord2d((x + pol_size) / pol, y / pol);
					glVertex3f(x + pol_size, y, -10);

					glNormal3f(0, 0, 1);
					glTexCoord2d((x + pol_size) / pol, (y + pol_size) / pol);
					glVertex3f(x + pol_size, y + pol_size, -10);
				}
			}
		}
	}
	glEnd();
	glDisable(GL_DEPTH_TEST);*/
	Renderer::DropSettings();
	glUseProgram(0);

}