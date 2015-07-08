#version 120
#pragma optionNV(fastmath on)
#pragma optionNV(fastprecision on)
#pragma optionNV(ifcvt none)
#pragma optionNV(inline all)
#pragma optionNV(strict on)
#pragma optionNV(unroll all)
varying vec3 pos,n;
varying vec2 texCoord;
uniform int Width;
uniform int Height;
uniform sampler2D tex;
float EPS = 0.001;

void main(void) {
	vec2 offset = vec2(1.0/ float(Width),1.0/ float(Height));
    if(texCoord.x <= EPS || texCoord.x >= 1.0 - EPS || texCoord.y <= EPS || texCoord.y >= 1.0 - EPS) {
        gl_FragColor = vec4(vec3(0.0),1.0);
    }
    vec4 core00 = texture2D(tex,texCoord + vec2(-offset.x,-offset.y)),
            core01 = texture2D(tex,texCoord + vec2(-offset.x,0.0)),
            core11 = texture2D(tex,texCoord),
            core10 = texture2D(tex,texCoord + vec2(0.0,-offset.y)),
            core20 = texture2D(tex,texCoord + vec2(offset.x,-offset.y)),
            core21 = texture2D(tex,texCoord + vec2(offset.x,0.0)),
            core22 = texture2D(tex,texCoord + vec2(offset.x,offset.y)),
            core02 = texture2D(tex,texCoord + vec2(-offset.x,offset.y)),
            core12  = texture2D(tex,texCoord + vec2(0.0,offset.y));
    vec4 Gx = vec4(0.0);
    vec4 Gy = Gx;
    Gx = -core00 - core01 - core01 - core02 + core22 + core21 + core21 + core20;

    Gy = -core00 - core10 - core10 - core20 + core02 + core12 + core12 + core22;  

    vec4 color = Gx * Gx + Gy * Gy;
	float left_lum = (color.x + color.y + color.z) * 0.3333 ; 
    float lum = pow(left_lum * left_lum * (core11.x + core11.y + core11.z) * 0.3333, 0.5) * 0.08333;
    float angle = sin(atan(((Gx.x + Gx.y + Gx.z)/(Gy.x + Gy.y + Gy.z))));
    if(angle > 0.0) color = vec4(angle * angle, 0.3 * angle * angle,0.0,1.0);
    else color = vec4(0.0,0.3 * angle * angle,1.0 * angle * angle,1.0);
    gl_FragColor = color*lum*1.0;
}
