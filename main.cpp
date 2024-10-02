#define _CRT_SECURE_NO_WARNINGS

#include <GL/bmpfuncs.h>
#include <GL/glut.h>
#include "GL/glext.h"
#include <GL/freeglut.h>
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ObjParser/ObjParser.h"

double radius = 15;
double theta = 60, phi = 45;
double pi = 3.14159265;
double center[3] = { 0,0,0 };
double up[3] = { 0,1, 0}; // 구 좌표계

int current_width = 500;
int current_height = 500; // 출력 화면 크기

int chuckio = 0;
int pencilio = 0;
int basketio = 0;// 부품별 선택 여부를 저장하는 변수
int handleio = 0;// 0 : 미선택, 1 : 선택 

int chdl = 0; // 척 손잡이 조작에 따른 구멍의 개방 여부
// 0 : 미개방(close), 척 손잡이의 미조작
// 1 : 개방(open), 척 손잡이의 조작
// 구멍이 개방되어야 연필이 삽입 가능하다.

double ctr = 0;
double ptr = 0;
double btr = 0;
double htr = 0; // 부품별 평행/회전 이동량을 저장하는 변수
double hrot = 0; // hrot : 손잡이의 회전량,
double chdlrot = 0; //chdlrot : 척 손잡이의 회전량(이동량)

int dmode = 0; // 시범 모드(demonstration mode) 여부 저장 변수
// 0 : normal, 1 : demonstration mode

int dstep = 1; // 시범 모드 단계 저장 변수
/*1. 척을 앞으로 확장시킨다.
2. 척 손잡이를 당겨 척 구멍을 개방한다.
3. 개방된 구멍으로 연필을 삽입한다.
4. 척 손잡이를 다시 놓아 연필을 고정한다.
5. 연필깎이 손잡이를 회전시켜 연필을 깎는다.
6. 척 손잡이를 당겨 고정된 연필을 풀어준다.
7. 연필을 꺼낸다.
8. 척 손잡이를 놓아 준다.
9. 척을 다시 초기 자리로 넣는다.*/

int gotonext = 0; // 시범 모드 다음 단계 진행 가능 저장 변수
// 0 : 다음 단계 진행 중단, 1 : 다음 단계 진행

ObjParser* basket;
ObjParser* pencil;
ObjParser* chuck;
ObjParser* chuckhandle;
ObjParser* body;
ObjParser* handle; // 부품 오브젝트 변수

//GLUquadricObj* qobj = gluNewQuadric();

GLuint texturepencil;
GLuint texturebody;
GLuint texturebasket;
GLuint texturechuck;
GLuint texturehandle;
GLuint texturechuckhandle;// 부품 텍스쳐 변수

void resetall();
void light_setting();
void printinstruction();
void setTextureMapping();
void drawstringoverwindow();

void resetall() { // 처음으로 다시 초기화
	ctr = 0; ptr = 0; btr = 0; htr = 0;
	basketio = 0; pencilio = 0; chuckio = 0; handleio = 0;
	radius = 15;
	theta = 60, phi = 45;
	dmode = 0; dstep = 1;
	chdl = 0; chdlrot = 0; hrot = 0;
	glutPostRedisplay();
}

void init() {
	printf("init function called\n");
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	printinstruction();
	light_setting(); // lighting 기본 세팅
	GLfloat lmodel_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

	glEnable(GL_TEXTURE_2D);
	setTextureMapping(); // 텍스쳐들 바인딩
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); //GL_REPLACE : polygon의 원래 색상은 무시하고 texture로 덮음

}

