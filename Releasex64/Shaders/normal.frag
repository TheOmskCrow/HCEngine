#version 120
uniform sampler2D unit_wave;
varying vec2 texCoord;
const vec2 size = vec2(10.0,1.0);
uniform float Width;
uniform float zFar;
uniform float zNear;
uniform float Height;
vec3 off = vec3(1.0/Width,0.0,1.0/Height);

void main() {	
	float z_b = texture2D(unit_wave,texCoord).x;
	float z_n = 2.0 * z_b - 1.0;
	float z_e = 2.0 * zNear * zFar / (zFar + zNear - z_n *(zFar - zNear));
	z_e -= 0.1;
	z_e /= 100.0;
	z_e += 0.1;
	if(z_e > 0.98) z_e = 0.98;
	float s11 = z_e;

	vec3 normal = normalize(vec3(dFdx(s11) * 500.0, dFdy(s11) * 500.0, 1.0)) * 0.5 + 0.5;
	gl_FragColor.xyz = normal;
	gl_FragColor.w = 1.0;
}