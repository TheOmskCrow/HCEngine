#version 120
uniform	sampler2D mainTex;
varying vec2 v_blurTexCoords[21];
varying float gauss[21];
varying vec2 v_texCoord;

void main (void)
{
	vec4 color = vec4(0.0);
	int a = 1;
	int b = 0;
	for(int i = 0; i < 11; i++) {
		color += texture2D(mainTex, v_blurTexCoords[i]) * a;
		a++;
		b+=a;
	}
	a--;
	for(int i = 11; i < 21; i++) {
		color += texture2D(mainTex, v_blurTexCoords[i]) * a;
		a--;
		b+=a;
	}
	
	
	gl_FragColor = (color / b);
}

