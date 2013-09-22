/*
 * main.cpp
 *
 *  Created on: Sep 21, 2013
 *      Author: tomas
 */

#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>

#define GRID_SIZE 32

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

//  Sample rect
//	glVertex2f(-0.5, 0.5);
//	glVertex2f(0.5, 0.5);
//
//	glVertex2f(0.5, -0.5);
//	glVertex2f(-0.5, -0.5);

}

void display(void) {
	int i;
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0.5, 0.5, 0.5);

	glBegin(GL_LINES);
	for (i = -(GRID_SIZE/2); i <= (GRID_SIZE/2); i++) { //TODO optimize
		float x = i / (float)(GRID_SIZE/2);
		glVertex2f(x, -1.0);
		glVertex2f(x, 1.0);

		glVertex2f(-1.0, x);
		glVertex2f(1.0, x);
	}
	glEnd();

	colorGridCell(0, 0);
	colorGridCell(31, 31);
	colorGridCell(0, 31);
	colorGridCell(31, 0);

	glFlush();
}

void init(void) {
}

int main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(100, 50);
	glutCreateWindow("CG1");
	init();
	glutDisplayFunc(display);
	glutMainLoop();
	return 0;
}
