#version 450
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec4 inColor;

layout(location = 0) out vec2 fragUV;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec4 fragColor;

layout( push_constant , std430) uniform pc
{
	mat4 MVP;
};

void main()
{
	fragUV = inUV;
	fragNormal = inNormal;
	fragColor = inColor;
	gl_Position = MVP * vec4(inPosition,1);
}
