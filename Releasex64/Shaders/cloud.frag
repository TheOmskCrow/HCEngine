#version 120
#pragma optionNV(fastmath on)
#pragma optionNV(fastprecision on)
#pragma optionNV(ifcvt none)
#pragma optionNV(inline all)
#pragma optionNV(strict on)
#pragma optionNV(unroll all)
varying vec3 pos,n;
varying vec2 _texCoord;
uniform sampler2D cloud1;
uniform sampler2D cloud2;
uniform sampler2D cloud3;
uniform sampler2D cloud4;
uniform vec3 center;
uniform vec3 sun;
uniform vec3 eyePos;
uniform float rad;
uniform vec3 atmosphere;
varying float type;
varying float locSpeed;
uniform vec2 movOffset;
uniform float addit_transp = 1.0;

float module(vec3 a, vec3 b) {
    vec3 c = a - b;
    return sqrt(c.x*c.x + c.y*c.y + c.z*c.z);
}

float dist(vec2 a, vec2 b) {
    vec2 c = a - b;
    return sqrt(c.x*c.x + c.y*c.y);
}

void main(void) {
	vec2 texCoord = _texCoord + movOffset * locSpeed;
	
	float transpCenter = min((1.0 - min(dist(vec2(0.5, 0.5), _texCoord) * 2.0, 1.0)), 1.0);
	
    vec4 color;
	if(type < 0.5) color.w = texture2D(cloud1,texCoord).x;
	else if(type < 1.5) color.w = texture2D(cloud2,texCoord).x;
	else if(type < 2.5) color.w = texture2D(cloud3,texCoord).x;
	else color.w = texture2D(cloud4,texCoord).x;
	
	color.w *= transpCenter;
//	color.w /= 2.0;
	color.xyz = vec3(1.0);
	float poz = pos.z + 50.0;
	float r = max(0.0,min(dot(vec3(0.0,0.0,1.0),normalize(sun)),1.0));
	/*if(poz > center.z + 100.0) r /= 1.0;
	else if(poz < center.z) r /= 2.0;
	else {
		r /= 1.0 + (1.0 - (poz - center.z)/100.0);
	}*/
	r = max(0.05,r);
	
	float _r = max(r*0.5,min(r*1.5,dot(n,normalize(sun))));
	gl_FragColor = vec4(color.xyz*_r,color.w * addit_transp);
	float tmp = gl_FragColor.w;
	float fr = module(eyePos,pos);
	float start = 0.0;
	float end = 10000.0;
	vec4 summar = vec4(atmosphere,1.0);
	if(fr < end) {
	    fr /= end;
	    gl_FragColor = gl_FragColor*(1.0 - fr) + summar*fr;
    }
	else gl_FragColor = summar;
	gl_FragColor.w = tmp;
}
