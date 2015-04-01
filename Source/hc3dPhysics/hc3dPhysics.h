#pragma once
#include "Bullet\Bullet-C-Api.h"
#include "Bullet\btBulletCollisionCommon.h"
#include "Bullet\btBulletDynamicsCommon.h"
#include "Bullet\BulletCollision\CollisionShapes\btHeightfieldTerrainShape.h"
#include "Bullet\BulletCollision/CollisionDispatch/btGhostObject.h"
#include "Bullet\BulletDynamics/Character/btKinematicCharacterController.h"
#include "BulletDynamics/Vehicle/btRaycastVehicle.h"

#include <hc3dContentLoader.h>
#include <hc3dMath.h>
#include <vector>
#include <time.h>
#include "dllmain.h"

namespace hc3d {
	class HC3DPHYSICS_API TerrainHF
	{
	public:
		TerrainHF();
		float *heightfield;
		btHeightfieldTerrainShape* heightfieldShape;
		btDefaultMotionState* myMotionState;
		btRigidBody* body;
	};

	class HC3DPHYSICS_API Collision
	{
	public:
		Collision(void);
		~Collision(void);
		static void Init();
		static TerrainHF *AddTerrain(int size, float **hmap, Vector3D offset);
		static void DeleteTerrain(TerrainHF * terrain);
		static void ResolveCollision();
		static void AddRigidBox(Vector3D size, Vector3D position, btScalar mass);
		static void AddRigidSphere(Vector3D size, Vector3D position, btScalar mass);
		static int AddBody(Vector3D size, Vector3D position, Vector3D rotation, btScalar mass, std::vector<Vector3D> vertexList, bool isStatic = true);
		static void AddVehicle();
		static void Explode(Vector3D point, float force);

		static btScalar* GetObjectMatrix(int i);
		static void specialKeyboard(int key, int x, int y);
		static void specialKeyboardUp(int key, int x, int y);
		static btDiscreteDynamicsWorld* dynamicsWorld;
	private:
		static std::vector<btRigidBody*> objects;
		static std::vector<Vector3D> objectOrigins;
		static btRigidBody* mPlayerObject;
		static btKinematicCharacterController* m_character;
		static btPairCachingGhostObject* m_ghostObject;

		static btRigidBody* m_carChassis;
		static btRaycastVehicle::btVehicleTuning m_tuning;
		static btVehicleRaycaster* m_vehicleRayCaster;
		static btRaycastVehicle* m_vehicle;
		static btCollisionShape* m_wheelShape;
	private:
		static float CalcTerrainHeight(size_t size, float **hmap, Vector3D position);
		static void QuaternionToEulerXYZ(const btQuaternion &quat, btVector3 &euler);
		static btDefaultCollisionConfiguration* collisionConfiguration;
		static btCollisionDispatcher* dispatcher;
		static btBroadphaseInterface* overlappingPairCache;
		static btSequentialImpulseConstraintSolver* solver;
		static float _min(float, float);
		static float _max(float, float);
	};
}
