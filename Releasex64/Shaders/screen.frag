#version 130
#extension GL_EXT_gpu_shader4 : enable
#pragma optionNV(fastmath on)
#pragma optionNV(fastprecision on)
#pragma optionNV(ifcvt none)
#pragma optionNV(inline all)
#pragma optionNV(strict on)
#pragma optionNV(unroll all)


in vec3 pos;
in vec2 _texCoord;
uniform bool AA;
uniform vec3 eyeNorm;
uniform vec3 eyePos;
uniform bool godRays;
uniform bool bloom_;
uniform bool toneMapping;
uniform bool depthOfField;
uniform bool filmGrain;
uniform sampler2D tex1;
uniform sampler2D abberation;
uniform sampler2D tex2;
uniform sampler2D spec1;
uniform sampler2D spec2;
uniform sampler2D screen_noise;
uniform int num;
uniform float brightness;
uniform float flash;
uniform float blur;
uniform float rnd_x;
uniform float rnd_y;
uniform vec4 sun;
uniform vec4 sun2;
uniform float Width;
uniform float Height;
uniform int frames;
vec4 test[13];
float x_off = 1.0/Width;
float y_off = 1.0/Height;
float center_depth;
const float AA_sensivity = 0.05;
uniform float start_coord[2];
uniform float time;
uniform float end_coord[2];
uniform sampler2D font;
uniform sampler2D g_blur;
uniform sampler2D exposure;
uniform sampler2D D_O_F;
uniform sampler2D D_O_F_prev;
uniform int text_size;
uniform mat4 mv;
uniform mat4 pr;
vec2 texCoord;
const float SSAOradius = 1.0;
const float SSAOdistScale = 1.0;
const float SSAObias = 1.0;
const vec4 AllOnes = vec4(1.0);
uniform float coeff;
uniform float coeff1;
out vec4 FragColor;

float Noise(float x, float y ,float z) {
    float n = 2.0*z + x + y * 57.0;
    n = float(int(( ((4.0*(n-z) * (n * 0.5 * (z + 15731.0) + 789721.0) + 43589.0)) / 0.0000093132))%1000);
    n *= 0.001;
    return n;
}
vec4 _abberation() {
	vec2 offset = vec2(x_off, y_off);
    if(texCoord.x <= x_off || texCoord.x >= 1.0 - x_off || texCoord.y <= y_off || texCoord.y >= 1.0 - y_off) {
        return vec4(vec3(0.0),1.0);
    }
	vec4 color = vec4(0.0);
	const float radius = 1.0;
    for(float i = -radius; i <= radius; i++) {
		for(float j = -radius; j <= radius; j++) {
			color += texture2D(abberation,texCoord + vec2(i * offset.x,j * offset.y));
		}
	}
	float right_ret = radius * 2.0 + 1.0;
    return color / (right_ret * right_ret);
}
float height(float x, float y) {
    x += 500.0;
    y += 500.0;
    float sum = 0.0;
    for(float k = 0.0; k < 2.0; k++) {
        sum += (1.0/ (k * k)) * abs(2.0 * Noise((k * k)*x,(k * k)*(frames * 0.04 + time),(k * k) * y)-1.0);
    }
    return sum;
}

bool check(int a, int b) {
	if(test[0].x - test[a].x > AA_sensivity || test[0].y - test[a].y > AA_sensivity || test[0].y - test[a].y > AA_sensivity) {
		if(test[0].x - test[b].x > AA_sensivity || test[0].y - test[b].y > AA_sensivity || test[0].y - test[b].y > AA_sensivity) {
			return true;
		}
		else return false;
	}
	else return false;
}


vec4 antialliasing(sampler2D tex)
{
	vec4 FragColor = vec4(0.0);
	vec2 frameBufSize;
	frameBufSize.x = Width;
	frameBufSize.x = Height;
	const float FXAA_SPAN_MAX = 4.0;
	const float FXAA_REDUCE_MUL = 1.0/4.0;
	const float FXAA_REDUCE_MIN = 1.0/32.0;
	vec3 rgbNW=texture2D(tex,texCoord+(vec2(-x_off,-y_off))).xyz;
	vec3 rgbNE=texture2D(tex,texCoord+(vec2(x_off,-y_off))).xyz;
	vec3 rgbSW=texture2D(tex,texCoord+(vec2(-x_off,y_off))).xyz;
	vec3 rgbSE=texture2D(tex,texCoord+(vec2(x_off,y_off))).xyz;
	vec3 rgbM=texture2D(tex,texCoord).xyz;
	vec3 luma=vec3(0.299, 0.587, 0.114);
	float lumaNW = dot(rgbNW, luma);
	float lumaNE = dot(rgbNE, luma);
	float lumaSW = dot(rgbSW, luma);
	float lumaSE = dot(rgbSE, luma);
	float lumaM = dot(rgbM, luma);
	float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW,lumaSE)));
	float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW,lumaSE)));
	vec2 dir;
	dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
	dir.y = ((lumaNW + lumaSW) - (lumaNE + lumaSE));
	float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL),FXAA_REDUCE_MIN);
	float rcpDirMin = 1.0/(min(abs(dir.x), abs(dir.y)) + dirReduce);
	dir = min(vec2( FXAA_SPAN_MAX, FXAA_SPAN_MAX),max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX),dir * rcpDirMin)) / frameBufSize;

	vec3 rgbA = 0.5 * texture2D(tex, texCoord.xy + dir * (0.33333 - 0.5)).xyz + 0.5 * texture2D(tex, texCoord.xy + dir * (0.66666 - 0.5)).xyz;

	vec3 rgbB = rgbA * .5 + .25 * texture2D(tex, texCoord.xy + dir * (- 0.5)).xyz + .25 * texture2D(tex, texCoord.xy + dir * (0.5)).xyz;
	float lumaB = dot(rgbB, luma);
	if((lumaB < lumaMin) || (lumaB > lumaMax)) {
		FragColor.xyz=rgbA;
	}
	else {
		FragColor.xyz=rgbB;
	}
	return FragColor;
}
vec4 bloom() {
	vec4 ret = texture2D(g_blur,texCoord) * 0.5;
	vec4 float_bloom = ret;
	float_bloom *= pow((dot(float_bloom, AllOnes)) * 0.333, 2.0)*2.0;//2 or 1
	ret += float_bloom;
	return ret; 
}

