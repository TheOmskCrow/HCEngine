#version 120
varying vec3 pos;
varying vec2 _texCoord;

void main(void) {
    gl_Position = ftransform();
    gl_ClipVertex = gl_ModelViewMatrix*gl_Vertex;
    pos = gl_Vertex.xyz;
    _texCoord = gl_MultiTexCoord0.xy;
}