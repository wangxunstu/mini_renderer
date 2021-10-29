#version 330                                                                        
                                                                                    
layout (location = 0) in vec3 Position;                                                                                         
layout (location = 1) in vec3 Normal; 
layout (location = 2) in vec2 TexCoord; 
layout (location = 3) in ivec4 BoneIDs;
layout (location = 4) in vec4 Weights;


out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
	vec4 FragPosLightSpace;
    vec2 TexCoords;
	
} vs_out;                                                              

const int MAX_BONES = 100;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

uniform mat4 Bones[MAX_BONES];

uniform mat4  lightSpaceMatrix;


const mat4 bias = mat4(	0.5, 0.0, 0.0, 0.0,
						0.0, 0.5, 0.0, 0.0,
						0.0, 0.0, 0.5, 0.0,
						0.5, 0.5, 0.5, 1.0);
						


void main()
{       
    mat4 BoneTransform = Bones[BoneIDs[0]] * Weights[0];
    BoneTransform     += Bones[BoneIDs[1]] * Weights[1];
    BoneTransform     += Bones[BoneIDs[2]] * Weights[2];
    BoneTransform     += Bones[BoneIDs[3]] * Weights[3];

    vec4 PosL    = BoneTransform * vec4(Position, 1.0);
	
	
	
    gl_Position  = projection*view*model* PosL;
  
    vec4 NormalL = BoneTransform * vec4(Normal, 0.0);

	vs_out.FragPosLightSpace = bias * lightSpaceMatrix *model*PosL;
	
    vs_out.Normal    =   normalize(vec3(transpose(inverse(model)) * (BoneTransform* vec4(Normal, 0.0))));
    vs_out.FragPos   = (model * PosL).xyz;   
    vs_out.TexCoords = TexCoord;	
}