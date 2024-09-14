#version 450

layout( push_constant , std430) uniform pc
{
	vec4 geometry;
	vec4 color;
};


void main()
{
	float sx = geometry.x - geometry.z;
	float ex = geometry.x + geometry.z;
	float sy = geometry.y - geometry.w;
	float ey = geometry.y + geometry.w;
	vec2 positions[] = 
	{
	vec2(sx,sy),
	vec2(ex,sy),
	vec2(sx,ey),
	vec2(ex,ey)
	};
	
	gl_Position = vec4(positions[gl_VertexIndex],0,1);
}


