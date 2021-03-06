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
#include <math.h> // for fabs
#include <sys/time.h>
#include <unistd.h>
using namespace std;

#define GRID_SIZE 32
#define USE_OPTIMIZED_LINE_ALG true
#define KEY_SPACE 32
#define RAND_SEED 1

#define RED 1
#define GREEN 2
#define BLUE 3
#define ORANGE 4

const float HALF_GRID_SIZE = GRID_SIZE / 2;
const float CORNER_LENGTH = 1.0 / (float)HALF_GRID_SIZE;

struct pointF2 {
  float x;
  float y;
};

struct pointInt2 {
  int x;
  int y;
};

struct lineF2 {
	pointF2 begin, end;
};

struct lineInt2 {
	pointInt2 begin, end;
};

int windowSizeH = 600;
int windowSizeV = 600;

bool shouldDrawUserLine = false;
bool shouldDrawBresenhamLine = false;
bool snapToGrid = true;
bool showHelp = false;

lineF2 mouseLine;
lineF2 userLine;
lineInt2 approxLine;


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

void drawBresenhamLine(int lineBeginX, int lineBeginY, int lineEndX,
		int lineEndY, bool useInts, bool draw) {

	bool isSteep = abs(lineEndY - lineBeginY) > abs(lineEndX - lineBeginX);
	if (isSteep) {
		swap(lineBeginX, lineBeginY);
		swap(lineEndX, lineEndY);
	}

	if (lineBeginX > lineEndX) {
		swap(lineBeginX, lineEndX);
		swap(lineBeginY, lineEndY);
	}

	int deltaX = lineEndX - lineBeginX;
	int deltay = abs(lineEndY - lineBeginY);
	int currentY = lineBeginY;

	if (useInts) {
		int error = deltaX / 2;

		int yStep = 1;
		if (lineBeginY >= lineEndY) {
			yStep = -1;
		}

		for (int currentX = lineBeginX; currentX <= lineEndX; currentX++) {

			if (draw) {
				if (isSteep) {
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
		float error = 0;
		float deltaError = fabs((float) deltay / (float) deltaX);

		int yStep = 1;
		if (lineBeginY >= lineEndY) {
			yStep = -1;
		}

		for (int currentX = lineBeginX; currentX <= lineEndX; currentX++) {
			if (draw) {
				if (isSteep) {
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
			transformScreenToWorldCoordinates(x, y, mouseLine.begin.x, mouseLine.begin.y);
			mouseLine.end.x = mouseLine.begin.x;
			mouseLine.end.y = mouseLine.begin.y;
			shouldDrawUserLine = true;

			transformWorldToGridCoordinates(mouseLine.begin.x, mouseLine.begin.y, approxLine.begin.x, approxLine.begin.y);
			approxLine.end.x = approxLine.begin.x;
			approxLine.end.y = approxLine.begin.y;
			shouldDrawBresenhamLine = true;
		}
		glutPostRedisplay();
	} else if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_UP)){
		showHelp = !showHelp;
		glutPostRedisplay();
	}
}

void mouseDragCallback(int x, int y){

	transformScreenToWorldCoordinates(x, y, mouseLine.end.x, mouseLine.end.y);
	transformWorldToGridCoordinates(mouseLine.end.x, mouseLine.end.y, approxLine.end.x, approxLine.end.y);

	glutPostRedisplay();
}

void drawUserLine(){

	if (snapToGrid) {
		transformGridToWorldCoordinates(approxLine.begin.x, approxLine.begin.y, userLine.begin.x, userLine.begin.y, true);
		transformGridToWorldCoordinates(approxLine.end.x, approxLine.end.y, userLine.end.x, userLine.end.y, true);
	} else {
		userLine.begin.x = mouseLine.begin.x;
		userLine.begin.y = mouseLine.begin.y;
		userLine.end.x = mouseLine.end.x;
		userLine.end.y = mouseLine.end.y;
	}

	glColor3f(1.0, 0.0, 0.0);

	glBegin(GL_LINES);
		glVertex2f(userLine.begin.x, userLine.begin.y);
		glVertex2f(userLine.end.x, userLine.end.y);
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

void renderBitmapString(float x, float y, void *font, char *string) {

  char *c;
  glRasterPos2f(x, y);
  for (c=string; *c != '\0'; c++) {
    glutBitmapCharacter(font, *c);
  }
}

void drawHelpOverlay(){

	glColor3f(0.1, 0.1, 0.1);

	glBegin(GL_QUADS);
		glVertex2f(-1.0, 1.0);
		glVertex2f(1.0, 1.0);

		glVertex2f(1.0, 0.7);
		glVertex2f(-1.0, 0.7);
	glEnd();

	glColor3f(1.0, 1.0, 1.0);

	char *helpString = "Hold and drag left mouse button to draw a line";
	renderBitmapString(-0.9, 0.9, GLUT_BITMAP_TIMES_ROMAN_24, helpString);
	helpString = "Right click to close this help window";
	renderBitmapString(-0.9, 0.8, GLUT_BITMAP_TIMES_ROMAN_24, helpString);
}

void display(void) {

	glClear(GL_COLOR_BUFFER_BIT);

	if (shouldDrawBresenhamLine){
		drawBresenhamLine(approxLine.begin.x, approxLine.begin.y, approxLine.end.x, approxLine.end.y, USE_OPTIMIZED_LINE_ALG, true);
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
	return min + (rand() % (int)(max - min + 1));
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
	}

	gettimeofday(&end, NULL);

	return (end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec);
}

void testBresenhamLineAlgorithmExecutionSpeed(long numberOfCalls){

	long execTimeUsingInt   = getLineDrawingExecutionSpeedMicroseconds(true, numberOfCalls);
	long execTimeUsingFloat = getLineDrawingExecutionSpeedMicroseconds(false, numberOfCalls);

	cout << "Execution time using integers: " << execTimeUsingInt   << " microseconds" << endl;
	cout << "Execution time using floats:   " << execTimeUsingFloat << " microseconds" << endl;
	cout << "Difference (float version - integer version): " << execTimeUsingFloat - execTimeUsingInt << " microseconds" << endl;
}

void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if(h == 0)
		h = 1;


//	if (h > w) {
//		windowSizeH = h;
//		windowSizeV = h;
//	} else {
//		windowSizeH = w;
//		windowSizeV = w;
//	}

			windowSizeH = h;
			windowSizeV = w;

//	glutReshapeWindow (windowSizeH, windowSizeV);


	// Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);

        // Reset Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Get Back to the Modelview
	glMatrixMode(GL_MODELVIEW);
}

void init(void) {
}

int main(int argc, char *argv[]) {

	if (argc == 2) {

		int numberOfCalls = atoi(argv[1]);

		if (numberOfCalls < 1){
			cout << "Error: Invalid parameter. Number of calls must be positive, non-zero integer." << endl;
		} else {
			cout << "Starting test. Calculating lines using Bresenham's algorithm for " << numberOfCalls << " times" << endl;
			testBresenhamLineAlgorithmExecutionSpeed(numberOfCalls);
			cout << "Test finished" << endl;
		}

	} else if (argc == 1) {
		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
		glutInitWindowSize(windowSizeH, windowSizeV);
		glutInitWindowPosition(100, 50);
		glutCreateWindow("Bresenham's line algorithm: left-click mouse drag - draws line, right-click - show help");
		init();
		glutDisplayFunc(display);
		glutMouseFunc(mouseClickCallback);
		glutMotionFunc(mouseDragCallback);

		glutReshapeFunc(changeSize);
		glutMainLoop();
	} else {
		cout << "Error: Wrong argument count. Usage: No parameters - open line drawing window,"<< endl
				<< "1 parameter - perform benchmark by approximating entered number of random lines";
	}

	return 0;
}
