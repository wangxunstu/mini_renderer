#version 330 core
out vec4 FragColor;

in vec2 TexCoords;


#define MAX_DIR_LIGHT 100


// IBL
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

uniform sampler2D gBuffer0;
uniform sampler2D gBuffer1;
uniform sampler2D gBuffer2;
uniform sampler2D gBuffer3;


// lights
struct DirectionalLight
{
    vec3 direction;
    vec3 color;
    float intensity;
};

uniform DirectionalLight u_directionLights[MAX_DIR_LIGHT];

uniform sampler2DShadow u_shadowMap;

uniform  mat4 lightSpaceMatrix[MAX_DIR_LIGHT];

uniform int u_shininess;

uniform vec3 cameraPos;

uniform bool u_enable_hdr_post;

const float PI = 3.14159265359f;

const float INV_PI = 0.318309891613572f;

const mat4 bias = mat4(	0.5, 0.0, 0.0, 0.0,
						0.0, 0.5, 0.0, 0.0,
						0.0, 0.0, 0.5, 0.0,
						0.5, 0.5, 0.5, 1.0);
						
										
struct DirectionalLight
{
    vec3 direction;
    vec3 color;
    float intensity;
};


vec3 ACESFilm(vec3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((x*(a*x+b))/(x*(c*x+d)+e),0.0f,1.0f);
}



// ----------------------------------------------------------------------------

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}   
// ----------------------------------------------------------------------------



float random(vec2 co)
{
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}



