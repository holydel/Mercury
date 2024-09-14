#version 450

layout(push_constant) uniform constants
{
	vec4 geometry;
	vec4 color;
};


layout(location = 0) out vec4 outColor;

void main() {
    outColor = color;
}


