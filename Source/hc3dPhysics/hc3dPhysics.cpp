#include "StdAfx.h"
#include "hc3dPhysics.h"
#include "GLDebugDraw.h"
#include "Control/Camera.h"
#include "Control/Info.h"
#include <math.h>
#include <iostream>



#define PLAYER_MASS 80
#define CUBE_HALF_EXTENTS 1

using namespace hc3d;


btDefaultCollisionConfiguration* Collision::collisionConfiguration;
btCollisionDispatcher* Collision::dispatcher;
btBroadphaseInterface* Collision::overlappingPairCache;
btSequentialImpulseConstraintSolver* Collision::solver;
btDiscreteDynamicsWorld* Collision::dynamicsWorld;
btRigidBody *Collision::mPlayerObject;
std::vector<btRigidBody*> Collision::objects;
std::vector<Vector3D> Collision::objectOrigins;
GLDebugDrawer debugDrawer;
btKinematicCharacterController* Collision::m_character;
btPairCachingGhostObject* Collision::m_ghostObject;

btRigidBody* Collision::m_carChassis;
btRaycastVehicle::btVehicleTuning Collision::m_tuning;
btVehicleRaycaster* Collision::m_vehicleRayCaster;
btRaycastVehicle* Collision::m_vehicle;
btCollisionShape* Collision::m_wheelShape;


//   indexRightAxis = 0; 
//   indexUpAxis = 2; 
//   indexForwardAxis = 1; 
const int maxProxies = 32766;
const int maxOverlap = 65535;

///btRaycastVehicle is the interface for the constraint that implements the raycast vehicle
///notice that for higher-quality slow-moving vehicles, another approach might be better
///implementing explicit hinged-wheel constraints with cylinder collision, rather then raycasts
float   gEngineForce = 0.f;
float   gBreakingForce = 0.f;

float   maxEngineForce = 100000.f;//this should be engine/velocity dependent
float   maxBreakingForce = 1000.f;

float   gVehicleSteering = 0.f;
float   steeringIncrement = 0.04f;
float   steeringClamp = 0.3f;
float   wheelRadius = 10.f;
float   wheelWidth = 6.f;
float   wheelFriction = 1000;//BT_LARGE_FLOAT;
float   suspensionStiffness = 20.f;
float   suspensionDamping = 2.3f;
float   suspensionCompression = 4.4f;
float   rollInfluence = 0.1f;//1.0f;
int rightIndex = 0;
int upIndex = 2;
int forwardIndex = 1;
float offsetZ = 300.0f;

btVector3 wheelDirectionCS0(0, 0, -1);
btVector3 wheelAxleCS(1, 0, 0);
btScalar suspensionRestLength(0.6);

TerrainHF::TerrainHF() {

}

Collision::Collision(void)
{

}


Collision::~Collision(void)
{
}

float Collision::CalcTerrainHeight(size_t size, float **hmap, Vector3D pos) {
	pos /= 2.0;
	int posX = pos.x;
	int posY = pos.y;
	const float x = pos.x - posX;
	const float y = pos.y - posY;
	float z = 0;
	if (posX > size) return -10.0;
	if (posX < 0) return -10.0;
	if (posY > size) return -10.0;
	if (posY < 0) return -10.0;
	posX = min(posX, size - 2);
	posY = min(posY, size - 2);
	z += hmap[posX][posY] * (1.0 - x)*(1.0 - y);
	z += hmap[posX + 1][posY] * (x)*(1.0 - y);
	z += hmap[posX + 1][posY + 1] * (x)*(y);
	z += hmap[posX][posY + 1] * (1.0 - x)*(y);
	return z;
}

void Collision::DeleteTerrain(TerrainHF *terrain) {
	dynamicsWorld->removeRigidBody(terrain->body);
	delete terrain->body;
	delete terrain->myMotionState;
	delete terrain->heightfieldShape;
	delete[] terrain->heightfield;
	delete terrain;
}

