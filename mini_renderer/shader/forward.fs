#version 330 core

#define MAX_DIR_LIGHT 5

in VS_OUT {
    //vec3 FragPos;
    //vec3 Normal;
	//vec4 FragPosLightSpace;
    //vec2 TexCoords;
	
	vec4 color;
} fs_in;




//struct DirectionalLight
//{
//    vec3 direction;
//    vec3 color;
//    float intensity;
//};
//
//uniform DirectionalLight u_directionLights[MAX_DIR_LIGHT];
//
//uniform vec3 cameraPos;
//
//uniform sampler2DShadow u_shadowMap;
//
//uniform sampler2D texture_diffuse1;
//
//uniform float u_hasDiffuseTexture;
//
//uniform int shadowType;
//
//uniform float u_shininess;
//
//uniform vec3  u_diffuseColor;
//
//uniform vec3  u_specularColor;
//
//uniform bool u_enable_hdr_post;
//
//
out vec4 FragColor;
//
//
//const float PI = 3.14159265359f;
//const float INV_PI = 0.318309891613572f;
//
//float random(vec2 co)
//{
//    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
//}
//
//float calculateShadow(vec4 fragPosLightSpace)
//{
//   vec2 RandomSample[8];
//   RandomSample[0] = vec2(-0.7171f, 0.7171f);
//   RandomSample[1] = vec2(0.0f, -0.875f);
//   RandomSample[2] = vec2(0.5303f, 0.5303f);
//   RandomSample[3] = vec2(-0.625f, 0.0f);
//   RandomSample[4] = vec2(0.3536f, -0.3536f);
//   RandomSample[5] = vec2(0.0f, 0.375f);
//   RandomSample[6] = vec2(-0.1768f, -0.1768f);
//   RandomSample[7] = vec2(0.125f, 0.0f);
//   
//   
//   
//    vec2 poissonDisk[64];
//    poissonDisk[0] = vec2(-0.613392, 0.617481);
//    poissonDisk[1] = vec2(0.170019, -0.040254);
//    poissonDisk[2] = vec2(-0.299417, 0.791925);
//    poissonDisk[3] = vec2(0.645680, 0.493210);
//    poissonDisk[4] = vec2(-0.651784, 0.717887);
//    poissonDisk[5] = vec2(0.421003, 0.027070);
//    poissonDisk[6] = vec2(-0.817194, -0.271096);
//    poissonDisk[7] = vec2(-0.705374, -0.668203);
//    poissonDisk[8] = vec2(0.977050, -0.108615);
//    poissonDisk[9] = vec2(0.063326, 0.142369);
//    poissonDisk[10] = vec2(0.203528, 0.214331);
//    poissonDisk[11] = vec2(-0.667531, 0.326090);
//    poissonDisk[12] = vec2(-0.098422, -0.295755);
//    poissonDisk[13] = vec2(-0.885922, 0.215369);
//    poissonDisk[14] = vec2(0.566637, 0.605213);
//    poissonDisk[15] = vec2(0.039766, -0.396100);
//    poissonDisk[16] = vec2(0.751946, 0.453352);
//    poissonDisk[17] = vec2(0.078707, -0.715323);
//    poissonDisk[18] = vec2(-0.075838, -0.529344);
//    poissonDisk[19] = vec2(0.724479, -0.580798);
//    poissonDisk[20] = vec2(0.222999, -0.215125);
//    poissonDisk[21] = vec2(-0.467574, -0.405438);
//    poissonDisk[22] = vec2(-0.248268, -0.814753);
//    poissonDisk[23] = vec2(0.354411, -0.887570);
//    poissonDisk[24] = vec2(0.175817, 0.382366);
//    poissonDisk[25] = vec2(0.487472, -0.063082);
//    poissonDisk[26] = vec2(-0.084078, 0.898312);
//    poissonDisk[27] = vec2(0.488876, -0.783441);
//    poissonDisk[28] = vec2(0.470016, 0.217933);
//    poissonDisk[29] = vec2(-0.696890, -0.549791);
//    poissonDisk[30] = vec2(-0.149693, 0.605762);
//    poissonDisk[31] = vec2(0.034211, 0.979980);
//    poissonDisk[32] = vec2(0.503098, -0.308878);
//    poissonDisk[33] = vec2(-0.016205, -0.872921);
//    poissonDisk[34] = vec2(0.385784, -0.393902);
//    poissonDisk[35] = vec2(-0.146886, -0.859249);
//    poissonDisk[36] = vec2(0.643361, 0.164098);
//    poissonDisk[37] = vec2(0.634388, -0.049471);
//    poissonDisk[38] = vec2(-0.688894, 0.007843);
//    poissonDisk[39] = vec2(0.464034, -0.188818);
//    poissonDisk[40] = vec2(-0.440840, 0.137486);
//    poissonDisk[41] = vec2(0.364483, 0.511704);
//    poissonDisk[42] = vec2(0.034028, 0.325968);
//    poissonDisk[43] = vec2(0.099094, -0.308023);
//    poissonDisk[44] = vec2(0.693960, -0.366253);
//    poissonDisk[45] = vec2(0.678884, -0.204688);
//    poissonDisk[46] = vec2(0.001801, 0.780328);
//    poissonDisk[47] = vec2(0.145177, -0.898984);
//    poissonDisk[48] = vec2(0.062655, -0.611866);
//    poissonDisk[49] = vec2(0.315226, -0.604297);
//    poissonDisk[50] = vec2(-0.780145, 0.486251);
//    poissonDisk[51] = vec2(-0.371868, 0.882138);
//    poissonDisk[52] = vec2(0.200476, 0.494430);
//    poissonDisk[53] = vec2(-0.494552, -0.711051);
//    poissonDisk[54] = vec2(0.612476, 0.705252);
//    poissonDisk[55] = vec2(-0.578845, -0.768792);
//    poissonDisk[56] = vec2(-0.772454, -0.090976);
//    poissonDisk[57] = vec2(0.504440, 0.372295);
//    poissonDisk[58] = vec2(0.155736, 0.065157);
//    poissonDisk[59] = vec2(0.391522, 0.849605);
//    poissonDisk[60] = vec2(-0.620106, -0.328104);
//    poissonDisk[61] = vec2(0.789239, -0.419965);
//    poissonDisk[62] = vec2(-0.545396, 0.538133);
//    poissonDisk[63] = vec2(-0.178564, -0.596057);
//
//
//    float shadow = 0.0;
//	
//	int samples = 64;
//	
//	vec2 s = 1.0f/vec2(textureSize(u_shadowMap,0));
//
//	for(int i=0;i<samples;i++)
//	{
//	  float angle =  random(gl_FragCoord.xy) *  PI*0.125f; 
//	  
//	  vec2 rotation = vec2(sin(angle),cos(angle));
//	  
//	  vec2 randOffset = poissonDisk[i];
//	  
//	  //randOffset.x = poissonDisk[i].x*cos(angle) - poissonDisk[i].y*sin(angle);
//	  //randOffset.y = poissonDisk[i].x*sin(angle) + poissonDisk[i].y*cos(angle);
//	  
//	  
//	  shadow += textureProj(u_shadowMap, vec4(fragPosLightSpace.xy + randOffset*fragPosLightSpace.w*s, fragPosLightSpace.z, fragPosLightSpace.w));
//	}
//	
//	
//    shadow /= float(samples);
//    
//    if(fragPosLightSpace.z/fragPosLightSpace.w > 1.0)
//        shadow = 1.0;
//        
//    return shadow;
//}
//
//
//vec3 blinnPhongBRDF(vec3 Id, vec3 kd, vec3 Is, vec3 ks, vec3 normal, vec3 lightVec, vec3 halfway, float shininess)
//{
//    float diffuseFactor = max(dot(normal, lightVec), 0.0);
//    vec3 diffuse = Id * kd * diffuseFactor;
//    
//    float specFactor = pow(max(dot(normal, halfway), 0.0), shininess);
//    
//	vec3 specular =  Is * ks * specFactor;
//    
//    //if (shininess > 0.0 && diffuseFactor > 0.0)
//       
//    
//    return  diffuse + specular;
//}
//
//
//vec3 ACESFilm(vec3 x)
//{
//    float a = 2.51f;
//    float b = 0.03f;
//    float c = 2.43f;
//    float d = 0.59f;
//    float e = 0.14f;
//    return clamp((x*(a*x+b))/(x*(c*x+d)+e),0.0f,1.0f);
//}


