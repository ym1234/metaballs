#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "ogl.c"

typedef struct Vec {
	unsigned int x;
	unsigned int y;
} Vec;

Vec winsize(Display *d, Window win) {
	XWindowAttributes gwa;
	XGetWindowAttributes(d, win, &gwa);
	return (Vec) { gwa.width, gwa.height };
}

float rand1() {
	return (float) rand() / RAND_MAX;
}

float rand2() {
	return rand1() - 0.5;
}

int main(void) {
	Display *d = XOpenDisplay(NULL);
	if (d == NULL) {
		fprintf(stderr, "Cannot open display\n");
		exit(-1);
	}
	Window win = create_window(d);
	int shaders[2] =  { compile_shader("grid.v.glsl", GL_VERTEX_SHADER), compile_shader("grid.f.glsl", GL_FRAGMENT_SHADER)};
	unsigned int prog = create_program(2, shaders);
	glDeleteShader(shaders[0]);
	glDeleteShader(shaders[1]);

	float vertices[] = { -1., 3, -1., -1., 3., -1 };

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glUseProgram(prog);
	glBindVertexArray(VAO);

	Vec wz =  winsize(d, win);

#define NUM_BALLS 10
	float *p = calloc(NUM_BALLS * 3, sizeof(*p));
	float *v = calloc(NUM_BALLS * 2, sizeof(*v));

#define M 500
	srand(time(0));
	for (int i = 0; i < NUM_BALLS; ++i) {
		p[i * 3] = rand1();
		p[i * 3 + 1] = rand1();
		p[i * 3 + 2] = rand1() * M;

		v[i * 2] = rand2();
		v[i * 2 + 1] = rand2();
	}

	glUniform3fv(1, NUM_BALLS, p);
	glUniform1ui(glGetUniformLocation(prog, "M"), M);
	glUniform2ui(glGetUniformLocation(prog, "Dim"), wz.x, wz.y);
	/* glEnable(GL_MULTISAMPLE); */

	XEvent xev;

	struct timeval tv;
	gettimeofday(&tv, NULL);
	double time = tv.tv_sec * 1000.0 + tv.tv_usec / 1000000.0;
	double dt = 0;

	while (1) {
		while (XPending(d)) {
			XNextEvent(d, &xev);
			switch (xev.type) {
				case ClientMessage: {
					if ((Atom)xev.xclient.data.l[0] == atom) {
						exit(0);
					}
				} break;
				case ConfigureNotify: { // TODO
					Vec wz_t = winsize(d, win);
					if (wz.x == wz.x && wz.y == wz.y) {
						continue;
					}
					wz = wz_t;
					/* printf("%d, %d\n", wz.x, wz.y); */
					glUniform2ui(glGetUniformLocation(prog, "Dim"), wz.x, wz.y);
					glScissor(0, 0, wz.x, wz.y);
					glEnable(GL_SCISSOR_TEST);
					glViewport(0, 0, wz.x, wz.y);
				} break;
			}
		}

		for (int i = 0; i < NUM_BALLS; ++i) {
			double vx = v[i * 2], vy = v[i * 2 + 1];
			p[i * 3] += vx * dt;
			p[i * 3 + 1] += vy * dt;

			double px = p[i * 3];
			double py = p[i * 3 + 1];
			if (((px >= 1.) && (vx > 0)) || ((px <= 0) && (vx < 0))) {
				v[i * 2] = -vx;
			}
			if (((py >= 1.) && (vy > 0)) || ((py <= 0) && (vy < 0))) {
				v[i * 2 + 1]  = -vy;
			}
			/* printf("%lf, %lf, %lf, %lf, %lf\n", dt, p[i * 3], p[i * 3 + 1], v[i * 2], v[i * 2 + 1]); */
		}
		glUniform3fv(1, NUM_BALLS, p);

		glDrawArrays(GL_TRIANGLES, 0, 3);
		glXSwapBuffers(d, win);
		glFinish();

		gettimeofday(&tv, NULL);
		double time2 = tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0;
		dt = (time2 - time) / 1000;
		time = time2;
	}
}
