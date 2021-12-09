// 2021CG_Lab2_110522060.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
//
#include <GL/glut.h>
#include <GL/gl.h>
#include<iostream>
#include <list>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>     
#include <stdlib.h>

#define _USE_MATH_DEFINES
#include <cmath>
#include<math.h>
#include <tuple>
#include <iomanip>
using namespace std;
class point
{
private:
	int x = 0;
	int y = 0;
public:
	point(int _x, int _y) {
		x = _x;
		y = _y;
	}
	int getX() {
		return x;
	}
	int getY() {
		return y;
	}
	void setX(int new_x) {
		x = new_x;
	}
	void setY(int new_y) {
		y = new_y;
	}
};
class pointInDouble
{
private:
	double x;
	double y;
public:
	pointInDouble
	(double _x, double _y) {
		x = _x;
		y = _y;
	};
	double getX() {
		return x;
	};
	double getY() {
		return y;
	}
	void setX(int new_x) {
		x = new_x;
	}
	void setY(int new_y) {
		y = new_y;
	}

};
ifstream FileInput;
vector<pair<pointInDouble, pointInDouble>> windowSidePair;
vector<pair<point, point>> viewSidePair;
list<point> pointList;
list<point> pointListBackUp;
point lineTemp = point(NULL, NULL);
int lineCounter = 0;
int squareCounter = 0;
int triCounter = 0;
string fileName = "";

static double defaultMatrix[3][3] = { 1.0, 0.0, 0.0,
									  0.0, 1.0, 0.0,
									  0.0, 0.0, 1.0 };
static double transfoMat[3][3] = { 1.0, 0.0, 0.0,
											 0.0, 1.0, 0.0,
											 0.0, 0.0, 1.0 };
static double squareMat[3][4] = { -1.0,-1.0, 1.0, 1.0,
									   -1.0, 1.0, 1.0,-1.0,
										1.0, 1.0 ,1.0, 1.0 };
static double triMat[3][3] = { 0.0,-1.0, 1.0,
									   1.0,-1.0,-1.0,
									   1.0, 1.0, 1.0 };
//clipping use
typedef int OutCode;
const int INSIDE = 0; // 0000
const int LEFT = 1;   // 0001
const int RIGHT = 2;  // 0010
const int BOTTOM = 4; // 0100
const int TOP = 8;    // 1000
OutCode computeOutCode(double x, double y, double vxl, double vxr, double vyb, double vyt)
{
	OutCode code;

	code = INSIDE;          // initialised as being inside of [[clip window]]

	if (x < vxl)           // to the left of clip window
		code |= LEFT;
	else if (x > vxr)      // to the right of clip window
		code |= RIGHT;
	if (y < vyb)           // below the clip window
		code |= BOTTOM;
	else if (y > vyt)      // above the clip window
		code |= TOP;

	return code;
}
void matrixOutput(double matrix[3][3]) {
	for (int i = 0; i < 3; i++)
	{
		cout << "{";
		for (int j = 0; j < 2; j++) {
			cout << setw(10) << matrix[i][j] << ",";
		}
		cout << setw(10) << matrix[i][2] << "}" << endl;
	}
}
double angleToRadian(double angle) {
	return (angle * M_PI / (double)180.0);
}


