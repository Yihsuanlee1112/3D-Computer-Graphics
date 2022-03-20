// 2021CG_Lab3_110522060.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
//

#include <iostream>
#include <iostream>
#include <fstream>
#include <gl/glut.h>
#include <string>
#include <typeinfo>
#include <cmath>
#include <vector>


struct poi {
	float x, y, z, w;
};
struct plane {
	int count;
	bool visible = 1;
	std::vector<poi> poly;
};
struct object {
	std::vector<plane> planes;
};
static float width = 0, height = 0;

std::vector<object> objs;

static float PI = 3.1415926535 / 180;
using namespace std;
static int mode = -1;
//cos(degree * M_PI / 180)
static int noback = 0;

int state = -1;
//for clipping
float vxmin;
float vxmax;
float vymin;
float vymax;
float hav;

static float TM[4][4] = { 1.0, 0.0, 0.0, 0.0,
						  0.0, 1.0, 0.0, 0.0,
						  0.0, 0.0, 1.0, 0.0,
						  0.0, 0.0, 0.0, 1.0 };
static float EM[4][4] = { 1.0, 0.0, 0.0, 0.0,
						  0.0, 1.0, 0.0, 0.0,
						  0.0, 0.0, 1.0, 0.0,
						  0.0, 0.0, 0.0, 1.0 };
static float PM[4][4] = { 1.0, 0.0, 0.0, 0.0,
						  0.0, 1.0, 0.0, 0.0,
						  0.0, 0.0, 1.0, 0.0,
						  0.0, 0.0, 0.0, 1.0 };
static float WTV[4][4] = { 1.0, 0.0, 0.0, 0.0,
						  0.0, 1.0, 0.0, 0.0,
						  0.0, 0.0, 1.0, 0.0,
						  0.0, 0.0, 0.0, 1.0 };
static float calM[4][4] = { 1.0, 0.0, 0.0, 0.0,
						  0.0, 1.0, 0.0, 0.0,
						  0.0, 0.0, 1.0, 0.0,
						  0.0, 0.0, 0.0, 1.0 };
static float A[4][4] = { 0.0, 0.0, 0.0, 0.0,
						  0.0, 0.0, 0.0, 0.0,
						  0.0, 0.0, 0.0, 0.0,
						  0.0, 0.0, 0.0, 0.0 };


int f_to_int(float a) {
	float b = (int)(a + 0.5) / 1.0;
	int ans = b;
	return ans;
}
GLfloat transX(float x) {
	GLfloat f = 0;
	f = x - width / 2;
	f = f / (width / 2);
	return f;
}
GLfloat transY(float y) {
	GLfloat f = 0;
	f = y - height / 2;
	f = f / (height / 2);
	f = 0 - f;
	//f = y / height;
	return f;
}

void printM(float M[4][4]) {
	int i, j;

	for (i = 0; i < 4; i++) {
		cout << "[";
		for (j = 0; j < 4; j++) {
			cout << M[i][j] << " ";
		}
		cout << "]\n";
	}
}

float getNum(char file[2000], int countWord) {
	float aNum = 0;
	float liNum = 0;
	int i = 0;
	double f = 0;
	int m = 0;
	int j = 0, k = 0, mj = 0;

	for (k = 0; k < countWord; k++) {
		for (j = mj; j < 2000; j++) {
			if (file[j] == ' ' && file[j + 1] != ' ') {
				mj = j + 1;
				break;
			}
		}
	}
	//cout << "mj:" << mj<<"\n";
	for (i = mj; i < 2000; i++) {
		if (file[i] == '\0' || file[i] == ' ')
			break;
		if (file[i] == '.') {
			f = -1;
		}
		else if (file[i] == '-') {
			m = 1;
		}
		else if (file[i] >= '0' && file[i] <= '9') {
			int te = file[i] - '0';
			if (f == 0) {
				aNum = aNum * 10 + te;
			}
			else {
				aNum += te * pow(10, f);
				f--;
			}
		}
	}
	if (m == 1) {
		aNum = 0 - aNum;
	}

	return aNum;
}

