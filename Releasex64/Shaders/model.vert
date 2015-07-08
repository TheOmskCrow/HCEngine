#version 120
varying vec3 pos,l,n,v,h;
varying vec3 model_pos;
varying vec2 texCoord;
uniform vec3 eyePos;
uniform vec3 lightPos;
uniform int shadowing;
varying	vec3 et;
varying	vec3 lt;
varying vec3 ht;

void main(void) {
    gl_Position = ftransform();
	model_pos = gl_Vertex.xyz;
    gl_ClipVertex = gl_ModelViewMatrix*gl_Vertex;
    pos = (gl_ModelViewMatrix*gl_Vertex).xyz;
    texCoord = gl_MultiTexCoord0.xy;
	texCoord.y *= -1.0;
    l = normalize(lightPos);
    n = normalize(gl_NormalMatrix * gl_Normal).xyz;
    v = normalize(eyePos - pos);
    h = normalize(l + v);


    vec3 tangent;
    vec3 binormal;
    tangent = cross(gl_Normal, vec3(0.0, 0.0, 1.0));
    /*vec3 c2 = cross(gl_Normal, vec3(0.0, 1.0, 0.0));
    if (length(c1)>length(c2)) {
        tangent = c1;
    }
    else {
        tangent = c2;
    }*/
    tangent = normalize(tangent);
    binormal = cross(gl_Normal, tangent);
    binormal = normalize(binormal);
    vec3 t = tangent;
    vec3 b = binormal; 
	vec3 half = normalize(l + pos);
    et = vec3 ( dot ( v, t ), dot ( v, b ), dot ( v, n ) );
    lt = vec3 ( dot ( l, t ), dot ( l, b ), dot ( l, n ) );
	ht = vec3 ( dot ( half, t ), dot ( half, b ), dot ( half, n ) );
}