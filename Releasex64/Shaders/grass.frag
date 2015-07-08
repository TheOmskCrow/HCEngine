#version 120
#pragma optionNV(fastmath on)
#pragma optionNV(fastprecision on)
#pragma optionNV(ifcvt none)
#pragma optionNV(inline all)
#pragma optionNV(strict on)
#pragma optionNV(unroll all)
varying vec3 pos,l,n,v,h;
varying vec2 texCoord;
uniform sampler2D model;
uniform vec3 eyePos; 
uniform int flash;
uniform int shadowing;
uniform vec3 eyeNorm;
uniform float Width;
uniform vec3 atmosphere;
uniform float dstbl = 1.0;
uniform float Height;
uniform sampler2D normalMap;
uniform sampler2D heightMap;
uniform float shadowDist = 100.0; 
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

void main(void) {
	float fr = dist(eyePos,pos);
	float start = 0.0;
	float end = 5000.0;
	vec4 summar = (vec4(atmosphere,1.0) + vec4(min(max( dot(normalize(l), vec3(0.0,0.0,1.0)), 0.1 ),0.5)))/2.0;
	float bump_ammount =0;// 1.0 - (min(dist(eyePos,pos),MAX_BUMP_DIST))/MAX_BUMP_DIST;
	vec2 tex = texCoord;
	
   float specular = 0.0;
   float  r;
   vec4 color;
   color = texture2D(model,vec2(texCoord.x,1.0 - texCoord.y));
   float limit = min(max(0.05,dot(l, vec3(0,0,1))),1.0);
   if(limit > 0.05) {
		r = max(min(dot(l,n),limit),0.0);
		if(r > 0.0);
		else {
			r = max(min(dot(l,-n),limit),0.0);
		}
   }
   else {
		r = limit;
   }
   vec2 tc = texCoord;
   gl_FragColor = vec4(color.xyz*r,color.w);
   if(color.w < 0.5) gl_FragColor.w = 0.0;
   else gl_FragColor.w = 1.0;
   if(dstbl > 1.0) {
		float dist_blur = 1.0 - ((min(max(fr,450.0),500.0)-450.0)/50.0);
		gl_FragColor.w *= dist_blur;
   }

   

   
	if(flash == 1) {
		vec3 Normal = normalize(pos - eyePos);
		float flashlight = dot(normalize(eyeNorm),Normal);
		float distf = dist(pos,eyePos);
		float diffuse = 1.0 - min(distf,5.0)/5.0;
		float _r =  0.6*0.5*diffuse;
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
		gl_FragColor.xyz += color.xyz*_r;
	}
	float tmp_trans = gl_FragColor.w;
	if(fr < end) {
	    fr /= end;
	    gl_FragColor = gl_FragColor*(1.0 - fr) + summar*fr;
    }
	gl_FragColor.w = tmp_trans;
}