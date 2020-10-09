#version 410

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;

uniform vec3 LightPos;
out vec4 frag_color;

void main() {

     // Lecture 5
     vec3 lightColor = vec3(1.0, 1.0, 1.0);

     // Ambient
     float ambientStr = 0.1;
     vec3 ambient = ambientStr * lightColor * inColor;

     //Diffuse
     vec3 norm = normalize(inNormal);
     vec3 lightDir = normalize(LightPos - inPos);
     float dif = max(dot(norm, lightDir), 0.0);
     vec3 diffuse = dif * inColor;

     //Attenuation
     float dist = length(LightPos - inPos);
     diffuse = diffuse / dist;  

     //Specular
     vec3 camPos = vec3(0.0, 0.0, 3.0);
     //float specStr = 1.0;
     //vec3 camDir = normalize(camPos - inPos);
     //vec3 reflectDir = reflect(-lightDir, norm);
     //float spec = pow(max(dot(camDir, reflectDir), 0.0), 128);
     //vec3 specular = specStr * spec * lightColor;

     //Blinn-Phong
     vec3 viewDir = normalize(camPos - inPos);
     vec3 reflectDir = reflect(-lightDir, norm);
     vec3 halfwayDir = normalize(lightDir + viewDir);
     float spec = pow(max(dot(norm, halfwayDir), 0.0), 128);           
     vec3 specular = vec3(1.0) * spec * lightColor;

     vec3 result = (ambient + diffuse + specular);

     frag_color = vec4(result, 1.0);
}