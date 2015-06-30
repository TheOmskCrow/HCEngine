#include "StdAfx.h"
#include <limits>
#include "hc3dPhysics.h"
#include "GLDebugDraw.h"
#include "Control/Camera.h"
#include "Control/Info.h"
#include <math.h>
#include <iostream>



#define PLAYER_MASS 80
#define CUBE_HALF_EXTENTS 1
#define LOOK_AT_QUEUE_SIZE 3
#define MAX_ENGINE_FORCE 1000000
#define MIN_ENGINE_FORCE 100000

using namespace hc3d;

std::vector<Vector3D> Collision::positionsC;
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
int Collision::bindedObject = -1;
btVector3 Collision::vehicleRotation(0, 0, 3.1415 / 2.0);
bool Collision::vehicleLeft = false;
bool Collision::vehicleRight = false;
bool Collision::vehicleUp = false;
bool Collision::vehicleDown = false;

bool Collision::engineFront = false;
bool Collision::engineBack = false;
bool Collision::engineLeft = false;
bool Collision::engineRight = false;
std::queue<Vector3D> Collision::lookAtS;
std::queue<Vector3D> Collision::rollByS;
std::queue<Vector3D> Collision::positionS;
float Collision::engineForce = 500000.0f;

btVector3 wheelDirectionCS0(0, 0, -1);
btVector3 wheelAxleCS(1, 0, 0);
btScalar suspensionRestLength(0.6);
int Collision::cadr = 0;
clock_t Collision::startTime;
float Collision::dist;
btVector3 Collision::lastPos;

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
	if (terrain == nullptr) 
		return;
	dynamicsWorld->removeRigidBody(terrain->body);
	delete terrain->body;
	delete terrain->myMotionState;
	delete terrain->heightfieldShape;
	delete[] terrain->heightfield;
	delete terrain;
}

