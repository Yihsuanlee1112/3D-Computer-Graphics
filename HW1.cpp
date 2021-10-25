#include <GL/glut.h>
#include <GL/gl.h>
#include <iostream>
using namespace std;

void display() {
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();
}

void drawSquare(int x, int y) {
	glPointSize(10);
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POINTS);
	glVertex2i(x, glutGet(GLUT_WINDOW_HEIGHT) - y);
	glEnd();
	glutSwapBuffers();
}

void mouse(int bin, int state, int x, int y) {
	if (bin == GLUT_LEFT_BUTTON && state == GLUT_DOWN) drawSquare(x, y);
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	gluOrtho2D(0, 500, 0, 500);
	glutCreateWindow("Your First GLUT  Window!");

	glutDisplayFunc(display);
	glutMouseFunc(mouse);
	gluOrtho2D(0, 500, 0, 500);
	glutSwapBuffers();
	glutMainLoop();
}