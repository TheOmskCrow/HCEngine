#version 120
void main()
{
    gl_Position = ftransform();
	gl_TexCoord[0].xy = gl_MultiTexCoord0.xy;
/*vec4(mix(vec2(-1.0), vec2(3.0), bvec2(gl_VertexID == 1, gl_VertexID == 2)), 0.0, 1.0);*/
}
