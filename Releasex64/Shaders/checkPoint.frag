#version 120
uniform sampler2D tex;
uniform int checked;
varying vec2 texCoord;


void main() {	
	gl_FragColor = texture2D(tex, texCoord);
	if(checked == 1) gl_FragColor.xyz = vec3(0.1, 0.6, 0.2);
	else if(checked == 2) gl_FragColor.xyz = vec3(0.7, 0.1, 0.2);
	else gl_FragColor.xyz = vec3(0.6, 0.6, 0.0);
	if(gl_FragColor.w > 0.0) gl_FragColor.w = 1.0;
	else gl_FragColor.w = 0.0;
}