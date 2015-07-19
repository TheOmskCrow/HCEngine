#pragma once
#include "../dllstorage.h"
#include <map>
#include <string>
#include "hc3dContentLoader.h"
#include "hc3dMath.h"

namespace hc3d {
#define NONE 0
#define ATMOSPHERE 1

	class HC3DSTORAGE_API Material
	{
		friend class Renderer;
	public:
		bool SetDiffuse(GLuint diffuseMap, Vector3D farColor = Vector3D(0.5f, 0.5f, 0.5f), float farDist = 400.0f);
		bool SetDiffuse(std::string diffuseMapName, Vector3D farColor = Vector3D(0.5f, 0.5f, 0.5f), float farDist = 400.0f);

		bool SetNormal(GLuint normalMap, float bumpVal = 1.0f);
		bool SetNormal(std::string normalMapName, float bumpVal = 1.0f);

		bool SetHeight(GLuint heightMap, float parallaxOffset = 0.01f);
		bool SetHeight(std::string heightMapName, float parallaxOffset = 0.01f);

		bool SetSpecular(GLuint specularMap, float specPow = 30.0f, float specVal = 1.0f);
		bool SetSpecular(std::string specularMapName, float specPow = 30.0f, float specVal = 1.0f);

		bool SetAmbient(int ambientType);
		bool DeleteMaterial();

		Material();
		~Material();
	private:
		GLuint diffuseMap; //main texture
		GLuint normalMap; //normals of main texture
		GLuint heightMap; //heights of main texture
		GLuint specularMap; //specular parts of main texture

		int ambientType; //type of ambient lighting

		bool _initialized; //initialization of material
		bool _useBump; //usage of normals (bump)
		bool _useParallax; //usage of height (parallax)
		bool _useSpecular; //usage of specularity

		float bumpVal; //strength of bump 0 -> 1
		float parallaxOffset; //offset of parallax 0 -> inf
		float specPow; //power of specular reflection 1 -> inf
		float specVal; //brightness of specular 0 -> 1

		Vector3D diffuseFarColor; //Far color of diffuse texture
		float diffuseFarDist; //Distance after which color will become far
	};

	typedef std::map<std::string, Material> Materials;
	typedef std::map<GLuint, bool> GLuintMap;

	class HC3DSTORAGE_API MatLib{
		friend class Material;
	private:
		static Materials materials;
		static GLuintMap usedTextureID;

	public:
		static bool Add(const std::string name, IN const Material data);
		static bool Get(const std::string& name, OUT Material& data);
		static bool IsUsed(const std::string);
		static bool IsUsed(const GLuint& textureID);

	private:
		static bool Use(const GLuint textureID);
		static bool UnUse(const GLuint textureID);
	};
}