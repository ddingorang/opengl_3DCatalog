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
double up[3] = { 0,1, 0}; // �� ��ǥ��

int current_width = 500;
int current_height = 500; // ��� ȭ�� ũ��

int chuckio = 0;
int pencilio = 0;
int basketio = 0;// ��ǰ�� ���� ���θ� �����ϴ� ����
int handleio = 0;// 0 : �̼���, 1 : ���� 

int chdl = 0; // ô ������ ���ۿ� ���� ������ ���� ����
// 0 : �̰���(close), ô �������� ������
// 1 : ����(open), ô �������� ����
// ������ ����Ǿ�� ������ ���� �����ϴ�.

double ctr = 0;
double ptr = 0;
double btr = 0;
double htr = 0; // ��ǰ�� ����/ȸ�� �̵����� �����ϴ� ����
double hrot = 0; // hrot : �������� ȸ����,
double chdlrot = 0; //chdlrot : ô �������� ȸ����(�̵���)

int dmode = 0; // �ù� ���(demonstration mode) ���� ���� ����
// 0 : normal, 1 : demonstration mode

int dstep = 1; // �ù� ��� �ܰ� ���� ����
/*1. ô�� ������ Ȯ���Ų��.
2. ô �����̸� ��� ô ������ �����Ѵ�.
3. ����� �������� ������ �����Ѵ�.
4. ô �����̸� �ٽ� ���� ������ �����Ѵ�.
5. ���ʱ��� �����̸� ȸ������ ������ ��´�.
6. ô �����̸� ��� ������ ������ Ǯ���ش�.
7. ������ ������.
8. ô �����̸� ���� �ش�.
9. ô�� �ٽ� �ʱ� �ڸ��� �ִ´�.*/

int gotonext = 0; // �ù� ��� ���� �ܰ� ���� ���� ���� ����
// 0 : ���� �ܰ� ���� �ߴ�, 1 : ���� �ܰ� ����

ObjParser* basket;
ObjParser* pencil;
ObjParser* chuck;
ObjParser* chuckhandle;
ObjParser* body;
ObjParser* handle; // ��ǰ ������Ʈ ����

//GLUquadricObj* qobj = gluNewQuadric();

GLuint texturepencil;
GLuint texturebody;
GLuint texturebasket;
GLuint texturechuck;
GLuint texturehandle;
GLuint texturechuckhandle;// ��ǰ �ؽ��� ����

void resetall();
void light_setting();
void printinstruction();
void setTextureMapping();
void drawstringoverwindow();

void resetall() { // ó������ �ٽ� �ʱ�ȭ
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
	light_setting(); // lighting �⺻ ����
	GLfloat lmodel_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

	glEnable(GL_TEXTURE_2D);
	setTextureMapping(); // �ؽ��ĵ� ���ε�
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); //GL_REPLACE : polygon�� ���� ������ �����ϰ� texture�� ����

}