void checkMode(char file[2000]) {
	//get first word of file
	char word[20]; //first word of file
	int i = 0;
	for (i = 0; i < 2000; i++) {
		if (file[i] == ' ' || file[i] == '\0') {
			word[i] = '\0';
			break;
		}
		else {
			word[i] = file[i];
		}
	}
	if (strcmp(word, "scale") == 0) {
		cout << "<action: scale>";
		mode = 1;
	}
	else if (strcmp(word, "rotate") == 0) {
		cout << "<action: rotate>";
		mode = 2;
	}
	else if (strcmp(word, "translate") == 0) {
		cout << "<action: translate>";
		mode = 3;
	}
	else if (strcmp(word, "object") == 0) {
		cout << "<action: object>";
		mode = 4;
	}
	else if (strcmp(word, "observer") == 0) {
		cout << "<action: observer>";
		mode = 5;
	}
	else if (strcmp(word, "viewport") == 0) {
		cout << "<action: viewport>";
		mode = 6;
	}
	else if (strcmp(word, "display") == 0) {
		cout << "<action: display>";
		mode = 7;
	}
	else if (strcmp(word, "nobackfaces") == 0) {
		cout << "<action: nobackfaces>";
		mode = 8;
	}
	else if (strcmp(word, "reset") == 0) {
		cout << "<action: reset>";
		mode = 9;
	}
	else if (strcmp(word, "end") == 0) {
		cout << "<action: end>";
		mode = 10;
	}
	else if (file[0] == '#') {
		cout << "<command>";
		cout << file;
		mode = 11;
	}
	cout << endl;
}

poi cross(poi a, poi b) {
	poi ans;
	ans.x = a.y * b.z - a.z * b.y;
	ans.y = a.z * b.x - a.x * b.z;
	ans.z = a.x * b.y - a.y * b.x;
	return ans;
}

float len_of_vector(poi p) {
	float ans = 0;
	ans = (float)sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
	return ans;
}

int dotNum = 0;
int planeNum = 0;
int nowLoadP = 0; // the plane in planes[] that belong to current obj
int getObj(char name[100]) {
	fstream obj;
	poi points[2000];
	obj.open(name, ios::in);
	cout << name << "\n";
	char objfile[2000];
	obj.getline(objfile, sizeof(obj), '\n');
	while (objfile[0] == '\0') {
		obj.getline(objfile, sizeof(obj), '\n');
	}
	dotNum = getNum(objfile, 0);
	int pN = getNum(objfile, 1);
	planeNum += pN;

	//cout << "d p: " << dotNum << " " << planeNum << "\n";

	int i = 0;
	for (i = 0; i < dotNum; i++) {//read points' coordinate
		obj.getline(objfile, sizeof(obj), '\n');
		//cout << objfile << "\n";
		if (objfile[0] == ' ') {
			points[i].x = getNum(objfile, 1);
			points[i].y = getNum(objfile, 2);
			points[i].z = getNum(objfile, 3);
			points[i].w = 1;
		}
		else {
			points[i].x = getNum(objfile, 0);
			points[i].y = getNum(objfile, 1);
			points[i].z = getNum(objfile, 2);
			points[i].w = 1;
		}

		//cout << points[i].x << " " << points[i].y << " " << points[i].z << "\n";
	}
	object ob;

	for (i = 0; i < pN; i++) {
		//planes[i].count save the number of points in current plane
		int j = 0;
		obj.getline(objfile, sizeof(obj), '\n');
		int nu = getNum(objfile, 0);
		plane add;
		poi ad;
		add.count = nu;
		for (j = 0; j < nu; j++) {
			ad.x = points[(int)getNum(objfile, j + 1) - 1].x;
			ad.y = points[(int)getNum(objfile, j + 1) - 1].y;
			ad.z = points[(int)getNum(objfile, j + 1) - 1].z;
			ad.w = points[(int)getNum(objfile, j + 1) - 1].w;
			add.poly.push_back(ad);
		}
		ob.planes.push_back(add);
	}
	objs.push_back(ob);
	obj.close();
	return pN;
}