float calculateShadow(vec4 fragPosLightSpace)
{
   vec2 RandomSample[8];
   RandomSample[0] = vec2(-0.7171f, 0.7171f);
   RandomSample[1] = vec2(0.0f, -0.875f);
   RandomSample[2] = vec2(0.5303f, 0.5303f);
   RandomSample[3] = vec2(-0.625f, 0.0f);
   RandomSample[4] = vec2(0.3536f, -0.3536f);
   RandomSample[5] = vec2(0.0f, 0.375f);
   RandomSample[6] = vec2(-0.1768f, -0.1768f);
   RandomSample[7] = vec2(0.125f, 0.0f);
   
   
   
    vec2 poissonDisk[64];
    poissonDisk[0] = vec2(-0.613392, 0.617481);
    poissonDisk[1] = vec2(0.170019, -0.040254);
    poissonDisk[2] = vec2(-0.299417, 0.791925);
    poissonDisk[3] = vec2(0.645680, 0.493210);
    poissonDisk[4] = vec2(-0.651784, 0.717887);
    poissonDisk[5] = vec2(0.421003, 0.027070);
    poissonDisk[6] = vec2(-0.817194, -0.271096);
    poissonDisk[7] = vec2(-0.705374, -0.668203);
    poissonDisk[8] = vec2(0.977050, -0.108615);
    poissonDisk[9] = vec2(0.063326, 0.142369);
    poissonDisk[10] = vec2(0.203528, 0.214331);
    poissonDisk[11] = vec2(-0.667531, 0.326090);
    poissonDisk[12] = vec2(-0.098422, -0.295755);
    poissonDisk[13] = vec2(-0.885922, 0.215369);
    poissonDisk[14] = vec2(0.566637, 0.605213);
    poissonDisk[15] = vec2(0.039766, -0.396100);
    poissonDisk[16] = vec2(0.751946, 0.453352);
    poissonDisk[17] = vec2(0.078707, -0.715323);
    poissonDisk[18] = vec2(-0.075838, -0.529344);
    poissonDisk[19] = vec2(0.724479, -0.580798);
    poissonDisk[20] = vec2(0.222999, -0.215125);
    poissonDisk[21] = vec2(-0.467574, -0.405438);
    poissonDisk[22] = vec2(-0.248268, -0.814753);
    poissonDisk[23] = vec2(0.354411, -0.887570);
    poissonDisk[24] = vec2(0.175817, 0.382366);
    poissonDisk[25] = vec2(0.487472, -0.063082);
    poissonDisk[26] = vec2(-0.084078, 0.898312);
    poissonDisk[27] = vec2(0.488876, -0.783441);
    poissonDisk[28] = vec2(0.470016, 0.217933);
    poissonDisk[29] = vec2(-0.696890, -0.549791);
    poissonDisk[30] = vec2(-0.149693, 0.605762);
    poissonDisk[31] = vec2(0.034211, 0.979980);
    poissonDisk[32] = vec2(0.503098, -0.308878);
    poissonDisk[33] = vec2(-0.016205, -0.872921);
    poissonDisk[34] = vec2(0.385784, -0.393902);
    poissonDisk[35] = vec2(-0.146886, -0.859249);
    poissonDisk[36] = vec2(0.643361, 0.164098);
    poissonDisk[37] = vec2(0.634388, -0.049471);
    poissonDisk[38] = vec2(-0.688894, 0.007843);
    poissonDisk[39] = vec2(0.464034, -0.188818);
    poissonDisk[40] = vec2(-0.440840, 0.137486);
    poissonDisk[41] = vec2(0.364483, 0.511704);
    poissonDisk[42] = vec2(0.034028, 0.325968);
    poissonDisk[43] = vec2(0.099094, -0.308023);
    poissonDisk[44] = vec2(0.693960, -0.366253);
    poissonDisk[45] = vec2(0.678884, -0.204688);
    poissonDisk[46] = vec2(0.001801, 0.780328);
    poissonDisk[47] = vec2(0.145177, -0.898984);
    poissonDisk[48] = vec2(0.062655, -0.611866);
    poissonDisk[49] = vec2(0.315226, -0.604297);
    poissonDisk[50] = vec2(-0.780145, 0.486251);
    poissonDisk[51] = vec2(-0.371868, 0.882138);
    poissonDisk[52] = vec2(0.200476, 0.494430);
    poissonDisk[53] = vec2(-0.494552, -0.711051);
    poissonDisk[54] = vec2(0.612476, 0.705252);
    poissonDisk[55] = vec2(-0.578845, -0.768792);
    poissonDisk[56] = vec2(-0.772454, -0.090976);
    poissonDisk[57] = vec2(0.504440, 0.372295);
    poissonDisk[58] = vec2(0.155736, 0.065157);
    poissonDisk[59] = vec2(0.391522, 0.849605);
    poissonDisk[60] = vec2(-0.620106, -0.328104);
    poissonDisk[61] = vec2(0.789239, -0.419965);
    poissonDisk[62] = vec2(-0.545396, 0.538133);
    poissonDisk[63] = vec2(-0.178564, -0.596057);


    float shadow = 0.0;
	
	int samples = 64;
	
	vec2 s = 1.0f/vec2(textureSize(u_shadowMap,0));

	for(int i=0;i<samples;i++)
	{
	  float angle =  random(gl_FragCoord.xy) *  PI * 0.5f; 
	  
	  vec2 rotation = vec2(sin(angle),cos(angle));
	  
	  vec2 randOffset = poissonDisk[i];
	  
	  randOffset.x=poissonDisk[i].x*cos(angle) -poissonDisk[i].y*sin(angle);
	  randOffset.y=poissonDisk[i].x*sin(angle) +poissonDisk[i].y*cos(angle);
	  
	  
	  shadow += textureProj(u_shadowMap, vec4(fragPosLightSpace.xy + randOffset*fragPosLightSpace.w*s, fragPosLightSpace.z, fragPosLightSpace.w));
	}
	
	
    shadow /= float(samples);
    
    if(fragPosLightSpace.z/fragPosLightSpace.w > 1.0)
        shadow = 1.0;
        
    return shadow;
}



vec3 blinnPhongBRDF(vec3 Id, vec3 kd, vec3 Is, vec3 ks, vec3 normal, vec3 lightVec, vec3 halfway, float shininess)
{
    float diffuseFactor = max(dot(normal, lightVec), 0.0);
    vec3 diffuse = Id * kd * diffuseFactor;
    
    float specFactor = pow(max(dot(normal, halfway), 0.0), shininess);
    vec3 specular = vec3(0.0);
    
    if (shininess > 0.0 && diffuseFactor > 0.0)
        specular = Is * ks * specFactor;
    
    return diffuse + specular;
}


