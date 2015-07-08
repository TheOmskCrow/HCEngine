#version 120
varying vec3 pos;
varying vec2 texCoord;
uniform int reflection;

void main(void) {
    gl_Position = ftransform();
    gl_ClipVertex = gl_ModelViewMatrix*gl_Vertex;
    pos = (gl_ModelViewMatrix*gl_Vertex).xyz;
	texCoord = gl_MultiTexCoord0.xy;
	texCoord.y *= -1.0;
	if(reflection == 1) pos.z *= -1.0;
}