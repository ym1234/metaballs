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

float rand1(void) {
	return (float) rand() / RAND_MAX;
}

float rand2(void) {
	return rand1() - 0.5;
}


unsigned int *cib(int n) {
	unsigned int *v = calloc(n * n * 2 * 3, sizeof(unsigned int));
	unsigned int *z = v;
	for (unsigned int i = 0; i < n ; ++i) {
		for (unsigned int j = 0; j < n; ++j) {
			int x = i * (n + 1) + j;
			int y = i * (n + 1) + j + 1;
			int f = (i + 1) * (n + 1) + j;
			int k = (i + 1) * (n + 1) + j + 1;
			z[0] = x;
			z[1] = f;
			z[2] = k;
			/* printf("(%d, %d, %d) ", i * (n + 1) + j, i * (n + 1) + j + 1, (i + 1) * (n + 1) + j); */
			/* printf("(%d, %d, %d) ", z[0], z[1], z[2]); */
			z[3] = x;
			z[4] = k;
			z[5] = y;
			/* printf("(%d, %d, %d) ", i * (n + 1) + j + 1, (i + 1) * (n + 1) + j, (i + 1) * (n + 1) + j + 1); */
			/* printf("(%d, %d, %d) ", z[3], z[4], z[5]); */
			z += 6;
		}
		/* printf("\n"); */
	}
	return v;
}

float *cv(int n) {
	float step = 2 / (float) n;
	float *v = calloc((n+1) * (n+1) * 2, sizeof(float));
	float *z = v;
	for (float i = -1.; i <= 1.; i += step) {
		for (float j = -1; j <= 1; j += step) {
			/* int idx = 2 * (i * (n + 1) + j); */
			z[0] = i;
			z[1] = j;
			/* printf("(%f, %f) ", z[0], z[1]); */
			z += 2;
		}
		/* printf("\n"); */
	}
	return v;
}

int main(void) {
	Display *d = XOpenDisplay(NULL);
	if (d == NULL) {
		fprintf(stderr, "Cannot open display\n");
		exit(-1);
	}

	/* float vertices[] = { -1., 3, -1., -1., 3., -1 }; */
	int n = 200;
	float *vr = cv(n);
	unsigned int *tvr = cib(n);

	Window win = create_window(d);
	int shaders[3] =  {
		compile_shader("grid.v.glsl", GL_VERTEX_SHADER),
		compile_shader("grid.f.glsl", GL_FRAGMENT_SHADER),
		compile_shader("grid.g.glsl", GL_GEOMETRY_SHADER)
	};
	unsigned int prog = create_program(3, shaders);
	glDeleteShader(shaders[0]);
	glDeleteShader(shaders[1]);
	glDeleteShader(shaders[2]);

	glUseProgram(prog);

	unsigned int VBO, VAO, IBF;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBF);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, (n + 1) * (n + 1) * 2 * sizeof(float), vr, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBF);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, n * n * 2 * 3 * sizeof(unsigned int), tvr, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);
	glFinish();

	Vec wz =  winsize(d, win);

#define NUM_BALLS 30
	float *p = malloc(NUM_BALLS * 3 * sizeof(*p));
	float *v = malloc(NUM_BALLS * 2 * sizeof(*v));

#define M 50
	srand(time(0));
	for (int i = 0; i < NUM_BALLS; ++i) {
		p[i * 3] = rand1();
		p[i * 3 + 1] = rand1();
		p[i * 3 + 2] = rand1() * M;
		/* p[i * 3 + 2] = M; */

		v[i * 2] = rand2();
		v[i * 2 + 1] = rand2();
	}

	glUniform3fv(glGetUniformLocation(prog, "Balls"), NUM_BALLS, p);
	glUniform1ui(glGetUniformLocation(prog, "NUM_BALLS"), NUM_BALLS);
	glUniform1ui(glGetUniformLocation(prog, "M"), M);
	glUniform2ui(glGetUniformLocation(prog, "Dim"), wz.x, wz.y);
	glEnable(GL_MULTISAMPLE);
	XEvent xev;

	struct timeval tv;
	gettimeofday(&tv, NULL);
	double time = tv.tv_sec * 1000.0 + tv.tv_usec / 1000000.0;
	double dt = 0;

	char framerate[100] = {0};
	XSetForeground(d, DefaultGC(d, 0), 0x0000ff00); // red

	glPointSize(10);
	GLenum modes[3] = {GL_POINT, GL_LINE, GL_FILL };
	int stop = 0;
	int step = 0;
	int mdx = 2;
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
					if (wz_t.x == wz.x && wz_t.y == wz.y) {
						continue;
					}
					wz = wz_t;
					glUniform2ui(glGetUniformLocation(prog, "Dim"), wz.x, wz.y);
					glScissor(0, 0, wz.x, wz.y);
					glEnable(GL_SCISSOR_TEST);
					glViewport(0, 0, wz.x, wz.y);
				} break;
				case KeyPress: {
					unsigned int keycode = xev.xkey.keycode;
					if (keycode == 59) {
						mdx = (mdx + 1) % 3;
					} else if (keycode == 60) {
						mdx = (mdx + 2) % 3;
					} else if (keycode == 44 || keycode == 65) {
						stop = !stop;
					} else if (keycode == 47) {
						step = 1;
					} else if (keycode == 46) {
						step = -1;
					} else {
						printf("unhandled keycode: %d\n", keycode);
					}
				} break;
			}
		}
		if (stop) dt = 0;
		if (step) dt = step * 0.1; step = 0;
		for (int i = 0; i < NUM_BALLS; ++i) {
			double vx = v[i * 2], vy = v[i * 2 + 1];
			p[i * 3] += vx * dt;
			p[i * 3 + 1] += vy * dt;

#define X(p, pv, x) \
			if (((p > 1.) && (pv > 0)) || ((p < -1) && (pv < 0))) {\
				v[i * 2 + x] = -pv;\
			}

			X(p[i * 3], vx, 0);
			X(p[i * 3 + 1], vy, 1);
		}

		glUniform3fv(glGetUniformLocation(prog, "Balls"), NUM_BALLS, p);
		glClear(GL_COLOR_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, modes[mdx]);
		glDrawElements(GL_TRIANGLES, n * n * 2 * 3, GL_UNSIGNED_INT, NULL);
		glXSwapBuffers(d, win);
		glFinish();
		XSync(d, 0);

		gettimeofday(&tv, NULL);
		double time2 = tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0;
		dt = (time2 - time) / 1000;
		time = time2;

		XDrawString(d, win, DefaultGC(d, 0), 100, 100, framerate, snprintf(framerate, 100, "Framerate: %f", 1 / dt));
		XSync(d, 0);
	}
}
