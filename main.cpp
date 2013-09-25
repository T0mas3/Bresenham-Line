/*
 * main.cpp
 *
 *  Created on: Sep 21, 2013
 *      Author: tomas
 */

#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
#include <iostream> // TODO unused?
#include <math.h>       /* fabs */
#include <sys/time.h>
#include <unistd.h>
using namespace std;

#define GRID_SIZE 32
#define RAND_SEED 1

int windowSizeH = 600;
int windowSizeV = 600;

bool shouldDrawUserLine = false;
bool shouldDrawBresenhamLine = false;
bool snapToGrid = true;

float beginX = 0;
float beginY = 0;
float endX = 0;
float endY = 0;

// TODO better titles
int gridBeginX = 0;
int gridBeginY = 0;
int gridEndX = 0;
int gridEndY = 0;


void transformScreenToWorldCoordinates(int screenX, int screenY, float &worldX, float &worldY) {

	worldX = ((float)screenX / (float)windowSizeH) * 2 - 1;
	worldY = ((float)screenY / (float)windowSizeV) * -2 + 1;
}

void transformWorldToGridCoordinates(float worldX, float worldY, int &gridX, int &gridY){
	gridX = (int)( ( ( worldX+1)/2 ) * GRID_SIZE );
	gridY = (int)( ( (-worldY+1)/2 ) * GRID_SIZE );
}

void transformGridToWorldCoordinates(int gridX, int gridY, float &worldX, float &worldY){

	float cornerLength = (float)(1.0/(GRID_SIZE/2)); // TODO optimize

	worldX = ( ( (float)gridX / GRID_SIZE) * 2 ) - 1 + (cornerLength/2);
	worldY = ( ( (float)gridY / GRID_SIZE) * -2 ) + 1 - (cornerLength/2);
}

void colorGridCell(int x, int y) {

	float cornerLength = (float)(1.0/(GRID_SIZE/2)); // TODO optimize
	float topLeftX = (cornerLength * x) - 1;
	float topLeftY = ((cornerLength * y) - 1) * -1;

	glColor3f(0, 0.5, 0.5);

	glBegin(GL_QUADS);
		glVertex2f(topLeftX, topLeftY);
		glVertex2f(topLeftX + cornerLength, topLeftY);

		glVertex2f(topLeftX + cornerLength, topLeftY - cornerLength);
		glVertex2f(topLeftX, topLeftY - cornerLength);
	glEnd();

}

void drawBresenhamLine(int gridBeginX, int gridBeginY, int gridEndX,
		int gridEndY, bool useInts, bool draw) {

	bool steep = abs(gridEndY - gridBeginY) > abs(gridEndX - gridBeginX);
	if (steep) {
		swap(gridBeginX, gridBeginY);
		swap(gridEndX, gridEndY);
	}

	if (gridBeginX > gridEndX) {
		swap(gridBeginX, gridEndX);
		swap(gridBeginY, gridEndY);
	}

	if (useInts) {
		int deltaX = gridEndX - gridBeginX;
		int deltay = abs(gridEndY - gridBeginY);
		int error = deltaX / 2;
		int currentY = gridBeginY;

		int yStep = 1;
		if (gridBeginY >= gridEndY) {
			yStep = -1;
		}

		for (int currentX = gridBeginX; currentX <= gridEndX; currentX++) {

			if (draw) {
				if (steep) {
					colorGridCell(currentY, currentX);
				} else {
					colorGridCell(currentX, currentY);
				}
			}

			error = error - deltay;
			if (error < 0) {
				currentY = currentY + yStep;
				error = error + deltaX;
			}
		}
	} else {
		int deltaX = gridEndX - gridBeginX;
		int deltay = abs(gridEndY - gridBeginY);
		float error = 0;
		float deltaError = fabs((float) deltay / (float) deltaX);
		int currentY = gridBeginY;

		int yStep = 1;
		if (gridBeginY >= gridEndY) {
			yStep = -1;
		}

		for (int currentX = gridBeginX; currentX <= gridEndX; currentX++) {
			if (draw){
				if (steep) {
					colorGridCell(currentY, currentX);
				} else {
					colorGridCell(currentX, currentY);
				}
			}


			error = error + deltaError;
			if (error >= 0.5) {
				currentY = currentY + yStep;
				error = error - 1.0;
			}
		}
	}
}

