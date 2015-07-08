#version 120
varying vec3 pos,n;
varying vec2 texCoord;

void main(void) {
    gl_Position = ftransform();
    gl_ClipVertex = gl_ModelViewMatrix*gl_Vertex;
    pos = gl_Vertex.xyz;
    texCoord = gl_MultiTexCoord0.xy;
	n = gl_Normal.xyz;
}