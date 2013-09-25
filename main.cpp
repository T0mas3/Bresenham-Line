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

#define GRID_SIZE 32 // TODO fix grid when odd number of cells
#define KEY_SPACE 32
#define RAND_SEED 1

const float HALF_GRID_SIZE = GRID_SIZE / 2;
const float CORNER_LENGTH = 1.0 / (float)HALF_GRID_SIZE;

int windowSizeH = 600;
int windowSizeV = 600;

bool shouldDrawUserLine = false;
bool shouldDrawBresenhamLine = false;
bool snapToGrid = true;
bool showHelp = false;

float mouseBeginX = 0;
float mouseBeginY = 0;
float mouseEndX = 0;
float mouseEndY = 0;

// TODO better titles
int gridBeginX = 0;
int gridBeginY = 0;
int gridEndX = 0;
int gridEndY = 0;

float userLineBeginX = 0;
float userLineBeginY = 0;
float userLineEndX = 0;
float userLineEndY = 0;

void transformScreenToWorldCoordinates(int screenX, int screenY, float &worldX, float &worldY) {

	worldX = ((float)screenX / (float)windowSizeH) * 2 - 1;
	worldY = ((float)screenY / (float)windowSizeV) * -2 + 1;
}

void transformWorldToGridCoordinates(float worldX, float worldY, int &gridX, int &gridY){
	gridX = (int)( ( ( worldX+1)/2 ) * GRID_SIZE );
	gridY = (int)( ( (-worldY+1)/2 ) * GRID_SIZE );
}

void transformGridToWorldCoordinates(int gridX, int gridY, float &worldX, float &worldY, bool cellCenter){

	worldX = ( ( (float)gridX / GRID_SIZE) * 2 ) - 1;
	worldY = ( ( (float)gridY / GRID_SIZE) * -2 ) + 1;

	if (cellCenter){
		worldX += (CORNER_LENGTH / 2);
		worldY -= (CORNER_LENGTH / 2);
	}

}

void colorGridCell(int x, int y) {

	float topLeftX;
	float topLeftY;

	transformGridToWorldCoordinates(x, y, topLeftX, topLeftY, false);

	glColor3f(0, 0.5, 0.5);

	glBegin(GL_QUADS);
		glVertex2f(topLeftX, topLeftY);
		glVertex2f(topLeftX + CORNER_LENGTH, topLeftY);

		glVertex2f(topLeftX + CORNER_LENGTH, topLeftY - CORNER_LENGTH);
		glVertex2f(topLeftX, topLeftY - CORNER_LENGTH);
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
			transformScreenToWorldCoordinates(x, y, mouseBeginX, mouseBeginY);
			mouseEndX = mouseBeginX;
			mouseEndY = mouseBeginY;
			shouldDrawUserLine = true;

			transformWorldToGridCoordinates(mouseBeginX, mouseBeginY, gridBeginX, gridBeginY);
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

	transformScreenToWorldCoordinates(x, y, mouseEndX, mouseEndY);
	transformWorldToGridCoordinates(mouseEndX, mouseEndY, gridEndX, gridEndY);

	glutPostRedisplay();
}

void keyboardCallback(unsigned char key, int x, int y){

	switch (key) {

	case KEY_SPACE:

		showHelp = !showHelp;
		glutPostRedisplay();
		break;
	}

}

void drawUserLine(){

	if (snapToGrid) {
		transformGridToWorldCoordinates(gridBeginX, gridBeginY, userLineBeginX, userLineBeginY, true);
		transformGridToWorldCoordinates(gridEndX, gridEndY, userLineEndX, userLineEndY, true);
	} else {
		userLineBeginX = mouseBeginX;
		userLineBeginY = mouseBeginY;
		userLineEndX = mouseEndX;
		userLineEndY = mouseEndY;
	}

	glColor3f(1.0, 0.0, 0.0);

	glBegin(GL_LINES);
		glVertex2f(userLineBeginX, userLineBeginY);
		glVertex2f(userLineEndX, userLineEndY);
	glEnd();
}

void drawGrid(){

	glColor3f(0.5, 0.5, 0.5);

	glBegin(GL_LINES);
	for (int i = -HALF_GRID_SIZE; i <= HALF_GRID_SIZE; i++) {
		float xAndY = i / (float)(HALF_GRID_SIZE);
		glVertex2f(xAndY, -1.0);
		glVertex2f(xAndY, 1.0);

		glVertex2f(-1.0, xAndY);
		glVertex2f(1.0, xAndY);
	}
	glEnd();
}

//TODO do not forget to reset colors

void drawHelpOverlay(){

	glColor3f(0.1, 0.1, 0.1);

	glBegin(GL_QUADS);
		glVertex2f(-1.0, 1.0);
		glVertex2f(1.0, 1.0);

		glVertex2f(1.0, 0);
		glVertex2f(-1.0, 0);
	glEnd();


	glColor3f(1.0, 1.0, 1.0);
	glRasterPos2f(-0.9, 0.9);
}

void display(void) {

	glClear(GL_COLOR_BUFFER_BIT);

	if (shouldDrawBresenhamLine){
		drawBresenhamLine(gridBeginX, gridBeginY, gridEndX, gridEndY, false, true);
	}

	drawGrid();

	if (shouldDrawUserLine) {
		drawUserLine();
	}

	if (showHelp) {
		drawHelpOverlay();
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
	glutCreateWindow("Bresenham's line algorithm demo - press \"ENTER\" for help");
	init();
	glutDisplayFunc(display);

	glutMouseFunc(mouseClickCallback);
	glutMotionFunc(mouseDragCallback);
	glutKeyboardFunc(keyboardCallback);

	glutMainLoop();
	return 0;
}
