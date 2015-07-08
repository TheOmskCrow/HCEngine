#version 120
#pragma optionNV(fastmath on)
#pragma optionNV(fastprecision on)
#pragma optionNV(ifcvt none)
#pragma optionNV(inline all)
#pragma optionNV(strict on)
#pragma optionNV(unroll all)
varying vec3 pos;
varying vec2 texCoord;
uniform sampler2D star;
uniform float time;
uniform float poz;
uniform vec3 pozit;
uniform vec3 sunz;
uniform vec3 sunz2;
uniform vec3 light;
#extension GL_EXT_gpu_shader4 : enable


float Noise(float x, float y ,float z) {
    float n = 2.0*z + x + y * 57.0;
    n = float(int(( ((4.0*(n-z) * (n/2.0 * (z + 15731.0) + 789721.0) + 43589.0)) / 107374.0))%1000);
    n /= 1000.0;
    return n;
}

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
	float poz2 = pos2.z;
    bool lower = false;
    vec3 sun = sunz + pozit;
    vec3 sun2 = sunz2 + pozit;
    vec2 poser = pos2.xy;
    if((pos2.z - poz) < 0.0) {
        lower = true;
       pos2.z = poz;
    }
	lower = false;
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
	pos2.z = poz2;
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
	vec4 output1 = vec4(0.0);
	vec4 output2 = vec4(0.0);
    if(!lower) {
		float radius = 0.25;
		if((sun.z-pozit.z) > 0.0) {
	        step = (sun.z-pozit.z)/20000.0 + 0.75;
	    }
	    else {
			step = 0.75;
			radius = 0.25 - abs(sun.z-pozit.z)/5000.0;
		}
	 
	    float doter = dot(normalize(sun-pozit),normalize(pos2-pozit));
	    if(doter > 0.999) {
			float blur = doter;
			output1 += vec4(1.0,1.0,1.0*step,0.0)*pow(blur,2.0) + vec4(0.5,0.5,0.5*step,0.0);
		}
		else if(doter > (0.5-radius)) {
	        float blur = (doter - (0.5-radius))*1.25;
			output1 = vec4(0.35,0.35,0.35*step,0.0)*pow(blur,2.0);
			if(doter > 0.995) {
					float blur2 = (doter - 0.995)*250.0;
					output1 += vec4(0.5,0.5,0.5*step,0.0)*pow(blur2,2.0);
			}
		}
    }
	if(!lower) {
		float radius = 0.25;
		if((sun2.z-pozit.z) > 0.0) {
	        step = (sun2.z-pozit.z)/20000.0 + 0.75;
	    }
	    else {
			step = 0.75;
			radius = 0.25 - abs(sun2.z-pozit.z)/5000.0;
		}

	    float doter = dot(normalize(sun2-pozit),normalize(pos2-pozit));
	    if(doter > 0.9998) {
			float blur = doter;
			output2 += vec4(.8,0.35,.35*step,0.0)*pow(blur,2.0) + vec4(0.8,0.35,.35*step,0.0);
		}
		else if(doter > (0.5-radius)) {
	        float blur = (doter - (0.5-radius))*1.25;
			output2 += vec4(0.36,0.35,0.35*step,0.0)*pow(blur,2.0);
			if(doter > 0.997) {
					float blur2 = (doter - 0.997)*250.0;
					output2 += vec4(1.2,0.8,0.8*step,0.0)*pow(blur2,2.0);
			}
		}
    }
	gl_FragColor += (output1 + output2);
	gl_FragColor.w = 1.0;
	float relee_inten = min(max(1.0 - (pozit.z / 3000.0),0.0),1.0);
	float blend_c = 0.45 * relee_inten;
	if(sun.z <= 0.0) blend_c /= (abs(sun.z)+250.0)/250.0;
	float Relee_disp = 0.6;
	if(sun.z > 0.0) Relee_disp = min(max(1.0 / ((sun.z)/300.0),0.0),1.0)*0.6;
	if((pos.z - pozit.z)*cosinus >= 0.0 && (pos.z - pozit.z)*cosinus <= 2000.0) {
		float blend = (2000.0 - (pos.z - pozit.z)*cosinus)/2000.0;	
		gl_FragColor += vec4(blend_c*pow(blend,2.0))*vec4(0.4+Relee_disp,0.7,0.6,1.0);
	}
	else if((pos.z - pozit.z)*cosinus < 0.0) {
		gl_FragColor += vec4(blend_c)*vec4(0.4+Relee_disp,0.7,0.6,1.0);
	}

	float lum = gl_FragColor.x*0.299 + gl_FragColor.y*0.587 + gl_FragColor.z*0.114;

    vec4 color = texture2D(star,texCoord);
    float blink = Noise(pos.x/10.0,pos.y/10.0,time);
    blink /=4.0;
    blink += 0.75;
    color.w *= blink;

     float _blue = abs(pos.z);
     _blue /= 3.0;
     _blue += 0.7;
     int col = int(abs(pos.z)*100.0);
     if(col % 2 == 0) {

         color.y *= _blue*2.0;
         color.z *= _blue;
     }
     else {
         color.y *= _blue*2.0;
         color.x *= _blue;
     }
	if(lum < 0.05) {
		gl_FragColor = color*(1.0 - lum * 20.0)*2.0;
	}
	else gl_FragColor.w = 0.0;
}

