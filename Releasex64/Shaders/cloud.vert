#version 120
varying vec3 pos,n;
varying float type;
varying vec2 _texCoord;
varying float locSpeed;

void main(void) {
    gl_Position = ftransform();
    gl_ClipVertex = gl_ModelViewMatrix*gl_Vertex;
    pos = gl_Vertex.xyz;
    _texCoord = gl_MultiTexCoord0.xy;
	locSpeed = gl_Color.y;
	type = gl_Color.z;
	n = gl_Normal.xyz;
}