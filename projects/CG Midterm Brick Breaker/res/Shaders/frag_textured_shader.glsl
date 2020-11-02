#version 410
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inNormal;

uniform vec3  u_AmbientCol;
uniform float u_AmbientStrength;

uniform vec3  u_LightPos;
uniform vec3  u_LightCol;
uniform float u_AmbientLightStrength;
uniform float u_SpecularLightStrength;
uniform float u_Shininess;

//TEXTURE STUFF
uniform sampler2D s_Diffuse;
uniform sampler2D s_Specular;

uniform float u_LightAttenuationConstant;
uniform float u_LightAttenuationLinear;
uniform float u_LightAttenuationQuadratic;
/////////////////////////////////////////////

uniform vec3  u_CamPos;

out vec4 frag_color;

void main() { 
	
	vec3 ambient = ((u_AmbientLightStrength * u_LightCol) + (u_AmbientCol * u_AmbientStrength));

	// Diffuse
	vec3 N = normalize(inNormal);
	vec3 lightDir = normalize(u_LightPos - inPos);
	
	float dif = max(dot(N, lightDir), 0.0);
	vec3 diffuse = dif * u_LightCol;// add diffuse intensity

	//Attenuation
	float dist = length(u_LightPos - inPos);
	//diffuse = diffuse / dist; // (dist*dist)
	//TEXTURE STUFF
	float attenuation = 1.0f / (
		u_LightAttenuationConstant + 
		u_LightAttenuationLinear * dist +
		u_LightAttenuationQuadratic * dist * dist);
	
	// Specular
	vec3 viewDir = normalize(u_CamPos - inPos);
	vec3 h		 = normalize(lightDir + viewDir);

	//TEXTURE STUFF
	float texSpec = texture(s_Specular, inUV).x;
	
	float spec = pow(max(dot(N, h), 0.0), u_Shininess);
	//vec3 specular = u_SpecularLightStrength * spec * u_LightCol; 

	//TEXTURE STUFF
	vec3 specular = u_SpecularLightStrength * texSpec * spec * u_LightCol;

	vec4 textureColor = texture(s_Diffuse, inUV);

	vec3 result = (
	(u_AmbientCol * u_AmbientStrength) + 
	(ambient + diffuse + specular) * attenuation
	) * inColor * textureColor.rgb;

	frag_color = vec4(result, textureColor.a);

	//vec3 result = (ambient + diffuse + specular) * inColor;

	//frag_color = vec4(result, 1.0); 
}