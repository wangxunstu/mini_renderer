#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;


uniform vec3 cameraPos;

uniform sampler2DShadow u_shadowMap;

uniform  mat4 lightSpaceMatrix;

uniform bool u_hasDiffuseTexture;
uniform bool u_hasSpecularTexture;

uniform int shadowType;

uniform int u_shininess;

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

#define MAX_DIR_LIGHT 5

uniform DirectionalLight u_directionLights[MAX_DIR_LIGHT];



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

    float shadow = 0.0;
	
	vec2 s = 1.0f/vec2(textureSize(u_shadowMap,0));

	for(int i=0;i<8;i++)
	{
	  float angle = random(gl_FragCoord.xy) *0.785398f; // 3.1415926f*0.25f; 
	  
	  vec2 rotation = vec2(sin(angle),cos(angle));
	  
	  shadow += textureProj(u_shadowMap, vec4(fragPosLightSpace.xy+RandomSample[i]*fragPosLightSpace.w*s*rotation, fragPosLightSpace.z, fragPosLightSpace.w));
	}
	
	
    shadow /= 8.0;
    
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
   
    vec3  fragPos = texture(gPosition, TexCoords).rgb;
    vec3  diffuse = texture(gAlbedo, TexCoords).rgb;
    float shininess = u_shininess;
    
	vec4 lightSpaceFragPos = bias * lightSpaceMatrix * vec4(fragPos,1.0f);
	
	float shadow = calculateShadow(lightSpaceFragPos);
	
	
	vec3 lightColor = u_directionLights[0].color;
	vec3 specularColor = lightColor;
   
    vec3 N = normalize(texture(gNormal, TexCoords).rgb);
    vec3 L = normalize(u_directionLights[0].direction); 
	vec3 V = normalize(cameraPos - fragPos);
	vec3 H = normalize(L+V);

    vec3 color = blinnPhongBRDF(lightColor,diffuse,lightColor,specularColor,N,L,H,shininess);	
   
    color*=shadow;
    
    FragColor = vec4(color*u_directionLights[0].intensity, 1.0);
}
