#pragma once
#include <Object.h>

namespace hc3d {
	class Terrain : public Object
	{
	public: //public methods
		Terrain(std::string mapName, Vector3D offset);
		void Init();
		void Draw();
		void shadow_shot(int);
		void setShader();
		float calc_height(Vector3D);
		Vector3D calc_normal(Vector3D);
		static GLuint get_shaderprogram();
		bool Loaded() { return loaded; }
		void Load();

	public: // public fields
		int list_id;
		int LoD;
		float **hmap;
		Vector3D **nmap;
		Vector3D **tmap;
		bool **drawed_map;
		TerrainHF * terrainCollisionBox;

	public: // static public fields
		static int dimension;
		static bool reflect;
		static bool refract;
		static GLuint *texture;

	private: //private methods
		inline bool Visible();
		float dist(Vector3D a, Vector3D b);
		float dot(Vector3D a, Vector3D b);
		void Delete();
		void LodGenerate(int num, int level);
		void CenterGenerate(int num);
		void InitCache(std::string normalMapName, std::string heightMapName);

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
		bool loaded;
		bool loading;
		size_t listNum;

	private://static fields
		static GLuint shaderprogram;
		static GLuint *caust;
	};

}