#version 120
varying vec3 pos,l,n,v,h;
varying vec2 texCoord;
uniform sampler2D model;
uniform sampler2D model2;
uniform vec3 eyePos;
uniform sampler2D _shadow0;  
uniform sampler2D _shadow1;  
uniform sampler2D _shadow2;  
uniform sampler2D _shadow3;  
uniform sampler2D _shadow4;  
uniform sampler2D _shadow5; 
uniform sampler2D _shadow6; 
varying vec4 posit0;
varying vec4 posit1;
varying vec4 posit2;
varying vec4 posit3;
varying vec4 posit4;
varying vec4 posit5;
varying vec4 posit6;
uniform int flash;
uniform float tx;
uniform int shadowing;
uniform vec3 eyeNorm;
uniform float shadowDist = 100.0; 
uniform float Width;
uniform float Height;
uniform float hue;
varying	vec3 et;
varying	vec3 lt;
float MAX_BUMP_DIST = 20.0;
 
 vec4 convert(vec4 color) {
   vec3 hsl;
   float min_rgb = min(min(color.r, color.g), color.b);
   float max_rgb = max(max(color.r, color.g), color.b);

   hsl.b = (min_rgb + max_rgb) / 2.0;//lightness
   if(hsl.b > 0.5) {//saturation
		hsl.g = (max_rgb - min_rgb)/(2.0 - max_rgb - min_rgb);
   }
   else {
		hsl.g = (max_rgb - min_rgb)/(max_rgb + min_rgb);
   }

   if(max_rgb == color.r) {
		hsl.r = (color.g - color.b)/(max_rgb - min_rgb);
   }
   else if(max_rgb == color.g) {
		hsl.r = 2.0 + (color.b - color.r)/(max_rgb - min_rgb);
   }
   else {
		hsl.r = 4.0 + (color.r - color.g)/(max_rgb - min_rgb);
   }

   
   

   hsl.r /= 6.0;
   hsl.r += hue;
   if(hsl.r > 1.0) hsl.r -= 1.0;
   float temporary_1, temporary_2, temporary_r, temporary_g, temporary_b;

   if(hsl.b < 0.5) {
		temporary_1 = hsl.b * (1.0 + hsl.g);
   }
   else {
		temporary_1 = hsl.b + hsl.g - hsl.b * hsl.g;
   }

   temporary_2 = 2.0 * hsl.b - temporary_1;

   temporary_r = hsl.r + 0.333;
   temporary_g = hsl.r;
   temporary_b = hsl.r - 0.333;

   if(temporary_r > 1.0) temporary_r -= 1.0;
   if(temporary_b < 0.0) temporary_b += 1.0;

   if((6.0 * temporary_r) < 1.0) {
		color.r = temporary_2 + (temporary_1 - temporary_2) *6.0 * temporary_r;
   }
   else if((2.0 * temporary_r) < 1.0) {
		color.r = temporary_1;
   }
   else if((3.0 * temporary_r) < 2.0) {
		color.r = temporary_2 + (temporary_1 - temporary_2) * ( 0.666 - temporary_r) * 6.0;
   }
   else color.r = temporary_2;
   ///////////////////
   if((6.0 * temporary_g) < 1.0) {
		color.g = temporary_2 + (temporary_1 - temporary_2) *6.0 * temporary_g;
   }
   else if((2.0 * temporary_g) < 1.0) {
		color.g = temporary_1;
   }
   else if((3.0 * temporary_g) < 2.0) {
		color.g = temporary_2 + (temporary_1 - temporary_2) * ( 0.666 - temporary_g) * 6.0;
   }
   else color.g = temporary_2;
   ////////////////
   if((6.0 * temporary_b) < 1.0) {
		color.b = temporary_2 + (temporary_1 - temporary_2) *6.0 * temporary_b;
   }
   else if((2.0 * temporary_b) < 1.0) {
		color.b = temporary_1;
   }
   else if((3.0 * temporary_b) < 2.0) {
		color.b = temporary_2 + (temporary_1 - temporary_2) * ( 0.666 - temporary_b) * 6.0;
   }
   else color.b = temporary_2;

   return color;
}