void resetOne(float M[4][4]) {
	int i, j;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			if (i == j)
				M[i][j] = 1.0;
			else
				M[i][j] = 0.0;
		}
	}
}
void resetZero(float M[4][4]) {
	int i, j;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			M[i][j] = 0.0;
		}
	}
}

void loadMatrix(float M[4][4], float N[4][4]) { // M = N
	int i, j;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			M[i][j] = N[i][j];
		}
	}
}

void mulMatrix(float M[4][4], float N[4][4]) {  // A = M*N
	resetZero(A);
	int i, j, k;
	for (k = 0; k < 4; k++) {
		for (i = 0; i < 4; i++) {
			float num = 0.0;
			for (j = 0; j < 4; j++) {
				num += M[k][j] * N[j][i];
			}
			A[k][i] = num;
		}
	}
}
poi mulMatrixPoi(float M[4][4], poi p) {
	poi ans;
	float a[4] = { 0,0,0,0 };
	float b[4] = { p.x,p.y, p.z,p.w };
	int i, j;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			a[i] += M[i][j] * b[j];
		}
	}
	ans.x = a[0];
	ans.y = a[1];
	ans.z = a[2];
	ans.w = a[3];
	//cout << ans.x << " " << ans.y << " " << ans.z << " " << ans.w << ")\n";
	return ans;
}

std::vector<object> clipping(std::vector<object> now_objs) {
	int i, j, p;
	for (i = 0; i < now_objs.size(); i++) {
		for (j = 0; j < now_objs[i].planes.size(); j++) {
			//get 6 equation
			//w+x >= 0, w-x >= 0, w+y >= 0, w-y >= 0, z >= 0, w-z >= 0
			int m = 0;
			for (m = 0; m < 6; m++) {
				std::vector <poi> poisave;
				//cout << "poi number before" << poisave.size() << "\n";
				poi a, b;
				float c1, c2;
				for (p = 0; p < now_objs[i].planes[j].poly.size(); p++) {
					//get two points
					a = now_objs[i].planes[j].poly[p];
					if (p != now_objs[i].planes[j].poly.size() - 1)
						b = now_objs[i].planes[j].poly[p + 1];
					else
						b = now_objs[i].planes[j].poly[0];
					//cout << "a(" << a.x << " " << a.y << " " << a.z << " " << a.w << ")";
					//cout << " b(" << b.x << " " << b.y << " " << b.z << " " << b.w << ")\n";

					//cout << "m: " << m << "\n";
					if (m == 0) {
						c1 = a.w + a.x;
						c2 = b.w + b.x;
					}
					else if (m == 1) {
						c1 = a.w - a.x;
						c2 = b.w - b.x;
					}
					else if (m == 2) {
						c1 = a.w + a.y;
						c2 = b.w + b.y;
					}
					else if (m == 3) {
						c1 = a.w - a.y;
						c2 = b.w - b.y;
					}
					else if (m == 4) {
						c1 = a.z;
						c2 = b.z;
					}
					else if (m == 5) {
						c1 = a.w - a.z;
						c2 = b.w - b.z;
					}
					//cout << "(" << c1 << " " << c2 << "):";
					if (c1 < 0 && c2 < 0) {//trivially reject
						//nothing
						//cout << "trivially reject\n";
					}
					else if (c1 >= 0 && c2 >= 0) {//keep for next equation
						//cout << "push b\n";
						if (poisave.size() != 0) {
							if (poisave.back().x != b.x || poisave.back().y != b.y || poisave.back().z != b.z)
								poisave.push_back(b);
						}
						else
							poisave.push_back(b);
					}
					else {
						float t = c1 / (c1 - c2);
						float x = a.x + t * (b.x - a.x);
						float y = a.y + t * (b.y - a.y);
						float z = a.z + t * (b.z - a.z);
						float w = a.w + t * (b.w - a.w);
						poi inter;
						inter.x = x;
						inter.y = y;
						inter.z = z;
						inter.w = w;
						if (c1 >= 0 && c2 < 0) {//In > Out
							//cout << "puch inter\n";
							if (poisave.size() != 0) {
								if (poisave.back().x != inter.x || poisave.back().y != inter.y || poisave.back().z != inter.z)
									poisave.push_back(inter);
							}
							else
								poisave.push_back(inter);
						}
						else if (c1 < 0 && c2 >= 0) {//Out > In
							//cout << "push inter&b\n";
							if (poisave.size() == 0) {
								poisave.push_back(inter);
							}
							else {
								if (poisave.back().x != inter.x || poisave.back().y != inter.y || poisave.back().z != inter.z) {
									poisave.push_back(inter);
								}
							}

							poisave.push_back(b);
						}
					}
				}
				now_objs[i].planes[j].poly = poisave;
				now_objs[i].planes[j].count = poisave.size();
				//cout << "poi number " << poisave.size() << "\n";

			}
		}
	}
	return now_objs;
}

