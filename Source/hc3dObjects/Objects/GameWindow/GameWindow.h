#pragma once
#include <Object.h>

namespace hc3d {

	class GameWindow : public Object
	{
	public:
		GameWindow(void);
		~GameWindow(void);
		void Draw();
		void Init();
		void init_text(char*);
		void render_text();
		float radius;
	private:
		int num;
		float flash;
		float currentFocus;
		Vector3D center, sun, sun2, pos;
		int text_size;
		int frames;
		GLfloat start_coord[2], end_coord[2];
		int text[100];
		GLfloat screen_coord[200];
		GLfloat font_coord[200];

		void calc_font_coord(int, int);
		void setShader(float, float);
		void setTex(float, float);
		GLuint shaderprogram, shaderprogram2, shaderprogram3, shaderprogram4, shaderprogram5, shaderprogram6, shaderprogram7, shaderprogram8, shaderprogram9, shaderprogram10;
		GLuint cloud1, cloud2, depth1, depth2, screen_noise, font, g_blur, D_O_F, D_O_F_prev, l_depth, normal, SSAOnormal, SSAOposition, abberation;
	};
}
