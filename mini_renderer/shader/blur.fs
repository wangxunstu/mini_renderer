#version 330 core

in vec2 Texcoord;
uniform sampler2D blurTex;

uniform vec4 blurParams;

void main()
{
	
	vec2 uv = Texcoord;
	
	float weights[15] = float[15] ( 153, 816, 3060, 8568, 18564, 31824,    43758, 48620, 43758,    31824, 18564, 8568, 3060, 816, 153 );
	
	//float weights[25]=float[25](1,4, 7, 4, 1,
    //                            4,16,26,16,4,
    //                            7,26,41,26,7,
    //                            4,16,26,16,4,
    //                            1,4, 7, 4, 1);
	
	float weightSum = 262106.0f;   //273.0f
	
	vec2 coords = uv.xy  - blurParams.xy * 6.0;
	vec3 color = vec3(0.0f);
	
	for (int i = 0; i < 15; ++i)
	{
		color.rgb += texture(blurTex, coords).rgb * (weights[i] / weightSum);
		coords += blurParams.xy;
	}
	
	
	
	gl_FragColor=vec4(color.rgb,1.0f);
	
}