TerrainHF * Collision::AddTerrain(int size, float** hmap, Vector3D offset) {
	TerrainHF *ter = new TerrainHF();
	float *heightfield = new (float[size * size]);
	ter->heightfield = heightfield;

	int highest = -999999, j = 0;
	for (int i = 0; i <= size; i++)
		for (int j = 0; j <= size; j++) {
			float z = CalcTerrainHeight(size, hmap, Vector3D(i * 2, j * 2, 0)) + 20.0;
			heightfield[j * size + i] = z;
			if (z > highest)
			highest = z;
	}

	btHeightfieldTerrainShape* heightfieldShape = new btHeightfieldTerrainShape(size, size, heightfield, 1000, 2, true, false);
	ter->heightfieldShape = heightfieldShape;

	heightfieldShape->setLocalScaling(btVector3(2, 2, 1));
	heightfieldShape->setUseZigzagSubdivision();
	btCollisionShape* trimeshShape = heightfieldShape;

	float mass = 0.f;
	btTransform   startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(btVector3(size, size, 480));//TODO: WTF is 480???
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	ter->myMotionState = myMotionState;

	btVector3 localInertia(0, 0, 0);

	heightfieldShape->calculateLocalInertia(mass, localInertia);

	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, trimeshShape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);
	ter->body = body;

	body->setFriction(0.9);
	body->translate(btVector3(offset.x, offset.y, offset.z));
	dynamicsWorld->addRigidBody(body); //add the body to the dynamics world
	return ter;
}

void Collision::Init() {
	//Creating a static shape which will act as ground

	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new	btCollisionDispatcher(collisionConfiguration);
	overlappingPairCache = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver;
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0, 0, -100));

	{
		//btCollisionShape* groundShape = new btBoxShape(btVector3(4000, 4000, 4000));
		btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 0, 1), 5);
		//objects.push_back(groundShape);

		btScalar mass = 0; //rigidbody is static if mass is zero, otherwise dynamic
		btVector3 localInertia(0, 0, 0);

		groundShape->calculateLocalInertia(mass, localInertia);

		btTransform groundTransform;
		groundTransform.setIdentity();
		groundTransform.setOrigin(btVector3(0, 0, -15));

		btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform); //motionstate provides interpolation capabilities, and only synchronizes 'active' objects
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);
		//body->setRestitution(0.0);
		body->setFriction(0.5);
		//body->setRollingFriction(0.9);
		dynamicsWorld->addRigidBody(body); //add the body to the dynamics world
	}

	{
		btCollisionShape* playerShape = new btBoxShape(btVector3(1, 1, 3));//new btCapsuleShapeZ(1,4);
		//new btSphereShape(4);//
		// btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0,1,0),50);
		//	objects.push_back(playerShape);

		btScalar pmass = PLAYER_MASS; //rigidbody is static if mass is zero, otherwise dynamic
		btVector3 plocalInertia(0, 0, 0);

		playerShape->calculateLocalInertia(pmass, plocalInertia);
		btTransform playerTransform;
		playerTransform.setIdentity();
		playerTransform.setOrigin(btVector3(1000, 1000, 60));

		btDefaultMotionState* pMotionState = new btDefaultMotionState(playerTransform); //motionstate provides interpolation capabilities, and only synchronizes 'active' objects
		btRigidBody::btRigidBodyConstructionInfo prbInfo(pmass, pMotionState, playerShape, plocalInertia);
		mPlayerObject = new btRigidBody(prbInfo);
		mPlayerObject->setAngularFactor(btVector3(0, 0, 0));
		mPlayerObject->setActivationState(DISABLE_DEACTIVATION);
		dynamicsWorld->addRigidBody(mPlayerObject);
	}
	AddVehicle();
	debugDrawer.setDebugMode(btIDebugDraw::DBG_DrawWireframe);
	dynamicsWorld->setDebugDrawer(&debugDrawer);
}

inline float distanceV3(Vector3D a, Vector3D b) {
	Vector3D c;
	c.x = (a.x - b.x);
	c.y = (a.y - b.y);
	c.z = (a.z - b.z);
	return (float)sqrt((c.x *c.x + c.y*c.y + c.z*c.z));
}

float Collision::_max(float a, float b) {
	return a > b ? a : b;
}

float Collision::_min(float a, float b) {
	return a < b ? a : b;
}

