#version 330 core

in vec2 Texcoord;

uniform sampler2D sceneTex;
uniform sampler2D lumTex;
uniform sampler2D bloom0;
uniform sampler2D bloom1;
uniform sampler2D bloom2;

uniform vec4 toneMappingParam;
uniform vec4 toneMappingCurve;
uniform vec4 colorBalance;

out vec4 FragColor;

void main()
{
	
	float A = toneMappingCurve.x;
	float B = toneMappingCurve.y;
	float C = toneMappingCurve.z;
	float D = toneMappingCurve.w;
	float E = toneMappingParam.w;
	
	vec2 uv = Texcoord;
	
	vec4 sceneColor = pow(texture(sceneTex, uv), vec4(2.2f));
	
	float exposure =  texture(lumTex, vec2(0.5f, 0.5f)).r;
	
	vec3 bloom = texture(bloom0, uv).rgb * 0.5f;
	bloom += texture(bloom1, uv).rgb * 0.3f;
	bloom += texture(bloom2, uv).rgb * 0.2f;
	
	sceneColor.rgb += bloom.rgb * toneMappingParam.zzz;
	
	exposure = mix(toneMappingParam.x, exposure, toneMappingParam.y);
	
	sceneColor *= exposure;
	sceneColor.rgb *= colorBalance.rgb;
	
	sceneColor = (sceneColor * (A * sceneColor + B)) / (sceneColor * (C * sceneColor + D) + E);
	
	vec3 color = pow(sceneColor.rgb, vec3(0.45454f));
	
	FragColor = vec4(color.rgb,1.0f);
	
}