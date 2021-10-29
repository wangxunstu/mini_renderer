#version 330 core

in vec2 V_Texcoord;

uniform sampler2D tex0;
uniform sampler2D tex1;

vec4 adapteParams;

//params 0.149175361f, 0.690104187f, 0.500000000f, 0.0500000007f

void main()
{

    vec2 uv = V_Texcoord;
	
	float adapted_lum = texture(tex0, vec2(0.5f, 0.5f)).r;        //1x1 lumTex
    float current_lum = texture(tex1, vec2(0.5f, 0.5f)).r + 1e-6;

	current_lum = clamp(adapteParams.x / current_lum, adapteParams.y, adapteParams.z);
	
	float new_adaptation = adapted_lum + (current_lum - adapted_lum) * adapteParams.w;
	new_adaptation = clamp(new_adaptation, 0.01f, 1.0f);
	


	
	gl_FragColor= vec4(new_adaptation,new_adaptation,new_adaptation,1.0f);
	
}