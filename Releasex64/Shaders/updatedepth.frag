#version 120
uniform sampler2D depth;
uniform sampler2D curDepth;
uniform int console;
varying vec2 texCoord;
uniform float time;

void main(void)
{
	//if(texCoord.x > 0.49 && texCoord.x < 0.51 && texCoord.y > 0.49 && texCoord.y < 0.51) {
		float current;
		if(console == 1) current = 0.0;
		else current = texture2D(depth, vec2(0.5, 0.5)).x;
		float currentFocus = texture2D(curDepth, vec2(0.5, 0.5)).x;
		currentFocus += time * current;
		currentFocus /= 1.0 + time;
		gl_FragColor = vec4(vec3(currentFocus), 1.0);
	//}
}