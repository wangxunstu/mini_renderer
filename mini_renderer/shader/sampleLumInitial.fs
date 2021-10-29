#version 330 core

in vec2 Texcoord;

uniform sampler2D sampleInitialtex;
uniform vec4 offset1;
uniform vec4 offset2;

void main()
{
    vec2 uv = Texcoord;
    
	vec3 cof = vec3(0.2126f,0.7152f,0.0722f);
	
	float lum = 0.0f;
	
	lum+= dot(pow(texture(sampleInitialtex,uv + offset1.xy).rgb,vec3(2.2f)),cof);
	lum+= dot(pow(texture(sampleInitialtex,uv + offset1.zw).rgb,vec3(2.2f)),cof);
	lum+= dot(pow(texture(sampleInitialtex,uv + offset2.xy).rgb,vec3(2.2f)),cof);
	lum+= dot(pow(texture(sampleInitialtex,uv + offset2.zw).rgb,vec3(2.2f)),cof);
    
	lum = min(lum*0.25f,64.0f);

	gl_FragColor = vec4(lum,lum,lum,1.0f);
	
}