#version 120
#pragma optionNV(fastmath on)
#pragma optionNV(fastprecision on)
#pragma optionNV(ifcvt none)
#pragma optionNV(inline all)
#pragma optionNV(strict on)
#pragma optionNV(unroll all)
varying vec3 worldNormal;
varying vec3 position;
varying vec4 pos;
uniform vec3 lightPos;
uniform vec3 eyePos;
uniform vec3 eyeNorm;
uniform int flash;
varying vec3 posit;
uniform float Width;
uniform float Height;
varying vec2 texCoord;
uniform int shadowing; 
uniform vec3 atmosphere;
uniform sampler2D envMap;
uniform sampler2D normalMap;
uniform sampler2D waterTex;
uniform float shadowDist = 100.0; 
uniform sampler2D envMap2;
varying float z_normal;
vec3 et;
vec3 lt;
float fresnel_output = 0.9; // 1.0 - ideal clear water
varying float timer,ter_hei;
const vec2 off1 = vec2(5.0,1.0);
const vec2 off2 = vec2(1.0,6.0);


vec3 hcount(vec3 l, vec3 v) {
    float module = sqrt(pow(l.x+v.x,2.0) + pow(l.y+v.y,2.0) + pow(l.z+v.z,2.0));
    return (l+v)/module;
}

float dist(vec3 start, vec3 end) {
    vec3 vect = start - end;
    return sqrt(vect.x*vect.x + vect.y*vect.y + vect.z*vect.z)/10.0;
}

float module(vec3 a, vec3 b) {
    vec3 c = a - b;
    return sqrt(c.x*c.x + c.y*c.y + c.z*c.z);
}

#extension GL_EXT_gpu_shader4 : enable

