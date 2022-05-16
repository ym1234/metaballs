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

long long get_time() {
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts.tv_sec * 1e9 + ts.tv_nsec;
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
	int n = 100;
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

#define NUM_BALLS 20
	float *p = malloc(NUM_BALLS * 3 * sizeof(*p));
	float *v = malloc(NUM_BALLS * 2 * sizeof(*v));

#define M 100
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
	XEvent xev;

	long long time = get_time();
	double dt = 0;

	XSetForeground(d, DefaultGC(d, 0), 0x00ffff00); // green
	XSetBackground(d, DefaultGC(d, 0), 0x00000000); // green

	glPointSize(10);
	GLenum modes[3] = { GL_POINT, GL_LINE, GL_FILL };
	char *modes_s[3] = { "POINT", "LINE", "FILL" };
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
					switch (keycode) {
						case 59: mdx = (mdx + 1) % 3; glPolygonMode(GL_FRONT_AND_BACK, modes[mdx]); break;
						case 60: mdx = (mdx + 2) % 3; glPolygonMode(GL_FRONT_AND_BACK, modes[mdx]); break;
						case 44:
						case 65: stop = !stop; break;
						case 46: stop = 1; step =  1; break;
						case 47: stop = 1; step = -1; break;
						default: printf("unhandled keycode: %d\n", keycode); break;

					}
				} break;
			}
		}

		if (stop) dt = 0;
		if (step) dt = step * 0.01; step = 0;
		for (int i = 0; i < NUM_BALLS; ++i) {
#define X(x) \
			do {\
				float pp = p[i * 3 + x], pv = v[i * 2 + x];\
				p[i * 3 + x] += pv * dt; \
				if ((pp > 1. && dt * pv > 0) || (pp < -1 && dt * pv < 0)) {\
					v[i * 2 + x] = -pv;\
				}\
			} while(0);

			X(0);
			X(1);
		}

		glUniform3fv(glGetUniformLocation(prog, "Balls"), NUM_BALLS, p);
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawElements(GL_TRIANGLES, n * n * 2 * 3, GL_UNSIGNED_INT, NULL);

		glFinish();
		glXSwapBuffers(d, win);

		long long time2 = get_time();
		dt = (time2 - time) / 1e9;
		time = time2;

		char framerate[100] = {0};
		XDrawImageString(d, win, DefaultGC(d, 0), 0, 100, framerate, snprintf(framerate, 100, "Framerate: %f", 1 / dt));
		XDrawImageString(d, win, DefaultGC(d, 0), 0, 115, framerate, snprintf(framerate, 100, "Mode: GL_%s", modes_s[mdx]));
		XDrawImageString(d, win, DefaultGC(d, 0), 0, 130, framerate, snprintf(framerate, 100, "Stop: %s", stop ? "true" : "false"));
		XSync(d, 0);
	}
}
