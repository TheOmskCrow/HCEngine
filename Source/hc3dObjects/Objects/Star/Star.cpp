#include "StdAfx.h"
#include "Star.h"
#include "../Terrain/Terrain.h"
#include "../Atmosphere/Atmosphere.h"

#define pi 3.1415
#define pi180 0.01745277777777777777777777777778
#define pi2 1.5775

using namespace hc3d;

Star::Star(void)
{
	list_id = 0;
}


Star::~Star(void)
{
}
Vector3D generate_angle() {
    Vector3D result;
	result.x = (rand()*pi/180.0);
    result.y = (rand()*pi/180.0);
    result.z = (rand()*pi/180.0);
    //result.x = (result.x + 60*pi/180.0);
    return result;
}

double generate_size() {
    float result;
    float gauss = pow((rand()%100)/100.0,4.0);
    if(gauss < 0.1) gauss = sqrt(sqrt(gauss));
    result = rand();
    result = max(5000.0,result*gauss*30.0);
    return result;
}
void Star::rebuildMatrix() {
	Matrix4x4 matx;

	matx.set(1, 0, 0, 0,
		0, cos(latitude), -sin(latitude), 0,
		0, sin(latitude), cos(latitude), 0,
		0, 0, 0, 1);
	Matrix4x4 maty;
	maty.set(cos(longitude), 0, sin(longitude), 0,
		0, 1, 0, 0,
		-sin(longitude), 0, cos(longitude), 0,
		0, 0, 0, 1);

	matx = matx.mult(maty);

	for (int i = 0; i < num; i++) {
		matrcies[i][0] = matx.mult(pos[i][0]);
		matrcies[i][1] = matx.mult(pos[i][1]);
		matrcies[i][2] = matx.mult(pos[i][2]);
		matrcies[i][3] = matx.mult(pos[i][3]);
	}
}

