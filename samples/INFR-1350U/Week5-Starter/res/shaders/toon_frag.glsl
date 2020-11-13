#version 410

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inNormal;

uniform vec3 u_LightPos;
uniform vec3 u_CamPos;

out vec4 frag_color;

uniform sampler2D myTextureSampler;

const float lightIntensity = 10.0;

//Lecture 10c - Toon Shading
const int bands = 3;
const float scaleFactor = 1.0 / bands;

void main() {
	
	vec3 L = normalize(u_LightPos - inPos);
	vec3 V = normalize(u_CamPos - inPos);

	float dist = length(u_LightPos - inPos);

	float diffuse = max(0, dot(L, inNormal));
	vec3 diffuseOut = (diffuse * inColor) / (dist*dist);
	
	diffuseOut = diffuseOut*lightIntensity;

	//Lecture 10c
	diffuseOut = floor(diffuseOut * bands) * scaleFactor;

	//Outline Effect
	float edge = (dot(V, inNormal) < 0.4) ? 0.0 : 1.0;

	vec3 result = diffuseOut * edge;
	
	frag_color = vec4(result, 1.0);
}