vec4 god_rays_1(vec4 sun_) {	 
	 if(dot(normalize(eyeNorm),normalize(sun_.xyz)) < 0.0) return vec4(0.0);
	 vec4 pos2 = mv*pr*sun_;
	
	 vec2 sun_c;
	 
	 if(pos2.z < 0.0) return vec4(0.0);
	 sun_c.x = (pos2.x / pos2.w) * 0.5 + 0.5;
     sun_c.y = (pos2.y / pos2.w) * 0.5 + 0.5;
/*
	 float w_value = Width/2048.0; 
	 float z_value = (1.0 - w_value) * 0.5;

	 sun_c.x = sun_c.x*w_value + z_value;

	 w_value = Height/2048.0; 
	 z_value = (1.0 - w_value) * 0.5;

	 sun_c.y = sun_c.y*w_value + z_value;
	 */
	 vec2 coor = sun_c;//screen light pos
     vec2 delta = (coor - texCoord);
	 float fres2 = min(sqrt(delta.x*delta.x + delta.y*delta.y),1.0);
	 delta *= coeff;
     vec2 new_texCoord = texCoord;
     vec3 color = vec3(0.0);
	 float blend = min(min(1.0 - coor.x, 1.0 - coor.y), min(coor.x, coor.y));
	 float gamma = 1.0;
     if(coor.x >= 1.0 || coor.x <= 0.0 || coor.y >= 1.0 || coor.y <= 0.0) return vec4(0.0);
	 else if(blend < 0.1) {
		 gamma *= blend * 10.0;
	 }
      //= sqrt(pow(abs(delta.x),2.0) + pow(abs(delta.y),2.0))*80.0;
     //gamma = 1.0 - min(1.0, gamma);
    // if(gamma == 0.0) return vec4(0.0);
     float fresnel = coeff1;
     vec3 sample;
	 for(int i = 0; i < 20; i++) {
		sample = texture2D(g_blur,new_texCoord).xyz * 3.0 - AllOnes.xyz;
		
	 	sample.x = max(0.0,sample.x); sample.y = max(0.0,sample.y); sample.z = max(0.0,sample.z);
     	color += sample * fresnel;//*(pow(1.0 - fres2,2.0));//or without 1.0 -
     	new_texCoord += delta;
     }
	 float lum = (color.x + color.y + color.z) / 3.0;
	 if(lum > 0.4) gamma *= 0.4 / lum;
     return vec4(vec3(color),1.0) * gamma;

}

vec4 add_text() {
	if(texCoord.x >= start_coord[0] && texCoord.x <= start_coord[0]+(end_coord[0]-start_coord[0]) * 0.01*text_size && texCoord.y >= start_coord[1]+(end_coord[0]-start_coord[0]) * 0.01*(100.0-text_size) && texCoord.y <= end_coord[1] ) {
		vec2 delta;
		delta.x = (texCoord.x - start_coord[0])/(end_coord[0] - start_coord[0]);
		delta.y = 1.0 - (texCoord.y - start_coord[1])/(end_coord[1] - start_coord[1]);
		return texture2D(font,delta);
	}
	return vec4(0.0);
}

void main(void) {
	texCoord = _texCoord;
	vec4 _bloom = bloom();
    vec4 color = vec4(0.0);
    if(godRays) color += god_rays_1(sun * 0.5 + sun2 * 0.5);
	color += _abberation();
	
	vec4 diffuse = vec4(0.0);
	if(eyePos.z <= 0.0) diffuse += _bloom * 2.0;
	else if(AA) diffuse += antialliasing(tex1);
	else diffuse += texture2D(tex1,texCoord);	
    
	FragColor = color;

    vec2 xyoffset = texCoord + vec2(rnd_x, rnd_y);

	FragColor += _bloom * _bloom;


	float r  = (max(0.0,min(1.0,dot(vec3(0.0,0.0,1.0),normalize(sun.xyz - pos.xyz)))));
	
	if(depthOfField && eyePos.z > 0.0) {
		float currentFocus = texture2D(D_O_F_prev,vec2(0.5,0.5)).x;
		float frag_focus = texture2D(D_O_F,texCoord).x;
	
		float blend = currentFocus/frag_focus;
		if(blend > 1.0) blend = 1.0 / blend;
		if(blend < 0.1) diffuse = texture2D(g_blur,texCoord);
		else if(blend >= 0.1 && blend <= 0.2) {
			blend -= 0.1;
			blend *= 10.0;
			diffuse = mix(texture2D(g_blur,texCoord), diffuse, blend);//FragColor * (1.0-blend) + bloom() * 2.0 * blend;
		}
	}
	FragColor += diffuse;
	if(filmGrain) FragColor += texture2D(screen_noise, xyoffset * 2.0) * 0.03;
	if(toneMapping) {
		vec4 lumin = vec4(0.3, 0.59, 0.11, 0.0);
		vec4 lum = textureLod(tex1,vec2(0.5,0.5),10.0);
		float _lum = dot(lum, lumin);
		float _exp = pow((1.0 - _lum) + 0.5, 0.5);
		FragColor *= _exp;
	}
}

