#include "StdAfx.h"
#include "Clouds.h"
#include <fstream>
#include <iostream>
#include <string>

using namespace std;
using namespace hc3d;

#define pi 3.1415
Clouds::Clouds(void)
{
	layerHeight = 1000.0f;
}


Clouds::~Clouds(void)
{
}
float Clouds::dist(Vector3D a, Vector3D b) {
	Vector3D c = a - b;
	return sqrt(c.x*c.x + c.y*c.y);
}

void ConvertToMdl(const char* input) {
	bool vn = false, vt = false, v = false, f = false;
	int vn_num = 0, vt_num = 0, v_num = 0, f_num = 0;
	string vn_buf, vt_buf, v_buf, f_buf;
	const char* name = input;
	ifstream file(name);
	char buffer, old, very_old;
	if (!file.is_open()) {
		cout << "no such file\n";
		return;
	}
	file >> std::noskipws;
	file >> old;
	ofstream file2("Models/Clouds.mdl");
	while (1) {
		file >> buffer;
		if (file.eof()) break;
		if (old == 'v' && buffer == 'n' && very_old == '\n')
			vn = true;
		if (old == 'v' && buffer == 't' && very_old == '\n')
			vt = true;
		if (old == 'v' && buffer == ' ' && very_old == '\n')
			v = true;
		if (old == 'f' && buffer == ' ' && very_old == '\n')
			f = true;
		if (vn)
		if ((buffer <= '9' && buffer >= '0') || buffer == '\n' || buffer == '\t' || buffer == ' ' || buffer == '-' || buffer == '.') {
			vn_buf += buffer;
		}
		if (vt)
		if ((buffer <= '9' && buffer >= '0') || buffer == '\n' || buffer == '\t' || buffer == ' ' || buffer == '-' || buffer == '.') {
			vt_buf += buffer;

		}
		if (v)
		if ((buffer <= '9' && buffer >= '0') || buffer == '\n' || buffer == '\t' || buffer == ' ' || buffer == '-' || buffer == '.') {
			v_buf += buffer;

		}
		if (f) {
			if (buffer == '/') buffer = ' ';
			if ((buffer <= '9' && buffer >= '0') || buffer == '\n' || buffer == '\t' || buffer == ' ' || buffer == '-' || buffer == '.') {
				f_buf += buffer;
			}
		}
		if (buffer == '\n') {
			if (vn) {
				vn = false;
				vn_num++;
			}
			if (v) {
				v = false;
				v_num++;
			}
			if (vt) {
				vt = false;
				vt_num++;
			}
			if (f) {
				f = false;
				f_num++;
			}
		}
		very_old = old;
		old = buffer;


	}
	file2 << v_num << '\n' << v_buf << vn_num << '\n' << vn_buf << vt_num << '\n' << vt_buf << f_num << '\n' << f_buf;
	return;
}

