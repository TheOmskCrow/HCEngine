#version 120
#pragma optionNV(fastmath on)
#pragma optionNV(fastprecision on)
#pragma optionNV(ifcvt none)
#pragma optionNV(inline all)
#pragma optionNV(strict on)
#pragma optionNV(unroll all)
varying vec3 n;
varying vec3 pos;
uniform bool reflection;
uniform float poz;
uniform vec3 pozit;
uniform vec3 sunz;
uniform vec3 sunz2;
uniform sampler2D tex;
uniform vec3 light;
varying vec2 texCoord;


float module(vec2 a,vec2 b) {
    vec2 c = b - a;
    return sqrt(c.x*c.x + c.y*c.y);
}
float module2(vec3 a,vec3 b) {
    vec3 c = b - a;
    return sqrt(c.x*c.x + c.y*c.y + c.z*c.z);
}

void main(void) {
vec3 pos2 = pos;
    bool lower = false;
    vec3 sun = sunz + pozit;
    vec2 poser = pos2.xy;
    if((pos2.z - poz) < 0.0) {
        lower = true;
       pos2.z = poz;
    }
    float cosinus = 5000.0/module(vec2(pos2.x,pos2.y),vec2(pozit.x,pozit.y));
    float tetta = acos((pos2.z - pozit.z)/module2(pozit,pos2));
    float tetta_s = acos((sun.z - pozit.z)/module2(pozit,sun));
    vec3 a = sun - pozit;
    vec3 b = pos2 - pozit;
    float gamma = dot(a,b)/(dot(a,a)*dot(b,b));
    float step = 1.0 - (5000.0 - sunz.z)/5000.0;
    if(step <= 0.1) step = step + (0.1 - step)/1.2;
    step = max(0.0,step);
    float blue = 0.0;
    float zz = (pos2.z - pozit.z)*cosinus;
    if(zz <= 1000.0) {
        zz = 1.0 - (zz/1000.0);
        blue = zz;
    }
    else blue = 0.0;
    float steper = step;
    if(step <= 0.2) {
        steper = 1.0 - (steper*5.0);
        blue *= steper;
    }
    else blue = 0.0;
    vec4 Yz = vec4(0.05*step-poz/200000.0,0.05*step-poz/200000.0,0.25*step-poz/40000.0,1.0);//vec4(0.04-poz/100000.0,0.05-poz/80000.0,0.25-poz/4000.0,1.0);
    Yz.x = min(max(0.0,Yz.x),1.0);
    Yz.y = min(max(0.0,Yz.y),1.0);
    Yz.z = min(max(0.0,Yz.z),1.0);
    if(blue != 0.0) {
        Yz.z -= Yz.z*blue;
        Yz.y -= Yz.y*blue/2.0;
    }
    vec4 Yc = Yz*((0.91+10.0*exp(-3.0*gamma)+0.45*pow(cos(gamma),2.0))*(1.0-exp(-0.32/cos(tetta))))/((0.91+10.0*exp(-3.0*tetta_s)+0.45*pow(cos(tetta_s),2.0))*(1.0-exp(-0.32)));
    Yc.x = min(max(0.0,Yc.x),1.0);
    Yc.y = min(max(0.0,Yc.y),1.0);
    Yc.z = min(max(0.0,Yc.z),1.0);
    Yc.w = min(max(0.0,Yc.w),1.0);
    gl_FragColor = Yc;
	gl_FragColor.gb = vec2(gl_FragColor.b * 0.5, gl_FragColor.b * 0.85);
    if(!lower) {
    float radius = 0.25;
    if((sun.z-pozit.z) > 0.0) {
        step = (sun.z-pozit.z)/20000.0 + 0.75;
    }
    else {
        step = 0.75;
        radius = 0.25 - abs(sun.z-pozit.z)/5000.0;
    }
	float relee = 1.0;
	if(sun.z < 1000.0) {
		relee = max(0.0,sun.z / 1000.0);
	}
    float doter = dot(normalize(sun-pozit),normalize(pos2-pozit));
   // float dist = module2(pos2,sun);
    if(doter > 0.999) {
        float blur = doter;
        gl_FragColor = vec4(1.1,0.8*max(min(1.0, relee*1.5),0.15),0.6*step*relee,0.0)*pow(blur,2.0) + gl_FragColor + vec4(0.55,0.4*max(min(1.0, relee*1.5),0.15),0.3*step*relee,0.0);
    }
    else if(doter > (0.5-radius)) {
        float blur = (doter - (0.5-radius))*1.25;
        gl_FragColor = vec4(0.4,0.3*max(min(1.0, relee*1.5),0.15),0.25*step*relee,0.0)*pow(blur,2.0) + gl_FragColor;
        if(doter > 0.995) {
                float blur2 = (doter - 0.995)*250.0;
                gl_FragColor = vec4(0.4,0.3*max(min(1.0, relee*1.5),0.15),0.25*step*relee,0.0)*pow(blur2,2.0) + gl_FragColor;
        }
    }
    }
    gl_FragColor.w = 1.0;
	
	gl_FragColor.x = max(0.0, gl_FragColor.x);
	gl_FragColor.y = max(0.0, gl_FragColor.y);
	gl_FragColor.z = max(0.0, gl_FragColor.z);
	vec4 color = texture2D(tex,texCoord)/2.0;
	float lum = gl_FragColor.x*0.299 + gl_FragColor.y*0.587 + gl_FragColor.z*0.114;

	if(lum < 0.1 && pos.z > 10000.0) {//problem with outer space travel
		gl_FragColor += color*(1.0 - lum * 10.0)*2.0;
	}
}

