#version 330 core

in vec2 Texcoord;

uniform sampler2D sampleIterativeTex;
uniform vec4 offset1;
uniform vec4 offset2;

void main()
{
    vec2 uv = Texcoord;
	
	float lum = 0.0f;
	
	lum+= texture(sampleIterativeTex,uv+offset1.xy).r;
	lum+= texture(sampleIterativeTex,uv+offset1.zw).r;
	lum+= texture(sampleIterativeTex,uv+offset2.xy).r;
	lum+= texture(sampleIterativeTex,uv+offset2.zw).r;
    
	lum = min(lum*0.25f,64.0f);

	gl_FragColor = vec4(lum,lum,lum,1.0f);
	
}