void main()
{    
  
//  vec3 diffuseColor = u_diffuseColor; 
//  if(u_hasDiffuseTexture>0.1f)
//     diffuseColor = pow(texture(texture_diffuse1,fs_in.TexCoords).rgb,vec3(1.0f));
//  
// 
//  
//  vec3 specColor = u_specularColor;
//  
//  vec3 lightColor = u_directionLights[0].color;
//  
//  vec3 L =  normalize(u_directionLights[0].direction);
// 
//  vec3 N =  normalize(fs_in.Normal);  
// 
//  vec3 V = normalize(cameraPos - fs_in.FragPos);
// 
//  vec3 H = normalize(L+V);
//  
//  float  diffFactor = max(dot(L,N),0.0f);
//  float  specFactor = pow(max(dot(N,H),0.0f),64.0);
//  
//  float normalizeFactor = float(u_shininess+1)/(2.0f*PI);
//  
//  vec3 diff = diffFactor*diffuseColor*lightColor*INV_PI;
//  vec3 spec = specFactor*0.5f*specColor*lightColor*normalizeFactor;
//  
//  vec3 ambinet = vec3(0.001f);
//  
//  float shadow = 1.0f;
//  
//  //shadow = calculateShadow(fs_in.FragPosLightSpace);
//  
//  
//  vec3 color = ambinet+(diff+spec)*shadow;
//
//  if(!u_enable_hdr_post)
//  {
//     color = ACESFilm(color);
//  }
//   
//  color = pow(color,vec3(0.454545f));
    
  FragColor = fs_in.color; 

}