#include<gl/GLUT.H>
#include <iostream>
#include <typeinfo>
#include<math.h>
using namespace std;

struct vector {
	int x;
	int y;
};
int xa = 0, ya = 0;
int xb = 0, yb = 0;
int xd = 0, yd = 0;
int xs = 0, ys = 0;//starting point of polygon
int sx = 0, sy = 0;
int difX = 0, difY = 0;
int midX = 0, midY = 0;
GLfloat x, y;
vector save[25000];
int save_count = 0;
static int winX = 500; //window size X coordinate
static int winY = 500; //window size Y coordinate
static int click_count = 0;
static char mode = '0';

void mouseFunc(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		click_count++;
		xd = x;
		yd = y;
		//cout << click_count << endl;
		//cout << "LB_DOWN x: " << xd << " y: " << yd << endl;
		switch (mode)
		{
		case 'd':
			DrawPoint();
			break;
		case 'l':
			if (click_count % 2 == 1) {
				//DrawPoint();
				//save first point
				xa = xd;
				ya = yd;
			}
			else {
				//save second point
				xb = xd;
				yb = yd;
				drawLine();

				//cout << "<" << xa << " " << ya << ">" << endl;
				//cout << "<" << xb << " " << yb << ">" << endl;
			}
			break;
		case 'p':
			if (click_count == 1) {
				xa = xd;
				ya = yd;
				//save first point
				xs = xd;
				ys = yd;
			}
			else {
				sx = xd, sy = yd;
				xb = xd, yb = yd;
				drawLine();
			}
			break;
		case 'o':
			xa = xd;
			ya = yd;

			break;
		}
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		xd = x;
		yd = y;
		//cout << "LB_UP x: " << xd << " y: " << yd << endl;
		if (mode == 'o') {
			click_count++;
			xb = xd;
			yb = yd;
			drawCircle();
		}
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		if (mode == 'p') {
			xb = xs;
			yb = ys;
			drawLine();
			click_count = 0;
		}
	}

}


void keyboardFunc(unsigned char c, int x, int y)
{
	switch (c)
	{
	case 27:
		std::cout << c << endl;
		break;
	case 'd'://draw point
		mode = 'd';
		click_count = 0;
		break;
	case 'l'://draw line
		mode = 'l';
		click_count = 0;
		break;
	case 'p'://draw polygon
		mode = 'p';
		click_count = 0;
		break;
	case 'o'://draw circle
		mode = 'o';
		click_count = 0;
		break;
	case 'c'://clear
		click_count = 0;
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glFlush();
		break;
	case 'r'://recover
		cout << "recover" << endl;
		redraw();
		break;
	case 'q'://quit
		exit(1);
		break;
	}
}

void savePixel(int x, int y) {
	save_count++;
	save[save_count].x = x;
	save[save_count].y = y;
}

void horizontal() {
	int a = xa;
	xa = xb;
	xb = a;
}

void changexy() {
	int a = ya, b = yb;
	ya = xa;
	xa = a;
	yb = xb;
	xb = b;
}

void turn(int state) {
	if (state == 2) {
		horizontal();
	}
	else if (state == 3) {
		changexy();
		horizontal();
	}
	else if (state == 4) {
		changexy();
	}
}

void turnBack(int state) {//xd yd
	if (state == 2) {
		if (xd < midX) {
			xd -= 2 * (xd - midX);
		}
		else {
			xd += 2 * (midX - xd);
		}
	}
	else if (state == 3) {

		if (xd < midX) {
			xd -= 2 * (xd - midX);
		}
		else {
			xd += 2 * (midX - xd);
		}
		int s = xd;
		xd = yd;
		yd = s;

	}
	else if (state == 4) {
		int s = xd;
		xd = yd;
		yd = s;
	}

}

int rotate() {  //turn xa, ya xb, yb
	int state = 0;
	float a = xa, b = ya, c = xb, d = yb;
	float m = (d - b) / (c - a);
	//cout << "m = " << m << endl;
	if (m >= 0 && m <= 1) {
		state = 1;
	}
	else if (m < 0 && m > -1) {
		state = 2;
	}
	else if (m <= -1) {
		state = 3;
	}
	else if (m > 1) {
		state = 4;
	}
	//cout << "*" << state << "*";
	return state;
}

GLfloat trans(int p) {
	float pf = p;
	GLfloat A;
	A = 2 * pf / winX - 1;
	return A;
}

void DrawPoint() //draw (xd, yd)
{
	x = trans(xd);
	y = 0 - trans(yd);
	glBegin(GL_POINTS);
	glVertex3f(x, y, 0.0);
	glEnd();
	glFlush();

}

