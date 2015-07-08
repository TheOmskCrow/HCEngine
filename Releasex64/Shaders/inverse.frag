#version 120
varying vec3 pos;
varying vec2 texCoord;
uniform sampler2D cloud;
uniform vec3 center;
uniform vec3 sun;
uniform int light;
uniform float rad;
uniform float frames;

float module(vec3 a, vec3 b) {
    vec3 c = a - b;
    return sqrt(c.x*c.x + c.y*c.y + c.z*c.z);
}

void main(void) {
    vec4 color = texture2D(cloud,texCoord);
    float fren = module(center,pos);
	if(fren > rad) color.w = 0.0;
	else {
		color.w *= 1.0 - fren/rad;
		color.x *= 0.4 + max((1.0 - fren/1000.0*sqrt(frames))/1.5,0.0);
		color.y *= 0.2 + max((1.0 - fren/1000.0*sqrt(frames))/2.0,0.0);
		color.z *= 0.2;// + max((1.0 - fren/1000.0*sqrt(frames))/2.0,0.0);;
	}
	vec3 _center = center;
	if(center.z > pos.z) _center.z = pos.z;
	vec3 l = normalize(sun - pos);
	vec3 n = normalize(pos - _center);
	float r1 = max(0.5,dot(l,n));
	if(light == 0) r1 = 1.0;
    gl_FragColor = color*r1;
}