void calRotateTMMulTM(double angleTransfoMat[3][3]) {
	double temp[3][3];
	double temp2[3][3];

	pair<double, double> translateTemp = { transfoMat[0][2], transfoMat[1][2] };


	double Tneg[3][3] = { 1.0, 0.0, -1 * translateTemp.first,
							   0.0, 1.0, -1 * translateTemp.second,
							   0.0, 0.0, 1.0 };
	double T[3][3] = { 1.0, 0.0, translateTemp.first,
					   0.0, 1.0, translateTemp.second,
					   0.0, 0.0, 1.0 };

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			temp2[i][j] = 0;
			for (int k = 0; k < 3; k++) {
				temp2[i][j] += angleTransfoMat[i][k] * transfoMat[k][j];
			}
		}
	}
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			transfoMat[i][j] = 0;
			transfoMat[i][j] = temp2[i][j];
		}
	}
}
void matrixOutput(double matrix[3][4]) {
	for (int i = 0; i < 3; i++)
	{
		cout << "{";
		for (int j = 0; j < 3; j++) {
			cout << setw(10) << matrix[i][j] << ",";
		}
		cout << setw(10) << matrix[i][3] << "}" << endl;
	}
}
void drawSquare(int x, int y) {
	glPointSize(1);
	glColor3f(0.0f, 1.0f, 1.0f);
	glBegin(GL_POINTS);
	glVertex2i(x, glutGet(GLUT_WINDOW_HEIGHT) - y);
	glEnd();
}
void drawAllPoint() {
	glClear(GL_COLOR_BUFFER_BIT);
	list<point> ::iterator p;
	for (p = pointList.begin(); p != pointList.end(); p++) {
		drawSquare(p->getX(), (1000 - (p->getY())));
	}
	glutSwapBuffers();
}
void drawLine(point src, point des) {
	int dx = des.getX() - src.getX();
	if (dx < 0) {
		point temp = point(des.getX(), des.getY());
		des.setX(src.getX());
		des.setY(src.getY());
		src.setX(temp.getX());
		src.setY(temp.getY());
	}//switch end
	int dy = des.getY() - src.getY();
	dx = des.getX() - src.getX();
	int x = src.getX();
	int y = src.getY();
	float slope = dy / (float)dx;
	int direction = slope > 0 ? 1 : (slope = -slope, -1);  //0<slope<∞
	if (dy == 0) {  //slope =0
		for (int tempX = min(src.getX(), des.getX()); tempX < max(src.getX(), des.getX()); tempX++) {
			pointList.push_back(point(tempX, des.getY()));
		}
	}
	else if (dx == 0) //slope=∞
	{
		for (int tempY = min(src.getY(), des.getY()); tempY < max(src.getY(), des.getY()); tempY += 1)
		{
			pointList.push_back(point(des.getX(), tempY));
		}
	}
	else if (0 < slope && slope <= 1) { // 0<slope<=1

		float d_init = dy * direction + (float)dx / 2;
		int incE = dy * direction;
		int incNE = dy * direction - dx;
		float d = d_init;
		pointList.push_back(point(x, y));
		while (x <= des.getX())
		{
			if (d <= 0) {
				pointList.push_back(point(x, y));
				x += 1;
				d += incE;
			}
			else
			{
				pointList.push_back(point(x, y));
				x += 1;
				y += direction;
				d += incNE;
			}
		}
	}

	else if (slope > 1)
	{
		float d_init = dx + (float)dy * direction / 2;
		int incE = dx;
		int incNE = dx - dy * direction;
		float d = d_init;
		pointList.push_back(point(x, y));
		while (y * direction <= des.getY() * direction)
		{
			if (d <= 0) {
				pointList.push_back(point(x, y));
				y += direction;
				d += incE;
			}
			else
			{
				pointList.push_back(point(x, y));
				x += 1;
				y += direction;
				d += incNE;
			}
		}
	}
}
//    a-----c
//    |     |
//	  b-----d
void drawBorder(double vxl, double vxr, double vyb, double vyt) {
	point a(vxl, vyt), b(vxl, vyb), c(vxr, vyt), d(vxr, vyb);   
	drawLine(a, b);												
	drawLine(a, c);											   
	drawLine(b, d);
	drawLine(c, d);
}
void quit() {
	exit(1);
}

void saveToSideList(double result[3][4]) {
	pointInDouble p0(result[0][0], result[1][0]),
		p1(result[0][1], result[1][1]),
		p2(result[0][2], result[1][2]),
		p3(result[0][3], result[1][3]);

	windowSidePair.push_back(make_pair(p0, p1));

	windowSidePair.push_back(make_pair(p1, p2));

	windowSidePair.push_back(make_pair(p2, p3));

	windowSidePair.push_back(make_pair(p3, p0));
}
void saveToSideList(double result[3][3]) {
	pointInDouble p0(result[0][0], result[1][0]),
		p1(result[0][1], result[1][1]),
		p2(result[0][2], result[1][2]);

	windowSidePair.push_back(make_pair(p0, p1));

	windowSidePair.push_back(make_pair(p1, p2));

	windowSidePair.push_back(make_pair(p2, p0));
}

