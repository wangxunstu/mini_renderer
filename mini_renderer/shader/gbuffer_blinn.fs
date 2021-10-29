#version 330 core
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedo;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

uniform bool u_hasDiffuseTexture;
uniform bool u_hasSpecularTexture;


void main()
{    
    vec4 albedo = vec4(1.0f);
	
	if(u_hasDiffuseTexture)
	   albedo = texture(texture_diffuse1,TexCoords);
        
    gPosition = vec4(FragPos,0.0f);
  
    gNormal = vec4(Normal,   0.0f);
	
	gAlbedo = albedo;
  
}