#version 120
#extension GL_EXT_gpu_shader4 : enable
uniform sampler2D gdepth;
uniform sampler2D grandom;
uniform sampler2D gdiffuse;
uniform float Width;
uniform float zFar;
uniform float zNear;
uniform float Height;

#define M_PI 3.1415926535897932384626433832795
// sampling radius is in view space
#define SAMPLING_RADIUS 0.5
#define NUM_SAMPLING_DIRECTIONS 8
// sampling step is in texture space
vec2 SAMPLING_STEP = vec2(1.0 / Width, 1.0 / Height);
#define NUM_SAMPLING_STEPS 4
#define THRESHOLD 0.1
#define SCALE 1.0
#define TANGENT_BIAS 0.2

/*
float filter(float x)
{
    return max(0, 1.0 - x*x);
}
*/

vec3 posFromDepth(vec2 coord){
	float z_b = texture2D(gdepth,coord).x;
	float z_n = 2.0 * z_b - 1.0;
	float z_e = 2.0 * zNear * zFar / (zFar + zNear - z_n *(zFar - zNear));
	z_e -= 0.1;
	z_e /= 1000.0;
	z_e += 0.1;
	if(z_e > 0.999) z_e = 0.999;
	float d = z_e;
	vec3 tray = mat3x3(gl_ProjectionMatrixInverse)*vec3((coord.x-0.5)*2.0,(coord.y-0.5)*2.0,1.0);
	return tray*d;
}

vec4 readNormal(vec2 coord)
{
	float z_b = texture2D(gdepth,coord).x;
	float z_n = 2.0 * z_b - 1.0;
	float z_e = 2.0 * zNear * zFar / (zFar + zNear - z_n *(zFar - zNear));
	z_e -= 0.1;
	z_e /= 1000.0;
	z_e += 0.1;
	float dst = z_e;
	if(z_e > 0.98) z_e = 0.98;
	float s11 = z_e;

	vec3 normal = normalize(vec3(dFdx(s11) * 500.0, dFdy(s11) * 500.0, 1.0)) * 0.5 + 0.5;
	return vec4(normalize(normal*2.0 - 1.0), dst);
}

