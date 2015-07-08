#version 130
#extension GL_EXT_gpu_shader4 : enable
#pragma optionNV(fastmath on)
#pragma optionNV(fastprecision on)
#pragma optionNV(ifcvt none)
#pragma optionNV(inline all)
#pragma optionNV(strict on)
#pragma optionNV(unroll all)
in vec3 pos,l,n,v,h;
in vec2 texCoord;
uniform vec3 eyePos;
uniform vec3 eyeNorm;
uniform float time;
uniform int flash;
uniform float Width;
uniform float Height;
uniform vec3 lightPos;
uniform sampler2D caustics;
uniform int reflect;
in vec2 offset;
in vec3 et;
in vec3 lt;
uniform vec3 atmosphere;
/*
matList:
0 - beach1 (sand)
1 - mountain1 (stone)
2 - snow1 (snow)
3 - grass1 (grass)
4 - mask1
5 - mask2
*/
uniform sampler2D diffuseSamplers[16];
uniform sampler2D heightSamplers[16];
uniform sampler2D normalSamplers[16];
uniform sampler2D specularSamplers[16];

uniform float bumpVal[16];
uniform float parallaxOffset[16];
uniform float specVal[16];
uniform float specPow[16];
uniform int ambientType[16];

uniform int _useDiffuse[16];
uniform int _useHeight[16];
uniform int _useNormal[16];
uniform int _useSpecular[16];
uniform vec3 farColor[16];
uniform float farDist[16];

uniform int materialNum;

const float deltaBlend = 5.0;
const float grassStart = 6.0;
const float snowStart = 250.0;
const float snowConcentrate = 150.0; // промежуток высоты, необходимый для того, чтобы снег достиг максимальной концентрации
const float farBlendingPart = 0.3; //коэффициент исчезновения текстура -> больше коэффициент - плавнее исчезает текстура
const vec4 zeroVec = vec4(0.0);
const vec3 zAxis = vec3(0.0, 0.0, 1.0);
const vec4 sunLight = vec4(1.0);


float flashLight = 0.0; 
float fr;
const vec4 flashLightColor = vec4(1.0);
out vec4 FragColor;
  

vec3 hcount(vec3 l, vec3 v) {
	vec3 c = l + v;
    float _module = sqrt(c.x*c.x + c.y*c.y + c.z*c.z);
    return (l + v) / _module;
}
float dist(vec2 a, vec2 b) {
	vec2 c = a - b;
	return sqrt(c.x*c.x + c.y*c.y);
} 
float dist(vec3 a, vec3 b) {
	vec3 c = a - b;
	return sqrt(c.x*c.x + c.y*c.y + c.z*c.z);
}  
float module(vec3 a, vec3 b) {
    vec3 c = a - b;
    return sqrt(c.x*c.x + c.y*c.y + c.z*c.z);
} 

vec2 paralax(vec2 T, in sampler2D u_heightTexture, float parallaxScale) {
   vec3 V = et;
   const float minLayers = 10;
   const float maxLayers = 15;
   float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0, 0, 1), V)));

   // height of each layer
   float layerHeight = 1.0 / numLayers;
   // current depth of the layer
   float curLayerHeight = 0;
   // shift of texture coordinates for each layer
   vec2 dtex = parallaxScale * V.xy / V.z / numLayers;

   // current texture coordinates
   vec2 currentTextureCoords = T;

   // depth from heightmap
   float heightFromTexture = texture2D(u_heightTexture, currentTextureCoords).r;

   // while point is above the surface
   int steps = 0;
   while(heightFromTexture > curLayerHeight) 
   {
	if(steps > 10) break;
      // to the next layer
      curLayerHeight += layerHeight; 
      // shift of texture coordinates
      currentTextureCoords -= dtex;
      // new depth from heightmap
      heightFromTexture = texture2D(u_heightTexture, currentTextureCoords).r;
	  steps++;
   }

   ///////////////////////////////////////////////////////////
	vec2 texStep = parallaxScale * lt.xy / lt.z / numLayers;
   // previous texture coordinates
   vec2 prevTCoords = currentTextureCoords + texStep;

   // heights for linear interpolation
   float nextH	= heightFromTexture - curLayerHeight;
   float prevH	= texture2D(u_heightTexture, prevTCoords).r
                           - curLayerHeight + layerHeight;

   // proportions for linear interpolation
   float weight = nextH / (nextH - prevH);

   // interpolation of texture coordinates
   vec2 finalTexCoords = prevTCoords * weight + currentTextureCoords * (1.0-weight);

   // interpolation of depth values
   float parallaxHeight = curLayerHeight + prevH * weight + nextH * (1.0 - weight);

   // return result
   return finalTexCoords;
}

