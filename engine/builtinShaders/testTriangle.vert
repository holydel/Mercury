layout(location = 0) out vec3 fragColor;

void main()
	{
		//const array of positions for the triangle
		const vec2 positions[3] = vec2[3](
			vec2(0.7f, 0.7f),
			vec2(-0.7f, 0.7f),
			vec2(0.0f, -0.7f)
			);

		const vec3 colors[3] = vec3[3](
			vec3(1.0f, 0.0f, 0.0f),
			vec3(0.0f, 1.0f, 0.0f),
			vec3(0.0f, 0.0f, 1.0f)
		
				);
		//output the position of each vertex
		gl_Position = vec4(positions[gl_VertexIndex], 0.0f, 1.0f);
		fragColor = colors[gl_VertexIndex];
	}




