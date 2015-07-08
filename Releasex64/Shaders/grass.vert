#version 120
varying vec3 pos,l,n,v,h;
varying vec2 texCoord;
uniform vec3 eyePos;
uniform int type;
uniform vec3 windCenter;
uniform float distanse;
uniform vec3 lightPos;
uniform float offset;
uniform float dstbl;
uniform vec3 terNormal;
varying	vec3 et;
varying	vec3 lt;

uniform vec3 border1;
uniform vec3 border2;
uniform vec3 border3;
uniform vec3 border4;
uniform vec3 center;

float dist(vec3 a, vec3 b) {
	vec3 c = a - b;
	return sqrt(c.x*c.x + c.y*c.y + c.z*c.z);
}
void main(void) {
	float material = 0.25;
	if(type == 1) material = 0.05;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;	
	vec4 stp = gl_Vertex;	
	float phase = offset/2.0 + (stp.x + stp.y)/10.0;
	float ammount = 0.0;
	if(abs(stp.z) > 2.0) {
		ammount = 0.25;
		stp.x += ammount*(sin(phase*20.0)*abs(sin(phase)))*material;
		stp.y += ammount*(sin(phase*20.0)*abs(cos(phase/2.0)))*material;
	}
	else if(abs(stp.z) > 0.4) {
		ammount = 0.125;
		stp.x += ammount*(sin(phase*20.0)*abs(sin(phase)))*material;
		stp.y += ammount*(sin(phase*30.0)*abs(cos(phase/2.0)))*material;
	}
	if(type == 0) {
		vec2 stpRes = (gl_ModelViewMatrix*stp).xy;
		float z = 0;
		z += border1.z * (10.0 - (stpRes.x - border1.x))*(10.0 - (stpRes.y - border1.y))*0.01;
		z += border2.z * (10.0 - (stpRes.x - border2.x))*(10.0 - (border2.y - stpRes.y))*0.01;
		z += border3.z * (10.0 - (border3.x - stpRes.x))*(10.0 - (stpRes.y - border3.y))*0.01;
		z += border4.z * (10.0 - (border4.x - stpRes.x))*(10.0 - (border4.y - stpRes.y))*0.01;
		
		float res = center.z - z;
		if(res > 0.0) stp.z -= res;
	}
	gl_Position = gl_ModelViewProjectionMatrix * stp;
    gl_ClipVertex = gl_ModelViewMatrix*stp;
    pos = (gl_ModelViewMatrix*stp).xyz;

	vec3 cur = pos;
	float dst = dist(windCenter,cur) - distanse;
	dst = abs(dst);
	if(dst < 10.0 && distanse < 100.0) {
		float smooth = pow(1.0/max(1.0,dst),0.5);
		vec2 windVelocity = normalize(cur.xy - windCenter.xy)*40.0*smooth;
		gl_Position += gl_ProjectionMatrix*vec4(windVelocity.x*ammount,windVelocity.y*ammount,0.0,0.0);
		pos += vec3(windVelocity.x*ammount,windVelocity.y*ammount,0.0);
	}
    texCoord = gl_MultiTexCoord0.xy;
    l = normalize(lightPos);
	n = normalize(terNormal);
	if(dstbl > 0.5) n = normalize(n + vec3(0.0,0.0,1.0));
    v = normalize(eyePos - pos);
    h = normalize(l + v);

    vec3 tangent;
    vec3 binormal;
    vec3 c1 = cross(gl_Normal, vec3(0.0, 0.0, 1.0));
    vec3 c2 = cross(gl_Normal, vec3(0.0, 1.0, 0.0));
    if (length(c1)>length(c2)) {
        tangent = c1;
    }
    else {
        tangent = c2;
    }
    tangent = normalize(tangent);
    binormal = cross(gl_Normal, tangent);
    binormal = normalize(binormal);
    vec3 t = tangent;
    vec3 b = binormal;                                                    
    et = vec3 ( dot ( v, t ), dot ( v, b ), dot ( v, n ) );
    lt = vec3 ( dot ( l, t ), dot ( l, b ), dot ( l, n ) );
}