void Collision::Explode(Vector3D exPoint, float force) {
	//std::cout << "Explosion" << std::endl;
	for (size_t i = 0; i < objects.size(); i++) {
		btVector3 origin = objects[i]->getWorldTransform().getOrigin();
		Vector3D objPosition = Vector3D(origin.x(), origin.y(), origin.z());
		//std::cout << "objPosition = " << objPosition.x << " " << objPosition.y << " " << objPosition.z << std::endl;
		//std::cout << "exPoint = " << exPoint.x << " " << exPoint.y << " " << exPoint.z << std::endl;
		float dist = distanceV3(exPoint, objPosition);
		objPosition -= exPoint;
		objPosition.Normalize();
		dist = (100.0f - _min(_max(1.0f, dist), 100.0f)) / 100.0f;
		objPosition *= dist * force;
		//	std::cout << "dist = " << dist << std::endl;
		objects[i]->applyCentralImpulse(btVector3(objPosition.x, objPosition.y, objPosition.z));
	}
}

void Collision::AddVehicle() {
	m_vehicle = 0;
	m_wheelShape = 0;
	btCollisionShape* chassisShape = new btBoxShape(btVector3(10.0f, 10.0f, 0.2f));
	//btCompoundShape* compound = new btCompoundShape();
	//	btTransform localTrans;
	//	localTrans.setIdentity();
	//localTrans effectively shifts the center of mass with respect to the chassis
	//localTrans.setOrigin(btVector3(0, 0, 1));

	//compound->addChildShape(localTrans, chassisShape);


	btScalar pmass = 800; //rigidbody is static if mass is zero, otherwise dynamic
	btVector3 plocalInertia(0, 0, 0);

	chassisShape->calculateLocalInertia(pmass, plocalInertia);
	btTransform playerTransform;
	playerTransform.setIdentity();
	playerTransform.setOrigin(btVector3(1200, 1200, 20));

	btDefaultMotionState* pMotionState = new btDefaultMotionState(playerTransform); //motionstate provides interpolation capabilities, and only synchronizes 'active' objects
	btRigidBody::btRigidBodyConstructionInfo prbInfo(pmass, pMotionState, chassisShape, plocalInertia);
	m_carChassis = new btRigidBody(prbInfo);
	m_carChassis->setActivationState(DISABLE_DEACTIVATION);

	dynamicsWorld->addRigidBody(m_carChassis);
	m_wheelShape = new btCylinderShapeX(btVector3(wheelWidth, wheelRadius, wheelRadius));

	/// create vehicle
	{
		m_vehicleRayCaster = new btDefaultVehicleRaycaster(dynamicsWorld);
		m_vehicle = new btRaycastVehicle(m_tuning, m_carChassis, m_vehicleRayCaster);
		///never deactivate the vehicl
		//m_vehicle->
		dynamicsWorld->addVehicle(m_vehicle);
		//dynamicsWorld->addAction(m_vehicle);

		float connectionHeight = 0.0f;


		bool isFrontWheel = true;

		//choose coordinate system
		m_vehicle->setCoordinateSystem(rightIndex, upIndex, forwardIndex);

		btVector3 connectionPointCS0(CUBE_HALF_EXTENTS - 24, 2 * CUBE_HALF_EXTENTS - 24, connectionHeight);

		m_vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, m_tuning, isFrontWheel);

		connectionPointCS0 = btVector3(-CUBE_HALF_EXTENTS + 24, 2 * CUBE_HALF_EXTENTS - 24, connectionHeight);

		m_vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, m_tuning, isFrontWheel);

		connectionPointCS0 = btVector3(-CUBE_HALF_EXTENTS + 24, -2 * CUBE_HALF_EXTENTS + 24, connectionHeight);

		isFrontWheel = false;
		m_vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, m_tuning, isFrontWheel);

		connectionPointCS0 = btVector3(CUBE_HALF_EXTENTS - 24, -2 * CUBE_HALF_EXTENTS + 24, connectionHeight);

		m_vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, m_tuning, isFrontWheel);

		for (int i = 0; i<m_vehicle->getNumWheels(); i++)
		{
			btWheelInfo& wheel = m_vehicle->getWheelInfo(i);
			wheel.m_suspensionStiffness = suspensionStiffness;
			wheel.m_wheelsDampingRelaxation = suspensionDamping;
			wheel.m_wheelsDampingCompression = suspensionCompression;
			wheel.m_frictionSlip = wheelFriction;
			wheel.m_rollInfluence = rollInfluence;
		}
	}
}

