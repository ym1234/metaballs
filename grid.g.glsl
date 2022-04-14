#version 430
layout (triangles) in;
layout (triangle_strip, max_vertices = 32) out;
out vec4 color1;

#define THRESHOLD 1
float u(float a, float b, float c) {
	return (c - a) / (b - a);
}

void main(void) {
	color1 = vec4(0.2,0.2,0.2,1);
	int x = 0;

	int y = 0;
	int o = 0;
	int a = 0;

	for (int i = 0; i < 3; i++) {
		/* for (int i = 0; i < 3; ++i) { */
			gl_Position = gl_in[i].gl_Position * vec4(1, 1, 0, 1);
			EmitVertex();
		/* } */
		if (gl_in[i].gl_Position.z >= THRESHOLD) {
			++x;
			o = y;
			y = i;
		} else {
			a = i;
		}
	}
	EndPrimitive();


	if (x == 1) {
		color1 = vec4(0, 0.5, 1, 1);
		vec4 v = gl_in[y].gl_Position;
		vec4 f = gl_in[int(mod((y - 1), 3))].gl_Position;
		vec4 s = gl_in[(y + 1) % 3].gl_Position;

		gl_Position = v * vec4(1, 1, 0, 1);
		EmitVertex();
		gl_Position = mix(v, f, u(v.z, f.z, THRESHOLD)) * vec4(1, 1, 0, 1);
		EmitVertex();
		gl_Position = mix(v, s, u(v.z, s.z, THRESHOLD)) * vec4(1, 1, 0, 1);
		EmitVertex();
		EndPrimitive();
	} else if (x == 2) {
		color1 = vec4(0, 0.2, 0.6, 1);
		vec4 v = gl_in[y].gl_Position;
		vec4 k = gl_in[o].gl_Position;
		vec4 f = gl_in[a].gl_Position;

		vec4 c = mix(v, f, u(v.z, f.z, THRESHOLD));
		vec4 h = mix(k, f, u(k.z, f.z, THRESHOLD));

		gl_Position = v * vec4(1, 1, 0, 1);
		EmitVertex();
		gl_Position = c * vec4(1, 1, 0, 1);
		EmitVertex();
		gl_Position = k * vec4(1, 1, 0, 1);
		EmitVertex();
		gl_Position = h * vec4(1, 1, 0, 1);
		EmitVertex();
		EndPrimitive();
	} else if (x == 3)  {
		color1 = vec4(0, 0.5, 0.5, 1);
		for (int i = 0; i < 3; i++) {
			gl_Position = gl_in[i].gl_Position * vec4(1, 1, 0, 1);
			EmitVertex();
		}
		EndPrimitive();
	}
}
