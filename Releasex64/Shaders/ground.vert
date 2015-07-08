#version 120
#pragma optionNV(fastmath on)
#pragma optionNV(fastprecision on)
#pragma optionNV(ifcvt none)
#pragma optionNV(inline all)
#pragma optionNV(strict on)
#pragma optionNV(unroll all)
varying vec3 pos,l,n,v,h;
varying vec2 texCoord;
uniform vec3 eyePos;
uniform vec3 lightPos;
varying vec2 offset;
uniform mat4 mv;
uniform mat4 pr;
uniform float time;
uniform int reflect;
varying	vec3 et;
varying	vec3 lt;

#extension GL_EXT_gpu_shader4 : enable
float Noise(float x, float y ,float z) {
    float n = 2.0*z + x + y * 57.0;
    n = float(int(( ((4.0*(n-z) * (n/2.0 * (z + 15731.0) + 789721.0) + 43589.0)) / 107374.0))%1000);
    n /= 1000.0;
    return n;
}

float height(float x, float y) {
    x+=500.0;
    y+=500.0;
    float sum = 0.0;
    for(int k = 0; k < 2; k++) {
        sum += (1.0/pow(2.0,float(k)))*abs(2.0*Noise(pow(2.0,float(k))*(x),pow(2.0,float(k))*(time/25.0),pow(2.0,float(k))*(y))-1.0);
    }
    return sum;
}

vec3 normaln(vec3 A, vec3 B, vec3 C) {
    float x = A.y*(B.z - C.z)+ B.y*(C.z - A.z) + C.y*(A.z - B.z);
    float y = A.z*(B.x - C.x) + B.z*(C.x - A.x) + C.z*(A.x - B.x);
    float z = A.x*(B.y - C.y) + B.x*(C.y - A.y) + C.x*(A.y - B.y);
    return vec3(x,y,z);
}


void main(void) {
    gl_Position = ftransform();
    gl_ClipVertex = gl_ModelViewMatrix*gl_Vertex;
    pos = (gl_ModelViewMatrix*gl_Vertex).xyz;
    texCoord = gl_MultiTexCoord0.xy;
    l = normalize(lightPos);
    n = normalize(gl_NormalMatrix * gl_Normal).xyz;
    v = normalize(eyePos - pos);
    h = normalize(l + v);
	

    vec3 poser = (gl_ModelViewMatrix*gl_Vertex).xyz;
    offset = vec2(0.0);
	if(pos.z <= 0.0) {      
    poser.z =  height(pos.x, pos.y);
    vec3 A,B,C;
    float sm_pm = 1.0;
    A = vec3(poser.x-sm_pm,poser.y-sm_pm,height(poser.x-sm_pm, poser.y-sm_pm));
    B = vec3(poser.x,poser.y-sm_pm,height(poser.x, poser.y-sm_pm));
    C = vec3(poser.x,poser.y,poser.z);
    offset = normalize(normaln(A,B,C)).xy/40.0;
	if(reflect == 1) pos.z *= -1.0;
    }
 
	vec3 tangent;
    vec3 binormal;
    vec3 c1 = cross(gl_Normal, vec3(0.0, 0.0, 1.0));
    vec3 c2 = cross(gl_Normal, vec3(0.0, 1.0, 0.0));
    if (length(c1)>length(c2)) {
        tangent = c1;
    }
    else {
        tangent = c2;
    }
    tangent = normalize(tangent);
    binormal = cross(gl_Normal, tangent);
    binormal = normalize(binormal);
    vec3 t = tangent;
    vec3 b = binormal;                                                    
    et = vec3 ( dot ( v, t ), dot ( v, b ), dot ( v, n ) );
    lt = vec3 ( dot ( l, t ), dot ( l, b ), dot ( l, n ) );
}
