#version 430
layout (location = 0) in vec2 aPos;
#define NUM_BALLS 1
uniform vec3 Balls[NUM_BALLS];
uniform uint M;
uniform uvec2 Dim;

void main(void) {
	float sum = 0;
	for (int i = 0; i < NUM_BALLS; i++) {
		/* vec2 b = vec2(Balls[i].x  * Dim.x, Balls[i].y * Dim.y); */
		vec2 b = vec2(Balls[i]);
		sum += Balls[i].z / sqrt(pow(b.x - aPos.x, 2) + pow(b.y - aPos.y, 2));
	}
    gl_Position = vec4(aPos, sum, 1);
}