void setTextureMapping() {
	int imgWidth, imgHeight, channels;
	uchar* img = readImageData("pencil.bmp", &imgWidth, &imgHeight, &channels);
	int texNum = 1;
	glGenTextures(texNum, &texturepencil); // 연필
	glBindTexture(GL_TEXTURE_2D, texturepencil);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//gluBuild2DMipmaps(GL_TEXTURE_2D, 3, imgWidth, imgHeight, GL_RGB, GL_UNSIGNED_BYTE, img);

	img = readImageData("basket.bmp", &imgWidth, &imgHeight, &channels);
	glGenTextures(texNum, &texturebasket); // 연필밥 받이
	glBindTexture(GL_TEXTURE_2D, texturebasket);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	img = readImageData("chuck.bmp", &imgWidth, &imgHeight, &channels);
	glGenTextures(texNum, &texturechuck); // 척(본체 앞부분)
	glBindTexture(GL_TEXTURE_2D, texturechuck);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	img = readImageData("handle.bmp", &imgWidth, &imgHeight, &channels);
	glGenTextures(texNum, &texturehandle); // 손잡이
	glBindTexture(GL_TEXTURE_2D, texturehandle);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	//GL_REPEAT 둘중 하나 선택
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	img = readImageData("body.bmp", &imgWidth, &imgHeight, &channels);
	glGenTextures(texNum, &texturebody); // 본체
	glBindTexture(GL_TEXTURE_2D, texturebody);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	//GL_REPEAT 둘중 하나 선택
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	img = readImageData("chuckhandle.bmp", &imgWidth, &imgHeight, &channels);
	glGenTextures(texNum, &texturechuckhandle); // 척 손잡이
	glBindTexture(GL_TEXTURE_2D, texturechuckhandle);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	//GL_REPEAT 둘중 하나 선택
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void draw_obj(ObjParser* objParser)
{
	glBegin(GL_TRIANGLES);
	for (unsigned int n = 0; n < objParser->getFaceSize(); n += 3) {
		glNormal3f(objParser->normal[objParser->normalIdx[n] - 1].x,
			objParser->normal[objParser->normalIdx[n] - 1].y,
			objParser->normal[objParser->normalIdx[n] - 1].z);
		glVertex3f(objParser->vertices[objParser->vertexIdx[n] - 1].x,
			objParser->vertices[objParser->vertexIdx[n] - 1].y,
			objParser->vertices[objParser->vertexIdx[n] - 1].z);

		glNormal3f(objParser->normal[objParser->normalIdx[n + 1] - 1].x,
			objParser->normal[objParser->normalIdx[n + 1] - 1].y,
			objParser->normal[objParser->normalIdx[n + 1] - 1].z);
		glVertex3f(objParser->vertices[objParser->vertexIdx[n + 1] - 1].x,
			objParser->vertices[objParser->vertexIdx[n + 1] - 1].y,
			objParser->vertices[objParser->vertexIdx[n + 1] - 1].z);

		glNormal3f(objParser->normal[objParser->normalIdx[n + 2] - 1].x,
			objParser->normal[objParser->normalIdx[n + 2] - 1].y,
			objParser->normal[objParser->normalIdx[n + 2] - 1].z);
		glVertex3f(objParser->vertices[objParser->vertexIdx[n + 2] - 1].x,
			objParser->vertices[objParser->vertexIdx[n + 2] - 1].y,
			objParser->vertices[objParser->vertexIdx[n + 2] - 1].z);
	}
	glEnd();
}

void draw_obj_with_texture(ObjParser* objParser, GLuint texture)
{
	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);	// texture 색 보존을 위한 enable
	glBindTexture(GL_TEXTURE_2D, texture);
	glBegin(GL_TRIANGLES);
	for (unsigned int n = 0; n < objParser->getFaceSize(); n += 3) {
		glTexCoord2f(objParser->textures[objParser->textureIdx[n] - 1].x,
			objParser->textures[objParser->textureIdx[n] - 1].y);
		glNormal3f(objParser->normal[objParser->normalIdx[n] - 1].x,
			objParser->normal[objParser->normalIdx[n] - 1].y,
			objParser->normal[objParser->normalIdx[n] - 1].z);
		glVertex3f(objParser->vertices[objParser->vertexIdx[n] - 1].x,
			objParser->vertices[objParser->vertexIdx[n] - 1].y,
			objParser->vertices[objParser->vertexIdx[n] - 1].z);

		glTexCoord2f(objParser->textures[objParser->textureIdx[n + 1] - 1].x,
			objParser->textures[objParser->textureIdx[n + 1] - 1].y);
		glNormal3f(objParser->normal[objParser->normalIdx[n + 1] - 1].x,
			objParser->normal[objParser->normalIdx[n + 1] - 1].y,
			objParser->normal[objParser->normalIdx[n + 1] - 1].z);
		glVertex3f(objParser->vertices[objParser->vertexIdx[n + 1] - 1].x,
			objParser->vertices[objParser->vertexIdx[n + 1] - 1].y,
			objParser->vertices[objParser->vertexIdx[n + 1] - 1].z);

		glTexCoord2f(objParser->textures[objParser->textureIdx[n + 2] - 1].x,
			objParser->textures[objParser->textureIdx[n + 2] - 1].y);
		glNormal3f(objParser->normal[objParser->normalIdx[n + 2] - 1].x,
			objParser->normal[objParser->normalIdx[n + 2] - 1].y,
			objParser->normal[objParser->normalIdx[n + 2] - 1].z);
		glVertex3f(objParser->vertices[objParser->vertexIdx[n + 2] - 1].x,
			objParser->vertices[objParser->vertexIdx[n + 2] - 1].y,
			objParser->vertices[objParser->vertexIdx[n + 2] - 1].z);
	}
	glEnd();
	glEnable(GL_BLEND);
}

