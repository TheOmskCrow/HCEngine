#version 120
varying vec3 pos,l,n,v,h;
varying vec2 texCoord;
uniform sampler2D model;
uniform vec3 eyePos;
uniform sampler2D _shadow0;  
uniform sampler2D _shadow1;  
uniform sampler2D _shadow2;  
uniform sampler2D _shadow3;  
uniform sampler2D _shadow4;  
uniform sampler2D _shadow5; 
uniform sampler2D _shadow6; 
varying vec4 posit0;
varying vec4 posit1;
varying vec4 posit2;
varying vec4 posit3;
varying vec4 posit4;
varying vec4 posit5;
varying vec4 posit6;
uniform int flash;
uniform int shadowing;
uniform vec3 eyeNorm;
uniform vec3 offset;
uniform float Width;
uniform float Height;
uniform float transp;
uniform sampler2D normalMap;
uniform sampler2D heightMap;
varying	vec3 et;
varying	vec3 lt;
float MAX_BUMP_DIST = 20.0;
 
vec3 hcount(vec3 l, vec3 v) {
    float module = sqrt(pow(l.x+v.x,2.0) + pow(l.y+v.y,2.0) + pow(l.z+v.z,2.0));
    return (l+v)/module;
}
float dist(vec3 a, vec3 b) {
	vec3 c = a - b;
	return sqrt(c.x*c.x + c.y*c.y + c.z*c.z);
}  
float lin_shadow(sampler2D _sh_tex, float x,float y,vec2 tex_coor,vec3 shad_coor) {
        float z = 0.0;
	vec4 s;
	if(x == 0.0 && y == 0.0) return 1.0;
	vec2 real_coor = vec2(x,y);
	vec2 tex_offset = vec2(1.0/Width,1.0/Height);
	for(float i = -tex_offset.x; i <= tex_offset.x; i+=tex_offset.x) {
		for(float j = -tex_offset.y; j <= tex_offset.y; j+=tex_offset.y) {
			float shadow_blur = 2.0*sqrt(2.0) - abs(i-x)*abs(j-y)*Width*Height;
			float tmp_res = texture2D(_sh_tex,tex_coor + vec2(i,j)).x + 0.00001;
			if(i == 0.0 && j == 0.0) s.x = tmp_res;
			if(i == tex_offset.x && j == 0.0) s.y = tmp_res;
			if(i == tex_offset.x && j == tex_offset.y) s.z = tmp_res;
			if(i == 0.0 && j == tex_offset.y) s.w = tmp_res;
			if(shad_coor.z >= tmp_res) z += shadow_blur/(2.0*sqrt(2.0));
		}
	}
	z /= 9.0;
	float _z = 0.0;
	if(shad_coor.z >= s.x) {
		_z += (tex_offset.x-x)*(tex_offset.y-y)*Width*Height;
    }
    if(shad_coor.z >= s.y) {
		_z += (x)*(tex_offset.y-y)*Width*Height;
	}
	if(shad_coor.z >= s.z) {
		_z += (x)*(y)*Width*Height;	
	}
	if(shad_coor.z >= s.w) {
		_z += (tex_offset.x-x)*(y)*Width*Height;
	}
	_z = max(min(_z,1.0),0.0);
	return (_z + z)/2.0;
}
void main(void) {

	float fr = dist(eyePos,pos);
	float start = 0.0;
	float end = 5000.0;
	float summar = min(max( dot(normalize(l), vec3(0.0,0.0,1.0)), 0.1 ),0.5);
	if(fr > end) { 
   		gl_FragColor = vec4(summar);
   		return;
	} 
   vec2 tc = texCoord + vec2(offset.xy);
   gl_FragColor = texture2D(model,tc);
    gl_FragColor.w = pow(gl_FragColor.w,1.0/transp+0.001);
	if(fr > start) {
		fr = 1.0/(1.0 - (fr/end));
		fr = max(fr,0.5);
		gl_FragColor.x = summar + (gl_FragColor.x - summar)/fr;
		gl_FragColor.y = summar + (gl_FragColor.y - summar)/fr;
		gl_FragColor.z = summar + (gl_FragColor.z - summar)/fr;
    }
}