void main()
{
	vec2 out_Tex = gl_TexCoord[0].st;
    vec3 viewPos = posFromDepth(out_Tex);
    vec4 viewN = readNormal(out_Tex);
	float dst = viewN.w;
	
	if(dst > 0.999) {
		gl_FragColor = texture2D(gdiffuse, out_Tex); 
	}
	vec3 viewNorm = viewN.xyz;
    float total = 0.0;
    float sample_direction_increment = 2 * M_PI / NUM_SAMPLING_DIRECTIONS;
    for (int i = 0; i < NUM_SAMPLING_DIRECTIONS; i++) {
        // no jittering or randomization of sampling direction just yet
        float sampling_angle = i * sample_direction_increment; // azimuth angle theta in the paper
        vec2 sampleDir = vec2(cos(sampling_angle), sin(sampling_angle));
        // we will now march along sampleDir and calculate the horizon
        // horizon starts with the tangent plane to the surface, whose angle we can get from the normal
        float tangentAngle = acos(dot(vec3(sampleDir, 0), viewNorm)) - (0.5 * M_PI) + TANGENT_BIAS;
        float horizonAngle = tangentAngle;
        vec3 lastDiff = vec3(0);
        for (int j = 0; j < NUM_SAMPLING_STEPS; j++) {
            // march along the sampling direction and see what the horizon is
            vec2 sampleOffset = SAMPLING_STEP * sampleDir * float(j+1);
            vec2 offTex = out_Tex + sampleOffset;

            vec3 off_viewPos = posFromDepth(offTex);
            // we now have the view space position of the offset point
            vec3 diff = off_viewPos.xyz - viewPos.xyz;
            if (length(diff) < SAMPLING_RADIUS) {
                // skip samples which are outside of our local sampling radius
                lastDiff = diff;
                float elevationAngle = atan(diff.z / length(diff.xy));
                horizonAngle = max(horizonAngle, elevationAngle);
            }
        }
        // the paper uses this attenuation but I like the other way better
        float normDiff = length(lastDiff) / SAMPLING_RADIUS;
        float attenuation = 1 - normDiff*normDiff;
      //  float attenuation = 1.0 / (1 + length(lastDiff));
        // now compare horizon angle to tangent angle to get ambient occlusion
        float occlusion = clamp(attenuation * (sin(horizonAngle) - sin(tangentAngle)), 0.0, 1.0);
        total += 1.0 - occlusion;
    }
    total /= NUM_SAMPLING_DIRECTIONS;
	
	
	total = min(1.0, 1.7 - total);
	if(dst > 0.98) {
		total = min(total + (0.999 - 0.98) * 50.0, 1.0);
	}
    gl_FragColor = texture2D(gdiffuse, out_Tex) * total;//vec4(total, total, total, 1.0);//texture2D(gdiffuse, out_Tex) * total;//
	
}
/*
vec3 readNormal(vec2 coord)
{
	float z_b = texture2D(gdepth,coord).x;
	float z_n = 2.0 * z_b - 1.0;
	float z_e = 2.0 * zNear * zFar / (zFar + zNear - z_n *(zFar - zNear));
	z_e -= 0.1;
	z_e /= 1000.0;
	z_e += 0.1;
	if(z_e > 0.98) z_e = 0.98;
	float s11 = z_e;

	vec3 normal = normalize(vec3(dFdx(s11) * 500.0, dFdy(s11) * 500.0, 1.0)) * 0.5 + 0.5;
	return normalize(normal*2.0 - 1.0);
}


//Ambient Occlusion form factor:
float aoFF(vec3 ddiff, vec3 cnorm, float c1, float c2){
	vec3 vv = normalize(ddiff);
	float rd = length(ddiff);
	return (1.0-clamp(dot(readNormal(gl_TexCoord[0].st + vec2(c1,c2)),-vv),0.0,1.0)) *
	clamp(dot( cnorm,vv ),0.0,1.0)*
	(1.0 - 1.0/sqrt(1.0/(rd*rd) + 1.0));
}
//GI form factor:
float giFF(vec3 ddiff,vec3 cnorm, float c1, float c2){
	vec3 vv = normalize(ddiff);
	float rd = length(ddiff);
	return 1.0*clamp(dot(readNormal(gl_TexCoord[0].st+vec2(c1,c2)),-vv),0.0,1.0)*
	clamp(dot( cnorm,vv ),0.0,1.0)/
	(rd*rd+1.0);
}

void main()
{
	//read current normal,position and color.
	vec3 n = readNormal(gl_TexCoord[0].st);
	vec3 p = posFromDepth(gl_TexCoord[0].st);
	vec3 diffuse = vec3(0.3,0.3,0.3);
	vec3 col = diffuse;
	
	//randomization texture
	vec2 fres = vec2(Width/128.0*5,Height/128.0*5);
	vec3 random = texture2D(grandom, gl_TexCoord[0].st*fres.xy).xyz;
	random = random*2.0-vec3(1.0);
	
	//initialize variables:
	float ao = 0.0;
	vec3 gi = vec3(0.0,0.0,0.0);
	float incx = 1.0/Width*0.1;
	float incy = 1.0/Height*0.1;
	float pw = incx;
	float ph = incy;



	float z_b = texture2D(gdepth, gl_TexCoord[0].st).x;
	float z_n = 2.0 * z_b - 1.0;
	float z_e = 2.0 * zNear * zFar / (zFar + zNear - z_n *(zFar - zNear));
	z_e -= 0.1;
	z_e /= 1000.0;
	z_e += 0.1;
	if(z_e > 0.999) z_e = 0.999;
	float cdepth = z_e;
	
	//3 rounds of 8 samples each.
	for(float i=0.0; i<3.0; ++i)
	{
		float npw = (pw+0.0007*random.x)/cdepth;
		float nph = (ph+0.0007*random.y)/cdepth;
	
		vec3 ddiff = posFromDepth(gl_TexCoord[0].st+vec2(npw,nph))-p;
		vec3 ddiff2 = posFromDepth(gl_TexCoord[0].st+vec2(npw,-nph))-p;
		vec3 ddiff3 = posFromDepth(gl_TexCoord[0].st+vec2(-npw,nph))-p;
		vec3 ddiff4 = posFromDepth(gl_TexCoord[0].st+vec2(-npw,-nph))-p;
		vec3 ddiff5 = posFromDepth(gl_TexCoord[0].st+vec2(0,nph))-p;
		vec3 ddiff6 = posFromDepth(gl_TexCoord[0].st+vec2(0,-nph))-p;
		vec3 ddiff7 = posFromDepth(gl_TexCoord[0].st+vec2(npw,0))-p;
		vec3 ddiff8 = posFromDepth(gl_TexCoord[0].st+vec2(-npw,0))-p;
		
		ao+= aoFF(ddiff,n,npw,nph);
		ao+= aoFF(ddiff2,n,npw,-nph);
		ao+= aoFF(ddiff3,n,-npw,nph);
		ao+= aoFF(ddiff4,n,-npw,-nph);
		ao+= aoFF(ddiff5,n,0,nph);
		ao+= aoFF(ddiff6,n,0,-nph);
		ao+= aoFF(ddiff7,n,npw,0);
		ao+= aoFF(ddiff8,n,-npw,0);
		
		gi+= giFF(ddiff,n,npw,nph)*diffuse;
		gi+= giFF(ddiff2,n,npw,-nph)*diffuse;
		gi+= giFF(ddiff3,n,-npw,nph)*diffuse;
		gi+= giFF(ddiff4,n,-npw,-nph)*diffuse;
		gi+= giFF(ddiff5,n,0,nph)*diffuse;
		gi+= giFF(ddiff6,n,0,-nph)*diffuse;
		gi+= giFF(ddiff7,n,npw,0)*diffuse;
		gi+= giFF(ddiff8,n,-npw,0)*diffuse;
		
		//increase sampling area:
		pw += incx;
		ph += incy;
	}
	ao/=24.0;
	gi/=24.0;
	
	
	if(z_e >= 0.999) gl_FragColor = texture2D(gdiffuse, gl_TexCoord[0].st);
	else {
		float AO = 1.0 - (col-ao+gi*3.0);
	
		if(z_e >= 0.98) AO += (1.0 - (1.0 - z_e) * 50.0);
		gl_FragColor = texture2D(gdiffuse, gl_TexCoord[0].st) * vec4(vec3(min(AO, 1.0)),1.0);
	}
	
	
}
*/