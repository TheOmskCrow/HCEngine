#version 150
#extension GL_EXT_gpu_shader4 : enable
#pragma optionNV(fastmath on)
#pragma optionNV(fastprecision on)
#pragma optionNV(ifcvt none)
#pragma optionNV(inline all)
#pragma optionNV(strict on)
#pragma optionNV(unroll all)

uniform float zFar;
uniform float zNear;

uniform mat4 mv;
uniform mat4 pr;

uniform float Width;
uniform float Height;

in vec2 _texCoord;

uniform vec3 eyePos;
uniform sampler2D depth_texture;
uniform sampler2D _shadow0;
uniform sampler2D _shadow1;
uniform sampler2D _shadow2;
uniform sampler2D color;
uniform sampler2D noise_texture;

in mat4 shadow_matrix0;
in mat4 shadow_matrix1;
in mat4 shadow_matrix2;

in vec4 vertex;
out vec4 FragColor;

bool inShadow()
{
	float s11 = texture2D(depth_texture, _texCoord).x;

	vec3 normal = normalize(vec3(dFdx(s11) * 500.0, dFdy(s11) * 500.0, 1.0)) * 0.5 + 0.5;
	float res = dot(normalize(normal*2.0 - 1.0), normalize(eyePos));
	if(res < 0.0) return true;
	else return false;
}


float lin_shadow(sampler2D _sh_tex, float x,float y,vec2 tex_coor,vec3 shad_coor) {
	////////////////
	float WH = Width*Height;
    float z = 0.0;
	vec4 s;
	if(x == 0.0 && y == 0.0) return 1.0;
	vec2 real_coor = vec2(x,y);
	vec2 tex_offset = vec2(1.0/Width,1.0/Height);
	const float fstsqrt = 2.0 * sqrt(2.0);
	const float _fstsqrt = 1.0 / fstsqrt;
	for(float i = -tex_offset.x; i <= tex_offset.x; i+=tex_offset.x) {
		for(float j = -tex_offset.y; j <= tex_offset.y; j+=tex_offset.y) {
			float shadow_blur;
			float tmp_res;
			if(i == 0.0 && j == 0.0) {
				tmp_res = texture2D(_sh_tex,tex_coor + vec2(i,j)).x + 0.00001;
				shadow_blur = fstsqrt - abs(i-x)*abs(j-y)*WH;
				s.x = tmp_res;
				
			}
			else if(i == tex_offset.x && j == 0.0) {
				tmp_res = texture2D(_sh_tex,tex_coor + vec2(i,j)).x + 0.00001;
				shadow_blur = fstsqrt - abs(i-x)*abs(j-y)*WH;
				s.y = tmp_res;
			}
			else if(i == tex_offset.x && j == tex_offset.y) {
				tmp_res = texture2D(_sh_tex,tex_coor + vec2(i,j)).x + 0.00001;
				shadow_blur = fstsqrt - abs(i-x)*abs(j-y)*WH;
				s.z = tmp_res;
			}
			else if(i == 0.0 && j == tex_offset.y) {
				tmp_res = texture2D(_sh_tex,tex_coor + vec2(i,j)).x + 0.00001;
				shadow_blur = fstsqrt - abs(i-x)*abs(j-y)*WH;
				s.w = tmp_res;
			}
			else continue;
			if(shad_coor.z >= tmp_res)	z += shadow_blur/(_fstsqrt);
		}
	}
	z *= 0.025;
	float _z = 0.0;
	if(shad_coor.z >= s.x) {
			_z += (tex_offset.x-x)*(tex_offset.y-y)*WH;
    }
    if(shad_coor.z >= s.y) {
		_z += (x)*(tex_offset.y-y)*WH;
	}
	if(shad_coor.z >= s.z) {
		_z += (x)*(y)*WH;	
	}
	if(shad_coor.z >= s.w) {
		_z += (tex_offset.x-x)*(y)*WH;
	}
	_z = max(min(_z,1.0),0.0);
	return _z * 0.5 + z * 0.5;
}

void main() {
	float fDepth = texture2D(depth_texture, _texCoord.xy).x;

	 vec2 noise = 2.0*texture2D(noise_texture, _texCoord).xy
              - vec2(1.0);
					
	vec4 inv_proj = inverse(mv * pr) * 
                 vec4(vertex.xy, 2.0 * fDepth - 1.0, 1.0);
	
	
	if(fDepth > 0.9999) {
		FragColor = texture2D(color, _texCoord);
		return;
	}
	
	float blend = 1.0 - min((0.9999 - fDepth) * 10000.0, 1.0); 
	
	vec4 posit0 = shadow_matrix0 * inv_proj;
	vec4 posit1 = shadow_matrix1 * inv_proj;
	vec4 posit2 = shadow_matrix2 * inv_proj;
 
	int shad_flag = 0;
	vec3 shad_coor;
	if(shad_flag == 0)  {
		shad_coor = (posit0.xyz / posit0.w) * 0.5 + 0.5;
		if(shad_coor.x <= 0.0 || shad_coor.x >= 1.0 || shad_coor.y <= 0.0 || shad_coor.y >= 1.0 ) shad_flag++;
	}
	if(shad_flag == 1)  {
		shad_coor = (posit1.xyz / posit1.w) * 0.5 + 0.5;
		if(shad_coor.x <= 0.0 || shad_coor.x >= 1.0 || shad_coor.y <= 0.0 || shad_coor.y >= 1.0 ) shad_flag++;
	}
	if(shad_flag == 2)  {
		shad_coor = (posit2.xyz / posit2.w) * 0.5 + 0.5;
		if(shad_coor.x <= 0.0 || shad_coor.x >= 1.0 || shad_coor.y <= 0.0 || shad_coor.y >= 1.0 ) shad_flag++;
	}
	vec2 sun_c = shad_coor.xy;
	
	int _int_x = int(sun_c.x * Width);
	int _int_y = int(sun_c.y * Height);
		
	float int_x = sun_c.x - float(_int_x)/Width;
	float int_y = sun_c.y - float(_int_y)/Height;
	
	float pmax = 0.0;	
	if(shad_flag == 0) pmax = (1.0 - lin_shadow(_shadow0,int_x,int_y,sun_c.xy - vec2(int_x,int_y),shad_coor)) * 0.5 + 0.5;
	else if(shad_flag == 1) pmax = (1.0 - lin_shadow(_shadow1,int_x,int_y,sun_c.xy - vec2(int_x,int_y),shad_coor)) * 0.5 + 0.5;
	else if(shad_flag == 2) pmax = (1.0 - lin_shadow(_shadow2,int_x,int_y,sun_c.xy - vec2(int_x,int_y),shad_coor)) * 0.5 + 0.5;
	else pmax = 1.0;
	
	pmax += blend;
	pmax = min(pmax, 1.0);
	
	//if(pmax == 1.0 && inShadow()) pmax = 0.5; 
	FragColor = texture2D(color, _texCoord) * pmax;
}