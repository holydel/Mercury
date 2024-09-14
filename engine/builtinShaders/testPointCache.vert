layout( push_constant ) uniform constants
{
	mat4 MVP;
	vec4 cameraPosAndTime;
} PushConstants;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 outColor;
void main()
	{
		//output the position of each vertex
		outColor = inColor;
		gl_Position = PushConstants.MVP * vec4(inPosition,1.0);
		float pointsize = 8.0f;
		
		gl_PointSize = pointsize - (distance(PushConstants.cameraPosAndTime.xyz, inPosition.xyz) / pointsize);
	}