pair<double, double> windowToViewport(double _Xw, double _Yw, double wxl, double wxr, double wyb, double wyt, double vxl, double vxr, double vyb, double vyt) {

	double Xv, Yv, Xw = _Xw, Yw = _Yw;
	double Sx = (vxr - vxl) / (wxr - wxl);
	double Sy = (vyt - vyb) / (wyt - wyb);
	Xv = (vxl + (Xw - wxl) * Sx);
	Yv = (vyb + (Yw - wyb) * Sy);

	return make_pair(Xv, Yv);
}
pair<point, point> clipping(double vx0, double vy0, double vx1, double vy1, double vxl, double vxr, double vyb, double vyt) {
	OutCode code0 = computeOutCode(vx0, vy0, vxl, vxr, vyb, vyt);
	OutCode	code1 = computeOutCode(vx1, vy1, vxl, vxr, vyb, vyt);

	while (true) {
		if (!(code0 | code1)) {
			return make_pair(point(vx0, vy0), point(vx1, vy1));
			break;
		}
		else if (code0 & code1) {
			return make_pair(point(NULL, NULL), point(NULL, NULL));
			break;
		}
		else {
			double x, y;
			OutCode outcodeOut = code1 > code0 ? code1 : code0;
			if (outcodeOut & TOP) {//點在視窗上面
				x = vx0 + (vx1 - vx0) * (vyt - vy0) / (vy1 - vy0);
				y = vyt;
			}
			else if (outcodeOut & BOTTOM) {//點在視窗下面
				x = vx0 + (vx1 - vx0) * (vyb - vy0) / (vy1 - vy0);
				y = vyb;
			}
			else if (outcodeOut & RIGHT) {//點在視窗右邊
				y = vy0 + (vy1 - vy0) * (vxr - vx0) / (vx1 - vx0);
				x = vxr;
			}
			else if (outcodeOut & LEFT) {//點在視窗左邊
				y = vy0 + (vy1 - vy0) * (vxl - vx0) / (vx1 - vx0);
				x = vxl;
			}

			if (outcodeOut == code0) {
				vx0 = x;
				vy0 = y;
				code0 = computeOutCode(vx0, vy0, vxl, vxr, vyb, vyt);
			}
			else {
				vx1 = x;
				vy1 = y;
				code1 = computeOutCode(vx1, vy1, vxl, vxr, vyb, vyt);
			}
		}
	}
}