int Collision::AddBody(Vector3D size, Vector3D position, Vector3D rotation, btScalar mass, std::vector<Vector3D> vertexList, bool isStatic) {
	//btConvexHullShape *tmp = new btConvexHullShape();
	btCollisionShape* boxShape;
	btVector3 localInertia(0, 0, 0);
	if (isStatic) {
		btTriangleMesh *tmp = new btTriangleMesh();
		for (size_t i = 0; i < vertexList.size(); i += 3) {
			tmp->addTriangle(btVector3(vertexList[i].x * size.x, vertexList[i].y * size.y, vertexList[i].z * size.z),
				btVector3(vertexList[i + 1].x * size.x, vertexList[i + 1].y * size.y, vertexList[i + 1].z * size.z),
				btVector3(vertexList[i + 2].x * size.x, vertexList[i + 2].y * size.y, vertexList[i + 2].z * size.z));
		}

		boxShape = new btBvhTriangleMeshShape(tmp, true);
	}
	else {
		if (vertexList.size() > 400) {
			btTriangleMesh *tmp = new btTriangleMesh();
			for (size_t i = 0; i < vertexList.size(); i += 3) {
				tmp->addTriangle(btVector3(vertexList[i].x * size.x, vertexList[i].y * size.y, vertexList[i].z * size.z),
					btVector3(vertexList[i + 1].x * size.x, vertexList[i + 1].y * size.y, vertexList[i + 1].z * size.z),
					btVector3(vertexList[i + 2].x * size.x, vertexList[i + 2].y * size.y, vertexList[i + 2].z * size.z));
			}

			boxShape = new btConvexTriangleMeshShape(tmp);
		}
		else {
			btConvexHullShape *tmp = new btConvexHullShape();
			for (size_t i = 0; i < vertexList.size(); i += 3) {
				tmp->addPoint(btVector3(vertexList[i].x * size.x, vertexList[i].y * size.y, vertexList[i].z * size.z));
			}
			boxShape = tmp;
		}
		boxShape->calculateLocalInertia(mass, localInertia);
	}

	btTransform startTransform;
	startTransform.setIdentity();
	/*	if (isStatic) {
	glPushMatrix();
	glLoadIdentity();
	glTranslated(position.x, position.y, position.z);
	glRotatef(rotation.x, 1, 0, 0);
	glRotatef(rotation.y, 0, 1, 0);
	glRotatef(rotation.z, 0, 0, 1);
	glScaled(size.x, size.y, size.z);


	GLfloat m[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, m);
	startTransform.setFromOpenGLMatrix(m);
	glPopMatrix();
	}
	else {*/
	startTransform.setOrigin(btVector3(position.x, position.y, position.z));
	btQuaternion rotationTransform;
	//rotationTransform.setRotation(btVector3(1, 0, 0), rotation.x / 180.0 * 3.141592);
	//rotationTransform.setRotation(btVector3(0, 1, 0), rotation.y / 180.0 * 3.141592);
	//	rotationTransform.setRotation(btVector3(0, 0, 1), rotation.z / 180.0 * 3.141592);
	rotationTransform.setEuler(rotation.y / 180.0 * 3.141592, rotation.x / 180.0 * 3.141592, rotation.z / 180.0 * 3.141592);
	startTransform.setRotation(rotationTransform);
	//	}
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, boxShape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);
	objects.push_back(body);
	//body->setRestitution(0.0);
	//mPlayerObject->setAngularFactor(btVector3(0, 0, 1));
	//body->setFriction(0.5);
	body->setActivationState(DISABLE_DEACTIVATION);
	dynamicsWorld->addRigidBody(body);
	return objects.size() - 1;
}

