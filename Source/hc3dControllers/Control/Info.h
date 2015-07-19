#pragma once
#include <hc3dContentLoader.h>
#include <hc3dMath.h>
#include <vector>
#include <time.h>
#include <dllmain.h>

#pragma warning(disable : 4251)

namespace hc3d {
	extern "C" {
		struct HC3D_API ConsoleWord {
			std::string word;
			float y;
			Vector3D color;
		};

		typedef std::vector<ConsoleWord> ConsoleLine;
		typedef std::vector<ConsoleLine> ConsoleText;
	}
	/// Класс, обеспечивающий передачу данных между модулями и расчитывающий часто используемую информацию;
	class HC3D_API Info
	{
	public:
		static double getTime();
		static void SetSun(Vector3D sun);
		static void atmosphere_calc();
		Info(void);
		static void run(int, int);
		static void init();
		static void console();
		static void draw_string(std::string str, int offset, Vector3D color);
		static void run_script(ConsoleLine input);
		static void inputChar(unsigned char key);
		static float dist(Vector3D start, Vector3D end);
		static int width();
		static int height();
		static void AddOutput(std::string outputStr, Vector3D color);
		~Info(void);
		///
		static float GetGlobalScale();
		static float GetEarthZ();
		static Vector3D GetSun();
		static int GetShadow();
		static Vector3D GetAtmoColor();
		static int GetFlash();
		static Vector3D GetEyeNormal();
		static bool GetShader(); static void SetShader(bool);
		static int GetCurTextureNum(); static void RaiseCurTextureNum();
		static float GetElapsedTime();
		static bool GetSSAO();
		static bool GetAA();
		static bool GetGodRays();
		static bool GetBloom();
		static bool GetFilmGrain();
		static bool GetDOF();
		static bool GetToneMapping();
		static bool GetConsoleRun();
		static int GetMaxWater();
		static float GetShadowDist();
		static float GetLatitude(); static void SetLatitude(float);
		static float GetLongitude(); static void SetLongitude(float);
		static GLuint GetModelShader(); static void SetModelShader(GLuint);
		static bool GetModelShaderInited(); static void SetModelShaderInited(bool);
		static GLuint GetCurBindTexture(); static void RaiseCurBindTexture();
		static bool GetRefract(); static void SetRefract(bool refract);
		static bool GetReflect(); static void SetReflect(bool reflect);
		static void StartRound(); static void EndRound(bool success = true);
		static bool IsRunned();
		static void AddPlayerScore(int); static void SubPlayerScore(int);
		static void AddMessage(std::string message, Vector3D color, float time = 30.0);
		static void DropMessages();
		static int GetPlaneState();
		static void SetVehSpeed(float);
		static Vector3D GetForestLocation();
		static float GetCoeff();
		static float GetCoeff1();
	public:
		static float vehSpeed;
		static GLuint curBindTexture;
		static float latitude;
		static Vector3D forestLocation;
		static float longitude;
		static float shadowDist;
		static int planeState;
		static int jetpack;
		static int LoD;
		static bool SSAO;
		static bool AA;
		static bool godRays;
		static bool bloom;
		static bool filmGrain;
		static bool depthOfField;
		static bool toneMapping;
		static bool console_run;
		static bool sceneBuild;
		static float planet_z;
		static int cur_loading_texture;
		static GLuint shaderprogram;
		static GLuint modelShader;
		static bool modelShaderInited;
		static float global_scale;
		static float grass_time;
		static float ter_time;
		static float tree_time;
		static float water_time;
		static float atmo_time;
		static float window_time;
		static float models_time;
		static float cloud_time;
		static Vector3D sun;
		static Vector3D gun1;
		static Vector3D gun1_rotation;
		static Vector3D yaw_pitch;
		static bool jet_gas;
		static bool jet_stop;
		static bool collision;
		static clock_t prev_time;
		static float gravity;
		static clock_t real_time;
		static clock_t spawn_time;
		static clock_t time_frame;
		static float elapsed_time;
		static int max_water;
		static int min_quad;
		static Vector3D eye_normal;
		static bool shader;
		static int flash;
		static int shadow;
		static float num_of_lightning;
		static float wait;
		static Vector3D atmo_color;
		static ConsoleWord cur_word;
		static ConsoleLine cur_line;
		static ConsoleText console_output;
		static float coeff;
		static float coeff1;
	private:
		static bool atmosphereRun;
		static void render_fps(int, int, int, int);
		static void DrawMessage(int, int);
		static void calc_fps();
		static char* init_fps_string(char*);
		static char* GetGameString(char *);
	private:
		static std::vector<std::string> messages;
		static std::vector<Vector3D> messageColors;
		static std::vector<float> messageLife;
		static bool runned;
		static bool finished;
		static clock_t roundTime;
		static clock_t startRoundTime;
		static int cur_time;
		static int start_time;
		static int frames;
		static int _width;
		static int _height;
		static float fps;
		static double time;
		static bool refract;
		static bool reflect;
	};

}