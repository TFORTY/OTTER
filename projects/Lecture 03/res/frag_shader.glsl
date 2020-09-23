#version 410

layout(location = 1) in vec3 inColor;

out vec4 frag_col;

void main()
{
	frag_col = vec4(inColor, 1.0);
}