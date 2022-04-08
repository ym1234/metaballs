#version 430

layout(origin_upper_left) in vec4 gl_FragCoord;
layout(location = 0) out vec4 color;

#define NUM_BALLS 10
layout(location = 1) uniform vec3 Balls[NUM_BALLS];

uniform uint M;
uniform uvec2 Dim;

void main() {
    vec2 sp = gl_FragCoord.xy;

	float sum = 0;
	for (int i = 0; i < NUM_BALLS; i++) {
		vec2 b = vec2(Balls[i].x  * Dim.x, Balls[i].y * Dim.y);
		sum += Balls[i].z / ((b.x - sp.x) * (b.x - sp.x) + (b.y - sp.y) * (b.y - sp.y));
	}

	if (sum > 0.020) {
		/* color = vec4(sum * sum, 0.5 * sum, sum, 1); */
		color = vec4(1, 1, 1, 1);
	} else {
		/* color = vec4(sqrt(sum), 1 - 0.5 * sum, 1 - 0.25 * sum, 1); */
		color = vec4(0, 0, 0, 1);
	}
}
