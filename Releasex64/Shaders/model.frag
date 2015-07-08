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
varying vec3 model_pos;
uniform vec3 eyePos; 
uniform int flash;
uniform float max_dist = 5000.0;
uniform vec4 distance_col = vec4(0.2,0.2,0.0,1.0);
uniform int shadowing;
uniform vec3 eyeNorm;
uniform float Width;
uniform float Height;
uniform sampler2D groundMap;
uniform sampler2D normalMap;
uniform sampler2D heightMap;
uniform sampler2D specularMap;
uniform int specEnabled = 0;
uniform vec3 atmosphere;
uniform float shadowDist = 100.0; 
uniform int reverse = 1;
varying	vec3 et;
varying	vec3 lt;
varying vec3 ht;
float MAX_BUMP_DIST = 2000.0;
 
vec3 hcount(vec3 l, vec3 v) {
    float module = sqrt(pow(l.x+v.x,2.0) + pow(l.y+v.y,2.0) + pow(l.z+v.z,2.0));
    return (l+v)/module;
}
float dist(vec3 a, vec3 b) {
	vec3 c = a - b;
	return sqrt(c.x*c.x + c.y*c.y + c.z*c.z);
}  

vec2 paralax(vec2 T, in sampler2D u_heightTexture) {
   float parallaxScale = 0.01;
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

void main(void) {
	vec2 tc = texCoord;
	float ground_blend = min(max(1.0 / abs(model_pos.z/2.0),0.0),1.0);
	if(reverse == 1) tc.y *= -1.0;
	float fr = dist(eyePos,pos);
	float start = 0.0;
	float end = 5000.0;
	vec4 summar = (vec4(atmosphere,1.0) + vec4(min(max( dot(normalize(l), vec3(0.0,0.0,1.0)), 0.1 ),0.5)))/2.0;
	float bump_ammount = 1.0;
	vec2 tex = tc;
	float specular = 0.0;
	vec3 bump;
	vec2 offsetTex = tex;
	if(bump_ammount != 0.0)
	{
		//tex = paralax(tex, heightMap);
		//tex = tex * sqrt(bump_ammount) + tc*(1.0-sqrt(bump_ammount));
		float specular_ammount = 1.0;
		if(specEnabled == 1) specular_ammount = texture2D(specularMap, tex).x;
		bump = normalize(texture2D(normalMap, tex).xyz * 2.0 - 1.0);
		vec3 R = hcount(normalize(et),normalize(lt));
		specular = pow ( max ( dot ( bump, R), 0.0 ), 9.0)*sqrt(bump_ammount)*specular_ammount;
	}
   float  r;
   float limit = min(max(0.05,dot(l, vec3(0,0,1))),1.0);
   if(bump_ammount != 0.0) r =  min(max(0.2*limit,dot(lt, bump)),0.6*limit*1.5);
   else r =  min(max(0.2*limit,dot(l, n)),0.6*limit*1.5);
   if(dot(l, n) > 0.0) r += specular*limit;
   
   
   vec4 color = distance_col;
   float transp = 1.0;
   float tmp_fren = 0.0;
   color *=  tmp_fren;
   transp *=  tmp_fren;
   vec4 tmp_color = texture2D(model,tc); 
   color += tmp_color*(1.0 - tmp_fren);
   transp += tmp_color.w*(1.0 - tmp_fren);

   
   gl_FragColor = color*r;

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
	if(fr < end) {
	    fr /= end;
	    gl_FragColor = gl_FragColor*(1.0 - fr) + summar*fr;
    }
	gl_FragColor.w = transp;
}