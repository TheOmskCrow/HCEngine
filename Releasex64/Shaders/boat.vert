#version 120
varying vec3 pos,l,n,v,h;
varying vec2 texCoord;
varying vec4 posit0;
varying vec4 posit1;
varying vec4 posit2;
varying vec4 posit3;
varying vec4 posit4;
varying vec4 posit5;
varying vec4 posit6;
uniform vec3 eyePos;
uniform vec3 lightPos;

void main(void) {
    gl_Position = ftransform();
   	posit0 = gl_TextureMatrix[1]*gl_Vertex;
    posit1 = gl_TextureMatrix[2]*gl_Vertex;
	posit2 = gl_TextureMatrix[3]*gl_Vertex;
	posit3 = gl_TextureMatrix[4]*gl_Vertex;
	posit4 = gl_TextureMatrix[5]*gl_Vertex;
	posit5 = gl_TextureMatrix[6]*gl_Vertex;
	posit6 = gl_TextureMatrix[7]*gl_Vertex;
    gl_ClipVertex = gl_ModelViewMatrix*gl_Vertex;
    pos = gl_Vertex.xyz;
    texCoord = gl_MultiTexCoord0.xy;
    l = normalize(lightPos - pos);
    n = normalize(gl_NormalMatrix * gl_Normal).xyz;
    v = normalize(eyePos - pos);
    h = normalize(l + v);
}