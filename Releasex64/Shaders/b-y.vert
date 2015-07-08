#version 120
varying vec2 v_blurTexCoords[21];
varying float gauss[21];
varying vec2 v_texCoord;

uniform float Width;
uniform float Height;

void main()
{
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    v_texCoord = gl_MultiTexCoord0.xy;
	float height = 1.0 / Height;
	int a = 0;
	for(int i = -10; i <= 10; i++) {
		gauss[a] = abs(float(i));
		v_blurTexCoords[a] = v_texCoord + vec2(0.0, float(i) * height);
		a++;
	}
}
