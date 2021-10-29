#version 330 core

in vec2 Texcoord;

uniform sampler2D tex;
uniform vec4 offset1;
uniform vec4 offset2;

void main()
{
    vec2 uv = Texcoord;
    
	vec4 color0 = texture(tex,uv+offset1.xy);
	vec4 color1 = texture(tex,uv+offset1.zw);
	vec4 color2 = texture(tex,uv+offset2.xy);
	vec4 color3 = texture(tex,uv+offset2.zw);
	
	vec4 color = (color0+color1+color2+color3)*0.25f;

	gl_FragColor = color;
	
}