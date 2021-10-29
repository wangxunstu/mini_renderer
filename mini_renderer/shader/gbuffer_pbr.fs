#version 330 core
layout (location = 0) out vec4 gBuffer0;
layout (location = 1) out vec4 gBuffer1;
layout (location = 2) out vec4 gBuffer2;
layout (location = 3) out vec4 gBuffer3;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;


uniform bool u_hasDiffuseTexture;
uniform bool u_hasSpecularTexture;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;


//gBuffer layout
//pos.xyz      ao
//normal.xyz   roughness
//albedo.xyz   metallic

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;

uniform int shadingID;	

vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(normalMap, TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(FragPos);
    vec3 Q2  = dFdy(FragPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N   = normalize(Normal);
    vec3 T   = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B   = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}


void main()
{    
    
	vec3  albedo = vec3(1.0f);
    
	if(u_hasDiffuseTexture)
	  albedo = texture(texture_diffuse1,TexCoords).rgb;
	
	
    vec3  normal     = Normal;
	float metallic   = texture(metallicMap, TexCoords).r;
    float roughness  = texture(roughnessMap, TexCoords).r;
    float ao         = texture(aoMap, TexCoords).r;
	
	
	if(shadingID == 1)
	{
	  normal = getNormalFromMap();
	  albedo = texture(albedoMap, TexCoords).rgb;
	}
	
	
	
    gBuffer0 = vec4(FragPos,ao);
  
    gBuffer1 = vec4(normal,roughness);
	
	gBuffer2 = vec4(albedo,metallic);
	
	gBuffer3 = vec4(0,0,0,shadingID);
  
}