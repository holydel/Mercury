layout(location = 0) in vec3 fragColor;
layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(fragColor+vec3(0.2,0.3,0.4), 1.0);
}