//void mouse(int button, int state, int x, int y) {
//	printf("Mouse button is clicked! (%d, %d, %d, %d)\n", button, state, x, y);
//}
void mouseWheel(int but, int dir, int x, int y) {
	if (dir > 0 && radius > 2) {
		if (handleio == 1) { // 손잡이를 선택하였을 때는 줌 인 효과를 주지 않고
			hrot += 1; // 손잡이의 회전 이동만 구현한다. (시계방향)
		}
		else { // 손잡이 선택 이외의 상황에서는
			radius--; // 줌 인을 하면 된다.
		}
	}
	else if(radius < 100) {
		if (handleio == 1) { // 손잡이를 선택하였을 때는 줌 아웃 효과를 주지 않고
			hrot -= 1; // 손잡이의 회전 이동만 구현한다. (반시계방향)
		}
		else { // 손잡이 선택 이외의 상황에서는
			radius++; // 줌 아웃을 하면 된다.
		}
	}
}
//void motion(int x, int y) {
//	printf("Mouse is moving! (%d, %d)\n", x, y);
//}

void draw_string(void* font, const char* str, float x_position, float y_position, 
	float red, float green, float blue)
{
	glPushAttrib(GL_LIGHTING_BIT);
	glDisable(GL_LIGHTING);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(-5, 5, -5, 5);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
			glLoadIdentity();
			glColor3f(red, green, blue);
			glRasterPos3f(x_position, y_position, 0);
			for (unsigned int i = 0; i < strlen(str); i++) {
				glutBitmapCharacter(font, str[i]);
			}
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopAttrib();
}

void drawstringoverwindow() {
	if (dmode == 0) { // 일반 모드일 때
		if (chuckio == 1) { // 부품이 선택될 때 마다 부품 이름을 viewport에 출력
			draw_string(GLUT_BITMAP_TIMES_ROMAN_24,
				"chuck", -1, -4, 1, 1, 1);
		}
		else if (pencilio == 1) {
			draw_string(GLUT_BITMAP_TIMES_ROMAN_24,
				"pencil", -1, -4, 1, 1, 1);
		}
		else if (basketio == 1) {
			draw_string(GLUT_BITMAP_TIMES_ROMAN_24,
				"basket", -1, -4, 1, 1, 1);
		}
		else if (handleio == 1) {
			draw_string(GLUT_BITMAP_TIMES_ROMAN_24,
				"handle", -1, -4, 1, 1, 1);
		}
	}

	else if (dmode == 1) { // 시범 모드일 때
		draw_string(GLUT_BITMAP_TIMES_ROMAN_24, "demonstration mode", -2, -3, 1, 1, 1);
		for (int i = 1; i <= dstep; i++) {
			char str[20];
			sprintf(str, "step %d", i);
			if (dstep == i && gotonext == 1) {
				draw_string(GLUT_BITMAP_TIMES_ROMAN_24, str, -1, -4, 1, 1, 1); // 몇 번째 step인지 출력
			}
		}
	}
}

void resize(int width, int height) {
	current_width = width, current_height = height;
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (float)width / (float)height, 1, 500);
	glMatrixMode(GL_MODELVIEW);
}

