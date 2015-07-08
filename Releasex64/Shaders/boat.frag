#version 120
varying vec3 pos,l,n,v,h;
varying vec2 texCoord;
uniform sampler2D boat;
uniform sampler2D envMap;
uniform sampler2D noiseMap;
uniform int transparent = 0;
uniform float level = 1.0;
uniform vec3 eyePos;
uniform int float_coord = 0;
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
uniform vec3 eyeNorm;
uniform float Width;
uniform float rnd_x;
uniform float rnd_y;
uniform float Height;
uniform float value = 1.0;
 
float dist(vec3 a, vec3 b) {
	vec3 c = a - b;
	return sqrt(c.x*c.x + c.y*c.y + c.z*c.z);
}  
float lin_shadow(sampler2D _sh_tex, float x,float y,vec2 tex_coor,vec3 shad_coor) {
        float z = 0.0;
	vec4 s;
	if(x == 0.0 && y == 0.0) return 1.0;
	vec2 real_coor = vec2(x,y);
//	if(tex_coor.x < 0.1 || tex_coor.x > 0.9 || tex_coor.y < 0.1 || tex_coor.y > 0.9) return 0.0;
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
//	return z;
/*	s.x = texture2D(_sh_tex,tex_coor).x + 0.00001;
	s.y = texture2D(_sh_tex,tex_coor+vec2(tex_offset.x,0.0)).x + 0.00001;
	s.z = texture2D(_sh_tex,tex_coor+tex_offset).x + 0.00001;
	s.w = texture2D(_sh_tex,tex_coor+vec2(0.0,tex_offset.y)).x + 0.00001;*/
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
//    return z;
}
void main(void) {
	if(level != 0.0) {
   float  r;
   float limit = min(max(0.0,dot(l, vec3(0,0,1))),1.0);
   r =  min(max(0.2*limit,dot(l, n)),0.6*limit*1.5)*value;
   vec2 tc = texCoord;
   vec4 color = texture2D(boat,tc);
   
   gl_FragColor = color*r;

    int shad_flag = 0;
      vec3 shad_coor;
	if(shad_flag == 0) shad_coor = (posit0.xyz / posit0.w) * 0.5 + 0.5;
	if(shad_coor.x <= 0.1 || shad_coor.x >= 0.9 || shad_coor.y <= 0.1 || shad_coor.y >= 0.9 ) shad_flag++;
	if(shad_flag == 1) shad_coor = (posit1.xyz / posit1.w) * 0.5 + 0.5;
	if(shad_coor.x <= 0.1 || shad_coor.x >= 0.9 || shad_coor.y <= 0.1 || shad_coor.y >= 0.9 ) shad_flag++;
	if(shad_flag == 2) shad_coor = (posit2.xyz / posit2.w) * 0.5 + 0.5;
	if(shad_coor.x <= 0.1 || shad_coor.x >= 0.9 || shad_coor.y <= 0.1 || shad_coor.y >= 0.9 ) shad_flag++;
	if(shad_flag == 3) shad_coor = (posit3.xyz / posit3.w) * 0.5 + 0.5;
	if(shad_coor.x <= 0.1 || shad_coor.x >= 0.9 || shad_coor.y <= 0.1 || shad_coor.y >= 0.9 ) shad_flag++;
	if(shad_flag == 4) shad_coor = (posit4.xyz / posit4.w) * 0.5 + 0.5;
	if(shad_coor.x <= 0.1 || shad_coor.x >= 0.9 || shad_coor.y <= 0.1 || shad_coor.y >= 0.9 ) shad_flag++;
	if(shad_flag == 5) shad_coor = (posit5.xyz / posit5.w) * 0.5 + 0.5;
	if(shad_coor.x <= 0.1 || shad_coor.x >= 0.9 || shad_coor.y <= 0.1 || shad_coor.y >= 0.9 ) shad_flag++;
	if(shad_flag == 6) shad_coor = (posit6.xyz / posit6.w) * 0.5 + 0.5;
	if(shad_coor.x <= 0.1 || shad_coor.x >= 0.9 || shad_coor.y <= 0.1 || shad_coor.y >= 0.9 ) shad_flag++;
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

	float pmax;
	if(shad_flag == 0) pmax = (1.0 - lin_shadow(_shadow0,int_x,int_y,sun_c.xy - vec2(int_x,int_y),shad_coor))/2.0 + 0.5;
	else if(shad_flag == 1) pmax = (1.0 - lin_shadow(_shadow1,int_x,int_y,sun_c.xy - vec2(int_x,int_y),shad_coor))/2.0 + 0.5;
	else if(shad_flag == 2) pmax = (1.0 - lin_shadow(_shadow2,int_x,int_y,sun_c.xy - vec2(int_x,int_y),shad_coor))/2.0 + 0.5;
	else if(shad_flag == 3) pmax = (1.0 - lin_shadow(_shadow3,int_x,int_y,sun_c.xy - vec2(int_x,int_y),shad_coor))/2.0 + 0.5;
	else if(shad_flag == 4) pmax = (1.0 - lin_shadow(_shadow4,int_x,int_y,sun_c.xy - vec2(int_x,int_y),shad_coor))/2.0 + 0.5;
	else if(shad_flag == 5) pmax = (1.0 - lin_shadow(_shadow5,int_x,int_y,sun_c.xy - vec2(int_x,int_y),shad_coor))/2.0 + 0.5;
	else if(shad_flag == 6) pmax = (1.0 - lin_shadow(_shadow6,int_x,int_y,sun_c.xy - vec2(int_x,int_y),shad_coor))/2.0 + 0.5;
	else pmax = 1.0;

	gl_FragColor.xyz*=pmax;
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
	gl_FragColor *= level;
	}
	if(transparent == 1 || level != 1.0) {
		float noise_val = 0.5 - abs(0.5 - level);
		vec2 coor;
		vec4 ver = vec4(pos,1.0);
		vec4 pos2 = gl_ModelViewProjectionMatrix * ver;
		coor.x = (pos2.x / pos2.w) + 1.0;
		coor.y = (pos2.y / pos2.w) + 1.0;
		coor /= 2.0;
		vec2 offset;
		float distscale = 0.05;
		offset.x = -n.x*distscale;//+0.055;
		offset.y =  -n.y*distscale;//+0.055;
		float offset_val_x = 1.0;
		float offset_val_y = 1.0;
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
		vec4 noise_res = texture2D(noiseMap,summary+vec2(rnd_x,rnd_y));
		if(noise_res.x > 0.5) noise_res *= vec4(rnd_x,0.0,rnd_y,0.0);
		gl_FragColor += texture2D(envMap,summary)*(1.0 - level) + noise_res*noise_val*0.4;
	}
}