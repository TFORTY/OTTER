#version 410

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inNormal;

uniform vec3 light_pos;
uniform vec3 eye_pos;

out vec4 frag_color;

uniform sampler2D myTextureSampler;

const float lightIntensity = 10.0;

//Lecture 10c - Toon Shading
const int bands = 3;
const float scaleFactor = 1.0 / bands;

void main() {
	
	vec3 L = normalize(light_pos - inPos);
	vec3 V = normalize(eye_pos - inPos);

	float dist = length(light_pos - inPos);

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