// 2021CG_Lab2_110522060.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
//
#include <GL/glut.h>
#include <GL/gl.h>
#include<iostream>
#include <list>
//#include "2021CG_Lab2_110522060.h"
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
#define windowH 1000
#define windowW 1000
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
	};
	int getX() {
		return x;
	};
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
ifstream inputFile;
vector<pair<pointInDouble, pointInDouble>> windowSidePair;
vector<pair<point, point>> viewSidePair;
list<point> pointList;
//list<point> clippingPointList;
list<point> pointListBackUp;
point lineTemp = point(NULL, NULL);
//point circleTemp = point(NULL, NULL);
//point polygonSrc = point(NULL, NULL);
//point polygontemp = point(NULL, NULL);
//int circleCounter = 0;
int lineCounter = 0;
int squareCounter = 0;
int triangleCounter = 0;
//int mode = 0; //  d,l,p,o,c,r,q
string fileName = "";

static double defaultMatrix[3][3] = { 1.0, 0.0, 0.0,
									  0.0, 1.0, 0.0,
									  0.0, 0.0, 1.0 };
static double transformationMatrix[3][3] = { 1.0, 0.0, 0.0,
											 0.0, 1.0, 0.0,
											 0.0, 0.0, 1.0 };
static double squareMatrix[3][4] = { -1.0,-1.0, 1.0, 1.0,
									   -1.0, 1.0, 1.0,-1.0,
										1.0, 1.0 ,1.0, 1.0 };
static double triangleMatrix[3][3] = { 0.0,-1.0, 1.0,
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

double angleToRadian(double angle) {  //from angle to radian
	return (angle * M_PI / (double)180.0);
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
	//cout << endl;
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
	//cout << endl;
}
void calRotateTMMulTM(double angleTransformationMatrix[3][3]) {
	double temp[3][3];
	double temp2[3][3];

	pair<double, double> translateTemp = { transformationMatrix[0][2], transformationMatrix[1][2] };


	double Tnegative[3][3] = { 1.0, 0.0, -1 * translateTemp.first,
							   0.0, 1.0, -1 * translateTemp.second,
							   0.0, 0.0, 1.0 };
	double T[3][3] = { 1.0, 0.0, translateTemp.first,
					   0.0, 1.0, translateTemp.second,
					   0.0, 0.0, 1.0 };

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			temp2[i][j] = 0;
			for (int k = 0; k < 3; k++) {
				temp2[i][j] += angleTransformationMatrix[i][k] * transformationMatrix[k][j];
			}
		}
	}
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			transformationMatrix[i][j] = 0;
			transformationMatrix[i][j] = temp2[i][j];
		}
	}
}
void quit() {
	exit(1);
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
		drawSquare(p->getX(), (windowH - (p->getY())));
	}
	glutSwapBuffers();
}

