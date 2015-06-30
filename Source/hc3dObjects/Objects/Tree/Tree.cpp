#include "StdAfx.h"
#include "Tree.h"
#include <fstream>
#include <iostream>
#include <string>
#include "hc3dObjects.h"
#include "hc3dMath.h"
#include "../Water/Water.h"

using namespace hc3d;
using namespace std;

Tree::Tree(void)
{
}


Tree::~Tree(void)
{
}

float Tree::distance(Vector3D a, Vector3D b) {
    Vector3D c;
    c.x = (a.x  - b.x );
    c.y = (a.y - b.y);
    c.z = (a.z - b.z);
    return (float) sqrt((c.x *c.x  + c.y*c.y + c.z*c.z));
}
Vector3D* Tree::qSort(Vector3D *A, int low, int high) {
          int i = low;
          int j = high;
          float x = distance(A[(low+high)/2],pos);
          do {
              while(distance(A[i],pos) > x) ++i;
              while(distance(A[j],pos) < x) --j;
              if(i <= j){
				  swap(gr_size[i],gr_size[j]);
				  swap(A[i],A[j]);
				  swap(angle[i],angle[j]);
				  swap(cadr[i],cadr[j]);
                  i++; j--;
              }
          } while(i < j);
          if(low < j) A = qSort(A, low, j);
          if(i < high) A = qSort(A, i, high);
          return A;
}

void Tree::Init(){
		model = new Model[5];

		std::vector<std::string> treeNames;
		std::vector<float> treeDist;
		treeDist.push_back(550);
		treeNames.push_back("Models/tree2/tree3.obj");
		model[0].init("Models/tree2/tree1.obj","null","null","null");
		model[0].AddLodModels(treeNames, treeDist);
		model[0].AddHitBoxModel("Models/tree2/tree1collision.obj");
		treeNames.clear();

		treeNames.push_back("Models/tree2/tree4.obj");
		model[1].init("Models/tree2/tree2.obj","null","null","null");
		model[1].AddLodModels(treeNames, treeDist);
		model[1].AddHitBoxModel("Models/tree2/tree2collision.obj");
		treeNames.clear();

		treeNames.push_back("Models/tree2/tree free1.obj");
		model[2].init("Models/tree2/tree free.obj", "null", "null", "null");
		model[2].AddLodModels(treeNames, treeDist);
		model[2].AddHitBoxModel("Models/tree2/tree freecollision.obj");
		treeNames.clear();

		model[3].init("Models/tree2/leaves1.obj","null","null","null");
		model[4].init("Models/tree2/leaves2.obj","null","null","null");

		std::vector<std::string> leafNames;
		std::vector<float> leafDist;
		leafNames.push_back("Models/tree2/leaves3.obj");
		leafDist.push_back(400);
		model[3].AddLodModels(leafNames, leafDist);
		
        Tree_num = 30000;
        offset = 0.0;
        tree = new Vector3D[Tree_num];
		angle = new float[Tree_num];
        cadr = new float[Tree_num];
		gr_size = new float[Tree_num];
		gr_width = new float[Tree_num];
		type = new float[Tree_num];
        Tree_init();
		glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
		Info::RaiseCurTextureNum();
		tex = LoadTexture("Texture/trunk.png");
		glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
		Info::RaiseCurTextureNum();
		tex1 = LoadTexture("Texture/tree.png");
		glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
		Info::RaiseCurTextureNum();
		tex2 = LoadTexture("Texture/tree_normal.png");
		glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
		Info::RaiseCurTextureNum();
		tex3 = LoadTexture("Texture/tree1.png");
		shaderprogram = LoadShader("Shaders/model.vert","Shaders/model.frag");
		shaderprogram1 = LoadShader("Shaders/grass.vert","Shaders/grass.frag");
}
float Tree::rnd() {
    return float(rand()%100000)/100000.0;
}
void Tree::Tree_init(){
		//Terrain ter;
        for(int i = 0; i < Tree_num; i++) {
			while(1) {
            Vector3D data;
			data.x = float(rand()) * float(rand() % 10 + 1);
			data.y = float(rand()) * float(rand() % 10 + 1);
			data.z = CalcTerHeight(Vector3D(data.x,data.y,0.0));
			float Tree_coeff = hc3d::Math::hc3dMax(0.0, hc3d::Math::hc3dMin(1.0, (CalcTerNormal(Vector3D(data.x, data.y, 0.0))*Vector3D(0.0, 0.0, 1.0))));
			Tree_coeff -= 0.975;
			Tree_coeff *= 40.0;
			data.z -=4.0;
            tree[i] = data;
			data.z += 4.0;
			angle[i] = rnd()*360.0;
			gr_size[i] = rnd() + 0.25;
			gr_width[i] = rnd()/0.5 + 0.5;
			type[i] = rand()%3;
			if(type[i] == 1) {
				gr_size[i] *= 1.6;
			}
			if (data.z < 100.0 && data.z > 3.0/* && (distance(Vector3D(3000, 2000, 0), tree[i]) < 500 ||
				distance(Vector3D(1600, 2500, 0), tree[i]) < 600
				|| distance(Vector3D(5000, 2000, 0), tree[i]) < 1000
				|| distance(Vector3D(3000, 6000, 0), tree[i]) < 500
				|| distance(Vector3D(6000, 12000, 0), tree[i]) < 1000
				|| distance(Vector3D(8000, 10000, 0), tree[i]) < 1000
				|| distance(Vector3D(4000, 9000, 0), tree[i]) < 1000
				|| distance(Vector3D(7000, 6000, 0), tree[i]) < 1000
				|| distance(Vector3D(10000, 1000, 0), tree[i]) < 1000
				|| distance(Vector3D(12000, 8000, 0), tree[i]) < 1000
				|| distance(Vector3D(6000, 4000, 0), tree[i]) < 1000
				|| distance(Vector3D(3000, 7000, 0), tree[i]) < 1000
				|| distance(Vector3D(9000, 13000, 0), tree[i]) < 1000)*/) {
				bool flag = true;
				for(int j = 0; j < i; j++) {
					if(distance(tree[j],tree[i]) < 1.0) flag = false;
				}
			/*	if (flag) {
					if (type[i] == 2) model[2].addCollision(tree[i], Vector3D(0, 0, angle[i]), Vector3D(gr_size[i], gr_size[i], gr_size[i]), true);
					else if (type[i] == 0) model[0].addCollision(tree[i], Vector3D(0, 0, angle[i]), Vector3D(gr_size[i], gr_size[i], gr_size[i]), true);
					else if (type[i] == 1) model[1].addCollision(tree[i], Vector3D(0, 0, angle[i]), Vector3D(gr_size[i], gr_size[i], gr_size[i]), true);
					
					
				}*/
				break;
			}
			}
        }
    }