TerrainHF * Collision::AddTerrain(int size, float* heightfield, Vector3D offset) {
	TerrainHF *ter = new TerrainHF();
	ter->heightfield = heightfield;

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
		playerTransform.setOrigin(btVector3(-1000, -1000, 310));

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

std::vector<std::vector<float> > Collision::bt3x3ToVector(btMatrix3x3 R) {
	auto row0 = R[0];
	auto row1 = R[1];
	auto row2 = R[2];
	std::vector<float> row_0, row_1, row_2;
	row_0.push_back(row0.x());
	row_0.push_back(row0.y());
	row_0.push_back(row0.z());

	row_1.push_back(row1.x());
	row_1.push_back(row1.y());
	row_1.push_back(row1.z());

	row_2.push_back(row2.x());
	row_2.push_back(row2.y());
	row_2.push_back(row2.z());

	std::vector<std::vector<float> > res = { row_0, row_1, row_2 };

	return res;
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

Vector3D Collision::VectorToEuler(Vector3D direction) {
	float distance = sqrt(direction.x * direction.x + direction.y * direction.y);
	float pitch = atan2(direction.z, distance);
	float yaw = atan2(direction.x, -direction.y) - 3.1415 / 2.0;
	return Vector3D(yaw, pitch, 0);
}

bool Collision::IsBinded() {
	return bindedObject != -1;
}

Vector3D Collision::GetVehiclePosition() {
	if (bindedObject != -1) {
		auto position = objects[bindedObject]->getWorldTransform().getOrigin();
		return Vector3D(position.x(), position.y(), position.z());
	}
	return Vector3D(0, 0, 0);
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

	if (dynamicsWorld)//step the simulation
		dynamicsWorld->stepSimulation(Info::GetElapsedTime());

	/// Process vehicle
	if (bindedObject != -1) {
		objects[bindedObject]->setLinearVelocity(btVector3(0, 0, 0));
		if (engineFront) engineForce += 5000.0f * Info::GetElapsedTime();
		else if (engineBack) engineForce -= 5000.0f * Info::GetElapsedTime();

		if (engineLeft) vehicleRotation.setZ(min(vehicleRotation.z() + 0.005 * Info::GetElapsedTime(), 0.05));
		else if (engineRight) vehicleRotation.setZ(max(vehicleRotation.z() - 0.005 * Info::GetElapsedTime(), -0.05));

		if (vehicleLeft) vehicleRotation.setY(min(vehicleRotation.y() + 0.04 * Info::GetElapsedTime(), 0.2));
		else if (vehicleRight) vehicleRotation.setY(max(vehicleRotation.y() - 0.04 * Info::GetElapsedTime(), -0.2));
		
		if (vehicleUp) vehicleRotation.setX(min(vehicleRotation.x() + 0.02 * Info::GetElapsedTime(), 0.1));
		else if (vehicleDown) vehicleRotation.setX(max(vehicleRotation.x() - 0.02 * Info::GetElapsedTime(), -0.1));

	//	std::cout << vehicleRotation.x() << " " << vehicleRotation.y() << " " << vehicleRotation.z() << std::endl;
		engineForce -= 500.0f * Info::GetElapsedTime();
		engineForce = max(min(engineForce, MAX_ENGINE_FORCE), MIN_ENGINE_FORCE);

		auto velocity = objects[bindedObject]->getAngularVelocity();
		velocity.setX(max(min(velocity.x() / 1.1, 5.0), -5.0));
		velocity.setY(max(min(velocity.y() / 1.1, 5.0), -5.0));
		velocity.setZ(max(min(velocity.z() / 1.1, 5.0), -5.0));
		objects[bindedObject]->setAngularVelocity(velocity);


		btVector3 rotX(1, 0, 0);
		btVector3 rotY(0, 1, 0);
		btVector3 rotZ(0, 0, 1);

		objects[bindedObject]->getWorldTransform().getOrigin();
		btTransform Y(btQuaternion(rotY, vehicleRotation.y()), btVector3(0, 0, 0));
		btTransform Z(btQuaternion(rotZ, vehicleRotation.z()), btVector3(0, 0, 0));
		btTransform X(btQuaternion(rotX, vehicleRotation.x()), btVector3(0, 0, 0));
		Y.mult(objects[bindedObject]->getWorldTransform(), Y);
		Y.mult(Y, Z);
		Y.mult(Y, X);
		objects[bindedObject]->setWorldTransform(Y);

		vehicleRotation.setX(vehicleRotation.x() / 1.3);
		vehicleRotation.setY(vehicleRotation.y() / 1.3);
		vehicleRotation.setZ(vehicleRotation.z() / 1.3);
	}

	//std::cout << gEngineForce << "" << std::endl;
	btVector3 newPosition;
	if (bindedObject == -1) newPosition = mPlayerObject->getWorldTransform().getOrigin();
	else {

		auto orientationMat = objects[bindedObject]->getWorldTransform().getBasis().inverse();
		
		auto row1 = orientationMat[0];
		auto row2 = orientationMat[1];
		auto row3 = orientationMat[2];
		
		//auto Euler = MatrixToEuler(bt3x3ToVector(objects[bindedObject]->getWorldTransform().getBasis().inverse()));
		//QuaternionToEulerXYZ(objects[bindedObject]->getWorldTransform().getRotation(), Euler);

		Vector3D A = Vector3D(0,20,10);
		Vector3D lookAt = Vector3D(0, -10, 5);
		Vector3D goTo = Vector3D(0, -engineForce, 0);
		Vector3D rollBy = Vector3D(0, 0, 1);
		Matrix4x4 matz;
		matz.set(row1.x(), row1.y(), row1.z(), 0,
			row2.x(), row2.y(), row2.z(), 0,
			row3.x(), row3.y(), row3.z(), 0,
			0, 0, 0, 1);
			
		A = matz.mult(A);
		lookAt = matz.mult(lookAt);
		rollBy = matz.mult(rollBy);
		goTo = matz.mult(goTo);
		//A = matx.mult(A);
		//A = matz.mult(A);
		
		newPosition = objects[bindedObject]->getWorldTransform().getOrigin();
		objects[bindedObject]->applyCentralImpulse(btVector3(goTo.x, goTo.y, goTo.z));
		lookAt += Vector3D(newPosition.x(), newPosition.y(), newPosition.z());
		newPosition.setX(newPosition.x() + A.x);
		newPosition.setY(newPosition.y() + A.y);
		newPosition.setZ(newPosition.z() + A.z);

		if (!lookAtS.size()) {
			for (int i = 0; i < LOOK_AT_QUEUE_SIZE; i++) {
				lookAtS.push(lookAt * float(i) / (float)LOOK_AT_QUEUE_SIZE);
				rollByS.push(rollBy * float(i) / (float)LOOK_AT_QUEUE_SIZE);
				positionS.push(Vector3D(newPosition.x(), newPosition.y(), newPosition.z()));
			}
		}
		lookAtS.push(lookAt);
		lookAt = lookAtS.front();
		lookAtS.pop();

		
		rollByS.push(rollBy);
		rollBy = rollByS.front();
		rollByS.pop();

		positionS.push(Vector3D(newPosition.x(), newPosition.y(), newPosition.z()));
		newPosition.setX(positionS.front().x);
		newPosition.setY(positionS.front().y);
		newPosition.setZ(positionS.front().z);
		positionS.pop();

		auto direction = lookAt - Vector3D(newPosition.x(), newPosition.y(), newPosition.z());
		auto Euler = VectorToEuler(direction);
		Camera::SetLookAt(lookAt);
		Camera::SetRollBy(rollBy);
		//std::cout << Euler.x << " " << Euler.y << " " << Euler.z << " " << std::endl;
		Camera::SetEuler(Euler.x, Euler.y, Euler.z);
	}
	Camera::setPosition(Vector3D(newPosition.x(), newPosition.y(), newPosition.z()));
	
}

void Collision::BindCamera(int objectID) {
	bindedObject = objectID;
}

void Collision::QuaternionToEulerXYZ(const btQuaternion &quat, btVector3 &euler)
{
	float w = quat.getW();   float x = quat.getX();   float y = quat.getY();   float z = quat.getZ();
	double sqw = w*w; double sqx = x*x; double sqy = y*y; double sqz = z*z;
	euler.setZ((atan2(2.0 * (x*y + z*w), (sqx - sqy - sqz + sqw))));
	euler.setX((atan2(2.0 * (y*z + x*w), (-sqx - sqy + sqz + sqw))));
	euler.setY((asin(-2.0 * (x*z - y*w))));
}



const float PI = 3.14159265358979323846264f;

bool closeEnough(const float& a, const float& b, const float& epsilon = std::numeric_limits<float>::epsilon()) {
	return (epsilon > std::abs(a - b));
}

Vector3D Collision::MatrixToEuler(const std::vector<std::vector<float> >& R) {

	//check for gimbal lock
	if (closeEnough(R[0][2], -1.0f)) {
		float x = 0; //gimbal lock, value of x doesn't matter
		float y = PI / 2;
		float z = x + atan2(R[1][0], R[2][0]);
		return Vector3D(x, y, z);
	}
	else if (closeEnough(R[0][2], 1.0f)) {
		float x = 0;
		float y = -PI / 2;
		float z = -x + atan2(-R[1][0], -R[2][0]);
		return Vector3D(x, y, z);
	}
	else { //two solutions exist
		float x1 = -asin(R[0][2]);
		float x2 = PI - x1;

		float y1 = atan2(R[1][2] / cos(x1), R[2][2] / cos(x1));
		float y2 = atan2(R[1][2] / cos(x2), R[2][2] / cos(x2));

		float z1 = atan2(R[0][1] / cos(x1), R[0][0] / cos(x1));
		float z2 = atan2(R[0][1] / cos(x2), R[0][0] / cos(x2));

		//choose one solution to return
		//for example the "shortest" rotation
		if ((std::abs(x1) + std::abs(y1) + std::abs(z1)) <= (std::abs(x2) + std::abs(y2) + std::abs(z2))) {
			return Vector3D(x1, y1, z1);
		}
		else return Vector3D(x2, y2, z2);
	}
}

btScalar* Collision::GetObjectMatrix(int i) {
	btScalar *m = new btScalar[16];
	btTransform t;
	btMatrix3x3	rot; rot.setIdentity();
	const int numObjects = dynamicsWorld->getNumCollisionObjects();


	objects[i]->getWorldTransform().getOpenGLMatrix(m);

	return m;
}

void Collision::CRelease() {
	FILE* fileId;
	fopen_s(&fileId, "checkpoints.txt", "w");
	if (!fileId) return;
	for (size_t i = 0; i < positionsC.size(); i++) {
		fprintf(fileId, "%f %f %f\n", positionsC[i].x, positionsC[i].y, positionsC[i].z);
	}
	fclose(fileId);
}

void Collision::Keyboard(int key, int x, int y) {
	if (key == 'q' || key == 'Q') exit(0);
	if (key == 'w' || key == 'W') {
		if (!engineBack) engineFront = true;
	}
	else if (key == 's' || key == 'S') {
		if (!engineFront) engineBack = true;
	}
	else if (key == 'a' || key == 'A') {
		if (!engineRight) engineLeft = true;
	}
	else if (key == 'd' || key == 'D') {
		if (!engineLeft) engineRight = true;
	}
	else if (key == 'r' || key == 'R') {
		if (!Info::IsRunned()) Info::StartRound();
		else Info::EndRound(false);
	}
	else if (key == ' ' || key == 'f' || key == 'F') {
		auto tmp = objects[bindedObject]->getWorldTransform().getOrigin();
		positionsC.push_back(Vector3D(tmp.x(), tmp.y(), tmp.z()));
		CRelease();
	}

}

void Collision::KeyboardUp(int key, int x, int y) {
	if (key == 'w' || key == 'W') {
		engineFront = false;
	}
	else if (key == 's' || key == 'S') {
		engineBack = false;
	}
	else if (key == 'a' || key == 'A') {
		engineLeft = false;
	}
	else if (key == 'd' || key == 'D') {
		engineRight = false;
	}
}

void Collision::specialKeyboardUp(int key, int x, int y)
{	
	switch (key)
	{
	case GLUT_KEY_UP:
	{
		vehicleUp = false;
		break;
	}
	case GLUT_KEY_DOWN:
	{
		vehicleDown = false;
		break;
	}
	case GLUT_KEY_LEFT:
	{
		vehicleLeft = false;
		break;
	}
	case GLUT_KEY_RIGHT:
	{
		vehicleRight = false;
		break;
	}
	default:
		break;
	}

}

btVector3 control(btVector3 in)
{
	// play around with the factor until you find a matching one
	static int Kp = 0.5;
	return in * Kp;
}

void Collision::specialKeyboard(int key, int x, int y)
{

	//      printf("key = %i x=%i y=%i\n",key,x,y);
	if (bindedObject != -1) {
		switch (key)
		{
		case GLUT_KEY_LEFT:
		{
			if (!vehicleRight)
			vehicleLeft = true;
			break;
		}
		case GLUT_KEY_RIGHT:
		{
			if (!vehicleLeft)
			vehicleRight = true;
			break; 
		}
		case GLUT_KEY_UP:
		{
			if (!vehicleDown)
			vehicleUp = true;
			break;
		}
		case GLUT_KEY_DOWN:
		{

			if (!vehicleUp)
			vehicleDown = true;
			break;
		}
		default:
			break;
		}
	}

	//      glutPostRedisplay();


}
