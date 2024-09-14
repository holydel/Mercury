layout(location = 0) in vec3 inColor;
layout(location = 0) out vec4 outColor;

void main() {
	float b = 1.0 - clamp(length(gl_PointCoord.xy * 2.0 - vec2(1.0f,1.0f)),0.0f,1.0f);
	
    outColor = vec4(inColor * b, 1.0);
}