void setTextureMapping() {
	int imgWidth, imgHeight, channels;
	uchar* img = readImageData("pencil.bmp", &imgWidth, &imgHeight, &channels);
	int texNum = 1;
	glGenTextures(texNum, &texturepencil); // ����
	glBindTexture(GL_TEXTURE_2D, texturepencil);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//gluBuild2DMipmaps(GL_TEXTURE_2D, 3, imgWidth, imgHeight, GL_RGB, GL_UNSIGNED_BYTE, img);

	img = readImageData("basket.bmp", &imgWidth, &imgHeight, &channels);
	glGenTextures(texNum, &texturebasket); // ���ʹ� ����
	glBindTexture(GL_TEXTURE_2D, texturebasket);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	img = readImageData("chuck.bmp", &imgWidth, &imgHeight, &channels);
	glGenTextures(texNum, &texturechuck); // ô(��ü �պκ�)
	glBindTexture(GL_TEXTURE_2D, texturechuck);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	img = readImageData("handle.bmp", &imgWidth, &imgHeight, &channels);
	glGenTextures(texNum, &texturehandle); // ������
	glBindTexture(GL_TEXTURE_2D, texturehandle);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	//GL_REPEAT ���� �ϳ� ����
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	img = readImageData("body.bmp", &imgWidth, &imgHeight, &channels);
	glGenTextures(texNum, &texturebody); // ��ü
	glBindTexture(GL_TEXTURE_2D, texturebody);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	//GL_REPEAT ���� �ϳ� ����
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	img = readImageData("chuckhandle.bmp", &imgWidth, &imgHeight, &channels);
	glGenTextures(texNum, &texturechuckhandle); // ô ������
	glBindTexture(GL_TEXTURE_2D, texturechuckhandle);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	//GL_REPEAT ���� �ϳ� ����
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
	glEnable(GL_TEXTURE_2D);	// texture �� ������ ���� enable
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
		if (handleio == 1) { // �����̸� �����Ͽ��� ���� �� �� ȿ���� ���� �ʰ�
			hrot += 1; // �������� ȸ�� �̵��� �����Ѵ�. (�ð����)
		}
		else { // ������ ���� �̿��� ��Ȳ������
			radius--; // �� ���� �ϸ� �ȴ�.
		}
	}
	else if(radius < 100) {
		if (handleio == 1) { // �����̸� �����Ͽ��� ���� �� �ƿ� ȿ���� ���� �ʰ�
			hrot -= 1; // �������� ȸ�� �̵��� �����Ѵ�. (�ݽð����)
		}
		else { // ������ ���� �̿��� ��Ȳ������
			radius++; // �� �ƿ��� �ϸ� �ȴ�.
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
	if (dmode == 0) { // �Ϲ� ����� ��
		if (chuckio == 1) { // ��ǰ�� ���õ� �� ���� ��ǰ �̸��� viewport�� ���
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

	else if (dmode == 1) { // �ù� ����� ��
		draw_string(GLUT_BITMAP_TIMES_ROMAN_24, "demonstration mode", -2, -3, 1, 1, 1);
		for (int i = 1; i <= dstep; i++) {
			char str[20];
			sprintf(str, "step %d", i);
			if (dstep == i && gotonext == 1) {
				draw_string(GLUT_BITMAP_TIMES_ROMAN_24, str, -1, -4, 1, 1, 1); // �� ��° step���� ���
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
	glLineWidth(3); // ��ǥ���� �β�
	glBegin(GL_LINES);
	glColor3f(1, 0, 0); // X���� red
	glVertex3f(0, 0, 0);
	glVertex3f(4, 0, 0);
	glColor3f(0, 1, 0); // Y���� green
	glVertex3f(0, 0, 0);
	glVertex3f(0, 4, 0);
	glColor3f(0, 0, 1); // Z���� blue
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 4);
	glEnd();
	glLineWidth(1); // �β� �ٽ� ȯ��
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

void emissionfunc(ObjParser* object, GLuint texture) { // ���õ� ��ǰ�� highlight�Ǵ� ȿ�� �ο�
	GLfloat emission[] = { 0.5, 0.5, 0.5, 1.0f };
	glMaterialfv(GL_FRONT, GL_EMISSION, emission);
	draw_obj_with_texture(object, texture);// 
	GLfloat light_position[] = { 0.0, 0.0, 0.0, 1.0 };
	glLightfv(GL_LIGHT1, GL_POSITION, light_position); // ��ü���� ���� ���� ���� �ϳ� �߰�(GL_LIGHT1)
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
	draw_string(GLUT_BITMAP_TIMES_ROMAN_24, "pencil sharpener", -2, 4, 1, 1, 1); // �߾� ��ܿ� ���� ���

	drawstringoverwindow(); // viewport�� ���� ���

	glPushMatrix(); // ��ü ����(����)
		draw_obj_with_texture(body, texturebody); 
	glPopMatrix();

	glPushMatrix(); // ���� ����
		glTranslatef(0, 0, 5); // ��ġ ����
		glTranslatef(0, 0, -0.1 * ptr); // ������ �����̵�
		if (dstep == 3 && gotonext == 1) { // �ÿ���� : step 3. ���� ����
			if (ptr > 35) {
				printf("pencil fully inserted into hole, ready for sharpening\n");
				dstep = 4; gotonext = 0;
			}
			else ptr += 0.1;
		}
		else if (dstep == 7 && gotonext == 1) { // �ù���� : step 7. ���� ������
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

	glPushMatrix(); // ���ʹ� ���� ����
		glTranslatef(0, 0, 0.1); // ��ġ ����
		glTranslatef(0, 0, 0.2 * btr); // ���ʹ� ������ �����̵�
		if (basketio == 1) {
			emissionfunc(basket, texturebasket); // ���õǾ��� �� emission���� ����
		}
		else draw_obj_with_texture(basket, texturebasket);
	glPopMatrix();

	glPushMatrix(); // ô(��ü �պκ�) ����
		glTranslatef(0, 0, -0.65); // ��ġ ����
		glTranslatef(0, 0, 0.1 * ctr); // ô�� �����̵�
		if (dstep == 1 && gotonext == 1) { // �ù���� : step 1. ô ����
			if (ctr > 8) {
				printf("chuck pulled out\n");
				dstep = 2; gotonext = 0;
			}
			else ctr += 0.04;
		}
		else if (dstep == 9 && gotonext == 1) { // �ù���� : step 9. ô ����ġ
			if (ctr < 0) {
				printf("chuck pulled out\n"); 
				printf("demonstration is over. back to normal mode\n"); // �ù���� ��! �ٽ� �Ϲ� ����
				dmode = 0; gotonext = 0;
			}
			else ctr -= 0.04;
		}
		if (chuckio == 1) {
			emissionfunc(chuck, texturechuck); // ���õǾ��� �� emission���� ����
		}
		else draw_obj_with_texture(chuck, texturechuck);
		// ô ������ ����
		glTranslatef(0, 0, .65); // ��ġ ����
		glRotatef(chdlrot, 0, 0, 1); // ô �������� ȸ���̵�
		if ((dstep == 2 || dstep == 6) && gotonext == 1) { // �ù���� : step 2�� 6. ô ������ �������� ô ���� ����
			if (chdlrot > 7) {
				printf("opened the hole\n");
				if (dstep == 2) { // step 2������ 3�� �̵�
					dstep = 3; gotonext = 0;
				}
				else if (dstep == 6) { // step 6������ 7�� �̵�
					dstep = 7; gotonext = 0;
				}
				
			}
			else chdlrot += 0.04;
		}
		else if ((dstep == 4 || dstep == 8) && gotonext == 1) { // �ù���� : step 4. ô �����̸� �����־� ���� ����
			if (chdlrot < 0) {									// step 8. ô ������ �����ֱ�
				if (dstep == 4) { // step 4������ 5�� �̵�
					printf("pencil locked\n");
					dstep = 5; gotonext = 0;
				}
				else if (dstep == 8) { // step 8������ 9�� �̵�
					printf("chuck handle released\n");
					dstep = 9; gotonext = 0;
				}
			}
			else chdlrot -= 0.04;
		}
		draw_obj_with_texture(chuckhandle, texturechuckhandle);
	glPopMatrix();

	glPushMatrix(); // ������ ����
		glTranslatef(0, 0, -0.1 * htr); // �������� �����̵�
		glTranslatef(0, 0.72, 0); // ������ ���� �κ��� ���� �߽����� �����̴� ȸ���ؾ� �Ѵ�
		glRotatef(20*hrot, 0, 0, 1);
		glTranslatef(0, -0.72, 0);
		if (dstep == 5 && gotonext == 1) { // �ù���� : step 5. �����̸� ȸ������ ���� ���
			if (hrot > 72) {
				printf("sharpening finished\n");
				dstep = 6; gotonext = 0;
			}
			else hrot += 0.1;
		}
		if (handleio == 1) {
			emissionfunc(handle, texturehandle); // ���õǾ��� �� emission���� ����
		}
		else draw_obj_with_texture(handle, texturehandle);
	glPopMatrix();


	glFlush();
	glutPostRedisplay();
	glutSwapBuffers();
}

bool pencilinserted() { // ������ ô ������ ����Ͽ����� �Ǵ�
	return ctr + ptr > 17;
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'q': // ô(��ü �պκ�) ����
		pencilio = 0; basketio = 0; handleio = 0; // �ٸ� �������� �� �ʱ�ȭ
		chuckio = 1; // ô�� ���õǾ���
		printf("chuck is selected\n");
		break;
	case 'w': // ���� ����
		basketio = 0; handleio = 0; chuckio = 0;
		pencilio = 1;
		printf("pencil is selected\n");
		break;
	case 'e': // ���ʹ� ���� ����
		chuckio = 0; pencilio = 0; handleio = 0;
		basketio = 1;
		printf("basket is selected\n");
		break;
	case 'r': // ������ ����
		pencilio = 0; chuckio = 0; basketio = 0;
		handleio = 1;
		printf("handle is selected\n");
		break;
	default : 
		break;
	}
	
	if (key == 'o') {
		if (chuckio == 1) { // ô�� ���õǾ���, �ִ� Ȯ�� �������� Ȯ��
			if (ctr >= 8) printf("chuck extended to maximum range\n"); // ô�� �ִ� Ȯ�� ���̸�ŭ Ȯ��Ǿ��ٴ� �޼��� ���
			else if (ctr + ptr == 17) printf("chuck can't extend, chuck hole is closed\n"); // ������ ������ �ְ�, ô�� Ȯ��Ǿ �浹�� ��
			else if (pencilinserted()) printf("chuck locked\n"); // ������ ���Ե� ���¿����� ô�� ������ �� ����
			else ctr += 1;
		}
		else if (pencilio == 1) {
			if (chdl == 0) {
				if (pencilinserted()) printf("pencil is locked by chuck hole, extraction unable\n");
				// ������ open ���¿��� ������ ����� ���� �ְ�, �� ������ ������ ��׸� ������ �����Ǿ� ������ �� ����.
				else ptr -= 1; // ������ ���� �־ ������ ô ���ۿ� �����ϱ� �������� ������ ����
			}
			else {
				ptr -= 1; // ô ������ ���� ���� ���� �����Ӱ� ���� ����
			}
		}
		else if (basketio == 1) { // ���ʹ� ���̰� ���õǾ���, �ִ� Ȯ�� �������� Ȯ��

			if (btr >= 13) printf("basket fully taken out\n"); // ���ʹ� ���̰� �ִ�� Ȯ��Ǿ��ٴ� �޼��� ���
			else btr += 1;
		}
		else if (handleio == 1) htr += 1;
	}
	
	if (key == 'i') {
		if (chuckio == 1) { // ô�� ���õǾ��� �� ���
			if (ctr <= 0) printf("chuck is closed\n");
			else if (pencilinserted()) printf("chuck locked\n"); //������ ���Ե� ���¿����� ô�� ������ �� ����
			else ctr -= 1;
		}
		else if (pencilio == 1) { // ������ ���õǾ���, �ִ� ���� ���� �������� ����
			if (ptr >= 35) printf("pencil inserted to maximum depth\n"); // ������ ������ ���ԵǾ��ٴ� �޼��� ���
			else {
				if (chdl <= 0) {
					if (pencilinserted()) printf("pencil is locked by chuck hole, insertion unable\n");
					// ������ open ���¿��� ������ ����� ���� �ְ�, �� ������ ������ ��׸� ������ �����Ǿ� ������ �� ����.
					else if (ctr + ptr == 17) printf("pencil cannot be inserted(chuck hole is closed!!)\n"); // ô ������ ���� ���� ���� ������ ������ �� ����.
					else ptr += 1; // ������ ���� �־ ������ ô ���ۿ� �����ϱ� �������� ����/������ ����
				}
				else {
					ptr += 1; // ô ������ ���� ���� ���� �����Ӱ� ���� ����
				}
			}
		}
		else if (basketio == 1) { // ���ʹ� ���̰� ���õǾ��� �� ���
			if (btr <= 0) printf("basket is closed\n");
			else btr -= 1;
		}
		else if (handleio == 1) { // �����̰� ���õǾ��� �� ���
			if (htr <= 0) printf("handle inserted properly\n");
			else htr -= 1;
		}
		 
	}

	if (dmode == 1 && key == 'k') { // demonstration mode���� 'k'�� ������ step ����
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
	else if (dmode == 1 && key != 'k') { // demonstration mode �����ϰ� 'k' �� �ٸ� Ű�� �����ٸ�
		printf("demonstration mode cancelled..\n");
		dmode = 0; // demonstration mode ����
	}

	glFlush();
	glutPostRedisplay();
}

void specialkeyboard(int key, int x, int y) { //  �� ��ǥ�迡�� ���� ���� �Լ�
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
		chdl = 1; // ������ open, ô �������� ����
		chdlrot = 7; // ô �������� �������� �������� ȸ���� �ݿ�
		printf("chuck hole opened\n");
	}
	else if (option == 2){
		chdl = 0; // ������ close
		chdlrot = 0; // ô �����̴� ���� ��ġ��
		printf("chuck hole closed\n");
	}

}

void main_menu_function(int option)
{
	if (option == 999) { // ���� �ɼ�
		exit(0);
	} 
	else if (option == 11) { // �ʱ�ȭ �ɼ�
		printf("reset to initial state\n");
		resetall();
	}
	else if (option == 12) { // ��� ���� ���
		printinstruction();
	}
	else if (option == 1) { // �ù� ���(demonstration mode) ����
		printf("-----demonstration mode-----\n");
		printf("we will show you how to use mechanical pencil sharpener\n");
		resetall();
		dmode = 1; // �ù� ������� ����
		printf("press k to continue every step\n"); // k�� �Ź� ���� ������ �� �ܰ� ����
		printf("press any key except k to cancel mode\n"); // �ù� ��带 �ߴ��ϰ� ������ k�� ������ �ƹ� Ű�� ������ �ȴ�.
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