void draw_axis(void)
{
	glLineWidth(3); // 좌표축의 두께
	glBegin(GL_LINES);
	glColor3f(1, 0, 0); // X축은 red
	glVertex3f(0, 0, 0);
	glVertex3f(4, 0, 0);
	glColor3f(0, 1, 0); // Y축은 green
	glVertex3f(0, 0, 0);
	glVertex3f(0, 4, 0);
	glColor3f(0, 0, 1); // Z축은 blue
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 4);
	glEnd();
	glLineWidth(1); // 두께 다시 환원
}

void light_setting() {
	GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat diffuseLight[] = { 1.0f, 1, 1, 1.0f };
	GLfloat specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat light_position[] = { 15, 15, 15, 1.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glFrontFace(GL_CW);
}

void printinstruction() {
	printf("-------instruction-------\n");
	printf("mouse up scroll : zoom in\n");
	printf("mouse down scroll : zoom out\n");
	printf("viewpoint : keyboard up, down, left, right\n");
	printf("-------------------------\n");
	printf("select part you want to move\n");
	printf("q : chuck, w : pencil, e : basket, r : handle\n");
	printf("after you select, i for insertion, o for extension\n");
	printf("-------------------------\n");
	printf("mouse right click for additional menus\n");
	printf("quit : terminates program\n");
	printf("reset : go back to initial state\n");
	printf("chuck hole : open, close\n");
	printf("demonstration : demonstrates how to use\n\n");
}

void emissionfunc(ObjParser* object, GLuint texture) { // 선택된 부품이 highlight되는 효과 부여
	GLfloat emission[] = { 0.5, 0.5, 0.5, 1.0f };
	glMaterialfv(GL_FRONT, GL_EMISSION, emission);
	draw_obj_with_texture(object, texture);// 
	GLfloat light_position[] = { 0.0, 0.0, 0.0, 1.0 };
	glLightfv(GL_LIGHT1, GL_POSITION, light_position); // 객체에서 빛을 내는 광원 하나 추가(GL_LIGHT1)
	emission[0] = 0.0f; emission[1] = 0.0f; emission[2] = 0.0f;
	glMaterialfv(GL_FRONT, GL_EMISSION, emission);
}

void draw() {
	double kz = radius * sin(theta * pi / 180) * cos(phi * pi / 180);
	double kx = radius * sin(theta * pi / 180) * sin(phi * pi / 180);
	double ky = radius * cos(theta * pi / 180);
	double upvector = cos(phi * pi / 180);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_DEPTH_TEST);
	gluLookAt(kx, ky, kz, 0, 0, 0,0, 1, 0);
	draw_string(GLUT_BITMAP_TIMES_ROMAN_24, "pencil sharpener", -2, 4, 1, 1, 1); // 중앙 상단에 제목 출력

	drawstringoverwindow(); // viewport에 글자 출력

	glPushMatrix(); // 본체 로컬(월드)
		draw_obj_with_texture(body, texturebody); 
	glPopMatrix();

	glPushMatrix(); // 연필 로컬
		glTranslatef(0, 0, 5); // 위치 보정
		glTranslatef(0, 0, -0.1 * ptr); // 연필의 평행이동
		if (dstep == 3 && gotonext == 1) { // 시연모드 : step 3. 연필 삽입
			if (ptr > 35) {
				printf("pencil fully inserted into hole, ready for sharpening\n");
				dstep = 4; gotonext = 0;
			}
			else ptr += 0.1;
		}
		else if (dstep == 7 && gotonext == 1) { // 시범모드 : step 7. 연필 꺼내기
			if (ptr < 0) {
				printf("pencil successfully released\n");
				dstep = 8; gotonext = 0;
			}
			else ptr -= 0.1;
		}
		if (pencilio == 1) {
			emissionfunc(pencil, texturepencil);
		}
		else draw_obj_with_texture(pencil, texturepencil);
	glPopMatrix();

	glPushMatrix(); // 연필밥 받이 로컬
		glTranslatef(0, 0, 0.1); // 위치 보정
		glTranslatef(0, 0, 0.2 * btr); // 연필밥 받이의 평행이동
		if (basketio == 1) {
			emissionfunc(basket, texturebasket); // 선택되었을 때 emission으로 강조
		}
		else draw_obj_with_texture(basket, texturebasket);
	glPopMatrix();

	glPushMatrix(); // 척(본체 앞부분) 로컬
		glTranslatef(0, 0, -0.65); // 위치 보정
		glTranslatef(0, 0, 0.1 * ctr); // 척의 평행이동
		if (dstep == 1 && gotonext == 1) { // 시범모드 : step 1. 척 당기기
			if (ctr > 8) {
				printf("chuck pulled out\n");
				dstep = 2; gotonext = 0;
			}
			else ctr += 0.04;
		}
		else if (dstep == 9 && gotonext == 1) { // 시범모드 : step 9. 척 원위치
			if (ctr < 0) {
				printf("chuck pulled out\n"); 
				printf("demonstration is over. back to normal mode\n"); // 시범모드 끝! 다시 일반 모드로
				dmode = 0; gotonext = 0;
			}
			else ctr -= 0.04;
		}
		if (chuckio == 1) {
			emissionfunc(chuck, texturechuck); // 선택되었을 때 emission으로 강조
		}
		else draw_obj_with_texture(chuck, texturechuck);
		// 척 손잡이 로컬
		glTranslatef(0, 0, .65); // 위치 보정
		glRotatef(chdlrot, 0, 0, 1); // 척 손잡이의 회전이동
		if ((dstep == 2 || dstep == 6) && gotonext == 1) { // 시범모드 : step 2와 6. 척 손잡이 조작으로 척 구멍 개방
			if (chdlrot > 7) {
				printf("opened the hole\n");
				if (dstep == 2) { // step 2였으면 3로 이동
					dstep = 3; gotonext = 0;
				}
				else if (dstep == 6) { // step 6였으면 7로 이동
					dstep = 7; gotonext = 0;
				}
				
			}
			else chdlrot += 0.04;
		}
		else if ((dstep == 4 || dstep == 8) && gotonext == 1) { // 시범모드 : step 4. 척 손잡이를 놓아주어 연필 고정
			if (chdlrot < 0) {									// step 8. 척 손잡이 놓아주기
				if (dstep == 4) { // step 4였으면 5로 이동
					printf("pencil locked\n");
					dstep = 5; gotonext = 0;
				}
				else if (dstep == 8) { // step 8였으면 9로 이동
					printf("chuck handle released\n");
					dstep = 9; gotonext = 0;
				}
			}
			else chdlrot -= 0.04;
		}
		draw_obj_with_texture(chuckhandle, texturechuckhandle);
	glPopMatrix();

	glPushMatrix(); // 손잡이 로컬
		glTranslatef(0, 0, -0.1 * htr); // 손잡이의 평행이동
		glTranslatef(0, 0.72, 0); // 손잡이 위쪽 부분의 원을 중심으로 손잡이는 회전해야 한다
		glRotatef(20*hrot, 0, 0, 1);
		glTranslatef(0, -0.72, 0);
		if (dstep == 5 && gotonext == 1) { // 시범모드 : step 5. 손잡이를 회전시켜 연필 깎기
			if (hrot > 72) {
				printf("sharpening finished\n");
				dstep = 6; gotonext = 0;
			}
			else hrot += 0.1;
		}
		if (handleio == 1) {
			emissionfunc(handle, texturehandle); // 선택되었을 떄 emission으로 강조
		}
		else draw_obj_with_texture(handle, texturehandle);
	glPopMatrix();


	glFlush();
	glutPostRedisplay();
	glutSwapBuffers();
}

