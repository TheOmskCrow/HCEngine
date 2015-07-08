#version 120
const float pi = 3.14159;
uniform float time;
varying vec4 pos;
varying vec3 posit;
varying vec2 vx;
varying vec3 position;
varying vec3 worldNormal;
varying float timer,ter_hei;
uniform vec3 eyePos;
uniform vec3 lightPos;
varying vec2 texCoord;
varying float z_normal;
#extension GL_EXT_gpu_shader4 : enable


float Noise(float x, float y ,float z) {
    float n = 2.0*z + x + y * 57.0;
    n = float(int(( ((4.0*(n-z) * (n/2.0 * (z + 15731.0) + 789721.0) + 43589.0)) / 107374.0))%1000);
    n /= 1000.0;
    return n;
}

float dist(vec3 start, vec3 end) {
    vec3 vect = start - end;
    return sqrt(vect.x*vect.x + vect.y*vect.y + vect.z*vect.z);
}
float h(float x, float y) {
	return 0.0;
    x+=500.0;
    y+=500.0;
	x*=4.0;
	y*=4.0;
    float sum = 0.0;
    for(int k = 0; k < 2; k++) {
        sum += (1.0/pow(2.0,float(k)))*abs(2.0*Noise(pow(2.0,float(k))*(x),pow(2.0,float(k))*(time/8.0),pow(2.0,float(k))*(y))-1.0);
    }
    return sum;
}


vec3 normaln(vec3 A, vec3 B, vec3 C) {
    float x = A.y*(B.z - C.z)+ B.y*(C.z - A.z) + C.y*(A.z - B.z);
    float y = A.z*(B.x - C.x) + B.z*(C.x - A.x) + C.z*(A.x - B.x);
    float z = A.x*(B.y - C.y) + B.x*(C.y - A.y) + C.x*(A.y - B.y);
    return vec3(x,y,z);
}
void main() {
	z_normal = gl_Normal.y;
	
	pos = gl_Vertex;
	vx = vec2(gl_Vertex.x, gl_Vertex.y);
	position = pos.xyz / pos.w;
	worldNormal = vec3(0,0,1.0);

	timer = time;
	gl_TexCoord[0] = gl_ModelViewProjectionMatrix * pos;
	ter_hei = gl_Normal.z;
	gl_Position = gl_ModelViewProjectionMatrix * pos;
	posit = gl_Position.xyz;
	texCoord = gl_MultiTexCoord0.xy;
}