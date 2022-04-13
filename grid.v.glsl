#version 430

#define MAX_BALLS 1000

layout (location = 0) in vec2 aPos;
uniform vec3 Balls[MAX_BALLS];
uniform uint NUM_BALLS;
uniform uint M;
uniform uvec2 Dim;

void main(void) {
	float sum = 0;
	vec2 k = aPos * Dim;
	for (int i = 0; i < NUM_BALLS; i++) {
		vec2 b = vec2(Balls[i]) * Dim;
		sum += Balls[i].z / sqrt(pow(b.x - k.x, 2) + pow(b.y - k.y, 2));
	}
    gl_Position = vec4(aPos, sum, 1);
}