bool pencilinserted() { // 연필이 척 구멍을 통과하였는지 판단
	return ctr + ptr > 17;
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'q': // 척(본체 앞부분) 선택
		pencilio = 0; basketio = 0; handleio = 0; // 다른 변수들은 다 초기화
		chuckio = 1; // 척이 선택되었음
		printf("chuck is selected\n");
		break;
	case 'w': // 연필 선택
		basketio = 0; handleio = 0; chuckio = 0;
		pencilio = 1;
		printf("pencil is selected\n");
		break;
	case 'e': // 연필밥 받이 선택
		chuckio = 0; pencilio = 0; handleio = 0;
		basketio = 1;
		printf("basket is selected\n");
		break;
	case 'r': // 손잡이 선택
		pencilio = 0; chuckio = 0; basketio = 0;
		handleio = 1;
		printf("handle is selected\n");
		break;
	default : 
		break;
	}
	
	if (key == 'o') {
		if (chuckio == 1) { // 척이 선택되었고, 최대 확장 전까지는 확장
			if (ctr >= 8) printf("chuck extended to maximum range\n"); // 척이 최대 확장 길이만큼 확장되었다는 메세지 출력
			else if (ctr + ptr == 17) printf("chuck can't extend, chuck hole is closed\n"); // 연필은 가만히 있고, 척이 확장되어서 충돌할 때
			else if (pencilinserted()) printf("chuck locked\n"); // 연필이 삽입된 상태에서는 척이 움직일 수 없음
			else ctr += 1;
		}
		else if (pencilio == 1) {
			if (chdl == 0) {
				if (pencilinserted()) printf("pencil is locked by chuck hole, extraction unable\n");
				// 구멍이 open 상태에서 연필이 통과한 적이 있고, 그 다음에 구멍을 잠그면 연필은 고정되어 움직일 수 없다.
				else ptr -= 1; // 구멍이 닫혀 있어도 연필이 척 구멍에 도달하기 전까지는 적출이 가능
			}
			else {
				ptr -= 1; // 척 구멍이 열려 있을 때는 자유롭게 적출 가능
			}
		}
		else if (basketio == 1) { // 연필밥 받이가 선택되었고, 최대 확장 전까지는 확장

			if (btr >= 13) printf("basket fully taken out\n"); // 연필밥 받이가 최대로 확장되었다는 메세지 출력
			else btr += 1;
		}
		else if (handleio == 1) htr += 1;
	}
	
	if (key == 'i') {
		if (chuckio == 1) { // 척이 선택되었을 때 축소
			if (ctr <= 0) printf("chuck is closed\n");
			else if (pencilinserted()) printf("chuck locked\n"); //연필이 삽입된 상태에서는 척이 움직일 수 없음
			else ctr -= 1;
		}
		else if (pencilio == 1) { // 연필이 선택되었고, 최대 삽입 길이 전까지는 삽입
			if (ptr >= 35) printf("pencil inserted to maximum depth\n"); // 연필이 끝까지 삽입되었다는 메세지 출력
			else {
				if (chdl <= 0) {
					if (pencilinserted()) printf("pencil is locked by chuck hole, insertion unable\n");
					// 구멍이 open 상태에서 연필이 통과한 적이 있고, 그 다음에 구멍을 잠그면 연필은 고정되어 움직일 수 없다.
					else if (ctr + ptr == 17) printf("pencil cannot be inserted(chuck hole is closed!!)\n"); // 척 구멍이 닫혀 있을 때는 연필을 삽입할 수 없다.
					else ptr += 1; // 구멍이 닫혀 있어도 연필이 척 구멍에 도달하기 전까지는 삽입/적출이 가능
				}
				else {
					ptr += 1; // 척 구멍이 열려 있을 때는 자유롭게 삽입 가능
				}
			}
		}
		else if (basketio == 1) { // 연필밥 받이가 선택되었을 때 축소
			if (btr <= 0) printf("basket is closed\n");
			else btr -= 1;
		}
		else if (handleio == 1) { // 손잡이가 선택되었을 때 축소
			if (htr <= 0) printf("handle inserted properly\n");
			else htr -= 1;
		}
		 
	}

	if (dmode == 1 && key == 'k') { // demonstration mode에서 'k'를 누르며 step 진행
		gotonext = 1;
		if (dstep == 1) {
			printf("step 1. pull chuck out\n"); Sleep(1000);
		}
		else if (dstep == 2) {
			printf("step 2. move chuck handle to left to open the hole and hold it\n"); Sleep(1000);
		}
		else if (dstep == 3) {
			printf("step 3. insert pencil into the hole\n"); Sleep(1000);
		}
		else if (dstep == 4) {
			printf("step 4. release chuck handle to hold pencil\n"); Sleep(1000);
		}
		else if (dstep == 5) {
			printf("step 5. now rotate handle to sharpen pencil\n"); Sleep(1000);
		}
		else if (dstep == 6) {
			printf("step 6. move chuck handle to left again to release pencil\n"); Sleep(1000);
		}
		else if (dstep == 7) {
			printf("step 7. release pencil\n"); Sleep(1000);
		}
		else if (dstep == 8) {
			printf("step 8. release chuck handle\n"); Sleep(1000);
		}
		else if (dstep == 9) {
			printf("step 9. push chuck in\n"); Sleep(1000);
		}

	}
	else if (dmode == 1 && key != 'k') { // demonstration mode 실행하고 'k' 외 다른 키를 누른다면
		printf("demonstration mode cancelled..\n");
		dmode = 0; // demonstration mode 해제
	}

	glFlush();
	glutPostRedisplay();
}

