#version 430
layout (triangles) in;
layout (triangle_strip, max_vertices = 12) out;
out vec4 color1;

void main(void) {
	color1 = vec4(0.2,0.2,0.2,1);
	int x = 0;

	int y = 0;
	int o = 0;
	int a = 0;

	for (int i = 0; i < 3; i++) {
		gl_Position = gl_in[i].gl_Position * vec4(1, 1, 0, 1);
		EmitVertex();
		if (gl_in[i].gl_Position.z >= 1.) {
			++x;
			o = y;
			y = i;
		} else {
			a = i;
		}
	}
	EndPrimitive();

	if (x == 1) {
		color1 = vec4(0, 0.7, 1, 1);
		vec4 v = gl_in[y].gl_Position;
		vec4 f = gl_in[o].gl_Position;
		vec4 s = gl_in[(y + 1) % 3].gl_Position;

		gl_Position = v * vec4(1, 1, 0, 1);
		EmitVertex();
		gl_Position = mix(v, f, (1 - v.z) / (f.z - v.z)) * vec4(1, 1, 0, 1);
		EmitVertex();
		gl_Position = mix(v, s, (1 - v.z) / (s.z - v.z)) * vec4(1, 1, 0, 1);
		EmitVertex();
		EndPrimitive();
	} else if (x == 2) {
		color1 = vec4(0, 0.7, 1, 1);
		vec4 v = gl_in[y].gl_Position;
		vec4 k = gl_in[o].gl_Position;
		vec4 f = gl_in[a].gl_Position;

		vec4 c = mix(v, f, (1 - v.z) / (f.z - v.z));
		vec4 h = mix(k, f, (1 - k.z) / (f.z - k.z));

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
		color1 = vec4(0, 0.7, 1, 1);
		for (int i = 0; i < 3; i++) {
			gl_Position = gl_in[i].gl_Position * vec4(1, 1, 0, 1);
			EmitVertex();
		}
		EndPrimitive();
	}
}