void Collision::AddRigidBox(Vector3D size, Vector3D position, btScalar mass) {
	btCollisionShape* boxShape = new btBoxShape(btVector3(size.x, size.y, size.z));


	//boxShape->
	btVector3 localInertia(0, 0, 0);

	boxShape->calculateLocalInertia(mass, localInertia);

	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(btVector3(position.x, position.y, position.z));

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, boxShape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);
	objects.push_back(body);
	//body->setRestitution(0.0);
	//mPlayerObject->setAngularFactor(btVector3(0, 0, 1));
	body->setFriction(0.5);
	dynamicsWorld->addRigidBody(body);
}

void Collision::ResolveCollision() {
	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(btVector3(Camera::getPosition().x, Camera::getPosition().y, Camera::getPosition().z - 2.0));
	float jump = 0.0;
	float zSpeed = mPlayerObject->getLinearVelocity().z();
	if (Camera::GetSpeed().z > 0.0) {
		jump = 3.0;
	}
	mPlayerObject->setLinearVelocity(btVector3(Camera::GetSpeed().x, Camera::GetSpeed().y, zSpeed + jump));


	int wheelIndex = 2;
	m_vehicle->applyEngineForce(gEngineForce, wheelIndex);
	m_vehicle->setBrake(gBreakingForce, wheelIndex);
	wheelIndex = 3;
	m_vehicle->applyEngineForce(gEngineForce, wheelIndex);
	m_vehicle->setBrake(gBreakingForce, wheelIndex);

	wheelIndex = 0;
	m_vehicle->setSteeringValue(gVehicleSteering, wheelIndex);
	wheelIndex = 1;
	m_vehicle->setSteeringValue(gVehicleSteering, wheelIndex);


	for (int i = 0; i < m_vehicle->getNumWheels(); i++)
	{
		//synchronize the wheels with the (interpolated) chassis worldtransform
		m_vehicle->updateWheelTransform(i, true);
	}


	if (dynamicsWorld)//step the simulation
		dynamicsWorld->stepSimulation(Info::GetElapsedTime());

	//std::cout << gEngineForce << "" << std::endl;
	btVector3 newPosition = mPlayerObject->getWorldTransform().getOrigin();
	Camera::setPosition(Vector3D(newPosition.x(), newPosition.y(), newPosition.z() + 1));
}

void Collision::QuaternionToEulerXYZ(const btQuaternion &quat, btVector3 &euler)
{
	float w = quat.getW();   float x = quat.getX();   float y = quat.getY();   float z = quat.getZ();
	double sqw = w*w; double sqx = x*x; double sqy = y*y; double sqz = z*z;
	euler.setZ((atan2(2.0 * (x*y + z*w), (sqx - sqy - sqz + sqw))));
	euler.setX((atan2(2.0 * (y*z + x*w), (-sqx - sqy + sqz + sqw))));
	euler.setY((asin(-2.0 * (x*z - y*w))));
}

btScalar* Collision::GetObjectMatrix(int i) {
	btScalar *m = new btScalar[16];
	btTransform t;
	btMatrix3x3	rot; rot.setIdentity();
	const int numObjects = dynamicsWorld->getNumCollisionObjects();


	objects[i]->getWorldTransform().getOpenGLMatrix(m);

	return m;
}

void Collision::specialKeyboardUp(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
	{
						gEngineForce = 0.f;
						break;
	}
	case GLUT_KEY_DOWN:
	{
						  gBreakingForce = 0.f;
						  break;
	}
	default:
		break;
	}

}


void Collision::specialKeyboard(int key, int x, int y)
{

	//      printf("key = %i x=%i y=%i\n",key,x,y);

	switch (key)
	{
	case GLUT_KEY_LEFT:
	{
						  gVehicleSteering += steeringIncrement;
						  if (gVehicleSteering > steeringClamp)
							  gVehicleSteering = steeringClamp;

						  break;
	}
	case GLUT_KEY_RIGHT:
	{
						   gVehicleSteering -= steeringIncrement;
						   if (gVehicleSteering < -steeringClamp)
							   gVehicleSteering = -steeringClamp;

						   break;
	}
	case GLUT_KEY_UP:
	{
						gEngineForce = maxEngineForce;
						gBreakingForce = 0.f;
						break;
	}
	case GLUT_KEY_DOWN:
	{
						  gBreakingForce = maxBreakingForce;
						  gEngineForce = 0.f;
						  break;
	}
	default:
		break;
	}

	//      glutPostRedisplay();


}