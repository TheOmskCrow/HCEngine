#version 120
varying vec3 pos,l,n,v,h;
varying vec2 texCoord;
varying vec4 posit0;
varying vec4 posit1;
varying vec4 posit2;
varying vec4 posit3;
varying vec4 posit4;
varying vec4 posit5;
varying vec4 posit6;
uniform vec3 eyePos;
uniform vec3 lightPos;
varying	vec3 et;
varying	vec3 lt;

void main(void) {
    gl_Position = ftransform();
   	posit0 = gl_TextureMatrix[1]*gl_ModelViewMatrix*gl_Vertex;
    posit1 = gl_TextureMatrix[2]*gl_ModelViewMatrix*gl_Vertex;
	posit2 = gl_TextureMatrix[3]*gl_ModelViewMatrix*gl_Vertex;
	posit3 = gl_TextureMatrix[4]*gl_ModelViewMatrix*gl_Vertex;
	posit4 = gl_TextureMatrix[5]*gl_ModelViewMatrix*gl_Vertex;
	posit5 = gl_TextureMatrix[6]*gl_ModelViewMatrix*gl_Vertex;
	posit6 = gl_TextureMatrix[7]*gl_ModelViewMatrix*gl_Vertex;
    gl_ClipVertex = gl_ModelViewMatrix*gl_Vertex;
    pos = (gl_ModelViewMatrix*gl_Vertex).xyz;
    texCoord = gl_MultiTexCoord0.xy;
	vec3 nrm = gl_Normal;
	nrm.x = 0.0;
    l = normalize(lightPos);
    n = normalize(gl_NormalMatrix * nrm).xyz;
    v = normalize(eyePos - pos);
    h = normalize(l + v);

    vec3 tangent;
    vec3 binormal;
    vec3 c1 = cross(nrm, vec3(0.0, 0.0, 1.0));
    vec3 c2 = cross(nrm, vec3(0.0, 1.0, 0.0));
    if (length(c1)>length(c2)) {
        tangent = c1;
    }
    else {
        tangent = c2;
    }
    tangent = normalize(tangent);
    binormal = cross(nrm, tangent);
    binormal = normalize(binormal);
    vec3 t = tangent;
    vec3 b = binormal;                                                    
    et = vec3 ( dot ( v, t ), dot ( v, b ), dot ( v, n ) );
    lt = vec3 ( dot ( l, t ), dot ( l, b ), dot ( l, n ) );
}