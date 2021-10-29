#version 330

in vec2 uv;
in vec3 Normal;                                                                   
in vec3 WorldPos;  

         
out vec4 FragColor;

uniform vec3 cameraPos;
uniform vec3 lightDir;
uniform sampler2D texture_diffuse1;
uniform sampler2DShadow shadowMap;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
	vec4 FragPosLightSpace;
    vec2 TexCoords;
	
} fs_in;

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
	
	vec2 s = 1.0f/vec2(textureSize(shadowMap,0));

	for(int i=0;i<8;i++)
	{
	  float angle = random(gl_FragCoord.xy) *0.785398f; // 3.1415926f*0.25f; 
	  
	  vec2 rotation = vec2(sin(angle),cos(angle));
	  
	  shadow += textureProj(shadowMap, vec4(fragPosLightSpace.xy+RandomSample[i]*fragPosLightSpace.w*s*rotation, fragPosLightSpace.z, fragPosLightSpace.w));
	}
	
	
    shadow /= 8.0;
    
    if(fragPosLightSpace.z/fragPosLightSpace.w > 1.0)
        shadow = 1.0;
        
    return shadow;
}

                                                          
void main()
{                                    
  vec3 L =  normalize(lightDir);
  vec3 N =  normalize(fs_in.Normal);
  
  vec3 V = normalize(cameraPos - fs_in.FragPos);
  vec3 H = normalize(L+V);
  
  float  diff = max(dot(L,N),0.0f);
  float spec = pow(max(dot(N,H),0.0f),32.0);
  
  vec3 diffuseColor = diff*vec3(1.0f);
  vec3 specColor = spec*vec3(0.5f);
  vec3 ambinet = vec3(0.05f);
  
  vec3 baseColor = texture(texture_diffuse1,fs_in.TexCoords).rgb;
  
  float shadow = calculateShadow(fs_in.FragPosLightSpace);
   
  vec3 color = ambinet+ (diffuseColor + specColor)*baseColor;
  
  
  FragColor=vec4(color,1.0f);  
}