void main()
{		
    // material properties

	vec3 WorldPos    =    texture(gBuffer0,TexCoords).rgb;
    float ao         =    texture(gBuffer0,TexCoords).a;
	
    vec3 normal      =    texture(gBuffer1,TexCoords).rgb;
    float roughness  =    texture(gBuffer1,TexCoords).a;
	  
	vec3 albedo      =    texture(gBuffer2,TexCoords).rgb;
    float metallic   =    texture(gBuffer2,TexCoords).a;
	
	float shadingID    =    texture(gBuffer3,TexCoords).a;
	
	if(shadingID > 0.5f)
	{
	
	   // input lighting data
       vec3 N = normal;
       vec3 V = normalize(cameraPos - WorldPos);
       vec3 R = reflect(-V, N); 
	   
       // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
       // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
       vec3 F0 = vec3(0.04); 
       F0 = mix(F0, albedo, metallic);
	   
       // reflectance equation
       vec3 Lo = vec3(0.0);
       for(int i = 0; i < 4; ++i) 
       {
           // calculate per-light radiance
           vec3 L = normalize(u_directionLights[i].direction-WorldPos);
           vec3 H = normalize(V + L);
           
	   	float distance = length(u_directionLights[i].direction - WorldPos);
           float attenuation = 1.0 / (distance * distance);
           
	   	vec3 radiance = u_directionLights[i].color * attenuation;
	   
           // Cook-Torrance BRDF
           float NDF = DistributionGGX(N, H, roughness);   
           float G   = GeometrySmith(N, V, L, roughness);    
           vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);        
           
           vec3 nominator    = NDF * G * F;
           float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; // 0.001 to prevent divide by zero.
           vec3 specular = nominator / denominator;
           
            // kS is equal to Fresnel
           vec3 kS = F;
        
           vec3 kD = vec3(1.0) - kS;
       
           kD *= 1.0 - metallic;	                
               
         
           float NdotL = max(dot(N, L), 0.0);        
	   
           // add to outgoing radiance Lo
           Lo += (kD * albedo / PI + specular) * radiance * NdotL; // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
       }   
       
       // ambient lighting (we now use IBL as the ambient term)
       vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
       
       vec3 kS = F;
       vec3 kD = 1.0 - kS;
       kD *= 1.0 - metallic;	  
       
       vec3 irradiance = texture(irradianceMap, N).rgb;
       vec3 diffuse      = irradiance * albedo;
       
       // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
       const float MAX_REFLECTION_LOD = 4.0;
       vec3 prefilteredColor = textureLod(prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb;    
       vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
       vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);
	   
       vec3 ambient = (kD * diffuse + specular) * ao;
       
       vec3 color = ambient + Lo;
	   
	   if(!u_enable_hdr_post)
	   {
	      color = ACESFilm(color);
	   }
	   
       color = pow(color, vec3(0.45454f)); 
	   
       FragColor = vec4(color , 1.0);
	
	
	}else	
	{
	    vec3  fragPos      = texture(gBuffer0, TexCoords).rgb;
        vec3  normal       = texture(gBuffer1, TexCoords).rgb;
		vec3  diffuseColor = texture(gBuffer2, TexCoords).rgb;
		
		
		float shininess = u_shininess;
        
		float shadow = 0.0f;
		
		for(int i = 0; i < MAX_DIR_LIGHT;i++)
		{
		   		
	      vec4 lightSpaceFragPos = bias * lightSpaceMatrix[i] * vec4(fragPos,1.0f);
	      
	       shadow += calculateShadow(lightSpaceFragPos);
	    
		}
		
		
		shadow = clamp(shadow,0.0f,1.0f);

	    
	    vec3 lightColor = u_directionLights[0].color;
	    
		vec3 specColor = vec3(1.0f);
					    
        vec3 N = normalize(normal);
	    
		vec3 V = normalize(cameraPos - fragPos);
		       
        float normalizeFactor = float(u_shininess+1)/(2.0f*PI);
      
		vec3 ambient = vec3(0.001f);
	    
        vec3 color = ambient;
		
		for(int i=0;i<MAX_DIR_LIGHT;i++)
		{
          vec3   L          = normalize(u_directionLights[i].direction); 
		  
		  vec3   H          = normalize(L+V);
          
		  float  diffFactor = max(dot(L,N),0.0f);
          
		  float  specFactor = pow(max(dot(N,H),0.0f),u_shininess);
		  
		  vec3 diff = diffFactor*diffuseColor*lightColor*INV_PI;
          
		  vec3 spec = specFactor*0.5f*specColor*lightColor*normalizeFactor;
		  
		  color+=(diff+spec);
		}	
		
		color *= shadow;
	
		
	    if(!u_enable_hdr_post)
	    {
	       color = ACESFilm(color);
	    }
		
		color = pow(color, vec3(0.45454f)); 
        
        FragColor = vec4(color*u_directionLights[0].intensity, 1.0);
	  
	}
	
	
    
}
