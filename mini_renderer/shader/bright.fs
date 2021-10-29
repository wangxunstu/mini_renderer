#version 330 core

in vec2 Texcoord;
uniform sampler2D brightTex;

uniform vec4 brightParams;
//2.00000000f, 3.00000000f, 1.00000000f,0.00100000005f

void main()
{
    vec2 uv = Texcoord;
	
	
	vec4 vSample = texture(brightTex, uv);
	
	vSample *= brightParams.z;
	
	vSample.rgb = (vSample.r > 10000.0f) ? vec3(1, 1, 1) : vSample.rgb;
	
	vSample.rgb *= vec3(brightParams.x);
	vSample.rgb -= vec3(brightParams.y);
	vSample.rgb = max(vSample.rgb, vec3(0.0f));  
	
	vSample.rgb /= (vec3(brightParams.w) + vSample.rgb);
	
	vSample *= 8.0;
	
	
	gl_FragColor=vec4(vSample.rgb,1.0f);
	
}