float Noise(float x, float y ,float z) {
    float n = 2.0*z + x + y * 57.0;
    n = float(int(( ((4.0*(n-z) * (n/2.0 * (z + 15731.0) + 789721.0) + 43589.0)) / 107374.0))%1000);
    n /= 1000.0;
    return n;
}
void main() {
	{
		vec3 l = normalize(lightPos);
		vec3 n = vec3(0,0,1.0);
		vec3 v = normalize(eyePos - pos.xyz);
	
		vec3 tangent;
		vec3 binormal;
		vec3 c1 = cross(vec3(0.0,0.0,1.0), vec3(0.0, 0.0, 1.0));
		vec3 c2 = cross(vec3(0.0,0.0,1.0), vec3(0.0, 1.0, 0.0));
		if (length(c1)>length(c2)) {
			tangent = c1;
		}
		else {
			tangent = c2;
		}
		tangent = normalize(tangent);
		binormal = cross(vec3(0.0,0.0,1.0), tangent);
		binormal = normalize(binormal);
		vec3 t = tangent;
		vec3 b = binormal;                                                    
		et = vec3 ( dot ( v, t ), dot ( v, b ), dot ( v, n ) );
		lt = vec3 ( dot ( l, t ), dot ( l, b ), dot ( l, n ) );
	}
	vec3 l = lt;//normalize(lightPos - position);
	float fr = module(eyePos, pos.xyz);
	float start = 0.0;
	float end = 5000.0;
	vec4 atmo_summary = (vec4(atmosphere,1.0) + vec4(min(max( dot(normalize(l), vec3(0.0,0.0,1.0)), 0.1 ),0.5)))/2.0;

    fr = min(end,fr);
    vec3 worldnormal = worldNormal;
	vec2 offset;
	float waves = abs(ter_hei*100.0 - pos.z);
	if(waves > 10.0) {
		waves = 10.0;
	}
	float distscale = 0.01 * waves;
	float bump_size = 10.0;
    vec3 bump;
    if(fr < 1500.0)
	bump = normalize( texture2D(normalMap, texCoord/500.0  + timer*-off1/2.0).xyz * 2.0 - 1.0) + normalize( texture2D(normalMap, texCoord/200.0 + timer*-off2/2.0).xyz * 2.0 - 1.0) * (500.0 - max(0.0,fr - 1000.0))/500.0;
    else bump = vec3(0.0, 0.0, 1.0);
    bump /= 2.0;
	vec3 r0 = hcount(normalize(et),normalize(lt));
	float r2 = pow ( max ( dot ( bump, r0), 0.0 ), 30.0);
	vec3 worldnormal2 = bump*worldnormal;
	worldnormal += bump;
	worldnormal /= 2.0;
	worldnormal += worldnormal2;
	worldnormal/=2.0;
	offset.x = -worldnormal.x*distscale;//+0.055;
    offset.y =  -worldnormal.y*distscale;//+0.055;
    vec3 n = bump;
    float r = min(max((0.0,0.5*dot(l,(vec3(0.0,0.0,1.0)))),dot(l, n)),1.0);
    vec3 v = et;//normalize(eyePos - position);
	if(lightPos.z < -0.1)r2 /= -lightPos.z*10.0;
	n = worldnormal;
    vec2 coor;
    vec4 ver = pos;
    ver.w = 1.0;
    vec4 pos2 = gl_ModelViewProjectionMatrix * ver;
    coor.x = (pos2.x / pos2.w) + 1.0;
    coor.y = (pos2.y / pos2.w) + 1.0;
	n.z *= z_normal;
    float cosQ = abs(dot(n, v)/(dot(n,n)*dot(v,v)));
	n.z *= z_normal;
    float fresnel;
	if(z_normal > 0.0) fresnel = max(min(pow(1.0 - cosQ,2.0) - 0.1,1.0),0.0);//max(pow(1.0 - cosQ,3.0) - 0.1,0.0);
	else fresnel = max(min(1.0 - cosQ,1.0),0.0);
    vec4 color;
    vec4 sum = vec4(0.3,0.3,0.3,1.0);
    sum.x = sum.x - abs(1.0/4.0);
    sum.y = sum.y - abs(1.0/6.0);
    sum.z = sum.z - abs(1.0/10.0);
    vec3 l2 = normalize(lightPos - eyePos);
    float rr = dot(l2,vec3(0.0,0.0,1.0));
    if(lightPos.z < 0.0) rr = 0.0;
    sum = sum*rr;
	float offset_val_x = 1.0;
	float offset_val_y = 1.0;
	coor/=2.0;
	if(coor.x > 0.9) {
		offset_val_x = (1.0 - coor.x)*10.0;
	}
	if(coor.x < 0.1) {
		offset_val_x = (coor.x)*10.0;
	}

	if(coor.y > 0.9) {
		offset_val_y = (1.0 - coor.y)*10.0;
	}
	if(coor.y < 0.1) {
		offset_val_y = (coor.y)*10.0;
	}
	offset = vec2(offset.x*offset_val_x,offset.y*offset_val_y);
    vec2 summary = coor + offset;//(coor+offset)/2.0;//  
    if(summary.x > 1.0) summary.x = 0.0;
    if(summary.y > 1.0) summary.y = 0.0;
    if(summary.x < 0.0) summary.x = 1.0;
    if(summary.y < 0.0) summary.y = 1.0;
	
	float pmax = 1.0;
	
	if(pmax != 1.0) r2 *= min(1.0,max(0.0,(pmax-0.5)*2.0));
	r*= pmax + (1.0 - pmax)/2.0;
	float deep_fresnel = 0.0;// max(-10.0,min(ter_hei*100.0,0.0))/-10.0;
	vec4 deep_color = vec4(atmosphere * 0.5, 0.0);
	float _r = 0.0;
	if(flash == 1 && fr < 100.0) {
		vec3 Normal = normalize(pos.xyz - eyePos);
		float flashlight = dot(normalize(eyeNorm),Normal);
		float distf = dist(pos.xyz,eyePos)*10.0;
		float diffuse = 1.0 - min(distf,5.0)/5.0;
		_r =  0.6*0.5*diffuse;
		float dist_max = 50.0;
		if(distf < dist_max) {
			float distf_coeff = distf /(dist_max * 100.0) * 7.0 + 0.9;
			if(flashlight > distf_coeff) {
				float blurf = (1.0 - distf_coeff) /10.0;
				float _blur = 1.0 / blurf;
				if(flashlight <= distf_coeff + blurf) blurf = (flashlight - distf_coeff)*_blur;
				else if(flashlight <= distf_coeff + 4.0*blurf && flashlight >= distf_coeff + 3.0*blurf) blurf = (1.0 - (flashlight - distf_coeff - 3.0 * blurf)*_blur)/2.0 + 0.5;
				else if(flashlight <= distf_coeff + 6.0*blurf && flashlight >= distf_coeff + 4.0*blurf) blurf = ((flashlight - distf_coeff - 4.0 * blurf)*_blur/2.0)/2.0 + 0.5;
				else if(flashlight <= distf_coeff + 9.0*blurf && flashlight >= distf_coeff + 6.0*blurf) blurf = ((flashlight - distf_coeff - 6.0 * blurf)*_blur/3.0)/2.0 + 1.0;
				else if(flashlight <= distf_coeff + 10.0*blurf && flashlight >= distf_coeff + 9.0*blurf) blurf = (1.0 - (flashlight - distf_coeff - 9.0 * blurf)*_blur)/2.0 + 1.0;
				else blurf = 1.0;
				distf = min(distf,dist_max);
				distf/= dist_max;
				distf = 1.0 - distf;
				float rf = max(distf,0.0)*blurf;
				_r += 0.1*rf;
				}
		}
	}
	vec4 env1, env2;
	if(z_normal > 0.0) {
		env1 = texture2D(envMap, summary);
		env2 = texture2D(envMap2, summary);
	}
	else {
		env1 = vec4(vec3(0.5) * max(dot(normalize(l), vec3(0.0,0.0,1.0)),0.0), 1.0);
		env2 = texture2D(envMap2, summary);
	}
    color = (env1 * fresnel_output)*fresnel + (env2 * fresnel_output)*(1.0-fresnel) + deep_color*(r + _r)*(1.0 - fresnel_output) + vec4(1.2,1.2,1.0,1.0)*r2*fresnel_output;
    gl_FragColor = color;

    vec4 summar = texture2D(envMap2, summary);
    float summ = summar.x + summar.y + summar.z;
    if(eyePos.z > pos.z) {
		if(fr > start) {
			fr /= end;
			fr = min(fr,0.5);
			gl_FragColor = gl_FragColor*(1.0 - fr) + atmo_summary*fr;
		}
    }
	gl_FragColor.w = 1.0;
	r -= 0.07;
	waves = abs(min(ter_hei*100.0 - pos.z,0.0));

	if(waves < 1.1 && z_normal > 0.0) {
		float transp = pow(max((waves - 0.1),0.0),2.0);
		gl_FragColor.w *= transp;
	}
}