vec2 getTexCoord(int samplerNum, vec2 texCoord) {
	if(_useHeight[samplerNum] == 1) texCoord = paralax(texCoord, heightSamplers[samplerNum], parallaxOffset[samplerNum]);
	return texCoord;
}

vec4 getMaterial(int samplerNum, vec2 texCoord) {
	vec4 color = zeroVec;
	if(_useDiffuse[samplerNum] == 1) {
		vec4 diffuseFarColor = vec4(farColor[samplerNum], 1.0);
		float diffuseFarDist = farDist[samplerNum];
		if(fr > diffuseFarDist) color = diffuseFarColor;
		else {
			color = texture2D(diffuseSamplers[samplerNum], texCoord);
			float diffuseFarDelta = diffuseFarDist * farBlendingPart;
			float diffuseFarDistLow = diffuseFarDist - diffuseFarDelta;
			if(fr > diffuseFarDistLow) {
				color = mix(color, diffuseFarColor, (fr - diffuseFarDistLow) / diffuseFarDelta);
			}
		}
	}
	return color;
}

vec4 getLighting(int samplerNum, vec2 texCoord, vec3 light, vec4 lightColor) {
	float r = max(0.0, dot(light, n));

	if(_useNormal[samplerNum] == 1)
	{
		vec3 bump = normalize(texture2D(normalSamplers[samplerNum], texCoord).xyz * 2.0 - 1.0);
		float specular = 0.0f;
		if(_useSpecular[samplerNum] == 1) {
			vec3 R = hcount(normalize(et),normalize(lt));
			float specular_ammount = texture2D(specularSamplers[samplerNum], texCoord).x;
			specular = pow ( max ( dot ( bump, R), 0.0 ), specPow[samplerNum]) * specular_ammount * specVal[samplerNum];
		}
		
		if(r > 0.0) {
			float _r = bumpVal[samplerNum] * max(0.0, dot(lt, bump)) + r * (1.0 - bumpVal[samplerNum]) + specular;
			if(r < 0.1) {
				r = mix(r, _r, r * 10.0);
			}
			else r = _r;
		}
		
	}
	return r * lightColor + flashLight * flashLightColor;
}

