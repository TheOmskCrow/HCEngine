#version 120
varying vec3 n;
varying vec3 pos;
uniform int reflection;

void main(void) {
    gl_Position = ftransform();
    gl_ClipVertex = gl_ModelViewMatrix*gl_Vertex;
    n = normalize(gl_Normal.xyz);
    pos = gl_Vertex.xyz;
	//if(reflection == 1) pos.z *= -1.0;
}
