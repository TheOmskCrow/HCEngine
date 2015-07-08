#version 120
varying vec3 pos,n;
varying vec2 texCoord;
uniform sampler2D cloud1;
uniform sampler2D cloud2;
uniform sampler2D cloud3;
uniform sampler2D cloud4;
uniform vec3 center;
uniform vec3 sun;
uniform vec3 eyePos;
uniform float rad;
varying float type;

float module(vec3 a, vec3 b) {
    vec3 c = a - b;
    return sqrt(c.x*c.x + c.y*c.y + c.z*c.z);
}

void main(void) {
    vec4 color;
	vec4 tx;
	if(type < 0.5) color.w = texture2D(cloud1,texCoord).x;
	else if(type < 1.5) color.w = texture2D(cloud2,texCoord).x;
	else if(type < 2.5) {
		tx = texture2D(cloud3,texCoord);
		color.w = tx.w;
	}
	else {
		tx = texture2D(cloud4,texCoord);
		color.w = tx.w;
	}
	color.w -= 0.25;
	color.w *= 1.5;
	color.xyz = vec3(1.0);
	if(type > 2.5) color.xyz = tx.xyz*1.5;
	float poz = pos.z + 50.0;
	gl_FragColor = vec4(color.xyz,color.w);
}