void Tree::setShader(int shad) {
			glUniform1i(glGetUniformLocation(
	        		shad, "type"), 0);
			glUniform1i(glGetUniformLocation(
				shad, "shadowing"), Info::GetShadow());
			glUniform1f(glGetUniformLocation(
				shad, "offset"), offset);
			glUniform1f(glGetUniformLocation(
				shad, "dstbl"), 0.0);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, Shadow::GetShadowMap()[0]);
	        glUniform1i(glGetUniformLocation(
	        		shad, "_shadow0"), 1);

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, Shadow::GetShadowMap()[1]);
	        glUniform1i(glGetUniformLocation(
	        		shad, "_shadow1"), 2);

			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, Shadow::GetShadowMap()[2]);
	        glUniform1i(glGetUniformLocation(
	        		shad, "_shadow2"), 3);

			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, Shadow::GetShadowMap()[3]);
	        glUniform1i(glGetUniformLocation(
	        		shad, "_shadow3"), 4);
			glUniform3f(glGetUniformLocation(
				shaderprogram, "atmosphere"), Info::GetAtmoColor().x, Info::GetAtmoColor().y, Info::GetAtmoColor().z);
			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_2D, Shadow::GetShadowMap()[4]);
	        glUniform1i(glGetUniformLocation(
	        		shad, "_shadow4"), 5);
			glActiveTexture(GL_TEXTURE6);
			glBindTexture(GL_TEXTURE_2D, Shadow::GetShadowMap()[5]);
	        glUniform1i(glGetUniformLocation(
	        		shad, "_shadow5"), 6);

			glActiveTexture(GL_TEXTURE7);
			glBindTexture(GL_TEXTURE_2D, Shadow::GetShadowMap()[6]);
	        glUniform1i(glGetUniformLocation(
	        		shad, "_shadow6"), 7);
			glUniform1f(glGetUniformLocation(
				shad, "shadowDist"), Info::GetShadowDist());
			glUniform1f(glGetUniformLocation(
				shad, "Width"), Info::width());
			glUniform1f(glGetUniformLocation(
				shad, "max_dist"), 500.0);
			glUniform1f(glGetUniformLocation(
				shad, "Height"), Info::height());
			if(shad == shaderprogram1) {
				glActiveTexture(GL_TEXTURE15);
				glBindTexture(GL_TEXTURE_2D, tex1);
				glUniform1i(glGetUniformLocation(
					shad, "model"), 15);
			}
			else {
				glActiveTexture(GL_TEXTURE15);
				glBindTexture(GL_TEXTURE_2D, tex2);
				glUniform1i(glGetUniformLocation(
					shad, "normalMap"), 15);
			//	Info::RaiseCurBindTexture();
				glActiveTexture(GL_TEXTURE16);
				glBindTexture(GL_TEXTURE_2D, tex2);
				glUniform1i(glGetUniformLocation(
					shaderprogram, "groundMap"), 16);
			//	Info::RaiseCurBindTexture();
			}
			Vector3D sun = 0.5*(Info::GetSun());
			glUniform3f(glGetUniformLocation(
	        		shad, "lightPos"), sun.x, sun.y, sun.z);
			Camera camera;
			Vector3D player = Camera::getPosition();
			glUniform3f(glGetUniformLocation(
	        		shad, "eyePos"), player.x, player.y, player.z);
			glUniform1i(glGetUniformLocation(
	        		shad, "flash"), Info::GetFlash());
			glUniform3f(glGetUniformLocation(
				shad, "eyeNorm"), Info::GetEyeNormal().x, Info::GetEyeNormal().y, Info::GetEyeNormal().z);
}
void Tree::Draw() {
	float yaw = 0;
	float pitch = 0;
	if (!Info::GetShader() || Info::GetRefract() || Info::GetReflect()) yaw = 1;
	pos = Camera::getPosition();
	// tree = qSort(tree,0,Tree_num-1);
	if (Info::GetShader()) {
		glUseProgram(shaderprogram);
		setShader(shaderprogram);
	}
	bool tmp = Info::GetShader();
	Info::SetShader(false);
	if (yaw == 0 && !Info::GetRefract() && !Info::GetReflect()) offset += (rnd() / 5.0 + 0.05)*Info::GetElapsedTime();
	float pi = 3.141592;
	Vector3D player = Camera::getPosition();
	int samples = 0;

	for (int i = 0; i < Tree_num; i++) {
		float dst = distance(tree[i], player);
		if (yaw == 0.0 && dst > 2500.0) continue;
		else if (yaw != 0.0 && dst > Info::GetShadowDist()) continue;
		Vector3D a = tree[i] - Camera::getPosition();
		a.Normalize();
		Vector3D b = Info::GetEyeNormal();
		b.Normalize();
		Vector3D _a = a;
		Vector3D _b = b;
		_a.Normalize();
		_b.Normalize();
		float norm_vec = _a*_b;
		if (yaw == 0.0) if (norm_vec < 0.5 && dst > 150.0) continue;


		float size = 1.5;
		float offset = 0;
		if (size >= 1) offset = 0.5;

		glPushMatrix();
		glTranslated(tree[i].x, tree[i].y, tree[i].z);
		glRotatef(angle[i], 0, 0, 1);
		glScaled(gr_size[i], gr_size[i], gr_size[i]);
		glScaled(gr_width[i], gr_width[i], 1.0);
		if (type[i] == 2) {
			glActiveTexture(GL_TEXTURE17);
			glBindTexture(GL_TEXTURE_2D, tex3);
			glUniform1i(glGetUniformLocation(
				shaderprogram, "model"), 17);
			model[2].draw(dst);
		}
		else {
			glActiveTexture(GL_TEXTURE17);
			glBindTexture(GL_TEXTURE_2D, tex);
			glUniform1i(glGetUniformLocation(
				shaderprogram, "model"), 17);

			if (type[i] == 0) model[0].draw(dst);
			else if (type[i] == 1) model[1].draw(dst);
		}
		glPopMatrix();
	}
	Info::SetShader(tmp);
	if (Info::GetShader()) glUseProgram(0);



	//if(Info::shader) {
	glUseProgram(shaderprogram1);
	setShader(shaderprogram1);
	//}
	tmp = Info::GetShader();
	Info::SetShader(false);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//     if(Info::shader)glDepthMask(false);

	for (int i = 0; i < Tree_num; i++) {
		float dst = distance(tree[i], player);
		if (yaw == 0.0 && dst > 2500.0) continue;
		else if (yaw != 0.0 && dst > Info::GetShadowDist()) continue;
		Vector3D a = tree[i] - Camera::getPosition();
		a.Normalize();
		Vector3D b = Info::GetEyeNormal();
		b.Normalize();
		Vector3D _a = a;
		Vector3D _b = b;
		_a.Normalize();
		_b.Normalize();
		float norm_vec = _a*_b;
		if (yaw == 0.0) if (norm_vec < 0.6 && dst > 150.0) continue;


		float size = 1.5;
		float offset = 0;
		if (size >= 1) offset = 0.5;

		glPushMatrix();
		glTranslated(tree[i].x, tree[i].y, tree[i].z);
		glRotatef(angle[i], 0, 0, 1);
		glScaled(gr_size[i], gr_size[i], gr_size[i]);
		glScaled(gr_width[i], gr_width[i], 1.0);
		if (type[i] == 2) model[4].draw(dst);
		else model[3].draw(dst);
		glPopMatrix();
	}
	//	printf("Trees tria drawing: %d\n",samples);
	//		if(Info::shader)   glDepthMask(true);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	Info::SetShader(tmp);
	glUseProgram(0);

}