void main(void) {
   float lambert = max(min(dot(l,n), 1.0), 0.0);
   fr = module(eyePos,pos);
   float start = 0.0;
   float end = 5000.0;
   vec4 summar = vec4(atmosphere,1.0);
   float grass_coeff = max(min(dot(zAxis, n), 1.0), 0.0);
   if(reflect == 1) grass_coeff -= grass_coeff;

   vec3 r0 = normalize (hcount(l,v));
   float r2  = pow ( max ( dot ( n, r0), 0.0 ), 5.0);
   vec4 color = zeroVec, _grass = zeroVec, _snow = zeroVec, _mountain = zeroVec, _shore = zeroVec;
   vec4 atmosphereAmb = vec4(atmosphere * 0.3 * (1.0 - lambert), 1.0);
   float deep_blend = 0.0;
   
   //init flashlight
   
	if(flash == 1) {
		vec3 Normal = normalize(pos - eyePos);
		float flashlight = dot(normalize(eyeNorm),Normal);
		float distf = fr;
		float diffuse = 1.0 - min(distf,5.0) * 0.2;
		flashLight = diffuse;
		float dist_max = 50.0;
		if(distf < dist_max) {
			float distf_coeff = distf /(dist_max * 100.0) * 7.0 + 0.9;
			if(flashlight > distf_coeff) {
				float blurf = (1.0 - distf_coeff) * 0.1;
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
				flashLight += 0.2*rf;
			}
		}
	}
	
   //init global materials
	vec4 lighting;
   if(pos.z > grassStart - deltaBlend) { //init mountain
		if(fr > 110.0) {
			lighting = getLighting(1, texCoord * 0.1, l, sunLight);
			if(ambientType[1] == 1) lighting += atmosphereAmb;
			_mountain = getMaterial(1, texCoord * 0.1) * lighting;
		}
		else {
			lighting = getLighting(1, texCoord, l, sunLight);
			if(ambientType[1] == 1) lighting += atmosphereAmb;
			_mountain = getMaterial(1, texCoord) * lighting;
			if(fr > 10.0) {
				float blend = max((fr - 10.0) * 0.01, 0.01);
				vec4 lightingOut = getLighting(1, texCoord * 0.1, l, sunLight);
				if(ambientType[1] == 1) lightingOut += atmosphereAmb;
				_mountain = mix(_mountain, getMaterial(1, texCoord * 0.1) * lightingOut, blend);
			}
		}
   }
   if(pos.z < grassStart + deltaBlend) { //init shore 
		vec2 shoreTex = texCoord * 1.5;
		lighting = getLighting(0, shoreTex, l, sunLight);
		if(ambientType[0] == 1) lighting += atmosphereAmb;
		shoreTex = getTexCoord(0, shoreTex);
		_shore = getMaterial(0, shoreTex) * lighting;
   }
   
   // calc color and init local materials
   if(pos.z >= snowStart) { //snow and mountain
	   
	    //init snow
		if(fr > 110.0) {
			lighting = getLighting(2, texCoord * 0.1, l, sunLight);
		}
		else {
			lighting = getLighting(2, texCoord, l, sunLight);
			if(fr > 10.0) {
				float blend = max((fr - 10.0) * 0.01, 0.01);
				vec4 lightingOut = getLighting(2, texCoord * 0.1, l, sunLight);
				lighting = mix(lighting, lightingOut, blend);
			}
		}
		if(ambientType[2] == 1) lighting += atmosphereAmb;
		_snow = getMaterial(2, texCoord) * lighting;
		
		//
		float snowOffset = max(1.0 - (pos.z - snowStart) / snowConcentrate, 0.0);
		float maskBlend = max(getMaterial(4, texCoord * 0.005).x - snowOffset, 0.0);
		float normalBlend = min(max((grass_coeff - 0.5) * 5.0, 0.0), 1.0);
		maskBlend = max(min(maskBlend * 11.0 - 5.0, 1.0), 0.0);
		float snowBlend = maskBlend * normalBlend;
		color = mix(_mountain, _snow, snowBlend);
		
		
   }
   else if(pos.z >= grassStart) { //mountain, grass and shore
	   
	   //init grass
		vec4 grassTex = getMaterial(3, texCoord * 0.5);
		lighting = getLighting(3, texCoord * 2.0, l, sunLight);
		if(fr < 200.0) {
			float blend = fr * 0.005; // 1.0 -> 0.0 // 200 -> 0
			grassTex = mix(getMaterial(3, texCoord * 2.0), grassTex, blend);
		}
		_grass = grassTex;
		if(ambientType[3] == 1) lighting += atmosphereAmb;
		_grass *= lighting;
		///
		
		if(grass_coeff < 0.8) color = _mountain;
		else if(grass_coeff > 0.9) color = _grass;
		else {
			float blend = (grass_coeff - 0.8) * 10.0;
			color =  mix(_mountain, _grass, blend);
		}
		if(pos.z >= snowStart - (deltaBlend * 20.0)) { //blend with snow
		    float delta = -2.0 * ((snowStart - pos.z) / (deltaBlend * 20.0) - 0.5);
		    float blend = min(max(getMaterial(5, texCoord * 0.25).x - delta, 0.0), 1.0); 
		    color = mix(_mountain, color, blend);
		}
		else if(pos.z < grassStart + deltaBlend) { //blend with sand
			float delta = 2.0 * ((pos.z - grassStart) / deltaBlend - 0.5);
			float blend = min(max(getMaterial(5, texCoord * 0.25).x - delta, 0.0), 1.0); 
			color = mix(color, _shore, blend);
		}
	}
	else { //shore only
		   vec4 deep_color = vec4(atmosphere * 0.5,0.0);
		   if(pos.z > 0.0) deep_blend = 0.0;
		   else {
		        deep_blend = min(abs(pos.z) * 0.03, 1.0);
		   }
		   color = mix(_shore, deep_color, deep_blend);
	}
	float transparency = color.w;

	FragColor = color;

	FragColor.w = transparency;
	
	vec2 offset2;
	offset2.x = offset.y;
	offset2.y = offset.x;
	if(fr < 200.0 && pos.z < 0.0) {
		float tmp_blend = 1.0;
		if(fr > 100.0) {
			tmp_blend -= (fr - 100.0)* 0.01;
		}
		float fr_caustics = min(max( dot(normalize(l), n)-0.2, 0.0 ),0.5);
		if(pos.z >= -1.0) fr_caustics *= (0.0 - pos.z);
		vec4 caust = (texture2D(caustics,(texCoord * 4.0 + vec2(-time+0.41,-time+0.29))/4.3) + texture2D(caustics,(texCoord * 4.0 + vec2(time,0.0))/3.1))*fr_caustics * 0.5;
		caust = vec4(vec3((caust.r + caust.g + caust.b) * 0.5), 1.0);
		caust = caust*caust*4.0;
		if(pos.z < 1.0) FragColor += caust*(deep_blend * 0.5 + 0.5)*tmp_blend;
	}

	if(fr < end) {
	    fr /= end;
	    FragColor = FragColor*(1.0 - fr) + summar*fr;
    }
	else FragColor = summar;
	FragColor.w = transparency;	
}