void Clouds::Init() {
	pos = Camera::getPosition();
	texture = new GLuint[4];
	glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
	Info::RaiseCurTextureNum();
	texture[0] = LoadTexture("Texture/clouds/cloud1.png");
	glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
	Info::RaiseCurTextureNum();
	texture[1] = LoadTexture("Texture/clouds/cloud2.png");
	glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
	Info::RaiseCurTextureNum();
	texture[2] = LoadTexture("Texture/clouds/cloud3.png");
	glActiveTexture(GL_TEXTURE0 + Info::GetCurTextureNum());
	Info::RaiseCurTextureNum();
	texture[3] = LoadTexture("Texture/clouds/cloud4.png");
	shaderprogram = LoadShader("Shaders/cloud.vert","Shaders/cloud.frag");
	start = Vector3D(-100,-100,100);
	/*_width = 200;
	_height = 200;
	_depth = 50;
	data = new Data[_width*_height*_depth];
	Perlin perlin;
	perlin.init();
	int count = 0;
	int result = 0;
	Vector3D seed((rand())%1000/1000.0,(rand())%1000/1000.0,(rand())%1000/1000.0);
	for(int i = start.x; i < start.x+_width; i++) {
		for(int j = start.y; j < start.y+_height; j++) {
			for(int h = start.z; h < start.z+_depth; h++) {
				data[count].location = Vector3D((rand()%20000)-10000,(rand()%20000)-10000, 530 + (rand()%1000)/50.0);
				data[count].value = perlin.PerlinNoise3D(((float)i+seed.x)/100.0,((float)j+seed.y)/100.0,((float)h+seed.z)/100.0,1.5,1.5,8);
			//	std::cout << data[count].value << "\n";
				if(data[count].value > 0.35) result++;
				count++;
			}
		}
	}*/
	exploded = false;
	sort = false;
	ConvertToMdl("Models/cloud.obj");
	ifstream file("Models/Clouds.mdl");
    int _size;
	if (!file.is_open()) std::cout << "can not load 'cloud2.mdl'";
    file >> _size;
	int size = _size;
    float vertex_size = size*3;
    float *vertex = new float[size*3];
    for(int i = 0; i < size*3; i++) {
        file >> vertex[i];
    }
    file >> size;
    float vertex_n_size = size*3;
    float *vertex_n = new float[size*3];
    for(int i = 0; i < size*3; i++) {
        file >> vertex_n[i];
    }
    file >> size;
    float vertex_t_size = size*3;
	float *vertex_t = new float[size*3];
    for(int i = 0; i < size*3; i++) {
        file >> vertex_t[i];
    }
    file >> size;
    float faces_size = size*9;
    int *faces = new int[size*9];
    for(int i = 0; i < size*9; i++) {
        file >> faces[i];
    }

	num_of_puff = size;
    list = new Vector3D[num_of_puff];
	speed = new Vector3D[num_of_puff];
	this->size = new int[num_of_puff];
	type = new int[num_of_puff];
	locSpeed = new float[num_of_puff];
	int scale = 3;
    /*for(int i = 0; i < _size*3; i+=3) {
        file >> tmp;
		list[i/3].x = tmp*scale+1000;
		file >> tmp;
		list[i/3].y = tmp*scale+1000;
		file >> tmp;
		list[i/3].z = tmp*scale+350;
		speed[i/3] = Vector3D(((float)(rand()%1000))/100000.0+0.1,((float)(rand()%1000))/100000.0+0.1,0);
		this->size[i/3] = rand()%50 + 35;
		type[i/3] = rand()%4;
    }*/
	num_of_normal = size*3;
	normal = new Vector3D[size*3];
	
	cloudSpeed = Vector3D(1.0, 1.0, 0.0);
	for(int i = 0; i < faces_size; i+=9) {
		Vector3D n1 = Vector3D(vertex_n[(faces[i+2]-1)*3],vertex_n[(faces[i+2]-1)*3+1],vertex_n[(faces[i+2]-1)*3+2]);
		Vector3D n2	= Vector3D(vertex_n[(faces[i+5]-1)*3],vertex_n[(faces[i+5]-1)*3+1],vertex_n[(faces[i+5]-1)*3+2]);
		Vector3D n3 = Vector3D(vertex_n[(faces[i+8]-1)*3],vertex_n[(faces[i+8]-1)*3+1],vertex_n[(faces[i+8]-1)*3+2]);
		Vector3D vert = (Vector3D(vertex[(faces[i]-1)*3],vertex[(faces[i]-1)*3+1],vertex[(faces[i]-1)*3+2])+
						Vector3D(vertex[(faces[i+3]-1)*3],vertex[(faces[i+3]-1)*3+1],vertex[(faces[i+3]-1)*3+2])+
						Vector3D(vertex[(faces[i+6]-1)*3],vertex[(faces[i+6]-1)*3+1],vertex[(faces[i+6]-1)*3+2]))/3.0;
		vert.x = vert.x * scale + 1000;
		vert.y = vert.y * scale + 1000;
		vert.z = vert.z * scale + layerHeight;
		list[i/9] = vert; 
		normal[i/3] = n1;
		normal[i/3+1] = n2;
		normal[i/3+2] = n3;
		speed[i/9] = Vector3D(((float)(rand()%1000))/100000.0+0.1,((float)(rand()%1000))/100000.0+0.1,0);
		this->size[i/9] = rand()%50 + 35;
		type[i/9] = rand()%4;
		locSpeed[i / 9] = (rand() % 1000) / 1000.0;
    }
	std::cout << "puff_num = " << num_of_puff << "\n";
	list = qSort(list,0,num_of_puff-1);
	delete[] vertex;
	delete[] vertex_n;
	delete[] vertex_t;
	delete[] faces;
}
void Clouds::setShader(Vector3D posit,float rad,int light){
			glActiveTexture(GL_TEXTURE0 + Info::GetCurBindTexture());
		    glBindTexture(GL_TEXTURE_2D,texture[0]);
	        glUniform1i(glGetUniformLocation(
				shaderprogram, "cloud1"), Info::GetCurBindTexture());
			Info::RaiseCurBindTexture();

			glActiveTexture(GL_TEXTURE0 + Info::GetCurBindTexture());
		    glBindTexture(GL_TEXTURE_2D,texture[1]);
	        glUniform1i(glGetUniformLocation(
				shaderprogram, "cloud2"), Info::GetCurBindTexture());
			Info::RaiseCurBindTexture();

			glActiveTexture(GL_TEXTURE0 + Info::GetCurBindTexture());
		    glBindTexture(GL_TEXTURE_2D,texture[2]);
	        glUniform1i(glGetUniformLocation(
				shaderprogram, "cloud3"), Info::GetCurBindTexture());
			Info::RaiseCurBindTexture();

			glActiveTexture(GL_TEXTURE0 + Info::GetCurBindTexture());
		    glBindTexture(GL_TEXTURE_2D,texture[3]);
	        glUniform1i(glGetUniformLocation(
				shaderprogram, "cloud4"), Info::GetCurBindTexture());
			Info::RaiseCurBindTexture();

			glUniform3f(glGetUniformLocation(
				shaderprogram, "atmosphere"), Info::GetAtmoColor().x, Info::GetAtmoColor().y, Info::GetAtmoColor().z);
			glUniform1f(glGetUniformLocation(
	        		shaderprogram, "rad"), 10000);
	        glUniform3f(glGetUniformLocation(
				shaderprogram, "center"), 0, 0, layerHeight);
			Vector3D eye = Camera::getPosition();
			Vector3D sun = Info::GetSun();
			glUniform3f(glGetUniformLocation(
				shaderprogram, "sun"), sun.x,sun.y,sun.z);
			glUniform3f(glGetUniformLocation(
				shaderprogram, "eyePos"), eye.x,eye.y,eye.z);
			glUniform2f(glGetUniformLocation(
				shaderprogram, "movOffset"), Info::getTime() * 0.001, 0.0);
}
float Clouds::distance(Vector3D a, Vector3D b) {
    Vector3D c;
    c.x = (a.x  - b.x );
    c.y = (a.y - b.y);
    c.z = (a.z - b.z);
    return (float) sqrt((c.x *c.x  + c.y*c.y + c.z*c.z));
}
Vector3D* Clouds::qSort(Vector3D *A, int low, int high) {
          int i = low;
          int j = high;
          float x = distance(A[(low+high)/2],pos);
          do {
              while(distance(A[i],pos) > x) ++i;
              while(distance(A[j],pos) < x) --j;
              if(i <= j){
                  Vector3D temp;
				  swap(A[i],A[j]);
				  swap(type[i],type[j]);
				  swap(locSpeed[i], locSpeed[j]);
				  swap(size[i],size[j]);
				  swap(speed[i],speed[j]);

				  swap(normal[i*3],normal[j*3]);
				  swap(normal[i*3+1],normal[j*3+1]);
				  swap(normal[i*3+2],normal[j*3+2]);
                  i++; j--;
              }
          } while(i < j);
          if(low < j) A = qSort(A, low, j);
          if(i < high) A = qSort(A, i, high);
          return A;
}
void Clouds::Draw() {
		//TODO: вынести переменные в Info
		bool reflect = false;
		Vector3D offset(0, 0, 0);

		list = qSort(list,0,num_of_puff-1);
		Vector3D pl = Camera::getPosition();
		pos = pl;
		float sky_size = 10000.0;
		if(!Info::GetShader()) {
			yaw = 0;
			pitch = 0;
		}
		else {
			yaw = Camera::GetYaw();
			pitch = -Camera::GetPitch();
		}
		int radius = 2;
		Vector3D point1 = Vector3D(0,-radius,-radius);
	    Vector3D point2 = Vector3D(0,radius,-radius);
	    Vector3D point3 = Vector3D(0,radius,radius);
	    Vector3D point4 = Vector3D(0,-radius,radius);
	    if(reflect) pitch = -pitch;

	    Matrix4x4 maty;
	    Matrix4x4 matz;
	    yaw = -yaw;   			    
	    maty.set((float)cos(pitch),0,(float)sin(pitch),0,
                   0,1,0,0,
                   (float)-sin(pitch),0,(float)cos(pitch),0,
                   0,0,0,1);
    

	    matz.set((float)cos(yaw+pi),(float)-sin(yaw+pi),0,0,
    		(float)sin(yaw+pi),(float)cos(yaw+pi),0,0,
                   0,0,1,0,
                   0,0,0,1);
	   // System.out.println(inverse_puff[start_puff].x + " , " + inverse_puff[start_puff].y + " , " + inverse_puff[start_puff].z);
		matz = matz.mult(maty); 
		point1 = matz.mult(point1);
		point2 = matz.mult(point2);
		point3 = matz.mult(point3);
		point4 = matz.mult(point4);
	    
	    
	//	if(Info::shader) {	
		glUseProgram(shaderprogram);
			setShader(Vector3D(0,0,0),400,1);
	//	}
		glEnable(GL_DEPTH_TEST);
        glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER,0.0);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		if(Info::GetShader()) glDepthMask(false);
	    glBegin(GL_QUADS);
			for(int i = 0; i < num_of_puff; i++) {
				int loc_size = size[i];
			//	if(abs(dist(speed[i],Vector3D(0,0,0))) > 1) speed[i]/=1.5;
				if(Info::GetShader()) list[i] += cloudSpeed * Info::GetElapsedTime();
				if(list[i].x < (pl.x - sky_size)) list[i].x += sky_size*2.0;
				else if (list[i].x > (pl.x + sky_size)) list[i].x -= sky_size*2.0;

				if(list[i].y < (pl.y - sky_size)) list[i].y += sky_size*2.0;
				else if(list[i].y > (pl.y + sky_size)) list[i].y -= sky_size*2.0;

				glColor3f(0,locSpeed[i],type[i]);
				glNormal3f(normal[i*3].x,normal[i*3].y,normal[i*3].z);
				glTexCoord2d(0,0);
				glVertex3f(point1.x*loc_size+list[i].x,point1.y*loc_size+list[i].y,point1.z*loc_size+list[i].z);

				glNormal3f(normal[i*3+1].x,normal[i*3+1].y,normal[i*3+1].z);
		    	glTexCoord2d(1,0);
		    	glVertex3f(point2.x*loc_size+list[i].x,point2.y*loc_size+list[i].y,point2.z*loc_size+list[i].z);

				glNormal3f(normal[i*3+2].x,normal[i*3+2].y,normal[i*3+2].z);
		    	glTexCoord2d(1,1);
		    	glVertex3f(point3.x*loc_size+list[i].x,point3.y*loc_size+list[i].y,point3.z*loc_size+list[i].z);

				glNormal3f(normal[i*3+2].x,normal[i*3+2].y,normal[i*3+2].z);
		    	glTexCoord2d(0,1);
		    	glVertex3f(point4.x*loc_size+list[i].x,point4.y*loc_size+list[i].y,point4.z*loc_size+list[i].z);
			}
		glEnd();
		glDisable(GL_DEPTH_TEST);
        glDisable(GL_ALPHA_TEST);
        glDisable(GL_BLEND);
        if(Info::GetShader()) glDepthMask(true);
		glUseProgram(0);
}