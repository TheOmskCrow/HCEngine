#version 120
uniform sampler2D depth;
varying vec2 texCoord;
uniform float zNear;
uniform float zFar;

void main(void)
{
	float z_b = texture2D(depth,texCoord).x;
	float z_n = 2.0 * z_b - 1.0;
	float z_e = 2.0 * zNear * zFar / (zFar + zNear - z_n *(zFar - zNear));
	z_e -= 0.1;
	z_e /= 100.0;
	z_e += 0.1;
	gl_FragColor = vec4(vec3(z_e), 1.0);
}