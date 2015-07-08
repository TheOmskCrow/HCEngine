#version 120
varying vec2 v_texCoord;
uniform float Width;
uniform float Height;
uniform vec2 direction;
uniform sampler2D texture; 
uniform int offsetType;
float samples = 10.0; 
float bokeh = 0.4;
vec2 dir1 = vec2(0.0, 1.0 / Height);
vec2 dir2 = vec2(-0.866 / Width, -0.5 / Height);
vec2 dir3 = vec2(0.866 / Width, -0.5 / Height);
float threshold = 2.0;
float gain = 1.0;

vec4 getColor(vec2 coords) //processing the sample
{
	vec4 col = vec4(0.0);
	
	col.r = texture2D(texture,coords + dir1).r;
	col.g = texture2D(texture,coords + dir2).g;
	col.b = texture2D(texture,coords + dir3).b;
	
	vec3 lumcoeff = vec3(0.299,0.587,0.114);
	float lum = dot(col.rgb, lumcoeff);
	float thresh = max((lum-threshold)*gain, 0.0);
	return col+mix(vec4(0.0),col,thresh);
}

void main (void)
{
    vec4 msum = vec4(0.0);
	vec4 sum = vec4(0.0);
	
	vec2 direction = vec2(direction.x / Width, direction.y / Height); 
	
	float delta = 1.0 / samples;
    for (float i = -samples / 2.0; i < samples / 2.0; i++) {
        vec4 color = getColor(v_texCoord + direction * i); 
		sum += color * delta;
        msum = max(color, msum); 
    }
	gl_FragColor = mix(sum, msum, bokeh);
	
	
}