vec3 hcount(vec3 l, vec3 v) {
    float module = sqrt(pow(l.x+v.x,2.0) + pow(l.y+v.y,2.0) + pow(l.z+v.z,2.0));
    return (l+v)/module;
}
float dist(vec3 a, vec3 b) {
	vec3 c = a - b;
	return sqrt(c.x*c.x + c.y*c.y + c.z*c.z);
}  
float lin_shadow(sampler2D _sh_tex, float x,float y,vec2 tex_coor,vec3 shad_coor) {
        float z = 0.0;
	vec4 s;
	if(x == 0.0 && y == 0.0) return 1.0;
	vec2 real_coor = vec2(x,y);
	vec2 tex_offset = vec2(1.0/Width,1.0/Height);
	for(float i = -tex_offset.x; i <= tex_offset.x; i+=tex_offset.x) {
		for(float j = -tex_offset.y; j <= tex_offset.y; j+=tex_offset.y) {
			float shadow_blur = 2.0*sqrt(2.0) - abs(i-x)*abs(j-y)*Width*Height;
			float tmp_res = texture2D(_sh_tex,tex_coor + vec2(i,j)).x + 0.00001;
			if(i == 0.0 && j == 0.0) s.x = tmp_res;
			if(i == tex_offset.x && j == 0.0) s.y = tmp_res;
			if(i == tex_offset.x && j == tex_offset.y) s.z = tmp_res;
			if(i == 0.0 && j == tex_offset.y) s.w = tmp_res;
			if(shad_coor.z >= tmp_res) z += shadow_blur/(2.0*sqrt(2.0));
		}
	}
	z /= 9.0;
	float _z = 0.0;
	if(shad_coor.z >= s.x) {
		_z += (tex_offset.x-x)*(tex_offset.y-y)*Width*Height;
    }
    if(shad_coor.z >= s.y) {
		_z += (x)*(tex_offset.y-y)*Width*Height;
	}
	if(shad_coor.z >= s.z) {
		_z += (x)*(y)*Width*Height;	
	}
	if(shad_coor.z >= s.w) {
		_z += (tex_offset.x-x)*(y)*Width*Height;
	}
	_z = max(min(_z,1.0),0.0);
	return (_z + z)/2.0;
}
void main(void) {

	float fr = dist(eyePos,pos);
	float start = 0.0;
	float end = 5000.0;
	float summar = min(max( dot(normalize(l), vec3(0.0,0.0,1.0)), 0.1 ),0.5);
	if(fr > end) { 
   		gl_FragColor = vec4(summar);
   		return;
	} 
	float bump_ammount =0;// 1.0 - (min(dist(eyePos,pos),MAX_BUMP_DIST))/MAX_BUMP_DIST;
	vec2 tex = texCoord;
   float specular = 0.0;
   float  r;
   float limit = min(max(0.05,dot(l, vec3(0,0,1))),1.0);
   r =  min(max(0.2*limit,dot(l, vec3(0,0,1))),0.6*limit*1.5) + specular;
   vec2 tc = texCoord;
   vec4 color;
   if(tx < 0.25) color = texture2D(model,tc);
   else color = texture2D(model2,tc);
   
   color = convert(color);

   gl_FragColor = vec4(color.x*r,color.y*0.5 + color.y*r*0.5, color.z*0.5 + color.z*0.5*r,color.w);
   float pmax = 1.0;
   if(shadowing == 1 && fr < shadowDist) {
		int shad_flag = 0;
			vec3 shad_coor;
		if(shad_flag == 0)  {
			shad_coor = (posit0.xyz / posit0.w) * 0.5 + 0.5;
			if(shad_coor.x <= 0.1 || shad_coor.x >= 0.9 || shad_coor.y <= 0.1 || shad_coor.y >= 0.9 ) shad_flag++;
		}
		if(shad_flag == 1)  {
			shad_coor = (posit1.xyz / posit1.w) * 0.5 + 0.5;
			if(shad_coor.x <= 0.1 || shad_coor.x >= 0.9 || shad_coor.y <= 0.1 || shad_coor.y >= 0.9 ) shad_flag++;
		}
		vec2 sun_c = shad_coor.xy;
	
		float w_value = Width/2048.0; 
		float z_value = (1.0 - w_value)/2.0;
	
		sun_c.x = sun_c.x*w_value + z_value;
	
		w_value = Height/2048.0; 
		z_value = (1.0 - w_value)/2.0;
	
		sun_c.y = sun_c.y*w_value + z_value;
	
		int _int_x = int(sun_c.x * Width);
		int _int_y = int(sun_c.y * Height);
		
		float int_x = sun_c.x - float(_int_x)/Width;
		float int_y = sun_c.y - float(_int_y)/Height;
	
		
		if(shad_flag == 0) pmax = (1.0 - lin_shadow(_shadow0,int_x,int_y,sun_c.xy - vec2(int_x,int_y),shad_coor))/2.0 + 0.5;
		else if(shad_flag == 1) pmax = (1.0 - lin_shadow(_shadow1,int_x,int_y,sun_c.xy - vec2(int_x,int_y),shad_coor))/2.0 + 0.5;
		else pmax = 1.0;
	
		gl_FragColor.xyz*=pmax;
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
		gl_FragColor += color*_r;
	}
	if(fr > start) {
		fr = 1.0/(1.0 - (fr/end));
		fr = max(fr,0.5);
		gl_FragColor.x = summar + (gl_FragColor.x - summar)/fr;
		gl_FragColor.y = summar + (gl_FragColor.y - summar)/fr;
		gl_FragColor.z = summar + (gl_FragColor.z - summar)/fr;
    }
}