#version 120
varying vec4 vertex;
varying vec2 _texCoord;
varying mat4 shadow_matrix0;
varying mat4 shadow_matrix1;
varying mat4 shadow_matrix2;

void main(void) {
    gl_Position = ftransform();
    gl_ClipVertex = gl_ModelViewMatrix*gl_Vertex;
    vertex = gl_Vertex;
    _texCoord = gl_MultiTexCoord0.xy;
	shadow_matrix0 = gl_TextureMatrix[1];
	shadow_matrix1 = gl_TextureMatrix[2];
	shadow_matrix2 = gl_TextureMatrix[3];
}