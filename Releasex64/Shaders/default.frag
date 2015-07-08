#version 120
uniform sampler2D tex;
varying vec2 texCoord;


void main() {	
	gl_FragColor = texture2D(tex, texCoord);
	if(texCoord.x >= 0.4995 && texCoord.x <= 0.5005 && texCoord.y >= 0.4995 && texCoord.y <= 0.5005 ) {
		vec4 exposure = vec4(0.0);
		vec4 t_color;
		for(float i = 0.0; i <= 1.0; i+=1.0/128.0) {
			for(float j = 0.0; j <= 1.0; j+=1.0/128.0) {
				t_color = texture2D(tex, vec2(i,j));
				exposure += t_color/(50.0);
			}
		}
	
	vec4 color = exposure/(128.0);
	float exp = color.x*0.3 + color.y*0.59 + color.z *0.11;
	gl_FragColor = vec4(exp);
	}
}