void mouseClickCallback (int button, int state, int x, int y){

	if (button == GLUT_LEFT_BUTTON){

		if ((state == GLUT_DOWN)){
			transformScreenToWorldCoordinates(x, y, beginX, beginY);
			endX = beginX;
			endY = beginY;
			shouldDrawUserLine = true;

			transformWorldToGridCoordinates(beginX, beginY, gridBeginX, gridBeginY);
			gridEndX = gridBeginX;
			gridEndY = gridBeginY;
			shouldDrawBresenhamLine = true;
		}
//		else if (state == GLUT_UP) {
//			transformWorldToGridCoordinates(beginX, beginY, gridBeginX, gridBeginY);
//			transformWorldToGridCoordinates(endX, endY, gridEndX, gridEndY);
//
//		}
		glutPostRedisplay();
	}
}

void mouseDragCallback(int x, int y){

	transformScreenToWorldCoordinates(x, y, endX, endY);
	transformWorldToGridCoordinates(endX, endY, gridEndX, gridEndY);

	glutPostRedisplay();
}

void drawUserLine(){

	float lineBeginX = 0;
	float lineBeginY = 0;
	float lineEndX = 0;
	float lineEndY = 0;

	if (snapToGrid) {
		transformGridToWorldCoordinates(gridBeginX, gridBeginY, lineBeginX, lineBeginY);
		transformGridToWorldCoordinates(gridEndX, gridEndY, lineEndX, lineEndY);
	} else {
		lineBeginX = beginX;
		lineBeginY = beginY;
		lineEndX = endX;
		lineEndY = endY;
	}

	glColor3f(1.0, 0.0, 0.0);

	glBegin(GL_LINES);
		glVertex2f(lineBeginX, lineBeginY);
		glVertex2f(lineEndX, lineEndY);
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

	if (shouldDrawBresenhamLine){
		drawBresenhamLine(gridBeginX, gridBeginY, gridEndX, gridEndY, false, true);
	}

	if (shouldDrawUserLine) {
		drawUserLine();
	}

	glFlush();
}

int getRandomIntInRange(int min, int max) {
	return min + (rand() % (int)(max - min + 1)); // TODO review algorithm
	// TODO make random seed always the same
}

long getLineDrawingExecutionSpeedMicroseconds(bool useInts, long lineDraws){

	int min = 0;
	int max = GRID_SIZE - 1;

	int randBeginX;
	int randBeginY;
	int randEndX;
	int randEndY;

	srand(RAND_SEED);

	struct timeval start, end;

	gettimeofday(&start, NULL);

	for (long i = 0; i < lineDraws; i++) {
		randBeginX = getRandomIntInRange(min, max);
		randBeginY = getRandomIntInRange(min, max);
		randEndX = getRandomIntInRange(min, max);
		randEndY = getRandomIntInRange(min, max);

		drawBresenhamLine(randBeginX, randBeginY, randEndX, randEndY, useInts, false);

		//		cout << randBeginX << " " << randBeginX << " to " << randEndX << " " << randEndY << endl;
	}

	gettimeofday(&end, NULL);

	return end.tv_usec - start.tv_usec;
}

void testBresenhamLineAlgorithmExecutionSpeed(long numberOfCalls){

	long execTimeUsingInt   = getLineDrawingExecutionSpeedMicroseconds(true, numberOfCalls);
	long execTimeUsingFloat = getLineDrawingExecutionSpeedMicroseconds(false, numberOfCalls);

	cout << execTimeUsingInt << " " << execTimeUsingFloat << endl;
}

void init(void) {
}

int main(int argc, char *argv[]) {

//	testBresenhamLineAlgorithmExecutionSpeed(10000);
//	return 0;

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