void clearScreen() {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glFlush();
}
int sqCou = 0;
int triCou = 0;
void clearData() {
	sqCou = 0;
	triCou = 0;
}

int xa = 0, ya = 0;
int xb = 0, yb = 0;
int xd = 0, yd = 0;
int difX = 0;
int difY = 0;
int midX = 0;
int midY = 0;
GLfloat x, y;
void horizontal() {
	int a = xa;
	xa = xb;
	xb = a;
}

void xychange() {
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
		xychange();
		horizontal();
	}
	else if (state == 4) {
		xychange();
	}
}

void turnBack(int state) {//xd yd
	if (state == 2) {
		if (xd < midX / 2) {
			xd -= (2 * xd - midX);
		}
		else {
			xd += (midX - 2 * xd);
		}
	}
	else if (state == 3) {

		if (xd < midX / 2) {
			xd -= (2 * xd - midX);
		}
		else {
			xd += (midX - 2 * xd);
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
void drawLine(poi pa, poi pb) { //draw(xa, ya)(xb, yb)
	//cout << "#<" << xa << " " << ya << ">";
	//cout << "#<" << xb << " " << yb << ">";
	xa = f_to_int(pa.x);
	ya = f_to_int(pa.y);
	xb = f_to_int(pb.x);
	yb = f_to_int(pb.y);

	xd = xa, yd = ya;
	glBegin(GL_POINTS);
	x = transX(xd);
	y = 0 - transY(yd);
	glVertex3f(x, y, 0.0);

	int state = rotate();
	//cout << "state " << state;
	turn(state);
	if ((xa > xb && ya > yb) || (xa > xb && ya == yb)) {
		int save = xa;
		xa = xb;
		xb = save;
		save = ya;
		ya = yb;
		yb = save;
	}
	midX = (xa + xb);
	midY = (ya + yb);

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
		x = transX(xd);
		y = 0 - transY(yd);
		glVertex3f(x, y, 0.0);
	}

	glEnd();
	glFlush();
}

int main(int argc, char* argv[]) {
	system("pause");

	//input file(argv[1])
	fstream fileIn;
	fileIn.open(argv[1], ios::in);
	char file[2000];
	fileIn.getline(file, sizeof(file), '\n');
	width = getNum(file, 0);
	height = getNum(file, 1);
	cout << "w h:" << width << " " << height << "\n";

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(width, height);
	glutCreateWindow("106502547");

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glFlush();

	glPointSize(1);
	glColor3f(0.0, 1.0, 0.0);


	while (fileIn.getline(file, sizeof(file), '\n')) {

		//system("pause");
		//cout << file[0] << endl;
		checkMode(file);
		if (file[0] == '\0')
			continue;
		if (mode == 1) {//scale
			float SM[4][4] = { 1.0, 0.0, 0.0, 0.0,
						  0.0, 1.0, 0.0, 0.0,
						  0.0, 0.0, 1.0, 0.0,
						  0.0, 0.0, 0.0, 1.0 };
			float x = getNum(file, 1);
			float y = getNum(file, 2);
			float z = getNum(file, 3);
			cout << "x y z: " << x << " " << y << " " << z << "\n";
			SM[0][0] = x;
			SM[1][1] = y;
			SM[2][2] = z;
			mulMatrix(SM, TM);
			loadMatrix(TM, A);
		}
		else if (mode == 2) { //rotate
			float RM[4][4] = { 1.0, 0.0, 0.0, 0.0,
						  0.0, 1.0, 0.0, 0.0,
						  0.0, 0.0, 1.0, 0.0,
						  0.0, 0.0, 0.0, 1.0 };
			float x = getNum(file, 1);
			float y = getNum(file, 2);
			float z = getNum(file, 3);
			cout << "x y z: " << x << " " << y << " " << z << "\n";
			if (x != 0) {
				double r = x * PI;
				RM[1][1] = cos(r);
				RM[2][2] = RM[1][1];
				RM[2][1] = sin(r);
				RM[1][2] = 0 - RM[2][1];
			}
			else if (y != 0) {
				double r = y * PI;
				RM[0][0] = cos(r);
				RM[2][2] = RM[0][0];
				RM[0][2] = sin(r);
				RM[2][0] = 0 - RM[0][2];
			}
			else if (z != 0) {
				double r = z * PI;
				RM[0][0] = cos(r);
				RM[1][1] = RM[0][0];
				RM[1][0] = sin(r);
				RM[0][1] = 0 - RM[1][0];
			}
			mulMatrix(RM, TM);
			loadMatrix(TM, A);
		}
		else if (mode == 3) { //translate
			float TrM[4][4] = { 1.0, 0.0, 0.0, 0.0,
						  0.0, 1.0, 0.0, 0.0,
						  0.0, 0.0, 1.0, 0.0,
						  0.0, 0.0, 0.0, 1.0 };
			float x = getNum(file, 1);
			float y = getNum(file, 2);
			float z = getNum(file, 3);
			cout << "x y z: " << x << " " << y << " " << z << "\n";
			TrM[0][3] = x;
			TrM[1][3] = y;
			TrM[2][3] = z;
			mulMatrix(TrM, TM);
			loadMatrix(TM, A);
		}
		else if (mode == 4) { //object
			char name[100];
			int n = 0, u = 0, re = 0;
			//get name
			for (u = 0; u < 100; u++) {
				if (file[u] == '\0')
					break;
				if (re == 1) {
					name[n] = file[u];
					n++;
				}
				if (file[u] == ' ')
					re = 1;
			}
			name[n] = '\0';
			int nowPlaneNum = getObj(name);
			// mul TM
			int i = 0;
			int p = objs.size() - 1;
			cout << "TM:\n";
			printM(TM);
			for (i = 0; i < objs[p].planes.size(); i++) {
				//save calM back to planes[i].poly[j]
				int j = 0;
				for (j = 0; j < objs.back().planes[i].count; j++) {//load points into matrix
					objs.back().planes[i].poly[j] = mulMatrixPoi(TM, objs.back().planes[i].poly[j]);
				}
			}
			nowLoadP += nowPlaneNum;
		}
		else if (mode == 5) { //observer
			resetOne(EM);
			float saveP = PM[1][1];
			resetOne(PM);
			PM[1][1] = saveP;
			float em[4][4] = { 1.0, 0.0, 0.0, 0.0,
						  0.0, 1.0, 0.0, 0.0,
						  0.0, 0.0, 1.0, 0.0,
						  0.0, 0.0, 0.0, 1.0 };
			float grm[4][4] = { 1.0, 0.0, 0.0, 0.0,
						  0.0, 1.0, 0.0, 0.0,
						  0.0, 0.0, 1.0, 0.0,
						  0.0, 0.0, 0.0, 1.0 };
			float mirror[4][4] = { 1.0, 0.0, 0.0, 0.0,
						  0.0, 1.0, 0.0, 0.0,
						  0.0, 0.0, 1.0, 0.0,
						  0.0, 0.0, 0.0, 1.0 };
			float Tilt[4][4] = { 1.0, 0.0, 0.0, 0.0,
						  0.0, 1.0, 0.0, 0.0,
						  0.0, 0.0, 1.0, 0.0,
						  0.0, 0.0, 0.0, 1.0 };
			float Ex = getNum(file, 1);
			float Ey = getNum(file, 2);
			float Ez = getNum(file, 3);
			float COIx = getNum(file, 4);
			float COIy = getNum(file, 5);
			float COIz = getNum(file, 6);
			float tilt = getNum(file, 7);
			float hither = getNum(file, 8);
			float yon = getNum(file, 9);
			hav = getNum(file, 10);
			cout << Ex << " " << Ey << " " << Ez << " " << COIx << " " << COIy << " " << COIz << " " << tilt << " " << hither << " " << yon << " " << hav << "\n";
			//EM T(-Eye location)
			em[0][3] = 0 - Ex;
			em[1][3] = 0 - Ey;
			em[2][3] = 0 - Ez;
			cout << "TM\n";
			printM(em);
			//GRM
			poi v1, v2, v3;
			poi eyeV, topV;
			//resetOne(calM);
			eyeV.x = COIx - Ex; eyeV.y = COIy - Ey; eyeV.z = COIz - Ez;
			topV.x = 0; topV.y = 1; topV.z = 0;
			v3 = eyeV;
			grm[2][0] = v3.x / len_of_vector(v3); grm[2][1] = v3.y / len_of_vector(v3); grm[2][2] = v3.z / len_of_vector(v3);
			v1 = cross(topV, eyeV);
			grm[0][0] = v1.x / len_of_vector(v1); grm[0][1] = v1.y / len_of_vector(v1); grm[0][2] = v1.z / len_of_vector(v1);
			v2 = cross(v3, v1);
			grm[1][0] = v2.x / len_of_vector(v2); grm[1][1] = v2.y / len_of_vector(v2); grm[1][2] = v2.z / len_of_vector(v2);
			cout << "GRM\n";
			printM(grm);
			//Mirror
			mirror[0][0] = -1;
			cout << "Mirror\n";
			printM(mirror);
			//Tilt
			resetOne(calM);
			Tilt[0][0] = cos(tilt * PI);
			Tilt[1][1] = Tilt[0][0];
			Tilt[0][1] = sin(tilt * PI);
			Tilt[1][0] = 0 - Tilt[0][1];
			cout << "Tilt\n";
			printM(Tilt);

			mulMatrix(grm, em);
			loadMatrix(em, A);
			mulMatrix(mirror, em);
			loadMatrix(em, A);
			mulMatrix(Tilt, em);
			loadMatrix(em, A);
			loadMatrix(EM, em);
			//PM 
			//cout << "tan(hav*PI)" << tan(hav*PI) << "\n";
			PM[2][2] = yon * tan(hav * PI) / (yon - hither);
			//cout << "yon - hither" << yon - hither << "\n";
			PM[3][2] = tan(hav * PI);
			PM[2][3] = hither * yon * tan(hav * PI) / (hither - yon);
			PM[3][3] = 0.0;
			cout << "EM\n";
			printM(EM);
		}
		else if (mode == 6) { //viewport
			resetOne(WTV);
			vxmin = getNum(file, 1);
			vxmax = getNum(file, 2);
			vymin = getNum(file, 3);
			vymax = getNum(file, 4);
			cout << vxmin << " " << vxmax << " " << vymin << " " << vymax << "\n";
			float wxmin = -1, wxmax = 1;
			float wymin = -1, wymax = 1;
			float sx = ((vxmax - vxmin) / (wxmax - wxmin)) / (wxmax - wxmin) * width;
			float sy = ((vymax - vymin) / (wymax - wymin)) / (wymax - wymin) * height;
			float ratioX = (vxmin - wxmin) / (wxmax - wxmin);
			float ratioY = (vymin - wymin) / (wymax - wymin);
			//PM
			PM[1][1] = (vxmax - vxmin) / (vymax - vymin);
			//cout << "vymax - vymin" << vymax - vymin << "\n";
			cout << "PM\n";
			printM(PM);
			//WTV
			//T(-wxmin, -wymin)
			resetOne(calM);
			calM[0][3] = 1;//0 - wxmin;
			calM[1][3] = 1;//0 - wymin;
			calM[2][3] = 0;
			//cout << "TMM\n";
			//printM(calM);
			mulMatrix(calM, WTV);
			loadMatrix(WTV, A);
			resetOne(calM);
			//S(sx,sy)
			calM[0][0] = sx;
			calM[1][1] = sy;
			calM[2][2] = 1;
			//cout << "SM\n";
			//printM(calM);
			mulMatrix(calM, WTV);
			loadMatrix(WTV, A);
			resetOne(calM);
			//T(vxmin, vymin)
			calM[0][3] = ratioX * width;
			calM[1][3] = ratioY * height;
			calM[2][3] = 0;
			//cout << "TMM\n";
			//printM(calM);

			mulMatrix(calM, WTV);
			loadMatrix(WTV, A);
			resetOne(calM);
			cout << "WTV\n";
			printM(WTV);

		}
		else if (mode == 7) { //display
			//calculate dots and store back for line clip
			//cout << "*"<<objs.size()<<"\n";
			clearScreen();
			//reset num
			xa = 0, ya = 0;
			xb = 0, yb = 0;
			xd = 0, yd = 0;
			difX = 0;
			difY = 0;
			midX = 0;
			midY = 0;
			unsigned int i = 0, j = 0, p = 0;
			std::vector<object> now_objs;// store now points
			resetOne(calM);
			mulMatrix(PM, EM);
			loadMatrix(calM, A);
			//cout << "PM*EM\n";
			//printM(calM);
			for (p = 0; p < objs.size(); p++) {
				object obj_add;
				for (i = 0; i < objs[p].planes.size(); i++) {
					plane p_add;
					//PM * EM * objs[p].planes[i].poly[j]
					for (j = 0; j < objs[p].planes[i].poly.size(); j++) {

						//cout << "(" << objs[p].planes[i].poly[j].x << " " << objs[p].planes[i].poly[j].y << " " << objs[p].planes[i].poly[j].z <<" "<< objs[p].planes[i].poly[j].w<<")";
						poi add = mulMatrixPoi(calM, objs[p].planes[i].poly[j]);

						p_add.poly.push_back(add);
						//cout << "<" << add.x << " " << add.y << " " << add.z << " "<<add.w<<">\n";
					}
					p_add.count = objs[p].planes[i].count;
					obj_add.planes.push_back(p_add);
				}
				now_objs.push_back(obj_add);
			}

			//clip
			now_objs = clipping(now_objs);

			//perspective divide
			//cout << "total obj: " << now_objs.size() << "\n";
			for (p = 0; p < now_objs.size(); p++) {
				//cout << "obj" << p << " total i(3751): " << now_objs[p].planes.size() << "\n";
				for (i = 0; i < now_objs[p].planes.size(); i++) {
					//cout << "plane" << i << "\n";
					for (j = 0; j < now_objs[p].planes[i].poly.size(); j++) {
						//cout << "j" << j << ": ";
						float w = now_objs[p].planes[i].poly[j].w;
						now_objs[p].planes[i].poly[j].x /= w;
						now_objs[p].planes[i].poly[j].y /= w;
						now_objs[p].planes[i].poly[j].z /= w;
						now_objs[p].planes[i].poly[j].w /= w;
					}
				}
			}
			/*cout << "after perspective divide\n";
			for (p = 0; p < now_objs.size(); p++) {
				for (i = 0; i < now_objs[p].planes.size(); i++) {
					cout << i << ":\n";
					int k = 0;
					for (k = 0; k < now_objs[p].planes[i].poly.size(); k++) {
						cout << "(" << now_objs[p].planes[i].poly[k].x << " " << now_objs[p].planes[i].poly[k].y << " " << now_objs[p].planes[i].poly[k].z << " " << now_objs[p].planes[i].poly[k].w << ")\n";
					}
				}
			}*/

			//removeback
			if (noback == 1) {
				for (p = 0; p < now_objs.size(); p++) {
					int planesize = now_objs[p].planes.size();
					for (i = 0; i < planesize; i++) {
						if (now_objs[p].planes[i].poly.empty() == false) {
							//get two vector for cross
							poi a, b;
							a.x = now_objs[p].planes[i].poly.at(1).x - now_objs[p].planes[i].poly.at(0).x;
							a.y = now_objs[p].planes[i].poly.at(1).y - now_objs[p].planes[i].poly.at(0).y;
							a.z = now_objs[p].planes[i].poly.at(1).z - now_objs[p].planes[i].poly.at(0).z;
							b.x = now_objs[p].planes[i].poly.at(2).x - now_objs[p].planes[i].poly.at(1).x;
							b.y = now_objs[p].planes[i].poly.at(2).y - now_objs[p].planes[i].poly.at(1).y;
							b.z = now_objs[p].planes[i].poly.at(2).z - now_objs[p].planes[i].poly.at(1).z;
							poi c = cross(a, b);
							if (c.z >= 0) {
								//cout << "p i: " << p << " " << i << "\n";
								now_objs[p].planes[i].visible = 0;
							}
						}
						else {
							now_objs[p].planes[i].visible = 0;
						}
					}
				}
			}

			//WTV
			//cout << "total obj: " << now_objs.size() << "\n";
			for (p = 0; p < now_objs.size(); p++) {
				for (i = 0; i < now_objs[p].planes.size(); i++) {
					if (now_objs[p].planes[i].visible == 1 && noback == 1) {
						for (j = 0; j < now_objs[p].planes[i].poly.size(); j++) {
							poi save_back = mulMatrixPoi(WTV, now_objs[p].planes[i].poly[j]);
							now_objs[p].planes[i].poly[j] = save_back;
						}
					}
				}
			}
			glBegin(GL_POINTS);
			glPointSize(1);
			for (p = 0; p < now_objs.size(); p++) {
				//cout << "obj" << p << "\n";
				for (i = 0; i < now_objs[p].planes.size(); i++) {
					if (now_objs[p].planes[i].visible == 1 && noback == 1) {
						for (j = 0; j < now_objs[p].planes[i].poly.size(); j++) {
							poi puta = now_objs[p].planes[i].poly[j];
							poi putb;
							if (j != now_objs[p].planes[i].poly.size() - 1) {
								putb = now_objs[p].planes[i].poly[j + 1];
							}
							else {
								putb = now_objs[p].planes[i].poly[0];
							}
							//cout << "(" << puta.x << " " << puta.y << " " << puta.z << ") ";
							//cout << "(" << putb.x << " " << putb.y << " " << putb.z << ")\n";
							drawLine(puta, putb);
							//cout << "<" << x << " " << y << ")\n";
							//cout << "*";
							glColor3f(0.0, 1.0, 0.0);
							glVertex3f(x, y, 0.0);
						}
						//cout << "\n";
					}

				}
			}
			glEnd();
			glFlush();
			//draw
			now_objs.clear();
			system("pause");
		}
		else if (mode == 8) { //nobackfaces
			noback = 1;
		}
		else if (mode == 9) { //reset
			resetOne(TM);
		}
		else if (mode == 10) { //end
			exit(1);
			break;
		}
	}

	objs.clear();
	glutMainLoop();
	fileIn.close();
	system("pause");
	return 0;
}
