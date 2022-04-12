#version 430
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

void main(void) {
	int x = 0;
	for (int i = 0; i < 3; i++) {
		/* x += int(gl_in[0].gl_Position.z < 0.3 && gl_in[0].gl_Position.z > 0.29 ); */
		x += int(gl_in[0].gl_Position.z < 0.3);
	}
	if (x <= 2) {
		gl_Position = gl_in[0].gl_Position.xyww;
		gl_Position.z = 0;
		EmitVertex();
		gl_Position = gl_in[1].gl_Position.xyww;
		gl_Position.z = 0;
		EmitVertex();
		gl_Position = gl_in[2].gl_Position.xyww;
		gl_Position.z = 0;
		EmitVertex();
		EndPrimitive();
	}
}