void Star::Init() {
	num = 20000;
	list_id = 0;
	latitude = Info::GetLatitude();
	longitude = Info::GetLongitude();
	matrcies = new Vector3D*[num];
	for (int i = 0; i < num; i++) {
		matrcies[i] = new Vector3D[4];
	}

	yaw = new float[num];
	pitch = new float[num];
	roll = new float[num];
	size = new float[num];
	pos = new Vector3D*[num];
	for(int i = 0; i < num; i++) {
		pos[i] = new Vector3D[4];
	}


	texture = new GLuint[1];
	texture[0] = LoadTexture("Texture/star.png");
	shaderprogram = LoadShader("Shaders/star.vert","Shaders/star.frag");
    for(int i = 0; i < num; i++) {
        size[i] = generate_size();
        Vector3D A = Vector3D(-100000000.0,0.0,0.0);
        Vector3D B = Vector3D(-100000000.0,size[i],0.0);
        Vector3D C = Vector3D(-100000000.0,size[i],size[i]);
        Vector3D D = Vector3D(-100000000.0,0.0,size[i]);
        Vector3D angle = generate_angle();
        yaw[i] = angle.x;
        pitch[i] = angle.y;
        roll[i] = angle.z;
        Matrix4x4 matx;
		matx.set(1,0,0,0,
                 0,cos(yaw[i]),-sin(yaw[i]),0,
                 0,sin(yaw[i]),cos(yaw[i]),0,
                 0,0,0,1);
        Matrix4x4 maty;
		maty.set(cos(pitch[i]),0,sin(pitch[i]),0,
                 0,1,0,0,
                 -sin(pitch[i]),0,cos(pitch[i]),0,
                 0,0,0,1);
        Matrix4x4 matz;
		matz.set(cos(roll[i]),-sin(roll[i]),0,0,
                 sin(roll[i]),cos(roll[i]),0,0,
                 0,0,1,0,
                 0,0,0,1);

		matx = matx.mult(maty);
		matx = matx.mult(matz);
		A = matx.mult(A);
		B = matx.mult(B);
		C = matx.mult(C);
		D = matx.mult(D);
        pos[i][0] = A;
        pos[i][1] = B;
        pos[i][2] = C;
        pos[i][3] = D;
    }
}
void Star::setShader() {
			Vector3D sun = Info::GetSun();
			float tt = Info::getTime()/60000.0;
	        glActiveTexture(GL_TEXTURE0 + Info::GetCurBindTexture());
		    glBindTexture(GL_TEXTURE_2D,texture[0]);
	        glUniform1i(glGetUniformLocation(
	        		shaderprogram, "star"), Info::GetCurBindTexture());
			Info::RaiseCurBindTexture();

			glUniform1f(glGetUniformLocation(
	        		shaderprogram, "sun"), sun.z);
			glUniform1f(glGetUniformLocation(
	        		shaderprogram, "time"), tt);
			Vector3D position = Camera::getPosition();
			if (Info::GetReflect()) position.z *= -1.0;
	        glUniform1f(glGetUniformLocation(
	        		shaderprogram, "poz"), position.z);
	        glUniform3f(glGetUniformLocation(
	        		shaderprogram, "pozit"), position.x
	        		, position.y, position.z);
	        glUniform3f(glGetUniformLocation(
	        		shaderprogram, "sunz"), sun.x, sun.y, sun.z);
			glUniform3f(glGetUniformLocation(
	        		shaderprogram, "sunz2"), sun.x, sun.y, sun.z);
			glUniform3f(glGetUniformLocation(
				shaderprogram, "light"), sun.x,sun.y,sun.z);
			int refl = 0;
			if (Info::GetReflect()) refl = 1;
			glUniform1i(glGetUniformLocation(
				shaderprogram, "reflection"), refl);			
}
void Star::Draw() {
	Vector3D temp[4];
	static bool enter = false;
	//if (list_id = 0) 
	if (!enter)
	if (latitude != Info::GetLatitude() || longitude != Info::GetLongitude()) {
		latitude = Info::GetLatitude();
		longitude = Info::GetLongitude();
		rebuildMatrix();
		glDeleteLists(list_id, 1);
		list_id = 0;
		enter = true;
	}
	
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glUseProgram(shaderprogram);
	setShader();
	glPushMatrix();
	Vector3D player = Camera::getPosition();
	glTranslated(player.x, player.y, player.z);
	glRotatef((90 - (Info::GetLatitude()) / pi * 180), 1, 0, 0);
	glRotatef((Info::GetLongitude()) / pi * 180, 0, 0, 1);

	glScaled(1.0 * Info::GetGlobalScale(), 1.0 * Info::GetGlobalScale(), 1.0 * Info::GetGlobalScale());
	Vector3D sun = Info::GetSun();
	//float suner = sun.z + 500;
	//suner = fabs(suner) / 1000.0;
	

	//suner = (1.0 - min(suner, 1.0))*90.0 + 10.0;
	
	if(list_id != 0){
		glCallList(list_id);
	}
	else {
		list_id = glGenLists(1);
		glNewList(list_id, GL_COMPILE);

		glBegin(GL_QUADS);


		const float *size0 = &size[0];
		const float *roll0 = &roll[0];

		for (int i = 0; i < num; i++) {

			const float sizeI = *(size0 + i);
			const float rollI = *(roll0 + i) * pi2;

			temp[0] = matrcies[i][0];
			temp[1] = matrcies[i][1];
			temp[2] = matrcies[i][2];
			temp[3] = matrcies[i][3];

			const float ang = rand()* pi180;
			glNormal3f(ang, sizeI, rollI);
			glTexCoord2d(0, 0);
			glVertex3f(temp[0].x, temp[0].y, temp[0].z);
			glNormal3f(ang, sizeI, rollI);
			glTexCoord2d(1, 0);
			glVertex3f(temp[1].x, temp[1].y, temp[1].z);
			glNormal3f(ang, sizeI, rollI);
			glTexCoord2d(1, 1);
			glVertex3f(temp[2].x, temp[2].y, temp[2].z);
			glNormal3f(ang, sizeI, rollI);
			glTexCoord2d(0, 1);
			glVertex3f(temp[3].x, temp[3].y, temp[3].z);

		}
		glEnd();
		glEndList();
	}
	glPopMatrix();
	glUseProgram(0);
	glDisable(GL_CULL_FACE);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
}