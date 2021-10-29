#version 330 core

in vec2 V_Texcoord;
uniform sampler2D U_Texture;
void main()
{
	vec4 color =texture2D(U_Texture,V_Texcoord);
	
	gl_FragColor=vec4(color.rgb,1.0f);
	
}