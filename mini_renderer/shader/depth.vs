#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 3) in ivec4 BoneIDs;
layout (location = 4) in vec4 Weights;


uniform mat4 Bones[100];

uniform mat4 model;

uniform mat4 lightSpaceMatrix;

uniform bool skining;

void main()
{
    mat4 BoneTransform = Bones[BoneIDs[0]] * Weights[0];
    BoneTransform     += Bones[BoneIDs[1]] * Weights[1];
    BoneTransform     += Bones[BoneIDs[2]] * Weights[2];
    BoneTransform     += Bones[BoneIDs[3]] * Weights[3];
    
	vec4 posL = vec4(aPos,1.0f);
	
    if(skining)
	{
	   posL=BoneTransform*posL;
	}
   	
    gl_Position = lightSpaceMatrix * model* posL;
}