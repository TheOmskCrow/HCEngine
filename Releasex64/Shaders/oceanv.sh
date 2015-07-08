#version 120
#extension GL_ARB_gpu_shader5 : enable

attribute vec3 vertex;
attribute vec3 normal;
attribute vec3 texture;

uniform vec3 light_position;

varying vec3 light_vector;
varying vec3 normal_vector;
varying vec3 halfway_vector;
varying float fog_factor;
varying vec2 tex_coord;

void main() {
	vec3 flipVertex = vertex.xzy;
	
	gl_Position = gl_ModelViewMatrix * vec4(flipVertex, 1.0);
	fog_factor = min(-gl_Position.z/500.0, 1.0);
	gl_Position = gl_ProjectionMatrix * gl_Position;

	vec4 v = gl_ModelViewMatrix * vec4(flipVertex, 1.0);
	vec3 normal1 = normalize(normal);

	light_vector = normalize((gl_ModelViewMatrix * vec4(light_position, 1.0)).xyz - v.xyz);
	normal_vector = (inverse(transpose(gl_ModelViewMatrix)) * vec4(normal1, 0.0)).xyz;
        halfway_vector = light_vector + normalize(-v.xyz);

	tex_coord = texture.xy;
}