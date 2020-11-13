#version 410
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inNormal;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outColor;
layout(location = 2) out vec2 outUV;
layout(location = 3) out vec3 outNormal;

uniform mat4 u_ModelViewProjection;
uniform mat4 u_View;
uniform mat4 u_Model;
uniform mat3 u_ModelRotation;

uniform mat4 Model, View, Projection;
out vec3 color;
out vec2 texUV;
out vec3 world_normal;
out vec3 world_pos;

//uniform mat4 MVP;

void main() {

	gl_Position = u_ModelViewProjection * vec4(inPosition, 1.0);

	outPosition = (u_Model * vec4(inPosition, 1.0)).xyz;
	outUV = inUV;
	outNormal = u_ModelRotation * inNormal;
	outColor = inColor;
}