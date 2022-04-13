#version 430
layout(location = 0) out vec4 color;
in vec4 color1;

void main() {
	color = color1;
	/* color = vec4(0, 0.3, 0.5, 1); */
}
