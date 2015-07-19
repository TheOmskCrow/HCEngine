#pragma once
#include <stdexcept>
#include <hc3dContentLoader.h>
#include <hc3dControllers.h>
#include <hc3dPhysics.h>
#include <hc3dSceneBuilder.h>
#include <hc3dMath.h>
#include <vector>

namespace hc3d {
	class HC3D_API Object
	{
	public:
		Object() {}

		virtual void Init() {
			std::runtime_error(std::string("Method Init() not overloaded \n"));
		}

		virtual void Draw() {
			std::runtime_error(std::string("Method Draw() not overloaded \n"));
		}
	};

	class HC3D_API Vehicle : public Object
	{
	public:
		Vehicle() {}

	};

	class HC3D_API Objects {

	private:
		std::vector<Object*> list;
	public:
		Objects() {}

		void push_back(Object* obj) {
			list.push_back(obj);
		}
		void pop_back() {
			list.pop_back();
		}
		size_t size() {
			return list.size();
		}

		Object* operator[](int i) {
			return list[i];
		}
		void Init() {
			Info::init();
			Camera::init();
			Shadow::init();
			for (size_t i = 0; i < list.size(); i++) {
				list[i]->Init();
			}
		}
		/// Отрисовать все объекты
		/// Первый параметр определяет наличие теней на сцене, второй - наличие преломлений / отражений водной поверхности
		void MakeShadows(int num, float perspective_size, Vector3D lightPosition) {

			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			GLfloat* mv = new GLfloat[16];
			GLfloat* pr = new GLfloat[16];
			glColorMask(false, false, false, false);
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(4, 4);

			glClear(GL_DEPTH_BUFFER_BIT);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();

			float widthHeightRatio = (float)Info::width() / (float)Info::height();
			gluPerspective(perspective_size, widthHeightRatio, 200.0, 12000.0);//TODO: dynamic values

			Vector3D c_pos = Camera::getPosition();
			gluLookAt(lightPosition.x, lightPosition.y, lightPosition.z,		// eye
				c_pos.x, c_pos.y, c_pos.z,	// center
				0, 0, 1);						// up
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glGetFloatv(GL_MODELVIEW_MATRIX, mv);
			glGetFloatv(GL_PROJECTION_MATRIX, pr);


			glEnable(GL_TEXTURE_2D);
			Info::SetShader(false);
			if (num < 4) {
				for (size_t i = 0; i < list.size(); i++) {
					list[i]->Draw();
				}
				SceneBuilder::Draw();
				//draw here
			}
			Shadow::shadow_shot(num);
			Info::SetShader(true);

			glDisable(GL_POLYGON_OFFSET_FILL);
			glColorMask(true, true, true, true);

			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();

			glActiveTexture(GL_TEXTURE0 + num);								// set correct texcoord transform
			glMatrixMode(GL_TEXTURE);
			glPushMatrix();

			glLoadIdentity();

			glMultMatrixf(pr);
			glMultMatrixf(mv);
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
		}

		void CreateShadows() {
			Vector3D lightPosition = Camera::getPosition() + Info::GetSun() * 3.0;
			MakeShadows(1, 0.5, lightPosition);
			if (Info::GetShadowDist() > 150.0) MakeShadows(2, 3, lightPosition);
			if (Info::GetShadowDist() > 250.0) MakeShadows(3, 8, lightPosition);
		}

		void Draw() {
			for (size_t i = 0; i < list.size(); i++) {
				list[i]->Draw();
			}
		}
	};

}                                                               