void drawLine(point src, point des) {
	int dx = des.getX() - src.getX();
	if (dx < 0) {   // des.x < src.x then switch if needed?
		point temp = point(des.getX(), des.getY());
		des.setX(src.getX());
		des.setY(src.getY());
		src.setX(temp.getX());
		src.setY(temp.getY());
		//cout << "switch   \n\n";
	}//switch end
	//cout << src.getX() << " " << src.getY() << " " << des.getX() << " " << des.getY() << "\n\n";  //debug
	int dy = des.getY() - src.getY();
	dx = des.getX() - src.getX();
	int x = src.getX();
	int y = src.getY();
	float slope = dy / (float)dx;
	int direction = slope > 0 ? 1 : (slope = -slope, -1);  //0<slope<∞
	//cout<<slope<<" " << dx << " " << dy << " " << "\n";  //debug
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
			//cout << d << "\n\n";  //debug
			if (d <= 0) {
				pointList.push_back(point(x, y));
				x += 1;
				//cout << x <<" " << y << "\n";  //debug
				d += incE;
			}
			else
			{
				pointList.push_back(point(x, y));
				x += 1;
				y += direction;
				//cout << x <<" " << y << "\n";  //debug
				d += incNE;
			}
		}
	}

	else if (slope > 1) //slope>1 then switch (y,x)
	{
		float d_init = dx + (float)dy * direction / 2;
		int incE = dx;
		int incNE = dx - dy * direction;
		float d = d_init;
		pointList.push_back(point(x, y));
		while (y * direction <= des.getY() * direction)
		{
			//cout << d << "\n\n";  //debug
			if (d <= 0) {
				pointList.push_back(point(x, y));
				y += direction;
				//cout << x <<" " << y << "\n";  //debug
				d += incE;
			}
			else
			{
				pointList.push_back(point(x, y));
				x += 1;
				y += direction;
				//cout << x <<" " << y << "\n";  //debug
				d += incNE;
			}
		}
	}
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
void drawBorder(double vxl, double vxr, double vyb, double vyt) {
	point a(vxl, vyt), b(vxl, vyb), c(vxr, vyt), d(vxr, vyb);   //    a-----c
	drawLine(a, b);												//    |     |
	drawLine(a, c);											    //	  b-----d
	drawLine(b, d);
	drawLine(c, d);
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
			// bitwise OR is 0: both points inside window; trivially accept and exit loop
			return make_pair(point(vx0, vy0), point(vx1, vy1));
			break;
		}
		else if (code0 & code1) {
			// bitwise AND is not 0: both points share an outside zone (LEFT, RIGHT, TOP,
			// or BOTTOM), so both must be outside window; exit loop (accept is false)
			return make_pair(point(NULL, NULL), point(NULL, NULL));
			break;
		}
		else {
			// failed both tests, so calculate the line segment to clip
			// from an outside point to an intersection with clip edge
			double x, y;

			// At least one endpoint is outside the clip rectangle; pick it.
			OutCode outcodeOut = code1 > code0 ? code1 : code0;

			// Now find the intersection point;
			// use formulas:
			//   slope = (y1 - y0) / (x1 - x0)
			//   x = x0 + (1 / slope) * (ym - y0), where ym is ymin or ymax
			//   y = y0 + slope * (xm - x0), where xm is xmin or xmax
			// No need to worry about divide-by-zero because, in each case, the
			// outcode bit being tested guarantees the denominator is non-zero
			if (outcodeOut & TOP) {           // point is above the clip window
				x = vx0 + (vx1 - vx0) * (vyt - vy0) / (vy1 - vy0);
				y = vyt;
			}
			else if (outcodeOut & BOTTOM) { // point is below the clip window
				x = vx0 + (vx1 - vx0) * (vyb - vy0) / (vy1 - vy0);
				y = vyb;
			}
			else if (outcodeOut & RIGHT) {  // point is to the right of clip window
				y = vy0 + (vy1 - vy0) * (vxr - vx0) / (vx1 - vx0);
				x = vxr;
			}
			else if (outcodeOut & LEFT) {   // point is to the left of clip window
				y = vy0 + (vy1 - vy0) * (vxl - vx0) / (vx1 - vx0);
				x = vxl;
			}

			// Now we move outside point to intersection point to clip
			// and get ready for next pass.
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
//void fakeClipping(double vxl, double vxr, double vyb, double vyt) {
//	list<point> ::iterator p;
//	for (p= pointList.begin(); p != pointList.end(); p++) {
//		point newP(p->getX(), p->getY());
//		if (p->getX() < vxl)
//		{
//			newP.setX(vxl);
//		}
//		if (p->getX() > vxr)
//		{
//			newP.setX(vxr);
//		}
//		if (p->getY() < vyb)
//		{
//			newP.setY(vyb);
//		}
//		if (p->getY() > vyt)
//		{
//			newP.setY(vyt);
//		}
//		clippingPointList.push_back(newP);
//	}
//}
void modeSwitch(string command) {
	char space_char = ' ';
	vector<string> words{};
	istringstream iss(command);
	for (string command; iss >> command; ) {
		words.push_back(command);
	}
	//stringstream sstream(command);
	//string word;
	//while (getline(sstream, word, space_char)) {
	//	//word.erase(std::remove_if(word.begin(), word.end(), ispunct), word.end());
	//	//words.push_back(word);
	//}

	if (!words.empty()) {

		/*for (const auto& str : words) {
			cout << str << " ";
		}*/
		//cout << words[0] << endl;
		//mode select
		if (words[0] == "scale")
		{
			cout << "Scale" << endl;
			double x = atof(words[1].c_str());
			double y = atof(words[2].c_str());
			//cout << x << "　" << y << endl;
			transformationMatrix[0][0] = transformationMatrix[0][0] * x;
			transformationMatrix[1][1] = transformationMatrix[1][1] * y;
			matrixOutput(transformationMatrix);

		}
		else if (words[0] == "rotate")
		{
			cout << "Rotate" << endl;
			double angleTransformationMatrix[3][3] = { 1.0, 0.0, 0.0,
													   0.0, 1.0, 0.0,
													   0.0, 0.0, 1.0 };
			double degree = atof(words[1].c_str());
			//cout << degree << endl;

			angleTransformationMatrix[0][0] = cos(angleToRadian(degree));
			angleTransformationMatrix[0][1] = -1.0 * sin(angleToRadian(degree));
			angleTransformationMatrix[1][0] = sin(angleToRadian(degree));
			angleTransformationMatrix[1][1] = cos(angleToRadian(degree));

			calRotateTMMulTM(angleTransformationMatrix);
			matrixOutput(transformationMatrix);

		}
		else if (words[0] == "translate")
		{
			cout << "Translate" << endl;
			double x = atof(words[1].c_str());
			double y = atof(words[2].c_str());
			//cout << x << "　" << y << endl;
			transformationMatrix[0][2] = transformationMatrix[0][2] + x;
			transformationMatrix[1][2] = transformationMatrix[1][2] + y;
			matrixOutput(transformationMatrix);

		}
		else if (words[0] == "square")
		{
			//cout << "--square--" << endl;
			double result[3][4];
			for (int i = 0; i < 3; i++) {
				//cout << "{";
				for (int j = 0; j < 4; j++) {
					result[i][j] = 0;
					for (int k = 0; k < 3; k++) {
						result[i][j] += transformationMatrix[i][k] * squareMatrix[k][j];
					}
					//cout << setw(10) << result[i][j] << "";
				}
				//cout << "}" << endl;
			}
			saveToSideList(result);
			squareCounter += 1;
			cout << "You have" << setw(2) << squareCounter << " squares." << endl;
		}
		else if (words[0] == "triangle")
		{
			double result[3][3];
			//cout << "Triangle" << endl;
			//cout << endl;
			for (int i = 0; i < 3; i++) {
				//cout << "{";
				for (int j = 0; j < 3; j++) {
					result[i][j] = 0;
					for (int k = 0; k < 3; k++) {
						result[i][j] += transformationMatrix[i][k] * triangleMatrix[k][j];
					}
					//cout << setw(10) << result[i][j] << "\t";
				}
				//cout << "}" << endl;
			}
			//matrixOutput(result);
			saveToSideList(result);
			triangleCounter += 1;
			cout << "You have" << setw(2) << triangleCounter << " triangles." << endl;
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
			//cout << wxl << " " << wxr << " " << wyb << " " << wyt << " " << vxl << " " << vxr << " " << vyb << " " << vyt << endl;
			//world to view space
			int firstVx = 0, firstVy = 0, secondVx = 0, secondVy = 0;
			point clippingP0(0, 0), clippingP1(0, 0);
			for (int i = 0; i < windowSidePair.size(); i++) {
				//cout << "----side pair NO." << i << endl <<setw(6)<<"first" << setw(4) << windowSidePair[i].first.getX() << setw(4) << windowSidePair[i].first.getY() << endl<<setw(6)<<"second" << setw(4) << windowSidePair[i].second.getX() << setw(4) << windowSidePair[i].second.getY() << endl;
				//world to view space
				tie(firstVx, firstVy) = windowToViewport(windowSidePair[i].first.getX(), windowSidePair[i].first.getY(), wxl, wxr, wyb, wyt, vxl, vxr, vyb, vyt);
				tie(secondVx, secondVy) = windowToViewport(windowSidePair[i].second.getX(), windowSidePair[i].second.getY(), wxl, wxr, wyb, wyt, vxl, vxr, vyb, vyt);

				//clipping
				tie(clippingP0, clippingP1) = clipping(firstVx, firstVy, secondVx, secondVy, vxl, vxr, vyb, vyt);
				//point viewP0(firstVx,firstVy), viewP1(secondVx,secondVy);
				//cout << "--after WtoV"<< endl << setw(6) << "first" << setw(4) << clippingP0.getX() << setw(4) << clippingP0 .getY()<< endl << setw(6) << "second" << setw(4) << clippingP1.getX() << setw(4) << clippingP1.getY() << endl;

				drawLine(clippingP0, clippingP1);
			}
			drawBorder(vxl, vxr, vyb, vyt);
			//fakeClipping(vxl, vxr, vyb, vyt);  //just a joke, you knowwwwwww
			//drawClippingPoint();
			drawAllPoint();
			//cal wvm
			double TnegativeW[3][3] = { 1.0, 0.0, -wxl,
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
						temp[i][j] += S[i][k] * TnegativeW[k][j];
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
			//cout << "--clear data--" << endl;
			pointList.clear();
			windowSidePair.clear();
			squareCounter = 0;
		}
		else if (words[0] == "clearScreen")
		{
			//cout << "--clear screen--" << endl;
			pointList.clear();
			drawAllPoint();
			/*clippingPointList.clear();
			drawClippingPoint();*/
		}
		else if (words[0] == "reset")
		{
			//cout << "--reset--" << endl;
			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j < 3; j++)
				{
					transformationMatrix[i][j] = defaultMatrix[i][j];
				}
			}
		}
		else if (words[0] == "end")
		{
			//cout << "--end--" << endl;

			quit();
		}
		else if (words[0] == "#") {
			for (int j = 0; j < words.size(); j++)
			{
				cout << words[j] << " ";
			}
			cout << endl;
		}
		else  // "NULL" or #......
		{
			//cout << "--skip--" << endl;
		}
	}
}

void display() {
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	string lineInFile;

	inputFile.open(fileName);
	if (!inputFile.is_open()) {
		perror("Error open");
		exit(EXIT_FAILURE);
	}
	while (getline(inputFile, lineInFile)) {
		modeSwitch(lineInFile);
	}
	inputFile.close();
	glFlush();
}

int main(int argc, char** argv) {
	system("pause");
	glutInit(&argc, argv);
	fileName = argv[1];

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(windowW, windowH);
	glutInitWindowPosition(0, 0);
	gluOrtho2D(0, windowW, 0, windowH);
	glutCreateWindow("110522060");
	glutDisplayFunc(display);
	gluOrtho2D(0, windowW, 0, windowH);
	glutSwapBuffers();
	glutMainLoop();
}