void drawLine() { //draw(xa, ya)(xb, yb)
	//cout << "#<" << xa << " " << ya << ">";
	//cout << "#<" << xb << " " << yb << ">";
	if (mode != 'o') {
		glBegin(GL_POINTS);
		savePixel(xd, yd);
		x = trans(xd);
		y = 0 - trans(yd);
		glVertex3f(x, y, 0.0);

		difX = abs(xa - xb);
		difY = abs(ya - yb);

		int state = rotate();
		turn(state);
		if (xa > xb && ya > yb) {
			int save = xa;
			xa = xb;
			xb = save;
			save = ya;
			ya = yb;
			yb = save;
		}

		midX = (xa + xb) / 2;
		midY = (ya + yb) / 2;

		//draw line alg
		int x_ = xa, y_ = ya;
		int a = yb - ya, b = xa - xb;
		int d = (a + b) / 2;

		while (x_ <= xb) {
			if (d <= 0) {
				x_++;
				d += a;
			}
			else {
				x_++;
				y_++;
				d += (a + b);
			}

			xd = x_;
			yd = y_;
			turnBack(state);

			savePixel(xd, yd);
			x = trans(xd);
			y = 0 - trans(yd);
			glVertex3f(x, y, 0.0);
		}
	}
	if (mode == 'p') {
		xa = sx;
		ya = sy;
	}
	glEnd();
	glFlush();
}

double distance() {
	int f = xa - xb;
	int s = ya - yb;
	double a = f * f + s * s;
	a = sqrt(a);
	//cout << "R':" << a;
	return a;
}

void drawCircle() {
	if (click_count % 2 == 0) {

		glBegin(GL_POINTS);
		int R = distance();
		//cout << "R:" << R << endl;
		int a = 0, b = R;
		int incE = 3;
		int incSE = -2 * R + 5;
		int d = 1 - R;

		//plot
		savePixel(xd, yd);
		xd = xa + a, yd = ya + b;
		x = trans(xd);
		y = 0 - trans(yd);
		glVertex3f(x, y, 0.0);

		savePixel(xd, yd);
		xd = xa + a, yd = ya - b;
		x = trans(xd);
		y = 0 - trans(yd);
		glVertex3f(x, y, 0.0);

		savePixel(xd, yd);
		xd = xa + b, yd = ya + a;
		x = trans(xd);
		y = 0 - trans(yd);
		glVertex3f(x, y, 0.0);

		savePixel(xd, yd);
		xd = xa - b;
		x = trans(xd);
		glVertex3f(x, y, 0.0);
		while (a < b) {
			if (d < 0) {
				d += incE;
				incE += 2;
				incSE += 2;
				a++;
			}
			else {
				d += incSE;
				incE += 2;
				incSE += 4;
				a++;
				b--;
			}
			xd = xa + a, yd = ya + b;
			savePixel(xd, yd);
			x = trans(xd);
			y = 0 - trans(yd);
			glVertex3f(x, y, 0.0);

			xd = xa - a;
			savePixel(xd, yd);
			x = trans(xd);
			glVertex3f(x, y, 0.0);

			xd = xa + a, yd = ya - b;
			savePixel(xd, yd);
			x = trans(xd);
			y = 0 - trans(yd);
			glVertex3f(x, y, 0.0);

			xd = xa - a, yd = ya - b;
			savePixel(xd, yd);
			x = trans(xd);
			y = 0 - trans(yd);
			glVertex3f(x, y, 0.0);
			//
			xd = xa + b, yd = ya + a;
			savePixel(xd, yd);
			x = trans(xd);
			y = 0 - trans(yd);
			glVertex3f(x, y, 0.0);

			xd = xa - b;
			savePixel(xd, yd);
			x = trans(xd);
			glVertex3f(x, y, 0.0);

			xd = xa + b, yd = ya - a;
			savePixel(xd, yd);
			x = trans(xd);
			y = 0 - trans(yd);
			glVertex3f(x, y, 0.0);

			xd = xa - b, yd = ya - a;
			savePixel(xd, yd);
			x = trans(xd);
			y = 0 - trans(yd);
			glVertex3f(x, y, 0.0);
		}

		glEnd();
		glFlush();
	}

}

void redraw() {
	int i = 0;
	for (i = 0; i < save_count; i++) {
		xd = save[i].x;
		yd = save[i].y;
		x = trans(xd);
		y = 0 - trans(yd);
		glBegin(GL_POINTS);
		glVertex3f(x, y, 0.0);
		glEnd();
		glFlush();
	}
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);//initialize the GLUT library.
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);//Bit mask to select a single buffered window. 
	glutInitWindowSize(500, 500); //int width , int height
	glutInitWindowPosition(1000, 500);//Window X, Y location in pixels.(int x, int y)
	glutCreateWindow("110522060");//creates a top-level window.
	glPointSize(3);//specify the diameter of rasterized points
	glColor3f(1.0f, .0f, 0.0f);//set the current color
	glutKeyboardFunc(keyboardFunc);// sets the keyboard callback for the current window.
	glutMouseFunc(mouseFunc);//sets the mouse callback for the current window.
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);//specify clear values for the color buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//clear buffers to preset values
	glFinish();//block until all GL execution is complete
	glutMainLoop();//enters the GLUT event processing loop. This routine should be called at most once in a GLUT program. 

	return 0;
}