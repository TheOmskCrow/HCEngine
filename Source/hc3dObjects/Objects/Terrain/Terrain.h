#pragma once
#include <Object.h>

namespace hc3d {
	class Terrain : public Object
	{
	public: //public methods
		Terrain(std::string mapName, Vector3D offset);
		void Init(); //just overload
		void InitMaterials();
		HANDLE InitHM(); // heightmap init
		HANDLE InitCollision(); // heightmap init
		void Draw();
		void setShader();
		float calc_height(Vector3D);
		Vector3D calc_normal(Vector3D);
		static GLuint get_shaderprogram();
		bool Loaded() { return loadedHM; }
		bool Busy() { return busy; }

		void Process();
		

	public: // public fields
		int state;
		int list_id;
		int LoD;
		float **hmap;
		Vector3D **nmap;
		Vector3D **tmap;
		bool **drawed_map;
		TerrainHF * terrainCollisionBox;

	public: // static public fields
		static int dimension;

	private: //private methods
		HANDLE RunMultiThreadTask();
		inline bool Visible();
		inline bool Accessible();
		float dist(Vector3D a, Vector3D b);
		float dot(Vector3D a, Vector3D b);
		
		void LodGenerate(int num, int level);
		void CenterGenerate(int num);
		void InitCache(std::string normalMapName, std::string heightMapName);

		void LoadHM();
		void LoadCollision();
		void DeleteHM();
		void DeleteCollision();


	private: //private fields
		int **static_lists;
		int pol;
		Vector3D *list_center;
		GLfloat *r_vertex; int r_vertex_size;
		GLfloat *r_vertex_n; int r_vertex_n_size;
		GLfloat *r_vertex_t; int r_vertex_t_size;
		GLuint* BufferName;
		std::string mapName;
		Vector3D offset;
		bool exploded;
		bool loadedHM;
		bool loadedCol;
		bool busy;
		size_t listNum;

	private://static fields
		static GLuint *caust;
	};

	typedef std::vector<std::pair<hc3d::Terrain*, hc3d::Vector3D> > TerrainList;

	class Terrains : public Object
	{
	public:
		Terrains();
		void Init();
		void Draw();
		static void AddTerrain(std::string path, Vector3D size, Vector3D location);

	public:
		static TerrainList list;
		bool wait;
	};

}