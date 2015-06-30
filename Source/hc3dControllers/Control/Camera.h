#pragma once

#include <hc3dContentLoader.h>
#include <hc3dMath.h>
#include <dllmain.h>

namespace hc3d {
	class HC3D_API Camera
	{
		//private:

	public:
		Camera(void);
		static void init();
		static void set_camera(int, int, int, int);
		static void SetEuler(float yaw, float pitch, float roll);
		static void SetLookAt(Vector3D lookAt);
		static Vector3D GetLookAt();
		static void SetRollBy(Vector3D rollBy);
		~Camera(void);
		static float rnd();
		static Vector3D getPosition();
		static void setPosition(Vector3D);
		static Vector3D GetSpeed();
		static Vector3D getEyePosition();
		static Vector3D GetCamSpeed();
		static float getTP();
		static float GetYaw();
		static float GetPitch();
		static float GetRoll();
		static float GetBoatYaw();
	public:
		static bool FP;
		static float PLAYER_SPEED;
		static float sensivity;
		static float yaw;
		static float pitch;
		static float roll;
		static float frames;
		static Vector3D lookAt;
		static Vector3D rollBy;
		static bool goa;
		static bool gow;
		static bool gos;
		static bool god;
		static float shake;
		static Vector3D cam_speed;
		static Vector3D direction;
		static bool boat_mode;
		static bool camera_inited;
		static float boat_yaw;
		static float boat_pitch;
		static float boat_roll;
		static float speed;
		static float boat_yaw_s;
		static float boat_pitch_s;
		static float boat_roll_s;
	private:
		static Vector3D mousex;
		static Vector3D mousey;
		static Vector3D position;
		static Vector3D eyePosition;
		static float TP_ammount;
	};

}