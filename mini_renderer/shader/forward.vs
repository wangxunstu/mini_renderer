#version 330 core
layout (location = 0) in vec3  aPos;
layout (location = 1) in vec3  aNormal;
//layout (location = 2) in vec2  aTexCoords;
//layout (location = 3) in ivec4 aBoneIDs;
//layout (location = 4) in vec4  aWeights;

out VS_OUT {
    //vec3 FragPos;
    //vec3 Normal;
	//vec4 FragPosLightSpace;
    //vec2 TexCoords;
	
	vec4 color;
} vs_out;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

const int N_BANDS = 5;
uniform samplerBuffer shCoefficients;
uniform vec3 lightSHCoefficients[N_BANDS * N_BANDS];


//uniform mat4  lightSpaceMatrix;
//
//uniform bool skining;
//
//const int MAX_BONES = 100;
//uniform mat4 Bones[MAX_BONES];
//
//
//const mat4 bias = mat4(	0.5, 0.0, 0.0, 0.0,
//						0.0, 0.5, 0.0, 0.0,
//						0.0, 0.0, 0.5, 0.0,
//						0.5, 0.5, 0.5, 1.0);
//						
						

void main()
{
    
	//mat4 BoneTransform = Bones[aBoneIDs[0]] * aWeights[0];
    //BoneTransform     += Bones[aBoneIDs[1]] * aWeights[1];
    //BoneTransform     += Bones[aBoneIDs[2]] * aWeights[2];
    //BoneTransform     += Bones[aBoneIDs[3]] * aWeights[3];

    vec4 PosL    =  vec4(aPos, 1.0);
    //vec4 NormalL =  vec4(aNormal, 0.0);
	// 
	//if(skining) 
	//{
	//   PosL = BoneTransform*PosL;
	//   NormalL = BoneTransform*NormalL;
	//}
	//
	//vs_out.FragPos   =  vec3 (model * PosL);
	//
    //vs_out.Normal    =    normalize(vec3(transpose(inverse(model)) * NormalL));
	//
	//vs_out.FragPosLightSpace = bias * lightSpaceMatrix *model*PosL;
    //vs_out.TexCoords = aTexCoords;
	
	int NN = N_BANDS * N_BANDS;
	
	vec3 vertexColor = vec3(0,0,0);
	for(int i = 0 ; i < NN ; i++)
	{
		int offset = (gl_VertexID * NN + i ) * 3;
		float r = texelFetch( shCoefficients, offset + 0 ).r;	// Extract RGB components of the vertex SH coefficients.
		float g = texelFetch( shCoefficients, offset + 1 ).r;
		float b = texelFetch( shCoefficients, offset + 2 ).r;
		
		vertexColor+= vec3(r,g,b);
		
	}
	
	vs_out.color = vec4(vertexColor*10.0f,1.0);
	
	
    gl_Position = projection * view * model* PosL;
}