void SwitchMode(string command) {
	char space_char = ' ';
	vector<string> words{};
	istringstream iss(command);
	for (string command; iss >> command; ) {
		words.push_back(command);
	}

	if (!words.empty()) {
		if (words[0] == "scale")
		{
			cout << "Scale" << endl;
			double x = atof(words[1].c_str());
			double y = atof(words[2].c_str());
			transfoMat[0][0] = transfoMat[0][0] * x;
			transfoMat[1][1] = transfoMat[1][1] * y;
			matrixOutput(transfoMat);
		}
		else if (words[0] == "rotate")
		{
			cout << "Rotate" << endl;
			double angleTransfoMat[3][3] = { 1.0, 0.0, 0.0,
													   0.0, 1.0, 0.0,
													   0.0, 0.0, 1.0 };
			double degree = atof(words[1].c_str());
			angleTransfoMat[0][0] = cos(angleToRadian(degree));
			angleTransfoMat[0][1] = -1.0 * sin(angleToRadian(degree));
			angleTransfoMat[1][0] = sin(angleToRadian(degree));
			angleTransfoMat[1][1] = cos(angleToRadian(degree));

			calRotateTMMulTM(angleTransfoMat);
			matrixOutput(transfoMat);
		}
		else if (words[0] == "translate")
		{
			cout << "Translate" << endl;
			double x = atof(words[1].c_str());
			double y = atof(words[2].c_str());
			transfoMat[0][2] = transfoMat[0][2] + x;
			transfoMat[1][2] = transfoMat[1][2] + y;
			matrixOutput(transfoMat);
		}
		else if (words[0] == "square")
		{
			double result[3][4];
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 4; j++) {
					result[i][j] = 0;
					for (int k = 0; k < 3; k++) {
						result[i][j] += transfoMat[i][k] * squareMat[k][j];
					}
				}
			}
			saveToSideList(result);
			squareCounter += 1;
			cout << "You have" << setw(2) << squareCounter << " squares." << endl;
		}
		else if (words[0] == "triangle")
		{
			double result[3][3];
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					result[i][j] = 0;
					for (int k = 0; k < 3; k++) {
						result[i][j] += transfoMat[i][k] * triMat[k][j];
					}
				}
			}
			saveToSideList(result);
			triCounter += 1;
			cout << "You have" << setw(2) << triCounter << " triangles." << endl;
		}
		else if (words[0] == "view")
		{
			cout << "view" << endl << "WVM" << endl;
			double wxl = atof(words[1].c_str());
			double wxr = atof(words[2].c_str());
			double wyb = atof(words[3].c_str());
			double wyt = atof(words[4].c_str());
			double vxl = atof(words[5].c_str());
			double vxr = atof(words[6].c_str());
			double vyb = atof(words[7].c_str());
			double vyt = atof(words[8].c_str());
			int firstVx = 0, firstVy = 0, secondVx = 0, secondVy = 0;
			point clippingP0(0, 0), clippingP1(0, 0);
			for (int i = 0; i < windowSidePair.size(); i++) {
				tie(firstVx, firstVy) = windowToViewport(windowSidePair[i].first.getX(), windowSidePair[i].first.getY(), wxl, wxr, wyb, wyt, vxl, vxr, vyb, vyt);
				tie(secondVx, secondVy) = windowToViewport(windowSidePair[i].second.getX(), windowSidePair[i].second.getY(), wxl, wxr, wyb, wyt, vxl, vxr, vyb, vyt);
				
				tie(clippingP0, clippingP1) = clipping(firstVx, firstVy, secondVx, secondVy, vxl, vxr, vyb, vyt);
				drawLine(clippingP0, clippingP1);
			}
			drawBorder(vxl, vxr, vyb, vyt);
			drawAllPoint();
			//wvm
			double TnegW[3][3] = { 1.0, 0.0, -wxl,
							   0.0, 1.0, -wyb,
							   0.0, 0.0, 1.0 };
			double Tv[3][3] = { 1.0, 0.0, vxl,
									   0.0, 1.0, vyb,
									   0.0, 0.0, 1.0 };
			double S[3][3] = { (vxr - vxl) / (wxr - wxl), 0.0, 0.0,
							   0.0, (vyt - vyb) / (wyt - wyb), 0.0,
							   0.0, 0.0, 1.0 };
			double temp[3][3];
			double wvm[3][3];
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					temp[i][j] = 0;
					for (int k = 0; k < 3; k++) {
						temp[i][j] += S[i][k] * TnegW[k][j];
					}
				}
			}
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					wvm[i][j] = 0;
					for (int k = 0; k < 3; k++) {
						wvm[i][j] += Tv[i][k] * temp[k][j];
					}
				}
			}
			matrixOutput(wvm);
			system("pause");
		}
		else if (words[0] == "clearData")
		{
			pointList.clear();
			windowSidePair.clear();
			squareCounter = 0;
		}
		else if (words[0] == "clearScreen")
		{
			pointList.clear();
			drawAllPoint();
		}
		else if (words[0] == "reset")
		{
			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j < 3; j++)
				{
					transfoMat[i][j] = defaultMatrix[i][j];
				}
			}
		}
		else if (words[0] == "end")
		{
			quit();
		}
		else if (words[0] == "#") {
			for (int j = 0; j < words.size(); j++)
			{
				cout << words[j] << " ";
			}
			cout << endl;
		}
		else  
		{
			//NULL
		}
	}
}

void display() {
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	string lineInFile;

	FileInput.open(fileName);
	if (!FileInput.is_open()) {
		perror("Error open");
		exit(EXIT_FAILURE);
	}
	while (getline(FileInput, lineInFile)) {
		SwitchMode(lineInFile);
	}
	FileInput.close();
	glFlush();
}

int main(int argc, char** argv) {
	system("pause");
	glutInit(&argc, argv);
	fileName = argv[1];

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(1000, 1000);
	glutInitWindowPosition(0, 0);
	gluOrtho2D(0, 1500, 0, 1500);
	glutCreateWindow("110522060");
	glutDisplayFunc(display);
	gluOrtho2D(0, 1500, 0, 1500);
	glutSwapBuffers();
	glutMainLoop();
}