void specialkeyboard(int key, int x, int y) { //  구 좌표계에서 시점 변경 함수
	if (key == GLUT_KEY_LEFT) {
		phi -= 3;
		if (phi < 0) phi += 360;
	}
	else if (key == GLUT_KEY_RIGHT) {
		phi += 3;
		if (phi > 360) phi -= 360;
	}
	else if (key == GLUT_KEY_UP) {
		theta -= 3;
		if (theta <= 0) {
			theta += 360;
		}
	}
	else if (key == GLUT_KEY_DOWN) {
		theta += 3;
		if (theta > 360) theta -= 360;
	}

}

void idle() {

	if (theta > 180 && theta <= 360) up[1] = -1;
	else up[1] = 1;
	
}

void sub_menu_function(int option)
{
	if (option == 1) {
		chdl = 1; // 구멍은 open, 척 손잡이의 조작
		chdlrot = 7; // 척 손잡이의 조작으로 손잡이의 회전량 반영
		printf("chuck hole opened\n");
	}
	else if (option == 2){
		chdl = 0; // 구멍은 close
		chdlrot = 0; // 척 손잡이는 원래 위치로
		printf("chuck hole closed\n");
	}

}

void main_menu_function(int option)
{
	if (option == 999) { // 종료 옵션
		exit(0);
	} 
	else if (option == 11) { // 초기화 옵션
		printf("reset to initial state\n");
		resetall();
	}
	else if (option == 12) { // 사용 설명 출력
		printinstruction();
	}
	else if (option == 1) { // 시범 모드(demonstration mode) 시작
		printf("-----demonstration mode-----\n");
		printf("we will show you how to use mechanical pencil sharpener\n");
		resetall();
		dmode = 1; // 시범 모드임을 저장
		printf("press k to continue every step\n"); // k를 매번 누를 때마다 매 단계 실행
		printf("press any key except k to cancel mode\n"); // 시범 모드를 중단하고 싶으면 k를 제외한 아무 키를 누르면 된다.
	}

}

int main(int argc, char** argv)
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(300, 300);
	glutCreateWindow("12181746 final project");
	init();

	basket = new ObjParser("basket.obj"); 
	pencil = new ObjParser("pencil.obj");
	chuck = new ObjParser("chuck.obj");
	body = new ObjParser("body.obj");
	handle = new ObjParser("handle.obj");
	chuckhandle = new ObjParser("chuckhandle.obj");
	//glutMouseFunc(mouse);
	//glutMotionFunc(motion);
	glutMouseWheelFunc(mouseWheel);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	glutReshapeFunc(resize);
	glutDisplayFunc(draw);
	glutSpecialFunc(specialkeyboard);

	int submenu1;
	submenu1 = glutCreateMenu(sub_menu_function);
	glutAddMenuEntry("Open", 1);
	glutAddMenuEntry("Close", 2);

	glutCreateMenu(main_menu_function);
	glutAddMenuEntry("Quit", 999);
	glutAddMenuEntry("Reset", 11);
	glutAddMenuEntry("Instruction", 12);
	glutAddSubMenu("chuck hole", submenu1);
	glutAddMenuEntry("demonstration", 1);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutMainLoop();

	return 0;

}


