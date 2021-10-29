#version 330 core


in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;


uniform vec3 cameraPos;
uniform vec3 lightDir;


out vec4 FragColor;




void main()
{    

  vec3 L =  normalize(lightDir);
  vec3 N =  normalize(fs_in.Normal);
  
  vec3 V = normalize(cameraPos - fs_in.FragPos);
  vec3 H = normalize(L+V);
  
  float  diff = max(dot(L,N),0.0f);
  float spec = pow(max(dot(N,H),0.0f),32.0);
  
  vec3 diffuseColor = diff*vec3(1.0f);
  vec3 specColor = spec*vec3(0.5f);
  vec3 ambinet = vec3(0.05f);

  vec3 color = ambinet+ (diffuseColor+specColor);
  
  
   FragColor=vec4(color,1.0f);

}