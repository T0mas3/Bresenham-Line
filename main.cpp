/*
 * main.cpp
 *
 *  Created on: Sep 21, 2013
 *      Author: tomas
 */

#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>

#include <iostream>
using namespace std;

#define GRID_SIZE 32

int windowSizeH = 600;
int windowSizeV = 600;

bool drawLine = false;

float beginX = 0;
float beginY = 0;

float endX = 1;
float endY = 1;

void transformScreenToWorldCoordinates(int screenX, int screenY, float &worldX, float &worldY) {

	worldX = ((float)screenX / (float)windowSizeH) * 2 - 1;
	worldY = ((float)screenY / (float)windowSizeV) * -2 + 1;
}

void colorGridCell(int x, int y) {

	float cornerLength = (float)(1.0/(GRID_SIZE/2)); // TODO optimize
	float topLeftX = (cornerLength * x) - 1;
	float topLeftY = (cornerLength * y) - 1;

	glColor3f(0, 0.5, 0.5);

	glBegin(GL_QUADS);
		glVertex2f(topLeftX, topLeftY);
		glVertex2f(topLeftX + cornerLength, topLeftY);

		glVertex2f(topLeftX + cornerLength, topLeftY + cornerLength);
		glVertex2f(topLeftX, topLeftY + cornerLength);
	glEnd();

}

void mouseClickCallback (int button, int state, int x, int y){

	if (button == GLUT_LEFT_BUTTON){

		if ((state == GLUT_DOWN)){
			transformScreenToWorldCoordinates(x, y, beginX, beginY);
			endX = beginX;
			endY = beginY;
			drawLine = true;

		}
		glutPostRedisplay();
	}
}

void mouseDragCallback(int x, int y){

	transformScreenToWorldCoordinates(x, y, endX, endY);

	glutPostRedisplay();
}

void drawUserLine(){

	glColor3f(1.0, 0.0, 0.0);

	glBegin(GL_LINES);
		glVertex2f(beginX, beginY);
		glVertex2f(endX, endY);
	glEnd();
}

void drawGrid(){

	glColor3f(0.5, 0.5, 0.5);
	int i;

	glBegin(GL_LINES);
	for (i = -(GRID_SIZE / 2); i <= (GRID_SIZE / 2); i++) { //TODO optimize
		float x = i / (float) (GRID_SIZE / 2);
		glVertex2f(x, -1.0);
		glVertex2f(x, 1.0);

		glVertex2f(-1.0, x);
		glVertex2f(1.0, x);
	}
	glEnd();
}

void display(void) {

	glClear(GL_COLOR_BUFFER_BIT);

	drawGrid();

	if (drawLine) {
		drawUserLine();
	}

	glFlush();
}

void init(void) {
}

int main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(windowSizeH, windowSizeV);
	glutInitWindowPosition(100, 50);
	glutCreateWindow("CG1");
	init();
	glutDisplayFunc(display);

	glutMouseFunc(mouseClickCallback);
	glutMotionFunc(mouseDragCallback);

	glutMainLoop();
	return 0;
}
