#version 330 core

out vec4 FragColor;

out vec3 WorldPos;

uniform samplerCube cubemap;

void main()
{
   
   FragColor = texture(cubemap,WorldPos);
  
}