#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightPos;
uniform vec3 planeCenter;
uniform vec3 planeNormal;

void main()
{
    
	vec3 wPos  =  vec3 (model * vec4(aPos,1.0f));

    vec3 lightDir = normalize(lightPos); 
    float dist = dot(planeCenter - wPos, planeNormal) / dot(lightDir, planeNormal);
    wPos = wPos + lightDir * dist;
 
	
	wPos.y+=0.01f;
	
    gl_Position = projection * view * vec4(wPos, 1.0);
}