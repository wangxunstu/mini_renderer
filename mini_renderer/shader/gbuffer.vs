#version 330 core
layout (location = 0) in vec3  aPos;
layout (location = 1) in vec3  aNormal;
layout (location = 2) in vec2  aTexCoords;
layout (location = 3) in ivec4 aBoneIDs;
layout (location = 4) in vec4  aWeights;


out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform bool skining;

const int MAX_BONES = 100;
uniform mat4 Bones[MAX_BONES];


void main()
{

    mat4 BoneTransform = Bones[aBoneIDs[0]] * aWeights[0];
    BoneTransform     += Bones[aBoneIDs[1]] * aWeights[1];
    BoneTransform     += Bones[aBoneIDs[2]] * aWeights[2];
    BoneTransform     += Bones[aBoneIDs[3]] * aWeights[3];

    vec4 PosL    =  vec4(aPos, 1.0);
    vec4 NormalL =  vec4(aNormal, 0.0);
	 
	if(skining) 
	{
	   PosL = BoneTransform*PosL;
	   NormalL = BoneTransform*NormalL;
	}
	
   
    vec4 worldPos = model * PosL;
    FragPos = worldPos.xyz; 
    TexCoords = aTexCoords;
    
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    Normal = normalize(normalMatrix*NormalL.xyz);

    gl_Position = projection * view * worldPos;
}